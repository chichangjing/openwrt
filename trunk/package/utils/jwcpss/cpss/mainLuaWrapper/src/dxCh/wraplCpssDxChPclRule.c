/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChPclRule.c
*
* DESCRIPTION:
*       A lua wrapper for PCL Rule functions
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/pcl/cpssDxChPcl.h>
#include <generic/private/prvWraplGeneral.h>
#include <cpssCommon/wrapCpssDebugInfo.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

/*******************************************************************************
*   Trunk wrapper errors                                                       *
*******************************************************************************/      
GT_CHAR pclRuleStatusPatternAndActionGettingErrorMessage[]         = 
                "Could not get PLC Rule valid status, Mask, Pattern and Action";

                
/***** declarations ********/

use_prv_enum(CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT)
use_prv_enum(CPSS_DXCH_PCL_RULE_OPTION_ENT)
use_prv_struct(CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC)
use_prv_struct(CPSS_DXCH_PCL_ACTION_STC)
use_prv_print_struct(CPSS_DXCH_PCL_ACTION_STC)
use_prv_print_union(CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT, 
                    CPSS_DXCH_PCL_RULE_FORMAT_UNT)

/***** declarations ********/

/*******************************************************************************
* pvrCpssDxChPCLRuleFormatFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of Policy rules getting from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                                - lua state
*        L_index                          - entry lua stack index
* 
* OUTPUTS:
*       ruleFormatPtr                     - PCL rule format
*       errorMessagePtr                   - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssDxChPCLRuleFormatFromLuaGet
(
    IN  lua_State                           *L,
    IN  GT_32                               L_index,
    OUT CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  *ruleFormatPtr,
    OUT GT_CHAR_PTR                         *errorMessagePtr
)
{
    GT_STATUS                       status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    *ruleFormatPtr = 
        (CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT) 
            prv_lua_to_c_enum(L, enumDescr_CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT);
    
    lua_pop(L, 1);
    
    return status;
}

/*******************************************************************************
* pvrCpssDxChPCLRuleOptionFromLuaGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of Enumerator getting for the TCAM rule write option flags 
*       from lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        L                                - lua state
*        L_index                          - entry lua stack index
* 
* OUTPUTS:
*       ruleOptionPtr                     - Enumerator getting for the TCAM rule  
*                                           write option flags
*       errorMessagePtr                   - error message
*
* RETURNS:
*       operation succed or error code and error message if occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pvrCpssDxChPCLRuleOptionFromLuaGet
(
    IN  lua_State                           *L,
    IN  GT_32                               L_index,
    OUT CPSS_DXCH_PCL_RULE_OPTION_ENT       *ruleOptionPtr,
    OUT GT_CHAR_PTR                         *errorMessagePtr
)
{
    GT_STATUS                       status          = GT_OK; 
    
    *errorMessagePtr = NULL;
    
    lua_pushvalue(L, L_index);
    
    *ruleOptionPtr = 
        (CPSS_DXCH_PCL_RULE_OPTION_ENT) 
            prv_lua_to_c_enum(L, enumDescr_CPSS_DXCH_PCL_RULE_OPTION_ENT);
    
    lua_pop(L, 1);
    
    return status;
}


/*******************************************************************************
* pvrCpssDxChRuleFormatIngressExtendedUDBToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of PCL Rule ingress extended "UDB" Key fields pushing to 
*       lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status                            - caller status
*       L                                 - lua state
*       ruleFormatIngressExtendedPtr      - PCL Rule ingress extended "UDB" Key 
*                                           fields
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssDxChRuleFormatIngressExtendedUDBToLuaPush
(
    IN GT_STATUS                            status,
    IN lua_State                            *L,
    IN CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC   
                                            *ruleFormatIngressExtendedPtr
)
{   
    if (GT_OK == status)
    {
        prv_c_to_lua_CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXTENDED_UDB_STC(
            L, ruleFormatIngressExtendedPtr); 
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* pvrCpssDxChPCLActionToLuaPush
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS wrapper of Policy Engine Action pushing to lua stack.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       status                            - caller status
*       L                                 - lua state
*       pclActionPtr                      - Policy Engine Action
* 
* OUTPUTS:
*
* RETURNS:
*       1 if string was pushed to stack otherwise 0
*
* COMMENTS:
*
*******************************************************************************/
int pvrCpssDxChPCLActionToLuaPush
(
    IN GT_STATUS                            status,
    IN lua_State                            *L,
    IN CPSS_DXCH_PCL_ACTION_STC             *pclActionPtr
)
{   
    if (GT_OK == status)
    {
        prv_c_to_lua_CPSS_DXCH_PCL_ACTION_STC(L, pclActionPtr); 
        
        return 1;        
    }
    else
    {
        return 0;    
    }
}


/*******************************************************************************
* wrlCpssDxChIngressExtendedUDBPclRuleParsedGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       The function gets the Policy Rule valid status, Mask, Pattern and Action 
*       with Ingress Extended UDB rule pattern
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       L                   - lua state
* 
* OUTPUTS:
*       devices/ports list pused to lua stack 
*
* RETURNS:
*       5; GT_OK and rule validness, mask, pattern and policy are pused to lua 
*       stack if no errors occurs
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChIngressExtendedUDBPclRuleParsedGet  
(
    IN lua_State *L
)
{
    GT_U8                               devNum                  =    
                                                    (GT_U8) lua_tonumber(L, 1); 
                                                                    /* devId */
    CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat              =
                                CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E;
    GT_U32                              ruleIndex               =   
                                                    (GT_U32) lua_tonumber(L, 3);
                                                                /* ruleIndex */
    CPSS_DXCH_PCL_RULE_OPTION_ENT       ruleOptionsBmp          =   
                                    CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E;
    GT_BOOL                             isRuleValid             = GT_FALSE;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       mask;
    CPSS_DXCH_PCL_RULE_FORMAT_UNT       pattern;
    CPSS_DXCH_PCL_ACTION_STC            action;
    GT_STATUS                           status                  = GT_OK;
    GT_CHAR_PTR                         error_message           = NULL;    
    int                                 returned_param_count    = 0;     
    P_CALLING_CHECKING;    

    status = pvrCpssDxChPCLRuleFormatFromLuaGet(L, 2, &ruleFormat, 
                                                &error_message);
    if (GT_OK == status) 
    {    
        status = pvrCpssDxChPCLRuleOptionFromLuaGet(L, 4, &ruleOptionsBmp, 
                                                    &error_message);
    }                                            

    if (GT_OK == status) 
    { 
        CPSS_API_CALL_LOCK();
        status = cpssDxChPclRuleParsedGet(devNum, ruleFormat, ruleIndex, 
                                          ruleOptionsBmp, &isRuleValid, &mask, 
                                          &pattern, &action);
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChPclRuleParsedGet,
            PC_NUMBER(devNum,                   devNum,                  GT_U8, 
            PC_ENUM  (ruleFormat,               ruleFormat, 
                                            CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT, 
            PC_NUMBER(ruleIndex,                ruleIndex,              GT_U32, 
            PC_ENUM  (ruleOptionsBmp,           ruleOptionsBmp, 
                                                 CPSS_DXCH_PCL_RULE_OPTION_ENT,             
            PC_BOOL  (isRuleValid,              isRuleValid,             
            PC_UNION (ruleFormat,   &mask,      mask,      
                                                 CPSS_DXCH_PCL_RULE_FORMAT_UNT, 
            PC_UNION (ruleFormat,   &pattern,   pattern,      
                                                 CPSS_DXCH_PCL_RULE_FORMAT_UNT,                 
            PC_STRUCT(&action,                  action,      
                                                      CPSS_DXCH_PCL_ACTION_STC,             
            PC_LAST_PARAM)))))))),
            PC_STATUS);               
        if (GT_OK != status)  
        {                                          
            error_message   = pclRuleStatusPatternAndActionGettingErrorMessage;
        }
    }    
    
    returned_param_count    +=      
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=      
        pvrCpssBooleanToLuaPush(status, L, isRuleValid);
    returned_param_count    +=             
        pvrCpssDxChRuleFormatIngressExtendedUDBToLuaPush(
            status, L, &mask.ruleIngrExtUdb);
    returned_param_count    +=                 
        pvrCpssDxChRuleFormatIngressExtendedUDBToLuaPush(
            status, L, &pattern.ruleIngrExtUdb);
    returned_param_count    +=                 
        pvrCpssDxChPCLActionToLuaPush(status, L, &action);
    return returned_param_count;             
}

/*******************************************************************************
* wrlCpssDxChPclRuleGet
*
* DESCRIPTION:
*   The function gets the Policy Rule Mask, Pattern and Action in Hw format
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                   - lua state
*           1:GT_U8                  devNum     device number
*           2.CPSS_PCL_RULE_SIZE_ENT ruleSize   size of the Rule.
*           3:GT_U32                 ruleIndex  index of the rule in the TCAM.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       status, { mask = {21 words}, pattern = { 21 words }, action = { 4 words }
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - For DxCh3 and above if cannot determinate
*                                  the rule size by found X/Y bits of compare mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
use_prv_enum(CPSS_PCL_RULE_SIZE_ENT);
int wrlCpssDxChPclRuleGet  
(
    IN lua_State *L
)
{
    GT_STATUS              status = GT_OK;
    GT_U8                  devNum = 0; /* assigned with a default value - 0 */
    CPSS_PCL_RULE_SIZE_ENT ruleSize = CPSS_PCL_RULE_SIZE_STD_E; /* assigned with a default value - CPSS_PCL_RULE_SIZE_STD_E */
    GT_U32                 ruleIndex = 0; /* assigned with a default value - 0 */
    GT_U32                 maskArr[21];
    GT_U32                 patternArr[21];
    GT_U32                 actionArr[4];
    int                    i;

    if (lua_gettop(L) < 3)
    {
        lua_pushinteger(L, GT_BAD_PARAM);
        return 1;
    }
    PARAM_NUMBER(status, devNum,    1, GT_U8);
    PARAM_ENUM  (status, ruleSize,  2, CPSS_PCL_RULE_SIZE_ENT);
    PARAM_NUMBER(status, ruleIndex, 3, GT_U32);
    if (status != GT_OK)
    {
        lua_pushinteger(L, (lua_Integer)status);
        return 1;
    }
    
    status = cpssDxChPclRuleGet(devNum, ruleSize, ruleIndex, maskArr, patternArr, actionArr);

    lua_pushinteger(L, (lua_Integer)status);
    if (status != GT_OK)
        return 1;
    lua_newtable(L);
    lua_newtable(L); /* mask */
    for (i = 0; i < 21; i++)
    {
        lua_pushnumber(L, maskArr[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setfield(L, -2, "mask");
    lua_newtable(L); /* pattern */
    for (i = 0; i < 21; i++)
    {
        lua_pushnumber(L, patternArr[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setfield(L, -2, "pattern");
    lua_newtable(L); /* action */
    for (i = 0; i < 4; i++)
    {
        lua_pushnumber(L, actionArr[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setfield(L, -2, "action");
    return 2;
}
