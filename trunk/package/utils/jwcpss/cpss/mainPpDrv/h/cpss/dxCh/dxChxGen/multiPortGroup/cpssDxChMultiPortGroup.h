/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChMultiPortGroup.h
*
* DESCRIPTION:
*       The file hold APIs for building infrastructure of the connections between
*       the port groups of a multi-port group device, to get FDB and TTI
*       enhancements.
*
* FILE REVISION NUMBER:
*       $Revision: 14 $
*
*******************************************************************************/
#ifndef __cpssDxChMultiPortGrouph
#define __cpssDxChMultiPortGrouph

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>

/*
 * typedef: enum CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT
 *
 * Description: Enumeration of the multi port groups lookup not found command
 *
 * Enumerations:
 *    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E  - do not redirect
 *                              to ring. meaning that the lookup on the ring ends.
 *    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E  - redirect
 *                              to ring. meaning that the lookup on the ring continue.
*/
typedef enum{
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E,
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E
}CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT;



/*
 * typedef: enum CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT
 *
 * Description: Enumeration of the FDB modes
 *
 * Enumerations:
 *    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E  - all port groups have the
 *          same FDB entries. (unified tables)
 *          A.K.A 32K mode for Lion (1*32K)
 *          A.K.A 64K mode for Lion2 (1*64K)
 *    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E  - the port groups of
 *          the device are split to :
 *              in Lion  to 2 groups .
 *              in Lion2 to 4 groups or to 2 groups.
 *          Each port group in those groups may hold different FDB entries.
 *          But the those groups hold the same entries.
 *          A.K.A  64K mode for Lion  (2*32K) - 2 groups
 *          A.K.A 256K mode for Lion2 (4*64K) - 2 groups
 *          A.K.A 128K mode for Lion2 (2*64K) - 4 groups
 *    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E - each port group of
 *          the device may hold different FDB entries.
 *          A.K.A 128K mode for Lion  (4*32K)
 *          A.K.A 512K mode for Lion2 (8*64K)
 *
 *
 */
typedef enum{
    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E,
    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E,
    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E
}CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT;

/*
 * typedef: structure CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC
 *
 * Description: structure of the multi port groups lookup not found command and
 *              next ring info
 *
 * Fields:
 *  unknownDaCommand - multi port groups FDB lookup not found command.(redirect/not to the ring)
 *  nextRingInterface - info about the next (target) ring interface.
 *                      the relevant interfaces are :
 *                      CPSS_INTERFACE_PORT_E and CPSS_INTERFACE_TRUNK_E
 *
*/
typedef struct{
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT      unknownDaCommand;
    CPSS_INTERFACE_INFO_STC    nextRingInterface;
}CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC;



/*
 * typedef: structure CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC
 *
 * Description: structure of the ring ports info
 *
 * Fields:
 *  numOfRingPorts - number of ring ports in array of ringPortsPtr.
 *  ringPortsPtr - (pointer to) array of ring ports that are used for traffic
 *              initialized at 'srcPortGroupId' and do lookup at other 'port groups'
 *
 *  for example :
 *      1. In order to avoid oversubscribing when there are 3 uplinks on port
 *         group 0 , it should be configured:
 *           srcPortGroupId = 0
 *           numOfRingPorts = 9
 *           ringPortsPtr[] = 3 ports in port group 1 , 3 ports in port group 2
 *                            and 3 ports in port group 3
 *
 *       2. example how to use for 64K FDB mode:
 *          lets assume application want port groups 0,1 to be 'group A' and
 *          port groups 2,3 to be 'group B' --> meaning 64K FDB in 'group A'
 *          involve only ports from port group 0,1
 *          lets assume 1 uplink in each port group.
 *          it should be configured:
 *           srcPortGroupId = 0
 *           numOfRingPorts = 1
 *           ringPortsPtr[] = 1 port in port group 1
 *
 *           srcPortGroupId = 1
 *           numOfRingPorts = 1
 *           ringPortsPtr[] = 1 port in port group 0
 *
 *           srcPortGroupId = 2
 *           numOfRingPorts = 1
 *           ringPortsPtr[] = 1 port in port group 3
 *
 *           srcPortGroupId = 3
 *           numOfRingPorts = 1
 *           ringPortsPtr[] = 1 port in port group 2
 *
 *       trunkIdNum - number of 'internal trunk' in internalTrunksArr[]
 *       trunkIdPtr[] - array of trunks (trunk-IDs) , that used for multi-ring
 *                      ports , when more then single ring port used for
 *                      passing traffic from source port group through any
 *                      other port group.
 *                      explanation on how cpssDxChMultiPortGroupConfigSet
 *                      'bind' the ring ports to those trunks:
 *                      assume srcPortGroupId = 2 , numOfRingPorts = 9
 *                      ringPortsPtr[] = 3 ports in port group 3 --> will get trunkIdPtr[0]
 *                                       3 ports in port group 0 --> will get trunkIdPtr[1]
 *                                       3 ports in port group 1 --> will get trunkIdPtr[2]
 *                      --> so trunkId are set according to the cyclic order of
 *                      the port groups that given as ring ports.
 *
*/
typedef struct{
    GT_U32                srcPortGroupId;
    GT_U32                numOfRingPorts;
    GT_PHYSICAL_PORT_NUM  *ringPortsPtr;
    GT_U32                trunkIdNum;
    GT_TRUNK_ID           *trunkIdPtr;
}CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC;


/*******************************************************************************
* cpssDxChMultiPortGroupConfigSet
*
* DESCRIPTION:
*
*       The purpose of this API is configure a Multi portGroup topology.
*       This API configures several tables/registers in the HW to support
*       requested topology. the aggregated functionality described in details in
*       the user guide for this feature.
*       Application can implement the buildup of the topology differently.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - the device number
*       numOfUplinkPorts - number of uplink ports in uplinkPortsArr[]
*       uplinkPortsArr[] - array of uplink ports.
*                   configures the Uplinks of the defined topology.
*       ringInfoNum - number of elements in array of ringInfoArr[]
*       ringInfoArr[] - array of information about ring connections.
*               The ringInfoNum & ringInfoArr holds the information of rings ports in topology.
*               Array is built in such way that for each portGroup that has uplinks should
*               configure all rings ports that would be used to channel its traffic to
*               all other portGroups (For Lion in 64K mode its only to its member in couple)
*               Multiple rings ports can be used to pass traffic between two hops of
*               same channel - in that case these groups of ports should be assigned with
*               trunk Id .
*               Same Rings ports can be reused to pass different channels from different
*               source portGroups, thus creating over-subscription of traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or one of the ports in uplinkPortsArr[] ,
*                   or in ringInfoArr[]
*    GT_BAD_PTR   - on NULL pointer
*    GT_OUT_OF_CPU_MEM - dynamic allocation for internal DB failed
*    GT_FAIL - internal management implementation error.
*    GT_ALREADY_EXIST   - already initialized
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*    To enable TTI multi-lookup application has to enable lookup on the relevant ring ports
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupConfigSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  numOfUplinkPorts,
    IN  GT_PORT_NUM   uplinkPortsArr[],
    IN  GT_U32  ringInfoNum,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC   ringInfoArr[]
);

/*******************************************************************************
* cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
*
* DESCRIPTION:
*
*       1. By default traffic flowing via Multi-portGroups Ring Trunks is
*          distributed over Trunk’s member ports based on Packet-info hashing.
*       2. API should be called after topology buildup , in case application
*          wishes to use srcPort Trunk hashing over certain multi-portGroup
*          ring Trunk.
*       3. Application should use this API to map portGroup local source ports
*          to next’s PortGroup Trunk member ports.
*       4. Source ports used for mapping are always local portGroup ports.
*       5. Each Source Port is mapped to Trunk member port
*       6. Multiple Source Ports can be mapped to same Trunk member port allowing
*          oversubscribing
*       7. Application can't map two Source ports that falls into same Source
*          hash index into different trunk member ports (when Modulo 8 mode used)
*       8. API configures Trunk designated table
*       9. API is consuming a unique device number for each unique
*           trunkId (starts from deviceNum 31..19 ) - up to max of 12 devices.
*          these device numbers (from device map table) are used for mapping to
*          Trunks Designated member table.
*       10. The application is responsible to check ports in trunkPort are really
*           the members of trunkId. (the function does not check)
*
*       the function sets the designated device table with the portsArr[].trunkPort
*       in indexes that match hash (%8(modulo) or %64(modulo))
*       on the ports in portsArr[].srcPort .
*       this to allow 'Src port' trunk hash for traffic sent to the specified trunk.
*
*       the function sets the trunk members (only in 'trunk members' table) for
*       this trunkId with single member which is the 'virtual interface' {port,device}.
*       this 'virtual interface' actually point the PP to the 'device map table'
*       which points to the specified trunk , and will set 'src port'
*       hash.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - the trunkId that the ports in portsArr[].trunkPort belongs to.
*       numOfPorts  - number of pairs in array portsArr[].
*       portsArr[] - (array of) pairs of 'source ports' ,'trunk ports'
*                     for the source port hash.
*       mode - hash mode (%8 or %64 (modulo))
*
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or trunkId or port in portsArr[].srcPort or
*                   port in portsArr[].trunkPort or mode
*                   or conflict according to description bullet#7
*    GT_OUT_OF_CPU_MEM - failed on dynamic allocation
*    GT_FULL        - the DB is full with the previous 12 unique trunks
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
(
    IN GT_U8             devNum,
    IN GT_TRUNK_ID       trunkId,
    IN GT_U32            numOfPorts,
    IN CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC  portsArr[],
    IN CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT  mode
);


/*******************************************************************************
* cpssDxChMultiPortGroupPortLookupEnableSet
*
* DESCRIPTION:
*      function enable/disable whether an incoming packet from the port is
*       subject to multi-Port Group lookup
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
*       enable      - GT_TRUE -  an incoming packet from the port is
*                               subject to multi-Port Group lookup
*                     GT_FALSE - an incoming packet from the port is NOT
*                               subject to multi-Port Group lookup
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupPortLookupEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);


/*******************************************************************************
* cpssDxChMultiPortGroupPortLookupEnableGet
*
* DESCRIPTION:
*      function gets whether an incoming packet from the port is
*       subject to multi-Port Group FDB lookup (enable/disable)
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
* OUTPUTS:
*       enablePtr   - (pointer to) enable :
*                     GT_TRUE -  an incoming packet from the port is
*                               subject to multi-Port Group FDB lookup
*                     GT_FALSE - an incoming packet from the port is NOT
*                               subject to multi-Port Group FDB lookup
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupPortLookupEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT  GT_BOOL                *enablePtr
);



/*******************************************************************************
* cpssDxChMultiPortGroupPortRingEnableSet
*
* DESCRIPTION:
*      function enable/disable a port to be 'Ring port'.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
*       enable      - GT_TRUE -  a ring port
*                     GT_FALSE - not a ring port
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupPortRingEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChMultiPortGroupPortRingEnableGet
*
* DESCRIPTION:
*      function gets whether a port is 'Ring port' (enable/disable)
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
* OUTPUTS:
*       enablePtr   - (pointer to) enable :
*                     GT_TRUE -  a ring port
*                     GT_FALSE - not a ring port
*
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_BAD_STATE - the settings in HW are not synchronized between the 2 sections
*                   (HA,TTI)
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupPortRingEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChMultiPortGroupLookupNotFoundTableSet
*
* DESCRIPTION:
*      function set the multi port groups lookup not found info per source port groupId
*      The Table has 4 entries (representing each of the possible
*      4 ingress source portGroups) in each portGroup. Each entry is configured with either
*      REDIECT or NO_REDIRECT command. If entry is set with REDIRECT command,
*      application has to supply ring port interface information which can be
*      either port or Trunk-ID at the next portGroup
*
*      This configuration should be done similarly in cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
*      enable = GT_FALSE in API of cpssDxChMultiBridgeUnknownDaMaskEnableSet
*      is like unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E in current API
*      and
*      enable=GT_TRUE is like unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       sourcePortGroupId - the source Port Group Id (APPLICABLE RANGES: 0..3),
*                          that started the lookups in the ring for the handled packet
*       infoPtr      - (pointer to) the setting for the sourcePortGroupId for lookup not found.
*
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId or parameter in infoPtr
*    GT_OUT_OF_RANGE - on 'next interface' port/trunk values out of range
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupLookupNotFoundTableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  sourcePortGroupId,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC   *infoPtr
);


/*******************************************************************************
* cpssDxChMultiPortGroupLookupNotFoundTableGet
*
* DESCRIPTION:
*      function get the multi port groups FDB lookup not found info per source port groupId
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                              - read only from first active port group of the bitmap.
*       sourcePortGroupId - the source Port Group Id (APPLICABLE RANGES: 0..3),
*                          that started the lookups in the ring for the handled packet
*
* OUTPUTS:
*       infoPtr      - (pointer to) the setting for the sourcePortGroupId for lookup not found.
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupLookupNotFoundTableGet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  sourcePortGroupId,
    OUT  CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC   *infoPtr
);

/*******************************************************************************
* cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
*
* DESCRIPTION:
*      Function enable/disable the multi port groups Unknown DA mask
*
*      This configuration should be done similarly in cpssDxChMultiPortGroupLookupNotFoundTableSet
*      unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E in API of
*      cpssDxChMultiPortGroupLookupNotFoundTableSet
*      is like enable = GT_FALSE in current API
*      and
*      unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E
*      is like enable=GT_TRUE
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       sourcePortGroupId - the source Port Group Id (APPLICABLE RANGES: 0..3),
*                          that started the lookups in the ring for the handled packet
*       enable - enable/disable the mask
*               GT_FALSE -  Disable --> the 'src port group' is the last port group in
*               the inter- port group ring, If the DA is not found, and the packet
*               source port group is 'src port group', the relevant VLAN command
*               is applied to the packet:
*               UnregisteredIPv4BC , UnregisteredIPv4BC , UnknownUnicastCmd ,
*               UnregisteredIPv6MulticastCmd , UnregisteredIPv4MulticastCmd ,
*               UnregisteredNonIPMulticastCmd
*               GT_TRUE - Enable --> the 'src port group' is NOT the last port group
*               in the inter- port group ring,If the DA is not found, and the packet
*               source port group is 'src port group', the relevant Unknown/Unregistered
*               VLAN command is ignored and the packet is assigned the FORWARD
*               command
*
* OUTPUTS:
*       none
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  sourcePortGroupId,
    IN  GT_BOOL enable
);

/*******************************************************************************
* cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet
*
* DESCRIPTION:
*      Function get the enable/disable state of the multi port groups Unknown DA mask
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                              - read only from first active port group of the bitmap.
*       sourcePortGroupId - the source Port Group Id (APPLICABLE RANGES: 0..3),
*                          that started the lookups in the ring for the handled packet
*
* OUTPUTS:
*       enablePtr - (pointer to) enable/disable the mask
*               GT_FALSE -  Disable --> the 'src port group' is the last port group in
*               the inter- port group ring, If the DA is not found, and the packet
*               source port group is 'src port group', the relevant VLAN command
*               is applied to the packet:
*               UnregisteredIPv4BC , UnregisteredIPv4BC , UnknownUnicastCmd ,
*               UnregisteredIPv6MulticastCmd , UnregisteredIPv4MulticastCmd ,
*               UnregisteredNonIPMulticastCmd
*               GT_TRUE - Enable --> the 'src port group' is NOT the last port group
*               in the inter- port group ring,If the DA is not found, and the packet
*               source port group is 'src port group', the relevant Unknown/Unregistered
*               VLAN command is ignored and the packet is assigned the FORWARD
*               command
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  sourcePortGroupId,
    OUT  GT_BOOL *enablePtr
);


/*******************************************************************************
* cpssDxChMultiPortGroupFdbModeSet
*
* DESCRIPTION:
*       Function sets multi port groups FDB modes
*       and affects the way FDB low-level APIs are handling the various operations.
*       (no HW operations , set value to 'DB')
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       fdbMode     - FDB mode.
* OUTPUTS:
*       none
*
* RETURNS:
*    GT_OK        - on success
*    GT_BAD_PARAM - on wrong devNum or fdbMode
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupFdbModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode
);

/*******************************************************************************
* cpssDxChMultiPortGroupFdbModeGet
*
* DESCRIPTION:
*       Function return the FDB mode for the multi-port group.
*       (no HW operations , return value from 'DB')
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       fdbModePtr  - (pointer to) FDB mode.
*
* RETURNS:
*    GT_OK        - on success
*    GT_BAD_PARAM - on wrong devNum or fdbMode
*    GT_BAD_PTR   - on NULL pointer
*    GT_BAD_STATE - the DB with unknown value
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS cpssDxChMultiPortGroupFdbModeGet
(
    IN  GT_U8   devNum,
    OUT  CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT *fdbModePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChMultiPortGrouph */
