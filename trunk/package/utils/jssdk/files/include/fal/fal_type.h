 
#ifndef _FAL_TYPE_H_
#define _FAL_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef a_uint32_t fal_port_t;

#if (SW_MAX_NR_PORT <= 32)
    typedef a_uint32_t fal_pbmp_t;
#else
    typedef a_uint64_t fal_pbmp_t;
#endif

typedef enum {
    FAL_ENABLE    = 1,
    FAL_DISABLE   = 2
} fal_enable_t;

typedef struct {
    a_uint8_t uc[6];
} fal_mac_addr_t;

typedef a_uint32_t fal_ip4_addr_t;

typedef struct {
    a_uint32_t ul[4];
} fal_ip6_addr_t;

typedef enum {
    FAL_MAC_FRWRD = 0,		/**<   packets are normally forwarded */
    FAL_MAC_DROP,			/**<   packets are dropped */
    FAL_MAC_CPY_TO_CPU,		/**<   packets are copyed to cpu */
    FAL_MAC_RDT_TO_CPU		/**<   packets are redirected to cpu */
} fal_fwd_cmd_t;

typedef enum {
    FAL_FRAME_BASED = 1,
    FAL_BYTE_BASED,
    FAL_RATE_MODE_END
} fal_traffic_unit_t;

typedef a_uint32_t fal_queue_t;
typedef a_uint32_t fal_bucket_t;

#define MV88E6097_MAX_PORT_NUM 11		/* 10 lport, 1 cpu port  */

#if defined(MARVELL_88E6097)
#define MV88E6097_MAX_MEMBERS_SIZE 2  	/* 2 byte max, 11 member */
#elif defined(MARVELL_3236) 
#define MV88E6097_MAX_MEMBERS_SIZE 4 	/* 2 byte max, 11 member */
#else 
#define MV88E6097_MAX_MEMBERS_SIZE 2 
#endif

/* jwsPortList type */
typedef struct {
	a_uint8_t members[MV88E6097_MAX_MEMBERS_SIZE];
} fal_portlist_t;


#ifdef __cplusplus
}
#endif
#endif

