#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/eth_stats.h"

static void eth_stats_get_all(int __signal){
    if(__signal==SIGINT){
        signal(SIGINT,  SIG_IGN);
        RESET_CURSOR();
        eth_stats_get_all_all_row();
        siglongjmp(_loop_env, 1);
    }
}

int parse_eth_stats_get_all_all(char* argv){
    signal(SIGINT,  eth_stats_get_all);
    char * extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
	    CLEAR();
		while(1){
			RESET_CURSOR();
			alarm(CLI_TIMEOUT);
			eth_stats_get_all_all_row();
			usleep(500000);
		}
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_clear_statistics_all(char* argv){
    char * extra = strtok(argv, " ");
    if(NULL == extra){
        printf(" %% Clear statistics on ports.\n");
        eth_stats_clear();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

