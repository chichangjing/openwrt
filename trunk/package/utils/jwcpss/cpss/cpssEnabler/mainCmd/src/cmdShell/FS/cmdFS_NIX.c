/****************************************************
* cmdFS_NIXRAM.c
*
* DESCRIPTION:
*       cmdFS implementation for UNIX-like OSes
*       based on mounted filesystemd
*
* DEPENDENCIES:
*       Linux, FreeBSD, etc
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

/***** Include files ***************************************************/
#include <cmdShell/FS/cmdFS.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#define USE_FTS
#ifdef USE_FTS
# ifndef __u_char_defined
  typedef __u_short u_short;
# endif
#include <fts.h>
#endif /* USE_FTS */

/***** Defines  ********************************************************/
#define CMD_FS_ROOT_ENV_NAME    "CMDFS_ROOT"
#define CMD_FS_DEFAULT_ROOT     "/cmdFS"
#define CMD_FS_MAX_OPEN_DIRS   10

/***** Private Types ***************************************************/
typedef struct CMD_FS_FDESCR_DIR_STC {
    int                 flags;      /* open flags, 0 means not opened */
    DIR*                dir;
#ifdef USE_FTS
    FTS*                fts;
    int                 prefixlen;
#endif /* defined(USE_FTS) */
} CMD_FS_FDESCR_DIR_STC;

/***** Private Data ****************************************************/
static char *cmdFS_root = NULL;
static const char *last_error = "";
static CMD_FS_FDESCR_DIR_STC    fdescr[CMD_FS_MAX_OPEN_DIRS];
#define FD                  fdescr[fd]
#define CMD_FS_CHECK_FD() \
    if (fd < 0 || fd >= CMD_FS_MAX_OPEN_DIRS) \
    { \
        last_error = "Wrong file descriptor"; \
        return -1; \
    } \
    if (FD.flags == 0) \
    { \
        last_error = "Bad file descriptor"; \
        return -1; \
    }

/***** Private Functions ***********************************************/

/*******************************************************************************
* cmdFSmkname
*
* DESCRIPTION:
*       map name to cmdFS_root
*
* INPUTS:
*       name    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       mapped name
*
* COMMENTS:
*
*******************************************************************************/
char *cmdFSmkname(const char* name)
{
    char *nm;
    if (name[0] == '/')
        name++;
    nm = (char*)malloc(strlen(cmdFS_root) + strlen(name) + 2);
    strcpy(nm, cmdFS_root);
    strcat(nm, "/");
    strcat(nm, name);
    return nm;
}

/*******************************************************************************
* cmdFSmkdir_p
*
* DESCRIPTION:
*       create directory for new file (recursive)
*
* INPUTS:
*       name    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 if sucessful
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSmkdir_p(char* name)
{
    int k;
    int last_slash = -1;
    struct stat st;
    for (k = 0; name[k]; k++)
    {
        if (name[k] == '/')
            last_slash = k;
    }
    if (last_slash <= 0)
        return 1;
    name[last_slash] = 0;
    k = stat(name, &st);
    name[last_slash] = '/';
    if (k == 0)
    {
        if (!S_ISDIR(st.st_mode))
        {
            last_error = "the part of path is not a directory";
            return 0;
        }
        return 1;
    }
    if (errno != ENOENT)
    {
        last_error = "Failed to create directory for file";
        return 0;
    }

    name[last_slash] = 0;
    if (!cmdFSmkdir_p(name))
    {
        name[last_slash] = '/';
        return 0;
    }
    k = mkdir(name, 0777);
    name[last_slash] = '/';
    if (k < 0)
    {
        last_error = "Failed to create directory for file";
        return 0;
    }
    return 1;
}

/***** Public Functions ************************************************/

/*******************************************************************************
* cmdFSinit
*
* DESCRIPTION:
*       Initialize cmdFS, initialize built-in files
*
* INPUTS:
*       None
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
int cmdFSinit(void)
{
    if (cmdFS_root == NULL)
    {
        cmdFS_root = getenv(CMD_FS_ROOT_ENV_NAME);
        if (cmdFS_root != NULL)
        {
            if (cmdFS_root[0] != '/')
            {
                cmdFS_root = NULL;
            }
            else
            {
                int l = strlen(cmdFS_root);
                if (l && cmdFS_root[l-1] == '/')
                {
                    char *nm = (char*)malloc(l+1);
                    strcpy(nm, cmdFS_root);
                    nm[l-1] = 0;
                    cmdFS_root = nm;
                }
            }
        }
        if (!cmdFS_root)
        {
            cmdFS_root = CMD_FS_DEFAULT_ROOT;
        }

        memset(fdescr, 0, sizeof(fdescr));
    }
    return 0;
}

/*******************************************************************************
* cmdFSlastError
*
* DESCRIPTION:
*       Return string with last error description
*
* INPUTS:
*       None
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
const char* cmdFSlastError(void)
{
    return last_error;
}

/*******************************************************************************
* cmdFSopen
*
* DESCRIPTION:
*       Open and possible create a new file
*
* INPUTS:
*       name    - file name
*       flags   - open flags, see GEN_FS_O_*,
*                 like POSIX open()
*
* OUTPUTS:
*       None
*
* RETURNS:
*       file descriptor or < 0 if error
*
* COMMENTS:
*       read POSIX open() as reference
*
*******************************************************************************/
int cmdFSopen(const char* name, int flags)
{
    int fd;
    char *nm;
    int sys_flags;

    /* map open flags to system */
    sys_flags = 0;

    if ((flags & GEN_FS_O_RDWR) == 0)
    {
        last_error = "open(): wrong flags";
        return -1;
    }
    if (flags & GEN_FS_O_RDONLY)
        sys_flags |= O_RDONLY;
    if (flags & GEN_FS_O_WRONLY)
        sys_flags |= O_WRONLY;
    if (flags & GEN_FS_O_CREAT)
    {
        if ((flags & GEN_FS_O_WRONLY) == 0)
        {
            last_error = "Wrong open flags (O_CREAT w/o write)";
            return -1;
        }
        sys_flags |= O_CREAT;
    }
    if (flags & GEN_FS_O_EXCL)
        sys_flags |= O_EXCL;
    if (flags & GEN_FS_O_TRUNC)
        sys_flags |= O_TRUNC;
    if (flags & GEN_FS_O_APPEND)
        sys_flags |= O_APPEND;

    nm = cmdFSmkname(name);
    if (flags & GEN_FS_O_CREAT)
    {
        if (!cmdFSmkdir_p(nm))
        {
            free(nm);
            return -1;
        }
    }
    fd = open(nm, sys_flags, 0666);
    if (fd < 0)
    {
        last_error = strerror(errno);
    }
    free(nm);

    return fd;
}

/*******************************************************************************
* cmdFSclose
*
* DESCRIPTION:
*       Close a file descriptor
*
* INPUTS:
*       fd      - file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 if success, < 0 if error
*
* COMMENTS:
*       read POSIX close() as reference
*
*******************************************************************************/
int cmdFSclose(int fd)
{
    int ret;
    ret = close(fd);
    if (ret < 0)
        last_error = strerror(errno);
    return ret;
}

/*******************************************************************************
* cmdFSread
*
* DESCRIPTION:
*       read from a file descriptor
*
* INPUTS:
*       fd      - file descriptor
*       count   - number of bytes to read
*
* OUTPUTS:
*       buf     - read data to buffer started at this pointer
*
* RETURNS:
*       The number of bytes read or <0 of error
*
* COMMENTS:
*       read POSIX read() as reference
*
*******************************************************************************/
int cmdFSread(int fd, void *buf, int count)
{
    int ret;
    ret = read(fd, buf, count);
    if (ret < 0)
        last_error = strerror(errno);
    return ret;
}

/*******************************************************************************
* cmdFSwrite
*
* DESCRIPTION:
*       Write to a file descriptor
*
* INPUTS:
*       fd      - file descriptor
*       buf     - write from buffer started at this pointer
*       count   - number of bytes to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       The number of bytes written or <0 of error
*
* COMMENTS:
*       read POSIX write() as reference
*
*******************************************************************************/
int cmdFSwrite(int fd, const void *buf, int count)
{
    int ret;
    ret = write(fd, buf, count);
    if (ret < 0)
        last_error = strerror(errno);
    return ret;
}


/*******************************************************************************
* cmdFSlseek
*
* DESCRIPTION:
*       reposition read/write file offset
*
* INPUTS:
*       fd      - file descriptor
*       offset  - 
*       whence  - one of
*                 GEN_FS_SEEK_SET   - The offset is set to offset bytes
*                 GEN_FS_SEEK_CUR   - The offset is set to current location
*                                     plus offset bytes
*                 GEN_FS_SEEK_END   - The offset is set to size of the file
*                                     PLUS offset bytes
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
*       read POSIX lseek() as reference
*
*******************************************************************************/
int cmdFSlseek(int fd, int offset, int whence)
{
    int w;
    int ret;
    switch (whence)
    {
        case GEN_FS_SEEK_SET:
            w = SEEK_SET;
            break;
        case GEN_FS_SEEK_CUR:
            w = SEEK_CUR;
            break;
        case GEN_FS_SEEK_END:
            w = SEEK_END;
            break;
        default:
            last_error = "Bad parameter";
            return -1;
    }
    ret = (int)lseek(fd, (off_t)offset, w);
    if (ret < 0)
        last_error = strerror(errno);
    return ret;
}


/*******************************************************************************
* cmdFSstat
*
* DESCRIPTION:
*       get file status
*
* INPUTS:
*       name    - file name
*
* OUTPUTS:
*       buf     - pointer to stat structure
*
* RETURNS:
*       0 if success
*       -1 if error
*
* COMMENTS:
*       read POSIX stat() as reference
*
*******************************************************************************/
int cmdFSstat(const char* name, GEN_FS_STAT_STC *buf)
{
    char *nm;
    struct stat st;
    int ret;

    nm = cmdFSmkname(name);
    ret = stat(nm, &st);
    if (ret < 0)
    {
        last_error = strerror(errno);
    }
    free(nm);

    if (ret < 0)
        return ret;

    if (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
    {
        last_error = "Not a file/directory";
        return -1;
    }
    if (buf)
    {
        buf->st_mode = S_ISREG(st.st_mode)
            ? GEN_FS_S_IFREG
            : GEN_FS_S_IFDIR;
        buf->st_mode |= st.st_mode & 0777;
        buf->st_size = (int)st.st_size;
        buf->st_ctime = (unsigned)st.st_ctime;
        buf->st_mtime = (unsigned)st.st_mtime;
    }
    return ret;
}


/*******************************************************************************
* cmdFSunlink
*
* DESCRIPTION:
*       delete a name and possibly the file it refers to
*
* INPUTS:
*       name    - file name
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - on success
*
* COMMENTS:
*       read POSIX unlink() as reference
*
*******************************************************************************/
int cmdFSunlink(const char* name)
{
    char *nm;
    int ret;

    nm = cmdFSmkname(name);
    ret = unlink(nm);
    if (ret < 0)
    {
        last_error = strerror(errno);
    }
    free(nm);
    return ret;
}

/*******************************************************************************
* cmdFSopendir
*
* DESCRIPTION:
*       open a directory
*
* INPUTS:
*       name    - directory name
*                 (will beignored in current implementation)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       directory file descriptor or <0 if error
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSopendir(const char *name)
{
    int fd;
    char *nm;

    /* look for new fd */
    for (fd = 0; fd < CMD_FS_MAX_OPEN_DIRS; fd++)
    {
        if (FD.flags == 0)
        {
            break;
        }
    }
    if (fd >= CMD_FS_MAX_OPEN_DIRS)
    {
        last_error = "No enough descriptors";
        return -1;
    }
    /* unused file descriptor found. */
    FD.flags = 1;
    FD.dir = NULL;
#ifdef USE_FTS
    FD.fts = NULL;

    if (name != NULL)
    {
        if (strcmp(name, " ") == 0)
            name = NULL;
        else if (strcmp(name, "") == 0)
            name = NULL;
    }
    if (name == NULL) /* || name == "/" */
    {
        char * const paths[] = { cmdFS_root, NULL };
        FD.fts = fts_open(paths, FTS_NOCHDIR|FTS_NOSTAT,NULL);
        if (FD.fts == NULL)
        {
            last_error = strerror(errno);
            FD.flags = 0;
            return -1;
        }
        FD.prefixlen = strlen(cmdFS_root)+1;
        return fd;
    }
    nm = cmdFSmkname(name);
#else /* !defined(USE_FTS) */
    nm = cmdFSmkname(name?name:"/");
#endif /* USE_FTS */
    FD.dir = opendir(nm);
    if (FD.dir == NULL)
    {
        last_error = strerror(errno);
    }
    free(nm);

    if (FD.dir == NULL)
    {
        FD.flags = 0;
        return -1;
    }

    return fd;
}

