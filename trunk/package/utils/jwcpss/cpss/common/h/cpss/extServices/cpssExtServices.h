/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExtServices.h
*
* DESCRIPTION:
*       External Driver wrapper. definitions for bind OS , external driver
*       dependent services and trace services to CPSS .
*
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __cpssExtServicesh
#define __cpssExtServicesh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#include <cpss/extServices/os/gtOs/cpssOsStr.h>
#include <cpss/extServices/os/gtOs/cpssOsMem.h>
#include <cpss/extServices/os/gtOs/cpssOsStr.h>
#include <cpss/extServices/os/gtOs/cpssOsIo.h>
#include <cpss/extServices/os/gtOs/cpssOsInet.h>
#include <cpss/extServices/os/gtOs/cpssOsTimer.h>
#include <cpss/extServices/os/gtOs/cpssOsRand.h>
#include <cpss/extServices/os/gtOs/cpssOsSem.h>
#include <cpss/extServices/os/gtOs/cpssOsIntr.h>
#include <cpss/extServices/os/gtOs/cpssOsTask.h>
#include <cpss/extServices/os/gtOs/cpssOsStdLib.h>
#include <cpss/extServices/os/gtOs/cpssOsMsgQ.h>
#include <cpss/extServices/os/gtOs/cpssOsLog.h>
#include <cpss/extServices/os/gtStack/cpssOsSocket.h>
#include <cpss/extServices/os/gtStack/cpssOsSelect.h>

#include <cpss/extServices/extDrv/drivers/cpssCacheMng.h>
#include <cpss/extServices/extDrv/drivers/cpssSmiHwCtrl.h>
#include <cpss/extServices/extDrv/drivers/cpssTwsiHwCtrl.h>
#include <cpss/extServices/extDrv/drivers/cpssDmaDrv.h>
#include <cpss/extServices/extDrv/drivers/cpssEthPortCtrl.h>
#include <cpss/extServices/extDrv/drivers/cpssHsuDrv.h>
#include <cpss/extServices/extDrv/drivers/cpssIntDrv.h>
#include <cpss/extServices/extDrv/drivers/cpssPciDrv.h>
#include <cpss/extServices/extDrv/drivers/cpssMgmtHwIfDrv.h>
#include <cpss/extServices/extDrv/drivers/cpssDragoniteDrv.h>

#include <cpss/extServices/trace/cpssTraceHw.h>


/* CPSS_EXT_DRV_BIND_STC -
*    structure that hold the "external driver" functions needed be
*    bound to cpss.
*
*    extDrvMgmtCacheBindInfo - set of call back functions -
*                        cache manipulation
*    extDrvHwIfSmiBindInfí - set of call back functions -
*                        SMI interface manipulation
*    extDrvHwIfTwsiBindInfo - set of call back functions -
*                        TWSI interface manipulation
*    extDrvDmaBindInfo - set of call back functions -
*                        DMA manipulation
*    extDrvEthPortBindInfo -set of call back functions -
*                        CPU Ethernet port manipulation
*    extDrvEthPortBindInfo -set of call back functions -
*                        HSU functions
*    extDrvIntBindInfo - set of call back functions -
*                        interrupts manipulation
*    extDrvPciInfo - set of call back functions -
*                        PCI manipulation
*
*/
typedef struct{
    CPSS_EXT_DRV_MGMT_CACHE_STC  extDrvMgmtCacheBindInfo;
    CPSS_EXT_DRV_HW_IF_SMI_STC   extDrvHwIfSmiBindInfo;
    CPSS_EXT_DRV_HW_IF_TWSI_STC  extDrvHwIfTwsiBindInfo;
    CPSS_EXT_DRV_DMA_STC         extDrvDmaBindInfo;
    CPSS_EXT_DRV_ETH_PORT_STC    extDrvEthPortBindInfo;
    CPSS_EXT_DRV_HSU_DRV_STC     extDrvHsuDrvBindInfo;
    CPSS_EXT_DRV_INT_STC         extDrvIntBindInfo;
    CPSS_EXT_DRV_PCI_STC         extDrvPciInfo;
    CPSS_EXT_DRV_MGMT_HW_IF_STC  extDrvMgmtHwIfBindInfo;
    CPSS_EXT_DRV_DRAGONITE_STC   extDrvDragoniteInfo;
}CPSS_EXT_DRV_FUNC_BIND_STC;

/************* functions ******************************************************/

/* CPSS_OS_FUNC_BIND_STC -
*    structure that hold the "os" functions needed be bound to cpss.
*
*       osMemBindInfo -  set of call back functions -
*                        CPU memory manipulation
*       osStrBindInfo - set of call back functions -
*                        strings manipulation
*       osSemBindInfo - set of call back functions -
*                           semaphore manipulation
*       osIoBindInfo - set of call back functions -
*                           I/O manipulation
*       osInetBindInfo - set of call back functions -
*                           inet manipulation
*       osTimeBindInfo - set of call back functions -
*                           time manipulation
*       osIntBindInfo - set of call back functions -
*                           interrupts manipulation
*       osRandBindInfo - set of call back functions -
*                           random numbers manipulation
*       osTaskBindInfo - set of call back functions -
*                           tasks manipulation
*       osStdLibBindInfo - set of call back functions -
*                           stdlib functions (qsort, bsearch, etc)
*       osMsgQBindInfo - set of call back functions -
*                            message queues
*       osLogBindInfo - set of call back functions - log manipulation
*/
typedef struct{
    CPSS_OS_MEM_BIND_STC    osMemBindInfo;
    CPSS_OS_STR_BIND_STC    osStrBindInfo;
    CPSS_OS_SEM_BIND_STC    osSemBindInfo;
    CPSS_OS_IO_BIND_STC     osIoBindInfo;
    CPSS_OS_INET_BIND_STC   osInetBindInfo;
    CPSS_OS_TIME_BIND_STC   osTimeBindInfo;
    CPSS_OS_INT_BIND_STC    osIntBindInfo;
    CPSS_OS_RAND_BIND_STC   osRandBindInfo;
    CPSS_OS_TASK_BIND_STC   osTaskBindInfo;
    CPSS_OS_STDLIB_BIND_STC osStdLibBindInfo;
    CPSS_OS_MSGQ_BIND_STC   osMsgQBindInfo;
    CPSS_OS_LOG_BIND_STC    osLogBindInfo;
    CPSS_SOCKET_BIND_STC    osSocketInfo;
    CPSS_SOCKET_SELECT_BIND_STC osSocketSelectInfo;
}CPSS_OS_FUNC_BIND_STC;


/* CPSS_TRACE_FUNC_BIND_STC -
*    structure that hold the "trace" functions needed be bound to cpss.
*
*       traceHwBindInfo -  set of call back functions -
*                           HW trace functions
*/
typedef struct{
    CPSS_TRACE_HW_ACCESS_FUNC_BIND_STC    traceHwBindInfo;
}CPSS_TRACE_FUNC_BIND_STC;


/************* functions ******************************************************/
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
GT_STATUS   cpssExtServicesBind(
    IN CPSS_EXT_DRV_FUNC_BIND_STC   *extDrvFuncBindInfoPtr,
    IN CPSS_OS_FUNC_BIND_STC        *osFuncBindPtr,
    IN CPSS_TRACE_FUNC_BIND_STC     *traceFuncBindPtr
);

#ifdef __cplusplus
}
#endif

#endif  /* __cpssExtServicesh */


