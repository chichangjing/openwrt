

#ifndef __MII_IF_H__
#define __MII_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <net/if.h>

#define SIOCGMIIPHY 0x8947
#define SIOCGMIIREG 0x8948
#define SIOCSMIIREG 0x8949

struct smi_context {
    int socket;
    int status;
    int ioctl_type;
    char devname[IFNAMSIZ];
    struct ifreq ifr;
    unsigned int dev_addr;
    unsigned int reg_addr;
    unsigned int reg_val;
    unsigned short *data;
};
#if 0
extern int smi_read(struct smi_context *ctx);
extern int smi_write(struct smi_context *ctx);
extern int smi_init(struct smi_context *ctx, const char *devname);
extern void smi_free(struct smi_context *ctx);
#endif


#ifdef __cplusplus
}
#endif
#endif



