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
 * @brief TM Configuration Library Control Interface - internal part
 *
 * @file tm_ctl_internal.h
 *
 * $Revision: 2.0 $
 */

#ifndef   	TM_CTL_INTERNAL_H
#define   	TM_CTL_INTERNAL_H

#include "tm_defs.h"	 /* in order to define tm_handle */


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
int tm_lib_sw_init(tm_handle hEnv, tm_handle *htm, struct tm_lib_init_params *init_params_PTR);


/** 
 * @brief   Initiate TM related H/W resources.
 * *
 * @return an integer return code.
 * @retval zero on success.
 * @retval -EINVAL if any of handles is NULL.
 * @retval -EBADF if any of handles is invalid.
 *
 * @retval TM_CONF_CANNT_GET_LAD_FREQUENCY.
 * @retval TM_HW_GEN_CONFIG_FAILED.
 */


int tm_lib_hw_init(tm_handle hndl);

/** Close TM configuration library.
 * 
 *   @param[in]		hndl		TM lib handle
 *
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 */
int tm_lib_close_sw(tm_handle hndl);


/** Restart TM configuration library H/W.
 * 
 *   @param[in]		hndl		TM lib handle
 *
 *   @retval zero on success. 
 *   @retval -EINVAL if hndl is NULL.
 *   @retval -EBADF if hndl is an invalid handle.
 */
int tm_lib_restart_hw(tm_handle hndl);

#endif   /* TM_CTL_INTERNAL_H */

