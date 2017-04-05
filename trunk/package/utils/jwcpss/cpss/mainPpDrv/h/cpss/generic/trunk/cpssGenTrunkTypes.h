/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenTrunkTypes.h
*
* DESCRIPTION:
*       API definitions for 802.3ad Link Aggregation (Trunk) facility
*
*
*       States and Modes :
*       A. Application can work in one of 2 modes:
*           1. use "high level" trunk manipulations set of APIs - Mode "HL"
*           2. use "low level" trunk HW tables/registers set of APIs - MODE "LL"
*
*       B. Using "high level" trunk manipulations set of APIs - "High level"
*           Mode.
*           In this mode the CPSS synchronize all the relevant trunk
*           tables/registers, implement some WA for trunks Errata.
*           1. Create trunk
*           2. Destroy trunk
*           3. Add/remove member to/from trunk
*           4. Enable/disable in trunk
*           5. Add/Remove port to/from "non-trunk" entry
*        C. Using "low level" trunk HW tables/registers set of APIs- "Low level"
*           Mode.
*           In this mode the CPSS allow direct access to trunk tables/registers.
*           In this mode the Application required to implement the
*           synchronization between the trunk t6ables/registers and to implement
*           the WA to some of the trunk Errata.
*
*           1. Set per port the trunkId
*           2. Set per trunk the trunk members , number of members
*           3. Set per trunk the Non-trunk local ports bitmap
*           4. Set per Index the designated local ports bitmap
*
*         D. Application responsibility is not to use a mix of using API from
*            those 2 modes
*            The only time that the Application can shift between the 2 modes,
*            is only when there are no trunks in the device.
*
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#ifndef __cpssGenTrunkTypesh
#define __cpssGenTrunkTypesh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>

/* max number of members in a trunk */
#define CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS   8

/*
 * Typedef: struct CPSS_TRUNK_MEMBER_STC
 *
 * Description: A structure to hold the info on trunk member : device,port
 *  the trunk holds only 'physical' ports
 *
 * Fields:
 *    port     - the physical port number
 *    hwDevice - the HW device number
 */
typedef struct{
    GT_PHYSICAL_PORT_NUM    port;
    GT_HW_DEV_NUM           hwDevice;

}CPSS_TRUNK_MEMBER_STC;

/*
 * Typedef: struct CPSS_TRUNK_WEIGHTED_MEMBER_STC
 *
 * Description: A structure to hold the info on weighted trunk member : member ,weight.
 *
 *
 * Fields:
 *    member - the trunk member ({dev,port})
 *    weight - the weight of this member.
 *             this weight is relative to the total weights of all members of this trunk
 */
typedef struct{
    CPSS_TRUNK_MEMBER_STC   member;
    GT_U32                  weight;
}CPSS_TRUNK_WEIGHTED_MEMBER_STC;

/*
 * Typedef: enumeration CPSS_TRUNK_TYPE_ENT
 *
 * Description: An enumeration for the trunk type in the DB of the CPSS.
 *
 * values:
 *    CPSS_TRUNK_TYPE_FREE_E    - the trunk is empty .
 *              trunk at this stage supports ALL (high level) APIs.
 *    CPSS_TRUNK_TYPE_REGULAR_E - the trunk used as regular trunk.
 *              supports next (high level) APIs. (DXCH example) :
 *       cpssDxChTrunkMembersSet ,
 *       cpssDxChTrunkMemberAdd , cpssDxChTrunkMemberRemove,
 *       cpssDxChTrunkMemberEnable , cpssDxChTrunkMemberDisable
 *       cpssDxChTrunkDbEnabledMembersGet , cpssDxChTrunkDbDisabledMembersGet
 *       cpssDxChTrunkDesignatedMemberSet , cpssDxChTrunkDbDesignatedMemberGet
 *       and cpssDxChTrunkDbIsMemberOfTrunk
 *
 *    CPSS_TRUNK_TYPE_CASCADE_E - the trunk used as "cascade trunk" ('Local trunk').
 *              supports next (high level) APIs. (DXCH example) :
 *       cpssDxChTrunkCascadeTrunkPortsSet , cpssDxChTrunkCascadeTrunkPortsGet
 *       cpssDxChTrunkCascadeTrunkWithWeightedPortsSet , cpssDxChTrunkCascadeTrunkWithWeightedPortsGet
 *       and cpssDxChTrunkDbIsMemberOfTrunk
 *
 */
