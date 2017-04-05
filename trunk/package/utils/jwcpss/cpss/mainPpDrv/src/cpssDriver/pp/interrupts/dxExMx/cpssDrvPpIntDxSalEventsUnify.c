/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDxSalEventsUnify.c
*
* DESCRIPTION:
*       Conversion routines for unified events -- Salsa devices.
*
* FILE REVISION NUMBER:
*       $Revision: 3$
*
*******************************************************************************/
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsSalsa.h>
/* get the unify events definitions */
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>

/*******************************************************************************
* internal definitions
*******************************************************************************/
/* #define SAL_UNI_CNVRT_DEBUG */

#ifdef SAL_UNI_CNVRT_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

/* enumeration to convert name from the API to application to be more
  "consistent" with the naming conventions of this file */
enum{
    CPSS_PP_PORT_TQ_WATCHDOG_EX_E         = CPSS_PP_TQ_WATCHDOG_EX_PORT_E,
    CPSS_PP_PORT_TQ_TXQ2_FLUSH_E          = CPSS_PP_TQ_TXQ2_FLUSH_PORT_E,
    CPSS_PP_PORT_TQ_HOL_REACHED_E         = CPSS_PP_TQ_HOL_REACHED_PORT_E,
    CPSS_PP_TX_END_QUEUE_E                = CPSS_PP_TX_END_E
};

/*
 * Typedef: enum UNI_EV_2_EV_OFFSET
 *
 * Enumeration: Lists the conversion table column data types.
 *
 * Enums:
 *      UNI_EV              - Unified event
 *      SALSA            - salsa events
 *      EXT_DATA            - Additional data for unified event
 *      TABLE_ENTRY_SIZE_E    - Table size (MUST be last !!)
 *
 */
typedef enum
{
    UNI_EV_E,
    SALSA_E,
    EXT_DATA_E,
    TABLE_ENTRY_SIZE_E
} UNI_EV_2_EV_OFFSET;



/* build name group - per index
    example : INDEX_EV(TG,GPP_INTERRUPT,2)
    target: PRV_CPSS_TG_GPP_INTERRUPT_INDEX2
*/
#define INDEX_EV(dev,event,index)    PRV_CPSS_##dev##_##event##_##index##_E

#define UNI_PORT_GEN_EV(event)     CPSS_PP_PORT_##event##_E

/* build name of group of events: per port , per queue , per index
    example : GROUP_EV(TG,12,IPG_TOO_SMALL,PORT,3)
    target: PRV_CPSS_TG_12_IPG_TOO_SMALL_PORT3
*/
#define GROUP_EV(dev,event,group,index)   PRV_CPSS_##dev##_##event##_##group##index##_E

/* build name group - per port
    example : PORT_EV(TG,12,IPG_TOO_SMALL,3)
    target: PRV_CPSS_TG_12_IPG_TOO_SMALL_PORT3
*/
#define PORT_EV(dev,event,port)     GROUP_EV(dev,event,PORT,port)

/* build the name of the event :
    example : GEN_EV(TG,12,TQ_MC_FIFO_OVERRUN)
    target: PRV_CPSS_TG_12_TQ_MC_FIFO_OVERRUN_E
*/
#define GEN_EV(dev,event)          PRV_CPSS_##dev##_##event##_E

/* the name of the unify event - based on the event name */
#define UNI_GEN_EV(event)          CPSS_PP_##event##_E

/* build name group - per queue
    example : QUEUE_EV(TG,12,TX_BUFFER,5)
    target: PRV_CPSS_TG_12_TX_BUFFER_QUEUE5
*/
#define QUEUE_EV(dev,event,queue)    GROUP_EV(dev,event,QUEUE,queue)

#define UNI_QUEUE_GEN_EV(event)    CPSS_PP_##event##_QUEUE_E

/* CPU_PORT exists on all devices */
#define CPU_PORT_EV_ALL(event) \
    {UNI_PORT_GEN_EV(event) , \
        PORT_EV(SAL,event,_CPU_31),    \
        31                            \
    }

#define GEN_EV_ALL(event)       \
    {UNI_GEN_EV(event) ,        \
        GEN_EV(SAL,event),   \
        0                       \
    }

/* build name group - per index
    example : INTERRUPT_EV(TG,12,GPP,2)
    target: PRV_CPSS_TG_12_GPP_INTERRUPT2
*/
#define INTERRUPT_EV(dev,event,index)    GROUP_EV(dev,event,INTERRUPT,index)

/* the name of the unify event , but specific per queue -
   based on the event name , and queue number
   example : UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(RX_BUFFER,6)
   target : CPSS_RX_BUFFER_QUEUE6
*/
#define UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(event,queue) CPSS_PP_##event##_QUEUE##queue##_E

/* interrupts that has index , the used number is (index-1) */
#define INTERRUPT_EV_ALL(event,index)       \
    {UNI_GEN_EV(event) ,        \
        INTERRUPT_EV(SAL,event,index),    \
        (index-1)                           \
    }

/* index only in extra parameter */
#define EV_INDEX_ONLY(event,index)       \
    {UNI_GEN_EV(event) ,    \
        GEN_EV(SAL,event),   \
        index               \
    }

/* index only in extra parameter that uses different name for the unify name
   and for the PPs */
#define EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index)       \
    {UNI_GEN_EV(event) ,    \
        GEN_EV(SAL,ppEvent),   \
        index               \
    }

/* support up ports 0..23
   all devices support those events
*/
#define PORT_EV_ALL(event,port)\
    {UNI_PORT_GEN_EV(event) ,        \
        PORT_EV(SAL,event,port),  \
        port                         \
    }

/* ports 0..23 */
#define LOOP_ALL_PORTS(event)     \
    PORT_EV_ALL(event,0),         \
    PORT_EV_ALL(event,1),         \
    PORT_EV_ALL(event,2),         \
    PORT_EV_ALL(event,3),         \
    PORT_EV_ALL(event,4),         \
    PORT_EV_ALL(event,5),         \
    PORT_EV_ALL(event,6),         \
    PORT_EV_ALL(event,7),         \
    PORT_EV_ALL(event,8),         \
    PORT_EV_ALL(event,9),         \
    PORT_EV_ALL(event,10),        \
    PORT_EV_ALL(event,11),        \
    PORT_EV_ALL(event,12),        \
    PORT_EV_ALL(event,13),        \
    PORT_EV_ALL(event,14),        \
    PORT_EV_ALL(event,15),        \
    PORT_EV_ALL(event,16),        \
    PORT_EV_ALL(event,17),        \
    PORT_EV_ALL(event,18),        \
    PORT_EV_ALL(event,19),        \
    PORT_EV_ALL(event,20),        \
    PORT_EV_ALL(event,21),        \
    PORT_EV_ALL(event,22),        \
    PORT_EV_ALL(event,23)

/* index in extra parameter and in PP event name*/
#define EV_INDEX_SPECIFIC(event,index)       \
    {UNI_GEN_EV(event) ,    \
        INDEX_EV(SAL,event,index),  \
        index               \
    }

/* general events that are per queue */
#define QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,queue)       \
    {UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(event,queue) ,        \
        QUEUE_EV(SAL,event,queue),    \
        queue                           \
    }

#define QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(event)    \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,0),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,1),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,2),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,3),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,4),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,5),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,6),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,7)

/* general events that are per queue */
#define QUEUE_EV_ALL(event,queue)       \
    {UNI_QUEUE_GEN_EV(event) ,          \
        QUEUE_EV(SAL,event,queue),   \
        queue                           \
    }

/* the conversion table */
static GT_U16 ppUni2IntCauseType[][TABLE_ENTRY_SIZE_E] = {
      GEN_EV_ALL(MISC_TWSI_TIME_OUT),
      GEN_EV_ALL(MISC_TWSI_STATUS),
      GEN_EV_ALL(MISC_ILLEGAL_ADDR),
      GEN_EV_ALL(MISC_CPU_PORT_RX_OVERRUN),
      GEN_EV_ALL(MISC_CPU_PORT_TX_OVERRUN),

      INTERRUPT_EV_ALL(GPP,1),
      INTERRUPT_EV_ALL(GPP,2),
      INTERRUPT_EV_ALL(GPP,3),
      INTERRUPT_EV_ALL(GPP,4),
      INTERRUPT_EV_ALL(GPP,5),
      INTERRUPT_EV_ALL(GPP,6),
      INTERRUPT_EV_ALL(GPP,7),
      INTERRUPT_EV_ALL(GPP,8),

      GEN_EV_ALL(BM_EMPTY_CLEAR),
      GEN_EV_ALL(BM_EMPTY_INC),
      GEN_EV_ALL(BM_AGED_PACKET),
      GEN_EV_ALL(BM_MAX_BUFF_REACHED),
      GEN_EV_ALL(BM_PORT_RX_BUFFERS_CNT_UNDERRUN),
      GEN_EV_ALL(BM_PORT_RX_BUFFERS_CNT_OVERRUN),
      GEN_EV_ALL(BM_INVALID_ADDRESS),
      GEN_EV_ALL(BM_WRONG_SRC_PORT),
      GEN_EV_ALL(BM_MC_INC_OVERFLOW),
      GEN_EV_ALL(MAC_AGE_VIA_TRIGGER_ENDED),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,BM_FLL_PARITY,0),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,BM_CTRL0_PARITY,1),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,BM_CTRL1_PARITY,2),

      LOOP_ALL_PORTS(LINK_STATUS_CHANGED),/*LINK_CHANGE*/
      LOOP_ALL_PORTS(AN_COMPLETED),/*PORT_AN_COMPLETED*/
      LOOP_ALL_PORTS(RX_FIFO_OVERRUN),/*RX_FIFO_OVERRUN*/
      LOOP_ALL_PORTS(TX_FIFO_UNDERRUN),/*PORT_TX_FIFO_UNDERRUN*/

      EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,0),
      EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,1),
      EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,2),
      EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,3),

      EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,0),
      EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,1),
      EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,2),
      EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,3),

      EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,0),
      EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,1),
      EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,2),
      EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,3),

      LOOP_ALL_PORTS(SYNC_STATUS_CHANGED),/*PORT_SYNC_STATUS_CHANGED*/
      LOOP_ALL_PORTS(PRBS_ERROR),/*PORT_PRBS_ERROR*/

      LOOP_ALL_PORTS(TX_CRC_ERROR),/*GOP_TX_CRC_ERROR_ON_PORT*/
      CPU_PORT_EV_ALL(TX_CRC_ERROR),

      LOOP_ALL_PORTS(TQ_WATCHDOG_EX),/*TQ_WATCHDOG_EX_PORT*/
      CPU_PORT_EV_ALL(TQ_WATCHDOG_EX),/*TQ_WATCHDOG_EX_CPU*/
      LOOP_ALL_PORTS(TQ_TXQ2_FLUSH),/*TQ_TXQ2_FLUSH_PORT*/

      EV_INDEX_ONLY(TQ_TXQ2_MG_FLUSH,1),
      EV_INDEX_ONLY(TQ_ONE_ECC_ERROR,1),
      EV_INDEX_ONLY(TQ_TWO_ECC_ERROR,1),
      EV_INDEX_ONLY(TQ_MG_READ_ERR,1),

      LOOP_ALL_PORTS(TQ_HOL_REACHED),/*TQ_HOL_REACHED_PORT*/
      CPU_PORT_EV_ALL(TQ_HOL_REACHED),/*TQ_HOL_REACHED_PORT*/


      GEN_EV_ALL(TQ_TOTAL_DESC_UNDERFLOW),
      GEN_EV_ALL(TQ_TOTAL_DESC_OVERFLOW),
      GEN_EV_ALL(TQ_SNIFF_DESC_DROP),
      GEN_EV_ALL(EB_NA_FIFO_FULL),
      GEN_EV_ALL(EB_MG_ADDR_OUT_OF_RANGE),
      GEN_EV_ALL(EB_VLAN_SECURITY_BREACH),
      GEN_EV_ALL(EB_NA_NOT_LEARNED_SECURITY_BREACH),
      GEN_EV_ALL(EB_SA_MSG_DISCARDED),
      GEN_EV_ALL(EB_QA_MSG_DISCARDED),
      GEN_EV_ALL(EB_SA_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_DA_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_DA_SA_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_NA_ON_LOCKED_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_MAC_RANGE_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_INVALID_SA_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_VLAN_NOT_VALID_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_VLAN_NOT_MEMBER_DROP_DROP_SECURITY_BREACH),
      GEN_EV_ALL(EB_VLAN_RANGE_DROP_SECURITY_BREACH),
      GEN_EV_ALL(MAC_NUM_OF_HOP_EXP),
      GEN_EV_ALL(MAC_NA_LEARNED),
      GEN_EV_ALL(MAC_NA_NOT_LEARNED),
      GEN_EV_ALL(MAC_AGE_VIA_TRIGGER_ENDED),
      GEN_EV_ALL(MAC_MG_ADDR_OUT_OF_RANGE),
      GEN_EV_ALL(MAC_UPDATE_FROM_CPU_DONE),
      GEN_EV_ALL(MAC_MESSAGE_TO_CPU_READY),
      GEN_EV_ALL(MAC_NA_SELF_LEARNED),
      GEN_EV_ALL(MAC_NA_FROM_CPU_LEARNED),
      GEN_EV_ALL(MAC_NA_FROM_CPU_DROPPED),
      GEN_EV_ALL(MAC_AGED_OUT),
      GEN_EV_ALL(MAC_FIFO_2_CPU_EXCEEDED),

      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(MAC_1ECC_ERRORS,MAC_1ECC_ERRORS_EVEN,0),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(MAC_1ECC_ERRORS,MAC_1ECC_ERRORS_ODD,1),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(MAC_2ECC_ERRORS,MAC_2ECC_ERRORS_EVEN,0),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(MAC_2ECC_ERRORS,MAC_2ECC_ERRORS_ODD,1),

      GEN_EV_ALL(EB_AUQ_PENDING),

      EV_INDEX_ONLY(TX_END,0),/*TX_END_QUEUE*/

      QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(RX_BUFFER)/*RX_BUFFER_QUEUE*/

};
#define NUM_OF_TBL_ENTRIES \
    (sizeof(ppUni2IntCauseType)/(sizeof(GT_U16)*TABLE_ENTRY_SIZE_E))


/*******************************************************************************
* ppColumnGet
*
* DESCRIPTION:
*       Returns the pp column in conversion table.
*
* INPUTS:
*       devNum     - the device number that set the event .
*
* OUTPUTS:
*       ppColumnPtr - The device column in table.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_BAD_PARAM - unknown device type or number of ports
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS ppColumnGet
(
    IN  GT_U8               devNum,
    OUT GT_U32              *ppColumnPtr
)
{
    *ppColumnPtr = SALSA_E;

    if(prvCpssDrvPpConfig[devNum]->devFamily != CPSS_PP_FAMILY_SALSA_E)
    {
        DBG_INFO(("file:%s line:%d intType:%d\n", __FILE__, __LINE__,
                  intCause->intType));
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvPpIntSalsaIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type -- Salsa
*
* INPUTS:
*       devNum      - The device number.
*       portGroupId      - The portGroupId. support the multi-port-groups device.
*                     ignored for non multi-port-groups device.
*       intCauseIndex - The interrupt cause to convert.
*
* OUTPUTS:
*       uniEvPtr    - (pointer to)The unified event type.
*       extDataPtr  - (pointer to)The event extended data.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the interrupt cause to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntSalsaIntCauseToUniEvConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_U32                      intCauseIndex,
    OUT GT_U32                      *uniEvPtr,
    OUT GT_U32                      *extDataPtr
)
{
    GT_STATUS       rc;
    GT_U32          ii;          /* iterator                     */
    GT_U32          ppColumn;   /* conversion table pp column   */

    CPSS_NULL_PTR_CHECK_MAC(uniEvPtr);
    CPSS_NULL_PTR_CHECK_MAC(extDataPtr);

    portGroupId = portGroupId;/*parameter not used*/

    *uniEvPtr   = CPSS_UNI_RSRVD_EVENT_E;
    *extDataPtr = 0;

    rc = ppColumnGet(devNum, &ppColumn);
    if(rc != GT_OK)
    {
        return rc;
    }

    for (ii = 0; ii < NUM_OF_TBL_ENTRIES ; ii++)
    {
        if (ppUni2IntCauseType[ii][ppColumn] == intCauseIndex)
        {
            *uniEvPtr   = ppUni2IntCauseType[ii][UNI_EV_E];
            *extDataPtr = ppUni2IntCauseType[ii][EXT_DATA_E];
            return GT_OK;
        }
    }
    DBG_INFO(("file:%s line:%d event not found intCause = %d\n", __FILE__,
              __LINE__, intCauseIndex));

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}



