#include <pwd.h>
#include <stdio.h>
#include <setjmp.h>
#include <shadow.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include "cli.h"
#include "cli_parse.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/device.h"

jmp_buf _init_env_1;

static int __login_in(void){
    char * delim;
    char * enc_pw_str;
    char username[_PWD_LEN+sizeof('\0')];
    char password[_PWD_LEN+sizeof('\0')];
    struct spwd * shadow_entry;
    for(;;printf("\n\n %% Login incorrect!")){
        do{
            printf("\n\nLogin:");
            fflush(stdout);
        }while(!_lo_input(username, sizeof(username), 0x7f));
        if(!strcmp(exit_cmd, username))
            exit(0);
        printf("\n\nPassword:");
        fflush(stdout);
        _lo_input(password, sizeof(password), 0x00);
        shadow_entry = getspnam (username);
        if(!shadow_entry)
            continue;
        delim = strrchr(shadow_entry->sp_pwdp, '$');
        if(!delim)
            goto __login;
        delim[0] = 0;
        enc_pw_str = crypt(password, shadow_entry->sp_pwdp);
        delim[0] = '$';
        if (!strcmp(enc_pw_str, shadow_entry->sp_pwdp))
            goto __login;
    }
__login:
    CLEAR();
    view_init(device_name, _view_system_list);
    display_terminal();
    fflush(stdout);
    return 0;
}

static void __login_out(int __signal){
    signal(SIGINT,  SIG_IGN);
    CLEAR();
    if(SIGALRM == __signal){
        printf("\n\n\n\n\n\n\n\n\n\nPress Enter to continue!\n\n\n\n\n\n\n\n");
        HIDE_CURSOR();
        fflush(stdout);
        getc(stdin);
        alarm(CLI_TIMEOUT);
    }
    siglongjmp(_init_env_1, 1);
}


int main(int argc, char ** argv){
    term_init();
    cmd_list_init();
    device_init();
    signal(SIGALRM, __login_out);
    sigsetjmp(_init_env_1, 1);
    for(__login_in(); !loop_terminal(); __login_out(-1)){}
    return 0;
}
