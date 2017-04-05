#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mtd/mtd-user.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include "jwf-tool.h"


int jwf_erase_partition(int fd)
{
	mtd_info_t meminfo;
	erase_info_t erase;
	struct stat st;

	fstat(fd, &st);
	if (!S_ISCHR(st.st_mode))
		printf("%s: not a char device", MTD_DEV_PATH);

	ioctl(fd, MEMGETINFO, &meminfo);
	erase.length = meminfo.erasesize;

	for (erase.start = 0; erase.start < meminfo.size;
	     erase.start += meminfo.erasesize) {

        int ret;
        int offset = erase.start;

        ret = ioctl(fd, MEMGETBADBLOCK, &offset);
        if (ret > 0) {
            printf("\nSkipping bad block at 0x%08x", erase.start);
            continue;
        }
        if (ret < 0) {
            printf("MEMGETBADBLOCK error");
        }

		ioctl(fd, MEMERASE, &erase);
	}

    return 0;
}


int jwf_raw_read(struct jw_factory *jwfs)
{
    int fd = 0;
    int ret = 0;
    struct jw_factory factory;

    if (jwfs == NULL) {
        printf("invalid argument\n");
        return -1;
    }

    fd = open(MTD_DEV_PATH, O_RDONLY);
    if (fd < 0) {
        perror(MTD_DEV_PATH);
        return -1;
    }

    ret = read(fd, &factory, sizeof(struct jw_factory));
    if (ret < 0) {
        perror("read");
        goto EXIT;
    }

    if (factory.jw_mac[0] == 0xff) 
        memset(factory.jw_mac, 0x00, MAX_MAC_LEN);

    if (factory.jw_uid[0] == 0xff)
        memset(factory.jw_uid, 0x00, MAX_UID_LEN);
    
    if (factory.jw_hardware_ver[0] == 0xff) 
        memset(factory.jw_hardware_ver, 0x00, MAX_HDV_LEN);

    if (factory.jw_product_type[0] == 0xff) 
        memset(factory.jw_product_type, 0x00, MAX_PRDT_LEN);

    memcpy(jwfs, &factory, sizeof(struct jw_factory));

    close(fd);
    return 0;

EXIT:
    close(fd);
    return -1;
}


int jwf_raw_write(struct jw_factory *jwfs)
{
    int fd = 0;
    int ret = 0;

    if (jwfs == NULL) {
        printf("invalid argument\n");
        return -1;
    }

    fd = open(MTD_DEV_PATH, O_RDWR);
    if (fd < 0) {
        perror(MTD_DEV_PATH);
        return -1;
    }

    jwf_erase_partition(fd);

    int  i = 0;
    for (i = 0; i < sizeof(struct jw_factory); i++) {
        write(fd, 0x00, sizeof(struct jw_factory));
    }

    lseek(fd, 0, SEEK_SET);

    ret = write(fd, jwfs, sizeof(struct jw_factory));
    if (ret < 0) {
        perror("read");
        goto EXIT;
    }

    close(fd);
    return 0;

EXIT:
    close(fd);

    return -1;
}


int jwf_uid_get(char *uid,int max) 
{
    struct jw_factory factory;
    int ret = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    if(strlen(factory.jw_uid) > max)
        return -1;

    strcpy(uid,factory.jw_uid);
    //printf("uid = %s\n", factory.jw_uid);

    return 0;
}


int jwf_uid_set(const char *uid)
{
    struct jw_factory factory;
    int ret = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    memset(&factory.jw_uid, 0, 32);
    memcpy(&factory.jw_uid, uid, 32);

    ret = jwf_raw_write(&factory);
    if (ret < 0) {
        printf("write uid fail\n");
        return -1;
    }

    return 0;
}


int jwf_mac_get(unsigned char *mac)
{
    struct jw_factory factory;
    int ret = 0;
    int i = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    memcpy(mac,factory.jw_mac,6);
    printf("mac = ");
    for (i = 0; i < 8; i++) {
        printf("%02x:", factory.jw_mac[i]);
    }
    printf("\n");
    return 0;
}


int jwf_mac_set(const unsigned char *mac)
{
    struct jw_factory factory;
    int ret = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    memset(&factory.jw_mac, 0, 8);
    memcpy(&factory.jw_mac, mac, 8);

    ret = jwf_raw_write(&factory);
    if (ret < 0) {
        printf("write mac fail\n");
        return -1;
    }

    return 0;
}


int jwf_prdt_get(unsigned char *prdt)
{
    struct jw_factory factory;
    int ret = 0;
    int i = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    memcpy(prdt,factory.jw_product_type,MAX_PRDT_LEN);

    return 0;
}


int jwf_prdt_set(const unsigned char *prdt)
{
    struct jw_factory factory;
    int ret = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    memset(&factory.jw_product_type, 0, MAX_PRDT_LEN);
    memcpy(&factory.jw_product_type, prdt, MAX_PRDT_LEN);

    ret = jwf_raw_write(&factory);
    if (ret < 0) {
        printf("write product type fail\n");
        return -1;
    }

    return 0;
}


int jwf_hdv_get(unsigned char *hdv)
{
    struct jw_factory factory;
    int ret = 0;
    int i = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    memcpy(hdv,factory.jw_hardware_ver,MAX_HDV_LEN);

    return 0;
}


int jwf_hdv_set(const unsigned char *hdv)
{
    struct jw_factory factory;
    int ret = 0;

    ret = jwf_raw_read(&factory);
    if (ret < 0) {
        printf("jw_raw_read fail\n");
        return -1;
    }

    memset(&factory.jw_hardware_ver, 0, MAX_HDV_LEN);
    memcpy(&factory.jw_hardware_ver, hdv, MAX_HDV_LEN);

    ret = jwf_raw_write(&factory);
    if (ret < 0) {
        printf("write hardware version fail\n");
        return -1;
    }

    return 0;
}


