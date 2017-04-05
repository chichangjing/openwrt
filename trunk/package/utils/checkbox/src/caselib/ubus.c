#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "log.h"
#include "ubus.h"
struct blob_buf buf;
static const int timeout = 30;
const char * blobmsg_type_info[__BLOBMSG_TYPE_LAST] = {
	[BLOBMSG_TYPE_UNSPEC] 	= "BLOBMSG_TYPE_UNSPEC",
	[BLOBMSG_TYPE_ARRAY] 	= "BLOBMSG_TYPE_ARRAY",
	[BLOBMSG_TYPE_TABLE]	= "BLOBMSG_TYPE_TABLE",
	[BLOBMSG_TYPE_STRING]	= "BLOBMSG_TYPE_STRING",
	[BLOBMSG_TYPE_INT64]	= "BLOBMSG_TYPE_INT64",
	[BLOBMSG_TYPE_INT32]	= "BLOBMSG_TYPE_INT32",
	[BLOBMSG_TYPE_INT16]	= "BLOBMSG_TYPE_INT16",
	[BLOBMSG_TYPE_INT8]		= "BLOBMSG_TYPE_INT8",
};
bool is_num_d(const char * str){
	int i;
	int len = strlen(str);
	for(i=0; i<len; i++){
		if(0>str[i]-0x30 || 9<str[i]-0x30){
			return 0;
		}
	}
	return 1;
}
void receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg){
	char *str;
	if (!msg)
		return;
	str = blobmsg_format_json_indent(msg, true, -1);
	_info("%s\n", str);
	free(str);
	return;
}
int ubus_call(const char *path, const char *method, struct blob_buf * buf, recv_data_cb *cb){
	int ret = 1;
	uint32_t id;
	const char *ubus_socket = NULL;
	struct ubus_context *ctx;
	ctx = ubus_connect(ubus_socket);

	if (!ctx){
		_error("ubus connect error!\n");
		return -1;
	}
	ret = ubus_lookup_id(ctx, path, &id);
	if (ret){
		_error("ubus look up id error: %s\n", path);
		return -1;
	}
	ret = ubus_invoke(ctx, id, method, buf->head, cb, NULL, timeout * 1000);
	if (ret){
		_error("ubus invoke error!\n");
		return -1;
	}
	ubus_free(ctx);
	return ret;
}
