/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenDragoniteInterrupts.c
*
* DESCRIPTION:
*       CPSS generic DRAGONITE APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/dragonite/cpssGenDragonite.h>
#include <cpss/generic/dragonite/private/prvCpssGenDragonite.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>

#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>
#include <cpss/driver/interrupts/cpssDrvComIntSvcRtn.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>

/*****************************************************************************
*                         DEFINITIONS
******************************************************************************/
GT_U32 *irqMemPtr = NULL;

static PRV_CPSS_DRAGONITE_INTERRUPT_CTRL_STC dragoniteInterruptsCtrlStruct;
static GT_BOOL dragoniteInterruptsInitDone = GT_FALSE;

static GT_STATUS dragoniteInterruptsHwReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *data
);

static GT_STATUS dragoniteInterruptsHwWriteRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   data
);

static PRV_CPSS_DRV_INTERRUPT_SCAN_STC dragoniteIntrScanArray[] =
{
    {0,                                 /*      bitNum          - Sum bit num in upper hierarchy, representing this      */
                                        /*                        register.                                               */
    GT_FALSE,                           /*      isGpp           - Is hierarchy connected to another device.               */
    0,                                  /*      gppId           - The GPP Id represented by this node (Valid only if     */
                                        /*                        isGpp is GT_TRUE).                                     */
    NULL,                               /*      gppFuncPtr      - Pointer to isrFunc to be called if isGPP == GT_TRUE.   */
    0xdeadbeef,                         /*      causeRegAddr    - Address of the interrupt cause register to scan.       */
    0xdeadbeef,                         /*      maskRegAddr     - Address of the interrupt mask register to update after */
                                        /*                        scanning.                                              */
    dragoniteInterruptsHwReadRegister,        /*      pRegReadFunc    - A pointer to a function to be used for reading the     */
                                        /*                        interrupt cause register.                              */
    dragoniteInterruptsHwWriteRegister,       /*      pRegWriteFunc   - A pointer to a function to be used for writing the     */
                                        /*                        interrupt mask register.                               */
    PRV_CPSS_GEN_DRAGONITE_IRQ_MIN_E,         /*      startIdx        - The start interrupt index representing the interrupts  */
                                        /*                        to be scanned.                                         */
    PRV_CPSS_GEN_DRAGONITE_IRQ_MAX_E,         /*      endIdx          - The end interrupt index representing the interrupts to */
                                        /*                        be scanned.                                            */
    /* bit 31 - interrupt memory owner bit */
    0x7FFFFFFF,                         /*      nonSumBitMask   - bit mask of non sum bits in the register                */
    /* clear interrupts cause manually on read */
    0x7FFFFFFF,                         /*      rwBitMask       - bit mask of R/W bits that should be cleared by write.   */
    0x7FFFFFFF,                         /*      maskRcvIntrEn   - bit mask of interrupts to be masked after receive.      */
    0,                                  /*      subIntrListLen  - List length sub interrupts list.                        */
    NULL,                               /*      subIntrScan     - pointer to sum bit interrupt register structure.        */
    NULL}                               /*      nextIntrScan    - pointer to next interrupt struct in hierarchy.          */
};

/*
 * Typedef: enum UNI_EV_2_EV_OFFSET
 *
 * Enumeration: Lists the conversion table column data types.
 *
 * Enums:
 *      UNI_EV              - Unified event
 *      DRAGONITE_EV              - DRAGONITE event
 *      TABLE_ENTRY_SIZE    - Table size (MUST be last !!)
 *
 */
typedef enum
{
    UNI_EV,
    DRAGONITE_EV,
    TABLE_ENTRY_SIZE
} UNI_EV_2_EV_OFFSET;

