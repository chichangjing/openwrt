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
* gtTimeMeasure.h
*
* DESCRIPTION:
*       Tool for measuring exact times of part of code or
*       function calls execution.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __gtTimeMeasureh
#define __gtTimeMeasureh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

#define GT_DEFINE_TIME_EXACT_NODE(_name)                 \
    {                                                    \
        static GT_TIME_EXACT_NODE __node =               \
            {GT_FALSE, _name, NULL, {0,0}, {0,0}, 0};    \
        gtTimeExactProcessNode(&__node);                 \
    }

/*
 * Typedef: struct GT_TIME_EXACT
 *
 * Description:
 *     This structure defines the exact time.
 *
 *
 * Fields:
 *     seconds     - the whole seconds part of time.
 *     nanoSeconds - the nano seconds part of time, less then 1000000000.
 *
 *  Comments:
 *     The maximal value for such time representation more
 *     then 8 years.
 *     The osTimeRT function used for such time retrieving.
 */
typedef struct
{
    GT_U32          seconds;
    GT_U32          nanoSeconds;
} GT_TIME_EXACT;


/*
 * Typedef: struct GT_TIME_EXACT_NODE
 *
 * Description:
 *     This structure defines the exact time node.
 *     Dedicated to accomulate the time passed to the next time node.
 *
 *
 * Fields:
 *     isInitialized   - GT_FALSE - not initialized, GT_TRUE - initialized
 *     nodeName        - the name of node for printing
 *     nextNodePtr     - pointer to the next node for printing
 *                       not used for summing.
 *     lastMeasure     - the last mesured time.
 *     summaryTime     - the summary time up to the next time node.
 *     eventCount      - event count.
 *
 *  Comments:
 */
typedef struct _pss_GT_TIME_EXACT_NODE
{
    GT_BOOL                        isInitialized;
    char                           *nodeName;
    struct _pss_GT_TIME_EXACT_NODE *nextNodePtr;
    GT_TIME_EXACT                  lastMeasure;
    GT_TIME_EXACT                  summaryTime;
    GT_U32                         eventCount;
} GT_TIME_EXACT_NODE;

/*******************************************************************************
* gtTimeExactAdd
*
* DESCRIPTION:
*           Used to add exact times
*
* INPUTS:
*         time1Ptr - pointer to the time1 for summing
*         time2Ptr - pointer to the time2 for summing
* OUTPUTS:
*         sumPtr  - pointer to the time sum
*
* RETURNS:
*       None.
*
* COMMENTS:
*       The sum can be the same than one (or both) of added times
*
*******************************************************************************/
GT_VOID  gtTimeExactAdd
(
    IN      GT_TIME_EXACT *time1Ptr,
    IN      GT_TIME_EXACT *time2Ptr,
    OUT     GT_TIME_EXACT *sumPtr
);

/*******************************************************************************
* gtTimeExactSub
*
* DESCRIPTION:
*           Used to subtract exact times
*
* INPUTS:
*         time1Ptr - pointer to the time1 to subtract from
*         time2Ptr - pointer to the time2 to subtract it
* OUTPUTS:
*         subPtr  - pointer to the time sum
*
* RETURNS:
*       None.
*
* COMMENTS:
*       The sum can be the same than one (or both) of subtracted times
*
*******************************************************************************/
GT_VOID  gtTimeExactSub
(
    IN      GT_TIME_EXACT *time1Ptr,
    IN      GT_TIME_EXACT *time2Ptr,
    OUT     GT_TIME_EXACT *subPtr
);

/*******************************************************************************
* gtTimeExactProcessNode
*
* DESCRIPTION:
*           Used to process the node
*
* INPUTS:
*         nodePtr - pointer to processed node
* OUTPUTS:
*         nodePtr - pointer to processed node
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtTimeExactProcessNode
(
    INOUT   GT_TIME_EXACT_NODE *nodePtr
);

/*******************************************************************************
* gtTimeExactDumpNodes
*
* DESCRIPTION:
*           Used to dump all the nodes
*
* INPUTS:
*         None.
* OUTPUTS:
*         None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtTimeExactDumpNodes
(
    GT_VOID
);

/*******************************************************************************
* gtTimeExactResetNodes
*
* DESCRIPTION:
*           Used to Reset all the nodes
*
* INPUTS:
*         None.
* OUTPUTS:
*         None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID  gtTimeExactResetNodes
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif

#endif  /* __gtTimeMeasureh */
/* Do Not Add Anything Below This Line */

