#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/rate_limit.h"
#include "../ubus/ubus_call.h"

int parse_display_rate_limit(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        rate_limit_get_general();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck0_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-0 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck0_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_ENABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck1_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-1 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck1_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_ENABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck2_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-2 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck2_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_ENABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck3_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-3 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck3_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_ENABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck4_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-4 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck4_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_ENABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}


int parse_in_buck0_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-0 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck0_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_DISABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck1_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-1 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck1_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_DISABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck2_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-2 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck2_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_DISABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck3_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-3 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck3_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_DISABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck4_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s bucket-4 inbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        ingress_buck4_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_DISABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_in_buck0_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a bucket-0 inbound rate limit value: 64~100000 Kbps!\n");
        return CB_FAIL;
    }
    if(64>_atoi_s(value) || 100000<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid bucket-0 inbound rate limit value:\"%d\"(64~100000 Kbps)!\n", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck0_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-0 inbound rate limit mode is disable, can`t to set bucket-0 inbound rate limit value.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-0 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s bucket-0 inbound rate limit value: \"%s\"!\n", port_name_to_show(device_gt->current_idx), value);
    ingress_buck0_set_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}

int parse_in_buck1_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a bucket-1 inbound rate limit value: 64~100000 Kbps!\n");
        return CB_FAIL;
    }
    if(64>_atoi_s(value) || 100000<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid bucket-1 inbound rate limit value:\"%d\"(64~100000 Kbps)!\n", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck1_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-1 inbound rate limit mode is disable, can`t to set bucket-1 inbound rate limit value.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-1 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s bucket-1 inbound rate limit value: \"%s\"!\n", port_name_to_show(device_gt->current_idx), value);
    ingress_buck1_set_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}
int parse_in_buck2_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a bucket-2 inbound rate limit value: 64~100000 Kbps!\n");
        return CB_FAIL;
    }
    if(64>_atoi_s(value) || 100000<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid bucket-2 inbound rate limit value:\"%d\"(64~100000 Kbps)!\n", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck2_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-2 inbound rate limit mode is disable, can`t to set bucket-2 inbound rate limit value.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-2 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s bucket-2 inbound rate limit value: \"%s\"!\n", port_name_to_show(device_gt->current_idx), value);
    ingress_buck2_set_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}
int parse_in_buck3_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a bucket-3 inbound rate limit value: 64~100000 Kbps!\n");
        return CB_FAIL;
    }
    if(64>_atoi_s(value) || 100000<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid bucket-3 inbound rate limit value:\"%d\"(64~100000 Kbps)!\n", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck3_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-3 inbound rate limit mode is disable, can`t to set bucket-3 inbound rate limit value.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-3 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s bucket-3 inbound rate limit value: \"%s\"!\n", port_name_to_show(device_gt->current_idx), value);
    ingress_buck3_set_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}
int parse_in_buck4_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a bucket-4 inbound rate limit value: 64~100000 Kbps!\n");
        return CB_FAIL;
    }
    if(64>_atoi_s(value) || 100000<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid bucket-4 inbound rate limit value:\"%d\"(64~100000 Kbps)!\n", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck4_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-4 inbound rate limit mode is disable, can`t to set bucket-4 inbound rate limit value.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-4 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s bucket-4 inbound rate limit value: \"%s\"!\n", port_name_to_show(device_gt->current_idx), value);
    ingress_buck4_set_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}

int parse_in_buck0_frame(char* argv){
    char * frame;
    char * extra;
    int i, j, mark, other_bucket=0;
    int mask=0;
    char frame_mask[4];
    ingress_buck0_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-0 inbound rate limit mode is disable, can`t to set bucket-0 inbound rate limit frame type.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-0 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
        if(_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME == i){
            frame = strtok(argv, " ");
            if(NULL == frame){
                posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
                printf(" %% Please give a rate limit frame type!\n");
                return CB_FAIL;
            }
        }else{
            frame = strtok(NULL, " ");
        }
        if(NULL != frame){
            for(j=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME, mark=0; j<_RATE_LIMITING_TYPE_MAX; j++){
                if(0 == strcmp(frame, rate_limit_frame_type[j])){
                    mask = mask | (1<<(j-1));
                    mark = 1;
                    break;
                }
            }
            if(0 == mark){
                posi_err(strlen(_terminal_view->cmd_head) + (frame-(char*)&in_put_buf)/sizeof(char));
                printf(" %% Please give a valid rate limit frame type!\n");
                return CB_FAIL;
            }
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck1_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck2_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck3_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck4_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    other_bucket = other_bucket & mask;
    if(other_bucket){
        printf(" %% Frame type had been set in other bucket:");
        for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
            if(other_bucket & 1<<(i-1))
                printf(" %s", rate_limit_frame_type[i]);
        }
        printf(".\n %% Can`t set this frame type again!\n");
        return CB_FAIL;
    }
    sprintf(frame_mask, "%2d%c", mask, '\0');
    printf(" %% Set %s bucket-0 inbound rate limit frame type: \"%s\"!\n", port_name_to_show(device_gt->current_idx), frame_mask);
    ingress_buck0_set_frame(port_idx_to_port(device_gt->current_idx), frame_mask);
    return CB_SUCCESS;
}

int parse_in_buck1_frame(char* argv){
    char * frame;
    char * extra;
    int i, j, mark, other_bucket=0;
    int mask=0;
    char frame_mask[4];
    ingress_buck1_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-1 inbound rate limit mode is disable, can`t to set bucket-1 inbound rate limit frame type.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-1 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
        if(_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME == i){
            frame = strtok(argv, " ");
            if(NULL == frame){
                posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
                printf(" %% Please give a rate limit frame type!\n");
                return CB_FAIL;
            }
        }else{
            frame = strtok(NULL, " ");
        }
        if(NULL != frame){
            for(j=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME, mark=0; j<_RATE_LIMITING_TYPE_MAX; j++){
                if(0 == strcmp(frame, rate_limit_frame_type[j])){
                    mask = mask | (1<<(j-1));
                    mark = 1;
                    break;
                }
            }
            if(0 == mark){
                posi_err(strlen(_terminal_view->cmd_head) + (frame-(char*)&in_put_buf)/sizeof(char));
                printf(" %% Please give a valid rate limit frame type!\n");
                return CB_FAIL;
            }
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck0_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck2_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck3_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck4_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    other_bucket = other_bucket & mask;
    if(other_bucket){
        printf(" %% Frame type had been set in other bucket:");
        for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
            if(other_bucket & 1<<(i-1))
                printf(" %s", rate_limit_frame_type[i]);
        }
        printf(".\n %% Can`t set this frame type again!\n");
        return CB_FAIL;
    }
    sprintf(frame_mask, "%2d%c", mask, '\0');
    printf(" %% Set %s bucket-1 inbound rate limit frame type: \"%s\"!\n", port_name_to_show(device_gt->current_idx), frame_mask);
    ingress_buck0_set_frame(port_idx_to_port(device_gt->current_idx), frame_mask);
    return CB_SUCCESS;
}
int parse_in_buck2_frame(char* argv){
    char * frame;
    char * extra;
    int i, j, mark, other_bucket=0;
    int mask=0;
    char frame_mask[4];
    ingress_buck2_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-2 inbound rate limit mode is disable, can`t to set bucket-2 inbound rate limit frame type.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-2 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
        if(_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME == i){
            frame = strtok(argv, " ");
            if(NULL == frame){
                posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
                printf(" %% Please give a rate limit frame type!\n");
                return CB_FAIL;
            }
        }else{
            frame = strtok(NULL, " ");
        }
        if(NULL != frame){
            for(j=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME, mark=0; j<_RATE_LIMITING_TYPE_MAX; j++){
                if(0 == strcmp(frame, rate_limit_frame_type[j])){
                    mask = mask | (1<<(j-1));
                    mark = 1;
                    break;
                }
            }
            if(0 == mark){
                posi_err(strlen(_terminal_view->cmd_head) + (frame-(char*)&in_put_buf)/sizeof(char));
                printf(" %% Please give a valid rate limit frame type!\n");
                return CB_FAIL;
            }
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck1_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck0_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck3_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck4_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    other_bucket = other_bucket & mask;
    if(other_bucket){
        printf(" %% Frame type had been set in other bucket:");
        for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
            if(other_bucket & 1<<(i-1))
                printf(" %s", rate_limit_frame_type[i]);
        }
        printf(".\n %% Can`t set this frame type again!\n");
        return CB_FAIL;
    }
    sprintf(frame_mask, "%2d%c", mask, '\0');
    printf(" %% Set %s bucket-2 inbound rate limit frame type: \"%s\"!\n", port_name_to_show(device_gt->current_idx), frame_mask);
    ingress_buck2_set_frame(port_idx_to_port(device_gt->current_idx), frame_mask);
    return CB_SUCCESS;
}
int parse_in_buck3_frame(char* argv){
    char * frame;
    char * extra;
    int i, j, mark, other_bucket=0;
    int mask=0;
    char frame_mask[4];
    ingress_buck3_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-3 inbound rate limit mode is disable, can`t to set bucket-3 inbound rate limit frame type.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-3 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
        if(_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME == i){
            frame = strtok(argv, " ");
            if(NULL == frame){
                posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
                printf(" %% Please give a rate limit frame type!\n");
                return CB_FAIL;
            }
        }else{
            frame = strtok(NULL, " ");
        }
        if(NULL != frame){
            for(j=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME, mark=0; j<_RATE_LIMITING_TYPE_MAX; j++){
                if(0 == strcmp(frame, rate_limit_frame_type[j])){
                    mask = mask | (1<<(j-1));
                    mark = 1;
                    break;
                }
            }
            if(0 == mark){
                posi_err(strlen(_terminal_view->cmd_head) + (frame-(char*)&in_put_buf)/sizeof(char));
                printf(" %% Please give a valid rate limit frame type!\n");
                return CB_FAIL;
            }
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck1_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck2_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck0_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck4_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    other_bucket = other_bucket & mask;
    if(other_bucket){
        printf(" %% Frame type had been set in other bucket:");
        for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
            if(other_bucket & 1<<(i-1))
                printf(" %s", rate_limit_frame_type[i]);
        }
        printf(".\n %% Can`t set this frame type again!\n");
        return CB_FAIL;
    }
    sprintf(frame_mask, "%2d%c", mask, '\0');
    printf(" %% Set %s bucket-3 inbound rate limit frame type: \"%s\"!\n", port_name_to_show(device_gt->current_idx), frame_mask);
    ingress_buck3_set_frame(port_idx_to_port(device_gt->current_idx), frame_mask);
    return CB_SUCCESS;
}
int parse_in_buck4_frame(char* argv){
    char * frame;
    char * extra;
    int i, j, mark, other_bucket=0;
    int mask=0;
    char frame_mask[4];
    ingress_buck4_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s bucket-4 inbound rate limit mode is disable, can`t to set bucket-4 inbound rate limit frame type.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid bucket-4 inbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
        if(_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME == i){
            frame = strtok(argv, " ");
            if(NULL == frame){
                posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
                printf(" %% Please give a rate limit frame type!\n");
                return CB_FAIL;
            }
        }else{
            frame = strtok(NULL, " ");
        }
        if(NULL != frame){
            for(j=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME, mark=0; j<_RATE_LIMITING_TYPE_MAX; j++){
                if(0 == strcmp(frame, rate_limit_frame_type[j])){
                    mask = mask | (1<<(j-1));
                    mark = 1;
                    break;
                }
            }
            if(0 == mark){
                posi_err(strlen(_terminal_view->cmd_head) + (frame-(char*)&in_put_buf)/sizeof(char));
                printf(" %% Please give a valid rate limit frame type!\n");
                return CB_FAIL;
            }
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    ingress_buck1_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck2_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck3_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    ingress_buck0_get_frame(port_idx_to_port(device_gt->current_idx));
    other_bucket = other_bucket | device_gt->vlan_port_list;
    other_bucket = other_bucket & mask;
    if(other_bucket){
        printf(" %% Frame type had been set in other bucket:");
        for(i=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME; i<_RATE_LIMITING_TYPE_MAX; i++){
            if(other_bucket & 1<<(i-1))
                printf(" %s", rate_limit_frame_type[i]);
        }
        printf(".\n %% Can`t set this frame type again!\n");
        return CB_FAIL;
    }
    sprintf(frame_mask, "%2d%c", mask, '\0');
    printf(" %% Set %s bucket-4 inbound rate limit frame type: \"%s\"!\n", port_name_to_show(device_gt->current_idx), frame_mask);
    ingress_buck4_set_frame(port_idx_to_port(device_gt->current_idx), frame_mask);
    return CB_SUCCESS;
}


int parse_out_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s outbound rate limit enable.\n", port_name_to_show(device_gt->current_idx));
        outgress_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_ENABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_out_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Set %s outbound rate limit disable.\n", port_name_to_show(device_gt->current_idx));
        outgress_set_status(port_idx_to_port(device_gt->current_idx), i2a[_RATE_LIMITING_MODE_DISABLE]);
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_out_value(char* argv){
    char * value;
    char * extra;
    if(!(value = strtok(argv, " "))){
        printf(" %% Give a outbound rate limit value: 64~100000 Kbps!\n");
        return CB_FAIL;
    }
    if(64>_atoi_s(value) || 100000<_atoi_s(value)){
        posi_err(strlen(_terminal_view->cmd_head) + (value-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Invalid outbound rate limit value:\"%d\"(64~100000 Kbps)!\n", _atoi_s(value));
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    outgress_get_status(port_idx_to_port(device_gt->current_idx));
    switch (device_gt->current_mode){
        case _RATE_LIMITING_MODE_ENABLE:
            break;
        case _RATE_LIMITING_MODE_DISABLE:
            printf(" %% %s outbound rate limit mode is disable, can`t to set outbound rate limit value.\n", port_name_to_show(device_gt->current_idx));
            return CB_FAIL;
            break;
        default:
            printf(" %% Invalid outbound rate limit mode: %d.\n", device_gt->current_mode);
            return CB_FAIL;
            break;
    }
    printf(" %% Set %s outbound rate limit value: \"%s\"!\n", port_name_to_show(device_gt->current_idx), value);
    outgress_set_value(port_idx_to_port(device_gt->current_idx), value);
    return CB_SUCCESS;
}
