/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
*******************************************************************************
* mvHwsPortCtrlOs.c
*
* DESCRIPTION:
*       Port Control OS Initialization
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>

/* Port Control Process Handlers */
xTaskHandle mvHwsSupervisorHandler;
xTaskHandle mvHwsGeneralHandler;
xTaskHandle mvHwsPortHandler;
xTaskHandle mvHwsApPortHandler;
xTaskHandle mvHwsApDetectHandler;
#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
xTaskHandle mvHwsDebugHandler;
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

/* Port Control Semaphore Handlers */
#ifdef BOBK_DEV_SUPPORT
xSemaphoreHandle mvHwsAvagoSem;
#endif /* BOBK_DEV_SUPPORT */
xSemaphoreHandle mvHwsSyncSem;
xSemaphoreHandle mvHwsLogSem;
xSemaphoreHandle mvHwsIpcSem;
#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
xSemaphoreHandle mvHwsDebugSem;
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

/* Port Control Queue Handlers */
xQueueHandle mvHwsGeneralQueue;
xQueueHandle mvHwsPortQueue;
xQueueHandle mvHwsApDetectQueue;
xQueueHandle mvHwsPortCtrlQueue[] =
{
    [M0_DEBUG]           NULL,
    [M1_SUPERVISOR]      NULL,
    [M2_PORT_MNG]        NULL,
    [M3_PORT_SM]         NULL,
    [M4_AP_PORT_MNG]     NULL,
    [M5_AP_PORT_DET]     NULL,
    [M5_AP_PORT_DET_EXT] NULL,
    [M6_GENERAL]         NULL
};

/*******************************************************************************
* mvPortCtrlOsResourcesInit
*
* DESCRIPTION: Allocate and Initialize OS Resources
*              - Allocate system memoty queues
*              - Allocate system semaphores
*              - Allocate system processes
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlOsResourcesInit(void)
{
    /* Message Queue Section */
    /* ===================== */
    mvHwsGeneralQueue = xQueueCreate(MV_GENERAL_MSG_NUM, MV_GENERAL_MSG_SIZE);
    mvHwsPortCtrlQueue[M6_GENERAL] = mvHwsGeneralQueue;
    if(mvHwsGeneralQueue == 0)
    {
        mvPcPrintf("Port Ctrl Error, Failed to create General queue\n");
        return GT_FAIL;
    }

    mvHwsApDetectQueue = xQueueCreate(MV_AP_MSG_NUM, MV_AP_MSG_SIZE);
    mvHwsPortCtrlQueue[M5_AP_PORT_DET] = mvHwsApDetectQueue;
    if(mvHwsApDetectQueue == 0)
    {
        mvPcPrintf("Port Ctrl Error, Failed to create AP Detect queue\n");
        return GT_FAIL;
    }

    mvHwsPortQueue = xQueueCreate(MV_PORT_MSG_NUM, MV_PORT_MSG_SIZE);
    mvHwsPortCtrlQueue[M2_PORT_MNG] = mvHwsPortQueue;
    if(mvHwsPortQueue == 0)
    {
        mvPcPrintf("Port Ctrl Error, Failed to create Port Init queue\n");
        return GT_FAIL;
    }

    /* Semaphore Section */
    /* ================= */
#ifdef BOBK_DEV_SUPPORT
    mvHwsAvagoSem = xSemaphoreCreateMutex();
    if(mvHwsAvagoSem == NULL)
    {
        mvPcPrintf("Port Ctrl Error, Failed to create Avago Semaphore\n");
        return GT_FAIL;
    }
#endif /* BOBK_DEV_SUPPORT */

    vSemaphoreCreateBinary(mvHwsSyncSem);
    if(mvHwsSyncSem != NULL)
    {
        if(xSemaphoreTake(mvHwsSyncSem, (portTickType) 10) != pdTRUE)
        {
            mvPcPrintf("Port Ctrl Error, Failed to take Sync Semaphore\\n");
            return GT_FAIL;
        }
    }
    else
    {
        mvPcPrintf("Port Ctrl Error, Failed to create Sync Semaphore\\n");
        return GT_FAIL;
    }

    mvHwsLogSem = xSemaphoreCreateMutex();
    if(mvHwsLogSem == NULL)
    {
        mvPcPrintf("Port Ctrl Error, Failed to create Log Semaphore\n");
        return GT_FAIL;
    }

    vSemaphoreCreateBinary(mvHwsIpcSem);
    if(mvHwsIpcSem == NULL)
    {
        mvPcPrintf("Port Ctrl Error, Failed to create IPC Semaphore\n");
        return GT_FAIL;
    }

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
    vSemaphoreCreateBinary(mvHwsDebugSem);
    if(mvHwsDebugSem == NULL)
    {
        mvPcPrintf("Port Ctrl Error, Failed to create Debug Semaphore\n");
        return GT_FAIL;
    }

    xSemaphoreTake(mvHwsDebugSem, (portTickType) portMAX_DELAY);
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

    /* Process Section */
    /* =============== */

    /* M1_SUPERVISOR Process */
    if (xTaskCreate(mvPortCtrlSpvRoutine,
                (signed char*) "Supervisor",
                MV_SUPERVISOR_STACK,
                NULL,
                MV_SUPERVISOR_PRIORITY,
                &mvHwsSupervisorHandler) != pdPASS)
    {
        mvPcPrintf("Info, Create mvPortCtrlSpvRoutine Failed\n");
        return GT_FAIL;
    }

    /* M2_PORT_MNG Process */
    if (xTaskCreate(mvPortCtrlPortRoutine,
                (signed char*) "Port Mng",
                MV_PORT_STACK,
                NULL,
                MV_PORT_PRIORITY,
                &mvHwsPortHandler) != pdPASS)
    {
        mvPcPrintf("Info, Create mvPortCtrlPortRoutine Failed\n");
        return GT_FAIL;
    }

    /* M4_AP_PORT_MNG Process */
    if (xTaskCreate(mvPortCtrlApPortRoutine,
                (signed char*) "AP Port Mng",
                MV_PORT_STACK,
                NULL,
                MV_PORT_PRIORITY,
                &mvHwsApPortHandler) != pdPASS)
    {
        mvPcPrintf("Info, Create mvPortCtrlApPortRoutine Failed\n");
        return GT_FAIL;
    }

    /* M5_AP_PORT_DET Process */
    if (xTaskCreate(mvHwsApDetectRoutine,
                (signed char*) "AP Detect",
                MV_AP_STACK,
                NULL,
                MV_AP_PRIORITY,
                &mvHwsApDetectHandler) != pdPASS)
    {
        mvPcPrintf("Info, Create mvHwsApRoutine Failed\n");
        return GT_FAIL;
    }

    /* M6_GENERAL Process */
    if (xTaskCreate(mvPortCtrlGenRoutine,
                (signed char*) "General",
                MV_GENERAL_STACK,
                NULL,
                MV_GENERAL_PRIORITY,
                &mvHwsGeneralHandler) != pdPASS)
    {
        mvPcPrintf("Info, Create mvPortCtrlGenRoutine Failed\n");
        return GT_FAIL;
    }

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
    if (xTaskCreate(mvPortCtrlDebugRoutine,
                (signed char*) "Debug",
                MV_DEBUG_STACK,
                NULL,
                MV_DEBUG_PRIORITY,
                &mvHwsDebugHandler) != pdPASS)
    {
        mvPcPrintf("Info, Create mvPortCtrlGenRoutine Failed\n");
        return GT_FAIL;
    }
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlSyncLock
* mvPortCtrlSyncUnlock
*
* DESCRIPTION: These two functions are used to synchronize the start of
*              execution of al processes
*
*              All process are blocked on mvPortCtrlProcessWait
*              the main routine will enable the semaphore (Give)
*              the first process that successfully take the semaphore
*              immediately release it to the next process,  untill all process
*              are active and running
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlSyncLock(void)
{
    if(xSemaphoreTake(mvHwsSyncSem, (portTickType) portMAX_DELAY) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Take sync semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

GT_STATUS mvPortCtrlSyncUnlock(void)
{
    if(xSemaphoreGive(mvHwsSyncSem) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Give sync semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlLogLock
* mvPortCtrlLogUnlock
*
* DESCRIPTION: These two functions are used to protect real time log
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlLogLock(void)
{
    if(xSemaphoreTake(mvHwsLogSem, (portTickType) portMAX_DELAY) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Take log semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

GT_STATUS mvPortCtrlLogUnlock(void)
{
    if(xSemaphoreGive(mvHwsLogSem) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Give log semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlIpcLock
* mvPortCtrlIpcUnlock
*
* DESCRIPTION: These two functions are used to protect IPC
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlIpcLock(void)
{
    if(xSemaphoreTake(mvHwsIpcSem, (portTickType) portMAX_DELAY) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Take IPC semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

GT_STATUS mvPortCtrlIpcUnlock(void)
{
    if(xSemaphoreGive(mvHwsIpcSem) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Give IPC semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

#ifdef BOBK_DEV_SUPPORT
/*******************************************************************************
* mvPortCtrlAvagoLock
* mvPortCtrlAvagoUnlock
*
* DESCRIPTION: These two functions are used to protect Avago access
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlAvagoLock(void)
{
    if(xSemaphoreTake(mvHwsAvagoSem, (portTickType) portMAX_DELAY) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Take Avago semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

GT_STATUS mvPortCtrlAvagoUnlock(void)
{
    if(xSemaphoreGive(mvHwsAvagoSem) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Give Avago semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}
#endif /* BOBK_DEV_SUPPORT */

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE
/*******************************************************************************
* mvPortCtrlDbgLock
* mvPortCtrlDbgUnlock
*
* DESCRIPTION: These two functions are sync debug print
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlDbgLock(void)
{
    if(xSemaphoreTake(mvHwsDebugSem, (portTickType) portMAX_DELAY) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Take Debug semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}

GT_STATUS mvPortCtrlDbgUnlock(void)
{
    if(xSemaphoreGive(mvHwsDebugSem) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, Failed to Give Debug semaphore\n");
        return GT_FAIL;
    }

    return GT_OK;
}
#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

/*******************************************************************************
* mvPortCtrlCurrentTs
*
* DESCRIPTION: Return current time stamp
*
* INPUTS:
*       None
*
* OUTPUTS:
*       Current time stamp in msec
*
* RETURNS:
*       None
*
*******************************************************************************/
GT_U32 mvPortCtrlCurrentTs(void)
{
    return (xTaskGetTickCount() / portTICK_RATE_MS);
}

/*******************************************************************************
* mvPortCtrlProcessDelay
*
* DESCRIPTION: Delay process execution for [x] msec
*
* INPUTS:
*       GT_U32 duration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlProcessDelay(GT_U32 duration)
{
    const portTickType xDelay = duration / portTICK_RATE_MS;

    vTaskDelay(xDelay);

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlThresholdSet
*
* DESCRIPTION: Set threshold for port
*
* INPUTS:
*       GT_U32 delay - duration in ms
*       GT_U16 *thrershold - target threshold
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
*******************************************************************************/
void mvPortCtrlThresholdSet(GT_U32 delay, GT_U32 *thrershold)
{
    GT_U32 currentTimestamp;

    if (delay == PORT_CTRL_TIMER_DEFAULT)
    {
        *thrershold = 0;
    }
    else
    {
        currentTimestamp = xTaskGetTickCount() / portTICK_RATE_MS;
        *thrershold = currentTimestamp + (delay / portTICK_RATE_MS);
    }
}

/*******************************************************************************
* mvPortCtrlThresholdCheck
*
* DESCRIPTION: Check if port threshold exceeded
*
* INPUTS:
*       GT_U32 thrershold - target threshold
*
* OUTPUTS:
*       None
*
* RETURNS:
*       1 - threshold exceeded
*       0 - threshold not exceeded
*
*******************************************************************************/
GT_U32 mvPortCtrlThresholdCheck(GT_U32 thrershold)
{
    GT_U32 currentTimestamp;

    if (thrershold > PORT_CTRL_TIMER_DEFAULT)
    {
        currentTimestamp = xTaskGetTickCount() / portTICK_RATE_MS;

        if (currentTimestamp >= thrershold)
        {
            return 1;
        }
    }

    return 0;
}

/*******************************************************************************
* mvPortCtrlProcessPendMsgNum
*
* DESCRIPTION: Receive number of messages pending in one of the system message
*              queues
*
* INPUTS:
*       MV_PORT_CTRL_MODULE module
*       GT_U32              msgNum
*
* OUTPUTS:
*       Number of pending messages in the queue
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlProcessPendMsgNum(MV_PORT_CTRL_MODULE module, GT_U32 *msgNum)
{
    if (mvHwsPortCtrlQueue[module] == NULL)
    {
        return GT_FAIL;
    }

    *msgNum = (GT_U32)uxQueueMessagesWaiting(mvHwsPortCtrlQueue[module]);

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlProcessMsgRecv
*
* DESCRIPTION: Receive a message from one of the system message queues
*              duration defines the time to wait until the message is received
*              in case the queue is empty.
*              duration == 0, return immediately
*
* INPUTS:
*       MV_PORT_CTRL_MODULE        module
*       GT_U32                     duration
*       MV_HWS_IPC_CTRL_MSG_STRUCT *msgPtr
*
* OUTPUTS:
*       MV_HWS_IPC_CTRL_MSG_STRUCT* received message pointer
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlProcessMsgRecv(MV_PORT_CTRL_MODULE module,
                                   GT_U32 duration,
                                   MV_HWS_IPC_CTRL_MSG_STRUCT *msgPtr)
{
    if (mvHwsPortCtrlQueue[module] == NULL)
    {
        return GT_FAIL;
    }

    if(xQueueReceive(mvHwsPortCtrlQueue[module], msgPtr, (portTickType) duration / portTICK_RATE_MS) != pdTRUE)
    {
        return GT_EMPTY;
    }

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlProcessMsgSend
*
* DESCRIPTION: Send a message to a system message queues
*
* INPUTS:
*       MV_PORT_CTRL_MODULE  module
*       MV_HWS_INTERNAL__MSG *msgPtr
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvPortCtrlProcessMsgSend(MV_PORT_CTRL_MODULE module,
                                   MV_HWS_IPC_CTRL_MSG_STRUCT *msgPtr)
{
    if (mvHwsPortCtrlQueue[module] == NULL)
    {
        return GT_FAIL;
    }

    if((xQueueSend(mvHwsPortCtrlQueue[module], msgPtr, (portTickType) MV_PROCESS_MSG_TX_DELAY)) != pdTRUE)
    {
        mvPcPrintf("Port Ctrl Error, module %d Queue FULL\n", module);
        return GT_FAIL;
    }

    return GT_OK;
}


