/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssHwInit.h
*
* DESCRIPTION:
*       Includes CPSS basic Hw initialization functions, and data structures.
*
* FILE REVISION NUMBER:
*       $Revision: 38 $
*
*******************************************************************************/
#ifndef __cpssHwInith
#define __cpssHwInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>

#include <cpss/generic/extMac/cpssExtMacDrv.h>

/* Define the maximum number of RX queues */
#define CPSS_MAX_RX_QUEUE_CNS     8

/* Define the maximum number of TX queues */
#define CPSS_MAX_TX_QUEUE_CNS     8

/* Defines the max number SRAMs. The number includes CTRL SRAM (wide SRAM) and
   routing SRAM (narrow SRAM)*/
#define CPSS_MAX_NUMBER_OF_EXTERNAL_SRAMS_CNS 5

/* Defines the max number internal SRAMs */
#define CPSS_MAX_NUMBER_LPM_INTERNAL_SRAMS_CNS 4

/* constant used for field of :
    CPSS_DXCH_PP_PHASE1_INIT_INFO_STC:: intVecNum --> Nofity the cpss NOT capture
    the interrupt line (for this device) with it's ISR
*/
#define CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS                 0xFFFFFFFF

/*
 * typedef: enum CPSS_MAC_SA_LSB_MODE_ENT
 *
 * Description: Enumeration of MAC SA lsb modes ,
 *              this relate to "pause packets" and to "routed packets".
 *
 * Enumerations:
 *    CPSS_SA_LSB_PER_PORT_E    - Sets the MAC SA least significant byte
 *                                according to the per port setting.
 *                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Puma2; Puma3; ExMx.)
 *                              (NOT APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 *    CPSS_SA_LSB_PER_PKT_VID_E         - Sets the MAC SA least significant 12-bits
 *                                        according to the packet's VLAN ID.
 *    CPSS_SA_LSB_PER_VLAN_E            - Sets the MAC SA least significant byte
 *                                        according to the per vlan setting.
 *    CPSS_SA_LSB_FULL_48_BIT_GLOBAL    - Global MAC SA: Sets full 48-bit MAC SA
 *                                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3.)
 *
 */
typedef enum
{
    CPSS_SA_LSB_PER_PORT_E,
    CPSS_SA_LSB_PER_PKT_VID_E,
    CPSS_SA_LSB_PER_VLAN_E,
    CPSS_SA_LSB_FULL_48_BIT_GLOBAL
}CPSS_MAC_SA_LSB_MODE_ENT;


/*
 * Typedef enum CPSS_SYS_HA_MODE_ENT
 *
 * Description:
 *      define the High Availability mode of the CPU
 *
 * Enumerations:
 *    CPSS_SYS_HA_MODE_ACTIVE_E    - unit HA mode is active
 *    CPSS_SYS_HA_MODE_STANDBY_E   - unit HA mode is standby
 *
 */
typedef enum
{
    CPSS_SYS_HA_MODE_ACTIVE_E,
    CPSS_SYS_HA_MODE_STANDBY_E
}CPSS_SYS_HA_MODE_ENT;

/*
 * typedef: enum CPSS_PP_INTERFACE_CHANNEL_ENT
 *
 * Description: Packet Processor interface channel to the CPU
 *
 *
 * Enumerations:
 *    CPSS_CHANNEL_PCI_E       - channel to the CPU with PCI
 *    CPSS_CHANNEL_SMI_E       - channel to the CPU with SMI
 *    CPSS_CHANNEL_TWSI_E      - channel to the CPU with TWSI
 *    CPSS_CHANNEL_PEX_E       - channel to the CPU with PCI express (PEX)
 *    CPSS_CHANNEL_PEX_MBUS_E  - channel to the CPU with PCI express (PEX or MBUS)
 *                               with 8 Address Region Completion Mechanism
 *                               (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *    CPSS_CHANNEL_PEX_KERNEL_E         - kernel PEX driver
 *                             appAddrCompletionRegionsBmp must be set to one of
 *                             0x0f to use 4 regions addres completion
 *                                  (64M virtual addresses)
 *                             0x03 to use 2 regions addres completion
 *                                  (32M virtual addresses)
 *
 *
 *
 *   CPSS_CHANNEL_LAST_E       -  for internal use , must be last
 */
typedef enum
{
    CPSS_CHANNEL_PCI_E    = 0x0,
    CPSS_CHANNEL_SMI_E,
    CPSS_CHANNEL_TWSI_E,
    CPSS_CHANNEL_PEX_E,
    CPSS_CHANNEL_PEX_MBUS_E,

    CPSS_CHANNEL_PEX_KERNEL_E,
#ifdef DEVELOPER_NEW_DRIVERS
    CPSS_CHANNEL_PEX_NEWDRV_E,
    CPSS_CHANNEL_PEX_MBUS_NEWDRV_E,
    CPSS_CHANNEL_PEX_MBUS_KERNEL_E,
#endif

    CPSS_CHANNEL_LAST_E
}CPSS_PP_INTERFACE_CHANNEL_ENT;

#ifndef DEVELOPER_NEW_DRIVERS
#define CPSS_CHANNEL_IS_PCI_COMPATIBLE_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PCI_E || (_channel) == CPSS_CHANNEL_PEX_E \
        || (_channel) == CPSS_CHANNEL_PEX_MBUS_E \
        || (_channel) == CPSS_CHANNEL_PEX_KERNEL_E \
    )

#define CPSS_CHANNEL_IS_PCI_PEX_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PCI_E || (_channel) == CPSS_CHANNEL_PEX_E \
        || (_channel) == CPSS_CHANNEL_PEX_KERNEL_E \
    )

#define CPSS_CHANNEL_IS_PEX_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PEX_E \
        || (_channel) == CPSS_CHANNEL_PEX_KERNEL_E \
    )

#define CPSS_CHANNEL_IS_PEX_MBUS_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PEX_MBUS_E )
#else /* DEVELOPER_NEW_DRIVERS */
/* to be enabled later:
 *    CPSS_CHANNEL_PEX_NEWDRV_E         - new PEX driver
 *    CPSS_CHANNEL_PEX_MBUS_NEWDRV_E    - new PEX_MBUS driver
 *    CPSS_CHANNEL_PEX_MBUS_KERNEL_E    - kernel PEX MBUS driver
 */
#define CPSS_CHANNEL_IS_PCI_COMPATIBLE_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PCI_E || (_channel) == CPSS_CHANNEL_PEX_E \
        || (_channel) == CPSS_CHANNEL_PEX_MBUS_E \
        || (_channel) == CPSS_CHANNEL_PEX_NEWDRV_E \
        || (_channel) == CPSS_CHANNEL_PEX_MBUS_NEWDRV_E \
        || (_channel) == CPSS_CHANNEL_PEX_KERNEL_E \
        || (_channel) == CPSS_CHANNEL_PEX_MBUS_KERNEL_E \
    )

#define CPSS_CHANNEL_IS_PCI_PEX_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PCI_E || (_channel) == CPSS_CHANNEL_PEX_E \
        || (_channel) == CPSS_CHANNEL_PEX_NEWDRV_E \
        || (_channel) == CPSS_CHANNEL_PEX_KERNEL_E \
    )

#define CPSS_CHANNEL_IS_PEX_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PEX_E \
        || (_channel) == CPSS_CHANNEL_PEX_NEWDRV_E \
        || (_channel) == CPSS_CHANNEL_PEX_KERNEL_E \
    )

#define CPSS_CHANNEL_IS_PEX_MBUS_MAC(_channel) \
    ((_channel) == CPSS_CHANNEL_PEX_MBUS_E \
        || (_channel) == CPSS_CHANNEL_PEX_MBUS_NEWDRV_E \
        || (_channel) == CPSS_CHANNEL_PEX_MBUS_KERNEL_E \
    )
#endif /* DEVELOPER_NEW_DRIVERS */


/*
 * typedef: enum CPSS_SRAM_SIZE_ENT
 *
 * Description: Defines the external SRAM possible sizes.
 *
 * Enumerations:
 *     CPSS_SRAM_SIZE_NOT_USED_E  - SRAM is not connected
 *     CPSS_SRAM_SIZE_12KB_E      - SRAM size is 12 KByte
 *     CPSS_SRAM_SIZE_32KB_E      - SRAM size is 32 KByte
 *     CPSS_SRAM_SIZE_64KB_E      - SRAM size is 64 KByte
 *     CPSS_SRAM_SIZE_128KB_E     - SRAM size is 128 KByte
 *     CPSS_SRAM_SIZE_256KB_E     - SRAM size is 256 KByte
 *     CPSS_SRAM_SIZE_512KB_E     - SRAM size is 512 KByte
 *     CPSS_SRAM_SIZE_1MB_E       - SRAM size is 1 Mbyte
 *     CPSS_SRAM_SIZE_2MB_E       - SRAM size is 2 Mbyte
 *     CPSS_SRAM_SIZE_4MB_E       - SRAM size is 4 Mbyte
 *     CPSS_SRAM_SIZE_8MB_E       - SRAM size is 8 Mbyte
 *     CPSS_SRAM_SIZE_16MB_E      - SRAM size is 16 Mbyte
 *     CPSS_SRAM_SIZE_32MB_E      - SRAM size is 32 Mbyte
 *     CPSS_SRAM_SIZE_64MB_E      - SRAM size is 64 Mbyte
 *     CPSS_SRAM_SIZE_128MB_E     - SRAM size is 128 Mbyte
 *     CPSS_SRAM_SIZE_256MB_E     - SRAM size is 256 Mbyte
 */
typedef enum
{
    CPSS_SRAM_SIZE_NOT_USED_E     = 0x0,
    CPSS_SRAM_SIZE_12KB_E         = 12 * _1KB,
    CPSS_SRAM_SIZE_32KB_E         = _32KB,
    CPSS_SRAM_SIZE_64KB_E         = _64KB,
    CPSS_SRAM_SIZE_128KB_E        = _128KB,
    CPSS_SRAM_SIZE_256KB_E        = _256KB,
    CPSS_SRAM_SIZE_512KB_E        = _512KB,
    CPSS_SRAM_SIZE_1MB_E          = _1MB,
    CPSS_SRAM_SIZE_2MB_E          = _2MB,
    CPSS_SRAM_SIZE_4MB_E          = _4MB,
    CPSS_SRAM_SIZE_8MB_E          = _8MB,
    CPSS_SRAM_SIZE_16MB_E         = _16MB,
    CPSS_SRAM_SIZE_32MB_E         = _32MB,
    CPSS_SRAM_SIZE_64MB_E         = _64MB,
    CPSS_SRAM_SIZE_128MB_E        = _128MB,
    CPSS_SRAM_SIZE_256MB_E        = _256MB
}CPSS_SRAM_SIZE_ENT;

/*
 * typedef: enum CPSS_SRAM_RATE_ENT
 *
 * Description: Defines the rate of the external SRAM.
 *
 * Enumerations:
 *      CPSS_SRAM_RATE_DDR_E  - Double data rate
 *      CPSS_SRAM_RATE_QDR_E  - Quadric data rate
 *
 */
typedef enum
{
    CPSS_SRAM_RATE_DDR_E =0,
    CPSS_SRAM_RATE_QDR_E =1
}CPSS_SRAM_RATE_ENT;

/*
 * typedef: enum CPSS_SRAM_TYPE_ENT
 *
 * Description: Defines the type of the external SRAM (narrow or wide).
 *
 * Enumerations:
 *      CPSS_SRAM_TYPE_NARROW_E    - Narrow SRAM
 *      CPSS_SRAM_TYPE_WIDE_E      - Wide SRAM
 */
typedef enum
{
    CPSS_SRAM_TYPE_NARROW_E,
    CPSS_SRAM_TYPE_WIDE_E
}CPSS_SRAM_TYPE_ENT;

/*
 * typedef: enum CPSS_EXTERNAL_MEMORY_ENT
 *
 * Description: Defines which external memory is it.
 *
 * Enumerations:
 *      CPSS_EXTERNAL_MEMORY_0_E - external memory 0
 *      CPSS_EXTERNAL_MEMORY_1_E - external memory 1
 *      CPSS_EXTERNAL_MEMORY_2_E - external memory 2
 *      CPSS_EXTERNAL_MEMORY_3_E - external memory 3
 *      CPSS_EXTERNAL_MEMORY_INVALID_E - Not connected
 */
typedef enum
{
    CPSS_EXTERNAL_MEMORY_0_E = 0, /* refers to external memory 0 */
    CPSS_EXTERNAL_MEMORY_1_E = 1, /* refers to external memory 1 */
    CPSS_EXTERNAL_MEMORY_2_E = 2, /* refers to external memory 2 */
    CPSS_EXTERNAL_MEMORY_3_E = 3, /* refers to external memory 3 */
    CPSS_EXTERNAL_MEMORY_INVALID_E = 0x7FFFFFFF
} CPSS_EXTERNAL_MEMORY_ENT;


/*
 * typedef: enum CPSS_NARROW_SRAM_CFG_ENT
 *
 * Description: Defines the memory configuration of the narrow SRAM.
 *
 * Enumerations:
 * CPSS_TWIST_INTERNAL_E      - Internal memory for TWIST type asic - backward
 *                            compatibility
 * CPSS_TWIST_EXTERNAL_E      - Only external memory for TWIST type asic -
 *                            backward compatibility
 * CPSS_TWO_EXTERNAL_250MHZ_E - Two external memories that work in 250 MHz and
 *                            therefore in order to support wire speed the
 *                            internal memories are used as well.
 * CPSS_TWO_EXTERNAL_300MHZ_E - Two external narrow srams each working in 300MHz
 *                            It supports IPv4 and IPv6 routing in full wire
 *                            speed.
 * CPSS_DROP_IN_CAPACITY_E    - Support taking the Tiger as a drop in to TwistD
 *                            boards. With emphasis on capacity (more IPv4 mc
 *                            groups, less wire speed)
 * CPSS_DROP_IN_SPEED_E       - Support taking the Tiger as a drop in to TwistD
 *                            boards. With emphasis on speed (more wire speed,
 *                            less IPv4 mc groups)
 * CPSS_TWO_INTERNAL_E        - Two internal narrow srams 32Kbyte each working
 *                            300 MHz. It supports IPv4 and IPv6 routing in full
 *                            wire speed.
 * CPSS_ONE_INTERNAL_E        - One 64 Kbyte internal narrow sram for IPv4 only
 *                            backward compatible mode.
 * CPSS_NRAM_NOT_EXISTS_E     - No nram configuration
 */
typedef enum
{
    CPSS_TWIST_INTERNAL_E,
    CPSS_TWIST_EXTERNAL_E,
    CPSS_TWO_EXTERNAL_250MHZ_E,
    CPSS_TWO_EXTERNAL_300MHZ_E,
    CPSS_DROP_IN_CAPACITY_E,
    CPSS_DROP_IN_SPEED_E,
    CPSS_TWO_INTERNAL_E,
    CPSS_ONE_INTERNAL_E,
    CPSS_NRAM_NOT_EXISTS_E
} CPSS_NARROW_SRAM_CFG_ENT;


/*
 * Typedef: enum CPSS_SRAM_FREQUENCY_ENT
 *
 * Description: Defines the different Sram frequencies that an sram can have.
 *
 * Fields:
 *  CPSS_SRAM_FREQ_125_MHZ_E    - 125 MHz sram frequency.
 *  CPSS_SRAM_FREQ_166_MHZ_E    - 166 MHz sram frequency.
 *  CPSS_SRAM_FREQ_200_MHZ_E    - 200 MHz sram frequency.
 *  CPSS_SRAM_FREQ_250_MHZ_E    - 250 MHz sram frequency.
 *  CPSS_SRAM_FREQ_300_MHZ_E    - 300 MHz sram frequency.
 *
 */
typedef enum
{
    CPSS_SRAM_FREQ_125_MHZ_E = 125,
    CPSS_SRAM_FREQ_166_MHZ_E = 166,
    CPSS_SRAM_FREQ_200_MHZ_E = 200,
    CPSS_SRAM_FREQ_250_MHZ_E = 250,
    CPSS_SRAM_FREQ_300_MHZ_E = 300
}CPSS_SRAM_FREQUENCY_ENT;



/*
 * Typedef: struct CPSS_SRAM_CFG_STC
 *
 * Description: Includes external Sram configuration values
 *              This struct is used for External Wide & Narrow Srams.
 *
 * Fields:
 *      sramSize        - The external Sram size.
 *      sramFrequency   - Holds the Sram clock frequency.
 *      sramRate        - This field indicates the rate of external memory.
 *      externalMemory  - Indicates to which external memory the memory
 *                        parameters are related.
 *      sramType        - Determines if it's narrow SRAM or wide SRAM.
 *
 */
typedef struct
{
    CPSS_SRAM_SIZE_ENT        sramSize;
    CPSS_SRAM_FREQUENCY_ENT   sramFrequency;
    CPSS_SRAM_RATE_ENT        sramRate;
    CPSS_EXTERNAL_MEMORY_ENT  externalMemory;
    CPSS_SRAM_TYPE_ENT        sramType;
}CPSS_SRAM_CFG_STC;



/*
 * Typedef: struct CPSS_BUFF_MGMT_CFG_STC
 *
 * Description: Includes common buffer management configuration values, to be
 *              set to Pp's registers at initialization.
 *
 * Fields:
 *      overwriteDef     - when required, indicates to overwrite the PP's default
 *                         values.
 *                             GT_TRUE  - overwrite,
 *                             GT_FALSE - ignore the rest of this struct's data.
 *      numPpBuffers     - The number of buffers allocated in the PP Buffer
 *                         DRAM. This value must not exceed the maximal allowed
 *                         number of buffers (depending on the device type).
 *      numUplinkBuffers - The total number of buffers used by the uplink
 *                         device
 *
 * Comments:
 *      - Refer to the data-sheet for complete description of these parameters.
 *      - Setting an undefined values, like reserved values, may result in
 *        unpredictable results.
 *
 */
typedef struct
{
    GT_BOOL         overwriteDef;
    GT_U16          numPpBuffers;
    GT_U16          numUplinkBuffers;
}CPSS_BUFF_MGMT_CFG_STC;



/*
 * Typedef: enum CPSS_RX_BUFF_ALLOC_METHOD_ENT
 *
 * Description: Defines the different allocation methods for the Rx / Tx
 *              descriptors, and Rx buffers.
 *
 * Fields:
 *     CPSS_RX_BUFF_DYNAMIC_ALLOC_E- The user provides a function pointer which
 *                        dynamically allocates a single Rx buffer.
 *     CPSS_RX_BUFF_STATIC_ALLOC_E- The user provides a block of memory in which
 *                        the driver then partitions into Rx buffers.
 *     CPSS_RX_BUFF_NO_ALLOC_E - CPSS doesn't manage the RX buffers allocation;
 *                        application must provide buffers for RX queues.
 *                        Supported in SDMA CPU port mode and for DXCH devices only.
 *
 */
typedef enum
{
    CPSS_RX_BUFF_DYNAMIC_ALLOC_E = 0,
    CPSS_RX_BUFF_STATIC_ALLOC_E     ,
    CPSS_RX_BUFF_NO_ALLOC_E
}CPSS_RX_BUFF_ALLOC_METHOD_ENT;

/*
 * Typedef: enum CPSS_TX_BUFF_ALLOC_METHOD_ENT
 *
 * Description: Defines the different allocation methods for the Tx
 *              descriptors and Tx buffers.
 *
 * Fields:
 *     CPSS_TX_BUFF_DYNAMIC_ALLOC_E- The user provides a function pointer which
 *                        dynamically allocates descriptors & buffers memory.
 *     CPSS_TX_BUFF_STATIC_ALLOC_E- The user provides a block of memory in which
 *                        the driver then partitions into Tx descriptors &
 *                        buffers.
 *
 */
typedef enum
{
    CPSS_TX_BUFF_DYNAMIC_ALLOC_E = 0,
    CPSS_TX_BUFF_STATIC_ALLOC_E
}CPSS_TX_BUFF_ALLOC_METHOD_ENT;


/*
 * Typedef: enum CPSS_TX_SDMA_QUEUE_MODE_ENT
 *
 * Description: Defines the different working mode of Tx SDMA queue.
 *
 * Fields:
 *     CPSS_TX_SDMA_QUEUE_MODE_NORMAL_E - SDMA updates the descriptor ownership
 *                                        bit to mark CPU ownership.
 *     CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E - SDMA will not return the
 *                                           descriptor ownership to the CPU.
 */
typedef enum
{
    CPSS_TX_SDMA_QUEUE_MODE_NORMAL_E,
    CPSS_TX_SDMA_QUEUE_MODE_PACKET_GENERATOR_E
}CPSS_TX_SDMA_QUEUE_MODE_ENT;

/*
 * Typedef: CPSS_TX_BUFF_MALLOC_FUNC
 *
 * Description: Prototype of the function pointer to be provided by the
 *              user to perform Rx descriptors & buffers allocation.
 *              The CPSS invokes this user function when allocating
 *              Rx buffers. This is relevant only if  the DYNAMIC_ALLOC
 *              allocation method is used.
 *
 * Fields:
 *      size   - the size  to allocate.
 *      align - The required byte-alignment for the allocation.
 *
 */
typedef GT_U8* (*CPSS_TX_BUFF_MALLOC_FUNC)
(
    IN GT_U32 size,
    IN GT_U32 align
);

/*
 * Typedef: CPSS_RX_BUFF_MALLOC_FUNC
 *
 * Description: Prototype of the function pointer to be provided by the
 *              user to perform Rx buffer allocation.
 *              The CPSS invokes this user function when allocating
 *              Rx buffers. This is relevant only if  the DYNAMIC_ALLOC
 *              allocation method is used.
 *
 * Fields:
 *      size   - the size of the Rx buffer to allocate.  This value reflects
 *               the Rx buffer size that the user defines in the
 *               TAPI gtSysConfig.h structure GT_SYS_CONFIG
 *      align - The required byte-alignment for the Rx buffer
 *
 */
typedef GT_U8* (*CPSS_RX_BUFF_MALLOC_FUNC)
(
    IN GT_U32 size,
    IN GT_U32 align
);


/*
 * Typedef: CPSS_RX_BUF_INFO_STC
 *
 * Description: Rx buffer Information
 *
 * Fields:
 *      allocMethod     - There are choices for providing Rx Buffers to the CPSS:
 *                          1) The user passes a block of memory which the
 *                             CPSS will internally partition into Rx Buffers.
 *                          2) The user passes a function pointer to a
 *                             user supplied Rx Buffer allocator
 *      bufferPercentage   - A table (entry per queue) that describes the buffer
 *                            dispersal among all Rx queues. (values 0..100)
 *                            NOTE: The accumulation of all queues should not
 *                                  be more than 100% !
 *      rxBufSize           - The size of the SDMA Rx data buffer. If the Data
 *                            buffer size is smaller than received packet
 *                            size, the packet is "chained" over multiple
 *                            buffers. This value is used by the CPSS when
 *                            initializing the RX descriptor rings.
 *      headerOffset        - The number of bytes before the start of the Rx
 *                            buffer to reserve for the application use.
 *                            This value is used by the CPSS when
 *                            initializing the RX descriptor rings.
 *                            NOTE: This parameter must be synchronized among
 *                            all system CPUs.
 *      buffersInCachedMem  - GT_TRUE if buffers allocated in cached CPU memory,
 *                            GT_FALSE otherwise. Used for DX only - ignored for all other.
 *                            When using cached RX buffers on Linux/FreeBSD please take
 *                            in mind high impact on system performance due to IOCTL from
 *                            user to kernel space during the invalidate action.
 *
 *      buffData Union :
 *      - staticAlloc:(relevant when CPSS_RX_BUFF_STATIC_ALLOC_E)
 *          rxBufBlockPtr  - (Pointer to) a block of memory to be used for
 *                            allocating Rx packet data buffers.
 *          rxBufBlockSize  - The raw size in byte of rxBufBlock.
 *
 *      - dynamicAlloc:(relevant when CPSS_RX_BUFF_DYNAMIC_ALLOC_E)
 *          mallocFunc      - Function pointer to be invoked for
 *                            allocating buffers.
 *          numOfRxBuffers  - The number of Buffers that need to be allocated.
 *                            This field is used by Eth port NetworkInterface.
 *
 * Comments:
 *      If DYNAMIC_ALLOC method is used, then the function provided by user
 *      should have the following prototype defined by CPSS_RX_BUFF_MALLOC_FUNC
 *
 */
typedef struct
{
    CPSS_RX_BUFF_ALLOC_METHOD_ENT   allocMethod;
    GT_U32                  bufferPercentage[CPSS_MAX_RX_QUEUE_CNS];
    GT_U32                  rxBufSize;
    GT_U32                  headerOffset;
    GT_BOOL                 buffersInCachedMem;

    union
    {
        struct
        {
            GT_U32  *rxBufBlockPtr;
            GT_U32  rxBufBlockSize;
        }staticAlloc;

        struct
        {
            CPSS_RX_BUFF_MALLOC_FUNC    mallocFunc;
            GT_U32                      numOfRxBuffers;
        }dynamicAlloc;
    }buffData;
}CPSS_RX_BUF_INFO_STC;


/*
 * typedef: enum CPSS_BUFFER_DRAM_PARAM_ENT
 *
 * Description: Type of buffer DRAM.
 *
 * Enumerations:
 *     CPSS_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E        -
 *              Use default DRAM configuration.
 *     CPSS_BUFFER_DRAM_SAMSUNG_REV_A_250MHZ_E         -
 *               SAMSUNG A-Ver (K4D28163HD-TC40)  250 Mhz
 *     CPSS_BUFFER_DRAM_SAMSUNG_REV_E_250MHZ_E         -
 *               SAMSUNG E-Ver (K4D261638E-TC40)  250 Mhz
 *     CPSS_BUFFER_DRAM_HYNIX_HY5DU283222F_33_250MHZ_E -
 *               Hynix HY5DU283222F-33  250 Mhz
 *     CPSS_BUFFER_DRAM_SAMSUNG_REV_A_300MHZ_E         -
 *               SAMSUNG A-Ver (K4D28163HD-TC40)  300 Mhz
 *     CPSS_BUFFER_DRAM_SAMSUNG_REV_E_300MHZ_E         -
 *               SAMSUNG E-Ver (K4D261638E-TC40)  300 Mhz
 *
 */
typedef enum
{
    CPSS_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E = 0,
    CPSS_BUFFER_DRAM_SAMSUNG_REV_A_250MHZ_E = 1,
    CPSS_BUFFER_DRAM_SAMSUNG_REV_E_250MHZ_E,
    CPSS_BUFFER_DRAM_HYNIX_HY5DU283222F_33_250MHZ_E,
    CPSS_BUFFER_DRAM_SAMSUNG_REV_A_300MHZ_E,
    CPSS_BUFFER_DRAM_SAMSUNG_REV_E_300MHZ_E
}CPSS_BUFFER_DRAM_PARAM_ENT;

/*
 * typedef: enum CPSS_FLOW_DRAM_PARAM_ENT
 *
 * Description: Type of flow DRAM.
 *
 * Enumerations:
 *
 * CPSS_FLOW_DRAM_USE_DEFAULT_DRAM_CONF_E - Use default DRAM configuration.
 * CPSS_FLOW_DRAM_SAMSUNG_REV_A_250MHZ_E -  SAMSUNG A-Ver (K4D28163HD-TC40)  250 Mhz
 * CPSS_FLOW_DRAM_SAMSUNG_REV_E_250MHZ_E -  SAMSUNG E-Ver (K4D261638E-TC40)  250 Mhz
 * CPSS_FLOW_DRAM_SAMSUNG_REV_A_166MHZ_E -  SAMSUNG A-Ver (K4D28163HD-TC40)  166 Mhz
 * CPSS_FLOW_DRAM_SAMSUNG_REV_E_166MHZ_E -  SAMSUNG E-Ver (K4D261638E-TC40)  166 Mhz
 * CPSS_FLOW_DRAM_SAMSUNG_REV_A_200MHZ_E -  SAMSUNG A-Ver (K4D28163HD-TC40)  200 Mhz
 * CPSS_FLOW_DRAM_SAMSUNG_REV_E_200MHZ_E -  SAMSUNG E-Ver (K4D261638E-TC40)  200 Mhz
 */
typedef enum
{
    CPSS_FLOW_DRAM_USE_DEFAULT_DRAM_CONF_E = 0,
    CPSS_FLOW_DRAM_SAMSUNG_REV_A_250MHZ_E = 1,
    CPSS_FLOW_DRAM_SAMSUNG_REV_E_250MHZ_E,
    CPSS_FLOW_DRAM_SAMSUNG_REV_A_166MHZ_E,
    CPSS_FLOW_DRAM_SAMSUNG_REV_E_166MHZ_E,
    CPSS_FLOW_DRAM_SAMSUNG_REV_A_200MHZ_E,
    CPSS_FLOW_DRAM_SAMSUNG_REV_E_200MHZ_E
}CPSS_FLOW_DRAM_PARAM_ENT;


/* Delimiter for registers list regs */
#define CPSS_REG_LIST_DELIMITER_CNS         0xFFFFFFFF

/* Delimiter for phy addr regs */
#define CPSS_REG_PHY_ADDR_TYPE_CNS          0xEEEEEEEE

/*
 * Typedef: struct CPSS_REG_VALUE_INFO_STC
 *
 * Description: Holds a specific register value to be used for configuring a
 *              device's register in system.
 *              This structure is a part of a values' list to be
 *              written into the device in the initialization sequence.
 *
 * Fields:
 *      addrType    - Holds the register address to write to.
 *      regMask     - Holds the valid bits out of regVal to be written to the
 *                    register, value of 0xFFFFFFFF will write to the whole
 *                    register.
 *      regVal      - Holds the register value to write (after a bit wise & with
 *                    regMask).
 *      repeatCount - Number of repeatitions this register writes. (Useful
 *                    for DFCDL's values configuration).
 *
 */
typedef struct
{
    GT_U32  addrType;
    GT_U32  regMask;
    GT_U32  regVal;
    GT_U32  repeatCount;
}CPSS_REG_VALUE_INFO_STC;


/*
 * typedef: struct CPSS_NET_IF_CFG_STC
 *
 * Description: Network interface configuration parameters
 *
 * Fields:
 *    txDescBlock      - Pointer to a block of host memory to be used
 *                       for allocating Tx packet descriptor structures.
 *    txDescBlockSize  - The raw size in bytes of txDescBlock memory.
 *    rxDescBlock      - Pointer to a block memory to be used for
 *                       allocating Rx description structures.(Rx descriptor
 *                       structures to cpu)
 *    rxDescBlockSize  - The raw size in byte of rxDescBlock.
 *    rxBufInfo        - Rx buffers allocation information.
 */
typedef struct
{
    GT_U32  *txDescBlock;
    GT_U32   txDescBlockSize;
    GT_U32  *rxDescBlock;
    GT_U32   rxDescBlockSize;
    CPSS_RX_BUF_INFO_STC rxBufInfo;
}CPSS_NET_IF_CFG_STC;


/*
 * typedef: struct CPSS_MULTI_NET_IF_RX_SDMA_QUEUE_STC
 *
 * Description: Network Multi-Group interface configuration parameters for
 *              Rx SDMA.
 *
 * Fields:
 *     buffAndDescAllocMethod     - Memory allocation method for buffers
 *                                  and descriptors: static or dynamic.
 *    numOfRxDesc      - Number of descriptors to allocate.
 *    numOfRxBuff      - Number of buffers to allocate.
 *    buffSize         - The size of the SDMA Rx data buffer. If the Data
 *                       buffer size is smaller than received packet size,
 *                       the packet is "chained" over multiple buffers.
 *                       This value is used by the CPSS when initializing the
 *                       RX descriptor rings.
 *   buffHeaderOffset  - The number of bytes before the start of the Rx buffer
 *                       to reserve for the application use. This value is used
 *                       by the CPSS when initializing the RX descriptor rings.
 *                       NOTE: This parameter must be synchronized among all
 *                       system CPUs.
 *    buffersInCachedMem  - GT_TRUE if buffers allocated in cached CPU memory,
 *                          GT_FALSE otherwise.
 *    memData Union:
 *          Memory allocation definitions.
 *          staticAlloc -
 *              buffAndDescMemPtr - (pointer to) memory block static allocation.
 *              buffAndDescMemSize - size of allocated block.
 *          dynamicAlloc -
 *                buffAndDescMallocFunc - function pointer for memory allocation.
 *
 */
typedef struct
{
    CPSS_RX_BUFF_ALLOC_METHOD_ENT   buffAndDescAllocMethod;
    GT_U32                          numOfRxDesc;
    GT_U32                          numOfRxBuff;
    GT_U32                          buffSize;
    GT_U32                          buffHeaderOffset;
    GT_BOOL                         buffersInCachedMem;

    union
    {
        struct
            {
            GT_U32  *buffAndDescMemPtr;
            GT_U32   buffAndDescMemSize;
        } staticAlloc;

        struct
        {
            CPSS_RX_BUFF_MALLOC_FUNC buffAndDescMallocFunc;
        } dynamicAlloc;
   } memData;
} CPSS_MULTI_NET_IF_RX_SDMA_QUEUE_STC;


/*
 * typedef: struct CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC
 *
 * Description: Network Multi-Group interface configuration parameters for
 *              Tx SDMA.
 *
 * Fields:
 *     queueMode       - Queue working mode.
 *     buffAndDescAllocMethod     - Memory allocation method for buffers
 *                                  and descriptors: static or dynamic.
 *    numOfTxDesc      - Number of descriptors to allocate.
 *    numOfTxBuff      - Number of buffers to allocate.
 *                       Relevant only to queue working as generator.
 *    buffSize         - Allocated buffer size (internal packet additions,
 *                       e.g. DSA tag, should be taken into consideration when
 *                       defining this length).
 *                       Relevant only to queue working as generator.
 *    buffersInCachedMem  - GT_TRUE if buffers allocated in cached CPU memory,
 *                          GT_FALSE otherwise.
 *    memData Union:
 *          Memory allocation definitions. Descriptors related. For queue
 *          working as generator also relevant for buffers.
 *          staticAlloc -
 *              buffAndDescMemPtr - (pointer to) memory block static allocation.
 *              buffAndDescMemSize - size of allocated block.
 *          dynamicAlloc -
 *                buffAndDescMallocFunc - function pointer for memory allocation.
 *
 */
typedef struct
{
    CPSS_TX_SDMA_QUEUE_MODE_ENT     queueMode;
    CPSS_TX_BUFF_ALLOC_METHOD_ENT   buffAndDescAllocMethod;
    GT_U32                          numOfTxDesc;
    GT_U32                          numOfTxBuff;
    GT_U32                          buffSize;
    GT_BOOL                         buffersInCachedMem;

    union
    {
        struct
            {
            GT_U32  *buffAndDescMemPtr;
            GT_U32   buffAndDescMemSize;
        } staticAlloc;

        struct
        {
            CPSS_TX_BUFF_MALLOC_FUNC buffAndDescMallocFunc;
        } dynamicAlloc;
     }memData;
} CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC;


/*
 * typedef: struct CPSS_MULTI_NET_IF_CFG_STC
 *
 * Description: Multi-Group Network interface configuration parameters.
 *
 * Fields:
 *    rxSdmaQueuesConfig - Rx queues related configurations.
 *    txSdmaQueuesConfig - Tx queues related configurations.
 */
typedef struct
{
    CPSS_MULTI_NET_IF_RX_SDMA_QUEUE_STC
            rxSdmaQueuesConfig[CPSS_MAX_PORT_GROUPS_CNS][CPSS_MAX_RX_QUEUE_CNS];
    CPSS_MULTI_NET_IF_TX_SDMA_QUEUE_STC
            txSdmaQueuesConfig[CPSS_MAX_PORT_GROUPS_CNS][CPSS_MAX_TX_QUEUE_CNS];
}CPSS_MULTI_NET_IF_CFG_STC;


/*
 * typedef: struct CPSS_AUQ_CFG_STC
 *
 * Description:  Address Update Queue configuration parameters
 *
 * Fields:
 *    auDescBlock      - The block of memory used for the Address Update Queue.
 *                       The packet processor writes AU messages to this queue.
 *    auDescBlockSize  - Size of auDescBlock (in Bytes).
 */
typedef struct
{
    GT_U8   *auDescBlock;
    GT_U32  auDescBlockSize;
}CPSS_AUQ_CFG_STC;

/*
 * typedef: enum CPSS_PP_UPLINK_CONFIG_ENT
 *
 * Description: Enumeration of the different uplink devices.
 *
 * Enumerations:
 *      CPSS_PP_UPLINK_BACK_TO_BACK_E   - Uplink connected to another PP.
 *      CPSS_PP_UPLINK_FA_E             - Uplink connected to FA device.
 *      CPSS_PP_NO_UPLINK_E             - The Uplink isn't connected.
 */
typedef enum
{
    CPSS_PP_UPLINK_BACK_TO_BACK_E = 0,
    CPSS_PP_UPLINK_FA_E ,
    CPSS_PP_NO_UPLINK_E
}CPSS_PP_UPLINK_CONFIG_ENT;


/*
 * typedef: enum CPSS_AU_MESSAGE_LENGTH_ENT
 *
 * Description: packet buffer sizes
 *
 * Enumerations:
 *    CPSS_AU_MESSAGE_LENGTH_4_WORDS_E  - AU message is 16 byte length
 *    CPSS_AU_MESSAGE_LENGTH_8_WORDS_E  - AU message is 32 byte length
 */
typedef enum
{
    CPSS_AU_MESSAGE_LENGTH_4_WORDS_E  = 0,
    CPSS_AU_MESSAGE_LENGTH_8_WORDS_E
}CPSS_AU_MESSAGE_LENGTH_ENT;

