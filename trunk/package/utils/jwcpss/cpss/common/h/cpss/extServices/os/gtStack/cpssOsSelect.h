/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsSelect.h
*
* DESCRIPTION:
*       extended operating system wrapper library implements select()
*       functions
*
* DEPENDENCIES:
*       select implementation of the opertaing system
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __cpssOsSelect_h
#define __cpssOsSelect_h

#ifdef __cplusplus
extern "C" {
#endif

#include <cpss/extServices/os/gtOs/gtGenTypes.h>
#include <cpss/extServices/os/gtStack/cpssOsSocket.h>

#define GT_INFINITE 0xffffffff

/*******************************************************************************
* CPSS_SOCKET_SELECT_CREATE_SET_FUNC()
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
typedef GT_VOID * (*CPSS_SOCKET_SELECT_CREATE_SET_FUNC)
(
    void
);


/*******************************************************************************
* CPSS_SOCKET_SELECT_ERASE_SET_FUNC()
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
typedef GT_VOID (*CPSS_SOCKET_SELECT_ERASE_SET_FUNC)
(
    GT_VOID *  set
);


/*******************************************************************************
* CPSS_SOCKET_SELECT_ZERO_SET_FUNC()
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
typedef GT_VOID (*CPSS_SOCKET_SELECT_ZERO_SET_FUNC)
(
    GT_VOID *  set
);


/*******************************************************************************
* CPSS_SOCKET_SELECT_ADD_FD_TO_SET_FUNC()
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
typedef GT_VOID (*CPSS_SOCKET_SELECT_ADD_FD_TO_SET_FUNC)
(
    GT_VOID *      set,
    CPSS_SOCKET_FD fd
);


/*******************************************************************************
* CPSS_SOCKET_SELECT_CLEAR_FD_FROM_SET_FUNC()
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
typedef GT_VOID (*CPSS_SOCKET_SELECT_CLEAR_FD_FROM_SET_FUNC)
(
    GT_VOID *      set,
    CPSS_SOCKET_FD fd
);


/*******************************************************************************
* CPSS_SOCKET_SELECT_IS_FD_SET_FUNC()
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
typedef GT_BOOL (*CPSS_SOCKET_SELECT_IS_FD_SET_FUNC)
(
    GT_VOID *      set,
    CPSS_SOCKET_FD fd
);


/*******************************************************************************
* CPSS_SOCKET_SELECT_COPY_SET_FUNC()
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
typedef GT_VOID (*CPSS_SOCKET_SELECT_COPY_SET_FUNC)
(
    GT_VOID *  srcSet ,
    GT_VOID *  dstSet 
);


/*******************************************************************************
* CPSS_SOCKET_SELECT_FUNC()
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
typedef GT_SSIZE_T (*CPSS_SOCKET_SELECT_FUNC)
(
    CPSS_SOCKET_FD width ,
    GT_VOID *      readSet ,
    GT_VOID *      writeSet ,
    GT_VOID *      exceptionSet ,
    GT_U32         timeOut
);

/*******************************************************************************
* CPSS_SOCKET_SELECT_GET_SOCKET_FD_SIZE_FUNC()
*
* DESCRIPTION:
*       Returns the size of sockaddr_in.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       sockAddrSize - to be filled with sockaddr_in size.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       USERS THAT IMPLEMENTS THEIR OWN OS LAYER CAN RETURN SIZE = 0.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_SELECT_GET_SOCKET_FD_SIZE_FUNC)
(
    OUT GT_U32*    sockFdSize
);

/* CPSS_SOCKET_SELECT_BIND_STC -
    structure that hold the "socket select" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_SOCKET_SELECT_CREATE_SET_FUNC         osSelectCreateSetFunc;
    CPSS_SOCKET_SELECT_ERASE_SET_FUNC          osSelectEraseSetFunc;
    CPSS_SOCKET_SELECT_ZERO_SET_FUNC           osSelectZeroSetFunc;
    CPSS_SOCKET_SELECT_ADD_FD_TO_SET_FUNC      osSelectAddFdToSetFunc;
    CPSS_SOCKET_SELECT_CLEAR_FD_FROM_SET_FUNC  osSelectClearFdFromSetFunc;
    CPSS_SOCKET_SELECT_IS_FD_SET_FUNC          osSelectIsFdSetFunc;
    CPSS_SOCKET_SELECT_COPY_SET_FUNC           osSelectCopySetFunc;
    CPSS_SOCKET_SELECT_FUNC                    osSelectFunc;
    CPSS_SOCKET_SELECT_GET_SOCKET_FD_SIZE_FUNC osSocketGetSocketFdSetSizeFunc;
}CPSS_SOCKET_SELECT_BIND_STC;

#ifdef __cplusplus
}
#endif

#endif  /* ifndef __cpssOsSelect_h */
/* Do Not Add Anything Below This Line */

