/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRam.h
*
* DESCRIPTION:
*       Private CPSS DXCH LPM RAM functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmRamh
#define __prvCpssDxChLpmRamh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>

/* max number of virtual routers */
#define PRV_CPSS_DXCH_LPM_RAM_NUM_OF_VIRTUAL_ROUTERS_CNS     4096

/* set bit in the octet_to_block bitmap */
#define PRV_CPSS_DXCH_LPM_RAM_OCTET_TO_BLOCK_MAPPING_SET_MAC(shadow,protocol,octet,block)\
    (shadowPtr->lpmRamOctetsToBlockMappingPtr[block].octetsToBlockMappingBitmap[protocol] |= (1<<octet))

/* clear bit in the octet_to_block bitmap */
#define PRV_CPSS_DXCH_LPM_RAM_OCTET_TO_BLOCK_MAPPING_CLEAR_MAC(shadow,protocol,octet,block)\
    (shadowPtr->lpmRamOctetsToBlockMappingPtr[block].octetsToBlockMappingBitmap[protocol] &= ~(1<<octet))
  
/* get whether the block is used for the given protocol by a specific octet */
#define PRV_CPSS_DXCH_LPM_RAM_IS_BLOCK_USED_BY_OCTET_IN_PROTOCOL_GET_MAC(shadow,protocol,octet,block)\
    (((shadowPtr->lpmRamOctetsToBlockMappingPtr[block].octetsToBlockMappingBitmap[protocol]&(1<<octet))!=0)?GT_TRUE:GT_FALSE)

/* get whether the block is used for the given protocol by any octet */
#define PRV_CPSS_DXCH_LPM_RAM_IS_BLOCK_USED_BY_PROTOCOL_GET_MAC(shadow,protocol,block)\
      ((shadowPtr->lpmRamOctetsToBlockMappingPtr[block].octetsToBlockMappingBitmap[protocol]==0)?GT_FALSE:GT_TRUE)

/* the LPM RAM total blocks size including the gap */
#define PRV_CPSS_DXCH_LPM_RAM_TOTAL_BLOCKS_SIZE_INCLUDING_GAP_CNS _16KB 

/*******************************************************************************
* prvCpssDxChLpmRamMemFreeListMng
*
* DESCRIPTION:
*       This function is used to collect all Pp Narrow Sram memory free
*       operations inorder to be freed at the end of these operations.
*
* INPUTS:
*       ppMemAddr - the address in the HW (the device memory pool) to record.
*       operation - the operation (see
*                   PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_RESET_OP_E,
*                   PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ADD_MEM_OP_E,
*                   PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_FREE_MEM_OP_E
*       memBlockListPtrPtr - (pointer to) the memory block list to act upon.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamMemFreeListMng
(
    IN  GT_UINTPTR                                  ppMemAddr,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_OP_ENT       operation,
    IN  PRV_CPSS_DXCH_LPM_RAM_MEM_LIST_ENTRY_STC    **memBlockListPtrPtr
);

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PARAMS_STC
 *
 * Description: A struct that holds the parameters for update of the VRF table
 *
 * Fields:
 *      vrId      - the virtual router/fabric id
 *      protocol  - the protocol to update
 *      shadowPtr - the LPM shadow to update
 */
typedef struct _writeVRFTable2DeviceRAMFuncParams
{
    IN GT_U32                                       vrId;
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT         protocol;
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC             *shadowPtr;
}PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PARAMS_STC;

/*******************************************************************************
* prvCpssDxChLpmRamDbCreate
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. The LPM DB manage adding, deleting and searching
*       This function creates LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtrPtr       - the LPM DB
*       protocolBitmap    - the protocols that are used in this LPM DB
*       memoryCfgPtr      - (pointer to) the memory configuration of this LPM DB 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_OUT_OF_CPU_MEM        - on failure to allocate memory
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbCreate
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  **lpmDbPtrPtr,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_BMP        protocolBitmap,
    IN PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC      *memoryCfgPtr

);

/*******************************************************************************
* prvCpssDxChLpmRamDbDelete
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. The LPM DB manage adding, deleting and searching
*       IPv4/6 UC/MC and FCoE prefixes.
*       This function deletes LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_OUT_OF_CPU_MEM        - on failure to allocate memory
*       GT_FAIL                  - on error
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbDelete
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamDbConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       LPM DB is a database that holds LPM shadow information that is shared
*       to one PP or more. The LPM DB manages adding, deleting and searching
*       IPv4/6 UC/MC and FCoE prefixes.
*       This function retrieves configuration of LPM DB.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - (pointer to) the LPM DB
*
* OUTPUTS:
*       protocolBitmapPtr - (pointer to) the protocols that are used in this LPM DB
*       memoryCfgPtr      - (pointer to) the memory configuration of this LPM DB
*
* RETURNS:
*       GT_OK             - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbConfigGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC    *lpmDbPtr,
    OUT PRV_CPSS_DXCH_LPM_PROTOCOL_BMP          *protocolBitmapPtr,
    OUT PRV_CPSS_DXCH_LPM_RAM_CONFIG_STC        *memoryCfgPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamDbDevListAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds list of devices to an existing LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       devListArray      - array of device ids to add to the LPM DB
*       numOfDevs         - the number of device ids in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbDevListAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U8                                 devListArray[],
    IN GT_U32                                numOfDevs
);

/*******************************************************************************
* prvCpssDxChLpmRamDbDevListRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function removes devices from an existing LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       devListArray      - array of device ids to remove from the LPM DB
*       numOfDevs         - the number of device ids in the array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbDevListRemove
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U8                                 devListArray[],
    IN GT_U32                                numOfDevs
);

/*******************************************************************************
* prvCpssDxChLpmRamDbDevListGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       numOfDevsPtr      - points to the size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr      - points to the number of devices retrieved
*       devListArray      - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to device list
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbDevListGet
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    INOUT GT_U32                                *numOfDevsPtr,
    OUT   GT_U8                                 devListArray[]
);

/*******************************************************************************
* prvCpssDxChLpmRamVirtualRouterAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       vrId              - The virtual's router ID.
*       vrConfigPtr       - Configuration of the virtual router
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_ALREADY_EXIST         - if the vr id is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamVirtualRouterAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId,
    IN PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC   *vrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamVirtualRouterDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function deletes a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB
*       vrId              - The virtual's router ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_FOUND             - if the LPM DB id or vr id does not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All prefixes must be previously deleted.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamVirtualRouterDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId
);

/*******************************************************************************
* prvCpssDxChLpmRamVirtualRouterGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets configuration of a virtual router for specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - the LPM DB.
*       vrId              - The virtual's router ID.
*
* OUTPUTS:
*       vrConfigPtr       - Configuration of the virtual router
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_NOT_FOUND             - if the LPM DB id or vr id does not found
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamVirtualRouterGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN  GT_U32                               vrId,
    OUT PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC  *vrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a new IPv4 prefix to a Virtual Router in a
*       specific LPM DB or overrides an existing existing IPv4 prefix.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address of this prefix.
*       prefixLen         - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr    - (points to) The next hop pointer to set for this prefix.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_ALREADY_EXIST         - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPADDR                                    ipAddr,
    IN GT_U32                                       prefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *nextHopInfoPtr,
    IN GT_BOOL                                      override
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Creates a new or override an existing bulk of IPv4 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv4PrefixArrayLen    - Length of UC prefix array.
*       ipv4PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully added
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if adding one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixBulkAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Deletes an existing IPv4 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr        - The LPM DB.
*       vrId            - The virtual router id.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId,
    IN GT_IPADDR                             ipAddr,
    IN GT_U32                                prefixLen
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixBulkDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*       Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv4PrefixArrayLen    - Length of UC prefix array.
*       ipv4PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully deleted
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - if NULL pointer
*       GT_FAIL                  - if deleting one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixBulkDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Flushes the unicast IPv4 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixSearch
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given ip-uc address, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr    - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPADDR                                   ipAddr,
    IN  GT_U32                                      prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an IP-Unicast prefix with larger (ipAddrPtr,prefixLenPtr)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB.
*       vrId              - The virtual router Id to get the entry from.
*       ipAddrPtr         - The ip address to start the search from.
*       prefixLenPtr      - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopInfoPtr    - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (ipAddrPtr,prefixLenPtr) must be a valid values, it
*           means that they exist in the IP-UC Table, unless this is the first
*           call to this function, then the value of (ipAddrPtr,prefixLenPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv4
*           UC prefix get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC          *lpmDbPtr,
    IN    GT_U32                                        vrId,
    INOUT GT_IPADDR                                     *ipAddrPtr,
    INOUT GT_U32                                        *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC     *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4UcPrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given ip address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopPointerPtr - The next hop pointer bounded to the longest match
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4UcPrefixGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPADDR                                   ipAddr,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopPointerPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual private network identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*       mcRouteEntryPtr   - the mc Route pointer to set for the mc entry.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_NOT_FOUND             - prefix was not found when override is GT_TRUE
*       GT_ALREADY_EXIST         - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  To override the default mc route use ipGroup = 0.
*       2.  If (S,G) MC group is added when (*,G) doesn't exists then implicit
*           (*,G) is added pointing to (*,*) default. Application added (*,G)
*           will override the implicit (*,G).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPADDR                                    ipGroup,
    IN GT_U32                                       ipGroupPrefixLen,
    IN GT_IPADDR                                    ipSrc,
    IN GT_U32                                       ipSrcPrefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *mcRouteEntryPtr,
    IN GT_BOOL                                      override
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefixLen = 0.
*       2.  If no (*,G) was added but (S,G) were added, then implicit (*,G)
*           that points to (*,*) is added. If (*,G) is added later, it will
*           replace the implicit (*,G).
*           When deleting (*,G), then if there are still (S,G), an implicit (*,G)
*           pointing to (*,*) will be added.
*           When deleting last (S,G) and the (*,G) was implicitly added, then
*           the (*,G) will be deleted as well.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipGroup,
    IN GT_U32                               ipGroupPrefixLen,
    IN GT_IPADDR                            ipSrc,
    IN GT_U32                               ipSrcPrefixLen
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntriesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntriesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the muticast (ipSrc,ipGroup) entry, used
*       to find specific multicast adrress entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the next entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the next entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteEntryPtr   - the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPADDR                                   ipGroup,
    IN  GT_U32                                      ipGroupPrefixLen,
    IN  GT_IPADDR                                   ipSrc,
    IN  GT_U32                                      ipSrcPrefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *mcRouteEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv4McEntryGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the next muticast (ipSrcPtr,ipGroupPtr) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr            - The LPM DB
*       vrId                - The virtual router Id.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroupPtr prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrcPtr prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - The next ip Group address.
*       ipGroupPrefixLenPtr - ipGroupPtr prefix length.
*       ipSrcPtr            - The next ip Source address.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*       mcRouteEntryPtr     - the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device.
*
* COMMENTS:
*       1.  The values of (ipSrcPtr,ipGroupPtr) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipSrcPtr,ipGroupPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv4
*           MC get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv4McEntryGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipGroupPtr,
    INOUT GT_U32                                    *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                                 *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRouteEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       creates a new or override an existing Ipv6 prefix in a Virtual Router
*       for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - Points to the destination IP address of this prefix.
*       prefixLen         - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr    - Points to the next hop pointer to set for this prefix.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the vrId was not created yet, or
*       GT_ALREADY_EXIST         - if prefix already exist when override is GT_FALSE, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPV6ADDR                                  ipAddr,
    IN GT_U32                                       prefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *nextHopInfoPtr,
    IN GT_BOOL                                      override
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv6PrefixArrayLen    - Length of UC prefix array.
*       ipv6PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully added
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if adding one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixBulkAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC     *lpmDbPtr,
    IN GT_U32                                   ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC      *ipv6PrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       deletes an existing Ipv6 prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router id.
*       ipAddr        - Points to the destination IP address of the prefix.
*       prefixLen     - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success, or
*       GT_OUT_OF_RANGE          - if prefix length is too big, or
*       GT_ERROR                 - if the vrId was not created yet, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NO_SUCH               - if the given prefix doesn't exitst in the VR, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       The default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixBulkDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       ipv6PrefixArrayLen    - Length of UC prefix array.
*       ipv6PrefixArrayPtr    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully deleted
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if deleting one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixBulkDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC     *lpmDbPtr,
    IN GT_U32                                   ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC      *ipv6PrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the unicast IPv6 Routing table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixSearch
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given ip-uc address, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - Points to the destination IP address to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr    - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found
*       GT_OUT_OF_RANGE          - if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixSearch
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPV6ADDR                                 ipAddr,
    IN  GT_U32                                      prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an IP-Unicast prefix with larger (ipAddrPtr,prefixLenPtr)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router Id to get the entry from.
*       ipAddrPtr         - The ip address to start the search from.
*       prefixLenPtr      - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr         - The ip address of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopInfoPtr    - the next hop pointer associated with the found
*                           ipAddr.
*
* RETURNS:
*       GT_OK                    - if the required entry was found
*       GT_OUT_OF_RANGE          - if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (ipAddrPtr,prefixLenPtr) must be a valid values, it
*           means that they exist in the IP-UC Table, unless this is the first
*           call to this function, then the value of (ipAddrPtr,prefixLenPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv6
*           UC prefix get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6UcPrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given ip address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       ipAddr            - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopInfoPtr    - The next hop pointer bounded to the longest match
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6UcPrefixGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPV6ADDR                                 ipAddr,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To add the multicast routing information for IP datagrams from
*       a particular source and addressed to a particular IP multicast
*       group address for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual private network identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*       mcRouteEntryPtr   - (pointer to) the mc Route pointer to set for the mc entry.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if prefix was not found when override is GT_TRUE, or
*       GT_ALREADY_EXIST         - if prefix already exist when override is GT_FALSE, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       1.  To override the default mc route use ipGroup = 0.
*       2.  If (S,G) MC group is added when (*,G) doesn't exists then implicit
*           (*,G) is added pointing to (*,*) default. Application added (*,G)
*           will override the implicit (*,G).
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryAdd
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC         *lpmDbPtr,
    IN GT_U32                                       vrId,
    IN GT_IPV6ADDR                                  ipGroup,
    IN GT_U32                                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                                  ipSrc,
    IN GT_U32                                       ipSrcPrefixLen,
    IN PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC    *mcRouteEntryPtr,
    IN GT_BOOL                                      override
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router identifier.
*       ipGroup           - The IP multicast group address.
*       ipGroupPrefixLen  - The number of bits that are actual valid in,
*                           the ipGroup.
*       ipSrc             - the root address for source base multi tree protocol.
*       ipSrcPrefixLen    - The number of bits that are actual valid in,
*                           the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device, or
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefixLen = 0.
*       2.  If no (*,G) was added but (S,G) were added, then implicit (*,G)
*           that points to (*,*) is added. If (*,G) is added later, it will
*           replace the implicit (*,G).
*           When deleting (*,G), then if there are still (S,G), an implicit (*,G)
*           pointing to (*,*) will be added.
*           When deleting last (S,G) and the (*,G) was implicitly added, then
*           the (*,G) will be deleted as well.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryDel
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipGroup,
    IN GT_U32                               ipGroupPrefixLen,
    IN GT_IPV6ADDR                          ipSrc,
    IN GT_U32                               ipSrcPrefixLen
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntriesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       flushes the multicast IP Routing table and stays with the default entry
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntriesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *lpmDbPtr,
    IN GT_U32                               vrId
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the muticast (ipSrc,ipGroup) entry, used
*       to find specific multicast adrress entry
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router Id.
*       ipGroup           - The ip Group address to get the next entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the next entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteEntryPtr   - (pointer to) the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_IPV6ADDR                                 ipGroup,
    IN  GT_U32                                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                                 ipSrc,
    IN  GT_U32                                      ipSrcPrefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *mcRouteEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamIpv6McEntryGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns the next muticast (ipSrcPtr,ipGroupPtr) entry, used
*       to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr            - The LPM DB
*       vrId                - The virtual router Id.
*       ipGroupPtr          - (pointer to) The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - (pointer to) the ipGroupPtr prefix length.
*       ipSrcPtr            - (pointer to) The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - (pointer to) ipSrcPtr prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - (pointer to) The next ip Group address.
*       ipGroupPrefixLenPtr - (pointer to) the ipGroupPtr prefix length.
*       ipSrcPtr            - (pointer to) The next ip Source address.
*       ipSrcPrefixLenPtr   - (pointer to) ipSrcPtr prefix length.
*       mcRouteEntryPtr     - (pointer to) the mc route entry ptr of the found mc mc route
*
* RETURNS:
*       GT_OK                    - if found, or
*       GT_OUT_OF_RANGE         -  if prefix length is too big, or
*       GT_BAD_PTR               - if NULL pointer, or
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table, or
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  The values of (ipSrcPtr,ipGroupPtr) must be a valid values, it
*           means that they exist in the IP-Mc Table, unless this is the first
*           call to this function, then the value of (ipSrcPtr,ipGroupPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv6
*           MC get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamIpv6McEntryGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipGroupPtr,
    INOUT GT_U32                                    *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                               *ipSrcPtr,
    INOUT GT_U32                                    *ipSrcPrefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *mcRouteEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function adds a new FCoE prefix to a Virtual Router in a
*       specific LPM DB or overrides an existing existing FCoE prefix.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       fcid              - The FC_ID of this prefix.
*       prefixLen         - The number of bits that are actual valid in the FC_ID.
*       nextHopInfoPtr    - (points to) The next hop pointer to set for this prefix.
*       override          - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_ALREADY_EXIST         - prefix already exist when override is GT_FALSE
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate PP memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixAdd
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_FCID                                     fcid,
    IN  GT_U32                                      prefixLen,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr,
    IN  GT_BOOL                                     override
);

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Deletes an existing FCoE prefix in a Virtual Router for the specified
*       LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr        - The LPM DB.
*       vrId            - The virtual router id.
*       fcid            - The destination FC_ID address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the FC_ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixDel
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC    *lpmDbPtr,
    IN  GT_U32                                  vrId,
    IN  GT_FCID                                 fcid,
    IN  GT_U32                                  prefixLen
);

#if 0
/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixBulkAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Creates a new or override an existing bulk of FCoE prefixes in a Virtual
*       Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       fcidPrefixArrayLen    - Length of FC_ID prefix array.
*       fcidPrefixArrayPtr    - The FC_ID prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully added
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - if adding one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixBulkAdd
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC            *lpmDbPtr,
    IN  GT_U32                                          fcidPrefixArrayLen,
    IN  CPSS_DXCH_FCOE_LPM_PREFIX_BULK_OPERATION_STC    *fcidPrefixArrayPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixBulkDel
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*       Deletes an existing bulk of FCoE prefixes in a Virtual Router for the
*       specified LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       fcidPrefixArrayLen    - Length of FC_ID prefix array.
*       fcidPrefixArrayPtr    - The FC_ID prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if all prefixes were successfully deleted
*       GT_OUT_OF_RANGE         -  if prefix length is too big
*       GT_BAD_PTR               - if NULL pointer
*       GT_FAIL                  - if deleting one or more prefixes failed; the
*                                  array will contain return status for each prefix
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixBulkDel
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC            *lpmDbPtr,
    IN  GT_U32                                          fcidPrefixArrayLen,
    IN  CPSS_DXCH_FCOE_LPM_PREFIX_BULK_OPERATION_STC    *fcidPrefixArrayPtr
);
#endif

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixesFlush
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       Flushes the FCoE forwarding table and stays with the default prefix
*       only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr      - The LPM DB
*       vrId          - The virtual router identifier.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixesFlush
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN GT_U32                                vrId
);

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixSearch
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function searches for a given FC_ID, and returns the next
*       hop pointer information bound to it if found.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       fcid              - The FC_ID to look for.
*       prefixLen         - The number of bits that are actual valid in the
*                           FC_ID.
*
* OUTPUTS:
*       nextHopInfoPtr    - If found this is The next hop pointer to for this
*                           prefix.
*
* RETURNS:
*       GT_OK                    - the required entry was found, or
*       GT_OUT_OF_RANGE          - the prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixSearch
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_FCID                                     fcid,
    IN  GT_U32                                      prefixLen,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixGetNext
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function returns an FCoE prefix with larger (FC_ID,prefix)
*       than the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB.
*       vrId              - The virtual router Id to get the entry from.
*       fcidPtr           - The FC_ID to start the search from.
*       prefixLenPtr      - The number of bits that are actual valid in the
*                           FC_ID.
*
* OUTPUTS:
*       fcidPtr           - The FC_ID of the found entry.
*       prefixLenPtr      - The prefix length of the found entry.
*       nextHopInfoPtr    - the next hop pointer associated with the found FC_ID
*
* RETURNS:
*       GT_OK                    - the required entry was found, or
*       GT_OUT_OF_RANGE          - the prefix length is too big, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - no more entries are left in the FC_ID table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  The values of (FC_ID,prefix) must be a valid values, it
*           means that they exist in the forwarding Table, unless this is the
*           first call to this function, then the value of (FC_ID,prefix) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the FC_ID
*           prefix get function.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixGetNext
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr,
    IN    GT_U32                                    vrId,
    INOUT GT_FCID                                   *fcidPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamFcoePrefixGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*       This function gets a given FC_ID address, and returns the next
*       hop pointer information bounded to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr          - The LPM DB
*       vrId              - The virtual router id.
*       fcid              - The FC_ID to look for.
*
* OUTPUTS:
*       prefixLenPtr      - Points to the number of bits that are actual valid
*                           in the longest match
*       nextHopInfoPtr    - The next hop pointer bound to the longest match
*
* RETURNS:
*       GT_OK                    - if the required entry was found, or
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_FOUND             - if the given prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamFcoePrefixGet
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC        *lpmDbPtr,
    IN  GT_U32                                      vrId,
    IN  GT_FCID                                     fcid,
    OUT GT_U32                                      *prefixLenPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *nextHopInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmRamUpdateVrTableFuncWrapper
*
* DESCRIPTION:
*       This function is a wrapper to PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PTR
*
* INPUTS:
*       data - the parmeters for PRV_CPSS_DXCH_LPM_RAM_UPDATE_VRF_TABLE_FUNC_PTR
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamUpdateVrTableFuncWrapper
(
    IN  GT_PTR                  data
);

#if 0
/*******************************************************************************
* ipLpmDbGetL3
*
* DESCRIPTION:
*       Retrives a specifc shadow's ip Table memory Size needed and info
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       iterationSizePtr      - points to data size in bytes supposed to be processed
*                               in current iteration
*       iterPtr               - points to the iterator, to start - set to 0.
*
* OUTPUTS:
*       tableSizePtr          - points to the table size calculated (in bytes).
*       tablePtr              - points to the table size info block.
*       iterationSizePtr      - points to data size in bytes left after iteration .
*       iterPtr               - points to the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - on bad iterPtr
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS ipLpmDbGetL3
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    OUT   GT_U32                                *tableSizePtr,
    OUT   GT_VOID                               *tablePtr,
    INOUT GT_U32                                *iterationSizePtr,
    INOUT GT_UINTPTR                            *iterPtr
);

/*******************************************************************************
* ipLpmDbSetL3
*
* DESCRIPTION:
*       Set the data needed for core IP shadow reconstruction used after HSU.
*
* INPUTS:
*       lpmDbPtr              - The LPM DB
*       tablePtr              - points to the table size info block
*       iterationSizePtr      - points to data size in bytes supposed to be processed
*                               in current iteration
*       iterPtr               - points to the iterator, to start - set to 0.
*
* OUTPUTS:
*       iterationSizePtr      - points to data size in bytes left after iteration step.
*       iterPtr               - points to the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                 - on success
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS ipLpmDbSetL3
(
    IN    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC  *lpmDbPtr,
    IN    GT_VOID                               *tablePtr,
    INOUT GT_U32                                *iterationSizePtr,
    INOUT GT_UINTPTR                            *iterPtr
);

/*******************************************************************************
* prvCpssExMxPmLpmDbIdGetNext
*
* DESCRIPTION:
*       This function retrieve next LPM DB ID from LPM DBs Skip List
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       slIteratorPtr - The iterator Id that was returned from the last call to
*                       this function.
* OUTPUTS:
*       lpmDbId       - retrieved LPM DB ID
*       slIteratorPtr - The iteration Id to be sent in the next call to this
*                       function to get the next data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_NO_MORE               - on absence of elements in skip list
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssExMxPmLpmDbIdGetNext
(
    OUT     GT_U32        *lpmDbIdPtr,
    INOUT   GT_UINTPTR    *slIteratorPtr
);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmRamh */

