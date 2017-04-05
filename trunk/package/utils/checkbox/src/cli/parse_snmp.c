#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/port_config.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/snmp.h"

int parse_display_snmp(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
#if 0
		device_gt->current_mode = 0;
		snmp_get_status();
		switch(device_gt->current_mode){
				case 2:
					printf(" %% Now SNMP is disabled!");
					return CB_FAIL;
					break;
				case 1:
					break;
				default:
					printf(" %% SNMP mode error!");
					return CB_FAIL;
					break;
		}
#endif
		snmp_get_info();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_snmp_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set snmp enable.\n");
        snmp_set_status(i2a[_SNMP_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_snmp_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set snmp disable.\n");
        snmp_set_status(i2a[_SNMP_MODE_DISABLED]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_snmp_port(char* argv){
	char *port;
	char *extra;
	port = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == port){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a UDP port!\n");
		return CB_FAIL;
	}
	if(!is_num_d(port)){
		posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give valid UDP port!\n");
		return CB_FAIL;
	}
	if(0 >=atoi(port) || 65535 < atoi(port)){
		posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
		printf(" %% UDP port must be in 1~65535! Please give valid UDP port!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	device_gt->current_mode = 0;
	snmp_get_status();
	switch(device_gt->current_mode){
		case 2:
			printf(" %% Now SNMP is disabled!\n");
			return CB_FAIL;
			break;
		case 1:
			break;
		default:
			printf(" %% SNMP mode error!\n");
			return CB_FAIL;
			break;
	}
	printf(" %% Set SNMP port: [%s]!\n", port);
	snmp_set_port(port);
	return CB_SUCCESS;
}
int parse_snmp_readonly_name(char* argv){
	char *readonly_name;
	char *extra;
	readonly_name = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == readonly_name){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a read-only name!\n");
		return CB_FAIL;
	}
	if(3>strlen(readonly_name) || 64<strlen(readonly_name)){
		posi_err(strlen(_terminal_view->cmd_head) + (readonly_name-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Read-only name must be 3~64 char! Please give a valid read-only name!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	device_gt->current_mode = 0;
	snmp_get_status();
	switch(device_gt->current_mode){
		case 2:
			printf(" %% Now SNMP is disabled!\n");
			return CB_FAIL;
			break;
		case 1:
			break;
		default:
			printf(" %% SNMP mode error!\n");
			return CB_FAIL;
			break;
	}
	printf(" %% Set SNMP read-only name: [%s]!\n", readonly_name);
	snmp_set_readonly_name(readonly_name);
	return CB_SUCCESS;
}
int parse_snmp_readwrite_name(char* argv){
	char *readwrite_name;
	char *extra;
	readwrite_name = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == readwrite_name){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a read-only name!\n");
		return CB_FAIL;
	}
	if(3>strlen(readwrite_name) || 64<strlen(readwrite_name)){
		posi_err(strlen(_terminal_view->cmd_head) + (readwrite_name-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Read-write name must be 3~64 char! Please give a valid read-write name!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	device_gt->current_mode = 0;
	snmp_get_status();
	switch(device_gt->current_mode){
		case 2:
			printf(" %% Now SNMP is disabled!\n");
			return CB_FAIL;
			break;
		case 1:
			break;
		default:
			printf(" %% SNMP mode error!\n");
			return CB_FAIL;
			break;
	}
	printf(" %% Set SNMP read-write name: [%s]!\n", readwrite_name);
	snmp_set_readwrite_name(readwrite_name);
	return CB_SUCCESS;
}
int parse_display_snmp_trap(char* argv){
	char *extra;
	if(!(extra = strtok(argv, " "))){
#if 0
		device_gt->current_mode = 0;
		snmp_trap_get_status();
		switch(device_gt->current_mode){
				case 2:
					printf(" %% Now SNMP-Trap is disabled!");
					return CB_FAIL;
					break;
				case 1:
					break;
				default:
					printf(" %% SNMP-Trap mode error!");
					return CB_FAIL;
					break;
		}
#endif
	    HIGHT_LIGHT();
	    printf("%122s\n", "Trap:");
	    UN_HIGHT_LIGHT();
	    print_row(122);
		snmp_trap_get_info();
		power_trap_get_status();
		print_row(122);
		printf(" %-9s  %20s  %20s  %21s  %20s  %21s\n", "port", "link status trap", "TX threshold trap", "TX threshold value(%)", "RX threshold trap", "RX threshold value(%)");
		print_row(122);
		get_if_trap_all();
	    print_row(122);
	    fflush(stdout);
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_snmp_trap_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set snmp trap enable.\n");
        snmp_trap_set_status(i2a[_SNMP_TRAP_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_power_trap_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set trap power disable.\n");
        power_trap_set_status(i2a[_POWER_TRAP_MODE_DISABLED]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_power_trap_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set trap power enable.\n");
        power_trap_set_status(i2a[_POWER_TRAP_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_snmp_trap_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set snmp trap disable.\n");
        snmp_trap_set_status(i2a[_SNMP_TRAP_MODE_DISABLED]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_snmp_trap_port(char* argv){
	char *port;
	char *extra;
	port = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == port){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a UDP port!\n");
		return CB_FAIL;
	}
	if(!is_num_d(port)){
		posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give valid UDP port!\n");
		return CB_FAIL;
	}
	if(0 >=atoi(port) || 65535 < atoi(port)){
		posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
		printf(" %% UDP port must be in 1~65535! Please give valid UDP port!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	device_gt->current_mode = 0;
	snmp_get_status();
	switch(device_gt->current_mode){
		case 2:
			printf(" %% Now SNMP-Trap is disabled!\n");
			return CB_FAIL;
			break;
		case 1:
			break;
		default:
			printf(" %% SNMP-Trap mode error!\n");
			return CB_FAIL;
			break;
	}
	printf(" %% Set SNMP-trap port: [%s]!\n", port);
	snmp_trap_set_trap_port(port);
	return CB_SUCCESS;
}

int parse_snmp_trap_server_ip(char* argv){
	char *server_ip;
	char *extra;
	server_ip = strtok(argv, " ");
	extra = strtok(NULL, " ");
	if(NULL == server_ip){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a server ip!\n");
		return CB_FAIL;
	}
	if(CB_FAIL == ip_check(server_ip)){
		posi_err(strlen(_terminal_view->cmd_head) + (server_ip-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give valid server ip!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	device_gt->current_mode = 0;
	snmp_get_status();
	switch(device_gt->current_mode){
		case 2:
			printf(" %% Now SNMP-Trap is disabled!\n");
			return CB_FAIL;
			break;
		case 1:
			break;
		default:
			printf(" %% SNMP-Trap mode error!\n");
			return CB_FAIL;
			break;
	}
	printf(" %% Set SNMP-trap server ip: [%s]!\n", server_ip);
	snmp_trap_set_trap_server_ip(server_ip);
	return CB_SUCCESS;
}
