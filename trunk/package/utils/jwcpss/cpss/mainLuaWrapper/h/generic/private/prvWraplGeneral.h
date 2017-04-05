/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvWraplGeneral.h
*
* DESCRIPTION:
*       A general private functions of lua wrappers.
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*******************************************************************************/


#ifndef __prvWraplGeneral_h__
#define __prvWraplGeneral_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 


#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <lua.h>


/* Enable debug info printing in lua wrappers                   */
#define LUA_DEBUG_PRINT


/***** declarations ********/

typedef struct
{
    GT_HW_DEV_NUM   hwDevNum;
    GT_PORT_NUM     portNum;
} WRAP_DEV_PORT_STC; 
    
/***** declarations ********/


/*******************************************************************************
*   lua wrappers constants                                                     *
*******************************************************************************/
extern const GT_ETHERADDR  zeroMacAddr;


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
);


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
);


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
);


/*******************************************************************************
* prvWrlCpssStatusToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Pushing of status and error_message (if relevant) ti lua stack.
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
);


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
);


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
);


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
*       pushed_bool         - pushed boolean value to lua stack number
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
);


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
);


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
);


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
*       pushed_number       - pushed to lua stack number
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
);


/*******************************************************************************
* pvrCpssNumberToLuaOnConditionPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of conditional pushing integer number to lua stack.
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
);


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
);


/*******************************************************************************
* pvrCpssStringToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of pushing C xero terminated string to lua stack.
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
);


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
);


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
);


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
);


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
);


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
);


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
);


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
*       GT_OK, if success otherwise error code and message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssPortsMembersToLuaPush
(
    IN  GT_STATUS               status,
    IN  lua_State               *L,
    OUT CPSS_PORTS_BMP_STC      *portsMemberPtr
);


/*******************************************************************************
* pvrCpssPortsMembersFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port members array from lua stack.
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
);


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
);


/*******************************************************************************
* wrlCpssDxChBrgMcEntryWrite
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Builds and writes bridge mac-entry to HW.
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
int wrlCpssDxChBrgMcEntryWrite
(
    IN lua_State            *L
);


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
);


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
);


/*******************************************************************************
* pvrCpssPortBitmatToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of port bitmap getting from lua stack.
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
);


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
);


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
);


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
);


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
);


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
);


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
);


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
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvWraplGeneral_h__ */
