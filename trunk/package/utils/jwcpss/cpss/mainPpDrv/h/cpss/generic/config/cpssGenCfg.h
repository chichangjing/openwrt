/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenCfg.h
*
* DESCRIPTION:
*       CPSS generic configuration types.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __cpssGenCfgh
#define __cpssGenCfgh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>

/*
 * typedef: CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT
 *
 * Description: defines device's HW device usage
 *
 * Enumerations:
 *  CPSS_GEN_CFG_HW_DEV_NUM_MODE_SINGLE_E - device uses single HW device number
 *  CPSS_GEN_CFG_HW_DEV_NUM_MODE_DUAL_E - device uses dual HW device number
*/

typedef enum
{
    CPSS_GEN_CFG_HW_DEV_NUM_MODE_SINGLE_E,
    CPSS_GEN_CFG_HW_DEV_NUM_MODE_DUAL_E
} CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT;


/*
 * typedef: struct CPSS_GEN_CFG_DEV_INFO_STC
 *
 * Description: Generic device info structure
 *
 * Fields:
 *      devType        - device type of the PP.
 *      revision       - the device's revision number.
 *      devFamily      - CPSS's device family that device belongs to.
 *      devSubFamily   - CPSS's device sub-family that device belongs to.
 *      maxPortNum     - maximal port's number not including CPU one.
 *      numOfVirtPorts - number of virtual ports.
 *                       Relevant only for devices with virtual ports support.
 *      existingPorts  - bitmap of actual exiting ports not including CPU one.
 *      hwDevNumMode   - HW device number mode.
 *      cpuPortMode    - CPU port mode.
 *      numOfLedInfPerPortGroup - a number of LED interfaces per Port Group
 *                       If a device is not multi-port-group this value is the
 *                       same as a number of LED Interfaces per device.
 */
typedef struct
{
    CPSS_PP_DEVICE_TYPE              devType;
    GT_U8                            revision;
    CPSS_PP_FAMILY_TYPE_ENT          devFamily;
    CPSS_PP_SUB_FAMILY_TYPE_ENT      devSubFamily;
    GT_U8                            maxPortNum;
    GT_U8                            numOfVirtPorts;
    CPSS_PORTS_BMP_STC               existingPorts;
    CPSS_GEN_CFG_HW_DEV_NUM_MODE_ENT hwDevNumMode;
    CPSS_NET_CPU_PORT_MODE_ENT       cpuPortMode;
    GT_U32                           numOfLedInfPerPortGroup;
}CPSS_GEN_CFG_DEV_INFO_STC;

/*******************************************************************************
* cpssPpCfgNextDevGet
*
* DESCRIPTION:
*
*       Return the number of the next existing device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to start from. For the first one devNum should be 0xFF.
*
* OUTPUTS:
*       nextDevNumPtr - number of next device after devNum.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NO_MORE               - devNum is the last device. nextDevNumPtr will be set to 0xFF.
*       GT_BAD_PARAM             - devNum > max device number
*       GT_BAD_PTR               - nextDevNumPtr pointer is NULL.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPpCfgNextDevGet
(
    IN  GT_U8 devNum,
    OUT GT_U8 *nextDevNumPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenCfgh */
