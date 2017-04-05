/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* osFreeBsdSelect.c
*
* DESCRIPTION:
*       FreeBsd User Mode Operating System wrapper. 
*       synchronous I/O multiplexing facility
*
* DEPENDENCIES:
*       FreeBsd, CPU independed 
*       select implementation of the operating system
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <gtOs/gtOsMem.h>
#include <gtOs/gtGenTypes.h>
#include <gtStack/gtStackTypes.h>
#include <gtStack/gtOsSelect.h>
#include <gtUtil/gtBmPool.h>

#define SOCKET_FD_SET_POOL_SIZE   50

GT_POOL_ID   socketFdPool = 0 ;     /* pool ID to be used for allocating
                                         memory for FD sets */

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
GT_VOID * osSelectCreateSet
(
)
{
   fd_set * set ;

    if( socketFdPool == 0 )
    {

        if(gtPoolCreatePool(sizeof(fd_set), 
                            GT_4_BYTE_ALIGNMENT,
                            SOCKET_FD_SET_POOL_SIZE, 
                            GT_TRUE, &socketFdPool) != GT_OK)
        {
            return NULL;
        }
    }

    set = (fd_set*) gtPoolGetBuf(socketFdPool);
    if (set == NULL) 
        return NULL ;

    FD_ZERO(set) ;    
  
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
GT_VOID osSelectEraseSet 
(
    GT_VOID *  set  
)
{
    if( socketFdPool != 0 )
    {
        gtPoolFreeBuf(socketFdPool, set);
    }
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
GT_VOID osSelectZeroSet 
(
    GT_VOID *  set  
)
{
    FD_ZERO((fd_set *)set) ;
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
GT_VOID osSelectAddFdToSet
(
    GT_VOID *  set ,
    GT_FD      fd   
)
{
    FD_SET((int)fd, (fd_set*)set) ;
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
GT_VOID osSelectClearFdFromSet
(
    GT_VOID *  set ,
    GT_FD      fd   
)
{
    FD_CLR((int)fd, (fd_set*)set) ;
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
GT_BOOL osSelectIsFdSet
(
    GT_VOID *  set ,
    GT_FD      fd   
)
{
    if( (FD_ISSET((int)fd, (fd_set*)set)) != 0 )
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
GT_VOID osSelectCopySet
(
    GT_VOID *  srcSet ,
    GT_VOID *  dstSet 
)
{
    *((fd_set*)dstSet) = *((fd_set*)srcSet) ;
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
    int lwidth ;

    if (width == 0) 
        lwidth = FD_SETSIZE ;
    else
        lwidth = (int)width ;
 
    if (timeOut == GT_INFINITE)
        return (GT_SSIZE_T)(select(lwidth ,  
                         (fd_set*)readSet ,
                         (fd_set*)writeSet ,
                         (fd_set*)exceptionSet ,
                         NULL));
    else 
    {
        /* Convert to seconds and calculate the reminder in microseconds */
        tv.tv_sec = timeOut/1000 ; 
        tv.tv_usec = (timeOut - (tv.tv_sec*1000)) * 1000; 

        return (GT_SSIZE_T)(select(lwidth ,  
                        (fd_set*)readSet ,
                        (fd_set*)writeSet ,
                        (fd_set*)exceptionSet ,
                        &tv));
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
******************************************************************************/
GT_STATUS osSocketGetSocketFdSetSize
(
    OUT GT_U32*    sockFdSize
)
{
    *sockFdSize = sizeof( fd_set);

    return GT_OK ;
}
