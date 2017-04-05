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
 * @brief declarations of platform independent wrappers for data locking .
 *
 * @file tm_locking_interface.h
 *
 * $Revision: 2.0 $
 */
#ifndef TM_LOCKING_INTERFACE_H
#define TM_LOCKING_INTERFACE_H




int	tm_create_locking_staff(void * environment_handle);
int	tm_destroy_locking_staff(void * environment_handle);


/** Lock the TM nodes configuration data from other threads.
 *
 * @param[in]   hndl    TM lib handle
 *
 * @return an integer return code.
 * @retval 0 on success.
 * @retval -EINVAL if hndl is NULL.
 * @retval -EBADF if hndl is invalid.
 */
int tm_nodes_lock(void * environment_handle);

/** Lock the TM Global configuration data from other threads.
 */
int tm_glob_lock(void * environment_handle);

/** Lock the TM scheduling configuration data from other threads.
 */
int tm_sched_lock(void * environment_handle);

/** Unlock TM nodes configuration data for other threads.
 *
 * @param[in]   hndl    TM lib handle
 *
 * @return an integer return code.
 * @retval 0 on success.
 * @retval -EINVAL if hndl is NULL.
 * @retval -EBADF if hndl is invalid.
 */
int tm_nodes_unlock(void * environment_handle);

/** Unlock the TM Global configuration data from other threads.
 */
int tm_glob_unlock(void * environment_handle);

/** Unlock the TM Scheduling configuration data from other threads.
 */
int tm_sched_unlock(void * environment_handle);


#endif   /* TM_LOCKING_INTERFACE_H */

