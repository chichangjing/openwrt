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
#include "log.h"
#include "base.h"

#define JRPPD_CONF_NAME "/var/run/jrppd.conf"

struct command {
	int nargs;
	int optargs;
	const char *name;
	int (*func) (int argc, char *const *argv);
	const char *help;
};

static int update_f = 1;
static int rpp_state = RPP_ENABLED;

// 字符串转无符号整型
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

static int cmd_jrpp(int argc, char *const *argv)
{
	int r, enable;

	if (!strcmp(argv[1], "on") || !strcmp(argv[1], "yes") || !strcmp(argv[1], "1")) 
		enable = RPP_ENABLED;
	else if (!strcmp(argv[1], "off") || !strcmp(argv[1], "no") || !strcmp(argv[1], "0"))
		enable = RPP_DISABLED;
	else 
    {
		fprintf(stderr, "Invalid argument\n");
		return -1;
	}

	r = CTL_enable_jrpp(enable);
	if (r < 0) 
    {
		fprintf(stderr, "Failed to %s jrpp.\n", (enable == 1)? "enable":"disable");
		return -1;
	}

    rpp_state = enable;

	return 0;
}

static int cmd_set_topo(int argc,char *const *argv)
{
    int rc;
	int ring_id = atoi(argv[1]);
	int state;

    if(!strcmp("enable",argv[2]) || !strcmp("on",argv[2]))
    {
        state = RING_ENABLED;
    }
    else if(!strcmp("disable",argv[2]) || !strcmp("off",argv[2]))
    {
        state = RING_DISABLED;
    }
    else
    {
		fprintf(stderr, "invalid arguement\n");
        return -1;
    }

	rc = CTL_set_topo(ring_id, state);
	if (0 != rc) {
		fprintf(stderr, "set ring#%d topo fail\n", ring_id);
		return -1;
	}

    printf("set ring#%d topo %s OK\n",ring_id,(state == RING_ENABLED)?"enable":"disable");

    return 0;
}

static int cmd_show_topo(int argc,char *const *argv)
{
	int rc;
	int ring_id = getuint(argv[1]);
    if(ring_id > MAX_RING_ID)
    {
        fprintf(stderr,"Invalid arguement\n");
        return -1;
    }

    RPP_RING_TOPO_T topo;
    rc = CTL_get_ring_topo(ring_id,&topo);
	if (rc) 
    {
		fprintf(stderr, "can't get topo for ring\n");
		return -1;
	}

    printf("Ring#%d topo information...\n",ring_id);
    printf("    ================================================================================================================================================================\n");
    printf("    NodeId                RingState     RingMode     Status     NodeRole     MasterId             PrimaryPort[Stp * Neighber]               SecdPort[Stp * Neighber]\n");
    printf("    ================================================================================================================================================================\n");
    int idx;
    for(idx = 0;idx < topo.num;idx++)
    {
        printf("    %d-%02x:%02x:%02x:%02x:%02x:%02x   %s     ",\
               topo.node[idx].node_id.prio,topo.node[idx].node_id.addr[0],topo.node[idx].node_id.addr[1],topo.node[idx].node_id.addr[2],\
               topo.node[idx].node_id.addr[3],topo.node[idx].node_id.addr[4],topo.node[idx].node_id.addr[5],\
               (topo.node[idx].state == RING_ENABLED)?"Enabled":"Disabled");
        if(topo.node[idx].state == RING_ENABLED)
            printf("%s     %s     %s     %d-%02x:%02x:%02x:%02x:%02x:%02x     Port-%d[%s * %02x:%02x:%02x:%02x:%02x:%02x]     Port-%d[%s * %02x:%02x:%02x:%02x:%02x:%02x]\n",\
               (topo.node[idx].rpp_mode == RPP_ENHANCED)?"Enhanced":"Compactible",\
               (topo.node[idx].status == RPP_HEALTH)?"Health":"Fault",\
               (topo.node[idx].node_role == NODE_TYPE_MASTER)?"Master":"Transit",\
               topo.node[idx].master_id.prio,topo.node[idx].master_id.addr[0],topo.node[idx].master_id.addr[1],topo.node[idx].master_id.addr[2],\
               topo.node[idx].master_id.addr[3],topo.node[idx].master_id.addr[4],topo.node[idx].master_id.addr[5],\
               topo.node[idx].primary.port_no,(topo.node[idx].primary.stp == FORWARDING)?"Forwarding":"Blocking",\
               topo.node[idx].primary.neighber_mac[0],topo.node[idx].primary.neighber_mac[1],topo.node[idx].primary.neighber_mac[2],\
               topo.node[idx].primary.neighber_mac[3],topo.node[idx].primary.neighber_mac[4],topo.node[idx].primary.neighber_mac[5],\
               topo.node[idx].secondary.port_no,(topo.node[idx].secondary.stp == FORWARDING)?"Forwarding":"Blocking",\
               topo.node[idx].secondary.neighber_mac[0],topo.node[idx].secondary.neighber_mac[1],topo.node[idx].secondary.neighber_mac[2],\
               topo.node[idx].secondary.neighber_mac[3],topo.node[idx].secondary.neighber_mac[4],topo.node[idx].secondary.neighber_mac[5]);
        else
            printf("\n");
    }
    
    return 0;
}

static int cmd_show_ring(int argc, char *const *argv)
{
	int rc;
	int ring_id = getuint(argv[1]);
    if(ring_id > MAX_RING_ID)
    {
        fprintf(stderr,"Invalid arguement\n");
        return -1;
    }

	RPP_RING_STATE_T state;
	
    memset(&state,0,sizeof(RPP_RING_STATE_T));
	rc = CTL_get_ring_state(ring_id, &state);
	if (rc) 
    {
		fprintf(stderr, "can't get state for ring\n");
		return -1;
	}

    printf("			====================================================\n");
	printf("				Rapid Ring Protect Feature is [ %s ]    \n",(state.rpp_state == RPP_ENABLED)?"Enable":"Disable");
    printf("			====================================================\n\n");
    printf("Ring#%d information...\n",ring_id);
    printf("    RingEnable  : %s\n",(state.ring_state == RING_ENABLED)?"Enable":"Disable");

    if(state.ring_state == RING_ENABLED)
    {
        printf("    RingMode    : %s\n",(state.rpp_mode == RPP_ENHANCED)?"Enhanced":"Compatible");
        printf("    RingState   : %s\n",(state.ring_status == RPP_HEALTH)?"Health":"Fault");
        printf("    NodeRole    : %s\n",(state.node_role == NODE_TYPE_MASTER)?"Master":"Transit");
        printf("    NodeId      : %d-%02x:%02x:%02x:%02x:%02x:%02x\n",state.node_id.prio,\
               state.node_id.addr[0],state.node_id.addr[1],state.node_id.addr[2],\
               state.node_id.addr[3],state.node_id.addr[4],state.node_id.addr[5]);
        printf("    Master      : %d-%02x:%02x:%02x:%02x:%02x:%02x\n",state.master_id.prio,\
                   state.master_id.addr[0],state.master_id.addr[1],state.master_id.addr[2],\
                   state.master_id.addr[3],state.master_id.addr[4],state.master_id.addr[5]);
        printf("    NodeMachine : %s\n",state.m_node_st);
        printf("    HelloSeq    : %d\n",state.hello_seq);
    }

    RPP_PORT_STATE_T *port = &state.primary;
    printf("\n");
    printf("    =========================================================================================\n");
    printf("    RingPort    PortRole    Link      STP     AuthMachine     BallotMachine          Neighbor\n");
    printf("    =========================================================================================\n");
    if(port->port_no)
    {
        if(state.ring_state == RING_ENABLED)
        {
            printf("    Port%-4d    %s    %s    %s    %s        %s      [%02x:%02x:%02x:%02x:%02x:%02x]\n",\
                   port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary",\
                   (port->link_state == LINK_UP)?"Up":"Down",(port->dot1d_state == FORWARDING)?"Forwarding":"Blocking",\
                   port->m_auth_st,port->m_ballot_st,port->neigbor_mac[0],port->neigbor_mac[1],port->neigbor_mac[2],\
                   port->neigbor_mac[3],port->neigbor_mac[4],port->neigbor_mac[5]);
        }
        else
        {
            printf("    Port%-4d    %s\n",port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary");
        }
    }

    port = &state.secondary;
    if(port->port_no)
    {
        if(state.ring_state == RING_ENABLED)
        {
            printf("    Port%-4d    %s    %s    %s    %s        %s      [%02x:%02x:%02x:%02x:%02x:%02x]\n",\
                   port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary",\
                   (port->link_state == LINK_UP)?"Up":"Down",(port->dot1d_state == FORWARDING)?"Forwarding":"Blocking",\
                   port->m_auth_st,port->m_ballot_st,port->neigbor_mac[0],port->neigbor_mac[1],port->neigbor_mac[2],\
                   port->neigbor_mac[3],port->neigbor_mac[4],port->neigbor_mac[5]);
        }
        else
        {
            printf("    Port%-4d    %s\n",port->port_no,(port->role == PRIMARY_PORT)?"primary":"secondary");
        }
    }

	return 0;	
}

static int cmd_add_ring(int argc, char *const *argv)
{
	int rc;
	int ring_id = getuint(argv[1]);
    if(ring_id > MAX_RING_ID)
    {
        fprintf(stderr,"Invalid arguement\n");
        return -1;
    }
	
	rc = CTL_add_ring(ring_id);
	if (rc) 
    {
		fprintf(stderr, "add ring#%d failed\n", ring_id);
		return -1;
	}
	return 0;
}

static int cmd_del_ring(int argc, char *const *argv)
{
	int rc;
	int ring_id = getuint(argv[1]);
	
	rc = CTL_del_ring(ring_id);
	if (rc) 
    {
		fprintf(stderr, "delete ring#%d failed\n", ring_id);
		return -1;
	}
	return 0;
}

static int cmd_add_ringport(int argc, char *const *argv)
{
	int rc;
	int ring_id         = getuint(argv[1]);
	int primary_port    = getuint(argv[2]);
	int secondary_port  = getuint(argv[3]);
    if(ring_id > MAX_RING_ID || primary_port > MAX_PORT_NUM || secondary_port > MAX_PORT_NUM)
    {
        fprintf(stderr,"Invalid arguement\n");
        return -1;
    }
	
	rc = CTL_add_ringport(ring_id, primary_port, secondary_port);
	if (rc) 
    {
		fprintf(stderr, "add ringport failed for ring#%d\n", ring_id);
		return -1;
	}
	return 0;
}

static int cmd_del_ringport(int argc, char *const *argv)
{
	int rc;
	int ring_id = getuint(argv[1]);
	
	rc = CTL_del_ringport(ring_id);
	if (rc) 
    {
		fprintf(stderr, "del ringport failed for ring#%d\n", ring_id);
		return -1;
	}
	return 0;
}

static int cmd_set_nodeprio(int argc, char *const *argv)
{
    int rc;
    int ring_id = atoi(argv[1]);
    unsigned char prio;

    if(!strcmp("low",argv[2]))
        prio = 2;
    else if(!strcmp("medium",argv[2]))
        prio = 1;
    else if(!strcmp("high",argv[2]))
        prio = 0;
    else
    {
        fprintf(stderr,"invalid arguement: [%s]!\n",argv[2]);
        return -1;
    }

	RPP_RING_CFG_T ring_cfg = {
        .node_priority      = prio,
        .field_mask         = RING_CFG_PRIO
    };

	rc = CTL_set_ring_config(ring_id, &ring_cfg);
    if(rc)
    {
		fprintf(stderr, "set config failed for ring#%d\n", ring_id);
        return -1; 
    }
	return 0;
}

static int cmd_set_ringmode(int argc, char *const *argv)
{
    int rc;
	int ring_id = atoi(argv[1]);
	int enable;
	
    if(!strcmp("enable",argv[2]) || !strcmp("on",argv[2]))
    {
        enable = RING_ENABLED;
    }
    else if(!strcmp("disable",argv[2]) || !strcmp("off",argv[2]))
    {
        enable = RING_DISABLED;
    }
    else
    {
		fprintf(stderr, "invalid arguement\n");
        return -1;
    }

	rc = CTL_enable_ring(ring_id, enable);
	if (0 != rc) {
		fprintf(stderr, "set ring#%d mode fail\n", ring_id);
		return -1;
	}

    if(enable == RING_ENABLED)
        update_f = 0;

	return 0;
}

static int cmd_debuglevel(int argc, char *const *argv)
{
	return CTL_set_debug_level(getuint(argv[1]));
}

static const struct command commands[] = {
	{1, 0,	"jrpp",             cmd_jrpp,               "{on|off}                                           Enable/disable jrpp protocol"},			
	{1, 0,	"showring",         cmd_show_ring,          "<ring_id>                                          Show jrpp ring"},
	{1, 0,	"showtopo",         cmd_show_topo,          "<ring_id>                                          Show ring topo"},
	{2, 0,	"settopo",          cmd_set_topo,           "<ring_id> {enable|didsble}                         Set ring topo enable/disable"},
	{1, 0,	"addring",          cmd_add_ring,           "<ring_id>                                          Add a jrpp ring (0~65535)"},
	{1, 0,	"delring",          cmd_del_ring,           "<ring_id>                                          Delete a jrpp ring"},
	{3, 0,	"addringport",      cmd_add_ringport,       "<ring_id> <primary_port> <secondary_port>          Add ringport for jrpp ring"},
	{1, 0,	"delringport",      cmd_del_ringport,       "<ring_id>                                          Delete ringport for jrpp ring"},
	{2, 0,	"setnodeprio",      cmd_set_nodeprio,       "<ring_id> <priority>                               Set node priority"},
	{2, 0,	"setringmode",      cmd_set_ringmode,       "<ring_id> {enable|disable}                         Enable/disable jrpp ring"},		
	{1, 0,	"debuglevel",       cmd_debuglevel,         "<level>                                            Default level=4. 0=None 1=Error 2=Info 3=Jrpp 4=Debug"},
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
		fprintf(stderr, "Not found command '%s'\n", argv[0]);
		return 1;
	}
	if (argc < cmd->nargs + 1 || argc > cmd->nargs + cmd->optargs + 1) {
		fprintf(stderr, "Incorrect number of arguments\n");
		return 1;
	}
		
	return cmd->func(argc, argv);
}

static void cmd_help()
{
	int i;
	
	printf("Usage: jrppctl [commands]\n\n");
	printf("Commands:\n");
	for (i=0; i<sizeof(commands)/sizeof(commands[0]); i++) {
		printf("        %-12s  %s\n", commands[i].name, commands[i].help);
	}
	printf("\n");
}

static void load_config(void)
{
    FILE *file = fopen(JRPPD_CONF_NAME, "r");
    if (file) 
    {
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
                    fprintf(stderr, "error tokenizing for line '%s'", line);
                    continue;
                case 1:
                    fprintf(stderr, "unmatched quotes for line '%s'", line);
                    continue;
                }
                if (cargc != 0) 
                {
                    memset(cmdbuf, 0, 256);
                    offset = 0;
                    for(i=0; i<cargc; i++) {
                        sprintf(pCmd + offset, "%s ", cargv[i]);
                        offset += strlen(cargv[i]) + 1;
                        if(offset > 256) {
                            fprintf(stderr, "command line is too long");
                            continue;
                        }
                    }
                    log_info("command exec: %s", cmdbuf);
                    cmd_exec(cargc, cargv);
                }
                cmd_tokenize_free(cargc, cargv);
            }
            free(line);
        }
        fclose(file);

        /* update ports flag is none-zero means no ring enable
         * should update ports manually
         */
        if(update_f || rpp_state == RPP_DISABLED)
            CTL_update_ports();
    } 
    else 
    {
		fprintf(stderr, "unable to open %s\n", JRPPD_CONF_NAME);
	}
}

int main(int argc, char** argv)
{
	int conf_input = 0;
	int f;
	static const struct option options[] = {
		{.name = "help",.val = 'h'},
		{.name = "config_file",.val = 'c'},
		{0}
	};
    
	log_init("jrppctl",LOG_USER);
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

	if(conf_input == 1) 
    {
        log_info("Process configuration file: %s\n", JRPPD_CONF_NAME);
        load_config();
        return 0;
	}

	if(argc == optind)
		goto help;
	
	argc -= optind;
	argv += optind;

    cmd_exec(argc, argv);
    return 0;

help:
	cmd_help();
	
	return 1;
}

