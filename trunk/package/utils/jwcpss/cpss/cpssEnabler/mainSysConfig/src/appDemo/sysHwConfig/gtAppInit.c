/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppInit.c
*
* DESCRIPTION:
*       This file includes functions to be called on system initialization,
*       for demo and special purposes.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 47 $
*
*******************************************************************************/



#include <gtOs/gtOsGen.h>
#include <gtOs/gtOsMem.h>
#include <gtOs/gtOsIo.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/shell/cmdMain.h>

/*support 'makefile' ability to define the size of the allocation */
#ifndef APP_DEMO_OSMEM_DEFAULT_MEM_INIT_SIZE
    /* Default memory size */
    #define APP_DEMO_OSMEM_DEFAULT_MEM_INIT_SIZE (2048*1024)
#endif /* ! APP_DEMO_OSMEM_DEFAULT_MEM_INIT_SIZE */


GT_STATUS appDemoCpssInit(GT_VOID);
#define INIT_BEFORE_SHELL
/* sample code fo initializatuion befor the shell starts */
#ifdef INIT_BEFORE_SHELL

#include <gtOs/gtOsSem.h>
#include <gtOs/gtOsTask.h>

#include <extUtils/trafficEngine/tgfTrafficGenerator.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h>
GT_SEM semCmdSysInit;

GT_STATUS cpssEnableCpuCommunication()
{
	GT_STATUS rc;
	int i = 0;

    CPSS_INTERFACE_INFO_STC outPortInterface;

#if 0
    GT_U32 windowResolution = 100000;
    GT_U32 pktLimit = 20;
    GT_U32 windowSize = 4000;
    rc = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(0,windowResolution);
    if(rc != GT_OK)
   	{
    	osPrintf("set WindowResolution %d nSec failed\n",windowResolution);
   	}
    else
    {
    	rc = cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(0,&windowResolution);
    	if(rc != GT_OK)
    	{
    		osPrintf("get WindowResolution nSec failed\n");
    	}
    	else
    	{
    		osPrintf("get WindowResolution true val %d nSec \n",windowResolution);
    		rc = cpssDxChNetIfCpuCodeRateLimiterTableSet(0,1,windowSize, pktLimit);
    		if(rc != GT_OK)
    		{
    			osPrintf("set rate limit failed\n");
    		}
    		else
    		{
    			osPrintf("set rate limit succeed \n");
    		}
    	}
    }

#endif
#if 0
    rc = cpssDxChBrgVlanIpCntlToCpuSet(0,1,CPSS_DXCH_BRG_IP_CTRL_IPV4_IPV6_E);
    if(rc != GT_OK)
    {
     	osPrintf("cpssDxChBrgVlanIpCntlToCpuSet failed ret is %d \n",rc);
    }

    rc = cpssDxChBrgGenArpBcastToCpuCmdSet(0,CPSS_DXCH_ARP_BCAST_CMD_MODE_VLAN_E,CPSS_PACKET_CMD_MIRROR_TO_CPU_E);
    if(rc != GT_OK)
    {
    	osPrintf("cpssDxChBrgGenArpBcastToCpuCmdSet failed ret is %d \n",rc);
    }
#endif

#ifdef CONFIG_INFO_MSYS_24G4SFP
	for(i = 0; i < 28; i++)
	{
		osMemSet(&outPortInterface, 0, sizeof(outPortInterface));
	    outPortInterface.type            = CPSS_INTERFACE_PORT_E;
	    outPortInterface.devPort.hwDevNum  = 0;
	    outPortInterface.devPort.portNum   = i;

	    rc = tgfTrafficGeneratorPortTxEthCaptureSet(
	            &outPortInterface, TGF_CAPTURE_MODE_PCL_E, GT_TRUE);
	    if(rc != GT_OK)
	    {
	    	osPrintf("enable pcl policy on (%d, %d) failed \n",0,i);
	    }

	}

#endif

	return GT_OK;
}

GT_STATUS cpssStartObcpClientProcess()
{
	GT_STATUS rc;

	if(access("obcpSwitch",0) != -1)
	{
		 pid_t childPid = -1;
		 childPid = fork();

		 if (childPid == 0)
		 {
			 char *rtspArgv[] = {"obcpSwitch", 0};
			 execv(rtspArgv[0], rtspArgv);
		 }
		 else if (childPid == -1)
		 {
			 osPrintf("start obcpSwitch failed \n");
		 }
	}

	return GT_OK;
}

/*******************************************************************************
* cpssInitSystemCall
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
GT_STATUS cpssInitSystemCall(GT_VOID)
{
    GT_STATUS rc;
    GT_ETHERADDR myMac;
    osPrintf("cpssInitSystem(19,2,0) b will be called\n");
    rc = cpssInitSystem(1,2,0);
    osPrintf("cpssInitSystem(19,2,0) returned 0x%X\n", rc );
#if 0
    /**<set pcl action to enable mirror packet to cpu*/
    cpssEnableCpuCommunication();

    osNetworkStackInit();
    myMac.arEther[0]=0x00;
    myMac.arEther[1]=0x90;
    myMac.arEther[2]=0x9A;
    myMac.arEther[3]=0x9F;
    myMac.arEther[4]=0xA9;
    myMac.arEther[5]=0x3A;

    rc = osNetworkStackConnect(0, 1, &myMac);
    osPrintf("osNetworkStackConnect returned 0x%X\n", rc );
#endif
#if 0
    rc = osSemSignal(semCmdSysInit);
    osPrintf("Signal semaphore, rc = 0x%X\n", rc);
#endif
    return GT_OK;
}
/*******************************************************************************
* cpssInitSystemCallTask
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
GT_STATUS cpssInitSystemCallTask(GT_VOID)
{
    GT_STATUS rc;
    GT_U32    taskSysInitCmd;

    rc = osWrapperOpen(NULL);
    osPrintf("osWrapperOpen called, rc = 0x%X\n", rc);
#if 0
    rc = osSemBinCreate("semCmdSysInit", OS_SEMB_EMPTY, &semCmdSysInit);
    osPrintf("Create semCmdSysInit semaphore, rc = 0x%X\n", rc);
#endif
    osTimerWkAfter(1000);
    rc = osTaskCreate (
        "CMD_InitSystem", 10 /* priority */,
        32768 /*stack size */,
        (unsigned (__TASKCONV *)(void*)) cpssInitSystemCall,
        NULL, &taskSysInitCmd);
    osPrintf("Create taskSysInitCmd task, rc = 0x%X, id = 0x%X \n",
           rc, taskSysInitCmd);
#if 0
    rc = osSemWait(semCmdSysInit, 600000); /* 10 minutes */
    osTimerWkAfter(1000);
    osPrintf("Wait for semaphore, rc = 0x%X\n", rc);
#endif
    return GT_OK;
}

#endif /*INIT_BEFORE_SHELL*/

/*******************************************************************************
* userAppInitialize
*
* DESCRIPTION:
*       This routine is the starting point of the Driver.
*       Called from  userAppInit() or from win32:main()
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
GT_STATUS userAppInitialize(
    IN GT_U32 devNum
)
{
    GT_STATUS rc;

    /* must be called before any Os function */
    rc = osWrapperOpen(NULL);
    if(rc != GT_OK)
    {
        osPrintf("osWrapper initialization failure!\n");
        return rc;
    }

    /* Initialize memory pool. It must be done before any memory allocations */
    /* must be before osWrapperOpen(...) that calls osStaticMalloc(...) */
    rc = osMemInit(APP_DEMO_OSMEM_DEFAULT_MEM_INIT_SIZE, GT_TRUE);
    if (rc != GT_OK)
    {
        osPrintf("osMemInit() failed, rc=%d\n", rc);
        return rc;
    }

    /* run appDemoCpssInit() directly, not from console => show board list => show sw version
     * this allow application to work without console task */
    rc = appDemoCpssInit();
    if (rc != GT_OK)
    {
        osPrintf("appDemoCpssInit() failed, rc=%d\n", rc);
    }

    /* Set gtInitSystem to be the init function */
    /*cpssInitSystemFuncPtr = (GT_INTFUNCPTR)cpssInitSystem;*/

#if 1 /*def INIT_BEFORE_SHELL*/
    osPrintf("cpssInitSystemCallTask will be called\n");
    cpssInitSystemCall();
    /*cpssInitSystemCallTask();*/
    osPrintf("cpssInitSystemCallTask was called\n");
#endif /*INIT_BEFORE_SHELL*/
    /* Start the command shell */

#if 0
    cmdInit(devNum);
#endif
    return GT_OK;
} /* userAppInitialize */

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
    return userAppInitialize(0);
} /* userAppInit */



