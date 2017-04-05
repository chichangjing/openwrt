#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "crpp.h"

static int is_bridge(char *if_name)
{
    char path[255];
    sprintf(path, "/sys/class/net/%s/bridge", if_name);
    return (0 == access(path, R_OK));
}

static int _isbridge(const struct dirent *entry)
{
    return is_bridge((char *)(entry->d_name));
}

static int get_bridge_name(char *br_name)
{
    int bridge_count;
    struct dirent **namelist;
    bridge_count = scandir("/sys/class/net", &namelist, _isbridge, alphasort);
    if((bridge_count < 0) || (bridge_count > 1))
        return -1;

    namelist[0]->d_name[IFNAMSIZ - 1] = '\0';
    strncpy(br_name, namelist[0]->d_name, IFNAMSIZ);
    free(namelist[0]);
    free(namelist);

    return 0;
}

static int get_if_mac(int fd,const char *if_name, char *mac)
{
    struct ifreq ifr;
    memset(ifr.ifr_name, 0, sizeof(ifr.ifr_name));
    strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name));
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0)
        return -1;
    memcpy(mac, &ifr.ifr_hwaddr.sa_data[0], 6);

    return 0;
}

int bridge_init(crpp_t *p)
{
    int netsock = socket(AF_INET, SOCK_DGRAM, 0);
    if(netsock < 0)
        return -1;

    if(get_bridge_name(p->if_name))
    {
        LOG_ERROR("get bridge name fail");
        return -1;
    }

    if(get_if_mac(netsock,p->if_name,p->mac))
    {
        LOG_ERROR("get interface mac fail");
        return -1;
    }

    return 0;
}
