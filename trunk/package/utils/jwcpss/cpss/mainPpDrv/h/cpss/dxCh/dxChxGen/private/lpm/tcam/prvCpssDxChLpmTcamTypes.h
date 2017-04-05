/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamTypes.h
*
* DESCRIPTION:
*       Private cpss DXCH LPM library type definitions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*
*******************************************************************************/
#ifndef __prvCpssDxChLpmTcamTypesh
#define __prvCpssDxChLpmTcamTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmTypes.h>

/*
 * typedef: struct PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC
 *
 * Description:
 *      This struct hold the capacity configuration required from the TCAM LPM
 *      manager. According to these requirements the TCAM entries will be
 *      fragmented.
 *
 * Fields:
 *  numOfIpv4Prefixes           - The total number of Ipv4 Uc prefixes and
 *                                ipv4 MC group prefixes required.
 *  numOfIpv4McSourcePrefixes   - The number of Ipv4 MC source prefixes required.
 *  numOfIpv6Prefixes           - The total number of Ipv6 Uc prefixes,
 *                                ipv6 MC group prefixes and ipv6 MC sources
 *                                prefixes required.
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STCT
{
    GT_U32  numOfIpv4Prefixes;
    GT_U32  numOfIpv4McSourcePrefixes;
    GT_U32  numOfIpv6Prefixes;
}PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC;

/*
 * typedef: struct PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC
 *
 * Description:
 *      This struct hold the indexes available for the TCAM LPM manager.
 *      it defines the From-To indexes in the TCAM the lpm manager can use.
 *
 *      Cheetah+ systems: The indexes are the PCL Rule indexes dedicated for the
 *                        IP.
 *                        Pay attention that when used to indicate PCL Tcam
 *                        lines, the line x means pcl tcam indexes x,x+512.
 *      Cheetah2 systems: The indexes are the LINE indexes dedicated to for the
 *                        IP (UC + MC) out of the 1024 lines avilable. pay
 *                        attention that the a line x in the router tcam means
 *                        router tcam indexes x,x+1024,x+2048,x+3072,x+4096.
 *      Cheetah2 with VR support systems:
 *                        The indexes are the LINE indexes dedicated to for the
 *                        IP UC out of the total lines available in PCL/Router.
 *                        Pay attention that when used to indicate Router Tcam
 *                        lines, the line x means router tcam indexes
 *                        x,x+1024,x+2048,x+3072,x+4096.
 *                        Pay attention that when used to indicate PCL Tcam
 *                        lines, the line x means pcl tcam indexes x,x+512.
 *
 * Fields:
 *  firstIndex - this is the first index availble (from)
 *  lastIndex  - this is the last index availble (to - including)
 *
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STCT
{
    GT_U32  firstIndex;
    GT_U32  lastIndex;
}PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_CONFIG_STCT
 *
 * Description: TCAM configurations for LPM DB
 *
 * Fields:
 *      indexesRange             - range of TCAM indexes availble for the LPM DB
 *      partitionEnable          - GT_TRUE:  partition the TCAM range according
 *                                           to the tcamLpmManagerCapcityCfg,
 *                                           any unused TCAM entries will be
 *                                           allocated to IPv4 UC entries
 *                                 GT_FALSE: allocate TCAM entries on demand
 *                                           while guaranteeing entries as
 *                                           specified in tcamLpmManagerCapcityCfg
 *      tcamLpmManagerCapcityCfg - capacity configuration required from the TCAM
 *                                 LPM manager
 *      tcamManagerHandlerPtr    - TCAM manager handler
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_CONFIG_STCT
{
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_INDEX_RANGE_STC    indexesRange;
    GT_BOOL                                          partitionEnable;
    PRV_CPSS_DXCH_LPM_TCAM_MANGER_CAPCITY_CFG_STC    tcamLpmManagerCapcityCfg;
    GT_VOID                                          *tcamManagerHandlerPtr;
} PRV_CPSS_DXCH_LPM_TCAM_CONFIG_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STCT
 *
 * Description: IPv6 UC Prefix information for bulk operations
 *
 * Fields:
 *      supportIpv4Uc           - whether this VR supports UC Ipv4.
 *      defIpv4UcNextHopInfo    - the route entry info associated with the default
 *                                Ipv4 UC route entry.
 *      supportIpv4Mc           - whether this VR support MC Ipv4.
 *      defIpv4McRouteLttEntry  - the LTT entry pointing to the default ipv4 MC route
 *                                entry.
 *      supportIpv6Uc           - whether this VR supports UC Ipv6.
 *      defIpv6UcNextHopInfo    - the route entry info associated with the default
 *                                Ipv6 UC route entry.
 *      supportIpv6Mc           - whether this VR supports MC Ipv6
 *      defIpv6McRouteLttEntry  - the LTT entry pointing to the default Ipv6 MC route
 *                                entry;
 * Comments:
 *       supportIpv4Uc, supportIpv4Mc, supportIpv6Uc, supportIpv6Mc can't be all
 *       GT_FALSE.
 */
typedef struct PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STCT
{
    GT_BOOL                                                supportIpv4Uc;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT                 defIpv4UcNextHopInfo;
    GT_BOOL                                                supportIpv4Mc;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC              defIpv4McRouteLttEntry;
    GT_BOOL                                                supportIpv6Uc;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_INFO_UNT                 defIpv6UcNextHopInfo;
    GT_BOOL                                                supportIpv6Mc;
    PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC              defIpv6McRouteLttEntry;
} PRV_CPSS_DXCH_LPM_TCAM_VR_CONFIG_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChLpmTcamTypesh */

