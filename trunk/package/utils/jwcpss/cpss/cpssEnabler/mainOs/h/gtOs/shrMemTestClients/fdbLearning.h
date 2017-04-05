/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* fdbLearning.h
*
* DESCRIPTION:
*       This file contains function declarations, typedefs and defines for fdbLearning.c 
*       
*       
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __fdbLearningh
#define __fdbLearningh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */



/*************            defines            *******************************/
#define EV_HANDLER_MAX_PRIO     200
#define _PRV_CPSS_MAX_PP_DEVICES_CNS 1

#ifdef FDB_PROCESS_DEBUG
	static char * uniEvName[CPSS_UNI_EVENT_COUNT_E] = {UNI_EV_NAME};
	#define DBG_LOG(x)  osPrintf x
	#define DBG_INFO(x)   osPrintf 
#else
	#define DBG_LOG(x)  
	#define DBG_INFO(x)  
#endif



/*************           typedefs          *******************************/
typedef struct
{
    GT_UINTPTR          evHndl;
    GT_U32              hndlrIndex;
} EV_HNDLR_PARAM;

/**************** static func declaration *****************************/
/*******************************************************************************
* fdbLearningEventTreat
*
* DESCRIPTION:
*       This routine handles FDB learning events
*
* INPUTS:
*       devNum      - the device number.
*       uniEv       - Unified event number
*       evExtData   - Unified event additional information
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
static GT_STATUS fdbLearningEventTreat
(
    GT_U8                   devNum,
    GT_U32                  uniEv,
    GT_U32                  evExtData
);

/*******************************************************************************
* fdbLearningAuMsgGet
*
* DESCRIPTION:
*       This routine handles Address Update messages.
*
* INPUTS:
*       devNum      - the device number.
*       evExtData   - Unified event additional information
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
static GT_STATUS fdbLearningAuMsgGet
(
    IN GT_U8                devNum,
    GT_U32                  evExtData
);

/*******************************************************************************
* fdbLearningAuMsgHandle
*
* DESCRIPTION:
*       This routine gets and handles the ChDx Address Update messages.
*
* INPUTS:
*       devNum      - the device number.
*       evExtData   - Unified event additional information
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
static GT_STATUS fdbLearningAuMsgHandle
(
    IN GT_U8                devNum,
    GT_U32                  evExtData
);

/*******************************************************************************
* fdbLearningEventsHndlr
*
* DESCRIPTION:
*       This routine is the event handler for PSS Event-Request-Driven mode
*       (polling mode).
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
*       None.
*
*******************************************************************************/
static unsigned __TASKCONV fdbLearningEventsHndlr
(
    GT_VOID * param
);



/**************** var decl *****************************/

/* event array */

#ifdef CHX_FAMILY
CPSS_UNI_EV_CAUSE_ENT       evHndlrCauseArr[] =  
	{
		CPSS_PP_EB_AUQ_PENDING_E,
		CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E
	};
#endif

#ifdef EXMXPM_FAMILY
CPSS_UNI_EV_CAUSE_ENT       evHndlrCauseArr[] =  
	{
		CPSS_PP_EB_AUQ_PENDING_E
	};
#endif

/*******************************************************************************
* prvUniEvMaskAllSet
*
* DESCRIPTION:
*       This routine unmasks all the events according to the unified event list.
*
* INPUTS:
*       cpssUniEventArr - The CPSS unified event list.
*       arrLength   - The unified event list length.
*       operation   - type of operation mask/unmask to do on the events
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL if failed.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvUniEvMaskAllSet
(
    IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
);
/*******************************************************************************
* fdbEventRequestDrvnModeInit
*
* DESCRIPTION:
*       This routine run event handler.
*
* INPUTS:
*       None.
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
GT_STATUS fdbEventRequestDrvnModeInit
(
    IN GT_VOID
);

GT_VOID shrMemPrintMapsDebugInfo(GT_VOID);

GT_STATUS interruptEthPortRxSR
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           rxQueue
);

GT_STATUS interruptEthPortTxEndSR
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments
);

GT_VOID shrMemPrintMapsDebugInfo(GT_VOID);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* __fdbLearningh */


