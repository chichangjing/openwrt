/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssBindFunc.h
*
* DESCRIPTION:
*       external functions wrapper. definitions for bind OS,extDrv,trace
*       dependent utilities to CPSS .
*
*       private CPSS file
*
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*******************************************************************************/

#ifndef __prvCpssBindFunch
#define __prvCpssBindFunch

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/cpssExtServices.h>

/* macro to free allocation of pointer only in not NULL , and set it NULL */
#define FREE_PTR_MAC(ptr) if(ptr)cpssOsFree(ptr); ptr = NULL

/************* global *********************************************************/

extern CPSS_EXT_DRV_MGMT_CACHE_FLUSH_FUNC      cpssExtDrvMgmtCacheFlush;
extern CPSS_EXT_DRV_MGMT_CACHE_INVALIDATE_FUNC cpssExtDrvMgmtCacheInvalidate;

extern CPSS_EXTDRV_DMA_WRITE_FUNC  cpssExtDrvDmaWrite;
extern CPSS_EXTDRV_DMA_READ_FUNC   cpssExtDrvDmaRead;

extern CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC       cpssExtDrvEthPortRxInit;
extern CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_SET_FUNC       cpssExtDrvEthRawSocketModeSet;
extern CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_GET_FUNC       cpssExtDrvEthRawSocketModeGet;
extern CPSS_EXTDRV_LINUX_MODE_SET_FUNC       cpssExtDrvLinuxModeSet;
extern CPSS_EXTDRV_LINUX_MODE_GET_FUNC       cpssExtDrvLinuxModeGet;
extern CPSS_EXTDRV_HSU_MEM_BASE_ADDR_GET_FUNC       cpssExtDrvHsuMemBaseAddrGet;
extern CPSS_EXTDRV_HSU_WARM_RESTART_FUNC            cpssExtDrvHsuWarmRestart;
extern CPSS_EXTDRV_HSU_INBOUND_SDMA_ENABLE_FUNC     cpssExtDrvInboundSdmaEnable;
extern CPSS_EXTDRV_HSU_INBOUND_SDMA_DISABLE_FUNC    cpssExtDrvInboundSdmaDisable;
extern CPSS_EXTDRV_HSU_INBOUND_SDMA_STATE_GET_FUNC  cpssExtDrvInboundSdmaStateGet;
extern CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC       cpssExtDrvEthPortTxInit;
extern CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC        cpssExtDrvEthPortEnable;
extern CPSS_EXTDRV_ETH_PORT_DISABLE_FUNC       cpssExtDrvEthPortDisable;
extern CPSS_EXTDRV_ETH_PORT_TX_FUNC            cpssExtDrvEthPortTx;
extern CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC  cpssExtDrvEthInputHookAdd;
extern CPSS_EXTDRV_ETH_RAW_SOCKET_RX_HOOK_ADD_FUNC  cpssExtDrvEthRawSocketRxHookAdd;
extern CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC  cpssExtDrvEthTxCompleteHookAdd;
extern CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC  cpssExtDrvEthRxPacketFree;
extern CPSS_EXTDRV_ETH_PORT_TX_MODE_SET_FUNC    cpssExtDrvEthPortTxModeSet;
extern CPSS_EXTDRV_ETH_CPU_CODE_TO_QUEUE        cpssExtDrvEthCpuCodeToQueue;
extern CPSS_EXTDRV_ETH_CPU_PREPEND_TWO_BYTES_FUNC cpssExtDrvEthPrePendTwoBytesHeaderSet;

extern CPSS_EXTDRV_INT_CONNECT_FUNC        cpssExtDrvIntConnect;
extern CPSS_EXTDRV_INT_ENABLE_FUNC         cpssExtDrvIntEnable;
extern CPSS_EXTDRV_INT_DISABLE_FUNC        cpssExtDrvIntDisable;
extern CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC  cpssExtDrvSetIntLockUnlock;

extern CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC      cpssExtDrvPciConfigWriteReg;
extern CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC       cpssExtDrvPciConfigReadReg;
extern CPSS_EXTDRV_PCI_DEV_FIND_FUNC              cpssExtDrvPciFindDev;
extern CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC           cpssExtDrvGetPciIntVec;
extern CPSS_EXTDRV_INT_MASK_GET_FUNC              cpssExtDrvGetIntMask;
extern CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC cpssExtDrvEnableCombinedPciAccess;
extern CPSS_EXTDRV_PCI_DOUBLE_WRITE_FUNC          cpssExtDrvPciDoubleWrite;
extern CPSS_EXTDRV_PCI_DOUBLE_READ_FUNC           cpssExtDrvPciDoubleRead;

extern CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC             cpssExtDrvHwIfSmiInitDriver;
extern CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC               cpssExtDrvHwIfSmiWriteReg;
extern CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC                cpssExtDrvHwIfSmiReadReg;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC       cpssExtDrvHwIfSmiTskRegRamRead;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC      cpssExtDrvHwIfSmiTskRegRamWrite;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC       cpssExtDrvHwIfSmiTskRegVecRead;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC      cpssExtDrvHwIfSmiTskRegVecWrite;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC          cpssExtDrvHwIfSmiTaskWriteReg;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC           cpssExtDrvHwIfSmiTaskReadReg;
extern CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC     cpssExtDrvHwIfSmiInterruptWriteReg;
extern CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC      cpssExtDrvHwIfSmiInterruptReadReg;
extern CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC       cpssExtDrvSmiDevVendorIdGet;
extern CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC    cpssExtDrvHwIfSmiTaskWriteRegField;

extern CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC  cpssExtDrvHwIfTwsiInitDriver;
extern CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC    cpssExtDrvHwIfTwsiWriteReg;
extern CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC     cpssExtDrvHwIfTwsiReadReg;

extern CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC      cpssExtDrvI2cMgmtMasterInit;
extern CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC        cpssExtDrvMgmtReadRegister;
extern CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC       cpssExtDrvMgmtWriteRegister;
extern CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC    cpssExtDrvMgmtIsrReadRegister;
extern CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC   cpssExtDrvMgmtIsrWriteRegister;

extern CPSS_EXTDRV_DRAGONITE_GET_BASE_ADDRESS_FUNC cpssExtDrvDragoniteShMemBaseAddrGet;

extern CPSS_OS_INET_NTOHL_FUNC  cpssOsNtohl;
extern CPSS_OS_INET_HTONL_FUNC  cpssOsHtonl;
extern CPSS_OS_INET_NTOHS_FUNC  cpssOsNtohs;
extern CPSS_OS_INET_HTONS_FUNC  cpssOsHtons;
extern CPSS_OS_INET_NTOA_FUNC   cpssOsInetNtoa;

extern CPSS_OS_INT_ENABLE_FUNC    cpssOsIntEnable;
extern CPSS_OS_INT_DISABLE_FUNC   cpssOsIntDisable;
extern CPSS_OS_INT_MODE_SET_FUNC  cpssOsSetIntLockUnlock;
extern CPSS_OS_INT_CONNECT_FUNC   cpssOsInterruptConnect;

extern CPSS_OS_IO_BIND_STDOUT_FUNC cpssOsBindStdOut;
extern CPSS_OS_IO_PRINTF_FUNC      cpssOsPrintf;
extern CPSS_OS_IO_VPRINTF_FUNC     cpssOsVprintf;
extern CPSS_OS_IO_SPRINTF_FUNC     cpssOsSprintf;
extern CPSS_OS_IO_VSPRINTF_FUNC    cpssOsVsprintf;
extern CPSS_OS_IO_SNPRINTF_FUNC    cpssOsSnprintf;
extern CPSS_OS_IO_VSNPRINTF_FUNC   cpssOsVsnprintf;
extern CPSS_OS_IO_PRINT_SYNC_FUNC  cpssOsPrintSync;
extern CPSS_OS_IO_GETS_FUNC        cpssOsGets;

extern CPSS_OS_IO_F_LAST_ERROR_STR_FUNC cpssOsFlastErrorStr;
extern CPSS_OS_IO_FOPEN_FUNC       cpssOsFopen;
extern CPSS_OS_IO_FCLOSE_FUNC      cpssOsFclose;
extern CPSS_OS_IO_REWIND_FUNC      cpssOsRewind;
extern CPSS_OS_IO_FPRINTF_FUNC     cpssOsFprintf;
extern CPSS_OS_IO_FGETS_FUNC       cpssOsFgets;
extern CPSS_OS_IO_FWRITE_FUNC      cpssOsFwrite;
extern CPSS_OS_IO_FREAD_FUNC       cpssOsFread;
extern CPSS_OS_IO_FGET_LENGTH_FUNC cpssOsFgetLength;

extern CPSS_OS_FATAL_ERROR_FUNC    cpssOsFatalError;

extern CPSS_OS_BZERO_FUNC             cpssOsBzero;
extern CPSS_OS_MEM_SET_FUNC           cpssOsMemSet;
extern CPSS_OS_MEM_CPY_FUNC           cpssOsMemCpy;
extern CPSS_OS_MEM_MOVE_FUNC          cpssOsMemMove;
extern CPSS_OS_MEM_CMP_FUNC           cpssOsMemCmp;
/*extern CPSS_OS_STATIC_MALLOC_FUNC     cpssOsStaticMalloc; -- not to be used in CPSS !!*/
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    extern CPSS_OS_MALLOC_FUNC            cpssOsMalloc_MemoryLeakageDbg;
    extern CPSS_OS_REALLOC_FUNC           cpssOsRealloc_MemoryLeakageDbg;
    extern CPSS_OS_FREE_FUNC              cpssOsFree_MemoryLeakageDbg;
    #define cpssOsMalloc(_size)           cpssOsMalloc_MemoryLeakageDbg(_size           ,__FILE__,__LINE__)
    #define cpssOsRealloc(_ptr,_size)     cpssOsRealloc_MemoryLeakageDbg(_ptr,_size     ,__FILE__,__LINE__)
    #define cpssOsFree(_memblock)         cpssOsFree_MemoryLeakageDbg(_memblock         ,__FILE__,__LINE__)
