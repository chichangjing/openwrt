
/*******************************************************************************
 *              (C), Copyright 2001, Marvell International Ltd.                 *
 * THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
 * NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
 * OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
 * DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
 * THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
 * IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
 ********************************************************************************
 * tgfCommon.h
 *
 * DESCRIPTION:
 *       Common helper API, types, variables for enhanced UTs
 *
 *
 * FILE REVISION NUMBER:
 *       $Revision: 1 $
 *
 *******************************************************************************/
#ifndef __tgfCommon
#define __tgfCommon

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* copy field from source structure into destination structure  */
#define PRV_TGF_STC_S2D_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        (dstStcPtr)->field = (srcStcPtr)->field

/* copy field from destination structure into source structure  */
#define PRV_TGF_STC_D2S_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        (srcStcPtr)->field = (dstStcPtr)->field

/* copy field from source structure into destination structure  - for union field */
#define PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        cpssOsMemCpy((&(dstStcPtr)->field),&((srcStcPtr)->field),sizeof((dstStcPtr)->field))

/* copy field from destination structure into source structure  - for union field */
#define PRV_TGF_STC_D2S_UNION_FIELD_COPY_MAC(dstStcPtr, srcStcPtr, field)            \
        PRV_TGF_STC_S2D_UNION_FIELD_COPY_MAC(srcStcPtr,dstStcPtr,field)

/******************************************************************************\
 *                          Private type definitions                          *
\******************************************************************************/

/*
 * Typedef: enum PRV_TGF_OUTLIF_TYPE_ENT
 *
 * Description: enumerator for outlif type
 *
 * Fields:
 *      PRV_TGF_OUTLIF_TYPE_LL_E     - outlif is a regular ethernet interface
 *      PRV_TGF_OUTLIF_TYPE_DIT_E    - outlif is a DIT entry
 *      PRV_TGF_OUTLIF_TYPE_TUNNEL_E - outlif is a tunnel
 */
typedef enum {
    PRV_TGF_OUTLIF_TYPE_LL_E,
    PRV_TGF_OUTLIF_TYPE_DIT_E,
    PRV_TGF_OUTLIF_TYPE_TUNNEL_E
} PRV_TGF_OUTLIF_TYPE_ENT;

/*
 * typedef: enum PRV_TGF_TS_PASSENGER_PACKET_TYPE_ENT
 *
 * Description:
 *      Tunnel Start passenger protocol types
 *
 * Enumerations:
 *      PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E - ethernet
 *      PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E    - other (IP or MPLS)
 *
 */
typedef enum
{
    PRV_TGF_TS_PASSENGER_PACKET_TYPE_ETHERNET_E,
    PRV_TGF_TS_PASSENGER_PACKET_TYPE_OTHER_E
} PRV_TGF_TS_PASSENGER_PACKET_TYPE_ENT;

/*
 * Typedef: structure PRV_TGF_OUTLIF_INFO_STC
 *
 * Description: Defines the interface info
 *
 * Fields:
 *      outlifType          - the outlif type (Link Layer/DIT/Tunnel)
 *      interfaceInfo       - interface info (devPort, trunk, vlan, vidx, device)
 *      arpPtr              - pointer to an ARP pointer
 *      ditPtr              - pointer to a DIT entry
 *      tunnelStartPtr      - tunnel start information
 *      passengerPacketType - the packet type
 *      ptr                 - pointer to a tunnel entry
 *
 */
typedef struct
{
    PRV_TGF_OUTLIF_TYPE_ENT   outlifType;

    CPSS_INTERFACE_INFO_STC   interfaceInfo;

    union {
        GT_U32  arpPtr;
        GT_U32  ditPtr;

        struct {
            PRV_TGF_TS_PASSENGER_PACKET_TYPE_ENT passengerPacketType;
            GT_U32                               ptr;
        } tunnelStartPtr;

    } outlifPointer;

} PRV_TGF_OUTLIF_INFO_STC;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __tgfCommon */
