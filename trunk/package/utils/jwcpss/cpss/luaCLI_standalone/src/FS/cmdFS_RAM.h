/****************************************************
* cmdFS_RAM.h
*
* DESCRIPTION:
*       RAM cmdFS implementation
*
* DEPENDENCIES:
*
* COMMENTS:
*       This FS has no protection for simultaneous write:
*       there should be only one writer at time and no readers
*       when writer active
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#ifndef __cmdFS_RAM_h__
#define __cmdFS_RAM_h__

#if !defined(ENV_MAINCMD) && !defined(ENV_POSIX)
#  error "No layer defined. Define one of ENV_MAINCMD, ENV_POSIX"
#endif

#ifdef ENV_MAINCMD
#  include "cmdFS_RAM_mainCmd.inc"
#endif
#ifdef ENV_POSIX
#  include "cmdFS_RAM_posix.inc"
#endif

/***** Private Types ***************************************************/

typedef struct CMD_FS_CHUNK_STC {
    size_t                  chunksize;
    struct CMD_FS_CHUNK_STC *next;
    char                    data[1];
} CMD_FS_CHUNK_STC;

typedef struct CMD_FS_INODE_STC {
    char                    *name;
    CMD_FS_CHUNK_STC        *data;
    int                     nrefs; /* number of references */
    int                     flags;
    size_t                  size;
    unsigned                ctime;
    unsigned                mtime;
    struct CMD_FS_INODE_STC *next;
} CMD_FS_INODE_STC;
#define CMD_FS_FLAGS_EMBEDDED   1
#define CMD_FS_FLAGS_COMPRESSED 2

/***** Forward declarations ********************************************/
CMD_FS_INODE_STC* cmdFS_RAM_newFileEntry(const char* name);

#endif /* __cmdFS_RAM_h__ */
