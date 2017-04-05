/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* cmdBase.h
*
* DESCRIPTION:
*       commander database api header
*
* DEPENDENCIES:
*       gtOs.h
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*******************************************************************************/

#ifndef __cmdBase_h__
#define __cmdBase_h__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Include files ***************************************************/
#include <cmdShell/common/cmdCommon.h>
#ifdef CPSS_PRODUCT
#  include <cpss/extServices/os/gtOs/gtGenTypes.h>
#endif

/* commander compile-time configuration macros */
#define CMD_MAX_ARGS    256     /* maximum # of arguments       */
#define CMD_MAX_FIELDS  256     /* maximum # of fields          */

/*
 * Typedef: function cmdWrapperFuncPtr
 *
 * Description: definition of the prototype of all the commands wrappper
 *              functions. This prototype should be used when implementing
 *              wrapper functions.
 *
 * INPUTS:
 *      inArgs - an array of command line input parameters
 *               for numeric values it holds the value
 *               for string values it holds a pointer to the input string
 *
 * OUTPUTS:
 *      outArgs - a poiter to a string that should be field with the function
 *                Galtis output.
 */
typedef CMD_STATUS (*cmdWrapperFuncPtr)
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*
 * Typedef: struct CMD_COMMAND
 *
 * Description: definition of single command (api function) used by database
 *              description tables.
 *
 * Fields:
 *      commandName     - The command name
 *      funcReference   - Pointer to the function
 *      funcArgs        - Number of aruments
 *      funcFields      - Number of fields for table related commands
 *
 */
typedef struct
{
    GT_CHAR  *commandName;
    cmdWrapperFuncPtr funcReference;
    GT_U32  funcArgs;
    GT_U32  funcFields;
} CMD_COMMAND;

/*******************************************************************************
* cmdInitDatabase
*
* DESCRIPTION:
*       create the commands database by calling the libraries
*       initialization functions
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_STATUS
*
* COMMENTS:
*       none
*
*******************************************************************************/
#if (defined CPSS_PRODUCT) || (defined PSS_PRODUCT)
GT_STATUS cmdInitDatabase(GT_VOID);
#define cmdCpssInitDatabase cmdInitDatabase
#else
GT_STATUS cmdCpssInitDatabase(GT_VOID);
#endif

/*******************************************************************************
* cmdInitLibrary
*
* DESCRIPTION:
*       add library commands to the global command database
*
* INPUTS:
*       list   - an array of commands data structure
*       amount - the number of command in the commands list
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on failure
*
* COMMENTS:
*       none
*
*******************************************************************************/
GT_STATUS cmdInitLibrary
(
    IN CMD_COMMAND list[],
    IN GT_U32 amount
);

/*******************************************************************************
* cmdGetCommand
*
* DESCRIPTION:
*       performs binary search on the cmdDatabase for finding the right
*       command entry
*
* INPUTS:
*       commandName - null terminated string holding command name
*
* OUTPUTS:
*       command - Pointer to command entry - on success. NULL if not found
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - if command database not initialized.
*       GT_NOT_FOUND - if the command was not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdGetCommand
(
    IN const GT_CHAR *commandName,
    OUT CMD_COMMAND** command
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__cmdBase_h__*/
