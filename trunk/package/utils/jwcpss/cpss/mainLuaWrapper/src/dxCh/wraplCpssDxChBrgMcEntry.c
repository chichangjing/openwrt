/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChBrgFdb.c
*
* DESCRIPTION:
*       A lua wrapper for bridge multicast entries.
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgMc.h>
#include <cpssCommon/wraplCpssTypes.h>
#include <cpssCommon/wrapCpssDebugInfo.h>
#include <generic/private/prvWraplGeneral.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>


/*******************************************************************************
*   Bridge multicast wrapper errors                                         *
*******************************************************************************/  
GT_CHAR fdbMcEntryReadErrorMessage[]    = "Could not read multicast entry";
GT_CHAR fdbMcEntryWriteErrorMessage[]   = "Could not write multicast entry";

/***** declarations ********/

use_prv_struct(CPSS_MAC_ENTRY_EXT_STC)
use_prv_print_struct(CPSS_PORTS_BMP_STC)

/***** declarations ********/


/*******************************************************************************
* prvCpssDxChDoesMulticastGroupIndexExist
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
*       vidx                  - multicast group index
*       P_CALLING_FORMAL_DATA - general debug information (environment variables 
*                               etc); could be empty
*
* OUTPUTS:
*       doesMulticastGroupIndexExist         
*                             - multicast group index existence
*       errorMessagePtr       - error message
*
* RETURNS:
*       operation execution status
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChDoesMulticastGroupIndexExist
(
    IN  GT_U8                   devNum,
    IN  GT_U16                  vidx,
    OUT GT_BOOL                 *doesMulticastGroupIndexExist, 
    OUT GT_CHAR_PTR             *errorMessagePtr
    P_CALLING_FORMAL_DATA    
)
{
    CPSS_PORTS_BMP_STC          portsMembers; 
    GT_STATUS                   status          = GT_OK;

    *doesMulticastGroupIndexExist   = GT_TRUE;
    *errorMessagePtr                = NULL;
    
    CPSS_API_CALL_LOCK();
    status = cpssDxChBrgMcEntryRead(devNum, vidx, &portsMembers);  
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChBrgMcEntryRead,
        PC_NUMBER(devNum,                   devNum,                  GT_U8,   
        PC_NUMBER(vidx,                     vidx,                   GT_U16,           
        PC_STRUCT(&portsMembers,            portBitmap, CPSS_PORTS_BMP_STC,     
        PC_LAST_PARAM))),
        PC_STATUS);
    
    if (GT_OK != status)
    {
        status                          = GT_OK;
        *doesMulticastGroupIndexExist   = GT_FALSE;
    }  
        
    if ((GT_OK == status) && (GT_TRUE == *doesMulticastGroupIndexExist))
    {
        *doesMulticastGroupIndexExist   = 
            GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&portsMembers);
    }
    
    return status;
}


/*******************************************************************************
* wrlCpssDxChBrgFirstEmptyVidxGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Getting of lowest vidx, that does not contain port members.
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
*       2, GT_OK and founded vidx pushed to lua stack, if it was found
*       1, GT_OK pushed to lua stack, if such vidx was not found
*       2, error code and error code pushed to lua stack
*
* COMMENTS:
*
*******************************************************************************/
int wrlCpssDxChBrgFirstEmptyVidxGet
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  = 
                                                    (GT_U8)  lua_tonumber(L, 1); 
                                                                    /* devId */ 
    GT_U16                  vidx                    = 
                                                    (GT_U16) lua_tonumber(L, 2); 
                                                                    /* vidx*/
    GT_BOOL                 isEmptyVidxFound        = GT_FALSE;                                                                    
    CPSS_PORTS_BMP_STC      portsMembers;   
    GT_U16                  floodVidx               = 0;
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;                                                 
    P_CALLING_CHECKING;    

    cpssOsMemSet(&portsMembers, 0, sizeof(portsMembers));
                                                
    floodVidx = (GT_U16) PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_VIDX_MAC(devNum);
                                                
    while ((GT_OK == status) && (GT_FALSE == isEmptyVidxFound) && 
           (floodVidx != vidx) &&
           (vidx < PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.vidxNum))
    {
        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgMcEntryRead(devNum, vidx, &portsMembers);          
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgMcEntryRead,
            PC_NUMBER(devNum,               devNum,                  GT_U8,   
            PC_NUMBER(vidx,                 vidx,                   GT_U16,           
            PC_STRUCT(&portsMembers,        portBitmap, CPSS_PORTS_BMP_STC,     
            PC_LAST_PARAM))),
            PC_STATUS); 
        
        if (GT_OK != status)
        {
            error_message = fdbMcEntryReadErrorMessage;
        }  
        
        if ((GT_OK == status) && CPSS_PORTS_BMP_IS_ZERO_MAC(&portsMembers))
        {
            isEmptyVidxFound = GT_TRUE;
        }
        else
        {
            vidx++;
        }
    }
    
    returned_param_count    +=    
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=        
        pvrCpssNumberToLuaOnConditionPush(status, L, vidx, isEmptyVidxFound);
    return returned_param_count;             
}


/*******************************************************************************
* wrlCpssDxChBrgMcEntryRead
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Reads bridge multicast entry.
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
int wrlCpssDxChBrgMcEntryRead
(
    IN lua_State            *L
)
{
    GT_U8                   devNum                  = 
                                                    (GT_U8)  lua_tonumber(L, 1); 
                                                                    /* devId */ 
    GT_U16                  vidx                    = 
                                                    (GT_U16) lua_tonumber(L, 2); 
                                                                    /* vidx*/
    CPSS_PORTS_BMP_STC      portsMembers;   
    GT_STATUS               status                  = GT_OK;
    GT_CHAR_PTR             error_message           = NULL;
    int                     returned_param_count    = 0;   
    P_CALLING_CHECKING;      
                                                
    cpssOsMemSet(&portsMembers, 0, sizeof(portsMembers));
                                                
    CPSS_API_CALL_LOCK();
    status = cpssDxChBrgMcEntryRead(devNum, vidx, &portsMembers);       
    CPSS_API_CALL_UNLOCK();
    P_CALLING_API(
        cpssDxChBrgMcEntryRead,
        PC_NUMBER(devNum,                   devNum,                  GT_U8,   
        PC_NUMBER(vidx,                     vidx,                   GT_U16,           
        PC_STRUCT(&portsMembers,            portBitmap, CPSS_PORTS_BMP_STC,     
        PC_LAST_PARAM))),
        PC_STATUS);     
    if (GT_OK != status)
    {
        error_message = fdbMcEntryReadErrorMessage;
    }

    if (GT_OK == status)  
    {
    }
    
    returned_param_count    +=     
        prvWrlCpssStatusToLuaPush(status, L, error_message);
    returned_param_count    +=     
        pvrCpssPortsMembersToLuaPush(status, L, &portsMembers);
    return returned_param_count;           
}


/*******************************************************************************
* wrlCpssDxChBrgMcEntryWrite
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Builds and writes bridge multicast entry to HW.
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
)
{
    GT_U8                               devNum  = (GT_U8)  lua_tonumber(L, 1); 
                                                                    /* devId */ 
    GT_U16                              vidx    = (GT_U16) lua_tonumber(L, 2); 
                                                                    /* vidx*/
    CPSS_PORTS_BMP_STC                  portsMembers;   
    GT_STATUS                           status  = GT_OK;
    GT_CHAR_PTR             error_message   = NULL;
    P_CALLING_CHECKING;                                                

    cpssOsMemSet(&portsMembers, 0, sizeof(portsMembers));                                                
    
    status = pvrCpssPortsMembersFromLuaGet(L, 3, &portsMembers, &error_message);

    if (GT_OK == status)  
    {          
        CPSS_API_CALL_LOCK();
        status = cpssDxChBrgMcEntryWrite(devNum, vidx, &portsMembers);       
        CPSS_API_CALL_UNLOCK();
        P_CALLING_API(
            cpssDxChBrgMcEntryWrite,
            PC_NUMBER(devNum,               devNum,                     GT_U8,
            PC_NUMBER(vidx,                 vidx,                      GT_U16,
            PC_STRUCT(&portsMembers,        portBitmap,    CPSS_PORTS_BMP_STC,
            PC_LAST_PARAM))),
            PC_STATUS); 
        if (GT_OK != status)
        {
            error_message = fdbMcEntryWriteErrorMessage;
        }
    }
    
    return prvWrlCpssStatusToLuaPush(status, L, error_message);
}