/*******************************************************************************
* cmdFSclosedir
*
* DESCRIPTION:
*       close a directory
*
* INPUTS:
*       fd      - directory file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 if success or <0 if error
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSclosedir(int fd)
{
    int ret;
    CMD_FS_CHECK_FD();
#ifdef USE_FTS
    if (FD.fts != NULL)
    {
        ret = fts_close(FD.fts);
    }
    else
#endif
    ret = closedir(FD.dir);
    if (ret < 0)
        last_error = strerror(errno);
    return ret;
}


/*******************************************************************************
* cmdFSreaddir
*
* DESCRIPTION:
*       read a directory entry
*
* INPUTS:
*       fd      - directory file descriptor
*
* OUTPUTS:
*       dirPtr  - pointer to directory entry structure
*
* RETURNS:
*       1 On success
*       0 On end of directory
*       <0 On error
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSreaddir(int fd, GEN_FS_DIRENT_STC *dirPtr)
{
    struct dirent *d;

    CMD_FS_CHECK_FD();

#ifdef USE_FTS
    if (FD.fts != NULL)
    {
        FTSENT  *ent;
        char    *p;
        int     plen;
        while ((ent = fts_read(FD.fts)) != NULL)
        {
            if (ent->fts_info == FTS_F)
                break;
        }
        if (ent == NULL)
            return 0;
        p = ent->fts_path + FD.prefixlen;
        plen = ent->fts_pathlen - FD.prefixlen;
        if (plen > sizeof(dirPtr->d_name))
            plen = sizeof(dirPtr->d_name);
        strncpy(dirPtr->d_name, p, plen);
        if (plen < sizeof(dirPtr->d_name))
            dirPtr->d_name[plen] = 0;
        return 1;
    }
#endif
    d = readdir(FD.dir);
    if (d == NULL)
        return 0;
    if (dirPtr)
    {
        strncpy(dirPtr->d_name, d->d_name, sizeof(dirPtr->d_name));
    }
    return 1;
}

/* genFS definition */
genFS_STC cmdFS = {
    cmdFSlastError,
    cmdFSopen,
    cmdFSclose,
    cmdFSread,
    cmdFSwrite,
    cmdFSlseek,
    cmdFSstat,
    cmdFSunlink,
    cmdFSopendir,
    cmdFSclosedir,
    cmdFSreaddir
};

#ifdef TEST
int main()
{
    int fd;
    char buf[100];
    int k;

    fd = cmdFSopen("test", GEN_FS_O_WRONLY); /* failure */
    fd = cmdFSopen("test", GEN_FS_O_WRONLY|GEN_FS_O_CREAT);
    k = cmdFSwrite(fd, "12345", 5);
    k = cmdFSwrite(fd, "67890abc", 8);
    cmdFSclose(fd);
    fd = cmdFSopen("test", GEN_FS_O_RDWR);
    k = cmdFSread(fd, buf, 3);
    k = cmdFSwrite(fd, "xyzkl", 5);
    k = cmdFSread(fd, buf, 10); /* 3 bytes read */
    k = cmdFSread(fd, buf, 10); /* 0 bytes: eof */
    cmdFSclose(fd);
    return 0;
}
#endif
