
#include "ctl_func.h"
#include "ctl_socket.h"
#include "ctl_client.h"
#include "log.h"

CLIENT_SIDE_FUNCTION(enable_rstp)
CLIENT_SIDE_FUNCTION(get_bridge_state)
CLIENT_SIDE_FUNCTION(set_bridge_config)
CLIENT_SIDE_FUNCTION(get_port_state)
CLIENT_SIDE_FUNCTION(set_port_config)
CLIENT_SIDE_FUNCTION(set_debug_level)
CLIENT_SIDE_FUNCTION(set_port_trace)

#include <base.h>
const char *CTL_error_explanation(int err_no)
{
#define CHOOSE(a) #a
	static const char *rstp_error_names[] = RSTP_ERRORS;
	static const char *ctl_error_names[] = { CTL_ERRORS };

#undef CHOOSE
	if (err_no < 0)
		return "Error doing ctl command";
	else if (err_no >= STP_OK && err_no < STP_LAST_DUMMY)
		return rstp_error_names[err_no];
	else if (err_no > Err_Dummy_Start && err_no < Err_Dummy_End)
		return ctl_error_names[err_no - Err_Dummy_Start - 1];

	static char buf[32];
	sprintf(buf, "Unknown error code %d", err_no);
	return buf;
}

void vDprintf(int level, const char *fmt, va_list ap)
{
	char logbuf[256];
	
	logbuf[sizeof(logbuf) - 1] = 0;
	vsnprintf(logbuf, sizeof(logbuf) - 1, fmt, ap);
	printf("%s\n", logbuf);
}


void Dprintf(int level, const char *fmt, ...)
{
	char logbuf[256];
	logbuf[sizeof(logbuf) - 1] = 0;
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(logbuf, sizeof(logbuf) - 1, fmt, ap);
	va_end(ap);
	printf("%s\n", logbuf);
}


