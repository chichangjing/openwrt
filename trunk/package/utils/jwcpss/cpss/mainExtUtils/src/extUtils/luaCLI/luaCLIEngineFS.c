/****************************************************
* luaCLIEngineFS.c
*
* DESCRIPTION:
*       genFS support for Lua:
*       - Lua loader
*       - Lua API to access files
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <lua.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <extUtils/luaCLI/luaCLIEngine.h>


/***** Private Types ***************************************************/

typedef struct LoadF {
  genFS_PTR fs;
  int       fd;
  char      buff[1024];
} LoadF;

/***** Private Data ****************************************************/

/***** Public Functions ************************************************/

/*******************************************************************************
* LuaEngineFSReader
*
* DESCRIPTION:
*       The reader function used by lua_load
*
* INPUTS:
*       L       - Lua state
*       file    - data pointer. will point to variable with pointer returned
*                 by genFS open()
*
* OUTPUTS:
*       size    - chunk size
*
* RETURNS:
*       pointer to chunk data
*
* COMMENTS:
*       Example of luaEngineFSReader() usage:
*           void *file;
*           LoadF   lf;
*
*           lf.fd = lf.fs->open(filename, GEN_FS_O_RDONLY);
*           if (lf.fd < 0)
*               return ERR;
*           ... read first chunk
*           data = luaEngineFSReader(L, &lf, &size);
*           ... read next chunk
*           data = luaEngineFSReader(L, &lf, &size);
*
*******************************************************************************/
static const char * luaEngineFSReader(lua_State *L, LoadF* lf, size_t *size)
{
    int k;
    (void)L;
    k = lf->fs->read(lf->fd, lf->buff, sizeof(lf->buff));
    if (k < 0) /* eof or error */
        return NULL;
    *size = k;
    return (k > 0) ? lf->buff : NULL;
}

static const char *pushnexttemplate (lua_State *L, const char *path) {
  const char *l;
  char sep = *LUA_PATHSEP;
  while (*path == *LUA_PATHSEP) path++;  /* skip separators */
  if (*path == '\0') return NULL;  /* no more templates */
  l = path;
  while (*l != '\0' && *l != sep)
      l++;
  lua_pushlstring(L, path, l - path);  /* template */
  return l;
}


/*******************************************************************************
* luaEngineFsFindFile
*
* DESCRIPTION:
*       lookup file in genFS using relative paths from package.path
*
* INPUTS:
*       L       - Lua state
*       fs      - genFS API
*       name    - filename
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NULL or filename
*
* COMMENTS:
*
*******************************************************************************/
static const char* luaEngineFsFindFile(lua_State *L, genFS_PTR fs, const char *name)
{
  const char *path;
  name = luaL_gsub(L, name, ".", "/");
  lua_getglobal(L, "package");
  lua_getfield(L, -1, "path");
  lua_remove(L, -2);
  path = lua_tostring(L, -1);
  if (path == NULL)
    luaL_error(L, LUA_QL("package.path") " must be a string");
  lua_pushstring(L, "");  /* error accumulator */
  while ((path = pushnexttemplate(L, path)) != NULL) {
    const char *filename;
    int fd;
    filename = luaL_gsub(L, lua_tostring(L, -1), LUA_PATH_MARK, name);
    if (filename[0] != '.' || (filename[1] != '/' && filename[1] != '\\'))
    {
        /* skip all except ./ */
        lua_pop(L, 2);
        continue;
    }
    fd = fs->open(filename+2, GEN_FS_O_RDONLY);
    if (fd >= 0)
    {
        /* file exist and is readable */
        fs->close(fd);
        return filename+2;
    }
    lua_pop(L, 2);  /* remove path template and file name */ 
    lua_pushfstring(L, "\n\tno genFS file " LUA_QS, filename+2);
    lua_concat(L, 2);
  }
  return NULL;  /* not found */
}


/*******************************************************************************
* luaEngineFSLoader
*
* DESCRIPTION:
*       package loader for genFS
*
* INPUTS:
*       L       - Lua state
*         1st   - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       lua function or error message
*
* COMMENTS:
*
*******************************************************************************/
static int luaEngineFSLoader(lua_State *L)
{
    LoadF lf;
    int status;
    const char *filename;
    int fnameindex = lua_gettop(L) + 1;  /* index of filename on the stack */
    if (!lua_isstring(L, 1))
    {
        lua_pushliteral(L, "\n\tluaEngineFSLoader: first parameter must be a string");
        return 1;
    }
    lf.fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!lf.fs)
    {
        lua_pushliteral(L, "\n\t_genFS API not defined");
        return 1;
    }
