/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChTrunk.c
*
* DESCRIPTION:
*       A lua wrapper for trunk functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssTypes.h>
#include <generic/private/prvWraplGeneral.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpssCommon/wraplCpssExtras.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <cpssCommon/wraplCpssTrunk.h>
#include <dxCh/wraplCpssDxChTrunk.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

/*******************************************************************************
*   Trunk wrapper warnings                                                     *
*******************************************************************************/  
GT_CHAR portOccurrenceInOthersTrunksWarningMessage[]            =
                                            "Port occures in port-channel %d";
GT_CHAR xcatDeviceAndTrunkIDOfWATrafficToCPUWarningMessage[]    =
                                                "Trunk WA CPU traffic problem";    
GT_CHAR maximumTrunkMembersCountExceedingWarningMessage[]       = 
                                    "Exceeding the maximum trunk members count";

/*******************************************************************************
*   Trunk wrapper errors                                                       *
*******************************************************************************/      
GT_CHAR trunkCreationErrorMessage[]         = 
                            "Could not create trunk with given configuration";
GT_CHAR trunkPortCountOveflowErrorMessage[] = 
                                                        "To many trunk members";
GT_CHAR trunkGettingErrorMessage[]          = 
                                "Could not read requested trunk information";


/***** declarations ********/

use_prv_print_struct_array(CPSS_TRUNK_MEMBER_STC);

/***** declarations ********/
                                
/*******************************************************************************
* check_wraplCpssDxChTrunk
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of trunk wrapper.
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
*
* COMMENTS:
*
*******************************************************************************/
int check_wraplCpssDxChTrunk()
{
    cpssOsPrintf("File wraplCpssDxChTrunk.c inside");
    return 1;
}


/*******************************************************************************
* pvrCpssTrunkMembersArrayToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Push dev/port array to lua stack.
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
*       L_index             - entry lua stack index
* 
* OUTPUTS:
*
* RETURNS:
*       1 if array was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssTrunkMembersArrayToLuaPush
(
    IN GT_STATUS                status,
    IN lua_State                *L,
    IN GT_U32                   numMembers,
    IN CPSS_TRUNK_MEMBER_STC    membersArray[]
)
{
    GT_U32                      index;
    
    if (GT_OK == status)
    {
        lua_newtable(L);
        for (index = 0; index < numMembers; index++)
        {
            lua_pushnumber(L, membersArray[index].hwDevice);
            lua_rawget(L, -2);
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                lua_pushnumber(L, membersArray[index].hwDevice);
                lua_newtable(L);
                lua_rawset(L, -3);
                lua_pushnumber(L, membersArray[index].hwDevice);
                lua_rawget(L, -2);
            }
            lua_pushnumber(L, membersArray[index].port);
            lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
            lua_pop(L, 1);
        }
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssTrunkMembersArrayFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Convert lua trunk member array to lua dev/port array
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L_index             - entry lua stack index
* 
* OUTPUTS:
*       numMembers          - number of taken devNum/portNum pairs
*       membersArray        - converted array
*       error_message       - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       GT_OK if no errors occurs otherwise error code and error message
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssTrunkMembersArrayFromLuaGet
(
    IN  lua_State               *L,
    IN  GT_32                   L_index,
    OUT GT_U32                  *numMembers,
    OUT CPSS_TRUNK_MEMBER_STC   *membersArray,
    OUT GT_CHAR_PTR             *error_message
)
{
    GT_U8                   devNum;
    GT_PORT_NUM             portNum;
    GT_U32                  index;
    GT_U8                   trunkPortCountOveflowFlag = GT_FALSE; 
    GT_STATUS               status = GT_OK;
    
    *numMembers = 0;
    
    if (0 != lua_istable(L, L_index))
    {  
        lua_pushnil(L);
        while ((0 != lua_next(L, L_index)) && 
               (GT_FALSE == trunkPortCountOveflowFlag))
        {
            devNum = (GT_U8) lua_tointeger(L, -2);
            
            for (index = 1; 
                 (index <= lua_objlen(L, -1)) && 
                 (GT_FALSE == trunkPortCountOveflowFlag); 
                 index++)
            {
                lua_rawgeti(L, -1, index);
                portNum = (GT_PORT_NUM) lua_tointeger(L, -1);
                lua_pop(L, 1);
                
                membersArray[*numMembers].port   = portNum;
                membersArray[*numMembers].hwDevice = devNum;
                if (CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS != *numMembers)
                {
                    (*numMembers)++;
                }
                else
                {
                    trunkPortCountOveflowFlag = GT_TRUE;
                    status          = GT_OUT_OF_RANGE;        
                    *error_message  = trunkPortCountOveflowErrorMessage;
                }                
            }
            
            lua_pop(L, 1);
        }
    }
   
    return status;
}


/*******************************************************************************
* prvCpssDxChDoesTrunkExist
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Check's that trunk exists.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum                - device number
*       trunkId               - trunk id
*       P_CALLING_FORMAL_DATA - general debug information (environment variables 
*                               etc); could be empty
*
* OUTPUTS:
*       doesVlanExist         - vlan existence
*       errorMessagePtr       - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDoesTrunkExist
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_BOOL                 *doesVlanExist, 
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_U32                      numMembers      = 0;
    CPSS_TRUNK_MEMBER_STC       membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_STATUS                   status          = GT_OK;

    *doesVlanExist      = GT_TRUE;
    *errorMessagePtr    = NULL;
    
    CPSS_API_CALL_LOCK();
    status = cpssDxChTrunkTableEntryGet(devNum, trunkId, &numMembers, 
                                        membersArray);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChTrunkTableEntryGet,
        PC_NUMBER(devNum,                       devNum,                  GT_U8, 
        PC_NUMBER(trunkId,                      trunkId,           GT_TRUNK_ID, 
        PC_NUMBER(numMembers,                   numMembers,             GT_U32,         
        PC_ARRAY (numMembers, &membersArray,    membersArray, 
                                                         CPSS_TRUNK_MEMBER_STC,        
        PC_LAST_PARAM)))),
        PC_STATUS);        
    if (GT_OK != status)
    {
        status          = GT_OK;
        *doesVlanExist  = GT_FALSE;
    }
        
    if ((GT_OK == status) && (GT_TRUE == *doesVlanExist))
    {
        *doesVlanExist  = (0 != numMembers) ? GT_TRUE : GT_FALSE;
    }
    
    return status;
}


/*******************************************************************************
* prvCpssDxChSupportedTrunksCount
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of supported trunks count on device.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum              - device number
* 
* OUTPUTS:
*       trunksNumberPtr     - number of trunks on device
*       errorMessagePtr     - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSupportedTrunksCount
(
    IN  GT_U8                   devNum,
    OUT GT_U32_PTR              trunksNumberPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_STATUS       status              = GT_OK;
    
    *errorMessagePtr   = NULL;   
        
    *trunksNumberPtr = 
        WRL_PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw;        
        
    return status;    
}


/*******************************************************************************
* prvCpssDxChTrunkMembersCount
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of trunk members (dev/ports) count.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum              - device number
*       trunkId             - trunk id
*       P_CALLING_FORMAL_DATA 
*                           - general debug information (environment variables 
*                             etc); could be empty
* 
* OUTPUTS:
*       numMembersInTrunkPtr
*                           - number of trunk members
*       errorMessagePtr     - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChTrunkMembersCount
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_U32_PTR              numMembersInTrunkPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    CPSS_TRUNK_MEMBER_STC 
                    membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_STATUS       status              = GT_OK;
    
    *errorMessagePtr   = NULL;    
    
    CPSS_API_CALL_LOCK();
    status = cpssDxChTrunkTableEntryGet(devNum, trunkId, numMembersInTrunkPtr, 
                                        membersArray);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChTrunkTableEntryGet,
        PC_NUMBER(devNum,                       devNum,                  GT_U8, 
        PC_NUMBER(trunkId,                      trunkId,           GT_TRUNK_ID, 
        PC_NUMBER(*numMembersInTrunkPtr,        numMembers,             GT_U32,         
        PC_ARRAY (*numMembersInTrunkPtr, &membersArray, membersArray, 
                                                         CPSS_TRUNK_MEMBER_STC,        
        PC_LAST_PARAM)))),
        PC_STATUS);                                        
    if (GT_OK != status)  
    {
        *errorMessagePtr    = trunkGettingErrorMessage;
    }      
        
    return status;
}


/*******************************************************************************
* prvCpssDxChIsDevicePortTrunkMember
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking that dev/port pair is member of passed trunk.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       trunkId             - trunk id
*       hwDevNum            - hardware device number
*       hwPortNum           - hardware port number
*       P_CALLING_FORMAL_DATA 
*                           - general debug information (environment variables 
*                             etc); could be empty
* 
* OUTPUTS:
*       isMemberInTrunkPtr - property that dev/port pair are members of trunk
*       errorMessagePtr    - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       operation execution staus
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChIsDevicePortTrunkMember
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_PHYSICAL_PORT_NUM    hwDevNum,
    IN  GT_HW_DEV_NUM           hwPortNum,    
    OUT GT_BOOL                 *isMemberInTrunkPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr    
    P_CALLING_FORMAL_DATA    
)
{
    GT_U32                      numberOfTrunkMembers    = 0;
    CPSS_TRUNK_MEMBER_STC       membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_STATUS                   status                  = GT_OK;

    *isMemberInTrunkPtr = GT_TRUE;
    *errorMessagePtr    = NULL;
    
    CPSS_API_CALL_LOCK();
    status = cpssDxChTrunkTableEntryGet(devNum, trunkId, &numberOfTrunkMembers, 
                                        membersArray);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChTrunkTableEntryGet,
        PC_NUMBER(devNum,                       devNum,                  GT_U8, 
        PC_NUMBER(trunkId,                      trunkId,           GT_TRUNK_ID, 
        PC_NUMBER(numberOfTrunkMembers,         numMembers,             GT_U32,         
        PC_ARRAY (numberOfTrunkMembers, &membersArray, membersArray, 
                                                         CPSS_TRUNK_MEMBER_STC,        
        PC_LAST_PARAM)))),
        PC_STATUS);                                        
    if (GT_OK != status)  
    {
        *isMemberInTrunkPtr = GT_FALSE;
        status              = GT_OK;
    }     
     
    if ((GT_OK == status) && (GT_TRUE == *isMemberInTrunkPtr))
    {
        status = prvCpssIsDevicePortBetweenTrunkMembers(hwDevNum, hwPortNum,
                                                        numberOfTrunkMembers,
                                                        membersArray,
                                                        isMemberInTrunkPtr,
                                                        errorMessagePtr);
    }
    
    return status;
}


/*******************************************************************************
* prvCpssDxChMaximumTrunkMembersCount
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum trunk members (dev/ports) count.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
* 
* OUTPUTS:
*       maximumNumberMembersInTrunkPtr
*                           - maximum number of trunk members
*       errorMessage        - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       GT_OK and maximum member counts pused to lua stack
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMaximumTrunkMembersCount
(
    IN  GT_U8                   devNum,
    OUT GT_U32_PTR              maximumNumberMembersInTrunkPtr,
    OUT GT_CHAR_PTR             *errorMessage    
)
{
    GT_STATUS       status      = GT_OK;    
    
    *errorMessage   = NULL;
    
    *maximumNumberMembersInTrunkPtr = 
        PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk;
    
    return status;
}


/*******************************************************************************
* prvXcatDeviceAndTrunkIDOfWATrafficToCPUCheck
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking, that device need the WA but the trunkId not according to the 
*       rules.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       trunkId             - trunk id
* 
* OUTPUTS:
*       chekingResultPtr    - checking result
*       errorMessage        - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
 GT_STATUS prvXcatDeviceAndTrunkIDOfWATrafficToCPUCheck
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_BOOL                 *chekingResultPtr,
    OUT GT_CHAR_PTR             *errorMessage
)
{
    GT_STATUS                   status          = GT_OK;                                                               
                           
    *errorMessage       = NULL;
                            
    *chekingResultPtr   = 
        GT_FALSE == WRL_PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->initDone;
    
    if ((GT_OK == status) && (GT_FALSE== *chekingResultPtr))
    {
        *chekingResultPtr   =  
            0 == WRL_PRV_CPSS_DXCH_CHECK_TRUNK_WRONG_SOURCE_PORT_TO_CPU_WA_MAC(devNum);
    }

    if ((GT_OK == status) && (GT_FALSE == *chekingResultPtr))
    {
        *chekingResultPtr   = 
            1 == WRL_XCAT_TRUNKID_CHECK_FOR_WA_WRONG_SOURCE_PORT_TO_CPU_MAC(trunkId);
    }

    if ((GT_OK == status) && (GT_FALSE == *chekingResultPtr))
    {
        *chekingResultPtr   = (((trunkId) & 0x7e) == 0x7e) || ((trunkId) == 0);
    }

    return status;                                                                 
}


/*******************************************************************************
* prvPortOccurrenceInOthersTrunksCheck
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking, that port in not occured in other trunks.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       trunkId             - trunk id
*       hwDevNum            - hardware device number
*       hwPortNum           - hardware port number
*       supportedTrunksNumber
*                           - number of supported trunks on given device
*       P_CALLING_FORMAL_DATA - general debug information (environment variables 
*                               etc); could be empty
* 
* OUTPUTS:
*       chekingResultPtr    - checking result
*       occurenceTrunkIdPtr - trunk id, where checked trunk occures
*       errorMessage        - possible error message, relevant 
*                                   if status != GT_OK
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
 GT_STATUS prvPortOccurrenceInOthersTrunksCheck
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_PHYSICAL_PORT_NUM    hwDevNum,
    IN  GT_HW_DEV_NUM           hwPortNum, 
    IN  GT_U32                  supportedTrunksNumber,    
    OUT GT_BOOL                 *chekingResultPtr,
    OUT GT_TRUNK_ID             *occurenceTrunkIdPtr,
    OUT GT_CHAR_PTR             *errorMessage
    P_CALLING_FORMAL_DATA
)
{
    GT_TRUNK_ID                 currentTrunkId;
    GT_BOOL                     isMemberInCurrentTrunk  = GT_FALSE;
    GT_STATUS                   status                  = GT_OK;                                                               
                           
    *chekingResultPtr       = GT_TRUE;
    *occurenceTrunkIdPtr    = 0;    
    *errorMessage           = NULL;
    
    for (currentTrunkId = 1; 
         (currentTrunkId < supportedTrunksNumber) && (GT_OK == status) &&
         (GT_TRUE == *chekingResultPtr); 
         currentTrunkId++)
    {
        if (trunkId != currentTrunkId)
        {
            status = prvCpssDxChIsDevicePortTrunkMember(devNum, currentTrunkId,
                                                        hwDevNum, hwPortNum,    
                                                        &isMemberInCurrentTrunk,
                                                        errorMessage 
                                                        P_CALLING_ACTUAL_DATA);
            if (GT_OK == status)
            {
                *chekingResultPtr = GT_FALSE == isMemberInCurrentTrunk;
                
                if (GT_FALSE == *chekingResultPtr)
                {
                    *occurenceTrunkIdPtr    = currentTrunkId;    
                }
            }
        }
    }    
        
    return status;    
}


/*******************************************************************************
* wrlDxChTrunkMemberAddingConditionChecking
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of condition for trunk member adding.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L[1]                - device number
*       L[2]                - trunk id
*       L[3]                - hardware device number
*       L[4]                - hardware port number
* 
* OUTPUTS:
*
* RETURNS:
*       GT_OK and true are pused to lua stack if no errors occurs and checking 
*       was successful
*       GT_OK, true and warning message are pused to lua stack if no errors 
*       occurs and checking was not successful
*       error status and error message it error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlDxChTrunkMemberAddingConditionChecking
(
    IN lua_State *L
)
{
    GT_U8                       devNum                  =  
                                                    (GT_U8) lua_tonumber(L, 1); 
                                                                    /* devId */
    GT_TRUNK_ID                 trunkId                 = 
                                            (GT_TRUNK_ID) lua_tonumber(L, 2); 
                                                                 /* trunkId */                                                                
    GT_HW_DEV_NUM               hwDevNum                =
                                            (GT_HW_DEV_NUM) lua_tonumber(L, 3); 
                                                                /* hwDevNum */    
    GT_PHYSICAL_PORT_NUM        hwPortNum               = 
                                    (GT_PHYSICAL_PORT_NUM) lua_tonumber(L, 4); 
                                                                /* hwPortNum */
    GT_BOOL                     isTrunkMember;                                                                
    GT_BOOL                     chekingResult           = GT_TRUE;
    GT_BOOL                     isPortAlreadyInTrunk    = GT_FALSE;
    GT_U32                      supportedTrunksNumber   = 0;
    GT_U32                      numMembersInTrunk       = 0;
    GT_U32                      maximumNumberMembersInTrunk
                                                        = 0;
    GT_TRUNK_ID                 temporaryTrunkId        = 0;
    GT_CHAR_PTR                 conditionWarningString  = NULL;
    GT_STATUS                   status                  = GT_OK;
    GT_CHAR_PTR                 error_message           = NULL;
    int                         returned_param_count    = 0;
    P_CALLING_CHECKING;

    status = prvCpssDxChIsDevicePortTrunkMember(devNum, trunkId, hwDevNum, 
                                                hwPortNum, &isTrunkMember, 
                                                &error_message 
                                                P_CALLING_ACTUAL_DATA); 
                                                
    chekingResult           = GT_FALSE == isTrunkMember;
    isPortAlreadyInTrunk    = GT_TRUE == isTrunkMember;
           
    if ((GT_OK == status) && (GT_TRUE == chekingResult))
    {
        status = prvCpssDxChSupportedTrunksCount(devNum, &supportedTrunksNumber,
                                                 &error_message);        
    }    
    
    if ((GT_OK == status) && (GT_TRUE == chekingResult))
    {
        status = prvPortOccurrenceInOthersTrunksCheck(
                     devNum, trunkId, hwDevNum, hwPortNum, 
                     supportedTrunksNumber, &chekingResult, &temporaryTrunkId, 
                     &error_message P_CALLING_ACTUAL_DATA);
        if ((GT_OK == status) && (GT_FALSE == chekingResult))
        {
            cpssOsSprintf(wraplWarningMessageString, 
                          portOccurrenceInOthersTrunksWarningMessage, 
                          temporaryTrunkId);
            conditionWarningString  = wraplWarningMessageString;
        }                                                      
    }               
           
    if ((GT_OK == status) && (GT_TRUE == chekingResult))
    {
        status = prvXcatDeviceAndTrunkIDOfWATrafficToCPUCheck(devNum, trunkId, 
                                                              &chekingResult, 
                                                              &error_message);
        if ((GT_OK == status) && (GT_FALSE == chekingResult))
        {
            conditionWarningString  = 
                xcatDeviceAndTrunkIDOfWATrafficToCPUWarningMessage;
        }
    }
    
    if ((GT_OK == status) && (GT_TRUE == chekingResult))
    {          
        status = prvCpssDxChTrunkMembersCount(devNum, trunkId, 
                                              &numMembersInTrunk,
                                              &error_message 
                                              P_CALLING_ACTUAL_DATA);
    }
                                              
    if ((GT_OK == status) && (GT_TRUE == chekingResult))
    {
        status = prvCpssDxChMaximumTrunkMembersCount(
                     devNum, &maximumNumberMembersInTrunk, &error_message);        
    }

    if ((GT_OK == status) && (GT_TRUE == chekingResult))
    {
        chekingResult   = maximumNumberMembersInTrunk >= numMembersInTrunk + 1;
        if (GT_FALSE == chekingResult)
        {
            conditionWarningString  = 
                maximumTrunkMembersCountExceedingWarningMessage; 
        }        
    }
    
    returned_param_count    += 
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    += 
        pvrCpssBooleanToLuaPush(status, L, chekingResult);
    returned_param_count    += 
        pvrCpssBooleanToLuaPush(status, L, isPortAlreadyInTrunk);
    returned_param_count    += 
           pvrCpssStringToLuaPush(status, L, conditionWarningString);  
    return returned_param_count;
}
 

/*******************************************************************************
* wrlCpssDxChTrunkAdd
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Creation of new trunk.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L[1]                - device number
*       L[2]                - trunk id
*       L[2]                - dev/port array 
*                               sample: {[0] = {1, 2}}
* 
* OUTPUTS:
*
* RETURNS:
*       GT_OK is pused to lua stack if no errors occurs otherwise 
*       error code and error message
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChTrunkAdd
(
    IN lua_State *L
)
{
    GT_U8                       devNum =  (GT_U8)       lua_tonumber(L, 1); 
                                                                    /* devId */
    GT_TRUNK_ID                 trunkId = (GT_TRUNK_ID) lua_tonumber(L, 2); 
                                                                  /* trunkId */
    GT_U32						numMembers;
    CPSS_TRUNK_MEMBER_STC		membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];    
    GT_STATUS                   status = GT_OK;
    GT_CHAR_PTR                 error_message = NULL;
    P_CALLING_CHECKING;        
    
    /* Converting of trunk members array. */
    status = pvrCpssTrunkMembersArrayFromLuaGet(L, 3, &numMembers, membersArray,
                                                &error_message);    
    
    /* Creating of new trunk. */
    if (GT_OK == status)
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChTrunkTableEntrySet(devNum, trunkId, numMembers,
                                            membersArray);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChTrunkTableEntrySet,
            PC_NUMBER(devNum,                   devNum,                  GT_U8, 
            
            PC_NUMBER(trunkId,                  trunkId,           GT_TRUNK_ID, 
            PC_NUMBER(numMembers,               numMembers,             GT_U32,         
            PC_ARRAY (numMembers, &membersArray,membersArray, 
                                                         CPSS_TRUNK_MEMBER_STC,        
            PC_LAST_PARAM)))),
            PC_STATUS);                                               
        if (GT_OK != status)
        {
            error_message   = trunkCreationErrorMessage;
        }
    }

    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}


