#include <Copyright.h>
#include <gtHwCntl.h>
#include <gtRmuCntl.h>
#include <gtSem.h>

/********************************************************************************
* gtRmuCntl.c
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

#define RMU_PRINT(a) printf(a)

#define LITTLE_ENDIAN_MACHINE 1

#if LITTLE_ENDIAN_MACHINE
#define SWAP16(a) ((((a >> 0 ) & 0xff) << 8 ) | \
                  (((a >> 8 ) & 0xff) << 0 ))

#define SWAP32(a) ((((a >> 0 ) & 0xff) << 24) | \
                  (((a >> 8 ) & 0xff) << 16) | \
                  (((a >> 16) & 0xff) << 8 ) | \
                  (((a >> 24) & 0xff) << 0 ))

#define SWAP64(a) ((((a >> 0 ) & 0xff) << 56) | \
                  (((a >> 8 ) & 0xff) << 48) | \
                  (((a >> 16) & 0xff) << 40) | \
                  (((a >> 24) & 0xff) << 32) | \
                  (((a >> 32) & 0xff) << 24) | \
                  (((a >> 40) & 0xff) << 16) | \
                  (((a >> 48) & 0xff) << 8 ) | \
                  (((a >> 56) & 0xff) << 0 ))
#define SWAP32_BE(a) a
#else
#define SWAP16(a) a
#define SWAP32(a) a
#define SWAP64(a) a
#define SWAP32_BE(a) ((((a >> 0 ) & 0xff) << 24) | \
                  (((a >> 8 ) & 0xff) << 16) | \
                  (((a >> 16) & 0xff) << 8 ) | \
                  (((a >> 24) & 0xff) << 0 ))
#endif

static GT_U32 gtRmuGetSeqNum(void)
{
	static GT_U32 seq_num = 0;
	seq_num += 1;
	if(seq_num > RMU_MAX_SEQ_NUM)
		seq_num = 0;
	return seq_num;
}

static int rmuCreateRWFrame(unsigned char* frame, HW_DEV_REG_ACCESS* data)
{
	unsigned char* buf = frame;
	unsigned long i;
	HW_DEV_RW_REG* entry;
	int len;

	len = 0;

	for (i=0; i<data->entries; i++)
	{
		entry = &data->rw_reg_list[i];
		switch (entry->cmd)
		{
			case RM_REG_READ:
				*buf++ = (0x2 << 2) | ((entry->addr & 0x18) >> 3);
				*buf++ = ((entry->addr & 0x7) << 5) | (entry->reg & 0x1F);
				*buf++ = 0;
				*buf++ = 0;
				break;
			case RM_REG_WRITE:
				*buf++ = (0x1 << 2) | ((entry->addr & 0x18) >> 3);
				*buf++ = ((entry->addr & 0x7) << 5) | (entry->reg & 0x1F);
				*buf++ = (entry->data & 0xFF00) >> 8;
				*buf++ = entry->data & 0xFF;
				break;
			case RM_REG_WAIT_TILL_0:
				*buf++ = (0x1 << 4) | ((entry->addr & 0x18) >> 3);
				*buf++ = ((entry->addr & 0x7) << 5) | (entry->reg & 0x1F);
				*buf++ = entry->data & 0xF;
				*buf++ = 0;
				break;
			case RM_REG_WAIT_TILL_1:
				*buf++ = (0x1 << 4) | (0x3 << 2) | ((entry->addr & 0x18) >> 3);
				*buf++ = ((entry->addr & 0x7) << 5) | (entry->reg & 0x1F);
				*buf++ = entry->data & 0xF;
				*buf++ = 0;
				break;
			default:
				return -1;
		}
		len += 4;
	}

	*buf++ = 0xFF;
	*buf++ = 0xFF;
	*buf++ = 0xFF;
	*buf++ = 0xFF;

	return len+4;
}

/*
 * Parse the header of the response frame.
 * Return:
 * 	GT_OK on success
 *	GT_FAIL on fail
 */
