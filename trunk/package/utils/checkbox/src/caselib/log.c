#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include "log.h"
int		_log_level	=	_LOG_MAX;
#if (_TO_WHERE == _TO_SYSLOG)
void	log_init(const char *prog){
	openlog(prog,	LOG_NDELAY, LOG_DAEMON);
}
#elif (_TO_WHERE	==	_TO_TERMINAL)
void	log_init(const char *prog){
}
#endif

void	set_log_level(int log_level){
	if(log_level>=_LOG_EMERG && log_level < _LOG_MAX){
		_log_level = log_level;
		return;
	}else{
		_error("invalid log level set! \n");
		return;
	}
}
void	print_row(int n){
	while(n>0){
		_info("-");
		n--;
	}
	_info("\n");
}
