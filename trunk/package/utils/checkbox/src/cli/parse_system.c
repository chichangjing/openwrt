#include <pwd.h>
#include <shadow.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/system_info.h"
#include "../ubus/netconfig.h"

int parse_display_history_cmd(char* argv){
    char * extra = strtok(argv, " ");
    if(NULL == extra){
        display_history_cmd();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_display_system_info(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		sysinfo_get_all();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_display_network(char* argv){
	char *extra;
	extra = strtok(argv, " ");
	if(NULL == extra){
		netconfig_get_ip_mode();
		switch(device_gt->current_mode){
			case _IP_MODE_STATIC:
				netconfig_get_all();
				return CB_SUCCESS;
				break;
			case _IP_MODE_DHCP:
				printf(" %% Switch ip Mode is %s now!\n", ip_mode[1][device_gt->current_mode]);
				return CB_FAIL;
				break;
			default:
				printf(" %% Switch ip Mode error: [%d]!\n", device_gt->current_mode);
				return CB_FAIL;
		}
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_sysinfo_set_location(char* argv){
	if(strlen(argv) < 3){
		posi_err(strlen(_terminal_view->cmd_head) + (argv-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Too short device location! Must between 3 and 64 characters!\n");
		return CB_FAIL;
	}else if(strlen(argv) > 64){
		posi_err(strlen(_terminal_view->cmd_head) + (argv-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Too long device location! Must between 3 and 64 characters!\n");
		return CB_FAIL;
	}else{
		sysinfo_set_location(argv);
		return CB_SUCCESS;
	}
}

int parse_sysinfo_set_ip_mode(char* argv){
        return CB_SUCCESS;
}

int parse_sysinfo_set_name(char* argv){
	if(strlen(argv) < 3){
		posi_err(strlen(_terminal_view->cmd_head) + (argv-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Too short device description!\n");
		return CB_FAIL;
	}else if(strlen(argv) > 64){
		posi_err(strlen(_terminal_view->cmd_head) + (argv-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Too long device description!\n");
		return CB_FAIL;
	}else{
		sysinfo_set_name(argv);
		return CB_SUCCESS;
	}
}

int parse_ping(char* argv){
    char * ip_address;
    char * extra;
    char cmd[sizeof("ping 255.255.255.255 -c 5")+8];
    if(!(ip_address = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen(in_put_buf));
        printf(" %% Please give a ip address!\n");
        return CB_FAIL;
    }
    if(CB_FAIL == ip_check(ip_address)){
        posi_err(strlen(_terminal_view->cmd_head) + (ip_address-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give valid ip address!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    sprintf(cmd, "ping %s -c 5", ip_address);
    system(cmd);
    return CB_SUCCESS;
}

int parse_netconfig_set_static_ip(char* argv){
	char *ip_address;
	char *netmask;
	char *gateway;
	char *extra;
	int _argv_len= strlen((char*)(&in_put_buf));
	char ip_cf[sizeof("ifconfig br-lan 255.255.255.255 netmask 255.255.255.255 >/dev/null 2>&1")+8];
	char route_cf[sizeof("route del default>/dev/null;route add default gw 255.255.255.255 >/dev/null 2>&1")+8];
	ip_address = strtok(argv, " ");
	netmask = strtok(NULL, " ");
	gateway = strtok(NULL, " ");
	extra = strtok(NULL, " ");
	if(NULL == ip_address){
		posi_err(strlen(_terminal_view->cmd_head) + _argv_len);
		printf(" %% Please give a ip address!\n");
		return CB_FAIL;
	}
	if(CB_FAIL == ip_check(ip_address)){
		posi_err(strlen(_terminal_view->cmd_head) + (ip_address-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give valid ip address!\n");
		return CB_FAIL;
	}
	if(NULL == netmask){
		posi_err(strlen(_terminal_view->cmd_head) + _argv_len);
		printf(" %% Please give a netmask!\n");
		return CB_FAIL;
	}
	if(CB_FAIL == ip_check(netmask)){
		posi_err(strlen(_terminal_view->cmd_head) + (netmask-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give valid netmask!\n");
		return CB_FAIL;
	}
	if(NULL == gateway){
		posi_err(strlen(_terminal_view->cmd_head) + _argv_len);
		printf(" %% Please input a gateway!\n");
		return CB_FAIL;
	}
	if(CB_FAIL == ip_check(gateway)){
		posi_err(strlen(_terminal_view->cmd_head) + (gateway-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give valid gateway!\n");
		return CB_FAIL;
	}
	if(NULL != extra){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	device_gt->current_mode = -1;
	netconfig_get_ip_mode();
	switch (device_gt->current_mode){
		case _IP_MODE_STATIC:
			printf(" %% Terminal set switch ip %s netmask %s gateway %s.\n",ip_address, netmask, gateway);
			netconfig_set_static_ip(ip_address, netmask, gateway);
			sprintf(ip_cf, "ifconfig br-lan %s netmask %s >/dev/null 2>&1", ip_address, netmask);
			sprintf(route_cf, "route del default>/dev/null;route add default gw %s >/dev/null 2>&1", gateway);
			system((char*)ip_cf);
			system((char*)route_cf);
			return CB_SUCCESS;
			break;
		case _IP_MODE_DHCP:
			printf(" %% Switch ip Mode is %s now!\n", ip_mode[1][device_gt->current_mode]);
			return CB_FAIL;
			break;
		default:
			printf(" %% Switch ip Mode error: [%d]!\n", device_gt->current_mode);
			return CB_FAIL;
			break;
	}
	return CB_SUCCESS;
}

int parse_reboot(char* argv){
    char *extra;
    char buf[16];
    if(!(extra = strtok(argv, " "))){
        printf(" System will be reboot, continue?[Y/N]:");
        fflush(stdout);
        for(;;printf(" Only accept \"Y\" or \"N\" to continue:"),fflush(stdout)){
            _lo_input(buf,sizeof(buf), 0x7f);
            if(!strcmp(buf, "Y")||!strcmp(buf, "y")||!strcmp(buf, "yes")||!strcmp(buf, "YES")){
                printf(" System reboot......\n");
                system("reboot");
                while((getc(stdin))){}
                return CB_SUCCESS;
            }else if(!strcmp(buf, "N")||!strcmp(buf, "n") || !strcmp(buf, "NO")||!strcmp(buf, "no")){
                return CB_SUCCESS;
            }
        }
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    return CB_SUCCESS;
}


int parse_save_config(char* argv){
    char *extra;
    char buf[16];
    if(!(extra = strtok(argv, " "))){
        printf(" Current config will write to device, continue?[Y/N]:");
        fflush(stdout);
        for(;;printf(" Only accept \"Y\" or \"N\" to continue:"),fflush(stdout)){
            _lo_input(buf,sizeof(buf), 0x7f);
            if(!strcmp(buf, "Y")||!strcmp(buf, "y")){
                printf(" Save configuration file......\n");
                save_config();
                return CB_SUCCESS;
            }else if(!strcmp(buf, "N")||!strcmp(buf, "n")){
                return CB_SUCCESS;
            }
        }
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    return CB_SUCCESS;
}
#if 0
static void _password(char * password, char * _pw_scry){
#if 0
    struct passwd *pwd;
    pwd = getpwnam("root");
    if(pwd ==NULL)
        printf("no username found.n");
    else{
        printf("passwd: %sn", pwd->pw_passwd);
        if(strcmp(pwd->pw_passwd, "x") == 0){
            printf("shadow used.n");
#endif
            struct spwd *shd= getspnam("root");
            if(shd != NULL){
                char salt[16];
                int i=0,j=0;
                while(shd->sp_pwdp[i]!='\0'){
                    salt[i]=shd->sp_pwdp[i];
                    if(salt[i]=='$'){
                        j++;
                        if(j==3){
                            salt[i+1]='\0';
                            break;
                        }
                    }
                    i++;
                }
                strcpy(_pw_scry,crypt(password, salt));
            }
#if 0
        }
    }
#endif
}

// root:$1$neMLyjnt$KFC9X7hp39OqZkXO2grFC0:17087:0:99999:7:::

static int _change_shadow(char * _pw_scry){
    FILE * fp;
    char scrypt[80];
    memset(scrypt, 0, sizeof(scrypt));
    if(!(fp=fopen("/etc/shadow","rt+")))
        return CB_FAIL;
    fgets(scrypt, sizeof(scrypt), fp);
    if(strlen(scrypt) == strlen("root::0:0:99999:7:::")){
        memcpy(scrypt+strlen("root:")+strlen("$1$rMQY5e/9$ZG8jDDPFCPCw9G/mFpTq9/"), scrypt+strlen("root:"), strlen(":0:0:99999:7:::"));
        memcpy(scrypt+strlen("root:"), _pw_scry, strlen("$1$rMQY5e/9$ZG8jDDPFCPCw9G/mFpTq9/"));
    }else{
        memcpy(scrypt+strlen("root:"), _pw_scry, strlen("$1$rMQY5e/9$ZG8jDDPFCPCw9G/mFpTq9/"));
    }
    fseek(fp,0,SEEK_SET);
    fputs(scrypt, fp);
    fclose(fp);
    return CB_SUCCESS;
}
#endif
int parse_pass_word(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        system("passwd");
        return CB_SUCCESS;
#if 0
        char * delim;
        char * enc_pw_str;
        char old_password[_PWD_LEN+sizeof('\0')];
        char new_password_1[_PWD_LEN+sizeof('\0')];
        char new_password_2[_PWD_LEN+sizeof('\0')];
        char _pw_scry[80];
        struct spwd *shadow_entry;
        memset(_pw_scry, 0, sizeof(_pw_scry));
        printf("Enter your old password:");
        fflush(stdout);
        _lo_input(old_password,sizeof(old_password), 0x00);

        shadow_entry = getspnam ("root");
        if(!shadow_entry)
            return CB_FAIL;
        delim = strrchr(shadow_entry->sp_pwdp, '$');
        if(delim){
            delim[0] = 0;
            enc_pw_str = crypt(old_password, shadow_entry->sp_pwdp);
            delim[0] = '$';
            if (strcmp(enc_pw_str, shadow_entry->sp_pwdp)){
                printf(" %% Password error!");
                return CB_FAIL;
            }
        }
        printf("Enter your new password:");
        fflush(stdout);
        _lo_input(new_password_1,sizeof(new_password_1), 0x00);

        printf("Retry your new password:");
        fflush(stdout);
        _lo_input(new_password_2,sizeof(new_password_2),0x00);

        if(!strcmp(new_password_1, new_password_2)){
            _password(new_password_1, _pw_scry);
            if(!_change_shadow(_pw_scry)){
                printf(" %% Password changed!");
                return CB_SUCCESS;
            }
        }
        printf(" %% Password didn`t changed!");
        return CB_FAIL;

#endif
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    return CB_SUCCESS;
}
