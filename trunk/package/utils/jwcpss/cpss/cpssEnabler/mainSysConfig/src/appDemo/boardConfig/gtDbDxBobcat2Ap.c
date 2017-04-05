/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxBobcat2Ap.c
*
* DESCRIPTION:
*       AP task for Bobcat2 board.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/init/cpssInit.h>

/* structures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>

#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>

/* include the external services */
#include <cmdShell/common/cmdExtServices.h>

/*******************************************************************************
 * External definitions
 ******************************************************************************/

/*******************************************************************************
 * Global variables
 ******************************************************************************/

#ifndef ASIC_SIMULATION_NO_HW
/* I intentionaly not define it as static to enable init at any time from command
   line */
GT_STATUS bc2AppDemoApInit
(
    IN  GT_U8   devNum
);
#endif

#ifndef ASIC_SIMULATION_NO_HW

#ifdef LINUX

extern GT_U32  startSeconds;
extern GT_U32  startNanoSeconds;

extern void mvSysTimerMeasureStart
(
    void
);

extern void mvSysTimerMeasureGet
(
    GT_U32 *miliSecPtr
);
#endif

static GT_U32 doTimeMeasure = 0;
static GT_U32 doTimeMeasurePort = 0;
extern GT_U32 apDoTimeMeasure
(
    IN  GT_U32                  enable,
    IN  GT_PHYSICAL_PORT_NUM    portNum
);

#ifdef LINUX
#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    cpssOsTaskLock();                                                    \
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &(key))

#define INT_UNLOCK(key)                                                     \
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &(key));\
    cpssOsTaskUnLock()

#endif

static GT_U32   allowPrint=0;/* option to disable the print in runtime*/
#define AP_DBG_PRINT_MAC(x) if(allowPrint) cpssOsPrintf x

extern GT_U32 apBc2TaskPrintControl(IN GT_U32  allowPrintNew)
{
    GT_U32  oldState = allowPrint;
    allowPrint = allowPrintNew;

    return oldState;
}

/*******************************************************************************
* apRestartOnPort
*
* DESCRIPTION:
*       Cause restart AP on port
*
* INPUTS:
*       devNum - device number
*       portNum - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       other - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS apRestartOnPort
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum   
)
{
    GT_STATUS rc;

    AP_DBG_PRINT_MAC(("mac and pcs reset:portNum=%d\n", portNum));
    rc = cpssDxChPortMacResetStateSet(devNum, portNum, GT_TRUE);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortMacResetStateSet:rc=%d,portNum=%d\n",
                        rc, portNum);
        return rc;
    }
    rc = cpssDxChPortPcsResetSet(devNum, portNum, 
                                 CPSS_PORT_PCS_RESET_MODE_ALL_E,
                                 GT_TRUE);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortPcsResetSet:rc=%d,portNum=%d\n",
                        rc, portNum);
    }

    return rc;
}

/*******************************************************************************
* bc2AppDemoApResolvedPortsConfig
*
* DESCRIPTION:
*       Configure ports of core that get AP resolution and get bitmap of ports
*       where need to run TRX training
*
* INPUTS:
*       devNum - device number
*       portGroupId - core number
*
* OUTPUTS:
*       trxTrainBmpPtr - (pointer to) bitmap of ports where need to run TRX
*                           training
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID bc2AppDemoApResolvedPortsConfig
(
    IN      GT_U8               devNum
)
{
    GT_STATUS           rc;                /* return code         */
    GT_U32              i;                 /* iterator            */
    CPSS_PORTS_BMP_STC  initPortBmp, *initPortBmpPtr; /* port
                                            to configure resolved interface */
    CPSS_DXCH_PORT_AP_STATUS_STC    apStatus;   /* current AP status of port where
                                                    AP resolved */
    GT_U32              apResolvedPortsBmp1;/* bitmap of ports on local core where
                                                AP resolved*/
    GT_PHYSICAL_PORT_NUM            portNum;    /* global number of port where
                                                    AP resolved */

#if ((defined _VXWORKS) && (defined CPU_78200)) || (defined LINUX)
    GT_U32  portCreateTime; /* port interface configuration in mili-seconds */
#endif

    initPortBmpPtr = &initPortBmp;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortBmpPtr);

    rc = cpssDxChPortApResolvedPortsBmpGet(devNum, 0, &apResolvedPortsBmp1);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortApResolvedPortsBmpGet:rc=%d\n", rc);
        return;
    }
    AP_DBG_PRINT_MAC(("cpssDxChPortApResolvedPortsBmpGet:apResolvedPortsBmp1=0x%x\n",
                      apResolvedPortsBmp1));

    for(i = 0; apResolvedPortsBmp1>>i; i++)
    {
        if((apResolvedPortsBmp1>>i) & 1)
        {
            portNum = 48 + i;
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))
            if((1 == doTimeMeasure) && (portNum == doTimeMeasurePort))
            {
                mvSysTimerMeasureStart();
            }
