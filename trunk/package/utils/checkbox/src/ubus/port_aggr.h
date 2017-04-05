#ifndef _PORT_AGGR
#define _PORT_AGGR
void port_aggr_get_mode_select(void);
void port_aggr_get_load_balance(void);
void port_aggr_get_channel_num(void);
void port_aggr_get_channel_id_one(const char *idx);
void port_aggr_get_channel_id_one_d(void);
void port_aggr_get_channel_member_one(const char *idx);
void port_aggr_get_channel_member_all(void);
void port_aggr_get_channel_one(const char *idx);
void port_aggr_get_channel_all(void);

void port_aggr_set_mode_select(const char *mode_select);
void port_aggr_set_load_balance(const char *load_balance);
void port_aggr_set_channel_id_one_d(void);
void port_aggr_set_channel_member_one(const char *idx, const char *channel_member);
void port_aggr_set_channel_one(const char *idx, const char *channel_id, const char *channel_member);
void port_aggr_clear_one(const char *idx);
#endif
