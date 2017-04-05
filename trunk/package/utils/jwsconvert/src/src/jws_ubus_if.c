/*
 * Copyright (C) 2011 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <sys/time.h>
#include <unistd.h>

#include <libubox/ustream.h>
#include <libubox/blobmsg_json.h>
#include <pthread.h>
#include <semaphore.h>

#include "jws_ubus_if.h"
#include "libubus.h"
#include "jwslog.h"
#include "string.h"

static struct ubus_context *ctx = NULL;
static struct blob_buf b;
static sem_t  bin_sem;

static uint32_t buff_u32;
static char buff_str[256];

enum {
	GET_INFO_RET_ARRAY,
	__GET_INFO_RET_MAX,
};

static const struct blobmsg_policy get_ret_policy[] = {
    [GET_INFO_RET_ARRAY] = {.name = "ret", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
	SET_INFO_RET_STRING,
	__SET_INFO_RET_MAX,
};

static const struct blobmsg_policy set_ret_policy[] = {
    [SET_INFO_RET_STRING] = {.name = "ret", .type = BLOBMSG_TYPE_STRING},
};

/**
 * [receive_call_result_data description]
 * @param req  [description]
 * @param type [description]
 * @param msg  [description]
 */
void receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg)
{
	char *str;
	if (!msg)
		return;

	str = blobmsg_format_json_indent(msg, true, 0);

	LogWrite(DEBUG, "%s\n", str);
	free(str);
}

char *jws_write_buf_str(const char *buf, int size)
{
	int num = 0;

	num = sizeof(buff_str) - 1;
	if (num >= size)
	{
		num = size;
	}
	strncpy(buff_str, buf, num);
	buff_str[num] = '\0';

	return buff_str;
}

char *jws_read_buf_str(void)
{
	return buff_str;
}

uint32_t jws_write_buf_u32(uint32_t buf)
{
	buff_u32 = buf;

	return 0;
}

uint32_t jws_read_buf_u32(void)
{
	return buff_u32;
}

/**
 * [sysinfo_ret_handler_cb description]
 * @param req  [description]
 * @param type [description]
 * @param msg  [description]
 */
void sysinfo_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int  rem, _rem;
	const char *key = NULL, *value = NULL;
	struct blob_attr *tb[__GET_INFO_RET_MAX], *cur, *_cur;

	if (!msg)
		return;

	blobmsg_parse(get_ret_policy, ARRAY_SIZE(get_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[GET_INFO_RET_ARRAY]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

    blobmsg_for_each_attr(cur, tb[GET_INFO_RET_ARRAY], rem) {
    	blobmsg_for_each_attr(_cur, cur, _rem){
    		if (blobmsg_type(_cur) == BLOBMSG_TYPE_STRING) {
    		key = blobmsg_name(_cur);
            value = blobmsg_get_string(_cur);
            LogWrite(DEBUG, "%s:%s\n", key, value);
            jws_write_buf_str(value, strlen(value));

	        } else {
	        	LogWrite(ERROR, "file:%s line:%d func:%s, invalid attr type\n",
						__FILENAME__, __LINE__, __FUNCTION__);
	        }
    	}
    }

	return;
}

/**
 * [port_config_get_ret_handler_cb description]
 * @param req  [description]
 * @param type [description]
 * @param msg  [description]
 */
void port_config_get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int  rem, _rem, __rem, ___rem;
	const char *key = NULL;
	char *value = NULL;
	struct blob_attr *tb[__GET_INFO_RET_MAX], *cur, *_cur, *__cur, *___cur;

	if (!msg)
		return;

	blobmsg_parse(get_ret_policy, ARRAY_SIZE(get_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[GET_INFO_RET_ARRAY]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

    blobmsg_for_each_attr(cur, tb[GET_INFO_RET_ARRAY], rem) {
    	blobmsg_for_each_attr(_cur, cur, _rem){
    		blobmsg_for_each_attr(__cur, _cur, __rem){
    			blobmsg_for_each_attr(___cur, __cur, ___rem){
		    		if (blobmsg_type(___cur) == BLOBMSG_TYPE_STRING) {
		    		key = blobmsg_name(___cur);
		            value = blobmsg_get_string(___cur);
		            LogWrite(DEBUG, "%s:%s\n", key, value);
					jws_write_buf_str(value, strlen(value));
			        } else {
			        	LogWrite(ERROR, "file:%s line:%d func:%s, invalid attr type\n",
								__FILENAME__, __LINE__, __FUNCTION__);
			        }
			    }
		    }
    	}
    }

	return;
}

void port_mirror_get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int  rem, _rem;
	const char *key = NULL, *value = NULL;
	struct blob_attr *tb[__GET_INFO_RET_MAX], *cur, *_cur;

	if (!msg)
		return;

	blobmsg_parse(get_ret_policy, ARRAY_SIZE(get_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[GET_INFO_RET_ARRAY]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

    blobmsg_for_each_attr(cur, tb[GET_INFO_RET_ARRAY], rem) {
    	blobmsg_for_each_attr(_cur, cur, _rem){
    		if (blobmsg_type(_cur) == BLOBMSG_TYPE_STRING) {
    		key = blobmsg_name(_cur);
            value = blobmsg_get_string(_cur);
            LogWrite(DEBUG, "%s:%s\n", key, value);
            jws_write_buf_str(value, strlen(value));

	        } else {
	        	LogWrite(ERROR, "file:%s line:%d func:%s, invalid attr type\n",
						__FILENAME__, __LINE__, __FUNCTION__);
	        }
    	}
    }

	return;
}


void port_mirror_table_get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int  rem, _rem, __rem, ___rem;
	const char *key = NULL;
	char *value = NULL;
	struct blob_attr *tb[__GET_INFO_RET_MAX], *cur, *_cur, *__cur, *___cur;

	if (!msg)
		return;

	blobmsg_parse(get_ret_policy, ARRAY_SIZE(get_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[GET_INFO_RET_ARRAY]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

    blobmsg_for_each_attr(cur, tb[GET_INFO_RET_ARRAY], rem) {
    	blobmsg_for_each_attr(_cur, cur, _rem){
    		blobmsg_for_each_attr(__cur, _cur, __rem){
    			blobmsg_for_each_attr(___cur, __cur, ___rem){
		    		if (blobmsg_type(___cur) == BLOBMSG_TYPE_STRING) {
		    		key = blobmsg_name(___cur);
		            value = blobmsg_get_string(___cur);
		            LogWrite(DEBUG, "%s:%s\n", key, value);
					jws_write_buf_str(value, strlen(value));
			        } else {
			        	LogWrite(ERROR, "file:%s line:%d func:%s, invalid attr type\n",
								__FILENAME__, __LINE__, __FUNCTION__);
			        }
			    }
		    }
    	}
    }

	return;
}

void set_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	const char *key = NULL;
	char *value = NULL;
	struct blob_attr *tb[__SET_INFO_RET_MAX];

	if (!msg)
		return;

	blobmsg_parse(set_ret_policy, ARRAY_SIZE(set_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[SET_INFO_RET_STRING]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

	key = blobmsg_name(tb[SET_INFO_RET_STRING]);
	value = blobmsg_get_string(tb[SET_INFO_RET_STRING]);
	LogWrite(DEBUG, "%s : %s\n", key, value);
	if (strcmp(value, "0") != 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to return value\n",
				__FILENAME__, __LINE__, __FUNCTION__);
	}

	return;
}

void clear_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	const char *key = NULL;
	char *value = NULL;
	struct blob_attr *tb[__SET_INFO_RET_MAX];

	if (!msg)
		return;

	blobmsg_parse(set_ret_policy, ARRAY_SIZE(set_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[SET_INFO_RET_STRING]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

	key = blobmsg_name(tb[SET_INFO_RET_STRING]);
	value = blobmsg_get_string(tb[SET_INFO_RET_STRING]);
	LogWrite(DEBUG, "%s : %s\n", key, value);
	if (strcmp(value, "0") != 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to return value\n",
				__FILENAME__, __LINE__, __FUNCTION__);
	}

	return;
}

void get_table_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int  rem, _rem, __rem, ___rem;
	const char *key = NULL;
	char *value = NULL;
	struct blob_attr *tb[__GET_INFO_RET_MAX], *cur, *_cur, *__cur, *___cur;

	if (!msg)
		return;

	blobmsg_parse(get_ret_policy, ARRAY_SIZE(get_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[GET_INFO_RET_ARRAY]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

    blobmsg_for_each_attr(cur, tb[GET_INFO_RET_ARRAY], rem) {
    	blobmsg_for_each_attr(_cur, cur, _rem){
    		blobmsg_for_each_attr(__cur, _cur, __rem){
    			blobmsg_for_each_attr(___cur, __cur, ___rem){
		    		if (blobmsg_type(___cur) == BLOBMSG_TYPE_STRING) {
		    		key = blobmsg_name(___cur);
		            value = blobmsg_get_string(___cur);
		            LogWrite(DEBUG, "%s:%s\n", key, value);
					jws_write_buf_str(value, strlen(value));
			        } else {
			        	LogWrite(ERROR, "file:%s line:%d func:%s, invalid attr type\n",
								__FILENAME__, __LINE__, __FUNCTION__);
			        }
			    }
		    }
    	}
    }

	return;
}

void get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int  rem, _rem;
	const char *key = NULL, *value = NULL;
	struct blob_attr *tb[__GET_INFO_RET_MAX], *cur, *_cur;

	if (!msg)
		return;

	blobmsg_parse(get_ret_policy, ARRAY_SIZE(get_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[GET_INFO_RET_ARRAY]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

    blobmsg_for_each_attr(cur, tb[GET_INFO_RET_ARRAY], rem) {
    	blobmsg_for_each_attr(_cur, cur, _rem){
    		if (blobmsg_type(_cur) == BLOBMSG_TYPE_STRING) {
    		key = blobmsg_name(_cur);
            value = blobmsg_get_string(_cur);
            LogWrite(DEBUG, "%s:%s\n", key, value);
            jws_write_buf_str(value, strlen(value));

	        } else {
	        	LogWrite(ERROR, "file:%s line:%d func:%s, invalid attr type\n",
						__FILENAME__, __LINE__, __FUNCTION__);
	        }
    	}
    }

	return;
}

int ubus_get_cmd_enc(char *str, const int size, const int index, const char *key)
{
	char str1[] = "{\"get_args\":[{\"idx-";
	char str2[] = "\":[\"";
	char str3[] = "\"]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(idx_str) + strlen(key);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	
	return 0;
}

int ubus_get_port_cmd_enc(char *str, const int size, const int index, const char *key)
{
	char str1[] = "{\"get_args\":[{\"port-";
	char str2[] = "\":[\"";
	char str3[] = "\"]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(idx_str) + strlen(key);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	
	return 0;
}


struct mv88exxxx_hw_stat{
	char string[32];
	char value[32];
};

enum {
	MIB_RX_BYTES_STAT,
	MIB_RX_UNICAST_PACKETS_STAT,
	MIB_RX_BROADCAST_PACKETS_STAT,
	MIB_RX_MULTICAST_PACKETS_STAT,
	MIB_RX_PAUSE_PACKETS_STAT,
	MIB_TX_BYTES_STAT,
	MIB_TX_UNICAST_PACKETS_STAT,
	MIB_TX_BROADCAST_PACKETS_STAT,
	MIB_TX_MULTICAST_PACKETS_STAT,
	MIB_TX_PAUSE_PACKETS_STAT,
	MIB_INFO_RET_MAX,
};

static struct mv88exxxx_hw_stat mv88e6097_hw_stat[]={
	[MIB_RX_BYTES_STAT] = {.string = "rx_bytes", .value = "\0"},
	[MIB_RX_UNICAST_PACKETS_STAT] = {.string = "rx_unicast_packets", .value = "\0"},
	[MIB_RX_BROADCAST_PACKETS_STAT] = {.string = "rx_broadcast_packets", .value = "\0"},
	[MIB_RX_MULTICAST_PACKETS_STAT] = {.string = "rx_multicast_packets", .value = "\0"},
	[MIB_RX_PAUSE_PACKETS_STAT] = {.string = "rx_pause_packets", .value = "\0"},
	[MIB_TX_BYTES_STAT] = {.string = "tx_bytes", .value = "\0"},
	[MIB_TX_UNICAST_PACKETS_STAT] = {.string = "tx_unicast_packets", .value = "\0"},
	[MIB_TX_BROADCAST_PACKETS_STAT] = {.string = "tx_broadcast_packets", .value = "\0"},
	[MIB_TX_MULTICAST_PACKETS_STAT] = {.string = "tx_multicast_packets", .value = "\0"},
	[MIB_TX_PAUSE_PACKETS_STAT] = {.string = "tx_pause_packets", .value = "\0"},
};

static int ubus_get_port_entry_cmd_enc(char *str,
					const int size,
					const int index)
{
	char str1[] = "{\"get_args\":[{\"port-";
	char str2[] = "\":[";
	char str3[] = "]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;
	int  key_num = 0;
	int  i;

	memset(str, 0, size*sizeof(char));

	if (str == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	key_num = sizeof(mv88e6097_hw_stat)/sizeof(struct mv88exxxx_hw_stat);
	for (i = 0; i < key_num; ++i)
	{
		str_len += strlen(mv88e6097_hw_stat[i].string);
		if (i+1 == key_num) {
			str_len += 2;
		}else{
			str_len += 3;
		}
	}
	str_len += strlen(str1) + strlen(str2) + strlen(str3) + strlen(idx_str);

	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	for (i = 0; i < key_num; ++i)
	{
		strcat(str, "\"");
		strcat(str, mv88e6097_hw_stat[i].string);
		if (i+1 == key_num) {
			strcat(str, "\"");
		}else{
			strcat(str, "\"");
			strcat(str, ",");
		}
	}
	strcat(str, str3);
	
	return 0;
}

static void get_port_entry_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg)
{
	int  rem, _rem, __rem, ___rem;
	const char *key = NULL;
	char *value = NULL;
	struct blob_attr *tb[__GET_INFO_RET_MAX], *cur, *_cur, *__cur, *___cur;
	int  key_num = 0;
	int i;

	if (!msg)
		return;

	key_num = sizeof(mv88e6097_hw_stat)/sizeof(struct mv88exxxx_hw_stat);

	blobmsg_parse(get_ret_policy, ARRAY_SIZE(get_ret_policy), tb, blob_data(msg), blob_len(msg));
	if (!tb[GET_INFO_RET_ARRAY]) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse attr type\n",
				__FILENAME__, __LINE__, __FUNCTION__);
        return;
    }

    blobmsg_for_each_attr(cur, tb[GET_INFO_RET_ARRAY], rem) {
    	blobmsg_for_each_attr(_cur, cur, _rem){
    		blobmsg_for_each_attr(__cur, _cur, __rem){
    			blobmsg_for_each_attr(___cur, __cur, ___rem){
		    		if (blobmsg_type(___cur) == BLOBMSG_TYPE_STRING) {
		    			key = blobmsg_name(___cur);
		            	value = blobmsg_get_string(___cur);
		            	#if 0
		            	LogWrite(DEBUG, "%s:%s\n", key, value);
		            	#endif
		            	for (i = 0; i < key_num; ++i) {
		            		if(0 == strcmp(key, mv88e6097_hw_stat[i].string)){
		            			strcpy(mv88e6097_hw_stat[i].value, value);
		            			break;
		            		}
		            	}
			        } else {
			        	LogWrite(ERROR, "file:%s line:%d func:%s, invalid attr type\n",
								__FILENAME__, __LINE__, __FUNCTION__);
			        }
			    }
		    }
    	}
    }

	return;
}

int get_port_entry_statistics(const int index, fal_mib_info_t *mib_Info)
{
	int key_id;
	char str[256];
	int key_num;
	int ret;
	unsigned int data32 = 0;
	unsigned long long data64 = 0;

	ubus_get_port_entry_cmd_enc(str, sizeof(str), index);
	ret = ubus_invoke_command("eth_stats", "eth_stats_get", 
                        get_port_entry_ret_handler_cb, str);
	if (ret != 0){
        return -1;
    }

	key_num = sizeof(mv88e6097_hw_stat)/sizeof(struct mv88exxxx_hw_stat);

	for (key_id = 0; key_id < key_num; ++key_id) {
		switch(key_id){
			case MIB_RX_BYTES_STAT:
				data64 = atoll(mv88e6097_hw_stat[key_id].value);
				mib_Info->RxGoodByteLo = (unsigned int)data64;
				mib_Info->RxGoodByteHi = (unsigned int)(data64 >> 32);
				break;
			case MIB_RX_UNICAST_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->RxUnicastPkts = data32;
				break;
			case MIB_RX_BROADCAST_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->RxBroadcastPkts = data32;
				break;
			case MIB_RX_MULTICAST_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->RxMulticastPkts = data32;
				break;
			case MIB_RX_PAUSE_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->RxPausePkts = data32;
				break;
			case MIB_TX_BYTES_STAT:
				data64 = atoll(mv88e6097_hw_stat[key_id].value);
				mib_Info->TxByteLo = (unsigned int)data64;
				mib_Info->TxByteHi = (unsigned int)(data64 >> 32);
				break;
			case MIB_TX_UNICAST_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->TxUnicastPkts = data32;
				break;
			case MIB_TX_BROADCAST_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->TxBroadcastPkts = data32;
				break;
			case MIB_TX_MULTICAST_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->TxMulticastPkts = data32;
				break;
			case MIB_TX_PAUSE_PACKETS_STAT:
				data32 = atoi(mv88e6097_hw_stat[key_id].value);
				mib_Info->TxPausePkts = data32;
				break;
			default:
				LogWrite(ERROR, "file:%s line:%d func:%s, port mib info invalid\n",
					__FILENAME__, __LINE__, __FUNCTION__);
				break;
		}

	}

	return 0;
}

int ubus_get_qos_port_cmd_enc(char *str, const int size, const int index, const char *key)
{
	char str1[] = "{\"get_args\":[{\"port-";
	char str2[] = "\":[\"";
	char str3[] = "\"]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(idx_str) + strlen(key);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	
	return 0;
}

int ubus_get_qos_8021p_cmd_enc(char *str, const int size, const int index, const char *key)
{
	char str1[] = "{\"get_args\":[{\"8021p-";
	char str2[] = "\":[\"";
	char str3[] = "\"]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(idx_str) + strlen(key);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	
	return 0;
}

int ubus_get_qos_dscp_cmd_enc(char *str, const int size, const int index, const char *key)
{
	char str1[] = "{\"get_args\":[{\"dscp-";
	char str2[] = "\":[\"";
	char str3[] = "\"]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(idx_str) + strlen(key);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	
	return 0;
}

int ubus_set_cmd_enc(char *str, const int size, const int index, const char *key, const char *value)
{
	char str1[] = "{\"set_args\":[{\"idx-";
	char str2[] = "\":[{\"";
	char str3[] = "\":\"";
	char str4[] = "\"}]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL || value == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(str4) +
				strlen(idx_str) + strlen(key) + strlen(value);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	strcat(str, value);
	strcat(str, str4);
	
	return 0;
}

int ubus_set_port_cmd_enc(char *str, const int size, const int index, const char *key, const char *value)
{
	char str1[] = "{\"set_args\":[{\"port-";
	char str2[] = "\":[{\"";
	char str3[] = "\":\"";
	char str4[] = "\"}]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL || value == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(str4) +
				strlen(idx_str) + strlen(key) + strlen(value);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	strcat(str, value);
	strcat(str, str4);
	
	return 0;
}

int ubus_set_qos_port_cmd_enc(char *str, const int size, const int index, const char *key, const char *value)
{
	char str1[] = "{\"set_args\":[{\"port-";
	char str2[] = "\":[{\"";
	char str3[] = "\":\"";
	char str4[] = "\"}]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL || value == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(str4) +
				strlen(idx_str) + strlen(key) + strlen(value);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	strcat(str, value);
	strcat(str, str4);
	
	return 0;
}

int ubus_set_qos_8021p_cmd_enc(char *str, const int size, const int index, const char *key, const char *value)
{
	char str1[] = "{\"set_args\":[{\"8021p-";
	char str2[] = "\":[{\"";
	char str3[] = "\":\"";
	char str4[] = "\"}]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL || value == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(str4) +
				strlen(idx_str) + strlen(key) + strlen(value);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	strcat(str, value);
	strcat(str, str4);
	
	return 0;
}

int ubus_set_qos_dscp_cmd_enc(char *str, const int size, const int index, const char *key, const char *value)
{
	char str1[] = "{\"set_args\":[{\"dscp-";
	char str2[] = "\":[{\"";
	char str3[] = "\":\"";
	char str4[] = "\"}]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL || key == NULL || value == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(str3) + strlen(str4) +
				strlen(idx_str) + strlen(key) + strlen(value);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	strcat(str, key);
	strcat(str, str3);
	strcat(str, value);
	strcat(str, str4);
	
	return 0;
}

int ubus_add_cmd_enc(char *str, const int size, const int index)
{
	char str1[] = "{\"set_args\":[{\"idx-";
	char str2[] = "\":[]}]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(idx_str);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	
	return 0;
}

int ubus_clear_cmd_enc(char *str, const int size, const int index)
{
	char str1[] = "{\"clear_args\":[\"idx-";
	char str2[] = "\"]}";
	char idx_str[10] = {0};
	int  str_len = 0;

	str_len = size;

	if (str == NULL)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Pointor don't NULL\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (index < 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to index\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	snprintf(idx_str, sizeof(idx_str), "%d", index);

	str_len = strlen(str1) + strlen(str2) + strlen(idx_str);
	if (str_len > size)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, str size don't enough\n",
				__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	strcat(str, str1);
	strcat(str, idx_str);
	strcat(str, str2);
	
	return 0;
}

/**
 * Invoke ubusd special method
 * @param path   object path
 * @param method method name	
 * @param cb     reply data callback function
 */
int ubus_invoke_command(const char *path, const char *method, ubus_data_handler_t cb, char *str)
{  
	uint32_t id;

	LogWrite(DEBUG, "path:%s method:%s data:%s\n", path, method, str);

	if (!ctx) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to connect to ubusd\n",
			__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	blob_buf_init(&b, 0);
	if (str != NULL && !blobmsg_add_json_from_string(&b, str)) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to parse message data\n",
			__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	if (ubus_lookup_id(ctx, path, &id)) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to look up %s object\n",
			__FILENAME__, __LINE__, __FUNCTION__, path);
		return -1;
	}

	if(UBUS_STATUS_OK != ubus_invoke(ctx, id, method, b.head, cb, 0, 3000)) {
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to callback\n",
			__FILENAME__, __LINE__, __FUNCTION__);
		return -1;
	}

	return 0;
}  

/**
 * Ubus connect to ubusd thread
 * @param  arg [description]
 * @return     [description]
 */
void *ubus_client_main(void *arg)
{
	const char *ubus_socket = NULL;

	uloop_init();

	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		sem_post(&bin_sem);
		LogWrite(ERROR, "file:%s line:%d func:%s, Failed to connect to ubus\n",
			__FILENAME__, __LINE__, __FUNCTION__);
		pthread_exit("Failed to connect to ubus\n");
	}

	ubus_add_uloop(ctx);

	sem_post(&bin_sem);

	pthread_exit("Thread ubus_client_main stop\n");
}

/**
 * Initialation ubus connect thread
 * @return  [description]
 */
int ubus_client_init(void)
{
	int ret;
	pthread_t a_thread;

	LogWrite(DEBUG, "BUILD TIME:%s %s\n", __DATE__, __TIME__);
	LogWrite(DEBUG, "Ubus client initialization\n");

	ret = sem_init(&bin_sem, 0, 0);
	if (ret != 0)  
	{ 
		LogWrite(ERROR, "file:%s line:%d func:%s, Semaphore initialization failed\n",
			__FILENAME__, __LINE__, __FUNCTION__);

		return ret;
	}

	ret = pthread_create(&a_thread, NULL, ubus_client_main, (void *)NULL);
	if (ret != 0)
	{
		LogWrite(ERROR, "file:%s line:%d func:%s, Create ubus client pthread failed\n",
			__FILENAME__, __LINE__, __FUNCTION__);

		return ret;
	}

	sem_wait(&bin_sem);

	return ret;
}

/**
 * test function ubus_invoke_command
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int test_main(int argc, char const *argv[])
{
	ubus_client_init();

	ubus_invoke_command("sysinfo", "sysinfo_get", receive_call_result_data, 
						"{\"get_args\":[\"model_name\",\"hardware_version\"]}");
	ubus_invoke_command("port_config", "port_config_get", receive_call_result_data, 
							"{\"get_args\":\
								[\
									{\
										\"idx-1\":\
										[\
											\"type\",\
											\"admin_mode\",\
											\"link_status\",\
											\"physical_mode\",\
											\"physical_status\",\
											\"flow_control_mode\",\
											\"state\",\
											\"link_trap_mode\",\
											\"threshold_trap_mode\",\
											\"tx_threshold_value\",\
											\"rx_threshold_value\"\
										]\
									}\
								]\
							}"
						);
	return 0;
}