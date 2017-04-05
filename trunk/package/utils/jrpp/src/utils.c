#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <linux/un.h>
#include <linux/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <net/if.h>

#include "utils.h"

#define SYSFS_PATH_MAX 256

int netsock = -1;

static int _filter_dot(const struct dirent *entry)
{
	const char *n = entry->d_name;

	return !(n[0] == '.' && (n[1] == 0 || (n[1] == '.' && n[2] == 0)));
}

int is_bridge(char *if_name)
{
    char path[SYSFS_PATH_MAX];
	
    sprintf(path, "/sys/class/net/%s/bridge", if_name);
    return (0 == access(path, R_OK));
}

static int _isbridge(const struct dirent *entry)
{
#if 0
	char path[SYSFS_PATH_MAX];
	struct stat st;

	snprintf(path, SYSFS_PATH_MAX, "/sys/class/net/%s/bridge", entry->d_name);
	return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
#else
	return is_bridge((char *)(entry->d_name));
#endif
}

int set_bridge_stp(char *br_name, int stp_val)
{
	char path[SYSFS_PATH_MAX];
	FILE *f;
	
	snprintf(path, SYSFS_PATH_MAX, "/sys/class/net/%s/bridge", br_name);
	f = fopen(path, "w");
	if (f) {
		fprintf(f, "%d\n", stp_val);
		fclose(f);
	}

	return 0;
}

int get_hwaddr(char *ifname, unsigned char *hwaddr)
{
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(netsock, SIOCGIFHWADDR, &ifr) < 0) {
		return -1;
	}
	memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	
	return 0;
}

int get_bridge_name(char *br_name) 
{
	int bridge_count;
	struct dirent **namelist;

	bridge_count = scandir("/sys/class/net", &namelist, _isbridge, alphasort);
	if((bridge_count < 0) || (bridge_count > 1)) {
		return -1;
	}

	namelist[0]->d_name[IFNAMSIZ - 1] = '\0';
	strncpy(br_name, namelist[0]->d_name, IFNAMSIZ);
	
    free(namelist[0]);
    free(namelist);

	return 0;
}

int get_if_mac(const char *if_name, char *mac) 
{
	struct ifreq ifr;

	memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
	strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name));
	if (ioctl(netsock, SIOCGIFHWADDR, &ifr) < 0) {
		return -1;
	}
	memcpy(mac, &ifr.ifr_hwaddr.sa_data[0], 6);
	
	return 0;
}

int get_port_count(char *br_name, int *count) 
{
	int port_count;
	char path[SYSFS_PATH_MAX];
	struct dirent **namelist;
	
	sprintf(path, "/sys/class/net/%s/brif", br_name);
	port_count = scandir(path, &namelist, _filter_dot, alphasort); 
	if (port_count < 0) {
		return -1;
	}
	*count = port_count;
	
    free(namelist[0]);
    free(namelist);

	return 0;
}

int netsock_init(void)
{
	netsock = socket(AF_INET, SOCK_DGRAM, 0);
	if (netsock < 0) {
		return -1;
	}
	return 0;
}
