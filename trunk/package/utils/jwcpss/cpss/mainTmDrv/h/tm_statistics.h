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
 * @brief TM statistics gathering interface.
 *
 * @file tm_statistics.h
 *
 * $Revision: 2.0 $
 */

#ifndef TM_STATISTICS_H
#define TM_STATISTICS_H

#include "tm_defs.h"


/*
in order to support OS without  64 bit variables  statistics counters are extracted  to 8bytes string
in OS wih 64 bit variables support it can be casted to uint64_t  , for other systems it can be treated as 
Lo & Hi parts - unsigned integers 
*/
typedef uint8_t	U64[8];


/** Read QMR Packet Statistics.
 *
 *   @param[in]     hndl					- TM lib handle.
 *   @param[out]    num_pkts_to_unins_queue	- pointer to 8 Bytes of memory where "packets from SMS that have arrived to not installed Queue" value will be copyed
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if pkt_counters is NULL.
 *   @retval TM_HW_GET_QMR_PKT_STAT_FAILED if read from HW fails.
 */
int tm_qmr_get_num_pkts_to_unins_queue(tm_handle hndl, U64 * num_pkts_to_unins_queue);


/** Read RCB Packet Statistics. */

/** Read count of Non-error packets that are passed to SMS.
 *
 *   @param[in]     hndl            - TM lib handle.
 *   @param[out]    num_pkts_to_sms - pointer to 8 Bytes of memory where "Non-error packets that are passed to SMS"  value will be copyed
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if pkt_counters is NULL.
 *   @retval TM_HW_GET_RCB_PKT_STAT_FAILED if read from HW fails.
 */
int tm_rcb_get_num_pkts_to_sms(tm_handle hndl,  U64 * num_pkts_to_sms);


/** Read count of packets with CRC error.
 *
 *   @param[in]     hndl					- TM lib handle.
 *   @param[out]    num_crc_err_pkts_to_sms	- pointer to 8 Bytes of memory where "packets with CRC error"  value will be copyed
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if pkt_counters is NULL.
 *   @retval TM_HW_GET_RCB_PKT_STAT_FAILED if read from HW fails.
 */
int tm_rcb_get_num_crc_err_pkts_to_sms(tm_handle hndl,  U64 * num_crc_err_pkts_to_sms);



/** Read count of packets with error from SMS that has been written to DRAM.
 *
 *   @param[in]     hndl						- TM lib handle.
 *   @param[out]    num_errs_from_sms_to_dram	- pointer to 8 Bytes of memory where "packets with error from SMS that has been written to DRAM"  value will be copyed
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if pkt_counters is NULL.
 *   @retval TM_HW_GET_RCB_PKT_STAT_FAILED if read from HW fails.
 */
int tm_rcb_get_num_errs_from_sms_to_dram(tm_handle hndl,  U64 * num_errs_from_sms_to_dram);


/** Read count of dropped port flush packets.
 *
 *   @param[in]     hndl					- TM lib handle.
 *   @param[out]    num_port_flush_drp_pkts	- pointer to 8 Byte
 *         of memory where "dropped port flush packets" value
 *         will be copyed
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EFAULT if pkt_counters is NULL.
 *   @retval TM_HW_GET_RCB_PKT_STAT_FAILED if read from HW fails.
 */
int tm_rcb_get_num_port_flush_drp_pkts(tm_handle hndl,  U64 * num_port_flush_drp_pkts);

#endif   /* TM_STATISTICS_H */

