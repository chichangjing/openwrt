/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaCLIEngine_genWrapper.c
*
* DESCRIPTION:
*       a generic wrapper to CPSS APIs
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/

#define CPSS_GENWRAPPER_ENGINE
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <extUtils/luaCLI/luaCLIEngine.h>

#define MAX_PARAMS 30
#define MAX_TYPE_LENGTH 30

typedef enum
{
    CPSS_API_PARAMETER_TYPE_IN,
    CPSS_API_PARAMETER_TYPE_OUT,
    CPSS_API_PARAMETER_TYPE_INOUT,
    CPSS_API_PARAMETER_TYPE_WRONG
} CPSS_API_PARAMETER_TYPE;

/*************************************************************************
* mgmType_to_c_int
* mgmType_to_c_GT_U8
* mgmType_to_c_GT_U16
* mgmType_to_c_GT_U32
*
* Description:
*       Convert to integer/GT_U8/GT_U16/GT_U32
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_c_int(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        lua_pushnumber(L, 0);
    else if (!lua_isnumber(L, 1))
        lua_pushnil(L);
    else
        lua_pushvalue(L, 1);
    return 1;
}

/*************************************************************************
* mgmType_to_lua_int
* mgmType_to_lua_GT_U8
* mgmType_to_lua_GT_U16
* mgmType_to_lua_GT_U32
*
* Description:
*       Convert integer/GT_U8/GT_U16/GT_U32 to number
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_lua_int(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isnumber(L, 1))
        lua_pushnil(L);
    else
        lua_pushvalue(L, 1);
    return 1;
}

/*************************************************************************
* mgmType_to_c_bool
* mgmType_to_c_GT_BOOL
*
* Description:
*       Convert to integer(GT_BOOL)
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_c_bool(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        lua_pushnumber(L, 0);
    else if (lua_isboolean(L, 1))
        lua_pushnumber(L, lua_toboolean(L, 1));
    else if (lua_isnumber(L, 1))
        lua_pushnumber(L, lua_tointeger(L, 1) == 0 ? 0 : 1);
    else
        lua_pushnumber(L, 0);
    return 1;
}

/*************************************************************************
* mgmType_to_lua_bool
* mgmType_to_lua_GT_BOOL
*
* Description:
*       Convert integer(GT_BOOL) to boolean
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  number if successful
*
*************************************************************************/
static int mgmType_to_lua_bool(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_isnumber(L, 1))
        lua_pushnil(L);
    else
        lua_pushboolean(L, lua_tointeger(L, 1) == 0 ? 0 : 1);
    return 1;
}

/*************************************************************************
* string_equal_to
*
* Description:
*       Compare string at the top of Lua's stack with constant string
*
* Parameters:
*       string at the top of
*
* Returns:
*  Nonzero if strings equal
*  zero if not equal
*
*************************************************************************/
static int string_equal_to(lua_State *L, const char *literal)
{
    int result;
    lua_pushstring(L, literal);
    result = lua_equal(L, -1, -2);
    lua_pop(L, 1);
    return result;
}
/*************************************************************************
* interpretParameterType
*
* Description:
*       Convert parameter type ("IN", "OUT", "INOUT") to enum
*
* Parameters:
*       string at the top of
*
* Returns:
*  CPSS_API_PARAMETER_TYPE_WRONG if error
*  enum value if successful
*
*************************************************************************/
static CPSS_API_PARAMETER_TYPE interpretParameterType(lua_State *L)
{
    CPSS_API_PARAMETER_TYPE ret = CPSS_API_PARAMETER_TYPE_WRONG;
    if (string_equal_to(L, "IN"))
        ret = CPSS_API_PARAMETER_TYPE_IN;
    if (string_equal_to(L, "OUT"))
        ret = CPSS_API_PARAMETER_TYPE_OUT;
    if (string_equal_to(L, "INOUT"))
        ret = CPSS_API_PARAMETER_TYPE_INOUT;
    lua_pop(L, 1);
    return ret;
}
/*************************************************************************
* cpssGenWrapper
*
* Description:
*       Call CPSS API function
*
* Parameters:
*  cpssAPIname     - Fx: "prvCpssDrvHwPpReadRegister"
*  params          - table:
*                    each row is table of 3 elements:
*                    1. direction (IN | OUT | INOUT)
*                    2. parameter type
*                       the valid types:
*                       "int"
*                       "bool"
*                       "GT_U8",
*                       "GT_U16",
*                       "GT_U32",
*                       "string"
*                       "custom_type_name" with a set of functions:
*                          mgmType_to_c_${custom_type_name}(param)
*                                  - convert lua variable/table to
*                                    userdata object
*                          mgmType_to_lua_${custom_type_name}(param)
*                                  - convert userdata object to lua
*                    3. parameter name
*                    4. parameter value(optional, for IN and INOUT only)
*  noLock          - if true then don't protect call with mutex
*
*
* Returns:
*  GT_STATUS, array of output values if successful
*  string if error
*
*
* Example
* status, values = cpssGenWrapper(
*                      "prvCpssDrvHwPpReadRegister", {
*                           { "IN", "GT_U8", "devNum", 0 },
*                           { "IN", "GT_U32","regAddr", 0x200000 },
*                           { "OUT","GT_U32","regData" }
*                      })
* regData = values.regData
*************************************************************************/
static int cpssGenWrapper(
    lua_State *L
)
{
    GT_UINTPTR              paramsArray[MAX_PARAMS];
    GT_U32                  valuesArray[MAX_PARAMS];
    CPSS_API_PARAMETER_TYPE ptypesArray[MAX_PARAMS];
    int                     vtypesArray[MAX_PARAMS];
    GT_CHAR                 utypesArray[MAX_PARAMS][MAX_TYPE_LENGTH];
    GT_STATUS               retval;
    typedef GT_STATUS (*CPSS_API_FUNC_PTR)(GT_UINTPTR, ...);
    CPSS_API_FUNC_PTR funcPtr;
    int nparams;
    int k,values;
    int kv;
    int doLock = 1;
    LUA_CLI_GETFUNCBYNAME_T  getFunctionByName;

    getFunctionByName = (LUA_CLI_GETFUNCBYNAME_T)
            luaCLIEngineGetData(L, "_getFunctionByName", 1);
    if (getFunctionByName == NULL)
    {
        lua_pushliteral(L, "_getFunctionByName not defined");
        return 1;
    }
    if (lua_gettop(L) < 2)
    {
        lua_pushliteral(L, "no enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "bad parameter 1");
        return 1;
    }
    if (!lua_istable(L, 2))
    {
        lua_pushliteral(L, "bad parameter 2");
        return 1;
    }
    if (lua_gettop(L) > 2 && lua_isboolean(L,3) && lua_toboolean(L,3))
    {
        doLock = 0;
    }

    lua_settop(L, 2); /* remove extra parameters */

    funcPtr = (CPSS_API_FUNC_PTR) getFunctionByName(lua_tostring(L, 1));
    if (!funcPtr)
    {
        lua_pushliteral(L, "function '");
        lua_pushvalue(L, 1);
        lua_pushliteral(L, "' not found");
        lua_concat(L, 3);
        return 1;
    }

    cpssOsMemSet(paramsArray, 0, sizeof(paramsArray));
    /* prepare parameters */
    nparams = (int)lua_objlen(L, 2);
    for (k = kv = 0; k < nparams; k++, kv++)
    {
        int param;
        lua_rawgeti(L, 2, k+1); /* params[k+1] */
        param = lua_gettop(L); /* param = params[k+1] */

        lua_rawgeti(L, param, 1); /* params[k+1][1] */
        ptypesArray[k] = interpretParameterType(L);
        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_WRONG)
        {
            lua_pushfstring(L, "bad parameter type for parameter %d, must be IN|OUT|INOUT", k);
            return 1;
        }
        paramsArray[k] = 0;
        valuesArray[kv] = 0;
        vtypesArray[k] = 0;
        /* convert lua value before call (allocate ram for output) */
        lua_pushliteral(L, "mgmType_to_c_");
        lua_rawgeti(L, param, 2);
        if (!lua_isstring(L, -1))
        {
            lua_pushfstring(L, "no type specified for parameter %d", k+1);
            return 1;
        }
        cpssOsStrCpy(utypesArray[k], lua_tostring(L, -1));
        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_IN &&
                string_equal_to(L, "string"))
        {
            /* special case: `IN string' will be passed as `const char*' */
            lua_pop(L, 2);
            lua_rawgeti(L, param, 4);
        }
        else
        {
            lua_concat(L, 2);
            lua_getglobal(L, lua_tostring(L, -1));
            if (!lua_isfunction(L, -1))
            {
                /* try to lookup C function with the same name */
                lua_CFunction cvtFunc;
                cvtFunc = (lua_CFunction) getFunctionByName(lua_tostring(L, -2));
                if (cvtFunc)
                {
                    lua_pop(L, 1);
                    lua_pushcfunction(L, cvtFunc);
                }
            }
            if (!lua_isfunction(L, -1))
            {
                lua_pushfstring(L, "function '%s' is not defined", lua_tostring(L, -2));
                return 1;
            }
            if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_OUT)
            {
                lua_pushnil(L);
            }
            else
            {
                lua_rawgeti(L, param, 4);
            }
            if (lua_pcall(L, 1, 1, 0))
            {
                lua_pushfstring(L, "error while executing %s: ", lua_tostring(L, -2));
                lua_insert(L, -2);
                lua_concat(L, 2);
                return 2;
            }
            /* remove function name */
            lua_remove(L, -2);
            if (lua_isnil(L, -1))
            {
                lua_pushfstring(L, "the parameter %d cannot be converted to required type", k+1);
                return 1;
            }
        }

        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_IN)
        {
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                paramsArray[k] = (GT_UINTPTR)((GT_U32)lua_tonumber(L, -1));
                lua_pop(L, 1);
            }
            else if (lua_type(L, -1) == LUA_TUSERDATA)
            {
                lua_insert(L, -2);
#if __WORDSIZE == 64
#define PUSH_PTR_TO_PARAMS(ptr) \
                paramsArray[k] = (GT_UINTPTR)(ptr)
#define ALIGN_FOR_PTR_IN_VALUES() { \
                if (kv & 1) \
                    kv++; \
}
#define PUSH_PTR_TO_VALUES(ptr) { \
                *((void**)(valuesArray+kv)) = (ptr); \
                kv++; \
}
#else
#define PUSH_PTR_TO_PARAMS(ptr) \
                paramsArray[k] = (GT_UINTPTR)(ptr)
#define ALIGN_FOR_PTR_IN_VALUES()
#define PUSH_PTR_TO_VALUES(ptr) \
                valuesArray[kv] = (GT_U32)(ptr)
#endif

                PUSH_PTR_TO_PARAMS(lua_touserdata(L, -2));
                /* don't free object here */
            }
            else if (lua_type(L, -1) == LUA_TLIGHTUSERDATA)
            {
                PUSH_PTR_TO_PARAMS(lua_touserdata(L,-1));
                lua_pop(L, 1);
            }
            else if (lua_type(L, -1) == LUA_TSTRING)
            {
                PUSH_PTR_TO_PARAMS(lua_tostring(L, -1));
                lua_pop(L, 1);
            }
            else
            /* handle strings */
            {
                lua_pushfstring(L, "p1: not supported param %d", k+1);
                return 1;
            }
        }
        else /* ptypesArray[k] == OUT || INOUT */
        {
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                PUSH_PTR_TO_PARAMS(valuesArray+kv);
                valuesArray[kv] = (GT_U32)lua_tonumber(L, -1);
                lua_pop(L, 1);
            }
            else if (lua_type(L, -1) == LUA_TUSERDATA)
            {
                lua_insert(L, -2);
                PUSH_PTR_TO_PARAMS(lua_touserdata(L, -2));
                valuesArray[kv] = lua_gettop(L) - 1;
                vtypesArray[k] = 1;
                /* don't free object here */
            }
            else if (lua_type(L, -1) == LUA_TLIGHTUSERDATA)
            {
                ALIGN_FOR_PTR_IN_VALUES();
                PUSH_PTR_TO_PARAMS(valuesArray+kv);
                PUSH_PTR_TO_VALUES(lua_touserdata(L, -1));
                lua_pop(L, 1);
                vtypesArray[k] = 2;
            }
            else
            {
                lua_pushliteral(L, "p2: not supported");
                return 1;
            }
        }
        lua_pop(L, 1); /* params[k+1] */
    }

    /* call function */
    if (doLock)
    {
        CPSS_API_CALL_LOCK();
    }
    retval = funcPtr(
        paramsArray[0],
        paramsArray[1],
        paramsArray[2],
        paramsArray[3],
        paramsArray[4],
        paramsArray[5],
        paramsArray[6],
        paramsArray[7],
        paramsArray[8],
        paramsArray[9],
        paramsArray[10],
        paramsArray[11],
        paramsArray[12],
        paramsArray[13],
        paramsArray[14],
        paramsArray[15],
        paramsArray[16],
        paramsArray[17],
        paramsArray[18],
        paramsArray[19],
        paramsArray[20],
        paramsArray[21],
        paramsArray[22],
        paramsArray[23],
        paramsArray[24],
        paramsArray[25],
        paramsArray[26],
        paramsArray[27],
        paramsArray[28],
        paramsArray[29]);
    if (doLock)
    {
        CPSS_API_CALL_UNLOCK();
    }

    /* push results */
    lua_pushnumber(L, retval);
    lua_newtable(L);
    values = lua_gettop(L);

    for (k = kv = 0; k < nparams; k++, kv++)
    {
        int param;
        if (ptypesArray[k] == CPSS_API_PARAMETER_TYPE_IN)
            continue;

        lua_rawgeti(L, 2, k+1); /* params[k+1] */
        param = lua_gettop(L); /* param = params[k+1] */

#if defined(CPU_BE) /* in Big Endien, need to fix the U16 and U8 because the gen wrapper expect a U32 variable */
        if (cpssOsStrCmp("GT_U16", utypesArray[k]) == 0) {  
            valuesArray[kv] = valuesArray[kv] >> 16;
        }
        if (cpssOsStrCmp("GT_U8", utypesArray[k]) == 0) { 
            valuesArray[kv] = valuesArray[kv] >> 24;
        }
#endif
        
        /* convert return values to lua */
        lua_pushliteral(L, "mgmType_to_lua_");
        lua_rawgeti(L, param, 2);
        lua_concat(L, 2);
        lua_getglobal(L, lua_tostring(L, -1));
        if (!lua_isfunction(L, -1))
        {
            /* try to lookup C function with the same name */
            lua_CFunction cvtFunc;
            cvtFunc = (lua_CFunction) getFunctionByName(lua_tostring(L, -2));
            if (cvtFunc)
            {
                lua_pop(L, 1);
                lua_pushcfunction(L, cvtFunc);
            }
        }
        if (!lua_isfunction(L, -1))
        {
            lua_pushfstring(L, "function '%s' is not defined", lua_tostring(L, -2));
            return 1;
        }
        if (vtypesArray[k] == 0)
        {
            lua_pushnumber(L, valuesArray[kv]);
        }
        else if (vtypesArray[k] == 2)
        {
            /* lightuserdata */
#if __WORDSIZE == 64
            /* align */
            if (kv & 1)
                kv++;
            lua_pushlightuserdata(L, *((void**)(valuesArray+kv)));
            kv++;
#else
            lua_pushlightuserdata(L, (void*)valuesArray[kv]);
#endif
        }
        else /* vtypesArray[nparams] == 1 */
        {
            lua_pushvalue(L, valuesArray[kv]); /* previosly stored index */
        }
        /*TODO*/
        if (lua_pcall(L, 1, 1, 0))
        {
            lua_pushfstring(L, "error while executing %s: ", lua_tostring(L, -2));
            lua_insert(L, -2);
            lua_concat(L, 2);
            return 2;
        }
        /* remove function name */
        lua_remove(L, -2);

        lua_rawgeti(L, param, 3);
        lua_insert(L, -2);
        lua_setfield(L, values, lua_tostring(L, -2));
        lua_pop(L, 1); /* params[k+1][3] */

        lua_pop(L, 1); /* params[k+1] */
    }
    return 2;
}

/*************************************************************************
* cpssGenWrapperCheckParam
*
* Description:
*       mgmType_to_lua_${custom_type_name}(mgmType_to_c_${custom_type_name}(param))
*       This can be used for
*       1. Convert 
*
* Parameters:
*       custom_type_name - Fx: "CPSS_MAC_ENTRY_EXT_STC"
*       param            - A value to check
*
*
* Returns:
*       output value if successful
*       string if error
*
*
* Example
*       value = cpssGenWrapperCheckParam("CPSS_MAC_ENTRY_EXT_STC", nil)
*       value.isStatic = true
*       value = cpssGenWrapperCheckParam("CPSS_MAC_ENTRY_EXT_STC", value)
*
*************************************************************************/
static int cpssGenWrapperCheckParam(
    lua_State *L
)
{
    int i;
    LUA_CLI_GETFUNCBYNAME_T  getFunctionByName;

    getFunctionByName = (LUA_CLI_GETFUNCBYNAME_T)
            luaCLIEngineGetData(L, "_getFunctionByName", 1);
    if (getFunctionByName == NULL)
    {
        lua_pushliteral(L, "_getFunctionByName not defined");
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "wrong type name");
        return 1;
    }
    lua_settop(L, 2);
    /*
     * push "mgmType_to_lua_${custom_type_name}"
     * push function mgmType_to_lua_${custom_type_name}
     * push "mgmType_to_c_${custom_type_name}"
     * push function mgmType_to_c_${custom_type_name}
     */
    for (i = 0; i < 2; i++)
    {
        /* convert lua value to C */
        lua_pushstring(L, i == 0 ? "mgmType_to_lua_" : "mgmType_to_c_");
        lua_pushvalue(L, 1);
        lua_concat(L, 2);
        lua_getglobal(L, lua_tostring(L, -1));
        if (!lua_isfunction(L, -1))
        {
            /* try to lookup C function with the same name */
            lua_CFunction cvtFunc;
            cvtFunc = (lua_CFunction) getFunctionByName(lua_tostring(L, -2));
            if (cvtFunc)
            {
                lua_pop(L, 1);
                lua_pushcfunction(L, cvtFunc);
            }
        }
        if (!lua_isfunction(L, -1))
        {
            lua_pushfstring(L, "function '%s' is not defined", lua_tostring(L, -2));
            return 1;
        }
    }

    lua_pushvalue(L, 2);
     
    /*
     * call function mgmType_to_c_${custom_type_name} (param)
     * call function mgmType_to_lua_${custom_type_name}
     */
    for (i = 0; i < 2; i++)
    {
        if (lua_pcall(L, 1, 1, 0))
        {
            lua_pushfstring(L, "error while executing %s: ", lua_tostring(L, -2));
            lua_insert(L, -2);
            lua_concat(L, 2);
            return 1;
        }
        /* remove function name */
        lua_remove(L, -2);
    }
    return 1;
}

