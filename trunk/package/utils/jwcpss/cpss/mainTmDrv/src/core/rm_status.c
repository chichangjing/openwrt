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
 * @brief Resource Manager status functions implementation.
 *
 * @file rm_status.c
 *
 * $Revision: 2.0 $
 */

#include "rm_internal_types.h"
#include "rm_status.h"


#define TEST_RESOURCE_STATUS(resource_type) \
	switch (resource_status(&(ctl->rm_profiles[resource_type]), entry_ind))\
	{\
		case  -1: return -EFAULT;\
		case   0: return 0;\
		default: return 1;\
	}\
/** 
 */
int rm_shaping_profile_status(rmctl_t hndl, uint32_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_SHAPING_PRF);
}
/** 
 */
int rm_wred_queue_curve_status(rmctl_t hndl, uint8_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_WRED_Q_CURVE);
}
/** 
 */
int rm_wred_a_node_curve_status(rmctl_t hndl, uint8_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_WRED_A_CURVE);
}
/** 
 */
int rm_wred_b_node_curve_status(rmctl_t hndl, uint8_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_WRED_B_CURVE);
}
/** 
 */
int rm_wred_c_node_curve_status_cos(rmctl_t hndl, uint8_t cos, uint8_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	TEST_RESOURCE_STATUS(RM_WRED_C_CURVE_COS_0+cos);
}
/** 
 */
int rm_wred_port_curve_status(rmctl_t hndl, uint8_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_WRED_P_CURVE);
}
/** 
 */
int rm_wred_port_curve_status_cos(rmctl_t hndl, uint8_t cos, uint8_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	TEST_RESOURCE_STATUS(RM_WRED_P_CURVE_COS_0+cos);
}
/** 
 */
int rm_queue_drop_profile_status(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_Q_DROP_PRF);
}
/** 
 */
int rm_a_node_drop_profile_status(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_A_DROP_PRF);
}
/** 
 */
int rm_b_node_drop_profile_status(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_B_DROP_PRF);
}
/** 
 */
int rm_c_node_drop_profile_status_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	TEST_RESOURCE_STATUS(RM_C_DROP_PRF_COS_0+cos);
}
/** 
 */
int rm_port_drop_profile_status(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	TEST_RESOURCE_STATUS(RM_P_DROP_PRF);
}
/** 
 */
int rm_port_drop_profile_status_cos(rmctl_t hndl, uint8_t cos, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	if (cos >=RM_COS) return -EFAULT;
	TEST_RESOURCE_STATUS(RM_P_DROP_PRF_COS_0+cos);
}

int rm_validate_queue_aging_profile(rmctl_t hndl, uint16_t entry_ind)
{
    RM_HANDLE(ctl, hndl);
	return validate_resource_range(&(ctl->rm_profiles[RM_Q_DROP_PRF]), entry_ind, AGING_RESOURCE_SLOT_SIZE);
}
