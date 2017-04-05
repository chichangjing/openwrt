#ifndef __RMI_ATU_H__
#define __RMI_ATU_H__

#include <pthread.h>
//#include <linux/mutex.h>

#include "Rmu.h"


typedef struct _rmi_ATU_dump{
    unsigned char da[6];
    unsigned char sa[6];
    unsigned char tag[4];
    unsigned char eth_type[2];
    unsigned char req_format[2];
    unsigned char res[2];
    unsigned char req_code[2];
    unsigned char res_data[2];
}Rmi_ATU_dump;

enum{
    UNICAST_ENTRY_UNUSED=0,
    UNICAST_ENTRY_STATIC_POLICY=0x8,
    UNICAST_ENTRY_STATIC_POLICY_PRIOVERRIDE=0x9,
    UNICAST_ENTRY_STATIC_NORATELIMIT=0xA,
    UNICAST_ENTRY_STATIC_NORATELIMIT_PRIOVERRIDE=0xB,
    UNICAST_ENTRY_STATIC_MGMT=0xC,
    UNICAST_ENTRY_STATIC_MGMT_PRIOVERRIDE=0xD,
    UNICAST_ENTRY_STATIC=0xE,
    UNICAST_ENTRY_STATIC_PRIOVERRIDE=0xF,
    UNICAST_ENTRY_MAX,
}Unicast_stats;

enum{
    MULTICAST_ENTRY_UNUSED=0,
    MULTICAST_ENTRY_STATIC_POLICY=0x8,
    MULTICAST_ENTRY_STATIC_POLICY_PRIOVERRIDE=0x9,
    MULTICAST_ENTRY_STATIC_NORATELIMIT=0xA,
    MULTICAST_ENTRY_STATIC_NORATELIMIT_PRIOVERRIDE=0xB,
    MULTICAST_ENTRY_STATIC_MGMT=0xC,
    MULTICAST_ENTRY_STATIC_MGMT_PRIOVERRIDE=0xD,
    MULTICAST_ENTRY_STATIC=0xE,
    MULTICAST_ENTRY_STATIC_PRIOVERRIDE=0xF,
    MULTICAST_ENTRY_MAX,
}Multicast_stats;

typedef enum{
    ADDRESS_UNICAST_ADDRESS,
    ADDRESS_MULTICAST_ADDRESS,
    ADDRESS_TYPE_MAX,
}Address_type;

typedef enum{
    DPV_PORT,
    DPV_TRUNK,
    DPV_MAX,
}Dpv_type;

typedef struct _Atu_entry{
    Rmi_mac mac;
    u_int16_t dpv;
    u_int8_t stats;
    u_int8_t fid;
    u_int8_t pri;
    u_int8_t dpv_type;
    u_int8_t type;
}Atu_entry;

typedef struct _Atu{
    pthread_mutex_t mutex;
    u_int16_t number;
    u_int16_t device;
    Atu_entry entry[ENTRY_MV6097];
}Atu;

void atu_init(void);

void show_atu_entry(void);
int32_t ATU_dump(int32_t dev);
#endif
