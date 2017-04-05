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
* cpssDxChCscd.h
*
* DESCRIPTION:
*       CPSS DXCH Cascading API.
*
* FILE REVISION NUMBER:
*       $Revision: 40 $
*
*******************************************************************************/

#ifndef __cpssDxChCscdh
#define __cpssDxChCscdh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cscd/cpssGenCscd.h>
#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
/*
 * typedef: enum CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT
 *
 * Description: egress QoS remap mode type
 *
 *
 * Enumerations:
 *      CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E   - disable QOS remapping
 *      CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E - enable QOS remap for control traffic
 *      CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E  - enable QOS remap for data traffic
 *      CPSS_DXCH_CSCD_QOS_REMAP_ALL_E        - enable QOS remap for both control
 *                                              and data traffic
 *
 */
typedef enum
{
    CPSS_DXCH_CSCD_QOS_REMAP_DISABLED_E,
    CPSS_DXCH_CSCD_QOS_REMAP_CNTRL_ONLY_E,
    CPSS_DXCH_CSCD_QOS_REMAP_DATA_ONLY_E,
    CPSS_DXCH_CSCD_QOS_REMAP_ALL_E
}CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT;


/*
 * typedef: enum CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT
 *
 * Description: HyperG.Stack Port Speed
 *
 *
 * Enumerations:
 *  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E    - 10 Gbps - each of the XAUI
 *                                     four SERDES lanes operates at 3.125 Gbps.
 *  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E - 12 Gbps - each of the XAUI
 *                                     four SERDES lanes operates at 3.75 Gbps.
 *
 */
typedef enum
{
    CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_STANDART_E,
    CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ACCELERATED_E
} CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT;

/*
 * typedef: CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT
 *
 * Description: type of the load balancing trunk hash for packets forwarded
 *              via trunk cascade link
 *              Enabled to set the load balancing trunk hash for packets
 *              forwarded via an trunk uplink to be based on the packet's source
 *              port, and not on the packets data.
 *
 * Enumerations:
 *       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
 *              Load balancing trunk hash is based on the ingress pipe
 *              hash mode as configured by function
 *              cpssDxChTrunkHashModeSet(...)
 *       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load balancing trunk
 *              hash for this cascade trunk interface is based on the packet's
 *              source port, regardless of the ingress pipe hash mode.
 *       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E - Load balancing trunk
 *              hash for this cascade trunk interface is based on the packet's
 *              destination port, regardless of the ingress pipe hash mode.
 *              Relevant only for Lion and above.
 *      CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DYNAMIC_LOAD_BALANCING_E -
 *              Unicast:
 *                A logical AND will be performed between the trunk members from the "not trunk members table"
 *                and the queues which did not pass the Xoff threshold (according to the priority of the packet)
 *                from the queue status indications.
 *                - If the result is 0 a logical AND will be performed between the trunk members from the "not
 *                trunk members table" and the designated members table, access to the designated members
 *                table is based on the hash.
 *                - From the result the first port which fulfills the both requirements will be selected.
 *              Multicast:
 *                Current filtering will be performed using the designated members table;
 *
 *
*/


typedef enum
{
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DST_PORT_E,
    CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_DYNAMIC_LOAD_BALANCING_E

} CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT;

/*
 * typedef: CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT
 *
 * Description: TX/RX HyperG.link CRC Mode
 *
 * Enumerations:
 *  CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E   - one byte CRC mode
 *  CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E - four bytes CRC mode
*/

typedef enum
{
    CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E,
    CPSS_DXCH_CSCD_PORT_CRC_TWO_BYTES_E,
    CPSS_DXCH_CSCD_PORT_CRC_THREE_BYTES_E,
    CPSS_DXCH_CSCD_PORT_CRC_FOUR_BYTES_E
} CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT;


/*
 * typedef: CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT
 *
 * Description: Describe Trust Forward DSA modes configured for cascade ports.
 *
 * Enumerations:
 *  CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_GLOBAL_E   - global QoS mode: this is the legacy mode where the packet QoS
 *                                                      profile index is copied from the incoming packet DSA tag<QoS Profile>.
 *                                                      The QoS Profile Table maps QoS profile to the associated TC & DP.
 *                                                      This mode  supports up to 128 QoS profiles globally.
 *  CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_EXTENDED_E - extended QoS mode: DSA tag <QoS Profile> is used to assign
 *                                                      packet's TC and DP. QoS Profile Table is bypassed in this mode.
*/

typedef enum
{
    CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_GLOBAL_E,
    CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_EXTENDED_E
} CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT;




/*
 * typedef: struct CPSS_DXCH_CSCD_QOS_TC_REMAP_STC
 *
 * Description: Structure for remapping TC to new TCs, for each DSA tag
 *              and source port type.
 *
 * Fields:
 *    forwardLocalTc    - Traffic Class for DSA tag FORWARD and for
 *                        local to stack port
 *    forwardStackTc    - Traffic Class for DSA tag FORWARD and for
 *                        stack to stack port
 *    toAnalyzerLocalTc - Traffic Class for DSA tag TO_ANALYZER and
 *                        for local to stack port
 *    toAnalyzerStackTc - Traffic Class for DSA tag TO_ANALYZER and
 *                        for stack to stack port
 *    toCpuLocalTc      - Traffic Class for DSA tag TO_CPU and for
 *                        local to stack port
 *    toCpuStackTc      - Traffic Class for DSA tag TO_CPU and for
 *                        stack to stack port
 *    fromCpuLocalTc    - Traffic Class for DSA tag FROM_CPU and for
 *                        local to stack port
 *    fromCpuStackTc    - Traffic Class for DSA tag FROM_CPU and for
 *                        stack to stack port
 */
typedef struct
{
    GT_U32      forwardLocalTc;
    GT_U32      forwardStackTc;
    GT_U32      toAnalyzerLocalTc;
    GT_U32      toAnalyzerStackTc;
    GT_U32      toCpuLocalTc;
    GT_U32      toCpuStackTc;
    GT_U32      fromCpuLocalTc;
    GT_U32      fromCpuStackTc;
}CPSS_DXCH_CSCD_QOS_TC_REMAP_STC;

/*
 * typedef: struct CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC
 *
 * Description: This structure describes tc,dp remapping index
 *              {tc,portType,dsaCmd,DP} in tc,dp remapping table.
 *              This 8 bit index is a result of {3 bit tc,1 bit portType,2 bit dsaCmd,2 bit DP}
 *
 * Fields:
 *    tc       - QoS profile Traffic Class.(APPLICABLE RANGES: 0..7)
 *    dp       - QoS profile Drop Precedence.
 *    isStack  - source physical port type (stack port or network port):
 *               GT_TRUE  - stack port
 *               GT_FALSE - network port.
 *    dsaTagCmd - dsa tag command.
 */
typedef struct
{
    GT_U32                   tc;
    CPSS_DP_LEVEL_ENT        dp;
    GT_BOOL                  isStack;
    CPSS_DXCH_NET_DSA_CMD_ENT dsaTagCmd;
}CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC;


/*
 * typedef: enum CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT
 *
 * Description: Device Map lookup mode.
 *              The mode defines index calculation that is used to
 *              access Device Map table.
 *
 * Enumerations:
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E - Target Device used as index.
 *
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E -
 *            Both Target Device and Target Port are used to get index.
 *            Index bits representation is Target Device[4:0], Target Port[5:0].
 *            for eArch {<TrgDev>[5:0], Target Port[5:0]}
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_DEV_E -
 *            Both Target Device and Source Device are used to get index.
 *            Index bits representation is Target Device[4:0], Source Device[4:0].
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_PORT_E -
 *            Both Target Device and Source Port are used to get index.
 *            Index bits representation is Target Device[4:0], Source Port[5:0].
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E -
 *            Target Device Source Device and Source Port are used to get index.
 *            Index bits representation is Target Device[4:0],
 *            Source Device[0], Source Port[4:0].
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E -
 *            Target Device Source Device and Source Port are used to get index.
 *            Index bits representation is Target Device[4:0],
 *            Source Device[1:0], Source Port[3:0].
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E -
 *            Target Device Source Device and Source Port are used to get index.
 *            Index bits representation is Target Device[4:0],
 *            Source Device[2:0], Source Port[2:0].
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E -
 *            Target Device Source Device and Source Port are used to get index.
 *            Index bits representation is Target Device[4:0],
 *            Source Device[3:0], Source Port[1:0].
 *      CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E -
 *            Target Device Source Device and Source Port are used to get index.
 *            Index bits representation is Target Device[4:0],
 *            Source Device[4:0], Source Port[0].
 */
typedef enum
{
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_DEV_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_PORT_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E,
     CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E

}CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT
 *
 * Description: Ingress physical port assignment mode
 *
 * Enumerations:
 *      CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DISABLED_E -
 *          No mapping is performed: Source Physical Port = My Physical Port
 *      CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_INTERLAKEN_E -
 *          This is an Interlaken interface. Based on the Rx ILK interface, the
 *          packet is mapped to an ingress physical port number on this device
 *      CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DSA_E -
 *          This is a cascaded DSA interface. Based on the DSA source
 *          information, the packet is mapped to an ingress physical port number
 *          on this device
 */
typedef enum
{
    CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DISABLED_E,
    CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_INTERLAKEN_E,
    CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_DSA_E
}CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT;

