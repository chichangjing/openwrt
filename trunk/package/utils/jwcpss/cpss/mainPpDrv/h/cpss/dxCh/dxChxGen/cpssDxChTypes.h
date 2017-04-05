/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChTypes.h
*
* DESCRIPTION:
*       CPSS DXCH Generic types.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __cpssDxChTypesh
#define __cpssDxChTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnelTypes.h>

/*
 * typedef: struct CPSS_DXCH_OUTPUT_INTERFACE_STC
 *
 * Description: Defines the interface info
 *
 * Fields:
 *   isTunnelStart      - if set to GT_TRUE, packet is to be tunneled.
 *   tunnelStartInfo    - Tunnel Start information (Relevant if isTunnelStart== GT_TRUE)
 *              passengerPacketType - Type of passenger packet going to be
 *                                    encapsulated.
 *              ptr                 - A pointer to a tunnel entry
 *   physicalInterface   - defines the phisical interface info. Reffer to
 *                         CPSS_INTERFACE_INFO_STC. The vidx and vlanId fields
 *                         are not in use in this case.
 *
 * Notes:
 *      1. The structure is used in following cases:
 *          a. Logical Target Mapping
 *
 */

typedef struct
{
    GT_BOOL                 isTunnelStart;

    struct{
        CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT passengerPacketType;
        GT_U32                              ptr;
    }tunnelStartInfo;

    CPSS_INTERFACE_INFO_STC physicalInterface;
} CPSS_DXCH_OUTPUT_INTERFACE_STC;

/*
 * typedef: enum CPSS_DXCH_MEMBER_SELECTION_MODE_ENT
 *
 *      Description: enumerator that hold values for the type of how many bits
 *      are used in a member selection function.
 *      Used for trunk member selection and by L2 ECMP member selection.
 *
 * Enumerations:
 *      CPSS_DXCH_MEMBER_SELECTION_MODE_12_BITS_E -
 *                  Use the entire 12 bit hash in the member selection function.
 *                  ((Hash[11:0] * #members)/4096)
 *
 *      CPSS_DXCH_MEMBER_SELECTION_MODE_6_LSB_E -
 *                  Use only the 6 least significant bits in the member selection.
 *                  ((Hash[5:0] * #members)/64)
 *
 *      CPSS_DXCH_MEMBER_SELECTION_MODE_6_MSB_E -
 *                  Use only the 6 most significant bits in the member selection.
 *                  ((Hash[11:6] * #members)/64)
 *
 */
typedef enum {
    CPSS_DXCH_MEMBER_SELECTION_MODE_12_BITS_E = 0,
    CPSS_DXCH_MEMBER_SELECTION_MODE_6_LSB_E,
    CPSS_DXCH_MEMBER_SELECTION_MODE_6_MSB_E
}CPSS_DXCH_MEMBER_SELECTION_MODE_ENT;

/*
 * typedef: enum CPSS_DXCH_FDB_LEARN_PRIORITY_ENT
 *
 * Description: Enumeration of FDB Learn Priority
 *
 * Enumerations:
 *      CPSS_DXCH_FDB_LEARN_PRIORITY_LOW_E - low priority
 *      CPSS_DXCH_FDB_LEARN_PRIORITY_HIGH_E - high priority
 */
typedef enum
{
    CPSS_DXCH_FDB_LEARN_PRIORITY_LOW_E,
    CPSS_DXCH_FDB_LEARN_PRIORITY_HIGH_E
} CPSS_DXCH_FDB_LEARN_PRIORITY_ENT;

/*
 * typedef: enum CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT
 *
 * Description: the possible forms to write a pair of entries
 *
 * Enumerations:
 *
 *  CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E   - reads/writes just the
 *                                             first entry of the pair.
 *  CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E - reads/writes
 *                        just the second entry of the pair and the next pointer
 *  CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E  - reads/writes the whole pair.
 */
typedef enum
{
    CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E                = 0,
    CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E  = 1,
    CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E                     = 2
}CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT;


/*
 * typedef: enum CPSS_DXCH_MLL_ACCESS_CHECK_MIN_MAX_ENT
 *
 * Description: whether a limit is used as upper limit or lower limit
 *
 * Enumerations:
 *  CPSS_DXCH_MLL_ACCESS_CHECK_MIN_ENT - MLL pointer limit is used as a lower limit
 *  CPSS_DXCH_MLL_ACCESS_CHECK_MAX_ENT - MLL pointer limit is used as a upper limit
 */
typedef enum
{
    CPSS_DXCH_MLL_ACCESS_CHECK_MIN_ENT = 0,
    CPSS_DXCH_MLL_ACCESS_CHECK_MAX_ENT = 1
}CPSS_DXCH_MLL_ACCESS_CHECK_MIN_MAX_ENT;

/*
 * Typedef: enum CPSS_DXCH_ETHERTYPE_TABLE_ENT
 *
 * Description:
 *      This enumeration defines etherType table selection
 * Fields:
 *     CPSS_DXCH_ETHERTYPE_TABLE_INGRESS_E - Ingress TPID table
 *     CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_E  - Egress TPID table
 *     CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E - Egress Logical Port TPID table
 *     CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_TS_E - Egress tunnel start TPID table
 */
typedef enum{

    CPSS_DXCH_ETHERTYPE_TABLE_INGRESS_E = CPSS_DIRECTION_INGRESS_E,
    CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_E = CPSS_DIRECTION_EGRESS_E,
    CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_LOGICAL_PORT_E,
    CPSS_DXCH_ETHERTYPE_TABLE_EGRESS_TS_E

}CPSS_DXCH_ETHERTYPE_TABLE_ENT;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChTypesh */
