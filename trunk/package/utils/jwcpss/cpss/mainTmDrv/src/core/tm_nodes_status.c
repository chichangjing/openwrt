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
 * @brief tm_nodes_status implementation.
 *
 * @file tm_nodes_status.c
 *
 * $Revision: 2.0 $
 */

#include "tm_nodes_status.h"
#include "tm_errcodes.h"
#include "tm_locking_interface.h"
#include "set_hw_registers.h"
#include "tm_nodes_ctl_internal.h"


/**
 */
int tm_read_port_status(tm_handle hndl,
                        uint8_t index,
                        struct tm_port_status *tm_status)
{
	int rc;

	TM_CTL(ctl, hndl);

	rc = tm_nodes_lock(TM_ENV(ctl));
	if (rc)  return rc;

	VALIDATE_NODE_ALLOCATED(port,index)
	if (rc) goto out;
	
	if (get_hw_port_status(ctl, index, tm_status))	rc = -TM_HW_READ_PORT_STATUS_FAIL;

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_c_node_status(tm_handle hndl,
                          uint32_t index,
                          struct tm_node_status *tm_status)
{
    int rc;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(C_LEVEL,index)   /** if supported - inside macro */
	
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

	VALIDATE_NODE_ALLOCATED(c_node,index)
	if (rc) goto out;
	
    if ( get_hw_c_node_status(ctl, index, tm_status))  rc = -TM_HW_READ_C_NODE_STATUS_FAIL;
    
out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_b_node_status(tm_handle hndl,
                          uint32_t index,
                          struct tm_node_status *tm_status)
{
    int rc;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(B_LEVEL,index)   /** if supported - inside macro */
	
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

	VALIDATE_NODE_ALLOCATED(b_node,index)
	if (rc) goto out;
	
    if ( get_hw_b_node_status(ctl, index, tm_status)) rc = -TM_HW_READ_B_NODE_STATUS_FAIL;

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_a_node_status(tm_handle hndl,
                          uint32_t index,
                          struct tm_node_status *tm_status)
{
    int rc;

    TM_CTL(ctl, hndl);
	
	CONVERT_TO_PHYSICAL(A_LEVEL,index)   /** if supported - inside macro */

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)   return rc;

	VALIDATE_NODE_ALLOCATED(a_node,index)
	if (rc) goto out;
	
    if ( get_hw_a_node_status(ctl, index, tm_status)) rc = -TM_HW_READ_A_NODE_STATUS_FAIL;

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_read_queue_status(tm_handle hndl,
                         uint32_t index,
                         struct tm_node_status *tm_status)
{
    int rc;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(Q_LEVEL,index)   /** if supported - inside macro */

	rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

	VALIDATE_NODE_ALLOCATED(queue,index)
	if (rc) goto out;
	
    if ( get_hw_queue_status(ctl, index, tm_status)) rc = -TM_HW_READ_QUEUE_STATUS_FAIL;

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}


/**
 */
int tm_drop_get_queue_length(tm_handle hndl,
                             enum tm_level level,
                             uint32_t index,
                             uint32_t *inst_q_length,
							 uint32_t *av_q_length)
{
    int rc;

    TM_CTL(ctl, hndl);

	CONVERT_TO_PHYSICAL(level,index)   /** if supported - inside macro */
	
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;

    if (level > P_LEVEL)
	{
        rc = -ERANGE;
        goto out;
    }

    switch (level)
	{
    case Q_LEVEL:
		VALIDATE_NODE_ALLOCATED(queue,index)
        break;
    case A_LEVEL:
		VALIDATE_NODE_ALLOCATED(a_node,index)
        break;
    case B_LEVEL:
		VALIDATE_NODE_ALLOCATED(b_node,index)
        break;
    case C_LEVEL:
		VALIDATE_NODE_ALLOCATED(c_node,index)
       break;
    case P_LEVEL:
 		VALIDATE_NODE_ALLOCATED(port,index)
        break;
    default:
        rc = -ERANGE;
    }
    if (rc) goto out;

    if ( get_hw_queue_length(ctl, level, index, inst_q_length, av_q_length)) rc = -TM_HW_GET_QUEUE_LENGTH_FAIL;

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

