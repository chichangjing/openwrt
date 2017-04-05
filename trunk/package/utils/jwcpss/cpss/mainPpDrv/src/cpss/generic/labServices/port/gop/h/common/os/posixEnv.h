#ifndef __posixEnv_h__
#define __posixEnv_h__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

#define hwsMalloc               malloc
#define hwsFree                 free
#define hwsRealloc              realloc

#define hwsMemCmp               memcmp
#define hwsMemCpy               memcpy
#define hwsMemSet               memset
#define hwsMemMove              memmove

#define hwsStrLen               strlen
#define hwsStrCpy               strcpy
#define hwsStrChr               strchr
#define hwsStrCmp               strcmp
#define hwsStrNCpy              strncpy
#define hwsStrNCmp              strncmp
#define hwsStrrChr              strrchr
#define hwsStrtol               strtol
#define hwsStrtoul              strtoul
#define hwsStrStr               strstr

#define hwsDelayMs(ms)          usleep(ms*1000)
#define hwsTime                 time
size_t hwsStrfTime(char *buf, size_t bufSiz, const char *fmt, time_t tim);
void hwsTimeRT(GT_U32 *seconds, GT_U32 *nanoSeconds);

#define hwsSprintf              sprintf
#define hwsVsprintf             vsprintf
#define hwsPrintf               printf
#define hwsSnprintf             snprintf
#define hwsVsnprintf            vsnprintf

typedef FILE * HWS_FILEP;
#define HWS_FILEP_INVALID       NULL
#define HWS_STDOUT              stdout
#define HWS_STDERR              stderr
#define hwsFopen                fopen
#define hwsFclose               fclose
#define hwsFprintf              fprintf
#define hwsFgets                fgets
#define hwsFwrite               fwrite
#define hwsFread                fread
#define hwsRewind               rewind
#define hwsFgetLength           cpssOsFgetLength
long hwsFgetLength(FILE *streamP);
#define hwsFlastErrorStr()      strerror(errNo)


#define hwsSocketTcpCreate(_s)  socket(AF_INET, SOCK_STREAM, 0)
#define hwsSocketUdpCreate(_s)  socket(AF_INET, SOCK_DGRAM, 0)
#define hwsSocketTcpDestroy     close
#define hwsSocketUdpDestroy     close
int hwsSocketCreateAddr(const char *ipAddr, unsigned ipPort, void **addrP, int *addrLen);
#define hwsSocketDestroyAddr(_addr) free(_addr)
#define hwsSocketBind(_fd,_addrp,_addrLen) \
                                bind((int)_fd, (struct sockaddr *)_addrp, _addrLen)
#define hwsSocketListen(_fd,_maxconn) \
                                listen(_fd, (int)_maxconn)
#define hwsSocketAccept(_fd,_addrp,_addrLenP) \
                                accept(_fd, (struct sockaddr*)_addrp, _addrLenP)
#define hwsSocketConnect(_fd,_addrp,_addrLen) \
                                connect(_fd, (struct sockaddr *)_addrp, _addrLen)
#define hwsSocketSend(_fd,_buff,_len) \
                                send(_fd, _buff, _len, 0)
#define hwsSocketRecv(_fd,_buff,_len,_rmFlg) \
                                recv(_fd, _buff, _len, 0)
#define hwsSocketLastError()    (errno)
#define hwsSocketLastErrorStr() strerror(errno)
#define HWS_SOCKET_EAGAIN       EAGAIN
#define HWS_SOCKET_EINTR        EINTR
#define HWS_SOCKET_EINVAL       EINVAL
#define HWS_SOCKET_EMSGSIZE     EMSGSIZE
#define HWS_SOCKET_EISCONN      EISCONN
#define HWS_SOCKET_ESHUTDOWN    ESHUTDOWN
#define HWS_SOCKET_EWOULDBLOCK  EWOULDBLOCK
#define HWS_SOCKET_EINPROGRESS  EINPROGRESS
#define HWS_SOCKET_EALREADY     EALREADY
#define HWS_SOCKET_ECONNRESET   ECONNRESET
#define HWS_SOCKET_ECONNREFUSED ECONNREFUSED
#define HWS_SOCKET_EBADF        EBADF
#define HWS_SOCKET_ENETUNREACH  ENETUNREACH
#define HWS_SOCKET_EHOSTUNREACH EHOSTUNREACH

/*TODO*/

#define HWS_TASK_ID             pthread_t
int hwsTaskCreate(
    const char *name,
    int prio,
    size_t stack,
    unsigned (*start_addr)(GT_VOID*),
    void* args,
    HWS_TASK_ID *tid
);

GT_STATUS hwsI2cInitDrv(void);
GT_STATUS hwsI2cWriteReg(GT_U32 devSlvId, GT_U32 regAddr, GT_U32 value);
GT_STATUS hwsI2cReadReg((GT_U32  devSlvId, GT_U32  regAddr, GT_U32 *dataPtr);


#endif /* __posixEnv_h__ */
