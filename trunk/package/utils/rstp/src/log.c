
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "log.h"

void log_init(const char *progname)
{
	openlog(progname, LOG_PID | LOG_NDELAY, LOG_DAEMON);
}

void log_print(int level, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(level, fmt, ap);
	va_end(ap);
}

void log_err(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_ERROR, fmt, ap);
	va_end(ap);
}

void log_info(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_INFO, fmt, ap);
	va_end(ap);
}

void log_trace(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_MODULE, fmt, ap);
	va_end(ap);
}

void log_debug(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_DEBUG, fmt, ap);
	va_end(ap);
}

