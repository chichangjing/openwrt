/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChHwInit.h
*
* DESCRIPTION:
*       Includes CPSS level basic Hw initialization functions, and data
*       structures.
*
*       The DxCh driver supports next 'families' :
* ---   DxCh        (first generation)  :
*           10 GE ports : 98DX107
*           24GE+3XG ports : 98DX270,98DX273,98DX249,98DX269,98DX169
*           24GE+2XG ports : 98DX260,98DX263,98DX262,98DX268
*           24GE     ports : 98DX250,98DX253,98DX243,98DX248
*           16GE     ports : 98DX163,98DX166,98DX167
*           12GE+1XG ports : 98DX133
*           3XG      ports : 98DX803
*           subFamilies :
*              DxCh-Diamond :
*                   10GE ports : 98DX106
*
* ---   DxCh2       (second generation) :
*           24GE+4XG ports : 98DX285
*           24GE+3XG ports : 98DX275
*           24GE+2XG ports : 98DX265
*           24GE     ports : 98DX255
*           12GE+1XG ports : 98DX145
*           4XG      ports : 98DX804
*
* ---   DxCh3       (3'rd generation)   :
*           24GE+4XG ports : 98DX5128,98DX5128_1,98DX287,98DX5129
*           24GE+2XG ports : 98DX5126,98DX5127
*           24GE     ports : CPSS_98DX5124_CNS
*           24 1/2.5 GE + 2XG ports : 98DX5156
*           24 1/2.5 GE + 2XG + 2XGS ports : 98DX5157
*           16 1/2.5 GE ports + 2XG : 98DX5154
*           16 1/2.5 GE ports + 2XG + 2XGS ports : 98DX5155
*           8  1/2.5 GE ports + 2XG : 98DX5152
*           4  1/2.5 GE ports + 2XG : 98DX5151
*           8 XG or 6XG + 2 Stack : 98DX8108,98DX8109
*           10 XG or 6XG + 4 Stack : 98DX8110,98DX8110_1,98DX8111
*
* ---   DxCh xCat   (4'th generation)   :
*
*                      FE       GE       Stack  Stack     Stack     Stack
*                                         GE     GE/2.5GE  GE/2.5/   GE/2.5/
*                                               (DQX)     10/12GE   5/10/12GE
*                                                         (DQX/DHX) (DQX/DHX/XAUI)
*           98DX1000 Series: AlleyCat-FE WEBSMART- 333MHz, 0K Cache, SPI, DDR2
*           98DX1025     24        0        4        0        0        0
*           98DX1026     24        0        2        2        0        0
*           98DX1027     24        0        2        2        0        0
*
*           98DX1100 Series: TomCat-FE (SMB L2+) - 600MHz, 16KBKB/128KB Cache
*           98DX1122     24        0        0        4        0        0
*           98DX1123     24        0        0        4        0        0
*
*           98DX2100 Series: Bobcat-FE (L3+ / Metro) - 800MHz, , 16KB/256K Cache
*           98DX2101     8         0        0        4        0        0
*           98DX2122     24        0        0        4        0        0
*           98DX2123     24        0        0        4        0        0
*           98DX2142     48        0        0        4        0        0
*           98DX2151     24        0        0        2        0        2
*
*           98DX3000 Series: AlleyCat-GE (Entry L2) UNMANAGED- 333MHz, 0K Cache, 256KB Internal SRAM
*           98DX3011     0        16        2        0        0        0
*           98DX3021     0        24        2        0        0        0
*
*           98DX3000 Series: AlleyCat-GE (Entry L2) -  333MHz, , 0K Cache, 128KB memory
*           98DX3001     0         8        2        0        0        0
*           98DX3010     0        16        0        0        0        0
*           98DX3011     0        16        2        0        0        0
*           98DX3020     0        24        0        0        0        0
*           98DX3021     0        24        2        0        0        0
*
*           98DX3100 Series: TomCat-GE (SMB L2+) - 600MHz, 16KBKB/128KB Cache
*           98DX3101     0         8        2        0        0        0
*           98DX3121     0        24        0        2        2        0
*           98DX3122     0        24        0        0        0        4
*           98DX3123     0        24        0        0        0        4
*           98DX3124     0        24        0        0        2        2
*           98DX3125     0        24        0        0        2        2
*
*           98DX4100 Series: Bobcat-GE (L3+ / Metro) - 800MHz,1,16KB/256KB Cache
*           98DX4101     0         8        0        4        0        0
*           98DX4121     0        24        0        4        0        0
*           98DX4122     0        24        0        0        0        4
*           98DX4123     0        24        0        0        0        4
*           98DX5158     0        24        0        0        0        4
*           98DX5159     0        24        0        0        0        4
*
*       NOTE: about 'APPLICABLE DEVICES'
*           the statements of :
*           1. 'All DxCh Devices' means that the API applicable to all the
*               listed 'families' and all their sub devices
*           2. 'DxCh2 and above' means that the API applicable to the
*               listed 'families' below DxCh2 (including DxCh2) and all their
*               sub devices (DxCh2 , DxCh3 , DxCh xCat ,...)
*           3. 'DxCh3 and above' means that the API applicable to the
*               listed 'families' below DxCh3 (including DxCh3) and all their
*               sub devices (DxCh3 , DxCh xCat ,...)
*           4. 'DxCh xCat and above' means that the API applicable to the
*               listed 'families' below DxCh xCat (including DxCh xCat) and all their
*               sub devices (DxCh xCat , Lion...)
*           5. 'Lion and above' means that the API applicable to the
*               listed 'families' below Lion (including Lion) and all their
*               sub devices (Lion ,...)
*
*
* FILE REVISION NUMBER:
*       $Revision: 59 $
*
*******************************************************************************/

#ifndef __cpssDxChHwInith
#define __cpssDxChHwInith

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cpssHwInit/cpssHwInit.h>

/* Defines the max number SRAMs.
The number includes CTRL SRAM (wide SRAM) and routing SRAM (narrow SRAM)*/
#define CPSS_DXCH_MAX_NUMBER_OF_EXTERNAL_SRAMS_CNS 5

/* Use this constant to retrieve core clock value from HW. */
#define CPSS_DXCH_AUTO_DETECT_CORE_CLOCK_CNS 0xFFFFFFFF


/*  number of AU messages that application need to allocate as extra DMA cache
*   memory for the AUQ (extra to the size needed for the regular application
*   requirements) that will be use by the CPSS , for a secondary AUQ to workaround (WA)
*   the problem described for PRV_CPSS_DXCH_FER_FDB_AUQ_LEARNING_AND_SCANING_DEADLOCK_WA_E
*
*   NOTE: actual size in bytes is :
*       --> CPSS_DXCH_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS * descSizeInBytes
*       where descSizeInBytes retrieved from :
*       cpssDxChHwAuDescSizeGet(devNum,&descSizeInBytes);
*
*/
#define CPSS_DXCH_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS  64

/*
 * typedef: enum CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT
 *
 * Description: Defines SERDES reference clock type.
 *
 * Enumerations:
 *  CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E     - 25 MHz external
 *                                                                 single ended reference clock.
 *  CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E    - 125 MHz external
 *                                                                 single ended reference clock.
 *  CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E            - 125 MHz external
 *                                                                 differential reference clock.
 *  CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_SINGLE_ENDED_E - 156.25 MHz external
 *                                                                 single ended reference clock.
 *  CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E         - 156.25 MHz external
 *                                                                 differential reference clock.
 *  CPSS_DXCH_PP_SERDES_REF_CLOCK_INTERNAL_125_E                 - 125 MHz internal reference clock
 */
typedef enum
{
    CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E,
    CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_SINGLE_ENDED_E,
    CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_125_DIFF_E,
    CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_SINGLE_ENDED_E,
    CPSS_DXCH_PP_SERDES_REF_CLOCK_EXTERNAL_156_25_DIFF_E,
    CPSS_DXCH_PP_SERDES_REF_CLOCK_INTERNAL_125_E
}
CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT;

/*
 * typedef: struct CPSS_DXCH_PP_PHASE1_INIT_INFO_STC
 *
 * Description:
 *      This struct defines specific hardware parameters, those parameters are
 *      determined according to the board specific hardware components.
 *
 * Fields:
 *    devNum            - Temporary device number to allow basic communication.
 *                        (APPLICABLE RANGES: 0..31)
 *    busBaseAddr       - the unique bus Base address (PCI/PEX base address /
 *                        SMI id select / TWSI id select...) that the device
 *                        connected to on the management interface bus
 *                        (PCI/PEX/SMI/TWSI)
 *                        NOTE: Use CPSS_PARAM_NOT_USED_CNS for 'multi port groups'
 *                              device to indicate to use info from array of
 *                              multiPortGroupsInfoPtr
 *    internalPciBase   - Base address to which the internal PCI registers
 *                        are mapped to.
 *                        relevant only when mngInterfaceType = PCI / PEX
 *                        NOTE: ignored when busBaseAddr == CPSS_PARAM_NOT_USED_CNS
 *    resetAndInitControllerBase - Base address to which Reset and Init
 *                                 Controller (a.k.a. DFX server) are mapped to.
 *                                 (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *    hwAddr            - HW (bus) address. Now used for CPSS_CHANNEL_PEX_KERNEL_E only
 *    intVecNum         - The interrupt vector number this PP is connected to.
 *                        may use value CPSS_PP_INTERRUPT_VECTOR_NOT_USED_CNS
 *                        when NO ISR bind needed for this device.
 *                        NOTE: ignored when busBaseAddr == CPSS_PARAM_NOT_USED_CNS
 *    intMask           - The interrupt mask to enable PP interrupts.
 *                        NOTE: ignored when busBaseAddr == CPSS_PARAM_NOT_USED_CNS
 *    coreClock         - The PP core clock in MHz.
 *                        CPSS_DXCH_AUTO_DETECT_CORE_CLOCK_CNS may be used for
 *                        core clock auto detection.
 *    mngInterfaceType  - Management interface type (PCI/SMI/TWSI/PEX).
 *    ppHAState         - CPU High Availability mode(Active or Standby).
 *    powerDownPortsBmp - The bitmap of ports those SERDES power status is not changed
 *                        during cpssDxChHwPpPhase1Init. The bitmap is supported only
 *                        for DXCH3 devices and above and only for not Combo SERDES.
 *                        The bitmap is ignored for other device. In the DXCH3 devices
 *                        the HW default state of SERDES is power down. If port's bit
 *                        is 0 then cpssDxChHwPpPhase1Init changes power state of the
 *                        port's SERDES to be UP. If port's bit is 1 then
 *                        cpssDxChHwPpPhase1Init does not changes power state of the
 *                        port's SERDES and it stay to be DOWN.
 *    numOfPortGroups        - number of port groups that the device support , used to know
 *                        the number of elements in the array of multiPortGroupsInfoPtr.
 *                        NOTE: relevant only when busBaseAddr equal to CPSS_PARAM_NOT_USED_CNS
 *                              relevant to 'multi-port-groups' device.
 *                        for example : Use 4 for Lion device.
 *                                      for ch1,2,3/xcat ignored.
 *    multiPortGroupsInfoPtr - Pointer to array of info about the port groups of the
 *                        'multi-port-groups' device (according to numOfPortGroups)
 *                        index in the array is the 'portGroupId'.
 *                        inactive port group defined by : CPSS_PP_PORT_GROUP_INIT_INFO_STC::busBaseAddr = CPSS_PARAM_NOT_USED_CNS
 *                        NOTE: relevant only when busBaseAddr equal to CPSS_PARAM_NOT_USED_CNS (CPSS_DXCH_PP_PHASE1_INIT_INFO_STC::busBaseAddr)
 *                              relevant to 'multi port groups' device.
 *   serdesRefClock     - SERDES reference clock type.
 *   initSerdesDefaults - GT_TRUE - cpss performs SERDES initialization.
 *                        GT_FALSE- cpss doesn't perform SERDES initialization.
 *                        It is supported for DxCh3 and above devices.
 *   isExternalCpuConnected - it's ignored for all not xCat devices
 *                            it's ignored for xCat devices without internal CPU
 *                            it's relevant only for xCat devices with internal CPU
 *                            GT_TRUE - External CPU is connected to the PP. CPSS configures PP to work with external CPU
 *                                      even if internal CPU is active
 *                            GT_FALSE- External CPU is not connected to the PP (only internal CPU active)
 *
 *   tcamParityCalcEnable   - GT_TRUE  - enable all TCAMs parity calculation
 *                            GT_FALSE - disable all TCAMs parity calculation
 *                            To enable parity checks by device, use cpssDxChDiagDataIntegrityTcamParityDaemonEnableSet.
 *                            (APPLICABLE DEVICES: Lion2)
 *   isrAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
 *                            used by interrupt handling routine.
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *   appAddrCompletionRegionsBmp  - Bitmap of configurable address completion regions
 *                            used by CPSS API.
 *                            (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
 *                            Bobcat2, Caelum, Bobcat3 PPs that have 8 address completion regions.
 *                            Region 0 provide access to the lower addresses that contain also
 *                            configuration of address completion regions. It must never be configured.
 *                            Lists (coded as ranges) of other regions can be assigned for several CPUs
 *                            for interrupt handling (for such CPU that processes the PP interrupts)
 *                            and for all other CPSS API using.
 *                            These assigned ranges of regions must not have common members.
 *                            These assigned ranges of regions must not contain zero range.
 *   enableLegacyVplsModeSupport - GT_TRUE  - enable VPLS mode support
 *                                 GT_FALSE - disable VPLS mode support
 *                                 (APPLICABLE DEVICES: xCat, xCat3).
 *                                 xCat device: applicable starting from revision C0
 *
 *  maxNumOfPhyPortsToUse      - Set the number of physical ports that the device should support.
 *                                 (APPLICABLE DEVICES: Bobcat3).
 *                               Bobcat3 device supports next value:
 *                                  0   - means default of 256 ports mode
 *                                  256 - means 256 ports mode
 *                                  512 - means 512 ports mode
 *
 *                               In Bobcat3 there is tradeoff between number of physical ports and some tables
 *                               that hold all physical ports 'per entry'
 *                               So Bobcat3 can support 256 physical ports like Bobcat2  :
 *                                   8K vlans , 4K STG , 4K trunks , 4K SST-ID
 *                               Or Bobcat3 can support 512 physical ports :
 *                                   4K vlans, 2K STG , 2K trunks ,  2K SST-ID , single device (8bit port-id in eDSA)
 *                                   NOTE: Next tables keep 256 ports only !!!   :
 *                                      L2 Port Isolation , L3 Port Isolation
 *
 *
 */
typedef struct
{
    GT_U8                                  devNum;
    GT_UINTPTR                             busBaseAddr;
    GT_UINTPTR                             internalPciBase;
    GT_UINTPTR                             resetAndInitControllerBase;
    CPSS_HW_ADDR_STC                       hwAddr;
    GT_U32                                 intVecNum;
    GT_UINTPTR                             intMask;
    GT_U32                                 coreClock;
    CPSS_PP_INTERFACE_CHANNEL_ENT          mngInterfaceType;
    CPSS_SYS_HA_MODE_ENT                   ppHAState;
    CPSS_PORTS_BMP_STC                     powerDownPortsBmp;
    GT_U32                                 numOfPortGroups;
    CPSS_PP_PORT_GROUP_INIT_INFO_STC      *multiPortGroupsInfoPtr;
    CPSS_DXCH_PP_SERDES_REF_CLOCK_ENT      serdesRefClock;
    GT_BOOL                                initSerdesDefaults;
    GT_BOOL                                isExternalCpuConnected;
    GT_BOOL                                tcamParityCalcEnable;
    GT_U32                                 isrAddrCompletionRegionsBmp;
    GT_U32                                 appAddrCompletionRegionsBmp;
    GT_BOOL                                enableLegacyVplsModeSupport;
    GT_U32                                 maxNumOfPhyPortsToUse;
}CPSS_DXCH_PP_PHASE1_INIT_INFO_STC;

/*
 * typedef: struct CPSS_DXCH_PP_PHASE2_INIT_INFO_STC
 *
 * Description: PP Hw phase2 init Parameters.
 *
 * Fields:
 *    newDevNum        - The new device number to assign to this PP.
 *    netIfCfg         - Network interface configuration parameters.
 *                       Relevant only if useMultiNetIfSdma == GT_FALSE.
 *    auqCfg           - Address Update Queue configuration parameters.
 *                       NOTE:
 *                       when useSecondaryAuq = GT_TRUE , the cpss will use
 *                       64*4 words from the AUQ for the 'Secondary AUQ'.
 *    fuqUseSeparate - GT_TRUE  - use separate queue  FDB Upload messages ,
 *                   - GT_FALSE - use Address Update queue for FDB Upload messages
 *                     The parameter used only for DxCh2 and above devices and
 *                     ignored for another devices
 *    fuqCfg         - FDB Upload Queue configuration parameters.The parameter
 *                     used only for DxCh2 and above devices and ignored
 *                     for another devices
 *    useSecondaryAuq - a flag to indicate that the application needs the cpss
 *                       to implement the WA for the triggered action
 *                       completion while AU queue is full. The triggered FDB action
 *                       cannot be completed till AU queue is full. The 'Secondary AUQ'
 *                       may be used in order to fix the problem.
 *                       the Cpss will use a 'Secondary AUQ' , and for that will
 *                       remove CPSS_DXCH_PP_FDB_AUQ_DEADLOCK_EXTRA_MESSAGES_NUM_WA_CNS
 *                       entries from the AUQ.
 *                      GT_TRUE - cpss need to implement the WA for the triggered FDB action
 *                                cannot be completed till AU queue is full issue.
 *                                cpss will use 64*4 words from the AUQ (see auqCfg) for that
 *                                'Secondary AUQ'.
 *                      GT_FALSE - cpss WILL NOT implement the WA for the triggered FDB action
 *                                cannot be completed till AU queue is full issue.
 *                                cpss WILL NOT use 64*4 words from the AUQ (see auqCfg) for that
 *                                'Secondary AUQ'.
 *                                When useDoubleAuq == GT_TRUE, CPSS will use 2*64*4 words
 *                                from the AUQ (see auqCfg) for that  'Secondary AUQ'.
 *    netifSdmaPortGroupId     - value of port group ID used for CPU network
 *                               interface (relevant only for multi-port group devices while
 *                               working in both(!!!) MII and SDMA mode).
 *                               Relevant only if useMultiNetIfSdma == GT_FALSE.
 *    auMessageLength   - The AU massage length. Extending the AU message increases
 *                        the size of the message to more than 16 bytes
 *    useDoubleAuq    - Support configuration of two AUQ memory regions.
 *                      GT_TRUE - CPSS manages two AU Queues with the same size:
 *                      auqCfg->auDescBlockSize / 2.
 *                      GT_FALSE - CPSS manages single AU Queue with size:
 *                      auqCfg->auDescBlockSize.
 *    useMultiNetIfSdma - Enable Multi-Group network interface configurations.
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *    multiNetIfCfg     - Multi-Group related network interface configuration
 *                        parameters.
 *                        Relevant only if useMultiNetIfSdma == GT_TRUE.
 *                        (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *
 * Comments:
 *    1.    The Driver allocates Tx descriptors, Rx Descriptors, and Rx data
 *          buffers out of the respective blocks of memory provided by the user.
 *
 *          The number of allocated items is a function of the block size, item
 *          structure size, and the HW alignment requirement for the specific
 *          item (and the Rx Buffer headerOffset as defined in sysConfig.h)
 *
 *    2.    The Prestera PP PCI Host Interface directly updates the Rx/Tx
 *          descriptors, Rx data buffers, and the Address  Update Queue.
 *          It is the user's responsibility to determine whether these memory
 *          blocks (txDescBlock, rxDescBlock, rxbufBlock, AUQ) are from
 *          cachable. non-cachable memory regions. For example if the
 *          system supports cache-coherency between the CPU & the
 *          PCI, those blocks can be in a cacheble area with no
 *          special handling
 *
 *          If there is no cache coherency, then:
 *              1)  the Rx/Tx descriptors must be allocated from non-cachable
 *                  memory.
 *              2)  the Rx data buffers and AUQ may be from cachable memory,
 *                  if the user flushes the buffer memory cache prior to
 *                  returning the buffer to the Rx pool, and cache-line
 *                  reads do not cross data buffer boundaries.
 *
 *    3.    The provided auDescBlock will be divided into AU descriptors. The
 *          size of each AU descriptor can be obtained via
 *          cpssDxChHwAuDescSizeGet(), The number of address update descriptors
 *          is calculated by:(auDescBlockSize / <size of a single descriptor> ).
 *
 *    4.    The number of Rx descriptors to be allocated per Rx queue will be
 *          calculated as follows:
 *              -   if <allocation method> = STATIC_ALLOC, then the number of
 *                  Rx descs. per Rx queue will be:
 *                  (rxBufBlockSize / rxBufSize) / <number of Rx queues>
 *                  while The Rx buffer size must be a multiple of 8.
 *                  In this case the user must take care to provide an Rx Desc.
 *                  block which is large enough to allocate an Rx desc. per
 *                  allocated Rx buffer.
 *              -   if <allocation method> = DYNAMIC_ALLOC, then the number of
 *                  Rx descriptors per queue will be:
 *                  (rxDescBlockSize / <size of a single Rx desc.>) /
 *                  <number of Rx queues>.
 *                  The size of a single Rx descriptor can be obtained by
 *                  calling cpssDxChHwRxDescSizeGet().
 *          rxBufSize in the above calculations is received via TAPI
 *          sysConfig.h sysConfig() API.
 *
 *    5.    The number of Tx descriptors to be allocated per Tx queue will be
 *          calculated as follows:
 *          (txDescBlockSize / <size of a single Tx desc.>) /
 *          <number of Rx queues>.
 *          The size of a single Tx descriptor can be obtained by calling
 *          cpssDxChHwTxDescSizeGet().
 *
 *    6.   DxCh3 and above devices can upload the CNC 2048 counters block to the
 *         same FDB Upload Queue (fuqCfg). Two CNC counters takes place of one
 *         FU message.To upload whole CNC block the place for 1024 FU messages
 *         need to be allocated.
 *
 *
 *     7.  about 'multi port groups' device :
 *          a. netIfCfg : Cpss use all SMDA relate allocation with port group
 *             defined by netifSdmaPortGroupId that will be the only one used
 *             for rx/tx traffic from/to cpu.
 *          b. auqCfg : Cpss divide the DMA space evenly between all active
 *             port groups.
 *             Because there is no logic to divide it non-evenly (also for B0
 *             with 'split FDB mode')
 *          c. fuqUseSeparate : Apply to all port groups.
 *          d. fuqCfg : Cpss divide  the DMA configuration between all active
 *             port groups (to support CNC , and also for B0 with 'split FDB
 *             mode').
 */
typedef struct
{
    GT_U8                       newDevNum;
    CPSS_NET_IF_CFG_STC         netIfCfg;
    CPSS_AUQ_CFG_STC            auqCfg;
    GT_BOOL                     fuqUseSeparate;
    CPSS_AUQ_CFG_STC            fuqCfg;
    GT_BOOL                     useSecondaryAuq;
    GT_BOOL                     noTraffic2CPU;
    GT_U32                      netifSdmaPortGroupId;
    CPSS_AU_MESSAGE_LENGTH_ENT  auMessageLength;
    GT_BOOL                     useDoubleAuq;
    GT_BOOL                     useMultiNetIfSdma;
    CPSS_MULTI_NET_IF_CFG_STC   multiNetIfCfg;
}CPSS_DXCH_PP_PHASE2_INIT_INFO_STC;


/*
 * typedef: struct CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC
 *
 * Description: The structure defines the address decoding window fields.
 *                    Address driven by one of the PP unit (e.g. accessing DDR via PCIe) must hit
 *                    one of the windows in order to be driven outside the PP e.g. on the PCIe.
 *                    Some of the windows help to generate full 64bit address.
 *                    PP units generate a 32 bits physical address to target unit. If the driven address
 *                    hit the window, then the PP will prefix the driven address with the
 *                    configured 32 bits (remap address high).
 *
 * Fields:
 *     enableWindow - GT_TRUE to enable window, GT_FALSE to disable window.
 *                             if GT_FALSE other fields ignored.
 *
 *      baseAddress - Physical base address of the window, must be aligned to the window size.
 *
 *      windowSize   - Size of the window to remap, in steps of 64K. Must a power of 2.
 *                            i.e. 1 stands for 64K, 2 for 128K, 4 for 512K  and  64K for 4G window size.
 *
 *      remapAddress - Specifies physical address bits[63:32] to be driven to the target interface.
 *                              Relevant only for target interfaces that support more than 4 GB of address space.
 *                              This field ignored for windows 5 and 6, which do not support remapping to 64bit addressing mode.
 *
 *
 */
typedef struct
{
    GT_BOOL                                  enableWindow;
    GT_U32                                   baseAddress;
    GT_U32                                   windowSize;
    GT_U32                                   remapAddress;
}CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC;


/*
 * typedef: enum CPSS_DXCH_IMPLEMENT_WA_ENT
 *
 * Description:
 *      enumeration for the types of WAs (workarounds that CPSS need to do) for
 *      the DxCh devices
 *
 *
 * Enumerations:
 *      CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E - WA for xCat A1
 *          "wrong trunk id source port information of packet to CPU" erratum.
 *              NOTE: the request for this WA (if needed) must be before calling
 *              cpssDxChTrunkInit(...)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E - WA for xCat A1
 *          "packets of size 256*x-4 sent from CPU stack SDMA" erratum.
 *              NOTE: the request for this WA (if needed) must be before calling
 *              cpssDxChNetIfInit(...)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E - WA for xCat A1
 *              The CPU may hang when there is Register read in parallel with
 *              one of the following:
 *              AU message
 *              SDMA – packet to / from CPU.
 *              To avoid this problem, Register Read the AU Fifo instead
 *              of using the AU DMA.
 *              The WA influents only AU messages treatment.
 *              An application should use cpssDxChHwPpImplementWaInit in order
 *              to enable WA for SDMA packet to/from CPU -
 *              CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E.
 *              NOTE: the request for this WA (if needed) must be before calling
 *              cpssDxChCfgPpLogicalInit (...).
 *      CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E - WA for XCAT
 *              A1 and Lion.
 *              Traffic from the CPU via SDMA mechanism can corrupt the switch
 *              internal buffer memory.
 *              To avoid this, traffic sent from the CPU is padded with 8 bytes of
 *              zero’s, for byte counts of <byteblock>*n+k (1<=k<=8) (including
 *              DSA tag), where n and k are integers and <byteblock> is 256 for
 *              xCat and 512 for Lion.
 *              NOTE: the request for this WA (if needed) must be after calling
 *              cpssDxChHwPpPhase1Init(...).
 *      CPSS_DXCH_IMPLEMENT_WA_RGMII_EDGE_ALIGN_MODE_E - WA for xCat
 *              When working through RGMII interface with external HW (such as
 *              external PHY/CPU), the switch RGMII over RGMII pins must be set
 *              to edge align mode.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FU_FROM_NON_SOURCE_PORT_GROUP_E -
 *      WA for Lion.
 *      FDB messages of : AA,TA,QR,FU related to entry of 'port' of 'own device' ,
 *                  may be received in CPU from all port groups , and not only from
 *                  the port group of the 'port'.
 *      the Cpss will filter those 'duplicated' messages , so application will not be aware to them.
 *      NOTEs:
 *      1. the request for this WA (if needed) should be before calling cpssDxChBrgFdbInit(...)
 *      2. the messages still occupy the AU/FU queue, until removed by cpss.
 *          a call to: cpssDxChBrgFdbAuMsgBlockGet(...) will remove those from AUQ.
 *          a call to: cpssDxChBrgFdbFuMsgBlockGet(...) will remove those from FUQ.
 *          a call to: cpssDxChBrgFdbAuqFuqMessagesNumberGet(...) will return
 *                  value of 'num of messages' as if those messages not exists.
 *      3. for device with HW support of this filter , a call to this WA not needed.
 *         but when WA called , the CPSS will disable the HW filter support !
 *
 *      CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E  -
 *              WA for XCAT/XCAT2/LION.
 *              There are some memory spaces that cause the CPU or the
 *              management interface of device to freeze. To avoid this, every
 *              time the memory is accessed, a test will be performed.
 *              If the memory address falls within the range of problematic
 *              addresses, then an error will be returned.
 *              The memory space will not be accessed.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_IP_MC_UNDER_STRESS_E - WA for dxCh3, revision B2.
 *              IP Multicast traffic under stress conditions may lead to device hang.
 *              (FEr#2263)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_GE_PORT_UNIDIRECT_E - WA for XCAT/XCAT2/LION.
 *              in GE port interface mode transmission does not function when
 *              unidirect mode is set.
 *              (FEr#2795)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_2_5G_SGMII_LINK_UP_E - WA for XCAT/XCAT2.
 *              Port where enabled dp_clk hung upon link establish, on XCAT
 *              relevant for both network ports and stack ports, on XCAT2
 *              relevant only for stack ports.
 *              The name of enumerator is historical and misleading, pay
 *              attention this problem can happen on 1G speed too, the issue is
 *              dp_clk!
 *              (HWEr#3065)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_CFG_E - WA for XCAT.
 *          enable two WA's below:
 *      CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_NETWORKP_CFG_E - WA for XCAT.
 *      CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_STACKP_CFG_E   - WA for XCAT.
 *              While configuring dp_clk and sync_fifo for SGMII 2.5G traffic
 *               damaged on devices where dp_clk or sync_fifo are global bits
 *               per network ports and stacking ports. 1G modes can work with
 *               dp_clk, except for half duplex 10/100 configuration, so customers
 *               that may need this mode should not use this WA.
 *              QSGMII is not functional with these configurations, so we forced to
 *              split them to Network Ports configuration and Stack Ports config.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_SERDES_INTERNAL_REG_ACCESS_E - WA for LION.
 *              If we manage to push burst of transactions from pex, it may cause
 *              a stuck on the Seredes registers SM. Each transaction to the serdes
 *              registers goes through this SM. If 2 consecutive (WR) transactions
 *              are issued, the SM “get lost”, and the next RD access will not get
 *              acknowledged and MG will hang. The minimal delay needed is
 *              ~170-200ns. The required delay is between access to same group
 *              of 4 SD (0-3, 4-7, etc’). Shortly speaking 2 dummy reads to MG (0x4c)
 *              register is needed to avoid the problem.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_IPM_BRIDGE_COPY_GET_DROPPED_E - WA for LION2 B0
 *              IPM bridge copy get dropped when there is no member in the local
 *              hemisphere vlan
 *              WA solution:
 *              The WA use not connected to MAC ports 14 and 64+14 in
 *              the TxQ.
 *              Ports 14 of each hemisphere have Link UP in the TxQ unit
 *              The WA sets in every VLAN enabled for IP MC Routing Port
 *              14 and port 64+14 as a member.
 *              Packets in such VLANs go to ports 14 ands 64+14 TxQ
 *              queues as well as to other ports in VLANs.
 *              The WA uses the Tail drop to avoid packets stuck in the
 *              ports 14 ands 64+14 TxQ queues.
 *              The tails drop configurations is:
 *              - Tail Drop must be enabled
 *              - Configure Port 14 and port 64+14 to TailDropProfile 8
 *                NOTE: User can use only 7 out of 8 available profiles
 *              - Set Queue limits of profile 8 to all zeros
 *              (FE-7263524)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_NO_ALLIGNMENT_LOCK_E - WA for Lion2 A0(487 DIP) and
 *                                                      B0(different implementation)
 *              after several connect/disconnect get GearBox sync + no alignment
 *              lock.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_MC_BANDWIDTH_RESTRICTION_E -
 *              The device supports duplication of traffic to ports within the assigned
 *              VLAN. The device incorporates two internal
 *              engines that are responsible for the replication to all ports.
 *              However, due to this erratum, In some scenarios, although the traffic bandwidth
 *              in a specific PortGroup does not exceed the allowed bandwidth,
 *              not all VLAN members will transmit the replication.
 *              This behavior is due to extra replication (relay port)
 *              that is performed between the two internal replication engines.
 *              The restriction applies to a packet size under 128B only.
 *              The problem relevant both to Assigned Multicast (VIDX) and VLAN flooded traffic.
 *              Workaround:
 *              Add the relay port to the VLAN only if
 *              ports from both hemisphere are members in the VLAN.
 *              (FE-6041254)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_DISMATCH_IF_LINK_E - WA for Lion2 B0(AN333)
 *              10G/20G-R interfaces show false link while connected to interface
 *              with higher speed
 *      CPSS_DXCH_IMPLEMENT_WA_CUT_THROUGH_SLOW_TO_FAST_E -
 *              WA for LION2 B1.
 *              Lion2 device does not allow cut through traffic from slow port to faster one.
 *              The WA allows traffic from slow port with cut through enabled
 *              to be transmitted from faster port.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_RXAUI_LINK_E - - WA for Lion2 B0
 *              RXAUI sometimes fails to establish link.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_TRI_SPEED_PORT_AN_FC_E - WA for Bobcat2 A0
 *              Flow Control Auto-Negotiation for tri-speed ports requires software intervention.
 *              (FE-1789640)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_E
 *              bobcat2 rev A0 : when 40G port don't pass through TM it shall get
 *              30G at TxQ (FE-8708889)
 *
 *      CPSS_DXCH_IMPLEMENT_WA_WRONG_MIB_COUNTERS_LINK_DOWN_E
 *              removing wrong Mib counters after link down
 *
 *      CPSS_DXCH_IMPLEMENT_WA_ROUTER_TCAM_RM_E - WA for xCat (revision A2 and above) and xCat3
 *              Incorrect default value of bit 31 in the Routing TCAM Control register
 *              (RM#30230)
 * 
 *      CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E -
 *              Bobcat2 device only: Adjust FIFO thresholds for 1G and 10G ports in order to get precise PTP timestamp value under traffic.
 *
 *      CPSS_DXCH_IMPLEMENT_WA_LAST_E - last value indicator --> no to be used
 *              as WA for something.
*/
typedef enum
{
    CPSS_DXCH_IMPLEMENT_WA_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_E,
    CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_E,
    CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E,
    CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FU_FROM_NON_SOURCE_PORT_GROUP_E,

    CPSS_DXCH_IMPLEMENT_WA_SDMA_PKTS_FROM_CPU_STACK_PADDING_E,
    CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E,
    CPSS_DXCH_IMPLEMENT_WA_RGMII_EDGE_ALIGN_MODE_E,

    CPSS_DXCH_IMPLEMENT_WA_IP_MC_UNDER_STRESS_E,

    CPSS_DXCH_IMPLEMENT_WA_GE_PORT_UNIDIRECT_E,

    CPSS_DXCH_IMPLEMENT_WA_2_5G_SGMII_LINK_UP_E,

    CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_CFG_E,
    CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_NETWORKP_CFG_E,
    CPSS_DXCH_IMPLEMENT_WA_SGMII_2_5G_UNDER_TRAFFIC_STACKP_CFG_E,

    CPSS_DXCH_IMPLEMENT_WA_SERDES_INTERNAL_REG_ACCESS_E,

    CPSS_DXCH_IMPLEMENT_WA_IPM_BRIDGE_COPY_GET_DROPPED_E,

    CPSS_DXCH_IMPLEMENT_WA_NO_ALLIGNMENT_LOCK_E,

    CPSS_DXCH_IMPLEMENT_WA_MC_BANDWIDTH_RESTRICTION_E,

    CPSS_DXCH_IMPLEMENT_WA_DISMATCH_IF_LINK_E,

    CPSS_DXCH_IMPLEMENT_WA_CUT_THROUGH_SLOW_TO_FAST_E,

    CPSS_DXCH_IMPLEMENT_WA_RXAUI_LINK_E,

    CPSS_DXCH_IMPLEMENT_WA_TRI_SPEED_PORT_AN_FC_E,

    CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_REV_A0_40G_NOT_THROUGH_TM_IS_PA_30G_E,

    CPSS_DXCH_IMPLEMENT_WA_WRONG_MIB_COUNTERS_LINK_DOWN_E,

    CPSS_DXCH_IMPLEMENT_WA_ROUTER_TCAM_RM_E,

    CPSS_DXCH_IMPLEMENT_WA_BOBCAT2_PTP_TIMESTAMP_E,

    /* must be last value -- don't use this value */
    CPSS_DXCH_IMPLEMENT_WA_LAST_E
}CPSS_DXCH_IMPLEMENT_WA_ENT;

/*******************************************************************************
* cpssDxChHwPpPhase1Init
*
* DESCRIPTION:
*       This function performs basic hardware configurations on the given PP, in
*       Hw registration phase.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*
* OUTPUTS:
*       deviceTypePtr - The Pp's device Id.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on failure.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_ALREADY_EXIST         - the devNum already in use
*       GT_NOT_SUPPORTED         - the device not supported by CPSS
*       GT_OUT_OF_CPU_MEM        - failed to allocate CPU memory,
*       GT_BAD_VALUE             - the driver found unknown device type
*       GT_NOT_IMPLEMENTED       - the CPSS was not compiled properly
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpPhase1Init
(
    IN      CPSS_DXCH_PP_PHASE1_INIT_INFO_STC   *ppPhase1ParamsPtr,
    OUT     CPSS_PP_DEVICE_TYPE                 *deviceTypePtr
);

/*******************************************************************************
* cpssDxChHwPpPhase2Init
*
* DESCRIPTION:
*       This function performs basic hardware configurations on the given PP.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The PP's device number.
*       ppPhase2ParamsPtr  - Phase2 initialization parameters..
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpPhase2Init
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_PP_PHASE2_INIT_INFO_STC       *ppPhase2ParamsPtr
);

/*******************************************************************************
* cpssDxChHwPpStartInit
*
* DESCRIPTION:
*       This phase performs the start-init operation on a given device, and sets
*       it's registers according to the registers value list passed by user.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - The device number to which the start-init operation
*                         should be done.
*       loadFromEeprom  - GT_TRUE, trigger the device to re-load the eeprom.
*       initDataListPtr - List of registers values to be set to the device.
*       initDataListLen - Number of elements in ppRegConfigList
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - on timed out retries
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function should be invoked after cpssDxChHwPpPhase1Init() and
*       before cpssDxChHwPpPhase2Init().
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpStartInit
(
    IN  GT_U8               devNum,
    IN  GT_BOOL             loadFromEeprom,
    IN  CPSS_REG_VALUE_INFO_STC   *initDataListPtr,
    IN  GT_U32              initDataListLen
);

/*******************************************************************************
* cpssDxChHwPpImplementWaInit
*
* DESCRIPTION:
*       This function allow application to state which WA (workarounds) the CPSS
*       should implement.
*       NOTEs:
*       1. The function may be called several times with different needed WA.
*       2. The CPSS will implement the WA for the requested WA , even when the HW
*          not require the 'WA' anymore.
*          for example:
*                assume the xcat A1 has erratum regarding "wrong trunk id
*                source port information of packet to CPU" , and in the WA the CPSS
*                will limit application to specific trunk-IDs.
*                but if no longer have this problem , but the application
*                still request for the WA , the CPSS will continue to handle the
*                WA.
*                further more - application may ask the WA for ch3...
*       3. The function can be invoked only after cpssDxChHwPpPhase1Init().
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - The device number.
*       numOfWa             - number of WA needed (number of elements in
*                             waArr[] and additionalInfoBmpArr[] arrays).
*       waArr[]             - (array of) WA needed to implement
*       additionalInfoBmpArr[] - (array of) bitmap for additional WA info.
*                              The meaning of the additional info is located
*                              in the description of the specific WA. May be NULL.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or numOfWa = 0 or bad WA value
*       GT_BAD_STATE             - the WA can't be called at this stage ,
*                                  because it's relate library already initialized.
*                                  meaning it is 'too late' to request for the WA.
*       GT_NOT_IMPLEMENTED       - the WA can't be implemented for the current device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - on timed out retries
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpImplementWaInit
(
    IN GT_U8                        devNum,
    IN GT_U32                       numOfWa,
    IN CPSS_DXCH_IMPLEMENT_WA_ENT   waArr[],
    IN GT_U32                       additionalInfoBmpArr[]
);

/*******************************************************************************
* cpssDxChPpHwImplementWaGet
*
* DESCRIPTION:
*       Gets information on whether a specific workaround was implemented
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum      - The device number.
*       wa          - the workaround
*
* OUTPUTS:
*       isImplemented - GT_TRUE: the workaround is implemented on the device
*                      GT_FALSE: the workaround is not implemented on the device
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum or bad WA value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_IMPLEMENTED       - no information on whether the erratum was
*                                  implemented or not
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPpHwImplementWaGet
(
    IN  GT_U8                       devNum,
    IN  CPSS_DXCH_IMPLEMENT_WA_ENT  wa,
    OUT GT_BOOL                     *isImplemented
);


/*******************************************************************************
* cpssDxChHwAuDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Address Update
*       descriptor, for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwAuDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
);


/*******************************************************************************
* cpssDxChHwRxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Rx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwRxDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
);


/*******************************************************************************
* cpssDxChHwTxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Tx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS cpssDxChHwTxDescSizeGet
(
    IN  CPSS_PP_DEVICE_TYPE     devType,
    OUT GT_U32                  *descSizePtr
);


/*******************************************************************************
* cpssDxChHwRxBufAlignmentGet
*
* DESCRIPTION:
*       This function returns the required alignment in bytes of a RX buffer,
*       for a given device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       byteAlignmentPtr - The buffer alignment (in bytes).
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwRxBufAlignmentGet
(
    IN  CPSS_PP_DEVICE_TYPE devType,
    OUT GT_U32    *byteAlignmentPtr
);


/*******************************************************************************
* cpssDxChHwPpSoftResetTrigger
*
* DESCRIPTION:
*       This routine issue soft reset for a specific pp.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number to reset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Soft reset is the process in which selected entities (as configured by
*       using the cpssDxChHwPpSoftResetSkipParamSet API) are reset to their
*       default values.
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpSoftResetTrigger
(
    IN  GT_U8 devNum
);


/*******************************************************************************
* cpssDxChHwPpSoftResetSkipParamSet
*
* DESCRIPTION:
*       This routine configure skip parameters related to soft reset.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       skipType    - the skip parameter to set
*                     see comments in CPSS_HW_PP_RESET_SKIP_TYPE_ENT.
*       skipEnable  - GT_FALSE: Do Not Skip
*                     GT_TRUE:  Skip
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, or skipType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpSoftResetSkipParamSet
(
    IN  GT_U8                           devNum,
    IN  CPSS_HW_PP_RESET_SKIP_TYPE_ENT  skipType,
    IN  GT_BOOL                         skipEnable

);

/*******************************************************************************
* cpssDxChHwPpSoftResetSkipParamGet
*
* DESCRIPTION:
*       This routine return configuration of skip parameters related to soft reset.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       skipType    - the skip parameter to set
*                     see comments in CPSS_HW_PP_RESET_SKIP_TYPE_ENT.
*
* OUTPUTS:
*       skipEnablePtr   - GT_FALSE: Do Not Skip
*                         GT_TRUE:  Skip
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, or skipType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpSoftResetSkipParamGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_HW_PP_RESET_SKIP_TYPE_ENT  skipType,
    OUT GT_BOOL                         *skipEnablePtr

);

/*******************************************************************************
* cpssDxChHwPpInitStageGet
*
* DESCRIPTION:
*       Indicates the initialization stage of the device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - device number to reset.
*
* OUTPUTS:
*       initStagePtr - pointer to the stage of the device
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpInitStageGet
(
    IN  GT_U8                      devNum,
    OUT CPSS_HW_PP_INIT_STAGE_ENT  *initStagePtr
);

/*******************************************************************************
* cpssDxChCfgHwDevNumSet
*
* DESCRIPTION:
*       write HW device number
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       hwDevNum - HW device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_OUT_OF_RANGE          - on hwDevNum > 31
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHwDevNumSet
(
    IN GT_U8            devNum,
    IN GT_HW_DEV_NUM    hwDevNum
);

/*******************************************************************************
* cpssDxChCfgHwDevNumGet
*
* DESCRIPTION:
*       Read HW device number
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       hwDevNumPtr - Pointer to HW device number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS cpssDxChCfgHwDevNumGet
(
    IN GT_U8            devNum,
    OUT GT_HW_DEV_NUM   *hwDevNumPtr
);

/*******************************************************************************
* cpssDxChHwInterruptCoalescingSet
*
* DESCRIPTION:
*       Configures the interrupt coalescing parameters and enable\disable the
*       functionality.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: interrupt coalescing is enabled
*                 GT_FALSE: interrupt coalescing is disabled
*       period  - Minimal IDLE period between two consecutive interrupts.
*                 The units of this input parameter is in nSec, the
*                 resolution is in 320 nSec (the HW resolution for the
*                 interrupt coalescing period is 0.32uSec). In case interrupt
*                 coalescing is enabled (enable == GT_TRUE)
*                 (APPLICABLE RANGES: 320..5242560).
*                 Otherwise (enable == GT_FALSE), ignored.
*       linkChangeOverride  - GT_TRUE: A link change in one of the ports
*                             results interrupt regardless of the (coalescing)
*                             period.
*                             GT_FALSE: A link change in one of the ports
*                             does not results interrupt immediately but
*                             according to the (coalescing) period.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - parameter out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChHwInterruptCoalescingSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable,
    IN GT_U32   period,
    IN GT_BOOL  linkChangeOverride
);

/*******************************************************************************
* cpssDxChHwInterruptCoalescingGet
*
* DESCRIPTION:
*       Gets the interrupt coalescing configuration parameters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE: interrupt coalescing is enabled
*                                 GT_FALSE: interrupt coalescing is disabled
*       periodPtr  - (pointer to) Minimal IDLE period between two consecutive
*                    interrupts. The units are in nSec with resolution of
*                    320nSec (due to HW resolution) and is relevant only in
*                    case interrupt coalescing is enabled (*enablePtr == GT_TRUE).
*                    (APPLICABLE RANGES: 320..5242560).
*       linkChangeOverridePtr  - (pointer to)
*                                GT_TRUE: A link change in one of the ports
*                                results interrupt regardless of the (coalescing)
*                                period.
*                                GT_FALSE: A link change in one of the ports
*                                does not results interrupt immediately but
*                                according to the (coalescing) period.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The HW resolution for the interrupt coalescing period is 0.32uSec.
*
*******************************************************************************/
GT_STATUS cpssDxChHwInterruptCoalescingGet
(
    IN GT_U8                devNum,
    OUT GT_BOOL         *enablePtr,
    OUT GT_U32            *periodPtr,
    OUT GT_BOOL     *linkChangeOverridePtr
);

/*******************************************************************************
* cpssDxChHwCoreClockGet
*
* DESCRIPTION:
*       This function returns the core clock value from cpss DB and from HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - The Pp's device number.
*
* OUTPUTS:
*       coreClkDbPtr    - Pp's core clock from cpss DB (MHz)
*       coreClkHwPtr    - Pp's core clock read from HW (MHz) or zero
*                         if PLL configuration as sampled at reset could
*                         not be mapped to core clock value.
* RETURNS:
*       GT_OK                    - on success,
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwCoreClockGet
(
    IN  GT_U8   devNum,
    OUT GT_U32  *coreClkDbPtr,
    OUT GT_U32  *coreClkHwPtr
);

/*******************************************************************************
* cpssDxChHwInitLion2GeBackwardCompatibility
*
* DESCRIPTION:
*       Init parameters of serdes initialization for 1.25G frequncy accordingly to
*       HWS version 43. Proposed for Lion2 B0 only.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Must be call before cpssDxChHwPpPhase1Init.
*
*******************************************************************************/
GT_VOID cpssDxChHwInitLion2GeBackwardCompatibility
(
    GT_VOID
);

/*******************************************************************************
* cpssDxChHwPpAddressDecodeWindowConfigSet
*
* DESCRIPTION:
*       This function perfroms the address decoding window configuration.
*       If a window is enabled and an address driven by one the PP's units hit the window,
*       the window's configuration applied on this address access.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - The PP's device number.
*       windowNumber   - window number (APPLICABLE RANGES: 0..5).
*       windowConfigPtr  - window's configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_HW_ERROR             - on hardware error
*       GT_BAD_PARAM            - wrong devNum, window size or window base address
*       GT_NOT_SUPPORTED    - if this feature not supported by the device.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpAddressDecodeWindowConfigSet
(
    IN  GT_U8                                     devNum,
    IN  GT_U32                                    windowNumber,
    IN  CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC  *windowConfigPtr
);

/*******************************************************************************
* cpssDxChHwPpAddressDecodeWindowConfigGet
*
* DESCRIPTION:
*       This function returns the address decoding window configuration.
*       If a window is enabled and an address driven by one the PP's units hit the window,
*       the window's configuration applied on this address access.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum             - The PP's device number.
*       windowNumber   - window number (APPLICABLE RANGES: 0..5).
*
* OUTPUTS:
*       windowConfigPtr - window's configuration.
*
* RETURNS:
*       GT_OK on success, or
*       GT_FAIL otherwise.
*       GT_HW_ERROR             - on hardware error
*       GT_BAD_PARAM            - wrong devNum, window size or window base address
*       GT_BAD_STATE           - get illegal value from HW.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChHwPpAddressDecodeWindowConfigGet
(
    IN  GT_U8                                     devNum,
    IN  GT_U32                                    windowNumber,
    OUT CPSS_DXCH_ADDR_DECODE_WINDOW_CONFIG_STC  *windowConfigPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChHwInith */
