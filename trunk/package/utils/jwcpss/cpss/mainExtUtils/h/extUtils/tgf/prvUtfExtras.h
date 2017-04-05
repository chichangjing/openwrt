
/*******************************************************************************
*              (C), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvUtfExtras.h
*
* DESCRIPTION:
*       Internal header which defines API for helpers functions
*       which are specific for cpss unit testing.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
 *******************************************************************************/
#ifndef __prvUtfExtras
#define __prvUtfExtras

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* get common defs */
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>

/*
 * Typedef: enumeration UTF_PP_FAMILY_BIT_ENT
 *
 * Description: The PP family representing bit.
 *
 * Fields:
 *      UTF_TWISTC_E      - (EXMX)   Twist-C family devices
 *      UTF_TWISTD_E      - (EXMX)   Twist-D family devices
 *      UTF_SAMBA_E       - (EXMX)   Samba family devices
 *      UTF_TIGER_E       - (EXMX)   Tiger family devices
 *      UTF_SALSA_E       - (DXSAL)  Salsa family devices
 *      UTF_PUMA2_E       - (EXMXPM) Puma2 family devices
 *      UTF_PUMA3_E       - (EXMXPM) Puma3 family devices
 *      UTF_CH1_E         - (DXCH)   Cheetah family devices
 *      UTF_CH1_DIAMOND_E - (DXCH)   Cheetah Diamond family devices
 *      UTF_CH2_E         - (DXCH)   Cheetah 2 family devices
 *      UTF_CH3_E         - (DXCH)   Cheetah 3 family devices
 *      UTF_XCAT_E        - (DXCH)   xCat family devices
 *      UTF_LION_E        - (DXCH)   Lion family devices
 *      UTF_XCAT2_E       - (DXCH)   xCat2 family devices
 *      UTF_LION2_E       - (DXCH)   Lion2 family devices
 *      UTF_LION3_E       - (DXCH)   Lion3 family devices
 *      UTF_BOBCAT2_E     - (DXCH)   Bobcat2 family devices
 *      UTF_XCAT3_E       - (DXCH)   xCat3 family devices
 *      UTF_BOBCAT3_E     - (DXCH)   Bobcat3 family devices
 *      UTF_CAELUM_E      - (DXCH)   Caelum family devices
 *      UTF_ALL_FAMILY_E  - All devices
 *      UTF_DXCH_E        - (DXCH) All DxCh families devices
 *      UTF_PUMA_E        - (EXMXPM) All Puma families devices
 *
 */
typedef enum
{
    UTF_NONE_FAMILY_E = 0,
    UTF_TWISTC_E      = BIT_0,
    UTF_TWISTD_E      = BIT_1,
    UTF_SAMBA_E       = BIT_2,
    UTF_TIGER_E       = BIT_3,
    UTF_SALSA_E       = BIT_4,
    UTF_PUMA2_E       = BIT_5,
    UTF_PUMA3_E       = BIT_6,
    UTF_CH1_E         = BIT_7,
    UTF_CH1_DIAMOND_E = BIT_8,
    UTF_CH2_E         = BIT_9,
    UTF_CH3_E         = BIT_10,
    UTF_XCAT_E        = BIT_11,
    UTF_LION_E        = BIT_12,
    UTF_XCAT2_E       = BIT_13,
    UTF_LION2_E       = BIT_14,
    UTF_LION3_E       = BIT_15,
    UTF_BOBCAT2_E     = BIT_16,
    UTF_XCAT3_E       = BIT_17,
    UTF_BOBCAT3_E     = BIT_18,
    UTF_CAELUM_E      = BIT_19,

    UTF_PP_FAMILY_BIT__END_OF_DEVICES___E ,/* used for calculation of UTF_ALL_FAMILY_E
                                            must be after the last 'bit' of the last device */

    /* Calc the 'All families' */
    UTF_ALL_FAMILY_E  = ((UTF_PP_FAMILY_BIT__END_OF_DEVICES___E - 1) * 2) - 1 ,


    UTF_PUMA_E        = (UTF_PUMA2_E | UTF_PUMA3_E),
    UTF_EXMX_E        = (UTF_TWISTC_E | UTF_TWISTD_E | UTF_SAMBA_E | UTF_TIGER_E),
    UTF_DX_SAL_E      =  (UTF_SALSA_E),

    /* Calc the 'dxch' devices bmp based on 'All families' and the other families
       that do NOT change often */
    UTF_DXCH_E        = UTF_ALL_FAMILY_E - UTF_PUMA_E - UTF_EXMX_E - UTF_DX_SAL_E
} UTF_PP_FAMILY_BIT_ENT;


/*******************************************************************************
* prvUtfHwDeviceNumberSet
*
* DESCRIPTION:
*       Set HW device number from the SW device number
*
* INPUTS:
*       dev      - SW device number
*       hwDev    - new HW device number
*
* OUTPUTS:

*
* RETURNS:
*       GT_OK           -   Set revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfHwDeviceNumberSet
(
    IN GT_U8               dev,
    IN GT_HW_DEV_NUM       hwDev
);

/*******************************************************************************
* prvUtfHwDeviceNumberGet
*
* DESCRIPTION:
*       Get HW device number from the SW device number
*
* INPUTS:
*       swDevNum    - SW device number
*
* OUTPUTS:
*       hwDevPtr    - (pointer to)HW device number
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfHwDeviceNumberGet
(
    IN  GT_U32              swDevNum,
    OUT GT_HW_DEV_NUM       *hwDevPtr
);

/*******************************************************************************
* prvUtfSwFromHwDeviceNumberGet
*
* DESCRIPTION:
*       Get SW device number from the HW device number
*
* INPUTS:
*       hwDev   - HW device number
*
* OUTPUTS:
*       devPtr  - (pointer to)SW device number
*
* RETURNS:
*       GT_OK           -   Get revision of device OK
*       GT_BAD_PARAM    -   Invalid device id
*       GT_BAD_PTR      -   Null pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvUtfSwFromHwDeviceNumberGet
(
    IN GT_HW_DEV_NUM    hwDev,
    OUT GT_U32          *devPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvUtfExtras */
