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
* os_lxExecute.c
*
* DESCRIPTION:
*       vxworks extended system wrapper layer
*
*       this module implements pipe io and standard io redirection routines
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

/***** Include files ***************************************************/
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include <cmdShell/os/cmdExecute.h>

typedef GT_STATUS (*FUNC_PTR)();
typedef struct
{
    char * name;
    FUNC_PTR function;
} COMMAND_STC;
#define COMMAND(NAME) { #NAME, (FUNC_PTR) NAME}

#if (defined CPSS_PRODUCT) || (defined PSS_PRODUCT)
extern GT_STATUS gtInitSystem();
#else
extern GT_STATUS cpssInitSystem();
#endif
extern GT_STATUS smidwr
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

extern GT_STATUS smidrd
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr
);

extern GT_STATUS smiwr
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
);

extern GT_STATUS smird
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr
);

extern GT_STATUS driverPpRegsPrintRegValSalsa
(
    IN GT_BOOL              inAllDevFlag,
    IN GT_U8                inDevNum,
    IN GT_U32  inAttrib,
    IN GT_U32               inRegOffset
);
extern GT_STATUS ethPortStat(void);
extern GT_STATUS macTblStat(GT_U8 devNum);
extern void macTblDump
(
    GT_U8   devNum ,
    GT_BOOL full
);
GT_STATUS gtVlanTableDump
(
    IN GT_U8 devNum
);
#ifndef SOHO_FAMILY
GT_STATUS hwPpGetRegField
(
    IN GT_U8    devNum,
    IN GT_U32   regAddr,
    IN GT_U32   fieldOffset,
    IN GT_U32   fieldLength,
    OUT GT_U32  *fieldData
);
GT_STATUS hwPpSetRegField
(
    IN GT_U8  devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 fieldData

);
#endif

/*******************************************************************************
* modProcStat
*
* DESCRIPTION:
*       Shows process statistics from /proc/mvKernelExt
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS modProcStat(void)
{
    FILE* f;
    f = fopen("/proc/mvKernelExt", "r");
    if (f == NULL)
    {
        fprintf(stderr,"can't open /proc/mvKernelExt: %s",strerror(errno));
        return GT_FAIL;
    }
    while (!feof(f))
    {
        char buf[200];
        if (fgets(buf, sizeof(buf)-1, f) == NULL)
            break;
        buf[sizeof(buf)-1] = 0;
        fputs(buf, stderr);
    }
    fclose(f);
    return GT_OK;
}

/*******************************************************************************
* modSemStat
*
* DESCRIPTION:
*       Shows semaphores statistics from /proc/mvKernelExtSem
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS modSemStat(void)
{
    FILE* f;
    f = fopen("/proc/mvKernelExtSem", "r");
    if (f == NULL)
    {
        fprintf(stderr,"can't open /proc/mvKernelExtSem: %s",strerror(errno));
        return GT_FAIL;
    }
    while (!feof(f))
    {
        char buf[200];
        if (fgets(buf, sizeof(buf)-1, f) == NULL)
            break;
        buf[sizeof(buf)-1] = 0;
        fputs(buf, stderr);
    }
    fclose(f);
    return GT_OK;
}

/* external symbold required for symbol lookup */
extern int mainArgc;
extern const char** mainArgv;
extern int main();

/*******************************************************************************
* osShellGetFunctionByName
*
* DESCRIPTION:
*       Lookup executable file symbol table for function
*
* INPUTS:
*       funcName - null terminated command string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       NULL        - if function is not found
*       function pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_VOIDFUNCPTR osShellGetFunctionByName
(
    IN  const char* funcName
)
    /* lookup symbol using .map file */
    char buf[200], name[100];
#define BAD_PTR 0xffffffff
    unsigned int mainPtr = BAD_PTR;
    unsigned int symbolPtr = BAD_PTR;
    unsigned int ptr;
    FILE* f;
    

    if (mainArgc < 1 || mainArgv[0] == NULL)
        return NULL;
    sprintf(buf, "%s.map", mainArgv[0]);

    f = fopen(buf, "r");
    if (f == NULL)
    {
        /* debug */
        fprintf(stderr,"can't open %s: %s", buf, strerror(errno));
        return NULL;
    }
    while (!feof(f))
    {
        char buf[200];
        if (fgets(buf, sizeof(buf)-1, f) == NULL)
            break;
        buf[sizeof(buf)-1] = 0;
        sscanf(buf, "%i %s", &ptr, name);
        if (!strcmp(name, "main"))
            mainPtr = ptr;
        if (!strcmp(name, funcName))
            symbolPtr = ptr;
        if (mainPtr != BAD_PTR && symbolPtr != BAD_PTR)
            break;
    }
    fclose(f);
    
    if (mainPtr == BAD_PTR || symbolPtr == BAD_PTR)
        return NULL;

    return (GT_VOIDFUNCPTR)(symbolPtr + (((unsigned int)main) - mainPtr));
}
static COMMAND_STC commands[] =
{
#if (defined CPSS_PRODUCT) || (defined PSS_PRODUCT)
    COMMAND(gtInitSystem),
#else
    COMMAND(cpssInitSystem),
#endif
    COMMAND(smiwr),
    COMMAND(smird),
    COMMAND(smidwr),
    COMMAND(smidrd),
    COMMAND(ethPortStat),
#ifndef SOHO_FAMILY
    COMMAND(hwPpGetRegField),
    COMMAND(hwPpSetRegField),
#endif
#ifdef SALSA_FAMILY
    COMMAND(driverPpRegsPrintRegValSalsa),
    COMMAND(macTblStat),
    COMMAND(macTblDump),
    COMMAND(gtVlanTableDump),
#endif
    COMMAND(modProcStat),
    COMMAND(modSemStat),
    COMMAND(exit)
};

extern char *strtok_r(char *s, const char *delim, char **ptrptr);


/*******************************************************************************
* osShellExecute
*
* DESCRIPTION:
*       execute command through OS shell
*
* INPUTS:
*       command - null terminated command string
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS   osShellExecute
(
    IN  char*   command
)
{
    FUNC_PTR func;
    GT_U8 copy[128];
    GT_U8 ** ptrptr;
    GT_U8 * cmd;
    GT_U8 * tmp;
    GT_U32 argv[32];
    GT_32 argc;
    FILE * fp;
    GT_U32 i;
    extern FILE *popen(const char *command, const char *type);
    extern int pclose(FILE *stream);
    GT_STATUS rc;
    strncpy(copy, command, 127);
    copy[127] = '\0';

    cmd = strtok_r((char *)copy, " \n\r\t,", (char **)&ptrptr);
    if (0 == cmd)
    {
        return GT_FAIL;
    }
    else
    {
        func = 0;
        for (i = 0; i < sizeof(commands)/sizeof(commands[0]); i++)
        {
            if (0 == strcmp(commands[i].name, cmd))
            {
                func = commands[i].function;
                break;
            }
        }
        if (func == 0)
            func = (FUNC_PTR)osShellGetFunctionByName((char*)cmd);
        if (0 != func)
        {
            /* found, now search for arguments */
            argc = 0;
            for (;;)
            {
                tmp = strtok_r(0, " \n\r\t,", (char **)&ptrptr);
                if (0 == tmp)
                {
                    break;
                }
                argv[argc++] = strtol(tmp,0,0);/* converts a character string,to a long integer value*/
            }
            switch (argc)
            {
                case 0:
                rc = (func)();
                break;
                case 1:
                rc = (func)(argv[0]);
                break;
                case 2:
                rc = (func)(argv[0],argv[1]);
                break;
                case 3:
                rc = (func)(argv[0],argv[1],argv[2]);
                break;
                case 4:
                rc = (func)(argv[0],argv[1],argv[2],argv[3]);
                break;
                case 5:
                rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4]);
                break;
                case 6:
                rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
                break;
                case 7:
                rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6]);
                break;
                case 8:
                rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
                        argv[7]);
                break;
                case 9:
                rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
                        argv[7],argv[8]);
                break;
                case 10:
                rc = (func)(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],
                        argv[7],argv[8],argv[9]);
                break;
                default:
                    return GT_FAIL;
            }
            cmdOsPrintf("return code is %d\n", rc);
            return rc;
        }
    }

    fp = popen((const char *)command, "r");
    if (NULL == fp)
    {
        return GT_FAIL;
    }

    while (fgets((char *)copy, sizeof(copy), fp) != NULL)
    {
        cmdOsPrintf(copy);
    }

    pclose(fp);

    return GT_OK;
}

