/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChVlan.c
*
* DESCRIPTION:
*       A lua wrapper for vlan functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 16 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <generic/private/prvWraplGeneral.h>
#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>
#include <dxCh/wraplCpssDxChVlan.h>
#include <cpssCommon/wraplCpssTypes.h>


#include <lua.h>

/* DxCh max number of entries to be read from tables\classes */
#define  CPSS_DXCH_VLAN_MAX_ENTRY_CNS 4096


/***** declarations ********/

use_prv_enum(CPSS_BRG_IPM_MODE_ENT);
use_prv_enum(CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT);
use_prv_struct(CPSS_DXCH_BRG_VLAN_INFO_STC);
use_prv_struct(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC);
use_prv_print_struct(CPSS_PORTS_BMP_STC);
use_prv_print_struct(CPSS_DXCH_BRG_VLAN_INFO_STC);
use_prv_print_struct(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC);

/***** declarations ********/


/*******************************************************************************
*   Vlan entries wrapper errors                                                *
*******************************************************************************/
GT_CHAR vlanEntryReadErrorMessage[] = "Could not read vlan entry";


int check_wraplCpssDxChVlan()
{
    cpssOsPrintf("File wraplCpssDxChVlan.c inside");
    return 1;
}


/*******************************************************************************
*   Mirror wrapper errors                                                      *
*******************************************************************************/
GT_CHAR brgVlanEntryReadErrorMessage[] =
                                        "Could not read vlan entry.";


/*******************************************************************************
* prvCpssDxChMaximumVlanIdGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum vlan id.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum             - device number
*
* OUTPUTS:
*        maxVlanIdPtr       - maximum vlan id
*        errorMessagePtrPtr - error message
*
* RETURNS:
        0 or error code and error message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMaximumVlanIdGet
(
    IN  GT_U8                   devNum,
    OUT GT_U32                  *maxVlanIdPtr,
    OUT GT_CHAR_PTR             *errorMessagePtrPtr
)
{
    GT_STATUS                   status          = GT_OK;
    *errorMessagePtrPtr = NULL;

    *maxVlanIdPtr   =
        PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_VLAN_MAC(devNum);


    return status;
}


/*******************************************************************************
* prvCpssDxChIsPortTaggingCmdSupported
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of port tagging supporting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum             - device number
*
* OUTPUTS:
*        isSupportedPtr     - port tagging supporting property
*        errorMessagePtrPtr - error message
*
* RETURNS:
        0 or error code and error message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIsPortTaggingCmdSupported
(
    IN  GT_U8                   devNum,
    OUT GT_BOOL                 *isSupportedPtr,
    OUT GT_CHAR_PTR             *errorMessagePtrPtr
)
{
    GT_STATUS                   status          = GT_OK;

    *errorMessagePtrPtr = NULL;

    *isSupportedPtr     =
        PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.featureInfo.tr101Supported;

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgVlanInfoFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of CPSS_DXCH_BRG_VLAN_INFO_STC from lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                                     - lua state
*       L_index                               - entry lua stack index
*
* OUTPUTS:
*       vlanInfo                              - vlan info structure
*       errorMessagePtr                       - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgVlanInfoFromLuaGet
(
    IN  lua_State                               *L,
    IN  GT_32                                   L_index,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC             *vlanInfo,
    OUT GT_CHAR_PTR                             *errorMessagePtr
)
{
    GT_STATUS                                   status  = GT_OK;

    *errorMessagePtr = NULL;

    cpssOsMemSet(vlanInfo, 0, sizeof(*vlanInfo));

    lua_pushvalue(L, L_index);
    prv_lua_to_c_CPSS_DXCH_BRG_VLAN_INFO_STC(L, vlanInfo);
    lua_pop(L, 1);

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgVlanInfoMaskFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of CPSS_DXCH_BRG_VLAN_INFO_STC comparision mask from lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                                     - lua state
*       L_index                               - entry lua stack index
*
* OUTPUTS:
*       vlanInfo                              - vlan info structure
*       errorMessagePtr                       - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgVlanInfoMaskFromLuaGet
(
    IN  lua_State                               *L,
    IN  GT_32                                   L_index,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC             *vlanInfo,
    OUT GT_CHAR_PTR                             *errorMessagePtr
)
{
    GT_STATUS                                   status  = GT_OK;

    *errorMessagePtr = NULL;

    cpssOsMemSet(vlanInfo, 0, sizeof(*vlanInfo));

    lua_pushvalue(L, L_index);
    prv_lua_to_c_mask_CPSS_DXCH_BRG_VLAN_INFO_STC(L, vlanInfo);
    lua_pop(L, 1);

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgVlanInfoToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing value of type
*       CPSS_DXCH_BRG_VLAN_INFO_STC to lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       status                - caller status
*       L                     - lua state
*       vlanInfo              - vlan info structure
*       condition             - pushing condition
*
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int prvCpssDxChBrgVlanInfoToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC
                                *vlanInfo,
    IN GT_BOOL                  condition
)
{
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        prv_c_to_lua_CPSS_DXCH_BRG_VLAN_INFO_STC(L, vlanInfo);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* prvCpssDxChBrgVlanPortsTagingCommandsFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC from lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                                     - lua state
*       L_index                               - entry lua stack index
*
* OUTPUTS:
*       vlanPortsTaggingCommands              - vlan ports tagging commands
*       errorMessagePtr                       - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgVlanPortsTagingCommandsFromLuaGet
(
    IN  lua_State                               *L,
    IN  GT_32                                   L_index,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    *vlanPortsTaggingCommands,
    OUT GT_CHAR_PTR                             *errorMessagePtr
)
{
    GT_STATUS                                   status  = GT_OK;

    *errorMessagePtr = NULL;

    cpssOsMemSet(vlanPortsTaggingCommands, 0,
                 sizeof(*vlanPortsTaggingCommands));

    lua_pushvalue(L, L_index);
    prv_lua_to_c_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC(L,
                                                      vlanPortsTaggingCommands);
    lua_pop(L, 1);

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgVlanPortsTagingCommandsToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing value of type
*       CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC to lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       status                - caller status
*       L                     - lua state
*       vlanPortsTaggingCommands
*                             - vlan ports tagging commands
*       condition             - pushing condition
*
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int prvCpssDxChBrgVlanPortsTagingCommandsToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
                                *vlanPortsTaggingCommands,
    IN GT_BOOL                  condition
)
{
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        prv_c_to_lua_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC(
            L, vlanPortsTaggingCommands);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* prvCpssDxChBrgVlanPortsTagingCommandsMaskFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC comparision mask from
*       lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                                     - lua state
*       L_index                               - entry lua stack index
*
* OUTPUTS:
*       vlanPortsTaggingCommands              - vlan ports tagging commands
*       errorMessagePtr                       - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgVlanPortsTagingCommandsMaskFromLuaGet
(
    IN  lua_State                               *L,
    IN  GT_32                                   L_index,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    *vlanPortsTaggingCommands,
    OUT GT_CHAR_PTR                             *errorMessagePtr
)
{
    GT_STATUS                                   status  = GT_OK;

    *errorMessagePtr = NULL;

    cpssOsMemSet(vlanPortsTaggingCommands, 0,
                 sizeof(*vlanPortsTaggingCommands));

    lua_pushvalue(L, L_index);
    prv_lua_to_c_mask_CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC(
        L, vlanPortsTaggingCommands);
    lua_pop(L, 1);

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgPortsTagingDataAndVlanInfoFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of vlan info, its ports, tagging and tagging commands from lua
*       stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                                     - lua state
*       L_index                               - entry lua stack index
*
* OUTPUTS:
*       portsMembers                          - vlan ports
*       portsTagging                          - vlan tagged ports
*       vlanInfo                              - vlan info
*       portsTaggingCmd                       - vlan tagging commands
*       errorMessagePtr                       - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgPortsTagingDataAndVlanInfoFromLuaGet
(
    IN    lua_State                             *L,
    IN GT_32                                    L_index,
    OUT CPSS_PORTS_BMP_STC                      *portsMembers,
    OUT CPSS_PORTS_BMP_STC                      *portsTagging,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC             *vlanInfo,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    *portsTaggingCmd,
    OUT GT_CHAR_PTR                             *errorMessagePtr
)
{
    GT_STATUS                                   status  = GT_OK;

    *errorMessagePtr = NULL;

    status  = pvrCpssPortBitmatFromLuaGet(
                  L, L_index++, portsMembers, errorMessagePtr);

    if (GT_OK == status)
    {
        status  = pvrCpssPortBitmatFromLuaGet(
                      L, L_index++, portsTagging, errorMessagePtr);
    }

    if (GT_OK == status)
    {
        status  = prvCpssDxChBrgVlanInfoFromLuaGet(
                      L, L_index++, vlanInfo, errorMessagePtr);
    }

    if (GT_OK == status)
    {
        status  = prvCpssDxChBrgVlanPortsTagingCommandsFromLuaGet(
                      L, L_index, portsTaggingCmd, errorMessagePtr);
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChBrgPortsTagingDataAndVlanInfoMasksFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Mask getting of vlan info, its ports, tagging and tagging commands from
*       lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                                     - lua state
*       L_index                               - entry lua stack index
*
* OUTPUTS:
*       portsMembers                          - vlan ports
*       portsTagging                          - vlan tagged ports
*       vlanInfo                              - vlan info
*       portsTaggingCmd                       - vlan tagging commands
*       errorMessagePtr                       - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBrgPortsTagingDataAndVlanInfoMasksFromLuaGet
(
    IN  lua_State                               *L,
    IN  GT_32                                   L_index,
    OUT CPSS_PORTS_BMP_STC                      *portsMembers,
    OUT CPSS_PORTS_BMP_STC                      *portsTagging,
    OUT CPSS_DXCH_BRG_VLAN_INFO_STC             *vlanInfo,
    OUT CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    *portsTaggingCmd,
    OUT GT_CHAR_PTR                             *errorMessagePtr
)
{
    GT_STATUS                                   status  = GT_OK;

    *errorMessagePtr = NULL;

    status  = pvrCpssPortBitmatMaskFromLuaGet(
                  L, L_index++, portsMembers, errorMessagePtr);

    if (GT_OK == status)
    {
        status  = pvrCpssPortBitmatMaskFromLuaGet(
                      L, L_index++, portsTagging, errorMessagePtr);
    }

    if (GT_OK == status)
    {
        status  = prvCpssDxChBrgVlanInfoMaskFromLuaGet(
                      L, L_index++, vlanInfo, errorMessagePtr);
    }

    if (GT_OK == status)
    {
        status  = prvCpssDxChBrgVlanPortsTagingCommandsMaskFromLuaGet(
                      L, L_index, portsTaggingCmd, errorMessagePtr);
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChVlanInfoToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Read vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status                              - caller status
*       L                                   - lua state
*       devNum                              - device bunber
*       portsMembersPtr                     - bmp of ports members in vlan CPU
*                                             port supported
*       portsTaggingPtr                     - bmp of ports tagged in the vlan -
*                                             The parameter is relevant for
*                                             DxCh1, DxCh2 and DxCh3 devices.
*                                             The parameter is relevant for xCat
*                                             and above devices without TR101
*                                             feature support. The parameter is
*                                             not relevant and ignored for xCat
*                                             and above devices with TR101
*                                             feature support.
*       vlanInfoPtr                         - VLAN specific information
*       portsTaggingCmdPtr                  - ports tagging commands in the
*                                             vlan. The parameter is relevant
*                                             only for xCat and above with TR101
*                                             feature support. The parameter is
*                                             not relevant and ignored for other
*                                             devices.
*       condition                           - pushing condition
*
* OUTPUTS:
*
* RETURNS:
*       1 if boolean value was pushed to stack otherwise 0k
*
* COMMENTS:
*
*******************************************************************************/
int prvCpssDxChVlanInfoToLuaOnConditionPush
(
    IN GT_STATUS                            status,
    IN lua_State                            *L,
    IN GT_U8                                devNum,
    IN CPSS_PORTS_BMP_STC                   *portsMembersPtr,
    IN CPSS_PORTS_BMP_STC                   *portsTaggingPtr,
    IN CPSS_DXCH_BRG_VLAN_INFO_STC          *vlanInfoPtr,
    IN CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC *portsTaggingCmdPtr,
    IN GT_BOOL                              condition
)
{
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        GT_PORT_NUM                         port;
        GT_BOOL                             isTaggingCommandSupported
                                                                = GT_FALSE;
        GT_CHAR_PTR                         errorMessagePtr;

        int t, t1;

        lua_newtable(L);
        t = lua_gettop(L);

        lua_newtable(L); /* portsMembers */
        t1 = lua_gettop(L);
        for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
        {
            if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr, port))
            {
                lua_pushnumber(L, port);
                lua_rawseti(L, t1, lua_objlen(L, t1) + 1);
            }
        }
        lua_setfield(L, t, "portsMembers");

        status = prvCpssDxChIsPortTaggingCmdSupported
                     (devNum, &isTaggingCommandSupported, &errorMessagePtr);

        if ((GT_OK == status) && (GT_TRUE == isTaggingCommandSupported))
        {
            lua_newtable(L); /* portsTaggingCmd */
            t1 = lua_gettop(L);
            for (port = 0; port < CPSS_MAX_PORTS_NUM_CNS; port++)
            {
                prv_c_to_lua_enum(
                    L, enumDescr_CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT,
                    portsTaggingCmdPtr->portsCmd[port]);
                lua_rawseti(L, t1, port);
            }
            lua_setfield(L, t, "portsTaggingCmd");
        }

        lua_newtable(L); /* portsTagging */
        t1 = lua_gettop(L);
            for (port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
            {
                if (GT_TRUE == isTaggingCommandSupported)
                {
                    if ((GT_TRUE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr, port)) &&
                        (GT_TRUE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsTaggingPtr, port)))
                    {
                        lua_pushnumber(L, port);
                        lua_rawseti(L, t1, lua_objlen(L, t1) + 1);
                    }
                }
                else
                {
                    if ((GT_TRUE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr, port)) &&
                        (CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E !=
                                            portsTaggingCmdPtr->portsCmd[port]))
                    {
                        lua_pushnumber(L, port);
                        lua_rawseti(L, t1, lua_objlen(L, t1) + 1);
                    }
                }
            }
        lua_setfield(L, t, "portsTagging");

    #undef  F_BOOL
    #define F_BOOL(field) \
        lua_pushboolean(L, vlanInfoPtr->field); \
        lua_setfield(L, t, #field);

    /* fieldtype in F_NUMBER is ignored,
     * fields will be shared with wrlCpssDxChGetVlanEntrySet */
    #undef  F_NUMBER
    #define F_NUMBER(field, fieldtype) \
        lua_pushnumber(L, vlanInfoPtr->field); \
        lua_setfield(L, t, #field);

    #undef  F_ENUM
    #define F_ENUM(field, fieldtype) \
        prv_c_to_lua_enum(L, enumDescr_##fieldtype, vlanInfoPtr->field); \
        lua_setfield(L, t, #field);

        F_BOOL(unkSrcAddrSecBreach);
        F_NUMBER(unregNonIpMcastCmd, CPSS_PACKET_CMD_ENT);
        F_NUMBER(unregIpv4McastCmd, CPSS_PACKET_CMD_ENT);
        F_NUMBER(unregIpv6McastCmd, CPSS_PACKET_CMD_ENT);
        F_NUMBER(unkUcastCmd, CPSS_PACKET_CMD_ENT);
        F_NUMBER(unregIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
        F_NUMBER(unregNonIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
        F_BOOL(ipv4IgmpToCpuEn);
        F_BOOL(mirrToRxAnalyzerEn);
        F_BOOL(ipv6IcmpToCpuEn);
        F_NUMBER(ipCtrlToCpuEn, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
        F_ENUM(ipv4IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
        F_ENUM(ipv6IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
        F_BOOL(ipv4IpmBrgEn);
        F_BOOL(ipv6IpmBrgEn);
        F_NUMBER(ipv6SiteIdMode, CPSS_IP_SITE_ID_ENT);
        F_BOOL(ipv4UcastRouteEn);
        F_BOOL(ipv4McastRouteEn);
        F_BOOL(ipv6UcastRouteEn);
        F_BOOL(ipv6McastRouteEn);
        F_NUMBER(stgId, GT_U32);
        F_BOOL(autoLearnDisable);
        F_BOOL(naMsgToCpuEn);
        F_NUMBER(mruIdx, GT_U32);
        F_BOOL(bcastUdpTrapMirrEn);
        F_NUMBER(vrfId, GT_U32);
        /* Fields floodVidx and floodVidxMode is not supported and ignored  */
        /* for DxCh1, DxCh1_Diamond, DxCh2 and DxCh3 devices.               */
        if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)   &&
           (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH2_E)  &&
           (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH3_E))
        {
            F_NUMBER(floodVidx, GT_U16);
            F_NUMBER(floodVidxMode, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
        }
        F_NUMBER(portIsolationMode, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
        F_BOOL(ucastLocalSwitchingEn);
        F_BOOL(mcastLocalSwitchingEn);

        lua_settop(L, t);

        return 1;
    }
    else
    {
        return 0;
    }
}


/*******************************************************************************
* prvCpssDxChDoesVlanExist
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Check's that vlan exists.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum                           - device number
*        vlanId                           - vlan id
*       P_CALLING_FORMAL_DATA             - general debug information
*                                           (environment variables etc); could
*                                           be empty
*
* OUTPUTS:
*        doesVlanExistPtr                 - vlan validness property
*        errorMessagePtr                  - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDoesVlanExist
(
    IN  GT_U8                               devNum,
    IN  GT_U16                              vlanId,
    OUT GT_BOOL                             *doesVlanExistPtr,
    OUT GT_CHAR_PTR                         *errorMessagePtrPtr
    P_CALLING_FORMAL_DATA
)
{
    CPSS_PORTS_BMP_STC                      portsMembers;
    CPSS_PORTS_BMP_STC                      portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC             vlanInfo;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    portsTaggingCmd;
    GT_STATUS                               status              = GT_OK;

    *errorMessagePtrPtr = NULL;

    CPSS_API_CALL_LOCK();
    status = cpssDxChBrgVlanEntryRead(devNum, vlanId, &portsMembers,
                                      &portsTagging, &vlanInfo,
                                      doesVlanExistPtr, &portsTaggingCmd);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChBrgVlanEntryRead,
        PC_NUMBER(devNum,                       devNum,                  GT_U8,
        PC_NUMBER(vlanId,                       vlanId,                 GT_U16,
        PC_STRUCT(&portsMembers,                portsMembers,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&portsTagging,                portsTagging,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&vlanInfo,                    vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
        PC_BOOL  (*doesVlanExistPtr,            isValid,
        PC_STRUCT(&portsTaggingCmd,             portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
        PC_LAST_PARAM))))))),
        PC_STATUS);
    if (GT_OK == status)
    {
        *errorMessagePtrPtr  = vlanEntryReadErrorMessage;
    }

    return status;
}


/*******************************************************************************
* prvCpssDxChNextBrgPortVlanGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next port vlan.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum             - device number
*        portNum            - port numer
*        vlanIdPtr          - first checked vlanId
*       P_CALLING_FORMAL_DATA                 - general debug information
*                                               (environment variables etc);
*                                               could be empty
*
* OUTPUTS:
*        vlanIdPtr          - founded vlanId (if relevant)
*        vlanWasFoundPtr    - port vlan founding property
*       portsMembersPtr                       - bmp of ports members in vlan CPU
*                                               port supported
*       portsTaggingPtr                       - bmp of ports tagged in the vlan.
*                                               The parameter is relevant for
*                                               DxCh1, DxCh2 and DxCh3 devices.
*                                               The parameter is relevant for
*                                               xCat and above devices without
*                                               TR101 feature support. The
*                                               parameter is  not relevant and
*                                               ignored for xCat and above
*                                               devices with TR101 feature
*                                               support.
*       vlanInfoPtr                           - VLAN specific information
*       portsTaggingCmdPtr                    - ports tagging commands in the
*                                               vlan. The parameter is relevant
*                                               only for xCat and above with
*                                               TR101 feature support. The
*                                               parameter is not relevant and
*                                               ignored for other
*                                               devices.
*        errorMessagePtr    - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChNextBrgPortVlanGet
(
    IN    GT_U8                                 devNum,
    IN    GT_PORT_NUM                           portNum,
    INOUT GT_U16                                *vlanIdPtr,
    OUT   GT_BOOL                               *vlanWasFoundPtr,
    OUT   CPSS_PORTS_BMP_STC                    *portsMembersPtr,
    OUT   CPSS_PORTS_BMP_STC                    *portsTaggingPtr,
    OUT   CPSS_DXCH_BRG_VLAN_INFO_STC           *vlanInfoPtr,
    OUT   CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC  *portsTaggingCmdPtr,
    OUT   GT_CHAR_PTR                           *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_BOOL                                 isValid;
    GT_STATUS                               status              = GT_OK;

    *vlanWasFoundPtr = GT_FALSE;
    *errorMessagePtr = NULL;

    while ((GT_OK == status) &&
           (*vlanIdPtr <= PRV_CPSS_DXCH_PP_HW_INFO_LAST_INDEX_IN_TABLE_VLAN_MAC(devNum)) &&
           (GT_FALSE == *vlanWasFoundPtr))
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgVlanEntryRead(devNum, (*vlanIdPtr)++,
                                          portsMembersPtr, portsTaggingPtr,
                                          vlanInfoPtr, &isValid,
                                          portsTaggingCmdPtr);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgVlanEntryRead,
            PC_NUMBER(devNum,                   devNum,                  GT_U8,
            PC_NUMBER(*vlanIdPtr,               vlanId,                 GT_U16,
            PC_STRUCT(portsMembersPtr,          portsMembers,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(portsTaggingPtr,          portsTagging,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(vlanInfoPtr,              vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
            PC_BOOL  (isValid,                  isValid,
            PC_STRUCT(portsTaggingCmdPtr,       portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
            PC_LAST_PARAM))))))),
            PC_STATUS);
        if (GT_OK != status)
        {
            *errorMessagePtr = brgVlanEntryReadErrorMessage;
        }

        if ((GT_OK == status) && (GT_TRUE == isValid) &&
            CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr, portNum))
        {
            *vlanWasFoundPtr = GT_TRUE;
        }
    }

    if ((GT_OK == status) && (GT_TRUE == *vlanWasFoundPtr))
    {
        (*vlanIdPtr)--;
    }


    return status;
}


/*******************************************************************************
* wrlCpssDxChNextBrgPortVlanGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next port vlan.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       2; GT_OK and next port vlan id is pused to lua stack if no errors
*       occurs and such port exists
*       1; GT_OK is pused to lua stack if no errors occurs and such vlan is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextBrgPortVlanGet
(
    IN lua_State            *L
)
{
    GT_U8                                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_PORT_NUM                             portNum                 =
                                            (GT_PORT_NUM) lua_tonumber(L, 2);
                                                                  /* portNum */
    GT_U16                                  vlanId                  =
                                                    (GT_U16) lua_tonumber(L, 3);
                                                                  /* portNum */
    GT_BOOL                                 vlanWasFound            = GT_FALSE;
    CPSS_PORTS_BMP_STC                      portsMembers;
    CPSS_PORTS_BMP_STC                      portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC             vlanInfo;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC    portsTaggingCmd;
    GT_STATUS                               status                  = GT_OK;
    GT_CHAR_PTR                             error_message           = NULL;
    int                                     returned_param_count    = 0;
    P_CALLING_CHECKING;

    cpssOsMemSet(&portsMembers,     0, sizeof(portsMembers));
    cpssOsMemSet(&portsTagging,     0, sizeof(portsTagging));
    cpssOsMemSet(&vlanInfo,         0, sizeof(vlanInfo));
    cpssOsMemSet(&portsTaggingCmd,  0, sizeof(portsTaggingCmd));

    status = prvCpssDxChNextBrgPortVlanGet(devNum, portNum, &vlanId,
                                           &vlanWasFound, &portsMembers,
                                           &portsTagging, &vlanInfo,
                                           &portsTaggingCmd, &error_message
                                           P_CALLING_ACTUAL_DATA);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, vlanId, vlanWasFound);
    returned_param_count    +=
        prvCpssDxChVlanInfoToLuaOnConditionPush(status, L, devNum,
                                                &portsMembers, &portsTagging,
                                                &vlanInfo, &portsTaggingCmd,
                                                vlanWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChIsPortTaggingCmdSupported
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of port tagging supporting.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state (in stack should be device number)
*
* OUTPUTS:
*        isSupportedPtr     - port tagging supporting property
*        errorMessagePtrPtr - error message
*
* RETURNS:
         2 and supporting property pushed to lua stack
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS wrlCpssDxChIsPortTaggingCmdSupported
(
    IN lua_State                *L
)
{
    GT_U8                       devNum                          =
                                                    (GT_U8)  lua_tonumber(L, 1);
    GT_BOOL                     isPortTaggingCommandSSupported  = GT_FALSE;
    GT_STATUS                   status                          = GT_OK;
    GT_CHAR_PTR                 errorMessagePtr                 = NULL;
    int                         returned_param_count            = 0;

        status = prvCpssDxChIsPortTaggingCmdSupported
                 (devNum, &isPortTaggingCommandSSupported, &errorMessagePtr);

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, errorMessagePtr);
    returned_param_count    +=
        pvrCpssBooleanToLuaPush(status, L, isPortTaggingCommandSSupported);
    return returned_param_count;
}


int wrlCpssDxChVlanExists(
    lua_State *L
)
{
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC     vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_STATUS                       rc;
    P_CALLING_CHECKING;

    CPSS_API_CALL_LOCK();
    rc = cpssDxChBrgVlanEntryRead(
            (GT_U8)lua_tonumber(L, 1) /*devNum*/,
            (GT_U16)lua_tonumber(L, 2) /*vlanId*/,
            &portsMembersPtr,
            &portsTaggingPtr,
            &vlanInfoPtr,
            &isValidPtr,
            &portsTaggingCmd);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChBrgVlanEntryRead,
        PC_NUMBER(lua_tonumber(L, 1),           devNum,                  GT_U8,
        PC_NUMBER(lua_tonumber(L, 2),           vlanId,                 GT_U16,
        PC_STRUCT(&portsMembersPtr,             portsMembers,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&portsTaggingPtr,             portsTagging,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&vlanInfoPtr,                 vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
        PC_BOOL  (isValidPtr,                   isValid,
        PC_STRUCT(&portsTaggingCmd,             portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
        PC_LAST_PARAM))))))),
        rc);

    lua_pushboolean(L, (rc == GT_OK && isValidPtr != GT_FALSE) ? 1 : 0);
    return 1;
}

int wrlCpssDxChVlanList(
    lua_State *L
)
{
    CPSS_PORTS_BMP_STC              portsMembersPtr;
    CPSS_PORTS_BMP_STC              portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC     vlanInfoPtr;
    GT_BOOL                         isValidPtr = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    int                             cnt = 0;
    GT_U16                          vlanId;
    GT_STATUS                       rc;
    P_CALLING_CHECKING;

    lua_settop(L, 1);
    lua_newtable(L);
    for (vlanId = 0; vlanId < CPSS_DXCH_VLAN_MAX_ENTRY_CNS; vlanId++)
    {
        CPSS_API_CALL_LOCK();
        rc = cpssDxChBrgVlanEntryRead(
                (GT_U8)lua_tonumber(L, 1) /*devNum*/,
                vlanId,
                &portsMembersPtr,
                &portsTaggingPtr,
                &vlanInfoPtr,
                &isValidPtr,
                &portsTaggingCmd);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgVlanEntryRead,
            PC_NUMBER(lua_tonumber(L, 1),       devNum,                  GT_U8,
            PC_NUMBER(vlanId,                   vlanId,                 GT_U16,
            PC_STRUCT(&portsMembersPtr,         portsMembers,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&portsTaggingPtr,         portsTagging,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&vlanInfoPtr,             vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
            PC_BOOL  (isValidPtr,               isValid,
            PC_STRUCT(&portsTaggingCmd,         portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
            PC_LAST_PARAM))))))),
            rc);
        if (rc == GT_OK && isValidPtr != GT_FALSE)
        {
            lua_pushnumber(L, vlanId);
            lua_rawseti(L, 2, ++cnt);
        }
    }
    return 1;
}


/*******************************************************************************
* wrlCpssDxChGetVlanInfo
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Read vlan entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state (in stack should be device number and
                              trunk id)
*
* OUTPUTS:
*
* RETURNS:
*       1 and result pushed to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChGetVlanInfo
(
    lua_State *L
)
{
    GT_U8                                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U16                                  vlanId                  =
                                                    (GT_U16) lua_tonumber(L, 2);
                                                                    /* vlanId */
    CPSS_PORTS_BMP_STC                      portsMembers;
    CPSS_PORTS_BMP_STC                      portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC             vlanInfo;
    GT_BOOL                                 isValid = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_STATUS                               status;
    P_CALLING_CHECKING;

    CPSS_API_CALL_LOCK();
    status = cpssDxChBrgVlanEntryRead(devNum, vlanId, &portsMembers,
                                      &portsTagging, &vlanInfo, &isValid,
            &portsTaggingCmd);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChBrgVlanEntryRead,
        PC_NUMBER(devNum,                       devNum,                  GT_U8,
        PC_NUMBER(vlanId,                       vlanId,                 GT_U16,
        PC_STRUCT(&portsMembers,                portsMembers,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&portsTagging,                portsTagging,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&vlanInfo,                    vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
        PC_BOOL  (isValid,                      isValid,
        PC_STRUCT(&portsTaggingCmd,             portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
        PC_LAST_PARAM))))))),
        PC_STATUS);

    return prvCpssDxChVlanInfoToLuaOnConditionPush(status, L, devNum,
                                                   &portsMembers, &portsTagging,
                                                   &vlanInfo, &portsTaggingCmd,
                                                   isValid);
}


/*******************************************************************************
* wrlCpssDxChVlanEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Builds and writes vlan entry to HW.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1 and result pushed to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChVlanEntrySet
(
    IN lua_State                            *L
)
{
    CPSS_PORTS_BMP_STC               portsMembersPtr;
    CPSS_PORTS_BMP_STC               portsTaggingPtr;
    CPSS_DXCH_BRG_VLAN_INFO_STC      vlanInfoPtr;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd;
    GT_U32                           i; /* loop iterator */
    GT_STATUS                        result;
    GT_U8                            devNum = (GT_U8)lua_tonumber(L, 1);
    GT_U16                           vlanId = (GT_U16)lua_tonumber(L, 2);
    P_CALLING_CHECKING;

    lua_settop(L, 3);

    /* portsmembers */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembersPtr);
    lua_getfield(L, 3, "portsMembers");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        for (i = 1; i <= lua_objlen(L, -1); i++)
        {
            lua_rawgeti(L, -1, i);
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                int portNum = lua_tointeger(L, -1);
                if (portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembersPtr, portNum);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    /* portstagging */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTaggingPtr);
    lua_getfield(L, 3, "portsTagging");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        for (i = 1; i <= lua_objlen(L, -1); i++)
        {
            lua_rawgeti(L, -1, i);
            if (lua_type(L, -1) == LUA_TNUMBER)
            {
                int portNum = lua_tointeger(L, -1);
                if (portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts)
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&portsTaggingPtr, portNum);
                }
            }
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

    /* ports tagging commands */
    for (i = 0; i < CPSS_MAX_PORTS_NUM_CNS; i++)
    {
        portsTaggingCmd.portsCmd[i] =
            CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsTaggingPtr, i)
                                    ? CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E
                                    : CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }
    lua_getfield(L, 3, "portsTaggingCmd");
    if (lua_type(L, -1) == LUA_TTABLE)
    {
        int t = lua_gettop(L);
        lua_pushnil(L);

        while (0 != lua_next(L, t))
        {
            if ((LUA_TNUMBER == lua_type(L, -1)) &&
                (LUA_TNUMBER == lua_type(L, -2)))
            {
                int portNum = (GT_PORT_NUM) lua_tointeger(L, -2);
                CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT portTagingCmd =
                    (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT) lua_tointeger(L, -1);

                portsTaggingCmd.portsCmd[portNum] = portTagingCmd;
            }

            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);

#undef  F_BOOL
#define F_BOOL(field) \
    lua_getfield(L, 3, #field); \
    if (lua_type(L, -1) == LUA_TBOOLEAN) \
    { \
        vlanInfoPtr.field = lua_toboolean(L, -1) ? GT_TRUE : GT_FALSE; \
    } \
    else \
    { \
        vlanInfoPtr.field = GT_FALSE; \
    } \
    lua_pop(L, 1);

#undef  F_NUMBER
#define F_NUMBER(field, fieldtype) \
    lua_getfield(L, 3, #field); \
    if (lua_type(L, -1) == LUA_TNUMBER) \
    { \
        vlanInfoPtr.field = (fieldtype)lua_tointeger(L, -1); \
    } \
    else \
    { \
        vlanInfoPtr.field = (fieldtype)0; \
    } \
    lua_pop(L, 1);

#undef  F_ENUM
#define F_ENUM(field, fieldtype) \
    lua_getfield(L, 3, #field); \
    vlanInfoPtr.field = (fieldtype)prv_lua_to_c_enum(L, enumDescr_ ## fieldtype); \
    lua_pop(L, 1);

    F_BOOL(unkSrcAddrSecBreach);
    F_NUMBER(unregNonIpMcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv4McastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv6McastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unkUcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_NUMBER(unregNonIpv4BcastCmd, CPSS_PACKET_CMD_ENT);
    F_BOOL(ipv4IgmpToCpuEn);
    F_BOOL(mirrToRxAnalyzerEn);
    F_BOOL(ipv6IcmpToCpuEn);
    F_NUMBER(ipCtrlToCpuEn, CPSS_DXCH_BRG_IP_CTRL_TYPE_ENT);
    F_ENUM(ipv4IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_ENUM(ipv6IpmBrgMode, CPSS_BRG_IPM_MODE_ENT);
    F_BOOL(ipv4IpmBrgEn);
    F_BOOL(ipv6IpmBrgEn);
    F_NUMBER(ipv6SiteIdMode, CPSS_IP_SITE_ID_ENT);
    F_BOOL(ipv4UcastRouteEn);
    F_BOOL(ipv4McastRouteEn);
    F_BOOL(ipv6UcastRouteEn);
    F_BOOL(ipv6McastRouteEn);
    F_NUMBER(stgId, GT_U32);
    F_BOOL(autoLearnDisable);
    F_BOOL(naMsgToCpuEn);
    F_NUMBER(mruIdx, GT_U32);
    F_BOOL(bcastUdpTrapMirrEn);
    F_NUMBER(vrfId, GT_U32);
    F_NUMBER(floodVidx, GT_U16);
    F_NUMBER(floodVidxMode, CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_ENT);
    F_NUMBER(portIsolationMode, CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_CMD_ENT);
    F_BOOL(ucastLocalSwitchingEn);
    F_BOOL(mcastLocalSwitchingEn);
    F_NUMBER(mirrToRxAnalyzerIndex, GT_U32);
    F_BOOL(mirrToTxAnalyzerEn);
    F_NUMBER(mirrToTxAnalyzerIndex, GT_U32);
    F_NUMBER(fidValue, GT_U32);
    F_NUMBER(unknownMacSaCmd, CPSS_PACKET_CMD_ENT);
    F_BOOL(ipv4McBcMirrToAnalyzerEn);
    F_NUMBER(ipv4McBcMirrToAnalyzerIndex, GT_U32);
    F_BOOL(ipv6McMirrToAnalyzerEn);
    F_NUMBER(ipv6McMirrToAnalyzerIndex, GT_U32);

#undef  F_BOOL
#undef  F_NUMBER
    lua_getfield(L, 3, "vrfId");
    if (lua_type(L, -1) != LUA_TNUMBER)
    {
        vlanInfoPtr.vrfId = 0;
        vlanInfoPtr.floodVidx = 0xFFF;
        vlanInfoPtr.floodVidxMode = CPSS_DXCH_BRG_VLAN_FLOOD_VIDX_MODE_UNREG_MC_E;
        vlanInfoPtr.portIsolationMode = CPSS_DXCH_BRG_VLAN_PORT_ISOLATION_DISABLE_CMD_E;
        vlanInfoPtr.ucastLocalSwitchingEn = GT_FALSE;
        vlanInfoPtr.mcastLocalSwitchingEn = GT_FALSE;
    }
    lua_pop(L, 1);

    /* call cpss api function */
    result = cpssDxChBrgVlanEntryWrite(devNum,
                                       vlanId,
                                       &portsMembersPtr,
                                       &portsTaggingPtr,
                                       &vlanInfoPtr,
                                       &portsTaggingCmd);
    P_CALLING_API(
        cpssDxChBrgVlanEntryWrite,
        PC_NUMBER(devNum,                       devNum,                  GT_U8,
        PC_NUMBER(vlanId,                       vlanId,                 GT_U16,
        PC_STRUCT(&portsMembersPtr,             portsMembers,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&portsTaggingPtr,             portsTagging,
                                                            CPSS_PORTS_BMP_STC,
        PC_STRUCT(&vlanInfoPtr,                 vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
        PC_STRUCT(&portsTaggingCmd,             portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
        PC_LAST_PARAM)))))),
        result);

    lua_pushnumber(L, result);
    return 1;
}

int wrlCpssDxChVlanEntryInvalidate(
    lua_State *L
)
{
    GT_STATUS                       rc;
    GT_U8                           devNum = (GT_U8)lua_tonumber(L, 1);
    GT_U16                          vlanId = (GT_U16)lua_tonumber(L, 2);
    P_CALLING_CHECKING;

    rc = cpssDxChBrgVlanEntryInvalidate(devNum, vlanId);
    P_CALLING_API(
        cpssDxChBrgVlanEntryInvalidate,
        PC_NUMBER(devNum,                       devNum,                  GT_U8,
        PC_NUMBER(vlanId,                       vlanId,                 GT_U16,
        PC_LAST_PARAM)),
        rc);

    lua_pushnumber(L, rc);
    return 1;
}


/*******************************************************************************
* wrlCpssDxChValidVlansGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of valid vlans array.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*       number of returned to lua elements, status and vlans array to
*       lua stack, if no error occures, otherwise status and error message to
*       lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChValidVlansGet
(
    IN lua_State            *L
)
{
    GT_U8                               devNum  = (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U16                              vlanId  = (GT_U16) lua_tonumber(L, 2);
                                                                   /* vlanId */
    GT_U32                              maxVlanId;
    CPSS_PORTS_BMP_STC                  portsMembers;
    CPSS_PORTS_BMP_STC                  portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC         vlanInfo;
    GT_U32                              validVlanIndex          = 1;
    GT_BOOL                             isValid = GT_FALSE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
                                        portsTaggingCmd;
    int                                 luaTopElementIndex;
    GT_STATUS                           status                  = GT_OK;
    GT_CHAR_PTR                         error_message           = NULL;
    int                                 returned_param_count    = 1;
    P_CALLING_CHECKING;

    lua_newtable(L);
    luaTopElementIndex = lua_gettop(L);

    status = prvCpssDxChMaximumVlanIdGet(devNum, &maxVlanId, &error_message);

    while ((GT_OK == status) && (vlanId <= maxVlanId))
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgVlanEntryRead(devNum, vlanId++, &portsMembers,
                                          &portsTagging, &vlanInfo, &isValid,
                                          &portsTaggingCmd);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgVlanEntryRead,
            PC_NUMBER(devNum,                   devNum,                  GT_U8,
            PC_NUMBER(vlanId,                   vlanId,                 GT_U16,
            PC_STRUCT(&portsMembers,            portsMembers,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&portsTagging,            portsTagging,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&vlanInfo,                vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
            PC_BOOL  (isValid,                  isValid,
            PC_STRUCT(&portsTaggingCmd,         portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
            PC_LAST_PARAM))))))),
            PC_STATUS);
        if (GT_TRUE == isValid)
        {
            lua_pushnumber(L, vlanId - 1);
            lua_rawseti(L, luaTopElementIndex, validVlanIndex++);
        }
    }

    if (GT_OK != status)
    {
        error_message = vlanEntryReadErrorMessage;
        lua_remove(L, 1);
        returned_param_count--;
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaAtStackPositionPush(status, L, 1, error_message);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChVlanIdGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of vlan id which matches the pattern.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*
* OUTPUTS:
*       operation succed and vlan id if it was found
*       operation succed if vlan was not found
*       error code and error message pushed to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChVlanIdGet
(
    IN lua_State            *L
)
{
    GT_U8                               devNum                  = (GT_U8)
                                                            lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U16                              vlanId                  = (GT_U16)
                                                            lua_tonumber(L, 2);
                                                                   /* vlanId */
    CPSS_PORTS_BMP_STC                  portsMembersPattern;
                                                            /* portsMembers */
    CPSS_PORTS_BMP_STC                  portsTaggingPattern;
                                                            /* portsTagging */
    CPSS_DXCH_BRG_VLAN_INFO_STC         vlanInfoPattern;
                                                                /* vlanInfo */
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
                                        portsTaggingCmdPattern;
                                                        /* portsTaggingCmd */
    CPSS_PORTS_BMP_STC                  portsMembersMask;
    CPSS_PORTS_BMP_STC                  portsTaggingMask;
    CPSS_DXCH_BRG_VLAN_INFO_STC         vlanInfoMask;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
                                        portsTaggingCmdMask;
    GT_BOOL                             descendingSearch        = (GT_BOOL)
                                                        lua_toboolean(L, 7);
                                                        /* Descending search */
    GT_BOOL                             vlanWasFound            = GT_FALSE;
    GT_16                               vlanDecrementIncrement  = (GT_16)
                                            (GT_TRUE == descendingSearch? -1:0);
    GT_U32                              maxVlanId               = 0;
    CPSS_PORTS_BMP_STC                  searchedPortsMembers;
    CPSS_PORTS_BMP_STC                  searchedPortsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC         searchedVlanInfo;
    GT_BOOL                             searchedVlanValidness   = GT_TRUE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
                                        searchedPortsTaggingCmd;
    GT_STATUS                           status                  = GT_OK;
    GT_CHAR_PTR                         error_message           = NULL;
    int                                 returned_param_count    = 0;
    P_CALLING_CHECKING;

    cpssOsMemSet(&portsMembersPattern,      0, sizeof(portsMembersPattern));
    cpssOsMemSet(&portsTaggingPattern,      0, sizeof(portsTaggingPattern));
    cpssOsMemSet(&vlanInfoPattern,          0, sizeof(vlanInfoPattern));
    cpssOsMemSet(&portsTaggingCmdPattern,   0, sizeof(portsTaggingCmdPattern));

    cpssOsMemSet(&searchedPortsMembers,     0, sizeof(searchedPortsMembers));
    cpssOsMemSet(&searchedPortsTagging,     0, sizeof(searchedPortsTagging));
    cpssOsMemSet(&searchedVlanInfo,         0, sizeof(searchedVlanInfo));
    cpssOsMemSet(&searchedVlanValidness,    0, sizeof(searchedVlanValidness));

    status  = prvCpssDxChBrgPortsTagingDataAndVlanInfoFromLuaGet(
                  L, 3, &portsMembersPattern, &portsTaggingPattern,
                  &vlanInfoPattern, &portsTaggingCmdPattern, &error_message);

    if (GT_OK == status)
    {
        status  = prvCpssDxChBrgPortsTagingDataAndVlanInfoMasksFromLuaGet(
                      L, 3, &portsMembersMask, &portsTaggingMask,
                      &vlanInfoMask, &portsTaggingCmdMask, &error_message);
    }

    if (GT_OK == status)
    {
        status  = prvCpssDxChMaximumVlanIdGet(devNum, &maxVlanId,
                                              &error_message);
    }

    if ((GT_OK == status) &&
        (GT_TRUE == descendingSearch) && (vlanId > maxVlanId))
    {
        vlanId  = (GT_U16) maxVlanId;
    }

    while ((GT_OK == status) &&
           (0 < vlanId) && (vlanId <= maxVlanId) &&
           (GT_FALSE == vlanWasFound))
    {
        CPSS_API_CALL_LOCK();
        status  = cpssDxChBrgVlanEntryRead(devNum, vlanId,
                                           &searchedPortsMembers,
                                           &searchedPortsTagging,
                                           &searchedVlanInfo,
                                           &searchedVlanValidness,
                                           &searchedPortsTaggingCmd);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgVlanEntryRead,
            PC_NUMBER(devNum,                   devNum,                  GT_U8,
            PC_NUMBER(vlanId,                   vlanId,                 GT_U16,
            PC_STRUCT(&searchedPortsMembers,    portsMembers,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&searchedPortsTagging,    portsTagging,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&searchedVlanInfo,        vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
            PC_BOOL  (searchedVlanValidness,    isValid,
            PC_STRUCT(&searchedPortsTaggingCmd, portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
            PC_LAST_PARAM))))))),
            PC_STATUS);

        vlanWasFound = searchedVlanValidness;

        if ((GT_OK == status) && (GT_TRUE == vlanWasFound))
        {
            vlanWasFound =
                prv_lua_compare_mask(&searchedPortsMembers,
                                     &portsMembersPattern, &portsMembersMask,
                                     sizeof(searchedPortsMembers))?
                GT_TRUE:GT_FALSE;
        }

        if ((GT_OK == status) && (GT_TRUE == vlanWasFound))
        {
            vlanWasFound =
                prv_lua_compare_mask(&searchedPortsTagging,
                                     &portsTaggingPattern, &portsTaggingMask,
                                     sizeof(searchedPortsTagging))?
                GT_TRUE:GT_FALSE;
        }

        if ((GT_OK == status) && (GT_TRUE == vlanWasFound))
        {
            vlanWasFound =
                prv_lua_compare_mask(&searchedVlanInfo, &vlanInfoPattern,
                                     &vlanInfoMask,
                                     sizeof(searchedVlanInfo))?
                GT_TRUE:GT_FALSE;
        }

        if ((GT_OK == status) && (GT_TRUE == vlanWasFound))
        {
            vlanWasFound =
                prv_lua_compare_mask(&searchedPortsTaggingCmd,
                                     &portsTaggingCmdPattern,
                                     &portsTaggingCmdMask,
                                     sizeof(searchedPortsTaggingCmd))?
                GT_TRUE:GT_FALSE;
        }

        vlanId  = (GT_U16) (vlanId + vlanDecrementIncrement);
    }

    if ((GT_OK == status) && (GT_TRUE == vlanWasFound))
    {
        vlanId  = (GT_U16) (vlanId - vlanDecrementIncrement);
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(status, L, vlanId, vlanWasFound);
    returned_param_count    +=
        pvrCpssPortBitmatToLuaOnConditionPush(status, L, &searchedPortsMembers,
                                              vlanWasFound);
    returned_param_count    +=
        pvrCpssPortBitmatToLuaOnConditionPush(status, L, &searchedPortsTagging,
                                              vlanWasFound);
    returned_param_count    +=
        prvCpssDxChBrgVlanInfoToLuaOnConditionPush(status, L, &searchedVlanInfo,
                                                   vlanWasFound);
    returned_param_count    +=
        prvCpssDxChBrgVlanPortsTagingCommandsToLuaOnConditionPush(
            status, L, &searchedPortsTaggingCmd, vlanWasFound);
    return returned_param_count;
}


/*******************************************************************************
* wrlCpssDxChNextUnusedMRUIndexGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next unused MRU-index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L[0]                - device number
*       L[2]                - starting MRU-index
*
* OUTPUTS:
*       GT_OK and next unsed MRU-index, if exists, pushed to lua stack
*       GT_OK, if unsed MRU-index does not exist, pushed to lua stack
*       error code and error message pushed to lua stack
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextUnusedMRUIndexGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                  mruIndex                =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                /* mruIndex */
    GT_U32                  maxVlanId;
    GT_U16                  vlanId                  = 0;
    GT_U32                  allMRUIndexes[WRL_PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS];
    CPSS_PORTS_BMP_STC      portsMembers;
    CPSS_PORTS_BMP_STC      portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC
                            vlanInfo;
    GT_BOOL                 isValid                 = GT_TRUE;
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC
                            portsTaggingCmd;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;
#ifdef CPSS_LOG_ENABLE
    GT_BOOL                 orig_cpssLogEnable = cpssLogEnableGet();
#endif /*CPSS_LOG_ENABLE*/
    P_CALLING_CHECKING;

    cpssOsMemSet(&allMRUIndexes, 0, sizeof(allMRUIndexes));

    status = prvCpssDxChMaximumVlanIdGet(devNum, &maxVlanId, &error_message);

#ifdef CPSS_LOG_ENABLE
    if(orig_cpssLogEnable == GT_TRUE)
    {
        /* disable the log */
        cpssLogEnableSet(GT_FALSE);
    }
#endif /*CPSS_LOG_ENABLE*/

    while ((GT_OK == status) && (vlanId <= maxVlanId))
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgVlanEntryRead(devNum, vlanId, &portsMembers,
                                          &portsTagging, &vlanInfo, &isValid,
                                          &portsTaggingCmd);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgVlanEntryRead,
            PC_NUMBER(devNum,                   devNum,                  GT_U8,
            PC_NUMBER(vlanId,                   vlanId,                 GT_U16,
            PC_STRUCT(&portsMembers,            portsMembers,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&portsTagging,            portsTagging,
                                                            CPSS_PORTS_BMP_STC,
            PC_STRUCT(&vlanInfo,                vlanInfo,
                                                   CPSS_DXCH_BRG_VLAN_INFO_STC,
            PC_BOOL  (isValid,                  isValid,
            PC_STRUCT(&portsTaggingCmd,         portsTaggingCmd,
                                          CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC,
            PC_LAST_PARAM))))))),
            PC_STATUS);
        if (GT_TRUE == isValid)
        {
            allMRUIndexes[vlanInfo.mruIdx]++;
        }

        vlanId++;
    }

#ifdef CPSS_LOG_ENABLE
    if(orig_cpssLogEnable == GT_TRUE)
    {
        /* enable the log */
        cpssLogEnableSet(GT_TRUE);
    }
#endif /*CPSS_LOG_ENABLE*/

    while (mruIndex < WRL_PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS)
    {
        if (0 == allMRUIndexes[mruIndex])
        {
            break;
        }
        else
        {
            mruIndex++;
        }
    }

    returned_param_count    +=
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=
        pvrCpssNumberToLuaOnConditionPush(
            status, L, mruIndex,
            mruIndex < WRL_PRV_CPSS_DXCH_BRG_MRU_INDEX_MAX_CNS);
    return returned_param_count;
}
