#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/lldp.h"

int parse_lldp_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set lldp enable.\n");
        lldp_set_status(i2a[_LLDP_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_lldp_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set lldp disable.\n");
        lldp_set_status(i2a[_LLDP_MODE_DISABLED]);
        return CB_SUCCESS;
    }
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_display_lldp_geighbor(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
	    device_gt->current_mode = 0;
		lldp_get_status();
		switch(device_gt->current_mode){
            case _LLDP_MODE_DISABLED:
                printf(" %% Now LLDP is disabled!\n");
                return CB_FAIL;
                break;
            case _LLDP_MODE_ENABLE:
                break;
            default:
                printf(" %% LLDP mode error!\n");
                return CB_FAIL;
                break;
		}
		lldp_get_entry_num();
		if(0 == device_gt->idx_num){
			printf(" %% No LLDP neighbor!\n");
			return CB_FAIL;
		}else if(0 > device_gt->idx_num){
			printf(" %% LLDP mode error!\n");
			return CB_FAIL;
		}
		lldp_get_entry_info();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}
