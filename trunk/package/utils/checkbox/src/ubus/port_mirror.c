#include "ubus_call.h"
#include "device_info.h"
#include "port_mirror.h"

static void recive_port_mirror_destination_port(struct ubus_request *req, int type, struct blob_attr *msg){
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

static void recive_port_mirror_mode(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	HIGHT_LIGHT();
	_PRINT_INFO("%30s%30s\n", "Interface:", "Mode:")
	UN_HIGHT_LIGHT();
	print_row(60);
	_PRINT_INFO("%30s%30s\n", "Destnation:", port_name[1][device_gt->current_mode])
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
	blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
	if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
	blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
	if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
	_PRINT_INFO("%30s", port_ubus_to_show(blobmsg_name(lvl[3])))
	blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
	if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
	blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
	if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
	_PRINT_INFO("%30s\n", port_mirror_mode_to_show(blobmsg_get_string(lvl[5])))
	}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(60);
	fflush(stdout);
}

static void recive_set_port_mirror_destination(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Mirror destination set successfully.\n")
		else
			_PRINT_INFO(" %% Mirror destination set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_set_port_mirror_mode(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO("\n %% Mirror mode set successfully.\n")
		else
			_PRINT_INFO("\n %% Mirror mode set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR("\n %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void port_mirror_get_destination_port(void){
	void *tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "get_args");
	blobmsg_add_string(&buf, NULL, "destination_port");
	blobmsg_close_array(&buf, tb);

	const char *path = "port_mirror";
	const char *method = "port_mirror_get";

	ubus_call(path, method, &buf, &recive_port_mirror_destination_port);
}

void port_mirror_get_mode_all(void){
	int i;
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=PORT_1; i<_PORT_MAX; i++){
			if(i == device_gt->current_mode)
				continue;
			tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, port_name[2][i]);
			blobmsg_add_string(&buf, NULL, "mode");
			blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_mirror";
	const char *method = "port_mirror_get";

	ubus_call(path, method, &buf, &recive_port_mirror_mode);
}

void port_mirror_get_mode_one(const char *port){
	void *tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
	tb[2] = blobmsg_open_array(&buf, port);
	blobmsg_add_string(&buf, NULL, "mode");
	blobmsg_close_array(&buf, tb[2]);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_mirror";
	const char *method = "port_mirror_get";

	ubus_call(path, method, &buf, &recive_port_mirror_mode);
}

void port_mirror_set_destination(const char *port){
	void *tb[2];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
	blobmsg_add_string(&buf, "destination_port", port);
	blobmsg_close_array(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_mirror";
	const char *method = "port_mirror_set";

	ubus_call(path, method, &buf, &recive_set_port_mirror_destination);
}

void port_mirror_set_mode_all(const char *mode){
	int i;
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		for(i=PORT_1; i<_PORT_MAX; i++){
			if(i == device_gt->current_mode)
				continue;
			tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, port_name[2][i]);
			tb[3] = blobmsg_open_table(&buf, NULL);
			blobmsg_add_string(&buf, "mode", mode);
			blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_mirror";
	const char *method = "port_mirror_set";

	ubus_call(path, method, &buf, &recive_set_port_mirror_mode);
}

void port_mirror_set_mode_one(const char *port, const char *mode){
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
	tb[2] = blobmsg_open_array(&buf, port);
	tb[3] = blobmsg_open_table(&buf, NULL);
	blobmsg_add_string(&buf, "mode", mode);
	blobmsg_close_table(&buf, tb[3]);
	blobmsg_close_array(&buf, tb[2]);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_mirror";
	const char *method = "port_mirror_set";

	ubus_call(path, method, &buf, &recive_set_port_mirror_mode);
}
