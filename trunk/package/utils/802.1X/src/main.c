#include "unixsock.h"
#include "notify.h"
#include "ubus.h"
#include "conf.h"
#include "iface.h"

#include "eap_common/eap_register.h"


/**<    name rule: JW.year|month.01(main ver).01(sub ver) */
#define VERSION "JW.201703.0.01"    
#define MODULE_NAME "802.1X Authenticator"
#define MODULE_PID_FILE "/var/run/nas.pid"

struct ieee802_1x_g {
    eAuthState state;
    struct ieee802_1x_conf conf;
}ifaces = {
    .state = AUTH_DISABLED,
};

static int usage(const char *prog)
{
	LOG_ERROR("Usage: %s [options]\n"
		"Options:\n"
		"\t-d <level>\tEnable debug messages\n"
		"\t-u <level>\tSelect channel to commit log\n"
		"\n", prog);
	return 1;
}

static int ifaces_read_configure(void)
{
    if(ubus_init() < 0)
    {
        LOG_ERROR("ubus init fail");
        return -1;
    }
    
    ubus_get_8021x_status(&ifaces.state);
    LOG_NORMAL("globle 802.1x state = %s",(ifaces.state == AUTH_ENABLED)?"ENABLED":"DISABLED");

    if(ifaces.state == AUTH_ENABLED)
        ubus_get_8021x_conf(&ifaces.conf);

    return 0;
}

static int ifaces_init(void)
{
    if (eap_server_register_methods())
    {
        LOG_ERROR("regist EAP methods error");
        return -1;
    }

    if(ifaces_read_configure())
    {
        LOG_ERROR("read 802.1X config file error");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
	int ch;
    int level,channel;
    
	while ((ch = getopt(argc, argv, "vu:d:s:")) != -1) {
		switch (ch) {
		case 'd':
			level = atoi(optarg);
            log_threshold(level);
			break;
		case 'u':
			channel = atoi(optarg);
            log_set_channel(channel);
			break;
        case 'v':
            printf("JOYWARE 802.1X version: %s\n",VERSION);
            return 0;
		default:
			return usage(argv[0]);
		}
	}

    log_init(MODULE_NAME);

    LOG_NORMAL("802.1X starting...");
    if(daemon(0,0) < 0)
    {
        LOG_ERROR("daemon error");
        return -1;
    }

    FILE *file = fopen(MODULE_PID_FILE, "w");
    if(file == NULL)
    {
        LOG_ERROR("unable to open %s", MODULE_PID_FILE);
        return -1;
    }
    fprintf(file, "%d\n", getpid());
    fclose(file);

    if(uloop_init() < 0)
    {
        LOG_ERROR("create epoll fail");
        return -1;
    }

	if(ifaces_init() < 0)
    {
        LOG_ERROR("802.1X init error");
        return -1;
    }
    LOG_NORMAL("802.1X init OK");
    LOG_NORMAL("");

    uloop_run();
	return 0;
}
