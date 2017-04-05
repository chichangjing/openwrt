
#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdarg.h>

#define LOG_LEVEL_NONE     0
#define LOG_LEVEL_ERROR    1
#define LOG_LEVEL_INFO     2
#define LOG_LEVEL_MODULE   3
#define LOG_LEVEL_DEBUG    4
#define LOG_LEVEL_MAX      5
#define LOG_LEVEL_DEFAULT  LOG_LEVEL_INFO

extern void vDprintf(int level, const char *fmt, va_list ap);

void log_init(const char *progname);
void log_err(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_trace(const char *fmt, ...);
void log_debug(const char *fmt, ...);

static inline void dump_hex(void *b, int l)
{
	unsigned char *buf = (unsigned char *)b;
	char logbuf[80];
	int i, j;
	
	for (i = 0; i < l; i += 16) {
		for (j = 0; j < 16 && i + j < l; j++)
			sprintf(logbuf + j * 3, "%02X ", buf[i + j]);
		log_debug("\033[1;30m%s\033[0m", logbuf);
	}
	log_debug("\n");
}

#endif

