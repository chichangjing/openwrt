/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssMirrorTypes.h
*
* DESCRIPTION:
*       Global definitions for mirror API functions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __prvCpssMirrorTypesh
#define __prvCpssMirrorTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/*max tx mirror ports limited to 8 */
#define PRV_CPSS_MIRROR_MAX_PORTS_CNS  8    


/* macro to get device number number from Tx source sniffer field.
    entry1 - data
*/
#define PRV_CPSS_MIRROR_TX_SRC_SNIFFER_REG_DEV_GET_MAC(entry1) \
    ( (entry1 & 0x7F) >> 1 )

/* macro to get port number from Tx source sniffer field.
    entry1 - data
*/
#define PRV_CPSS_MIRROR_TX_SRC_SNIFFER_REG_PORT_GET_MAC(entry1) \
    ( (entry1 & 0x3F00) >> 8 )

/* macro to get enable bit from Tx source sniffer field.
    entry1 - data
*/
#define PRV_CPSS_MIRROR_TX_SRC_SNIFFER_REG_ENB_BIT_MAC(entry1) ((entry1) & 0x1) 


/* Macro to Convert analyzer port and device to null port and device.
    _analyzDev - Analyzer device
    _analyzPort - Analyzer port  
    _ownDevice - Owner device
    _devNullPort - port
*/
#define PRV_CPSS_MIRROR_CONVERT_ANALYZER_FOR_CPU_OR_NULL_PORT_MAC(             \
                       _analyzDev, _analyzPort, _ownDevice, _devNullPort)      \
{                                                                              \
    if (_analyzPort == CPSS_CPU_PORT_NUM_CNS)                                  \
    {                                                                          \
        _analyzDev = _ownDevice;                                               \
    }                                                                          \
    else                                                                       \
    if (_analyzPort == CPSS_NULL_PORT_NUM_CNS)                                 \
    {                                                                          \
        _analyzDev = _ownDevice;                                               \
        _analyzPort = _devNullPort;                                            \
    }                                                                          \
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssMirrorTypesh */

