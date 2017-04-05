#include "RmiAtu.h"

#include <stdio.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <pthread.h>


static Atu atu;

void atu_init(void){
    pthread_mutex_init(&atu.mutex, NULL);
}

void show_atu_entry(void){
    pthread_mutex_lock(&atu.mutex);
    int32_t idx=0;
#if 1
    for(;idx<atu.number;idx++){
        printf("  %-2s  %-8s %02x%02x-%02x%02x-%02x%02x %5s-%-2d \n", (0==atu.entry[idx].type)?"U":"M", \
                (0==atu.entry[idx].stats)?"Unused":(atu.entry[idx].stats<8?"Aging":"Static"), \
                        atu.entry[idx].mac.addr[0], atu.entry[idx].mac.addr[1], \
                        atu.entry[idx].mac.addr[2], atu.entry[idx].mac.addr[3], \
                        atu.entry[idx].mac.addr[4], atu.entry[idx].mac.addr[5], \
                        (atu.entry[idx].dpv_type)?"trunk":"port", \
                        (atu.entry[idx].type)? '*':P_GE_Z20208D[atu.entry[idx].dpv]);
    }
#endif
    printf("Total mac address: %d.\n", atu.number);
    pthread_mutex_unlock(&atu.mutex);
}

static int16_t __atu_dump(u_int8_t dev, Atu_entry * atu_entry, int16_t continue_code){
    int32_t mac_entry_num;
    Rmi_req req = {
            .dev=dev,
            .req_format=REQ_DUMP_ATU_FARMOT,
            .res=0x0000,
            .req_code=REQ_DUMP_ATU_CODE,
            .req_data=continue_code,
            .pad=REQ_DUMP_ATU_PADING,
    };
    rmi_req_format(&req, (Rmi_req_bpu *)(rmi_buf));
    rmi_pkt_send(rmi_buf, sizeof(Rmi_req_bpu));
    rmi_pkt_rcv(rmi_buf, BUF_LEN);
    char * seg = rmi_buf+RMI_HEAD_LEN;
    for(mac_entry_num=0;
            mac_entry_num<RMI_ATU_ENTRY_MAX && (seg[0] || seg[1]);
            mac_entry_num++, seg+=RMI_ATU_ENTRY_LEN, atu.number++){
        atu_entry[mac_entry_num].stats=(u_int8_t)((seg[0])>>4);
        atu_entry[mac_entry_num].dpv_type=(u_int8_t)(((seg[0])&0x8)>>3);
        int32_t dpv =(int32_t)(((seg[0])&0x7)*0x100+(seg[1]));
        atu_entry[mac_entry_num].dpv=0;
        if(dpv){
            while(dpv>1){
                dpv>>=1;
                atu_entry[mac_entry_num].dpv++;
            }
        }
        int32_t fid =(int32_t)((((seg[9])&0xf)*0x100+(seg[10])));
        atu_entry[mac_entry_num].fid=0;
        if(fid){
            while(fid>1){
                fid>>=1;
                atu_entry[mac_entry_num].fid++;
            }
        }
        int32_t pri =(int32_t)(((seg[9])&0x70)>>4);
        atu_entry[mac_entry_num].pri=0;
        if(pri){
            while(pri>1){
                pri>>=1;
                atu_entry[mac_entry_num].pri++;
            }
        }
        atu_entry[mac_entry_num].type=(int8_t)(seg[2]%2);
        memcpy(&(atu_entry[mac_entry_num].mac), &(seg[2]), ETH_ALEN);
    }
    seg+= (RMI_ATU_ENTRY_MAX==mac_entry_num) ? 0:RMI_ATU_ENTRY_LEN;
    continue_code = (RMI_ATU_ENTRY_MAX==mac_entry_num) ? (int16_t)(seg[0]*0x100+seg[1]):0x0000;
    buf_unlock();
    return continue_code;
}

int32_t ATU_dump(int32_t dev){
    if(dev>0xff){
        return RMI_ERR_PARA_INVALID;
    }
    int16_t recv;
    recv = REQ_DUMP_ATU_DATA;
    pthread_mutex_lock(&atu.mutex);
    memset(&atu, 0, sizeof(Atu));
    Atu_entry * atu_entry = atu.entry;
    while((recv=__atu_dump((u_int8_t)(dev), atu_entry, recv))){
        atu_entry=&atu.entry[atu.number];
    }
    pthread_mutex_unlock(&atu.mutex);
    return 0;
}
