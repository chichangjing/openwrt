/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplTFTPC.c
*
* DESCRIPTION:
*       A lua wrapper for cmdEventFiedld(), cmdEventRun()
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
**************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/shell/cmdEngine.h>
#include <cmdShell/common/cmdWrapUtils.h>

#include <lua.h>


/************* Defines ********************************************************/

/************* Typedefs *******************************************************/

/************* Externs ********************************************************/

/************* Functions ******************************************************/

/*******************************************************************************
* wrapl_cmdEventFields: lua function
*
* DESCRIPTION:
*       parses input buffer, and tells whether command has fields as input
*
* INPUTS:
*       inBuffer   - string holding command buffer
*
* RETURNS:
*       true       - fields need to be read
*       false      - command has no fields as input
*       nil if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int wrapl_cmdEventFields
(
    lua_State *L
)
{
    GT_BOOL rc;
    GT_CHAR inputBuffer[4 * CMD_MAX_ARGS] = {0};

    if (!lua_isstring(L,1))
        return 0; /* nil */
    cmdOsStrCpy(inputBuffer, lua_tostring(L, 1));
    rc = cmdEventFields(inputBuffer);
    lua_pushboolean(L, (rc == GT_TRUE)?1:0);
    return 1;
}


/*******************************************************************************
* wrapl_cmdEventRun: lua function
*
* DESCRIPTION:
*       command interpreter; parses and executes single command stored
*       in a string
*
* INPUTS:
*       inBuffer   - string holding command buffer
*       inFieldBuffer - string holding field values
*
* RETURNS:
*       true,output  - on success
*       false        - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
int wrapl_cmdEventRun
(
    lua_State *L
)
{
    GT_CHAR *outputBuffer;
    GT_CHAR inputBuffer[4 * CMD_MAX_ARGS] = {0};
    GT_CHAR fieldBuffer[4 * CMD_MAX_FIELDS] = {0};
    CMD_STATUS status;

    if (!lua_isstring(L, 1))
        return 0;
    cmdOsStrCpy(inputBuffer, lua_tostring(L, 1));
    if (lua_isstring(L, 2))
        cmdOsStrCpy(fieldBuffer, lua_tostring(L, 2));

    outputBuffer = NULL;
    /* execute command: pass command buffer to cmdEventRun */
    status = cmdEventRun(inputBuffer, fieldBuffer, &outputBuffer);

    if (status == CMD_OK)
    {
        lua_pushboolean(L, 1);
        if (outputBuffer != NULL)
            lua_pushstring(L, outputBuffer);
    }
    else
    {
        GT_CHAR galtisError[64];
        galtisOutput((GT_8*)galtisError, status, "");
        lua_pushboolean(L, 0);
        lua_pushstring(L, galtisError);
    }

    return 2;
    
}
