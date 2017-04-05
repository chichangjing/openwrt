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
* cpssOsSocket.h
*
* DESCRIPTION:
*       extended operating system wrapper library implements TCP/IP and UDP/IP
*       sockets under the Linux operating system..
*
* DEPENDENCIES:
*       socket implementation of the opertaing system
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssOsSocket_h
#define __cpssOsSocket_h

#ifdef __cplusplus
extern "C" {
#endif

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifndef GT_SOCKET_DEFAULT_SIZE
#define GT_SOCKET_DEFAULT_SIZE    -1
#endif /* GT_SOCKET_DEFAULT_SIZE */

/* Socket file descriptor and socket port generic type */
#ifndef _WIN32
typedef int CPSS_SOCKET_FD;
#else
typedef GT_UINTPTR CPSS_SOCKET_FD;
#endif
typedef int CPSS_SOCKET_PORT;



/*******************************************************************************
 * Typedef:     enum   CPSS_SOCKET_SHUTDOWN_OPERATION
 *
 * Description: enum that describes what types of operation will no longer be allowed
 *
 * Comments:
 *
 *******************************************************************************/
typedef enum
{
    CPSS_SOCKET_SHUTDOWN_CLOSE_INPUT,                  /* receives disallowed */
    CPSS_SOCKET_SHUTDOWN_CLOSE_OUTPUT,                 /* sends disallowed */
    CPSS_SOCKET_SHUTDOWN_CLOSE_ALL                     /* sends and receives disallowed */
}CPSS_SOCKET_SHUTDOWN_OPERATION;



/* Define socket's errors */
#define CPSS_SOCKET_ERROR_BASE    (0x30000)
/* Sockets definitions of regular Berkeley error constants */
#define CPSS_SOCKET_EAGAIN        (CPSS_SOCKET_ERROR_BASE + 11)
#define CPSS_SOCKET_EINTR         (CPSS_SOCKET_ERROR_BASE + 4)
#define CPSS_SOCKET_EINVAL        (CPSS_SOCKET_ERROR_BASE + 22)
#define CPSS_SOCKET_EMSGSIZE      (CPSS_SOCKET_ERROR_BASE + 36)
#define CPSS_SOCKET_EISCONN       (CPSS_SOCKET_ERROR_BASE + 56)
#define CPSS_SOCKET_ESHUTDOWN     (CPSS_SOCKET_ERROR_BASE + 58)
#define CPSS_SOCKET_EWOULDBLOCK   (CPSS_SOCKET_ERROR_BASE + 70)
#define CPSS_SOCKET_EINPROGRESS   (CPSS_SOCKET_ERROR_BASE + 68)
#define CPSS_SOCKET_EALREADY      (CPSS_SOCKET_ERROR_BASE + 69)
#define CPSS_SOCKET_ECONNRESET    (CPSS_SOCKET_ERROR_BASE + 103)
#define CPSS_SOCKET_ECONNREFUSED  (CPSS_SOCKET_ERROR_BASE + 111)
#define CPSS_SOCKET_EBADF         (CPSS_SOCKET_ERROR_BASE + 9)
#define CPSS_SOCKET_ENETUNREACH   (CPSS_SOCKET_ERROR_BASE + 101)
#define CPSS_SOCKET_EHOSTUNREACH  (CPSS_SOCKET_ERROR_BASE + 113)

/*******************************************************************************
* CPSS_SOCKET_LAST_ERROR_FUNC()
*
* DESCRIPTION:
*       Rerturn last error code for a given socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 - no error
*       CPSS_SOCKET_E* on error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_LAST_ERROR_FUNC)
(
    IN  CPSS_SOCKET_FD   socketFd
);

/*******************************************************************************
* CPSS_SOCKET_LAST_ERROR_STR_FUNC
*
* DESCRIPTION:
*       returns error string for last error
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       string
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef const char * (*CPSS_SOCKET_LAST_ERROR_STR_FUNC) (void);

/*******************************************************************************
* CPSS_SOCKET_TCP_CREATE_FUNC()
*
* DESCRIPTION:
*       Create a socket from TCP type
*
* INPUTS:
*       sockSize - the size of the receive & send socket buffer. -1 indicates
*                  the default size.
*
* OUTPUTS:
*
* RETURNS:
*       Valid file descriptor on success
*       -1 On fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef CPSS_SOCKET_FD (*CPSS_SOCKET_TCP_CREATE_FUNC) (GT_32 sockSize);


/*******************************************************************************
* CPSS_SOCKET_UDP_CREATE_FUNC()
*
* DESCRIPTION:
*       sockSize - the size of the receive & send socket buffer. -1 indicates
*                  the default size.*
* INPUTS:
*       None
*
* OUTPUTS:
*
* RETURNS:
*       Valid file descriptor on success
*       -1 On fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef CPSS_SOCKET_FD (*CPSS_SOCKET_UDP_CREATE_FUNC)(GT_32 sockSize);


/*******************************************************************************
* CPSS_SOCKET_TCP_DESTROY_FUNC()
*
* DESCRIPTION:
*       Destroy a TCP socket
*
* INPUTS:
*       socketFd - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_TCP_DESTROY_FUNC)
(
    CPSS_SOCKET_FD   socketFd
);


/*******************************************************************************
* CPSS_SOCKET_UDP_DESTROY_FUNC()
*
* DESCRIPTION:
*       Destroy a UDP socket
*
* INPUTS:
*       socketFd - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_UDP_DESTROY_FUNC)
(
    IN  CPSS_SOCKET_FD   socketFd
);


/*******************************************************************************
* CPSS_SOCKET_CREATE_ADDR_FUNC()
*
* DESCRIPTION:
*       Build a TCP/IP address to be used across all other functions
*
* INPUTS:
*       ipAddr       - String represents a dotted decimal IP address, such as
*                      "10.2.40.10"
*                      If ipAddr == NULL function will return INADDR_ANY
*       ipPort       - IP Port
*
* OUTPUTS:
*       sockAdd      - Pointer to a buffer pointer sent by the caller that is
*                      used as the address. The caller is unaware of what is
*                      the sockAddr size and type.THe function allocates the
*                      required size for the structure
*       sockAddrLen  - Length of the size used in sockAdd
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_CREATE_ADDR_FUNC)
(
    IN  const GT_CHAR *  ipAddr,
    IN  CPSS_SOCKET_PORT ipPort,
    IN  GT_VOID **       sockAddr,
    IN  GT_SIZE_T *      sockAddrLen
);


/*******************************************************************************
* CPSS_SOCKET_DESTROY_ADDR_FUNC()
*
* DESCRIPTION:
*       Destroy a TCP/IP address object built via  osSocketBuildAddr()
*
* INPUTS:
*       sockAdd      - Pointer to a buffer pointer sent by the caller that is
*                      used as the address. The caller is unaware of what is
*                      the sockAddr size and type.
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
typedef GT_VOID (*CPSS_SOCKET_DESTROY_ADDR_FUNC)
(
    IN  GT_VOID *       sockAddr
);


/*******************************************************************************
* CPSS_SOCKET_BIND_FUNC()
*
* DESCRIPTION:
*       Bind a name to a TCP or UDP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       sockAddr    - A pointer to the local socket address structure,
*                     probably filled out by osSocketBuildAddr.The caller
*                     is unaware of what is the sockAddr type.
*       sockAddrLen - Length of socketAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_BIND_FUNC)
(
    IN  CPSS_SOCKET_FD   socketFd ,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T        sockAddrLen
);


/*******************************************************************************
* CPSS_SOCKET_LISTEN_FUNC()
*
* DESCRIPTION:
*       Listen for new connections on a TCP socket
*
* INPUTS:
*       socketFd       - Socket descriptor
*       maxConnections - Number of connections to queue
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       Used only for TCP connection oriented sockets
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_LISTEN_FUNC)
(
    IN  CPSS_SOCKET_FD   socketFd ,
    IN  GT_SSIZE_T       maxConnections
);


/*******************************************************************************
* CPSS_SOCKET_ACCEPT_FUNC()
*
* DESCRIPTION:
*       Extracts the first TCP connection request on the queue of pending
*       connections and creates a new connected socket.
*
* INPUTS:
*       socketFd    - Socket descriptor
*       sockAddr    - A pointer to a socket address structure, represents the
*                     client (target) address that requests the conenction.
*                     If the server is not interested in the Client's protocol
*                     information, a NULL should be provided.
*       sockAddrLen - A value-result parameter, initialized to the size of
*                     the allocated buffer sockAddr, and modified on return
*                     to indicate the actual size of the address stored there.
*                     If the server is not interested in the Client's protocol
*                     information, a NULL should be provided.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       socketFd - Socket descriptor
*
* COMMENTS:
*       Used only for TCP connection oriented sockets
*
*******************************************************************************/
typedef CPSS_SOCKET_FD (*CPSS_SOCKET_ACCEPT_FUNC)
(
    IN  CPSS_SOCKET_FD   socketFd,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T *      sockAddrLen
);


/*******************************************************************************
* CPSS_SOCKET_CONNECT_FUNC()
*
* DESCRIPTION:
*       Connect socket to a remote address:
*       For UDP sockets, specifies the address to which datagrams are sent by
*       default, and the only address from which datagrams are received. For
*       TCP sockets, used by the TCP client that attempts to make a connection
*       to the TCP server.
*
* INPUTS:
*       socketFd    - Socket descriptor
*       sockAddr    - A pointer to a socket address structure, represents the
*                     remote address to connect to. For TCP, it is the server
*                     address.
*       sockAddrLen - Length of sockAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_CONNECT_FUNC)
(
    IN  CPSS_SOCKET_FD   socketFd,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T        sockAddrLen
);


/*******************************************************************************
* CPSS_SOCKET_SET_NONBLOCK_FUNC()
*
* DESCRIPTION:
*       Set a socket option to be non-blocked
*
* INPUTS:
*       socketFd - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_SET_NONBLOCK_FUNC)
(
    IN  CPSS_SOCKET_FD     socketFd
);


/*******************************************************************************
* CPSS_SOCKET_SET_BLOCK_FUNC()
*
* DESCRIPTION:
*       Set a socket option to be blocked
*
* INPUTS:
*       socketFd    - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_SET_BLOCK_FUNC)
(
    IN  CPSS_SOCKET_FD     socketFd
);


/*******************************************************************************
* CPSS_SOCKET_SEND_FUNC()
*
* DESCRIPTION:
*       Send data to a TCP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to send
*       buffLen     - Max length to send
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SSIZE_T (*CPSS_SOCKET_SEND_FUNC)
(
    IN  CPSS_SOCKET_FD  socketFd ,
    IN  const GT_VOID * buff,
    IN  GT_SIZE_T       buffLen
);


/*******************************************************************************
* CPSS_SOCKET_SENDTO_FUNC()
*
* DESCRIPTION:
*       Send data to a UDP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to send
*       buffLen     - Max length to send
*       sockAddr    - A pointer to the targent socket address structure,
*                     probably filled out by osSocketBuildAddr.  The caller
*                     is unaware of what is the sockAddr type.
*       sockAddrLen - Length of socketAddr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SSIZE_T (*CPSS_SOCKET_SENDTO_FUNC)
(
    IN  CPSS_SOCKET_FD  socketFd ,
    IN  const GT_VOID * buff,
    IN  GT_SIZE_T       buffLen,
    IN  GT_VOID *       sockAddr,
    IN  GT_SIZE_T       sockAddrLen
);


/*******************************************************************************
* CPSS_SOCKET_RECV_FUNC()
*
* DESCRIPTION:
*       Receive data from a TCP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to receive
*       buffLen     - Max length to send
*       removeFlag  - Flag indicate if to remove or no remove the data from
*                     the socket buffer
*
* OUTPUTS:
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SSIZE_T (*CPSS_SOCKET_RECV_FUNC)
(
    IN  CPSS_SOCKET_FD socketFd,
    IN  GT_VOID *      buff,
    IN  GT_SIZE_T      buffLen,
    IN  GT_BOOL        removeFlag
);


/*******************************************************************************
* CPSS_SOCKET_RECVFROM_FUNC()
*
* DESCRIPTION:
*       Receive data from a UDP socket
*
* INPUTS:
*       socketFd    - Socket descriptor
*       buff        - Buffer to send
*       buffLen     - Max length to send
*       removeFlag  - Flag indicate if to remove or no remove the data from
*                     the socket buffer
*
* OUTPUTS:
*       sockAddr    - A pointer to a socket address structure, represents
*                     the source (target) address that has sent the packet.
*                     the caller can then compare the address later.
*                     If this is NULL, this information will not be provided.
*       sockAddrLen - A value-result parameter, initialized to the size of
*                     the allocated buffer sockAddr, and modified on return
*                     to indicate the actual size of the address stored there.
*
* RETURNS:
*       Number of bytes sent
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_SSIZE_T (*CPSS_SOCKET_RECVFROM_FUNC)
(
    IN  CPSS_SOCKET_FD socketFd ,
    IN  GT_VOID *      buff,
    IN  GT_SIZE_T      buffLen,
    IN  GT_BOOL        removeFlag,
    OUT GT_VOID *      sockAddr,
    OUT GT_SIZE_T *    sockAddrLen
);


/*******************************************************************************
* CPSS_SOCKET_SET_SOCKET_NOLINGER_FUNC()
*
* DESCRIPTION:
*       Set the socket option to be no linger when closing connection.
*
* INPUTS:
*       socketFd    - Socket descriptor
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_SET_SOCKET_NOLINGER_FUNC)
(
    IN  CPSS_SOCKET_FD     socketFd
);

/*******************************************************************************
* CPSS_SOCKET_EXTRACT_IPADDR_FROM_SOCKET_ADDR_FUNC()
*
* DESCRIPTION:
*       Extracts the IP address from sockaddr_in structure.
*
* INPUTS:
*       sockAddr    - pointer to sockaddr_in.
*       ipAddr      - pointer to address in GT_U32 format to be filled by the
*                     routine.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_EXTRACT_IPADDR_FROM_SOCKET_ADDR_FUNC)
(
    IN  GT_VOID*   sockAddr,
    OUT GT_U32*    ipAddr
);

/*******************************************************************************
* CPSS_SOCKET_GET_ADDR_SIZE_FUNC()
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
typedef GT_STATUS (*CPSS_SOCKET_GET_ADDR_SIZE_FUNC)
(
    OUT GT_U32*    sockAddrSize
);



/*******************************************************************************
* CPSS_SOCKET_SHUTDOWN_FUNC()
*
* DESCRIPTION:
*       This routine shuts down all, or part, of a connection-based socket.
*
* INPUTS:
*       socketFd    - Socket descriptor
*       operation   - 0 = receives disallowed
*                     1 = sends disallowed
*                     2 = sends and disallowed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on fail
*
* COMMENTS:
*
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_SOCKET_SHUTDOWN_FUNC)
(
  IN	CPSS_SOCKET_FD   				socketFd ,
  IN	CPSS_SOCKET_SHUTDOWN_OPERATION 	operation
);


/* CPSS_SOCKET_BIND_STC -
    structure that hold the "socket" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_SOCKET_LAST_ERROR_FUNC           osSocketLastErrorFunc;
    CPSS_SOCKET_LAST_ERROR_STR_FUNC       osSocketLastErrorStrFunc;
    CPSS_SOCKET_TCP_CREATE_FUNC           osSocketTcpCreateFunc;
    CPSS_SOCKET_UDP_CREATE_FUNC           osSocketUdpCreateFunc;
    CPSS_SOCKET_TCP_DESTROY_FUNC          osSocketTcpDestroyFunc;
    CPSS_SOCKET_UDP_DESTROY_FUNC          osSocketUdpDestroyFunc;
    CPSS_SOCKET_CREATE_ADDR_FUNC          osSocketCreateAddrFunc;
    CPSS_SOCKET_DESTROY_ADDR_FUNC         osSocketDestroyAddrFunc;
    CPSS_SOCKET_BIND_FUNC                 osSocketBindFunc;
    CPSS_SOCKET_LISTEN_FUNC               osSocketListenFunc;
    CPSS_SOCKET_ACCEPT_FUNC               osSocketAcceptFunc;
    CPSS_SOCKET_CONNECT_FUNC              osSocketConnectFunc;
    CPSS_SOCKET_SET_NONBLOCK_FUNC         osSocketSetNonBlockFunc;
    CPSS_SOCKET_SET_BLOCK_FUNC            osSocketSetBlockFunc;
    CPSS_SOCKET_SEND_FUNC                 osSocketSendFunc;
    CPSS_SOCKET_SENDTO_FUNC               osSocketSendToFunc;
    CPSS_SOCKET_RECV_FUNC                 osSocketRecvFunc;
    CPSS_SOCKET_RECVFROM_FUNC             osSocketRecvFromFunc;
    CPSS_SOCKET_SET_SOCKET_NOLINGER_FUNC  osSocketSetSocketNoLingerFunc;
    CPSS_SOCKET_EXTRACT_IPADDR_FROM_SOCKET_ADDR_FUNC
                                          osSocketExtractIpAddrFromSocketAddrFunc;
    CPSS_SOCKET_GET_ADDR_SIZE_FUNC        osSocketGetSocketAddrSizeFunc;
    CPSS_SOCKET_SHUTDOWN_FUNC             osSocketShutDownFunc;
}CPSS_SOCKET_BIND_STC;


#ifdef __cplusplus
}
#endif

#endif  /* ifndef __cpssOsSocket_h */
/* Do Not Add Anything Below This Line */
