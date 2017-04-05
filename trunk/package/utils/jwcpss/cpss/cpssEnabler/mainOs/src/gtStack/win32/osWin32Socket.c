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
* osWin32Select.c
*
* DESCRIPTION:
*       Win32 User Mode Operating System wrapper.
*       synchronous I/O multiplexing facility
*
* DEPENDENCIES:
*       Win32, CPU independed
*       TCP/IP and UDP/IP sockets under the Windows operating system
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <winsock2.h>
#include <stdio.h>

#include <gtOs/gtOsIo.h>
#include <gtOs/gtOsMem.h>
#include <gtStack/gtStackTypes.h>
#include <gtStack/gtOsSocket.h>
#include <gtUtil/gtBmPool.h>


/* the user configure socket buffer size */
extern GT_U32 gSocketBuffSize;

GT_POOL_ID  socketAddrPool = 0;;   /* pool ID to be used for allocating
                                         memory for socket address structures */
#define SOCKET_ADDR_POOL_SIZE   100

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
    int len = sizeof(error);
    struct {
        int errno_code;
        int err;
    } err_map[] = {
        { WSATRY_AGAIN, GT_SOCKET_EAGAIN },
        { WSAEINTR, GT_SOCKET_EINTR },
        { WSAEINVAL, GT_SOCKET_EINVAL },
        { WSAEMSGSIZE, GT_SOCKET_EMSGSIZE },
        { WSAEISCONN, GT_SOCKET_EISCONN },
        { WSAESHUTDOWN, GT_SOCKET_SHUTDOWN },
        { WSAEWOULDBLOCK, GT_SOCKET_EWOULDBLOCK },
        { WSAEINPROGRESS, GT_SOCKET_EINPROGRESS },
        { WSAEALREADY, GT_SOCKET_EALREADY },
        { WSAECONNRESET, GT_SOCKET_ECONNRESET },
        { WSAECONNREFUSED, GT_SOCKET_ECONNREFUSED },
        { WSAEBADF, GT_SOCKET_EBADF },
        { WSAENETUNREACH, GT_SOCKET_ENETUNREACH },
        { WSAEHOSTUNREACH, GT_SOCKET_EHOSTUNREACH },
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
    static char error[40];
    sprintf(error, "WSA error %d", WSAGetLastError());
    return error;
}

