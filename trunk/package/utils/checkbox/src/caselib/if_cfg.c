#include <stdio.h>
#include "ubus.h"
#include "dev_info.h"
#include "if_cfg.h"
#include "log.h"

static void
port_config_recive_set(struct ubus_request *req, int type, \
		struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
			if(0 == strcmp(blobmsg_get_string(lvl[1]), "0")){
				_info("set successfully! \n");
			}else{
				_info("set failed:%s! \n", blobmsg_get_string(lvl[1]));
			}
		}else{
			_error("ubus message type error:%s! \n", blobmsg_type_info[blobmsg_type(lvl[1])]);
		}
	}
}

static void
port_config_set_config_message(struct blob_buf *buf, const char *port, \
		const char *config, const char *mode){
	void *tb[3];
	tb[0] = blobmsg_open_array(buf, "set_args");
		tb[1] = blobmsg_open_table(buf, NULL);
			tb[2] = blobmsg_open_array(buf, port);
				tb[3] = blobmsg_open_table(buf, NULL);
					blobmsg_add_string(buf, config, mode);
				blobmsg_close_table(buf, tb[3]);
			blobmsg_close_array(buf, tb[2]);
		blobmsg_close_table(buf, tb[1]);
	blobmsg_close_array(buf, tb[0]);
}

static void
port_config_set_config(const char *config, const char *port, \
		const char *mode){
	blobmsg_buf_init(&buf);

	port_config_set_config_message(&buf, port, config, mode);
	const char *path = "port_config";
	const char *method = "port_config_set";

	ubus_call(path, method, &buf, &port_config_recive_set);
}

void port_config_set_admin_mode(const char *port, const char *mode){
	const char *config = "admin_mode";
	_info("set %8s admin mode : %4s ! \n", port, mode);
	port_config_set_config(config, port, mode);
}

void all_port_down(void){
	int i;
	for(i=0; i<_DEV_PORT_MAX; i++){
		port_config_set_admin_mode(dev_port_name[i], dev_port_admin_mode[DEV_PORT_CONFIG_ADMIN_MODE_DISABLED]);
	}
}
