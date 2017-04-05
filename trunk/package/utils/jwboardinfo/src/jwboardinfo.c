
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "joyware.h"


int main (int argc,char **argv){

    printf("cpu=%s\n", CONFIG_JWS_CPU);
    printf("hardware_version=%s\n", CONFIG_JWS_HARDWARE_VERSION);
    printf("product_type=%s\n", CONFIG_JWS_PRODUCT_TYPE);
    printf("ports_total=%d\n", CONFIG_JWS_PORT_TOTAL_NR);
    printf("ports_optical=%d\n", CONFIG_JWS_PORT_OPTICAL_NR);
    printf("ports_ether=%d\n", CONFIG_JWS_PORT_ETHER_NR);
    printf("uid=%s\n", CONFIG_JWS_SERIAL_NUMBER);
    printf("cpu_seq=%d\n", CONFIG_JWS_PORT_CPU_SEQ);

    struct jws_port_info *op, *p;

    p = (struct jws_port_info *)malloc(sizeof(struct jws_port_info) * (CONFIG_JWS_PORT_TOTAL_NR + 1));
    if (p == NULL) {
        perror("malloc");
        exit(-1);
    }
    op = p;

    jws_port_info_get(p);
    int i = 0;
    for (i = 0; i < (CONFIG_JWS_PORT_TOTAL_NR + 1); i++, p++) {
        printf("[%d]: %d:%d:%d:%d\n", p->logic_seq, p->chip, p->speed, p->type, p->physical_seq);
    }
    
    free(op);

	return 0;
}
