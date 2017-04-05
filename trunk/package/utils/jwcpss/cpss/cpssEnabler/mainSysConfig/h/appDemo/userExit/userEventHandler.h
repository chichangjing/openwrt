/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* userEventHandler.h
*
* DESCRIPTION:
*       Application-Demo Request Driven event handler
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/

#ifndef __userEventHandlerh
#define __userEventHandlerh

#include <cpss/generic/events/cpssGenEventUnifyTypes.h>
#include <cpss/generic/events/cpssGenEventRequests.h>
#include <cpss/extServices/cpssExtServices.h>
#include <extUtils/rxEventHandler/rxEventHandler.h>

#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#endif
#ifdef EXMXPM_FAMILY
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef MAX_NUM_DEVICES
#define MAX_NUM_DEVICES 128
#endif
extern GT_U32   *uniEventCounters[MAX_NUM_DEVICES];

typedef GT_STATUS EVENT_COUNTER_INCREMENT_FUNC
(
    IN  GT_U8   devNum,
    IN GT_U32   uniEvCounter,
    IN GT_U32   evExtData
);

/****************************************************************************/
/* App-Demo (priority 0 Lowest) default events                              */
/****************************************************************************/
#define APP_DEMO_P0_UNI_EV_DEFAULT { CPSS_PP_SLV_WRITE_ERR_E                    \
                                    ,CPSS_PP_MAS_WRITE_ERR_E                    \
                                    ,CPSS_PP_ADDR_ERR_E                         \
                                    ,CPSS_PP_MAS_ABORT_E                        \
                                    ,CPSS_PP_TARGET_ABORT_E                     \
                                    ,CPSS_PP_SLV_READ_ERR_E                     \
                                    ,CPSS_PP_RETRY_CNTR_E                       \
                                    ,CPSS_PP_MISC_C2C_W_FAR_END_UP_E            \
                                    ,CPSS_PP_MISC_C2C_N_FAR_END_UP_E            \
                                    ,CPSS_PP_MISC_C2C_DATA_ERR_E                \
                                    ,CPSS_PP_MISC_MSG_TIME_OUT_E                \
                                    ,CPSS_PP_MISC_ILLEGAL_ADDR_E                \
                                    ,CPSS_PP_INTERNAL_PHY_E                     \
                                    ,CPSS_PP_GPP_E                              \
                                    ,CPSS_PP_BM_MAX_BUFF_REACHED_E              \
                                    ,CPSS_PP_BM_INVALID_ADDRESS_E               \
                                    ,CPSS_PP_BM_RX_MEM_READ_ECC_ERROR_E         \
                                    ,CPSS_PP_BM_VLT_ECC_ERR_E                   \
                                    ,CPSS_PP_BM_MISC_E                          \
                                    ,CPSS_PP_PORT_AN_COMPLETED_E                \
                                    ,CPSS_PP_PORT_SYNC_STATUS_CHANGED_E         \
                                    ,CPSS_PP_PORT_RX_FIFO_OVERRUN_E             \
                                    ,CPSS_PP_PORT_TX_FIFO_UNDERRUN_E            \
                                    ,CPSS_PP_PORT_TX_FIFO_OVERRUN_E             \
                                    ,CPSS_PP_PORT_TX_UNDERRUN_E                 \
                                    ,CPSS_PP_PORT_ADDRESS_OUT_OF_RANGE_E        \
                                    ,CPSS_PP_TQ_WATCHDOG_EX_PORT_E              \
                                    ,CPSS_PP_TQ_TXQ2_FLUSH_PORT_E               \
                                    ,CPSS_PP_TQ_TXQ2_MG_FLUSH_E                 \
                                    ,CPSS_PP_TQ_ONE_ECC_ERROR_E                 \
                                    ,CPSS_PP_TQ_TWO_ECC_ERROR_E                 \
                                    ,CPSS_PP_TQ_MG_READ_ERR_E                   \
                                    ,CPSS_PP_TQ_TOTAL_DESC_UNDERFLOW_E          \
                                    ,CPSS_PP_TQ_TOTAL_DESC_OVERFLOW_E           \
                                    ,CPSS_PP_TQ_SNIFF_DESC_DROP_E               \
                                    ,CPSS_PP_TQ_MLL_PARITY_ERR_E                \
                                    ,CPSS_PP_TQ_MC_FIFO_OVERRUN_E               \
                                    ,CPSS_PP_TQ_RED_REACHED_PORT_E              \
                                    ,CPSS_PP_TQ_MISC_E                          \
                                    ,CPSS_PP_EB_MG_ADDR_OUT_OF_RANGE_E          \
                                    ,CPSS_PP_EB_NA_NOT_LEARNED_SECURITY_BREACH_E\
                                    ,CPSS_PP_EB_VLAN_SECURITY_BREACH_E          \
                                    ,CPSS_PP_EB_VLAN_TBL_OP_DONE_E              \
                                    ,CPSS_PP_EB_INGRESS_FILTER_PCKT_E           \
                                    ,CPSS_PP_EB_NA_FIFO_FULL_E                  \
                                    ,CPSS_PP_EB_SA_MSG_DISCARDED_E              \
                                    ,CPSS_PP_MAC_SFLOW_E                        \
                                    ,CPSS_PP_MAC_NUM_OF_HOP_EXP_E               \
                                    ,CPSS_PP_MAC_TBL_READ_ECC_ERR_E             \
                                    ,CPSS_PP_MAC_NA_LEARNED_E                   \
                                    ,CPSS_PP_MAC_NA_NOT_LEARNED_E               \
                                    ,CPSS_PP_MAC_NA_FROM_CPU_LEARNED_E          \
                                    ,CPSS_PP_MAC_MG_ADDR_OUT_OF_RANGE_E         \
                                    ,CPSS_PP_MAC_FIFO_2_CPU_EXCEEDED_E          \
                                    ,CPSS_PP_LX_LB_ERR_E                        \
                                    ,CPSS_PP_LX_TCB_CNTR_E                      \
                                    ,CPSS_PP_LX_IPV4_MC_ERR_E                   \
                                    ,CPSS_PP_LX_IPV4_LPM_ERR_E                  \
                                    ,CPSS_PP_LX_IPV4_ROUTE_ERR_E                \
                                    ,CPSS_PP_LX_IPV4_CNTR_E                     \
                                    ,CPSS_PP_LX_L3_L7_ERR_ADDR_E                \
                                    ,CPSS_PP_LX_TRUNK_ADDR_OUT_OF_RANGE_E       \
                                    ,CPSS_PP_LX_IPV4_REFRESH_AGE_OVERRUN_E      \
                                    ,CPSS_PP_LX_PCE_PAR_ERR_E                   \
                                    ,CPSS_PP_LX_TC_2_RF_CNTR_ALRM_E             \
                                    ,CPSS_PP_LX_TC_2_RF_PLC_ALRM_E              \
                                    ,CPSS_PP_LX_TC_2_RF_TBL_ERR_E               \
                                    ,CPSS_PP_LX_CTRL_MEM_2_RF_ERR_E             \
                                    ,CPSS_PP_RX_CNTR_OVERFLOW_E                 \
                                    ,CPSS_PP_MAC_PACKET_RSSI_LESS_THAN_THRESH_E \
                                    ,CPSS_PP_MAC_UPD_AVG_RSSI_LESS_THAN_THRESH_E\
                                    ,CPSS_FA_UC_PRIO_BUF_FULL_E                 \
                                    ,CPSS_FA_MC_PRIO_BUF_FULL_E                 \
                                    ,CPSS_FA_UC_RX_BUF_FULL_E                   \
                                    ,CPSS_FA_MC_RX_BUF_FULL_E                   \
                                    ,CPSS_FA_WD_FPORT_GRP_E                     \
                                    ,CPSS_FA_MC_WD_EXP_E                        \
                                    ,CPSS_FA_GLBL_DESC_FULL_E                   \
                                    ,CPSS_FA_MAC_UPD_THRS_E                     \
                                    ,CPSS_FA_DROP_DEV_EN_E                      \
                                    ,CPSS_FA_MISC_I2C_TIMEOUT_E                 \
                                    ,CPSS_FA_MISC_STATUS_E                      \
                                    ,CPSS_XBAR_GPORT_INVALID_TRG_DROP_CNT_INC_E \
                                    ,CPSS_XBAR_GPORT_DIAG_CELL_RX_CNTR_INC_E    \
                                    ,CPSS_XBAR_MISC_E                           \
                                    ,CPSS_XBAR_HOST_SEND_CELL_E                 \
                                    ,CPSS_XBAR_HG_ALIGNMENT_LOCK_CHANGE_E       \
                                    ,CPSS_XBAR_HG_LANE_COMMA_ALIGN_CHANGE_E     \
                                    ,CPSS_XBAR_HG_PING_E                        \
                                    ,CPSS_XBAR_SXBAR_INV_ADDR_E                 \
                                    ,CPSS_XBAR_S_INV_ADDR_E                     \
                                    ,CPSS_XBAR_G_INV_ADDR_E                     \
                                    ,CPSS_XBAR_MC_INV_ADDR_E                    \
                                    ,CPSS_XBAR_SD_INV_ADDR_E                    \
                                    ,CPSS_PP_MISC_PEX_ADDR_UNMAPPED_E           \
                                    ,CPSS_PP_MISC_GENXS_READ_DMA_DONE_E         \
                                    ,CPSS_PP_BM_TRIGGER_AGING_DONE_E            \
                                    ,CPSS_PP_GOP_COUNT_COPY_DONE_E              \
                                    ,CPSS_PP_GOP_COUNT_EXPIRED_E                \
                                    ,CPSS_PP_EB_TCC_E                           \
                                    ,CPSS_PP_BUF_MEM_MAC_ERROR_E                \
                                    ,CPSS_PP_BUF_MEM_ONE_ECC_ERROR_E            \
                                    ,CPSS_PP_BUF_MEM_TWO_OR_MORE_ECC_ERRORS_E   \
                                    ,CPSS_PP_POLICER_ADDR_OUT_OF_MEMORY_E       \
                                    ,CPSS_PP_POLICER_DATA_ERR_E                 \
                                    ,CPSS_PP_POLICER_IPFIX_WRAP_AROUND_E        \
                                    ,CPSS_PP_POLICER_IPFIX_ALARM_E              \
                                    ,CPSS_PP_POLICER_IPFIX_ALARMED_ENTRIES_FIFO_WRAP_AROUND_E \
                                    ,CPSS_PP_PCL_TCC_ECC_ERR_E                  \
                                    ,CPSS_PP_PCL_MG_ADDR_OUT_OF_RANGE_E         \
                                    ,CPSS_PP_PCL_LOOKUP_DATA_ERROR_E            \
                                    ,CPSS_PP_PCL_ACTION_ERROR_DETECTED_E        \
                                    ,CPSS_PP_PCL_MG_LOOKUP_RESULTS_READY_E      \
                                    ,CPSS_PP_PCL_INLIF_TABLE_DATA_ERROR_E       \
                                    ,CPSS_PP_PCL_CONFIG_TABLE_DATA_ERROR_E      \
                                    ,CPSS_PP_PCL_LOOKUP_FIFO_FULL_E             \
                                    ,CPSS_PP_CNC_WRAPAROUND_BLOCK_E             \
                                    ,CPSS_PP_CNC_DUMP_FINISHED_E                \
                                    ,CPSS_PP_SCT_RATE_LIMITER_E                 \
                                    ,CPSS_PP_EGRESS_SFLOW_E                     \
                                    ,CPSS_PP_INGRESS_SFLOW_SAMPLED_CNTR_E       \
                                    ,CPSS_PP_EGRESS_SFLOW_SAMPLED_CNTR_E        \
                                    ,CPSS_PP_TTI_CPU_ADDRESS_OUT_OF_RANGE_E     \
                                    ,CPSS_PP_TTI_ACCESS_DATA_ERROR_E            \
                                    ,CPSS_PP_TCC_TCAM_ERROR_DETECTED_E          \
                                    ,CPSS_PP_TCC_TCAM_BIST_FAILED_E             \
                                    ,CPSS_PP_BCN_COUNTER_WRAP_AROUND_ERR_E      \
                                    ,CPSS_PP_GTS_GLOBAL_FIFO_FULL_E             \
                                    ,CPSS_PP_GTS_VALID_TIME_SAMPLE_MESSAGE_E    \
                                    ,CPSS_PP_TX_END_E                           \
                                    ,CPSS_PP_TX_ERR_QUEUE_E                     \
                                    ,CPSS_PP_BM_EMPTY_CLEAR_E                   \
                                    ,CPSS_PP_BM_PORT_RX_BUFFERS_CNT_UNDERRUN_E  \
                                    ,CPSS_PP_BM_PORT_RX_BUFFERS_CNT_OVERRUN_E   \
                                    ,CPSS_PP_BM_WRONG_SRC_PORT_E                \
                                    ,CPSS_PP_BM_MC_INC_OVERFLOW_E               \
                                    ,CPSS_PP_BM_MC_INC_UNDERRUN_E               \
                                    ,CPSS_PP_CRITICAL_HW_ERROR_E                \
                                    ,CPSS_PP_PEX_HIT_DEFAULT_WIN_ERR_E          \
                                    ,CPSS_PP_DATA_INTEGRITY_ERROR_E         \
                                    ,CPSS_PP_MAC_BANK_LEARN_COUNTERS_OVERFLOW_E \
                                    ,CPSS_PP_PORT_EEE_E                         \
                                    ,CPSS_PP_OAM_EXCEPTION_KEEPALIVE_AGING_E    \
                                    ,CPSS_PP_OAM_EXCEPTION_EXCESS_KEEPALIVE_E   \
                                    ,CPSS_PP_OAM_EXCEPTION_INVALID_KEEPALIVE_E  \
                                    ,CPSS_PP_OAM_EXCEPTION_RDI_STATUS_E         \
                                    ,CPSS_PP_OAM_EXCEPTION_TX_PERIOD_E          \
                                    ,CPSS_PP_OAM_EXCEPTION_MEG_LEVEL_E          \
                                    ,CPSS_PP_OAM_EXCEPTION_SOURCE_INTERFACE_E   \
                                    ,CPSS_PP_OAM_ILLEGAL_ENTRY_INDEX_E          \
                                    }

#define APP_DEMO_P1_UNI_EV_DEFAULT { CPSS_PP_RX_BUFFER_QUEUE0_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE0_E                    \
                                    ,CPSS_PP_RX_BUFFER_QUEUE1_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE1_E                    \
                                    }

#define APP_DEMO_P2_UNI_EV_DEFAULT { CPSS_PP_RX_BUFFER_QUEUE2_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE2_E                    \
                                    ,CPSS_PP_RX_BUFFER_QUEUE3_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE3_E                    \
                                    }

#define APP_DEMO_P3_UNI_EV_DEFAULT { CPSS_PP_RX_BUFFER_QUEUE4_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE4_E                    \
                                    }

#define APP_DEMO_P4_UNI_EV_DEFAULT { CPSS_PP_RX_BUFFER_QUEUE5_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE5_E                    \
                                    }

#define APP_DEMO_P5_UNI_EV_DEFAULT { CPSS_PP_RX_BUFFER_QUEUE6_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE6_E                    \
                                    }

#define APP_DEMO_P6_UNI_EV_DEFAULT { CPSS_PP_RX_BUFFER_QUEUE7_E                 \
                                    ,CPSS_PP_RX_ERR_QUEUE7_E                    \
                                    }


#define APP_DEMO_P6_UNI_EV_SINGLE_RX_TASK_MODE { CPSS_PP_RX_BUFFER_QUEUE0_E     \
                                                ,CPSS_PP_RX_BUFFER_QUEUE1_E     \
                                                ,CPSS_PP_RX_BUFFER_QUEUE2_E     \
                                                ,CPSS_PP_RX_BUFFER_QUEUE3_E     \
                                                ,CPSS_PP_RX_BUFFER_QUEUE4_E     \
                                                ,CPSS_PP_RX_BUFFER_QUEUE5_E     \
                                                ,CPSS_PP_RX_BUFFER_QUEUE6_E     \
                                                ,CPSS_PP_RX_BUFFER_QUEUE7_E     \
                                                ,CPSS_PP_RX_ERR_QUEUE0_E        \
                                                ,CPSS_PP_RX_ERR_QUEUE1_E        \
                                                ,CPSS_PP_RX_ERR_QUEUE2_E        \
                                                ,CPSS_PP_RX_ERR_QUEUE3_E        \
                                                ,CPSS_PP_RX_ERR_QUEUE4_E        \
                                                ,CPSS_PP_RX_ERR_QUEUE5_E        \
                                                ,CPSS_PP_RX_ERR_QUEUE6_E        \
                                                ,CPSS_PP_RX_ERR_QUEUE7_E        \
                                               }

#define APP_DEMO_P7_UNI_EV_DEFAULT { CPSS_PP_EB_AUQ_PENDING_E                   \
                                    ,CPSS_PP_EB_AUQ_ALMOST_FULL_E               \
                                    ,CPSS_PP_EB_AUQ_FULL_E                      \
                                    ,CPSS_PP_EB_AUQ_OVER_E                      \
                                    ,CPSS_PP_EB_FUQ_PENDING_E                   \
                                    ,CPSS_PP_EB_FUQ_FULL_E                      \
                                    ,CPSS_PP_EB_SECURITY_BREACH_UPDATE_E        \
                                    ,CPSS_PP_MAC_TBL_OP_DONE_E                  \
                                    ,CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E         \
                                                                                \
/* the event of CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E must be last one on this */ \
/* list because we will remove it on ExMx systems                           */  \
                                    ,CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E        \
                                    }

#define APP_DEMO_P7_UNI_EV_NO_AUQ_PENDING { CPSS_PP_EB_AUQ_OVER_E               \
                                    ,CPSS_PP_EB_FUQ_PENDING_E                   \
                                    ,CPSS_PP_EB_FUQ_FULL_E                      \
                                    ,CPSS_PP_EB_SECURITY_BREACH_UPDATE_E        \
                                    ,CPSS_PP_MAC_TBL_OP_DONE_E                  \
                                    ,CPSS_PP_MAC_MESSAGE_TO_CPU_READY_E         \
                                                                                \
/* the event of CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E must be last one on this */ \
/* list because we will remove it on ExMx systems                           */  \
                                    ,CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E        \
                                    }
/* Currently not used */
#define APP_DEMO_P8_UNI_EV_DEFAULT { CPSS_UNI_RSRVD_EVENT_E }

/****************************************************************************/
/* App-Demo (priority 9 Highest) default events                             */
/****************************************************************************/
#define APP_DEMO_P9_UNI_EV_DEFAULT { CPSS_PP_PORT_LINK_STATUS_CHANGED_E         \
                                    ,CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E           \
                                    ,CPSS_XBAR_HG_PING_RECEIVED_E               \
                                    ,CPSS_XBAR_HOST_RX_FIFO_PEND_E              \
                                    /* for XCAT2 B1, acutally means "Signal Detect change" */   \
                                    ,CPSS_PP_PORT_PRBS_ERROR_QSGMII_E           \
                                    /* for Lion2 & Puma 3, has two meanings: "sigdet */ \
                                    /* change" and "gb lock change" */          \
                                    ,CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E  \
                                    /* for Lion2 RXAUI link up WA */                           \
                                    ,CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E                   \
                                    }

#define APP_DEMO_PROCESS_NUMBER     10
#define APP_DEMO_SINGLE_RX_TASK_NUMBER 6
#define BUFF_LEN    20
#define APP_DEMO_MSGQ_SIZE 500

/* number of packets to get from the queue in one round */
#define APP_DEMO_SINGLE_RX_TASK_P0_WEIGHT 20
#define APP_DEMO_SINGLE_RX_TASK_P1_WEIGHT 20
#define APP_DEMO_SINGLE_RX_TASK_P2_WEIGHT 20
#define APP_DEMO_SINGLE_RX_TASK_P3_WEIGHT 20
#define APP_DEMO_SINGLE_RX_TASK_P4_WEIGHT 20
#define APP_DEMO_SINGLE_RX_TASK_P5_WEIGHT 20
#define APP_DEMO_SINGLE_RX_TASK_P6_WEIGHT 20
#define APP_DEMO_SINGLE_RX_TASK_P7_WEIGHT 20

typedef struct
{
    GT_U8                       devNum;
    GT_U8                       queue;
    GT_U32                      numOfBuff;
    GT_U8*                      packetBuffs[BUFF_LEN];
    GT_U32                      buffLenArr[BUFF_LEN];
#ifdef CHX_FAMILY
    CPSS_DXCH_NET_RX_PARAMS_STC dxChNetRxParams;
#endif
#ifdef EXMXPM_FAMILY
    CPSS_EXMXPM_NET_RX_PARAMS_STC exMxPmNetRxParams;
#endif
} APP_DEMO_RX_PACKET_PARAMS;

extern CPSS_OS_MUTEX rxMutex;

/*
 * typedef: enum APP_DEMO_FATAL_ERROR_TYPE
 *
 * Description: Enumeration for fatal error handling.
 *
 * Enumerations:
 *  APP_DEMO_FATAL_ERROR_NOTIFY_ONLY_TYPE_E  - No execution of osFatalError
 *                                             from event handler.
 *                                             Error notification only.
 *  APP_DEMO_FATAL_ERROR_EXECUTE_TYPE_E      - Execute osFatalError
 *                                             from event handler and
 *                                             error notification.
 *  APP_DEMO_FATAL_ERROR_SILENT_TYPE_E       - No execution of osFatalError,
 *                                             no error notification,
 *                                             increment Galtis event counter only.
 *
 * Comments:
 *         None.
 */
typedef enum
{
    APP_DEMO_FATAL_ERROR_NOTIFY_ONLY_TYPE_E,
    APP_DEMO_FATAL_ERROR_EXECUTE_TYPE_E,
    APP_DEMO_FATAL_ERROR_SILENT_TYPE_E
}APP_DEMO_FATAL_ERROR_TYPE;

/*******************************************************************************
* RX_PACKET_RECEIVE_CB_FUN
*
* DESCRIPTION:
*       Function called to handle incoming Rx packet in the CPU
*
* APPLICABLE DEVICES: ALL Devices
*
* INPUTS:
*       devNum       - Device number.
*       queueIdx     - The queue from which this packet was received.
*       numOfBuffPtr - Num of used buffs in packetBuffs.
*       packetBuffs  - The received packet buffers list.
*       buffLen      - List of buffer lengths for packetBuffs.
*       rxParamsPtr  - (pointer to)information parameters of received packets
*
* RETURNS:
*       GT_OK - no error
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*RX_PACKET_RECEIVE_CB_FUN)
(
    IN GT_U8            devNum,
    IN GT_U8            queueIdx,
    IN GT_U32           numOfBuff,
    IN GT_U8            *packetBuffs[],
    IN GT_U32           buffLen[],
    IN GT_VOID          *rxParamsPtr
);

/*******************************************************************************
* appDemoPrintLinkChangeFlagSet
*
* DESCRIPTION:
*       function to allow set the flag of : printLinkChangeEnabled
*
* INPUTS:
*       enable - enable/disable the printings of 'link change'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoPrintLinkChangeFlagSet
(
    IN GT_U32   enable
);

/*******************************************************************************
* appDemoAllowProcessingOfAuqMessages
*
* DESCRIPTION:
*       function to allow set the flag of : allowProcessingOfAuqMessages
*
* INPUTS:
*       enable - enable/disable the processing of the AUQ messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*    flag that state the tasks may process events that relate to AUQ messages.
*    this flag allow us to stop processing those messages , by that the AUQ may
*    be full, or check AU storm prevention, and other.
*
*******************************************************************************/
GT_STATUS   appDemoAllowProcessingOfAuqMessages
(
    GT_BOOL     enable
);

/*******************************************************************************
* appDemoTraceAuqFlagSet
*
* DESCRIPTION:
*       function to allow set the flag of : traceAuq
*
* INPUTS:
*       enable - enable/disable the printings of indication of AUQ/FUQ messages:
*           "+",    CPSS_NA_E
*           "qa",   CPSS_QA_E should not be
*           "qr",   CPSS_QR_E
*           "-",    CPSS_AA_E
*           "t",    CPSS_TA_E
*           "sa",   CPSS_SA_E should not be
*           "qi",   CPSS_QI_E should not be
*           "f"     CPSS_FU_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoTraceAuqFlagSet
(
    IN GT_U32   enable
);


/*******************************************************************************
* appDemoEventRequestDrvnModeInit
*
* DESCRIPTION:
*       This routine spawns the App Demo event handlers.
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
GT_STATUS appDemoEventRequestDrvnModeInit
(
    IN GT_VOID
);

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_BIND_FUNC
*
* DESCRIPTION:
*       This routine binds a user process to unified event. The routine returns
*       a handle that is used when the application wants to wait for the event
*       (cpssEventSelect), receive the event(cpssEventRecv) or transmit a packet
*       using the Network Interface.
*
*       NOTE : the function does not mask/unmask the HW events in any device.
*              This is Application responsibility to unmask the relevant events
*              on the needed devices , using function cpssEventDeviceMaskSet
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       uniEventArr - The unified event list.
*       arrLength   - The unified event list length.
*
* OUTPUTS:
*       hndlPtr     - (pointer to) The user handle for the bounded events.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on failure
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_FULL - when trying to set the "tx buffer queue unify event"
*                 (CPSS_PP_TX_BUFFER_QUEUE_E) with other events in the same
*                 handler
*       GT_ALREADY_EXIST - one of the unified events already bound to another
*                 handler (not applicable to CPSS_PP_TX_BUFFER_QUEUE_E)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*APP_DEMO_CPSS_EVENT_BIND_FUNC)
(
    IN  CPSS_UNI_EV_CAUSE_ENT uniEventArr[],
    IN  GT_U32               arrLength,
    OUT GT_U32               *hndlPtr
);

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_SELECT_FUNC
*
* DESCRIPTION:
*       This function waiting for one of the events ,relate to the handler,
*       to happen , and gets a list of events (in array of bitmaps format) that
*       occurred .
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl                - The user handle for the bounded events.
*       timeoutPtr          - (pointer to) Wait timeout in milliseconds
*                             NULL pointer means wait forever.
*       evBitmapArrLength   - The bitmap array length (in words).
*
* OUTPUTS:
*       evBitmapArr         - The bitmap array of the received events.
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_TIMEOUT - when the "time out" requested by the caller expired and no
*                    event occurred during this period
*       GT_BAD_PARAM - bad hndl parameter , the hndl parameter is not legal
*                     (was not returned by cpssEventBind(...))
*       GT_BAD_PTR  - evBitmapArr parameter is NULL pointer
*                     (and evBitmapArrLength != 0)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*APP_DEMO_CPSS_EVENT_SELECT_FUNC)
(
    IN  GT_UINTPTR           hndl,
    IN  GT_U32              *timeoutPtr,
    OUT GT_U32              evBitmapArr[],
    IN  GT_U32              evBitmapArrLength
);

/*******************************************************************************
* APP_DEMO_CPSS_EVENT_RECV_FUNC
*
* DESCRIPTION:
*       This function gets general information about the selected unified event.
*       The function retrieve information about the device number that relate to
*       the event , and extra info about port number / priority queue that
*       relate to the event.
*       The function retrieve the info about the first occurrence of this event
*       in the queue.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
*
* INPUTS:
*       hndl        - The user handle for the bounded events.
*       evCause     - The specified unify event that info about it's first
*                     waiting occurrence required.
* OUTPUTS:
*       evExtDataPtr- (pointer to)The additional date (port num / priority
*                     queue number) the event was received upon.
*       evDevPtr    - (pointer to)The device the event was received upon
*
* RETURNS:
*       GT_OK  - success
*       GT_FAIL - general failure
*       GT_NO_MORE   - There is no more info to retrieve about the specified
*                      event .
*       GT_BAD_PARAM - bad hndl parameter ,
*                      or hndl bound to CPSS_PP_TX_BUFFER_QUEUE_E --> not allowed
*                      use dedicated "get tx ended info" function instead
*       GT_BAD_PTR  - one of the parameters is NULL pointer
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*APP_DEMO_CPSS_EVENT_RECV_FUNC)
(
    IN  GT_UINTPTR            hndl,
    IN  CPSS_UNI_EV_CAUSE_ENT evCause,
    OUT GT_UINTPTR          *evExtDataPtr,
    OUT GT_U8               *evDevPtr
);

/*******************************************************************************
* appDemoCpssEventDeviceMaskSet
*
* DESCRIPTION:
*       This routine mask/unmasks an unified event on specific device.
*
* APPLICABLE DEVICES:  All devices (PP / FA / XBAR)
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
*       operation  - the operation : mask / unmask
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK  - on success
*       GT_FAIL - on failure.
*       GT_BAD_PARAM - bad unify event value or bad device number
*
* COMMENTS:
*
*       The OS interrupts and the ExtDrv interrupts are locked during the
*       operation of the function.
*
*******************************************************************************/
typedef GT_STATUS (*APP_DEMO_CPSS_EVENT_DEVICE_MASK_SET_FUNC)
(
    IN GT_U8                    devNum,
    IN CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN CPSS_EVENT_MASK_SET_ENT  operation
);

/*******************************************************************************
* appDemoEventFatalErrorEnable
*
* DESCRIPTION:
*       Set fatal error handling type.
*
* INPUTS:
*       fatalErrorType  - fatal error handling type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - on wrong fatalErrorType
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoEventFatalErrorEnable
(
    APP_DEMO_FATAL_ERROR_TYPE fatalErrorType
);

/*******************************************************************************
* appDemoAuMessageNumberDump
*
* DESCRIPTION:
*       Get number of Au messages.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoAuMessageNumberDump
(
    GT_VOID
);

/*******************************************************************************
* appDemoEventRequestDrvnModeReset
*
* DESCRIPTION:
*       This routine destroys event handlers tasks, delete message queues, clean
*       bindings for App Demo event handlers.
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
GT_STATUS appDemoEventRequestDrvnModeReset
(
    IN GT_VOID
);

GT_STATUS appDemoGenEventCounterIncrement
(
    IN  GT_U8   devNum,
    IN GT_U32   uniEvCounter,
    IN GT_U32   evExtData
);

GT_STATUS appDemoEventsDataBaseGet
(
    OUT GT_U32 ***eventCounterBlockGet
);

/*******************************************************************************
* appDemoGenEventCounterGet
*
* DESCRIPTION:
*     get the number of times that specific event happened.
*
* INPUTS:
*     devNum      - device number
*     uniEvent    - unified event
*     clearOnRead - do we 'clear' the counter after 'read' it
*               GT_TRUE - set counter to 0 after get it's value
*               GT_FALSE - don't update the counter (only read it)
* OUTPUTS:
*     counterPtr  - (pointer to)the counter (the number of times that specific event happened)
*
* RETURNS:
*     GT_OK              - on success.
*     GT_BAD_PARAM       - on bad devNum or uniEvent.
*     GT_BAD_PTR         - on NULL pointer.
*     GT_NOT_INITIALIZED - the counters DB not initialized for the device.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS appDemoGenEventCounterGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_UNI_EV_CAUSE_ENT    uniEvent,
    IN  GT_BOOL                  clearOnRead,
    OUT GT_U32                  *counterPtr
);

/*******************************************************************************
* appDemoEventsToTestsHandlerBind
*
* DESCRIPTION:
*       This routine creates new event handler for the unified event list.
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
GT_STATUS appDemoEventsToTestsHandlerBind
(
    IN  CPSS_UNI_EV_CAUSE_ENT       cpssUniEventArr[],
    IN  GT_U32                      arrLength,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __userEventHandlerh */

