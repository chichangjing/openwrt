#ifndef __CLI_H__
#define __CLI_H__
#include "stdio.h"
#include <setjmp.h>
#define _EXIT_CLI       0x135246
#define _PWD_LEN        16
#define INPUTBUFLEN     256
#define KEY_HELP        0x00    //0x3F
#define KEY_BACKSPACE   0x08
#define KEY_TAB         0x09
#define KEY_DELETE      0x7F
#define KEY_SPACE       0x20
#define KEY_UP          0x41
#define KEY_DOWN        0x42
#define KEY_RIGHT       0x43
#define KEY_LEFT        0x44

#define VK_LEFT         37

#define TEXT_LEN        0x32
#define _LINE_MAX       0x10

typedef void    _t_fun_(void);
typedef int     _c_fun_(char * argv);

struct  _cmd_node   {
    char    *   name;
    char    *   help;
    _t_fun_ *   _t_fun_;
};
struct  _cmd_opt    {
    int	     parm_num;
    union{
        char    *   name;
        char    **      parameters;
    };
};
struct  _cmd_   {
    int	     node_num;
    struct  _cmd_node   **  _cmd_node;
    int	     optn_num;
    struct  _cmd_opt    **  _cmd_optn;
    _c_fun_  *   _c_fun_;
};
struct  _cmd_list   {
	int		 cmd_num;
	struct  _cmd_   **  _cmd_;
};
struct  _terminal_view  {
    int	     cursor;
    char    cmd_head[64];
    struct  _cmd_list   _cmd_list[1];
};
struct 	_cmd_record {
    char	*	_record_cmd;
    struct 	_cmd_record *   next;
    struct 	_cmd_record *   prev;
};
struct	_cmd_record_list {
    int	    record_num;
    int	    record_num_max;
    struct  _cmd_record *   head;
    struct  _cmd_record *   tail;
    struct  _cmd_record *   current_cmd;
};

int  view_init(char * cmd_head, struct _cmd_list * _cmd_list);
int  _lo_input(char * buf, int len, char mask);
int  display_history_cmd(void);
int  display_terminal(void);
int  exit_cli(char * argv);
int  loop_terminal(void);
int  set_cmd_max(int max);
void term_init(void);
void cmd_list_init(void);
void posi_err(unsigned int len);
void _more_print(const char * str);
void print_nchar(int n, char _letter);


#ifndef ___CLI___
extern char in_put_buf[INPUTBUFLEN];
extern int CLI_TIMEOUT;
extern jmp_buf _loop_env;
extern char *  device_name;
extern char *  exit_cmd;
extern struct  _terminal_view _terminal_view[1];
#endif

#define _ARRY_LEN(__array) (sizeof(__array)/sizeof(__array[0]))
#define _NEW_OPT_NODE(__optn, __parms) struct _cmd_opt __optn[1]={{.parm_num=_ARRY_LEN(__parms), .parameters=(char**)__parms}}
#define _NEW_OPT_HELP(__optn, __help) struct _cmd_opt __optn[1]={{.parm_num=0, .name=(char*)__help}}
#define _NEW_OPT_LIST(__optn_list, ...) struct  _cmd_opt * __optn_list[] = {__VA_ARGS__}
#define _NEW_NOD_NAME(__node, __name) struct _cmd_node __node[1]={{.name=__name,}}
#define _NEW_NOD_HELP(__node, __name, __help) struct _cmd_node __node[1]={{.name=__name, .help=__help,}}
#define _NEW_NOD_FUNC(__node, __name, __help, __t_fun__) struct _cmd_node __node[1]={{.name=__name, .help=__help, ._t_fun_=__t_fun__}}
#define _NEW_NOD_LIST(__node_list, ...) struct  _cmd_node * __node_list[] = {__VA_ARGS__};
#define _ADD_NODE(__node) .node_num = _ARRY_LEN(__node), ._cmd_node = __node
#define _ADD_OPTN(__optn) .optn_num = _ARRY_LEN(__optn), ._cmd_optn = __optn
#define _ADD_OPTN_NULL()  .optn_num = 0, ._cmd_optn = NULL
#define _NEW_CMD_OPTN(__cmd__, __cmd_node, __c_fun, __cmd_optn) struct _cmd_ __cmd__[1]={{_ADD_NODE(__cmd_node), _ADD_OPTN(__cmd_optn), ._c_fun_=&__c_fun}}
#define _NEW_CMD_NULL(__cmd__, __cmd_node, __c_fun) struct _cmd_ __cmd__[1]={{_ADD_NODE(__cmd_node), _ADD_OPTN_NULL(), ._c_fun_=&__c_fun}}
#define _NEW_CMD_LIST(__cmd_list, __list) struct _cmd_list __cmd_list[1]={{.cmd_num=_ARRY_LEN(__list), ._cmd_=__list,}}
#endif
