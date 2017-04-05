/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpIntDxChEventsUnify.c
*
* DESCRIPTION:
*       Conversion routines for unified events -- DxCh devices.
*
* FILE REVISION NUMBER:
*       $Revision: 33 $
*
*******************************************************************************/
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah2.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsCheetah3.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsDxChXcat.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsDxChXcat2.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLion.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxEventsLionB.h>
/* get the unify events definitions */
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>

/* TEMP */
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

/* enumeration to convert name from the API to application to be more
  "consistent" with the naming conventions of this file */
enum{
    CPSS_PP_PORT_BM_MAX_BUFF_REACHED_E    = CPSS_PP_BM_MAX_BUFF_REACHED_E,
    CPSS_PP_PORT_TQ_WATCHDOG_EX_E         = CPSS_PP_TQ_WATCHDOG_EX_PORT_E,
    CPSS_PP_PORT_TQ_TXQ2_FLUSH_E          = CPSS_PP_TQ_TXQ2_FLUSH_PORT_E,
    CPSS_PP_TX_END_QUEUE_E                = CPSS_PP_TX_END_E,
    CPSS_PP_PORT_BM_TQ_PARITY_ERROR_E     = CPSS_PP_BM_TQ_PARITY_ERROR_PORT_E,
    CPSS_PP_PORT_MAC_SFLOW_E              = CPSS_PP_MAC_SFLOW_E,
    CPSS_PP_PORT_EGRESS_STC_E             = CPSS_PP_EGRESS_SFLOW_E,
    CPSS_PP_PORT_TQ_DESC_FULL_E           = CPSS_PP_TQ_PORT_DESC_FULL_E,
    CPSS_PP_PORT_TQ_FULL_XG_E             = CPSS_PP_TQ_PORT_FULL_XG_E

};

/*
    build enum value like:
    PRV_CPSS_LION_LINK_STATUS_CHANGED_PORT0_E = PRV_CPSS_LION_PORT0_LINK_STATUS_CHANGED_E,

    because the generation from Cider put the port num before the name and not after like the ch1,2,3 did
*/

#define DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,port)  \
    PRV_CPSS_##dev##_##intName##_PORT##port##_E =               \
    PRV_CPSS_##dev##_PORT##port##_##intName##_E

/* for per XG port per lane ports - XPCS */
#define DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port,lane)  \
    PRV_CPSS_##dev1##_##intName##_LANE##lane##_PORT##port##_E =               \
    PRV_CPSS_##dev2##_PORT##port##_LANE##lane##_SUM_##intName##_E

/* XG port -- by postfix */
#define DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,port)  \
    PRV_CPSS_##dev##_##intName##_PORT##port##_E =               \
    PRV_CPSS_##dev##_PORT##port##_##postfix##_E

#define DEV_PORTS_INT_CONVERT_PORT_TO_END(dev,intName)   \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,0) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,1) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,2) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,3) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,4) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,5) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,6) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,7) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,8) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,9) , \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,10) ,\
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(dev,intName,11)

/*XG ports*/
#define LION_XG_PORTS_INT_CONVERT_PORT_TO_END(intName)   \
    DEV_PORTS_INT_CONVERT_PORT_TO_END(LION_XG,intName)

/*XG ports*/
#define LION_B_XG_PORTS_INT_CONVERT_PORT_TO_END(intName)   \
    DEV_PORTS_INT_CONVERT_PORT_TO_END(LION_B_XG,intName)

/*XG ports - XPCS */
#define LION_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(intName)   \
    DEV_PORTS_INT_CONVERT_PORT_TO_END(LION_XPCS,intName)

/*XG ports - XPCS */
#define LION_B_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(intName)   \
    DEV_PORTS_INT_CONVERT_PORT_TO_END(LION_B_XPCS,intName)

/* XG ports - all the event of all lanes of a port */
#define DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port)   \
    DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port,0) , \
    DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port,1) , \
    DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port,2) , \
    DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port,3) , \
    DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port,4) , \
    DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,port,5)

/* XG ports - all the event of all lanes of all ports */
#define DEV_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName)   \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,0) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,1) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,2) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,3) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,4) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,5) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,6) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,7) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,8) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,9) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,10) , \
    DEV_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(dev1,dev2,intName,11)

#define LION_B_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(intName)   \
        DEV_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(LION_B,LION_B_XG,intName)

/*  CPU port
    build enum value like:
    PRV_CPSS_LION_LINK_STATUS_CHANGED_PORT_CPU_63_E = PRV_CPSS_LION_CPU_PORT_SUM_LINK_STATUS_CHANGED_E,

    because the generation from Cider put the port num before the name and not after like the ch1,2,3 did
*/
#define DEV_CPU_PORT_INT_CONVERT_PORT_TO_END(dev,intName)  \
    PRV_CPSS_##dev##_##intName##_PORT_CPU_63_E =            \
    PRV_CPSS_##dev##_CPU_PORT_SUM_##intName##_E

/* all GE ports + cpu port */
#define LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(intName)   \
    DEV_PORTS_INT_CONVERT_PORT_TO_END(LION_B,intName),                    \
    DEV_CPU_PORT_INT_CONVERT_PORT_TO_END(LION_B,intName)

/* port  - by prefix + postfix */
#define DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,port)  \
    PRV_CPSS_##dev##_##intName##_PORT##port##_E =               \
    PRV_CPSS_##dev##_##prefix##_PORT##port##_##postfix##_E

/* all GE ports  - by prefix + postfix */
#define DEV_PORTS_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix)   \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,0), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,1), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,2), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,3), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,4), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,5), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,6), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,7), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,8), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,9), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,10),\
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PARTS(dev,intName,prefix,postfix,11)

/* all GE ports  - by prefix + postfix */
#define LION_B_PORTS_INT_CONVERT_PORT_BY_PARTS(intName,prefix,postfix)   \
        DEV_PORTS_INT_CONVERT_PORT_BY_PARTS(LION_B,intName,prefix,postfix)

/* port -- by prefix */
#define DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,port)  \
    PRV_CPSS_##dev##_##intName##_PORT##port##_E =               \
    PRV_CPSS_##dev##_##prefix##_PORT##port##_E


/* all GE ports -- by prefix */
#define DEV_PORTS_INT_CONVERT_PORT_BY_BY_PREFIX(dev,intName,prefix)   \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,0), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,1), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,2), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,3), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,4), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,5), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,6), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,7), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,8), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,9), \
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,10),\
    DEV_PER_PORT_INT_CONVERT_PORT_BY_PREFIX(dev,intName,prefix,11)

#define LION_B_PORTS_INT_CONVERT_PORT_BY_BY_PREFIX(intName,prefix)   \
        DEV_PORTS_INT_CONVERT_PORT_BY_BY_PREFIX(LION_B,intName,prefix)

/* all GE ports + cpu port -- by prefix */
#define LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_BY_PREFIX(intName,prefix)   \
    DEV_PORTS_INT_CONVERT_PORT_BY_BY_PREFIX(LION_B,intName,prefix),                    \
    DEV_CPU_PORT_INT_CONVERT_PORT_TO_END(LION_B,intName)

/* all XG ports -- by postfix */
#define XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(dev,intName,postfix)   \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,0), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,1), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,2), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,3), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,4), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,5), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,6), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,7), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,8), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,9), \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,10),\
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(dev,intName,postfix,11)

/* Lion all XG ports -- by postfix */
#define LION_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(intName,postfix)   \
             XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(LION_XG,intName,postfix)

/* Lion B all XG ports -- by postfix */
#define LION_B_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(intName,postfix)   \
             XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(LION_B_XG,intName,postfix)

/* Lion B all XLG ports -- by postfix */
#define LION_B_XLG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(intName,postfix)   \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(LION_B_XLG,intName,postfix,10)


/*XLG ports*/
#define LION_B_XLG_PORTS_INT_CONVERT_PORT_TO_END(intName)   \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(LION_B_XLG,intName,10)



/*XG ports - XPCS */
#define LION_B_XLG_PORTS_PCS_INT_CONVERT_PORT_TO_END(intName)   \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(LION_B_XPCS_XLG,intName,10)

/* for XG ports */
#define XCAT_PER_XG_PORT_INT_CONVERT_PORT_TO_END(intName,port)  \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(XCAT_XG,intName,port)

/*XG ports*/
#define XCAT_XG_PORTS_INT_CONVERT_PORT_TO_END(intName)   \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_TO_END(intName,24) , \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_TO_END(intName,25) , \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_TO_END(intName,26) , \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_TO_END(intName,27)

/* XG port -- by postfix */
#define XCAT_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(intName,postfix,port)  \
    DEV_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(XCAT_XG,intName,postfix,port)


/* all XG ports -- by postfix */
#define XCAT_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(intName,postfix)   \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(intName,postfix,24), \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(intName,postfix,25), \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(intName,postfix,26), \
    XCAT_PER_XG_PORT_INT_CONVERT_PORT_BY_POSTFIX(intName,postfix,27)

/* for XG ports - XPCS */
#define XCAT_PER_XG_PORT_XPCS_INT_CONVERT_PORT_TO_END(intName,port)  \
    DEV_PER_PORT_INT_CONVERT_PORT_TO_END(XCAT_XPCS,intName,port)

/*XG ports - XPCS */
#define XCAT_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(intName)   \
    XCAT_PER_XG_PORT_XPCS_INT_CONVERT_PORT_TO_END(intName,24) , \
    XCAT_PER_XG_PORT_XPCS_INT_CONVERT_PORT_TO_END(intName,25) , \
    XCAT_PER_XG_PORT_XPCS_INT_CONVERT_PORT_TO_END(intName,26) , \
    XCAT_PER_XG_PORT_XPCS_INT_CONVERT_PORT_TO_END(intName,27)

/* for per XG port per lane ports - XPCS */
#define XCAT_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(intName,port,lane)  \
    DEV_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(XCAT,XCAT_XG,intName,port,lane)

/* XG ports - all the event of all lanes of a port */
#define XCAT_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(intName,port)   \
    XCAT_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(intName,port,0) , \
    XCAT_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(intName,port,1) , \
    XCAT_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(intName,port,2) , \
    XCAT_PER_XG_PORT_PER_LANE_INT_CONVERT_PORT_TO_END(intName,port,3)

/* XG ports - all the event of all lanes of all ports */
#define XCAT_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(intName)   \
    XCAT_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(intName,24) , \
    XCAT_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(intName,25) , \
    XCAT_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(intName,26) , \
    XCAT_PER_XG_PORT_LANES_INT_CONVERT_PORT_TO_END(intName,27)

/* enumeration to convert the lion interrupts to be like ch1,2,3 conventions .

    because all the names of interrupts that generated using PORT_EV and others
    not match the cider names.
*/
enum XCAT_FIX_NAMES_ENT{

    /* start GE port interrupts */
/*    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(LINK_STATUS_CHANGED),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(AN_COMPLETED),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(RX_FIFO_OVERRUN),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(TX_FIFO_UNDERRUN),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(TX_FIFO_OVERRUN),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(TX_UNDERRUN),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(ADDRESS_OUT_OF_RANGE),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(SYNC_STATUS_CHANGED),
    XCAT_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(PRBS_ERROR),*/

    /* start XG port interrupts */
    XCAT_XG_PORTS_INT_CONVERT_PORT_TO_END(LINK_STATUS_CHANGED),
    XCAT_XG_PORTS_INT_CONVERT_PORT_TO_END(RX_FIFO_OVERRUN),
    XCAT_XG_PORTS_INT_CONVERT_PORT_TO_END(TX_UNDERRUN),
    XCAT_XG_PORTS_INT_CONVERT_PORT_TO_END(ADDRESS_OUT_OF_RANGE),
    XCAT_XG_PORTS_INT_CONVERT_PORT_TO_END(FC_STATUS_CHANGED),
    XCAT_XG_PORTS_INT_CONVERT_PORT_TO_END(NO_BUFF_PACKET_DROP),
    XCAT_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(ILLEGAL_SEQUENCE,UNKNOWN_SEQUENCE),
    XCAT_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(FAULT_TYPE_CHANGE,FAULT_CHANGED),
    XCAT_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(COUNT_EXPIRED,XG_COUNT_EXPIRED),
    XCAT_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(COUNT_COPY_DONE,XG_COUNT_COPY_DONE),
    /* end XG port interrupts */

    /* start XG port - XPCS interrupts */
    XCAT_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(DESKEW_TIMEOUT),
    XCAT_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(DETECTED_COLUMN_IIAII),
    XCAT_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(DESKEW_ERROR),
    XCAT_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(PPM_FIFO_UNDERRUN),
    XCAT_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(PPM_FIFO_OVERRUN),
    XCAT_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(LINK_STATUS_CHANGED),
    /* end XG port - XPCS interrupts */

    /* start per port per lane */
    XCAT_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(PRBS_ERROR),
    XCAT_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(SIGNAL_DETECT_CHANGED),
    XCAT_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(SYNC_STATUS_CHANGED),
    XCAT_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(DETECTED_IIAII),
    XCAT_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(DISPARITY_ERROR),
    XCAT_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(SYMBOL_ERROR),
    /* end per port per lane */

    PRV_CPSS_XCAT_PCL_ADDR_OUT_RANGE_E = PRV_CPSS_XCAT_POLICY_ENGINE_MG_ADDR_OUT_OF_RANGE_E,

    PRV_CPSS_XCAT_POLICER_ADDR_OUT_OF_MEMORY_E = PRV_CPSS_XCAT_IPLR0_SUM_ADDRESS_OUT_OF_MEMORY_E,
    PRV_CPSS_XCAT_POLICER_DATA_ERR_E = PRV_CPSS_XCAT_IPLR0_SUM_DATA_ERROR_E,

    XCAT_END_E/* must be last*/
};

/* conversion of name keeping texts before and after the name */
#define PRV_CPSS_REDEF12_MAC(_dev, _text1, _new_name, _old_name, _text2)  \
    PRV_CPSS_##_dev##_##_text1##_##_new_name##_##_text2##_E =             \
    PRV_CPSS_##_dev##_##_text1##_##_old_name##_##_text2##_E

/* conversion of name keeping text before the name */
#define PRV_CPSS_REDEF1_MAC(_dev, _text1, _new_name, _old_name)  \
    PRV_CPSS_##_dev##_##_text1##_##_new_name##_E =               \
    PRV_CPSS_##_dev##_##_text1##_##_old_name##_E

/* conversion of name keeping text after the name */
#define PRV_CPSS_REDEF2_MAC(_dev, _new_name, _old_name, _text2)  \
    PRV_CPSS_##_dev##_##_new_name##_##_text2##_E =               \
    PRV_CPSS_##_dev##_##_old_name##_##_text2##_E

/* simple conversion of a name */
#define PRV_CPSS_REDEF_MAC(_dev, _new_name, _old_name)  \
    PRV_CPSS_##_dev##_##_new_name##_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_E

/* conversion of a name for network ports 0-23 */
#define PRV_CPSS_REDEF_PORT0_23_MAC(_dev, _new_name, _old_name)  \
    PRV_CPSS_##_dev##_##_new_name##_PORT0_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT0_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT1_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT1_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT2_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT2_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT3_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT3_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT4_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT4_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT5_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT5_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT6_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT6_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT7_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT7_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT8_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT8_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT9_E =                 \
    PRV_CPSS_##_dev##_##_old_name##_PORT9_E,                  \
    PRV_CPSS_##_dev##_##_new_name##_PORT10_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT10_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT11_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT11_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT12_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT12_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT13_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT13_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT14_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT14_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT15_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT15_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT16_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT16_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT17_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT17_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT18_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT18_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT19_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT19_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT20_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT20_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT21_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT21_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT22_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT22_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT23_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT23_E                  \

/* conversion of a name for stack ports 24-27 */
#define PRV_CPSS_REDEF_PORT24_27_MAC(_dev, _new_name, _old_name)  \
    PRV_CPSS_##_dev##_##_new_name##_PORT24_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT24_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT25_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT25_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT26_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT26_E,                 \
    PRV_CPSS_##_dev##_##_new_name##_PORT27_E =                \
    PRV_CPSS_##_dev##_##_old_name##_PORT27_E                  \

/* simple conversion of a name for network ports 0-23 */
#define PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(_name)   \
    PRV_CPSS_XCAT2_##_name##_PORT0_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_0_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT1_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_1_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT2_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_2_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT3_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_3_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT4_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_4_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT5_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_5_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT6_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_6_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT7_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_7_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT8_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_8_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT9_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_9_##_name##_E,          \
    PRV_CPSS_XCAT2_##_name##_PORT10_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_10_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT11_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_11_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT12_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_12_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT13_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_13_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT14_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_14_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT15_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_15_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT16_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_16_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT17_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_17_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT18_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_18_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT19_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_19_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT20_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_20_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT21_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_21_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT22_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_22_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT23_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_23_##_name##_E          \

/* simple conversion of a name for network ports 0-23 */
#define PRV_CPSS_XCAT2_REDEF1_NETW_PORT0_23_MAC(_new_name, _old_name)   \
    PRV_CPSS_XCAT2_##_new_name##_PORT0_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_0_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT1_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_1_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT2_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_2_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT3_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_3_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT4_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_4_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT5_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_5_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT6_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_6_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT7_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_7_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT8_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_8_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT9_E =                \
    PRV_CPSS_XCAT2_PORTS_NETW_9_##_old_name##_E,          \
    PRV_CPSS_XCAT2_##_new_name##_PORT10_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_10_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT11_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_11_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT12_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_12_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT13_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_13_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT14_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_14_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT15_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_15_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT16_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_16_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT17_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_17_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT18_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_18_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT19_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_19_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT20_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_20_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT21_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_21_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT22_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_22_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT23_E =               \
    PRV_CPSS_XCAT2_PORTS_NETW_23_##_old_name##_E          \

/* simple conversion of a name for stack ports 23-27 */
#define PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(_name)   \
    PRV_CPSS_XCAT2_##_name##_PORT24_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_24_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT25_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_25_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT26_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_26_##_name##_E,         \
    PRV_CPSS_XCAT2_##_name##_PORT27_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_27_##_name##_E          \

/* simple conversion of a name for stack ports 23-27 */
#define PRV_CPSS_XCAT2_REDEF1_STACK_PORT24_27_MAC(_new_name, _old_name)   \
    PRV_CPSS_XCAT2_##_new_name##_PORT24_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_24_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT25_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_25_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT26_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_26_##_old_name##_E,         \
    PRV_CPSS_XCAT2_##_new_name##_PORT27_E =                \
    PRV_CPSS_XCAT2_PORTS_STACK_27_##_old_name##_E          \

/* conversion of a name for CPU port */
#define PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(_name)   \
    PRV_CPSS_XCAT2_##_name##_PORT_CPU_63_E =        \
    PRV_CPSS_XCAT2_CPU_PORT_##_name##_E

/* conversion of a name for queues 0-7 */
#define PRV_CPSS_XCAT2_REDEF1_QUEUE0_7_MAC(_new_name, _old_name)   \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE0_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE0_E,                  \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE1_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE1_E,                  \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE2_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE2_E,                  \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE3_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE3_E,                  \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE4_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE4_E,                  \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE5_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE5_E,                  \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE6_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE6_E,                  \
    PRV_CPSS_XCAT2_##_new_name##_QUEUE7_E =                 \
    PRV_CPSS_XCAT2_##_old_name##_QUEUE7_E                   \

/* conversion of a name for GOPs 0-3 */
#define PRV_CPSS_XCAT2_REDEF_MIB_GOP0_3_MAC(_name)   \
    PRV_CPSS_XCAT2_GOP_##_name##_0_E =               \
    PRV_CPSS_XCAT2_PORTS_MIB_GOP0_##_name##_E,       \
    PRV_CPSS_XCAT2_GOP_##_name##_1_E =               \
    PRV_CPSS_XCAT2_PORTS_MIB_GOP1_##_name##_E,       \
    PRV_CPSS_XCAT2_GOP_##_name##_2_E =               \
    PRV_CPSS_XCAT2_PORTS_MIB_GOP2_##_name##_E,       \
    PRV_CPSS_XCAT2_GOP_##_name##_3_E =               \
    PRV_CPSS_XCAT2_PORTS_MIB_GOP3_##_name##_E        \

/* conversion of a name for IPLR0, IPLR1, EPLR */
#define PRV_CPSS_XCAT2_REDEF_PLR_MAC(_name)   \
    PRV_CPSS_XCAT2_IPLR0_SUM_##_name##_E =    \
        PRV_CPSS_XCAT2_IPLR0_##_name##_E,     \
    PRV_CPSS_XCAT2_IPLR1_SUM_##_name##_E =    \
        PRV_CPSS_XCAT2_IPLR1_##_name##_E,     \
    PRV_CPSS_XCAT2_EPLR_SUM_##_name##_E =     \
        PRV_CPSS_XCAT2_EPLR_##_name##_E       \

/* conversion of a name for IPCL0, IPCR0_1, IPCL1 */
#define PRV_CPSS_XCAT2_REDEF_IPCL_MAC(_name)   \
    PRV_CPSS_XCAT2_POLICY_ENGINE_PCL0_##_name##_E =    \
        PRV_CPSS_XCAT2_IPCL_PCL0_##_name##_E,          \
    PRV_CPSS_XCAT2_POLICY_ENGINE_PCL0_1_##_name##_E =  \
        PRV_CPSS_XCAT2_IPCL_PCL01_##_name##_E,         \
    PRV_CPSS_XCAT2_POLICY_ENGINE_PCL1_##_name##_E =    \
        PRV_CPSS_XCAT2_IPCL_PCL1_##_name##_E           \

enum XCAT2_FIX_NAMES_ENT
{
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT1,TXQ_GPP_INTERRUPT0),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT2,TXQ_GPP_INTERRUPT1),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT3,TXQ_GPP_INTERRUPT2),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT4,TXQ_GPP_INTERRUPT3),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT5,TXQ_GPP_INTERRUPT4),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT6,TXQ_GPP_INTERRUPT5),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT7,TXQ_GPP_INTERRUPT6),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT8,TXQ_GPP_INTERRUPT7),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT9,TXQ_GPP_INTERRUPT8),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT10,TXQ_GPP_INTERRUPT9),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT11,TXQ_GPP_INTERRUPT10),
    PRV_CPSS_REDEF_MAC(XCAT2,GPP_INTERRUPT12,TXQ_GPP_INTERRUPT11),

    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,EMPTY_CLEAR),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,AGED_PACKET),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,MAX_BUFF_REACHED),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,PORT_RX_BUFFERS_CNT_UNDERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,PORT_RX_BUFFERS_CNT_OVERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,GE_CNT_UNDERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,GE_CNT_OVERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,XG_CNT_UNDERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,XG_CNT_OVERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,GLOBAL_CNT_UNDERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,GLOBAL_CNT_OVERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,TRIGGER_AGING_DONE),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,LL_PORT1_PARITY_ERROR),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,LL_PORT2_PARITY_ERROR),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,CNRL_MEM_PARITY_ERROR),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,MC_CNT_PARITY_ERROR),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,WRONG_SRC_PORT),
    PRV_CPSS_REDEF_MAC(XCAT2,BM_MC_INC_OVERFLOW,BM0_MC_INC_OVERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,MC_INC_UNDERRUN),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM0,PORT_RX_FULL),

    PRV_CPSS_REDEF_PORT0_23_MAC(
        XCAT2,BM_MAX_BUFF_REACHED,BM1_MAX_BUFF_REACHED),
    PRV_CPSS_REDEF_PORT24_27_MAC(
        XCAT2,BM_MAX_BUFF_REACHED,BM1_MAX_BUFF_REACHED),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM1,MAX_BUFF_REACHED_GIG_PORTS),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM1,MAX_BUFF_REACHED_HG_PORTS),
    PRV_CPSS_REDEF2_MAC(XCAT2,BM,BM1,MAX_BUFF_REACHED_PORT_CPU_63),

    PRV_CPSS_REDEF12_MAC(XCAT2,BUF,MEM,MEM_REG0,MAC_ERROR),

    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(LINK_STATUS_CHANGED),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(AN_COMPLETED),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(RX_FIFO_OVERRUN),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(TX_UNDERRUN),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(ADDRESS_OUT_OF_RANGE),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(SYNC_STATUS_CHANGED),
    PRV_CPSS_XCAT2_REDEF1_NETW_PORT0_23_MAC(PRBS_ERROR, PRBS_ERROR_PORT),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(TX_FIFO_OVERRUN),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(TX_FIFO_UNDERRUN),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(PRBS_ERROR_QSGMII),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(PCS_RX_PATH_RCV_LPI),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(PCS_TX_PATH_RCV_LPI),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(MAC_RX_PATH_RCV_LPI),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(MIB_COUNTERS_DONE),
    PRV_CPSS_XCAT2_REDEF_NETW_PORT0_23_MAC(MIB_COUNTERS_WRAPAROUND),

    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(LINK_STATUS_CHANGED),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(AN_COMPLETED),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(RX_FIFO_OVERRUN),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(TX_UNDERRUN),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(ADDRESS_OUT_OF_RANGE),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(SYNC_STATUS_CHANGED),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(PRBS_ERROR),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(TX_FIFO_OVERRUN),
    PRV_CPSS_XCAT2_REDEF_CPU_PORT_MAC(TX_FIFO_UNDERRUN),

    PRV_CPSS_REDEF_PORT0_23_MAC(XCAT2,TQ_TXQ2_FLUSH,TXQ_FLUSH),
    PRV_CPSS_REDEF_PORT24_27_MAC(XCAT2,TQ_TXQ2_FLUSH,TXQ_FLUSH),
    PRV_CPSS_REDEF2_MAC(XCAT2,TQ_TXQ2,TXQ,FLUSH_PORT_CPU_63),

    PRV_CPSS_REDEF_PORT0_23_MAC(XCAT2,TQ_DESC_FULL,TXQ_FULL_DESC),
    PRV_CPSS_REDEF_PORT24_27_MAC(XCAT2,TQ_DESC_FULL,TXQ_FULL_DESC),

    PRV_CPSS_REDEF_MAC(
        XCAT2,TQ_SNIFF_DESC_DROP,TXQ_GEN_EGR_MIRROR_DESC_DROP),
    PRV_CPSS_REDEF2_MAC(XCAT2,TQ,TXQ_GEN,BAD_ADDR),
    PRV_CPSS_REDEF2_MAC(XCAT2,TQ,TXQ_GEN,TOTAL_DESC_UNDERFLOW),
    PRV_CPSS_REDEF2_MAC(XCAT2,TQ,TXQ_GEN,TOTAL_BUFF_UNDERFLOW),
    PRV_CPSS_REDEF2_MAC(XCAT2,TQ,TXQ_GEN,DESC_FULL),
    PRV_CPSS_REDEF2_MAC(XCAT2,TQ,TXQ_GEN,BUF_FULL),

    PRV_CPSS_REDEF_MAC(
        XCAT2,TQ_GIGA_FIFO_FULL,TXQ_FULL_DESC_GIGA_MULTICAST),
    PRV_CPSS_REDEF_MAC(
        XCAT2,TQ_MC_DESC_FULL,TXQ_FULL_DESC_MULTICAST_LIMIT_REACHED),
    PRV_CPSS_REDEF_MAC(
        XCAT2,TQ_XG_MC_FIFO_FULL,TXQ_FULL_DESC_XG_MULTICAST),

    PRV_CPSS_REDEF_PORT0_23_MAC(XCAT2,MAC_SFLOW,PREEGR_INGR_STC),
    PRV_CPSS_REDEF_PORT24_27_MAC(XCAT2,MAC_SFLOW,PREEGR_INGR_STC),

    PRV_CPSS_REDEF_PORT0_23_MAC(XCAT2,EGRESS_STC,TXQ_EGRESS_STC),
    PRV_CPSS_REDEF_PORT24_27_MAC(XCAT2,EGRESS_STC,TXQ_EGRESS_STC),

    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(LINK_STATUS_CHANGED),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(AN_COMPLETED),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(RX_FIFO_OVERRUN),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(TX_UNDERRUN),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(ADDRESS_OUT_OF_RANGE),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(SYNC_STATUS_CHANGED),
    PRV_CPSS_XCAT2_REDEF1_STACK_PORT24_27_MAC(PRBS_ERROR, PRBS_ERROR_PORT),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(TX_FIFO_OVERRUN),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(TX_FIFO_UNDERRUN),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(PRBS_ERROR_QSGMII),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(PCS_RX_PATH_RCV_LPI),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(PCS_TX_PATH_RCV_LPI),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(MAC_RX_PATH_RCV_LPI),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(MIB_COUNTERS_DONE),
    PRV_CPSS_XCAT2_REDEF_STACK_PORT24_27_MAC(MIB_COUNTERS_WRAPAROUND),

    PRV_CPSS_REDEF2_MAC(XCAT2,EB,BRIDGE,NA_FIFO_FULL),
    PRV_CPSS_REDEF_MAC(
        XCAT2,EB_ADDR_OUT_RANGE,BRIDGE_ADDR_OUT_OF_RANGE),
    PRV_CPSS_REDEF_MAC(
        XCAT2,BRIDGE_ADDR_OUT_RANGE,BRIDGE_ADDR_OUT_OF_RANGE),
    PRV_CPSS_REDEF2_MAC(XCAT2,EB,BRIDGE,SECURITY_BREACH_UPDATE),

    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,NUM_OF_HOP_EXP),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,NA_LEARNED),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,NA_NOT_LEARNED),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,AGE_VIA_TRIGGER_ENDED),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,UPDATE_FROM_CPU_DONE),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,MESSAGE_TO_CPU_READY),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,NA_SELF_LEARNED),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,NA_FROM_CPU_LEARNED),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,NA_FROM_CPU_DROPPED),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,AGED_OUT),
    PRV_CPSS_REDEF2_MAC(XCAT2,MAC,FDB_MAC,FIFO_2_CPU_EXCEEDED),

    PRV_CPSS_REDEF2_MAC(XCAT2,EB,MISC_EB,AUQ_OVER),
    PRV_CPSS_REDEF2_MAC(XCAT2,EB,MISC_EB,AUQ_ALMOST_FULL),
    PRV_CPSS_REDEF2_MAC(XCAT2,EB,MISC_EB,AUQ_FULL),
    PRV_CPSS_REDEF2_MAC(XCAT2,EB,MISC_EB,AUQ_PENDING),
    PRV_CPSS_REDEF2_MAC(XCAT2,EB,MISC_EB,FUQ_FULL),
    PRV_CPSS_REDEF2_MAC(XCAT2,EB,MISC_EB,FUQ_PENDING),

    PRV_CPSS_XCAT2_REDEF1_QUEUE0_7_MAC(TX_BUFFER,TX_SDMA_BUFFER),
    PRV_CPSS_XCAT2_REDEF1_QUEUE0_7_MAC(TX_ERR,TX_SDMA_ERR),
    PRV_CPSS_XCAT2_REDEF1_QUEUE0_7_MAC(TX_END,TX_SDMA_END),
    PRV_CPSS_XCAT2_REDEF1_QUEUE0_7_MAC(RX_BUFFER,RX_SDMA_BUFFER),
    PRV_CPSS_XCAT2_REDEF1_QUEUE0_7_MAC(RX_ERR,RX_SDMA_ERR),

    PRV_CPSS_REDEF_MAC(
        XCAT2,SDMA_RX_ERROR_RES_CNT_OFF,RX_SDMA_ERROR_RES_CNT_OVERFLOW),
    PRV_CPSS_REDEF_MAC(
        XCAT2,SDMA_RX_BYTE_CNT_OFF,RX_SDMA_BYTE_CNT_OVERFLOW),
    PRV_CPSS_REDEF_MAC(
        XCAT2,SDMA_RX_PACKET_CNT_OFF,RX_SDMA_PACKET_CNT_OVERFLOW),

    PRV_CPSS_XCAT2_REDEF_MIB_GOP0_3_MAC(ADDRESS_OUT_OF_RANGE),
    PRV_CPSS_XCAT2_REDEF_MIB_GOP0_3_MAC(COUNT_EXPIRED),
    PRV_CPSS_XCAT2_REDEF_MIB_GOP0_3_MAC(COUNT_COPY_DONE),

    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_1_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_1_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_2_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_2_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_3_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_3_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_4_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_4_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_5_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_5_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_6_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_6_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_7_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_7_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_8_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_8_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_9_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_9_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_10_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_10_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_11_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_11_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_12_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_12_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_13_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_13_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_14_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_14_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_15_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_15_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_16_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_16_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_17_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_17_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_18_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_18_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_19_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_19_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_20_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_20_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_21_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_21_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_22_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_22_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_23_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_23_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_24_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_24_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_25_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_25_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_26_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_26_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_27_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_27_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_28_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_28_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_29_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_29_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_30_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_30_PKT_DROP_E,
    PRV_CPSS_XCAT2_SCT_RATE_LIMITER_31_PKT_DROP_E =
        PRV_CPSS_XCAT2_PREEGR_STC_RATE_LIM_31_PKT_DROP_E,

    PRV_CPSS_REDEF2_MAC(XCAT2,BUF_MEM,BUF_MEM_REG1,BANK0_ONE_ECC_ERROR),
    PRV_CPSS_REDEF2_MAC(
        XCAT2,BUF_MEM,BUF_MEM_REG1,BANK0_TWO_OR_MORE_ECC_ERRORS),
    PRV_CPSS_REDEF2_MAC(XCAT2,BUF_MEM,BUF_MEM_REG1,BANK1_ONE_ECC_ERROR),
    PRV_CPSS_REDEF2_MAC(
        XCAT2,BUF_MEM,BUF_MEM_REG1,BANK1_TWO_OR_MORE_ECC_ERRORS),

    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK0_E =
        PRV_CPSS_XCAT2_CNC_BLOCK0_WRAPAROUND_E,
    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK1_E =
        PRV_CPSS_XCAT2_CNC_BLOCK1_WRAPAROUND_E,
    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK2_E =
        PRV_CPSS_XCAT2_CNC_BLOCK2_WRAPAROUND_E,
    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK3_E =
        PRV_CPSS_XCAT2_CNC_BLOCK3_WRAPAROUND_E,
    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK4_E =
        PRV_CPSS_XCAT2_CNC_BLOCK4_WRAPAROUND_E,
    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK5_E =
        PRV_CPSS_XCAT2_CNC_BLOCK5_WRAPAROUND_E,
    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK6_E =
        PRV_CPSS_XCAT2_CNC_BLOCK6_WRAPAROUND_E,
    PRV_CPSS_XCAT2_CNC_WRAPAROUND_BLOCK7_E =
        PRV_CPSS_XCAT2_CNC_BLOCK7_WRAPAROUND_E,

    PRV_CPSS_XCAT2_REDEF_PLR_MAC(DATA_ERROR),
    PRV_CPSS_XCAT2_REDEF_PLR_MAC(ADDRESS_OUT_OF_MEMORY),
    PRV_CPSS_XCAT2_REDEF_PLR_MAC(IPFIX_WRAP_AROUND),
    PRV_CPSS_XCAT2_REDEF_PLR_MAC(IPFIX_ALARM),
    PRV_CPSS_XCAT2_REDEF_PLR_MAC(IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND),

    PRV_CPSS_XCAT2_PCL_ADDR_OUT_RANGE_E =
        PRV_CPSS_XCAT2_IPCL_MG_ADDR_OUT_OF_RANGE_E,
    PRV_CPSS_XCAT2_REDEF_IPCL_MAC(TCAM_ACCESS_DATA_ERROR),
    PRV_CPSS_XCAT2_REDEF_IPCL_MAC(ACTION1_TCAM_ACCESS_DATA_ERROR),
    PRV_CPSS_XCAT2_REDEF_IPCL_MAC(ACTION2_TCAM_ACCESS_DATA_ERROR),
    PRV_CPSS_XCAT2_REDEF_IPCL_MAC(ACTION3_TCAM_ACCESS_DATA_ERROR),
    PRV_CPSS_XCAT2_REDEF_IPCL_MAC(FIFO_FULL),

    PRV_CPSS_XCAT2_TCC_LOWER_SUM_TCAM_ERROR_DETECTED_E =
        PRV_CPSS_XCAT2_TCC_LOWER_UNIT_TCAM_ERROR_DETECTED_E,
    PRV_CPSS_XCAT2_TCC_UPPER_SUM_TCAM_ERROR_DETECTED_E =
        PRV_CPSS_XCAT2_TCC_UPPER_UNIT_TCAM_ERROR_DETECTED_E,
    PRV_CPSS_XCAT2_TCC_LOWER_SUM_TCAM_BIST_FAILED_E =
        PRV_CPSS_XCAT2_TCC_LOWER_UNIT_TCAM_BIST_FAILED_E,
    PRV_CPSS_XCAT2_TCC_UPPER_SUM_TCAM_BIST_FAILED_E =
        PRV_CPSS_XCAT2_TCC_UPPER_UNIT_TCAM_BIST_FAILED_E,

    XCAT2_END_E/* must be last*/
};

enum LION_B_FIX_NAMES_ENT{
    PRV_CPSS_LION_B_GPP_INTERRUPT1_E  =  PRV_CPSS_LION_B_MISC_GPP_0_E ,
    PRV_CPSS_LION_B_GPP_INTERRUPT2_E  ,
    PRV_CPSS_LION_B_GPP_INTERRUPT3_E  ,
    PRV_CPSS_LION_B_GPP_INTERRUPT4_E  ,
    PRV_CPSS_LION_B_GPP_INTERRUPT5_E  ,
    PRV_CPSS_LION_B_GPP_INTERRUPT6_E  ,
    PRV_CPSS_LION_B_GPP_INTERRUPT7_E  ,
    PRV_CPSS_LION_B_GPP_INTERRUPT8_E  ,

    PRV_CPSS_LION_B_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E = PRV_CPSS_LION_B_BM_0_PORT_RX_BUFFERS_CNT_UNDERRUN_E,
    PRV_CPSS_LION_B_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E,
    PRV_CPSS_LION_B_BM_GE_CNT_UNDERRUN_E =              PRV_CPSS_LION_B_BM_0_GE_CNT_UNDERRUN_E,
    PRV_CPSS_LION_B_BM_GE_CNT_OVERRUN_E,
    PRV_CPSS_LION_B_BM_XG_CNT_UNDERRUN_E =              PRV_CPSS_LION_B_BM_0_XG_CNT_UNDERRUN_E,
    PRV_CPSS_LION_B_BM_XG_CNT_OVERRUN_E,
    PRV_CPSS_LION_B_BM_GLOBAL_CNT_UNDERRUN_E =          PRV_CPSS_LION_B_BM_0_GLOBAL_CNT_UNDERRUN_E,
    PRV_CPSS_LION_B_BM_GLOBAL_CNT_OVERRUN_E,

    PRV_CPSS_LION_B_CPU_PORT_SUM_TQ_TXQ2_FLUSH_E =      PRV_CPSS_LION_B_TXQ_DQ_FLUSH_DONE_PORT15_E,

    PRV_CPSS_LION_B_CPU_PORT_SUM_BM_MAX_BUFF_REACHED_E =PRV_CPSS_LION_B_BM_1_MAX_BUFF_REACHED_PORT_CPU_63_E,


    /* start GE port interrupts */
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(LINK_STATUS_CHANGED),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(AN_COMPLETED),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(RX_FIFO_OVERRUN),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(TX_FIFO_UNDERRUN),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(TX_FIFO_OVERRUN),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(TX_UNDERRUN),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(ADDRESS_OUT_OF_RANGE),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(SYNC_STATUS_CHANGED),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_TO_END(PRBS_ERROR),

    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_BY_PREFIX(TQ_TXQ2_FLUSH,TXQ_DQ_FLUSH_DONE),
    LION_B_CPU_AND_OTHER_PORTS_INT_CONVERT_PORT_BY_PREFIX(BM_MAX_BUFF_REACHED,BM_1_MAX_BUFF_REACHED),

    LION_B_PORTS_INT_CONVERT_PORT_BY_BY_PREFIX(TQ_DESC_FULL,TXQ_QUEUE_PORT_DESC_FULL),
    LION_B_PORTS_INT_CONVERT_PORT_BY_BY_PREFIX(EGRESS_STC,TXQ_DQ_ERGESS_STC_SAMPLE_LOADED),

    LION_B_PORTS_INT_CONVERT_PORT_BY_PARTS(MAC_SFLOW,INGRESS_STC,INGRESS_SAMPLE_LOADED),
    /* end GE port interrupts */

    /* start XG port interrupts */
    LION_B_XG_PORTS_INT_CONVERT_PORT_TO_END(LINK_STATUS_CHANGED),
    LION_B_XG_PORTS_INT_CONVERT_PORT_TO_END(RX_FIFO_OVERRUN),
    LION_B_XG_PORTS_INT_CONVERT_PORT_TO_END(TX_UNDERRUN),
    LION_B_XG_PORTS_INT_CONVERT_PORT_TO_END(ADDRESS_OUT_OF_RANGE),
    LION_B_XG_PORTS_INT_CONVERT_PORT_TO_END(FC_STATUS_CHANGED),
    LION_B_XG_PORTS_INT_CONVERT_PORT_TO_END(NO_BUFF_PACKET_DROP),
    LION_B_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(ILLEGAL_SEQUENCE,UNKNOWN_SEQUENCE),
    LION_B_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(FAULT_TYPE_CHANGE,FAULT_CHANGED),
    LION_B_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(COUNT_EXPIRED,XG_COUNT_EXPIRED),
    LION_B_XG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(COUNT_COPY_DONE,XG_COUNT_COPY_DONE),
    /* end XG port interrupts */

    /* start XG port - XPCS interrupts */
    LION_B_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(DESKEW_TIMEOUT),
/* internal--> LION_B_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(DETECTED_COLUMN_IIAII),*/
/* internal--> LION_B_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(DESKEW_ERROR),         */
    LION_B_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(PPM_FIFO_UNDERRUN),
    LION_B_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(PPM_FIFO_OVERRUN),
    LION_B_XG_PORTS_XPCS_INT_CONVERT_PORT_TO_END(LINK_STATUS_CHANGED),
    /* end XG port - XPCS interrupts */

    /* start per port per lane */
    LION_B_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(PRBS_ERROR),
/* internal--> LION_B_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(SIGNAL_DETECT_CHANGED), */
/* internal--> LION_B_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(SYNC_STATUS_CHANGED),   */
/* internal--> LION_B_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(DETECTED_IIAII),        */
/* internal--> LION_B_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(DISPARITY_ERROR),       */
/* internal--> LION_B_XG_PORTS_LANES_INT_CONVERT_PORT_TO_END(SYMBOL_ERROR),          */
    /* end per port per lane */


    /* start XLG port interrupts */
    LION_B_XLG_PORTS_INT_CONVERT_PORT_TO_END(LINK_STATUS_CHANGED),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_TO_END(RX_FIFO_OVERRUN),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_TO_END(TX_UNDERRUN),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_TO_END(ADDRESS_OUT_OF_RANGE),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_TO_END(FC_STATUS_CHANGED),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_TO_END(NO_BUFF_PACKET_DROP),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(ILLEGAL_SEQUENCE,UNKNOWN_SEQUENCE),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(FAULT_TYPE_CHANGE,FAULT_CHANGED),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(COUNT_EXPIRED,XG_COUNT_EXPIRED),
    LION_B_XLG_PORTS_INT_CONVERT_PORT_BY_BY_POSTFIX(COUNT_COPY_DONE,XG_COUNT_COPY_DONE),
    /* end XLG port interrupts */

    /* start XLG port - 40G PCS interrupts */
    LION_B_XLG_PORTS_PCS_INT_CONVERT_PORT_TO_END(ALIGN_LOCK_LOST),
    /* end XLG port - 40G PCS interrupts */

    PRV_CPSS_LION_B_BM_LL_PORT1_PARITY_ERROR_E =    PRV_CPSS_LION_B_BM_0_LL_PORT1_PARITY_ERROR_E,
    PRV_CPSS_LION_B_BM_LL_PORT2_PARITY_ERROR_E =    PRV_CPSS_LION_B_BM_0_LL_PORT2_PARITY_ERROR_E,
    PRV_CPSS_LION_B_BM_CNRL_MEM_PARITY_ERROR_E =    PRV_CPSS_LION_B_BM_0_CNRL_MEM_PARITY_ERROR_E,
    PRV_CPSS_LION_B_BM_MC_CNT_PARITY_ERROR_E   =    PRV_CPSS_LION_B_BM_0_MC_CNT_PARITY_ERROR_E,
    PRV_CPSS_LION_B_BM_PORT_RX_FULL_E =             PRV_CPSS_LION_B_BM_0_PORT_RX_FULL_E,
    PRV_CPSS_LION_B_BM_EMPTY_CLEAR_E =              PRV_CPSS_LION_B_BM_0_EMPTY_CLEAR_E,
    PRV_CPSS_LION_B_BM_AGED_PACKET_E =              PRV_CPSS_LION_B_BM_0_AGED_PACKET_E,
    PRV_CPSS_LION_B_BM_MAX_BUFF_REACHED_E =         PRV_CPSS_LION_B_BM_0_MAX_BUFF_REACHED_E,
    PRV_CPSS_LION_B_BM_WRONG_SRC_PORT_E =           PRV_CPSS_LION_B_BM_0_WRONG_SRC_PORT_E,
    PRV_CPSS_LION_B_BM_MC_INC_OVERFLOW_E =          PRV_CPSS_LION_B_BM_0_MC_INC_OVERFLOW_E,
    PRV_CPSS_LION_B_BM_MC_INC_UNDERRUN_E =          PRV_CPSS_LION_B_BM_0_MC_INC_UNDERRUN_E,

    PRV_CPSS_LION_B_MISC_CPU_PORT_RX_OVERRUN_E =    PRV_CPSS_LION_B_CPU_PORT_SUM_RX_FIFO_OVERRUN_E,


    PRV_CPSS_LION_B_TXQ_GEN_DESC_FULL_E =           PRV_CPSS_LION_B_TXQ_QUEUE_GEN_GLOBAL_DESC_FULL_E,
    PRV_CPSS_LION_B_TXQ_GEN_BUF_FULL_E =            PRV_CPSS_LION_B_TXQ_QUEUE_GEN_GLOBAL_BUFF_FULL_E,
    PRV_CPSS_LION_B_TXQ_GEN_BAD_ADDR_E =            PRV_CPSS_LION_B_TXQ_QUEUE_GEN_WRONG_ADDR_E,

    PRV_CPSS_LION_B_TQ_SNIFF_DESC_DROP_E =          PRV_CPSS_LION_B_TXQ_DQ_GEN_EGRESS_MIRROR_DESC_DROP_E,

    PRV_CPSS_LION_B_BUF_MEM_MAC_ERROR_E =           PRV_CPSS_LION_B_LAST_INT_E, CPSS_TBD_BOOKMARK /* dummy assignment */


    PRV_CPSS_LION_B_EB_NA_FIFO_FULL_E =             PRV_CPSS_LION_B_BRIDGE_UPDATE_FIFO_FULL_E,
    PRV_CPSS_LION_B_BRIDGE_ADDR_OUT_RANGE_E =       PRV_CPSS_LION_B_BRIDGE_WRONG_ADDR_E,
    PRV_CPSS_LION_B_EB_SECURITY_BREACH_UPDATE_E =   PRV_CPSS_LION_B_BRIDGE_UPDATE_SECURITY_BREACH_E,

    PRV_CPSS_LION_B_MAC_NUM_OF_HOP_EXP_E =          PRV_CPSS_LION_B_FDB_NUM_OF_HOP_EX_P_E,
    PRV_CPSS_LION_B_MAC_NA_LEARNED_E =              PRV_CPSS_LION_B_FDB_NA_LEARNT_E,
    PRV_CPSS_LION_B_MAC_NA_NOT_LEARNED_E =          PRV_CPSS_LION_B_FDB_NA_NOT_LEARNT_E,
    PRV_CPSS_LION_B_MAC_AGE_VIA_TRIGGER_ENDED_E =   PRV_CPSS_LION_B_FDB_AGE_VIA_TRIGGER_ENDED_E,
    PRV_CPSS_LION_B_MAC_UPDATE_FROM_CPU_DONE_E =    PRV_CPSS_LION_B_FDB_AU_PROC_COMPLETED_E,
    PRV_CPSS_LION_B_MAC_MESSAGE_TO_CPU_READY_E =    PRV_CPSS_LION_B_FDB_AU_MSG_TOCPU_READY_E,
    PRV_CPSS_LION_B_MAC_NA_SELF_LEARNED_E =         PRV_CPSS_LION_B_FDB_NA_SELF_LEARNT_E,
    PRV_CPSS_LION_B_MAC_NA_FROM_CPU_LEARNED_E =     PRV_CPSS_LION_B_FDB_N_AFROM_CPU_LEARNED_E,
    PRV_CPSS_LION_B_MAC_NA_FROM_CPU_DROPPED_E =     PRV_CPSS_LION_B_FDB_N_AFROM_CPU_DROPPED_E,
    PRV_CPSS_LION_B_MAC_AGED_OUT_E =                PRV_CPSS_LION_B_FDB_AGED_OUT_E,
    PRV_CPSS_LION_B_MAC_FIFO_2_CPU_EXCEEDED_E =     PRV_CPSS_LION_B_FDB_AU_FIFO_TO_CPU_IS_FULL_E,

    PRV_CPSS_LION_B_EB_AUQ_PENDING_E =              PRV_CPSS_LION_B_MISC_AUQ_PENDING_E,
    PRV_CPSS_LION_B_EB_AUQ_FULL_E =                 PRV_CPSS_LION_B_MISC_AU_QUEUE_FULL_E,
    PRV_CPSS_LION_B_EB_AUQ_OVER_E =                 PRV_CPSS_LION_B_MISC_AUQ_OVERRUN_E,
    PRV_CPSS_LION_B_EB_AUQ_ALMOST_FULL_E =          PRV_CPSS_LION_B_MISC_AUQ_ALMOST_FULL_E,
    PRV_CPSS_LION_B_EB_FUQ_PENDING_E =              PRV_CPSS_LION_B_MISC_FUQ_PENDING_E,
    PRV_CPSS_LION_B_EB_FUQ_FULL_E =                 PRV_CPSS_LION_B_MISC_FU_QUEUE_FULL_E,

    PRV_CPSS_LION_B_TX_BUFFER_QUEUE0_E = PRV_CPSS_LION_B_TRANSMIT_SDMA_TX_BUFFER_QUEUE0_E,
    PRV_CPSS_LION_B_TX_BUFFER_QUEUE1_E ,
    PRV_CPSS_LION_B_TX_BUFFER_QUEUE2_E ,
    PRV_CPSS_LION_B_TX_BUFFER_QUEUE3_E ,
    PRV_CPSS_LION_B_TX_BUFFER_QUEUE4_E ,
    PRV_CPSS_LION_B_TX_BUFFER_QUEUE5_E ,
    PRV_CPSS_LION_B_TX_BUFFER_QUEUE6_E ,
    PRV_CPSS_LION_B_TX_BUFFER_QUEUE7_E ,

    PRV_CPSS_LION_B_TX_ERR_QUEUE0_E = PRV_CPSS_LION_B_TRANSMIT_SDMA_TX_ERROR_QUEUE0_E,
    PRV_CPSS_LION_B_TX_ERR_QUEUE1_E ,
    PRV_CPSS_LION_B_TX_ERR_QUEUE2_E ,
    PRV_CPSS_LION_B_TX_ERR_QUEUE3_E ,
    PRV_CPSS_LION_B_TX_ERR_QUEUE4_E ,
    PRV_CPSS_LION_B_TX_ERR_QUEUE5_E ,
    PRV_CPSS_LION_B_TX_ERR_QUEUE6_E ,
    PRV_CPSS_LION_B_TX_ERR_QUEUE7_E ,

    PRV_CPSS_LION_B_TX_END_QUEUE0_E = PRV_CPSS_LION_B_TRANSMIT_SDMA_TX_END_QUEUE0_E,
    PRV_CPSS_LION_B_TX_END_QUEUE1_E ,
    PRV_CPSS_LION_B_TX_END_QUEUE2_E ,
    PRV_CPSS_LION_B_TX_END_QUEUE3_E ,
    PRV_CPSS_LION_B_TX_END_QUEUE4_E ,
    PRV_CPSS_LION_B_TX_END_QUEUE5_E ,
    PRV_CPSS_LION_B_TX_END_QUEUE6_E ,
    PRV_CPSS_LION_B_TX_END_QUEUE7_E ,

    PRV_CPSS_LION_B_RX_BUFFER_QUEUE0_E = PRV_CPSS_LION_B_RECEIVE_SDMA_RX_BUFFER_QUEUE0_E,
    PRV_CPSS_LION_B_RX_BUFFER_QUEUE1_E ,
    PRV_CPSS_LION_B_RX_BUFFER_QUEUE2_E ,
    PRV_CPSS_LION_B_RX_BUFFER_QUEUE3_E ,
    PRV_CPSS_LION_B_RX_BUFFER_QUEUE4_E ,
    PRV_CPSS_LION_B_RX_BUFFER_QUEUE5_E ,
    PRV_CPSS_LION_B_RX_BUFFER_QUEUE6_E ,
    PRV_CPSS_LION_B_RX_BUFFER_QUEUE7_E ,

    PRV_CPSS_LION_B_RX_ERR_QUEUE0_E = PRV_CPSS_LION_B_RECEIVE_SDMA_RX_ERROR_QUEUE0_E,
    PRV_CPSS_LION_B_RX_ERR_QUEUE1_E ,
    PRV_CPSS_LION_B_RX_ERR_QUEUE2_E ,
    PRV_CPSS_LION_B_RX_ERR_QUEUE3_E ,
    PRV_CPSS_LION_B_RX_ERR_QUEUE4_E ,
    PRV_CPSS_LION_B_RX_ERR_QUEUE5_E ,
    PRV_CPSS_LION_B_RX_ERR_QUEUE6_E ,
    PRV_CPSS_LION_B_RX_ERR_QUEUE7_E ,

    PRV_CPSS_LION_B_SDMA_RX_ERROR_RES_CNT_OFF_E =       PRV_CPSS_LION_B_RECEIVE_SDMA_RESOURCE_ERROR_CNT_OF_E,
    PRV_CPSS_LION_B_SDMA_RX_BYTE_CNT_OFF_E =            PRV_CPSS_LION_B_RECEIVE_SDMA_BYTE_CNT_OF_E,
    PRV_CPSS_LION_B_SDMA_RX_PACKET_CNT_OFF_E =          PRV_CPSS_LION_B_RECEIVE_SDMA_PACKET_CNT_OF_E,

    PRV_CPSS_LION_B_PEX_DL_DOWN_TX_ACC_ERR_E           = PRV_CPSS_LION_B_PEX_DL_DWN_TX_ACC_ERR_E ,
    PRV_CPSS_LION_B_PEX_MASTER_DISABLED_E              = PRV_CPSS_LION_B_PEX_MDIS_E              ,
    PRV_CPSS_LION_B_PEX_ERROR_WR_TO_REG_E              = PRV_CPSS_LION_B_PEX_ERR_WR_TO_REG_E     ,
    PRV_CPSS_LION_B_PEX_HIT_DEFAULT_WIN_ERR_E          = PRV_CPSS_LION_B_PEX_HIT_DFLT_WIN_ERR_E  ,
    PRV_CPSS_LION_B_PEX_COR_ERROR_DET_E                = PRV_CPSS_LION_B_PEX_COR_ERR_DET_E       ,
    PRV_CPSS_LION_B_PEX_NON_FATAL_ERROR_DET_E          = PRV_CPSS_LION_B_PEX_NF_ERR_DET_E        ,
    PRV_CPSS_LION_B_PEX_FATAL_ERROR_DET_E              = PRV_CPSS_LION_B_PEX_F_ERR_DET_E         ,
    PRV_CPSS_LION_B_PEX_RCV_ERROR_FATAL_E              = PRV_CPSS_LION_B_PEX_RCV_ERR_FATAL_E     ,
    PRV_CPSS_LION_B_PEX_RCV_ERROR_NON_FATAL_E          = PRV_CPSS_LION_B_PEX_RCV_ERR_NON_FATAL_E ,
    PRV_CPSS_LION_B_PEX_RCV_ERROR_COR_E                = PRV_CPSS_LION_B_PEX_RCV_ERR_COR_E       ,
    PRV_CPSS_LION_B_PEX_PEX_SLAVE_HOT_RESET_E          = PRV_CPSS_LION_B_PEX_PEX_SLV_HOT_RESET_E ,
    PRV_CPSS_LION_B_PEX_PEX_SLAVE_DISABLE_LINK_E       = PRV_CPSS_LION_B_PEX_PEX_SLV_DIS_LINK_E  ,
    PRV_CPSS_LION_B_PEX_PEX_SLAVE_LOOPBACK_E           = PRV_CPSS_LION_B_PEX_PEX_SLV_LB_E        ,


    PRV_CPSS_LION_B_SCT_RATE_LIMITER_1_PKT_DROP_E = PRV_CPSS_LION_B_STC_RATE_LIMITERS_STC_RATE_LIMITER_1_PKT_DROPPED_E,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_2_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_3_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_4_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_5_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_6_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_7_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_8_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_9_PKT_DROP_E   ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_10_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_11_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_12_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_13_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_14_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_15_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_16_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_17_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_18_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_19_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_20_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_21_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_22_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_23_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_24_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_25_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_26_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_27_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_28_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_29_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_30_PKT_DROP_E  ,
    PRV_CPSS_LION_B_SCT_RATE_LIMITER_31_PKT_DROP_E  ,

    PRV_CPSS_LION_B_BUF_MEM_BANK0_ONE_ECC_ERROR_E =             PRV_CPSS_LION_B_MPPM_BK0_ECC_1_ERROR_E,
    PRV_CPSS_LION_B_BUF_MEM_BANK0_TWO_OR_MORE_ECC_ERRORS_E =    PRV_CPSS_LION_B_MPPM_BK0_ECC_2_OR_MORE_ERROR_E,
    PRV_CPSS_LION_B_BUF_MEM_BANK1_ONE_ECC_ERROR_E =             PRV_CPSS_LION_B_MPPM_BK1_ECC_1_ERROR_E,
    PRV_CPSS_LION_B_BUF_MEM_BANK1_TWO_OR_MORE_ECC_ERRORS_E =    PRV_CPSS_LION_B_MPPM_BK1_ECC_2_OR_MORE_ERROR_E,

    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK0_E = PRV_CPSS_LION_B_CNC_BLOCK0_WRAPAROUND_E,
    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK1_E,
    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK2_E,
    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK3_E,
    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK4_E,
    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK5_E,
    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK6_E,
    PRV_CPSS_LION_B_CNC_WRAPAROUND_BLOCK7_E,


    PRV_CPSS_LION_B_TTI_CPU_ADDRESS_OUT_OF_RANGE_E =        PRV_CPSS_LION_B_TTI_ENG_WRONG_ADDR_E,
    PRV_CPSS_LION_B_TTI_ACCESS_DATA_ERROR_E =               PRV_CPSS_LION_B_TTI_ENG_ACCESS_DATA_ERROR_E,

    PRV_CPSS_LION_B_IPLR0_SUM_DATA_ERROR_E                             =    PRV_CPSS_LION_B_IPLR0_DATA_ERROR_E                             ,
    PRV_CPSS_LION_B_IPLR0_SUM_ADDRESS_OUT_OF_MEMORY_E                  ,
    PRV_CPSS_LION_B_IPLR0_SUM_IPFIX_WRAP_AROUND_E                      ,
    PRV_CPSS_LION_B_IPLR0_SUM_IPFIX_ALARM_E                            ,
    PRV_CPSS_LION_B_IPLR0_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E ,

    PRV_CPSS_LION_B_IPLR1_SUM_DATA_ERROR_E                             =    PRV_CPSS_LION_B_IPLR1_DATA_ERROR_E                             ,
    PRV_CPSS_LION_B_IPLR1_SUM_ADDRESS_OUT_OF_MEMORY_E                  ,
    PRV_CPSS_LION_B_IPLR1_SUM_IPFIX_WRAP_AROUND_E                      ,
    PRV_CPSS_LION_B_IPLR1_SUM_IPFIX_ALARM_E                            ,
    PRV_CPSS_LION_B_IPLR1_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E ,

    PRV_CPSS_LION_B_EPLR_SUM_DATA_ERROR_E                             =    PRV_CPSS_LION_B_EPLR_DATA_ERROR_E                             ,
    PRV_CPSS_LION_B_EPLR_SUM_ADDRESS_OUT_OF_MEMORY_E                  ,
    PRV_CPSS_LION_B_EPLR_SUM_IPFIX_WRAP_AROUND_E                      ,
    PRV_CPSS_LION_B_EPLR_SUM_IPFIX_ALARM_E                            ,
    PRV_CPSS_LION_B_EPLR_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E ,

    PRV_CPSS_LION_B_PCL_ADDR_OUT_RANGE_E =      PRV_CPSS_LION_B_POLICY_ENGINE_WRONG_ADDR_E,

    PRV_CPSS_LION_B_TCC_LOWER_SUM_TCAM_ERROR_DETECTED_E =   PRV_CPSS_LION_B_TCC_LOWER_GEN_TCAM_ERROR_DETECTED_E,
    PRV_CPSS_LION_B_TCC_LOWER_SUM_TCAM_BIST_FAILED_E =      PRV_CPSS_LION_B_TCC_LOWER_GEN_TCAM_BIST_FAILED_E,

    PRV_CPSS_LION_B_TCC_UPPER_SUM_TCAM_ERROR_DETECTED_E =   PRV_CPSS_LION_B_TCC_UPPER_GEN_TCAM_ERROR_DETECTED_E,
    PRV_CPSS_LION_B_TCC_UPPER_SUM_TCAM_BIST_FAILED_E =      PRV_CPSS_LION_B_TCC_UPPER_GEN_TCAM_BIST_FAILED_E,


    LION_B_END_E/* must be last*/

};

typedef struct{
    GT_U32  startIndex;
    GT_BOOL fullRegister;/* 'full register' or 'single bit' */
    struct{/* relevant only to 'full register' */
        GT_U32  nextPortIndex;/* the index of the next port */
        GT_U32  numRegisters;/* number of continues registers */
    }registerOffset;
}PER_PORT_INTERRUPTS_STC;

/* Lion B : array of interrupts that are 'per port'
    and need conversion from a 'per port group' extraData to global extraData .
*/
static PER_PORT_INTERRUPTS_STC lionBPerPortInterrupts[]=
{
    /*********************/
    /* register per port */
    /*********************/

    {
        PRV_CPSS_LION_B_XG_PORT0_SUM_XG_XPCS_SUMMARY_E,
        GT_TRUE,
        {   /*registerOffset*/
            PRV_CPSS_LION_B_XG_PORT1_SUM_XG_XPCS_SUMMARY_E, /*nextPortIndex*/
            0 /*numRegisters*/
        }
    },

    {
        PRV_CPSS_LION_B_PORT0_E,
        GT_TRUE,
        {   /*registerOffset*/
            PRV_CPSS_LION_B_PORT1_E, /*nextPortIndex*/
            0 /*numRegisters*/
        }
    },

    {
        PRV_CPSS_LION_B_XG_PORT0_PORT_E,
        GT_TRUE,
        {   /*registerOffset*/
            PRV_CPSS_LION_B_XG_PORT1_PORT_E, /*nextPortIndex*/
            0 /*numRegisters*/
        }
    },

    {
        PRV_CPSS_LION_B_XPCS_PORT0_E,
        GT_TRUE,
        {   /*registerOffset*/
            PRV_CPSS_LION_B_XPCS_PORT1_E, /*nextPortIndex*/
            0 /*numRegisters*/
        }
    },

    /****************/
    /* bit per port */
    /****************/
    { PRV_CPSS_LION_B_BM_1_MAX_BUFF_REACHED_PORT0_E,GT_FALSE,{0,0} },
    { PRV_CPSS_LION_B_TXQ_DQ_FLUSH_DONE_PORT0_E,GT_FALSE,{0,0}  },
    { PRV_CPSS_LION_B_TXQ_DQ_ERGESS_STC_SAMPLE_LOADED_PORT0_E,GT_FALSE,{0,0}  },
    { PRV_CPSS_LION_B_TXQ_QUEUE_PORT_DESC_FULL_PORT0_E,GT_FALSE,{0,0}  },
    { PRV_CPSS_LION_B_TXQ_QUEUE_PORT_BUFF_FULL_PORT0_E,GT_FALSE,{0,0}  },
    { PRV_CPSS_LION_B_INGRESS_STC_PORT0_INGRESS_SAMPLE_LOADED_E,GT_FALSE,{0,0} }
};

static GT_U32   lionBPerPortInterruptsSize = sizeof(lionBPerPortInterrupts)/sizeof(lionBPerPortInterrupts[0]);

/* Lion B : array of interrupts that are 'per port'
    and need conversion from a 'per port group' extraData to global extraData .
*/
static PER_PORT_INTERRUPTS_STC lionBPerPortPerLaneInterrupts[]=
{
    {
        PRV_CPSS_LION_B_XG_PORT0_LANE0_SUM_SUM_E,
        GT_TRUE,
        {   /*registerOffset*/
            PRV_CPSS_LION_B_XG_PORT1_LANE0_SUM_SUM_E /*nextPortIndex*/,
            6/* 6 lane registers per port */
        }
    }
};

static GT_U32   lionBPerPortPerLaneInterruptsSize = sizeof(lionBPerPortPerLaneInterrupts)/sizeof(lionBPerPortPerLaneInterrupts[0]);

/*
 * Typedef: enum UNI_EV_2_EV_OFFSET
 *
 * Enumeration: Lists the conversion table column data types.
 *
 * Enums:
 *      UNI_EV_E            - Unified event
 *      CHEETAH_E           - Cheetah event
 *      CHEETAH2_E          - Cheetah2 event
 *      CHEETAH3_E          - Cheetah3 event , and xcat A0
 *      XCAT_E              - xcat (A1) events
 *      XCAT2_E             - xcat2 events
 *      LION_B_E            - Lion B events
 *      EXT_DATA_E          - Additional data for unified event
 *      TABLE_ENTRY_SIZE_E  - Table size (MUST be last !!)
 *
 */
typedef enum
{
    UNI_EV_E,
    CHEETAH_E,
    CHEETAH2_E,
    CHEETAH3_E,
    XCAT_E,
    XCAT2_E,
    LION_B_E,
    EXT_DATA_E,
    TABLE_ENTRY_SIZE_E
} UNI_EV_2_EV_OFFSET;


/* build the name of the event :
    example : GEN_EV(CH,TQ_MC_FIFO_OVERRUN)
    target: PRV_CPSS_CH_TQ_MC_FIFO_OVERRUN_E
*/
#define GEN_EV(dev,event)          PRV_CPSS_##dev##_##event##_E

/* the name of the unify event - based on the event name */
#define UNI_GEN_EV(event)          CPSS_PP_##event##_E

/* build name of group of events: per port , per queue , per index
    example : GROUP_EV(CH,IPG_TOO_SMALL,PORT,3)
    target: PRV_CPSS_CH_IPG_TOO_SMALL_PORT3
*/
#define GROUP_EV(dev,event,group,index)   PRV_CPSS_##dev##_##event##_##group##index##_E

/* build name of group of events: per lane per port
    example: GROUP_2_EV(CH_HX,DETECTED_IIAII,LANE,0,PORT,25)
    target: PRV_CPSS_CH_HX_DETECTED_IIAII_LANE0_PORT25_E
*/
#define GROUP_2_EV(dev,event,group1,index1,group2,index2)   PRV_CPSS_##dev##_##event##_##group1##index1##_##group2##index2##_E

#define PREFIX_GROUP_EV_MAC(dev,event,group,index)   PRV_CPSS_##dev##_##group##index##_##event##_E

/* build name group - per port
    example : PORT_EV(CH,IPG_TOO_SMALL,3)
    target: PRV_CPSS_CH_IPG_TOO_SMALL_PORT3
*/
#define PORT_EV(dev,event,port)     GROUP_EV(dev,event,PORT,port)

/* build name group - per index
    example : INDEX_EV(CH,GPP_INTERRUPT,2)
    target: PRV_CPSS_CH_GPP_INTERRUPT_INDEX2
*/
#define INDEX_EV(dev,event,index)    PRV_CPSS_##dev##_##event##_##index##_E

/* build name group - per index
    example : INDEX_EV2(CH3,CNC_WRAPAROUND_BLOCK,0)
    target: PRV_CPSS_CH3_CNC_WRAPAROUND_BLOCK0
*/
#define INDEX_EV2(dev,event,index)    PRV_CPSS_##dev##_##event##index##_E


/* build name group - per queue
    example : QUEUE_EV(CH,TX_BUFFER,5)
    target: PRV_CPSS_CH_TX_BUFFER_QUEUE5
*/
#define QUEUE_EV(dev,event,queue)    GROUP_EV(dev,event,QUEUE,queue)

/* the name of the unify event , but specific per queue -
   based on the event name , and queue number
   example : UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(RX_BUFFER,6)
   target : CPSS_RX_BUFFER_QUEUE6
*/
#define UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(event,queue) CPSS_PP_##event##_QUEUE##queue##_E

/* build name group - per index
    example : INTERRUPT_EV(CH,GPP,2)
    target: PRV_CPSS_CH_GPP_INTERRUPT2
*/
#define INTERRUPT_EV(dev,event,index)    GROUP_EV(dev,event,INTERRUPT,index)

#define UNI_PORT_GEN_EV(event)     CPSS_PP_PORT_##event##_E

#define UNI_QUEUE_GEN_EV(event)    CPSS_PP_##event##_QUEUE_E

#define SCT_DROP_PKTS_EV(dev,event,index) PRV_CPSS_##dev##_##event##_##index##_##PKT_DROP##_E

#define BM_BANK_EV(dev,event,index) PREFIX_GROUP_EV_MAC(dev,event,BUF_MEM_BANK,index)

#define HGSMIB_PORT_EV(dev, ppEvent, port)  PREFIX_GROUP_EV_MAC(dev,ppEvent,HGSMIB_PORT_,port)

#define UNI_PORT_PCS_GEN_EV(event)     CPSS_PP_PORT_PCS_##event##_E

#define UNI_PORT_LANE_GEN_EV(event)    CPSS_PP_PORT_LANE_##event##_E

#define PORT_LANE_EV(dev,event,port,lane) GROUP_2_EV(dev,event,LANE,lane,PORT,port)

/* convert portGroupId to extData */
#define PORT_GROUP_ID_TO_EXT_DATA_CONVERT(portGroupId) ((portGroupId) << 16)



/* CPU_PORT exists on all devices */
#define CPU_PORT_EV_ALL(event)          \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         PORT_EV(CH,event,_CPU_63),      \
/*CHEETAH2_E, */         PORT_EV(CH2,event,_CPU_63),     \
/*CHEETAH3_E, */         PORT_EV(CH3,event,_CPU_63),     \
/*XCAT_E,     */         PORT_EV(XCAT,event,_CPU_63),    \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,_CPU_63),   \
/*LION_B_E,   */         PORT_EV(LION_B,event,_CPU_63),  \
/*EXT_DATA_E, */         63                              \
    }

/* CPU_PORT exists on all devices */
#define CPU_PORT_EV_ALL_UNTIL_XCAT(event)          \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         PORT_EV(CH,event,_CPU_63),      \
/*CHEETAH2_E, */         PORT_EV(CH2,event,_CPU_63),     \
/*CHEETAH3_E, */         PORT_EV(CH3,event,_CPU_63),     \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         63                              \
    }

/* CPU_PORT on all devices except cheetah3 */
#define CPU_PORT_EV_ALL_NO_CH3(event)   \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event),            \
/*CHEETAH_E,  */         PORT_EV(CH,event,_CPU_63),      \
/*CHEETAH2_E, */         PORT_EV(CH2,event,_CPU_63),     \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         63                              \
     }

/* build general entry in DB , that support all the devices
   it uses index 0 in field EXT_DATA
   example: GEN_EV_ALL(SLV_WRITE_ERR)
   target :
   GT_PP_SLV_WRITE_ERR ,
   PRV_CPSS_CH_SLV_WRITE_ERR,
   PRV_CPSS_CH2_52_SLV_WRITE_ERR,
   PRV_CPSS_CH3_SLV_WRITE_ERR
   0
*/
#define GEN_EV_ALL(event)       \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,        \
/*CHEETAH_E,  */         GEN_EV(CH,event),       \
/*CHEETAH2_E, */         GEN_EV(CH2,event),      \
/*CHEETAH3_E, */         GEN_EV(CH3,event),      \
/*XCAT_E,     */         GEN_EV(XCAT,event),     \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),    \
/*LION_B_E,   */         GEN_EV(LION_B,event),   \
/*EXT_DATA_E, */         0                       \
    }

/* event from cheetah 2 */
#define FROM_CH2_EV(event)      \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),             \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH2_E, */         GEN_EV(CH2,event),          \
/*CHEETAH3_E, */         GEN_EV(CH3,event),          \
/*XCAT_E,     */         GEN_EV(XCAT,event),         \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),        \
/*LION_B_E,   */         GEN_EV(LION_B,event),       \
/*EXT_DATA_E, */         0                           \
    }


/* event from Cheetah 3*/
#define FROM_CH3_EV(event)      \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),         \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         GEN_EV(CH3,event),      \
/*XCAT_E,     */         GEN_EV(XCAT,event),     \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),    \
/*LION_B_E,   */         GEN_EV(LION_B,event),   \
/*EXT_DATA_E, */         0                       \
    }

/* event from Cheetah 3*/
#define FROM_CH3_EV_PEX(event)                   \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),         \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         GEN_EV(CH3,event),      \
/*XCAT_E,     */         GEN_EV(XCAT,event),     \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E, \
/*LION_B_E,   */         GEN_EV(LION_B,event),   \
/*EXT_DATA_E, */         0                       \
    }

/* event from Cheetah 3 until xcat */
#define FROM_CH3_UNTIL_XCAT_EV(event)            \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),         \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         GEN_EV(CH3,event),      \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E, \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E, \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E, \
/*EXT_DATA_E, */         0                       \
    }

/* event from Cheetah 3 , but not LION B */
#define FROM_CH3_NOT_LION_B_EV(event)            \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),         \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         GEN_EV(CH3,event),      \
/*XCAT_E,     */         GEN_EV(XCAT,event),     \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),    \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E, \
/*EXT_DATA_E, */         0                       \
    }


/* event for Cheetah1,2 */
#define GEN_EV_ALL_CH1_CH2_ONLY(event)    \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,            \
/*CHEETAH_E,  */         GEN_EV(CH,event),           \
/*CHEETAH2_E, */         GEN_EV(CH2,event),          \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,     \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         0                           \
    }

/* event for all until LionB */
#define GEN_EV_UNTIL_LIONB(event)      \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),             \
/*CHEETAH_E,  */         GEN_EV(CH,event),           \
/*CHEETAH2_E, */         GEN_EV(CH2,event),          \
/*CHEETAH3_E, */         GEN_EV(CH3,event),          \
/*XCAT_E,     */         GEN_EV(XCAT,event),         \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),        \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         0                           \
    }

/* interrupts that has index , the used number is (index-1) */
#define INTERRUPT_EV_ALL(event,index)       \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                    \
/*CHEETAH_E,  */         INTERRUPT_EV(CH,event,index),       \
/*CHEETAH2_E, */         INTERRUPT_EV(CH2,event,index),      \
/*CHEETAH3_E, */         INTERRUPT_EV(CH3,event,index),      \
/*XCAT_E,     */         INTERRUPT_EV(XCAT,event,index),     \
/*XCAT2_E,    */         INTERRUPT_EV(XCAT2,event,index),    \
/*LION_B_E,   */         INTERRUPT_EV(LION_B,event,index),   \
/*EXT_DATA_E, */         (index-1)                           \
    }

/* interrupts 8..12 that has index , the used number is (index-1) */
#define INTERRUPT_8_12_EV_ALL(event,index)       \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                    \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,      \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,      \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,      \
/*XCAT_E,     */         INTERRUPT_EV(XCAT,event,index),  \
/*XCAT2_E,    */         INTERRUPT_EV(XCAT2,event,index),     \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         (index-1)                   \
    }



/* index only in extra parameter */
#define EV_INDEX_ONLY(event,index)  \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,            \
/*CHEETAH_E,  */         GEN_EV(CH,event),           \
/*CHEETAH2_E, */         GEN_EV(CH2,event),          \
/*CHEETAH3_E, */         GEN_EV(CH3,event),          \
/*XCAT_E,     */         GEN_EV(XCAT,event),         \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),        \
/*LION_B_E,   */         GEN_EV(LION_B,event),       \
/*EXT_DATA_E, */         index                       \
    }

/* GOP MIB -- index in extra parameter and in PP event name*/
#define GOP_MIB_EV_INDEX_SPECIFIC(event,index)      \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                    \
/*CHEETAH_E,  */         INDEX_EV(CH,event,index),           \
/*CHEETAH2_E, */         INDEX_EV(CH2,event,index),          \
/*CHEETAH3_E, */         INDEX_EV(CH3,event,index),          \
/*XCAT_E,     */         INDEX_EV(XCAT,event,index),         \
/*XCAT2_E,    */         INDEX_EV(XCAT2,event,index),        \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,             \
/*EXT_DATA_E, */         index                               \
    }


/* index only in extra parameter that uses different name for the unify name
   and for the PPs */
#define EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,         \
/*CHEETAH_E,  */         GEN_EV(CH,ppEvent),      \
/*CHEETAH2_E, */         GEN_EV(CH2,ppEvent),     \
/*CHEETAH3_E, */         GEN_EV(CH3,ppEvent),     \
/*XCAT_E,     */         GEN_EV(XCAT,ppEvent),    \
/*XCAT2_E,    */         GEN_EV(XCAT2,ppEvent),   \
/*LION_B_E,   */         GEN_EV(LION_B,ppEvent),  \
/*EXT_DATA_E, */         index                    \
    }

/* Ch and Ch2 - index only in extra parameter */
#define CH1_CH2_ONLY_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                \
/*CHEETAH_E,  */         GEN_EV(CH,ppEvent),             \
/*CHEETAH2_E, */         GEN_EV(CH2,ppEvent),            \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         index                           \
    }

/* From Ch2 - index only in extra parameter */
#define FROM_CH2_ONLY_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,             \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,      \
/*CHEETAH2_E, */         GEN_EV(CH2,ppEvent),         \
/*CHEETAH3_E, */         GEN_EV(CH3,ppEvent),         \
/*XCAT_E,     */         GEN_EV(XCAT,ppEvent),        \
/*XCAT2_E,    */         GEN_EV(XCAT2,ppEvent),       \
/*LION_B_E,   */         GEN_EV(LION_B,ppEvent),      \
/*EXT_DATA_E, */         index                        \
    }

/* For all until LionB - index only in extra parameter */
#define UNTIL_LIONB_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,             \
/*CHEETAH_E,  */         GEN_EV(CH,ppEvent),          \
/*CHEETAH2_E, */         GEN_EV(CH2,ppEvent),         \
/*CHEETAH3_E, */         GEN_EV(CH3,ppEvent),         \
/*XCAT_E,     */         GEN_EV(XCAT,ppEvent),        \
/*XCAT2_E,    */         GEN_EV(XCAT2,ppEvent),       \
/*EXT_DATA_E, */         index                        \
    }


/* Ch2 - index only in extra parameter */
#define CH2_ONLY_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         GEN_EV(CH2,ppEvent),            \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         index                           \
    }

/* port event all devices , for ports 0..11 */
#define PORT_EV_ALL_ANY_0_11(event,port)     \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,          \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),        \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),       \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),       \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),      \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port),     \
/*LION_B_E,   */         PORT_EV(LION_B,event,port),    \
/*EXT_DATA_E, */         port                           \
    }

    /* port event all devices , for ports 12..23 */
#define PORT_EV_ALL_ANY_12_23(event,port)     \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,       \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),     \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),    \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),    \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),   \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port),  \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         port                        \
    }

/* port event all devices ,for ports 24..26 */
#define PORT_EV_ALL_ANY_24_26(event,port)     \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,       \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),     \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),    \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),    \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),   \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port),  \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         port                        \
    }

/* port 27 event for supporting devices  */
#define PORT_27_EV(event) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,      \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,    \
/*CHEETAH2_E, */         PORT_EV(CH2,event,27),     \
/*CHEETAH3_E, */         PORT_EV(CH3,event,27),     \
/*XCAT_E,     */         PORT_EV(XCAT,event,27),    \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,27),   \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,    \
/*EXT_DATA_E, */         27                         \
    }

/* port event all devices , for ports 0..11 */
#define PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,port)  \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,       \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),     \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),    \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),    \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,     \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         port                        \
    }

    /* port event all devices , for ports 12..23 */
#define PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,port) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,       \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),     \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),    \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),    \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,     \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         port                        \
    }

/* port event all devices ,for ports 24..26 */
#define PORT_EV_ALL_ANY_24_26_UNTIL_XCAT(event,port)     \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,       \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),     \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),    \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),    \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,     \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         port                        \
    }

/* port 27 event for supporting devices  */
#define PORT_27_EV_UNTIL_XCAT(event) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,     \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,   \
/*CHEETAH2_E, */         PORT_EV(CH2,event,27),    \
/*CHEETAH3_E, */         PORT_EV(CH3,event,27),    \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,   \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,   \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,   \
/*EXT_DATA_E, */         27                        \
    }

/* port PCS on ch hx event all devices */
#define CH1_ONLY_HX_PORT_PCS_EV_ALL(event,port)   \
/*UNI_EV_E,   */     {UNI_PORT_PCS_GEN_EV(event) ,       \
/*CHEETAH_E,  */         PORT_EV(CH_HX,event,port),      \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }

/*
the unified event is of 'per port per lane' , although that the CH-HX
    support it 'per port'. so we 'emulate' it as 'lane 0'

    port on ch hx event all devices -- like regular port only with HX indication
*/
#define CH1_ONLY_HX_PORT_EV_ALL_EMULATE_LANE_0(event,port) \
/*UNI_EV_E,   */     {UNI_PORT_LANE_GEN_EV(event) ,      \
/*CHEETAH_E,  */         PORT_EV(CH_HX,event,port),      \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         LANE_PORT_TO_EXT_DATA_CONVERT(port,0)\
    }


/* port 27 on ch2 event all devices */
#define CH2_ONLY_PORT_EV_ALL_27(event) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,       \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH2_E, */         PORT_EV(CH2,event,27),      \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,     \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,     \
/*EXT_DATA_E, */         27                          \
    }

/* port on devices from ch3 , ports 0..11 */
#define FROM_CH3_PORT_EV_ALL_0_11(event,port) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),        \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),       \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port),      \
/*LION_B_E,   */         PORT_EV(LION_B,event,port),     \
/*EXT_DATA_E, */         port                            \
    }

/* port on devices from ch3 , ports 12..23 */
#define FROM_CH3_PORT_EV_ALL_12_23(event,port) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),        \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),       \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port),      \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }

/* port on devices from ch3 , ports 24..27 */
#define FROM_CH3_STACK_PORT_EV_ALL_24_27(event,port) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),        \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),       \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port),      \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }


/* port from ch2  */
#define FROM_CH2_PORT_EV_ALL(event,port) \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,        \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,      \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),     \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),     \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),    \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port),   \
/*LION_B_E,   */         PORT_EV(LION_B,event,port),  \
/*EXT_DATA_E, */         port                         \
    }

/* xg port on ch2 and ch3 event all devices , port 27
   for ch2.. but for ch3 with XG indication
*/
#define FROM_CH2_XG_PORT_EV_ALL_27(event)  \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,             \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,           \
/*CHEETAH2_E, */         PORT_EV(CH2,event,27),            \
/*CHEETAH3_E, */         PORT_EV(CH3_XG,event,27),         \
/*XCAT_E,     */         PORT_EV(XCAT_XG,event,27),        \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,           \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,           \
/*EXT_DATA_E, */         27                                \
    }


/* port event all devices, ch1,2 only */
#define CH1_CH2_ONLY_PORT_EV_ALL(event,port)      \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),         \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),        \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }

/* port event all devices, ports 24..26
   for all devices but for ch3 with XG indication
*/
#define PORT_EV_ALL_WITH_CH3_XG_24_26(event,port)         \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,               \
/*CHEETAH_E,  */         PORT_EV(CH,event,port),             \
/*CHEETAH2_E, */         PORT_EV(CH2,event,port),            \
/*CHEETAH3_E, */         PORT_EV(CH3_XG,event,port),         \
/*XCAT_E,     */         PORT_EV(XCAT_XG,event,port),        \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,             \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,             \
/*EXT_DATA_E, */         port                                \
    }


/* xg port event from ch3 devices
   ch3 with XG indication --> ports 24..27
*/
#define FROM_CH3_XG_PORT_EV_ALL_24_27(event,port)  \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         PORT_EV(CH3_XG,event,port),     \
/*XCAT_E,     */         PORT_EV(XCAT_XG,event,port),    \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }

/* xg port event from ch3 devices
   XG ports 24..27 for any devices
*/
#define FROM_CH3_ANY_XG_PORT_EV_ALL_24_27(event,port)  \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         PORT_EV(CH3,event,port),        \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),       \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }

/* xg port event on ch3 devices
   ch3 with XG indication  --> ports 0,4,10,12,16,22

   NO lion
*/
#define FROM_CH3_XG_PORT_EV_ALL_0_4_10_12_16_22(event,port)  \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         PORT_EV(CH3_XG,event,port),     \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }

/* xg port event on Lion devices : for ports 0..11 */
#define LION_XG_PORT_EV_ALL(event,port)  \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         PORT_EV(LION_B_XG,event,port),  \
/*EXT_DATA_E, */         port                            \
    }


/* event for HGSMIB XG port , 24..26 */
#define PRV_HGSMIB_PORT_EV_ALL_24_26(event,ppEvent, port)\
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event),                             \
/*CHEETAH_E,  */         HGSMIB_PORT_EV(CH, ppEvent, port),               \
/*CHEETAH2_E, */         HGSMIB_PORT_EV(CH2, ppEvent, port),              \
/*CHEETAH3_E, */         PORT_EV(CH3_XG, event, port),                    \
/*XCAT_E,     */         PORT_EV(XCAT_XG, event, port),                   \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,                          \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,                          \
/*EXT_DATA_E, */         port                                             \
    }

/* event for HGSMIB XG port , 27 */
#define PRV_HGSMIB_PORT_EV_ALL_27(event,ppEvent)    \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event),                 \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,              \
/*CHEETAH2_E, */         HGSMIB_PORT_EV(CH2, ppEvent, 27),    \
/*CHEETAH3_E, */         PORT_EV(CH3_XG, event, 27),          \
/*XCAT_E,     */         PORT_EV(XCAT_XG, event, 27),         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,              \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,              \
/*EXT_DATA_E, */         27                                   \
    }


/* specific SCT drop packets event for cheetah2 and cheetah3 */
#define FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event, index)          \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),                          \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,                  \
/*CHEETAH2_E, */         SCT_DROP_PKTS_EV(CH2, event, index),     \
/*CHEETAH3_E, */         SCT_DROP_PKTS_EV(CH3, event, index),     \
/*XCAT_E,     */         SCT_DROP_PKTS_EV(XCAT, event, index),    \
/*XCAT2_E,    */         SCT_DROP_PKTS_EV(XCAT2, event, index),   \
/*LION_B_E,   */         SCT_DROP_PKTS_EV(LION_B, event, index),  \
/*EXT_DATA_E, */         index                                    \
    }

/* BM bank event , from ch3 */
#define FROM_CH3_EV_BM_BANK(event, ppEvent, index)   \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                        \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH3_E, */         BM_BANK_EV(CH3,ppEvent,index),          \
/*XCAT_E,     */         BM_BANK_EV(XCAT,ppEvent,index),         \
/*XCAT2_E,    */         BM_BANK_EV(XCAT2,ppEvent,index),        \
/*LION_B_E,   */         BM_BANK_EV(LION_B,ppEvent,index),       \
/*EXT_DATA_E, */         index                                   \
    }


/* CNC block event from cheetah3 */
#define FROM_CH3_EV_CNC_BLOCK_0_1(event,index)  \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                        \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH3_E, */         INDEX_EV2(CH3,event,index),             \
/*XCAT_E,     */         INDEX_EV2(XCAT,event,index),            \
/*XCAT2_E,    */         INDEX_EV2(XCAT2,event,index),           \
/*LION_B_E,   */         INDEX_EV2(LION_B,event,index),          \
/*EXT_DATA_E, */         index                                   \
    }

/* CNC block event from cheetah3 , blocks 2..7
   lion not support it
   Lion B support it
*/
#define FROM_CH3_EV_CNC_BLOCK_2_7(event,index)         \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                        \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH3_E, */         INDEX_EV2(CH3,event,index),             \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*XCAT2_E,    */         INDEX_EV2(XCAT2,event,index),           \
/*LION_B_E,   */         INDEX_EV2(LION_B,event,index),          \
/*EXT_DATA_E, */         index                                   \
    }

/* CNC block event from cheetah3 , blocks 8..11
   lion not support it
   Lion B not support it
*/
#define FROM_CH3_EV_CNC_BLOCK_8_11(event,index)                  \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                        \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*CHEETAH3_E, */         INDEX_EV2(CH3,event,index),             \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,                 \
/*EXT_DATA_E, */         index                                   \
    }

/* ports 0..11 */
#define LOOP_0_11_PORTS(event)    \
    PORT_EV_ALL_ANY_0_11(event,0),         \
    PORT_EV_ALL_ANY_0_11(event,1),         \
    PORT_EV_ALL_ANY_0_11(event,2),         \
    PORT_EV_ALL_ANY_0_11(event,3),         \
    PORT_EV_ALL_ANY_0_11(event,4),         \
    PORT_EV_ALL_ANY_0_11(event,5),         \
    PORT_EV_ALL_ANY_0_11(event,6),         \
    PORT_EV_ALL_ANY_0_11(event,7),         \
    PORT_EV_ALL_ANY_0_11(event,8),         \
    PORT_EV_ALL_ANY_0_11(event,9),         \
    PORT_EV_ALL_ANY_0_11(event,10),        \
    PORT_EV_ALL_ANY_0_11(event,11)

/* ports 12..23 */
#define LOOP_12_23_PORTS(event)    \
    PORT_EV_ALL_ANY_12_23(event,12),        \
    PORT_EV_ALL_ANY_12_23(event,13),        \
    PORT_EV_ALL_ANY_12_23(event,14),        \
    PORT_EV_ALL_ANY_12_23(event,15),        \
    PORT_EV_ALL_ANY_12_23(event,16),        \
    PORT_EV_ALL_ANY_12_23(event,17),        \
    PORT_EV_ALL_ANY_12_23(event,18),        \
    PORT_EV_ALL_ANY_12_23(event,19),        \
    PORT_EV_ALL_ANY_12_23(event,20),        \
    PORT_EV_ALL_ANY_12_23(event,21),        \
    PORT_EV_ALL_ANY_12_23(event,22),        \
    PORT_EV_ALL_ANY_12_23(event,23)


/* ports 0..11 */
#define LOOP_0_11_PORTS_UNTIL_XCAT(event)    \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,0),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,1),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,2),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,3),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,4),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,5),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,6),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,7),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,8),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,9),         \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,10),        \
    PORT_EV_ALL_ANY_0_11_UNTIL_XCAT(event,11)

/* ports 12..23 */
#define LOOP_12_23_PORTS_UNTIL_XCAT(event)    \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,12),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,13),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,14),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,15),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,16),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,17),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,18),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,19),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,20),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,21),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,22),        \
    PORT_EV_ALL_ANY_12_23_UNTIL_XCAT(event,23)


/* ports 0..23 */
#define LOOP_0_23_PORTS(event)    \
    LOOP_0_11_PORTS(event),       \
    LOOP_12_23_PORTS(event)

/* ports 0..23 */
#define LOOP_0_23_PORTS_UNTIL_XCAT(event)    \
    LOOP_0_11_PORTS_UNTIL_XCAT(event),       \
    LOOP_12_23_PORTS_UNTIL_XCAT(event)


/* port event all devices , for ports 12..23 */
#define GE_STACKING_PORT_EV_24_27(event,port)     \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,      \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,    \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,    \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,    \
/*XCAT_E,     */         PORT_EV(XCAT,event,port),  \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port), \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,    \
/*EXT_DATA_E, */         port                       \
    }

/* GE stacking ports 24..27 */
#define LOOP_GE_STACKING_PORTS_24_27(event)    \
    GE_STACKING_PORT_EV_24_27(event,24),        \
    GE_STACKING_PORT_EV_24_27(event,25),        \
    GE_STACKING_PORT_EV_24_27(event,26),        \
    GE_STACKING_PORT_EV_24_27(event,27)




/* ports 0..23, for ch1,2 */
#define CH1_CH2_ONLY_LOOP_0_23_PORTS(event)    \
    CH1_CH2_ONLY_PORT_EV_ALL(event,0),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,1),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,2),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,3),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,4),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,5),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,6),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,7),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,8),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,9),         \
    CH1_CH2_ONLY_PORT_EV_ALL(event,10),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,11),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,12),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,13),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,14),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,15),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,16),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,17),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,18),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,19),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,20),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,21),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,22),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,23)


/* ports 0..11, for CH3 */
#define CH3_LOOP_0_11_PORTS(event)    \
    FROM_CH3_PORT_EV_ALL_0_11(event,0),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,1),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,2),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,3),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,4),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,5),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,6),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,7),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,8),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,9),         \
    FROM_CH3_PORT_EV_ALL_0_11(event,10),        \
    FROM_CH3_PORT_EV_ALL_0_11(event,11)

/* ports 12..23, for CH3 */
#define CH3_LOOP_12_23_PORTS(event)    \
    FROM_CH3_PORT_EV_ALL_12_23(event,12),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,13),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,14),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,15),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,16),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,17),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,18),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,19),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,20),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,21),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,22),        \
    FROM_CH3_PORT_EV_ALL_12_23(event,23)

/* ports 0..23, for devices from CH3 */
#define FROM_CH3_LOOP_0_23_PORTS(event)    \
    CH3_LOOP_0_11_PORTS(event),        \
    CH3_LOOP_12_23_PORTS(event)

/* ports 24..26 */
#define LOOP_24_26_PORTS(event)   \
    PORT_EV_ALL_ANY_24_26(event,24),        \
    PORT_EV_ALL_ANY_24_26(event,25),        \
    PORT_EV_ALL_ANY_24_26(event,26)

/* ports 24..26 */
#define LOOP_24_26_PORTS_UNTIL_XCAT(event)   \
    PORT_EV_ALL_ANY_24_26_UNTIL_XCAT(event,24),        \
    PORT_EV_ALL_ANY_24_26_UNTIL_XCAT(event,25),        \
    PORT_EV_ALL_ANY_24_26_UNTIL_XCAT(event,26)

/* ports 24..27, for devices from ch3 */
#define FROM_CH3_ANY_LOOP_24_27_XG_PORTS(event)      \
    FROM_CH3_ANY_XG_PORT_EV_ALL_24_27(event,24),     \
    FROM_CH3_ANY_XG_PORT_EV_ALL_24_27(event,25),     \
    FROM_CH3_ANY_XG_PORT_EV_ALL_24_27(event,26),     \
    FROM_CH3_ANY_XG_PORT_EV_ALL_24_27(event,27)


/* ports 24..26 */
#define CH1_CH2_ONLY_LOOP_24_26_PORTS(event)   \
    CH1_CH2_ONLY_PORT_EV_ALL(event,24),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,25),        \
    CH1_CH2_ONLY_PORT_EV_ALL(event,26)

/* ports 24..26  , for all devices but for ch3 with XG indication */
#define LOOP_24_26_PORTS_WITH_CH3_XG(event)   \
    PORT_EV_ALL_WITH_CH3_XG_24_26(event,24),        \
    PORT_EV_ALL_WITH_CH3_XG_24_26(event,25),        \
    PORT_EV_ALL_WITH_CH3_XG_24_26(event,26)

/* ports 0..27 , CPU (63) */
#define LOOP_ALL_PORTS(event)           \
    LOOP_0_23_PORTS(event),             \
    LOOP_XG_PORTS(event),               \
    LOOP_GE_STACKING_PORTS_24_27(event),\
    CPU_PORT_EV_ALL(event)


/* events for GE ports only
   ports 0..23 , CPU (63)
*/
#define LOOP_ALL_PORTS_GE_ONLY(event)   \
    LOOP_0_23_PORTS(event),             \
    LOOP_GE_STACKING_PORTS_24_27(event),\
    CPU_PORT_EV_ALL(event)

/* port event XCAT2 only, for all ports */
#define PORT_EV_ALL_XCAT2_ONLY(event,port)     \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,      \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,    \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,    \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,    \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,    \
/*XCAT2_E,    */         PORT_EV(XCAT2,event,port), \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,    \
/*EXT_DATA_E, */         port                       \
    }

/* events for GE ports of XCAT2 only
   ports 0..23, ge stacking 24..27
*/
#define LOOP_XCAT2_PORTS_GE_ONLY(event) \
    PORT_EV_ALL_XCAT2_ONLY(event,0),    \
    PORT_EV_ALL_XCAT2_ONLY(event,1),    \
    PORT_EV_ALL_XCAT2_ONLY(event,2),    \
    PORT_EV_ALL_XCAT2_ONLY(event,3),    \
    PORT_EV_ALL_XCAT2_ONLY(event,4),    \
    PORT_EV_ALL_XCAT2_ONLY(event,5),    \
    PORT_EV_ALL_XCAT2_ONLY(event,6),    \
    PORT_EV_ALL_XCAT2_ONLY(event,7),    \
    PORT_EV_ALL_XCAT2_ONLY(event,8),    \
    PORT_EV_ALL_XCAT2_ONLY(event,9),    \
    PORT_EV_ALL_XCAT2_ONLY(event,10),   \
    PORT_EV_ALL_XCAT2_ONLY(event,11),   \
    PORT_EV_ALL_XCAT2_ONLY(event,12),   \
    PORT_EV_ALL_XCAT2_ONLY(event,13),   \
    PORT_EV_ALL_XCAT2_ONLY(event,14),   \
    PORT_EV_ALL_XCAT2_ONLY(event,15),   \
    PORT_EV_ALL_XCAT2_ONLY(event,16),   \
    PORT_EV_ALL_XCAT2_ONLY(event,17),   \
    PORT_EV_ALL_XCAT2_ONLY(event,18),   \
    PORT_EV_ALL_XCAT2_ONLY(event,19),   \
    PORT_EV_ALL_XCAT2_ONLY(event,20),   \
    PORT_EV_ALL_XCAT2_ONLY(event,21),   \
    PORT_EV_ALL_XCAT2_ONLY(event,22),   \
    PORT_EV_ALL_XCAT2_ONLY(event,23),   \
    PORT_EV_ALL_XCAT2_ONLY(event,24),   \
    PORT_EV_ALL_XCAT2_ONLY(event,25),   \
    PORT_EV_ALL_XCAT2_ONLY(event,26),   \
    PORT_EV_ALL_XCAT2_ONLY(event,27)

/* for ch3 xg ports 0,4,10,12,16,22 (XG port below port 24) */
#define CH3_ONLY_XG_PORTS_0_4_10_12_16_22(event)       \
    FROM_CH3_XG_PORT_EV_ALL_0_4_10_12_16_22(event,0),            \
    FROM_CH3_XG_PORT_EV_ALL_0_4_10_12_16_22(event,4),            \
    FROM_CH3_XG_PORT_EV_ALL_0_4_10_12_16_22(event,10),           \
    FROM_CH3_XG_PORT_EV_ALL_0_4_10_12_16_22(event,12),           \
    FROM_CH3_XG_PORT_EV_ALL_0_4_10_12_16_22(event,16),           \
    FROM_CH3_XG_PORT_EV_ALL_0_4_10_12_16_22(event,22)

/* loop lion 0..11 XG ports , with XG indication */
#define LION_ONLY_LOOP_XG_PORTS(event)   \
    LION_XG_PORT_EV_ALL(event,0),         \
    LION_XG_PORT_EV_ALL(event,1),   \
    LION_XG_PORT_EV_ALL(event,2),   \
    LION_XG_PORT_EV_ALL(event,3),   \
    LION_XG_PORT_EV_ALL(event,4),   \
    LION_XG_PORT_EV_ALL(event,5),   \
    LION_XG_PORT_EV_ALL(event,6),   \
    LION_XG_PORT_EV_ALL(event,7),   \
    LION_XG_PORT_EV_ALL(event,8),   \
    LION_XG_PORT_EV_ALL(event,9),   \
    LION_XG_PORT_EV_ALL(event,10),  \
    LION_XG_PORT_EV_ALL(event,11)

/* from ch3 , XG ports with XG indication */
#define FROM_CH3_LOOP_XG_PORTS(event)        \
    CH3_ONLY_XG_PORTS_0_4_10_12_16_22(event),      \
    FROM_CH3_XG_PORT_EV_ALL_24_27(event,24),       \
    FROM_CH3_XG_PORT_EV_ALL_24_27(event,25),       \
    FROM_CH3_XG_PORT_EV_ALL_24_27(event,26),       \
    FROM_CH3_XG_PORT_EV_ALL_24_27(event,27),       \
    LION_ONLY_LOOP_XG_PORTS(event),                \
    LOOP_XLG_PORTS(event)

/*
   for all devices but for ch3 with XG indication
   Lion not support GOP MIB interrupts
*/
#define PRV_HGSMIB_LOOP_XG_PORTS_24_27(event,ppEvent)        \
    PRV_HGSMIB_PORT_EV_ALL_24_26(event,ppEvent,24),          \
    PRV_HGSMIB_PORT_EV_ALL_24_26(event,ppEvent,25),          \
    PRV_HGSMIB_PORT_EV_ALL_24_26(event,ppEvent,26),          \
    PRV_HGSMIB_PORT_EV_ALL_27(event,ppEvent)

/*
   for ch1,2 diff name then from ch3
   support all devices with XG ports
*/
#define LOOP_HGSMIB_XG_PORTS_DIF_NAMES(eventChCh2, eventCh3) \
    PRV_HGSMIB_LOOP_XG_PORTS_24_27(eventCh3, eventChCh2),    \
    CH3_ONLY_XG_PORTS_0_4_10_12_16_22(eventCh3),             \
    LION_ONLY_LOOP_XG_PORTS(eventCh3),                       \
    LOOP_XLG_PORTS(eventCh3)


/* ports 24..27 */
#define LOOP_XG_PORTS(event)                    \
    LOOP_24_26_PORTS_WITH_CH3_XG(event),        \
    FROM_CH2_XG_PORT_EV_ALL_27(event),          \
    CH3_ONLY_XG_PORTS_0_4_10_12_16_22(event),   \
    LION_ONLY_LOOP_XG_PORTS(event),             \
    LOOP_XLG_PORTS(event)

/* XLG port event on Lion-B devices : for port 10 */
#define LION_B_XLG_PORT_EV_ALL(event,port)  \
/*UNI_EV_E,   */     {UNI_PORT_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         PORT_EV(LION_B_XLG,event,port), \
/*EXT_DATA_E, */         port                            \
    }

/* loop lion 0..11 XG ports , with XG indication */
#define LION_B_LOOP_XLG_PORTS(event)   \
    LION_B_XLG_PORT_EV_ALL(event,10)  \

/* XLG ports :
    Lion-B port 10
*/
#define LOOP_XLG_PORTS(event)                    \
    LION_B_LOOP_XLG_PORTS(event)


/* ch1 loop XPCS ports ,
    ports 25,26 --> with HX indication
*/
#define CH1_ONLY_XPCS_LOOP_PORTS(event)               \
    CH1_ONLY_HX_PORT_PCS_EV_ALL(event,25),       \
    CH1_ONLY_HX_PORT_PCS_EV_ALL(event,26)

/* port PCS from lion ports 0..11 */
#define FROM_LION_PORT_PCS_EV_ALL_0_11(event,port)        \
/*UNI_EV_E,   */     {UNI_PORT_PCS_GEN_EV(event),         \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,          \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,          \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,          \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,          \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,          \
/*LION_B_E,   */         PORT_EV(LION_B_XPCS,event,port), \
/*EXT_DATA_E, */         port                             \
    }

/* port PCS from xcat ports 24..27 */
#define FROM_XCAT_PORT_PCS_EV_ALL_24_27(event,port)      \
/*UNI_EV_E,   */     {UNI_PORT_PCS_GEN_EV(event) ,       \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         PORT_EV(XCAT_XPCS,event,port),  \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         port                            \
    }

/* lion loop XPCS ports ,
    ports 0..11 --> with XPCS indication
*/
#define FROM_LION_XPCS_LOOP_PORTS_0_11(event)      \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,0),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,1),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,2),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,3),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,4),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,5),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,6),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,7),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,8),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,9),       \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,10),      \
    FROM_LION_PORT_PCS_EV_ALL_0_11(event,11)


/* XLG port PCS lion port 10 */
#define LION_XLG_PCS_EV(event,port)   \
/*UNI_EV_E,   */     {UNI_PORT_PCS_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,  \
/*LION_B_E,   */         PORT_EV(LION_B_XPCS_XLG,event,port),  \
/*EXT_DATA_E, */         port                            \
    }

/* lion loop XLG PCS ports ,
    port 10 --> with XLG PCS indication
*/
#define LION_XLG_PCS_LOOP_PORTS(event)            \
    LION_XLG_PCS_EV(event,10)

/* lion loop XPCS ports ,
    ports 0..11 --> with XPCS indication
*/
#define FROM_XCAT_XPCS_LOOP_PORTS_24_27(event)      \
    FROM_XCAT_PORT_PCS_EV_ALL_24_27(event,24),       \
    FROM_XCAT_PORT_PCS_EV_ALL_24_27(event,25),       \
    FROM_XCAT_PORT_PCS_EV_ALL_24_27(event,26),       \
    FROM_XCAT_PORT_PCS_EV_ALL_24_27(event,27)

/* loop XPCS ports ,
    currently supported:
        ch1 for ports 25,26 --> with HX indication
        lion for ports 0..11

*/
#define XPCS_LOOP_PORTS(event)             \
    CH1_ONLY_XPCS_LOOP_PORTS(event),       \
    FROM_LION_XPCS_LOOP_PORTS_0_11(event),\
    FROM_XCAT_XPCS_LOOP_PORTS_24_27(event)

/* loop XPCS ports ,
    currently supported:
        ch1 for ports 25,26 --> with HX indication
        not LION

*/
#define XPCS_LOOP_PORTS_UNTIL_LION(event)             \
    CH1_ONLY_XPCS_LOOP_PORTS(event),       \
    FROM_XCAT_XPCS_LOOP_PORTS_24_27(event)


/* loop XLG PCS ports ,
    currently supported:
        lion for port 10

*/
#define XLG_PCS_LOOP_PORTS(event)             \
        LION_XLG_PCS_LOOP_PORTS(event)



/*  the unified event is of 'per port per lane' , although that the CH-HX
    support it 'per port'. so we 'emulate' it as 'lane 0'
*/
#define CH1_ONLY_LOOP_HX_PORTS_EMULATE_LANE_0(event)               \
    CH1_ONLY_HX_PORT_EV_ALL_EMULATE_LANE_0(event,25),       \
    CH1_ONLY_HX_PORT_EV_ALL_EMULATE_LANE_0(event,26)

/* ch1 only -- event per port per lane */
#define CH1_ONLY_LANE_PORT(event,port,lane)  \
/*UNI_EV_E,   */     {UNI_PORT_LANE_GEN_EV(event) ,           \
/*CHEETAH_E,  */         PORT_LANE_EV(CH_HX,event,port,lane), \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         LANE_PORT_TO_EXT_DATA_CONVERT(port,lane)\
    }

/* ch1 only -- loop on lanes 0,1 of port (should be 25,26)*/
#define CH1_ONLY_LANES_PORT(event,port) \
    CH1_ONLY_LANE_PORT(event,port,0)    ,\
    CH1_ONLY_LANE_PORT(event,port,1)

/* ch1 only -- loop on ports 25,26 on all lanes 0,1 */
#define CH1_ONLY_LANES_PORTS(event) \
    CH1_ONLY_LANES_PORT(event,25)   ,\
    CH1_ONLY_LANES_PORT(event,26)

/* Lion -- event per port per lane */
#define FROM_LION_LANE_PORT_0_11(event,port,lane)  \
/*UNI_EV_E,   */     {UNI_PORT_LANE_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         PORT_LANE_EV(LION_B,event,port,lane), \
/*EXT_DATA_E, */         LANE_PORT_TO_EXT_DATA_CONVERT(port,lane)\
    }


/* Lion -- loop on lanes 0..5 of port (should be 0..11) */
#define FROM_LION_LANES_PORT_0_11(event,port) \
    FROM_LION_LANE_PORT_0_11(event,port,0)    ,\
    FROM_LION_LANE_PORT_0_11(event,port,1)    ,\
    FROM_LION_LANE_PORT_0_11(event,port,2)    ,\
    FROM_LION_LANE_PORT_0_11(event,port,3)    ,\
    FROM_LION_LANE_PORT_0_11(event,port,4)    ,\
    FROM_LION_LANE_PORT_0_11(event,port,5)

/* Lion  -- loop on ports 0..11 , for lanes 0..5 */
#define FROM_LION_LANE_LOOP_PORTS_0_11(event) \
    FROM_LION_LANES_PORT_0_11(event,0)   ,\
    FROM_LION_LANES_PORT_0_11(event,1)   ,\
    FROM_LION_LANES_PORT_0_11(event,2)   ,\
    FROM_LION_LANES_PORT_0_11(event,3)   ,\
    FROM_LION_LANES_PORT_0_11(event,4)   ,\
    FROM_LION_LANES_PORT_0_11(event,5)   ,\
    FROM_LION_LANES_PORT_0_11(event,6)   ,\
    FROM_LION_LANES_PORT_0_11(event,7)   ,\
    FROM_LION_LANES_PORT_0_11(event,8)   ,\
    FROM_LION_LANES_PORT_0_11(event,9)   ,\
    FROM_LION_LANES_PORT_0_11(event,10)  ,\
    FROM_LION_LANES_PORT_0_11(event,11)

/* XCAT -- event per port per lane */
#define FROM_XCAT_LANE_PORT_24_27(event,port,lane)  \
/*UNI_EV_E,   */     {UNI_PORT_LANE_GEN_EV(event) ,           \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,         \
/*XCAT_E,     */         PORT_LANE_EV(XCAT,event,port,lane), \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,         \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E,         \
/*EXT_DATA_E, */         LANE_PORT_TO_EXT_DATA_CONVERT(port,lane)\
    }

/* XCAT -- loop on lanes 0..3 of port (should be 24..27) */
#define FROM_XCAT_LANES_PORT_24_27(event,port) \
    FROM_XCAT_LANE_PORT_24_27(event,port,0)    ,\
    FROM_XCAT_LANE_PORT_24_27(event,port,1)    ,\
    FROM_XCAT_LANE_PORT_24_27(event,port,2)    ,\
    FROM_XCAT_LANE_PORT_24_27(event,port,3)

/* xcat  -- loop on ports 24..27 , for lanes 0..3 */
#define FROM_XCAT_LANE_LOOP_PORTS_24_27(event) \
    FROM_XCAT_LANES_PORT_24_27(event,24)   ,\
    FROM_XCAT_LANES_PORT_24_27(event,25)   ,\
    FROM_XCAT_LANES_PORT_24_27(event,26)   ,\
    FROM_XCAT_LANES_PORT_24_27(event,27)

/* generation 1 of the lanes (xcat - but no lion,ch1) */
#define FROM_XCAT_UNTILL_LION_LANE_LOOP_PORTS(event)        \
    FROM_XCAT_LANE_LOOP_PORTS_24_27(event)

/* generation 1 of the lanes (xcat,lion - but no ch1) */
#define FROM_XCAT_LANE_LOOP_PORTS(event)        \
    FROM_XCAT_UNTILL_LION_LANE_LOOP_PORTS(event), \
    FROM_LION_LANE_LOOP_PORTS_0_11(event)

/* loop on all lanes in all ports :
    currently supported for :
        Ch1 , ports 25,26 - 2 lanes (per port)
        Lion ports 0..11  - 6 lanes (per port)
*/
#define LANE_LOOP_PORTS(event)      \
    LANE_LOOP_PORTS_UNTIL_LION(event),    \
    FROM_XCAT_LANE_LOOP_PORTS(event)

/* loop on all lanes in all ports :
    currently supported for :
        Ch1 , ports 25,26 - 2 lanes (per port)
        Lion - not supported
*/
#define LANE_LOOP_PORTS_UNTIL_LION(event)      \
    CH1_ONLY_LANES_PORTS(event),               \
    FROM_XCAT_UNTILL_LION_LANE_LOOP_PORTS(event)



/* ports 0..27 , for ch1,2 */
#define CH1_CH2_ONLY_LOOP_ALL_PORTS_NO_CPU(event)     \
    CH1_CH2_ONLY_LOOP_0_23_PORTS(event),             \
    CH1_CH2_ONLY_LOOP_24_26_PORTS(event),            \
    CH2_ONLY_PORT_EV_ALL_27(event)

/* ports 0..27 , (no CPU) , ch3 without XG indication */
#define LOOP_ALL_PORTS_NO_CPU_NO_CH3_XG(event)      \
    LOOP_0_23_PORTS(event),                         \
    LOOP_24_26_PORTS(event),                        \
    PORT_27_EV(event)

/* ports 0..27 , (no CPU) , ch3 without XG indication */
#define LOOP_ALL_PORTS_NO_CPU_NO_CH3_XG_UNTIL_XCAT(event)      \
    LOOP_0_23_PORTS_UNTIL_XCAT(event),                         \
    LOOP_24_26_PORTS_UNTIL_XCAT(event),                        \
    PORT_27_EV_UNTIL_XCAT(event)


/* ports 0..27 , (with CPU) , ch3 without XG indication */
#define LOOP_ALL_PORTS_NO_CH3_XG(event)         \
    LOOP_ALL_PORTS_NO_CPU_NO_CH3_XG(event),     \
    CPU_PORT_EV_ALL(event)

/* ports 0..27 , (with CPU) , ch3 without XG indication -- until xcat */
#define LOOP_ALL_PORTS_NO_CH3_XG_UNTIL_XCAT(event)         \
    LOOP_ALL_PORTS_NO_CPU_NO_CH3_XG_UNTIL_XCAT(event),     \
    CPU_PORT_EV_ALL_UNTIL_XCAT(event)

/* ports 0..27 , CPU (63), not for Cheetah3 */
#define LOOP_ALL_PORTS_NO_CH3(event)           \
    CH1_CH2_ONLY_LOOP_ALL_PORTS_NO_CPU(event),       \
    CPU_PORT_EV_ALL_NO_CH3(event)

/* general events that are per queue */
#define QUEUE_EV_ALL(event,queue)    \
/*UNI_EV_E,   */     {UNI_QUEUE_GEN_EV(event),        \
/*CHEETAH_E,  */         QUEUE_EV(CH,event,queue),    \
/*CHEETAH2_E, */         QUEUE_EV(CH2,event,queue),   \
/*CHEETAH3_E, */         QUEUE_EV(CH3,event,queue),   \
/*XCAT_E,     */         QUEUE_EV(XCAT,event,queue),   \
/*XCAT2_E,    */         QUEUE_EV(XCAT2,event,queue),   \
/*LION_B_E,   */         QUEUE_EV(LION_B,event,queue),   \
/*EXT_DATA_E, */         queue                        \
    }

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
#define QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,queue)   \
/*UNI_EV_E,   */     {UNI_GEN_EV_SPECIFIC_QUEUE_UNI_NAME(event,queue),       \
/*CHEETAH_E,  */         QUEUE_EV(CH,event,queue),                           \
/*CHEETAH2_E, */         QUEUE_EV(CH2,event,queue),                          \
/*CHEETAH3_E, */         QUEUE_EV(CH3,event,queue),                          \
/*XCAT_E,     */         QUEUE_EV(XCAT,event,queue),                          \
/*XCAT2_E,    */         QUEUE_EV(XCAT2,event,queue),                          \
/*LION_B_E,   */         QUEUE_EV(LION_B,event,queue),                          \
/*EXT_DATA_E, */         queue                                               \
    }

/* event for Queue 0..7 , all devices */
#define QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(event)        \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,0),          \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,1),          \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,2),          \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,3),          \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,4),          \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,5),          \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,6),          \
    QUEUE_EV_ALL_SPECIFIC_QUEUE_UNI_NAME(event,7)

/* sct drop pkts event 1..31 , from ch2 devices */
#define FROM_CH2_LOOP_ALL_SCT_DROP_PKTS(event)           \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,1),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,2),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,3),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,4),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,5),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,6),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,7),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,8),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,9),         \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,10),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,11),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,12),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,13),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,14),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,15),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,16),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,17),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,18),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,19),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,20),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,21),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,22),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,23),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,24),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,25),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,26),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,27),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,28),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,29),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,30),        \
    FROM_CH2_EV_SPECIFIC_SCT_DROP_PKTS(event,31)

/* from ch3 BM banks events */
#define FROM_CH3_LOOP_BM_BANKS(event, ppEvent)   \
    FROM_CH3_EV_BM_BANK(event, ppEvent, 0),      \
    FROM_CH3_EV_BM_BANK(event, ppEvent, 1)

/* CNC blocks 0..11 , from ch3 */
#define FROM_CH3_LOOP_CNC_BLOCKS(event)      \
    FROM_CH3_EV_CNC_BLOCK_0_1(event,0),          \
    FROM_CH3_EV_CNC_BLOCK_0_1(event,1),          \
    FROM_CH3_EV_CNC_BLOCK_2_7(event,2),          \
    FROM_CH3_EV_CNC_BLOCK_2_7(event,3),          \
    FROM_CH3_EV_CNC_BLOCK_2_7(event,4),          \
    FROM_CH3_EV_CNC_BLOCK_2_7(event,5),          \
    FROM_CH3_EV_CNC_BLOCK_2_7(event,6),          \
    FROM_CH3_EV_CNC_BLOCK_2_7(event,7),          \
    FROM_CH3_EV_CNC_BLOCK_8_11(event,8),          \
    FROM_CH3_EV_CNC_BLOCK_8_11(event,9),          \
    FROM_CH3_EV_CNC_BLOCK_8_11(event,10),         \
    FROM_CH3_EV_CNC_BLOCK_8_11(event,11)

/* event from XCAT */
#define FROM_XCAT_EV(event)      \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),         \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*XCAT_E,     */         GEN_EV(XCAT,event),     \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),    \
/*LION_B_E,   */         GEN_EV(LION_B,event),   \
/*EXT_DATA_E, */         0                       \
    }

/* event from XCAT */
#define FROM_XCAT_NOT_LION_B_EV(event)      \
/*UNI_EV_E,   */     {UNI_GEN_EV(event),         \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*XCAT_E,     */         GEN_EV(XCAT,event),     \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),    \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E, \
/*EXT_DATA_E, */         0                       \
    }

/* from xcat -- index only in extra parameter that uses different name for the unify name
   and for the PPs */
#define FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,            \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT_E,     */         GEN_EV(XCAT,ppEvent),       \
/*XCAT2_E,    */         GEN_EV(XCAT2,ppEvent),      \
/*LION_B_E,   */         GEN_EV(LION_B,ppEvent),     \
/*EXT_DATA_E, */         index                       \
    }

/* from xcat -- index only in extra parameter that uses different name for the unify name
   and for the PPs */
#define FROM_XCAT1_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,            \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT_E,     */         GEN_EV(XCAT,ppEvent),       \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,     \
/*LION_B_E,   */         GEN_EV(LION_B,ppEvent),     \
/*EXT_DATA_E, */         index                       \
    }

/* from xcat -- index only in extra parameter that uses different name for the unify name
   and for the PPs */
#define XCAT1_AND_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,            \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,     \
/*XCAT_E,     */         GEN_EV(XCAT,ppEvent),       \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,     \
/*LION_B_E,   */         GEN_EV(LION_B,ppEvent),     \
/*EXT_DATA_E, */         index                       \
    }

/* from Lion-B -- index only in extra parameter that uses different name for the unify name
   and for the PPs */
#define FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(event,ppEvent,index) \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,                                    \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E,                             \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E,                             \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E,                             \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E,                             \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E,                             \
/*LION_B_E,   */         GEN_EV(LION_B,ppEvent),                             \
/*EXT_DATA_E, */         index                                               \
    }


/* gen for devices with TXQ rev 0 only */
#define TXQ_REV_0_GEN_EV_ALL(event)              \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,        \
/*CHEETAH_E,  */         GEN_EV(CH,event),       \
/*CHEETAH2_E, */         GEN_EV(CH2,event),      \
/*CHEETAH3_E, */         GEN_EV(CH3,event),      \
/*XCAT_E,     */         GEN_EV(XCAT,event),     \
/*XCAT2_E,    */         GEN_EV(XCAT2,event),    \
/*LION_B_E,   */         CPSS_UNI_RSRVD_EVENT_E, \
/*EXT_DATA_E, */         0                       \
    }

/* gen for devices with TXQ from rev 1  */
#define TXQ_FROM_REV_1_GEN_EV_ALL(event)         \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,        \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E, \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E, \
/*LION_B_E,   */         GEN_EV(LION_B,event),   \
/*EXT_DATA_E, */         0                       \
    }

/* for specific port group for devices with TXQ from rev 1,
   do for all 8 TC
   with prefix for port groups and prefix for TCs
   */
#define TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,tc)       \
/*UNI_EV_E,   */     {UNI_GEN_EV(event) ,        \
/*CHEETAH_E,  */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH2_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*CHEETAH3_E, */         CPSS_UNI_RSRVD_EVENT_E, \
/*XCAT_E,     */         CPSS_UNI_RSRVD_EVENT_E, \
/*XCAT2_E,    */         CPSS_UNI_RSRVD_EVENT_E, \
/*LION_B_E,   */         GROUP_2_EV(LION_B,ppEventPart1,PORT_GROUP_,portGroupId,ppEventPart2##_TC_,tc),   \
/*EXT_DATA_E, */         (portGroupId) * 8 + (tc)\
    }

/* for specific port group for devices with TXQ from rev 1,
   do for all 8 TC  */
#define TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_ALL_TC(event,ppEventPart1,portGroupId,ppEventPart2) \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,0),    \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,1),    \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,2),    \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,3),    \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,4),    \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,5),    \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,6),    \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_SPECIFIC_TC(event,ppEventPart1,portGroupId,ppEventPart2,7)

/* for 4 port groups for all TC for devices with TXQ from rev 1  */
#define TXQ_FROM_REV_1_FOR_4_PORT_GROUPS_ALL_TC(event,ppEventPart1,ppEventPart2)         \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_ALL_TC(event,ppEventPart1,0,ppEventPart2),        \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_ALL_TC(event,ppEventPart1,1,ppEventPart2),        \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_ALL_TC(event,ppEventPart1,2,ppEventPart2),        \
    TXQ_FROM_REV_1_SPECIFIC_PORT_GROUP_ALL_TC(event,ppEventPart1,3,ppEventPart2)

/* for ALL port groups for all TC for devices with TXQ from rev 1  */
#define TXQ_FROM_REV_1_FOR_ALL_PORT_GROUPS_ALL_TC(event,ppEventPart1,ppEventPart2)         \
    TXQ_FROM_REV_1_FOR_4_PORT_GROUPS_ALL_TC(event,ppEventPart1,ppEventPart2)


/* the conversion table */
static GT_U16 ppUni2IntCauseType[][TABLE_ENTRY_SIZE_E] = {
      GEN_EV_ALL_CH1_CH2_ONLY(MAS_READ_ERR),
      GEN_EV_ALL_CH1_CH2_ONLY(SLV_READ_ERR),
      GEN_EV_ALL_CH1_CH2_ONLY(MAS_WRITE_ERR),
      GEN_EV_ALL_CH1_CH2_ONLY(ADDR_ERR),
      GEN_EV_ALL_CH1_CH2_ONLY(MAS_ABORT),
      GEN_EV_ALL_CH1_CH2_ONLY(TARGET_ABORT),
      GEN_EV_ALL_CH1_CH2_ONLY(SLV_READ_ERR),
      GEN_EV_ALL_CH1_CH2_ONLY(RETRY_CNTR),

      GEN_EV_ALL(MISC_TWSI_TIME_OUT),
      GEN_EV_ALL(MISC_TWSI_STATUS),
      GEN_EV_ALL(MISC_ILLEGAL_ADDR),
      GEN_EV_ALL(MISC_CPU_PORT_RX_OVERRUN),
      GEN_EV_ALL_CH1_CH2_ONLY(MISC_CPU_PORT_TX_OVERRUN),
      CPU_PORT_EV_ALL_NO_CH3(TX_CRC_ERROR),

      INTERRUPT_EV_ALL(GPP,1),
      INTERRUPT_EV_ALL(GPP,2),
      INTERRUPT_EV_ALL(GPP,3),
      INTERRUPT_EV_ALL(GPP,4),
      INTERRUPT_EV_ALL(GPP,5),
      INTERRUPT_EV_ALL(GPP,6),
      INTERRUPT_EV_ALL(GPP,7),
      INTERRUPT_EV_ALL(GPP,8),
      INTERRUPT_8_12_EV_ALL(GPP,9),
      INTERRUPT_8_12_EV_ALL(GPP,10),
      INTERRUPT_8_12_EV_ALL(GPP,11),
      INTERRUPT_8_12_EV_ALL(GPP,12),


      GEN_EV_UNTIL_LIONB(BM_EMPTY_CLEAR),
      GEN_EV_ALL(BM_AGED_PACKET),
      GEN_EV_ALL(BM_MAX_BUFF_REACHED),

      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_UNDERRUN ,BM_PORT_RX_BUFFERS_CNT_UNDERRUN  ,0) ,
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_UNDERRUN ,BM_GE_CNT_UNDERRUN               ,1) ,
      UNTIL_LIONB_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_UNDERRUN ,BM_XG_CNT_UNDERRUN   ,2) ,
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_UNDERRUN ,BM_GLOBAL_CNT_UNDERRUN           ,3) ,

      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_OVERRUN  ,BM_PORT_RX_BUFFERS_CNT_OVERRUN   ,0) ,
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_OVERRUN  ,BM_GE_CNT_OVERRUN                ,1) ,
      UNTIL_LIONB_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_OVERRUN  ,BM_XG_CNT_OVERRUN    ,2) ,
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_PORT_RX_BUFFERS_CNT_OVERRUN  ,BM_GLOBAL_CNT_OVERRUN            ,3) ,

      GEN_EV_ALL(BM_WRONG_SRC_PORT),
      GEN_EV_UNTIL_LIONB(BM_MC_INC_OVERFLOW),
      GEN_EV_UNTIL_LIONB(BM_MC_INC_UNDERRUN),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,       BM_LL_PORT2_PARITY_ERROR, 0),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,       BM_LL_PORT1_PARITY_ERROR, 1),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,       BM_CNRL_MEM_PARITY_ERROR, 2),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,       BM_MC_CNT_PARITY_ERROR, 3),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,       BM_PORT_RX_FULL,        4),
      CH2_ONLY_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(BM_MISC,   BM_MPPM_PARTY_ERROR,    5),
      GEN_EV_ALL(BUF_MEM_MAC_ERROR),

      LOOP_ALL_PORTS(LINK_STATUS_CHANGED),/*LINK_CHANGE*/
      LOOP_ALL_PORTS_NO_CH3_XG(BM_MAX_BUFF_REACHED),/*BM_MAX_BUFF_REACHED*/
      LOOP_ALL_PORTS_GE_ONLY(AN_COMPLETED),/*PORT_AN_COMPLETED*/
      LOOP_ALL_PORTS(RX_FIFO_OVERRUN),/*RX_FIFO_OVERRUN*/
      LOOP_ALL_PORTS_GE_ONLY(TX_FIFO_UNDERRUN),/*PORT_TX_FIFO_UNDERRUN*/
      LOOP_ALL_PORTS_GE_ONLY(TX_FIFO_OVERRUN),/*PORT_TX_FIFO_OVERRUN*/
      LOOP_ALL_PORTS(TX_UNDERRUN),/*PORT_TX_UNDERRUN*/
      LOOP_ALL_PORTS(ADDRESS_OUT_OF_RANGE),/*PORT_ADDRESS_OUT_OF_RANGE*/

      GOP_MIB_EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,0),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,1),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,2),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_ADDRESS_OUT_OF_RANGE,3),

      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,0),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,1),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,2),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_EXPIRED,3),

      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,0),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,1),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,2),
      GOP_MIB_EV_INDEX_SPECIFIC(GOP_COUNT_COPY_DONE,3),

      LOOP_ALL_PORTS_GE_ONLY(SYNC_STATUS_CHANGED),/*PORT_SYNC_STATUS_CHANGED*/
      LOOP_ALL_PORTS_GE_ONLY(PRBS_ERROR),/*PORT_PRBS_ERROR*/
      LOOP_XCAT2_PORTS_GE_ONLY(PRBS_ERROR_QSGMII),/*PORT_PRBS_ERROR_QSGMII*/
      LOOP_XG_PORTS(FC_STATUS_CHANGED),/*PORT_FC_STATUS_CHANGED*/
      LOOP_XG_PORTS(ILLEGAL_SEQUENCE),/*PORT_ILLEGAL_SEQUENCE*/
      LOOP_XG_PORTS(FAULT_TYPE_CHANGE),/*PORT_FAULT_TYPE_CHANGE*/
      FROM_CH3_LOOP_XG_PORTS(NO_BUFF_PACKET_DROP),/*PORT_NO_BUFF_PACKET_DROP*/

      LOOP_HGSMIB_XG_PORTS_DIF_NAMES(COUNTER_WRAP, COUNT_EXPIRED),/*COUNT_EXPIRED*/
      LOOP_HGSMIB_XG_PORTS_DIF_NAMES(MIB_CAPTURE, COUNT_COPY_DONE),/*COUNT_COPY_DONE*/

      LOOP_ALL_PORTS_NO_CH3_XG_UNTIL_XCAT(TQ_WATCHDOG_EX),/*TQ_WATCHDOG_EX_PORT*/
      LOOP_ALL_PORTS_NO_CH3_XG(TQ_TXQ2_FLUSH),/*TQ_TXQ2_FLUSH_PORT*/
      LOOP_ALL_PORTS_NO_CH3(BM_TQ_PARITY_ERROR),/*BM_TQ_PARITY_ERROR_PORT*/


      GEN_EV_ALL(TQ_SNIFF_DESC_DROP),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TQ_MISC,TXQ_GEN_BAD_ADDR,              0),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TQ_MISC,TXQ_GEN_DESC_FULL,             1),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TQ_MISC,TXQ_GEN_BUF_FULL,              2),
      CH1_CH2_ONLY_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TQ_MISC,TXQ_GEN_PARITY_ERR_DESC_MEM,   3),
      CH1_CH2_ONLY_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TQ_MISC,TXQ_GEN_PARITY_ERR_PTR_MEM,    4),
      TXQ_REV_0_GEN_EV_ALL(TQ_TOTAL_DESC_UNDERFLOW),CPSS_TBD_BOOKMARK /* Lion B : what the event ? */
      TXQ_REV_0_GEN_EV_ALL(TQ_TOTAL_BUFF_UNDERFLOW),CPSS_TBD_BOOKMARK /* Lion B : what the event ? */

      FROM_CH3_NOT_LION_B_EV(TQ_MC_DESC_FULL),
      FROM_CH3_NOT_LION_B_EV(TQ_GIGA_FIFO_FULL),
      FROM_CH3_NOT_LION_B_EV(TQ_XG_MC_FIFO_FULL),

      FROM_CH3_LOOP_0_23_PORTS(TQ_DESC_FULL),
      FROM_CH3_ANY_LOOP_24_27_XG_PORTS(TQ_FULL_XG),/*the same like TQ_DESC_FULL but for ports 24..27*/

      GEN_EV_ALL(EB_NA_FIFO_FULL),
      GEN_EV_ALL(EB_SECURITY_BREACH_UPDATE),
      FROM_CH3_UNTIL_XCAT_EV(EB_TCC),/* from xcat see TCC_TCAM_ERROR_DETECTED */
      GEN_EV_ALL(MAC_NUM_OF_HOP_EXP),
      GEN_EV_ALL(MAC_NA_LEARNED),
      GEN_EV_ALL(MAC_NA_NOT_LEARNED),
      GEN_EV_ALL(MAC_AGE_VIA_TRIGGER_ENDED),
      GEN_EV_ALL(MAC_UPDATE_FROM_CPU_DONE),
      GEN_EV_ALL(MAC_MESSAGE_TO_CPU_READY),
      GEN_EV_ALL(MAC_NA_SELF_LEARNED),
      GEN_EV_ALL(MAC_NA_FROM_CPU_LEARNED),
      GEN_EV_ALL(MAC_NA_FROM_CPU_DROPPED),
      GEN_EV_ALL(MAC_AGED_OUT),
      GEN_EV_ALL(MAC_FIFO_2_CPU_EXCEEDED),
      GEN_EV_ALL(EB_AUQ_PENDING),
      GEN_EV_ALL(EB_AUQ_FULL),
      GEN_EV_ALL(EB_AUQ_OVER),
      GEN_EV_ALL(EB_AUQ_ALMOST_FULL),
      FROM_CH2_EV(EB_FUQ_PENDING),
      FROM_CH2_EV(EB_FUQ_FULL),
      FROM_CH3_EV(MISC_GENXS_READ_DMA_DONE),
      FROM_CH3_EV(MISC_PEX_ADDR_UNMAPPED),
      FROM_CH2_ONLY_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(EB_MG_ADDR_OUT_OF_RANGE,BRIDGE_ADDR_OUT_RANGE,0),

      LOOP_ALL_PORTS_NO_CPU_NO_CH3_XG(MAC_SFLOW),/*MAC_SFLOW*/
      LOOP_ALL_PORTS_NO_CPU_NO_CH3_XG(EGRESS_STC),/*EGRESS_STC*/

      LOOP_ALL_QUEUES(TX_BUFFER),/*TX_BUFFER_QUEUE*/
      LOOP_ALL_QUEUES(TX_ERR),/*TX_ERR_QUEUE*/
      LOOP_ALL_QUEUES(TX_END),/*TX_END_QUEUE*/
      QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(RX_BUFFER),/*RX_BUFFER_QUEUE*/
      QUEUE_EV_ALL_QUEUES_SPECIFIC_UNI_NAME(RX_ERR),/*RX_ERR_QUEUE*/

      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(RX_CNTR_OVERFLOW,SDMA_RX_ERROR_RES_CNT_OFF,0),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(RX_CNTR_OVERFLOW,SDMA_RX_BYTE_CNT_OFF,1),
      EV_INDEX_ONLY_DIFF_NAME_FOR_PP(RX_CNTR_OVERFLOW,SDMA_RX_PACKET_CNT_OFF,2),

      /* per port XPCS events */
      XPCS_LOOP_PORTS(DESKEW_TIMEOUT),/* DESKEW_TIMEOUT */
      XPCS_LOOP_PORTS_UNTIL_LION(DETECTED_COLUMN_IIAII),/* DETECTED_COLUMN_IIAII */
      XPCS_LOOP_PORTS_UNTIL_LION(DESKEW_ERROR),/* DESKEW_ERROR */
      XPCS_LOOP_PORTS(PPM_FIFO_UNDERRUN),/* PPM_FIFO_UNDERRUN */
      XPCS_LOOP_PORTS(PPM_FIFO_OVERRUN), /* PPM_FIFO_OVERRUN */
      XPCS_LOOP_PORTS(LINK_STATUS_CHANGED),/*LINK_STATUS_CHANGED*/

      /* per port XLG events */
      XLG_PCS_LOOP_PORTS(ALIGN_LOCK_LOST),/*ALIGN_LOCK_LOST*/

      /* per port per lane */
      LANE_LOOP_PORTS_UNTIL_LION(SIGNAL_DETECT_CHANGED),/* SIGNAL_DETECT_CHANGED */
      LANE_LOOP_PORTS_UNTIL_LION(SYNC_STATUS_CHANGED),/* SYNC_STATUS_CHANGED */
      LANE_LOOP_PORTS_UNTIL_LION(DETECTED_IIAII),/* DETECTED_IIAII */
      FROM_XCAT_LANE_LOOP_PORTS(PRBS_ERROR),/* ch-hx not support */
      FROM_XCAT_UNTILL_LION_LANE_LOOP_PORTS(DISPARITY_ERROR),  /* ch-hx ,lion not support */
      FROM_XCAT_UNTILL_LION_LANE_LOOP_PORTS(SYMBOL_ERROR),     /* ch-hx ,lion not support */

      /* for next 'per port per lane' that the CH-HX supports as 'per port' ,
        we bind it to 'per port for lane 0' */
      CH1_ONLY_LOOP_HX_PORTS_EMULATE_LANE_0(DISPARITY_ERROR),/* on HX those are not per lane , but per port */
      CH1_ONLY_LOOP_HX_PORTS_EMULATE_LANE_0(SYMBOL_ERROR),   /* on HX those are not per lane , but per port */
      CH1_ONLY_LOOP_HX_PORTS_EMULATE_LANE_0(PRBS_ERROR),     /* on HX those are not per lane , but per port */

      FROM_CH3_EV_PEX(PEX_DL_DOWN_TX_ACC_ERR),
      FROM_CH3_EV_PEX(PEX_MASTER_DISABLED),
      FROM_CH3_EV_PEX(PEX_ERROR_WR_TO_REG),
      FROM_CH3_EV_PEX(PEX_HIT_DEFAULT_WIN_ERR),
      FROM_CH3_EV_PEX(PEX_COR_ERROR_DET),
      FROM_CH3_EV_PEX(PEX_NON_FATAL_ERROR_DET),
      FROM_CH3_EV_PEX(PEX_FATAL_ERROR_DET),
      FROM_CH3_EV_PEX(PEX_DSTATE_CHANGED),
      FROM_CH3_EV_PEX(PEX_BIST),
      FROM_CH3_EV_PEX(PEX_RCV_ERROR_FATAL),
      FROM_CH3_EV_PEX(PEX_RCV_ERROR_NON_FATAL),
      FROM_CH3_EV_PEX(PEX_RCV_ERROR_COR),
      FROM_CH3_EV_PEX(PEX_RCV_CRS),
      FROM_CH3_EV_PEX(PEX_PEX_SLAVE_HOT_RESET),
      FROM_CH3_EV_PEX(PEX_PEX_SLAVE_DISABLE_LINK),
      FROM_CH3_EV_PEX(PEX_PEX_SLAVE_LOOPBACK),
      FROM_CH3_EV_PEX(PEX_PEX_LINK_FAIL),
      FROM_CH3_EV_PEX(PEX_RCV_A),
      FROM_CH3_EV_PEX(PEX_RCV_B),
      FROM_CH3_EV_PEX(PEX_RCV_C),
      FROM_CH3_EV_PEX(PEX_RCV_D),
      FROM_CH2_LOOP_ALL_SCT_DROP_PKTS(SCT_RATE_LIMITER),
      FROM_CH3_LOOP_BM_BANKS(BUF_MEM_ONE_ECC_ERROR, ONE_ECC_ERROR),
      FROM_CH3_LOOP_BM_BANKS(BUF_MEM_TWO_OR_MORE_ECC_ERRORS, TWO_OR_MORE_ECC_ERRORS),
      FROM_CH3_LOOP_CNC_BLOCKS(CNC_WRAPAROUND_BLOCK),
      FROM_CH3_EV(CNC_DUMP_FINISHED),

    /************************** TTI *****************************/
    FROM_XCAT_EV(TTI_CPU_ADDRESS_OUT_OF_RANGE),
    FROM_XCAT_EV(TTI_ACCESS_DATA_ERROR),


    /********************** Policer *************************/
    FROM_CH3_UNTIL_XCAT_EV(POLICER_DATA_ERR), /*index 0 --> iplr 0 */
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_DATA_ERR,IPLR0_SUM_DATA_ERROR,0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_DATA_ERR,EPLR_SUM_DATA_ERROR,1), /* no mistake --> index 1 (like puma)*/
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_DATA_ERR,IPLR1_SUM_DATA_ERROR,2),/* no mistake --> index 2 */

    FROM_CH3_UNTIL_XCAT_EV(POLICER_ADDR_OUT_OF_MEMORY), /*index 0 --> iplr 0 */
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_ADDR_OUT_OF_MEMORY,IPLR0_SUM_ADDRESS_OUT_OF_MEMORY,0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_ADDR_OUT_OF_MEMORY,EPLR_SUM_ADDRESS_OUT_OF_MEMORY,1), /* no mistake --> index 1 (like puma)*/
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_ADDR_OUT_OF_MEMORY,IPLR1_SUM_ADDRESS_OUT_OF_MEMORY,2),/* no mistake --> index 2 */

    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_WRAP_AROUND,IPLR0_SUM_IPFIX_WRAP_AROUND,0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_WRAP_AROUND,EPLR_SUM_IPFIX_WRAP_AROUND,1), /* no mistake --> index 1 (like puma)*/
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_WRAP_AROUND,IPLR1_SUM_IPFIX_WRAP_AROUND,2),/* no mistake --> index 2 */

    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_ALARM,IPLR0_SUM_IPFIX_ALARM,0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_ALARM,EPLR_SUM_IPFIX_ALARM,1), /* no mistake --> index 1 (like puma)*/
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_ALARM,IPLR1_SUM_IPFIX_ALARM,2),/* no mistake --> index 2 */

    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND,IPLR0_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND,0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND,EPLR_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND,1), /* no mistake --> index 1 (like puma)*/
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND,IPLR1_SUM_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND,2),/* no mistake --> index 2 */


    /********************** PCL *************************/
    EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_MG_ADDR_OUT_OF_RANGE,PCL_ADDR_OUT_RANGE,0),
    FROM_CH3_UNTIL_XCAT_EV(PCL_TCC_ECC_ERR), /* from xcat see TCC_TCAM_ERROR_DETECTED */

    /* Policy Engine Interrupt Cause: PCL0(LU0, LU1) and PCL1(LU0), Event index = 0,1,2 */
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_LOOKUP_DATA_ERROR, POLICY_ENGINE_PCL0_TCAM_ACCESS_DATA_ERROR, 0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_LOOKUP_DATA_ERROR, POLICY_ENGINE_PCL0_1_TCAM_ACCESS_DATA_ERROR, 1),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_LOOKUP_DATA_ERROR, POLICY_ENGINE_PCL1_TCAM_ACCESS_DATA_ERROR, 2),

    XCAT1_AND_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_CONFIG_TABLE_DATA_ERROR, POLICY_ENGINE_PCL_CONFIG_TABLE_0_PARITY_ERROR,0),
    XCAT1_AND_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_CONFIG_TABLE_DATA_ERROR, POLICY_ENGINE_PCL_CONFIG_TABLE_1_PARITY_ERROR,1),

    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_LOOKUP_FIFO_FULL, POLICY_ENGINE_PCL0_FIFO_FULL     , 0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_LOOKUP_FIFO_FULL, POLICY_ENGINE_PCL0_1_FIFO_FULL   , 1),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(PCL_LOOKUP_FIFO_FULL, POLICY_ENGINE_PCL1_FIFO_FULL     , 2),

    /************* TCC - Tcam ********************/
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TCC_TCAM_ERROR_DETECTED, TCC_LOWER_SUM_TCAM_ERROR_DETECTED,0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TCC_TCAM_ERROR_DETECTED, TCC_UPPER_SUM_TCAM_ERROR_DETECTED,1),

    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TCC_TCAM_BIST_FAILED, TCC_LOWER_SUM_TCAM_BIST_FAILED,0),
    FROM_XCAT_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(TCC_TCAM_BIST_FAILED, TCC_UPPER_SUM_TCAM_BIST_FAILED,1),

    /* BCN */
    FROM_XCAT_NOT_LION_B_EV(BCN_COUNTER_WRAP_AROUND_ERR),


    /* GTS - ingress - 0 , Egress - 1 */
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(GTS_GLOBAL_FIFO_FULL,  TTI_GTS_GLOBAL_FIFO_FULL,   0),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(GTS_GLOBAL_FIFO_FULL,  HA_GTS_GLOBAL_FIFO_OVERRUN, 1),

    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(GTS_VALID_TIME_SAMPLE_MESSAGE, TTI_GTS_VALID_TIME_SAMPLE_MESSAGE,  0),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(GTS_VALID_TIME_SAMPLE_MESSAGE, HA_GTS_VALID_TIME_SAMPLE_MESSAGE,   1),

    /* Critical HW Error */
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_BURST_FIFO_UNDERRUN, PRV_CPSS_LION_B_TXDMA_GEN_BURST_FIFO_UNDERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_CLEAR_FIFO_FULL    , PRV_CPSS_LION_B_TXDMA_GEN_CLEAR_FIFO_FULL_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_0_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_0_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_1_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_1_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_2_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_2_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_3_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_3_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_4_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_4_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_5_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_5_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_6_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_6_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_7_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_7_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_8_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_8_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_9_OVERRUN   , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_9_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_10_OVERRUN  , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_10_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_11_OVERRUN  , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_11_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_TXFIFO_12_OVERRUN  , PRV_CPSS_LION_B_TXDMA_GEN_TXFIFO_12_OVERRUN_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_CUT_THROUGH_ERROR  , PRV_CPSS_LION_B_TXDMA_GEN_CUT_THROUGH_ERROR_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, TXDMA_GEN_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS, PRV_CPSS_LION_B_TXDMA_GEN_HA_INFO_FIFO_ECC_TWO_OR_MORE_ERRORS_E),

    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, RXDMA_GEN_IBUF_SYNC_FIFO_0_FULL, PRV_CPSS_LION_B_RXDMA_GEN_IBUF_SYNC_FIFO_0_FULL_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, RXDMA_GEN_IBUF_SYNC_FIFO_1_FULL, PRV_CPSS_LION_B_RXDMA_GEN_IBUF_SYNC_FIFO_1_FULL_E),

    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP0_PORTA_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP0_PORTA_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP1_PORTA_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP1_PORTA_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP2_PORTA_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP2_PORTA_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP3_PORTA_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP3_PORTA_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP0_PORTB_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP0_PORTB_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP1_PORTB_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP1_PORTB_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP2_PORTB_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP2_PORTB_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP3_PORTB_ECC_ERR,        PRV_CPSS_LION_B_CTU_FCP3_PORTB_ECC_ERR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP0_START_ON_VALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP0_START_ON_VALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP1_START_ON_VALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP1_START_ON_VALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP2_START_ON_VALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP2_START_ON_VALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP3_START_ON_VALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP3_START_ON_VALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP0_END_ON_INVALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP0_END_ON_INVALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP1_END_ON_INVALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP1_END_ON_INVALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP2_END_ON_INVALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP2_END_ON_INVALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_OCP3_END_ON_INVALID_ENTRY, PRV_CPSS_LION_B_CTU_OCP3_END_ON_INVALID_ENTRY_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FIFO_OVERRUN,              PRV_CPSS_LION_B_CTU_FIFO_OVERRUN_E             ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP0_SAME_ADDR_WR_A_RD_B,  PRV_CPSS_LION_B_CTU_FCP0_SAME_ADDR_WR_A_RD_B_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP1_SAME_ADDR_WR_A_RD_B,  PRV_CPSS_LION_B_CTU_FCP1_SAME_ADDR_WR_A_RD_B_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP2_SAME_ADDR_WR_A_RD_B,  PRV_CPSS_LION_B_CTU_FCP2_SAME_ADDR_WR_A_RD_B_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP3_SAME_ADDR_WR_A_RD_B,  PRV_CPSS_LION_B_CTU_FCP3_SAME_ADDR_WR_A_RD_B_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP0_SAME_ADDR_WR_B_RD_A,  PRV_CPSS_LION_B_CTU_FCP0_SAME_ADDR_WR_B_RD_A_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP1_SAME_ADDR_WR_B_RD_A,  PRV_CPSS_LION_B_CTU_FCP1_SAME_ADDR_WR_B_RD_A_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP2_SAME_ADDR_WR_B_RD_A,  PRV_CPSS_LION_B_CTU_FCP2_SAME_ADDR_WR_B_RD_A_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_FCP3_SAME_ADDR_WR_B_RD_A,  PRV_CPSS_LION_B_CTU_FCP3_SAME_ADDR_WR_B_RD_A_E ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_DESC0_UNUSED_FIFO_ECC_ERR, PRV_CPSS_LION_B_CTU_DESC0_UNUSED_FIFO_ECC_ERR_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_DESC1_UNUSED_FIFO_ECC_ERR, PRV_CPSS_LION_B_CTU_DESC1_UNUSED_FIFO_ECC_ERR_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_DESC2_UNUSED_FIFO_ECC_ERR, PRV_CPSS_LION_B_CTU_DESC2_UNUSED_FIFO_ECC_ERR_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, CTU_DESC3_UNUSED_FIFO_ECC_ERR, PRV_CPSS_LION_B_CTU_DESC3_UNUSED_FIFO_ECC_ERR_E),

    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, BMA_DIST_CLEAR_FIFO_ECC_ERR,      PRV_CPSS_LION_B_BMA_DIST_CLEAR_FIFO_ECC_ERR_E   ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, BMA_MC_CNT_PARITY_ERROR,          PRV_CPSS_LION_B_BMA_MC_CNT_PARITY_ERROR_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, BMA_MC_CNT_RMW_CLEAR_FIFO_FULL,   PRV_CPSS_LION_B_BMA_MC_CNT_RMW_CLEAR_FIFO_FULL_E),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, BMA_MC_CNT_COUNTER_FLOW,          PRV_CPSS_LION_B_BMA_MC_CNT_COUNTER_FLOW_E       ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, BMA_NEXT_FIFO_FULL,               PRV_CPSS_LION_B_BMA_NEXT_FIFO_FULL_E            ),
    FROM_LION_EV_INDEX_ONLY_DIFF_NAME_FOR_PP(CRITICAL_HW_ERROR, BMA_CLEAR_SHIFTER_ECC_ERR,        PRV_CPSS_LION_B_BMA_CLEAR_SHIFTER_ECC_ERR_E     ),
};

#define NUM_OF_TBL_ENTRIES \
    (sizeof(ppUni2IntCauseType)/(sizeof(GT_U16)*TABLE_ENTRY_SIZE_E))

/*******************************************************************************
* ppColumnGet
*
* DESCRIPTION:
*       Returns the pp column in conversion table. (per port group)
*
* INPUTS:
*       devNum     - the device number that set the event .
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
        case CPSS_PP_FAMILY_CHEETAH_E:
            baseDevType = CHEETAH_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH2_E:
            baseDevType = CHEETAH2_E;
            break;
        case CPSS_PP_FAMILY_CHEETAH3_E:
            baseDevType = CHEETAH3_E;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            if(PRV_CPSS_PP_MAC(devNum)->revision != 0)
            {
                baseDevType = XCAT_E;
            }
            else
            {
                baseDevType = CHEETAH3_E;
            }
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            baseDevType = XCAT_E;
            break;
        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            baseDevType = XCAT2_E;
            break;
        case CPSS_PP_FAMILY_DXCH_LION_E:
                baseDevType = LION_B_E;
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
* chIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type -- Cheetah 1,2,3,xcat
*
* INPUTS:
*       devNum      - The device number.
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
static GT_STATUS chIntCauseToUniEvConvert
(
    IN  GT_U8                       devNum,
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

    /* event isn't found */
    return/* it's not error for LOG */GT_NOT_FOUND;
}


/*******************************************************************************
* prvCpssDrvPpIntCheetahUniEvToIntCauseConvert
*
* DESCRIPTION:
*       Converts unified event type and extra data to Interrupt Cause event
*       -- cheetah devices
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
GT_STATUS prvCpssDrvPpIntCheetahUniEvToIntCauseConvert
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      uniEv,
    IN  GT_U32                      extData,
    OUT  GT_U32                     *intCauseIndexPtr
)
{
    GT_STATUS       rc;
    GT_U32          ii;          /* iterator                     */
    GT_U32          ppColumn;   /* conversion table pp column   */

    CPSS_NULL_PTR_CHECK_MAC(intCauseIndexPtr);

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


/*******************************************************************************
* lionIsPerPortPerLaneInterrupt
*
* DESCRIPTION:
*       check is Interrupt Cause event is per port per lane -- Lion devices
*       --> per Port Group (to support multi-port-groups device)
*
* INPUTS:
*       devNum      - The device number.
*       intCauseIndex - The interrupt cause to convert.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE - interrupt is per port per lane
*       GT_FALSE - interrupt is NOT per port per lane
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL lionIsPerPortPerLaneInterrupt(
    IN  GT_U8                       devNum,
    IN  GT_U32                      intCauseIndex
)
{
    GT_U32  ii;
    GT_U32  port;/* port iterator */
    GT_U32  portMax = 12;/* max phy port per port group in lion*/
    GT_U32  numRegisters;/*max number of registers to check*/
    GT_U32  offset;/* offset of the uniEvent from the base uniEvent*/
    GT_U32  startIndex;/* temp value for start index */
    PER_PORT_INTERRUPTS_STC  *currPerPortPerLaneInterrupts;/* (pointer to)current info */
    GT_U32  currPerPortPerLaneInterruptsSize;

    if(prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        currPerPortPerLaneInterrupts = lionBPerPortPerLaneInterrupts;
        currPerPortPerLaneInterruptsSize = lionBPerPortPerLaneInterruptsSize;
    }
    else
    {
        return GT_FALSE;
    }

    for(ii = 0;ii < currPerPortPerLaneInterruptsSize ; ii++)
    {
        CPSS_COVERITY_NON_ISSUE_BOOKMARK
        /* coverity[overrun-local] */
        if(currPerPortPerLaneInterrupts[ii].startIndex > intCauseIndex)
        {
            /* intCauseIndex out of lower range */
            continue;
        }

        /* offset in indexes */
        offset =  currPerPortPerLaneInterrupts[ii].registerOffset.nextPortIndex -
                  currPerPortPerLaneInterrupts[ii].startIndex;

        if(intCauseIndex > ((offset * (portMax-1))+ 32 + currPerPortPerLaneInterrupts[ii].startIndex))
        {
            /* intCauseIndex out of upper range */
            continue;
        }

        if(currPerPortPerLaneInterrupts[ii].registerOffset.numRegisters)
        {
            numRegisters = currPerPortPerLaneInterrupts[ii].registerOffset.numRegisters;
        }
        else
        {
            numRegisters = 1;
        }

        /* check that the intCauseIndex is really fit to on of the registers */
        for(port = 0 ; port < portMax ; port++)
        {
            startIndex = (offset * port) + currPerPortPerLaneInterrupts[ii].startIndex;

            if(intCauseIndex < startIndex || intCauseIndex >= (startIndex + (32 * numRegisters)))
            {
                /* intCauseIndex out of upper/lower range of the port */
                continue;
            }

            /* intCauseIndex belongs to this port */
            return GT_TRUE;
        }
    }/*ii*/

    return GT_FALSE;
}

/*******************************************************************************
* lionIsPerPortInterrupt
*
* DESCRIPTION:
*       check is Interrupt Cause event is per port -- Lion devices
*       --> per Port Group (to support multi-port-groups device)
*
* INPUTS:
*       devNum      - The device number.
*       intCauseIndex - The interrupt cause to convert.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE - interrupt is per port
*       GT_FALSE - interrupt is NOT per port
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL lionIsPerPortInterrupt(
    IN  GT_U8                       devNum,
    IN  GT_U32                      intCauseIndex
)
{
    GT_U32  ii;
    GT_U32  port;/* port iterator */
    GT_U32  portMax = 12;/* max phy port per port group in lion*/
    GT_U32  numRegisters;/*max number of registers to check*/
    GT_U32  offset;/* offset of the uniEvent from the base uniEvent*/
    GT_U32  startIndex;/* temp value for start index */
    PER_PORT_INTERRUPTS_STC  *currPerPortInterrupts;/* (pointer to)current info */
    GT_U32  currPerPortInterruptsSize;

    if(prvCpssDrvPpConfig[devNum]->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
    {
        currPerPortInterrupts = lionBPerPortInterrupts;
        currPerPortInterruptsSize = lionBPerPortInterruptsSize;
    }
    else
    {
        return GT_FALSE;
    }

    for(ii = 0;ii < currPerPortInterruptsSize ; ii++)
    {
        if(currPerPortInterrupts[ii].fullRegister == GT_TRUE)
        {
            if(currPerPortInterrupts[ii].startIndex > intCauseIndex)
            {
                /* intCauseIndex out of lower range */
                continue;
            }

            /* offset in indexes */
            offset =  currPerPortInterrupts[ii].registerOffset.nextPortIndex -
                      currPerPortInterrupts[ii].startIndex;

            if(intCauseIndex > ((offset * (portMax-1))+ 32 + currPerPortInterrupts[ii].startIndex))
            {
                /* intCauseIndex out of upper range */
                continue;
            }

            if(currPerPortInterrupts[ii].registerOffset.numRegisters)
            {
                numRegisters = currPerPortInterrupts[ii].registerOffset.numRegisters;
            }
            else
            {
                numRegisters = 1;
            }

            /* check that the intCauseIndex is really fit to on of the registers */
            for(port = 0 ; port < portMax ; port++)
            {
                startIndex = (offset * port) + currPerPortInterrupts[ii].startIndex;

                if(intCauseIndex < startIndex || intCauseIndex >= (startIndex + (32 * numRegisters)))
                {
                    /* intCauseIndex out of upper/lower range of the port */
                    continue;
                }

                /* intCauseIndex belongs to this port */
                return GT_TRUE;
            }
        }
        else /* single bit per port */
        {
            if(currPerPortInterrupts[ii].startIndex > intCauseIndex ||
               (currPerPortInterrupts[ii].startIndex + portMax) <=  intCauseIndex)
            {
                /* intCauseIndex out of upper/lower range */
                continue;
            }


            return GT_TRUE;
        }
    }/*ii*/

    return GT_FALSE;
}

/*******************************************************************************
* muliPortGroupIndicationCheckAndConvert
*
* DESCRIPTION:
*       for multi-port Group device
*       check if current uni-event need Convert due to multi-port group indication
*
* INPUTS:
*       portGroupId      - The port group Id.
*       uniEv            - unified event
*       extDataPtr  - (pointer to)The event extended data.
*
* OUTPUTS:
*       extDataPtr  - (pointer to)The event extended data.
*
* RETURNS:
*       none
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void muliPortGroupIndicationCheckAndConvert(
    IN      GT_U32                    portGroupId,
    IN      GT_U32                    uniEv,
    INOUT   GT_U32                    *extDataPtr
)
{
    switch(uniEv)
    {
        /* GPP */
        case CPSS_PP_GPP_E:
        /*CNC*/
        case CPSS_PP_CNC_WRAPAROUND_BLOCK_E:
        case CPSS_PP_CNC_DUMP_FINISHED_E:
        /*Policer*/
        case CPSS_PP_POLICER_DATA_ERR_E:
        case CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E:
        case CPSS_PP_POLICER_IPFIX_ALARM_E:
        case CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E:
        case CPSS_PP_DATA_INTEGRITY_ERROR_E:
            break;
        default:
            return;
    }

    /* convert for those events */
    *extDataPtr += PORT_GROUP_ID_TO_EXT_DATA_CONVERT(portGroupId);

    return;
}


/*******************************************************************************
* chIntCauseToUniEvConvertDedicatedTables
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. - For devices
*       with dedicated tables (Lion2).
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
static GT_STATUS chIntCauseToUniEvConvertDedicatedTables
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portGroupId,
    IN  GT_U32                      intCauseIndex,
    OUT GT_U32                      *uniEvPtr,
    OUT GT_U32                      *extDataPtr
)
{
    GT_U32   ii; /* iterator                     */
    GT_U16   *tableWithExtDataPtr;
    GT_U32   tableWithoutExtDataSize,tableWithExtDataSize;
    PRV_CPSS_DRV_EVENT_INFO_TYPE   *tableWithoutExtDataPtr;
    GT_U32   tmpUniEvent;
    GT_U32   tmpExtData;
    GT_U32   numReservedPorts;

    CPSS_NULL_PTR_CHECK_MAC(uniEvPtr);
    CPSS_NULL_PTR_CHECK_MAC(extDataPtr);

    *uniEvPtr   = CPSS_UNI_RSRVD_EVENT_E;
    *extDataPtr = 0;

    /* get the tables */
    tableWithExtDataPtr     = prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataArr;
    tableWithExtDataSize    = prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataSize;
    tableWithoutExtDataPtr  = prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableArr;
    tableWithoutExtDataSize = prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableSize;
    numReservedPorts        = prvCpssDrvPpConfig[devNum]->intCtrl.numReservedPorts;

    if(tableWithoutExtDataPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* Search the map table for unified event without extended data */
    for (ii=0; ii < tableWithoutExtDataSize;ii++)
    {
        if (tableWithoutExtDataPtr[ii][1] == intCauseIndex)
        {
            /* found */
            *uniEvPtr = tableWithoutExtDataPtr[ii][0];
            muliPortGroupIndicationCheckAndConvert(portGroupId,*uniEvPtr,extDataPtr);
            if(CPSS_PP_PORT_802_3_AP_E == *uniEvPtr)
            {
                *uniEvPtr += portGroupId;
            }
            return GT_OK;
        }
    }

    ii = 0;
    /* Search the map table for unified event with extended data */
    while (ii < tableWithExtDataSize)
    {
        /* remember the uni event */
        tmpUniEvent = tableWithExtDataPtr[ii++];
        while (tableWithExtDataPtr[ii] != MARK_END_OF_UNI_EV_CNS)
        {
            if (tableWithExtDataPtr[ii] ==  intCauseIndex)
            {
                /* found */
                *uniEvPtr = tmpUniEvent;
                tmpExtData = tableWithExtDataPtr[ii+1];
                if ((tmpUniEvent == CPSS_PP_DATA_INTEGRITY_ERROR_E) || (tmpUniEvent == CPSS_PP_CRITICAL_HW_ERROR_E))
                {
                    /* These unified events use interrupt enum as extData.
                       There is no conversion is used. */
                    *extDataPtr = tmpExtData;
                }
                else if(tmpUniEvent == CPSS_PP_PORT_EEE_E)/* patch because 'port indication' not supports '<< 8' */
                {
                    GT_U32  portNum = U32_GET_FIELD_MAC(tmpExtData,8,8);/* from bit 8 take 8 bits */
                    GT_U32  extVal  = U32_GET_FIELD_MAC(tmpExtData,0,8);/* from bit 0 take 8 bits */

                    /* convert the port to 'global port' */
                    /* each port group has it's reserved ports */
                    portNum += (numReservedPorts * portGroupId);

                    /* rebuild the '*extDataPtr' */
                    *extDataPtr = extVal | (portNum << 8);
                }
                else
                {
                    *extDataPtr = CLEAR_MARK_INT_MAC(tmpExtData);
                    if (IS_MARKED_PER_PORT_PER_LANE_INT_MAC(tmpExtData))
                    {
                        /* each port group has 16 reserved ports */
                        /* each port has 256 reserved lanes */
                        *extDataPtr +=
                            (LANE_PORT_TO_EXT_DATA_CONVERT(numReservedPorts,0) * portGroupId);
                    }
                    else if (IS_MARKED_PER_PORT_INT_MAC(tmpExtData))
                    {
                       /* each port group has 16 reserved ports */
                        *extDataPtr += (numReservedPorts * portGroupId);
                    }
                    else if (IS_MARKED_PER_HEM_INT_MAC(tmpExtData))
                    {
                       /* each hemisphere group has 64 reserved ports */
                        *extDataPtr += (portGroupId >= 4) ? 64 : 0;
                    }
                }

                muliPortGroupIndicationCheckAndConvert(portGroupId,*uniEvPtr,extDataPtr);
                return GT_OK;
            }
            ii +=2;
        }
        ii++;
    }

    DBG_INFO(("file:%s line:%d event not found intCause = %d\n", __FILE__,
              __LINE__, intCauseIndex));

    return /* do not register as error to the LOG */GT_NOT_FOUND;
}

/*******************************************************************************
* prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert
*
* DESCRIPTION:
*       Converts Interrupt Cause event to unified event type. -- cheetah devices
*       --> per Port Group (to support multi-port-groups device)
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
GT_STATUS prvCpssDrvPpPortGroupIntCheetahIntCauseToUniEvConvert
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
    GT_BOOL         isPerPort;/* is the interrupt per port */
    GT_BOOL         isPerPortPerLane;/*is the interrupt per port per lane */
    GT_U32          numReservedPorts;

    if(prvCpssDrvPpConfig[devNum]->intCtrl.uniEvMapTableWithExtDataArr)
    {
        /* Lion2 uses dedicated explicit tables, i.e. ppUni2IntCauseType is not used */
        return chIntCauseToUniEvConvertDedicatedTables(devNum,
                portGroupId,intCauseIndex,uniEvPtr,extDataPtr);
    }
    else
    if(prvCpssDrvPpConfig[devNum]->devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        return chIntCauseToUniEvConvert(devNum,
                intCauseIndex,uniEvPtr,extDataPtr);
    }

    /* LION handling */

    CPSS_NULL_PTR_CHECK_MAC(uniEvPtr);
    CPSS_NULL_PTR_CHECK_MAC(extDataPtr);

    numReservedPorts = 16;

    *uniEvPtr   = CPSS_UNI_RSRVD_EVENT_E;
    *extDataPtr = 0;

    rc = ppColumnGet(devNum, &ppColumn);
    if(rc != GT_OK)
    {
        return rc;
    }

    /*check if the interrupt is 'per port' */
    isPerPortPerLane = lionIsPerPortPerLaneInterrupt(devNum,intCauseIndex);

    if(isPerPortPerLane == GT_TRUE)
    {
        isPerPort = GT_FALSE;/* just to avoid compilation warning */
    }
    else
    {
        /*check if the interrupt is 'per port' */
        isPerPort = lionIsPerPortInterrupt(devNum,intCauseIndex);
    }

    for (ii = 0; ii < NUM_OF_TBL_ENTRIES ; ii++)
    {
        if (ppUni2IntCauseType[ii][ppColumn] == intCauseIndex)
        {
            /* need to check if the interrupt relate to 'per port' and therefore
               need to be converted to 'global index' rather then local port group's index
             */

            *uniEvPtr   = ppUni2IntCauseType[ii][UNI_EV_E];
            *extDataPtr = ppUni2IntCauseType[ii][EXT_DATA_E];

            if(isPerPortPerLane == GT_TRUE)
            {
                /* each port group has 16 reserved ports */
                /* each port has 256 reserved lanes */
                *extDataPtr += (LANE_PORT_TO_EXT_DATA_CONVERT(numReservedPorts,0) * portGroupId);

            }
            else if(isPerPort == GT_TRUE)
            {
                /* each port group has 16 reserved ports */
                *extDataPtr += (numReservedPorts * portGroupId);
            }
            else
            {
                /* check if need to convert extParam - due to multi-port group indication */
                muliPortGroupIndicationCheckAndConvert(portGroupId,*uniEvPtr,extDataPtr);
            }

            return GT_OK;
        }
    }
    DBG_INFO(("file:%s line:%d event not found intCause = %d\n", __FILE__,
              __LINE__, intCauseIndex));

    return /* do not register as error to the LOG */GT_NOT_FOUND;
}


#ifdef ASIC_SIMULATION
    #define PRV_CPSS_DRV_DXCH_PRINT_EVENTS
#endif /*ASIC_SIMULATION*/

#ifdef PRV_CPSS_DRV_DXCH_PRINT_EVENTS

GT_STATUS prvCpssDrvDxChPrintEvents
(
    GT_U32  deviceFamily/* 0 - all , 1 - ch1 , 2 - ch2 , 3-ch3 ,4 -xcat,5 lion , 6 - lion-B ..*/,
    GT_U32  extraDeviceType/* 0 - all , 1 - ch1Hx , 2 - ch1 diamond Cut */
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
    char* familyNames[] = {"empty" , "CH1" , "CH2" , "CH3" , "XCAT", "XCAT2" ,
                                "LION_B" , "???" , "???", "???" , "???"};

    (void)extraDeviceType;/* not supported parameter (yet) */

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


    /* print the events with port group indication */
    if (deviceFamily == LION_B_E)
    {
        GT_U32  dummyUniEvent;
        GT_U32  dummyPortGroupId = 1;

        cpssOsPrintf(" events with port group indication : \n");

        for (dummyUniEvent = 0; dummyUniEvent < CPSS_UNI_EVENT_COUNT_E; dummyUniEvent++)
        {
            GT_U32  dummyExtParam1,dummyExtParam2;

            dummyExtParam1 = dummyExtParam2 = 5;

            muliPortGroupIndicationCheckAndConvert(dummyPortGroupId,dummyUniEvent,&dummyExtParam2);

            if(dummyExtParam1 != dummyExtParam2)
            {
                /* there was conversion */
                cpssOsPrintf("%s \n",uniEvName[dummyUniEvent]);
            }
        }

        cpssOsPrintf(" End of events with port group indication \n");
    }


    return GT_OK;
}
#endif /*PRV_CPSS_DRV_DXCH_PRINT_EVENTS*/


