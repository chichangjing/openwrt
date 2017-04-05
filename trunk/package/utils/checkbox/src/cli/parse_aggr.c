#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/port_aggr.h"

static int num_1(int mask){
	int i;
	for(i=0;mask>0;mask=mask>>1)
		i += 1 == mask%1 ? 1:0;
	return i;
}

int parse_aggr_view_init(char* argv){
	char *extra;
	char *trunk_id;
	trunk_id = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == trunk_id){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give your trunk ID: <1~15>!\n");
		return CB_FAIL;
	}
	if(!is_num_d(trunk_id)){
		posi_err(strlen(_terminal_view->cmd_head) + (trunk_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid trunk ID: <1~15>!\n");
		return CB_FAIL;
	}
	if(0 >= atoi(trunk_id) || 15 < atoi(trunk_id)){
		posi_err(strlen(_terminal_view->cmd_head) + (trunk_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid trunk ID: <1~15>!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	memset(device_gt, 0, sizeof(struct device_gt));
	port_aggr_get_mode_select();
	switch(device_gt->current_mode){
		case _PORT_AGGR_MODE_STATIC:
			break;
		case _PORT_AGGR_MODE_DISABLED:
			printf(" %% Trunk is Disable!\n");
			return CB_FAIL;
			break;
		default:
			printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
			return CB_FAIL;
			break;
	}
	sprintf(device_gt->current_view_name, "%d", atoi(trunk_id));
	port_aggr_get_channel_num();
	if(0 > device_gt->idx_num){
		printf(" %% Switch config error: [%d]!\n", device_gt->idx_num);
		return CB_FAIL;
	}
    if(8 < device_gt->idx_num){
        printf(" %% Can`t add more trunk group!\n");
        return CB_FAIL;
    }
	if(0 < device_gt->idx_num){
		port_aggr_get_channel_id_one_d();
	}
	if(!*device_gt->current_idx && 8 == device_gt->idx_num){
        printf(" %% Trunk group \"%s\" is not exsit! \n %% Can`t add more trunk group!\n", device_gt->current_view_name);
        return CB_FAIL;
	}
	if(!*device_gt->current_idx){
		port_aggr_set_channel_id_one_d();
		printf(" %% New trunk group \"%s\".\n", device_gt->current_view_name);
	}
    memset(_terminal_view->cmd_head, 0, sizeof(_terminal_view->cmd_head));
    sprintf(_terminal_view->cmd_head, "[%s%d]# ", cli_view_name_tbl[PORT_TRUNK_VIEW], atoi(trunk_id));
    _terminal_view->_cmd_list->cmd_num  =   _view_aggr_list->cmd_num;
    _terminal_view->_cmd_list->_cmd_    =   _view_aggr_list->_cmd_;
    _terminal_view->cursor  =   0;
	return 1;
}

int parse_port_aggr_get_mode_select(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		port_aggr_get_mode_select();
		switch(device_gt->current_mode){
			case _PORT_AGGR_MODE_STATIC:
				printf(" %% Now trunk mode is Static!\n");
				return CB_SUCCESS;
				break;
			case _PORT_AGGR_MODE_DISABLED:
				printf(" %% Now trunk mode is Disable!\n");
				return CB_FAIL;
				break;
			default:
				printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
				return CB_FAIL;
				break;
		}
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_port_aggr_get_load_balance(char * argv){
	char * extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		port_aggr_get_mode_select();
		switch(device_gt->current_mode){
			case _PORT_AGGR_MODE_STATIC:
				printf(" %% Now trunk mode is Static!\n");
				break;
			case _PORT_AGGR_MODE_DISABLED:
				printf(" %% Now trunk is Disable!\n");
				return CB_FAIL;
				break;
			default:
				printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
				return CB_FAIL;
				break;
		}
		port_aggr_get_load_balance();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_port_aggr_get_channel_all(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		port_aggr_get_mode_select();
		switch(device_gt->current_mode){
			case _PORT_AGGR_MODE_STATIC:
				printf(" %% Now trunk mode is Static!\n");
				break;
			case _PORT_AGGR_MODE_DISABLED:
				printf(" %% Now trunk mode is Disable!\n");
				return CB_FAIL;
				break;
			default:
				printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
				return CB_FAIL;
				break;
		}
		port_aggr_get_channel_num();
		if(0 > device_gt->idx_num){
			printf(" %% Switch config error: [%d]!\n", device_gt->idx_num);
			return CB_FAIL;
		}
		if(!device_gt->idx_num){
			printf(" %% No trunk group set yet!\n");
			return CB_SUCCESS;
		}
		port_aggr_get_channel_all();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}


int parse_aggr_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set trunk enable");
        port_aggr_set_mode_select(i2a[_PORT_AGGR_MODE_STATIC]);
        return CB_SUCCESS;
    }
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_aggr_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set trunk disable");
        port_aggr_set_mode_select(i2a[_PORT_AGGR_MODE_DISABLED]);
        return CB_SUCCESS;
    }
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_port_aggr_set_load_balance(char* argv){
	char *mode;
	char *extra;
	const char *mode_ubus;
	mode = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == mode){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a trunk load balance mode!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	port_aggr_get_mode_select();
	switch(device_gt->current_mode){
		case _PORT_AGGR_MODE_STATIC:
			break;
		case _PORT_AGGR_MODE_DISABLED:
			printf(" %% Now trunk mode is Disable!\n");
			return CB_FAIL;
			break;
		default:
			printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
			return CB_FAIL;
			break;
	}
	mode_ubus = port_aggr_load_balance_to_mode(mode);
	if(NULL == mode_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
		printf(" %% The trunk load balance mode is Invalid !\n");
		return CB_FAIL;
	}
	printf(" %% Port trunk set load balance mode: %s\n", mode);
	port_aggr_set_load_balance(mode_ubus);
	return CB_SUCCESS;
}

int parse_port_aggr_set_channel_member_one(char* argv){
	char * extra;
	char * member;
	char channel_member[8];
	int mark = 0;
	int i=0, j=0, tmp;
	int member_mask = 0x0000;
	port_aggr_get_mode_select();
	switch(device_gt->current_mode){
		case _PORT_AGGR_MODE_STATIC:
			break;
		case _PORT_AGGR_MODE_DISABLED:
			printf(" %% Now trunk mode is Disable!\n");
			return CB_FAIL;
			break;
		default:
			printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
			return CB_FAIL;
			break;
	}
	for(i=0; i<_AGGR_PORT; i++){
		if(0 == i){
			member = strtok(argv, " ");
			if(NULL == member){
				posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
				printf(" %% Please give a trunk member port!\n");
				return CB_FAIL;
			}
		}else{
			member = strtok(NULL, " ");
		}
		if(NULL != member){
			for(j=PORT_1, mark=0; j<_PORT_MAX; j++){
				if(0 == strcmp(member, port_name[2][j])){
					member_mask = member_mask | (1<<(16-j));
					mark = 1;
					break;
				}
			}
			if(0 == mark){
				posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
				printf(" %% Please give a valid member port!\n");
				return CB_FAIL;
			}
		}
	}
	extra = strtok(NULL, " ");
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	memset(device_gt->current_idx, 0, sizeof(device_gt->current_idx));
	port_aggr_get_channel_num();
	if(0 > device_gt->idx_num){
		printf(" %% Switch config error: [%d]!\n", device_gt->idx_num);
		return CB_FAIL;
	}
	if(0 < device_gt->idx_num){
		port_aggr_get_channel_id_one_d();
	}
	if(!*device_gt->current_idx){
		port_aggr_set_channel_id_one_d();
		printf(" %% New trunk group \"%s\".\n", device_gt->current_view_name);
	}
	port_aggr_get_channel_member_one(device_gt->current_idx);
	port_aggr_get_channel_member_all();
	if(0>device_gt->vlan_port_list || 0>device_gt->vlan_tag_list){
		printf(" %% Switch Config error : [%d][%d]!\n", device_gt->vlan_port_list, device_gt->vlan_tag_list);
		return CB_FAIL;
	}
	device_gt->vlan_tag_list = ~device_gt->vlan_port_list & device_gt->vlan_tag_list;
	tmp = member_mask & device_gt->vlan_tag_list;
	if(1<tmp){
		printf(" %% Ports in Other Group:");
		for(i=PORT_1; i<_PORT_MAX; i++){
			if(1 <= tmp/0x8000)
				printf(" %s", port_name[1][i]);
			tmp = tmp%0x8000;
			tmp = tmp<<1;
		}
		printf(".\n");
		fflush(stdout);
		return CB_FAIL;
	}
	if(_AGGR_PORT<num_1(member_mask)){
		printf(" %% Too many port set!\n");
		return CB_FAIL;
	}
	sprintf(channel_member, "%02x-%02x%c", member_mask>>8, 0x00ff & member_mask, '\0');
	printf(" %% Trunk Group %s set member: [%s].\n", device_gt->current_view_name, channel_member);
	port_aggr_set_channel_member_one(device_gt->current_idx, channel_member);
	return CB_SUCCESS;
}

int parse_port_aggr_add_channel_member_one(char* argv){
	char * extra;
	char * member;
	char channel_member[8];
	int tmp = 1;
	int mark = 0;
	int i=0, j=0;
	int member_mask = 0x0000;
	port_aggr_get_mode_select();
	switch(device_gt->current_mode){
		case _PORT_AGGR_MODE_STATIC:
			break;
		case _PORT_AGGR_MODE_DISABLED:
			printf(" %% Now trunk mode is Disable!\n");
			return CB_FAIL;
			break;
		default:
			printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
			return CB_FAIL;
			break;
	}
	for(i=0; i<_AGGR_PORT; i++){
		if(0 == i){
			member = strtok(argv, " ");
			if(NULL == member){
				posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
				printf(" %% Please give a trunk member port!\n");
				return CB_FAIL;
			}
		}else{
			member = strtok(NULL, " ");
		}
		if(NULL != member){
			for(j=PORT_1, mark=0; j<_PORT_MAX; j++){
				if(0 == strcmp(member, port_name[2][j])){
					member_mask = member_mask | 1<<(16-j);
					mark = 1;
					break;
				}
			}
			if(0 == mark){
				posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
				printf(" %% Please give a valid member port!\n");
				return CB_FAIL;
			}
		}
	}
    extra = strtok(NULL, " ");
    if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
    memset(device_gt->current_idx, 0, sizeof(device_gt->current_idx));
    port_aggr_get_channel_num();
	if(0 > device_gt->idx_num){
		printf(" %% Switch config error: [%d]!\n", device_gt->idx_num);
		return CB_FAIL;
	}
	if(0 < device_gt->idx_num){
		port_aggr_get_channel_id_one_d();
	}
	if(!*device_gt->current_idx){
		port_aggr_set_channel_id_one_d();
		printf(" %% New trunk group \"%s\".\n", device_gt->current_view_name);
	}
	port_aggr_get_channel_member_one(device_gt->current_idx);
	port_aggr_get_channel_member_all();
	if(0>device_gt->vlan_port_list || 0>device_gt->vlan_tag_list){
		printf(" %% Switch Config error : [%d][%d]!\n", device_gt->vlan_port_list, device_gt->vlan_tag_list);
		return CB_FAIL;
	}
	member_mask = device_gt->vlan_port_list | member_mask;
	device_gt->vlan_tag_list = ~device_gt->vlan_port_list & device_gt->vlan_tag_list;
	tmp = member_mask & device_gt->vlan_tag_list;
	if(1<tmp){
		printf(" %% Ports in Other Group:");
		fflush(stdout);
		for(i=PORT_1;i<_PORT_MAX;i++){
			if(1 <= tmp/0x8000){
				printf(" %s", port_name[1][i]);
			}
			tmp = tmp%0x8000;
			tmp = tmp<<1;
		}
		printf(".\n");
		return CB_FAIL;
	}
	if(_AGGR_PORT<num_1(member_mask)){
		printf(" %% Too many port set!\n");
		return CB_FAIL;
	}
	sprintf(channel_member, "%02x-%02x%c", member_mask>>8, 0x00ff & member_mask, '\0');
	printf(" %% Trunk Group %s set member: [%s].\n", device_gt->current_view_name, channel_member);
	port_aggr_set_channel_member_one(device_gt->current_idx, channel_member);
	return CB_SUCCESS;
}

int parse_port_aggr_del_channel_member_one(char* argv){
	char *extra;
	char *member;
	char channel_member[8];
	int tmp = 1;
	int mark = 0;
	int i=0, j=0;
	int member_mask = 0x0000;
    port_aggr_get_mode_select();
    switch(device_gt->current_mode){
        case _PORT_AGGR_MODE_STATIC:
            break;
        case _PORT_AGGR_MODE_DISABLED:
            printf(" %% Now trunk mode is Disable!\n");
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid trunk mode: [%d]!\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    for(i=0; i<_AGGR_PORT; i++){
        if(0 == i){
            member = strtok(argv, " ");
            if(NULL == member){
                posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
                printf(" %% Please give a trunk member port!\n");
                return CB_FAIL;
            }
        }else{
            member = strtok(NULL, " ");
        }
        if(NULL != member){
            for(j=PORT_1, mark=0; j<_PORT_MAX; j++){
                if(0 == strcmp(member, port_name[2][j])){
                    member_mask = member_mask | 1<<(16-j);
                    mark = 1;
                    break;
                }
            }
            if(0 == mark){
                posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
                printf(" %% Please give a valid member port!\n");
                return CB_FAIL;
            }
        }
    }
    extra = strtok(NULL, " ");
    if(NULL != extra){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    memset(device_gt->current_idx, 0, sizeof(device_gt->current_idx));
    port_aggr_get_channel_num();
    if(0 > device_gt->idx_num){
        printf(" %% Switch config error: [%d]!\n", device_gt->idx_num);
        return CB_FAIL;
    }
    if(0 < device_gt->idx_num){
        port_aggr_get_channel_id_one_d();
    }
    if(!*device_gt->current_idx){
        printf(" %% No trunk group [%s] exsit!\n", device_gt->current_view_name);
        printf(" %% Exit trunk View!\n");
        memset(device_gt, 0, sizeof(struct device_gt));
        memset(_terminal_view, 0, sizeof(struct _terminal_view));
        view_init(device_name, _view_system_list);
        return CB_FAIL;
    }
    port_aggr_get_channel_member_one(device_gt->current_idx);
    port_aggr_get_channel_member_all();
	if(0>device_gt->vlan_port_list || 0>device_gt->vlan_tag_list){
		printf(" %% Switch Config error : [%d][%d]!\n", device_gt->vlan_port_list, device_gt->vlan_tag_list);
		return CB_FAIL;
	}
	member_mask = ~member_mask & device_gt->vlan_port_list;
	device_gt->vlan_tag_list = ~device_gt->vlan_port_list & device_gt->vlan_tag_list;
	tmp = member_mask & device_gt->vlan_tag_list;
	if(1<tmp){
		printf(" %% Ports in Other Group:");
		fflush(stdout);
		for(i=PORT_1;i<_PORT_MAX;i++){
			if(1 <= tmp/0x8000){
				printf(" %s", port_name[1][i]);
			}
			tmp = tmp%0x8000;
			tmp = tmp<<1;
		}
		printf(".\n");
		return CB_FAIL;
	}
	if(_AGGR_PORT<num_1(member_mask)){
		printf(" %% Too many port set!\n");
		return CB_FAIL;
	}
	sprintf(channel_member, "%02x-%02x%c", member_mask>>8, 0x00ff & member_mask, '\0');
	printf(" %% Trunk Group %s set member: [%s].\n", device_gt->current_view_name, channel_member);
	port_aggr_set_channel_member_one(device_gt->current_idx, channel_member);
	return CB_SUCCESS;
}

int parse_port_aggr_clear_one(char* argv){
	if(NULL == strtok(argv, " ")){
		port_aggr_get_mode_select();
		switch(device_gt->current_mode){
			case _PORT_AGGR_MODE_STATIC:
				break;
			case _PORT_AGGR_MODE_DISABLED:
				printf(" %% Now trunk mode is Disabled!\n");
				view_init(device_name, _view_system_list);;
				return CB_FAIL;
				break;
			default:
				printf(" %% Switch config error: [%d]!\n", device_gt->current_mode);
				return CB_FAIL;
				break;
		}
		memset(device_gt->current_idx, 0, sizeof(device_gt->current_idx));
		port_aggr_get_channel_num();
		if(0 > device_gt->idx_num){
			printf(" %% Switch config error: [%d]!\n", device_gt->idx_num);
			return CB_FAIL;
		}
		if(0 < device_gt->idx_num){
			port_aggr_get_channel_id_one_d();
		}
		if(!*device_gt->current_idx){
			printf(" %% No trunk group [%s] exsit!\n", device_gt->current_view_name);
			printf(" %% Exit trunk View!\n");
			memset(device_gt, 0, sizeof(struct device_gt));
	       memset(_terminal_view, 0, sizeof(struct _terminal_view));
	       view_init(device_name, _view_system_list);
			return CB_FAIL;
		}
		printf(" %% Clear trunk Group: [%s]!\n", device_gt->current_view_name);
		port_aggr_clear_one(device_gt->current_idx);
		printf(" %% Exit trunk View!\n");
       memset(device_gt, 0, sizeof(struct device_gt));
       memset(_terminal_view, 0, sizeof(struct _terminal_view));
		view_init(device_name, _view_system_list);
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (argv-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

