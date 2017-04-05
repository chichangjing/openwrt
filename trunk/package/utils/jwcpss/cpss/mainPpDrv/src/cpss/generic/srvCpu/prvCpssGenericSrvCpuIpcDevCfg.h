/*******************************************************************************
*              (c), Copyright 2015, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenericSrvCpuIpcDevCfg.h
*
* DESCRIPTION:
*       Service CPU IPC per-device config
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvCpssGenericSrvCpuIpcDevCfg_h__
#define __prvCpssGenericSrvCpuIpcDevCfg_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/srvCpu/prvCpssGenericSrvCpuIpc.h>
#include "mv_ipc/mv_ipc.h"

typedef struct PRV_CPSS_IPC_DEV_CFG_STCT {
    void    *sramBase;
    void    *fwAddress;
    void    *shmAddress;
    GT_BOOL linkConfigured;
    unsigned char link_id;
    struct ipc_link_info link_cfg;
} PRV_CPSS_IPC_DEV_CFG_STC;
extern PRV_CPSS_IPC_DEV_CFG_STC *prvIpcDevCfg[PRV_CPSS_MAX_PP_DEVICES_CNS];



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssGenericSrvCpuIpcDevCfg_h__ */
