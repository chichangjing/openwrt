#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/fdb.h"
#include "../caselib/cs_type.h"

int parse_mac_aging_time(char* argv){
    char * time;
    char * extra;
    if(!(time = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a aging time(15~3600)!\n");
        return CB_FAIL;
    }
    if(!is_cs32(time) || !(str2cs32(time) >= 15) || !(str2cs32(time) <= 3600)){
        posi_err(strlen(_terminal_view->cmd_head) + (time-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid aging time(15~3600)!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    printf(" %% Set mac aging time: %s.\n", time);
    set_mac_aging_time(time);
    return CB_SUCCESS;
}

int parse_if_fdb_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s port security enable.\n", port_name_to_show(device_gt->current_idx));
        set_fdb_mode(port_idx_to_port(device_gt->current_idx), i2a[_IF_FDB_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_if_fdb_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s port security disable.\n", port_name_to_show(device_gt->current_idx));
        set_fdb_mode(port_idx_to_port(device_gt->current_idx), i2a[_IF_FDB_MODE_DISABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_if_fdb_mac(char* argv){
    char * mac;
    char * extra;
    int i=1;
    char _idx[16];
    if(!(mac = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a mac address!\n");
        return CB_FAIL;
    }
    if(!is_mac(mac)){
        posi_err(strlen(_terminal_view->cmd_head) + (mac-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid mac address!\n");
        return CB_FAIL;
    }
    if((mac[1]-'0')%2){
        posi_err(strlen(_terminal_view->cmd_head) + (mac-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Can`t set a multicast mac address!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    get_fdb_mode_d(port_idx_to_port(device_gt->current_idx));
    switch(device_gt->current_mode){
    case _IF_FDB_MODE_DISABLE:
        printf(" %% Port security is disable, can`t to set mac address!\n");
        return CB_FAIL;
        break;
    case _IF_FDB_MODE_ENABLE:
        get_fdb_num_d();
        if(device_gt->idx_num>=65535)
            printf(" %% Can`t to set more port security mac address!\n");
        memset(device_gt->tmp, 0, sizeof(device_gt->tmp));
        sprintf(device_gt->tmp, "%s", mac2str(str2mac(mac), _base_mac_3));
        device_gt->current_mode=0;
        for(;!(device_gt->current_mode) && i<=device_gt->idx_num;i++){
            memset(_idx, 0, sizeof(_idx));
            sprintf(_idx, "idx-%d", i);
            repeat_fdb(_idx);
        }
        if(device_gt->current_mode){
            printf(" %% Port security mac address has been setted in this port!\n");
            return CB_FAIL;
        }else{
            printf(" %% Add security mac address: %s.\n", mac);
            sprintf(_idx, "idx-%d", i);
            set_fdb_mac(_idx, port_name_to_num(port_idx_to_port(device_gt->current_idx)),\
                    mac2str(str2mac(mac), _base_mac_3));
            return CB_SUCCESS;
        }
        break;
    default:
        printf(" %% Port security mode error: %d!\n", device_gt->current_mode);
        return CB_FAIL;
        break;
    }
}
