/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenBrgGen.h
*
* DESCRIPTION:
*       CPSS Bridge Generic APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/
#ifndef __cpssGenBrgSrcIdh
#define __cpssGenBrgSrcIdh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Typedef: CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT
 *
 * Description:
 * This enum defines Bridge Source Id Assignment Mode.
 *
 * Fields:
 *  CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E - 
 *                                     If packet's MAC Source address 
 *                                     found in the FDB then the packet is
 *                                     assigned with Source ID taken from that
 *                                     SA FDB entry, otherwise 
 *                                     Packet is assigned with the port's 
 *                                     default Source ID.     
 *
 *                                     For xCat above
 *                                     If packet's MAC Source address
 *                                     found in the FDB then the packet is
 *                                     assigned with Source ID taken from that
 *                                     SA FDB entry, otherwise 
 *                                     Packet is assigned with extended DSA tag's
 *                                     Source ID (if exists) or port's default Source ID.
 *
 *  CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E -
 *                                     Packet is assigned with the port's 
 *                                     default Source ID regardless of 
 *                                     MAC SA matching in the FDB.   
 *       
 *                                     For xCat above
 *                                     Packet is assigned with extended DSA tag's
 *                                     Source ID (if exists) or port's default Source ID
 *                                     otherwise.   
 * 
 *  CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E - 
 *                                     For xCat above
 *                                     If packet's MAC Destination address 
 *                                     found in the FDB then the packet is
 *                                     assigned with Source ID taken from that
 *                                     DA FDB entry, otherwise 
 *                                     Packet is assigned with extended DSA tag's
 *                                     Source ID (if exists) or port's default Source ID.
 *
 */

typedef enum 
{
    CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_PORT_DEFAULT_E,
    CPSS_BRG_SRC_ID_ASSIGN_MODE_PORT_DEFAULT_E,
    CPSS_BRG_SRC_ID_ASSIGN_MODE_FDB_DA_PORT_DEFAULT_E
}CPSS_BRG_SRC_ID_ASSIGN_MODE_ENT;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenBrgGenh */


