/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssGen.h
*
* DESCRIPTION:
*       A generic tools for Lua <=> C interface
*
* DEPENDENCIES:
*       A lua type wrapper
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>

#include <lua.h>
#include <cmdShell/lua/genwrapper_API.h>


/***** declarations ********/

#define WRL_GET_PARAM_ENUM(var,type,L_index) \
    if (status == GT_OK) \
    { \
        int value; \
        status = prv_lua_to_c_enum_chk(L, enumDescr_ ## type, L_index, &value, &error_message); \
        if (status == GT_OK) \
            var = (type) value; \
    }

#define WRL_GET_PARAM_END() \
    if (status != GT_OK && error_message == NULL) \
    { \
        error_message = wrlFunctionArgumentsFromLuaGettingErrorMessage; \
    }


/*************************************************************************
* prv_lua_to_c_enum_chk
*
* Description:
*       Convert lua value to enum
*
* Parameters:
*       value at L_index in Lua stack
*
* Returns:
*       integer
*
*************************************************************************/
GT_STATUS prv_lua_to_c_enum_chk(
    IN  lua_State   *L,
    IN  enumDescr   *descr,
    IN  int         L_index,
    OUT int         *value,
    OUT GT_CHAR_PTR *errorrMessagePtr
);
