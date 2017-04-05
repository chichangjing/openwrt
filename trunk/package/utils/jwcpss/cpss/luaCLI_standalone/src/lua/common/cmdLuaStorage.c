/****************************************************
* cmdLuaStorage.c
*
* DESCRIPTION:
*       A global storage for different Lua Instances
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
**************************************************************************/

#if !defined(ENV_MAINCMD) && !defined(ENV_POSIX)
#  error "No layer defined. Define one of ENV_MAINCMD, ENV_POSIX"
#endif

/***** Include files ***************************************************/
#include <lua.h>

#ifdef ENV_MAINCMD
#  include "cmdLua_os_mainCmd.inc"
#endif
#ifdef ENV_POSIX
#  include "cmdLua_os_posix.inc"
#endif
#ifdef SHARED_MEMORY
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsSharedMalloc.h>

/* redefine stuff */
#undef LUACLI_MUTEX_TYPE
#undef LUACLI_MUTEX_INITIALIZER
#undef LUACLI_MUTEX_CREATE
#undef LUACLI_MUTEX_LOCK
#undef LUACLI_MUTEX_UNLOCK
#undef LUACLI_MEMCPY
#undef LUACLI_MALLOC
#undef LUACLI_FREE

#define LUACLI_MUTEX_TYPE       GT_MUTEX
#define LUACLI_MUTEX_CREATE     osMutexCreate
#define LUACLI_MUTEX_LOCK       osMutexLock
#define LUACLI_MUTEX_UNLOCK     osMutexUnlock
#define LUACLI_MEMCPY           osMemCpy
#define LUACLI_MALLOC           osMalloc
#define LUACLI_FREE             osFree

#endif /* SHARED_MEMORY */


/***** Defines  ********************************************************/
#define SER_TYPE_NIL            1
#define SER_TYPE_BOOL_TRUE      2
#define SER_TYPE_BOOL_FALSE     3
#define SER_TYPE_NUMBER         4
#define SER_TYPE_STRING         5
#define SER_TYPE_USERDATA       6
#define SER_TYPE_LIGHTUSERDATA  7
#define SER_TYPE_FUNCTION       8
#define SER_TYPE_TABLE          9

#define STORAGE_LOCK()          LUACLI_MUTEX_LOCK(storage->mtx)
#define STORAGE_UNLOCK()        LUACLI_MUTEX_UNLOCK(storage->mtx)


/***** Types ***********************************************************/
#define KEY_LEN         20
struct STORAGE_NODE_STC {
    struct STORAGE_NODE_STC *next;
    char                    key[KEY_LEN];
    char                    *bufPtr;
};
typedef struct STORAGE_NODE_STC STORAGE_NODE_STCT;
struct STORAGE_SHARED_DATA_STC {
    int                 initialized;
    LUACLI_MUTEX_TYPE   mtx;
    STORAGE_NODE_STCT   *nodes;
};
typedef struct STORAGE_SHARED_DATA_STC STORAGE_SHARED_DATA_STCT;


/***** Private Data ****************************************************/
#ifndef SHARED_MEMORY
static STORAGE_SHARED_DATA_STCT storage_static = {
    0,
    LUACLI_MUTEX_INITIALIZER,
    NULL
};
static STORAGE_SHARED_DATA_STCT *storage = &storage_static;
#else /* SHARED_MEMORY */
static STORAGE_SHARED_DATA_STCT *storage = NULL;
#endif

/***** Private Functions ***********************************************/

/*******************************************************************************
* serialize_Lua_value
*
* DESCRIPTION:
*       Serialize value. If bufPtr == NULL just calculate required size
*
* INPUTS:
*       A variable at top of stack
*
* OUTPUTS:
*       bufPtr  - A buffer to store serialized data.
*
* RETURNS:
*       The length of serialized data
*
* COMMENTS:
*       Data stored as TLV { char type, size_t length, char data[] }
*
*******************************************************************************/
static int serialize_Lua_value(lua_State *L, char *bufPtr)
{
#define TL  (1+sizeof(size_t))
#define STORE_TL(t,l) \
    if (bufPtr) { \
        size_t l1 = l; \
        *bufPtr = (t); \
        LUACLI_MEMCPY(bufPtr+1, &l1, sizeof(size_t)); \
    }
    switch (lua_type(L,-1))
    {
        case LUA_TNONE: /* for a non-valid index */
            return 0;
        case LUA_TNIL:
            STORE_TL(SER_TYPE_NIL, 0);
            return TL;
        case LUA_TBOOLEAN:
            STORE_TL(lua_toboolean(L, -1) ? SER_TYPE_BOOL_TRUE : SER_TYPE_BOOL_FALSE, 0);
            return TL;
        case LUA_TNUMBER:
            {
                lua_Number num;
                STORE_TL(SER_TYPE_NUMBER, sizeof(num));
                if (bufPtr)
                {
                    num = lua_tonumber(L, -1);
                    LUACLI_MEMCPY(bufPtr+TL, &num, sizeof(num));
                }
                return TL + sizeof(num);
            }
        case LUA_TSTRING:
            {
                const char *s;
                size_t len;
                s = lua_tolstring(L, -1, &len);
                STORE_TL(SER_TYPE_STRING, len);
                if (bufPtr)
                {
                    LUACLI_MEMCPY(bufPtr+TL, s, len);
                }
                return TL + (int)len;
            }
        case LUA_TUSERDATA:
            {
                size_t len;
                len = lua_objlen(L, -1);
                STORE_TL(SER_TYPE_USERDATA, len);
                if (bufPtr)
                {
                    LUACLI_MEMCPY(bufPtr+TL, lua_touserdata(L, -1), len);
                }
                return TL + (int)len;
            }
        case LUA_TLIGHTUSERDATA:
            STORE_TL(SER_TYPE_LIGHTUSERDATA, sizeof(void*));
            if (bufPtr)
            {
                void* ptr;
                ptr = lua_touserdata(L, -1);
                LUACLI_MEMCPY(bufPtr+TL, &ptr, sizeof(ptr));
            }
            return TL + sizeof(void*);
        case LUA_TFUNCTION:
            if (lua_iscfunction(L, -1))
            {
                STORE_TL(SER_TYPE_FUNCTION, sizeof(lua_CFunction));
                if (bufPtr)
                {
                    lua_CFunction   func = lua_tocfunction(L, -1);
                    LUACLI_MEMCPY(bufPtr+TL, &func, sizeof(func));
                }
                return TL + sizeof(lua_CFunction);
            }
        case LUA_TTHREAD:
            return 0;
        case LUA_TTABLE:
            /* table is in the stack at index '1' */
            {
                size_t current_offset;
                current_offset = TL;
                lua_pushnil(L);  /* first key */
                while (lua_next(L, -2) != 0)
                {
                    /* uses 'key' (at index -2) and 'value' (at index -1) */
                    /* store key */
                    lua_pushvalue(L, -2);
                    current_offset += serialize_Lua_value(L, bufPtr ? bufPtr + current_offset : NULL);
                    lua_pop(L, 1);
                    /* store key */
                    current_offset += serialize_Lua_value(L, bufPtr ? bufPtr + current_offset : NULL);
                    /* removes 'value'; keeps 'key' for next iteration */
                    lua_pop(L, 1);
                }
                STORE_TL(SER_TYPE_TABLE, current_offset - TL);
                return (int)current_offset;
            }
    }
    return 0;
#undef TL
#undef STORE_TL
}

/*******************************************************************************
* deserialize_Lua_value
*
* DESCRIPTION:
*      Deserialize value.
*
* INPUTS:
*       bufPtr  - A buffer with stored data
*
* OUTPUTS:
*       Lua value at the top of stack
*
* RETURNS:
*       The length of deserialized data
*
* COMMENTS:
*       Data stored as TLV { int type, size_t length, char data[] }
*
*******************************************************************************/
static int deserialize_Lua_value(lua_State *L, char *bufPtr)
{
#define TL  (1+sizeof(size_t))
    char type;
    int len;

    type = *bufPtr;
    LUACLI_MEMCPY(&len, bufPtr+1, sizeof(size_t));
    switch (type)
    {
        case SER_TYPE_NIL:
            lua_pushnil(L);
            break;
        case SER_TYPE_BOOL_TRUE:
            lua_pushboolean(L, 1);
            break;
        case SER_TYPE_BOOL_FALSE:
            lua_pushboolean(L, 0);
            break;
        case SER_TYPE_NUMBER:
            {
                lua_Number num;
                LUACLI_MEMCPY(&num, bufPtr + TL, sizeof(num));
                lua_pushnumber(L, num);
            }
            break;
        case SER_TYPE_STRING:
            lua_pushlstring(L, bufPtr + TL, len);
            break;
        case SER_TYPE_USERDATA:
            lua_newuserdata(L, len);
            LUACLI_MEMCPY(lua_touserdata(L, -1), bufPtr + TL, len);
            break;
        case SER_TYPE_LIGHTUSERDATA:
            {
                void*   ptr;
                LUACLI_MEMCPY(&ptr, bufPtr + TL, sizeof(ptr));
                lua_pushlightuserdata(L, ptr);
            }
            break;
        case SER_TYPE_FUNCTION:
            {
                lua_CFunction func;
                LUACLI_MEMCPY(&func, bufPtr + TL, sizeof(func));
                lua_pushcfunction(L, func);
            }
            break;
        case SER_TYPE_TABLE:
            {
                size_t  current_offset;
                lua_newtable(L);
                current_offset = TL;
                while (current_offset < len + TL)
                {
                    /* get key */
                    current_offset += deserialize_Lua_value(L, bufPtr + current_offset);
                    /* get value */
                    current_offset += deserialize_Lua_value(L, bufPtr + current_offset);
                    /* add */
                    lua_settable(L, -3);
                }
            }
    }
    return TL + len;
#undef TL
}


/*******************************************************************************
* keyMatch
*
* DESCRIPTION:
*       Check if key matches Lua value
*
* INPUTS:
*       L
*       keyIdx
*       keyPtr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       non-zero if Lua value at index keyIdx matches one pointed by keyPtr
*
* COMMENTS:
*
*******************************************************************************/
static int keyMatch(
    lua_State*      L,
    int             keyIdx,
    const char      *keyPtr
)
{
    const char *s;
    size_t len, k;
    s = lua_tolstring(L, keyIdx, &len);
    for (k = 0; k < KEY_LEN && k < len; k++)
    {
        if (keyPtr[k] != s[k])
            return 0;
    }
    if (k == KEY_LEN)
        return 1;
    return keyPtr[k] == 0;
}

/*******************************************************************************
* keyStore
*
* DESCRIPTION:
*       Store key
*
* INPUTS:
*       L
*       keyIdx
*
* OUTPUTS:
*       keyPtr
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static void keyStore
(
    lua_State*  L,
    int         keyIdx,
    char        *keyPtr
)
{
    const char *s;
    size_t len, k;
    s = lua_tolstring(L, keyIdx, &len);
    for (k = 0; k < KEY_LEN && k < len; k++)
    {
        keyPtr[k] = s[k];
    }
    if (k < KEY_LEN)
        keyPtr[k] = 0;
}

/*******************************************************************************
* findNode
*
* DESCRIPTION:
*       Find a node
*
* INPUTS:
*       L
*       keyIdx
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Node pointer or Null
*
* COMMENTS:
*
*******************************************************************************/
static STORAGE_NODE_STCT* findNode
(
    lua_State*      L,
    int             keyIdx
)
{
    STORAGE_NODE_STCT* node = storage->nodes;
    while (node != NULL)
    {
        if (keyMatch(L, keyIdx, node->key))
            break;
        node = node->next;
    }
    return node;
}

/*******************************************************************************
* addNode
*
* DESCRIPTION:
*       Add new node
*
* INPUTS:
*       L
*       keyIdx
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Node pointer or Null
*
* COMMENTS:
*
*******************************************************************************/
static STORAGE_NODE_STCT* addNode
(
    lua_State*      L,
    int             keyIdx
)
{
    STORAGE_NODE_STCT* node;
    node = (STORAGE_NODE_STCT*) LUACLI_MALLOC(sizeof(*node));
    if (node == NULL)
        return NULL;
    keyStore(L, keyIdx, node->key);
    node->bufPtr = NULL;

    node->next = storage->nodes;
    storage->nodes = node;

    return node;
}

/*******************************************************************************
* luaGlobalCheckInit
*
* DESCRIPTION:
*       Initialize Lua global repository if not initialized yet
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS luaGlobalCheckInit(void)
{
    char mtxName[16] = "luaGlobalMtx";

#ifdef SHARED_MEMORY
    /* allocate data in shared memory or open existing one */
    if (storage == NULL)
    {
        storage = (STORAGE_SHARED_DATA_STCT*)shrMemAllocStatic(
            "luaGlobalData",
            sizeof(*storage));
        if (storage == NULL)
            return GT_FAIL;
    }
#endif

    if (storage->initialized != 0) 
    {
        return GT_OK;
    }
    LUACLI_MUTEX_CREATE(mtxName, &storage->mtx);
    storage->initialized = 1;
    return GT_OK;
}

/***** Public Functions ************************************************/

/*******************************************************************************
* globalStore
*
* DESCRIPTION:
*       Store Lua value in global repository
*
* INPUTS:
*       L       - lua_State
*        1st    - key
*        2nd    - value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       true or false, error string
*
* COMMENTS:
*
*******************************************************************************/
int luaGlobalStore(lua_State *L)
{
    size_t              len;
    char                *bufPtr;
    STORAGE_NODE_STCT*  node;

    if (luaGlobalCheckInit() != GT_OK)
    {
        lua_pushboolean(L, 0);
        lua_pushliteral(L, "Initialization Failed");
        return 2;
    }

    if (lua_gettop(L) < 2)
    {
        /* return false */
        lua_pushboolean(L, 0);
        lua_pushliteral(L, "No enough parameters");
        return 2;
    }
    if (lua_type(L, 1) != LUA_TSTRING)
    {
        /* bad key, return false */
        lua_pushboolean(L, 0);
        lua_pushliteral(L, "Bad key, must be a string");
        return 2;
    }
    else if (lua_objlen(L, 1)>KEY_LEN) 
    {
        /* bad key, return false */
        lua_pushboolean(L, 0);
        lua_pushliteral(L, "variable name is too long");
        return 2;
    }

    STORAGE_LOCK();
    node = findNode(L, 1);
    if (node == NULL)
        node = addNode(L, 1);
    if (node == NULL)
    {
        STORAGE_UNLOCK();
        lua_pushboolean(L, 0);
        lua_pushliteral(L, "Failed to allocate new entry");
        return 2;
    }


    /* serialize data */
    lua_settop(L, 2);
    len = serialize_Lua_value(L, NULL);
    bufPtr = (char*)LUACLI_MALLOC(len);
    if (bufPtr == NULL)
    {
        STORAGE_UNLOCK();
        lua_pushboolean(L, 0);
        lua_pushliteral(L, "Failed to allocate memory for value");
        return 2;
    }
    serialize_Lua_value(L, bufPtr);

    /* now store data */
    if (node->bufPtr != NULL)
    {
        LUACLI_FREE(node->bufPtr);
    }
    node->bufPtr = bufPtr;
    STORAGE_UNLOCK();

    /* return true */
    lua_pushboolean(L, 1);
    return 1;
}

