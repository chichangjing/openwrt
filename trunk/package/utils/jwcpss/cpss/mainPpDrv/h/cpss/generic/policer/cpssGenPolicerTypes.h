/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenPolicerTypes.h
*
* DESCRIPTION:
*       Generic definitions for Traffic Policer.
*       
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

#ifndef __cpssGenPolicerTypesh
#define __cpssGenPolicerTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/******************************************************************************/
/**********************   Policer API Enumeration Types  **********************/
/******************************************************************************/

/*
 * typedef: enum CPSS_POLICER_PACKET_SIZE_MODE_ENT
 *
 * Description: Enumeration of metering and counting packet size.
 *
 * Enumerations:
 *
 *   CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E - the policing and counting is 
 *                                         done according to Tunnel Passenger
 *                                         packet size:
 *                                         For IP-Over-x packets, the counting
 *                                         includes the passenger packet's
 *                                         Byte Count, excluding the tunnel 
 *                                         header and the packets CRC
 *                                         For, Ethernet-Over-x packets, the
 *                                         counting includes the passenger
 *                                         packet's Byte Count, excluding the
 *                                         tunnel header and if packet includes
 *                                         Two CRC patterns (one for the inner
 *                                         packet and one for the outer packets)
 *                                         also excluding the outer CRC.
 *
 *    CPSS_POLICER_PACKET_SIZE_L3_ONLY_E     - the policing and counting is 
 *                                         done according to L3 datagram 
 *                                         size only.
 *
 *    CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E  - the policing and counting is 
 *                                         done according to the packet 
 *                                         length including L2.
 *
 *    CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E  - the policing and counting is 
 *                                         done according to the packet 
 *                                         length including L1
 *                                         (include preamble + IFG + FCS).
 */
typedef enum
{
    CPSS_POLICER_PACKET_SIZE_TUNNEL_PASSENGER_E, 
    CPSS_POLICER_PACKET_SIZE_L3_ONLY_E, 
    CPSS_POLICER_PACKET_SIZE_L2_INCLUDE_E,
    CPSS_POLICER_PACKET_SIZE_L1_INCLUDE_E
} CPSS_POLICER_PACKET_SIZE_MODE_ENT;

/*
 * typedef: CPSS_POLICER_TB_MODE_ENT
 *
 * Description: Policer Token Bucket mode
 *
 * Enumerations:
 *
 *      CPSS_POLICER_TB_STRICT_E - If Token Bucket Size Count > Packet's Byte  
 *                  Count then packet is comforming, else it is out of profile
 *	    CPSS_POLICER_TB_LOOSE_E  - If Token Bucket Size Count  > Policer MRU     
 *                  then packet is comforming, else it is out of profile
*/
typedef enum 
{
    CPSS_POLICER_TB_STRICT_E,
    CPSS_POLICER_TB_LOOSE_E
}CPSS_POLICER_TB_MODE_ENT;


/*
 * typedef: CPSS_POLICER_MRU_ENT
 *
 * Description: Policer MRU 	
 *
 * Enumerations:
 *
 *	CPSS_POLICER_MRU_1536_E - 1.5 KB,
 *	CPSS_POLICER_MRU_2K_E   - 2 KB,
 *	CPSS_POLICER_MRU_10K_E  - 10 KB
*/
typedef enum 
{
    CPSS_POLICER_MRU_1536_E,
    CPSS_POLICER_MRU_2K_E,
    CPSS_POLICER_MRU_10K_E
}CPSS_POLICER_MRU_ENT ;

/*
 * typedef: enum CPSS_POLICER_COLOR_MODE_ENT
 *
 * Description: Enumeration of Meter color modes
 *
 * Enumerations:
 *      CPSS_POLICER_COLOR_BLIND_E - color blind mode.
 *      CPSS_POLICER_COLOR_AWARE_E - color aware mode.
 * Notes:
 *      This applies to both Meter types:
 *      single rate three color marking (srTCM)
 *      two rate single color marking (trTCM)
 */
typedef enum
{
    CPSS_POLICER_COLOR_BLIND_E,
    CPSS_POLICER_COLOR_AWARE_E
} CPSS_POLICER_COLOR_MODE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenPolicerTypesh */

