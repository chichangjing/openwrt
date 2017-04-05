#include "ubus_call.h"
#include "device_info.h"
#include "system_info.h"

static void recive_sysinfo_get_all(struct ubus_request *req, int type, struct blob_attr *msg){
	char * sysinfo[] = {
			"Device Up Time:",
			"Device Type:",
			"Device Serial Number:",
			"Hardware Version:",
			"Software Version:",
			"Device Name:",
			"Device Location:",
			"Device Description:",
	};
	int rem[6];
	int i=0;
	int up_time;
	HIGHT_LIGHT();
	_PRINT_INFO("%120s\n", "Device Info:")
	UN_HIGHT_LIGHT();
	print_row(120);
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				if(0 == i){
					_PRINT_INFO("%40s%6s", sysinfo[i], "")
					up_time = atoi(blobmsg_get_string(lvl[3]))/100;
					_PRINT_INFO("%d Day %d Hour %d Minutes %d Second\n", up_time/3600/24, (up_time%(3600*24))/3600, (up_time%3600)/60, up_time%60)
				}else if(i>0 && i<(sizeof(sysinfo)/sizeof(sysinfo[0])))
					_PRINT_INFO("%40s%6s%-s\n", sysinfo[i], "", blobmsg_get_string(lvl[3]))
				else
					_PRINT_INFO(" %% Switch config error!\n")
				i++;
			}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(120);
	fflush(stdout);
}

static void recive_sysinfo_set_name(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% System name set successfully.\n")
        else
            _PRINT_INFO(" %% System name set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

static void recive_sysinfo_set_location(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% System location set successfully.\n")
        else
            _PRINT_INFO(" %% System location set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

static void recive_sysinfo_save_config(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Save configuration successfully.\n")
        else
            _PRINT_INFO(" %% Save configuration failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

void sysinfo_get_all(void){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "sys_uptime");
		blobmsg_add_string(&buf, NULL, "model_name");
		blobmsg_add_string(&buf, NULL, "unique_id");
		blobmsg_add_string(&buf, NULL, "hardware_version");
		blobmsg_add_string(&buf, NULL, "software_version");
		blobmsg_add_string(&buf, NULL, "sys_name");
		blobmsg_add_string(&buf, NULL, "sys_location");
		blobmsg_add_string(&buf, NULL, "description");
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "sysinfo";
	const char *method = "sysinfo_get";

	ubus_call(path, method, &buf, &recive_sysinfo_get_all);
}

void sysinfo_set_name(const char *description){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "sys_name", description);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "sysinfo";
	const char *method = "sysinfo_set";

	ubus_call(path, method, &buf, &recive_sysinfo_set_name);
}

void sysinfo_set_location(const char *sys_location){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
		blobmsg_add_string(&buf, "sys_location", sys_location);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "sysinfo";
	const char *method = "sysinfo_set";

	ubus_call(path, method, &buf, &recive_sysinfo_set_location);
}

void save_config(void){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
    tb[1] = blobmsg_open_table(&buf, NULL);
        blobmsg_add_string(&buf, "save_config", "2");
    blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "sysinfo";
    const char *method = "sysinfo_set";

    ubus_call(path, method, &buf, &recive_sysinfo_save_config);
}
