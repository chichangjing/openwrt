#include "../../include/acci.h"

#include <termios.h>
#include <stdbool.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/time.h>
#include <stdio.h>

#define MAKE_ACCI

#ifndef INPUTBUFLEN
#define INPUTBUFLEN     256
#endif

#define CLEAR()             printf("\033[2J")                       // 清除屏幕
#define CLEAR_ROW()         printf("\033[K")                        // 清除行
#define MOVEUP(x)           printf("\033[%dA", (x))                 // 上移光标
#define MOVEDOWN(x)         printf("\033[%dB", (x))                 // 下移光标
#define MOVELEFT(y)         printf("\033[%dD", (y))                 // 左移光标
#define MOVERIGHT(y)        printf("\033[%dC", (y))                 // 右移光标
#define MOVETO(x,y)         printf("\033[%d;%dH", (x), (y))         // 定位光标
#define RESET_CURSOR()      printf("\033[H")                        // 光标复位
#define HIDE_CURSOR()       printf("\033[?25l")                     // 隐藏光标
#define SHOW_CURSOR()       printf("\033[?25h")                     // 显示光标
#define HIGHT_LIGHT()       printf("\033[1m")                       // 反显
#define UN_HIGHT_LIGHT()    printf("\033[0m")                       // 反显

jmp_buf _loop_env;

struct  _terminal_view _terminal_view[1];

char    in_put_buf[INPUTBUFLEN];
char *  device_name	=	"<Z20208D> ";
char *  exit_cmd    =   "6097";

static  struct  _cmd_record_list    _cmd_record_list[1];
static  struct  termios termios[1];
static  u_int32_t _line_max;
static u_int32_t CLI_TIMEOUT = 300;

void print_nchar(u_int32_t n, char _letter){
    char * tmp = (char *)malloc(sizeof(char) * n + sizeof('\0'));
    memset(tmp, 0, sizeof(char)*n);
    memset(tmp, _letter, sizeof(char)*n-1);
    printf("%s\n", tmp);
    free(tmp);
    tmp=NULL;
    return;
}

void posi_err(u_int32_t len){
    if(len <= INPUTBUFLEN){
        for(; len>0; printf("%1s", ""), len--){}
        printf("^\n");
    }
}

void _more_print(const char * str){
    const char * head = str;
    for(_line_max=0; *head; head++){
        if('\n' == *head){
            if(_LINE_MAX <= _line_max){
                MOVELEFT(strlen("--more--"));
                CLEAR();
                printf("%s--more--", str);
            }else{
                printf("%s", str);
            }
            fflush(stdout);
            _line_max++;
        }
    }
}

static char sig_getc(){
    SHOW_CURSOR();
    char c = getc(stdin);
    alarm(CLI_TIMEOUT);
    return c;
}

static void _loop_init(u_int32_t __signal){
    if(__signal==SIGINT){
        alarm(CLI_TIMEOUT);
        printf("\n");
        siglongjmp(_loop_env, 1);
    }
}

int exit_cli(char * argv){
    char *extra;
    if(!(extra = strtok(argv, " ")))
        return _EXIT_CLI;
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    return printf(" %% Too many parameters!\n");
}

void term_init(void){
    signal(SIGINT,  SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    struct	termios	 term_tmp[1];
    tcgetattr(0,	(void *)termios);
    memcpy(term_tmp,	termios, sizeof(struct termios));
    term_tmp->c_lflag &= ~(ICANON|ECHO|ECHONL);
    tcsetattr(0, TCSANOW, (void *)term_tmp);
}

void cmd_list_init(void){
	struct _cmd_record * tmp = (struct _cmd_record *)_MALLOC_LEN(sizeof(struct _cmd_record));
	tmp->_record_cmd = (char *)_MALLOC_LEN(strlen("") + sizeof('\0'));
	strcpy(tmp->_record_cmd, "");
	tmp->next   =   NULL;
	tmp->prev   =   tmp;

	_cmd_record_list->record_num_max    =   100;
	_cmd_record_list->current_cmd   =   NULL;
	_cmd_record_list->record_num    =   0;
	_cmd_record_list->tail  =   NULL;
	_cmd_record_list->head  =   tmp;
}

int32_t view_init(char * cmd_head, struct _cmd_list * _cmd_list){
    _terminal_view->_cmd_list->cmd_num  =   _cmd_list->cmd_num;
    _terminal_view->_cmd_list->_cmd_    =   _cmd_list->_cmd_;
    _terminal_view->cursor  =   0;
    memset(_terminal_view->cmd_head, 0, sizeof(_terminal_view->cmd_head));
    strcpy(_terminal_view->cmd_head, cmd_head);
	return  1;
}

int32_t time_out_set(u_int32_t time){
    return time>=10 && time<=3600 ? (CLI_TIMEOUT = time):-1;
}

int32_t set_cmd_max(u_int32_t max){
    if(max<3 || max>100)
        return -1;
    _cmd_record_list->record_num_max = max;
    while(_cmd_record_list->record_num > _cmd_record_list->record_num_max){
        _cmd_record_list->tail = _cmd_record_list->tail->prev;
        _FREE(_cmd_record_list->tail->next->_record_cmd);
        _cmd_record_list->tail->next->_record_cmd = NULL;
        _cmd_record_list->tail->next->prev = NULL;
        _cmd_record_list->tail->next->next = NULL;
        _FREE(_cmd_record_list->tail->next);
        _cmd_record_list->tail->next = NULL;
        _cmd_record_list->record_num--;
    }
    return 0;
}

int32_t display_history_cmd(void){
    u_int32_t i;
    struct _cmd_record * tmp = _cmd_record_list->tail;
    printf("\033[1m%60s\033[0m\n", "command history:");
    print_nchar(61, '-');
    for(i=0; tmp!=tmp->prev; tmp=tmp->prev, i++)
        printf(" %8d:    %s\n", i, tmp->_record_cmd);
    print_nchar(61, '-');
    printf(" %% Total command count: %d. Max command count:%d.\n", \
            _cmd_record_list->record_num, _cmd_record_list->record_num_max);
    print_nchar(61, '-');
    return fflush(stdout);
}

int32_t	display_terminal(void){
    return printf("\n"
                  " *****************************************************\n"
			       " * JoyWare Industry Switch Terminal.                 *\n"
			       " * Build in %12s %8s.                   *\n"
			       " * Copyright(c) 2016 JoyWare Electronics Co., Ltd.   *\n"
			       " * All rights reserved.                              *\n"
			       " *****************************************************\n"
			       "\n",  __DATE__, __TIME__);
}

static void cmd_record(const char * _cmd){
    struct _cmd_record * tmp;
    for(tmp=_cmd_record_list->head; tmp; tmp = tmp->next){
        if(0 == strcmp(_cmd, tmp->_record_cmd)){
            tmp->prev->next = tmp->next;
            if(tmp->next)
                tmp->next->prev = tmp->prev;
            else
                _cmd_record_list->tail = tmp->prev;
			break;
        }
    }
    if(!tmp){
		tmp = (struct _cmd_record *)_MALLOC_LEN(sizeof(struct  _cmd_record));
		tmp->_record_cmd = (char *)_MALLOC_LEN(strlen(_cmd)+sizeof('\0'));
		strcpy(tmp->_record_cmd, _cmd);
		_cmd_record_list->record_num++;
	}
	tmp->next = _cmd_record_list->head->next;
	tmp->prev = _cmd_record_list->head;
	if(!_cmd_record_list->head->next)
		_cmd_record_list->tail = tmp;
	else
		_cmd_record_list->head->next->prev = tmp;
	_cmd_record_list->head->next = tmp;
	while(_cmd_record_list->record_num > _cmd_record_list->record_num_max){
		_cmd_record_list->tail = _cmd_record_list->tail->prev;
		_FREE(_cmd_record_list->tail->next->_record_cmd);
		_cmd_record_list->tail->next->_record_cmd = NULL;
		_cmd_record_list->tail->next->prev = NULL;
		_cmd_record_list->tail->next->next = NULL;
		_FREE(_cmd_record_list->tail->next);
		_cmd_record_list->tail->next = NULL;
		_cmd_record_list->record_num--;
	}
}

static void hander_input(char c){
	if(INPUTBUFLEN - 1 == strlen(in_put_buf))
		return;
	u_int32_t i;
	for(i = strlen(in_put_buf); i > _terminal_view->cursor; i--)
		in_put_buf[i] = in_put_buf[i-1];
	in_put_buf[_terminal_view->cursor] = c;
	CLEAR_ROW();
	printf("%s", in_put_buf + _terminal_view->cursor);
	_terminal_view->cursor ++;
	if(strlen(in_put_buf) - _terminal_view->cursor)
		MOVELEFT(strlen(in_put_buf) - _terminal_view->cursor);
	fflush(stdout);
}

static void hander_backspace(void){
	if('\0' == in_put_buf[0] || 0==_terminal_view->cursor)
		return;
	u_int32_t i;
	for(i=_terminal_view->cursor-1; i < strlen(in_put_buf); i++)
		in_put_buf[i] = in_put_buf[i+1];
	_terminal_view->cursor--;
	MOVELEFT(1);
	CLEAR_ROW();
	printf("%s", in_put_buf + _terminal_view->cursor);
	if(strlen(in_put_buf) - _terminal_view->cursor)
		MOVELEFT(strlen(in_put_buf) - _terminal_view->cursor);
	fflush(stdout);
}

static void hander_delete(void){
	if(_terminal_view->cursor == strlen(in_put_buf))
		return;
	u_int32_t i;
	for(i=_terminal_view->cursor; i < strlen(in_put_buf); i++)
		in_put_buf[i] = in_put_buf[i+1];
	CLEAR_ROW();
	printf("%s", in_put_buf + _terminal_view->cursor);
	if(strlen(in_put_buf) - _terminal_view->cursor)
		MOVELEFT(strlen(in_put_buf) - _terminal_view->cursor);
	fflush(stdout);
}

static void hander_up(void){
	if(NULL == _cmd_record_list->current_cmd)
		_cmd_record_list->current_cmd = _cmd_record_list->head;
	if(NULL != _cmd_record_list->current_cmd->next){
		_cmd_record_list->current_cmd = _cmd_record_list->current_cmd->next;
		memset(in_put_buf, 0, sizeof(char)*INPUTBUFLEN);
		strcpy(in_put_buf, _cmd_record_list->current_cmd->_record_cmd);
		if(0 != _terminal_view->cursor)
			MOVELEFT(_terminal_view->cursor);
		CLEAR_ROW();
		printf("%s", in_put_buf);
		_terminal_view->cursor = strlen(in_put_buf);
		fflush(stdout);
	}
}

static void hander_down(void){
	if(NULL == _cmd_record_list->current_cmd)
		_cmd_record_list->current_cmd = _cmd_record_list->head;
	if(NULL != _cmd_record_list->current_cmd->prev){
		_cmd_record_list->current_cmd = _cmd_record_list->current_cmd->prev;
		memset(in_put_buf,  0,  sizeof(char)*INPUTBUFLEN);
		strcpy(in_put_buf,  _cmd_record_list->current_cmd->_record_cmd);
		if(0 != _terminal_view->cursor)
			MOVELEFT(_terminal_view->cursor);
		CLEAR_ROW();
		printf("%s", in_put_buf);
		_terminal_view->cursor = strlen(in_put_buf);
		fflush(stdout);
	}
}

static	void hander_right(void){
	if(_terminal_view->cursor < strlen(in_put_buf)){
		_terminal_view->cursor++;
		MOVERIGHT(1);
		fflush(stdout);
	}
}

static	void hander_left(void){
    if(_terminal_view->cursor > 0){
        _terminal_view->cursor--;
        MOVELEFT(1);
        fflush(stdout);
	}
}

static int32_t str_cmp(const char *str_1, const char *str_2){
    u_int32_t i, len;
    for(i=0, len = strlen(str_1)<strlen(str_2) ? strlen(str_1):strlen(str_2);i<len && str_1[i] ==  str_2[i];i++){}
    return i;
}

static int32_t var_cmd_idx(int cmd_node, const char * cmd_text, struct _cmd_list * _cmd_list){
    u_int32_t i, cmd_num;
    for(i=0, cmd_num=_cmd_list->cmd_num; i<_cmd_list->cmd_num; i++){
        if(_cmd_list->_cmd_[i] && (cmd_node >= _cmd_list->_cmd_[i]->node_num \
            || strlen(cmd_text) > strlen(_cmd_list->_cmd_[i]->_cmd_node[cmd_node]->name) \
            || 0 != strncmp(cmd_text, _cmd_list->_cmd_[i]->_cmd_node[cmd_node]->name, strlen(cmd_text))))
            _cmd_list->_cmd_[i] = NULL;
        if(!_cmd_list->_cmd_[i])
            cmd_num--;
    }
    return cmd_num;
}

static int32_t var_cmd(int cmd_node, const char * cmd_text, struct _cmd_list * _cmd_list, struct _cmd_ ** _cmd_){
    u_int32_t i, cmd_num;
    for(i=0, cmd_num=_cmd_list->cmd_num; i<_cmd_list->cmd_num; i++){
        if(_cmd_list->_cmd_[i] && (cmd_node >= _cmd_list->_cmd_[i]->node_num \
            || strlen(cmd_text) > strlen(_cmd_list->_cmd_[i]->_cmd_node[cmd_node]->name) \
            || 0 != strcmp(cmd_text, _cmd_list->_cmd_[i]->_cmd_node[cmd_node]->name)))
            _cmd_list->_cmd_[i] = NULL;
        if(_cmd_list->_cmd_[i] && cmd_node == _cmd_list->_cmd_[i]->node_num - 1 \
                && strcmp(cmd_text, _cmd_list->_cmd_[i]->_cmd_node[cmd_node]->name) == 0)
            *_cmd_ = _cmd_list->_cmd_[i];
        if(!_cmd_list->_cmd_[i])
            cmd_num--;
    }
    return cmd_num;
}

static void var_cmd_node(int cmd_node, struct _cmd_node ** _cmd_node, struct _cmd_list * _cmd_list){
    u_int32_t i, j;
    for(i=0; i<_cmd_list->cmd_num; i++){
        if(!_cmd_list->_cmd_[i])
            continue;
        for(j=0; j<=i; j++){
            if(_cmd_node[j] == _cmd_list->_cmd_[i]->_cmd_node[cmd_node])
                break;
            if(!_cmd_node[j]){
                _cmd_node[j] = _cmd_list->_cmd_[i]->_cmd_node[cmd_node];
                break;
            }
        }
    }
}

static void complete_text(char * cpl_len, int len, struct _cmd_node ** _cmd_node){
    u_int32_t i=0, j=0, mark =0;
    memset(cpl_len, '\0', TEXT_LEN);
    for(i=0; i<len && _cmd_node[i]; i++){
        if(mark){
            j = str_cmp(cpl_len, _cmd_node[i]->name);
            memset(cpl_len, '\0', TEXT_LEN);
            memcpy(cpl_len, _cmd_node[i]->name, j);
        }else{
            strcpy(cpl_len, _cmd_node[i]->name);
            mark = 1;
        }
        printf("%12s%-20s%s\n", "", _cmd_node[i]->name ? _cmd_node[i]->name:"", \
                _cmd_node[i]->help ? _cmd_node[i]->help:"");
    }
}

static int32_t var_option(const char * cmd_text, int opt_num, struct _cmd_opt ** _cmd_opt){
    u_int32_t i=0, j=0;
    for(i=0; i<opt_num; i++){
        if(_cmd_opt[i] && _cmd_opt[i]->parm_num){
            for(j=0; j<_cmd_opt[i]->parm_num; j++){
                if(NULL != _cmd_opt[i]->parameters[j]
                        && 0==strcmp(cmd_text, _cmd_opt[i]->parameters[j])){
                    _cmd_opt[i] = NULL;
                    return 1;
                }
            }
            return -1;
        }
       if(_cmd_opt[i] && !_cmd_opt[i]->parm_num){
           _cmd_opt[i]=NULL;
           return 1;
        }
    }
    return 0;
}

static int32_t list_option(const char * cmd_text, char * cpl_len, int opt_num, struct _cmd_opt ** _cmd_opt){
    char * tmp = NULL;
    memset(cpl_len, '\0', TEXT_LEN);
    u_int32_t i=0, j=0, m=0, mark=0, var_opt_num = -1;
    for(i=0; i<opt_num; i++){
        if(_cmd_opt[i] && _cmd_opt[i]->parm_num){
            for(j=0, var_opt_num=-1; j<_cmd_opt[i]->parm_num; j++){
                if(_cmd_opt[i]->parameters[j])
                    tmp = _cmd_opt[i]->parameters[j];
                else
                    continue;
                if(!strncmp(cmd_text, tmp, strlen(cmd_text))){
                    var_opt_num++;
                    printf("%12s%s\n", "", tmp);
                    if(mark){
                        m = str_cmp(cpl_len, tmp);
                        memset(cpl_len, '\0', TEXT_LEN);
                        memcpy(cpl_len, tmp, m);
                    }else{
                        memcpy(cpl_len, tmp, strlen(tmp));
                        mark =1;
                    }
                    continue;
                }
            }
            return var_opt_num;
        }
        if(_cmd_opt[i] && !_cmd_opt[i]->parm_num){
            char * posi = (char *)malloc(strlen(in_put_buf) + strlen(_terminal_view->cmd_head) + sizeof('\0'));
            posi = memset(posi, 0x00, strlen(in_put_buf) + strlen(_terminal_view->cmd_head) + sizeof('\0'));
            posi = memset(posi, 0x20, strlen(in_put_buf) + strlen(_terminal_view->cmd_head));
            printf("%s%s\n", posi, _cmd_opt[i]->name);
            free(posi);
            posi = NULL;
            return 0;
        }
    }
    return 0;
}

static void hander_tab(void){
    printf("\n");
    u_int32_t i;
    u_int32_t cmd_node = 0;
    u_int32_t space_posi = 0;
    bool space_mark = true;
    char cpl_len[TEXT_LEN];
    char cmd_text[TEXT_LEN];
    struct _cmd_    *   _cmd_;
    struct _cmd_opt **  _cmd_opt;
    struct _cmd_list    var_cmd_list[1];
    const int cmd_len = strlen(in_put_buf);
    var_cmd_list->cmd_num = _terminal_view->_cmd_list->cmd_num;
    struct _cmd_node * cmd_node_var[var_cmd_list->cmd_num];
    if(!(var_cmd_list->_cmd_ = (struct _cmd_ **)_MALLOC_LEN(\
            _terminal_view->_cmd_list->cmd_num * sizeof(struct _cmd_ *)))){
        fprintf(stderr, " %% Terminal error: %s!\n", "_cmd_");
        goto _end;
    }
    memcpy(var_cmd_list->_cmd_, _terminal_view->_cmd_list->_cmd_, \
            _terminal_view->_cmd_list->cmd_num * sizeof(struct _cmd_ *));
    for(i=0, _cmd_=NULL; i<=cmd_len && !_cmd_; i++){
        if (i == cmd_len){
            memset(cmd_text, '\0', sizeof(cmd_text));
            memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
            if(0>=var_cmd_idx(cmd_node, cmd_text, var_cmd_list)){
                posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                printf(" %% No valid cammand found!\n");
                goto _c_end;
            };
            memset(cmd_node_var, 0, sizeof(struct _cmd_node *) * var_cmd_list->cmd_num);
            var_cmd_node(cmd_node, cmd_node_var, var_cmd_list);
            complete_text(cpl_len, var_cmd_list->cmd_num, cmd_node_var);
            memcpy(in_put_buf + space_posi, cpl_len, strlen(cpl_len));
            goto _c_end;
        }
        if (KEY_SPACE == in_put_buf[i]){
            if(!space_mark){
                memset(cmd_text, '\0', sizeof(cmd_text));
                memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
                if(0>=var_cmd(cmd_node, cmd_text, var_cmd_list, &_cmd_)){
                    posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                    printf(" %% No valid cammand found!\n");
                    goto _c_end;
                };
                cmd_node++;
            }
            space_posi = i + 1;
            space_mark = true;
            continue;
        }else{
            space_mark = false;
            continue;
        }
    }
    if(!_cmd_){
        posi_err(strlen(_terminal_view->cmd_head) + space_posi);
        printf(" %% No valid cammand found!\n");
        goto _c_end;
    }
    if(!(_cmd_opt = (struct _cmd_opt **)_MALLOC_LEN(sizeof(struct _cmd_opt *) * _cmd_->optn_num))){
        fprintf(stderr, " %% Terminal error: %s!\n", "_cmd_opt");
        goto _c_end;
    }
    memcpy(_cmd_opt, _cmd_->_cmd_optn, sizeof(struct _cmd_opt *) * _cmd_->optn_num);
    for(; i<=cmd_len && NULL != _cmd_; i++){
        if(i == cmd_len){
            memset(cmd_text, '\0', sizeof(cmd_text));
            memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
            if(0 > list_option(cmd_text, cpl_len, _cmd_->optn_num, _cmd_opt)){
                posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                printf(" %% No valid parameter found!\n");
                goto _o_end;
            };
            memcpy(in_put_buf + space_posi, cpl_len, strlen(cpl_len));
            goto _o_end;
        }
        if(in_put_buf[i] == KEY_SPACE){
            if(!space_mark){
                memset(cmd_text, '\0', sizeof(cmd_text));
                memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
                if(0 > var_option(cmd_text, _cmd_->optn_num, _cmd_opt)){
                    posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                    printf(" %% No valid parameter found!\n");
                    goto _o_end;
                }
            }
            space_posi = i + 1;
            space_mark = true;
            continue;
        }else{
            space_mark = false;
            continue;
        }
    }
_o_end:
    free(_cmd_opt);
_c_end:
    free(var_cmd_list->_cmd_);
_end:
    printf("%s%s", _terminal_view->cmd_head, in_put_buf);
    _terminal_view->cursor = strlen(in_put_buf);
    fflush(stdout);
    return;
}

static void hander_help(void){
    u_int32_t i;
    u_int32_t cmd_node = 0;
    u_int32_t space_posi = 0;
    bool space_mark = true;
    char cpl_len[TEXT_LEN];
    char cmd_text[TEXT_LEN];
    struct _cmd_    *   _cmd_;
    struct _cmd_opt **  _cmd_opt;
    struct _cmd_list    var_cmd_list[1];
    const u_int32_t cmd_len = strlen(in_put_buf);
    var_cmd_list->cmd_num = _terminal_view->_cmd_list->cmd_num;
    struct _cmd_node * cmd_node_var[var_cmd_list->cmd_num];
    if(!(var_cmd_list->_cmd_ = (struct _cmd_ **)_MALLOC_LEN(\
            _terminal_view->_cmd_list->cmd_num * sizeof(struct _cmd_ *)))){
        fprintf(stderr, " %% Terminal error: %s!\n", "_cmd_");
        goto _end;
    }
    memcpy(var_cmd_list->_cmd_, _terminal_view->_cmd_list->_cmd_, \
            _terminal_view->_cmd_list->cmd_num * sizeof(struct _cmd_ *));
    for(i=0, _cmd_=NULL; i<=cmd_len && !_cmd_; i++){
        if (i == cmd_len){
            memset(cmd_text, '\0', sizeof(cmd_text));
            memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
            if(0>var_cmd_idx(cmd_node, cmd_text, var_cmd_list)){
                posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                printf(" %% No valid cammand found!\n");
                goto _c_end;
            };
            memset(cmd_node_var, 0, sizeof(struct _cmd_node *) * var_cmd_list->cmd_num);
            var_cmd_node(cmd_node, cmd_node_var, var_cmd_list);
            complete_text(cpl_len, var_cmd_list->cmd_num, cmd_node_var);
            memcpy(in_put_buf + space_posi, cpl_len, strlen(cpl_len));
            goto _c_end;
        }
        if (in_put_buf[i] == KEY_SPACE){
            if(!space_mark){
                memset(cmd_text, '\0', sizeof(cmd_text));
                memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
                if(0>var_cmd(cmd_node, cmd_text, var_cmd_list, &_cmd_)){
                    posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                    printf(" %% No valid cammand found!\n");
                    goto _c_end;
                };
                cmd_node++;
            }
            space_posi = i + 1;
            space_mark = true;
            continue;
        }else{
            space_mark = false;
            continue;
        }
    }
    if(!_cmd_){
        posi_err(strlen(_terminal_view->cmd_head) + space_posi);
        printf(" %% No valid cammand found!\n");
        goto _c_end;
    }
    if(!(_cmd_opt = (struct _cmd_opt **)_MALLOC_LEN(sizeof(struct _cmd_opt *) * _cmd_->optn_num))){
        fprintf(stderr, " %% Terminal error: %s!\n", "_cmd_opt");
        goto _c_end;
    }
    memcpy(_cmd_opt, _cmd_->_cmd_optn, sizeof(struct _cmd_opt *) * _cmd_->optn_num);
    for(; i<=cmd_len && NULL != _cmd_; i++){
        if(i == cmd_len){
            memset(cmd_text, '\0', sizeof(cmd_text));
            memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
            if(0 >= list_option(cmd_text, cpl_len, _cmd_->optn_num, _cmd_opt)){
                posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                printf(" %% No valid parameter found!\n");
                goto _o_end;
            };
            memcpy(in_put_buf + space_posi, cpl_len, strlen(cpl_len));
            goto _o_end;
        }
        if(in_put_buf[i] == KEY_SPACE){
            if(!space_mark){
                memset(cmd_text, '\0', sizeof(cmd_text));
                memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
                if(0 >= var_option(cmd_text, _cmd_->optn_num, _cmd_opt)){
                    posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                    printf(" %% No valid parameter found!\n");
                    goto _o_end;
                }
            }
            space_posi = i + 1;
            space_mark = true;
            continue;
        }else{
            space_mark = false;
            continue;
        }
    }
_o_end:
    free(_cmd_opt);
_c_end:
    free(var_cmd_list->_cmd_);
_end:
    printf("%s%s\n", _terminal_view->cmd_head, in_put_buf);
    _terminal_view->cursor = strlen(in_put_buf);
    fflush(stdout);
    return;
}

static int32_t hander_enter(void){
    if(printf("\n") && !in_put_buf[0])
        return 1;
    cmd_record(in_put_buf);
    u_int32_t i;
    u_int32_t cmd_node = 0;
    u_int32_t space_posi = 0;
    bool space_mark = true;
    char cmd_text[TEXT_LEN];
    struct _cmd_    *   _cmd_;
    struct _cmd_list    var_cmd_list[1];
    const  u_int32_t cmd_len = strlen(in_put_buf);
    var_cmd_list->cmd_num = _terminal_view->_cmd_list->cmd_num;
    if(!(var_cmd_list->_cmd_ = (struct _cmd_ **)_MALLOC_LEN(\
            _terminal_view->_cmd_list->cmd_num * sizeof(struct _cmd_ *)))){
        fprintf(stderr, " %% Terminal error: %s!\n", "_cmd_");
        goto _end;
    }
    memcpy(var_cmd_list->_cmd_, _terminal_view->_cmd_list->_cmd_, \
            _terminal_view->_cmd_list->cmd_num * sizeof(struct _cmd_ *));
    for(i=0, _cmd_=NULL; i<=cmd_len && !_cmd_; i++){
        if (i == cmd_len || in_put_buf[i] == KEY_SPACE){
            if(!space_mark){
                memset(cmd_text, '\0', sizeof(cmd_text));
                memcpy(cmd_text, in_put_buf + space_posi, i - space_posi);
                if(0>=var_cmd(cmd_node, cmd_text, var_cmd_list, &_cmd_)){
                    posi_err(strlen(_terminal_view->cmd_head) + space_posi);
                    printf(" %% No valid cammand found!\n");
                    goto _c_end;
                };
                cmd_node++;
            }
            space_posi = i + 1;
            space_mark = true;
            continue;
        }else{
            space_mark = false;
            continue;
        }
    }
    if(!_cmd_){
        posi_err(strlen(_terminal_view->cmd_head) + space_posi);
        printf(" %% No valid cammand found!\n");
        goto _c_end;
    }
    if(!_cmd_->_c_fun_){
        printf(" %% No valid functin found!\n");
        goto _c_end;
    }
    return _cmd_->_c_fun_(in_put_buf+i);
_c_end:
    free(var_cmd_list->_cmd_);
_end:
    return fflush(stdout);
}

int32_t loop_terminal(void){
    char c;
    while(1){
        signal(SIGINT,  _loop_init);
        sigsetjmp(_loop_env, 1);
        _terminal_view->cursor = 0;
        _cmd_record_list->current_cmd = NULL;
        memset(in_put_buf, 0, sizeof(char)*INPUTBUFLEN);
        printf("%s", _terminal_view->cmd_head);
        fflush(stdout);
        while ((c = sig_getc(stdin)) != '\n') {
			if(0x1B == c){
				c = sig_getc(stdin);
				if(0x5B == c || 79==c){
					c = sig_getc(stdin);
					switch (c){
						case 0x33:
							c = sig_getc(stdin);
							if(0x7e == c){
								hander_delete();
							//hander_backspace();
							}
							continue;
							break;
						case KEY_UP:
							hander_up();
							continue;
							break;
						case KEY_DOWN:
							hander_down();
							continue;
							break;
						case KEY_RIGHT:
							hander_right();
							continue;
							break;
						case KEY_LEFT:
							hander_left();
							continue;
							break;
					}
				}
				continue;
			}
			if(c >= 0x20 && c <= 0x7E){
				hander_input(c);
			}else if(KEY_BACKSPACE  == c){
				hander_backspace();
			}else if(KEY_DELETE     == c){
			    hander_delete();
				//hander_backspace();
			}else if(KEY_TAB        == c){
				hander_tab();
			}else if(KEY_HELP       == c){
			    hander_help();
			}
		}
        HIDE_CURSOR();
        signal(SIGINT,  SIG_IGN);
        if(_EXIT_CLI == hander_enter())
            goto _end;
        SHOW_CURSOR();
        fflush(stdout);
	}
_end:
    return 0;
}

int32_t _lo_input(char * buf, int32_t len, char mask){
    char c;
    int cursor=0;
    memset(buf, 0, len);
    while ((c = sig_getc(stdin)) != '\n') {
        if(0x1B == c){
            c = sig_getc(stdin);
            if(0x5B == c){
                c = sig_getc(stdin);
                switch (c){
                    case 0x33:
                        c = sig_getc(stdin);
                        if(0x7e == c){}
                        continue;
                        break;
                    case KEY_UP:
                        continue;
                        break;
                    case KEY_DOWN:
                        continue;
                        break;
                    case KEY_RIGHT:
                        continue;
                        break;
                    case KEY_LEFT:
                        continue;
                        break;
                }
            }
            continue;
        }
        if(c >= 0x20 && c <= 0x7E){
            if(cursor<len-1){
                buf[cursor]=c;
                cursor++;
                if(mask<0x7f)
                    printf("%c", mask);
                else
                    printf("%c", c);
                fflush(stdout);
            }
        }else if(KEY_BACKSPACE  == c){
            if(cursor>0){
                buf[cursor]='\0';
                cursor--;
                if(mask)
                    MOVELEFT(1);
                CLEAR_ROW();
                fflush(stdout);
            }
        }else if(KEY_DELETE     == c){
            if(cursor>0){
                buf[cursor]='\0';
                cursor--;
                if(mask)
                    MOVELEFT(1);
                CLEAR_ROW();
                fflush(stdout);
            }
        }else if(KEY_TAB        == c){
        }else if(KEY_HELP       == c){
        }
    }
    printf("\n");
    return strlen(buf);
}

#ifdef MAKE_ACCI


struct acci_node display_nodes[]={
        {
                .help="display terminal configuration",
                .name="display",
                .sub_num=0,
                .type_node=_cmd_end,
        },
        {
                .help="display terminal configuration",
                .name="terminal",
                .sub_num=0,
                .type_node=_cmd_end,
        },
};

struct _cmd_ display_cmd={
        .acci_node=display_nodes,
        .node_num=_ARRY_LEN(display_nodes),
};

struct acci_node acl_nodes[]={
        {
                .help="acl information configuration",
                .name="acl",
                .sub_num=0,
                .para_node=_string,
                .type_node=_necessary_cmd,
        },
        {
                .help="acl number",
                .name="number",
                .sub_num=0,
                .para_node=_string,
                .type_node=_necessary_cmd,
        },

        {
                .name="INTEGER<2000-2999>",
                .help="Specify a basic acl",
                .sub_num=0,
                .para_node=_int,
                .type_node=_cmd_end,
        },
        {
                .name="INTEGER<3000-3999>",
                .help="Specify an advanced acl",
                .sub_num=0,
                .para_node=_int,
                .type_node=_cmd_end,
        },
        {
                .name="INTEGER<4000-4999>",
                .help="Specify a link acl",
                .sub_num=0,
                .para_node=_int,
                .type_node=_cmd_end,
        },
        {
                .name="INTEGER<5000-5999>",
                .help="Specify a user-defined frame or packet header acl",
                .sub_num=0,
                .para_node=_int,
                .type_node=_cmd_end,
        },
};

struct _cmd_ acl_cmd={
        .acci_node=acl_nodes,
        .node_num=_ARRY_LEN(acl_nodes),
};

struct _cmd_ * system_cmds = {
        &acl_cmd,
        &display_cmd,
};
struct acci_list system_view={
        .acci_cmd=system_cmds,
        .cmd_num=_ARRY_LEN(system_cmds),
};

int32_t main(int32_t argc, char ** argv){
    view_init(device_name, system_view);
    loop_terminal();
    return 1;
}

#endif
