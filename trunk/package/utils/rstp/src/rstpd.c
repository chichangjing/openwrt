
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

#include "jssdk_mode.h"
#include "fal/fal.h"

#include "log.h"
#include "utils.h"
#include "epoll_loop.h"
#include "packet.h"
#include "ctl_server.h"
#include "rstp_if.h"

#define MODULE_NAME			"rstpd"
#define MODULE_PID_FILE		"/var/run/rstpd.pid"
#define MODULE_CTL_PATH		"/usr/sbin/rstpctl"

static int become_daemon = 1;
static int is_daemon = 0;
int log_level = LOG_LEVEL_MODULE;

static pid_t load_configure(void)
{
	pid_t rstpctl = vfork();
	int devnull;
	const char *rstpctl_path = MODULE_CTL_PATH;
	
	switch (rstpctl) {
	case -1:
		log_err("unable to fork");
		return -1;
	case 0:
		if ((devnull = open("/dev/null", O_RDWR, 0)) != -1) {
			dup2(devnull,   STDIN_FILENO);
			dup2(devnull,   STDOUT_FILENO);
			if (devnull > 2) 
				close(devnull);
			
			log_info("execute \"rstpctl -c\" ...");
			execl(rstpctl_path, "rstpctl", "-c", (char *)NULL);
		}
		_exit(127);
		break;
	default:
		return rstpctl;
	}
	
	return -1;
}

static void usage(void)
{
	printf("rstpd [option]\n\n");
	printf("   -d          don't running as daemon\n");
	printf("   -v <level>  log level\n");
	printf("\n");
}

int main(int argc, char** argv)
{
	int c;
	
	while ((c = getopt(argc, argv, "dv:")) != -1) {
		switch (c) {
		case 'd':
			become_daemon = 0;
			break;
		case 'v':
			{
				char *end;
				long l;
				
				l = strtoul(optarg, &end, 0);
				if (*optarg == 0 || *end != 0 || l > LOG_LEVEL_MAX) {
					fprintf(stderr, "Invalid loglevel %s", optarg);
					exit(1);
				}
				log_level = l;
			}
			break;
		default:
			fprintf(stderr, "Unknown option '%c'\n", c);
			goto help;
		}
	}

	if (become_daemon) {
		#if 1
		FILE *f = fopen(MODULE_PID_FILE, "w");
		if (!f) {
			syslog(LOG_ERR, "unable to open %s", MODULE_PID_FILE);
			return -1;
		}
		log_init(MODULE_NAME);
	    if (daemon(0,0) < 0) {
	        syslog(LOG_ERR, "daemon() failed\n");
	        return -1;
	    }
		is_daemon = 1;
		fprintf(f, "%d\n", getpid());
		fclose(f);
		#else
		int pid;
		char *spid;

		
		
		if (daemon(0, 0) != 0){
	        syslog(LOG_ERR, "daemon() failed\n");
	        return -1;
	    }
		
		if ((pid = open(MODULE_PID_FILE, O_TRUNC | O_CREAT | O_WRONLY, 0666)) == -1) {
	        syslog(LOG_ERR, "unable to open pid file %s", MODULE_PID_FILE);
	        return -1;
	    }

		if (asprintf(&spid, "%d\n", getpid()) == -1) {
	        syslog(LOG_ERR, "unable to create pid file %s", MODULE_PID_FILE);
	        return -1;
	    }

		if (write(pid, spid, strlen(spid)) == -1) {
	        syslog(LOG_ERR, "unable to write pid file %s", MODULE_PID_FILE);
	        return -1;
	    }

		syslog(LOG_INFO, "pid = %s", spid);
		
		free(spid);
		close(pid);		
		#endif
	}
	
	log_info("\033[1;32m[ %s starting ... ]\033[0m", MODULE_NAME);

    if (1)
	{
		ssdk_init_cfg init_cfg = def_init_cfg;
		
	    init_cfg.reg_mode = HSL_MDIO;
	    init_cfg.reg_func.mdio_set = NULL;
	    init_cfg.reg_func.mdio_get = NULL;
		init_cfg.en_init_chip = 0;
		if(ssdk_init(0, &init_cfg) != SW_OK) {
			log_err("error: ssdk_init failed\n");
			exit(1);
		}

		if(fal_mode_tag(0) != SW_OK) {
			log_err("error: fal_mode_tag() failed\n");
			exit(1);
		}
	}

	if(netsock_init() < 0)    { log_err("error: netsock_init() failed\n");exit(1); }
	if(br_info_init() < 0)    { log_err("error: rstp_setup() failed\n");exit(1); }		
	if(init_epoll() < 0)      { log_err("error: init_epoll() failed\n"); exit(1); }
	if(ctl_server_init() < 0) { log_err("error: ctl_server_init() failed\n");exit(1); }
	if(pdu_sock_init() < 0)   { log_err("error: pdu_sock_init() failed\n");exit(1); }
	if(init_bridge_ops() < 0) { log_err("error: init_bridge_ops() failed\n");exit(1); }

	log_info("%s initialize successfully.\n", MODULE_NAME);
	
	signal(SIGPIPE, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	/* Loading configuration */
	if(load_configure() == -1)
		log_err("load_configure failed");
	
	return epoll_main_loop();

help:
	usage();
	return -1;
}

void vDprintf(int level, const char *fmt, va_list ap)
{
	if (level > log_level)
		return;

	if (!is_daemon) {
		char logbuf[256];
		logbuf[255] = 0;
		time_t clock;
		struct tm *local_tm;
		time(&clock);
		local_tm = localtime(&clock);
		int l = strftime(logbuf, sizeof(logbuf) - 1, "%F %T ", local_tm);
		vsnprintf(logbuf + l, sizeof(logbuf) - l - 1, fmt, ap);
		printf("%s\n", logbuf);
	} else {
		//vsyslog((level <= LOG_LEVEL_INFO) ? LOG_INFO : LOG_DEBUG, fmt, ap);
		vsyslog(LOG_INFO, fmt, ap);
	}
}


