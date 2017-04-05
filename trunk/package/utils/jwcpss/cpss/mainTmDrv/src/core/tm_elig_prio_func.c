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
 * @brief tm eligible functions API implementation 
 *
 * @file tm_elig_prio_func.c
 *
 * $Revision: 2.0 $
 */

#include <stdlib.h>
#include <errno.h>

#include "tm_elig_prio_func.h"
#include "tm_errcodes.h"
#include "tm_core_types.h"

#include "set_hw_registers.h"

#include "tm_set_local_db_defaults.h"
#include "tm_os_interface.h"



int tm_config_elig_prio_func_table(tm_handle hndl, int updateHW)
{
    int rc = 0;

    TM_CTL(ctl, hndl);

	/* Update SW image */

    set_default_queue_elig_prio_func_table(ctl->tm_elig_prio_q_lvl_tbl);

	set_default_node_elig_prio_func_table(ctl->tm_elig_prio_a_lvl_tbl);
	set_default_node_elig_prio_func_table(ctl->tm_elig_prio_b_lvl_tbl);
	set_default_node_elig_prio_func_table(ctl->tm_elig_prio_c_lvl_tbl);
	set_default_node_elig_prio_func_table(ctl->tm_elig_prio_p_lvl_tbl);

	if (updateHW)
	{
		/* update HW */
		rc = set_hw_elig_prio_func_tbl_q_level(hndl);
		if (rc) return rc;
		rc = set_hw_elig_prio_func_tbl_a_level(hndl);
		if (rc) return rc;
		rc = set_hw_elig_prio_func_tbl_b_level(hndl);
		if (rc) return rc;
		rc = set_hw_elig_prio_func_tbl_c_level(hndl);
		if (rc) return rc;
		rc = set_hw_elig_prio_func_tbl_p_level(hndl);
		if (rc) return rc;
	}
    if (rc)
        rc = -TM_HW_ELIG_PRIO_FUNC_FAILED;
	return rc;
}



	
/**
 */
int tm_elig_prio_func_config(tm_handle hndl,
							uint16_t elig_prio_func_index,
							enum tm_level level,
							union tm_elig_prio_func *func_value_arr)
{
    int rc = 0;

    TM_CTL(ctl, hndl);

	if (elig_prio_func_index >= TM_ELIG_FUNC_TABLE_SIZE) return  -EDOM;
	/* TM_ELIG_Q_DEQ_DIS function udate is not allowed */
	if (elig_prio_func_index == TM_ELIG_DEQ_DISABLE) return  -EDOM;
	/* TM_NODE_DISABLED_FUN  slot is for internal use */
	if (elig_prio_func_index == TM_NODE_DISABLED_FUN) return  -EDOM;
	
	switch (level)
	{		
		case Q_LEVEL:
		{
			/* Update SW image for queue level */
			set_sw_q_elig_prio_func(ctl->tm_elig_prio_q_lvl_tbl, elig_prio_func_index, func_value_arr);
			/* update HW */
			rc = set_hw_q_elig_prio_func_entry(hndl, elig_prio_func_index);		
			break;
		}
		case A_LEVEL:
		{
			/* Update SW image for A node level */
			set_sw_n_elig_prio_func(ctl->tm_elig_prio_a_lvl_tbl, elig_prio_func_index, func_value_arr);
			/* update HW */
			rc=set_hw_a_lvl_elig_prio_func_entry(hndl, elig_prio_func_index);
			break;
		}
		case B_LEVEL:
		{
			/* Update SW image for B node level */
			set_sw_n_elig_prio_func(ctl->tm_elig_prio_b_lvl_tbl, elig_prio_func_index, func_value_arr);
			/* update HW */
			rc=set_hw_b_lvl_elig_prio_func_entry(hndl, elig_prio_func_index);
			break;
		}
		case C_LEVEL:
		{
			/* Update SW image for C node level */
			set_sw_n_elig_prio_func(ctl->tm_elig_prio_c_lvl_tbl, elig_prio_func_index, func_value_arr);
			/* update HW */
			rc=set_hw_c_lvl_elig_prio_func_entry(hndl, elig_prio_func_index);
			break;
		}
		case P_LEVEL:
		{
			/* Update SW image for Port level */
			set_sw_n_elig_prio_func(ctl->tm_elig_prio_p_lvl_tbl, elig_prio_func_index, func_value_arr);
			/* update HW */
			rc=set_hw_p_lvl_elig_prio_func_entry(hndl, elig_prio_func_index);
			break;
		}
		default: return -EADDRNOTAVAIL;
	}
	if (rc)
		rc = -TM_HW_ELIG_PRIO_FUNC_FAILED;
	return rc;
}


/**
 */
int tm_elig_prio_func_config_all_levels(tm_handle hndl,
									uint16_t elig_prio_func_ptr,
									union tm_elig_prio_func *func_value_arr)
{

	enum tm_level lvl;
	int rc = 0;

    TM_CTL(ctl, hndl);

	if (elig_prio_func_ptr >= TM_ELIG_FUNC_TABLE_SIZE)
	{
		rc = -EDOM;
		goto out;
	}

	for (lvl = A_LEVEL; lvl <= P_LEVEL; lvl++)
	{
		rc = tm_elig_prio_func_config(ctl, elig_prio_func_ptr, lvl, func_value_arr);
		if (rc)
		{
            rc = -TM_HW_ELIG_PRIO_FUNC_FAILED;
			goto out;
        }
	}
out:
	return rc;
}




/**
 * Show A to P level Eligible Priority Function
 */
int tm_show_hw_elig_prio_func(tm_handle hndl, enum tm_level level, uint16_t func_offset)
{
	int rc = -EFAULT;
	struct tm_elig_prio_func_node tm_elig_prio_lvl_tbl;
	struct tm_elig_prio_func_node *params = &tm_elig_prio_lvl_tbl;
	int i;
	uint16_t elig_func_val_0;
	uint16_t elig_func_val_1;
	uint16_t elig_func_val_2;
	uint16_t elig_func_val_3;

    TM_CTL(ctl, hndl);

	rc = tm_get_hw_elig_prio_func(hndl, level, func_offset, params);

	if (rc)
		goto out;

	tm_printf("Function: %d\n", func_offset);

	for (i = 0; i < 8; i++) {
		elig_func_val_0 = params->tbl_entry[i].func_out[0];
		elig_func_val_1 = params->tbl_entry[i].func_out[1];
		elig_func_val_2 = params->tbl_entry[i].func_out[2];
		elig_func_val_3 = params->tbl_entry[i].func_out[3];

		tm_printf("%02d: 0x%03x  %02d: 0x%03x  %02d: 0x%03x  %02d: 0x%03x\n",
				(i*4), elig_func_val_0,
				(i*4) + 1, elig_func_val_1,
				(i*4) + 2, elig_func_val_2,
				(i*4) + 3, elig_func_val_3);
	}

out:
	return rc;
}

/**
 * Show deatailed A to P level Eligible Priority Function header
 */
void tm_show_elig_func_header(void)
{
	/* in - MinTBNeg, MaxTBNeg, PropPrio */
	/* out - Elig, SchdPrio, PropPrio, MinTBUsed, MaxTBUsed */
	tm_printf("      Input            OutPut          Value\n");
	tm_printf("    ---------     -----------------    -----\n");
	tm_printf("    M   M   P     E   S   P   M   M\n");
	tm_printf("    i   a   r     l   c   r   i   a\n");
	tm_printf("    n   x   o     i   h   o   n   x\n");
	tm_printf("    T   T   p     g   d   p   T   T\n");
	tm_printf("    B   B   P         P   P   B   B\n");
	tm_printf("    N   N   r         r   r   U   U\n");
	tm_printf("    e   e   i         i   i   s   s\n");
	tm_printf("    g   g   o         o   o   e   e\n");
	tm_printf("    -   -   -     -   -   -   -   -\n");
}

/**
 * Show deatailed A to P level Eligible Priority Function
 */
int tm_show_hw_elig_prio_func_detailed(tm_handle hndl, enum tm_level level, uint16_t func_offset)
{
	int rc = -EFAULT;
	struct tm_elig_prio_func_out elig_func_out[4];
	struct tm_elig_prio_func_node tm_elig_prio_lvl_tbl;
	struct tm_elig_prio_func_node *params = &tm_elig_prio_lvl_tbl;	
	int i,j;
	int entry_index;
	uint16_t elig_val[4];
	int prop_prio;
	
	rc = tm_get_hw_elig_prio_func(hndl, level, func_offset, params);

	if (rc)
		goto out;

	tm_printf("Function: %d\n", func_offset);
	tm_show_elig_func_header();

	for (i = 0; i < 8; i++) {
		convert_value_to_elig_func(params->tbl_entry[i].func_out[0], &elig_func_out[0]);
		convert_value_to_elig_func(params->tbl_entry[i].func_out[1], &elig_func_out[1]);
		convert_value_to_elig_func(params->tbl_entry[i].func_out[2], &elig_func_out[2]);
		convert_value_to_elig_func(params->tbl_entry[i].func_out[3], &elig_func_out[3]);

		elig_val[0] = params->tbl_entry[i].func_out[0];
		elig_val[1] = params->tbl_entry[i].func_out[1];
		elig_val[2] = params->tbl_entry[i].func_out[2];
		elig_val[3] = params->tbl_entry[i].func_out[3];

		for (j = 0; j < 4; j++) {
			entry_index = i*4 + j;	
			prop_prio = entry_index & 7;
			tm_printf("%02d: %d   %d   %d     %d   %d   %d   %d   %d   0x%03x\n",
					entry_index,
					entry_index / 16,
					(entry_index / 8) %2,
					prop_prio,
					elig_func_out[j].elig,
					elig_func_out[j].sched_prio,
					elig_func_out[j].prop_prio,
					elig_func_out[j].min_tb,
					elig_func_out[j].max_tb,
					elig_val[j]);
		}
	}

out:
	return rc;
}


/**
 * Show Q level Eligible Priority Function
 */
int tm_show_hw_elig_prio_q_func(tm_handle hndl, uint16_t func_offset)
{
	int rc = -EFAULT;
	struct tm_elig_prio_func_queue tm_elig_prio_q_tbl;
	struct tm_elig_prio_func_queue *params = &tm_elig_prio_q_tbl;
	uint16_t elig_func_val_0;
	uint16_t elig_func_val_1;
	uint16_t elig_func_val_2;
	uint16_t elig_func_val_3;

	rc = tm_get_hw_elig_prio_q_func(hndl, func_offset, params);

	if (rc)
		goto out;

	elig_func_val_0 = params->tbl_entry.func_out[0];
	elig_func_val_1 = params->tbl_entry.func_out[1];
	elig_func_val_2 = params->tbl_entry.func_out[2];
	elig_func_val_3 = params->tbl_entry.func_out[3];

	tm_printf("%02d: 0x%03x  %02d: 0x%03x  %02d: 0x%03x  %02d: 0x%03x\n",
			0, elig_func_val_0,
			1, elig_func_val_1,
			2, elig_func_val_2,
			3, elig_func_val_3);

out:
	return rc;
}

/**
 * Show deatailed q level Eligible Priority Function header
 */
void tm_show_elig_func_q_header(void)
{
	/* in - MinTBNeg, MaxTBNeg, PropPrio */
	/* out - Elig, SchdPrio, PropPrio, MinTBUsed, MaxTBUsed */
	tm_printf("    Input       OutPut            Value\n");
	tm_printf("    -----    -----------------    -----\n");
	tm_printf("    M   M    E   S   P   M   M\n");
	tm_printf("    i   a    l   c   r   i   a\n");
	tm_printf("    n   x    i   h   o   n   x\n");
	tm_printf("    T   T    g   d   p   T   T\n");
	tm_printf("    B   B        P   P   B   B\n");
	tm_printf("    N   N        r   r   U   U\n");
	tm_printf("    e   e        i   i   s   s\n");
	tm_printf("    g   g        o   o   e   e\n");
	tm_printf("    -   -    -   -   -   -   -\n");
}

/**
 * Show deatailed q level Eligible Priority Function
 */
int tm_show_hw_elig_prio_func_q_detailed(tm_handle hndl, uint16_t func_offset)
{
	int rc = -EFAULT;
	struct tm_elig_prio_func_out elig_func_out[4];
	struct tm_elig_prio_func_queue tm_elig_prio_q_tbl;
	struct tm_elig_prio_func_queue *params = &tm_elig_prio_q_tbl;
	int j;
	int entry_index;
	uint16_t elig_val[4];
	
	rc = tm_get_hw_elig_prio_q_func(hndl, func_offset, params);
	if (rc)
		goto out;

	tm_printf("Function: %d\n", func_offset);
	tm_show_elig_func_q_header();

	convert_value_to_elig_func(params->tbl_entry.func_out[0], &elig_func_out[0]);
	convert_value_to_elig_func(params->tbl_entry.func_out[1], &elig_func_out[1]);
	convert_value_to_elig_func(params->tbl_entry.func_out[2], &elig_func_out[2]);
	convert_value_to_elig_func(params->tbl_entry.func_out[3], &elig_func_out[3]);

	elig_val[0] = params->tbl_entry.func_out[0];
	elig_val[1] = params->tbl_entry.func_out[1];
	elig_val[2] = params->tbl_entry.func_out[2];
	elig_val[3] = params->tbl_entry.func_out[3];

	for (j = 0; j < 4; j++) {
		entry_index =  j;	
		tm_printf("%02d: %d   %d    %d   %d   %d   %d   %d     0x%03x\n",
			entry_index,
			entry_index / 2,
			entry_index  %2,
			elig_func_out[j].elig,
			elig_func_out[j].sched_prio,
			elig_func_out[j].prop_prio,
			elig_func_out[j].min_tb,
			elig_func_out[j].max_tb,
			elig_val[j]);
	}

out:
	return rc;
} 

int tm_dump_elig_func(tm_handle hndl, int level, uint16_t func_index, int format)
{
	int rc = 0;

	if (level < Q_LEVEL || level > P_LEVEL)
	{
		tm_printf("Error: wrong level param\n");
		return 1;
	}

	if (func_index > 63)
	{
		tm_printf("Error: wrong func_index param\n");
		return 1;
	}

    if (level == Q_LEVEL)
        if (format == 1)
            rc = tm_show_hw_elig_prio_func_q_detailed(hndl, func_index);
        else
            rc = tm_show_hw_elig_prio_q_func(hndl,  func_index);
    else
    {
        if (format == 1)
            rc = tm_show_hw_elig_prio_func_detailed(hndl, level, func_index);
        else
            rc = tm_show_hw_elig_prio_func(hndl, level, func_index);
    }

    return rc;
}