/*******************************************************************************
* osSocketTcpCreate()
*
* DESCRIPTION:
*       Create a socket from UDP type
*
* INPUTS:
*       sockSize - the size of the receive & send socket buffer. -1 indicates
*                  the default size.*
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
    int       rc;
    WSADATA   ptr;
    int optval = 1 ;
    GT_SOCKET_FD ret;

    rc = WSAStartup (0x0202, &ptr);
    if (rc != 0)
        return (INVALID_SOCKET);

    ret =  socket(AF_INET, SOCK_STREAM, 0);
    if (ret == INVALID_SOCKET)
    {
        rc = WSAGetLastError ();
        return (GT_SOCKET_FD)(-1);                /* failure */
    }
    /* this section was commented since it caused the sockets to fail in connection */
    /* if(sockSize != GT_SOCKET_DEFAULT_SIZE)
    {
        if (setsockopt (ret, SOL_SOCKET, SO_SNDBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
        if (setsockopt (ret, SOL_SOCKET, SO_RCVBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
    }*/

    return (ret);

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
    int rc;
    WSADATA   ptr;
    int optval = 1 ;
    GT_SOCKET_FD ret;

    rc = WSAStartup (0x0202, &ptr);
    if (rc != 0)
        return (INVALID_SOCKET);

    ret =  socket(AF_INET, SOCK_DGRAM, 0);
    if (ret == INVALID_SOCKET)
    {
        rc = WSAGetLastError ();
        return (GT_SOCKET_FD)(-1);                /* failure */
    }

    if(sockSize != GT_SOCKET_DEFAULT_SIZE)
    {
        if (setsockopt (ret, SOL_SOCKET, SO_SNDBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
        if (setsockopt (ret, SOL_SOCKET, SO_RCVBUF, (GT_CHAR*)&optval,
                        sizeof (optval)) != 0)
        {
            return GT_ERROR;
        }
    }
    return (ret);

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
GT_STATUS osSocketTcpDestroy(GT_SOCKET_FD  socketFd)
{
    int ret;

    if (closesocket((SOCKET)socketFd)==0)
       return GT_OK ;

    ret = WSAGetLastError ();

    return (GT_ERROR);
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
GT_STATUS osSocketUdpDestroy(IN  GT_SOCKET_FD   socketFd )
{
    int ret;

    if (closesocket((SOCKET)socketFd) == 0)
       return GT_OK ;

    ret = WSAGetLastError ();

        return (GT_ERROR);
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
GT_STATUS osSocketCreateAddr( IN  const GT_CHAR *  ipAddr,
                              IN  GT_SOCKET_PORT   ipPort,
                              IN  GT_VOID **       sockAddr,
                              IN  GT_SIZE_T *      sockAddrLen )
{
    unsigned long address;

    if (sockAddr == NULL || sockAddrLen == NULL)
        return GT_FAIL ;


    if( socketAddrPool == 0 )
    {
        if(gtPoolCreatePool(sizeof(struct sockaddr_in),
                            GT_4_BYTE_ALIGNMENT,
                            SOCKET_ADDR_POOL_SIZE,
                            GT_TRUE, &socketAddrPool) != GT_OK)
         return GT_FAIL;
                *sockAddr = (struct sockaddr_in*) gtPoolGetBuf(socketAddrPool);
    }
    else
        *sockAddr = (struct sockaddr_in*) gtPoolGetBuf(socketAddrPool);

    if (*sockAddr==NULL)
        return GT_FAIL ;

    *sockAddrLen = sizeof(struct sockaddr_in) ;
    ((struct sockaddr_in*)(*sockAddr))->sin_family = AF_INET ;
    ((struct sockaddr_in*)(*sockAddr))->sin_port = htons((u_short)ipPort) ;

    if (ipAddr == NULL)
    {
        ((struct sockaddr_in*)(*sockAddr))->sin_addr.s_addr = INADDR_ANY;
        return GT_OK;
    }
    address = inet_addr(ipAddr);
    if (address == INADDR_NONE)
        return (GT_FAIL);
    else
        {
            ((struct sockaddr_in*)(*sockAddr))->sin_addr.s_addr =address;
            return (GT_OK);
        }
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
GT_VOID osSocketDestroyAddr(IN  GT_VOID * sockAddr)
{
    if( socketAddrPool != 0 )
    {
        gtPoolFreeBuf(socketAddrPool,sockAddr);
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
GT_STATUS osSocketBind(IN  GT_SOCKET_FD     socketFd ,
                       IN  GT_VOID *        sockAddr,
                       IN  GT_SIZE_T        sockAddrLen)
{
    int ret;
    WSADATA   ptr;

    ret = WSAStartup (0x0202, &ptr);
    if (ret != 0)
        return (INVALID_SOCKET);

    if (bind ((SOCKET) socketFd,(struct sockaddr*) sockAddr,
              (int) sockAddrLen)==0)
    {
        return GT_OK ;
    }
    else
        ret = WSAGetLastError ();

    return GT_FAIL ;
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
GT_STATUS osSocketListen(IN  GT_SOCKET_FD     socketFd ,
                         IN  GT_SSIZE_T       maxConnections)
{
    int ret;


    if (listen( (SOCKET)socketFd, (int)maxConnections)==0)
    {
                return GT_OK ;
    }
    else
    {
                ret = WSAGetLastError ();
    }

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
*
* OUTPUTS:
*       sockAddr    - A pointer to a socket address structure, represents the
*                     client (target) address that requests the conenction.
*                     If the server is not interested in the Client's protocol
*                     information, a NULL should be provided.
*       sockAddrLen - A value-result parameter, initialized to the size of
*                     the allocated buffer sockAddr, and modified on return
*                     to indicate the actual size of the address stored there.
*
* RETURNS:
*       Valid file descriptor on success
*       -1 On fail
*
* COMMENTS:
*       Used only for TCP connection oriented sockets
*
*******************************************************************************/
GT_SOCKET_FD osSocketAccept(IN  GT_SOCKET_FD     socketFd ,
                            IN  GT_VOID *        sockAddr,
                            IN  GT_SIZE_T *      sockAddrLen)
{
    int ret;
    GT_SOCKET_FD newSocket;
    int len = 0;

    if (sockAddr == NULL)
        sockAddrLen = NULL;

    if (sockAddrLen != NULL)
        len = (int)(*sockAddrLen);

    newSocket = accept ((SOCKET) socketFd,(struct sockaddr *) sockAddr,
                sockAddrLen ? &len : NULL);

    if (sockAddrLen != NULL)
        *sockAddrLen = (GT_SIZE_T)len;

    if ( (newSocket  == INVALID_SOCKET) )
    {
        ret = WSAGetLastError ();
        return (0);
    }

    return (newSocket);
}

/*******************************************************************************
* osSocketConnect()
*
* DESCRIPTION:
*       Connect socket to a remote address:
*       For UDP sockets, specifies the address to which datagrams are sent by
*                default, and the only address from which datagrams are received. For
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
GT_STATUS osSocketConnect(IN  GT_SOCKET_FD     socketFd ,
                          IN  GT_VOID *        sockAddr,
                          IN  GT_SIZE_T        sockAddrLen)
{
    int ret;

    /* address should be this computer or loop back 127.0.0.1 */
    ret =  connect ((SOCKET) socketFd,(struct sockaddr*) sockAddr,
                                        (int)sockAddrLen);

    if (ret == 0)
        return(GT_OK);
    else
    {
        ret = WSAGetLastError ();
        return GT_FAIL ;
    }
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
GT_STATUS osSocketSetNonBlock(IN  GT_SOCKET_FD  socketFd)
{
    u_long temp = 1;
    int ret;

    ret = ioctlsocket( (SOCKET) socketFd,FIONBIO, &temp );

    if (ret == 0)
       return(GT_OK);

    ret = WSAGetLastError ();

    return (GT_OK);
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
GT_STATUS osSocketSetBlock(IN  GT_SOCKET_FD socketFd)
{
    u_long temp = 0;
    int ret;

    ret = ioctlsocket( (SOCKET) socketFd, FIONBIO, &temp );

    if (ret == 0)
       return(GT_OK);

    ret = WSAGetLastError ();

    return (GT_OK);

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
GT_SSIZE_T osSocketSend(IN  GT_SOCKET_FD   socketFd ,
                     IN  const GT_VOID *buff,
                     IN  GT_SIZE_T      buffLen)
{
    int ret;
    int sizeSent;

    sizeSent = send ((SOCKET)socketFd, buff, (int)buffLen, 0);
    if (sizeSent == SOCKET_ERROR)
    {
        ret = WSAGetLastError ();
    }

    return (sizeSent);
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
GT_SSIZE_T osSocketSendTo( IN  GT_SOCKET_FD   socketFd ,
                        IN  const GT_VOID *buff,
                        IN  GT_SIZE_T      buffLen,
                        IN  GT_VOID *      sockAddr,
                        IN  GT_SIZE_T      sockAddrLen)
{
    int ret;
    int sizeSent;

    sizeSent = sendto ((SOCKET)socketFd, buff, (int)buffLen, 0, (struct sockaddr*)sockAddr,
                                           (int)sockAddrLen);
    if (sizeSent == SOCKET_ERROR)
        ret = WSAGetLastError ();

    return (sizeSent);
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
GT_SSIZE_T osSocketRecv(IN  GT_SOCKET_FD   socketFd,
                     IN  GT_VOID *      buff,
                     IN  GT_SIZE_T      buffLen,
                     IN  GT_BOOL        removeFlag)
{
    int flags = 0 ;
    int ret;
    int recvSize;

    if (removeFlag==GT_FALSE)
        flags = MSG_PEEK ;

    recvSize = recv ((SOCKET)socketFd, buff, (int)buffLen, flags);
     if (recvSize == SOCKET_ERROR)
     {
         ret = WSAGetLastError ();
         if(ret == WSAEWOULDBLOCK )
             return 0;
     }
     
     return ( (GT_SSIZE_T) recvSize);
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
GT_SSIZE_T osSocketRecvFrom( IN  GT_SOCKET_FD   socketFd ,
                          IN  GT_VOID *      buff,
                          IN  GT_SIZE_T      buffLen,
                          IN  GT_BOOL        removeFlag,
                          OUT GT_VOID *      sockAddr,
                          OUT GT_SIZE_T *    sockAddrLen)
{
    int flags = 0 ;
    int ret;
    int recvSize;
    int fromLen = 0;

    if (sockAddrLen != NULL)
        fromLen = (int)(*sockAddrLen);


    if (removeFlag==GT_FALSE)
        flags = MSG_PEEK ;


    recvSize = recvfrom ((SOCKET)socketFd, buff, (int)buffLen, flags,
                         (struct sockaddr*)sockAddr,
                         sockAddrLen ? &fromLen : NULL);

    if (recvSize == SOCKET_ERROR)
            ret = WSAGetLastError ();

    if (sockAddrLen != NULL)
        *sockAddrLen = (GT_SIZE_T)fromLen;

    return ( (GT_SSIZE_T) recvSize);

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
    SOCKET_LINGER        sockLinger;
    int                  ret,err;
    WSADATA              ptr;

    sockLinger.lOnOff = 1; /* disable the linger option */
    sockLinger.lLinger = 0;

    ret = WSAStartup (0x0202, &ptr);
    if (ret != 0)
            return (INVALID_SOCKET);

    ret = setsockopt (socketFd, SOL_SOCKET, SO_LINGER, (GT_CHAR*)&sockLinger,
                    sizeof (sockLinger));
    if(ret != 0)
    {
        err = WSAGetLastError();
        return GT_FAIL;
    }

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
*        independent of the other direction.
*       Even though shutdown will eventually terminate the TCP connection,
*        it does not close the socket or frees its resources.
*        In order to close the socket and free its resources only close can be used.
*
* INPUTS:
*       socketFd - File descriptor
*        SOCKET_SHUTDOWN_OPERATION -
*                    SOCKET_SHUTDOWN_CLOSE_INPUT,
*                    SOCKET_SHUTDOWN_CLOSE_OUTPUT,
*                    SOCKET_SHUTDOWN_CLOSE_ALL
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
  IN GT_SOCKET_FD                       socketFd ,
  IN SOCKET_SHUTDOWN_OPERATION         operation
)
{
    int how = SD_BOTH;

    switch (operation)
    {
        case SOCKET_SHUTDOWN_CLOSE_INPUT :
            how = SD_RECEIVE;
            break;
        case SOCKET_SHUTDOWN_CLOSE_OUTPUT :
            how = SD_SEND;
            break;
        case SOCKET_SHUTDOWN_CLOSE_ALL :
            how = SD_BOTH;
            break;
        default :
            break;
    }

    if (shutdown (socketFd, how)== ERROR)
    {
        int ret = WSAGetLastError();
        int error = 0;

        switch(ret)
        {
            case WSANOTINITIALISED: /*A successful WSAStartup must occur before using this function.*/
                error = 1;
                break;
            case WSAENETDOWN: /*The network subsystem has failed.*/
                error = 2;
                break;
            case WSAEINVAL: /*The how parameter is not valid, or is not consistent with the socket type.
                              For example, SD_SEND is used with a UNI_RECV socket type.*/
                error = 3;
                break;
            case WSAEINPROGRESS: /*A blocking Windows Sockets 1.1 call is in progress,
                            or the service provider is still processing a callback function.*/
                error = 4;
                break;
            case WSAENOTCONN: /*The socket is not connected (connection-oriented sockets only).*/
                error = 5;
                break;
            case WSAENOTSOCK: /*The descriptor is not a socket.*/
                error = 6;
                break;
            default:
                error = ret;
                break;
        }

        if(ret != 0 )
        {
            printf("osSocketShutDown: failed WSAGetLastError[%d] error[%d]\n",ret,error);
        }


        return GT_FAIL;
    }

    return GT_OK;
}
