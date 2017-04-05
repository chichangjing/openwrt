#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/port_mirror.h"
int parse_port_mirror_get_destination_port(char * argv){
	char * extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		port_mirror_get_destination_port();
		switch(device_gt->current_mode){
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
				printf(" %% Mirroring destnation : [%s]\n", port_name[1][device_gt->current_mode]);
				return CB_SUCCESS;
				break;
			default:
				printf(" %% No destnation set yet! Please set a destnation port!\n");
				return CB_FAIL;
				break;
		}
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_port_mirror_get_mode_all(char * argv){
	char * extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		port_mirror_get_destination_port();
		switch(device_gt->current_mode){
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
				port_mirror_get_mode_all();
				return CB_SUCCESS;
				break;
			default:
				printf(" %% No destnation set yet! Please set a destnation port!\n");
				return CB_FAIL;
				break;
		}
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_port_mirror_get_mode_one(char * argv){
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
	port_mirror_get_destination_port();
		switch(device_gt->current_mode){
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
				if(0 == strcmp(port_name[2][device_gt->current_mode], port_ubus)){
					printf(" %% %s is the destnation Port!\n", port_name[1][device_gt->current_mode]);
					return CB_SUCCESS;
				}
				port_mirror_get_mode_one(port_ubus);
				return CB_SUCCESS;
				break;
			default:
				printf(" %% No destnation Set! Please set a destnation port!\n");
				return CB_FAIL;
				break;
		}
	return CB_SUCCESS;
}

int parse_port_mirror_set_destination(char * argv){
	char * port;
	char * extra;
	const char * port_ubus;
	port = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(port == NULL){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char *)(&in_put_buf)));
		printf(" %% Please give a port!\n");
		return CB_FAIL;
	}
	port_ubus = port_name_to_num(port);
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
	printf(" %% Port mirror set destination %s.\n", port);
	port_mirror_set_destination(port_ubus);
	return CB_SUCCESS;
}

int parse_port_mirror_disable(char * argv){
	char * extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		port_mirror_set_destination("0");
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_port_mirror_set_mode_all(char * argv){
	char * mode;
	char * extra;
	const char * mode_ubus;
	mode = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == mode){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char *)(&in_put_buf)));
		printf(" %% Please give a port mirroring mode!\n");
		return CB_FAIL;
	}
	mode_ubus = port_mirror_mode_to_mode(mode);
	if(NULL == mode_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port mirroring mode!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	port_mirror_get_destination_port();
	switch(device_gt->current_mode){
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
			printf(" %% Port mirror set global mode %s.\n", mode);
			port_mirror_set_mode_all(mode_ubus);
			return CB_SUCCESS;
			break;
		default:
			printf(" %% No destnation Set! Please set a destnation port!\n");
			return CB_FAIL;
			break;
	}
	return CB_SUCCESS;
}

int parse_port_mirror_set_mode_one(char * argv){
	char * port;
	char * mode;
	char * extra;
	const char * port_ubus;
	const char * mode_ubus;
	port  = strtok(argv, " ");
	mode  = strtok(NULL, " ");
	extra = strtok(NULL, " ");
	if(NULL == port){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char *)(&in_put_buf)));
		printf(" %% Please give a port!\n");
		return CB_FAIL;
	}
	port_ubus = port_name_to_port(port);
	if(NULL == port_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (port-(char *)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port!\n");
		return CB_FAIL;
	}
	if(NULL == mode){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char *)(&in_put_buf)));
		printf(" %% Please give a port mirroring mode!\n");
		return CB_FAIL;
	}
	mode_ubus = port_mirror_mode_to_mode(mode);
	if(NULL == mode_ubus){
		posi_err(strlen(_terminal_view->cmd_head) + (mode-(char *)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid port mirroring mode!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	port_mirror_get_destination_port();
	switch(device_gt->current_mode){
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
			if(0 == strcmp(port_name[2][device_gt->current_mode], port_ubus)){
				printf(" %% %s is the destnation Port!\n", port_name[1][device_gt->current_mode]);
				return CB_FAIL;
			}
			printf(" %% Port mirror set %s mode %s.\n", port, mode);
			port_mirror_set_mode_one(port_ubus, mode_ubus);
			return CB_SUCCESS;
			break;
		default:
			printf(" %% No destnation Set! Please set a destnation port!\n");
			return CB_FAIL;
			break;
	}
	return CB_SUCCESS;
}
