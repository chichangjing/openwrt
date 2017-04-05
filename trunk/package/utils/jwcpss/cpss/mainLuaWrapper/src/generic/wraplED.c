/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplED.c
*
* DESCRIPTION:
*       A lua wrapper for simple editor (ED)
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
**************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <lua.h>


/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Externs ********************************************************/
int ed_main(char *filename, char *prompt, void* iostream);

/************* Functions ******************************************************/

/*******************************************************************************
* wraplED: lua function
*
* DESCRIPTION:
*       call ED for specified file
*
* INPUTS:
*       filename   - string or nil
*       prompt     - optional ED command prompt
*
* RETURNS:
*       0  if successful
*       nonzero on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int wraplED
(
    lua_State *L
)
{
    void* IOStream;
    char *filename = NULL;
    char *prompt = NULL;

    lua_getglobal(L, "_IOStream");
    IOStream = lua_touserdata(L, -1);
    lua_pop(L, 1);
    if (IOStream == NULL)
    {
        lua_pushinteger(L, 1);
        return 1;
    }
    if (lua_isstring(L, 1))
    {
        filename = (char*)((GT_UINTPTR)lua_tostring(L, 1));
    } else if (!lua_isnil(L, 1))
    {
        /* bad param */
        lua_pushinteger(L, 1);
        return 1;
    }
    if (lua_gettop(L) > 1)
    {
        if (lua_isstring(L, 2))
        {
            prompt = (char*)((GT_UINTPTR)lua_tostring(L, 2));
        }
    }
    lua_pushinteger(L, ed_main(filename, prompt, IOStream));
    return 1;
}