static GT_STATUS rm_parse_header(RMU_FRAME_HEADER *header, unsigned char* buf, int len)
{
	unsigned int idx = 0;
	DBG_INFO(("frame: rm_parse_header called\n"));
	DBG_INFO(("%#x %#x %#x %#x %#x %#x %#x %#x\n",
				*buf,*(buf+1),*(buf+2),*(buf+3),
				*(buf+4),*(buf+5),*(buf+6),*(buf+7)));

	if (len < 30)
	{
		DBG_INFO(("frame: length is too short (%i)\n",len));
		return GT_FAIL;
	}

	idx = 0;
	header->dst_address = buf+idx;
	idx += 6;
	header->src_address = buf+idx;
	idx += 6;

	if(RMU_PACKET_ETHER_TYPE_SIZE)
	{
		header->protocol = SWAP16(*(unsigned short*)(buf+12));
		idx += 2;
		if (*(unsigned short*)(buf+idx) != 0)
		{
			DBG_INFO(("frame: ether type is not followed by zero (%#x)\n",*(unsigned short*)(buf+14)));
			return GT_FAIL;
		}
		idx += 2;
	}

	if (*(buf+idx) & 0xE0)	/* 1st Octet of DSA is in 000x_xxxx */
	{
		DBG_INFO(("frame: invalid 1st octet (%#x)\n",*(buf+idx)));
		return GT_FAIL;
	}
	header->sw_dev_no = (unsigned int)*(buf+idx);
	idx += 1;

	if (*(buf+idx) != 0)				/* 2nd Octet of DSA should be 0 */
	{
		DBG_INFO(("frame: invalid 2nd octet (%#x)\n",*(buf+idx)));
		return GT_FAIL;
	}
	idx += 1;

	if ((*(buf+idx) & 0x1F) != 0x1F)	/* 3nd Octet of DSA should be in xxx1_1111 */
	{
		DBG_INFO(("frame: invalid 3rd octet (%#x)\n",*(buf+idx)));
		return GT_FAIL;
	}

	header->pri = (unsigned int)*(buf+idx) >> 5;
	idx += 1;
	header->seq = (unsigned int)*(buf+idx);
	idx += 1;

	idx += 2;
	header->format = SWAP16(*(unsigned short*)(buf+idx));
	idx += 2;
	header->product = SWAP16(*(unsigned short*)(buf+idx));
	idx += 2;
	header->cmd = SWAP16(*(unsigned short*)(buf+idx));
	idx += 2;

	header->data_len = len - idx;
	header->data = buf + idx;

	return GT_OK;
}

static GT_STATUS rm_parse_dump_atu(RMU_DEV_DUMP_ATU* usr_buf, unsigned char* frame, int frame_len)
{
	unsigned int i, len, entryState = 0;
	GT_ATU_ENTRY* entry;

	len = frame_len;
	entry = &usr_buf->entries[0];

	for (i=0; i<RMU_MAX_ATU_ENTRIES; i++,entry++)
	{
		if (len < 10)
		{
			if (len == 2)
			{
				DBG_INFO(("frame: len==2 ??\n"));
				break;
			}
			DBG_INFO(("frame: len doesn't match with the frame format\n"));
			return GT_FAIL;
		}

		entryState = ((*frame) & 0xF0) >> 4;
		if (entryState == 0)
			break;

		entry->entryState.ucEntryState = entryState;
		entry->portVec = ((*frame++) & 0x7) << 8;
		entry->portVec |= *frame++;
		entry->macAddr.arEther[0] = *frame++;
		entry->macAddr.arEther[1] = *frame++;
		entry->macAddr.arEther[2] = *frame++;
		entry->macAddr.arEther[3] = *frame++;
		entry->macAddr.arEther[4] = *frame++;
		entry->macAddr.arEther[5] = *frame++;
		entry->prio = (*frame & 0xE0) >> 5;
		entry->DBNum = (*frame++ & 0xF) << 8;
		entry->DBNum |= *frame++;

		len -= 10;
	}

	if (entryState == 0)
		usr_buf->from = 0;
	else
		usr_buf->from = SWAP16(*(unsigned short*)frame);
	usr_buf->valid_entries = i;

	return GT_OK;
}

static GT_STATUS rm_parse_dump_mib(RMU_DEV_DUMP_MIB* usr_buf, unsigned char* frame, int frame_len)
{
	GT_STATS_COUNTERS3 counter;
	unsigned int 	*rmonCounter;
	unsigned int	data;
	unsigned int	stat;
	int		len;

	len = frame_len;
	rmonCounter = (unsigned int*)&usr_buf->rmon.counter3;

	if(len < 6)
	{
		DBG_INFO(("frame: invalid frame length\n"));
		return GT_FAIL;
	}

	/* get port information */
	frame++;
	data = (*frame++) >> 3;
	if (data != usr_buf->port)
	{
		DBG_INFO(("frame: wrong port for mib\n"));
		return GT_FAIL;
	}

	data = ((frame[0]) << 24) | ((frame[1]) << 16) |
			((frame[2]) << 8) | (frame[3]);
	frame += 4;

	usr_buf->timeStamp = data;

	len -= 6;

	for(counter=STATS3_InGoodOctetsLo; counter<=STATS3_Late; counter++)
	{
		if(len < 12)
		{
			DBG_INFO(("frame: invalid frame lengh\n"));
			return GT_FAIL;
		}

		stat = ((frame[0]) << 24) | ((frame[1]) << 16) |
				((frame[2]) << 8) | (frame[3]);
		frame += 4;

		*rmonCounter++ = stat;
		len -= 4;
	}

	if(len < 12)
	{
		DBG_INFO(("frame: invalid frame length\n"));
		return GT_FAIL;
	}

	usr_buf->stat.inDiscardHi = ((frame[0]) << 8) | (frame[1]);
	frame += 2;
	usr_buf->stat.inDiscardLo = ((frame[0]) << 8) | (frame[1]);
	frame += 4;
	usr_buf->stat.inFiltered = ((frame[0]) << 8) | (frame[1]);
	frame += 4;
	usr_buf->stat.outFiltered = ((frame[0]) << 8) | (frame[1]);

	return GT_OK;
}


static GT_STATUS rm_parse_rw_reg(HW_DEV_REG_ACCESS* usr_buf, unsigned char* frame, int frame_len)
{
	unsigned char* buf = frame;
	unsigned int i, op, entries = usr_buf->entries;
	HW_DEV_RW_REG* entry;
	HW_DEV_RW_REG  tmpEntry;
	int wobfail = 0;

	for (i=0; i<entries; i++)
	{
		/* frame should be end with 4 bytes of 0xFF */
		if ((*(buf) == 0xFF) && (*(buf+1) == 0xFF) &&
			(*(buf+2) == 0xFF) && (*(buf+3) == 0xFF))
		{
			DBG_INFO(("frame: end of frame found\n"));
			break;
		}

		frame_len -= 4;
		if (frame_len < 4)
		{
			DBG_INFO(("frame: invalid frame len\n"));
			return GT_FAIL;
		}

		entry = &usr_buf->rw_reg_list[i];

		/* parse the frame data */
		op = *buf >> 2;

		tmpEntry.addr = (*buf++ & 0x3) << 3;
		tmpEntry.addr |= (*buf & 0xE0) >> 5;
		tmpEntry.reg =  *buf++ & 0x1F;
		tmpEntry.data = *buf++ << 8;
		tmpEntry.data |= (*buf++);

		switch (op)
		{
			case 1:	/* Write */
				tmpEntry.cmd = RM_REG_WRITE;
				break;
			case 2:	/* Read */
				tmpEntry.cmd = RM_REG_READ;
				break;
			case 4:	/* Wait till 0 */
				tmpEntry.cmd = RM_REG_WAIT_TILL_0;
				wobfail = ((tmpEntry.data & 0xFF) == 0xFF) ? 1 : 0;
				tmpEntry.data >>= 8;
				break;
			case 7:	/* Wait till 1 */
				tmpEntry.cmd = RM_REG_WAIT_TILL_1;
				wobfail = ((tmpEntry.data & 0xFF) == 0xFF) ? 1 : 0;
				tmpEntry.data >>= 8;
				break;
			default:
				DBG_INFO(("frame: unknown op code (%#x)\n",op));
				return GT_FAIL;
		}

		entry->data = tmpEntry.data;

		if (wobfail)
		{
			DBG_INFO(("frame: rw wob request failed\n"));
			i++;
			break;
		}
	}

	usr_buf->entries = i;

	return GT_OK;

}

/*
 * Parse the data of the response frame.
 * Return:
 * 	RM_OK on success
 *		RM_BADMSG on fail
 */
static GT_STATUS rm_parse_data(unsigned char* usrbuf, RMU_FRAME_HEADER *header)
{
	unsigned int len = header->data_len;
	GT_STATUS status;

	DBG_INFO(("frame: rm_parse_data called\n"));

	switch(header->cmd)
	{
		case RMU_CODE_GET_ID:
			status = GT_OK;
			break;

		case RMU_CODE_DUMP_ATU:
			status = rm_parse_dump_atu((RMU_DEV_DUMP_ATU*)usrbuf, header->data, len);
			break;

		case RMU_CODE_DUMP_MIB:
			status = rm_parse_dump_mib((RMU_DEV_DUMP_MIB*)usrbuf, header->data, len);
			break;

		case RMU_CODE_RW_REG:
			status = rm_parse_rw_reg((HW_DEV_REG_ACCESS*)usrbuf, header->data, len);
			break;

		case RMU_CODE_ERROR:
			status = GT_FAIL;
			break;

		default:
			DBG_INFO(("frame: unknown cmd %#x\n",header->cmd));
			status = GT_FAIL;
	}

	return status;
}


static GT_STATUS rm_res_handler(RMU_FRAME_HEADER *req_header, unsigned char* buf, int len )
{
	GT_STATUS status = GT_OK;
	RMU_FRAME_HEADER	header;
	/* unsigned long flags;*/

	DBG_INFO(("rm: rm_res_handler is called\n"));

	status = rm_parse_header(&header, buf, len);
	if (status != GT_OK)
	{
		DBG_INFO(("rm: rm_parse_header return failed\n"));
		return status;
	}

	if (header.cmd == RMU_CODE_GET_ID)
	{
		req_header->format = header.format;
		req_header->product = header.product;
		*(GT_U32 *)req_header->data = (header.format << 16) | header.product;
	}


	/* parse the data */
	status = rm_parse_data(req_header->data, &header);
	if (status != GT_OK)
	{
		DBG_INFO(("rm: rm_parse_data return failed\n"));
		return status;
	}
	return status;
}

/* Build request frame based on the input. */

static GT_U32 gtRmuBuildFrame(RMU_FRAME_HEADER *header, unsigned char *buf)
{
	GT_U16 data, flag;
	GT_U32 len, tmplen;

	len = 0;

	gtMemCpy(buf+len, header->dst_address, RMU_PACKET_DA_SIZE);
	len += RMU_PACKET_DA_SIZE;	/* hw address size */

	gtMemCpy(buf+len, header->src_address, RMU_PACKET_SA_SIZE);
	len += RMU_PACKET_DA_SIZE;	/* hw address size */

	if(RMU_PACKET_ETHER_TYPE_SIZE)
	{
		/* Ether Type */
		*(unsigned short*)(buf+len) = header->protocol;
		len += 2;	/* ether type size */
		*(unsigned short*)(buf+len) = 0;
		len += 2;	/* extra 2 bytes */
	}

	/* DSA Tag */
	*(buf+len) = 0x40 | ((unsigned char)header->sw_dev_no & 0x1F);
	len++;

	*(buf+len) = 0xFA;
	len++;

	*(buf+len) = 0x0F | ((unsigned char)(header->pri << 5));
	len++;

	*(buf+len) = (unsigned char)header->seq;
	len++;

	/* Length/Type field */
	*(unsigned short*)(buf+len) = (GT_U16)SWAP16(RMU_LEN_TYPE);
	len += 2;	/* Length/Type field size */

	/* Request Format */
	switch(header->cmd)
	{
		case RMU_CODE_GET_ID:
			*(unsigned short*)(buf+len) = 0;
			len += 2;	/* 2 bytes of format */

			*(unsigned short*)(buf+len) = 0;
			len += 2;	/* 2 bytes of product id */

			*(unsigned short*)(buf+len) = SWAP16(header->cmd);
			len += 2;	/* 2 bytes of command */

			*(unsigned short*)(buf+len) = 0;
			len += 2;	/* 2 bytes of data */
			break;

		case RMU_CODE_DUMP_ATU:
			data = (unsigned short)(((RMU_DEV_DUMP_ATU*)(header->data))->from);

			*(unsigned short*)(buf+len) = SWAP16(header->format);
			len += 2;	/* 2 bytes of format */

			*(unsigned short*)(buf+len) = 0;
			len += 2;	/* 2 bytes of product id */

			*(unsigned short*)(buf+len) = SWAP16(header->cmd);
			len += 2;	/* 2 bytes of command */

			*(unsigned short*)(buf+len) = SWAP16(data);
			len += 2;	/* 2 bytes of data */
			break;

		case RMU_CODE_DUMP_MIB:
			data = (unsigned short)(((RMU_DEV_DUMP_MIB*)(header->data))->port);
			flag = (unsigned short)(((RMU_DEV_DUMP_MIB*)(header->data))->flag);
			if (flag & RMU_RMON_FLUSH_REQUIRED)
				data |= 0x8000;

			*(unsigned short*)(buf+len) = SWAP16(header->format);
			len += 2;	/* 2 bytes of format */

			*(unsigned short*)(buf+len) = 0;
			len += 2;	/* 2 bytes of product id */

			*(unsigned short*)(buf+len) = SWAP16(header->cmd);
			len += 2;	/* 2 bytes of command */

			*(unsigned short*)(buf+len) = SWAP16(data);
			len += 2;	/* 2 bytes of data */
			break;

		case RMU_CODE_RW_REG:
			*(unsigned short*)(buf+len) = SWAP16(header->format);
			len += 2;	/* 2 bytes of format */

			*(unsigned short*)(buf+len) = 0;
			len += 2;	/* 2 bytes of product id */

			*(unsigned short*)(buf+len) = SWAP16(header->cmd);
			len += 2;	/* 2 bytes of command */

			tmplen = rmuCreateRWFrame(buf+len, (HW_DEV_REG_ACCESS*)(header->data));
			if (tmplen <= 0)
				return 0;
			len += tmplen;	/* size of data */
			break;

		default:
			DBG_INFO(("frame: unknown cmd %#x\n",header->cmd));
			return 0;
	}

	return len;
}


static GT_STATUS gtRmuReqHandler
(
		IN GT_QD_DEV* dev,
		IN GT_U16 cmd,
		INOUT void* data
)
{
	GT_STATUS retVal = GT_OK;
	RMU_FRAME_HEADER header;
	GT_U8 DA[6] = RMU_DEFAULT_DST_ADDRESS;
    GT_U8 SA[6] = RMU_DEFAULT_SRC_ADDRESS;
    GT_U8 reqBuf[RMU_MAX_PACKET_SIZE];
    GT_U8 *rspBuf;
	GT_U32 reqlen, rsplen;

	header.src_address = &SA[0];
	header.dst_address = &DA[0];
	header.protocol = RMU_DEFAULT_ETYPE;
	header.sw_dev_no = dev->devNum;
	header.pri = RMU_FRAME_PRIORITY;
	header.seq = gtRmuGetSeqNum();
	header.cmd = cmd;
	header.format = RMU_REQ_FORMAT;
	header.data = data;
	header.data_len = 0;
	header.product = 0;

	reqlen = gtRmuBuildFrame(&header, reqBuf);
	if (reqlen == 0)
	{
		DBG_INFO(("Error: req packet length is zero.\n"));
		return GT_FAIL;
	}

	retVal = dev->rmuTxRx(reqBuf, reqlen, &rspBuf, &rsplen);
	if (retVal != GT_OK) {
	    DBG_INFO(("Error: rmuTxRx failed\n"));
	    return GT_FAIL;
	}

	retVal = rm_res_handler(&header, rspBuf, rsplen);

	return retVal;
}

GT_STATUS gtRmuGetId
(
		IN GT_QD_DEV *dev,
		OUT GT_U32 *chip_id
)
{
	return gtRmuReqHandler(dev,RMU_CODE_GET_ID,(void *)chip_id);
}

GT_STATUS gtRmuAtuDump
(
		IN GT_QD_DEV *dev,
		OUT RMU_DEV_DUMP_ATU *dump_atu
)
{
	return gtRmuReqHandler(dev,RMU_CODE_DUMP_ATU,(void *)dump_atu);
}

GT_STATUS gtRmuMibDump
(
		IN GT_QD_DEV *dev,
		IN GT_U8 port,
		IN GT_U8 enClearOnRead,
		OUT RMU_DEV_DUMP_MIB *dump_mib
)
{
	dump_mib->port = port;
	dump_mib->flag = enClearOnRead;
	return gtRmuReqHandler(dev,RMU_CODE_DUMP_MIB,(void *)dump_mib);
}

GT_STATUS gtRmuReadRegister
(
		IN GT_QD_DEV* dev,
		IN GT_U32 devAddr,
		IN GT_U32 regAddr,
		OUT GT_U32 *data
)

{
	GT_STATUS retVal;
    HW_DEV_REG_ACCESS regDataR;

	regDataR.entries = 1;
	regDataR.rw_reg_list[0].cmd = HW_REG_READ;
	regDataR.rw_reg_list[0].addr = devAddr;
	regDataR.rw_reg_list[0].reg = regAddr;

	retVal = gtRmuRegAccess(dev, &regDataR);
	if (retVal != GT_OK)
	{
		return -1;
	}

	*data = regDataR.rw_reg_list[0].data;

	return 0;
}

GT_STATUS gtRmuWriteRegister
(
		IN GT_QD_DEV* dev,
		IN GT_U32 devAddr,
		IN GT_U32 regAddr,
		IN GT_U32 data
)
{
	GT_STATUS retVal;
	HW_DEV_REG_ACCESS regDataW;


	regDataW.entries = 1;
	regDataW.rw_reg_list[0].cmd = HW_REG_WRITE;
	regDataW.rw_reg_list[0].addr = devAddr;
	regDataW.rw_reg_list[0].reg = regAddr;
	regDataW.rw_reg_list[0].data = data;

	retVal = gtRmuRegAccess(dev, &regDataW);
	if (retVal != GT_OK)
	{
	    return -1;
	}
	return 0;
}

GT_STATUS gtRmuRegAccess
(
		IN GT_QD_DEV* dev,
		INOUT HW_DEV_REG_ACCESS* ret_data
)
{
    return  gtRmuReqHandler(dev,RMU_CODE_RW_REG,ret_data);
}

GT_STATUS gtSetRmuMode
(
		IN GT_QD_DEV *dev,
		IN GT_U8 port,
		IN GT_U8 enable,
		IN GT_U8 priority
)
{
    GT_U8 retVal;

#ifdef GT_RMGMT_ACCESS
    GT_U8 dsa_type = 0;

    if(RMU_ETHER_TYPE_DSA_ENABLE)
        dsa_type = 3;
    else
        dsa_type = 1;

    if(enable)
    {
        if(dev->rmu.rmuEn == 0)
        {
            retVal = hwSetGlobalRegField(dev, QD_REG_GLOBAL_CONTROL2,8,3,port);/*RMU Mode Set to port 0*/
            if (retVal !=GT_OK) {
                DBG_INFO(("\n Setting register failed in set_rmu_mode\n"));
                return GT_FALSE;
            }
			/* port is set to dsa mode (0x1-DSA mode, 0x3-EtherType DSA mode */
            retVal = hwSetPortRegField(dev, port, QD_REG_PORT_CONTROL,8,2,dsa_type);
            if (retVal !=GT_OK) {
                DBG_INFO(("\n Setting register failed in set_rmu_mode\n"));
                return GT_FALSE;
            }
        }
        dev->rmu.rmuEn = 1;
        dev->use_smi = GT_TRUE;
        /* dev->rmu.prio = priority; */
    }
    else
    {
    	dev->use_smi = GT_FALSE;
        dev->rmu.rmuEn = 0;
    }
#endif

    return GT_OK;
}

