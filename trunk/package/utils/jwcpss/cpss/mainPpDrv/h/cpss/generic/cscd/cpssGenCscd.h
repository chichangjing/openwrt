/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssGenCscd.h
*
* DESCRIPTION:
*       Cascading generic definitions .
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

#ifndef __cpssGenCscdh
#define __cpssGenCscdh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

#define CPSS_CSCD_MAX_DEV_IN_CSCD_CNS   32

/* deprecated legacy names that should not be used ,
    and replaced by CPSS_CSCD_PORT_DSA_MODE_1_WORD_E ,
    and CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E*/
enum{
    CPSS_CSCD_PORT_DSA_MODE_REGULAR_E = 0,
    CPSS_CSCD_PORT_DSA_MODE_EXTEND_E  = 1
};

/*
 * typedef: enum  CPSS_CSCD_PORT_TYPE_ENT
 *
 * Description: Cascading port type enumeration
 *              define the DSA (Distributed System architecture) tag
 *
 * Fields:
 *      CPSS_CSCD_PORT_DSA_MODE_1_WORD_E - port used for cascading using
 *                                  regular DSA tag. (1 word DSA tag)
 *                      APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
 *                      APPLICABLE DEVICES: Puma2.
 *      CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E - port used for cascading using
 *                                   extended DSA tag. (2 words DSA tag)
 *                      APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
 *                      APPLICABLE DEVICES: Puma2; Puma3.
 *      CPSS_CSCD_PORT_NETWORK_E   - regular network port. port not used for
 *                                   cascading and not using DSA tag.
 *                      APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
 *                      APPLICABLE DEVICES: Puma2; Puma3.
 *      CPSS_CSCD_PORT_DSA_MODE_3_WORDS_E - port used for cascading using
 *                                   3 words DSA tag.
 *                      APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
 *      CPSS_CSCD_PORT_DSA_MODE_4_WORDS_E - port used for cascading using
 *                                   4 words DSA tag.
 *                      APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.
 *
 */
typedef enum
{
    CPSS_CSCD_PORT_DSA_MODE_1_WORD_E = CPSS_CSCD_PORT_DSA_MODE_REGULAR_E,
    CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E = CPSS_CSCD_PORT_DSA_MODE_EXTEND_E,
    CPSS_CSCD_PORT_NETWORK_E,
    CPSS_CSCD_PORT_DSA_MODE_3_WORDS_E,
    CPSS_CSCD_PORT_DSA_MODE_4_WORDS_E
} CPSS_CSCD_PORT_TYPE_ENT;


/*
 * typedef: enum  CPSS_CSCD_LINK_TYPE_ENT
 *
 * Description: Type of DSA support on device
 *
 * Values:
 *      CPSS_CSCD_LINK_TYPE_TRUNK_E   - cascading link is trunk
 *      CPSS_CSCD_LINK_TYPE_PORT_E    - cascading link is physical port
 *
 * Comments:
 *      None
 */

typedef enum
{
    CPSS_CSCD_LINK_TYPE_TRUNK_E,
    CPSS_CSCD_LINK_TYPE_PORT_E
} CPSS_CSCD_LINK_TYPE_ENT;


/*
 * typedef: enum  CPSS_DXCH_CSCD_LINK_TYPE_STC
 *
 * Description: Cascading Link definition structure
 *
 * Fields:
 *       linkNum  - number of port or trunk
 *       linkType - trunk/port
 *
 * Comments:
 *      None
 */
typedef struct
{
    GT_U32                    linkNum;
    CPSS_CSCD_LINK_TYPE_ENT   linkType;
} CPSS_CSCD_LINK_TYPE_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenCscdh */


