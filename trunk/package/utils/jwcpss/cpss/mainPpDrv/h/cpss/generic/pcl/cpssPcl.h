/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssPcl.h
*
* DESCRIPTION:
*       Common PCL definitions.
*
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __cpssPclh
#define __cpssPclh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: CPSS_PCL_DIRECTION_ENT
 *
 * Description:
 *      PCL direction.
 * Fields:
 *      CPSS_PCL_DIRECTION_INGRESS_E - The PCL will work on ingress traffic.
 *      CPSS_PCL_DIRECTION_EGRESS_E  - The PCL will work on egress traffic.
 *
 * Comment:
 */
typedef enum
{
    CPSS_PCL_DIRECTION_INGRESS_E = 0,
    CPSS_PCL_DIRECTION_EGRESS_E
} CPSS_PCL_DIRECTION_ENT;

/*
 * typedef: CPSS_PCL_LOOKUP_NUMBER_ENT
 *
 * Description:
 *      Number of lookup for the PCL
 *
 * Fields:
 *      CPSS_PCL_LOOKUP_0_E - first lookup #0.
 *      CPSS_PCL_LOOKUP_1_E - second lookup #1.
 *      CPSS_PCL_LOOKUP_0_0_E - the 0-th sub lookup of lookup #0
 *                       (APPLICABLE DEVICES: xCat2)
 *      CPSS_PCL_LOOKUP_0_1_E - the 1-th sub lookup of lookup #0
 *                       (APPLICABLE DEVICES: xCat2)
 *      The values below are aliases for old names that are more suitable
 *      for Functional Specification documents terminology of Bobcat2 and above PPs.
 *      xCat2 and Lion2 LOOKUP_0_0 and LOOKUP_0_1
 *      already have no shared parameters in HW, but terminology yet is old and
 *      CPSS_PCL_LOOKUP_0_E using means to configure both.
 *      CPSS_PCL_LOOKUP_NUMBER_0_E - Lookup 0, Alias for CPSS_PCL_LOOKUP_0_0_E
 *      CPSS_PCL_LOOKUP_NUMBER_1_E - Lookup 1, Alias for CPSS_PCL_LOOKUP_0_1_E
 *      CPSS_PCL_LOOKUP_NUMBER_2_E - Lookup 2, Alias for CPSS_PCL_LOOKUP_1_E
 *
 *
 * Comment:
 */
typedef enum
{
    CPSS_PCL_LOOKUP_0_E = 0,
    CPSS_PCL_LOOKUP_1_E,
    CPSS_PCL_LOOKUP_0_0_E,
    CPSS_PCL_LOOKUP_0_1_E,
    CPSS_PCL_LOOKUP_NUMBER_0_E = CPSS_PCL_LOOKUP_0_0_E,
    CPSS_PCL_LOOKUP_NUMBER_1_E = CPSS_PCL_LOOKUP_0_1_E,
    CPSS_PCL_LOOKUP_NUMBER_2_E = CPSS_PCL_LOOKUP_1_E
} CPSS_PCL_LOOKUP_NUMBER_ENT;

/*
 * Typedef: enum CPSS_PCL_RULE_SIZE_ENT
 *
 * Description:
 *     This enum describes sizes of Policy Rules.
 * Fields:
 *     CPSS_PCL_RULE_SIZE_STD_E           - standard size rule
 *     CPSS_PCL_RULE_SIZE_EXT_E           - extended size rule
 *     CPSS_PCL_RULE_SIZE_ULTRA_E         - ultra size rule
 *                                         (APPLICABLE DEVICES: Lion2; Bobcat2; Caelum; Bobcat3)
 *     CPSS_PCL_RULE_SIZE_10_BYTES_E      - 10-bytes rule
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     CPSS_PCL_RULE_SIZE_20_BYTES_E      - 20-bytes rule
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     CPSS_PCL_RULE_SIZE_30_BYTES_E      - 30-bytes rule - synonim of CPSS_PCL_RULE_SIZE_STD_E
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     CPSS_PCL_RULE_SIZE_40_BYTES_E      - 40-bytes rule
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     CPSS_PCL_RULE_SIZE_50_BYTES_E      - 50-bytes rule
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     CPSS_PCL_RULE_SIZE_60_BYTES_E      - 60-bytes rule - synonim of CPSS_PCL_RULE_SIZE_EXT_E
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *     CPSS_PCL_RULE_SIZE_80_BYTES_E      - 80-bytes rule - synonim of CPSS_PCL_RULE_SIZE_ULTRA_E
 *                                         (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 *  Comments:
 */
typedef enum
{
    CPSS_PCL_RULE_SIZE_STD_E,
    CPSS_PCL_RULE_SIZE_30_BYTES_E = CPSS_PCL_RULE_SIZE_STD_E,
    CPSS_PCL_RULE_SIZE_EXT_E,
    CPSS_PCL_RULE_SIZE_60_BYTES_E = CPSS_PCL_RULE_SIZE_EXT_E,
    CPSS_PCL_RULE_SIZE_ULTRA_E,
    CPSS_PCL_RULE_SIZE_80_BYTES_E = CPSS_PCL_RULE_SIZE_ULTRA_E,
    CPSS_PCL_RULE_SIZE_10_BYTES_E,
    CPSS_PCL_RULE_SIZE_20_BYTES_E,
    CPSS_PCL_RULE_SIZE_40_BYTES_E,
    CPSS_PCL_RULE_SIZE_50_BYTES_E
} CPSS_PCL_RULE_SIZE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssPclh */

