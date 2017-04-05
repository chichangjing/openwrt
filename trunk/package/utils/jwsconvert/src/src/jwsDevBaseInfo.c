#include <stdio.h>
#include "jwsDevBaseInfo.h"
#include "jwslog.h"
#include <stddef.h>
#include <errno.h>

#include <sys/ioctl.h>
#include "jwsethtool-copy.h"
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include "joyware.h"

static struct jws_port_info portTable[CONFIG_JWS_PORT_TOTAL_NR + 1];

int getJwsDevBaseInfo(struct stJwsDevBaseInfo *jwsDevBaseInfo)
{
    int i;

    jwsDevBaseInfo->port_num = CONFIG_JWS_PORT_TOTAL_NR;
    strcpy(jwsDevBaseInfo->serial_num, CONFIG_JWS_SERIAL_NUMBER);

    jws_port_info_get(portTable);

    for (i = 1; i <= jwsDevBaseInfo->port_num; ++i)
    {
        switch(portTable[i].speed){
        case JWS_PORT_SPEED_MEGA_E:
            snprintf(jwsDevBaseInfo->port_name[i-1], sizeof(jwsDevBaseInfo->port_name[i-1]), "Fex-%d", i);
            break;
        case JWS_PORT_SPEED_GIGA_E:
            snprintf(jwsDevBaseInfo->port_name[i-1], sizeof(jwsDevBaseInfo->port_name[i-1]), "Gex-%d", i);
            break;
        default:
            LogWrite(ERROR, "file:%s line:%d func:%s, get port speed error\n",
                __FILENAME__, __LINE__, __FUNCTION__);
            break;
        }
    }
    
    return 0;
}

int getPortIndex(char *descr)
{
    int idx;

    if (strncmp(descr, "Gex-", strlen("Gex-")) == 0)
    {
        idx = atoi(descr + strlen("Gex-"));
    }else if(strncmp(descr, "Fex-", strlen("Fex-")) == 0)
    {
        idx = atoi(descr + strlen("Gex-"));
    }

    return idx;
}

char *pow_status_print(enum PWSTATUS pwStatus)
{
    char *pw_log_msg[4] = {"PW1OK_AND_PW2OK","PW1OK_AND_PW2FAIL",
                            "PW1FAIL_AND_PW2OK","PW1FAIL_AND_PW2FAIL"};

    return pw_log_msg[pwStatus-1];
}


int ifinfo_get_status(char *ifname, struct ifinfo *ifentry)
{
    FILE *fp;
    char line[256];

    if (!(fp = fopen("/proc/net/dev", "r"))) {
        return -2;
    }

    fgets(line, sizeof(line), fp);
    fgets(line, sizeof(line), fp);

    do{
        if(fscanf(fp," %6[^:]:%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u",
            ifentry->name,
            &(ifentry->rx_bytes), &(ifentry->rx_packets),
            &(ifentry->rx_errs), &(ifentry->rx_drop),
            &(ifentry->rx_fifo), &(ifentry->rx_frame),
            &(ifentry->rx_compressed), &(ifentry->rx_multicast),

            &(ifentry->tx_bytes), &(ifentry->tx_packets),
            &(ifentry->tx_errs), &(ifentry->tx_drop),
            &(ifentry->tx_fifo), &(ifentry->tx_frame),
            &(ifentry->tx_compressed), &(ifentry->tx_multicast))
            != 17){
            fclose(fp);
            return -1;
        }
        fgets(line, sizeof(line), fp);

    } while(strcmp(ifentry->name, ifname));

    fclose(fp);
    return 0;
}

static int send_ioctl(struct cmd_context *ctx, void *cmd)
{
    ctx->ifr.ifr_data = cmd;
    return ioctl(ctx->fd, SIOCETHTOOL, &ctx->ifr);
}

static struct ethtool_gstrings *
get_stringset(struct cmd_context *ctx, enum ethtool_stringset set_id,
          ptrdiff_t drvinfo_offset, int null_terminate)
{
    struct {
        struct ethtool_sset_info hdr;
        u32 buf[1];
    } sset_info;
    struct ethtool_drvinfo drvinfo;
    u32 len, i;
    struct ethtool_gstrings *strings;

    sset_info.hdr.cmd = ETHTOOL_GSSET_INFO;
    sset_info.hdr.reserved = 0;
    sset_info.hdr.sset_mask = 1ULL << set_id;
    if (send_ioctl(ctx, &sset_info) == 0) {
        len = sset_info.hdr.sset_mask ? sset_info.hdr.data[0] : 0;
    } else if (errno == EOPNOTSUPP && drvinfo_offset != 0) {
        /* Fallback for old kernel versions */
        drvinfo.cmd = ETHTOOL_GDRVINFO;
        if (send_ioctl(ctx, &drvinfo))
            return NULL;
        len = *(u32 *)((char *)&drvinfo + drvinfo_offset);
    } else {
        return NULL;
    }

    strings = calloc(1, sizeof(*strings) + len * ETH_GSTRING_LEN);
    if (!strings)
        return NULL;

    strings->cmd = ETHTOOL_GSTRINGS;
    strings->string_set = set_id;
    strings->len = len;
    if (len != 0 && send_ioctl(ctx, strings)) {
        free(strings);
        return NULL;
    }

    if (null_terminate)
        for (i = 0; i < len; i++)
            strings->data[(i + 1) * ETH_GSTRING_LEN - 1] = 0;

    return strings;
}

struct stNicStatsTable *nicStatsTableCreat(void)
{
    struct stNicStatsTable *head;

    head = (struct stNicStatsTable *)malloc(sizeof(struct stNicStatsTable));

    head->num = 0;
    head->next = NULL;

    return head;
}

int nicStatsEntryInsert(struct stNicStatsTable *head, 
                        struct stNicStatsEntry *entry)
{
    struct stNicStatsEntry *new_entry;
    struct stNicStatsEntry *table_entry;

    if(NULL == head){
        return -1;
    }

    new_entry = (struct stNicStatsEntry *)malloc(sizeof(struct stNicStatsEntry));
    if (new_entry == NULL) {
        return -1;
    }

    strcpy(new_entry->key, entry->key);
    new_entry->value = entry->value;
    new_entry->next = NULL;

    if(NULL == head->next){
        head->next = new_entry;
        return 0;
    }

    for(table_entry = head->next; 
        NULL != table_entry->next;
        table_entry = table_entry->next);
    table_entry->next = new_entry;

    head->num++;

    return 0;
    
}

int nicStatsTableDisplay(struct stNicStatsTable *head)
{
    struct stNicStatsEntry *table_entry;

    if(NULL == head){
        return -1;
    }

    LogWrite(DEBUG, "file:%s line:%d func:%s, number of entry: %d\n",
            __FILENAME__, __LINE__, __FUNCTION__, head->num);

    for (table_entry = head->next; NULL != table_entry; table_entry = table_entry->next)
    {
        LogWrite(DEBUG, "file:%s line:%d func:%s, %s: %llu\n",
            __FILENAME__, __LINE__, __FUNCTION__, table_entry->key, table_entry->value);
    }

    return 0;
}

int nicStatsTablefree(struct stNicStatsTable *head)
{
    struct stNicStatsEntry *table_entry;
    struct stNicStatsEntry *free_entry;

    if(NULL == head){
        return -1;
    }

    table_entry = head->next;
    free(head);

    for (free_entry = table_entry; NULL != table_entry; free_entry = table_entry)
    {
        table_entry = table_entry->next;
        free(free_entry);
    }

    return 0;
}

static int do_NIC_statistics(struct cmd_context *ctx, struct stNicStatsTable *head)
{
    struct ethtool_gstrings *strings;
    struct ethtool_stats *stats;
    unsigned int n_stats, sz_stats, i;
    int err;
    struct stNicStatsEntry entry;

    strings = get_stringset(ctx, ETH_SS_STATS,
                offsetof(struct ethtool_drvinfo, n_stats),
                0);
    if (!strings) {
        perror("Cannot get stats strings information");
        return -1;
    }

    n_stats = strings->len;
    if (n_stats < 1) {
        fprintf(stderr, "no stats available\n");
        free(strings);
        return -1;
    }

    sz_stats = n_stats * sizeof(u64);

    stats = calloc(1, sz_stats + sizeof(struct ethtool_stats));
    if (!stats) {
        fprintf(stderr, "no memory available\n");
        free(strings);
        return -1;
    }

    stats->cmd = ETHTOOL_GSTATS;
    stats->n_stats = n_stats;
    err = send_ioctl(ctx, stats);
    if (err < 0) {
        perror("Cannot get stats information");
        free(strings);
        free(stats);
        return -1;
    }

    /* todo - pretty-print the strings per-driver */
    #if 0
    fprintf(stdout, "NIC statistics:\n");
    #endif
    for (i = 0; i < n_stats; i++) {
        #if 0
        LogWrite(DEBUG, "file:%s line:%d func:%s, %s: %llu\n",
                __FILENAME__, __LINE__, __FUNCTION__,
                &strings->data[i * ETH_GSTRING_LEN],
                stats->data[i]);
        #endif
        strcpy(entry.key, (char *)&strings->data[i * ETH_GSTRING_LEN]);
        entry.value = stats->data[i];

        if(0 != nicStatsEntryInsert(head, &entry)){
            LogWrite(ERROR, "file:%s line:%d func:%s, insert entry failed\n",
                __FILENAME__, __LINE__, __FUNCTION__);
        }

        #if 0
        fprintf(stdout, "     %.*s: %llu\n",
            ETH_GSTRING_LEN,
            &strings->data[i * ETH_GSTRING_LEN],
            stats->data[i]);
        #endif
    }
    free(strings);
    free(stats);

    return 0;
}

int get_NIC_statistics(char *ifname, struct stNicStatsTable *head)
{
    struct cmd_context ctx;

    if (NULL == head) {
        return -1;
    }

    ctx.devname = ifname;
    if (strlen(ctx.devname) >= IFNAMSIZ)
        return -2;

    /* Setup our control structures. */
    memset(&ctx.ifr, 0, sizeof(ctx.ifr));
    strcpy(ctx.ifr.ifr_name, ctx.devname);

    /* Open control socket. */
    ctx.fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctx.fd < 0) {
        perror("Cannot get control socket");
        return -3;
    }

    if (0 !=do_NIC_statistics(&ctx, head)){
        /* close socket */
        shutdown(ctx.fd, SHUT_RDWR);
        close(ctx.fd);
        return -4;
    }

    /* close socket */
    shutdown(ctx.fd, SHUT_RDWR);
    close(ctx.fd);

    return 0;
}

int get_NIC_statistics_entry(struct stNicStatsTable *head, struct stNicStatsEntry *entry)
{
    struct stNicStatsEntry *table_entry;

    for (table_entry = head->next; NULL != table_entry; table_entry = table_entry->next)
    {
        if(strcmp(table_entry->key, entry->key) == 0)
        {
            entry->value = table_entry->value;
            return 0;
        }
    }

    return -1;
}
