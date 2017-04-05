/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssCommon.c
*
* DESCRIPTION:
*       Provides wrappers for register access from lua and device number validation
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/


#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpss/generic/config/cpssGenCfg.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#include <stdlib.h>

#include <gtOs/gtOsTimer.h>
#include <lua.h>

int check_wraplCpssCommon()
{
    cpssOsPrintf("File wraplCpssCommon.c inside");
    return 1;
}

int wrlIsDevIdValid(
    lua_State *L
)
{
    lua_pushboolean(L,
            (int) prvCpssIsDevExists((GT_U8) lua_tointeger(L, 1)));
    return 1;
}

int wrlDevList(
    lua_State *L
)
{
    GT_U8 devId = 0xFF;
    int c = 0;

    lua_newtable(L);

    while (GT_OK == prvWrlDevFilterNextDevGet(devId, &devId))
    {
        lua_pushnumber(L, devId);
        lua_rawseti(L, -2, ++c);
    }
    return 1;
}

int wrlReadRegister(
    lua_State *L
)
{
    GT_U32      regData;
    GT_STATUS   rc;

    rc = prvCpssDrvHwPpReadRegister(
        (GT_8)lua_tointeger(L, 1),
        (GT_U32)lua_tointeger(L, 2),
        &regData);

    if (rc != GT_OK)
    {
        lua_pushfstring(L, "%d", rc);
        return 1;
    }

    lua_pushnumber(L, regData);
    return 1;
}

int wrlWriteRegister(
    lua_State *L
)
{
    GT_STATUS   rc;

    rc = prvCpssDrvHwPpWriteRegister(
        (GT_8)lua_tointeger(L, 1),
        (GT_U32)lua_tointeger(L, 2),
        (GT_U32)lua_tointeger(L, 3));

    lua_pushnumber(L, rc);
    return 1;
}
int wrlGetSystemTime(
    lua_State *L
)
{
    GT_U32  seconds;
    GT_U32  nanoseconds;
    lua_Number ret;

    if (osTimeRT(&seconds,&nanoseconds)!=GT_OK)
        lua_pushnil(L);
    else
    {
        ret =seconds+nanoseconds/1000000000.;
            lua_pushnumber(L,ret);
    }

    return 1;
}

/*******************************************************************************
* luaCLI_getTraceDBLine
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Reads a line from the trace database, either read or write and returns the
*       values to lua
*
* APPLICABLE DEVICES:
*        All devices.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state (in stack should be index of line and
*                              accessType for read or write)
*
* OUTPUTS:
*       1 Address of the write/read operation, nil if failed
*       2 Data of the write/read operation, nil if failed
*       3 The bitmap of the write/read operation, nil if failed
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int luaCLI_getTraceDBLine(lua_State *L)
{
    GT_U32 index;
    OUT APP_DEMO_CPSS_HW_ACCESS_DB_STC dbLine;

    if ((lua_gettop(L)==1) &&  (lua_isnumber(L, 1)))
    {
        index = (GT_U32) lua_tointeger(L, 1);
    }
    else
    {
            lua_pushnil(L);
            return 1;
    }

    if (appDemoTraceHwAccessDbLine(index,&dbLine)!=GT_OK)
    {
            lua_pushnil(L);
            return 1;
    }

    lua_pushnumber(L,dbLine.addr);
    lua_pushnumber(L,dbLine.data);
    lua_pushnumber(L,dbLine.accessParamsBmp);

/*
    if (data_high == 0)
        lua_pushnumber(L, data);
    else
    {
        char hexstring[20];
        cpssOsSprintf(hexstring, "0x%08x%08x", data_high, data);
        lua_pushstring(L, hexstring);
    }*/
    return 3;
}

extern GT_STATUS   osShellExecute(IN  char*   command);
/*******************************************************************************
* luaShellExecute
*
* DESCRIPTION:
*       allow to call any 'C' function from LUA.
*
* INPUTS:
*       L       - lua_State
*       [1] - the string to operate the command + it's parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 - error or function not found
*       1 - function was called
*
* COMMENTS:
*
*******************************************************************************/
int luaShellExecute(lua_State *L)
{
#ifndef CLI_STANDALONE
    const char      *commmand_and_params;
    char      *non_const_commmand_and_params;
    size_t          len;
    GT_STATUS       rc;

    if (! lua_isstring(L, 1))
    {
        /*error*/
        return 0;
    }

    commmand_and_params = lua_tolstring(L, 1, &len);

    cpssOsPrintf("luaShellExecute: %s \n",commmand_and_params);

    non_const_commmand_and_params = (char*)commmand_and_params;

    rc = osShellExecute(non_const_commmand_and_params);

    if (rc != GT_OK)
    {
        cpssOsPrintf("ERROR : non exist function : %s \n",commmand_and_params);
    }
    return (rc == GT_OK) ? 1 : 0;
#else
    return 0;
#endif /*CLI_STANDALONE*/
}

#ifndef CLI_STANDALONE
/*******************************************************************************
* luaConsoleClearScreen
*
* DESCRIPTION:
*       'clear screen' of the 'console application'
*
* INPUTS:
*       L       - lua_State
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 - ok
*      -1 - on error
*
* COMMENTS:
*
*******************************************************************************/
int luaConsoleClearScreen(lua_State *L)
{
    #ifdef _WIN32
        system("cls");
    #else
        int rc;
        rc = system("clear");
        if (rc == -1)
        {
            return rc;
        }
    #endif

    return 0;
}

static unsigned __TASKCONV myLuaThread
(
    IN GT_VOID * inputString
)
{
    GT_STATUS rc;
    char *non_const_commmand_and_params = (char *)inputString;

    cpssOsPrintf("STARTED (parallel run of): %s \n",non_const_commmand_and_params);

    rc = osShellExecute(non_const_commmand_and_params);
    if (rc != GT_OK)
    {
        cpssOsPrintf("ERROR : non exist function : %s \n",non_const_commmand_and_params);
    }

    cpssOsPrintf("ENDED (parallel run of): %s \n",non_const_commmand_and_params);

    cpssOsFree(non_const_commmand_and_params);

    return 0;
}

static GT_STATUS createMyThread(const char      *commmand_and_params)
{
    static GT_U32   threadId = 0;
    GT_TASK tid;
    static char myName[30];
    char *non_const_commmand_and_params;

    non_const_commmand_and_params = cpssOsMalloc(cpssOsStrlen(commmand_and_params) + 1);
    if(non_const_commmand_and_params == NULL)
    {
        cpssOsPrintf("createMyThread : failed no alloc [%d] bytes \n",cpssOsStrlen(commmand_and_params));
        return GT_FAIL;
    }

    cpssOsStrCpy(non_const_commmand_and_params,commmand_and_params);

    threadId++;

    cpssOsSprintf(myName,"luaParalel_%d",threadId);

    if(osTaskCreate(myName, 0 , 0x2000,
        myLuaThread,(GT_VOID*)non_const_commmand_and_params, &tid) != GT_OK)
    {
        return GT_FAIL;
    }

    return GT_OK;
}
#endif /*!CLI_STANDALONE*/

/*******************************************************************************
* luaShellExecute_inOtherThread
*
* DESCRIPTION:
*       allow to call any 'C' function from LUA , but called in new dedicated
*       thread for this operation
*
* INPUTS:
*       L       - lua_State
*       [1] - the string to operate the command + it's parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 - error or function not found
*       1 - function was called
*
* COMMENTS:
*
*******************************************************************************/
int luaShellExecute_inOtherThread(lua_State *L)
{
#ifndef CLI_STANDALONE
    GT_STATUS rc;
    const char      *commmand_and_params;
    size_t          len;

    if (! lua_isstring(L, 1))
    {
        /*error*/
        return 0;
    }

    commmand_and_params = lua_tolstring(L, 1, &len);

    rc = createMyThread(commmand_and_params);
    return (rc == GT_OK) ? 1 : 0;
#else
    return 0;
#endif /*CLI_STANDALONE*/
}