typedef enum{
    CPSS_TRUNK_TYPE_FREE_E,
    CPSS_TRUNK_TYPE_REGULAR_E,
    CPSS_TRUNK_TYPE_CASCADE_E
}CPSS_TRUNK_TYPE_ENT;

/*
 * typedef: enum CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_E
 *
 * Description: enumeration for the options that each port can be represented in
 *          the entries of the 'designated MC table'.
 *          it is relevant for devices that hold separated 'Designated Table for UC and MC'
 *
 * Enumerations:
 *  CPSS_TRUNK_DESIGNATED_TABLE_MC_MEMBER_MODE_NATIVE_WEIGHTED_E - this is
 *          the 'regular' mode in which the port hold representation in the
 *          'Designated MC Table' according to it's 'weight' in the (cascade)trunk
 *
 *  CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORBID_ALL_E - the port forbid
 *          all MC traffic (hence no representation in 'Designated MC Table',
 *          so other ports in the trunk get more representation)
 *
 *  CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORCE_ALL_E - the port force
 *          all MC traffic (hence representation in each and every entry of
 *          'Designated MC Table' , this also cause other ports in the trunk
 *          get more representation)
 *
 *
 ************************************************
 *  example of those modes: (port 3 will have different modes)
 *
 *      assume 3 ports 1,2,3 in the same trunk :
 *      port 1 mode 'NATIVE_WEIGHTED' --> this is 'default' of all ports
 *      port 2 mode 'NATIVE_WEIGHTED'
 *      port 3 mode 'NATIVE_WEIGHTED'
 *
 *      the 'Designated MC Table' would look like this:
 *
 *     port |  0   1   2   3   4   5   6
 *      --------------------------------
 *  index 0 |  1   x           1   1   1
 *        1 |  1       x       1   1   1
 *        2 |  1           x   1   1   1
 *        3 |  1   x           1   1   1
 *        4 |  1       x       1   1   1
 *        5 |  1           x   1   1   1
 *        6 |  1   x           1   1   1
 *        7 |  1       x       1   1   1
 *
 ************************************************
 *      port 1 mode 'NATIVE_WEIGHTED'
 *      port 2 mode 'NATIVE_WEIGHTED'
 *      port 3 mode 'FORBID_ALL'
 *
 *      the 'Designated MC Table' would look like this:
 *
 *     port |  0   1   2   3   4   5   6
 *      --------------------------------
 *  index 0 |  1   x       -   1   1   1
 *        1 |  1       x   -   1   1   1
 *        2 |  1   x       -   1   1   1
 *        3 |  1       x   -   1   1   1
 *        4 |  1   x       -   1   1   1
 *        5 |  1       x   -   1   1   1
 *        6 |  1   x       -   1   1   1
 *        7 |  1       x   -   1   1   1
 *
 *
 ************************************************
 *      port 1 mode 'NATIVE_WEIGHTED'
 *      port 2 mode 'NATIVE_WEIGHTED'
 *      port 3 mode 'FORCE_ALL'
 *
 *      the 'Designated MC Table' would look like this:
 *
 *     port |  0   1   2   3   4   5   6
 *      --------------------------------
 *  index 0 |  1   x       x   1   1   1
 *        1 |  1       x   x   1   1   1
 *        2 |  1   x       x   1   1   1
 *        3 |  1       x   x   1   1   1
 *        4 |  1   x       x   1   1   1
 *        5 |  1       x   x   1   1   1
 *        6 |  1   x       x   1   1   1
 *        7 |  1       x   x   1   1   1
 *
 *
 */
typedef enum{
    CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_NATIVE_WEIGHTED_E   = GT_TRUE,  /*1*/
    CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORBID_ALL_E        = GT_FALSE, /*0*/
    CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORCE_ALL_E         = 2         /*2*/
}CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT;

/*
 * typedef: enum CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT
 *
 * Description: enumeration to allow application to be responsible for the
 *      'TrunkId of the port'.
*       in High level mode APIs the 'TrunkId of the port' is set internally be
*       the CPSS and may override the current value of the 'TrunkId of the port'.
*       This API allows application to notify the CPSS to not manage the 'TrunkId
*       of the port'.
 *
 * Enumerations:
 *  CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_CPSS_E - (the default mode) the
 *      CPSS fully manage the 'trunkId' of the port.
 *
 *  CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_APPLICATION_E - the application
 *      fully manage the 'trunkId' of the port.
 *
*/
typedef enum{
    CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_CPSS_E,
    CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_APPLICATION_E
}CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenTrunkTypesh */

