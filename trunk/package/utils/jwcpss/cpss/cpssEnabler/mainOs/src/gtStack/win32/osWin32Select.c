/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osLinuxSelect.c
*
* DESCRIPTION:
*       win32 User Mode Operating System wrapper.
*       synchronous I/O multiplexing facility
*
* DEPENDENCIES:
*       windows, CPU independed
*       select implementation of the operating system
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <winsock2.h>

#include <gtOs/gtOsMem.h>
#include <gtStack/gtStackTypes.h>
#include <gtStack/gtOsSelect.h>
#include <gtUtil/gtBmPool.h>


#ifndef __CALL
#define __CALL __stdcall
#endif

#ifndef __FAR
#define __FAR
#endif


/***** Public Functions ************************************************/
#if 0
extern int __CALL __FAR __WSAFDIsSet(unsigned int , fd_set __FAR *);
#endif


/*******************************************************************************
* osSelectCreateSet()
*
* DESCRIPTION:
*       Create a set of file descriptors for the select function
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       Pointer to the set. If unable to create, returns null. Note that the
*       pointer is from void type.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID * osSelectCreateSet()
{
    fd_set * set ;

    set = (fd_set*) osMalloc(sizeof(fd_set));
    if (set != NULL)
    {
        FD_ZERO(set) ;
    }

    return (GT_VOID*) set ;
}


/*******************************************************************************
* osSelectEraseSet()
*
* DESCRIPTION:
*       Erase (delete) a set of file descriptors
*
* INPUTS:
*       set - Pointer to the set.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osSelectEraseSet(GT_VOID *  set)
{
     osFree(set);
}

/*******************************************************************************
* osSelectZeroSet()
*
* DESCRIPTION:
*       Zeros a set of file descriptors
*
* INPUTS:
*       set - Pointer to the set.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osSelectZeroSet(GT_VOID *  set)
{
    FD_ZERO((struct fd_set *)set) ;
}


/*******************************************************************************
* osSelectAddFdToSet()
*
* DESCRIPTION:
*       Add a file descriptor to a specific set
*
* INPUTS:
*       set - Pointer to the set
*       fd  - A file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osSelectAddFdToSet(GT_VOID *  set ,GT_FD  fd)
{
    FD_SET((SOCKET)fd, (struct fd_set * )set) ;
}


/*******************************************************************************
* osSelectClearFdFromSet()
*
* DESCRIPTION:
*       Remove (clear) a file descriptor from a specific set
*
* INPUTS:
*       set - Pointer to the set
*       fd  - A file descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osSelectClearFdFromSet(GT_VOID *  set ,GT_FD  fd)
{
    FD_CLR((SOCKET)fd, (struct fd_set*)set) ;
}


/*******************************************************************************
* osSelectIsFdSet()
*
* DESCRIPTION:
*       Test if a specific file descriptor is set in a set
*
* INPUTS:
*       set - Pointer to the set
*       fd  - A file descriptor
*
* OUTPUTS:
*    None
*
* RETURNS:
*       GT_TRUE  (non zero) if set , returned as unsigned int
*       GT_FALSE (zero) if not set , returned as unsigned int
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_BOOL osSelectIsFdSet(GT_VOID *  set ,GT_FD  fd)
{
    if ( (FD_ISSET((SOCKET)fd, (struct fd_set*)set)) != 0)
    {
        return GT_TRUE;
    }
    else
    {
        return GT_FALSE;
    }
}


/*******************************************************************************
* osSelectCopySet()
*
* DESCRIPTION:
*       Duplicate sets (require 2 pointers for sets)
*
* INPUTS:
*       srcSet - Pointer to source set
*       dstSet - Pointer to destination set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Mone
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOID osSelectCopySet(GT_VOID *  srcSet, GT_VOID *  dstSet)
{
    *((struct fd_set*)dstSet) = *((struct fd_set*)srcSet) ;
}


/*******************************************************************************
* osSelect()
*
* DESCRIPTION:
*       OS Dependent select function
*
* INPUTS:
*       width       - The highest-numbered descriptor in any of the next three
*                     sets + 1 (if zero, The default length will be taken)
*       readSet     - Pointer to a read operation  descriptor set
*       writeSet    - Pointer to a write operation descriptor set
*       exceptionSet- Pointer to an exception descriptor set (not supported in
*                     all OS, such as VxWorks)
*       timeOut     - Maximum time to wait on in milliseconds. Sending a
*                     GT_INFINITE value will block indefinitely. Zero value cause
*                     no block.
*
* OUTPUTS:
*       According to posix, all files descriptors will be zeroed , and only
*       bits that represents file descriptors which are ready will be set.
*
* RETURNS:
*       On success, returns the number of descriptors contained in the
*       descriptor sets,  which  may be zero if the timeout expires before
*       anything interesting happens. On  error, -1 is returned, and errno
*       is set appropriately; the sets and timeout become
*       undefined, so do not rely on their contents after an error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_SSIZE_T osSelect
(
    GT_FD      width ,
    GT_VOID *  readSet ,
    GT_VOID *  writeSet ,
    GT_VOID *  exceptionSet ,
    GT_U32     timeOut
)
{
    struct timeval tv ;
    int lwidth ,rc,ec;

    /* Convert to seconds and calculate the reminder in microseconds */
    tv.tv_sec = timeOut/1000 ;
    tv.tv_usec = (timeOut - (tv.tv_sec*1000)) * 1000;

    if (width == 0)
        lwidth = FD_SETSIZE ;
    else
        lwidth = (int)width ;

    if (timeOut == GT_INFINITE)

        return (GT_SSIZE_T)(select(lwidth ,
                         (struct fd_set*)readSet ,
                         (struct fd_set*)writeSet ,
                         (struct fd_set*)exceptionSet ,
                         NULL)) ;
    else
        {
                WORD wVersionRequested;
                WSADATA wsaData;
                int err;

                wVersionRequested = MAKEWORD( 2, 2 );

                err = WSAStartup( wVersionRequested, &wsaData );
                if ( err != 0 )
                {
                        /* Tell the user that we could not find a usable */
                        /* WinSock DLL.                                  */
                        return 0;
                }


        rc =  (GT_SSIZE_T)(select(lwidth ,
                         (struct fd_set*)readSet ,
                         (struct fd_set*)writeSet ,
                         (struct fd_set*)exceptionSet ,
                         &tv)) ;
                if(rc == SOCKET_ERROR)
                {
                        ec = WSAGetLastError();
                        Sleep(timeOut);
                }

                return (GT_SSIZE_T)rc;
        }
    /*
    ** Note that now FD_ISSET on the file descriptor with its
    ** corresponded set will be true if the file descriptor is
    ** ready
    */
}

/*******************************************************************************
* osSocketGetSocketFdSetSize()
*
* DESCRIPTION:
*       Returns the size of fd_set.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       sockFdSize - to be filled with fd_set size.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       USERS THAT IMPLEMENTS THEIR OWN OS LAYER CAN RETURN SIZE = 0.
*
*******************************************************************************/
GT_STATUS osSocketGetSocketFdSetSize
(
    OUT GT_U32*    sockFdSize
)
{
    *sockFdSize = sizeof(struct fd_set);

    return GT_OK ;
}
