#ifndef _SET_HW_REGISTERS_H_
#define _SET_HW_REGISTERS_H_


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
 * @brief internal API for read/write  TM H/W 
 *
 * @file set_hw_registers.h
 *
 * $Revision: 2.0 $
 */

#include "tm_core_types.h"

int set_hw_shaping_status(tm_handle hndl, enum tm_level level);

int set_hw_dwrr_limit(tm_handle hndl);

int set_hw_gen_conf(tm_handle hndl);

int set_hw_max_dp_mode(tm_handle hndl);

int set_hw_q_red_curves(tm_handle hndl,
                        uint8_t curve_ind,
                        const tm_wred_vec curve_green,
                        const tm_wred_vec curve_yellow,
                        const tm_wred_vec curve_red);
int set_hw_queues_wred_curve(tm_handle hndl, uint8_t curve_ind);
int set_hw_queues_default_wred_curve(tm_handle hndl,uint8_t *prob_array);

int set_hw_a_red_curves(tm_handle hndl,
                        uint8_t curve_ind,
                        const tm_wred_vec curve_green,
                        const tm_wred_vec curve_yellow,
                        const tm_wred_vec curve_red);
int set_hw_a_nodes_wred_curve(tm_handle hndl, uint8_t curve_ind);
int set_hw_a_nodes_default_wred_curve(tm_handle hndl,uint8_t *prob_array);

int __set_hw_b_nodes_wred_curve(tm_handle hndl, const uint8_t *prob_array,
                                uint8_t curve_ind);
int set_hw_b_nodes_wred_curve(tm_handle hndl, uint8_t curve_ind);
int set_hw_b_nodes_default_wred_curve(tm_handle hndl,uint8_t *prob_array);

int __set_hw_c_nodes_wred_curve(tm_handle hndl, const uint8_t *prob_array,
                                uint8_t cos, uint8_t curve_ind);
int set_hw_c_nodes_wred_curve(tm_handle hndl, uint8_t cos, uint8_t curve_ind);
int set_hw_c_nodes_default_wred_curve(tm_handle hndl, uint8_t cos, uint8_t *prob_array);

int __set_hw_ports_wred_curve(tm_handle hndl, const uint8_t *prob_array);
int set_hw_ports_wred_curve(tm_handle hndl, uint8_t curve_ind);
int set_hw_ports_default_wred_curve(tm_handle hndl, uint8_t *prob_array);

int __set_hw_ports_wred_curve_cos(tm_handle hndl, const uint8_t *prob_array,
                                  uint8_t cos);
int set_hw_ports_wred_curve_cos(tm_handle hndl, uint8_t cos, uint8_t curve_ind);
int set_hw_ports_default_wred_curve_cos(tm_handle hndl, uint8_t cos, uint8_t *prob_array);

int set_hw_queue_drop_profile(tm_handle hndl, uint32_t prof_ind);
int set_hw_queue_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile);

int set_hw_a_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind);
int set_hw_a_nodes_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile);

int set_hw_b_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind);
int set_hw_b_nodes_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile);

int set_hw_c_nodes_drop_profile(tm_handle hndl, uint8_t cos, uint32_t prof_ind);
int set_hw_c_nodes_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint8_t cos);

int set_hw_ports_drop_profile(tm_handle hndl, uint32_t prof_ind, uint8_t port_ind);
int set_hw_ports_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint8_t port_ind);

int set_hw_ports_drop_profile_cos(tm_handle hndl, uint8_t cos, uint32_t prof_ind, uint8_t port_ind);
int set_hw_ports_default_drop_profile_cos(tm_handle hndl,  struct tm_drop_profile *profile, uint8_t cos,uint8_t port_ind);

int set_hw_aging_status(tm_handle hndl);
int get_hw_aging_status(tm_handle hndl);

int set_hw_node_shaping(tm_handle hndl, enum tm_level level, uint32_t node_ind);
int set_hw_node_shaping_ex(tm_handle hndl, enum tm_level level, uint32_t node_ind, struct tm_shaping_profile *profile);

int set_hw_tm2tm_channels(tm_handle hndl);

int set_hw_drop_aqm_mode(tm_handle hndl);

int set_hw_drop_color_num(tm_handle hndl);

int set_hw_tm2tm_aqm_mode(tm_handle hndl);

int set_hw_periodic_scheme(tm_handle hndl);

int set_hw_node_mapping(tm_handle hndl, enum tm_level lvl, uint32_t index);

int set_hw_node_elig_prio_function(tm_handle hndl, enum tm_level node_level, uint32_t node_index);

int set_hw_node_disable(tm_handle hndl, enum tm_level node_level, uint32_t node_index);

int set_hw_queue_drop_profile_ptr(tm_handle hndl, uint32_t node_ind);
int set_hw_queue_params(tm_handle hndl, uint32_t queue_ind);

int set_hw_a_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind);
int set_hw_a_node_params(tm_handle hndl, uint32_t node_ind);

int set_hw_b_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind);
int set_hw_b_node_params(tm_handle hndl, uint32_t node_ind);

int set_hw_c_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind , uint8_t cos);
int set_hw_c_node_params(tm_handle hndl, uint32_t node_ind);

int set_hw_port_elig_prio_func_ptr(tm_handle hndl, uint8_t port_ind);

int set_hw_port_shaping(tm_handle hndl, uint8_t port_ind);

int set_hw_port_scheduling(tm_handle hndl, uint8_t port_ind);

int set_hw_port_drop_global(tm_handle hndl, uint8_t port_ind);

int set_hw_port_drop_cos(tm_handle hndl, uint8_t port_ind);


int set_hw_port(tm_handle hndl, uint8_t port_ind);

int set_hw_tree_deq_status(tm_handle hndl);
int get_hw_tree_deq_status(tm_handle hndl);

int set_hw_tree_dwrr_priority(tm_handle hndl);
int get_hw_tree_dwrr_priority_status(tm_handle hndl);

int set_hw_install_queue(tm_handle hndl, uint32_t index);

int set_hw_uninstall_queue(tm_handle hndl, uint32_t index);

int set_hw_uninstall_default_queues(tm_handle hndl);

int set_hw_disable_ports(tm_handle hndl, uint32_t total_ports);

int set_hw_elig_prio_func_tbl_q_level(tm_handle hndl);

int set_hw_elig_prio_func_tbl_a_level(tm_handle hndl);

int set_hw_elig_prio_func_tbl_b_level(tm_handle hndl);

int set_hw_elig_prio_func_tbl_c_level(tm_handle hndl);

int set_hw_elig_prio_func_tbl_p_level(tm_handle hndl);

int set_hw_q_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset);

int set_hw_a_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset);

int set_hw_b_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset);

int set_hw_c_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset);

int set_hw_p_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset);

int set_hw_port_deficit_clear(tm_handle hndl, uint8_t index);

int set_hw_c_node_deficit_clear(tm_handle hndl, uint32_t index);

int set_hw_b_node_deficit_clear(tm_handle hndl, uint32_t index);

int set_hw_a_node_deficit_clear(tm_handle hndl, uint32_t index);

int set_hw_queue_deficit_clear(tm_handle hndl, uint32_t index);

int get_hw_flush_queue_status(tm_handle hndl, uint32_t index, uint8_t * status);

int set_hw_flush_queue(tm_handle hndl, uint32_t index, uint8_t block_en);

int get_hw_flush_port_status(tm_handle hndl, uint32_t index, uint8_t * status);

int set_hw_flush_port(tm_handle hndl, uint32_t index, uint8_t block_en);

int set_hw_tm2tm_glob_config(tm_handle hndl,
					   uint8_t cos_sel,
					   struct tm_ext_hdr *ext_hdr,
					   struct tm_ctrl_pkt_str *control_pkt,
					   struct tm_delta_range *range);

int set_hw_dp_remote_resp(tm_handle hndl, enum tm2tm_channel remote_lvl);

int set_hw_dp_local_resp(tm_handle hndl,uint8_t port_dp, enum tm_level local_lvl);
int get_hw_dp_local_resp(tm_handle hndl, uint8_t * port_dp, enum tm_level  *local_lvl);

int set_hw_tm2tm_lc_config(tm_handle hndl,
							 uint16_t quantum,
							 void *qmap_hdr_ptr, /* pointer to 8 bytes of qmap_header content */
							 uint8_t ctrl_hdr,
							 uint8_t tm_port);



int set_hw_tm2tm_enable(tm_handle hndl);

int set_hw_port_sms_attr_pbase(tm_handle hndl, uint8_t index);

int set_hw_port_sms_attr_qmap_pars(tm_handle hndl, uint8_t index);

int set_hw_dp_source(tm_handle hndl);
int get_hw_dp_source(tm_handle hndl);

int set_hw_queue_cos(tm_handle hndl, uint32_t index, uint8_t cos);
int get_hw_queue_cos(tm_handle hndl, uint32_t index, uint8_t * cos);


/*********************************************************************************/

int get_hw_gen_params(tm_handle hndl);


int get_hw_port_status(tm_handle hndl,
				     uint8_t index,
				     struct tm_port_status *tm_status);

int get_hw_c_node_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status);

int get_hw_b_node_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status);

int get_hw_a_node_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status);

int get_hw_queue_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status);

int get_hw_queue_length(tm_handle hndl,
				      enum tm_level level,
				      uint32_t index,
				      uint32_t * inst_q_length,
				      uint32_t * av_q_length);

int get_hw_qmr_errors(tm_handle hndl, struct tm_error_info *info);

int get_hw_bap_errors(tm_handle hndl, struct tm_error_info *info);

int get_hw_rcb_errors(tm_handle hndl, struct tm_error_info *info);

int get_hw_sched_errors(tm_handle hndl, struct tm_error_info *info);

int get_hw_drop_errors(tm_handle hndl, struct tm_error_info *info);

int get_hw_tm_units_status(tm_handle hndl, struct tm_units_error_status_info *units_errors);

int get_hw_tm_bap_unit_status(tm_handle hndl, int bap_num, struct tm_bap_unit_error_status_info *bap_unit_errors);

/*
 in order to process 64 bit counters on OS without 64 bit support
 following functions assumes that void * counter_address is address of 8 bytes of memory
 where 64 bits of counter will be copied.
*/
int get_hw_qmr_num_pkts_to_unins_queue(tm_handle hndl , void * counter_address);

int get_hw_rcb_get_num_pkts_to_sms(tm_handle hndl,  void * counter_address);

int get_hw_rcb_get_num_num_crc_err_pkts_to_sms(tm_handle hndl,   void * counter_address);

int get_hw_rcb_get_num_errs_from_sms_to_dram(tm_handle hndl,  void * counter_address);

int get_hw_rcb_get_num_port_flush_drp_pkts(tm_handle hndl,  void * counter_address);


/* common macros for  set_hw_registers.c & set_hw_registers_platform.c */
#define NODE_VALIDATION(nodes_count)	\
	if (node_ind >= nodes_count) rc = -EFAULT; 



int tm_get_hw_elig_prio_func(tm_handle hndl, enum tm_level level, uint16_t func_offset, struct tm_elig_prio_func_node *params);
int tm_get_hw_elig_prio_q_func(tm_handle hndl, uint16_t func_offset, struct tm_elig_prio_func_queue *params);

int tm_get_hw_queue_drop_profile_ptr(tm_handle hndl, uint32_t queue_ind, uint16_t * queue_profile_ptr);
int tm_get_hw_a_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind, uint16_t * profile_ptr);
int tm_get_hw_b_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind, uint16_t * profile_ptr);
int tm_get_hw_c_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind , uint8_t cos, uint16_t * profile_ptr);

int tm_get_hw_queue_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile);
int tm_get_hw_a_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile);
int tm_get_hw_b_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile);
int tm_get_hw_c_nodes_drop_profile(tm_handle hndl, uint8_t cos, uint32_t prof_ind, struct tm_drop_profile *profile);
int tm_get_hw_ports_drop_profile_cos(tm_handle hndl, uint8_t cos, uint32_t prof_ind, struct tm_drop_profile *profile);
int tm_get_hw_ports_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile);
int tm_get_hw_wred_curve(tm_handle hndl, enum tm_level level, uint8_t cos, uint8_t curve_ind, uint8_t *prob_array);


#endif /* _SET_HW_REGISTERS_H_ */
