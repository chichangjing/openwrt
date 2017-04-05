/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChLpm.h
*
* DESCRIPTION:
*       LPM APIs
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __cpssDxChLpmh
#define __cpssDxChLpmh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/lpm/cpssDxChLpmTypes.h>

/*
 * typedef: enum CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT
 *
 * Description: LPM Exception Status
 *
 * Enumerations:
 *  CPSS_DXCH_LPM_EXCEPTION_HIT_E      - Hit: No exceptions
 *  CPSS_DXCH_LPM_EXCEPTION_LPM_ECC_E  - An non recoverable
 *                                ECC error was found in the LPM database
 *  CPSS_DXCH_LPM_EXCEPTION_ECMP_ECC_E - An non recoverable ECC error was found
 *                                in the ECMP database and the leaf is from
 *                                ECMP/Qos type
 *  CPSS_DXCH_LPM_EXCEPTION_PBR_BUCKET_E    - Policy Base routing lookup and
 *                                the entry fetch from the memory is from type
 *                                Bucket (LPM<PointerType> = Bucket)
 *  CPSS_DXCH_LPM_EXCEPTION_CONTINUE_TO_LOOKUP_E  - Unicast lookup IPv4/IPv6/FCoE
 *                                that reach a bucket with <PointToLookup1> set
 *  CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_E   -  After lookup in LPM stage7
 *                                the received LPM<PointerType> is Bucket
 *                               (pass the next DIP LPM stages transparently)
 *  CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_E   - After lookup in LPM stage7
 *                               the received LPM<PointerType> is Bucket
 *                               (pass the next SIP LPM stages transparently)
 *  CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV4_PACKETS_E   - After lookup in LPM stage7
 *                               the received LPM<PointerType> is Bucket and
 *                               <PointToSIP> is unset (pass the next DIP LPM
 *                               stages transparently)
 *  CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV4_PACKETS_E  - After lookup in LPM stage7
 *                               the received LPM<PointerType> is Bucket
 *                               (pass the next SIP LPM stages transparently)
 *  CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_IPV6_PACKETS_E -  After lookup in
 *                              LPM stage31 the received LPM<PointerType>
 *  CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_IPV6_PACKETS_E -  After lookup in
 *                              LPM stage31 the received LPM<PointerType> is Bucket
 *  CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV6_PACKETS_E  - After lookup in LPM stage31
 *                              the received LPM<PointerType> is Bucket and
 *                              <PointToSIP> is unset
 *  CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV6_PACKETS_E  - After lookup in LPM stage31
 *                              the received LPM<PointerType> is Bucket
 *  CPSS_DXCH_LPM_EXCEPTION_FCOE_D_ID_LOOKUP_E      - After lookup in LPM stage5
 *                              the received LPM<PointerType> is Bucket
 *                              (pass the next DIP LPM stages transparently)
 *  CPSS_DXCH_LPM_EXCEPTION_FCOE_S_ID_LOOKUP_E - After lookup in LPM stage5 the
 *                              received LPM<PointerType> is Bucket
 *                              (pass the next SIP LPM stages transparently)
 */
typedef enum
{
    CPSS_DXCH_LPM_EXCEPTION_HIT_E                           = 0,
    CPSS_DXCH_LPM_EXCEPTION_LPM_ECC_E                       = 1,
    CPSS_DXCH_LPM_EXCEPTION_ECMP_ECC_E                      = 2,
    CPSS_DXCH_LPM_EXCEPTION_PBR_BUCKET_E                    = 3,
    CPSS_DXCH_LPM_EXCEPTION_CONTINUE_TO_LOOKUP_E            = 4,
    CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_E              = 5,
    CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_E              = 6,
    CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV4_PACKETS_E            = 7,
    CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV4_PACKETS_E            = 8,
    CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_0_IPV6_PACKETS_E = 9,
    CPSS_DXCH_LPM_EXCEPTION_UNICAST_LOOKUP_1_IPV6_PACKETS_E = 10,
    CPSS_DXCH_LPM_EXCEPTION_DST_G_IPV6_PACKETS_E            = 11,
    CPSS_DXCH_LPM_EXCEPTION_SRC_G_IPV6_PACKETS_E            = 12,
    CPSS_DXCH_LPM_EXCEPTION_FCOE_D_ID_LOOKUP_E              = 13,
    CPSS_DXCH_LPM_EXCEPTION_FCOE_S_ID_LOOKUP_E              = 14
}CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT;

