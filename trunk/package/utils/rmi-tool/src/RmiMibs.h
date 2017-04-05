#ifndef __RMI_MIB_H__
#define __RMI_MIB_H__
#include <pthread.h>
//#include <linux/mutex.h>

#include "Rmu.h"

typedef enum{
    MIB_INGOODOCTETSLO=0x00,
    MIB_INGOODOCTETSHI,
    MIB_INBADOCTETS,
    MIB_OUTFCSERR,
    MIB_INUNICASTS,
    MIB_DEFERRED,
    MIB_INBROADCASTS,
    MIB_INMULTICASTS,
    MIB_64_OCTETS,
    MIB_65TO127_OCTETS,
    MIB_128TO255_OCTETS,
    MIB_256TO511_OCTETS,
    MIB_512TO1023_OCTETS,
    MIB_1024TOMAX_OCTETS,
    MIB_OUTOCTETSLO,
    MIB_OUTOCTETSHI,
    MIB_OUTUNICASTS,
    MIB_EXCESSIVE,
    MIB_OUTMULTICASTS,
    MIB_OUTBROADCASTS,
    MIB_SINGLE,
    MIB_OUTPAUSES,
    MIB_INPAUSES,
    MIB_MULTIPLE,
    MIB_INUNDERSIZE,
    MIB_INFRAGMENTS,
    MIB_INOVERSIZE,
    MIB_INJABBER,
    MIB_INRXERR,
    MIB_INFCSERR,
    MIB_COLLISIONS,
    MIB_LATE,

    MIB_INDISCARDS,
    MIB_INFILTERED,
    MIB_OUTFILTERED,
    MIB_MAX,
}MIB_type;

typedef struct _MIB_P{
    u_int8_t Device;
    u_int16_t Port;
    u_int32_t TimeStamp;
    u_int64_t InGoodOctets;
    u_int32_t InBadOctets;
    u_int32_t OutFCSErr;
    u_int32_t InUnicasts;
    u_int32_t Deferred;
    u_int32_t InBroadcasts;
    u_int32_t InMulticasts;
    u_int32_t _64_Octets;
    u_int32_t _65to127_Octets;
    u_int32_t _128to255_Octets;
    u_int32_t _256to511_Octets;
    u_int32_t _512to1023_Octets;
    u_int32_t _1024tomax_Octets;
    u_int64_t OutOctets;
    u_int32_t OutUnicasts;
    u_int32_t Excessive;
    u_int32_t OutMulticasts;
    u_int32_t OutBroadcasts;
    u_int32_t Single;
    u_int32_t OutPauses;
    u_int32_t InPauses;
    u_int32_t Multiple;
    u_int32_t InUndersize;
    u_int32_t InFragments;
    u_int32_t InOversize;
    u_int32_t InJabber;
    u_int32_t InRxErr;
    u_int32_t InFCSErr;
    u_int32_t Collisions;
    u_int32_t Late;
    u_int32_t InDiscards;
    u_int16_t InFiltered;
    u_int16_t OutFiltered;
}MIB_P;

typedef struct _mib_rate{
    u_int32_t Incre_TimeStamp;

    u_int64_t Incre_InGoodOctets;
    u_int32_t Incre_InUnicasts;
    u_int32_t Incre_InMulticasts;
    u_int32_t Incre_InBroadcasts;

    u_int64_t Incre_OutOctets;
    u_int32_t Incre_OutUnicasts;
    u_int32_t Incre_OutMulticasts;
    u_int32_t Incre_OutBroadcasts;
}MIB_RATE;

typedef struct _rmi_MIB_dump{

    unsigned char da[6];
    unsigned char sa[6];
    unsigned char tag[4];
    unsigned char eth_type[2];

    unsigned char req_format[2];
    unsigned char res[2];
    unsigned char req_code[2];

    unsigned char Device[1];
    unsigned char Port[1];
    unsigned char TimeStamp[4];

    unsigned char InGoodOctetsLo[4];
    unsigned char InGoodOctetsHi[4];
    unsigned char InBadOctets[4];
    unsigned char OutFCSErr[4];
    unsigned char InUnicasts[4];
    unsigned char Deferred[4];
    unsigned char InBroadcasts[4];
    unsigned char InMulticasts[4];
    unsigned char _64_Octets[4];
    unsigned char _65to127_Octets[4];
    unsigned char _128to255_Octets[4];
    unsigned char _256to511_Octets[4];
    unsigned char _512to1023_Octets[4];
    unsigned char _1024tomax_Octets[4];
    unsigned char OutOctetsLo[4];
    unsigned char OutOctetsHi[4];
    unsigned char OutUnicasts[4];
    unsigned char Excessive[4];
    unsigned char OutMulticasts[4];
    unsigned char OutBroadcasts[4];
    unsigned char Single[4];
    unsigned char OutPauses[4];
    unsigned char InPauses[4];
    unsigned char Multiple[4];
    unsigned char InUndersize[4];
    unsigned char InFragments[4];
    unsigned char InOversize[4];
    unsigned char InJabber[4];
    unsigned char InRxErr[4];
    unsigned char InFCSErr[4];
    unsigned char Collisions[4];
    unsigned char Late[4];

    unsigned char InDiscardsHi[4];
    unsigned char InDiscardsLo[4];
    unsigned char InFiltered[4];
    unsigned char OutFiltered[4];
}Rmi_MIB_dump;

typedef struct _MIBS{
    u_int16_t Number;
    u_int16_t Device;
    pthread_mutex_t mutex[PORTS_MV6097];
    MIB_P Port[PORTS_MV6097];
    MIB_RATE Rate[PORTS_MV6097];
}MIBS;

void show_rates(void);
void show_mib_help(void);
void show_mibs(int16_t port);

int32_t MIB_dump(int32_t dev, int32_t port);
int32_t MIB_clear(int32_t dev, int32_t port);

void mib_init(void);
#endif
