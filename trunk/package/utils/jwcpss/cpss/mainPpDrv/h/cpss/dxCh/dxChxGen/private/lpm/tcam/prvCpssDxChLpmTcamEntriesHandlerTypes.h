/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamEntriesHandlerTypes.h
*
* DESCRIPTION:
*       This file contains types for the object to handle TCAM entries.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#ifndef __prvCpssDxChLpmTcamEntriesHandlerTypesh
#define __prvCpssDxChLpmTcamEntriesHandlerTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamMgm.h>

/*
 * typedef: enum PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT
 *
 * Description:
 *      TCAM entry type.
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E        - IPv4 UC
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E  - IPv4 MC group
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E - IPv4 MC source
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E        - IPv6 UC
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E  - IPv6 MC group
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E - IPv6 MC source
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NON_IP_ENTRY_E         - entry using router tcam
 *                                                                      entry but not for prefix
 *
 * Comments:
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_UC_ENTRY_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_GROUP_ENTRY_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV4_MC_SOURCE_ENTRY_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_UC_ENTRY_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_GROUP_ENTRY_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_IPV6_MC_SOURCE_ENTRY_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NON_IP_ENTRY_E

}PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ENTRY_TYPE_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT
 *
 * Description:
 *      TCAM entries handler index allocation method.
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MIN_E    - allocate minimal index.
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MID_E    - allocate near middle index.
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MAX_E    - allocate maximal index.
 *
 * Comments:
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MIN_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MID_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_MAX_E

}PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_ALLOC_METHOD_ENT;


/*
 * typedef: enum PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_MODE_ENT
 *
 * Description:
 *      Operation mode of the TCAM entries handler
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E - LPM DB TCAM is
 *                  partitioned so each entry type has fixed section of the TCAM
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E - LPM DB TCAM
 *                      is not partitioned; all entry types share
 *                      the TCAM entries and TCAM entries are allocated on
 *                      demand for the different entries type;
 *                      used for Cheetah 3 with no partition only
 *      PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_INVALID_MODE_E - invalid
 *
 * Comments:
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_PARTITION_MODE_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_NO_PARTITION_MODE_E,
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_INVALID_MODE_E

}PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_MODE_ENT;

/*
 * typedef: enum PRV_CPSS_DXCH_LPM_TCAM_SEARCH_ORDER_ENT
 *
 * Description:
 *      Search order of the router/TTI TCAM.
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E - column search order;
 *                       meaning entries in first column,
 *                       then entries in second column etc...
 *      PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E - row search column;
 *                     meaning first row, then second row etc...
 *
 * Comments:
 *
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_COLUMN_SEARCH_ORDER_E,
    PRV_CPSS_DXCH_LPM_TCAM_ROW_SEARCH_ORDER_E

}PRV_CPSS_DXCH_LPM_TCAM_SEARCH_ORDER_ENT;

/*
 * typedef: struct PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC
 *
 * Description:
 *      TCAM Entries handler object.
 *
 * Fields:
 *      ---- general fields ----
 *
 *      operationMode         - mode of operation of the tcam entries handler
 *
 *      ---- partition mode (using TCAM pool manager) fields ----
 *
 *      ipTcamPool            - The pools that will manage the ip TCAM rules
 *                              for each protocol stack and for two VRs for ch2
 *                              (one in Router Tcam and second in pcl).
 *      ipMcSrcTcamPool       - The pool that will manage the ip mc source TCAM
 *                              rules.
 *      tcamShadowType        - TCAM shadow type (used for pool identify)
 *
 *      ---- no-partition support fields ----
 *
 *      prefixPtrArray        - array of pointers where each pointer represent
 *                              prefix associated with TCAM entry; there is one
 *                              pointer per TCAM entry that is handled by the
 *                              TCAM entries handler; ordered according to TCAM
 *                              search order.
 *      entriesUsagePtr       - (points to) array of words that represents
 *                              whether entry is used or not (1 indicates the
 *                              entry is used, 0 indicates the entry is free);
 *                              one bit is reserved for each entry; the array
 *                              is ordered by TCAM search order; the entries
 *                              indexes are within the TCAM range allocated to
 *                              this TCAM entries handler;
 *                              for example if the second word is 0x00000201 then
 *                              it means that in the range [32-63] the entries
 *                              32 and 41 are used while all other are free.
 *      lineUsagePtr          - (points to) array of words that represents the
 *                              usage of lines; 4 bits are reserved for each
 *                              line; each of these 4 bits represent whether
 *                              the entry in the coresponding column is used
 *                              or not (1 indicates the entry is used, 0 indicates
 *                              the entry is free); if the 4 bits are all 0 it
 *                              means this line is free; the lines indexes are
 *                              within the TCAM range allocated to this TCAM
 *                              entries handler;
 *                              for example if the second word is 0x000050F1 then
 *                              it means the following: in line 8 (note that
 *                              each word represent 8 lines) the entry in column
 *                              0 is used; in line 9 all entries are used; in
 *                              line 11 the entries in columns 0 and 2 are used;
 *                              lines 10,12,13,14,15 are not used;
 *      firstTcamLine         - first TCAM line allocated for this TCAM handler
 *      numOfTcamLines        - number of TCAM lines allocated for this TCAM handler
 *      numOfTcamEntries      - total number of TCAM entries in the TCAM handler
 *      tcamSearchOrder       - search order of the router/TTI TCAM (column by column
 *                              or row by row)
 *      tcamTotalLines        - total number in the entire TCAM
 *      allocatedIpv4         - number of IPv4 prefixes (UC and MC group)
 *                              currently allocated
 *      allocatedIpv4McSrc    - number of IPv4 MC source prefixes currently allocated
 *      allocatedIpv6         - number of IPv6 prefixes (UC, MC group and MC source)
 *                              currently allocated
 *      guaranteedIpv4        - number of IPv4 prefixes (UC and MC group) guaranteed
 *                              by this TCAM handler
 *      guaranteedIpv4McSrc   - number of IPv4 MC source prefixes guaranteed
 *                              by this TCAM handler
 *      guaranteedIpv6        - number of IPv6 prefixes (UC, MC group and MC source)
 *                              guaranteed by this TCAM handler
 *
 * Comments:
 *
 *      Guaranteed TCAM entries means that at any given time the TCAM entries
 *      handler needs to reserve space so that number of allocated prefixes +
 *      the reserve space will not be less than number of Guaranteed entries.
 *
 */
