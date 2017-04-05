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
 * @brief  functions for set/get  common for all platforms registers
 *
 * @file set_hw_registers.c
 *
 * $Revision: 2.0 $
*/

#include "tm_core_types.h"
#include "set_hw_registers_imp.h"

#include "set_hw_registers.h"
#include "tm_os_interface.h"
#include "rm_status.h"

/**
 */
int set_hw_dwrr_limit(tm_handle hndl)
{
    int rc = 0;
    TM_REGISTER_VAR(TM_Sched_PortDWRRBytesPerBurstsLimit)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.PortDWRRBytesPerBurstsLimit,TM_Sched_PortDWRRBytesPerBurstsLimit)
	if (rc)	goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Sched_PortDWRRBytesPerBurstsLimit, limit , ctl->dwrr_bytes_burst_limit)
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.PortDWRRBytesPerBurstsLimit,TM_Sched_PortDWRRBytesPerBurstsLimit)

out:
    COMPLETE_HW_WRITE;
	return rc;
}


int set_hw_gen_conf(tm_handle hndl)
{
    int rc = 0;
    TM_REGISTER_VAR(TM_Sched_PortExtBPEn)

    TM_CTL(ctl, hndl)
    
	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.PortExtBPEn, TM_Sched_PortExtBPEn);
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Sched_PortExtBPEn , En , ctl->port_ext_bp_en);
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.PortExtBPEn, TM_Sched_PortExtBPEn)
out:
    COMPLETE_HW_WRITE;
	return rc;
}

/**
 */
int set_hw_max_dp_mode(tm_handle hndl)
{
    int rc = 0;
    int i;
    uint8_t port = 0;
    uint8_t c_lvl = 0;
    uint8_t b_lvl = 0;
    uint8_t a_lvl = 0;
    uint8_t queue = 0;

    TM_REGISTER_VAR(TM_Drop_WREDMaxProbModePerColor)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.WREDMaxProbModePerColor,TM_Drop_WREDMaxProbModePerColor)
	if (rc)	goto out;
	/* assign register fields */		
	for (i=0; i<3; i++)
	{
        port  = port  | (ctl->dp_unit.local[P_LEVEL].max_p_mode[i] << (i*2));
	    c_lvl = c_lvl | (ctl->dp_unit.local[C_LEVEL].max_p_mode[i] << (i*2));
	    b_lvl = b_lvl | (ctl->dp_unit.local[B_LEVEL].max_p_mode[i] << (i*2));
	    a_lvl = a_lvl | (ctl->dp_unit.local[A_LEVEL].max_p_mode[i] << (i*2));
	    queue = queue | (ctl->dp_unit.local[Q_LEVEL].max_p_mode[i] << (i*2));
	}

    TM_REGISTER_SET(TM_Drop_WREDMaxProbModePerColor,Port,port)
    TM_REGISTER_SET(TM_Drop_WREDMaxProbModePerColor,Clvl,c_lvl)
    TM_REGISTER_SET(TM_Drop_WREDMaxProbModePerColor,Blvl,b_lvl)
    TM_REGISTER_SET(TM_Drop_WREDMaxProbModePerColor,Alvl,a_lvl)
    TM_REGISTER_SET(TM_Drop_WREDMaxProbModePerColor,Queue,queue)
	/* write register */
	TM_WRITE_REGISTER(TM.Drop.WREDMaxProbModePerColor,TM_Drop_WREDMaxProbModePerColor)
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_q_red_curves(tm_handle hndl,
                        uint8_t curve_ind,
                        const tm_wred_vec curve_green,
                        const tm_wred_vec curve_yellow,
                        const tm_wred_vec curve_red)
{
    int rc = -ERANGE;
    int i;

    TM_REGISTER_VAR(TM_Drop_QueueREDCurve_Color)

    TM_CTL(ctl, hndl)

    TM_RESET_REGISTER(TM.Drop.QueueREDCurve_Color[0], TM_Drop_QueueREDCurve_Color)
    if (rc) goto out;

    for (i = 0; i < 32; i++)
	{
        const int ind = curve_ind*32 + i;

        TM_REGISTER_SET(TM_Drop_QueueREDCurve_Color, Prob, curve_green[i])
        TM_WRITE_TABLE_REGISTER(TM.Drop.QueueREDCurve_Color[0], ind, TM_Drop_QueueREDCurve_Color)
        if (rc)  goto out;

        TM_REGISTER_SET(TM_Drop_QueueREDCurve_Color, Prob, curve_yellow[i])
        TM_WRITE_TABLE_REGISTER(TM.Drop.QueueREDCurve_Color[1], ind, TM_Drop_QueueREDCurve_Color)
        if (rc) goto out;

        TM_REGISTER_SET(TM_Drop_QueueREDCurve_Color, Prob, curve_red[i])
        TM_WRITE_TABLE_REGISTER(TM.Drop.QueueREDCurve_Color[2], ind,TM_Drop_QueueREDCurve_Color);
        if (rc) goto out;
    }

out:
    COMPLETE_HW_WRITE
    return rc;
}

/**
 */
int __set_hw_queues_wred_curve(tm_handle hndl, uint8_t *prob_array, uint8_t curve_ind)
{
    int rc = -ERANGE;
    int i;
    int j;
    int ind;
    TM_REGISTER_VAR(TM_Drop_QueueREDCurve_Color)

    TM_CTL(ctl, hndl)

	for (i=0; i<32; i++)
	{
		/* reset register reserved fields */
		TM_RESET_REGISTER(TM.Drop.QueueREDCurve_Color[0],TM_Drop_QueueREDCurve_Color)
		if (rc)	goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_QueueREDCurve_Color , Prob , prob_array[i])
		ind = curve_ind*32 + i;
		for (j=0; j<3; j++)
		{
			/* write register */
			/* the same curve for each color */
			TM_WRITE_TABLE_REGISTER(TM.Drop.QueueREDCurve_Color[j], ind ,TM_Drop_QueueREDCurve_Color)
			if (rc)	goto out;
		}
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


int set_hw_queues_wred_curve(tm_handle hndl,uint8_t curve_ind)
{
    int rc = -ERANGE;
    uint8_t *prob_array;

    TM_CTL(ctl, hndl)

    if (curve_ind < TM_NUM_WRED_QUEUE_CURVES)
	{
		prob_array = (uint8_t *)(ctl->tm_wred_q_lvl_curves[curve_ind].prob);
		return __set_hw_queues_wred_curve(hndl, prob_array,curve_ind);
	}
    COMPLETE_HW_WRITE;
	return rc;
}

int set_hw_queues_default_wred_curve(tm_handle hndl,uint8_t *prob_array)
{
	return __set_hw_queues_wred_curve(hndl, prob_array,0);
}

/*****************************/

#if 0
/* unused */
/**
 */
int set_hw_a_red_curves(tm_handle hndl,
                        uint8_t curve_ind,
                        const tm_wred_vec curve_green,
                        const tm_wred_vec curve_yellow,
                        const tm_wred_vec curve_red)
{
    int rc = -ERANGE;
    int i;

    TM_REGISTER_VAR(TM_Drop_AlvlREDCurve_Color)

    TM_CTL(ctl, hndl)

    TM_RESET_REGISTER(TM.Drop.AlvlREDCurve_Color[0], TM_Drop_AlvlREDCurve_Color)
    if (rc)  goto out;

    for (i = 0; i < 32; i++)
	{
        const int ind = curve_ind*32 + i;

        TM_REGISTER_SET(TM_Drop_AlvlREDCurve_Color, Prob, curve_green[i])
        TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlREDCurve_Color[0], ind, TM_Drop_AlvlREDCurve_Color)
        if (rc) goto out;

        TM_REGISTER_SET(TM_Drop_AlvlREDCurve_Color, Prob, curve_yellow[i])
        TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlREDCurve_Color[1], ind, TM_Drop_AlvlREDCurve_Color)
        if (rc) goto out;

        TM_REGISTER_SET(TM_Drop_AlvlREDCurve_Color, Prob, curve_red[i])
        TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlREDCurve_Color[2], ind, TM_Drop_AlvlREDCurve_Color)
        if (rc) goto out;
    }

out:
    COMPLETE_HW_WRITE
    return rc;
}
#endif

int __set_hw_a_nodes_wred_curve(tm_handle hndl, uint8_t *prob_array, uint8_t curve_ind)
{
	int rc = -ERANGE;
	int i;
	int j;
	int ind;

    TM_REGISTER_VAR(TM_Drop_AlvlREDCurve_Color)

    TM_CTL(ctl, hndl);

	for (i=0; i<32; i++)
	{
		ind = curve_ind*32 + i;
		for (j=0; j<3; j++)
		{
			/* reset register reserved fields */
			TM_RESET_TABLE_REGISTER(TM.Drop.AlvlREDCurve_Color[j], ind ,TM_Drop_AlvlREDCurve_Color)
			if (rc)	goto out;
			/* assign register fields */		
			TM_REGISTER_SET(TM_Drop_AlvlREDCurve_Color , Prob , prob_array[i])
			/* write register */
			TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlREDCurve_Color[j], ind ,TM_Drop_AlvlREDCurve_Color)
			if (rc)	goto out;
		}
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


int set_hw_a_nodes_wred_curve(tm_handle hndl,  uint8_t curve_ind)
{
	int rc = -ERANGE;
	uint8_t *prob_array;

    TM_CTL(ctl, hndl)

    if (curve_ind < TM_NUM_WRED_A_NODE_CURVES)
	{
		prob_array = (uint8_t *)(ctl->tm_wred_a_lvl_curves[curve_ind].prob);
		return __set_hw_a_nodes_wred_curve(hndl,prob_array,curve_ind);
	}
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_a_nodes_default_wred_curve(tm_handle hndl,uint8_t *prob_array)
{
	return __set_hw_a_nodes_wred_curve(hndl, prob_array,0);
}

/*****************************/


int __set_hw_b_nodes_wred_curve(tm_handle hndl, const uint8_t *prob_array, uint8_t curve_ind)
{
    int rc = -ERANGE;
    int i;
    TM_REGISTER_VAR(TM_Drop_BlvlREDCurve_Table)

    TM_CTL(ctl, hndl)

	for (i=0; i<32; i++)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.BlvlREDCurve_Table[curve_ind], i ,TM_Drop_BlvlREDCurve_Table);
		if (rc)	goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_BlvlREDCurve_Table , Prob , prob_array[i]);
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.BlvlREDCurve_Table[curve_ind], i ,TM_Drop_BlvlREDCurve_Table);
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


int set_hw_b_nodes_wred_curve(tm_handle hndl,   uint8_t curve_ind)
{
    int rc = -ERANGE;
    uint8_t *prob_array;

    TM_CTL(ctl, hndl)

    if (curve_ind < TM_NUM_WRED_B_NODE_CURVES)
	{
		prob_array = (uint8_t *)(ctl->tm_wred_b_lvl_curves[curve_ind].prob);
		return __set_hw_b_nodes_wred_curve(hndl, prob_array,curve_ind);
	}
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_b_nodes_default_wred_curve(tm_handle hndl,uint8_t *prob_array)
{
	return __set_hw_b_nodes_wred_curve(hndl, prob_array,0);
}

/*****************************/

int __set_hw_c_nodes_wred_curve(tm_handle hndl, const uint8_t *prob_array, uint8_t cos, uint8_t curve_ind)
{
    int rc =  -EFAULT;
    int i;
    int ind;
    TM_REGISTER_VAR(TM_Drop_ClvlREDCurve_CoS)

    TM_CTL(ctl, hndl)

	for (i=0; i<32; i++)
	{
		ind = curve_ind*32 + i;
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.ClvlREDCurve_CoS[cos], ind ,TM_Drop_ClvlREDCurve_CoS)
		if (rc)	goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_ClvlREDCurve_CoS , Prob , prob_array[i])
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.ClvlREDCurve_CoS[cos], ind ,TM_Drop_ClvlREDCurve_CoS)
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


int set_hw_c_nodes_wred_curve(tm_handle hndl,  uint8_t cos, uint8_t curve_ind)
{
    uint8_t *prob_array;
    int rc =  -EFAULT;

    TM_CTL(ctl, hndl)

    if (curve_ind < TM_NUM_WRED_C_NODE_CURVES)
	{
		prob_array = (uint8_t *)(ctl->tm_wred_c_lvl_curves[cos][curve_ind].prob);
		return  __set_hw_c_nodes_wred_curve(hndl, prob_array, cos,curve_ind);
	}

    COMPLETE_HW_WRITE
	return rc;
}
int set_hw_c_nodes_default_wred_curve(tm_handle hndl, uint8_t cos, uint8_t *prob_array)
{
	return __set_hw_c_nodes_wred_curve(hndl, prob_array, cos, 0);
}

/*****************************/

int __set_hw_ports_wred_curve(tm_handle hndl, const uint8_t *prob_array)
{
    int rc =  -EFAULT;
    int i;
    TM_REGISTER_VAR(TM_Drop_PortREDCurveG)

    TM_CTL(ctl, hndl)

	for (i=0; i<32; i++)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.PortREDCurveG,i,TM_Drop_PortREDCurveG)
		if (rc)	goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_PortREDCurveG , Prob , prob_array[i])
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.PortREDCurveG, i, TM_Drop_PortREDCurveG)
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_ports_wred_curve(tm_handle hndl, uint8_t curve_ind)
{
    int rc =  -EFAULT;
    uint8_t *prob_array;

    TM_CTL(ctl, hndl)

    if (curve_ind < TM_NUM_WRED_PORT_CURVES)
	{
		prob_array = (uint8_t *)(ctl->tm_wred_ports_curves[curve_ind].prob);
		return __set_hw_ports_wred_curve(hndl,prob_array);
	}
    COMPLETE_HW_WRITE
	return rc;
}
int set_hw_ports_default_wred_curve(tm_handle hndl, uint8_t *prob_array)
{
	return	__set_hw_ports_wred_curve(hndl,prob_array);
}


/*****************************/

int __set_hw_ports_wred_curve_cos(tm_handle hndl, const uint8_t *prob_array,
                                  uint8_t cos)
{
    int rc = 0;
    int i;
    TM_REGISTER_VAR(TM_Drop_PortREDCurve_CoS)

    TM_CTL(ctl, hndl)

	for (i=0; i<32; i++)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.PortREDCurve_CoS[cos],i,TM_Drop_PortREDCurve_CoS)
		if (rc)	goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_PortREDCurve_CoS , Prob , prob_array[i]);
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.PortREDCurve_CoS[cos],i,TM_Drop_PortREDCurve_CoS)
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_ports_wred_curve_cos(tm_handle hndl, uint8_t cos, uint8_t curve_ind)
{
    int rc = -EFAULT;
    uint8_t *prob_array;
    TM_CTL(ctl, hndl);
    if (curve_ind < TM_NUM_WRED_PORT_CURVES)
	{
		prob_array = (uint8_t *)(ctl->tm_wred_ports_curves_cos[cos][curve_ind].prob);
		return __set_hw_ports_wred_curve_cos( hndl, prob_array, cos);
	}
    COMPLETE_HW_WRITE
	return rc;
}
int set_hw_ports_default_wred_curve_cos(tm_handle hndl, uint8_t cos, uint8_t *prob_array)
{
	return __set_hw_ports_wred_curve_cos( hndl, prob_array, cos);
}

int tm_get_hw_wred_curve(tm_handle hndl, enum tm_level level, uint8_t cos, uint8_t curve_ind, uint8_t *prob_array)
{
    int rc =-EFAULT;
    int i;
    int ind;

    TM_REGISTER_VAR(TM_Drop_QueueREDCurve_Color)
    TM_REGISTER_VAR(TM_Drop_AlvlREDCurve_Color)
    TM_REGISTER_VAR(TM_Drop_BlvlREDCurve_Table)
    TM_REGISTER_VAR(TM_Drop_ClvlREDCurve_CoS)
	TM_REGISTER_VAR(TM_Drop_PortREDCurve_CoS)
    TM_REGISTER_VAR(TM_Drop_PortREDCurveG)     

    TM_CTL(ctl, hndl)

    /* validity check */
    switch (level)
    {
    case Q_LEVEL:
        if (curve_ind > TM_NUM_WRED_QUEUE_CURVES)
            goto out; 
        break;		
    case A_LEVEL:
        if (curve_ind > TM_NUM_WRED_A_NODE_CURVES)
            goto out; 
        break;
    case B_LEVEL:
        if (curve_ind > TM_NUM_WRED_B_NODE_CURVES)
            goto out; 
        break;
    case C_LEVEL:
        if (curve_ind > TM_NUM_WRED_C_NODE_CURVES)
            goto out; 

        if (cos > TM_WRED_COS)
            goto out; 
        break;
    case P_LEVEL:
        if (curve_ind > TM_NUM_WRED_PORT_CURVES)
            goto out; 

        if (cos == (uint8_t)TM_INVAL)
        {

        }
        else
        {
            if (cos > TM_WRED_COS)
                goto out; 
        }
    default:
        rc =  -EFAULT;
        goto out;
        break;
    }

    /* since the same curve is configured for each color,
       reading from first color */
               
	for (i = 0; i < 32; i++)
	{
        ind = curve_ind*32 + i;
	
        switch (level)
        {
        case Q_LEVEL:
            TM_READ_TABLE_REGISTER(TM.Drop.QueueREDCurve_Color[0], ind , TM_Drop_QueueREDCurve_Color)
            if (rc)	goto out;
            /* get register value */	
            TM_REGISTER_GET(TM_Drop_QueueREDCurve_Color , Prob , prob_array[i], (uint8_t))
            break;		
        case A_LEVEL:
            TM_READ_TABLE_REGISTER(TM.Drop.AlvlREDCurve_Color[0], ind , TM_Drop_AlvlREDCurve_Color)
            if (rc)	goto out;
            /* get register value */	
            TM_REGISTER_GET(TM_Drop_AlvlREDCurve_Color , Prob , prob_array[i], (uint8_t))
            break;
        case B_LEVEL:
            TM_READ_TABLE_REGISTER(TM.Drop.BlvlREDCurve_Table[0], ind , TM_Drop_BlvlREDCurve_Table)
            if (rc)	goto out;
            /* get register value */	
            TM_REGISTER_GET(TM_Drop_BlvlREDCurve_Table , Prob , prob_array[i], (uint8_t))
            break;
        case C_LEVEL:
            TM_READ_TABLE_REGISTER(TM.Drop.ClvlREDCurve_CoS[cos], ind , TM_Drop_ClvlREDCurve_CoS)
            if (rc)	goto out;
            /* get register value */	
            TM_REGISTER_GET(TM_Drop_ClvlREDCurve_CoS , Prob , prob_array[i], (uint8_t))
            break;
        case P_LEVEL:
            if (cos != (uint8_t)TM_INVAL)
            {
                TM_READ_TABLE_REGISTER(TM.Drop.PortREDCurve_CoS[cos], ind , TM_Drop_PortREDCurve_CoS)
                if (rc)	goto out;
                /* get register value */	
                TM_REGISTER_GET(TM_Drop_PortREDCurve_CoS , Prob , prob_array[i], (uint8_t))
            }
            else
            {
                TM_READ_TABLE_REGISTER(TM.Drop.PortREDCurveG, ind , TM_Drop_PortREDCurveG)
                if (rc)	goto out;
                /* get register value */	
                TM_REGISTER_GET(TM_Drop_PortREDCurveG , Prob , prob_array[i], (uint8_t))
            }
            break;
        default:
            rc =  -EFAULT;
            goto out;
            break;
        }
    }

out:
    return rc;
}


/*****************************/
 
/**
 */
int __set_hw_queue_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint32_t prof_ind)
{
    int rc =  -EFAULT;	

    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDParams)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDScaleRatio)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDMinThresh)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDDPRatio)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfTailDrpThresh)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
	TM_RESET_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDParams , prof_ind , TM_Drop_QueueDropPrfWREDParams)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , AQLExp , profile->aql_exp)
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , ColorTDEn , profile->color_td_en)
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , ScaleExpColor0 , profile->scale_exp[0])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , ScaleExpColor1 , profile->scale_exp[1])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , ScaleExpColor2 , profile->scale_exp[2])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , CurveIndexColor0 , profile->curve_id[0])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , CurveIndexColor1 , profile->curve_id[1])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDParams , CurveIndexColor2 , profile->curve_id[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDParams , prof_ind , TM_Drop_QueueDropPrfWREDParams)
	if (rc)	goto out;
	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDScaleRatio, prof_ind, TM_Drop_QueueDropPrfWREDScaleRatio)
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDScaleRatio , ScaleRatioColor0 , profile->scale_ratio[0])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDScaleRatio , ScaleRatioColor1 , profile->scale_ratio[1])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDScaleRatio , ScaleRatioColor2 , profile->scale_ratio[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDScaleRatio, prof_ind, TM_Drop_QueueDropPrfWREDScaleRatio)
	if (rc)	goto out;
	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDMinThresh, prof_ind,TM_Drop_QueueDropPrfWREDMinThresh)
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDMinThresh , MinTHColor0 , profile->min_threshold[0])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDMinThresh , MinTHColor1 , profile->min_threshold[1])
	TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDMinThresh , MinTHColor2 , profile->min_threshold[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDMinThresh, prof_ind,TM_Drop_QueueDropPrfWREDMinThresh)
	if (rc)	goto out;

    /* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDDPRatio, prof_ind, TM_Drop_QueueDropPrfWREDDPRatio)
    if (rc) goto out;
    /* assign register fields */
    TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDDPRatio, DPRatio0, profile->dp_ratio[0])
    TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDDPRatio, DPRatio1, profile->dp_ratio[1])
    TM_REGISTER_SET(TM_Drop_QueueDropPrfWREDDPRatio, DPRatio2, profile->dp_ratio[2])
    /* write register */
    TM_WRITE_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDDPRatio, prof_ind, TM_Drop_QueueDropPrfWREDDPRatio)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.QueueDropPrfTailDrpThresh, prof_ind, TM_Drop_QueueDropPrfTailDrpThresh)
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_QueueDropPrfTailDrpThresh , TailDropThreshRes , profile->td_thresh_res)
	TM_REGISTER_SET(TM_Drop_QueueDropPrfTailDrpThresh , TailDropThresh , profile->td_threshold_bursts)
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.QueueDropPrfTailDrpThresh, prof_ind, TM_Drop_QueueDropPrfTailDrpThresh)
	if (rc)	goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_queue_drop_profile(tm_handle hndl, uint32_t prof_ind)
{
    int rc =  -EFAULT;
	struct tm_drop_profile *profile;

    TM_CTL(ctl, hndl)

	if (prof_ind < TM_NUM_QUEUE_DROP_PROF)
	{
		profile = &(ctl->tm_q_lvl_drop_profiles[prof_ind]);
		return __set_hw_queue_drop_profile(hndl, profile, prof_ind);
	}
    COMPLETE_HW_WRITE
	return rc;
}
int set_hw_queue_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile)
{
	return __set_hw_queue_drop_profile(hndl, profile, TM_NO_DROP_PROFILE);
}

/**
 */
int __set_hw_a_nodes_drop_profile(tm_handle hndl, struct tm_drop_profile *profile,  uint32_t prof_ind)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDParams)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDScaleRatio)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDMinThresh)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDDPRatio)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfTailDrpThresh)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDParams, prof_ind, TM_Drop_AlvlDropPrfWREDParams)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , AQLExp , profile->aql_exp)
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , ColorTDEn , profile->color_td_en)
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , ScaleExpColor0 , profile->scale_exp[0])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , ScaleExpColor1 , profile->scale_exp[1])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , ScaleExpColor2 , profile->scale_exp[2])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , CurveIndexColor0 , profile->curve_id[0])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , CurveIndexColor1 , profile->curve_id[1])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDParams , CurveIndexColor2 , profile->curve_id[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDParams, prof_ind, TM_Drop_AlvlDropPrfWREDParams)
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDScaleRatio, prof_ind, TM_Drop_AlvlDropPrfWREDScaleRatio)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDScaleRatio , ScaleRatioColor0 , profile->scale_ratio[0])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDScaleRatio , ScaleRatioColor1 , profile->scale_ratio[1])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDScaleRatio , ScaleRatioColor2 , profile->scale_ratio[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDScaleRatio, prof_ind, TM_Drop_AlvlDropPrfWREDScaleRatio)
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDMinThresh, prof_ind, TM_Drop_AlvlDropPrfWREDMinThresh)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDMinThresh , MinTHColor2 , profile->min_threshold[2])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDMinThresh , MinTHColor1 , profile->min_threshold[1])
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDMinThresh , MinTHColor0 , profile->min_threshold[0])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDMinThresh, prof_ind, TM_Drop_AlvlDropPrfWREDMinThresh)
	if (rc)	goto out;

    /* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDDPRatio, prof_ind, TM_Drop_AlvlDropPrfWREDDPRatio)
    if (rc) goto out;
    /* assign register fields */
    TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDDPRatio, DPRatio0, profile->dp_ratio[0])
    TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDDPRatio, DPRatio1, profile->dp_ratio[1])
    TM_REGISTER_SET(TM_Drop_AlvlDropPrfWREDDPRatio, DPRatio2, profile->dp_ratio[2])
    /* write register */
    TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDDPRatio, prof_ind, TM_Drop_AlvlDropPrfWREDDPRatio)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.AlvlDropPrfTailDrpThresh, prof_ind,TM_Drop_AlvlDropPrfTailDrpThresh)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfTailDrpThresh , TailDropThreshRes , profile->td_thresh_res)
	TM_REGISTER_SET(TM_Drop_AlvlDropPrfTailDrpThresh , TailDropThresh , profile->td_threshold_bursts)
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlDropPrfTailDrpThresh, prof_ind,TM_Drop_AlvlDropPrfTailDrpThresh)
	if (rc)	goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_a_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind)
{
    int rc =  -EFAULT;
    struct tm_drop_profile *profile;


    TM_CTL(ctl, hndl)

    if (prof_ind < TM_NUM_A_NODE_DROP_PROF)
	{
		profile = &(ctl->tm_a_lvl_drop_profiles[prof_ind]);
		return __set_hw_a_nodes_drop_profile(hndl,profile,prof_ind);
	}
    COMPLETE_HW_WRITE
	return rc;
}
int set_hw_a_nodes_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile)
{
	return __set_hw_a_nodes_drop_profile(hndl,profile,TM_NO_DROP_PROFILE);
}


/**
 */
int __set_hw_b_nodes_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint32_t prof_ind)
{
    int rc =  -EFAULT;
 
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDParams)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDScaleRatio)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDMinThresh)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDDPRatio)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfTailDrpThresh)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDParams, prof_ind, TM_Drop_BlvlDropPrfWREDParams)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , AQLExp , profile->aql_exp)
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , ColorTDEn , profile->color_td_en)
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , ScaleExpColor0 , profile->scale_exp[0])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , ScaleExpColor1 , profile->scale_exp[1])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , ScaleExpColor2 , profile->scale_exp[2])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , CurveIndexColor0 , profile->curve_id[0])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , CurveIndexColor1 , profile->curve_id[1])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDParams , CurveIndexColor2 , profile->curve_id[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDParams, prof_ind, TM_Drop_BlvlDropPrfWREDParams)
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDScaleRatio, prof_ind,TM_Drop_BlvlDropPrfWREDScaleRatio)
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDScaleRatio , ScaleRatioColor0 , profile->scale_ratio[0])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDScaleRatio , ScaleRatioColor1 , profile->scale_ratio[1])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDScaleRatio , ScaleRatioColor2 , profile->scale_ratio[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDScaleRatio, prof_ind,TM_Drop_BlvlDropPrfWREDScaleRatio)
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDMinThresh, prof_ind, TM_Drop_BlvlDropPrfWREDMinThresh)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDMinThresh , MinTHColor2 , profile->min_threshold[2])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDMinThresh , MinTHColor1 , profile->min_threshold[1])
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDMinThresh , MinTHColor0 , profile->min_threshold[0])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDMinThresh, prof_ind, TM_Drop_BlvlDropPrfWREDMinThresh)
	if (rc)	goto out;

    /* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDDPRatio, prof_ind, TM_Drop_BlvlDropPrfWREDDPRatio)
    if (rc) goto out;
    /* assign register fields */
    TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDDPRatio, DPRatio0, profile->dp_ratio[0])
    TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDDPRatio, DPRatio1, profile->dp_ratio[1])
    TM_REGISTER_SET(TM_Drop_BlvlDropPrfWREDDPRatio, DPRatio2, profile->dp_ratio[2])
    /* write register */
    TM_WRITE_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDDPRatio, prof_ind, TM_Drop_BlvlDropPrfWREDDPRatio)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.BlvlDropPrfTailDrpThresh, prof_ind, TM_Drop_BlvlDropPrfTailDrpThresh)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfTailDrpThresh , TailDropThreshRes , profile->td_thresh_res)
	TM_REGISTER_SET(TM_Drop_BlvlDropPrfTailDrpThresh , TailDropThresh , profile->td_threshold_bursts)
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.BlvlDropPrfTailDrpThresh, prof_ind, TM_Drop_BlvlDropPrfTailDrpThresh)
	if (rc)	goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_b_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind)
{
    int rc =  -EFAULT;
    struct tm_drop_profile *profile;

    TM_CTL(ctl, hndl)

	if (prof_ind < TM_NUM_B_NODE_DROP_PROF)
	{
		profile = &(ctl->tm_b_lvl_drop_profiles[prof_ind]);
		return __set_hw_b_nodes_drop_profile(hndl, profile, prof_ind);
	}
    COMPLETE_HW_WRITE
	return rc;
}
int set_hw_b_nodes_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile)
{
	return __set_hw_b_nodes_drop_profile(hndl, profile, TM_NO_DROP_PROFILE);
}



/**
 */
int __set_hw_c_nodes_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint8_t cos, uint32_t prof_ind)
{
    int rc =  -EFAULT;
    
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDParams_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDMinThresh_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDDPRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfTailDrpThresh_CoS)
	
    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDParams_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDParams_CoS)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , AQLExp , profile->aql_exp)
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , ColorTDEn , profile->color_td_en)
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , ScaleExpColor0 , profile->scale_exp[0])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , ScaleExpColor1 , profile->scale_exp[1])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , ScaleExpColor2 , profile->scale_exp[2])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , CurveIndexColor0 , profile->curve_id[0])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , CurveIndexColor1 , profile->curve_id[1])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDParams_CoS , CurveIndexColor2 , profile->curve_id[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDParams_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDParams_CoS)
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDScaleRatio_CoS[cos], prof_ind,  TM_Drop_ClvlDropPrfWREDScaleRatio_CoS)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS , ScaleRatioColor0 , profile->scale_ratio[0])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS , ScaleRatioColor1 , profile->scale_ratio[1])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS , ScaleRatioColor2 , profile->scale_ratio[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDScaleRatio_CoS[cos], prof_ind,  TM_Drop_ClvlDropPrfWREDScaleRatio_CoS)
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDMinThresh_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDMinThresh_CoS);
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDMinThresh_CoS , MinTHColor0 , profile->min_threshold[0])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDMinThresh_CoS , MinTHColor1 , profile->min_threshold[1])
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDMinThresh_CoS , MinTHColor2 , profile->min_threshold[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDMinThresh_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDMinThresh_CoS)
	if (rc)	goto out;

    /* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDDPRatio_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDDPRatio_CoS)
    if (rc) goto out;
    /* assign register fields */
    TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDDPRatio_CoS, DPRatio0, profile->dp_ratio[0])
    TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDDPRatio_CoS, DPRatio1, profile->dp_ratio[1])
    TM_REGISTER_SET(TM_Drop_ClvlDropPrfWREDDPRatio_CoS, DPRatio2, profile->dp_ratio[2])
    /* write register */
    TM_WRITE_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDDPRatio_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDDPRatio_CoS)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.ClvlDropPrfTailDrpThresh_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfTailDrpThresh_CoS)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfTailDrpThresh_CoS , TailDropThreshRes , profile->td_thresh_res)
	TM_REGISTER_SET(TM_Drop_ClvlDropPrfTailDrpThresh_CoS , TailDropThresh , profile->td_threshold_bursts)
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.ClvlDropPrfTailDrpThresh_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfTailDrpThresh_CoS)
	if (rc)	goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_c_nodes_drop_profile(tm_handle hndl,  uint8_t cos, uint32_t prof_ind)
{
    int rc =  -EFAULT;
    struct tm_drop_profile *profile;
	
    TM_CTL(ctl, hndl)

    if (prof_ind < TM_NUM_C_NODE_DROP_PROF)
	{
		profile = &(ctl->tm_c_lvl_drop_profiles[cos][prof_ind]);
		return __set_hw_c_nodes_drop_profile(hndl, profile, cos, prof_ind);
	}
    COMPLETE_HW_WRITE
	return rc;
}
int set_hw_c_nodes_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint8_t cos)
{
	return __set_hw_c_nodes_drop_profile(hndl, profile, cos, TM_NO_DROP_PROFILE);
}

/**
 */
int __set_hw_ports_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint32_t prof_ind, uint8_t port_ind)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDParamsG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDScaleRatioG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDMinThreshG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDDPRatioG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfTailDrpThreshG)

    TM_CTL(ctl, hndl)

	profile = &(ctl->tm_p_lvl_drop_profiles[prof_ind]);
	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDParamsG, port_ind,TM_Drop_PortDropPrfWREDParamsG)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , AQLExp , profile->aql_exp)
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , ColorTDEn , profile->color_td_en)
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , ScaleExpColor0 , profile->scale_exp[0])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , ScaleExpColor1 , profile->scale_exp[1])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , ScaleExpColor2 , profile->scale_exp[2])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , CurveIndexColor0 , profile->curve_id[0])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , CurveIndexColor1 , profile->curve_id[1])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParamsG , CurveIndexColor2 , profile->curve_id[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDParamsG, port_ind,TM_Drop_PortDropPrfWREDParamsG)
	if (rc)	goto out;
	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDScaleRatioG, port_ind, TM_Drop_PortDropPrfWREDScaleRatioG)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDScaleRatioG , ScaleRatioColor0 , profile->scale_ratio[0])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDScaleRatioG , ScaleRatioColor1 , profile->scale_ratio[1])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDScaleRatioG , ScaleRatioColor2 , profile->scale_ratio[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDScaleRatioG, port_ind, TM_Drop_PortDropPrfWREDScaleRatioG)
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDMinThreshG, port_ind, TM_Drop_PortDropPrfWREDMinThreshG)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDMinThreshG , MinTHColor0 , profile->min_threshold[0])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDMinThreshG , MinTHColor1 , profile->min_threshold[1])
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDMinThreshG , MinTHColor2 , profile->min_threshold[2])
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDMinThreshG, port_ind, TM_Drop_PortDropPrfWREDMinThreshG)
	if (rc)	goto out;

    /* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDDPRatioG, port_ind, TM_Drop_PortDropPrfWREDDPRatioG)
    if (rc) goto out;
    /* assign register fields */
    TM_REGISTER_SET(TM_Drop_PortDropPrfWREDDPRatioG, DPRatio0, profile->dp_ratio[0])
    TM_REGISTER_SET(TM_Drop_PortDropPrfWREDDPRatioG, DPRatio1, profile->dp_ratio[1])
    TM_REGISTER_SET(TM_Drop_PortDropPrfWREDDPRatioG, DPRatio2, profile->dp_ratio[2])
    /* write register */
    TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDDPRatioG, port_ind, TM_Drop_PortDropPrfWREDDPRatioG)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfTailDrpThreshG, port_ind,  TM_Drop_PortDropPrfTailDrpThreshG)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfTailDrpThreshG , TailDropThreshRes , profile->td_thresh_res)
	TM_REGISTER_SET(TM_Drop_PortDropPrfTailDrpThreshG , TailDropThresh , profile->td_threshold_bursts)
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfTailDrpThreshG, port_ind,  TM_Drop_PortDropPrfTailDrpThreshG)
	if (rc)	goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_ports_drop_profile(tm_handle hndl,  uint32_t prof_ind, uint8_t port_ind)
{
    int rc =  -EFAULT;
    struct tm_drop_profile *profile;

    TM_CTL(ctl, hndl)

	profile = &(ctl->tm_p_lvl_drop_profiles[prof_ind]);
	rc  = __set_hw_ports_drop_profile(hndl, profile, prof_ind,port_ind);

    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_ports_default_drop_profile(tm_handle hndl, struct tm_drop_profile *profile, uint8_t port_ind)
{
	return __set_hw_ports_drop_profile(hndl, profile, TM_NO_DROP_PROFILE,port_ind);
}
/**
 */


int __set_hw_ports_drop_profile_cos(tm_handle hndl, struct tm_drop_profile *profile, uint8_t cos, uint32_t port_ind)
{
    int rc = 0;
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDParams_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDScaleRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDMinThresh_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDDPRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfTailDrpThresh_CoS)


    TM_CTL(ctl, hndl);
	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDParams_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDParams_CoS)
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , AQLExp ,			profile->aql_exp);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , ColorTDEn ,			profile->color_td_en);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , ScaleExpColor2 ,	profile->scale_exp[2]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , ScaleExpColor1 ,	profile->scale_exp[1]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , ScaleExpColor0 ,	profile->scale_exp[0]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , CurveIndexColor0 ,	profile->curve_id[0]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , CurveIndexColor1 ,	profile->curve_id[1]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDParams_CoS , CurveIndexColor0 ,	profile->curve_id[2]);
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDParams_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDParams_CoS);
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDScaleRatio_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDScaleRatio_CoS);
	if (rc)	goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDScaleRatio_CoS , ScaleRatioColor0 ,			profile->scale_ratio[0]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDScaleRatio_CoS , ScaleRatioColor1 ,			profile->scale_ratio[1]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDScaleRatio_CoS , ScaleRatioColor2 ,			profile->scale_ratio[2]);
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDScaleRatio_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDScaleRatio_CoS);
	/* write register */
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDMinThresh_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDMinThresh_CoS);
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDMinThresh_CoS , MinTHColor0 ,			profile->min_threshold[0]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDMinThresh_CoS , MinTHColor1 ,			profile->min_threshold[1]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDMinThresh_CoS , MinTHColor2 ,			profile->min_threshold[2]);
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDMinThresh_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDMinThresh_CoS);
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfWREDDPRatio_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDDPRatio_CoS);
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDDPRatio_CoS , DPRatio0 ,			profile->dp_ratio[0]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDDPRatio_CoS , DPRatio1 ,			profile->dp_ratio[1]);
	TM_REGISTER_SET(TM_Drop_PortDropPrfWREDDPRatio_CoS , DPRatio2 ,			profile->dp_ratio[2]);
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfWREDDPRatio_CoS[cos] , port_ind , TM_Drop_PortDropPrfWREDDPRatio_CoS);
	if (rc)	goto out;

	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Drop.PortDropPrfTailDrpThresh_CoS[cos] , port_ind , TM_Drop_PortDropPrfTailDrpThresh_CoS);
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_PortDropPrfTailDrpThresh_CoS , TailDropThresh ,			profile->td_threshold_bursts);
	TM_REGISTER_SET(TM_Drop_PortDropPrfTailDrpThresh_CoS , TailDropThreshRes ,		profile->td_thresh_res);
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Drop.PortDropPrfTailDrpThresh_CoS[cos] , port_ind , TM_Drop_PortDropPrfTailDrpThresh_CoS);
	if (rc)	goto out;
out:
    COMPLETE_HW_WRITE;
	return rc;
}

int set_hw_ports_drop_profile_cos(tm_handle hndl, uint8_t cos, uint32_t prof_ind, uint8_t port_ind)
{
    int rc =  -EFAULT;
    struct tm_drop_profile *profile;

    TM_CTL(ctl, hndl)

	profile = &(ctl->tm_p_lvl_drop_profiles_cos[cos][prof_ind]);
	rc = __set_hw_ports_drop_profile_cos(hndl, profile, cos, port_ind);

    COMPLETE_HW_WRITE;
	return rc;
}

int set_hw_ports_default_drop_profile_cos(tm_handle hndl,  struct tm_drop_profile *profile, uint8_t cos, uint8_t port_ind)
{
	return __set_hw_ports_drop_profile_cos(hndl, profile, cos, port_ind);
}



int set_hw_aging_status(tm_handle hndl)
{
    int rc = 0;
    TM_REGISTER_VAR(TM_Drop_AgingUpdEnable)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.AgingUpdEnable , TM_Drop_AgingUpdEnable)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_AgingUpdEnable , En , ctl->aging_status)
	/* write register */
	TM_WRITE_REGISTER(TM.Drop.AgingUpdEnable , TM_Drop_AgingUpdEnable)
	if (rc)	goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int get_hw_aging_status(tm_handle hndl)
{
    int rc;
    TM_REGISTER_VAR(TM_Drop_AgingUpdEnable)

    TM_CTL(ctl, hndl)

	TM_READ_REGISTER(TM.Drop.AgingUpdEnable , TM_Drop_AgingUpdEnable)
    if (rc) goto out;

	TM_REGISTER_GET(TM_Drop_AgingUpdEnable, En , ctl->aging_status , (uint8_t) )  /* casting to uint8_t */
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_tm2tm_channels(tm_handle hndl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_TM2TM_TMtoTMElementsPerEgTMLevel )
    TM_REGISTER_VAR(TM_TM2TM_TMtoTMEgTMLevelMap)
    TM_REGISTER_VAR(TM_TM2TM_TM2TMGenModePerLevel)
    TM_REGISTER_VAR(TM_TM2TM_TMtoTMEgTMBPLevelMap)
    TM_REGISTER_VAR(TM_TM2TM_TMtoTMBaseOffsetPerIngTMLevel)
    TM_REGISTER_VAR(TM_TM2TM_TM2TMXonXoffThresh)

    TM_CTL(ctl, hndl)

 	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMtoTMElementsPerEgTMLevel, TM_TM2TM_TMtoTMElementsPerEgTMLevel)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TMtoTMElementsPerEgTMLevel, Ports , ctl->port_ch.egr_elems) /* 6 bits */
    TM_REGISTER_SET(TM_TM2TM_TMtoTMElementsPerEgTMLevel, Nodes , ctl->node_ch.egr_elems) /* 16 bits */
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.TMtoTMElementsPerEgTMLevel, TM_TM2TM_TMtoTMElementsPerEgTMLevel)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMtoTMEgTMLevelMap, TM_TM2TM_TMtoTMEgTMLevelMap)
	if (rc) goto out;
	/* assign register fields */		
	/* write register */
    TM_REGISTER_SET(TM_TM2TM_TMtoTMEgTMLevelMap , Port , ctl->port_ch.src_lvl) /* 3 bits */
    TM_REGISTER_SET(TM_TM2TM_TMtoTMEgTMLevelMap , Node , ctl->node_ch.src_lvl) /* 3 bits */
    TM_WRITE_REGISTER(TM.TM2TM.TMtoTMEgTMLevelMap, TM_TM2TM_TMtoTMEgTMLevelMap)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TM2TMGenModePerLevel, TM_TM2TM_TM2TMGenModePerLevel)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TM2TMGenModePerLevel , PortMode , ctl->port_ch.mode) /* 1 bit */
    TM_REGISTER_SET(TM_TM2TM_TM2TMGenModePerLevel , NodeMode , ctl->node_ch.mode) /* 1 bit */
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.TM2TMGenModePerLevel, TM_TM2TM_TM2TMGenModePerLevel)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMtoTMEgTMBPLevelMap, TM_TM2TM_TMtoTMEgTMBPLevelMap)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TMtoTMEgTMBPLevelMap , Port , ctl->port_ch.bp_map) /* 1 bit */
    TM_REGISTER_SET(TM_TM2TM_TMtoTMEgTMBPLevelMap , Node , ctl->node_ch.bp_map) /* 1 bit */
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.TMtoTMEgTMBPLevelMap, TM_TM2TM_TMtoTMEgTMBPLevelMap)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMtoTMBaseOffsetPerIngTMLevel, TM_TM2TM_TMtoTMBaseOffsetPerIngTMLevel)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TMtoTMBaseOffsetPerIngTMLevel , PortBA , ctl->port_ch.bp_offset) /* 16 bits */
    TM_REGISTER_SET(TM_TM2TM_TMtoTMBaseOffsetPerIngTMLevel , NodeBA , ctl->node_ch.bp_offset) /* 16 bits */
    TM_WRITE_REGISTER(TM.TM2TM.TMtoTMBaseOffsetPerIngTMLevel, TM_TM2TM_TMtoTMBaseOffsetPerIngTMLevel)
	/* write register */
    if (rc) goto out;

    /* resolution mode must be checked */

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TM2TMXonXoffThresh, TM_TM2TM_TM2TMXonXoffThresh)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_TM2TM_TM2TMXonXoffThresh , PortXon , ctl->port_ch.bp_xon)   /* 6 bits */
    TM_REGISTER_SET(TM_TM2TM_TM2TMXonXoffThresh , PortXoff , ctl->port_ch.bp_xoff) /* 6 bits */
    TM_REGISTER_SET(TM_TM2TM_TM2TMXonXoffThresh , NodeXon , ctl->node_ch.bp_xon)   /* 6 bits */
    TM_REGISTER_SET(TM_TM2TM_TM2TMXonXoffThresh , NodeXoff , ctl->node_ch.bp_xoff) /* 6 bits */
    TM_WRITE_REGISTER(TM.TM2TM.TM2TMXonXoffThresh, TM_TM2TM_TM2TMXonXoffThresh)
	/* write register */
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

/**
 */
int set_hw_drop_aqm_mode(tm_handle hndl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_WREDDropProbMode)
    TM_REGISTER_VAR(TM_Drop_DPSource)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.WREDDropProbMode, TM_Drop_WREDDropProbMode)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Port , 1)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Clvl , 1)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Blvl , 1)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Alvl , 1)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Queue , 1)

    if ((ctl->dp_unit.local[Q_LEVEL].color_num == TM_1_COLORS) ||
        (ctl->dp_unit.local[Q_LEVEL].color_num == TM_2_COLORS))
	{
        TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Queue , 0)
	}

    if ((ctl->dp_unit.local[A_LEVEL].color_num == TM_1_COLORS) ||
        (ctl->dp_unit.local[A_LEVEL].color_num == TM_2_COLORS))
	{
        TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Alvl , 0)
	}
    if ((ctl->dp_unit.local[B_LEVEL].color_num == TM_1_COLORS) ||
        (ctl->dp_unit.local[B_LEVEL].color_num == TM_2_COLORS))
	{
        TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Blvl , 0)
	}
    if ((ctl->dp_unit.local[C_LEVEL].color_num == TM_1_COLORS) ||
        (ctl->dp_unit.local[C_LEVEL].color_num == TM_2_COLORS))
	{
        TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Clvl , 0)
	}

    if ((ctl->dp_unit.local[P_LEVEL].color_num == TM_1_COLORS) ||
        (ctl->dp_unit.local[P_LEVEL].color_num == TM_2_COLORS))
	{
        TM_REGISTER_SET(TM_Drop_WREDDropProbMode, Port , 0)
	}
	/* write register */
    TM_WRITE_REGISTER(TM.Drop.WREDDropProbMode, TM_Drop_WREDDropProbMode)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.DPSource, TM_Drop_DPSource);
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Drop_DPSource , PortSrc , ctl->dp_unit.local[P_LEVEL].dp_src[0] + ctl->dp_unit.local[P_LEVEL].dp_src[1]*2 + ctl->dp_unit.local[P_LEVEL].dp_src[2]*4);
    TM_REGISTER_SET(TM_Drop_DPSource , ClvlSrc , ctl->dp_unit.local[C_LEVEL].dp_src[0]);
    TM_REGISTER_SET(TM_Drop_DPSource , BlvlSrc , ctl->dp_unit.local[B_LEVEL].dp_src[0]);
    TM_REGISTER_SET(TM_Drop_DPSource , AlvlSrc , ctl->dp_unit.local[A_LEVEL].dp_src[0]);
    TM_REGISTER_SET(TM_Drop_DPSource , QueueSrc , ctl->dp_unit.local[Q_LEVEL].dp_src[0]);
	/* write register */
    TM_WRITE_REGISTER(TM.Drop.DPSource, TM_Drop_DPSource)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_drop_color_num(tm_handle hndl)
{
    int rc =  -EFAULT;
    TM_REGISTER_VAR(TM_Drop_WREDDropProbMode)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.WREDDropProbMode, TM_Drop_WREDDropProbMode)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Port , 0)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Clvl , 0)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Blvl , 0)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Alvl , 0)
    TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Queue , 0)

    if (ctl->dp_unit.local[Q_LEVEL].color_num > 2)	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Queue, 1 )
    if (ctl->dp_unit.local[A_LEVEL].color_num > 2)	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Alvl , 1 )
    if (ctl->dp_unit.local[B_LEVEL].color_num > 2)	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Blvl , 1 )
    if (ctl->dp_unit.local[C_LEVEL].color_num > 2)	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Clvl , 1 )
    if (ctl->dp_unit.local[P_LEVEL].color_num > 2)	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Port , 1 )
	/* write register */
    TM_WRITE_REGISTER(TM.Drop.WREDDropProbMode, TM_Drop_WREDDropProbMode)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_tm2tm_aqm_mode(tm_handle hndl)
{
    int rc =  -EFAULT;

	uint8_t port_ch_color_num;
    uint8_t node_ch_color_num;
    enum tm_level src_lvl;
    int remote_mode;
    int q_mode;
    int a_mode;
    int b_mode;
    int c_mode;
    int p_mode;

    TM_REGISTER_VAR(TM_Drop_WREDDropProbMode)
    TM_REGISTER_VAR(TM_TM2TM_RemoteDPMode)
    TM_REGISTER_VAR(TM_Drop_DPSource)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.RemoteDPMode, TM_TM2TM_RemoteDPMode)
	if (rc) goto out;
	/* assign register fields */		
	port_ch_color_num = ctl->dp_unit.remote[TM2TM_PORT_CH].color_num;
    if (port_ch_color_num == 2)
	{ /* mode 1 */
       TM_REGISTER_SET(TM_TM2TM_RemoteDPMode , PortMode , 0)
    }
    else
	{ /* one color in remote */
        src_lvl = ctl->port_ch.src_lvl;
        if (ctl->dp_unit.local[src_lvl].color_num == 2)
            remote_mode = 1; /* mode 0 */
        else /* one color in local */
            remote_mode = 2; /* mode 2 */
        TM_REGISTER_SET(TM_TM2TM_RemoteDPMode , PortMode , remote_mode);
    }

    node_ch_color_num = ctl->dp_unit.remote[TM2TM_NODE_CH].color_num;
    if (node_ch_color_num == 2)
	{ /* mode 1 */
       TM_REGISTER_SET(TM_TM2TM_RemoteDPMode , NodeMode , 0);
    }
    else
	{ /* one color in remote */
        src_lvl = ctl->node_ch.src_lvl;
        if (ctl->dp_unit.local[src_lvl].color_num == 2)
            remote_mode = 1; /* mode 0 */
        else /* one color in local */
            remote_mode = 2; /* mode 2 */
		TM_REGISTER_SET(TM_TM2TM_RemoteDPMode , NodeMode , remote_mode)
    }
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.RemoteDPMode, TM_TM2TM_RemoteDPMode)
    if (rc) goto out;

    if (ctl->dp_unit.local[P_LEVEL].color_num < 3)
        p_mode = 0;
    else
        p_mode = 1;
    if (ctl->dp_unit.local[C_LEVEL].color_num < 3)
        c_mode = 0;
    else
        c_mode = 1;
    if (ctl->dp_unit.local[B_LEVEL].color_num < 3)
        b_mode = 0;
    else
        b_mode = 1;
    if (ctl->dp_unit.local[A_LEVEL].color_num < 3)
        a_mode = 0;
    else
        a_mode = 1;
    if (ctl->dp_unit.local[Q_LEVEL].color_num < 3)
        q_mode = 0;
    else
        q_mode = 1;

    if (ctl->dp_unit.remote[TM2TM_PORT_CH].configured == TM_ENABLE)
	{
        if (port_ch_color_num == 1)
		{
            switch (ctl->port_ch.src_lvl) {
            case A_LEVEL:
                if (ctl->dp_unit.local[A_LEVEL].color_num == 1)
                    a_mode = 0;
                break;
            case B_LEVEL:
                if (ctl->dp_unit.local[B_LEVEL].color_num == 1)
                    b_mode = 0;
                break;
            case C_LEVEL:
                if (ctl->dp_unit.local[C_LEVEL].color_num == 1)
                    c_mode = 0;
                break;
            case P_LEVEL:
                if (ctl->dp_unit.local[P_LEVEL].color_num == 1)
                    p_mode = 0;
                break;
            }
        }
    }

    if (ctl->dp_unit.remote[TM2TM_NODE_CH].configured == TM_ENABLE)
	{
        if (node_ch_color_num == 1)
		{
            switch (ctl->node_ch.src_lvl) {
            case Q_LEVEL:
                if (ctl->dp_unit.local[Q_LEVEL].color_num == 1)
                    q_mode = 0;
                break;
            case A_LEVEL:
                if (ctl->dp_unit.local[A_LEVEL].color_num == 1)
                    a_mode = 0;
                break;
            case B_LEVEL:
                if (ctl->dp_unit.local[B_LEVEL].color_num == 1)
                    b_mode = 0;
                break;
            case C_LEVEL:
                if (ctl->dp_unit.local[C_LEVEL].color_num == 1)
                    c_mode = 0;
                break;
            }
        }
    }

    if ((ctl->dp_unit.remote[TM2TM_PORT_CH].configured == TM_ENABLE) &&
        (ctl->dp_unit.remote[TM2TM_NODE_CH].configured == TM_ENABLE))
	{
        /* There is a low chance that both Port & Node channels are 1 color */
        if (ctl->node_ch.src_lvl == ctl->port_ch.src_lvl)
		{
            switch (ctl->node_ch.src_lvl)
			{
            case A_LEVEL:
                a_mode = 1;
                break;
            case B_LEVEL:
                b_mode = 1;
                break;
            case C_LEVEL:
                c_mode = 1;
                break;
            case P_LEVEL:
                p_mode = 1;
                break;
            }
		}
    }


    /* Case that remote chennels are not configured at all */
    if ((ctl->dp_unit.remote[TM2TM_PORT_CH].configured != TM_ENABLE) &&
        (ctl->dp_unit.remote[TM2TM_NODE_CH].configured != TM_ENABLE))
	{
        if (ctl->dp_unit.local[P_LEVEL].color_num < 3)
            p_mode = 0;
        if (ctl->dp_unit.local[C_LEVEL].color_num < 3)
            c_mode = 0;
        if (ctl->dp_unit.local[B_LEVEL].color_num < 3)
            b_mode = 0;
        if (ctl->dp_unit.local[A_LEVEL].color_num < 3)
            a_mode = 0;
        if (ctl->dp_unit.local[Q_LEVEL].color_num < 3)
            q_mode = 0;
    }

	

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.WREDDropProbMode, TM_Drop_WREDDropProbMode)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Port , p_mode)
	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Clvl , c_mode)
	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Blvl , b_mode)
	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Alvl , a_mode)
	TM_REGISTER_SET(TM_Drop_WREDDropProbMode , Queue, q_mode)
	/* write register */
    TM_WRITE_REGISTER(TM.Drop.WREDDropProbMode, TM_Drop_WREDDropProbMode)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.DPSource, TM_Drop_DPSource)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Drop_DPSource , PortSrc , ctl->dp_unit.local[P_LEVEL].dp_src[0] + ctl->dp_unit.local[P_LEVEL].dp_src[1]*2 + ctl->dp_unit.local[P_LEVEL].dp_src[2]*4 )
    TM_REGISTER_SET(TM_Drop_DPSource , ClvlSrc , ctl->dp_unit.local[C_LEVEL].dp_src[0])
    TM_REGISTER_SET(TM_Drop_DPSource , BlvlSrc , ctl->dp_unit.local[B_LEVEL].dp_src[0])
    TM_REGISTER_SET(TM_Drop_DPSource , AlvlSrc , ctl->dp_unit.local[A_LEVEL].dp_src[0])
    TM_REGISTER_SET(TM_Drop_DPSource , QueueSrc, ctl->dp_unit.local[Q_LEVEL].dp_src[0])
	/* write register */
	TM_WRITE_REGISTER(TM.Drop.DPSource, TM_Drop_DPSource)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_periodic_scheme(tm_handle hndl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_PortPerRateShpPrms)
    TM_REGISTER_VAR(TM_Sched_ClvlPerRateShpPrms)
    TM_REGISTER_VAR(TM_Sched_BlvlPerRateShpPrms)
    TM_REGISTER_VAR(TM_Sched_AlvlPerRateShpPrms)
    TM_REGISTER_VAR(TM_Sched_QueuePerRateShpPrms)
    TM_REGISTER_VAR(TM_Sched_ScrubSlotAlloc)

    TM_CTL(ctl, hndl)

	rc=set_hw_shaping_status(hndl,P_LEVEL);
    if (rc)
        goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.PortPerRateShpPrms, TM_Sched_PortPerRateShpPrms)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Sched_PortPerRateShpPrms , L , ctl->level_data[P_LEVEL].L)
    TM_REGISTER_SET(TM_Sched_PortPerRateShpPrms , K , ctl->level_data[P_LEVEL].K)
    TM_REGISTER_SET(TM_Sched_PortPerRateShpPrms , N , ctl->level_data[P_LEVEL].N)
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.PortPerRateShpPrms, TM_Sched_PortPerRateShpPrms)
    if (rc) goto out;

	rc=set_hw_shaping_status(hndl,C_LEVEL);
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.ClvlPerRateShpPrms, TM_Sched_ClvlPerRateShpPrms)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Sched_ClvlPerRateShpPrms , L , ctl->level_data[C_LEVEL].L)
    TM_REGISTER_SET(TM_Sched_ClvlPerRateShpPrms , K , ctl->level_data[C_LEVEL].K)
    TM_REGISTER_SET(TM_Sched_ClvlPerRateShpPrms , N , ctl->level_data[C_LEVEL].N)
	/* write register */
	TM_WRITE_REGISTER(TM.Sched.ClvlPerRateShpPrms, TM_Sched_ClvlPerRateShpPrms)
    if (rc) goto out;

	rc=set_hw_shaping_status(hndl,B_LEVEL);
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.BlvlPerRateShpPrms, TM_Sched_BlvlPerRateShpPrms)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Sched_BlvlPerRateShpPrms , L , ctl->level_data[B_LEVEL].L)
    TM_REGISTER_SET(TM_Sched_BlvlPerRateShpPrms , K , ctl->level_data[B_LEVEL].K)
    TM_REGISTER_SET(TM_Sched_BlvlPerRateShpPrms , N , ctl->level_data[B_LEVEL].N)
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.BlvlPerRateShpPrms, TM_Sched_BlvlPerRateShpPrms)
    if (rc) goto out;

	rc=set_hw_shaping_status(hndl,A_LEVEL);
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.AlvlPerRateShpPrms, TM_Sched_AlvlPerRateShpPrms)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Sched_AlvlPerRateShpPrms , L , ctl->level_data[A_LEVEL].L)
    TM_REGISTER_SET(TM_Sched_AlvlPerRateShpPrms , K , ctl->level_data[A_LEVEL].K)
    TM_REGISTER_SET(TM_Sched_AlvlPerRateShpPrms , N , ctl->level_data[A_LEVEL].N)
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.AlvlPerRateShpPrms, TM_Sched_AlvlPerRateShpPrms)
    if (rc) goto out;

	rc=set_hw_shaping_status(hndl,Q_LEVEL);
    if (rc) goto out;
 
	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.QueuePerRateShpPrms, TM_Sched_QueuePerRateShpPrms)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Sched_QueuePerRateShpPrms , L , ctl->level_data[Q_LEVEL].L)
    TM_REGISTER_SET(TM_Sched_QueuePerRateShpPrms , K , ctl->level_data[Q_LEVEL].K)
    TM_REGISTER_SET(TM_Sched_QueuePerRateShpPrms , N , ctl->level_data[Q_LEVEL].N)
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.QueuePerRateShpPrms, TM_Sched_QueuePerRateShpPrms)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.ScrubSlotAlloc, TM_Sched_ScrubSlotAlloc)
	if (rc) goto out;
	/* assign register fields */		
	/* write register */
	TM_REGISTER_SET(TM_Sched_ScrubSlotAlloc , PortSlots , ctl->level_data[P_LEVEL].scrub_slots)
    TM_REGISTER_SET(TM_Sched_ScrubSlotAlloc , ClvlSlots , ctl->level_data[C_LEVEL].scrub_slots)
    TM_REGISTER_SET(TM_Sched_ScrubSlotAlloc , BlvlSlots , ctl->level_data[B_LEVEL].scrub_slots)
    TM_REGISTER_SET(TM_Sched_ScrubSlotAlloc , AlvlSlots , ctl->level_data[A_LEVEL].scrub_slots)
    TM_REGISTER_SET(TM_Sched_ScrubSlotAlloc , QueueSlots ,ctl->level_data[Q_LEVEL].scrub_slots)
    TM_WRITE_REGISTER(TM.Sched.ScrubSlotAlloc, TM_Sched_ScrubSlotAlloc)
    if (rc) goto out;

out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_node_mapping(tm_handle hndl, enum tm_level lvl, uint32_t index)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_PortRangeMap)
    TM_REGISTER_VAR(TM_Sched_ClvltoPortAndBlvlRangeMap)
    TM_REGISTER_VAR(TM_Sched_BLvltoClvlAndAlvlRangeMap)
    TM_REGISTER_VAR(TM_Sched_ALvltoBlvlAndQueueRangeMap)
    TM_REGISTER_VAR(TM_Sched_QueueAMap)

    TM_CTL(ctl, hndl)

	switch (lvl)
	{
    case P_LEVEL:
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortRangeMap , index , TM_Sched_PortRangeMap)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortRangeMap , Lo , ctl->tm_port_array[index].mapping.childLo)
		TM_REGISTER_SET(TM_Sched_PortRangeMap , Hi , ctl->tm_port_array[index].mapping.childHi)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortRangeMap , index , TM_Sched_PortRangeMap)
		break;
    case C_LEVEL:
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.ClvltoPortAndBlvlRangeMap , index , TM_Sched_ClvltoPortAndBlvlRangeMap)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_ClvltoPortAndBlvlRangeMap , Port , ctl->tm_c_node_array[index].mapping.nodeParent)
		TM_REGISTER_SET(TM_Sched_ClvltoPortAndBlvlRangeMap , BlvlLo , ctl->tm_c_node_array[index].mapping.childLo)
		TM_REGISTER_SET(TM_Sched_ClvltoPortAndBlvlRangeMap , BlvlHi , ctl->tm_c_node_array[index].mapping.childHi)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.ClvltoPortAndBlvlRangeMap , index , TM_Sched_ClvltoPortAndBlvlRangeMap)
		break;
    case B_LEVEL:
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.BLvltoClvlAndAlvlRangeMap , index , TM_Sched_BLvltoClvlAndAlvlRangeMap)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_BLvltoClvlAndAlvlRangeMap , Clvl , ctl->tm_b_node_array[index].mapping.nodeParent)
		TM_REGISTER_SET(TM_Sched_BLvltoClvlAndAlvlRangeMap , AlvlLo , ctl->tm_b_node_array[index].mapping.childLo)
		TM_REGISTER_SET(TM_Sched_BLvltoClvlAndAlvlRangeMap , AlvlHi , ctl->tm_b_node_array[index].mapping.childHi)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.BLvltoClvlAndAlvlRangeMap , index , TM_Sched_BLvltoClvlAndAlvlRangeMap)
		break;
    case A_LEVEL:
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.ALvltoBlvlAndQueueRangeMap , index , TM_Sched_ALvltoBlvlAndQueueRangeMap)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_ALvltoBlvlAndQueueRangeMap , Blvl , ctl->tm_a_node_array[index].mapping.nodeParent)
		TM_REGISTER_SET(TM_Sched_ALvltoBlvlAndQueueRangeMap , QueueLo , ctl->tm_a_node_array[index].mapping.childLo)
		TM_REGISTER_SET(TM_Sched_ALvltoBlvlAndQueueRangeMap , QueueHi , ctl->tm_a_node_array[index].mapping.childHi)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.ALvltoBlvlAndQueueRangeMap , index , TM_Sched_ALvltoBlvlAndQueueRangeMap)
		break;
    case Q_LEVEL:
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.QueueAMap , index , TM_Sched_QueueAMap)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_QueueAMap , Alvl , ctl->tm_queue_array[index].mapping.nodeParent)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.QueueAMap , index , TM_Sched_QueueAMap)
		break;
    }
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
 
int set_hw_queue_drop_profile_ptr(tm_handle hndl, uint32_t queue_ind)
{	
    int rc =  -EFAULT;

	int entry;
    int base_ind;
    TM_REGISTER_VAR(TM_Drop_QueueDropProfPtr)

    TM_CTL(ctl, hndl)
	if (queue_ind < ctl->tm_total_queues)
	{
		
		/* Set drop profile pointer entry with data exisitng in SW image
		 * to avoid read-modify-write from HW */
		/* Entry in the table */
		entry = queue_ind/TM_Q_DRP_PROF_PER_ENTRY;
		base_ind = entry*TM_Q_DRP_PROF_PER_ENTRY;
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.QueueDropProfPtr , entry , TM_Drop_QueueDropProfPtr)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_QueueDropProfPtr , ProfPtr0 , ctl->tm_q_lvl_drop_prof_ptr[base_ind+0])
		TM_REGISTER_SET(TM_Drop_QueueDropProfPtr , ProfPtr1 , ctl->tm_q_lvl_drop_prof_ptr[base_ind+1])
		TM_REGISTER_SET(TM_Drop_QueueDropProfPtr , ProfPtr2 , ctl->tm_q_lvl_drop_prof_ptr[base_ind+2])
		TM_REGISTER_SET(TM_Drop_QueueDropProfPtr , ProfPtr3 , ctl->tm_q_lvl_drop_prof_ptr[base_ind+3])
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.QueueDropProfPtr , entry , TM_Drop_QueueDropProfPtr)
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_queue_params(tm_handle hndl, uint32_t queue_ind)
{	
    int rc =  -EFAULT;

    struct tm_queue *queue = NULL;

   TM_REGISTER_VAR(TM_Sched_QueueQuantum)

    TM_CTL(ctl, hndl)
	if (queue_ind < ctl->tm_total_queues)
	{
		queue = &(ctl->tm_queue_array[queue_ind]);
				
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.QueueQuantum , queue_ind , TM_Sched_QueueQuantum)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_QueueQuantum , Quantum , queue->dwrr_quantum)
		TM_WRITE_TABLE_REGISTER(TM.Sched.QueueQuantum , queue_ind , TM_Sched_QueueQuantum)
		/* write register */
		if (rc)	goto out;
		
		/* Set drop profile pointer entry with data exisitng in SW image */
		rc = set_hw_queue_drop_profile_ptr(hndl,queue_ind);
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_queue(tm_handle hndl, uint32_t queue_ind)
{	
    int rc =  -EFAULT;
	rc= set_hw_node_mapping(hndl, Q_LEVEL,queue_ind);
	if (rc)	goto out;
	rc= set_hw_node_elig_prio_function(hndl, Q_LEVEL,queue_ind);
	if (rc)	goto out;
	rc= set_hw_queue_params(hndl,queue_ind);
out:
	return rc;
}

/**
 */
int set_hw_a_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind)
{
    int rc =  -EFAULT;

	int entry;
    int base_ind;
    TM_REGISTER_VAR(TM_Drop_AlvlDropProfPtr)

    TM_CTL(ctl, hndl)

    if (node_ind < ctl->tm_total_a_nodes)
	{
		/* Set drop profile pointer entry with data exisitng in SW image
		 * to avoid read-modify-write from HW */
		/* Entry in the table */
		entry = node_ind/TM_A_DRP_PROF_PER_ENTRY;
		base_ind = entry*TM_A_DRP_PROF_PER_ENTRY;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.AlvlDropProfPtr , entry , TM_Drop_AlvlDropProfPtr);
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_AlvlDropProfPtr , ProfPtr0 , ctl->tm_a_lvl_drop_prof_ptr[base_ind+0])
		TM_REGISTER_SET(TM_Drop_AlvlDropProfPtr , ProfPtr1 , ctl->tm_a_lvl_drop_prof_ptr[base_ind+1])
		TM_REGISTER_SET(TM_Drop_AlvlDropProfPtr , ProfPtr2 , ctl->tm_a_lvl_drop_prof_ptr[base_ind+2])
		TM_REGISTER_SET(TM_Drop_AlvlDropProfPtr , ProfPtr3 , ctl->tm_a_lvl_drop_prof_ptr[base_ind+3])
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.AlvlDropProfPtr , entry , TM_Drop_AlvlDropProfPtr)
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_a_node_params(tm_handle hndl, uint32_t node_ind)
{
    int rc =  -EFAULT;

    struct tm_a_node *node = NULL;

    TM_REGISTER_VAR(TM_Sched_AlvlQuantum)
    TM_REGISTER_VAR(TM_Sched_AlvlDWRRPrioEn)

    TM_CTL(ctl, hndl)

    if (node_ind < ctl->tm_total_a_nodes)
	{
		node = &(ctl->tm_a_node_array[node_ind]);
		
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.AlvlQuantum , node_ind , TM_Sched_AlvlQuantum)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_AlvlQuantum , Quantum , node->dwrr_quantum)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.AlvlQuantum , node_ind , TM_Sched_AlvlQuantum)
		if (rc)	goto out;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.AlvlDWRRPrioEn , node_ind , TM_Sched_AlvlDWRRPrioEn)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_AlvlDWRRPrioEn , En , node->dwrr_enable_per_priority_mask)
		TM_WRITE_TABLE_REGISTER(TM.Sched.AlvlDWRRPrioEn , node_ind , TM_Sched_AlvlDWRRPrioEn)
		/* write register */
		if (rc)	goto out;
		
		/* Set drop profile pointer entry with data exisitng in SW image */
		rc = set_hw_a_node_drop_profile_ptr(hndl, node_ind);
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


int set_hw_a_node(tm_handle hndl, uint32_t node_ind)
{
    int rc =  -EFAULT;
	rc = set_hw_node_mapping(hndl, A_LEVEL, node_ind);
	if (rc)	goto out;
	rc = set_hw_node_elig_prio_function(hndl, A_LEVEL, node_ind);
	if (rc)	goto out;
	rc = set_hw_a_node(hndl, node_ind);
out:
	return rc;
}


int set_hw_b_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind)
{	
    int rc =  -EFAULT;
    int entry;
    int base_ind;
 
    TM_REGISTER_VAR(TM_Drop_BlvlDropProfPtr)

    TM_CTL(ctl, hndl)

	if (node_ind < ctl->tm_total_b_nodes)
	{
		/* Set drop profile pointer entry with data exisitng in SW image
		 * to avoid read-modify-write from HW */
		/* Entry in the table */
		entry = node_ind/TM_B_DRP_PROF_PER_ENTRY;
		base_ind = entry*TM_B_DRP_PROF_PER_ENTRY;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.BlvlDropProfPtr , entry , TM_Drop_BlvlDropProfPtr);
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr0 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+0])
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr1 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+1])
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr2 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+2])
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr3 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+3])
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr4 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+4])
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr5 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+5])
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr6 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+6])
		TM_REGISTER_SET(TM_Drop_BlvlDropProfPtr , ProfPtr7 , ctl->tm_b_lvl_drop_prof_ptr[base_ind+7])
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.BlvlDropProfPtr , entry , TM_Drop_BlvlDropProfPtr)
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_b_node_params(tm_handle hndl, uint32_t node_ind)
{	
    int rc =  -EFAULT;
    struct tm_b_node *node = NULL;

    TM_REGISTER_VAR(TM_Sched_BlvlQuantum)
    TM_REGISTER_VAR(TM_Sched_BlvlDWRRPrioEn)

    TM_CTL(ctl, hndl)

	if (node_ind < ctl->tm_total_b_nodes)
	{
		node = &(ctl->tm_b_node_array[node_ind]);

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.BlvlQuantum , node_ind , TM_Sched_BlvlQuantum)
		if (rc) goto out;
		/* assign register fields */		
		/* write register */
		TM_REGISTER_SET(TM_Sched_BlvlQuantum , Quantum , node->dwrr_quantum)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.BlvlQuantum , node_ind , TM_Sched_BlvlQuantum)
		if (rc)	goto out;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.BlvlDWRRPrioEn , node_ind , TM_Sched_BlvlDWRRPrioEn)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_BlvlDWRRPrioEn , En , node->dwrr_enable_per_priority_mask)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.BlvlDWRRPrioEn , node_ind , TM_Sched_BlvlDWRRPrioEn)
		if (rc)	goto out;

		/* Set drop profile pointer entry with data exisitng in SW image */
		rc = set_hw_b_node_drop_profile_ptr(hndl, node_ind);
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_b_node(tm_handle hndl, uint32_t node_ind)
{
    int rc =  -EFAULT;
	rc = set_hw_node_mapping(hndl, B_LEVEL, node_ind);
	if (rc)	goto out;
	rc = set_hw_node_elig_prio_function(hndl, B_LEVEL, node_ind);
	if (rc)	goto out;
	rc = set_hw_b_node_params(hndl, node_ind);
out:
	return rc;
}




int set_hw_c_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind , uint8_t cos)
{	
    int rc =  -EFAULT;
    int entry;
    int base_ind;

    TM_REGISTER_VAR(TM_Drop_ClvlDropProfPtr_CoS)

    TM_CTL(ctl, hndl)

	if ((node_ind < ctl->tm_total_c_nodes) && (cos < TM_WRED_COS))
	{
		/* Set drop profile pointer entry with data exisitng in SW image
		 * to avoid read-modify-write from HW */
		/* Entry in the table */
		entry = node_ind/TM_C_DRP_PROF_PER_ENTRY;
		base_ind = entry*TM_C_DRP_PROF_PER_ENTRY;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Drop.ClvlDropProfPtr_CoS[cos] , entry , TM_Drop_ClvlDropProfPtr_CoS)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr0 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+0])
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr1 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+1])
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr2 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+2])
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr3 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+3])
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr4 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+4])
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr5 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+5])
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr6 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+6])
		TM_REGISTER_SET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr7 , ctl->tm_c_lvl_drop_prof_ptr[cos][base_ind+7])
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Drop.ClvlDropProfPtr_CoS[cos] , entry , TM_Drop_ClvlDropProfPtr_CoS)
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_c_node_params(tm_handle hndl, uint32_t node_ind)
{	
    int rc =  -EFAULT;
    int i;
    struct tm_c_node *node = NULL;

    TM_REGISTER_VAR(TM_Sched_ClvlQuantum)
    TM_REGISTER_VAR(TM_Sched_ClvlDWRRPrioEn)

    TM_CTL(ctl, hndl)

	if (node_ind < ctl->tm_total_c_nodes)
	{
		node = &(ctl->tm_c_node_array[node_ind]);

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.ClvlQuantum , node_ind , TM_Sched_ClvlQuantum)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_ClvlQuantum , Quantum , node->dwrr_quantum)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.ClvlQuantum , node_ind , TM_Sched_ClvlQuantum)
		if (rc)	goto out;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.ClvlDWRRPrioEn , node_ind , TM_Sched_ClvlDWRRPrioEn)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_ClvlDWRRPrioEn , En , node->dwrr_enable_per_priority_mask)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.ClvlDWRRPrioEn , node_ind , TM_Sched_ClvlDWRRPrioEn)
		if (rc)	goto out;

		for (i=0; i<TM_WRED_COS; i++)
		{
			/* Set drop profile pointer entry with data exisitng in SW image */
			rc = set_hw_c_node_drop_profile_ptr(hndl, node_ind , i);
			if (rc) goto out;
		}
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}



int set_hw_c_node(tm_handle hndl, uint32_t node_ind)
{
    int rc =  -EFAULT;
	rc = set_hw_node_mapping(hndl, C_LEVEL, node_ind);
	if (rc)	goto out;
	rc = set_hw_node_elig_prio_function(hndl, C_LEVEL, node_ind);
	if (rc)	goto out;
	rc = set_hw_c_node_params(hndl, node_ind);
out:
	return rc;
}


/**
 */

/**
 */
int set_hw_port_scheduling(tm_handle hndl, uint8_t port_ind)
{
    int rc =  -EFAULT;
    struct tm_port *port = NULL;
   
    TM_REGISTER_VAR(TM_Sched_PortQuantumsPriosLo)
    TM_REGISTER_VAR(TM_Sched_PortQuantumsPriosHi)
    TM_REGISTER_VAR(TM_Sched_PortDWRRPrioEn)

    TM_CTL(ctl, hndl);

    if (port_ind < ctl->tm_total_ports)
	{
		port = &(ctl->tm_port_array[port_ind]);

		/* DWRR for Port */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortQuantumsPriosLo , port_ind , TM_Sched_PortQuantumsPriosLo)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosLo , Quantum0 , port->dwrr_quantum_per_level[0].quantum)
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosLo , Quantum1 , port->dwrr_quantum_per_level[1].quantum)
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosLo , Quantum2 , port->dwrr_quantum_per_level[2].quantum)
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosLo , Quantum3 , port->dwrr_quantum_per_level[3].quantum)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortQuantumsPriosLo , port_ind , TM_Sched_PortQuantumsPriosLo)
		if (rc)	goto out;

		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortQuantumsPriosHi , port_ind , TM_Sched_PortQuantumsPriosHi)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosHi , Quantum4 , port->dwrr_quantum_per_level[4].quantum)
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosHi , Quantum5 , port->dwrr_quantum_per_level[5].quantum)
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosHi , Quantum6 , port->dwrr_quantum_per_level[6].quantum)
		TM_REGISTER_SET(TM_Sched_PortQuantumsPriosHi , Quantum7 , port->dwrr_quantum_per_level[7].quantum)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortQuantumsPriosHi , port_ind , TM_Sched_PortQuantumsPriosHi)
		if (rc) goto out;

		/* DWRR for C-nodes in Port's range */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortDWRRPrioEn , port_ind , TM_Sched_PortDWRRPrioEn)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortDWRRPrioEn ,En , port->dwrr_enable_per_priority_mask)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortDWRRPrioEn , port_ind , TM_Sched_PortDWRRPrioEn)
		if (rc)	goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_port_drop_global(tm_handle hndl, uint8_t port_ind)
{
    int rc =  -EFAULT;
    struct tm_port *port = NULL;

    TM_CTL(ctl, hndl)

    if (port_ind < ctl->tm_total_ports)
	{
		port = &(ctl->tm_port_array[port_ind]);
		rc = set_hw_ports_drop_profile(hndl, port->wred_profile_ref, port_ind);
	}
    return rc;
}



int set_hw_port_drop_cos(tm_handle hndl, uint8_t port_ind)
{
    int rc = 0;
    struct tm_port *port ;
	uint8_t cos;

    TM_CTL(ctl, hndl)

    if (port_ind < ctl->tm_total_ports)
	{
		port = &(ctl->tm_port_array[port_ind]);
		for (cos=0; cos<TM_WRED_COS; cos++)
		{
			rc = set_hw_ports_drop_profile_cos(hndl, cos, port->wred_profile_ref_cos[cos], port_ind);
			if (rc) return rc;
		}
	}
    return rc;
}



/**
 */
int set_hw_port(tm_handle hndl, uint8_t port_ind)
{
    int rc =  -EFAULT;
    TM_CTL(ctl, hndl);
    if (port_ind < ctl->tm_total_ports)
	{
		rc = set_hw_node_mapping(hndl, P_LEVEL, port_ind);
		if (rc < 0)
			goto out;

		rc = set_hw_port_shaping(hndl, port_ind);
		if (rc < 0)
			goto out;

		rc = set_hw_port_scheduling(hndl, port_ind);
		if (rc < 0)
			goto out;

		rc = set_hw_port_drop_global(hndl, port_ind);
		if (rc < 0)
			goto out;

		rc = set_hw_node_elig_prio_function(hndl, P_LEVEL, port_ind);
	}
out:
    /*   reset_hw_connection(ctl); already done  */
    return rc;
}


/**
 */
int set_hw_tree_deq_status(tm_handle hndl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_TreeDeqEn)

    TM_CTL(ctl, hndl)
    
	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.TreeDeqEn, TM_Sched_TreeDeqEn)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Sched_TreeDeqEn , En , ctl->tree_deq_status)
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.TreeDeqEn, TM_Sched_TreeDeqEn )
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int get_hw_tree_deq_status(tm_handle hndl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_TreeDeqEn)

    TM_CTL(ctl, hndl)
    
	TM_READ_REGISTER(TM.Sched.TreeDeqEn, TM_Sched_TreeDeqEn)
    if (rc)  goto out;

	TM_REGISTER_GET(TM_Sched_TreeDeqEn, En , ctl->tree_deq_status, (uint8_t) ) /* no casting */
out:
    COMPLETE_HW_WRITE
	return rc;
}

/**
 */
int set_hw_tree_dwrr_priority(tm_handle hndl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_TreeDWRRPrioEn)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Sched.TreeDWRRPrioEn, TM_Sched_TreeDWRRPrioEn)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Sched_TreeDWRRPrioEn , PrioEn , ctl->tree_dwrr_enable_per_priority_mask)
	/* write register */
    TM_WRITE_REGISTER(TM.Sched.TreeDWRRPrioEn, TM_Sched_TreeDWRRPrioEn)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int get_hw_tree_dwrr_priority_status(tm_handle hndl)

{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_TreeDWRRPrioEn)

    TM_CTL(ctl, hndl)

	TM_READ_REGISTER(TM.Sched.TreeDWRRPrioEn, TM_Sched_TreeDWRRPrioEn)
	if (rc) goto out;
 	
	TM_REGISTER_GET(TM_Sched_TreeDWRRPrioEn , PrioEn , ctl->tree_dwrr_enable_per_priority_mask , (uint8_t) ) 
 out:
    COMPLETE_HW_WRITE
	return rc;
}

/**
 */
int set_hw_install_queue(tm_handle hndl, uint32_t index)
{	
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_QMgr_QueueInstallCmd)

    TM_CTL(ctl, hndl)
	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.QMgr.QueueInstallCmd, TM_QMgr_QueueInstallCmd)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_QMgr_QueueInstallCmd , Queue , index)
	/* write register */
    TM_WRITE_REGISTER(TM.QMgr.QueueInstallCmd, TM_QMgr_QueueInstallCmd)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_uninstall_queue(tm_handle hndl, uint32_t index)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_QMgr_QueueRemoveCmd)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.QMgr.QueueRemoveCmd, TM_QMgr_QueueRemoveCmd)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_QMgr_QueueRemoveCmd , Queue , index)
	/* write register */
    TM_WRITE_REGISTER(TM.QMgr.QueueRemoveCmd, TM_QMgr_QueueRemoveCmd)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}




/*
	function  set_hw_node_shaping_ex(hndl,level,node_ind, profile); is platform dependent - in file ...platform.c
*/

#define PROFILE_VALIDATION(nodes,nodes_count)	\
		NODE_VALIDATION(nodes_count)\
		if (rc==0)\
		{\
			sh_profile_index=ctl->nodes[node_ind].shaping_profile_ref;\
			if (rm_shaping_profile_status(ctl->rm,sh_profile_index) == 1)\
				profile = &(ctl->tm_shaping_profiles[sh_profile_index]);\
			else\
				rc= -EFAULT;\
		}

int set_hw_node_shaping(tm_handle hndl, enum tm_level level, uint32_t node_ind)
{
    int rc = 0;
    uint32_t	sh_profile_index;
    struct tm_shaping_profile *profile = NULL;
	
    TM_CTL(ctl, hndl)

	/* get shaper parameters from attached shaping profile */
	switch (level)
	{
	case Q_LEVEL:
		PROFILE_VALIDATION(tm_queue_array,ctl->tm_total_queues)
		if (rc)	goto out;
		break;		
	case A_LEVEL:
		PROFILE_VALIDATION(tm_a_node_array,ctl->tm_total_a_nodes)
		if (rc)	goto out;
		break;
	case B_LEVEL:
		PROFILE_VALIDATION(tm_b_node_array,ctl->tm_total_b_nodes)
		if (rc)	goto out;
		break;
	case C_LEVEL:
		PROFILE_VALIDATION(tm_c_node_array,ctl->tm_total_c_nodes)
		if (rc)	goto out;
		break;
	default:
		rc =  -EFAULT;
		goto out;
		break;
	}
	rc = set_hw_node_shaping_ex(hndl,level,node_ind, profile);
out:
    COMPLETE_HW_WRITE;
	return rc;
}

/**
 */
int set_hw_node_elig_prio_function_ex(tm_handle hndl, enum tm_level node_level, uint32_t node_index,uint8_t elig_prio_func_ptr)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_QueueEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_PortEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_AlvlEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_BlvlEligPrioFuncPtr)
    TM_REGISTER_VAR(TM_Sched_ClvlEligPrioFuncPtr)

    TM_CTL(ctl, hndl)

    switch (node_level)
	{
    case P_LEVEL:
        if (node_index < ctl->tm_total_ports)
		{
			/* reset register reserved fields */
			TM_RESET_TABLE_REGISTER(TM.Sched.PortEligPrioFuncPtr , node_index , TM_Sched_PortEligPrioFuncPtr)
			if (rc) break;
			/* assign register fields */		
			TM_REGISTER_SET(TM_Sched_PortEligPrioFuncPtr , Ptr , elig_prio_func_ptr)
			/* write register */
			TM_WRITE_TABLE_REGISTER(TM.Sched.PortEligPrioFuncPtr , node_index , TM_Sched_PortEligPrioFuncPtr)
		}
		break;
    case C_LEVEL:
        if (node_index < ctl->tm_total_c_nodes)
		{
			/* reset register reserved fields */
			TM_RESET_TABLE_REGISTER(TM.Sched.ClvlEligPrioFuncPtr , node_index , TM_Sched_ClvlEligPrioFuncPtr)
			if (rc) break;
			/* assign register fields */		
			TM_REGISTER_SET(TM_Sched_ClvlEligPrioFuncPtr , Ptr , elig_prio_func_ptr)
			/* write register */
			TM_WRITE_TABLE_REGISTER(TM.Sched.ClvlEligPrioFuncPtr , node_index , TM_Sched_ClvlEligPrioFuncPtr)
		}
		break;
    case B_LEVEL:
        if (node_index < ctl->tm_total_b_nodes)
		{
			/* reset register reserved fields */
			TM_RESET_TABLE_REGISTER(TM.Sched.BlvlEligPrioFuncPtr , node_index , TM_Sched_BlvlEligPrioFuncPtr)
			if (rc) break;
			/* assign register fields */		
			TM_REGISTER_SET(TM_Sched_BlvlEligPrioFuncPtr , Ptr , elig_prio_func_ptr)
			/* write register */
			TM_WRITE_TABLE_REGISTER(TM.Sched.BlvlEligPrioFuncPtr , node_index , TM_Sched_BlvlEligPrioFuncPtr)
		}
		break;
    case A_LEVEL:
        if (node_index < ctl->tm_total_a_nodes)
		{
		/* reset register reserved fields */
			TM_RESET_TABLE_REGISTER(TM.Sched.AlvlEligPrioFuncPtr , node_index , TM_Sched_AlvlEligPrioFuncPtr)
			if (rc) break;
			/* assign register fields */		
			TM_REGISTER_SET(TM_Sched_AlvlEligPrioFuncPtr , Ptr , elig_prio_func_ptr)
			/* write register */
			TM_WRITE_TABLE_REGISTER(TM.Sched.AlvlEligPrioFuncPtr , node_index , TM_Sched_AlvlEligPrioFuncPtr)
		}
		break;
    case Q_LEVEL:
        if (node_index < ctl->tm_total_queues)
		{
			/* reset register reserved fields */
			TM_RESET_TABLE_REGISTER(TM.Sched.QueueEligPrioFuncPtr , node_index , TM_Sched_QueueEligPrioFuncPtr)
			if (rc) break;
			/* assign register fields */		
			TM_REGISTER_SET(TM_Sched_QueueEligPrioFuncPtr , Ptr , elig_prio_func_ptr)
			/* write register */
			TM_WRITE_TABLE_REGISTER(TM.Sched.QueueEligPrioFuncPtr , node_index , TM_Sched_QueueEligPrioFuncPtr)
		}
		break;
    }
    COMPLETE_HW_WRITE
	return rc;
}

/**
 */
int set_hw_node_elig_prio_function(tm_handle hndl, enum tm_level node_level, uint32_t node_index)
{
    int rc =  -EFAULT;
	
    TM_CTL(ctl, hndl)

    switch (node_level)
	{
		case P_LEVEL:
		{
			ctl->tm_port_array[node_index].node_temporary_disabled = 0;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, ctl->tm_port_array[node_index].elig_prio_func);
		}
		case C_LEVEL:
		{
			ctl->tm_c_node_array[node_index].node_temporary_disabled = 0;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, ctl->tm_c_node_array[node_index].elig_prio_func);
		}
		case B_LEVEL:
		{
			ctl->tm_b_node_array[node_index].node_temporary_disabled = 0;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, ctl->tm_b_node_array[node_index].elig_prio_func);
		}
		case A_LEVEL:
		{
			ctl->tm_a_node_array[node_index].node_temporary_disabled = 0;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, ctl->tm_a_node_array[node_index].elig_prio_func); 
		}
		case Q_LEVEL:
		{
			ctl->tm_queue_array[node_index].node_temporary_disabled = 0;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, ctl->tm_queue_array[node_index].elig_prio_func); 
		}
		default: break;
    }
    COMPLETE_HW_WRITE;
	return rc;
}

int set_hw_node_disable(tm_handle hndl, enum tm_level node_level, uint32_t node_index)
{
    int rc =  -EFAULT;

    TM_CTL(ctl, hndl)

    switch (node_level)
	{
		case P_LEVEL:
		{
			ctl->tm_port_array[node_index].node_temporary_disabled = 1;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, TM_NODE_DISABLED_FUN);
		}	 
		case C_LEVEL:
		{
			 ctl->tm_c_node_array[node_index].node_temporary_disabled = 1;
			 return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, TM_NODE_DISABLED_FUN);
		}
		case B_LEVEL:
		{
			ctl->tm_b_node_array[node_index].node_temporary_disabled = 1;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, TM_NODE_DISABLED_FUN);
		}
		case A_LEVEL:
		{
			ctl->tm_a_node_array[node_index].node_temporary_disabled = 1;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, TM_NODE_DISABLED_FUN); 
		}
		case Q_LEVEL:
		{
			ctl->tm_queue_array[node_index].node_temporary_disabled = 1;
			return set_hw_node_elig_prio_function_ex(hndl, node_level, node_index, TM_NODE_DISABLED_FUN); 
		}
		default: break;
    }
    COMPLETE_HW_WRITE;
	return rc;
}

/**
 */
int set_hw_disable_ports(tm_handle hndl, uint32_t total_ports)
{	
    int rc =  -EFAULT;
    int i;

    TM_REGISTER_VAR(TM_Sched_PortEligPrioFuncPtr)

    TM_CTL(ctl, hndl)
    
	/* Disable Ports */
    for (i = 0; i < (int)total_ports; i++)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortEligPrioFuncPtr , i , TM_Sched_PortEligPrioFuncPtr)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortEligPrioFuncPtr , Ptr , TM_ELIG_DEQ_DISABLE)  /* DeQ disable function ID */
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortEligPrioFuncPtr , i , TM_Sched_PortEligPrioFuncPtr)
		if (rc)	goto out;
    }
out:
    COMPLETE_HW_WRITE
	return rc;
}




/**
 *  Configure user Q level Eligible Priority Function 
 */
int set_hw_q_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset)
{	
    int rc =  -EFAULT;
    struct tm_elig_prio_func_queue *params;

    TM_REGISTER_VAR(TM_Sched_QueueEligPrioFunc)

    TM_CTL(ctl, hndl)

	params = &(ctl->tm_elig_prio_q_lvl_tbl[func_offset]);
	/* reset register reserved fields */
    TM_RESET_TABLE_REGISTER(TM.Sched.QueueEligPrioFunc, func_offset, TM_Sched_QueueEligPrioFunc)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Sched_QueueEligPrioFunc , FuncOut0 ,  params->tbl_entry.func_out[0]) 
	TM_REGISTER_SET(TM_Sched_QueueEligPrioFunc , FuncOut1 ,  params->tbl_entry.func_out[1]) 
	TM_REGISTER_SET(TM_Sched_QueueEligPrioFunc , FuncOut2 ,  params->tbl_entry.func_out[2]) 
	TM_REGISTER_SET(TM_Sched_QueueEligPrioFunc , FuncOut3 ,  params->tbl_entry.func_out[3]) 
	/* write register */
	TM_WRITE_TABLE_REGISTER(TM.Sched.QueueEligPrioFunc, func_offset, TM_Sched_QueueEligPrioFunc)
    if (rc)
    	goto out;

out:
    COMPLETE_HW_WRITE
	return rc;
}

/**
 *  Configure user Node level Eligible Priority Function
 */
int set_hw_a_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset)
{	
    int rc =  -EFAULT;
    int i;

    TM_REGISTER_VAR(TM_Sched_AlvlEligPrioFunc_Entry)

    TM_CTL(ctl, hndl)

	for (i = 0; i < 8; i++)
	{ 	
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.AlvlEligPrioFunc_Entry, func_offset + i*64, TM_Sched_AlvlEligPrioFunc_Entry)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut0 , ctl->tm_elig_prio_a_lvl_tbl[func_offset].tbl_entry[i].func_out[0])
		TM_REGISTER_SET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut1 , ctl->tm_elig_prio_a_lvl_tbl[func_offset].tbl_entry[i].func_out[1]) 
		TM_REGISTER_SET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut2 , ctl->tm_elig_prio_a_lvl_tbl[func_offset].tbl_entry[i].func_out[2]) 
		TM_REGISTER_SET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut3 , ctl->tm_elig_prio_a_lvl_tbl[func_offset].tbl_entry[i].func_out[3]) 
		TM_WRITE_TABLE_REGISTER(TM.Sched.AlvlEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_AlvlEligPrioFunc_Entry)
		/* write register */
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_b_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset)
{	
    int rc =  -EFAULT;
    int i;

    TM_REGISTER_VAR(TM_Sched_BlvlEligPrioFunc_Entry)

    TM_CTL(ctl, hndl)

	for (i = 0; i < 8; i++)
	{   
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.BlvlEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_BlvlEligPrioFunc_Entry);
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_BlvlEligPrioFunc_Entry , FuncOut0 , ctl->tm_elig_prio_b_lvl_tbl[func_offset].tbl_entry[i].func_out[0])
		TM_REGISTER_SET(TM_Sched_BlvlEligPrioFunc_Entry , FuncOut1 , ctl->tm_elig_prio_b_lvl_tbl[func_offset].tbl_entry[i].func_out[1]) 
		TM_REGISTER_SET(TM_Sched_BlvlEligPrioFunc_Entry , FuncOut2 , ctl->tm_elig_prio_b_lvl_tbl[func_offset].tbl_entry[i].func_out[2]) 
		TM_REGISTER_SET(TM_Sched_BlvlEligPrioFunc_Entry , FuncOut3 , ctl->tm_elig_prio_b_lvl_tbl[func_offset].tbl_entry[i].func_out[3]) 
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.BlvlEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_BlvlEligPrioFunc_Entry)
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_c_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset)
{	
    int rc =  -EFAULT;
    int i;

    TM_REGISTER_VAR(TM_Sched_ClvlEligPrioFunc_Entry)

    TM_CTL(ctl, hndl)

	for (i = 0; i < 8; i++)
	{   
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.ClvlEligPrioFunc_Entry, func_offset + i*64,TM_Sched_ClvlEligPrioFunc_Entry);
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_ClvlEligPrioFunc_Entry , FuncOut0 , ctl->tm_elig_prio_c_lvl_tbl[func_offset].tbl_entry[i].func_out[0]); 
		TM_REGISTER_SET(TM_Sched_ClvlEligPrioFunc_Entry , FuncOut1 , ctl->tm_elig_prio_c_lvl_tbl[func_offset].tbl_entry[i].func_out[1]); 
		TM_REGISTER_SET(TM_Sched_ClvlEligPrioFunc_Entry , FuncOut2 , ctl->tm_elig_prio_c_lvl_tbl[func_offset].tbl_entry[i].func_out[2]); 
		TM_REGISTER_SET(TM_Sched_ClvlEligPrioFunc_Entry , FuncOut3 , ctl->tm_elig_prio_c_lvl_tbl[func_offset].tbl_entry[i].func_out[3]); 
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.ClvlEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_ClvlEligPrioFunc_Entry);
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE;
	return rc;
}

int set_hw_p_lvl_elig_prio_func_entry(tm_handle hndl, uint16_t func_offset)
{	
    int rc =  -EFAULT;
    int i;

    TM_REGISTER_VAR(TM_Sched_PortEligPrioFunc_Entry)

    TM_CTL(ctl, hndl)

	for (i = 0; i < 8; i++)
	{   /* Entry ID */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_PortEligPrioFunc_Entry)
		if (rc) goto out;
		/* assign register fields */		
		/* write register */
		TM_REGISTER_SET(TM_Sched_PortEligPrioFunc_Entry , FuncOut0 , ctl->tm_elig_prio_p_lvl_tbl[func_offset].tbl_entry[i].func_out[0])
		TM_REGISTER_SET(TM_Sched_PortEligPrioFunc_Entry , FuncOut1 , ctl->tm_elig_prio_p_lvl_tbl[func_offset].tbl_entry[i].func_out[1])
		TM_REGISTER_SET(TM_Sched_PortEligPrioFunc_Entry , FuncOut2 , ctl->tm_elig_prio_p_lvl_tbl[func_offset].tbl_entry[i].func_out[2])
		TM_REGISTER_SET(TM_Sched_PortEligPrioFunc_Entry , FuncOut3 , ctl->tm_elig_prio_p_lvl_tbl[func_offset].tbl_entry[i].func_out[3])
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortEligPrioFunc_Entry , func_offset + i*64 ,TM_Sched_PortEligPrioFunc_Entry);
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_elig_prio_func_tbl_q_level(tm_handle hndl)
{	
    int j;
    int rc ;

    for (j = 0; j < TM_ELIG_FUNC_TABLE_SIZE; j++) 
	{ 
		rc = set_hw_q_elig_prio_func_entry(hndl, j);
		if (rc) break;
	}
	return rc;
}


/**
 */
int set_hw_elig_prio_func_tbl_a_level(tm_handle hndl)
{
	int rc =  -EFAULT;
	int j;
	for (j=0; j< TM_ELIG_FUNC_TABLE_SIZE; j++)
	{
		rc = set_hw_a_lvl_elig_prio_func_entry(hndl,j);
		if (rc) break;
    }
	return rc;
}


/**
 */
int set_hw_elig_prio_func_tbl_b_level(tm_handle hndl)
{
	int rc =  -EFAULT;
	int j;
	for (j=0; j<TM_ELIG_FUNC_TABLE_SIZE; j++)
	{
		rc = set_hw_b_lvl_elig_prio_func_entry(hndl,j);
		if (rc) break;
    }
	return rc;
}


/**
 */
int set_hw_elig_prio_func_tbl_c_level(tm_handle hndl)
{
	int rc =  -EFAULT;
	int j;
	for (j=0; j<TM_ELIG_FUNC_TABLE_SIZE; j++)
	{
		rc = set_hw_c_lvl_elig_prio_func_entry(hndl,j);
		if (rc) break;
    }
	return rc;
}


/**
 */
int set_hw_elig_prio_func_tbl_p_level(tm_handle hndl)
{
	int rc =  -EFAULT;
	int j;
	for (j=0; j<TM_ELIG_FUNC_TABLE_SIZE; j++)
	{
		rc = set_hw_p_lvl_elig_prio_func_entry(hndl,j);
		if (rc) break;
    }
	return rc;
}


/**
 */
int set_hw_port_deficit_clear(tm_handle hndl, uint8_t index)
{	
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_PortDefPrioHi)
    TM_REGISTER_VAR(TM_Sched_PortDefPrioLo)

    TM_CTL(ctl, hndl)

    if (index < ctl->tm_total_ports)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortDefPrioHi, index, TM_Sched_PortDefPrioHi)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortDefPrioHi , Deficit0 , 0x1)
		TM_REGISTER_SET(TM_Sched_PortDefPrioHi , Deficit1 , 0x1) 
		TM_REGISTER_SET(TM_Sched_PortDefPrioHi , Deficit2 , 0x1) 
		TM_REGISTER_SET(TM_Sched_PortDefPrioHi , Deficit3 , 0x1) 
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortDefPrioHi, index, TM_Sched_PortDefPrioHi)
		if (rc)	goto out;
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.PortDefPrioLo, index, TM_Sched_PortDefPrioLo)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_PortDefPrioLo , Deficit0 , 0x1)
		TM_REGISTER_SET(TM_Sched_PortDefPrioLo , Deficit1 , 0x1) 
		TM_REGISTER_SET(TM_Sched_PortDefPrioLo , Deficit2 , 0x1) 
		TM_REGISTER_SET(TM_Sched_PortDefPrioLo , Deficit3 , 0x1) 
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.PortDefPrioLo, index, TM_Sched_PortDefPrioLo)
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_c_node_deficit_clear(tm_handle hndl, uint32_t index)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_CLvlDef)

    TM_CTL(ctl, hndl)
    
	if (index < ctl->tm_total_c_nodes)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.CLvlDef, index, TM_Sched_CLvlDef)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_CLvlDef , Deficit , 0x1)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.CLvlDef, index, TM_Sched_CLvlDef)
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_b_node_deficit_clear(tm_handle hndl, uint32_t index)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_BlvlDef)

    TM_CTL(ctl, hndl)

    if (index < ctl->tm_total_b_nodes)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.BlvlDef, index, TM_Sched_BlvlDef)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_BlvlDef , Deficit , 0x1)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.BlvlDef, index, TM_Sched_BlvlDef)
		if (rc)  goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_a_node_deficit_clear(tm_handle hndl, uint32_t index)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_AlvlDef)

    TM_CTL(ctl, hndl)
    if (index < ctl->tm_total_a_nodes)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.AlvlDef, index, TM_Sched_AlvlDef)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_AlvlDef , Deficit , 0x1)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.AlvlDef, index, TM_Sched_AlvlDef)
		if (rc)
		  goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_queue_deficit_clear(tm_handle hndl, uint32_t index)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_QueueDef)

    TM_CTL(ctl, hndl)

    if (index < ctl->tm_total_queues)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.Sched.QueueDef, index, TM_Sched_QueueDef)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_Sched_QueueDef , Deficit , 0x1)
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.Sched.QueueDef, index, TM_Sched_QueueDef)
		if (rc)
		  goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int get_hw_flush_queue_status(tm_handle hndl, uint32_t index, uint8_t * status)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_QMgr_QueueFlush)

    TM_CTL(ctl, hndl)

    if (index < ctl->tm_total_queues)
	{
        TM_READ_TABLE_REGISTER(TM.QMgr.QueueFlush, index, TM_QMgr_QueueFlush)
        if (rc) goto out;
        TM_REGISTER_GET(TM_QMgr_QueueFlush, Flush, *status, (uint8_t))
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
#define TM_FLUSH_MAX_RETRIES     16
int set_hw_flush_queue(tm_handle hndl, uint32_t index, uint8_t block_en)
{
    int rc = -EFAULT;
	int retries = 0;
    uint32_t inst_q_length = (uint32_t)(-1);
    uint32_t av_q_length = (uint32_t)(-1);
	uint8_t status = (uint8_t)(-1);

    TM_REGISTER_VAR(TM_QMgr_QueueFlush)

    TM_CTL(ctl, hndl)

    if (index < ctl->tm_total_queues)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.QMgr.QueueFlush, index, TM_QMgr_QueueFlush)
		if (rc) goto out;
		/* assign register fields */
		TM_REGISTER_SET(TM_QMgr_QueueFlush, Flush, 1) /* enable */
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.QMgr.QueueFlush, index, TM_QMgr_QueueFlush)
		if (rc) goto out;

        if (block_en) { /* blocking API mode */
            rc = flush_hw_connection(TM_ENV(ctl));
            if (rc)
                goto out;

            while (retries < TM_FLUSH_MAX_RETRIES)
            {
              rc = get_hw_flush_queue_status(hndl, index, &status);
              if (rc) goto out;
              if (status == 0)
                  break; /* end */
              else
                  retries++;
            }
            if (status)
            {
                /* mainly for linux WM test rechecking */
                rc = get_hw_queue_length(hndl, Q_LEVEL, index, &inst_q_length, &av_q_length);
                if (inst_q_length)
                    return -EFAULT;
                else
                    status = TM_DISABLE;
            }
        }
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int get_hw_flush_port_status(tm_handle hndl, uint32_t index, uint8_t * status)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_QMgr_PortFlush)

    TM_CTL(ctl, hndl)

    if (index < ctl->tm_total_ports)
	{
        TM_READ_TABLE_REGISTER(TM.QMgr.PortFlush, index, TM_QMgr_PortFlush)
        if (rc) goto out;
        TM_REGISTER_GET(TM_QMgr_PortFlush, Flush, *status, (uint8_t))
		if (rc) goto out;
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_flush_port(tm_handle hndl, uint32_t index, uint8_t block_en)
{
    int rc =  -EFAULT;
    int retries = 0;
    uint32_t inst_q_length = (uint32_t)(-1);
    uint32_t av_q_length = (uint32_t)(-1);
	uint8_t status = (uint8_t)(-1);

    TM_REGISTER_VAR(TM_QMgr_PortFlush)

    TM_CTL(ctl, hndl);

    if (index < ctl->tm_total_ports)
	{
		/* Set flush bit on the Port */
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.QMgr.PortFlush, index, TM_QMgr_PortFlush)
		if (rc) goto out;
		/* assign register fields */
		TM_REGISTER_SET(TM_QMgr_PortFlush, Flush, 1) /* enable */
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.QMgr.PortFlush, index, TM_QMgr_PortFlush)
		if (rc) goto out;

        if (block_en) { /* blocking API mode */
            rc = flush_hw_connection(TM_ENV(ctl));
            if (rc)
                goto out;

            while (retries < TM_FLUSH_MAX_RETRIES)
            {
              rc = get_hw_flush_port_status(hndl, index, &status);
              if (rc) goto out;
              if (status == 0)
                  break; /* end */
              else
                  retries++;
            }
            if (status)
            {
                /* mainly for linux WM test rechecking */
                rc = get_hw_queue_length(hndl, P_LEVEL, index, &inst_q_length, &av_q_length);
                if (inst_q_length)
                    return -EFAULT;
                else
                    status = TM_DISABLE;
            }
        }
	}
out:
    COMPLETE_HW_WRITE;
	return rc;
}


/**
 */
int set_hw_tm2tm_glob_config(tm_handle hndl,
					   uint8_t cos_sel,
					   struct tm_ext_hdr *ext_hdr,
					   struct tm_ctrl_pkt_str *control_pkt,
					   struct tm_delta_range *range)
{
    int rc =  -EFAULT;
    int i;

    TM_REGISTER_VAR(TM_Drop_TMtoTMDPCoSSel)
    TM_REGISTER_VAR(TM_TM2TM_ExtHeadersSize)
    TM_REGISTER_VAR(TM_TM2TM_ExtHeadersContent)
    TM_REGISTER_VAR(TM_TM2TM_TMtoTMPktStructure)
    TM_REGISTER_VAR(TM_TM2TM_TMHeader)
    TM_REGISTER_VAR(TM_TM2TM_Delta2PrioMap)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.TMtoTMDPCoSSel, TM_Drop_TMtoTMDPCoSSel)
	if (rc) goto out;
	/* assign register fields */		
	TM_REGISTER_SET(TM_Drop_TMtoTMDPCoSSel , CDPCoSSel , cos_sel)
	/* write register */
    TM_WRITE_REGISTER(TM.Drop.TMtoTMDPCoSSel, TM_Drop_TMtoTMDPCoSSel)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.ExtHeadersSize,  TM_TM2TM_ExtHeadersSize)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_ExtHeadersSize , Size , ext_hdr->size)
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.ExtHeadersSize,  TM_TM2TM_ExtHeadersSize)
    if (rc) goto out;

    for (i = 0; i < 4; i++)
	{
		/* reset register reserved fields */
		TM_RESET_TABLE_REGISTER(TM.TM2TM.ExtHeadersContent, i, TM_TM2TM_ExtHeadersContent)
		if (rc) goto out;
		/* assign register fields */		
		TM_REGISTER_SET(TM_TM2TM_ExtHeadersContent,Headers, ext_hdr->content[i])
		/* write register */
		TM_WRITE_TABLE_REGISTER(TM.TM2TM.ExtHeadersContent, i, TM_TM2TM_ExtHeadersContent)
        if (rc) goto out;
    }
	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMtoTMPktStructure, TM_TM2TM_TMtoTMPktStructure)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TMtoTMPktStructure , Ports , control_pkt->ports)
    TM_REGISTER_SET(TM_TM2TM_TMtoTMPktStructure , Nodes , control_pkt->nodes)
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.TMtoTMPktStructure, TM_TM2TM_TMtoTMPktStructure)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMHeader, TM_TM2TM_TMHeader)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TMHeader , Header , 0x1)
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.TMHeader, TM_TM2TM_TMHeader)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.Delta2PrioMap, TM_TM2TM_Delta2PrioMap)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_Delta2PrioMap , UpperRange0 , range->upper_range0)
    TM_REGISTER_SET(TM_TM2TM_Delta2PrioMap , UpperRange1 , range->upper_range1)
    TM_REGISTER_SET(TM_TM2TM_Delta2PrioMap , UpperRange2 , range->upper_range2)
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.Delta2PrioMap, TM_TM2TM_Delta2PrioMap)
    if (rc)  goto out;

out:
    COMPLETE_HW_WRITE
	return rc;

}


/**
 */
int set_hw_dp_remote_resp(tm_handle hndl, enum tm2tm_channel remote_lvl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_TM2TM_RespRemoteDPSel)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.RespRemoteDPSel, TM_TM2TM_RespRemoteDPSel)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_RespRemoteDPSel , DPSel , remote_lvl)
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.RespRemoteDPSel, TM_TM2TM_RespRemoteDPSel)
    if (rc) goto out;

out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_dp_local_resp(tm_handle hndl, uint8_t port_dp, enum tm_level local_lvl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_RespLocalDPSel)

    TM_CTL(ctl, hndl)

	(void)port_dp;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.RespLocalDPSel, TM_Drop_RespLocalDPSel)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Drop_RespLocalDPSel , DPSel , local_lvl)
    TM_REGISTER_SET(TM_Drop_RespLocalDPSel , PortDPSel , port_dp)
	/* write register */
    TM_WRITE_REGISTER(TM.Drop.RespLocalDPSel, TM_Drop_RespLocalDPSel)
    if (rc)
      goto out;

out:
    COMPLETE_HW_WRITE
	return rc;
}

/**
 */
int get_hw_dp_local_resp(tm_handle hndl, uint8_t * port_dp, enum tm_level  *local_lvl)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_RespLocalDPSel)

    TM_CTL(ctl, hndl)

    TM_READ_REGISTER(TM.Drop.RespLocalDPSel, TM_Drop_RespLocalDPSel)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_GET(TM_Drop_RespLocalDPSel , DPSel , *local_lvl, (enum tm_level) )
    TM_REGISTER_GET(TM_Drop_RespLocalDPSel , PortDPSel , *port_dp, (uint8_t) )
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
int set_hw_tm2tm_lc_config(tm_handle hndl,
					 uint16_t quantum,
					 void * qmap_hdr_ptr,
					 uint8_t ctrl_hdr,
					 uint8_t tm_port)
{
    int rc =  -EFAULT;


    TM_REGISTER_VAR(TM_TM2TM_QmapHeaderCont)

    TM_REGISTER_VAR(TM_Drop_TMtoTMPktGenQuantum)
    TM_REGISTER_VAR(TM_TM2TM_ContHeader)
    TM_REGISTER_VAR(TM_TM2TM_TMPort)

    TM_CTL(ctl, hndl)

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.TMtoTMPktGenQuantum, TM_Drop_TMtoTMPktGenQuantum)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Drop_TMtoTMPktGenQuantum , Quantum , quantum)
	/* write register */
    TM_WRITE_REGISTER(TM.Drop.TMtoTMPktGenQuantum, TM_Drop_TMtoTMPktGenQuantum)
    if (rc) goto out;

	/* because register QmapHeaderCont contains 64 bits of Qmap header only,
	   in order to provide platform independent code (some platforms don't support 64 bit variables)
	    the QmapHeaderCont register is processed directly
	*/
	
	tm_memcpy(TM_REGISTER_VAR_ADDR(TM_TM2TM_QmapHeaderCont), qmap_hdr_ptr, 8 /*bytes */) ;
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.QmapHeaderCont, TM_TM2TM_QmapHeaderCont)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.ContHeader , TM_TM2TM_ContHeader)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_ContHeader , LnCard , ctrl_hdr)
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.ContHeader , TM_TM2TM_ContHeader)
    if (rc) goto out;

	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMPort , TM_TM2TM_TMPort)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TMPort , Port , tm_port)
	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.TMPort , TM_TM2TM_TMPort)
    if (rc) goto out;

out:
    COMPLETE_HW_WRITE
	return rc;

}


/**
 */
int set_hw_tm2tm_enable(tm_handle hndl)
{
     int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_TM2TM_TMtoTMEn)

    TM_CTL(ctl, hndl)
	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.TM2TM.TMtoTMEn, TM_TM2TM_TMtoTMEn)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_TM2TM_TMtoTMEn , MsgQmrPck_alw_not_full , 0x0) /* reset value */
    TM_REGISTER_SET(TM_TM2TM_TMtoTMEn , IngUpdEn , 0x1 );               /* reset value */
    TM_REGISTER_SET(TM_TM2TM_TMtoTMEn , En , 0x1);                     /* enable */
 	/* write register */
    TM_WRITE_REGISTER(TM.TM2TM.TMtoTMEn, TM_TM2TM_TMtoTMEn)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}


/**
 */
/*
int set_hw_port_sms_attr_pbase(tm_handle hndl, uint8_t index)
{
    int rc =  -EFAULT;
	(void) hndl;
	(void)index;
  check if it exists in SN
    struct TM_RCB_SMSPortAttrPBase p_attr;

   	TM_CTL(ctl, hndl)

    p_attr.PBase = ctl->tm_port_sms_attr_pbase[index].pbase;
    p_attr.PShift = ctl->tm_port_sms_attr_pbase[index].pshift;
    // debug fields
    p_attr.AddTiRxHead = 0;
    p_attr.RxChanFieldEn = 0;

    p_attr.RxTimeFieldEn = 0;
	rc = tm_table_entry_write(TM_ENV(ctl), (void*)&TM.RCB.SMSPortAttrPBase, index, &p_attr);
    if (rc)
      goto out;

out:
	COMPLETE_HW_WRITE
	return rc;
}
*/

/**
 */

/*
int set_hw_port_sms_attr_qmap_pars(tm_handle hndl, uint8_t index)
{
    int rc =  -EFAULT;
	(void) hndl;
	(void) index;
  check if it exists in SN
	struct TM_RCB_SMSPortAttrQmapPars q_map ;

   	TM_CTL(ctl, hndl)

    q_map.ParseMode = ctl->tm_port_sms_attr_qmap_pars[index].mode;
    q_map.BaseQ = ctl->tm_port_sms_attr_qmap_pars[index].base_q;
    q_map.DfltColor = ctl->tm_port_sms_attr_qmap_pars[index].dcolor;
    // debug fields
    q_map.EType = 0x8100;
    q_map.QmapStrip = 0;
    q_map.Offs = 0;
	rc = tm_table_entry_write(TM_ENV(ctl), (void*)&TM.RCB.SMSPortAttrQmapPars, index, &q_map);
    if (rc)
      goto out;

out:
	COMPLETE_HW_WRITE
	return rc;
}
*/

int set_hw_dp_source(tm_handle hndl)
{
 	int rc=0;
    TM_REGISTER_VAR(TM_Drop_DPSource)

    TM_CTL(ctl, hndl)
	
	/* reset register reserved fields */
    TM_RESET_REGISTER(TM.Drop.DPSource, TM_Drop_DPSource)
	if (rc) goto out;
	/* assign register fields */		
    TM_REGISTER_SET(TM_Drop_DPSource, PortSrc , ctl->dp_unit.local[P_LEVEL].dp_src[0] + ctl->dp_unit.local[P_LEVEL].dp_src[1]*2 + ctl->dp_unit.local[P_LEVEL].dp_src[2]*4)
    TM_REGISTER_SET(TM_Drop_DPSource, ClvlSrc , ctl->dp_unit.local[C_LEVEL].dp_src[0] + ctl->dp_unit.local[C_LEVEL].dp_src[1]*2 + ctl->dp_unit.local[C_LEVEL].dp_src[2]*4)
    TM_REGISTER_SET(TM_Drop_DPSource, BlvlSrc , ctl->dp_unit.local[B_LEVEL].dp_src[0] + ctl->dp_unit.local[B_LEVEL].dp_src[1]*2 + ctl->dp_unit.local[B_LEVEL].dp_src[2]*4)
    TM_REGISTER_SET(TM_Drop_DPSource, AlvlSrc , ctl->dp_unit.local[A_LEVEL].dp_src[0] + ctl->dp_unit.local[A_LEVEL].dp_src[1]*2 + ctl->dp_unit.local[A_LEVEL].dp_src[2]*4)
    TM_REGISTER_SET(TM_Drop_DPSource, QueueSrc, ctl->dp_unit.local[Q_LEVEL].dp_src[0] + ctl->dp_unit.local[Q_LEVEL].dp_src[1]*2 + ctl->dp_unit.local[Q_LEVEL].dp_src[2]*4)
 	/* write register */
    TM_WRITE_REGISTER(TM.Drop.DPSource, TM_Drop_DPSource)
    if (rc) goto out;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int get_hw_dp_source(tm_handle hndl)
{
 	int rc=0;
	uint8_t value = 0;

    TM_REGISTER_VAR(TM_Drop_DPSource)

    TM_CTL(ctl, hndl)
	
    TM_READ_REGISTER(TM.Drop.DPSource, TM_Drop_DPSource)
    if (rc) goto out;
	/* assign register fields */
    TM_REGISTER_GET(TM_Drop_DPSource, PortSrc , value, (uint8_t) )
    ctl->dp_unit.local[P_LEVEL].dp_src[0] = value & 1;
	ctl->dp_unit.local[P_LEVEL].dp_src[1] = (value & 2) >> 1;
	ctl->dp_unit.local[P_LEVEL].dp_src[2] = (value & 4) >> 2;
    TM_REGISTER_GET(TM_Drop_DPSource, ClvlSrc , value, (uint8_t) )
    ctl->dp_unit.local[C_LEVEL].dp_src[0] = value & 1;
	ctl->dp_unit.local[C_LEVEL].dp_src[1] = (value & 2) >> 1;
	ctl->dp_unit.local[C_LEVEL].dp_src[2] = (value & 4) >> 2;
    TM_REGISTER_GET(TM_Drop_DPSource, BlvlSrc , value, (uint8_t) )
    ctl->dp_unit.local[B_LEVEL].dp_src[0] = value & 1;
	ctl->dp_unit.local[B_LEVEL].dp_src[1] = (value & 2) >> 1;
	ctl->dp_unit.local[B_LEVEL].dp_src[2] = (value & 4) >> 2;
    TM_REGISTER_GET(TM_Drop_DPSource, AlvlSrc , value, (uint8_t) )
    ctl->dp_unit.local[A_LEVEL].dp_src[0] = value & 1;
	ctl->dp_unit.local[A_LEVEL].dp_src[1] = (value & 2) >> 1;
	ctl->dp_unit.local[A_LEVEL].dp_src[2] = (value & 4) >> 2;
    TM_REGISTER_GET(TM_Drop_DPSource, QueueSrc , value, (uint8_t) )
    ctl->dp_unit.local[Q_LEVEL].dp_src[0] = value & 1;
	ctl->dp_unit.local[Q_LEVEL].dp_src[1] = (value & 2) >> 1;
	ctl->dp_unit.local[Q_LEVEL].dp_src[2] = (value & 4) >> 2;
out:
    COMPLETE_HW_WRITE
	return rc;
}

int set_hw_queue_cos(tm_handle hndl, uint32_t index, uint8_t cos)
{
	int rc=0;
    uint32_t entry = index/4;
    uint32_t offset = index% 4;

    TM_REGISTER_VAR(TM_Drop_QueueCoSConf)

    TM_CTL(ctl, hndl)
    if (index < ctl->tm_total_queues)
	{
		/* not necessary to reset register -  it content is readed from H/W */
		TM_READ_TABLE_REGISTER(TM.Drop.QueueCoSConf, entry, TM_Drop_QueueCoSConf)
		if (rc)	  goto out;
		switch (offset)
		{
			case 0 : TM_REGISTER_SET(TM_Drop_QueueCoSConf, QueueCos0,cos); break;
			case 1 : TM_REGISTER_SET(TM_Drop_QueueCoSConf, QueueCos1,cos); break;
			case 2 : TM_REGISTER_SET(TM_Drop_QueueCoSConf, QueueCos2,cos); break;
			case 3 : TM_REGISTER_SET(TM_Drop_QueueCoSConf, QueueCos3,cos); break;
			default : break;
		}
		TM_WRITE_TABLE_REGISTER(TM.Drop.QueueCoSConf, entry, TM_Drop_QueueCoSConf)
	}
out:
    COMPLETE_HW_WRITE
    return rc;
}
int get_hw_queue_cos(tm_handle hndl, uint32_t index, uint8_t * cos)
{
	int rc=0;
    uint32_t entry = index/4;
    uint32_t offset = index% 4;

    TM_REGISTER_VAR(TM_Drop_QueueCoSConf)

    TM_CTL(ctl, hndl)
    if (index < ctl->tm_total_queues)
	{
		TM_READ_TABLE_REGISTER(TM.Drop.QueueCoSConf, entry, TM_Drop_QueueCoSConf)
		if (rc)	  goto out;
		switch (offset)
		{
			case 0 : TM_REGISTER_GET(TM_Drop_QueueCoSConf, QueueCos0, *cos, (uint8_t)); break;
			case 1 : TM_REGISTER_GET(TM_Drop_QueueCoSConf, QueueCos1, *cos, (uint8_t)); break;
			case 2 : TM_REGISTER_GET(TM_Drop_QueueCoSConf, QueueCos2, *cos, (uint8_t)); break;
			case 3 : TM_REGISTER_GET(TM_Drop_QueueCoSConf, QueueCos3, *cos, (uint8_t)); break;
			default : break;
		}
	}
out:
    COMPLETE_HW_WRITE
    return rc;
}











int get_hw_gen_params(tm_handle hndl)
{
	int rc;
    TM_REGISTER_VAR(TM_QMgr_MinPkgSize)
    TM_REGISTER_VAR(TM_QMgr_PortChunksEmitPerSel)

    TM_CTL(ctl, hndl)

	/* Read minimum package size in bytes (not in Chunk unit) */
	TM_READ_REGISTER(TM.QMgr.MinPkgSize,  TM_QMgr_MinPkgSize)
    if (rc)  return rc;
	TM_REGISTER_GET(TM_QMgr_MinPkgSize, Size , ctl->min_pkg_size, (uint16_t) ) /* no casting */
	ctl->min_pkg_size *= 64;

   /* Read Port Chuncks Emit */
 	TM_READ_REGISTER(TM.QMgr.PortChunksEmitPerSel,  TM_QMgr_PortChunksEmitPerSel)
    if (rc)
      return rc;
	TM_REGISTER_GET(TM_QMgr_PortChunksEmitPerSel, Chunks , ctl->port_ch_emit , (uint8_t) )
    return rc;
}


/**
 */
int get_hw_port_status(tm_handle hndl,
				     uint8_t index,
				     struct tm_port_status *tm_status)
{  
	int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_PortShpBucketLvls)
    TM_REGISTER_VAR(TM_Sched_PortDefPrioHi)
    TM_REGISTER_VAR(TM_Sched_PortDefPrioLo)

    TM_CTL(ctl, hndl)

	TM_READ_TABLE_REGISTER(TM.Sched.PortShpBucketLvls, index , TM_Sched_PortShpBucketLvls)
	if (rc)	return rc;

	TM_REGISTER_GET(TM_Sched_PortShpBucketLvls, MinLvl , tm_status->min_bucket_level , (uint32_t) ) /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_PortShpBucketLvls, MaxLvl , tm_status->max_bucket_level , (uint32_t) )  /* casting to uint32_t */

	TM_READ_TABLE_REGISTER(TM.Sched.PortDefPrioHi, index , TM_Sched_PortDefPrioHi)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_PortDefPrioHi, Deficit0 , tm_status->deficit[0]  ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_PortDefPrioHi, Deficit1 , tm_status->deficit[1]  ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_PortDefPrioHi, Deficit2 , tm_status->deficit[2]  ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_PortDefPrioHi, Deficit3 , tm_status->deficit[3]  ,  (uint32_t) )  /* casting to uint32_t */

	TM_READ_TABLE_REGISTER(TM.Sched.PortDefPrioLo, index , TM_Sched_PortDefPrioLo)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_PortDefPrioLo, Deficit0 , tm_status->deficit[4]  ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_PortDefPrioLo, Deficit1 , tm_status->deficit[5]  ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_PortDefPrioLo, Deficit2 , tm_status->deficit[6]  ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_PortDefPrioLo, Deficit3 , tm_status->deficit[7]  ,  (uint32_t) )  /* casting to uint32_t */
	return rc;
}


/**
 */
int get_hw_c_node_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status)
{  
	int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_ClvlShpBucketLvls)
    TM_REGISTER_VAR(TM_Sched_CLvlDef)
	
    TM_CTL(ctl, hndl)

	TM_READ_TABLE_REGISTER(TM.Sched.ClvlShpBucketLvls, index , TM_Sched_ClvlShpBucketLvls)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_ClvlShpBucketLvls, MinLvl , tm_status->min_bucket_level ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_ClvlShpBucketLvls, MaxLvl , tm_status->max_bucket_level ,  (uint32_t) )  /* casting to uint32_t */

	TM_READ_TABLE_REGISTER(TM.Sched.CLvlDef, index , TM_Sched_CLvlDef)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_CLvlDef, Deficit, tm_status->deficit ,  (uint32_t) )  /* casting to uint32_t */
	return rc;
}


/**
 */
int get_hw_b_node_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status)
{  
	int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_BlvlShpBucketLvls)
    TM_REGISTER_VAR(TM_Sched_BlvlDef)
	
    TM_CTL(ctl, hndl)

	TM_READ_TABLE_REGISTER(TM.Sched.BlvlShpBucketLvls, index , TM_Sched_BlvlShpBucketLvls)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_BlvlShpBucketLvls, MinLvl , tm_status->min_bucket_level ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_BlvlShpBucketLvls, MaxLvl , tm_status->max_bucket_level ,  (uint32_t) )  /* casting to uint32_t */

	TM_READ_TABLE_REGISTER(TM.Sched.BlvlDef, index , TM_Sched_BlvlDef)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_BlvlDef, Deficit, tm_status->deficit ,  (uint32_t) )  /* casting to uint32_t */
	return rc;
}


/**
 */
int get_hw_a_node_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status)
{  
	int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_AlvlShpBucketLvls)
    TM_REGISTER_VAR(TM_Sched_AlvlDef)
	
    TM_CTL(ctl, hndl)

	TM_READ_TABLE_REGISTER(TM.Sched.AlvlShpBucketLvls, index , TM_Sched_AlvlShpBucketLvls)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_AlvlShpBucketLvls, MinLvl , tm_status->min_bucket_level ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_AlvlShpBucketLvls, MaxLvl , tm_status->max_bucket_level ,  (uint32_t) )  /* casting to uint32_t */
	TM_READ_TABLE_REGISTER(TM.Sched.AlvlDef, index , TM_Sched_AlvlDef)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_AlvlDef, Deficit, tm_status->deficit ,  (uint32_t) )  /* casting to uint32_t */
	return rc;
}


/**
 */
int get_hw_queue_status(tm_handle hndl,
				       uint32_t index,
				       struct tm_node_status *tm_status)
{  
	int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_QueueShpBucketLvls)
    TM_REGISTER_VAR(TM_Sched_QueueDef)

    TM_CTL(ctl, hndl)
	
	TM_READ_TABLE_REGISTER(TM.Sched.QueueShpBucketLvls, index , TM_Sched_QueueShpBucketLvls)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_QueueShpBucketLvls, MinLvl , tm_status->min_bucket_level ,  (uint32_t) )  /* casting to uint32_t */
	TM_REGISTER_GET(TM_Sched_QueueShpBucketLvls, MaxLvl , tm_status->max_bucket_level ,  (uint32_t) )  /* casting to uint32_t */

	TM_READ_TABLE_REGISTER(TM.Sched.QueueDef, index , TM_Sched_QueueDef)
	if (rc)
		return rc;
	TM_REGISTER_GET(TM_Sched_QueueDef, Deficit, tm_status->deficit ,  (uint32_t) )  /* casting to uint32_t */
	return rc;
}


/**
 */
int get_hw_queue_length(tm_handle hndl,
				      enum tm_level level,
				      uint32_t index,
				      uint32_t * inst_q_length,
				      uint32_t * av_q_length)
{  
	int rc =  -EFAULT;

	TM_REGISTER_VAR(TM_Drop_QueueAvgQueueLength)		/* average */
	TM_REGISTER_VAR(TM_QMgr_QueueLength)				/* instant */
    TM_REGISTER_VAR(TM_Drop_AlvlInstAndAvgQueueLength)
    TM_REGISTER_VAR(TM_Drop_BlvlInstAndAvgQueueLength)
    TM_REGISTER_VAR(TM_Drop_ClvlInstAndAvgQueueLength)
    TM_REGISTER_VAR(TM_Drop_PortInstAndAvgQueueLength)

    TM_CTL(ctl, hndl)

	switch (level)
	{
	case Q_LEVEL:
		TM_READ_TABLE_REGISTER(TM.Drop.QueueAvgQueueLength,index,TM_Drop_QueueAvgQueueLength)
		if (rc)
			return rc;
		TM_REGISTER_GET(TM_Drop_QueueAvgQueueLength, AQL , *av_q_length ,  (uint32_t) )  /* casting to uint32_t */

		TM_READ_TABLE_REGISTER(TM.QMgr.QueueLength,index,TM_QMgr_QueueLength)
		if (rc)
			return rc;
		TM_REGISTER_GET(TM_QMgr_QueueLength, Len , *inst_q_length ,  (uint32_t) )  /* casting to uint32_t */
		break;
	case A_LEVEL:
		TM_READ_TABLE_REGISTER(TM.Drop.AlvlInstAndAvgQueueLength,index,TM_Drop_AlvlInstAndAvgQueueLength)
		if (rc)
			return rc;
		TM_REGISTER_GET(TM_Drop_AlvlInstAndAvgQueueLength, QL , *inst_q_length ,  (uint32_t) )  /* casting to uint32_t */
		TM_REGISTER_GET(TM_Drop_AlvlInstAndAvgQueueLength, AQL , *av_q_length ,  (uint32_t) )  /* casting to uint32_t */
		break;
	case B_LEVEL:
		TM_READ_TABLE_REGISTER(TM.Drop.BlvlInstAndAvgQueueLength,index,TM_Drop_BlvlInstAndAvgQueueLength)
		if (rc)
			return rc;
		TM_REGISTER_GET(TM_Drop_BlvlInstAndAvgQueueLength, QL , *inst_q_length ,  (uint32_t) )  /* casting to uint32_t */
		TM_REGISTER_GET(TM_Drop_BlvlInstAndAvgQueueLength, AQL , *av_q_length ,  (uint32_t) )  /* casting to uint32_t */
		break;
	case C_LEVEL:
		TM_READ_TABLE_REGISTER(TM.Drop.ClvlInstAndAvgQueueLength,index,TM_Drop_ClvlInstAndAvgQueueLength)
		if (rc)
			return rc;
		TM_REGISTER_GET(TM_Drop_ClvlInstAndAvgQueueLength, QL , *inst_q_length ,  (uint32_t) )  /* casting to uint32_t */
		TM_REGISTER_GET(TM_Drop_ClvlInstAndAvgQueueLength, AQL , *av_q_length ,  (uint32_t) )  /* casting to uint32_t */
	case P_LEVEL:
		TM_READ_TABLE_REGISTER(TM.Drop.PortInstAndAvgQueueLength,index,TM_Drop_PortInstAndAvgQueueLength)
		if (rc)
			return rc;
		TM_REGISTER_GET(TM_Drop_PortInstAndAvgQueueLength, QL , *inst_q_length ,  (uint32_t) )  /* casting to uint32_t */
		TM_REGISTER_GET(TM_Drop_PortInstAndAvgQueueLength, AQL , *av_q_length ,  (uint32_t) )  /* casting to uint32_t */
		break;
	}
	return rc;
}


/**
 */
int get_hw_qmr_errors(tm_handle hndl, struct tm_error_info *info)
{
    int rc =  -EFAULT;
    TM_REGISTER_VAR(TM_QMgr_ErrCnt)
    TM_REGISTER_VAR(TM_QMgr_ExcCnt)

    TM_CTL(ctl, hndl)

    TM_READ_REGISTER(TM.QMgr.ErrCnt , TM_QMgr_ErrCnt)
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_QMgr_ErrCnt , Cnt , info->error_counter ,  (uint16_t) )  /* casting to uint16_t */

    TM_READ_REGISTER(TM.QMgr.ExcCnt , TM_QMgr_ExcCnt);
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_QMgr_ExcCnt, Cnt , info->exception_counter , (uint16_t) )  /* casting to uint16_t */

	return rc;
}


/**
 */

int get_hw_bap_errors(tm_handle hndl, struct tm_error_info *info)
{
    int rc =  -EFAULT;
    int i;

    TM_REGISTER_VAR(TM_BAP_ErrCnt)
    TM_REGISTER_VAR(TM_BAP_ExcCnt)

    TM_CTL(ctl, hndl)

    for(i=0; i<4; i++)
    {
	TM_READ_REGISTER(TM.BAP[i].ErrCnt, TM_BAP_ErrCnt)
        if (rc)
	    return rc;
	TM_REGISTER_GET(TM_BAP_ErrCnt , Cnt , info[i].error_counter ,(uint16_t) )  /* casting to uint16_t */
    }
    for(i=0; i<4;i++)
    {
 	TM_READ_REGISTER(TM.BAP[i].ExcCnt, TM_BAP_ExcCnt);
        if (rc)
	    return rc;
	TM_REGISTER_GET(TM_BAP_ExcCnt , Cnt , info[i].exception_counter , (uint16_t) )  /* casting to uint16_t */
    }
    return rc;
}


/**
 */
int get_hw_rcb_errors(tm_handle hndl, struct tm_error_info *info)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_RCB_ErrCnt)
    TM_REGISTER_VAR(TM_RCB_ExcCnt)

    TM_CTL(ctl, hndl);

    TM_READ_REGISTER(TM.RCB.ErrCnt , TM_RCB_ErrCnt)
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_RCB_ErrCnt , Cnt , info->error_counter ,(uint16_t) )  /* casting to uint16_t */

    TM_READ_REGISTER(TM.RCB.ExcCnt , TM_RCB_ExcCnt);
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_RCB_ExcCnt, Cnt , info->exception_counter ,(uint16_t) )  /* casting to uint16_t */
 
    return rc;
}


/**
 */
int get_hw_sched_errors(tm_handle hndl, struct tm_error_info *info)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Sched_ErrCnt)
    TM_REGISTER_VAR(TM_Sched_ExcCnt)

    TM_CTL(ctl, hndl)

    TM_READ_REGISTER(TM.Sched.ErrCnt , TM_Sched_ErrCnt)
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_Sched_ErrCnt , Cnt , info->error_counter ,(uint16_t) )  /* casting to uint16_t */

    TM_READ_REGISTER(TM.Sched.ExcCnt , TM_Sched_ExcCnt)
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_Sched_ExcCnt, Cnt , info->exception_counter ,(uint16_t) )  /* casting to uint16_t */

    return rc;
}


/**
 */
int get_hw_drop_errors(tm_handle hndl, struct tm_error_info *info)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_ErrCnt)
    TM_REGISTER_VAR(TM_Drop_ExcCnt)

    TM_CTL(ctl, hndl)

    TM_READ_REGISTER(TM.Drop.ErrCnt , TM_Drop_ErrCnt)
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_Drop_ErrCnt , Cnt , info->error_counter ,(uint16_t) )  /* casting to uint16_t */

    TM_READ_REGISTER(TM.Drop.ExcCnt,TM_Drop_ExcCnt)
    if (rc)
        return rc;
    TM_REGISTER_GET(TM_Drop_ExcCnt, Cnt , info->exception_counter ,(uint16_t) )  /* casting to uint16t */
	return rc;
}

/** 
 */ 
int get_hw_tm_units_status(tm_handle hndl, struct tm_units_error_status_info *units_error)
{
	int rc =  -EFAULT;	
	uint32_t data[2];

	TM_CTL(ctl, hndl)

    tm_memset(units_error, 0, sizeof(struct tm_units_error_status_info));

	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.QMgr.ErrStus), data);
	if (rc)
		return rc;

    /*tm_printf("TM.QMgr.ErrStus: 0x%0x 0x%0x\n", data[0], data[1]);*/

	units_error->qmgr_status = data[0];

	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.Sched.ErrStus), data);
	if (rc)
		return rc;

	units_error->sched_status = data[0];

	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.Drop.ErrStus), data);
	if (rc)
		return rc;

	units_error->drop_status = data[0];

	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.RCB.ErrStus), data);
	if (rc)
		return rc;

	units_error->rcb_status = data[0];

	return rc;
}

/** 
 */ 
int get_hw_tm_bap_unit_status(tm_handle hndl, int bap_num, struct tm_bap_unit_error_status_info *bap_unit_errors)
{
	int rc =  -EFAULT;	
	uint32_t data[2];

	TM_CTL(ctl, hndl)

	if (bap_num >= TM_MAX_BAP)
	{
		return rc;
	}

    tm_memset(bap_unit_errors, 0, sizeof(struct tm_bap_unit_error_status_info));

	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.BAP[bap_num].ErrStus), data);
	if (rc)
		return rc;

	bap_unit_errors->bap_status = data[0];

	return rc;
}


/**
 */
int get_hw_qmr_num_pkts_to_unins_queue(tm_handle hndl, void * counter_address)
{
	int rc =  -EFAULT;
    TM_CTL(ctl, hndl)
	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.QMgr.UninstalledQueueDrpPktCnt), counter_address);
	return rc;
}

/**
 */


int get_hw_rcb_get_num_pkts_to_sms(tm_handle hndl,  void * counter_address)
{
	int rc =  -EFAULT;
    TM_CTL(ctl, hndl)
	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.RCB.NonErrToSMSPktCnt), counter_address);
	return rc;
}


int get_hw_rcb_get_num_num_crc_err_pkts_to_sms(tm_handle hndl,   void * counter_address)
{
	int rc =  -EFAULT;
    TM_CTL(ctl, hndl)
	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.RCB.CRCErrPktCnt), counter_address);
	return rc;
}

int get_hw_rcb_get_num_errs_from_sms_to_dram(tm_handle hndl,  void * counter_address)
{
	int rc =  -EFAULT;
    TM_CTL(ctl, hndl)
	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.RCB.SMSErrPasPktCnt), counter_address);
	return rc;
}

int get_hw_rcb_get_num_port_flush_drp_pkts(tm_handle hndl,  void * counter_address)
{
	int rc =  -EFAULT;
    TM_CTL(ctl, hndl)
	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(TM.RCB.PortFlushDrpDataPktCnt), counter_address);
	return rc;
}


/**
 * get A to P level Eligible Priority Function
 */
int tm_get_hw_elig_prio_func(tm_handle hndl, enum tm_level level, uint16_t func_offset, struct tm_elig_prio_func_node *params)
{
	int rc = -EFAULT;
	int i;

	TM_REGISTER_VAR(TM_Sched_AlvlEligPrioFunc_Entry)
	TM_REGISTER_VAR(TM_Sched_BlvlEligPrioFunc_Entry)
	TM_REGISTER_VAR(TM_Sched_ClvlEligPrioFunc_Entry)
	TM_REGISTER_VAR(TM_Sched_PortEligPrioFunc_Entry)

    TM_CTL(ctl, hndl)

	switch (level) {
		case Q_LEVEL:
			rc = -EFAULT;
			break;
		case A_LEVEL:
			for (i = 0; i < 8; i++) {
				TM_READ_TABLE_REGISTER(TM.Sched.AlvlEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_AlvlEligPrioFunc_Entry)
				if (rc)	goto out;
				TM_REGISTER_GET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut0 , params->tbl_entry[i].func_out[0], (uint16_t))
				TM_REGISTER_GET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut1 , params->tbl_entry[i].func_out[1], (uint16_t)) 
				TM_REGISTER_GET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut2 , params->tbl_entry[i].func_out[2], (uint16_t)) 
				TM_REGISTER_GET(TM_Sched_AlvlEligPrioFunc_Entry , FuncOut3 , params->tbl_entry[i].func_out[3], (uint16_t)) 
			}
			break;
		case B_LEVEL:
			for (i = 0; i < 8; i++) {
				TM_READ_TABLE_REGISTER(TM.Sched.BlvlEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_BlvlEligPrioFunc_Entry)
				if (rc)	goto out;
				TM_REGISTER_GET(TM_Sched_BlvlEligPrioFunc_Entry, FuncOut0, params->tbl_entry[i].func_out[0], (uint16_t));
				TM_REGISTER_GET(TM_Sched_BlvlEligPrioFunc_Entry, FuncOut1, params->tbl_entry[i].func_out[1], (uint16_t));
				TM_REGISTER_GET(TM_Sched_BlvlEligPrioFunc_Entry, FuncOut2, params->tbl_entry[i].func_out[2], (uint16_t));
				TM_REGISTER_GET(TM_Sched_BlvlEligPrioFunc_Entry, FuncOut3, params->tbl_entry[i].func_out[3], (uint16_t));
			}
			break;
		case C_LEVEL:
			for (i = 0; i < 8; i++) {
				TM_READ_TABLE_REGISTER(TM.Sched.ClvlEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_ClvlEligPrioFunc_Entry)
				if (rc) goto out;
				TM_REGISTER_GET(TM_Sched_ClvlEligPrioFunc_Entry, FuncOut0, params->tbl_entry[i].func_out[0], (uint16_t));
				TM_REGISTER_GET(TM_Sched_ClvlEligPrioFunc_Entry, FuncOut1, params->tbl_entry[i].func_out[1], (uint16_t));
				TM_REGISTER_GET(TM_Sched_ClvlEligPrioFunc_Entry, FuncOut2, params->tbl_entry[i].func_out[2], (uint16_t));
				TM_REGISTER_GET(TM_Sched_ClvlEligPrioFunc_Entry, FuncOut3, params->tbl_entry[i].func_out[3], (uint16_t));
			}
			break;
		case P_LEVEL:
			for (i = 0; i < 8; i++) {
				TM_READ_TABLE_REGISTER(TM.Sched.PortEligPrioFunc_Entry, func_offset + i*64 ,TM_Sched_PortEligPrioFunc_Entry)
				if (rc) goto out;
				TM_REGISTER_GET(TM_Sched_PortEligPrioFunc_Entry, FuncOut0, params->tbl_entry[i].func_out[0], (uint16_t));
				TM_REGISTER_GET(TM_Sched_PortEligPrioFunc_Entry, FuncOut1, params->tbl_entry[i].func_out[1], (uint16_t));
				TM_REGISTER_GET(TM_Sched_PortEligPrioFunc_Entry, FuncOut2, params->tbl_entry[i].func_out[2], (uint16_t));
				TM_REGISTER_GET(TM_Sched_PortEligPrioFunc_Entry, FuncOut3, params->tbl_entry[i].func_out[3], (uint16_t));
			}
			break;
		default:
			goto out;
		}
out:
	return rc;
}

/**
 * get q level Eligible Priority Function
 */
int tm_get_hw_elig_prio_q_func(tm_handle hndl, uint16_t func_offset, struct tm_elig_prio_func_queue *params)
{
	int rc = -EFAULT;

	TM_REGISTER_VAR(TM_Sched_QueueEligPrioFunc)

    TM_CTL(ctl, hndl);

	TM_READ_TABLE_REGISTER(TM.Sched.QueueEligPrioFunc, func_offset  ,TM_Sched_QueueEligPrioFunc)
	if (rc)	goto out;
	TM_REGISTER_GET(TM_Sched_QueueEligPrioFunc, FuncOut0, params->tbl_entry.func_out[0], (uint16_t));
	TM_REGISTER_GET(TM_Sched_QueueEligPrioFunc, FuncOut1, params->tbl_entry.func_out[1], (uint16_t));
	TM_REGISTER_GET(TM_Sched_QueueEligPrioFunc, FuncOut2, params->tbl_entry.func_out[2], (uint16_t));
	TM_REGISTER_GET(TM_Sched_QueueEligPrioFunc, FuncOut3, params->tbl_entry.func_out[3], (uint16_t));

out:
	return rc;
}
/**
 * get drop profile number for Q/A/B/C nodes from H/W
 */

int tm_get_hw_queue_drop_profile_ptr(tm_handle hndl, uint32_t queue_ind, uint16_t *queue_profile_ptr)
{	
    int rc =  -EADDRNOTAVAIL;

	int entry;
    int offset;
    TM_REGISTER_VAR(TM_Drop_QueueDropProfPtr)

    TM_CTL(ctl, hndl)
	if (queue_ind < ctl->tm_total_queues)
	{
		/* Entry in the table */
		entry = queue_ind/TM_Q_DRP_PROF_PER_ENTRY;
		offset = queue_ind % TM_Q_DRP_PROF_PER_ENTRY;
		/* read register */
		TM_READ_TABLE_REGISTER(TM.Drop.QueueDropProfPtr , entry , TM_Drop_QueueDropProfPtr)
		if (rc) goto out;
		/* read register field */		
		switch (offset) 
		{
			case 0:	TM_REGISTER_GET(TM_Drop_QueueDropProfPtr , ProfPtr0 , *queue_profile_ptr, (uint16_t)) ; break;
			case 1:	TM_REGISTER_GET(TM_Drop_QueueDropProfPtr , ProfPtr1 , *queue_profile_ptr, (uint16_t)) ; break;
			case 2:	TM_REGISTER_GET(TM_Drop_QueueDropProfPtr , ProfPtr2 , *queue_profile_ptr, (uint16_t)) ; break;
			default:
			case 3:	TM_REGISTER_GET(TM_Drop_QueueDropProfPtr , ProfPtr3 , *queue_profile_ptr, (uint16_t)) ; break;
		}
	}
out:
	return rc;
}


int tm_get_hw_a_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind, uint16_t * profile_ptr)
{
    int rc =  -EFAULT;

	int entry;
    int offset;
    TM_REGISTER_VAR(TM_Drop_AlvlDropProfPtr)

    TM_CTL(ctl, hndl)

    if (node_ind < ctl->tm_total_a_nodes)
	{
		/* Entry in the table */
		entry  = node_ind / TM_A_DRP_PROF_PER_ENTRY;
		offset = node_ind % TM_A_DRP_PROF_PER_ENTRY;
		/* read register */
		TM_READ_TABLE_REGISTER(TM.Drop.AlvlDropProfPtr , entry , TM_Drop_AlvlDropProfPtr);
		if (rc) goto out;
		/* read register field */
		switch(offset)
		{
			case 0: TM_REGISTER_GET(TM_Drop_AlvlDropProfPtr , ProfPtr0 , * profile_ptr, (uint16_t)); break;
			case 1: TM_REGISTER_GET(TM_Drop_AlvlDropProfPtr , ProfPtr1 , * profile_ptr, (uint16_t)); break;
			case 2: TM_REGISTER_GET(TM_Drop_AlvlDropProfPtr , ProfPtr2 , * profile_ptr, (uint16_t)); break;
			default:
			case 3: TM_REGISTER_GET(TM_Drop_AlvlDropProfPtr , ProfPtr3 , * profile_ptr, (uint16_t)); break;
		}
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int tm_get_hw_b_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind, uint16_t * profile_ptr)
{
    int rc =  -EFAULT;
    int entry;
    int offset;

    TM_REGISTER_VAR(TM_Drop_BlvlDropProfPtr)

    TM_CTL(ctl, hndl)

	if (node_ind < ctl->tm_total_b_nodes)
	{
		/* Entry in the table */
		entry  = node_ind / TM_B_DRP_PROF_PER_ENTRY;
		offset = node_ind % TM_B_DRP_PROF_PER_ENTRY;
		/* read register*/
		TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropProfPtr , entry , TM_Drop_BlvlDropProfPtr);
		if (rc) goto out;
		/* read register field */
		switch(offset)
		{
			case 0: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr0 , * profile_ptr, (uint16_t)); break;
			case 1: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr1 , * profile_ptr, (uint16_t)); break;
			case 2: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr2 , * profile_ptr, (uint16_t)); break;
			case 3: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr3 , * profile_ptr, (uint16_t)); break;
			case 4: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr4 , * profile_ptr, (uint16_t)); break;
			case 5: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr5 , * profile_ptr, (uint16_t)); break;
			case 6: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr6 , * profile_ptr, (uint16_t)); break;
			default:
			case 7: TM_REGISTER_GET(TM_Drop_BlvlDropProfPtr , ProfPtr7 , * profile_ptr, (uint16_t)); break;
		}
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int tm_get_hw_c_node_drop_profile_ptr(tm_handle hndl, uint32_t node_ind , uint8_t cos, uint16_t * profile_ptr)
{
    int rc =  -EFAULT;
    int entry;
    int offset;

    TM_REGISTER_VAR(TM_Drop_ClvlDropProfPtr_CoS)

    TM_CTL(ctl, hndl)

	if ((node_ind < ctl->tm_total_c_nodes) && (cos < TM_WRED_COS))
	{
		/* Entry in the table */
		entry  = node_ind / TM_C_DRP_PROF_PER_ENTRY;
		offset = node_ind % TM_C_DRP_PROF_PER_ENTRY;
		/* read register*/
		TM_READ_TABLE_REGISTER(TM.Drop.ClvlDropProfPtr_CoS[cos] , entry , TM_Drop_ClvlDropProfPtr_CoS)
		if (rc) goto out;
		/* read register field */
		switch (offset)
		{
			case 0: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr0 , * profile_ptr, (uint16_t)); break;
			case 1: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr1 , * profile_ptr, (uint16_t)); break;
			case 2: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr2 , * profile_ptr, (uint16_t)); break;
			case 3: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr3 , * profile_ptr, (uint16_t)); break;
			case 4: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr4 , * profile_ptr, (uint16_t)); break;
			case 5: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr5 , * profile_ptr, (uint16_t)); break;
			case 6: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr6 , * profile_ptr, (uint16_t)); break;
			default:
			case 7: TM_REGISTER_GET(TM_Drop_ClvlDropProfPtr_CoS , ProfPtr7 , * profile_ptr, (uint16_t)); break;
		}
	}
out:
    COMPLETE_HW_WRITE
	return rc;
}

int tm_get_hw_queue_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile)
{
    int rc =  -EFAULT;	

    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDParams)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDScaleRatio)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDMinThresh)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfWREDDPRatio)
    TM_REGISTER_VAR(TM_Drop_QueueDropPrfTailDrpThresh)

    TM_CTL(ctl, hndl)

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDParams , prof_ind , TM_Drop_QueueDropPrfWREDParams)
    if (rc) goto out;
    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , AQLExp , profile->aql_exp, (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , ColorTDEn , profile->color_td_en, (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , ScaleExpColor0 , profile->scale_exp[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , ScaleExpColor1 , profile->scale_exp[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , ScaleExpColor2 , profile->scale_exp[2], (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , CurveIndexColor0 , profile->curve_id[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , CurveIndexColor1 , profile->curve_id[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDParams , CurveIndexColor2 , profile->curve_id[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDScaleRatio, prof_ind, TM_Drop_QueueDropPrfWREDScaleRatio)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDScaleRatio , ScaleRatioColor0 , profile->scale_ratio[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDScaleRatio , ScaleRatioColor1 , profile->scale_ratio[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDScaleRatio , ScaleRatioColor2 , profile->scale_ratio[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDMinThresh, prof_ind,TM_Drop_QueueDropPrfWREDMinThresh)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDMinThresh , MinTHColor0 , profile->min_threshold[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDMinThresh , MinTHColor1 , profile->min_threshold[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDMinThresh , MinTHColor2 , profile->min_threshold[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.QueueDropPrfWREDDPRatio, prof_ind, TM_Drop_QueueDropPrfWREDDPRatio)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDDPRatio, DPRatio0, profile->dp_ratio[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDDPRatio, DPRatio1, profile->dp_ratio[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfWREDDPRatio, DPRatio2, profile->dp_ratio[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.QueueDropPrfTailDrpThresh, prof_ind, TM_Drop_QueueDropPrfTailDrpThresh)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_QueueDropPrfTailDrpThresh , TailDropThreshRes , profile->td_thresh_res, (uint8_t))
    TM_REGISTER_GET(TM_Drop_QueueDropPrfTailDrpThresh , TailDropThresh , profile->td_threshold_bursts, (uint32_t))
out:
    return rc;
} 

int tm_get_hw_a_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDParams)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDScaleRatio)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDMinThresh)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfWREDDPRatio)
    TM_REGISTER_VAR(TM_Drop_AlvlDropPrfTailDrpThresh)

    TM_CTL(ctl, hndl)

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDParams , prof_ind , TM_Drop_AlvlDropPrfWREDParams)
    if (rc) goto out;
    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , AQLExp , profile->aql_exp, (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , ColorTDEn , profile->color_td_en, (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , ScaleExpColor0 , profile->scale_exp[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , ScaleExpColor1 , profile->scale_exp[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , ScaleExpColor2 , profile->scale_exp[2], (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , CurveIndexColor0 , profile->curve_id[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , CurveIndexColor1 , profile->curve_id[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDParams , CurveIndexColor2 , profile->curve_id[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDScaleRatio, prof_ind, TM_Drop_AlvlDropPrfWREDScaleRatio)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDScaleRatio , ScaleRatioColor0 , profile->scale_ratio[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDScaleRatio , ScaleRatioColor1 , profile->scale_ratio[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDScaleRatio , ScaleRatioColor2 , profile->scale_ratio[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDMinThresh, prof_ind,TM_Drop_AlvlDropPrfWREDMinThresh)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDMinThresh , MinTHColor0 , profile->min_threshold[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDMinThresh , MinTHColor1 , profile->min_threshold[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDMinThresh , MinTHColor2 , profile->min_threshold[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.AlvlDropPrfWREDDPRatio, prof_ind, TM_Drop_AlvlDropPrfWREDDPRatio)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDDPRatio, DPRatio0, profile->dp_ratio[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDDPRatio, DPRatio1, profile->dp_ratio[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfWREDDPRatio, DPRatio2, profile->dp_ratio[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.AlvlDropPrfTailDrpThresh, prof_ind, TM_Drop_AlvlDropPrfTailDrpThresh)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfTailDrpThresh , TailDropThreshRes , profile->td_thresh_res, (uint8_t))
    TM_REGISTER_GET(TM_Drop_AlvlDropPrfTailDrpThresh , TailDropThresh , profile->td_threshold_bursts, (uint32_t))
out:
    return rc;
} 

int tm_get_hw_b_nodes_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDParams)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDScaleRatio)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDMinThresh)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfWREDDPRatio)
    TM_REGISTER_VAR(TM_Drop_BlvlDropPrfTailDrpThresh)

    TM_CTL(ctl, hndl)

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDParams , prof_ind , TM_Drop_BlvlDropPrfWREDParams)
    if (rc) goto out;
    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , AQLExp , profile->aql_exp, (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , ColorTDEn , profile->color_td_en, (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , ScaleExpColor0 , profile->scale_exp[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , ScaleExpColor1 , profile->scale_exp[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , ScaleExpColor2 , profile->scale_exp[2], (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , CurveIndexColor0 , profile->curve_id[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , CurveIndexColor1 , profile->curve_id[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDParams , CurveIndexColor2 , profile->curve_id[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDScaleRatio, prof_ind, TM_Drop_BlvlDropPrfWREDScaleRatio)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDScaleRatio , ScaleRatioColor0 , profile->scale_ratio[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDScaleRatio , ScaleRatioColor1 , profile->scale_ratio[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDScaleRatio , ScaleRatioColor2 , profile->scale_ratio[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDMinThresh, prof_ind,TM_Drop_BlvlDropPrfWREDMinThresh)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDMinThresh , MinTHColor0 , profile->min_threshold[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDMinThresh , MinTHColor1 , profile->min_threshold[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDMinThresh , MinTHColor2 , profile->min_threshold[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDDPRatio, prof_ind, TM_Drop_BlvlDropPrfWREDDPRatio)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDDPRatio, DPRatio0, profile->dp_ratio[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDDPRatio, DPRatio1, profile->dp_ratio[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfWREDDPRatio, DPRatio2, profile->dp_ratio[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfTailDrpThresh, prof_ind, TM_Drop_BlvlDropPrfTailDrpThresh)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfTailDrpThresh , TailDropThreshRes , profile->td_thresh_res, (uint8_t))
    TM_REGISTER_GET(TM_Drop_BlvlDropPrfTailDrpThresh , TailDropThresh , profile->td_threshold_bursts, (uint32_t))
out:
    return rc;
} 

int tm_get_hw_c_nodes_drop_profile(tm_handle hndl, uint8_t cos, uint32_t prof_ind, struct tm_drop_profile *profile)
{
    int rc =  -EFAULT;
    
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDParams_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDMinThresh_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfWREDDPRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_ClvlDropPrfTailDrpThresh_CoS)
	
    TM_CTL(ctl, hndl)

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDParams_CoS[cos], prof_ind , TM_Drop_ClvlDropPrfWREDParams_CoS)
    if (rc) goto out;

    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , AQLExp , profile->aql_exp, (uint8_t));
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , ColorTDEn , profile->color_td_en, (uint8_t));
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , ScaleExpColor0 , profile->scale_exp[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , ScaleExpColor1 , profile->scale_exp[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , ScaleExpColor2 , profile->scale_exp[2], (uint8_t))
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , CurveIndexColor0 , profile->curve_id[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , CurveIndexColor1 , profile->curve_id[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDParams_CoS , CurveIndexColor2 , profile->curve_id[2], (uint8_t))

	/* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDScaleRatio_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDScaleRatio_CoS)
	if (rc)	goto out;

	/* get register fields */		 
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS , ScaleRatioColor0 , profile->scale_ratio[0], (uint16_t))
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS , ScaleRatioColor1 , profile->scale_ratio[1], (uint16_t))
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDScaleRatio_CoS , ScaleRatioColor2 , profile->scale_ratio[2], (uint16_t))

	/* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDMinThresh_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDMinThresh_CoS);
	if (rc)	goto out;

	/* get register fields */		 
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDMinThresh_CoS , MinTHColor0 , profile->min_threshold[0], (uint16_t))
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDMinThresh_CoS , MinTHColor1 , profile->min_threshold[1], (uint16_t))
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDMinThresh_CoS , MinTHColor2 , profile->min_threshold[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.ClvlDropPrfWREDDPRatio_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfWREDDPRatio_CoS)
    if (rc) goto out;

    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDDPRatio_CoS, DPRatio0, profile->dp_ratio[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDDPRatio_CoS, DPRatio1, profile->dp_ratio[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_ClvlDropPrfWREDDPRatio_CoS, DPRatio2, profile->dp_ratio[2], (uint8_t))

	/* reset register reserved fields */
    TM_READ_TABLE_REGISTER(TM.Drop.ClvlDropPrfTailDrpThresh_CoS[cos], prof_ind, TM_Drop_ClvlDropPrfTailDrpThresh_CoS)
	if (rc)	goto out;

	/* get register fields */		 
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfTailDrpThresh_CoS , TailDropThreshRes , profile->td_thresh_res, (uint8_t))
	TM_REGISTER_GET(TM_Drop_ClvlDropPrfTailDrpThresh_CoS , TailDropThresh , profile->td_threshold_bursts, (uint32_t))
	/* write register */
out:
	return rc;
}

int tm_get_hw_ports_drop_profile_cos(tm_handle hndl, uint8_t cos, uint32_t prof_ind, struct tm_drop_profile *profile)
{
    int rc =  -EFAULT;
    
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDParams_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDScaleRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDMinThresh_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDDPRatio_CoS)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfTailDrpThresh_CoS)
	
    TM_CTL(ctl, hndl)

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.PortDropPrfWREDParams_CoS[cos], prof_ind , TM_Drop_PortDropPrfWREDParams_CoS)
    if (rc) goto out;

    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , AQLExp , profile->aql_exp, (uint8_t));
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , ColorTDEn , profile->color_td_en, (uint8_t));
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , ScaleExpColor0 , profile->scale_exp[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , ScaleExpColor1 , profile->scale_exp[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , ScaleExpColor2 , profile->scale_exp[2], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , CurveIndexColor0 , profile->curve_id[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , CurveIndexColor1 , profile->curve_id[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParams_CoS , CurveIndexColor2 , profile->curve_id[2], (uint8_t))

	/* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.PortDropPrfWREDScaleRatio_CoS[cos], prof_ind, TM_Drop_PortDropPrfWREDScaleRatio_CoS)
	if (rc)	goto out;

	/* get register fields */		 
	TM_REGISTER_GET(TM_Drop_PortDropPrfWREDScaleRatio_CoS , ScaleRatioColor0 , profile->scale_ratio[0], (uint16_t))
	TM_REGISTER_GET(TM_Drop_PortDropPrfWREDScaleRatio_CoS , ScaleRatioColor1 , profile->scale_ratio[1], (uint16_t))
	TM_REGISTER_GET(TM_Drop_PortDropPrfWREDScaleRatio_CoS , ScaleRatioColor2 , profile->scale_ratio[2], (uint16_t))

	/* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.PortDropPrfWREDMinThresh_CoS[cos], prof_ind, TM_Drop_PortDropPrfWREDMinThresh_CoS);
	if (rc)	goto out;

	/* get register fields */		 
	TM_REGISTER_GET(TM_Drop_PortDropPrfWREDMinThresh_CoS , MinTHColor0 , profile->min_threshold[0], (uint16_t))
	TM_REGISTER_GET(TM_Drop_PortDropPrfWREDMinThresh_CoS , MinTHColor1 , profile->min_threshold[1], (uint16_t))
	TM_REGISTER_GET(TM_Drop_PortDropPrfWREDMinThresh_CoS , MinTHColor2 , profile->min_threshold[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.PortDropPrfWREDDPRatio_CoS[cos], prof_ind, TM_Drop_PortDropPrfWREDDPRatio_CoS)
    if (rc) goto out;

    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDDPRatio_CoS, DPRatio0, profile->dp_ratio[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDDPRatio_CoS, DPRatio1, profile->dp_ratio[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDDPRatio_CoS, DPRatio2, profile->dp_ratio[2], (uint8_t))

	/* reset register reserved fields */
    TM_READ_TABLE_REGISTER(TM.Drop.PortDropPrfTailDrpThresh_CoS[cos], prof_ind, TM_Drop_PortDropPrfTailDrpThresh_CoS)
	if (rc)	goto out;

	/* get register fields */		 
	TM_REGISTER_GET(TM_Drop_PortDropPrfTailDrpThresh_CoS , TailDropThreshRes , profile->td_thresh_res, (uint8_t))
	TM_REGISTER_GET(TM_Drop_PortDropPrfTailDrpThresh_CoS , TailDropThresh , profile->td_threshold_bursts, (uint32_t))
	/* write register */
out:
	return rc;
}

int tm_get_hw_ports_drop_profile(tm_handle hndl, uint32_t prof_ind, struct tm_drop_profile *profile)
{
    int rc =  -EFAULT;

    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDParamsG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDScaleRatioG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDMinThreshG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfWREDDPRatioG)
    TM_REGISTER_VAR(TM_Drop_PortDropPrfTailDrpThreshG)

    TM_CTL(ctl, hndl)

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDParams , prof_ind , TM_Drop_PortDropPrfWREDParamsG)
    if (rc) goto out;
    /* get register fields */		 
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , AQLExp , profile->aql_exp, (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , ColorTDEn , profile->color_td_en, (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , ScaleExpColor0 , profile->scale_exp[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , ScaleExpColor1 , profile->scale_exp[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , ScaleExpColor2 , profile->scale_exp[2], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , CurveIndexColor0 , profile->curve_id[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , CurveIndexColor1 , profile->curve_id[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDParamsG , CurveIndexColor2 , profile->curve_id[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDScaleRatio, prof_ind, TM_Drop_PortDropPrfWREDScaleRatioG)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDScaleRatioG , ScaleRatioColor0 , profile->scale_ratio[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDScaleRatioG , ScaleRatioColor1 , profile->scale_ratio[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDScaleRatioG , ScaleRatioColor2 , profile->scale_ratio[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDMinThresh, prof_ind,TM_Drop_PortDropPrfWREDMinThreshG)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDMinThreshG , MinTHColor0 , profile->min_threshold[0], (uint16_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDMinThreshG , MinTHColor1 , profile->min_threshold[1], (uint16_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDMinThreshG , MinTHColor2 , profile->min_threshold[2], (uint16_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfWREDDPRatio, prof_ind, TM_Drop_PortDropPrfWREDDPRatioG)
    if (rc) goto out;
    /* get register fields */
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDDPRatioG, DPRatio0, profile->dp_ratio[0], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDDPRatioG, DPRatio1, profile->dp_ratio[1], (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfWREDDPRatioG, DPRatio2, profile->dp_ratio[2], (uint8_t))

    /* read register */
    TM_READ_TABLE_REGISTER(TM.Drop.BlvlDropPrfTailDrpThresh, prof_ind, TM_Drop_PortDropPrfTailDrpThreshG)
    if (rc) goto out;
    /* get register fields */		
    TM_REGISTER_GET(TM_Drop_PortDropPrfTailDrpThreshG , TailDropThreshRes , profile->td_thresh_res, (uint8_t))
    TM_REGISTER_GET(TM_Drop_PortDropPrfTailDrpThreshG , TailDropThresh , profile->td_threshold_bursts, (uint32_t))
out:
    return rc;
} 

