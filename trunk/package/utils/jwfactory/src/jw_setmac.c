#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>

#include "jwf-tool.h"


void usage(void)
{
    printf("jw_setmac [mac]\n");
}


unsigned char a2x(const char c)  
{  
    switch(c) {  
        case '0'...'9':  
            return (unsigned char)atoi(&c);  
        case 'a'...'f':  
            return 0xa + (c-'a');  
        case 'A'...'F':  
            return 0xa + (c-'A');  
        default:  
            goto error;  
    }  

    return 0;
error:  
    return -1;  
}  
  

#define MAC_LEN_IN_BYTE 6  
int jw_str2mac(char *s, unsigned char *mac)
{
    char hi = 0, lo = 0;
    int i = 0;

    for(i = 0; i < MAC_LEN_IN_BYTE; i++) {
        hi = a2x(s[i * 3]);
        lo = a2x(s[i * 3 + 1]);
        if (hi < 0 || lo < 0) {
            printf("invalud string in mac addr\n");
            return -1;
        }
        mac[i] = (hi << 4) | lo;
        printf("mac [%d] = [%02x]\n", i, mac[i]);
    }

    return 0;
}


int main(int argc, char **argv)
{
    int ch;
    unsigned char mac[8];

    if (argc < 2) {
        struct jw_factory factory;
        jwf_raw_read(&factory);
        printf("mac = [%02x:%02x:%02x:%02x:%02x:%02x]\n", factory.jw_mac[0], factory.jw_mac[1], \
               factory.jw_mac[2], factory.jw_mac[3] ,factory.jw_mac[4], factory.jw_mac[5]);
        return 0;
    }

    if (jw_str2mac(argv[1], mac)) { 
        printf("invalid mac format\n");
        return -1;
    }

    jwf_mac_set(mac);

    return 0;
}