#endif
            rc = cpssDxChPortApPortStatusGet(devNum, portNum, &apStatus);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortApPortStatusGet:rc=%d\n", rc);
                return;
            }

            if(apStatus.hcdFound)
            {
                AP_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet:portNum=%d\n", portNum));
                rc = cpssDxChPortModeSpeedSet(devNum, initPortBmp,
                                              GT_TRUE,
                                              apStatus.portMode.ifMode,
                                              apStatus.portMode.speed);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortModeSpeedSet:rc=%d,portNum=%d,ifMode=%d,speed=%d\n",
                                    rc, portNum, apStatus.portMode.ifMode,
                                    apStatus.portMode.speed);
                    (GT_VOID)apRestartOnPort(devNum, portNum);
                    return;
                }

#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX) && (!defined ASIC_SIMULATION_NO_HW))
                if((1 == doTimeMeasure) && (portNum == doTimeMeasurePort))
                {
                    mvSysTimerMeasureGet(&portCreateTime);
                    cpssOsPrintf("port=%d:portCreateTime=%d(miliSec)\n", portNum,
                                    portCreateTime);
                    mvSysTimerMeasureStart(); /* restart timer */
                }
#endif
            }
        }
    }

    return;
}


/*******************************************************************************
* bc2AppDemoApHndlTask
*
* DESCRIPTION:
*       This routine is the event handler for 802.3ap event
*
* INPUTS:
*       param - The process data structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*    This function is part of the AP appDemo code that can be used as reference code.
*    This code was only tested on a single-device system. 
*    Code changes may be required for a multi-device system (controlled by one CPU).
* 
*******************************************************************************/
static unsigned __TASKCONV bc2AppDemoApHndlTask
(
    GT_VOID * param
)
{
    GT_STATUS           rc;                /* return code         */
    GT_UINTPTR          evHndl;            /* event handler       */
    GT_U8               devNum;            /* device number       */
    GT_U32              extendedData;       /* event extended data */
    
    evHndl = (GT_UINTPTR)param;

    while (1)
    {
        rc = cpssEventSelect(evHndl, NULL, NULL, 0);
        if(rc != GT_OK)
        {
            cpssOsPrintf("bc2AppDemoApHndlTask:cpssEventSelect: err [%d]\n",
                            rc);
            return rc;
        }

        if ((rc = cpssEventRecv(evHndl, CPSS_PP_PORT_802_3_AP_E, &extendedData, &devNum)) == GT_OK)
        {
            AP_DBG_PRINT_MAC(("cpssEventRecv:devNum=%d\n", devNum));
            
            bc2AppDemoApResolvedPortsConfig(devNum);
        }
        else
        {
            if(rc != GT_NO_MORE)
            {
                cpssOsPrintf("bc2AppDemoApHndlTask:cpssEventRecv: err [%d]\n",
                                rc);
            }
        }
    } /* while(1) */

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif

}

/*******************************************************************************
* lion2AppDemoApInit
*
* DESCRIPTION:
*       Example of init for 802.3ap support.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*   This function is part of the AP appDemo code that can be used as reference code.
*   This code was only tested on a single-device system. 
*   Code changes may be required for a multi-device system (controlled by one CPU).
*
********************************************************************************/
GT_STATUS bc2AppDemoApInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc;             /* return code */
    GT_32       intKey;         /* The interrupt lock key       */
    GT_CHAR     apTaskName[65]; /* string to build per core AP task */
    GT_TASK     apTaskId;       /* AP Task Id */
    CPSS_UNI_EV_CAUSE_ENT evHndlrCauseAllArr[1];/* AP(Z80) port resolved interrupt */
    GT_UINTPTR  evntHndl;    /* event handler pointer */

    /* AP engine enabled before phase 1 init */

    evHndlrCauseAllArr[0] = CPSS_PP_PORT_802_3_AP_E;

    /* lock section to disable interruption of ISR while unmasking events */
    INT_LOCK(intKey);

    rc = cpssEventBind(evHndlrCauseAllArr, 1, &evntHndl);
    if (GT_OK != rc)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssEventBind", rc);        
        INT_UNLOCK(intKey);
        return rc;
    }

    /* call the CPSS to enable those interrupts in the HW of the device */
    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_PORT_802_3_AP_E, 
                                                            CPSS_EVENT_UNMASK_E);
    INT_UNLOCK(intKey);    
    if (GT_OK != rc)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssEventDeviceMaskSet", rc);        
        return rc;
    }

    cpssOsSprintf(apTaskName, "apD%d", devNum);
    rc = cpssOsTaskCreate(apTaskName,               /* Task Name      */
                          210,                        /* Task Priority  */
                          _32K,                     /* Stack Size     */
                          bc2AppDemoApHndlTask,   /* Starting Point */
                          (GT_VOID*)evntHndl,         /* Arguments list */
                          &apTaskId);               /* task ID        */
    
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssOsTaskCreate:bc2AppDemoApHndlTask", rc);        
    return rc;
}



#endif 