#else  /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    extern CPSS_OS_MALLOC_FUNC            cpssOsMalloc;
    extern CPSS_OS_REALLOC_FUNC           cpssOsRealloc;
    extern CPSS_OS_FREE_FUNC              cpssOsFree;
#endif /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/

extern CPSS_OS_CACHE_DMA_MALLOC_FUNC  cpssOsCacheDmaMalloc;
extern CPSS_OS_CACHE_DMA_FREE_FUNC    cpssOsCacheDmaFree;
extern CPSS_OS_PHY_TO_VIRT_FUNC       cpssOsPhy2Virt;
extern CPSS_OS_VIRT_TO_PHY_FUNC       cpssOsVirt2Phy;

extern CPSS_OS_RAND_FUNC  cpssOsRand;
extern CPSS_OS_SRAND_FUNC cpssOsSrand;

extern CPSS_OS_MUTEX_CREATE_FUNC       cpssOsMutexCreate;
extern CPSS_OS_MUTEX_DELETE_FUNC       cpssOsMutexDelete;
extern CPSS_OS_MUTEX_LOCK_FUNC         cpssOsMutexLock;
extern CPSS_OS_MUTEX_UNLOCK_FUNC       cpssOsMutexUnlock;

extern CPSS_OS_SIG_SEM_BIN_CREATE_FUNC  cpssOsSigSemBinCreate;
extern CPSS_OS_SIG_SEM_M_CREATE_FUNC    cpssOsSigSemMCreate;
extern CPSS_OS_SIG_SEM_C_CREATE_FUNC    cpssOsSigSemCCreate;
extern CPSS_OS_SIG_SEM_DELETE_FUNC      cpssOsSigSemDelete;
extern CPSS_OS_SIG_SEM_WAIT_FUNC        cpssOsSigSemWait;
extern CPSS_OS_SIG_SEM_SIGNAL_FUNC      cpssOsSigSemSignal;

extern CPSS_OS_STR_LEN_FUNC     cpssOsStrlen;
extern CPSS_OS_STR_CPY_FUNC     cpssOsStrCpy;
extern CPSS_OS_STR_N_CPY_FUNC   cpssOsStrNCpy;
extern CPSS_OS_STR_CHR_FUNC     cpssOsStrChr;
extern CPSS_OS_STR_STR_FUNC     cpssOsStrStr;
extern CPSS_OS_STR_REV_CHR_FUNC cpssOsStrRChr;
extern CPSS_OS_STR_CMP_FUNC     cpssOsStrCmp;
extern CPSS_OS_STR_N_CMP_FUNC   cpssOsStrNCmp;
extern CPSS_OS_STR_CAT_FUNC     cpssOsStrCat;
extern CPSS_OS_STR_N_CAT_FUNC   cpssOsStrNCat;
extern CPSS_OS_TO_UPPER_FUNC    cpssOsToUpper;
extern CPSS_OS_STR_TO_32_FUNC   cpssOsStrTo32;
extern CPSS_OS_STR_TO_U32_FUNC  cpssOsStrToU32;

extern CPSS_OS_TIME_WK_AFTER_FUNC  cpssOsTimerWkAfter;
extern CPSS_OS_TIME_TICK_GET_FUNC  cpssOsTickGet;
extern CPSS_OS_TIME_GET_FUNC       cpssOsTime;
extern CPSS_OS_TIME_RT_FUNC        cpssOsTimeRT;
extern CPSS_OS_GET_SYS_CLOCK_RATE_FUNC cpssOsGetSysClockRate;
extern CPSS_OS_DELAY_FUNC          cpssOsDelay;
extern CPSS_OS_STRF_TIME_FUNC      cpssOsStrfTime;
extern CPSS_OS_TASK_CREATE_FUNC    cpssOsTaskCreate;
extern CPSS_OS_TASK_DELETE_FUNC    cpssOsTaskDelete;
extern CPSS_OS_TASK_GET_SELF_FUNC  cpssOsTaskGetSelf;
extern CPSS_OS_TASK_LOCK_FUNC      cpssOsTaskLock;
extern CPSS_OS_TASK_UNLOCK_FUNC    cpssOsTaskUnLock;

