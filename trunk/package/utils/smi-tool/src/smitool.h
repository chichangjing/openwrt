
#ifndef __SMITOOL_H__
#define __SMITOOL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <net/if.h>

#define STRFMT_ADDR32    "%#010lX"
#define STRFMT_ADDR32_2  "0x%08lX"

#if defined(SIOCGPARAMS)  && SIOCGPARAMS != SIOCDEVPRIVATE+3
#error Changed definition for SIOCGPARAMS
#else
#define SIOCGPARAMS (SIOCDEVPRIVATE+3) 		/* Read operational parameters. */
#define SIOCSPARAMS (SIOCDEVPRIVATE+4) 		/* Set operational parameters. */
#endif

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

extern int StrToNumber(char *str , unsigned int * ulValue);
extern int smi_read(struct smi_context *ctx);
extern int smi_write(struct smi_context *ctx);
extern int smi_init(struct smi_context *ctx, const char *devname);
extern void smi_free(struct smi_context *ctx);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __STRFUNC_H__ */
