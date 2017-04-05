#include "ubus_call.h"
#include "device_info.h"
#include "snmp.h"

static void recive_snmp_get_status(struct ubus_request *req, int type, 	struct blob_attr *msg){
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

static void recive_snmp_trap_get_status(struct ubus_request *req, int type, struct blob_attr *msg){
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
static void recive_snmp_get_info(struct ubus_request *req, int type, struct blob_attr *msg){
	int i = 0;
	int rem[6];
    HIGHT_LIGHT();
    printf("%64s\n", "SNMP Agent Info:");
    UN_HIGHT_LIGHT();
    print_row(64);
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				switch(i){
                    case 0:
                        if(0==strcmp(blobmsg_get_string(lvl[3]), "1"))
                            printf(" %-30s%-4s%-30s\n", "Snmp-agent status",":", "enable");
                        else if(0==strcmp(blobmsg_get_string(lvl[3]), "2"))
                            printf(" %-30s%-4s%-30s\n", "Snmp-agent status",   ":", "disable");
                        else
                            printf(" %-30s%-4s%-30s\n", "Snmp-agent status",   ":", "unknow");
                        break;
                    case 1:
                        printf(" %-30s%-4s%-30s\n", "Read-only Community Name", ":", blobmsg_get_string(lvl[3]));
                        break;
                    case 2:
                        printf(" %-30s%-4s%-30s\n", "Read-write Community Name", ":", blobmsg_get_string(lvl[3]));
                        break;
                    case 3:
                        printf(" %-30s%-4s%-30s\n", "Snmp-agent port", ":", blobmsg_get_string(lvl[3]));
                        break;
                    default:
                        printf(" %% Switch error: [%s]!\n", blobmsg_get_string(lvl[3]));
                        break;
				}
				i++;
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(64);
	fflush(stdout);
}
static void recive_snmp_trap_get_info(struct ubus_request *req, int type, struct blob_attr *msg){
	int i = 0;
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				switch(i){
                    case 0:
                        if(0==strcmp(blobmsg_get_string(lvl[3]), "1"))
                            printf(" %-30s%-4s%-30s\n", "Trap status",":", "enable");
                        else if(0==strcmp(blobmsg_get_string(lvl[3]), "2"))
                            printf(" %-30s%-4s%-30s\n", "Trap status",   ":", "disable");
                        else
                            printf(" %-30s%-4s%-30s\n", "Trap status",   ":", "unknow");
                        break;
                    case 1:
                        printf(" %-30s%-4s%-30s\n", "Trap port", ":", blobmsg_get_string(lvl[3]));
                        break;
                    case 2:
                        printf(" %-30s%-4s%-30s\n", "Trap server IP", ":", blobmsg_get_string(lvl[3]));
                        break;
                    default:
                        printf(" %% Switch error: [%s]!\n", blobmsg_get_string(lvl[3]));
                        break;
				}
				i++;
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}
static void recive_trap_power_info(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
                if(0==strcmp(blobmsg_get_string(lvl[3]), "1"))
                    printf(" %-30s%-4s%-30s\n", "Trap power status",":", "enable");
                else if(0==strcmp(blobmsg_get_string(lvl[3]), "2"))
                    printf(" %-30s%-4s%-30s\n", "Trap power status",   ":", "disable");
                else
                    printf(" %-30s%-4s%-30s\n", "Trap power status",   ":", "unknow");
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}
static void recive_snmp_set_status(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% SNMP set status successfully.\n")
		else
			_PRINT_INFO(" %% SNMP set status failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}
static void recive_snmp_set_readonly_name(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% SNMP set read-only name successfully.\n")
		else
			_PRINT_INFO(" %% SNMP set read-only name failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}
static void recive_snmp_set_readwrite_name(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0")){
			_PRINT_INFO(" %% SNMP set read-write name successfully.\n")
		}else{
			_PRINT_INFO(" %% SNMP set read-write  name failed: [%s]!\n", blobmsg_get_string(lvl[1]))
		}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}
static void recive_snmp_set_port(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% SNMP set port successfully.\n")
		else
			_PRINT_INFO(" %% SNMP set port failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}
static void recive_snmp_trap_set_status(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% SNMP-trap set status successfully.\n")
		else
			_PRINT_INFO(" %% SNMP-trap set status failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_power_trap_set_status(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Trap power set status successfully.\n")
        else
            _PRINT_INFO(" %% Trap power set status failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

static void recive_snmp_trap_set_port(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% SNMP-trap set port successfully.\n")
		else
			_PRINT_INFO(" %% SNMP-trap set port failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}
static void recive_snmp_trap_set_server_ip(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% SNMP-trap set server ip successfully.\n")
		else
			_PRINT_INFO(" %% SNMP-trap set server ip failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}
void snmp_get_status(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "status");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_get";

	ubus_call(path, method, &buf, &recive_snmp_get_status);
}

void snmp_get_info(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
	    blobmsg_add_string(&buf, NULL, "status");
		blobmsg_add_string(&buf, NULL, "readonly_name");
		blobmsg_add_string(&buf, NULL, "readwrite_name");
		blobmsg_add_string(&buf, NULL, "port");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_get";

	ubus_call(path, method, &buf, &recive_snmp_get_info);
}
void snmp_set_status(const char *status){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "status", status);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_set";

	ubus_call(path, method, &buf, &recive_snmp_set_status);
}

void snmp_set_readonly_name(const char *readonly_name){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "readonly_name", readonly_name);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_set";

	ubus_call(path, method, &buf, &recive_snmp_set_readonly_name);
}

void snmp_set_readwrite_name(const char *readwrite_name){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "readwrite_name", readwrite_name);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_set";

	ubus_call(path, method, &buf, &recive_snmp_set_readwrite_name);
}

void snmp_set_port(const char *port){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "port", port);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_set";

	ubus_call(path, method, &buf, &recive_snmp_set_port);
}

void snmp_trap_get_status(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "trap_status");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_get";

	ubus_call(path, method, &buf, &recive_snmp_trap_get_status);
}

void snmp_trap_get_info(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
	    blobmsg_add_string(&buf, NULL, "trap_status");
		blobmsg_add_string(&buf, NULL, "trap_port");
		blobmsg_add_string(&buf, NULL, "trap_server_ip");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_get";

	ubus_call(path, method, &buf, &recive_snmp_trap_get_info);
}

void power_trap_get_status(void){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        blobmsg_add_string(&buf, NULL, "power_trap");
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "trap";
    const char *method = "trap_get";

    ubus_call(path, method, &buf, &recive_trap_power_info);
}

void snmp_trap_set_status(const char *status){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "trap_status", status);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_set";

	ubus_call(path, method, &buf, &recive_snmp_trap_set_status);
}

void snmp_trap_set_trap_port(const char *trap_port){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "trap_port", trap_port);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_set";

	ubus_call(path, method, &buf, &recive_snmp_trap_set_port);
}

void snmp_trap_set_trap_server_ip(const char *trap_server_ip){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "trap_server_ip", trap_server_ip);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "snmp";
	const char *method = "snmp_set";

	ubus_call(path, method, &buf, &recive_snmp_trap_set_server_ip);
}

void power_trap_set_status(const char *status){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
    tb[1] = blobmsg_open_table(&buf, NULL);
        blobmsg_add_string(&buf, "power_trap", status);
    blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "trap";
    const char *method = "trap_set";

    ubus_call(path, method, &buf, &recive_power_trap_set_status);
}
