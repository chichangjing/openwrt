/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* appDemoDb.c
*
* DESCRIPTION:
*       App demo database.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <appDemo/sysHwConfig/appDemoDb.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#ifdef IMPL_GALTIS
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#endif /*IMPL_GALTIS*/

/* maximum length of the name of database entry */
#define APP_DEMO_DB_NAME_MAX_LEN_CNS    128

/* maximum size of app demo database */
#define APP_DEMO_DB_MAX_SIZE_CNS        100

/*
 * Typedef: struct APP_DEMO_DB_ENTRY_STC
 *
 * Description:
 *      Holds single app demo databse entry
 *
 * Fields:
 *      name      - name of the database entry
 *      value     - value of the database entry
 *
 */
typedef struct
{
    GT_CHAR name[APP_DEMO_DB_NAME_MAX_LEN_CNS];
    GT_U32  value;

}APP_DEMO_DB_ENTRY_STC;


/* app demo databse */
static APP_DEMO_DB_ENTRY_STC    appDemoDb[APP_DEMO_DB_MAX_SIZE_CNS];
static GT_U32                   appDemoDbSize = 0;
static GT_U32                   initDone = 0;


#ifdef IMPL_GALTIS
/*******************************************************************************
* cmdLibResetAppDemoDb
*
* DESCRIPTION:
*     Library database reset function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID cmdLibResetAppDemoDb
(
    GT_VOID
)
{
    appDemoDbSize = 0;
    initDone = 0;
}
#endif /*IMPL_GALTIS*/

/*******************************************************************************
* appDemoDbEntryAdd
*
* DESCRIPTION:
*       Set AppDemo DataBase value.This value will be considered during system
*       initialization process.
*
* INPUTS:
*       namePtr         - points to parameter name
*       value           - parameter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - if name is too long
*       GT_BAD_PTR      - if NULL pointer
*       GT_NO_RESOURCE  - if Database is full
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoDbEntryAdd
(
    IN  GT_CHAR *namePtr,
    IN  GT_U32  value
)
{
#ifdef IMPL_GALTIS
    GT_STATUS   rc;
#endif /*IMPL_GALTIS*/
    GT_U32 i;

    if(initDone == 0)
    {
#ifdef IMPL_GALTIS
        /* register function to reset DB during system reset */
        rc = wrCpssRegisterResetCb(cmdLibResetAppDemoDb);
        if(rc != GT_OK)
        {
            return rc;
        }
#endif /*IMPL_GALTIS*/
        initDone = 1;
    }

    /* check parameters */
    if (namePtr == NULL)
        return GT_BAD_PTR;

    /* check database is not full */
    if (appDemoDbSize >= APP_DEMO_DB_MAX_SIZE_CNS)
        return GT_NO_RESOURCE;

    /* check name is not too long */
    if (cpssOsStrlen(namePtr) > APP_DEMO_DB_NAME_MAX_LEN_CNS)
        return GT_BAD_PARAM;

    /* search if the name already exists, if so override the value */
    for (i = 0 ; i < appDemoDbSize ; i++)
    {
        if (cpssOsStrCmp(namePtr, appDemoDb[i].name) == 0)
        {
            appDemoDb[i].value = value;
            return GT_OK;
        }
    }

    /* the entry wasn't already in database, add it */
    cpssOsStrCpy(appDemoDb[appDemoDbSize].name, namePtr);
    appDemoDb[appDemoDbSize].value = value;
    appDemoDbSize++;

    return GT_OK;
}


/******************************************************************************
* appDemoDbEntryGet
*
* DESCRIPTION:
*       Get parameter value from AppDemo DataBase.
*
* INPUTS:
*       namePtr         - points to parameter name
*
* OUTPUTS:
*       valuePtr        - points to parameter value
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - if name is too long
*       GT_BAD_PTR      - if NULL pointer
*       GT_NO_SUCH      - there is no such parameter in Database
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDbEntryGet
(
    IN  GT_CHAR *namePtr,
    OUT GT_U32  *valuePtr
)
{
    GT_U32 i;

    /* check parameters */
    if ((namePtr == NULL) || (valuePtr == NULL))
        return GT_BAD_PTR;

    /* check name is not too long */
    if (cpssOsStrlen(namePtr) > APP_DEMO_DB_NAME_MAX_LEN_CNS)
        return GT_BAD_PARAM;

    /* search the name in the databse */
    for (i = 0 ; i < appDemoDbSize ; i++)
    {
        if (cpssOsStrCmp(namePtr, appDemoDb[i].name) == 0)
        {
            *valuePtr = appDemoDb[i].value;
            return GT_OK;
        }
    }

    /* the entry wasn't found */
    return GT_NO_SUCH;
}


/******************************************************************************
* appDemoDbDump
*
* DESCRIPTION:
*       Dumps entries set in AppDemo database to console.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*
* COMMENTS:
*       None.
*
******************************************************************************/
GT_STATUS appDemoDbDump
(
    GT_VOID
)
{
    GT_U32 i;

    /* print header */
    cpssOsPrintf("App Demo Database Dump:\n");
    cpssOsPrintf("");

    /* check if database is empty */
    if (appDemoDbSize == 0)
    {
        cpssOsPrintf("App Demo Database is empty\n");
        return GT_OK;
    }

    /* print database values */
    for (i = 0 ; i < appDemoDbSize ; i++)
    {
        cpssOsPrintf("[%3d] %s = %d\n", i+1, appDemoDb[i].name, appDemoDb[i].value);
    }

    return GT_OK;
}

