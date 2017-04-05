/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvComIntSvcRtn.h
*
* DESCRIPTION:
*       Includes generic interrupt service routine functions declarations.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#ifndef __cpssDrvComIntSvcRtnh
#define __cpssDrvComIntSvcRtnh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/events/cpssGenEventCtrl.h>
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/driver/interrupts/cpssDrvComIntEvReqQueues.h>

/* place holder in scan tree for PCI/PEX cause register
    (used only during initialization - filled with actual value during
     prvCpssDrvExMxDxHwPpMaskRegInfoGet(...))*/
#define     PRV_CPSS_DRV_SCAN_PEX_CAUSE_REG_PLACE_HOLDER_CNS   0xAABBAABA
/* place holder in scan tree for PCI/PEX mask register
    (used only during initialization - filled with actual value during
     prvCpssDrvExMxDxHwPpMaskRegInfoGet(...))*/
#define     PRV_CPSS_DRV_SCAN_PEX_MASK_REG_PLACE_HOLDER_CNS    0xAABBAABB

/* The following define the PRV_CPSS_DRV_INTERRUPT_SCAN_STC structure stack
   size needed to   */
/* traverse the interrupt cause tree with no usage of recursion. If the     */
/* init phase fails because of small stack, feel free to enlarge it.        */
#define PRV_CPSS_DRV_SCAN_STACK_SIZE_CNS          40
#define PRV_CPSS_DRV_SCAN_STACK_LAST_ENTRY_CNS    (PRV_CPSS_DRV_SCAN_STACK_SIZE_CNS - 1)

/* flag that state the DB of cpss common was released and now we are after that stage
    NOTE: the flag start with GT_FALSE , but once changed to GT_TRUE it will stay
          that way forever.
*/
extern GT_BOOL  prvCpssCommonAfterDbRelease;

/*******************************************************************************
* PRV_CPSS_DRV_COM_INT_TX_ENDED_CB
*
* DESCRIPTION:
*       a call back function to be called when the "Tx completed" event is
*       received
*
* INPUTS:
*       devNum - the device that got the "tx ended" event
*       queue  - the queue that the packet was send from
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_DRV_COM_INT_TX_ENDED_CB)
(
    IN  GT_U8   devNum,
    IN  GT_U8   queue
);

/* a new name to this structure to be used by internal cpss implementation */
typedef CPSS_INTERRUPT_SCAN_STC                PRV_CPSS_DRV_INTERRUPT_SCAN_STC;
typedef CPSS_INTERRUPTS_HW_WRITE_REGISTER_FUNC PRV_CPSS_DRV_HW_WRITE_REGISTER_FUNC;
typedef CPSS_INTERRUPTS_HW_READ_REGISTER_FUNC  PRV_CPSS_DRV_HW_READ_REGISTER_FUNC;
typedef CPSS_INTERRUPTS_GPP_ISR_FUNC           PRV_CPSS_DRV_GPP_ISR_FUNC;


/*
 * Typedef: struct PRV_CPSS_DRV_ISR_PARAMS_STC
 *
 * Description: Holds interrupt service routine parameters to be used when an
 *              interrupt is received.
 *
 * Fields:
 *      isrFuncPtr  - Isr function pointer to be called on interrupt reception.
 *      cookie      - A user define parameter to be passed to the user on
 *                    interrupt service routine invocation.
 *
 */
typedef struct PRV_CPSS_DRV_ISR_PARAMS_STCT
{
    CPSS_EVENT_ISR_FUNC isrFuncPtr;
    void                *cookie;
    struct PRV_CPSS_DRV_ISR_PARAMS_STCT   *next;
}PRV_CPSS_DRV_ISR_PARAMS_STC;


/*
 * Typedef: struct PRV_CPSS_DRV_GPP_INT_INFO_STC
 *
 * Description: Holds Gpp interrupt binding information.
 *
 * Fields:
 *      gppIsrPtr   - The function pointer to be invoked in Gpp interrupt
 *                    reception.
 *      cookie      - A user defined parameter to be passed to the isr, in
 *                    addition to the gppId parameter.
 *
 */
typedef struct
{
    CPSS_EVENT_ISR_FUNC    gppIsrPtr;
    void            *cookie;
}PRV_CPSS_DRV_GPP_INT_INFO_STC;


/*
 * Typedef: struct PRV_CPSS_DRV_INT_VEC_MEM_LIST_STC
 *
 * Description: interrupt vector members list, points to a list of devices
 *              connected to the specified interrupt vector.
 *
 * Fields:
 *      intVecNum - The interrupt vector number representing this list.
 *      devListHead - A pointer to the head of the linked list holding the
 *                    device number.
 *
 */
typedef struct
{
    GT_U32 intVecNum;
    PRV_CPSS_DRV_ISR_PARAMS_STC *first;
    PRV_CPSS_DRV_ISR_PARAMS_STC *isrParamsArray[PRV_CPSS_MAX_PP_DEVICES_CNS];
}PRV_CPSS_DRV_INT_VEC_MEM_LIST_STC;


/*******************************************************************************
* prvCpssDrvInterruptEnable
*
* DESCRIPTION:
*       Enable  interrupts in accordance with given interrupt mask.
*
* INPUTS:
*       intMask     - The interrupt mask to enable/disable interrupts on
*                     this device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptEnable
(
    IN  GT_U32          intMask
);


/*******************************************************************************
* prvCpssDrvInterruptConnect
*
* DESCRIPTION:
*       Connect a given function to the given interrupt vector (mask).
*
* INPUTS:
*       intVecNum   - The interrupt vector number this device is connected to.
*       intMask     - The interrupt mask to enable/disable interrupts on
*                     this device.
*       intRoutine  - A pointer to the interrupt routine to be connected to the
*                     given interrupt line.
*       cookie      - A user defined cookie to be passed to the isr on interrupt
*                     reception.
*       connectionId- Id to be used for future access to the connected isr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptConnect
(
    IN  GT_U32          intVecNum,
    IN  GT_U32          intMask,
    IN  CPSS_EVENT_ISR_FUNC    intRoutine,
    IN  void            *cookie,
    OUT GT_UINTPTR      *connectionId
);


/*******************************************************************************
* prvCpssDrvInterruptDisconnect
*
* DESCRIPTION:
*       Disconnect a device from a given interrupt vector.
*
* INPUTS:
*       intVecNum   - The interrupt vector number the device is connected to.
*       connectioId - The connection Id returned on the interrupt connect
*                     operation.
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
GT_STATUS prvCpssDrvInterruptDisconnect
(
    IN  GT_U32      intVecNum,
    IN  GT_UINTPTR  connectionId
);


/*******************************************************************************
* prvCpssDrvInterruptScanInit
*
* DESCRIPTION:
*       This function initializes the Devices interrupt data structures for
*       isr interrupt scan operations.
*
* INPUTS:
*       intrScanArrayLen- intrScanArray Length.
*       intrScanArray   - Array of hierarchy tree of interrupt scan struct to
*                         initialize.
*       scanArrayIdx    - start index in scan array to use.
*       subIntrScan     - Pointer to be assigned with scan tree.
*
* OUTPUTS:
*       scanArrayIdx    - New start index in scan array to use.
*       subIntrScan     - Pointer to scan tree created.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptScanInit
(
    IN      GT_U32          intrScanArrayLen,
    IN      const PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *intrScanArray,
    INOUT   GT_U32          *scanArrayIdx,
    INOUT   PRV_CPSS_DRV_INTERRUPT_SCAN_STC  **subIntrScan
);


/*******************************************************************************
* prvCpssDrvInterruptScanFree
*
* DESCRIPTION:
*       This function Frees the Device's interrupt data structures
*
* INPUTS:
*       subIntrScan     - Pointer to be assigned with scan tree.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prvCpssDrvInterruptScanFree
(
    IN   PRV_CPSS_DRV_INTERRUPT_SCAN_STC  *subIntrScan
);



/*******************************************************************************
* prvCpssDrvInterruptCauseRegsClear
*
* DESCRIPTION:
*       This function clears the interrupt cause registers for the given device.
*
* INPUTS:
*       devNum          - The device number to scan.
*       portGroupId          - The portGroupId. for multi-port-groups device.
*       pIntScanNode    - Struct for accessing interrupt register in hierarchy
*                         tree.
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
GT_STATUS prvCpssDrvInterruptCauseRegsClear
(
    IN GT_U8               devNum,
    IN GT_U32              portGroupId,
    IN PRV_CPSS_DRV_INTERRUPT_SCAN_STC      *pIntScanNode
);


/*******************************************************************************
* prvCpssDrvIntEvReqDrvnScan
*
* DESCRIPTION:
*       This function scans a given interrupt cause register, and performs all
*       needed interrupt nodes queuing.
*
* INPUTS:
*       devNum         - The device number to scan.
*       portGroupId          - The portGroupId. for multi-port-groups device.
*       intNodesPool   - Pool of interrupt nodes belonging to the scanned
*                        device.
*       intMaskShadow  - Interrupt mask registers shadow array.
*       pIntScanNode   - Structure for accessing interrupt register in hierarchy
*                         tree.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0 - always.
*
* COMMENTS:
*       This routine is used on systems configured to poll events.
*
*******************************************************************************/
GT_U8 prvCpssDrvIntEvReqDrvnScan
(
    GT_U8               devNum,
    GT_U32              portGroupId,
    PRV_CPSS_DRV_EV_REQ_NODE_STC            *intNodesPool,
    GT_U32              *intMaskShadow,
    PRV_CPSS_DRV_INTERRUPT_SCAN_STC      *pIntScanNode
);

/*******************************************************************************
* prvCpssDrvInterruptPpTxEndedCbBind
*
* DESCRIPTION:
*       bind a callback function for the device(Pp) for the "tx ended" event
*
* INPUTS:
*       devNum - the device number of the device that received the "tx ended"
*                event
*       txEndedCb - the call back function to be called when the "tx ended"
*                   event occur on the device
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - on bad device number.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptPpTxEndedCbBind
(
    IN  GT_U8   devNum,
    IN  PRV_CPSS_DRV_COM_INT_TX_ENDED_CB txEndedCb
);

/*******************************************************************************
* prvCpssDrvInterruptsMaskDefSummaryInit
*
* DESCRIPTION:
*       Initialize the interrupt mask default value for polling mode. Only the
*       summary bits need to be unmasked, cause bits are unmasked by user app
*       on event bind
*
* INPUTS:
*       devMaskRegSize      - The size of interrupt mask register array.
*       maskRegMapArr       - The interrupt mask register map.
*       intScanRootPtr      - The interrupt information hierarchy tree root.
*
* OUTPUTS:
*       maskRegDefArr       - The interrupt mask default value.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvInterruptsMaskDefSummaryInit
(
    IN GT_U32                   devMaskRegSize,
    IN GT_U32                   *maskRegMapArr,
    IN GT_U32                   *maskRegDefArr,
    OUT PRV_CPSS_DRV_INTERRUPT_SCAN_STC          *intScanRootPtr
);

/*******************************************************************************
* prvCpssDrvComIntSvcRtnDbRelease
*
* DESCRIPTION:
*       private (internal) function to release the DB of the interrupts.
*       NOTE: function  'free' the allocated memory.
*             and restore DB to 'pre-init' state
*
*  APPLICABLE DEVICES: ALL
*
* INPUTS:
*       none
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDrvComIntSvcRtnDbRelease
(
    void
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDrvComIntSvcRtnh */

