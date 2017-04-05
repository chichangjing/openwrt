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
