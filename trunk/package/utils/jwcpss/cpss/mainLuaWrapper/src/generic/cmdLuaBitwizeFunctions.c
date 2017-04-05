/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdLuaBitwizeFunctions.c
*
* DESCRIPTION:
*       Lua bitwize operations 
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
**************************************************************************/
#include <lua.h>

#define OPERATOR(name,operation) \
static int bitwize ## name( \
    lua_State *L \
) \
{ \
    unsigned int a, b; \
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) \
        return 0; \
    a = (unsigned int)lua_tonumber(L, 1); \
    b = (unsigned int)lua_tonumber(L, 2); \
    lua_pushnumber(L, operation); \
    return 1; \
}


OPERATOR(And, a & b);
OPERATOR(Or, a | b);
OPERATOR(Xor, a ^ b);
OPERATOR(SHL, ((b < 32) ? a << b : 0));
OPERATOR(SHR, ((b < 32) ? a >> b : 0));

static int bitwizeNot(
    lua_State *L
)
{
    /* check parameters */
    unsigned int a;
    if (!lua_isnumber(L, 1))
    {
        /* return nothing */
        return 0;
    }
    a = (unsigned int)lua_tonumber(L, 1);
    lua_pushnumber(L, ~a);
    return 1;
}

int registerBitwizeFunctions(
    lua_State *L
)
{
    lua_register(L, "bit_and", bitwizeAnd);
    lua_register(L, "bit_or" , bitwizeOr);
    lua_register(L, "bit_not", bitwizeNot);
    lua_register(L, "bit_xor", bitwizeXor);
    lua_register(L, "bit_shl", bitwizeSHL);
    lua_register(L, "bit_shr", bitwizeSHR);
    return 0;
}

/* provides link for the file */
int check_cmdLuaBitwizeFunctions()
{
    return 1;
}

