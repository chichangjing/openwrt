/****************************************************
* FS.h
*
* DESCRIPTION:
*       file system API.
*       Currently required for Lua running on board
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __genFS_h__
#define __genFS_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Defines  ********************************************************/
#define GEN_FS_O_RDONLY 0x01
#define GEN_FS_O_WRONLY 0x02
#define GEN_FS_O_RDWR   0x03
#define GEN_FS_O_CREAT  0x04
#define GEN_FS_O_EXCL   0x08
#define GEN_FS_O_TRUNC  0x10
#define GEN_FS_O_APPEND 0x20
/* special flag to compress data on new created file */
#define GEN_FS_O_COMPRESS 0x40

#define GEN_FS_SEEK_SET 0
#define GEN_FS_SEEK_CUR 1
#define GEN_FS_SEEK_END 2

/* bitmask for the file type bitfields */
#define GEN_FS_S_IFMT     0170000
/* regular file */
#define GEN_FS_S_IFREG    0100000
/* directory */
#define GEN_FS_S_IFDIR    0040000
/* is it a regular file? */
#define GEN_FS_S_ISREG(m) ((m) & GEN_FS_S_IFREG)
/* directory? */
#define GEN_FS_S_ISDIR(m) ((m) & GEN_FS_S_IFDIR)


#define GEN_FS_NAME_MAX 256

/***** Public Types ****************************************************/
typedef struct GEN_FS_STAT_STC {
    int st_size; /* total size, in bytes */
    int st_mode; /* file mode, see GEN_FS_S_* */
    unsigned st_ctime; /* creation time (time_t) */
    unsigned st_mtime; /* modification time (time_t) */
} GEN_FS_STAT_STC;

typedef struct GEN_FS_DIRENT_STC {
    char d_name [GEN_FS_NAME_MAX+1];
} GEN_FS_DIRENT_STC;

/***** Public Types ****************************************************/
typedef struct CMD_FS_STAT_STC {
    int st_size; /* total size, in bytes */
    int st_mode; /* file mode, see GEN_FS_S_* */
    unsigned st_ctim; /* creation time (time_t) */
    unsigned st_mtim; /* modification time (time_t) */
} CMD_FS_STAT_STC;

typedef struct CMD_FS_DIRENT_STC {
    char d_name [GEN_FS_NAME_MAX+1];
} CMD_FS_DIRENT_STC;

/***** Public Functions ************************************************/

struct genFS_STC {
    /*******************************************************************************
    * lastError
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
    const char* (*lastError)(void);




    /*******************************************************************************
    * open
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
    int (*open)(const char* name, int flags);

    /*******************************************************************************
    * close
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
    int (*close)(int fd);

    /*******************************************************************************
    * read
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
    int (*read)(int fd, void *buf, int count);

    /*******************************************************************************
    * write
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
    int (*write)(int fd, const void *buf, int count);

    /*******************************************************************************
    * genFSlseek
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
    int (*lseek)(int fd, int offset, int whence);

    /*******************************************************************************
    * stat
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
    int (*stat)(const char* name, GEN_FS_STAT_STC *buf);

    /*******************************************************************************
    * unlink
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
    int (*unlink)(const char* name);


    /*******************************************************************************
    * opendir
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
    int (*opendir)(const char *name);

    /*******************************************************************************
    * closedir
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
    int (*closedir)(int fd);


    /*******************************************************************************
    * readdir
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
    int (*readdir)(int fd, GEN_FS_DIRENT_STC *dirPtr);
};
typedef struct genFS_STC genFS_STC;
typedef struct genFS_STC *genFS_PTR;

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __genFS_h__ */

#if undef
/* Sample */
int ls(genFS_PTR fs)
{
    GEN_FS_DIRENT_STC   dirent;
    GEN_FS_STAT_STC     stat;
    int                 dirFd;
    int                 rc;

    dirFd = fs->opendir(NULL);
    if (dirFd < 0)
    {
        printf("err\n");
        return -1;
    }
    rc = 1;
    while (rc == 1)
    {
         rc = fs->readdir(dirFd, &dirent);
         if (rc == 1)
         {
             fs->stat(dirent.d_name, &stat);
             /* print file name and size */
             printf("%-30s %d\n", dirent.d_name, stat.st_size);
         }
    }
    fs->closedir(dirFd);
    return 0;
}
#endif
