/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenericSrvCpuIpc.h
*
* DESCRIPTION:
*       Service CPU IPC APIs
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <cpss/generic/srvCpu/prvCpssGenericSrvCpuLoad.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include "prvCpssGenericSrvCpuIpcDevCfg.h"

#ifndef LINUX_NOKM
#define DO_CONFIGURE_SRAM
#endif

/* rename to avoid */
#define mvHwsServiceCpuFw _prv_mvHwsServiceCpuFw
#define mvHwsServiceCpuFwSize _prv_mvHwsServiceCpuFwSize
#include "../labServices/port/gop/src/port/silicon/serviceCpu/mvHwsServiceCpuFw.h"
#define mvHwsServiceCpuCm3Fw _prv_mvHwsServiceCpuCm3Fw
#define mvHwsServiceCpuCm3FwSize _prv_mvHwsServiceCpuCm3FwSize
#include "../labServices/port/gop/src/port/silicon/serviceCpu/mvHwsServiceCpuCm3Fw.h"

PRV_CPSS_IPC_DEV_CFG_STC *prvIpcDevCfg[PRV_CPSS_MAX_PP_DEVICES_CNS];

#ifdef DO_CONFIGURE_SRAM
static GT_STATUS mv_configure_sram(
    GT_UINTPTR regsBase,
    GT_U32     sramPhys,
    GT_U32     sramSize
);
#endif /* DO_CONFIGURE_SRAM */

static struct ipc_link_info ipc_link_cfg_bc2_freertos = {
    6,                              /* num_of_chn */
    mv_true,                        /* is_master */
    MV_IPC_NODE_ID_SLAVE,           /* remote_node_id */
    0,                              /* shmem_size */
    NULL,                           /* shmem_virt_addr */
    NULL,                           /* dev_id */
    NULL,                           /* send_trigger */
    NULL,                           /* enable_chn_doorbell */
    NULL,                           /* irq_init */
    50,                             /* master_free_region_percent */

    { 1, 1, 2, 2, 2, 2, 0, 0 },     /* chn_rx_info_array */
    { 4, 4, 1, 1, 1, 1, 0, 0 },     /* chn_tx_info_array */

    { 0, 0, 16, 16, 16, 16, 0, 0 }, /* chn_rx_buffer_info */
    { 16, 16, 0, 0, 0, 0, 0, 0 }    /* chn_tx_buffer_info */
};

static struct ipc_link_info ipc_link_cfg_cm3_freertos = {
    6,                              /* num_of_chn */
    mv_true,                        /* is_master */
    MV_IPC_NODE_ID_SLAVE,           /* remote_node_id */
    0,                              /* shmem_size */
    NULL,                           /* shmem_virt_addr */
    NULL,                           /* dev_id */
    NULL,                           /* send_trigger */
    NULL,                           /* enable_chn_doorbell */
    NULL,                           /* irq_init */
    50,                             /* master_free_region_percent */

    { 1, 1, 2, 2, 2, 2, 0, 0 },     /* chn_rx_info_array */
    { 4, 4, 1, 1, 1, 1, 0, 0 },     /* chn_tx_info_array */

    {  0,  0, 16, 16, 16, 16, 0, 0 },/* chn_rx_buffer_info */
    { 16, 16,  0,  0,  0,  0, 0, 0 }/* chn_tx_buffer_info */
};

typedef struct PRV_CPSS_FIRMWARE_CONFIG_STCT {
    const char *name;
    GT_U32      sramPhys;   /* MSYS/CM3 address */
    GT_U32      sramSize;
    GT_U32      fwLoadAddr; /* MSYS/CM3 address */
    GT_U8      *fwData;
    GT_U32      fwSize;
    GT_U32      ipcShmAddr; /* MSYS address */
    GT_U32      ipcShmSize; /* Size of shm */
    unsigned char           linkId;
    struct ipc_link_info   *linkInfoPtr;
    GT_STATUS (*pre_load_func)(
        GT_U8 devNum,
        struct PRV_CPSS_FIRMWARE_CONFIG_STCT *config
    );
} PRV_CPSS_FIRMWARE_CONFIG_STC;

static GT_STATUS prv_bc2_pre_load(
    GT_U8 devNum,
    struct PRV_CPSS_FIRMWARE_CONFIG_STCT *config
)
{
#if 0
    GT_STATUS rc;
    /* CPU0 Boot Address Redirect Register */
    rc = cpssDrvPpHwInternalPciRegWrite(
                devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                0x22124, 0xffff0000);
    if (rc != GT_OK)
        return rc;
#endif
#ifdef DO_CONFIGURE_SRAM
    cpssOsPrintf("Configure SRAM...\n");
    return mv_configure_sram(prvCpssDrvPpConfig[devNum]->hwCtrl[0].internalPciBase,
                config->sramPhys, config->sramSize);
#else /* !defined(DO_CONFIGURE_SRAM) */
    return GT_OK;
#endif /* DO_CONFIGURE_SRAM */
}

static PRV_CPSS_FIRMWARE_CONFIG_STC bc2_fw_config = {
    "bobcat2",                      /* name */
    0xffe00000,                     /* sramPhys */
    _2M,                            /* sramSize */
    0xfff80000,                     /* fwLoadAddr */
    (GT_U8*)mvHwsServiceCpuFw,      /* fwData */
    sizeof(mvHwsServiceCpuFw),      /* fwSize */
    0xfff40000,                     /* ipcShmAddr */
    0x40000,                        /* ipcShmSize */
    IPC_SCPU_FREERTOS_LINK_ID,      /* linkId */
    &ipc_link_cfg_bc2_freertos,     /* linkInfoPtr */
    prv_bc2_pre_load                /* pre_load_func */
};

#ifdef _VISUALC
#pragma warning(push)
#pragma warning(disable: 4100)
#endif
static GT_STATUS prv_cm3_pre_load(
    GT_U8 devNum,
    struct PRV_CPSS_FIRMWARE_CONFIG_STCT *config GT_UNUSED
)
{
    GT_U32 reg;
    /* Confi Processor Global Configuration Register
     * 0x500
     */
    cpssDrvPpHwRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                0x500, &reg);
    reg |= 0x00100000; /* bit 20: init ram */
    reg &= 0xfff7ffff; /* bit 19: CM3_PROC_CLK_DIS */
    cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                0x500, reg);

    cpssDrvPpHwRegisterRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                0x54, &reg);
    reg |= 1; /* bit 0: CM3_METAL_FIX_JTAG_EN */
    cpssDrvPpHwRegisterWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                0x54, reg);
    return GT_OK;
}
#ifdef _VISUALC
#pragma warning(pop)
#endif

static PRV_CPSS_FIRMWARE_CONFIG_STC cm3_fw_config = {
    "cm3(cetus/caelum)",            /* name */
    0x0,                            /* sramPhys */
    _128K,                          /* sramSize */
    0,                              /* fwLoadAddr */
    (GT_U8*)mvHwsServiceCpuCm3Fw,   /* fwData */
    sizeof(mvHwsServiceCpuCm3Fw),   /* fwSize */
    0x1f800,                        /* ipcShmAddr */
    0x800,                          /* ipcShmSize */
    IPC_CM3_FREERTOS_LINK_ID,       /* linkId */
    &ipc_link_cfg_cm3_freertos,     /* linkInfoPtr */
    prv_cm3_pre_load                /* pre_load_func */
};

struct PRV_CPSS_FIRMWARE_DETECT_DEV_STCT {
    GT_U16                          devId;
    GT_U16                          devIdMask;
    PRV_CPSS_FIRMWARE_CONFIG_STC   *fw_config;
} fwConfig[] = {
    { 0xfc00, 0xff00, &bc2_fw_config }, /* bobcat2 */
    { 0xbe00, 0xff00, &cm3_fw_config }, /* cetus */
    { 0xbc00, 0xff00, &cm3_fw_config }, /* caelum */
#if 0
    { 0xf400, ~0xff, /*TODO */ }, /* ac3 */
#endif

    { 0,0,NULL }
};

/*******************************************************************************
* prvCpssGenericSrvCpuCheck
*
* DESCRIPTION:
*       Check if device applicable
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssGenericSrvCpuCheck
(
    IN GT_U8    devNum,
    OUT PRV_CPSS_FIRMWARE_CONFIG_STC **config
)
{
    GT_U32 i;
    GT_U16 deviceId; /* PCI device Id */
    *config = NULL;

    if (devNum >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;
    if (prvCpssDrvPpConfig[devNum] == NULL)
        return GT_BAD_PARAM;

    deviceId = ((prvCpssDrvPpConfig[devNum]->devType)>>16) & 0xffff;

    for (i = 0; fwConfig[i].fw_config; i++)
    {
        if ((deviceId & fwConfig[i].devIdMask) == fwConfig[i].devId)
        {
            *config = fwConfig[i].fw_config;
            break;
        }
    }
    if (!(*config))
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }
    /* TODO: BC2 MSYS not supported!!! */

    /* TODO: WA hack!!! called in Phase1Part1, so sramBaseAddress need to
     * be provided in Phase1InitParams */
    if ((*config)->linkId == IPC_CM3_FREERTOS_LINK_ID)
    {
        prvCpssDrvPpConfig[devNum]->resourceMapping.sramBaseAddress =
                prvCpssDrvPpConfig[devNum]->hwCtrl[0].baseAddr + 0x40000;
        prvCpssDrvPpConfig[devNum]->resourceMapping.sramSize = (*config)->sramSize;
    }
    if (prvCpssDrvPpConfig[devNum]->resourceMapping.sramBaseAddress == 0 &&
        prvCpssDrvPpConfig[devNum]->resetAndInitControllerBase != 0)
    {
        prvCpssDrvPpConfig[devNum]->resourceMapping.sramBaseAddress =
                prvCpssDrvPpConfig[devNum]->resetAndInitControllerBase + _2M;
    }

    if (prvCpssDrvPpConfig[devNum]->resourceMapping.sramBaseAddress == 0)
        return GT_NOT_APPLICABLE_DEVICE;

    if (prvCpssDrvPpConfig[devNum]->resourceMapping.sramSize == 0)
        prvCpssDrvPpConfig[devNum]->resourceMapping.sramSize = (*config)->sramSize;

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericSrvCpuLoad
*
* DESCRIPTION:
*       Upload firmware to service CPU according to firmware table
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuLoad
(
    IN GT_U8    devNum
)
{
    CPSS_RESOURCE_MAPPING_STC *map;
    PRV_CPSS_FIRMWARE_CONFIG_STC *config;
    PRV_CPSS_IPC_DEV_CFG_STC *ipc;
    GT_STATUS rc;

    rc = prvCpssGenericSrvCpuCheck(devNum, &config);
    if (rc != GT_OK)
        return rc;
    if (prvIpcDevCfg[devNum] != NULL)
        return GT_ALREADY_EXIST;

    map = &(prvCpssDrvPpConfig[devNum]->resourceMapping);

    prvCpssGenericSrvCpuStop(devNum);

    if (config->pre_load_func != NULL)
    {
        rc = config->pre_load_func(devNum, config);
        if (rc != GT_OK)
            return rc;
    }

    /* TODO: load from NFS */

    if ((config->fwLoadAddr - config->sramPhys) + config->fwSize > map->sramSize)
    {
        return GT_BAD_PARAM;
    }
#define SRAM_VIRT_ADDR(_sram_addr) (void*)(map->sramBaseAddress+((_sram_addr)-config->sramPhys))
    cpssOsPrintf("Uploading firmware to %s:0x%x\n", config->name, config->fwLoadAddr);
    cpssOsPrintf(" memcpy(%p, fw, 0x%x);\n",
        SRAM_VIRT_ADDR(config->fwLoadAddr), (unsigned)(config->fwSize));
    cpssOsMemCpy(SRAM_VIRT_ADDR(config->fwLoadAddr), config->fwData, config->fwSize);
    if (cpssOsMemCmp(SRAM_VIRT_ADDR(config->fwLoadAddr), config->fwData, config->fwSize) != 0)
    {
        cpssOsPrintf(" memcmp failed!\n");
        return GT_FAIL;
    }
    /* TODO: counter ??? */

    /* Fill prvIpcDevCfg[devNum] */
    ipc = (PRV_CPSS_IPC_DEV_CFG_STC*)cpssOsMalloc(sizeof(*ipc));
    if (!ipc)
        return GT_NO_RESOURCE;
    prvIpcDevCfg[devNum] = ipc;
    cpssOsMemSet(ipc, 0, sizeof(*ipc));
    ipc->sramBase = (void*)map->sramBaseAddress;
    ipc->fwAddress = SRAM_VIRT_ADDR(config->fwLoadAddr);
    ipc->shmAddress = SRAM_VIRT_ADDR(config->ipcShmAddr);
    ipc->linkConfigured = GT_FALSE;
    ipc->link_id = config->linkId;
    ipc->link_cfg = *(config->linkInfoPtr);
    ipc->link_cfg.shmem_size = config->ipcShmSize;
    ipc->link_cfg.shmem_virt_addr = ipc->shmAddress;
#if 0
    ipc->link_cfg.send_trigger = ;       /* Trigger to remote node to start RX */
    ipc->link_cfg.enable_chn_doorbell = ; /* Register the channel in RX ISR/Timer */
    ipc->link_cfg.irq_init = ;
#endif

    /* Configure link before FW started */
    cpssOsPrintf("Configuring ipc link... shmAddress=%p\n",ipc->shmAddress);
    rc = ipc_shmem_link_setup(ipc->link_id, ipc->link_cfg);
    if (rc != 0)
    {
        cpssOsPrintf("Failed to setup link %d for device %d\n", ipc->link_id, devNum);
        return GT_FAIL;
    }

#undef SRAM_VIRT_ADDR
    return GT_OK;
}


/*******************************************************************************
* prvCpssGenericSrvCpuStartStop
*
* DESCRIPTION:
*       Start/Stop (unreset/reset) service CPU(MSYS)
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*       start                    - start/stop service CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssGenericSrvCpuStartStop
(
    IN GT_U8    devNum,
    IN GT_BOOL  start
)
{
    PRV_CPSS_FIRMWARE_CONFIG_STC *config;
    GT_U32  reg;
    GT_STATUS rc;

    rc = prvCpssGenericSrvCpuCheck(devNum, &config);
    if (rc != GT_OK)
        return rc;

    if (config->linkId == IPC_SCPU_FREERTOS_LINK_ID)
    {
        /* CPUn SW Reset Control Register
         * 20800+8*n
         * Bit 0: CPU_SW_RESET
         */
        rc = cpssDrvPpHwInternalPciRegRead(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    0x20800, &reg);
        if (rc != GT_OK)
            return rc;
        if (start == GT_TRUE)
            reg &= 0xfffffffe;
        else
            reg |= 1;
        rc = cpssDrvPpHwInternalPciRegWrite(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    0x20800, reg);
    }
    if (config->linkId == IPC_CM3_FREERTOS_LINK_ID)
    {
        /* Confi Processor Global Configuration Register
         * 0x500
         * Bit 28: Controls CM3 activation: 0: Disable 1:enable
         */
        rc = cpssDrvPpHwRegisterRead(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    0x500, &reg);
        if (rc != GT_OK)
            return rc;
        if (start == GT_TRUE)
            reg |= 0x10000000;
        else
            reg &= 0xefffffff;
        rc = cpssDrvPpHwRegisterWrite(
                    devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                    0x500, reg);
    }

    return rc;
}

/*******************************************************************************
* prvCpssGenericSrvCpuStop
*
* DESCRIPTION:
*       Stop (reset) service CPU(MSYS)
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuStop
(
    IN GT_U8    devNum
)
{
    return prvCpssGenericSrvCpuStartStop(devNum, GT_FALSE);
}

/*******************************************************************************
* prvCpssGenericSrvCpuStart
*
* DESCRIPTION:
*       Start (unreset) service CPU(MSYS)
*
* APPLICABLE DEVICES:
*       Bobcat2
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx; Caelum; Bobcat3.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericSrvCpuStart
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc;
    rc = prvCpssGenericSrvCpuStartStop(devNum, GT_TRUE);
    if (rc == GT_OK)
    {
        /* sleep 2 sec till FW up */
        cpssOsTimerWkAfter(2000);
    }
    return rc;
}


#ifdef DO_CONFIGURE_SRAM
/* #define CONFIG_SRAM_DEBUG */
#define BITS(_data,_start,_end) (((_data)>>(_end)) & ((1 << ((_start)-(_end)+1))-1))
static GT_U32 mv_reg_read(GT_UINTPTR regsBase, GT_U32 regAddr)
{
    GT_U32 val = *((volatile GT_U32*)(regsBase+regAddr));
#ifdef CONFIG_SRAM_DEBUG
    cpssOsPrintf("RD base=%p regAddr=0x%08x data=0x%08x\n",(void*)regsBase,regAddr,val);
#endif
    return CPSS_32BIT_LE(val);
}
static void mv_reg_write(GT_UINTPTR regsBase, GT_U32 regAddr, GT_U32 value)
{
    *((volatile GT_U32*)(regsBase+regAddr)) = CPSS_32BIT_LE(value);
#ifdef CONFIG_SRAM_DEBUG
    cpssOsPrintf("WR base=%p regAddr=0x%08x data=0x%08x\n",(void*)regsBase,regAddr,value);
#endif
}
static void mv_reg_write_field(GT_UINTPTR regsBase, GT_U32 regAddr, GT_U32 mask, GT_U32 value)
{
    GT_U32 val;
    val = mv_reg_read(regsBase, regAddr);
    val &= ~mask;
    val |= value;
    mv_reg_write(regsBase, regAddr, val);
}

static GT_STATUS mv_configure_sram(
    GT_UINTPTR regsBase,
    GT_U32     sramPhys,
    GT_U32     sramSize
)
{
    GT_U32     i, data;
    GT_U32     waySize = _64K;
    GT_U32     waysNo = 8;
    GT_U32     waysMask = 0xff;
    GT_U32     L2Size = _512K;


    if ((sramSize % _64K) != 0)
        return 1;

    /* Enable access to L2 configuration registers */
    /*reset bit 12 in MSYS_CIB_CONTROL_REG 0x20280 */
    mv_reg_write_field(regsBase, 0x20280, 0x00001000, 0x00000000);

    /* enable L2 cache and make L2 Auxiliary Control Register enable */
    mv_reg_write_field(regsBase, 0x8100, 0x00000001, 1);
#ifdef CONFIG_SRAM_DEBUG
    cpssOsPrintf("  Configuring SRAM. base=%p...\n",(void*)regsBase);

    /* L2 Control Register */
    data = mv_reg_read(regsBase, 0x8100);
    cpssOsPrintf("L2 Control Register=0x%08x\n", data);
#endif
    /* L2 Auxiliary Control Register */
    data = mv_reg_read(regsBase, 0x8104);
#ifdef CONFIG_SRAM_DEBUG
    cpssOsPrintf("L2 Auxiliary Control Register=0x%08x\n", data);
#endif
    if (data != 0)
    {
        /* bits 11:10 are L2 cache size 0-256K,1-512K,2-1M,3-2M */
        L2Size = _256K << BITS(data,11,10);
        /* bits 16:13 are cache way associative */
        if (BITS(data,16,13) == 3) {
            waysNo = 4;
            waysMask = 0x0f;
        }
        if (BITS(data,16,13) == 7) {
            waysNo = 8;
            waysMask = 0xff;
        }
        if (BITS(data,16,13) == 11) {
            waysNo = 16;
            waysMask = 0xffff;
        }
        if (BITS(data,16,13) == 15) {
            waysNo = 32;
            waysMask = 0xffffffff;
        }

        /* bits 19:17 are way size 2-16K,3-32K,4-64K,7-512K */
        waySize = _4K << BITS(data,19,17);
#ifdef CONFIG_SRAM_DEBUG
        cpssOsPrintf("\tL2Size=0x%x\n\twaysNo=%d, waySize=0x%x\n",L2Size,waysNo,waySize);
#endif
    }

    /* check params */
    if (sramSize > L2Size)
        return 2;
    if ((sramSize % waySize) != 0)
        return 2;
    if ((sramSize / waySize) > waysNo)
        return 2;


    /*write 0x01000102 to Coherency Fabric Control Register 0x20200:
     * bit25: snoop cpu1 enable == 0
     * bit24: snoop cpu0 enable == 0
     * bit8: MBUS Error Propagation == 1 (propagated on read)
     */
    mv_reg_write(regsBase, 0x20200, 0x00000102);
    /* CPU0 Configuration Register 0x21800
     * bit16: Shared L2 present
     */
    mv_reg_write_field(regsBase, 0x21800, 0x00010000, 0x00000000);

    /*******************/
    /* sram allocation */
    /*******************/

    /* 1. lockdown way i for all masters
     *   L2 CPU Data Lockdown
     *     0x8900, 0x8908   set bit i to 1
     *   L2 CPU Instruction Lockdown
     *     0x8904, 0x890c   set bit i to 1
     *   IO Bridge Lockdown
     *     0x8984 set bit i to 1
     */
    mv_reg_write_field(regsBase, 0x8900, waysMask, waysMask);
    mv_reg_write_field(regsBase, 0x8908, waysMask, waysMask);
    mv_reg_write_field(regsBase, 0x8904, waysMask, waysMask);
    mv_reg_write_field(regsBase, 0x890c, waysMask, waysMask);
    mv_reg_write_field(regsBase, 0x8984, waysMask, waysMask);

    /* 2. Trigger an Allocation Block command
     *   Set <Allocation Way ID> to be i
     *     0x878c bits 4:0 is way (0..32)
     *   Set <Allocation Data> to Disable (0x0)
     *     0x878c bits 7:6
     *   Set <Allocation Atomicity> to Foreground (0x0)
     *     0x878c bit 8
     *   Set <Allocation Base Address> to be saddr[31:10]
     *     0x878c bits 31:10
     */
    for (i = 0; i < waysNo; i++)
    {
        GT_U32 offset = i * waySize;
        if (offset >= sramSize)
            break;
        data = 0;
        data |= i & 0x1f; /* Allocation Way ID bits 4:0 */
        data |= sramPhys + offset; /* bits 31:10 */
        /* Allocation Data == Disable (0) */
        /* Allocation Atomicity == Foreground (0) */
        mv_reg_write(regsBase, 0x878c, data);
    }

    /* L2 Control register: disable L2 */
    mv_reg_write_field(regsBase, 0x8100, 0x00000001, 0);

    /* 3. Configure one the SRAM windows SRAM Window n Control Register (n=0–3)
     * (Table 346 p. 580) to direct the required range to be an SRAM:
     *   0x20240 + n*4
     * Set Base to be saddr[31:16]
     * Set Size to 64KB (0x0)
     *   bits 10:8  0==64K, 1==128K, 3=256K, 7=512K
     * Set WinEn to True
     *   bit 0 to 1
     */
    for (i = 0; i < 4; i++)
    {
        GT_U32 offset = i * _512K;
        if (offset < sramSize)
        {
            data = sramPhys + offset; /* base, bits 31:16 */
            data |= 0x0700; /* bits 10:8 == 7 == 512K */
            data |= 0x1; /* enable */
            mv_reg_write(regsBase, 0x20240+i*4, data);
        } else {
            /* disable */
            mv_reg_write(regsBase, 0x20240+i*4, 0);
        }
    }

    /* Disable window 13 used by BootROM */
    mv_reg_write(regsBase, 0x200b8, 0);

    return 0;
}
#endif /* DO_CONFIGURE_SRAM */
