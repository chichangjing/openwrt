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
 * @brief cpss mutex implementation for TM.
 *
 * @file cpss_locking_implementation.c
 *
 * $Revision: 2.0 $
 */

#include "tm_locking_interface.h"
#include "tm_platform_implementation_definitions.h"


int	tm_create_locking_staff(void * environment_handle)
{
	int rc = 0;
	(void)environment_handle;
	return rc;
}

int	tm_destroy_locking_staff(void * environment_handle)
{
	(void)environment_handle;
	return 0;
}

/**
 */
  
int tm_glob_lock(void * environment_handle)
{
	(void)environment_handle;
    return 0;
}

/**
 */
int tm_glob_unlock(void * environment_handle)
{
	(void)environment_handle;
    return 0;
}

/**
 */
 
int tm_nodes_lock(void * environment_handle)
{
	(void)environment_handle;
    return 0;
}

/**
 */
int tm_nodes_unlock(void * environment_handle)
{
	(void)environment_handle;
    return 0;
}

/**
 */
 
int tm_sched_lock(void * environment_handle)
{
	(void)environment_handle;
    return 0;
}

/**
 */
int tm_sched_unlock(void * environment_handle)
{
	(void)environment_handle;
    return 0;
}