/* the conversion table */
static GT_U32 dragoniteUni2IntCauseType[][TABLE_ENTRY_SIZE] = {
    { CPSS_DRAGONITE_PORT_ON_E      , PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_ON_E          },
    { CPSS_DRAGONITE_PORT_OFF_E     , PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_OFF_E         },
    { CPSS_DRAGONITE_DETECT_FAIL_E  , PRV_CPSS_GEN_DRAGONITE_IRQ_DETECT_FAIL_E      },
    { CPSS_DRAGONITE_PORT_FAULT_E       , PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_FAULT_E   },
    { CPSS_DRAGONITE_PORT_UDL_E     , PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_UDL_E         },
    { CPSS_DRAGONITE_PORT_FAULT_ON_STARTUP_E, PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_FAULT_ON_STARTUP_E    },
    { CPSS_DRAGONITE_PORT_PM_E      , PRV_CPSS_GEN_DRAGONITE_IRQ_PORT_PM_E          },
    { CPSS_DRAGONITE_POWER_DENIED_E , PRV_CPSS_GEN_DRAGONITE_IRQ_POWER_DENIED_E     },
    { CPSS_DRAGONITE_OVER_TEMP_E        , PRV_CPSS_GEN_DRAGONITE_IRQ_OVER_TEMP_E    },
    { CPSS_DRAGONITE_TEMP_ALARM_E       , PRV_CPSS_GEN_DRAGONITE_IRQ_TEMP_ALARM_E   },
    { CPSS_DRAGONITE_DEVICE_FAULT_E , PRV_CPSS_GEN_DRAGONITE_IRQ_DEVICE_FAULT_E     },
    { CPSS_DRAGONITE_OVER_CONSUM_E  , PRV_CPSS_GEN_DRAGONITE_IRQ_OVER_CONSUM_E      },
    { CPSS_DRAGONITE_VMAIN_LOW_AF_E , PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_LOW_AF_E     },
    { CPSS_DRAGONITE_VMAIN_LOW_AT_E , PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_LOW_AT_E     },
    { CPSS_DRAGONITE_VMAIN_HIGH_E       , PRV_CPSS_GEN_DRAGONITE_IRQ_VMAIN_HIGH_E   },
    { CPSS_DRAGONITE_READ_EVENT_E       , PRV_CPSS_GEN_DRAGONITE_IRQ_READ_EVENT_E   },
    /* use here reserved interrupt to simulate error interrupt to application */
    { CPSS_DRAGONITE_ERROR_E          , PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED0_E   },
    { CPSS_DRAGONITE_WRITE_EVENT_E  , PRV_CPSS_GEN_DRAGONITE_IRQ_WRITE_EVENT_E      }
};

#define NUM_OF_TBL_ENTRIES \
    (sizeof(dragoniteUni2IntCauseType)/(sizeof(GT_U32)*TABLE_ENTRY_SIZE))

/*****************************************************************************
* dragoniteInterruptsHwReadRegister
*
* DESCRIPTION:
*       Function performing hardware read of Dragonite interrupt cause register.
*       Just this one register interesting to read for Dragonite interrupt 
*       management
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; ExMxPm; ExMx.
*
* INPUTS:
*       devNum  - The device number to read. - not relevant for DRAGONITE
*       portGroupId - The port group Id. support the 'multi-port-groups' device.
*                for PP - for non 'multi-port-group' devices use PRV_CPSS_DRV_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS
*                for FA,Xbar,DRAGONITE - not relevant
*       regAddr - The register's address to read from - ignored, because actually used to read just cause register
*                   for DRAGONITE - not relevant
* OUTPUTS:
 *       dataPtr    - Data word pointer for read data.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dragoniteInterruptsHwReadRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    OUT GT_U32  *dataPtr
)
{
    /* to avoid warning */
    devNum = devNum;
    portGroupId = portGroupId;
    regAddr = regAddr;

    CPSS_NULL_PTR_CHECK_MAC(dataPtr);

    /* no need to check IMO bit, because it's checked on start of interrupt scan routine */
    *dataPtr = PRV_CPSS_DRAGONITE_IRQ_CAUSE_BITS_GET_MAC(irqMemPtr); /* take cause bits without IMO */

    PRV_CPSS_DRAGONITE_IRQ_CAUSE_BITS_SET_MAC(irqMemPtr, 0); /* clear interrupts cause register */

    return GT_OK;
}

/*****************************************************************************
* dragoniteInterruptsHwWriteRegister
*
* DESCRIPTION:
*       Function performing hardware write of Dragonite interrupt cause or 
*       mask registers
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; ExMxPm; ExMx.
*
* INPUTS:
*       devNum  - The device number to write - ignored for DRAGONITE
*       portGroupId - The port group Id. support the 'multi-port-groups' device.
*                for PP - for non 'multi-port-group' devices use PRV_CPSS_DRV_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS
*                for FA,Xbar,DRAGONITE - not relevant
*       regAddr - The register's address to write to - may be or cause or mask register
*       data    - Data word to write.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS dragoniteInterruptsHwWriteRegister
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   data
)
{
    GT_U32 *regPtr;
    GT_U32 tmpData;

    /* to avoid warning */
    devNum = devNum;
    portGroupId = portGroupId;

    /* no need to check IMO bit, because it's checked on start of interrupt scan routine */

    regPtr = (GT_U32*)((GT_UINTPTR)(regAddr));

    /* for both cause and mask register bit 31 not relevant:
     * in cause register it's IMO_flag and in mask register
     * it's IRQ_polarity flag
     */
    tmpData = PRV_CPSS_GEN_DRAGONITE_U32_GET_MAC(*regPtr);
    U32_SET_FIELD_MAC(tmpData, 0, 31, data);
    PRV_CPSS_GEN_DRAGONITE_U32_SET_MAC(*regPtr, tmpData);

    return GT_OK;
}

/*****************************************************************************
* dragoniteDrvReqDrvnMaskSet
*
* DESCRIPTION:
*       This function masks/unmasks a given interrupt bit in the relevant
*       interrupt mask register.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; ExMxPm; ExMx.
*
* INPUTS:
*       evNode  - The interrupt node representing the interrupt to be
*                 unmasked.
*       operation - The operation to perform, mask or unmask the interrupt
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*       Used in Event Request Driven mode (polling).
*
*******************************************************************************/
static GT_STATUS dragoniteDrvReqDrvnMaskSet
(
    IN struct PRV_CPSS_DRV_EV_REQ_NODE_STCT    *evNode,
    IN CPSS_EVENT_MASK_SET_ENT   operation
)
{
    GT_U32 maskIdx;
    GT_U32 *intMaskShadow;

    intMaskShadow = dragoniteInterruptsCtrlStruct.intMaskShadow;

    maskIdx = evNode->intCause >> 5;

    if (CPSS_EVENT_MASK_E == operation)
    {
        /* mask the interrupt */
        intMaskShadow[maskIdx] &= ~(evNode->intBitMask);
    }
    else
    {
        /* unmask the interrupt */
        intMaskShadow[maskIdx] |= (evNode->intBitMask);
    }

    PRV_CPSS_DRAGONITE_IRQ_MASK_SET_MAC(irqMemPtr, intMaskShadow[maskIdx]);

    return GT_OK;
}

/*******************************************************************************
* interruptDragoniteSR
*
* DESCRIPTION:
*       This is the DRAGONITE controller interrupt service routine, it scans the
*       interrupts and enqueues each of them to the interrupt queues structure.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; ExMxPm; ExMx.
*
* INPUTS:
*       cookiePtr  - pointer to cookie.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       1 - if interrupts where received from the given device, or
*       0 - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U8 interruptDragoniteSR
(
    IN void *cookiePtr
)
{
    GT_U8 retVal = GT_OK;

    /* to avoid warning */
    cookiePtr = cookiePtr;

    if(0 == PRV_CPSS_DRAGONITE_IRQ_IMO_FLAG_GET_MAC(irqMemPtr))
    {
        retVal = prvCpssDrvIntEvReqDrvnScan(0 , 0, /* devnum and portGroupId - don't care */
                                            dragoniteInterruptsCtrlStruct.intNodesPool,
                                            dragoniteInterruptsCtrlStruct.intMaskShadow,
                                            dragoniteInterruptsCtrlStruct.intScanRoot);

        /* pass memory owner ship to DRAGONITE controller - enable interrupts to host */
        PRV_CPSS_DRAGONITE_IRQ_IMO_FLAG_SET_MAC(irqMemPtr, 1);
    }
    else
    {
        if(dragoniteInterruptsCtrlStruct.intNodesPool[PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED0_E].intCbFuncPtr != NULL)
        {
            dragoniteInterruptsCtrlStruct.intNodesPool[PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED0_E].intCbFuncPtr(0,
                                                PRV_CPSS_GEN_DRAGONITE_IRQ_SYSTEM_RESERVED0_E);
        }
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_READY, LOG_ERROR_NO_MSG);
    }

    /* return 0 to stop interrupt service loop */
    return retVal;
}

/*******************************************************************************
* dragoniteIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; ExMxPm; ExMx.
*
* INPUTS:
*       intCause - The interrupt cause to convert.
*
* OUTPUTS:
*       uniEvPtr    - The unified event type.
*       extDataPtr  - The event extended data.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND otherwise.
*
* COMMENTS:
*       Usually return status of this function not checked
*
*******************************************************************************/
static GT_STATUS dragoniteIntCauseToUniEvConvert
(
    IN  GT_U32      intCause,
    OUT GT_U32      *uniEvPtr,
    OUT GT_U32      *extDataPtr
)
{
    GT_U32  i;

    if (NULL == uniEvPtr || NULL == extDataPtr)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    *uniEvPtr   = CPSS_UNI_RSRVD_EVENT_E;
    *extDataPtr = 0;

    for (i = 0; i < NUM_OF_TBL_ENTRIES; i++)
    {
        if (dragoniteUni2IntCauseType[i][DRAGONITE_EV] == intCause)
        {
            *uniEvPtr   = dragoniteUni2IntCauseType[i][UNI_EV];
            return GT_OK;
        }
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* dragoniteInterruptsRegsInit
*
* DESCRIPTION:
*       Initialize IRQ related registers in DTCM
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; ExMxPm; ExMx.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID dragoniteInterruptsRegsInit
(
    GT_VOID
)
{
    /* mask/disable all DRAGONITE interrupts */
    PRV_CPSS_DRAGONITE_IRQ_MASK_SET_MAC(irqMemPtr, 0);

    /* pass memory owner ship to DRAGONITE controller - enable Dragonite to 
     * update cause register and read mask register
     */
    PRV_CPSS_DRAGONITE_IRQ_IMO_FLAG_SET_MAC(irqMemPtr, 1);

    return;
}

/*******************************************************************************
* drvEventMaskDevice
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on Dragonite
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; ExMxPm; ExMx.
*
* INPUTS:
*       devNum - not in use
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
    /* to avoid warning */
    devNum = devNum;

    return prvCpssDrvEvReqUniEvMaskSet(dragoniteInterruptsCtrlStruct.intNodesPool,
                                        PRV_CPSS_GEN_DRAGONITE_IRQ_MAX_E+1,
                                        (GT_U32)uniEvent, evExtData ,operation);
}

/*******************************************************************************
* dragoniteInterruptsInit
*
* DESCRIPTION:
*       Initialize the DRAGONITE interrupts mechanism
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* INPUTS:
*    intVectNum - DRAGONITE interrupt vector number
*    intMask    - DRAGONITE interrupt mask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS dragoniteInterruptsInit
(
    IN GT_U32 intVectNum,
    IN GT_U32 intMask
)
{
    GT_U32          scanArrIdx;
    GT_U32          i;
    GT_STATUS       retVal;
    PRV_CPSS_DRV_EV_REQ_NODE_STC *dragoniteIntNodesPool;

    if(dragoniteInterruptsInitDone)
    {/* just reinit Dragonites IRQ registers, CPSS DRAGONITE interrupt initialized already */
        dragoniteInterruptsRegsInit();
        return GT_OK;
    }

    dragoniteIntrScanArray[0].causeRegAddr = (GT_U32)(GT_UINTPTR)&((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqCause;
    dragoniteIntrScanArray[0].maskRegAddr  = (GT_U32)(GT_UINTPTR)&((PRV_CPSS_DRAGONITE_IRQ_MEMORY_STC*)irqMemPtr)->irqMask;

    scanArrIdx = 0;
    retVal = prvCpssDrvInterruptScanInit(sizeof(dragoniteIntrScanArray)/sizeof(PRV_CPSS_DRV_INTERRUPT_SCAN_STC), /* intrScanArrayLen */
                                        dragoniteIntrScanArray,
                                        &scanArrIdx,
                                        &dragoniteInterruptsCtrlStruct.intScanRoot);
    if(retVal != GT_OK)
    {
        return retVal;
    }

    dragoniteIntNodesPool = cpssOsMalloc(sizeof(PRV_CPSS_DRV_EV_REQ_NODE_STC)*(PRV_CPSS_GEN_DRAGONITE_IRQ_MAX_E+1));
    for (i = 0; i <= PRV_CPSS_GEN_DRAGONITE_IRQ_MAX_E; i++)
    {
        dragoniteIntNodesPool[i].devNum = 0;
        dragoniteIntNodesPool[i].portGroupId = 0;
        dragoniteIntNodesPool[i].intStatus = PRV_CPSS_DRV_EV_DRVN_INT_ACTIVE_E;
        dragoniteIntNodesPool[i].intRecStatus = PRV_CPSS_DRV_EV_DRVN_INT_IDLE_E;
    #ifdef CPSS_PRODUCT
        dragoniteIntNodesPool[i].intType = 0; /* dummy value - this field is only
                                         * for compatibility with PSS
                                         */
    #endif
        dragoniteIntNodesPool[i].intCause = i;
        /* return value of IntCauseToUniEvConvert not interesting here */
        dragoniteIntCauseToUniEvConvert(i, &dragoniteIntNodesPool[i].uniEvCause,
                                        &dragoniteIntNodesPool[i].uniEvExt);
        dragoniteIntNodesPool[i].intMaskSetFptr = dragoniteDrvReqDrvnMaskSet;
        dragoniteIntNodesPool[i].intCbFuncPtr = NULL;
        dragoniteIntNodesPool[i].intMaskReg = dragoniteIntrScanArray[0].maskRegAddr;
        dragoniteIntNodesPool[i].intBitMask = 1 << (i & 0x1F);
        dragoniteIntNodesPool[i].prevPtr = NULL;
        dragoniteIntNodesPool[i].nextPtr = NULL;
    }
    dragoniteInterruptsCtrlStruct.intNodesPool = dragoniteIntNodesPool;
    dragoniteInterruptsCtrlStruct.intVecNum = intVectNum;
    dragoniteInterruptsCtrlStruct.intMask = intMask;
    dragoniteInterruptsCtrlStruct.isrFuncPtr = interruptDragoniteSR;
    dragoniteInterruptsCtrlStruct.intMaskShadow = cpssOsMalloc(sizeof(GT_U32));
    dragoniteInterruptsCtrlStruct.intMaskShadow[0] = 0x7fffffff;

    /* bind a callback function for the mask/unmask of unify-events for Dragonite */
    retVal = prvCpssGenEventMaskDeviceBind(PRV_CPSS_DEVICE_TYPE_DRAGONITE_E,&drvEventMaskDevice);
    if(retVal != GT_OK)
    {
        return retVal;
    }

    /* connect the interrupts    */
    retVal = cpssGenEventInterruptConnect(dragoniteInterruptsCtrlStruct.intVecNum,
                                          (GT_U32)dragoniteInterruptsCtrlStruct.intMask,
                                          dragoniteInterruptsCtrlStruct.isrFuncPtr,
                                          NULL,
                                          (GT_UINTPTR *)(&dragoniteInterruptsCtrlStruct.isrConnectionId));

    dragoniteInterruptsRegsInit();

    dragoniteInterruptsInitDone = (retVal == GT_OK) ? GT_TRUE : GT_FALSE;

    return retVal;
}