/*******************************************************************************
* wrlCpssDxChTrunkExists
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Checking of trunk existance.
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
*       GT_OK and 1 is pused to lua stack if vlan exists otherwise GT_OK and 0 
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChTrunkExists
(
    IN lua_State *L
)
{
    GT_U32                  numMembers = 0;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_STATUS               rc;
    P_CALLING_CHECKING;    

    CPSS_API_CALL_LOCK();
    rc = cpssDxChTrunkTableEntryGet(
            (GT_U8)       lua_tonumber(L, 1) /*devNum*/,
            (GT_TRUNK_ID) lua_tonumber(L, 2) /*trunkId*/,
            &numMembers,
            membersArray);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChTrunkTableEntryGet,
        PC_NUMBER(lua_tonumber(L, 1),           devNum,                  GT_U8, 
        PC_NUMBER(lua_tonumber(L, 2),           trunkId,           GT_TRUNK_ID, 
        PC_NUMBER(numMembers,                   numMembers,             GT_U32,         
        PC_ARRAY (numMembers, &membersArray,    membersArray, 
                                                         CPSS_TRUNK_MEMBER_STC,        
        PC_LAST_PARAM)))),
        rc);            

	lua_pushinteger(L, 0);
    lua_pushboolean(L, ((rc == GT_OK) && (0 != numMembers)) ? GT_TRUE : GT_FALSE);
    return 2;
}


/*******************************************************************************
* wrlCpssDxChTrunkMembersCount
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of trunk members (dev/ports) count.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L[1]                - device number
* 
* OUTPUTS:
*
* RETURNS:
*       GT_OK and member counts pused to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChTrunkMembersCount
(
    IN lua_State    *L
)
{
    GT_U8           devNum                  = (GT_U8) lua_tonumber(L, 1); 
                                                                    /* devId */
    GT_TRUNK_ID     trunkId                 = (GT_TRUNK_ID) lua_tonumber(L, 2); 
                                                                  /* trunkId */ 
    GT_U32          numMembersInTrunk       = 0;
    GT_STATUS       status                  = GT_OK;
    GT_CHAR_PTR     error_message           = NULL;
    int             returned_param_count    = 0; 
    P_CALLING_CHECKING;    
    
    status = prvCpssDxChTrunkMembersCount(devNum, trunkId, &numMembersInTrunk,
                                          &error_message P_CALLING_ACTUAL_DATA);
  
    returned_param_count    +=       
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=         
        pvrCpssNumberToLuaPush(status, L, numMembersInTrunk);
    return returned_param_count;             
}


/*******************************************************************************
* wrlCpssDxChMaximumTrunkMembersCount
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum trunk members (dev/ports) count.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*       L[1]                - device number
* 
* OUTPUTS:
*
* RETURNS:
*       GT_OK and maximum member counts pused to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChMaximumTrunkMembersCount
(
    IN lua_State    *L
)
{
    GT_U8           devNum                      = (GT_U8) lua_tonumber(L, 1); 
                                                                    /* devId */
    GT_U32          maximumNumberMembersInTrunk = 0;
    GT_STATUS       status                      = GT_OK;
    GT_CHAR_PTR     error_message               = NULL;
    int             returned_param_count        = 0;    
    
    status = prvCpssDxChMaximumTrunkMembersCount(
                 devNum, &maximumNumberMembersInTrunk, &error_message);    
    
    returned_param_count    +=     
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=     
        pvrCpssNumberToLuaPush(status, L, maximumNumberMembersInTrunk);
    return returned_param_count;            
}


