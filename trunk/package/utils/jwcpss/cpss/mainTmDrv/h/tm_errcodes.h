/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief Traffic Manager error codes.
 *
 * @file tm_errcodes.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_ERRCODES_H
#define   	TM_ERRCODES_H

/** HW error codes */
enum tm_hw_err_codes
{
    TM_MIN_ERROR_CODE = 0,               /* First @ 1000 + 1. */
    TM_HW_GEN_CONFIG_FAILED,                /**< 1 */
    TM_HW_WRED_CURVE_FAILED,                /**< 2 */
    TM_HW_DROP_PROFILE_FAILED,              /**< 3 */
    TM_HW_CONF_PER_SCHEME_FAILED,           /**< 4 */
    TM_HW_TREE_CONFIG_FAIL,                 /**< 5 */
    TM_HW_AGING_CONFIG_FAIL,                /**< 6 */
    TM_HW_PORT_CONFIG_FAIL,                 /**< 7 */
    TM_HW_C_NODE_CONFIG_FAIL,               /**< 8 */
    TM_HW_B_NODE_CONFIG_FAIL,               /**< 9 */
    TM_HW_A_NODE_CONFIG_FAIL,               /**< 10 */
    TM_HW_QUEUE_CONFIG_FAIL,				/**< 11 */
    TM_HW_PERIODIC_UPDATE_STATUS_SET_FAILED,/**< 12 */
    TM_HW_PORT_DWRR_BYTES_PER_BURST_FAILED, /**< 13 */
    TM_HW_QMR_GET_ERRORS_FAILED,            /**< 14 */
    TM_HW_BAP_GET_ERRORS_FAILED,            /**< 15 */
    TM_HW_RCB_GET_ERRORS_FAILED,            /**< 16 */
    TM_HW_SCHED_GET_ERRORS_FAILED,          /**< 17 */
    TM_HW_DROP_GET_ERRORS_FAILED,           /**< 18 */
    TM_HW_SHAPING_PROF_FAILED,              /**< 19 */
    TM_HW_READ_PORT_STATUS_FAIL,            /**< 20 */
    TM_HW_READ_C_NODE_STATUS_FAIL,          /**< 21 */
    TM_HW_READ_B_NODE_STATUS_FAIL,          /**< 22 */
    TM_HW_READ_A_NODE_STATUS_FAIL,          /**< 23 */
    TM_HW_READ_QUEUE_STATUS_FAIL,           /**< 24 */
    TM_HW_GET_QUEUE_LENGTH_FAIL,            /**< 25 */
    TM_HW_GET_QMR_PKT_STAT_FAILED,          /**< 26 */
    TM_HW_GET_RCB_PKT_STAT_FAILED,          /**< 27 */
	TM_HW_SET_SMS_PORT_ATTR_FAILED,         /**< 28 */
    TM_HW_ELIG_PRIO_FUNC_FAILED,            /**< 29 */
    TM_HW_AQM_CONFIG_FAIL,                  /**< 30 */
    TM_HW_COLOR_NUM_CONFIG_FAIL,            /**< 31 */
    TM_HW_DP_QUERY_RESP_CONF_FAILED,        /**< 32 */
    TM_HW_QUEUE_COS_CONF_FAILED,            /**< 33 */
    TM_HW_TM2TM_GLOB_CONF_FAILED,           /**< 34 */
    TM_HW_TM2TM_CHANNEL_CONF_FAILED,        /**< 35 */
    TM_HW_TM2TM_LC_CONF_FAILED,             /**< 36 */
    TM_HW_TM2TM_ENABLE_FAILED,              /**< 37 */

    TM_HW_MAX_ERROR 						/**< 38 */
};

/** SW (configuration) error codes */
enum tm_conf_err_codes
{
    TM_CONF_INVALID_PROD_NAME = TM_HW_MAX_ERROR + 1, /**< 39 */ 
    TM_CONF_PER_RATE_L_K_N_NOT_FOUND,      /**< 40 */
    TM_CONF_PORT_IND_OOR,                  /**< 41 */
    TM_CONF_PORT_QUANTUM_OOR,              /**< 42 */
    TM_CONF_PORT_DWRR_PRIO_OOR,            /**< 43 */
    TM_CONF_P_WRED_PROF_REF_OOR,           /**< 44 */
    TM_CONF_PORT_BS_OOR,                   /**< 45 */
    TM_CONF_Q_SHAPING_PROF_REF_OOR,        /**< 46 */
    TM_CONF_Q_QUANTUM_OOR,                 /**< 47 */
    TM_CONF_Q_WRED_PROF_REF_OOR,           /**< 48 */
    TM_CONF_A_NODE_IND_OOR,                /**< 49 */
    TM_CONF_A_SHAPING_PROF_REF_OOR,        /**< 50 */
    TM_CONF_A_QUANTUM_OOR,                 /**< 51 */
    TM_CONF_A_DWRR_PRIO_OOR,               /**< 52 */
    TM_CONF_A_WRED_PROF_REF_OOR,           /**< 53 */
    TM_CONF_B_NODE_IND_OOR,                /**< 54 */
    TM_CONF_B_SHAPING_PROF_REF_OOR,        /**< 55 */
    TM_CONF_B_QUANTUM_OOR,                 /**< 56 */
    TM_CONF_B_DWRR_PRIO_OOR,               /**< 57 */
    TM_CONF_B_WRED_PROF_REF_OOR,           /**< 58 */
    TM_CONF_C_NODE_IND_OOR,                /**< 59 */
    TM_CONF_C_SHAPING_PROF_REF_OOR,        /**< 60 */
    TM_CONF_C_QUANTUM_OOR,                 /**< 61 */
    TM_CONF_C_DWRR_PRIO_OOR,               /**< 62 */
    TM_CONF_C_WRED_PROF_REF_OOR,           /**< 63 */
    TM_CONF_C_WRED_COS_OOR,                /**< 64 */
    TM_CONF_ELIG_PRIO_FUNC_ID_OOR,         /**< 65 */
    TM_CONF_DP_COS_SEL_OOR,                /**< 66 */
    TM_CONF_EXT_HDR_SIZE_OOR,              /**< 67 */
    TM_CONF_CTRL_PKT_STR_OOR,              /**< 68 */
    TM_CONF_TM2TM_EGR_ELEMS_OOR,           /**< 69 */
    TM_CONF_TM2TM_SRC_LVL_OOR,             /**< 70 */
    TM_CONF_TM2TM_BP_LVL_OOR,              /**< 71 */
    TM_CONF_TM2TM_BP_THRESH_OOR,           /**< 72 */
    TM_CONF_TM2TM_DP_LVL_OOR,              /**< 73 */
    TM_CONF_TM2TM_CTRL_HDR_OOR,            /**< 74 */
    TM_CONF_TM2TM_PORT_FOR_CTRL_PKT_OOR,   /**< 75 */
    TM_CONF_PORT_SPEED_OOR,                /**< 76 */
    TM_CONF_PORT_BW_OUT_OF_SPEED,          /**< 77 */
    TM_CONF_INVALID_NUM_OF_C_NODES,        /**< 78 */
    TM_CONF_INVALID_NUM_OF_B_NODES,        /**< 79 */
    TM_CONF_INVALID_NUM_OF_A_NODES,        /**< 80 */
    TM_CONF_INVALID_NUM_OF_QUEUES,         /**< 81 */
    TM_CONF_TM2TM_AQM_INVALID_COLOR_NUM,   /**< 82 */
    TM_CONF_REORDER_NODES_NOT_ADJECENT,    /**< 83 */
    TM_CONF_BURST_TOO_SMALL_FOR_GIVEN_RATE,/**< 84 */
    TM_CONF_REORDER_CHILDREN_NOT_AVAIL,    /**< 85 */
    TM_CONF_PORT_IND_NOT_EXIST,            /**< 86 */
    TM_CONF_A_NODE_IND_NOT_EXIST,          /**< 87 */
    TM_CONF_B_NODE_IND_NOT_EXIST,          /**< 88 */
    TM_CONF_C_NODE_IND_NOT_EXIST,          /**< 89 */
    TM_CONF_CANNT_GET_LAD_FREQUENCY,       /**< 90 */
    TM_CONF_UPD_RATE_NOT_CONF_FOR_LEVEL,   /**< 91 */
    TM_CONF_TM2TM_CHANNEL_NOT_CONFIGURED,  /**< 92 */
    TM_CONF_PORT_IND_USED,                 /**< 93 */
	TM_BW_OUT_OF_RANGE,					   /**< 94 */
	TM_BW_UNDERFLOW,                       /**< 95 */
 	TM_CONF_SHAPING_PROF_REF_OOR,          /**< 96 */ 
	TM_WRONG_SHP_PROFILE_LEVEL,			   /**< 97 */ 	
    TM_CONF_NULL_LOGICAL_NAME,             /**< 98 */
    TM_CONF_WRONG_LOGICAL_NAME,            /**< 99 */
    TM_CONF_NEED_REALLOC,                  /**< 100 */
    TM_CONF_MAX_ERROR                      /**< max */
};


#endif   /* TM_ERRCODES_H */