/*
 * typedef: enum CPSS_BUF_MODE_ENT
 *
 * Description: packet buffer sizes
 *
 * Enumerations:
 *    CPSS_BUF_1536_E - 1536 byte buffers
 *    CPSS_BUF_2K_E   - 2K byte  buffers
 *    CPSS_BUF_10K_E  - 10K byte buffers
 */
typedef enum
{
    CPSS_BUF_1536_E  = 0,
    CPSS_BUF_2K_E,
    CPSS_BUF_10K_E
}CPSS_BUF_MODE_ENT;

/*
 * typedef: enum CPSS_BANK_MODE_ENT
 *
 * Description: packet bank modes
 *
 * Enumerations:
 *    CPSS_NO_SMART_BANK_E       - no smart bank
 *    CPSS_PARTIAL_SMART_BANK_E  - partial smart bank
 *    CPSS_REGULAR_SMART_BANK_E  - regular smart bank
 */
typedef enum
{
    CPSS_NO_SMART_BANK_E = 0,
    CPSS_PARTIAL_SMART_BANK_E,
    CPSS_REGULAR_SMART_BANK_E
}CPSS_BANK_MODE_ENT;


/*
 * typedef: enum CPSS_DRAM_SIZE_ENT
 *
 * Description: Packet Processor DRAM sizes
 *
 * Enumerations:
 *    CPSS_DRAM_NOT_USED_E   - External DRAM is not connected.
 *    CPSS_DRAM_64KB_E       - DRAM size is 64KByte.
 *    CPSS_DRAM_128KB_E      - DRAM size is 128KByte.
 *    CPSS_DRAM_8MB_E        - DRAM size is 8 MByte.
 *    CPSS_DRAM_16MB_E       - DRAM size is 16 MByte.
 *    CPSS_DRAM_32MB_E       - DRAM size is 32 MByte.
 *    CPSS_DRAM_64MB_E       - DRAM size is 64 MByte.
 *    CPSS_DRAM_128MB_E      - DRAM size is 128 MByte.
 *    CPSS_DRAM_256MB_E      - DRAM size is 256 MByte.
 *    CPSS_DRAM_512MB_E      - DRAM size is 512 MByte.
 *    CPSS_DRAM_1GB_E        - DRAM size is 1 GByte.
 *
 * Notes: Some Prestera devices do not have external memory interfaces
 */
typedef enum
{
    CPSS_DRAM_NOT_USED_E   = 0x0,
    CPSS_DRAM_64KB_E       = _64KB,
    CPSS_DRAM_128KB_E      = _128KB,
    CPSS_DRAM_8MB_E        = _8MB,
    CPSS_DRAM_16MB_E       = _16MB,
    CPSS_DRAM_32MB_E       = _32MB,
    CPSS_DRAM_64MB_E       = _64MB,
    CPSS_DRAM_128MB_E      = _128MB,
    CPSS_DRAM_256MB_E      = _256MB,
    CPSS_DRAM_512MB_E      = _512MB,
    CPSS_DRAM_1GB_E        = _1GB
}CPSS_DRAM_SIZE_ENT;



/*
 * Typedef: enum CPSS_DRAM_FREQUENCY_ENT
 *
 * Description: Defines the different DRAM frequencies that a DRAM can have.
 *
 * Enumerations:
 *  CPSS_DRAM_FREQ_100_MHZ_E    - 100 MHz dram frequency.
 *  CPSS_DRAM_FREQ_125_MHZ_E    - 125 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_166_MHZ_E    - 166 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_200_MHZ_E    - 200 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_250_MHZ_E    - 250 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_300_MHZ_E    - 300 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_667_MHZ_E    - 667 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_800_MHZ_E    - 800 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_933_MHZ_E    - 933 MHz DRAM frequency.
 *  CPSS_DRAM_FREQ_1066_MHZ_E   - 1066 MHz dram frequency.
 *
 */
typedef enum
{
    CPSS_DRAM_FREQ_100_MHZ_E = 100,
    CPSS_DRAM_FREQ_125_MHZ_E = 125,
    CPSS_DRAM_FREQ_166_MHZ_E = 166,
    CPSS_DRAM_FREQ_200_MHZ_E = 200,
    CPSS_DRAM_FREQ_250_MHZ_E = 250,
    CPSS_DRAM_FREQ_300_MHZ_E = 300,
    CPSS_DRAM_FREQ_667_MHZ_E = 667,
    CPSS_DRAM_FREQ_800_MHZ_E = 800,
    CPSS_DRAM_FREQ_933_MHZ_E = 933,
    CPSS_DRAM_FREQ_1066_MHZ_E = 1066
}CPSS_DRAM_FREQUENCY_ENT;

/*
 * Typedef: enum  CPSS_DRAM_PAGE_SIZE_ENT
 *
 * Description:
 *  memory page size in the external DRAM
 *
 * Enumerations:
 *  CPSS_DRAM_PAGE_SIZE_512B_E - page size 512 Bytes
 *  CPSS_DRAM_PAGE_SIZE_1K_E   - page size 1K Bytes
 *  CPSS_DRAM_PAGE_SIZE_2K_E   - page size 2K Bytes
 *  CPSS_DRAM_PAGE_SIZE_4K_E   - page size 4K Bytes
*/
typedef enum{
    CPSS_DRAM_PAGE_SIZE_512B_E = 512,
    CPSS_DRAM_PAGE_SIZE_1K_E   = 1024,
    CPSS_DRAM_PAGE_SIZE_2K_E   = 2048,
    CPSS_DRAM_PAGE_SIZE_4K_E   = 4096
}CPSS_DRAM_PAGE_SIZE_ENT;

/*
 * Typedef: enum CPSS_DRAM_BUS_WIDTH_ENT
 *
 * Description: Defines the DRAM interface bus width in bits.
 *
 * Enumerations:
 *  CPSS_DRAM_BUS_WIDTH_4_E     - 4 bits bus width.
 *  CPSS_DRAM_BUS_WIDTH_8_E     - 8 bits bus width.
 *  CPSS_DRAM_BUS_WIDTH_16_E    - 16 bits bus width.
 *  CPSS_DRAM_BUS_WIDTH_32_E    - 32 bits bus width.
 *
 */
typedef enum
{
    CPSS_DRAM_BUS_WIDTH_4_E,
    CPSS_DRAM_BUS_WIDTH_8_E,
    CPSS_DRAM_BUS_WIDTH_16_E,
    CPSS_DRAM_BUS_WIDTH_32_E
}CPSS_DRAM_BUS_WIDTH_ENT;

/*
 * Typedef: enum CPSS_DRAM_TEMPERATURE_ENT
 *
 * Description: Defines the DRAM operation temperature 
 *              (refresh interval related).
 *
 * Enumerations:
 *  CPSS_DRAM_TEMPERATURE_NORMAL_E  - Up to 85C operating temperature.
 *  CPSS_DRAM_TEMPERATURE_HIGH_E    - Operating temperature can exceed 85C.
 *
 */
typedef enum
{
    CPSS_DRAM_TEMPERATURE_NORMAL_E,
    CPSS_DRAM_TEMPERATURE_HIGH_E
}CPSS_DRAM_TEMPERATURE_ENT;

/*
 * Typedef: enum CPSS_DRAM_SPEED_BIN_ENT
 *
 * Description: Defines DRAM speed bining (JEDEC standard name).
 *
 * Enumerations:
 *  CPSS_DRAM_SPEED_BIN_DDR3_800D_E     - DDR3-800D JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_800E_E     - DDR3-800E JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1066E_E    - DDR3-1066E JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1066F_E    - DDR3-1066F JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1066G_E    - DDR3-1066G JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1333F_E    - DDR3-1333F JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1333G_E    - DDR3-1333G JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1333H_E    - DDR3-1333H JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1333J_E    - DDR3-1333J JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1600G_E    - DDR3-1600G JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1600H_E    - DDR3-1600H JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1600J_E    - DDR3-1600J JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1600K_E    - DDR3-1600K JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1866J_E    - DDR3-1866J JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1866K_E    - DDR3-1866K JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1866L_E    - DDR3-1866L JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_1866M_E    - DDR3-1866M JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_2133K_E    - DDR3-2133K JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_2133L_E    - DDR3-2133L JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_2133M_E    - DDR3-2133M JEDEC standard name.
 *  CPSS_DRAM_SPEED_BIN_DDR3_2133N_E    - DDR3-2133N JEDEC standard name.
 *
 */
typedef enum
{
    CPSS_DRAM_SPEED_BIN_DDR3_800D_E,
    CPSS_DRAM_SPEED_BIN_DDR3_800E_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1066E_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1066F_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1066G_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1333F_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1333G_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1333H_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1333J_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1600G_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1600H_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1600J_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1600K_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1866J_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1866K_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1866L_E,
    CPSS_DRAM_SPEED_BIN_DDR3_1866M_E,
    CPSS_DRAM_SPEED_BIN_DDR3_2133K_E,
    CPSS_DRAM_SPEED_BIN_DDR3_2133L_E,
    CPSS_DRAM_SPEED_BIN_DDR3_2133M_E,
    CPSS_DRAM_SPEED_BIN_DDR3_2133N_E
}CPSS_DRAM_SPEED_BIN_ENT;

/*
 * Typedef: struct CPSS_DRAM_CFG_STC
 *
 * Description: Includes external DRAM configuration values.
 *              This structure is used for Flow & Buffers DRAMs.
 *
 * Fields:
 *      dramSize        - The external DRAM size.
 *                        In case the structure includes parameters for the
 *                        buffers DRAM, this field is ignored.
 *      dramFrequency   - The dram clock frequency.
 *      overwriteDef    - When required, indicates to overwrite the PP's default
 *                        values.
 *                          GT_TRUE  - overwrite,
 *                          GT_FALSE - ignore the rest of this struct's data.
 *      mrs             - DRAM mode register.
 *      emrs            - DRAM extend mode register.
 *      dramTiming
 *              tdqss       - Write latency.
 *              trcd        - Active read\write delay.
 *              trp         - Pre-charge cmd period.
 *              twr         - Recovery time.
 *              twtr        - Internal write to read cmd delay.
 *              tras        - Minimum row active time.
 *              trrd        - Active bank A to Bank B delay.
 *              tmrd        - Load mode reg cmd period.
 *              trfc        - Auto refresh cmd period.
 *              rd2rdDelay  - Number of cycles before driving read after read.
 *              rd2wrDelay  - Number of cycles before driving write after read.
 *              writeCount  - Maximum write transactions.
 *              readCount   - Maximum read transactions.
 *      refreshCounter  - This field indicates the maximal period that the
 *                        controller will issue refresh commands.
 *                        valid values: 0 - 0x3FFF
 *      trCdWr          - This field is applicable only for TWIST devices:
 *                        Active to Write delay.
 *                        valid range: 0x0 - 0x8
 *      operation       - This field indicates the Operation mode:
 *                          0 = Normal.
 *                          1 = Pre charge all banks.
 *                          2 = Refresh (followed by a Pre charge all banks).
 *                          3 = Mode Register Set (followed by a Pre charge all
 *                              banks).
 *                          4 = Extended Mode Register Set (followed by a
 *                              Pre charge all banks).
 *                          5 = No Operation.
 *                          6, 7 = Reserved.
 *   dramPageSize - DRAM page size -- actual only for PPs with external
 *                  Flow DRAM only. This parameter is ignored for Buffer DRAM.
 *
 * Comments:
 *      - Refer to the data-sheet for complete description of these parameters.
 *      - Setting an undefined values, like reserved values, may result in
 *        unpredictable results.
 */
typedef struct
{
    CPSS_DRAM_SIZE_ENT      dramSize;
    CPSS_DRAM_FREQUENCY_ENT dramFrequency;
    GT_BOOL                 overwriteDef;
    GT_U16                  mrs;
    GT_U16                  emrs;
    struct
    {
        GT_U8   tdqss;
        GT_U8   trcd;
        GT_U8   trp;
        GT_U8   twr;
        GT_U8   twtr;
        GT_U8   tras;
        GT_U8   trrd;
        GT_U8   tmrd;
        GT_U8   trfc;
        GT_U8   rd2rdDelay;
        GT_U8   rd2wrDelay;
        GT_U8   writeCount;
        GT_U8   readCount;
    }                       dramTiming;
    GT_U16                  refreshCounter;
    GT_U8                   trCdWr;
    GT_U8                   operation;
    CPSS_DRAM_PAGE_SIZE_ENT dramPageSize;
}CPSS_DRAM_CFG_STC;

/*
 * typedef: struct CPSS_RLDRAM_CFG_STC
 *
 * Description:
 *      This struct defines specific hardware parameters relevant when RLDRAM is used
 *      for the Buffer Memory.
 *
 * Fields:
 *  dramSize        - The external RLDRAM size.
 *  overwriteDef    - When required, indicates to overwrite the PP's default values.
 *                    GT_TRUE  - overwrite.
 *                    GT_FALSE - ignore the rest of this struct's data.
 *  mrs             - RLDRAM internal control register.
 *  refreshLimit    - the upper limit for interval between refresh operations.
 *                    Range: 0-4095
 *  dramTiming      - RLDRAM timing attributes:
 *                    tWL - The delay from the time that the write command was issued
 *                          to the first valid written data. The write delay should be
 *                          programmed as tRD+1. Range: 5-15
 *                    tRL - The delay from the time that the read command was issued to
 *                          the first read data. Range: 0-15
 *                    tRC - Row cycle time. Range: 4-15
 *                    tMPSC - The delay to be inserted after executing the MRS command
 *                            before execution of any other command. Range: 0-15
 *  pwrUpDelay      - idle time during initialization process. Range: 0-131071
 *  pwrUpRefDelay   - interval between two consecutive refresh command during the
 *                    initialization process. Range: 0-4095
 * Comment:
 *  All time dependent parameters (interval, timing, delay) are in relevant dram cycle units
 *
*/
typedef struct
{
    CPSS_DRAM_SIZE_ENT  dramSize;
    GT_BOOL             overwriteDef;
    GT_U32              mrs;
    GT_U32              refreshLimit;
    struct
    {
        GT_U32 tWL;
        GT_U32 tRL;
        GT_U32 tRC;
        GT_U32 tMPSC;
    }dramTiming;
    GT_U32 pwrUpDelay;
    GT_U32 pwrUpRefDelay;
}
CPSS_RLDRAM_CFG_STC;

/*
 * Typedef enum CPSS_HW_PP_RESET_SKIP_TYPE_ENT
 *
 * Description:
 *      define the skip type parameter to set. setting this parameters are
 *      relevant when setting <SoftResetTrigger> to 1.
 *          CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E:Skip the registers
 *                        initialization at soft reset.
 *              Disable = Set: At software reset, registers are set to their
 *                        default value.
 *              Enable  = Not Set: At software reset, registers are not set to
 *                        their default value. Their value remains as it was
 *                        before the software reset.
 *
 *          CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E:Skip the tables
 *                        initialization at soft reset.
 *              Disable = Set: At software reset, tables are set to their
 *                        default value.
 *              Enable = Not Set: At software reset, tables are not set to
 *                       their default value. Their value remains as it was
 *                       before the software reset.
 *
 *          CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E: Skip the EEPROM
 *                        initialization at soft reset.
 *              Disable  = Performed: At software reset, EEPROM initialization
 *                         is performed.
 *              Enable   = Not Performed: At software reset, EEPROM initialization
 *                         is not performed
 *
 *          CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E: Skip the PEX initialization at soft reset.
 *              Disable = Do Not Skip: Reset the PEX on SW Reset.
 *              Enable  = Skip: Do not reset the PEX on SW Reset.
 *
 *          CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E: Soft reset done without link loss.
 *              Disable = Do Not Skip: Link Loss on SW Reset.
 *              Enable  = Skip: No Link Loss on SW Reset.
 *
 *          CPSS_HW_PP_RESET_SKIP_TYPE_ALL_E: All options above
 *              Disable = Do Not Skip on SW Reset.
 *              Enable  = Skip on SW Reset.
 *
 *          CPSS_HW_PP_RESET_SKIP_TYPE_ALL_EXCLUDE_PEX_E :
 *              like CPSS_HW_PP_RESET_SKIP_TYPE_ALL_E but exclude CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E
 *              this case is useful when wanting the device to do soft reset
 *              without the need to reconfigure the PEX again.
 */
typedef enum {
    CPSS_HW_PP_RESET_SKIP_TYPE_REGISTER_E = 0,
    CPSS_HW_PP_RESET_SKIP_TYPE_TABLE_E,
    CPSS_HW_PP_RESET_SKIP_TYPE_EEPROM_E,
    CPSS_HW_PP_RESET_SKIP_TYPE_PEX_E,
    CPSS_HW_PP_RESET_SKIP_TYPE_LINK_LOSS_E,
    CPSS_HW_PP_RESET_SKIP_TYPE_ALL_E,
    CPSS_HW_PP_RESET_SKIP_TYPE_ALL_EXCLUDE_PEX_E
} CPSS_HW_PP_RESET_SKIP_TYPE_ENT;


/*
 * Typedef enum CPSS_HW_PP_INIT_STAGE_ENT
 *
 * Description:
 *      Indicates the initialization stage of the device.
 *
 *      CPSS_HW_PP_INIT_STAGE_INIT_DURING_RESET_E = Initialization is during reset
 *
 *      CPSS_HW_PP_INIT_STAGE_EEPROM_DONE_INT_MEM_DONE_E = Initialize after reset.
 *                   EEPROM initialization is done and Internal memory
 *                   initialization is not done.
 *
 *      CPSS_HW_PP_INIT_STAGE_EEPROM_NOT_DONE_INT_MEM_DONE_E = Initialize after reset.
 *                   Internal memory initialization is done and EEPROM
 *                   initialization is not done.
 *
 *      CPSS_HW_PP_INIT_STAGE_FULLY_FUNC_E = Initialization is fully functional.
 *
 */
typedef enum {
    CPSS_HW_PP_INIT_STAGE_INIT_DURING_RESET_E = 0,
    CPSS_HW_PP_INIT_STAGE_EEPROM_DONE_INT_MEM_DONE_E,
    CPSS_HW_PP_INIT_STAGE_EEPROM_NOT_DONE_INT_MEM_DONE_E,
    CPSS_HW_PP_INIT_STAGE_FULLY_FUNC_E
} CPSS_HW_PP_INIT_STAGE_ENT;

/*
 * typedef: struct CPSS_PP_PORT_GROUP_INIT_INFO_STC
 *
 * Description:
 *      This struct defines specific hardware parameters, that needed 'per port group'
 *      in a 'multi-port-groups' device.
 *
 * Fields:
 *    busBaseAddr       - the unique bus Base address (PCI/PEX base address /
 *                        SMI id select / TWSI id select...) that the device
 *                        connected to on the management interface bus
 *                        (PCI/PEX/SMI/TWSI)
 *                        NOTE: Use CPSS_PARAM_NOT_USED_CNS for inactive port group
 *    internalPciBase   - Base address to which the internal PCI registers
 *                        are mapped to.
 *                        relevant only when mngInterfaceType = PCI / PEX
 *                        NOTE: ignored when busBaseAddr == CPSS_PARAM_NOT_USED_CNS
 *    hwAddr            - HW (bus) address. Required for CPSS_CHANNEL_PEX_KERNEL_E only
 *    intVecNum         - The interrupt vector number this PP is connected to.
 *                        may use value CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS
 *                        when NO ISR bind needed for this device.
 *                        NOTE: ignored when busBaseAddr == CPSS_PARAM_NOT_USED_CNS
 *    intMask           - The interrupt mask to enable PP interrupts.
 *                        NOTE: ignored when busBaseAddr == CPSS_PARAM_NOT_USED_CNS
 */
typedef struct
{
    GT_UINTPTR            busBaseAddr;
    GT_UINTPTR            internalPciBase;
    CPSS_HW_ADDR_STC      hwAddr;
    GT_U32                intVecNum;
    GT_UINTPTR            intMask;
} CPSS_PP_PORT_GROUP_INIT_INFO_STC;

/*******************************************************************************
* cpssHwInitResourceMappingSet
*
* DESCRIPTION:
*       This function should be called to configure various resource
*       which are mapped to CPSS virtual address space
*       For example BC2/AC3/BobK SRAM for Service CPU
*
* APPLICABLE DEVICES: ALL devices
*
* INPUTS:
*       devNum             - The PP's device number to be initialized.
*       resourceMappingPtr - Resource mapping
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on error
*       GT_BAD_PTR   - on null pointer
*       GT_NOT_INITIALIZED - if the driver was not initialized
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssHwInitResourceMappingSet
(
    IN  GT_U8                       devNum,
    IN  CPSS_RESOURCE_MAPPING_STC  *resourceMappingPtr
);

/*******************************************************************************
* cpssPpInterruptsDisable
*
* DESCRIPTION:
*       This API disable PP interrupts
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum            - The device number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssPpInterruptsDisable
(
    IN GT_U8     devNum
);


/*******************************************************************************
* cpssPpConfigDevDataImport
*
* DESCRIPTION:
*       Imports data for device specific information.  Used in catch up process
*       during initialization as the first step in the catch up process for
*       configuring the standby CPU.
*
*       NOTE: for a device that will be initialized in "pp phase 1" with
*       High availability mode of "standby" , this function MUST be called prior
*       to the "pp phase 1"
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum                   - The device number.
*       configDevDataBufferPtr   - pointer to a pre allocated buffer for holding
*                                    information on devices in the system
*       configDevDataBufferSize  - size of the pre allocated buffer
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                  - on success.
*       GT_BAD_PARAM           - wrong device Number or configDevDataBufferSize
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpConfigDevDataImport
(
    IN GT_U8     devNum,
    IN void      *configDevDataBufferPtr,
    IN GT_U32    configDevDataBufferSize
);

/*******************************************************************************
* cpssPpConfigDevDataExport
*
* DESCRIPTION:
*       Imports data for device specific information.  Used in catch up process
*       during initialization as the first step in the catch up process for
*       configuring the standby CPU.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - The device number.
*       configDevDataBufferPtr      - pointer to a pre allocated buffer for
*                                     holding information on devices in the system
*       configDevDataBufferSizePtr  - size of the pre allocated buffer
*
* OUTPUTS:
*       configDevDataBufferPtr      - pointer to a pre allocated buffer for
*                                     holding information on devices in the system
*                                     filled by the function.
*       configDevDataBufferSizePtr  - size of the filled data in the pre
*                                     allocated buffer, in case of failure
*                                     (GT_BAD_SIZE), needed size of the block
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_SIZE              - block is not big enough to hold all the data
*       GT_BAD_PARAM             - wrong device Number
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssPpConfigDevDataExport
(
    IN    GT_U8     devNum,
    INOUT void      *configDevDataBufferPtr,
    INOUT GT_U32    *configDevDataBufferSizePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* cpssHwInit */

