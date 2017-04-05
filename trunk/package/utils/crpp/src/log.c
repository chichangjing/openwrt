
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "log.h"

static int log_level = LOG_LEVEL_DEBUG;
static int log_channel = TO_SYSLOG;

void vDprintf(int level, const char *fmt, va_list ap)
{
	if (level > log_level)
		return;

	if (log_channel == TO_STDIO) 
    {
		char logbuf[256];
		logbuf[255] = 0;
		time_t clock;
		struct tm *local_tm;
		time(&clock);
		local_tm = localtime(&clock);
		int l = strftime(logbuf, sizeof(logbuf) - 1, "%F %T ", local_tm);
		vsnprintf(logbuf + l, sizeof(logbuf) - l - 1, fmt, ap);
		printf("%s\n", logbuf);
	} 
    else 
    {
		vsyslog(LOG_INFO, fmt, ap);
	}
}

#if 0
void vDprintf(int level, const char *fmt, va_list ap)
{
	char logbuf[256];
	
	logbuf[sizeof(logbuf) - 1] = 0;
	vsnprintf(logbuf, sizeof(logbuf) - 1, fmt, ap);
	printf("%s\n", logbuf);
}
#endif

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

void log_threshold(int level)
{
    log_level = level;
}

void log_set_channel(int channel)
{
    log_channel = channel;
}

void log_init(const char *progname,unsigned char facility)
{
	openlog(progname, LOG_PID | LOG_NDELAY, facility);
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

