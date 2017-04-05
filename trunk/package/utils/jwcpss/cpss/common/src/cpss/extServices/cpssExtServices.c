/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExtServices.c
*
* DESCRIPTION:
*       External Driver wrapper. definitions for bind OS , external driver
*       dependent services and trace services to CPSS .
*
*
* FILE REVISION NUMBER:
*       $Revision:  24 $
*******************************************************************************/

/************* Includes *******************************************************/

#include <cpss/extServices/cpssExtServices.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpssCommon/cpssPresteraDefs.h>

#define CPSS_NOT_IMPLEMENTED_EXTENDED_DEBUG

/* macro to force casting between 2 functions prototypes */
#define FORCE_FUNC_CAST     (void *)

#ifndef CPSS_NOT_IMPLEMENTED_EXTENDED_DEBUG
/* define a STUB function that "do nothing" but return "not implemented" */
/* the function parameters do not matter , since the function is "forced to
   do casting"
*/

#define STR_NOT_IMPLEMENTED_CNS " extServiceFuncNotImplementedCalled \n"
static GT_STATUS extServiceFuncNotImplementedCalled
(
    void
)
{
    if(cpssOsPrintf != (CPSS_OS_IO_PRINTF_FUNC)extServiceFuncNotImplementedCalled)
    {
        /* we already have "printf" from the application
          but current pointer of a function was not initialized yet */
        cpssOsPrintf(STR_NOT_IMPLEMENTED_CNS);
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
}
/* STUB function that "do nothing" but return 0.
 * Applicable for malloc, realloc, etc
 */
static GT_UINTPTR extServiceFuncNotImplementedCalled0
(
    void
)
{
    if(cpssOsPrintf != (CPSS_OS_IO_PRINTF_FUNC)extServiceFuncNotImplementedCalled)
    {
        /* we already have "printf" from the application
          but current pointer of a function was not initialized yet */
        cpssOsPrintf(STR_NOT_IMPLEMENTED_CNS);
    }

    return 0;
}
#define DECLARE_FUNC(_name,_type) \
    _type _name = (_type)extServiceFuncNotImplementedCalled;
#define DECLARE_FUNC0(_name,_type) \
    _type _name = (_type)extServiceFuncNotImplementedCalled0;

#else /* defined CPSS_NOT_IMPLEMENTED_EXTENDED_DEBUG */
#define STR_NOT_IMPLEMENTED_CNS " extServiceFuncNotImplementedCalled: %s\n"
static GT_STATUS extServiceFuncNotImplementedCalled_cpssOsPrintf(void);
static void extServiceFuncNotImplementedCalledPrint(const char *name)
{
    if(cpssOsPrintf != (CPSS_OS_IO_PRINTF_FUNC)extServiceFuncNotImplementedCalled_cpssOsPrintf)
    {
        /* we already have "printf" from the application
          but current pointer of a function was not initialized yet */
        cpssOsPrintf(STR_NOT_IMPLEMENTED_CNS, name);
    }
}
#define DECLARE_FUNC(_name,_type) \
    static GT_STATUS extServiceFuncNotImplementedCalled_ ## _name(void) \
    { \
        extServiceFuncNotImplementedCalledPrint(#_name); \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG); \
    } \
    _type _name = (_type)extServiceFuncNotImplementedCalled_ ## _name;
#define DECLARE_FUNC0(_name,_type) \
    static GT_UINTPTR extServiceFuncNotImplementedCalled_ ## _name(void) \
    { \
        extServiceFuncNotImplementedCalledPrint(#_name); \
        return 0; \
    } \
    _type _name = (_type)extServiceFuncNotImplementedCalled_ ## _name;


#endif
/* macro to replace the content of pointer only if replaced with non-NULL pointer */
#define REPLACE_IF_NOT_NULL_MAC(_target,_source)   \
    _target = _source ? _source : _target
/************* Prototypes *****************************************************/

/* PRV_CPSS_BIND_FUNC_STC -
*    structure that hold the  functions needed be bound to cpss.
*/
typedef struct{
    CPSS_EXT_DRV_FUNC_BIND_STC   extDrv;
    CPSS_OS_FUNC_BIND_STC        os;
    CPSS_TRACE_FUNC_BIND_STC     trace;
}PRV_CPSS_BIND_FUNC_STC;

/* need to fill stub function */
static PRV_CPSS_BIND_FUNC_STC   prvCpssBindFuncs;

DECLARE_FUNC(cpssExtDrvMgmtCacheFlush, CPSS_EXT_DRV_MGMT_CACHE_FLUSH_FUNC);
DECLARE_FUNC(cpssExtDrvMgmtCacheInvalidate, CPSS_EXT_DRV_MGMT_CACHE_INVALIDATE_FUNC);

DECLARE_FUNC(cpssExtDrvDmaWrite, CPSS_EXTDRV_DMA_WRITE_FUNC);
DECLARE_FUNC(cpssExtDrvDmaRead, CPSS_EXTDRV_DMA_READ_FUNC);

DECLARE_FUNC(cpssExtDrvEthPortRxInit, CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC);
DECLARE_FUNC(cpssExtDrvEthRawSocketModeSet, CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_SET_FUNC);
DECLARE_FUNC(cpssExtDrvEthRawSocketModeGet, CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_GET_FUNC);
DECLARE_FUNC(cpssExtDrvLinuxModeSet, CPSS_EXTDRV_LINUX_MODE_SET_FUNC);
DECLARE_FUNC(cpssExtDrvLinuxModeGet, CPSS_EXTDRV_LINUX_MODE_GET_FUNC);
DECLARE_FUNC(cpssExtDrvHsuMemBaseAddrGet, CPSS_EXTDRV_HSU_MEM_BASE_ADDR_GET_FUNC);
DECLARE_FUNC(cpssExtDrvHsuWarmRestart, CPSS_EXTDRV_HSU_WARM_RESTART_FUNC);
DECLARE_FUNC(cpssExtDrvInboundSdmaEnable, CPSS_EXTDRV_HSU_INBOUND_SDMA_ENABLE_FUNC);
DECLARE_FUNC(cpssExtDrvInboundSdmaDisable, CPSS_EXTDRV_HSU_INBOUND_SDMA_DISABLE_FUNC);
DECLARE_FUNC(cpssExtDrvInboundSdmaStateGet, CPSS_EXTDRV_HSU_INBOUND_SDMA_STATE_GET_FUNC);
DECLARE_FUNC(cpssExtDrvEthPortTxInit, CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC);
DECLARE_FUNC(cpssExtDrvEthPortEnable, CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC);
DECLARE_FUNC(cpssExtDrvEthPortDisable, CPSS_EXTDRV_ETH_PORT_DISABLE_FUNC);
DECLARE_FUNC(cpssExtDrvEthPortTx, CPSS_EXTDRV_ETH_PORT_TX_FUNC);
DECLARE_FUNC(cpssExtDrvEthInputHookAdd, CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC);
DECLARE_FUNC(cpssExtDrvEthRawSocketRxHookAdd, CPSS_EXTDRV_ETH_RAW_SOCKET_RX_HOOK_ADD_FUNC);
DECLARE_FUNC(cpssExtDrvEthTxCompleteHookAdd, CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC);
DECLARE_FUNC(cpssExtDrvEthRxPacketFree, CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC);
DECLARE_FUNC(cpssExtDrvEthPortTxModeSet, CPSS_EXTDRV_ETH_PORT_TX_MODE_SET_FUNC);
DECLARE_FUNC(cpssExtDrvEthCpuCodeToQueue, CPSS_EXTDRV_ETH_CPU_CODE_TO_QUEUE);
DECLARE_FUNC(cpssExtDrvEthPrePendTwoBytesHeaderSet, CPSS_EXTDRV_ETH_CPU_PREPEND_TWO_BYTES_FUNC);

DECLARE_FUNC(cpssExtDrvIntConnect, CPSS_EXTDRV_INT_CONNECT_FUNC);
DECLARE_FUNC(cpssExtDrvIntEnable, CPSS_EXTDRV_INT_ENABLE_FUNC);
DECLARE_FUNC(cpssExtDrvIntDisable, CPSS_EXTDRV_INT_DISABLE_FUNC);
DECLARE_FUNC(cpssExtDrvSetIntLockUnlock, CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC);

DECLARE_FUNC(cpssExtDrvPciConfigWriteReg, CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC);
DECLARE_FUNC(cpssExtDrvPciConfigReadReg, CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC);
DECLARE_FUNC(cpssExtDrvPciFindDev, CPSS_EXTDRV_PCI_DEV_FIND_FUNC);
DECLARE_FUNC(cpssExtDrvGetPciIntVec, CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC);
DECLARE_FUNC(cpssExtDrvGetIntMask, CPSS_EXTDRV_INT_MASK_GET_FUNC);
DECLARE_FUNC(cpssExtDrvEnableCombinedPciAccess, CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC);
DECLARE_FUNC(cpssExtDrvPciDoubleWrite, CPSS_EXTDRV_PCI_DOUBLE_WRITE_FUNC);
DECLARE_FUNC(cpssExtDrvPciDoubleRead, CPSS_EXTDRV_PCI_DOUBLE_READ_FUNC);

DECLARE_FUNC(cpssExtDrvHwIfSmiInitDriver, CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiWriteReg, CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiReadReg, CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiTskRegRamRead, CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiTskRegRamWrite, CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiTskRegVecRead, CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiTskRegVecWrite, CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiTaskWriteReg, CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiTaskReadReg, CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiInterruptWriteReg, CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiInterruptReadReg, CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC);
DECLARE_FUNC(cpssExtDrvSmiDevVendorIdGet, CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfSmiTaskWriteRegField, CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC);

DECLARE_FUNC(cpssExtDrvHwIfTwsiInitDriver, CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfTwsiWriteReg, CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC);
DECLARE_FUNC(cpssExtDrvHwIfTwsiReadReg, CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC);

DECLARE_FUNC(cpssExtDrvI2cMgmtMasterInit, CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC);
DECLARE_FUNC(cpssExtDrvMgmtReadRegister, CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC);
DECLARE_FUNC(cpssExtDrvMgmtWriteRegister, CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC);
DECLARE_FUNC(cpssExtDrvMgmtIsrReadRegister, CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC);
DECLARE_FUNC(cpssExtDrvMgmtIsrWriteRegister, CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC);

DECLARE_FUNC(cpssExtDrvDragoniteShMemBaseAddrGet, CPSS_EXTDRV_DRAGONITE_GET_BASE_ADDRESS_FUNC);

DECLARE_FUNC(cpssOsNtohl, CPSS_OS_INET_NTOHL_FUNC);
DECLARE_FUNC(cpssOsHtonl, CPSS_OS_INET_HTONL_FUNC);
DECLARE_FUNC(cpssOsNtohs, CPSS_OS_INET_NTOHS_FUNC);
DECLARE_FUNC(cpssOsHtons, CPSS_OS_INET_HTONS_FUNC);
DECLARE_FUNC(cpssOsInetNtoa, CPSS_OS_INET_NTOA_FUNC);

DECLARE_FUNC(cpssOsIntEnable, CPSS_OS_INT_ENABLE_FUNC);
DECLARE_FUNC(cpssOsIntDisable, CPSS_OS_INT_DISABLE_FUNC);
DECLARE_FUNC(cpssOsSetIntLockUnlock, CPSS_OS_INT_MODE_SET_FUNC);
DECLARE_FUNC(cpssOsInterruptConnect, CPSS_OS_INT_CONNECT_FUNC);

DECLARE_FUNC(cpssOsBindStdOut, CPSS_OS_IO_BIND_STDOUT_FUNC);
DECLARE_FUNC(cpssOsPrintf, CPSS_OS_IO_PRINTF_FUNC);
DECLARE_FUNC(cpssOsVprintf, CPSS_OS_IO_VPRINTF_FUNC);
DECLARE_FUNC(cpssOsSprintf, CPSS_OS_IO_SPRINTF_FUNC);
DECLARE_FUNC(cpssOsVsprintf, CPSS_OS_IO_VSPRINTF_FUNC);
DECLARE_FUNC(cpssOsSnprintf, CPSS_OS_IO_SNPRINTF_FUNC);
DECLARE_FUNC(cpssOsVsnprintf, CPSS_OS_IO_VSNPRINTF_FUNC);
DECLARE_FUNC(cpssOsPrintSync, CPSS_OS_IO_PRINT_SYNC_FUNC);
DECLARE_FUNC0(cpssOsGets, CPSS_OS_IO_GETS_FUNC);

DECLARE_FUNC0(cpssOsFlastErrorStr, CPSS_OS_IO_F_LAST_ERROR_STR_FUNC);
DECLARE_FUNC0(cpssOsFopen, CPSS_OS_IO_FOPEN_FUNC);
DECLARE_FUNC(cpssOsFclose, CPSS_OS_IO_FCLOSE_FUNC);
DECLARE_FUNC(cpssOsRewind, CPSS_OS_IO_REWIND_FUNC);
DECLARE_FUNC0(cpssOsFprintf, CPSS_OS_IO_FPRINTF_FUNC);
DECLARE_FUNC0(cpssOsFgets, CPSS_OS_IO_FGETS_FUNC);
DECLARE_FUNC0(cpssOsFwrite, CPSS_OS_IO_FWRITE_FUNC);
DECLARE_FUNC0(cpssOsFread, CPSS_OS_IO_FREAD_FUNC);
DECLARE_FUNC0(cpssOsFgetLength, CPSS_OS_IO_FGET_LENGTH_FUNC);
DECLARE_FUNC(cpssOsFatalError, CPSS_OS_FATAL_ERROR_FUNC);

DECLARE_FUNC(cpssOsBzero, CPSS_OS_BZERO_FUNC);
DECLARE_FUNC(cpssOsMemSet, CPSS_OS_MEM_SET_FUNC);
DECLARE_FUNC0(cpssOsMemCpy, CPSS_OS_MEM_CPY_FUNC);
DECLARE_FUNC0(cpssOsMemMove, CPSS_OS_MEM_MOVE_FUNC);
DECLARE_FUNC0(cpssOsMemCmp, CPSS_OS_MEM_CMP_FUNC);
DECLARE_FUNC0(cpssOsStaticMalloc, CPSS_OS_STATIC_MALLOC_FUNC);
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    DECLARE_FUNC0(cpssOsMalloc_MemoryLeakageDbg, CPSS_OS_MALLOC_FUNC);
    DECLARE_FUNC0(cpssOsRealloc_MemoryLeakageDbg, CPSS_OS_REALLOC_FUNC);
    DECLARE_FUNC(cpssOsFree_MemoryLeakageDbg, CPSS_OS_FREE_FUNC);
#else  /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    DECLARE_FUNC0(cpssOsMalloc, CPSS_OS_MALLOC_FUNC);
    DECLARE_FUNC0(cpssOsRealloc, CPSS_OS_REALLOC_FUNC);
    DECLARE_FUNC(cpssOsFree, CPSS_OS_FREE_FUNC);
#endif /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/

DECLARE_FUNC0(cpssOsCacheDmaMalloc, CPSS_OS_CACHE_DMA_MALLOC_FUNC);
DECLARE_FUNC(cpssOsCacheDmaFree, CPSS_OS_CACHE_DMA_FREE_FUNC);
DECLARE_FUNC(cpssOsPhy2Virt, CPSS_OS_PHY_TO_VIRT_FUNC);
DECLARE_FUNC(cpssOsVirt2Phy, CPSS_OS_VIRT_TO_PHY_FUNC);

DECLARE_FUNC(cpssOsRand, CPSS_OS_RAND_FUNC);
DECLARE_FUNC(cpssOsSrand, CPSS_OS_SRAND_FUNC);

DECLARE_FUNC(cpssOsMutexCreate, CPSS_OS_MUTEX_CREATE_FUNC);
DECLARE_FUNC(cpssOsMutexDelete, CPSS_OS_MUTEX_DELETE_FUNC);
DECLARE_FUNC(cpssOsMutexLock, CPSS_OS_MUTEX_LOCK_FUNC);
DECLARE_FUNC(cpssOsMutexUnlock, CPSS_OS_MUTEX_UNLOCK_FUNC);

DECLARE_FUNC(cpssOsSigSemBinCreate, CPSS_OS_SIG_SEM_BIN_CREATE_FUNC);
DECLARE_FUNC(cpssOsSigSemMCreate, CPSS_OS_SIG_SEM_M_CREATE_FUNC);
DECLARE_FUNC(cpssOsSigSemCCreate, CPSS_OS_SIG_SEM_C_CREATE_FUNC);
DECLARE_FUNC(cpssOsSigSemDelete, CPSS_OS_SIG_SEM_DELETE_FUNC);
DECLARE_FUNC(cpssOsSigSemWait, CPSS_OS_SIG_SEM_WAIT_FUNC);
DECLARE_FUNC(cpssOsSigSemSignal, CPSS_OS_SIG_SEM_SIGNAL_FUNC);

DECLARE_FUNC0(cpssOsStrlen, CPSS_OS_STR_LEN_FUNC);
DECLARE_FUNC0(cpssOsStrCpy, CPSS_OS_STR_CPY_FUNC);
DECLARE_FUNC0(cpssOsStrNCpy, CPSS_OS_STR_N_CPY_FUNC);
DECLARE_FUNC0(cpssOsStrChr, CPSS_OS_STR_CHR_FUNC);
DECLARE_FUNC0(cpssOsStrStr, CPSS_OS_STR_STR_FUNC);
DECLARE_FUNC0(cpssOsStrRChr, CPSS_OS_STR_REV_CHR_FUNC);
DECLARE_FUNC0(cpssOsStrCmp, CPSS_OS_STR_CMP_FUNC);
DECLARE_FUNC0(cpssOsStrNCmp, CPSS_OS_STR_N_CMP_FUNC);
DECLARE_FUNC0(cpssOsStrCat, CPSS_OS_STR_CAT_FUNC);
DECLARE_FUNC0(cpssOsStrNCat, CPSS_OS_STR_N_CAT_FUNC);
DECLARE_FUNC0(cpssOsToUpper, CPSS_OS_TO_UPPER_FUNC);
DECLARE_FUNC0(cpssOsStrTo32, CPSS_OS_STR_TO_32_FUNC);
DECLARE_FUNC0(cpssOsStrToU32, CPSS_OS_STR_TO_U32_FUNC);

DECLARE_FUNC(cpssOsTimerWkAfter, CPSS_OS_TIME_WK_AFTER_FUNC);
DECLARE_FUNC0(cpssOsTickGet, CPSS_OS_TIME_TICK_GET_FUNC);
DECLARE_FUNC0(cpssOsTime, CPSS_OS_TIME_GET_FUNC);
DECLARE_FUNC(cpssOsTimeRT, CPSS_OS_TIME_RT_FUNC);
DECLARE_FUNC(cpssOsGetSysClockRate, CPSS_OS_GET_SYS_CLOCK_RATE_FUNC);
DECLARE_FUNC(cpssOsDelay, CPSS_OS_DELAY_FUNC);
DECLARE_FUNC0(cpssOsStrfTime, CPSS_OS_STRF_TIME_FUNC);

DECLARE_FUNC(cpssOsTaskCreate, CPSS_OS_TASK_CREATE_FUNC);
DECLARE_FUNC(cpssOsTaskDelete, CPSS_OS_TASK_DELETE_FUNC);
DECLARE_FUNC(cpssOsTaskGetSelf, CPSS_OS_TASK_GET_SELF_FUNC);
DECLARE_FUNC(cpssOsTaskLock, CPSS_OS_TASK_LOCK_FUNC);
DECLARE_FUNC(cpssOsTaskUnLock, CPSS_OS_TASK_UNLOCK_FUNC);

DECLARE_FUNC(cpssOsQsort, CPSS_OS_STDLIB_QSORT_FUNC);
DECLARE_FUNC(cpssOsBsearch, CPSS_OS_STDLIB_BSEARCH_FUNC);

DECLARE_FUNC(cpssOsMsgQCreate, CPSS_OS_MSGQ_CREATE_FUNC);
DECLARE_FUNC(cpssOsMsgQDelete, CPSS_OS_MSGQ_DELETE_FUNC);
DECLARE_FUNC(cpssOsMsgQSend, CPSS_OS_MSGQ_SEND_FUNC);
DECLARE_FUNC(cpssOsMsgQRecv, CPSS_OS_MSGQ_RECV_FUNC);
DECLARE_FUNC(cpssOsMsgQNumMsgs, CPSS_OS_MSGQ_NUM_MSGS_FUNC);
DECLARE_FUNC(cpssOsLog, CPSS_OS_LOG_FUNC);

DECLARE_FUNC(cpssTraceHwAccessWrite, CPSS_TRACE_HW_ACCESS_WRITE_FUNC);
DECLARE_FUNC(cpssTraceHwAccessRead, CPSS_TRACE_HW_ACCESS_READ_FUNC);
DECLARE_FUNC(cpssTraceHwAccessDelay, CPSS_TRACE_HW_ACCESS_DELAY_FUNC);


DECLARE_FUNC0(cpssOsSocketLastError, CPSS_SOCKET_LAST_ERROR_FUNC);
DECLARE_FUNC(cpssOsSocketLastErrorStr, CPSS_SOCKET_LAST_ERROR_STR_FUNC);
DECLARE_FUNC(cpssOsSocketTcpCreate, CPSS_SOCKET_TCP_CREATE_FUNC);
DECLARE_FUNC(cpssOsSocketUdpCreate, CPSS_SOCKET_UDP_CREATE_FUNC);
DECLARE_FUNC(cpssOsSocketTcpDestroy, CPSS_SOCKET_TCP_DESTROY_FUNC);
DECLARE_FUNC(cpssOsSocketUdpDestroy, CPSS_SOCKET_UDP_DESTROY_FUNC);
DECLARE_FUNC(cpssOsSocketCreateAddr, CPSS_SOCKET_CREATE_ADDR_FUNC);
DECLARE_FUNC(cpssOsSocketDestroyAddr, CPSS_SOCKET_DESTROY_ADDR_FUNC);
DECLARE_FUNC(cpssOsSocketBind, CPSS_SOCKET_BIND_FUNC);
DECLARE_FUNC(cpssOsSocketListen, CPSS_SOCKET_LISTEN_FUNC);
DECLARE_FUNC(cpssOsSocketAccept, CPSS_SOCKET_ACCEPT_FUNC);
DECLARE_FUNC(cpssOsSocketConnect, CPSS_SOCKET_CONNECT_FUNC);
DECLARE_FUNC(cpssOsSocketSetNonBlock, CPSS_SOCKET_SET_NONBLOCK_FUNC);
DECLARE_FUNC(cpssOsSocketSetBlock, CPSS_SOCKET_SET_BLOCK_FUNC);
DECLARE_FUNC(cpssOsSocketSend, CPSS_SOCKET_SEND_FUNC);
DECLARE_FUNC(cpssOsSocketSendTo, CPSS_SOCKET_SENDTO_FUNC);
DECLARE_FUNC(cpssOsSocketRecv, CPSS_SOCKET_RECV_FUNC);
DECLARE_FUNC(cpssOsSocketRecvFrom, CPSS_SOCKET_RECVFROM_FUNC);
DECLARE_FUNC(cpssOsSocketSetSocketNoLinger, CPSS_SOCKET_SET_SOCKET_NOLINGER_FUNC);
DECLARE_FUNC(cpssOsSocketExtractIpAddrFromSocketAddr, CPSS_SOCKET_EXTRACT_IPADDR_FROM_SOCKET_ADDR_FUNC);
DECLARE_FUNC(cpssOsSocketGetSocketAddrSize, CPSS_SOCKET_GET_ADDR_SIZE_FUNC);
DECLARE_FUNC(cpssOsSocketShutDown, CPSS_SOCKET_SHUTDOWN_FUNC);


DECLARE_FUNC(cpssOsSelectCreateSet, CPSS_SOCKET_SELECT_CREATE_SET_FUNC);
DECLARE_FUNC(cpssOsSelectEraseSet, CPSS_SOCKET_SELECT_ERASE_SET_FUNC);
DECLARE_FUNC(cpssOsSelectZeroSet, CPSS_SOCKET_SELECT_ZERO_SET_FUNC);
DECLARE_FUNC(cpssOsSelectAddFdToSet, CPSS_SOCKET_SELECT_ADD_FD_TO_SET_FUNC);
DECLARE_FUNC(cpssOsSelectClearFdFromSet, CPSS_SOCKET_SELECT_CLEAR_FD_FROM_SET_FUNC);
DECLARE_FUNC(cpssOsSelectIsFdSet, CPSS_SOCKET_SELECT_IS_FD_SET_FUNC);
DECLARE_FUNC(cpssOsSelectCopySet, CPSS_SOCKET_SELECT_COPY_SET_FUNC);
DECLARE_FUNC(cpssOsSelect, CPSS_SOCKET_SELECT_FUNC);
DECLARE_FUNC(cpssOsSocketGetSocketFdSetSize, CPSS_SOCKET_SELECT_GET_SOCKET_FD_SIZE_FUNC);

/*******************************************************************************
* cpssExtServicesBind
*
* DESCRIPTION:
*       bind the cpss with OS , external driver functions.
* INPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of call back functions
*       osFuncBindPtr - (pointer to) set of call back functions
*       traceFuncBindPtr - (pointer to) set of call back functions
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       function must be called by application before phase 1 init
*
*******************************************************************************/
/* can't use here cpssOsMemCpy, because this pointer isn't initialized yet */
static GT_VOID * localMemCpy
(
    IN GT_VOID *      destination,
    IN const GT_VOID* source,
    IN GT_U32         size
)
{
    GT_U8* dst_ptr = (GT_U8 *)destination;
    GT_U8* src_ptr = (GT_U8 *)source;

    while(size--)
    {
        *dst_ptr++ = *src_ptr++;
    }

    return destination;
}

GT_STATUS   cpssExtServicesBind(
    IN CPSS_EXT_DRV_FUNC_BIND_STC   *extDrvFuncBindInfoPtr,
    IN CPSS_OS_FUNC_BIND_STC        *osFuncBindPtr,
    IN CPSS_TRACE_FUNC_BIND_STC     *traceFuncBindPtr
)
{
    if(extDrvFuncBindInfoPtr)
    {
        /* ext drv functions */
        localMemCpy(&prvCpssBindFuncs.extDrv, extDrvFuncBindInfoPtr,
                     sizeof(prvCpssBindFuncs.extDrv));
    }

    if(osFuncBindPtr)
    {
        /* OS functions */
        localMemCpy(&prvCpssBindFuncs.os, osFuncBindPtr,
                     sizeof(prvCpssBindFuncs.os));
    }

    if(traceFuncBindPtr)
    {
        /* HW access trace functions */
        localMemCpy(&prvCpssBindFuncs.trace, traceFuncBindPtr,
                     sizeof(prvCpssBindFuncs.trace));
    }

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvMgmtCacheFlush      , prvCpssBindFuncs.extDrv.extDrvMgmtCacheBindInfo.extDrvMgmtCacheFlush);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvMgmtCacheInvalidate , prvCpssBindFuncs.extDrv.extDrvMgmtCacheBindInfo.extDrvMgmtCacheInvalidate);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvDmaWrite            , prvCpssBindFuncs.extDrv.extDrvDmaBindInfo.extDrvDmaWriteDriverFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvDmaRead             , prvCpssBindFuncs.extDrv.extDrvDmaBindInfo.extDrvDmaReadFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthPortRxInit       , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortRxInitFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthRawSocketModeSet       , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthRawSocketModeSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthRawSocketModeGet       , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthRawSocketModeGetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvLinuxModeSet       , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvLinuxModeSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvLinuxModeGet       , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvLinuxModeGetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHsuMemBaseAddrGet         , prvCpssBindFuncs.extDrv.extDrvHsuDrvBindInfo.extDrvHsuMemBaseAddrGetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHsuWarmRestart           , prvCpssBindFuncs.extDrv.extDrvHsuDrvBindInfo.extDrvHsuWarmRestartFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvInboundSdmaEnable        , prvCpssBindFuncs.extDrv.extDrvHsuDrvBindInfo.extDrvHsuInboundSdmaEnableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvInboundSdmaDisable       , prvCpssBindFuncs.extDrv.extDrvHsuDrvBindInfo.extDrvHsuInboundSdmaDisableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvInboundSdmaStateGet      , prvCpssBindFuncs.extDrv.extDrvHsuDrvBindInfo.extDrvHsuInboundSdmaStateGetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthPortTxInit            , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortTxInitFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthPortEnable       , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortEnableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthPortDisable      , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortDisableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthPortTx           , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortTxFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthInputHookAdd     , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortInputHookAddFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthRawSocketRxHookAdd     , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthRawSocketRxHookAddFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthTxCompleteHookAdd, prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortTxCompleteHookAddFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthRxPacketFree     , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortRxPacketFreeFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthPortTxModeSet    , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPortTxModeSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthCpuCodeToQueue   , prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthCpuCodeToQueueFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEthPrePendTwoBytesHeaderSet, prvCpssBindFuncs.extDrv.extDrvEthPortBindInfo.extDrvEthPrePendTwoBytesHeaderSetFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvIntConnect          , prvCpssBindFuncs.extDrv.extDrvIntBindInfo.extDrvIntConnectFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvIntEnable           , prvCpssBindFuncs.extDrv.extDrvIntBindInfo.extDrvIntEnableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvIntDisable          , prvCpssBindFuncs.extDrv.extDrvIntBindInfo.extDrvIntDisableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvSetIntLockUnlock    , prvCpssBindFuncs.extDrv.extDrvIntBindInfo.extDrvIntLockModeSetFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvPciConfigWriteReg   , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciConfigWriteRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvPciConfigReadReg    , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciConfigReadRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvPciFindDev          , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciDevFindFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvGetPciIntVec        , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciIntVecFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvGetIntMask          , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciIntMaskFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvEnableCombinedPciAccess  , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciCombinedAccessEnableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvPciDoubleWrite      , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciDoubleWriteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvPciDoubleRead       , prvCpssBindFuncs.extDrv.extDrvPciInfo.extDrvPciDoubleReadFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiInitDriver        , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiInitDriverFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiWriteReg          , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiWriteRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiReadReg           , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiReadRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiTskRegRamRead     , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamReadFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiTskRegRamWrite    , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamWriteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiTskRegVecRead     , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecReadFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiTskRegVecWrite    , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecWriteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiTaskWriteReg      , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiTaskReadReg       , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskReadRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiInterruptWriteReg , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntWriteRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiInterruptReadReg  , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntReadRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvSmiDevVendorIdGet        , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiDevVendorIdGetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfSmiTaskWriteRegField , prvCpssBindFuncs.extDrv.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteFieldFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfTwsiInitDriver , prvCpssBindFuncs.extDrv.extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiInitDriverFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfTwsiWriteReg   , prvCpssBindFuncs.extDrv.extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiWriteRegFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvHwIfTwsiReadReg    , prvCpssBindFuncs.extDrv.extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiReadRegFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvI2cMgmtMasterInit    , prvCpssBindFuncs.extDrv.extDrvMgmtHwIfBindInfo.extDrvI2cMgmtMasterInitFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvMgmtReadRegister     , prvCpssBindFuncs.extDrv.extDrvMgmtHwIfBindInfo.extDrvMgmtReadRegisterFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvMgmtWriteRegister    , prvCpssBindFuncs.extDrv.extDrvMgmtHwIfBindInfo.extDrvMgmtWriteRegisterFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvMgmtIsrReadRegister  , prvCpssBindFuncs.extDrv.extDrvMgmtHwIfBindInfo.extDrvMgmtIsrReadRegisterFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvMgmtIsrWriteRegister , prvCpssBindFuncs.extDrv.extDrvMgmtHwIfBindInfo.extDrvMgmtIsrWriteRegisterFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssExtDrvDragoniteShMemBaseAddrGet , prvCpssBindFuncs.extDrv.extDrvDragoniteInfo.extDrvDragoniteShMemBaseAddrGetFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsNtohl       , prvCpssBindFuncs.os.osInetBindInfo.osInetNtohlFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsHtonl       , prvCpssBindFuncs.os.osInetBindInfo.osInetHtonlFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsNtohs       , prvCpssBindFuncs.os.osInetBindInfo.osInetNtohsFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsHtons       , prvCpssBindFuncs.os.osInetBindInfo.osInetHtonsFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsInetNtoa    , prvCpssBindFuncs.os.osInetBindInfo.osInetNtoaFunc);

#if !defined(_linux) && !defined(_FreeBSD)
    REPLACE_IF_NOT_NULL_MAC(cpssOsIntEnable        , prvCpssBindFuncs.os.osIntBindInfo.osIntEnableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsIntDisable       , prvCpssBindFuncs.os.osIntBindInfo.osIntDisableFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSetIntLockUnlock , prvCpssBindFuncs.os.osIntBindInfo.osIntModeSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsInterruptConnect , prvCpssBindFuncs.os.osIntBindInfo.osIntConnectFunc);
#endif

    REPLACE_IF_NOT_NULL_MAC(cpssOsBindStdOut , prvCpssBindFuncs.os.osIoBindInfo.osIoBindStdOutFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsPrintf     , prvCpssBindFuncs.os.osIoBindInfo.osIoPrintfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsVprintf    , prvCpssBindFuncs.os.osIoBindInfo.osIoVprintfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSprintf    , prvCpssBindFuncs.os.osIoBindInfo.osIoSprintfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsVsprintf   , prvCpssBindFuncs.os.osIoBindInfo.osIoVsprintfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSnprintf   , prvCpssBindFuncs.os.osIoBindInfo.osIoSnprintfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsVsnprintf  , prvCpssBindFuncs.os.osIoBindInfo.osIoVsnprintfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsPrintSync  , prvCpssBindFuncs.os.osIoBindInfo.osIoPrintSynchFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsGets       , prvCpssBindFuncs.os.osIoBindInfo.osIoGetsFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsFlastErrorStr , prvCpssBindFuncs.os.osIoBindInfo.osIoFlastErrorStrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFopen         , prvCpssBindFuncs.os.osIoBindInfo.osIoFopenFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFclose        , prvCpssBindFuncs.os.osIoBindInfo.osIoFcloseFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsRewind        , prvCpssBindFuncs.os.osIoBindInfo.osIoRewindFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFprintf       , prvCpssBindFuncs.os.osIoBindInfo.osIoFprintfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFgets         , prvCpssBindFuncs.os.osIoBindInfo.osIoFgets);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFwrite        , prvCpssBindFuncs.os.osIoBindInfo.osIoFwriteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFread         , prvCpssBindFuncs.os.osIoBindInfo.osIoFreadFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFgetLength    , prvCpssBindFuncs.os.osIoBindInfo.osIoFgetLengthFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsFatalError    , prvCpssBindFuncs.os.osIoBindInfo.osIoFatalErrorFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsBzero         , prvCpssBindFuncs.os.osMemBindInfo.osMemBzeroFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMemSet        , prvCpssBindFuncs.os.osMemBindInfo.osMemSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMemCpy        , prvCpssBindFuncs.os.osMemBindInfo.osMemCpyFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMemMove       , prvCpssBindFuncs.os.osMemBindInfo.osMemMoveFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMemCmp        , prvCpssBindFuncs.os.osMemBindInfo.osMemCmpFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStaticMalloc  , prvCpssBindFuncs.os.osMemBindInfo.osMemStaticMallocFunc);

#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    REPLACE_IF_NOT_NULL_MAC(cpssOsMalloc_MemoryLeakageDbg        , prvCpssBindFuncs.os.osMemBindInfo.osMemMallocFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsRealloc_MemoryLeakageDbg       , prvCpssBindFuncs.os.osMemBindInfo.osMemReallocFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFree_MemoryLeakageDbg          , prvCpssBindFuncs.os.osMemBindInfo.osMemFreeFunc);
#else  /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    REPLACE_IF_NOT_NULL_MAC(cpssOsMalloc        , prvCpssBindFuncs.os.osMemBindInfo.osMemMallocFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsRealloc       , prvCpssBindFuncs.os.osMemBindInfo.osMemReallocFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsFree          , prvCpssBindFuncs.os.osMemBindInfo.osMemFreeFunc);
#endif /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    REPLACE_IF_NOT_NULL_MAC(cpssOsCacheDmaMalloc, prvCpssBindFuncs.os.osMemBindInfo.osMemCacheDmaMallocFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsCacheDmaFree  , prvCpssBindFuncs.os.osMemBindInfo.osMemCacheDmaFreeFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsPhy2Virt      , prvCpssBindFuncs.os.osMemBindInfo.osMemPhyToVirtFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsVirt2Phy      , prvCpssBindFuncs.os.osMemBindInfo.osMemVirtToPhyFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsRand    , prvCpssBindFuncs.os.osRandBindInfo.osRandFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSrand   , prvCpssBindFuncs.os.osRandBindInfo.osSrandFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsMutexCreate     , prvCpssBindFuncs.os.osSemBindInfo.osMutexCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMutexDelete     , prvCpssBindFuncs.os.osSemBindInfo.osMutexDeleteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMutexLock       , prvCpssBindFuncs.os.osSemBindInfo.osMutexLockFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMutexUnlock     , prvCpssBindFuncs.os.osSemBindInfo.osMutexUnlockFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsSigSemBinCreate , prvCpssBindFuncs.os.osSemBindInfo.osSigSemBinCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSigSemMCreate   , prvCpssBindFuncs.os.osSemBindInfo.osSigSemMCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSigSemCCreate   , prvCpssBindFuncs.os.osSemBindInfo.osSigSemCCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSigSemDelete    , prvCpssBindFuncs.os.osSemBindInfo.osSigSemDeleteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSigSemWait      , prvCpssBindFuncs.os.osSemBindInfo.osSigSemWaitFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSigSemSignal    , prvCpssBindFuncs.os.osSemBindInfo.osSigSemSignalFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsStrlen   , prvCpssBindFuncs.os.osStrBindInfo.osStrlenFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrCpy   , prvCpssBindFuncs.os.osStrBindInfo.osStrCpyFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrNCpy  , prvCpssBindFuncs.os.osStrBindInfo.osStrNCpyFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrChr   , prvCpssBindFuncs.os.osStrBindInfo.osStrChrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrStr   , prvCpssBindFuncs.os.osStrBindInfo.osStrStrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrRChr  , prvCpssBindFuncs.os.osStrBindInfo.osStrRevChrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrCmp   , prvCpssBindFuncs.os.osStrBindInfo.osStrCmpFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrNCmp  , prvCpssBindFuncs.os.osStrBindInfo.osStrNCmpFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrCat   , prvCpssBindFuncs.os.osStrBindInfo.osStrCatFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrNCat  , prvCpssBindFuncs.os.osStrBindInfo.osStrStrNCatFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsToUpper  , prvCpssBindFuncs.os.osStrBindInfo.osStrChrToUpperFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrTo32  , prvCpssBindFuncs.os.osStrBindInfo.osStrTo32Func);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrToU32 , prvCpssBindFuncs.os.osStrBindInfo.osStrToU32Func);

    REPLACE_IF_NOT_NULL_MAC(cpssOsTimerWkAfter, prvCpssBindFuncs.os.osTimeBindInfo.osTimeWkAfterFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsTickGet     , prvCpssBindFuncs.os.osTimeBindInfo.osTimeTickGetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsTime        , prvCpssBindFuncs.os.osTimeBindInfo.osTimeGetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsTimeRT      , prvCpssBindFuncs.os.osTimeBindInfo.osTimeRTFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsGetSysClockRate , prvCpssBindFuncs.os.osTimeBindInfo.osGetSysClockRateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsDelay       , prvCpssBindFuncs.os.osTimeBindInfo.osDelayFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsStrfTime    , prvCpssBindFuncs.os.osTimeBindInfo.osStrftimeFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsTaskCreate  , prvCpssBindFuncs.os.osTaskBindInfo.osTaskCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsTaskDelete  , prvCpssBindFuncs.os.osTaskBindInfo.osTaskDeleteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsTaskGetSelf , prvCpssBindFuncs.os.osTaskBindInfo.osTaskGetSelfFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsTaskLock    , prvCpssBindFuncs.os.osTaskBindInfo.osTaskLockFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsTaskUnLock  , prvCpssBindFuncs.os.osTaskBindInfo.osTaskUnLockFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsQsort       , prvCpssBindFuncs.os.osStdLibBindInfo.osQsortFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsBsearch     , prvCpssBindFuncs.os.osStdLibBindInfo.osBsearchFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsMsgQCreate  , prvCpssBindFuncs.os.osMsgQBindInfo.osMsgQCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMsgQDelete  , prvCpssBindFuncs.os.osMsgQBindInfo.osMsgQDeleteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMsgQSend    , prvCpssBindFuncs.os.osMsgQBindInfo.osMsgQSendFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMsgQRecv    , prvCpssBindFuncs.os.osMsgQBindInfo.osMsgQRecvFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsMsgQNumMsgs , prvCpssBindFuncs.os.osMsgQBindInfo.osMsgQNumMsgsFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsLog         , prvCpssBindFuncs.os.osLogBindInfo.osLogFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketLastError, prvCpssBindFuncs.os.osSocketInfo.osSocketLastErrorFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketLastErrorStr, prvCpssBindFuncs.os.osSocketInfo.osSocketLastErrorStrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketTcpCreate, prvCpssBindFuncs.os.osSocketInfo.osSocketTcpCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketUdpCreate, prvCpssBindFuncs.os.osSocketInfo.osSocketUdpCreateFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketTcpDestroy, prvCpssBindFuncs.os.osSocketInfo.osSocketTcpDestroyFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketUdpDestroy, prvCpssBindFuncs.os.osSocketInfo.osSocketUdpDestroyFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketCreateAddr, prvCpssBindFuncs.os.osSocketInfo.osSocketCreateAddrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketDestroyAddr, prvCpssBindFuncs.os.osSocketInfo.osSocketDestroyAddrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketBind, prvCpssBindFuncs.os.osSocketInfo.osSocketBindFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketListen, prvCpssBindFuncs.os.osSocketInfo.osSocketListenFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketAccept, prvCpssBindFuncs.os.osSocketInfo.osSocketAcceptFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketConnect, prvCpssBindFuncs.os.osSocketInfo.osSocketConnectFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketSetNonBlock, prvCpssBindFuncs.os.osSocketInfo.osSocketSetNonBlockFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketSetBlock, prvCpssBindFuncs.os.osSocketInfo.osSocketSetBlockFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketSend, prvCpssBindFuncs.os.osSocketInfo.osSocketSendFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketSendTo, prvCpssBindFuncs.os.osSocketInfo.osSocketSendToFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketRecv, prvCpssBindFuncs.os.osSocketInfo.osSocketRecvFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketRecvFrom, prvCpssBindFuncs.os.osSocketInfo.osSocketRecvFromFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketSetSocketNoLinger, prvCpssBindFuncs.os.osSocketInfo.osSocketSetSocketNoLingerFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketExtractIpAddrFromSocketAddr, prvCpssBindFuncs.os.osSocketInfo.osSocketExtractIpAddrFromSocketAddrFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketGetSocketAddrSize, prvCpssBindFuncs.os.osSocketInfo.osSocketGetSocketAddrSizeFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketShutDown, prvCpssBindFuncs.os.osSocketInfo.osSocketShutDownFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssOsSelectCreateSet, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectCreateSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSelectEraseSet, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectEraseSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSelectZeroSet, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectZeroSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSelectAddFdToSet, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectAddFdToSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSelectClearFdFromSet, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectClearFdFromSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSelectIsFdSet, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectIsFdSetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSelectCopySet, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectCopySetFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSelect, prvCpssBindFuncs.os.osSocketSelectInfo.osSelectFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssOsSocketGetSocketFdSetSize, prvCpssBindFuncs.os.osSocketSelectInfo.osSocketGetSocketFdSetSizeFunc);

    REPLACE_IF_NOT_NULL_MAC(cpssTraceHwAccessWrite  , prvCpssBindFuncs.trace.traceHwBindInfo.traceHwAccessWriteFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssTraceHwAccessRead   , prvCpssBindFuncs.trace.traceHwBindInfo.traceHwAccessReadFunc);
    REPLACE_IF_NOT_NULL_MAC(cpssTraceHwAccessDelay   , prvCpssBindFuncs.trace.traceHwBindInfo.traceHwAccessDelayFunc);

#if defined(CPSS_LOG_ENABLE)
    prvCpssLogInit();
#endif
    return GT_OK;
}