#if 0
    lua_pushvalue(L, 1);
    lua_pushliteral(L, ".lua");
    lua_concat(L, 2);

    lf.fd = lf.fs->open(lua_tostring(L, -1), GEN_FS_O_RDONLY);
#else
    filename = luaEngineFsFindFile(L, lf.fs, lua_tostring(L, 1));
    if (filename == NULL)
    {
        return 1;
    }
    lf.fd = lf.fs->open(filename, GEN_FS_O_RDONLY);
#endif
    if (lf.fd < 0)
    {
        lua_pushliteral(L, "\n\tgenFS open(\"");
        lua_insert(L, -2);
        lua_pushliteral(L, "\"): ");
        lua_pushstring(L, lf.fs->lastError());
        lua_concat(L, 4);
        
        return 1;
    }

    status = lua_load(L, (lua_Reader)luaEngineFSReader, &lf, lua_tostring(L, 1));
    lf.fs->close(lf.fd);
    if (status != 0)
    {
        lua_Debug ar;
        lua_pushliteral(L, "");  /* else, no information available... */
        if (lua_getstack(L, 1, &ar)) {  /* check function at level */
            lua_getinfo(L, "Sl", &ar);  /* get info about it */
            if (ar.currentline > 0) {  /* is there info? */
                lua_pushfstring(L, "%s:%d: ", ar.short_src, ar.currentline);
                lua_concat(L, 2);
            }
        }

        lua_pushfstring(L, "error loading module " LUA_QS " from file " LUA_QS ":\n\t%s",
                        lua_tostring(L, 1), lua_tostring(L, fnameindex), lua_tostring(L, -1));
        lua_concat(L, 2);
        lua_error(L);
    }
    return 1;
}


/*******************************************************************************
* luaEngineFS_loadfile
*
* DESCRIPTION:
*       loads file as Lua chunk. Cycle through all loaders
*
* INPUTS:
*       L       - Lua state
*       name    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       error string or function to call
*
* COMMENTS:
*
*******************************************************************************/
int luaEngineFS_loadfile(lua_State *L, const char *name)
{
    /* get package.loader */
    int status;
    LoadF lf;

    lf.fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!lf.fs)
    {
        return LUA_ERRFILE;
    }
    status = luaL_loadfile(L, name);
    if (status != LUA_ERRFILE)
    {
        return status;
    }

    lf.fd = lf.fs->open(name, GEN_FS_O_RDONLY);
    if (lf.fd < 0)
    {
        lua_pushfstring(L, "\n\tgenFS open(" LUA_QS "): %s", name, lf.fs->lastError());
        lua_concat(L, 2);
        
        return LUA_ERRFILE;
    }
    lua_pop(L, 1); /* remove "file not found" returned by luaL_loadfile() */

    status = lua_load(L, (lua_Reader)luaEngineFSReader, &lf, name);
    lf.fs->close(lf.fd);
    if (status != 0)
    {
        lua_Debug ar;
        lua_pushfstring(L, "error loading file " LUA_QS ":\n\t", name);
        if (lua_getstack(L, 1, &ar)) {  /* check function at level */
            lua_getinfo(L, "Sl", &ar);  /* get info about it */
            if (ar.currentline > 0) {  /* is there info? */
                lua_pushfstring(L, "%s:%d: ", name, ar.currentline);
                lua_concat(L, 2);
            }
        }
        lua_insert(L, -2);
        lua_concat(L, 2);
        return status;
    }
    return status;
}

/*******************************************************************************
* l_loadfile
*
* DESCRIPTION:
*       loadfile() with genFS support
*
* INPUTS:
*       L       - Lua state
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
static int l_loadfile(lua_State *L)
{
    const char *fname = luaL_optstring(L, 1, NULL);
    luaEngineFS_loadfile(L, fname);
    if (lua_isfunction(L, -1)) /* ok? */
    {
        return 1;
    }
    lua_pushnil(L);
    lua_insert(L, -2);  /* put before error message */
    return 2;  /* return nil plus error message */
}

/*******************************************************************************
* l_dofile
*
* DESCRIPTION:
*       dofile() with genFS support
*
* INPUTS:
*       L       - Lua state
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
static int l_dofile(lua_State *L)
{
    const char *fname = luaL_optstring(L, 1, NULL);
    int n = lua_gettop(L);
    luaEngineFS_loadfile(L, fname);
    if (!lua_isfunction(L, -1)) /* err? */
    {
        lua_error(L);
    }
    lua_call(L, 0, LUA_MULTRET);
    return lua_gettop(L) - n;
}




/*******************************************************************************
* luaEngineFSregisterLoader
*
* DESCRIPTION:
*       register package loader for genFS
*
* INPUTS:
*       L       - Lua state
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
int luaEngineFSregisterLoader(lua_State *L)
{
    /* package loader */
    lua_getfield(L, LUA_GLOBALSINDEX, "package");
    lua_getfield(L, -1, "loaders");
    if (lua_istable(L, -1))
    {
        int i;
        for (i=1; ; i++) {
            lua_rawgeti(L, -1, i);  /* get a loader */
            if (lua_isnil(L, -1))
                break;
            /* already registered */
            if (lua_tocfunction(L, -1) == luaEngineFSLoader)
                break;
            lua_pop(L, 1);
        }
        lua_pop(L,1);
        lua_pushcfunction(L, luaEngineFSLoader);
        lua_rawseti(L, -2, i);
    }
    lua_pop(L, 2);

    /* override loadfile(), dofile() with ones supporting genFS */
    lua_pushcfunction(L, l_loadfile);
    lua_setfield(L, LUA_GLOBALSINDEX, "loadfile");
    lua_pushcfunction(L, l_dofile);
    lua_setfield(L, LUA_GLOBALSINDEX, "dofile");

    return 0;
}



/*******************************************************************************
* l_genFSopen
*
* DESCRIPTION:
*       fs.open
*       Open and possible create a new file
*
* INPUTS:
*       L       - Lua state
*        1st    - file name
*        2nd    - open flags:
*                   "r" : read mode(the default)
*                   "w" : write mode;
*                   "a" : append mode;
*                   "r+": update mode, all previous data is preserved;
*                   "w+": update mode, all previous data is erased;
*                   "a+": append update mode, previous data is preserved,
*                         writing is only allowed at the end of file.
*                   "wc": write mode, compress data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       file handle
*       nil + error message if error
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSopen(lua_State *L)
{
    int flags = GEN_FS_O_RDONLY;
    int fd;
    genFS_PTR fs;
    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "genFS not defined");
        return 2;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameter");
        return 2;
    }
    if (lua_isstring(L, 2))
    {
#define OPT(str, value) \
        lua_pushliteral(L, str); \
        if (lua_equal(L, -1, 2)) \
            flags = value;
        OPT("r", GEN_FS_O_RDONLY);
        OPT("w", GEN_FS_O_WRONLY|GEN_FS_O_CREAT);
        OPT("a", GEN_FS_O_WRONLY|GEN_FS_O_CREAT|GEN_FS_O_APPEND);
        OPT("r+", GEN_FS_O_RDWR|GEN_FS_O_CREAT);
        OPT("w+", GEN_FS_O_RDWR|GEN_FS_O_CREAT|GEN_FS_O_TRUNC);
        OPT("a+", GEN_FS_O_RDWR|GEN_FS_O_CREAT|GEN_FS_O_APPEND);
        OPT("wc", GEN_FS_O_WRONLY|GEN_FS_O_CREAT|GEN_FS_O_COMPRESS);
#undef  OPT
    }
    fd = fs->open(lua_tostring(L, 1), flags);
    if (fd < 0)
    {
        lua_pushnil(L);
        lua_pushstring(L, fs->lastError());
        return 2;
    }
    lua_pushinteger(L, fd);
    return 1;
}

/*******************************************************************************
* l_genFSclose
*
* DESCRIPTION:
*       fs.close
*       Close a file descriptor
*
* INPUTS:
*       L       - Lua state
*        1st    - file descriptor
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
static int l_genFSclose(lua_State *L)
{
    if (lua_isnumber(L, 1))
    {
        genFS_PTR fs;
        fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
        if (fs)
            fs->close((int)lua_tointeger(L, 1));
    }
    return 0;
}

/*******************************************************************************
* l_genFSread
*
* DESCRIPTION:
*       fs.read
*       read from a file descriptor
*
* INPUTS:
*       L       - Lua state
*        1st    - file descriptor
*        2nd    - number of bytes to read
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string or nil + error message if error
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSread(lua_State *L)
{
    char    buf[1024];
    int     fd, len;
    int     ret;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "genFS not defined");
        return 2;
    }
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameter");
        return 2;
    }

    fd = (int)lua_tointeger(L, 1);
    len = (int)lua_tointeger(L, 2);

    lua_pushliteral(L, "");
    while (len > 0)
    {
        ret = fs->read(fd, buf, (unsigned)len < sizeof(buf) ? len : (int)(sizeof(buf)));
        if (ret < 0)
        {
            if (lua_objlen(L, -1) > 0)
            {
                return 1;
            }
            lua_pop(L, 1);
            lua_pushnil(L);
            lua_pushstring(L, fs->lastError());
            return 2;
        }
        if (ret == 0)
        {
            /* EOF */
            return 1;
        }
        lua_pushlstring(L, buf, ret);
        lua_concat(L, 2);
        len -= ret;
    }
    return 1;
}


