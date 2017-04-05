#ifndef _UBUS_CALL
#define _UBUS_CALL
#include <libubox/blobmsg_json.h>
#include "libubus.h"

#define CB_SUCCESS  0
#define CB_FAIL     -1

extern struct blob_buf buf;
typedef void (*run_fun)(void);
typedef void recv_data_cb(struct ubus_request *req, int type, struct blob_attr *msg);

#define CLEAR()             printf("\033[2J")						// 清除屏幕
#define CLEAR_ROW()         printf("\033[K")						// 清除行
#define MOVEUP(x)           printf("\033[%dA", (x))					// 上移光标
#define MOVEDOWN(x)         printf("\033[%dB", (x))					// 下移光标
#define MOVELEFT(y)         printf("\033[%dD", (y))					// 左移光标
#define MOVERIGHT(y)        printf("\033[%dC",(y))					// 右移光标
#define MOVETO(x,y)         printf("\033[%d;%dH", (x), (y))			// 定位光标
#define RESET_CURSOR()      printf("\033[H")						// 光标复位
#define HIDE_CURSOR()       printf("\033[?25l")					    // 隐藏光标
#define SHOW_CURSOR()       printf("\033[?25h")					    // 显示光标
#define HIGHT_LIGHT()       printf("\033[1m")						// 反显
//#define HIGHT_LIGHT()     printf("\033[1;7m")						// 反显
#define UN_HIGHT_LIGHT()    printf("\033[0m")					    // 反显
//#define UN_HIGHT_LIGHT()  printf("\033[0;27m")					// 反显

#define _PRINT_INFO(str, ...) \
		fprintf(stdout, str, ##__VA_ARGS__);
#define _PRINT_ERROR(str, ...) 	\
        fprintf(stderr, str, ##__VA_ARGS__);
#define _MALLOC_STR(__str)  ((char*)malloc(0==(strlen(__str) + sizeof('0'))%4 ?\
        strlen(__str) + sizeof('0'):((strlen(__str) + sizeof('0'))/4+1)*4))
#define _MALLOC_LEN(__len)  (malloc(0==((__len)%4) ? (__len):((__len)/4+1)*4))
extern const char * const blobmsg_type_info[__BLOBMSG_TYPE_LAST];

void print_row(int n);
bool is_num_d(const char * str);
int  _atoi_s(const char * str);
void receive_set_result_data(struct ubus_request *req, int type, struct blob_attr *msg);
void receive_call_result_data(struct ubus_request *req, int type, struct blob_attr *msg);
int  ubus_call(const char *path, const char *method, struct blob_buf * buf, recv_data_cb *cb);
#endif
