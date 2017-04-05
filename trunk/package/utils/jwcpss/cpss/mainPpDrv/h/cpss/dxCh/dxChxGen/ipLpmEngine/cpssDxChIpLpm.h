/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChIpLpm.h
*
* DESCRIPTION:
*       the CPSS DXCH LPM Hierarchy manager .
*
* FILE REVISION NUMBER:
*       $Revision: 43 $
*******************************************************************************/

#ifndef __cpssDxChIpLpmh
#define __cpssDxChIpLpmh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>


/*******************************************************************************
* cpssDxChIpLpmDBCreate
*
* DESCRIPTION:
*       This function creates an LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmDBId               - the LPM DB id
*       shadowType            - the type of shadow to maintain
*       protocolStack         - the type of protocol stack this LPM DB support
*       memoryCfgPtr          - (pointer to)the memory configuration for this LPM DB
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_SUPPORTED         - request is not supported if partitioning
*                                  is disabled.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCreate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT             shadowType,
    IN CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack,
    IN CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT           *memoryCfgPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBDelete
*
* DESCRIPTION:
*   This function deletes LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId                   - the LPM DB id (range 32 bits)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - the LPM DB is not empty
*       GT_FAIL                   - on error
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDelete
(
    IN  GT_U32      lpmDbId
);

/*******************************************************************************
* cpssDxChIpLpmDBConfigGet
*
* DESCRIPTION:
*       This function retrieves configuration of the LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId                       - the LPM DB id.
*
* OUTPUTS:
*       shadowTypePtr         - (pointer to)the type of shadow to maintain
*       protocolStackPtr      - (pointer to)the type of protocol stack this LPM DB support
*       memoryCfgPtr          - (pointer to)the memory configuration for this LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PTR               - NULL pointer.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBConfigGet
(
    IN  GT_U32                             lpmDBId,
    OUT CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT   *shadowTypePtr,
    OUT CPSS_IP_PROTOCOL_STACK_ENT         *protocolStackPtr,
    OUT CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT *memoryCfgPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*       indexesRangePtr             - (pointer to)the range of TCAM indexes availble 
*                                     for this LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - (pointer to)the new capacity configuration. 
*                                     When partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       This API is relevant only when using TCAM Manager. This API is used for
*       updating only the capacity configuration of the LPM. for updating the
*       lines reservation for the TCAM Manger use cpssDxChTcamManagerRangeUpdate.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCapacityUpdate
(
    IN GT_U32                                       lpmDBId,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC *indexesRangePtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC *tcamLpmManagerCapcityCfgPtr
);


/*******************************************************************************
* cpssDxChIpLpmDBCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*
* OUTPUTS:
*       indexesRangePtr             - (pointer to) the range of TCAM indexes
*                                     available for this LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - (pointer to) the partitioning enabling status
*                                     GT_TRUE: the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - (pointer to) the current capacity
*                                     configuration. when partitionEnable in
*                                     cpssDxChIpLpmDBCreate was set to GT_TRUE
*                                     this means current prefixes partition,
*                                     when this was set to GT_FALSE this means
*                                     that the current guaranteed prefixes
*                                     allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCapacityGet
(
    IN  GT_U32                                          lpmDBId,
    OUT GT_BOOL                                         *partitionEnablePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    *indexesRangePtr,
    OUT CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    *tcamLpmManagerCapcityCfgPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBCheetah2VrSupportCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment in Cheetah2
*   devices with support for virtual router.
*
* APPLICABLE DEVICES:
*        DxCh2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId          - the LPM DB id.
*       protocolStack    - the type of protocol stack this LPM DB support.
*       pclTcamCfgPtr    - (pointer to) the pcl tcam configuration
*       routerTcamCfgPtr - (pointer to) the router tcam configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_ALREADY_EXIST         - if the LPM DB id is already used.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - NULL pointer.
*
* COMMENTS:
*       Since LTT rules are shared between PCL and Router Tcam, several checks
*       are made to supplied parameters to make sure that all prefixes could be
*       inserted.
*
*       Note that LTT entries corresponding to routerTcamCfgPtr->tcamRange
*       are allocated to Lpm Manager.
*
*       Furthermore:
*
*       routerTcamCfgPtr->tcamRange.lastIndex -
*       routerTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Router Tcam lines).
*
*       pclTcamCfgPtr->tcamRange.lastIndex -
*       pclTcamCfgPtr->tcamRange.firstIndex + 1 must be less than 1024
*       (number of Pcl Tcam lines).
*
*       (routerTcamCfgPtr->tcamRange.lastIndex -
*        routerTcamCfgPtr->tcamRange.firstIndex + 1) * 5
*           must be larger or equal to
*       pclTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       pclTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv4Prefixes +
*       routerTcamCfgPtr->prefixesCfg.numOfIpv6Prefixes
*
*       Multicast is not supported.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBCheetah2VrSupportCreate
(
    IN GT_U32                                                   lpmDBId,
    IN CPSS_IP_PROTOCOL_STACK_ENT                               protocolStack,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *pclTcamCfgPtr,
    IN CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC *routerTcamCfgPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB. this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevListAdd
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
);

/*******************************************************************************
* cpssDxChIpLpmDBDevsListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing LPM DB. this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devListArr     - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBDevsListRemove
(
    IN GT_U32   lpmDBId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
);

/*******************************************************************************
* cpssDxChIpLpmDBDevListGet
*
* DESCRIPTION:
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDbId        - the LPM DB id.
*       numOfDevsPtr   - (pointer to) the size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr   - (pointer to) the number of devices retreived
*       devListArray   - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to
*                                  the device list
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
GT_STATUS cpssDxChIpLpmDBDevListGet
(
    IN    GT_U32                        lpmDbId,
    INOUT GT_U32                        *numOfDevsPtr,
    OUT   GT_U8                         devListArray[]
);

/*******************************************************************************
* cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; xCat2; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmTcamLinesReserve
(
    IN  GT_U32                                          lpmDBId,
    IN  GT_U32                                          linesToReserveArray[],
    IN  GT_U32                                          numberOfLinesToReserve,
    IN  CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    allocateReserveLinesAs
);

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
*
* DESCRIPTION:
*   The function uses available TCAM rules at the beginning of the TCAM range
*   allocated to the LPM DB and place there "do nothing" rules that will make
*   sure that Multicast traffic is matched by these rules and not by the
*   Unicast default rule.
*   When policy based routing Unicast traffic coexists with IP based Multicast
*   (S,G,V) bridging (used in IGMP and MLD protocols), there is a need to add
*   default Multicast rules in order to make sure that the Multicast traffic
*   will not be matched by the Unicast default rule.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       pclIdArrayLen             - size of the PCL ID array (APPLICABLE RANGES: 1..1048575)
*       pclIdArray                - array of PCL ID that may be in used by the LPM DB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_BAD_PARAM              - if wrong value in any of the parameters
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_ALREADY_EXIST          - if default MC already set
*       GT_NOT_SUPPORTED          - if the LPM DB doesn't configured to
*                                   operate in policy based routing mode
*       GT_FULL                   - if TCAM is full
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - the LPM DB must be configured to use policy based routing
*       One default Multicast rule will be set for each protocol stack supported
*       by the LPM DB and for each PCL ID that may be used in the LPM DB (each
*       virtaul router uses unique PCL ID).
*       For each protocol stack, the Multicast default rules will use the
*       prefixes allocated for Unicast prefixes for this protocol. For example
*       if the application allocates 10 IPv4 Unicast prefixes, then after setting
*       one default Multicast prefix, only 9 IPv4 Unicast prefixes will be
*       available.
*       The default Multicast rules will capture all IPv4/IPv6 Multicast
*       traffic with the same assigned PCL ID. As a result all rules configured
*       to match IP Multicast traffic with same assigned PCL ID that reside
*       after the Multicast defaults will not be matched.
*       The default Multicast rules can not be deleted after set.
*       The default Multicast rules can be set no more than once.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
(
    IN  GT_U32  lpmDBId,
    IN  GT_U32  pclIdArrayLen,
    IN  GT_U32  pclIdArray[]
);

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
*
* DESCRIPTION:
*   The function gets whether the LPM DB is configured to support default MC
*   rules in policy based routing mode and the rule indexes and PCL ID of those
*   default rules.
*   Refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet for more details.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       lpmDBId                           - the LPM DB id.
*       defaultIpv4RuleIndexArrayLenPtr   - (pointer to) the allocated size of the array
*       defaultIpv6RuleIndexArrayLenPtr   - (pointer to) the allocated size of the array
*       pclIdArrayLenPtr                  - (pointer to) the allocated size of the array
*
* OUTPUTS:
*       defaultMcUsedPtr                  - (pointer to) whether default MC is used for this LPM DB
*       protocolStackPtr                  - (pointer to) protocol stack supported by this LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArray         - rule indexes of the default IPv4 MC (in case
*                                           the LPM DB support IPv4)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArrayLenPtr   - (pointer to) number of elements filled in the array
*       defaultIpv6RuleIndexArray         - rule indexes of the default IPv6 MC (in case
*                                           the LPM DB support IPv6)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv6RuleIndexArrayLenPtr   - (pointer to) number of elements filled in the array
*       pclIdArray                        - array of PCL ID that may be in used by the LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       pclIdArrayLenPtr                  - (pointer to) number of elements filled in the array
* 
* RETURNS:
*       GT_OK                             - on success
*       GT_NOT_FOUND                      - if the LPM DB id is not found
*       GT_BAD_PTR                        - if one of the parameters is NULL pointer.
*       GT_FULL                           - if any of the arrays is not big enough
*       GT_FAIL                           - on failure
*       GT_NOT_APPLICABLE_DEVICE          - on not applicable device
*
* COMMENTS:
*       The rule indexes are according to explanations in cpssDxChPclRuleSet.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
(
    IN      GT_U32                          lpmDBId,
    OUT     GT_BOOL                         *defaultMcUsedPtr,
    OUT     CPSS_IP_PROTOCOL_STACK_ENT      *protocolStackPtr,
    OUT     GT_U32                          defaultIpv4RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv4RuleIndexArrayLenPtr,
    OUT     GT_U32                          defaultIpv6RuleIndexArray[],
    INOUT   GT_U32                          *defaultIpv6RuleIndexArrayLenPtr,
    OUT     GT_U32                          pclIdArray[],
    INOUT   GT_U32                          *pclIdArrayLenPtr
);

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
);


/*******************************************************************************
* cpssDxChIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*   This function adds a shared virtual router in system for specific LPM DB.
*   Prefixes that reside within shared virtual router will participate in the
*   lookups of all virtual routers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  Only one shared virtual router can exists at any time within a
*           given LPM DB.
*       2.  Virtual router ID that is used for non-shared virtual router can't
*           be used for the shared virtual router and via versa.
*       3.  Shared prefixes can't overlap non-shared prefixes. If the shared
*           virtual router supports shared prefixes type then adding non-shared
*           virtual router that supports the same prefixes type will fail.
*           Also, if a non-shared virtual router that supports prefixes type
*           exists, then adding a shared virtual router that supports the same
*           prefixes type will fail.
*       4.  When the shared virtual router supports IPv4 UC prefixes, then the
*           the following will apply:
*             - The <match all> default TCAM entry will not be written to TCAM
*               (however TCAM entry will still be allocated to this entry)
*             - The following prefixes will be added to match all non-MC traffic:
*               0x00/1, 0x80/2, 0xC0/3, 0xF0/4
*           Same applies when the shared virtual router supports IPv6 UC
*           prefixes. The prefixes added to match all non-MC traffic are:
*           0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8
*       5.  The application should not delete the non-MC prefixes (when added).
*       6.  Manipulation of the default UC will not be applied on the non-MC
*           prefixes. The application should manipulate those non-MC entries
*           directly when needed.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterSharedAdd
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC        *vrConfigPtr
);

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       vrConfigPtr         - (pointer to) Virtual router configuration.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC          *vrConfigPtr
);

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmVirtualRouterDel
(
    IN GT_U32                                 lpmDBId,
    IN GT_U32                                 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - (pointer to) the route entry info accosiated with this
*                         UC prefix.
*       override        - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPADDR                                ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv4PrefixArrayLen - length of UC prefix array
*       ipv4PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
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
*       GT_NO_SUCH - If the given prefix doesn't exist in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPADDR                            ipAddr,
    IN GT_U32                               prefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv4 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv4PrefixArrayLen - length of UC prefix array
*       ipv4PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of the prefixes' lengths is too big
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv4PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC  *ipv4PrefixArrayPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
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
GT_STATUS cpssDxChIpLpmIpv4UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - (pointer to) the route entry info associated with
*                              this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
* 
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
);



/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - (pointer to) the ip address of the found entry
*       prefixLenPtr         - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr       - (pointer to) the route entry info associated
*                              with this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this uc prefix
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this uc
*                              prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPADDR                                 *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr

);

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr         - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr       - (pointer to) the route entry info associated with
*                              this UC prefix, if found.
*       tcamRowIndexPtr      - (pointer to) the TCAM row index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - (pointer to) the TCAM column index of this UC
*                              prefix, if found.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4UcPrefixGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPADDR                               ipAddr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefixLen      - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - (pointer to) the LTT entry pointing to the MC
*                             route entry associated with this MC route.
*       override            - whether to override an mc Route pointer for the
*                             given prefix
*       tcamDefragmentationEnable - whether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*                             (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                              DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - if one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPADDR                    ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPADDR                    ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      tcamDefragmentationEnable
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPADDR    ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPADDR    ipSrc,
    IN GT_U32       ipSrcPrefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
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
GT_STATUS cpssDxChIpLpmIpv4McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - the virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - (pointer to) the ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - (pointer to) the ipGroup prefix Length.
*       ipSrcPtr            - (pointer to) the ip Source address to get the next
*                             entry for.
*       ipSrcPrefixLenPtr   - (pointer to) ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - (pointer to) the next ip Group address
*       ipGroupPrefixLenPtr        - (pointer to) the ipGroup prefix Length
*       ipSrcPtr                   - (pointer to) the next ip Source address
*       ipSrcPrefixLenPtr          - (pointer to) ipSrc prefix length
*       mcRouteLttEntryPtr         - (pointer to) the LTT entry pointer pointing
*                                    to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - (pointer to) TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - (pointer to) TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - (pointer to) TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - (pointer to) TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The value of ipGroupPtr must be a valid value, it
*       means that it exists in the IP-Mc Table, unless this is the first
*       call to this function, then it's value is 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntryGetNext
(
    IN    GT_U32                        lpmDBId,
    IN    GT_U32                        vrId,
    INOUT GT_IPADDR                     *ipGroupPtr,
    INOUT GT_U32                        *ipGroupPrefixLenPtr,
    INOUT GT_IPADDR                     *ipSrcPtr,
    INOUT GT_U32                        *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC    *mcRouteLttEntryPtr,
    OUT GT_U32                          *tcamGroupRowIndexPtr,
    OUT GT_U32                          *tcamGroupColumnIndexPtr,
    OUT GT_U32                          *tcamSrcRowIndexPtr,
    OUT GT_U32                          *tcamSrcColumnIndexPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the multicast (ipSrc, ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc + ipGroup
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - (pointer to) the LTT entry pointer pointing
*                                    to the MC route entry associated with this
*                                    MC route.
*       tcamGroupRowIndexPtr       - (pointer to) TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - (pointer to) TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - (pointer to) TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - (pointer to) TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPADDR                   ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPADDR                   ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamGroupColumnIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr,
    OUT GT_U32                      *tcamSrcColumnIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - (pointer to) the route entry info accosiated with this
*                         UC prefix.
*       override        - override the existing entry if it already exists
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAdd
(
    IN GT_U32                                   lpmDBId,
    IN GT_U32                                   vrId,
    IN GT_IPV6ADDR                              ipAddr,
    IN GT_U32                                   prefixLen,
    IN CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *nextHopInfoPtr,
    IN GT_BOOL                                  override,
    IN GT_BOOL                                  tcamDefragmentationEnable
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv6PrefixArrayLen - length of UC prefix array
*       ipv6PrefixArrayPtr - (pointer to) the UC prefix array
*       tcamDefragmentationEnable - whether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixAddBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr,
    IN GT_BOOL                              tcamDefragmentationEnable
);


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
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
*       GT_NO_SUCH               - If the given prefix doesn't exist in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDel
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               vrId,
    IN GT_IPV6ADDR                          ipAddr,
    IN GT_U32                               prefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDelBulk
*
* DESCRIPTION:
*   Deletes an existing bulk of Ipv6 prefixes in a Virtual Router for the
*   specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - the LPM DB id
*       ipv6PrefixArrayLen - length of UC prefix array
*       ipv6PrefixArrayPtr - (pointer to) the UC prefix array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big, or
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixDelBulk
(
    IN GT_U32                               lpmDBId,
    IN GT_U32                               ipv6PrefixArrayLen,
    IN CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC  *ipv6PrefixArrayPtr
);


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
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
GT_STATUS cpssDxChIpLpmIpv6UcPrefixesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found.
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix, if
*                         found
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixSearch
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    IN  GT_U32                                  prefixLen,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - (pointer to) the ip address of the found entry
*       prefixLenPtr    - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found.
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix. if
*                         found.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGetNext
(
    IN    GT_U32                                    lpmDBId,
    IN    GT_U32                                    vrId,
    INOUT GT_IPV6ADDR                               *ipAddrPtr,
    INOUT GT_U32                                    *prefixLenPtr,
    OUT   CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    *nextHopInfoPtr,
    OUT GT_U32                                      *tcamRowIndexPtr,
    OUT GT_U32                                      *tcamColumnIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr    - (pointer to) the prefix length of the found entry
*       nextHopInfoPtr  - (pointer to) the route entry info associated with
*                         this UC prefix, if found
*       tcamRowIndexPtr - (pointer to) the TCAM row index of this uc prefix, if
*                         found
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - (pointer to) the TCAM column index of this uc
*                            prefix, if found
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6UcPrefixGet
(
    IN  GT_U32                                  lpmDBId,
    IN  GT_U32                                  vrId,
    IN  GT_IPV6ADDR                             ipAddr,
    OUT GT_U32                                  *prefixLenPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  *nextHopInfoPtr,
    OUT GT_U32                                  *tcamRowIndexPtr,
    OUT GT_U32                                  *tcamColumnIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - (pointer to) the LTT entry pointing to the MC route
*                            entry associated with this MC route.
*       override        - whether to override the mcRoutePointerPtr for the
*                         given prefix
*       tcamDefragmentationEnable - whether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryAdd
(
    IN GT_U32                       lpmDBId,
    IN GT_U32                       vrId,
    IN GT_IPV6ADDR                  ipGroup,
    IN GT_U32                       ipGroupPrefixLen,
    IN GT_IPV6ADDR                  ipSrc,
    IN GT_U32                       ipSrcPrefixLen,
    IN CPSS_DXCH_IP_LTT_ENTRY_STC   *mcRouteLttEntryPtr,
    IN GT_BOOL                      override,
    IN GT_BOOL                      tcamDefragmentationEnable
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryDel
(
    IN GT_U32       lpmDBId,
    IN GT_U32       vrId,
    IN GT_IPV6ADDR  ipGroup,
    IN GT_U32       ipGroupPrefixLen,
    IN GT_IPV6ADDR  ipSrc,
    IN GT_U32       ipSrcPrefixLen
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
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
GT_STATUS cpssDxChIpLpmIpv6McEntriesFlush
(
    IN GT_U32 lpmDBId,
    IN GT_U32 vrId
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - the LPM DB id
*       vrId                - the virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - (pointer to) the ip Group address to get the next
*                             entry for
*       ipGroupPrefixLenPtr - (pointer to) the ipGroup prefix length
*       ipSrcPtr            - (pointer to) the ip Source address to get the next entry for
*       ipSrcPrefixLenPtr   - (pointer to) the ipSrc prefix length
*
* OUTPUTS:
*       ipGroupPtr           - (pointer to) the next ip Group address.
*       ipGroupPrefixLenPtr  - (pointer to) ipGroup prefix length.
*       ipSrcPtr             - (pointer to) the next ip Source address.
*       ipSrcPrefixLenPtr    - (pointer to) ipSrc prefix length.
*       mcRouteLttEntryPtr   - (pointer to) the LTT entry pointer pointing to
*                              the MC route entry associated with this MC route
*       tcamGroupRowIndexPtr - (pointer to) TCAM group row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - (pointer to) TCAM source row  index.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*       values, it means that they exist in the IP-Mc Table, unless this is
*       the first call to this function, then the value of (ipGroupPtr,
*       ipSrcPtr) is (0,0).
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntryGetNext
(
    IN    GT_U32                      lpmDBId,
    IN    GT_U32                      vrId,
    INOUT GT_IPV6ADDR                 *ipGroupPtr,
    INOUT GT_U32                      *ipGroupPrefixLenPtr,
    INOUT GT_IPV6ADDR                 *ipSrcPtr,
    INOUT GT_U32                      *ipSrcPrefixLenPtr,
    OUT   CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                        *tcamGroupRowIndexPtr,
    OUT GT_U32                        *tcamSrcRowIndexPtr
);

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the multicast (ipSrc, ipGroup) entry, used to find
*   specific multicast adrress entry, and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroup           - The ip Group address to get the entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLen    - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - (pointer to) the LTT entry pointer pointing to
*                              the MC route entry associated with this MC route
*       tcamGroupRowIndexPtr - (pointer to) TCAM group row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - (pointer to) TCAM source row  index
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmIpv6McEntrySearch
(
    IN  GT_U32                      lpmDBId,
    IN  GT_U32                      vrId,
    IN  GT_IPV6ADDR                 ipGroup,
    IN  GT_U32                      ipGroupPrefixLen,
    IN  GT_IPV6ADDR                 ipSrc,
    IN  GT_U32                      ipSrcPrefixLen,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC  *mcRouteLttEntryPtr,
    OUT GT_U32                      *tcamGroupRowIndexPtr,
    OUT GT_U32                      *tcamSrcRowIndexPtr
);



/*******************************************************************************
* cpssDxChIpLpmDBMemSizeGet
*
* DESCRIPTION:
*   This function gets the memory size needed to export the Lpm DB,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*
* OUTPUTS:
*   lpmDbSizePtr      - (pointer to) the table size calculated (in bytes)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBMemSizeGet
(
    IN    GT_U32     lpmDBId,
    OUT   GT_U32     *lpmDbSizePtr
);

/*******************************************************************************
* cpssDxChIpLpmDBExport
*
* DESCRIPTION:
*   This function exports the Lpm DB into the preallocated memory,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - (pointer to) block data size supposed to be
*                               exported.
*                               in current iteration.
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - (pointer to) block data size that was not used.
*                               in current iteration.
*       lpmDbMemBlockPtr      - (pointer to) allocated for lpm DB memory area.
*       iterPtr               - (pointer to) the iterator, if = 0 then the
*                               operation is done.
* 
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBExport
(
    IN    GT_U32     lpmDBId,
    OUT   GT_U32     *lpmDbMemBlockPtr,
    INOUT GT_U32     *lpmDbMemBlockSizePtr,
    INOUT GT_UINTPTR *iterPtr
);

/*******************************************************************************
* cpssDxChIpLpmDBImport
*
* DESCRIPTION:
*   This function imports the Lpm DB recived and reconstruct it,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - (pointer to) block data size supposed to be
*                               imported.
*                               in current iteration.
*       lpmDbMemBlockPtr      - (pointer to) allocated for lpm DB memory area.
*       iterPtr               - (pointer to) the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - (pointer to) block data size that was not used.
*                               in current iteration.
*       iterPtr               - (pointer to) the iterator, if = 0 then the
*                               operation is done.
* 
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS cpssDxChIpLpmDBImport
(
    IN    GT_U32     lpmDBId,
    IN    GT_U32     *lpmDbMemBlockPtr,
    INOUT GT_U32     *lpmDbMemBlockSizePtr,
    INOUT GT_UINTPTR *iterPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChIpLpmh */
