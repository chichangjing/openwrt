/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDrvPpDefs.h
*
* DESCRIPTION:
*       Includes driver level structs needed for implementing HW related
*       functions, and interrupt handling. And common definitions for driver
*       level communication.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#ifndef __prvCpssDrvPpDefsh
#define __prvCpssDrvPpDefsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/prvCpssDrvErrataMng.h>

/* constant to state that the parameter with this value need to be ignored
    This meaning is given only for fields that explicitly stated to support it */
#define PRV_CPSS_DRV_PARAM_NOT_USED_CNS 0xFFFFFFFF

/* bmp for hw interfaces */
#define PRV_CPSS_DRV_HW_IF_PCI_BMP_CNS      (1 << CPSS_CHANNEL_PCI_E)
#define PRV_CPSS_DRV_HW_IF_SMI_BMP_CNS      (1 << CPSS_CHANNEL_SMI_E)
#define PRV_CPSS_DRV_HW_IF_TWSI_BMP_CNS     (1 << CPSS_CHANNEL_TWSI_E)
#define PRV_CPSS_DRV_HW_IF_PEX_BMP_CNS      (1 << CPSS_CHANNEL_PEX_E)

/*set the HW interface bmp */
#define PRV_CPSS_DRV_HW_IF_BMP_MAC(mngInterfaceType)\
    (1 << (GT_U32)mngInterfaceType)

/* bmp of "PCI" compatible behavior */
#define PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum)\
    ((prvCpssDrvPpConfig[devNum]->drvHwIfSupportedBmp &        \
     PRV_CPSS_DRV_HW_IF_PCI_BMP_CNS) ? 1 : 0)

/* bmp of "SMI" compatible behavior */
#define PRV_CPSS_DRV_HW_IF_SMI_COMPATIBLE_MAC(devNum)\
    ((prvCpssDrvPpConfig[devNum]->drvHwIfSupportedBmp &        \
     PRV_CPSS_DRV_HW_IF_SMI_BMP_CNS) ? 1 : 0)

/* bmp of "TWSI" compatible behavior */
#define PRV_CPSS_DRV_HW_IF_TWSI_COMPATIBLE_MAC(devNum)\
    ((prvCpssDrvPpConfig[devNum]->drvHwIfSupportedBmp &        \
     PRV_CPSS_DRV_HW_IF_TWSI_BMP_CNS) ? 1 : 0)

/* bmp of "PEX" compatible behavior */
#define PRV_CPSS_DRV_HW_IF_PEX_COMPATIBLE_MAC(devNum)\
    ((prvCpssDrvPpConfig[devNum]->drvHwIfSupportedBmp &        \
     PRV_CPSS_DRV_HW_IF_PEX_BMP_CNS) ? 1 : 0)

/*
 * Typedef: struct PRV_CPSS_DRV_PORT_GROUPS_INFO_STC
 *
 * Description: A structure to hold info about the port groups of a device
 *              NOTE: for 'non multi port groups' device
 *                      isMultiPortGroupDevice = GT_FALSE
 *                      numOfPortGroups = 1 ,
 *                      activePortGroupsBmp = 1
 *                      firstActivePortGroup = 0  (CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS)
 *                      lastActivePortGroup = 0   (CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS)
 * Fields:
 *      isMultiPortGroupDevice - is the device : 'multi-port-groups' device.
 *      numOfPortGroups        - The number of port groups that the device support.
 *                     NOTE: this number not depend on the number of active port groups ,
 *                     for example this value is 4 always for Lion.
 *      activePortGroupsBmp    - Bitmap of active port groups
 *      firstActivePortGroup   - The portGroupId of the first active port group id
 *      lastActivePortGroup    - The portGroupId of the last active port group id,
 *                     Note that there can be non active port groups
 *                     between this port group and the firstActivePortGroup
 *                     used for loops and for allocations for port groups.
 *                     --> used instead of : 'num Of port groups'
 *                     dynamic allocate fields that are 'per port group' , according
 *                     to (lastValidPortGroup + 1)
 *
 *      debugForcedPortGroupId - this has debug purpose only , to 'force' specific
 *                               portGroupId      to check.
 *                               used only when != CPSS_PORT_GROUP_UNAWARE_MODE_CNS
 *                     NOTE : relevant only for multi-port-groups device.
 *
 *
*/
typedef struct
{
    GT_BOOL     isMultiPortGroupDevice;
    GT_U32      numOfPortGroups;
    GT_U32      activePortGroupsBmp;
    GT_U32      firstActivePortGroup;
    GT_U32      lastActivePortGroup;

    GT_U32      debugForcedPortGroupId;
} PRV_CPSS_DRV_PORT_GROUPS_INFO_STC;

/* macro to start a loop on active port group ids */
#define PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(_devNum, _portGroupId)   \
    for((_portGroupId) = prvCpssDrvPpConfig[_devNum]->portGroupsInfo.firstActivePortGroup;\
        (_portGroupId) <= prvCpssDrvPpConfig[_devNum]->portGroupsInfo.lastActivePortGroup;\
        (_portGroupId)++)                                                \
    {                                                                    \
        if(0 == (prvCpssDrvPpConfig[_devNum]->portGroupsInfo.activePortGroupsBmp & (1<<(_portGroupId))))\
        {                                                                \
            continue;                                                    \
        }

/* macro to end a loop on active port groups */
#define PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(_devNum, _portGroupId)    \
    }

/*
   macro to check that the given portGroupId is in range of allocated array .
   NOTE: value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is always valid
*/
#define PRV_CPSS_DRV_IS_IN_RANGE_PORT_GROUP_ID_MAC(_devNum, _portGroupId) \
    (((_portGroupId) >= CPSS_MAX_PORT_GROUPS_CNS) ? 0 : 1)

/*
   macro to check that the given portGroupId is valid .
   NOTE: value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is always valid
*/
#define PRV_CPSS_DRV_IS_VALID_PORT_GROUP_ID_MAC(_devNum,_portGroupId)      \
    (((_portGroupId) == CPSS_PORT_GROUP_UNAWARE_MODE_CNS) ? 1 :    \
     (0 == PRV_CPSS_DRV_IS_IN_RANGE_PORT_GROUP_ID_MAC(_devNum,_portGroupId)) ? 0 :            \
     (prvCpssDrvPpConfig[_devNum]->portGroupsInfo.activePortGroupsBmp & (1<<(_portGroupId))) ? 1 : \
     0)

/* check the need to force portGroupId */
#define PRV_CPSS_DRV_FORCE_PORT_GROUP_ID_CHECK_MAC(_devNum,_portGroupId)               \
    if(prvCpssDrvPpConfig[_devNum]->portGroupsInfo.debugForcedPortGroupId !=           \
       CPSS_PORT_GROUP_UNAWARE_MODE_CNS)                                                \
        _portGroupId = prvCpssDrvPpConfig[_devNum]->portGroupsInfo.debugForcedPortGroupId

/*
   macro to check that the given portGroupId is valid .
   NOTE:
    1. value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is always valid
    2. for 'non multi-port-groups' device , the portGroupId is
        set to be the 'only' port group.
*/
#define PRV_CPSS_DRV_CHECK_PORT_GROUP_ID_MAC(_devNum,_portGroupId)                   \
    if(prvCpssDrvPpConfig[_devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_TRUE)\
    {                                                                                \
        PRV_CPSS_DRV_FORCE_PORT_GROUP_ID_CHECK_MAC(_devNum,_portGroupId);            \
                                                                                     \
        if(0 == PRV_CPSS_DRV_IS_VALID_PORT_GROUP_ID_MAC(_devNum,_portGroupId))       \
        {                                                                            \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,                              \
                prvCpssLogErrorMsgPortGroupNotValid, _devNum,_portGroupId);          \
        }                                                                            \
    }                                                                                \
    else                                                                             \
        _portGroupId = prvCpssDrvPpConfig[_devNum]->portGroupsInfo.firstActivePortGroup


/*
   macro to check that the given portGroup is active , for a 'read' operation.
   macro will update the portGroupId , when portGroupId = CPSS_PORT_GROUP_UNAWARE_MODE_CNS
   or when device is non multi-port-group device.
   NOTE: value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is valid --> converted to
            prvCpssDrvPpConfig[_devNum]->portGroupsInfo.firstActivePortGroup

*/
#define PRV_CPSS_DRV_CHECK_ACTIVE_PORT_GROUP_ID_FOR_READ_OPERATION_MAC(_devNum,_portGroupId)     \
    if(prvCpssDrvPpConfig[_devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_TRUE)            \
    {                                                                                            \
        PRV_CPSS_DRV_FORCE_PORT_GROUP_ID_CHECK_MAC(_devNum,_portGroupId);                        \
                                                                                                 \
        if((_portGroupId) == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)                                   \
        {   /*use the first active port group */                                                 \
            _portGroupId = prvCpssDrvPpConfig[_devNum]->portGroupsInfo.firstActivePortGroup;     \
        }                                                                                        \
        else if(0 == PRV_CPSS_DRV_IS_IN_RANGE_PORT_GROUP_ID_MAC(_devNum,_portGroupId) ||         \
               (0 == (prvCpssDrvPpConfig[_devNum]->portGroupsInfo.activePortGroupsBmp & (1<<(_portGroupId)))))\
        {   /* non active port group */                                                          \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,                                          \
                prvCpssLogErrorMsgPortGroupNotActive, _devNum, _portGroupId);                    \
        }                                                                                        \
    }                                                                                            \
    else  /*use the only 'active port group' */                                                  \
        _portGroupId = prvCpssDrvPpConfig[_devNum]->portGroupsInfo.firstActivePortGroup

/*
 * Typedef: struct PRV_CPSS_DRIVER_PP_CONFIG_STC
 *
 * Description: Includes all driver level configuration info, for HW access,
 *              interrupt handling.
 *
 * Fields:
 *      hwDevNum   - The PP's HW device number.
 *      devType    - The PP's device type.
 *      numOfPorts - number of ports in device
 *      devFamily   - CPSS's device family that current device belongs to.
 *      devSubFamily - CPSS's device sub-family that current device belongs to.
 *      drvHwIfSupportedBmp  - bmp of supported Management interface types.
 *                          this BMP allow to support "old" hw if with it's
 *                          newer protocol version, like in the case of "PCI"
 *                          and "PEX"(PCI Express)
 *      mngInterfaceType    - the management interface SMI/PCI/TWSI/PEX
 *
 *      hwCtrl[]  - (Array of) A struct containing control fields for the hw access
 *                functions.
 *                  index in the array is the 'port group'.
 *                  for 'non-multi-port-groups' device use index CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS.
 *      drivers[]  - (Array of) Array of pointer to PP drivers
 *                  index in the array is the 'port group'.
 *                  for 'non-multi-port-groups' device use index CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS.
 *      resetAndInitControllerBase - Base address to which Reset and Init
 *                                 Controller (a.k.a. DFX server) are mapped to.
 *                                 (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
 *      resourceMapping - resource mapping info (mapping of SRAM, etc)
 *      intCtrl - Interrupt control struct.
 *      errata  - info about what Errata should be fixed in the CPSS driver for
 *                the device (depend on : revision , devType)
 *      portGroupsInfo - Info about the port groups of the device
 *                  NOTE: relevant also to the 'non multi port groups' devices
 */
typedef struct
{
    GT_HW_DEV_NUM                              hwDevNum;
    CPSS_PP_DEVICE_TYPE                        devType;
    GT_U8                                      numOfPorts;
    CPSS_PP_FAMILY_TYPE_ENT                    devFamily;
    CPSS_PP_SUB_FAMILY_TYPE_ENT                devSubFamily;
    GT_U32                                     drvHwIfSupportedBmp;
    CPSS_PP_INTERFACE_CHANNEL_ENT              mngInterfaceType;

    PRV_CPSS_DRV_STRUCT_PP_PORT_GROUP_HW_STC   hwCtrl[CPSS_MAX_PORT_GROUPS_CNS];
    GT_UINTPTR                                 resetAndInitControllerBase;
    CPSS_RESOURCE_MAPPING_STC                  resourceMapping;
    CPSS_BUS_DRIVER_OBJ_STC*                   drivers[CPSS_MAX_PORT_GROUPS_CNS];
    PRV_CPSS_DRV_INT_CTRL_STC                  intCtrl;
    PRV_CPSS_DRV_ERRATA_STC                    errata;
    PRV_CPSS_DRV_PORT_GROUPS_INFO_STC          portGroupsInfo;
}PRV_CPSS_DRIVER_PP_CONFIG_STC;


/* array of pointers to the valid driver devices */
extern PRV_CPSS_DRIVER_PP_CONFIG_STC* prvCpssDrvPpConfig[PRV_CPSS_MAX_PP_DEVICES_CNS];
/* flag allowing memory access trace */
extern GT_BOOL memoryAccessTraceOn;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDrvPpDefsh */


