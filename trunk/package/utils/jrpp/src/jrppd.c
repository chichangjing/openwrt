#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <getopt.h>
#include <syslog.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>


#include "jssdk_mode.h"
#include "fal/fal.h"

#include "log.h"
#include "utils.h"
#include "ctl_server.h"
#include "packet.h"
#include "jrpp_if.h"
#include "jrppd.h"
#include "librtnl.h"
#include "ubus_client.h"

#define MODULE_NAME			"jrppd"
#define MODULE_PID_FILE		"/var/run/jrppd.pid"
#define MODULE_CTL_PATH		"/usr/sbin/jrppctl"

static pid_t load_configure(void)
{
	pid_t jrppctl = vfork();
	int devnull;
	const char *jrppctl_path = MODULE_CTL_PATH;
	
	switch (jrppctl) {
	case -1:
		LOG_ERROR("unable to fork");
		return -1;
	case 0:
		if ((devnull = open("/dev/null", O_RDWR, 0)) != -1) {
			dup2(devnull,   STDIN_FILENO);
			dup2(devnull,   STDOUT_FILENO);
			if (devnull > 2) 
				close(devnull);
			
			log_info("execute \"jrppctl -c\" ...");
			execl(jrppctl_path, "jrppctl", "-c", (char *)NULL);
		}
		_exit(127);
		break;
	default:
		return jrppctl;
	}
	
	return -1;
}

static int jssdk_init(void)
{
    ssdk_init_cfg init_cfg = {
        .reg_mode = HSL_MDIO,
        .reg_func.mdio_set = NULL,
        .reg_func.mdio_get = NULL,
        .en_init_chip = 0,
    };

    if(ssdk_init(0, &init_cfg) != SW_OK) 
    {
        return -1;
    }

    return 0;
}


static void usage(void)
{
	printf("jrppd [option]\n\n");
	printf("   -d          don't running as daemon\n");
	printf("   -v <level>  log level\n");
	printf("\n");
}


int main(int argc, char** argv)
{
	int c;
    int become_daemon = 1;
	
	while ((c = getopt(argc, argv, "dv:")) != -1) {
		switch (c) {
		case 'd':
			become_daemon = 0;
            log_set_channel(become_daemon);
			break;
		case 'v':
        {
            char *end;
            int level;
         
			level = strtoul(optarg, &end, 0);
            if (*optarg == 0 || *end != 0 || level > LOG_LEVEL_MAX) {
                fprintf(stderr, "Invalid loglevel %s", optarg);
                exit(1);
            }
            log_threshold(level);
        }
			break;
		default:
			fprintf(stderr, "Unknown option '%c'\n", c);
			goto help;
		}
	}

	if (become_daemon) {
		FILE *f = fopen(MODULE_PID_FILE, "w");
		if (!f) {
			syslog(LOG_ERR, "unable to open %s", MODULE_PID_FILE);
			return -1;
		}
		log_init(MODULE_NAME,LOG_DAEMON);
	    if (daemon(0,0) < 0) {
	        syslog(LOG_ERR, "daemon() failed\n");
	        return -1;
	    }
		fprintf(f, "%d\n", getpid());
		fclose(f);
	}
	
	log_info("[ %s starting ... ]", MODULE_NAME);

    if(uloop_init() < 0)
    {
        LOG_ERROR("create epoll fail");
        return -1;
    }

    if(ubus_client_init() < 0)  { LOG_ERROR("error: init ubus client failed\n");  exit(1); }
	if(jssdk_init() < 0)        { LOG_ERROR("error: jssdk_init() failed\n");      exit(1); }
	if(netsock_init() < 0)      { LOG_ERROR("error: netsock_init() failed\n");    exit(1); }
	if(rpp_init() < 0)          { LOG_ERROR("error: rpp_init() failed\n");        exit(1); }
	if(ctl_server_init() < 0)   { LOG_ERROR("error: ctl_server_init() failed\n"); exit(1); }
	if(pdu_sock_init() < 0)     { LOG_ERROR("error: pdu_sock_init() failed\n");   exit(1); }
	if(rtnl_init(RTMGRP_LINK) < 0)  { LOG_ERROR("error: netlink init failed\n");      exit(1); }
    
	log_info("%s initialize successfully.\n", MODULE_NAME);
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
    signal(SIGTERM,sig_kill_handler);

	/* Loading configuration */
	if(load_configure() == -1)
    {
		LOG_ERROR("load_configure failed");
        exit(1);
    }

    uloop_run();

	return 0;

help:
	usage();
	return -1;
}


