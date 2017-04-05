/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChNst.c
*
* DESCRIPTION:
*       A lua wrapper for NST Port Isolation functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

use_prv_struct(CPSS_INTERFACE_INFO_STC)
use_prv_struct(CPSS_PORTS_BMP_STC)
use_prv_enum(CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)

extern void prvCpssDrvPortsFullMaskGet
(
    IN GT_U32                   numOfPorts,
    OUT CPSS_PORTS_BMP_STC       *portsBmpPtr
);

/*******************************************************************************
* wrlCpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Nst port isolation table entry set
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNstPortIsolationTableEntrySet
(
    IN lua_State *L
)
{
    GT_U8 devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC srcInterface;
    GT_BOOL cpuPortMember;
    CPSS_PORTS_BMP_STC  localPortsMembers;
    GT_STATUS              status = GT_OK;
    CPSS_PORTS_BMP_STC  existingPorts;

    PARAM_NUMBER(status, devNum,    1, GT_U8);
    PARAM_ENUM  (status,  trafficType,  2, CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT);
    PARAM_STRUCT(status, &srcInterface, 3,  CPSS_INTERFACE_INFO_STC);
    PARAM_BOOL(status, cpuPortMember, 4);
    PARAM_STRUCT(status, &localPortsMembers, 5,  CPSS_PORTS_BMP_STC);

    if (status != GT_OK)
    {
        lua_pushinteger(L, (lua_Integer)status);
        return 1;
    }

    /* get the bmp of supported port on this device */
    if(!PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        prvCpssDrvPortsFullMaskGet(PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts,
                               &existingPorts);
    }
    else
    {
        prvCpssDrvPortsFullMaskGet(
            PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(devNum) ,
                &existingPorts);
    }


    /* (&localPortsMembers) = (&localPortsMembers) & (&existingPorts) */
    CPSS_PORTS_BMP_BITWISE_AND_MAC(&localPortsMembers,&localPortsMembers,&existingPorts);

    status = cpssDxChNstPortIsolationTableEntrySet(
        devNum,
        trafficType,
        srcInterface,
        cpuPortMember,
        &localPortsMembers);

    lua_pushinteger(L, (lua_Integer)status);
    return 1;
}


/*******************************************************************************
* wrlCpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Nst port isolation delete
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNstPortIsolationPortDelete
(
    IN lua_State *L
)
{
    GT_U8 devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT trafficType;
    CPSS_INTERFACE_INFO_STC srcInterface;
    GT_PHYSICAL_PORT_NUM  portNum;
    GT_STATUS              status = GT_OK;

    PARAM_NUMBER(status, devNum,        1, GT_U8);
    PARAM_ENUM  (status,  trafficType,  2, CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT);
    PARAM_STRUCT(status, &srcInterface, 3,  CPSS_INTERFACE_INFO_STC);
    PARAM_NUMBER(status, portNum,       4,  GT_PHYSICAL_PORT_NUM);

    if (status != GT_OK)
    {
        lua_pushinteger(L, (lua_Integer)status);
        return 1;
    }

    status = cpssDxChNstPortIsolationPortDelete(
        devNum,
        trafficType,
        srcInterface,
        portNum);

    lua_pushinteger(L, (lua_Integer)status);
    return 1;
}

/*******************************************************************************
* wrlCpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Nst port isolation delete
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
*
* OUTPUTS:
*       cpuPortMember        - is it cpu port member
*       localPortsMembers    - port members bitmap
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNstPortIsolationTableEntryGet
(
    IN lua_State *L
)
{
    GT_U8 devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC srcInterface;
    GT_BOOL                 cpuPortMember;
    CPSS_PORTS_BMP_STC      localPortsMembers;
    GT_STATUS               status = GT_OK;

    cmdOsMemSet(&localPortsMembers,     0, sizeof(localPortsMembers));

    PARAM_NUMBER(status, devNum,        1, GT_U8);
    PARAM_ENUM  (status, trafficType,   2, CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT);
    PARAM_STRUCT(status, &srcInterface, 3, CPSS_INTERFACE_INFO_STC);

    if (status != GT_OK)
    {
        lua_pushinteger(L, (lua_Integer)status);
        return 1;
    }

    status = cpssDxChNstPortIsolationTableEntryGet(
        devNum,
        trafficType,
        srcInterface,
        &cpuPortMember,
        &localPortsMembers
    );

    lua_pushnumber(L, status);
    lua_pushboolean(L, cpuPortMember);
    prv_c_to_lua_CPSS_PORTS_BMP_STC(L, &localPortsMembers);
    return 3;
}
