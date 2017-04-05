#ifndef _PARSE_AGGR
#define _PARSE_AGGR
int parse_aggr_view_init(char* argv);
int parse_port_aggr_get_mode_select(char* argv);
int parse_port_aggr_get_load_balance(char* argv);
int parse_port_aggr_get_channel_id_one(char* argv);
int parse_port_aggr_get_channel_member_one(char* argv);
int parse_port_aggr_get_channel_one(char* argv);
int parse_port_aggr_get_channel_all(char* argv);
int parse_aggr_enable(char* argv);
int parse_aggr_disable(char* argv);
int parse_port_aggr_set_load_balance(char* argv);
int parse_port_aggr_set_channel_member_one(char* argv);
int parse_port_aggr_add_channel_member_one(char* argv);
int parse_port_aggr_del_channel_member_one(char* argv);
int parse_port_aggr_clear_one(char* argv);

#endif
