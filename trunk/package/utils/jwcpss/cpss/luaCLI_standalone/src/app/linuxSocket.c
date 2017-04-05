#define _BSD_SOURCE
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

#include <cmdShell/common/cmdCommon.h>

#define GT_INFINITE 0xffffffff

static GT_SOCKET_FD _createTcpSock(GT_32 sockSize)
{
    GT_SOCKET_FD sock;
    int          optval = 1;
    
    sock = socket(AF_INET, SOCK_STREAM, 0) ;
    
    if (sock < 0)
    {
        return GT_ERROR;
    }
    
    /* if REUSEADDR (linux doesn't support SO_REUSEPORT) not defined and program closed and opend 
     * during short time OS returns error - "address already in use"
     */
    if(setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR, (GT_CHAR*) &optval, sizeof(optval)) != 0)
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


static GT_SOCKET_FD _createUdpSock (GT_32 sockSize)
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


static GT_STATUS _destroyTcp 
(
    GT_SOCKET_FD   socketFd 
)
{
    errno = 0 ;
    /* TBD - Do we need to call shutdown(sockeFd,2) ;*/
    if (close((int)socketFd)==0)
       return GT_OK ;
    else
       return GT_ERROR ;
}


static GT_STATUS _destroyUdp 
(
    IN  GT_SOCKET_FD   socketFd 
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
       return GT_OK ;
    else
       return GT_ERROR ;
}


static GT_STATUS _createAddr
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

    *sockAddr = (struct sockaddr_in*) calloc(1, sizeof(struct sockaddr_in));
    
    if (*sockAddr == NULL)
    {
        return GT_FAIL;
    }
    
    *sockAddrLen = sizeof(struct sockaddr_in) ;

    ((struct sockaddr_in*)(*sockAddr))->sin_family = AF_INET;    
    ((struct sockaddr_in*)(*sockAddr))->sin_port = htons((int)ipPort);

    if (ipAddr == NULL)
    {
        ((struct sockaddr_in*)(*sockAddr))->sin_addr.s_addr = INADDR_ANY;
        return GT_OK;
    }
    if (inet_aton(ipAddr, &(((struct sockaddr_in*)(*sockAddr))->sin_addr)) == 0)
    {
        free(sockAddr);
        return GT_FAIL ;
    }

    return GT_OK;
}
static GT_VOID _destroyAddr
(
    IN  GT_VOID *       sockAddr
)
{
    free(sockAddr);
}

static GT_STATUS _bindSock
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
       return GT_FAIL ;
}

static GT_STATUS _connectSock
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

static GT_SSIZE_T _socketSend
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
static GT_SSIZE_T _socketSendTo
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

static GT_SSIZE_T _socketRecv
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

static GT_SSIZE_T _socketRecvFrom
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
    if (removeFlag==GT_FALSE)
        flags = MSG_PEEK ;
  
    return (GT_SSIZE_T) recvfrom((int)socketFd,             /* Socket FD                */
                              buff,                      /* Buffer to receive        */
                              (size_t)buffLen,           /* Max length to received   */
                              flags,                     /* Flags                    */
                              (struct sockaddr*)sockAddr,/* Socket address recv from */
                              (socklen_t*)sockAddrLen);  /* Size of socket address   */ 
}

static GT_VOID * _selectCreateSet
(
)
{
    fd_set * set ;

    set = (fd_set*) malloc(sizeof(fd_set));
    if (set == NULL) 
        return NULL ;

    FD_ZERO(set) ;    
  
    return (GT_VOID*) set ;

}

static GT_VOID _selectEraseSet 
(
    GT_VOID *  set  
)
{
    free(set);
}

static GT_VOID _selectZeroSet 
(
    GT_VOID *  set  
)
{
    FD_ZERO((fd_set *)set) ;
}

static GT_VOID _selectAddFdToSet
(
    GT_VOID *  set ,
    GT_FD      fd   
)
{
    FD_SET((int)fd, (fd_set*)set) ;
}

static GT_BOOL _selectIsFdSet
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

static GT_SSIZE_T _select
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
    {
        return (GT_SSIZE_T)(select(lwidth ,  
               (fd_set*)readSet ,
               (fd_set*)writeSet ,
               (fd_set*)exceptionSet ,
               NULL)) ;
    }
    else 
    {
        /* Convert to seconds and calculate the reminder in microseconds */
        tv.tv_sec = timeOut/1000 ; 
        tv.tv_usec = (timeOut - (tv.tv_sec*1000)) * 1000; 

        return (GT_SSIZE_T)(select(lwidth ,  
                        (fd_set*)readSet ,
                        (fd_set*)writeSet ,
                        (fd_set*)exceptionSet ,
                        &tv)) ;
    }
    /* 
    ** Note that now FD_ISSET on the file descriptor with its 
    ** corresponded set will be true if the file descriptor is
    ** ready 
    */
}


CMD_OS_SOCKET_TCP_CREATE_FUN    cmdOsSocketTcpCreate = _createTcpSock;
CMD_OS_SOCKET_UDP_CREATE_FUN    cmdOsSocketUdpCreate = _createUdpSock;
CMD_OS_SOCKET_TCP_DESTROY_FUN   cmdOsSocketTcpDestroy = _destroyTcp;
CMD_OS_SOCKET_UDP_DESTROY_FUN   cmdOsSocketUdpDestroy = _destroyUdp;
CMD_OS_SOCKET_CREATE_ADDR_FUN   cmdOsSocketCreateAddr = _createAddr;
CMD_OS_SOCKET_DESTROY_ADDR_FUN  cmdOsSocketDestroyAddr = _destroyAddr;
CMD_OS_SOCKET_BIND_FUN          cmdOsSocketBind = _bindSock;
CMD_OS_SOCKET_CONNECT_FUN       cmdOsSocketConnect = _connectSock;
CMD_OS_SOCKET_SEND_FUN          cmdOsSocketSend = _socketSend;
CMD_OS_SOCKET_SEND_TO_FUN       cmdOsSocketSendTo = _socketSendTo;
CMD_OS_SOCKET_RECV_FUN          cmdOsSocketRecv = _socketRecv;
CMD_OS_SOCKET_RECV_FROM_FUN     cmdOsSocketRecvFrom = _socketRecvFrom;

CMD_OS_SELECT_CREATE_SET_FUN    cmdOsSelectCreateSet = _selectCreateSet;
CMD_OS_SELECT_ERASE_SET_FUN     cmdOsSelectEraseSet = _selectEraseSet;
CMD_OS_SELECT_ZERO_SET_FUN      cmdOsSelectZeroSet = _selectZeroSet;
CMD_OS_SELECT_ADD_FD_TO_SET_FUN cmdOsSelectAddFdToSet = _selectAddFdToSet;
CMD_OS_SELECT_IS_FD_SET_FUN     cmdOsSelectIsFdSet = _selectIsFdSet;
CMD_OS_SELECT_FUN               cmdOsSelect = _select;
