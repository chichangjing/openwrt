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
* mvHwsPortCtrlInit.c
*
* DESCRIPTION:
*       Port Control Initialization
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <mvHwsPortCtrlInc.h>
#include <mvHwsServiceCpuInt.h>

/*******************************************************************************
* mvPortCtrlHwInit
*
* DESCRIPTION: Initialize HW related tasks
*              - IPC infrastructure
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
GT_STATUS mvPortCtrlHwInit(void)
{
    /* Initialize Interrupts */
    mvHwsServCpuIntrInit(0/* To Add Shared memory section for interrupt map*/);

    /* Initialize IPC */
    CHECK_STATUS(mvPortCtrlIpcInit());

    return GT_OK;
}

/*******************************************************************************
* mvPortCtrlSwInit
*
* DESCRIPTION: Initialize Application related tasks
*              - Initialize Firmware - AAPL structures
*              - Initialize Database
*              - Initialize Real-time Log
*              - Initialize OS Resources
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
GT_STATUS mvPortCtrlSwInit(void)
{
    /* Initialize Device */
    CHECK_STATUS(mvPortCtrlDevInit());
    /* Initialize Database */
    CHECK_STATUS(mvPortCtrlDbInit());
    /* Initialize Real-time Log */
    CHECK_STATUS(mvPortCtrlLogInit());
    /* Initialize OS Resources */
    CHECK_STATUS(mvPortCtrlOsResourcesInit());

    return GT_OK;
}


