/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsServiceCpuFw.c
*
* DESCRIPTION:
*           This file contains HWS Firmware loading support
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <mvHwsPortApInitIf.h>
#include <mvHwsIpcApis.h>
#include <mvHwsPortInitIf.h>
#include <mvHwsPortCtrlApInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <mvHwsIpcDefs.h>

/* TODO:
 * Before switch to userspace IPC (cpss/generic/srvCpu/ *):
 * 1. Implement CM3 in userspace
 * 2. CPSS_USERSPACE_IPC will use CPSS functions regardless
 *    of PP type (BC2/BobK)
 * 3. Implement interrupt handling for user space IPC
 */
#define CPSS_USERSPACE_IPC

#ifdef CPSS_USERSPACE_IPC
#include <cpss/generic/srvCpu/prvCpssGenericSrvCpuLoad.h>
#include <cpss/generic/srvCpu/prvCpssGenericSrvCpuIpc.h>
#endif
#include <sys/types.h>
#include <fcntl.h>

#ifndef _VISUALC
#include <sys/ioctl.h>
#include <unistd.h>
#include <mvHwsServiceCpuFwIf.h>
#ifndef CPSS_USERSPACE_IPC
#include "mvHwsServiceCpuFw.h"
#include "mvHwsServiceCpuCm3Fw.h"
#endif

/**************************** Pre-Declaration ********************************************/
#define SERVICECPU_CONFIG_ITEMS        6
#define SERVICECPU_HOSTMEM_INFO_ITEMS  4

struct servicecpu_config
{
    unsigned int items[SERVICECPU_CONFIG_ITEMS];
};

struct servicecpu_hostmem_info
{
    unsigned int items[SERVICECPU_HOSTMEM_INFO_ITEMS];
};

struct ipcchn_set
{
    unsigned int chn_id;
    int          chn_en;
    char         *dev_name;
};

enum cm3_state
{
    D_RESET,
    D_UNRESET,
    D_UNRESET_AND_PROTECT_MEM
};

/**************************** Definition **************************************/
#define SERVICECPU_IOC_MAGIC        's'
#define SERVICECPU_IOC_UNRESET      _IOW(SERVICECPU_IOC_MAGIC, 1, unsigned int)
#define SERVICECPU_IOC_SENDIRQ      _IO(SERVICECPU_IOC_MAGIC, 2)
#define SERVICECPU_IOC_CONFIG       _IOW(SERVICECPU_IOC_MAGIC, 3, struct servicecpu_config)
#define SERVICECPU_IOC_MMAPINFO     _IOR(SERVICECPU_IOC_MAGIC, 4, struct servicecpu_hostmem_info)
#define SERVICECPU_IOC_SET_IPCCHN   _IOW(SERVICECPU_IOC_MAGIC, 5, struct ipcchn_set)

#define IPCW_IOC_MAGIC              'I'
#define IPCW_IOC_GET_STAT           _IOR(IPCW_IOC_MAGIC, 0, unsigned int)

#define CM3_IOC_MAGIC                'c'
#define CM3_IOC_SETMEM_TYPE         _IOW(CM3_IOC_MAGIC, 0, unsigned int)
#define CM3_IOC_UNRESET             _IOW(CM3_IOC_MAGIC, 1, enum cm3_state)
#define CM3_IOC_SENDIRQ             _IOW(CM3_IOC_MAGIC, 2, unsigned int)
#define CM3_IOC_READ                _IOW(CM3_IOC_MAGIC, 3, struct cm3_filep)
#define CM3_IOC_WRITE               _IOW(CM3_IOC_MAGIC, 4, struct cm3_filep)
#define CM3_IOC_SET_IPCCHN          _IOWR(CM3_IOC_MAGIC, 5, struct ipcchn_set)

#undef MV_FW_DEBUG
#ifdef MV_FW_DEBUG
#define fw_debug osPrintf
#else
#define fw_debug(...)
#endif /* MV_IPC_DEBUG */

/**************************** Globals *****************************************/
int mvHwsServiceCpuFd;
int mvHwsServiceCpuChnlFd[MV_HWS_IPC_CH_NUM];
#endif /* _VISUALC */


GT_U32 mvHwsServiceCpuEnabled[HWS_MAX_DEVICE_NUM] = {0};

/* loading FW from NFS variables */
GT_BOOL loadFromNfs = GT_FALSE;
GT_U32 nfsFwSize;

/**************************** Functions *****************************************/
extern void mvHwsAvagoAaplAddrGet(unsigned char devNum, unsigned int **devAddr,
                                  unsigned int *devSize);

extern void mvHwsApPortCtrlDebugParamsSet(GT_U32 apFwLogBaseAddr, GT_U32 apFwLogCountAddr,
                                          GT_U32 apFwLogPointerAddr, GT_U32 apFwLogResetAddr);

/*******************************************************************************
* mvHwsServiceCpuEnable
*
* DESCRIPTION:
*       Service CPU Enable for device
*
* INPUTS:
*       devNum - device number
*       enable - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuEnable(GT_U8 devNum, GT_U32 enable)
{
    mvHwsServiceCpuEnabled[devNum] = enable;

    return GT_OK;
}

/*******************************************************************************
* mvHwsServiceCpuEnableGet
*
* DESCRIPTION:
*       Return Service CPU Enable state
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_U32 mvHwsServiceCpuEnableGet(GT_U8 devNum)
{
    return (mvHwsServiceCpuEnabled[devNum]);
}

#if !defined(_VISUALC) && defined(CPSS_USERSPACE_IPC)
/*******************************************************************************
* mvHwsServiceCpuFwLoad
*
* DESCRIPTION:
*       Service CPU FW Load
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuFwLoad(GT_U8 devNum)
{
#ifdef CPSS_USERSPACE_IPC
    return prvCpssGenericSrvCpuLoad(devNum);
#else /* !defined(CPSS_USERSPACE_IPC) */
    GT_STATUS rc;
#ifdef CPSS_BLOB
    rc = GT_NOT_IMPLEMENTED;
#else /* !defined(CPSS_BLOB) */
    ssize_t fwSizeDnld;
    unsigned char *fwBuf = NULL;
    char    *devName = NULL;
    GT_U32 fwSize = 0;

    /* by default, use embedded FW */

    /* Initialize FW parameters */
    if (hwsDeviceSpecInfo[devNum].devType == BobcatA0)
    {
        devName = "/dev/servicecpu";

        fwBuf  = mvHwsServiceCpuFw;
        fwSize = mvHwsServiceCpuFwSize;
    }
    else
    if (hwsDeviceSpecInfo[devNum].devType == BobK)
    {
        devName = "/dev/cm3";

        fwBuf  = mvHwsServiceCpuCm3Fw;
        fwSize = mvHwsServiceCpuCm3FwSize;
    }

    /* Open Service CPU Char device */
    mvHwsServiceCpuFd = open(devName, O_RDWR);
    if (mvHwsServiceCpuFd <= 0)
    {
        osPrintf("Cannot open %s device\n", devName);
        return GT_FAIL;
    }

    if(loadFromNfs)
    {
        int rfd;
        char* name = "/usr/bin/RTOSDemo.bin";

        osPrintf("Loading FW from NFS (FW size is %d)\n", nfsFwSize);

        /* open file with firmware */
        rfd = open(name, O_RDONLY);
        if (rfd <= 0)
        {
            osPrintf("Cannot open %s file.\n", name);
            return GT_FAIL;
        }

        /* allocate buffer */
        fwBuf = hwsOsMallocFuncPtr(nfsFwSize);
        if (fwBuf == NULL)
        {
            osPrintf("Cannot malloc FW buffer.\n");
            return GT_NO_RESOURCE;
        }

        /* read FW from NFSr */
        if (read(rfd, (void*)fwBuf, (ssize_t)nfsFwSize) < 0)
        {
            osPrintf("Cannot read from %s.\n", name);
            return GT_FAIL;
        }

        fwSize = nfsFwSize;
    }

    /* Load Service CPU Char device */
    if (lseek(mvHwsServiceCpuFd, 0, SEEK_SET) < 0)
    {
        return GT_FAIL;
    }

    fwSizeDnld = write(mvHwsServiceCpuFd, fwBuf, (ssize_t)fwSize);
    if (fwSizeDnld < 0)
    {
        osPrintf("Cannot write to %s\n", devName);
        return GT_FAIL;
    }

    if(loadFromNfs)
    {
        hwsOsFreeFuncPtr(fwBuf);
    }

    fw_debug("Write %d bytes to %s\n", fwSizeDnld, devName);

    rc = GT_OK;
#endif /* !defined(CPSS_BLOB) */
    return rc;
#endif /* !defined(CPSS_USERSPACE_IPC) */
}

/*******************************************************************************
* mvHwsServiceCpuFwActivate
*
* DESCRIPTION:
*       None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuFwActivate(GT_U8 devNum)
{
#ifdef CPSS_USERSPACE_IPC
    return prvCpssGenericSrvCpuStart(devNum);
#else /* !defined(CPSS_USERSPACE_IPC) */
    GT_STATUS rc;
#ifdef CPSS_BLOB
    rc = GT_NOT_IMPLEMENTED;
#else
    int rcode;
    unsigned int reset;

    if (hwsDeviceSpecInfo[devNum].devType == BobcatA0)
    {
        reset = 1;

        /* Start Service CPU Execution */
        rcode = ioctl(mvHwsServiceCpuFd, SERVICECPU_IOC_UNRESET, &reset);
        if (rcode < 0)
        {
            osPrintf("IOCTL Reset command failed\n");
            return GT_FAIL;
        }
    }
    else if (hwsDeviceSpecInfo[devNum].devType == BobK)
    {
        reset = D_UNRESET;

        /* Start Service CPU Execution */
        rcode = ioctl(mvHwsServiceCpuFd, CM3_IOC_UNRESET, &reset);
        if (rcode < 0)
        {
            osPrintf("IOCTL Reset command failed\n");
            return GT_FAIL;
        }
    }

    rc = GT_OK;
#endif /* !defined(CPSS_BLOB) */
    return rc;
#endif /* !defined(CPSS_USERSPACE_IPC) */
}

/*******************************************************************************
* mvHwsServiceCpuIpcCfg
*
* DESCRIPTION:
*       Service CPU FW Load
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuIpcCfg(GT_U8 devNum)
{
#ifdef CPSS_USERSPACE_IPC
    return prvCpssGenericSrvCpuIpcInit(devNum);
#else /* !defined(CPSS_USERSPACE_IPC) */
    GT_STATUS rc;
#ifdef CPSS_BLOB
    rc = GT_NOT_IMPLEMENTED;
#else
    int rcode;
    int chIdx;
    char chn_name[32];
    struct ipcchn_set chnl;
    int chArr[] =
    {
        MV_HWS_IPC_HI_PRI_RX_CH_ID,
        MV_HWS_IPC_LO_PRI_RX_CH_ID,
        MV_HWS_IPC_TX_0_CH_ID,
        MV_HWS_IPC_TX_1_CH_ID,
        MV_HWS_IPC_TX_2_CH_ID,
        MV_HWS_IPC_TX_3_CH_ID
    };

    for (chIdx = 0; chIdx < MV_HWS_IPC_CH_NUM; chIdx++)
    {
        chnl.chn_id   = chArr[chIdx];
        chnl.chn_en   = 1;
        chnl.dev_name = chn_name;

        if (hwsDeviceSpecInfo[devNum].devType == BobcatA0)
        {
            fw_debug("Enable channel %d\n", chArr[chIdx]);
            rcode = ioctl(mvHwsServiceCpuFd, SERVICECPU_IOC_SET_IPCCHN, &chnl);
            if (rcode < 0)
            {
                osPrintf("IOCTL Set channel %d failed\n", chArr[chIdx]);
                return GT_FAIL;
            }
        }
        else if (hwsDeviceSpecInfo[devNum].devType == BobK)
        {
            fw_debug("Enable channel %d\n", chArr[chIdx]);
            rcode = ioctl(mvHwsServiceCpuFd, CM3_IOC_SET_IPCCHN, &chnl);
            if (rcode < 0)
            {
                osPrintf("IOCTL Set channel %d failed\n", chArr[chIdx]);
                return GT_FAIL;
            }
        }

        fw_debug("Open channel %d %s\n", chArr[chIdx], chnl.dev_name);
        mvHwsServiceCpuChnlFd[chArr[chIdx]] = open(chnl.dev_name, O_RDWR);
        if (mvHwsServiceCpuChnlFd[chArr[chIdx]] < 0)
        {
            osPrintf("Failed to open %s\n", chnl.dev_name);
            return GT_FAIL;
        }
    }

    rc = GT_OK;
#endif /* !defined(CPSS_BLOB) */
    return rc;
#endif /* !defined(CPSS_USERSPACE_IPC) */
}
#endif /* !defined(_VISUALC) && defined(CPSS_USERSPACE_IPC) */

/*******************************************************************************
* mvHwsServiceCpuFwInit
*
* DESCRIPTION:
*       HW Services Firmware initialization
*       Firmware loading, IPC channel configuration, and Activate (take out of reset)
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuFwInit
(
    GT_U8 devNum
)
{
#if !defined(_VISUALC) && defined(CPSS_USERSPACE_IPC)
    if (mvHwsServiceCpuFwLoad(devNum) != GT_OK)
        return GT_FAIL;

    if (mvHwsServiceCpuFwActivate(devNum) != GT_OK)
        return GT_FAIL;

    if (mvHwsServiceCpuIpcCfg(devNum) != GT_OK)
        return GT_FAIL;

    mvHwsIpcDbInit(devNum);

#else
    devNum = devNum;
#endif /* !defined(_VISUALC) && defined(CPSS_USERSPACE_IPC) */

    return GT_OK;
}

/*******************************************************************************
* mvHwsServiceCpuFwPostInit
*
* DESCRIPTION:
*       Init HWS Firmware After Avago Serdes init
*
* INPUTS:
*       devNum    - system device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuFwPostInit(GT_U8 devNum)
{
    GT_U32       serviceCpuAvagoCfgAddr;
    unsigned int *hostAvagoCfgAddr = 0;
    unsigned int hostAvagoCfgSize = 0;
    MV_HWS_AP_PORT_DEBUG apDebug = {0,0,0,0,0};

    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if ((hwsDeviceSpecInfo[devNum].devType == BobK) &&
        (mvHwsServiceCpuEnableGet(devNum)))
    {
        /* In BOBK-CM3 the Host is responsible to load Avago Firmware and init AAPL structure
        ** Once initialized the Host should load Host AAPL structure instance to the CM3 AAPL
        ** structure instance
        ** First step is to get CM3 AAPL structure base address in CM3 'firmware section"
        ** Second step is to write into the CM3 'firmware section" AAPL structure base address
        ** the Host AAPL structure contect
        */
        if (mvHwsPortAvagoCfgAddrGetIpc(devNum, 0, 0, &serviceCpuAvagoCfgAddr) == GT_OK)
        {
#ifndef ASIC_SIMULATION
            mvHwsAvagoAaplAddrGet(devNum, &hostAvagoCfgAddr, &hostAvagoCfgSize);
#endif
            mvHwsServiceCpuWrite(devNum, (unsigned int)serviceCpuAvagoCfgAddr, hostAvagoCfgAddr, hostAvagoCfgSize);
        }
        else
        {
            osPrintf("mvHwsServiceCpuFwPostInit Failed to load Avago AAPL\n");
        }
    }

    /* In order provide access to Service CPU real-time log
    ** The Host is required to receive the LOG parameters addresses.
    ** Once received he, the LOG can be access from Host
    */
    if (mvHwsApPortCtrlDebugGet(devNum, 0, 0, &apDebug) == GT_OK)
    {
        mvHwsApPortCtrlDebugParamsSet(apDebug.apFwLogBaseAddr    - apDebug.apFwBaseAddr,
                                      apDebug.apFwLogCountAddr   - apDebug.apFwBaseAddr,
                                      apDebug.apFwLogPointerAddr - apDebug.apFwBaseAddr,
                                      apDebug.apFwLogResetAddr   - apDebug.apFwBaseAddr);
    }
    else
    {
        osPrintf("mvHwsServiceCpuFwPostInit Failed to retrive LOG parameters\n");
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsServiceCpuMsgSend
*
* DESCRIPTION:
*       Send message to Service CPU via IPC channel
*
* INPUTS:
*       channel - transmit channel to send the message
*       msg - message buffer to transmit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuMsgSend
(
    unsigned char devNum,
    unsigned int channel,
    char* msg
)
{
#ifdef CPSS_USERSPACE_IPC
    return prvCpssGenericSrvCpuIpcMessageSend(
            devNum, channel,
            (GT_U8*)msg, sizeof(MV_HWS_IPC_CTRL_MSG_STRUCT));
#else /* !defined(CPSS_USERSPACE_IPC) */
    GT_STATUS rc;
#ifndef _VISUALC
#ifdef CPSS_BLOB
    rc = GT_NOT_IMPLEMENTED;
#else
    int status;
    int numOfBytes;
    int msgSize = sizeof(MV_HWS_IPC_CTRL_MSG_STRUCT);

    devNum = devNum;

    /* Validate target channel is attached */
    if ((ioctl(mvHwsServiceCpuChnlFd[channel], IPCW_IOC_GET_STAT, &status)) < 0)
    {
        return GT_FAIL;
    }
    else
    {
        if (status == 1)
        {
            fw_debug("Channel-%d attached\n", channel);
        }
        else
        {
            osPrintf("Channel-%d Not attached\n", channel);
            return GT_OK;
        }
    }

    /* Write the message to the channel char device */
    numOfBytes = write(mvHwsServiceCpuChnlFd[channel], msg, msgSize);
    if (numOfBytes != msgSize)
    {
        osPrintf("Failed to write to channel %d msgSize %d numOfBytes %d (Max message size shold be <= %d)\n",
                 channel, msgSize, numOfBytes, MV_HWS_MAX_IPC_CTRL_MSG_LENGTH);
        return GT_FAIL;
    }

    fw_debug("Send message to Service CPU on channel %d (msg size %d bytes)\n", channel, numOfBytes);
#endif /* !defined(CPSS_BLOB) */
#else
    devNum = devNum;
    channel = channel;
    msg = msg;
#endif /* _VISUALC */

    rc =  GT_OK;
    return rc;
#endif /* !defined(CPSS_USERSPACE_IPC) */
}

/*******************************************************************************
* mvHwsServiceCpuMsgRecv
*
* DESCRIPTION:
*       Receive message from Service CPU via IPC channel
*
* INPUTS:
*       channel - transmit channel to send the message
*       msg - message buffer to transmit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuMsgRecv
(
    unsigned char devNum,
    unsigned int channel,
    char* msg
)
{
#ifdef CPSS_USERSPACE_IPC
    return prvCpssGenericSrvCpuIpcMessageRecv(devNum, channel,
            (GT_U8*)msg, NULL);
#else /* !defined(CPSS_USERSPACE_IPC) */
    GT_STATUS rc;
#ifndef _VISUALC
#ifdef CPSS_BLOB
    return GT_NOT_IMPLEMENTED;
#else
    int status;
    int numOfBytes;
    int msgSize = sizeof(MV_HWS_IPC_REPLY_MSG_STRUCT);

    devNum = devNum;
    /* Validate receive channel is attached */
    if ((ioctl(mvHwsServiceCpuChnlFd[channel], IPCW_IOC_GET_STAT, &status)) < 0)
    {
        return GT_FAIL;
    }
    else
    {
        if (status == 1)
        {
            fw_debug("Channel-%d atached\n", channel);
        }
        else
        {
            osPrintf("Channel-%d Not atached\n", channel);
            return GT_OK;
        }
    }

    /* Read message to the channel char device */
    numOfBytes = read(mvHwsServiceCpuChnlFd[channel], msg, msgSize);
    if (numOfBytes != msgSize)
    {
        osPrintf("Failed to read from channel %d (msg size %d bytes)\n", channel, numOfBytes);
        return GT_FAIL;
    }
#endif /* !defined(CPSS_BLOB) */
#else
    devNum = devNum;
    channel = channel;
    msg = msg;
#endif /* _VISUALC */

    rc = GT_OK;
    return rc;
#endif /* !defined(CPSS_USERSPACE_IPC) */
}

/*******************************************************************************
* mvHwsServiceCpuRead
*
* DESCRIPTION:
*       Read from Service CPU "firmware section"
*
* INPUTS:
*       offset - address, offset from "firmware" base address (0xFFF80000)
*       data   - value
*       size   - size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuRead(unsigned char devNum, unsigned int offset, unsigned int *data, unsigned int size)
{
/*TODO: design issue*/
#ifdef CPSS_USERSPACE_IPC
    return prvCpssGenericSrvCpuRead(devNum, offset, data, size);
#else /* !defined(CPSS_USERSPACE_IPC) */
    GT_STATUS rc;
#if defined(ASIC_SIMULATION) || defined(CPSS_BLOB)
    /* avoid warnings */
    devNum = devNum;
    offset = offset;
    data   = data;
    size   = size;
#else
    int rcode;

    devNum = devNum;

    if (lseek(mvHwsServiceCpuFd, offset, SEEK_SET) < 0)
    {
        return GT_FAIL;
    }

    rcode = read(mvHwsServiceCpuFd, (char*)data, (ssize_t)size);
    if (rcode < 0)
    {
        osPrintf("Cannot read offset %x\n", offset);
        return GT_FAIL;
    }

#endif /* ASIC_SIMULATION */

    rc = GT_OK;
    return rc;
