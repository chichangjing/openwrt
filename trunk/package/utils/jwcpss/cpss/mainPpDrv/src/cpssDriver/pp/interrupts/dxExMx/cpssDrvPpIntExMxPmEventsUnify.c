/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntExMxPmEventsUnify.c
*
* DESCRIPTION:
*       Conversion routines for unified events -- Puma devices.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsPuma.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsPuma3.h>
/* get the unify events definitions */
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/private/prvCpssGenEvReq.h>

/*******************************************************************************
* internal definitions
*******************************************************************************/
/* #define CH_UNI_CNVRT_DEBUG */

#ifdef CH_UNI_CNVRT_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

/* Puma3 interrupt conversion tables */
extern GT_U16 puma3UniEvMapTableWithExtData[];
extern GT_U32 puma3UniEvMapTableWithExtDataSize;
extern GT_U16 puma3UniEvMapTable[][2];
extern GT_U32 puma3UniEvMapTableSize;


/* enumeration to convert name from the API to application to be more
  "consistent" with the naming conventions of this file */
enum{
    CPSS_PP_PORT_BM_MAX_BUFF_REACHED_E    = CPSS_PP_BM_MAX_BUFF_REACHED_E,
    CPSS_PP_PORT_BM_EGRESS_MAX_BUFF_REACHED_E = CPSS_PP_BM_EGRESS_MAX_BUFF_REACHED_E,
    CPSS_PP_PORT_TQ_TXQ2_FLUSH_E          = CPSS_PP_TQ_TXQ2_FLUSH_PORT_E,
    CPSS_PP_TX_END_QUEUE_E                = CPSS_PP_TX_END_E,
    CPSS_PP_PORT_BM_TQ_PARITY_ERROR_E     = CPSS_PP_BM_TQ_PARITY_ERROR_PORT_E,
    CPSS_PP_PORT_MAC_SFLOW_E              = CPSS_PP_MAC_SFLOW_E,
    CPSS_PP_PORT_EGRESS_SFLOW_E           = CPSS_PP_EGRESS_SFLOW_E,
    CPSS_PP_PORT_TQ_DESC_FULL_E           = CPSS_PP_TQ_PORT_DESC_FULL_E,
    CPSS_PP_PORT_TQ_FULL_XG_E             = CPSS_PP_TQ_PORT_FULL_XG_E
};

/*
 * Typedef: enum UNI_EV_2_EV_OFFSET
 *
 * Enumeration: Lists the conversion table column data types.
 *
 * Enums:
 *      UNI_EV_E            - Unified event
 *      PUMA_E              - Puma event
 *
 *      EXT_DATA_E          - Additional data for unified event
 *      TABLE_ENTRY_SIZE_E  - Table size (MUST be last !!)
 *
 */
typedef enum
{
    UNI_EV_E,
    PUMA_E,

    EXT_DATA_E,
    TABLE_ENTRY_SIZE_E
} UNI_EV_2_EV_OFFSET;


/* build the name of the event :
    example : GEN_EV_MAC(PM,TQ_MC_FIFO_OVERRUN)
    target: PRV_CPSS_EXMXPM_TQ_MC_FIFO_OVERRUN_E
*/
#define GEN_EV_MAC(dev,event)          PRV_CPSS_EXMX##dev##_##event##_E

#define GEN_EV_WITH_REG_NAME_MAC(dev,regName,event)   PRV_CPSS_EXMX##dev##_##regName##_##event##_E

/* the name of the unify event - based on the event name */
#define UNI_GEN_EV_MAC(event)          CPSS_PP_##event##_E

/* the name of the unify event ,with reg name - based on the event name */
#define UNI_GEN_EV_WITH_REG_NAME_MAC(regName,event)  CPSS_PP_##regName##_##event##_E

/* build name of group of events: per port , per queue , per index
    example : GROUP_EV_MAC(PM,IPG_TOO_SMALL,PORT,3)
    target: PRV_CPSS_EXMXPM_IPG_TOO_SMALL_PORT3
*/
#define GROUP_EV_MAC(dev,event,group,index)   PRV_CPSS_EXMX##dev##_##event##_##group##index##_E

/* build name of group of events: per lane per port
    example: GROUP_2_EV(CH_HX,DETECTED_IIAII,LANE,0,PORT,25)
    target: PRV_CPSS_CH_HX_DETECTED_IIAII_LANE0_PORT25_E
*/
#define GROUP_2_EV_MAC(dev,event,group1,index1,group2,index2)   PRV_CPSS_EXMX##dev##_##group1##index1##_##event##_##group2##index2##_E

#define PREFIX_GROUP_EV_MAC(dev,event,group,index)   PRV_CPSS_EXMX##dev##_##group##index##_##event##_E

/* build name group - per port
    example : PORT_EV_MAC(PM,IPG_TOO_SMALL,3)
    target: PRV_CPSS_EXMXPM_IPG_TOO_SMALL_PORT3
*/
#define PORT_EV_MAC(dev,event,port)     GROUP_EV_MAC(dev,event,PORT,port)

#define CPU_PORT_EV_MAC(dev,event)      PRV_CPSS_EXMX##dev##_##event##_##CPU##_E

/* build name group - per port (prefix)
    example : PORT_EV_MAC(PM,IPG_TOO_SMALL,3)
    target: PRV_CPSS_EXMXPM_PORT3_IPG_TOO_SMALL
*/
#define PREFIX_PORT_EV_MAC(dev,event,port)     PREFIX_GROUP_EV_MAC(dev,event,PORT,port)

/* build name group - per index
    example : INDEX_EV_MAC(PM,GPP_INTERRUPT,2)
    target: PRV_CPSS_EXMXPM_GPP_INTERRUPT_INDEX2
*/
#define INDEX_EV_MAC(dev,event,index)    PRV_CPSS_EXMX##dev##_##event##index##_E

/* build name group - per queue
    example : QUEUE_EV_MAC(PM,TX_BUFFER,5)
    target: PRV_CPSS_EXMXPM_TX_BUFFER_QUEUE5
*/
#define QUEUE_EV_MAC(dev,event,queue)    GROUP_EV_MAC(dev,event,QUEUE,queue)

/* the name of the unify event , but specific per queue -
   based on the event name , and queue number
   example : UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME_MAC(RX_BUFFER,6)
   target : CPSS_RX_BUFFER_QUEUE6
*/
#define UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME_MAC(event,queue) CPSS_PP_##event##_QUEUE##queue##_E

/* build name group - per index
    example : INTERRUPT_EV_MAC(PM,GPP,2)
    target: PRV_CPSS_EXMXPM_GPP_INTERRUPT2
*/
#define INTERRUPT_EV_MAC(dev,event,index)    GROUP_EV_MAC(dev,event,INTERRUPT,index)

#define UNI_PORT_GEN_EV_MAC(event)     CPSS_PP_PORT_##event##_E

#define UNI_QUEUE_GEN_EV_MAC(event)    CPSS_PP_##event##_QUEUE_E

#define UNI_PORT_PCS_GEN_EV_MAC(event)     CPSS_PP_PORT_PCS_##event##_E

#define UNI_PORT_LANE_GEN_EV_MAC(event)    CPSS_PP_PORT_LANE_##event##_E

#define UNI_HGL_EV_MAC(event)           CPSS_PP_HGLINK_##event##_E

#define PORT_XPC_PORT_EV_MAC(dev,event,index) PRV_CPSS_EXMX##dev##_XPC##index##_##event##_E

#define PORT_XPC_PORT_24_EV_MAC(dev,event) PORT_XPC_PORT_EV_MAC(dev,event,0)

#define PORT_XPC_PORT_25_EV_MAC(dev,event) PORT_XPC_PORT_EV_MAC(dev,event,1)

#define PORT_LANE_EV_MAC(dev,event,port,lane) GROUP_2_EV_MAC(dev,event,LANE,lane,PORT,port)

#define HGL_EV_MAC(dev,event,hgl)  PREFIX_GROUP_EV_MAC(dev,event,HGLINK,hgl)

/* convert port,lane to extData */
#define LANE_PORT_TO_EXT_DATA_CONVERT_MAC(port,lane) ((port) << 8 | (lane))


/* CPU_PORT exists on all devices */
#define CPU_PORT_EV_ALL_MAC(event) \
    {UNI_PORT_GEN_EV_MAC(event) , \
        CPU_PORT_EV_MAC(PM,event),    \
        63                            \
    }

/* CPU_PORT exists on all devices */
#define CPU_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent) \
    {UNI_PORT_GEN_EV_MAC(uniEvent) , \
        CPU_PORT_EV_MAC(PM,pmEvent),    \
        63                            \
    }

/* build general entry in DB , that support all the devices
   it uses index 0 in field EXT_DATA
   example: GEN_EV_ALL(SLV_WRITE_ERR)
   target :
   GT_PP_SLV_WRITE_ERR ,
   PRV_CPSS_EXMXPM_SLV_WRITE_ERR,
   0
*/
#define GEN_EV_ALL_MAC(event)       \
    {UNI_GEN_EV_MAC(event) ,        \
        GEN_EV_MAC(PM,event),       \
        0                       \
    }

/* interrupts that has index , the used number is (index-1) */
#define INTERRUPT_EV_ALL_MAC(event,index)       \
    {UNI_GEN_EV_MAC(event) ,        \
        INTERRUPT_EV_MAC(PM,event,index),    \
        (index-1)                        \
    }

/* index in extra parameter and in PP event name*/
#define EV_INDEX_SPECIFIC_MAC(event,index)       \
    {UNI_GEN_EV_MAC(event) ,    \
        INDEX_EV_MAC(PM,event,index),   \
        index               \
    }

#define EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(event, ppEvent, index)       \
    {UNI_GEN_EV_MAC(event) ,    \
        INDEX_EV_MAC(PM, ppEvent,index),   \
        index               \
    }

/* index only in extra parameter that uses different name for the unify name
   and for the PPs */
#define EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(event,ppEvent,index)       \
    {UNI_GEN_EV_MAC(event) ,    \
        GEN_EV_MAC(PM,ppEvent),   \
        index               \
    }

#define EV_ONLY_DIFF_NAME_FOR_PP_MAC(event,ppEvent)       \
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(event,ppEvent,0)


/* port event all devices */
#define PORT_EV_ALL_MAC(event,port) \
    {UNI_PORT_GEN_EV_MAC(event) ,      \
        PORT_EV_MAC(PM,event,port),    \
        port                       \
    }

/* port event all devices */
#define PREFIX_PORT_EV_ALL_MAC(event,port) \
    {UNI_PORT_GEN_EV_MAC(event) ,      \
        PREFIX_PORT_EV_MAC(PM,event,port),    \
        port                       \
    }

/* port event all devices */
#define PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,port) \
    {UNI_PORT_GEN_EV_MAC(uniEvent) ,      \
        PORT_EV_MAC(PM,pmEvent,port),    \
        port                       \
    }

/* port event all devices */
#define PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,port) \
    {UNI_PORT_GEN_EV_MAC(uniEvent) ,      \
        PREFIX_PORT_EV_MAC(PM,pmEvent,port),    \
        port                       \
    }


/* port on ch hx event all devices */
#define CH_HX_PORT_EV_ALL_MAC(event,port) \
    {UNI_PORT_GEN_EV_MAC(event) ,      \
        PORT_EV_MAC(CH_HX,event,port),    \
        port                       \
    }

/* ports 0..23 */
#define LOOP_0_23_PORTS_MAC(event)           \
    PORT_EV_ALL_MAC(event,0),         \
    PORT_EV_ALL_MAC(event,1),         \
    PORT_EV_ALL_MAC(event,2),         \
    PORT_EV_ALL_MAC(event,3),         \
    PORT_EV_ALL_MAC(event,4),         \
    PORT_EV_ALL_MAC(event,5),         \
    PORT_EV_ALL_MAC(event,6),         \
    PORT_EV_ALL_MAC(event,7),         \
    PORT_EV_ALL_MAC(event,8),         \
    PORT_EV_ALL_MAC(event,9),         \
    PORT_EV_ALL_MAC(event,10),        \
    PORT_EV_ALL_MAC(event,11),        \
    PORT_EV_ALL_MAC(event,12),        \
    PORT_EV_ALL_MAC(event,13),        \
    PORT_EV_ALL_MAC(event,14),        \
    PORT_EV_ALL_MAC(event,15),        \
    PORT_EV_ALL_MAC(event,16),        \
    PORT_EV_ALL_MAC(event,17),        \
    PORT_EV_ALL_MAC(event,18),        \
    PORT_EV_ALL_MAC(event,19),        \
    PORT_EV_ALL_MAC(event,20),        \
    PORT_EV_ALL_MAC(event,21),        \
    PORT_EV_ALL_MAC(event,22),        \
    PORT_EV_ALL_MAC(event,23)

/* ports 0..23, 27 */
#define PREFIX_LOOP_0_23_27_PORTS_MAC(event)           \
    PREFIX_PORT_EV_ALL_MAC(event,0),         \
    PREFIX_PORT_EV_ALL_MAC(event,1),         \
    PREFIX_PORT_EV_ALL_MAC(event,2),         \
    PREFIX_PORT_EV_ALL_MAC(event,3),         \
    PREFIX_PORT_EV_ALL_MAC(event,4),         \
    PREFIX_PORT_EV_ALL_MAC(event,5),         \
    PREFIX_PORT_EV_ALL_MAC(event,6),         \
    PREFIX_PORT_EV_ALL_MAC(event,7),         \
    PREFIX_PORT_EV_ALL_MAC(event,8),         \
    PREFIX_PORT_EV_ALL_MAC(event,9),         \
    PREFIX_PORT_EV_ALL_MAC(event,10),        \
    PREFIX_PORT_EV_ALL_MAC(event,11),        \
    PREFIX_PORT_EV_ALL_MAC(event,12),        \
    PREFIX_PORT_EV_ALL_MAC(event,13),        \
    PREFIX_PORT_EV_ALL_MAC(event,14),        \
    PREFIX_PORT_EV_ALL_MAC(event,15),        \
    PREFIX_PORT_EV_ALL_MAC(event,16),        \
    PREFIX_PORT_EV_ALL_MAC(event,17),        \
    PREFIX_PORT_EV_ALL_MAC(event,18),        \
    PREFIX_PORT_EV_ALL_MAC(event,19),        \
    PREFIX_PORT_EV_ALL_MAC(event,20),        \
    PREFIX_PORT_EV_ALL_MAC(event,21),        \
    PREFIX_PORT_EV_ALL_MAC(event,22),        \
    PREFIX_PORT_EV_ALL_MAC(event,23),        \
    PREFIX_PORT_EV_ALL_MAC(event,27)

/* virtual ports 0..61 */
#define LOOP_0_61_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent) \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 0),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 1),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 2),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 3),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 4),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 5),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 6),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 7),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 8),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent, 9),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,10),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,11),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,12),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,13),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,14),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,15),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,16),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,17),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,18),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,19),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,20),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,21),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,22),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,23),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,24),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,25),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,26),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,27),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,28),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,29),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,30),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,31),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,32),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,33),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,34),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,35),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,36),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,37),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,38),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,39),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,40),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,41),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,42),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,43),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,44),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,45),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,46),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,47),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,48),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,49),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,50),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,51),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,52),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,53),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,54),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,55),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,56),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,57),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,58),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,59),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,60),     \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,61)

/* ports 0..23 */
#define LOOP_0_23_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent) \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,0),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,1),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,2),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,3),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,4),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,5),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,6),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,7),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,8),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,9),         \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,10),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,11),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,12),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,13),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,14),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,15),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,16),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,17),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,18),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,19),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,20),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,21),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,22),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,23)

/* ports 0..23 */
#define PREFIX_LOOP_0_23_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent) \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,0),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,1),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,2),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,3),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,4),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,5),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,6),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,7),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,8),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,9),         \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,10),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,11),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,12),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,13),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,14),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,15),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,16),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,17),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,18),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,19),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,20),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,21),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,22),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,23)

/* ports 24..25 */
#define LOOP_24_25_PORTS_MAC(event)   \
    PORT_EV_ALL_MAC(event,24),        \
    PORT_EV_ALL_MAC(event,25)

/* ports 24..25 */
#define PREFIX_LOOP_24_25_PORTS_MAC(event)   \
    PREFIX_PORT_EV_ALL_MAC(event,24),        \
    PREFIX_PORT_EV_ALL_MAC(event,25)

/* ports 24..25 */
#define LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)   \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,24),        \
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,25)

/* ports 24..25 */
#define PREFIX_LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)   \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,24),        \
    PREFIX_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,25)

/* ports 0..23 , CPU (63) */
#define LOOP_ALL_PORTS_NO_XG_MAC(event)           \
    LOOP_0_23_PORTS_MAC(event),             \
    CPU_PORT_EV_ALL_MAC(event)

/* ports 0..23 , with no XG, no CPU  */
#define LOOP_ALL_PORTS_NO_XG_NO_CPU_MAC(event)           \
    LOOP_0_23_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(event,event)

/* ports 0..23 , with no XG, no CPU  */
#define PREFIX_LOOP_ALL_PORTS_NO_XG_NO_CPU_MAC(event)           \
    PREFIX_LOOP_0_23_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(event,event)

/* ports 24..25 */
#define LOOP_XG_PORTS_MAC(event)           \
    LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(event,event),

/* ports 24..25 */
#define PREFIX_LOOP_XG_PORTS_MAC(event)           \
    PREFIX_LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(event,event)

/* ports 24..25 */
#define LOOP_XG_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)           \
    LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)

/* ports 24..25 */
#define PREFIX_LOOP_XG_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)           \
    PREFIX_LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)

/* ports 0..25 */
#define LOOP_ALL_PORTS_NO_CPU_MAC(event)           \
    LOOP_0_23_PORTS_MAC(event),             \
    LOOP_24_25_PORTS_MAC(event)

/* ports 0..25 */
#define PREFIX_LOOP_ALL_PORTS_NO_CPU_MAC(event)           \
    PREFIX_LOOP_0_23_27_PORTS_MAC(event),             \
    PREFIX_LOOP_24_25_PORTS_MAC(event)


/* ports 0..27 , CPU (63) */
#define LOOP_ALL_PORTS_MAC(event)           \
    LOOP_ALL_PORTS_NO_CPU_MAC(event),       \
    CPU_PORT_EV_ALL_MAC(event)

#define LOOP_ALL_PORTS_NO_CPU_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)\
    LOOP_0_23_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent),         \
    LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)

#define LOOP_ALL_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)\
    LOOP_0_23_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent),         \
    LOOP_24_25_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent),        \
    CPU_PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)


/* general events that are per queue */
#define QUEUE_EV_ALL_MAC(event,queue)       \
    {UNI_QUEUE_GEN_EV_MAC(event) ,        \
        QUEUE_EV_MAC(PM,event,queue),    \
        queue                           \
    }

#define QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,queue)       \
    {UNI_QUEUE_GEN_EV_MAC(uniEvent) ,        \
        QUEUE_EV_MAC(PM,pmEvent,queue),    \
        queue                           \
    }



    /* loop on all queues 0..7 */
#define LOOP_ALL_QUEUES_MAC(event)          \
    QUEUE_EV_ALL_MAC(event,0),              \
    QUEUE_EV_ALL_MAC(event,1),              \
    QUEUE_EV_ALL_MAC(event,2),              \
    QUEUE_EV_ALL_MAC(event,3),              \
    QUEUE_EV_ALL_MAC(event,4),              \
    QUEUE_EV_ALL_MAC(event,5),              \
    QUEUE_EV_ALL_MAC(event,6),              \
    QUEUE_EV_ALL_MAC(event,7)

    /* loop on all queues 0..7 */
#define LOOP_ALL_QUEUES_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)          \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,0),              \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,1),              \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,2),              \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,3),              \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,4),              \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,5),              \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,6),              \
    QUEUE_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,7)



/* general events that are per queue */
#define QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,queue)       \
    {UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME_MAC(event,queue) ,        \
        QUEUE_EV_MAC(PM,event,queue),    \
        queue                         \
    }

/* general events that are per queue */
#define QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,queue)       \
    {UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME_MAC(uniEvent,queue) ,        \
        QUEUE_EV_MAC(PM,pmEvent,queue),    \
        queue                         \
    }

#define QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME_MAC(event)    \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,0),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,1),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,2),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,3),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,4),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,5),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,6),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_MAC(event,7)

#define QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent)    \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,0),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,1),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,2),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,3),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,4),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,5),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,6),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent,pmEvent,7)

#define LOOP_256_PORTS_RATE_LIMIT_ONLY_DIFF_NAME_FOR_PP_MAC(uniEvent, pmEvent)  \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  0),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  1),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  2),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  3),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  4),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  5),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  6),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  7),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  8),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent,  9),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 10),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 11),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 12),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 13),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 14),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 15),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 16),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 17),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 18),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 19),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 20),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 21),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 22),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 23),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 24),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 25),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 26),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 27),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 28),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 29),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 30),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 31),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 32),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 33),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 34),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 35),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 36),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 37),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 38),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 39),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 40),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 41),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 42),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 43),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 44),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 45),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 46),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 47),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 48),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 49),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 50),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 51),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 52),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 53),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 54),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 55),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 56),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 57),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 58),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 59),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 60),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 61),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 62),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 63),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 64),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 65),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 66),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 67),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 68),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 69),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 70),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 71),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 72),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 73),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 74),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 75),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 76),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 77),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 78),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 79),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 80),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 81),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 82),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 83),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 84),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 85),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 86),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 87),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 88),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 89),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 90),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 91),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 92),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 93),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 94),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 95),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 96),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 97),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 98),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 99),                \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 100),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 101),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 102),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 103),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 104),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 105),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 106),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 107),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 108),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 109),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 110),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 111),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 112),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 113),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 114),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 115),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 116),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 117),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 118),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 119),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 120),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 121),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 122),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 123),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 124),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 125),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 126),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 127),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 128),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 129),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 130),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 131),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 132),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 133),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 134),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 135),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 136),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 137),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 138),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 139),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 140),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 141),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 142),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 143),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 144),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 145),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 146),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 147),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 148),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 149),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 150),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 151),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 152),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 153),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 154),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 155),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 156),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 157),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 158),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 159),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 160),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 161),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 162),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 163),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 164),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 165),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 166),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 167),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 168),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 169),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 170),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 171),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 172),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 173),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 174),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 175),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 176),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 177),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 178),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 179),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 180),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 181),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 182),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 183),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 184),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 185),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 186),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 187),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 188),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 189),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 190),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 191),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 192),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 193),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 194),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 195),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 196),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 197),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 198),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 199),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 200),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 201),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 202),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 203),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 204),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 205),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 206),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 207),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 208),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 209),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 210),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 211),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 212),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 213),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 214),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 215),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 216),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 217),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 218),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 219),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 220),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 221),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 222),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 223),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 224),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 225),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 226),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 227),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 228),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 229),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 230),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 231),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 232),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 233),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 234),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 235),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 236),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 237),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 238),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 239),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 240),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 241),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 242),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 243),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 244),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 245),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 246),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 247),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 248),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 249),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 250),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 251),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 252),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 253),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 254),               \
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(uniEvent, pmEvent, 255)

/* build general entry in DB , that support all the devices
   it uses index 0 in field EXT_DATA
   example: DIFF_REG_GEN_EV_ALL(BM,BM_ING0,EMPTY_CLEAR)
   target :
   CPSS_PP_BM_EMPTY_CLEAR_E ,
   PRV_CPSS_EXMXPM_BM_EGR0_EMPTY_CLEAR_E

   uniReg -- register name in the unified events
   pmReg  -- register name in the Puma

*/
#define DIFF_REG_GEN_EV_ALL_MAC(uniReg,pmReg,event)       \
    {UNI_GEN_EV_WITH_REG_NAME_MAC(uniReg,event) ,        \
        GEN_EV_WITH_REG_NAME_MAC(PM,pmReg,event),       \
        0                       \
    }


/* port 24 (XPC0) PCS on ch hx event all devices */
#define PORT_PCS_EV_ALL_24_MAC(event)    \
    {UNI_PORT_PCS_GEN_EV_MAC(event) ,    \
        PORT_XPC_PORT_24_EV_MAC(PM,event),  \
        24                               \
    }


/* port 25 (XPC0) PCS on ch hx event all devices */
#define PORT_PCS_EV_ALL_25_MAC(event)    \
    {UNI_PORT_PCS_GEN_EV_MAC(event) ,    \
        PORT_XPC_PORT_25_EV_MAC(PM,event),  \
        25                               \
    }


/* loop XPC ports ,
        ports 24,25
*/
#define XPC_LOOP_PORTS_MAC(event)       \
    PORT_PCS_EV_ALL_24_MAC(event),       \
    PORT_PCS_EV_ALL_25_MAC(event)

/*  event per port per lane */
#define LANE_PORT_MAC(event,port,lane)                  \
    {UNI_PORT_LANE_GEN_EV_MAC(event) ,                  \
        PORT_LANE_EV_MAC(PM,event,port,lane),           \
        LANE_PORT_TO_EXT_DATA_CONVERT_MAC(port,lane)    \
    }

/* loop on lanes 0..3 of port (should be 24,25) */
#define LANES_PORT_MAC(event,port) \
    LANE_PORT_MAC(event,port,0)    ,\
    LANE_PORT_MAC(event,port,1)    ,\
    LANE_PORT_MAC(event,port,2)    ,\
    LANE_PORT_MAC(event,port,3)

/* loop on XG port 24,25 , for lanes 0..3 */
#define LANE_LOOP_PORTS_MAC(event) \
    LANES_PORT_MAC(event,24)   ,\
    LANES_PORT_MAC(event,25)


/* HGL interface event (should be 0..3) */
#define HGL_INTERFACE_MAC(event,hgl) \
    {UNI_HGL_EV_MAC(event) ,         \
        HGL_EV_MAC(PM,event,hgl),    \
        hgl                          \
    }

/* loop on HGL interfaces : 0,1,2,3 */
#define LOOP_HGL_INTERFACES_MAC(event) \
    HGL_INTERFACE_MAC(event,0)   ,\
    HGL_INTERFACE_MAC(event,1)   ,\
    HGL_INTERFACE_MAC(event,2)   ,\
    HGL_INTERFACE_MAC(event,3)


/* loop on all lanes in all ports :
    currently supported for :
        Ch1 , ports 25,26 - 2 lanes (per port)
        Lion ports 0..11  - 6 lanes (per port)
*/
#define LANE_LOOP_PORTS(event)      \
    CH1_ONLY_LANES_PORTS(event),    \
    FROM_LION_LANE_LOOP_PORTS(event)

/* the conversion table */
static GT_U16 ppUni2IntCauseType[][TABLE_ENTRY_SIZE_E] = {

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_PARITY_ERROR, NARROW_SRAM4_PARITY_ERROR, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_ECC_ERROR, NARROW_SRAM4_ECC_ERROR, 0),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_PARITY_ERROR, NARROW_SRAM3_PARITY_ERROR, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_ECC_ERROR, NARROW_SRAM3_ECC_ERROR, 1),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_PARITY_ERROR, NARROW_SRAM2_PARITY_ERROR, 2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_ECC_ERROR, NARROW_SRAM2_ECC_ERROR, 2),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_PARITY_ERROR, NARROW_SRAM1_PARITY_ERROR, 3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_ECC_ERROR, NARROW_SRAM1_ECC_ERROR, 3),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_PARITY_ERROR, CSU36_PARITY_ERROR, 4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_ECC_ERROR, CSU36_ECC_ERROR, 4),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_PARITY_ERROR, DSU0_RX_PARITY_ERROR, 5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_MEMORY_PARITY_ERROR, DSU1_RX_PARITY_ERROR, 6),

    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MISC_TWSI_TIME_OUT,MISC_TWSI_TIMEOUT),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(BUF_MEM_MAC_ERROR, WRITE_DMA_AND_XRMON_XRMON),
    GEN_EV_ALL_MAC(MISC_TWSI_STATUS),
    GEN_EV_ALL_MAC(MISC_GENXS_READ_DMA_DONE),
    GEN_EV_ALL_MAC(MISC_PEX_ADDR_UNMAPPED),
    GEN_EV_ALL_MAC(MISC_ILLEGAL_ADDR),
    EV_INDEX_SPECIFIC_MAC(GPP,0),
    EV_INDEX_SPECIFIC_MAC(GPP,1),
    EV_INDEX_SPECIFIC_MAC(GPP,2),
    EV_INDEX_SPECIFIC_MAC(GPP,3),
    EV_INDEX_SPECIFIC_MAC(GPP,4),
    EV_INDEX_SPECIFIC_MAC(GPP,5),
    EV_INDEX_SPECIFIC_MAC(GPP,6),
    EV_INDEX_SPECIFIC_MAC(GPP,7),

    /*************/
    /* egress BM */
    /*************/
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(BM_EGRESS_AGED_PACKET,BM_EGR0_AGED_PACKET),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(BM_EGRESS_MAX_BUFF_REACHED,BM_EGR0_GLOBAL_BUFF_LIMIT_REACHED),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MISC, BM_EGR0_PORT_RX_FULL, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MISC, BM_EGR0_CLEAR_B4BIND, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MISC, BM_EGR0_TRIGGERED_AGING_DONE, 2),
    /********************/
    /* end of egress BM */
    /********************/

    /**************/
    /* ingress BM */
    /**************/
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(BM_AGED_PACKET,BM_ING0_AGED_PACKET),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MAX_BUFF_REACHED,BM_ING0_GLOBAL_BUFF_LIMIT_REACHED),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MISC, BM_ING0_PORT_RX_FULL, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MISC, BM_ING0_CLEAR_B4BIND, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MISC, BM_ING0_TRIGGERED_AGING_DONE, 2),

    LOOP_ALL_PORTS_NO_CPU_ONLY_DIFF_NAME_FOR_PP_MAC(BM_MAX_BUFF_REACHED,BM_ING1_BUFF_LIMIT_REACHED),
    /********************/
    /* end of ingress BM */
    /********************/

    PREFIX_LOOP_ALL_PORTS_NO_CPU_MAC(LINK_STATUS_CHANGED),  /*LINK_CHANGE*/
    PREFIX_LOOP_ALL_PORTS_NO_XG_NO_CPU_MAC(AN_COMPLETED),   /*PORT_AN_COMPLETED*/
    PREFIX_LOOP_ALL_PORTS_NO_CPU_MAC(ADDRESS_OUT_OF_RANGE), /*PORT_ADDRESS_OUT_OF_RANGE*/

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_ADDRESS_OUT_OF_RANGE,PORTS_GOP0_MIBS_ADDR_OUT_OF_RANGE, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_ADDRESS_OUT_OF_RANGE,PORTS_GOP1_MIBS_ADDR_OUT_OF_RANGE, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_ADDRESS_OUT_OF_RANGE,PORTS_GOP2_MIBS_ADDR_OUT_OF_RANGE, 2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_ADDRESS_OUT_OF_RANGE,PORTS_GOP3_AND_XG_MIBS_ADDR_OUT_OF_RANGE, 3),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(PORT_NO_BUFF_PACKET_DROP, PORT24_NO_BUFFER_PKT_DROP, 24),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(PORT_NO_BUFF_PACKET_DROP, PORT25_NO_BUFFER_PKT_DROP, 25),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_EXPIRED,PORTS_GOP0_MIBS_COUNT_EXPIRED, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_EXPIRED,PORTS_GOP1_MIBS_COUNT_EXPIRED, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_EXPIRED,PORTS_GOP2_MIBS_COUNT_EXPIRED, 2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_EXPIRED,PORTS_GOP3_AND_XG_MIBS_COUNT_EXPIRED, 3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_EXPIRED,PORTS_GOP0_MIBS_XG_COUNT_EXPIRED, 4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_EXPIRED,PORTS_GOP2_MIBS_XG_COUNT_EXPIRED, 5),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_COPY_DONE,PORTS_GOP0_MIBS_COUNT_COPY_DONE, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_COPY_DONE,PORTS_GOP1_MIBS_COUNT_COPY_DONE, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_COPY_DONE,PORTS_GOP2_MIBS_COUNT_COPY_DONE, 2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_COPY_DONE,PORTS_GOP3_AND_XG_MIBS_COUNT_COPY_DONE, 3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_COPY_DONE,PORTS_GOP0_MIBS_XG_COUNT_COPY_DONE, 4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_COUNT_COPY_DONE,PORTS_GOP2_MIBS_XG_COUNT_COPY_DONE, 5),

    EV_ONLY_DIFF_NAME_FOR_PP_MAC(XSMI_WRITE_DONE,PORTS_GOP0_MIBS_XSMII_WRITE),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_GIG_BAD_FC_PACKET_GOOD_CRC,PORTS_GOP0_MIBS_GIG_BAD_FC_PKT_GOOD_CRC_EV, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_GIG_BAD_FC_PACKET_GOOD_CRC,PORTS_GOP1_MIBS_GIG_BAD_FC_PKT_GOOD_CRC_EV, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_GIG_BAD_FC_PACKET_GOOD_CRC,PORTS_GOP2_MIBS_GIG_BAD_FC_PKT_GOOD_CRC_EV, 2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_GIG_BAD_FC_PACKET_GOOD_CRC,PORTS_GOP3_AND_XG_MIBS_GIG_BAD_FC_PKT_GOOD_CRC_EV, 3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_XG_BAD_FC_PACKET_GOOD_CRC,PORTS_GOP0_MIBS_XG_BAD_FC_PKT_GOOD_CRC_EV, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_XG_BAD_FC_PACKET_GOOD_CRC,PORTS_GOP2_MIBS_XG_BAD_FC_PKT_GOOD_CRC_EV, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(GOP_XG_BAD_FC_PACKET_GOOD_CRC,PORTS_GOP3_AND_XG_MIBS_GIG_BAD_FC_PKT_GOOD_CRC_EV, 2),

    PREFIX_LOOP_ALL_PORTS_NO_XG_NO_CPU_MAC(SYNC_STATUS_CHANGED),/*PORT_SYNC_STATUS_CHANGED*/
    PREFIX_LOOP_ALL_PORTS_NO_XG_NO_CPU_MAC(PRBS_ERROR),/*PORT_PRBS_ERROR*/

    PREFIX_LOOP_XG_PORTS_MAC(FC_STATUS_CHANGED),/*PORT_FC_STATUS_CHANGED*/
    PREFIX_LOOP_XG_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(ILLEGAL_SEQUENCE,UNKNOWN_SEQUENCE),/*PORT_ILLEGAL_SEQUENCE*/
    PREFIX_LOOP_XG_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(FAULT_TYPE_CHANGE,FAULT_CHANGED),/*PORT_FAULT_TYPE_CHANGE*/

    LOOP_ALL_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_TXQ2_FLUSH,TXQ_FLUSH_DONE),/*TQ_TXQ2_FLUSH_PORT*/
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_TXQ2_FLUSH,TXQ_FLUSH_DONE,26),/*TQ_TXQ2_FLUSH_PORT26*/
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_TXQ2_FLUSH,TXQ_FLUSH_DONE,27),/*TQ_TXQ2_FLUSH_PORT26*/

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_MISC,TXQ_GENERAL_BAD_ADDR, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_MISC,TXQ_GENERAL_DESC_FULL, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_MISC,TXQ_GENERAL_BUFFER_FULL, 2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_MISC,TXQ_GENERAL_PARITY_ERROR_IN_TXQ_DESC_MEM, 3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_MISC,TXQ_GENERAL_PARITY_ERROR_IN_TXQ_PTR_MEM_FLL_RAM_PARITY, 4),

    LOOP_0_23_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_DESC_FULL,TXQ_FULL),
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_DESC_FULL,TXQ_FULL, 26),
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_DESC_FULL,TXQ_FULL, 27),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_GIGA_FIFO_FULL,TXQ_FULL_GIG_MC_FIFO_FULL),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_XG_MC_FIFO_FULL,TXQ_FULL_XG_MC_FIFO_FULL),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_MC_DESC_FULL,TXQ_FULL_MC_FIFO_DESC_DROP),
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_FULL_XG, TXQ_FULL_XG, 24),
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(TQ_FULL_XG, TXQ_FULL_XG, 25),

    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_MG_ADDR_OUT_OF_RANGE,BRIDGE_ADDRESS_OUT_OF_RANGE),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_SECURITY_BREACH_UPDATE,BRIDGE_UPDATE_SECURITY_BREACH_REGISTER),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_NUM_OF_HOP_EXP,FDB_NUM_OF_HOP_EXP),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_NA_LEARNED,FDB_NA_LEARNT),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_NA_NOT_LEARNED,FDB_NA_NOT_LEARNT),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_AGE_VIA_TRIGGER_ENDED,FDB_AGE_VIA_TRIGGER_ENDED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_TBL_READ_ECC_ERR,FDB_DATA_ERROR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_UPDATE_FROM_CPU_DONE,FDB_AU_PROC_COMPLETED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_NA_SELF_LEARNED,FDB_NA_SELF_LEARNT),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_NA_FROM_CPU_LEARNED,FDB_NA_FROM_CPU_LEARNED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_NA_FROM_CPU_DROPPED,FDB_NA_FROM_CPU_DROPPED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_AGED_OUT,FDB_AGED_OUT),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_FIFO_2_CPU_EXCEEDED,FDB_AU_FIFO_TO_CPU_IS_FULL),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_SA_MSG_DISCARDED,FDB_NA_DISCARDED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_QA_MSG_DISCARDED,FDB_QA_DISCARDED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(BM_VLT_ECC_ERR,FDB_VLT_DATA_ERROR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(BM_VLT_ECC_ERR,FDB_VLAN_ECC_ERROR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_VIDX_TBL_DATA_ERROR,FDB_VIDX_ECC_ERROR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_STG_TBL_DATA_ERROR,FDB_SPAN_PARITY_ERROR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_AUQ_PENDING,MISC_AUQ_PENDING),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_AUQ_FULL,MISC_AU_QUEUE_FULL),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_FUQ_PENDING,MISC_FUQ_PENDING),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EB_FUQ_FULL,MISC_FU_QUEUE_FULL),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_MG_ADDR_OUT_OF_RANGE,FDB_ADDRESS_OUT_OF_MEMORY),

    LOOP_0_61_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(MAC_SFLOW,INGRESS_STC),               /*MAC_SFLOW*/
    LOOP_0_61_PORTS_ONLY_DIFF_NAME_FOR_PP_MAC(EGRESS_SFLOW,EGRESS_STC),             /*MAC_SFLOW*/
    PORT_EV_ALL_ONLY_DIFF_NAME_FOR_PP_MAC(EGRESS_SFLOW,EGRESS_STC,62),

    LOOP_ALL_QUEUES_ONLY_DIFF_NAME_FOR_PP_MAC(TX_BUFFER,TRANSMIT_SDMA_TX_BUFFER),/*TX_BUFFER_QUEUE*/
    LOOP_ALL_QUEUES_ONLY_DIFF_NAME_FOR_PP_MAC(TX_ERR,TRANSMIT_SDMA_TX_ERROR),/*TX_ERR_QUEUE*/
    LOOP_ALL_QUEUES_ONLY_DIFF_NAME_FOR_PP_MAC(TX_END,TRANSMIT_SDMA_TX_END),/*TX_END_QUEUE*/
    QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(RX_BUFFER,RECEIVE_SDMA_RX_BUFFER),/*RX_BUFFER_QUEUE*/
    QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME_ONLY_DIFF_NAME_FOR_PP_MAC(RX_ERR,RECEIVE_SDMA_RX_ERROR),/*RX_ERR_QUEUE*/

    /************************** PEX *****************************************/
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_DL_DOWN_TX_ACC_ERR, PEX_DL_DWN_TX_ACC_ERR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_MASTER_DISABLED, PEX_MDIS),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_ERROR_WR_TO_REG, PEX_ERR_WR_TO_REG),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_HIT_DEFAULT_WIN_ERR, PEX_HIT_DFLT_WIN_ERR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_COR_ERROR_DET, PEX_COR_ERR_DET),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_NON_FATAL_ERROR_DET, PEX_NF_ERR_DET),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_FATAL_ERROR_DET, PEX_F_ERR_DET),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_DSTATE_CHANGED, PEX_DSTATE_CHANGED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_BIST, PEX_BIST),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_RCV_ERROR_FATAL, PEX_RCV_ERR_FATAL),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_RCV_ERROR_NON_FATAL, PEX_RCV_ERR_NON_FATAL),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_RCV_ERROR_COR, PEX_RCV_ERR_COR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_RCV_CRS, PEX_RCV_CRS),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_PEX_SLAVE_HOT_RESET, PEX_SLV_HOT_RESET),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_PEX_SLAVE_DISABLE_LINK, PEX_SLV_DIS_LINK),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_PEX_SLAVE_LOOPBACK, PEX_SLV_LB),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PEX_PEX_LINK_FAIL, PEX_LINK_FAIL),

    /********************** Policer *************************/
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(POLICER_ADDR_OUT_OF_MEMORY, INGRESS_POLICER_ADDRESS_OUT_OF_MEMORY, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(POLICER_DATA_ERR, INGRESS_POLICER_DATA_ERROR, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(POLICER_ADDR_OUT_OF_MEMORY, EGRESS_POLICER_ADDRESS_OUT_OF_MEMORY, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(POLICER_DATA_ERR, EGRESS_POLICER_DATA_ERROR, 1),

    /************************** TTI *****************************/
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(TTI_CPU_ADDRESS_OUT_OF_RANGE,      TTI_CPU_ADDRESS_OUT_OF_RANGE),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(TTI_ACCESS_DATA_ERROR,             TTI_TTI_TCAM_ACCESS_DATA_ERROR),

    /* Policy Engine Interrupt Cause: PCL0(LU0, LU1) and PCL1(LU0, LU1), Event index = 0,1,2,3*/
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(PCL_LOOKUP_DATA_ERROR, POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR_LU, 0),
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(PCL_LOOKUP_DATA_ERROR, POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR_LU, 1),
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(PCL_LOOKUP_DATA_ERROR, POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR_LU, 2),
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(PCL_LOOKUP_DATA_ERROR, POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR_LU, 3),
    EV_INDEX_ONLY_DIFF_NAME_SPECIFIC_MAC(PCL_LOOKUP_FIFO_FULL, POLICY_ENGINE_FIFO_FULL_LU, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(PCL_LOOKUP_FIFO_FULL, POLICY_ENGINE_FIFO_FULL_LU1, 2),/* 2 --> like in xcat(1 is used for lookup 01)*/

    /* TCAM */
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_INT_SUM, EXTERNAL_TCAM_EXTERNAL_SUM),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_IO_ERR, EXTERNAL_TCAM_IO_ERROR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_RX_PARITY_ERR, EXTERNAL_TCAM_RX_PARITY_ERR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_MG_CMD_EXE_DONE, EXTERNAL_TCAM_MG_CMD_EXE_DONE),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_NEW_DATA_IN_MAILBOX, EXTERNAL_TCAM_NEW_DATA_IN_MAILBOX),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_MG_WRONG_EXP_RES_LEN, EXTERNAL_TCAM_MG_LONG_EXP_RES_LEN),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_RSP_IDQ_EMPTY, EXTERNAL_TCAM_RSP_IDQ_EMPTY),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_PHASE_ERROR, EXTERNAL_TCAM_PHASE_ERR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(EXTERNAL_TCAM_TRAINING_SEQUENCE_DONE, EXTERNAL_TCAM_TRAIN_SEQ_DONE),

    /* XBAR */
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_RL,  XBAR_GCORE0_SUM_RX_FIFO_OVERRUN_CNTR_RL,     0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_RL, XBAR_GCORE0_SUM_INVALID_TRG_DROP_CNTR_RL,    0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_RL,  XBAR_GCORE0_SUM_SRC_FILTER_DROP_CNTR_RL,     0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_RL,          XBAR_GCORE0_SUM_RX_CELL_CNTR_RL,             0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL,       XBAR_GCORE0_SUM_P0_TX_DROP_CNTR_RL,          0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_RL,      XBAR_GCORE0_SUM_P1_TX_DROP_CNTR_RL,          0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_INC, XBAR_GCORE0_SUM_RX_FIFO_OVERRUN_CNTR_INC,    0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_INC,XBAR_GCORE0_SUM_INVALID_TRG_DROP_CNTR_INC,   0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_INC, XBAR_GCORE0_SUM_SRC_FILTER_DROP_CNTR_INC,    0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_INC,         XBAR_GCORE0_SUM_RX_CELL_CNTR_INC,            0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC,      XBAR_GCORE0_SUM_P0_TX_DROP_CNTR_INC,         0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_INC,     XBAR_GCORE0_SUM_P1_TX_DROP_CNTR_INC,         0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF,  XBAR_GCORE0_SUM_P0_TX_FC_CHANGED_TO_XOFF,     0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON,   XBAR_GCORE0_SUM_P0_TX_FC_CHANGED_TO_XON,      0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_ADDR,             XBAR_GCORE0_SUM_INVALID_ADDR,                0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_RL,          XBAR_GCORE0_SUM_TX_CELL_CNTR_RL,             0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_INC,         XBAR_GCORE0_SUM_TX_CELL_CNTR_INC,            0),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_RL,  XBAR_GCORE1_SUM_RX_FIFO_OVERRUN_CNTR_RL,     1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_RL, XBAR_GCORE1_SUM_INVALID_TRG_DROP_CNTR_RL,    1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_RL,  XBAR_GCORE1_SUM_SRC_FILTER_DROP_CNTR_RL,     1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_RL,          XBAR_GCORE1_SUM_RX_CELL_CNTR_RL,             1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL,       XBAR_GCORE1_SUM_P0_TX_DROP_CNTR_RL,          1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_RL,      XBAR_GCORE1_SUM_P1_TX_DROP_CNTR_RL,          1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_INC, XBAR_GCORE1_SUM_RX_FIFO_OVERRUN_CNTR_INC,    1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_INC,XBAR_GCORE1_SUM_INVALID_TRG_DROP_CNTR_INC,   1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_INC, XBAR_GCORE1_SUM_SRC_FILTER_DROP_CNTR_INC,    1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_INC,         XBAR_GCORE1_SUM_RX_CELL_CNTR_INC,            1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC,      XBAR_GCORE1_SUM_P0_TX_DROP_CNTR_INC,         1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_INC,     XBAR_GCORE1_SUM_P1_TX_DROP_CNTR_INC,         1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF, XBAR_GCORE1_SUM_P0_TX_FC_CHANGED_TO_XOFF,    1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON,  XBAR_GCORE1_SUM_P0_TX_FC_CHANGED_TO_XON,     1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_ADDR,             XBAR_GCORE1_SUM_INVALID_ADDR,                1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_RL,          XBAR_GCORE1_SUM_TX_CELL_CNTR_RL,             1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_INC,         XBAR_GCORE1_SUM_TX_CELL_CNTR_INC,            1),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_RL,  XBAR_GCORE2_SUM_RX_FIFO_OVERRUN_CNTR_RL,     2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_RL, XBAR_GCORE2_SUM_INVALID_TRG_DROP_CNTR_RL,    2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_RL,  XBAR_GCORE2_SUM_SRC_FILTER_DROP_CNTR_RL,     2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_RL,          XBAR_GCORE2_SUM_RX_CELL_CNTR_RL,             2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL,       XBAR_GCORE2_SUM_P0_TX_DROP_CNTR_RL,          2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_RL,      XBAR_GCORE2_SUM_P1_TX_DROP_CNTR_RL,          2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_INC, XBAR_GCORE2_SUM_RX_FIFO_OVERRUN_CNTR_INC,    2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_INC,XBAR_GCORE2_SUM_INVALID_TRG_DROP_CNTR_INC,   2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_INC, XBAR_GCORE2_SUM_SRC_FILTER_DROP_CNTR_INC,    2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_INC,         XBAR_GCORE2_SUM_RX_CELL_CNTR_INC,            2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC,      XBAR_GCORE2_SUM_P0_TX_DROP_CNTR_INC,         2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_INC,     XBAR_GCORE2_SUM_P1_TX_DROP_CNTR_INC,         2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF, XBAR_GCORE2_SUM_P0_TX_FC_CHANGED_TO_XOFF,    2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON,  XBAR_GCORE2_SUM_P0_TX_FC_CHANGED_TO_XON,     2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_ADDR,             XBAR_GCORE2_SUM_INVALID_ADDR,                2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_RL,          XBAR_GCORE2_SUM_TX_CELL_CNTR_RL,             2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_INC,         XBAR_GCORE2_SUM_TX_CELL_CNTR_INC,            2),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_RL,  XBAR_GCORE3_SUM_RX_FIFO_OVERRUN_CNTR_RL,     3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_RL, XBAR_GCORE3_SUM_INVALID_TRG_DROP_CNTR_RL,    3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_RL,  XBAR_GCORE3_SUM_SRC_FILTER_DROP_CNTR_RL,     3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_RL,          XBAR_GCORE3_SUM_RX_CELL_CNTR_RL,             3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL,       XBAR_GCORE3_SUM_P0_TX_DROP_CNTR_RL,          3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_RL,      XBAR_GCORE3_SUM_P1_TX_DROP_CNTR_RL,          3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_INC, XBAR_GCORE3_SUM_RX_FIFO_OVERRUN_CNTR_INC,    3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_INC,XBAR_GCORE3_SUM_INVALID_TRG_DROP_CNTR_INC,   3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_INC, XBAR_GCORE3_SUM_SRC_FILTER_DROP_CNTR_INC,    3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_INC,         XBAR_GCORE3_SUM_RX_CELL_CNTR_INC,            3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC,      XBAR_GCORE3_SUM_P0_TX_DROP_CNTR_INC,         3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_INC,     XBAR_GCORE3_SUM_P1_TX_DROP_CNTR_INC,         3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF, XBAR_GCORE3_SUM_P0_TX_FC_CHANGED_TO_XOFF,    3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON,  XBAR_GCORE3_SUM_P0_TX_FC_CHANGED_TO_XON,     3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_ADDR,             XBAR_GCORE3_SUM_INVALID_ADDR,                3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_RL,          XBAR_GCORE3_SUM_TX_CELL_CNTR_RL,             3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_INC,         XBAR_GCORE3_SUM_TX_CELL_CNTR_INC,            3),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_RL,  XBAR_GCORE4_SUM_RX_FIFO_OVERRUN_CNTR_RL,     4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_RL, XBAR_GCORE4_SUM_INVALID_TRG_DROP_CNTR_RL,    4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_RL,  XBAR_GCORE4_SUM_SRC_FILTER_DROP_CNTR_RL,     4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_RL,          XBAR_GCORE4_SUM_RX_CELL_CNTR_RL,             4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL,       XBAR_GCORE4_SUM_P0_TX_DROP_CNTR_RL,          4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_RL,      XBAR_GCORE4_SUM_P1_TX_DROP_CNTR_RL,          4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_INC, XBAR_GCORE4_SUM_RX_FIFO_OVERRUN_CNTR_INC,    4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_INC,XBAR_GCORE4_SUM_INVALID_TRG_DROP_CNTR_INC,   4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_INC, XBAR_GCORE4_SUM_SRC_FILTER_DROP_CNTR_INC,    4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_INC,         XBAR_GCORE4_SUM_RX_CELL_CNTR_INC,            4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC,      XBAR_GCORE4_SUM_P0_TX_DROP_CNTR_INC,         4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_INC,     XBAR_GCORE4_SUM_P1_TX_DROP_CNTR_INC,         4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF,  XBAR_GCORE4_SUM_P0_TX_FC_CHANGED_TO_XOFF,     4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON,   XBAR_GCORE4_SUM_P0_TX_FC_CHANGED_TO_XON,      4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_ADDR,             XBAR_GCORE4_SUM_INVALID_ADDR,                4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_RL,          XBAR_GCORE4_SUM_TX_CELL_CNTR_RL,             4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_INC,         XBAR_GCORE4_SUM_TX_CELL_CNTR_INC,            4),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_RL,  XBAR_GCORE5_SUM_RX_FIFO_OVERRUN_CNTR_RL,     5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_RL, XBAR_GCORE5_SUM_INVALID_TRG_DROP_CNTR_RL,    5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_RL,  XBAR_GCORE5_SUM_SRC_FILTER_DROP_CNTR_RL,     5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_RL,          XBAR_GCORE5_SUM_RX_CELL_CNTR_RL,             5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_RL,       XBAR_GCORE5_SUM_P0_TX_DROP_CNTR_RL,          5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_RL,      XBAR_GCORE5_SUM_P1_TX_DROP_CNTR_RL,          5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_FIFO_OVERRUN_CNTR_INC, XBAR_GCORE5_SUM_RX_FIFO_OVERRUN_CNTR_INC,    5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_TRG_DROP_CNTR_INC,XBAR_GCORE5_SUM_INVALID_TRG_DROP_CNTR_INC,   5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SRC_FILTER_DROP_CNTR_INC, XBAR_GCORE5_SUM_SRC_FILTER_DROP_CNTR_INC,    5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_RX_CELL_CNTR_INC,         XBAR_GCORE5_SUM_RX_CELL_CNTR_INC,            5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_DROP_CNTR_INC,      XBAR_GCORE5_SUM_P0_TX_DROP_CNTR_INC,         5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_SHP_TX_DROP_CNTR_INC,     XBAR_GCORE5_SUM_P1_TX_DROP_CNTR_INC,         5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XOFF, XBAR_GCORE5_SUM_P0_TX_FC_CHANGED_TO_XOFF,    5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_PRIORITY_0_3_TX_FC_CHANGED_TO_XON,  XBAR_GCORE5_SUM_P0_TX_FC_CHANGED_TO_XON,     5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_INVALID_ADDR,             XBAR_GCORE5_SUM_INVALID_ADDR,                5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_RL,          XBAR_GCORE5_SUM_TX_CELL_CNTR_RL,             5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(XBAR_TX_CELL_CNTR_INC,         XBAR_GCORE5_SUM_TX_CELL_CNTR_INC,            5),

    /******************************* STC **************************************/
    LOOP_256_PORTS_RATE_LIMIT_ONLY_DIFF_NAME_FOR_PP_MAC(SCT_RATE_LIMITER, RATE_LIMIT_EXPIRED),

    /***************** PP Fabric Reassembly engine events *********************/
    GEN_EV_ALL_MAC(CRX_MAIL_ARRIVED),
    GEN_EV_ALL_MAC(CRX_READ_EMPTY_CPU),
    GEN_EV_ALL_MAC(CRX_DATA_DROP),
    GEN_EV_ALL_MAC(CRX_CPU_DROP),
    GEN_EV_ALL_MAC(CRX_AGED_OUT_CONTEXT),
    GEN_EV_ALL_MAC(CRX_CRC_DROP),
    GEN_EV_ALL_MAC(CRX_PACKET_LENGTH_DROP),
    GEN_EV_ALL_MAC(CRX_MAX_BUFFERS_DROP),
    GEN_EV_ALL_MAC(CRX_BAD_CONTEXT_DROP),
    GEN_EV_ALL_MAC(CRX_NO_BUFFERS_DROP),
    GEN_EV_ALL_MAC(CRX_MULTICAST_CONGESTION_DROP),
    GEN_EV_ALL_MAC(CRX_DATA_FIFO_UNDERRUN),
    GEN_EV_ALL_MAC(CRX_DATA_FIFO_OVERRUN),
    GEN_EV_ALL_MAC(CRX_CPU_UNDERRUN),
    GEN_EV_ALL_MAC(CRX_CPU_OVERRUN),
    GEN_EV_ALL_MAC(CRX_E2E_FIFO_UNDERRUN),
    GEN_EV_ALL_MAC(CRX_E2E_FIFO_OVERRUN),
    GEN_EV_ALL_MAC(CRX_UNKNOWN_CELL_TYPE),
    GEN_EV_ALL_MAC(CRX_COUNTER_BAD_ADDR),
    GEN_EV_ALL_MAC(CRX_PSM_BAD_ADDR),

    /***************** PP Fabric Segmentation engine events ********************/
    GEN_EV_ALL_MAC(TXD_SEG_FIFO_PARITY_ERR),
    GEN_EV_ALL_MAC(TXD_CPU_MAIL_SENT),
    GEN_EV_ALL_MAC(TXD_SEG_FIFO_OVERRUN),
    GEN_EV_ALL_MAC(TXD_WRONG_CH_TO_FABRIC_DIST),

    /************************** Secondary Global *********************************/
    GEN_EV_ALL_MAC(EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_LOW),
    GEN_EV_ALL_MAC(EXTERNAL_MEMORY_RLDRAM_UNIT_PARITY_ERR_HIGH),
    GEN_EV_ALL_MAC(EXTERNAL_MEMORY_RLDRAM_UNIT_MG_CMD_DONE),
    GEN_EV_ALL_MAC(IPVX_DATA_ERROR),
    GEN_EV_ALL_MAC(IPVX_ADDRESS_OUT_OF_MEMORY),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM0_DATA_ERROR, 0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM1_DATA_ERROR, 1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM2_DATA_ERROR, 2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM3_DATA_ERROR, 3),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM4_DATA_ERROR, 4),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM5_DATA_ERROR, 5),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM6_DATA_ERROR, 6),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(IPVX_LPM_DATA_ERROR, IPVX_LPM7_DATA_ERROR, 7),

    /* per XG port - XPCS events (port 24,25) */
    XPC_LOOP_PORTS_MAC(DESKEW_TIMEOUT),
    XPC_LOOP_PORTS_MAC(DETECTED_COLUMN_IIAII),
    XPC_LOOP_PORTS_MAC(DESKEW_ERROR),
    XPC_LOOP_PORTS_MAC(PPM_FIFO_UNDERRUN),
    XPC_LOOP_PORTS_MAC(PPM_FIFO_OVERRUN),

    /* Per XG port - per lane events (ports 24,25 , lanes 0..3)*/
    LANE_LOOP_PORTS_MAC(PRBS_ERROR),
    LANE_LOOP_PORTS_MAC(DISPARITY_ERROR),
    LANE_LOOP_PORTS_MAC(SYMBOL_ERROR),
    LANE_LOOP_PORTS_MAC(CJR_PAT_ERROR),
    LANE_LOOP_PORTS_MAC(SIGNAL_DETECT_CHANGED),
    LANE_LOOP_PORTS_MAC(DETECTED_IIAII),

    /* per HGL event (HGL 0,1,2,3) */
    LOOP_HGL_INTERFACES_MAC(PING_RECEIVED),
    LOOP_HGL_INTERFACES_MAC(PING_SENT),
    LOOP_HGL_INTERFACES_MAC(MAC_TX_OVERRUN),
    LOOP_HGL_INTERFACES_MAC(MAC_TX_UNDERRUN),
    LOOP_HGL_INTERFACES_MAC(RX_ERR_CNTR_RL),
    LOOP_HGL_INTERFACES_MAC(BAD_PCS_TO_MAC_REFORMAT_RX),
    LOOP_HGL_INTERFACES_MAC(BAD_HEAD_CELL_RX),
    LOOP_HGL_INTERFACES_MAC(BAD_LENGTH_CELL_RX),
    LOOP_HGL_INTERFACES_MAC(FC_CELL_RX_INT),
    LOOP_HGL_INTERFACES_MAC(FC_CELL_TX_INT),
    LOOP_HGL_INTERFACES_MAC(ADDR_OUT_OF_RANGE),
    LOOP_HGL_INTERFACES_MAC(FC_XOFF_DEAD_LOCK_TC0),
    LOOP_HGL_INTERFACES_MAC(FC_XOFF_DEAD_LOCK_TC1),
    LOOP_HGL_INTERFACES_MAC(FC_XOFF_DEAD_LOCK_TC2),
    LOOP_HGL_INTERFACES_MAC(FC_XOFF_DEAD_LOCK_TC3),

    /* TCC interrupt */
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PCL_TCC_ECC_ERR,                   TCC_TCAM_ERROR_DETECTED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PCL_ACTION_ERROR_DETECTED,         TCC_ACTION_ERROR_DETECTED),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PCL_MG_LOOKUP_RESULTS_READY,       TCC_MG_LOOKUP_RESULTS_READY),

    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PCL_INLIF_TABLE_DATA_ERROR, POLICY_ENGINE_INLIF_TABLE_ECC_ERROR),
    EV_ONLY_DIFF_NAME_FOR_PP_MAC(PCL_CONFIG_TABLE_DATA_ERROR, POLICY_ENGINE_PCL_CONF_TABLE_PARITY_ERROR),

    /* PCS Tx/Rx EQ interrupts PCS Egress EQ, DQ and Ingress DQ. Corresponded events index = 0, 1, 2 */
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_FULL,            PCS_EGR_EQ_TX_SYNC_FIFO_FULL,           0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_OVERRUN,         PCS_EGR_EQ_TX_SYNC_FIFO_OVERRUN,        0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_UNDERRUN,        PCS_EGR_EQ_TX_SYNC_FIFO_UNDERRUN,       0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_FULL,            PCS_EGR_EQ_RX_SYNC_FIFO_FULL,           0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_OVERRUN,         PCS_EGR_EQ_RX_SYNC_FIFO_OVERRUN,        0),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_UNDERRUN,        PCS_EGR_EQ_RX_SYNC_FIFO_UNDERRUN,       0),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_FULL,            PCS_EGR_DQ_TX_SYNC_FIFO_FULL,           1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_OVERRUN,         PCS_EGR_DQ_TX_SYNC_FIFO_OVERRUN,        1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_UNDERRUN,        PCS_EGR_DQ_TX_SYNC_FIFO_UNDERRUN,       1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_FULL,            PCS_EGR_DQ_RX_SYNC_FIFO_FULL,           1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_OVERRUN,         PCS_EGR_DQ_RX_SYNC_FIFO_OVERRUN,        1),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_UNDERRUN,        PCS_EGR_DQ_RX_SYNC_FIFO_UNDERRUN,       1),

    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_FULL,            PCS_INGR_DQ_TX_SYNC_FIFO_FULL,          2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_OVERRUN,         PCS_INGR_DQ_TX_SYNC_FIFO_OVERRUN,       2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_TX_SYNC_FIFO_UNDERRUN,        PCS_INGR_DQ_TX_SYNC_FIFO_UNDERRUN,      2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_FULL,            PCS_INGR_DQ_RX_SYNC_FIFO_FULL,          2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_OVERRUN,         PCS_INGR_DQ_RX_SYNC_FIFO_OVERRUN,       2),
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP_MAC(STAT_INF_RX_SYNC_FIFO_UNDERRUN,        PCS_INGR_DQ_RX_SYNC_FIFO_UNDERRUN,      2),

    /* DIT Events */
    GEN_EV_ALL_MAC(DIT_MC_DIT_TTL_TRHOLD_EXCEED),
    GEN_EV_ALL_MAC(DIT_UC_DIT_TTL_TRHOLD_EXCEED),
    GEN_EV_ALL_MAC(DIT_DATA_ERROR),
    GEN_EV_ALL_MAC(DIT_MC0_PARITY_ERROR),
    GEN_EV_ALL_MAC(DIT_INT_TABLE_ECC_ERROR_CNTR),
    GEN_EV_ALL_MAC(DIT_MC0_IP_TV_PARITY_ERROR_CNTR),
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
    UNI_EV_2_EV_OFFSET  baseDevType;/* base EV type for this device family */

    if(devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS ||
       prvCpssDrvPpConfig[devNum] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (prvCpssDrvPpConfig[devNum]->devFamily)
    {
        case CPSS_PP_FAMILY_PUMA_E:
            baseDevType = PUMA_E;
            break;
        default:
            DBG_INFO(("file:%s line:%d intType:%d\n", __FILE__, __LINE__,
                      intCause->intType));
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *ppColumnPtr = baseDevType;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvPpIntPumaIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. -- Puma devices
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
GT_STATUS prvCpssDrvPpIntPumaIntCauseToUniEvConvert
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

    if (prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        rc = prvCpssDrvIntCauseToUniEvConvertDedicatedTables(
            puma3UniEvMapTable, puma3UniEvMapTableSize,
            puma3UniEvMapTableWithExtData, puma3UniEvMapTableWithExtDataSize,
            portGroupId, intCauseIndex, uniEvPtr, extDataPtr);
        if (rc != GT_OK)
        {
            DBG_INFO(("file:%s line:%d event not found intCause = %d\n", __FILE__,
                      __LINE__, intCauseIndex));
        }
        return rc;
    }

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


/*******************************************************************************
* prvCpssDrvPpIntPumaUniEvToIntCauseConvert
*
* DESCRIPTION:
*       Converts unified event type and extra data to Interrupt Cause event
*       -- ExMxPm devices
*
* INPUTS:
*       devNum      - The device number.
*       uniEv       - The unified event type to convert.
*       extData     - The event extended data to convert.
*
* OUTPUTS:
*       intCauseIndexPtr - (pointer to)The interrupt cause.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_NOT_FOUND - the uniEv and extData to convert was not found
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvPpIntPumaUniEvToIntCauseConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      uniEv,
    IN  GT_U32                      extData,
    OUT GT_U32                      *intCauseIndexPtr
)
{
    GT_STATUS       rc;
    GT_U32          ii;          /* iterator                     */
    GT_U32          ppColumn;   /* conversion table pp column   */

    CPSS_NULL_PTR_CHECK_MAC(intCauseIndexPtr);

    if (prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_PUMA3_E)
    {
        return prvCpssDrvIntUniEvToCauseConvertDedicatedTables(
            puma3UniEvMapTable, puma3UniEvMapTableSize,
            puma3UniEvMapTableWithExtData, puma3UniEvMapTableWithExtDataSize,
            2 /*portGroupsAmount*/, uniEv, extData, intCauseIndexPtr);
    }

    *intCauseIndexPtr   = 0;

    rc = ppColumnGet(devNum, &ppColumn);
    if(rc != GT_OK)
    {
        return rc;
    }

    for (ii = 0; ii < NUM_OF_TBL_ENTRIES ; ii++)
    {
        if (uniEv   == ppUni2IntCauseType[ii][UNI_EV_E] &&
            extData == ppUni2IntCauseType[ii][EXT_DATA_E])
        {
            *intCauseIndexPtr = ppUni2IntCauseType[ii][ppColumn];
            return GT_OK;
        }
    }

    DBG_INFO(("file:%s line:%d event not found uniEv,extData ={%d,%d}\n", __FILE__,
              __LINE__, uniEv,extData));

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}

#ifdef ASIC_SIMULATION
    #define PRV_CPSS_DRV_EXMXPM_PRINT_EVENTS
#endif /*ASIC_SIMULATION*/

#ifdef PRV_CPSS_DRV_EXMXPM_PRINT_EVENTS
/* code logic copied from prvCpssDrvDxChPrintEvents(...) */
GT_STATUS prvCpssDrvExMxPmPrintEvents
(
    GT_U32  deviceFamily/* 0 - all , 1 - pm2 , 2 - pm3 */
)
{
    GT_U32 ii;
    GT_U32 jj,jjStart,jjEnd;
    GT_U32  uniEvent;
    GT_U32  ppEvent;
    char * uniEvName[CPSS_UNI_EVENT_COUNT_E] = {UNI_EV_NAME};
    char *eventStr;
    GT_U32 uniEvLen;
    GT_U32 exteraData=0;
    GT_U32 lastUniEvent=0xffffffff;
    char* familyNames[] = {"empty" , "PM2" , "PM3" , "???" , "???", "???" , "???"};

    uniEvLen = NUM_OF_TBL_ENTRIES;

    cpssOsPrintf("Unify_event:");

    if(deviceFamily >= EXT_DATA_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(deviceFamily == 0)
    {
        jjStart = UNI_EV_E + 1;
        jjEnd = EXT_DATA_E;
    }
    else
    {
        jjStart = deviceFamily;
        jjEnd = jjStart + 1;
    }

    for(jj = jjStart ; jj < jjEnd; jj++)
    {
        cpssOsPrintf("    %s",familyNames[jj]);
    }

    cpssOsPrintf("    extraData \n");

    for (ii=0; ii < uniEvLen; ii++)
    {
        uniEvent = ppUni2IntCauseType[ii][UNI_EV_E];
        exteraData = ppUni2IntCauseType[ii][EXT_DATA_E];
        eventStr = uniEvName[uniEvent];

        for(jj = jjStart ; jj < jjEnd; jj++)
        {
            ppEvent = ppUni2IntCauseType[ii][jj];

            if(ppEvent != CPSS_UNI_RSRVD_EVENT_E)
            {
                break;
            }
        }

        if(jj == jjEnd)
        {
            continue;
        }

        if(uniEvent != lastUniEvent)
        {
            cpssOsPrintf("\n");
            cpssOsPrintf("%s", eventStr);

            for(jj = jjStart ; jj < jjEnd; jj++)
            {
                ppEvent = ppUni2IntCauseType[ii][jj];

                if(ppEvent == CPSS_UNI_RSRVD_EVENT_E)
                {
                    /* run till the end of the table to check if the unified event supported for this [jj] */
                    GT_U32  kk;
                    for(kk = ii + 1 ; kk < uniEvLen ; kk ++)
                    {
                        if(uniEvent != ppUni2IntCauseType[kk][UNI_EV_E])
                        {
                            /* assume the same unified events are consecutive */
                            break;
                        }

                        ppEvent = ppUni2IntCauseType[kk][jj];

                        if(ppEvent == CPSS_UNI_RSRVD_EVENT_E)
                        {
                            continue;
                        }

                        /* found that device supported */
                        cpssOsPrintf("    %d",ppEvent);
                        break;
                    }

                    if(kk == uniEvLen || uniEvent != ppUni2IntCauseType[kk][UNI_EV_E])
                    {
                        cpssOsPrintf(" ------ ");
                    }

                    continue;
                }

                cpssOsPrintf("    %d",ppEvent);
            }
        }

        lastUniEvent = uniEvent;


        cpssOsPrintf("    %d ",exteraData);/* no need for "\n" in here , so we can see all exteraData in single line */

    }

    cpssOsPrintf("\n");

    return GT_OK;
}
#endif /*PRV_CPSS_DRV_EXMXPM_PRINT_EVENTS*/



