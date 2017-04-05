#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "cli_parse.h"
#include "cli.h"
#include "../ubus/device_info.h"
#include "../ubus/ubus_call.h"
#include "../ubus/vlan.h"
#include "../ubus/device.h"
#include "../caselib/cs_type.h"

int parse_vlan_set_mode(char* argv){
	char * mode;
	char * extra;
	if(!(mode = strtok(argv, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give a Vlan mode!\n");
		return CB_FAIL;
	}
	if(!str_idx(mode, vlan_mode_tbl[0])){
        posi_err(strlen(_terminal_view->cmd_head) + (mode-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Please give a valid Vlan mode!\n");
        return CB_FAIL;
	}
	if((extra = strtok(NULL, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	printf(" %% Vlan set mode: %s.\n", mode);
	vlan_set_mode(csu322str(str_idx(mode, vlan_mode_tbl[0])));
	return CB_SUCCESS;
}

int parse_vlan_get_general(char* argv){
	char *extra;
	if(!(extra = strtok(argv, " "))){
	    vlan_get_general();
	    vlan_list();
		return CB_SUCCESS;
	}else{
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
}

int parse_isolate_get_general(char* argv){
    char *extra;
    if(!(extra = strtok(argv, " "))){
        vlan_get_general();
        isolate_list();
        return CB_SUCCESS;
    }else{
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
}

int parse_Q8021_vlan_view_init(char * argv){
	char * vlan_id;
	char * extra;
	if(!(vlan_id = strtok(argv, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
		printf(" %% Please give your vlan ID!\n");
		return CB_FAIL;
	}
	if(!is_csu32(vlan_id) || 1 >= str2csu32(vlan_id) || 4096<str2csu32(vlan_id)){
		posi_err(strlen(_terminal_view->cmd_head) + (vlan_id-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Please give a valid vlan id(2~4096)!\n");
		return CB_FAIL;
	}
	if((extra = strtok(NULL, " "))){
		posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
		printf(" %% Give too many parameters!\n");
		return CB_FAIL;
	}
	vlan_get_general();
	switch(global_vlan->mode){
        case VLAN_MODE_8021D:
            break;
        case VLAN_MODE_PORTBASED:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
	}
	if(vlan2idx(str2csu32(vlan_id))){
	    printf(" %% Vlan %s is exsit. Enter vlan view.\n", vlan_id);
	}else{
	    printf(" %% Vlan %s is not exsit. Add new vlan %s.\n", vlan_id, vlan_id);
	    Q8021_vlan_set_id(id2idx(global_vlan->count+1), csu322str(str2csu32(vlan_id)));
	}
    memset(device_gt, 0, sizeof(struct device_gt));
    device_gt->view_name.vlan = str2csu32(vlan_id);
    memset(_terminal_view->cmd_head, 0, sizeof(_terminal_view->cmd_head));
    sprintf(_terminal_view->cmd_head, "[%s%d]# ", cli_view_name_tbl[VLAN_VIEW], str2csu32(vlan_id));
    _terminal_view->_cmd_list->cmd_num  =   _view_vlan_list->cmd_num;
    _terminal_view->_cmd_list->_cmd_    =   _view_vlan_list->_cmd_;
    _terminal_view->cursor  =   0;

	return 0;
}

int parse_Q8021_vlan_tag_port_set(char* argv){
    char * extra;
    char * member;
    char port_list[10];
    char tag_list[10];
    int j=0;
    int untag_mask = 0;
    int tag_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            tag_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_8021D:
            break;
        case VLAN_MODE_PORTBASED:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    untag_mask = ~tag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list;
	sprintf(tag_list, "%02x-%02x%c", tag_mask>>8, 0x00ff & tag_mask, '\0');
	sprintf(port_list, "%02x-%02x%c", (tag_mask|untag_mask)>>8, 0x00ff&(tag_mask|untag_mask), '\0');
	printf(" %% Vlan %d set tag port: %04x, set untag port: %04x.\n", device_gt->view_name.vlan, tag_mask, untag_mask);
	Q8021_vlan_set_port_list(id2idx(vlan2idx(device_gt->view_name.vlan)), port_list, tag_list);
	return CB_SUCCESS;
}

int parse_Q8021_vlan_tag_port_add(char* argv){
    char * extra;
    char * member;
    char port_list[10];
    char tag_list[10];
    int j=0;
    int untag_mask = 0;
    int tag_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            tag_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_8021D:
            break;
        case VLAN_MODE_PORTBASED:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    tag_mask|=global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].tag_list;
    untag_mask = ~tag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list;
    sprintf(tag_list, "%02x-%02x%c", tag_mask>>8, 0x00ff & tag_mask, '\0');
    sprintf(port_list, "%02x-%02x%c", (tag_mask|untag_mask)>>8, 0x00ff&(tag_mask|untag_mask), '\0');
    printf(" %% Vlan %d set tag port: %04x, set untag port: %04x.\n", device_gt->view_name.vlan, tag_mask, untag_mask);
    Q8021_vlan_set_port_list(id2idx(vlan2idx(device_gt->view_name.vlan)), port_list, tag_list);
	return CB_SUCCESS;
}

int parse_Q8021_vlan_tag_port_del(char* argv){
    char * extra;
    char * member;
    char port_list[10];
    char tag_list[10];
    int j=0;
    int untag_mask = 0;
    int tag_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            tag_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_8021D:
            break;
        case VLAN_MODE_PORTBASED:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    tag_mask=~tag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].tag_list;
    untag_mask = ~tag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list;
    sprintf(tag_list, "%02x-%02x%c", tag_mask>>8, 0x00ff & tag_mask, '\0');
    sprintf(port_list, "%02x-%02x%c", (tag_mask|untag_mask)>>8, 0x00ff&(tag_mask|untag_mask), '\0');
    printf(" %% Vlan %d set tag port: %04x, set untag port: %04x.\n", device_gt->view_name.vlan, tag_mask, untag_mask);
    Q8021_vlan_set_port_list(id2idx(vlan2idx(device_gt->view_name.vlan)), port_list, tag_list);
	return CB_SUCCESS;
}

int parse_Q8021_vlan_untag_port_set(char* argv){
    char * extra;
    char * member;
    char port_list[10];
    char tag_list[10];
    int j=0;
    int untag_mask = 0;
    int tag_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            untag_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_8021D:
            break;
        case VLAN_MODE_PORTBASED:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    if(~(global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list)
            & global_vlan->untag_list & untag_mask){
        untag_mask = ~(global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list)
                    & global_vlan->untag_list & untag_mask;
        printf(" %% Untag port has been set in other vlan: ");
        int i;
        for(i=PORT_1;i<_PORT_MAX;i++){
            if(untag_mask & (1<<(16-i))){
                printf("%8s", id2port(i));
            }
        }
        printf("\n %% Check config to continue!\n");
        return CB_FAIL;
    }
    tag_mask = ~untag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].tag_list;
    sprintf(tag_list, "%02x-%02x%c", tag_mask>>8, 0x00ff & tag_mask, '\0');
    sprintf(port_list, "%02x-%02x%c", (tag_mask|untag_mask)>>8, 0x00ff&(tag_mask|untag_mask), '\0');
    printf(" %% Vlan %d set tag port: %04x, set untag port: %04x.\n", device_gt->view_name.vlan, tag_mask, untag_mask);
    Q8021_vlan_set_port_list(id2idx(vlan2idx(device_gt->view_name.vlan)), port_list, tag_list);
    return CB_SUCCESS;
}

int parse_Q8021_vlan_untag_port_add(char* argv){
    char * extra;
    char * member;
    char port_list[10];
    char tag_list[10];
    int j=0;
    int untag_mask = 0;
    int tag_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            untag_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_8021D:
            break;
        case VLAN_MODE_PORTBASED:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    untag_mask|=global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list;
    if(~(global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list)
            & global_vlan->untag_list & untag_mask){
        untag_mask = ~(global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list)
                    & global_vlan->untag_list & untag_mask;
        printf(" %% Untag port has been set in other vlan: ");
        int i;
        for(i=PORT_1;i<_PORT_MAX;i++){
            if(untag_mask & (1<<(16-i))){
                printf("%8s", id2port(i));
            }
        }
        printf("\n %% Check config to continue!\n");
        return CB_FAIL;
    }
    tag_mask = ~untag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].tag_list;
    sprintf(tag_list, "%02x-%02x%c", tag_mask>>8, 0x00ff & tag_mask, '\0');
    sprintf(port_list, "%02x-%02x%c", (tag_mask|untag_mask)>>8, 0x00ff&(tag_mask|untag_mask), '\0');
    printf(" %% Vlan %d set tag port: %04x, set untag port: %04x.\n", device_gt->view_name.vlan, tag_mask, untag_mask);
    Q8021_vlan_set_port_list(id2idx(vlan2idx(device_gt->view_name.vlan)), port_list, tag_list);
    return CB_SUCCESS;
}

int parse_Q8021_vlan_untag_port_del(char* argv){
    char * extra;
    char * member;
    char port_list[10];
    char tag_list[10];
    int j=0;
    int untag_mask = 0;
    int tag_mask = 0;
    if(!(member = strtok(argv, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + strlen((char*)(&in_put_buf)));
        printf(" %% Please give a member port!\n");
        return CB_FAIL;
    }
    for(; member; member = strtok(NULL, " ")){
        j = str_idx(member, port_name[2]);
        if(PORT_1>j || _PORT_MAX<=j){
            posi_err(strlen(_terminal_view->cmd_head) + (member-(char*)&in_put_buf)/sizeof(char));
            printf(" %% Please give a valid member port!\n");
            return CB_FAIL;
        }else{
            untag_mask |= (1<<(16-j));
        }
    }
    if((extra = strtok(NULL, " "))){
        posi_err(strlen(_terminal_view->cmd_head) + (extra-(char*)&in_put_buf)/sizeof(char));
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    vlan_get_general();
    switch(global_vlan->mode){
        case VLAN_MODE_8021D:
            break;
        case VLAN_MODE_PORTBASED:
            printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
            return CB_FAIL;
            break;
        default:
            printf(" %% Vlan mode error: %d.!", global_vlan->mode);
            return CB_FAIL;
            break;
    }
    untag_mask=~untag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list;
    if(~(global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list)
            & global_vlan->untag_list & untag_mask){
        untag_mask = ~(global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].utag_list)
                    & global_vlan->untag_list & untag_mask;
        printf(" %% Untag port has been set in other vlan: ");
        int i;
        for(i=PORT_1;i<_PORT_MAX;i++){
            if(untag_mask & (1<<(16-i))){
                printf("%8s", id2port(i));
            }
        }
        printf("\n %% Check config to continue!\n");
        return CB_FAIL;
    }
    tag_mask = ~untag_mask&global_vlan->dev_vlan[vlan2idx(device_gt->view_name.vlan)-1].tag_list;
    sprintf(tag_list, "%02x-%02x%c", tag_mask>>8, 0x00ff & tag_mask, '\0');
    sprintf(port_list, "%02x-%02x%c", (tag_mask|untag_mask)>>8, 0x00ff&(tag_mask|untag_mask), '\0');
    printf(" %% Vlan %d set tag port: %04x, set untag port: %04x.\n", device_gt->view_name.vlan, tag_mask, untag_mask);
    Q8021_vlan_set_port_list(id2idx(vlan2idx(device_gt->view_name.vlan)), port_list, tag_list);
    return CB_SUCCESS;
}

int parse_Q8021_vlan_name(char * argv){
    if(argv && 0<strlen(argv) && 16>=strlen(argv)){
        vlan_get_general();
        switch(global_vlan->mode){
            case VLAN_MODE_8021D:
                break;
            case VLAN_MODE_PORTBASED:
                printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
                return CB_FAIL;
                break;
            default:
                printf(" %% Vlan mode error: %d.!", global_vlan->mode);
                return CB_FAIL;
                break;
        }
        if(vlan2idx(device_gt->view_name.vlan)){
            printf(" %% Set vlan %d describe :%s.\n", device_gt->view_name.vlan, argv);
            Q8021_vlan_set_name(id2idx(vlan2idx(device_gt->view_name.vlan)), argv);
            return CB_SUCCESS;
        }else{
            printf(" %% Vlan %d is not exsit!\n", device_gt->view_name.vlan);
            return CB_FAIL;
        }

    }else{
        printf(" %% Invalid vlan describe(1~16 characters)!\n");
        return CB_FAIL;
    }
    return CB_SUCCESS;
}

int parse_Q8021_vlan_clear(char* argv){
    if(argv && 0==strlen(argv)){
        vlan_get_general();
        switch(global_vlan->mode){
            case VLAN_MODE_8021D:
                break;
            case VLAN_MODE_PORTBASED:
                printf(" %% Vlan mode is %s now.!", vlan_mode_tbl[0][global_vlan->mode]);
                return CB_FAIL;
                break;
            default:
                printf(" %% Vlan mode error: %d.!", global_vlan->mode);
                return CB_FAIL;
                break;
        }
        if(vlan2idx(device_gt->view_name.vlan)){
            printf(" %% Clear vlan %d.\n", device_gt->view_name.vlan);
            Q8021_vlan_clear(id2idx(vlan2idx(device_gt->view_name.vlan)));
        }else{
            printf(" %% Vlan %d is not exsit!\n", device_gt->view_name.vlan);
        }
        memset(device_gt, 0, sizeof(struct device_gt));
        memset(_terminal_view, 0, sizeof(struct _terminal_view));
        view_init(device_name, _view_system_list);;
        return CB_SUCCESS;
    }else{
        printf(" %% Give too many parameters!\n");
        return CB_FAIL;
    }
    return CB_SUCCESS;
}

