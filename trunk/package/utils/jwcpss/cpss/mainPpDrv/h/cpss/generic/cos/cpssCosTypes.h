/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssCosTypes.h
*
* DESCRIPTION:
*       Includes structures definition of COS module.
*
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#ifndef __cpssCosTypesh
#define __cpssCosTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>

/* DSCP range limit */
#define CPSS_DSCP_RANGE_CNS  64

/* EXP range limit */
#define CPSS_EXP_RANGE_CNS  8

/* Traffic class range limit */
#define CPSS_TC_RANGE_CNS   8

/* Traffic class range limit */
#define CPSS_4_TC_RANGE_CNS   4


/* User priority range limit */
#define CPSS_USER_PRIORITY_RANGE_CNS    8

/* Drop Precedence range limit */
#define CPSS_DP_RANGE_CNS 8

/* Drop Precedence range limit */
#define CPSS_4_DP_RANGE_CNS 4


/*
 * typedef: enum CPSS_DP_LEVEL_ENT
 *
 * Description: Enumeration of drop precedence levels.
 *
 * Enumerations:
 *    CPSS_DP_GREEN_E  - conforming
 *    CPSS_DP_YELLOW_E - last conforming
 *    CPSS_DP_RED_E    - non conforming
 */
typedef enum
{
    CPSS_DP_GREEN_E = 0,
    CPSS_DP_YELLOW_E,
    CPSS_DP_RED_E

}CPSS_DP_LEVEL_ENT;


/*
 * typedef: enum CPSS_COS_TABLE_TYPE_ENT
 *
 * Description:
 *
 * Enumerations:
 *      CPSS_COS_TABLE_MARKING_E - The COS table used for initial marking of the
 *      packet COS.
 *      CPSS_COS_TABLE_REMARKING_E - The COS table used for re-marking of the
 *      packet COS by the Traffic Conditioner.
 *
 * COMMENTS:
 *      Some packet processors do not support separate marking and remarking
 *      tables. For these devices, the same COS table is updated regardless of
 *      the CPSS_COS_TABLE_TYPE_ENT. See the packet processor data sheet for
 *      details.
 */
typedef enum
{
    CPSS_COS_TABLE_MARKING_E = 0,
    CPSS_COS_TABLE_REMARKING_E
} CPSS_COS_TABLE_TYPE_ENT;


/*
 * Typedef: CPSS_COS_STC
 *
 * Description: The Prestera Class-of-Service parameters.
 *
 * Fields:
 *    userPriority    - IEEEE 802.1p User Priority, range 0-7
 *    dropPrecedence  - Drop precedence (color), range 0-2
 *    trafficClass    - Egress tx traffic class queue, range 0-7
 */
typedef struct
{
    CPSS_DP_LEVEL_ENT   dropPrecedence;
    GT_U8               userPriority;
    GT_U8               trafficClass;
}CPSS_COS_STC;


/*
 * typedef: struct CPSS_COS_MAP_STC
 *
 * Description: Structure for Cos Mapping entry
 *
 * Fields:
 *      dscp     - DiffServ Code point
 *      cosParam - Class of service parameters.
 *      expField - Experimental field in MPLS Shim header, used for DiffServ
 *                 support of MPLS
 *      userDef  - User defined 15 bits value passed to the CIB for external
 *                 processing
 */
typedef struct
{
    GT_U8           dscp;
    CPSS_COS_STC    cosParam;
    GT_U8           expField;
    GT_U16          userDef;
}CPSS_COS_MAP_STC;


/*
 * typedef: enum CPSS_COS_USER_PRIO_MODE_ENT
 *
 * Description:
 *      untagged packet VPT assign mode.
 *
 * Enumerations:
 *      CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E
 *            assign ser priority (VPT) according to ingress packet port.
 *      CPSS_COS_USER_PRIO_MODE_TC_PER_PORT_E
 *            assign user priority (VPT) according to TC.
 *      CPSS_COS_USER_PRIO_MODE_MAX_E
 *            the MAX value (to use for array sizes)
 *
 * COMMENTS:
 *      Untagged packet gets the user priority (VPT)
 *      according to the mode, that is:
 *      1. User priority per port.
 *      2. TC per port which is mapped to user priority
 *         using the TC to User priority table.
 */
typedef enum
{
    CPSS_COS_USER_PRIO_MODE_USER_PRIORITY_PER_PORT_E = 0,
    CPSS_COS_USER_PRIO_MODE_TC_PER_PORT_E,
    CPSS_COS_USER_PRIO_MODE_MAX_E
} CPSS_COS_USER_PRIO_MODE_ENT;


/*
 * typedef: enum CPSS_COS_MAC_ADDR_TYPE_ENT
 *
 * Description:
 *      Enumeration for the MAC TC assignment settings.
 *
 * Enumerations:
 *      CPSS_COS_MAC_ADDR_TYPE_NONE_E    - No override of TC
 *      CPSS_COS_MAC_ADDR_TYPE_ALL_E     - Set TC if match mac entry of both
 *                                         SA,DA mac entries
 *      CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E - Set TC if match mac entry by
 *                                         SA mac entry
 *      CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E - Set TC if match mac entry by
 *                                         DA mac entry
 *
 * COMMENTS:
 *      Not all values supported by all PP.
 *
 */
typedef enum
{
    CPSS_COS_MAC_ADDR_TYPE_NONE_E,
    CPSS_COS_MAC_ADDR_TYPE_ALL_E,
    CPSS_COS_MAC_ADDR_TYPE_SA_ONLY_E,
    CPSS_COS_MAC_ADDR_TYPE_DA_ONLY_E
}CPSS_COS_MAC_ADDR_TYPE_ENT;


/*
 * typedef: enum CPSS_COS_PARAM_ENT
 *
 * Description:
 *      enumeration for the Cos parameters
 *
 * Enumerations:
 *     CPSS_COS_PARAM_DSCP_E - DSCP parameter
 *     CPSS_COS_PARAM_EXP_E  - EXP parameter
 *     CPSS_COS_PARAM_TC_E   - TC parameter
 *     CPSS_COS_PARAM_UP_E   - UP parameter
 *     CPSS_COS_PARAM_DP_E   - DP parameter
 *
 */
typedef enum
{
    CPSS_COS_PARAM_DSCP_E,
    CPSS_COS_PARAM_EXP_E,
    CPSS_COS_PARAM_TC_E,
    CPSS_COS_PARAM_UP_E,
    CPSS_COS_PARAM_DP_E
}CPSS_COS_PARAM_ENT;


/**********************************************************************
 * typedef: struct CPSS_QOS_ENTRY_STC
 *
 * Description:
 *      Port QoS Configuration.
 *
 * Enumerations:
 *      qosProfileId     -  The port's QoSProfile. This is the port's default
 *                          QoSProfile and may be assigned to the packet as part
 *                          of the QoS assignment algorithm.
 *      assignPrecedence -  port marking of the QoS Precedence.
 *      enableModifyUp   - mac modification mode of the up filed
 *      enableModifyDscp - mac modification mode of the dscp filed (or EXP for MPLS)
 * COMMENTS:
 ***********************************************************************/
typedef struct
{
    GT_U32                                       qosProfileId;
    CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT  assignPrecedence;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT        enableModifyUp;
    CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT        enableModifyDscp;
}CPSS_QOS_ENTRY_STC;


/*
 * typedef: enum CPSS_QOS_PORT_TRUST_MODE_ENT
 *
 * Description:
 *      Trust modes for the port.
 *
 * Enumerations:
 *      CPSS_QOS_PORT_NO_TRUST_E - untrust: packet QosProfile is assigned as the
 *                             port's default PortQosProfile.
 *      CPSS_QOS_PORT_TRUST_L2_E - if the packet is tagged the Qos Profile is
 *          assigned according to User Priority field, if the packet isn't
 *          tagged the  QoS Profile is assigned by default Port's QoS Profile.
 *      CPSS_QOS_PORT_TRUST_L3_E - if the packet is IPv4/IPv6 the QoS profile is
 *          assigned to the Dscp field or remapped Dscp, else according to
 *          Port's Default QoS Profile.
 *      CPSS_QOS_PORT_TRUST_L2_L3_E - if the packet is IPv4/IPv6 the packet is
 *          assigned like Trust L3, else if the packet is tagged - like Trust
 *          L2, else the packet is assigned Port's Default QoS profile.
 *
 * COMMENTS:
 */
typedef enum
{
    CPSS_QOS_PORT_NO_TRUST_E = 0,
    CPSS_QOS_PORT_TRUST_L2_E,
    CPSS_QOS_PORT_TRUST_L3_E,
    CPSS_QOS_PORT_TRUST_L2_L3_E
}CPSS_QOS_PORT_TRUST_MODE_ENT;


/**********************************************************************
 * typedef: enum CPSS_MAC_QOS_RESOLVE_ENT
 *
 * Description:
 *      MAC QoS Marking Conflict Resolution mode.
 *
 * Enumerations:
 *      CPSS_MAC_QOS_GET_FROM_DA_E - get the QoS attribute index from DA.
 *      CPSS_MAC_QOS_GET_FROM_SA_E - get the QoS attribute index from SA
 *
 * COMMENTS:
 */
typedef enum
{
     CPSS_MAC_QOS_GET_FROM_DA_E =0,
     CPSS_MAC_QOS_GET_FROM_SA_E
} CPSS_MAC_QOS_RESOLVE_ENT;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssExMxCosTypesh */

