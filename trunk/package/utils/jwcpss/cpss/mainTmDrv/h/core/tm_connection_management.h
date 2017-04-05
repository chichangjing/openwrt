#ifndef _TM_CONNECTION_MANAGEMENT_H_
#define _TM_CONNECTION_MANAGEMENT_H_

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
 * @brief interface for communication with another TM modules.
 *
 * @file tm_comnnection_management.h
 *
 * $Revision: 2.0 $
 */


int set_hw_connection(void * environment_handle);
int flush_hw_connection(void * environment_handle);
int reset_hw_connection(void * environment_handle,int error);
int close_hw_connection(void * environment_handle);

#endif	/* _TM_CONNECTION_MANAGEMENT_H_ */