/*************************************************************************
* prv_lua_to_c_enum
*
* Description:
*       Convert lua value to enum
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       integer
*
*************************************************************************/
int prv_lua_to_c_enum(
    lua_State   *L,
    enumDescr   *descr
)
{
    if (lua_isnumber(L, -1))
        return (int)lua_tointeger(L, -1);
    if (!descr)
        return 0;
    if (lua_isstring(L, -1))
    {
        const char *s = lua_tostring(L, -1);
        while(descr->string)
        {
            if (cpssOsStrCmp(s, descr->string) == 0)
                return descr->enumValue;
            descr++;
        }
    }
    return 0;
}

/*************************************************************************
* prv_c_to_lua_enum
*
* Description:
*       Convert enum to lua
*
* Parameters:
*       cmd
*
* Returns:
*       string at the top of stack
*
*************************************************************************/
void prv_c_to_lua_enum(
    lua_State   *L,
    enumDescr   *descr,
    int         enumValue
)
{
    if (descr)
    {
        for(; descr->string; descr++)
        {
            if (enumValue == descr->enumValue)
            {
                lua_pushstring(L, descr->string);
                return;
            }
        }
    }
    lua_pushnil(L);
}

#ifndef LUACLI_NOCPSS
/*******************************************************************************
* prvCpssGetEnumString
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Returns enum member string according to its enumerated value or NULL 
*       when enumerated value is wrong.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       enumValue             - enum enumerated value
*       descrPtr              - enum support struct array
*
* OUTPUTS:
*
* RETURNS:
*       enum member string or NULL when entry enumerated value is wrong
*
* COMMENTS:
*
*******************************************************************************/
const GT_CHAR* prvCpssGetEnumString
(   
    IN  GT_U32                  enumValue,
    IN  enumDescr               *descrPtr    
)
{
    if (NULL != descrPtr)
    {
        while(NULL != descrPtr->string)
        {
            if (enumValue == (GT_U32) descrPtr->enumValue)
                return (const GT_CHAR_PTR) descrPtr->string;
                
            descrPtr++;
        }    
    }   
    
    return NULL;          
}
#endif /* !defined(LUACLI_NOCPSS) */


/*************************************************************************
* prv_lua_check_enum
*
* Description:
*       Return true if L => 1st is a member of enum
*
* Parameters:
*       L => 1st    - string to check
*
* Returns:
*       integer
*
*************************************************************************/
int prv_lua_check_enum(
    lua_State   *L,
    enumDescr   *descr
)
{
    const char *str;

    if (lua_type(L, 1) != LUA_TSTRING)
    {
        lua_pushboolean(L, 0);
        return 0;
    }

    str = lua_tostring(L, 1);
    if (descr)
    {
        for(; descr->string; descr++)
        {
            if (cpssOsStrCmp(str, descr->string) == 0)
            {
                lua_pushboolean(L, 1);
                return 1;
            }
        }
    }
    lua_pushboolean(L, 0);
    return 1;
}

/*************************************************************************
* prv_lua_complete_enum
*
* Description:
*       Return the list of alternatives for autocomplete matching L => 1st
*
* Parameters:
*       L => 1st    - string to complete
*
* Returns:
*       integer
*
*************************************************************************/
int prv_lua_complete_enum(
    lua_State   *L,
    enumDescr   *descr
)
{
    const char *str;
    size_t      len;
    int         index = 0;

    if (lua_type(L, 1) != LUA_TSTRING)
        return 0;

    str = lua_tolstring(L, 1, &len);
    lua_newtable(L);
    if (descr)
    {
        for(; descr->string; descr++)
        {
            if (cpssOsStrNCmp(str, descr->string, len) == 0)
            {
                lua_pushstring(L, descr->string);
                lua_rawseti(L, -2, ++index);
            }
        }
    }
    return 1;
}

#ifndef LUACLI_NOCPSS
/*************************************************************************
* prv_swap_byte_array
*
* DESCRIPTION:
*       Swaps array in reversiveorder
*
* INPUTS:
*       input_data        - input byte array
*       dataLen           - swapping bytes counmt
*
* OUTPUTS:
*       output_data       - output byte array
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
*
*************************************************************************/
GT_STATUS prv_swap_byte_array
(
    IN  const GT_U8_PTR     input_data,
    IN  GT_U32              dataLen,
    OUT GT_U8_PTR           output_data
)
{
    GT_U32 k;
    GT_U8  data;
    
    for (k = 0; k < dataLen / 2; k++)
    {
        data = input_data[k];
        output_data[k] = input_data[dataLen - k - 1];
        output_data[dataLen - k - 1] = data;
    }
    
    return GT_OK;
}


/*************************************************************************
* prv_lua_apply_mask
*
* DESCRIPTION:
*       Compare two structures using ignore mask
*
* INPUTS:
*       input_data  - Input data
*       output_data - Output data
*       appiedMask  - data mask: the bits which will be reseted
*                     are set to 1
*       dataLen     - Data length (the same for data1, data2, ignoreMask)
*
* OUTPUTS:
*       data        - masked data
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
*
*************************************************************************/
GT_STATUS prv_lua_apply_mask(
    IN  const void* input_data,
    IN  const void* output_data,
    IN  const void* appiedMask,
    IN  GT_U32      dataLen
)
{
    GT_U32 k;
    for (k = 0; k < dataLen; k++)
    {
        char mask = (char) ~(((const char*)appiedMask)[k]);
        ((char*)output_data)[k] = (char) (((const char*)input_data)[k] & mask);
    }
    return GT_OK;
}


/*************************************************************************
* prv_lua_compare_mask
*
* DESCRIPTION:
*       Compare two structures using ignore mask
*
* INPUTS:
*       data1       - Input data
*       data2       - Input data
*       ignoreMask  - data mask: the bits which will be ignored
*                     are set to 1
*       dataLen     - Data length (the same for data1, data2, ignoreMask)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       1 if matched
*       0 if not matched
*
* COMMENTS:
*       None
*
*************************************************************************/
int prv_lua_compare_mask(
    IN  const void* data1,
    IN  const void* data2,
    IN  const void* ignoreMask,
    IN  GT_U32      dataLen
)
{
    GT_U32 k;
    for (k = 0; k < dataLen; k++)
    {
        char mask = (char) ~(((const char*)ignoreMask)[k]);
        if (    ( ((const char*)data1)[k] & mask ) !=
                ( ((const char*)data2)[k] & mask ) )
        {
            return 0;
        }
    }
    return 1;
}


/*************************************************************************
* prv_lua_to_c_GT_U64
*
* Description:
*       Convert to "c" GT_U64
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       
*
*************************************************************************/
void prv_lua_to_c_GT_U64(
    lua_State *L,
    GT_U64  *val
)
{
    F_ARRAY_START(val, -1, l);
        F_ARRAY_NUMBER(val, l, 0, GT_U32);
        F_ARRAY_NUMBER(val, l, 1, GT_U32);
    F_ARRAY_END(val, -1, l);
}

/*************************************************************************
* prv_c_to_lua_GT_U64
*
* Description:
*       Convert GT_U64 to lua
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_c_to_lua_GT_U64(
    lua_State *L,
    GT_U64 *val
)
{
    int t;
    lua_newtable(L);
    t = lua_gettop(L);
    FO_ARRAY_START(val, t, l);
        FO_ARRAY_NUMBER(val, l, 0, GT_U32);
        FO_ARRAY_NUMBER(val, l, 1, GT_U32);
    FO_ARRAY_END(val, t, l);
}

add_mgm_type(GT_U64);
#endif /* !defined(LUACLI_NOCPSS) */


/*************************************************************************
* mgmType_to_c_GT_PTR
*
* Description:
*       Convert to GT_PTR
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  LUA_TLIGHTUSERDATA if successful
*
*************************************************************************/
static int mgmType_to_c_GT_PTR(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        lua_pushlightuserdata(L, NULL);
    else if (!lua_islightuserdata(L, 1))
        lua_pushnil(L);
    else
        lua_pushvalue(L, 1);
    return 1;
}

/*************************************************************************
* mgmType_to_lua_GT_PTR
*
* Description:
*       Convert GT_PTR to Lua lightuserdata
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  LUA_TLIGHTUSERDATA if successful
*
*************************************************************************/
static int mgmType_to_lua_GT_PTR(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_islightuserdata(L, 1))
        lua_pushnil(L);
    else
        lua_pushvalue(L, 1);
    return 1;
}

/*************************************************************************
* mgmType_to_c_uintptr
* mgmType_to_c_GT_UINTPTR
*
* Description:
*       Convert to GT_PTR
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  LUA_TLIGHTUSERDATA if successful
*
*************************************************************************/
static int mgmType_to_c_uintptr(
    lua_State *L
)
{
    if (lua_isnumber(L, 1))
        lua_pushlightuserdata(L, (void*)((GT_UINTPTR)lua_tonumber(L, 1)));
    else if (lua_islightuserdata(L, 1))
        lua_pushvalue(L, 1);
    else
        lua_pushlightuserdata(L, NULL);
    return 1;
}

/*************************************************************************
* mgmType_to_lua_uintptr
* mgmType_to_lua_GT_UINTPTR
*
* Description:
*       Convert GT_PTR to Lua lightuserdata
*
* Parameters:
*  value
*
* Returns:
*  none if error
*  LUA_TLIGHTUSERDATA if successful
*
*************************************************************************/
static int mgmType_to_lua_uintptr(
    lua_State *L
)
{
    if (lua_isnil(L, 1))
        return 1;
    if (!lua_islightuserdata(L, 1))
        lua_pushnil(L);
    else
        lua_pushnumber(L, (lua_Number)((GT_UINTPTR)lua_touserdata(L, 1)));
    return 1;
}



/*******************************************************************************
* GenWrapper_init
*
* DESCRIPTION:
*       initializing of CPSS contexted lua tipes
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS: 
*       L           - lua stack
*
* OUTPUTS:
*
* RETURNS:
*       0 - no returned rarameters added no lua stack
*
* COMMENTS:
*
*******************************************************************************/
int luaEngineGenWrapper_init(lua_State *L)
{
    lua_register(L, "cpssGenWrapper", cpssGenWrapper);
    lua_register(L, "cpssGenWrapperCheckParam", cpssGenWrapperCheckParam);
    lua_register(L, "mgmType_to_c_int", mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_U8", mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_U16", mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_TRUNK_ID",             mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_32",                   mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_U32", mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_PORT_NUM",             mgmType_to_c_int);
    lua_register(L, "mgmType_to_c_GT_PHYSICAL_PORT_NUM",    mgmType_to_c_int);
    lua_register(L, "mgmType_to_lua_int", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_U8", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_U16", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_32",                 mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_U32", mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_PORT_NUM",           mgmType_to_lua_int);
    lua_register(L, "mgmType_to_lua_GT_PHYSICAL_PORT_NUM",  mgmType_to_lua_int);
    lua_register(L, "mgmType_to_c_bool", mgmType_to_c_bool);
    lua_register(L, "mgmType_to_c_GT_BOOL", mgmType_to_c_bool);
    lua_register(L, "mgmType_to_lua_bool", mgmType_to_lua_bool);
    lua_register(L, "mgmType_to_lua_GT_BOOL", mgmType_to_lua_bool);
#ifndef LUACLI_NOCPSS
    lua_register(L, "mgmType_to_c_GT_U64", mgmType_to_c_GT_U64);
    lua_register(L, "mgmType_to_lua_GT_U64", mgmType_to_lua_GT_U64);
#endif /* !defined(LUACLI_NOCPSS) */
    lua_register(L, "mgmType_to_c_GT_PTR", mgmType_to_c_GT_PTR);
    lua_register(L, "mgmType_to_lua_GT_PTR", mgmType_to_lua_GT_PTR);
    lua_register(L, "mgmType_to_c_uintptr", mgmType_to_c_uintptr);
    lua_register(L, "mgmType_to_c_GT_UINTPTR", mgmType_to_c_uintptr);
    lua_register(L, "mgmType_to_lua_uintptr", mgmType_to_lua_uintptr);
    lua_register(L, "mgmType_to_lua_GT_UINTPTR", mgmType_to_lua_uintptr);
    return 0;
}
