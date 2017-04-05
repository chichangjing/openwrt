/****************************************************
* cmdFS_VIRT.c
*
* DESCRIPTION:
*       Virtual cmdFS implementation - based on single file on real FS
*
* DEPENDENCIES:
*
* COMMENTS:
*       This FS is read only
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CMD_FS_STRCMP(a,b)    strcmp(a,b)

char* CMD_FS_STRDUP(const char *s)
{
   char *d = malloc(strlen(s)+1);
   if (d != NULL)
       strcpy(d,s);
   return d;
} 

#define CMD_FS_STRNCPY(d,s,n) strncpy(d,s,n)
#define CMD_FS_BZERO(p,l)     memset(p,0,l)
#define CMD_FS_BCOPY(s,d,l)   memcpy(d,s,l)
#define CMD_FS_MALLOC(l)      malloc(l)
#define CMD_FS_FREE(l)        free(l)

#include <cmdFS.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <cmdFS.h>
#include <sys/types.h>
#include <sys/stat.h> 

/***** Defines  ********************************************************/
#define CMD_FS_MAX_OPEN_FILES   50
#define CMD_FS_O_DIRECTORY      0x40000000

static const char *cmdFS_root = NULL;
/* Big endian mode */ 

#define OAM_BYTESWAP_MAC(data)              \
        (((data) << 24))  |                 \
        (((data) & 0xff00)      << 8)   |   \
        (((data) & 0xff0000)    >> 8)   |   \
        (((data) >> 24))

#if (CPU==SIMSPARCSOLARIS)   
    /* SOLARIS simulation */
    #define CMD_FS_DEFAULT_ROOT     "./certified"
#else
    #define CMD_FS_DEFAULT_ROOT     "/certified"
#endif

#define CMD_FS_ROOT_ENV_NAME      "CMDFS_ROOT"
#define CMD_FS_ROOT_EMBEDDED_DATA "cmdFS_embedded_data.bin"

int   embedded_fd          = -1 ;
int   embedded_data_len    = 0;


/***** Private Types ***************************************************/

typedef struct CMD_FS_EMBEDDED_DATA_FILE_INFO_STC {
    unsigned data_offset;
    unsigned data_len   ;
    unsigned mtime      ;
    char     fname[244] ;
} CMD_FS_EMBEDDED_DATA_FILE_INFO_STC;

typedef struct CMD_FS_INODE_STC {
    char             *name       ;
    unsigned          mtime      ;
    unsigned          data_offset;
    unsigned          data_len   ;
    struct CMD_FS_INODE_STC *next;
} CMD_FS_INODE_STC;

typedef struct CMD_FS_FDESCR_STC {
    int                 flags;      /* open flags, 0 means not opened */
    CMD_FS_INODE_STC    *inode;
    int                 pos;        /* current position */
} CMD_FS_FDESCR_STC;


/***** Forward declarations ********************************************/

/***** Private Data ****************************************************/
static CMD_FS_INODE_STC *cmdFSdir = NULL;
static CMD_FS_FDESCR_STC    fdescr[CMD_FS_MAX_OPEN_FILES];
static const char *last_error = "";

/***** Private Functions ***********************************************/

/*******************************************************************************
* CMD_FS_CHECK_FD
*
* DESCRIPTION:
*       Check that fd is valid opened file descriptor
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       -1  - on error
*
* COMMENTS:
*
*******************************************************************************/
#define FD                  fdescr[fd]
#define CMD_FS_CHECK_FD() \
    if (fd < 0 || fd >= CMD_FS_MAX_OPEN_FILES) \
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
* cmdFS_ALU_lookupFile
*
* DESCRIPTION:
*       Scan directory linked list for given file
*
* INPUTS:
*       name    - filename
*
* OUTPUTS:
*       None
*
* RETURNS:
*       inode pointer
*       - NULL if error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_FS_INODE_STC* cmdFS_ALU_lookupFile(const char *name)
{
    CMD_FS_INODE_STC  *inode;

    for (inode = cmdFSdir; inode; inode = inode->next)
    {
        if (!CMD_FS_STRCMP(inode->name, name))
        {
            return inode;
        }
    }
    return NULL;
}


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

/*******************************************************************************/

void MemDumpCmd( unsigned char * pData, unsigned char nBytes) {

	unsigned int    dwAdr;
	unsigned int    Adr;
	unsigned char szT[256], szC[256], szB[256], *p;
	int i=0;
    p     = pData;
    dwAdr = (unsigned int )p;
    printf("\n");

	while (nBytes) 
    {
		memset(szB, 0x00, 256);
		memset(szT, 0x00, 256);
		memset(szC, 0x00, 256);
	
		for(i=0;i< 16; ++i, ++p) {
			if(nBytes) {
				sprintf(szT, "%02x ", *p);
				strcat(szB, szT);
				szC[i]=(*p >= 0x20 && *p <= 0x7F) ? *p : '.';
			} else {
				strcat(szB, "   ");
				szC[i]= ' ';
		 	}
		 	szC[i+1] = 0;
		 	if (nBytes) --nBytes;
		} 
        printf("\n\r%08lx %s %s",dwAdr, szB, szC );
        dwAdr += 16;
        Adr += 16;
	} 
    printf("\n");
    
    return;

} 

/*******************************************************************************/

static int cmdFS_RAM_printFiles(void)
{
    CMD_FS_INODE_STC  *inode;
    int i = 0 ;

    for (inode = cmdFSdir; inode; inode = inode->next)
    {
        printf("---------------------------------------------------------\n");
        printf("Entry:%d \n", i);
        printf("inode       = %d=0x%x\n", inode             , inode             );
        printf("name        = %s\n"     , inode->name       , inode->name       );
        printf("next        = %d=0x%x\n", inode->next       , inode->next       );
        printf("mtime       = %d=0x%x\n", inode->mtime      , inode->mtime      );
        printf("data_offset = %d=0x%x\n", inode->data_offset, inode->data_offset);
        printf("data_len    = %d=0x%x\n", inode->data_len   , inode->data_len   );
        i++;
    }
    return 0;
}

/*******************************************************************************/
static CMD_FS_INODE_STC* cmdFS_RAM_newFileEntryFlash(
    char             * name     ,
    unsigned         mtime      ,
    unsigned         data_offset,
    unsigned         data_len   )
{
    CMD_FS_INODE_STC *inode;
    inode = cmdFSdir;
    if (inode)
    {
        while(inode->next)
            inode = inode->next;
        inode->next = (CMD_FS_INODE_STC*) CMD_FS_MALLOC(sizeof(*inode));
        inode = inode->next;
    } else {
        inode = cmdFSdir = (CMD_FS_INODE_STC*) CMD_FS_MALLOC(sizeof(*inode));
    }
    CMD_FS_BZERO(inode, sizeof(*inode));
    inode->name        = CMD_FS_STRDUP(name);
    inode->mtime       = mtime;

    inode->data_offset = data_offset;
    inode->data_len    = data_len   ;

    return inode;
}

/*******************************************************************************/


static int prvCmdLoadEmbeddedFilesTable(void)
{
    int                                 i = 0 ;
    CMD_FS_INODE_STC*                   inode = NULL ;
    CMD_FS_EMBEDDED_DATA_FILE_INFO_STC  embedded_file_head;
    char                                fname[244];
    unsigned                            data_offset, data_len, mtime;
    unsigned                            embedded_data_files_numbers = 0;
    int rc = 0;

    for (i=0;;i++)
    {

        rc = cmdFSreadFlash(embedded_fd,&embedded_file_head,sizeof(embedded_file_head)) ;
        if (rc < 0)
        {
            return -1;
        }

        if (embedded_file_head.data_offset == 0)
        {
            break;
        }

        data_offset =   embedded_file_head.data_offset ;
        data_len    =   embedded_file_head.data_len    ;
        mtime       =   embedded_file_head.mtime       ;

        memcpy(fname,embedded_file_head.fname,244);

        inode = cmdFS_RAM_newFileEntryFlash(fname ,
                                            mtime,
                                            data_offset,
                                            data_len );

        embedded_data_files_numbers++;
    }

    if (rc < 0)
    {
        return -1;
    }


    return 0;
}

/*******************************************************************************/

static int prvCmdFSopenEmbeddedData(const char* name)
{
    char *nm;

    struct stat fileStat;
    long result;

    if (embedded_fd >= 0)
        return embedded_fd;

    /* map open flags to system */
    nm = cmdFSmkname(name);

    embedded_fd = open(nm, O_RDONLY, 0666);
    if (embedded_fd < 0)
    {
        last_error = strerror(errno);
        embedded_data_len = 0;
    }
    else
    {
        if(fstat(embedded_fd,&fileStat) < 0)
        {        
            free(nm);
            return -1;    
        }
        embedded_data_len   = fileStat.st_size;
    }

    free(nm);


    return embedded_fd;
}

/*******************************************************************************/
/***** Public Functions ************************************************/

/*******************************************************************************/

/*******************************************************************************
* cmdFSterminate
*
* DESCRIPTION:
*       Close a file descriptor, free memory
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 if success, < 0 if error
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSterminate(void)
{
    int rc = 0;


    CMD_FS_INODE_STC  *inode, *ptr;
    /* free inodes */
    inode = cmdFSdir;

    while (inode)
    {
        ptr = inode;
        inode = inode->next;
        if (ptr->name)
            CMD_FS_FREE(ptr->name);
        CMD_FS_FREE(ptr);
    }
    cmdFSdir = NULL ;

    if (embedded_fd < 0)
    {
        return 0;
    }
    rc = close(embedded_fd);
    if (rc < 0)
    {
        last_error = strerror(errno);
    }

    embedded_fd = -1; 

    return rc;
}

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
    int rc = 0;
    if (embedded_fd >= 0)
    {
        /* already initialized */
        return 0;
    }

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
    }

    CMD_FS_BZERO(fdescr, sizeof(fdescr));

    rc = prvCmdFSopenEmbeddedData(CMD_FS_ROOT_EMBEDDED_DATA);
    if (rc < 0)
    {
        return -1;
    }

    rc = prvCmdLoadEmbeddedFilesTable();
    if (rc < 0)
    {
        return -1;
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
*       flags   - open flags, see CMD_FS_O_*,
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
    int rc = 0;
    CMD_FS_INODE_STC  *inode;
    char     buf[256] ;   

    if (embedded_fd < 0)
    {
        last_error = "open(): not initialized";
        return -1;
    }
    if (flags == 0 || (flags & CMD_FS_O_DIRECTORY))
    {
        last_error = "open(): wrong flags";
        return -1;
    }
    if (flags & CMD_FS_O_WRONLY)
    {
        last_error = "open(): EPERM: read only FS";
        return -1;
    }

    /* look for new fd */
    for (fd = 0; fd < CMD_FS_MAX_OPEN_FILES; fd++)
    {
        if (FD.flags == 0)
        {
            break;
        }
    }
    if (fd >= CMD_FS_MAX_OPEN_FILES)
    {
        last_error = "No enough descriptors";
        return -1;
    }
    /* unused file descriptor found. lock it first */
    FD.flags = -1; /* lock */

    inode = cmdFS_ALU_lookupFile(name);

    if (inode == NULL)
    {
        FD.flags = 0; /* unlock */
        last_error = "Failed to open file (no such file)";
        return -1;
    }
    FD.flags = flags;
    FD.inode = inode;
    FD.pos = 0;
    
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
    CMD_FS_CHECK_FD();
    FD.flags = 0;
    return 0;
}

/*******************************************************************************
* cmdFSreadFlash
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
int cmdFSreadFlash(int fd, void *buf, int count)
{
    int ret;
    ret = read(fd, buf, count);
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
    int      n;
    int      rc;
    CMD_FS_CHECK_FD();


    if (count < 0)
    {
        last_error = "Bad parameter: count";
        return -1;
    }
    if (buf == NULL)
    {
        last_error = "Bad parameter: buf";
        return -1;
    }
    memset(buf,0,count);

    if ((FD.flags & CMD_FS_O_RDONLY) == 0)
    {
        last_error = "File is not opened for reading";
        return -1;
    }

    if (FD.pos == FD.inode->data_len)
    {
        return 0;
    }

    n = (int)(FD.inode->data_len - FD.pos);
    if (n > count)
    {
        n = count;
    }

    rc = lseek(embedded_fd, (off_t)FD.inode->data_offset + FD.pos, SEEK_SET);
    if (rc < 0)
    {
        last_error = strerror(errno);
        return -1;
    }

    rc = cmdFSreadFlash(embedded_fd,buf,n) ;
    if (rc >= 0)
        FD.pos += rc;
    return rc;
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
    const char  *iptr = (const char*)buf;
    int         nwritten = 0;

    CMD_FS_CHECK_FD();

    if (count < 0)
    {
        last_error = "Bad parameter: count";
        return -1;
    }
    if (buf == NULL)
    {
        last_error = "Bad parameter: buf";
        return -1;
    }
    if ((FD.flags & CMD_FS_O_WRONLY) == 0)
    {
        last_error = "File is not opened for writing";
        return -1;
    }

    last_error = "read only FS";
    return -1;
}

/*******************************************************************************
* cmdFSmmap
*
* DESCRIPTION:
*       Map file content to RAM
*
* INPUTS:
*       fd      - file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to file data
*
* COMMENTS:
*       
*
*******************************************************************************/
void* cmdFSmmap(int fd)
{
    last_error = "Not supported";
    return NULL;
}

/*******************************************************************************
* cmdFSmunmap
*
* DESCRIPTION:
*       Unmap 
*
* INPUTS:
*       fd      - file descriptor
*       ptr     - pointer to mapped memory
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       read POSIX write() as reference
*
*******************************************************************************/
void cmdFSmunmap(int fd, void *ptr)
{
	return;
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
*                 CMD_FS_SEEK_SET   - The offset is set to offset bytes
*                 CMD_FS_SEEK_CUR   - The offset is set to current location
*                                     plus offset bytes
*                 CMD_FS_SEEK_END   - The offset is set to size of the file
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
    unsigned npos = -1;
    CMD_FS_CHECK_FD();
    switch (whence)
    {
        case CMD_FS_SEEK_SET:
            npos = offset;
            break;
        case CMD_FS_SEEK_CUR:
            npos = offset + (int)FD.pos;
            break;
        case CMD_FS_SEEK_END:
            npos = offset + (int)FD.inode->data_len;
            break;
        default:
            last_error = "bad param: whence";
            return -1;
    }

    if (npos < 0 || npos > FD.inode->data_len)
    {
        last_error = "Parameter out of range";
        return -1;
    }
    FD.pos = npos;
    return FD.pos;
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
int cmdFSstat(const char* name, CMD_FS_STAT_STC *buf)
{
    CMD_FS_INODE_STC  *inode;

    inode = cmdFS_ALU_lookupFile(name);
    if (!inode)
    {
        last_error = "File not found";
        return -1;
    }
    CMD_FS_BZERO(buf, sizeof(*buf));
    buf->st_mode = CMD_FS_S_IFREG; /* regular file */
    buf->st_ctime = inode->mtime;
    buf->st_mtime = inode->mtime;
    buf->st_size = inode->data_len;

    return 0;
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
    last_error = "Not supported: read only FS";
    return -1;
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

    /* look for new fd */
    for (fd = 0; fd < CMD_FS_MAX_OPEN_FILES; fd++)
    {
        if (FD.flags == 0)
        {
            break;
        }
    }
    if (fd >= CMD_FS_MAX_OPEN_FILES)
    {
        last_error = "No enough descriptors";
        return -1;
    }
    /* unused file descriptor found. */
    FD.flags = CMD_FS_O_DIRECTORY;
    FD.inode = cmdFSdir;
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
    return cmdFSclose(fd);
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
int cmdFSreaddir(int fd, CMD_FS_DIRENT_STC *dirPtr)
{
    CMD_FS_CHECK_FD();

    if ((FD.flags & CMD_FS_O_DIRECTORY) == 0)
    {
        last_error = "descriptor is not a directory";
        return -1;
    }
    if (dirPtr == NULL)
    {
        last_error = "Bad parameter: dirPtr";
        return -1;
    }
    if (FD.inode == NULL)
    {
        return 0;
    }
    CMD_FS_BZERO(dirPtr, sizeof(*dirPtr));
    CMD_FS_STRNCPY(dirPtr->d_name, FD.inode->name, CMD_FS_NAME_MAX);
    FD.inode = FD.inode->next;
    return 1;
}
