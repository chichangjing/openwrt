/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssTypes.h
*
* DESCRIPTION:
*       A lua wrapper for 
*       It implements support for the following types:
*           CPSS_MAC_ENTRY_EXT_STC (mask)
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#ifdef CHX_FAMILY
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#endif

#include <lua.h>


/*************************************************************************
* prv_lua_to_c_mask_CPSS_PORTS_BMP_STC
*
* Description:
*       Convert lua CPSS_PORTS_BMP_STC comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_PORTS_BMP_STC(
    lua_State                               *L,
    CPSS_PORTS_BMP_STC                      *val
);


#ifdef CHX_FAMILY
/*************************************************************************
* prv_lua_to_c_mask_CPSS_DXCH_BRG_VLAN_INFO_STC
*
* Description:
*       Convert lua CPSS_DXCH_BRG_VLAN_INFO_STC comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_DXCH_BRG_VLAN_INFO_STC
(
    lua_State                               *L,
    CPSS_DXCH_BRG_VLAN_INFO_STC             *val
);


/*************************************************************************
* prv_lua_to_c_mask_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
*
* Description:
*       Convert lua CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC comparision mask 
*       to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
(
    lua_State                               *L,
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    *val
);
#endif


/*************************************************************************
* prv_lua_to_c_mask_CPSS_MAC_ENTRY_EXT_STC
*
* Description:
*       Convert lua CPSS_MAC_ENTRY_EXT_STC comparision mask to "C"
*
* Parameters:
*       value at the top of stack
*
* Returns:
*       data at the top of stack
*
*************************************************************************/
void prv_lua_to_c_mask_CPSS_MAC_ENTRY_EXT_STC
(
    lua_State               *L,
    CPSS_MAC_ENTRY_EXT_STC  *val
);

