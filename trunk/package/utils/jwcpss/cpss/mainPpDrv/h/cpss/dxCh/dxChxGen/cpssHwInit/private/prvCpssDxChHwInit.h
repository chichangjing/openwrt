/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwInit.h
*
* DESCRIPTION:
*       Includes Core level basic Hw initialization functions, and data
*       structures.
*
* FILE REVISION NUMBER:
*       $Revision: 41 $
*
*******************************************************************************/
#ifndef __prvCpssDxChHwInith
#define __prvCpssDxChHwInith

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PRV_CPSS_DXCH_PROFILES_NUM_CNS       4
#define PRV_CPSS_DXCH2_PROFILES_NUM_CNS      8

#define PRV_CPSS_XCAT_NETWORK_PORTS_SERDES_NUM_CNS  6
#define PRV_CPSS_XCAT_SERDES_NUM_CNS        22
#define PRV_CPSS_XCAT3_SERDES_NUM_CNS       12
#define PRV_CPSS_LION_SERDES_NUM_CNS        24
#define PRV_CPSS_LION2_SERDES_NUM_CNS       24

#define PRV_CPSS_XCAT2_SERDES_NUM_CNS       10

#define PRV_CPSS_BOBCAT2_SERDES_NUM_CNS     36
#define PRV_CPSS_ALDRIN_SERDES_NUM_CNS      33

/* Target local port to MAC mapping for CPU port */
#define PRV_CPSS_DXCH_TARGET_LOCAL_LINK_STATUS_CPU_PORT_NUM_CNS 255

/* Max number of LED ports in LED interface */
#define BOBCAT2_LED_INTERFACE_PORTS_NUM_CNS  12

/*
 * Typedef: enum PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ENT
 *
 * Description: An enum for fields of the EGF_SHT table/register that represent the
 *           physical port and/or the eport.
 * Notes:
 *
*/
typedef enum{
     PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_STP_STATE_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_EGRESS_VLAN_FILTER_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_UC_SRC_ID_FILTER_E          /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_MC_LOCAL_ENABLE_E           /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_IGNORE_PHY_SRC_MC_FILTER_E  /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_STP_STATE_MODE_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ODD_ONLY_SRC_ID_FILTER_ENABLE_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ROUTED_FORWARD_RESTRICTED_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_IPMC_ROUTED_FILTER_ENABLE_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_FROM_CPU_FORWARD_RESTRICTED_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_BRIDGED_FORWARD_RESTRICTED_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_PORT_ISOLATION_MODE_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_EGRESS_EPORT_EVLAN_FILTER_ENABLE_E /* only per EPort */
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_MESH_ID_E /* only per EPort */
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_EPORT_ASSOCIATED_VID1_E
    ,PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_DROP_ON_EPORT_VID1_MISMATCH_E
}PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_ENT
 *
 * Description: An enum for fields of the EGF_EFT register that represent the
 *           physical port .
 * Notes:
 *
*/
typedef enum{
    /* MC_FIFO */
     PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_MC_FIFO_PORT_TO_MC_FIFO_ATTRIBUTION_E                /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_MC_FIFO_PORT_TO_HEMISPHERE_MAP_E                     /* only per physical port */

    /* EGR_FILTER */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_FC_TRIGGER_BY_CN_FRAME_ON_PORT_ENABLE_E   /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_CN_FRAME_TX_ON_PORT_ENABLE_E              /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNREGED_BC_FILTER_ENABLE_E                /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNKNOWN_UC_FILTER_ENABLE_E                /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_UNREGED_MC_FILTER_ENABLE_E                /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_OAM_LOOPBACK_FILTER_ENABLE_E              /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_VIRTUAL_2_PHYSICAL_PORT_REMAP_E           /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_EGR_FILTER_PHYSICAL_PORT_LINK_STATUS_MASK_E          /* only per physical port */

    /* DEV_MAP_TABLE */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_DEV_MAP_TABLE_ADDR_CONSTRUCT_PROFILE_E               /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_DEV_MAP_TABLE_LOCAL_SRC_PORT_MAP_OWN_DEV_ENABLE_E    /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_DEV_MAP_TABLE_LOCAL_TRG_PORT_MAP_OWN_DEV_ENABLE_E    /* only per physical port */
    ,PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_DEV_MAP_TABLE_PORT_ADDR_CONSTRUCT_MODE_ADDRESS_PROFILE_E /* only per physical port */

}PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_ENT;

/*
 * Typedef: enum PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT
 *
 * Description: An enum for types of entries that occupy the TS/ARP/NAT memory
 *
 * values -
 *      PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ARP_E : ARP (48 bits)
 *      PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_REGULAR_E : Tunnel start (192 bits)
 *      PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_DOUBLE_E  : Tunnel start - double length (384 bits)
 *                                                       (for IPv6 TS)
 *      PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_NAT_E : NAT (Network Address Translation. 192 bits)
 *                                                (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Notes:
 *
*/
typedef enum{
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ARP_E,
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_REGULAR_E,
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_TUNNEL_START_DOUBLE_E,
    PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_NAT_E
}PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT;


/* For init system with HW access and register defaults */
extern GT_BOOL dxChInitRegDefaults;

/* For init system with no HW write to the device Device_ID within a Prestera chipset.*/
extern GT_BOOL dxChFastBootSkipOwnDeviceInit;


/*******************************************************************************
* prvCpssDxChHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrInit
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah2 devices.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrInit
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for cheetah-3 devices.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat devices.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcat3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat3 devices.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3HwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChLionHwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Lion devices.
*
* APPLICABLE DEVICES:
*        Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChLion2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Lion2 devices.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion2HwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChLion3HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion3HwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChBobcat2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh Bobcat2 devices.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2HwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcat2HwRegAddrInit
*
* DESCRIPTION:
*       This function initializes the registers struct for DxCh xCat2 devices.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_CPU_MEM        - on malloc failed
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat2HwRegAddrInit
(
    IN GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh2HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh2HwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxCh3HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxCh3HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcatHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcatHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatHwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcat3HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcat3HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3HwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChXcat2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChXcat2HwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat2HwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChLionHwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChLionHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLionHwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChLion2HwRegAddrRemove
*
* DESCRIPTION:
*       This function release the memory that was allocated by the function
*       prvCpssDxChLionHwRegAddrInit(...)
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum  - The PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion2HwRegAddrRemove
(
    IN  GT_U32 devNum
);

/*******************************************************************************
* prvCpssDxChHwRegAddrPortMacUpdate
*
* DESCRIPTION:
*       This function updates mac registers addresses for given port accordingly
*           to currently used MAC Unit
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ifMode   - new interface mode used with this MAC
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrPortMacUpdate
(
    IN GT_U32 devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

/*******************************************************************************
* prvCpssDxCh3HwPpPllUnlockWorkaround
*
* DESCRIPTION:
*       Workaround to eliminate the PLL unlocking issue.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                  - device number
*       networkSerdesPowerUpBmp - bitmap of network SERDES to be power UP
*       xgDevice                - GT_TRUE for XG device, GT_FALSE for GE device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*
* COMMENTS:
*       PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC usage has actual influence
*       only for XG devices, where it is used to get the port number from
*       the power up SERDES bitmap (networkSerdesPowerUpBmp).
*       For GE devices the macro does nothing usefull since previouse one
*       (PRV_CPSS_DXCH_SKIP_NOT_EXIST_SERDES_MAC) make all necessary checks.
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3HwPpPllUnlockWorkaround
(
    IN GT_U8    devNum,
    IN GT_U32   networkSerdesPowerUpBmp,
    IN GT_BOOL  xgDevice
);


/*******************************************************************************
* prvCpssDxChHwDropModeToHwValueAndPacketCommandConvert
*
* DESCRIPTION:
*       convert from  'drop mode' to : packet command and to HW value
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       dropMode - drop mode
* OUTPUTS:
*       packetCommandPtr  - (pointer to) packet command (can be NULL --> ignored)
*       fieldValuePtr     - (pointer to) HW field value (can be NULL --> ignored)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwDropModeToHwValueAndPacketCommandConvert
(
    IN GT_U8        devNum,
    IN CPSS_DROP_MODE_TYPE_ENT dropMode,
    OUT CPSS_PACKET_CMD_ENT *packetCommandPtr,
    OUT GT_U32              *fieldValuePtr
);

/*******************************************************************************
* prvCpssDxChHwHwValueToDropModeAndPacketCommandConvert
*
* DESCRIPTION:
*       convert from HW value : to 'drop mode' and to packet command
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       fieldValue     - HW field value
* OUTPUTS:
*       packetCommandPtr  - (pointer to) packet command (can be NULL --> ignored)
*       dropModePtr       - (pointer to) drop mode (can be NULL --> ignored)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwHwValueToDropModeAndPacketCommandConvert
(
    IN GT_U8        devNum,
    IN GT_U32       fieldValue,
    OUT CPSS_PACKET_CMD_ENT     *packetCommandPtr,
    OUT CPSS_DROP_MODE_TYPE_ENT *dropModePtr
);


/*******************************************************************************
* prvCpssDxChHwPpPortToGopConvert
*
* DESCRIPTION:
*    Private (internal) function converts Port number to Group Of Ports number
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       phyPortNum   - physical port number
*
* OUTPUTS:
*       portGroupPtr - (pointer to) Port Group Number
*                       for not multi port group devices
*                       CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       gopNumPtr    - (pointer to) Group of Ports Number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwPpPortToGopConvert
(
    IN  GT_U8                        devNum,
    IN  GT_PHYSICAL_PORT_NUM         phyPortNum,
    OUT GT_U32                       *portGroupPtr,
    OUT GT_U32                       *gopNumPtr
);

/*******************************************************************************
* prvCpssDxChHwEgfShtFieldSet
*
* DESCRIPTION:
*       Write a field to the EGF_SHT table/register that represent the
*           physical port and/or the eport.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - the device number
*       portNum - the port number (physical port and/or the eport)
*       fieldType - the field type
*       accessPhysicalPort - indication if to access to the physical ports registers
*                   relevant only for the portNum < 256
*       accessEPort  - indication if to access to the EPort table
*       value - the data write to the register and/or table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   prvCpssDxChHwEgfShtFieldSet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ENT  fieldType,
    IN GT_BOOL                  accessPhysicalPort,
    IN GT_BOOL                  accessEPort,
    IN GT_U32                   value
);

/*******************************************************************************
* prvCpssDxChHwEgfShtFieldGet
*
* DESCRIPTION:
*       Read a field to the EGF_SHT table/register that represent the
*           physical port or the eport.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - the device number
*       portNum - the port number (physical port or the eport)
*       fieldType - the field type
*       accessPhysicalPort - GT_TRUE : indication if to access to the physical ports registers
*                               relevant only for the portNum < 256
*                            GT_FALSE : access to the ePort table
*
* OUTPUTS:
*       valuePtr - (pointer to)the data read from the register or table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   prvCpssDxChHwEgfShtFieldGet
(
    IN GT_U8                    devNum,
    IN GT_PORT_NUM              portNum,
    IN PRV_CPSS_DXCH_HW_EGF_SHT_FIELD_ENT  fieldType,
    IN GT_BOOL                  accessPhysicalPort,
    OUT GT_U32                   *valuePtr
);


/*******************************************************************************
* prvCpssDxChHwEgfEftFieldSet
*
* DESCRIPTION:
*       Write a field to the EGF_EFT register that represent the
*           physical port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - the device number
*       portNum - the port number (physical port)
*       fieldType - the field type
*       value - the data write to the register
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   prvCpssDxChHwEgfEftFieldSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_ENT  fieldType,
    IN GT_U32                   value
);

/*******************************************************************************
* prvCpssDxChHwEgfEftFieldGet
*
* DESCRIPTION:
*       Read a field from the EGF_EFT register that represent the
*           physical port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - the device number
*       portNum - the port number (physical port)
*       fieldType - the field type
*
* OUTPUTS:
*       valuePtr - (pointer to)the data read from the register
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   prvCpssDxChHwEgfEftFieldGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN PRV_CPSS_DXCH_HW_EGF_EFT_FIELD_ENT  fieldType,
    OUT GT_U32                   *valuePtr
);

/*******************************************************************************
* prvCpssDxChHwEgfShtPortsBmpTableEntryWrite
*
* DESCRIPTION:
*       Write a whole entry to the table. - for 'bmp of ports' - EGF_SHT tables
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       portsMembersPtr - (pointer to) the bmp of ports that will be written to
*                       the table.
*       isCpuMember_supported - indication if to use 'isCpuMember'
*       isCpuMember     - indication to add the CPU port to the BMP of ports
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwEgfShtPortsBmpTableEntryWrite
(
    IN GT_U8               devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32               tableIndex,
    IN CPSS_PORTS_BMP_STC  *portsMembersPtr,
    IN GT_BOOL              isCpuMember_supported,
    IN GT_BOOL              isCpuMember
);

/*******************************************************************************
* prvCpssDxChHwEgfShtPortsBmpTableEntryRead
*
* DESCRIPTION:
*       Read a whole entry of the table. - for 'bmp of ports' - EGF_SHT tables
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       isCpuMember_supported - indication if to use 'isCpuMemberPtr'
* OUTPUTS:
*       portsMembersPtr - (pointer to) the bmp of ports that will be written to
*                       the table.
*       isCpuMemberPtr  - (pointer to) indication to add the CPU port to the BMP of ports
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwEgfShtPortsBmpTableEntryRead
(
    IN GT_U8               devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32               tableIndex,
    OUT CPSS_PORTS_BMP_STC  *portsMembersPtr,
    IN GT_BOOL              isCpuMember_supported,
    OUT GT_BOOL              *isCpuMemberPtr
);

/*******************************************************************************
* prvCpssDxChHwEgfShtPortsBmpTableEntrySetPort
*
* DESCRIPTION:
*       set (enable/disable) single port (bit index) in entry to the table. - for 'bmp of ports' - EGF_SHT tables
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       tableType - the specific table name
*       entryIndex   - index in the table
*       portNum - the port number
*       enable - enable/disable the port in the entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwEgfShtPortsBmpTableEntrySetPort
(
    IN GT_U8               devNum,
    IN PRV_CPSS_DXCH_TABLE_ENT tableType,
    IN GT_U32               tableIndex,
    IN GT_U32               portNum,
    IN GT_BOOL              enable
);

/*******************************************************************************
* prvCpssDxChHwHaTunnelStartArpNatTableEntryWrite
*
* DESCRIPTION:
*       Write a whole entry to the table. HA memory of 'TunnelStart/ARP/NAT(for NAT capable devices)'
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       entryType - entry type (arp/TS/TS_double)
*       tableIndex   - index in the table.
*                       the ratio is 4 ARP per 1 TS ('regular')
*                       so for 1K TS the indexes are:
*                           for ARP : (APPLICABLE RANGES: 0,1,2,3,4..(4K-1))
*                           for TS  : (APPLICABLE RANGES: 0,1,2,3,4..(1K-1))
*                           for TS_double : (APPLICABLE RANGES: 0,2,4,6,8..(1K-2))
*                           for NAT: (APPLICABLE RANGES: 0,1,2,3,4..(1K-1))
*                       NOTE: the 'double' TS must use only 'even' indexes (0,2,4,6,...)
*
*       entryValuePtr - (pointer to) the data that will be written to the table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwHaTunnelStartArpNatTableEntryWrite
(
    IN GT_U8                                        devNum,
    IN GT_U32                                       portGroupId,
    IN PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT   entryType,
    IN GT_U32                                       tableIndex,
    IN GT_U32                                       *entryValuePtr
);

/*******************************************************************************
* prvCpssDxChHwHaTunnelStartArpNatTableEntryRead
*
* DESCRIPTION:
*       Read a whole entry from the table. HA memory of 'TunnelStart/ARP/NAT(for NAT capable devices)'
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port-group devices that need to access
*                specific port group
*       entryType - entry type (arp/TS/TS_double)
*       tableIndex   - index in the table.
*                       the ratio is 4 ARP per 1 TS ('regular')
*                       so for 1K TS the indexes are:
*                           for ARP : (APPLICABLE RANGES: 0,1,2,3,4..(4K-1))
*                           for TS  : (APPLICABLE RANGES: 0,1,2,3,4..(1K-1))
*                           for TS_double : (APPLICABLE RANGES: 0,2,4,6,8..(1K-2))
*                           for NAT: (APPLICABLE RANGES: 0,1,2,3,4..(1K-1))
*                       NOTE: the 'double' TS must use only 'even' indexes (0,2,4,6,...)
*
* OUTPUTS:
*       entryValuePtr - (pointer to) the data that will be read from the table
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwHaTunnelStartArpNatTableEntryRead
(
    IN GT_U8                                        devNum,
    IN GT_U32                                       portGroupId,
    IN PRV_CPSS_DXCH_HA_TS_ARP_NAT_ENTRY_TYPE_ENT   entryType,
    IN GT_U32                                       tableIndex,
    OUT GT_U32                                      *entryValuePtr
);

/*******************************************************************************
* prvCpssDxChXcat3PortMacRegAddrSwap
*
* DESCRIPTION:
*       MAC address DB update, in order to support extended MAC feature.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portsBmp  - physical port number
*       enable    - extended MAC mode enabled
*                       GT_TRUE - update registers to work in extended mode
*                       GT_FALSE - update registers to work in regular mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcat3PortMacRegAddrSwap
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);


/*******************************************************************************
* prvCpssDxChHwXcat3RegAddrPortMacUpdate
*
* DESCRIPTION:
*       This function updates mac registers addresses for given port accordingly
*       to currently used MAC Unit.
*
* APPLICABLE DEVICES:
*       xCat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PARAM             - wrong device type to operate
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwXcat3RegAddrPortMacUpdate
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
);

/*******************************************************************************
* hwPpPhase1Part2Enhanced_BandWidthByDevRevCoreClockGet
*
* DESCRIPTION:
*       This function returns supported bandwidth by dev family/reviosion/corecolock
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       pipeBWCapacityInGbpsPtr - pointer to bw coresponding to dev family/reviosion/corecolock

* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PTR       - on error
*       GT_NOT_SUPPORTED - on device family/revison/coreclock not found
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_U32 hwPpPhase1Part2Enhanced_BandWidthByDevRevCoreClockGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *pipeBWCapacityInGbpsPtr
);



/*******************************************************************************
* prvCpssDxChPortLedInterfaceGet
*
* DESCRIPTION:
*       Get LED interface number by port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       interfaceNumber - (pointer to) interface number
*
* RETURNS:
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - not supported device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChPortLedInterfaceGet
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32              *interfaceNumberPtr
);


/*******************************************************************************
* prvCpssLedStreamNumOfInterfacesInPortGroupGet
*
* DESCRIPTION:
*       Get the number of LED interfaces that the device's core supports
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.; Bobcat2; Caelum; Bobcat3
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Number of LED interfaces that the device's core supports
*
* COMMENTS:
*       Multi-core device will return information per core !!!
*******************************************************************************/
GT_U32   prvCpssLedStreamNumOfInterfacesInPortGroupGet
(
    IN GT_U8 devNum
);


#ifdef ASIC_SIMULATION
GT_VOID simCoreClockOverwrite
(
    GT_U32 simUserDefinedCoreClockMHz
);

#endif

/*******************************************************************************
* prvCpssDxChHwRegAddrDbRemove
*
* DESCRIPTION:
*       remove the DB of 'register address'
*
* INPUTS:
*       devNum -  the device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChHwRegAddrDbRemove
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChHwInitNumOfSerdesGet
*
* DESCRIPTION:
*       Get number of SERDES lanes in device.
*       Function returns number of SERDES in port group for Lion and Lion2
*
* APPLICABLE DEVICES:
*       xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 - for not applicable device
*       Number of number of SERDES lanes in device or port group
*
* COMMENTS:
*
*******************************************************************************/
GT_U32   prvCpssDxChHwInitNumOfSerdesGet
(
    IN GT_U8 devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #define __prvCpssDxChHwInith  */

