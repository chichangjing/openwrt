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
 * @brief tm_sched implementation.
 *
 * @file tm_sched.c
 *
 * $Revision: 2.0 $
 */

#include <stdio.h>
#include <errno.h>

#include "tm_sched.h"

#include "tm_errcodes.h"
#include "tm_locking_interface.h"
#include "set_hw_registers.h"
#include "tm_set_local_db_defaults.h"
#include "tm_os_interface.h"
#include "tm_hw_configuration_interface.h"
#include "tm_shaping_utils.h"

/**
*/
int tm_sched_general_config(tm_handle hndl, uint8_t port_ext_bp)
{
    int rc;

    TM_CTL(ctl, hndl)

    rc = tm_glob_lock(hndl);
    if (rc)  return rc;

    if ((port_ext_bp != TM_ENABLE) && (port_ext_bp != TM_DISABLE))
	{
        rc = -EFAULT;
        goto out;
    }

    /* update relevant fields in hndl */
    ctl->port_ext_bp_en = port_ext_bp;

    rc = set_hw_gen_conf(hndl);
    if (rc < 0) rc = TM_HW_GEN_CONFIG_FAILED;

out:
    if (rc == TM_HW_GEN_CONFIG_FAILED)  set_sw_gen_conf_default(hndl);

    tm_glob_unlock(hndl);
    return rc;
}



/**
 */
int tm_configure_periodic_scheme(tm_handle hndl, struct tm_level_periodic_params *lvl_params)
{
    int rc;
    int i;

	unsigned int	base_update_frequency;
	unsigned int	node_shaper_update_frequency;
	unsigned int	port_shaper_update_frequency;
	double			desired_factor;
    uint16_t		L;
	uint16_t		K;
	uint16_t		N;

    TM_CTL(ctl, hndl)

    rc = tm_sched_lock(hndl);
    if (rc)
        return rc;

	if (ctl->periodic_scheme_state == TM_ENABLE)
	{
        rc = -EPERM;
        goto out;
    }


    base_update_frequency = ctl->freq/4;

    /* check if shaping is Enabled/Disabled per level */
    /* no need to indicate the number of elements participating periodic
     * shaper update for Rev B */
    for (i=Q_LEVEL; i<=P_LEVEL; i++)
	{
		switch (lvl_params[i].per_state)
		{
			case TM_ENABLE:		ctl->level_data[i].shaping_status=TM_ENABLE;break;
			case TM_DISABLE:	ctl->level_data[i].shaping_status=TM_DISABLE;break;
			default:/*all others */
			{
				rc = -EADDRNOTAVAIL;
				goto out;
			}
		}
		switch (lvl_params[i].shaper_dec)
		{
			case TM_ENABLE:		ctl->level_data[i].shaper_dec=TM_ENABLE;break;
			case TM_DISABLE:	ctl->level_data[i].shaper_dec=TM_DISABLE;break;
			default:/*all others */
			{
				rc = -ENODEV;
				goto out;
			}
		}
	}
	/* here play a little with frequencies */	
	if (base_update_frequency > OPTIMAL_NODE_FREQUENCY)
		node_shaper_update_frequency=OPTIMAL_NODE_FREQUENCY;
	else													
		node_shaper_update_frequency = (base_update_frequency/1000000)*1000000; /* rounding to integer MHz */

	desired_factor= (double)node_shaper_update_frequency/base_update_frequency;
	if (!found_L_K_N(desired_factor, &L, &K , &N)) 	return TM_CONF_PER_RATE_L_K_N_NOT_FOUND;

	/*set periodic update for levels */
	/* queue  */
	ctl->level_data[Q_LEVEL].level_update_frequency=node_shaper_update_frequency;
	ctl->level_data[Q_LEVEL].unit=Q_UNIT;
	ctl->level_data[Q_LEVEL].per_interval = node_shaper_update_frequency*8/(1<<MAX_DIVIDER_EXP)/Q_UNIT;
	ctl->level_data[Q_LEVEL].token_res_exp=QUEUE_OPTIMAL_RES_EXP;
	ctl->level_data[Q_LEVEL].L = L;
    ctl->level_data[Q_LEVEL].K = K;
    ctl->level_data[Q_LEVEL].N = N;
	/* A-level */
	ctl->level_data[A_LEVEL].level_update_frequency=node_shaper_update_frequency;
	ctl->level_data[A_LEVEL].unit=A_UNIT;
	ctl->level_data[A_LEVEL].per_interval = node_shaper_update_frequency*8/(1<<MAX_DIVIDER_EXP)/A_UNIT;
	ctl->level_data[A_LEVEL].token_res_exp=A_NODE_OPTIMAL_RES_EXP;
    ctl->level_data[A_LEVEL].L = L;
    ctl->level_data[A_LEVEL].K = K;
    ctl->level_data[A_LEVEL].N = N;
	/* B-level */
	ctl->level_data[B_LEVEL].level_update_frequency=node_shaper_update_frequency;
	ctl->level_data[B_LEVEL].unit=B_UNIT;
	ctl->level_data[B_LEVEL].per_interval = node_shaper_update_frequency*8/(1<<MAX_DIVIDER_EXP)/B_UNIT;
	ctl->level_data[B_LEVEL].token_res_exp=B_NODE_OPTIMAL_RES_EXP;
    ctl->level_data[B_LEVEL].L = L;
    ctl->level_data[B_LEVEL].K = K;
    ctl->level_data[B_LEVEL].N = N;
	/* C-level */
	ctl->level_data[C_LEVEL].level_update_frequency=node_shaper_update_frequency;
	ctl->level_data[C_LEVEL].unit=C_UNIT;
	ctl->level_data[C_LEVEL].per_interval = node_shaper_update_frequency*8/(1<<MAX_DIVIDER_EXP)/C_UNIT;
	ctl->level_data[C_LEVEL].token_res_exp=C_NODE_OPTIMAL_RES_EXP;
    ctl->level_data[C_LEVEL].L = L;
    ctl->level_data[C_LEVEL].K = K;
    ctl->level_data[C_LEVEL].N = N;
	/*---------------------------------------------------------------------------------------------------------
	*/
	if (base_update_frequency > OPTIMAL_PORT_FREQUENCY)
		port_shaper_update_frequency=OPTIMAL_PORT_FREQUENCY;
	else													
		port_shaper_update_frequency = (base_update_frequency/5000000)*5000000; /* rounding in order to get more accuracy */
	/* here provide it in by K,L,N */
	desired_factor= (double)port_shaper_update_frequency/base_update_frequency;
	if (found_L_K_N(desired_factor, &L, &K , &N ))
	{
		ctl->level_data[P_LEVEL].L=L;
		ctl->level_data[P_LEVEL].K=K;
		ctl->level_data[P_LEVEL].N=N;
		ctl->level_data[P_LEVEL].level_update_frequency=port_shaper_update_frequency;
		ctl->level_data[P_LEVEL].per_interval = port_shaper_update_frequency*8/PORT_PERIODS_MAX/PORT_UNIT;
		ctl->level_data[P_LEVEL].unit=PORT_UNIT;
		ctl->level_data[P_LEVEL].token_res_exp=PORT_OPTIMAL_RES_EXP;
	}
	else return TM_CONF_PER_RATE_L_K_N_NOT_FOUND;

#ifdef PRINT_SHAPING_RESULTS
	tm_printf("--------------------------------------------------------------\n");
	tm_printf("base update frequency = % d \n",base_update_frequency);
	for (i=Q_LEVEL ; i <= P_LEVEL ; i++)
	{
		tm_printf(" level : %u, frequency : %u , per_interval : %4u , token_res_exp=%d  K=%u, L=%u, N=%2u,(desired factor=%f factor=%f)\n",
				  i,
				  ctl->level_data[i].level_update_frequency,
				  ctl->level_data[i].per_interval,
				  ctl->level_data[i].token_res_exp,
				  ctl->level_data[i].K,
				  ctl->level_data[i].L,
				  ctl->level_data[i].N,
				  1.0*ctl->level_data[i].level_update_frequency/base_update_frequency,
				  1.0-(double)(ctl->level_data[i].K + ctl->level_data[i].L)/(double)((ctl->level_data[i].N+1)*ctl->level_data[i].K+ctl->level_data[i].L*((ctl->level_data[i].N+1)+1)));
	}
	tm_printf("--------------------------------------------------------------\n");
#endif

    /* for all levels at once */
#if 0
    rc = calculate_scrubbing(hndl);
    if (rc)
        goto out;
#endif
    /* Download to HW */
    rc = set_hw_periodic_scheme(hndl);
    if (rc)
	{
        rc = TM_HW_CONF_PER_SCHEME_FAILED;
        goto out;
    }

    ctl->periodic_scheme_state = TM_ENABLE;
out:
    tm_sched_unlock(hndl);
    return rc;
}

int tm_periodic_update_status_set(tm_handle hndl, enum tm_level level, uint8_t status)
{
    int rc;
	uint8_t old_status;

    TM_CTL(ctl, hndl)

    rc = tm_sched_lock(TM_ENV(ctl));
    if (rc)
        return rc;

    /* check parameters validity */
    if ((uint32_t)level > P_LEVEL)
	{
        rc = -EFAULT;
        goto out;
    }
    if ((status != TM_ENABLE) && (status != TM_DISABLE))
	{
        rc = -EACCES;
        goto out;
    }
    old_status = ctl->level_data[level].shaping_status;
	ctl->level_data[level].shaping_status = status; /* TM_ENABLE/TM_DISABLE */

    rc = set_hw_shaping_status((tm_handle)ctl, level);
    if (rc < 0)
	{
        /* set to default */
        ctl->level_data[level].shaping_status = old_status;
        rc = TM_HW_PERIODIC_UPDATE_STATUS_SET_FAILED;
    }
out:
    tm_sched_unlock(TM_ENV(ctl));
    return rc;
}


int tm_periodic_update_status_get(tm_handle hndl, struct tm_level_periodic_params  *status)
{
    int rc;
	int i;

    TM_CTL(ctl, hndl)

    rc = tm_sched_lock(TM_ENV(ctl));
    if (rc) return rc;

    for (i=Q_LEVEL; i<=P_LEVEL; i++)
	{
		status[i].per_state = ctl->level_data[i].shaping_status;
		status[i].shaper_dec = ctl->level_data[i].shaper_dec;
	}

    tm_sched_unlock(TM_ENV(ctl));
    return rc;
}

/**
 */
int tm_port_level_set_dwrr_bytes_per_burst_limit(tm_handle hndl, uint8_t bytes)
{
    int rc;

    TM_CTL(ctl, hndl)

	rc = tm_sched_lock(hndl);
    if (rc) return rc;

    /* check parameters validity */
    if (bytes > 0x7F)
	{
        rc = -EFAULT;
        goto out;
    }

    ctl->dwrr_bytes_burst_limit = bytes;

    rc = set_hw_dwrr_limit(hndl);
    if (rc < 0)
	{
        /* set to default */
        ctl->dwrr_bytes_burst_limit = 0x40;
        rc = TM_HW_PORT_DWRR_BYTES_PER_BURST_FAILED;
    }

out:
    tm_sched_unlock(hndl);
    return rc;
}

int tm_sched_max_selected_unit(tm_handle hndl, unsigned mtu)
{
    TM_CTL(ctl, hndl)

    return ctl->min_pkg_size + mtu;
}

int tm_sched_port_max_selected_unit(tm_handle hndl, unsigned mtu)
{
    uint32_t bytes;
    uint32_t msu;

    TM_CTL(ctl, hndl)

    bytes = 16 * 4 * ctl->port_ch_emit * ctl->dwrr_bytes_burst_limit;
    msu = ctl->min_pkg_size + mtu;

    if (msu >= bytes)
        return msu;
    else
        return bytes;
}

int tm_sched_get_port_quantum_limits(tm_handle hndl, struct tm_quantum_limits * port_quantum_limits)
{
    TM_CTL(ctl, hndl)
	port_quantum_limits->resolution  = ctl->port_quantum_chunk_size_bytes;      
	port_quantum_limits->minQuantum  = ctl->min_port_quantum_bytes;
	port_quantum_limits->maxQuantum  = ctl->max_port_quantum_bytes;
	return 0;
}
int tm_sched_get_node_quantum_limits(tm_handle hndl, struct  tm_quantum_limits * node_quantum_limits)
{
    TM_CTL(ctl, hndl)
	node_quantum_limits->resolution  = ctl->node_quantum_chunk_size_bytes;
	node_quantum_limits->minQuantum  = ctl->min_node_quantum_bytes;
	node_quantum_limits->maxQuantum  = ctl->max_node_quantum_bytes;
	return 0;
}

