/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplOsSem.c
*
* DESCRIPTION:
*       A lua wrapper for osSem*()
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
**************************************************************************/
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <lua.h>


/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Functions ******************************************************/

/*******************************************************************************
* luaOsSigSemCreate: lua function
*
* DESCRIPTION:
*       Create and initialize binary semaphore
*
* INPUTS:
*       name       - semaphore name
*       isFull     - (optional) boolean, initial state is full if true
*
* RETURNS:
*       semId
*       string on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaOsSigSemCreate
(
    lua_State *L
)
{
    CPSS_OS_SIG_SEM semId;
    CPSS_OS_SEMB_STATE_ENT init = CPSS_OS_SEMB_EMPTY_E;
    char semName[22]; /* cpssOsSigSemBinCreate accepts names less then 21 chars */
    GT_STATUS     status;

    if (lua_gettop(L) < 1)
    {
        lua_pushliteral(L, "no enough parameters");
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "bad parameter 1");
        return 1;
    }
    /* workaround: copy semaphore name to fix const/not const issue */
    cpssOsStrNCpy(semName, lua_tostring(L, 1), sizeof(semName));
    semName[sizeof(semName)-1] = 0;
    if (lua_isboolean(L, 2))
    {
        if (lua_toboolean(L, 2))
        {
            init = CPSS_OS_SEMB_FULL_E;
        }
    }
    status = cpssOsSigSemBinCreate(semName, init, &semId);
    if (status != GT_OK)
    {
        lua_pushfstring(L, "error returned: %d", status);
        return 1;
    }
    lua_pushnumber(L, semId);
    return 1;
}
/*******************************************************************************
* luaOsSigSemDelete: lua function
*
* DESCRIPTION:
*       Delete sig sem
*
* INPUTS:
*       semId     - semaphore id
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaOsSigSemDelete
(
    lua_State *L
)
{
    CPSS_OS_SIG_SEM semId;
    if (!lua_isnumber(L, 1))
    {
        return 0;
    }
    semId = (CPSS_OS_SIG_SEM)lua_tonumber(L, 1);
    cpssOsSigSemDelete(semId);
    return 0;
}

/*******************************************************************************
* luaOsSigSemSignal: lua function
*
* DESCRIPTION:
*       Signal semaphore
*
* INPUTS:
*       semId     - semaphore id
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaOsSigSemSignal
(
    lua_State *L
)
{
    CPSS_OS_SIG_SEM semId;
    if (!lua_isnumber(L, 1))
    {
        return 0;
    }
    semId = (CPSS_OS_SIG_SEM)lua_tonumber(L, 1);
    cpssOsSigSemSignal(semId);
    return 0;
}

/*******************************************************************************
* luaOsSigSemWait: lua function
*
* DESCRIPTION:
*       Signal semaphore
*
* INPUTS:
*       semId     - semaphore id
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       None
*
*******************************************************************************/
int luaOsSigSemWait
(
    lua_State *L
)
{
    CPSS_OS_SIG_SEM semId;
    GT_U32          timeOut = CPSS_OS_SEM_WAIT_FOREVER_CNS;
    GT_STATUS       ret;
    if (!lua_isnumber(L, 1))
    {
        lua_pushnumber(L, GT_BAD_PARAM);
        return 1;
    }
    semId = (CPSS_OS_SIG_SEM)lua_tonumber(L, 1);
    if (lua_isnumber(L, 2))
    {
        timeOut = (GT_U32)lua_tonumber(L, 2);
        if (timeOut == 0)
        {
            timeOut = CPSS_OS_SEM_NO_WAIT_CNS;
        }
    }
    ret = cpssOsSigSemWait(semId, timeOut);
    lua_pushinteger(L, ret);
    return 1;
}
