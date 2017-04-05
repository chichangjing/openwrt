/*
 * (c), Copyright 2011-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
* @brief Miscellaneous internal functions for Eligible
* Priority Functions Tables configuration.
*
*
* @file tm_elig_prio_func.h
*
* $Revision: 2.0 $
*/

#ifndef TM_ELIG_PRIO_H
#define TM_ELIG_PRIO_H

#include "tm_defs.h"


/** Configure the Eligible Priority Function according 
*   to the User Application parameters
*
*	The new Eligible Priority Function is written to the 
*	proper Queue or Node level Eligible Table. 
*	A pointer in the API is provided to the location of the new Eligible Priority Function in the table.
*	
*	Note: 	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*
*   @param[in]	hndl					TM lib handle
*	@param[in]	elig_prio_func_ptr		The new configured eligible function pointer
*	@param[in]	level					A level to configure the Eligible function with	
*	@param[in]	func_out_arr			The Eligible Priority Function structure array pointer
*	
*   @retval zero on success. 
*   @retval -EINVAL if hndl is NULL.
*   @retval -EBADF if hndl is an invalid handle.
*	@retval -EDOM  if (elig_prio_func_ptr > 63)
*   @retval -EADDRNOTAVAIL if level is invalid
*	@retval TM_HW_ELIG_PRIO_FUNC_FAILED when the configuration to the HW failed
*/

int tm_elig_prio_func_config(tm_handle hndl,
							 uint16_t elig_prio_func_ptr,
							 enum tm_level level,
							 union tm_elig_prio_func *func_out_arr);


/** Configure the Eligible Priority Function according 
*   to the User Application parameters
*
*	The following API configures the same Eligible Priority Functions 
*	at all nodes (A, B, C, and Port) levels Elig. Prio. Tables 
*	according to the user?s parameters.
*	It has the same functionality as tm_elig_prio_func_config() 
*	and can be used at the user convenience to configure the 
*	same eligible function to all the Nodes levels (except for Q level)
*	
*	Note: 	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*
*   @param[in]	hndl					TM lib handle
*	@param[in]	elig_prio_func_ptr		The new configured eligible function pointer
*	@param[in]	func_out_arr			The Eligible Priority Function structure array pointer
*	
*   @retval zero on success. 
*   @retval -EINVAL if hndl is NULL.
*   @retval -EBADF if hndl is an invalid handle.
*	@retval -EDOM  if (elig_prio_func_ptr > 63)
*	@retval TM_HW_ELIG_PRIO_FUNC_FAILED when the configuration to the HW failed
*/
int tm_elig_prio_func_config_all_levels(tm_handle hndl,
										uint16_t elig_prio_func_ptr,
										union tm_elig_prio_func *func_out_arr);


/*  internal functions  */

/** Configure the default Eligible Priority Functions 
*   to the Queue and Nodes (A...Port) Elig. Prio. Tables
*
*   The tm_config_elig_prio_func_table() configures the 
*   default Eligible Priority functions to be usable by the user  
*   The configuration is done to all Nodes levels (A...Port) with  
*   the same default Eligible functions. 
*   Queue level Table is configured with different default functions
*   which are suitable to its operation. 
*   
*   @param[in]		hndl		TM lib handle
*   @param[in]		updateHW	if 0 - not update HW / otherwise update HW
*
*   @retval zero on success. 
*   @retval -EINVAL if hndl is NULL.
*   @retval -EBADF if hndl is an invalid handle.
*	@retval TM_HW_ELIG_PRIO_FUNC_FAILED when the configuration to the HW failed
*/

int tm_config_elig_prio_func_table(tm_handle hndl, int updateHW);



int tm_dump_elig_func(tm_handle hndl, int level, uint16_t func_index, int format);

#endif   /* TM_ELIG_PRIO_H */

