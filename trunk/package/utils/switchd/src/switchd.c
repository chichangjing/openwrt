#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/reboot.h>

#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

#include "switchd.h"
#include <syslog.h>

#define def_init_cfg  {.reg_mode = HSL_MDIO};

ssdk_init_cfg init_cfg = def_init_cfg;

/**<    name rule: JW.year|month.01(main ver).01(sub ver) */
#define VERSION "JW.201608.01.01"    

static int usage(const char *prog)
{
	LOG_ERROR("Usage: %s [options]\n"
		"Options:\n"
		"\t-s <path>\tPath to ubus socket\n"
		"\t-h <path>\trun as hotplug daemon\n"
		"\t-d <level>\tEnable debug messages\n"
		"\t-u <level>\tSelect channel to commit log\n"
		"\n", prog);
	return 1;
}

int main(int argc, char **argv)
{
	int ch;
	char *dbglvl = getenv("DBGLVL");
    unsigned int debug = LOG_ERR;
    int channel = ULOG_STDIO;
    
	if (dbglvl) {
		debug = atoi(dbglvl);
		unsetenv("DBGLVL");
	}

	while ((ch = getopt(argc, argv, "vu:d:s:")) != -1) {
		switch (ch) {
		case 's':
			ubus_socket = optarg;
			break;
		case 'd':
			debug = atoi(optarg);
			break;
		case 'u':
			channel = atoi(optarg);
			break;
        case 'v':
            printf("switchd version: %s\n",VERSION);
            return 0;
		default:
			return usage(argv[0]);
		}
	}

	ulog_open(channel, LOG_USER, "switched");
    //openlog("switchd",LOG_CONS | LOG_PID,LOG_LOCAL1);


    LOG_SET_THRESHOLD(debug);

    /*  jssdk init */ 
    init_cfg.reg_mode = HSL_MDIO;
    init_cfg.reg_func.mdio_set = NULL;
    init_cfg.reg_func.mdio_get = NULL;
    init_cfg.en_init_chip = 0;
    ssdk_init(0, &init_cfg);

    /*****< UCI configure backup */
//    system("[ -f /etc/config/.switchd ] || cp /etc/config/switchd /etc/config/.switchd");



	//setsid();
	uloop_init();
	//switchd_signal();
	
	switchd_connect_ubus();

    checkFpgaVersion();

	uloop_run();
	uloop_done();

	return 0;
}