#endif /* !defined(CPSS_USERSPACE_IPC) */
}

/*******************************************************************************
* mvHwsServiceCpuWrite
*
* DESCRIPTION:
*       Write to Service CPU "firmware section"
*
* INPUTS:
*       offset - register address, offset from "firmware" base address (0xFFF80000)
*       data   - register value
*       size   - register size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuWrite(unsigned char devNum, unsigned int offset, unsigned int *data, unsigned int size)
{
/*TODO: design issue*/
#ifdef CPSS_USERSPACE_IPC
    return prvCpssGenericSrvCpuWrite(devNum, offset, data, size);
#else /* !defined(CPSS_USERSPACE_IPC) */
    GT_STATUS rc;
#if defined(ASIC_SIMULATION) || defined(CPSS_BLOB)
    /* avoid warnings */
    devNum = devNum;
    offset = offset;
    data   = data;
    size   = size;
#else
    int rcode;

    devNum = devNum;

    if (lseek(mvHwsServiceCpuFd, offset, SEEK_SET) < 0)
    {
        return GT_FAIL;
    }

    rcode = write(mvHwsServiceCpuFd, (char*)data, (ssize_t)size);
    if (rcode < 0)
    {
        osPrintf("Cannot read offset %x\n", offset);
        return GT_FAIL;
    }
#endif /* ASIC_SIMULATION */

    rc = GT_OK;
    return rc;
#endif /* !defined(CPSS_USERSPACE_IPC) */
}

/*******************************************************************************
* mvHwsServiceCpuEnableNfsLoad
*
* DESCRIPTION:
*       Debug function which Enables FW loading from NFS
*
* INPUTS:
*       enable - enable/disable NFS FW loading
*       fwSize - FW (binary) size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuEnableNfsLoad(GT_BOOL enable, GT_U32 fwSize)
{
    osPrintf("%s FW load from NFS\n", (enable) ? "Enable" : "Disable");
    loadFromNfs = enable;
    nfsFwSize = fwSize;

    return GT_OK;
}

