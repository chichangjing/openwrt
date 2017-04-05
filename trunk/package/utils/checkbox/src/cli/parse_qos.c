#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/qos.h"

int parse_display_dscp_map_table(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		qos_get_dscp_queue();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_display_p8021_map_table(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		qos_get_P8021_queue();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_p8021_map_table_set(char* argv){
	char *p8021_id;
	char *p8021_priority;
	char *extra;
	p8021_id = strtok(argv, " ");
	p8021_priority = strtok(NULL, " ");
	extra = strtok(NULL, " ");
	if(NULL == p8021_id){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a 802.1p priority: <0~7>!\n");
		return CB_FAIL;
	}
	if(strlen(p8021_id)>1 || 0>*p8021_id-0x30 || 7 < *p8021_id-0x30){
		posi_err(strlen(_terminal_view->cmd_head) + (p8021_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid 802.1p priority: <0~7>!\n");
		return CB_FAIL;
	}
	if(NULL == p8021_priority){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a map queue: <0~3>!\n");
		return CB_FAIL;
	}
	if(strlen(p8021_priority)>1 || 0>*p8021_priority-0x30 || 3 < *p8021_priority-0x30){
		posi_err(strlen(_terminal_view->cmd_head) + (p8021_priority-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid map queue: <0~3>!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	printf(" %% Set 802.1p priority [%s] map queue [%s]!\n", p8021_id, p8021_priority);
	qos_set_P8021_queue(p8021_id, p8021_priority);
	return CB_SUCCESS;
}

int parse_dscp_map_table_set(char* argv){
	char *dscp_id;
	char *dscp_priority;
	char *extra;
	dscp_id = strtok(argv, " ");
	dscp_priority = strtok(NULL, " ");
	extra = strtok(NULL, " ");
	if(NULL == dscp_id){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a DSCP priority: <0~63>!\n");
		return CB_FAIL;
	}
	if(!is_num_d(dscp_id)){
		posi_err(strlen(_terminal_view->cmd_head) + (dscp_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid DSCP priority: <0~63>!\n");
		return CB_FAIL;
	}
	if(strlen(dscp_id)>2){
		posi_err(strlen(_terminal_view->cmd_head) + (dscp_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid DSCP priority: <0~63>!\n");
		return CB_FAIL;
	}
	if(0>*dscp_id-0x30 || 9 < *dscp_id-0x30){
		posi_err(strlen(_terminal_view->cmd_head) + (dscp_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid DSCP priority: <0~63>!\n");
		return CB_FAIL;
	}
	if(2==strlen(dscp_id)){
		if(0>*(dscp_id+1)-0x30 || 9 < *(dscp_id+1)-0x30){
			posi_err(strlen(_terminal_view->cmd_head) + (dscp_id-(char*)&in_put_buf)/sizeof(char));
			printf(" %% Please give a valid DSCP priority: <0~63>!\n");
			return CB_FAIL;
		}
	}
	if(0>atoi(dscp_id) || 63<atoi(dscp_id)){
		posi_err(strlen(_terminal_view->cmd_head) + (dscp_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid DSCP priority: <0~63>!\n");
		return CB_FAIL;
	}
	if(NULL == dscp_priority){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a map queue: <0~3>!\n");
		return CB_FAIL;
	}
	if(strlen(dscp_priority)>1){
		posi_err(strlen(_terminal_view->cmd_head) + (dscp_priority-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid map queue: <0~3>!\n");
		return CB_FAIL;
	}
	if(0>*dscp_priority-0x30 || 3 < *dscp_priority-0x30){
		posi_err(strlen(_terminal_view->cmd_head) + (dscp_priority-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid map queue: <0~3>!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	printf(" %% Set 802.1p priority [%s] map queue [%s]!\n", dscp_id, dscp_priority);
	qos_set_dscp_queue(dscp_id, dscp_priority);
	return CB_SUCCESS;
}

