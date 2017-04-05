#ifndef __RMU_H__
#define __RMU_H__

#include <string.h>
#include <sys/types.h>
#include <linux/types.h>
#include <linux/if_ether.h>

#define RMI_IF "eth0"

#define PRI_MV6097 0x07
#define DEV_MV6097 0
#define BUF_LEN 1500

#define ENTRY_MV6097 8192
#define PORTS_MV6097 11

#define SMI_DEV_MAX_MV6097 24
#define SMI_REG_MAX_MV6097 0x20

#define RMI_TYPE 0xff77
#define LOG_ERROR printf

#define RMI_HEAD_LEN 24         //rmi数据头长度
#define RMI_ATU_CODE_LEN 2
#define RMI_ATU_ENTRY_LEN 10    //每个mac entry长度
#define RMI_ATU_ENTRY_MAX 48    //每次dump获取的mac entry最大数量

#define REQ_GEID_FARMOT 0x0000
#define REQ_GEID_CODE   0x0000
#define REQ_GEID_DATA   0x0000
#define REQ_GEID_PADING 0x0000

#define RES_GEID_FARMOT 0x0001
#define RES_GEID_CODE   0x0000

#define REQ_DUMP_ATU_FARMOT 0x0001
#define REQ_DUMP_ATU_CODE   0x1000
#define REQ_DUMP_ATU_DATA   0x0000
#define REQ_DUMP_ATU_PADING 0x0000

#define RES_DUMP_ATU_FARMOT 0x0001
#define RES_DUMP_ATU_CODE   0x1000

#define REQ_DUMP_MIB_FARMOT 0x0001
#define REQ_DUMP_MIB_CODE   0x1020
#define REQ_DUMP_MIB_PADING 0x0000

#define RES_DUMP_MIB_FARMOT 0x0001
#define RES_DUMP_MIB_CODE   0x1000

#define REQ_DUMP_MIB_CLEAR_FARMOT 0x0001
#define REQ_DUMP_MIB_CLEAR_CODE   0x1020
#define REQ_DUMP_MIB_CLEAR_PADING 0x0000

#define RES_DUMP_MIB_CLEAR_FARMOT 0x0001
#define RES_DUMP_MIB_CLEAR_CODE   0x2000

#define RES_REG_SET_FARMOT 0x0001
#define RES_REG_SET_CODE   0x2000
#define RES_REG_SET_PADING 0x0000

#define RES_REG_GET_FARMOT 0x0001
#define RES_REG_GET_CODE   0x2000
#define RES_REG_GET_PADING 0x0000

extern char rmi_buf[BUF_LEN];
extern u_int8_t P_GE_Z20208D[PORTS_MV6097];
extern u_int8_t L_GE_Z20208D[PORTS_MV6097];

enum{
    NO_ERR=1,
    RMI_ERR_PARA_INVALID,
    RMI_ERR_SOCKET,
    RMI_ERR_PACKET_SEND,
    RMI_ERR_PACKET_RECV,
    RMI_ERR_FORMAT,
    RMI_ERR_CODE,
    RMI_ERR_DATE,
    RMI_ERR_MAX
};

#if 0
typedef struct _mv_tag{
    u_int8_t seq_num;
    unsigned pri:3;
    unsigned code_1:1;
    unsigned res_1:9;
    unsigned code_2:2;
    unsigned res_2:4;
    unsigned dev:5;
}Mv_tag;

typedef struct _dsa_tag{
    u_int8_t seq_num;
    unsigned pri:3;
    unsigned zero:1;
    unsigned res_1:10;
    unsigned rmm:1;
    unsigned res_2:4;
    unsigned dev:5;
}Dsa_tag;
#endif

typedef struct _mac{
    char addr[ETH_ALEN];
}Rmi_mac;

typedef struct _rmi_req_bpu{
    Rmi_mac da;
    Rmi_mac sa;
    u_int8_t tag[4];
    u_int16_t eth_type;

    u_int16_t req_format;
    u_int16_t res;
    u_int16_t req_code;
    u_int16_t req_data;
    u_int16_t pad;
}Rmi_req_bpu;


typedef struct _rmi_req{
    u_int16_t dev;
    u_int16_t req_format;
    u_int16_t res;
    u_int16_t req_code;
    u_int16_t req_data;
    u_int16_t pad;
}Rmi_req;

char * resolve_u32(int32_t _count);
char * resolve_u64(int64_t _count);

void buf_hex(char * buf, int32_t len);

void buf_lock(void);
void buf_unlock(void);

int32_t rmi_id_init(u_int32_t __seq_num);
int32_t rmi_sock_init(char * if_name);
int32_t rmi_req_format(Rmi_req * req, Rmi_req_bpu * bpu);
int32_t rmi_pkt_send(char * data, u_int32_t len);
int32_t rmi_pkt_rcv(char * data, u_int32_t len);

#endif
