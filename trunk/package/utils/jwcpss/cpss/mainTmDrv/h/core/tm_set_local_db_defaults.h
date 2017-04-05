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
 * @brief interface to functions for reset TM S/W DB values
 *
 * @file set_local_db_defaults.h
 *
 * $Revision: 2.0 $
 */

#ifndef _SET_LOCAL_DB_DEFAULTS_H_
#define _SET_LOCAL_DB_DEFAULTS_H_


#include "tm_core_types.h"
#include "rm_internal_types.h"


int set_sw_sched_conf_default(void * hndl);
int set_sw_gen_conf_default(void * hndl);

int	set_sw_shaping_profile_default(struct tm_shaping_profile *profiles,
                                    uint32_t prof_index);
int	set_sw_drop_profile_default(struct tm_drop_profile *profile,
                            uint32_t prof_index);

int set_sw_wred_curve_default(struct tm_wred_curve *curve,
                                            uint16_t curve_index);
                                            
void set_sw_queue_default(struct tm_queue * queue_node, int bIncludeMapping);
void set_sw_a_node_default(struct tm_a_node * node, int bIncludeMapping);
void set_sw_b_node_default(struct tm_b_node * node, int bIncludeMapping);
void set_sw_c_node_default(struct tm_c_node * node, int bIncludeMapping);
void set_sw_port_default(struct tm_port * port_node, int bIncludeMapping);

/* initializing default values for eligible functions tables*/
void set_default_node_elig_prio_func_table(struct tm_elig_prio_func_node *func_table);
void set_default_queue_elig_prio_func_table(struct tm_elig_prio_func_queue  *func_table);
                                                  
int is_queue_elig_fun_uses_shaper(struct tm_elig_prio_func_queue * queue_func_table,uint8_t func_index);

int is_node_elig_fun_uses_shaper(struct tm_elig_prio_func_node * node_func_table, uint8_t func_index);
                                                  
/* Auxiliary function */
uint16_t convert_elig_func_to_value(struct tm_elig_prio_func_out *elig_func);
void convert_value_to_elig_func(uint16_t elig_val, struct tm_elig_prio_func_out *elig_func);

void set_sw_q_elig_prio_func(struct tm_elig_prio_func_queue *func_table, 
                                   uint16_t func_offset,
                                   union tm_elig_prio_func *queue_func_value_arr);

void set_sw_n_elig_prio_func(struct tm_elig_prio_func_node *func_table, 
                                   uint16_t func_offset,
                                   union tm_elig_prio_func *node_func_value_arr);

#endif /* _SET_LOCAL_DB_DEFAULTS_H_ */
