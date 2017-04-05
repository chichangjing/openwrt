/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdLuaConfigFile.c
*
* DESCRIPTION:
*       a LUA CLI configuration file support
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
**************************************************************************/

#include <cmdShell/os/cmdOs.h>
#include <cmdShell/FS/cmdFS.h>
#include <extUtils/luaCLI/luaCLIEngineCLI.h>
#include <lualib.h>
#include <stdio.h>

#ifdef ENV_MAINCMD
# include "cmdLua_os_mainCmd.inc"
#endif
#ifdef ENV_POSIX
# include "cmdLua_os_posix.inc"
#endif

/* externs */
GT_STATUS cmdFS_tftp_get(
    IN  const char  *server,
    IN  GT_U16      port,
    IN  const char  *localfile,
    IN  const char  *remotefile,
    IN  GT_BOOL     compress,
    OUT char        *errorBuf
);
int luaCLI_init
(
    IOStreamPTR IOStream,
    lua_State **luaPtr,
    void *ud
);

/*******************************************************************************
* prvFileNamePath
*
* DESCRIPTION:
*       make full path for config file: add directory
*
* INPUTS:
*       name    - file name, eg "config.txt"
*
* OUTPUTS:
*       None
*
* RETURNS:
*       absolute file path "/path/to/startup/dir/config.txt"
*       NULL on error
*
* COMMENTS:
*       must be freed by cmdOsFree()
*
*******************************************************************************/
static char* prvFileNamePath(
  IN const char *name
)
{
    GT_U32 len = 0;
    char *ret;
    if (cmdOsConfigFilePath != NULL)
    {
        len = cmdOsStrlen(cmdOsConfigFilePath);
    }
    len += cmdOsStrlen(name) + 1;
    ret = (GT_CHAR*)cmdOsMalloc(len);
    if (ret == NULL)
    {
        return NULL;
    }
    if (cmdOsConfigFilePath != NULL)
    {
        cmdOsStrCpy(ret, cmdOsConfigFilePath);
    }
    else
        ret[0] = 0;
    cmdOsStrCat(ret, name);
    return ret;
}

#ifndef VXWORKS
/*******************************************************************************
* prvCopyLocalFile
*
* DESCRIPTION:
*       Copy file on HOST FS
*
* INPUTS:
*       srcName - source file name eg "/path/to/startup/dir/config.txt"
*       dstName - destication file name eg "/path/to/startup/dir/config.txt.bak"
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCopyLocalFile(
  IN const char *srcName,
  IN const char *dstName
)
{
    FILE *fdst, *fsrc;
    char buf[64];
    size_t r;
    fsrc = fopen(srcName,"r");
    if (fsrc == NULL)
        return GT_FAIL;
    fdst = fopen(dstName,"w+");
    if (fdst == NULL)
    {
        fclose(fsrc);
        return GT_FAIL;
    }
    while (!feof(fsrc))
    {
        r = fread(buf, 1, sizeof(buf), fsrc);
        if (r == 0)
            break;
        if (fwrite(buf, 1, r, fsrc) != r)
        {
            fclose(fsrc);
            fclose(fdst);
            return GT_FAIL;
        }
    }
    fclose(fsrc);
    fclose(fdst);
    return GT_OK;
}
#endif

/*******************************************************************************
* cmdCopyFromRAMFStoDisk
*
* DESCRIPTION:
*       copy a file (if exist) from the RAM-FS to the hard-drive
*
* INPUTS:
*        L       - Lua state
*        1st    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 - on success
*       -1 - on failure
*
* COMMENTS:
*
*******************************************************************************/
int cmdCopyFromRAMFStoDisk(lua_State *L)
{
#ifndef VXWORKS
    int  configFD_RAMFS = -1;
    FILE *of = NULL;
    char *configPath = NULL;
    char *backupPath = NULL;
    int  status = -1;
    char buf[64];
    int  rc;
    
    do
    {
        if (!lua_isstring(L, 1))
            break;

        if ((configFD_RAMFS = cmdFS.open(lua_tostring(L, 1), GEN_FS_O_RDONLY)) < 0)
            break;

        /* create backup */
        if ((configPath = prvFileNamePath(lua_tostring(L, 1))) == NULL)
            break;
        lua_pushvalue(L, 1);
        lua_pushliteral(L, ".bak");
        lua_concat(L, 2);
        if ((backupPath = prvFileNamePath(lua_tostring(L, -1))) != NULL)
            prvCopyLocalFile(configPath, backupPath);

        /* save */
        of = fopen(configPath, "w+");
        if (of == NULL)
            break;

        /* now copy */
        while ((rc = cmdFS.read(configFD_RAMFS, buf, sizeof(buf))) > 0 )
        {
            if (fwrite(buf, 1, rc, of) == 0)
                break;
        }
        if (rc <= 0)
            status = 0;

    } while(0);
    /* cleanup */
    if (configFD_RAMFS >= 0)
        cmdFS.close(configFD_RAMFS);
    if (configPath != NULL)
        cmdOsFree(configPath);
    if (backupPath != NULL)
        cmdOsFree(backupPath);
    if (of != NULL)
        fclose(of);

    lua_pushinteger(L, status);
    return 1;

#else
    
    lua_pushinteger(L, 0);
    return 1;

#endif /* VXWORKS */
}



/*******************************************************************************
* prvLoadFileFromLocalFS
*
* DESCRIPTION:
*       Load file from OS filesystem to cmdFS
*
* INPUTS:
*       filename    filename to load
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvLoadFileFromLocalFS(
  IN const char *filename
)
{
    char *configPath = NULL;
    FILE *fi;
    int fd, r;
    char buf[64];
    if ((configPath = prvFileNamePath(filename)) == NULL)
        return GT_FAIL;
    fi = fopen(configPath, "r");
    cmdOsFree(configPath);
    if (fi == NULL)
    {
        return GT_FAIL;
    }
    
    fd = cmdFS.open(filename, GEN_FS_O_WRONLY | GEN_FS_O_CREAT | GEN_FS_O_TRUNC);
    if (fd < 0)
    {
        fclose(fi);
        return GT_FAIL;
    }
    while(!feof(fi))
    {
        r = (int)fread(buf,1,sizeof(buf), fi);
        if (r <= 0)
            break;
        cmdFS.write(fd,buf,r);
    }
    fclose(fi);
    cmdFS.close(fd);
    return GT_OK;
}

/*******************************************************************************
* wraplLoadFileFromLocalFS
*
* DESCRIPTION:
*       Load file from OS filesystem to cmdFS (Lua Wrapper)
*
* INPUTS:
*        L       - Lua state
*        1st    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
int wraplLoadFileFromLocalFS(lua_State *L)
{
#ifndef VXWORKS
    if (!lua_isstring(L, 1))
    {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L,
            prvLoadFileFromLocalFS(lua_tostring(L, 1)) == GT_OK
            ? 1
            : 0);
    return 1;
#else
    lua_pushboolean(L, 0);
    return 1;
#endif
}

/*******************************************************************************
* prvLoadConfigFile
*
* DESCRIPTION:
*       Load configuration file from OS filesystem/tftp server to cmdFS
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvLoadConfigFile(GT_VOID)
{
    GEN_FS_STAT_STC st;
    GT_STATUS ret;

    /* check if file already loaded */
    if (cmdFS.stat(cmdOsConfigFileName,&st) == 0)
        return GT_OK;

    if (prvLoadFileFromLocalFS(cmdOsConfigFileName) == GT_OK)
        return GT_OK;

    {
        /* try to load file from tftp server */
        char server_ip[30],local_ip[30],remote_name[80], errBuf[128];
        ret = cmdOsGetServerAndLocalIP(local_ip, server_ip);
        if (ret == GT_OK)
        {
            if (local_ip[0])
            {
                cmdOsSprintf(remote_name,"%s.%s",cmdOsConfigFileName,local_ip);
                ret = cmdFS_tftp_get(server_ip, 69, cmdOsConfigFileName, remote_name, GT_FALSE, errBuf);
                if (ret == GT_OK)
                    return ret;
                cmdOsPrintf("Failed to get %s from %s: %s\n",remote_name, server_ip, errBuf);
            }
            ret = cmdFS_tftp_get(server_ip, 69, cmdOsConfigFileName, cmdOsConfigFileName,GT_FALSE, errBuf);
            if (ret == GT_OK)
                return ret;
            cmdOsPrintf("Failed to get %s from %s: %s\n",cmdOsConfigFileName, server_ip, errBuf);
        }
    }
    /* try other methods to load file */

    return GT_FAIL;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/************* stdout stream implementation (no input) ***************************/
static int stdoutStreamDestroy(IOStreamPTR stream GT_UNUSED)
{
    return 0;
}
static int stdoutStreamRead(IOStreamPTR stream GT_UNUSED, void* bufferPtr GT_UNUSED, int bufferLength GT_UNUSED)
{
    return 0;
}
static int stdoutStreamReadChar(IOStreamPTR stream GT_UNUSED, char* charPtr GT_UNUSED, int timeOut GT_UNUSED)
{
    return -1;
}
static int stdoutStreamReadLine(IOStreamPTR stream GT_UNUSED, char* bufferPtr GT_UNUSED, int bufferLength GT_UNUSED)
{
    return 0;
}
static int stdoutStreamWrite(IOStreamPTR stream GT_UNUSED, const void* bufferPtr, int bufferLength)
{
    cmdOsPrintf("%*s",bufferLength,(const char*)bufferPtr);
    return bufferLength;
}
static int stdoutStreamWriteBuf(IOStreamPTR stream GT_UNUSED, const char* s, int len)
{
    cmdOsPrintf("%*s",len,(const char*)s);
    return len;
}
static int stdoutStreamWriteLine(IOStreamPTR stream GT_UNUSED, const char *s)
{
    cmdOsPrintf("%s",s);
    return cmdOsStrlen(s);
}
static int stdoutStreamConnected(IOStreamPTR stream GT_UNUSED)
{
    return 1;
}
static int stdoutStreamSetTtyMode(IOStreamPTR stream GT_UNUSED, int mode GT_UNUSED)
{
    return 0;
}

static IOStreamSTC stdoutStream = {
    stdoutStreamDestroy,
    stdoutStreamRead,
    stdoutStreamReadChar,
    stdoutStreamReadLine,
    stdoutStreamWrite,
    stdoutStreamWriteBuf,
    stdoutStreamWriteLine,
    stdoutStreamConnected,
    NULL,     /* grabStd */
    stdoutStreamSetTtyMode,
    NULL,     /* getFd */
    GT_FALSE, /* isConsole */
    NULL,     /* customPtr */
    0,        /* flags */
    GT_FALSE  /* wasCr */
};


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

typedef void* (*LUACLI_MALLOC_T)(int size);

/*******************************************************************************
* luaCLI_LoadConfigurationFile
*
* DESCRIPTION:
*       Load configuration file from OS filesystem/tftp server to cmdFS,
*       then execute commands
*
* INPUTS:
*       None
*
* OUTPUTS:
*       luaInstancePtrPtr
*
* RETURNS:
*       GT_OK
*       GT_FAIL
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS luaCLI_LoadConfigurationFile(
  IN  IOStreamPTR IOStream,
  OUT GT_VOID_PTR *luaInstancePtrPtr
)
{
    lua_State   *L = NULL;
    static LUA_CLI_MALLOC_STC memMgr = { NULL, NULL, 0, 0 };
    GT_STATUS rc;
    
    memMgr.malloc = (LUACLI_MALLOC_T)LUACLI_MALLOC_F;
    memMgr.free = LUACLI_FREE;

    if (luaInstancePtrPtr != NULL)
        *luaInstancePtrPtr = NULL;

    if (prvLoadConfigFile() != GT_OK)
        return GT_FAIL;

    if (IOStream == NULL)
        IOStream = &stdoutStream;

    rc = luaCLIEngine_init(&memMgr, IOStream, &cmdFS, osShellGetFunctionByName, &L);
    if (rc != GT_OK)
    {
        return rc;
    }
    IOStream->writeLine(IOStream,"Commiting lua commands from Configuration file\n");

    lua_getglobal(L, "handle_multi_line"); /* in case of multi line commands, the file need to be modified*/
    lua_pushstring(L, cmdOsConfigFileName);
    if (lua_pcall(L, 1, 1, 0) != 0)
    {
        IOStream->writeLine(IOStream,"Lua error: ");
        IOStream->writeLine(IOStream, lua_tostring(L, -1));
        lua_close(L);
        return GT_FAIL;
    }
    if (lua_isstring(L, -1))
    {
        IOStream->writeLine(IOStream, lua_tostring(L,-1));
        lua_close(L);
        return GT_FAIL;
    }
    /* now load config */
    lua_getglobal(L, "config_file_load");
    lua_pushstring(L, cmdOsConfigFileName);
    lua_newtable(L);
    if (lua_pcall(L, 2, 1, 0) != 0)
    {
        IOStream->writeLine(IOStream,"Lua error: ");
        IOStream->writeLine(IOStream, lua_tostring(L, -1));
        lua_close(L);
        return GT_FAIL;
    }
    if (lua_isstring(L, -1))
    {
        IOStream->writeLine(IOStream, lua_tostring(L,-1));
        lua_close(L);
        return GT_FAIL;
    }
    if (lua_toboolean(L,-1) && (luaInstancePtrPtr != NULL))
    {
        lua_getglobal(L, "CLIexit_state");
        if (!lua_toboolean(L, -1))
        {
            /* no CLIexit at the end, set default shell luaCLI */ 
            cmdDefaultShellType = CMD_STARTUP_SHELL_LUACLI_E;
            *luaInstancePtrPtr = (void*)L;
            return GT_OK;
        }
        else
        {
            /* CLIexit at the end, set default shell cmdShell */ 
            cmdDefaultShellType = CMD_STARTUP_SHELL_CMDSHELL_E;
        }
    }
    lua_close(L);
    return GT_OK;
}
