
#ifndef _RPP_BASE_H_
#define _RPP_BASE_H_

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <netinet/in.h>
#include <linux/if_bridge.h>

#include "rpp_pdu.h"
#include "times.h"

#ifndef Bool
#define Bool        int
#define False       0
#define True        1
#endif

#define LED_OPEN    0
#define LED_CLOSE   1

#define JRPP_ERRORS { \
	CHOOSE(RPP_OK),                                       \
	CHOOSE(RPP_Cannot_Find_Ring),                         \
	CHOOSE(RPP_Imlicite_Instance_Create_Failed),          \
	CHOOSE(RPP_Small_Node_Priority),                      \
	CHOOSE(RPP_Large_Node_Priority),                      \
	CHOOSE(RPP_Invalid_Node_Priority),                    \
	CHOOSE(RPP_Small_Hello_Time),                         \
	CHOOSE(RPP_Large_Hello_Time),                         \
	CHOOSE(RPP_Ring_Had_Not_Yet_Been_Created),            \
	CHOOSE(RPP_Port_Is_Absent_In_The_Ring),               \
	CHOOSE(RPP_Big_len8023_Format),                       \
	CHOOSE(RPP_Small_len8023_Format),                     \
	CHOOSE(RPP_Invalid_Protocol),                         \
	CHOOSE(RPP_Had_Not_Yet_Been_Enabled_On_The_Ring),     \
	CHOOSE(RPP_Cannot_Create_Instance_For_Ring),          \
	CHOOSE(RPP_Cannot_Create_Instance_For_Port),          \
	CHOOSE(RPP_There_Are_No_Ports),                       \
	CHOOSE(RPP_Cannot_Compute_Node_Prio),                 \
	CHOOSE(RPP_Another_Error),                            \
	CHOOSE(RPP_Nothing_To_Do),                            \
	CHOOSE(RPP_LAST_DUMMY),                               \
}

#define CHOOSE(a) a
typedef enum JRPP_ERRORS JRPP_ERRORS_T;
#undef CHOOSE

#define RPP_CALLOC(x, y)      calloc(x, y)
#define RPP_FREE(x)           free(x)


void log_info(const char *fmt, ...);
void log_err(const char *fmt, ...);

#define LOG_ERROR(fmt,...) log_err("\033[1;35m[ERROR] "fmt"\033[0m", ## __VA_ARGS__)

#endif

