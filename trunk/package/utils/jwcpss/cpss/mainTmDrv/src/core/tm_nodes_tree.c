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
 * @brief tm_nodes_tree implementation.
 *
 * @file tm_nodes_tree.c
 *
 * $Revision: 2.0 $
 */

#include "tm_nodes_tree.h"

#include "tm_locking_interface.h"
#include "tm_errcodes.h"
#include <errno.h>
#include "set_hw_registers.h"


/**
 */
int tm_tree_change_status(tm_handle hndl, uint8_t status)
{
	int rc;

    TM_CTL(ctl, hndl);
 
    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc)
        return rc;
    if ((status != TM_ENABLE) && (status != TM_DISABLE))
	{
        rc = -EFAULT;
        goto out;
    }

    ctl->tree_deq_status = status;

    rc = set_hw_tree_deq_status(hndl);
    if (rc < 0)
        rc = -TM_HW_TREE_CONFIG_FAIL;

out:
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

int tm_tree_get_deq_status(tm_handle hndl, uint8_t * status)
{
	int rc;

    TM_CTL(ctl, hndl);

	rc = get_hw_tree_deq_status(hndl);
    if (rc < 0)
	{
        rc = -TM_HW_TREE_CONFIG_FAIL;
		goto out;
	}
    *status = ctl->tree_deq_status;
out:
    return rc;
}

/**
 */
int tm_tree_set_dwrr_prio(tm_handle hndl, uint8_t * prios)
{
    int rc ;
    int i;

    TM_CTL(ctl, hndl);

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;
    
    ctl->tree_dwrr_enable_per_priority_mask = 0;
    for (i=0; i<8; i++)
	{
        if ((prios[i] != TM_ENABLE) && (prios[i] != TM_DISABLE))
		{
            rc = -EFAULT;
            goto out;
        }
        ctl->tree_dwrr_enable_per_priority_mask = ctl->tree_dwrr_enable_per_priority_mask | (prios[i] << i);
    }

    rc = set_hw_tree_dwrr_priority(hndl);
    if (rc) rc = -TM_HW_TREE_CONFIG_FAIL;

out:
    if (rc < 0)	ctl->tree_dwrr_enable_per_priority_mask = 0;
    
    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}

int tm_tree_get_dwrr_prio_status(tm_handle hndl, uint8_t *prios)
{
    int rc ;
    int i;

    TM_CTL(ctl, hndl);

    rc = tm_nodes_lock(TM_ENV(ctl));
    if (rc) return rc;
    
    rc = get_hw_tree_dwrr_priority_status(hndl);
    if (rc) rc = -TM_HW_TREE_CONFIG_FAIL;

    for (i=0; i<8; i++)
	{
        prios[i] = ( (ctl->tree_dwrr_enable_per_priority_mask & (1 << i)) !=0);
    }

    tm_nodes_unlock(TM_ENV(ctl));
    return rc;
}
