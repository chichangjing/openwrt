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
 * @brief Resource Manager allocation functions implementation.
 *
 * @file rm_alloc.c
 *
 * $Revision: 2.0 $
 */

#include "rm_internal_types.h"
#include "rm_alloc.h"

/** 
 */


/** 
 */ 
int rm_find_free_shaping_profile(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_SHAPING_PRF]));
}
/** 
 */
int rm_find_free_wred_queue_curve(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_WRED_Q_CURVE]));
}
/** 
 */
int rm_find_free_wred_a_node_curve(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_WRED_A_CURVE]));
}
/** 
 */
int rm_find_free_wred_b_node_curve(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_WRED_B_CURVE]));
}
/** 
 */
int rm_find_free_wred_c_node_curve(rmctl_t hndl, uint8_t cos)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return get_free_resource(&(ctl->rm_profiles[RM_WRED_C_CURVE_COS_0+cos]));
}
/** 
 */
int rm_find_free_wred_port_curve(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_WRED_P_CURVE]));
}
/** 
 */
int rm_find_free_wred_port_curve_cos(rmctl_t hndl, uint8_t cos)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return get_free_resource(&(ctl->rm_profiles[RM_WRED_P_CURVE_COS_0+cos]));
}
/** 
 */
int rm_find_free_queue_drop_profile(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_Q_DROP_PRF]));
}
/** 
 */
int rm_find_free_a_node_drop_profile(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_A_DROP_PRF]));
}
/** 
 */
int rm_find_free_b_node_drop_profile(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_B_DROP_PRF]));
}
/** 
 */
int rm_find_free_c_node_drop_profile(rmctl_t hndl, uint8_t cos)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return get_free_resource(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_0+cos]));
}
/** 
 */
int rm_find_free_port_drop_profile(rmctl_t hndl)
{
    RM_HANDLE(ctl, hndl);
	return get_free_resource(&(ctl->rm_profiles[RM_P_DROP_PRF]));
}
/** 
 */
int rm_find_free_port_drop_profile_cos(rmctl_t hndl, uint8_t cos)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	return get_free_resource(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_0+cos]));
}

int rm_get_next_allocated_drop_resource(rmctl_t hndl, enum tm_level level, uint8_t cos, int index)
{
    int next_index = -1;

    RM_HANDLE(ctl, hndl);
    
    switch (level)
    {
        case Q_LEVEL:
            next_index = get_next_allocated_resource(&(ctl->rm_profiles[RM_Q_DROP_PRF]), index);
            break;

        case A_LEVEL:
            next_index = get_next_allocated_resource(&(ctl->rm_profiles[RM_A_DROP_PRF]), index);
        break;

        case B_LEVEL:
            next_index = get_next_allocated_resource(&(ctl->rm_profiles[RM_B_DROP_PRF]), index);
        break;

        case C_LEVEL:
            if (cos < RM_COS)
                next_index = get_next_allocated_resource(&(ctl->rm_profiles[RM_C_DROP_PRF_COS_0+cos]), index);
        break;

        case P_LEVEL:
            if (cos < RM_COS)
                next_index = get_next_allocated_resource(&(ctl->rm_profiles[RM_P_DROP_PRF_COS_0+cos]), index);
            else
                next_index = get_next_allocated_resource(&(ctl->rm_profiles[RM_P_DROP_PRF]), index);
        break;
        default:
        break;
    }

    return next_index;
}

/** 
 * Aging profile requires allocation of pool of AGING_RESOURCE_SLOT_SIZE(=4) drop profiles 
 */
int rm_find_free_queue_aging_profile(rmctl_t hndl)
{
	int result;
    RM_HANDLE(ctl, hndl);
	result = get_free_resource_range_slot(&(ctl->rm_profiles[RM_Q_DROP_PRF]),AGING_RESOURCE_SLOT_SIZE);
	if (result == -1)  return -ENOBUFS;	
	return result;
}
