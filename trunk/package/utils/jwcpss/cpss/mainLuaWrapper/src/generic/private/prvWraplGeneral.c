/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvWraplGeneral.c
*
* DESCRIPTION:
*       A general private functions of lua wrappers.
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*******************************************************************************/


#include <cpss/generic/cpssTypes.h>
#include <cpssCommon/wraplCpssTypes.h>
#include <cpssCommon/wraplCpssExtras.h>
#include <generic/private/prvWraplGeneral.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

/*******************************************************************************
*   lua wrappers constants                                                     *
*******************************************************************************/
const GT_ETHERADDR  zeroMacAddr = {{0,0,0,0,0,0}};

/*******************************************************************************
*   lua wrappers errors                                                  *
*******************************************************************************/
GT_CHAR wrongIpProtocolDefinitionErrorMessage[]     =
                                                    "Wrong ip prtocol defined";


/***** declarations ********/

use_prv_enum(CPSS_PORT_INTERFACE_MODE_ENT)
use_prv_enum(CPSS_PORT_SPEED_ENT)
use_prv_enum(CPSS_PORT_FLOW_CONTROL_ENT);
use_prv_enum(CPSS_IP_PROTOCOL_STACK_ENT);
use_prv_enum(CPSS_FDB_ACTION_MODE_ENT);
use_prv_struct(GT_ETHERADDR);
use_prv_struct(CPSS_PORTS_BMP_STC);
use_prv_struct(WRAP_DEV_PORT_STC);
use_prv_struct(GT_IPADDR);
use_prv_struct(GT_IPV6ADDR);

/***** declarations ********/

/*******************************************************************************
* prvCpssIsZeroMacAddress
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Compares mac address with 00:00:00:00:00:00.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       macAddrPtr            - entry mac address
*
* OUTPUTS:
*       isZeroPtr             - GT_TRUE, if mac address is equal to 
*                               00:00:00:00:00:00, otherwise GT_FALSE
*       errorMessagePtr       - error message
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssIsZeroMacAddress
(   
    IN  GT_ETHERADDR            *macAddrPtr,
    OUT GT_BOOL                 *isZeroPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_U16                      macAddressIndex;
    GT_STATUS                   status      = GT_OK;
    *errorMessagePtr = NULL;
    
    *isZeroPtr = GT_TRUE;
    
    for(macAddressIndex = 0; 
        (GT_TRUE == *isZeroPtr) && (macAddressIndex < 6); 
        macAddressIndex++)
    {
        if (0x00 != macAddrPtr->arEther[macAddressIndex])
        {
            *isZeroPtr = GT_FALSE;
        }
    }
    
    return status;
}


/*******************************************************************************
* prvCpssIsSameMacAddresses
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Compares two mac addresses.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       firstMacAddrPtr       - first compared mac address
*       secondMacAddrPtr      - second compared mac address
*
* OUTPUTS:
*       areSamePtr            - GT_TRUE, if mac addresses are equal, 
*                               otherwise GT_FALSE
*       errorMessagePtr       - error message
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssIsSameMacAddresses
(   
    IN  GT_ETHERADDR            *firstMacAddrPtr,
    IN  GT_ETHERADDR            *secondMacAddrPtr,
    OUT GT_BOOL                 *areSamePtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_U16                      macAddressIndex;
    GT_STATUS                   status      = GT_OK;
    *errorMessagePtr = NULL;
    
    *areSamePtr = GT_TRUE;
    
    for(macAddressIndex = 0; 
        (GT_TRUE == *areSamePtr) && (macAddressIndex < 6); 
        macAddressIndex++)
    {
        if (firstMacAddrPtr->arEther[macAddressIndex] != 
            secondMacAddrPtr->arEther[macAddressIndex])
        {
            *areSamePtr = GT_FALSE;
        }
    }
    
    return status;
}


/*******************************************************************************
* prvWrlCpssNothingToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Nop-function for data to lua pushing. This function is needed for 
*       vxWorks code optimization turning-off.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       number of pushed values: 0
*
* COMMENTS:
*
*******************************************************************************/
int prvWrlCpssNothingToLuaPush
(   
)
{
    return 0;
}


/*******************************************************************************
* prvWrlCpssStatusToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Pushing of status and error_message (if relevant) to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       error_message       - possible error message, relevant 
*                                   if status != GT_OK
*
*
* OUTPUTS:
*
* RETURNS:
*       number of pushed values: 1 or 2 if error_message is relevat
*
* COMMENTS:
*
*******************************************************************************/
int prvWrlCpssStatusToLuaPush
(   
    IN GT_STATUS                status,
    IN lua_State                *L, 
    IN GT_CHAR_PTR              error_message
)
{
    if (GT_OK == status)
    {
        lua_pushinteger(L, status);
        return 1;
    }
    else
    {
        lua_pushinteger(L, status);
        lua_pushstring(L, error_message);
        return 2;    
    }
}


/*******************************************************************************
* prvWrlCpssStatusToLuaAtStackPositionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Pushing of status and error_message (if relevant) to lua stack with 
*       forcibly setting of stack position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       stack_position      - position in lua stack
*       error_message       - possible error message, relevant 
*                                   if status != GT_OK
*
*
* OUTPUTS:
*
* RETURNS:
*       number of pushed values: 1 or 2 if error_message is relevat
*
* COMMENTS:
*
*******************************************************************************/
int prvWrlCpssStatusToLuaAtStackPositionPush
(   
    IN GT_STATUS                status,
    IN lua_State                *L, 
    IN int                      stack_position,
    IN GT_CHAR_PTR              error_message
)
{
    if (GT_OK == status)
    {
        lua_pushinteger(L, status);
        lua_insert(L, stack_position);
        return 1;
    }
    else
    {
        lua_pushinteger(L, status);
        lua_insert(L, stack_position);
        lua_pushstring(L, error_message);
        lua_insert(L, stack_position + 1);
        return 2;    
    }
}


