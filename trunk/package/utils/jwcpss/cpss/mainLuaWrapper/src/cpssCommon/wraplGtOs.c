/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapGtOs.c
*
* DESCRIPTION:
*       Provides wrappers for input/output facility of operating System wrapper.
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <gtOs/gtOsIo.h>
#include <lua.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
/*******************************************************************************
* Os std output function and its user parameter                                *
*******************************************************************************/
static OS_BIND_STDOUT_FUNC_PTR      writeFunctionPtr;
static GT_VOID_PTR                  writeFunctionParam;


/*******************************************************************************
* wrlOsSetStdOutToNull
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Setting of current std output function to null-output function.
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
*       GT_OK pused to lua stack 
*
* COMMENTS:
*
*******************************************************************************/
int wrlOsResetStdOutFunction
(
    IN lua_State *L
)
{
    GT_STATUS               status;

    status = osBindStdOut(&osNullStdOutFunction, NULL);
    
	lua_pushinteger(L, status);
    return 1;
}


/*******************************************************************************
* wrlOsSaveCurrentStdOutFunction
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Saving of current std output function.
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
*       GT_OK pused to lua stack 
*
* COMMENTS:
*
*******************************************************************************/
int wrlOsSaveCurrentStdOutFunction
(
    IN lua_State *L
)
{
    GT_STATUS               status;

    status = osGetStdOutFunction(&writeFunctionPtr, &writeFunctionParam);
    
	lua_pushinteger(L, status);
    return 1;
}

/*******************************************************************************
* wrlOsRestoreStdOutFunction
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Restoring of std output function to last saved.
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
*       GT_OK pused to lua stack 
*
* COMMENTS:
*
*******************************************************************************/
int wrlOsRestoreStdOutFunction
(
    IN lua_State *L
)
{
    GT_STATUS               status;

    status = osBindStdOut(writeFunctionPtr, writeFunctionParam);
    
	lua_pushinteger(L, status);
    return 1;
}

#ifdef __cplusplus
}
#endif

