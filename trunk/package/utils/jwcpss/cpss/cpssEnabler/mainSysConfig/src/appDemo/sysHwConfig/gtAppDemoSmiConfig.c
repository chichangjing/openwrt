/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppDemoSmiConfig.c
*
* DESCRIPTION:
*       Prestera Devices smi initialization & detection module.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#include <appDemo/sysHwConfig/gtAppDemoSmiConfig.h>
#include <gtExtDrv/drivers/gtSmiHwCtrl.h>

#include <cpss/generic/cpssTypes.h>


#ifndef PRESTERA_NO_HW
#include <appDemo/sysHwConfig/gtAppDemoPciConfig.h>
#endif

/*******************************************************************************
* internal definitions
*******************************************************************************/
#ifdef PRESTERA_DEBUG
#define APP_INIT_DEBUG
#endif

#ifdef APP_INIT_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif


#define DEV_ID(gtDevice) ((gtDevice)>>16)
/*******************************************************************************
* Internal usage environment parameters
*******************************************************************************/
static GT_SMI_DEV_VENDOR_ID device[] =
{
    /* Salsa devices */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX240_CNS)},  /* 24 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX160_CNS)},  /* 16 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX120_CNS)},  /* 12 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX241_CNS)},  /* 24 + NO Mngment Dev  */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX161_CNS)},  /* 16 + NO Mngment Dev  */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX121_CNS)},  /* 12 + NO Mngment Dev  */

    /* cheetah devices */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX270_CNS)},  /* 24 + 3XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX260_CNS)},  /* 24 + 2XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX250_CNS)},  /* 24 + Mngment Dev     */

    /* cheetah+ devices */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX273_CNS)},  /* 24 + 3XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX263_CNS)},  /* 24 + 2XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX253_CNS)},  /* 24 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX243_CNS)},  /* 24 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX106_CNS)},  /* 10 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX107_CNS)},  /* 10 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX133_CNS)},  /* 12 + 1XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX167_CNS)},  /* 16 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX163_CNS)},  /* 16 + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX166_CNS)},  /* 16 + Mngment Dev     */

    /* cheetah REDUCED devices */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX262_CNS)},  /* 24 + 2XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX268_CNS)},  /* 24 + 2XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX248_CNS)},  /* 24 + Mngment Dev     */

    /* cheetah 3 devices */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5128_CNS)},  /* 24 + 4XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5128_1_CNS)},/* 24 + 4XG + Mngment Dev     */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX8110_CNS)},  /* 10XG(4XG+6 Stack) + Mngment Dev */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5124_CNS)},  /* 24 port Tri-speed Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5126_CNS)},  /* 24 port Tri-speed Gigabit Ethernet and 2 port 10 Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5127_CNS)},  /* 24 port Tri-speed Gigabit Ethernet and 2 port 10/16/20Gbps HGS2 (HyperG.Stacks) Multi-layer Ethernet Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5129_CNS)},  /* 24 port Tri-speed Gigabit Ethernet and 2 port 10 Gigabit Ethernet plus 2 port 10/16/20Gbps HGS2 (HyperG.Stacks) Multi-layer Ethernet Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5151_CNS)},  /* 4 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5151_1_CNS)},/* 4 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5152_CNS)},  /* 8 port 1/2.5 Gigabit Ethernet and 2  port 10 Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5152_1_CNS)},/* 8 port 1/2.5 Gigabit Ethernet and 2  port 10 Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5154_CNS)},  /* 16 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet Multi-layer switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5154_1_CNS)},/* 16 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet Multi-layer switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5155_CNS)},  /* 16 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet plus 2 port 10/16/20Gbps HGS2 (HyperG.Stacks) Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5155_1_CNS)},/* 16 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet plus 2 port 10/16/20Gbps HGS2 (HyperG.Stacks) Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5156_CNS)},  /* 24 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet Multi-layer switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5156_1_CNS)},/* 24 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet Multi-layer switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5157_CNS)},  /* 24 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet plus 2 port 10/16/20Gbps HGS2 (HyperG.Stacks) Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX5157_1_CNS)},/* 24 port 1/2.5 Gigabit Ethernet and 2 port 10 Gigabit Ethernet plus 2 port 10/16/20Gbps HGS2 (HyperG.Stacks) Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX8110_1_CNS)},/* 10 port 10 Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX8108_CNS)},  /* 8 port 10 Gigabit Ethernet Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX8109_CNS)},  /* 6 port 10 Gigabit Ethernet plus 2 port 10/16/20Gbps HGS2 (HyperG.Stacks) Multi-layer Switch */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX8111_CNS)},  /* 10XG(4XG+6 Stack) + Mngment Dev */

    /* xcat devices */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX4121_CNS)},  /*    */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98DX3122_CNS)},  /*    */

    /* lion2 Hooper devices */
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98CX8121_CNS)},
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98CX8123_CNS)},
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98CX8124_CNS)},
    {GT_SMI_VENDOR_ID,DEV_ID(CPSS_98CX8129_CNS)},
	
    {0,0}
};

#ifndef PRESTERA_NO_HW

/*******************************************************************************
* gtPresteraGetSmiDev
*
* DESCRIPTION:
*       This routine searches for Prestera Devices Over the SMI.
*
* INPUTS:
*       first - whether to bring the first device, if GT_FALSE return the next
*               device.
*
* OUTPUTS:
*       smiInfo <- the next device SMI info.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*       GT_NO_MORE - no more prestera devices.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetSmiDev
(
    IN  GT_BOOL     first,
    OUT GT_SMI_INFO *smiInfo
)
{
    static GT_U32   deviceIdx = 0;      /* device id index      */
    GT_U32          i;                  /* device instance      */
    GT_U16          vendorId;
    GT_U16          devId;
    GT_U16          devNum;

    /* check parameters */
    if(smiInfo == NULL)
    {
        return GT_BAD_PARAM;
    }
    /* check whether first call */
    if(first == GT_TRUE)
    {
		/* Hooper devices connected to 0-3 */
        deviceIdx = 0x0;
    }
    /* call the BSP SMI facility to get the next Prestera device */
    for (i = deviceIdx; i < 32; i++)
    {
        if(extDrvSmiDevVendorIdGet(&vendorId,
                                   &devId,
                                   i) != GT_OK)
        {
            continue;
        }

        /* a device has been found, search in the device list */
        /* if it exists add it to the device list             */
        for (devNum = 0; device[devNum].vendorId != 0 ; devNum++)
        {
            if (device[devNum].vendorId == vendorId &&
                device[devNum].devId == devId)
            {
                DBG_INFO(("found the SMI device 0x%04x\n", devId));

                smiInfo->smiDevVendorId.devId       = devId;
                smiInfo->smiDevVendorId.vendorId    = vendorId;
                smiInfo->smiIdSel                   = i;

                deviceIdx = i + 1;

                return GT_OK;
            }
        }
    }
    return GT_NO_MORE;
}

#else


/*******************************************************************************
* gtPresteraGetSmiDev
*
* DESCRIPTION:
*       This routine search for Prestera Devices Over the SMI.
*
* INPUTS:
*       first - whether to bring the first device, if GT_FALSE return the next
*               device.
*
* OUTPUTS:
*       pciInfo - the next device SMI info.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*       GT_NO_MORE - no more prestera devices.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtPresteraGetSmiDev
(
    IN  GT_BOOL     first,
    OUT GT_SMI_INFO *smiInfo
)
{
    static GT_U32 instance;

    /* check parameters */
    if(smiInfo == NULL)
    {
        return GT_BAD_PARAM;
    }
    if(first == GT_TRUE)
    {
        instance = 0;
    }
    if(i >= 2)
    {
        return GT_NO_MORE;
    }
    i++;
    smiInfo->smiDevVendorId.devId = device[0].devId;
    smiInfo->smiDevVendorId.vendorId = device[0].vendorId;

    return GT_OK;
}

#endif