/*******************************************************************************
* pvrCpssBooleanFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of getting boolean value from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
* 
* OUTPUTS:
*       isValueTaken        - property, that bool value was in stack
*       value               - taken value
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssBooleanFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_BOOL                 *isValueTaken,
    OUT GT_BOOL                 *value,
    OUT GT_CHAR_PTR             *error_message
)
{   
    GT_STATUS                   status          = GT_OK; 
    error_message = NULL;
    
    if (0 == lua_isnil(L, L_index))
    { 
        if (lua_isboolean(L, L_index))
        {
            lua_pushvalue(L, L_index);
            *value = (GT_BOOL) lua_toboolean(L, -1);
            lua_pop(L, 1);        
            *isValueTaken = GT_TRUE;
        }
        else
        {
            *isValueTaken = GT_FALSE;
        }
    } 
    else
    {
        *isValueTaken = GT_FALSE;
    }
    
    return status;
}


/*******************************************************************************
* pvrCpssBooleanToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing boolean value to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       pushed_bool         - pushed boolean value to lua stack 
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssBooleanToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_BOOL                  pushed_bool
)
{   
    if (GT_OK == status)
    {
        lua_pushboolean(L, pushed_bool);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssBooleanToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing boolean value to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       pushed_bool         - pushed boolean value to lua stack 
*       condition           - pushing condition
* 
* OUTPUTS:
*
* RETURNS:
*       1 if boolean value was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssBooleanToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_BOOL                  pushed_bool,
    IN GT_BOOL                  condition
)
{   
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        lua_pushboolean(L, pushed_bool);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssNumberFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of getting number value from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
* 
* OUTPUTS:
*       isValueTaken        - property, that bool value was in stack
*       value               - taken value
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssNumberFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_BOOL                 *isValueTaken,
    OUT GT_U32                  *value,
    OUT GT_CHAR_PTR             *error_message
)
{   
    GT_STATUS                   status          = GT_OK; 
    error_message = NULL;
    
    if (0 == lua_isnil(L, L_index))
    { 
        if (lua_isnumber(L, L_index))
        {
            lua_pushvalue(L, L_index);
            *value = (GT_U32) lua_tonumber(L, -1);
            lua_pop(L, 1);        
            *isValueTaken = GT_TRUE;
        }
        else
        {
            *isValueTaken = GT_FALSE;
        }
    } 
    else
    {
        *isValueTaken = GT_FALSE;
    }
    
    return status;
}


/*******************************************************************************
* pvrCpssNumberToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing integer number to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       pushed_number       - pushed number to lua stack number
* 
* OUTPUTS:
*
* RETURNS:
*       1 if data was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssNumberToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_U32                   pushed_number
)
{   
    if (GT_OK == status)
    {
        lua_pushinteger(L, pushed_number);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssNumberToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing of integer number to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       pushed_number       - pushed number to lua stack number
*       condition           - pushing condition
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssNumberToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_U32                   pushed_number,
    IN GT_BOOL                  condition
)
{   
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        lua_pushinteger(L, pushed_number);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssAPIResultCodeStringFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get's API result code string from lua API result code array.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                 - lua state
*       resultCode        - API result code
* 
* OUTPUTS:
*       resultCodeString  - API result code string
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssAPIResultCodeStringFromLuaGet
(
    IN lua_State            *L,
    IN GT_U32               resultCode,
    OUT GT_CHAR_PTR         *resultCodeString
)
{
    size_t                  slen            = 0;
    GT_STATUS               status          = GT_OK; 
    
    *resultCodeString   = NULL;
    
    lua_getglobal(L, LUA_ENVIRONMENT_API_RETURN_CODE_ARRAY);
    
    if(lua_istable(L, -1))
    {
        lua_rawgeti(L, -1, resultCode); 
        if(lua_isstring(L, -1))
        {
            *resultCodeString = (GT_CHAR_PTR) lua_tolstring(L, -1, &slen);
        }
        lua_pop(L, 1); 
    }
    lua_pop(L, 1);    
  
    return status;
}


/*******************************************************************************
* pvrCpssStringToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing C zero terminated string to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       pushed_string       - pushed to lua stack string
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssStringToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_CHAR_PTR              pushed_string
)
{   
    if ((GT_OK == status) && (NULL != pushed_string))
    {
        lua_pushstring(L, pushed_string);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssStringToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing C zero terminated string to lua 
*       stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status              - caller status
*       L                   - lua state
*       pushed_string       - pushed to lua stack string
*       condition           - pushing condition
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssStringToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_CHAR_PTR              pushed_string,
    IN GT_BOOL                  condition
)
{   
    if ((GT_OK == status) && (NULL != pushed_string) && (GT_TRUE == condition))
    {
        lua_pushstring(L, pushed_string);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssPortInterfaceModeFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port interface mode getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                        - lua state
*        L_index                  - entry lua stack index
* 
* OUTPUTS:
*       portInterfaceModePtr      - port interface mode
*       errorMessagePtr           - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortInterfaceModeFromLuaGet
(
    IN  lua_State                       *L,
    IN  GT_32                           L_index,
    OUT CPSS_PORT_INTERFACE_MODE_ENT    *portInterfaceModePtr,
    OUT GT_CHAR_PTR                     *errorMessagePtr
)
{
    GT_STATUS                           status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    *portInterfaceModePtr = 
        (CPSS_PORT_INTERFACE_MODE_ENT) 
            prv_lua_to_c_enum(
                L, 
                enumDescr_CPSS_PORT_INTERFACE_MODE_ENT);
    
    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssPortSpeedFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port speed getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                        - lua state
*        L_index                  - entry lua stack index
* 
* OUTPUTS:
*       portSpeedPtr              - port speed
*       errorMessagePtr           - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortSpeedFromLuaGet
(
    IN  lua_State                       *L,
    IN  GT_32                           L_index,
    OUT CPSS_PORT_SPEED_ENT             *portSpeedPtr,
    OUT GT_CHAR_PTR                     *errorMessagePtr
)
{
    GT_STATUS                           status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    *portSpeedPtr = (CPSS_PORT_SPEED_ENT) prv_lua_to_c_enum(
                                              L, 
                                              enumDescr_CPSS_PORT_SPEED_ENT);
    
    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssPortFlowControlFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port flow control getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                        - lua state
*        L_index                  - entry lua stack index
* 
* OUTPUTS:
*       portFlowControlPtr        - port flow control
*       errorMessagePtr           - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortFlowControlFromLuaGet
(
    IN  lua_State                   *L,
    IN  GT_32                       L_index,
    OUT CPSS_PORT_FLOW_CONTROL_ENT  *portFlowControlPtr,
    OUT GT_CHAR_PTR                 *errorMessagePtr
)
{
    GT_STATUS                       status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    *portFlowControlPtr = 
        (CPSS_PORT_FLOW_CONTROL_ENT) prv_lua_to_c_enum(
                                         L, 
                                         enumDescr_CPSS_PORT_FLOW_CONTROL_ENT);
    
    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssDevicePortStructToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of WRAP_DEV_PORT_STC struct pushing to lua stack.
*       WRAP_DEV_PORT_STC - struct which contains dev/port pair.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
* 
* OUTPUTS:
*       status              - caller status
*       portsMemberPtr      - ports members
*
* RETURNS:
*       1 if WRAP_DEV_PORT_STC was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssDevicePortStructToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN WRAP_DEV_PORT_STC        *devPortStructPtr
)
{ 
    if (GT_OK == status)
    {
        prv_c_to_lua_WRAP_DEV_PORT_STC(L, devPortStructPtr);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssPortsMembersFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port members array getting from lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*       L_index             - entry lua stack index
* 
* OUTPUTS:
*       portsMemberPtr      - ports members
*       errorMessagePtr     - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortsMembersFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT CPSS_PORTS_BMP_STC      *portsMemberPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{ 
    GT_PORT_NUM                 portNum         = 0;
    GT_STATUS                   status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(portsMemberPtr);
    
    if (0 != lua_istable(L, L_index))
    {           
        lua_pushnil(L);
        
        while (0 != lua_next(L, L_index))
        {       
            portNum = (GT_PORT_NUM) lua_tointeger(L, -1);
                   
            if (portNum < CPSS_MAX_PORTS_NUM_CNS)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(portsMemberPtr, portNum);
            }
            lua_pop(L, 1);
        }
    }

    
    return status;
}


/*******************************************************************************
* pvrCpssPortsMembersMaskFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port members array comparision mask getting from lua 
*       stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                   - lua state
*       L_index             - entry lua stack index
* 
* OUTPUTS:
*       portsMemberPtr      - ports members
*       errorMessagePtr     - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortsMembersMaskFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT CPSS_PORTS_BMP_STC      *portsMemberPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{ 
    GT_PORT_NUM                 portNum         = 0;
    GT_STATUS                   status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    cpssOsMemSet(portsMemberPtr, 0xff, sizeof(*portsMemberPtr));
    
    if (0 != lua_istable(L, L_index))
    {           
        lua_pushnil(L);
        
        while (0 != lua_next(L, L_index))
        {       
            portNum = (GT_PORT_NUM) lua_tointeger(L, -1);
                   
            if (portNum < CPSS_MAX_PORTS_NUM_CNS)
            {
                CPSS_PORTS_BMP_MASK_PORT_SET_MAC(portsMemberPtr, portNum);
            }
               
            lua_pop(L, 1);
        }
        
        lua_pop(L, 1);
    }

    
    return status;
}


/*******************************************************************************
* pvrCpssPortsMembersToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing of port members array to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
* 
* OUTPUTS:
*       status              - caller status
*       portsMemberPtr      - ports members
*
* RETURNS:
*       1 if data pushed, otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortsMembersToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_PORTS_BMP_STC       *portsMemberPtr
)
{ 
    int                         index           = 1;
    GT_PORT_NUM                 portNum         = 0;    
    
    if (GT_OK == status)
    {
        int t;       
        lua_newtable(L);
        t = lua_gettop(L);
        
        for (portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
        {
            if (CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMemberPtr, portNum))
            {
                lua_pushnumber(L, portNum);
                lua_rawseti(L, -2, index++);
            }
        }        
        
        lua_settop(L, t);
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssPortBitmatFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port bitmap getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
* 
* OUTPUTS:
*       portsBmpPtr         - port bitmat
*       errorMessagePtr     - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortBitmatFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT CPSS_PORTS_BMP_STC      *portsBmpPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{ 
    GT_STATUS                   status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);    
    prv_lua_to_c_CPSS_PORTS_BMP_STC(L, portsBmpPtr);
    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssPortBitmatMaskFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port bitmap comparision mask getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
* 
* OUTPUTS:
*       portsBmpPtr         - port bitmat
*       errorMessagePtr     - error message
*
* RETURNS:
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortBitmatMaskFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT CPSS_PORTS_BMP_STC      *portsBmpPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{ 
    GT_STATUS                   status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index); 
    prv_lua_to_c_mask_CPSS_PORTS_BMP_STC(L, portsBmpPtr);    
    lua_pop(L, 1);    
    
    return status;
}


/*******************************************************************************
* pvrCpssPortBitmatToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing value of type CPSS_PORTS_BMP_STC to lua stack.
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
*       portsBmpPtr           - port bitmat
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssPortBitmatToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_PORTS_BMP_STC       *portsBmpPtr
)
{ 
    if (GT_OK == status)
    {
        prv_c_to_lua_CPSS_PORTS_BMP_STC(L, portsBmpPtr); 
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssPortBitmatToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing value of type CPSS_PORTS_BMP_STC to 
*       lua stack.
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
*       portsBmpPtr           - port bitmat
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
int pvrCpssPortBitmatToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN CPSS_PORTS_BMP_STC       *portsBmpPtr,
    IN GT_BOOL                  condition
)
{ 
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        prv_c_to_lua_CPSS_PORTS_BMP_STC(L, portsBmpPtr); 
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssMacAddrFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of mac address getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                  - lua state
*        L_index            - entry lua stack index
* 
* OUTPUTS:
*       macAddrPtr          - mac address
*       errorMessagePtr     - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssMacAddrFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_ETHERADDR            *macAddrPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_STATUS                   status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_settop(L, L_index);
    prv_lua_to_c_GT_ETHERADDR(L, macAddrPtr);
    
    return status;
}


/*******************************************************************************
* pvrCpssMacAddrToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing of mac address to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status                - caller status
*       L                     - lua state
*       macAddrPtr            - entry mac address
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
int pvrCpssMacAddrToLuaOnConditionPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_ETHERADDR             *macAddrPtr,
    IN GT_BOOL                  condition
)
{   
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        prv_c_to_lua_GT_ETHERADDR(L, macAddrPtr); 
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssIpProtocolFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of ip protocol getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                        - lua state
*        L_index                  - entry lua stack index
* 
* OUTPUTS:
*       ipProtocolStackPtr        - ip protocol
*       errorMessagePtr           - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssIpProtocolFromLuaGet
(
    IN  lua_State                   *L,
    IN  GT_32                       L_index,
    OUT CPSS_IP_PROTOCOL_STACK_ENT  *ipProtocolStackPtr,
    OUT GT_CHAR_PTR                 *errorMessagePtr
)
{
    GT_STATUS                       status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    *ipProtocolStackPtr = 
        (CPSS_IP_PROTOCOL_STACK_ENT) prv_lua_to_c_enum(
                                         L, 
                                         enumDescr_CPSS_IP_PROTOCOL_STACK_ENT);
    
    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssFdbActionModeFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of fdb action mode getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                        - lua state
*        L_index                  - entry lua stack index
* 
* OUTPUTS:
*       fdbActionModePtr          - fdb action mode
*       errorMessagePtr           - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssFdbActionModeFromLuaGet
(
    IN  lua_State                   *L,
    IN  GT_32                       L_index,
    OUT CPSS_FDB_ACTION_MODE_ENT    *fdbActionModePtr,
    OUT GT_CHAR_PTR                 *errorMessagePtr
)
{
    GT_STATUS                       status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    *fdbActionModePtr = 
        (CPSS_FDB_ACTION_MODE_ENT) prv_lua_to_c_enum(
                                       L, 
                                       enumDescr_CPSS_FDB_ACTION_MODE_ENT);
    
    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssIpV6AddrToStringConvert
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of ip v6 address to string conversion.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       ipAddress                 - ip address
* 
* OUTPUTS:
*       ipAddressStringPtr        - result string
*       errorMessagePtr           - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssIpV6AddrToStringConvert
(
    IN  const GT_IPV6ADDR           *ipAddress,
    OUT GT_CHAR_PTR                 ipAddressStringPtr,
    OUT GT_CHAR_PTR                 *errorMessagePtr
)
{
    GT_U16                          octet_index;
    GT_U16                          firstFullZeroSerieLength    = 0;
    GT_BOOL                         wasFirstFullZeroSerie       = GT_FALSE;
    GT_STATUS                       status                      = GT_OK;     
       
    cpssOsStrCpy(ipAddressStringPtr, "");
    *errorMessagePtr    = NULL;
    
    /* convert to string: 2190:0190:0000:0000:0000:0000:0000:0000 */    
    for (octet_index = 0; octet_index < 14; octet_index+=2)
    {
        if ((0 == ipAddress->arIP[octet_index]) && 
            (0 == ipAddress->arIP[octet_index + 1]))
        {
            if ((0 != octet_index) && (GT_FALSE == wasFirstFullZeroSerie))
            {
                switch (firstFullZeroSerieLength)
                {
                    case 0:
                        cpssOsStrCat(ipAddressStringPtr, "0:");
                        break;
                        
                    case 1:
                    cpssOsStrCat(ipAddressStringPtr, ":");
                        break;
                        
                    default:
                        break;                        
                }
                firstFullZeroSerieLength++;
            }
            else
            {
                cpssOsStrCat(ipAddressStringPtr, "0:");
                firstFullZeroSerieLength++;
            }
        }
        else 
        {
            if (0 == ipAddress->arIP[octet_index])
            {
                cpssOsSprintf(ipAddressStringPtr, "%s%x:", ipAddressStringPtr, 
                        ipAddress->arIP[octet_index + 1]);
            }
            else
            {
                cpssOsSprintf(ipAddressStringPtr, "%s%x%02x:", ipAddressStringPtr, 
                        ipAddress->arIP[octet_index], 
                        ipAddress->arIP[octet_index + 1]);
            }
            
            if (0 < firstFullZeroSerieLength)
            {
                if (1 < firstFullZeroSerieLength)
            {
                wasFirstFullZeroSerie   = GT_TRUE;
            }
                firstFullZeroSerieLength = 0;
            }
        }
    }
    
    octet_index = 14;
    if ((0 == ipAddress->arIP[octet_index]) && 
        (0 == ipAddress->arIP[octet_index + 1]))    
    {
        cpssOsStrCat(ipAddressStringPtr, "0");
    }
    else if (0 == ipAddress->arIP[octet_index])
    {
        cpssOsSprintf(ipAddressStringPtr, "%s%x", ipAddressStringPtr, 
                ipAddress->arIP[octet_index + 1]);
    }
    else
    {
        cpssOsSprintf(ipAddressStringPtr, "%s%x%02x", ipAddressStringPtr, 
                ipAddress->arIP[octet_index], ipAddress->arIP[octet_index + 1]);
    }
    
    return status;
}