/*******************************************************************************
* globalGet
*
* DESCRIPTION:
*       Get Lua value from global repository
*
* INPUTS:
*       L       - lua_State
*        1st    - key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Value or nil if not found/wrong key
*
* COMMENTS:
*
*******************************************************************************/
int luaGlobalGet(lua_State *L)
{
    STORAGE_NODE_STCT*  node;

    if (luaGlobalCheckInit() != GT_OK)
    {
        /* not initialized, return nil */
        lua_pushnil(L);
        return 1;
    }

    if (lua_type(L, 1) != LUA_TSTRING)
    {
        /* bad key, return nil */
        lua_pushnil(L);
        return 1;
    }
    STORAGE_LOCK();
    node = findNode(L, 1);
    if (node == NULL || node->bufPtr == NULL)
    {
        /* key not found, return nil */
        STORAGE_UNLOCK();
        lua_pushnil(L);
        return 1;
    }
    deserialize_Lua_value(L, node->bufPtr);
    STORAGE_UNLOCK();
    return 1;
}

/*******************************************************************************
* luaGlobalGetIntVal
*
* DESCRIPTION:
*       Get Lua value from global repository
*
* INPUTS:
*       key         - key
*
* OUTPUTS:
*       valuePtr    - pointer to integer value
*
* RETURNS:
*       GT_OK, GT_NOT_FOUND, GT_GET_ERROR, GT_BAD_PARAM
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS luaGlobalGetIntVal(
  IN  const char *key,
  OUT GT_U32     *valuePtr
)
{
    STORAGE_NODE_STCT*  node;
    lua_Number num;

    if (valuePtr == NULL)
    {
        return GT_BAD_PARAM;
    }
    if (luaGlobalCheckInit() != GT_OK)
    {
        /* not initialized, return nil */
        return GT_NOT_FOUND;
    }

    STORAGE_LOCK();
    /* find node */
    node = storage->nodes;
    while (node != NULL)
    {
        if (LUACLI_STRNCMP(key, node->key, KEY_LEN) == 0)
            break;
        node = node->next;
    }
    if (node == NULL || node->bufPtr == NULL)
    {
        /* key not found, return nil */
        STORAGE_UNLOCK();
        return GT_NOT_FOUND;
    }
    if (node->bufPtr[0] != SER_TYPE_NUMBER)
    {
        /* not a numeric value */
        STORAGE_UNLOCK();
        return GT_GET_ERROR;
    }

    LUACLI_MEMCPY(&num, node->bufPtr + (1+sizeof(size_t)), sizeof(num));
    *valuePtr = (GT_U32)num;
    STORAGE_UNLOCK();
    return GT_OK;
}


#define TEST
#ifdef  TEST
#include <stdio.h>
int testSer(lua_State *L)
{
    size_t  len;
    lua_settop(L, 1);
    len = serialize_Lua_value(L, NULL);
    lua_newuserdata(L, len);
    lua_pushvalue(L, -2);
    serialize_Lua_value(L, lua_touserdata(L, -2));
    lua_pop(L, 1);
    return 1;
}

int dumpUD(lua_State *L)
{
    size_t  len, k;
    char    *buf;
    if (lua_type(L, 1) != LUA_TUSERDATA)
    {
        printf("not a userdata\n");
        return 0;
    }
    len = lua_objlen(L, 1);
    buf = lua_touserdata(L, 1);
    if (len)
    {
        for (k = 0; k < len; k++)
        {
            if (k % 16 == 0 && k)
                printf("\n");
            printf(" %02x", (unsigned char)buf[k]);
        }
        printf("\n");
    }
    else
        printf("len == 0\n");
    return 0;
}

int testDeser(lua_State *L)
{
    if (lua_type(L, 1) != LUA_TUSERDATA)
    {
        return 0;
    }
    deserialize_Lua_value(L, lua_touserdata(L, 1));
    return 1;
}
#endif /* TEST */

