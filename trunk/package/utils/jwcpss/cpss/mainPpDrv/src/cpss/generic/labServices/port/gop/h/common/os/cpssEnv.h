#ifndef __cpssEnv_h__
#define __cpssEnv_h__

#define CPSS_BUILD

#include <cpss/extServices/private/prvCpssBindFunc.h>

#define hwsMalloc               cpssOsMalloc
#define hwsFree                 cpssOsFree
#define hwsRealloc              cpssOsRealloc

#define hwsMemCmp               cpssOsMemCmp
#define hwsMemCpy               cpssOsMemCpy
#define hwsMemSet               cpssOsMemSet
#define hwsMemMove              cpssOsMemMove

#define hwsStrLen               cpssOsStrlen
#define hwsStrCpy               cpssOsStrCpy
#define hwsStrChr               cpssOsStrChr
#define hwsStrCmp               cpssOsStrCmp
#define hwsStrNCpy              cpssOsStrNCpy
#define hwsStrNCmp              cpssOsStrNCmp
#define hwsStrrChr              cpssOsStrRChr
#define hwsStrtol               cpssOsStrTo32
#define hwsStrtoul              cpssOsStrToU32
#define hwsStrStr               cpssOsStrStr

#define HWS_MUTEX               CPSS_OS_MUTEX
#define hwsOsMutexCreate        cpssOsMutexCreate
#define hwsOsMutexDelete        cpssOsMutexDelete
#define hwsOsMutexLock          cpssOsMutexLock
#define hwsOsMutexUnlock        cpssOsMutexUnlock


#define hwsDelayMs              cpssOsTimerWkAfter
#define hwsTime                 cpssOsTime
#define hwsStrfTime             cpssOsStrfTime
#define hwsTimeRT               cpssOsTimeRT

#define hwsSprintf              cpssOsSprintf
#define hwsVsprintf             cpssOsVsprintf
#define hwsPrintf               cpssOsPrintf
#define hwsSnprintf             cpssOsSnprintf
#define hwsVsnprintf            cpssOsVsnprintf

#define HWS_FILEP               CPSS_OS_FILE
#define HWS_FILEP_INVALID       CPSS_OS_FILE_INVALID
#define HWS_STDOUT              CPSS_OS_FILE_STDOUT
#define HWS_STDERR              CPSS_OS_FILE_STDERR
#define hwsFopen                cpssOsFopen
#define hwsFclose               cpssOsFclose
#define hwsFprintf              cpssOsFprintf
#define hwsFgets                cpssOsFgets
#define hwsFwrite               cpssOsFwrite
#define hwsFread                cpssOsFread
#define hwsRewind               cpssOsRewind
#define hwsFgetLength           cpssOsFgetLength
#define hwsFlastErrorStr        cpssOsFlastErrorStr


#define hwsSocketTcpCreate      cpssOsSocketTcpCreate
#define hwsSocketUdpCreate      cpssOsSocketUdpCreate
#define hwsSocketTcpDestroy     cpssOsSocketTcpDestroy
#define hwsSocketUdpDestroy     cpssOsSocketUdpDestroy
#define hwsSocketCreateAddr     cpssOsSocketCreateAddr
#define hwsSocketDestroyAddr    cpssOsSocketDestroyAddr
#define hwsSocketBind           cpssOsSocketBind
#define hwsSocketListen         cpssOsSocketListen
#define hwsSocketAccept         cpssOsSocketAccept
#define hwsSocketConnect        cpssOsSocketConnect
#define hwsSocketSend           cpssOsSocketSend
#define hwsSocketRecv           cpssOsSocketRecv
#define hwsSocketLastError      cpssOsSocketLastError
#define hwsSocketLastErrorStr   cpssOsSocketLastErrorStr
#define HWS_SOCKET_EAGAIN       CPSS_SOCKET_EAGAIN
#define HWS_SOCKET_EINTR        CPSS_SOCKET_EINTR
#define HWS_SOCKET_EINVAL       CPSS_SOCKET_EINVAL
#define HWS_SOCKET_EMSGSIZE     CPSS_SOCKET_EMSGSIZE
#define HWS_SOCKET_EISCONN      CPSS_SOCKET_EISCONN
#define HWS_SOCKET_ESHUTDOWN    CPSS_SOCKET_ESHUTDOWN
#define HWS_SOCKET_EWOULDBLOCK  CPSS_SOCKET_EWOULDBLOCK
#define HWS_SOCKET_EINPROGRESS  CPSS_SOCKET_EINPROGRESS
#define HWS_SOCKET_EALREADY     CPSS_SOCKET_EALREADY
#define HWS_SOCKET_ECONNRESET   CPSS_SOCKET_ECONNRESET
#define HWS_SOCKET_ECONNREFUSED CPSS_SOCKET_ECONNREFUSED
#define HWS_SOCKET_EBADF        CPSS_SOCKET_EBADF
#define HWS_SOCKET_ENETUNREACH  CPSS_SOCKET_ENETUNREACH
#define HWS_SOCKET_EHOSTUNREACH CPSS_SOCKET_EHOSTUNREACH



#define HWS_TASK_ID             CPSS_TASK
#define hwsTaskCreate           cpssOsTaskCreate

#define hwsI2cInitDrv           cpssExtDrvHwIfTwsiInitDriver
#define hwsI2cWriteReg          cpssExtDrvHwIfTwsiWriteReg
#define hwsI2cReadReg           cpssExtDrvHwIfTwsiReadReg

#endif /* __cpssEnv_h__ */
