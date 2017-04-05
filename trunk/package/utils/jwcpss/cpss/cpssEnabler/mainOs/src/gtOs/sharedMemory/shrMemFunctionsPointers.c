/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemFunctionsPointers.c
*
* DESCRIPTION:
*       This file contains bind routines for BM architectures to avoid
*       pointer issue. Work-around uses dlsym system call to avoid
*       private addresses to functions.
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/

/************* Includes *******************************************************/
#include <stdio.h>
#include <dlfcn.h>
#include <gtOs/gtGenTypes.h>
#include <cpss/extServices/cpssExtServices.h>
#include <gtOs/gtOsMem.h>

#include <gtExtDrv/drivers/gtIntDrv.h>
#include <gtExtDrv/drivers/gtPciDrv.h>
#include <gtExtDrv/drivers/gtCacheMng.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>
#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <gtExtDrv/drivers/gtDmaDrv.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <gtExtDrv/drivers/gtI2cDrv.h>
#include <gtExtDrv/drivers/gtHwIfDrv.h>

/*******************************************************************************
* getCpssSharedLibraryHandle
*
* DESCRIPTION:
*       Gets handler of library object (DSO)
*
* INPUTS:
*       lib_name - library name to be opened
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       NULL - on error
*       Otherwise - handler to shared object
*
* COMMENTS:
*       Used for dlsym system call.
*
*******************************************************************************/
static GT_VOID *getCpssSharedLibraryHandle(const char *lib_name)
{
    GT_VOID *handle = dlopen(lib_name, RTLD_LAZY);
    if (!handle)
    {
        fprintf (stderr, "%s\n", dlerror());
        return NULL;
    }

    dlerror();    /* Clear any existing error */
    return handle;
}

/*******************************************************************************
* getFunctionThruDynamicLoader
*
* DESCRIPTION:
*       Gets address of specified function into dynamic symbol 
*       (described by handle) thru dlsym system call.
*
* INPUTS:
*       handle - handle of dynamic object to be searched into
*       func_name - name of symbol to be searched for
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       NULL - on error
*       Otherwise - pointer to function
*
* COMMENTS:
*       Uses dlsym system call to overcome private-pointer-to-func issue
*       observed for ARM GnuEABI.
*
*******************************************************************************/
static GT_VOID *getFunctionThruDynamicLoader(GT_VOID *handle, const char *func_name)
{
    GT_VOID *ptr=dlsym(handle, func_name);
    if (!ptr)
    {
        fprintf(stderr, "getFunctionThruDynamicLoader, function %s: %s\n", func_name, dlerror());
        return NULL;
    }
    return ptr;
}


/*******************************************************************************
* closeLibraryHandle
*
* DESCRIPTION:
*       Close library handle opened by getCpssSharedLibraryHandle
*
* INPUTS:
*       handle - handle of dynamic library to be closed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       Cleanup hanle opened by dlopen.
*
*******************************************************************************/
static GT_VOID closeLibraryHandle(GT_VOID *handle)
{
    dlclose(handle);
}

/*******************************************************************************
* shrMemGetDefaultOsBindFuncsThruDynamicLoader
*
* DESCRIPTION:
*       Receives default cpss bind from OS
* INPUTS:
*       None.
* OUTPUTS:
*       osFuncBindPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Uses dlsym system call to overcome private-pointer-to-func issue
*       observed for ARM GnuEABI.
*
*******************************************************************************/
GT_STATUS shrMemGetDefaultOsBindFuncsThruDynamicLoader(
    OUT CPSS_OS_FUNC_BIND_STC *osFuncBindPtr
)
{
    GT_VOID *handle=getCpssSharedLibraryHandle("libhelper.so.0");
    if(handle==NULL) return GT_FAIL;

    osMemSet(osFuncBindPtr,0,sizeof(*osFuncBindPtr));

    if (!handle)
    {
        fprintf (stderr, "%s\n", dlerror());
        return GT_FAIL;
    }

    dlerror();    /* Clear any existing error */

    /* bind the OS functions to the CPSS */
    if((osFuncBindPtr->osMemBindInfo.osMemBzeroFunc=getFunctionThruDynamicLoader(handle, "osBzero")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemSetFunc=getFunctionThruDynamicLoader(handle, "osMemSet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemCpyFunc=getFunctionThruDynamicLoader(handle, "osMemCpy")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemMoveFunc=getFunctionThruDynamicLoader(handle, "osMemMove")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemCmpFunc=getFunctionThruDynamicLoader(handle, "osMemCmp")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemStaticMallocFunc=getFunctionThruDynamicLoader(handle, "osStaticMalloc")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemMallocFunc=getFunctionThruDynamicLoader(handle, "osMalloc")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemReallocFunc=getFunctionThruDynamicLoader(handle, "osRealloc")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemFreeFunc=getFunctionThruDynamicLoader(handle, "osFree")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemCacheDmaMallocFunc=getFunctionThruDynamicLoader(handle, "osCacheDmaMalloc")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemCacheDmaFreeFunc=getFunctionThruDynamicLoader(handle, "osCacheDmaFree")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemPhyToVirtFunc=getFunctionThruDynamicLoader(handle, "osPhy2Virt")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMemBindInfo.osMemVirtToPhyFunc=getFunctionThruDynamicLoader(handle, "osVirt2Phy")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osStrBindInfo.osStrlenFunc=getFunctionThruDynamicLoader(handle, "osStrlen")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrCpyFunc=getFunctionThruDynamicLoader(handle, "osStrCpy")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrNCpyFunc=getFunctionThruDynamicLoader(handle, "osStrNCpy")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrChrFunc=getFunctionThruDynamicLoader(handle, "osStrChr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrStrFunc=getFunctionThruDynamicLoader(handle, "osStrStr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrRevChrFunc=getFunctionThruDynamicLoader(handle, "osStrrChr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrCmpFunc=getFunctionThruDynamicLoader(handle, "osStrCmp")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrNCmpFunc=getFunctionThruDynamicLoader(handle, "osStrNCmp")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrCatFunc=getFunctionThruDynamicLoader(handle, "osStrCat")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrStrNCatFunc=getFunctionThruDynamicLoader(handle, "osStrNCat")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrChrToUpperFunc=getFunctionThruDynamicLoader(handle, "osToUpper")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrTo32Func=getFunctionThruDynamicLoader(handle, "osStrTo32")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStrBindInfo.osStrToU32Func=getFunctionThruDynamicLoader(handle, "osStrToU32")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osSemBindInfo.osMutexCreateFunc=getFunctionThruDynamicLoader(handle, "osMutexCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osMutexDeleteFunc=getFunctionThruDynamicLoader(handle, "osMutexDelete")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osMutexLockFunc=getFunctionThruDynamicLoader(handle, "osMutexLock")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osMutexUnlockFunc=getFunctionThruDynamicLoader(handle, "osMutexUnlock")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osSemBindInfo.osSigSemBinCreateFunc=getFunctionThruDynamicLoader(handle, "osSemBinCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osSigSemMCreateFunc=getFunctionThruDynamicLoader(handle, "osSemMCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osSigSemCCreateFunc=getFunctionThruDynamicLoader(handle, "osSemCCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osSigSemDeleteFunc=getFunctionThruDynamicLoader(handle, "osSemDelete")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osSigSemWaitFunc=getFunctionThruDynamicLoader(handle, "osSemWait")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSemBindInfo.osSigSemSignalFunc=getFunctionThruDynamicLoader(handle, "osSemSignal")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osIoBindInfo.osIoBindStdOutFunc=getFunctionThruDynamicLoader(handle, "osBindStdOut")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoPrintfFunc=getFunctionThruDynamicLoader(handle, "osPrintf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoVprintfFunc=getFunctionThruDynamicLoader(handle, "osVprintf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoSprintfFunc=getFunctionThruDynamicLoader(handle, "osSprintf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoVsprintfFunc=getFunctionThruDynamicLoader(handle, "osVsprintf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoSnprintfFunc=getFunctionThruDynamicLoader(handle, "osSnprintf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoVsnprintfFunc=getFunctionThruDynamicLoader(handle, "osVsnprintf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoPrintSynchFunc=getFunctionThruDynamicLoader(handle, "osPrintSync")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoGetsFunc=getFunctionThruDynamicLoader(handle, "osGets")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFlastErrorStrFunc=getFunctionThruDynamicLoader(handle, "osFlastErrorStr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFopenFunc=getFunctionThruDynamicLoader(handle, "osFopen")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFcloseFunc=getFunctionThruDynamicLoader(handle, "osFclose")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoRewindFunc=getFunctionThruDynamicLoader(handle, "osRewind")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFprintfFunc=getFunctionThruDynamicLoader(handle, "osFprintf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFgets=getFunctionThruDynamicLoader(handle, "osFgets")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFwriteFunc=getFunctionThruDynamicLoader(handle, "osFwrite")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFreadFunc=getFunctionThruDynamicLoader(handle, "osFread")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFgetLengthFunc=getFunctionThruDynamicLoader(handle, "osFgetLength")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIoBindInfo.osIoFatalErrorFunc=getFunctionThruDynamicLoader(handle, "osFatalError")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osInetBindInfo.osInetNtohlFunc=getFunctionThruDynamicLoader(handle, "osNtohl")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osInetBindInfo.osInetHtonlFunc=getFunctionThruDynamicLoader(handle, "osHtonl")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osInetBindInfo.osInetNtohsFunc=getFunctionThruDynamicLoader(handle, "osNtohs")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osInetBindInfo.osInetHtonsFunc=getFunctionThruDynamicLoader(handle, "osHtons")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osInetBindInfo.osInetNtoaFunc=getFunctionThruDynamicLoader(handle, "osInetNtoa")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osTimeBindInfo.osTimeWkAfterFunc=getFunctionThruDynamicLoader(handle, "osTimerWkAfter")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTimeBindInfo.osTimeTickGetFunc=getFunctionThruDynamicLoader(handle, "osTickGet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTimeBindInfo.osTimeGetFunc=getFunctionThruDynamicLoader(handle, "osTime")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTimeBindInfo.osTimeRTFunc=getFunctionThruDynamicLoader(handle, "osTimeRT")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTimeBindInfo.osGetSysClockRateFunc=getFunctionThruDynamicLoader(handle, "osGetSysClockRate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTimeBindInfo.osDelayFunc=getFunctionThruDynamicLoader(handle, "osDelay")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTimeBindInfo.osStrftimeFunc=getFunctionThruDynamicLoader(handle, "osStrftime")) == NULL) return GT_FAIL;

#if !defined(_linux) && !defined(_FreeBSD)
    if((osFuncBindPtr->osIntBindInfo.osIntEnableFunc=getFunctionThruDynamicLoader(handle, "osIntEnable")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIntBindInfo.osIntDisableFunc=getFunctionThruDynamicLoader(handle, "osIntDisable")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIntBindInfo.osIntModeSetFunc=getFunctionThruDynamicLoader(handle, "osSetIntLockUnlock")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osIntBindInfo.osIntConnectFunc=getFunctionThruDynamicLoader(handle, "osInterruptConnect")) == NULL) return GT_FAIL;
#endif
    if((osFuncBindPtr->osRandBindInfo.osRandFunc=getFunctionThruDynamicLoader(handle, "osRand")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osRandBindInfo.osSrandFunc=getFunctionThruDynamicLoader(handle, "osSrand")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osTaskBindInfo.osTaskCreateFunc=getFunctionThruDynamicLoader(handle, "osTaskCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTaskBindInfo.osTaskGetSelfFunc=getFunctionThruDynamicLoader(handle, "osTaskGetSelf")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTaskBindInfo.osTaskLockFunc=getFunctionThruDynamicLoader(handle, "osTaskLock")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osTaskBindInfo.osTaskUnLockFunc=getFunctionThruDynamicLoader(handle, "osTaskUnLock")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osStdLibBindInfo.osQsortFunc=getFunctionThruDynamicLoader(handle, "osQsort")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osStdLibBindInfo.osBsearchFunc=getFunctionThruDynamicLoader(handle, "osBsearch")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osMsgQBindInfo.osMsgQCreateFunc=getFunctionThruDynamicLoader(handle, "osMsgQCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMsgQBindInfo.osMsgQDeleteFunc=getFunctionThruDynamicLoader(handle, "osMsgQDelete")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMsgQBindInfo.osMsgQSendFunc=getFunctionThruDynamicLoader(handle, "osMsgQSend")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMsgQBindInfo.osMsgQRecvFunc=getFunctionThruDynamicLoader(handle, "osMsgQRecv")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osMsgQBindInfo.osMsgQNumMsgsFunc=getFunctionThruDynamicLoader(handle, "osMsgQNumMsgs")) == NULL) return GT_FAIL;

    if((osFuncBindPtr->osSocketInfo.osSocketLastErrorFunc=getFunctionThruDynamicLoader(handle, "osSocketLastError")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketLastErrorStrFunc=getFunctionThruDynamicLoader(handle, "osSocketLastErrorStr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketTcpCreateFunc=getFunctionThruDynamicLoader(handle, "osSocketTcpCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketUdpCreateFunc=getFunctionThruDynamicLoader(handle, "osSocketUdpCreate")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketTcpDestroyFunc=getFunctionThruDynamicLoader(handle, "osSocketTcpDestroy")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketUdpDestroyFunc=getFunctionThruDynamicLoader(handle, "osSocketUdpDestroy")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketCreateAddrFunc=getFunctionThruDynamicLoader(handle, "osSocketCreateAddr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketDestroyAddrFunc=getFunctionThruDynamicLoader(handle, "osSocketDestroyAddr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketBindFunc=getFunctionThruDynamicLoader(handle, "osSocketBind")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketListenFunc=getFunctionThruDynamicLoader(handle, "osSocketListen")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketAcceptFunc=getFunctionThruDynamicLoader(handle, "osSocketAccept")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketConnectFunc=getFunctionThruDynamicLoader(handle, "osSocketConnect")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketSetNonBlockFunc=getFunctionThruDynamicLoader(handle, "osSocketSetNonBlock")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketSetBlockFunc=getFunctionThruDynamicLoader(handle, "osSocketSetBlock")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketSendFunc=getFunctionThruDynamicLoader(handle, "osSocketSend")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketSendToFunc=getFunctionThruDynamicLoader(handle, "osSocketSendTo")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketRecvFunc=getFunctionThruDynamicLoader(handle, "osSocketRecv")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketRecvFromFunc=getFunctionThruDynamicLoader(handle, "osSocketRecvFrom")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketSetSocketNoLingerFunc=getFunctionThruDynamicLoader(handle, "osSocketSetSocketNoLinger")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketExtractIpAddrFromSocketAddrFunc=getFunctionThruDynamicLoader(handle, "osSocketExtractIpAddrFromSocketAddr")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketGetSocketAddrSizeFunc=getFunctionThruDynamicLoader(handle, "osSocketGetSocketAddrSize")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketInfo.osSocketShutDownFunc=getFunctionThruDynamicLoader(handle, "osSocketShutDown")) == NULL) return GT_FAIL;


    if((osFuncBindPtr->osSocketSelectInfo.osSelectCreateSetFunc=getFunctionThruDynamicLoader(handle, "osSelectCreateSet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSelectEraseSetFunc=getFunctionThruDynamicLoader(handle, "osSelectEraseSet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSelectZeroSetFunc=getFunctionThruDynamicLoader(handle, "osSelectZeroSet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSelectAddFdToSetFunc=getFunctionThruDynamicLoader(handle, "osSelectAddFdToSet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSelectClearFdFromSetFunc=getFunctionThruDynamicLoader(handle, "osSelectClearFdFromSet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSelectIsFdSetFunc=getFunctionThruDynamicLoader(handle, "osSelectIsFdSet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSelectCopySetFunc=getFunctionThruDynamicLoader(handle, "osSelectCopySet")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSelectFunc=getFunctionThruDynamicLoader(handle, "osSelect")) == NULL) return GT_FAIL;
    if((osFuncBindPtr->osSocketSelectInfo.osSocketGetSocketFdSetSizeFunc=getFunctionThruDynamicLoader(handle, "osSocketGetSocketFdSetSize")) == NULL) return GT_FAIL;

    closeLibraryHandle(handle);

    return GT_OK;
}

/*******************************************************************************
* cpssGetDefaultExtDrvFuncs
*
* DESCRIPTION:
*       Receives default cpss bind from extDrv
* INPUTS:
*       None.
* OUTPUTS:
*       extDrvFuncBindInfoPtr - (pointer to) set of call back functions
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       Function should reside into CPSS library to resolve correct
*       pointers to functions.
*
*******************************************************************************/
GT_STATUS shrMemGetDefaultExtDrvFuncs
(
    OUT CPSS_EXT_DRV_FUNC_BIND_STC  *extDrvFuncBindInfoPtr
)
{
    GT_VOID *handle=getCpssSharedLibraryHandle("libcpss.so.0");
    if(handle==NULL) return GT_FAIL;

    osMemSet(extDrvFuncBindInfoPtr,0,sizeof(*extDrvFuncBindInfoPtr));

    /* bind the external drivers functions to the CPSS */
    if((extDrvFuncBindInfoPtr->extDrvMgmtCacheBindInfo.extDrvMgmtCacheFlush=
                getFunctionThruDynamicLoader(handle, "extDrvMgmtCacheFlush")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvMgmtCacheBindInfo.extDrvMgmtCacheFlush=
                getFunctionThruDynamicLoader(handle, "extDrvMgmtCacheFlush")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvMgmtCacheBindInfo.extDrvMgmtCacheInvalidate=
                getFunctionThruDynamicLoader(handle, "extDrvMgmtCacheInvalidate")) == NULL) return GT_FAIL;

    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiInitDriverFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiInitDriver")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiWriteRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiWriteReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiReadRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiReadReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamReadFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiTskRegRamRead")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegRamWriteFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiTskRegRamWrite")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecReadFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiTskRegVecRead")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskRegVecWriteFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiTskRegVecWrite")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiTaskWriteReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskReadRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiTaskReadReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntReadRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiInterruptReadReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiIntWriteRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfSmiInterruptWriteReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiDevVendorIdGetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvSmiDevVendorIdGet")) == NULL) return GT_FAIL;

/*  used only in linux -- will need to be under some kind of COMPILATION FLAG
    if((extDrvFuncBindInfoPtr->extDrvHwIfSmiBindInfo.extDrvHwIfSmiTaskWriteFieldFunc=
    getFunctionThruDynamicLoader(handle, "hwIfSmiTaskWriteRegField")) == NULL) return GT_FAIL;
*/

#ifdef GT_I2C
    if((extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiInitDriverFunc=
                getFunctionThruDynamicLoader(handle, "hwIfTwsiInitDriver")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiWriteRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfTwsiWriteReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHwIfTwsiBindInfo.extDrvHwIfTwsiReadRegFunc=
                getFunctionThruDynamicLoader(handle, "hwIfTwsiReadReg")) == NULL) return GT_FAIL;
#endif /* GT_I2C */

/*  XBAR related services */
#if defined(IMPL_FA) || defined(IMPL_XBAR)
    if((extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvI2cMgmtMasterInitFunc=
                getFunctionThruDynamicLoader(handle, "gtI2cMgmtMasterInit")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtReadRegisterFunc=
                getFunctionThruDynamicLoader(handle, "extDrvMgmtReadRegister")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtWriteRegisterFunc=
                getFunctionThruDynamicLoader(handle, "extDrvMgmtWriteRegister")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtIsrReadRegisterFunc=
                getFunctionThruDynamicLoader(handle, "extDrvMgmtIsrReadRegister")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvMgmtHwIfBindInfo.extDrvMgmtIsrWriteRegisterFunc=
                getFunctionThruDynamicLoader(handle, "extDrvMgmtIsrWriteRegister")) == NULL) return GT_FAIL;
#endif

    if((extDrvFuncBindInfoPtr->extDrvDmaBindInfo.extDrvDmaWriteDriverFunc=
                getFunctionThruDynamicLoader(handle, "extDrvDmaWrite")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvDmaBindInfo.extDrvDmaReadFunc=
                getFunctionThruDynamicLoader(handle, "extDrvDmaRead")) == NULL) return GT_FAIL;

    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortRxInitFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthPortRxInit")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketModeSetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthRawSocketModeSet")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketModeGetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthRawSocketModeGet")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvLinuxModeSetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvLinuxModeSet")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvLinuxModeGetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvLinuxModeGet")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuMemBaseAddrGetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvHsuMemBaseAddrGet")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvHsuDrvBindInfo.extDrvHsuWarmRestartFunc=
                getFunctionThruDynamicLoader(handle, "extDrvHsuWarmRestart")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxInitFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthPortTxInit")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortEnableFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthPortEnable")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortDisableFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthPortDisable")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthPortTx")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortInputHookAddFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthInputHookAdd")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthRawSocketRxHookAddFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthRawSocketRxHookAdd")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxCompleteHookAddFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthTxCompleteHookAdd")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortRxPacketFreeFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthRxPacketFree")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPortTxModeSetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthPortTxModeSet")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthCpuCodeToQueueFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthCpuCodeToQueue")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvEthPortBindInfo.extDrvEthPrePendTwoBytesHeaderSetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEthPrePendTwoBytesHeaderSet")) == NULL) return GT_FAIL;

    if((extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntConnectFunc=
                getFunctionThruDynamicLoader(handle, "extDrvIntConnect")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntEnableFunc=
                getFunctionThruDynamicLoader(handle, "extDrvIntEnable")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntDisableFunc=
                getFunctionThruDynamicLoader(handle, "extDrvIntDisable")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvIntBindInfo.extDrvIntLockModeSetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvSetIntLockUnlock")) == NULL) return GT_FAIL;

    if((extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciConfigWriteRegFunc=
                getFunctionThruDynamicLoader(handle, "extDrvPciConfigWriteReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciConfigReadRegFunc=
                getFunctionThruDynamicLoader(handle, "extDrvPciConfigReadReg")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciDevFindFunc=
                getFunctionThruDynamicLoader(handle, "extDrvPciFindDev")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciIntVecFunc=
                getFunctionThruDynamicLoader(handle, "extDrvGetPciIntVec")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciIntMaskFunc=
                getFunctionThruDynamicLoader(handle, "extDrvGetIntMask")) == NULL) return GT_FAIL;
    if((extDrvFuncBindInfoPtr->extDrvPciInfo.extDrvPciCombinedAccessEnableFunc=
                getFunctionThruDynamicLoader(handle, "extDrvEnableCombinedPciAccess")) == NULL) return GT_FAIL;
#ifdef DRAGONITE_TYPE_A1
    if((extDrvFuncBindInfoPtr->extDrvDragoniteInfo.extDrvDragoniteShMemBaseAddrGetFunc=
                getFunctionThruDynamicLoader(handle, "extDrvDragoniteShMemBaseAddrGet")) == NULL) return GT_FAIL;
#endif 
    closeLibraryHandle(handle);

    return GT_OK;
}
