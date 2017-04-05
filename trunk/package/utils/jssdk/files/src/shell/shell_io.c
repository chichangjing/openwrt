
#include <stdio.h>
#include "shell_io.h"
#include "shell.h"

#include "joyware.h"


#define SW_RTN_ON_NULL_PARAM(rtn) do { if ((rtn) == NULL) return SW_BAD_PARAM; } while(0)

#define DEFAULT_FLAG "default"
static char **full_cmdstrp;
static int talk_mode = 1;

int get_talk_mode(void)
{
    return talk_mode ;
}

void  set_talk_mode(int mode)
{
    talk_mode = mode;
}

void set_full_cmdstrp(char **cmdstrp)
{
    full_cmdstrp = cmdstrp;
}

static char *get_cmd_buf(char *tag, char *defval)
{
    if(!full_cmdstrp || !(*full_cmdstrp)) {
        dprintf("parameter (%s) or default (%s) absent\n", tag, defval);
        exit(1);
    }
    
    if (!strcasecmp(*(full_cmdstrp), DEFAULT_FLAG))  {
        full_cmdstrp++;
        return defval;
    } else {
        return *(full_cmdstrp++);
    }
}

static char *get_cmd_stdin(char *tag, char *defval)
{
    static char gsubcmdstr[128];
    int pos = 0;
    int c;

    if(defval) {
        dprintf("%s(%s): ", tag, defval); 
    } else {
        dprintf("%s: ", tag); 
    }
    
    memset(gsubcmdstr, 0, 128);

    while ((c = getchar()) != '\n') {
        gsubcmdstr[pos++] = c;
        if (pos == 127) {
            dprintf("too long command\n");
            return NULL;
        }
    }

    gsubcmdstr[pos] = '\0';

    return gsubcmdstr;
}

static char *get_sub_cmd(char *tag, char *defval)
{
    if(talk_mode)
        return get_cmd_stdin(tag, defval);
    else
        return get_cmd_buf(tag, defval);
}


static inline  a_bool_t is_hex(char c)
{
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
        || (c >= 'A' && c <= 'F'))
        return A_TRUE;

    return A_FALSE;
}

static inline a_bool_t is_dec(char c)
{
    if ((c >= '0') && (c <= '9'))
        return A_TRUE;

    return A_FALSE;
}

static a_uint8_t shift_high_low_8_bit(a_uint8_t data)
{
	data = (data<<4) | (data>>4);
	data = ((data<<2) & 0xcc) | ((data>>2) & 0x33);
	data = ((data<<1) & 0xaa) | ((data>>1) & 0x55);
	return data;
}

static sw_data_type_t sw_data_type[] = {
    SW_TYPE_DEF(SW_UINT8, NULL, NULL),
    SW_TYPE_DEF(SW_INT8, NULL, NULL),
    SW_TYPE_DEF(SW_UINT16, cmd_data_check_uint16, cmd_data_print_uint16),
    SW_TYPE_DEF(SW_INT16, NULL, NULL),
    SW_TYPE_DEF(SW_UINT32, cmd_data_check_uint32, cmd_data_print_uint32),
    SW_TYPE_DEF(SW_INT32, NULL, NULL),
    SW_TYPE_DEF(SW_UINT64, NULL, NULL),
    SW_TYPE_DEF(SW_INT64, NULL, NULL),
	SW_TYPE_DEF(SW_ENABLE, cmd_data_check_enable, cmd_data_print_enable),
	SW_TYPE_DEF(SW_JWS_PORTLIST, cmd_data_check_jwsportlist, NULL),
#ifdef IN_PORTCONTROL
	SW_TYPE_DEF(SW_PORT_TYPE, NULL, cmd_data_print_port_type),
    SW_TYPE_DEF(SW_PORT_LINK_STATUS, NULL, cmd_data_print_port_link_status),
    SW_TYPE_DEF(SW_PORT_PHYSICAL_MODE, cmd_data_check_port_physical_mode, cmd_data_print_port_physical_mode),
    SW_TYPE_DEF(SW_PORT_PHYSICAL_STATUS, NULL, cmd_data_print_port_physical_status),
    SW_TYPE_DEF(SW_PORT_DOT1D_STATE, cmd_data_check_port_dot1d_state, cmd_data_print_port_dot1d_state), 
    SW_TYPE_DEF(SW_PORT_COMB_TYPE, cmd_data_check_port_comb_type, cmd_data_print_port_comb_type),
    SW_TYPE_DEF(SW_PORT_COMB_PORT, NULL, cmd_data_print_port_comb_port),
#endif

#ifdef IN_MIB
	SW_TYPE_DEF(SW_MIB, NULL, cmd_data_print_mib),
#endif
	
#ifdef IN_MIRROR
	SW_TYPE_DEF(SW_MIRROR_MODE, cmd_data_check_mirror_source_port_mode, cmd_data_print_mirror_source_port_mode),
#endif

#ifdef IN_TRUNK
	SW_TYPE_DEF(SW_TRUNK_LOAD_BALANCE, cmd_data_check_trunk_load_balance, cmd_data_print_trunk_load_balance),
	SW_TYPE_DEF(SW_TRUNK_MODE, cmd_data_check_trunk_pag_mode, cmd_data_print_trunk_pag_mode),
	//SW_TYPE_DEF(SW_TRUNK_ROW_CREATE, cmd_data_check_trunk_members, NULL),
#endif

#ifdef IN_RATE
	SW_TYPE_DEF(SW_RATE_SPEED,NULL,cmd_data_print_rate_speed),
#endif

#ifdef IN_MCAST
	SW_TYPE_DEF(SW_MULTICAST_ADDR,cmd_data_check_multicast_addr,NULL),
#endif

#ifdef IN_VLAN
	SW_TYPE_DEF(SW_VLAN_MODE, cmd_data_check_vlan_mode, cmd_data_print_vlan_mode),
	SW_TYPE_DEF(SW_VLAN_EGRESS_TAG, cmd_data_check_vlan_egress_tag, cmd_data_print_vlan_egress_tag),
#endif

#ifdef IN_QOS
	SW_TYPE_DEF(SW_QOS_MAP, cmd_data_check_qos_map, cmd_data_print_qos_map),
	SW_TYPE_DEF(SW_QOS_SCHEDULE_MODE, cmd_data_check_qos_schedule_mode, cmd_data_print_qos_schedule_mode),
	SW_TYPE_DEF(SW_QOS_ASSIGN_MODE, cmd_data_check_qos_assign_mode, NULL),
	SW_TYPE_DEF(SW_QOS_DEFAULT_PRIO, cmd_data_check_qos_default_prio, NULL),
#endif

#ifdef IN_FDB
	SW_TYPE_DEF(SW_BLACKWHITE_LIST, cmd_data_check_mac_blackwhite_list, NULL),
	SW_TYPE_DEF(SW_ATU_STATUS, cmd_data_check_8021x_mac_atu_status, NULL),
#endif

    SW_TYPE_DEF(SW_PBMP, cmd_data_check_pbmp, cmd_data_print_pbmp),
    SW_TYPE_DEF(SW_SSDK_CFG, NULL, cmd_data_print_ssdk_cfg) 
};

sw_data_type_t *cmd_data_type_find(sw_data_type_e type)
{
    a_uint16_t i = 0;

    do {
        if (type == sw_data_type[i].data_type)
            return &sw_data_type[i];
    } while ( ++i < sizeof(sw_data_type)/sizeof(sw_data_type[0]));

    return NULL;
}

sw_error_t cmd_data_check_uint32(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (0 == cmd_str[0]) {
        return SW_BAD_VALUE;
    }

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", arg_val);
    else
        sscanf(cmd_str, "%d", arg_val);

    return SW_OK;
}

void cmd_data_print_uint32(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]: 0x%08x", param_name, *(a_uint32_t *) buf);
	dprintf("\n\n");
}

sw_error_t cmd_data_check_uint16(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (0 == cmd_str[0]) {
        return SW_BAD_VALUE;
    }

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", arg_val);
    else
        sscanf(cmd_str, "%d", arg_val);

    if (65535 < *arg_val) {
        return SW_BAD_PARAM;
    }

    return SW_OK;
}

void cmd_data_print_uint16(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]: 0x%04x\n", param_name, *(a_uint16_t *) buf);

}

sw_error_t cmd_data_check_pbmp(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", arg_val);
    else
        sscanf(cmd_str, "%d", arg_val);

    return SW_OK;

}

void cmd_data_print_pbmp(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]: 0x%08x", param_name, *(a_uint32_t *) buf);
	dprintf("\n\n");
}

sw_error_t cmd_data_check_enable(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "enable"))
        *arg_val = FAL_ENABLE;
    else if (!strcasecmp(cmd_str, "disable"))
        *arg_val = FAL_DISABLE;
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void cmd_data_print_enable(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_ENABLE) {
        dprintf("Enabled");
    } else if (*(a_uint32_t *) buf == FAL_DISABLE) {
        dprintf("Disabled");
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");
}

sw_error_t cmd_data_check_jwsportlist(char * cmd_str,fal_portlist_t * port_list,a_uint32_t size)
{

	char *tmp = NULL;
	a_uint32_t data;
	a_uint32_t i, j;
	//a_uint32_t len, num;

	if (cmd_str == NULL)
        return SW_BAD_PARAM;

	tmp = strtok(cmd_str,"-:");
	i = 0;
	while(tmp) {
		if(i >= MV88E6097_MAX_MEMBERS_SIZE) {
			return SW_BAD_VALUE;
		}

		if((strlen(tmp) >2)) {
			return SW_BAD_VALUE;
		}

		for (j = 0; j < strlen(tmp); j++) {
            if (A_FALSE == is_hex(tmp[j])) {
                return SW_BAD_VALUE;
            }
        }

		sscanf(tmp,"%x",&data);
		
		tmp = strtok(NULL,"-:");
		port_list->members[i] = data;
		//printf("%s, port_list[%d] = %02x\n",__FUNCTION__,i,port_list->members[i]);
		i++;
	}
/*
	tmp = cmd_str;
	len = strlen(tmp);
	num = len/2;
	printf("%s, len = %d, num = %d\n",__FUNCTION__,len,num);

	if((len % 2) != 0) {
		printf("portlist input error\n");
		return SW_BAD_PARAM;
	}

	for(i=0; i<strlen(tmp); i++) {
        if (A_FALSE == is_hex(tmp[i])) {
            return SW_BAD_VALUE;
        }
    }
	
	sscanf(tmp,"%x",&data);

	for(i=0; i<num; i++) {
		port_list->members[i] = (data & (0xff << ((num-i-1)*8))) >> ((num-i-1)*8);
		printf("%s, port_list[%d] = %02x\n",__FUNCTION__,i,port_list->members[i]);
	}
*/
	return SW_OK;
	
}

#ifdef IN_PORTCONTROL
/*port ctrl*/

void cmd_data_print_port_type(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *)buf == FAL_PORT_TYPE_FE) {
        dprintf("FE");
    } else if (*(a_uint32_t *)buf == FAL_PORT_TYPE_FX) {
        dprintf("FX");
    } else if (*(a_uint32_t *)buf == FAL_PORT_TYPE_GE) {
        dprintf("GE");
    } else if (*(a_uint32_t *)buf == FAL_PORT_TYPE_GX) {
        dprintf("GX");
    } else if (*(a_uint32_t *)buf == FAL_PORT_TYPE_CPU) {
        dprintf("CPU");
    }else if (*(a_uint32_t *)buf == FAL_PORT_TYPE_COMB) {
        dprintf("COMB");
    } else {
        dprintf("UN");
    }
	dprintf("\n\n");
}

void cmd_data_print_port_link_status(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *)buf == FAL_PORT_LINK_STATUS_UP) {
        dprintf("Link up");
    } else if (*(a_uint32_t *)buf == FAL_PORT_LINK_STATUS_DOWN) {
        dprintf("Link down");	
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");
}

sw_error_t cmd_data_check_port_physical_mode(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "auto-neg"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_AUTONEG;
    else if (!strcasecmp(cmd_str, "10-half"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_S10_HALF;
    else if (!strcasecmp(cmd_str, "10-full"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_S10_FULL;
    else if (!strcasecmp(cmd_str, "100-half"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_S100_HALF;
    else if (!strcasecmp(cmd_str, "100-full"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_S100_FULL;
    else if (!strcasecmp(cmd_str, "1000-full"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_S1000_FULL;
	else if (!strcasecmp(cmd_str, "1000-half"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_S1000_HALF;
	else if (!strcasecmp(cmd_str, "auto-copper-fiber"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_COPPER_FIBER_AUTO;
	else if (!strcasecmp(cmd_str, "auto-fiber"))
        *arg_val = FAL_PORT_PHYSICAL_MODE_FIBER_AUTO;
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void cmd_data_print_port_physical_mode(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_AUTONEG) {
        dprintf("auto-neg");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_S10_HALF) {
        dprintf("10-half");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_S10_FULL) {
        dprintf("10-full");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_S100_HALF) {
        dprintf("100-half");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_S100_FULL) {
        dprintf("100-full");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_S1000_FULL) {
        dprintf("1000-full");		
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_S1000_HALF) {
        dprintf("1000-half");		
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_COPPER_FIBER_AUTO) {
        dprintf("auto-copper-fiber");		
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_MODE_FIBER_AUTO) {
        dprintf("auto-fiber");		
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");
}


void cmd_data_print_port_physical_status(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_STATUS_S10_HALF) {
        dprintf("10Mbps half-duplex");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_STATUS_S10_FULL) {
        dprintf("10Mbps full-duplex");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_STATUS_S100_HALF) {
        dprintf("100Mbps half-duplex");
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_STATUS_S100_FULL) {
        dprintf("100Mbps full-duplex");	
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_STATUS_S1000_FULL) {
        dprintf("1000Mbps full-duplex");			
    } else if (*(a_uint32_t *) buf == FAL_PORT_PHYSICAL_STATUS_S1000_HALF) {
        dprintf("1000Mbps half-duplex");			
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");
}

sw_error_t cmd_data_check_port_dot1d_state(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "disabled"))
        *arg_val = FAL_PORT_DOT1D_STATE_DISABLED;
    else if (!strcasecmp(cmd_str, "blocking"))
        *arg_val = FAL_PORT_DOT1D_STATE_BLOCKING;
    else if (!strcasecmp(cmd_str, "learning"))
        *arg_val = FAL_PORT_DOT1D_STATE_LEARNING;
    else if (!strcasecmp(cmd_str, "forwarding"))
        *arg_val = FAL_PORT_DOT1D_STATE_FORWARDING;
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void cmd_data_print_port_dot1d_state(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_PORT_DOT1D_STATE_DISABLED) {
        dprintf("Disabled");
    } else if (*(a_uint32_t *) buf == FAL_PORT_DOT1D_STATE_BLOCKING) {
        dprintf("Blocking");
    } else if (*(a_uint32_t *) buf == FAL_PORT_DOT1D_STATE_LEARNING) {
        dprintf("Learning");
    } else if (*(a_uint32_t *) buf == FAL_PORT_DOT1D_STATE_FORWARDING) {
        dprintf("Forwarding");			
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");
}

sw_error_t cmd_data_check_port_comb_type(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "1000Base-X"))
        *arg_val = FAL_PORT_COMB_FORCE_1000BASE_X;
    else if (!strcasecmp(cmd_str, "100Base-FX"))
        *arg_val = FAL_PORT_COMB_FORCE_100BASE_FX;
    else if (!strcasecmp(cmd_str, "Copper"))
        *arg_val = FAL_PORT_COMB_FORCE_COPPER;
    else if (!strcasecmp(cmd_str, "auto-detect"))
        *arg_val = FAL_PORT_COMB_AUTO_FIBER_COPPER;
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void cmd_data_print_port_comb_type(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_PORT_COMB_FORCE_1000BASE_X) {
        dprintf("1000base-x");
    } else if (*(a_uint32_t *) buf == FAL_PORT_COMB_FORCE_100BASE_FX) {
        dprintf("100base-fx");
    } else if (*(a_uint32_t *) buf == FAL_PORT_COMB_FORCE_COPPER) {
        dprintf("copper");
    } else if (*(a_uint32_t *) buf == FAL_PORT_COMB_AUTO_FIBER_COPPER) {
        dprintf("auto-detect");			
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");
}

void cmd_data_print_port_comb_port(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_PORT_COMB_PORT) {
        dprintf("COMB Port");
    } else if (*(a_uint32_t *) buf == FAL_PORT_NOT_COMB_PORT) {
        dprintf("Not Comb");
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");
}


#endif /* IN_PORTCONTROL */

#ifdef IN_MIB
static char *mib_regname[] = {
	"RxGoodByteLo",
	"RxGoodByteHi",
	"RxUnicastPkts",
	"RxBroadcastPkts",
	"RxMulticastPkts",
	"RxPausePkts",
	"TxByteLo",
	"TxByteHi",
	"TxUnicastPkts",
	"TxBroadcastPkts",
	"TxMulticastPkts",
	"TxPausePkts"
};
void cmd_data_print_mib(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("\n[%s]\n", param_name);
    a_uint32_t offset = 0;
    for (offset = 0; offset < (sizeof (fal_mib_info_t) / sizeof (a_uint32_t)); offset++) {
        dprintf("  %-16s: %-10u\n", mib_regname[offset], *(buf + offset));
    }
	dprintf("\n");
}
#endif /* IN_MIB */

#ifdef IN_MIRROR
/* mirror cmd parse */

sw_error_t cmd_data_check_mirror_source_port_mode(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "tx"))
        *arg_val = FAL_MIRROR_MODE_TX;
    else if (!strcasecmp(cmd_str, "rx"))
        *arg_val = FAL_MIRROR_MODE_RX;
    else if (!strcasecmp(cmd_str, "both"))
        *arg_val = FAL_MIRROR_MODE_BOTH;
	else if (!strcasecmp(cmd_str, "disable"))
        *arg_val = FAL_MIRROR_MODE_DISABLE;
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;	
}

void cmd_data_print_mirror_source_port_mode(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_MIRROR_MODE_TX) {
        dprintf("tx");
    } else if (*(a_uint32_t *) buf == FAL_MIRROR_MODE_RX) {
        dprintf("rx");
    } else if (*(a_uint32_t *) buf == FAL_MIRROR_MODE_BOTH) {
        dprintf("both");
    } else if (*(a_uint32_t *) buf == FAL_MIRROR_MODE_DISABLE) {
        dprintf("disable");
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");	
}

#endif

#ifdef IN_TRUNK
/* trunk cmd parse */

sw_error_t cmd_data_check_trunk_load_balance(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "xor"))
        *arg_val = FAL_TRUNK_LOAD_BALANCE_XOR;
    else if (!strcasecmp(cmd_str, "hash"))
        *arg_val = FAL_TRUNK_LOAD_BALANCE_HASH;	
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;

}

void cmd_data_print_trunk_load_balance(a_uint8_t *param_name, a_uint32_t *buf, a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_TRUNK_LOAD_BALANCE_XOR) {
        dprintf("xor");
    } else if (*(a_uint32_t *) buf == FAL_TRUNK_LOAD_BALANCE_HASH) {
        dprintf("hash");
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");

}

sw_error_t cmd_data_check_trunk_pag_mode(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "disable"))
        *arg_val = FAL_TRUNK_MODE_DISABLE;
    else if (!strcasecmp(cmd_str, "static"))
        *arg_val = FAL_TRUNK_MODE_STATIC;	
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void cmd_data_print_trunk_pag_mode(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == 	FAL_TRUNK_MODE_DISABLE) {
        dprintf("disable");
    } else if (*(a_uint32_t *) buf == FAL_TRUNK_MODE_STATIC) {
        dprintf("static");
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");

}



#if 0
sw_error_t cmd_data_check_trunk_members(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
	a_uint32_t trunk_members;
	a_uint32_t i, j=0;
	a_uint32_t Lport[15] = {0};

    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (0 == cmd_str[0]) {
        return SW_BAD_VALUE;
    }

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", &trunk_members);

	for(i=0; i<10; i++) {
		if(trunk_members & 0x01) {
			Lport[j] = i+1;
			j++;
		}
		trunk_members = trunk_members >> 1;
	}
	*arg_val = Lport;

    return SW_OK;
}
#endif 

#endif

#ifdef IN_RATE
/* rate cmd parse */
void cmd_data_print_rate_speed(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	dprintf("[%s]: 0x%08d", param_name, *(a_uint32_t *) buf);
	dprintf("\n\n");
}

#endif

#ifdef IN_MCAST
/* multicast mac cmd parse */

sw_error_t cmd_data_check_multicast_addr(char *cmd_str, fal_multicast_addr_t *mac_val, a_uint32_t size)
{
	char *tmp = NULL;
	a_uint32_t i,j;
	a_uint32_t data,addr[8];

	
	if (cmd_str == NULL)
        return SW_BAD_PARAM;

	tmp = strtok(cmd_str,"-:");
	i = 0;
	while(tmp) {
		if(i >= 6) {
			return SW_BAD_VALUE;
		}

		if((strlen(tmp) >2) || (strlen(tmp) == 0)) {
			return SW_BAD_VALUE;
		}

		for (j = 0; j < strlen(tmp); j++) {
            if (A_FALSE == is_hex(tmp[j])) {
                return SW_BAD_VALUE;
            }
        }

		sscanf(tmp,"%x",&data);

		addr[i] = data;
		
		tmp = strtok(NULL,"-:");
		mac_val->mac[i] = addr[i];
		//printf("%s, mac_val->mac[%d] = %02x\n",__FUNCTION__,i,mac_val->mac[i]);
		i++;
	
	}

	if(i != 6){ /* Mac must be xx-xx-xx-xx-xx-xx, 6 bytes */
		return SW_BAD_PARAM;
	}
	
	return SW_OK;
	
}


#endif

#ifdef IN_VLAN
/* vlan cmd parse */

sw_error_t cmd_data_check_vlan_mode(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "port-based"))
        *arg_val = FAL_VLAN_MODE_PORT_BASED;
    else if (!strcasecmp(cmd_str, "ieee8021q"))
        *arg_val = FAL_VLAN_MODE_8021Q;	
    else {
        return SW_BAD_VALUE;
    }

    return SW_OK;

}

void cmd_data_print_vlan_mode(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size)
{
    dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_VLAN_MODE_PORT_BASED) {
        dprintf("port-based");
    } else if (*(a_uint32_t *) buf == FAL_VLAN_MODE_8021Q) {
        dprintf("ieee8021q");
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");

}

sw_error_t cmd_data_check_vlan_egress_tag(char * cmd_str,fal_vlan_egress_tag_t * arg_val,a_uint32_t size)
{
	char *tmp = NULL;
	a_uint32_t i;
	a_uint32_t data;
	a_uint8_t byte_tag[MV88E6097_MAX_MEMBERS_SIZE];
	char *egress_tag[CONFIG_JWS_PORT_TOTAL_NR];
	
	
	if (cmd_str == NULL)
        return SW_BAD_PARAM;

	tmp = strtok(cmd_str,"-:");
	i = 0;
	data = 0;
	while(tmp) {
		if(i >= (CONFIG_JWS_PORT_TOTAL_NR)) {
			return SW_BAD_VALUE;
		}

		if((strlen(tmp) >2) || (strlen(tmp) == 0)) {
			return SW_BAD_VALUE;
		}

		egress_tag[i] = tmp;

		if (!strcasecmp(egress_tag[i], "T")) {
	        data |= (1<<i);
			//printf("T ");
		}
	    else if (!strcasecmp(egress_tag[i], "UT")) {
	        //printf("UT ");	
	    }
		else if (!strcasecmp(egress_tag[i], "N")) {
			//printf("N ");
		}
	    else {
	        return SW_BAD_VALUE;
	    }
		
		tmp = strtok(NULL,"-:");
		i++;
	
	}

	byte_tag[0] = data & 0xFF;
	byte_tag[1] = ((data & 0xFF00) >> 8);

	for(i=0; i<MV88E6097_MAX_MEMBERS_SIZE; i++) {
		arg_val->members[i] = shift_high_low_8_bit(byte_tag[i]);
		//printf("%s, arg_val[%d] = %x\n",__FUNCTION__,i,arg_val->members[i]);
	}

	return SW_OK;
	
}

void cmd_data_print_vlan_egress_tag(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size)
{
	
}
	
#endif

#ifdef IN_QOS
/* QoS cmd parase */
sw_error_t cmd_data_check_qos_map(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
	if (cmd_str == NULL)
	   return SW_BAD_PARAM;

	if (!strcasecmp(cmd_str, "ip-mapping"))
	   *arg_val = FAL_QOS_IP_MAPPING;
	else if (!strcasecmp(cmd_str, "tag-mapping"))
	   *arg_val = FAL_QOS_TAG_MAPPING; 
	else {
	   return SW_BAD_VALUE;
	}

	return SW_OK;

}

void cmd_data_print_qos_map(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size)
{
	dprintf("[%s]: ", param_name);
	if (*(a_uint32_t *) buf == FAL_QOS_IP_MAPPING) {
		dprintf("ip-mapping");
	} else if (*(a_uint32_t *) buf == FAL_QOS_TAG_MAPPING) {
		dprintf("tag-mapping");
	} else {
		dprintf("Unknown");
	}
	dprintf("\n\n");

}

sw_error_t cmd_data_check_qos_schedule_mode(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
	if (cmd_str == NULL)
	   return SW_BAD_PARAM;

	if (!strcasecmp(cmd_str, "SR"))
	   *arg_val = FAL_QOS_SR_MODE;
	else if (!strcasecmp(cmd_str, "WRR"))
	   *arg_val = FAL_QOS_WRR_MODE; 
	else {
	   return SW_BAD_VALUE;
	}

	return SW_OK;

}

void cmd_data_print_qos_schedule_mode(a_uint8_t * param_name,a_uint32_t * buf,a_uint32_t size)
{
	dprintf("[%s]: ", param_name);
    if (*(a_uint32_t *) buf == FAL_QOS_SR_MODE) {
        dprintf("SR");
    } else if (*(a_uint32_t *) buf == FAL_QOS_WRR_MODE) {
        dprintf("WRR");
    } else {
        dprintf("Unknown");
    }
	dprintf("\n\n");	
}

sw_error_t cmd_data_check_qos_assign_mode(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
	if (cmd_str == NULL)
	   return SW_BAD_PARAM;

	if (!strcasecmp(cmd_str, "use-default"))
	   *arg_val = FAL_QOS_USE_DEFAULT_ASSIGN_MODE;
	else if (!strcasecmp(cmd_str, "use-tag"))
	   *arg_val = FAL_QOS_USE_TAG_ASSIGN_MODE; 
	else if (!strcasecmp(cmd_str, "use-ip"))
	   *arg_val = FAL_QOS_USE_IP_ASSIGN_MODE; 
	else if (!strcasecmp(cmd_str, "use-tag-ip"))
	   *arg_val = FAL_QOS_USE_TAG_IP_ASSIGN_MODE; 
	else {
	   return SW_BAD_VALUE;
	}

	return SW_OK;

}

sw_error_t cmd_data_check_qos_default_prio(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
	if (cmd_str == NULL)
	   return SW_BAD_PARAM;

	if (!strcasecmp(cmd_str, "prio-lowest"))
	   *arg_val = FAL_QOS_PRIO_LOWEST;
	else if (!strcasecmp(cmd_str, "prio-low"))
	   *arg_val = FAL_QOS_PRIO_LOW; 
	else if (!strcasecmp(cmd_str, "prio-high"))
	   *arg_val = FAL_QOS_PRIO_HIGH; 
	else if (!strcasecmp(cmd_str, "prio-highest"))
	   *arg_val = FAL_QOS_PRIO_HIGHEST; 
	else {
	   return SW_BAD_VALUE;
	}

	return SW_OK;

}


#endif

#ifdef IN_FDB
/* FDB cmd parase */
sw_error_t cmd_data_check_mac_blackwhite_list(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
	if (cmd_str == NULL)
	   return SW_BAD_PARAM;

	if (!strcasecmp(cmd_str, "blacklist"))
	   *arg_val = FAL_MAC_BLACKLIST;
	else if (!strcasecmp(cmd_str, "whitelist"))
	   *arg_val = FAL_MAC_WHITELIST; 
	else {
	   return SW_BAD_VALUE;
	}

	return SW_OK;

}

sw_error_t cmd_data_check_8021x_mac_atu_status(char * cmd_str,a_uint32_t * arg_val,a_uint32_t size)
{
	if (cmd_str == NULL)
	   return SW_BAD_PARAM;

	if (!strcasecmp(cmd_str, "dynamic"))
	   *arg_val = FAL_MAC_DYNAMIC;
	else if (!strcasecmp(cmd_str, "static"))
	   *arg_val = FAL_MAC_STATIC; 
	else {
	   return SW_BAD_VALUE;
	}

	return SW_OK;

}
#endif



sw_error_t cmd_data_check_confirm(char *cmdstr, a_bool_t def, a_bool_t * val, a_uint32_t size)
{
    if (0 == cmdstr[0]) {
        *val = def;
    } else if ((!strcasecmp(cmdstr, "yes")) || (!strcasecmp(cmdstr, "y"))) {
        *val = A_TRUE;
    } else if ((!strcasecmp(cmdstr, "no")) || (!strcasecmp(cmdstr, "n"))) {
        *val = A_FALSE;
    } else {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void cmd_data_print_confirm(a_uint8_t * param_name, a_bool_t val, a_uint32_t size)
{
    dprintf("%s", param_name);
    if (A_TRUE == val) {
        dprintf("Yes");
    } else if (A_FALSE == val) {
        dprintf("No");
    } else {
        dprintf("Unknown");
    }

    return;
}

sw_error_t cmd_data_check_portmap(char *cmdstr, fal_pbmp_t * val, a_uint32_t size)
{
    char *tmp = NULL;
    a_uint32_t i = 0;
    a_uint32_t port;

    *val = 0;
    //default input null
    if(!strcasecmp(cmdstr, "null")) {
        return SW_OK;
    }
    
    tmp = (void *) strtok(cmdstr, ",");
    while (tmp) {
        if (SW_MAX_NR_PORT <= i) {
            return SW_BAD_VALUE;
        }

        sscanf(tmp, "%d", &port);
        if (SW_MAX_NR_PORT <= port) {
            return SW_BAD_VALUE;
        }

        *val |= (0x1 << port);
        tmp = (void *) strtok(NULL, ",");
    }

    return SW_OK;
}

void cmd_data_print_portmap(a_uint8_t * param_name, fal_pbmp_t val, a_uint32_t size)
{
    a_uint32_t i;
    char tmp[16];
    tmp[0] = '\0';
    
    dprintf("%s", param_name);
    for (i = 0; i < SW_MAX_NR_PORT; i++) {
        if (val & (0x1 << i)) {
            if(strlen(tmp) == 0)
                sprintf(tmp, "%d", i);
            else
                sprintf(tmp+strlen(tmp), ",%d", i);
        }
    }
    dprintf("%s ", tmp);
    return;
}


#define cmd_data_check_element(info, defval, usage, chk_func, param) \
{\
    sw_error_t ret;\
    do {\
        cmd = get_sub_cmd(info, defval);\
        SW_RTN_ON_NULL_PARAM(cmd);\
        \
        if (!strncasecmp(cmd, "quit", 4)) {\
            return SW_BAD_VALUE;\
        } else if (!strncasecmp(cmd, "help", 4)) {\
            dprintf("%s", usage);\
            ret = SW_BAD_VALUE;\
        } else {\
            ret = chk_func param; \
            if (SW_OK != ret)\
                dprintf("%s", usage);\
        }\
    } while (talk_mode && (SW_OK != ret));\
}

sw_error_t cmd_data_check_integer(char *cmd_str, a_uint32_t * arg_val, a_uint32_t max_val, a_uint32_t min_val)
{
    a_uint32_t tmp;
    a_uint32_t i;

    if (NULL == cmd_str) {
        return SW_BAD_PARAM;
    }

    if (0 == cmd_str[0]) {
        return SW_BAD_PARAM;
    }

    if ((cmd_str[0] == '0') && ((cmd_str[1] == 'x') || (cmd_str[1] == 'X'))) {
        for (i = 2; i < strlen(cmd_str); i++) {
            if (A_FALSE == is_hex(cmd_str[i])) {
                return SW_BAD_VALUE;
            }
        }
        sscanf(cmd_str, "%x", &tmp);
    } else {
        for (i = 0; i < strlen(cmd_str); i++) {
            if (A_FALSE == is_dec(cmd_str[i])) {
                return SW_BAD_VALUE;
            }
        }
        sscanf(cmd_str, "%d", &tmp);
    }

    if ((tmp > max_val) || (tmp < min_val))
        return SW_BAD_PARAM;

    *arg_val = tmp;
    return SW_OK;
}

sw_error_t cmd_data_check_ip4addr(char *cmdstr, fal_ip4_addr_t * val)
{
    char *tmp = NULL;
    a_uint32_t i = 0, j;
    a_uint32_t addr;
    fal_ip4_addr_t ip4;

    memset(&ip4, 0, sizeof (fal_ip4_addr_t));
    if (NULL == cmdstr) {
        return SW_BAD_VALUE;
    }

    if (0 == cmdstr[0]) {
        return SW_BAD_VALUE;
    }

    tmp = (void *) strtok(cmdstr, ".");
    while (tmp) {
        if (4 <= i) {
            return SW_BAD_VALUE;
        }

        if ((3 < strlen(tmp)) || (0 == strlen(tmp))) {
            return SW_BAD_VALUE;
        }

        for (j = 0; j < strlen(tmp); j++) {
            if (A_FALSE == is_dec(tmp[j])) {
                return SW_BAD_VALUE;
            }
        }

        sscanf(tmp, "%d", &addr);
        if (255 < addr) {
            return SW_BAD_VALUE;
        }

        ip4 |= ((addr & 0xff) << (24 - i * 8));
        i++;
        tmp = (void *) strtok(NULL, ".");
    }

    if (4 != i) {
        return SW_BAD_VALUE;
    }

    *val = ip4;
    return SW_OK;
}

void cmd_data_print_ip4addr(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    a_uint32_t i;
    fal_ip4_addr_t ip4;

    ip4 = *((fal_ip4_addr_t *) buf);
    dprintf("%s", param_name);
    for (i = 0; i < 3; i++) {
        dprintf("%d.", (ip4 >> (24 - i * 8)) & 0xff);
    }
    dprintf("%d", (ip4 & 0xff));
}

sw_error_t cmd_data_check_ip6addr(char *cmdstr, fal_ip6_addr_t * val)
{
    char *tmp = NULL;
    a_uint32_t j;
    a_uint32_t i = 0, cnt = 0, rep = 0, loc = 0;
    a_uint32_t data;
    a_uint32_t addr[8];
    fal_ip6_addr_t ip6;

    if (NULL == cmdstr) {
        return SW_BAD_VALUE;
    }

    if (0 == cmdstr[0]) {
        return SW_BAD_VALUE;
    }

    for (i = 0; i < 8; i++) {
        addr[i] = 0;
    }

    for (i = 0; i < strlen(cmdstr); i++) {
        if (':' == cmdstr[i]) {
            if ((i == (strlen(cmdstr) - 1))
                || (0 == i)) {
                return SW_BAD_VALUE;
            }
            cnt++;

            if (':' == cmdstr[i - 1]) {
                rep++;
                loc = cnt - 1;
            }
        }
    }

    if (1 < rep) {
        return SW_BAD_VALUE;
    }

    tmp = (void *) strtok(cmdstr, ":");
    i = 0;
    while (tmp) {
        if (8 <= i) {
            return SW_BAD_VALUE;
        }

        if ((4 < strlen(tmp)) || (0 == strlen(tmp))) {
            return SW_BAD_VALUE;
        }

        for (j = 0; j < strlen(tmp); j++) {
            if (A_FALSE == is_hex(tmp[j])) {
                return SW_BAD_VALUE;
            }
        }

        sscanf(tmp, "%x", &data);
        if (65535 < data) {
            return SW_BAD_VALUE;
        }

        addr[i++] = data;
        tmp = (void *) strtok(NULL, ":");
    }

    if (0 == rep) {
        if (8 != i) {
            return SW_BAD_VALUE;
        }
    } else {
        if (8 <= i) {
            return SW_BAD_VALUE;
        }

        for (j = i - 1; j >= loc; j--) {
            addr[8 - i + j] = addr[j];
            addr[j] = 0;
        }
    }

    for (i = 0; i < 4; i++) {
        ip6.ul[i] = (addr[i * 2] << 16) | addr[i * 2 + 1];
    }

    dprintf("\n");
    for (i = 0; i < 4; i++) {
        dprintf("%08x  ", ip6.ul[i]);
    }
    dprintf("\n");

    *val = ip6;
    return SW_OK;
}

void cmd_data_print_ip6addr(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    a_uint32_t i;
    fal_ip6_addr_t ip6;

    ip6 = *(fal_ip6_addr_t *) buf;
    dprintf("%s", param_name);
    for (i = 0; i < 3; i++) {
        dprintf("%x:%x:", (ip6.ul[i] >> 16) & 0xffff, ip6.ul[i] & 0xffff);
    }
    dprintf("%x:%x", (ip6.ul[3] >> 16) & 0xffff, ip6.ul[3] & 0xffff);
}

inline char*  cmd_access_mode(hsl_access_mode mode)
{
    switch (mode) {
    case HSL_MDIO:
        return "MDIO";
    case HSL_SPI:
        return "SPI";
    }
    
    return "Unknown";
}


#define BOOL2STR(val_bool) (((val_bool)==A_TRUE)?"true":"false" )
#define BOOL2NAME(val_bool) (((feature->in_##val_bool)==A_TRUE)?(#val_bool):"" )
#define DEFINED2STR(name) (((init->reg_func.name))?"y":"n" )

static void _cmd_data_print_cfg(ssdk_cfg_t *entry)
{
    ssdk_init_cfg *init = &(entry->init_cfg);
    
    dprintf("  Build verison      : v%s\n", entry->build_ver);
	dprintf("  Build date         : %s\n", entry->build_date);
    dprintf("  Switch chip type   : %s\n", entry->chip_type);
	dprintf("  OS information     : %s\n", entry->os_info);
	dprintf("  CPU arch type      : %s\n", entry->cpu_type);
    dprintf("  Reg access mode    : %s\n", cmd_access_mode(init->reg_mode));
    dprintf("  Interface defined  : mdio_set(%s) mdio_get(%s) \n", DEFINED2STR(mdio_set), DEFINED2STR(mdio_get));
    dprintf("\n");
}

void cmd_data_print_ssdk_cfg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    ssdk_cfg_t *ssdk_cfg = (ssdk_cfg_t *) buf;
    dprintf("\nJssdk configuration info :\n");
    _cmd_data_print_cfg(ssdk_cfg);

	#if 0
    dprintf("\n3.SSDK FEATURES LIST:\n");

    ssdk_features *feature = &(ssdk_cfg->features);
    dprintf("%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n", 
        BOOL2NAME(acl), BOOL2NAME(fdb), BOOL2NAME(igmp), BOOL2NAME(leaky),
        BOOL2NAME(led), BOOL2NAME(mib), BOOL2NAME(mirror), BOOL2NAME(misc),
        BOOL2NAME(portcontrol), BOOL2NAME(portvlan), BOOL2NAME(qos), BOOL2NAME(rate),
        BOOL2NAME(stp), BOOL2NAME(vlan), BOOL2NAME(reduced_acl));
	#endif
    
}


