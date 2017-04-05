/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChMultiPortGroup.c
*
* DESCRIPTION:
*       The file hold API for building infrastructure of the connections between
*       the port groups of a multi-port group device, to get FDB and TTI
*       enhancements.
*
* FILE REVISION NUMBER:
*       $Revision: 37
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/multiPortGroup/private/prvCpssDxChMultiPortGroupLog.h>

/* get the device info and common info */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/multiPortGroup/cpssDxChMultiPortGroup.h>
/* include needed features */
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgPrvEdgeVlan.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgSrcId.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgEgrFlt.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>

/*  designated devices that we use to allow the 'next ring interface' to point
*   to it , so the designated devices will point to internal trunks
*/
#define DESIGNATED_DEVICE_MAC(offset)     (31 - (offset))

/* max number of internal trunks */
#define MAX_TRUNKS_NUM_CNS    12

/*******************************************************************************
* multiPortGroupConfigSetParametersCheck
*
* DESCRIPTION:
*       check parameter for function cpssDxChMultiPortGroupConfigSet(...)
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
*
*       ringInfoNum - number of elements in array of ringInfoArr[]
*       ringInfoArr[] - array of information about ring connections.
*               The ringInfoNum & ringInfoArr holds the information of rings ports in topology.
*               Array is built in such way that for each portGroup that has uplinks should
*               configure all rings ports that would be used to channel its traffic to
*               all other portGroups (In 64K its only to its member in couple)
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
* COMMENTS
*
*******************************************************************************/
static GT_STATUS multiPortGroupConfigSetParametersCheck
(
    IN  GT_U8   devNum,
    IN  GT_U32  numOfUplinkPorts,
    IN  GT_PORT_NUM   uplinkPortsArr[],
    IN  GT_U32  ringInfoNum,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC   ringInfoArr[]
)
{
    GT_U32      ii,jj,kk;/* iterators */
    GT_U32  numActivePortGroups = 0;/* number of active port groups */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC *currRingInfoPtr;/* current pointer to rings info*/
    GT_U32  numRingPortsInPortGroup[CPSS_MAX_PORT_GROUPS_CNS];/* number of ring ports per port group */
    GT_U32  chainLinkedLength[CPSS_MAX_PORT_GROUPS_CNS]={0};/* each (src) port group have
                            the length of linked port groups (number of port groups)*/
    GT_PORT_GROUPS_BMP  chainLinkedGroups[CPSS_MAX_PORT_GROUPS_CNS]={0};/* each (src) port group have
                            the bitmap port groups of linked port groups (bmp of port groups)*/

    CPSS_NULL_PTR_CHECK_MAC(ringInfoArr);
    CPSS_NULL_PTR_CHECK_MAC(uplinkPortsArr);

    if(numOfUplinkPorts == 0 || ringInfoNum == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* calculate number of active port groups */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        numActivePortGroups++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if(numActivePortGroups < 2)
    {
        /* not multi-port-groups device */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    /* check that UPLINKS are 'physical ports' (no meaning for CPU port) */
    for( ii = 0 ; ii < numOfUplinkPorts ; ii++ )
    {
        PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,uplinkPortsArr[ii]);

        /* check duplications of ports */
        for(jj = 0; jj < numOfUplinkPorts ;jj++)
        {
            if(ii == jj)
            {
                continue;
            }

            if(uplinkPortsArr[jj] == uplinkPortsArr[ii])
            {
                /* duplication of port */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
    }

    currRingInfoPtr = ringInfoArr;
    for( kk = 0 ; kk < ringInfoNum ; kk++ ,currRingInfoPtr++)
    {
        cpssOsMemSet(numRingPortsInPortGroup,0,sizeof(numRingPortsInPortGroup));

        if(currRingInfoPtr->numOfRingPorts == 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* check the src port group Id */
        if(currRingInfoPtr->srcPortGroupId >=
           PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        for(ii = 0 ; ii < currRingInfoPtr->numOfRingPorts ; ii++)
        {
            /* check that ring port are 'physical ports' (no meaning for CPU port) */
            PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,currRingInfoPtr->ringPortsPtr[ii]);

            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                currRingInfoPtr->ringPortsPtr[ii]);

            numRingPortsInPortGroup[portGroupId]++;

            /* check duplications of ports */
            for(jj = 0; jj < currRingInfoPtr->numOfRingPorts ;jj++)
            {
                if(ii == jj)
                {
                    continue;
                }

                if(currRingInfoPtr->ringPortsPtr[jj] == currRingInfoPtr->ringPortsPtr[ii])
                {
                    /* duplication of port */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
        }

        jj = 0;

        /* check that there are enough trunkIds and with valid values */
        for(portGroupId = 0 ; portGroupId < CPSS_MAX_PORT_GROUPS_CNS; portGroupId++)
        {

            if(currRingInfoPtr->srcPortGroupId == portGroupId)
            {
                /* no trunks for ports in 'source port group' */

                if(numRingPortsInPortGroup[portGroupId])
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                continue;
            }

            if(numRingPortsInPortGroup[portGroupId])
            {
                /* we have another port group on the 'linked list' ,
                   which started from 'srcPortGroupId' */
                chainLinkedLength[currRingInfoPtr->srcPortGroupId]++;
                chainLinkedGroups[currRingInfoPtr->srcPortGroupId] |= 1 << portGroupId;
            }

            if(numRingPortsInPortGroup[portGroupId] > 1)
            {
                /* trunk needed */
                if(jj >= currRingInfoPtr->trunkIdNum)
                {
                    /* not enough trunks supplied */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                CPSS_NULL_PTR_CHECK_MAC(currRingInfoPtr->trunkIdPtr);

                if(currRingInfoPtr->trunkIdPtr[jj] >= BIT_7 ||
                   currRingInfoPtr->trunkIdPtr[jj] == 0 )
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                jj++;
            }

        }  /* loop on portGroupId */
    }/* loop on kk */

    /**************************************/
    /* done checking the input parameters */
    /**************************************/

    return GT_OK;
}


/*******************************************************************************
* ringPortConfigModeSet
*
* DESCRIPTION:
*
*       function to set 'ring port' for cpssDxChMultiPortGroupConfigSet(...)
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - the device number
*       ringPort - port to set as 'ring port'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*
* COMMENTS
*
*******************************************************************************/
static GT_STATUS ringPortConfigSet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM   ringPort
)
{
    GT_STATUS   rc;/* return code */

    /* enable for multi-port groups lookup */
    rc = cpssDxChMultiPortGroupPortLookupEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* DSA tagged frames to pass via this port */
    rc = cpssDxChCscdPortTypeSet(devNum,ringPort,CPSS_PORT_DIRECTION_BOTH_E,CPSS_CSCD_PORT_DSA_MODE_EXTEND_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* allow FDB decisions for taffic coming with DSA tag 'forward' */
    rc = cpssDxChCscdPortBridgeBypassEnableSet(devNum,ringPort,GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set port as 'ring port' */
    rc = cpssDxChMultiPortGroupPortRingEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* disable 'flooding' of unknown unicast */
    rc = cpssDxChBrgPortEgrFltUnkEnable(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* disable 'flooding' of unregistered multicast */
    rc = cpssDxChBrgPortEgrFltUregMcastEnable(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* disable 'flooding' of unregistered broadcast */
    rc = cpssDxChBrgPortEgrFltUregBcEnable(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set special port interface mode to prevent ring port serdeses reset
       mac unreset done here inside */
    rc = cpssDxChPortInterfaceModeSet(devNum,ringPort,CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E);
    if(rc != GT_OK)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            CPSS_TBD_BOOKMARK_LION2
            /* this interface was not implemented for Lion2 yet */
                rc = GT_OK;
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }


    rc = cpssDxChPortSpeedSet(devNum, ringPort, CPSS_PORT_SPEED_10000_E);
    if(rc != GT_OK)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            CPSS_TBD_BOOKMARK_LION2
            /* this speed was not implemented for Lion2 yet */
                rc = GT_OK;
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }


    /* configure just native serdeses of ring port */
    rc = cpssDxChPortSerdesPowerStatusSet(devNum, ringPort, CPSS_PORT_DIRECTION_BOTH_E,
                                                0x3, GT_TRUE);
    if(rc != GT_OK)
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
        {
            CPSS_TBD_BOOKMARK_LION2
            /* this speed was not implemented for Lion2 yet */
                rc = GT_OK;
        }

        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* enable the port */
    rc = cpssDxChPortEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* force link up */
    rc = cpssDxChPortForceLinkPassEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* loopback mode */
    rc = cpssDxChPortInternalLoopbackEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* controlled learning */
    rc = cpssDxChBrgFdbPortLearnStatusSet(devNum,ringPort,GT_FALSE,CPSS_LOCK_FRWRD_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* NO NA to CPU --> no learning on those ports */
    rc = cpssDxChBrgFdbNaToCpuPerPortSet(devNum,ringPort,GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set short preamble - due to DSA tag */
    rc = cpssDxChPortPreambleLengthSet(devNum,ringPort,CPSS_PORT_DIRECTION_TX_E, 4);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* set shorter IPG : fixed mode */
    rc = cpssDxChPortXGmiiModeSet(devNum,ringPort,CPSS_PORT_XGMII_FIXED_E);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* set shorter IPG : 8 bytes instead of 12 */
    rc  = cpssDxChPortIpgBaseSet(devNum,ringPort,CPSS_PORT_XG_FIXED_IPG_8_BYTES_E);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* trust QOSProfile from DSA tag */
    rc = cpssDxChCosTrustDsaTagQosModeSet(devNum, ringPort, GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* multiPortGroupLookupNotFoundConfig
*
* DESCRIPTION:
*      function handle 2 APIs in the same time:
*       cpssDxChMultiPortGroupLookupNotFoundTableSet(...)
*       cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet(...)
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
static GT_STATUS multiPortGroupLookupNotFoundConfig
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  sourcePortGroupId,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC   *infoPtr
)
{
    GT_STATUS rc;

    rc = cpssDxChMultiPortGroupLookupNotFoundTableSet(devNum,
        portGroupsBmp,
        sourcePortGroupId ,
        infoPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet(devNum,
        portGroupsBmp,
        sourcePortGroupId ,
        (infoPtr->unknownDaCommand ==
            CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E) ?
                GT_FALSE : GT_TRUE);
}

/*******************************************************************************
* trunkCascadeTrunkPortsSet
*
* DESCRIPTION:
*       This function sets the 'cascade' trunk with the specified 'Local ports'
*       overriding any previous setting.
*       To allow LBH on cascade trunks ports between port-groups , this function
*       sets the 'per ingress port' to not be 'attached' to the trunk.
*
*       IMPORTANT : this setting assumes that 'flooding' is never send from
*                   cascade trunks !  (see egress filtering settings)
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       portsMembersPtr - (pointer to) local ports bitmap to be members of the
*                   cascade trunk.
*                   NULL - meaning that the trunk-id is 'invalidated' and
*                          trunk-type will be changed to : CPSS_TRUNK_TYPE_FREE_E
*                   not-NULL - meaning that the trunk-type will be : CPSS_TRUNK_TYPE_CASCADE_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - there are ports in the bitmap that not supported by
*                            the device.
*       GT_BAD_PARAM       - bad device number , or bad trunkId number , or number
*                            of ports (in the bitmap) larger then the number of
*                            entries in the 'Designated trunk table'
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS trunkCascadeTrunkPortsSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
    GT_STATUS               rc;
    GT_PHYSICAL_PORT_NUM    ii; /* port iterator */

    rc = cpssDxChTrunkCascadeTrunkPortsSet(devNum,trunkId,portsMembersPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(portsMembersPtr)
    {
        for(ii = 0 ; ii < CPSS_MAX_PORTS_NUM_CNS; ii++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr,ii))
            {
                continue;
            }

            /* the port is attached with trunkId in the 'per port' -->
              need to remove it to achieve LBH between cascade trunks */

            /* use trunk 'Low level' API to override what was set by cpssDxChTrunkCascadeTrunkPortsSet(...) */
            rc = cpssDxChTrunkPortTrunkIdSet(devNum, ii, GT_FALSE, 0);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* multiPortGroupConfigSet
*
* DESCRIPTION:
*
*       function to set mode for cpssDxChMultiPortGroupConfigSet(...)
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2
*
* INPUTS:
*       devNum           - the device number
*       numOfUplinkPorts - number of uplink ports in uplinkPortsArr[]
*       uplinkPortsArr[] - array of uplink ports.
*                          configures the Uplinks of the defined topology.
*
*       ringInfoNum      - number of elements in array of ringInfoArr[]
*       ringInfoArr[]    - array of information about ring connections.
*               The ringInfoNum & ringInfoArr holds the information of rings ports in topology.
*               Array is built in such way that for each portGroup that has uplinks should
*               configure all rings ports that would be used to channel its traffic to
*               all other portGroups (In 64K its only to its member in couple)
*               Multiple rings ports can be used to pass traffic between two hops of
*               same channel - in that case these groups of ports should be assigned with
*               trunk Id .
*               Same Rings ports can be reused to pass different channels from different
*               source portGroups, thus creating over-subscription of traffic.
*
*               The ringInfoArr may hold four possible entries - as all
*               portGroups may be the source of traffic,
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
* COMMENTS
*
*******************************************************************************/
static GT_STATUS multiPortGroupConfigSet
(
    IN  GT_U8                                              devNum,
    IN  GT_U32                                             numOfUplinkPorts,
    IN  GT_PORT_NUM                                        uplinkPortsArr[],
    IN  GT_U32                                             ringInfoNum,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC      ringInfoArr[]
)
{
    GT_STATUS   rc;/* return code */
    GT_U32  ii,jj,kk,mm;/*iterators*/
    GT_U32  port;/* current port to config */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  startPortGroupId;/* Id of the port group that 'start the ring'  */
    GT_U32  nextPortGroupId;/* Id of the next port group */
    GT_U32  prevPortGroupId;/* Id of the previous port group */
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*bitmap of Port Groups*/
    GT_TRUNK_ID trunksArr[CPSS_MAX_PORT_GROUPS_CNS];/* array of trunkIds to use for internal trunks ,
                        index to this array is the portGroupId of a ring port that
                        is member in the trunk */
    CPSS_PORTS_BMP_STC   cscdTrunkBmp[CPSS_MAX_PORT_GROUPS_CNS];/* bmp of ports members in the cascade trunk */
    CPSS_PORTS_BMP_STC   tmpCcscdTrunkBmp;/* tmp bmp of ports members in the cascade trunk */
    CPSS_TRUNK_TYPE_ENT  trunkType;/* trunk type */
    CPSS_PORTS_BMP_STC  networkPorts;/* bmp of network ports */
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC   lookupNotFoundinfo = {0};/*the setting for the
                                sourcePortGroupId for lookup not found.*/
    GT_U32                  numTrunkMembers;/* number of members in the trunk */
    CPSS_TRUNK_MEMBER_STC   trunkMembersArr[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];/*trunk members array*/


    /* calculate the remaining network ports */
    networkPorts = PRV_CPSS_PP_MAC(devNum)->existingPorts;

    /* configuration of 'uplink ports' */
    for (ii = 0 ; ii < numOfUplinkPorts; ii++)
    {
        port = uplinkPortsArr[ii];

        /* remove the uplinks from the 'network ports' */
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&networkPorts,port);

        /* set the port as uplink ports */
        rc = cpssDxChMultiPortGroupPortLookupEnableSet(devNum,port,GT_TRUE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }/*ii*/


    /***************************/
    /* clear previous settings */
    /***************************/

    /* set to all port groups that all source port groups has no redirect */
    lookupNotFoundinfo.unknownDaCommand =
        CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc = multiPortGroupLookupNotFoundConfig(devNum,
            CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
            portGroupId ,
            &lookupNotFoundinfo);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    /* set the device to filter the srcPort of own device on DSA tag frames */
    rc = cpssDxChCscdOrigSrcPortFilterEnableSet(devNum,GT_TRUE);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* configuration of 'ring ports' */
    for(kk = 0 ; kk < ringInfoNum ; kk++)
    {
        cpssOsMemSet(trunksArr,0,sizeof(trunksArr));
        cpssOsMemSet(cscdTrunkBmp,0,sizeof(cscdTrunkBmp));

        startPortGroupId = ringInfoArr[kk].srcPortGroupId;

        portGroupsBmp = 0;

        /* 1. set 'ring port' configuration */
        /* 2. calculate the portGroupsBmp of current 'link' (without the startPortGroupId) */
        for(jj = 0 ; jj <  ringInfoArr[kk].numOfRingPorts ; jj++)
        {
            port = ringInfoArr[kk].ringPortsPtr[jj];
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                port);

            /* set the port as ring port */
            rc = ringPortConfigSet(devNum,port);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(portGroupId == startPortGroupId)
            {
                /* RU ports not need to be in trunk */
                continue;
            }

            portGroupsBmp |= 1 << portGroupId;
        } /*jj*/

        if(portGroupsBmp == 0)
        {
            /* no port groups ? */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* set trunks for the ring ports (if needed) ,
           and add the ring ports to them */
        for(jj = 0 ; jj <  ringInfoArr[kk].numOfRingPorts ; jj++)
        {
            port = ringInfoArr[kk].ringPortsPtr[jj];
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                port);

            if(0 == (portGroupsBmp & (1 << portGroupId)))
            {
                /* RU ports not need to be in trunk */
                continue;
            }

            /* check that there are several ring ports on this port group
               otherwise not need trunk */
            for(mm = 0 ; mm <  ringInfoArr[kk].numOfRingPorts ; mm++)
            {
                if(port == ringInfoArr[kk].ringPortsPtr[mm])
                {
                    /* this is our current port , skip it */
                    continue;
                }

                if(portGroupId == PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                    ringInfoArr[kk].ringPortsPtr[mm]))
                {
                    /* at least 2 ring ports on the same port group so wee need trunk */
                    break;
                }
            }/*mm*/

            if(mm == ringInfoArr[kk].numOfRingPorts)
            {
                /* no need to create trunk on current port group */
                continue;
            }

            if(trunksArr[portGroupId] == 0)/* trunk Id was not yet defined */
            {
                /* find the index of the trunkId to use */

                /* the first trunkId goes to 'first port group (that is
                   not 'src port group')in link' , second to second port group ... */

                mm = 0;
                nextPortGroupId = startPortGroupId;

                while(1)
                {
                    rc = prvCpssPortGroupsNextPortGroupGet(devNum,nextPortGroupId,&nextPortGroupId);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    if(0 == ((1 << nextPortGroupId) & portGroupsBmp))
                    {
                        /* this port group not in the 'link' */
                        continue;
                    }

                    if(nextPortGroupId == portGroupId)
                    {
                        break;
                    }

                    mm++;/* index of the current port group in the link */
                }

                if(mm >= ringInfoArr[kk].trunkIdNum)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                trunksArr[portGroupId] = ringInfoArr[kk].trunkIdPtr[mm];

                rc = cpssDxChTrunkDbTrunkTypeGet(devNum,trunksArr[portGroupId],&trunkType);
                if(rc != GT_OK)
                {
                    return rc;
                }

                switch(trunkType)
                {
                    case CPSS_TRUNK_TYPE_CASCADE_E:
                        break;
                    case CPSS_TRUNK_TYPE_FREE_E:
                        break;
                    default:
                        /* this trunk already used for other usages */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

            }

            /* define the trunk member as the ring port on this device */
            CPSS_PORTS_BMP_PORT_SET_MAC(&cscdTrunkBmp[portGroupId],port);
        }/*jj*/

        /* create the inter connection trunks (if needed) */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            if(trunksArr[portGroupId] == 0)
            {
                continue;
            }


            /* this cascade trunk maybe reused for several port groups traffic
               passing through this port group */
            rc = cpssDxChTrunkCascadeTrunkPortsGet(devNum,trunksArr[portGroupId],&tmpCcscdTrunkBmp);
            if(rc != GT_OK)
            {
                return rc;
            }

            CPSS_PORTS_BMP_BITWISE_OR_MAC(&cscdTrunkBmp[portGroupId],&cscdTrunkBmp[portGroupId],&tmpCcscdTrunkBmp);

            rc = trunkCascadeTrunkPortsSet(devNum,trunksArr[portGroupId],&cscdTrunkBmp[portGroupId]);
            if(rc != GT_OK)
            {
                return rc;
            }

            numTrunkMembers = 0;
            for(jj = 0 ; jj < (PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum)+1); jj++)
            {
                if(CPSS_PORTS_BMP_IS_PORT_SET_MAC((&cscdTrunkBmp[portGroupId]),jj))
                {
                    if(numTrunkMembers == CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
                    {
                        break;
                    }
                    trunkMembersArr[numTrunkMembers].hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
                    trunkMembersArr[numTrunkMembers].port = jj;
                    numTrunkMembers++;
                }
            }

            /* NOTE: this calling 'low level trunk API' to allow setting the
                    trunk members for the use of the 'EQ' (per-egress) of the
                    device */

            /* set the 'trunk members table' with up to 8 first ports in the trunk
               to support 'packet hash info' (as defaults for application that
               not use the function cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet)
            */
            rc = cpssDxChTrunkTableEntrySet(devNum,trunksArr[portGroupId],
                numTrunkMembers,trunkMembersArr);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

        /* set the device map table , so traffic can 'jump' from port group to
           the next , via port or trunk */
        for(jj = 0 ; jj <  ringInfoArr[kk].numOfRingPorts ; jj++)
        {
            port = ringInfoArr[kk].ringPortsPtr[jj];
            portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
                port);

            if(0 == (portGroupsBmp & (1 << portGroupId)))
            {
                /* RU ports not need to be pointed from 'device map table' */
                continue;
            }

            /* find the previous port group , to get index into
               unknownUnregDesignatedDeviceArray[] */
            prevPortGroupId = portGroupId;
            while(1)
            {
                rc = prvCpssPortGroupsPrevPortGroupGet(devNum,prevPortGroupId,&prevPortGroupId);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if(prevPortGroupId == startPortGroupId)
                {
                    /* got to start of link */
                    break;
                }

                if(((1 << prevPortGroupId) & portGroupsBmp))
                {
                    /* found valid previous port group */
                    break;
                }
            }

            lookupNotFoundinfo.unknownDaCommand =
                CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E;
            if(trunksArr[portGroupId] == 0)/* no need trunk */
            {
                lookupNotFoundinfo.nextRingInterface.type = CPSS_INTERFACE_PORT_E;
                lookupNotFoundinfo.nextRingInterface.devPort.hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
                lookupNotFoundinfo.nextRingInterface.devPort.portNum = port;
            }
            else
            {
                lookupNotFoundinfo.nextRingInterface.type = CPSS_INTERFACE_TRUNK_E;
                lookupNotFoundinfo.nextRingInterface.trunkId = trunksArr[portGroupId];
            }

            rc = multiPortGroupLookupNotFoundConfig(devNum,
                (1 << prevPortGroupId) ,
                startPortGroupId ,
                &lookupNotFoundinfo);
            if(rc != GT_OK)
            {
                return rc;
            }
        }/*jj*/
    }/*kk*/

    /* do other device global settings */
    /* allow traffic to switch between port groups in the same device */
    rc = cpssDxChCscdDsaSrcDevFilterSet(devNum,GT_FALSE);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChMultiPortGroupConfigSet
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
static GT_STATUS internal_cpssDxChMultiPortGroupConfigSet
(
    IN  GT_U8   devNum,
    IN  GT_U32  numOfUplinkPorts,
    IN  GT_PORT_NUM   uplinkPortsArr[],
    IN  GT_U32  ringInfoNum,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC   ringInfoArr[]
)
{
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);

    /* check the input parameters */
    rc = multiPortGroupConfigSetParametersCheck(devNum,
        numOfUplinkPorts,uplinkPortsArr,ringInfoNum,ringInfoArr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set configuration */
    rc = multiPortGroupConfigSet(devNum,
        numOfUplinkPorts,uplinkPortsArr,ringInfoNum,ringInfoArr);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupConfigSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numOfUplinkPorts, uplinkPortsArr, ringInfoNum, ringInfoArr));

    rc = internal_cpssDxChMultiPortGroupConfigSet(devNum, numOfUplinkPorts, uplinkPortsArr, ringInfoNum, ringInfoArr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numOfUplinkPorts, uplinkPortsArr, ringInfoNum, ringInfoArr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
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
static GT_STATUS internal_cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
(
    IN GT_U8             devNum,
    IN GT_TRUNK_ID       trunkId,
    IN GT_U32            numOfPorts,
    IN CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC  portsArr[],
    IN CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT  mode
)
{
    GT_STATUS   rc;/* return code */
    GT_U32  portGroupId1,portGroupId2;/*port group Id 1,2*/
    GT_U32  ii;/* iterator */
    CPSS_CSCD_LINK_TYPE_STC trunkLink;/* trunk link info */
    GT_U32  port;/* port iterator and start and end barriers */
    CPSS_TRUNK_MEMBER_STC   trunkMemberArr[1];/* trunk member array */
    GT_HW_DEV_NUM           virtualHwDevNum;/* virtual HW device number */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portsArr);

    if(numOfPorts < 1)
    {
        /* no meaning to give 0 ports or 1 port */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(trunkId == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* all source ports must be in same port group */
    portGroupId1 = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portsArr[0].srcPort);
    /* all trunk ports must be in same port group */
    portGroupId2 = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,portsArr[0].trunkPort);

    if(portGroupId1 == portGroupId2)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(portGroupId1 >= PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    for(ii = 1 ; ii < numOfPorts; ii++)
    {
        if(PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
            portsArr[ii].srcPort) != portGroupId1)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if(PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum,
            portsArr[ii].trunkPort) != portGroupId2)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* set the designated trunk ports table to support the specific src ports
       hashing */
    rc = prvCpssDxChTrunkDesignatedTableForSrcPortHashMappingSet(devNum,
        numOfPorts,portsArr,mode);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr == NULL)
    {
        PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr =
            cpssOsMalloc(MAX_TRUNKS_NUM_CNS * sizeof(GT_TRUNK_ID));

        if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        cpssOsMemSet(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr,0,
                     MAX_TRUNKS_NUM_CNS * sizeof(GT_TRUNK_ID));
    }


    for(ii = 0 ; ii < MAX_TRUNKS_NUM_CNS ; ii++)
    {
        if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr[ii] == trunkId)
        {
            /* index found */
            break;
        }
        else if(PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr[ii] == 0)
        {
            /* use first empty place */
            PRV_CPSS_DXCH_PP_PORT_GROUPS_DEBUG_INFO_PTR_MAC(devNum)->internalTrunksPtr[ii] = trunkId;
            break;
        }
    }

    if(ii == MAX_TRUNKS_NUM_CNS)
    {
        /* the DB is full ... */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
    }

    virtualHwDevNum = DESIGNATED_DEVICE_MAC(ii);

    /* set the 'trunk members' with SINGLE 'dummy device' so it will be accessing
       the device map table , to be directed back to this trunk.
       we do this 'ping-pong' in order to not modify the 'port group map table'
       so the application can manage changes in this table (change/add/remove)
       uplinks ports , to/from the 'src port trunk hashing'
    */
    trunkMemberArr[0].port = 0;/* dummy port */
    if(PRV_CPSS_IS_DUAL_HW_DEVICE_MAC(virtualHwDevNum) && (virtualHwDevNum & 1))
    {
        virtualHwDevNum &= ~1;/* clear LSBits --> make is even */
        trunkMemberArr[0].port += 64;
    }
    trunkMemberArr[0].hwDevice = virtualHwDevNum;/* the reserved device */
    rc = cpssDxChTrunkTableEntrySet(devNum,trunkId,1,trunkMemberArr);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* set the link type */
    trunkLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
    trunkLink.linkNum = trunkId;

    /*
        Note : the code is not optimized in terms of 'write to HW' but it
        optimized in term of needed code.
        So we not check the 'device map table access mode befor we call
        cpssDxChCscdDevMapTableSet '

        and we call it for all relevant permutations on the needed 'target device'
    */
    /* the 'port' supports only 6 bits , even for device with 128 physical ports */
    for(port = 0 ; port < BIT_6; port++)
    {
        /* set the device map table to point to the trunkId , with 'source port' hash */
        rc = cpssDxChCscdDevMapTableSet(devNum,
                        virtualHwDevNum,
                        0,/*not used*/
                        port,
                        &trunkLink,
                        CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E, GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, trunkId, numOfPorts, portsArr, mode));

    rc = internal_cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet(devNum, trunkId, numOfPorts, portsArr, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, trunkId, numOfPorts, portsArr, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChMultiPortGroupPortLookupEnableSet
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
static GT_STATUS internal_cpssDxChMultiPortGroupPortLookupEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS   rc;         /* return status    */
    GT_U32      regAddr;    /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM   localPort;/* local port - support multi-port-groups device */
    GT_U32  regBit;/* bit in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    if(localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        regBit = 15;
    }
    else
    {
        regBit = localPort;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookup0;

    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, regBit, 1, BOOL2BIT_MAC(enable));
    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupPortLookupEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChMultiPortGroupPortLookupEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChMultiPortGroupPortLookupEnableGet
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
static GT_STATUS internal_cpssDxChMultiPortGroupPortLookupEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT  GT_BOOL                *enablePtr
)
{
    GT_STATUS   rc;         /* return status    */
    GT_U32      regAddr;    /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM   localPort;/* local port - support multi-port-groups device */
    GT_U32  hwValue;    /* value read from the HW */
    GT_U32  regBit;/* bit in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);
    if(localPort == CPSS_CPU_PORT_NUM_CNS)
    {
        regBit = 15;
    }
    else
    {
        regBit = localPort;
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookup0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, regBit, 1, &hwValue);

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupPortLookupEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChMultiPortGroupPortLookupEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMultiPortGroupPortRingEnableSet
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
static GT_STATUS internal_cpssDxChMultiPortGroupPortRingEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
)
{
    GT_STATUS   rc;         /* return status                                  */
    GT_U32      regAddr;    /* register address                               */
    GT_U32      regAddr1;   /* register address 1                             */
    GT_U32      portGroupId;/* the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM       localPort;  /* local port - support multi-port-groups device */
    GT_U32      regBit;     /* bit in register                               */
    GT_U32      hwData;     /* data to write to hw                           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    hwData = BOOL2BIT_MAC(enable);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.coreRingConfiguration[localPort / 32];
    regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.coreRingPortEnable [localPort / 32];
    regBit = localPort%32;

    /* set HA register */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,regBit,1,hwData);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set TTI register */
    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId,regAddr1,regBit,1,hwData);

    return rc;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupPortRingEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChMultiPortGroupPortRingEnableSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMultiPortGroupPortRingEnableGet
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
static GT_STATUS internal_cpssDxChMultiPortGroupPortRingEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
)
{
    GT_STATUS   rc;         /* return status    */
    GT_U32      regAddr;    /* register address */
    GT_U32      regAddr1;    /* register address 1 */
    GT_U32      portGroupId;/* the port group Id - support multi-port-groups device */
    GT_PHYSICAL_PORT_NUM   localPort;/* local port - support multi-port-groups device */
    GT_U32      hwValue;    /* value read from the HW */
    GT_U32      hwValue1;   /* value 1 read from the HW */
    GT_U32      regBit;     /* bit in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->haRegs.coreRingConfiguration[localPort / 32];
    regAddr1 = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->ttiRegs.coreRingPortEnable [localPort / 32];
    regBit = localPort%32;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, regBit, 1, &hwValue);

    if(rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr1, regBit, 1, &hwValue1);

    if(rc != GT_OK)
    {
        return rc;
    }

    if(hwValue != hwValue1)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupPortRingEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChMultiPortGroupPortRingEnableGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




/*******************************************************************************
* internal_cpssDxChMultiPortGroupLookupNotFoundTableSet
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
static GT_STATUS internal_cpssDxChMultiPortGroupLookupNotFoundTableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PORT_GROUPS_BMP      portGroupsBmp,
    IN  GT_U32                  sourcePortGroupId,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC   *infoPtr
)
{
    GT_STATUS   rc;      /* return status    */
    GT_U32      regAddr; /* register address */
    GT_U32  portGroupId; /*the port group Id - support multi-port-groups device */
    GT_U32  hwValue;     /* value to write to the HW */
    GT_U32  hwValue1 = 0;/* value 1 to write to the HW */
    GT_U32  regBit;      /* bit in register */
    GT_U32    hwDev = 0, hwPort = 0;/*temp values of port,hwDevNum*/

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPSS_NULL_PTR_CHECK_MAC(infoPtr);

    if(sourcePortGroupId >= PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(infoPtr->unknownDaCommand)
    {
        case CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E:
            hwValue = 0;
            break;
        case CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E:
            hwValue = 1;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(infoPtr->nextRingInterface.type)
    {
        case CPSS_INTERFACE_PORT_E:
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(infoPtr->nextRingInterface.devPort.hwDevNum,
                                                   infoPtr->nextRingInterface.devPort.portNum);
            hwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(infoPtr->nextRingInterface.devPort.hwDevNum,
                                                         infoPtr->nextRingInterface.devPort.portNum);
            hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(infoPtr->nextRingInterface.devPort.hwDevNum,
                                                          infoPtr->nextRingInterface.devPort.portNum);

            if((hwPort > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_PHY_PORT_MAC(devNum)) ||
               (hwDev > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum)))
            {
                /* 8 bits for the port number and 12 bits for device number */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            break;
        case CPSS_INTERFACE_TRUNK_E:
            if(infoPtr->nextRingInterface.trunkId > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_TRUNK_ID_MAC(devNum))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
            }
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regBit = 16 + sourcePortGroupId;
    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookup0;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, regBit, 1 , hwValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* use multiCoreLookupInfo[0] as base address to the 16 registers of
           Multi-Core Lookup configuration.
           In each register the following bits are occupied.
           NextRingIsTrunk bit[0], NextRingTrgDev bits[1:12],
           NextRingTrgPort/Trunk bits[13;24]*/
        regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookupInfo[0] +
                   (0x4 * sourcePortGroupId);

        switch(infoPtr->nextRingInterface.type)
        {
            case CPSS_INTERFACE_PORT_E:
                U32_SET_FIELD_MAC(hwValue1,0,1,0); /* Next Ring Is NOT Trunk*/
                U32_SET_FIELD_MAC(hwValue1,1,12,hwDev);
                U32_SET_FIELD_MAC(hwValue1,13,8,hwPort);
                break;
            case CPSS_INTERFACE_TRUNK_E:
                U32_SET_FIELD_MAC(hwValue1,0,1,1); /* Next Ring Is Trunk*/
                U32_SET_FIELD_MAC(hwValue1,13,12,infoPtr->nextRingInterface.trunkId);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, 0, 25 , hwValue1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    }
    else
    {
        regBit = 13 * (sourcePortGroupId % 2);
        regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookupInfo[sourcePortGroupId/2];

        switch(infoPtr->nextRingInterface.type)
        {
            case CPSS_INTERFACE_PORT_E:
                U32_SET_FIELD_MAC(hwValue1,0,1,0); /* Next Ring Is NOT Trunk*/
                U32_SET_FIELD_MAC(hwValue1,1,7,hwPort);
                U32_SET_FIELD_MAC(hwValue1,8,5,hwDev);
                break;
            case CPSS_INTERFACE_TRUNK_E:
                U32_SET_FIELD_MAC(hwValue1,0,1,1); /* Next Ring Is Trunk*/
                U32_SET_FIELD_MAC(hwValue1,1,7,infoPtr->nextRingInterface.trunkId);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, regBit, 13 , hwValue1);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupLookupNotFoundTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, sourcePortGroupId, infoPtr));

    rc = internal_cpssDxChMultiPortGroupLookupNotFoundTableSet(devNum, portGroupsBmp, sourcePortGroupId, infoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, sourcePortGroupId, infoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChMultiPortGroupLookupNotFoundTableGet
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
static GT_STATUS internal_cpssDxChMultiPortGroupLookupNotFoundTableGet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  sourcePortGroupId,
    OUT  CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC   *infoPtr
)
{
    GT_STATUS   rc;         /* return status    */
    GT_U32      regAddr;    /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  hwValue;    /* value to write to the HW */
    GT_U32  regBit;/* bit in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    CPSS_NULL_PTR_CHECK_MAC(infoPtr);

    if(sourcePortGroupId >= PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    regBit = 16 + sourcePortGroupId;
    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookup0;

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, regBit, 1 , &hwValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(hwValue)
    {
        infoPtr->unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E;
    }
    else
    {
        infoPtr->unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* use multiCoreLookupInfo[0] as base address to the 16 registers of
           Multi-Core Lookup configuration.
           In each register the following bits are occupied.
           NextRingIsTrunk bit[0], NextRingTrgDev bits[1:12],
           NextRingTrgPort/Trunk bits[13;24]*/
        regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookupInfo[0] +
                   (0x4 * sourcePortGroupId);

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, 0, 25 , &hwValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(U32_GET_FIELD_MAC(hwValue,0,1))
        {
            /* trunk */
            infoPtr->nextRingInterface.type = CPSS_INTERFACE_TRUNK_E;
            infoPtr->nextRingInterface.trunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(hwValue,13,12);
        }
        else
        {
            /* dev,port */
            infoPtr->nextRingInterface.type = CPSS_INTERFACE_PORT_E;
            infoPtr->nextRingInterface.devPort.hwDevNum = U32_GET_FIELD_MAC(hwValue,1,12);
            infoPtr->nextRingInterface.devPort.portNum = U32_GET_FIELD_MAC(hwValue,13,8);

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&infoPtr->nextRingInterface);
        }

    }
    else
    {
        regBit = 13 * (sourcePortGroupId % 2);
        regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->eq.multiCoreLookup.multiCoreLookupInfo[sourcePortGroupId/2];

        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, regBit, 13 , &hwValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(U32_GET_FIELD_MAC(hwValue,0,1))
        {
            /* trunk */
            infoPtr->nextRingInterface.type = CPSS_INTERFACE_TRUNK_E;
            infoPtr->nextRingInterface.trunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(hwValue,1,7);
        }
        else
        {
            /* dev,port */
            infoPtr->nextRingInterface.type = CPSS_INTERFACE_PORT_E;
            infoPtr->nextRingInterface.devPort.hwDevNum = U32_GET_FIELD_MAC(hwValue,8,5);
            infoPtr->nextRingInterface.devPort.portNum = U32_GET_FIELD_MAC(hwValue,1,6);

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&infoPtr->nextRingInterface);
        }
    }


    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupLookupNotFoundTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, sourcePortGroupId, infoPtr));

    rc = internal_cpssDxChMultiPortGroupLookupNotFoundTableGet(devNum, portGroupsBmp, sourcePortGroupId, infoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, sourcePortGroupId, infoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
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
static GT_STATUS internal_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  sourcePortGroupId,
    IN  GT_BOOL enable
)
{
    GT_STATUS   rc;         /* return status    */
    GT_U32      regAddr;    /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  hwValue;    /* value to write to the HW */
    GT_U32  regBit;/* bit in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    if(sourcePortGroupId >= PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(sourcePortGroupId < 4)
    {
        regBit = 16 + sourcePortGroupId;
    }
    else
    {
        if(sourcePortGroupId >= 8)
        {
            /* not support more then 8 port groups */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* the Lion2 hold the port groups 4..7 in bits 24..27 */
        regBit = 24 + (sourcePortGroupId - 4);
    }

    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[2];

    hwValue = BOOL2BIT_MAC(enable);

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum,portGroupId, regAddr, regBit, 1 , hwValue);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)


    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, sourcePortGroupId, enable));

    rc = internal_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet(devNum, portGroupsBmp, sourcePortGroupId, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, sourcePortGroupId, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet
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
static GT_STATUS internal_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32  sourcePortGroupId,
    OUT  GT_BOOL *enablePtr
)
{
    GT_STATUS   rc;         /* return status    */
    GT_U32      regAddr;    /* register address */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  hwValue = 0;    /* value to write to the HW */
    GT_U32  regBit;/* bit in register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(sourcePortGroupId >= PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.numOfPortGroups)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(sourcePortGroupId < 4)
    {
        regBit = 16 + sourcePortGroupId;
    }
    else
    {
        if(sourcePortGroupId >= 8)
        {
            /* not support more then 8 port groups */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* the Lion2 hold the port groups 4..7 in bits 24..27 */
        regBit = 24 + (sourcePortGroupId - 4);
    }
    regAddr  = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bridgeRegs.bridgeGlobalConfigRegArray[2];

    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum,portGroupId, regAddr, regBit, 1 , &hwValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    *enablePtr = BIT2BOOL_MAC(hwValue);

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, sourcePortGroupId, enablePtr));

    rc = internal_cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet(devNum, portGroupsBmp, sourcePortGroupId, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, sourcePortGroupId, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChMultiPortGroupFdbModeSet
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
static GT_STATUS internal_cpssDxChMultiPortGroupFdbModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);

    switch(fdbMode)
    {
        case CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
            /* LionB : 32K*/
            /* Lion2 : 64K*/
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode =
                PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E;
            break;
        case CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
            /* LionB : 64K*/
            /* Lion2 : 128K , 256K*/
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode =
                PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E;
            break;
        case CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E:
            /* LionB : 128K*/
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode =
                PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupFdbModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, fdbMode));

    rc = internal_cpssDxChMultiPortGroupFdbModeSet(devNum, fdbMode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, fdbMode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChMultiPortGroupFdbModeGet
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
static GT_STATUS internal_cpssDxChMultiPortGroupFdbModeGet
(
    IN  GT_U8   devNum,
    OUT  CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT *fdbModePtr
)
{

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E | CPSS_XCAT2_E | CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_DEVICE_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(fdbModePtr);

    switch(PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode)
    {
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
            *fdbModePtr = CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E;
            break;
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
            *fdbModePtr = CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E;
            break;
        case PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E:
            *fdbModePtr = CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChMultiPortGroupFdbModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, fdbModePtr));

    rc = internal_cpssDxChMultiPortGroupFdbModeGet(devNum, fdbModePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, fdbModePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}




