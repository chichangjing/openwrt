
#ifndef _HSL_PORT_PROPERTY_H_
#define _HSL_PORT_PROPERTY_H_

#ifdef __cplusplus
extern "c" {
#endif

typedef enum {
    HSL_PP_PHY = 0,    /* setting concerning phy */
    HSL_PP_INCL_CPU,   /* setting may include cpu port */
    HSL_PP_EXCL_CPU,   /* setting exclude cpu port */
    HSL_PP_END
} hsl_port_prop_t;

a_bool_t hsl_port_prop_check(a_uint32_t dev_id, fal_port_t port_id, hsl_port_prop_t p_type);
a_bool_t hsl_mports_prop_check(a_uint32_t dev_id, fal_pbmp_t port_bitmap, hsl_port_prop_t p_type);
a_bool_t hsl_port_validity_check(a_uint32_t dev_id, fal_port_t port_id);
a_bool_t hsl_mports_validity_check(a_uint32_t dev_id, fal_pbmp_t port_bitmap);
sw_error_t hsl_port_prop_portmap_get(a_uint32_t dev_id, fal_pbmp_t port_bitmap);
sw_error_t hsl_port_prop_set(a_uint32_t dev_id, fal_port_t port_id, hsl_port_prop_t p_type);
sw_error_t hsl_port_prop_portmap_set(a_uint32_t dev_id,  fal_port_t port_id);
sw_error_t hsl_port_prop_clr(a_uint32_t dev_id, fal_port_t port_id, hsl_port_prop_t p_type);
sw_error_t hsl_port_prop_get_phyid(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *phy_id);
sw_error_t hsl_port_prop_set_phyid(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t phy_id);
sw_error_t hsl_port_prop_init_by_dev(a_uint32_t dev_id);
sw_error_t hsl_port_prop_init(void);

#ifdef __cplusplus
}
#endif
#endif

