#include "ubus_call.h"
#include "device_info.h"
#include "netconfig.h"

static void recive_netconfig_get_all(struct ubus_request *req, int type, struct blob_attr *msg){
	char * net_info[] = {
			" Ip address",
			" Netmask",
			" Gateway",
			" Ip Mode",
	};
	int rem[6];
	int i=0;
	int mode;
	HIGHT_LIGHT();
	_PRINT_INFO("%60s", "Device IP:\n")
	UN_HIGHT_LIGHT();
	print_row(60);
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				if(3 == i){
					mode = atoi(blobmsg_get_string(lvl[3]));
					if(NULL != ip_mode[1][mode])
						_PRINT_INFO("%-30s%-4s%-26s\n", net_info[i], ":", ip_mode[1][mode])
				}else if(i>=0 && i<(sizeof(net_info)/sizeof(net_info[0]))){
					_PRINT_INFO("%-30s%-4s%-26s\n", net_info[i], ":", blobmsg_get_string(lvl[3]))
				}else
					_PRINT_INFO(" %% Switch config error!\n")
				i++;
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]\n!", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(60);
	fflush(stdout);
}

static void recive_netconfig_get_ip_mode(struct ubus_request *req, int type, struct blob_attr *msg){
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

static void recive_netconfig_set_static_ip(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Static ip set successfully.\n")
        else
            _PRINT_INFO(" %% Static ip set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

static void recive_netconfig_set_ip_mode(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Set ip mode successfully.\n")
        else
            _PRINT_INFO(" %% Set ip mode failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

void netconfig_get_all(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "ipaddr");
		blobmsg_add_string(&buf, NULL, "netmask");
		blobmsg_add_string(&buf, NULL, "gateway");
		blobmsg_add_string(&buf, NULL, "protocol");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "network2";
	const char *method = "network2_get";

	ubus_call(path, method, &buf, &recive_netconfig_get_all);
}

void netconfig_get_ip_mode(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "protocol");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "network2";
	const char *method = "network2_get";

	ubus_call(path, method, &buf, &recive_netconfig_get_ip_mode);
}

void netconfig_set_ip_mode(const char * ip_mode){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
    tb[1] = blobmsg_open_table(&buf, NULL);
        blobmsg_add_string(&buf, "protocol", ip_mode);
    blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "network2";
    const char *method = "network2_set";

    ubus_call(path, method, &buf, &recive_netconfig_set_ip_mode);
}

void netconfig_set_static_ip(const char *ip_address, const char *netmask, const char *gateway){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "ipaddr", ip_address);
		blobmsg_add_string(&buf, "netmask", netmask);
		blobmsg_add_string(&buf, "gateway", gateway);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "network2";
	const char *method = "network2_set";

	ubus_call(path, method, &buf, &recive_netconfig_set_static_ip);
}
