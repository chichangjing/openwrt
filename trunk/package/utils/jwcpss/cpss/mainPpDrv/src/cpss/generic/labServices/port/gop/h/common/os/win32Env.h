#ifndef __win32Env_h__
#define __win32Env_h__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <windows.h>
#include <process.h>
#include <time.h>
#include <winsock2.h>

#define hwsMalloc               malloc
#define hwsFree                 free
#define hwsRealloc              realloc

#define hwsMemCmp               memcmp
#define hwsMemCpy               memcpy
#define hwsMemSet               memset
#define hwsMemMove              memmove

#define hwsStrLen               strlen
#define hwsStrNCpy              strncpy
#define hwsStrNCmp              strncmp
#define hwsStrrChr              strrchr
#define hwsStrtol               strtol
#define hwsStrtoul              strtoul
#define hwsStrStr               strstr

#define hwsDelayMs(ms)          Sleep(ms)
#define hwsTime                 time
inline size_t hwsStrfTime(char *buf, size_t bufSiz, const char *fmt, time_t tim)
{
    struct tm tm_val;
    return strftime(buff, buffSiz, fmt, localtime_r(&tim, &tm_val));
}

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
inline long hwsFgetLength(FILE *streamP)
{
    long curr, length;
    curr = ftell(streamP);
    fseek(streamP, 0, SEEK_END);
    length = ftell(streamP);
    fseek(streamP, curr, SEEK_SET);
    return length;
}
#define hwsFlastErrorStr()      strerror(errNo)


#define hwsSocketTcpCreate(_s)  socket(AF_INET, SOCK_STREAM, 0)
#define hwsSocketUdpCreate(_s)  socket(AF_INET, SOCK_DGRAM, 0)
#define hwsSocketTcpDestroy     closesocket
#define hwsSocketUdpDestroy     closesocket
/*TODO:extern*/
inline int hwsSocketCreateAddr(const char *ipAddr, unsigned ipPort, void **addrP, int *addrLen)
{
    struct sockaddr_in *a = malloc(sizeof(*a));
    if (!a)
        return 1;
    *addrP = (void*)a;
    *addrLen = sizeof(*a);
    memset(a, 0, sizeof(*a));
    a->sin_family = AF_INET;
    a->sin_port = htons(ipPort);
    if (ipAddr == NULL)
    {
        a->sin_addr.s_addr = INADDR_ANY;
        return 0;
    }
    if (inet_aton(ipAddr, &(a->sin_addr)) == 0)
    {
        free(a);
        return 1;
    }
    return 0;
}
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
const char * hwsSocketLastErrorStr(void); /*TODO*/

#endif /* __posixEnv_h__ */
