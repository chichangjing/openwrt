/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChMirror.c
*
* DESCRIPTION:
*       A lua wrapper for mirroring functions
*       It implements support for the following types:
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 40 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>
/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/bridge/cpssGenBrgFdb.h>
#include <generic/private/prvWraplGeneral.h>
#include <dxCh/wraplCpssDxChDeviceInfo.h>
#include <dxCh/wraplCpssDxChMirror.h>
#include <cpssCommon/wraplCpssDeviceInfo.h>
#include <cpssCommon/wrapCpssDebugInfo.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>

int check_wraplCpssDxChMirror()
{
    return 1;
}

/* externs */
use_prv_struct(CPSS_INTERFACE_INFO_STC);

/*******************************************************************************
*   Mirror wrapper errors                                                      *
*******************************************************************************/      
GT_CHAR nextMirrorRxPortGetErrorMessage[] = 
                                        "Could not get next mirror rx port.";
GT_CHAR nextMirrorTxPortGetErrorMessage[] = 
                                        "Could not get next mirror tx port.";

                                        
/*******************************************************************************
* prvCpssDxChMaximumMirrorAnalyzerIndex
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum mirror analyzer index.
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
*       maximumMirrorAnalyzerIndexPtr  
                           - maximum mirror analyzer index
*       errorMessagePtr    - error message
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMaximumMirrorAnalyzerIndex
(
    OUT GT_U32_PTR              maximumMirrorAnalyzerIndexPtr,
    OUT GT_CHAR_PTR             *errorMessagePtr
)
{
    GT_STATUS                   status          = GT_OK; 
    *errorMessagePtr    = NULL;  

    *maximumMirrorAnalyzerIndexPtr = 
                                WRL_PRV_CPSS_DXCH_MIRROR_ANALYZER_MAX_INDEX_CNS;
    
    return status;    
}                                        
 
 
/*******************************************************************************
* prvCpssDxChNextRxMirroredPortGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next rx mirrored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum             - device number
*        portNumPtr         - first checked port number
*       P_CALLING_FORMAL_DATA 
*                         - general debug information (environment variables 
*                           etc); could be empty
* 
* OUTPUTS:
*        portNumPtr         - founded port number (if relevant)
*        portWasFoundPtr    - mirrored port founding property
*        errorMessagePtr    - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChNextRxMirroredPortGet
(
    IN    GT_U8             devNum,
    INOUT GT_PORT_NUM       *portNumPtr,
    OUT   GT_BOOL           *portWasFoundPtr, 
    OUT   GT_U32            *mirrorAnalyzerEntryIndex,
    OUT   GT_CHAR_PTR       *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_BOOL                 rxMirroringPortFound                = GT_FALSE;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC     
                            analyzerInterface;    
    GT_STATUS               status                              = GT_OK;   
    
    cmdOsMemSet(&analyzerInterface, 0, sizeof(analyzerInterface));
    
    while ((GT_OK == status) && (GT_FALSE == rxMirroringPortFound) &&
           (*portNumPtr < PRV_CPSS_PP_MAC(devNum)->numOfPorts))
    {
        if (WRL_PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, 
                                                             *portNumPtr))
        {
            CPSS_API_CALL_LOCK();
            status = cpssDxChMirrorRxPortGet(devNum, *portNumPtr, GT_TRUE, 
                                             &rxMirroringPortFound,                     
                                             mirrorAnalyzerEntryIndex);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChMirrorRxPortGet,
                PC_NUMBER(devNum,               devNum,                 GT_U8, 
                PC_NUMBER(*portNumPtr,          mirrPort,         GT_PORT_NUM,  
                PC_BOOL(GT_TRUE,                isPhysicalPort,
                PC_BOOL(rxMirroringPortFound,   enable,    
                PC_NUMBER(mirrorAnalyzerEntryIndex, 
                                                index,                 GT_U32,
                PC_LAST_PARAM))))),
                PC_STATUS);         
            if (GT_OK != status)
            {
                *errorMessagePtr = nextMirrorRxPortGetErrorMessage;
                break;
            }           
        }
        
        (*portNumPtr)++;
    }
    
    if ((GT_OK == status) && (GT_TRUE == rxMirroringPortFound) && 
        (*portNumPtr < PRV_CPSS_PP_MAC(devNum)->numOfPorts))
    {
        (*portNumPtr)--;

        *portWasFoundPtr = GT_TRUE;
    }
    else
    {
        *portWasFoundPtr = GT_FALSE;
    }
    
    return status;
}


/*******************************************************************************
* prvCpssDxChNextTxMirroredPortGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next tx mirrored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum             - device number
*        portNumPtr         - first checked port number
*       P_CALLING_FORMAL_DATA 
*                         - general debug information (environment variables 
*                           etc); could be empty
* 
* OUTPUTS:
*        portNumPtr         - founded port number (if relevant)
*        portWasFoundPtr    - mirrored port founding property
*        errorMessagePtr    - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChNextTxMirroredPortGet
(
    IN    GT_U8             devNum,
    INOUT GT_PORT_NUM       *portNumPtr,
    OUT   GT_BOOL           *portWasFoundPtr, 
    OUT   GT_U32            *mirrorAnalyzerEntryIndex,
    OUT   GT_CHAR_PTR       *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_BOOL                 txMirroringPortFound                = GT_FALSE;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC     
                            analyzerInterface;    
    GT_STATUS               status                              = GT_OK;   
    
    cmdOsMemSet(&analyzerInterface, 0, sizeof(analyzerInterface));
    
    while ((GT_OK == status) && (GT_FALSE == txMirroringPortFound) &&
           (*portNumPtr < PRV_CPSS_PP_MAC(devNum)->numOfPorts))
    {
        if (WRL_PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, 
                                                             *portNumPtr))
        {
            CPSS_API_CALL_LOCK();
            status = cpssDxChMirrorTxPortGet(devNum, *portNumPtr, GT_TRUE, 
                                             &txMirroringPortFound,                     
                                             mirrorAnalyzerEntryIndex);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChMirrorTxPortGet,
                PC_NUMBER(devNum,               devNum,                 GT_U8, 
                PC_NUMBER(*portNumPtr,          mirrPort,         GT_PORT_NUM,  
                PC_BOOL(GT_TRUE,                isPhysicalPort,
                PC_BOOL(txMirroringPortFound,   enable,    
                PC_NUMBER(mirrorAnalyzerEntryIndex, 
                                                index,                 GT_U32,
                PC_LAST_PARAM))))),
                PC_STATUS);                                                
            if (GT_OK != status)
            {
                *errorMessagePtr = nextMirrorRxPortGetErrorMessage;
                break;
            }           
        }
        
        (*portNumPtr)++;
    }
    
    if ((GT_OK == status) && (GT_TRUE == txMirroringPortFound) && 
        (*portNumPtr < PRV_CPSS_PP_MAC(devNum)->numOfPorts))
    {
        (*portNumPtr)--;

        *portWasFoundPtr = GT_TRUE;
    }
    else
    {
        *portWasFoundPtr = GT_FALSE;
    }
    
    return status;
}

 
/*******************************************************************************
* prvCpssDxChNextMirroredPortGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next mirrored port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum             - device number
*        checkMirrorAnalyzerEntryIndex
*                           - mirror analyzer entry checking
*        portNumPtr         - first checked port number
*        mirrorAnalyzerEntryIndex
*                           - cheked mirror analyzer entry (could be irrelevant)
*       P_CALLING_FORMAL_DATA 
*                         - general debug information (environment variables 
*                           etc); could be empty
* 
* OUTPUTS:
*        portNumPtr         - founded port number (if relevant)
*        mirrorAnalyzerEntryIndex
*                           - analyzer entry index of founded port
*        portWasFoundPtr    - mirrored port founding property
*        errorMessagePtr    - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChNextMirroredPortGet
(
    IN    GT_U8             devNum,
    IN    GT_BOOL           checkMirrorAnalyzerEntryIndex,
    INOUT GT_PORT_NUM       *portNumPtr,
    INOUT GT_U32            *mirrorAnalyzerEntryIndex,    
    OUT   GT_BOOL           *portWasFoundPtr, 
    OUT   GT_CHAR_PTR       *errorMessagePtr
    P_CALLING_FORMAL_DATA
)
{
    GT_BOOL                 rxMirroringPortFound                = GT_FALSE;
    GT_BOOL                 txMirroringPortFound                = GT_FALSE;
    GT_U32                  etalonMirrorAnalyzerEntryIndex      = 
                                                    *mirrorAnalyzerEntryIndex;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC     
                            analyzerInterface;    
    GT_STATUS               status                              = GT_OK;   
    
    cmdOsMemSet(&analyzerInterface, 0, sizeof(analyzerInterface));
    
    status = prvCpssDxChMaximumMirrorAnalyzerIndex(mirrorAnalyzerEntryIndex, 
                                                   errorMessagePtr);
    
    while ((GT_OK == status) && 
           (GT_FALSE == rxMirroringPortFound) && 
           (GT_FALSE == txMirroringPortFound) &&
           (*portNumPtr < PRV_CPSS_PP_MAC(devNum)->numOfPorts))
    {
        if (WRL_PRV_CPSS_DXCH_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, 
                                                             *portNumPtr))
        {
            CPSS_API_CALL_LOCK();
            status = cpssDxChMirrorRxPortGet(devNum, *portNumPtr, GT_TRUE, 
                                             &rxMirroringPortFound,                     
                                             mirrorAnalyzerEntryIndex);
            CPSS_API_CALL_UNLOCK();
            P_CALLING_API(
                cpssDxChMirrorRxPortGet,
                PC_NUMBER(devNum,               devNum,                 GT_U8, 
                PC_NUMBER(*portNumPtr,          mirrPort,         GT_PORT_NUM,  
                PC_BOOL(GT_TRUE,                isPhysicalPort,
                PC_BOOL(rxMirroringPortFound,   enable,    
                PC_NUMBER(mirrorAnalyzerEntryIndex, 
                                                index,                 GT_U32,
                PC_LAST_PARAM))))),
                PC_STATUS);                                                
            if (GT_OK != status)
            {
                *errorMessagePtr = nextMirrorRxPortGetErrorMessage;
                break;
            }
             
            if ((GT_OK == status) && (GT_TRUE == rxMirroringPortFound) &&
                (GT_TRUE == checkMirrorAnalyzerEntryIndex))
            {
                rxMirroringPortFound = 
                    etalonMirrorAnalyzerEntryIndex == *mirrorAnalyzerEntryIndex;
            }
             
            if ((GT_OK == status) && (GT_FALSE == rxMirroringPortFound))
            {
                CPSS_API_CALL_LOCK();
                status = cpssDxChMirrorTxPortGet(devNum, *portNumPtr, GT_TRUE, 
                                                 &txMirroringPortFound, 
                                                 mirrorAnalyzerEntryIndex); 
                CPSS_API_CALL_UNLOCK();
                P_CALLING_API(
                    cpssDxChMirrorTxPortGet,
                    PC_NUMBER(devNum,           devNum,                 GT_U8, 
                    PC_NUMBER(*portNumPtr,      mirrPort,         GT_PORT_NUM,  
                    PC_BOOL(GT_TRUE,            isPhysicalPort,
                    PC_BOOL(txMirroringPortFound,   
                                                enable,    
                    PC_NUMBER(mirrorAnalyzerEntryIndex, 
                                                index,                 GT_U32,
                    PC_LAST_PARAM))))),
                    PC_STATUS);                                                    
                if (GT_OK != status)
                {
                    *errorMessagePtr = nextMirrorTxPortGetErrorMessage;
                    break;
                }
                
                if ((GT_OK == status) && (GT_TRUE == txMirroringPortFound) &&
                    (GT_TRUE == checkMirrorAnalyzerEntryIndex))
                {
                    txMirroringPortFound = 
                        etalonMirrorAnalyzerEntryIndex == 
                            *mirrorAnalyzerEntryIndex;
                }                
            }
        }
        
        (*portNumPtr)++;
    }
    
    if ((GT_OK == status)                   && 
        ((GT_TRUE == rxMirroringPortFound)      || 
         (GT_TRUE == txMirroringPortFound))     && 
        (*portNumPtr < PRV_CPSS_PP_MAC(devNum)->numOfPorts))
    {
        (*portNumPtr)--;

        *portWasFoundPtr = GT_TRUE;
    }
    else
    {
        *portWasFoundPtr = GT_FALSE;
    }
    
    return status;
}


/*******************************************************************************
* wrlCpssDxChMaximumMirrorAnalyzerIndex
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of maximum mirror analyzer index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       L                     - lua state
* 
* OUTPUTS:
*       2; GT_OK and maximum mirror analyzer index are pused to lua stack if no  
*       errors occurs and such port exists 
*       2; error code and error message, if error occures
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChMaximumMirrorAnalyzerIndex
(
    IN lua_State                *L
)
{ 
    GT_U32                      maximumMirrorAnalyzerIndex  = 0;
    GT_STATUS                   status                      = GT_OK; 
    GT_CHAR_PTR                 error_message               = NULL;
    int                         returned_param_count        = 0; 

    status = prvCpssDxChMaximumMirrorAnalyzerIndex(&maximumMirrorAnalyzerIndex, 
                                                   &error_message);
    
    returned_param_count    +=      
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=  
        pvrCpssNumberToLuaPush(status, L, maximumMirrorAnalyzerIndex);   
    return returned_param_count;             
}


/*******************************************************************************
* wrlCpssDxChNextRxMirroredPortGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next rx mirrored port.
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
*        portNumPtr         - founded port number (if relevant)
*        portWasFoundPtr    - mirrored port founding property
*        errorMessagePtr    - error message
*
* RETURNS:
*       2; GT_OK and next mirrored port num is pused to lua stack if no errors 
*       occurs and such port exists 
*       1; GT_OK is pused to lua stack if no errors occurs and such port is not 
*       exists 
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextRxMirroredPortGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                      = 
                                                    (GT_U8)  lua_tonumber(L, 1); 
                                                                    /* devId */ 
    GT_PORT_NUM             portNum                     = 
                                            (GT_PORT_NUM) lua_tonumber(L, 2); 
                                                                  /* portNum */
    GT_BOOL                 portWasFound                = GT_FALSE;  
    GT_U32                  mirrorAnalyzerEntryIndex    = 0;
    GT_STATUS               status                      = GT_OK;
    GT_CHAR_PTR             error_message               = NULL; 
    int                     returned_param_count        = 0;    
    P_CALLING_CHECKING;    
    
    status = prvCpssDxChNextRxMirroredPortGet(devNum, &portNum, &portWasFound,    
                                              &mirrorAnalyzerEntryIndex, 
                                              &error_message 
                                              P_CALLING_ACTUAL_DATA);
                                
    returned_param_count    += 
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=     
        pvrCpssNumberToLuaOnConditionPush(status, L, portNum, portWasFound);
    returned_param_count    +=            
        pvrCpssNumberToLuaOnConditionPush(status, L, mirrorAnalyzerEntryIndex, 
                                          portWasFound);
    return returned_param_count;                                             
}


/*******************************************************************************
* wrlCpssDxChNextTxMirroredPortGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next tx mirrored port.
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
*        portNumPtr         - founded port number (if relevant)
*        portWasFoundPtr    - mirrored port founding property
*        errorMessagePtr    - error message
*
* RETURNS:
*       2; GT_OK and next mirrored port num is pused to lua stack if no errors 
*       occurs and such port exists 
*       1; GT_OK is pused to lua stack if no errors occurs and such port is not 
*       exists 
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextTxMirroredPortGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                      = 
                                                    (GT_U8)  lua_tonumber(L, 1); 
                                                                    /* devId */ 
    GT_PORT_NUM             portNum                     = 
                                            (GT_PORT_NUM) lua_tonumber(L, 2); 
                                                                  /* portNum */
    GT_BOOL                 portWasFound                = GT_FALSE;  
    GT_U32                  mirrorAnalyzerEntryIndex    = 0;
    GT_STATUS               status                      = GT_OK;
    GT_CHAR_PTR             error_message               = NULL; 
    int                     returned_param_count        = 0;      
    P_CALLING_CHECKING;
    
    status = prvCpssDxChNextTxMirroredPortGet(devNum, &portNum, &portWasFound,    
                                              &mirrorAnalyzerEntryIndex, 
                                              &error_message 
                                              P_CALLING_ACTUAL_DATA);
                                            
    returned_param_count    +=                                                
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=        
        pvrCpssNumberToLuaOnConditionPush(status, L, portNum, portWasFound);
    returned_param_count    +=               
        pvrCpssNumberToLuaOnConditionPush(status, L, mirrorAnalyzerEntryIndex, 
                                          portWasFound);
    return returned_param_count;                                               
}


