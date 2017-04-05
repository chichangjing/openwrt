
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <dirent.h>
#include <net/if.h>

#include "utils.h"
#include "ctl_client.h"
#include "ctl_func.h"

struct command {
	int nargs;
	int optargs;
	const char *name;
	int (*func) (int argc, char *const *argv);
	const char *help;
};

#define STP_IN_get_error_explanation CTL_error_explanation

static void print_bridge_id(UID_BRIDGE_ID_T * bridge_id, unsigned char cr)
{
	printf("%04lX-%02x%02x%02x%02x%02x%02x",
	       (unsigned long)bridge_id->prio,
	       (unsigned char)bridge_id->addr[0],
	       (unsigned char)bridge_id->addr[1],
	       (unsigned char)bridge_id->addr[2],
	       (unsigned char)bridge_id->addr[3],
	       (unsigned char)bridge_id->addr[4],
	       (unsigned char)bridge_id->addr[5]);
	if (cr)
		printf("\n");
}

static char *stp_state2str(RSTP_PORT_STATE stp_port_state, int detail)
{
	if (detail) {
		switch (stp_port_state) {
		case UID_PORT_DISABLED:
			return "Disabled";
		case UID_PORT_DISCARDING:
			return "Discarding";
		case UID_PORT_LEARNING:
			return "Learning";
		case UID_PORT_FORWARDING:
			return "Forwarding";
		case UID_PORT_NON_STP:
			return "NoStp";
		default:
			return "Unknown";
		}
	}

	switch (stp_port_state) {
	case UID_PORT_DISABLED:
		return "Dis";
	case UID_PORT_DISCARDING:
		return "Blk";
	case UID_PORT_LEARNING:
		return "Lrn";
	case UID_PORT_FORWARDING:
		return "Fwd";
	case UID_PORT_NON_STP:
		return "Non";
	default:
		return "Unk";
	}
}

unsigned int getuint(const char *s)
{
	char *end;
	long l;
	l = strtoul(s, &end, 0);
	if (*s == 0 || *end != 0 || l > INT_MAX) {
		fprintf(stderr, "Invalid unsigned int arg %s\n", s);
		exit(1);
	}
	return l;
}

int getenum(const char *s, const char *opt[])
{
	int i;
	for (i = 0; opt[i] != NULL; i++)
		if (strcmp(s, opt[i]) == 0)
			return i;

	fprintf(stderr, "Invalid argument %s: expecting one of ", s);
	for (i = 0; opt[i] != NULL; i++)
		fprintf(stderr, "%s%s", opt[i], (opt[i + 1] ? ", " : "\n"));

	exit(1);
}

int getyesno(const char *s, const char *yes, const char *no)
{
	/* Reverse yes and no so error message looks more normal */
	const char *opt[] = { yes, no, NULL };
	return 1 - getenum(s, opt);
}

static int set_bridge_cfg_value(unsigned long value, unsigned long val_mask)
{
	UID_STP_CFG_T uid_cfg;
	char *val_name;
	int rc;

	uid_cfg.field_mask = val_mask;
	switch (val_mask) {
	case BR_CFG_STATE:
		uid_cfg.stp_enabled = value;
		val_name = "state";
		break;
	case BR_CFG_PRIO:
		uid_cfg.bridge_priority = value;
		val_name = "priority";
		break;
	case BR_CFG_AGE:
		uid_cfg.max_age = value;
		val_name = "max_age";
		break;
	case BR_CFG_HELLO:
		uid_cfg.hello_time = value;
		val_name = "hello_time";
		break;
	case BR_CFG_DELAY:
		uid_cfg.forward_delay = value;
		val_name = "forward_delay";
		break;
	case BR_CFG_FORCE_VER:
		uid_cfg.force_version = value;
		val_name = "force_version";
		break;
	case BR_CFG_AGE_MODE:
	case BR_CFG_AGE_TIME:
	default:
		printf("Invalid value mask 0X%lx\n", val_mask);
		return -1;
		break;
	}

	rc = CTL_set_bridge_config(&uid_cfg);
	if (0 != rc) {
		printf("Can't change rstp bridge %s:%s\n", val_name, STP_IN_get_error_explanation(rc));
		return -1;
	}
	return 0;
}

static int set_port_cfg_value(int port_index, unsigned long value, unsigned long val_mask)
{
	UID_STP_PORT_CFG_T uid_cfg;
	int rc;
	char *val_name;

	BitmapClear(&uid_cfg.port_bmp);
	uid_cfg.field_mask = val_mask;
	switch (val_mask) {
	case PT_CFG_MCHECK:
		val_name = "mcheck";
		break;
	case PT_CFG_COST:
		uid_cfg.admin_port_path_cost = value;
		val_name = "path cost";
		break;
	case PT_CFG_PRIO:
		uid_cfg.port_priority = value;
		val_name = "priority";
		break;
	case PT_CFG_P2P:
		uid_cfg.admin_point2point = (ADMIN_P2P_T) value;
		val_name = "p2p flag";
		break;
	case PT_CFG_EDGE:
		uid_cfg.admin_edge = value;
		val_name = "adminEdge";
		break;
	case PT_CFG_NON_STP:
		uid_cfg.admin_non_stp = value;
		val_name = "adminNonStp";
		break;
#ifdef STP_DBG
	case PT_CFG_DBG_SKIP_TX:
		uid_cfg.skip_tx = value;
		val_name = "skip tx";
		break;
	case PT_CFG_DBG_SKIP_RX:
		uid_cfg.skip_rx = value;
		val_name = "skip rx";
		break;
#endif
	case PT_CFG_STATE:
	default:
		printf("Invalid value mask 0X%lx\n", val_mask);
		return -1;
	}

	rc = CTL_set_port_config(port_index, &uid_cfg);
	if (0 != rc) {
		printf("can't change rstp port[s] %s: %s\n", val_name, STP_IN_get_error_explanation(rc));
		return -1;
	}
	return 0;
}

static int cmd_show_bridge(int argc, char *const *argv)
{
	int r;
	UID_STP_STATE_T uid_state;
	UID_STP_CFG_T uid_cfg;

	r = CTL_get_bridge_state(&uid_cfg, &uid_state);
	if (r) {
		fprintf(stderr, "Failed to get bridge state\n");	
		return -1;
	}

	printf("\n");
	printf("==============================================\n");
	printf("          Bridge Information Display          \n");
	printf("==============================================\n");
	printf("  Bridge State             : ");
	switch (uid_state.stp_enabled) {
	case STP_ENABLED:
		printf("Enabled\n");
		break;
	case STP_DISABLED:
		printf("Disabled\n");
		break;
	default:
		printf("Unknown\n");
		return 0;
	}

	printf("  BridgeId                 : "); print_bridge_id(&uid_state.bridge_id, 1);
	printf("  Bridge ForceVersion      : %s\n", (uid_cfg.force_version < 2)? "STP" : "RSTP");
	printf("  Designated Root          : "); print_bridge_id(&uid_state.designated_root, 1);

	if (uid_state.root_port) {
	printf("  Root Port                : %04lx\n", (unsigned long)uid_state.root_port);
	printf("  Root Cost                : %-lu\n", (unsigned long)uid_state.root_path_cost);
	} else {
	printf("  Root Port                : None\n");
	}
	if (uid_state.Topo_Change)
		printf("  Topo Change Count        : %lu\n", uid_state.Topo_Change_Count);
	else
		printf("  Time Since Topo Change   : %lu\n", uid_state.timeSince_Topo_Change);

	printf("\n");
	printf("  MaxAge       : %-2d  Bridge MaxAge       : %-2d\n", (int)uid_state.max_age, (int)uid_cfg.max_age);
	printf("  HelloTime    : %-2d  Bridge HelloTime    : %-2d\n", (int)uid_state.hello_time, (int)uid_cfg.hello_time);
	printf("  ForwardDelay : %-2d  Bridge ForwardDelay : %-2d\n", (int)uid_state.forward_delay, (int)uid_cfg.forward_delay);
	printf("  HoldTime     : %-2d\n", (int)uid_cfg.hold_time);
	printf("\n");

	return 0;
}

static int cmd_show_port(int argc, char *const *argv)
{
	int i, r;
	char ifName[IFNAMSIZ];
	int port_count, port_index;
	UID_STP_PORT_CFG_T portCfg;
	UID_STP_PORT_STATE_T portState;

	
	if(get_bridge_name(ifName) < 0) {
		fprintf(stderr, "Error: not found bridge\n");
		return -1;
	}

	if(get_port_count(ifName, &port_count) < 0) {
		fprintf(stderr, "Error: get port count failed\n");
		return -1;
	}

	for (i=0; i<port_count; i++) {
		port_index = i+1;
		r = CTL_get_port_state(port_index, &portCfg, &portState);
		if (r) {
			fprintf(stderr, "Failed to get port%d state\n", port_index);
			return -1;
		}
#if 0
		if(port_index == 1) {
			printf("\n");
			printf("==============================================================\n");
			printf("(a)  (b) (c)  (d)         (e)               (f)        (g) (h)\n");
			printf("==============================================================\n");
		}
		printf("%c%c%c  ",
		       (portState.oper_point2point) ? ' ' : '*',
		       (portState.oper_edge) ? 'E' : ' ',
		       (portState.oper_stp_neigb) ? 'S' : ' ');
		printf("P%02d", port_index);
		printf(" %04lx %3s  ", (unsigned long)portState.port_id, stp_state2str(portState.state, 0));
		print_bridge_id(&portState.designated_root, 0);
		printf(" ");
		print_bridge_id(&portState.designated_bridge, 0);
		printf(" %4lx ", (unsigned long)portState.designated_port);
		switch (portState.role) {
			case 'A': printf("A\n"); break;
			case 'B': printf("B\n"); break;
			case 'R': printf("R\n"); break;
			case 'D': printf("D\n"); break;
			case '-': printf("N\n"); break;
			default:  printf(" \n"); break;
		}
#else
		if(port_index == 1) {
			printf("\n");
			printf("========================================================================================================\n");
			printf("Port  Id       Cost Role State      P2P  Edge  Desi-root         Desi-bridge        Desi-cost  Desi-port\n");
			printf("========================================================================================================\n");
		//	printf(" 01  8001 200000000  R   Forwarding  Y    N    8000-001ECD013197 8000-001ECD013197  200000000  8001     \n");
		}
		printf(" %02d  ", port_index);
		printf("%04lx ", (unsigned long)portState.port_id);
		if(portCfg.admin_port_path_cost == ADMIN_PORT_PATH_COST_AUTO)
			printf("%9s", "Auto");
		else
			printf("%9lu", portCfg.admin_port_path_cost);
		printf("  ");
		switch (portState.role) {
			case 'A': printf("A"); break;
			case 'B': printf("B"); break;
			case 'R': printf("R"); break;
			case 'D': printf("D"); break;
			case '-': printf("-"); break;
			default:  printf(" "); break;
		}
		printf("   ");
		printf("%-10s ", stp_state2str(portState.state, 1));

		//if((' ' != portState.role) && ('-' != portState.role)) {
			printf(" %c   ",(portState.oper_point2point) ? 'Y' : 'N');
			printf(" %c    ",(portState.oper_edge) ? 'Y' : 'N');
			print_bridge_id(&portState.designated_root, 0);
			printf(" ");
			print_bridge_id(&portState.designated_bridge, 0);
			printf("  ");
			printf("%-9ld ", (unsigned long)portState.designated_cost);
			printf("  ");
			printf("%4lx", (unsigned long)portState.designated_port);
		//}
		printf("\n");
#endif
	}
#if 0	
	printf("\n");
	printf("Note:\n");
	printf("   (a): P2P(Auto='*'), Edge(Yes='E'), Partner(STP='S')\n");
	printf("   (b): Port Index\n");
	printf("   (c): Port ID\n");
	printf("   (d): State: Dis, Blk, Lrn, Fwd, Non, Unk\n");
	printf("   (e): Designated Root\n");
	printf("   (f): Designated Bridge\n");
	printf("   (g): Designated PortID\n");
	printf("   (h): Role: Alternate, Backup, Root, Designated, NonStp\n");
	printf("\n");
#else
	printf("\n");
	printf("Note: (1) Port role value : 'A' : Alternate, 'B' : Backup, 'R' : Root, 'D' : Designated, '-' : NonStp\n");
	printf("\n");
#endif
	return 0;
}

static int cmd_show_portdetail(int argc, char *const *argv)
{
	int r;
	int port_index;
	UID_STP_PORT_CFG_T portCfg;
	UID_STP_PORT_STATE_T portState;
	
	port_index = strtoul(argv[1], 0, 10);
	
	r = CTL_get_port_state(port_index, &portCfg, &portState);
	if (r) {
		fprintf(stderr, "Failed to get port%d state\n", port_index);
		return -1;
	}

	printf("\n");
	printf("==============================================\n");
	printf("          Port%d Information Display          \n", port_index);
	printf("==============================================\n");
	printf("  PortId              : %04lx\n", (unsigned long)portState.port_id);
	printf("  Priority            : %d\n", (int)(portState.port_id >> 8));
	printf("  State               : %-9s uptime : %-9lu\n", stp_state2str(portState.state, 0), portState.uptime);
	if(portCfg.admin_port_path_cost == ADMIN_PORT_PATH_COST_AUTO)
	printf("  PathCost      admin : %-9s   oper : %-9lu\n", "Auto", portState.oper_port_path_cost);
	else
	printf("  PathCost      admin : %-9lu   oper : %-9lu\n", portCfg.admin_port_path_cost, portState.oper_port_path_cost);	
	printf("  Point2Point   admin : %-9s   oper : %-9s\n", 
										(portCfg.admin_point2point == P2P_FORCE_TRUE)  ? "ForceYes" : \
										(portCfg.admin_point2point == P2P_FORCE_FALSE) ? "ForceNo" : \
										(portCfg.admin_point2point == P2P_AUTO) ? "Auto" : "Unkown", 
					 					 portState.oper_point2point ? "Yes" : "No");
	printf("  Edge          admin : %-9s   oper : %-9s\n", portCfg.admin_edge ? "Y" : "N", portState.oper_edge ? "Y" : "N");
	printf("  Partner             : %-9s\n", portState.oper_stp_neigb ? "Slow" : "Rapid");

	printf("\n");
	if (' ' != portState.role) {
		if ('-' != portState.role) {
			printf("  PathCost            : %-lu\n", (unsigned long)(portState.path_cost));
			printf("  Designated Root     : "); print_bridge_id(&portState.designated_root, 1);
			printf("  Designated Cost     : %-ld\n", (unsigned long)portState.designated_cost);
			printf("  Designated Bridge   : "); print_bridge_id(&portState.designated_bridge, 1);
			printf("  Designated Port     : %-4lx\n\r", (unsigned long)portState.designated_port);
		}
			printf("  Role                : ");
		switch (portState.role) {
		case 'A': printf("Alternate\n"); break;
		case 'B': printf("Backup\n"); break;
		case 'R': printf("Root\n"); break;
		case 'D': printf("Designated\n"); break;
		case '-': printf("NonStp\n"); break;
		default: printf("Unknown\n"); break;
		}

		if ('R' == portState.role || 'D' == portState.role) {
			printf("  TcAck               : %c\n", portState.top_change_ack ? 'Y' : 'N');
			printf("  TcWhile             : %-3d\n", (int)portState.tcWhile);
		}
	}

	if (UID_PORT_DISABLED == portState.state || '-' == portState.role) {
		#if 0
		printf("  helloWhen:          : %d\n", (int)portState.helloWhen);
		printf("  lnkWhile:           : %d\n", (int)portState.lnkWhile);
		printf("  fdWhile:            : %d\n", (int)portState.fdWhile);
		#endif
	} else if ('-' != portState.role) {
		printf("  fdWhile             : %-3d\n", (int)portState.fdWhile);
		printf("  rcvdInfoWhile       : %-3d\n", (int)portState.rcvdInfoWhile);
		printf("  rbWhile             : %-3d\n", (int)portState.rbWhile);
		printf("  rrWhile             : %-3d\n", (int)portState.rrWhile);

		#if 0
		printf("  mdelayWhile:        : %d\n", (int)portState.mdelayWhile);
		printf("  lnkWhile:           : %d\n", (int)portState.lnkWhile);
		printf("  helloWhen:          : %d\n", (int)portState.helloWhen);
		printf("  txCount:            : %d\n", (int)portState.txCount);
		#endif
	}

		printf("  RSP BPDU rx count   : %lu\n", (unsigned long)portState.rx_rstp_bpdu_cnt);
		printf("  CNF BPDU rx count   : %lu\n", (unsigned long)portState.rx_cfg_bpdu_cnt);
		printf("  TCN BPDU rx count   : %lu\n", (unsigned long)portState.rx_tcn_bpdu_cnt);
		
	printf("\n");

	return 0;
}


static int cmd_rstp(int argc, char *const *argv)
{
	int r, enable;

	if (!strcmp(argv[1], "on") || !strcmp(argv[1], "yes") || !strcmp(argv[1], "1")) {
		enable = 1;
	} else if (!strcmp(argv[1], "off") || !strcmp(argv[1], "no") || !strcmp(argv[1], "0")) {
		enable = 0;
	} else {
		fprintf(stderr, "Incorrect argument\n");
		return 1;
	}

	r = CTL_enable_rstp(enable);
	if (r < 0) {
		fprintf(stderr, "Failed to %s rstp\n", (enable == 1)? "enable":"disable");
		return -1;
	}

	return 0;
}

static int cmd_set_bridge_state(int argc, char *const *argv)
{
	return set_bridge_cfg_value(getyesno(argv[1], "on", "off"),BR_CFG_STATE);
}

static int cmd_set_bridge_prio(int argc, char *const *argv)
{
	return set_bridge_cfg_value(getuint(argv[1]), BR_CFG_PRIO);
}

static int cmd_set_bridge_hello(int argc, char *const *argv)
{
	return set_bridge_cfg_value(getuint(argv[1]), BR_CFG_HELLO);
}

static int cmd_set_bridge_maxage(int argc, char *const *argv)
{
	return set_bridge_cfg_value(getuint(argv[1]), BR_CFG_AGE);
}

static int cmd_set_bridge_fdelay(int argc, char *const *argv)
{
	return set_bridge_cfg_value(getuint(argv[1]), BR_CFG_DELAY);
}

static int cmd_set_bridge_forcevers(int argc, char *const *argv)
{
	return set_bridge_cfg_value(2 * getyesno(argv[2], "normal", "slow"), BR_CFG_FORCE_VER);
}

static int cmd_set_port_prio(int argc, char *const *argv)
{
	int port_index = getuint(argv[1]);
	return set_port_cfg_value(port_index, getuint(argv[2]), PT_CFG_PRIO);
}

static int cmd_set_port_pathcost(int argc, char *const *argv)
{
	int port_index = getuint(argv[1]);
	return set_port_cfg_value(port_index, getuint(argv[2]), PT_CFG_COST);
}

static int cmd_set_port_edge(int argc, char *const *argv)
{
	int port_index = getuint(argv[1]);
	return set_port_cfg_value(port_index, getyesno(argv[2], "yes", "no"), PT_CFG_EDGE);
}

static int cmd_set_port_nonstp(int argc, char *const *argv)
{
	int port_index = getuint(argv[1]);
	return set_port_cfg_value(port_index, getyesno(argv[2], "yes", "no"), PT_CFG_NON_STP);
}

static int cmd_set_port_p2p(int argc, char *const *argv)
{
	const char *opts[] = { "yes", "no", "auto", NULL };
	int vals[] = { P2P_FORCE_TRUE, P2P_FORCE_FALSE, P2P_AUTO };
	
	int port_index = getuint(argv[1]);
	return set_port_cfg_value(port_index, vals[getenum(argv[2], opts)], PT_CFG_P2P);
}

static int cmd_portmcheck(int argc, char *const *argv)
{
	int port_index = getuint(argv[1]);
	return set_port_cfg_value(port_index, 0, PT_CFG_MCHECK);
}

static int cmd_debuglevel(int argc, char *const *argv)
{
	return CTL_set_debug_level(getuint(argv[1]));
}

static int cmd_set_trace(int argc, char *const *argv)
{
#define MACHINE_NUM	10
	int i, port_count, mach_index;
	BITMAP_T ports_bitmap;
	int port_index;
	char ifName[IFNAMSIZ];
	int enableflag=0;
	char const *mach_name[MACHINE_NUM] = {"pcost","edge","p2p","transmit","migrate","topoch","sttrans","roletrns","info","all"};

	if(get_bridge_name(ifName) < 0) {
		fprintf(stderr, "Error: not found bridge\n");
		return -1;
	}

	if(get_port_count(ifName, &port_count) < 0) {
		fprintf(stderr, "Error: get port count failed\n");
		return -1;
	}
		
	if('a' == argv[1][0]) {
		for(i=0; i<port_count; i++)
			BitmapSetBit(&ports_bitmap, i);
	} else {
		port_index = strtoul(argv[1], 0, 10);
		if(port_index > port_count) {
			fprintf(stderr, "Error: not found port%d\n", port_index);
			return -1;
		}
		BitmapClear(&ports_bitmap);
		BitmapSetBit(&ports_bitmap, port_index - 1);
	}

	if (strcmp(argv[3], "on")==0)
		enableflag =1;
	else if (strcmp(argv[3], "off")==0)
		enableflag =0;
	else {
		fprintf(stderr, "Error: pls use on or off\n");
		return -1;
	}

	if(strlen(argv[2]) > 8) {
		fprintf(stderr, "Error: invalid machine name\n");
		return -1;
	}

	for(mach_index=0; mach_index<MACHINE_NUM; mach_index++) {
		 if(strcmp(argv[2], mach_name[mach_index]) == 0) {
		 	CTL_set_port_trace(&ports_bitmap, mach_index, enableflag);
			break;
		 }
	}
	
	if(mach_index == MACHINE_NUM) {
		fprintf(stderr, "Error: not found machine name\n");
		return -1;
	}
	
	#if 0
    if ((strcmp(argv[2], "info") == 0) ||
     	(strcmp(argv[2], "roletrns") == 0) ||
     	(strcmp(argv[2], "sttrans") == 0) ||
     	(strcmp(argv[2], "topoch") == 0) ||
     	(strcmp(argv[2], "migrate") == 0) ||
     	(strcmp(argv[2], "transmit") == 0) ||
     	(strcmp(argv[2], "p2p") == 0) ||
     	(strcmp(argv[2], "edge") == 0) ||
     	(strcmp(argv[2], "all") == 0) ||
     	(strcmp(argv[2], "pcost") == 0)) {
		CTL_set_port_trace(&ports_bitmap, argv[2], enableflag);
    }
  	#endif
	
	return 0;
}

static const struct command commands[] = {

	{0, 0,	"showbridge",		cmd_show_bridge,			"                                    Show bridge information"},
	{0, 0,	"showport",			cmd_show_port,				"                                    Show port information"},
	{1, 0,	"showportdetail",	cmd_show_portdetail,		"<port>                              Show port information detail"},
	{1, 0,	"rstp",				cmd_rstp,					"{on|off}                            Enable/disable rstpd control"},	
	{1, 0,	"setbridgestate",	cmd_set_bridge_state,		"{on|off}                            Start/stop rstp (when enabled)"},	
	{1, 0,	"setbridgeprio",	cmd_set_bridge_prio,		"<priority>                          Set bridge priority (0-61440)"},
	{1, 0,	"sethello",			cmd_set_bridge_hello,		"<hellotime>                         Set bridge hello time (1-10s)"},
	{1, 0,	"setmaxage",		cmd_set_bridge_maxage,		"<maxage>                            Set bridge max age (6-40s)"},
	{1, 0,	"setfdelay",		cmd_set_bridge_fdelay,		"<fwd_delay>                         Set bridge forward delay (4-30s)"},
	{1, 0,	"setforcevers",		cmd_set_bridge_forcevers,	"{normal|slow}                       Normal RSTP or force to STP"},	
	{2, 0,	"setportprio",		cmd_set_port_prio,			"<port> <priority>                   Set port priority (0-255)"},
	{2, 0,	"setportpathcost",	cmd_set_port_pathcost,		"<port> <cost>                       Set port path cost (1-200000000)"},
	{2, 0,	"setportedge",		cmd_set_port_edge,			"<port> {yes|no}                     Configure if it is an edge port"},
	{2, 0,	"setportnonstp",	cmd_set_port_nonstp,		"<port> {yes|no}                     Disable STP for the port"},	
	{2, 0,	"setportp2p",		cmd_set_port_p2p,			"<port> {yes|no|auto}                Set whether p2p connection"},
	{1, 0,	"portmcheck",		cmd_portmcheck,				"<port>                              Try to get back from STP to RSTP mode"},	
	{1, 0,	"debuglevel",		cmd_debuglevel,				"<level>                             Default level=3. 1=Error 2=Info 3=Rstplib 4=Debug."},
	{3, 0,	"settrace",			cmd_set_trace,				"<port/all> <machine/all> <on/off>   Set port trace. detail see note(1)"},
	
};


const struct command *cmd_lookup(const char *cmd)
{
	int i;
	
	for (i=0; i<sizeof(commands)/sizeof(commands[0]); i++) {
		if (!strcmp(cmd, commands[i].name))
			return &commands[i];
	}
	return NULL;
}

void cmd_tokenize_free(int argc, char **argv)
{
	while (argc) free(argv[--argc]);
	free(argv);
}

int cmd_tokenize_line(const char *line, int *argc, char ***argv)
{
	int iargc = 0; char **iargv = NULL;
	char *ifs     = " \n\t";
	char *quotes  = "'\"";
	char *escapes = "\\";
	char empty = 2;		/* Empty character, will be removed from output but will mark a word. */
	int pos;
	/* Escape handle. Also escape quoted characters. */
	int escaped = 0;
	int ipos = 0;
	char quote = 0;
	char input[2*strlen(line) + 3]; /* 3 = 2 for '\n ' and 1 for \0 */
	memset(input, 0, 2*strlen(line) + 3);
	for (pos = 0; line[pos]; pos++) {
		if (line[pos] == '#' && !escaped && !quote)
			break;
		if (!escaped && strchr(escapes, line[pos]))
			escaped = 1;
		else if (!escaped && strchr(quotes, line[pos]) && !quote) {
			input[ipos++] = empty;
			input[ipos++] = '!';
			quote = line[pos];
		} else if (!escaped && quote == line[pos])
			quote = 0;
		else {
			input[ipos++] = line[pos];
			input[ipos++] = (escaped || quote)?'!':' ';
			escaped = 0;
		}
	}
	if (escaped || quote) return 1;
	/* Trick to not have to handle \0 in a special way */
	input[ipos++] = ifs[0];
	input[ipos++] = ' ';

	/* Tokenize, we don't have to handle quotes anymore */
	int wbegin  = -1;      /* Offset of the beginning of the current word */

#define CURRENT (input[2*pos])
#define ESCAPED (input[2*pos+1] != ' ')
	for (pos = 0; CURRENT; pos++) {
		if (wbegin == -1) {
			if (!ESCAPED && strchr(ifs, CURRENT))
				/* IFS while not in a word, continue. */
				continue;
			/* Start a word. */
			wbegin = pos;
			continue;
		}
		if (ESCAPED || !strchr(ifs, CURRENT))
			/* Regular character in a word. */
			continue;

		/* End of word. */
		char *word = calloc(1, pos - wbegin + 1);
		if (!word) goto error;
		int i,j;
		for (i = wbegin, j = 0;
		     i != pos;
		     i++)
			if (input[2*i] != empty) word[j++] = input[2*i];
		char **nargv = realloc(iargv, sizeof(char*) * (iargc + 1));
		if (!nargv) {
			free(word);
			goto error;
		}
		nargv[iargc++] = word;
		iargv  = nargv;
		wbegin = -1;
	}

	*argc = iargc;
	*argv = iargv;
	return 0;

error:
	cmd_tokenize_free(iargc, iargv);
	return -1;
}

static int cmd_exec(int argc, char** argv)
{
	const struct command *cmd;
	
	if(argc <= 0)
		return 1;
	if ((cmd = cmd_lookup(argv[0])) == NULL) {
		syslog(LOG_ERR, "Not found command '%s'\n", argv[0]);
		return 1;
	}
	if (argc < cmd->nargs + 1 || argc > cmd->nargs + cmd->optargs + 1) {
		syslog(LOG_ERR, "Incorrect number of arguments\n");
		return 1;
	}
	return cmd->func(argc, argv);
}

static void cmd_help()
{
	int i;
	
	printf("Usage: rstpctl [commands]\n\n");
	printf("Commands:\n");
	for (i=0; i<sizeof(commands)/sizeof(commands[0]); i++) {
		printf("        %-16s  %s\n", commands[i].name, commands[i].help);
	}
	printf("\n");
	printf("Note:\n");
	printf("    (1) machine may be folloing value:\n");
	printf("        info, roletrns, sttrans, topoch, migrate, transmit, p2p, edge, pcost, all\n");
	printf("\n");
}

int main(int argc, char** argv)
{
	const char *conf_file_name = "/var/run/rstpd.conf";
	int conf_input = 0;
	const struct command *cmd;
	int f;
	static const struct option options[] = {
		{.name = "help",.val = 'h'},
		{.name = "config_file",.val = 'c'},
		{0}
	};

	while ((f = getopt_long(argc, argv, "hc", options, NULL)) != EOF)
		switch (f) {
		case 'h':
			cmd_help();
			return 0;
		case 'c':
			conf_input = 1;
			break;			
		default:
			fprintf(stderr, "Unknown option '%c'\n", f);
			goto help;
	}

	if ((conf_input == 1) && (argc != optind)) {
		fprintf(stderr, "Invalid command '%s'\n", argv[0]);
		return 1;
	}

	if (ctl_client_init()) {
		return 1;
	}

	if(conf_input == 1) {
		syslog(LOG_INFO, "Process configuration file: %s\n", conf_file_name);
		FILE *file = fopen(conf_file_name, "r");
		if (file) {
			size_t len, offset;
			char *line;
			int cargc = 0; char **cargv = NULL;
			int i, n;
			char cmdbuf[256], *pCmd = &cmdbuf[0];
			
			while ((line = fgetln(file, &len))) {
				line = strndup(line, len);
				if (line[len - 1] == '\n') {
					line[len - 1] = '\0';
					
					n = cmd_tokenize_line(line, &cargc, &cargv);
					switch (n) {
					case -1:
						syslog(LOG_ERR, "error tokenizing for line '%s'", line);
						continue;
					case 1:
						syslog(LOG_ERR, "unmatched quotes for line '%s'", line);
						continue;
					}
					if (cargc != 0) {
						pCmd = &cmdbuf[0];
						memset(cmdbuf, 0, 256);
						offset = 0;
						for(i=0; i<cargc; i++) {
							sprintf(pCmd + offset, "%s ", cargv[i]);
							offset += strlen(cargv[i]) + 1;
							if(offset > 256) {
								syslog(LOG_INFO, "command line is too long");
								continue;
							}
						}
						syslog(LOG_INFO, "command exec: %s", cmdbuf);
						cmd_exec(cargc, cargv);
					}
					cmd_tokenize_free(cargc, cargv);
				}
				free(line);
			}
			fclose(file);
		} else {
			syslog(LOG_ERR, "unable to open %s\n", conf_file_name);
		}
		return 1;
	}

	if(argc == optind)
		goto help;
	
	argc -= optind;
	argv += optind;
	if ((cmd = cmd_lookup(argv[0])) == NULL) {
		fprintf(stderr, "Not found command '%s'\n", argv[0]);
		return 1;
	}

	if (argc < cmd->nargs + 1 || argc > cmd->nargs + cmd->optargs + 1) {
		fprintf(stderr, "Incorrect number of arguments\n");
		return 1;
	}

	return cmd->func(argc, argv);

help:
	cmd_help();
	
	return 1;
}