/*******************************************************************************
* l_genFSgets
*
* DESCRIPTION:
*       fs.gets
*       read line from a file descriptor
*
* INPUTS:
*       L       - Lua state
*        1st    - file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string or nil + error message if error
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSgets(lua_State *L)
{
    char    buf[1024];
    int     fd, pos;
    int     ret;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "genFS not defined");
        return 2;
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameter");
        return 2;
    }

    fd = (int)lua_tointeger(L, 1);

    lua_pushliteral(L, "");
    pos = 0;
    while (1)
    {
        ret = fs->read(fd, buf+pos, 1);
        if (ret <= 0)
        {
            if (lua_objlen(L, -1) + pos > 0)
            {
                if (pos > 0)
                {
                    lua_pushlstring(L, buf, pos);
                    lua_concat(L, 2);
                }
                return 1;
            }
            /* eof reached return error */
            lua_pop(L, 1);
            lua_pushnil(L);
            if (ret < 0)
            {
                lua_pushstring(L, fs->lastError());
            }
            else
            {
                lua_pushliteral(L, "<EOF>");
            }
            return 2;
        }
        if (buf[pos] == '\n')
        {
            lua_pushlstring(L, buf, pos+1);
            lua_concat(L, 2);
            break;
        }
        pos++;
        if ((unsigned)pos >= sizeof(buf))
        {
            lua_pushlstring(L, buf, pos);
            lua_concat(L, 2);
            pos = 0;
        }
    }
    return 1;
}

/*******************************************************************************
* l_genFSwrite
*
* DESCRIPTION:
*       fs.write
*       Write to a file descriptor
*
* INPUTS:
*       L       - Lua state
*        1st    - file descriptor
*        2nd    - string to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of bytes written or nil + error message
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSwrite(lua_State *L)
{
    int     fd;
    int     ret;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "genFS not defined");
        return 2;
    }

    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameter");
        return 2;
    }
    fd = (int)lua_tointeger(L, 1);
    ret = fs->write(fd, lua_tostring(L, 2), (int)lua_objlen(L, 2));
    if (ret < 0)
    {
        lua_pop(L, 1);
        lua_pushnil(L);
        lua_pushstring(L, fs->lastError());
        return 2;
    }
    lua_pushinteger(L, ret);
    return 1;
}


/*******************************************************************************
* l_genFSlseek
*
* DESCRIPTION:
*       fs.lseek
*       reposition read/write file offset
*
* INPUTS:
*       L       - Lua state
*        1st    - file descriptor
*        2nd    - offset
*        3rd    - one of
*                 0   - The offset is set to offset bytes
*                 1   - The offset is set to current location
*                       plus offset bytes
*                 2   - The offset is set to size of the file
*                       PLUS offset bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       the resulting offset location as measured in bytes from the
*                 beginning of the file.
*       -1 if error
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSlseek(lua_State *L)
{
    int     ret;
    int     fd, offset;
    int     whence = GEN_FS_SEEK_SET;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "genFS not defined");
        return 2;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameter");
        return 2;
    }
    fd = (int)lua_tointeger(L, 1);
    offset = (int)lua_tointeger(L, 2);
    if (lua_isnumber(L, 3))
    {
        if (lua_tointeger(L, 3) == 0)
            whence = GEN_FS_SEEK_SET;
        if (lua_tointeger(L, 3) == 1)
            whence = GEN_FS_SEEK_CUR;
        if (lua_tointeger(L, 3) == 2)
            whence = GEN_FS_SEEK_END;
    }
    ret = fs->lseek(fd,offset,whence);
    lua_pushinteger(L, ret);
    return 1;
}

/*******************************************************************************
* l_genFSstat
*
* DESCRIPTION:
*       fs.stat
*       get file status
*
* INPUTS:
*       L       - Lua state
*        1st    - file name
*
* OUTPUTS:
*
* RETURNS:
*       stat structure or nil
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSstat(lua_State *L)
{
    GEN_FS_STAT_STC st;
    int result;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        return 1;
    }

    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    result = fs->stat(lua_tostring(L, 1), &st);
    if (result < 0)
    {
        lua_pushnil(L);
        return 1;
    }
    lua_newtable(L);
    lua_pushinteger(L, st.st_size);
    lua_setfield(L, -2, "st_size");
    lua_pushinteger(L, st.st_mode);
    lua_setfield(L, -2, "st_mode");
    lua_pushnumber(L, st.st_ctime);
    lua_setfield(L, -2, "st_ctime");
    lua_pushnumber(L, st.st_mtime);
    lua_setfield(L, -2, "st_mtime");
    lua_pushstring(L, GEN_FS_S_ISREG(st.st_mode)
            ? "file"
            : (GEN_FS_S_ISDIR(st.st_mode)
                ? "dir"
                : "unk"));
    lua_setfield(L, -2, "type");
    return 1;
}

/*******************************************************************************
* l_genFSexists
*
* DESCRIPTION:
*       fs.exists
*       check if file/directory exists
*
* INPUTS:
*       L       - Lua state
*        1st    - file name
*
* OUTPUTS:
*
* RETURNS:
*       boolean
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSexists(lua_State *L)
{
    GEN_FS_STAT_STC st;
    int result;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        return 1;
    }

    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    result = fs->stat(lua_tostring(L, 1), &st);
    lua_pushboolean(L, (result < 0) ? 0 : 1);
    return 1;
}

/*******************************************************************************
* l_genFSfileexists
*
* DESCRIPTION:
*       fs.fileexists
*       check if file exists and regular file
*
* INPUTS:
*       L       - Lua state
*        1st    - file name
*
* OUTPUTS:
*
* RETURNS:
*       boolean
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSfileexists(lua_State *L)
{
    GEN_FS_STAT_STC st;
    int result;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        return 1;
    }

    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    result = fs->stat(lua_tostring(L, 1), &st);
    lua_pushboolean(L, (result >= 0 && GEN_FS_S_ISREG(st.st_mode)) ? 1 : 0);
    return 1;
}

/*******************************************************************************
* l_genFSunlink
*
* DESCRIPTION:
*       fs.unkink
*       delete a name and possibly the file it refers to
*
* INPUTS:
*       L       - Lua state
*        1st    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - on success
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSunlink(lua_State *L)
{
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushinteger(L, -1);
        return 1;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushinteger(L, -1);
        return 1;
    }
    lua_pushinteger(L, fs->unlink(lua_tostring(L, 1)));
    return 1;
}

/*******************************************************************************
* l_genFSopendir
*
* DESCRIPTION:
*       fs.opendir
*       open a directory
*
* INPUTS:
*       L       - Lua state
*        1st    - directory name
*                 (will beignored in current implementation)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       directory file descriptor or nil + error message
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSopendir(lua_State *L)
{
    int fd;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        lua_pushliteral(L, "genFS not defined");
        return 2;
    }
    if (!lua_isstring(L, 1))
    {
        lua_pushnil(L);
        lua_pushliteral(L, "Bad parameter");
        return 2;
    }
    fd = fs->opendir(lua_tostring(L, 1));
    if (fd < 0)
    {
        lua_pushnil(L);
        lua_pushstring(L, fs->lastError());
        return 2;
    }
    lua_pushinteger(L, fd);
    return 1;
}

/*******************************************************************************
* l_genFSclosedir
*
* DESCRIPTION:
*       fs.closedir
*       close a directory
*
* INPUTS:
*       L       - Lua state
*        1st    - directory file descriptor
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
static int l_genFSclosedir(lua_State *L)
{
    if (lua_isnumber(L, 1))
    {
        genFS_PTR fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
        if (fs)
            fs->closedir((int)lua_tointeger(L, 1));
    }
    return 0;
}


/*******************************************************************************
* l_genFSreaddir
*
* DESCRIPTION:
*       fs.readdir
*       read a directory entry
*
* INPUTS:
*       fd      - directory file descriptor
*
* OUTPUTS:
*       L       - Lua state
*        1st    - directory file descriptor
*
* RETURNS:
*       directory entry
*       0 On end of directory
*       nil + error string On error
*
* COMMENTS:
*
*******************************************************************************/
static int l_genFSreaddir(lua_State *L)
{
    GEN_FS_DIRENT_STC   dirent;
    GEN_FS_STAT_STC     st;
    int dirFd;
    int result;
    genFS_PTR fs;

    fs = (genFS_PTR)luaCLIEngineGetData(L, "_genFS", 0);
    if (!fs)
    {
        lua_pushnil(L);
        return 1;
    }

    if (!lua_isnumber(L, 1))
    {
        lua_pushnil(L);
        return 1;
    }
    dirFd = (int)lua_tointeger(L, 1);
    result = fs->readdir(dirFd, &dirent);
    if (result < 0)
    {
        lua_pushnil(L);
        lua_pushstring(L, fs->lastError());
        return 2;
    }
    if (result == 0)
    {
        lua_pushinteger(L, 0);
        return 1;
    }
    lua_newtable(L);
    lua_pushstring(L, dirent.d_name);
    lua_setfield(L, -2, "d_name");
    if (fs->stat(dirent.d_name, &st) >= 0)
    {
        lua_pushinteger(L, st.st_size);
        lua_setfield(L, -2, "st_size");
    }
    return 1;
}

static const luaL_Reg genfslib[] = {
  {"open",      l_genFSopen},
  {"close",     l_genFSclose},
  {"read",      l_genFSread},
  {"write",     l_genFSwrite},
  {"lseek",     l_genFSlseek},
  {"gets",      l_genFSgets},
  {"stat",      l_genFSstat},
  {"exists",    l_genFSexists},
  {"fileexists",l_genFSfileexists},
  {"unlink",    l_genFSunlink},
  {"opendir",   l_genFSopendir},
  {"closedir",  l_genFSclosedir},
  {"readdir",   l_genFSreaddir},
  {NULL, NULL}
};

/*******************************************************************************
* luaEngineFSregisterAPI
*
* DESCRIPTION:
*       register cmdFS API in Lua instance
*
* INPUTS:
*       L       - Lua state
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
int luaEngineFSregisterAPI(lua_State *L)
{
    luaL_register(L, "fs", genfslib);
    lua_pop(L, 1); /* pop the table from stack */
    return 0;
}


/***** Lua API Documentation **************************************************
*
*   fs.open(
*       filename,
*       mode            One of "r", "w", "a", "r+", "w+", "a+", "wc"
*   )                   Open file (using cmdFS)
*                       Returns file descriptor or nil + error_message
*
*   fs.close(fd)        Close file
*
*   fs.read(
*       fd,
*       nBbytes
*   )                   Read up to nBytes bytes from file
*                       Returns string or nil + error_message
*
*   fs.gets(fd)         Read a line from file
*                       Returns string or nil + error_message
*                       Special error message: <EOF>
*
*   fs.write(
*       fd,
*       string
*   )                   Write string to file
*                       Returns number of bytes nil + error_message
*
*   fs.lseek(
*       fd,
*       offset
*       whence)         whence == 0 offset is set to offset bytes
*                       whence == 1 The offset is set to current location
*                               plus offset bytes
*                       whence == 2 The offset is set to size of the file
*                               PLUS offset bytes
*                       Reposition read/write file offset
*                       Returns resulting offset location from the begin
*                       or < 0 if error
*
*   fs.stat(filename)   get file status
*                       Returns table:
*                       {
*                           st_size = <file_size>,
*                           st_mode = <mode_flags>,
*                           st_ctime = <time_t_value>,
*                           st_mtime = <time_t_value>,
*                           type = "file" | "dir" | etc
*                       } or nil if no file
*
*   fs.exists(name)     check if file/directory exists
*                       return boolean
*
*   fs.fileexists(name) check if file exists and regular file
*                       return boolean
*
*   fs.unlink(filename) Unlink (remove) file
*                       Returns 0 on success
*
*   fs.opendir(dirname) Open directory for read
*                       returns file descriptor or nil + error message
*
*   fs.closedir(fd)     Close directory
*
*   fs.readdir(fd)      Read directory
*                       Returns: { d_name = <file_name>, st_size = file_size }
*                       or 0 on the end of directory or nil + string on error
*
******************************************************************************/
