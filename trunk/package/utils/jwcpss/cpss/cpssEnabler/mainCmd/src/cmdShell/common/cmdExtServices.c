/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdExtServices.c
*
* DESCRIPTION:
*       Common definition and APIs for Commander external services.
*
*
* FILE REVISION NUMBER:
*       $Revision: 44 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>

/* hold main cmd external driver CB */
CPSS_EXT_DRV_FUNC_BIND_STC   cmdExtDrvFuncBindInfo;
/* hold main cmd OS CB */
CPSS_OS_FUNC_BIND_STC        cmdOsFuncBind;
/* hold main cmd extra OS CB */
CMD_OS_FUNC_BIND_EXTRA_STC   cmdOsExtraFuncBind;

/* hold main cmd extra CB */
CMD_FUNC_BIND_EXTRA_STC   cmdExtraFuncBind;

/* hold main cmd Trace CB */
CPSS_TRACE_FUNC_BIND_STC  cmdTraceFuncBind;

#define SET_NON_NULL(dst,src)   if(src != NULL) dst = src

#define STR_NOT_IMPLEMENTED_CNS " extServiceFuncNotImplementedCalled \n"
static GT_STATUS extServiceFuncNotImplementedCalled
(
    void
)
{
    if(cmdOsPrintf != (CPSS_OS_IO_PRINTF_FUNC)extServiceFuncNotImplementedCalled)
    {
        /* we already have "printf" from the application
          but current pointer of a function was not initialized yet */
        cmdOsPrintf(STR_NOT_IMPLEMENTED_CNS);
    }

    return GT_NOT_IMPLEMENTED;
}


/************* Prototypes *****************************************************/

CPSS_EXT_DRV_MGMT_CACHE_FLUSH_FUNC      cmdExtDrvMgmtCacheFlush                      = (CPSS_EXT_DRV_MGMT_CACHE_FLUSH_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXT_DRV_MGMT_CACHE_INVALIDATE_FUNC cmdExtDrvMgmtCacheInvalidate                 = (CPSS_EXT_DRV_MGMT_CACHE_INVALIDATE_FUNC) extServiceFuncNotImplementedCalled;

CPSS_EXTDRV_DMA_WRITE_FUNC  cmdExtDrvDmaWrite                                        = (CPSS_EXTDRV_DMA_WRITE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_DMA_READ_FUNC   cmdExtDrvDmaRead                                         = (CPSS_EXTDRV_DMA_READ_FUNC) extServiceFuncNotImplementedCalled;

CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC       cmdExtDrvEthPortRxInit                       = (CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_SET_FUNC       cmdExtDrvEthRawSocketModeSet                       = (CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_SET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_GET_FUNC       cmdExtDrvEthRawSocketModeGet                       = (CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_GET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_LINUX_MODE_SET_FUNC       cmdExtDrvLinuxModeSet                       = (CPSS_EXTDRV_LINUX_MODE_SET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_LINUX_MODE_GET_FUNC       cmdExtDrvLinuxModeGet                       = (CPSS_EXTDRV_LINUX_MODE_GET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HSU_MEM_BASE_ADDR_GET_FUNC        cmdExtDrvHsuMemBaseAddrGet                       = (CPSS_EXTDRV_HSU_MEM_BASE_ADDR_GET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HSU_WARM_RESTART_FUNC            cmdExtDrvHsuWarmRestart                       = (CPSS_EXTDRV_HSU_WARM_RESTART_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC       cmdExtDrvEthPortTxInit                       = (CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC        cmdExtDrvEthPortEnable                       = (CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_PORT_DISABLE_FUNC       cmdExtDrvEthPortDisable                      = (CPSS_EXTDRV_ETH_PORT_DISABLE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_PORT_TX_FUNC            cmdExtDrvEthPortTx                           = (CPSS_EXTDRV_ETH_PORT_TX_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC  cmdExtDrvEthInputHookAdd                   = (CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC  cmdExtDrvEthTxCompleteHookAdd        = (CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC  cmdExtDrvEthRxPacketFree                   = (CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC) extServiceFuncNotImplementedCalled;

CPSS_EXTDRV_INT_CONNECT_FUNC        cmdExtDrvIntConnect                              = (CPSS_EXTDRV_INT_CONNECT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_INT_ENABLE_FUNC         cmdExtDrvIntEnable                               = (CPSS_EXTDRV_INT_ENABLE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_INT_DISABLE_FUNC        cmdExtDrvIntDisable                              = (CPSS_EXTDRV_INT_DISABLE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC  cmdExtDrvSetIntLockUnlock                        = (CPSS_EXTDRV_INT_LOCK_MODE_SET_FUNC) extServiceFuncNotImplementedCalled;

CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC      cmdExtDrvPciConfigWriteReg                = (CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC       cmdExtDrvPciConfigReadReg                 = (CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_PCI_DEV_FIND_FUNC              cmdExtDrvPciFindDev                       = (CPSS_EXTDRV_PCI_DEV_FIND_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC           cmdExtDrvGetPciIntVec                     = (CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_INT_MASK_GET_FUNC              cmdExtDrvGetIntMask                       = (CPSS_EXTDRV_INT_MASK_GET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC cmdExtDrvEnableCombinedPciAccess         = (CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC) extServiceFuncNotImplementedCalled;

CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC             cmdExtDrvHwIfSmiInitDriver        = (CPSS_EXTDRV_HW_IF_SMI_INIT_DRIVER_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC               cmdExtDrvHwIfSmiWriteReg          = (CPSS_EXTDRV_HW_IF_SMI_WRITE_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC                cmdExtDrvHwIfSmiReadReg           = (CPSS_EXTDRV_HW_IF_SMI_READ_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC       cmdExtDrvHwIfSmiTskRegRamRead     = (CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_READ_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC      cmdExtDrvHwIfSmiTskRegRamWrite    = (CPSS_EXTDRV_HW_IF_SMI_TASK_REG_RAM_WRITE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC       cmdExtDrvHwIfSmiTskRegVecRead     = (CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_READ_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC      cmdExtDrvHwIfSmiTskRegVecWrite    = (CPSS_EXTDRV_HW_IF_SMI_TASK_REG_VEC_WRITE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC          cmdExtDrvHwIfSmiTaskWriteReg      = (CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC           cmdExtDrvHwIfSmiTaskReadReg       = (CPSS_EXTDRV_HW_IF_SMI_TASK_READ_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC     cmdExtDrvHwIfSmiInterruptWriteReg = (CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_WRITE_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC      cmdExtDrvHwIfSmiInterruptReadReg  = (CPSS_EXTDRV_HW_IF_SMI_INTERRUPT_READ_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC       cmdExtDrvSmiDevVendorIdGet        = (CPSS_EXTDRV_HW_IF_SMI_DEV_VENDOR_ID_GET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC    cmdExtDrvHwIfSmiTaskWriteRegField = (CPSS_EXTDRV_HW_IF_SMI_TASK_WRITE_REG_FIELD_FUNC) extServiceFuncNotImplementedCalled;

CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC  cmdExtDrvHwIfTwsiInitDriver                 = (CPSS_EXTDRV_HW_IF_TWSI_INIT_DRIVER_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC    cmdExtDrvHwIfTwsiWriteReg                   = (CPSS_EXTDRV_HW_IF_TWSI_WRITE_REG_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC     cmdExtDrvHwIfTwsiReadReg                    = (CPSS_EXTDRV_HW_IF_TWSI_READ_REG_FUNC) extServiceFuncNotImplementedCalled;

CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC      cmdExtDrvI2cMgmtMasterInit                = (CPSS_EXTDRV_I2C_MGMT_MASTER_INIT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC        cmdExtDrvMgmtReadRegister                 = (CPSS_EXTDRV_MGMT_READ_REGISTER_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC       cmdExtDrvMgmtWriteRegister                = (CPSS_EXTDRV_MGMT_WRITE_REGISTER_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC    cmdExtDrvMgmtIsrReadRegister              = (CPSS_EXTDRV_MGMT_ISR_READ_REGISTER_FUNC) extServiceFuncNotImplementedCalled;
CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC   cmdExtDrvMgmtIsrWriteRegister             = (CPSS_EXTDRV_MGMT_ISR_WRITE_REGISTER_FUNC) extServiceFuncNotImplementedCalled;


CPSS_OS_INET_NTOHL_FUNC  cmdOsNtohl                                                  = (CPSS_OS_INET_NTOHL_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_INET_HTONL_FUNC  cmdOsHtonl                                                  = (CPSS_OS_INET_HTONL_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_INET_NTOHS_FUNC  cmdOsNtohs                                                  = (CPSS_OS_INET_NTOHS_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_INET_HTONS_FUNC  cmdOsHtons                                                  = (CPSS_OS_INET_HTONS_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_INET_NTOA_FUNC   cmdOsInetNtoa                                               = (CPSS_OS_INET_NTOA_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_INT_ENABLE_FUNC    cmdOsIntEnable                                            = (CPSS_OS_INT_ENABLE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_INT_DISABLE_FUNC   cmdOsIntDisable                                           = (CPSS_OS_INT_DISABLE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_INT_MODE_SET_FUNC  cmdOsSetIntLockUnlock                                     = (CPSS_OS_INT_MODE_SET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_INT_CONNECT_FUNC   cmdOsInterruptConnect                                     = (CPSS_OS_INT_CONNECT_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_IO_BIND_STDOUT_FUNC cmdOsBindStdOut                                          = (CPSS_OS_IO_BIND_STDOUT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_IO_PRINTF_FUNC      cmdOsPrintf                                              = (CPSS_OS_IO_PRINTF_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_IO_SPRINTF_FUNC     cmdOsSprintf                                             = (CPSS_OS_IO_SPRINTF_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_IO_PRINT_SYNC_FUNC  cmdOsPrintSync                                           = (CPSS_OS_IO_PRINT_SYNC_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_IO_GETS_FUNC        cmdOsGets                                                = (CPSS_OS_IO_GETS_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_IO_FOPEN_FUNC       cmdOsFopen                                               = (CPSS_OS_IO_FOPEN_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_IO_FCLOSE_FUNC      cmdOsFclose                                              = (CPSS_OS_IO_FCLOSE_FUNC)extServiceFuncNotImplementedCalled;
CPSS_OS_IO_REWIND_FUNC      cmdOsRewind                                              = (CPSS_OS_IO_REWIND_FUNC)extServiceFuncNotImplementedCalled;
CPSS_OS_IO_FPRINTF_FUNC     cmdOsFprintf                                             = (CPSS_OS_IO_FPRINTF_FUNC)extServiceFuncNotImplementedCalled;
CPSS_OS_IO_FGETS_FUNC       cmdOsFgets                                               = (CPSS_OS_IO_FGETS_FUNC)extServiceFuncNotImplementedCalled;

CPSS_OS_BZERO_FUNC             cmdOsBzero                                            = (CPSS_OS_BZERO_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_MEM_SET_FUNC           cmdOsMemSet                                           = (CPSS_OS_MEM_SET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_MEM_CPY_FUNC           cmdOsMemCpy                                           = (CPSS_OS_MEM_CPY_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_MEM_CMP_FUNC           cmdOsMemCmp                                           = (CPSS_OS_MEM_CMP_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STATIC_MALLOC_FUNC     cmdOsStaticMalloc                                     = (CPSS_OS_STATIC_MALLOC_FUNC) extServiceFuncNotImplementedCalled;
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    CPSS_OS_MALLOC_FUNC            cmdOsMalloc_MemoryLeakageDbg                      = (CPSS_OS_MALLOC_FUNC) extServiceFuncNotImplementedCalled;
    CPSS_OS_REALLOC_FUNC           cmdOsRealloc_MemoryLeakageDbg                     = (CPSS_OS_REALLOC_FUNC) extServiceFuncNotImplementedCalled;
    CPSS_OS_FREE_FUNC              cmdOsFree_MemoryLeakageDbg                        = (CPSS_OS_FREE_FUNC) extServiceFuncNotImplementedCalled;
#else  /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    CPSS_OS_MALLOC_FUNC            cmdOsMalloc                                       = (CPSS_OS_MALLOC_FUNC) extServiceFuncNotImplementedCalled;
    CPSS_OS_REALLOC_FUNC           cmdOsRealloc                                      = (CPSS_OS_REALLOC_FUNC) extServiceFuncNotImplementedCalled;
    CPSS_OS_FREE_FUNC              cmdOsFree                                         = (CPSS_OS_FREE_FUNC) extServiceFuncNotImplementedCalled;
#endif /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/

CPSS_OS_CACHE_DMA_MALLOC_FUNC  cmdOsCacheDmaMalloc                                   = (CPSS_OS_CACHE_DMA_MALLOC_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_CACHE_DMA_FREE_FUNC    cmdOsCacheDmaFree                                     = (CPSS_OS_CACHE_DMA_FREE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_PHY_TO_VIRT_FUNC       cmdOsPhy2Virt                                         = (CPSS_OS_PHY_TO_VIRT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_VIRT_TO_PHY_FUNC       cmdOsVirt2Phy                                         = (CPSS_OS_VIRT_TO_PHY_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_RAND_FUNC  cmdOsRand                                                         = (CPSS_OS_RAND_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_SRAND_FUNC cmdOsSrand                                                        = (CPSS_OS_SRAND_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_MUTEX_CREATE_FUNC        cmdOsMutexCreate                                    = (CPSS_OS_MUTEX_CREATE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_MUTEX_DELETE_FUNC        cmdOsMutexDelete                                    = (CPSS_OS_MUTEX_DELETE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_MUTEX_LOCK_FUNC          cmdOsMutexLock                                      = (CPSS_OS_MUTEX_LOCK_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_MUTEX_UNLOCK_FUNC        cmdOsMutexUnlock                                    = (CPSS_OS_MUTEX_UNLOCK_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_SIG_SEM_BIN_CREATE_FUNC  cmdOsSigSemBinCreate                                = (CPSS_OS_SIG_SEM_BIN_CREATE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_SIG_SEM_DELETE_FUNC      cmdOsSigSemDelete                                   = (CPSS_OS_SIG_SEM_DELETE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_SIG_SEM_WAIT_FUNC        cmdOsSigSemWait                                     = (CPSS_OS_SIG_SEM_WAIT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_SIG_SEM_SIGNAL_FUNC      cmdOsSigSemSignal                                   = (CPSS_OS_SIG_SEM_SIGNAL_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_STR_LEN_FUNC     cmdOsStrlen                                                 = (CPSS_OS_STR_LEN_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_CPY_FUNC     cmdOsStrCpy                                                 = (CPSS_OS_STR_CPY_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_CHR_FUNC     cmdOsStrChr                                                 = (CPSS_OS_STR_CHR_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_REV_CHR_FUNC cmdOsStrRevChr                                              = (CPSS_OS_STR_REV_CHR_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_CMP_FUNC     cmdOsStrCmp                                                 = (CPSS_OS_STR_CMP_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_CAT_FUNC     cmdOsStrCat                                                 = (CPSS_OS_STR_CAT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_N_CAT_FUNC   cmdOsStrNCat                                                = (CPSS_OS_STR_N_CAT_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_TO_UPPER_FUNC    cmdOsToUpper                                                = (CPSS_OS_TO_UPPER_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_TO_32_FUNC   cmdOsStrTo32                                                = (CPSS_OS_STR_TO_32_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_STR_TO_U32_FUNC  cmdOsStrToU32                                               = (CPSS_OS_STR_TO_U32_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_TIME_WK_AFTER_FUNC  cmdOsTimerWkAfter                                        = (CPSS_OS_TIME_WK_AFTER_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_TIME_TICK_GET_FUNC  cmdOsTickGet                                             = (CPSS_OS_TIME_TICK_GET_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_TIME_GET_FUNC       cmdOsTime                                                = (CPSS_OS_TIME_GET_FUNC) extServiceFuncNotImplementedCalled;

CPSS_OS_TASK_CREATE_FUNC    cmdOsTaskCreate                                          = (CPSS_OS_TASK_CREATE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_TASK_DELETE_FUNC    cmdOsTaskDelete                                          = (CPSS_OS_TASK_DELETE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_TASK_LOCK_FUNC      cmdOsTaskLock                                            = (CPSS_OS_TASK_LOCK_FUNC) extServiceFuncNotImplementedCalled;
CPSS_OS_TASK_UNLOCK_FUNC    cmdOsTaskUnLock                                          = (CPSS_OS_TASK_UNLOCK_FUNC) extServiceFuncNotImplementedCalled;

CPSS_TRACE_HW_ACCESS_WRITE_FUNC     cmdTraceHwAccessWrite                            = (CPSS_TRACE_HW_ACCESS_WRITE_FUNC) extServiceFuncNotImplementedCalled;
CPSS_TRACE_HW_ACCESS_READ_FUNC      cmdTraceHwAccessRead                             = (CPSS_TRACE_HW_ACCESS_READ_FUNC) extServiceFuncNotImplementedCalled;
CPSS_TRACE_HW_ACCESS_DELAY_FUNC      cmdTraceHwAccessDelay                           = (CPSS_TRACE_HW_ACCESS_DELAY_FUNC) extServiceFuncNotImplementedCalled;

CMD_OS_TASK_GET_SELF_FUN    cmdOsTaskGetSelf                                         = (CMD_OS_TASK_GET_SELF_FUN) extServiceFuncNotImplementedCalled;
CMD_OS_TASK_SET_TASK_PRIOR_FUNC cmdOsSetTaskPrior                                    = (CMD_OS_TASK_SET_TASK_PRIOR_FUNC) extServiceFuncNotImplementedCalled;


CMD_OS_SOCKET_TCP_CREATE_FUN        cmdOsSocketTcpCreate                                      = (CMD_OS_SOCKET_TCP_CREATE_FUN     ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_UDP_CREATE_FUN        cmdOsSocketUdpCreate                                      = (CMD_OS_SOCKET_UDP_CREATE_FUN     ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_TCP_DESTROY_FUN       cmdOsSocketTcpDestroy                                     = (CMD_OS_SOCKET_TCP_DESTROY_FUN    ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_UDP_DESTROY_FUN       cmdOsSocketUdpDestroy                                     = (CMD_OS_SOCKET_UDP_DESTROY_FUN    ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_CREATE_ADDR_FUN       cmdOsSocketCreateAddr                                     = (CMD_OS_SOCKET_CREATE_ADDR_FUN    ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_DESTROY_ADDR_FUN      cmdOsSocketDestroyAddr                                    = (CMD_OS_SOCKET_DESTROY_ADDR_FUN   ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_BIND_FUN              cmdOsSocketBind                                           = (CMD_OS_SOCKET_BIND_FUN           ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_LISTEN_FUN            cmdOsSocketListen                                         = (CMD_OS_SOCKET_LISTEN_FUN         ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_ACCEPT_FUN            cmdOsSocketAccept                                         = (CMD_OS_SOCKET_ACCEPT_FUN         ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_CONNECT_FUN           cmdOsSocketConnect                                        = (CMD_OS_SOCKET_CONNECT_FUN        ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_SET_NON_BLOCK_FUN     cmdOsSocketSetNonBlock                                    = (CMD_OS_SOCKET_SET_NON_BLOCK_FUN  ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_SET_BLOCK_FUN         cmdOsSocketSetBlock                                       = (CMD_OS_SOCKET_SET_BLOCK_FUN      ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_SEND_FUN              cmdOsSocketSend                                           = (CMD_OS_SOCKET_SEND_FUN           ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_SEND_TO_FUN           cmdOsSocketSendTo                                         = (CMD_OS_SOCKET_SEND_TO_FUN        ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_RECV_FUN              cmdOsSocketRecv                                           = (CMD_OS_SOCKET_RECV_FUN           ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_RECV_FROM_FUN         cmdOsSocketRecvFrom                                       = (CMD_OS_SOCKET_RECV_FROM_FUN      ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_SET_SOCKET_NO_LINGER_FUN cmdOsSocketSetSocketNoLinger                           = (CMD_OS_SOCKET_SET_SOCKET_NO_LINGER_FUN) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_EXTRACT_IP_ADDR_FROM_SOCKET_ADDR_FUN cmdOsSocketExtractIpAddrFromSocketAddr     = (CMD_OS_SOCKET_EXTRACT_IP_ADDR_FROM_SOCKET_ADDR_FUN) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_GET_SOCKET_ADDR_SIZE_FUN cmdOsSocketGetSocketAddrSize                           = (CMD_OS_SOCKET_GET_SOCKET_ADDR_SIZE_FUN) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_SHUT_DOWN_FUN         cmdOsSocketShutDown                                       = (CMD_OS_SOCKET_SHUT_DOWN_FUN) extServiceFuncNotImplementedCalled;

CMD_OS_SELECT_CREATE_SET_FUN        cmdOsSelectCreateSet                                      = (CMD_OS_SELECT_CREATE_SET_FUN       ) extServiceFuncNotImplementedCalled;
CMD_OS_SELECT_ERASE_SET_FUN         cmdOsSelectEraseSet                                       = (CMD_OS_SELECT_ERASE_SET_FUN        ) extServiceFuncNotImplementedCalled;
CMD_OS_SELECT_ZERO_SET_FUN          cmdOsSelectZeroSet                                        = (CMD_OS_SELECT_ZERO_SET_FUN         ) extServiceFuncNotImplementedCalled;
CMD_OS_SELECT_ADD_FD_TO_SET_FUN     cmdOsSelectAddFdToSet                                     = (CMD_OS_SELECT_ADD_FD_TO_SET_FUN    ) extServiceFuncNotImplementedCalled;
CMD_OS_SELECT_CLEAR_FD_FROM_SET_FUN cmdOsSelectClearFdFromSet                                 = (CMD_OS_SELECT_CLEAR_FD_FROM_SET_FUN) extServiceFuncNotImplementedCalled;
CMD_OS_SELECT_IS_FD_SET_FUN         cmdOsSelectIsFdSet                                        = (CMD_OS_SELECT_IS_FD_SET_FUN        ) extServiceFuncNotImplementedCalled;
CMD_OS_SELECT_COPY_SET_FUN          cmdOsSelectCopySet                                        = (CMD_OS_SELECT_COPY_SET_FUN         ) extServiceFuncNotImplementedCalled;
CMD_OS_SELECT_FUN                   cmdOsSelect                                               = (CMD_OS_SELECT_FUN                  ) extServiceFuncNotImplementedCalled;
CMD_OS_SOCKET_GET_SOCKET_FD_SET_SIZE_FUN cmdOsSocketGetSocketFdSetSize                        = (CMD_OS_SOCKET_GET_SOCKET_FD_SET_SIZE_FUN) extServiceFuncNotImplementedCalled;


CMD_OS_POOL_CREATE_POOL_FUNC        cmdOsPoolCreatePool                                       = (CMD_OS_POOL_CREATE_POOL_FUNC     ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_CREATE_DMA_POOL_FUNC    cmdOsPoolCreateDmaPool                                    = (CMD_OS_POOL_CREATE_DMA_POOL_FUNC ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_RE_CREATE_POOL_FUNC     cmdOsPoolReCreatePool                                     = (CMD_OS_POOL_RE_CREATE_POOL_FUNC  ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_DELETE_POOL_FUNC        cmdOsPoolDeletePool                                       = (CMD_OS_POOL_DELETE_POOL_FUNC     ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_GET_BUF_FUNC            cmdOsPoolGetBuf                                           = (CMD_OS_POOL_GET_BUF_FUNC         ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_FREE_BUF_FUNC           cmdOsPoolFreeBuf                                          = (CMD_OS_POOL_FREE_BUF_FUNC        ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_GET_BUF_SIZE_FUNC       cmdOsPoolGetBufSize                                       = (CMD_OS_POOL_GET_BUF_SIZE_FUNC    ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_EXPAND_POOL_FUNC        cmdOsPoolExpandPool                                       = (CMD_OS_POOL_EXPAND_POOL_FUNC     ) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_GET_BUF_FREE_CNT_FUNC   cmdOsPoolGetBufFreeCnt                                    = (CMD_OS_POOL_GET_BUF_FREE_CNT_FUNC) extServiceFuncNotImplementedCalled;
CMD_OS_POOL_PRINT_STATS_FUNC        cmdOsPoolPrintStats                                       = (CMD_OS_POOL_PRINT_STATS_FUNC     ) extServiceFuncNotImplementedCalled;

CMD_CPU_ETHERNET_IS_CPU_ETHER_PORT_USED cmdIsCpuEtherPortUsed                                 = (CMD_CPU_ETHERNET_IS_CPU_ETHER_PORT_USED) extServiceFuncNotImplementedCalled;

CMD_CPSS_EVENT_BIND_FUNC            cmdCpssEventBind                                          = (CMD_CPSS_EVENT_BIND_FUNC           ) extServiceFuncNotImplementedCalled;
CMD_CPSS_EVENT_SELECT_FUNC          cmdCpssEventSelect                                        = (CMD_CPSS_EVENT_SELECT_FUNC         ) extServiceFuncNotImplementedCalled;
CMD_CPSS_EVENT_RECV_FUNC            cmdCpssEventRecv                                          = (CMD_CPSS_EVENT_RECV_FUNC           ) extServiceFuncNotImplementedCalled;
CMD_CPSS_EVENT_DEVICE_MASK_SET_FUNC cmdCpssEventDeviceMaskSet                                 = (CMD_CPSS_EVENT_DEVICE_MASK_SET_FUNC) extServiceFuncNotImplementedCalled;


CMD_APP_DB_ENTRY_ADD_FUNC       cmdAppDbEntryAdd                                              = (CMD_APP_DB_ENTRY_ADD_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_DB_ENTRY_GET_FUNC       cmdAppDbEntryGet                                              = (CMD_APP_DB_ENTRY_GET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_DB_DUMP_FUNC            cmdAppDbDump                                                  = (CMD_APP_DB_DUMP_FUNC     ) extServiceFuncNotImplementedCalled;
CMD_APP_PP_CONFIG_GET_FUNC      cmdAppPpConfigGet                                             = (CMD_APP_PP_CONFIG_GET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_INIT_SYSTEM_FUNC        cmdInitSystem                                                 = (CMD_APP_INIT_SYSTEM_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_INIT_SYSTEM_GET_FUNC    cmdInitSystemGet                                              = (CMD_APP_INIT_SYSTEM_GET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_RESET_SYSTEM_FUNC       cmdResetSystem                                                = (CMD_APP_RESET_SYSTEM_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_INIT_CONFI_FUNC         cmdInitConfi                                                 = (CMD_APP_INIT_CONFI_FUNC)   extServiceFuncNotImplementedCalled;
CMD_APP_SHOW_BOARDS_LIST_FUNC   cmdAppShowBoardsList                                          = (CMD_APP_SHOW_BOARDS_LIST_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_IS_SYSTEM_INITIALIZED_FUNC  cmdAppIsSystemInitialized                                 = (CMD_APP_IS_SYSTEM_INITIALIZED_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_PP_CONFIG_PRINT_FUNC  cmdAppPpConfigPrint                                             = (CMD_APP_PP_CONFIG_PRINT_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_ALLOW_PROCESSING_OF_AUQ_MESSAGES_FUNC cmdAppAllowProcessingOfAuqMessages              = (CMD_APP_ALLOW_PROCESSING_OF_AUQ_MESSAGES_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_TRACE_HW_ACCESS_ENABLE_FUNC cmdAppTraceHwAccessEnable                                 = (CMD_APP_TRACE_HW_ACCESS_ENABLE_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_TRACE_HW_ACCESS_OUTPUT_MODE_SET_FUNC cmdAppTraceHwAccessOutputModeSet                 = (CMD_APP_TRACE_HW_ACCESS_OUTPUT_MODE_SET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_START_HEAP_ALLOC_COUNTER_FUNC cmdAppStartHeapAllocCounter                             = (CMD_APP_START_HEAP_ALLOC_COUNTER_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_GET_HEAP_ALLOC_COUNTER_FUNC   cmdAppGetHeapAllocCounter                               = (CMD_APP_GET_HEAP_ALLOC_COUNTER_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_EVANT_FATAL_ERROR_ENABLE_FUNC cmdAppEventFatalErrorEnable                             = (CMD_APP_EVANT_FATAL_ERROR_ENABLE_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_EVANT_DATA_BASE_GET_FUNC cmdAppDemoEventsDataBaseGet                                  = (CMD_APP_EVANT_DATA_BASE_GET_FUNC) extServiceFuncNotImplementedCalled;

#ifdef EXMXPM_FAMILY
CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_INDEX_BY_LEN_AND_LOOKUP_GET_FUNC  cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet = (CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_INDEX_BY_LEN_AND_LOOKUP_GET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_ACTION_INDEX_GET_FUNC                  cmdAppExMxPmExternalTcamActionIndexGet             = (CMD_APP_EXMXPM_EXTERNAL_TCAM_ACTION_INDEX_GET_FUNC                ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_AND_ACTION_INDEX_GET_FUNC         cmdAppExMxPmExternalTcamRuleAndActionIndexGet      = (CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_AND_ACTION_INDEX_GET_FUNC       ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_FUNC             cmdAppExMxPmExternalTcamClientKeyLookupSet         = (CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_FUNC           ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_FUNC             cmdAppExMxPmExternalTcamClientKeyLookupGet         = (CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_FUNC           ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_BY_INDEX_FUNC    cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex  = (CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_SET_BY_INDEX_FUNC  ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_BY_INDEX_FUNC    cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex  = (CMD_APP_EXMXPM_EXTERNAL_TCAM_CLIENT_KEY_LOOKUP_GET_BY_INDEX_FUNC  ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_CONFIG_FUNC                            cmdAppExMxPmExternalTcamConfig                     = (CMD_APP_EXMXPM_EXTERNAL_TCAM_CONFIG_FUNC                          ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_STATE_GET_FUNC                    cmdAppExMxPmExternalTcamRuleStateGet               = (CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_STATE_GET_FUNC                  ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_VALID_STATUS_SET_FUNC             cmdAppExMxPmExternalTcamRuleValidStatusSet         = (CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_VALID_STATUS_SET_FUNC           ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_AND_ACTION_COPY_FUNC              cmdAppExMxPmExternalTcamRuleAndActionCopy          = (CMD_APP_EXMXPM_EXTERNAL_TCAM_RULE_AND_ACTION_COPY_FUNC            ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_HSU_OLD_IMAGE_PRE_UPDATE_PREPARATION_FUNC   cmdAppExMxPmHsuOldImagePreUpdatePreparation        = (CMD_APP_EXMXPM_EXTERNAL_HSU_OLD_IMAGE_PRE_UPDATE_PREPARATION_FUNC ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_EXTERNAL_HSU_NEW_IMAGE_UPDATE_AND_SYNCH_FUNC         cmdAppExMxPmHsuNewImageUpdateAndSynch              = (CMD_APP_EXMXPM_EXTERNAL_HSU_NEW_IMAGE_UPDATE_AND_SYNCH_FUNC       ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_PUMA3_PORT_MODE_SPEED_SET_FUNC                       cmdAppPuma3PortModeSpeedSet                        = (CMD_APP_EXMXPM_PUMA3_PORT_MODE_SPEED_SET_FUNC                     ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_PUMA3_PORT_MODE_SPEED_SET_EXT_FUNC                   cmdAppPuma3PortModeSpeedSet_ext                    = (CMD_APP_EXMXPM_PUMA3_PORT_MODE_SPEED_SET_EXT_FUNC                 ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_PUMA3_BOARD_TYPE_SET_FUNC                            cmdAppPuma3BoardTypeSet                            = (CMD_APP_EXMXPM_PUMA3_BOARD_TYPE_SET_FUNC                          ) extServiceFuncNotImplementedCalled;
CMD_APP_EXMXPM_PUMA3_LOOPBACK_PRE_INIT_SET_FUNC                     cmdAppPuma3LoopbackEnableSet                       = (CMD_APP_EXMXPM_PUMA3_LOOPBACK_PRE_INIT_SET_FUNC                   ) extServiceFuncNotImplementedCalled;
#endif /*EXMXPM_FAMILY*/

#ifdef CHX_FAMILY
CMD_APP_PORT_RATE_TABLE_GET_FUNC                                    cmdPortRateTableGet = (CMD_APP_PORT_RATE_TABLE_GET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_LION_PORT_MODE_SPEED_SET_FUNC                               cmdAppLionPortModeSpeedSet = (CMD_APP_LION_PORT_MODE_SPEED_SET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_XCAT2_STACK_PORTS_MODE_SPEED_SET_FUNC                       cmdAppXcat2StackPortsModeSpeedSet = (CMD_APP_LION_PORT_MODE_SPEED_SET_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_XCAT2_SFP_PORT_ENABLE_FUNC                                  cmdAppXcat2SfpPortEnable = (CMD_APP_XCAT2_SFP_PORT_ENABLE_FUNC) extServiceFuncNotImplementedCalled;
CMD_APP_DXCH_EXTERNAL_HSU_OLD_IMAGE_PRE_UPDATE_PREPARATION_FUNC     cmdAppDxChHsuOldImagePreUpdatePreparation        = (CMD_APP_DXCH_EXTERNAL_HSU_OLD_IMAGE_PRE_UPDATE_PREPARATION_FUNC ) extServiceFuncNotImplementedCalled;
CMD_APP_DXCH_EXTERNAL_HSU_NEW_IMAGE_UPDATE_AND_SYNCH_FUNC           cmdAppDxChHsuNewImageUpdateAndSynch              = (CMD_APP_DXCH_EXTERNAL_HSU_NEW_IMAGE_UPDATE_AND_SYNCH_FUNC       ) extServiceFuncNotImplementedCalled;
CMD_APP_TRUNK_CASCADE_TRUNK_PORTS_SET_FUNC                          cmdAppTrunkCascadeTrunkPortsSet = 0;
CMD_APP_TRUNK_MEMBERS_SET_FUNC                                      cmdAppTrunkMembersSet = 0;
CMD_APP_TRUNK_MEMBER_ADD_FUNC                                       cmdAppTrunkMemberAdd = 0;
CMD_APP_TRUNK_MEMBER_DISABLE_FUNC                                   cmdAppTrunkMemberDisable = 0;
CMD_APP_TRUNK_MEMBER_ENABLE_FUNC                                    cmdAppTrunkMemberEnable = 0;
CMD_APP_TRUNK_MEMBER_REMOVE_FUNC                                    cmdAppTrunkMemberRemove = 0;
CMD_APP_TRUNK_UPDATED_PORTS_GET_FUNC                                cmdAppTrunkUpdatedPortsGet = 0;
#endif /*CHX_FAMILY*/


/*******************************************************************************
* cmdExtServicesDump
*
* DESCRIPTION:
*       print which one of the external services was not bound
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdExtServicesDump
(
    void
)
{
    GT_U32  ii,jj;
    void*   *currPtr;
    GT_U32  sizes[] = {
                sizeof(cmdExtDrvFuncBindInfo) / sizeof(void*),
                sizeof(cmdOsFuncBind        ) / sizeof(void*),
                sizeof(cmdOsExtraFuncBind   ) / sizeof(void*),
                sizeof(cmdExtraFuncBind     ) / sizeof(void*),
                0};
    void**  ptrArray[]={
                (void*)&cmdExtDrvFuncBindInfo,
                (void*)&cmdOsFuncBind        ,
                (void*)&cmdOsExtraFuncBind   ,
                (void*)&cmdExtraFuncBind     ,
                0};


    jj=0;
    do
    {
        currPtr = ptrArray[jj];

        for(ii = 0 ; ii < sizes[jj] ; ii++ , currPtr++)
        {
            if((*currPtr) == (void*)extServiceFuncNotImplementedCalled)
            {
                cmdOsPrintf("in section [%ld] , function index [%ld] was not bound \n ",
                    jj,ii);
            }
            else if((*currPtr) == NULL)
            {
                cmdOsPrintf("in section [%ld] , function index [%ld] was given NULL \n ",
                    jj,ii);
            }

        }

        jj++;
    }while(sizes[++jj]);


    return GT_OK;

}

/*******************************************************************************
* cmdInitExtServices
*
* DESCRIPTION:
*       commander external services initialization
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdInitExtServices
(
    void
)
{
    static GT_BOOL cmdInitExtServices_done = GT_FALSE;
    if (cmdInitExtServices_done == GT_TRUE)
    {
        return GT_OK;
    }
    if(GT_OK != applicationExtServicesBind(&cmdExtDrvFuncBindInfo,&cmdOsFuncBind,&cmdOsExtraFuncBind,&cmdExtraFuncBind, &cmdTraceFuncBind))
    {
        return GT_FAIL;
    }
    SET_NON_NULL(cmdExtDrvMgmtCacheFlush             , cmdExtDrvFuncBindInfo.extDrvMgmtCacheBindInfo.extDrvMgmtCacheFlush                          );
    SET_NON_NULL(cmdExtDrvMgmtCacheInvalidate        , cmdExtDrvFuncBindInfo.extDrvMgmtCacheBindInfo.extDrvMgmtCacheInvalidate                     );

    SET_NON_NULL(cmdExtDrvDmaWrite                   , cmdExtDrvFuncBindInfo.extDrvDmaBindInfo.extDrvDmaWriteDriverFunc                            );
    SET_NON_NULL(cmdExtDrvDmaRead                    , cmdExtDrvFuncBindInfo.extDrvDmaBindInfo.extDrvDmaReadFunc                                   );

    SET_NON_NULL(cmdExtDrvEthPortRxInit              , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortRxInitFunc                         );
    SET_NON_NULL(cmdExtDrvEthPortTxInit              , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortTxInitFunc                         );
    SET_NON_NULL(cmdExtDrvEthPortEnable              , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortEnableFunc                         );
    SET_NON_NULL(cmdExtDrvEthPortDisable             , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortDisableFunc                        );
    SET_NON_NULL(cmdExtDrvEthPortTx                  , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortTxFunc                             );
    SET_NON_NULL(cmdExtDrvEthInputHookAdd            , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortInputHookAddFunc                   );
    SET_NON_NULL(cmdExtDrvEthTxCompleteHookAdd       , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortTxCompleteHookAddFunc              );
    SET_NON_NULL(cmdExtDrvEthRxPacketFree            , cmdExtDrvFuncBindInfo.extDrvEthPortBindInfo.extDrvEthPortRxPacketFreeFunc                   );

    SET_NON_NULL(cmdExtDrvIntConnect                 , cmdExtDrvFuncBindInfo.extDrvIntBindInfo.extDrvIntConnectFunc                                );
    SET_NON_NULL(cmdExtDrvIntEnable                  , cmdExtDrvFuncBindInfo.extDrvIntBindInfo.extDrvIntEnableFunc                                 );
    SET_NON_NULL(cmdExtDrvIntDisable                 , cmdExtDrvFuncBindInfo.extDrvIntBindInfo.extDrvIntDisableFunc                                );
    SET_NON_NULL(cmdExtDrvSetIntLockUnlock           , cmdExtDrvFuncBindInfo.extDrvIntBindInfo.extDrvIntLockModeSetFunc                            );

    SET_NON_NULL(cmdExtDrvPciConfigWriteReg          , cmdExtDrvFuncBindInfo.extDrvPciInfo.extDrvPciConfigWriteRegFunc                             );
    SET_NON_NULL(cmdExtDrvPciConfigReadReg           , cmdExtDrvFuncBindInfo.extDrvPciInfo.extDrvPciConfigReadRegFunc                              );
    SET_NON_NULL(cmdExtDrvPciFindDev                 , cmdExtDrvFuncBindInfo.extDrvPciInfo.extDrvPciDevFindFunc                                    );
    SET_NON_NULL(cmdExtDrvGetPciIntVec               , cmdExtDrvFuncBindInfo.extDrvPciInfo.extDrvPciIntVecFunc                                     );
    SET_NON_NULL(cmdExtDrvGetIntMask                 , cmdExtDrvFuncBindInfo.extDrvPciInfo.extDrvPciIntMaskFunc                                    );
    SET_NON_NULL(cmdExtDrvEnableCombinedPciAccess    , cmdExtDrvFuncBindInfo.extDrvPciInfo.extDrvPciCombinedAccessEnableFunc                       );

    SET_NON_NULL(cmdExtDrvHwIfSmiInitDriver          , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiInitDriverFunc                     );
    SET_NON_NULL(cmdExtDrvHwIfSmiWriteReg            , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiWriteRegFunc                       );
    SET_NON_NULL(cmdExtDrvHwIfSmiReadReg             , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiReadRegFunc                        );
    SET_NON_NULL(cmdExtDrvHwIfSmiTskRegRamRead       , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamReadFunc                 );
    SET_NON_NULL(cmdExtDrvHwIfSmiTskRegRamWrite      , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamWriteFunc                );
    SET_NON_NULL(cmdExtDrvHwIfSmiTskRegVecRead       , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecReadFunc                 );
    SET_NON_NULL(cmdExtDrvHwIfSmiTskRegVecWrite      , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecWriteFunc                );
    SET_NON_NULL(cmdExtDrvHwIfSmiTaskWriteReg        , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteRegFunc                   );
    SET_NON_NULL(cmdExtDrvHwIfSmiTaskReadReg         , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskReadRegFunc                    );
    SET_NON_NULL(cmdExtDrvHwIfSmiInterruptWriteReg   , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntWriteRegFunc                    );
    SET_NON_NULL(cmdExtDrvHwIfSmiInterruptReadReg    , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntReadRegFunc                     );
    SET_NON_NULL(cmdExtDrvSmiDevVendorIdGet          , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiDevVendorIdGetFunc                 );
    SET_NON_NULL(cmdExtDrvHwIfSmiTaskWriteRegField   , cmdExtDrvFuncBindInfo.extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteFieldFunc                 );

    SET_NON_NULL(cmdExtDrvHwIfTwsiInitDriver         , cmdExtDrvFuncBindInfo.extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiInitDriverFunc                   );
    SET_NON_NULL(cmdExtDrvHwIfTwsiWriteReg           , cmdExtDrvFuncBindInfo.extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiWriteRegFunc                     );
    SET_NON_NULL(cmdExtDrvHwIfTwsiReadReg            , cmdExtDrvFuncBindInfo.extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiReadRegFunc                      );

    SET_NON_NULL(cmdExtDrvI2cMgmtMasterInit          , cmdExtDrvFuncBindInfo.extDrvMgmtHwIfBindInfo.extDrvI2cMgmtMasterInitFunc                    );
    SET_NON_NULL(cmdExtDrvMgmtReadRegister           , cmdExtDrvFuncBindInfo.extDrvMgmtHwIfBindInfo.extDrvMgmtReadRegisterFunc                     );
    SET_NON_NULL(cmdExtDrvMgmtWriteRegister          , cmdExtDrvFuncBindInfo.extDrvMgmtHwIfBindInfo.extDrvMgmtWriteRegisterFunc                    );
    SET_NON_NULL(cmdExtDrvMgmtIsrReadRegister        , cmdExtDrvFuncBindInfo.extDrvMgmtHwIfBindInfo.extDrvMgmtIsrReadRegisterFunc                  );
    SET_NON_NULL(cmdExtDrvMgmtIsrWriteRegister       , cmdExtDrvFuncBindInfo.extDrvMgmtHwIfBindInfo.extDrvMgmtIsrWriteRegisterFunc                 );


    SET_NON_NULL(cmdOsNtohl                          , cmdOsFuncBind.osInetBindInfo.osInetNtohlFunc         );
    SET_NON_NULL(cmdOsHtonl                          , cmdOsFuncBind.osInetBindInfo.osInetHtonlFunc         );
    SET_NON_NULL(cmdOsNtohs                          , cmdOsFuncBind.osInetBindInfo.osInetNtohsFunc         );
    SET_NON_NULL(cmdOsHtons                          , cmdOsFuncBind.osInetBindInfo.osInetHtonsFunc         );
    SET_NON_NULL(cmdOsInetNtoa                       , cmdOsFuncBind.osInetBindInfo.osInetNtoaFunc          );

    SET_NON_NULL(cmdOsIntEnable                      , cmdOsFuncBind.osIntBindInfo.osIntEnableFunc          );
    SET_NON_NULL(cmdOsIntDisable                     , cmdOsFuncBind.osIntBindInfo.osIntDisableFunc         );
    SET_NON_NULL(cmdOsSetIntLockUnlock               , cmdOsFuncBind.osIntBindInfo.osIntModeSetFunc         );
    SET_NON_NULL(cmdOsInterruptConnect               , cmdOsFuncBind.osIntBindInfo.osIntConnectFunc         );

    SET_NON_NULL(cmdOsBindStdOut                     , cmdOsFuncBind.osIoBindInfo.osIoBindStdOutFunc        );
    SET_NON_NULL(cmdOsPrintf                         , cmdOsFuncBind.osIoBindInfo.osIoPrintfFunc            );
    SET_NON_NULL(cmdOsSprintf                        , cmdOsFuncBind.osIoBindInfo.osIoSprintfFunc           );
    SET_NON_NULL(cmdOsPrintSync                      , cmdOsFuncBind.osIoBindInfo.osIoPrintSynchFunc        );
    SET_NON_NULL(cmdOsGets                           , cmdOsFuncBind.osIoBindInfo.osIoGetsFunc              );

    SET_NON_NULL(cmdOsFopen                          , cmdOsFuncBind.osIoBindInfo.osIoFopenFunc             );
    SET_NON_NULL(cmdOsFclose                         , cmdOsFuncBind.osIoBindInfo.osIoFcloseFunc            );
    SET_NON_NULL(cmdOsRewind                         , cmdOsFuncBind.osIoBindInfo.osIoRewindFunc            );
    SET_NON_NULL(cmdOsFprintf                        , cmdOsFuncBind.osIoBindInfo.osIoFprintfFunc           );
    SET_NON_NULL(cmdOsFgets                          , cmdOsFuncBind.osIoBindInfo.osIoFgets                 );

    SET_NON_NULL(cmdOsBzero                          , cmdOsFuncBind.osMemBindInfo.osMemBzeroFunc           );
    SET_NON_NULL(cmdOsMemSet                         , cmdOsFuncBind.osMemBindInfo.osMemSetFunc             );
    SET_NON_NULL(cmdOsMemCpy                         , cmdOsFuncBind.osMemBindInfo.osMemCpyFunc             );
    SET_NON_NULL(cmdOsMemCmp                         , cmdOsFuncBind.osMemBindInfo.osMemCmpFunc             );
    SET_NON_NULL(cmdOsStaticMalloc                   , cmdOsFuncBind.osMemBindInfo.osMemStaticMallocFunc    );
#ifdef OS_MALLOC_MEMORY_LEAKAGE_DBG
    SET_NON_NULL(cmdOsMalloc_MemoryLeakageDbg        , cmdOsFuncBind.osMemBindInfo.osMemMallocFunc          );
    SET_NON_NULL(cmdOsRealloc_MemoryLeakageDbg       , cmdOsFuncBind.osMemBindInfo.osMemReallocFunc         );
    SET_NON_NULL(cmdOsFree_MemoryLeakageDbg          , cmdOsFuncBind.osMemBindInfo.osMemFreeFunc            );
#else  /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    SET_NON_NULL(cmdOsMalloc                         , cmdOsFuncBind.osMemBindInfo.osMemMallocFunc          );
    SET_NON_NULL(cmdOsRealloc                        , cmdOsFuncBind.osMemBindInfo.osMemReallocFunc         );
    SET_NON_NULL(cmdOsFree                           , cmdOsFuncBind.osMemBindInfo.osMemFreeFunc            );
#endif /*!OS_MALLOC_MEMORY_LEAKAGE_DBG*/
    SET_NON_NULL(cmdOsCacheDmaMalloc                 , cmdOsFuncBind.osMemBindInfo.osMemCacheDmaMallocFunc  );
    SET_NON_NULL(cmdOsCacheDmaFree                   , cmdOsFuncBind.osMemBindInfo.osMemCacheDmaFreeFunc    );
    SET_NON_NULL(cmdOsPhy2Virt                       , cmdOsFuncBind.osMemBindInfo.osMemPhyToVirtFunc       );
    SET_NON_NULL(cmdOsVirt2Phy                       , cmdOsFuncBind.osMemBindInfo.osMemVirtToPhyFunc       );

    SET_NON_NULL(cmdOsRand                           , cmdOsFuncBind.osRandBindInfo.osRandFunc              );
    SET_NON_NULL(cmdOsSrand                          , cmdOsFuncBind.osRandBindInfo.osSrandFunc             );

    SET_NON_NULL(cmdOsMutexCreate                    , cmdOsFuncBind.osSemBindInfo.osMutexCreateFunc  );
    SET_NON_NULL(cmdOsMutexDelete                    , cmdOsFuncBind.osSemBindInfo.osMutexDeleteFunc     );
    SET_NON_NULL(cmdOsMutexLock                      , cmdOsFuncBind.osSemBindInfo.osMutexLockFunc       );
    SET_NON_NULL(cmdOsMutexUnlock                    , cmdOsFuncBind.osSemBindInfo.osMutexUnlockFunc     );

    SET_NON_NULL(cmdOsSigSemBinCreate                , cmdOsFuncBind.osSemBindInfo.osSigSemBinCreateFunc    );
    SET_NON_NULL(cmdOsSigSemDelete                   , cmdOsFuncBind.osSemBindInfo.osSigSemDeleteFunc       );
    SET_NON_NULL(cmdOsSigSemWait                     , cmdOsFuncBind.osSemBindInfo.osSigSemWaitFunc         );
    SET_NON_NULL(cmdOsSigSemSignal                   , cmdOsFuncBind.osSemBindInfo.osSigSemSignalFunc       );

    SET_NON_NULL(cmdOsStrlen                         , cmdOsFuncBind.osStrBindInfo.osStrlenFunc             );
    SET_NON_NULL(cmdOsStrCpy                         , cmdOsFuncBind.osStrBindInfo.osStrCpyFunc             );
    SET_NON_NULL(cmdOsStrChr                         , cmdOsFuncBind.osStrBindInfo.osStrChrFunc             );
    SET_NON_NULL(cmdOsStrRevChr                      , cmdOsFuncBind.osStrBindInfo.osStrRevChrFunc          );
    SET_NON_NULL(cmdOsStrCmp                         , cmdOsFuncBind.osStrBindInfo.osStrCmpFunc             );
    SET_NON_NULL(cmdOsStrCat                         , cmdOsFuncBind.osStrBindInfo.osStrCatFunc             );
    SET_NON_NULL(cmdOsStrNCat                        , cmdOsFuncBind.osStrBindInfo.osStrStrNCatFunc         );
    SET_NON_NULL(cmdOsToUpper                        , cmdOsFuncBind.osStrBindInfo.osStrChrToUpperFunc      );
    SET_NON_NULL(cmdOsStrTo32                        , cmdOsFuncBind.osStrBindInfo.osStrTo32Func            );
    SET_NON_NULL(cmdOsStrToU32                       , cmdOsFuncBind.osStrBindInfo.osStrToU32Func           );

    SET_NON_NULL(cmdOsTimerWkAfter                   , cmdOsFuncBind.osTimeBindInfo.osTimeWkAfterFunc       );
    SET_NON_NULL(cmdOsTickGet                        , cmdOsFuncBind.osTimeBindInfo.osTimeTickGetFunc       );
    SET_NON_NULL(cmdOsTime                           , cmdOsFuncBind.osTimeBindInfo.osTimeGetFunc           );

    SET_NON_NULL(cmdOsTaskCreate                     , cmdOsFuncBind.osTaskBindInfo.osTaskCreateFunc        );
    SET_NON_NULL(cmdOsTaskDelete                     , cmdOsFuncBind.osTaskBindInfo.osTaskDeleteFunc        );
    SET_NON_NULL(cmdOsTaskLock                       , cmdOsFuncBind.osTaskBindInfo.osTaskLockFunc          );
    SET_NON_NULL(cmdOsTaskUnLock                     , cmdOsFuncBind.osTaskBindInfo.osTaskUnLockFunc        );

    SET_NON_NULL(cmdOsTaskGetSelf                    , cmdOsExtraFuncBind.osTasksBindInfo.osTaskGetSelf     );
    SET_NON_NULL(cmdOsSetTaskPrior                   , cmdOsExtraFuncBind.osTasksBindInfo.osSetTaskPrior    );

    SET_NON_NULL(cmdOsSocketTcpCreate                , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketTcpCreate                           );
    SET_NON_NULL(cmdOsSocketUdpCreate                , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketUdpCreate                           );
    SET_NON_NULL(cmdOsSocketTcpDestroy               , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketTcpDestroy                          );
    SET_NON_NULL(cmdOsSocketUdpDestroy               , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketUdpDestroy                          );
    SET_NON_NULL(cmdOsSocketCreateAddr               , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketCreateAddr                          );
    SET_NON_NULL(cmdOsSocketDestroyAddr              , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketDestroyAddr                         );
    SET_NON_NULL(cmdOsSocketBind                     , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketBind                                );
    SET_NON_NULL(cmdOsSocketListen                   , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketListen                              );
    SET_NON_NULL(cmdOsSocketAccept                   , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketAccept                              );
    SET_NON_NULL(cmdOsSocketConnect                  , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketConnect                             );
    SET_NON_NULL(cmdOsSocketSetNonBlock              , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketSetNonBlock                         );
    SET_NON_NULL(cmdOsSocketSetBlock                 , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketSetBlock                            );
    SET_NON_NULL(cmdOsSocketSend                     , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketSend                                );
    SET_NON_NULL(cmdOsSocketSendTo                   , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketSendTo                              );
    SET_NON_NULL(cmdOsSocketRecv                     , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketRecv                                );
    SET_NON_NULL(cmdOsSocketRecvFrom                 , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketRecvFrom                            );
    SET_NON_NULL(cmdOsSocketSetSocketNoLinger        , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketSetSocketNoLinger                   );
    SET_NON_NULL(cmdOsSocketExtractIpAddrFromSocketAddr, cmdOsExtraFuncBind.osSocketsBindInfo.osSocketExtractIpAddrFromSocketAddr       );
    SET_NON_NULL(cmdOsSocketGetSocketAddrSize        , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketGetSocketAddrSize                   );
    SET_NON_NULL(cmdOsSocketShutDown                 , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketShutDown                            );
    SET_NON_NULL(cmdOsSelectCreateSet                , cmdOsExtraFuncBind.osSocketsBindInfo.osSelectCreateSet                           );
    SET_NON_NULL(cmdOsSelectEraseSet                 , cmdOsExtraFuncBind.osSocketsBindInfo.osSelectEraseSet                            );
    SET_NON_NULL(cmdOsSelectZeroSet                  , cmdOsExtraFuncBind.osSocketsBindInfo.osSelectZeroSet                             );
    SET_NON_NULL(cmdOsSelectAddFdToSet               , cmdOsExtraFuncBind.osSocketsBindInfo.osSelectAddFdToSet                          );
    SET_NON_NULL(cmdOsSelectClearFdFromSet           , cmdOsExtraFuncBind.osSocketsBindInfo.osSelectClearFdFromSet                      );
    SET_NON_NULL(cmdOsSelectIsFdSet                  , cmdOsExtraFuncBind.osSocketsBindInfo.osSelectIsFdSet                             );
    SET_NON_NULL(cmdOsSelectCopySet                  , cmdOsExtraFuncBind.osSocketsBindInfo.osSelectCopySet                             );
    SET_NON_NULL(cmdOsSelect                         , cmdOsExtraFuncBind.osSocketsBindInfo.osSelect                                    );
    SET_NON_NULL(cmdOsSocketGetSocketFdSetSize       , cmdOsExtraFuncBind.osSocketsBindInfo.osSocketGetSocketFdSetSize                  );

    SET_NON_NULL(cmdOsPoolCreatePool                 , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolCreatePool                              );
    SET_NON_NULL(cmdOsPoolCreateDmaPool              , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolCreateDmaPool                           );
    SET_NON_NULL(cmdOsPoolReCreatePool               , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolReCreatePool                            );
    SET_NON_NULL(cmdOsPoolDeletePool                 , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolDeletePool                              );
    SET_NON_NULL(cmdOsPoolGetBuf                     , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolGetBuf                                  );
    SET_NON_NULL(cmdOsPoolFreeBuf                    , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolFreeBuf                                 );
    SET_NON_NULL(cmdOsPoolGetBufSize                 , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolGetBufSize                              );
    SET_NON_NULL(cmdOsPoolExpandPool                 , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolExpandPool                              );
    SET_NON_NULL(cmdOsPoolGetBufFreeCnt              , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolGetBufFreeCnt                           );
    SET_NON_NULL(cmdOsPoolPrintStats                 , cmdOsExtraFuncBind.osPoolsBindInfo.gtPoolPrintStats                              );

    SET_NON_NULL(cmdTraceHwAccessWrite                 , cmdTraceFuncBind.traceHwBindInfo.traceHwAccessWriteFunc                        );
    SET_NON_NULL(cmdTraceHwAccessRead                  , cmdTraceFuncBind.traceHwBindInfo.traceHwAccessReadFunc                        );
    SET_NON_NULL(cmdTraceHwAccessDelay                 , cmdTraceFuncBind.traceHwBindInfo.traceHwAccessDelayFunc                        );

    SET_NON_NULL(cmdIsCpuEtherPortUsed               , cmdExtraFuncBind.cpuEthernetPortBindInfo.cmdIsCpuEtherPortUsed               );

    SET_NON_NULL(cmdCpssEventBind                    , cmdExtraFuncBind.eventsBindInfo.cmdCpssEventBind                             );
    SET_NON_NULL(cmdCpssEventSelect                  , cmdExtraFuncBind.eventsBindInfo.cmdCpssEventSelect                           );
    SET_NON_NULL(cmdCpssEventRecv                    , cmdExtraFuncBind.eventsBindInfo.cmdCpssEventRecv                             );
    SET_NON_NULL(cmdCpssEventDeviceMaskSet           , cmdExtraFuncBind.eventsBindInfo.cmdCpssEventDeviceMaskSet                    );

    SET_NON_NULL(cmdAppDbEntryAdd                    , cmdExtraFuncBind.appDbBindInfo.cmdAppDbEntryAdd                              );
    SET_NON_NULL(cmdAppDbEntryGet                    , cmdExtraFuncBind.appDbBindInfo.cmdAppDbEntryGet                              );
    SET_NON_NULL(cmdAppDbDump                        , cmdExtraFuncBind.appDbBindInfo.cmdAppDbDump                                  );
    SET_NON_NULL(cmdAppPpConfigGet                   , cmdExtraFuncBind.appDbBindInfo.cmdAppPpConfigGet                             );
    SET_NON_NULL(cmdInitSystem                       , cmdExtraFuncBind.appDbBindInfo.cmdInitSystem                                 );
    SET_NON_NULL(cmdInitSystemGet                    , cmdExtraFuncBind.appDbBindInfo.cmdInitSystemGet                              );
    SET_NON_NULL(cmdResetSystem                      , cmdExtraFuncBind.appDbBindInfo.cmdResetSystem                                );
    SET_NON_NULL(cmdInitConfi                        , cmdExtraFuncBind.appDbBindInfo.cmdInitConfi                                  );
    SET_NON_NULL(cmdAppShowBoardsList                , cmdExtraFuncBind.appDbBindInfo.cmdAppShowBoardsList                          );
    SET_NON_NULL(cmdAppIsSystemInitialized           , cmdExtraFuncBind.appDbBindInfo.cmdAppIsSystemInitialized                     );
    SET_NON_NULL(cmdAppPpConfigPrint                 , cmdExtraFuncBind.appDbBindInfo.cmdAppPpConfigPrint                           );
    SET_NON_NULL(cmdAppAllowProcessingOfAuqMessages  , cmdExtraFuncBind.appDbBindInfo.cmdAppAllowProcessingOfAuqMessages            );
    SET_NON_NULL(cmdAppTraceHwAccessEnable           , cmdExtraFuncBind.appDbBindInfo.cmdAppTraceHwAccessEnable                     );
    SET_NON_NULL(cmdAppTraceHwAccessOutputModeSet    , cmdExtraFuncBind.appDbBindInfo.cmdAppTraceHwAccessOutputModeSet              );
    SET_NON_NULL(cmdAppStartHeapAllocCounter         , cmdExtraFuncBind.appDbBindInfo.cmdAppStartHeapAllocCounter                   );
    SET_NON_NULL(cmdAppGetHeapAllocCounter           , cmdExtraFuncBind.appDbBindInfo.cmdAppGetHeapAllocCounter                     );
    SET_NON_NULL(cmdAppEventFatalErrorEnable         , cmdExtraFuncBind.appDbBindInfo.cmdAppEventFatalErrorEnable                   );
    SET_NON_NULL(cmdAppDemoEventsDataBaseGet         , cmdExtraFuncBind.appDbBindInfo.cmdAppDemoEventsDataBaseGet                   );

#ifdef EXMXPM_FAMILY
    SET_NON_NULL(cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet  , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamRuleIndexByLenAndLookupGet);
    SET_NON_NULL(cmdAppExMxPmExternalTcamActionIndexGet              , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamActionIndexGet            );
    SET_NON_NULL(cmdAppExMxPmExternalTcamRuleAndActionIndexGet       , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamRuleAndActionIndexGet     );
    SET_NON_NULL(cmdAppExMxPmExternalTcamClientKeyLookupSet          , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamClientKeyLookupSet        );
    SET_NON_NULL(cmdAppExMxPmExternalTcamClientKeyLookupGet          , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamClientKeyLookupGet        );
    SET_NON_NULL(cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex   , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamClientKeyLookupSetByIndex );
    SET_NON_NULL(cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex   , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamClientKeyLookupGetByIndex );
    SET_NON_NULL(cmdAppExMxPmExternalTcamConfig                      , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamConfig                    );
    SET_NON_NULL(cmdAppExMxPmExternalTcamRuleStateGet                , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamRuleStateGet              );
    SET_NON_NULL(cmdAppExMxPmExternalTcamRuleValidStatusSet          , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamRuleValidStatusSet        );
    SET_NON_NULL(cmdAppExMxPmExternalTcamRuleAndActionCopy           , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmExternalTcamRuleAndActionCopy         );
    SET_NON_NULL(cmdAppExMxPmHsuOldImagePreUpdatePreparation         , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmHsuOldImagePreUpdatePreparation       );
    SET_NON_NULL(cmdAppExMxPmHsuNewImageUpdateAndSynch               , cmdExtraFuncBind.appDbBindInfo.cmdAppExMxPmHsuNewImageUpdateAndSynch             );
    SET_NON_NULL(cmdAppPuma3PortModeSpeedSet                         , cmdExtraFuncBind.appDbBindInfo.cmdAppPuma3PortModeSpeedSet                       );
    SET_NON_NULL(cmdAppPuma3PortModeSpeedSet_ext                     , cmdExtraFuncBind.appDbBindInfo.cmdAppPuma3PortModeSpeedSet_ext                   );
    SET_NON_NULL(cmdAppPuma3BoardTypeSet                             , cmdExtraFuncBind.appDbBindInfo.cmdAppPuma3BoardTypeSet                           );
    SET_NON_NULL(cmdAppPuma3LoopbackEnableSet                        , cmdExtraFuncBind.appDbBindInfo.cmdAppPuma3LoopbackEnableSet                      );
#endif /*EXMXPM_FAMILY*/

#ifdef CHX_FAMILY
    SET_NON_NULL(cmdPortRateTableGet                                 , cmdExtraFuncBind.appDbBindInfo.cmdPortRateTableGet                               );
    SET_NON_NULL(cmdAppLionPortModeSpeedSet                          , cmdExtraFuncBind.appDbBindInfo.cmdAppLionPortModeSpeedSet                        );
    SET_NON_NULL(cmdAppXcat2StackPortsModeSpeedSet                   , cmdExtraFuncBind.appDbBindInfo.cmdAppXcat2StackPortsModeSpeedSet                 );
    SET_NON_NULL(cmdAppXcat2SfpPortEnable                            , cmdExtraFuncBind.appDbBindInfo.cmdAppXcat2SfpPortEnable                          );
    SET_NON_NULL(cmdAppDxChHsuOldImagePreUpdatePreparation           , cmdExtraFuncBind.appDbBindInfo.cmdAppDxChHsuOldImagePreUpdatePreparation         );
    SET_NON_NULL(cmdAppDxChHsuNewImageUpdateAndSynch                 , cmdExtraFuncBind.appDbBindInfo.cmdAppDxChHsuNewImageUpdateAndSynch               );

#endif
    cmdInitExtServices_done = GT_TRUE;
    return GT_OK;
}


#ifdef GALTIS_NO_APP_DEMO

extern GT_STATUS cmdInit
(
    IN  GT_U32  devNum
);

/*******************************************************************************
* userAppInit
*
* DESCRIPTION:
*       This routine is the starting point of the Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS userAppInit(GT_VOID)
{
    /* Start the command shell */
    cmdInit(0);

    return GT_OK;
} /* userAppInit */

/*******************************************************************************
* applicationExtServicesBind
*
* DESCRIPTION:
*       the mainCmd calls this function , so the application (that implement
*       this function) will bind the mainCmd (and GaltisWrapper) with OS ,
*       external driver functions  and other application functions
*
* INPUTS:
*       none.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of external driver call back functions
*       osFuncBindPtr - (pointer to) set of OS call back functions
*       osExtraFuncBindPtr - (pointer to) set of extra OS call back functions (that CPSS not use)
*       extraFuncBindPtr - (pointer to) set of extra call back functions (that CPSS not use) (non OS functions)
*       traceFuncBindPtr - (pointer to) set of Trace call back functions
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       This function must be implemented by the Application !!!
*
*******************************************************************************/
GT_STATUS   applicationExtServicesBind(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC   *extDrvFuncBindInfoPtr,
    OUT CPSS_OS_FUNC_BIND_STC        *osFuncBindPtr,
    OUT CMD_OS_FUNC_BIND_EXTRA_STC   *osExtraFuncBindPtr,
    OUT CMD_FUNC_BIND_EXTRA_STC      *extraFuncBindPtr,
    OUT CPSS_TRACE_FUNC_BIND_STC     *traceFuncBindPtr
)
{

    /* function is built only to support 'linker' (the ability to build image) */

    /* the application must implement this function by itself , if application
       want to work with the mainCmd and the 'Galtis wrappers'.
       a reference (for function applicationExtServicesBind) can be found in file:
       <appDemo/boardConfig/appDemoBoardConfig.c> */


    return GT_NOT_IMPLEMENTED;
}
#endif /*GALTIS_NO_APP_DEMO*/
