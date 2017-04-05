/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChPortControl.c
*
* DESCRIPTION:
*       port control functions wrapper
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/
#include <string.h>
#include <generic/private/prvWraplGeneral.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterProfileStorage.h>

#include <cpssCommon/wraplCpssExtras.h>
#include <cpssCommon/wrapCpssDebugInfo.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>


int luaTop_GetInteger(IN lua_State *L, IN char * name)
{
    int I;
    lua_getfield(L,-1,name);
    I = (GT_U32)lua_tonumber(L,-1);
    lua_pop(L, 1);
    return I;
}

char * luaTop_GetString(IN lua_State *L, IN char * name)
{
    char * token;
    lua_getfield(L,-1,name);
    token = (char *)lua_tostring(L,-1); 
    lua_pop(L, 1);  /* cpu */

    return token;
}

int luaTop_ArrU8Get(IN lua_State *L, IN char * name, int maxSize, OUT GT_U8 *arrPtr)
{
    int id;

    lua_getfield(L,-1,name);
        lua_pushnil(L);  /* first key */
        id = 0;
        while (lua_next(L, -2) != 0) 
        {
            if (id == maxSize)
            {
                return -1;
            }
            arrPtr[id++] = (GT_U8)lua_tointeger(L,-1);
            lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
        }
    lua_pop(L, 1);

    return id;
}

int luaTop_ArrU8Get_woName(IN lua_State *L, int maxSize, OUT GT_U8 *arrPtr)
{
    int id;

    lua_pushnil(L);  /* first key */
    id = 0;
    while (lua_next(L, -2) != 0) 
    {
        if (id == maxSize)
        {
            return -1;
        }
        arrPtr[id++] = (GT_U8)lua_tointeger(L,-1);
        lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
    }

    return id;
}


/*------------------------*/

GT_U32 luaTop_CoreBmpBuild
(
    IN lua_State *L, 
    IN char * name
)
{
     GT_U8 coreList[32];
     int   size;
     int   i;
     GT_U32 coreBmp = 0;

     size = luaTop_ArrU8Get(L,name,32,&coreList[0]);
     for (i = 0 ; i < size; i++)
     {
         coreBmp = coreBmp | (1 << coreList[i]);
     }
     return coreBmp;
}

GT_U32 luaTop_TableSizeGet_woName
(
    IN lua_State *L
)
{
    GT_U32 id;
    lua_pushnil(L);  /* first key */
    id = 0;
    while (lua_next(L, -2) != 0) 
    {
        id++;
        lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
    }
    return id;
}


GT_U32 luaTop_TableSizeGet
(
    IN lua_State *L, 
    IN char * name
)
{
    GT_U32 id;
    lua_getfield(L,-1,name);
        lua_pushnil(L);  /* first key */
        id = 0;
        while (lua_next(L, -2) != 0) 
        {
            id++;
            lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
        }
    lua_pop(L, 1);

    return id;
}


/*---------------------------------------------------*/
/* config                                            */
/*---------------------------------------------------*/
void prvWrlDxChPortPizzaArbiterIfProfileConfig
(
    IN lua_State *L, 
    OUT CPSS_DXCH_PIZZA_PROFILE_STC * profilePtr
)
{
    char * token;
    lua_getfield(L,-1,"config");

        profilePtr->portNum     =  (GT_U32)luaTop_GetInteger(L,"total_ports");
        profilePtr->maxSliceNum =  (GT_U32)luaTop_GetInteger(L,"total_slices");


        lua_getfield(L,-1,"txqDef");
         
            profilePtr->txQDef.pizzaRepetitionNum = (GT_U32)luaTop_GetInteger(L,"TxQ_repetition");

            lua_getfield(L,-1,"cpu");

                token = luaTop_GetString(L,"type");

                profilePtr->txQDef.cpuPortDef.type = CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E;
                profilePtr->txQDef.cpuPortDef.cpuPortNum = CPSS_PA_INVALID_PORT;
                profilePtr->txQDef.cpuPortDef.cpuPortSlices[0] = CPSS_PA_INVALID_SLICE;
                if (0 != strcmp(token,"none"))
                {
                    profilePtr->txQDef.cpuPortDef.cpuPortNum = luaTop_GetInteger(L,"port");

                    if (0 == strcmp(token,"fixed"))
                    {
                        int sliceNum;

                        profilePtr->txQDef.cpuPortDef.type = CPSS_DXCH_CPUPORT_TYPE_FIXED_E;

                        sliceNum = luaTop_ArrU8Get(L,"sliceList"
                                                        ,sizeof(profilePtr->txQDef.cpuPortDef.cpuPortSlices)/sizeof(profilePtr->txQDef.cpuPortDef.cpuPortSlices[0])
                                                        ,/*OUT*/&profilePtr->txQDef.cpuPortDef.cpuPortSlices[0]);
                        profilePtr->txQDef.cpuPortDef.cpuPortSlices[sliceNum] = CPSS_PA_INVALID_SLICE;
                    }
                    else if (0 == strcmp(token,"external"))
                    {
                        profilePtr->txQDef.cpuPortDef.type = CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E;
                    }
                }
            lua_pop(L, 1);  /* cpu */
        lua_pop(L, 1);  /* txqDef */
    lua_pop(L, 1); /* config */
}


/*----------------------------------------------------------------*/
/* port priority                                                  */
/*----------------------------------------------------------------*/

void prvWrlDxChPortPizzaArbiterIfProfilePortPriority
(
    IN lua_State *L, 
    IN GT_U32 total_ports,
    OUT CPSS_DXCH_PORT_PRIORITY_MATR_STC * portPriorityMatrPtr
)
{
    GT_U32 id;
    int len;
    portPriorityMatrPtr->portN = total_ports;

    lua_getfield(L,-1,"port_priority");
        id = 0;
        lua_pushnil(L);  /* first key */
        while (lua_next(L, -2) != 0) 
        {
            len = sizeof(portPriorityMatrPtr->portPriorityList[id])/sizeof(portPriorityMatrPtr->portPriorityList[id][0]);
            len = luaTop_ArrU8Get_woName(L, len, /*OUT*/ &portPriorityMatrPtr->portPriorityList[id][0]);
            portPriorityMatrPtr->portPriorityList[id][len] = CPSS_PA_INVALID_PORT;

            id++;
            lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
        }
    lua_pop(L, 1); /* port_priority */
}

/*----------------------------------------------------------------*/
/* allocationn                                                    */
/*----------------------------------------------------------------*/
void prvWrlDxChPortPizzaArbiterIfProfileAllocationType_SliceList
(
    IN  lua_State *L,
    IN  GT_U32 total_ports,
    OUT CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * sliceListPtr
)
{
    GT_U32 id;
    GT_U32 portId;
    GT_U32 len;

    sliceListPtr->maxPortNum = total_ports;

    lua_getfield(L,-1,"allocationList");
    id = 0;
    lua_pushnil(L);  /* first key */
    while (lua_next(L, -2) != 0) 
    {
        portId = luaTop_GetInteger(L,"portId");
        
        len = luaTop_ArrU8Get(L,"slicesList",
                              sizeof(sliceListPtr->portConfArr[portId])/sizeof(sliceListPtr->portConfArr[portId][0]),
                              &sliceListPtr->portConfArr[portId][0]);
        sliceListPtr->portConfArr[portId][len] = CPSS_PA_INVALID_PORT;
        lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
        id++;
    }
    lua_pop(L, 1); /* allocationList */
}

void prvWrlDxChPortPizzaArbiterIfProfileAllocationType
(
    IN  lua_State *L, 
    char * allocationTypePtr,
    IN  GT_U32 total_ports,
    IN  GT_U32 total_slices,
    INOUT PortPizzaCfgSlicesListPool_STC  * portPizzaCfgSliceListPoolPtr,
    OUT CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pizzaPtr
)
{
    GT_U32 id;
    GT_U32 sliceN;
    CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC * pizzaCfgPtr;

    pizzaPtr->portNum     = total_ports;
    pizzaPtr->totalSlices = total_slices;

    lua_getfield(L,-1,allocationTypePtr);
    lua_pushnil(L);  /* first key */
    id = 0;
    while (lua_next(L, -2) != 0) 
    {
        sliceN    = luaTop_GetInteger(L, "sliceN");
        portPizzaCfgSlicesListPoolAlloc(portPizzaCfgSliceListPoolPtr,/*OUT*/&pizzaCfgPtr);
        pizzaPtr->pizzaCfg[id].sliceNum2Config = sliceN;

        prvWrlDxChPortPizzaArbiterIfProfileAllocationType_SliceList(L,total_ports,/*OUT*/pizzaCfgPtr);
        pizzaPtr->pizzaCfg[id].pizzaPtr        = pizzaCfgPtr;


        lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
        id++;
    }
    lua_pop(L, 1); /* allocationType : reg|ext */
}



void prvWrlDxChPortPizzaArbiterIfProfileAllocation
(
    IN    lua_State *L, 
    INOUT PortPizzaCfgSlicesListPool_STC  * portPizzaCfgSliceListPoolPtr,
    INOUT CPSS_DXCH_PIZZA_PROFILE_STC * profilePtr
)
{
    lua_getfield(L,-1,"allocation");
        prvWrlDxChPortPizzaArbiterIfProfileAllocationType(L,"reg",profilePtr->portNum,profilePtr->maxSliceNum,
                                                          /*INOUT*/portPizzaCfgSliceListPoolPtr,
                                                          /*INOUT*/profilePtr->regModePizzaPtr);
        prvWrlDxChPortPizzaArbiterIfProfileAllocationType(L,"ext",profilePtr->portNum,profilePtr->maxSliceNum,
                                                          /*INOUT*/portPizzaCfgSliceListPoolPtr,
                                                          /*INOUT*/profilePtr->extModePizzaPtr);
    lua_pop(L, 1); /* allocation */
}


/*----------------------------------------------------------------*/
/* port policy                                                    */
/*----------------------------------------------------------------*/
typedef struct PRV_WRL_DXCH_U32_SPEED_STCS
{
    GT_U32 speed;
    CPSS_PORT_SPEED_ENT speedEnm;
}PRV_WRL_DXCH_U32_SPEED_STC;

static PRV_WRL_DXCH_U32_SPEED_STC prvWrlDxChSpeed2Enm[] = 
{
     {   1,  CPSS_PORT_SPEED_1000_E  }
    ,{   5,  CPSS_PORT_SPEED_5000_E  }
    ,{  10,  CPSS_PORT_SPEED_10000_E }
    ,{  12,  CPSS_PORT_SPEED_12000_E }
    ,{  15,  CPSS_PORT_SPEED_15000_E }
    ,{  16,  CPSS_PORT_SPEED_16000_E }
    ,{  20,  CPSS_PORT_SPEED_20000_E }
    ,{  40,  CPSS_PORT_SPEED_40000_E }
    ,{  50,  CPSS_PORT_SPEED_50000_E }
    ,{  75,  CPSS_PORT_SPEED_75000_E }
    ,{ 100,  CPSS_PORT_SPEED_100G_E  }
};

GT_STATUS prvWrlDxChPortPizzaArbiterIfProfilePortPolicy_ConvertSpeed2Enm
(
    IN   GT_U32 speed,
    OUT  CPSS_PORT_SPEED_ENT * speedEnmPtr
)
{
    GT_U32 i;

    if (speedEnmPtr == NULL)
    {
        return GT_BAD_PTR;
    }
    for (i = 0 ; i < sizeof(prvWrlDxChSpeed2Enm)/sizeof(prvWrlDxChSpeed2Enm[0]); i++)
    {
        if (prvWrlDxChSpeed2Enm[i].speed == speed)
        {
            *speedEnmPtr = prvWrlDxChSpeed2Enm[i].speedEnm;
            return GT_OK;
        }
    }
    *speedEnmPtr = CPSS_PORT_SPEED_NA_E;
    return GT_NOT_FOUND;
}

GT_BOOL prvWrlDxChPortPizzaArbiterIfProfilePortPolicy_Specific
(
    IN lua_State *L, 
    IN GT_U32 arrMaxSize,
    OUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * arrPtr
)
{
    GT_STATUS rc;
    GT_U32 id;
    GT_U32 speed;
    GT_U32 sliceNum;
    CPSS_PORT_SPEED_ENT speedEnm;

    lua_pushnil(L);  /* first key */
    id = 0;
    while (lua_next(L, -2) != 0) 
    {
        if (id == arrMaxSize)
        {
            return GT_FALSE;
        }
        speed    = luaTop_GetInteger(L, "speed");
        sliceNum = luaTop_GetInteger(L, "slicesNum");

        rc = prvWrlDxChPortPizzaArbiterIfProfilePortPolicy_ConvertSpeed2Enm(speed,/*OUT*/&speedEnm);
        if (rc != GT_OK)
        {
            return GT_FALSE;
        }
        arrPtr[id].speed     = speedEnm;
        arrPtr[id].slicesNum = sliceNum;

        lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
        id++;
    }
    return GT_TRUE;
}

void prvWrlDxChPortPizzaArbiterIfPortPolicy
(
    IN    lua_State *L, 
    INOUT PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC * policyPoolPtr,
    OUT   CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * speedConvTablePtr
)
{
    GT_U32 portId;
    GT_U32 len;
    GT_U32 portNum;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *itemListPtr;

    portNum = luaTop_TableSizeGet(L,"port_policy");

    lua_getfield(L,-1,"port_policy");
        lua_pushnil(L);  /* first key */
        portId = 0;
        while (lua_next(L, -2) != 0) 
        {
            len = luaTop_TableSizeGet_woName(L);
            portSpeed2SliceNumPoolAlloc(policyPoolPtr,len+1,/*OUT*/&itemListPtr);

            speedConvTablePtr->arr[portId].portN = portId;
            speedConvTablePtr->arr[portId].portSpeed2SliceNumListPtr = itemListPtr;
            prvWrlDxChPortPizzaArbiterIfProfilePortPolicy_Specific(L,len,itemListPtr);

            lua_pop(L, 1); /* removes 'value'; keeps 'key' for next iteration */
            portId++;
        }
    lua_pop(L, 1); /* config */

    speedConvTablePtr->arr[portNum].portN                     = CPSS_PA_INVALID_PORT;
    speedConvTablePtr->arr[portNum].portSpeed2SliceNumListPtr = (CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC *)NULL;
}

/* 
typedef struct PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STCT
{
    GT_U32                    cur;
    GT_U32                    remains;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC    arr[150];
}PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC;
*/

/*-------------------------------------------------------------------
**    Interface to the structure PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC
**------------------------------------------------------------------*/
/*
GT_STATUS portSpeed2SliceNumPoolInit (INOUT PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC * pPool);
GT_STATUS portSpeed2SliceNumPoolAlloc(INOUT PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC * pPool, 
                                         IN GT_U32 size, 
                                        OUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC ** pItemList);
*/


typedef struct PizzaArbiterLuaToCConvMemory_STCT
{
    /*---------------------------------------------------------------------*/
    /* usefull data structures for policy (speed 2 slice) conversion table */
    /*---------------------------------------------------------------------*/
    CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC   portSpeed2SlicesConvTbl;
    PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC     portSpeed2SlicesConvPool;
    /*------------------------------------------*/
    /* usefull data structures for PizzaProfile */
    /*------------------------------------------*/
    CPSS_DXCH_PIZZA_PROFILE_STC                            profile;
    CPSS_DXCH_PORT_PRIORITY_MATR_STC                       portPriorityMatr;
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC        regModePizza;
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC        extModePizza;
    PortPizzaCfgSlicesListPool_STC                         portPizzaCfgSliceListPool;
}PizzaArbiterLuaToCConvMemory_STC;


static GT_STATUS PizzaArbiterLuaCConvMemory_Init
(
    PizzaArbiterLuaToCConvMemory_STC * memPtr
)
{
    GT_STATUS rc;

    /*---------------------------------------------------------------------*/
    /* usefull data structures for policy (speed 2 slice) conversion table */
    /*---------------------------------------------------------------------*/
    rc = portGroupSpeed2SliceNumListInit(&memPtr->portSpeed2SlicesConvTbl);
    if (GT_OK != rc)
    {
        return rc;
    }
    rc = portSpeed2SliceNumPoolInit(&memPtr->portSpeed2SlicesConvPool);
    if (GT_OK != rc)
    {
        return rc;
    }
    /*------------------------------------------*/
    /* usefull data structures for PizzaProfile */
    /*------------------------------------------*/
    rc = pizzaProfileInit(&memPtr->profile);
    if (GT_OK != rc) 
    {
        return rc;
    }
    rc = portPizzaCfgSliceListBySlicesNumInit(&memPtr->regModePizza,0,0);
    if (GT_OK != rc) 
    {
        return rc;
    }
    rc = portPizzaCfgSliceListBySlicesNumInit(&memPtr->extModePizza,0,0);
    if (GT_OK != rc) 
    {
        return rc;
    }
    rc = portPizzaCfgSlicesListPoolInit      (&memPtr->portPizzaCfgSliceListPool);
    if (GT_OK != rc) 
    {
        return rc;
    }
    rc = portPrioritiesMatrixInit            (&memPtr->portPriorityMatr);
    if (GT_OK != rc) 
    {
        return rc;
    }

    memPtr->profile.pPortPriorityMatrPtr = &memPtr->portPriorityMatr;
    memPtr->profile.regModePizzaPtr      = &memPtr->regModePizza;
    memPtr->profile.extModePizzaPtr      = &memPtr->extModePizza;

    return GT_OK;

}

PizzaArbiterLuaToCConvMemory_STC wrlPaIfUserTblMem1;

int wrlDxChPortPizzaArbiterIfUserTableSet
(
    IN lua_State *L
)
{
    int index;

    GT_STATUS rc;
    GT_U32 coreBmp;
    GT_U8  dev;
    int n;
    PizzaArbiterLuaToCConvMemory_STC * wrlPaIfUserTblMemPtr = &wrlPaIfUserTblMem1;

    /* 
    lua_getfield(L, Lindex, #field); 
    if (lua_type(L, -1) == LUA_TNUMBER) 

    lua_getfield()
    */

     rc = GT_OK;
     n = lua_gettop(L);
     if (n != 1 || lua_type(L, 1) != LUA_TTABLE)
     {
         cpssOsPrintf("FloatArray expects first parameter to be a table");
         return 0;
     }
     /* a = (int*)lua_newuserdata(L, tablesize * sizeof(int)); */
     lua_pushnil(L);
     index = 1;
     lua_pushnil(L);  /* first key */
     while (lua_next(L,  index) != 0) 
     {
         /* uses 'key' (at index -2) and 'value' (at index -1) */
         /*
         cpssOsPrintf("%s - %s\n",
                lua_typename(L, lua_type(L, -2)),
                lua_typename(L, lua_type(L, -1)));
         */
         /*-----------------*/
         /* config          */
         /*-----------------*/
             rc = PizzaArbiterLuaCConvMemory_Init(wrlPaIfUserTblMemPtr);

             dev     = (GT_U8)luaTop_GetInteger(L,"device");
             coreBmp = (GT_U32)luaTop_CoreBmpBuild(L,"coreList");
             prvWrlDxChPortPizzaArbiterIfPortPolicy(L,&wrlPaIfUserTblMemPtr->portSpeed2SlicesConvPool,
                                                     /*OUT*/&wrlPaIfUserTblMemPtr->portSpeed2SlicesConvTbl);

             prvWrlDxChPortPizzaArbiterIfProfileConfig      (L,/*OUT*/&wrlPaIfUserTblMemPtr->profile);
             prvWrlDxChPortPizzaArbiterIfProfilePortPriority(L,wrlPaIfUserTblMemPtr->profile.portNum,/*OUT*/&wrlPaIfUserTblMemPtr->portPriorityMatr);

             prvWrlDxChPortPizzaArbiterIfProfileAllocation  (L,&wrlPaIfUserTblMemPtr->portPizzaCfgSliceListPool,
                                                               /*INOUT*/&wrlPaIfUserTblMemPtr->profile);

             rc = cpssDxChPortPizzaArbiterIfUserTableSet(dev,coreBmp,&wrlPaIfUserTblMemPtr->profile,&wrlPaIfUserTblMemPtr->portSpeed2SlicesConvTbl);
             if (rc != GT_OK)
             {
                 break;
             }

         /* removes 'value'; keeps 'key' for next iteration */
         lua_pop(L, 1);
     }
     lua_pushnumber(L,rc);
     return 1;
}

/*
int wrlDxChPortPizzaArbiterIfUserTableGet
(
    IN lua_State *L
)
{
    GT_STATUS rc;

    lua_pushnumber(L,rc);
    return 1;
}
*/



