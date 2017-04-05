#ifndef __JWF_TOOL_H__
#define __JWF_TOOL_H__

#define MAX_MAC_LEN 8
#define MAX_UID_LEN 32
#define MAX_HDV_LEN 32
#define MAX_PRDT_LEN 32


struct jw_factory {
      unsigned char jw_mac[MAX_MAC_LEN]; //align 4
      char jw_uid[MAX_UID_LEN];
      char jw_hardware_ver[MAX_HDV_LEN];
      char jw_product_type[MAX_PRDT_LEN];
};


extern int jwf_raw_read(struct jw_factory *jwfs);
extern int jwf_raw_write(struct jw_factory *jwfs);
extern int jwf_uid_get( char *uid,int max);
extern int jwf_uid_set(const char *uid);
extern int jwf_mac_get(unsigned char *mac);
extern int jwf_mac_set(const unsigned char *mac);
extern int jwf_prdt_get(unsigned char *prdt);
extern int jwf_prdt_set(const unsigned char *prdt);
extern int jwf_hdv_get(unsigned char *hdv);
extern int jwf_hdv_set(const unsigned char *hdv);


#define MTD_DEV_PATH "/dev/mtd3"

#endif /* __JWF_TOOL_H__ */
