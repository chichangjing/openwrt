/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntExMxEventsUnify.c
*
* DESCRIPTION:
*       Conversion routines for unified events -- ExMx devices.
*
* FILE REVISION NUMBER:
*       $Revision: 3$
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsTiger.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsTwistC.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsTwistD.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsSamba.h>
/* get the unify events definitions */
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>


/*******************************************************************************
* internal definitions
*******************************************************************************/
/* #define EXMX_UNI_CNVRT_DEBUG */

#ifdef EXMX_UNI_CNVRT_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

/*
 * Typedef: enum UNI_EV_2_EV_OFFSET
 *
 * Enumeration: Lists the conversion table column data types.
 *
 * Enums:
 *      UNI_EV              - Unified event
 *      TWIST_12            - twist 12 port event
 *      TWIST_52            - twist 52 port event
 *      TWIST_D_12          - twistD 12 port event
 *      TWIST_D_52          - twistD 52 port event
 *      TWIST_D_XG          - twistD XG port event
 *      SAMBA_12            - Samba 12 port event
 *      SAMBA_52            - Samba 52 port event
 *      SAMBA_XG            - Samba XG port event
 *      TIGER_12            - Tiger 12 port event
 *      TIGER_52            - Tiger 52 port event
 *      TIGER_XG            - Tiger XG port event
 *      EXT_DATA            - Additional data for unified event
 *      TABLE_ENTRY_SIZE    - Table size (MUST be last !!)
 *
 */
typedef enum
{
    UNI_EV,

    TWIST_12,
    TWIST_52,

    TWIST_D_12,
    TWIST_D_52,
    TWIST_D_XG,

    SAMBA_12,
    SAMBA_52,
    SAMBA_XG,

    TIGER_12,
    TIGER_52,
    TIGER_XG,

    /*
     add here the new devices in the next format:
     xxx_12,
     xxx_52,
     xxx_XG
    */


    EXT_DATA,

    TABLE_ENTRY_SIZE
} UNI_EV_2_EV_OFFSET;

/* enumeration to convert name from the API to application to be more
  "consistent" with the naming conventions of this file */
enum{
    CPSS_PP_PORT_BM_MAX_BUFF_REACHED_E    = CPSS_PP_BM_MAX_BUFF_REACHED_E,
    CPSS_PP_PORT_TQ_WATCHDOG_EX_E         = CPSS_PP_TQ_WATCHDOG_EX_PORT_E,
    CPSS_PP_PORT_TQ_TXQ2_FLUSH_E          = CPSS_PP_TQ_TXQ2_FLUSH_PORT_E,
    CPSS_PP_PORT_TQ_RED_REACHED_E         = CPSS_PP_TQ_RED_REACHED_PORT_E,
    CPSS_PP_TX_END_QUEUE_E                = CPSS_PP_TX_END_E,
    CPSS_PP_PORT_TXQ_EXRMON_SHADOW_REG_EMPTY_E = CPSS_PP_EGRESS_SFLOW_E,
    CPSS_PP_PORT_EPF_EXRMON_SHADOW_REG_EMPTY_E = CPSS_PP_MAC_SFLOW_E,
    CPSS_PP_PORT_EPF_EXRMON_TOTAL_SAMP_FULL_E = CPSS_PP_INGRESS_SFLOW_SAMPLED_CNTR_E,
    CPSS_PP_PORT_TXQ_EXRMON_TOTAL_SAMP_FULL_E = CPSS_PP_EGRESS_SFLOW_SAMPLED_CNTR_E
};

/* devType :
one of

    TC_12, twist-c
    TC_52,
    TD_12, twist-d
    TD_52,
    TD_XG,
    SB_12, samba
    SB_52,
    SB_XG,
    TG_12, tiger
    TG_52,
    TG_XG,

event , on of the unify events names
*/
/* build the name of the device type :
    example : DEV_TYPE(TG,12)
    target: PRV_CPSS_TG_12_
*/
#define DEV_TYPE(dev,type)  PRV_CPSS_##dev##_##type##_

/* build the name of the event :
    example : GEN_EV(TG,12,TQ_MC_FIFO_OVERRUN)
    target: PRV_CPSS_TG_12_TQ_MC_FIFO_OVERRUN_E
*/
#define GEN_EV(dev,type,event)          PRV_CPSS_##dev##_##type##_##event##_E

/* build name of group of events: per port , per queue , per index
    example : GROUP_EV(TG,12,IPG_TOO_SMALL,PORT,3)
    target: PRV_CPSS_TG_12_IPG_TOO_SMALL_PORT3
*/
#define GROUP_EV(dev,type,event,group,index)   PRV_CPSS_##dev##_##type##_##event##_##group##index##_E

/* build name group - per port
    example : PORT_EV(TG,12,IPG_TOO_SMALL,3)
    target: PRV_CPSS_TG_12_IPG_TOO_SMALL_PORT3
*/
#define PORT_EV(dev,type,event,port)     GROUP_EV(dev,type,event,PORT,port)

/* build name group - per index
    example : INDEX_EV(TG,12,GPP_INTERRUPT,2)
    target: PRV_CPSS_TG_12_GPP_INTERRUPT_INDEX2
*/
#define INDEX_EV(dev,type,event,index)    PRV_CPSS_##dev##_##type##_##event##_##index##_E

/* build name group - per index
    example : INTERRUPT_EV(TG,12,GPP,2)
    target: PRV_CPSS_TG_12_GPP_INTERRUPT2
*/
#define INTERRUPT_EV(dev,type,event,index)    GROUP_EV(dev,type,event,INTERRUPT,index)

/* build name group - per queue
    example : QUEUE_EV(TG,12,TX_BUFFER,5)
    target: PRV_CPSS_TG_12_TX_BUFFER_QUEUE5
*/
#define QUEUE_EV(dev,type,event,queue)    GROUP_EV(dev,type,event,QUEUE,queue)

/* the name of the unify event - based on the event name */
#define UNI_GEN_EV(event)          CPSS_PP_##event##_E

#define UNI_PORT_GEN_EV(event)     CPSS_PP_PORT_##event##_E

#define UNI_QUEUE_GEN_EV(event)    CPSS_PP_##event##_QUEUE_E

/* the name of the unify event , but specific per queue -
   based on the event name , and queue number
   example : UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(RX_BUFFER,6)
   target : CPSS_RX_BUFFER_QUEUE6
*/
#define UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(event,queue) CPSS_PP_##event##_QUEUE##queue##_E

/* build general entry in DB , that support all the devices
   it uses index 0 in field EXT_DATA
   example: GEN_EV_ALL(SLV_WRITE_ERR)
   target :
   GT_PP_SLV_WRITE_ERR ,
   PRV_CPSS_TC_12_SLV_WRITE_ERR,
   PRV_CPSS_TC_52_SLV_WRITE_ERR,
   PRV_CPSS_TD_12_SLV_WRITE_ERR,
   PRV_CPSS_TD_52_SLV_WRITE_ERR,
   PRV_CPSS_TD_XG_SLV_WRITE_ERR,
   PRV_CPSS_SB_12_SLV_WRITE_ERR,
   PRV_CPSS_SB_52_SLV_WRITE_ERR,
   PRV_CPSS_SB_XG_SLV_WRITE_ERR,
   PRV_CPSS_TG_12_SLV_WRITE_ERR,
   PRV_CPSS_TG_52_SLV_WRITE_ERR,
   PRV_CPSS_TG_XG_SLV_WRITE_ERR,
   0
*/

#define GEN_EV_ALL(event)       \
    {UNI_GEN_EV(event) ,        \
        GEN_EV(TC,12,event),    \
        GEN_EV(TC,52,event),    \
        GEN_EV(TD,12,event),    \
        GEN_EV(TD,52,event),    \
        GEN_EV(TD,XG,event),    \
        GEN_EV(SB,12,event),    \
        GEN_EV(SB,52,event),    \
        GEN_EV(SB,XG,event),    \
        GEN_EV(TG,12,event),    \
        GEN_EV(TG,52,event),    \
        GEN_EV(TG,XG,event),    \
        0                       \
    }


/* event for XG only */
#define XG_EV_ALL(event)       \
    {UNI_GEN_EV(event) ,        \
        CPSS_UNI_RSRVD_EVENT_E, \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        GEN_EV(TD,XG,event),    \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        GEN_EV(SB,XG,event),    \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        GEN_EV(TG,XG,event),    \
        0                       \
    }

/* event for TIGER only */
#define TIGER_EV_INDEX_ONLY(event,index)       \
    {UNI_GEN_EV(event) ,        \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        CPSS_UNI_RSRVD_EVENT_E,     \
        INDEX_EV(TG,12,event,index),  \
        INDEX_EV(TG,52,event,index),  \
        INDEX_EV(TG,XG,event,index),  \
        index                       \
    }

/* event for non-TIGER device */
#define NO_TIGER_EV(event)      \
    {UNI_GEN_EV(event) ,        \
        GEN_EV(TC,12,event),    \
        GEN_EV(TC,52,event),    \
        GEN_EV(TD,12,event),    \
        GEN_EV(TD,52,event),    \
        GEN_EV(TD,XG,event),    \
        GEN_EV(SB,12,event),    \
        GEN_EV(SB,52,event),    \
        GEN_EV(SB,XG,event),    \
        CPSS_UNI_RSRVD_EVENT_E, \
        CPSS_UNI_RSRVD_EVENT_E, \
        CPSS_UNI_RSRVD_EVENT_E, \
        0                       \
    }

/* MX devices event  */
#define MX_EV_ONLY(event)       \
    {UNI_GEN_EV(event) ,        \
        GEN_EV(TC,12,event),    \
        GEN_EV(TC,52,event),    \
        CPSS_UNI_RSRVD_EVENT_E, \
        CPSS_UNI_RSRVD_EVENT_E, \
        CPSS_UNI_RSRVD_EVENT_E, \
        GEN_EV(SB,12,event),    \
        GEN_EV(SB,52,event),    \
        GEN_EV(SB,XG,event),    \
        CPSS_UNI_RSRVD_EVENT_E, \
        CPSS_UNI_RSRVD_EVENT_E, \
        CPSS_UNI_RSRVD_EVENT_E, \
        0                       \
    }

/* CPU_PORT exists on all devices */
#define CPU_PORT_EV_ALL(event) \
    {UNI_PORT_GEN_EV(event) , \
        PORT_EV(TC,12,event,_CPU_63),    \
        PORT_EV(TC,52,event,_CPU_63),    \
        PORT_EV(TD,12,event,_CPU_63),    \
        PORT_EV(TD,52,event,_CPU_63),    \
        PORT_EV(TD,XG,event,_CPU_63),    \
        PORT_EV(SB,12,event,_CPU_63),    \
        PORT_EV(SB,52,event,_CPU_63),    \
        PORT_EV(SB,XG,event,_CPU_63),    \
        PORT_EV(TG,12,event,_CPU_63),    \
        PORT_EV(TG,52,event,_CPU_63),    \
        PORT_EV(TG,XG,event,_CPU_63),    \
        63                              \
    }

/* port 0 exists on all devices */
#define PORT_0_EV_ALL(event) \
    {UNI_PORT_GEN_EV(event) ,        \
        PORT_EV(TC,12,event,0),    \
        PORT_EV(TC,52,event,0),    \
        PORT_EV(TD,12,event,0),    \
        PORT_EV(TD,52,event,0),    \
        PORT_EV(TD,XG,event,0),    \
        PORT_EV(SB,12,event,0),    \
        PORT_EV(SB,52,event,0),    \
        PORT_EV(SB,XG,event,0),    \
        PORT_EV(TG,12,event,0),    \
        PORT_EV(TG,52,event,0),    \
        PORT_EV(TG,XG,event,0),    \
        0                          \
    }


/* support up ports 1..11
   the XG devices not support those events
*/
#define PORT_UP_12_EV_ALL(event,port) \
    {UNI_PORT_GEN_EV(event) ,        \
        PORT_EV(TC,12,event,port),    \
        PORT_EV(TC,52,event,port),    \
        PORT_EV(TD,12,event,port),    \
        PORT_EV(TD,52,event,port),    \
        CPSS_UNI_RSRVD_EVENT_E,           \
        PORT_EV(SB,12,event,port),    \
        PORT_EV(SB,52,event,port),    \
        CPSS_UNI_RSRVD_EVENT_E,           \
        PORT_EV(TG,12,event,port),    \
        PORT_EV(TG,52,event,port),    \
        CPSS_UNI_RSRVD_EVENT_E       ,    \
        port                          \
    }

/* support up ports 12..51
   the XG, devices with 12 ports devices not support those events
*/
#define PORT_UP_52_EV_ALL(event,port) \
    {UNI_PORT_GEN_EV(event) ,        \
        CPSS_UNI_RSRVD_EVENT_E,           \
        PORT_EV(TC,52,event,port),    \
        CPSS_UNI_RSRVD_EVENT_E,           \
        PORT_EV(TD,52,event,port),    \
        CPSS_UNI_RSRVD_EVENT_E,           \
        CPSS_UNI_RSRVD_EVENT_E,           \
        PORT_EV(SB,52,event,port),    \
        CPSS_UNI_RSRVD_EVENT_E,           \
        CPSS_UNI_RSRVD_EVENT_E,           \
        PORT_EV(TG,52,event,port),    \
        CPSS_UNI_RSRVD_EVENT_E,           \
        port                          \
    }


/* port 0 on devices , exclude the 12G devices */
#define PORT_0_EXCLUDE_GE_DEVICES(event) \
    {UNI_PORT_GEN_EV(event) ,        \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(TC,52,event,0),    \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(TD,52,event,0),    \
        PORT_EV(TD,XG,event,0),    \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(SB,52,event,0),    \
        PORT_EV(SB,XG,event,0),    \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(TG,52,event,0),    \
        PORT_EV(TG,XG,event,0),    \
        0                          \
    }

/* port 1..51 on devices , only the FE devices */
#define PORT_UP_52_ONLY_FE_DEVICES(event,port) \
    {UNI_PORT_GEN_EV(event) ,        \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(TC,52,event,0),    \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(TD,52,event,0),    \
        CPSS_UNI_RSRVD_EVENT_E,        \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(SB,52,event,0),    \
        CPSS_UNI_RSRVD_EVENT_E,        \
        CPSS_UNI_RSRVD_EVENT_E,        \
        PORT_EV(TG,52,event,0),    \
        CPSS_UNI_RSRVD_EVENT_E,        \
        port                       \
    }

/* interrupts that has index , the used number is (index-1) */
#define INTERRUPT_EV_ALL(event,index)       \
    {UNI_GEN_EV(event) ,        \
        INTERRUPT_EV(TC,12,event,index),    \
        INTERRUPT_EV(TC,52,event,index),    \
        INTERRUPT_EV(TD,12,event,index),    \
        INTERRUPT_EV(TD,52,event,index),    \
        INTERRUPT_EV(TD,XG,event,index),    \
        INTERRUPT_EV(SB,12,event,index),    \
        INTERRUPT_EV(SB,52,event,index),    \
        INTERRUPT_EV(SB,XG,event,index),    \
        INTERRUPT_EV(TG,12,event,index),    \
        INTERRUPT_EV(TG,52,event,index),    \
        INTERRUPT_EV(TG,XG,event,index),    \
        (index-1)                           \
    }

/* general events that are per queue */
#define QUEUE_EV_ALL(event,queue)       \
    {UNI_QUEUE_GEN_EV(event) ,        \
        QUEUE_EV(TC,12,event,queue),    \
        QUEUE_EV(TC,52,event,queue),    \
        QUEUE_EV(TD,12,event,queue),    \
        QUEUE_EV(TD,52,event,queue),    \
        QUEUE_EV(TD,XG,event,queue),    \
        QUEUE_EV(SB,12,event,queue),    \
        QUEUE_EV(SB,52,event,queue),    \
        QUEUE_EV(SB,XG,event,queue),    \
        QUEUE_EV(TG,12,event,queue),    \
        QUEUE_EV(TG,52,event,queue),    \
        QUEUE_EV(TG,XG,event,queue),    \
        queue                           \
    }

/* loop on all ports 1..51 , but only on FE devices */
#define LOOP_1_51_PORTS_ONLY_FE_DEVICES(event)   \
    PORT_UP_52_ONLY_FE_DEVICES(event,1),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,2),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,3),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,4),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,5),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,6),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,7),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,8),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,9),         \
    PORT_UP_52_ONLY_FE_DEVICES(event,10),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,11),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,12),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,13),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,14),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,15),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,16),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,17),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,18),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,19),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,20),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,21),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,22),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,23),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,24),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,25),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,26),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,27),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,28),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,29),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,30),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,31),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,32),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,33),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,34),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,35),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,36),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,37),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,38),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,39),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,40),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,41),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,42),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,43),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,44),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,45),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,46),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,47),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,48),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,49),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,50),        \
    PORT_UP_52_ONLY_FE_DEVICES(event,51)

/* loop on all ports 0..51 , but no XG devices */
#define LOOP_ALL_PORTS_EXCLUDE_XG_DEVICES(event)   \
    PORT_UP_12_EV_ALL(event,0),                     \
    LOOP_1_51_PORTS(event)

/* loop on all ports 0..51 , but no 12GE devices */
#define LOOP_ALL_PORTS_EXCLUDE_GE_DEVICES(event)   \
    PORT_0_EXCLUDE_GE_DEVICES(event),              \
    LOOP_1_51_PORTS_ONLY_FE_DEVICES(event)

/* loop on all ports 0..51 , but only on FE devices */
#define LOOP_ALL_PORTS_ONLY_FE_DEVICES(event)   \
    PORT_UP_52_ONLY_FE_DEVICES(event,0),         \
    LOOP_1_51_PORTS_ONLY_FE_DEVICES(event)



/* loop on all ports 0..47 but only on FE ports device*/
#define LOOP_0_47_PORTS_ONLY_FE_PORTS(event)   \
    PORT_UP_52_EV_ALL(event,0) ,          \
                                        \
    PORT_UP_52_EV_ALL(event,1),         \
    PORT_UP_52_EV_ALL(event,2),         \
    PORT_UP_52_EV_ALL(event,3),         \
    PORT_UP_52_EV_ALL(event,4),         \
    PORT_UP_52_EV_ALL(event,5),         \
    PORT_UP_52_EV_ALL(event,6),         \
    PORT_UP_52_EV_ALL(event,7),         \
    PORT_UP_52_EV_ALL(event,8),         \
    PORT_UP_52_EV_ALL(event,9),         \
    PORT_UP_52_EV_ALL(event,10),        \
    PORT_UP_52_EV_ALL(event,11),        \
                                        \
    PORT_UP_52_EV_ALL(event,12),        \
    PORT_UP_52_EV_ALL(event,13),        \
    PORT_UP_52_EV_ALL(event,14),        \
    PORT_UP_52_EV_ALL(event,15),        \
    PORT_UP_52_EV_ALL(event,16),        \
    PORT_UP_52_EV_ALL(event,17),        \
    PORT_UP_52_EV_ALL(event,18),        \
    PORT_UP_52_EV_ALL(event,19),        \
    PORT_UP_52_EV_ALL(event,20),        \
    PORT_UP_52_EV_ALL(event,21),        \
    PORT_UP_52_EV_ALL(event,22),        \
    PORT_UP_52_EV_ALL(event,23),        \
    PORT_UP_52_EV_ALL(event,24),        \
    PORT_UP_52_EV_ALL(event,25),        \
    PORT_UP_52_EV_ALL(event,26),        \
    PORT_UP_52_EV_ALL(event,27),        \
    PORT_UP_52_EV_ALL(event,28),        \
    PORT_UP_52_EV_ALL(event,29),        \
    PORT_UP_52_EV_ALL(event,30),        \
    PORT_UP_52_EV_ALL(event,31),        \
    PORT_UP_52_EV_ALL(event,32),        \
    PORT_UP_52_EV_ALL(event,33),        \
    PORT_UP_52_EV_ALL(event,34),        \
    PORT_UP_52_EV_ALL(event,35),        \
    PORT_UP_52_EV_ALL(event,36),        \
    PORT_UP_52_EV_ALL(event,37),        \
    PORT_UP_52_EV_ALL(event,38),        \
    PORT_UP_52_EV_ALL(event,39),        \
    PORT_UP_52_EV_ALL(event,40),        \
    PORT_UP_52_EV_ALL(event,41),        \
    PORT_UP_52_EV_ALL(event,42),        \
    PORT_UP_52_EV_ALL(event,43),        \
    PORT_UP_52_EV_ALL(event,44),        \
    PORT_UP_52_EV_ALL(event,45),        \
    PORT_UP_52_EV_ALL(event,46),        \
    PORT_UP_52_EV_ALL(event,47)

/* ports 1..51 */
#define LOOP_1_51_PORTS(event)           \
    PORT_UP_12_EV_ALL(event,1),         \
    PORT_UP_12_EV_ALL(event,2),         \
    PORT_UP_12_EV_ALL(event,3),         \
    PORT_UP_12_EV_ALL(event,4),         \
    PORT_UP_12_EV_ALL(event,5),         \
    PORT_UP_12_EV_ALL(event,6),         \
    PORT_UP_12_EV_ALL(event,7),         \
    PORT_UP_12_EV_ALL(event,8),         \
    PORT_UP_12_EV_ALL(event,9),         \
    PORT_UP_12_EV_ALL(event,10),        \
    PORT_UP_12_EV_ALL(event,11),        \
                                        \
    PORT_UP_52_EV_ALL(event,12),        \
    PORT_UP_52_EV_ALL(event,13),        \
    PORT_UP_52_EV_ALL(event,14),        \
    PORT_UP_52_EV_ALL(event,15),        \
    PORT_UP_52_EV_ALL(event,16),        \
    PORT_UP_52_EV_ALL(event,17),        \
    PORT_UP_52_EV_ALL(event,18),        \
    PORT_UP_52_EV_ALL(event,19),        \
    PORT_UP_52_EV_ALL(event,20),        \
    PORT_UP_52_EV_ALL(event,21),        \
    PORT_UP_52_EV_ALL(event,22),        \
    PORT_UP_52_EV_ALL(event,23),        \
    PORT_UP_52_EV_ALL(event,24),        \
    PORT_UP_52_EV_ALL(event,25),        \
    PORT_UP_52_EV_ALL(event,26),        \
    PORT_UP_52_EV_ALL(event,27),        \
    PORT_UP_52_EV_ALL(event,28),        \
    PORT_UP_52_EV_ALL(event,29),        \
    PORT_UP_52_EV_ALL(event,30),        \
    PORT_UP_52_EV_ALL(event,31),        \
    PORT_UP_52_EV_ALL(event,32),        \
    PORT_UP_52_EV_ALL(event,33),        \
    PORT_UP_52_EV_ALL(event,34),        \
    PORT_UP_52_EV_ALL(event,35),        \
    PORT_UP_52_EV_ALL(event,36),        \
    PORT_UP_52_EV_ALL(event,37),        \
    PORT_UP_52_EV_ALL(event,38),        \
    PORT_UP_52_EV_ALL(event,39),        \
    PORT_UP_52_EV_ALL(event,40),        \
    PORT_UP_52_EV_ALL(event,41),        \
    PORT_UP_52_EV_ALL(event,42),        \
    PORT_UP_52_EV_ALL(event,43),        \
    PORT_UP_52_EV_ALL(event,44),        \
    PORT_UP_52_EV_ALL(event,45),        \
    PORT_UP_52_EV_ALL(event,46),        \
    PORT_UP_52_EV_ALL(event,47),        \
    PORT_UP_52_EV_ALL(event,48),        \
    PORT_UP_52_EV_ALL(event,49),        \
    PORT_UP_52_EV_ALL(event,50),        \
    PORT_UP_52_EV_ALL(event,51)

/* ports 0..51 */
#define LOOP_ALL_PORTS(event)           \
    PORT_0_EV_ALL(event) ,              \
    LOOP_1_51_PORTS(event)

/* support up ports 1..11
   only for TG devices
*/
#define PORT_UP_12_EV_ONLY_TG(event,port)   \
    {UNI_PORT_GEN_EV(event) ,        \
        CPSS_UNI_RSRVD_EVENT_E,      \
        CPSS_UNI_RSRVD_EVENT_E,      \
        CPSS_UNI_RSRVD_EVENT_E,      \
        CPSS_UNI_RSRVD_EVENT_E,      \
        CPSS_UNI_RSRVD_EVENT_E,      \
        CPSS_UNI_RSRVD_EVENT_E,      \
        CPSS_UNI_RSRVD_EVENT_E,      \
        CPSS_UNI_RSRVD_EVENT_E,      \
        PORT_EV(TG,12,event,port),   \
        PORT_EV(TG,52,event,port),   \
        CPSS_UNI_RSRVD_EVENT_E,      \
        port                         \
    }

/* support up ports 12..51
   only for TG devices
*/
#define PORT_UP_52_EV_ONLY_TG(event,port)   \
    {UNI_PORT_GEN_EV(event) ,      \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        PORT_EV(TG,52,event,port), \
        CPSS_UNI_RSRVD_EVENT_E,    \
        port                       \
    }

#define LOOP_1_51_PORTS_ONLY_TG(event)  \
    PORT_UP_12_EV_ONLY_TG(event,1),     \
    PORT_UP_12_EV_ONLY_TG(event,2),     \
    PORT_UP_12_EV_ONLY_TG(event,3),     \
    PORT_UP_12_EV_ONLY_TG(event,4),     \
    PORT_UP_12_EV_ONLY_TG(event,5),     \
    PORT_UP_12_EV_ONLY_TG(event,6),     \
    PORT_UP_12_EV_ONLY_TG(event,7),     \
    PORT_UP_12_EV_ONLY_TG(event,8),     \
    PORT_UP_12_EV_ONLY_TG(event,9),     \
    PORT_UP_12_EV_ONLY_TG(event,10),    \
    PORT_UP_12_EV_ONLY_TG(event,11),    \
                                        \
    PORT_UP_52_EV_ONLY_TG(event,12),    \
    PORT_UP_52_EV_ONLY_TG(event,13),    \
    PORT_UP_52_EV_ONLY_TG(event,14),    \
    PORT_UP_52_EV_ONLY_TG(event,15),    \
    PORT_UP_52_EV_ONLY_TG(event,16),    \
    PORT_UP_52_EV_ONLY_TG(event,17),    \
    PORT_UP_52_EV_ONLY_TG(event,18),    \
    PORT_UP_52_EV_ONLY_TG(event,19),    \
    PORT_UP_52_EV_ONLY_TG(event,20),    \
    PORT_UP_52_EV_ONLY_TG(event,21),    \
    PORT_UP_52_EV_ONLY_TG(event,22),    \
    PORT_UP_52_EV_ONLY_TG(event,23),    \
    PORT_UP_52_EV_ONLY_TG(event,24),    \
    PORT_UP_52_EV_ONLY_TG(event,25),    \
    PORT_UP_52_EV_ONLY_TG(event,26),    \
    PORT_UP_52_EV_ONLY_TG(event,27),    \
    PORT_UP_52_EV_ONLY_TG(event,28),    \
    PORT_UP_52_EV_ONLY_TG(event,29),    \
    PORT_UP_52_EV_ONLY_TG(event,30),    \
    PORT_UP_52_EV_ONLY_TG(event,31),    \
    PORT_UP_52_EV_ONLY_TG(event,32),    \
    PORT_UP_52_EV_ONLY_TG(event,33),    \
    PORT_UP_52_EV_ONLY_TG(event,34),    \
    PORT_UP_52_EV_ONLY_TG(event,35),    \
    PORT_UP_52_EV_ONLY_TG(event,36),    \
    PORT_UP_52_EV_ONLY_TG(event,37),    \
    PORT_UP_52_EV_ONLY_TG(event,38),    \
    PORT_UP_52_EV_ONLY_TG(event,39),    \
    PORT_UP_52_EV_ONLY_TG(event,40),    \
    PORT_UP_52_EV_ONLY_TG(event,41),    \
    PORT_UP_52_EV_ONLY_TG(event,42),    \
    PORT_UP_52_EV_ONLY_TG(event,43),    \
    PORT_UP_52_EV_ONLY_TG(event,44),    \
    PORT_UP_52_EV_ONLY_TG(event,45),    \
    PORT_UP_52_EV_ONLY_TG(event,46),    \
    PORT_UP_52_EV_ONLY_TG(event,47),    \
    PORT_UP_52_EV_ONLY_TG(event,48),    \
    PORT_UP_52_EV_ONLY_TG(event,49),    \
    PORT_UP_52_EV_ONLY_TG(event,50),    \
    PORT_UP_52_EV_ONLY_TG(event,51)

/* port 0 exists on TG devices */
#define PORT_0_EV_ONLY_TG(event) \
    {UNI_PORT_GEN_EV(event) ,        \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        CPSS_UNI_RSRVD_EVENT_E,    \
        PORT_EV(TG,12,event,0),    \
        PORT_EV(TG,52,event,0),    \
        PORT_EV(TG,XG,event,0),    \
        0                          \
    }

/* ports 0..51 for TG devices only */
#define LOOP_ALL_PORTS_ONLY_TG(event)   \
    PORT_0_EV_ONLY_TG(event) ,          \
    LOOP_1_51_PORTS_ONLY_TG(event)

/* loop on all queues 0..7 */
#define LOOP_ALL_QUEUES(event)          \
    QUEUE_EV_ALL(event,0),              \
    QUEUE_EV_ALL(event,1),              \
    QUEUE_EV_ALL(event,2),              \
    QUEUE_EV_ALL(event,3),              \
    QUEUE_EV_ALL(event,4),              \
    QUEUE_EV_ALL(event,5),              \
    QUEUE_EV_ALL(event,6),              \
    QUEUE_EV_ALL(event,7)

/* general events that are per queue */
#define QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,queue)       \
    {UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(event,queue) ,        \
        QUEUE_EV(TC,12,event,queue),    \
        QUEUE_EV(TC,52,event,queue),    \
        QUEUE_EV(TD,12,event,queue),    \
        QUEUE_EV(TD,52,event,queue),    \
        QUEUE_EV(TD,XG,event,queue),    \
        QUEUE_EV(SB,12,event,queue),    \
        QUEUE_EV(SB,52,event,queue),    \
        QUEUE_EV(SB,XG,event,queue),    \
        QUEUE_EV(TG,12,event,queue),    \
        QUEUE_EV(TG,52,event,queue),    \
        QUEUE_EV(TG,XG,event,queue),    \
        queue                           \
    }

/**/
#define QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(event)    \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,0),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,1),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,2),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,3),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,4),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,5),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,6),            \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,7)



/* generic event that uses different name for the unify name and for the PPs */
#define GEN_EV_ALL_DIFF_NAME_FOR_PP(uniEvent , ppEvent)       \
    {UNI_GEN_EV(uniEvent) ,        \
        GEN_EV(TC,12,ppEvent),    \
        GEN_EV(TC,52,ppEvent),    \
        GEN_EV(TD,12,ppEvent),    \
        GEN_EV(TD,52,ppEvent),    \
        GEN_EV(TD,XG,ppEvent),    \
        GEN_EV(SB,12,ppEvent),    \
        GEN_EV(SB,52,ppEvent),    \
        GEN_EV(SB,XG,ppEvent),    \
        GEN_EV(TG,12,ppEvent),    \
        GEN_EV(TG,52,ppEvent),    \
        GEN_EV(TG,XG,ppEvent),    \
        0                         \
    }


/* the conversion table */
static GT_U16 ppUni2IntCauseType[][TABLE_ENTRY_SIZE] = {
/*****************************************/
/* PCI */
    GEN_EV_ALL(SLV_WRITE_ERR),
    GEN_EV_ALL(MAS_WRITE_ERR),
    GEN_EV_ALL(ADDR_ERR),
    GEN_EV_ALL(MAS_ABORT),
    GEN_EV_ALL(TARGET_ABORT),
    GEN_EV_ALL(SLV_READ_ERR),
    GEN_EV_ALL(MAS_READ_ERR),
    GEN_EV_ALL(RETRY_CNTR),

/*****************************/
/* MISC */

    GEN_EV_ALL(MISC_C2C_W_FAR_END_UP),
    GEN_EV_ALL(MISC_C2C_N_FAR_END_UP),
    GEN_EV_ALL(MISC_C2C_DATA_ERR),
    GEN_EV_ALL(MISC_MSG_TIME_OUT),
    GEN_EV_ALL(MISC_ILLEGAL_ADDR),

/*****************************/
/* GPP */
    INTERRUPT_EV_ALL(GPP,1),
    INTERRUPT_EV_ALL(GPP,2),
    INTERRUPT_EV_ALL(GPP,3),

/*****************************/
/* BM */

    LOOP_ALL_PORTS(BM_MAX_BUFF_REACHED), /*BUFF_FULL*/

    GEN_EV_ALL(BM_INVALID_ADDRESS),  /*BM_ADDR_OUT_OF_RANGE*/
    GEN_EV_ALL(BM_RX_MEM_READ_ECC_ERROR),/*RX_MEM_READ_ECC_ERROR*/
    GEN_EV_ALL(BM_VLT_ECC_ERR),/*VLT_ECC_ERR*/
/*****************************/
/* PORT */
    LOOP_ALL_PORTS_EXCLUDE_XG_DEVICES(LINK_STATUS_CHANGED),/*LINK_CHANGE*/
    LOOP_ALL_PORTS_EXCLUDE_XG_DEVICES(AN_COMPLETED),/*AN_COMPLETE*/
    LOOP_ALL_PORTS(RX_FIFO_OVERRUN),/*RX_DROPPED_ON_FIFO_FULL , RX_PCKT_DROP_ON_RX_FIFO_FULL */
    LOOP_ALL_PORTS(TX_FIFO_UNDERRUN),/*TX_FIFO_UNDER_RUN,XG_TX_FIFO_UNDRN*/
    LOOP_ALL_PORTS(TQ_WATCHDOG_EX),/*WD_EXPIRED*/
    LOOP_ALL_PORTS(TQ_TXQ2_FLUSH), /*FLUSH_OCCURS*/
    LOOP_ALL_PORTS_EXCLUDE_GE_DEVICES(ILLEGAL_SEQUENCE), /*UNKNOWN_SEQUENCE,NOT_VALID_SEQN*/
    LOOP_0_47_PORTS_ONLY_FE_PORTS(IPG_TOO_SMALL),

    LOOP_ALL_PORTS_ONLY_TG(EPF_EXRMON_SHADOW_REG_EMPTY),/*EPF EXRMON SHADOW EMPTY*/
    LOOP_ALL_PORTS_ONLY_TG(TXQ_EXRMON_SHADOW_REG_EMPTY),/*EXRMON EXRMON_SHADOW EMPTY*/
    LOOP_ALL_PORTS_ONLY_TG(EPF_EXRMON_TOTAL_SAMP_FULL),/*EPF EXRMON TOTAL SAMPEL FULL*/
    LOOP_ALL_PORTS_ONLY_TG(TXQ_EXRMON_TOTAL_SAMP_FULL),/*TXQ EXRMON TOTAL SAMPEL FULL*/
/*****************************/
/* TQ */
    GEN_EV_ALL(TQ_TXQ2_MG_FLUSH),/*MG_FLUSH_OCCURS*/
    GEN_EV_ALL_DIFF_NAME_FOR_PP(TQ_ONE_ECC_ERROR,TQ_LINK_LIST_ECC_ERR_HI),/*LINK_LIST_READ_ECC_ERR_HI*/
    GEN_EV_ALL_DIFF_NAME_FOR_PP(TQ_TWO_ECC_ERROR,TQ_LINK_LIST_ECC_ERR_LO),/*LINK_LIST_READ_ECC_ERR_LO*/
    GEN_EV_ALL(TQ_MG_READ_ERR),/*MG_READ_ERR*/
    GEN_EV_ALL(TQ_TOTAL_DESC_UNDERFLOW),/*TOTAL_DESC_UNDERFLOW*/
    GEN_EV_ALL(TQ_TOTAL_DESC_OVERFLOW),/*TOTAL_DESC_OVERFLOW*/
    GEN_EV_ALL(TQ_SNIFF_DESC_DROP),/*SNIFF_DESC_DROP*/
    GEN_EV_ALL(TQ_MLL_PARITY_ERR),/*MLL_PARITY_ERR*/
    GEN_EV_ALL(TQ_MC_FIFO_OVERRUN),/*MC_FIFO_OVERRUN*/
/*****************************/
/* TQ - PORT */
    LOOP_ALL_PORTS_ONLY_FE_DEVICES(TQ_RED_REACHED),/*RED*/
    CPU_PORT_EV_ALL(TQ_RED_REACHED),/*RED*/
/*****************************/
/* XG PORT */
    /* only XG support */
    XG_EV_ALL(PORT_FAULT_TYPE_CHANGE),/*FAULT_CHANGE*/
    /* only XG support */
    XG_EV_ALL(PORT_CONSECUTIVE_TERM_CODE),/*CONSECUTIVE_TERMINATE*/
/*****************************/
/* EB */
    GEN_EV_ALL(EB_AUQ_FULL),/*AU_QUEUE_FULL*/
    GEN_EV_ALL(EB_NA_FIFO_FULL),/*NA_FIFO_FULL*/
    GEN_EV_ALL(EB_MG_ADDR_OUT_OF_RANGE),/*MG_ADDR_OUT_OF_RANGE*/
    GEN_EV_ALL(EB_NA_NOT_LEARNED_SECURITY_BREACH),/*NA_NOT_LRND_SEC_BREECH*/
    GEN_EV_ALL(MAC_TBL_READ_ECC_ERR),
    GEN_EV_ALL(EB_INGRESS_FILTER_PCKT),/*INGRESS_FILTER_PCKT*/
/*****************************/
/* MAC */

    /* only XG support */
    XG_EV_ALL(MAC_SFLOW), /*SFLOW*/
    GEN_EV_ALL(MAC_NUM_OF_HOP_EXP),/*NUM_OF_HOPS_EXP*/
    GEN_EV_ALL(MAC_NA_LEARNED),/*NA_LRND*/
    GEN_EV_ALL(MAC_NA_NOT_LEARNED),/*NA_NOT_LRND*/
    GEN_EV_ALL(MAC_AGE_VIA_TRIGGER_ENDED),/*AGE_VIA_TRIG_END*/
    GEN_EV_ALL(MAC_MG_ADDR_OUT_OF_RANGE),/*MAC0_ADDR_OUT_OF_RANGE*/
    GEN_EV_ALL_DIFF_NAME_FOR_PP(MAC_UPDATE_FROM_CPU_DONE,MAC_AU_PROCESSED),/*AU_PROCESSED*/

    /* use different name for the unify name and for the PPs ,
       because the MAC_MESSAGE_TO_CPU_READY also used for the "AUQ FULL" */
    GEN_EV_ALL_DIFF_NAME_FOR_PP(MAC_MESSAGE_TO_CPU_READY,MAC_AUQ_PENDING),/*AUQ_PENDING*/

    /* use different name for the unify name and for the PPs ,
       because the MAC_MESSAGE_TO_CPU_READY also used for the "AUQ pending" */
    GEN_EV_ALL_DIFF_NAME_FOR_PP(MAC_MESSAGE_TO_CPU_READY,EB_AUQ_FULL),/*AU_QUEUE_FULL*/
/*****************************/
/* LX */

    GEN_EV_ALL(LX_LB_ERR),/*LB_ERR*/
    GEN_EV_ALL(LX_CTRL_MEM_2_RF_ERR),/*CTRL_MEM_2_RF_ERR*/
    GEN_EV_ALL(LX_TCB_CNTR),/*TCB_CNTR*/
    NO_TIGER_EV(LX_IPV4_MC_ERR),/*IPV4_MC_ERR*/
    NO_TIGER_EV(LX_IPV4_LPM_ERR),/*IPV4_LPM_ERR*/
    TIGER_EV_INDEX_ONLY(LX_IPV4_LPM_ERR,0),/*LX_LPM0*/
    TIGER_EV_INDEX_ONLY(LX_IPV4_LPM_ERR,1),/*LX_LPM1*/
    TIGER_EV_INDEX_ONLY(LX_IPV4_LPM_ERR,2),/*LX_LPM2*/
    TIGER_EV_INDEX_ONLY(LX_IPV4_LPM_ERR,3),/*LX_LPM3*/
    TIGER_EV_INDEX_ONLY(LX_IPV4_LPM_ERR,4),/*LX_LPM4*/
    GEN_EV_ALL(LX_IPV4_ROUTE_ERR),/*IPV4_ROUTE_ERR*/
    GEN_EV_ALL(LX_IPV4_CNTR),/*IPV4_CNTR*/
    GEN_EV_ALL(LX_L3_L7_ERR_ADDR),/*L3_L7_ERR_ADDR*/
/* explicit entry  */
    {   UNI_GEN_EV(LX_TRUNK_ADDR_OUT_OF_RANGE)  ,  CPSS_UNI_RSRVD_EVENT_E                   ,CPSS_UNI_RSRVD_EVENT_E              ,  GEN_EV(TD,12,LX_TRUNK_ADDR_OUT_OF_RANGE)  ,GEN_EV(TD,52,LX_TRUNK_ADDR_OUT_OF_RANGE),GEN_EV(TD,XG,LX_TRUNK_ADDR_OUT_OF_RANGE)    ,CPSS_UNI_RSRVD_EVENT_E                   ,CPSS_UNI_RSRVD_EVENT_E          ,GEN_EV(SB,XG,TRUNK_ADDR_OUT_OF_RANGE)        ,CPSS_UNI_RSRVD_EVENT_E                        ,CPSS_UNI_RSRVD_EVENT_E                        ,CPSS_UNI_RSRVD_EVENT_E                         , 0 },
/* explicit entry  */
    {   UNI_GEN_EV(LX_IPV4_REFRESH_AGE_OVERRUN) ,  CPSS_UNI_RSRVD_EVENT_E                   ,CPSS_UNI_RSRVD_EVENT_E              ,  GEN_EV(TD,12,LX_IPV4_REFRESH_AGE_OVERRUN),GEN_EV(TD,52,LX_IPV4_REFRESH_AGE_OVERRUN),GEN_EV(TD,XG,LX_IPV4_REFRESH_AGE_OVERRUN)   ,CPSS_UNI_RSRVD_EVENT_E                  ,CPSS_UNI_RSRVD_EVENT_E           ,GEN_EV(SB,XG,LX_IPV4_REFRESH_AGE_OVERRUN)    ,CPSS_UNI_RSRVD_EVENT_E                        ,CPSS_UNI_RSRVD_EVENT_E                        ,GEN_EV(TG,XG,LX_IPV4_REFRESH_AGE_OVERRUN)      , 0 },
/* explicit entry  */
    {   UNI_GEN_EV(LX_PCE_PAR_ERR)              ,  CPSS_UNI_RSRVD_EVENT_E                   ,CPSS_UNI_RSRVD_EVENT_E              ,  GEN_EV(TD,12,LX_PCE_PAR_ERR)             ,GEN_EV(TD,52,LX_PCE_PAR_ERR)             ,GEN_EV(TD,XG,LX_PCE_PAR_ERR)                ,CPSS_UNI_RSRVD_EVENT_E                   ,CPSS_UNI_RSRVD_EVENT_E          ,GEN_EV(SB,XG,LX_PCE_PAR_ERR)                 ,CPSS_UNI_RSRVD_EVENT_E                        ,CPSS_UNI_RSRVD_EVENT_E                        ,GEN_EV(TG,XG,LX_PCE_PAR_ERR)                   , 0 },

    GEN_EV_ALL(LX_TC_2_RF_CNTR_ALRM),/*TC_2_RF_CNTR_ALRM*/
    GEN_EV_ALL(LX_TC_2_RF_PLC_ALRM),/*TC_2_RF_PLC_ALRM*/
    GEN_EV_ALL(LX_TC_2_RF_TBL_ERR),/*TC_2_RF_TBL_ERR*/
/* MX devices only */
    MX_EV_ONLY(LX_CLASSIFIER_HASH_PAR_ERR),/*CLH_ERR*/
/* MX devices only */
    MX_EV_ONLY(LX_FLOW_LKUP_PAR_ERR),/*FL_ERR*/
/* MX devices only */
    MX_EV_ONLY(LX_FLOW_KEY_TBL_PAR_ERR),/*KEY_ERR*/
/* MX devices only */
    MX_EV_ONLY(LX_MPLS_ILM_TBL_PAR_ERR),/*MPLS_ILM_ERR*/
/* MX devices only */
    MX_EV_ONLY(LX_MPLS_CNTR),/*MPLS_CNTR*/
/*****************************/
/* TX - QUEUE */
    LOOP_ALL_QUEUES(TX_BUFFER),
    LOOP_ALL_QUEUES(TX_ERR),
    LOOP_ALL_QUEUES(TX_END),
/*****************************/
/* RX - QUEUE */
    QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(RX_ERR),
    QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(RX_BUFFER)

};

#define NUM_OF_TBL_ENTRIES \
    (sizeof(ppUni2IntCauseType)/(sizeof(GT_U16)*TABLE_ENTRY_SIZE))



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
    GT_U32              offset;/* offset from the base EV type for this device
                                  family */

    switch (prvCpssDrvPpConfig[devNum]->devFamily)
    {
        case CPSS_PP_FAMILY_TWISTC_E:
            baseDevType = TWIST_12;
            break;
        case CPSS_PP_FAMILY_TWISTD_E:
            baseDevType = TWIST_D_12;
            break;
        case CPSS_PP_FAMILY_SAMBA_E:
            baseDevType = SAMBA_12;
            break;
        case CPSS_PP_FAMILY_TIGER_E:
            baseDevType = TIGER_12;
            break;
        default:
            DBG_INFO(("file:%s line:%d intType:%d\n", __FILE__, __LINE__,
                      intCause->intType));
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(prvCpssDrvPpConfig[devNum]->numOfPorts)
    {
        case 10:
        case 12:
            offset = 0;
            break;
        case 52:
            offset = 1;
            break;
        case 1:
            offset = 2;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    *ppColumnPtr = baseDevType + offset;

    return GT_OK;
}


/*******************************************************************************
* prvCpssDrvPpIntExMxIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. -- ExMx devices
*
* INPUTS:
*       devNum      - The device number.
*       portGroupId      - The port group Id.
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
GT_STATUS prvCpssDrvPpIntExMxIntCauseToUniEvConvert
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
    GT_U32          ppColumn = 0;   /* conversion table pp column   */

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
            *uniEvPtr   = ppUni2IntCauseType[ii][UNI_EV];
            *extDataPtr = ppUni2IntCauseType[ii][EXT_DATA];
            return GT_OK;
        }
    }
    DBG_INFO(("file:%s line:%d event not found intCause = %d\n", __FILE__,
              __LINE__, intCauseIndex));

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
}



