#ifndef _UBUS_H
#define _UBUS_H
#include <libubox/blobmsg_json.h>
#include "libubus.h"
extern struct blob_buf buf;
extern const char * blobmsg_type_info[__BLOBMSG_TYPE_LAST];
typedef void recv_data_cb(struct ubus_request *req, int type, struct blob_attr *msg);
bool is_num_d(const char * str);
void receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg);
int ubus_call(const char *path, const char *method, struct blob_buf * buf, recv_data_cb *cb);
#endif
