

#ifndef _CTL_FUNC_H_
#define _CTL_FUNC_H_

#include <bitmap.h>
#include <uid_stp.h>

int CTL_enable_rstp(int enable);
int CTL_get_bridge_state(UID_STP_CFG_T *cfg, UID_STP_STATE_T *state);
int CTL_set_bridge_config(UID_STP_CFG_T *cfg);
int CTL_get_port_state(int port_index, UID_STP_PORT_CFG_T *cfg, UID_STP_PORT_STATE_T *state);
int CTL_set_port_config(int port_index, UID_STP_PORT_CFG_T *cfg);
int CTL_set_debug_level(int level);
int CTL_set_port_trace(BITMAP_T *portmap, int mach_index, int enableflag);

#define CTL_ERRORS \
	CHOOSE(Err_Interface_not_a_bridge), \
	CHOOSE(Err_Bridge_RSTP_not_enabled), \
	CHOOSE(Err_Bridge_is_down), \
	CHOOSE(Err_Port_does_not_belong_to_bridge), \

#define CHOOSE(a) a

enum Errors {
	Err_Dummy_Start = 1000,
	CTL_ERRORS Err_Dummy_End
};

#undef CHOOSE

const char *CTL_error_explanation(int err);

#endif

