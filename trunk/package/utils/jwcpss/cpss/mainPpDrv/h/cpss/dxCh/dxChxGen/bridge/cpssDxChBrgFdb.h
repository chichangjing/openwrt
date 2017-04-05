/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgFdb.h
*
* DESCRIPTION:
*       FDB tables facility CPSS DxCh implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 44 $
*
*******************************************************************************/
#ifndef __cpssDxChBrgFdbh
#define __cpssDxChBrgFdbh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>

/*
 * typedef: enum CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT
 *
 * Description: FDB Lookup Key mode
 *
 * Enumerations:
 *      CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_E      - use Single Tag Key
 *      CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_VID1_E - use Double Tag Key
 *
 */
typedef enum
{
    CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_E,
    CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_FID_VID1_E
} CPSS_DXCH_BRG_FDB_LOOKUP_KEY_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_FDB_QUEUE_TYPE_ENT
 *
 * Description: Enumeration of FDB message queues (AU/FU)
 *
 * Enumerations:
 *      CPSS_DXCH_FDB_QUEUE_TYPE_AU_E - type for AUQ (address update queue)
 *      CPSS_DXCH_FDB_QUEUE_TYPE_FU_E - type for FUQ (FDB upload queue)
 */
typedef enum
{
    CPSS_DXCH_FDB_QUEUE_TYPE_AU_E,
    CPSS_DXCH_FDB_QUEUE_TYPE_FU_E
} CPSS_DXCH_FDB_QUEUE_TYPE_ENT;


/*
 * typedef: enum CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT
 *
 * Description: Enumeration how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
 *       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
 *       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
 *       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
 *
 * Enumerations:
 *      CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ALL_ZERO_E - use 16 bits 0.
 *      CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_FID_E  - use 16 bits of FID.
 *      CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_MAC_E  - use 16 LSBits of MAC.
 */
typedef enum
{
    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ALL_ZERO_E,
    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_FID_E,
    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_USE_MAC_E
} CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT
 *
 * Description: Enumeration for trunk aging mode
 *
 * Enumerations:
 *      CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_REGULAR_E - Entries associated with Trunks
 *              are aged out according to the <ActionMode> configuration.
 *      CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_FORCE_AGE_WITHOUT_REMOVAL_E  -
 *              Entries associated with Trunks are aged-without-removal regardless
 *              of the dev# they are associated with and regardless of the configuration
 *              in the ActionMode configuration
 */
typedef enum
{
    CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_REGULAR_E,
    CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_FORCE_AGE_WITHOUT_REMOVAL_E
}CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_FDB_DEL_MODE_ENT
 *
 * Description: Enum for FDB Deletetion Modes
 *
 * Enumerations:
 *      CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E - Only dynamic addresses matching the deleting action are deleted
 *      CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E - Dynamic and static addresses matching the deleting action are deleted
 *      CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E - Only static addresses matching the deleting action are deleted.
 *                                                 APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3.
 */
typedef enum
{
    CPSS_DXCH_BRG_FDB_DEL_MODE_DYNAMIC_ONLY_E  = GT_FALSE,
    CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E = GT_TRUE,
    CPSS_DXCH_BRG_FDB_DEL_MODE_STATIC_ONLY_E
} CPSS_DXCH_BRG_FDB_DEL_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT
 *
 * Description: Enumeration of FDB Table size in entries
 *
 * Enumerations:
 *   CPSS_DXCH_BRG_FDB_TBL_SIZE_8K_E  - 8K FDB table entries.
 *   CPSS_DXCH_BRG_FDB_TBL_SIZE_16K_E - 16K FDB table entries.
 *   CPSS_DXCH_BRG_FDB_TBL_SIZE_32K_E - 32K FDB table entries.
 *   CPSS_DXCH_BRG_FDB_TBL_SIZE_64K_E - 64K FDB table entries.
 *   CPSS_DXCH_BRG_FDB_TBL_SIZE_128K_E - 128K FDB table entries.
 *   CPSS_DXCH_BRG_FDB_TBL_SIZE_256K_E - 256K FDB table entries.
 */

typedef enum
{
    CPSS_DXCH_BRG_FDB_TBL_SIZE_8K_E = 0,
    CPSS_DXCH_BRG_FDB_TBL_SIZE_16K_E,
    CPSS_DXCH_BRG_FDB_TBL_SIZE_32K_E,
    CPSS_DXCH_BRG_FDB_TBL_SIZE_64K_E,
    CPSS_DXCH_BRG_FDB_TBL_SIZE_128K_E,
    CPSS_DXCH_BRG_FDB_TBL_SIZE_256K_E
} CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT;

/*
 * Typedef: CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC
 *
 * Description: struct contains the parameters for FDB table hash calculation.
 *
 * Fields:
 *       vlanMode    - the VLAN lookup mode.
 *       size        - the entries number in the FDB table.
 *       fid16BitHashEn - indication that the FDB hash uses 16 bits of FID.
 *                      GT_TRUE - use 16 bits FID
 *                      GT_FALSE - use 12 bits FID
 *       crcHashUpperBitsMode - 16 MSbits mode for of DATA into the hash function
 *                      relevant to hashMode = CRC_MULTI_HASH
 *       useZeroInitValueForCrcHash  - Use zero for initialization value of CRC function.
 *                                     Relevant only for CPSS_MAC_HASH_FUNC_CRC_E.
 *                                     Must be set as GT_TRUE for Bobcat2, Caelum, Bobcat3 devices.
 *                                     Must be set as GT_FALSE for other devices.
 *
 */
typedef struct
{
    CPSS_MAC_VL_ENT                 vlanMode;
    CPSS_DXCH_BRG_FDB_TBL_SIZE_ENT  size;
    GT_BOOL                         fid16BitHashEn;
    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT crcHashUpperBitsMode;
    GT_BOOL                          useZeroInitValueForCrcHash;
}CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC;

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortSet
*
* DESCRIPTION:
*       Enable/disable forwarding a new mac address message to CPU --per port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number, CPU port
*       enable  - If GT_TRUE, forward NA message to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortSet
(
    IN GT_U8        devNum,
    IN GT_PORT_NUM  portNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortGet
*
* DESCRIPTION:
*       Get Enable/disable forwarding a new mac address message to CPU --
*       per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number, CPU port
*
* OUTPUTS:
*       enablePtr  - If GT_TRUE, NA message is forwarded to CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaToCpuPerPortGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);


/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusSet
*
* DESCRIPTION:
*       Enable/disable learning of new source MAC addresses for packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*       cmd    - how to forward packets with unknown/changed SA,
*                 if status is GT_FALSE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on unknown command
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Automatic learning is not recommended in this mode, due to the fact
*           that FDBs are supposed to be synced.
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN GT_BOOL                  status,
    IN CPSS_PORT_LOCK_CMD_ENT   cmd
);

/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusGet
*
* DESCRIPTION:
*       Get state of new source MAC addresses learning on packets received
*       on specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       statusPtr - (pointer to) GT_TRUE for enable  or GT_FALSE otherwise
*       cmdPtr    - (pointer to) how to forward packets with unknown/changed SA,
*                   when (*statusPtr) is GT_FALSE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortLearnStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT GT_BOOL                 *statusPtr,
    OUT CPSS_PORT_LOCK_CMD_ENT  *cmdPtr
);

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventSet
*
* DESCRIPTION:
*   Enable/Disable New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE - enable NA Storm Prevention (SP),
*                    GT_FALSE - disable NA Storm Prevention
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       Enabling this mechanism in any of the ports, creates a situation where the
*       different portGroups populate different MACs at the same indexes,
*       causing FDBs to become unsynchronized, this leads to the situation that once
*       sending MAC update to different portGroups from CPU, it may succeed on some
*       of the portGroups, while failing on the others (due to SP entries already
*       occupying indexes)... So the only way to bypass these problems, assuming SP
*       is a must, is to add new MACs by exact index - this mode of operation
*       overwrites any SP entry currently there.
*       Application that enables the SP in any of the ports should not use the NA
*       sending mechanism (cpssDxChBrgFdbMacEntrySet / cpssDxChBrgFdbPortGroupMacEntrySet APIs)
*       to add new MAC addresses, especially to multiple portGroups -
*       as this may fail on some of the portGroups.
*       Removing or updating existing (non-SP) entries may still use the NA messages.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventGet
*
* DESCRIPTION:
*   Get status of New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr     - pointer to the status of repeated NA CPU messages
*                       GT_TRUE - NA Storm Prevention (SP) is enabled,
*                       GT_FALSE - NA Storm Prevention is disabled
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/portNum/auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaStormPreventGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortVid1LearningEnableSet
*
* DESCRIPTION:
*   Enable/Disable copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortVid1LearningEnableSet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    IN  GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgFdbPortVid1LearningEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable status of copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortVid1LearningEnableGet
(
    IN  GT_U8            devNum,
    IN  GT_PORT_NUM      portNum,
    OUT GT_BOOL          *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       devTableBmp - bmp of devices to set (APPLICABLE RANGES: 0..31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  devTableBmp
);

/*******************************************************************************
* cpssDxChBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       devTableBmpPtr - pointer to bmp of devices to set (APPLICABLE RANGES: 0..31).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDeviceTableGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *devTableBmpPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
);

/*******************************************************************************
* cpssDxChBrgFdbQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
);


/*******************************************************************************
* cpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
);



/*******************************************************************************
* cpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr

);

/*******************************************************************************
* cpssDxChBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
);


/*******************************************************************************
* cpssDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
);


/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenSet
*
* DESCRIPTION:
*       Set the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       lookupLen   - The maximal length of MAC table lookup, this must be
*                     value divided by 4 with no left over.
*                     (APPLICABLE RANGES: 4, 8, 12..32).
*                     in 'multi hash' mode the valid value is : 16.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_OUT_OF_RANGE          - lookupLen > 32 or lookupLen < 4
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
 GT_STATUS cpssDxChBrgFdbMaxLookupLenSet
(
    IN GT_U8    devNum,
    IN GT_U32   lookupLen
);

/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenGet
*
* DESCRIPTION:
*       Get the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       lookupLenPtr   - The maximal length of MAC table lookup, this must be
*                        value divided by 4 with no left over.
*                        (APPLICABLE RANGES: 4, 8, 12..32).
*                        in 'multi hash' mode the valid value is : 16.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLookupLenGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *lookupLenPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeSet
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeSet
(
    IN GT_U8            devNum,
    IN CPSS_MAC_VL_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeGet
*
* DESCRIPTION:
*       Get VLAN Lookup mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad param
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacVlanLookupModeGet
(
    IN  GT_U8            devNum,
    OUT CPSS_MAC_VL_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitSet
*
* DESCRIPTION:
*     Set Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*       msgRate    - maximal rate of AU messages per second
*       enable     - enable/disable message limit
*                    GT_TRUE - message rate is limited with msgRate
*                    GT_FALSE - there is no limitations, WA to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*     for multi-port groups device :
*       The rate-limiting is done individually in each one of the port Groups.
*       This means that potentially configuring this value would result a rate
*       which is up to n times bigger then expected (n is number of port groups)
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitSet
(
    IN GT_U8                       devNum,
    IN GT_U32                      msgRate,
    IN GT_BOOL                     enable
);

/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitGet
*
* DESCRIPTION:
*     Get Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       msgRatePtr - (pointer to) maximal rate of AU messages per second
*       enablePtr  - (pointer to) GT_TRUE - message rate is limited with
*                                           msgRatePtr
*                                 GT_FALSE - there is no limitations, WA to CPU
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on error.
*       GT_OUT_OF_RANGE          - out of range.
*       GT_BAD_PARAM             - on bad parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when DxCh Clock 200 MHz(DxCh2 220 MHz Clock) ,
*          granularity and rate changed linearly depending on clock.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgRateLimitGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *msgRatePtr,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongSet
*
* DESCRIPTION:
*       Enable/Disable sending NA messages to the CPU indicating that the device
*       cannot learn a new SA . It has reached its max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending NA messages to the CPU
*       indicating that the device cannot learn a new SA. It has reached its
*       max hop (bucket is full).
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the NA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgOnChainTooLongGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuSet
*
* DESCRIPTION:
*       Enabled/Disabled sending Aged Address (AA) messages to the CPU for the
*       Storm Prevention (SP) entries when those entries reach an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the SP AA message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending Aged Address (AA) messages
*       to the CPU for the Storm Prevention (SP) entries when those entries reach
*       an aged out status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the SP AA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSpAaMsgToCpuGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending an AA and TA address
*       update messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           Whenever applicable it's highly recommended disabling these messages,
*           as enabling this would stress the AUQ with excessive messages
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuGet
*
* DESCRIPTION:
*       Get state of sending an AA and TA address update messages to the CPU
*       as configured to PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or auMsgType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAAandTAToCpuGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbHashModeSet
*
* DESCRIPTION:
*       Sets the FDB hash function mode.
*       The CRC based hash function provides the best hash index distribution
*       for random addresses and vlans.
*       The XOR based hash function provides optimal hash index distribution
*       for controlled testing scenarios, where sequential addresses and vlans
*       are often used.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - hash function based mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_HASH_FUNC_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgFdbHashModeGet
*
* DESCRIPTION:
*       Gets the FDB hash function mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - hash function based mode:
*                 CPSS_FDB_HASH_FUNC_XOR_E - XOR based hash function mode
*                 CPSS_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashModeGet
(
    IN  GT_U8                         devNum,
    OUT CPSS_MAC_HASH_FUNC_MODE_ENT   *modePtr
);

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For Bobcat2 the device support aging time with granularity of 1 second
*           in range 1.. max. where max depend on FDB size and 'core clock'.
*           (the max value is very high and at least several thousands seconds !)
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008) . So value of 226 will be round down to 224 , and value of
*       255 will be round up to 256.
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*       So value of 234 will be round down to 230 ,
*       and value of 255 will be round up to 260.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   For the 270MHz range from 16..1008 (but due to rounding we support values 8..636)
*   HW supports steps 16
*   For the 220MHz range from 9..573 (but due to rounding we support values 5..577)
*   HW supports steps 9 (9.09)
*   For the 200MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*   For the 144 MHz range from 14..875 (but due to rounding we support values 7..882)
*   HW supports steps 13.88
*   For the 167 MHz range from 14..886 (but due to rounding we support values 7..893)
*   HW supports steps 14 (14.07)
*   For the 360MHz range from 10..630 (but due to rounding we support values 5..634)
*   HW supports steps 10
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutSet
(
    IN GT_U8  devNum,
    IN GT_U32 timeout
);

/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutGet
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE :
*       For Bobcat2 the device support aging time with granularity of 1 second
*           in range 1.. max. where max depend on FDB size and 'core clock'.
*           (the max value is very high and at least several thousands seconds !)
*       For DxCh3 the device support aging time with granularity of 16 seconds
*       (16,32..1008).
*       For others the device support aging time with
*       granularity of 10 seconds (10,20..630) .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       timeoutPtr - pointer to aging time in seconds.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For core clock of 270MHz The value ranges from 16 seconds to 1008
*       seconds in steps of 16 seconds.
*       For core clock of 220MHz The value ranges from 9 seconds to 573
*       seconds in steps of 9 seconds.
*       For core clock of 200 MHZ The value ranges from 10 seconds to 630
*       seconds in steps of 10 seconds.
*       For core clock of 144MHZ the aging timeout ranges from 14 (13.8) to 875
*       seconds in steps of 14 seconds.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgingTimeoutGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *timeoutPtr
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingSet
*
* DESCRIPTION:
*      Enable or disable secure aging. This mode is relevant to automatic or
*      triggered aging with removal of aged out FDB entries. This mode is
*      applicable together with secure auto learning mode see
*      cpssDxChBrgFdbSecureAutoLearnSet. In the secure aging mode the device
*      sets aged out unicast FDB entry with <Multiple> =  1 and VIDX = 0xfff but
*      not invalidates entry. This causes packets destined to this FDB entry MAC
*      address to be flooded to the VLAN.  But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable secure aging
*                 GT_FALSE - disable secure aging
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi-port groups device :
*       1. Unified FDBs mode and Unified-Linked FDBs mode :
*           Since Automatic learning is not recommended in this mode nor secured automatic
*           learning .
*       2. Linked FDBs mode - no limitations
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingSet
(
    IN GT_U8          devNum,
    IN GT_BOOL        enable
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAgingGet
*
* DESCRIPTION:
*      Get the status (enabled/disabled) of secure aging.
*      This mode is relevant to automatic or triggered aging with removal of
*      aged out FDB entries. This mode is applicable together with secure auto
*      learning mode see cpssDxChBrgFdbSecureAutoLearnGet. In the secure aging
*      mode  the device sets aged out unicast FDB entry with <Multiple> =  1 and
*      VIDX = 0xfff but not invalidates entry. This causes packets destined to
*      this FDB entry MAC address to be flooded to the VLAN.
*      But such packets are known ones.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable secure aging
*                    GT_FALSE - disable secure aging
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAgingGet
(
    IN  GT_U8          devNum,
    OUT GT_BOOL        *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnSet
*
* DESCRIPTION:
*        Configure secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enable/Disable secure automatic learning.
*                 The command assigned to packet with new SA not found in FDB
*                 when secure automatic learning enable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnSet
(
    IN GT_U8                                devNum,
    IN CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnGet
*
* DESCRIPTION:
*        Get secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr  - Enabled/Disabled secure automatic learning.
*                  The command assigned to packet with new SA not found in FDB
*                  when secure automatic learning enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecureAutoLearnGet
(
    IN  GT_U8                                devNum,
    OUT CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnable
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*       When the PP will do the transplanting , it will/won't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it will/won't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE transplanting is enabled on static entries,
*                GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnableGet
*
* DESCRIPTION:
*       Get whether the transplanting enabled to operate on static entries.
*       when the PP do the transplanting , it do/don't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it do/don't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE transplanting is enabled on static entries,
*                   GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticTransEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnable
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*       When the PP will do the Flush , it will/won't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB deletion mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnable
(
    IN GT_U8                           devNum,
    IN CPSS_DXCH_BRG_FDB_DEL_MODE_ENT  mode
);

/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnableGet
*
* DESCRIPTION:
*       Get whether flush delete operates on static entries.
*       When the PP do the Flush , it do/don't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - FDB deletion mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticDelEnableGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_BRG_FDB_DEL_MODE_ENT  *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE -  Actions are enabled
*                 GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbActionsEnableGet
*
* DESCRIPTION:
*       Get the status of FDB actions.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - actions are enabled
*                    GT_FALSE - actions are disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionsEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       actFinishedPtr  - GT_TRUE  - triggered action completed
*                         GT_FALSE - triggered action is not completed yet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'action finished' only when action finished on all port groups.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *actFinishedPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ACTION_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeGet
*
* DESCRIPTION:
*       Gets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - pointer to action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_MAC_ACTION_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemove
*
* DESCRIPTION:
*       enable the ability to delete static mac entries when the aging daemon in
*       PP encounter with entry registered on non-exist device in the
*       "device table" the PP removes it ---> this flag regard the static
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*       deleteStatic -  GT_TRUE - Action will delete static entries as well as
*                                 dynamic
*                       GT_FALSE - only dynamic entries will be deleted.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       It's application responsibility to check the status of Aging Trigger by
*       busy wait (use cpssDxChBrgFdbTrigActionStatusGet API), in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemove
(
    IN GT_U8                       devNum,
    IN GT_BOOL                     deleteStatic
);

/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
*
* DESCRIPTION:
*       Get whether enabled/disabled the ability to delete static mac entries
*       when the aging daemon in PP encounter with entry registered on non-exist
*       device in the "device table".
*       entries
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       deleteStaticPtr -  GT_TRUE - Action deletes entries if there associated
*                                    device number don't exist (static etries
*                                                                  as well)
*                          GT_FALSE - Action don't delete entries if there
*                                     associated device number don't exist
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on failure
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *deleteStaticPtr
);

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableSet
*
* DESCRIPTION:
*       Enable/Disable dropping the Address Update messages when the queue is
*       full .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - Drop the address update messages
*                 GT_FALSE - Do not drop the address update messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of dropping the Address Update messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - The address update messages are dropped
*                    GT_FALSE - The address update messages are not dropped
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDropAuEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with trunk,
*       regardless of the device to which they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*       ageOutAllDevOnTrunkEnable   - GT_TRUE : All entries associated with a
*                                     trunk, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     trunk on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnTrunkEnable
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with trunk, regardless of
*       the device to which they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                      trunk, regardless of the device they
*                                      are associated with are aged out.
*                                      GT_FALSE: Only entries associated with a
*                                      trunk on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnTrunkEnablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with port,
*       regardless of the device to which they are associated.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                       - device number
*       ageOutAllDevOnNonTrunkEnable - GT_TRUE : All entries associated with a
*                                     port, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     port on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
(
    IN GT_U8    devNum,
    IN GT_BOOL  ageOutAllDevOnNonTrunkEnable
);

/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with port, regardless of
*       the device to which they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnNonTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                         port, regardless of the device they
*                                         are associated with are aged out.
*                                         GT_FALSE: Only entries associated with a
*                                         port on the local device are aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *ageOutAllDevOnNonTrunkEnablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataSet
*
* DESCRIPTION:
*       Prepares the entry for transplanting (old and new interface parameters).
*       VLAN and VLAN mask for transplanting is set by
*       cpssDxChBrgFdbActionActiveVlanSet. Execution of transplanting is done
*       by cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       oldInterfacePtr - pointer to old Interface parameters
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters
*                         (it may be only port or trunk).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataSet
(
    IN GT_U8                    devNum,
    IN CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    IN CPSS_INTERFACE_INFO_STC  *newInterfacePtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataGet
*
* DESCRIPTION:
*       Get transplant data: old interface parameters and the new ones.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
*
* OUTPUTS:
*       oldInterfacePtr - pointer to old Interface parameters.
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters.
*                         (it may be only port or trunk).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionTransplantDataGet
(
    IN  GT_U8                    devNum,
    OUT CPSS_INTERFACE_INFO_STC  *oldInterfacePtr,
    OUT CPSS_INTERFACE_INFO_STC  *newInterfacePtr
);

/*******************************************************************************
* cpssDxChBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       entryOffsetPtr - (pointer to) to receive the offset from the hash calculated index
*                     NULL value means that caller not need to retrieve this value.
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was processed by PP.
*                      GT_FALSE - AU message processing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*       entryOffsetPtr - (pointer to) the offset from the hash calculated index
*                      of the entry that action performed on.
*                      (similar to parameter in an AU message CPSS_MAC_UPDATE_MSG_EXT_STC::entryOffset)
*                      The parameter ignored when NULL pointer.
*                      (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*                      the offset is relevant only when:
*                      1. 'completed = GT_TRUE and succeeded = GT_TRUE'
*                      2. The function that triggered the action is one of :
*                        a. cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbPortGroupMacEntryDelete
*                         the offset relate to the delete entry.
*                        b. cpssDxChBrgFdbMacEntrySet, cpssDxChBrgFdbPortGroupMacEntrySet functions.
*                         the offset relate to the inserted/updated entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *completedPtr,
    OUT GT_BOOL  *succeededPtr,
    INOUT GT_U32 *entryOffsetPtr
);


/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanSet
*
* DESCRIPTION:
*       Set action active vlan and vlan mask.
*       All actions will be taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       vlanId      - Vlan Id
*       vlanMask    - vlan mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanSet
(
    IN GT_U8    devNum,
    IN GT_U16   vlanId,
    IN GT_U16   vlanMask
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanGet
*
* DESCRIPTION:
*       Get action active vlan and vlan mask.
*       All actions are taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       vlanIdPtr      - pointer to Vlan Id
*       vlanMaskPtr    - pointer to vlan mask filter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveVlanGet
(
    IN  GT_U8    devNum,
    OUT GT_U16   *vlanIdPtr,
    OUT GT_U16   *vlanMaskPtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevSet
*
* DESCRIPTION:
*       Set the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       actDev      - Action active device (APPLICABLE RANGES: 0..31)
*       actDevMask  - Action active mask (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*                                  in 'Dual deviceId system' , if the Pattern/mask
*                                  of the LSBit of the actDev,actDevMask are 'exact match = 1'
*                                  because all devices in this system with 'even numbers'
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*       For 'Dual deviceId system':
*           the LSBit on the actDev,acdDevMask are ignored and actually taken from
*           MSBit of actTrunkPort and actTrunkPortMask from cpssDxChBrgFdbActionActiveInterfaceSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevSet
(
    IN GT_U8    devNum,
    IN GT_U32   actDev,
    IN GT_U32   actDevMask
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevGet
*
* DESCRIPTION:
*       Get the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actDevPtr      - pointer to action active device (APPLICABLE RANGES: 0..31)
*       actDevMaskPtr  - pointer to action active mask (APPLICABLE RANGES: 0..31)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actDev > 31 or actDevMask > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveDevGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actDevPtr,
    OUT GT_U32   *actDevMaskPtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceSet
*
* DESCRIPTION:
*       Set the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of : Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*       A FDB entry will be treated by the FDB action only if the following
*       cases are true:
*       1. The interface is trunk and "active trunk mask" is set and
*       "associated trunkId" masked by the "active trunk/port mask" equal to
*       "Active trunkId".
*       2. The interface is port and "associated portNumber" masked by the
*       "active trunk/port mask" equal to "Active portNumber".
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*       actIsTrunk       - determines if the interface is port or trunk
*       actIsTrunkMask   - action active trunk mask.
*       actTrunkPort     - action active interface (port/trunk)
*       actTrunkPortMask - action active port/trunk mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable Trunk-ID filter set actIsTrunk and actIsTrunkMask to 0x1,
*       set actTrunkPort to trunkID and actTrunkPortMask to 0x7F. To disable
*       Trunk-ID filter set all those parameters to 0x0.
*       To enable Port/device filter set actIsTrunk to 0x0, actTrunkPort to
*       portNum, actTrunkPortMask to 0x7F, actDev to device number and
*       actDevMask to 0x1F (all ones) by cpssDxChBrgFdbActionActiveDevSet.
*       To disable port/device filter set all those parameters to 0x0.
*
*       There is no check done of current status of action's register
*
************************************************
*       For 'Dual deviceId system':
*           the MSBit of the actTrunkPort and actTrunkPortMask are actually used as
*           LSBit of actDev and actDevMask of cpssDxChBrgFdbActionActiveDevSet
*           - configuration limitation :
*               not support next configuration :
*               ((actIsTrunkMask == 0 ) && //care don't if trunk/port
*                (actTrunkPortMask[MSBit] == 1)) // exact match of MSBit of trunk/port
*               reason for the limitation : trunk can come from 2 hemispheres
*
*           more detailed behavior:
*            A. If <isTrunk> is exact match = ‘port’
*                Then Cpss do :
*                < hwDevNum >[0] .Pattern = <port/trunk>[6].Pattern
*                < hwDevNum >[0] .Mask = <port/trunk>[6].Mask
*                <port/trunk>[6]. Pattern = 0
*                <port/trunk>[6]. Mask = no change
*            B. Else If <isTrunk> is exact match = ‘trunk’
*                Then Cpss do :
*                < hwDevNum >[0] .Mask = ‘don’t care’
*           C. Else // <isTrunk> is ‘don’t care’
*               1) if <port/trunk>[6] is ‘don’t care’
*                   Then Cpss do :
*                   < hwDevNum >[0] .Mask = ‘don’t care’
*               2) else //exact match – 0 or 1
*                   return bad state //because trunk can come from 2 hemispheres
************************************************
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceSet
(
    IN GT_U8   devNum,
    IN GT_U32  actIsTrunk,
    IN GT_U32  actIsTrunkMask,
    IN GT_U32  actTrunkPort,
    IN GT_U32  actTrunkPortMask
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceGet
*
* DESCRIPTION:
*       Get the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of: Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       actIsTrunkPtr       - determines if the interface is port or trunk
*       actIsTrunkMaskPtr   - action active trunk mask.
*       actTrunkPortPtr     - action active interface (port/trunk)
*       actTrunkPortMaskPtr - action active port/trunk mask.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveInterfaceGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *actIsTrunkPtr,
    OUT GT_U32  *actIsTrunkMaskPtr,
    OUT GT_U32  *actTrunkPortPtr,
    OUT GT_U32  *actTrunkPortMaskPtr
);

/*******************************************************************************
* cpssDxChBrgFdbIpmcAddrDelEnableSet
*
* DESCRIPTION:
*       Function enable/disable the address aging/deleting IPMC FDB entries.
*       when disabled : the IPMC FDB are not subject to aging/delete operations
*                       by the aging daemon.
*       when enabled : the IPMC FDB are subject to aging/delete operations
*                       by the aging daemon.
*                       aging is still subject to only 'non-static' entries.
*
*       NOTE: no other filter (vlan/dev...) is relevant for those entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable/disable the address aging/deleting IPMC FDB entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbIpmcAddrDelEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbIpmcAddrDelEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled of the address aging/deleting IPMC FDB entries.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable the address aging/deleting IPMC FDB entries.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbIpmcAddrDelEnableGet
(
    IN GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableSet
*
* DESCRIPTION:
*       Enable/Disable reading FDB entries via AU messages to the CPU.
*       The API only configures mode of triggered action.
*       To execute upload use the cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - enable FDB Upload
*                  GT_FALSE - disable FDB upload
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgFdbUploadEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of uploading FDB entries via AU messages to the CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - FDB Upload is enabled
*                     GT_FALSE - FDB upload is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUploadEnableGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE    - when previous FDB triggered action is not completed yet
*                         or CNC block upload not finished (or not all of it's
*                         results retrieved from the common used FU and CNC
*                         upload queue)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and trigger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start triggered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of status - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*   for multi-port groups device :
*           the trigger is done on ALL port groups regardless to operation mode
*           (Trigger aging/transplant/delete/upload).
*           see also description of function cpssDxChBrgFdbActionModeSet about
*           'multi-port groups device'.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrigActionStart
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the setting not allowed in current state of
*                                  configuration.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*       set configuration to all portGroups with same value.
*       1. Age with delete (may be Triggered or Auto)
*           a. Unified FDBs mode and Unified-Linked FDB mode :
*              This type of operation is not supported , due to the control
*              learning being used, the risk of AA messages being dropped & the
*              need to avoid dropping MAC entries from portGroups which are not
*              the source of these entries (meaning they would never be refreshed)
*              API returns GT_BAD_STATE if trying to use this action mode.
*           b. Linked FDBs mode :
*              Application should use this type of operation only in Automatic mode
*              (not in Controlled mode) + Static FDB entries .
*              In this mode automatic learnt addresses would be silently removed
*              by PP while application addresses would be removed only by specific
*              application API call.
*
*       2. Age without delete (may be Triggered or Auto)
*           The only type of Aging that can be used with Unified FDBs mode and
*           Unified-Linked FDB mode (in Linked FDBs mode it should be used when
*           using controlled mode), AA would be sent by PP to Application that
*           would remove entry from relevant portGroups.
*
*       3. Transplant (only Triggered)
*           a. Unified-Linked FDB mode and Linked FDB mode :
*               This action mode is not supported - API returns GT_BAD_STATE
*               because triggering this would possibly result with entries which
*               have their new 'source' information properly set - but these entries
*               would not be relocated to the new source portGroups, meaning that
*               packet received from new station location (which was supposed to
*               be transplanted), may still create NA message.
*               Computing new source portGroup is not always possible, especially
*               when station is relocated to trunk or it was previously residing
*               on multiple-portGroups.
*               Application is required to perform Flush operation instead.
*               This would clear previously learnt MAC
*
*       4. Delete (only Triggered)
*           supported in all FDB modes
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeSet
(
    IN GT_U8                        devNum,
    IN CPSS_FDB_ACTION_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgFdbActionModeGet
*
* DESCRIPTION:
*       Gets FDB action mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr    - FDB action mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionModeGet
(
    IN  GT_U8                        devNum,
    OUT CPSS_FDB_ACTION_MODE_ENT     *modePtr
);

/*******************************************************************************
* cpssDxChBrgFdbMacTriggerToggle
*
* DESCRIPTION:
*       Toggle Aging Trigger and cause the device to scan its MAC address table.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - the previous action not done
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*       for multi-port groups device :
*       the trigger is done on ALL port groups regardless to operation mode
*       (Trigger aging/transplant/delete/upload).
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacTriggerToggle
(
    IN GT_U8  devNum
);

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeSet
*
* DESCRIPTION:
*       Sets bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       trunkAgingMode - FDB aging mode for trunk entries.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeSet
(
    IN GT_U8                         devNum,
    IN CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT  trunkAgingMode
);

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeGet
*
* DESCRIPTION:
*       Get bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       trunkAgingModePtr - (pointer to) FDB aging mode for trunk entries.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or aging mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTrunkAgingModeGet
(
    IN GT_U8                        devNum,
    OUT CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT *trunkAgingModePtr
);

/*******************************************************************************
* cpssDxChBrgFdbInit
*
* DESCRIPTION:
*       Init FDB system facility for a device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_SUPPORTED         - on not supported FDB table size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbInit
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* cpssDxChBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           1. Unified FDB mode:
*              portGroup indication is required , for:
*                 a. Aging calculations of Trunks
*                 b. Aging calculations of regular entries when DA refresh is enabled.
*              In these 2 cases entries are considered as aged-out only if AA is
*              receives from all 4 portGroups.
*           2. In Unified-Linked FDB mode and Linked FDB mode:
*              portGroup indication is required for these reasons:
*                 a. Aging calculations of Trunk and regular entries which have
*                    been written to multiple portGroups.
*                 b. New addresses (NA) source portGroup indication is required
*                    so application can add new entry to the exact portGroup
*                    which saw this station (especially on trunk entries).
*                 c. Indication on which portGroup has removed this address when
*                    AA of delete is fetched (relevant when same entry was written
*                    to multiple portGroups).
*                 d. Indication on which portGroup currently has this address
*                    when QR (query reply) is being fetched (relevant when same
*                    entry was written to multiple portGroups).
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfAuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *auMessagesPtr
);

/*******************************************************************************
* cpssDxChBrgFdbFuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place FU messages in common FDB Address Update (AU) messages
*       queue or in separate queue only for FU messages. The function
*       cpssDxChHwPpPhase2Init configures queue for FU messages by the
*       fuqUseSeparate parameter. If common AU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate FU queue is used then function returns only FU messages.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - the device number from which FU are taken
*       numOfFuPtr - (pointer to)max number of FU messages to get
*
* OUTPUTS:
*       numOfFuPtr - (pointer to)actual number of FU messages that were received
*       fuMessagesPtr - array that holds received FU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfFuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - not all results of the CNC block upload
*                          retrieved from the common used FU and CNC
*                          upload queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           since FU is triggered on all port groups , 'duplicated' messages may
*           appear on entries that reside on several/all port groups.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbFuMsgBlockGet
(
    IN     GT_U8                       devNum,
    INOUT  GT_U32                      *numOfFuPtr,
    OUT    CPSS_MAC_UPDATE_MSG_EXT_STC *fuMessagesPtr
);

/************************************************************************
* cpssDxChBrgFdbAuqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       queueType - AUQ or FUQ. FUQ valid for DxCh2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*                    for multi-port groups device :
*                       number of AU messages returned is the summary from AUQs
*                       of all port groups.
*       endOfQueueReachedPtr - (pointer to)
*                       for NON multi-port groups device :
*                           GT_TRUE:
*                               The queue reached to the end.
*                           GT_FALSE:
*                               The queue NOT reached to the end.
*                       for multi-port groups device :
*                           GT_TRUE:
*                               At least one queue of the port groups has reached
*                               its end.(this would alert the application to
*                               start fetching the waiting messages, as at least
*                               one of the AUQs can't accept additional messages).
*                           GT_FALSE:
*                               No queue reached to the end.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, queueType.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_INITIALIZED       - on not initialized queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
GT_STATUS cpssDxChBrgFdbAuqFuqMessagesNumberGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_U32                       *numOfAuPtr,
    OUT GT_BOOL                      *endOfQueueReachedPtr
);

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable refreshing
*                 GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*       for multi-port groups device:
*           Once enabled, MAC address is eligibly to be aged-out if no packets were
*           received from it or sent to it from any other location during aging period.
*           In order to properly compute this application is required to keep a
*           'score board' for every regular entry (same as needed for Trunk entries),
*           each bit representing age bit from each underlying portGroup which this
*           entry is written to. Entry is aged-out if all relevant portGroups have
*           sent AA, indicating entry was aged-out (once DA refresh is disabled -
*           only the source portGroup indication interests us for aging calculations).
*           Note:
*           that it's highly recommended not to enable this feature, as this
*           feature enables transmissions of many AA messages to the CPU
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableGet
*
* DESCRIPTION:
*       Gets status (enabled/disabled) of destination address-based aging bit.
*       When this bit is set, the aging process is done both on the source and
*       the destination address (i.e. the age bit will be refresh when MAC DA
*       hit occurs, as well as MAC SA hit).
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status DA refreshing of aged bit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbAgeBitDaRefreshEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableSet
*
* DESCRIPTION:
*       Enable or disable learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableSet
(
    IN GT_U8                        devNum,
    IN GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableGet
*
* DESCRIPTION:
*       Get Enable or Disable state of learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbRoutedLearningEnableGet
(
    IN GT_U8   devNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbTriggerAuQueueWa
*
* DESCRIPTION:
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*       (FEr#3119)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_STATE             - the secondary AUQ not reached the end of
*                                  queue , meaning no need to set the secondary
*                                  base address again.
*                                  this protect the SW from losing unread messages
*                                  in the secondary queue (otherwise the PP may
*                                  override them with new messages).
*                                  caller may use function
*                                  cpssDxChBrgFdbSecondaryAuMsgBlockGet(...)
*                                  to get messages from the secondary queue.
*       GT_NOT_SUPPORTED         - the device not need / not support the WA
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbTriggerAuQueueWa
(
    IN  GT_U8 devNum
);

/*******************************************************************************
* cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSecondaryAuMsgBlockGet
(
    IN      GT_U8                                    devNum,
    INOUT   GT_U32                                   *numOfAuPtr,
    OUT     CPSS_MAC_UPDATE_MSG_EXT_STC              *auMessagesPtr
);


/*******************************************************************************
* cpssDxChBrgFdbQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - the device number
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPtr - (pointer to) is the relevant queue full
*                   GT_TRUE  - the queue is full
*                   GT_FALSE - the queue is not full
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum , queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       for multi-port groups device :
*           'queue is full' when at least one of the port groups has queue full.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueFullGet
(
    IN GT_U8    devNum,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_BOOL *isFullPtr
);

/*******************************************************************************
* cpssDxChBrgFdbQueueRewindStatusGet
*
* DESCRIPTION:
*       function check if the specific AUQ was 'rewind' since the last time
*       this function was called for that AUQ
*       this information allow the application to know when to finish processing
*       of the WA relate to cpssDxChBrgFdbTriggerAuQueueWa(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       rewindPtr       - (pointer to) was the AUQ rewind since last call
*                           GT_TRUE - AUQ was rewind since last call
*                           GT_FALSE - AUQ wasn't rewind since last call
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbQueueRewindStatusGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *rewindPtr
);

/*******************************************************************************
* cpssDxChBrgFdbMessagesQueueManagerInfoGet
*
* DESCRIPTION:
*       debug tool - print info that the CPSS hold about the state of the :
*           primary AUQ,FUQ,secondary AU queue
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMessagesQueueManagerInfoGet
(
    IN  GT_U8   devNum
);

/*******************************************************************************
* cpssDxChBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_BOOL      age
);

/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableSet
*
* DESCRIPTION:
*       Enable/Disable Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC.
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                 GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableGet
*
* DESCRIPTION:
*       Get the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                    GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbNaMsgVid1EnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);




/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******* start of functions with portGroupsBmp parameter **********************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryPtr  - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*       for multi-port groups device :
*           1. Application should not use this API to add new MAC addresses once
*              SP feature is enabled in any of the ports, due to the fact it may
*              fail on one or more of the portGroups - in this case need
*              to write by index.
*           2. Updating existing MAC addresses is not affected and
*              can be preformed with no problem
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntrySet
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum/vlan or portGroupsBmp
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupQaSend
(
    IN  GT_U8                       devNum,
    IN  GT_PORT_GROUPS_BMP          portGroupsBmp,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC  *macEntryKeyPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number or portGroupsBmp.
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryDelete
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
*
*       for multi-port groups device :
*           1. Unified FDBs mode :
*              API should return GT_BAD_PARAM if value other then CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*              was passed when working in this mode, due to the fact that all
*              FDBs are supposed to be synced.
*              Application should use this API to add new MAC addresses once SP
*              feature is enabled in any of the ports , due to the fact
*              cpssDxChBrgFdbMacEntrySet/cpssDxChBrgFdbPortGroupMacEntrySet API
*              may fail on one or more of the portGroups.
*           2. Unified-Linked FDB mode / Linked FDBs mode :
*              Setting either CPSS_PORT_GROUP_UNAWARE_MODE_CNS or any subset of
*              portGroup can be supported in this mode, still application should
*              note that in Linked FDBs when using multiple portGroups, setting
*              same index would potentially result affecting up to 4 different
*              MAC addresses (up to 2 different MAC addresses in Unified-Linked
*              FDB mode).
*           3. application should be aware that when configuring MAC addresses only
*              to portGroups which none of them is the real source portGroups,
*              these entries would be "unseen" from AU perspective
*              (HW is filtering any AA, TA, QR etc... messages these entries invokes).
*              And although these entries are "Unseen" there are still used for forwarding
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum,portGroupsBmp,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_PORT_GROUPS_BMP           portGroupsBmp,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                          - read only from first active port group of the bitmap.
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with the
*                     entry (even for vidx/trunk entries the field is used by
*                     PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Address entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32        index
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       completedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU message processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - AU message was processed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU message processed is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*       succeededPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU operation succeeded.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation succeeded , otherwise failed
*                      for multi-port groups device :
*                           when bit is set (1) - AU action succeeded (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU action has failed. (in the
*                                            corresponding port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *completedPortGroupsBmpPtr,
    OUT GT_PORT_GROUPS_BMP  *succeededPortGroupsBmpPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN GT_U32       index,
    IN GT_BOOL      age
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       actFinishedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'trigger action' processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - 'trigger action' was completed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'trigger action' is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupTrigActionStatusGet
(
    IN  GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    OUT GT_PORT_GROUPS_BMP  *actFinishedPortGroupsBmpPtr
);

/*******************************************************************************
* cpssDxChBrgFdbPortGroupQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum     - the device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      'the queue is full'.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           'queue is full' , otherwise 'queue is NOT full'
*                      for multi-port groups device :
*                           when bit is set (1) - 'queue is full' (in the
*                                            corresponding port group).
*                           when bit is not set (0) - 'queue is NOT full'. (in the corresponding
*                                            port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp or queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupQueueFullGet
(
    IN GT_U8    devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN CPSS_DXCH_FDB_QUEUE_TYPE_ENT  queueType,
    OUT GT_PORT_GROUPS_BMP  *isFullPortGroupsBmpPtr
);

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******* end of functions with portGroupsBmp parameter ************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

/*******************************************************************************
* cpssDxChBrgFdb16BitFidHashEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: For systems with both new and old devices must configure
*       FID = VID in the eVLAN table.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdb16BitFidHashEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgFdb16BitFidHashEnableGet
*
* DESCRIPTION:
*       Get global configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdb16BitFidHashEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbMaxLengthSrcIdEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLengthSrcIdEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgFdbMaxLengthSrcIdEnableGet
*
* DESCRIPTION:
*       Get global configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMaxLengthSrcIdEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbVid1AssignmentEnableSet
*
* DESCRIPTION:
*       Enable/Disable using of VID1 in FDB entry.
*       The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*       from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: <Tag1 VID> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  Tag1 VID> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbVid1AssignmentEnableSet
(
    IN GT_U8            devNum,
    IN GT_BOOL          enable
);

/*******************************************************************************
* cpssDxChBrgFdbVid1AssignmentEnableGet
*
* DESCRIPTION:
*        Get Enable/Disable status of using of VID1 in FDB entry.
*        The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*        from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: <Tag1 VID> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  Tag1 VID> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbVid1AssignmentEnableGet
(
    IN  GT_U8           devNum,
    OUT GT_BOOL         *enablePtr
);

/*******************************************************************************
* cpssDxChBrgFdbSaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSaLookupAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable,
    IN GT_U32  index
);

/*******************************************************************************
* cpssDxChBrgFdbSaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbSaLookupAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
);

/*******************************************************************************
* cpssDxChBrgFdbDaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDaLookupAnalyzerIndexSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enable,
    IN GT_U32  index
);

/*******************************************************************************
* cpssDxChBrgFdbDaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbDaLookupAnalyzerIndexGet
(
    IN  GT_U8   devNum,
    OUT GT_BOOL *enablePtr,
    OUT GT_U32  *indexPtr
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveUserDefinedSet
*
* DESCRIPTION:
*       Set the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       actUerDefined       - action active UerDefined
*       actUerDefinedMask   - action active UerDefined Mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actUerDefined or actUerDefinedMask out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveUserDefinedSet
(
    IN GT_U8    devNum,
    IN GT_U32   actUerDefined,
    IN GT_U32   actUerDefinedMask
);

/*******************************************************************************
* cpssDxChBrgFdbActionActiveUserDefinedGet
*
* DESCRIPTION:
*       Get the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actUerDefinedPtr      - pointer to action active UerDefined
*       actUerDefinedMaskPtr   - pointer to action active UerDefined Mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbActionActiveUserDefinedGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *actUerDefinedPtr,
    OUT GT_U32   *actUerDefinedMaskPtr
);

/*******************************************************************************
* cpssDxChBrgFdbLearnPrioritySet
*
* DESCRIPTION:
*       Set Learn priority per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       learnPriority  - Learn priority can be Low or High
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPrioritySet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbLearnPrioritySet
(
    IN GT_U8                                devNum,
    IN GT_PORT_NUM                          portNum,
    IN CPSS_DXCH_FDB_LEARN_PRIORITY_ENT     learnPriority
);

/*******************************************************************************
* cpssDxChBrgFdbLearnPriorityGet
*
* DESCRIPTION:
*       Get Learn priority per port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       learnPriorityPtr  - (pointer to)Learn priority can be Low or High
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPriorityGet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbLearnPriorityGet
(
    IN  GT_U8                               devNum,
    IN  GT_PORT_NUM                         portNum,
    OUT CPSS_DXCH_FDB_LEARN_PRIORITY_ENT    *learnPriorityPtr
);

/*******************************************************************************
* cpssDxChBrgFdbUserGroupSet
*
* DESCRIPTION:
*       Set User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       userGroup   - user group
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupSet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUserGroupSet
(
    IN GT_U8            devNum,
    IN GT_PORT_NUM      portNum,
    IN GT_U32           userGroup
);

/*******************************************************************************
* cpssDxChBrgFdbUserGroupGet
*
* DESCRIPTION:
*       Get User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       userGroupPtr  - (pointer to) user group
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupGet
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbUserGroupGet
(
    IN  GT_U8           devNum,
    IN  GT_PORT_NUM     portNum,
    OUT GT_U32          *userGroupPtr
);

/*******************************************************************************
* cpssDxChBrgFdbCrcHashUpperBitsModeSet
*
* DESCRIPTION:
*       Set mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       mode        - the upper bits mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbCrcHashUpperBitsModeSet
(
    IN  GT_U8           devNum,
    IN  CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     mode
);

/*******************************************************************************
* cpssDxChBrgFdbCrcHashUpperBitsModeGet
*
* DESCRIPTION:
*       Get mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       modePtr     - (pointer to) the upper bits mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbCrcHashUpperBitsModeGet
(
    IN  GT_U8           devNum,
    OUT  CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     *modePtr
);


/*******************************************************************************
* cpssDxChBrgFdbBankCounterValueGet
*
* DESCRIPTION:
*       Get the value of counter of the specific FDB table bank.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
* OUTPUTS:
*       valuePtr    - (pointer to)the value of the counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbBankCounterValueGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          bankIndex,
    OUT GT_U32          *valuePtr
);

/*******************************************************************************
* cpssDxChBrgFdbBankCounterUpdate
*
* DESCRIPTION:
*       Update (increment/decrement) the counter of the specific bank
*       The function will fail (GT_BAD_STATE) if the PP is busy with previous
*       update.
*       the application can determine if PP is ready by calling
*       cpssDxChBrgFdbBankCounterUpdateStatusGet.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*       NOTE: this function is needed to be called after calling one of the next:
*           'write by index' - cpssDxChBrgFdbMacEntryWrite ,
*                              cpssDxChBrgFdbPortGroupMacEntryWrite
*           'invalidate by index' - cpssDxChBrgFdbMacEntryInvalidate ,
*                              cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
*       The application logic should be:
*           if last action was 'write by index' then :
*               if the previous entry (in the index) was valid --
*                               do no call this function.
*               if the previous entry (in the index) was not valid --
*                               do 'increment'.
*           if last action was 'invalidate by index' then :
*               if the previous entry (in the index) was valid --
*                               do 'decrement'.
*               if the previous entry (in the index) was not valid --
*                               do no call this function.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
*       incOrDec    - increment or decrement the counter by one.
*                   GT_TRUE - increment the counter
*                   GT_FALSE - decrement the counter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_OUT_OF_RANGE - bankIndex > 15
*       GT_BAD_STATE    - the PP is not ready to get a new update from CPU.
*                         (PP still busy with previous update)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbBankCounterUpdate
(
    IN GT_U8                        devNum,
    IN GT_U32                       bankIndex,
    IN GT_BOOL                      incOrDec
);

/*******************************************************************************
* cpssDxChBrgFdbBankCounterUpdateStatusGet
*
* DESCRIPTION:
*       Get indication if PP finished processing last update of counter of a
*       specific bank. (cpssDxChBrgFdbBankCounterUpdate)
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       isFinishedPtr - (pointer to) indication that the PP finished processing
*                   the last counter update.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbBankCounterUpdateStatusGet
(
    IN GT_U8                        devNum,
    OUT GT_BOOL                     *isFinishedPtr
);


/*******************************************************************************
* cpssDxChBrgFdbMacEntryMove
*
* DESCRIPTION:
*       The function uses the NA message to send to the PP request to move 'valid'
*       entry from <origFdbIndex> in the FDB table to 'new index'.
*       The new index is chosen by the PP (according to the key of the 'Original entry')
*       in similar way to entry that not exists in the FDB and need to be inserted,
*       but with single difference that it can't use the <origFdbIndex>.
*       (AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*       The PP operation status can be retrieved by cpssDxChBrgFdbFromCpuAuMsgStatusGet
*       NOTE: this operation in the PP may fail on one of:
*           1. Original entry is not valid.
*           2. Original entry type is not supported.(IPv6 data/key)
*           3. No free destinations.
*
*
* APPLICABLE DEVICES:
*        none.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       origFdbIndex    - index of the entry that need to be moved to new index
*                       (as will be chosen by the device)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - origFdbIndex out of FDB table range
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6_MC, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbMacEntryMove
(
    IN GT_U8       devNum,
    IN GT_U32      origFdbIndex
);

/*******************************************************************************
* cpssDxChBrgFdbHashByParamsCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the FDB table.
*       The FDB table holds 4 types of entries :
*       1. for specific mac address and VLAN id.
*       2. for specific src IP, dst IP and VLAN id.
*       3. for specific mac address, VLAN id and VID1.
*          (APPLICABLE DEVICES:  Bobcat2; Caelum; Bobcat3)
*       4. for specific src IP, dst IP, VLAN id and VID1.
*          (APPLICABLE DEVICES:  Bobcat2; Caelum; Bobcat3)
*       for more details see CPSS_MAC_ENTRY_EXT_KEY_STC description.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hashMode        - hash mode
*       hashParamsPtr   - (pointer to) HASH parameters
*       macEntryKeyPtr  - (pointer to) key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad vlan-Id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - on not supported hash function for key type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashByParamsCalc
(
    IN  CPSS_MAC_HASH_FUNC_MODE_ENT        hashMode,
    IN  CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC *hashParamsPtr,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC        *macEntryKeyPtr,
    OUT GT_U32                            *hashPtr
);

/*******************************************************************************
* cpssDxChBrgFdbHashCrcMultiResultsByParamsCalc
*
* DESCRIPTION:
*       This function calculates the CRC multiple hash results
*       (indexes into the FDB table).
*       NOTE: the function do not access the HW , and do only SW calculations.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hashParamsPtr            - (pointer to) HASH parameters
*       entryKeyPtr              - (pointer to) entry key
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                   (APPLICABLE RANGES: 0..15)
*       numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 .
*                   (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong parameters in entryKeyPtr.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbHashCrcMultiResultsByParamsCalc
(
    IN  CPSS_DXCH_BRG_FDB_HASH_PARAMS_STC *hashParamsPtr,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC        *entryKeyPtr,
    IN  GT_U32                             multiHashStartBankIndex,
    IN  GT_U32                             numOfBanks,
    OUT GT_U32                             crcMultiHashArr[]
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChBrgFdbh */

