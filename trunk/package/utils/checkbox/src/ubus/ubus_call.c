#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ubus_call.h"
struct blob_buf buf;
static const int timeout = 30;
const char * const blobmsg_type_info[__BLOBMSG_TYPE_LAST] = {
	[BLOBMSG_TYPE_UNSPEC]= "BLOBMSG_TYPE_UNSPEC",
	[BLOBMSG_TYPE_ARRAY] 	= "BLOBMSG_TYPE_ARRAY",
	[BLOBMSG_TYPE_TABLE]	= "BLOBMSG_TYPE_TABLE",
	[BLOBMSG_TYPE_STRING]	= "BLOBMSG_TYPE_STRING",
	[BLOBMSG_TYPE_INT64]	= "BLOBMSG_TYPE_INT64",
	[BLOBMSG_TYPE_INT32]	= "BLOBMSG_TYPE_INT32",
	[BLOBMSG_TYPE_INT16]	= "BLOBMSG_TYPE_INT16",
	[BLOBMSG_TYPE_INT8]	= "BLOBMSG_TYPE_INT8",
};

void print_row(int n){
	for(CLEAR_ROW(); n>0;n--)
	    printf("-");
	printf("\n");
}

bool is_num_d(const char * str){
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if('0'>str[i] || '9'<str[i])
			return false;
	}
	return true;
}
int _atoi_s(const char * str){
    int len, _num;
    if(!str || !(len=strlen(str)))
        return -1;
    for(_num=0;*str;str++){
        if(*str>='0' && *str<='9' && 0xffffffff > (_num = 10*_num + (*str-'0')))
            continue;
        return -1;
    }
    return _num;
}

void receive_result_data(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(0 == strcmp(blobmsg_get_string(lvl[1]), "0")){
            return;
            //_PRINT_INFO(" %% Set successfully.\n")
        }else{
            _PRINT_INFO(" %% Set failed:%s!\n", blobmsg_get_string(lvl[1]))
        }
    }else{_PRINT_ERROR("Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

void receive_set_result_data(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(0 == strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Set successfully.\n")
        else
            _PRINT_INFO(" %% Set failed:%s!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR("Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

void receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg){
	if (msg)
	    printf("%s\n", blobmsg_format_json_indent(msg, true, -1));
	return;
}

int ubus_call(const char *path, const char *method, struct blob_buf * buf, recv_data_cb *cb){
	int ret = 1;
	uint32_t id;

	const char *ubus_socket = NULL;
	struct ubus_context *ctx;

	if (!(ctx = ubus_connect(ubus_socket)))
		return -1;
	if ((ret = ubus_lookup_id(ctx, path, &id)))
		return -1;
	if ((ret = ubus_invoke(ctx, id, method, buf->head, cb, NULL, timeout * 1000)))
		return -1;
	ubus_free(ctx);
	return ret;
}

