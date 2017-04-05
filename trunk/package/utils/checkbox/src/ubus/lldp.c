#include "ubus_call.h"
#include "device_info.h"
#include "lldp.h"

static void recive_lldp_get_status(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				device_gt->current_mode = atoi(blobmsg_get_string(lvl[3]));
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_lldp_get_entry_num(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				device_gt->idx_num = atoi(blobmsg_get_string(lvl[3]));
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_lldp_get_neighbor(struct ubus_request *req, int type, struct blob_attr *msg){
	int i;
	int rem[6];
	char *desc = NULL;
	char *tmp;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
				i=0;
				blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
				if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						switch (i){
							case 0:
								HIGHT_LIGHT();
								printf("%-24s%-4s%80s\n",  " Local Port",     "", port_name[2][atoi(blobmsg_get_string(lvl[5]))]);
								UN_HIGHT_LIGHT();
								print_row(108);
								break;
							case 1:
								printf("%-24s%-4s%-30s\n", " Neighbor Port",  ":", blobmsg_get_string(lvl[5]));
								break;
							case 2:
								printf("%-24s%-4s%-30s\n", " Neighbor IP",    ":", blobmsg_get_string(lvl[5]));
								break;
							case 3:
								printf("%-24s%-4s%-30s\n", " Neighbor MAC",   ":", blobmsg_get_string(lvl[5]));
								break;
							case 4:
								desc = malloc(strlen(blobmsg_get_string(lvl[5]))+1);
								strcpy(desc, blobmsg_get_string(lvl[5]));
								tmp = strtok(desc, "\n");
								printf("%-24s%-4s%s\n", " Neighbor Name", ":", tmp);
								for(tmp = strtok(NULL, "\n");NULL != tmp; tmp = strtok(NULL, "\n"))
									printf("%-24s%-4s%s\n", "'", "", tmp);
								free(desc);
								desc = NULL;
								break;
							case 5:
								desc = malloc(strlen(blobmsg_get_string(lvl[5]))+1);
								strcpy(desc, blobmsg_get_string(lvl[5]));
								tmp = strtok(desc, "\n");
								printf("%-24s%-4s%s\n", " Neighbor Description", ":", tmp);
								for(tmp = strtok(NULL, "\n");NULL != tmp; tmp = strtok(NULL, "\n"))
									printf("%-24s%-4s%s\n", "", "", tmp);
								free(desc);
								desc = NULL;
								break;
						}
						i++;
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(108);
	fflush(stdout);
}

static void recive_lldp_set_status(struct ubus_request *req, int type, 	struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Set LLDP status successfully.\n")
		else
			_PRINT_INFO(" %% Set LLDP status failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void lldp_get_status(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "admin_status");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "lldp_config";
	const char *method = "lldp_config_get";

	ubus_call(path, method, &buf, &recive_lldp_get_status);
}

void lldp_set_status(const char *mode){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "admin_status", mode);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "lldp_config";
	const char *method = "lldp_config_set";

	ubus_call(path, method, &buf, &recive_lldp_set_status);
}

void lldp_get_entry_num(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "entry_num");
	blobmsg_close_array(&buf, tb[0]);

	const   char    *path   =   "lldp_config";
	const   char    *method =   "lldp_config_get";

	ubus_call(path, method, &buf, &recive_lldp_get_entry_num);
}

void lldp_get_entry_info(void){
	int i;
	char    idx[10];
	void    *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=1; i<=device_gt->idx_num; i++){
			tb[1] = blobmsg_open_table(&buf, NULL);
			sprintf(idx, "%4s%-d", "idx-", i);
			tb[2] = blobmsg_open_array(&buf, idx);
				blobmsg_add_string(&buf, NULL, "local_port_number");
				blobmsg_add_string(&buf, NULL, "remote_port_description");
				blobmsg_add_string(&buf, NULL, "remote_ip");
				blobmsg_add_string(&buf, NULL, "remote_mac");
				blobmsg_add_string(&buf, NULL, "remote_sys_name");
				blobmsg_add_string(&buf, NULL, "remote_sys_description");
			blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "lldp_config";
	const char *method = "lldp_config_get";

	ubus_call(path, method, &buf, &recive_lldp_get_neighbor);
}
