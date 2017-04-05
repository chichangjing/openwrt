/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplTFTPC.c
*
* DESCRIPTION:
*       A lua wrapper for cmdFS_tftp_get(), cmdFS_tftp_put()
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
**************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <lua.h>


/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Externs ********************************************************/
GT_STATUS cmdFS_tftp_put(
    IN  const char  *server,
    IN  GT_U16      port,
    IN  const char  *localfile,
    IN  const char  *remotefile,
    OUT char        *errorBuf
);

GT_STATUS cmdFS_tftp_get(
    IN  const char  *server,
    IN  GT_U16      port,
    IN  const char  *localfile,
    IN  const char  *remotefile,
    IN  GT_BOOL     compress,
    OUT char        *errorBuf
);

/************* Functions ******************************************************/

/*******************************************************************************
* lua_tftp: lua function
*
* DESCRIPTION:
*       Get file using TFTP prorocol
*
* INPUTS:
*       operation  - one of "get" or "put"
*       server     - message queue name
*       port       - message type string
*       localfile  - the path for local file
*       remotefile - the path for remote file
*
* RETURNS:
*       0  if successful
*       nonzero and error string on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int lua_tftp
(
    lua_State *L
)
{
    GT_STATUS rc = GT_OK;
    int operation = 0;
    char errorBuf[1024];

    if (lua_gettop(L) < 5)
    {
        lua_pushinteger(L, GT_BAD_PARAM);
        lua_pushliteral(L, "no enough parameters");
        return 2;
    }
    if (!lua_isstring(L, 1))
    {
bad_operation:
        lua_pushinteger(L, GT_BAD_PARAM);
        lua_pushliteral(L, "bad param 1: must be \"get\" or \"get-compressed\" or \"put\"");
        return 2;
    }
    if (cpssOsStrCmp(lua_tostring(L, 1), "get") == 0)
        operation = 1;
    if (cpssOsStrCmp(lua_tostring(L, 1), "put") == 0)
        operation = 2;
    if (cpssOsStrCmp(lua_tostring(L, 1), "get_z") == 0)
        operation = 3;
    if (!operation)
        goto bad_operation;

    
    if (!lua_isstring(L, 2))
    {
        lua_pushinteger(L, GT_BAD_PARAM);
        lua_pushliteral(L, "bad parameter 2: must be a string");
        return 1;
    }
    if (!lua_isnumber(L, 3))
    {
        lua_pushinteger(L, GT_BAD_PARAM);
        lua_pushliteral(L, "bad parameter 3: must be a numeric");
        return 1;
    }
    if (!lua_isstring(L, 4))
    {
        lua_pushinteger(L, GT_BAD_PARAM);
        lua_pushliteral(L, "bad parameter 4: must be a string");
        return 1;
    }
    if (!lua_isstring(L, 5))
    {
        lua_pushinteger(L, GT_BAD_PARAM);
        lua_pushliteral(L, "bad parameter 4: must be a string");
        return 1;
    }

    errorBuf[0] = 0;
    switch (operation)
    {
        case 1:
            /* get */
            rc = cmdFS_tftp_get(
                    lua_tostring(L, 2),
                    (GT_U16)lua_tonumber(L, 3),
                    lua_tostring(L, 4),
                    lua_tostring(L, 5),
                    GT_FALSE,
                    errorBuf);
            break;
        case 2:
            /* put */
            rc = cmdFS_tftp_put(
                    lua_tostring(L, 2),
                    (GT_U16)lua_tonumber(L, 3),
                    lua_tostring(L, 4),
                    lua_tostring(L, 5),
                    errorBuf);
            break;
        case 3:
            /* get_z */
            rc = cmdFS_tftp_get(
                    lua_tostring(L, 2),
                    (GT_U16)lua_tonumber(L, 3),
                    lua_tostring(L, 4),
                    lua_tostring(L, 5),
                    GT_TRUE, /* compress local file */
                    errorBuf);
            break;
    }

    lua_pushinteger(L, rc);
    if (rc == GT_OK)
    {
        return 1;
    }
    lua_pushstring(L, errorBuf);
    return 2;
}
