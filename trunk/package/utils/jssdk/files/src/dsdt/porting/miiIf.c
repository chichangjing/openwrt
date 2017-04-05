
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
#include "miiIf.h"
#include <Copyright.h>
#include <msApiDefs.h>

struct smi_context jwCtx;
struct smi_context *pJwCtx = &jwCtx;
static const char jwIfName[IFNAMSIZ] = "eth0";

void jwMiiFree(void)
{
    close(pJwCtx->socket);
    memset(pJwCtx, 0, sizeof(struct smi_context));
}

GT_BOOL jwMiiInit(void)
{
    memset(pJwCtx, 0, sizeof(struct smi_context));

    if ((pJwCtx->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return GT_FALSE;
    }

    pJwCtx->data = (unsigned short *)(&pJwCtx->ifr.ifr_data);
    strncpy(pJwCtx->ifr.ifr_name, jwIfName, IFNAMSIZ);
    strncpy(pJwCtx->devname, jwIfName, IFNAMSIZ);
    
    pJwCtx->data[0] = 0;

    if (ioctl(pJwCtx->socket, SIOCGMIIPHY, &pJwCtx->ifr) >= 0) {
        pJwCtx->ioctl_type = SIOCGMIIREG;
    } else if (ioctl(pJwCtx->socket, SIOCDEVPRIVATE, &pJwCtx->ifr) >= 0) {
        pJwCtx->ioctl_type = SIOCDEVPRIVATE + 1;
    } else {
        fprintf(stderr, "SIOCGMIIPHY on %s failed: %s\n", jwIfName, strerror(errno));
        memset(pJwCtx, 0, sizeof(struct smi_context));
        return GT_FALSE;
    }

    pJwCtx->status = 1;
	
    return GT_TRUE;
}

GT_BOOL jwMiiRead (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg, unsigned int *value)
{
    if (!pJwCtx->status) {
        fprintf(stderr, "context not initialized\n");
        return GT_FALSE;
    }

	pJwCtx->data[0] = portNumber;
	pJwCtx->data[1] = MIIReg;

	if(ioctl(pJwCtx->socket, pJwCtx->ioctl_type, &pJwCtx->ifr) < 0) {
		fprintf(stderr, "SIOCGMIIREG on %s failed: %s, ioctl type = %x\n", 
			pJwCtx->ifr.ifr_name, strerror(errno), pJwCtx->ioctl_type);
		*value = 0xffff;
		return GT_FALSE;
	}

	*value = pJwCtx->data[3];

	return GT_TRUE;
}

GT_BOOL jwMiiWrite (GT_QD_DEV* dev, unsigned int portNumber , unsigned int MIIReg, unsigned int value)
{
    if (!pJwCtx->status) {
        fprintf(stderr, "context not initialized\n");
        return GT_FALSE;
    }

	pJwCtx->data[0] = portNumber;
	pJwCtx->data[1] = MIIReg;
	pJwCtx->data[2] = (unsigned short)(value & 0xFFFF);
	
	if (ioctl(pJwCtx->socket, pJwCtx->ioctl_type + 1, &pJwCtx->ifr) < 0) {
		fprintf(stderr, "SIOCSMIIREG on %s failed: %s\n", pJwCtx->ifr.ifr_name, strerror(errno));
        return GT_FALSE;
	}

    return GT_TRUE;	
}

