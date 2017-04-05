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
 * @brief TM Configuration Library Control Interface
 *
 * @file tm_ctl.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_CTL_H
#define   	TM_CTL_H

#include "tm_defs.h"
#include <cpss/generic/cpssTypes.h>


/** Initialize TM configuration library.
 * 
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if any of handles is NULL.
 *   @retval -EBADF if any of handles is invalid.
 *   @retval -ENOMEM when out of memory space.
 *
 *   @retval TM_CONF_INVALID_PROD_NAME.
 */

int tm_lib_open(GT_U8 devNum, struct tm_tree_structure *tree_struct, tm_handle * pHndl);

/** Customize Initialize TM configuration library.
 * 
 *   @param[in]     devNum          HW Device Number
 *   @param[in]     init_params_ptr struct of tm_lib_init_params
 *   @param[out]	hndl		    TM lib handle
 *
 *   @return an integer return code.
 *   @retval zero on success. 
 *   @retval -EINVAL if any of handles is NULL.
 *   @retval -EBADF if any of handles is invalid.
 *   @retval -ENOMEM when out of memory space.
 *
 *   @retval TM_CONF_INVALID_PROD_NAME.
 *   @retval TM_CONF_CANNT_GET_LAD_FREQUENCY.
 *   @retval TM_HW_GEN_CONFIG_FAILED.
 */
int tm_lib_open_ext(
	GT_U8 devNum,
	struct tm_tree_structure *tree_struct,
	struct tm_lib_init_params *init_params_ptr, 
	tm_handle * pHndl);

/** Close TM configuration library.
 * 
 *   @param[in]		hndl		TM lib handle
 *
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 */
int tm_lib_close(tm_handle hndl);

#endif   /* TM_CTL_H */

