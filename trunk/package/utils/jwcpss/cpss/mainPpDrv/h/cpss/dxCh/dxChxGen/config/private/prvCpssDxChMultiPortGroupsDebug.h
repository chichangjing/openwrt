/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMultiPortGroupsDebug.h
*
* DESCRIPTION:
*       This file includes the declaration of the "debug"
*       for the multi-port-groups device.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __prvCpssDxChMultiPortGroupsDebugh
#define __prvCpssDxChMultiPortGroupsDebugh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>


/*
 * Typedef: struct PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_PORT_GROUP_PORT_INFO_STC
 *
 * Description: A structure to hold debug info for
 *              the 'multi-port-groups' device.
 *              define 'port-group port' info as
 *              portGroupId, local port number
 *
 * Fields:
 *      portGroupId - the port-group-id of this port.
 *              when CPSS_PORT_GROUP_UNAWARE_MODE_CNS -->
 *              this port-group port info 'Not valid'
 *      localPort   - the local port number in this port-group.
 */
typedef struct
{
    GT_U32      portGroupId;
    GT_U32      localPort;
} PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_PORT_GROUP_PORT_INFO_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_UPLINK_INFO_STC
 *
 * Description: A structure to hold debug info for the
 *             'multi-port-groups' device.
 *              define 'Uplink' info with it's 'Internal connections'
 *
 * Fields:
 *      startPortGroupId - Id of the port group that 'start the ring' for this uplink
 *
 *      uplinkPortGroupPortInfo - the 'uplink' port-group port info.
 *
 *      numberOfInternalConnections - number of 'Inter connections' , see internalConnectionsArr.
 *      internalConnectionsArr - array of 'Internal connections' from the port-group of
 *                      the 'uplink' to the other port-groups.
 *                      (dynamic allocated array)
 *                      NOTE: those ports should be on active port-groups that reside
 *                      on other port-groups then the 'uplink' port-group port
 *
 */
typedef struct
{
    GT_U32      startPortGroupId;

    PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_PORT_GROUP_PORT_INFO_STC uplinkPortGroupPortInfo;

    GT_U32  numberOfInternalConnections;
    PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_PORT_GROUP_PORT_INFO_STC *internalConnectionsArr;

} PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_UPLINK_INFO_STC;


/*
 * Typedef: struct PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_STC
 *
 * Description: A structure to hold debug info for the 'multi-port-groups' device.
 *              this info used for internal behavior tests (debug) and
 *              configuration.
 *
 * Fields:
 *  debugInfoValid - is the debug info valid ?
 *  numOfUplinkPorts - number of 'uplinks'. see uplinkInfoArr.
 *  uplinkInfoArr - array of 'uplinks' . this array allocated dynamically ,
 *                  according to caller.
 *
 *  unknownUnregDesignatedDeviceArray - array of the device number(s) to use in the 'Device map table'
 *                  for traffic used as 'Trap to CPU' for 'Vlan unknown Uc, Unreg Mc'
 *                  access to this array is 'port group id' of the port group
 *                  from which the traffic 'jump' to the next port group
 *                  NOTE: the size is (CPSS_MAX_PORT_GROUPS_CNS)
 *
 *  unknownUnregDesignatedDeviceIndex - the index that used in the 'Cpu code table'
 *                  for the entry of 'Vlan unknown Uc, Unreg Mc'
 *
 *  internalTrunksPtr - array with the trunk id's to use for internal trunks ,
 *                  according to their port group
 *                  access to this array is 'port group id' of the port group
 *                  from which the traffic 'jump' to the next port group
 *                  NOTE: 1. the size is (CPSS_MAX_PORT_GROUPS_CNS)
 *                        2. used when numOfUplinkPorts > 1 the size is CPSS_MAX_PORT_GROUPS_CNS
 *
 *  numOfCpuCodes - number of cpu codes , see unknownUnregCpuCodesArray[]
 *  unknownUnregCpuCodesPtr - (pointer to) array of the CPU codes that we need to force to
 *                  go to the next port group (unless last port group)
 *
 *  lastPortGroupBmp - the bitmap of port groups that represent the 'last' port-group and ,
 *              'ends' their interconnection link.
 *              for example (when all 4 port-groups are active):
 *              for single link mode : ( ->0->1->2->3->)
 *                  when the uplink is port-group 0 --> lastPortGroupId is 3
 *                  when the uplink is port-group 1 --> lastPortGroupId is 0
 *                  when the uplink is port-group 2 --> lastPortGroupId is 1
 *                  when the uplink is port-group 3 --> lastPortGroupId is 2
 *              for dual link mode : (0<->1 , 2<->3   or 0<->2 , 1<->3 or 0<->3 , 1<->2)
 *                  when the uplink is port-group 0 --> lastPortGroupId is 1
 *                  when the uplink is port-group 1 --> lastPortGroupId is 0
 *                  when the uplink is port-group 2 --> lastPortGroupId is 3
 *                  when the uplink is port-group 3 --> lastPortGroupId is 2
 *
 *  srcIdFilterFloodToUplinks - the srcId filtering needed for :
 *              because packets sent across the ring are not DSA-tagged, the original source
 *              info is not passed to next port group, so that would prevent the use of
 *              source filtering of flooded packets. So we need to use SrcID filtering in
 *              this case, where traffic received on ring ports are filtered on uplink ports.
 *
 */
typedef struct
{
    GT_BOOL     debugInfoValid;

    GT_U32      numOfUplinkPorts;
    PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_UPLINK_INFO_STC *uplinkInfoArr;

    GT_U8       *unknownUnregDesignatedDeviceArray;/*[CPSS_MAX_PORT_GROUPS_CNS]*/
    GT_U32      unknownUnregDesignatedDeviceIndex;

    GT_TRUNK_ID *internalTrunksPtr;/*[CPSS_MAX_PORT_GROUPS_CNS]*/

    GT_U32      numOfCpuCodes;
    CPSS_NET_RX_CPU_CODE_ENT *unknownUnregCpuCodesPtr;

    GT_PORT_GROUPS_BMP      lastPortGroupBmp;

    GT_U32      srcIdFilterFloodToUplinks;

} PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_STC;

/* macro to PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC */
#define PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)   \
    (&PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.debugInfo)


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChMultiPortGroupsDebugh */

