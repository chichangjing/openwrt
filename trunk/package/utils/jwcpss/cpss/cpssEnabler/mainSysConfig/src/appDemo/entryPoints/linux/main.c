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
* main.c
*
* DESCRIPTION:
*       linux entry point implementation
*
*
* DEPENDENCIES:
*
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/
#define _BSD_SOURCE
#include <gtOs/gtOsSharedMemory.h>
#include <gtOs/gtOsInit.h>
#define CMDOS_NO_CMDCOMMON
#include <cmdShell/os/cmdOs.h>
#ifdef CMD_LUA_CLI
#include <termios.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef SHARED_MEMORY

/*#include <gtOs/gtOsTimer.h>*/
GT_BOOL isSlaveProcess = GT_FALSE;


/*extern GT_STATUS cmdInit(IN GT_U32 devNum);*/

void cmdAppInit(void)
{
/*    cmdInit(0); */
}

#endif


extern int userAppInit(void);

void preUserAppInit(void)
{
    /* here we will map the pss memory */
    
    userAppInit();
}

#if 1
/* in Shared Library mode, we can run a process from the secondary terminal that behaves as appDemo.  The use is:

appDemo -tty -slave
*/
int startEngine(int type)
{
    int i;
#ifdef CMD_LUA_CLI
    struct termios tc_saved;
#endif
/*
#ifdef SHARED_MEMORY
    if(shrMemInitSharedLibrary_FirstClient() != GT_OK)
        return -1;
#endif
*/

#ifdef CMD_LUA_CLI
    cmdDefaultShellType = CMD_STARTUP_SHELL_LUACLI_E;
#endif
#if 0
    for (i=1; i < argc; i++)
    {
#ifdef CMD_LUA_CLI
        if (strcmp("-cmdshell", argv[i]) == 0)
        {
            cmdDefaultShellType = CMD_STARTUP_SHELL_CMDSHELL_E;
        }
        if (strcmp("-luacli", argv[i]) == 0)
        {
            cmdDefaultShellType = CMD_STARTUP_SHELL_LUACLI_E;
        }
        /* support "-config" key to enter the name of the configuration file */
        if ((strcmp("-config", argv[i]) == 0) && (i + 1 < argc))
        {
            i++;
            if (cmdOsConfigFileName != NULL)
            {
                free(cmdOsConfigFileName);
            }
            cmdOsConfigFileName = strdup(argv[i]);
            continue;
        }
#endif
        /* no need to define environment variable PSEUDO to cause simulation
             * print output on local console
            */
         /* new key "-tty" causes all input/output be on local console */
        if (!strcmp(argv[i], "-tty"))
        {
            setenv("PSEUDO", "/dev/tty", 1);
        }
        
#ifdef SHARED_MEMORY
        if (!strcmp(argv[i], "-slave"))
        {
            isSlaveProcess = GT_TRUE;
        }
#endif
    }
#endif
#ifdef CMD_LUA_CLI
    tcgetattr(0, &tc_saved);
#endif


#ifdef SHARED_MEMORY
    if ((GT_TRUE == isSlaveProcess ?
            shrMemInitSharedLibrary()
            : shrMemInitSharedLibrary_FirstClient()) != GT_OK)
    {
        return -1;
    }
    if (osStartEngine(0, (const char**)0, "appDemo",
              GT_TRUE == isSlaveProcess ? 
                      cmdAppInit /* slave process will not init simulation */
                      : preUserAppInit
        ) != GT_OK)
        return 1;
#else
    if (osStartEngine(0, 0, "appDemo", preUserAppInit) != GT_OK)
        return 1;
#endif
#ifdef CMD_LUA_CLI
    tcsetattr(0, TCSAFLUSH, &tc_saved);
    printf("tty settings restored\n");
#endif
    return 0;
}
#endif