/*******************************************************************************
* wrlCpssDxChTrunkDevicesAndPortsList
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of trunk devices/ports list.
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
*       devices/ports list pused to lua stack 
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChTrunkDevicesAndPortsList
(
    IN lua_State *L
)
{
    GT_U8                   devNum                  =  
                                            (GT_U8)       lua_tonumber(L, 1); 
                                                                    /* devId */
    GT_TRUNK_ID             trunkId                 = 
                                            (GT_TRUNK_ID) lua_tonumber(L, 2); 
                                                                  /* trunkId */
    GT_U32                  numMembers              = 0;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS]; 
    GT_BOOL                 isInitializedMember;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;   
    int                     returned_param_count    = 0;     
    P_CALLING_CHECKING;

    CPSS_API_CALL_LOCK();
    status = cpssDxChTrunkTableEntryGet(devNum, trunkId, &numMembers,
                                        membersArray);
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChTrunkTableEntryGet,
        PC_NUMBER(devNum,                       devNum,                  GT_U8, 
        PC_NUMBER(trunkId,                      trunkId,           GT_TRUNK_ID, 
        PC_NUMBER(numMembers,                   numMembers,             GT_U32,         
        PC_ARRAY (numMembers, &membersArray,    membersArray, 
                                                         CPSS_TRUNK_MEMBER_STC,        
        PC_LAST_PARAM)))),
        PC_STATUS);  
    if (GT_OK != status)  
    {
        error_message   = trunkGettingErrorMessage;
    }        
 
    if ((GT_OK == status) && (1 == numMembers))
    {
        status = prvCpssIsHwDevicePortExist(
                     membersArray[0].hwDevice, membersArray[0].port,
                     &isInitializedMember, &error_message);
        
        if ((GT_OK == status) && (GT_FALSE == isInitializedMember))
        {
            numMembers = 0;
        }
    }
     
    returned_param_count    +=       
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=      
        pvrCpssTrunkMembersArrayToLuaPush(status, L, numMembers, membersArray);
    return returned_param_count;            
}

