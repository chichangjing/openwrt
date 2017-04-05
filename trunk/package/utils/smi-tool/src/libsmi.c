#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#ifdef use_linux_libc5
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#endif
#include "smitool.h"


void smi_free(struct smi_context *ctx)
{
    close(ctx->socket);
    memset(ctx, 0, sizeof(struct smi_context));
}


/* return 0 for success, others for fail*/
int smi_init(struct smi_context *ctx, const char *devname)
{
    if ((strlen(devname) > IFNAMSIZ) || !(strlen(devname))) {
        fprintf(stderr, "devname [%s] maybe invalid\n", devname);
    }

    memset(ctx, 0, sizeof(struct smi_context));

    if ((ctx->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    ctx->data = (unsigned short *)(&ctx->ifr.ifr_data);
    strncpy(ctx->ifr.ifr_name, devname, IFNAMSIZ);
    strncpy(ctx->devname, devname, IFNAMSIZ);
    
    ctx->data[0] = 0;

    if (ioctl(ctx->socket, SIOCGMIIPHY, &ctx->ifr) >= 0) {
        ctx->ioctl_type = SIOCGMIIREG;
    } else if (ioctl(ctx->socket, SIOCDEVPRIVATE, &ctx->ifr) >= 0) {
        ctx->ioctl_type = SIOCDEVPRIVATE + 1;
    } else {
        fprintf(stderr, "SIOCGMIIPHY on %s failed: %s\n", devname, strerror(errno));
        memset(ctx, 0, sizeof(struct smi_context));
        return -1;
    }

    ctx->status = 1;
    return 0;
}


int smi_read(struct smi_context *ctx)
{
    if (!ctx->status) {
        fprintf(stderr, "context not initialized\n");
        return -1;
    }

	ctx->data[0] = ctx->dev_addr;
	ctx->data[1] = ctx->reg_addr;
    printf("dev_addr %x, reg_addr %x\n", ctx->data[0], ctx->data[1]);

	if (ioctl(ctx->socket, ctx->ioctl_type, &ctx->ifr) < 0) {
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s, ioctl type = %x\n", ctx->ifr.ifr_name, strerror(errno), ctx->ioctl_type);
		return -1;
	}

    ctx->reg_val = ctx->data[3];

	return 0;
}


int smi_write(struct smi_context *ctx)
{
    if (!ctx->status) {
        fprintf(stderr, "context not initialized\n");
        return -1;
    }

	ctx->data[0] = ctx->dev_addr;
	ctx->data[1] = ctx->reg_addr;
	ctx->data[2] = ctx->reg_val;

	if (ioctl(ctx->socket, ctx->ioctl_type + 1, &ctx->ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", ctx->ifr.ifr_name, strerror(errno));
        return -1;
	}

    return 0;
}

#if 0

int main(int argc, char **argv)
{
    unsigned int dev_addr, reg_addr, reg_val;

    struct smi_context ctx;

    if (smi_init(&ctx, "eth0")) {
        fprintf(stderr, "smi init fail\n");
        return -1;
    }
    
    if (argc == 3) {
        if (StrToNumber(argv[1], &dev_addr)) {
            goto parse_error;
        }

        if (StrToNumber(argv[2], &reg_addr)) {
            goto parse_error;
        }

        ctx.dev_addr = dev_addr;
        ctx.reg_addr = reg_addr;
        smi_read(&ctx);
        printf("read data: %x\n", ctx.reg_val);

    } else if (argc == 4) {
        if (StrToNumber(argv[1], &dev_addr)) {
            goto parse_error;
        }

        if (StrToNumber(argv[2], &reg_addr)) {
            goto parse_error;
        }

        if (StrToNumber(argv[3], &reg_val)) {
            goto parse_error;
        }

        ctx.dev_addr = dev_addr;
        ctx.reg_addr = reg_addr;
        ctx.reg_val = reg_val;

        smi_write(&ctx);
    } else {
        fprintf(stderr, "invalid argument !\n");
        fprintf(stdout, "read: smi-tool [phy addr] [reg addr]\n");
        fprintf(stdout, "write: smi-tool [phy addr] [reg addr] [reg_val]\n");
        return -1;
    }

    smi_free(&ctx);

    return 0;

parse_error:
    fprintf(stderr, "parse argv error !\n");
    return -1;
}

#endif