/*******************************************************************************
* wrlCpssDxChNextMirroredPortGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of next mirrored port.
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
*        portNumPtr         - founded port number (if relevant)
*        portWasFoundPtr    - mirrored port founding property
*        errorMessagePtr    - error message
*
* RETURNS:
*       2; GT_OK and next mirrored port num is pused to lua stack if no errors 
*       occurs and such port exists 
*       1; GT_OK is pused to lua stack if no errors occurs and such port is not 
*       exists 
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChNextMirroredPortGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                      = 
                                                    (GT_U8)  lua_tonumber(L, 1); 
                                                                    /* devId */ 
    GT_PORT_NUM             portNum                     = 
                                            (GT_PORT_NUM) lua_tonumber(L, 2); 
                                                                  /* portNum */
    GT_BOOL                 portWasFound                = GT_FALSE;
    GT_U32                  mirrorAnalyzerEntryIndex    = 0;     
    GT_BOOL                 checkMirrorAnalyzerEntryIndex    
                                                        = GT_FALSE;
    GT_STATUS               status                      = GT_OK;
    GT_CHAR_PTR             error_message               = NULL; 
    int                     returned_param_count        = 0;     
    P_CALLING_CHECKING;
    
    status =  pvrCpssNumberFromLuaGet(L, 3, &checkMirrorAnalyzerEntryIndex, 
                                      &mirrorAnalyzerEntryIndex, 
                                      &error_message);
    
    if (GT_OK == status)
    {
        status = prvCpssDxChNextMirroredPortGet(
                     devNum, checkMirrorAnalyzerEntryIndex, &portNum, 
                     &mirrorAnalyzerEntryIndex, &portWasFound, &error_message 
                     P_CALLING_ACTUAL_DATA);
    }
    
    returned_param_count    +=        
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=        
        pvrCpssNumberToLuaOnConditionPush(status, L, portNum, portWasFound);
    returned_param_count    +=               
        pvrCpssNumberToLuaOnConditionPush(status, L, mirrorAnalyzerEntryIndex, 
                                          portWasFound);
    return returned_param_count;                                              
}


/*******************************************************************************
* wrlCpssDxChMirrorRxPortsListGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of mirrored ports list.
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
*       number of returned to lua elements, status and mirror rx ports array to 
*       lua stack, if no error occures, otherwise status and error message to
*       lua stack 
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChMirrorRxPortsListGet
(
    IN lua_State            *L
)
{
    GT_U8                               devNum                  = 
                                                    (GT_U8)  lua_tonumber(L, 1); 
                                                                    /* devId */ 
    GT_PORT_NUM                         portNum                 = 
                                            (GT_PORT_NUM) lua_tonumber(L, 2); 
                                                                   /* portNum */
    GT_BOOL                             portWasFound            = GT_TRUE;
    GT_U32                              mirrorAnalyzerEntryIndex    
                                                                = 0;
    int                                 mirrorRxPortIndex       = 1;    
    int                                 luaTopElementIndex;
    GT_STATUS                           status                  = GT_OK;
    GT_CHAR_PTR                         error_message           = NULL; 
    int                                 returned_param_count    = 1;     
    P_CALLING_CHECKING;    

    lua_newtable(L);
    luaTopElementIndex = lua_gettop(L);

    while ((GT_OK == status) && (GT_TRUE == portWasFound))
    {
        status = prvCpssDxChNextMirroredPortGet(devNum, GT_FALSE, &portNum,     
                                                &mirrorAnalyzerEntryIndex, 
                                                &portWasFound, &error_message
                                                P_CALLING_ACTUAL_DATA);
                                                
        if (GT_TRUE == portWasFound)
        {
            lua_pushnumber(L, portNum++); 
            lua_rawseti(L, luaTopElementIndex, mirrorRxPortIndex++);
        }
    }    
   
    if (GT_OK != status)
    {
        lua_remove(L, 1);
        returned_param_count--;
    }
    
    returned_param_count    +=       
        prvWrlCpssStatusToLuaAtStackPositionPush(status, L, 1, error_message);
    return returned_param_count;            
}


