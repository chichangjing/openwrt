#ifndef _PDU_H_
#define _PDU_H_

#define PDU_DSA_OFFSET      12
#define DSA_TAG_BIT         0x20
#define CRPP_PDU_SLEN 64
#define CRPP_PDU_LLEN 68
#define CRPP_MAX_AGE 256
//#define MAC_LEN 6
#define CRPP_DETECT_TIME (CRPP_MAX_AGE * 2) // ms

typedef struct {
    uint8_t tick[4];
    uint8_t ttl[4];
}MsgDetect_t;

// 32bytes
typedef union {
    uint8_t data[32];
    MsgDetect_t detect;
}MsgBody_t;

/* common ring protect protocol frame struct
 * 26bytes
 */
typedef struct {
    uint8_t dst[MAC_LEN];
    uint8_t src[MAC_LEN];
    uint8_t dsa[4];
    uint8_t len8023[2];
    uint8_t dsap;
    uint8_t ssap;
    uint8_t llc;
    uint8_t oui[3];
    uint8_t pid[2];
}crpphdr_t;

// 38bytes
typedef struct {
    uint8_t ver;
    uint8_t type;
    uint8_t port_id;
    uint8_t res[3];
    MsgBody_t msg;
}crppbody_t;

typedef struct {
    crpphdr_t hdr;
    crppbody_t body;
}crpppdu_t;

typedef enum {
    PACKET_DETECT
}ePacketType;


#endif
