/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpConGenInit.c
*
* DESCRIPTION:
*       Low level driver initialization of PPs, and declarations of global
*       variables
*
* FILE REVISION NUMBER:
*       $Revision: 30 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpssDriver/log/private/prvCpssDriverLog.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpssDriver/pp/interrupts/cpssDrvInterrupts.h>

#include <cpss/generic/events/private/prvCpssGenEvReq.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterruptsInit.h>

#ifdef  INCL_EXDXMX_DRIVER
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#endif

#define DEBUG_PRINT(x) cpssOsPrintf x

/*****************************************************************************
* Global
******************************************************************************/
/* array of pointers to the valid driver devices
   index to this array is devNum */
PRV_CPSS_DRIVER_PP_CONFIG_STC* prvCpssDrvPpConfig[PRV_CPSS_MAX_PP_DEVICES_CNS] =
    {NULL};

/* array including all driver management interface objects in Unit       */
PRV_CPSS_DRV_OBJ_STC* prvCpssDrvPpObjConfig[PRV_CPSS_MAX_PP_DEVICES_CNS] =
    {NULL};

/* driver global data that should be saving in HSU*/
PRV_CPSS_DRV_GLOBAL_DATA_STC drvGlobalInfo = {0};


/* DB to store device type id
   Note: The DB used for debug purpose only
*/
GT_U16   prvCpssDrvDebugDeviceId[PRV_CPSS_MAX_PP_DEVICES_CNS];

/*  DB to store flag for tracing hw write per device
        GT_TRUE: HW trace write access is enabled
        GT_FALSE: HW trace write access is disabled
*/
GT_BOOL  prvCpssDrvTraceHwWrite[PRV_CPSS_MAX_PP_DEVICES_CNS];

/*  DB to store flag for tracing hw read per device
        GT_TRUE: HW trace read access is enabled
        GT_FALSE: HW trace read access is disabled
*/
GT_BOOL  prvCpssDrvTraceHwRead[PRV_CPSS_MAX_PP_DEVICES_CNS];

/*  DB to store flag for tracing hw delay per device
        GT_TRUE: HW trace delay access is enabled
        GT_FALSE: HW trace delay access is disabled
*/
GT_BOOL  prvCpssDrvTraceHwDelay[PRV_CPSS_MAX_PP_DEVICES_CNS];

#ifdef CHX_FAMILY
/*Array of unsupported units for Bobcat2*/
PRV_CPSS_DXCH_UNIT_ENT bobcat2UnsupportedUnits[]={
        PRV_CPSS_DXCH_UNIT_GOP_E,
        PRV_CPSS_DXCH_UNIT_MIB_E,
        PRV_CPSS_DXCH_UNIT_SERDES_E,
        PRV_CPSS_DXCH_UNIT_ILKN_E,
        PRV_CPSS_DXCH_UNIT_LMS0_0_E,
        PRV_CPSS_DXCH_UNIT_LMS0_1_E,
        PRV_CPSS_DXCH_UNIT_LMS0_2_E,
        PRV_CPSS_DXCH_UNIT_LMS0_3_E,
        PRV_CPSS_DXCH_UNIT_LMS1_0_E,
        PRV_CPSS_DXCH_UNIT_LMS1_1_E,
        PRV_CPSS_DXCH_UNIT_LMS1_2_E,
        PRV_CPSS_DXCH_UNIT_LMS1_3_E,
        PRV_CPSS_DXCH_UNIT_LMS2_0_E,
        PRV_CPSS_DXCH_UNIT_LMS2_1_E,
        PRV_CPSS_DXCH_UNIT_LMS2_2_E,
        PRV_CPSS_DXCH_UNIT_LMS2_3_E,
        PRV_CPSS_DXCH_UNIT_CNC_0_E,
        PRV_CPSS_DXCH_UNIT_CNC_1_E,
        /* There is no more LMS unit starting from Bobcat2 B0. LMS was split to SMI , LED units */
        PRV_CPSS_DXCH_UNIT_SMI_0_E,
        PRV_CPSS_DXCH_UNIT_SMI_1_E,
        PRV_CPSS_DXCH_UNIT_SMI_2_E,
        PRV_CPSS_DXCH_UNIT_SMI_3_E,
        PRV_CPSS_DXCH_UNIT_LED_0_E,
        PRV_CPSS_DXCH_UNIT_LED_1_E,
        PRV_CPSS_DXCH_UNIT_LED_2_E,
        PRV_CPSS_DXCH_UNIT_LED_3_E,
        PRV_CPSS_DXCH_UNIT_LED_4_E
};
#endif

/*****************************************************************************
* Externals
******************************************************************************/

/*******************************************************************************
* drvEventMaskDevice
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* INPUTS:
*       devNum - PP's device number .
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                     queue number / other ) the event was received upon.
*                   may use value PRV_CPSS_DRV_EV_REQ_UNI_EV_EXTRA_DATA_ANY_CNS
*                   to indicate 'ALL interrupts' that relate to this unified
*                   event
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event or the evExtData within the unified
*                      event are not found in this device interrupts
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       This function called when OS interrupts and the ExtDrv interrupts are
*       locked !
*
*******************************************************************************/
static GT_STATUS drvEventMaskDevice
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData,
    IN CPSS_EVENT_MASK_SET_ENT  operation
)
{
    GT_STATUS  rc = GT_OK;
    GT_U32     portGroupId;/* port group Id */
    GT_BOOL    maskSetOk = GT_FALSE;

    /* check that the device exists and properly initialized */
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc = prvCpssDrvEvReqUniEvMaskSet(
                        prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool,
                        prvCpssDrvPpConfig[devNum]->intCtrl.numOfIntBits,
                            (GT_U32)uniEvent, evExtData, operation);
         /*
                GT_OK means at least one mask was set in the portGroup.
                GT_NOT_FOUND means this event and extra data was not found for this
                portGroupId.
                Some of the event don't exists in all port group therefore it is ok to not find the
                unified event or extra data in a port group. All port groups must
                be searched before the event is said to not be found.
             */
        if (rc == GT_OK)
        {
            maskSetOk = GT_TRUE;
        }

        /* GT_OK and GT_NOT_FOUND are only allowed */
        if ((rc != GT_NOT_FOUND) && (rc != GT_OK))
        {
            return rc;
        }
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if (maskSetOk)
    {
        return GT_OK;
    }

    return rc;
}

/*******************************************************************************
* prvFindCauseRegAddr
*
* DESCRIPTION:
*       This function finds cause register address in
*       CPSS_INTERRUPT_SCAN_STC tree by mask register address
*
*  APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3.
*
* INPUTS:
*       intScanRoot - CPSS_INTERRUPT_SCAN_STC tree.
*       maskReg     - Address of the interrupt mask register.
*
* OUTPUTS:
*       regAddr - Address of the interrupt cause register.
*       found   - true if cause register address was found in
*       CPSS_INTERRUPT_SCAN_STC tree, false otherwise.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID prvFindCauseRegAddr
(
    IN  PRV_CPSS_DRV_INTERRUPT_SCAN_STC* intScanRoot,
    IN  GT_U32 maskReg ,
    OUT GT_U32 *regAddr,
    OUT GT_BOOL *found
)
{
    GT_U32 i;
    if (maskReg==intScanRoot->maskRegAddr)
    {
        *regAddr=intScanRoot->causeRegAddr;
        *found=GT_TRUE;
        return;
    }
    for(i = 0; i < intScanRoot->subIntrListLen; i++)
    {
        prvFindCauseRegAddr(intScanRoot->subIntrScan[i],maskReg,regAddr,found);
    }
        return;
}

/*******************************************************************************
* prvCheckUnit
*
* DESCRIPTION:
*       This function checks if interrupt cause address register address is in
*       supported unit.
*
*  APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3.
*
* INPUTS:
*       devNum   - Device number.
*       causeReg - Address of the interrupt cause register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - interrupt cause register is in supported unit
*       GT_NOT_SUPPORTED - interrupt cause register is in unsupported unit
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCheckUnit
(
    IN GT_U8 devNum,
    IN GT_U32 causeReg
)
{
#ifdef CHX_FAMILY
    PRV_CPSS_DXCH_UNIT_ENT unit;
    GT_U32                 unitItr; /* unit iterator */
    GT_U32                 unsupportedUnitsArraySize; /* size of bobcat2UnsupportedUnits array */

    unsupportedUnitsArraySize=sizeof(bobcat2UnsupportedUnits)/
                              sizeof(bobcat2UnsupportedUnits[0]);

    if(prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E ||
       prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E ||
       prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
    {
        unit =  prvCpssDxChHwRegAddrToUnitIdConvert (devNum, causeReg);
        for (unitItr = 0; unitItr < unsupportedUnitsArraySize; unitItr++)
        {
            if (unit == bobcat2UnsupportedUnits[unitItr])
            {
                /* Return GT_NOT_SUPPORTED if unit is in list of unsupported units*/
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
#else
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
#endif
}

/*******************************************************************************
* drvEventGenerateDevice
*
* DESCRIPTION:
*       This debug routine configures device to generate unified event for
*       specific element in the event associated with extra data.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3.
*
* INPUTS:
*       devNum - device number - PP/FA/Xbar device number -
*                depend on the uniEvent
*                if the uniEvent is in range of PP events , then devNum relate
*                to PP
*                if the uniEvent is in range of FA events , then devNum relate
*                to FA
*                if the uniEvent is in range of XBAR events , then devNum relate
*                to XBAR
*       uniEvent   - The unified event.
*       evExtData - The additional data (port num / priority
*                   queue number / other ) the event was received upon.
*                   Use CPSS_PARAM_NOT_USED_CNS to generate events for all supported evExtData.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*       GT_NOT_FOUND - the unified event has no interrupts associated with it in
*                      the device Or the 'extra data' has value that not relate
*                      to the uniEvent
*       GT_NOT_INITIALIZED - the CPSS was not initialized properly to handle
*                   this type of event
*       GT_HW_ERROR - on hardware error
*       GT_NOT_SUPPORTED - on not supported unify event
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS drvEventGenerateDevice
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN GT_U32                   evExtData
)
{
    GT_U32            eventItr; /* event iterator */
    GT_STATUS   rc = GT_OK; /* return code*/
    GT_U32      portGroupId; /* port group Id */
    PRV_CPSS_DRV_EV_REQ_NODE_STC node; /*interrupt node*/
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC* intScanRoot; /*interrupt information hierarchy tree root*/
    GT_U32      mask; /* cause register mask */
    GT_U32      maskReg; /* address of mask register */
    GT_U32      causeReg; /* address of cause register */
    GT_U32      nodesPoolSize; /* size of nodes pool */
    GT_U32      found=0; /* number of found events in nodes pool */
    GT_BOOL     foundCause; /* variable indicates that cause register address was found
                             in intScanRoot tree (true - found; false - not found) */

    /* check that the device exists and properly initialized */
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);
    /*Check the device family*/
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
            CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E |
            CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E |
            CPSS_PUMA_E | CPSS_PUMA3_E | CPSS_XCAT3_E);

    /*Get Size of Nodes Pool*/
    nodesPoolSize=prvCpssDrvPpConfig[devNum]->intCtrl.numOfIntBits;
    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /*Find unified event by number or additional data and write bit of cause register*/
        for (eventItr = 0; eventItr < nodesPoolSize; eventItr++)
        {
            node=prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool[eventItr];
            if (node.uniEvCause != (GT_U32)uniEvent)
            {
                continue;
            }
            if(evExtData != CPSS_PARAM_NOT_USED_CNS)
            {
                if(node.uniEvExt != evExtData)
                {
                    /* we need to generate event only for the specific interrupts and not all
                     * interrupts that connected to the unified event */
                    continue;
                }
            }
            found++;
            /*Get mask and mask register address*/
            mask = node.intBitMask;
            maskReg = node.intMaskReg;
            /*Get Cause Register address*/
            intScanRoot = prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intScanRoot;
            foundCause = GT_FALSE;
            prvFindCauseRegAddr(intScanRoot,maskReg,&causeReg,&foundCause);
            if (foundCause == GT_TRUE)
            {
                /*Check if the event can be generated*/
                rc = prvCheckUnit(devNum, causeReg);
                if (rc != GT_OK)
                    return rc;
                /*Write data to cause register to generate event*/
                rc = prvCpssHwPpWriteRegBitMask(devNum,causeReg,mask,mask);
                if (rc != GT_OK)
                    return rc;
            }
            else
            {
                /*Return GT_FAIL if cause register address hasn't been found in intScanRoot tree*/
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
            }
        }
        if (found == 0)
        {
            /*Return GT_NOT_FOUND if unified event hasn't been found in nodes pool*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
        if (rc != GT_OK)
            return rc;
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId);
    return GT_OK;
}

/*******************************************************************************
* drvEventGppIsrConnect
*
* DESCRIPTION:
*       This function connects an Isr for a given Gpp interrupt.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR) with GPP
*
* INPUTS:
*       devNum      - The Pp device number at which the Gpp device is connected.
*       gppId       - The Gpp Id to be connected.
*       isrFuncPtr  - A pointer to the function to be called on Gpp interrupt
*                     reception.
*       cookie      - A cookie to be passed to the isrFuncPtr when its called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To disconnect a Gpp Isr, call this function with a NULL parameter in
*       the isrFuncPtr param.
*
*******************************************************************************/
static GT_STATUS drvEventGppIsrConnect
(
    IN  GT_U8                   devNum,
    IN  CPSS_EVENT_GPP_ID_ENT   gppId,
    IN  CPSS_EVENT_ISR_FUNC     isrFuncPtr,
    IN  void                    *cookie
)
{
    /* check that the device exists and properly initialized */
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    return prvCpssDrvDxExMxGppIsrConnect(devNum,gppId,isrFuncPtr,cookie);
}

/*******************************************************************************
* prvCpssDrvSysConfigPhase1
*
* DESCRIPTION:
*       This function sets cpssDriver system level system configuration
*       parameters before any of the device's phase1 initialization .
*
*
*  APPLICABLE DEVICES: ALL systems must call this function
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvSysConfigPhase1
(
    void
)
{
    GT_STATUS   rc = GT_NOT_INITIALIZED;
    GT_U32      i;
    static GT_BOOL initPrvCpssDrvDebugDeviceId = GT_TRUE;

    cpssOsMemSet(prvCpssDrvPpConfig,0,sizeof(prvCpssDrvPpConfig));
    cpssOsMemSet(prvCpssDrvPpObjConfig,0,sizeof(prvCpssDrvPpObjConfig));
    drvGlobalInfo.performReadAfterWrite = GT_TRUE;
    drvGlobalInfo.prvCpssDrvAfterDbRelease = GT_FALSE;

    /* bind a callback function for the mask/unmask of unify-events for ALL PP
        devices */
    rc = prvCpssGenEventMaskDeviceBind(PRV_CPSS_DEVICE_TYPE_PP_E,&drvEventMaskDevice);

    /* bind a callback function that generates events*/
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssGenEventGenerateDeviceBind(PRV_CPSS_DEVICE_TYPE_PP_E,&drvEventGenerateDevice);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*  initialize debug DB */
    if( GT_TRUE == initPrvCpssDrvDebugDeviceId )
    {
        for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            prvCpssDrvDebugDeviceId[i] = 0;
        }
        initPrvCpssDrvDebugDeviceId = GT_FALSE;
    }

    /*  initialize hw access trace DBs */
    for (i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        prvCpssDrvTraceHwWrite[i] = GT_FALSE;
        prvCpssDrvTraceHwRead[i] = GT_FALSE;
        prvCpssDrvTraceHwDelay[i] = GT_FALSE;
    }


    /* build the needed bus objects */
#ifdef  INCL_EXDXMX_DRIVER
    rc = prvCpssGenEventGppIsrConnectBind(&drvEventGppIsrConnect);
    if(rc != GT_OK)
    {
        return rc;
    }
#ifdef GT_PCI
    prvCpssDrvMngInfPciPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfPciPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control PCI */
    rc = prvCpssDrvHwPciDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }
    prvCpssDrvMngInfPexMbusPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfPexMbusPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control PEX with 8 Address Completion Ranges  */
    rc = prvCpssDrvHwPexMbusDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }
#endif /*GT_PCI*/

#ifdef GT_SMI
    prvCpssDrvMngInfSmiPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfSmiPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control SMI */
    rc = prvCpssDrvHwSmiDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }
#endif /*GT_SMI*/

#ifdef GT_I2C
    prvCpssDrvMngInfTwsiPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfTwsiPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control TWSI */
    rc = prvCpssDrvHwTwsiDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }
#endif /*GT_I2C*/

/*#ifdef GT_PEX

    prvCpssDrvMngInfPexPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfPexPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    * pointer to the object that control PEX *
    rc = prvCpssDrvHwPexDriverObjectInit();

    if(rc != GT_OK)
    {
        return rc;
    }
#endif*/

#if defined GT_PCI  /*|| defined GT_PEX*/

    /* HA StandBy driver allocation for PCI and PEX management interfaces */
    prvCpssDrvMngInfPciHaStandbyPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    cpssOsMemSet(prvCpssDrvMngInfPciHaStandbyPtr,0,sizeof(PRV_CPSS_DRV_MNG_INF_OBJ_STC));
    /* pointer to the object that control PCI -- for HA standby */
    rc = prvCpssDrvHwPciStandByDriverObjectInit();
    if(rc != GT_OK)
    {
        return rc;
    }

#endif /*GT_PEX*/
#endif

#ifdef  INCL_SOHO_DRIVER
    /* the cpss is not implemented for SOHO - yet */
    rc = GT_NOT_IMPLEMENTED;
#endif

    return rc;
}


/*******************************************************************************
* prvCpssDrvHwPpPhase1Init
*
* DESCRIPTION:
*       This function is called by cpssExMxHwPpPhase1Init() or other
*       cpss "phase 1" family functions, in order to enable PP Hw access,
*       the device number provided to this function may
*       be changed when calling prvCpssDrvPpHwPhase2Init().
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       ppOutInfoPtr  - (pointer to)the info that driver return to caller.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_HW_ERROR - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPhase1Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC *ppOutInfoPtr
)
{
    GT_STATUS rc;

    if(prvCpssDrvPpObjConfig[devNum])
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    if(prvCpssDrvMngInfPciPtr == NULL &&
       prvCpssDrvMngInfPexMbusPtr == NULL &&
       prvCpssDrvMngInfPciHaStandbyPtr == NULL &&
       prvCpssDrvMngInfSmiPtr == NULL &&
       prvCpssDrvMngInfTwsiPtr == NULL)
    {
        /* this is adding of first device after all devices were removed */
        rc = prvCpssDrvSysConfigPhase1();
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* create the device object for the functions for this device */
    prvCpssDrvPpObjConfig[devNum] = cpssOsMalloc(sizeof(PRV_CPSS_DRV_OBJ_STC));
    if(prvCpssDrvPpObjConfig[devNum] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

#ifdef INCL_EXDXMX_DRIVER
    /* Dx, Ex, Mx driver init */
    rc = prvCpssDrvDxExMxInitObject(devNum);

    if(rc != GT_OK)
        return rc;
#endif /* INCL_EXDXMX_DRIVER */

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpCfgPhase1Init(devNum,
                    ppInInfoPtr,ppOutInfoPtr);

    return rc;
}


/*******************************************************************************
* prvCpssDrvHwPpPhase2Init
*
* DESCRIPTION:
*       the function set parameters for the driver for the "init Phase 2".
*       the function "renumber" the current device number of the device to a
*       new device number.
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum     - The PP's current device number .
*       newDevNum  - The PP's "new" device number to register.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*       GT_ALREADY_EXIST - the new device number is already used
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpPhase2Init
(
    IN GT_U8    devNum,
    IN GT_U8    newDevNum
)
{
    if(devNum == newDevNum)
    {
        /* nothing more to do */
        return GT_OK;
    }

    if(prvCpssDrvPpObjConfig[newDevNum] || prvCpssDrvPpConfig[newDevNum])
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    /* swap the function objects */
    prvCpssDrvPpObjConfig[newDevNum] = prvCpssDrvPpObjConfig[devNum];
    prvCpssDrvPpObjConfig[devNum] = NULL;

    /* swap the info DB */
    prvCpssDrvPpConfig[newDevNum] = prvCpssDrvPpConfig[devNum];
    prvCpssDrvPpConfig[devNum] = NULL;

    /* update the devNum */
    prvCpssDrvPpConfig[newDevNum]->hwDevNum = newDevNum;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvHwPpRamBurstConfigSet
*
* DESCRIPTION:
*       Sets the Ram burst information for a given device.
*
* INPUTS:
*       devNum          - The Pp's device number.
*       ramBurstInfo    - A list of Ram burst information for this device.
*       burstInfoLen    - Number of valid entries in ramBurstInfo.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       Narrow SRAM burst is not supported under Tiger devices.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpRamBurstConfigSet
(
    IN  GT_U8               devNum,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfoPtr,
    IN  GT_U8               burstInfoLen
)
{
    GT_STATUS   rc;

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpSetRamBurstConfig(devNum,
                    ramBurstInfoPtr,burstInfoLen);
    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpInitInMasks

*
* DESCRIPTION:
*      Set the interrupt mask for a given device.
*
*  APPLICABLE DEVICES: ExMx devices
*
* INPUTS:
*       devNum      - The Pp's device number.
*       b4StartInit - Is the call to this function is done before / after start
*                     Init.
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*      None.
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpInitInMasks
(
    IN  GT_U8   devNum,
    IN  GT_BOOL b4StartInit
)
{
    GT_STATUS   rc;
    GT_U32  portGroupId;/* port group Id */

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* Call appropriate implementation by virtual function */
        rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpInitInMasks(devNum,portGroupId,
                        b4StartInit);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvHwPpDevRemove
*
* DESCRIPTION:
*       Synchronize the driver/Config library after a hot removal operation, of
*       the given device.
*
* INPUTS:
*       devNum   - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpDevRemove
(
    IN GT_U8    devNum
)
{
    GT_STATUS           rc;
    GT_U32  ii;
    GT_U32  portGroupId;/* port group Id */

    if(prvCpssDrvPpConfig[devNum] == NULL)
    {
        /* Unknown device  */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Remove the device from the interrupts queues */
    rc = prvCpssDrvInterruptDeviceRemove(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        prvCpssDrvInterruptScanFree(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intScanRoot);

        if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem)
        {
            cpssOsMutexDelete(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].hwComplSem);
        }

        FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo);

        FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intNodesPool);
        FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->intCtrl.portGroupInfo[portGroupId].intMaskShadow);
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]->intCtrl.accessB4StartInit);

    /* clear the memory */
    FREE_PTR_MAC(prvCpssDrvPpConfig[devNum]);
    FREE_PTR_MAC(prvCpssDrvPpObjConfig[devNum]);

    /* check if there are still existing devices */
    for(ii = 0 ; ii < PRV_CPSS_MAX_PP_DEVICES_CNS ; ii++)
    {
        if(prvCpssDrvPpConfig[ii])
        {
            break;
        }
    }

    if(ii == PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        /* no more existing devices */


        /* release the management interface bus pointers */
        FREE_PTR_MAC(prvCpssDrvMngInfPciPtr);
        FREE_PTR_MAC(prvCpssDrvMngInfPexMbusPtr);
        FREE_PTR_MAC(prvCpssDrvMngInfPciHaStandbyPtr);
        FREE_PTR_MAC(prvCpssDrvMngInfSmiPtr);
        FREE_PTR_MAC(prvCpssDrvMngInfTwsiPtr);

        /* release the DB of interrupt ISR (lines) manager */
        rc = prvCpssDrvComIntSvcRtnDbRelease();
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set that we are done releaseing the DB */
        drvGlobalInfo.prvCpssDrvAfterDbRelease = GT_TRUE;
    }



    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvHwPpHaModeSet
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpHaModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_SYS_HA_MODE_ENT mode
)
{
    GT_STATUS   rc;

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpHaModeSet(devNum,mode);

    return rc;
}


/*******************************************************************************
* internal_cpssDrvInterruptsTreeGet
*
* DESCRIPTION:
*       function return :
*       1. the root to the interrupts tree info of the specific device
*       2. the interrupt registers that can't be accesses before 'Start Init'
*
* APPLICABLE DEVICES: All devices --> Packet Processors (not Fa/Xbar)
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       numOfElementsPtr - (pointer to) number of elements in the tree.
*       treeRootPtrPtr - (pointer to) pointer to root of the interrupts tree info.
*       numOfInterruptRegistersNotAccessibleBeforeStartInitPtr - (pointer to)
*                           number of interrupt registers that can't be accessed
*                           before 'Start init'
*       notAccessibleBeforeStartInitPtrPtr (pointer to)pointer to the interrupt
*                           registers that can't be accessed before 'Start init'
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDrvInterruptsTreeGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *numOfElementsPtr,
    OUT const CPSS_INTERRUPT_SCAN_STC        **treeRootPtrPtr,
    OUT GT_U32  *numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,
    OUT GT_U32  **notAccessibleBeforeStartInitPtrPtr
)
{
    GT_STATUS   rc;
    GT_U32  portGroupId = CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS;/*tempo internal parameter and not getting it from application*/

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);
    PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(devNum,portGroupId);

    CPSS_NULL_PTR_CHECK_MAC(numOfElementsPtr);
    CPSS_NULL_PTR_CHECK_MAC(treeRootPtrPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfInterruptRegistersNotAccessibleBeforeStartInitPtr);
    CPSS_NULL_PTR_CHECK_MAC(notAccessibleBeforeStartInitPtrPtr);

    /* Call appropriate implementation by virtual function */
    rc = prvCpssDrvPpObjConfig[devNum]->genPtr->drvHwPpInterruptsTreeGet(devNum,portGroupId,numOfElementsPtr,treeRootPtrPtr,
        numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,notAccessibleBeforeStartInitPtrPtr);

    return rc;
}

/*******************************************************************************
* cpssDrvInterruptsTreeGet
*
* DESCRIPTION:
*       function return :
*       1. the root to the interrupts tree info of the specific device
*       2. the interrupt registers that can't be accesses before 'Start Init'
*
* APPLICABLE DEVICES: All devices --> Packet Processors (not Fa/Xbar)
*
* INPUTS:
*       devNum - the device number
*
* OUTPUTS:
*       numOfElementsPtr - (pointer to) number of elements in the tree.
*       treeRootPtrPtr - (pointer to) pointer to root of the interrupts tree info.
*       numOfInterruptRegistersNotAccessibleBeforeStartInitPtr - (pointer to)
*                           number of interrupt registers that can't be accessed
*                           before 'Start init'
*       notAccessibleBeforeStartInitPtrPtr (pointer to)pointer to the interrupt
*                           registers that can't be accessed before 'Start init'
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   cpssDrvInterruptsTreeGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *numOfElementsPtr,
    OUT const CPSS_INTERRUPT_SCAN_STC        **treeRootPtrPtr,
    OUT GT_U32  *numOfInterruptRegistersNotAccessibleBeforeStartInitPtr,
    OUT GT_U32  **notAccessibleBeforeStartInitPtrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDrvInterruptsTreeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, numOfElementsPtr, treeRootPtrPtr, numOfInterruptRegistersNotAccessibleBeforeStartInitPtr, notAccessibleBeforeStartInitPtrPtr));

    rc = internal_cpssDrvInterruptsTreeGet(devNum, numOfElementsPtr, treeRootPtrPtr, numOfInterruptRegistersNotAccessibleBeforeStartInitPtr, notAccessibleBeforeStartInitPtrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, numOfElementsPtr, treeRootPtrPtr, numOfInterruptRegistersNotAccessibleBeforeStartInitPtr, notAccessibleBeforeStartInitPtrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssDrvHwPpRenumber
*
* DESCRIPTION:
*       the function set parameters for the driver to renumber it's DB.
*       the function "renumber" the current device number of the device to a
*       new device number.
*       NOTE:
*       this function MUST be called under 'Interrupts are locked'
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       oldDevNum  - The PP's "old" device number .
*       newDevNum  - The PP's "new" device number swap the DB to.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*       GT_ALREADY_EXIST - the new device number is already used
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvHwPpRenumber
(
    IN GT_U8    oldDevNum,
    IN GT_U8    newDevNum
)
{
    if(oldDevNum == newDevNum)
    {
        /* nothing more to do */
        return GT_OK;
    }

    if(prvCpssDrvPpObjConfig[newDevNum] || prvCpssDrvPpConfig[newDevNum])
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    /* swap the function objects */
    prvCpssDrvPpObjConfig[newDevNum] = prvCpssDrvPpObjConfig[oldDevNum];
    prvCpssDrvPpObjConfig[oldDevNum] = NULL;

    /* swap the info DB */
    prvCpssDrvPpConfig[newDevNum] = prvCpssDrvPpConfig[oldDevNum];
    prvCpssDrvPpConfig[oldDevNum] = NULL;

    /* swap debug Device ID DB */
    prvCpssDrvDebugDeviceId[newDevNum] = prvCpssDrvDebugDeviceId[oldDevNum];
    prvCpssDrvDebugDeviceId[oldDevNum] = GT_FALSE;

    /* swap tracing hw write DB */
    prvCpssDrvTraceHwWrite[newDevNum] = prvCpssDrvTraceHwWrite[oldDevNum];
    prvCpssDrvTraceHwWrite[oldDevNum] = GT_FALSE;

    /* swap tracing hw read DB */
    prvCpssDrvTraceHwRead[newDevNum] = prvCpssDrvTraceHwRead[oldDevNum];
    prvCpssDrvTraceHwRead[oldDevNum] = GT_FALSE;

       /* swap tracing hw delay DB */
    prvCpssDrvTraceHwDelay[newDevNum] = prvCpssDrvTraceHwDelay[oldDevNum];
    prvCpssDrvTraceHwDelay[oldDevNum] = GT_FALSE;

    /* update the devNum */
    prvCpssDrvPpConfig[newDevNum]->hwDevNum = newDevNum;

    /*call the interrupts to renumber */
    return prvCpssDrvInterruptsRenumber(oldDevNum,newDevNum);
}


/*******************************************************************************
* prvCpssDrvDebugDeviceIdSet
*
* DESCRIPTION:
*       This is debug function.
*       The function overrides device ID by given value.
*       The function should be called before cpssDxChHwPpPhase1Init().
*
*  APPLICABLE DEVICES: All devices
*
* INPUTS:
*       devNum  - device number .
*       devType - device type to store.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL     - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvDebugDeviceIdSet
(
    IN GT_U8                    devNum,
    IN CPSS_PP_DEVICE_TYPE      devType
)
{
    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    prvCpssDrvDebugDeviceId[devNum] = (GT_U16)(devType >> 16);

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvAddrCheckWaBind
*
* DESCRIPTION:
*       Binds errata db with address check callback
*
* APPLICABLE DEVICES: All devices
*
* INPUTS:
*       devNum           - the device number
*       addrCheckFuncPtr - pointer to callback function that checks addresses.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_INITIALIZED       - the driver was not initialized for the device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssDrvAddrCheckWaBind
(
    IN  GT_U8                                   devNum,
    IN  PRV_CPSS_DRV_ERRATA_ADDR_CHECK_FUNC     addrCheckFuncPtr
)
{
    PRV_CPSS_DRV_CHECK_GEN_FUNC_OBJ_MAC(devNum);

    prvCpssDrvPpConfig[devNum]->errata.addrCheckFuncPtr = addrCheckFuncPtr;

    return GT_OK;
}


/*******************************************************************************
* Function: prvCpssDrvPortsFullMaskGet
*
* DESCRIPTION:
*       get bmp of all phy ports of the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       numOfPorts - number of physical ports in the device
*
* OUTPUTS:
*       portsBmpPtr - (pointer to) bmp of ports
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvCpssDrvPortsFullMaskGet
(
    IN GT_U32                   numOfPorts,
    OUT CPSS_PORTS_BMP_STC       *portsBmpPtr
)
{
    GT_U32  ii;

    if(numOfPorts)
    {
        for(ii = 0; ii < CPSS_MAX_PORTS_BMP_NUM_CNS; ii++)
        {
            if((GT_U32)(numOfPorts - 1) < (GT_U32)((ii+1)*32))
            {
                if(numOfPorts & 0x1f)
                {
                    (portsBmpPtr)->ports[ii] =
                        BIT_MASK_MAC((numOfPorts & 0x1f));
                }
                else
                {
                    (portsBmpPtr)->ports[ii] = 0xFFFFFFFF;
                }

                ii++;
                break;
            }

            (portsBmpPtr)->ports[ii] = 0xFFFFFFFF;
        }
    }
    else /* numOfPorts = 0 ... should not happen ...but lets implement */
    {
        ii = 0;/* to allow to reset the full bmp */
    }

    for(/* continue _ii */; ii < CPSS_MAX_PORTS_BMP_NUM_CNS; ii++)
    {
        (portsBmpPtr)->ports[ii] = 0;
    }
}

/*******************************************************************************
* Function: prvCpssDrvPortsBmpMaskWithMaxPorts
*
* DESCRIPTION:
*       mask bmp ports with the max number of ports of the of the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       portsBmpPtr - (pointer to) bmp of ports
*
* OUTPUTS:
*       portsBmpPtr - (pointer to) bmp of ports , after the mask
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvCpssDrvPortsBmpMaskWithMaxPorts
(
    IN GT_U8                        devNum,
    INOUT CPSS_PORTS_BMP_STC       *portsBmpPtr
)
{
    CPSS_PORTS_BMP_STC tmpPortsBmp;

    prvCpssDrvPortsFullMaskGet(PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts,&tmpPortsBmp);

    CPSS_PORTS_BMP_BITWISE_AND_MAC(portsBmpPtr,portsBmpPtr,&tmpPortsBmp);

    return;
}

/*******************************************************************************
* prvCpssDrvPpDump
*
* DESCRIPTION:
*       Dump function , to print the info on a specific PP -- CPSS driver.
*
*
*  APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum - device Number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - on bad device number
*
* COMMENTS:
*       function also called from the "cpss pp dump" : cpssPpDump(devNum)
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpDump
(
    IN GT_U8   devNum
)
{
    PRV_CPSS_DRIVER_PP_CONFIG_STC   *drvPpInfoPtr;

    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS ||
       prvCpssDrvPpConfig[devNum] == NULL)
    {
        DEBUG_PRINT(("bad device number [%d] \n",devNum));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    drvPpInfoPtr = prvCpssDrvPpConfig[devNum];

    DEBUG_PRINT(("\n"));
    DEBUG_PRINT(("start CPSS DRIVER Info about device number [%d]: \n",devNum));

    DEBUG_PRINT((" devNum = [%d] \n",drvPpInfoPtr->hwDevNum));

    DEBUG_PRINT((" devType = [0x%8.8x] \n",drvPpInfoPtr->devType));
    DEBUG_PRINT((" devFamily = [%s] \n",
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTC_E    ? " CPSS_PP_FAMILY_TWISTC_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E    ? " CPSS_PP_FAMILY_TWISTD_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_SAMBA_E     ? " CPSS_PP_FAMILY_SAMBA_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_SALSA_E     ? " CPSS_PP_FAMILY_SALSA_E ":

        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E   ? " CPSS_PP_FAMILY_CHEETAH_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E  ? " CPSS_PP_FAMILY_CHEETAH2_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E  ? " CPSS_PP_FAMILY_CHEETAH3_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E  ? " CPSS_PP_FAMILY_DXCH_XCAT_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E  ? " CPSS_PP_FAMILY_DXCH_XCAT3_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E  ? " CPSS_PP_FAMILY_DXCH_LION_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E  ? " CPSS_PP_FAMILY_DXCH_LION2_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E  ? " CPSS_PP_FAMILY_DXCH_BOBCAT2_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E  ? " CPSS_PP_FAMILY_DXCH_BOBCAT3_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E  ? " CPSS_PP_FAMILY_DXCH_ALDRIN_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_TIGER_E     ? " CPSS_PP_FAMILY_TIGER_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_PUMA_E      ? " CPSS_PP_FAMILY_PUMA_E ":
        drvPpInfoPtr->devFamily == CPSS_PP_FAMILY_PUMA3_E     ? " CPSS_PP_FAMILY_PUMA3_E ":
        " unknown "
    ));
    DEBUG_PRINT((" numOfPorts = [%d] \n",drvPpInfoPtr->numOfPorts));

    if(drvPpInfoPtr->portGroupsInfo.isMultiPortGroupDevice == GT_TRUE)
    {
        DEBUG_PRINT(("numOfPortGroups  = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.numOfPortGroups));
        DEBUG_PRINT(("activePortGroupsBmp   = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.activePortGroupsBmp));
        DEBUG_PRINT(("firstActivePortGroup  = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.firstActivePortGroup));
        DEBUG_PRINT(("lastActivePortGroup   = [%d] \n",
                     drvPpInfoPtr->portGroupsInfo.lastActivePortGroup));
    }
    else
    {
        if(drvPpInfoPtr->portGroupsInfo.numOfPortGroups != 1 ||
           drvPpInfoPtr->portGroupsInfo.activePortGroupsBmp != BIT_0 ||
           drvPpInfoPtr->portGroupsInfo.firstActivePortGroup != 0 ||
           drvPpInfoPtr->portGroupsInfo.lastActivePortGroup != 0)
        {
            DEBUG_PRINT(("Error : non 'multi port grops' device with bad configuration : \n"));
            DEBUG_PRINT(("numOfPortGroups --  = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.numOfPortGroups));
            DEBUG_PRINT(("activePortGroupsBmp --   = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.activePortGroupsBmp));
            DEBUG_PRINT(("firstActivePortGroup --  = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.firstActivePortGroup));
            DEBUG_PRINT(("lastActivePortGroup --   = [%d] \n",
                         drvPpInfoPtr->portGroupsInfo.lastActivePortGroup));
        }
    }

    DEBUG_PRINT((" drvChannel = [%s] \n",
        PRV_CPSS_DRV_HW_IF_PEX_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_PEX_E " :
        PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum)  ? " CPSS_CHANNEL_PCI_E " :
        PRV_CPSS_DRV_HW_IF_SMI_COMPATIBLE_MAC(devNum)  ? " CPSS_CHANNEL_SMI_E " :
        PRV_CPSS_DRV_HW_IF_TWSI_COMPATIBLE_MAC(devNum) ? " CPSS_CHANNEL_TWSI_E " :
        " unknown "
    ));

    DEBUG_PRINT(("finished CPSS DRIVER Info about device number [%d]: \n",devNum));
    DEBUG_PRINT(("\n"));

    return GT_OK;
}

