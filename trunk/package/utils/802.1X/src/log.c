#include <syslog.h>

#include "common/common.h"

static int log_level = LOG_LEVEL_DEFAULT;
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

void log_warn(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_WARN, fmt, ap);
	va_end(ap);
}

void log_info(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_INFO, fmt, ap);
	va_end(ap);
}

void log_debug(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vDprintf(LOG_LEVEL_DEBUG, fmt, ap);
	va_end(ap);
}

