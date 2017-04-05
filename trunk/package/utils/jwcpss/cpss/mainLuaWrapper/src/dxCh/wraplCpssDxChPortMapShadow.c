 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChPortMapShadow.c
*
* DESCRIPTION:
*       A lua wrapper for port map shadow.
*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssTypes.h>
#include <generic/private/prvWraplGeneral.h>

#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>  
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBobKResource.h>  
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/phy/prvCpssDxChSmiUnitDrv.h>

#include <stdlib.h>
#include <generic/private/prvWraplGeneral.h>
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

extern GT_STATUS prvCpssDxChCaelumPortResourseConfigGet
(
    IN  GT_U8                                       devNum,
    IN  GT_PHYSICAL_PORT_NUM                        portNum,
    OUT PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_STC      *resourcePtr
);



/*******************************************************************************
* wrlCpssDxChPortPhysicalPortMapShadowDBGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; error code and error message, if error occures.
*       12; GT_OK and port map shadows DB details.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS wrlCpssDxChPortPhysicalPortMapShadowDBGet
(
    IN lua_State            *L
)
{

    GT_STATUS                               status                  = GT_OK;
    GT_U8                                   devNum                 					=
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_PHYSICAL_PORT_NUM                    firstPhysicalPortNumber               	=
                                                    (GT_PHYSICAL_PORT_NUM) lua_tonumber(L, 2);
                                                                    /* firstPhysicalPortNumber */								
	
    static CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr = &portMapShadow;

    status = cpssDxChPortPhysicalPortDetailedMapGet(devNum, firstPhysicalPortNumber, /*OUT*/portMapShadowPtr);   

	if(status != GT_OK)
	{
		lua_pushinteger(L, status);
		return 1;
	}
	lua_pushinteger(L, status);
	lua_pushboolean(L, portMapShadowPtr->valid);
	lua_pushinteger(L, portMapShadowPtr->portMap.mappingType);
	lua_pushboolean(L, portMapShadowPtr->portMap.trafficManagerEn);
	lua_pushinteger(L, portMapShadowPtr->portMap.portGroup);
	lua_pushinteger(L, portMapShadowPtr->portMap.macNum);
	lua_pushinteger(L, portMapShadowPtr->portMap.rxDmaNum);
	lua_pushinteger(L, portMapShadowPtr->portMap.txDmaNum);
	lua_pushinteger(L, portMapShadowPtr->portMap.txFifoPortNum);
	lua_pushinteger(L, portMapShadowPtr->portMap.txqNum);
	lua_pushinteger(L, portMapShadowPtr->portMap.ilknChannel);
	lua_pushinteger(L, portMapShadowPtr->portMap.tmPortIdx);
	
    return 12;
}


/*******************************************************************************
* wrlCpssDxChCaelumTmPortResourceConfigGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; error code and error message, if error occures.
*       9; GT_OK and Caelum TM port resource config.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS wrlCpssDxChCaelumTmPortResourceConfigGet
(
    IN lua_State            *L
)
{

    GT_STATUS                               status                  = GT_OK;
    GT_U8                                   devNum             		=
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_PHYSICAL_PORT_NUM                    portNum	               	=
                                                    (GT_PHYSICAL_PORT_NUM) lua_tonumber(L, 2);
                                                                    /* firstPhysicalPortNumber */								
	
	PRV_CPSS_DXCH_CAELUM_PORT_RESOURCE_STC resource;
    status = prvCpssDxChCaelumPortResourseConfigGet(devNum, portNum, &resource);   

	if(status != GT_OK)
	{
		lua_pushinteger(L, status);
		return 1;
	}
	
	lua_pushinteger(L, status);
	lua_pushinteger(L, resource.txdmaCreditValue);
	lua_pushinteger(L, resource.txfifoHeaderCounterThresholdScdma);
	lua_pushinteger(L, resource.txfifoPayloadCounterThresholdScdma);
	lua_pushinteger(L, resource.txfifoScdmaPayloadThreshold);
	lua_pushinteger(L, resource.txfifoScdmaShiftersOutgoingBusWidth);
	lua_pushinteger(L, resource.rxdmaScdmaIncomingBusWidth);
	lua_pushinteger(L, resource.ethTxfifoOutgoingBusWidth);
	lua_pushinteger(L, resource.ethTxfifoScdmaPayloadThreshold);

    return 9;
}


/*******************************************************************************
* wrlCpssDxChBcat2PortResoursesStateGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; information table.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS wrlCpssDxChBcat2PortResoursesStateGet
(
    IN lua_State            *L
)
{
    GT_STATUS                               status                  = GT_OK;
    GT_U8                                   devNum             		=
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_PHYSICAL_PORT_NUM                    portNum	               	=
                                                    (GT_PHYSICAL_PORT_NUM) lua_tonumber(L, 2);
                                                                    /* firstPhysicalPortNumber */								
	GT_U32									index, t;

	CPSS_DXCH_BCAT2_PORT_RESOURCE_LIST_STC resList;
    status = prvCpssDxChBcat2PortResoursesStateGet(devNum, portNum, &resList);   
	index  = 0;
	if(status != GT_OK)
	{
		lua_newtable(L);
		t = lua_gettop(L);
		lua_pushinteger(L, status);
		lua_setfield(L, t, "status");
		lua_settop(L, t);
		return 1;
	}
	

    lua_newtable(L);
    t = lua_gettop(L);
	

    lua_pushnumber(L, status);
    lua_setfield(L, t, "status");
	
	lua_pushnumber(L, BC2_PORT_FLD_MAX);
    lua_setfield(L, t, "size");
	
	lua_pushnumber(L, resList.fldN);
    lua_setfield(L, t, "fldN");

	lua_newtable(L); 					/* array start */
	
	for(index = 0; index < BC2_PORT_FLD_MAX; index++)
	{
		lua_pushnumber(L, resList.arr[index].fldId); 
		lua_rawseti(L, -2, index * 3);
		lua_pushnumber(L, resList.arr[index].fldArrIdx); 
		lua_rawseti(L, -2, index * 3 + 1);
		lua_pushnumber(L, resList.arr[index].fldVal); 
		lua_rawseti(L, -2, index * 3 + 2);
	}
	
	lua_setfield(L, t, "arr");			/* array end */
	
	lua_settop(L, t);
	
    return 1;
}


/*******************************************************************************
* wrlCpssDxChBcat2SMIMappingDump
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; information table.
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS wrlCpssDxChBcat2SMIMappingDump
(
    IN lua_State            *L
)
{
    GT_STATUS                               rc	                    = GT_OK;
    GT_U8                                   devNum             		=
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U8				                    portGroupId              	=
                                                    (GT_U8) lua_tonumber(L, 2);
                                                                    /* portGroup */								
	GT_U32									inner_index, index, t, t2;

	char buffer[33];

	PRV_CPSS_DXCH_SMI_STATE_STC state;
	rc	= prvCpssDxChSMIStateGet(devNum, portGroupId, &state);
	
	index		= 0;
	inner_index = 0;
	if(rc != GT_OK)
	{
		lua_newtable(L);
		t = lua_gettop(L);
		lua_pushinteger(L, rc);
		lua_setfield(L, t, "status");
		lua_settop(L, t);
		return 1;
	}
    lua_newtable(L);
    t = lua_gettop(L);
	
    lua_pushnumber(L, rc);
    lua_setfield(L, t, "status");
	
	lua_pushnumber(L, CPSS_PHY_SMI_INTERFACE_MAX_E);
    lua_setfield(L, t, "CPSS_PHY_SMI_INTERFACE_MAX_E");
	
	lua_pushnumber(L, NUMBER_OF_PORTS_PER_SMI_UNIT_CNS);
    lua_setfield(L, t, "NUMBER_OF_PORTS_PER_SMI_UNIT_CNS");

	lua_newtable(L); 										/* array start */
	for(index = 0; index < CPSS_PHY_SMI_INTERFACE_MAX_E; index++)
	{
		lua_pushnumber(L, state.autoPollNumOfPortsArr[index]); 
		lua_rawseti(L, -2, index);
	}
	lua_setfield(L, t, "autoPollNumOfPortsArr");			/* array end */

	lua_newtable(L); 										/* array start */
	t2 = lua_gettop(L);

	for(index = 0; index < CPSS_PHY_SMI_INTERFACE_MAX_E; index++)
	{
		lua_newtable(L);											/* inner array start */
		for(inner_index = 0; inner_index < NUMBER_OF_PORTS_PER_SMI_UNIT_CNS; inner_index++)
		{
			lua_pushnumber(L, state.phyAddrRegArr[index][inner_index]); 
			lua_rawseti(L, -2,  inner_index);
		}
		cpssOsSprintf(buffer, "%d", index);
		lua_setfield(L, t2, buffer);								/* inner array end */
	}
	lua_setfield(L, t, "phyAddrRegArr");					/* array end */

	lua_settop(L, t);
    return 1;
}


