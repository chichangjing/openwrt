#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "parse_mirror.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/port_config.h"
#include "../ubus/port_mirror.h"
#include "../ubus/eth_stats.h"
#include "../ubus/qos.h"
#include "../ubus/vlan.h"
#include "../ubus/device.h"
#include "../caselib/cs_type.h"

int parse_display_interface(char* argv){
    char * port;
    char * extra;
    const char * port_ubus;
    port = strtok(argv, " ");
    extra = strtok(NULL, " ");
    if(port == NULL){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char *)(&in_put_buf)));
        printf(" %% Please give a valid port!\n");
        return CB_FAIL;
    }
    port_ubus = port_name_to_port(port);
    if(port_ubus == NULL){
        posi_err(strlen(_terminal_view->cmd_head) + (port-(char *)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port!\n");
        return CB_FAIL;
    }
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    port_config_get_port_config_one(port_ubus);
    eth_stats_get_one(port_ubus);
    return CB_SUCCESS;
}

int parse_interface_view_init(char* argv){
	char *port;
	char *extra;
	const char *idx;
	port = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == port){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a port!\n");
		return CB_FAIL;
	}
	idx = port_name_to_ubus(port);
	if(NULL == idx){
		posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	memset(device_gt, 0, sizeof(struct device_gt));
	strcpy(device_gt->current_view_name, port);
	strcpy(device_gt->current_idx, idx);
    sprintf(_terminal_view->cmd_head, "[%s]# ", port);
    _terminal_view->_cmd_list->cmd_num  =   _view_if_interface_list->cmd_num;
    _terminal_view->_cmd_list->_cmd_    =   _view_if_interface_list->_cmd_;
    _terminal_view->cursor  =   0;
	return CB_SUCCESS;
}

int parse_port_isolate(char* argv){
    char * extra;
    char * member;
    int mask_1, mask_2;
    char list_1[16], list_2[16];
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    if(!str_idx(member, port_name[2])){
        posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid member port!\n");
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_PORTBASED:
            break;
        case VLAN_MODE_8021D:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    mask_1 = ~(1<<(16-str_idx(member, port_name[2]))) & global_isot->dev_isot[idx2id(device_gt->current_idx)-1].mask;
    mask_2 = ~(1<<(16-idx2id(device_gt->current_idx))) & global_isot->dev_isot[str_idx(member, port_name[2])-1].mask;
    sprintf(list_1, "%02x-%02x%c", mask_1>>8, 0x00ff & mask_1, '\0');
    sprintf(list_2, "%02x-%02x%c", mask_2>>8, 0x00ff & mask_2, '\0');
    printf(" %% Isolate %s between %s.\n", id2port(idx2id(device_gt->current_idx)), member);
    port_base_vlan_set_port_list(id2port(idx2id(device_gt->current_idx)), list_1, member, list_2);
    return CB_SUCCESS;
}

int parse_port_unisolate(char* argv){
    char * extra;
    char * member;
    int mask_1, mask_2;
    char list_1[16], list_2[16];
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    if(!str_idx(member, port_name[2])){
        posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid member port!\n");
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_PORTBASED:
            break;
        case VLAN_MODE_8021D:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    mask_1 = 1<<(16-str_idx(member, port_name[2])) | global_isot->dev_isot[idx2id(device_gt->current_idx)-1].mask;
    mask_2 = 1<<(16-idx2id(device_gt->current_idx)) | global_isot->dev_isot[str_idx(member, port_name[2])-1].mask;
    sprintf(list_1, "%02x-%02x%c", mask_1>>8, 0x00ff & mask_1, '\0');
    sprintf(list_2, "%02x-%02x%c", mask_2>>8, 0x00ff & mask_2, '\0');
    printf(" %% Isolate %s between %s.\n", id2port(idx2id(device_gt->current_idx)), member);
    port_base_vlan_set_port_list(id2port(idx2id(device_gt->current_idx)), list_1, member, list_2);
    return CB_SUCCESS;
}

int parse_interface_port_mirror_set_mode(char* argv){
	char *mode;
	char *extra;
	const char *mode_ubus;
	mode = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == mode){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a port mirroring mode!\n");
		return CB_FAIL;
	}
	mode_ubus = port_mirror_mode_to_mode(mode);
	if(NULL == mode_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port mirroring mode!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	port_mirror_get_destination_port();
	switch(device_gt->current_mode){
		case 0:
			printf(" %% No destnation Set! Please set destnation first!\n");
			return CB_SUCCESS;
			break;
		case PORT_1:
		case PORT_2:
		case PORT_3:
		case PORT_4:
		case PORT_5:
		case PORT_6:
		case PORT_7:
		case PORT_8:
		case PORT_9:
		case PORT_10:
			if(0 == strcmp(port_name[0][device_gt->current_mode], device_gt->current_idx)){
				printf(" %% %s is the destnation Port!\n", port_name[1][device_gt->current_mode]);
				return CB_SUCCESS;
			}
			printf(" %% Port mirror set %s mode %s.\n", port_name_to_show(device_gt->current_idx), mode);
			port_mirror_set_mode_one(port_idx_to_port(device_gt->current_idx), mode_ubus);
			return CB_SUCCESS;
			break;
		default:
			printf(" %% Terminal work error!\n");
			return CB_FAIL;
			break;
	}
	return CB_SUCCESS;
}

int parse_interface_port_mirror_disable(char * argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        port_mirror_set_destination("0");
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_interface_port_mirror_set_destnation(char* argv){
	char *extra;
	if(!(extra = strtok(argv, " "))){
		port_mirror_set_destination(device_gt->current_idx+strlen("idx-"));
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_interface_physical_mode(char* argv){
	char *mode;
	char *extra;
	const char *mode_ubus;
	if(!(mode = strtok(argv, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a port physical mode!\n");
		return CB_FAIL;
	}
	if(!(mode_ubus = port_config_speed_to_mode(mode))){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port physical mode!\n");
		return CB_FAIL;
	}
	if(!(extra = strtok(NULL, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	port_config_get_admin_mode_one(port_idx_to_port(device_gt->current_idx));
	switch(device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% Current port is Disable to configure!\n");
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid port mode: [%d]!\n", device_gt->current_mode);
            return CB_FAIL;
            break;
	}
	printf(" %% Port config set %s physical mode %s.\n", port_name_to_show(device_gt->current_idx), mode);
	port_config_set_physical_mode_one(port_idx_to_port(device_gt->current_idx), mode_ubus);
	return CB_SUCCESS;
}

int parse_interface_admin_mode(char* argv){
	char *mode;
	char *extra;
	const char *mode_ubus;
	mode = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == mode){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a port admin mode!\n");
		return CB_FAIL;
	}
	mode_ubus = port_config_admin_mode_to_mode(mode);
	if(NULL == mode_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port admin mode!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	printf(" %% Port config set %s admin mode %s.\n", port_name_to_show(device_gt->current_idx), mode);
	port_config_set_admin_mode_one(port_idx_to_port(device_gt->current_idx), mode_ubus);
	return CB_SUCCESS;
}

int parse_interface_flow_control_mode(char* argv){
	char *mode;
	char *extra;
	const char *mode_ubus;
	mode = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == mode){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a port flow control mode!\n");
		return CB_FAIL;
	}
	mode_ubus = port_config_flow_control_to_mode(mode);
	if(NULL == mode_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port flow control mode!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
    port_config_get_admin_mode_one(port_idx_to_port(device_gt->current_idx));
    switch(device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% Current port is Disable to configure!\n");
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid port mode: [%d]!\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
	printf(" %% Port config set %s flow control mode %s.\n", port_name_to_show(device_gt->current_idx), mode);
	port_config_set_flow_control_mode_one(port_idx_to_port(device_gt->current_idx), mode_ubus);
	return CB_SUCCESS;
}

int parse_interface_qos_mode(char* argv){
    char *mode;
    char *extra;
    const char *mode_ubus;
    if(!(mode = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a qos mode!\n");
        return CB_FAIL;
    }
    if(!(mode_ubus = qos_assingment_to_ubus(mode))){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid qos mode!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    printf(" %% Set port %s qos mode %s.\n", port_name_to_show(device_gt->current_idx), mode);
    qos_set_mode(port_idx_to_port(device_gt->current_idx), mode_ubus);
    return CB_SUCCESS;
}

int parse_interface_qos_schedul_mode(char* argv){
    char *mode;
    char *extra;
    const char *mode_ubus;
    if(!(mode = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a schedul mode!\n");
        return CB_FAIL;
    }
    if(!(mode_ubus = qos_schedul_mode_to_ubus(mode))){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid schedul mode!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    printf(" %% Set port %s schedul mode %s.\n", port_name_to_show(device_gt->current_idx), mode);
    qos_set_schedul_mode(port_idx_to_port(device_gt->current_idx), mode_ubus);
    return CB_SUCCESS;
}

int parse_interface_default_priority(char* argv){
    char *mode;
    char *extra;
    const char *mode_ubus;
    if(!(mode = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a default priority!\n");
        return CB_FAIL;
    }
    if(!(mode_ubus = qos_default_priority_to_ubus(mode))){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid default priority!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    printf(" %% Set port %s default priority %s.\n", port_name_to_show(device_gt->current_idx), mode);
    qos_set_default_priority(port_idx_to_port(device_gt->current_idx), mode_ubus);
    return CB_SUCCESS;
}

int parse_interface_map_rule(char* argv){
    char *mode;
    char *extra;
    const char *mode_ubus;
    if(!(mode = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a map rule mode!\n");
        return CB_FAIL;
    }
    if(!(mode_ubus = qos_map_rule_to_ubus(mode))){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid map rule mode!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    qos_get_mode(port_idx_to_port(device_gt->current_idx));
    switch(device_gt->current_mode){
        case _QOS_ASSINGMENT_P8021_DSCP:
            break;
        case _QOS_ASSINGMENT_PORT_BASED:
        case _QOS_ASSINGMENT_P8021:
        case _QOS_ASSINGMENT_DSCP:
            printf(" %% Qos mode is %s! Qos map rule only allowed to configure in 802.1p&dscp mode.\n", qos_assingment[device_gt->current_mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid qos mode: [%d]!\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set port %s qos map rule mode %s.\n", port_name_to_show(device_gt->current_idx), mode);
    qos_set_map_rule(port_idx_to_port(device_gt->current_idx), mode_ubus);
    return CB_SUCCESS;
}

int parse_port_config_set_flow_control_mode_one(char* argv){
	char *port;
	char *mode;
	char *extra;
	const char *port_ubus;
	const char *mode_ubus;
	port  = strtok(argv, " ");
	mode  = strtok(NULL, " ");
	extra = strtok(NULL, " ");
	if(NULL == port){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a port!\n");
		return CB_FAIL;
	}
	port_ubus = port_name_to_ubus(port);
	if(NULL == port_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port!\n");
		return CB_FAIL;
	}
	if(NULL == mode){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a port flow control mode!\n");
		return CB_FAIL;
	}
	mode_ubus = port_config_flow_control_to_mode(mode);
	if(NULL == mode_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port flow control mode!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	printf(" %% Port_config set %s flow control %s.\n", port, mode);
	port_config_set_flow_control_mode_one(port_ubus, mode_ubus);
	return CB_SUCCESS;
}

int parse_if_trap_threshold_tx_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s trap threshold tx enable.\n", port_name_to_show(device_gt->current_idx));
        set_if_trap_threshold_tx_status(port_idx_to_port(device_gt->current_idx), i2a[_IF_TRAP_THRESHOLD_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_if_trap_threshold_tx_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s trap threshold tx disable.\n", port_name_to_show(device_gt->current_idx));;
        set_if_trap_threshold_tx_status(port_idx_to_port(device_gt->current_idx), i2a[_IF_TRAP_THRESHOLD_MODE_DISABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_if_trap_threshold_rx_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s trap threshold rx enable.\n", port_name_to_show(device_gt->current_idx));
        set_if_trap_threshold_rx_status(port_idx_to_port(device_gt->current_idx), i2a[_IF_TRAP_THRESHOLD_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_if_trap_threshold_rx_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s trap threshold rx disable.\n", port_name_to_show(device_gt->current_idx));;
        set_if_trap_threshold_rx_status(port_idx_to_port(device_gt->current_idx), i2a[_IF_TRAP_THRESHOLD_MODE_DISABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_if_trap_threshold_tx_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a trap threshold tx value!\n");
        return CB_FAIL;
    }
    if(1>_atoi_s(value) || 99<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid trap threshold tx value:\"%d\"!\n", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    get_if_trap_threshold_tx_status_d(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _IF_TRAP_THRESHOLD_MODE_ENABLE:
            break;
        case _IF_TRAP_THRESHOLD_MODE_DISABLE:
            printf(" %% %s TX trap threshold mode is disable, can`t to set threshold value.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid trap threshold mode: %d!\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s trap threshold tx value: \"%s\"!\n", port_name_to_show(device_gt->current_idx), value);
    set_if_trap_threshold_tx_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}

int parse_if_trap_threshold_rx_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a trap threshold rx value!");
        return CB_FAIL;
    }
    if(1>_atoi_s(value) || 99<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid trap threshold rx value:\"%d\"!", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
    get_if_trap_threshold_rx_status_d(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _IF_TRAP_THRESHOLD_MODE_ENABLE:
            break;
        case _IF_TRAP_THRESHOLD_MODE_DISABLE:
            printf(" %% %s RX trap threshold mode is disable, can`t to set threshold value", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid trap threshold mode: %d", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s trap threshold rx value: \"%s\"!", port_name_to_show(device_gt->current_idx), value);
    set_if_trap_threshold_rx_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}

int parse_if_trap_port_link_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s trap port status enable", port_name_to_show(device_gt->current_idx));
        set_if_trap_port_link_status(port_idx_to_port(device_gt->current_idx), i2a[_IF_TRAP_PORT_STATUS_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!");
    return CB_FAIL;
}

int parse_if_trap_port_link_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s trap port status disable.\n", port_name_to_show(device_gt->current_idx));
        set_if_trap_port_link_status(port_idx_to_port(device_gt->current_idx), i2a[_IF_TRAP_PORT_STATUS_MODE_DISABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}


#if 0
int parse_port_config_set_admin_mode_all(char* argv){
    char *mode;
    char *extra;
    const char *mode_ubus;
    mode = strtok(argv, " ");
    extra = strtok(NULL, " ");
    if(NULL == mode){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port admin mode!");
        return CB_FAIL;
    }
    mode_ubus = port_config_admin_mode_to_mode(mode);
    if(NULL == mode_ubus){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port flow control mode!");
        return CB_FAIL;
    }
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
    printf(" %% Port config set global admin_mode %s", mode);
    port_config_set_admin_mode_all(mode_ubus);
    return CB_SUCCESS;
}
int parse_port_config_set_physical_mode_all(char* argv){
    char *mode;
    char *extra;
    const char *mode_ubus;
    mode = strtok(argv, " ");
    extra = strtok(NULL, " ");
    if(NULL == mode){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port speed!");
        return CB_FAIL;
    }
    mode_ubus = port_config_speed_to_mode(mode);
    if(NULL == mode_ubus){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port speed!");
        return CB_FAIL;
    }
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
    printf(" %% Port config set global speed %s", mode);
    port_config_set_physical_mode_all(mode_ubus);
    return CB_SUCCESS;
}
int parse_port_config_set_flow_control_mode_all(char* argv){
    char *mode;
    char *extra;
    const char *mode_ubus;
    mode = strtok(argv, " ");
    extra = strtok(NULL, " ");
    if(NULL == mode){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port flow control mode!");
        return CB_FAIL;
    }
    mode_ubus = port_config_flow_control_to_mode(mode);
    if(NULL == mode_ubus){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port flow control mode!");
        return CB_FAIL;
    }
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
    printf(" %% Port config set global flow_control %s", mode);
    port_config_set_flow_control_mode_all(mode_ubus);
    return CB_SUCCESS;
}
#endif


#if 0
int parse_port_config_get_config_all_all(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_config_all_all();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}

int parse_port_config_get_config_one_port_all(char* argv){
    char *port;
    char *extra;
    const char *port_ubus;
    port = strtok(argv, " ");
    extra = strtok(NULL, " ");
    if(NULL == port){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port!");
        return CB_FAIL;
    }
    port_ubus = port_name_to_ubus(port);
    if(port_ubus == NULL){
        posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port!");
        return CB_FAIL;
    }
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
    port_config_get_config_one_port_all(port_ubus);
    return CB_SUCCESS;
}

int parse_port_config_get_type_all_port(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_type_all_port();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}

int parse_port_config_get_admin_mode_all_port(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_admin_mode_all_port();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}
int parse_port_config_get_operational_status_all_port(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_operational_status_all_port();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}
int parse_port_config_get_physical_mode_all_port(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_physical_mode_all_port();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}

int parse_port_config_get_physical_status_all_port(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_physical_mode_all_port();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}
int parse_port_config_get_flow_control_mode_all_port(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_flow_control_mode_all_port();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}
int parse_port_config_get_dot1d_state_mode_all_port(char* argv){
    char *extra;
    extra = strtok(argv, " ");
    if(NULL == extra){
        port_config_get_dot1d_state_all_port();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
}
int parse_port_config_set_admin_mode_one(char* argv){
    char *port;
    char *mode;
    char *extra;
    const char *port_ubus;
    const char *mode_ubus;
    port  = strtok(argv, " ");
    mode  = strtok(NULL, " ");
    extra = strtok(NULL, " ");
    if(NULL == port){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port!");
        return CB_FAIL;
    }
    port_ubus = port_name_to_ubus(port);
    if(NULL == port_ubus){
        posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port!");
        return CB_FAIL;
    }
    if(NULL == mode){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port admin mode!");
        return CB_FAIL;
    }
    mode_ubus = port_config_admin_mode_to_mode(mode);
    if(NULL == mode_ubus){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port admin mode!");
        return CB_FAIL;
    }
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
    printf(" %% Port config set %s admin mode %s", port, mode);
    port_config_set_admin_mode_one(port_ubus, mode_ubus);
    return CB_SUCCESS;
}

int parse_port_config_set_physical_mode_one(char* argv){
    char *port;
    char *mode;
    char *extra;
    const char *port_ubus;
    const char *mode_ubus;
    port  = strtok(argv, " ");
    mode  = strtok(NULL, " ");
    extra = strtok(NULL, " ");
    if(NULL == port){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port!");
        return CB_FAIL;
    }
    port_ubus = port_name_to_ubus(port);
    if(NULL == port_ubus){
        posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port!");
        return CB_FAIL;
    }
    if(NULL == mode){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a port speed!");
        return CB_FAIL;
    }
    mode_ubus = port_config_speed_to_mode(mode);
    if(NULL == mode_ubus){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid port speed!");
        return CB_FAIL;
    }
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!");
        return CB_FAIL;
    }
    printf(" %% Port config set %s speed %s", port, mode);
    port_config_set_physical_mode_one(port_ubus, mode_ubus);
    return CB_SUCCESS;
}
#endif
