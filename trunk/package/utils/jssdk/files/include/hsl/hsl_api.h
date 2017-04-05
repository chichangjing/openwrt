
#ifndef _HSL_API_H
#define _HSL_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal.h"

/********************************************************************************************************************************************
   Function prototype define --- Misc
 ********************************************************************************************************************************************/


/********************************************************************************************************************************************
   Function prototype define --- Port Control
 ********************************************************************************************************************************************/
typedef sw_error_t (*hsl_port_type_get) (a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type_info, fal_port_comb_t *comb_port);
typedef sw_error_t (*hsl_port_admin_mode_set) (a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
typedef sw_error_t (*hsl_port_admin_mode_get) (a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable);
typedef sw_error_t (*hsl_port_link_status_get) (a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status);
typedef sw_error_t (*hsl_port_physical_mode_set) (a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode);
typedef sw_error_t (*hsl_port_physical_mode_get) (a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode);
typedef sw_error_t (*hsl_port_physical_status_get) (a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status);
typedef sw_error_t (*hsl_port_speed_get) (a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed);
typedef sw_error_t (*hsl_port_duplex_get) (a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex);
typedef sw_error_t (*hsl_port_flowctrl_mode_set) (a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable);
typedef sw_error_t (*hsl_port_flowctrl_mode_get) (a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable);
typedef sw_error_t (*hsl_port_dot1d_state_set) (a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state);
typedef sw_error_t (*hsl_port_dot1d_state_get) (a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state);
typedef sw_error_t (*hsl_port_test_show_all) (a_uint32_t dev_id);
typedef sw_error_t (*hsl_port_pcs_status_get) (a_uint32_t dev_id,fal_port_t port_id,a_uint32_t *pcs_status);
typedef sw_error_t (*hsl_port_comb_type_set) (a_uint32_t dev_id, fal_port_t port_id, fal_port_comb_type_t comb_type);
typedef sw_error_t (*hsl_port_comb_type_get) (a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t *comb_type);

/********************************************************************************************************************************************
   Function prototype define --- Mirror 
 ********************************************************************************************************************************************/
typedef sw_error_t	(*hsl_mirror_dest_port_set) (a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t	(*hsl_mirror_dest_port_get) (a_uint32_t dev_id, fal_port_t *port_id);
typedef sw_error_t	(*hsl_mirror_source_port_mode_set) (a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode);
typedef sw_error_t	(*hsl_mirror_source_port_mode_get) (a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode);

/********************************************************************************************************************************************
   Function prototype define --- Mib
 ********************************************************************************************************************************************/
typedef sw_error_t (*hsl_mib_info_get) (a_uint32_t dev_id, fal_port_t port_id, fal_mib_info_t * mib_info);
typedef sw_error_t (*hsl_mib_info_clear) (a_uint32_t dev_id, fal_port_t port_id);

/********************************************************************************************************************************************
   Function prototype define --- Trunk
 ********************************************************************************************************************************************/
typedef sw_error_t	(*hsl_trunk_pag_mode_set) (a_uint32_t dev_id, fal_trunk_mode_t mode);
typedef sw_error_t	(*hsl_trunk_pag_mode_get) (a_uint32_t dev_id, fal_trunk_mode_t *mode);
typedef sw_error_t	(*hsl_trunk_load_balance_set) (a_uint32_t dev_id, fal_trunk_load_balance_t mode);
typedef sw_error_t	(*hsl_trunk_load_balance_get) (a_uint32_t dev_id, fal_trunk_load_balance_t *mode);
typedef sw_error_t	(*hsl_trunk_id_set) (a_uint32_t dev_id, fal_trunk_id_t trunk_id);
typedef sw_error_t	(*hsl_trunk_members_set) (a_uint32_t dev_id, fal_trunk_members_t *trunk_members);
typedef sw_error_t	(*hsl_trunk_row_create) (a_uint32_t dev_id, fal_trunk_id_t trunk_id, fal_trunk_members_t *trunk_members, a_uint32_t member_size);
typedef sw_error_t	(*hsl_trunk_row_destroy) (a_uint32_t dev_id, fal_trunk_id_t trunk_id);
typedef sw_error_t	(*hsl_trunk_show_row_status) (a_uint32_t dev_id, fal_trunk_id_t trunk_id);

/********************************************************************************************************************************************
   Function prototype define --- Rate
 ********************************************************************************************************************************************/
typedef sw_error_t	(*hsl_rate_bucket_egrl_set) (a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable);
typedef sw_error_t	(*hsl_rate_bucket_egrl_get) (a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t *speed,fal_enable_t *enable);
typedef sw_error_t  (*hsl_rate_bucket_pirl_set) (a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable);
typedef sw_error_t  (*hsl_rate_bucket_pirl_get) (a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t *type_mask,fal_rate_speed_t *speed,fal_enable_t *enable);

/********************************************************************************************************************************************
   Function prototype define --- Multicast
 ********************************************************************************************************************************************/
typedef sw_error_t	(*hsl_multicast_enable_set) (a_uint32_t dev_id,fal_enable_t enable);
typedef sw_error_t	(*hsl_multicast_enable_get) (a_uint32_t dev_id,fal_enable_t *enable);
typedef sw_error_t	(*hsl_multicast_row_create) (a_uint32_t dev_id,fal_multicast_members_t *multicast_members,fal_multicast_addr_t *multicast_addr, a_uint32_t members_size);
typedef sw_error_t	(*hsl_multicast_row_destroy) (a_uint32_t dev_id,fal_multicast_members_t *multicast_members,fal_multicast_addr_t *multicast_addr, a_uint32_t members_size);
typedef sw_error_t	(*hsl_mgmt_frame_set) (a_uint32_t dev_id,fal_multicast_addr_t *mgmt_addr,fal_enable_t enable);

/********************************************************************************************************************************************
   Function prototype define --- Vlan
 ********************************************************************************************************************************************/
typedef sw_error_t	(*hsl_vlan_mode_set) (a_uint32_t dev_id,fal_vlan_mode_t mode);
typedef sw_error_t	(*hsl_vlan_mode_get) (a_uint32_t dev_id,fal_vlan_mode_t *mode);
typedef sw_error_t	(*hsl_port_based_vlan_set) (a_uint32_t dev_id,fal_port_t port_id,fal_vlan_members_t *vlan_members, a_uint32_t member_size);
typedef sw_error_t	(*hsl_8021q_vlan_row_create) (a_uint32_t dev_id,fal_vlan_id_t vlan_id,fal_vlan_members_t *port_members, a_uint32_t member_size,fal_vlan_egress_tag_t *egress_tag);
typedef sw_error_t	(*hsl_8021q_vlan_row_destroy) (a_uint32_t dev_id,fal_vlan_id_t vlan_id);
typedef sw_error_t	(*hsl_8021q_vlan_show_status) (a_uint32_t dev_id);
typedef sw_error_t	(*hsl_8021q_vlan_row_status) (a_uint32_t dev_id,fal_vlan_id_t *vlan_id,fal_vlan_egress_tag_t *egress_tag);

/********************************************************************************************************************************************
   Function prototype define --- QOS
 ********************************************************************************************************************************************/
typedef sw_error_t	(*hsl_qos_port_prio_map_rule_set) (a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t qos_map);
typedef sw_error_t	(*hsl_qos_port_prio_map_rule_get) (a_uint32_t dev_id,fal_port_t port_id,fal_qos_map_t * qos_map);
typedef sw_error_t	(*hsl_qos_port_schedule_mode_set) (a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t schedule_mode);
typedef sw_error_t	(*hsl_qos_port_schedule_mode_get) (a_uint32_t dev_id,fal_port_t port_id,fal_qos_schedule_mode_t * schedule_mode);
typedef sw_error_t	(*hsl_qos_tag_prio_map_set) (a_uint32_t dev_id,fal_tag_map_t *tag_mapping);
typedef sw_error_t	(*hsl_qos_ip_prio_map_set) (a_uint32_t dev_id,fal_ip_map_t *ip_mapping);
typedef sw_error_t	(*hsl_qos_port_prio_assign_mode_set) (a_uint32_t dev_id,fal_port_t port_id,fal_qos_assign_mode_t assign_mode);
typedef sw_error_t	(*hsl_qos_port_default_prio_set) (a_uint32_t dev_id,fal_port_t port_id,fal_qos_prio_t priority);
typedef sw_error_t	(*hsl_qos_show_status) (a_uint32_t dev_id);

/********************************************************************************************************************************************
   Function prototype define --- FDB
 ********************************************************************************************************************************************/
typedef sw_error_t	(*hsl_mac_aging_timeout_set) (a_uint32_t dev_id,fal_mac_aging_t aging_time);
typedef sw_error_t	(*hsl_mac_secure_port_enable_set) (a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable);
typedef	sw_error_t	(*hsl_mac_row_create) (a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr);
typedef	sw_error_t	(*hsl_mac_row_destroy) (a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id);
typedef sw_error_t	(*hsl_mac_addr_table_show) (a_uint32_t dev_id);
typedef sw_error_t	(*hsl_mac_addr_table_clear) (a_uint32_t dev_id);
typedef sw_error_t	(*hsl_mac_show_blacklist) (a_uint32_t dev_id);
typedef sw_error_t	(*hsl_mac_flush_port) (a_uint32_t dev_id, fal_port_t port_id);
typedef sw_error_t	(*hsl_8021x_port_auth_set) (a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable);
typedef sw_error_t	(*hsl_8021x_mac_bind) (a_uint32_t dev_id,fal_port_t port_id,fal_atu_status atu_status,fal_fdb_mac_t * macAddress);

/* QOS */
/* FDB */
/* PortVlan */
/* VLAN */
/* Trunk */
/* STP */
/* IGMP */

/********************************************************************************************************************************************
   Function prototype define --- Register read/write
 ********************************************************************************************************************************************/
typedef sw_error_t (*hsl_reg_get) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len);
typedef sw_error_t (*hsl_reg_set) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len);
typedef sw_error_t (*hsl_reg_field_get) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, a_uint32_t value[], a_uint32_t value_len);
typedef sw_error_t (*hsl_reg_field_set) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, const a_uint8_t value[], a_uint32_t value_len);
typedef sw_error_t (*hsl_reg_entries_get) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t entry_len, a_uint8_t value[], a_uint32_t value_len);
typedef sw_error_t (*hsl_reg_entries_set) (a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t entry_len, const a_uint8_t value[], a_uint32_t value_len);
typedef sw_error_t (*hsl_phy_reg_get) (a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t * data);
typedef sw_error_t (*hsl_phy_reg_set) (a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t data);

/********************************************************************************************************************************************
   Function prototype define --- Device initialize
 ********************************************************************************************************************************************/
typedef sw_error_t (*hsl_dev_reset) (a_uint32_t dev_id);
typedef sw_error_t (*hsl_dev_mode_tag) (a_uint32_t dev_id);


typedef struct {
    /* Misc */

    
    /* Port control */
    hsl_port_type_get port_type_get;
    hsl_port_admin_mode_set port_admin_mode_set;
    hsl_port_admin_mode_get port_admin_mode_get;
    hsl_port_link_status_get port_link_status_get;
    hsl_port_physical_mode_set port_physical_mode_set;
    hsl_port_physical_mode_get port_physical_mode_get;
    hsl_port_physical_status_get port_physical_status_get;
    hsl_port_speed_get port_speed_get;
	hsl_port_duplex_get port_duplex_get;
    hsl_port_flowctrl_mode_set port_flowctrl_mode_set;
    hsl_port_flowctrl_mode_get port_flowctrl_mode_get;
    hsl_port_dot1d_state_set port_dot1d_state_set;
    hsl_port_dot1d_state_get port_dot1d_state_get;	
    hsl_port_test_show_all port_test_show_all;
	hsl_port_pcs_status_get port_pcs_status_get;
	hsl_port_comb_type_set port_comb_type_set;
	hsl_port_comb_type_get port_comb_type_get;

	/* Mib */
    hsl_mib_info_get mib_info_get;
    hsl_mib_info_clear mib_info_clear;
	
	/* Mirror */
	hsl_mirror_dest_port_set  mirror_dest_port_set;
	hsl_mirror_dest_port_get  mirror_dest_port_get;
	hsl_mirror_source_port_mode_set mirror_source_port_mode_set;
	hsl_mirror_source_port_mode_get mirror_source_port_mode_get;

	/* Trunk */
	hsl_trunk_pag_mode_set	   trunk_pag_mode_set;
	hsl_trunk_pag_mode_get	   trunk_pag_mode_get;
	hsl_trunk_load_balance_set trunk_load_balance_set;
	hsl_trunk_load_balance_get trunk_load_balance_get;
	hsl_trunk_id_set		   trunk_id_set;
	hsl_trunk_members_set	   trunk_members_set;
	hsl_trunk_row_create	   trunk_row_create;
	hsl_trunk_row_destroy	   trunk_row_destroy;
	hsl_trunk_show_row_status  trunk_show_row_status;

	/* Rate */
	hsl_rate_bucket_egrl_set   rate_bucket_egrl_set;
	hsl_rate_bucket_egrl_get   rate_bucket_egrl_get;
	hsl_rate_bucket_pirl_set   rate_bucket_pirl_set;
	hsl_rate_bucket_pirl_get   rate_bucket_pirl_get;

	/* Multicast */
	hsl_multicast_enable_set   multicast_enable_set;
	hsl_multicast_enable_get   multicast_enable_get;
	hsl_multicast_row_create   multicast_row_create;
	hsl_multicast_row_destroy  multicast_row_destroy;
	hsl_mgmt_frame_set		   mgmt_frame_set;

	/* Vlan */
	hsl_vlan_mode_set		   	vlan_mode_set;
	hsl_vlan_mode_get			vlan_mode_get;
	hsl_port_based_vlan_set   	port_based_vlan_set;
	hsl_8021q_vlan_row_create  	ieee8021q_vlan_row_create;
	hsl_8021q_vlan_row_destroy	ieee8021q_vlan_row_destroy;
	hsl_8021q_vlan_show_status	ieee8021q_vlan_show_status;
	hsl_8021q_vlan_row_status	ieee8021q_vlan_row_status;

	/* QOS */
	hsl_qos_port_prio_map_rule_set		qos_port_prio_map_rule_set;
	hsl_qos_port_prio_map_rule_get		qos_port_prio_map_rule_get;
	hsl_qos_port_schedule_mode_set		qos_port_schedule_mode_set;
	hsl_qos_port_schedule_mode_get		qos_port_schedule_mode_get;
	hsl_qos_tag_prio_map_set			qos_tag_prio_map_set;
	hsl_qos_ip_prio_map_set				qos_ip_prio_map_set;
	hsl_qos_port_prio_assign_mode_set	qos_port_prio_assign_mode_set;
	hsl_qos_port_default_prio_set		qos_port_default_prio_set;
	hsl_qos_show_status					qos_show_status;

	/* MAC */
	hsl_mac_aging_timeout_set			mac_aging_timeout_set;
	hsl_mac_secure_port_enable_set		mac_secure_port_enable_set;
	hsl_mac_row_create					mac_row_create;
	hsl_mac_row_destroy					mac_row_destroy;
	hsl_mac_addr_table_show				mac_addr_table_show;
	hsl_mac_addr_table_clear			mac_addr_table_clear;
	hsl_mac_show_blacklist				mac_show_blacklist;
	hsl_mac_flush_port					mac_flush_port;
	hsl_8021x_port_auth_set				ieee8021x_port_auth_set;
	hsl_8021x_mac_bind					ieee8021x_mac_bind;
	
    /* Register */
    hsl_reg_get reg_get;
    hsl_reg_set reg_set;
    hsl_reg_field_get reg_field_get;
    hsl_reg_field_set reg_field_set;
	hsl_phy_reg_get phy_reg_get;
	hsl_phy_reg_set phy_reg_set;

    /* Init */
    hsl_dev_reset dev_reset;
	hsl_dev_mode_tag dev_mode_tag;
} hsl_api_t;

hsl_api_t *hsl_api_ptr_get(a_uint32_t dev_id);
sw_error_t hsl_api_init(a_uint32_t dev_id);

#if defined(SW_API_LOCK) && (!defined(HSL_STANDALONG))
extern  aos_lock_t sw_hsl_api_lock;
#define FAL_API_LOCK    aos_lock(&sw_hsl_api_lock)
#define FAL_API_UNLOCK  aos_unlock(&sw_hsl_api_lock)
#else
#define FAL_API_LOCK
#define FAL_API_UNLOCK
#endif

#ifdef __cplusplus
}
#endif
#endif