/*******************************************************************************
* cpssDxChCscdPortTypeSet
*
* DESCRIPTION:
*            Configure a PP port to be a cascade port. Application is responsible
*            for setting the default values of the port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - The port to be configured as cascade
*       portDirection   - port's direction.
*                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3.)
*       portType - cascade  type DSA tag port or network port
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
*       1. This function also configures insertion of DSA tag for PFC frames.
*          Relevant to egress direction.
*          APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortTypeSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN CPSS_PORT_DIRECTION_ENT      portDirection,
    IN CPSS_CSCD_PORT_TYPE_ENT      portType
);

/*******************************************************************************
* cpssDxChCscdPortTypeGet
*
* DESCRIPTION:
*            Retrieve a PP port cascade port configuration.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - port number or CPU port
*       portDirection   - port's direction.
*                         (APPLICABLE DEVICES: Lion; Lion2; Bobcat2; Caelum; Bobcat3.)
*
* OUTPUTS:
*       portTypePtr - (pointer to) cascade type DSA tag port or network port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_STATE             - when portDirection = 'both' but ingress value
*                                  conflicts the egress value.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. When portDirection = 'Ingress direction' , the (*portTypePtr) will get value of:
*           CPSS_CSCD_PORT_NETWORK_E - network port indication.
*           CPSS_CSCD_PORT_DSA_MODE_1_WORD_E - cascade port indication.
*           APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortTypeGet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN CPSS_PORT_DIRECTION_ENT      portDirection,
    OUT CPSS_CSCD_PORT_TYPE_ENT     *portTypePtr
);

/*******************************************************************************
* cpssDxChCscdDevMapTableSet
*
* DESCRIPTION:
*            Set the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device
*            should be transmitted to.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       targetHwDevNum   - the HW device to be reached via cascade (APPLICABLE RANGES: 0..31)
*       sourceHwDevNum   - source HW device number (APPLICABLE RANGES: 0..31)
*          (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*         Relevant only for
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_DEV_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E,
*          modes, otherwise ignored.
*       portNum        - target / source port number
*                        (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*          Target for
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E mode,
*          Source for
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E,
*          modes, otherwise ignored.
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (APPLICABLE RANGES: 0..63)
*                      or trunk(APPLICABLE RANGES: 0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEn - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       Enabled to set the load balancing trunk hash for packets
*                       forwarded via an trunk uplink to be based on the
*                       packet's source port, and not on the packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet's source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2
*                       and above devices
*       egressAttributesLocallyEn - Determines whether the egress attributes
*                       are determined by the target port even if the target
*                       device is not the local device.
*                       (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or source device,
*                          or port number or bad trunk hash mode
*       GT_BAD_STATE     - the trunk is in bad state , one of:
*                           1. empty trunk (APPLICABLE DEVICES : Lion2)
*                           2. hold members from no local device (APPLICABLE DEVICES : Lion2)
*                           3. hold members from more then single hemisphere (APPLICABLE DEVICES : Lion2)
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapTableSet
(
    IN GT_U8                        devNum,
    IN GT_HW_DEV_NUM                targetHwDevNum,
    IN GT_HW_DEV_NUM                sourceHwDevNum,
    IN GT_PORT_NUM                  portNum,
    IN CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    IN CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT srcPortTrunkHashEn,
    IN GT_BOOL                      egressAttributesLocallyEn
);

/*******************************************************************************
* cpssDxChCscdDevMapTableGet
*
* DESCRIPTION:
*            Get the cascade map table . the map table define the local port or
*            trunk that packets destined to a destination device
*            should be transmitted to.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       targetHwDevNum   - the HW device to be reached via cascade (APPLICABLE RANGES: 0..31)
*       sourceHwDevNum   - source HW device number (APPLICABLE RANGES: 0..31)
*          (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*         Relevant only for
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_DEV_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E,
*          modes, otherwise ignored.
*       portNum        - target / source port number
*                        (APPLICABLE DEVICES: Lion, Lion2, Bobcat2, Caelum, Bobcat3)
*          Target for
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E mode,
*          Source for
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_1BIT_SRC_DEV_SRC_PORT_E
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_2BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_3BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_4BITS_SRC_DEV_SRC_PORT_E,
*           CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_5BITS_SRC_DEV_SRC_PORT_E,
*          modes, otherwise ignored.
*
* OUTPUTS:
*       cascadeLinkPtr - (pointer to)A structure holding the cascade link type
*                      (port (APPLICABLE RANGES: 0..63)
*                      or trunk(APPLICABLE RANGES: 0..127)) and the link number
*                      leading to the target device.
*       srcPortTrunkHashEnPtr - Relevant when (cascadeLinkPtr->linkType ==
*                       CPSS_CSCD_LINK_TYPE_TRUNK_E)
*                       (pointer to) Enabled to set the load balancing trunk
*                       hash for packets forwarded via an trunk uplink to be
*                       based on the packet's source port, and not on the
*                       packets data.
*                       Indicates the type of uplink.
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E -
*                       Load balancing trunk hash is based on the ingress pipe
*                       hash mode as configured by function
*                       cpssDxChTrunkHashGeneralModeSet(...)
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E - Load
*                       balancing trunk hash for this cascade trunk interface is
*                       based on the packet's source port, regardless of the
*                       ingress pipe hash mode
*                       NOTE : this parameter is relevant only to DXCH2 and
*                       above devices
*       egressAttributesLocallyEnPtr - Determines whether the egress attributes
*                       are determined by the target port even if the target
*                       device is not the local device.
*                       (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on wrong device or target device or source device,
*                          or port number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapTableGet
(
    IN GT_U8                         devNum,
    IN GT_HW_DEV_NUM                 targetHwDevNum,
    IN GT_HW_DEV_NUM                 sourceHwDevNum,
    IN GT_PORT_NUM                   portNum,
    OUT CPSS_CSCD_LINK_TYPE_STC      *cascadeLinkPtr,
    OUT CPSS_DXCH_CSCD_TRUNK_LINK_HASH_ENT *srcPortTrunkHashEnPtr,
    OUT GT_BOOL                      *egressAttributesLocallyEnPtr
);

/*******************************************************************************
* cpssDxChCscdRemapQosModeSet
*
*
* DESCRIPTION:
*       Enables/disables remapping of Tc and Dp for Data and Control Traffic
*       on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical port number
*       remapType    - traffic type to remap
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapQosModeSet
(
   IN GT_U8                             devNum,
   IN GT_PHYSICAL_PORT_NUM              portNum,
   IN CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT remapType
);

/*******************************************************************************
* cpssDxChCscdRemapQosModeGet
*
* DESCRIPTION:
*       Get remapping status of Tc and Dp for Data and Control Traffic
*       on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical port number
*
* OUTPUTS:
*       remapTypePtr - traffic type to remap
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapQosModeGet
(
   IN  GT_U8                             devNum,
   IN  GT_PHYSICAL_PORT_NUM              portNum,
   OUT CPSS_DXCH_CSCD_QOS_REMAP_TYPE_ENT *remapTypePtr
);

/*******************************************************************************
* cpssDxChCscdCtrlQosSet
*
* DESCRIPTION:
*       Set control packets TC and DP if Control Remap QoS enabled on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       ctrlTc      - The TC assigned to control packets forwarded to
*                     cascading ports (APPLICABLE RANGES: 0..7)
*       ctrlDp      - The DP assigned to CPU-to-network control traffic or
*                     network-to-CPU traffic.
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*       cpuToCpuDp  - The DP assigned to CPU-to-CPU control traffic
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE     - on wrong ctrlDp or cpuToCpuDp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCtrlQosSet
(
   IN GT_U8             devNum,
   IN GT_U8             ctrlTc,
   IN CPSS_DP_LEVEL_ENT ctrlDp,
   IN CPSS_DP_LEVEL_ENT cpuToCpuDp
);

/*******************************************************************************
* cpssDxChCscdCtrlQosGet
*
* DESCRIPTION:
*       Get control packets TC and DP if Control Remap QoS enabled on a port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*      ctrlTcPtr    - The TC assigned to control packets forwarded to
*                     cascading ports
*      ctrlDpPtr    - The DP assigned to CPU-to-network control traffic or
*                     network-to-CPU traffic.
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*      cpuToCpuDpPtr - The DP assigned to CPU-to-CPU control traffic
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE     - on wrong ctrlDp or cpuToCpuDp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCtrlQosGet
(
   IN  GT_U8             devNum,
   OUT GT_U8             *ctrlTcPtr,
   OUT CPSS_DP_LEVEL_ENT *ctrlDpPtr,
   OUT CPSS_DP_LEVEL_ENT *cpuToCpuDpPtr
);

/*******************************************************************************
* cpssDxChCscdRemapDataQosTblSet
*
* DESCRIPTION:
*       Set table to remap Data packets QoS parameters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       tc          - original packet TC (APPLICABLE RANGES: 0..7)
*       dp          - original packet DP
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*       remapTc     - TC assigned to data packets with DP and TC (APPLICABLE RANGES: 0..7)
*       remapDp     - DP assigned to data packets with DP and TC
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE             - on wrong DP or dp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapDataQosTblSet
(
   IN GT_U8              devNum,
   IN GT_U8              tc,
   IN CPSS_DP_LEVEL_ENT  dp,
   IN GT_U8              remapTc,
   IN CPSS_DP_LEVEL_ENT  remapDp
);

/*******************************************************************************
* cpssDxChCscdRemapDataQosTblGet
*
* DESCRIPTION:
*       Get QoS parameters from table to remap Data packets
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*       tc          - original packet TC (APPLICABLE RANGES: 0..7)
*       dp          - original packet DP
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* OUTPUTS:
*       remapTcPtr  - TC assigned to data packets with DP and TC
*       remapDpPtr  - DP assigned to data packets with DP and TC
*                     DxCh devices support only :
*                     CPSS_DP_RED_E and CPSS_DP_GREEN_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_VALUE             - on wrong DP or dp level value or tc value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdRemapDataQosTblGet
(
   IN  GT_U8              devNum,
   IN  GT_U8              tc,
   IN  CPSS_DP_LEVEL_ENT  dp,
   OUT GT_U8              *remapTcPtr,
   OUT CPSS_DP_LEVEL_ENT  *remapDpPtr
);

/*******************************************************************************
* cpssDxChCscdDsaSrcDevFilterSet
*
* DESCRIPTION:
*       Enable/Disable filtering the ingress DSA tagged packets in which
*       source id equals to local device number.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - physical device number
*       enableOwnDevFltr - enable/disable ingress DSA loop filter
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
*******************************************************************************/
GT_STATUS cpssDxChCscdDsaSrcDevFilterSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enableOwnDevFltr
);

