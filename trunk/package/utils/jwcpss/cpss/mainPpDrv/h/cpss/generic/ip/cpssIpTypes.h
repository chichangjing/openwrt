/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssIpTypes.h
*
* DESCRIPTION:
*       Common IP facility types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __cpssIpTypesh
#define __cpssIpTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*
 * Typedef: enum CPSS_IP_SITE_ID_ENT
 *
 * Description:
 *      This enum defines IPv6 Site ID (Used by Router for Ipv6 scope checking)
 * Enumerations:
 *     CPSS_IP_SITE_ID_INTERNAL_E - Internal
 *     CPSS_IP_SITE_ID_EXTERNAL_E - External
 *
 *  Comments:
 */
typedef enum
{
    CPSS_IP_SITE_ID_INTERNAL_E,
    CPSS_IP_SITE_ID_EXTERNAL_E
} CPSS_IP_SITE_ID_ENT;

/*
 * typedef: enum CPSS_IP_CNT_SET_ENT
 *
 * Description: Each UC/MC Route Entry result can be linked with one the
 *              below IP counter sets.
 *
 * Enumerations:
 *  CPSS_IP_CNT_SET0_E   - counter set 0
 *  CPSS_IP_CNT_SET1_E   - counter set 1
 *  CPSS_IP_CNT_SET2_E   - counter set 2
 *  CPSS_IP_CNT_SET3_E   - counter set 3
 *  CPSS_IP_CNT_NO_SET_E - do not link route entry with a counter set
 */
typedef enum
{
    CPSS_IP_CNT_SET0_E   = 0,
    CPSS_IP_CNT_SET1_E   = 1,
    CPSS_IP_CNT_SET2_E   = 2,
    CPSS_IP_CNT_SET3_E   = 3,
    CPSS_IP_CNT_NO_SET_E = 4
}CPSS_IP_CNT_SET_ENT;

/*
 * typedef: enum CPSS_IPV6_PREFIX_SCOPE_ENT
 *
 * Description: Address scope for IPv6 addresses
 *
 * Enumerations:
 *  CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E   - linked local scope
 *  CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E   - site local scope
 *  CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E - unique local scope
 *  CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E       - unique global scope
 */
typedef enum
{
    CPSS_IPV6_PREFIX_SCOPE_LINK_LOCAL_E   = 0,
    CPSS_IPV6_PREFIX_SCOPE_SITE_LOCAL_E   = 1,
    CPSS_IPV6_PREFIX_SCOPE_UNIQUE_LOCAL_E = 2,
    CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E       = 3
}CPSS_IPV6_PREFIX_SCOPE_ENT;

/*
 * typedef: enum CPSS_IPV6_MLL_SELECTION_RULE_ENT
 *
 * Description: Rule for choosing MLL for IPv6 multicast propogation
 *
 * Enumerations:
 *  CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E   - use local MLL only
 *  CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E  - use whole MLL
 */
typedef enum
{
    CPSS_IPV6_MLL_SELECTION_RULE_LOCAL_E   = 0,
    CPSS_IPV6_MLL_SELECTION_RULE_GLOBAL_E  = 1
}CPSS_IPV6_MLL_SELECTION_RULE_ENT;

/*
 * typedef: enum CPSS_IP_UNICAST_MULTICAST_ENT
 *
 * Description: Ip Unicast / Multicast selection
 *
 * Enumerations:
 *  CPSS_IP_UNICAST_E    - ip Unicast
 *  CPSS_IP_MULTICAST_E  - ip Multicast
 */
typedef enum
{
    CPSS_IP_UNICAST_E    = 0,
    CPSS_IP_MULTICAST_E  = 1
}CPSS_IP_UNICAST_MULTICAST_ENT;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssIpTypesh */

