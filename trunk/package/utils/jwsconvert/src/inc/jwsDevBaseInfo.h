#ifndef JWSDEVBASEINFO_H
#define JWSDEVBASEINFO_H

#include  <sys/socket.h>
#ifndef _NET_IF_H
#include  <linux/if.h>
#endif /* _NET_IF_H */

#define PORT_NAME_LEN (8)
#define PORT_MAX_NUM (48)

enum {
    ENABLE = 1,
    DISABLE = 2,
};

enum PW_ALARM_STATUS{
    PW_ALARM_ENABLE = 1,
    PW_ALARM_DISABLE = 2,
};

enum PW_SUPPLY_STATUS{
    PW_SUPPLY_ON = 0,
    PW_SUPPLY_OFF = 1,
};

enum PWSTATUS{
    PW1OK_AND_PW2OK = 1,
    PW1OK_AND_PW2FAIL,
    PW1FAIL_AND_PW2OK,
    PW1FAIL_AND_PW2FAIL,
};

struct ifinfo
{
    char name[8];
    unsigned int rx_bytes;
    unsigned int rx_packets;
    unsigned int rx_errs;
    unsigned int rx_drop;
    unsigned int rx_fifo;
    unsigned int rx_frame;
    unsigned int rx_compressed;
    unsigned int rx_multicast;

    unsigned int tx_bytes;
    unsigned int tx_packets;
    unsigned int tx_errs;
    unsigned int tx_drop;
    unsigned int tx_fifo;
    unsigned int tx_frame;
    unsigned int tx_compressed;
    unsigned int tx_multicast;
};

struct stJwsDevBaseInfo
{
	unsigned int port_num;
	char port_name[PORT_MAX_NUM][PORT_NAME_LEN];
        char serial_num[25];
};

/* Context for sub-commands */
struct cmd_context {
    const char *devname;    /* net device name */
    int fd;         /* socket suitable for ethtool ioctl */
    struct ifreq ifr;   /* ifreq suitable for ethtool ioctl */
    int argc;       /* number of arguments to the sub-command */
    char **argp;        /* arguments to the sub-command */
};

#define NAME_STR_LEN (32)

typedef unsigned long long u64;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned char u8;

struct stNicStatsEntry
{
    /* mib info stats */
    char key[NAME_STR_LEN];
    u64 value;
    struct stNicStatsEntry *next;
};

struct stNicStatsTable
{
    int num;
    struct stNicStatsEntry *next;
};

/* function declarations */
int ifinfo_get_status(char *ifname, struct ifinfo *ifentry);

int getJwsDevBaseInfo(struct stJwsDevBaseInfo *jwsDevBaseInfo);
int getPortIndex(char *descr);

struct stNicStatsTable *nicStatsTableCreat(void);
int get_NIC_statistics(char *ifname, struct stNicStatsTable *head);
int get_NIC_statistics_entry(struct stNicStatsTable *head, struct stNicStatsEntry *entry);
int nicStatsTablefree(struct stNicStatsTable *head);
int nicStatsTableDisplay(struct stNicStatsTable *head);

#endif /* JWSDEVBASEINFO_H */