/*******************************************************************************
* pvrCpssIpAddrFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of ip v4 or v6 address getting from lua stack.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       L                         - lua state
*       L_index                   - entry lua stack index
*       ipProtocolStack           - ip protocol type
* 
* OUTPUTS:
*       ipAddrPtr                 - ip v4 or v6 address
*       errorMessagePtr           - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssIpAddrFromLuaGet
(
    IN  lua_State                   *L,
    IN  GT_32                       L_index,
    IN  CPSS_IP_PROTOCOL_STACK_ENT  ipProtocolStack,
    OUT GT_IP_ADDR_TYPE_UNT         *ipAddrPtr,
    OUT GT_CHAR_PTR                 *errorMessagePtr
)
{
    GT_STATUS                   status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    switch (ipProtocolStack)
    {
    case CPSS_IP_PROTOCOL_IPV4_E:
        prv_lua_to_c_GT_IPADDR(L, &(ipAddrPtr->ipv4Addr));
        break;
        
    case CPSS_IP_PROTOCOL_IPV6_E:
        prv_lua_to_c_GT_IPV6ADDR(L, &(ipAddrPtr->ipv6Addr));
        break; 

    default:
        status              = GT_BAD_PARAM;
        *errorMessagePtr    = wrongIpProtocolDefinitionErrorMessage;
        break;
    }   

    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssIpAddrToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing of ip v4 or v6 address to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status                    - caller status
*       L                         - lua state
*       ipProtocolStack           - ip protocol type
*       ipAddrPtr                 - ip v4 address
*       condition                 - pushing condition
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssIpAddrToLuaOnConditionPush
(
    IN GT_STATUS                    status,
    IN lua_State                    *L,
    IN CPSS_IP_PROTOCOL_STACK_ENT   ipProtocolStack,
    IN GT_IP_ADDR_TYPE_UNT          *ipAddrPtr,
    IN GT_BOOL                      condition
)
{   
    if ((GT_OK == status) && (GT_TRUE == condition))
    {
        switch (ipProtocolStack)
        {
        case CPSS_IP_PROTOCOL_IPV4_E:
            prv_c_to_lua_GT_IPADDR(L, &(ipAddrPtr->ipv4Addr)); 
            break;
            
        case CPSS_IP_PROTOCOL_IPV6_E:
            prv_c_to_lua_GT_IPV6ADDR(L, &(ipAddrPtr->ipv6Addr));
            break; 

        default:
            return 0;
        }  
    
        return 1;        
    }
    else
    {
        return 0;    
    }
}

