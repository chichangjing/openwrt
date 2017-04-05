/*
 * Copyright (C) 2014 Marvell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/******************************************************************************
* mv_ipc.c
*
* DESCRIPTION:
*	IPC is a shared library (mv_ipc.c and mv_ipc.h) for both LSP and
*	FreeRTOS. It provides API which is used in user application/driver
*	(e.g. mv_ipc_demo.c). The IPC should not contain platform specific part.
*
* DEPENDENCIES:
*	All platform specific part should be hooked to the IPC library, by
*	filling some fields of struct ipc_link_info.
*
******************************************************************************/

#include "mv_ipc.h"

#undef MV_IPC_DEBUG

#ifdef MV_IPC_DEBUG
#define ipc_debug mv_os_printf
#else
#define ipc_debug(...)
#endif /* MV_IPC_DEBUG */

#define mvIpcDbgWrite(x, y)  (x = y);

/* Main data structure - links array */
struct ipc_link mv_ipc_links[MV_IPC_LINKS_NUM];

struct ipc_link_info ipc_links_info[MV_IPC_LINKS_NUM];

mv_bool ipc_is_master(unsigned int link_id)
{
	return ipc_links_info[link_id].is_master;
}

unsigned int ipc_get_remote_node_id(unsigned int link_id)
{
	return ipc_links_info[link_id].remote_node_id;
}

unsigned int ipc_who_am_i(unsigned int link_id)
{
	return mv_ipc_links[link_id].node_id;
}

void *get_shared_mem_offset(unsigned int link_id, void *virt_addr)
{
	GT_UINTPTR sh_mem_virt_addr = (GT_UINTPTR)ipc_links_info[link_id].shmem_virt_addr;

	/* check if virt_addr is from shared memory region */
	if (((GT_UINTPTR)virt_addr >= sh_mem_virt_addr) && ((GT_UINTPTR)virt_addr <
		 (sh_mem_virt_addr + ipc_links_info[link_id].shmem_size)))
		return (void *)((GT_UINTPTR)virt_addr - sh_mem_virt_addr);
	else
		return NULL;
}

/* allocate in shared mem block
 * returns offset in Ipc byte order
 */
static GT_U32 shmem_IpcAlloc(GT_UINTPTR shm_base, GT_U32 *heap, GT_U32 size)
{
	GT_U32 ret = mvU32Host2Ipc(*heap);
	memset((void *)(shm_base + *heap), 0, size);
	*heap += size;
	return ret;
}
/* convert offset in Ipc byte order to virt address */
static void* shmem_IpcOffsetToPtr(GT_UINTPTR shm_base, GT_U32 offset)
{
	return (void*)(shm_base + mvU32Ipc2Host(offset));
}
/* Initialize the IPC mechanism. If called by the IPC master, then it
 * allocates required resources in the shared memory used for the IPC
 * communication. It also copies configuration to the shared memory and fills
 * local structures with data.
 * If called by slave, it calls subroutine for slave configuration or
 * postpones the slave init, depends on master state.
 * It is called from ipc_shmem_link_setup() after the ipc_links_info is
 * filled-out.
 */

/* This routine read configuration from shared memory and fill local
 * structures with data configured by Master. Can be called from ipc_link_start or
 * postponed by and called from ipc_open_chn
 */
static int ipc_slave_config(unsigned int link_id)
{
	unsigned int chn_idx;
	struct ipc_link *link;
	struct ipc_link_shared *linksh;
	GT_UINTPTR shm_base;
	unsigned int tmp_addr;
 	struct ipc_channel_common *channels;

	/* Verify parameters */
	if (link_id > MV_IPC_LINKS_NUM) {
		mv_os_printf("IPC ERROR: IPC Init: Bad link id %d\n", link_id);
		return -1;
	}

	link = &mv_ipc_links[link_id];
	/* Copy ipc_link struct from the shared memory, which was earlier
	 * prepared by IPC master
	 */
	shm_base = (GT_UINTPTR)ipc_links_info[link_id].shmem_virt_addr;
	linksh = (struct ipc_link_shared *)shm_base;
	link->num_of_chn = mvU32Ipc2Host(linksh->num_of_chn);
	link->shmem_size = mvU32Ipc2Host(linksh->shmem_size);
	link->tx_shared_heap_addr = mvU32Ipc2Host(linksh->tx_shared_heap_addr);
	link->tx_shared_heap_size = mvU32Ipc2Host(linksh->tx_shared_heap_size);
	link->rx_shared_heap_addr = mvU32Ipc2Host(linksh->rx_shared_heap_addr);
	link->rx_shared_heap_size = mvU32Ipc2Host(linksh->rx_shared_heap_size);
	link->master_conf_done = mvU32Ipc2Host(linksh->master_conf_done);
	link->slave_link_initialized = mvU32Ipc2Host(linksh->slave_link_initialized);

	/* Override local parameters for link */
	link->node_id = MV_IPC_NODE_ID_SLAVE;
	link->shmem_base_addr = (GT_UINTPTR)ipc_links_info[link_id].shmem_virt_addr;
	link->remote_node_id = ipc_links_info[link_id].remote_node_id;
	link->channels = mv_os_malloc(sizeof(struct ipc_channel) * link->num_of_chn);

	/* Swap rx and tx fields for Heap region partition */
	tmp_addr = link->tx_shared_heap_addr;
	link->tx_shared_heap_addr = link->rx_shared_heap_addr;
	link->rx_shared_heap_addr = tmp_addr;
	tmp_addr = link->tx_shared_heap_size;
	link->tx_shared_heap_size = link->rx_shared_heap_size;
	link->rx_shared_heap_size = tmp_addr;

	/* Initialize all channels */
	channels = (struct ipc_channel_common *)shmem_IpcOffsetToPtr(shm_base, linksh->channels);
	for (chn_idx = 0; chn_idx < link->num_of_chn; chn_idx++) {

		link->channels[chn_idx].state = MV_CHN_CLOSED;
		link->channels[chn_idx].tx_enable = mv_false;
		link->channels[chn_idx].rx_enable = mv_false;
		link->channels[chn_idx].next_rx_msg_idx = 1;
		link->channels[chn_idx].next_tx_msg_idx = 1;

		/* Channel common struct copy and swap TX and RX queue start */
		link->channels[chn_idx].rx_msg_que_va = shmem_IpcOffsetToPtr(shm_base, channels[chn_idx].tx_msg_que_va);
		link->channels[chn_idx].tx_msg_que_va = shmem_IpcOffsetToPtr(shm_base, channels[chn_idx].rx_msg_que_va);
		link->channels[chn_idx].rx_ctrl_msg = (struct ipc_message *)link->channels[chn_idx].rx_msg_que_va;
		link->channels[chn_idx].tx_ctrl_msg = (struct ipc_message *)link->channels[chn_idx].tx_msg_que_va;
		link->channels[chn_idx].msg_rxque_size = mvU32Ipc2Host(channels[chn_idx].msg_txque_size);
		link->channels[chn_idx].msg_txque_size = mvU32Ipc2Host(channels[chn_idx].msg_rxque_size);
		link->channels[chn_idx].rxbuf_size = mvU32Ipc2Host(channels[chn_idx].txbuf_size);
		link->channels[chn_idx].txbuf_size = mvU32Ipc2Host(channels[chn_idx].rxbuf_size);
		link->channels[chn_idx].tx_msg_flag = shmem_IpcOffsetToPtr(shm_base, channels[chn_idx].rx_msg_flag);
		link->channels[chn_idx].rx_msg_flag = shmem_IpcOffsetToPtr(shm_base, channels[chn_idx].tx_msg_flag);

		ipc_debug("IPC HAL: Init channel %d with RxQ = 0x%08x; TxQ = 0x%08x\n",
			       chn_idx, (unsigned int)mvU32Ipc2Host(channels[chn_idx].rx_msg_que_va),
			       (unsigned int)mvU32Ipc2Host(channels[chn_idx].tx_msg_que_va));

		/* Set rx and tx functions */
		link->channels[chn_idx].send_trigger = ipc_links_info[link_id].send_trigger;
		link->channels[chn_idx].enable_chn_doorbell = ipc_links_info[link_id].enable_chn_doorbell;
	}

	return 0;
}

static int ipc_link_start(unsigned int link_id)
{
	unsigned int chn_idx;
	struct ipc_link *link;
	struct ipc_link_shared *linksh;
	GT_U32 heap = 0;
	GT_U32 flagsOffset;
	GT_UINTPTR shm_base;
	struct ipc_channel_common *channels;
	/* The runningOffset is offset in shared memory,
	 * used to compute addresses of queues and heap
	 */
	unsigned int heapSize;
	int txbuf_size, rxbuf_size;

	/* Verify parameters */
	if (link_id > MV_IPC_LINKS_NUM) {
		mv_os_printf("IPC ERROR: IPC Init: Bad link id %d\n", link_id);
		return -1;
	}

	link = &mv_ipc_links[link_id];

	if (mv_true == ipc_links_info[link_id].is_master) {
		/* Master configuration*/
		shm_base = (GT_UINTPTR)ipc_links_info[link_id].shmem_virt_addr;
		shmem_IpcAlloc(shm_base, &heap, sizeof(*linksh));
		linksh = (struct ipc_link_shared *)shm_base;

		link->node_id = MV_IPC_NODE_ID_MASTER;
		linksh->node_id = mvU32Host2Ipc(MV_IPC_NODE_ID_MASTER);
		link->shmem_base_addr = shm_base;
		linksh->shmem_base_addr = 0;
		link->shmem_size = ipc_links_info[link_id].shmem_size;
		linksh->shmem_size = mvU32Host2Ipc(ipc_links_info[link_id].shmem_size);
		link->num_of_chn = ipc_links_info[link_id].num_of_chn;
		linksh->num_of_chn = mvU32Host2Ipc(ipc_links_info[link_id].num_of_chn);
		link->remote_node_id = ipc_links_info[link_id].remote_node_id;
		linksh->remote_node_id = mvU32Host2Ipc(ipc_links_info[link_id].remote_node_id);
		link->channels = (struct ipc_channel *)mv_os_malloc(sizeof(struct ipc_channel) * link->num_of_chn);
		linksh->channels =  shmem_IpcAlloc(shm_base, &heap, sizeof(struct ipc_channel_common) * link->num_of_chn);
		channels = (struct ipc_channel_common *)shmem_IpcOffsetToPtr(shm_base, linksh->channels);

		/* Skip the RX/TX flags in Shared mem */
		flagsOffset = mvU32Ipc2Host(shmem_IpcAlloc(shm_base, &heap, 2 * sizeof(GT_U32) * link->num_of_chn));

		/* Initialize all channels */
		for (chn_idx = 0; chn_idx < link->num_of_chn; chn_idx++) {
			txbuf_size = ipc_links_info[link_id].chn_tx_buffer_info[chn_idx] * 4;
			rxbuf_size = ipc_links_info[link_id].chn_rx_buffer_info[chn_idx] * 4;

			link->channels[chn_idx].state = MV_CHN_CLOSED;
			link->channels[chn_idx].tx_enable = mv_false;
			link->channels[chn_idx].rx_enable = mv_false;
			link->channels[chn_idx].msg_rxque_size = ipc_links_info[link_id].chn_rx_info_array[chn_idx];
			link->channels[chn_idx].msg_txque_size = ipc_links_info[link_id].chn_tx_info_array[chn_idx];
			link->channels[chn_idx].rxbuf_size = rxbuf_size;
			link->channels[chn_idx].txbuf_size = txbuf_size;
			link->channels[chn_idx].next_rx_msg_idx = 1;
			link->channels[chn_idx].next_tx_msg_idx = 1;
			channels[chn_idx].msg_rxque_size = mvU32Host2Ipc(ipc_links_info[link_id].chn_rx_info_array[chn_idx]);
			channels[chn_idx].msg_txque_size = mvU32Host2Ipc(ipc_links_info[link_id].chn_tx_info_array[chn_idx]);
			channels[chn_idx].rxbuf_size = mvU32Host2Ipc(rxbuf_size);
			channels[chn_idx].txbuf_size = mvU32Host2Ipc(txbuf_size);


			/* Set RX queue start move offset to (queue size * message size) */


			/* allocate RX and TX queues */
			channels[chn_idx].rx_msg_que_va = shmem_IpcAlloc(shm_base, &heap,
					link->channels[chn_idx].msg_rxque_size *
					(sizeof(struct ipc_message) + rxbuf_size));
			channels[chn_idx].tx_msg_que_va = shmem_IpcAlloc(shm_base, &heap,
					link->channels[chn_idx].msg_txque_size *
					(sizeof(struct ipc_message) + txbuf_size));

#define _CHN_OFFS_TO_VIRT(_field) \
	link->channels[chn_idx]._field = shmem_IpcOffsetToPtr(shm_base, channels[chn_idx]._field);
			_CHN_OFFS_TO_VIRT(rx_msg_que_va);
			_CHN_OFFS_TO_VIRT(tx_msg_que_va);
			link->channels[chn_idx].rx_ctrl_msg = (struct ipc_message *)link->channels[chn_idx].rx_msg_que_va;
			link->channels[chn_idx].tx_ctrl_msg = (struct ipc_message *)link->channels[chn_idx].tx_msg_que_va;

			ipc_debug("IPC HAL: Init channel %d with RxQ = 0x%08x; TxQ = 0x%08x\n",
				       chn_idx, (unsigned int)mvU32Ipc2Host(channels[chn_idx].rx_msg_que_va),
				       (unsigned int)mvU32Ipc2Host(channels[chn_idx].tx_msg_que_va));

			ipc_debug("IPC HAL: RxQ size %d with %dB buffers; TxQ size %d with %dB buffers,\n",
				link->channels[chn_idx].msg_rxque_size, rxbuf_size,
				link->channels[chn_idx].msg_txque_size, txbuf_size);

			/* Set rx and tx functions */
			link->channels[chn_idx].send_trigger = ipc_links_info[link_id].send_trigger;
			link->channels[chn_idx].enable_chn_doorbell =
			    ipc_links_info[link_id].enable_chn_doorbell;

			channels[chn_idx].tx_msg_flag = mvU32Host2Ipc(flagsOffset + 2 * chn_idx * sizeof(GT_U32));
			channels[chn_idx].rx_msg_flag = mvU32Host2Ipc(flagsOffset + (2 * chn_idx + 1) * sizeof(GT_U32));
			_CHN_OFFS_TO_VIRT(tx_msg_flag);
			_CHN_OFFS_TO_VIRT(rx_msg_flag);

			/* Check if we have enough shared memory to copy channel */
			if (heap > (ipc_links_info[link_id].shmem_size)) {
				ipc_debug("IPC HAL: Init channels allocated 0x%X bytes, shmem is 0x%X bytes\n",
				       heap, ipc_links_info[link_id].shmem_size);
				return 1;
			}
#undef _CHN_OFFS_TO_VIRT
		}

		/* Heap region partition */
		heapSize = ipc_links_info[link_id].shmem_size - heap;
		link->tx_shared_heap_addr = heap;
		link->tx_shared_heap_size = (heapSize * ipc_links_info[link_id].master_free_region_percent)/100;
		heap += link->tx_shared_heap_size;
		link->rx_shared_heap_addr = heap;
		link->rx_shared_heap_size = ipc_links_info[link_id].shmem_size - heap;
		linksh->tx_shared_heap_addr = mvU32Host2Ipc(link->tx_shared_heap_addr);
		linksh->tx_shared_heap_size = mvU32Host2Ipc(link->tx_shared_heap_size);
		linksh->rx_shared_heap_addr = mvU32Host2Ipc(link->rx_shared_heap_addr);
		linksh->rx_shared_heap_size = mvU32Host2Ipc(link->rx_shared_heap_size);

		ipc_debug("running offset %d 0x%x, sizeof chn_common %d, sizeof *link %d\n",
		    heap, heap, sizeof(struct ipc_channel_common), sizeof(*link));

		/* Set magic value in link structures and copy to shared memory,
		 * this is ready state for client
		 */
		link->master_conf_done = MV_IPC_MASTER_CONFIG_MAGIC;
		linksh->master_conf_done = mvU32Host2Ipc(link->master_conf_done);

		ipc_debug("IPC HAL: Initialized interface as Master\n");
	} else { /* Slave configuration */
		shm_base = (GT_UINTPTR)ipc_links_info[link_id].shmem_virt_addr;
		linksh = (struct ipc_link_shared *)shm_base;


		if (mvU32Ipc2Host(linksh->master_conf_done) == MV_IPC_MASTER_CONFIG_MAGIC) {
			/* Master finished the init, Slave get the configuration */
			ipc_slave_config(link_id);

			/* Clear magic */
			link->master_conf_done = 0;
			linksh->master_conf_done = 0;
			link->slave_link_initialized = 0;

			ipc_debug("IPC HAL: Initialized interface as Slave\n");
		} else {
			/* Postpone the Slave init, will be done in ipc_open_chn */
			link->slave_link_initialized = MV_IPC_MASTER_CONFIG_MAGIC;
			ipc_debug("IPC HAL: Initialized interface as Slave, config postponed\n");
		}
	}

	return 0;
}

static int ipc_msg_offset(unsigned int msg_idx, unsigned int buf_size)
{
	return msg_idx * (sizeof(struct ipc_message) + buf_size);
}

static struct ipc_channel *get_chn_pointer(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_link *link;

	/* Verify parameters */
	if (link_id > MV_IPC_LINKS_NUM) {
		mv_os_printf("IPC ERROR: Ack attach: Bad link id %d\n", chn_id);
		return NULL;
	}

	link = &mv_ipc_links[link_id];

	if (chn_id > link->num_of_chn) {
		mv_os_printf("IPC ERROR:Ack attach: Bad channel id %d\n", chn_id);
		return NULL;
	}

	return &link->channels[chn_id];
}

char *ipc_getnext_txbuf(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;
	unsigned int msg_offset;
	struct ipc_message *curr_msg;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return NULL;

	msg_offset = ipc_msg_offset(chn->next_tx_msg_idx, chn->txbuf_size);

	curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->tx_msg_que_va + msg_offset);

	if (mvU32Ipc2Host(curr_msg->is_used) != mv_false) {
		ipc_debug("IPC HAL: No free Tx buff. Can't send, Msg %d used flag = %d\n",
			chn->next_tx_msg_idx, curr_msg->is_used);
		return NULL;
	}

	return (char *)((GT_UINTPTR)curr_msg + sizeof(struct ipc_message));
}

char *ipc_get_rxbuf_of_msg(struct ipc_message *msg)
{
	return (char *)msg + sizeof(struct ipc_message);
}

int ipc_chn_txbuf_max_size(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	return chn->txbuf_size;
}

/* Closes an ipc channel and prepares it for receiving messages */
int ipc_close(unsigned int link_id)
{
	struct ipc_link *link;
	unsigned int chn_idx;

	/* Verify parameters */
	if (link_id > MV_IPC_LINKS_NUM) {
		mv_os_printf("IPC ERROR: IPC close: Bad link id %d\n", link_id);
		return -1;
	}

	link = &mv_ipc_links[link_id];

	/* De-activate all channels */
	for (chn_idx = 0; chn_idx < link->num_of_chn; chn_idx++) {
		ipc_disable_chn_rx(link_id, chn_idx);

		if (link->channels[chn_idx].state == MV_CHN_ATTACHED)
			ipc_dettach_chn(link_id, chn_idx);

		if (link->channels[chn_idx].state == MV_CHN_OPEN)
			ipc_close_chn(link_id, chn_idx);
	}

	ipc_debug("IPC HAL: CLosed IPC interface\n");

	return 0;
}

/* Opens an ipc channel and prepares it for receiving messages */
int ipc_open_chn(unsigned int link_id, unsigned int chn_id, void *dev,
	 MV_IPC_RX_CLBK rx_clbk, MV_IPC_SET_CHN_STAT set_chn_status)
{
	struct ipc_link *link;
	struct ipc_link_shared *linksh;
	struct ipc_channel *chn;
	struct ipc_message *curr_msg;
	GT_UINTPTR shm_base;
	unsigned int msg_id, msg_offset;
	int status;

	/* Verify parameters */
	if (link_id > MV_IPC_LINKS_NUM) {
		mv_os_printf("IPC ERROR: Open Chn: Bad link id %d\n", chn_id);
		return -1;
	}

	link = &mv_ipc_links[link_id];

	/* Check if postponed Slave init needed */
	if (link->slave_link_initialized == MV_IPC_MASTER_CONFIG_MAGIC) {
		/* Read link structure from shared mem */
		shm_base = (GT_UINTPTR)ipc_links_info[link_id].shmem_virt_addr;
		linksh = (struct ipc_link_shared *)shm_base;
		link->master_conf_done = mvU32Ipc2Host(linksh->master_conf_done);
		if (link->master_conf_done == MV_IPC_MASTER_CONFIG_MAGIC) {
			/* Master finished the init, Slave get the configuration */
			status = ipc_slave_config(link_id);
			mv_os_printf("IPC MESSG: Open Chn:Postponed init done with status %d\n", status);

			/* Clear magic */
			link->master_conf_done = 0;
			linksh->master_conf_done = 0;
			link->slave_link_initialized = 0;
		} else {
			/* Master still not wake, cannot open the channel */
			mv_os_printf("IPC WARNG: Open Chn: Master not ready\n");
			link->slave_link_initialized = MV_IPC_MASTER_CONFIG_MAGIC;
			return MV_NOT_STARTED;
		}
	}

	if (chn_id > link->num_of_chn) {
		mv_os_printf("IPC ERROR: Open Chn: Bad channel id %d\n", chn_id);
		return -1;
	}

	chn = &link->channels[chn_id];

	if (chn->state != MV_CHN_CLOSED) {
		mv_os_printf("IPC ERROR: Can't open channel %d. It is already open %d\n",
			       chn_id, chn->state);
		return -1;
	}

	/* Initialize the transmit queue */
	for (msg_id = 0; msg_id < chn->msg_txque_size; msg_id++) {
		msg_offset = ipc_msg_offset(msg_id, chn->txbuf_size);
		curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->tx_msg_que_va + msg_offset);
		curr_msg->is_used = mvU32Host2Ipc(mv_false);
	}

	/* Initialize channel members */
	chn->state = MV_CHN_OPEN;
	chn->next_rx_msg_idx = 1;
	chn->next_tx_msg_idx = 1;
	chn->rx_enable = mv_true;
	chn->rx_callback = rx_clbk;
	chn->set_chn_status = set_chn_status;
	chn->private_data = dev;

	ipc_debug("IPC HAL: Opened channel %d successfully\n", chn_id);

	return 0;
}

/* Acknowledges and Attach request from receiver. */
static int ipc_ack_attach(unsigned int link_id, unsigned int chn_id, int acknowledge)
{
	struct ipc_link *link;
	struct ipc_channel *chn;
	struct ipc_message attach_msg;
	int status;

	/* Verify parameters */
	if (link_id > MV_IPC_LINKS_NUM) {
		mv_os_printf("IPC ERROR: Ack attach: Bad link id %d\n", chn_id);
		return -1;
	}

	link = &mv_ipc_links[link_id];

	if (chn_id > link->num_of_chn) {
		mv_os_printf("IPC ERROR:Ack attach: Bad channel id %d\n", chn_id);
		return -1;
	}

	chn = &link->channels[chn_id];

	/* Cannot acknowledge remote attach until local attach was requested */
	if ((chn->state != MV_CHN_ATTACHED) && (chn->state != MV_CHN_LINKING)) {
		ipc_debug("IPC HAL: Can't acknowledge attach. Channel in state %d\n", chn->state);
		return -1;
	}

	if (acknowledge == mv_true) {
		/* Check that channel is not already coupled to another CPU */
		if (chn->remote_node_id != link->remote_node_id) {
			ipc_debug("IPC HAL: Can't acknowledge attach. CPU %d != %d\n",
				       chn->remote_node_id, link->remote_node_id);
			return -1;
		}

		ipc_debug("IPC HAL: Acknowledging attach from CPU %d\n", link->remote_node_id);

		/* Send the attach acknowledge message */
		attach_msg.type = IPC_MSG_ATTACH_ACK;
		attach_msg.value = link->remote_node_id;
		attach_msg.size = 0;
		status = ipc_tx_ctrl_msg(link_id, chn_id, &attach_msg);
		if (status != 0) {
			mv_os_printf("IPC ERROR: Cannot Send attach acknowledge message\n");
			return -1;
		}
	}

	/* Now change my own state to attached */
	chn->state = MV_CHN_ATTACHED;

	if (chn->set_chn_status)
		chn->set_chn_status(1, chn->private_data);

	ipc_debug("IPC HAL: Attached channel %d\n", chn_id);

	return 0;
}

/* Acknowledges detach request from receiver. This closes the channel for
 * transmission and resets the queues
 */
static int ipc_ack_detach(unsigned int link_id, unsigned int chn_id, int acknowledge)
{
	struct ipc_channel *chn;
	struct ipc_message dettach_msg;
	int status;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	/* Cannot acknowledge remote detach until local attach was requested */
	if ((chn->state != MV_CHN_ATTACHED) && (chn->state != MV_CHN_UNLINKING)) {
		ipc_debug("IPC HAL: Can't acknowledge detach. Channel in state %d\n", chn->state);
		return -1;
	}

	if (acknowledge == mv_true) {
		/* Send the attach acknowledge message */
		dettach_msg.type = IPC_MSG_DETACH_ACK;
		dettach_msg.size = 0;
		dettach_msg.value = 0;

		status = ipc_tx_ctrl_msg(link_id, chn_id, &dettach_msg);
		if (status != 0) {
			mv_os_printf("IPC ERROR: Cannot Send detach acknowledge message\n");
			return -1;
		}
	}

	/* Now change my own state to linking */
	chn->state = MV_CHN_LINKING;

	if (chn->set_chn_status)
		chn->set_chn_status(0, chn->private_data);

	ipc_debug("IPC HAL: Acknowledging detach message\n");

	return 0;
}

static void ipc_rx_ctrl_msg(unsigned int link_id, unsigned int chn_id, struct ipc_message *msg);

/* Ask receiver to acknowledge attach request. To verify reception, message
 * transmission is possible only after receiver acknowledges the attach
 */
static int ipc_req_attach(unsigned int link_id, struct ipc_channel *chn, unsigned int chn_id)
{
	struct ipc_message attach_msg;
	int status;
	int backoff = 10, timeout = 50;

	ipc_debug("IPC HAL: Requesting attach from cpu %d\n", chn->remote_node_id);

	/* Send the attach message */
	attach_msg.type = IPC_MSG_ATTACH_REQ;
	attach_msg.value = ipc_who_am_i(link_id);
	status = ipc_tx_ctrl_msg(link_id, chn_id, &attach_msg);
	if (status != 0) {
		mv_os_printf("IPC ERROR: Cannot Send attach req message\n");
		return -1;
	}

	/* Give the receiver 100 usecs to reply */
	while ((chn->state != MV_CHN_ATTACHED) && timeout) {
#if 0
		ipc_rx_msg(link_id, chn_id, NULL);
#else
		/* process control messages like attach, detach, close */
		if (mvU32Ipc2Host(chn->rx_ctrl_msg->is_used) == mv_true)
			ipc_rx_ctrl_msg(link_id, chn_id, chn->rx_ctrl_msg);
#endif
		udelay(backoff);
		timeout--;
	}

	if (chn->state != MV_CHN_ATTACHED) {
		ipc_debug("IPC HAL: Cannot complete attach sequence. no reply from receiver after %d usec\n",
			       timeout * backoff);
		return -1;
	}

	ipc_debug("IPC HAL: Attached channel %d\n", chn_id);

	return 0;
}

/* Attempts to attach the TX queue to a remote CPU by sending a ATTACH ACK
 * messages to receiver. If the message is acknowledged, the channel state
 * becomes attached and message transmission is enabled.
 */
int ipc_attach_chn(unsigned int link_id, unsigned int chn_id, unsigned int remote_node_id, int *attached)
{
	struct ipc_channel *chn;
	struct ipc_message *curr_msg;
	unsigned int msg_id, msg_offset;
	int status;

	(*attached) = 0;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	if (chn->state == MV_CHN_CLOSED) {
		mv_os_printf("IPC ERROR: Can't attach channel %d. It is closed\n", chn_id);
		return -1;
	}

	if (chn->state == MV_CHN_ATTACHED) {
		(*attached) = 1;
		return 0;
	}

	chn->state = MV_CHN_LINKING;
	chn->remote_node_id = remote_node_id;
	chn->tx_enable = mv_true;

	/* Initialize the transmit queue */
	for (msg_id = 1; msg_id < chn->msg_txque_size; msg_id++) {
		msg_offset = ipc_msg_offset(msg_id, chn->txbuf_size);
		curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->tx_msg_que_va + msg_offset);
		curr_msg->is_used = mvU32Host2Ipc(mv_false);
	}

	/* Send req for attach to other side */
	status = ipc_req_attach(link_id, chn, chn_id);
	if (status == 0) {
		(*attached) = 1;
		ipc_debug("IPC HAL: Attached channel %d to link %d\n", chn_id, link_id);
	}

	return 0;
}

/* Detaches the channel from remote CPU. It notifies remote CPU, by sending
 * control message and waits for acknowledge. After calling this function data
 * messages cannot be sent anymore.
 */
int ipc_dettach_chn(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;
	struct ipc_message msg;
	int status;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	if (chn->state != MV_CHN_ATTACHED) {
		mv_os_printf("IPC ERROR: Detach: channel %d is not attached\n", chn_id);
		return -1;
	}

	msg.type = IPC_MSG_DETACH_REQ;
	msg.size = 0;
	msg.value = 0;

	status = ipc_tx_ctrl_msg(link_id, chn_id, &msg);
	if (status != 0) {
		mv_os_printf("IPC ERROR: Cannot Send detach request message\n");
		return -1;
	}

	chn->remote_node_id = 0;
	chn->state = MV_CHN_UNLINKING;

	return 0;
}

/* Closes the IPC channels. This disables the channels ability to receive messages */
int ipc_close_chn(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	if (chn->state == MV_CHN_CLOSED) {
		mv_os_printf("IPC ERROR: Close channel: Channel %d is already closed\n", chn_id);
		return -1;
	}

	chn->state = MV_CHN_CLOSED;
	chn->tx_enable = mv_false;
	chn->rx_enable = mv_false;
	chn->remote_node_id = 0;

	ipc_debug("IPC HAL: Closed channel %d successfully\n", chn_id);

	return 0;
}

/* Checks if the channel is ready to transmit */
mv_bool ipc_tx_ready(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;
	struct ipc_message *curr_msg;
	unsigned int msg_offset;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return mv_false;

	if (chn->state != MV_CHN_ATTACHED) {
		mv_os_printf("IPC ERROR: Tx Test: channel not attached, state is %d\n", chn->state);
		return mv_false;
	}

	/* Is next message still used by receiver, yes means full queue or bug */
	msg_offset = ipc_msg_offset(chn->next_tx_msg_idx, chn->txbuf_size);
	curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->tx_msg_que_va + msg_offset);
	if (mvU32Ipc2Host(curr_msg->is_used) != mv_false) {
		ipc_debug("IPC HAL: Tx Test: Can't send, Msg %d used flag = %d\n",
					chn->next_tx_msg_idx, curr_msg->is_used);
		return mv_false;
	}

	return mv_true;
}

/* Sends a control message to other side. These messages are not forwarded to the user */
int ipc_tx_ctrl_msg(unsigned int link_id, unsigned int chn_id, struct ipc_message *in_msg)
{
	struct ipc_channel *chn;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	if (chn->tx_enable == mv_false) {
		mv_os_printf("IPC ERROR: Tx Ctrl msg: Tx not enabled\n");
		return -1;
	}

	/* Write the message and pass */
	chn->tx_ctrl_msg->type = mvU32Host2Ipc(in_msg->type);
	chn->tx_ctrl_msg->size = mvU32Host2Ipc(in_msg->size);
	chn->tx_ctrl_msg->value = mvU32Host2Ipc(in_msg->value);

	/* Make sure the msg values are written before the used flag
	 * to ensure the polling receiver will get valid message once
	 * it detects is_used == mv_true.
	 */
	dmb();

	chn->tx_ctrl_msg->is_used = mvU32Host2Ipc(mv_true);

	mvIpcDbgWrite(chn->tx_ctrl_msg->align[0], mvU32Host2Ipc(MV_IPC_HAND_SHAKE_MAGIC));
	mvIpcDbgWrite(chn->tx_ctrl_msg->align[1], mvU32Host2Ipc(0));
	mvIpcDbgWrite(chn->tx_ctrl_msg->align[2], mvU32Host2Ipc(0));

	ipc_debug("IPC HAL: Sent control message 0x%8x on channel %d to link %d\n",
			(int)chn->tx_ctrl_msg, chn_id, link_id);

	/* Raise the TX ready flag and send the trigger */
	*(chn->tx_msg_flag) = mvU32Host2Ipc(0x1);

	/* If the send_trigger function registered, call it */
	if (chn->send_trigger != NULL)
		chn->send_trigger(chn->remote_node_id, chn_id, ipc_links_info[link_id].dev_id);

	return 0;
}

/* Main transmit function */
int ipc_tx_msg(unsigned int link_id, unsigned int chn_id, struct ipc_message *in_msg)
{
	struct ipc_channel *chn;
	struct ipc_message *curr_msg;
	unsigned int msg_offset;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	/* Test if TX ready to send */
	if (chn->state != MV_CHN_ATTACHED) {
		mv_os_printf("IPC ERROR: Tx Msg: channel not attached, state is %d\n", chn->state);
		return -1;
	}

	msg_offset = ipc_msg_offset(chn->next_tx_msg_idx, chn->txbuf_size);
	curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->tx_msg_que_va + msg_offset);
	/* Is next message still used by receiver, yes means full queue or bug */
	if (mvU32Ipc2Host(curr_msg->is_used) != mv_false) {
		ipc_debug("IPC HAL: Tx Msg: Can't send, Msg %d used flag = %d\n",
			chn->next_tx_msg_idx, curr_msg->is_used);
		return -1;
	}

	/* Write the message */
	if (in_msg->size > chn->txbuf_size) {
		mv_os_printf("IPC ERROR: Tx Msg: trying to send %dB data, while tx_buf size is %dB\n",
		    in_msg->size, chn->txbuf_size);
		return -1;
	}

	curr_msg->type = mvU32Host2Ipc(in_msg->type);
	curr_msg->size = mvU32Host2Ipc(in_msg->size);
	curr_msg->value = mvU32Host2Ipc(in_msg->value);

	/* Make sure the msg values are written before the used flag
	 * to ensure the polling receiver will get valid message once
	 * it detects is_used == mv_true.
	 */
	dmb();

	/* Pass ownership to remote cpu */
	curr_msg->is_used = mvU32Host2Ipc(mv_true);

	mvIpcDbgWrite(curr_msg->align[0], mvU32Host2Ipc(MV_IPC_HAND_SHAKE_MAGIC));
	mvIpcDbgWrite(curr_msg->align[1], mvU32Host2Ipc(0));
	mvIpcDbgWrite(curr_msg->align[2], mvU32Host2Ipc(0));

	chn->next_tx_msg_idx++;
	if (chn->next_tx_msg_idx == chn->msg_txque_size)
		chn->next_tx_msg_idx = 1;

	ipc_debug("IPC HAL: Sent message %d on channel %d to link %d\n",
			chn->next_tx_msg_idx - 1, chn_id, link_id);

	/* Raise the TX ready flag and send the trigger */
	*(chn->tx_msg_flag) = mvU32Host2Ipc(0x1);

	/* There is no need to register send_trigger */
	if (chn->send_trigger != NULL)
		chn->send_trigger(chn->remote_node_id, chn_id, ipc_links_info[link_id].dev_id);

	return 0;
}

/* This routine initializes IPC channel: setup receive queue and enable data receiving */
static void ipc_rx_ctrl_msg(unsigned int link_id, unsigned int chn_id, struct ipc_message *msg)
{
	ipc_debug("IPC HAL: Processing control message %d\n", msg->type);

	switch (mvU32Ipc2Host(msg->type)) {
	case IPC_MSG_ATTACH_REQ:
		ipc_ack_attach(link_id, chn_id, mv_true);
		break;

	case IPC_MSG_ATTACH_ACK:
		ipc_ack_attach(link_id, chn_id, mv_false);
		break;

	case IPC_MSG_DETACH_REQ:
		ipc_ack_detach(link_id, chn_id, mv_true);
		break;

	case IPC_MSG_DETACH_ACK:
		ipc_ack_detach(link_id, chn_id, mv_false);
		break;

	default:
		ipc_debug("IPC HAL: Unknown internal message type %d\n", msg->type);
	}

	mvIpcDbgWrite(msg->align[2], mvU32Host2Ipc(MV_IPC_HAND_SHAKE_MAGIC));

	ipc_release_msg(link_id, chn_id, msg);
}

/* Masks the given channel in ISR */
void ipc_disable_chn_rx(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return;

	chn->enable_chn_doorbell(link_id, chn_id, mv_false, ipc_links_info[link_id].dev_id);

	ipc_debug("IPC HAL: Disabled ISR for link %d, channel %d\n", link_id, chn_id);
	return;
}

/* Unmasks the given channel in ISR */
void ipc_enable_chn_rx(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return;

	if (chn->enable_chn_doorbell)
		chn->enable_chn_doorbell(link_id, chn_id, mv_true, ipc_links_info[link_id].dev_id);

	ipc_debug("IPC HAL: Enabled ISR for link %d, channel %d\n", link_id, chn_id);
	return;
}

/* Main Rx routine - should be called from interrupt routine
 * With usage of ipc_rx_msg function two paths of rx processing are allowed:
 * 1) if user callback (rx_callback) registered, process the msg using the
 * user callback.
 * 2) if user callback not registered, IPC layer will copy the msg buffer
 * to the indicated address (*msg_data).
 * User should be aware that 2) option requires memcpy, while using 1) one,
 * memcpy is not a must - user can operate on msg buffer, which is placed in
 * shared memory.
 *
 * The ipc layer is responsible for releasing msgs: after calling rx_callback
 * or after copying msg buffer to the indicated address.
 */
int ipc_rx_msg(unsigned int link_id, unsigned int chn_id, char *msg_data)
{
	struct ipc_channel *chn;
	struct ipc_message *curr_msg;
	unsigned int msg_offset;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	if (chn->state == MV_CHN_CLOSED)
		return -1;

	/* First process control messages like attach, detach, close */
	if (mvU32Ipc2Host(chn->rx_ctrl_msg->is_used) == mv_true)
		ipc_rx_ctrl_msg(link_id, chn_id, chn->rx_ctrl_msg);

	if (msg_data == NULL && chn->rx_callback == NULL)
		return 0;

	msg_offset = ipc_msg_offset(chn->next_rx_msg_idx, chn->rxbuf_size);
	curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->rx_msg_que_va + msg_offset);

	/* Check for unread data messages in queue */
	if (mvU32Ipc2Host(curr_msg->is_used) != mv_true) {
		/* No more messages, disable RX ready flag */
		*(chn->rx_msg_flag) = mvU32Host2Ipc(0x0);
		return MV_NO_MORE;
	}

	/* Increment msg idx to keep in sync with sender */
	chn->next_rx_msg_idx++;
	if (chn->next_rx_msg_idx == chn->msg_rxque_size)
		chn->next_rx_msg_idx = 1;

	/* Check if channel is ready to receive messages */
	if (chn->state < MV_CHN_OPEN) {
		mv_os_printf("IPC ERROR: Rx msg: Channel not ready, state = %d\n", chn->state);
		return 1;
	}

	mvIpcDbgWrite(curr_msg->align[2], mvU32Host2Ipc(MV_IPC_HAND_SHAKE_MAGIC));

	/* Now process user messages */
	ipc_debug("IPC HAL: Received message %d on channel %d\n",
			chn->next_rx_msg_idx - 1, chn_id);

	/* Call user function to care the message, if function missing, copy
	 * data to the msg_data pointer. Please note that the 1 solution (call
	 * rx_callback) msg can avoid copying */
	if (chn->rx_callback)
		chn->rx_callback(curr_msg, chn->private_data);
	else
		memcpy(msg_data, ipc_get_rxbuf_of_msg(curr_msg), curr_msg->size);

	ipc_release_msg(link_id, chn_id, curr_msg);

	return 0;
}

/* Check if RX flag raised */
mv_bool ipc_rx_msg_flag_check(unsigned int link_id, unsigned int chn_id)
{
	struct ipc_channel *chn;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return mv_false;

	if (chn->state == MV_CHN_CLOSED)
		return mv_false;

	if (mvU32Ipc2Host(*(chn->rx_msg_flag)) == 0x1)
		return mv_true;
	else
		return mv_false;
}

/* Return ownership on message to transmitter */
int ipc_release_msg(unsigned int link_id, unsigned int chn_id, struct ipc_message *msg)
{
	struct ipc_channel *chn;

	chn = get_chn_pointer(link_id, chn_id);
	if (!chn)
		return -1;

	if (chn->state == MV_CHN_CLOSED) {
		mv_os_printf("IPC ERROR: Msg release: Inactive channel id %d\n", chn_id);
		return -1;
	}

	if (mvU32Ipc2Host(msg->is_used == mv_false)) {
		mv_os_printf("IPC ERROR: Msg release: Msg %d owned by %d\n",
			chn->next_rx_msg_idx, msg->is_used);
		return -1;
	}

	msg->is_used = mvU32Host2Ipc(mv_false);
	mvIpcDbgWrite(msg->align[1], mvU32Host2Ipc(MV_IPC_HAND_SHAKE_MAGIC));

	ipc_debug("IPC HAL: Released message 0x%8x on channel %d\n", (int)msg, chn_id);

	return 0;
}

#if 0
/* Malloc buffer in shared memory heap for TX buffers
 * (Sequential malloc, no free allowed)
 */
void *ipc_sh_malloc(unsigned int link_id, unsigned int size)
{
	struct ipc_link *link;
	void *ptr;

	if (link_id > MV_IPC_LINKS_NUM) {
		mv_os_printf("IPC ERROR: Tx Msg: Bad link id %d\n", link_id);
		return NULL;
	}

	link = &mv_ipc_links[link_id];

	if (size > link->tx_shared_heap_size)
		return NULL;

	ptr = (void *)link->tx_shared_heap_addr;

	link->tx_shared_heap_addr += size;
	link->tx_shared_heap_size -= size;

	return ptr;
}
#endif

/* Process all Rx msg in all links and channels */
void ipc_rx_process(unsigned int link_id, char *enabled_chn, char *msg_data)
{
	unsigned int chn_id;
	int read_msgs = IPC_RX_MAX_MSGS_PER_ISR;

	/* NOTE: This ISR may be customised by user application requirements to
	 * make it more efficient
	 */

	/* Scan all rx flags */
	for (chn_id = 0; chn_id < ipc_links_info[link_id].num_of_chn; chn_id++) {
		/* Check if RX flag raised */
		if ((enabled_chn[chn_id] == 1) &&
		    (ipc_rx_msg_flag_check(link_id, chn_id) == mv_true)) {
			/* If ready to RX, start get the messages */
			ipc_debug("Got message in channel %d\n", chn_id);
			while (read_msgs) {
				if (ipc_rx_msg(link_id, chn_id, msg_data) != 0)
					break;
				read_msgs--;
			}
		}
	}
}

/* Process Rx msg basing on link id and channel id */
int ipc_rx_queue_process(unsigned int link_id, unsigned int chn_id, char *msg_data)
{
	if (ipc_rx_msg_flag_check(link_id, chn_id) == mv_true) {
		/* If ready to RX, start get the messages */
		ipc_debug("Got message in channel %d\n", chn_id);
		if (ipc_rx_msg(link_id, chn_id, msg_data) != 0)
			return -1;
		else
		/* Return 1 on receiving the new message
		   successfully. */
			return 1;
	}

	/* If the channel is not in illigal status
	   or there is no pending messages in the
	   rx queue, return '0' sliencely */
	return 0;
}

/* Send 'data' using link id and channel id. The 'data' will be copy to the next
 * free tx buffer, required fields of msg header will be filled and ipc_tx_msg
 * subroutine will be called
 */
int ipc_tx_queue_send(unsigned int link_id, unsigned int chn_id, const char *data, unsigned int length)
{
	int status, txbuf_size;
	struct ipc_message msg;

	char *ipc_demo_msg_buffer = ipc_getnext_txbuf(link_id, chn_id);
	if (!ipc_demo_msg_buffer) {
		mv_os_printf("IPC ERROR: No free buffer for tx in link %d and chn %d.\n",
			link_id, chn_id);
		return -1;
	}

	txbuf_size = ipc_chn_txbuf_max_size(link_id, chn_id);
	if (txbuf_size < 0) {
		mv_os_printf("IPC ERROR: unable to get txbuf max size in link %d, chn %d.\n",
			link_id, chn_id);
		return -1;
	}
	if ((int)length > txbuf_size) {
		mv_os_printf("IPC ERROR: TX length %d beyond TX Buffer size %d.\n",
			length, txbuf_size);
		return -1;
	}

	memcpy(ipc_demo_msg_buffer, data, length);

	msg.type = 0;
	msg.size = length;
	msg.value = ipc_who_am_i(link_id);

	ipc_debug("IPC: msg prepare to send in buffer %p(shmem offset), sh_virt_addr %p\n",
		get_shared_mem_offset(link_id, ipc_demo_msg_buffer), ipc_demo_msg_buffer);

	status = ipc_tx_msg(link_id, chn_id, &msg);

	if (status == -1)
		mv_os_printf("IPC ERROR: TX queue busy\n");

	return status;
}

/* Fill the local structures basing on link_info data, call platform-specific
 * irq_init subroutine and then initialize link by calling ipc_link_start
 */
int ipc_shmem_link_setup(unsigned char link_id, struct ipc_link_info link_info)
{
	int ret;

	ipc_links_info[link_id] = link_info;

	if (link_info.irq_init) {
		ret = link_info.irq_init(link_id, link_info.dev_id);
		if (ret != 0) {
			mv_os_printf("IPC: IPC HAL initialization failed - irq init\n");
			return ret;
		}
	}

	ret = ipc_link_start(link_id);
	if (ret != 0) {
		mv_os_printf("IPC: IPC HAL initialization failed\n");
		return ret;
	}

	return 0;
}

int ipc_shmem_link_cleanup(unsigned char link_id)
{
	struct ipc_link *link;
	link = &mv_ipc_links[link_id];

	mv_os_free(link->channels);

	return 0;
}
#define IPC_INCLUDE_DUMP
#ifdef IPC_INCLUDE_DUMP
void ipc_link_dump(unsigned int link_id)
{
    struct ipc_link_info *link_info;
	struct ipc_link *link;
	struct ipc_link_shared *linksh;
	struct ipc_channel *chn;
	GT_UINTPTR shm_base;
    unsigned int chn_id;
	struct ipc_message *curr_msg;
	unsigned int msg_offset, curr_rx_used, curr_tx_used;

	/* Verify parameters */
	if (link_id > MV_IPC_LINKS_NUM) {
		cpssOsPrintf("IPC ERROR: IPC Init: Bad link id %d\n", link_id);
		return;
	}

	link = &mv_ipc_links[link_id];
    link_info = &(ipc_links_info[link_id]);
	shm_base = (GT_UINTPTR)link_info->shmem_virt_addr;
    linksh = (struct ipc_link_shared *)shm_base;
    cpssOsPrintf("Link %d num_channels=%d master_c=0x%x slave_c=0x%x\n",
            link_id,
            mvU32Ipc2Host(linksh->num_of_chn),
            mvU32Ipc2Host(linksh->master_conf_done),
            mvU32Ipc2Host(linksh->slave_link_initialized));
    for (chn_id = 0; chn_id < link->num_of_chn; chn_id++)
    {
	    chn = get_chn_pointer(link_id, chn_id);
        cpssOsPrintf("  Chn %d local: state=%d\n", chn_id, chn->state);
        cpssOsPrintf("    rx_e=%d tx_e=%d rx_i=%d tx_i=%d\n",
                chn->rx_enable,
                chn->tx_enable,
                chn->next_rx_msg_idx,
                chn->next_tx_msg_idx);
        cpssOsPrintf("    rx_msg_flag=0x%x tx_msg_flag=0x%x\n",
                mvU32Ipc2Host(*(chn->rx_msg_flag)),
                mvU32Ipc2Host(*(chn->tx_msg_flag)));
        cpssOsPrintf("    rx_ctrl_msg->is_used=0x%x tx_ctrl_msg->is_used=0x%x\n",
                mvU32Ipc2Host(chn->rx_ctrl_msg->is_used),
                mvU32Ipc2Host(chn->tx_ctrl_msg->is_used));
        curr_rx_used = 0xff;
        curr_tx_used = 0xff;
        if (chn->next_rx_msg_idx < chn->msg_rxque_size)
        {
	        msg_offset = ipc_msg_offset(chn->next_rx_msg_idx, chn->rxbuf_size);
	        curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->rx_msg_que_va + msg_offset);
            curr_rx_used = mvU32Ipc2Host(curr_msg->is_used);
        }
        if (chn->next_tx_msg_idx < chn->msg_txque_size)
        {
	        msg_offset = ipc_msg_offset(chn->next_tx_msg_idx, chn->txbuf_size);
	        curr_msg = (struct ipc_message *)((GT_UINTPTR)chn->tx_msg_que_va + msg_offset);
            curr_tx_used = mvU32Ipc2Host(curr_msg->is_used);
        }
        cpssOsPrintf("    curr_rx_msg->is_used=0x%x curr_tx_msg->is_used=0x%x\n",
                curr_rx_used, curr_tx_used);
    }
}

int ipc_link_dump_bc2(void)
{
    ipc_link_dump(IPC_SCPU_FREERTOS_LINK_ID);
    return 0;
}
int ipc_link_dump_cm3(void)
{
    ipc_link_dump(IPC_CM3_FREERTOS_LINK_ID);
    return 0;
}
#endif /* IPC_INCLUDE_DUMP */