typedef struct
{
    PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_MODE_ENT operationMode;

    PRV_CPSS_DXCH_LPM_TCAM_MGM_MULTIPLE_POOLS_STC   ipTcamPool;
    PRV_CPSS_DXCH_LPM_TCAM_MGM_POOL_STC             *ipMcSrcTcamPool;
    PRV_CPSS_DXCH_LPM_SHADOW_TYPE_ENT               tcamShadowType;

    PRV_CPSS_DXCH_LPM_TCAM_RULE_NODE_STC            *prefixPtrArray;
    GT_U32                                          *entriesUsagePtr;
    GT_U32                                          *lineUsagePtr;
    GT_U32                                          firstTcamLine;
    GT_U32                                          numOfTcamLines;
    GT_U32                                          numOfTcamEntries;
    PRV_CPSS_DXCH_LPM_TCAM_SEARCH_ORDER_ENT         tcamSearchOrder;
    GT_U32                                          tcamTotalLines;
    GT_U32                                          allocatedIpv4;
    GT_U32                                          allocatedIpv4McSrc;
    GT_U32                                          allocatedIpv6;
    GT_U32                                          guaranteedIpv4;
    GT_U32                                          guaranteedIpv4McSrc;
    GT_U32                                          guaranteedIpv6;

}PRV_CPSS_DXCH_LPM_TCAM_ENTRIES_HANDLER_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamEntriesHandlerTypesh */

