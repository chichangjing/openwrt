/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdBase.c
*
* DESCRIPTION:
*       API commander database functionality implementation.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#include <cmdShell/cmdDb/cmdBase.h>
#include <stdlib.h>

/***** Command Registration and Initialization *************************/

#ifdef EX_FAMILY
#define EX_FAMILY_NUM_OF_CMD 1024
#else
#define EX_FAMILY_NUM_OF_CMD 0
#endif

#if (defined DX_FAMILY) || (defined SAL_FAMILY) || (defined CHX_FAMILY)
#define DX_FAMILY_NUM_OF_CMD 1024
#else
#define DX_FAMILY_NUM_OF_CMD 0
#endif

#ifdef MX_FAMILY
#define MX_FAMILY_NUM_OF_CMD 1024
#else
#define MX_FAMILY_NUM_OF_CMD 0
#endif

#ifdef EXMXPM_FAMILY
#define PM_FAMILY_NUM_OF_CMD 1024
#else
#define PM_FAMILY_NUM_OF_CMD 0
#endif

#ifdef TG_FAMILY
#define TG_FAMILY_NUM_OF_CMD 1024
#else
#define TG_FAMILY_NUM_OF_CMD 0
#endif

#ifdef TD_FAMILY
#define TD_FAMILY_NUM_OF_CMD 1024
#else
#define TD_FAMILY_NUM_OF_CMD 0
#endif

#ifdef SOHO_FAMILY
#define SOHO_FAMILY_NUM_OF_CMD 512
#else
#define SOHO_FAMILY_NUM_OF_CMD 0
#endif

#ifdef CPSS_PRODUCT
#define CPSS_API_NUM_OF_CMD_COMMAND (1024 + EX_FAMILY_NUM_OF_CMD    \
                                        + DX_FAMILY_NUM_OF_CMD  \
                                        + MX_FAMILY_NUM_OF_CMD  \
                                        + PM_FAMILY_NUM_OF_CMD  \
                                        + TG_FAMILY_NUM_OF_CMD  \
                                        + TD_FAMILY_NUM_OF_CMD  \
                                        + SOHO_FAMILY_NUM_OF_CMD)
#else
#define CPSS_API_NUM_OF_CMD_COMMAND 0
#endif 

#define NUM_OF_CMD_COMMAND (3072 + EX_FAMILY_NUM_OF_CMD    \
                                + DX_FAMILY_NUM_OF_CMD  \
                                + MX_FAMILY_NUM_OF_CMD  \
                                + PM_FAMILY_NUM_OF_CMD  \
                                + TG_FAMILY_NUM_OF_CMD  \
                                + TD_FAMILY_NUM_OF_CMD  \
                                + SOHO_FAMILY_NUM_OF_CMD    \
                                + CPSS_API_NUM_OF_CMD_COMMAND)

static CMD_COMMAND *dbCommands[NUM_OF_CMD_COMMAND];
static GT_U32 numCommands = 0;

/*******************************************************************************
* databaseSorted
*
* DESCRIPTION:
*                Database sort is delayed.
*                This helps to avoid database sorting for each
*                cmdInitLibrary call
*******************************************************************************/
static GT_BOOL databaseSorted = GT_FALSE;

/*******************************************************************************
* qsortRecordCompareFunc
*
* DESCRIPTION:
*                Compares two CMD_COMMAND records by command name
*                Required by qsort()
*
* INPUTS:
*                a  - pointer to record
*                b  - pointer to record
*
* OUTPUTS:
*       none
*
* RETURNS:
*        integer less than, equal to, or greater than zero if
*        a is less than, equal to, or greater than b
*
* COMMENTS:
*       none
*
*******************************************************************************/
static int qsortRecordCompareFunc(
        const GT_VOID* a,
        const GT_VOID* b
)
{
    return cmdOsStrCmp(
            (*((const CMD_COMMAND**)a))->commandName,
            (*((const CMD_COMMAND**)b))->commandName);
}

/*******************************************************************************
* cmdInitLibrary
*
* DESCRIPTION:
*                insert into        the command database (dbCommands) the recived list of
*                commands
*
* INPUTS:
*                list   - list of commands each entry is function name, function
*                reference and number of function arguments.
                amount - the number of commands to add.
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
GT_STATUS cmdInitLibrary
(
        IN CMD_COMMAND list[],
        IN GT_U32 amount
)
{
    unsigned int index;

    if( numCommands + amount > NUM_OF_CMD_COMMAND)
        return GT_NO_RESOURCE;

    /* store command references */
    for (index = 0; index < amount; index++)
        dbCommands[numCommands++] = &list[index];

    databaseSorted = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cmdGetCommand
*
* DESCRIPTION:
*       performs binery search on the cmdDatabase for finding the right cmd
*                entry
*
* INPUTS:
*       commandName    - null terminated string holding command name
*
* OUTPUTS:
*       command - Poiter to command entry - on success. NULL if not found
*
* RETURNS:
*       GT_OK - on success
*                GT_FAIL - if command database not initialized.
*                GT_NOT_FOUND - if the command was not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdGetCommand
(
        IN const GT_CHAR *commandName,
        OUT CMD_COMMAND** command
)
{
        GT_32 iFirst = 0;
        GT_32 iLast = numCommands;
        GT_32 iMiddle, iRet;

        /* dbCommands never has NULL value. It is pointer to static array
        if (dbCommands == NULL)
            return GT_FAIL;
        */

        if (databaseSorted == GT_FALSE)
        {
            qsort(dbCommands, numCommands, sizeof(dbCommands[0]), qsortRecordCompareFunc);
            databaseSorted = GT_TRUE;
        }

        while (iLast >= iFirst)
        {
                /*iMiddle = (iLast + iFirst) / 2;*/
                iMiddle = iFirst + (iLast - iFirst) / 2;
                if (iMiddle >= (GT_32) numCommands || iMiddle < 0)
                        return GT_NOT_FOUND;

                iRet = cmdOsStrCmp(dbCommands[iMiddle]->commandName, commandName);

                if (iRet > 0)
                        iLast = iMiddle - 1;
                else if (iRet < 0)
                        iFirst = iMiddle + 1;
                else if (iRet == 0)
                {
                        *command = dbCommands[iMiddle];
                        return GT_OK;
                }
        }

        return GT_NOT_FOUND;
}


