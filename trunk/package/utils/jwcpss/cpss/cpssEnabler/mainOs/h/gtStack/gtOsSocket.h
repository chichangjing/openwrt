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
* gtOsSelect.h
*
* DESCRIPTION:
*       extended operating system wrapper library implements TCP/IP and UDP/IP
*       sockets under the Linux operating system..
*
* DEPENDENCIES:
*       socket implementation of the opertaing system
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#ifndef __gtOsSocket_h
#define __gtOsSocket_h

#ifdef __cplusplus
extern "C" {
#endif

#include <gtOs/gtGenTypes.h>
#include <gtStack/gtStackTypes.h>
#include <gtStack/gtOsSelect.h>


#define GT_SOCKET_DEFAULT_SIZE    -1

/* Socket file descriptor and socket port generic type */
#ifndef _WIN32
typedef int GT_SOCKET_FD ;
#else
typedef GT_UINTPTR GT_SOCKET_FD ;
#endif
typedef int GT_SOCKET_PORT ;


/*******************************************************************************
 * Typedef:     struct   SOCKET_LINGER
 *
 * Description: Structure sent to setsockopt in order to set the SO_LINGER
 *              option for the socket
 *
 * Fields :
 *       lOnOff     - enable/disable the linger option when closing socket.
 *       lLinger    - indicates the amount of time to linger.
 * Comments:
 *
 *******************************************************************************/
typedef struct
{
    GT_32   lOnOff;
    GT_32   lLinger;
}SOCKET_LINGER;



/*******************************************************************************
 * Typedef:     enum   SOCKET_SHUTDOWN_OPERATION
 *
 * Description: enum that describes what types of operation will no longer be allowed
 *
 * Fields :
 *       lOnOff     - enable/disable the linger option when closing socket.
 *       lLinger    - indicates the amount of time to linger.
 * Comments:
 *
 *******************************************************************************/
typedef enum
{
    SOCKET_SHUTDOWN_CLOSE_INPUT,                  /* receives disallowed */
    SOCKET_SHUTDOWN_CLOSE_OUTPUT,                 /* sends disallowed */
    SOCKET_SHUTDOWN_CLOSE_ALL                     /* sends and receives disallowed */
}SOCKET_SHUTDOWN_OPERATION;



/* Define socket's errors */
#define GT_SOCKET_ERROR_BASE    0x30000
/* Sockets definitions of regular Berkeley error constants */
#define GT_SOCKET_EAGAIN        (GT_SOCKET_ERROR_BASE + 11)
#define GT_SOCKET_EINTR         (GT_SOCKET_ERROR_BASE + 4)
#define GT_SOCKET_EINVAL        (GT_SOCKET_ERROR_BASE + 22)
#define GT_SOCKET_EMSGSIZE            (GT_SOCKET_ERROR_BASE + 36)
#define GT_SOCKET_EISCONN       (GT_SOCKET_ERROR_BASE + 56)
#define GT_SOCKET_SHUTDOWN      (GT_SOCKET_ERROR_BASE + 58)
#define GT_SOCKET_EWOULDBLOCK   (GT_SOCKET_ERROR_BASE + 70)
#define GT_SOCKET_EINPROGRESS   (GT_SOCKET_ERROR_BASE + 68)
#define GT_SOCKET_EALREADY      (GT_SOCKET_ERROR_BASE + 69)
#define GT_SOCKET_ECONNRESET    (GT_SOCKET_ERROR_BASE + 103)
#define GT_SOCKET_ECONNREFUSED  (GT_SOCKET_ERROR_BASE + 111)
#define GT_SOCKET_EBADF         (GT_SOCKET_ERROR_BASE + 9)
#define GT_SOCKET_ENETUNREACH   (GT_SOCKET_ERROR_BASE + 101)
#define GT_SOCKET_EHOSTUNREACH  (GT_SOCKET_ERROR_BASE + 113)

/*******************************************************************************
* osSocketLastError
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
*       GT_SOCKET_E* on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS osSocketLastError
(
    IN  GT_SOCKET_FD   socketFd
);

/*******************************************************************************
* osSocketLastErrorStr
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
const char * osSocketLastErrorStr(void);

/*******************************************************************************
* osSocketTcpCreate()
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
GT_SOCKET_FD osSocketTcpCreate (GT_32 sockSize);


/*******************************************************************************
* osSocketUdpCreate()
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
GT_SOCKET_FD osSocketUdpCreate (GT_32 sockSize);


/*******************************************************************************
* osSocketTcpDestroy()
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
GT_STATUS osSocketTcpDestroy
(
    GT_SOCKET_FD   socketFd
);


/*******************************************************************************
* osSocketUdpDestroy()
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
GT_STATUS osSocketUdpDestroy
(
    IN  GT_SOCKET_FD   socketFd
);


/*******************************************************************************
* osSocketCreateAddr()
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
GT_STATUS osSocketCreateAddr
(
    IN  const GT_CHAR *  ipAddr,
    IN  GT_SOCKET_PORT   ipPort,
    IN  GT_VOID **       sockAddr,
    IN  GT_SIZE_T *      sockAddrLen
);


/*******************************************************************************
* osSocketDestroyAddr()
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
GT_VOID osSocketDestroyAddr
(
    IN  GT_VOID *       sockAddr
);


/*******************************************************************************
* osSocketBind()
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
GT_STATUS osSocketBind
(
    IN  GT_SOCKET_FD     socketFd ,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T        sockAddrLen
);


/*******************************************************************************
* osSocketListen()
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
GT_STATUS osSocketListen
(
    IN  GT_SOCKET_FD     socketFd ,
    IN  GT_SSIZE_T       maxConnections
);


/*******************************************************************************
* osSocketAccept()
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
GT_SOCKET_FD osSocketAccept
(
    IN  GT_SOCKET_FD     socketFd,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T *      sockAddrLen
);


/*******************************************************************************
* osSocketconnect()
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
GT_STATUS osSocketConnect
(
    IN  GT_SOCKET_FD     socketFd,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T        sockAddrLen
);


/*******************************************************************************
* osSocketSetNonBlock()
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
GT_STATUS osSocketSetNonBlock
(
    IN  GT_SOCKET_FD     socketFd
);


/*******************************************************************************
* osSocketSetBlock()
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
GT_STATUS osSocketSetBlock
(
    IN  GT_SOCKET_FD     socketFd
);


/*******************************************************************************
* osSocketSend()
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
GT_SSIZE_T osSocketSend
(
    IN  GT_SOCKET_FD   socketFd ,
    IN  const GT_VOID *buff,
    IN  GT_SIZE_T      buffLen
);


/*******************************************************************************
* osSocketSentTo()
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
GT_SSIZE_T osSocketSendTo
(
    IN  GT_SOCKET_FD   socketFd ,
    IN  const GT_VOID *buff,
    IN  GT_SIZE_T      buffLen,
    IN  GT_VOID *      sockAddr,
    IN  GT_SIZE_T      sockAddrLen
);


/*******************************************************************************
* osSocketRecv()
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
GT_SSIZE_T osSocketRecv
(
    IN  GT_SOCKET_FD   socketFd,
    IN  GT_VOID *      buff,
    IN  GT_SIZE_T      buffLen,
    IN  GT_BOOL        removeFlag
);


/*******************************************************************************
* osSocketRecvFrom()
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
GT_SSIZE_T osSocketRecvFrom
(
    IN  GT_SOCKET_FD   socketFd ,
    IN  GT_VOID *      buff,
    IN  GT_SIZE_T      buffLen,
    IN  GT_BOOL        removeFlag,
    OUT GT_VOID *      sockAddr,
    OUT GT_SIZE_T *    sockAddrLen
);


/*******************************************************************************
* osSocketSetSocketNoLinger()
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
GT_STATUS osSocketSetSocketNoLinger
(
    IN  GT_SOCKET_FD     socketFd
);

/*******************************************************************************
* osSocketExtractIpAddrFromSocketAddr()
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
GT_STATUS osSocketExtractIpAddrFromSocketAddr
(
    IN  GT_VOID*   sockAddr,
    OUT GT_U32*    ipAddr
);

/*******************************************************************************
* osSocketGetSocketAddrSize()
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
GT_STATUS osSocketGetSocketAddrSize
(
    OUT GT_U32*    sockAddrSize
);



/*******************************************************************************
* osSocketShutDown ()
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
GT_STATUS osSocketShutDown
(
  IN	GT_SOCKET_FD   				socketFd ,
  IN	SOCKET_SHUTDOWN_OPERATION 	operation
);


#ifdef __cplusplus
}
#endif

#endif  /* ifndef __gtOsSocket_h */
/* Do Not Add Anything Below This Line */

