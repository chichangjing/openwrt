#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/mcast_config.h"
#include "../caselib/cs_type.h"

int parse_mcast_view_init(char* argv){
	char * mac_addr;
	char * extra;
	if(!(mac_addr = strtok(argv, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give your Multicast address!\n");
		return CB_FAIL;
	}
	if(!is_mac(mac_addr) || mac_addr[1]%2=='0'%2){
		posi_err(strlen(_terminal_view->cmd_head) + (mac_addr-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid Multicast address!\n");
		return CB_FAIL;
	}
	if(is_manag_mcast(str2mac(mac_addr))){
        posi_err(strlen(_terminal_view->cmd_head) + (mac_addr-(char*)&in_put_buf)/sizeof(char));
        printf(" %% %s is used as a mangement address! Can`t to be configure!\n", mac_addr);
        return CB_FAIL;
	}
	if((extra = strtok(NULL, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	mcast_get_general();
    switch(global_mcast->mode){
        case MCAST_ENABLE_STATUS_ENABLE:
            printf(" %% Now Multicast is Enable.\n");
            break;
        case MCAST_ENABLE_STATUS_DISABLE:
            printf(" %% Now Multicast is Disabled!\n");
            break;
        default:
            printf(" %% Switch config error: [%d]!\n", global_mcast->mode);
            break;
    }
    if(mac2idx(str2mac(mac_addr))){
        printf(" %% %s is exsit. Enter multicast view.\n", mac_addr);
    }else{
        printf(" %% %s is not exsit. Add new multicast address.\n", mac_addr);
        mcast_config_set_mac_address_one(id2idx(global_mcast->count+1), mac2str(str2mac(mac_addr), _base_mac_3));
    }
    memset(device_gt, 0, sizeof(struct device_gt));
    device_gt->view_name.mac = str2mac(mac_addr);
    memset(_terminal_view->cmd_head, 0, sizeof(_terminal_view->cmd_head));
	sprintf(_terminal_view->cmd_head, "[%s%s]# ", cli_view_name_tbl[MULTICAST_VIEW], mac_addr);
    _terminal_view->_cmd_list->cmd_num  =   _view_mcast_list->cmd_num;
    _terminal_view->_cmd_list->_cmd_    =   _view_mcast_list->_cmd_;
    _terminal_view->cursor  =   0;
	return CB_SUCCESS;
}
int parse_mcast_config_get_group_members(char* argv){
	char * extra;
	if(!(extra = strtok(argv, " "))){
	    mcast_get_general();
	    mcast_list();
		switch(global_mcast->mode){
			case MCAST_ENABLE_STATUS_ENABLE:
				printf(" %% Now Multicast is Enable.\n");
				break;
			case MCAST_ENABLE_STATUS_DISABLE:
				printf(" %% Now Multicast is Disabled!\n");
				break;
			default:
				printf(" %% Switch config error: [%d]!\n", global_mcast->mode);
				break;
		}
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_mcast_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set Multicast enable.\n");
        mcast_config_set_enable_status(i2a[MCAST_ENABLE_STATUS_ENABLE]);
        return CB_SUCCESS;
    }
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_mcast_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set Multicast disable.\n");
        mcast_config_set_enable_status(i2a[MCAST_ENABLE_STATUS_DISABLE]);
        return CB_SUCCESS;
    }
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_mcast_set_group_members_one(char* argv){
	char * extra;
	char * member;
	char mcast_member[10];
	int j=0;
	int member_mask = 0;
	if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
	}
	for(; member; member = strtok(NULL, " ")){
	    j = str_idx(member, port_name[2]);
	    if(PORT_1>j || _PORT_MAX<=j){
	        posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
	        printf(" %% Please give a valid member port!\n");
	        return CB_FAIL;
	    }else{
	        member_mask |= (1<<(16-j));
	    }
	}
	if((extra = strtok(NULL, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	mcast_get_general();
    if(!mac2idx(device_gt->view_name.mac)){
        printf(" %% %s is not exsit.\n", mac2str(device_gt->view_name.mac, _base_mac_2));
        return CB_FAIL;
    }
	sprintf(mcast_member, "%02x-%02x%c", member_mask>>8, 0x00ff & member_mask, '\0');
	printf(" %% Multicast %s Set Member: [%s].\n", mac2str(device_gt->view_name.mac, _base_mac_2), mcast_member);
	mcast_config_set_group_members_one(id2idx(mac2idx(device_gt->view_name.mac)), mcast_member);
	return CB_SUCCESS;
}

int parse_mcast_add_group_members_one(char* argv){
    char * extra;
    char * member;
    char mcast_member[10];
    int j=0;
    int member_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            member_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    mcast_get_general();
    if(!mac2idx(device_gt->view_name.mac)){
        printf(" %% %s is not exsit.\n", mac2str(device_gt->view_name.mac, _base_mac_2));
        return CB_FAIL;
    }

	member_mask = member_mask | global_mcast->dev_mcast[mac2idx(device_gt->view_name.mac)-1].port;
	sprintf(mcast_member, "%02x-%02x%c", member_mask>>8, 0x00ff & member_mask, '\0');
	printf(" %% Multicast %s Set Member: [%s].\n", mac2str(device_gt->view_name.mac, _base_mac_2), mcast_member);
	mcast_config_set_group_members_one(id2idx(mac2idx(device_gt->view_name.mac)), mcast_member);
	return CB_SUCCESS;
}

int parse_mcast_del_group_members_one(char* argv){
    char * extra;
    char * member;
    char mcast_member[10];
    int j=0;
    int member_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            member_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    mcast_get_general();
    if(!mac2idx(device_gt->view_name.mac)){
        printf(" %% %s is not exsit.\n", mac2str(device_gt->view_name.mac, _base_mac_2));
        return CB_FAIL;
    }


	member_mask = ~member_mask & global_mcast->dev_mcast[mac2idx(device_gt->view_name.mac)-1].port;
	sprintf(mcast_member, "%02x-%02x%c", member_mask>>8, 0x00ff & member_mask, '\0');
	printf(" %% Multicast %s Set Member: [%s].\n", mac2str(device_gt->view_name.mac, _base_mac_2), mcast_member);
	mcast_config_set_group_members_one(id2idx(mac2idx(device_gt->view_name.mac)), mcast_member);
	return CB_SUCCESS;
}

int parse_mcast_clear(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
	    mcast_get_general();
#if 0
	    switch(global_mcast->mode){
	        case MCAST_ENABLE_STATUS_ENABLE:
	            printf(" %% Now Multicast is Enable.\n");
	            break;
	        case MCAST_ENABLE_STATUS_DISABLE:
	            printf(" %% Now Multicast is Disabled!\n");
	            break;
	        default:
	            printf(" %% Switch config error: [%d]!\n", global_mcast->mode);
	            break;
	    }
#endif
	    if(mac2idx(device_gt->view_name.mac)){
	        printf(" %% Clear multicast %s. Exit multicast view.\n", mac2str(device_gt->view_name.mac, _base_mac_2));
	        mcast_config_clear_one(id2idx(mac2idx(device_gt->view_name.mac)));
	    }else{
	        printf(" %% %s is not exsit. Exit multicast view.\n", mac2str(device_gt->view_name.mac, _base_mac_2));
	    }
       memset(device_gt, 0, sizeof(struct device_gt));
       memset(_terminal_view, 0, sizeof(struct _terminal_view));
		view_init(device_name, _view_system_list);;
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}
