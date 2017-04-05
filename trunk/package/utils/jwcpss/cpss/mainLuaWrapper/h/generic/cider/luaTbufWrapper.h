/*******************************************************************************
*              (c), Copyright 2011, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* luaTbufWrapper.h
*
* DESCRIPTION:
*       Lua <=> C interface for tbuf
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/
#ifndef __luaTbufWrapper_h__
#define __luaTbufWrapper_h__

#include <lua.h>
#include <generic/cider/tbuf.h>

/*******************************************************************************
* prvLuaToTbuf
*
* DESCRIPTION:
*       Get tbuf stored in Lua object placed on stack at L_index
*
* INPUTS:
*       L                   - lua state
*       L_index             - tbuf object index in lua stack 
* 
* OUTPUTS:
*       None
*
* RETURNS:
*       tbuf        (tbuf.ptr == NULL if error)
*
* COMMENTS:
*
*******************************************************************************/
tbuf prvLuaToTbuf(
    lua_State       *L,
    int             L_index
);

/*******************************************************************************
* prvLuaPushTbuf
*
* DESCRIPTION:
*       Push tbuf to Lua stack
*
* INPUTS:
*       L                   - lua state
*       buf                 - tbuf object
*       
* 
* OUTPUTS:
*       None
*
* RETURNS:
*       tbuf        (tbuf.ptr == NULL if error)
*
* COMMENTS:
*
*******************************************************************************/
void prvLuaPushTbuf(
    lua_State       *L,
    tbuf            buf
);

/*******************************************************************************
* prvLuaPushTbufAsString
*
* DESCRIPTION:
*       Convert tbuf object to Lua string
*
* INPUTS:
*       L                   - lua state
*           buf             - tbuf object
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       String at the top of Lua stack
*
* COMMENTS:
*
*******************************************************************************/
void prvLuaPushTbufAsString(lua_State *L, tbuf buf);

/*******************************************************************************
* prvLuaTbuf_register
*
* DESCRIPTION:
*       register tbuf functions
*
* INPUTS:
*       L                   - lua state
*       
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
int prvLuaTbuf_register(lua_State *L);

#endif /* __luaTbufWrapper_h__ */
