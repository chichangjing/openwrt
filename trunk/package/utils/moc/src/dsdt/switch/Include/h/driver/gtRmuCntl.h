#include <Copyright.h>

/********************************************************************************
* gtRmuCntl.h
*
* DESCRIPTION:
*       Functions definitions for RMU accessing single/multiple registers, ATU
*       entries and MIB counters.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __gtRmuCntlh
#define __gtRmuCntlh

#include <msApi.h>
#include <gtDrvSwRegs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RM Request Code List */
#define RMU_CODE_GET_ID			0x0000
#define RMU_CODE_DUMP_ATU		0x1000
#define RMU_CODE_DUMP_MIB		0x1020
#define RMU_CODE_RW_REG			0x2000
#define RMU_CODE_ERROR			0xFFFF
#define RMU_REQ_FORMAT 			0x0001

#define RMU_DEFAULT_DST_ADDRESS {0x01,0x50,0x43,0,0,0}
#define RMU_DEFAULT_SRC_ADDRESS {0,0,0,0,0,1}
#define RMU_DEFAULT_ETYPE 0x9100
#define RMU_FRAME_PRIORITY	6

#define RMU_RMON_FLUSH_REQUIRED		1

#define RM_REG_READ			0
#define RM_REG_WRITE		1
#define RM_REG_WAIT_TILL_0	2
#define RM_REG_WAIT_TILL_1	3


#define RMU_WAIT_ON_BIT_FALSE 0
#define RMU_WAIT_ON_BIT_TRUE 1

#define RMU_REQ_OPCODE_READ  2
#define RMU_REQ_OPCODE_WRITE 1
#define RMU_REQ_OPCODE_WAIT_VAL0  0
#define RMU_REQ_OPCODE_WAIT_VAL1  3

#define RMU_ETHER_TYPE_DSA_ENABLE 0

#define RMU_ETHER_TYPE       0x91000000
#define RMU_END_OF_FRAME     0xffffffff
#define RMU_DSA_TAG          0x40fa0f07
#define RMU_LEN_TYPE         0x0800

#define RMU_REQ_TYPE_SOHO    0x00010000
#define RMU_REQ_TYPE_GETID   0x00000000


#define RMU_PACKET_OCTET1_POS 0
#define RMU_PACKET_OCTET2_POS 1
#define RMU_PACKET_OCTET3_POS 2
#define RMU_PACKET_OCTET4_POS 3

#define RMU_PACKET_PREAMBLE_SIZE 7
#define RMU_PACKET_SFD_SIZE 1
#define RMU_PACKET_DA_SIZE 6
#define RMU_PACKET_SA_SIZE 6
#if RMU_ETHER_TYPE_DSA_ENABLE
    #define RMU_PACKET_ETHER_TYPE_SIZE 4
    #define RMU_PACKET_PAD_SIZE 0
#else
    #define RMU_PACKET_ETHER_TYPE_SIZE 0
    #define RMU_PACKET_PAD_SIZE 4
#endif
#define RMU_PACKET_DSA_TAG_SIZE 4
#define RMU_PACKET_LENGTH_TYPE_SIZE 2
#define RMU_PACKET_REQ_TYPE_SIZE 4
#define RMU_PACKET_REQ_FORMAT_SIZE 4
#define RMU_PACKET_REQ_CODE_SIZE 2
#define RMU_PACKET_REQ_DATA_SIZE 2
#define RMU_PACKET_REGCMD_WORD_SIZE 4
#define RMU_PACKET_FCS_SIZE 4

#define RMU_PACKET_PREFIX (RMU_PACKET_DA_SIZE + RMU_PACKET_SA_SIZE + \
            RMU_PACKET_ETHER_TYPE_SIZE + RMU_PACKET_DSA_TAG_SIZE + \
            RMU_PACKET_LENGTH_TYPE_SIZE)
#define RMU_PACKET_SUFFIX (RMU_PACKET_PAD_SIZE + RMU_PACKET_FCS_SIZE)

#define RMU_PACKET_REGRW_PREFIX_SIZE (RMU_PACKET_PREFIX + \
                                     + RMU_PACKET_REQ_FORMAT_SIZE)
#define RMU_PACKET_DEFAULT_REQ_SIZE (RMU_PACKET_PREFIX + \
            RMU_PACKET_REQ_FORMAT_SIZE + RMU_PACKET_REQ_CODE_SIZE + \
            RMU_PACKET_REGCMD_WORD_SIZE  + RMU_PACKET_SUFFIX)
#define RMU_PACKET_ATU_PREFIX_SIZE (24 + RMU_PACKET_ETHER_TYPE_SIZE)
#define RMU_PACKET_MIB_PREFIX_SIZE RMU_PACKET_PREFIX
#define RMU_PACKET_DSA_SEQ_NUM_POS (RMU_PACKET_DA_SIZE + \
            RMU_PACKET_SA_SIZE + RMU_PACKET_ETHER_TYPE_SIZE + \
            RMU_PACKET_OCTET4_POS)

#define RMU_MAX_ATU_ENTRIES 48
#define RMU_MAX_REGCMDS 120
#define RMU_MAX_PACKET_SIZE 512
#define RMU_MAX_SEQ_NUM 0xFF

/*
 * Data Structure for Frame Header
 */
typedef struct _RMU_FRAME_HEADER
{
	unsigned char*	src_address;
	unsigned char*	dst_address;
	unsigned short protocol;	/* protocol type */

	unsigned int		sw_dev_no;		/* switch device number */
	unsigned int		 pri;			/* priority */
	unsigned int		seq;			/* sequence number */

	unsigned short 	format;		/* frame format */
	unsigned short 	product;	/* product ID */
	unsigned short 	cmd;		/* request code */
	unsigned int			data_len;	/* response data len */
	unsigned char* 			data;		/* pointer to data */

} RMU_FRAME_HEADER;

typedef struct _RMU_DEV_DUMP_ATU
{
	unsigned long	from;		/* INPUT,OUTPUT:starting bin number */
	unsigned long	valid_entries;	/* OUTPUT:number of valid entries */
	GT_ATU_ENTRY	entries[RMU_MAX_ATU_ENTRIES];	/* OUTPUT:entries */
} RMU_DEV_DUMP_ATU;

typedef struct _RMU_DEV_DUMP_MIB
{
	unsigned long	port;	/* INPUT: port number start from 0 */
	unsigned long	flag;	/* INPUT: RM_RMON_FLUSH_REQUIRED */
	unsigned long	timeStamp;	/* OUTPUT: free running timer */
	union
	{
		GT_STATS_COUNTER_SET 	counter1;
		GT_STATS_COUNTER_SET3 	counter2;
		GT_STATS_COUNTER_SET3 	counter3;
	} rmon;					/* OUTPUT: RMON counter. */

	GT_PORT_STAT2	stat;	/* OUTPUT: discard/filtered counter */
} RMU_DEV_DUMP_MIB;


GT_STATUS gtRmuReadRegister
(
		IN GT_QD_DEV* dev,
		IN GT_U32 devAddr,
		IN GT_U32 regAddr,
		OUT GT_U32 *data
);

GT_STATUS gtRmuWriteRegister
(
		IN GT_QD_DEV* dev,
		IN GT_U32 devAddr,
		IN GT_U32 regAddr,
		IN GT_U32 data
);

GT_STATUS gtRmuRegAccess
(
        IN GT_QD_DEV* dev,
        INOUT HW_DEV_REG_ACCESS* ret_data
);

GT_STATUS gtSetRmuMode
(
		IN GT_QD_DEV *dev,
		IN GT_U8 port,
		IN GT_U8 enable,
		IN GT_U8 priority
);

GT_STATUS gtRmuGetId
(
		IN GT_QD_DEV *dev,
		OUT GT_U32 *chip_id
);

GT_STATUS gtRmuAtuDump
(
		IN GT_QD_DEV *dev,
		OUT RMU_DEV_DUMP_ATU *dump_atu
);

GT_STATUS gtRmuMibDump
(
		IN GT_QD_DEV *dev,
		IN GT_U8 port,
		IN GT_U8 enClearOnRead,
		OUT RMU_DEV_DUMP_MIB *dump_mib
);

#endif /* __gtRmuCntlh */
