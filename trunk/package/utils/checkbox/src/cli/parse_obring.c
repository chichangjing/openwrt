#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "parse_obring.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/ob_ring.h"
#include "../ubus/device.h"

int parse_display_ring(char* argv){
    char * ring_id;
    obring_get_general();
    if((ring_id=strtok(argv, " "))){
        for(;ring_id;ring_id=strtok(NULL, " ")){
            if(!is_csu32(ring_id)){
                print_row(64);
                printf(" %% %s is not a valid ring name(1~65535)!\n", ring_id);
                return CB_FAIL;
            }else{
                dev_ring_list(str2csu32(ring_id));
            }
        }
    }else{
        dev_ring_list(0);
    }
    if(global_ring->count)
        print_row(64);
    printf(" %% %s is %s, %s: %d\n", "ob-ring", jrpp_mode[global_ring->mode],"ring total", global_ring->count);
    return CB_SUCCESS;
}

int parse_clear_ring(char* argv){
    char * ring_id;
    obring_get_general();
    if((ring_id=strtok(argv, " "))){
        for(;ring_id;ring_id=strtok(NULL, " ")){
            if(!is_csu32(ring_id)){
                printf(" %% %s is not a valid ring name(1~65535)!\n", ring_id);
                return CB_FAIL;
            }else{
                obring_get_general();
                dev_ring_clear(str2csu32(ring_id));
            }
        }
    }else{
        dev_ring_clear(0);
    }
    return CB_SUCCESS;
}

int parse_obring_enable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Enable the globale ob-ring function.\n");
        obring_set_status(i2a[_JRPP_MODE_ENABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char *)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_obring_disable(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        printf(" %% Enable the globale ob-ring function.\n");
        obring_set_status(i2a[_JRPP_MODE_DISABLE]);
        return CB_SUCCESS;
    }
    posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
    printf(" %% Give too many parameters!\n");
    return CB_FAIL;
}

int parse_obring_view_init(char* argv){
    char * extra;
    char * ring_id;
    if(!(ring_id = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a ring ID: <1~65535>!\n");
        return CB_FAIL;
    }
    if(!is_cs32(ring_id) || str2cs32(ring_id)<1 || str2cs32(ring_id)>65535){
        posi_err(strlen(_terminal_view->cmd_head) + (ring_id-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid ring ID: <1~65535>!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    obring_get_general();
    switch(global_ring->mode){
        case _JRPP_MODE_DISABLE:
            printf(" %% OB ring has been disabled!\n");
            return CB_FAIL;
            break;
        case _JRPP_MODE_ENABLE:
            break;
        default:
            printf(" %% OB ring mode error: %d!", global_ring->mode);
            return CB_FAIL;
            break;
    }
    if(_exsit_ring(str2cs32(ring_id))){
        printf(" %% Ring %s is exsit. Enetr ring view.\n", ring_id);
        goto _enter;
    }
    if(!(_exsit_ring(str2cs32(ring_id))) && 0x65535 == global_ring->count){
        printf(" %% Ring %s is not exsit. Can`t add more ring.\n", ring_id);
        return CB_FAIL;
    }
    printf(" %% Ring %s is not exsit. Add new ring %s.\n", ring_id, ring_id);
    obring_name(id2idx(global_ring->count+1), id2ring(str2cs32(ring_id)));
_enter:
    memset(device_gt, 0, sizeof(struct device_gt));
    device_gt->view_name.ring = str2csu32(ring_id);
    memset(_terminal_view->cmd_head, 0, sizeof(_terminal_view->cmd_head));
    sprintf(_terminal_view->cmd_head, "[%s%d]# ", cli_view_name_tbl[RING_VIEW], str2cs32(ring_id));
    _terminal_view->_cmd_list->cmd_num  =   _view_obring_list->cmd_num;
    _terminal_view->_cmd_list->_cmd_    =   _view_obring_list->_cmd_;
    _terminal_view->cursor  =   0;
    return CB_SUCCESS;
}

int parse_obring_view_clear(char* argv){
    char * extra;
    if((extra = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    obring_get_general();
    switch(global_ring->mode){
        case _JRPP_MODE_DISABLE:
            printf(" %% OB ring has been disabled!\n");
            return CB_FAIL;
            break;
        case _JRPP_MODE_ENABLE:
            break;
        default:
            printf(" %% OB ring mode error: %d!", global_ring->mode);
            return CB_FAIL;
            break;
    }
    if(!(ringid2idx(device_gt->view_name.ring))){
        printf(" %% Ring %d is not exsit!\n", device_gt->view_name.ring);
    }else{
        printf(" %% Clear Ring %d !\n", device_gt->view_name.ring);
        obring_delete(id2idx(ringid2idx(device_gt->view_name.ring)));
    }
    //printf(" %% Exit ring view.\n");
    memset(device_gt, 0, sizeof(struct device_gt));
    memset(_terminal_view, 0, sizeof(struct _terminal_view));
    view_init(device_name, _view_system_list);
    return CB_SUCCESS;
}

int parse_obring_set_enable(char* argv){
    char * extra;
    if((extra=strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
    }
    obring_get_general();
    switch(global_ring->mode){
        case _JRPP_MODE_DISABLE:
            printf(" %% OB ring has been disabled!\n");
            return CB_FAIL;
            break;
        case _JRPP_MODE_ENABLE:
            break;
        default:
            printf(" %% OB ring mode error: %d!", global_ring->mode);
            return CB_FAIL;
            break;
    }
    if(!(ringid2idx(device_gt->view_name.ring))){
        printf(" %% Ring %d is not exsit!\n", device_gt->view_name.ring);
        return CB_FAIL;
    }
    if((global_ring->dev_ring[(ringid2idx(device_gt->view_name.ring))-1].master
                    && UsedIn_ring(global_ring->dev_ring[(ringid2idx(device_gt->view_name.ring))-1].master->idx))
            ||(global_ring->dev_ring[(ringid2idx(device_gt->view_name.ring))-1].slave
                    && UsedIn_ring(global_ring->dev_ring[(ringid2idx(device_gt->view_name.ring))-1].slave->idx)))
    {
        return CB_FAIL;
    }
    printf(" %% Set ring %d enable.\n", device_gt->view_name.ring);
    obring_set_mode(id2idx(ringid2idx(device_gt->view_name.ring)), csu82str(_RING_MODE_ENABLE));
    return CB_SUCCESS;
}

int parse_obring_set_disable(char* argv){
    char * extra;
    if((extra=strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
    }
    obring_get_general();
    switch(global_ring->mode){
        case _JRPP_MODE_DISABLE:
            printf(" %% OB ring has been disabled!\n");
            return CB_FAIL;
            break;
        case _JRPP_MODE_ENABLE:
            break;
        default:
            printf(" %% OB ring mode error: %d!", global_ring->mode);
            return CB_FAIL;
            break;
    }
    if(!(ringid2idx(device_gt->view_name.ring))){
        printf(" %% Ring %d is not exsit!\n", device_gt->view_name.ring);
        return CB_FAIL;
    }else{
        printf(" %% Set ring %d disable.\n", device_gt->view_name.ring);
        obring_set_mode(id2idx(ringid2idx(device_gt->view_name.ring)), csu82str(_RING_MODE_DISABLE));
        return CB_SUCCESS;
    }
}

int parse_obring_set_priority(char* argv){
    char * extra;
    char * priori;
    if(!(priori = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please a valid ring priority!\n");
        return CB_FAIL;
    }
    if(str_idx(priori, ring_prio)<_RING_PRIO_LOW || str_idx(priori, ring_prio)>=_RING_PRIO_MAX){
        posi_err(strlen(_terminal_view->cmd_head) + (priori-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please a valid ring priority!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    obring_get_general();
    switch(global_ring->mode){
        case _JRPP_MODE_DISABLE:
            printf(" %% OB ring has been disabled!\n");
            return CB_FAIL;
            break;
        case _JRPP_MODE_ENABLE:
            break;
        default:
            printf(" %% OB ring mode error: %d!", global_ring->mode);
            return CB_FAIL;
            break;
    }
    if(!(ringid2idx(device_gt->view_name.ring))){
        printf(" %% Ring %d is not exsit!\n", device_gt->view_name.ring);
        return CB_FAIL;
    }else{
        printf(" %% Set ring %d priority %s.\n", device_gt->view_name.ring, priori);
        obring_set_priority(id2idx(ringid2idx(device_gt->view_name.ring)), \
                csu322str(str_idx(priori, ring_prio)));
        return CB_SUCCESS;
    }
    return CB_SUCCESS;
}
int parse_obring_set_primary_port(char* argv){
    char * extra;
    char * port;
    if(!(port = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please a valid port!\n");
        return CB_FAIL;
    }
    if(str_idx(port, port_name[2])<PORT_1 || str_idx(port, port_name[2])>=_PORT_MAX){
        posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please a valid ring port!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    obring_get_general();
    switch(global_ring->mode){
        case _JRPP_MODE_DISABLE:
            printf(" %% OB ring has been disabled!\n");
            return CB_FAIL;
            break;
        case _JRPP_MODE_ENABLE:
            break;
        default:
            printf(" %% OB ring mode error: %d!", global_ring->mode);
            return CB_FAIL;
            break;
    }
    if(!(ringid2idx(device_gt->view_name.ring))){
        printf(" %% Ring %d is not exsit!\n", device_gt->view_name.ring);
        return CB_FAIL;
    }
    if(_RING_MODE_ENABLE == global_ring->dev_ring[ringid2idx(device_gt->view_name.ring)-1].mode
            && UsedIn_ring(str_idx(port, port_name[2]))){
        return CB_FAIL;
    }
    if(global_ring->dev_ring[ringid2idx(device_gt->view_name.ring)-1].slave &&
            str_idx(port, port_name[2])== global_ring->dev_ring[ringid2idx(\
                    device_gt->view_name.ring)-1].slave->idx)
    {
        printf(" %% %s has been used as a slave port in ring %d! Can`t to be set as a master port!\n", \
                               port, global_ring->dev_ring[ringid2idx(device_gt->view_name.ring)-1].ring_id);
        return CB_FAIL;
    }
    printf(" %% Set ring %d master port %s.\n", device_gt->view_name.ring, port);
    obring_set_primary_port(id2idx(ringid2idx(device_gt->view_name.ring)), \
                csu322str(str_idx(port, port_name[2])));
    return CB_SUCCESS;
}

int parse_obring_set_second_port(char* argv){
    char * extra;
    char * port;
    if(!(port = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please a valid port!\n");
        return CB_FAIL;
    }
    if(str_idx(port, port_name[2])<PORT_1 || str_idx(port, port_name[2])>=_PORT_MAX){
        posi_err(strlen(_terminal_view->cmd_head) + (port-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please a valid ring port!\n");
        return CB_FAIL;
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    obring_get_general();
    switch(global_ring->mode){
        case _JRPP_MODE_DISABLE:
            printf(" %% OB ring has been disabled!\n");
            return CB_FAIL;
            break;
        case _JRPP_MODE_ENABLE:
            break;
        default:
            printf(" %% OB ring mode error: %d!", global_ring->mode);
            return CB_FAIL;
            break;
    }
    if(_RING_MODE_ENABLE == global_ring->dev_ring[ringid2idx(device_gt->view_name.ring)-1].mode
            && !(ringid2idx(device_gt->view_name.ring))){
        printf(" %% Ring %d is not exsit!\n", device_gt->view_name.ring);
        return CB_FAIL;
    }
    if(UsedIn_ring(str_idx(port, port_name[2]))){
        return CB_FAIL;
    }
    if(global_ring->dev_ring[ringid2idx(device_gt->view_name.ring)-1].master
            &&str_idx(port, port_name[2])== global_ring->dev_ring[ringid2idx(device_gt->view_name.ring)-1].master->idx)
    {
        printf(" %% %s has been used as a master port in ring %d! Can`t to be set as a slave port!\n", \
                               port, global_ring->dev_ring[ringid2idx(device_gt->view_name.ring)-1].ring_id);
        return CB_FAIL;
    }
    printf(" %% Set ring %d slave port %s.\n", device_gt->view_name.ring, port);
    obring_set_second_port(id2idx(ringid2idx(device_gt->view_name.ring)), \
                csu322str(str_idx(port, port_name[2])));
    return CB_SUCCESS;
}
