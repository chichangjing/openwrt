#ifndef __SNMP_UBUS_CLIENT_IF_H
#define __SNMP_UBUS_CLIENT_IF_H

#include "libubus.h"

#include "init/ssdk_init.h"
#include "fal/fal_mib.h"

struct jw_switch_policy {
    char *name;
    enum blobmsg_type type;
    int(*ret_handler)(struct blob_buf *buf, int port_idx);
    int(*set_handler)(int port_idx, void *var);
};

int get_port_entry_statistics(const int index, fal_mib_info_t *mib_Info);

int ubus_get_cmd_enc(char *str, const int size, const int index, const char *key);
int ubus_get_port_cmd_enc(char *str, const int size, const int index, const char *key);
int ubus_get_qos_port_cmd_enc(char *str, const int size, const int index, const char *key);
int ubus_get_qos_8021p_cmd_enc(char *str, const int size, const int index, const char *key);
int ubus_get_qos_dscp_cmd_enc(char *str, const int size, const int index, const char *key);

int ubus_set_cmd_enc(char *str, const int size, const int index, const char *key, const char *value);
int ubus_set_port_cmd_enc(char *str, const int size, const int index, const char *key, const char *value);
int ubus_set_qos_port_cmd_enc(char *str, const int size, const int index, const char *key, const char *value);
int ubus_set_qos_8021p_cmd_enc(char *str, const int size, const int index, const char *key, const char *value);
int ubus_set_qos_dscp_cmd_enc(char *str, const int size, const int index, const char *key, const char *value);
int ubus_add_cmd_enc(char *str, const int size, const int index);
int ubus_clear_cmd_enc(char *str, const int size, const int index);

void receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg);
void set_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void get_table_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);

void clear_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void port_config_get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void port_mirror_get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void port_mirror_table_get_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);
void sysinfo_ret_handler_cb(struct ubus_request *req, int type, struct blob_attr *msg);
int ubus_invoke_command(const char *path, const char *method, ubus_data_handler_t cb, char *str);
int ubus_client_init(void);

uint32_t jws_write_buf_u32(uint32_t buf);
uint32_t jws_read_buf_u32(void);
char *jws_write_buf_str(const char *buf, int size);
char *jws_read_buf_str(void);
void jws_format_buf(void);

#endif /* __SNMP_UBUS_CLIENT_IF_H */								