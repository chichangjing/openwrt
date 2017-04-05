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
 * @brief internal API for TM TM Shaping configuration.
 *
 * @file tm_shaping_internal.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_SHAPING_INTERNAL_H
#define   	TM_SHAPING_INTERNAL_H

#include "tm_core_types.h"



/* reserves and initializes default shaping profile - TM_INF_SHP_PROFILE */
int init_infinite_shaping_profile(tm_handle hndl);

/* internal utilities for managing clients of shaping profiles */
							  
int check_shaping_profile_validity(	tm_handle hndl,uint32_t prof_index, enum tm_level level);
							  
int add_node_to_shaping_profile(tm_handle hndl, uint32_t prof_index,enum tm_level level,uint32_t node_index);

int remove_node_from_shaping_profile(tm_handle hndl, uint32_t prof_index,enum tm_level level,uint32_t node_index);
							  
#ifdef PER_NODE_SHAPING

	int update_node_shaping_proc(tm_handle hndl,
									  enum tm_level level,
									  uint32_t node_index,
									  struct tm_shaping_profile_params * node_shaping_params);

	int read_node_shaping_proc(tm_handle hndl,
								  enum tm_level level,
								  uint32_t node_index,
								  uint16_t * shaping_profile,
								  struct tm_shaping_profile_params * node_shaping_params);
#endif							  
#endif   /* PRV_TM_SHAPING_H */
