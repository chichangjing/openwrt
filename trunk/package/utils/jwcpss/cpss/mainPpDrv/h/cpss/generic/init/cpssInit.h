/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssInit.c
*
* DESCRIPTION:
*       Initialization function for PP's , regardless to PP's types.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __cpssInith
#define __cpssInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*******************************************************************************
* cpssPpInit
*
* DESCRIPTION:
*       This function initialize the internal DB of the CPSS regarding PPs.
*       This function must be called before any call to a PP functions ,
*       i.e before calling cpssExMxPmHwPpPhase1Init/cpssDxChHwPpPhase1Init.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpInit
(
    void
);
/*******************************************************************************
* cpssPpDestroy
*
* DESCRIPTION:
*       This function destroy the internal DB of the CPSS regarding PPs.
*       This function should be called after all devices have been removed from
*       the CPSS and system need to 'clean up' before restart initialization again.
*       so function must be called before recalling cpssPpInit(...)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       none.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpDestroy
(
    void
);

/*******************************************************************************
* cpssSystemDualDeviceIdModeEnableSet
*
* DESCRIPTION:
*       This function declare 'This system support dual device Id'.
*       The function MUST be called for any system with Lion2 ('Dual device Id' device).
*       This means that application MUST not use 'odd' hwDevNum to any device in the system.
*       Allowed to use only : 0,2,4..30 (also for ch1,2,3,xcat1,2,Lion in this system)
*
*       For such system this function must be called before any call to a PP functions ,
*       i.e before calling cpssDxChHwPpPhase1Init(...).
*
*       The function MUST be called only after cpssPpInit(...)
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       enable  - indication that the system is in 'dual deviceId' mode.
*               GT_TRUE - This system support dual device Id
*               GT_FALSE - This system NOT support dual device Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_STATE             - the function called before call to cpssPpInit(...).
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSystemDualDeviceIdModeEnableSet
(
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssSystemDualDeviceIdModeEnableGet
*
* DESCRIPTION:
*       This function retrieve if 'This system support dual device Id'.
*       This means that application MUST not use 'odd' hwDevNum to any device in the system.
*       Allowed to use only : 0,2,4..30 (also for ch1,2,3,xcat1,2,Lion in this system)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       enablePtr  - (pointer to)indication that the system is in 'dual deviceId' mode.
*               GT_TRUE - This system support dual device Id
*               GT_FALSE - This system NOT support dual device Id
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssSystemDualDeviceIdModeEnableGet
(
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssPpDump
*
* DESCRIPTION:
*       Dump function , to print the info on a specific PP.
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpDump
(
    IN GT_U8   devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssInith */

