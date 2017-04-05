/*
 * (c), Copyright 2009-2013, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief TM2TM interface.
 *
 * @file tm2tm.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM2TM_H
#define   	TM2TM_H

#include "tm_defs.h"

/****************/
/* System Setup */
/****************/

/** TM2TM Global Configuration.
 *
 *   @param[in]     hndl                TM lib handle. 
 *   @param[in]     cos_sel             C level CoS DP selector.       
 *   @param[in]     ext_hdr             External Headers structure pointer.
 *   @param[in]     control_pkt         Control Packet structure pointer.
 *   @param[in]     range               Delta Range Mapping to Priority structure pointer.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval TM_CONF_DP_COS_SEL_OOR if cos_sel is out of range.
 *   @retval TM_CONF_EXT_HDR_SIZE_OOR if ext_hdr field is not valid.
 *   @retval TM_CONF_CTRL_PKT_STR_OOR if control_pkt field is out of range.
 *   @retval TM_CONF_DELTA_RANGE_OOR if range field is out of range.
 *
 *   @retval TM_HW_TM2TM_GLOB_CONF_FAILED if download to HW fails.
 */
int tm2tm_global_config(tm_handle hndl,
                        uint8_t cos_sel,
                        struct tm_ext_hdr *ext_hdr,
                        struct tm_ctrl_pkt_str *control_pkt,
                        struct tm_delta_range *range);


/** TM2TM Port Channel Configuration.
 *
 *   @param[in]     hndl                TM lib handle. 
 *   @param[in]     egr_elems           Number of Elements in Egress TMs.       
 *   @param[in]     src_lvl             TM2TM Source Level (Port/C/B/A).
 *   @param[in]     mode                TM2TM Channel Mode (WRED/BP).
 *   @param[in]     bp_lvl              BP Destination Level (C/Q).
 *   @param[in]     bp_offset           BP Base Offset.
 *   @param[in]     bp_xon              BP Xon Thresholds.
 *   @param[in]     bp_xoff             BP Xoff Thresholds.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval TM_CONF_TM2TM_EGR_ELEMS_OOR if egr_elems is out of range.
 *   @retval TM_CONF_TM2TM_SRC_LVL_OOR if src_lvl is out of range.
 *   @retval TM_CONF_TM2TM_BP_THRESH_OOR if bp_xon/bp_off is not valid.
 *
 *   @retval TM_HW_TM2TM_CHANNEL_CONF_FAILED if download to HW fails.
 */
int tm2tm_port_channel_config(tm_handle hndl,
                              uint8_t egr_elems,
                              enum tm_level src_lvl,
                              enum tm2tm_mode mode,
                              enum tm_level bp_lvl,
                              uint16_t bp_offset,
                              uint8_t bp_xon,
                              uint8_t bp_xoff);


/** TM2TM Node Channel Configuration.
 *
 *   @param[in]     hndl                TM lib handle. 
 *   @param[in]     egr_elems           Number of Elements in Egress TMs.       
 *   @param[in]     src_lvl             TM2TM Source Level (C/B/A/Queue).
 *   @param[in]     mode                TM2TM Channel Mode (WRED/BP).
 *   @param[in]     bp_lvl              BP Destination Level (C/Q).
 *   @param[in]     bp_offset           BP Base Offset.
 *   @param[in]     bp_xon              BP Xon Thresholds.
 *   @param[in]     bp_xoff             BP Xoff Thresholds.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval TM_CONF_TM2TM_EGR_ELEMS_OOR if egr_elems is out of range.
 *   @retval TM_CONF_TM2TM_SRC_LVL_OOR if src_lvl is out of range.
 *   @retval TM_CONF_TM2TM_BP_THRESH_OOR if bp_xon/bp_off is not valid.
 *
 *   @retval TM_HW_TM2TM_CHANNEL_CONF_FAILED if download to HW fails.
 */
int tm2tm_node_channel_config(tm_handle hndl,
                              uint16_t egr_elems,
                              enum tm_level src_lvl,
                              enum tm2tm_mode mode,
                              enum tm_level bp_lvl,
                              uint16_t bp_offset,
                              uint8_t bp_xon,
                              uint8_t bp_xoff);


/** Drop Query Response Select.
 *
 *   @param[in]     hndl                TM lib handle. 
 *   @param[in]     local_or_remote     0 - local only, 1 - remote only, 2 - both local & remote.
 *   @param[in]     port_dp             0 - Global, 1 - CoS.
 *   @param[in]     local_lvl           Local response level (Q/A/B/C/Port).
 *   @param[in]     remote_lvl          Remote response level (Node/Port).
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval TM_CONF_TM2TM_DP_LVL_OOR if lvl out of range.
 *
 *   @retval TM_HW_DP_QUERY_RESP_CONF_FAILED if download to HW
 *           fails.
 */
int tm_set_dp_query_resp_lvl(tm_handle hndl,
                             uint8_t local_or_remote,
                             uint8_t port_dp,
                             enum tm_level local_lvl,
                             enum tm2tm_channel remote_lvl);


/************/
/* LC Setup */
/************/

/** TM2TM LC Configuration.
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     quantum         Generation Data Quantum.       
 *   @param[in]     qmap_hdr_ptr    pointer to 8 bytes of  Qmap Header content.
 *   @param[in]     ctrl_hdr        Control Header (Line Card ID).
 *   @param[in]     tm_port         TM Port for Control Packets.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval TM_CONF_CTRL_HDR_OOR if ctrl_hdr out of range.
 *   @retval TM_CONF_TM_PORT_FOR_CTRL_PKT_OOR if tm_port out of range.
 *
 *   @retval TM_HW_TM2TM_LC_CONF_FAILED if download to HW fails.
 */
int tm2tm_lc_config(tm_handle hndl,
                    uint16_t quantum,
                    void * qmap_hdr_ptr,
                    uint8_t ctrl_hdr,
                    uint8_t tm_port);


/** TM2TM Egress Drop AQM Mode Configuration.
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     lvl             Level (Q/A/B/C/Port).       
 *   @param[in]     num             Number of colors for this level.
 *   @param[in]     src             Array of dp sources for each color.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval -EPERM if lvl is invalid.
 *   @retval -EFAULT if num is invalid.
 *   @retval -EADDRNOTAVAIL if src is invalid.
 *
 *   @retval TM_HW_AQM_CONFIG_FAIL if download to HW fails.
 */
int tm2tm_set_egress_drop_aqm_mode(tm_handle hndl,
                                   enum tm_level lvl,
                                   enum tm_color_num num,
                                   enum tm_dp_source *src);


/** TM2TM Ingress Drop AQM Mode Configuration.
 *
 *   @param[in]     hndl            TM lib handle. 
 *   @param[in]     ch              Channel (Node/Port).       
 *   @param[in]     num             Number of colors for this channel.
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *   @retval TM_CONF_TM2TM_CHANNEL_NOT_CONFIGURED if channel is not configured at system setup.
 *   @retval TM_CONF_TM2TM_AQM_INVALID_COLOR_NUM if color number is incorrect.
 *
 *   @retval TM_HW_AQM_CONFIG_FAIL if download to HW fails.
 */
int tm2tm_set_ingress_drop_aqm_mode(tm_handle hndl,
                                    enum tm2tm_channel ch,
                                    enum tm_color_num num);


/** TM2TM Enable.
 *
 *   @param[in]     hndl            TM lib handle. 
 *
 *   @return an integer return code.
 *   @retval zero on success.
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is invalid.
 *  
 *   @retvel TM_HW_AQM_CONFIG_FAIL if download to HW fails.
 *   @retval TM_HW_TM2TM_ENABLE_FAILED if download to HW fails.
 */
int tm2tm_enable(tm_handle hndl);


#endif   /* TM2TM_H */