extern CPSS_OS_STDLIB_QSORT_FUNC   cpssOsQsort;
extern CPSS_OS_STDLIB_BSEARCH_FUNC cpssOsBsearch;

extern CPSS_OS_MSGQ_CREATE_FUNC    cpssOsMsgQCreate;
extern CPSS_OS_MSGQ_DELETE_FUNC    cpssOsMsgQDelete;
extern CPSS_OS_MSGQ_SEND_FUNC      cpssOsMsgQSend;
extern CPSS_OS_MSGQ_RECV_FUNC      cpssOsMsgQRecv;
extern CPSS_OS_MSGQ_NUM_MSGS_FUNC  cpssOsMsgQNumMsgs;
extern CPSS_OS_LOG_FUNC            cpssOsLog;
extern CPSS_TRACE_HW_ACCESS_WRITE_FUNC  cpssTraceHwAccessWrite;
extern CPSS_TRACE_HW_ACCESS_READ_FUNC   cpssTraceHwAccessRead;
extern CPSS_TRACE_HW_ACCESS_DELAY_FUNC   cpssTraceHwAccessDelay;

extern CPSS_SOCKET_LAST_ERROR_FUNC           cpssOsSocketLastError;
extern CPSS_SOCKET_LAST_ERROR_STR_FUNC       cpssOsSocketLastErrorStr;
extern CPSS_SOCKET_TCP_CREATE_FUNC           cpssOsSocketTcpCreate;
extern CPSS_SOCKET_UDP_CREATE_FUNC           cpssOsSocketUdpCreate;
extern CPSS_SOCKET_TCP_DESTROY_FUNC          cpssOsSocketTcpDestroy;
extern CPSS_SOCKET_UDP_DESTROY_FUNC          cpssOsSocketUdpDestroy;
extern CPSS_SOCKET_CREATE_ADDR_FUNC          cpssOsSocketCreateAddr;
extern CPSS_SOCKET_DESTROY_ADDR_FUNC         cpssOsSocketDestroyAddr;
extern CPSS_SOCKET_BIND_FUNC                 cpssOsSocketBind;
extern CPSS_SOCKET_LISTEN_FUNC               cpssOsSocketListen;
extern CPSS_SOCKET_ACCEPT_FUNC               cpssOsSocketAccept;
extern CPSS_SOCKET_CONNECT_FUNC              cpssOsSocketConnect;
extern CPSS_SOCKET_SET_NONBLOCK_FUNC         cpssOsSocketSetNonBlock;
extern CPSS_SOCKET_SET_BLOCK_FUNC            cpssOsSocketSetBlock;
extern CPSS_SOCKET_SEND_FUNC                 cpssOsSocketSend;
extern CPSS_SOCKET_SENDTO_FUNC               cpssOsSocketSendTo;
extern CPSS_SOCKET_RECV_FUNC                 cpssOsSocketRecv;
extern CPSS_SOCKET_RECVFROM_FUNC             cpssOsSocketRecvFrom;
extern CPSS_SOCKET_SET_SOCKET_NOLINGER_FUNC  cpssOsSocketSetSocketNoLinger;
extern CPSS_SOCKET_EXTRACT_IPADDR_FROM_SOCKET_ADDR_FUNC cpssOsSocketExtractIpAddrFromSocketAddr;
extern CPSS_SOCKET_GET_ADDR_SIZE_FUNC        cpssOsSocketGetSocketAddrSize;
extern CPSS_SOCKET_SHUTDOWN_FUNC             cpssOsSocketShutDown;

extern CPSS_SOCKET_SELECT_CREATE_SET_FUNC         cpssOsSelectCreateSet;
extern CPSS_SOCKET_SELECT_ERASE_SET_FUNC          cpssOsSelectEraseSet;
extern CPSS_SOCKET_SELECT_ZERO_SET_FUNC           cpssOsSelectZeroSet;
extern CPSS_SOCKET_SELECT_ADD_FD_TO_SET_FUNC      cpssOsSelectAddFdToSet;
extern CPSS_SOCKET_SELECT_CLEAR_FD_FROM_SET_FUNC  cpssOsSelectClearFdFromSet;
extern CPSS_SOCKET_SELECT_IS_FD_SET_FUNC          cpssOsSelectIsFdSet;
extern CPSS_SOCKET_SELECT_COPY_SET_FUNC           cpssOsSelectCopySet;
extern CPSS_SOCKET_SELECT_FUNC                    cpssOsSelect;
extern CPSS_SOCKET_SELECT_GET_SOCKET_FD_SIZE_FUNC cpssOsSocketGetSocketFdSetSize;

#ifdef __cplusplus
}
#endif

#endif  /* __prvCpssBindFunch */


