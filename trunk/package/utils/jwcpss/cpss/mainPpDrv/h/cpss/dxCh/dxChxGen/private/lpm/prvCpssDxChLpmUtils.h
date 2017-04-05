/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmUtils.c
*
* DESCRIPTION:
*       private LPM utility functions
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/
#ifndef __prvCpssDxChLpmUtilsh
#define __prvCpssDxChLpmUtilsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRam.h>

/*******************************************************************************
* prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap
*
* DESCRIPTION:
*       Convert IP protocol stack value to bitmap of protocols
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       protocolStack     - the IP protocol stack
*
* OUTPUTS:
*       protocolBitmapPtr - the protocol bitmap
*
* RETURNS:
*       None
*
* COMMENTS:
*       The function assumes that a validity check was done on protocolStack
*       before calling to this function.
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpProtocolStackToProtocolBitmap
(
    IN  CPSS_IP_PROTOCOL_STACK_ENT        protocolStack,
    OUT PRV_CPSS_DXCH_LPM_PROTOCOL_BMP    *protocolBitmapPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertProtocolBitmapToIpProtocolStack
*
* DESCRIPTION:
*       Convert bitmap of protocols to IP protocol stack value
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       protocolBitmap   - the protocol bitmap
*
* OUTPUTS:
*       protocolStackPtr - the IP protocol stack
*
* RETURNS:
*       None
*
* COMMENTS:
*       The function assumes that protocolBitmap holds at least one IP protocol
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertProtocolBitmapToIpProtocolStack
(
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_BMP    protocolBitmap,
    OUT CPSS_IP_PROTOCOL_STACK_ENT        *protocolStackPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertIpShadowTypeToLpmShadowType
*
* DESCRIPTION:
*       Convert enum of IP shadow type to LPM shadow type
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       ipShadowType     - the IP shadow type
*
* OUTPUTS:
*       lpmShadowTypePtr - the LPM shadow type
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmConvertIpShadowTypeToLpmShadowType
(
    IN  CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT    ipShadowType,
    OUT PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT   *lpmShadowTypePtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertLpmShadowTypeToIpShadowType
*
* DESCRIPTION:
*       Convert enum of LPM shadow type to IP shadow type
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmShadowType   - the LPM shadow type
*
* OUTPUTS:
*       ipShadowTypePtr - the IP shadow type
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmConvertLpmShadowTypeToIpShadowType
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT    lpmShadowType,
    OUT CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT     *ipShadowTypePtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry
*
* DESCRIPTION:
*       Convert IP LTT entry structure to private LPM route entry structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmHwType       - whether the device uses RAM for LPM
*       ipLttEntryPtr   - the IP LTT entry
*
* OUTPUTS:
*       routeEntryPtr   - the LPM route entry
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpLttEntryToLpmRouteEntry
(
    IN  PRV_CPSS_DXCH_LPM_HW_ENT                        lpmHwType,
    IN  CPSS_DXCH_IP_LTT_ENTRY_STC                      *ipLttEntryPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *routeEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry
*
* DESCRIPTION:
*       Convert private LPM route entry structure to IP LTT entry structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmHwType       - whether the device uses RAM for LPM
*       routeEntryPtr   - the LPM route entry
*
* OUTPUTS:
*       ipLttEntryPtr   - the IP LTT entry
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertLpmRouteEntryToIpLttEntry
(
    IN  PRV_CPSS_DXCH_LPM_HW_ENT                        lpmHwType,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC       *routeEntryPtr,
    OUT CPSS_DXCH_IP_LTT_ENTRY_STC                      *ipLttEntryPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertIpVrConfigToTcamVrConfig
*
* DESCRIPTION:
*       Convert IP VR config structure to private TCAM VR config structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3;.
*
* INPUTS:
*       shadowType      - the shadow type
*       ipVrConfigPtr      - the IP VR config structure
*
* OUTPUTS:
*       tcamVrConfigPtr    - the TCAM VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpVrConfigToTcamVrConfig
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr,
    OUT PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC     *tcamVrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertTcamVrConfigToIpVrConfig
*
* DESCRIPTION:
*       Convert private TCAM VR config structure to IP VR config structure
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3;.
*
* INPUTS:
*       shadowType      - the shadow type
*       tcamVrConfigPtr - the TCAM VR config structure
*
* OUTPUTS:
*       ipVrConfigPtr   - the IP VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertTcamVrConfigToIpVrConfig
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC     *tcamVrConfigPtr,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertIpVrConfigToRamVrConfig
*
* DESCRIPTION:
*       Convert IP VR config structure to private RAM VR config structure
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       ipVrConfigPtr      - the IP VR config structure
*
* OUTPUTS:
*       ramVrConfigPtr     - the RAM VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpVrConfigToRamVrConfig
(
    IN  CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr,
    OUT PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC      *ramVrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertRamVrConfigToIpVrConfig
*
* DESCRIPTION:
*       Convert private RAM VR config structure to IP VR config structure
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       ramVrConfigPtr  - the RAM VR config structure
*
* OUTPUTS:
*       ipVrConfigPtr   - the IP VR config structure
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertRamVrConfigToIpVrConfig
(
    IN  PRV_CPSS_DXCH_LPM_RAM_VR_CONFIG_STC      *ramVrConfigPtr,
    OUT CPSS_DXCH_IP_LPM_VR_CONFIG_STC           *ipVrConfigPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry
*
* DESCRIPTION:
*       Convert IP route entry union to private LPM route entry union
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3;.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       shadowType            - shadow type
*       ipRouteEntryInfoPtr   - IP route entry union
*
* OUTPUTS:
*       lpmRouteEntryInfoPtr  - private LPM route entry union
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertIpRouteEntryToLpmRouteEntry
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *ipRouteEntryInfoPtr,
    OUT PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT   *lpmRouteEntryInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry
*
* DESCRIPTION:
*       Convert private LPM route entry union to IP route entry union
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3;.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       shadowType           - shadow type
*       lpmRouteEntryInfoPtr - private LPM route entry union
*
* OUTPUTS:
*       ipRouteEntryInfoPtr  - IP route entry union
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID prvCpssDxChLpmConvertLpmRouteEntryToIpRouteEntry
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT        shadowType,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT   *lpmRouteEntryInfoPtr,
    OUT CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   *ipRouteEntryInfoPtr
);

/*******************************************************************************
* prvCpssDxChLpmGetHwType
*
* DESCRIPTION:
*       Convert LPM shadow type to HW type.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3;.
*
* NOT APPLICABLE DEVICES:
*       None
*
* INPUTS:
*       lpmShadowType        - shadow type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the HW type used for LPM (TCAM or RAM)
*
* COMMENTS:
*       None
*
*******************************************************************************/
PRV_CPSS_DXCH_LPM_HW_ENT prvCpssDxChLpmGetHwType
(
    IN  PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT lpmShadowType
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmUtilsh */

