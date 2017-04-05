/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssTypes.h
*
* DESCRIPTION:    Common Driver facility types definitions
*
* FILE REVISION NUMBER:
*       $Revision: 107 $
********************************************************************************/

#ifndef __cpssTypesh
#define __cpssTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define  CHX_FAMILY
/************ Includes ********************************************************/
#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/* macro to set TBD bookmarks */
#define CPSS_TBD_BOOKMARK

/* macro to set TBD eArch bookmarks */
#define CPSS_TBD_BOOKMARK_EARCH

/* macro to set TBD Lion2 bookmarks */
#define CPSS_TBD_BOOKMARK_LION2

/* macro to set TBD Puma3 bookmarks */
#define CPSS_TBD_BOOKMARK_PUMA3

/* macro to set TBD Bobcat2 bookmarks */
#define CPSS_TBD_BOOKMARK_BOBCAT2

/* macro to set TBD Bobcat2-bobk bookmarks */
#define CPSS_TBD_BOOKMARK_BOBCAT2_BOBK

/* macro to set TBD Bobcat3 bookmarks */
#define CPSS_TBD_BOOKMARK_BOBCAT3

/* macro to set TBD Xcat3 bookmarks */
#define CPSS_TBD_BOOKMARK_XCAT3

/* macro to set TBD Aldrin bookmarks */
#define CPSS_TBD_BOOKMARK_ALDRIN

/* CPSS API LOG related MACROs. */
/* _funcId - unique number for log creation */
/* _cpssApi - CPSS API name. */
/* _log_enter_args - _funcId & CPSS API arguments */
/* _rc - CPSS API return status */
#if defined(CPSS_LOG_ENABLE) && defined(CPSS_LOG_IN_MODULE_ENABLE)

/* macro for declaring on a function - for getting a unique identifier for it */
#define CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(_funcId, _func_name) \
    static const GT_U32 funcId = PRV_CPSS_LOG_FUNC_##_func_name##_E
/* macro for creating enter into function log if needed */
#define CPSS_LOG_API_ENTER_MAC(_params) \
    if (prvCpssLogEnabled == GT_TRUE) prvCpssLogApiEnter _params
/* macro for creating exit from function log if needed */
#define CPSS_LOG_API_EXIT_MAC(_funcId, _rc) \
    if (prvCpssLogEnabled == GT_TRUE) prvCpssLogApiExit(_funcId, _rc)
/* macro to log function error and exit from function - used in API error return statements */
#define CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(_funcId, _rc) \
    { \
        if (prvCpssLogEnabled == GT_TRUE) { \
            prvCpssLogError(__FUNCTION__, __FILENAME__, __LINE__, _rc, LOG_ERROR_NO_MSG); \
            prvCpssLogApiExit(_funcId, _rc); \
        } \
        return _rc; \
    }

#else

#define CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(_funcId, _cpssApi)
#define CPSS_LOG_API_ENTER_MAC(_log_enter_args)
#define CPSS_LOG_API_EXIT_MAC(_funcId, _rc)
/* macro to log function error and exit from function - used in API error return statements */
#define CPSS_LOG_API_ERROR_EXIT_AND_RETURN_MAC(_funcId, _rc) return _rc
#endif


/* macro for wrapping dedicated callback. */
/* _callback_args - _funcId (see LOG related MACROs above), CPSS API return status & arguments */
#define CPSS_APP_SPECIFIC_CB_MAC(_callback_args);

/* constant to state that the parameter with this value need to be ignored
    This meaning is given only for fields that explicitly stated to support it */
#define CPSS_PARAM_NOT_USED_CNS 0xFFFFFFFF

/* constant to represent application that not care about specific 'portGroup' in
   'multi-port-groups' device .

   for non 'multi-port-groups' device - parameter will be ignored anyway.
   for 'multi-port-groups' device - parameter state that action apply
   to all active port groups.

   see also GT_PORT_GROUPS_BMP
*/
#define CPSS_PORT_GROUP_UNAWARE_MODE_CNS 0xFFFFFFFF

/* number of max supported port groups in 'multi-port-groups' device */
#define CPSS_MAX_PORT_GROUPS_CNS  8

/* index in array of 'port-groups' for a non 'multi-port-groups' device */
#define CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS        0

/* CPU PORT NUMBER Definition */
#define CPSS_CPU_PORT_NUM_CNS           63

/* max ports in device in PPs of Prestera family  */
#define CPSS_MAX_PORTS_NUM_CNS          512

#define CPSS_MAX_PORTS_BMP_NUM_CNS   ((CPSS_MAX_PORTS_NUM_CNS + 31) / 32)

/* NULL Port */
#define CPSS_NULL_PORT_NUM_CNS          62

/* NULL Port for ExMx and ExMxTg devices */
#define CPSS_EXMX_NULL_PORT_NUM_CNS     61

/*
 * typedef: enum CPSS_UNICAST_MULTICAST_ENT
 *
 * Description: Unicast / Multicast selection
 *
 * Enumerations:
 *  CPSS_UNICAST_E            - Unicast packet
 *  CPSS_MULTICAST_E          - Multicast packet
 *  CPSS_UNICAST_MULTICAST_E  - Unicast or Multicast
 */
typedef enum
{
    CPSS_UNICAST_E            = 0,
    CPSS_MULTICAST_E          = 1,
    CPSS_UNICAST_MULTICAST_E  = 2
}CPSS_UNICAST_MULTICAST_ENT;

/*
 * typedef: enum CPSS_IP_PROTOCOL_STACK_ENT
 *
 * Description: type of IP stack used
 *
 *
 * Enumerations:
 *  CPSS_IP_PROTOCOL_IPV4_E   - IPv4 stack
 *  CPSS_IP_PROTOCOL_IPV6_E   - IPv6 stack
 *  CPSS_IP_PROTOCOL_IPV4V6_E - dual stack
 */
typedef enum
{
    CPSS_IP_PROTOCOL_IPV4_E     = 0,
    CPSS_IP_PROTOCOL_IPV6_E     = 1,
    CPSS_IP_PROTOCOL_IPV4V6_E   = 2
}CPSS_IP_PROTOCOL_STACK_ENT;

/*
 * typedef: CPSS_ADJUST_OPERATION_ENT
 *
 * Description:
 *      adjust operation.
 * Fields:
 *    CPSS_ADJUST_OPERATION_ADD_E          - add
 *    CPSS_ADJUST_OPERATION_SUBSTRUCT_E    - subtract
 *
 * Comment:
 */
typedef enum
{
    CPSS_ADJUST_OPERATION_ADD_E,
    CPSS_ADJUST_OPERATION_SUBSTRUCT_E
} CPSS_ADJUST_OPERATION_ENT;

/*
 * typedef: CPSS_SHIFT_OPERATION_ENT
 *
 * Description:
 *      shift operation.
 * Fields:
 *    CPSS_SHIFT_OPERATION_SHIFT_LEFT_E   - shift left
 *    CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E  - shift right
 *
 * Comment:
 */
typedef enum
{
    CPSS_SHIFT_OPERATION_SHIFT_LEFT_E,
    CPSS_SHIFT_OPERATION_SHIFT_RIGHT_E
} CPSS_SHIFT_OPERATION_ENT;

/*
 * Typedef: enum CPSS_INTERFACE_TYPE_ENT
 *
 * Description: enumerator for interface
 *   To be used for:
 *   1. set mac entry info
 *   2. set next hop info
 *   3. redirect pcl info
 *   4. get new Address info
 *
 * Fields:
 *      CPSS_INTERFACE_PORT_E        - the interface is of port type  {dev,port}
 *      CPSS_INTERFACE_TRUNK_E       - the interface is of trunk type {trunkId}
 *      CPSS_INTERFACE_VIDX_E        - the interface is of Vidx type  {vidx}
 *      CPSS_INTERFACE_VID_E         - the interface is of Vid type  {vlan-id}
 *      CPSS_INTERFACE_DEVICE_E      - the interface is device
 *      CPSS_INTERFACE_FABRIC_VIDX_E - the interface is of Vidx type  {fabricVidx}
 *      CPSS_INTERFACE_INDEX_E       - the interface index type
 */
typedef enum {
    CPSS_INTERFACE_PORT_E = 0,
    CPSS_INTERFACE_TRUNK_E,
    CPSS_INTERFACE_VIDX_E,
    CPSS_INTERFACE_VID_E,
    CPSS_INTERFACE_DEVICE_E,
    CPSS_INTERFACE_FABRIC_VIDX_E,
    CPSS_INTERFACE_INDEX_E
}CPSS_INTERFACE_TYPE_ENT;

/*
 * Typedef: GT_TRUNK_ID
 *
 * Description: Defines trunk id
 *
 *  used as the type for the trunk Id's
 *
 */
typedef GT_U16  GT_TRUNK_ID;


/*
 * Typedef: GT_HW_DEV_NUM
 *
 * Description: Defines HW dev num
 *
 *  used as the type for the hw dev num
 *
 */
typedef GT_U32 GT_HW_DEV_NUM;


/*
 * Typedef: GT_PORT_NUM
 *
 * Description: Defines port num
 *
 *  used as the type for the port num
 *
 */
typedef GT_U32 GT_PORT_NUM;

/*
 * Typedef: U32 GT_PHYSICAL_PORT_NUM
 *
 * Description: Defines physical port num
 *
 *  used as the type for the physical port num
 *
 */
typedef GT_U32 GT_PHYSICAL_PORT_NUM;


/*
 * typedef: struct CPSS_HW_ADDR_STC
 *
 * Description: Hardware (bus) address of PP
 *              Used for CPSS_CHANNEL_PEX_KERNEL_E driver ONLY
 *
 * Fields:
 *
 *   busNo        - PCI/PEX busNo
 *   devSel       - PCI/PEX deviceNo
 *   funcNo       - PCI/PEX func
 *
 * Comments:
 *
 */
typedef struct
{
    GT_U32      busNo;
    GT_U32      devSel;
    GT_U32      funcNo;
} CPSS_HW_ADDR_STC;


/*
 * typedef: struct CPSS_RESOURCE_MAPPING_STC
 *
 * Description:
 *      The structure reflect various resource mapping for device
 *
 * Fields:
 *      sramBaseAddress - virtual address of sram or 0
 *      sramSize        - SRAM size
 *
 */
typedef struct CPSS_RESOURCE_MAPPING_STCT
{
    GT_UINTPTR  sramBaseAddress;
    GT_U32      sramSize;
} CPSS_RESOURCE_MAPPING_STC;


/*
 * Typedef: structure CPSS_INTERFACE_INFO_STC
 *
 * Description: Defines the interface info
 *   To be used for:
 *   1. set mac entry info
 *   2. set next hop info
 *   3. redirect pcl info
 *   4. get new Address info
 *
 * Fields:
 *      type - the interface type -- port/trunk/vidx
 *
 *      !!!! NOTE : next files treated as UNION !!!!
 *
 *      devPort     -  info about the {dev,port}     - relevant to CPSS_INTERFACE_PORT_E
 *              hwDevNum  - the HW device number
 *              portNum   - the port number
 *                     APPLICABLE DEVICES: Puma2, Puma3.
 *                     Relevant to CPSS_INTERFACE_DEVICE_E for FDB APIs.
 *      trunkId     -  info about the {trunkId}      - relevant to CPSS_INTERFACE_TRUNK_E
 *      vidx        -  info about the {vidx}         - relevant to CPSS_INTERFACE_VIDX_E
 *      vlanId      -  info about the {vid}          - relevant to CPSS_INTERFACE_VID_E
 *      hwDevNum    -  info about the {HW device number}- relevant to CPSS_INTERFACE_DEVICE_E
 *                     APPLICABLE DEVICES: Puma2, Puma3, DxCh1, DxCh1_Diamond, DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2, Bobcat2; Caelum; Bobcat3.
 *                     Relevant to CPSS_INTERFACE_TRUNK_E for FDB APIs.
 *      fabricVidx  -  info about the {fabricVidx}   - relevant to CPSS_INTERFACE_FABRIC_VIDX_E
 *      index       -  info about the {index}        - relevant to CPSS_INTERFACE_INDEX_E
 *
 * Note: for field information please refer to cpssDxChCfgDevInfoGet in DX devices
 *      and cpssExMxPmCfgDevInfoGet in EXMXPM devices
 */
typedef struct{
    CPSS_INTERFACE_TYPE_ENT     type;

    struct{
        GT_HW_DEV_NUM   hwDevNum;
        GT_PORT_NUM     portNum;
    }devPort;

    GT_TRUNK_ID     trunkId;
    GT_U16          vidx;
    GT_U16          vlanId;
    GT_HW_DEV_NUM   hwDevNum;
    GT_U16          fabricVidx;
    GT_U32          index;
}CPSS_INTERFACE_INFO_STC;

/* macro CPSS_PORTS_BMP_PORT_SET_MAC
    to set a port in the bmp of ports

  portsBmpPtr - of type CPSS_PORTS_BMP_STC*
                pointer to the ports bmp
  portNum - the port num to set in the ports bmp
*/
#define CPSS_PORTS_BMP_PORT_SET_MAC(portsBmpPtr,portNum)   \
    (portsBmpPtr)->ports[(portNum)>>5] |= 1 << ((portNum)& 0x1f)

/* macro CPSS_PORTS_BMP_PORT_CLEAR_MAC
    to clear a port from the bmp of ports

  portsBmpPtr - of type CPSS_PORTS_BMP_STC*
                pointer to the ports bmp
  portNum - the port num to clear from the ports bmp
*/
#define CPSS_PORTS_BMP_PORT_CLEAR_MAC(portsBmpPtr,portNum)   \
    (portsBmpPtr)->ports[(portNum)>>5] &= ~(1 << ((portNum)& 0x1f))

/* macro CPSS_PORTS_BMP_IS_PORT_SET_MAC
    to check if port is set the bmp of ports (is corresponding bit is 1)

  portsBmpPtr - of type CPSS_PORTS_BMP_STC*
                pointer to the ports bmp
  portNum - the port num to set in the ports bmp

  return 0 -- port not set in bmp
  return 1 -- port set in bmp
*/
#define CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsBmpPtr,portNum)   \
    (((portsBmpPtr)->ports[(portNum)>>5] & (1 << ((portNum)& 0x1f)))? 1 : 0)

/* portsBmpTargetPtr = portsBmpSrc1Ptr | portsBmpSrc2Ptr */
#define CPSS_PORTS_BMP_BITWISE_OR_MAC(portsBmpTargetPtr,portsBmpSrc1Ptr,portsBmpSrc2Ptr) \
    {                                                                                    \
        GT_U32  __ii;                                                                    \
        for(__ii = 0 ; __ii < CPSS_MAX_PORTS_BMP_NUM_CNS; __ii++)                        \
            (portsBmpTargetPtr)->ports[__ii] = (portsBmpSrc1Ptr)->ports[__ii] | (portsBmpSrc2Ptr)->ports[__ii];\
    }

/* portsBmpTargetPtr = portsBmpSrc1Ptr & portsBmpSrc2Ptr */
#define CPSS_PORTS_BMP_BITWISE_AND_MAC(portsBmpTargetPtr,portsBmpSrc1Ptr,portsBmpSrc2Ptr) \
    {                                                                                    \
        GT_U32  __ii;                                                                    \
        for(__ii = 0 ; __ii < CPSS_MAX_PORTS_BMP_NUM_CNS; __ii++)                        \
            (portsBmpTargetPtr)->ports[__ii] = (portsBmpSrc1Ptr)->ports[__ii] & (portsBmpSrc2Ptr)->ports[__ii];\
    }

/* portsBmpTargetPtr = portsBmpSrc1Ptr & ~portsBmpSrc2Ptr */
#define CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(portsBmpTargetPtr,portsBmpSrc1Ptr,portsBmpSrc2Ptr) \
    {                                                                                    \
        GT_U32  __ii;                                                                    \
        for(__ii = 0 ; __ii < CPSS_MAX_PORTS_BMP_NUM_CNS; __ii++)                        \
            (portsBmpTargetPtr)->ports[__ii] = (portsBmpSrc1Ptr)->ports[__ii] & (~(portsBmpSrc2Ptr)->ports[__ii]);\
    }

/* portsBmpTargetPtr = ~ portsBmpSrcPtr */
#define CPSS_PORTS_BMP_BITWISE_NOT_MAC(portsBmpTargetPtr,portsBmpSrcPtr)        \
    {                                                                           \
        GT_U32  __ii;                                                           \
        for(__ii = 0 ; __ii < CPSS_MAX_PORTS_BMP_NUM_CNS; __ii++)               \
            (portsBmpTargetPtr)->ports[__ii] = (~(portsBmpSrcPtr)->ports[__ii]);\
    }

/* all ii 0.. max --> portsBmpTargetPtr[ii] == 0 */
#define CPSS_PORTS_BMP_IS_ZERO_MAC(portsBmpPtr)     \
    ((0 ==                                          \
        cpssOsMemCmp(portsBmpPtr,&cpssAllZeroPortsBmp,sizeof(CPSS_PORTS_BMP_STC))) ? 1 : 0)

/* set for all ii 0.. max --> portsBmpTargetPtr[ii] = 0 */
#define CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(portsBmpPtr)     \
    cpssOsMemSet(portsBmpPtr,0,sizeof(CPSS_PORTS_BMP_STC))

/* macro CPSS_PORTS_BMP_PORT_ENABLE_MAC
    to set or clear a port from the bmp of ports

  portsBmpPtr - of type CPSS_PORTS_BMP_STC*
                pointer to the ports bmp
  portNum - the port num to set the value from the ports bmp
  enable - of type GT_BOOL - GT_TRUE: set GT_FALSE: clear
*/
#define CPSS_PORTS_BMP_PORT_ENABLE_MAC(portsBmpPtr,portNum, enable)   \
    if ((enable) != GT_FALSE)                                         \
    {                                                                 \
        CPSS_PORTS_BMP_PORT_SET_MAC((portsBmpPtr),(portNum));      \
    }                                                                 \
    else                                                              \
    {                                                                 \
        CPSS_PORTS_BMP_PORT_CLEAR_MAC((portsBmpPtr),(portNum));       \
    }

/* macro CPSS_PORTS_BMP_PORT_SET_ALL_MAC
    set all ports in portsBmpPtr

  portsBmpPtr - of type CPSS_PORTS_BMP_STC*
                pointer to the ports bmp
*/
#define CPSS_PORTS_BMP_PORT_SET_ALL_MAC(portsBmpPtr) \
    cpssOsMemSet(portsBmpPtr,0xFF,sizeof(CPSS_PORTS_BMP_STC))

/* check if ports bmp in portsBmpPtr1 == portsBmpPtr2
    return 1 - equal
    return 0 - NOT equal
*/
#define CPSS_PORTS_BMP_ARE_EQUAL_MAC(portsBmpPtr1,portsBmpPtr2)     \
    (cpssOsMemCmp(portsBmpPtr1,portsBmpPtr2,sizeof(CPSS_PORTS_BMP_STC)) ? 0 : 1)


/*
 * Typedef: structure CPSS_PORTS_BMP_STC
 *
 * Description: Defines the bmp of ports (up to 512 ports)
 *
 * Fields:
 *      ports - array of bmp of ports
 *              ports[0] - hold bmp of the ports 0  - 31
 *              ports[1] - hold bmp of the ports 32 - 63
 *              ports[2] - hold bmp of the ports 64 - 95
 *              ports[3] - hold bmp of the ports 96 - 127
 *              ports[4] - hold bmp of the ports 128 - 159
 *              ports[5] - hold bmp of the ports 160 - 191
 *              ports[6] - hold bmp of the ports 192 - 223
 *              ports[7] - hold bmp of the ports 224 - 255
 *              ports[8] - hold bmp of the ports 256 - 287
 *              ports[9] - hold bmp of the ports 288 - 319
 *              ports[10] - hold bmp of the ports 320 - 351
 *              ports[11] - hold bmp of the ports 352 - 383
 *              ports[12] - hold bmp of the ports 384 - 415
 *              ports[13] - hold bmp of the ports 416 - 447
 *              ports[14] - hold bmp of the ports 448 - 479
 *              ports[15] - hold bmp of the ports 480 - 511
 *
 *  notes:
 *  can use next macros , for single port operation:
 *  CPSS_PORTS_BMP_PORT_SET_MAC , CPSS_PORTS_BMP_PORT_CLEAR_MAC
 *  CPSS_PORTS_BMP_IS_PORT_SET_MAC
 *
 *  bitwise operations on portsBmp:
 *  CPSS_PORTS_BMP_BITWISE_OR_MAC ,  CPSS_PORTS_BMP_BITWISE_AND_MAC ,
 *  CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC , CPSS_PORTS_BMP_BITWISE_NOT_MAC
 *
 *  others operations on portsBmp:
 *  CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC , CPSS_PORTS_BMP_IS_ZERO_MAC
 */
typedef struct{
    GT_U32      ports[CPSS_MAX_PORTS_BMP_NUM_CNS];
}CPSS_PORTS_BMP_STC;

/* BMP of ports with no ports members */
extern const CPSS_PORTS_BMP_STC cpssAllZeroPortsBmp;

/*
 * typedef: enum CPSS_BULK_OPERATION_TYPE_ENT
 *
 * Description: Possible operations to be used in bulk operation
 *
 * Enumerations:
 *      CPSS_BULK_OPERATION_ADD_E     - add
 *      CPSS_BULK_OPERATION_DELETE_E  - delete
 *
 */
typedef enum
{
    CPSS_BULK_OPERATION_ADD_E    = 0,
    CPSS_BULK_OPERATION_DELETE_E = 1
}CPSS_BULK_OPERATION_TYPE_ENT;

/* support running from the PSS */
#ifndef __gtTypesh

/*
 * Typedef: union GT_IPADDR
 *
 * Description: Defines the IP address
 *
 * Fields:
 *      u32Ip - the IP as single WORD
 *      arIP  - the IP as 4 bytes - NETWORK ORDER
 */
typedef union
{
    GT_U32  u32Ip;
    GT_U8   arIP[4];
}GT_IPADDR;

/*
 * Typedef: union GT_IPV6ADDR
 *
 * Description: Defines the IPv6 address
 *
 * Fields:
 *      u32Ip - the IP as 4 WORDs
 *      arIP  - the IP as 16 bytes - NETWORK ORDER
 */
typedef union
{
    GT_U32 u32Ip[4];
    GT_U8  arIP[16];
}GT_IPV6ADDR;


/*
 * Typedef: union GT_IP_ADDR_TYPE_UNT
 *
 * Description: Defines the type of IP address
 *
 * Fields:
 *      ipv6Addr - the IP addr is IPv6 type
 *      ipv4Addr  - the IP as IPv4 type
 */
typedef union
{
    GT_IPV6ADDR ipv6Addr;
    GT_IPADDR   ipv4Addr;
}GT_IP_ADDR_TYPE_UNT ;

/*
 * Typedef: structure GT_ETHERADDR
 *
 * Description: Defines the mac address
 *
 * Fields:
 *      arEther - the mac as 6 bytes - NETWORK ORDER
 *
 */
typedef struct
{
    GT_U8       arEther[6];
}GT_ETHERADDR;

/*
 * Typedef: structure GT_FCID
 *
 * Description: Defines FC_ID
 *
 * Fields:
 *      fcid - the FC_ID - NETWORK ORDER
 *
 */
typedef struct
{
    GT_U8       fcid[3];
}GT_FCID;

#define GT_HW_MAC_LOW32(macAddr)                \
                ((macAddr)->arEther[5] |          \
                ((macAddr)->arEther[4] << 8) |    \
                ((macAddr)->arEther[3] << 16) |   \
                ((macAddr)->arEther[2] << 24))

#define GT_HW_MAC_HIGH16(macAddr)           \
        ((macAddr)->arEther[1] | ((macAddr)->arEther[0] << 8))

#define GT_HW_MAC_LOW16(macAddr)            \
        ((macAddr)->arEther[5] | ((macAddr)->arEther[4] << 8))

#define GT_HW_MAC_HIGH32(macAddr)               \
                ((macAddr)->arEther[3] |          \
                ((macAddr)->arEther[2] << 8) |    \
                ((macAddr)->arEther[1] << 16) |   \
                ((macAddr)->arEther[0] << 24))

/* Constant of prefix for the 'Mac Index' format */
#define MAC_INDEX_PREFIX     0xFFFFFFFF

/* This is a service MACRO for the 'Mac Index' format,
   add to the 2 mac index bytes the 4 MAC_INDEX_PREFIX bytes. */
#define GT_ADD_MAC_INDEX_PREFIX(macIndex,macAddrIndex)                   \
                (macAddrIndex)->arEther[5] =  macIndex;                  \
                (macAddrIndex)->arEther[4] = (macIndex >> 8);            \
                (macAddrIndex)->arEther[3] =  MAC_INDEX_PREFIX;          \
                (macAddrIndex)->arEther[2] = (MAC_INDEX_PREFIX >> 8);    \
                (macAddrIndex)->arEther[1] = (MAC_INDEX_PREFIX >> 16);   \
                (macAddrIndex)->arEther[0] = (MAC_INDEX_PREFIX >> 24);

/*
 * typedef: enum GT_PRTCL
 *
 * Description: Enumeration of protocols codes.
 *
 * Enumerations:
 *    IPV4       - IPv4.
 *    IPV6       - IPv6.
 *    MPLS       - MPLS.
 *    USER_DEF_0 - User Defined Protocol 0.
 *    USER_DEF_1 - User Defined Protocol 1.
 *    L2CE_ETHER - An Ethernet Layer-2 Circuit Emulation (An Ethernet CRC must
 *                 be generated).
 *    L2CE_OTHER - Non Ethernet Layer-2 Circuit Emulation.
 */
typedef enum
{
    IPV4 = 0,
    IPV6,
    MPLS,
    USER_DEF_0,
    USER_DEF_1,
    L2CE_ETHER = 6,
    L2CE_OTHER
}GT_PRTCL;

/*
 * Typedef: GT_PORT_GROUPS_BMP
 *
 * Description: Defines bitmap of up to 32 port groups
 *
 *  used as the type for the 'port groups bitmap'
 *  this relevant only to multi-port groups devices
 *
 */
typedef GT_U32  GT_PORT_GROUPS_BMP;

/* support running from the PSS */
#endif /*__gtTypesh*/

/*
 * Typedef: enumeration CPSS_PP_FAMILY_TYPE_ENT
 *
 * Description: Defines the different device families that may exist in system.
 *
 * Fields:
 *      CPSS_PP_FAMILY_START_EXMX_E   - start of EXMX devices
 *        CPSS_PP_FAMILY_TWISTC_E       - Twist-C family devices
 *        CPSS_PP_FAMILY_TWISTD_E       - Twist-D family devices
 *        CPSS_PP_FAMILY_SAMBA_E        - Samba family devices
 *        CPSS_PP_FAMILY_TIGER_E        - Tiger family devices
 *      CPSS_PP_FAMILY_END_EXMX_E     - end of EXMX devices
 *      CPSS_PP_FAMILY_START_DXSAL_E  - start of DX Salsa devices
 *        CPSS_PP_FAMILY_SALSA_E        - Salsa family devices
 *      CPSS_PP_FAMILY_END_DXSAL_E    - end of DX Salsa devices
 *      CPSS_PP_FAMILY_START_DXCH_E   - start of DX CH devices
 *        CPSS_PP_FAMILY_CHEETAH_E      - Cheetah family devices
 *        CPSS_PP_FAMILY_CHEETAH2_E     - Cheetah 2 family devices      NOTE: must be > than CPSS_PP_FAMILY_CHEETAH_E
 *        CPSS_PP_FAMILY_CHEETAH3_E     - Cheetah 3 family devices      NOTE: must be > than CPSS_PP_FAMILY_CHEETAH2_E
 *        CPSS_PP_FAMILY_DXCH_XCAT_E    - (DXCH) xCat family devices    NOTE: must be > than CPSS_PP_FAMILY_CHEETAH3_E
 *        CPSS_PP_FAMILY_DXCH_XCAT3_E   - (DXCH) xCat3 family devices   NOTE: must be > than CPSS_PP_FAMILY_DXCH_XCAT_E
 *        CPSS_PP_FAMILY_DXCH_LION_E    - (DXCH) Lion family devices    NOTE: must be > than CPSS_PP_FAMILY_DXCH_XCAT_E
 *        CPSS_PP_FAMILY_DXCH_XCAT2_E   - (DXCH) xCat2 family devices   NOTE: must be > than CPSS_PP_FAMILY_DXCH_LION_E
 *        CPSS_PP_FAMILY_DXCH_LION2_E   - (DXCH) Lion 2 family devices  NOTE: must be > than CPSS_PP_FAMILY_DXCH_XCAT2_E
 *        CPSS_PP_FAMILY_DXCH_LION3_E   - (DXCH) Lion 3 family devices  NOTE: must be > than CPSS_PP_FAMILY_DXCH_LION2_E
 *        CPSS_PP_FAMILY_DXCH_BOBCAT2_E - (DXCH) Bobcat 2 family devices  NOTE: must be > than CPSS_PP_FAMILY_DXCH_LION3_E
 *        CPSS_PP_FAMILY_DXCH_BOBCAT3_E - (DXCH) Bobcat 3 family devices  NOTE: must be > than CPSS_PP_FAMILY_DXCH_BOBCAT2_E
 *        CPSS_PP_FAMILY_DXCH_ALDRIN_E  - (DXCH) Aldrin family devices  NOTE: must be > than CPSS_PP_FAMILY_DXCH_BOBCAT3_E
 *      CPSS_PP_FAMILY_END_DXCH_E     - end of DX CH devices
 *      CPSS_PP_FAMILY_START_EXMXPM_E - start of Puma devices
 *        CPSS_PP_FAMILY_PUMA_E         - Puma family devices (Puma 2)
 *        CPSS_PP_FAMILY_PUMA3_E        - Puma 3 family devices
 *      CPSS_PP_FAMILY_END_EXMXPM_E   - end of Puma devices
 *      CPSS_PP_FAMILY_LAST_E         - last value of enumeration ,
 *                                      used for Array size
 *      CPSS_MAX_FAMILY               - maximal value of the enum
 */
typedef enum
{

/*start*/  CPSS_PP_FAMILY_START_EXMX_E,

                CPSS_PP_FAMILY_TWISTC_E,
                CPSS_PP_FAMILY_TWISTD_E,
                CPSS_PP_FAMILY_SAMBA_E,
                CPSS_PP_FAMILY_TIGER_E,

/*end*/    CPSS_PP_FAMILY_END_EXMX_E,

/*start*/  CPSS_PP_FAMILY_START_DXSAL_E,

                CPSS_PP_FAMILY_SALSA_E,

/*end*/    CPSS_PP_FAMILY_END_DXSAL_E,

/*start*/  CPSS_PP_FAMILY_START_DXCH_E,

                CPSS_PP_FAMILY_CHEETAH_E,
                CPSS_PP_FAMILY_CHEETAH2_E,
                CPSS_PP_FAMILY_CHEETAH3_E,
                CPSS_PP_FAMILY_DXCH_XCAT_E,
                CPSS_PP_FAMILY_DXCH_XCAT3_E,
                CPSS_PP_FAMILY_DXCH_LION_E,
                CPSS_PP_FAMILY_DXCH_XCAT2_E,
                CPSS_PP_FAMILY_DXCH_LION2_E,
                CPSS_PP_FAMILY_DXCH_LION3_E,
                CPSS_PP_FAMILY_DXCH_BOBCAT2_E,
                CPSS_PP_FAMILY_DXCH_BOBCAT3_E,
                CPSS_PP_FAMILY_DXCH_ALDRIN_E,

/*end*/    CPSS_PP_FAMILY_END_DXCH_E,


/*start*/  CPSS_PP_FAMILY_START_EXMXPM_E,

                CPSS_PP_FAMILY_PUMA_E,
                CPSS_PP_FAMILY_PUMA3_E,

/*end*/    CPSS_PP_FAMILY_END_EXMXPM_E,


    CPSS_PP_FAMILY_LAST_E,

    CPSS_MAX_FAMILY = 0x7FFFFFFF
}CPSS_PP_FAMILY_TYPE_ENT;

/*
 * Typedef: enumeration CPSS_PP_SUB_FAMILY_TYPE_ENT
 *
 * Description: Defines the different device sub families that may exist in system.
 *
 * Fields:
 *  CPSS_PP_SUB_FAMILY_NONE_E - the device hold no special 'sub family' and the
 *                  identified without 'sub family'
 *
 *  CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E - this is bobk sub family within the bobcat2 family.
 *                  relevant only when 'family' is 'bobcat2'
 *
*/
typedef enum
{
    CPSS_PP_SUB_FAMILY_NONE_E,
    CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E,
    CPSS_MAX_SUB_FAMILY,                       /* never cross this boundary */
    CPSS_BAD_SUB_FAMILY = 0x7FFFFFFF
}CPSS_PP_SUB_FAMILY_TYPE_ENT;

/* device type place holder */
#define PRV_CPSS_DEVICE_TYPE_PLACE_HOLDER_CNS   0xFFEEFFEE

/*
 * Typedef: enumeration CPSS_PP_DEVICE_TYPE
 *
 * Description: Defines the different device type that may exist in system.
 *
 *      known values :
 *
 *  EXMX - Twist group:
 *      98MX610B     - 48 + 4 DSSMII full feature
 *      98MX620B     - 10 * 1G full feature
 *      98MX610BS    - 48 + 4 SSMII full feature
 *      98EX110BS    - 48 + 4 SSMII no external rams
 *      98EX111BS    - 48 + 4 SSMII no ns, ws
 *      98EX112BS    - 48 + 4 SSMII no ws, no df
 *      98EX110B     - 48 + 4 DSSMII no external rams
 *      98EX111B     - 48 + 4 DSSMII no ns, ws
 *      98EX112B     - 48 + 4 DSSMII no ws, no df
 *      98EX120B     - 12G no external rams
 *      98EX120B_    - 12G no external rams (TC enabled)
 *      98EX121B     - 12G no ns, no ws
 *      98EX122B     - 12G no ws, no df
 *      98EX128B     - 12G  with external memories
 *      98EX129B     - 12G  with ws, and fd
 *
 *  EXMX - Twist D group:
 *      98EX100D  - 24+4
 *      98EX110D  - 48+4 SSMII
 *      98EX115D  - 48+4 SSMII with external narrow sram
 *      98EX110DS - 48+4 DSSMII
 *      98EX115DS - 48+4 DSSMII with external narrow sram
 *      98EX120D  - 12G
 *      98EX125D  - 12G with external narrow sram
 *      98EX130D  - 1 * 10G (XG)
 *      98EX135D  - 1 * 10G (XG) with external sram
 *
 *  EXMX - Samba/Rumba group:
 *      98MX615/8   - 48+4 SSMII   with external narrow sram
 *      98MX625/8   - 12G          with external narrow sram
 *      98MX635/8   - 1 * 10G (XG) with external sram
 *
 *
 *  DXSAL - Salsa group:
 *      98DX240   - 24G  Management Dev
 *      98DX160   - 16G  Management Dev
 *      98DX120   - 12G  Management Dev
 *      98DX241   - 24G  NO Management Dev
 *      98DX161   - 16G  NO Management Dev
 *      98DX121   - 12G  NO Management Dev
 *
 *  DXCH - Cheetah group:
 *     98DX270,273,272     - 24G + 3 10G                    - PCI/SMI
 *     98DX260,262,263,268 - 24G + 2 10G                    - PCI/SMI
 *     98DX250,246,243,248 - 24G                            - PCI/SMI
 *     98DX107,106         - 10G                            - SMI
 *     98DX163,166,167     - 16G                            - SMI
 *     98DX803             - 3*10G                          - PCI
 *     98DX249             - 24G + 1 10G + 1 5G             - SMI
 *     98DX269             - 24G + 1 10G + 1 10/5G + 1 5G   - SMI
 *
 *  DXCH - Cheetah 2 group:
 *     98DX255             - 24           - PCI/SMI
 *     98DX265             - 24  + 2*10G  - PCI/SMI
 *     98DX275             - 24  + 3*10G  - PCI/SMI
 *     98DX285             - 24  + 4*10G  - PCI/SMI
 *
 *  DXCH - Cheetah 3 group:
 *     98DX5128             - 24G + 4*stack
 *     98DX8110             - 6XG + 4*stack
 *     98DX8111             - 6XG + 4*stack
 *
 *  EXMX - Tiger group:
 *     98EX116   - 48+4 SSMII with external narrow sram
 *     98EX106   - 24+4 SSMII with external narrow sram
 *     98EX108   - 24+4 SSMII with external narrow sram
 *     98EX126   - 12G with external narrow sram
 *     98EX136   - 1X10G with external narrow sram
 *
 *     98EX116DI   - 48+4
 *     98EX126DI   - 12G
 *     98EX136DI   - 1X10G
 *
 * EXMXPM - Puma Group :
 *     98EX24X     - 24 G device
 *     98MX84X     - 24 G device
 *     98EX26X     - 2XG device
 *     98MX86X     - 2XG device
 *
 *
 *  DXCH - xCat devices
 *
 *                                                     FE       GE       Stack  Stack     Stack     Stack
 *                                                                       GE     GE/2.5GE  GE/2.5/   GE/2.5/
 *                                                                              (DQX)     10/12GE   5/10/12GE
 *                                                                                        (DQX/DHX) (DQX/DHX/XAUI)
 *   *98DX1000 Series: AlleyCat-FE WEBSMART- 333MHz, 0K Cache, SPI, DDR2*
 *   CPSS_98DX1005_CNS       = 0xDF5111AB,               8        0        2        0        0        0
 *   CPSS_98DX1025_CNS       = 0xDFF511AB,              24        0        4        0        0        0
 *   CPSS_98DX1026_CNS       = 0xDF7111AB,              24        0        2        2        0        0
 *   CPSS_98DX1027_CNS       = 0xDF7311AB,              24        0        2        2        0        0
 *
 *   *98DX102x Series: PonCat-FE *
 *   CPSS_98DX1002_CNS       = 0xDFD111AB,               8        0        3        0        0        0
 *   CPSS_98DX1022_CNS       = 0xDFF111AB,              24        0        3        0        0        0
 *   CPSS_98DX1023_CNS       = 0xDFF311AB,              24        0        3        0        0        0
 *
 *   *98DX1100/1200 Series: TomCat-FE (SMB L2+) - 600MHz, 16KB/128KB Cache  *
 *   CPSS_98DX1122_CNS       = 0xDCB111AB,              24        0        0        4        0        0
 *   CPSS_98DX1123_CNS       = 0xDCB311AB,              24        0        0        4        0        0
 *   CPSS_98DX1222_CNS       = 0xDCF111AB,              24        0        0        4        0        0
 *   CPSS_98DX1223_CNS       = 0xDD7711AB,              24        0        0        4        0        0
 *
 *   *98DX2100 Series: Bobcat-FE (L3+ / Metro) - 800MHz, 16KB/256K Cache*
 *   CPSS_98DX2101_CNS       = 0xDD9111AB,               8        0        0        2        0        0
 *   CPSS_98DX2122_CNS       = 0xDDB111AB,              24        0        0        4        0        0
 *   CPSS_98DX2123_CNS       = 0xDDB311AB,              24        0        0        4        0        0
 *   CPSS_98DX2151_CNS       = 0xDD7511AB,              24        0        0        2        0        2
 *
 *   *98DX3000 Series: AlleyCat-GE (Entry L2) UNMANAGED- 333MHz, 0K Cache, 256KB Internal SRAM *
 *   CPSS_98DX3011_CNS       = 0xDE2011AB,              0        16        2        0        0        0
 *   CPSS_98DX3021_CNS       = 0xDE3011AB,              0        24        2        0        0        0
 *
 *   *98DX3000 Series: AlleyCat-GE WEBSMART- 333MHz, 0K Cache, SPI, DDR2 *
 *   CPSS_98DX3005_CNS       = 0xDF5011AB               0         8        2        0        0        0
 *   CPSS_98DX3015_CNS       = 0xDFE411AB               0        16        4        0        0        0
 *   CPSS_98DX3025_CNS       = 0xDFF411AB               0        24        4        0        0        0
 *   CPSS_98DX3026_CNS       = 0xDF7411AB               0        24        0        2        0        2
 *   CPSS_98DX3027_CNS       = 0xDF7611AB               0        24        0        2        0        2
 *   CPSS_98DXJ324_CNS       = 0xDF3411AB               0        24        4        0        0        0
 *
 *   *98DX303x Series: PonCat-GE *
 *   CPSS_98DX3002_CNS       = 0xDE1411AB               0        8        4        0        0        0
 *   CPSS_98DX3031_CNS       = 0xDE3411AB               0       24        4        0        0        0
 *   CPSS_98DX3032_CNS       = 0xDE7411AB               0       24        0        0        0        4
 *   CPSS_98DX3034_CNS       = 0xDEF411AB               0       24        0        2        0        2
 *
 *   *98DX3100 Series: TomCat-GE (SMB L2+) - 600MHz, 16KB/128KB Cache*
 *   CPSS_98DX3101_CNS       = 0xDC5011AB,              0         8        2        0        0        0
 *   CPSS_98DX3121_CNS       = 0xDCF011AB,              0        24        0        2        2        0
 *   CPSS_98DX3122_CNS       = 0xDCB411AB,              0        24        0        0        0        4
 *   CPSS_98DX3123_CNS       = 0xDCB611AB,              0        24        0        0        0        4
 *   CPSS_98DX3124_CNS       = 0xDC7411AB,              0        24        0        0        2        2
 *   CPSS_98DX3125_CNS       = 0xDC7611AB,              0        24        0        0        2        2
 *
 *   *98DX3200 Series: TomCat-METRO - 600MHz, 16KBKB/128KB Cache
 *   CPSS_98DX3201_CNS       = 0xDC9011AB,              0         8        0        4        0        0
 *   CPSS_98DX3222_CNS       = 0xDC3411AB,              0        24        0        0        0        4
 *   CPSS_98DX3223_CNS       = 0xDCF211AB,              0        24        0        0        0        4
 *
 *   *98DX4100 Series: Bobcat-GE (L3+ / Metro) - 800MHz,16KB/256KB Cache*
 *   CPSS_98DX4101_CNS       = 0xDD9011AB,              0         8        0        4        0        0
 *   CPSS_98DX4121_CNS       = 0xDDB011AB,              0        24        0        4        0        0
 *   CPSS_98DX4122_CNS       = 0xDD7411AB,              0        24        0        0        0        4
 *   CPSS_98DX4123_CNS       = 0xDDB611AB,              0        24        0        0        0        4
 *   CPSS_98DX4124_CNS       = 0xDDF411AB               0        24        0        0        2        2
 *   CPSS_98DX4125_CNS       = 0xDDF611AB               0        24        0        0        2        2
 *   CPSS_98DX5248_CNS       = 0xDD7611AB               0        24        0        0        0        4
 *   CPSS_98DX5258_CNS       = 0xDDB411AB               0        24        0        0        0        4
 *   CPSS_98DX5158_CNS       = 0xDDB411AB               0        24        0        0        0        4
 *   CPSS_98DX5159_CNS       = 0xDD3611AB               0        24        0        0        0        4
 *   CPSS_98DX5168_CNS       = 0xDD5411AB               0        12        0        0        0        2
 *   CPSS_98DXH525_CNS       = 0xDDE011AB               0        24        0        4        0        0
 *   CPSS_98DXH426_CNS       = 0xDD7011AB               0        24        0        2        0        2
 *
 *   *Obsolete devices - used for testing purposes only*
 *   CPSS_98DX3110_CNS       = 0xDC0211AB               0        24        4        0        0        0
 *   CPSS_98DX3110_1_CNS     = 0xDC0011AB               0        24        4        0        0        0
 *   CPSS_98DX5313_CNS       = 0xDDB211AB               0        24        0        0        0        4
 *
 *   *98DXH300 Series: TomCat-GE (DHX) - 333MHz, 16KB/128KB Cache, SPI/NAND, DDR2
 *   CPSS_98DXH325_CNS       = 0xDFB011AB               0        24        4        0        0        0
 *   CPSS_98DXH326_CNS       = 0xDFB411AB               0        24        2        0        0        2
 *   CPSS_98DXH327_CNS       = 0xDFB611AB               0        24        2        0        0        2
 *
 *   *98DXH100 Series: TomCat-GE (DHX) - 333MHz, 16KB/128KB Cache*
 *   CPSS_98DXH125_CNS       = 0xDFB111AB               24        0        4        0        0        0
 *   CPSS_98DXH126_CNS       = 0xDFB511AB               24        0        2        2        0        0
 *   CPSS_98DXH127_CNS       = 0xDFB711AB               24        0        2        2        0        0
 */
typedef GT_U32  CPSS_PP_DEVICE_TYPE;

#define    CPSS_98MX610B_CNS                  0x007011AB
#define    CPSS_98MX620B_CNS                  0x004011AB
#define    CPSS_98MX610BS_CNS                 0x006011AB
#define    CPSS_98EX110BS_CNS                 0x006711AB
#define    CPSS_98EX111BS_CNS                 0x006311AB
#define    CPSS_98EX112BS_CNS                 0x006511AB
#define    CPSS_98EX110B_CNS                  0x007711AB
#define    CPSS_98EX111B_CNS                  0x007311AB
#define    CPSS_98EX112B_CNS                  0x007511AB
#define    CPSS_98EX120B_CNS                  0x005711AB
#define    CPSS_98EX120B__CNS                 0x005611AB
#define    CPSS_98EX121B_CNS                  0x005311AB
#define    CPSS_98EX122B_CNS                  0x005511AB
#define    CPSS_98EX128B_CNS                  0x005011AB
#define    CPSS_98EX129B_CNS                  0x005211AB

#define    CPSS_98EX100D_CNS                  0x00EF11AB
#define    CPSS_98EX110D_CNS                  0x00E711AB
#define    CPSS_98EX115D_CNS                  0x00E111AB
#define    CPSS_98EX110DS_CNS                 0x00F711AB
#define    CPSS_98EX115DS_CNS                 0x00F111AB
#define    CPSS_98EX120D_CNS                  0x00D711AB
#define    CPSS_98EX125D_CNS                  0x00D111AB
#define    CPSS_98EX130D_CNS                  0x01D711AB
#define    CPSS_98EX135D_CNS                  0x01D111AB

/* samba devices */
#define    CPSS_98MX615_CNS                   0x01A011AB
#define    CPSS_98MX625_CNS                   0x018011AB
#define    CPSS_98MX625V0_CNS                 0x019011AB
 /* 12G Tiger as 98EX126 without External SRAMs */

#define    CPSS_98MX635_CNS                   0x01D011AB
#define    CPSS_98MX618_CNS                   0x00A011AB
#define    CPSS_98MX628_CNS                   0x009011AB
#define    CPSS_98MX638_CNS                   0x00D011AB

/* Salsa devices  */
#define    CPSS_98DX240_CNS                 0x0D1411AB
#define    CPSS_98DX160_CNS                 0x0D1511AB
#define    CPSS_98DX120_CNS                 0x0D1611AB
#define    CPSS_98DX241_CNS                 0x0D1011AB
#define    CPSS_98DX161_CNS                 0x0D1111AB
#define    CPSS_98DX121_CNS                 0x0D1211AB

/* Sapphire device */
#define    CPSS_88E6183_CNS                   0x01A311AB

/* Ruby device */
#define    CPSS_88E6093_CNS                   0x009311AB

/* Opal device */
#define    CPSS_88E6095_CNS                   0x009511AB

/* Jade device */
#define    CPSS_88E6185_CNS                   0x01A711AB
#define    CPSS_88E6155_CNS                   0x01A511AB

/* Cheetah devices */
#define    CPSS_98DX270_CNS                   0xD80411AB
#define    CPSS_98DX260_CNS                   0xD80C11AB
#define    CPSS_98DX250_CNS                   0xD81411AB

/* Cheetah+ devices */
#define    CPSS_98DX243_CNS                   0xD2D411AB
#define    CPSS_98DX273_CNS                   0xD80011AB
#define    CPSS_98DX263_CNS                   0xD80811AB
#define    CPSS_98DX253_CNS                   0xD81011AB
#define    CPSS_98DX107_CNS                   0xD81311AB
#define    CPSS_98DX106_CNS                   0xD3D311AB
#define    CPSS_98DX133_CNS                   0xD80D11AB
#define    CPSS_98DX163_CNS                   0xD2D511AB
#define    CPSS_98DX166_CNS                   0xD81511AB
#define    CPSS_98DX167_CNS                   0xD81111AB
/* 98DX167 A2 device has same ID as 98DX166 */
#define    CPSS_98DX167_A2_CNS                0xD81511AB
#define    CPSS_98DX249_CNS                   0xD82611AB
#define    CPSS_98DX269_CNS                   0xD82411AB
#define    CPSS_98DX169_CNS                   0xD82711AB

/* Cheetah2 devices */
#define    CPSS_98DX255_CNS                   0xD91411AB
#define    CPSS_98DX265_CNS                   0xD90C11AB
#define    CPSS_98DX275_CNS                   0xD90411AB
#define    CPSS_98DX285_CNS                   0xD91C11AB
#define    CPSS_98DX125_CNS                   0xD91511AB
#define    CPSS_98DX145_CNS                   0xD90D11AB
#define    CPSS_98DX804_CNS                   0xD91811AB

/* Cheetah3 24 Gig. ports devices */
#define    CPSS_98DX5128_CNS                   0xDB0011AB
#define    CPSS_98DX5128_1_CNS                 0xDB1011AB
#define    CPSS_98DX5124_CNS                   0xDB4011AB
#define    CPSS_98DX5126_CNS                   0xDB6011AB
#define    CPSS_98DX5127_CNS                   0xDB7011AB
#define    CPSS_98DX5129_CNS                   0xDB9011AB
#define    CPSS_98DX5151_CNS                   0xDB1211AB
#define    CPSS_98DX5151_1_CNS                 0xDB1811AB
#define    CPSS_98DX5152_CNS                   0xDB2211AB
#define    CPSS_98DX5152_1_CNS                 0xDB2811AB
#define    CPSS_98DX5154_CNS                   0xDB4211AB
#define    CPSS_98DX5154_1_CNS                 0xDB4811AB
#define    CPSS_98DX5155_CNS                   0xDB5211AB
#define    CPSS_98DX5155_1_CNS                 0xDB5811AB
#define    CPSS_98DX5156_CNS                   0xDB6211AB
#define    CPSS_98DX5156_1_CNS                 0xDB6811AB
#define    CPSS_98DX5157_CNS                   0xDB7211AB
#define    CPSS_98DX5157_1_CNS                 0xDB7811AB

/* Cheetah3 XG ports devices */
#define    CPSS_98DX8110_CNS                   0xDB0111AB
#define    CPSS_98DX8108_CNS                   0xDB8111AB
#define    CPSS_98DX8109_CNS                   0xDB9111AB
#define    CPSS_98DX8110_1_CNS                 0xDB1111AB
#define    CPSS_98DX8111_CNS                   0xDBB111AB

/* Cheetah3 devices - "for testing purposes".  */
#define    CPSS_98DX287_CNS                   0xDB0F11AB

/* Tiger devices */
#define    CPSS_98EX116_CNS                   0x012111AB
#define    CPSS_98EX106_CNS                   0x012A11AB
#define    CPSS_98EX108_CNS                   0x012B11AB
#define    CPSS_98EX126_CNS                   0x011111AB
#define    CPSS_98EX136_CNS                   0x015111AB

#define    CPSS_98EX126V0_CNS                 0x011311AB

/* Tiger Drop In devices */
#define    CPSS_98EX116DI_CNS                 0x012511AB
#define    CPSS_98EX126DI_CNS                 0x011511AB
#define    CPSS_98EX136DI_CNS                 0x015511AB

/* Cheetah reduced devices */
#define    CPSS_98DX248_CNS                   0xD85411AB
#define    CPSS_98DX262_CNS                   0xD84E11AB
#define    CPSS_98DX268_CNS                   0xD84C11AB
#define    CPSS_98DX803_CNS                   0xD80611AB

/* Puma devices : 24 G devices */
#define    CPSS_98EX240_CNS                   0xC24011AB
/* Puma 24G external TCAM disabled */
#define    CPSS_98EX240_1_CNS                 0xC20011AB
#define    CPSS_98MX840_CNS                   0x222211AB /* dummy number */
/* Puma devices : 2 XG devices */
#define    CPSS_98EX260_CNS                   0xC24111AB
#define    CPSS_98MX860_CNS                   0x444411AB /* dummy number */

/* Puma2 devices : 24 G devices */
#define    CPSS_98EX2106_CNS                   0xC30011AB
#define    CPSS_98EX2206_CNS                   0xC34011AB
#define    CPSS_98MX2306_CNS                   0xC36011AB
#define    CPSS_98EX8261_CNS                   0xC34211AB
#define    CPSS_98EX8301_CNS                   0xC34411AB
#define    CPSS_98EX8303_CNS                   0xC30411AB
#define    CPSS_98EX8501_CNS                   0xC30611AB

/*Puma2 lite : 24 G device */
#define    CPSS_98EX8306_CNS                   0xC30811AB


/* Puma2 devices : 2 port XG devices */
#define    CPSS_98EX2110_CNS                   0xC30111AB
#define    CPSS_98EX2210_CNS                   0xC34111AB
#define    CPSS_98MX2310_CNS                   0xC36111AB
#define    CPSS_98EX8262_CNS                   0xC34311AB
#define    CPSS_98EX8302_CNS                   0xC34511AB
#define    CPSS_98EX8305_CNS                   0xC30511AB
#define    CPSS_98EX8502_CNS                   0xC30711AB

/*Puma2 lite : 2 XG device */
#define    CPSS_98EX8307_CNS                   0xC30911AB

/* Ocelot device */
#define    CPSS_98FX950_CNS                    0xF95011AB

/**************************/                      /*  FE       GE       Stack  Stack     Stack     Stack         */
/****  xCat device start **/                      /*                     GE     GE/2.5GE  GE/2.5/   GE/2.5/      */
/**************************/                      /*                           (DQX)     10/12GE   5/10/12GE     */
                                                  /*                                     (DQX/DHX) (DQX/DHX/XAUI)*/

/*98DX1000 Series: AlleyCat-FE WEBSMART- 333MHz, 0K Cache, SPI, DDR2*/
#define    CPSS_98DX1005_CNS        0xDF5111AB    /*     8        0        2        0        0        0   */
#define    CPSS_98DX1025_CNS        0xDFF511AB    /*    24        0        4        0        0        0   */
#define    CPSS_98DX1026_CNS        0xDF7111AB    /*    24        0        2        2        0        0   */
#define    CPSS_98DX1027_CNS        0xDF7311AB    /*    24        0        2        2        0        0   */

/*98DX102x Series: PonCat-FE */
#define    CPSS_98DX1002_CNS        0xDFD111AB    /*     8        0        3        0        0        0   */
#define    CPSS_98DX1022_CNS        0xDFF111AB    /*    24        0        3        0        0        0   */
#define    CPSS_98DX1023_CNS        0xDFF311AB    /*    24        0        3        0        0        0   */

/*98DX1100/1200 Series: TomCat-FE (SMB L2+) - 600MHz, 16KBKB/128KB Cache  */
#define    CPSS_98DX1122_CNS        0xDCB111AB    /*    24        0        0        4        0        0   */
#define    CPSS_98DX1123_CNS        0xDCB311AB    /*    24        0        0        4        0        0   */
#define    CPSS_98DX1222_CNS        0xDCF111AB    /*    24        0        0        4        0        0   */
#define    CPSS_98DX1223_CNS        0xDD7711AB    /*    24        0        0        4        0        0   */

/*98DX2100 Series: Bobcat-FE (L3+ / Metro) - 800MHz, , 16KB/256K Cache*/
#define    CPSS_98DX2101_CNS        0xDD9111AB    /*     8        0        0        4        0        0   */
#define    CPSS_98DX2122_CNS        0xDDB111AB    /*    24        0        0        4        0        0   */
#define    CPSS_98DX2123_CNS        0xDDB311AB    /*    24        0        0        4        0        0   */
#define    CPSS_98DX2151_CNS        0xDD7511AB    /*    24        0        0        2        0        2   */

/*98DX3000 Series: AlleyCat-GE (Entry L2) UNMANAGED- 333MHz, 0K Cache, 256KB Internal SRAM */
#define    CPSS_98DX3011_CNS        0xDE2011AB    /*    0        16        2        0        0        0   */
#define    CPSS_98DX3021_CNS        0xDE3011AB    /*    0        24        2        0        0        0   */

/*98DX3000 Series: AlleyCat-GE WEBSMART- 333MHz, 0K Cache, SPI, DDR2*/
#define    CPSS_98DX3005_CNS        0xDF5011AB    /*    0         8        2        0        0        0   */
#define    CPSS_98DX3015_CNS        0xDFE411AB    /*    0        16        4        0        0        0   */
#define    CPSS_98DX3025_CNS        0xDFF411AB    /*    0        24        4        0        0        0   */
#define    CPSS_98DX3026_CNS        0xDF7411AB    /*    0        24        0        2        0        2   */
#define    CPSS_98DX3027_CNS        0xDF7611AB    /*    0        24        0        2        0        2   */
#define    CPSS_98DXJ324_CNS        0xDF3411AB    /*    0         24       4        0        0        0   */

/*98DX303x Series: PonCat-GE */
#define    CPSS_98DX3002_CNS        0xDE1411AB    /*     0        8        4        0        0        0   */
#define    CPSS_98DX3031_CNS        0xDE3411AB    /*     0       24        4        0        0        0   */
#define    CPSS_98DX3032_CNS        0xDE7411AB    /*     0       24        0        0        0        4   */
#define    CPSS_98DX3034_CNS        0xDEF411AB    /*     0       24        0        2        0        2   */

/*98DX3100 Series: TomCat-GE (SMB L2+) - 600MHz, 16KBKB/128KB Cache*/
#define    CPSS_98DX3101_CNS        0xDC5011AB    /*    0         8        2        0        0        0   */
#define    CPSS_98DX3121_CNS        0xDCF011AB    /*    0        24        0        2        2        0   */
#define    CPSS_98DX3122_CNS        0xDCB411AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX3123_CNS        0xDCB611AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX3124_CNS        0xDC7411AB    /*    0        24        0        0        2        2   */
#define    CPSS_98DX3125_CNS        0xDC7611AB    /*    0        24        0        0        2        2   */

/*98DX3200 Series: TomCat-METRO - 600MHz, 16KBKB/128KB Cache*/
#define    CPSS_98DX3201_CNS        0xDC9011AB    /*    0         8        0        4        0        0   */
#define    CPSS_98DX3222_CNS        0xDC3411AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX3223_CNS        0xDCF211AB    /*    0        24        0        0        0        4   */

/*98DX4100 Series: Bobcat-GE (L3+ / Metro) - 800MHz,1,16KB/256KB Cache*/
#define    CPSS_98DX4101_CNS        0xDD9011AB    /*    0         8        0        4        0        0   */
#define    CPSS_98DX4121_CNS        0xDDB011AB    /*    0        24        0        4        0        0   */
#define    CPSS_98DX4122_CNS        0xDD7411AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX4123_CNS        0xDDB611AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX4124_CNS        0xDDF411AB    /*    0        24        0        0        2        2   */
#define    CPSS_98DX4125_CNS        0xDDF611AB    /*    0        24        0        0        2        2   */
#define    CPSS_98DX5248_CNS        0xDD7611AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX5258_CNS        0xDD9411AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX5158_CNS        0xDDB411AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX5159_CNS        0xDD3611AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX5168_CNS        0xDD5411AB    /*    0        12        0        0        0        2   */
#define    CPSS_98DXH525_CNS        0xDDE011AB    /*    0        24        0        4        0        0   */
#define    CPSS_98DXH426_CNS        0xDD7011AB    /*    0        24        0        2        0        2   */
#define    CPSS_98DX4A01_CNS        0xDEB011AB    /*    0        24        0        0        0        4   */
#define    CPSS_98DX4A02_CNS        0xDE2411AB    /*    0        24        0        0        0        4   */

/* Obsolete devices - used for testing purposes only */
#define    CPSS_98DX3110_CNS        0xDC0211AB    /*    0        24       4        0        0        0   */
#define    CPSS_98DX3110_1_CNS      0xDC0011AB    /*    0        24       4        0        0        0   */
#define    CPSS_98DX5313_CNS        0xDDB211AB    /*    0        24       0        0        0        4   */

/* 98DXH300 Series: TomCat-GE (DHX) - 333MHz, 16KB/128KB Cache, SPI/NAND, DDR2 */
#define    CPSS_98DXH325_CNS        0xDFB011AB    /*    0        24        4        0        0        0   */
#define    CPSS_98DXH326_CNS        0xDFB411AB    /*    0        24        2        0        0        2   */
#define    CPSS_98DXH327_CNS        0xDFB611AB    /*    0        24        2        0        0        2   */

/* 98DXH100 Series: TomCat-FE (DHX) - 333MHz, 16KB/128KB Cache*/
#define    CPSS_98DXH125_CNS        0xDFB111AB    /*    24        0        4        0        0        0   */
#define    CPSS_98DXH126_CNS        0xDFB511AB    /*    24        0        2        2        0        0   */
#define    CPSS_98DXH127_CNS        0xDFB711AB    /*    24        0        2        2        0        0   */


/************************/
/****  xCat device end **/
/************************/

/**************************/                  /*    FE      GE      XG      Stack   Stack   */
/***  xCat3 device start **/                  /*                             GE      XG     */
/**************************/

/* Series: AlleyCat3-FE*/
#define    CPSS_98DX1233_CNS    0xF40811AB    /*     8       0       0        2       0     */
#define    CPSS_98DX1235_CNS    0xF40911AB    /*     24      0       0        4       0     */

/* Series: AlleyCat3-GE*/
#define    CPSS_98DX3223_1_CNS  0xF40711AB    /*     0       8       0        4       0     */
#define    CPSS_98DX3224_CNS    0xF40611AB    /*     0      16       0        4       0     */
#define    CPSS_98DX3225_CNS    0xF40511AB    /*     0      24       0        4       0     */
#define    CPSS_98DX3226_CNS    0xF40411AB    /*     0      24       0        0       4     */
#define    CPSS_98DX3233_CNS    0xF41311AB    /*     0       8       0        4       0     */
#define    CPSS_98DX3234_CNS    0xF41211AB    /*     0      16       0        4       0     */
#define    CPSS_98DX3235_CNS    0xF41111AB    /*     0      24       0        4       0     */
#define    CPSS_98DX3236_CNS    0xF41011AB    /*     0      24       0        0       4     */
#define    CPSS_98DXH333_CNS    0xF41411AB    /*     0       8       0        4       0     */
#define    CPSS_98DXT323_CNS    0xF41511AB    /*     0      24       0        4       0     */
#define    CPSS_98DXT321_CNS    0xF41611AB    /*     0      24       0        2       0     */

#define    CPSS_98DXC323_CNS    0xF41B11AB    /*     0      24       0        2       2     */
#define    CPSS_98DXN323_CNS    0xF41C11AB    /*     0      24       0        0       4     */

/* Series: PonCat3-FE*/
#define    CPSS_98DX1333_CNS    0xF41811AB    /*     8       0       0        2       0     */
#define    CPSS_98DX1335_CNS    0xF41911AB    /*     24      0       0        4       0     */
#define    CPSS_98DX1336_CNS    0xF41A11AB    /*     24      0       0        0       4     */

/* Series: PonCat3-GE*/
#define    CPSS_98DX3333_CNS    0xF40311AB    /*     0       8       0        4       0     */
#define    CPSS_98DX3334_CNS    0xF40211AB    /*     0      16       0        4       0     */
#define    CPSS_98DX3335_CNS    0xF40111AB    /*     0      24       0        4       0     */
#define    CPSS_98DX3336_CNS    0xF40011AB    /*     0      24       0        0       4     */

/************************/
/***  xCat3 device end **/
/************************/

/**********************/
/* start Lion devices */
/**********************/
#define    CPSS_98CX8248_CNS    0xE01311AB    /* 48 ports, RXAUI             */
#define    CPSS_98CX8248_1_CNS  0xE01F11AB    /* 48 ports, RXAUI             */
#define    CPSS_98CX8112_CNS    0xE01211AB    /* 12 ports, RXAUI             */
#define    CPSS_98CX8224_CNS    0xE00111AB    /* 24 ports, XAUI              */
#define    CPSS_98CX8224_1_CNS  0xE00D11AB    /* 24 ports, XAUI              */
#define    CPSS_98CX8203_CNS    0xE01111AB    /* 24 ports , RXAUI, port groups 0,1 */
#define    CPSS_98CX8113_CNS    0xE0C111AB    /* 24 port XAUI */
#define    CPSS_98CX8222_CNS    0xE08511AB    /* 16 XAUI + 2 * 40GbE   */
#define    CPSS_98CX8223_CNS    0xE09611AB    /* 24 RXAUI + 2 * 40GbE  */
#define    CPSS_98CX8226_CNS    0xE01D11AB    /* 24 ports, XAUI              */
#define    CPSS_98CX8234_CNS    0xE09711AB    /* 32 RXAUI + 4 * 40GbE  */
#define    CPSS_98CX8265_CNS    0xE04111AB    /* 48 port RXAUI         */
#define    CPSS_98CX8266_CNS    0xE05311AB    /* 24 port XAUI          */
#define    CPSS_98CX8267_CNS    0xE05F11AB    /*  4*XAUI + 8*RXAUI/4*DX + 16*HGS2       */
#define    CPSS_98DX5198_CNS    0xE02311AB    /* 44*1GbE/ 5G + 4*1GbE/2.5G/RXAUI        */
#define    CPSS_98DX5178_CNS    0xE02B11AB    /* 24*1GbE/ 5G + 8*1GbE/2.5G/RXAUI + 160G */
#define    CPSS_98FX9216_CNS    0xE03111AB    /* 16*HGL/  RHGL  Crossbar                */

/***********************/
/* End Of Lion devices */
/***********************/

/**********************/
/* start Lion2 devices */
/**********************/
/* Lion2 Data Center / Metro devices */
#define    CPSS_98CX8296_CNS    0xEC0511AB
#define    CPSS_98CX8297_CNS    0xEC0211AB
#define    CPSS_98CX8308_CNS    0xEC0811AB
#define    CPSS_98CX8297_1_CNS  0xEC0011AB
/* Fandago2 Switch Fabrics */
#define    CPSS_98FX9287_CNS    0xEC0D11AB
#define    CPSS_98FX9288_CNS    0xEC0C11AB

/***********************/
/* End Of Lion2 devices */
/***********************/

/******************************/
/* start Lion2 Hooper devices */
/******************************/
#define    CPSS_98CX8121_CNS    0x800011AB /* also used as dummy to support distributed Device_ID: in register 0x4c and DFX register - don't delete */
#define    CPSS_98CX8123_CNS    0x800111AB
#define    CPSS_98CX8124_CNS    0x800211AB
#define    CPSS_98CX8129_CNS    0x800311AB

/*******************************/
/* End Of Lion2 Hooper devices */
/*******************************/


/**********************/
/* start Lion3 devices */
/**********************/
#define    CPSS_98CXEEEF_CNS    0xEEEF11AB    /* dummy for simulation - Bobcat2; Caelum; Bobcat3 device  */

/***********************/
/* End Of Lion3 devices */
/***********************/

/**********************/                       /*            XAUI/DXAUI                                                          */
/* Start Bobcat2 devices */                    /*           RXAUI/DRXAUI                                                         */
/**********************/                       /*  Serdes     HX/DHX        10G-KR        20G-KR        40G-KR        Interlaken */
#define    CPSS_98DX4251_CNS        0xFC0011AB /*  36            6           24              12              6           1       */
#define    CPSS_98DX4220_CNS        0xFC0311AB /*  20            3           12              6               3           0       */
#define    CPSS_98DX8216_CNS        0xFC0411AB /*  16            4           16              8               4           0       */
#define    CPSS_98DX8224_CNS        0xFC0511AB /*  24            6           24             12               6           0       */
#define    CPSS_98DX4221_CNS        0xFC0211AB /*  26            3           12              6               3           0       */
#define    CPSS_98DX4222_CNS        0xFC0611AB /*  24            3           12              6               3           0       */
#define    CPSS_98DX8219_CNS        0xFC0811AB /*  26            5           20             10               5           0       */
#define    CPSS_98DX4223_CNS        0xFC0911AB /*  36            6           24             12               6           0       */
#define    CPSS_98DX4253_CNS        0xFC0711AB /*  14            4            8              0               0           0       */

#define    CPSS_98DX42XX_CNS        0xABBA11AB /* dummy for simulation - Read from PCI register of GM SIP5 */

/***********************/
/* End Of Bobcat2 devices */
/***********************/
/**********************/                       /*            XAUI/DXAUI                                                          */
/* Start BobK devices */                       /*           RXAUI/DRXAUI                                                         */
/**********************/                       /*  Serdes     HX/DHX        10G-KR        20G-KR        40G-KR        Interlaken */
                        /* dummy for simulation - Caelum */
#define    CPSS_98DX42KK_CNS        0xBE0111AB /*  25            3           12              6               3           0       */
                        /* Caelum */
#define    CPSS_98DX4203_CNS        0xBC0011AB /*  25            3           12              6               3           0       */
#define    CPSS_98DX4204_CNS        0xBC0111AB /*  19            3           12              6               3           0       */
#define    CPSS_98DX4211_CNS        0xBC1011AB /*  25            3           12              6               3           0       */
#define    CPSS_98DX4210_CNS        0xBC1111AB /*  19            3           12              6               3           0       */
                        /* Cetus */
#define    CPSS_98DX4235_CNS        0xBE0011AB /*  13            3           12              6               3           0       */
#define    CPSS_98DX8212_CNS        0xBE1011AB /*  13            3           12              6               3           0       */
#define    CPSS_98DX8208_CNS        0xBE1111AB /*   9            2            8              4               2           0       */

/***********************/
/* End Of BobK devices */
/***********************/

/************************/                     /*            XAUI/DXAUI                                                          */
/* Start Aldrin devices */                     /*           RXAUI/DRXAUI                                                         */
/************************/                     /*  Serdes     HX/DHX        10G-KR        20G-KR        40G-KR        Interlaken */
#define    CPSS_98DX8316_CNS        0xC81D11AB /*  16            4/8         16              8               4           0       */
#define    CPSS_98DX8324_CNS        0xC81E11AB /*  24            6/12        24              12              6           0       */
#define    CPSS_98DX8332_CNS        0xC81F11AB /*  32            8/16        32              16              8           0       */
/***********************/
/* End Of Aldrin devices */
/***********************/

/*************************/                       /*                                                                    */
/* Start Bobcat3 devices */                       /*                                                                    */
/*************************/                       /*  Serdes    10/25G-KR        40G/100G-KR      */
                        /* dummy for simulation - Bobcat3 */
#define    CPSS_98DX42FF_CNS        0xFF0011AB    /*  72           72                 18          */
/***********************/
/* End Of BobK devices */
/***********************/



/****************************/                    /*  FE         GE      Stack       Stack         */
/****  xCat2 device start **/                     /*                     GE/2.5GE    GE/2.5/       */
/****************************/                    /*                     (DQX)       5/10/12GE     */

/* AlleyCat2-GE UNMANAGED */
#define    CPSS_98DX3011B_CNS       0xE66211AB    /*     0       16       2        0    */
#define    CPSS_98DX3011C_CNS       0xE46011AB    /*     0       16       4        0    */
#define    CPSS_98DX3021B_CNS       0xE67211AB    /*     0       24       2        0    */
#define    CPSS_98DX3021C_CNS       0xE47011AB    /*     0       24       4        0    */

/* AlleyCat2-FE WEBSMART- 333MHz, 0K Cache */
#define    CPSS_98DX1033_CNS        0xE75B11AB    /*     8        0       2         0   */
#define    CPSS_98DX1034_CNS        0xE76B11AB    /*    16        0       4         0   */
#define    CPSS_98DX1035_CNS        0xE77B11AB    /*    24        0       4         0   */
#define    CPSS_98DX1053_CNS        0xE50111AB    /*     8        0       2         0   */

/* Obsolete device - used for testing purposes only */
#define    CPSS_98DX1035_1_CNS      0xE5A711AB    /*    24        0       4         0   */

#define    CPSS_98DX1055_CNS        0xE40111AB    /*    24        0       4         0   */

/* AlleyCat2-GE WEBSMART- 333MHz, 0K Cache */
#define    CPSS_98DX3027B_CNS       0xE77611AB    /*     0       24       4         0   */
#define    CPSS_98DX3033B_CNS       0xE75A11AB    /*     0        8       2         0   */
#define    CPSS_98DX3034B_CNS       0xE7EE11AB    /*     0       16       4         0   */
#define    CPSS_98DX3035B_CNS       0xE7FE11AB    /*     0       24       4         0   */
#define    CPSS_98DX3053_CNS        0xE42011AB    /*     0        8       2         0   */
#define    CPSS_98DX3054_CNS        0xE41011AB    /*     0       16       4         0   */
#define    CPSS_98DX3055_CNS        0xE40011AB    /*     0       24       4         0   */
#define    CPSS_98DX3036_CNS        0xDF7011AB    /*     0       24       0         4   */ /*XCAT*/
#define    CPSS_98DX3036A_CNS       0xDF7C11AB    /*     0       24       0         4   */ /*XCAT*/

/* PonCat2-FE */
#define    CPSS_98DX1133_CNS        0xE7DB11AB    /*     8        0       2         0   */
#define    CPSS_98DX1134_CNS        0xE7EB11AB    /*    16        0       4         0   */
#define    CPSS_98DX1135_CNS        0xE7FB11AB    /*    24        0       4         0   */
#define    CPSS_98DX1163_CNS        0xE55111AB    /*     8        0       2         0   */
#define    CPSS_98DX1165_CNS        0xE44111AB    /*    24        0       4         0   */

/* PonCat2-GE */
#define    CPSS_98DX3031B_CNS       0xE63611AB    /*     0       24       4         0   */
#define    CPSS_98DX3133_CNS        0xE61E11AB    /*     0        8       4         0   */
#define    CPSS_98DX3134_CNS        0xE62E11AB    /*     0       16       4         0   */
#define    CPSS_98DX3135_CNS        0xE63E11AB    /*     0       24       4         0   */
#define    CPSS_98DXZ365_CNS        0xE6BE11AB    /*     0       24       4         0   */
#define    CPSS_98DX3163_CNS        0xE45011AB    /*     0        8       4         0   */
#define    CPSS_98DX3165_CNS        0xE44011AB    /*     0       24       4         0   */
#define    CPSS_98DX3136_CNS        0xDEF011AB    /*     0       24       0         4   */ /*XCAT*/
#define    CPSS_98DXZ366_CNS        0xDC3011AB    /*     0       24       0         4   */ /*XCAT*/

/* VonCat2-FE  */
#define    CPSS_98DX1083_CNS        0xE58B11AB    /*     8        0       4         0   */
#define    CPSS_98DX1085_CNS        0xE5AB11AB    /*    24        0       4         0   */

/* VonCat2-GE */
#define    CPSS_98DX3083_CNS        0xE5BA11AB    /*     0        8       4         0   */
#define    CPSS_98DX3085_CNS        0xE5CA11AB    /*     0       24       4         0   */

/* AlleyCat2-GE Websmart (DHX) */
#define    CPSS_98DXH335_CNS        0xE7B211AB    /*     0        24       4         0   */
#define    CPSS_98DXH336_CNS        0xDFF011AB    /*     0        24       2         2   */ /*XCAT*/
#define    CPSS_98DXH337_CNS        0xDF3211AB    /*     0        24       2         2   */ /*XCAT*/

/*************************/
/* End Of xCat2 devices */
/*************************/

/* PCI quirks for PEX devices
 * Currently used for Linux only
 */
#define PCI_RESOURCE_PSS_REGS_PCI   1
#define PCI_RESOURCE_PSS_REGS_PEX   2
#define PCI_CONFIG_SIZE (4*1024)
#define PEX_CONFIG_SIZE 0x10000
#define MSYS_CONFIG_SIZE 0x100000
/*
 * Typedef: struct prvPciDeviceQuirks
 *
 * Description: PCI devices quirks
 *
 * Fields:
 *      pciId           Device PCI ID
 *      isPex           Flags that device is PEX connected
 *      configOffset    Some? PEX PP has config started at offset 0x70000 in
 *                      config address space, access to addresses below this
 *                      offset causes to hang system.
 *                      If packet processor is listed here then memory should
 *                      be mapped with offset added for both source(hw) and
 *                      destination (VM) addresses.
 *                      This will protect us from accessing to invalid addresses
 *                      causes just SEGV which can be easy hooked with gdb
 *      configSize      The PEX devices has longer config, so add this value here
 *      pciResourceReg  PCI resource number for registers address space (used by
 *                      mvPpDrv)
 *      hasDfx          The DFX registers are follows PP registers (64MBytes)
 */

/* macro to set PEX entry in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_PEX_ENTRY_MAC(pciId,pexOffset,dfx) \
    { (pciId),  GT_TRUE, pexOffset, PEX_CONFIG_SIZE, PCI_RESOURCE_PSS_REGS_PEX, dfx }

/* macro to set MSYS entry in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(pciId, dfx) \
    { (pciId),  GT_TRUE,  0, MSYS_CONFIG_SIZE, PCI_RESOURCE_PSS_REGS_PEX, dfx }

/* macro to set PEX 0x00000 entry in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_PEX_0x00000_ENTRY_MAC(pciId) \
    PRV_CPSS_DEVICE_QUIRKS_PEX_ENTRY_MAC(pciId,0x00000,GT_FALSE)

/* macro to set PEX 0x40000 entry in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_PEX_0x40000_ENTRY_MAC(pciId) \
    PRV_CPSS_DEVICE_QUIRKS_PEX_ENTRY_MAC(pciId,0x40000,GT_FALSE)

/* macro to set PEX 0x70000 entry in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(pciId) \
    PRV_CPSS_DEVICE_QUIRKS_PEX_ENTRY_MAC(pciId,0x70000,GT_FALSE)

/* macro to set PEX 0xF0000 entry in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(pciId) \
    PRV_CPSS_DEVICE_QUIRKS_PEX_ENTRY_MAC(pciId,0xF0000,GT_FALSE)

/* macro to set PUMA pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_PUMA_PEX_MAC \
    /* Puma2 devices , 24 ports */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX2106_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX2206_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98MX2306_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8261_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8301_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8303_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8501_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8306_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX240_CNS ), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX240_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98MX840_CNS ), \
    /* Puma2 devices : 2 port XG devices */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX2110_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX2210_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98MX2310_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8262_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8302_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8305_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8502_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX8307_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98EX260_CNS ), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98MX860_CNS ), \
    /* puma3 devices */                      \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x00000_ENTRY_MAC(CPSS_98MXEFEF_CNS ), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x00000_ENTRY_MAC(CPSS_98EX3318_CNS ), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x00000_ENTRY_MAC(CPSS_98EX3317_CNS ), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x00000_ENTRY_MAC(CPSS_98EX3316_CNS )

/* macro to set DXCH3 pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_DXCH3_PEX_MAC \
    /* cheetah-3 with 24 ports plus 4 XG ports */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5128_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5128_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX287_CNS ), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5129_CNS), \
    /* cheetah-3 with 10 XG ports: 6 XG + 4 Stack */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX8110_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX8110_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX8111_CNS),\
    /* cheetah-3 with 24 ports ports */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5124_CNS), \
    /* cheetah-3 with 24 ports plus 2 XG ports */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5126_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5127_CNS), \
    /* cheetah-3 with 4 1/2.5 Gigabit Ethernet ports + 2XG */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5151_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5151_1_CNS), \
    /* cheetah-3 with 8 1/2.5 Gigabit Ethernet ports + 2XG */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5152_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5152_1_CNS), \
    /* cheetah-3 with 16 1/2.5 Gigabit Ethernet ports + 2XG */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5154_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5154_1_CNS), \
    /* cheetah-3 with 16 1/2.5 Gigabit Ethernet ports + 2XG + 2XGS */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5155_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5155_1_CNS), \
    /* cheetah-3 with 24 1/2.5 Gigabit Ethernet ports + 2XG*/ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5156_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5156_1_CNS), \
    /* cheetah-3 with 24 1/2.5 Gigabit Ethernet ports + 2XG + 2XGS*/ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5157_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5157_1_CNS), \
    /* cheetah-3 with 8 XG ports: 8 XG or 6XG + 2 Stack */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX8108_CNS), \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX8109_CNS)

/* macro to set DXCH xCat pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_XCAT_PEX_MAC \
/*98DX1000 Series: AlleyCat-FE WEBSMART- 333MHz, 0K Cache, SPI, DDR2*/  \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1005_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1025_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1026_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1027_CNS),\
/*98DX102x Series: PonCat-FE */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1002_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1022_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1023_CNS),\
/*98DX1100/1200 Series: TomCat-FE (SMB L2+) - 600MHz, 16KBKB/128KB Cache  */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1122_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1123_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1222_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1223_CNS),\
/*98DX2100 Series: Bobcat-FE (L3+ / Metro) - 800MHz, , 16KB/256K Cache*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX2101_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX2122_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX2123_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX2151_CNS),\
/*98DX3000 Series: AlleyCat-GE (Entry L2) UNMANAGED- 333MHz, 0K Cache, 256KB Internal SRAM*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3011_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3021_CNS),\
/*98DX3000 Series: AlleyCat-GE WEBSMART- 333MHz, 0K Cache, SPI, DDR2*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3005_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3015_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3025_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3026_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3027_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXJ324_CNS),\
/*98DX303x Series: PonCat-GE */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3002_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3031_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3032_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3034_CNS),\
/*98DX3000 Series: AlleyCat2-GE WEBSMART*/ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3036_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3036A_CNS),\
/*98DX313x Series: PonCat2-GE */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3136_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXZ366_CNS),\
/*98DX3100 Series: TomCat-GE (SMB L2+) - 600MHz, 16KBKB/128KB Cache*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3101_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3121_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3122_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3123_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3124_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3125_CNS),\
/*98DX3200 Series: TomCat-METRO  - 600MHz, 16KBKB/128KB Cache*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3201_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3222_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3223_CNS),\
/*98DX4100 Series: Bobcat-GE (L3+ / Metro) - 800MHz,1,16KB/256KB Cache*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3110_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3110_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4101_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4121_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX5313_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4122_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4123_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4124_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4125_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX5248_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX5258_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX5158_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX5159_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX5168_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH525_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH426_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4A01_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX4A02_CNS),\
/*98DXH300 Series: TomCat-GE (DHX) - 333MHz, 0K Cache, SPI, DDR2*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH325_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH326_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH327_CNS),\
/*98DXH300 Series: AlleyCat2-GE Websmart (DHX) - 333MHz, 16KB/128KB Cache, SPI/NAND, DDR2*/ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH336_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH337_CNS),\
/*98DXH100 Series: TomCat-FE (DHX) - 333MHz, 0K Cache*/\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH125_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH126_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH127_CNS)

CPSS_TBD_BOOKMARK/* PEX address space might be used for MSYS registers access. */
/* macro to set DXCH xCat3 pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_XCAT3_PEX_MAC \
/* Series: AlleyCat3-FE*/  \
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX1233_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX1235_CNS, GT_TRUE),\
/* Series: AlleyCat3-GE*/  \
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3223_1_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3224_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3225_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3226_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3233_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3234_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3235_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3236_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DXH333_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DXT323_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DXT321_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DXC323_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DXN323_CNS, GT_TRUE),\
/* Series: PonCat3-FE*/  \
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX1333_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX1335_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX1336_CNS, GT_TRUE),\
/* Series: PonCat3-GE*/  \
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3333_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3334_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3335_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX3336_CNS, GT_TRUE)

/* macro to set Ocelot pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_OCELOT_PEX_MAC \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98FX950_CNS )

/* macro to set DXCH Lion pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_LION_PEX_MAC \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8248_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8248_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8112_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8224_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8224_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8203_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8113_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8222_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8223_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8226_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8234_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8265_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8266_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8267_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5198_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98DX5178_CNS)

/* macro to set DXCH xCat2 pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_XCAT2_PEX_MAC \
/* AlleyCat2-GE UNMANAGED */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3011B_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3011C_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3021B_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3021C_CNS),\
/* AlleyCat-FE WEBSMART- 333MHz, 0K Cache */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1033_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1034_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1035_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1053_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1035_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1055_CNS),\
/* AlleyCat-GE WEBSMART- 333MHz, 0K Cache */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3027B_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3033B_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3034B_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3035B_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3053_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3054_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3055_CNS),\
/* PonCat-FE */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1133_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1134_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1135_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1163_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1165_CNS),\
/* PonCat-GE */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3031B_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3133_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3134_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3135_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXZ365_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3163_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3165_CNS),\
/* VonCat-FE  */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1083_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX1085_CNS),\
/* VonCat-GE */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3083_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DX3085_CNS),\
/* AlleyCat-GE Websmart (DHX) */ \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0xF0000_ENTRY_MAC(CPSS_98DXH335_CNS)


/* macro to set DXCH Lion2 pex devices in the array of prvPciDeviceQuirks[]*/
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_LION2_PEX_MAC \
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8296_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8297_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8308_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8297_1_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8121_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8123_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8124_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CX8129_CNS),\
    PRV_CPSS_DEVICE_QUIRKS_PEX_0x70000_ENTRY_MAC(CPSS_98CXEEEF_CNS)

/* Bobcat2 PEX address space is used for MSYS registers access.
   Use offset 0 to access whole range of MSYS address space. */
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_BOBCAT2_PEX_MAC \
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4251_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4220_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4221_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4222_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4223_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8216_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8219_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8224_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4253_CNS, GT_TRUE)

/* Bobcat2-bobk PEX address space is used for MSYS registers access.
   Use offset 0 to access whole range of MSYS address space. */
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_BOBK_PEX_MAC \
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX42KK_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4203_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4204_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4211_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4210_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX4235_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8212_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8208_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8316_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8324_CNS, GT_TRUE),\
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX8332_CNS, GT_TRUE)

/* Bobcat3 PEX address space is used for MSYS registers access.
   Use offset 0 to access whole range of MSYS address space. */
#define PRV_CPSS_DEVICE_QUIRKS_DXCH_BOBCAT3_PEX_MAC \
    PRV_CPSS_DEVICE_QUIRKS_MSYS_ENTRY_MAC(CPSS_98DX42FF_CNS, GT_TRUE)




/* macro PRV_PCI_DEVICE_QUIRKS_ARRAY_MAC
 * Defines static PCI quirks array prvPciDeviceQuirks
*/
#define PRV_PCI_DEVICE_QUIRKS_ARRAY_MAC \
static struct prvPciDeviceQuirks { \
    GT_U32      pciId; \
    GT_BOOL     isPex; \
    GT_U32      configOffset; \
    GT_U32      configSize; \
    int         pciResourceReg; \
    GT_BOOL     hasDfx; \
} prvPciDeviceQuirks[] = { \
    PRV_CPSS_DEVICE_QUIRKS_PUMA_PEX_MAC,/* Puma devices */               \
    PRV_CPSS_DEVICE_QUIRKS_DXCH_LION_PEX_MAC,/*Lion devices*/            \
    PRV_CPSS_DEVICE_QUIRKS_DXCH_LION2_PEX_MAC,/*Lion2 devices*/          \
    PRV_CPSS_DEVICE_QUIRKS_DXCH3_PEX_MAC,/* DXCH3 devices */             \
    PRV_CPSS_DEVICE_QUIRKS_DXCH_XCAT_PEX_MAC,/* DXCH xCat devices */     \
    PRV_CPSS_DEVICE_QUIRKS_DXCH_XCAT2_PEX_MAC,/* DXCH xCat2 devices */   \
    PRV_CPSS_DEVICE_QUIRKS_OCELOT_PEX_MAC,/* Ocelot devices */           \
    PRV_CPSS_DEVICE_QUIRKS_DXCH_BOBCAT2_PEX_MAC,/*Bobcat2 devices*/      \
    PRV_CPSS_DEVICE_QUIRKS_DXCH_BOBK_PEX_MAC,   /*Bobk devices*/         \
    PRV_CPSS_DEVICE_QUIRKS_DXCH_XCAT3_PEX_MAC,/*xcat3 devices*/          \
    /* Default: PCI device */ \
    { 0xffffffff,         GT_FALSE,      0, PCI_CONFIG_SIZE, PCI_RESOURCE_PSS_REGS_PCI }  \
};




/* cheetah with 10 ports */
#define  PRV_CPSS_CH_10_DEVICES_MAC       \
CPSS_98DX107_CNS                           ,\
CPSS_98DX106_CNS

/* cheetah with 27 ports */
#define  PRV_CPSS_CH_27_DEVICES_MAC       \
CPSS_98DX270_CNS                           ,\
CPSS_98DX273_CNS                           ,\
CPSS_98DX803_CNS                           ,\
CPSS_98DX249_CNS                           ,\
CPSS_98DX269_CNS                           ,\
CPSS_98DX169_CNS

/* cheetah with 26 ports */
#define  PRV_CPSS_CH_26_DEVICES_MAC       \
CPSS_98DX260_CNS                           ,\
CPSS_98DX263_CNS                           ,\
CPSS_98DX262_CNS                           ,\
CPSS_98DX268_CNS

/* cheetah with 24 ports */
#define  PRV_CPSS_CH_24_DEVICES_MAC       \
CPSS_98DX250_CNS                           ,\
CPSS_98DX253_CNS                           ,\
CPSS_98DX243_CNS                           ,\
CPSS_98DX248_CNS

/* cheetah with 25 ports */
#define  PRV_CPSS_CH_25_DEVICES_MAC       \
CPSS_98DX133_CNS

/* cheetah with 16 ports */
#define  PRV_CPSS_CH_16_DEVICES_MAC       \
CPSS_98DX163_CNS                           ,\
CPSS_98DX166_CNS                           ,\
CPSS_98DX167_CNS

/* cheetah-2 with 28 ports */
#define  PRV_CPSS_CH2_28_DEVICES_MAC       \
CPSS_98DX285_CNS                           ,\
CPSS_98DX804_CNS

/* cheetah-2 with 27 ports */
#define  PRV_CPSS_CH2_27_DEVICES_MAC       \
CPSS_98DX275_CNS

/* cheetah-2 with 26 ports */
#define  PRV_CPSS_CH2_26_DEVICES_MAC       \
CPSS_98DX265_CNS                         ,\
CPSS_98DX145_CNS

/* cheetah-2 with 24 ports */
#define  PRV_CPSS_CH2_24_DEVICES_MAC       \
CPSS_98DX255_CNS

/* cheetah-2 with 12 ports */
#define  PRV_CPSS_CH2_12_DEVICES_MAC       \
CPSS_98DX125_CNS

/* cheetah-3 with 24 ports ports */
#define  PRV_CPSS_CH3_24_DEVICES_MAC       \
CPSS_98DX5124_CNS

/* cheetah-3 with 24 ports plus 2 XG ports */
#define  PRV_CPSS_CH3_26_DEVICES_MAC       \
CPSS_98DX5126_CNS                       ,\
CPSS_98DX5127_CNS

/* cheetah-3 with 24 ports plus 4 XG ports */
#define  PRV_CPSS_CH3_28_DEVICES_MAC       \
CPSS_98DX5128_CNS                       ,\
CPSS_98DX5128_1_CNS                     ,\
CPSS_98DX287_CNS                        ,\
CPSS_98DX5129_CNS

/* cheetah-3 with 4 1/2.5 Gigabit Ethernet ports + 2XG */
#define  PRV_CPSS_CH3_6_DEVICES_MAC       \
CPSS_98DX5151_CNS                       ,\
CPSS_98DX5151_1_CNS

/* cheetah-3 with 8 1/2.5 Gigabit Ethernet ports + 2XG */
#define  PRV_CPSS_CH3_10_DEVICES_MAC       \
CPSS_98DX5152_CNS                       ,\
CPSS_98DX5152_1_CNS

/* cheetah-3 with 16 1/2.5 Gigabit Ethernet ports + 2XG */
#define  PRV_CPSS_CH3_18_DEVICES_MAC       \
CPSS_98DX5154_CNS                       ,\
CPSS_98DX5154_1_CNS

/* cheetah-3 with 16 1/2.5 Gigabit Ethernet ports + 2XG + 2XGS */
#define  PRV_CPSS_CH3_20_DEVICES_MAC       \
CPSS_98DX5155_CNS                       ,\
CPSS_98DX5155_1_CNS

/* cheetah-3 with 24 1/2.5 Gigabit Ethernet ports + 2XG*/
#define  PRV_CPSS_CH3_24_2_DEVICES_MAC       \
CPSS_98DX5156_CNS                       ,\
CPSS_98DX5156_1_CNS

/* cheetah-3 with 24 1/2.5 Gigabit Ethernet ports + 2XG + 2XGS*/
#define  PRV_CPSS_CH3_24_4_DEVICES_MAC       \
CPSS_98DX5157_CNS                       ,\
CPSS_98DX5157_1_CNS

/* cheetah-3 with 8 XG ports: 8 XG or 6XG + 2 Stack */
#define  PRV_CPSS_CH3_8_XG_DEVICES_MAC       \
CPSS_98DX8108_CNS                       ,\
CPSS_98DX8109_CNS

/* cheetah-3 with 10 XG ports: 6 XG + 4 Stack */
#define  PRV_CPSS_CH3_XG_DEVICES_MAC       \
CPSS_98DX8110_CNS                       ,\
CPSS_98DX8110_1_CNS                     ,\
CPSS_98DX8111_CNS

/* Salsa with 24 ports */
#define  PRV_CPSS_SALSA_24_DEVICES_MAC       \
CPSS_98DX240_CNS                       ,\
CPSS_98DX241_CNS

/* Salsa with 16 ports */
#define  PRV_CPSS_SALSA_16_DEVICES_MAC       \
CPSS_98DX160_CNS                       ,\
CPSS_98DX161_CNS

/* Salsa with 12 ports */
#define  PRV_CPSS_SALSA_12_DEVICES_MAC       \
CPSS_98DX120_CNS                       ,\
CPSS_98DX121_CNS

#define  PRV_CPSS_DX_DEVICES_MAC       \
/* Salsa devices  */                       \
PRV_CPSS_SALSA_24_DEVICES_MAC             ,\
PRV_CPSS_SALSA_16_DEVICES_MAC             ,\
PRV_CPSS_SALSA_12_DEVICES_MAC             ,\
                                           \
/* Cheetah devices */                      \
PRV_CPSS_CH_10_DEVICES_MAC                 ,\
PRV_CPSS_CH_27_DEVICES_MAC                 ,\
PRV_CPSS_CH_26_DEVICES_MAC                 ,\
PRV_CPSS_CH_24_DEVICES_MAC                 ,\
PRV_CPSS_CH_16_DEVICES_MAC                 ,\
                                           \
/* Cheetah2 devices */                     \
PRV_CPSS_CH2_28_DEVICES_MAC                ,\
PRV_CPSS_CH2_27_DEVICES_MAC                ,\
PRV_CPSS_CH2_26_DEVICES_MAC                ,\
PRV_CPSS_CH2_24_DEVICES_MAC                ,\
PRV_CPSS_CH2_12_DEVICES_MAC                ,\
                                            \
/* Cheetah3 devices */                      \
PRV_CPSS_CH3_28_DEVICES_MAC                ,\
PRV_CPSS_CH3_26_DEVICES_MAC                ,\
PRV_CPSS_CH3_24_DEVICES_MAC                ,\
PRV_CPSS_CH3_6_DEVICES_MAC                 ,\
PRV_CPSS_CH3_10_DEVICES_MAC                ,\
PRV_CPSS_CH3_18_DEVICES_MAC                ,\
PRV_CPSS_CH3_20_DEVICES_MAC                ,\
PRV_CPSS_CH3_24_2_DEVICES_MAC              ,\
PRV_CPSS_CH3_24_4_DEVICES_MAC              ,\
PRV_CPSS_CH3_8_XG_DEVICES_MAC              ,\
PRV_CPSS_CH3_XG_DEVICES_MAC

/* Twist-C with 10 ports */
#define  PRV_CPSS_TWISTC_10_DEVICES_MAC     \
CPSS_98MX620B_CNS

/* Twist-C with 52 ports */
#define  PRV_CPSS_TWISTC_52_DEVICES_MAC     \
CPSS_98MX610B_CNS                          ,\
CPSS_98MX610BS_CNS                         ,\
CPSS_98EX110BS_CNS                         ,\
CPSS_98EX111BS_CNS                         ,\
CPSS_98EX112BS_CNS                         ,\
CPSS_98EX110B_CNS                          ,\
CPSS_98EX111B_CNS                          ,\
CPSS_98EX112B_CNS

/* Twist-C with 12 ports */
#define  PRV_CPSS_TWISTC_12_DEVICES_MAC     \
CPSS_98EX120B_CNS                          ,\
CPSS_98EX120B__CNS                         ,\
CPSS_98EX121B_CNS                          ,\
CPSS_98EX122B_CNS                          ,\
CPSS_98EX128B_CNS                          ,\
CPSS_98EX129B_CNS

/* Twist-D with 52 ports */
#define  PRV_CPSS_TWISTD_52_DEVICES_MAC     \
CPSS_98EX100D_CNS                          ,\
CPSS_98EX110D_CNS                          ,\
CPSS_98EX115D_CNS                          ,\
CPSS_98EX110DS_CNS                         ,\
CPSS_98EX115DS_CNS

/* Twist-D with 12 ports */
#define  PRV_CPSS_TWISTD_12_DEVICES_MAC     \
CPSS_98EX120D_CNS                          ,\
CPSS_98EX125D_CNS

/* Twist-D with 1XG ports */
#define  PRV_CPSS_TWISTD_1_DEVICES_MAC      \
CPSS_98EX130D_CNS                          ,\
CPSS_98EX135D_CNS


/* Samba with 52 ports */
#define  PRV_CPSS_SAMBA_52_DEVICES_MAC      \
CPSS_98MX615_CNS                           ,\
CPSS_98MX618_CNS

/* Samba with 12 ports */
#define  PRV_CPSS_SAMBA_12_DEVICES_MAC      \
CPSS_98MX625_CNS                           ,\
CPSS_98MX625V0_CNS                         ,\
CPSS_98MX628_CNS

/* Samba with 1XG ports */
#define  PRV_CPSS_SAMBA_1_DEVICES_MAC       \
CPSS_98MX635_CNS                           ,\
CPSS_98MX638_CNS


/* Tiger with 52 ports */
#define  PRV_CPSS_TIGER_52_DEVICES_MAC      \
CPSS_98EX116_CNS                           ,\
CPSS_98EX106_CNS                           ,\
CPSS_98EX108_CNS                           ,\
CPSS_98EX116DI_CNS

/* Tiger with 12 ports */
#define  PRV_CPSS_TIGER_12_DEVICES_MAC      \
CPSS_98EX126_CNS                           ,\
CPSS_98EX126V0_CNS                         ,\
CPSS_98EX126DI_CNS

/* Tiger with 1XG ports */
#define  PRV_CPSS_TIGER_1_DEVICES_MAC       \
CPSS_98EX136_CNS                           ,\
CPSS_98EX136DI_CNS

/* list of all EX-MX devices */
#define  PRV_CPSS_EXMX_DEVICES_MAC          \
    /* Twist devices    */                  \
PRV_CPSS_TWISTC_10_DEVICES_MAC             ,\
PRV_CPSS_TWISTC_12_DEVICES_MAC             ,\
PRV_CPSS_TWISTC_52_DEVICES_MAC             ,\
    /* Twist-D devices    */                \
PRV_CPSS_TWISTD_52_DEVICES_MAC             ,\
PRV_CPSS_TWISTD_12_DEVICES_MAC             ,\
PRV_CPSS_TWISTD_1_DEVICES_MAC              ,\
    /* samba devices */                     \
PRV_CPSS_SAMBA_52_DEVICES_MAC              ,\
PRV_CPSS_SAMBA_12_DEVICES_MAC              ,\
PRV_CPSS_SAMBA_1_DEVICES_MAC               ,\
    /* Tiger devices */                     \
PRV_CPSS_TIGER_52_DEVICES_MAC              ,\
PRV_CPSS_TIGER_12_DEVICES_MAC              ,\
PRV_CPSS_TIGER_1_DEVICES_MAC


/* Puma2 devices , 24 ports */
#define PRV_CPSS_PUMA_24_DEVICES_MAC      \
CPSS_98EX2106_CNS                         ,\
CPSS_98EX2206_CNS                         ,\
CPSS_98MX2306_CNS                         ,\
CPSS_98EX8261_CNS                         ,\
CPSS_98EX8301_CNS                         ,\
CPSS_98EX8303_CNS                         ,\
CPSS_98EX8501_CNS                         ,\
CPSS_98EX8306_CNS                         ,\
CPSS_98EX240_CNS                          ,\
CPSS_98EX240_1_CNS                        ,\
CPSS_98MX840_CNS
/* Puma2 devices : 2 port XG devices */
#define PRV_CPSS_PUMA_26_DEVICES_MAC      \
CPSS_98EX2110_CNS                         ,\
CPSS_98EX2210_CNS                         ,\
CPSS_98MX2310_CNS                         ,\
CPSS_98EX8262_CNS                         ,\
CPSS_98EX8302_CNS                         ,\
CPSS_98EX8305_CNS                         ,\
CPSS_98EX8502_CNS                         ,\
CPSS_98EX8307_CNS                         ,\
CPSS_98EX260_CNS                          ,\
CPSS_98MX860_CNS
/* list of all PUMA devices */
#define  PRV_CPSS_PUMA_DEVICES_MAC        \
/* PUMA with 24 ports */                  \
PRV_CPSS_PUMA_24_DEVICES_MAC             ,\
/* PUMA with 2XG ports */                 \
PRV_CPSS_PUMA_26_DEVICES_MAC

#define PRV_CPSS_PUMA_24G_DEVICES_CASES_MAC \
    /*caller start with 'case' */           \
         CPSS_98EX2106_CNS:                 \
    case CPSS_98EX2206_CNS:                 \
    case CPSS_98MX2306_CNS:                 \
    case CPSS_98EX8261_CNS:                 \
    case CPSS_98EX8301_CNS:                 \
    case CPSS_98EX8303_CNS:                 \
    case CPSS_98EX8501_CNS:                 \
    case CPSS_98EX240_CNS:                  \
    case CPSS_98EX240_1_CNS:                \
    case CPSS_98MX840_CNS/*caller end with ':' */

#define PRV_CPSS_PUMA_2XG_DEVICES_CASES_MAC \
    /*caller start with 'case' */           \
         CPSS_98EX2110_CNS:                 \
    case CPSS_98EX2210_CNS:                 \
    case CPSS_98MX2310_CNS:                 \
    case CPSS_98EX8262_CNS:                 \
    case CPSS_98EX8302_CNS:                 \
    case CPSS_98EX8305_CNS:                 \
    case CPSS_98EX8502_CNS:                 \
    case CPSS_98EX260_CNS:                  \
    case CPSS_98MX860_CNS/*caller end with ':' */

/***********************/
/* start Puma3 devices */
/***********************/
#define    CPSS_98MXEFEF_CNS    0xEFEF11AB    /* dummy for simulation - Puma3 device  */
#define    CPSS_98EX3316_CNS    0xE00011AB
#define    CPSS_98EX3317_CNS    0xE00211AB
#define    CPSS_98EX3318_CNS    0xE00411AB

#define     PRV_CPSS_PUMA3_DEVICES_MAC      \
           CPSS_98MXEFEF_CNS,               \
           CPSS_98EX3317_CNS,               \
           CPSS_98EX3318_CNS,               \
           CPSS_98EX3316_CNS

/************************/
/* End Of Puma3 devices */
/************************/

/* xCat devices */

/* xCat with 8FE,2stack ports */
#define PRV_CPSS_XCAT_8FE_2STACK_PORTS_DEVICES \
CPSS_98DX1005_CNS

/* xCat with 8FE,4stack ports */
#define PRV_CPSS_XCAT_8FE_4STACK_PORTS_DEVICES \
CPSS_98DX1002_CNS,                       \
CPSS_98DX2101_CNS

/* xCat with 24FE,4stack ports */
#define PRV_CPSS_XCAT_24FE_4STACK_PORTS_DEVICES  \
CPSS_98DX1122_CNS,                       \
CPSS_98DX1123_CNS,                       \
CPSS_98DX1222_CNS,                       \
CPSS_98DX1223_CNS,                       \
CPSS_98DX2122_CNS,                       \
CPSS_98DX2123_CNS,                       \
CPSS_98DX2151_CNS,                       \
CPSS_98DX1022_CNS,                       \
CPSS_98DX1023_CNS,                       \
CPSS_98DX1025_CNS,                       \
CPSS_98DX1026_CNS,                       \
CPSS_98DX1027_CNS,                       \
CPSS_98DXH125_CNS,                       \
CPSS_98DXH126_CNS,                       \
CPSS_98DXH127_CNS

/* xCat with 8GE,2stack ports */
#define PRV_CPSS_XCAT_8GE_2STACK_PORTS_DEVICES \
    CPSS_98DX3101_CNS,                   \
    CPSS_98DX3005_CNS

/* xCat with 8GE,4stack ports */
#define PRV_CPSS_XCAT_8GE_4STACK_PORTS_DEVICES \
    CPSS_98DX3002_CNS,                   \
    CPSS_98DX3201_CNS,                   \
    CPSS_98DX4101_CNS

/* xCat with 12GE,2stack ports (26,27)*/
#define PRV_CPSS_XCAT_12GE_2STACK_PORTS_26_27_DEVICES \
    CPSS_98DX5168_CNS

/* xCat with 16GE,2stack ports */
#define PRV_CPSS_XCAT_16GE_2STACK_PORTS_DEVICES \
    CPSS_98DX3011_CNS

/* xCat with 16GE,4stack ports */
#define PRV_CPSS_XCAT_16GE_4STACK_PORTS_DEVICES   \
    CPSS_98DX3015_CNS

/* xCat with 24GE without stack ports */
#define PRV_CPSS_XCAT_24GE_NO_STACK_PORTS_DEVICES   \
    PRV_CPSS_DEVICE_TYPE_PLACE_HOLDER_CNS/* remove it when real device exists */

/* xCat with 24GE,2stack ports */
#define PRV_CPSS_XCAT_24GE_2STACK_PORTS_DEVICES   \
    CPSS_98DX3021_CNS

/* xCat with 24GE,4stack ports */
#define PRV_CPSS_XCAT_24GE_4STACK_PORTS_DEVICES   \
    CPSS_98DX3110_CNS,                    \
    CPSS_98DX3110_1_CNS,                  \
    CPSS_98DX3121_CNS,                    \
    CPSS_98DX3122_CNS,                    \
    CPSS_98DX3123_CNS,                    \
    CPSS_98DX3124_CNS,                    \
    CPSS_98DX3125_CNS,                    \
    CPSS_98DX3222_CNS,                    \
    CPSS_98DX3223_CNS,                    \
    CPSS_98DX4121_CNS,                    \
    CPSS_98DX4122_CNS,                    \
    CPSS_98DX4123_CNS,                    \
    CPSS_98DX4124_CNS,                    \
    CPSS_98DX4125_CNS,                    \
    CPSS_98DX5248_CNS,                    \
    CPSS_98DX5258_CNS,                    \
    CPSS_98DX5158_CNS,                    \
    CPSS_98DX5159_CNS,                    \
    CPSS_98DXH525_CNS,                    \
    CPSS_98DXH426_CNS,                    \
    CPSS_98DX4A01_CNS,                    \
    CPSS_98DX4A02_CNS,                    \
    CPSS_98DX3025_CNS,                    \
    CPSS_98DX3026_CNS,                    \
    CPSS_98DX3027_CNS,                    \
    CPSS_98DX3031_CNS,                    \
    CPSS_98DX3032_CNS,                    \
    CPSS_98DX3034_CNS,                    \
    CPSS_98DX3036_CNS,                    \
    CPSS_98DX3036A_CNS,                   \
    CPSS_98DX3136_CNS,                    \
    CPSS_98DXZ366_CNS,                    \
    CPSS_98DXH325_CNS,                    \
    CPSS_98DXH326_CNS,                    \
    CPSS_98DXH327_CNS,                    \
    CPSS_98DXH336_CNS,                    \
    CPSS_98DXH337_CNS,                    \
    CPSS_98DX5313_CNS,                    \
    CPSS_98DXJ324_CNS

/* xCat3 devices */

/* xCat3 with 8GE,2stack ports */
#define PRV_CPSS_XCAT3_8GE_2STACK_PORTS_DEVICES \
    CPSS_98DX1233_CNS,                  \
    CPSS_98DX1333_CNS

/* xCat3 with 8GE,4stack ports */
#define PRV_CPSS_XCAT3_8GE_4STACK_PORTS_DEVICES \
    CPSS_98DX3223_1_CNS,                \
    CPSS_98DX3233_CNS,                  \
    CPSS_98DX3333_CNS,                  \
    CPSS_98DXH333_CNS

/* xCat3 with 16GE,4stack ports */
#define PRV_CPSS_XCAT3_16GE_4STACK_PORTS_DEVICES   \
    CPSS_98DX3334_CNS,                  \
    CPSS_98DX3224_CNS,                  \
    CPSS_98DX3234_CNS

/* xCat3 with 24GE,2stack ports */
#define PRV_CPSS_XCAT3_24GE_2STACK_PORTS_DEVICES   \
        CPSS_98DXT321_CNS

/* xCat3 with 24GE,4stack ports */
#define PRV_CPSS_XCAT3_24GE_4STACK_PORTS_DEVICES   \
    CPSS_98DX1235_CNS,                    \
    CPSS_98DX1335_CNS,                    \
    CPSS_98DX1336_CNS,                    \
    CPSS_98DX3235_CNS,                    \
    CPSS_98DX3236_CNS,                    \
    CPSS_98DX3225_CNS,                    \
    CPSS_98DX3226_CNS,                    \
    CPSS_98DX3335_CNS,                    \
    CPSS_98DX3336_CNS,                    \
    CPSS_98DXT323_CNS,                    \
    CPSS_98DXC323_CNS,                    \
    CPSS_98DXN323_CNS

#define PRV_CPSS_CHEETAH3_XG_PORTS_DEVICES_CASES_MAC             \
    /*caller start with 'case' */                                \
         CPSS_98DX8110_CNS   :                                   \
    case CPSS_98DX8108_CNS   :                                   \
    case CPSS_98DX8109_CNS   :                                   \
    case CPSS_98DX8110_1_CNS :                                   \
    case CPSS_98DX8111_CNS    /*caller end with ':' */

#define PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_XG_HX_QX_CASES_MAC \
    /*caller start with 'case' */                                \
         CPSS_98DX2151_CNS:                                      \
    case CPSS_98DX3122_CNS:                                      \
    case CPSS_98DX3123_CNS:                                      \
    case CPSS_98DX3124_CNS:                                      \
    case CPSS_98DX3125_CNS:                                      \
    case CPSS_98DX3222_CNS:                                      \
    case CPSS_98DX3223_CNS:                                      \
    case CPSS_98DX4122_CNS:                                      \
    case CPSS_98DX4123_CNS:                                      \
    case CPSS_98DX4124_CNS:                                      \
    case CPSS_98DX4125_CNS:                                      \
    case CPSS_98DX5248_CNS:                                      \
    case CPSS_98DX5258_CNS:                                      \
    case CPSS_98DX5158_CNS:                                      \
    case CPSS_98DX5159_CNS:                                      \
    case CPSS_98DX5168_CNS:                                      \
    case CPSS_98DXH426_CNS:                                      \
    case CPSS_98DX4A01_CNS:                                      \
    case CPSS_98DX4A02_CNS:                                      \
    case CPSS_98DX3026_CNS:                                      \
    case CPSS_98DX3027_CNS:                                      \
    case CPSS_98DX3032_CNS:                                      \
    case CPSS_98DX3034_CNS:                                      \
    case CPSS_98DX3036_CNS:                                      \
    case CPSS_98DX3036A_CNS:                                     \
    case CPSS_98DX3136_CNS:                                      \
    case CPSS_98DXZ366_CNS:                                      \
    case CPSS_98DXH326_CNS:                                      \
    case CPSS_98DXH327_CNS:                                      \
    case CPSS_98DXH336_CNS:                                      \
    case CPSS_98DXH337_CNS:                                      \
    case CPSS_98DX3110_CNS:                                      \
    case CPSS_98DX3110_1_CNS:                                    \
    case CPSS_98DX5313_CNS   /*caller end with ':' */


#define PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_HX_QX_CASES_NO_DUPLICATION_MAC  \
    /*caller start with 'case' */                                \
         CPSS_98DX3121_CNS    /*caller end with ':' */

#define PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_HX_QX_CASES_MAC    \
    /*caller start with 'case' */                                \
/* 3124 and 3125 agreed by marketing internally to move to XG to simplify B2B configuration */  \
         CPSS_98DX4124_CNS:/*this one exists in XG_HX_QX_CASES */\
    case CPSS_98DX4125_CNS:/*this one exists in XG_HX_QX_CASES */\
    case PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_HX_QX_CASES_NO_DUPLICATION_MAC/*caller end with ':' */

/* device list with QX/DQX stack ports only */
#define PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_QX_CASES_NO_DUPLICATION_MAC  \
         CPSS_98DX1026_CNS:                                      \
    case CPSS_98DX1027_CNS:                                      \
    case CPSS_98DX1122_CNS:                                      \
    case CPSS_98DX1123_CNS:                                      \
    case CPSS_98DX1222_CNS:                                      \
    case CPSS_98DX1223_CNS:                                      \
    case CPSS_98DX2101_CNS:                                      \
    case CPSS_98DX2122_CNS:                                      \
    case CPSS_98DX2123_CNS:                                      \
    case CPSS_98DX3201_CNS:                                      \
    case CPSS_98DX4101_CNS:                                      \
    case CPSS_98DX4111_CNS:                                      \
    case CPSS_98DXH525_CNS:                                      \
    case CPSS_98DXH126_CNS:                                      \
    case CPSS_98DXH127_CNS    /*caller end with ':' */

/* device list with QX/DQX stack ports (supports other modes) */
#define PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_QX_CASES_MAC       \
    /*caller start with 'case' */                                \
         CPSS_98DX3121_CNS:/*this one exists in HX_QX_CASES */   \
    case CPSS_98DX3026_CNS:/*this one exists in XG_HX_QX_CASES */\
    case CPSS_98DX3027_CNS:/*this one exists in XG_HX_QX_CASES */\
    case CPSS_98DX3034_CNS:/*this one exists in XG_HX_QX_CASES */\
    case CPSS_98DX2151_CNS:/*this one exists in XG_HX_QX_CASES */\
    case CPSS_98DXH336_CNS:/*this one exists in XG_HX_QX_CASES */\
    case CPSS_98DXH337_CNS:/*this one exists in XG_HX_QX_CASES */\
    case PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_QX_CASES_NO_DUPLICATION_MAC /*caller end with ':' */


#define PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_GIG_CASES_MAC      \
    /*caller start with 'case' */                                \
         CPSS_98DX1005_CNS:                                      \
    case CPSS_98DX1002_CNS:                                      \
    case CPSS_98DX1022_CNS:                                      \
    case CPSS_98DX1023_CNS:                                      \
    case CPSS_98DX1025_CNS:                                      \
    case CPSS_98DX3002_CNS:                                      \
    case CPSS_98DX3011_CNS:                                      \
    case CPSS_98DX3021_CNS:                                      \
    case CPSS_98DX3101_CNS:                                      \
    case CPSS_98DX3005_CNS:                                      \
    case CPSS_98DX3015_CNS:                                      \
    case CPSS_98DX3025_CNS:                                      \
    case CPSS_98DX3031_CNS:                                      \
    case CPSS_98DXH325_CNS:                                      \
    case CPSS_98DXH125_CNS:                                      \
    case CPSS_98DXJ324_CNS    /*caller end with ':' */

#define PRV_CPSS_DXCH_XCAT_FE_DEVICES_CASES_MAC \
    /*caller start with 'case' */               \
         CPSS_98DX2101_CNS:                     \
    case CPSS_98DX1122_CNS:                     \
    case CPSS_98DX1123_CNS:                     \
    case CPSS_98DX1222_CNS:                     \
    case CPSS_98DX1223_CNS:                     \
    case CPSS_98DX2122_CNS:                     \
    case CPSS_98DX2123_CNS:                     \
    case CPSS_98DX2151_CNS:                     \
    case CPSS_98DX1005_CNS:                     \
    case CPSS_98DX1002_CNS:                     \
    case CPSS_98DX1022_CNS:                     \
    case CPSS_98DX1023_CNS:                     \
    case CPSS_98DX1025_CNS:                     \
    case CPSS_98DX1026_CNS:                     \
    case CPSS_98DX1027_CNS:                     \
    case CPSS_98DXH125_CNS:                     \
    case CPSS_98DXH126_CNS:                     \
    case CPSS_98DXH127_CNS   /*caller end with ':' */

#define PRV_CPSS_DXCH_XCAT_DEVICES_CASES_MAC                        \
    /*caller start with 'case' */                                   \
         PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_XG_HX_QX_CASES_MAC : \
    case PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_HX_QX_CASES_NO_DUPLICATION_MAC:\
    case PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_QX_CASES_NO_DUPLICATION_MAC:   \
    case PRV_CPSS_DXCH_XCAT_DEVICES_WITH_STACK_GIG_CASES_MAC /*caller end with ':' */

#define PRV_CPSS_DXCH_XCAT3_GE_DEVICES_CASES_MAC    \
    /*caller start with 'case' */                   \
         CPSS_98DX3223_1_CNS:                       \
    case CPSS_98DX3224_CNS:                         \
    case CPSS_98DX3225_CNS:                         \
    case CPSS_98DX3226_CNS:                         \
    case CPSS_98DX3233_CNS:                         \
    case CPSS_98DX3234_CNS:                         \
    case CPSS_98DX3235_CNS:                         \
    case CPSS_98DX3236_CNS:                         \
    case CPSS_98DX3333_CNS:                         \
    case CPSS_98DX3334_CNS:                         \
    case CPSS_98DX3335_CNS:                         \
    case CPSS_98DXH333_CNS:                         \
    case CPSS_98DX3336_CNS:                         \
    case CPSS_98DXT323_CNS:                         \
    case CPSS_98DXT321_CNS:                         \
    case CPSS_98DXC323_CNS:                         \
    case CPSS_98DXN323_CNS   /*caller end with ':' */

#define PRV_CPSS_DXCH_XCAT3_FE_DEVICES_CASES_MAC    \
    /*caller start with 'case' */                   \
         CPSS_98DX1233_CNS:                         \
    case CPSS_98DX1235_CNS:                         \
    case CPSS_98DX1333_CNS:                         \
    case CPSS_98DX1335_CNS:                         \
    case CPSS_98DX1336_CNS   /*caller end with ':' */


/* xCat3 all devices case */
#define PRV_CPSS_DXCH_XCAT3_DEVICES_CASES_MAC                       \
    /*caller start with 'case' */                                   \
    PRV_CPSS_DXCH_XCAT3_FE_DEVICES_CASES_MAC:                   \
    case PRV_CPSS_DXCH_XCAT3_GE_DEVICES_CASES_MAC /*caller end with ':' */

/* lion devices with port groups 0,1,2,3 */
#define    CPSS_LION_PORT_GROUPS_0123_CNS   \
    CPSS_98CX8248_CNS                      ,\
    CPSS_98CX8248_1_CNS                    ,\
    CPSS_98CX8112_CNS                      ,\
    CPSS_98CX8224_CNS                      ,\
    CPSS_98CX8224_1_CNS                    ,\
    CPSS_98CX8113_CNS                      ,\
    CPSS_98CX8222_CNS                      ,\
    CPSS_98CX8223_CNS                      ,\
    CPSS_98CX8226_CNS                      ,\
    CPSS_98CX8234_CNS                      ,\
    CPSS_98CX8265_CNS                      ,\
    CPSS_98CX8266_CNS                      ,\
    CPSS_98CX8267_CNS                      ,\
    CPSS_98DX5198_CNS                      ,\
    CPSS_98DX5178_CNS                      ,\
    CPSS_98FX9216_CNS

/* lion3; Bobcat2 devices with port groups 0,1,2,3,4,5,6,7 */
#define    CPSS_LION3_PORT_GROUPS_01234567_CNS   \
    CPSS_98CXEEEF_CNS

/* lion3; Bobcat2 All devices */
#define    CPSS_LION3_ALL_DEVICES_CNS   \
    CPSS_LION3_PORT_GROUPS_01234567_CNS


/* lion2 devices with port groups 0,1,2,3,4,5,6,7 */
#define    CPSS_LION2_PORT_GROUPS_01234567_CNS   \
    CPSS_98CX8296_CNS                      ,\
    CPSS_98CX8297_CNS                      ,\
    CPSS_98CX8308_CNS                      ,\
    CPSS_98FX9287_CNS                      ,\
    CPSS_98FX9288_CNS                      ,\
    CPSS_98CX8297_1_CNS

/* lion2 Hooper devices with port groups 0,1,2,3 */
#define    CPSS_LION2_HOOPER_PORT_GROUPS_0123_CNS   \
    CPSS_98CX8121_CNS                      ,\
    CPSS_98CX8123_CNS                      ,\
    CPSS_98CX8124_CNS                      ,\
    CPSS_98CX8129_CNS

/* lion2,lion3,Bobcat2 devices with 3 mini-gop's per port group */
#define    CPSS_LION_2_3_THREE_MINI_GOPS_DEVICES_CASES_MAC  \
         CPSS_98CX8296_CNS:                                 \
    case CPSS_98CX8308_CNS

/* lion2,lion3,Bobcat2 devices with 4 mini-gop's per port group */
#define    CPSS_LION_2_3_FOUR_MINI_GOPS_DEVICES_CASES_MAC   \
         CPSS_LION3_ALL_DEVICES_CASES_MAC:                  \
    case CPSS_BOBCAT2_ALL_DEVICES_CASES_MAC:                \
    case CPSS_98CX8297_CNS:                                 \
    case CPSS_98FX9287_CNS:                                 \
    case CPSS_98FX9288_CNS:                                 \
    case CPSS_98CX8121_CNS:                                 \
    case CPSS_98CX8123_CNS:                                 \
    case CPSS_98CX8124_CNS:                                 \
    case CPSS_98CX8129_CNS:                                 \
    case CPSS_98CX8297_1_CNS /*caller end with ':' */

/* lion devices with port groups 0,1,2,3 -- for switch-case*/
#define CPSS_LION_PORT_GROUPS_0123_DEVICES_CASES_MAC  \
    /*caller start with 'case' */                     \
         CPSS_98CX8248_CNS:                           \
    case CPSS_98CX8248_1_CNS:                         \
    case CPSS_98CX8112_CNS:                           \
    case CPSS_98CX8224_CNS:                           \
    case CPSS_98CX8224_1_CNS:                         \
    case CPSS_98CX8113_CNS:                           \
    case CPSS_98CX8222_CNS:                           \
    case CPSS_98CX8223_CNS:                           \
    case CPSS_98CX8226_CNS:                           \
    case CPSS_98CX8234_CNS:                           \
    case CPSS_98CX8265_CNS:                           \
    case CPSS_98CX8266_CNS:                           \
    case CPSS_98CX8267_CNS:                           \
    case CPSS_98DX5198_CNS:                           \
    case CPSS_98DX5178_CNS:                           \
    case CPSS_98FX9216_CNS /*caller end with ':' */

/* lion3; Bobcat2 devices with port groups 0,1,2,3,4,5,6,7 -- for switch-case*/
#define CPSS_LION3_PORT_GROUPS_01234567_DEVICES_CASES_MAC  \
    /*caller start with 'case' */                     \
         CPSS_98CXEEEF_CNS    /*caller end with ':' */

/* lion3; Bobcat2 All devices -- for switch-case*/
#define CPSS_LION3_ALL_DEVICES_CASES_MAC  \
    /*caller start with 'case' */                     \
         CPSS_LION3_PORT_GROUPS_01234567_DEVICES_CASES_MAC    /*caller end with ':' */



/* lion2 devices with port groups 0,1,2,3,4,5,6,7 -- for switch-case*/
#define CPSS_LION2_PORT_GROUPS_01234567_DEVICES_CASES_MAC  \
    /*caller start with 'case' */                          \
         CPSS_98CX8296_CNS:                                \
    case CPSS_98CX8297_CNS:                                \
    case CPSS_98CX8308_CNS:                                \
    case CPSS_98FX9287_CNS:                                \
    case CPSS_98FX9288_CNS:                                \
    case CPSS_98CX8297_1_CNS/*caller end with ':' */

/* lion2 Hooper devices with port groups 0,1,2,3 -- for switch-case*/
#define CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC  \
    /*caller start with 'case' */                          \
         CPSS_98CX8121_CNS:                                \
    case CPSS_98CX8123_CNS:                                \
    case CPSS_98CX8124_CNS:                                \
    case CPSS_98CX8129_CNS /*caller end with ':' */

/* lion devices with port groups 0,1 */
#define    CPSS_LION_PORT_GROUPS_01___CNS \
    CPSS_98CX8203_CNS

/* lion devices with port groups 0,1 -- for switch-case*/
#define CPSS_LION_PORT_GROUPS_01___DEVICES_CASES_MAC  \
    /*caller start with 'case' */                     \
         CPSS_98CX8203_CNS/*caller end with ':' */


/* lion ALL devices  -- for switch-case*/
#define CPSS_LION_ALL_DEVICES_CASES_MAC                   \
    /*caller start with 'case' */                         \
         CPSS_LION_PORT_GROUPS_0123_DEVICES_CASES_MAC:    \
    case CPSS_LION_PORT_GROUPS_01___DEVICES_CASES_MAC /*caller end with ':' */

/* xCat2 devices */

/* xCat2 with 8FE,2stack ports */
#define PRV_CPSS_XCAT2_8FE_2STACK_PORTS_DEVICES \
    CPSS_98DX1033_CNS,                      \
    CPSS_98DX1053_CNS,                      \
    CPSS_98DX1133_CNS,                      \
    CPSS_98DX1163_CNS

/* xCat2 with 8FE,4stack ports */
#define PRV_CPSS_XCAT2_8FE_4STACK_PORTS_DEVICES \
    CPSS_98DX1083_CNS

/* xCat2 with 16FE,4stack ports */
#define PRV_CPSS_XCAT2_16FE_4STACK_PORTS_DEVICES \
    CPSS_98DX1034_CNS,                      \
    CPSS_98DX1134_CNS


/* xCat2 with 24FE, 4stack ports */
#define PRV_CPSS_XCAT2_24FE_4STACK_PORTS_DEVICES  \
    CPSS_98DX1035_CNS,                         \
    CPSS_98DX1035_1_CNS,                       \
    CPSS_98DX1135_CNS,                         \
    CPSS_98DX1165_CNS,                         \
    CPSS_98DX1055_CNS,                         \
    CPSS_98DX1085_CNS

/* xCat2 with 8GE,2stack ports */
#define PRV_CPSS_XCAT2_8GE_2STACK_PORTS_DEVICES \
    CPSS_98DX3033B_CNS,                         \
    CPSS_98DX3053_CNS

/* xCat2 with 8GE,4stack ports */
#define PRV_CPSS_XCAT2_8GE_4STACK_PORTS_DEVICES \
    CPSS_98DX3133_CNS,                    \
    CPSS_98DX3163_CNS,                    \
    CPSS_98DX3083_CNS

/* xCat2 with 16GE,2stack ports */
#define PRV_CPSS_XCAT2_16GE_2STACK_PORTS_DEVICES \
    CPSS_98DX3011B_CNS

/* xCat2 with 16GE,4stack ports */
#define PRV_CPSS_XCAT2_16GE_4STACK_PORTS_DEVICES   \
    CPSS_98DX3011C_CNS,                   \
    CPSS_98DX3034B_CNS,                   \
    CPSS_98DX3054_CNS,                    \
    CPSS_98DX3134_CNS

/* xCat2 with 24GE,2stack ports */
#define PRV_CPSS_XCAT2_24GE_2STACK_PORTS_DEVICES \
    CPSS_98DX3021B_CNS

/* xCat2 with 24GE,4stack ports */
#define PRV_CPSS_XCAT2_24GE_4STACK_PORTS_DEVICES   \
    CPSS_98DX3021C_CNS,                    \
    CPSS_98DX3027B_CNS,                    \
    CPSS_98DX3035B_CNS,                    \
    CPSS_98DX3055_CNS,                     \
    CPSS_98DX3031B_CNS,                    \
    CPSS_98DX3135_CNS,                     \
    CPSS_98DXZ365_CNS,                     \
    CPSS_98DX3165_CNS,                     \
    CPSS_98DX3085_CNS,                     \
    CPSS_98DXH335_CNS

/* xCat2 with FE ports case */
#define PRV_CPSS_DXCH_XCAT2_FE_DEVICES_CASES_MAC \
    /*caller start with 'case' */                 \
         CPSS_98DX1033_CNS:                       \
    case CPSS_98DX1034_CNS:                       \
    case CPSS_98DX1035_CNS:                       \
    case CPSS_98DX1053_CNS:                       \
    case CPSS_98DX1035_1_CNS:                     \
    case CPSS_98DX1055_CNS:                       \
    case CPSS_98DX1133_CNS:                       \
    case CPSS_98DX1134_CNS:                       \
    case CPSS_98DX1135_CNS:                       \
    case CPSS_98DX1163_CNS:                       \
    case CPSS_98DX1165_CNS:                       \
    case CPSS_98DX1083_CNS:                       \
    case CPSS_98DX1085_CNS  /*caller end with ':' */

/* xCat2 with GE ports case */
#define PRV_CPSS_DXCH_XCAT2_GE_DEVICES_CASES_MAC \
    /*caller start with 'case' */                 \
         CPSS_98DX3011B_CNS:                      \
    case CPSS_98DX3011C_CNS:                      \
    case CPSS_98DX3021B_CNS:                      \
    case CPSS_98DX3021C_CNS:                      \
    case CPSS_98DX3027B_CNS:                      \
    case CPSS_98DX3033B_CNS:                      \
    case CPSS_98DX3034B_CNS:                      \
    case CPSS_98DX3035B_CNS:                      \
    case CPSS_98DX3053_CNS:                       \
    case CPSS_98DX3054_CNS:                       \
    case CPSS_98DX3055_CNS:                       \
    case CPSS_98DX3031B_CNS:                      \
    case CPSS_98DX3133_CNS:                       \
    case CPSS_98DX3134_CNS:                       \
    case CPSS_98DX3135_CNS:                       \
    case CPSS_98DXZ365_CNS:                       \
    case CPSS_98DX3163_CNS:                       \
    case CPSS_98DX3165_CNS:                       \
    case CPSS_98DX3083_CNS:                       \
    case CPSS_98DX3085_CNS:                       \
    case CPSS_98DXH335_CNS  /*caller end with ':' */

/* xCat2 all devices case */
#define PRV_CPSS_DXCH_XCAT2_DEVICES_CASES_MAC                      \
    /*caller start with 'case' */                                   \
         PRV_CPSS_DXCH_XCAT2_FE_DEVICES_CASES_MAC : \
    case PRV_CPSS_DXCH_XCAT2_GE_DEVICES_CASES_MAC /*caller end with ':' */

/* Bobcat2 devices */
#define PRV_CPSS_BOBCAT2_DEVICES \
    CPSS_98DX42XX_CNS,           \
    CPSS_98DX4251_CNS,           \
    CPSS_98DX4220_CNS,           \
    CPSS_98DX4221_CNS,           \
    CPSS_98DX4222_CNS,           \
    CPSS_98DX4223_CNS,           \
    CPSS_98DX8216_CNS,           \
    CPSS_98DX8219_CNS,           \
    CPSS_98DX8224_CNS,           \
    CPSS_98DX4253_CNS

/* Bobk Caelum devices */
#define PRV_CPSS_BOBK_CAELUM_DEVICES \
    CPSS_98DX42KK_CNS,               \
    CPSS_98DX4203_CNS,               \
    CPSS_98DX4204_CNS,               \
    CPSS_98DX4211_CNS,               \
    CPSS_98DX4210_CNS

/* Bobk Cetus devices */
#define PRV_CPSS_BOBK_CETUS_DEVICES \
    CPSS_98DX4235_CNS,              \
    CPSS_98DX8212_CNS,              \
    CPSS_98DX8208_CNS

/* Aldrin devices */
#define PRV_CPSS_ALDRIN_DEVICES \
    CPSS_98DX8316_CNS,               \
    CPSS_98DX8324_CNS,               \
    CPSS_98DX8332_CNS


/* Bobk devices */
#define PRV_CPSS_BOBK_DEVICES \
    PRV_CPSS_BOBK_CAELUM_DEVICES,   \
    PRV_CPSS_BOBK_CETUS_DEVICES

/* bobcat2 all devices case */
#define CPSS_BOBCAT2_ALL_DEVICES_CASES_MAC        \
    /*caller start with 'case' */                 \
          CPSS_98DX4251_CNS:                      \
     case CPSS_98DX4220_CNS:                      \
     case CPSS_98DX4221_CNS:                      \
     case CPSS_98DX4222_CNS:                      \
     case CPSS_98DX4223_CNS:                      \
     case CPSS_98DX42XX_CNS:                      \
     case CPSS_98DX8216_CNS:                      \
     case CPSS_98DX8219_CNS:                      \
     case CPSS_98DX8224_CNS:                      \
     case CPSS_98DX4253_CNS    /*caller end with ':' */

/* BobK Caelum devices case */
#define CPSS_BOBK_CAELUM_DEVICES_CASES_MAC    \
    /*caller start with 'case' */                   \
         CPSS_98DX42KK_CNS:                         \
    case CPSS_98DX4203_CNS:                         \
    case CPSS_98DX4204_CNS:                         \
    case CPSS_98DX4211_CNS:                         \
    case CPSS_98DX4210_CNS /*caller end with ':' */

/* BobK Cetus devices case */
#define CPSS_BOBK_CETUS_DEVICES_CASES_MAC    \
    /*caller start with 'case' */                   \
         CPSS_98DX8208_CNS:                         \
    case CPSS_98DX4235_CNS:                         \
    case CPSS_98DX8212_CNS /*caller end with ':' */


/* Aldrin devices case */
#define CPSS_ALDRIN_DEVICES_CASES_MAC    \
         CPSS_98DX8316_CNS:                   \
    case CPSS_98DX8324_CNS:                   \
    case CPSS_98DX8332_CNS  /*caller end with ':' */

/* bobk all devices case */
#define CPSS_BOBK_ALL_DEVICES_CASES_MAC             \
    /*caller start with 'case' */                   \
         CPSS_BOBK_CAELUM_DEVICES_CASES_MAC:  \
    case CPSS_BOBK_CETUS_DEVICES_CASES_MAC /*caller end with ':' */


/* Bobcat3 devices */
#define PRV_CPSS_BOBCAT3_ALL_DEVICES \
    CPSS_98DX42FF_CNS

/* Bobcat3 all devices case */
#define CPSS_BOBCAT3_ALL_DEVICES_CASES_MAC          \
    /*caller start with 'case' */                   \
         CPSS_98DX42FF_CNS /*caller end with ':' */



/* specific error codes */
#define CPSS_PRESTERA_ERROR_BASE_CNS  0x10000

/* support running from the PSS */
#ifndef __gtTypesh

#define GT_HW_ERROR         (CPSS_PRESTERA_ERROR_BASE_CNS | (0x17))/* Hardware error*/
/* Transmit operation not succeeded */
#define GT_TX_ERROR         (CPSS_PRESTERA_ERROR_BASE_CNS | (0x18))
/* Receive operation not succeeded  */
#define GT_RCV_ERROR        (CPSS_PRESTERA_ERROR_BASE_CNS | (0x19))
/* Re-perform the interrupt handling */
#define GT_REDO             (CPSS_PRESTERA_ERROR_BASE_CNS | (0x1E))
/* PP memory allocation failed.          */
#define GT_OUT_OF_PP_MEM    (CPSS_PRESTERA_ERROR_BASE_CNS | (0x1D))
/* interrupts cannot be processed and should not be unmasked */
#define GT_STOP_INT         (CPSS_PRESTERA_ERROR_BASE_CNS | (0x1F))

/* support running from the PSS */
#endif /* ! defined __gtTypesh */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __cpssTypesh */

