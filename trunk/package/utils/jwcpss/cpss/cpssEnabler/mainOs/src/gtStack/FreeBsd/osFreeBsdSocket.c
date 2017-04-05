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
*       TCP/IP and UDP/IP sockets under the FreeBsd operating system
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>

#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtGenTypes.h>
#include <gtStack/gtStackTypes.h>
#include <gtStack/gtOsSocket.h>
#include <gtUtil/gtBmPool.h>

#define SOCKET_ADDR_POOL_SIZE   100

/* pool ID to be used for allocating    */
/* memory for socket address structures */
GT_POOL_ID  socketAddrPool = 0;

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
)
{
    int i, error = 0;
    socklen_t len = sizeof(error);
    struct {
        int errno_code;
        int err;
    } err_map[] = {
        { EAGAIN, GT_SOCKET_EAGAIN },
        { EINTR, GT_SOCKET_EINTR },
        { EINVAL, GT_SOCKET_EINVAL },
        { EMSGSIZE, GT_SOCKET_EMSGSIZE },
        { EISCONN, GT_SOCKET_EISCONN },
        { ESHUTDOWN, GT_SOCKET_SHUTDOWN },
        { EWOULDBLOCK, GT_SOCKET_EWOULDBLOCK },
        { EINPROGRESS, GT_SOCKET_EINPROGRESS },
        { EALREADY, GT_SOCKET_EALREADY },
        { ECONNRESET, GT_SOCKET_ECONNRESET },
        { ECONNREFUSED, GT_SOCKET_ECONNREFUSED },
        { EBADF, GT_SOCKET_EBADF },
        { ENETUNREACH, GT_SOCKET_ENETUNREACH },
        { EHOSTUNREACH, GT_SOCKET_EHOSTUNREACH },
        { 0, 0 }
    };
    i = getsockopt(socketFd, SOL_SOCKET, SO_ERROR, (void *)&error, &len );
    if (i != 0)
        return GT_SOCKET_ERROR_BASE;
    if (error == 0)
        return GT_OK;
    for (i = 0; err_map[i].errno_code != 0; i++)
    {
        if (err_map[i].errno_code == error)
            return err_map[i].err;
    }
    return GT_SOCKET_ERROR_BASE;
}

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
const char * osSocketLastErrorStr(void)
{
    return strerror(errno);
}

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
GT_SOCKET_FD osSocketTcpCreate (GT_32 sockSize)
{
    GT_SOCKET_FD sock;
    int          optval = 1;
    
    sock = socket(AF_INET, SOCK_STREAM, 0) ;
    
    if (sock < 0)
    {
        return GT_ERROR;
    }
    
    /* if REUSEADDR and REUSEPORT not defined and program closed and opend 
     * during short time OS returns error - "address already in use"
     */
    if(setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR, (GT_CHAR*) &optval, sizeof(optval)) != 0)
    {
        return GT_ERROR;
    }
    if(setsockopt(sock, SOL_SOCKET,  SO_REUSEPORT, (GT_CHAR*) &optval, sizeof(optval)) != 0)
    {
        return GT_ERROR;
    }
    /* TCP_NODELAY option needed to increase performance - it says to socket 
     * don't wait to buffer full - send info immediately
     */
    if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,  (GT_CHAR*) &optval, sizeof(optval)) != 0)
    {
        return GT_ERROR;
    }
    
    if(sockSize != GT_SOCKET_DEFAULT_SIZE)
    {   
        optval = sockSize;

        if (setsockopt (sock, SOL_SOCKET, SO_SNDBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
        if (setsockopt (sock, SOL_SOCKET, SO_RCVBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
    }

    return sock;
}


/*******************************************************************************
* osSocketUdpCreate()
*
* DESCRIPTION:
*       Create a socket from UDP type
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
GT_SOCKET_FD osSocketUdpCreate (GT_32 sockSize)
{
    GT_SOCKET_FD sock;
    int          optval;
    
    sock = socket(AF_INET, SOCK_DGRAM, 0) ;

    if (sock < 0)
    {
        return GT_ERROR;
    }    
    /* setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR, (GT_CHAR*) &optval, sizeof(optval)); */

    if(sockSize != GT_SOCKET_DEFAULT_SIZE)
    {   
        optval = sockSize;

        if (setsockopt (sock, SOL_SOCKET, SO_SNDBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
        if (setsockopt (sock, SOL_SOCKET, SO_RCVBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
    }
    return sock;
}


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
)
{
    struct linger ling;

    ling.l_onoff = 1;
    ling.l_linger = 5;

    /* attempt send any pending data before closing */
    setsockopt(socketFd, SOL_SOCKET, SO_LINGER,
               (GT_CHAR*) &ling, sizeof(struct linger));

    errno = 0; 
    if (close((int)socketFd)==0)
       return GT_OK;
    else
       return GT_ERROR;
}


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
)
{
    errno = 0 ;
    /* TBD - Do we need to call shutdown(sockeFd,2) ;*/
    if (close((int)socketFd)==0)
       return GT_OK ;
    else
       return GT_ERROR ;
}


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
)
{
    errno = 0;
    if (sockAddr == NULL || sockAddrLen == NULL)
    {
        return GT_FAIL ;
    }

    if(socketAddrPool == 0)
    {
        if(gtPoolCreatePool(sizeof(struct sockaddr_in), 
                            GT_4_BYTE_ALIGNMENT,
                            SOCKET_ADDR_POOL_SIZE, 
                            GT_TRUE, &socketAddrPool) != GT_OK)
        {
            return GT_FAIL;
        }
    }

    *sockAddr = (struct sockaddr_in*) gtPoolGetBuf(socketAddrPool);
    
    if (*sockAddr == NULL)
    {
        return GT_FAIL;
    }
    
    osMemSet(*sockAddr, 0, sizeof(struct sockaddr_in));
    *sockAddrLen = sizeof(struct sockaddr_in) ;

    ((struct sockaddr_in*)(*sockAddr))->sin_family = AF_INET;
    ((struct sockaddr_in*)(*sockAddr))->sin_len = sizeof(struct sockaddr_in);
    ((struct sockaddr_in*)(*sockAddr))->sin_port = htons((int)ipPort) ;

    if (ipAddr == NULL)
    {
        ((struct sockaddr_in*)(*sockAddr))->sin_addr.s_addr = INADDR_ANY;
        return GT_OK;
    }
    if (inet_aton(ipAddr, &(((struct sockaddr_in*)(*sockAddr))->sin_addr)) == 0)
    {
        gtPoolFreeBuf(socketAddrPool,*sockAddr);
        return GT_FAIL ;
    }

    return GT_OK;
}


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
)
{
    if( socketAddrPool != 0 )
    {
        gtPoolFreeBuf(socketAddrPool,sockAddr);
        sockAddr = NULL;
    }
}


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
)
{
    errno = 0 ;

    if (bind ((int) socketFd, 
              (struct sockaddr*) sockAddr, 
              (socklen_t) sockAddrLen) == 0)
       return GT_OK ;
    else
    {
        osPrintf("osSocketBind:bind failed: %s\n", strerror(errno));
        return GT_FAIL;
    }
}


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
)
{
    errno = 0 ;
    if (listen( (int)socketFd, (int)maxConnections)== 0)
       return GT_OK ;
    else
       return GT_FAIL ;
}


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
    IN  GT_SOCKET_FD     socketFd ,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T *      sockAddrLen
)
{
    GT_SOCKET_FD sock ;
    int optval = 1 ;
    socklen_t len = 0;
    
    if (sockAddrLen != NULL)
        len = (socklen_t)(*sockAddrLen);
    errno = 0 ;
    sock = accept ( (int) socketFd, 
                    (struct sockaddr*) sockAddr, 
                    sockAddrLen ? &len : NULL);

    if (sockAddrLen != NULL)
        *sockAddrLen = (GT_SIZE_T)len;

    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,  (GT_CHAR*) &optval, sizeof(optval));
    return sock ;
}


/*******************************************************************************
* osSocketConnect()
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
    IN  GT_SOCKET_FD     socketFd ,
    IN  GT_VOID *        sockAddr,
    IN  GT_SIZE_T        sockAddrLen
)
{    

    errno = 0;    

    if (connect ((int) socketFd, 
        (struct sockaddr*) sockAddr, 
        (socklen_t)sockAddrLen) != 0)
    {
        return GT_FAIL;
    }

    return GT_OK;
}


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
)
{
    int flags;
 
    if ((flags = fcntl((int)socketFd, F_GETFL, 0))  < 0)
       return GT_FAIL; 

    flags |= O_NONBLOCK ;

    if (fcntl((int)socketFd, F_SETFL, flags)  < 0)
        return GT_FAIL ;
    
    return GT_OK ;
}
  

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
)
{
    int flags ;
    
    if ((flags = fcntl((int)socketFd, F_GETFL, 0))  < 0)
       return GT_FAIL ; 

    flags &= ~O_NONBLOCK ;

    if (fcntl((int)socketFd, F_SETFL, flags)  < 0)
        return GT_FAIL ; 

    return GT_OK ;
}
  

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
)
{
    return (GT_SSIZE_T) send((int)socketFd,        /* Socket FD                 */
                          buff,                 /* Buffer to send            */
                          (size_t)buffLen,      /* Max length to send        */
                          0);                   /* Flags                     */
}


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
)
{
    return (GT_SSIZE_T) sendto((int)socketFd,             /* Socket FD                */
                            buff,                      /* Buffer to receive        */
                            (size_t)buffLen,           /* Max length to received   */
                            0,                         /* Flags                    */
                            (struct sockaddr*)sockAddr,/* Socket address recv from */
                            (socklen_t)sockAddrLen);   /* Size of socket address   */ 
}

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
)
{
    int flags = 0 ;
    if (removeFlag==GT_FALSE)
        flags = MSG_PEEK ;
  
    return (GT_SSIZE_T) recv((int)socketFd,        /* Socket FD                */
                          buff,                 /* Buffer to receive        */
                          (size_t)buffLen,      /* Max length to received   */
                          flags);               /* Flags                    */
}


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
)
{
    int flags = 0 ;
    socklen_t fromLen = 0;
    GT_SSIZE_T ret;

    if (sockAddrLen != NULL)
        fromLen = (socklen_t)(*sockAddrLen);

    if (removeFlag==GT_FALSE)
        flags = MSG_PEEK ;
  
    ret = (GT_SSIZE_T) recvfrom((int)socketFd,           /* Socket FD                */
                              buff,                      /* Buffer to receive        */
                              (size_t)buffLen,           /* Max length to received   */
                              flags,                     /* Flags                    */
                              (struct sockaddr*)sockAddr,/* Socket address recv from */
                              sockAddrLen ? & fromLen : NULL);/* Size of socket address */ 

    if (sockAddrLen != NULL)
        *sockAddrLen = (GT_SIZE_T)fromLen;

    return ret;
}


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
)
{
    SOCKET_LINGER sockLinger;
    
    sockLinger.lOnOff = 1; /* disable the linger option */
    sockLinger.lLinger = 0;

    if (setsockopt (socketFd, SOL_SOCKET, SO_LINGER, (GT_CHAR*)&sockLinger,
                    sizeof (sockLinger)) != 0)
        return GT_FAIL;
    
    return GT_OK ;
}

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
)
{
    /* extract the ip address from the socket address struct */
    *ipAddr = ntohl(((struct sockaddr_in*)(sockAddr))->sin_addr.s_addr);

    return GT_OK ;
}

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
)
{
    *sockAddrSize = sizeof(struct sockaddr_in);

    return GT_OK ;
}

/*******************************************************************************
* osSocketShutDown()
*
* DESCRIPTION:
*       The call allows either direction of the TCP connection to be closed, 
*    independent of the other direction.
*       Even though shutdown will eventually terminate the TCP connection, 
*    it does not close the socket or frees its resources. 
*    In order to close the socket and free its resources only close can be used. 
*
* INPUTS:
*       socketFd - File descriptor
*    SOCKET_SHUTDOWN_OPERATION - 
*            SOCKET_SHUTDOWN_CLOSE_INPUT,                
*            SOCKET_SHUTDOWN_CLOSE_OUTPUT,            
*            SOCKET_SHUTDOWN_CLOSE_ALL
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
*
*******************************************************************************/
GT_STATUS osSocketShutDown 
(
  IN GT_SOCKET_FD               socketFd ,
  IN SOCKET_SHUTDOWN_OPERATION     operation
)
{
    if (shutdown (socketFd, operation) != 0)
        return GT_ERROR;

    return GT_OK;
}

