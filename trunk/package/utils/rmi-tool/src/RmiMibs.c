#include "RmiMibs.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>


static MIBS mibs;

void show_mib_help(void){
    printf("%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            "%20s : %s \n"
            ,
            "In Good Bytes", "The sum of lengths of all good Ethernet frames received, that is frames that are \n"
                    "                           not bad frames. ",
            "In Bad Bytes", "The sum of lengths of all bad Ethernet frames received. ",
            "In Unicasts", "The number of good frames received that have a Unicast destination MAC address. ",
            "In Broadcasts", "The number of good frames received that have a Broadcast destination MAC address. ",
            "In Multicasts", "The number of good frames received that have a Multicast destination MAC address. ",
            "In Pauses", "The number of good frames received that have a Pause destination MAC address. ",
            "In Undersize", "Total frames received with a length of less than 64 Byte but with a valid FCS. ",
            "In Fragments", "Total frames received with a length of less than 64 Byte an invalid FCS. ",
            "In Oversize", "Total frames received with a length of more than MAX Byte but with a valid FCS. ",
            "In Jabber", "Total frames received with a length of more than MAX Byte but with an invalid FCS. ",
            "In RxErr", "Total frames received with a RxErr signal from the PHY. ",
            "In FCSErr", "Total frames received with a CRC error not counted in InFragments, InJabber or InRxErr. ",
            "In Discards", "The number of good, non-filtered frames that normally would have been forwarded, but \n"
                    "                           could not be due to a lack of buffer space. ",
            "In Filtered", "The number of good frames that were filtered due to ingress switch policy rules. These rules \n"
                    "                           include frames that are dropped due to Layer 2 Policy Control Lists, \n"
                    "                           802.1x authentication, MAC Address Learn Limiting, 802.1Q Security checks, \n"
                    "                           DiscardTagged & DiscardUntagged, PortStats other than Disabled, \n"
                    "                           and DA mapping back to the source port. ",
            "Out Bytes", "The sum of lengths of all Ethernet frames sent from this MAC. ",
            "Out Unicasts", "The sum of lengths of all Ethernet frames sent that have a Unicast destination MAC address. ",
            "Out FCSErr", "The number of frames transmitted with a invalid FCS. ",
            "Out Multicasts", "The sum of lengths of all Ethernet frames sent that have a Multicasts destination \n"
                    "                           MAC address. ",
            "Out Broadcasts", "The sum of lengths of all Ethernet frames sent that have a Broadcasts destination \n"
                    "                           MAC address. ",
            "Out Pauses", "The number of Flow Control frames sent out. ",
            "Out Filtered", "The number of good frames that were filtered due to egress switch policy rules. These \n"
                    "                           rules include frames that passed the ingress port`s policy but are dropped due to the egress \n"
                    "                           policy of this port including 802.1Q Security checks and PortStat other than Disabled. ",
            "64 Bytes", "Total frames received(and/or transmitted) with a length of exactly 64 bytes, including \n"
                    "                           those with errors. ",
            "65 to 127 Bytes", "Total frames received(and/or transmitted) with a length of between 65 and 127 bytes \n"
                    "                           inclusive, including those with errors. ",
            "128 to 255 Bytes", "Total frames received(and/or transmitted) with a length of between 128 and 255 bytes \n"
                    "                           inclusive, including those with errors. ",
            "256 to 511 Bytes", "Total frames received(and/or transmitted) with a length of between 256 and 511 bytes \n"
                    "                           inclusive, including those with errors. ",
            "512 to 1023 Bytes", "Total frames received(and/or transmitted) with a length of between 512 and 1023 bytes \n"
                    "                           inclusive, including those with errors. ",
            "1024 to Max Bytes", "Total frames received(and/or transmitted) with a length of between 1024 and MaxSize \n"
                    "                           bytes inclusive, including those with errors. "
            "Deferred ", "The total number of successfully transmitted frames that experienced no collisions but \n"
            "                           are delayed because the medium was busy during the first attempt. This counter is application \n"
            "                           in half-duplex only. "
            "Excessive", "The number of frams dropped in the transmit MAC because the frame experienced 16 consecutive \n"
                    "                           collisions. This counter is applicable in half-duplex only and only of DiscardExcessive is a one. ",
            "Single", "The total number of successfully transmitted frames that experienced exactly one collision. \n"
                    "                           This counter is applicable in half-duplex only. ",
            "Multiple", "The total number of successfully transmitted frames that experienced more than one collision. \n"
                    "                           This counter is applicable in half-duplex only. ",
            "Collisions", "The number of collision events seen of by the MAC not including those counted in Singal. \n"
                    "                           Multicast, Excessive, or Late. This counter is applicable in half-duplex only. ",
            "Late", "The number of times a collision is detected later than 512 bits-times into the transmission \n"
                    "                           of a frame. This counter is applicable in half-duplex only. ");
}

void show_mibs(int16_t port){
    pthread_mutex_lock(&mibs.mutex[port]);
    printf("%20s : %s \n", "In Good Bytes",        resolve_u64(mibs.Port[port].InGoodOctets));
    printf("%20s : %s \n", "In Bad Bytes",         resolve_u32(mibs.Port[port].InBadOctets));
    printf("%20s : %s \n", "In Unicasts",          resolve_u32(mibs.Port[port].InUnicasts));
    printf("%20s : %s \n", "In Broadcasts",        resolve_u32(mibs.Port[port].InBroadcasts));
    printf("%20s : %s \n", "In Multicasts",        resolve_u32(mibs.Port[port].InMulticasts));
    printf("%20s : %s \n", "In Pauses",            resolve_u32(mibs.Port[port].InPauses));
    printf("%20s : %s \n", "In Undersize",         resolve_u32(mibs.Port[port].InUndersize));
    printf("%20s : %s \n", "In Fragments",         resolve_u32(mibs.Port[port].InFragments));
    printf("%20s : %s \n", "In Oversize",          resolve_u32(mibs.Port[port].InOversize));
    printf("%20s : %s \n", "In Jabber",            resolve_u32(mibs.Port[port].InJabber));
    printf("%20s : %s \n", "In RxErr",             resolve_u32(mibs.Port[port].InRxErr));
    printf("%20s : %s \n", "In FCSErr",            resolve_u32(mibs.Port[port].InFCSErr));
    printf("%20s : %s \n", "In Discards",          resolve_u32(mibs.Port[port].InDiscards));
    printf("%20s : %s \n", "In Filtered",          resolve_u32(mibs.Port[port].InFiltered));
    printf("%20s : %s \n", "Out Bytes",            resolve_u64(mibs.Port[port].OutOctets));
    printf("%20s : %s \n", "Out Unicasts",         resolve_u32(mibs.Port[port].OutUnicasts));
    printf("%20s : %s \n", "Out FCSErr",           resolve_u32(mibs.Port[port].OutFCSErr));
    printf("%20s : %s \n", "Out Multicasts",       resolve_u32(mibs.Port[port].OutMulticasts));
    printf("%20s : %s \n", "Out Broadcasts",       resolve_u32(mibs.Port[port].OutBroadcasts));
    printf("%20s : %s \n", "Out Pauses",           resolve_u32(mibs.Port[port].OutPauses));
    printf("%20s : %s \n", "Out Filtered",         resolve_u32(mibs.Port[port].OutFiltered));
    printf("%20s : %s \n", "64 Bytes",             resolve_u32(mibs.Port[port]._64_Octets));
    printf("%20s : %s \n", "65 to 127 Bytes",      resolve_u32(mibs.Port[port]._65to127_Octets));
    printf("%20s : %s \n", "128 to 255 Bytes",     resolve_u32(mibs.Port[port]._128to255_Octets));
    printf("%20s : %s \n", "256 to 511 Bytes",     resolve_u32(mibs.Port[port]._256to511_Octets));
    printf("%20s : %s \n", "512 to 1023 Bytes",    resolve_u32(mibs.Port[port]._512to1023_Octets));
    printf("%20s : %s \n", "1024 to Max Bytes",    resolve_u32(mibs.Port[port]._1024tomax_Octets));
    printf("%20s : %s \n", "Deferred",             resolve_u32(mibs.Port[port].Deferred));
    printf("%20s : %s \n", "Excessive",            resolve_u32(mibs.Port[port].Excessive));
    printf("%20s : %s \n", "Single",               resolve_u32(mibs.Port[port].Single));
    printf("%20s : %s \n", "Multiple",             resolve_u32(mibs.Port[port].Multiple));
    printf("%20s : %s \n", "Collisions",           resolve_u32(mibs.Port[port].Collisions));
    printf("%20s : %s \n", "Late",                 resolve_u32(mibs.Port[port].Late));
    pthread_mutex_unlock(&mibs.mutex[port]);
}

#define CLEAR()             printf("\033[2J")                       // 清除屏幕
#define CLEAR_ROW()         printf("\033[K")                        // 清除行
#define MOVEUP(x)           printf("\033[%dA", (x))                 // 上移光标
#define MOVEDOWN(x)         printf("\033[%dB", (x))                 // 下移光标
#define MOVELEFT(y)         printf("\033[%dD", (y))                 // 左移光标
#define MOVERIGHT(y)        printf("\033[%dC", (y))                 // 右移光标
#define MOVETO(x,y)         printf("\033[%d;%dH", (x), (y))         // 定位光标
#define RESET_CURSOR()      printf("\033[H")                        // 光标复位
#define HIDE_CURSOR()       printf("\033[?25l")                     // 隐藏光标
#define SHOW_CURSOR()       printf("\033[?25h")                     // 显示光标
#define HIGHT_LIGHT()       printf("\033[1m")                       // 反显
#define UN_HIGHT_LIGHT()    printf("\033[0m")                       // 反显

void show_rates(void){
    int port;
    RESET_CURSOR();
    printf("%8s %20s %18s %18s %18s %20s %18s %18s %18s \n\033[K",
            " ", "In Bytes", "Unicast", "Multicast", "Broadcast",
            "Out Bytes", "Unicast", "Multicast", "Broadcast");
    for(port= 0; port<PORTS_MV6097; port++){
        pthread_mutex_lock(&mibs.mutex[port]);
        printf("%5s-%-2d", "port", port);
        printf("%14s Byte/s",  resolve_u64(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_InGoodOctets*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("%11s frame/s", resolve_u32(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_InUnicasts*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("%11s frame/s", resolve_u32(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_InMulticasts*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("%11s frame/s", resolve_u32(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_InBroadcasts*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("%14s Byte/s",  resolve_u64(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_OutOctets*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("%11s frame/s", resolve_u32(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_OutUnicasts*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("%11s frame/s", resolve_u32(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_OutMulticasts*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("%11s frame/s", resolve_u32(mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp ? 1.0*mibs.Rate[L_GE_Z20208D[port]].Incre_OutBroadcasts*1000000000/mibs.Rate[L_GE_Z20208D[port]].Incre_TimeStamp/512:0));
        printf("\n\033[K");
        pthread_mutex_unlock(&mibs.mutex[port]);
    }
}

int32_t MIB_dump(int32_t dev, int32_t port){
    if(port>=PORTS_MV6097 || dev>0xff){
        return RMI_ERR_PARA_INVALID;
    }
    Rmi_req req = {
            .dev=(u_int8_t)(dev),
            .req_format=REQ_DUMP_MIB_FARMOT,
            .res=0x0000,
            .req_code=REQ_DUMP_MIB_CODE,
            .req_data=0x000f&port,
            .pad=REQ_DUMP_MIB_PADING,
    };
    rmi_req_format(&req, (Rmi_req_bpu *)(rmi_buf));
    rmi_pkt_send(rmi_buf, sizeof(Rmi_req_bpu));
    rmi_pkt_rcv(rmi_buf, BUF_LEN);
    Rmi_MIB_dump * rmi_MIB_dump=(Rmi_MIB_dump * )rmi_buf;
    pthread_mutex_lock(&mibs.mutex[port]);
    mibs.Port[port].Device=dev;
    mibs.Port[port].Port=port;

    mibs.Rate[port].Incre_TimeStamp=ntohl(*(u_int32_t *)(rmi_MIB_dump->TimeStamp))-mibs.Port[port].TimeStamp;
    mibs.Rate[port].Incre_InGoodOctets=(u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->InGoodOctetsHi)))*0x100000000 \
            + (u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->InGoodOctetsLo)))-mibs.Port[port].InGoodOctets;
    mibs.Rate[port].Incre_InUnicasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->InUnicasts))-mibs.Port[port].InUnicasts;
    mibs.Rate[port].Incre_InBroadcasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->InBroadcasts))-mibs.Port[port].InBroadcasts;
    mibs.Rate[port].Incre_InMulticasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->InMulticasts))-mibs.Port[port].InMulticasts;
    mibs.Rate[port].Incre_OutOctets=(u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->OutOctetsHi)))*0x100000000 \
            + (u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->OutOctetsLo)))-mibs.Port[port].OutOctets;
    mibs.Rate[port].Incre_OutUnicasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutUnicasts))-mibs.Port[port].OutUnicasts;
    mibs.Rate[port].Incre_OutMulticasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutMulticasts))-mibs.Port[port].OutMulticasts;
    mibs.Rate[port].Incre_OutBroadcasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutBroadcasts))-mibs.Port[port].OutBroadcasts;

    mibs.Port[port].TimeStamp=ntohl(*(u_int32_t *)(rmi_MIB_dump->TimeStamp));
    mibs.Port[port].InGoodOctets=(u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->InGoodOctetsHi)))*0x100000000 \
            + (u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->InGoodOctetsLo)));
    mibs.Port[port].InBadOctets=ntohl(*(u_int32_t *)(rmi_MIB_dump->InBadOctets));
    mibs.Port[port].OutFCSErr=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutFCSErr));
    mibs.Port[port].InUnicasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->InUnicasts));
    mibs.Port[port].Deferred=ntohl(*(u_int32_t *)(rmi_MIB_dump->Deferred));
    mibs.Port[port].InBroadcasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->InBroadcasts));
    mibs.Port[port].InMulticasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->InMulticasts));
    mibs.Port[port]._64_Octets=ntohl(*(u_int32_t *)(rmi_MIB_dump->_64_Octets));
    mibs.Port[port]._65to127_Octets=ntohl(*(u_int32_t *)(rmi_MIB_dump->_65to127_Octets));
    mibs.Port[port]._128to255_Octets=ntohl(*(u_int32_t *)(rmi_MIB_dump->_128to255_Octets));
    mibs.Port[port]._256to511_Octets=ntohl(*(u_int32_t *)(rmi_MIB_dump->_256to511_Octets));
    mibs.Port[port]._512to1023_Octets=ntohl(*(u_int32_t *)(rmi_MIB_dump->_512to1023_Octets));
    mibs.Port[port]._1024tomax_Octets=ntohl(*(u_int32_t *)(rmi_MIB_dump->_1024tomax_Octets));
    mibs.Port[port].OutOctets=(u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->OutOctetsHi)))*0x100000000 \
            + (u_int64_t)(ntohl(*(u_int32_t *)(rmi_MIB_dump->OutOctetsLo)));
    mibs.Port[port].OutUnicasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutUnicasts));
    mibs.Port[port].Excessive=ntohl(*(u_int32_t *)(rmi_MIB_dump->Excessive));
    mibs.Port[port].OutMulticasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutMulticasts));
    mibs.Port[port].OutBroadcasts=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutBroadcasts));
    mibs.Port[port].Single=ntohl(*(u_int32_t *)(rmi_MIB_dump->Single));
    mibs.Port[port].OutPauses=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutPauses));
    mibs.Port[port].InPauses=ntohl(*(u_int32_t *)(rmi_MIB_dump->InPauses));
    mibs.Port[port].Multiple=ntohl(*(u_int32_t *)(rmi_MIB_dump->Multiple));
    mibs.Port[port].InUndersize=ntohl(*(u_int32_t *)(rmi_MIB_dump->InUndersize));
    mibs.Port[port].InFragments=ntohl(*(u_int32_t *)(rmi_MIB_dump->InFragments));
    mibs.Port[port].InOversize=ntohl(*(u_int32_t *)(rmi_MIB_dump->InOversize));
    mibs.Port[port].InJabber=ntohl(*(u_int32_t *)(rmi_MIB_dump->InJabber));
    mibs.Port[port].InRxErr=ntohl(*(u_int32_t *)(rmi_MIB_dump->InRxErr));
    mibs.Port[port].InFCSErr=ntohl(*(u_int32_t *)(rmi_MIB_dump->InFCSErr));
    mibs.Port[port].Collisions=ntohl(*(u_int32_t *)(rmi_MIB_dump->Collisions));
    mibs.Port[port].Late=ntohl(*(u_int32_t *)(rmi_MIB_dump->Late));
    mibs.Port[port].InDiscards=(u_int32_t)(ntohl(*(u_int16_t *)(rmi_MIB_dump->InDiscardsHi)))*0x10000 \
            + (u_int32_t)(ntohl(*(u_int16_t *)(rmi_MIB_dump->InDiscardsLo)));
    mibs.Port[port].InFiltered=ntohl(*(u_int32_t *)(rmi_MIB_dump->InFiltered));
    mibs.Port[port].OutFiltered=ntohl(*(u_int32_t *)(rmi_MIB_dump->OutFiltered));
    pthread_mutex_unlock(&mibs.mutex[port]);
    buf_unlock();
    return 0;
}


int32_t MIB_clear(int32_t dev, int32_t port){
    if(port>=PORTS_MV6097 || dev>0xff){
        return RMI_ERR_PARA_INVALID;
    }
    Rmi_req req = {
            .dev=(u_int8_t)(dev),
            .req_format=REQ_DUMP_MIB_CLEAR_FARMOT,
            .res=0x0000,
            .req_code=REQ_DUMP_MIB_CLEAR_CODE,
            .req_data=0x8000|(0x000f&(u_int16_t)(port)),
            .pad=REQ_DUMP_MIB_CLEAR_PADING,
    };
    rmi_req_format(&req, (Rmi_req_bpu *)(rmi_buf));
    rmi_pkt_send(rmi_buf, sizeof(Rmi_req_bpu));
    rmi_pkt_rcv(rmi_buf, BUF_LEN);
    buf_unlock();
    return 0;
}

void mib_init(void){
    int32_t port;
    for(port= 0;port<PORTS_MV6097;port++){
        pthread_mutex_init(&mibs.mutex[port], NULL);
    }

}


u_int32_t mib_get(int32_t port, MIB_P * mib_p){
    if(port>=PORTS_MV6097){
        return RMI_ERR_PARA_INVALID;
    }
    pthread_mutex_lock(&mibs.mutex[port]);
    memcpy(mib_p, &mibs.Port[L_GE_Z20208D[port]], sizeof(MIB_P));
    pthread_mutex_unlock(&mibs.mutex[port]);
    return 0;
}