/*******************************************************************************
* cpssDxChLpmNodeWrite
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API modes
*       Write an LPM Node (Bucket) to the HW.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       lpmLineOffset       - offset from the LPM base in LPM lines
*       nodeType            - type of node (bucket)
*       rangeSelectSecPtr   - the range selection (bitmap) section
*       numOfRanges         - number of next pointers in nextPointerArrayPtr
*       nextPointerArrayPtr - next pointers array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmNodeWrite
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   lpmLineOffset,
    IN CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT      nodeType,
    IN CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT   *rangeSelectSecPtr,
    IN GT_U32                                   numOfRanges,
    IN CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC      *nextPointerArrayPtr
);

/*******************************************************************************
* cpssDxChLpmNodeRead
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API modes
*       Read an LPM Node (Bucket) from the HW.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       lpmLineOffset       - offset from the LPM base in LPM lines
*       nodeType            - type of node (bucket)
*
* OUTPUTS:
*       rangeSelectSecPtr   - the range selection (bitmap) section
*       numOfRangesPtr      - number of next pointers in nextPointerArrayPtr
*       nextPointerArrayPtr - next pointers array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmNodeRead
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  lpmLineOffset,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT     nodeType,
    OUT CPSS_DXCH_LPM_NODE_RANGE_SELECTION_UNT  *rangeSelectSecPtr,
    OUT GT_U32                                  *numOfRangesPtr,
    OUT CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC     *nextPointerArrayPtr
);

/*******************************************************************************
* cpssDxChLpmNodeNextPointersWrite
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API modes
*       Write an LPM Node's (Bucket) Next Pointer entry (range) to the HW.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       lpmLineOffset       - offset from the LPM base in LPM lines
*       numOfNextPointers   - number of next pointers in nextPointerArrayPtr
*       nextPointerArrayPtr - next pointers array
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmNodeNextPointersWrite
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   lpmLineOffset,
    IN GT_U32                                   numOfNextPointers,
    IN CPSS_DXCH_LPM_NODE_NEXT_POINTER_STC      *nextPointerArrayPtr
);

/*******************************************************************************
* cpssDxChLpmLastLookupStagesBypassEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API modes
*       Set the enabling status of bypassing lookup stages 8-31.
*       bypassing the last lookup stages is recommended when IPv6 routing is not
*       used in the system, to decrease the latency caused by the LPM engine.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*       bypassEnabled       - the bypass enabling status:
*                             GT_TRUE  - enable bypassing of lookup stages 8-31
*                             GT_FALSE - disable bypassing of lookup stages 8-31
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmLastLookupStagesBypassEnableSet
(
    IN GT_U8                                    devNum,
    IN GT_BOOL                                  bypassEnabled
);

/*******************************************************************************
* cpssDxChLpmLastLookupStagesBypassEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : Low Level API modes
*       Get the enabling status of bypassing lookup stages 8-31.
*       bypassing the last lookup stages is recommended when IPv6 routing is not
*       used in the system, to decrease the latency caused by the LPM engine.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum              - the device number
*
* OUTPUTS:
*       bypassEnabledPtr    - (pointer to) the bypass enabling status:
*                             GT_TRUE  - enable bypassing of lookup stages 8-31
*                             GT_FALSE - disable bypassing of lookup stages 8-31
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmLastLookupStagesBypassEnableGet
(
    IN  GT_U8                                   devNum,
    OUT GT_BOOL                                 *bypassEnabledPtr
);



/*******************************************************************************
* cpssDxChLpmPortSipLookupEnableSet
*
* DESCRIPTION:
*       Enable/Disable SIP lookup for packets received from the given port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number
*       enable    - GT_TRUE:   enable SIP Lookup on the port
*                   GT_FALSE:  disable SIP Lookup on the port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on out of range
*       GT_BAD_STATE             - on invalid hardware value set
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When this attribute is set to "False", SIP Lookup is never performed for
*       the source ePort, regardless of other SIP related configurations
*       (i.e SIP/SA check, SIP filtering etc')
*
*******************************************************************************/
GT_STATUS cpssDxChLpmPortSipLookupEnableSet
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    IN  GT_BOOL                      enable
);

/*******************************************************************************
* cpssDxChLpmPortSipLookupEnableGet
*
* DESCRIPTION:
*       Return the SIP Lookup status for packets received from the given port.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number
*
* OUTPUTS:
*       enablePtr - (pointer to) GT_TRUE:   enable SIP Lookup on the port
*                                GT_FALSE:  disable SIP Lookup on the port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - on out of range
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       When this attribute is set to "False", SIP Lookup is never performed for
*       the source ePort, regardless of other SIP related configurations
*       (i.e SIP/SA check, SIP filtering etc')
*
*******************************************************************************/
GT_STATUS cpssDxChLpmPortSipLookupEnableGet
(
    IN  GT_U8                        devNum,
    IN  GT_PORT_NUM                  portNum,
    OUT GT_BOOL                      *enablePtr
);

/*******************************************************************************
* cpssDxChLpmLeafEntryWrite
*
* DESCRIPTION:
*       Write an LPM leaf entry, which is used for policy based routing, to the
*       HW
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum                   - the device number
*       leafIndex                - leaf index within the LPM PBR block
*       leafPtr                  - (pointer to) the leaf entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong input parameter
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       The PBR base address is set in cpssDxChCfgPpLogicalInit according to the
*       maxNumOfPbrEntries parameter in CPSS_DXCH_PP_CONFIG_INIT_STC.
*       See the datasheet for maximal leafIndex.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmLeafEntryWrite
(
    IN GT_U8                                    devNum,
    IN GT_U32                                   leafIndex,
    IN CPSS_DXCH_LPM_LEAF_ENTRY_STC             *leafPtr
);

/*******************************************************************************
* cpssDxChLpmLeafEntryRead
*
* DESCRIPTION:
*       Read an LPM leaf entry, which is used for policy based routing, from the
*       HW
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum                      - the device number
*       leafIndex                   - leaf index within the LPM PBR block
*
* OUTPUTS:
*       leafPtr                     - (pointer to) the leaf entry
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong input parameter
*       GT_BAD_PTR                  - on null pointer
*       GT_HW_ERROR                 - on hardware error
*       GT_FAIL                     - on error
*       GT_NOT_APPLICABLE_DEVICE    - on non applicable device
*
* COMMENTS:
*       The PBR base address is set in cpssDxChCfgPpLogicalInit according to the
*       maxNumOfPbrEntries parameter in CPSS_DXCH_PP_CONFIG_INIT_STC.
*       See the datasheet for maximal leafIndex.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmLeafEntryRead
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  leafIndex,
    OUT CPSS_DXCH_LPM_LEAF_ENTRY_STC            *leafPtr
);

/*******************************************************************************
* cpssDxChLpmPbrBaseAndSizeGet
*
* DESCRIPTION:
*       Return the base address and size of the policy based routing area within
*       the LPM memory
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum             - the device number
*
* OUTPUTS:
*       baseOffsetPtr      - (pointer to) PBR base offset within the LPM memory
*       sizePtr            - (pointer to) number of PBR leaf entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmPbrBaseAndSizeGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *baseOffsetPtr,
    OUT GT_U32  *sizePtr
);

/*******************************************************************************
* cpssDxChLpmExceptionStatusGet
*
* DESCRIPTION:
*       Return the Lpm Exception Status
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2.
*
* INPUTS:
*       devNum             - the device number
*
* OUTPUTS:
*       hitException       - hit Exception happend in LPM Stages
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters with bad value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on non applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChLpmExceptionStatusGet
(
    IN  GT_U8                               devNum,
    OUT CPSS_DXCH_LPM_EXCEPTION_STATUS_ENT  *hitException
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChLpmh */

