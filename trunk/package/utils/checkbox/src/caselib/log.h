#ifndef __LOG_H__
#define __LOG_H__
extern int _log_level;
void	log_init(const char *prog);
void	set_log_level(int log_level);
void	print_row(int n);
enum{
	_LOG_EMERG,
	_LOG_ALERT,
	_LOG_CRIT,
	_LOG_ERR,
	_LOG_WARING,
	_LOG_NOTICE,
	_LOG_IFNO ,
	_LOG_DEBUG,
	_LOG_MAX,
};
#define __B__BLACK		"\033[40m"
#define __B__RED		"\033[41m"
#define __B__GREEN		"\033[42m"
#define __B__YELLOW		"\033[43m"
#define __B__BLUE		"\033[44m"
#define __B__CYAN		"\033[45m"
#define __B__FUCHSIN	"\033[46m"
#define __B__WHITE		"\033[47m"

#define __F__BLACK		"\033[30m"
#define __F__RED		"\033[31m"
#define __F__GREEN		"\033[32m"
#define __F__YELLOW		"\033[33m"
#define __F__BLUE		"\033[34m"
#define __F__CYAN		"\033[35m"
#define __F__FUCHSIN	"\033[36m"
#define __F__WHITE		"\033[37m"
#define __F__BOLD		"\033[1m"

#define __C__INIT		"\033[0m"
#define __C__LIGHT		"\033[1m"							// 反显

#define __C__CLR 		"\033[2J"							// 清除屏幕
#define __C__CLR_ROW	"\033[K"							// 清除行
#define __C__RST_CUR 	"\033[H"							// 光标复位
#define __C__HIDE_CUR	"\033[?25l"							// 隐藏光标
#define __C__SHOW_CUR	"\033[?25h"							// 显示光标

#define __C__UP(x) 		"\033["#x"A"						// 上移光标
#define __C__DOWN(x) 	"\033["#x"B"						// 下移光标
#define __C__LEFT(y) 	"\033["#y"D"						// 左移光标
#define __C__RIGHT(y) 	"\033["#y"C"						// 右移光标
#define __C__TO(x,y) 	"\033["#x";"#y"H"					// 定位光标

#define _TO_SYSLOG	-1
#define _TO_TERMINAL	-2
#define _TO_WHERE	_TO_TERMINAL
#if (_TO_WHERE == _TO_SYSLOG)
	#define _SYSLOG(level, info, ...) syslog(level, info, ##__VA_ARGS__)
	#define _emerg(info, ...) 	do{if(_log_level>_LOG_EMERG) {_SYSLOG(_LOG_EMERG,	info,	##__VA_ARGS__);}}while(0)
	#define _alert(info, ...) 	do{if(_log_level>_LOG_ALERT) {_SYSLOG(_LOG_ALERT,		info,	##__VA_ARGS__);}}while(0)
	#define _crit(info, ...) 		do{if(_log_level>_LOG_CRIT)	 {_SYSLOG(_LOG_CRIT,			info,	##__VA_ARGS__);}}while(0)
	#define _error(info, ...) 	do{if(_log_level>_LOG_ERR)	 {_SYSLOG(_LOG_ERR,			info,	##__VA_ARGS__);}}while(0)
	#define _waring(info, ...) 	do{if(_log_level>_LOG_WARING){_SYSLOG(_LOG_WARING,	info,	##__VA_ARGS__);}}while(0)
	#define _notice(info, ...) 	do{if(_log_level>_LOG_NOTICE){_SYSLOG(_LOG_NOTICE,	info,	##__VA_ARGS__);}}while(0)
	#define _info(info, ...) 	do{if(_log_level>_LOG_IFNO)	 {_SYSLOG(_LOG_IFNO,			info,	##__VA_ARGS__);}}while(0)
	#define _debug(info, ...) 	do{if(_log_level>_LOG_DEBUG) {_SYSLOG(_LOG_DEBUG,	info,	##__VA_ARGS__);}}while(0)
#elif (_TO_WHERE == _TO_TERMINAL)
	#define _PRINT_LOG(level, info, ...) printf(info, ##__VA_ARGS__)
	#define _emerg(info, ...) 	do{if(_log_level>_LOG_EMERG) {_PRINT_LOG(_LOG_EMERG,	info,	##__VA_ARGS__);}}while(0)
	#define _alert(info, ...) 	do{if(_log_level>_LOG_ALERT) {_PRINT_LOG(_LOG_ALERT,		info,	##__VA_ARGS__);}}while(0)
	#define _crit(info, ...) 		do{if(_log_level>_LOG_CRIT)	 {_PRINT_LOG(_LOG_CRIT,			info,	##__VA_ARGS__);}}while(0)
	#define _error(info, ...) 	do{if(_log_level>_LOG_ERR)	 {_PRINT_LOG(_LOG_ERR,			info,	##__VA_ARGS__);}}while(0)
	#define _waring(info, ...) 	do{if(_log_level>_LOG_WARING){_PRINT_LOG(_LOG_WARING,	info,	##__VA_ARGS__);}}while(0)
	#define _notice(info, ...) 	do{if(_log_level>_LOG_NOTICE){_PRINT_LOG(_LOG_NOTICE,	info,	##__VA_ARGS__);}}while(0)
	#define _info(info, ...) 	do{if(_log_level>_LOG_IFNO)	 {_PRINT_LOG(_LOG_IFNO,			info,	##__VA_ARGS__);}}while(0)
	#define _debug(info, ...) 	do{if(_log_level>_LOG_DEBUG) {_PRINT_LOG(_LOG_DEBUG,	info,	##__VA_ARGS__);}}while(0)
#endif
#endif
