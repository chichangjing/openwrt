#error "Deprecated"
/*************************************************************************
* cmdLuaCommon.h
*
* DESCRIPTION:
*       Common function for cmdLua
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
**************************************************************************/

/***** Includes  ********************************************************/

/***** Defines  ********************************************************/
#define LUACLI_MEMSTAT

#ifdef LUACLI_MEMSTAT
typedef struct {
    int     totalUsed;
    int     nChunks;
} LUACLI_MEM_STAT_STC;
#endif /* LUACLI_MEMSTAT */

/***** Public Functions ************************************************/



/*******************************************************************************
* cmdLua_newstate
*
* DESCRIPTION:
*       Init new Lua state, enable minimal functionality
*       Should be used to create luaCLI and for new Lua task
*
* INPUTS:
*       memstat     - pointer to memstat structure or NULL
*
* OUTPUTS:
*       None
*
* RETURNS:
*       luaPtr
*
* COMMENTS:
*       None
*
*******************************************************************************/
lua_State* cmdLua_newstate
(
    void *ud
);
