/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChErrataMng.h
*
* DESCRIPTION:
*       DxCh manager for the Errata issues.
*
*
* FILE REVISION NUMBER:
*       $Revision: 124 $
*
*******************************************************************************/

#ifndef __prvCpssDxChErrataMngh
#define __prvCpssDxChErrataMngh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/generic/port/cpssPortTx.h>

/* macro for references purpose only -- must be 0 --> never 1 */
#define PRV_CPSS_DXCH_ERRATA_REFERENCE_CNS  0

/* number GT_U32 bmps needed to hold the known FEr errata bmp */
#define PRV_CPSS_DXCH_ERRATA_BMP_MAX_NUM_CNS \
                (1 + (((PRV_CPSS_DXCH_ERRATA_MAX_NUM_E)+1) >> 5))

/* get if the erratum need to be WA by CPSS
    devNum - the device id of the DxCh device
    FErNum -- id of the erratum
*/
#define PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,FErNum) \
  (((PRV_CPSS_DXCH_PP_MAC(devNum))->errata.FErArray[(FErNum)>>5] &(1<<((FErNum)&0x1f)))? \
      GT_TRUE : GT_FALSE)

/* set that the erratum need to be WA by CPSS
    devNum - the device id of the DxCh device
    FErNum -- 1 based number
*/
#define PRV_CPSS_DXCH_ERRATA_SET_MAC(devNum,FErNum) \
    (PRV_CPSS_DXCH_PP_MAC(devNum))->errata.FErArray[(FErNum)>>5] |= (1<<((FErNum)&0x1f))

/* set that the erratum NOT need to be WA by CPSS
    devNum - the device id of the DxCh device
    FErNum -- 1 based number
*/
#define PRV_CPSS_DXCH_ERRATA_CLEAR_MAC(devNum,FErNum) \
    (PRV_CPSS_DXCH_PP_MAC(devNum))->errata.FErArray[(FErNum)>>5] &= ~(1<<((FErNum)&0x1f))

/* stack port base for WA MAC counters shadows */
#define PRV_CPSS_DXCH_ERRATA_STACK_PORT_BASE_CNS 24

/* stack port amont for WA MAC counters shadows */
#define PRV_CPSS_DXCH_ERRATA_STACK_PORT_AMOUNT_CNS 4

/* stack port max-number for WA MAC counters shadows */
#define PRV_CPSS_DXCH_ERRATA_STACK_PORT_MAX_CNS \
    (PRV_CPSS_DXCH_ERRATA_STACK_PORT_BASE_CNS + \
        PRV_CPSS_DXCH_ERRATA_STACK_PORT_AMOUNT_CNS - 1)

/* stack ports with WA for not working clear-on-read mode */
#define PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_MAC(_devNum, _port) \
    ((GT_TRUE == PRV_CPSS_DXCH_ERRATA_GET_MAC(_devNum,                    \
        PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E))       \
        && (_port >= PRV_CPSS_DXCH_ERRATA_STACK_PORT_BASE_CNS)            \
        && (_port <= PRV_CPSS_DXCH_ERRATA_STACK_PORT_MAX_CNS))

/*
 * Typedef: struct PRV_CPSS_DXCH_PROFILE_INFO_STC
 *
 * Description: A structure to hold all PP data needed profiles information for
 *              IPM bridge copy get dropped when there is no member in the local
 *              hemisphere vlan Errata (FE-7263524)
 *
 * Fields:
 *       portMaxBuffLimit - maximal number of buffers for a port
 *       portMaxDescrLimit- maximal number of descriptors for a port
 *       tailDropProfileParamsArray - Drop Profile Parameters to associate with
 *                      the Traffic Class in this Profile.
 *
 */
typedef struct{
    GT_U32      portMaxBuffLimit;
    GT_U32      portMaxDescrLimit;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS tailDropProfileParamsArray[CPSS_TC_RANGE_CNS];

}PRV_CPSS_DXCH_PROFILE_INFO_STC;

/**
 * Typedef: enum PRV_CPSS_DXCH_ERRATA_ENT
 *
 * Description: enumerator for the numbers of Erratum that handles in the CPSS
 *              code
 *
 * Enumerations:
 *  PRV_CPSS_DXCH_FDB_AGING_WITH_REMOVAL_MODE_WA_E - In FDB Aging with removal
 *              mode, AA messages are sent for entries removed due to an
 *              invalid device.
 *              The CPU should ignore those messages, after ensuring that the
 *              DeviceId is invalid
 *              (FEr#22)
 *  PRV_CPSS_DXCH_TRUNK_CONFIG_FOR_TRUNK_NEXT_HOP_WA_E - When the Next Hop is a
 *              trunk, the router sets the TrunkHash in the descriptor to
 *              PktHash[2:0]. This means that the value of the TrunkHash may be
 *              set to any value: 0 through 7.
 *              If the Next Hop is a trunk with less than 8 members,
 *              the device may fetch an invalid trunk member.
 *              (FEr#137)
 *  PRV_CPSS_DXCH_INCORRECT_SRCDEV_FIELD_IN_MAILBOX_TO_CPU_DSA_TAG_WA_E -
 *              by Copy The mailbox mechanism enables implementation of a
 *              topology discovery algorithm in a stacking system.
 *              A FROM_CPU DSA-tagged packet with a mailbox indication is
 *              reformatted as a TO_CPU DSA-tagged packet and is forwarded
 *              to the host CPU.
 *              Unlike other TO_CPU DSA-tagged packets, in which the SrcPort and
 *              SrcDev fields preserve their value as it was originally set in
 *              the DSA tag, in a mailbox TO_CPU DSA tag, the SrcPort is set to
 *              the local device Source port number (at which the FROM_CPU DSA-
 *              tagged packet with a mailbox indication was received) and the
 *              SrcDev field is set to the local device number. However, due to
 *              this erratum, the SrcDev field is not set to the local device
 *              number.
 *              (FEr#2776)
 *  PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E  -
 *              Disabling the Egress Rate Shaper under traffic may hang its
 *              relevant transmit queue.
 *              To disable the Egress Rate Shaper follow the following procedure:
 *              1. Set MaxBucketSize to 0xFFF, Tokens to 0xFFF, SlowRateEn to 0.
 *              2. Set TokenBucketEn to 0.
 *              (FEr#47)
 *  PRV_CPSS_DXCH_JUMBO_FRAMES_MODE_SDWRR_WEIGHT_LIMITATION_WA_E  -
 *              The transmit queue Shaped Deficit Weighted Round Robin (SDWRR)
 *              weight may be configured from 0 through 255,
 *              corresponding to a weight of 1 through 256 respectively.
 *              However, due to this erratum, in Jumbo frames mode the maximal
 *              weight that may be configured is 254.
 *              (FEr#29)
 *  PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E -
 *              All packets forwarded to the CPU are forwarded with a
 *              TO_CPU DSA tag. One of the fields in this tag is <PktOrigBC>.
 *              However, when a packet forwarded to the CPU is from another
 *              device, it gets the packet's byte count as it enters the second
 *              device and not the original byte count of the packet that was
 *              received on the first device. The <PktOrigBC> will be either
 *              four or eight bytes larger than the received packet's byte count,
 *              according to the DSA tag that was added.
 *              (FEr#89)
 *  PRV_CPSS_DXCH_RATE_LIMIT_GIG_COUNTER_BITS_NUM_WA_E -
 *              When the value of the 1000MWindow field, in the Ingress Rate
 *              limit Configuration Register0 is higher than 31, the rate limit
 *              does not work properly.
 *              The byte/packets counter of the rate limit (in ports that were
 *              rate limit enabled) never return to zero, causing the bridge to
 *              drop all traffic from these ports.
 *              (FEr#140)
 *  PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E -
 *              Counters 24 (oversize) and 25 (jabber) in XG MIBs, which are
 *              placed in one entry, are treated as one long 64-bit counter.
 *              Therefore, when any one of them is read, counter 24 will always
 *              be accessed. Any additional read, even for another counter,
 *              will then access counter 25.
 *              (FEr#85)
 *  PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E -
 *              in some cases when performing scanning operations and the AUQ is
 *              full the learning operation is also stalled.
 *              The reason is:
 *              the learning and scanning machines share an access arbiter.
 *              In order to insure data coherency in the FDB only a one of
 *              the clients is allowed to access the FDB at any
 *              given time. An access being with a read and ends
 *              with a write and only then is the arbitration
 *              changed. In the scenario you saw, since the
 *              scanning machine is waiting for the AUQ to
 *              become available, it does not release the access
 *              arbiter. The learning machine in this case is
 *              stalled as well and is waiting for the scanning
 *              machine to release the access arbiter.
 *              (FEr#3119)
 *  PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E -
 *              To assign non-DSA tagged packets with a source-ID according to
 *              the port default configuration (ignoring the source FDB entry
 *              Source-ID assignment), set the corresponding port bit in the
 *              field in the Source-ID Assignment Mode Configuration register.
 *              However, due to this erratum, when reading from this register,
 *              wrong data is returned.
 *              (FEr#1084)
 *   PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E  -
 *              Bridge Access Matrix Line <7> register is write only.
 *              Shadow of the register should be used by SW.
 *              (FEr#1006)
 *   PRV_CPSS_DXCH2_POLICY_ACTION_DIRECT_BURST_ONLY_READ_WA_E -
 *              Direct access and indirect access to tables is supported
 *              by the device. However, due to this erratum, direct access
 *              to the Policy Action entry table,
 *              (address space: 0x0B8C0000 - 0x0B8C3FFC) is not supported
 *              for single access. (burst only). (FEr#1090)
 *
 *   PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E -
 *              Direct access to policer engine address space is not functional
 *              WA : The policer engine registers and tables, address space,
 *              0x0C000000 - 0x0C800000, must be accessed by using the indirect
 *              access mechanism, as specified in the device's Functional
 *              Specifications Doc. No. MV-S103086-00.
 *              (FEr#1001)
 *   PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E -
 *              Router ARP and Tunnel Start table do not support burst access via PCI.
 *              The device supports burst reads from its internal memory address
 *              space. However, due to this erratum, the Router Address
 *              Resolution Protocol (ARP) and Tunnel Start (TS) table,
 *              0x07E80000 - 0x07E87FF4, cannot be accessed by using the burst
 *              access feature.
 *              WA : Access to the Router ARP and TS table must done by single
 *              32-bit read/write access.
 *              (FEr#1003)
 *   PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E -
 *              RdWrTrig, bit[15] in the VLT Tables Access Control register is ignored.
 *              When the RdWrTrig field, bit[15] in the VLT Tables Access
 *              Control register (offset: 0x0A000010) is set to 1, a read or
 *              write command is performed. After the action is completed, this
 *              bit is cleared. However, due to this erratum, a read or write
 *              command is performed for every write access to the register,
 *              regardless of the value of the RdWrTrig field.
 *              WA : No dummy write commands should be done to the VLT Tables
 *              Access Control register (offset: 0x0A000010).
 *              (FEr#1007)
 *   PRV_CPSS_DXCH2_INDIRECT_ACCESS_ROUTER_AND_TT_TCAM_NOT_FUNCTION_CORRECTLY_WA_E -
 *              Indirect access for Router and Tunnel Termination (TT) TCAM does
 *              not function correctly.
 *              The device supports indirect access to the Router and Tunnel
 *              Termination (TT) table. Valid bits should be written to the
 *              arrays according to their location: Valid[0] to Array0, Valid[1]
 *              to Array1, etc. However, due to this erratum, this does not
 *              function correctly. So when writing to one array, set all bits
 *              to 1 to validate the entry, or all bits to 0 to invalidate the
 *              entry. Do the same for writing to all arrays in parallel.
 *              WA: When using the indirect access mechanism to access the Router
 *              and Tunnel termination (TT) table bits[4:0], the Valid field,
 *              bits[4:0] in the Router and TT TCAM Access Data Control register
 *              (offset: 0x02800418), must be set to the same value, either 0x0
 *              to invalidate or 0x1F to validate.
 *              An additional option is to use the direct access mechanism to
 *              access the Router and Tunnel termination (TT) table.
 *              (FEr#1062)
 *   PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E -
 *              XG PHY path failure.Wrong value of PHY register 0xFF34.
 *              WA: The following register write sequence should be performed
 *              after power-up, reset or after XG PHY power-up: FF00, FF00, and
 *              E000 to the PHY register 0xFF34.
 *              (RM#83)
 *   PRV_CPSS_DXCH2_RM_RESERVED_REGISTERS_0x0B820108_WA_E -
 *              Reserved registers 0x0B820108.Wrong value of reserved register
 *              0x0B820108.
 *              WA: Set the value 0xB80 to register 0x0B820108.
 *              (RM#1081)
 *  PRV_CPSS_DXCH_RM_FDB_TABLE_ENTRIES_WA_E -
 *              Some FDB table entries must be invalidated (RM#28)
 *              The FDB table has 16K entries. But half of FDB need
 *              to be invalidated by an application.
 *  PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E -
 *              The position of Vlan modification and of Match Counter
 *              fields is different from described (and from Egress PCL action)
 *              (FEr#1087)
 *  PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E :
 *              The following packet layer 4 sanity checks are enabled by default:
 *               ARP SA Mismatch Drop Enable - Enables dropping or ARP packets
 *                                             with MAC SA not equal to the ARP and payload
 *                                             data containing a field called <source hardware address>.
 *               TCP SYN With Data Drop Enable - Enable dropping of TCP SYN packets with data.
 *               TCP Over MC Drop Enable - Enables dropping of TCP packets over Multicast/Broadcast MAC DA.
 *               Fragmented ICPMDropEn - Enables dropping of fragmented ICMP packets.
 *               TCPFlagesZeroDropEn - Enables dropping of TCP packets with all flags set to zero.
 *               FINWithURGWithPSH DropEn - Enables dropping of TCP packets with FIN, URG, and PSH flags set.
 *               SYNWithFINDropEn - Enables dropping of TCP packets with both SYN and FIN flags set.
 *               TCPSYNWithRSTDropEn - Enables dropping of TCP packets with both SYN and RST flags set.
 *               TCPUDPZeroPortDropEn - Enables dropping of TCP or UPD packets with source or
 *                                       destination port equal
 *              (RM#3018)
 *  PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E -
 *              VLT Tables Indirect Access is not functional.
 *              (FEr#2006)
 *  PRV_CPSS_DXCH3_VLT_TABLES_WRITE_DIRECT_ACCESS_WA_E -
 *              Direct Access write does not work under traffic for VLT memory
 *              (VLAN table, Multicast Groups table, Span State Groups table,
 *               VRF Table).
 *              Direct write to VLT memory tables work in the following way:
 *              1. for each write/read access the entire table entry is
 *              written/read by device from the memory and placed in a special
 *              temp register.
 *              2. When new access operation is starting, the temp register
 *              still contains data from the previous operation.
 *              3. Write operation is done word-by-word through updating words
 *              of the temp register, but the entire register is written to the
 *              table as many times as there are words in table entry.
 *              Assume table entry length is 6 words, first the word 0 of the
 *              temp register is updated with new data and the entire register
 *              (6 words) is written to the table, then word 1 is updated,
 *              and again the entire 6 words are written, now word 0 and word 1
 *              are correct, words 2-5 contain junk from previous operation,
 *              but still are written to the table.  After 6 such operations
 *              the table entry content is correct, but during these operations
 *              there was a temporary inconsistency of table content.
 *              If done under the traffic, this may lead to unpredictable
 *              behavior of the system.
 *
 *              Among several VLT tables, two have multi word entries -
 *              VLAN and STG. In order to avoid the problem described above,
 *              CPSS reads the content of updated entry, this way setting the
 *              temp register with consistent content, and then performing write
 *              to the table operation.
 *
 *  PRV_CPSS_DXCH3_SAMPLE_AT_RESET_NET_REF_CLOCK_SELECT_WA_E -
 *              Sampled at Reset Network reference clock select is not functional
 *              (FEr#2014)
 *  PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E -
 *              Buffer Management GigE Ports Group Limits Configuration Register
 *              (Offset: 0x03000004) and
 *              Buffer Management HyperG.Stack Ports Group Limits Configuration
 *              Register (Offset: 0x03000008)
 *              default values are incorrect
 *              (RM#2003)
 *  PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E -
 *              LMS GigE Ports Mode Register1 (Offset: 0x05804144)
 *              default values are incorrect.
 *              (RM#2007)
 *  PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E -
 *              LMS HyperG.Stack Ports Mode Register (Offset: 0x84804144)
 *              default value is incorrect.
 *              (RM#2008)
 *  PRV_CPSS_DXCH3_RM_TCAM_PLL_REG_WA_E -
 *              The TCAM PLL Register (Offset: 0x000000BC) default values
 *              are incorrect.
 *              (RM#2011)
 *               FEr#2012: TCAM/CORE/MPPM clock frequencies above 250 MHz
 *               are not operational
 *  PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E -
 *              LMS0/1 Group1LED Interface0 Control Register 1
 *              and Class6 Manipulation Register default values are incorrect.
 *              (RM#2024)
 *  PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E -
 *              MAC2ME Registers Low default values are incorrect.
 *              (RM#2026)
 *  PRV_CPSS_DXCH3_RM_AN_CNF_REG_WA_E -
 *              PHY Auto-Negotiation Configuration Register0
 *              and PHY Auto-Negotiation Configuration Register2
 *              default values are incorrect.
 *              (RM#2061)
 *  PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E -
 *              The direct access to Pre-Egress Engine (EQ) tables for both
 *              reads and writes, under traffic is not functional. Pre-Egress
 *              Engine (EQ) tables include; CPU Code Table, Ingress STC Table,
 *              QoSProfile to QoS Table, Statistical Rate Limits Table, TO CPU
 *              Packet Rate Limiters, and Trunks Members Table.
 *              WA: The Pre-Egress Engine (EQ) tables address space must be
 *              accessed for read or for write by using indirect access only.
 *              (FEr#2028)
 *  PRV_CPSS_DXCH3_TXQ_PARITY_CALCULATION_E -
 *              TxQ Parity calculation is wrong for Descriptors & FLL memories.
 *              The TxQ calculates parity on the FLL RAM & Desciptor RAM,
 *              but calculates it on a different number of bits used when
 *              writing the data to the RAMs.
 *              This causes parity error on almost every packet reaching the TxQ.
 *              WA: interrupts ParityErrorIn TxqPtrMem FllRamParity and
 *              ParityErrorIn TxqDescMem in the Transmit Queue General Interrupt
 *              Cause register must be masked
 *              (FEr#204)
 *  PRV_CPSS_DXCH3_SDMA_WA_E -
 *              SDMA resource errors may cause PCI Express packets reception
 *              malfunction.
 *              When using the PCI Express host management interface
 *              for forwarding packets to/from the host CPU, the
 *              CPU receives packets from the device directly to
 *              a pre-allocated space in the host memory. To
 *              receive packets for a given CPU traffic class
 *              queue, the CPU prepares a linked list of
 *              descriptors located in the host memory. A
 *              resource error event occurs when it reaches the
 *              end of available descriptors in the linked list.
 *              This resource error occurs if either: The
 *              current descriptor for the Rx SDMA has a NULL
 *              next-descriptor pointer. OR The SDMA has reached
 *              a descriptor that is owned by the CPU (i.e., a
 *              cyclic descriptor list is implemented). This may
 *              happen at the start of a packet or in the middle
 *              of writing a packet to host memory (if it
 *              occupies more than a single descriptor buffer).
 *              This may also happen due to a speculative
 *              pre-fetch of a descriptor (i.e, without any
 *              current packet destined to the given queue). The
 *              device terminates packet writing to the host
 *              memory if it reaches the end of the linked list
 *              before completion of packet reception.Functional
 *              Impact SDMA resource errors may cause a PCI
 *              Express packets reception malfunction. WA:
 *              Configure the SDMA to drop packets upon resource
 *              errors, by setting to 1 the Retry AbortTC<
 *              traffic class queue> fields in the SDMA
 *              Configuration Register
 *              (FEr#2009)
 *   PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E -
 *              When the CPU reads the POLICY TCAM or ROUTER TCAM
 *              via direct address space the ECC error exception signaled.
 *              WA is to mask the interrupt for the CPU read time and to restore
 *              (unmask) it for all other time.
 *              (FEr#2033)
 *
 *              ECC check fails on reading part of PCL/Router TCAM row
 *              The PCL/Router TCAM is used by the PCL/Router engine for IPv4/6
 *              Unicast/Multicast lookups, and is also used by the tunnel
 *              termination/interface lookup.
 *              The PCL/Router TCAM is organized as rows, where each row contains
 *              four arrays of data; Array0, Array1, Array2, and Array3.
 *              PCL/Router TCAM on-chip table memory has built-in data integrity
 *              by Error Correction Code (ECC).
 *              However, due to this erratum, data returns on indirect read to
 *              part of a row (not all 4 arrays) or direct read containing
 *              incorrect ECC cause the ECC check to fail.
 *              WA : To indirectly read PCL/Router TCAM data part of a row
 *              (not all 4 arrays), or to read directly, perform the following:
 *              1. Mask the ECC interrupt.
 *              2. Read PCL/Router TCAM data.
 *              3. Read the Cause register and clear the interrupt.
 *              4. Unmask the ECC interrupt.
 *              (FEr#2050)
 *
 *           -- NOTE: must fill info in
 *              PRV_CPSS_DXCH_ERRATA_STC::info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E
 *              regarding the interrupt register and index , that a device
 *              family that needs this WA should set according to it's specific
 *              values.
 *
 *  PRV_CPSS_DXCH3_OUT_OF_PROFILE_QOS_ATTR_E -
 *              If the QoS Remarking Control En Modify DSCP or the En Modify UP
 *              are set in the Metering table entry for a Yellow or
 *              Red non-conforming packet, these QoS attributes will be changed
 *              regardless of the non-conforming Yellow or Red packet command.
 *              WA: Enable modification of UP/DSCP for non-conforming packets
 *              when <Policer Cmd> = 2 (QoS Profile Mark by table)
 *              (FEr#2046)
 * PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E -
 *              An Interrupt Cause register and an Interrupt Mask register are
 *              defined for each functional block. However, due to this erratum,
 *              - A write to the Transmit Queue Flush Interrupt Mask register
 *                (offset: 0x01800114), also causes a write to the Transmit
 *                Queue Full Interrupt Mask register (offset: 0x018001EC).
 *              - A write to the Transmit Queue Full Interrupt Mask register
 *                (offset: 0x018001EC) does not function.
 *              WA: disable get of TXQ Full interrupts in the Transmit Queue
 *              Interrupt Summary Mask register - 0x01800104
 *              (FEr#1098)
 * PRV_CPSS_DXCH3_RM_GE_SERDES_MISC_CONF_REG_WA_E -
 *              Network Port SERDES Misc Conf Register default values are incorrect.
 *              WA: Use the EEPROM or CPU to change the default value of Network
 *              Port SERDES Misc Conf Register: 0x0000000D at addresses
 *              0x09800014 + n*0x400: where n (0-23) represents Port.
 *              (RM#2062)
 * PRV_CPSS_DXCH3_RM_BM_TX_FIFO_THRESHOLD_CONF_REG_WA_E -
 *              Buffer Memory Tx Fifo Threshold and MPPM Access Arbiter
 *              Configuration Register default value of GigTxFifoThreshold is incorrect.
 *              WA: Use the EEPROM or CPU to change the default value of
 *              Buffer Memory Tx Fifo Threshold and MPPM Access Arbiter
 *              Configuration Register:0x444e610c at address 0x0780006C.
 *              (RM#2063)
 *              used only on ch3 XG devices
 * PRV_CPSS_DXCH3_TCAM_EFUSE_NOT_TRIGGERED_AUTO_WA_E -
 *              The device incorporates Policy TCAM and Router TCAM on-chip memories that
 *              contain an eFUSE mechanism. When the device is reset, the eFUSE mechanism
 *              replacement operation is not automatically activated.
 *              (FEr#2191)
 * PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E -
 *              Policy and Router TCAM Tables indirect read is not functional.
 *              Direct access and indirect access to tables are supported by the device.
 *              However, due to this erratum, the indirect read from the
 *              Policy and Router TCAM tables address space is not supported.
 *              Functional Impact :
 *              The indirect read from the Policy and Router TCAM tables address
 *              space is not functional.
 *              WA : The Policy and Router TCAM tables address space must be
 *              accessed for read by using the direct access only.
 *              (FEr#2004)
 *  PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E -
 *               Wrong default in FDB Global Configuration register
 *              (RM# 2704)
 *
 * PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E -
 *              for CH3 :
 *              The device supports IP Longest Prefix Match based on the Policy TCAM
 *              and policy-based routing based on the Router TCAM.
 *              However, due to this erratum, the ingress policy action can assign a
 *              router Lookup Translation Table (LTT) pointer only in the range of first column.
 *              TCAM based routing is operational at full range.
 *              Functional Impact :
 *              The ingress policy action can assign a  router Lookup Translation Table (LTT)
 *              pointer only in the range of first column.
 *              WA : None.
 *              (FEr#2018)
 *              for XCAT :
 *              Policy base routing LTT index can be written only to array0 of the LTT line.
 *              The Lookup Translation Table (LTT) is build from 4 entries in a line.
 *              When using the LTT for policy based routing, the index that received from the
 *              PCL/TTI used by the LTT as a line number, when the array number if always 0.
 *              Functional Impact :
 *              Thus, this feature uses only 25% of the capability of the LTT table.
 *              WA : When the application enter LTT index X in the PCL/TTI action, the actual
 *              value that will be written is divided by 4.
 *
 *  PRV_CPSS_DXCH3_TCAM_REPAIR_WA_E - PCL and Router TCAM Software Repair (TB#233)
 *
 *  PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E -
 *              Under heavy traffic stress, FDB accesses from the CPU can get stuck.
 *              Set the default value of bit[6] in the FDB Global Configuration
 *              Register (offset: 0x06000000) to 0x1 (giving strict priority to
 *              management accesses over the data-path).
 *              (RM#3014)
 *
 *  PRV_CPSS_DXCH3_SERDES_YIELD_IMPROVEMENT_WA_E -
 *              SERDES yield improvement due to PLL unlocking issue.
 *
 *  PRV_CPSS_DXCH3_TOGGLE_DEV_EN_UNDER_TRAFFIC_WA_E -
 *              Toggling Device Enable under traffic may cause system hang.
 *              (FEr#2261)
 *
 * PRV_CPSS_DXCH_XCAT_RM_TOTAL_BUFFER_LIMIT_CONF_IN_TXQ_E -
 *              Wrong default value in Total Buffer Limit Configuration in TxQ.
 *              (RM#3008)
 *
 * PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E -
 *              Wrong default values in Counting Mode Configuration of IPCL and
 *              in TTI Unit Global Configuration of TTI.
 *              The "Enable Counting" should be set by default.
 *              (part of RM#3012)
 *
 * PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E -
 *              Wrong default value of bit#28 of Transmit Queue Extended Control
 *              register. The bit should be set by default.
 *              (RM#3009)
 *
 * PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E -
 *              Wrong default values in Mirror Interface Parameter Register<%i> -
 *              bit #20 should be with value 1
 *              (part of RM#3012)
 *
 * PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E -
 *              Wrong default values in Policy Engine Configuration -
 *              bit #12 should be with value 1
 *              (part of RM#3012)
 *
 * PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E -
 *              Wrong default values in Dequeue SWRR Weight Register 3 -
 *              bit #0..8 : set to value 0x4b ,
 *                  because fields of XG and Gig1 weights are swapped
 *              (RM#3007)
 *
 * PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E -
 *              Wrong default values in IPLR0,1 --> Policer Control0 -
 *              bit #21 : set to 1
 *              (part of RM#3012)
 *
 * PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E -
 *              Wrong default values in IPLR0,1 --> Policer Control1 -
 *              bit #0 : set to 1
 *              (part of RM#3012)
 *
 * PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E -
 *              Wrong default values in IPLR0,1,EPLR --> Policer Control2 -
 *              bit #0 : set to 1
 *              (part of RM#3012)
 *
 *  PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E
 *              Wrong default values in Egress Policer Global Configuration -
 *              bit #0 : set to 1
 *              (part of RM#3012)
 *
 *  PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E -
 *              Wrong default values in BCN control -
 *              bit #1 : set to 0
 *              bit #3 : set to 1
 *              (part of RM#3012)
 *
 *  PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E -
 *              Wrong default values in Ports BCN Awareness Table -
 *              bits #0..27 : set to 0x0FFFFFFF
 *              (part of RM#3012)
 *
 * PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E -
 *              Wrong default values in TTI Unit Global Configuration -
 *              bit #3 : set to 1
 *              bit #19 : set to 0
 *              (part of RM#3012)
 *
 *  PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E -
 *              bit #8 in the 'per port' 'PORT VLAN QOS configuration'
 *                      ('Extended DSA Bypass Bridg') : set to 1
 *              (RM#3005)
 *
 *  PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E -
 *              Wrong default values in Port Serial Parameters Configuration
 *              bit #1 : set to 0
 *              (RM#3004)
 *
 *  PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E -
 *              Wrong default values in LMS0 Misc Configurations , LMS1 Misc
 *              Configurations.
 *              bit #15..16 : set to 2
 *              bit #20 : set to 0
 *              (RM#3002,RM#3003)
 *
 *  PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E -
 *              Traffic from the CPU via SDMA mechanism can corrupt the switch
 *              internal buffer memory.
 *              Solution: Traffic sent from the CPU is padded with 8 bytes of
 *              zeros for byte counts of <byteblock>*n+k (1<=k<=8) (including
 *              DSA tag), where n and k are integers and <byteblock> is 256 for
 *              xCat and 512 for Lion,
 *              (FEr#3048)
 *
 * PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E -
 *              when PP sending a packet to CPU from port that it is a member of
 *              trunk, the <source port> field should hold the source port (not
 *              the trunkId) , in DSA Marvell TAG header.
 *              but due to the erratum this field includes the <trunk ID> / 2.
 *              (FEr#3033)
 *
 * PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E -
 *              Reading of IEEE Reserved Multicast Configuration registers
 *              should be performed from:
 *              0x200X820 instead of 0x200X810 (table 2)
 *              0x200X828 instead of 0x200X818 (table 3)
 *              (FEr#3060)
 *
 * PRV_CPSS_DXCH_XCAT_RM_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E -
 *              Enable CPU lacks source port information on traffic
 *              received from trunk workaround:
 *              Set bit #17 in register 0x0C0002B0
 *              (RM#3017)
 *
 * PRV_CPSS_DXCH_XCAT_GIGA_PORT_PRBS_PARAM_INIT_WA_E -
 *              FEr#3054: Gig port PRBS is not functional.
 *              Initialization PRBS parameters of Giga MAC for ports 24-27.
 *              Bits 15:14 of registers
 *              0x0A806014, 0x0A806414, 0x0A806814, 0x0A806C14
 *              set to 0x03
 *
 * PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E -
 *              Enable CPU checking addresses and filtering the restricted ones.
 *              lastValidAddress - last valid address
 *              (FEr#2739)
 *
 * PRV_CPSS_DXCH_XCAT_RM_HEADER_ALTERATION_GLOBAL_CONFIGURATION_REGISTER_WA_E -
 *              Wrong default values in Header Alteration Configuration -
 *              bit #10 should be with value 0
 *              (RM#3024)
 *
 * PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E -
 *              TOD counter must be updated in resolution of 20ns.
 *              The device maintains a free-running Time Of Day (TOD) counter
 *              for IP Flow Information Export (IPFIX) and Precision Time
 *              Protocol (PTP) time-stamping. The TOD counters are incremented
 *              20 ns on every rise and fall of the clock.
 *              (FEr#2773)
 *
 * PRV_CPSS_DXCH_XCAT_ETH_OVER_IPV4_GRE_WA_E -
 *              Enable HA fix - tunneling support for Eth-over-IPv4.
 *
 * PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E
 *              The passenger Ethernet packet may optionally retain its original CRC end-to-end,
 *              or its CRC may be removed prior to being transported across the tunnel and
 *              regenerated prior to sending the packet to the remote customer attachment circuit.
 *              However, due to this erratum, the original passenger packet CRC is removed and not
 *              retained even if set to "Retain CRC".
 *              (FEr#2785)
 *
 * PRV_CPSS_DXCH_XCAT_RM_MAC_CTRL_REG3_PCS_PREAMBLE_WA_E -
 *              Enable Short Preamble fix
 *
 * PRV_CPSS_DXCH_XCAT_RM_METAL_FIX_SGMII25G_DPCLK_SEL_WA_E -
 *              Enable Split Network and Stack ports configuration (DP clock select) fix
 *
 * PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_CCFC_SRC_PORT_WA_E -
 *              Enable CCFC frames sent to internal cascade port fix
 *
 * PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E -
 *              in GE port interface mode the disable bit does not affect the transmit path.
 *              (FEr#2794)
 *
 * PRV_CPSS_DXCH_XCAT_FC_FORWARD_NOT_FUNCTIONAL_WA_E -
 *              Ethernet packets with MAC DA 01-80-C2-00-00-01 are terminated in ingress.
 *              (FEr#3102)
 *
 * PRV_CPSS_DXCH_XCAT_RM_SSMII_PAD_ZNR_ZPR_WA_E
 *              in the Pad_Control_1 register(offset: 0x48), configure the following fields:
 *              ssmii_pad_znr: bits[19:17] to 0x3.
 *              ssmii_pad_zpr: bits[16:14] to 0x3.
 *              (RM#3042)
 *
 * PRV_CPSS_DXCH_XCAT_RM_PASSENGER_TPID_REG_WA_E -
 *               The registers HA/Passenger TPID0_TPID1<%n> have default value 0x0 instead of 0x81008100.
 *              (RM#3043)
 *
 *  * PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E -
 *              The default value of the <SelPortSDMA> field in the Global
 *              Control register (offset 0x58 in each pipe) is 0x0.
 *              This value selects the CPU port rather than the SDMA as the
 *              interface for packet to/from CPU. To activate the SDMA as the
 *              interface to/from CPU, the first required modification prior to
 *              any access to SDMA registers is to set this bit to 0x1.
 *              If an access to any of the SDMA registers is performed prior to
 *              setting the <SelPortSDMA> field, then the PCI Express interface
 *              will hang.
 *              WA : Per management control interface, write to the following
 *              registers: Change bit 20 of 0x58 to 0x1
 *              (RM#2701)
 *
 *  PRV_CPSS_DXCH_LION_INDIRECT_ACCESS_TO_TCAM_IS_NOT_SUPPORTED_WA_E -
 *              Indirect access to TCAM (Router and PCL) is not supported
 *              (FEr#2744 --> include FEr#2004 as part of it)
 *
 *  PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E -
 *              FDB messages of : AA,TA,QR,FU related to entry of 'port' of
 *              'own device' , may be received in CPU from all port groups ,
 *              and not only from the port group of the 'port'.
 *              (FEr#2769)
 *
 *  PRV_CPSS_DXCH_LION_FAST_FAILOVER_WA_E -
 *              In fast failover the secondary port must be in the same core as the primary
 *              port.
 *              (FEr#2756)
 *
 *  PRV_CPSS_DXCH_LION_RM_SYNC_ETHERNET_NONE_SELECT_WA_E -
 *              Device default is that all SERDES are selected as sources for
 *              Synchronous Ethernet recovery clock while by default none
 *              should be.
 *              (RM#2705)
 *
 *  PRV_CPSS_DXCH_LION_RM_TCAM_CONFIG_WA_E -
 *              Fix value of TCAM after read delay
 *      The default values of the following registers are incorrect:
 *      Policy TCAM Configuration 0 Register Offset: 0x0D000208 Lion
 *      Policy TCAM Control Register Offset: 0x0D000148 Lion
 *      Policy TCAM Configuration 2 Register Offset: 0x0D000220 Lion
 *      Router TCAM Configuration 0 Register Offset: 0x0D80096C Lion
 *      Router TCAM Control Register Offset: 0x0D800960 Lion
 *      Router TCAM Configuration 2 Register Offset: 0x0D800978 Lion
 *              (RM#3013)
 *
 *  PRV_CPSS_DXCH_LION_URPF_PER_VLAN_NOT_SUPPORTED_WA_E -
 *              uRPF per VLAN is not supported
 *              (FEr#2732)
 *
 *  PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E -
 *              Token Buckets are not supported
 *
 *  PRV_CPSS_DXCH_LION_PREVENT_REDUNDANT_AU_FU_MESSAGES_NOT_SUPPORTED_WA_E -
 *              The Lion has new feature that should prevent
 *              redundant AU/FU messages to CPU (done by enabling mask AU/FU
 *              message to CPU on non local core), but the feature does not
 *              works.
 *              (FEr#2769)
 *
 * PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E -
 *              In Lion the direct access to Policer Counting memory is not
 *              working. The indirect access should be used in order to update
 *              the entry and read it.
 *              (FEr#2771)
 *
 *  PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_FDB_LIMIT_WA_E -
 *              The multi-port group architecture can be utilized to extend the
 *              Bridge FDB lookup across the device port groups.
 *              However, when the multi-port group FDB lookup is used, the device
 *              default settings of Auto-learning enabled, or Age mode set to
 *              Age-with-delete, can cause mis-synchronization of the different
 *              port groups FDB tables.
 *              (RM#2702)
 *
 *  PRV_CPSS_DXCH_LION_RM_ORIG_SRC_PORT_FILTERING_WA_E -
 *              By default, the <OrigSrcPortFilterEn> bit[3] in the Egress
 *              Filters Global Enables register (offset: 0x01810000) is 0x0 (disable).
 *              This value does not prevent filtering multi-destination packets
 *              from being sent back to the network port where it was initially
 *              received.
 *              When the multi-port group architecture is utilized to extend the
 *              Bridge FDB lookup across the device port groups, set the <OrigSrcPortFilterEn>
 *              to 0x1 (enable)
 *              (RM#2706)
 *
 *  PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_CPU_IN_VIDX_WA_E -
 *              Ports 15, 31, and 47 cannot be members of the Multicast group 0xFFF.
 *              A packet forwarded to a Multicast port group is assigned an index
 *              called "VIDX". At initialization, the VIDX Multicast table is
 *              cleared to all zeros, with the exception of an entry for VIDX
 *              0xFFF, which is initialized to have all ports as members.
 *              This enables Multicast packets to be flooded to all the VLAN ports.
 *              However, global ports 15, 31, and 47 cannot be members of the
 *              Multicast group 0xFFF.
 *              Exclude ports 15, 31, and 47 from the Multicast group 0xFFF membership
 *              in the Multicast Groups table, (offset: 0x1186FFF0).
 *              (RM#2707)
 *
 *  PRV_CPSS_DXCH_LION_CN_GLOBAL_REG_WA_E -
 *              The CnDropEn field, bit[5] in the CN Global Configuration register,
 *              (offset: 0x0A0B0000) enables dropping CN packets
 *              when any of the following clients is not ready:
 *                - Inc interface
 *                - Desc dup interface
 *                - Writes back FIFO full
 *              Set the CnDropEn field, bit[5] in the CN Global Configuration
 *              register, (offset: 0x0A0B0000) to 0x1 (enable).
 *              (FEr#2768)
 *
 *  PRV_CPSS_DXCH_LION_TEMPERATURE_SENSOR_INITIALIZATION_WA_E -
 *              need to Calibrate sensor on init in every power up
 *              (all below registers are in PortGroup0):
 *              1) Change Temperature sensor average num from 2 to 8 -
 *                  Register 0x118F0D88 Bits[21:19] = 0x3
 *              2) Change Temperature sensor RefCalCount from 0x113 to 0xf1 -
 *                  Register 0x118F0D8C Bits[17:9] = 0xF1
 *              3) Start init -
 *                  Register 0x118F0D8C Bit[31] = 0x0
 *              4) Start init -
 *                  Register 0x118F0D8C Bit[31] = 0x1
 *
 *  PRV_CPSS_DXCH_LION_RM_MCFIFO_DISTRIBUTION_REG_WA_E -
 *              need to configure all ports in the MCFIFO distribution mask register
 *              (0x01801010+ 0x4*n) as follow:
 *              1) configure ports 0-63
 *                  a) EVEN Ports - FIFO 0
 *                  b) ODD Ports - FIFO 1
 *              2) All ports should be configured in all PortGroups
 *
 * PRV_CPSS_DXCH_LION_RM_SD_PLL_REG_WA_E -
 *             XLG port frequency configuration
 *             The device XLG port frequency is driven from an internal PLL.
 *             The PLL frequency is locked according to a sampled at reset value
 *             from external pins. However, due to this erratum, the available
 *             values of the external pins are not correct and a software
 *             configuration is needed.
 *             (HWE#2706)
 *
 *  PRV_CPSS_DXCH_LION_SLOW_RATE_ON_PORT_TB_NOT_SUPPORTED_WA_E -
 *              Slow rate is not functional on port TB
 *             (FEr#2780)
 *
 *  PRV_CPSS_DXCH_LION_RM_UC_STRICT_OVER_MC_WA_E -
 *      With default configurations, the UC are not served at WS in presence of
 *      MC, therfore give UC Strict Priority over MC.
 *      Set bit 2 in the MC FIFO Global Configurations Register (0x01801000).
 *      (RM#2710)
 *
 *  PRV_CPSS_DXCH_LION_RM_SCHEDULER_DEFICIT_MODE_WA_E -
 *      The TxDMA isn't always ready to receive descriptor from the TxQ when
 *      instantaneous burstiness of data occurs.
 *      The workaround consists of 2 steps:
 *      1. Enabling the Deficit mode (handles higher burstiness of data on the
 *         ports).
 *      2. Set <Token Bucket Base Line> value to at least eight times the MTU.
 *      (RM#2709)
 *
 *  PRV_CPSS_DXCH_LION_RM_TX_MC_IN_CHUNKS_WA_E -
 *      WS is not achieved in "one to many" traffic at 316 Bytes and below.
 *      Set the number of MC descriptors duplications done per UC descriptor
 *      to 4:1.
 *      Clear bits 1:0 in the MC FIFO Global Configurations Register (0x01801000).
 *      (RM#2711)
 *
 *  PRV_CPSS_DXCH_LION_RM_BUFF_LIMIT_EGRESS_QUEUE_WA_E -
 *      Wrong total number of buffers limit in egress queues.
 *      Set bits 13:0 in the Global Buffer Limit Register (0x0A0A0310) to 0x1e70.
 *      (RM#2712)
 *
 *  PRV_CPSS_DXCH_LION_RM_QCN_FOR_CN_FORMAT_WA_E -
 *      Since QECN mode is obsolete, CN frame format must be set to QCN.
 *      Set bit 29 in the Extended Global Control 2 Register (0x8C).
 *      (RM#2713)
 *
 *  PRV_CPSS_DXCH_LION_XLG_WS_SUPPORT_WA_E -
 *      XLG port may not support wire speed in all cases.
 *      (FEr#2709)
 *
 *  PRV_CPSS_DXCH_LION_PFC_FOR_MANY_PRIORITY_WA_E -
 *      Only one priority queue can be specified for pausing at the same
 *      time in PFC frame.
 *      (FEr#2787)
 *
 *  PRV_CPSS_DXCH_LION_QCN_FORMAT_COMPLIANT_WA_E -
 *      QCN frame format is partially compliant with IEEE 802.1 Qau.
 *      Set bit 0 in the HA Misc Configuration Register (0xE800710).
 *      (FEr#2703)
 *
 *  PRV_CPSS_DXCH_LION_INACCURATE_PFC_XOFF_WA_E -
 *      The response to the XOFF PFC frame is inaccurate.
 *      Set bit 19 in the TTI Misc Confuguration Register (0x1000200).
 *      (RM#2788)
 *
 *  PRV_CPSS_DXCH_LION_INCOMPLETE_CPU_MAILBOX_INFO_WA_E -
 *      CPU Mailbox to Neighbor CPU device source information is not
 *      complete - wrong source port assigment.
 *      (FEr#2776)
 *
 *  PRV_CPSS_DXCH_LION_IPCL_FALSE_PARITY_ERROR_WA_E -
 *      The Ingress PCL Configuration tables contain the lookup attributes.
 *      The device incorporates a parity check on these tables.
 *      However, due to this erratum, since Ingress PCL Configuration table defaults
 *      are random data, a false parity error indication may occur
 *      during tables initialization
 *      (FEr#2774)
 *
 *  PRV_CPSS_DXCH_LION_PORT_REQUEST_RATE_RM_WA_E -
 *      Once the port request rate to the Link List exceeds its configurable limit,
 *      descriptors may be corrupted and cause the device to hang.
 *      Set PortReqMask<n> (n=0..3) to 0x4 in Port Request Mask register (0x11001004).
 *      (RM#2716)
 *
 * PRV_CPSS_DXCH_LION_WRONG_COUNTING_OF_IP_COUNTERS_PER_PORT_WA_E
 *      The Router engine supports multiple sets of Router management counters.
 *      Any of the four counter-sets can be bound to an interface defined by the 3-tuple:
 *      {device+port or Trunk, VLAN, packet type: IPv4 or IPv6}.
 *      However, due to this erratum, in some scenarios, the per {device+port}
 *      may incorrectly count packets from a wrong port
 *      (FEr#3116)
 *
 * PRV_CPSS_DXCH_LION_RM_PFC_DSA_TAG_E - default DSA tag content
 *       for PFC packets is not correct.(Lion2:RM-1752573, Design BTS#:435(DIP))
 *
 * PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E - due to this
 *          erratum, read access from the Egress STC table under
 *          heavy traffic may cause one of the following issues:
 *              Management access hang
 *              STC mechanism may stop
 *      (FEr#3142)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_STACK_PORT_MIB_CNTR_CONTROL_WA_E -
 *      Stack Port Don't Clear After Read bit in Tri-Speed Ports MAC
 *      and CPU Port MAC Configuration/Stack port MIB Counters Control
 *      register should be set to 0x0 by default for ports 24-27.
 *      (RM#3028)
 *
 * PRV_CPSS_DXCH_XCAT2_EPCL_GLOBAL_EN_NOT_FUNCTIONAL_WA_E -
 *      EPCL global enabling is not functional (Enable/Disable of EPCL
 *      processing is done per egress port, based on packet type).
 *      (FEr#3056)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_POLICER_COUNT_FORMAT_WA_E -
 *      The default value of the <Counting Entry Format Select> field, bit[4]
 *      in Policer Control1 register (EPLR: 0x03800004, IPLR0: 0x0C800004,
 *      IPLR1: 0x0D000004) is incorrect.
 *      Change the value to "Compressed format" (0x1).
 *      (RM#3026)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_INT_EXT_REF_CLK_WA_E -
 *      The default value of bit[30] (<int_ext_ref_clk_125> field) in the
 *      Device_Cfg register (0x28) is incorrect.
 *      Change the value to 0x0 (input_pin).
 *      (RM#3027)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_POLICER_MEM_CONTROL_WA_E -
 *      The default value of Hierarchical Policer memory control should be 0x5:
 *      Policer 0 - 28 entries, Policer 1 - 256 (172+84) entries.
 *      (RM#3029)
 *
 * PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E -
 *      The device implements MAC MIB counters, providing the
 *      counters necessary to support MAU, IEEE 802.3 and
 *      EtherLike MIB. The Port MAC MIB counters support
 *      different configuration options, such as enable/disable
 *      updating of the RMON Etherstat histogram counters for
 *      transmitted/received packets, enable/disable Counters
 *      Clear on read, MIB counters capture, etc. However, due
 *      to this erratum, Stack port MIB Counters Control
 *      Register, which controls the Flex ports (ports 24-27)
 *      MIB counters configurations, is not accessible for
 *      write. The Flex ports MIB counters default configuration
 *      behavior cannot be changed: MIB counter index 4 is sent
 *      deferred. MIB counter index 12 is Frames1024toMaxOctets.
 *      Stack Port histogram counters are enabled for updated
 *      for received/transmitted traffic. The MIB counters read
 *      by the CPU are not cleared after read. MIB Counters
 *      Control Register for network ports is accessible for
 *      write.
 *      (FEr#3092)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_INCORRECT_XG_SHAPER_TOKEN_BUCKET_WA_E -
 *      Ports 24-27 shaping rate defaults are different than that of the ports
 *      0-23.
 *      To enable the ports 24-27 to support shaping rates identical to ports
 *      0-23, set the <XgPortsTbAsGig> field, bit[31] in the Token Bucket
 *      Update Rate and MC FIFO Configuration Register (0x0180000C) to
 *      "Gig mode" (0x1).
 *      (RM#3030)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_BYPASS_PACKET_REFRESH_FDB_AGE_WA_E -
 *      Packets that bypass the Ingress Pipe perform a refresh of the age bit
 *      in the FDB.
 *      Set the value of the <Avoid_Age_Refresh> field, bit[22] in the Bridge
 *      Global Configuration2, to 0x1 (enable), so that packets that bypass the
 *      Ingress Pipe will not perform refresh of the age bit in the FDB.
 *      (RM#3031)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_PIPE_SELECT_WA_E -
 *      Wrong default in Pipe Select register and some new DFX settings.
 *      (RM#3032)
 *
 * PRV_CPSS_DXCH_XCAT2_RM_FLEX_LINK_PORTS_RATE_LIMIT_WA_E - the automatic
 *     assignment of the time windows to the FlexLink ports as a function of their
 *     operating speed is not functional and when rate limit is enabled on the
 *     FlexLink ports causes all packets to be dropped.
 *     (RM#3038)
 *
 * PRV_CPSS_DXCH_XCAT2_PTP_UDP_FRAGMENT_WA_E -  Wrong
 *     non-initial UDP fragments packets trap to CPU. Due to
 *     this erratum, the device checks non-initial UDP fragments
 *     and wrongly assumes a UDP header exists after the IPv4
 *     header. If the packet data contains the value 0x13F at
 *     the same  offset as the UDP header (2-byte offset from
 *     the start of data), the device traps the packet to the
 *     CPU.(FEr#3112)
 *
 * PRV_CPSS_DXCH_XCAT2_100FX_WA_E - traffic using fiber transceivers directly
 *    connected to FlexLink uplink ports. (HWEr#3068)
 *
 *  PRV_CPSS_DXCH_LION2_FAST_FAILOVER_WA_E -
 *              In fast failover the secondary port must be in the same TXQ unit
 *              as the primary port.
 *  PRV_CPSS_DXCH_LION2_RM_TTI_DUAL_DEV_ENABLE_WA_E -
 *             Dual Device Id field should be enable by default.
 *
 *  PRV_CPSS_DXCH_LION2_HGL_MAC_INT_WA_E -
 *             HGL mac interrupts recieved when HGL mac is no
 *             used.
 *
 *  PRV_CPSS_DXCH_XCAT2_PTP_FRAGMENT_PACKET_TRAP_WA_E - PTP
 *       fragment packets may be incorrectly identified as
 *       invalid. PTP fragment packets are forwarded normally to
 *       their destination port. However, due to this erratum, IP
 *       fragment packets may be incorrectly identified as PTP
 *       fragment packets by the TTI block and trapped to the
 *       CPU. (FEr#3124)
 *
 *  PRV_CPSS_DXCH_LION_RM_ENABLE_PFC_TRIGGER_WA_E -
 *      The default value of the pfc global trigger
 *      register<pfcEn> should be set to 0x1.
 *
 *  PRV_CPSS_DXCH_LION2_RM_WRONG_DEFAULT_CPFC_THRESHOLDS_WA_E -
 *      The default setting of the CPFC xoff and drop thresholds
 *      should be set to the maximum.
 *      (Ref #: RM-9082194)
 *
 *  PRV_CPSS_DXCH_LION2_RM_WRONG_DEFAULT_PFC_THRESHOLDS_WA_E -
 *      The default setting of the LLFC thresholds should be set
 *      to the maximum.
 *
 *  PRV_CPSS_DXCH_LION2_RM_LLFC_TRIGGER_GLOBAL_CONFIGURATION_WA_E
 *      The default value of the pfc global trigger
 *      register<pfcEn> should be set to 0x1.
 *
 *  PRV_CPSS_DXCH_LION2_RM_ENABLE_PFC_TRIGGER_WA_E -
 *      The default value of the cpfc global trigger
 *      register<cpfcEn> should be set to 0x1
 *      (Ref #: RM-9987707)
 *
 *  PRV_CPSS_DXCH_LION2_RM_GLOBAL_XOFF_ACTIVATION_ON_PORT_WA_E -
 *      The device Global pause default setting is incorrect.
 *
 *  PRV_CPSS_DXCH_LION2_LOCAL_SRC_PORT_DEV_MAP_ENABLE_WA_E -
 *      The device supports a Device Map table that maps a destination device
 *      number to a cascaded port or cascaded trunk group. The Device Map
 *      look-up for the local device is based on a per-source-port enable bit
 *      and per-destination-port enable bit configuration.
 *      However, due to this erratum,
 *      the Device Map table enable bit configuration supports only 6 bits
 *      (up to 64 ports) for the per-source-port field instead of 7 bits
 *      (up to 128 ports). This will cause the same local port number in both
 *      hemispheres to be enabled at the same time.
 *      The WA configures EGR units asymmetrically:
 *      - The Local Src Port Map Own Dev En bitmap of specific EGR unit
 *      comprises bits only source port group related to the unit.
 *      E.g. EGR0 comprise bits 0..15, EGR1 comprise bits 16..31 and so on.
 *      - Write non zero values of source and target port bitmaps only for
 *      ERG units where both target and source ports are exist.
 *      (FE-6816092)
 *
 * PRV_CPSS_DXCH_LION2_SHARE_EN_CHANGE_WA_E -
 *      Changing the share enable after some traffic passed in the TXDMA cause
 *      system to hang: stuck buffers & stuck port (TXFIFO). The
 *      failure occurs even when the change is not under traffic
 *      and when TXDMA is completely idle. The failure occurs
 *      when changing the share enable in both directions: share
 *      to not share and the other way around.
 *      (DIP 423, Lion-B0 #1327)
 *
 *  PRV_CPSS_DXCH_LION2_IPM_BRIDGE_COPY_GET_DROPPED_WA_E -
 *      IPM bridge copy get dropped when there is no member in
 *      the local hemisphere vlan
 *      WA is related to Lion2 devices starting from B0 revision
 *      WA solution:
 *      The WA use not connected to MAC ports 14 and 64+14 in
 *      the TxQ.
 *      Ports 14 of each hemisphere have Link UP in the TxQ unit
 *      The WA sets in every VLAN enabled for IP MC Routing Port
 *      14 and port 64+14 as a member.
 *      Packets in such VLANs go to ports 14 and 64+14 TxQ
 *      queues as well as to other ports in VLANs.
 *      The WA uses the Tail drop to avoid packets stuck in the
 *      ports 14 and 64+14 TxQ queues.
 *      The tails drop configurations is:
 *      - Tail Drop must be enabled
 *      - Configure Port 14 and port 64+14 to TailDropProfile 8
 *        NOTE: User can use only 7 out of 8 available profiles
 *      - Set Queue limits of profile 8 to all zeros
 *      (FE-7263524)
 * PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E -
 *      The device supports duplication of traffic to ports within the assigned
 *      VLAN. The device incorporates two internal
 *      engines that are responsible for the replication to all ports.
 *      However, due to this erratum, In some scenarios, although the traffic bandwidth
 *      in a specific PortGroup does not exceed the allowed bandwidth,
 *      not all VLAN members will transmit the replication.
 *      This behavior is due to extra replication (relay port)
 *      that is performed between the two internal replication engines.
 *      The restriction applies to a packet size under 128B only.
 *      The problem relevant both to Assigned Multicast (VIDX) and VLAN flooded traffic.
 *      Workaround
 *      Add the relay port to the VLAN only if
 *      ports from both hemisphere are members in the VLAN.
 *      (FE-6041254)
 *
 * PRV_CPSS_DXCH_LION2_SCHEDULER_PROFILE_UNDER_TRAFFIC_WA_E
 *      The device supports two independent groups of SDWRR
 *      queues on a port. Within each SDWRR group, the queues
 *      are serviced according to their configured weight.
 *      After changing weights profile a trigger to
 *      "UpdateSchedVarTrigger" Configuration bit need to be
 *      done to activate the new setting immediatlly. However,
 *      due to this erratum, triggering the
 *      "UpdateSchedVarTrigger" Configuration bit during traffic
 *      might cause BadOctetsReceived and RxErrorFrameReceived
 *      events in the Rx MAC MIBs.
 *      (FE-221066)
 *
 * PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E -
 *      When connecting a 10GBase-R port to a 40GBase-R port, where the
 *      IEEE 802.3ba Backplane Auto-Negotiation Service (AP) is
 *      not used, this could result in a false Link Up on the
 *      10GBase-R side.
 *      (AN-333: Lion2 10G to 40G Port Link)
 *
 * PRV_CPSS_DXCH_LION2_GIGE_MAC_LINK_STATUS_WA_E -
 *       In Lion2 port in tri-speed mode link status doesn't
 *      change in some cases when cable is disconnected/connected.
 *      The workaround fix the problem.
 *     (FE-4933007)
 * PRV_CPSS_DXCH_LION2_CUT_THROUGH_SLOW_TO_FAST_WA_E -
 *      Lion2 device does not allow cut through traffic
 *      from slow port to faster one. The WA allows traffic from
 *      slow port with cut through enabled to be transmitted
 *      from faster port.
 *
 *  PRV_CPSS_DXCH_LION2_WRONG_DEFAULT_MLL_DATA_INTEGRITY_PARITY_INTERRUPT_WA_E-
 *      When pariry error is detected in MLL FIFO, an interrupt
 *      is generated to indicate the error. However, due to this
 *      erratum, false interrupt is generated during init
 *      sequence. Reading cause register will fix it.
 *      (FE-8054972)
 *
 *  PRV_CPSS_DXCH_LION2_WRONG_DEFAULT_BMA_MC_DATA_INTEGRITY_PARITY_INTERRUPT_WA_E-
 *      When pariry error is detected in BMA MC data, an
 *      interrupt is generated to indicate the error. However,
 *      due to this erratum, false interrupt is generated during
 *      init sequence. Reading cause register will fix it.
 *      (FE-8071239)
 *
 * PRV_CPSS_DXCH_LION2_RM_WRONG_GIG_DESC_FIFO_THRESHOLD_WA_E -
 *      TXDMA releases buffers during normal operation. However,
 *      due to this erratum, buffers are not released causing
 *      the system (device) to hang, when jumbo packets are
 *      handled. In register: Buffer Memory GigFIFOs Threshold
 *      Field: GigDescFifoThreshold should be 0x4 instead of
 *      0xf.
 *      (RM-261461)
 *
 * PRV_CPSS_DXCH_LION2_PORT_MAC_MIB_COUNTERS_CORRUPTION_WA_E -
 *      The device implements MAC MIB counters, providing the counters necessary
 *      to support MAU, IEEE 802.3, and EtherLike MIB. However,
 *      due to this erratum, corruption in the MIB counters'
 *      values may occur upon exiting the port MAC reset state.
 *      This erratum is relevant in cases where traffic already
 *      passed through a port. Workaround: Clear the MIB
 *      counters after MAC reset. (FE-9085687)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_NON_ORIG_VID_ON_TRAP_TO_CPU_WA_E -
 *      the device supports 'orig vid' for ingress tagged packets that send to
 *      the CPU.
 *      due to this erratum, the vid , in the ingress tagged packets that send to
 *      the CPU , is vid that was modified by the ingress pipe.
 *      The WA that the CPSS do :
 *          1. set <Force new DSA TO_CPU> on the CPU port.
 *      Impact on application:
 *          The info about the 'tagging' of the packets is always 'not tagged'.
 *          (CPSS_DXCH_NET_RX_PARAMS_STC::dsaParam.dsaInfo.toCpu.isTagged == GT_FALSE)
 *          meaning that the application should (if need) do tag classification
 *          by it's own.
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_WRONG_TXDMA_FIFO_THRESHOLD_WA_E -
 *      The buffer memory read request FIFO queue is too short compared to the
 *      default threshold settings of the TxDMA FIFO queue. However, due to this
 *      erratum, read requests to the buffer memory sent by the TxDMA may get
 *      lost.
 *      (RM-3702958)
 *
 * PRV_CPSS_DXCH_BOBCAT2_RM_RATE_LIMIT_DEFICIT_COUNT_EN_WA_E -
 *      RateLimitDefecitCountEn default value should be set
 *
 * PRV_CPSS_DXCH_BOBCAT2_RM_STRICT_PRIORITY_FOR_UC_PACKETS_WA_E
 *      Wrong default value for register <MCUC SDWRR and Strict
 *      Priority Configuration>. The field <UcStrictEn> should
 *      be configured to "Strict Priority"
 *
 * PRV_CPSS_DXCH_BOBCAT2_RM_TTI_BAD_DEFAULT_VALUE_FIX_WA_E -
 *      Wrong TTI metal fix default value
 *      (RM-7143731)
 *
 * PRV_CPSS_DXCH_BOBCAT2_RM_GLOBAL_EPORT_RANGE_VALUE_AND_MASK_WA_E
 *      Default for the couple <Global Eport Range Value> and
 *      <Global ePort Range Mask> should be "never match"
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_BRIDGE_COMMANDS_DEFAULTS_WA_E -
 *      Wrong defaults of bridge commands
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_CONTROL_FROM_CPU_EGRESS_FILTER_WA_E
 *      Wrong default value for enabling VLAN filtering for
 *      control traffic from CPU
 *
 *  PRV_CPSS_DXCH_BOBCAT2_MPLS_TRANSIT_TUNNEL_PARSING_WA_E -
 *      When Passenger Parsing of Transit MPLS Tunnels is enabled,
 *      the device implicitly locates start of the passenger payload
 *      based on the bottom of stack label (S-bit is set).
 *      In this case, the TTI Action Entry <Tunnel Header Length> is not relevant.
 *      However, due to this erratum, the device does not automatically locate
 *      the bottom of stack label to find the start of the passenger payload.
 *      Instead, the device uses the TTI Action Entry <Tunnel Header Length>
 *      to locate the start of the passenger payload
 *      (FE-4300730)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_EGRESS_MIB_COUNTERS_NOT_ROC_WA_E -
 *      Egress MIB counters are not cleared on read.
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_TXQ_PORT_REQUEST_MASK_WA_E -
 *      TxQ Port Request mask shall have default value 4 instead of 3
 *      (FE-8708889)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_TAI_CAPTURE_STATUS_SET_WA_E -
 *      TAI Time Capture Status Valid bits (Capture Register0 Valid &
 *      Capture Register1 Valid) are initialized to 1 instead of 0 after reset.
 *      (RM-3039204)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_NOT_ALLOW_TTI_ACTION_TO_SET_MODIFY_UP_DSCP_WA_E -
 *      TTI Action can not set ModifyUP and/or ModifyDSCP
 *      if previously assigned with QoSPrecedence==HARD
 *
 *  PRV_CPSS_DXCH_BOBCAT2_ILKN_PACKET_SIZE_LIMITATION_NOT_ENFORCED_WA_E
 *      A packet size limitation can be enforced on the
 *      Interlaken port. Received packets that are longer than
 *      the specified limitation in bytes are dropped. Oversized
 *      packets received by the Interlaken port are not dropped
 *      and can cause the Interlaken port to stop responding.
 *      (FE-820601)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_ENH_TAILDROP_MODE_WRED_CONFLICT_WA_E
 *      WRED must not be enabled on shared pools in enhanced
 *      taildrop mode.
 *      When using enhanced tail drop (device default mode) and
 *      the WRTD(WRED) is enabled, the shared pool counters are
 *      not updated correctly by the enqueue and dequeue parts
 *      of the TXQ. (FE-4381191)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_PFC_NOT_SUPPORTED_PORTS_48_71_WA_E
 *      Ports 48-71 do not support Priority-based Flow Control
 *      (PFC).  (FE-6841659)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_FC_802_3X_NOT_SUPPORTED_TM_MAPPED_PORTS_WA_E
 *      Flow Control is not supported for ports that pass
 *      through the Traffic Manager. (FE-7583456)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_PROT_SWITCH_STAGE_EN_DEFAULT_VALUE_WA_E
 *      In Protection switching, Protection Switching Stage Enable bit[2] of the
 *      ePort Global Configuration register should be enabled by default.
 *      However, due to this erratum, Protection Switching Stage Enable bit[2]
 *      of the ePort Global Configuration register is incorrect.
 *      The WA sets bit to be 1 during initialization.
 *      (RM-7299863)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_FC_SENT_COUNTERS_PROBLEM_WHEN_FCU_BYPASS_DISABLE_WA_E
 *      When we are working with FC/PFC (FCA bypass disable) the
 *      FC TX packets are counted in FCA unit and in
 *      goodSentPcts and not in fcSent in Mib Counters
 *      (FE-5101126)
 *
 *  PRV_CPSS_DXCH_XCAT3_UC_LOCAL_SWITCH_FILTER_FOR_VIRTUAL_PORTS_WA_E
 *      The device supports the ability to enable/disable local switching
 *      traffic back through the ingress interface for known Unicast packets.
 *      However, due to this erratum, Unicast packets local switching filtering
 *      is not functional with logical ports.
 *      (FE-31600)
 *
 *  PRV_CPSS_DXCH_XCAT3_TRUNK_ID_ASSIGN_INSTEAD_OF_VIRTUAL_PORT_WA_E
 *      Traffic is identified in the FDB as "learned on a trunk", instead of
 *      learned from a logical port, when a source port is configured as a trunk
 *      member and traffic is assigned with a logical port.
 *      (FE-1000365)
 *
 *  PRV_CPSS_DXCH_XCAT3_OAM_LB_PORT_NOT_GETTING_HIGHEST_QOS_WA_E
 *      Traffic on OAM loopback port is not getting highest QoS, due to this
 *      erratum, the transmitted back traffic on the OAM loopback port gets
 *      randomized TC and DP.
 *      (FE-27970)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_DESCRIPTOR_ECC_PROTECTION_CORRUPTION_WITH_REMOTE_PORTS_WA_E
 *      ECC protection of descriptor fields is not operational
 *      when a remote physical port is used.
 *      ECC protection of the descriptor is not functional when
 *      the Remote Physical Ports feature is used, because the
 *      local device's source port field of the descriptor is
 *      modified by the TTI. False ECC indications appear. This
 *      includes any use of the Interlaken interface.
 *      (FE-9897292 )
 *
 * PRV_CPSS_DXCH_BOBCAT2_FDB_ROUTE_UC_DELETE_BY_MESSAGE_WA_E -
 *      CPU NA message for deleting an entry does not work for
 *      UC route entries (FE-2293984)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_TCAM_WRITE_PARITY_ERROR_WA_E
 *      The TCAM has parity protection. The parity daemon
 *      periodically reads TCAM entries and checks parity bit.
 *      The daemon does not synchronizes TCAM reads and parity DB
 *      reads with write to TCAM management operation. Therefore
 *      sometimes the daemon erroneously detects parity error.
 *      The WA disables daemon before write to TCAM and enables
 *      it back after write.
 *      (FE-5092995)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_EQ_TO_CPU_RATE_LIMIT_WINDOW_RESOLUTION_WA_E
 *      CPU rate limiter is not working at low values of Conf.
 *      When ToCPU RLWindow Resolution [10:0] <= 0x4, CPU rate limiter is not working.
 *      Problem also can occur with MG is accessing to Rate Limiter RAM,
 *      while Rate Limiter is Enabled.
 *      The problem is that design requires to receive response from RAM till clocks.
 *      Minimum allowed value is taken from RAM round-trip latency
 *
 * PRV_CPSS_DXCH_BOBCAT2_EPLR_TO_CPU_OAM_WA_E
 *      OAM engine uses Egress Policer (EPLR) for Loss Measurement (LM). TO_CPU
 *      packets are treated by EPLR for LM also. However, due to the erratum,
 *      TO_CPU OAM LM packets are not processed by EPLR.
 *      The WA enables processing of all TO_CPU packets by EPLR.
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_WRONG_AVS_INIT_WA_E - B0 registers misconfiguration
 *      We need to change the default setting of AVS in BC2B.
 *          1.        Need to sample on upper metal
 *          2.        Update voltage to 1.02V
 *      /Cider/EBU/Bobcat2B/Bobcat2 {Current}/<SERVER_IP> DFX Server/Units/
 *          DFX Server Units/DFX Server Registers/AVS <%a> Disabled Control 1
 *      Address 0x000F8128[23]
 *      Defaults (check) should be '1' - set to '0'
 *      To set to 1.02:
 *      /Cider/EBU/Bobcat2B/Bobcat2 {Current}/<SERVER_IP> DFX Server/Units/
 *              DFX Server Units/DFX Server Registers/AVS <%a> Enabled Control
 *      Address: 0x000F8130
 *      Bits 11:4 & 19:12 default is 35[Ofir A.]  (0x23), set to 38[Ofir A.]  (0x26)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RXDMA_PIP_IPV6_WRONG_TC_BITS_WA_E -
 *      relate the RxDMA PIP feature (SIP_5_10) , the 'TC' for the IPv6 packets
 *      should be bits 116..123 of the packet but actually taken from
 *      bits 118..125
 *
 *  PRV_CPSS_DXCH_BOBCAT2_LED_LINK_GIG_INDICATION_WA_E -
 *      The device LED controller provides port status for all ports modes.
 *      One of the indications is "link status". However, due to this erratum,
 *      the Gig MAC link indication status does not reflect the correct status
 *      of the port. Gig Mac is relevant for 1Gig and 2.5Gig ports.
 *      Workaround:
 *      When setting a port to use Gig MAC, set Field "port[n] class1 select"
 *      bits [9:5] in the "Port[n] indication select" register to 0xa.
 *      For all other ports mode set this field to 0x1.
 *      (FE-1904523)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_GPP_INTERRUPT_WA_E -
 *      The GPP interrupts need to be masked , as they generate non-stop interrupts.
 *      The device General Purpose Pins (GPP) are connected to the switch core
 *      and can be used as an input/output pin or as an interrupt input.
 *      However, due to this erratum, the switch core GPP are not functional and
 *      should not be used.
 *      (HWE-9666408)
 *
 * PRV_CPSS_DXCH_BOBCAT2_SGMII_TO_CPU_NOT_SUPPORTED_WA_E -
 *      The Control and Management subsystem is connected to the switch using the
 *      SDMA mechanism. In addition, packets may be transferred between the Control
 *      and Management subsystem and the switch by establishing an internal SGMII
 *      connection using SERDES 20 and/or SERDES 21.
 *      However, due to this erratum, an internal SGMII connection between the
 *      Control and Management subsystem and the switch is not operational.
 *      The SGMII interconnection between the Control and Management subsystem and
 *      the switch is not supported.
 *      (FE-5613945)
 *
 *  PRV_CPSS_DXCH_BOBCAT2_RM_WRONG_DEFAULT_DEVICE_CONTROL_14_WA_E -
 *      The default setting of the Device Control 14 register, bits [31..29]
 *      should be set to four.
 *      (Ref #: RM-7056426)
 *  
 *  PRV_CPSS_DXCH_BOBCAT2_RM_DFX_REGISTERS_MISSCONFIGURATION_WA_E
 *  -   Memories hold optimal WTC/RTC.
 *      However, due to this erratum, several memories do not hold
 *      optimal WTC/RTC in the memory wrapper registers.
 *      (Ref #: RM-79308357)
 *  
 *  PRV_CPSS_DXCH_XCAT3_GIG_MAC_HALF_DUPLEX_COLLISION_DOMAIN_RM_E
 *      The default value of bits [6-11] in the Port Serial
 *      Parameter Configuration register is wrong.
 *      (RM-6302994)
 *
 *  PRV_CPSS_DXCH_XCAT3_XLG_PORT_DROP_EVENT_PROBLEM_RM_E
 *      DMA Write access weight is increased in port RxFIFO.
 *      This change ensures that packets are written to memory
 *      If there is congestion on memory access.
 *      The Buffer Memory Tx Fifo Threshold and MPPM Aceess Arbiter Configuration
 *      Register is set as 0x4C4E6106
 *      For Stacking ports Port Rx FIFO is set to 4 to accomodate the pressure.
 *      The Port FIFO Thresholds Configuration Registers are set as 0x7BE4.
 *      (RM-6565603)
 *
 * PRV_CPSS_DXCH_XCAT3_RM_RX_SDMA_WRONG_QUEUE_WA_E -
 *      The CPU receives packets from the device directly to pre-allocate space in
 *      host memory using up to eight device Rx SDMA queues according to the
 *      configured mapping.
 *      However, due to this erratum, if there is a congestion on CPU port packets
 *      may go to the different queue from those configured in the mapping.
 *      WA -Set field set_port_sdma_ to 0 in register General_Configurations_1
 *      (RM-8732277)
 *
 * PRV_CPSS_DXCH_CAELUM_EGRESS_MEMORIES_NOT_RESET_WA_E -
 *      Device resets all memories and sets to default values after power UP or
 *      soft reset.
 *      However, due to this erratum,memories of several egress units (EPCL, EPLR, HA, e.t.c)
 *      are not reset and have random values.
 *      The WA fixes wrong default values of EPLR related memories.
 *
 * PRV_CPSS_DXCH_CAELUM_INGRESS_VLAN_RANGE_CONFIG_WA_E -
 *      Ingress Bridge VLAN Table has 4.5 K (4608) entries in BobK.
 *      However, due to this erratum, Ingress VLAN Range Configuration register
 *      is initialized to 8K-2 which is bigger than the table size.
 *      The WA fixes wrong default values of Ingress VLAN Range Configuration register.
 *
 *  PRV_CPSS_DXCH_ERRATA_MAX_NUM_E
 *              the MAX value to be used for array sizes
 *
 */
typedef enum{

    PRV_CPSS_DXCH_FDB_AGING_WITH_REMOVAL_MODE_WA_E,
    PRV_CPSS_DXCH_TRUNK_CONFIG_FOR_TRUNK_NEXT_HOP_WA_E,
    PRV_CPSS_DXCH_INCORRECT_SRCDEV_FIELD_IN_MAILBOX_TO_CPU_DSA_TAG_WA_E,
    PRV_CPSS_DXCH_TOKEN_BUCKET_RATE_SHAPING_DISABLE_WA_E,
    PRV_CPSS_DXCH_JUMBO_FRAMES_MODE_SDWRR_WEIGHT_LIMITATION_WA_E,
    PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E,
    PRV_CPSS_DXCH_RATE_LIMIT_GIG_COUNTER_BITS_NUM_WA_E,
    PRV_CPSS_DXCH_XG_MIB_READ_OVERSIZE_AND_JABBER_COUNTERS_WA_E,
    PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E,
    PRV_CPSS_DXCH2_READ_SRC_ID_ASSIGMENT_MODE_CONF_REG_WA_E,
    PRV_CPSS_DXCH2_BRIDGE_ACCESS_MATRIX_LINE7_WRITE_WA_E,
    PRV_CPSS_DXCH2_POLICY_ACTION_DIRECT_BURST_ONLY_READ_WA_E,
    PRV_CPSS_DXCH2_DIRECT_ACCESS_TO_POLICER_ADDRESS_SPACE_WA_E,
    PRV_CPSS_DXCH2_ROUTER_ARP_AND_TS_TABLE_NOT_SUPPORT_BURST_ACCESS_VIA_PCI_WA_E,
    PRV_CPSS_DXCH2_RDWRTRIG_BIT_IN_VLT_TABLES_ACCESS_CONTROL_REGISTER_IGNORED_E,
    PRV_CPSS_DXCH2_INDIRECT_ACCESS_ROUTER_AND_TT_TCAM_NOT_FUNCTION_CORRECTLY_WA_E,
    PRV_CPSS_DXCH_RM_XG_PHY_PATH_FAILURE_WA_E,
    PRV_CPSS_DXCH2_RM_RESERVED_REGISTERS_0x0B820108_WA_E,
    PRV_CPSS_DXCH_RM_FDB_TABLE_ENTRIES_WA_E,
    PRV_CPSS_DXCH2_PCL_EGRESS_ACTION_CNTR_VLAN_WRONG_BITS_POS_WA_E,
    PRV_CPSS_DXCH2_RM_BRG_LAYER_4_SANITY_CHECKS_WA_E,
    PRV_CPSS_DXCH3_VLT_INDIRECT_ACCESS_WA_E,
    PRV_CPSS_DXCH3_VLT_TABLES_WRITE_DIRECT_ACCESS_WA_E,
    PRV_CPSS_DXCH3_SAMPLE_AT_RESET_NET_REF_CLOCK_SELECT_WA_E,
    PRV_CPSS_DXCH3_RM_BM_GE_XG_PORT_GROUP_LIMIT_CFG_REG_WA_E,
    PRV_CPSS_DXCH3_RM_GE_PORTS_MODE1_REG_WA_E,
    PRV_CPSS_DXCH3_RM_XG_PORTS_MODE_REG_WA_E,
    PRV_CPSS_DXCH3_RM_TCAM_PLL_REG_WA_E,
    PRV_CPSS_DXCH3_RM_LMS0_1_GROUP1_LED_INTERFACE_WA_E,
    PRV_CPSS_DXCH3_RM_MAC2ME_LOW_REG_WA_E,
    PRV_CPSS_DXCH3_RM_AN_CNF_REG_WA_E,
    PRV_CPSS_DXCH3_DIRECT_ACCESS_TO_EQ_ADDRESS_SPACE_WA_E,
    PRV_CPSS_DXCH3_TXQ_PARITY_CALCULATION_E,
    PRV_CPSS_DXCH3_SDMA_WA_E,
    PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E,
    PRV_CPSS_DXCH3_OUT_OF_PROFILE_QOS_ATTR_E,
    PRV_CPSS_DXCH3_TXQ_FULL_INTERRUPT_NOT_FUNCTION_WA_E,
    PRV_CPSS_DXCH3_RM_GE_SERDES_MISC_CONF_REG_WA_E,
    PRV_CPSS_DXCH3_RM_BM_TX_FIFO_THRESHOLD_CONF_REG_WA_E,
    PRV_CPSS_DXCH3_TCAM_EFUSE_NOT_TRIGGERED_AUTO_WA_E,
    PRV_CPSS_DXCH3_POLICY_AND_ROUTER_TCAM_TABLES_INDIRECT_READ_WA_E,
    PRV_CPSS_DXCH_RM_FDB_GLOBAL_CONF_REG_WA_E,
    PRV_CPSS_DXCH3_LIMITED_NUMBER_OF_POLICY_BASED_ROUTES_WA_E,
    PRV_CPSS_DXCH3_TCAM_REPAIR_WA_E,
    PRV_CPSS_DXCH3_RM_CPU_ACCESS_TO_FDB_UNDER_TRAFFIC_WA_E,
    PRV_CPSS_DXCH3_SERDES_YIELD_IMPROVEMENT_WA_E,
    PRV_CPSS_DXCH3_TOGGLE_DEV_EN_UNDER_TRAFFIC_WA_E,

    PRV_CPSS_DXCH_XCAT_RM_TOTAL_BUFFER_LIMIT_CONF_IN_TXQ_E,
    PRV_CPSS_DXCH_XCAT_RM_CNC_ENABLE_COUNTING_E,
    PRV_CPSS_DXCH_XCAT_RM_TXQ_EXT_CNTR_REG_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_MIRROR_INTERFACE_PARAMETER_REGISTER_I_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_POLICY_ENGINE_CONFIGURATION_REGISTER_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_TXQ_DEQUEUE_SWRR_WEIGHT_REGISTER_3_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL0_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_INGRESS_POLICER_CONTROL1_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_POLICER_CONTROL2_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_EGRESS_POLICER_GLOBAL_CONFIGURATION_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_PORTS_BCN_AWARENESS_TABLE_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_TTI_UNIT_GLOBAL_CONFIGURATION_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_EXTENDED_DSA_BYPASS_BRIDGE_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_PORT_SERIAL_PARAMETERS_CONFIGURATION_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_LMS0_LMS1_MISC_CONFIGURATIONS_WA_E,
    PRV_CPSS_DXCH_XCAT_TX_CPU_CORRUPT_BUFFER_WA_E,
    PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E,
    PRV_CPSS_DXCH_XCAT_IEEE_RESERVED_MC_CONFG_REG_READ_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E,
    PRV_CPSS_DXCH_XCAT_GIGA_PORT_PRBS_PARAM_INIT_WA_E,
    PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_HEADER_ALTERATION_GLOBAL_CONFIGURATION_REGISTER_WA_E,
    PRV_CPSS_DXCH_XCAT_TOD_NANO_SEC_SET_WA_E,
    PRV_CPSS_DXCH_XCAT_ETH_OVER_IPV4_GRE_WA_E,
    PRV_CPSS_DXCH_XCAT_PASSENGER_ETH_PACKET_CRC_CANT_BE_RETAINED_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_MAC_CTRL_REG3_PCS_PREAMBLE_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_METAL_FIX_SGMII25G_DPCLK_SEL_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_BCN_CONTROL_CCFC_SRC_PORT_WA_E,
    PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E,
    PRV_CPSS_DXCH_XCAT_FC_FORWARD_NOT_FUNCTIONAL_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_SSMII_PAD_ZNR_ZPR_WA_E,
    PRV_CPSS_DXCH_XCAT_RM_PASSENGER_TPID_REG_WA_E,

    PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E,
    PRV_CPSS_DXCH_LION_INDIRECT_ACCESS_TO_TCAM_IS_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E,
    PRV_CPSS_DXCH_LION_RM_SYNC_ETHERNET_NONE_SELECT_WA_E,
    PRV_CPSS_DXCH_LION_RM_TCAM_CONFIG_WA_E,
    PRV_CPSS_DXCH_LION_URPF_PER_VLAN_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_LION_TB_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_LION_PREVENT_REDUNDANT_AU_FU_MESSAGES_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_LION_POLICER_COUNTING_MEM_ACCESS_WA_E,
    PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_FDB_LIMIT_WA_E,
    PRV_CPSS_DXCH_LION_RM_ORIG_SRC_PORT_FILTERING_WA_E,
    PRV_CPSS_DXCH_LION_RM_MULTI_PORT_GROUP_CPU_IN_VIDX_WA_E,
    PRV_CPSS_DXCH_LION_CN_GLOBAL_REG_WA_E,
    PRV_CPSS_DXCH_LION_TEMPERATURE_SENSOR_INITIALIZATION_WA_E,
    PRV_CPSS_DXCH_LION_RM_MCFIFO_DISTRIBUTION_REG_WA_E,
    PRV_CPSS_DXCH_LION_RM_SD_PLL_REG_WA_E,
    PRV_CPSS_DXCH_LION_SLOW_RATE_ON_PORT_TB_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_LION_RM_UC_STRICT_OVER_MC_WA_E,
    PRV_CPSS_DXCH_LION_RM_SCHEDULER_DEFICIT_MODE_WA_E,
    PRV_CPSS_DXCH_LION_RM_TX_MC_IN_CHUNKS_WA_E,
    PRV_CPSS_DXCH_LION_RM_BUFF_LIMIT_EGRESS_QUEUE_WA_E,
    PRV_CPSS_DXCH_LION_RM_QCN_FOR_CN_FORMAT_WA_E,
    PRV_CPSS_DXCH_LION_XLG_WS_SUPPORT_WA_E,
    PRV_CPSS_DXCH_LION_PFC_FOR_MANY_PRIORITY_WA_E,
    PRV_CPSS_DXCH_LION_QCN_FORMAT_COMPLIANT_WA_E,
    PRV_CPSS_DXCH_LION_INACCURATE_PFC_XOFF_WA_E,
    PRV_CPSS_DXCH_LION_INCOMPLETE_CPU_MAILBOX_INFO_WA_E,
    PRV_CPSS_DXCH_LION_IPCL_FALSE_PARITY_ERROR_WA_E,
    PRV_CPSS_DXCH_LION_PORT_REQUEST_RATE_RM_WA_E,
    PRV_CPSS_DXCH_LION_WRONG_COUNTING_OF_IP_COUNTERS_PER_PORT_WA_E,
    PRV_CPSS_DXCH_LION_RM_PFC_DSA_TAG_E,
    PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E,

    PRV_CPSS_DXCH_XCAT2_RM_STACK_PORT_MIB_CNTR_CONTROL_WA_E,
    PRV_CPSS_DXCH_XCAT2_EPCL_GLOBAL_EN_NOT_FUNCTIONAL_WA_E,
    PRV_CPSS_DXCH_XCAT2_RM_POLICER_COUNT_FORMAT_WA_E,
    PRV_CPSS_DXCH_XCAT2_RM_INT_EXT_REF_CLK_WA_E,
    PRV_CPSS_DXCH_XCAT2_RM_POLICER_MEM_CONTROL_WA_E,
    PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E,
    PRV_CPSS_DXCH_XCAT2_RM_INCORRECT_XG_SHAPER_TOKEN_BUCKET_WA_E,
    PRV_CPSS_DXCH_XCAT2_RM_BYPASS_PACKET_REFRESH_FDB_AGE_WA_E,
    PRV_CPSS_DXCH_XCAT2_RM_PIPE_SELECT_WA_E,
    PRV_CPSS_DXCH_XCAT2_RM_FLEX_LINK_PORTS_RATE_LIMIT_WA_E,
    PRV_CPSS_DXCH_XCAT2_PTP_UDP_FRAGMENT_WA_E,
    PRV_CPSS_DXCH_XCAT2_100FX_WA_E,
    PRV_CPSS_DXCH_LION2_FAST_FAILOVER_WA_E,
    PRV_CPSS_DXCH_LION2_RM_TTI_DUAL_DEV_ENABLE_WA_E,
    PRV_CPSS_DXCH_LION2_HGL_MAC_INT_WA_E,
    PRV_CPSS_DXCH_XCAT2_PTP_FRAGMENT_PACKET_TRAP_WA_E,

    PRV_CPSS_DXCH_LION_RM_ENABLE_PFC_TRIGGER_WA_E,
    PRV_CPSS_DXCH_LION2_RM_WRONG_DEFAULT_CPFC_THRESHOLDS_WA_E,
    PRV_CPSS_DXCH_LION2_RM_WRONG_DEFAULT_PFC_THRESHOLDS_WA_E,
    PRV_CPSS_DXCH_LION2_RM_LLFC_TRIGGER_GLOBAL_CONFIGURATION_WA_E,
    PRV_CPSS_DXCH_LION2_RM_ENABLE_PFC_TRIGGER_WA_E,
    PRV_CPSS_DXCH_LION2_RM_GLOBAL_XOFF_ACTIVATION_ON_PORT_WA_E,
    PRV_CPSS_DXCH_LION2_LOCAL_SRC_PORT_DEV_MAP_ENABLE_WA_E,
    PRV_CPSS_DXCH_LION2_SHARE_EN_CHANGE_WA_E,
    PRV_CPSS_DXCH_LION2_IPM_BRIDGE_COPY_GET_DROPPED_WA_E,
    PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E,
    PRV_CPSS_DXCH_LION2_SCHEDULER_PROFILE_UNDER_TRAFFIC_WA_E,
    PRV_CPSS_DXCH_LION2_DISMATCH_PORTS_LINK_WA_E,
    PRV_CPSS_DXCH_LION2_GIGE_MAC_LINK_STATUS_WA_E,
    PRV_CPSS_DXCH_LION2_CUT_THROUGH_SLOW_TO_FAST_WA_E,
    PRV_CPSS_DXCH_LION2_WRONG_DEFAULT_MLL_DATA_INTEGRITY_PARITY_INTERRUPT_WA_E,
    PRV_CPSS_DXCH_LION2_WRONG_DEFAULT_BMA_MC_DATA_INTEGRITY_PARITY_INTERRUPT_WA_E,
    PRV_CPSS_DXCH_LION2_RM_WRONG_GIG_DESC_FIFO_THRESHOLD_WA_E,
    PRV_CPSS_DXCH_LION2_PORT_MAC_MIB_COUNTERS_CORRUPTION_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_NON_ORIG_VID_ON_TRAP_TO_CPU_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_WRONG_TXDMA_FIFO_THRESHOLD_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_RATE_LIMIT_DEFICIT_COUNT_EN_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_STRICT_PRIORITY_FOR_UC_PACKETS_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_TTI_BAD_DEFAULT_VALUE_FIX_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_GLOBAL_EPORT_RANGE_VALUE_AND_MASK_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_BRIDGE_COMMANDS_DEFAULTS_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_CONTROL_FROM_CPU_EGRESS_FILTER_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_MPLS_TRANSIT_TUNNEL_PARSING_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_EGRESS_MIB_COUNTERS_NOT_ROC_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_TXQ_PORT_REQUEST_MASK_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_TAI_CAPTURE_STATUS_SET_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_NOT_ALLOW_TTI_ACTION_TO_SET_MODIFY_UP_DSCP_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_ILKN_PACKET_SIZE_LIMITATION_NOT_ENFORCED_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_ENH_TAILDROP_MODE_WRED_CONFLICT_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_PFC_NOT_SUPPORTED_PORTS_48_71_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_FC_802_3X_NOT_SUPPORTED_TM_MAPPED_PORTS_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_PROT_SWITCH_STAGE_EN_DEFAULT_VALUE_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_FC_SENT_COUNTERS_PROBLEM_WHEN_FCU_BYPASS_DISABLE_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_DESCRIPTOR_ECC_PROTECTION_CORRUPTION_WITH_REMOTE_PORTS_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_FDB_ROUTE_UC_DELETE_BY_MESSAGE_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_TCAM_WRITE_PARITY_ERROR_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_EQ_TO_CPU_RATE_LIMIT_WINDOW_RESOLUTION_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_EPLR_TO_CPU_OAM_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_WRONG_AVS_INIT_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RXDMA_PIP_IPV6_WRONG_TC_BITS_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_LED_LINK_GIG_INDICATION_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_GPP_INTERRUPT_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_SGMII_TO_CPU_NOT_SUPPORTED_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_WRONG_DEFAULT_DEVICE_CONTROL_14_WA_E,
    PRV_CPSS_DXCH_BOBCAT2_RM_DFX_REGISTERS_MISSCONFIGURATION_WA_E,

    PRV_CPSS_DXCH_XCAT3_UC_LOCAL_SWITCH_FILTER_FOR_VIRTUAL_PORTS_WA_E,
    PRV_CPSS_DXCH_XCAT3_TRUNK_ID_ASSIGN_INSTEAD_OF_VIRTUAL_PORT_WA_E,
    PRV_CPSS_DXCH_XCAT3_OAM_LB_PORT_NOT_GETTING_HIGHEST_QOS_WA_E,
    PRV_CPSS_DXCH_XCAT3_GIG_MAC_HALF_DUPLEX_COLLISION_DOMAIN_RM_E,
    PRV_CPSS_DXCH_XCAT3_XLG_PORT_DROP_EVENT_PROBLEM_RM_E,
    PRV_CPSS_DXCH_XCAT3_RM_RX_SDMA_WRONG_QUEUE_WA_E,

    PRV_CPSS_DXCH_CAELUM_EGRESS_MEMORIES_NOT_RESET_WA_E,
    PRV_CPSS_DXCH_CAELUM_INGRESS_VLAN_RANGE_CONFIG_WA_E,

    /* the last one */
    PRV_CPSS_DXCH_ERRATA_MAX_NUM_E
}PRV_CPSS_DXCH_ERRATA_ENT;

/*
 * Typedef: struct PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC
 *
 * Description: Describes restricted address range
 *
 * Fields:
 *      addrStart  - Range start offset
 *      addrEnd    - Range end offset
 *      addrMask   - In some cases specific addresses are not valid and not
 *                   the whole range. If address & addrMask equals addrPattern
 *                   then this address is not valid.
 *      addrPattern - Pattern to check address with addrMask, if addrMask is
 *                    not 0 and there is match, then this address is not valid.
 */
typedef struct
{
    GT_U32      addrStart;
    GT_U32      addrEnd;
    GT_U32      addrMask;
    GT_U32      addrPattern;
} PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC;


/*
 * Typedef: struct PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC
 *
 * Description: Describes all ranges of particular unit
 *
 * Fields:
 *      numOfRanges  - Number of ranges
 *      rangesPtr    - Pointer to ranges
 */
typedef struct
{
    GT_U32                              numOfRanges;
    PRV_CPSS_DXCH_ERRATA_ADDR_RANGE_STC *rangesPtr;
} PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC
 *
 * Description: Simulates the clear-on-read MAC counters behavior
 *
 * Fields:
 *      stackPortArr  - Array of data for ports 24-27
 *      clearOnRead   - GT_TRUE - clear-on-read, GT_FALSE - read without clear
 *      macCntrShadow - shadow for counters.
 */
typedef struct
{
    struct
    {
        GT_BOOL       clearOnRead;
        GT_U64        macCntrShadow[CPSS_LAST_MAC_COUNTER_NUM_E];
    } stackPortArr[PRV_CPSS_DXCH_ERRATA_STACK_PORT_AMOUNT_CNS];
} PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC;

/*
 * Typedef: struct PRV_CPSS_DXCH_ERRATA_STC
 *
 * Description: A structure to hold all Errata info that CPSS need to fix
 *              (workaround) for the PP .
 *
 * Fields:
 *      FErArray - array of bmp of FEr errata
 *                 every erratum is represented by 1 bit (32 errata in a bmp)
 *
 *      srcIdAssignedModeConfRegData -
 *                                data of srcIdAssignedModeConfReg (0x02040070).
 *      bridgeAccessMatrixLine7RegData -
 *                  data of Bridge Access Matrix Line <7> register (0x0200061C).
 *
 *      info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E :
 *              info relevant to PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E
 *              pclInterruptCauseRegAddr - interrupt register for the ECC
 *                  interrupt to the PCL tcam
 *              pclInterruptIndex - the index of the interrupt in the enum of
 *                  the interrupts
 *              routerInterruptCauseRegAddr - interrupt register for the ECC
 *                  interrupt to the Router tcam
 *              routerInterruptIndex - the index of the interrupt in the enum of
 *                  the interrupts
 *      info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E:
 *              info relevant to PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChTrunkTrafficToCpuWaInit(...)
 *              extraTrunksInfoPtr - (pointer to) extra trunks info needed for the
 *                  WA. the pointer is allocated dynamically during
 *                  cpssDxChTrunkInit(...) only when required(WA needed for the
 *                  PP and application enabled the WA)
 *                  additionalInfoBmp - bitmap for additional WA info
 *      info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E:
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChHwPpImplementWaInit(...)
 *              devNum - device number of PP that connected to CPU with MII port
 *      info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E:
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChHwPpImplementWaInit(...)
 *      info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E:
 *              info relate to PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChHwPpImplementWaInit(...)
 *              filterEnabled - the state that the filter should be.
 *                      GT_TRUE - the filter enabled
 *                      GT_FALSE - the filter disabled
 *              NOTE: only when enabled == GT_TRUE and filterEnabled == GT_TRUE
 *                    the CPSS will filter the unneeded messages
 *      info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E -
 *              info relate to addresses checking and filtering the restricted ones.
 *              unitRangesPtr - pointer to unit ranges db
 *              lastValidAddress - last valid address
 *      info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E -
 *              info related to stack ports Mac counters shadowing.
 *              stackMacCntPtr - (pointer to) shadow of Mac Counters
 *                               and clear-on-read control bit
 *                               for ports 24-27.
 *      info_PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E -
 *              info related to GE ports of xcat/xcat2/lion disable WA
 *              portTxQueuesBmpPtr - (pointer to) per queue ports bitmap array of
 *                                   shadow of TX queue state defined by application
 *      info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E -
 *              info related to GE ports of xcat/xcat2/lion unidirect mode WA
 *              enabled - was it enabled by the application --> see function
 *                  cpssDxChHwPpImplementWaInit(...)
 *              pcsLoopbackBmpPtr - (pointer to) shadow of pcs loopback state
 *                                      defined by application
 *              portEnableBmpPtr - (pointer to) shadow of port enable state
 *                                      defined by application
 *
 *      info_PRV_CPSS_DXCH_LION2_IPM_BRIDGE_COPY_GET_DROPPED_WA_E -
 *              info related to IPM bridge copy get dropped when there is no
 *              member in the local hemisphere vlan WA.(FE-7263524)
 *              enabled - was it enabled by the application --> see function
 *                      cpssDxChHwPpImplementWaInit(...)
 *              tailDropUcEnable - tail drop enable status prior to WA initialization
 *              profileInfo - A structure to hold all PP data needed profiles
 *                      information
 *      info_PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E
 *              info related to Multicast Bandwidth restriction problem
 *              that can be solved when all VLAN members belong to one hemisphere
 *              excluding relay port from this VLAN members
 *              (FE-6041254).
 *              enabled - was it enabled by the application --> see function
 *                      cpssDxChHwPpImplementWaInit(...)
 *      info_PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E
 *              info related to egress STC table read under traffic problem.
 *              stcEgressLimitShadow - shadow array of values that loaded
 *                                     into count down counter.
 *
 *      info_PRV_CPSS_DXCH_LION2_CUT_THROUGH_SLOW_TO_FAST_WA_E
 *              info related to Cut Through from slow to fast port problem.
 *              enabled - was it enabled by the application --> see function
 *                      cpssDxChHwPpImplementWaInit(...)
 *
 *      info_PRV_CPSS_DXCH_BOBCAT2_EGRESS_MIB_COUNTERS_NOT_ROC_WA_E
 *              info related to the Egress MIB counters not being cleared on read
 *              egressCntrShadow - shadow array of counters value already read.
 *              histogramCntrShadow - shadow array of counters value already read.
 *
 *      info_PRV_CPSS_DXCH_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_WA_E
 *              info related to 30G WA (BC2 Rev A0 40G port does not pass trough TM
 *              shall get 30G at TxQ -- enable/disable field
 *              (see FE-8708889)
 *
 *      info_PRV_CPSS_DXCH_XCAT_ROUTER_TCAM_RM_WA_E
 *              info related to incorrect default value of bit 31 in the Routing TCAM
 *              Control register (RM#30230)
 * 
 *      info_CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E
 *              info related to FIFO thresholds adjustment for 1G and 10G ports in order to get precise PTP timestamp value under traffic.
 */
typedef struct{
    GT_U32          FErArray[PRV_CPSS_DXCH_ERRATA_BMP_MAX_NUM_CNS];
    GT_U32          srcIdAssignedModeConfRegData;
    GT_U32          bridgeAccessMatrixLine7RegData;

    struct{
        GT_U32      pclInterruptCauseRegAddr;
        GT_U32      pclInterruptIndex;

        GT_U32      routerInterruptCauseRegAddr;
        GT_U32      routerInterruptIndex;
    }info_PRV_CPSS_DXCH3_CPU_READ_TCAM_ECC_INTERRUPT_WA_E;

    struct{
        GT_BOOL     enabled;
        void*       extraTrunksInfoPtr;
        GT_U32      additionalInfoBmp;
    }info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E;

    struct{
        GT_BOOL     enabled;
        GT_U8       devNum;
    }info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E;

    struct{
        GT_BOOL     enabled;
    }info_PRV_CPSS_DXCH_XCAT_FDB_AU_FIFO_CORRUPT_WA_E;

    struct{
        GT_BOOL     enabled;
        GT_BOOL     filterEnabled;
    }info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E;

    struct
    {
        PRV_CPSS_DXCH_ERRATA_UNIT_RANGES_STC    *unitRangesPtr[CPSS_MAX_PORT_GROUPS_CNS];
        GT_U32                                  lastValidAddress;
    }info_PRV_CPSS_DXCH_XCAT_RESTRICTED_ADDRESS_FILTERING_WA_E;

    struct
    {
        PRV_CPSS_DXCH_ERRATA_STACK_MAC_COUNT_SHADOW_STC *stackMacCntPtr;
    }info_PRV_CPSS_DXCH_XCAT2_STACK_MAC_COUNT_NO_CLEAR_ON_READ_WA_E;

    struct{
        CPSS_PORTS_BMP_STC *portTxQueuesBmpPtr;
    }info_PRV_CPSS_DXCH_XCAT_GE_PORT_DISABLE_WA_E;

    struct{
        GT_BOOL enabled;
        CPSS_PORTS_BMP_STC *pcsLoopbackBmpPtr;
        CPSS_PORTS_BMP_STC *portEnableBmpPtr;
    }info_PRV_CPSS_DXCH_XCAT_GE_PORT_UNIDIRECT_WA_E;

    struct{
        GT_BOOL                                     enabled;
        GT_BOOL                                     tailDropUcEnable;
        PRV_CPSS_DXCH_PROFILE_INFO_STC              profileInfo[CPSS_DP_RANGE_CNS];
    }info_PRV_CPSS_DXCH_LION2_IPM_BRIDGE_COPY_GET_DROPPED_WA_E;

    struct{
        GT_BOOL     enabled;
    }info_PRV_CPSS_DXCH_LION2_MC_BANDWIDTH_RESTRICTION_WA_E;

    struct{
        GT_U32      stcEgressLimitShadow[CPSS_MAX_PORTS_NUM_CNS];
    }info_PRV_CPSS_DXCH_LION_EGRESS_STC_TABLE_READ_WA_E;

    struct{
        CPSS_PORTS_BMP_STC *portForceLinkDownBmpPtr;
    }info_PRV_CPSS_DXCH_LION2_DISMATCH_PORT_LINK_WA_E;

    struct{
        GT_BOOL     enabled;
    }info_PRV_CPSS_DXCH_LION2_CUT_THROUGH_SLOW_TO_FAST_WA_E;

    struct{
        CPSS_PORT_EGRESS_CNTR_STC  egressCntrShadow[2];
        GT_U32                     histogramCntrShadow[4];
    }info_PRV_CPSS_DXCH_BOBCAT2_EGRESS_MIB_COUNTERS_NOT_ROC_WA_E;

    struct {
        GT_BOOL  enabled;
    }info_PRV_CPSS_DXCH_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_WA_E;

    struct {
        GT_BOOL  enabled;
    }info_PRV_CPSS_DXCH_XCAT_ROUTER_TCAM_RM_WA_E;

    struct {
        GT_BOOL  enabled;
    }info_CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E;

}PRV_CPSS_DXCH_ERRATA_STC;


/* first trunk ID needed for the WA for "wrong trunk id source port information of packet to CPU" */
/*
    (28*2)=56 is first trunk , because 0..27 are valid ports
    but we need to align the trunk-Id to 5 bits:
    bits
trunk bits 6 5 4 3 2 1 0
           ---------------
value      |1|a|b|c|d|e|0|
           ---------------

           the abcde are the 5 bits for trunkId values from the application.

           NOTE: value of abcde == 0,31 are not allowed -->
            so possible values are: 1..30 --> 30 values -->

    so the 7 bit values are :
        66,68,70..122,124 (even values only)

    devices with 127 trunks supports also cascade trunks 126,127 (no traffic to CPU)
*/
#define XCAT_FIRST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU \
    56

/* last trunk ID needed for the WA for "wrong trunk id source port information of packet to CPU" */
/*
    (62*2) is the last trunk ,because 63 which is CPU port
*/
#define XCAT_LAST_TRUNKID_FOR_WA_WRONG_SOURCE_PORT_TO_CPU \
    (62*2)

/* macro to check that the WA for trunks requested by the application
   (even if PP not require), for "wrong trunk id source port information of
   packet to CPU" */
#define PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(_devNum)  \
    ((PRV_CPSS_DXCH_PP_MAC(_devNum)->errata.                                \
        info_PRV_CPSS_DXCH_XCAT_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E.        \
            enabled == GT_TRUE) ? 1 : 0)

/* macro to check that the WA for "packets sent from cpu by sdma corrupted"
   requested by the application (even if PP not require)
*/
#define PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(_devNum)  \
    ((PRV_CPSS_DXCH_PP_MAC(_devNum)->errata.                                \
        info_PRV_CPSS_DXCH_XCAT_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_E.        \
            enabled == GT_TRUE) ? 1 : 0)

/* macro to check that the WA for "AU/FU messages from non-source port group"
   requested by the application (even if PP not require)
*/
#define PRV_CPSS_FDB_AUQ_FROM_NON_SOURCE_PORT_GROUP_WA_E(_devNum)  \
    ((PRV_CPSS_DXCH_PP_MAC(_devNum)->errata.                       \
        info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E. \
            enabled == GT_TRUE) ? 1 : 0)

/* macro to check that the filter enable/disabled for
    "AU/FU messages from non-source port group" */
#define PRV_CPSS_FDB_AUQ_FROM_NON_SOURCE_PORT_GROUP_FILTER_ENABLED_WA_E(_devNum)  \
    ((PRV_CPSS_DXCH_PP_MAC(_devNum)->errata.                       \
        info_PRV_CPSS_DXCH_LION_FDB_AU_FU_MESSAGES_FROM_NON_SOURCE_PORT_GROUP_WA_E. \
            filterEnabled == GT_TRUE) ? 1 : 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChErrataMngh */


