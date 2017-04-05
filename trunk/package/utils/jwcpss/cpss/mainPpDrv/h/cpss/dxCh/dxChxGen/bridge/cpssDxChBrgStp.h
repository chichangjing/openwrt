/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChBrgStp.h
*
* DESCRIPTION:
*       Spanning Tree protocol facility DxCh CPSS implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/
#ifndef __cpssDxChBrgStph
#define __cpssDxChBrgStph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/bridge/cpssGenBrgVlanTypes.h>

/* DxCh size of STG (Spanning Tree Group) entry in words */
/* NOTE:
    devices with up to 32 ports (cpu not included) use 2 words.
    devices with more than 32 ports (Lion 0..59) use 4 words.
    devices with more than 64 ports (Lion2 0..127) use 8 words.
    devices with more than 128 ports (Bobcat2, Caelum, Bobcat3 0..255) use 16 words.

  In general : using 2 bits per port. so 16 ports per 'GT_U32'.
*/
#define CPSS_DXCH_STG_ENTRY_SIZE_CNS    ((PRV_CPSS_MAX_PP_PORTS_NUM_CNS+15)/16)

/*
* typedef: enum CPSS_BRG_STP_STATE_MODE_ENT
*
* Description: Enumeration of STP state mode
*
* Enumerations:
*       CPSS_BRG_STP_E_VLAN_MODE_E -  STP state for this ePort is
*                               taken from the STP group table indexed by eVLAN
*                               table<Span State Group Index>.
*                               NOTE - eVLAN STG port state is based on
*                                      physical port.
*       CPSS_BRG_STP_E_PORT_MODE_E - STP state for this ePort is
*                               taken from <ePort STP state>
*/
typedef enum
{
    CPSS_BRG_STP_E_VLAN_MODE_E = 0,
    CPSS_BRG_STP_E_PORT_MODE_E
} CPSS_BRG_STP_STATE_MODE_ENT;

/*******************************************************************************
* cpssDxChBrgStpInit
*
* DESCRIPTION:
*       Initialize STP library.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpInit
(
    IN GT_U8    dev
);

/*******************************************************************************
* cpssDxChBrgStpStateSet
*
* DESCRIPTION:
*       Sets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       portNum- port number
*       stpId  - STG (Spanning Tree Group) index
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateSet
(
    IN GT_U8                    dev,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U16                   stpId,
    IN CPSS_STP_STATE_ENT       state
);

/*******************************************************************************
* cpssDxChBrgStpStateGet
*
* DESCRIPTION:
*       Gets STP state of port belonging within an STP group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev         - device number
*       portNum     - port number
*       stpId       - STG (Spanning Tree Group)
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or port or stpId or state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpStateGet
(
    IN GT_U8                    dev,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U16                   stpId,
    OUT CPSS_STP_STATE_ENT      *statePtr
);

/*******************************************************************************
* cpssDxChBrgStpEntryGet
*
* DESCRIPTION:
*       Gets the RAW STP entry from HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev    - device number
*       stpId  - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*
* OUTPUTS:
*       stpEntryWordArr     - (Pointer to) The raw stp entry to get.
*                         memory allocated by the caller.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryGet
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    OUT GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);

/*******************************************************************************
* cpssDxChBrgStpEntryWrite
*
* DESCRIPTION:
*       Writes a raw STP entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dev             - device number
*       stpId           - STG (Spanning Tree Group) index (APPLICABLE RANGES: 0..255)
*       stpEntryWordArr     - (Pointer to) The raw stp entry to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device or stpId
*       GT_BAD_PTR               - on NULL pointer parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpEntryWrite
(
    IN  GT_U8       dev,
    IN  GT_U16      stpId,
    IN  GT_U32      stpEntryWordArr[CPSS_DXCH_STG_ENTRY_SIZE_CNS]
);

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateSet
*
* DESCRIPTION:
*       Set Per-Port STP state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*       state  - STP port state.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable when <Port STP state Mode> = ePort
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateSet
(
    IN GT_U8                devNum,
    IN GT_PORT_NUM          portNum,
    IN CPSS_STP_STATE_ENT   state
);

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateGet
*
* DESCRIPTION:
*       Get Per-Port STP state.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       statePtr    - (pointer to) STP port state.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateGet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_NUM             portNum,
    OUT CPSS_STP_STATE_ENT      *statePtr
);

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateModeSet
*
* DESCRIPTION:
*       Determines whether the STP state for this port is
*       taken from the L2I ePort table or from the STP group table via the STG
*       index from eVLAN table
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       stateMode   - Port STP state Mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateModeSet
(
    IN GT_U8                        devNum,
    IN GT_PORT_NUM                  portNum,
    IN CPSS_BRG_STP_STATE_MODE_ENT  stateMode
);

/*******************************************************************************
* cpssDxChBrgStpPortSpanningTreeStateModeGet
*
* DESCRIPTION:
*       Get whether the STP state for this port is taken from the
*       L2I ePort table or from the STP group table via the STG index from eVLAN table.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       stateModePtr  - (pointer to) Port STP state Mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChBrgStpPortSpanningTreeStateModeGet
(
    IN  GT_U8                           devNum,
    IN  GT_PORT_NUM                     portNum,
    OUT CPSS_BRG_STP_STATE_MODE_ENT     *stateModePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ____cpssDxChBrgStph */