/*******************************************************************************
* cpssDxChCscdDsaSrcDevFilterGet
*
* DESCRIPTION:
*       get value of Enable/Disable filtering the ingress DSA tagged packets in which
*       source id equals to local device number.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - physical device number
*
* OUTPUTS:
*       enableOwnDevFltrPtr - (pointer to) enable/disable ingress DSA loop filter
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChCscdDsaSrcDevFilterGet
(
    IN GT_U8        devNum,
    OUT GT_BOOL      *enableOwnDevFltrPtr
);

/*******************************************************************************
* cpssDxChCscdHyperGInternalPortModeSet
*
* DESCRIPTION:
*       Change HyperG.stack internal port speed.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - cascade HyperG port to be configured
*       mode         - HyperG.Stack Port Speed 10Gbps or 12Gbps
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - the request is not supported
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGInternalPortModeSet
(
    IN  GT_U8                                devNum,
    IN  GT_PHYSICAL_PORT_NUM                 portNum,
    IN  CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT mode
);

/*******************************************************************************
* cpssDxChCscdHyperGInternalPortModeGet
*
* DESCRIPTION:
*       Get status of HyperG port mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - cascade HyperG port to be configured
*
* OUTPUTS:
*       modePtr      - HyperG.Stack Port Speed 10Gbps or 12Gbps
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - the request is not supported
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGInternalPortModeGet
(
    IN  GT_U8                                devNum,
    IN  GT_PHYSICAL_PORT_NUM                 portNum,
    OUT CPSS_DXCH_CSCD_HYPER_G_PORT_MODE_ENT *modePtr
);

/*******************************************************************************
* cpssDxChCscdHyperGPortCrcModeSet
*
* DESCRIPTION:
*       Set CRC mode to be standard 4 bytes or proprietary one byte CRC mode.
*
* APPLICABLE DEVICES:
*        DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3.
*
* INPUTS:
*       devNum           - physical device number
*       portNum          - port number
*       portDirection    - TX/RX cascade port direction
*       crcMode          - TX/RX HyperG.link CRC Mode
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
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGPortCrcModeSet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT  crcMode
);

/*******************************************************************************
* cpssDxChCscdHyperGPortCrcModeGet
*
* DESCRIPTION:
*       Get CRC mode (standard 4 bytes or proprietary one byte).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3.
*
* INPUTS:
*       devNum           - physical device number
*       portNum          - port number
*       portDirection    - TX/RX cascade port direction (ingress or egress)
*
* OUTPUTS:
*       crcModePtr       - TX/RX HyperG.link CRC Mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHyperGPortCrcModeGet
(
    IN  GT_U8                             devNum,
    IN  GT_PHYSICAL_PORT_NUM              portNum,
    IN  CPSS_PORT_DIRECTION_ENT           portDirection,
    OUT CPSS_DXCH_CSCD_PORT_CRC_MODE_ENT  *crcModePtr
);

/*******************************************************************************
* cpssDxChCscdFastFailoverFastStackRecoveryEnableSet
*
* DESCRIPTION:
*       Enable/Disable fast stack recovery.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE: enable fast stack recovery
*                          GT_FALSE: disable fast stack recovery
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverFastStackRecoveryEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
);

/*******************************************************************************
* cpssDxChCscdFastFailoverFastStackRecoveryEnableGet
*
* DESCRIPTION:
*       Get the status of fast stack recovery (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) fast stack recovery status
*                          GT_TRUE: fast stack recovery enabled
*                          GT_FALSE: fast stack recovery disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverFastStackRecoveryEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChCscdFastFailoverSecondaryTargetPortMapSet
*
* DESCRIPTION:
*       Set secondary target port map.
*       If the device is the device where the ring break occured, the
*       packet is looped on the ingress port and is egressed according to
*       Secondary Target Port Map. Also "packetIsLooped" bit is
*       set in DSA tag.
*       If the device receives a packet with "packetIsLooped" bit is set
*       in DSA tag, the packet is forwarded according to Secondary Target Port
*       Map.
*       Device MAP table (cpssDxChCscdDevMapTableSet) is not used in the case.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - ingress port number
*       secondaryTargetPort - secondary target port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - when secondaryTargetPort is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverSecondaryTargetPortMapSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN GT_PHYSICAL_PORT_NUM         secondaryTargetPort
);

/*******************************************************************************
* cpssDxChCscdFastFailoverSecondaryTargetPortMapGet
*
* DESCRIPTION:
*       Get Secondary Target Port Map for given device.
*       If the device is the device where the ring break occured, the
*       packet is looped on the ingress port and is egressed according to
*       Secondary Target Port Map. Also "packetIsLooped" bit is
*       set in DSA tag.
*       If the device receives a packet with "packetIsLooped" bit is set
*       in DSA tag, the packet is forwarded according to Secondary Target Port
*       Map.
*       Device MAP table (cpssDxChCscdDevMapTableSet) is not used in the case.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - ingress port number
*
* OUTPUTS:
*       secondaryTargetPortPtr - (pointer to) secondary target port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverSecondaryTargetPortMapGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_PHYSICAL_PORT_NUM        *secondaryTargetPortPtr
);

/*******************************************************************************
* cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet
*
* DESCRIPTION:
*       Enable/Disable fast failover loopback termination
*       for single-destination packets.
*       There are two configurable options for forwarding single-destination
*       packets that are looped-back across the ring:
*       - Termination Disabled.
*         Unconditionally forward the looped-back packet to the configured
*         backup ring port (for the given ingress ring port) on all the ring
*         devices until it reaches the far-end device where it is again
*         internally looped-back on the ring port and then forward it normally.
*       - Termination Enabled.
*         The looped-back packet passes through the
*         ring until it reaches the target device where it is egressed on its
*         target port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE: enable terminating local loopback
*                                   for fast stack recovery
*                          GT_FALSE: disable terminating local loopback
*                                   for fast stack recovery
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableSet
(
    IN GT_U8        devNum,
    IN GT_BOOL      enable
);


/*******************************************************************************
* cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet
*
* DESCRIPTION:
*       Get the status of fast failover loopback termination
*       for single-destination packets (Enabled/Disabled).
*       There are two configurable options for forwarding single-destination
*       packets that are looped-back across the ring:
*       - Termination Disabled.
*         Unconditionally forward the looped-back packet to the configured
*         backup ring port (for the given ingress ring port) on all the ring
*         devices until it reaches the far-end device where it is again
*         internally looped-back on the ring port and then forward it normally.
*       - Termination Enabled.
*         The looped-back packet passes through the
*         ring until it reaches the target device where it is egressed on its
*         target port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - GT_TRUE: terminating local loopback
*                                   for fast stack recovery enabled
*                          GT_FALSE: terminating local loopback
*                                   for fast stack recovery disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverTerminateLocalLoopbackEnableGet
(
    IN  GT_U8        devNum,
    OUT GT_BOOL      *enablePtr
);

/*******************************************************************************
* cpssDxChCscdFastFailoverPortIsLoopedSet
*
* DESCRIPTION:
*       Enable/disable Fast Failover on a failed port.
*       When port is looped and get packet with DSA tag <Packet is Looped> = 0,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 1
*        - bypass ingress and egress processing
*        - send packet through egress port that defined in secondary target
*          port map (cpssDxChCscdFastFailoverSecondaryTargetPortMapSet).
*       When port is looped and get packet with DSA tag <Packet is Looped> = 1,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 0
*        - Apply usual ingress and egress processing
*       When port is not looped and get packet then device do next:
*        - Apply usual ingress and egress processing
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number (including CPU)
*       isLooped         -  GT_FALSE - Port is not looped.
*                           GT_TRUE - Port is looped.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverPortIsLoopedSet
(
  IN GT_U8                     devNum,
  IN GT_PHYSICAL_PORT_NUM      portNum,
  IN GT_BOOL                   isLooped
);

/*******************************************************************************
* cpssDxChCscdFastFailoverPortIsLoopedGet
*
* DESCRIPTION:
*       Get status (Enable/Disable) of Fast Failover on the failed port.
*       When port is looped and get packet with DSA tag <Packet is Looped> = 0,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 1
*        - bypass ingress and egress processing
*        - send packet through egress port that defined in secondary target
*          port map (cpssDxChCscdFastFailoverSecondaryTargetPortMapSet).
*       When port is looped and get packet with DSA tag <Packet is Looped> = 1,
*       then device do next:
*        - set DSA tag <Packet is Looped> = 0
*        - Apply usual ingress and egress processing
*       When port is not looped and get packet then device do next:
*        - Apply usual ingress and egress processing
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number (including CPU)
*
* OUTPUTS:
*       isLoopedPtr       - (pointer to) Is Looped
*                           GT_FALSE - Port is not looped.
*                           GT_TRUE - Port is looped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdFastFailoverPortIsLoopedGet
(
  IN GT_U8                  devNum,
  IN GT_PHYSICAL_PORT_NUM   portNum,
  OUT GT_BOOL               *isLoopedPtr
);

/*******************************************************************************
* cpssDxChCscdQosPortTcRemapEnableSet
*
* DESCRIPTION:
*       Enable/Disable Traffic Class Remapping on cascading port.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - cascading port number or CPU port
*       enable           - GT_TRUE: enable Traffic Class remapping
*                          GT_FALSE: disable Traffic Class remapping
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosPortTcRemapEnableSet
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChCscdQosPortTcRemapEnableGet
*
* DESCRIPTION:
*       Get the status of Traffic Class Remapping on cascading port
*       (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum           - device number
*       portNum          - cascading port number or CPU port
*
* OUTPUTS:
*       enablePtr        - GT_TRUE: Traffic Class remapping enabled
*                          GT_FALSE: Traffic Class remapping disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosPortTcRemapEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChCscdPortQosDsaModeSet
*
* DESCRIPTION:
*       Configures DSA tag QoS trust mode for cascade port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number.
*       portNum                - cascading port number.
*       portQosDsaMode         - DSA tag QoS mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, port,portQosDsaTrustMode.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortQosDsaModeSet
(
    IN GT_U8                         devNum,
    IN  GT_PHYSICAL_PORT_NUM         portNum,
    IN CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT  portQosDsaMode
);


/*******************************************************************************
* cpssDxChCscdPortQosDsaModeGet
*
* DESCRIPTION:
*       Get DSA tag QoS trust mode for cascade port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                 - device number.
*       portNum                - cascading port number.
*
* OUTPUTS:
*       portQosDsaTrustModePtr - pointer to DSA tag QoS trust mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum, port.
*       GT_BAD_PTR               - portQosDsaTrustModePtr is NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortQosDsaModeGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PHYSICAL_PORT_NUM                        portNum,
    OUT CPSS_DXCH_CSCD_PORT_QOS_DSA_TRUST_MODE_ENT  *portQosDsaTrustModePtr
);


/*******************************************************************************
* cpssDxChCscdQosTcRemapTableSet
*
* DESCRIPTION:
*       Remap Traffic Class on cascading port to new Traffic Classes,
*       for each DSA tag type and for source port type (local or cascading).
*       If the source port is enabled for Traffic Class remapping, then traffic
*       will egress with remapped Traffic Class.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       tc              - Traffic Class of the packet on the source port (APPLICABLE RANGES: 0..7)
*       tcMappingsPtr   - (pointer to )remapped traffic Classes
*                         for ingress Traffic Class
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcRemapTableSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  tc,
    IN  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC         *tcMappingsPtr
);

/*******************************************************************************
* cpssDxChCscdQosTcRemapTableGet
*
* DESCRIPTION:
*       Get the remapped Traffic Classes for given Traffic Class.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number
*       tc              - Traffic Class of the packet on the source port (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       tcMappingsPtr   - (pointer to )remapped Traffic Classes
*                         for ingress Traffic Class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcRemapTableGet
(
    IN   GT_U8                                  devNum,
    IN   GT_U32                                 tc,
    OUT  CPSS_DXCH_CSCD_QOS_TC_REMAP_STC        *tcMappingsPtr
);


/*******************************************************************************
* cpssDxChCscdQosTcDpRemapTableSet
*
* DESCRIPTION:
*       Set remaping of {packet TC,packet ingress Port Type, packet DSA cmd,packet drop precedence}
*       on cascading port to new priority queue for enqueuing the packet and new drop
*       precedence assigned to this packet.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       tcDpRemappingPtr   - (pointer to )tc,dp remapping index structure.
*       newTc     - new priority queue assigned to the packet. (APPLICABLE RANGES: 0..7).
*       newDp     - new drop precedence assigned to the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcDpRemapTableSet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC *tcDpRemappingPtr,
    IN  GT_U32                                   newTc,
    IN  CPSS_DP_LEVEL_ENT                        newDp
);


/*******************************************************************************
* cpssDxChCscdQosTcDpRemapTableGet
*
* DESCRIPTION:
*       Get the remapped value of priority queue and  drop precedence assigned to the packet for given
*       {packet TC,packet ingress Port Type, packet DSA cmd,packet drop precedence}
*       on cascading port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       tcDpRemappingPtr   - (pointer to )tc,dp remapping index structure.
*
* OUTPUTS:
*       remappedTcPtr     - new priority queue assigned to the packet. (APPLICABLE RANGES: 0..7).
*       remappedDpPtr     - new drop precedence assigned to the packet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdQosTcDpRemapTableGet
(
    IN  GT_U8                                    devNum,
    IN  CPSS_DXCH_CSCD_QOS_TC_DP_REMAP_INDEX_STC *tcDpRemappingPtr,
    OUT  GT_U32                                  *remappedTcPtr,
    OUT  CPSS_DP_LEVEL_ENT                       *remappedDpPtr
);

/*******************************************************************************
* cpssDxChCscdPortBridgeBypassEnableSet
*
* DESCRIPTION:
*       The function enables/disables bypass of the bridge engine per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  -  PP's device number.
*       portNum -  port number or CPU port
*                  In eArch devices portNum is default ePort.
*       enable  -  GT_TRUE  - Enable bypass of the bridge engine.
*                  GT_FALSE - Disable bypass of the bridge engine.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If the DSA tag is not extended Forward, the bridging decision
*       is performed regardless of the setting.
*       When bypass enabled the Bridge engine still learn MAC source addresses,
*       but will not modify the packet command, attributes (or forwarding
*       decision).
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortBridgeBypassEnableSet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    IN  GT_BOOL     enable
);

/*******************************************************************************
* cpssDxChCscdPortBridgeBypassEnableGet
*
* DESCRIPTION:
*       The function gets bypass of the bridge engine per port
*       configuration status.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - PP's device number.
*       portNum - port number or CPU port
*                 In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr  -  pointer to bypass of the bridge engine per port
*                     configuration status.
*                     GT_TRUE  - Enable bypass of the bridge engine.
*                     GT_FALSE - Disable bypass of the bridge engine.
*
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortBridgeBypassEnableGet
(
    IN  GT_U8       devNum,
    IN  GT_PORT_NUM portNum,
    OUT GT_BOOL     *enablePtr
);

/*******************************************************************************
* cpssDxChCscdOrigSrcPortFilterEnableSet
*
* DESCRIPTION:
*       Enable/Disable filtering the multi-destination packet that was received
*       by the local device, sent to another device, and sent back to this
*       device, from being sent back to the network port at which it was
*       initially received.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       enable   - GT_TRUE - filter and drop the packet
*                - GT_FALSE - don't filter the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdOrigSrcPortFilterEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChCscdOrigSrcPortFilterEnableGet
*
* DESCRIPTION:
*       Get the status of filtering the multi-destination packet that was
*       received  by the local device, sent to another device, and sent back to
*       this device, from being sent back to the network port at which it was
*       initially received.
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
*       enablePtr   - GT_TRUE - filter and drop the packet
*                   - GT_FALSE - don't filter the packet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdOrigSrcPortFilterEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChCscdDevMapLookupModeSet
*
* DESCRIPTION:
*        Set lookup mode for accessing the Device Map table.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*       mode         - device Map lookup mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapLookupModeSet
(
    IN GT_U8                               devNum,
    IN CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   mode
);

/*******************************************************************************
* cpssDxChCscdDevMapLookupModeGet
*
* DESCRIPTION:
*       Get lookup mode for accessing the Device Map table.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       modePtr      - pointer to device Map lookup mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_BAD_STATE     - wrong hardware value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDevMapLookupModeGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_DEV_MAP_LOOKUP_MODE_ENT   *modePtr
);

/*******************************************************************************
* cpssDxChCscdPortLocalDevMapLookupEnableSet
*
* DESCRIPTION:
*       Enable / Disable the local target port for device map lookup
*       for local device.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number or CPU port
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port
*                         CPSS_DIRECTION_EGRESS_E  -  target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*       enable       - GT_TRUE  - the port is enabled for device map lookup
*                                 for local device.
*                    - GT_FALSE - No access to Device map table for
*                                 local devices.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or port number or portDirection
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To enable access to the Device Map Table for the local target devices
*       -  Enable the local source port for device map lookup
*       -  Enable the local target port for device map lookup
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableSet
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_DIRECTION_ENT   portDirection,
    IN GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChCscdPortLocalDevMapLookupEnableGet
*
* DESCRIPTION:
*       Get status of enabling / disabling the local target port
*       for device map lookup for local device.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number or CPU port
*       portDirection   - port's direction:
*                         CPSS_DIRECTION_INGRESS_E - source port
*                         CPSS_DIRECTION_EGRESS_E  -  target port
*                         CPSS_DIRECTION_BOTH_E    - both source and target ports
*
* OUTPUTS:
*       enablePtr    - pointer to status of enabling / disabling the local
*                      target port for device map lookup for local device.
*                    - GT_TRUE  - target port is enabled for device map lookup
*                                 for local device.
*                    - GT_FALSE - No access to Device map table for
*                                 local devices.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong device or port number or portDirection
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortLocalDevMapLookupEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DIRECTION_ENT      portDirection,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable initial local source port assignment from DSA tag, used
*       for centralized chassis.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       enable        - Centralized Chassis Mode
*                       GT_FALSE: Ingress port is not connected to a line-card
*                                 device in a centralized chassis system
*                       GT_TRUE:  Ingress port is connected to a line-card
*                                 device in a centralized chassis system
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisModeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisModeEnableGet
*
* DESCRIPTION:
*       Get initial local source port assignment from DSA tag, used
*       for centralized chassis.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) Centralized Chassis Mode
*                       GT_FALSE: Ingress port is not connected to a line-card
*                                 device in a centralized chassis system
*                       GT_TRUE:  Ingress port is connected to a line-card
*                                 device in a centralized chassis system
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisModeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_BOOL                             *enablePtr
);



/*******************************************************************************
* cpssDxChCscdCentralizedChassisReservedDevNumSet
*
* DESCRIPTION:
*       Configured what is the device number that must not be used by any of
*       the devices behind this port
*       Relevant only when <CC Mode Enable> = Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*       reservedHwDevNum - reserved hw dev num
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisReservedDevNumSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_HW_DEV_NUM                       reservedHwDevNum
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisReservedDevNumGet
*
* DESCRIPTION:
*       Get CC reserved device number
*       Relevant only when <CC Mode Enable> = Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       reservedHwDevNumPtr - (pointer to) reserved hw dev num
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisReservedDevNumGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_HW_DEV_NUM                       *reservedHwDevNumPtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisSrcIdSet
*
* DESCRIPTION:
*      Configured what is the source ID used by the line card directly
*      attached to the Centralized chassis port
*      Relevant only when <CC Mode Enable> = Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*       srcId          - Src Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisSrcIdSet
(
    IN  GT_U8                        devNum,
    IN  GT_PHYSICAL_PORT_NUM         portNum,
    IN  GT_U32                       srcId
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisSrcIdGet
*
* DESCRIPTION:
*       Get CC Src ID
*       Relevant only when <CC Mode Enable> = Enable
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       srcIdPtr - (pointer to) Src Id
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisSrcIdGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_U32                              *srcIdPtr
);

/*******************************************************************************
* cpssDxChCscdDbRemoteHwDevNumModeSet
*
* DESCRIPTION:
*       Set single/dual HW device number mode to remote HW device number.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       hwDevNum   - HW device number (APPLICABLE RANGES: 0..(4K-1)).
*       hwDevMode  - single/dual HW device number mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - hwDevNum is odd number and hwDevMode is
*                         CPSS_GEN_CFG_HW_DEV_NUM_MODE_DUAL_E
*       GT_OUT_OF_RANGE - when hwDevNum is out of range
*
* COMMENTS:
*       1. Only even device numbers allowed to be marked as "dual HW device"
*       2. "Dual HW device" mode must be  configured before any other
*           configuration that uses hwDevNum.
*       3. There are no restrictions on SW devNum for dual mode devices.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDbRemoteHwDevNumModeSet
(
    IN GT_HW_DEV_NUM                    hwDevNum,
    IN CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT hwDevMode
);

/*******************************************************************************
* cpssDxChCscdDbRemoteHwDevNumModeGet
*
* DESCRIPTION:
*       Get single/dual HW device number mode to remote HW device number.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       hwDevNum   - HW device number (APPLICABLE RANGES: 0..(4K-1)).
*
* OUTPUTS:
*       hwDevModePtr  - (pointer to) single/dual HW device number mode.
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - when hwDevNum is out of range
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdDbRemoteHwDevNumModeGet
(
    IN  GT_HW_DEV_NUM                    hwDevNum,
    OUT CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT *hwDevModePtr
);


/*******************************************************************************
* cpssDxChCscdPortStackAggregationEnableSet
*
* DESCRIPTION:
*        Enable/disable stack aggregation per port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE - enable stack aggregation
*                     GT_FALSE - disable stack aggregation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B0
*******************************************************************************/
GT_STATUS cpssDxChCscdPortStackAggregationEnableSet
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL              enable
);


/*******************************************************************************
* cpssDxChCscdPortStackAggregationEnableGet
*
* DESCRIPTION:
*        Get enable/disable status of stack aggregation per port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - aggregator port number
*
* OUTPUTS:
*       enablePtr   - (pointer to)GT_TRUE - enable stack aggregation
*                                 GT_FALSE - disable stack aggregation
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device or portNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B0
*******************************************************************************/
GT_STATUS cpssDxChCscdPortStackAggregationEnableGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL              *enablePtr
);


/*******************************************************************************
* cpssDxChCscdPortStackAggregationConfigSet
*
* DESCRIPTION:
*        Set stack aggregation configuration per egress port.
*        All Forward DSA tagged packets transmitted from cascaded port with
*        these configurable source device number, source port numbers and
*        source IDs when feature enabled by
*        cpssDxChCscdPortStackAggregationEnableSet().
*        Configuration should be applied on aggregator device only.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       aggHwDevNum - aggregator HW device number
*       aggPortNum  - aggregator port number
*       aggSrcId    - aggregator source ID
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portNum
*       GT_OUT_OF_RANGE          - on wrong aggHwDevNum, aggPortNum, aggSrcId
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B0
*******************************************************************************/
GT_STATUS cpssDxChCscdPortStackAggregationConfigSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_HW_DEV_NUM        aggHwDevNum,
    IN  GT_PORT_NUM          aggPortNum,
    IN  GT_U32               aggSrcId
);


/*******************************************************************************
* cpssDxChCscdPortStackAggregationConfigGet
*
* DESCRIPTION:
*        Get stack aggregation configuration per egress port.
*        All Forward DSA tagged packets transmitted from cascaded port with
*        these configurable source device number, source port numbers and
*        source IDs when feature enabled by
*        cpssDxChCscdPortStackAggregationEnableSet().
*        Configuration should be applied on aggregator device only.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number
*
* OUTPUTS:
*       aggHwDevNumPtr - (pointer to) aggregator HW device number
*       aggPortNumPtr  - (pointer to) aggregator port number
*       aggSrcIdPtr    - (pointer to) aggregator source ID
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device, portNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B0
*******************************************************************************/
GT_STATUS cpssDxChCscdPortStackAggregationConfigGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_HW_DEV_NUM        *aggHwDevNumPtr,
    OUT GT_PORT_NUM          *aggPortNumPtr,
    OUT GT_U32               *aggSrcIdPtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseSet
*
* DESCRIPTION:
*       Set initial default ePort mapping assignment on ingress centralized chassis
*       enabled ports, when packets are received from line-card port/trunk accordingly.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       portBaseEport - base value used for default ePort mapping on
*                       ingress CC enabled ports, when packets are
*                       received from line-card port.
*                       (APPLICABLE RANGES: 0..0x1FFFF)
*       trunkBaseEport- base value used for default ePort mapping on
*                       ingress CC enabled ports, when packets are
*                       received from line-card trunk.
*                       (APPLICABLE RANGES: 0..0x1FFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range parameter
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_PORT_NUM                         portBaseEport,
    IN  GT_PORT_NUM                         trunkBaseEport
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseGet
*
* DESCRIPTION:
*       Get initial default ePort mapping assignment on ingress centralized chassis
*       enabled ports, when packets are received from line-card port/trunk accordingly.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*
* OUTPUTS:
*       portBaseEportPtr  - (pointer to) base value used for default
*                           ePort mapping on ingress CC enabled ports,
*                           when packets are received from line-card port.
*                           (APPLICABLE RANGES: 0..0x1FFFF)
*       trunkBaseEportPtr - (pointer to) base value used for default
*                           ePort mapping on ingress CC enabled ports,
*                           when packets are received from line-card trunk.
*                           (APPLICABLE RANGES: 0..0x1FFFF)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisLineCardDefaultEportBaseGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_PORT_NUM                         *portBaseEportPtr,
    OUT GT_PORT_NUM                         *trunkBaseEportPtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountSet
*
* DESCRIPTION:
*       Set the amount of least significant bits taken from DSA tag
*       for assigning a default source ePort on CC ports, for packets received
*       from line-card device trunks/physical ports accordingly.
*       Relevant only when <CC Mode Enable> = Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       srcTrunkLsbAmount - amount of least significant bits taken from DSA<SrcTrunk>
*                           for assigning a default source ePort on CC ports, for
*                           packets received from line-card device trunks.
*                           Relevant only for packet DSA<SrcIsTrunk> = 1.
*                           (APPLICABLE RANGES: 0..7)
*       srcPortLsbAmount  - amount of least significant bits taken from DSA<SrcPort>
*                           for assigning a default source ePort on CC ports, for
*                           packets received from line-card device physical ports.
*                           Relevant only for packet DSA<SrcIsTrunk> = 0.
*                           (APPLICABLE RANGES: 0..6)
*       srcDevLsbAmount   - amount of least significant bits taken from DSA<SrcDev>
*                           for assigning a default source ePort on CC ports, for
*                           packets received from line-card device physical ports.
*                           Relevant only for packet DSA<SrcIsTrunk> = 0.
*                           (APPLICABLE RANGES: 0..5)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range parameter
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              srcTrunkLsbAmount,
    IN  GT_U32                              srcPortLsbAmount,
    IN  GT_U32                              srcDevLsbAmount
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountGet
*
* DESCRIPTION:
*       Get the amount of least significant bits taken from DSA tag
*       for assigning a default source ePort on CC ports, for packets received
*       from line-card device trunks/physical ports accordingly.
*       Relevant only when <CC Mode Enable> = Enable.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       srcTrunkLsbAmountPtr - (pointer to) amount of least significant bits taken
*                               from DSA<SrcTrunk> for assigning a default source
*                               ePort on CC ports, for packets received from
*                               line-card device trunks.
*                               Relevant only for packet DSA<SrcIsTrunk> = 1.
*                               (APPLICABLE RANGES: 0..7)
*       srcPortLsbAmountPtr  - (pointer to) amount of least significant bits taken
*                               from DSA<SrcPort> for assigning a default source
*                               ePort on CC ports, for packets received from
*                               line-card device physical ports.
*                               Relevant only for packet DSA<SrcIsTrunk> = 0.
*                               (APPLICABLE RANGES: 0..6)
*       srcDevLsbAmountPtr   - (pointer to) amount of least significant bits taken
*                               from DSA<SrcDev> for assigning a default source
*                               ePort on CC ports, for packets received from
*                               line-card device physical ports.
*                               Relevant only for packet DSA<SrcIsTrunk> = 0.
*                               (APPLICABLE RANGES: 0..5)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMappingDsaSrcLsbAmountGet
(
    IN  GT_U8                               devNum,
    OUT GT_U32                              *srcTrunkLsbAmountPtr,
    OUT GT_U32                              *srcPortLsbAmountPtr,
    OUT GT_U32                              *srcDevLsbAmountPtr
);

/*******************************************************************************
* cpssDxChCscdPortMruCheckOnCascadeEnableSet
*
* DESCRIPTION:
*       Enable/Disable MRU Check On Cascade Port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number or CPU port
*       enable        - MRU check Mode
*                       GT_FALSE: Disable MRU Check On Cascade Port
*                       GT_TRUE:  Enable MRU Check On Cascade Port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortMruCheckOnCascadeEnableSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  GT_BOOL                             enable
);

/*******************************************************************************
* cpssDxChCscdPortMruCheckOnCascadeEnableGet
*
* DESCRIPTION:
*       Get (Enable/Disable) MRU Check On Cascade Port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number or CPU port
*
* OUTPUTS:
*       enablePtr     - (pointer to) MRU check Mode
*                       GT_FALSE: Disable MRU Check On Cascade Port
*                       GT_TRUE:  Enable MRU Check On Cascade Port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChCscdPortMruCheckOnCascadeEnableGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    OUT GT_BOOL                             *enablePtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountSet
*
* DESCRIPTION:
*       Set the number of LSB bits taken from the DSA tag <source device> and
*       <source port>, that are used to map the My Physical port to a physical
*       port.
*       Relevant when <Assign Physical Port From DSA Enable> is enabled in the
*       My Physical Port Table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       srcDevLsbAmount     - the number of bits from the DSA <source device>
*                             used for the mapping
*                             (APPLICABLE RANGES: 0..15)
*       srcPortLsbAmount    - the number of bits from the DSA <source port> used
*                             for the mapping
*                             (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountSet
(
    IN GT_U8        devNum,
    IN GT_U32       srcDevLsbAmount,
    IN GT_U32       srcPortLsbAmount
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountGet
*
* DESCRIPTION:
*       Get the number of LSB bits taken from the DSA tag <source device> and
*       <source port>, that are used to map the My Physical port to a physical
*       port.
*       Relevant when <Assign Physical Port From DSA Enable> is enabled in the
*       My Physical Port Table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       srcDevLsbAmountPtr  - (pointer to) the number of bits from the DSA
*                             <source device> used for the mapping
*       srcPortLsbAmountPtr - (pointer to) the number of bits from the DSA
*                             <source port> used for the mapping
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortMappingDsaSrcLsbAmountGet
(
    IN  GT_U8       devNum,
    OUT GT_U32      *srcDevLsbAmountPtr,
    OUT GT_U32      *srcPortLsbAmountPtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeSet
*
* DESCRIPTION:
*       Set the ingress physical port assignment mode
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port
*       assignmentMode      - the physical port assignment mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeSet
(
    IN GT_U8                                                        devNum,
    IN GT_PHYSICAL_PORT_NUM                                         portNum,
    IN CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT     assignmentMode
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeGet
*
* DESCRIPTION:
*       Get the ingress physical port assignment mode
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port
*
* OUTPUTS:
*       assignmentModePtr   - (pointer to) the physical port assignment mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortAssignmentModeGet
(
    IN  GT_U8                                                       devNum,
    IN  GT_PHYSICAL_PORT_NUM                                        portNum,
    OUT CPSS_DXCH_CSCD_INGRESS_PHYSICAL_PORT_ASSIGNMENT_MODE_ENT    *assignmentModePtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortBaseSet
*
* DESCRIPTION:
*       Set the ingress physical port base. It's used to map the My Physical
*       port to a physical port.
*       Relevant only when the ingress Physical Port assignment mode is
*       configured to Interlaken or DSA mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port
*       physicalPortBase    - the physical port base (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortBaseSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   physicalPortBase
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortBaseGet
*
* DESCRIPTION:
*       Get the ingress physical port base. It's used to map the My Physical
*       port to a physical port.
*       Relevant only when the ingress Physical Port assignment mode is
*       configured to Interlaken or DSA mode.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port
*
* OUTPUTS:
*       physicalPortBasePtr - (pointer to) the physical port base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortBaseGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_U32                  *physicalPortBasePtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableSet
*
* DESCRIPTION:
*       Enable the assignment of the packet source device to be the local device
*       number
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port
*       enable              - enable the assignment
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL                  enable
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableGet
*
* DESCRIPTION:
*       Get the enabling status of the assignment of the packet source device to
*       be the local device number
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port
*
* OUTPUTS:
*       enablePtr           - (pointer to) the enabling status of the assignment
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisMyPhysicalPortAssignSrcDevEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableSet
*
* DESCRIPTION:
*       Enable mapping of target physical port to a remote physical port that
*       resides over a DSA-tagged interface
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - target device number
*       portNum             - target physical port
*       enable              - enable mapping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableGet
*
* DESCRIPTION:
*       Get the enabling status of the mapping of target physical port to a
*       remote physical port that resides over a DSA-tagged interface
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - target device number
*       portNum             - target physical port
*
* OUTPUTS:
*       enablePtr           - (pointer to) the enabling status
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_PHYSICAL_PORT_NUM      portNum,
    OUT GT_BOOL                   *enablePtr
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisRemotePhysicalPortMapSet
*
* DESCRIPTION:
*       Map a target physical port to a remote physical port that resides over
*       a DSA-tagged interface
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - target device number
*       portNum                 - target physical port
*       remotePhysicalHwDevNum  - the remote physical HW device
*                                 (APPLICABLE RANGES: 0..4095)
*       remotePhysicalPortNum   - the remote physical port
*                                 (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The mapping should be enabled in order to take effect. To enable the
*       mapping use cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableSet
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisRemotePhysicalPortMapSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_HW_DEV_NUM           remotePhysicalHwDevNum,
    IN  GT_PHYSICAL_PORT_NUM    remotePhysicalPortNum
);

/*******************************************************************************
* cpssDxChCscdCentralizedChassisRemotePhysicalPortMapGet
*
* DESCRIPTION:
*       Get the mapping of a target physical port to a remote physical port that
*       resides over a DSA-tagged interface
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                      - target device number
*       portNum                     - target physical port
*
* OUTPUTS:
*       remotePhysicalHwDevNumPtr   - (pointer to) the remote physical HW device
*       remotePhysicalPortNumPtr    - (pointer to) the remote physical port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The mapping takes effect only if it's enabled. To check whether it's
*       enabled use cpssDxChCscdCentralizedChassisRemotePhysicalPortMapEnableGet
*
*******************************************************************************/
GT_STATUS cpssDxChCscdCentralizedChassisRemotePhysicalPortMapGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_HW_DEV_NUM           *remotePhysicalHwDevNumPtr,
    OUT GT_PHYSICAL_PORT_NUM    *remotePhysicalPortNumPtr
);

/*******************************************************************************
* cpssDxChCscdHashInDsaEnableSet
*
* DESCRIPTION:
*       Enable a packet's hash to be extracted from(rx) and/or inserted into(tx)
*       DSA tag. Relevant for FORWARD DSA tagged packets only.
*
* APPLICABLE DEVICES:
*       Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       direction     - port's direction (rx/tx/both)
*       enable        - GT_TRUE:  enable
*                       GT_FALSE: disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHashInDsaEnableSet
(
    IN GT_U8                   devNum,
    IN GT_PHYSICAL_PORT_NUM    portNum,
    IN CPSS_PORT_DIRECTION_ENT direction,
    IN GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChCscdHashInDsaEnableGet
*
* DESCRIPTION:
*       Get value of flag determining whether a packet's hash should be
*       extracted from(rx)/inserted info(tx) DSA tag.
*       Relevand for FORWARD DSA tagged packets only.
*
* APPLICABLE DEVICES:
*       Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       direction     - port's direction (rx or tx. Not both!)
*
* OUTPUTS:
*       enablePtr     - (pointer to) flag value
*                       GT_TRUE:  use DSA to extract/store hash.
*                       GT_FALSE: don't use DSA to extract/store hash.

*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCscdHashInDsaEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChCscdh */

