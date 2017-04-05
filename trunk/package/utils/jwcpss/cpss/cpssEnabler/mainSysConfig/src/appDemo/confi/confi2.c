/*******************************************************************************
*         Copyright 2004, MARVELL SEMICONDUCTOR ISRAEL, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL SEMICONDUCTOR ISRAEL. (MSIL),  MARVELL TAIWAN, LTD. AND      *
* SYSKONNECT GMBH.                                                             *
********************************************************************************
* confi2.c
*
* DESCRIPTION:
*       Contains API for configuration script processing.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/

#include <appDemo/confi/confi2.h>
#include <cmdShell/shell/cmdParser.h>
#include <cmdShell/shell/cmdEngine.h>
#include <cmdShell/shell/cmdConsole.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <appDemo/os/appOs.h>
#include <cpss/extServices/cpssExtServices.h>
/*#include <gtOs/gtOsMem.h>*/
/*#include <gtOs/gtOsStr.h>*/
/*#include <gtExtDrv/drivers/gtFlashDrv.h>*/
/*#include <appDemo/confi/gtZlib.h>*/
#include <stdio.h>

#define CONFI_VERSION           "v1.0"
/* initial size of confi buffer */
#define CONFI_BUFFER_INI_SIZE   (128 * 1024)
/* size of confi signature      */
#define CONFI_SIGNATURE_SIZE    16

#define CONFI_TASK_PRIO         32
#ifndef errorMessage
#define errorMessage osPrintf
#endif

#ifndef CONFI_FLASH_MAX_MEM_SIZE
#define CONFI_FLASH_MAX_MEM_SIZE (128 * 1024)
#endif

#ifndef CONFI_FLASH_NUM_OF_CONFIGS
#define CONFI_FLASH_NUM_OF_CONFIGS 10
#endif

/************* Locally defined functions *************/
/* parse contents of the confi buffer */
static GT_STATUS    confi2Parse();
#if 0
/* read data from flash */
static GT_STATUS confiFlashReadInt
(
    IN const    GT_U32      theOffset,
    IN const    GT_U32      theLength,
    OUT         GT_CHAR     *pTheBuffer,
    IN          GT_CHAR     theConfiId
);
/* write data to flash */
static GT_STATUS confiFlashWriteInt
(
    IN          GT_CHAR     *pTheAddress,
    IN const    GT_U32      theLength,
    IN          GT_CHAR     theConfiId
);
#endif
/* get new line from confi buffer */
static GT_CHAR*   confiGetNewLine
(
    IN  GT_CHAR*  curLine
);

/* get new command from buffer */
static GT_STATUS confiGetNewCommand
(
    INOUT  GT_CHAR   **buf,
    OUT    GT_CHAR   **inBuffer,
    OUT    GT_CHAR   **inFieldBuffer
);

/* check script header */
static GT_STATUS confiCheckHeader
(
    IN  GT_CHAR *line
);

/* Filter log output */
static GT_STATUS confi2LogFltr
(
    IN  GT_CHAR  *srcLine,
    OUT GT_CHAR  *outLine
);

/* buffer contains configuration script */
GT_CONFI_BUFFER  confiBuffer;

static GT_STATUS    confiTaskRetVal; /* confi task rturn value */

/* Global variables */
GT_BOOL confi2LogFltrEnable = GT_TRUE; /* enable/disable filtering log output */
/* Read confi2 script from specified stream.
*  If stream == NULL read from default stream
*/
#if 0
#ifndef WIN32
static
#endif
GT_STATUS confi2ReadExt
(
    IN GT_STREAM    *stream
)
{
    GT_CHAR     confiLine[CMD_MAX_BUFFER+1]; /* buffer for one read line */
    GT_U32      confiLineLen;  /* length of read line */
    GT_STATUS   retVal = GT_OK;/* returned value      */
    GT_CHAR*    pNumberSymbolPtr;   /* pointer to the number             */
                                    /* symbol that represents comments   */
    GT_CHAR*    pConfiStr;          /* pointer to Confi string           */

    /* release old buffer */
    if(confiBuffer.buf != NULL)
    {
        osFree(confiBuffer.buf);
    }

    /* allocate confi buffer */
    confiBuffer.buf = (GT_CHAR*)osMalloc(CONFI_BUFFER_INI_SIZE);
    if(confiBuffer.buf == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* init buffer structure */
    confiBuffer.bufSize = CONFI_BUFFER_INI_SIZE;
    confiBuffer.dataSize = 0;

    /* Set buffer to null string */
    confiLine[0] = '\0';
    /* read line from input stream */
    if(stream != NULL)
    {
        if(stream->flags & STREAM_FILE)
        {
            osFgets(confiLine,CMD_MAX_BUFFER, stream->handle);
        }
        else
        {
            confiLineLen = CMD_MAX_BUFFER;
            retVal = cmdStreamRead(stream, -1, confiLine, &confiLineLen);
        }
    }
    else
    {
        retVal = cmdConsolReadLine(confiLine);
    }
    if(retVal != GT_OK)
    {
        return retVal;
    }

    while (confiLine[0] != '.')
    {
        /* remove comments */
        pNumberSymbolPtr = osStrChr(confiLine, ';');
        /* if # is found somewhere in the line */
        if (pNumberSymbolPtr != NULL)
        {
            /* truncate all the rest of the line */
            *pNumberSymbolPtr = '\0';
        }

        /* Trim leading whitespace characters from the string */
        pConfiStr = confiLine;
        while((*pConfiStr <= 0x20) && (*pConfiStr != '\0'))
        {
            pConfiStr++;
        }

        confiLineLen = osStrlen(pConfiStr);


        /* check whether there is enough space in buffer */
        if(confiBuffer.dataSize + confiLineLen + 1> confiBuffer.bufSize)
        {
            /* increase size of buffer x2 */
            confiBuffer.buf = (GT_CHAR*)osRealloc(confiBuffer.buf,
                                                confiBuffer.bufSize << 1);
            if(confiBuffer.buf == NULL)
            {
                return GT_OUT_OF_CPU_MEM;
            }
            confiBuffer.bufSize <<= 1;
        }

        /* copy line to confi buffer */
        if(confiLineLen > 0)
        {
            osStrCpy(confiBuffer.buf + confiBuffer.dataSize, pConfiStr);
            confiBuffer.dataSize += confiLineLen + 1;
        }

        /* Set buffer to null string */
        confiLine[0] = '\0';
        /* read line from input stream */
        /* read line from input stream */
        if(stream != NULL)
        {
            if(stream->flags & STREAM_FILE)
            {
                if(osFgets(confiLine,CMD_MAX_BUFFER, stream->handle) == NULL)
                {
                    break;
                }
            }
            else
            {
                confiLineLen = CMD_MAX_BUFFER;
                retVal = cmdStreamRead(stream, -1, confiLine, &confiLineLen);
            }
        }
        else
        {
            retVal = cmdConsolReadLine(confiLine);
        }
        if(retVal != GT_OK)
        {
            return retVal;
        }
    }

    /* parse the loaded confi */
    if(confi2Parse() != GT_OK)
    {
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* confiRead
*
* DESCRIPTION:
*       Read configuration script from input stream.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_OUT_OF_CPU_MEM - if out of memory
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   confi2Read()
{
    return confi2ReadExt(NULL);
}


/*******************************************************************************
* confi2FileRead
*
* DESCRIPTION:
*       Read configuration script from file.
*
* INPUTS:
*       filename - filename.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   confi2FileRead
(
    IN  GT_CHAR   *filename
)
{
    GT_STREAM  *stream;  /* input stream is file */
    GT_STATUS  retVal;  /* returned value */


    retVal = cmdStreamCreate(STREAM_FILE, (GT_U32)filename, &stream);
    if(retVal != GT_OK)
    {
        return retVal;
    }

    /* read confi from file stream */
    retVal = confi2ReadExt(stream);

    cmdStreamDestroy(stream);

    if(retVal != GT_OK)
    {
        return retVal;
    }

    return GT_OK;
}

#endif

/*******************************************************************************
* confiParse
*
* DESCRIPTION:
*       Parse configuration script loaded to buffer.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if there are no problems,
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS confi2Parse()
{
    GT_CHAR      *curLine;          /* current line       */
    GT_CHAR      *inBuffer;         /* input buffer       */
    GT_CHAR      *inFieldBuffer;    /* input field buffer */
    CMD_STATUS   cmdStatus;         /* status of parsing of command     */
    CMD_STATUS   retStatus = CMD_OK;
    GT_BOOL      isShellCmd;        /* is shell command  */
#if 0
    /* allocate memory */
    inBuffer = (GT_CHAR*)osMalloc(CMD_MAX_BUFFER);
    if(inBuffer == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    inFieldBuffer = (GT_CHAR*)osMalloc(CMD_MAX_BUFFER);
    if(inFieldBuffer == NULL)
    {
        osFree(inBuffer);
        return GT_OUT_OF_CPU_MEM;
    }

    /* get the first line, it should be header */
    curLine = confiGetNewLine((GT_CHAR*)NULL);
    if(confiCheckHeader(curLine) != GT_OK)
    {
        errorMessage("Script header is wrong\n");
        osFree(inBuffer);
        osFree(inFieldBuffer);
        return GT_FAIL;
    }

    while((curLine + osStrlen(curLine)) + 1 <
          (confiBuffer.buf + confiBuffer.dataSize))
    {
        /* move to next line */
        curLine = confiGetNewLine(curLine);
        /* check if we not cross the boundary */
        if(curLine > confiBuffer.buf + confiBuffer.dataSize)
        {
            osFree(inBuffer);
            osFree(inFieldBuffer);
            return GT_OUT_OF_RANGE;

        }

        confiGetNewCommand(&curLine, &inBuffer,&inFieldBuffer);

        cmdStatus = cmdParseLine(inBuffer,inFieldBuffer, &isShellCmd);
        if(cmdStatus != CMD_OK)
        {
            /* store last failed status */
            retStatus = cmdStatus;
            switch(cmdStatus)
            {
            case CMD_AGENT_ERROR:
                errorMessage("Command shell error\n");
                osFree(inBuffer);
                osFree(inFieldBuffer);
                return (GT_STATUS)GT_ERROR;
            case CMD_SYNTAX_ERROR:
                errorMessage("Syntax error in string %s\n", inBuffer);
                break;
            case CMD_ARG_UNDERFLOW:
                errorMessage("Parser: number of arguments in command"
                                    " %s is mismatched\n", inBuffer);
                break;
            default:
                continue;

            }
        }
        else
        {
            if(isShellCmd == GT_TRUE)
            {
                osPrintf("WARNING: the command %s is not found in Galtis DB\n",
                         inBuffer);
            }
        }
    }

#if 0
    /* get the first line, it should be header */
    fgets(lineBuf, 512, confiFilePtr);
    lineBuf[0] = '\0';
    while((fgets(lineBuf, 512, confiFilePtr)))
    {
        confiGetNewCommand(&curLine, &inBuffer,&inFieldBuffer);

        cmdStatus = cmdParseLine(inBuffer,inFieldBuffer, &isShellCmd);
        if(cmdStatus != CMD_OK)
        {
            /* store last failed status */
            retStatus = cmdStatus;
            switch(cmdStatus)
            {
            case CMD_AGENT_ERROR:
                errorMessage("Command shell error\n");
                osFree(inBuffer);
                osFree(inFieldBuffer);
                return GT_ERROR;
            case CMD_SYNTAX_ERROR:
                errorMessage("Syntax error in string %s\n", inBuffer);
                break;
            case CMD_ARG_UNDERFLOW:
                errorMessage("Parser: number of arguments in command"
                                    " %s is mismatched\n", inBuffer);
                break;
            default:
                continue;

            }
        }
        else
        {
            if(isShellCmd == GT_TRUE)
            {
                osPrintf("WARNING: the command %s is not found in Galtis DB\n",
                         inBuffer);
            }
        }
        lineBuf[0] = '\0';
    }
#endif
    osFree(inBuffer);
    osFree(inFieldBuffer);
#endif
    return (retStatus == CMD_OK ?GT_OK : GT_ERROR);
}

/*******************************************************************************
* confiRestore
*
* DESCRIPTION:
*       Restore the confi binary format from the flash
*
* INPUTS:
*       theConfiId  - confi ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                   - succeeded
*       GT_OUT_OF_CPU_MEM       - out of CPU memory
*       GT_ERROR                - failed
*
* COMMENTS:
*       Public.
*
*******************************************************************************/
GT_STATUS confi2Restore
(
    IN GT_U8        theConfiId
)
{
    GT_STATUS    rc = GT_OK;

#ifndef ASIC_SIMULATION
#if 0
    GT_CHAR   *pBuffer;         /* internal buffer        */
    GT_U32  comprSize;          /* compressed confi size  */
    GT_U32  uncomprSize;        /* uncompressed confiSize */
    GT_U32  destLen;            /* the total size of the destination buffer */
#endif
#endif

#ifdef ASIC_SIMULATION
     GT_CHAR    signature[CONFI_SIGNATURE_SIZE];
     FILE       *confiFilePtr;  /* file object */
     GT_CHAR    file_name[] = "confi2.txt";
     GT_U32     lSize;
     GT_U32     result;
     GT_CHAR    *tmpRes;        /* temp result for fgets to avoid warnings */
#endif

    /* Check the section number */
    if ((theConfiId == 0) || (theConfiId > CONFI_FLASH_NUM_OF_CONFIGS))
    {
        return GT_BAD_PARAM;
    }

#ifndef ASIC_SIMULATION
   #if 0
    /* read confi signature */
    if (GT_OK != confiFlashReadInt(0, CONFI_SIGNATURE_SIZE, signature,
                                   theConfiId))
    {
        errorMessage ("confiFlash error: gtExtDrvFlashReadBlock failed\n");
        return GT_ERROR;
    }

    /* check signature */
    if(confiCheckHeader(signature) != GT_OK)
    {
        errorMessage("confiFlash error: signature is wrong\n");
        return GT_ERROR;
    }

    /* extract compressed confi size */
    comprSize = *(GT_U32*)(signature + 8);
    /* extract uncompressed confi size */
    uncomprSize = *(GT_U32*)(signature + 12);

    /* allocate the internal buffer to write it into the ram */
    pBuffer = (GT_CHAR *)osMalloc (comprSize);

    if (NULL == pBuffer)
    {
        errorMessage("confiRestore error: it could not"
                     " allocate buffer of size %d\n",comprSize);
        return GT_OUT_OF_CPU_MEM;
    }

    /* allocate room in confi buffer */
    if(confiBuffer.buf != NULL)
    {
        /* free previous confi */
        osFree(confiBuffer.buf);
    }

    confiBuffer.buf = (GT_CHAR*)osMalloc(uncomprSize);
    if (NULL == confiBuffer.buf)
    {
        errorMessage("confiRestore error: it could not"
                     " allocate buffer of size %d\n", uncomprSize);
        osFree(pBuffer);
        return GT_OUT_OF_CPU_MEM;
    }

    confiBuffer.bufSize = uncomprSize;

    /* read all the flash into the internal buffer */
    if (GT_OK != confiFlashReadInt(CONFI_SIGNATURE_SIZE, comprSize, pBuffer,
                                   theConfiId))
    {
        errorMessage ("confiFlash error: gtExtDrvFlashReadBlock failed\n");
        osFree(pBuffer);
        return GT_ERROR;
    }

   /* uncompress data */
   destLen = uncomprSize;
   rc = gtUncompress(confiBuffer.buf, &destLen, pBuffer, comprSize);
   if(rc != Z_OK)
   {
       errorMessage ("uncompress of flash is failed\n");
       osFree(pBuffer);
       osFree(confiBuffer.buf);
       confiBuffer.buf = (GT_CHAR*)NULL;
       return GT_ERROR;

   }

   confiBuffer.dataSize = uncomprSize;
   osFree(pBuffer);
   #endif
#endif
#ifdef ASIC_SIMULATION
    /* open file */
    confiFilePtr = fopen(file_name,"rt");
    if (confiFilePtr == NULL)
    {
        errorMessage("confi read error: confi file not found %s\n", file_name);
        return GT_ERROR;
    }
    /* obtain file size: */
    fseek (confiFilePtr , 0 , SEEK_END);
    lSize = ftell (confiFilePtr);
    rewind (confiFilePtr);
    /* allocate memory to contain the whole file: */
    confiBuffer.buf = (GT_CHAR*)osMalloc(sizeof(GT_CHAR)*lSize);
    if (confiBuffer.buf == NULL)
    {
        fclose(confiFilePtr);
        return GT_OUT_OF_CPU_MEM;
    }
    tmpRes = fgets(signature, CONFI_SIGNATURE_SIZE, confiFilePtr);
    /* check signature */
    if(tmpRes == 0 || confiCheckHeader(signature) != GT_OK)
    {
        fclose(confiFilePtr);
        errorMessage("confiFlash error: signature is wrong\n");
        return GT_ERROR;
    }
    rewind (confiFilePtr);
    /* copy the file into the buffer: */
    result = (GT_U32)fread (confiBuffer.buf,1,lSize,confiFilePtr);
    if (result != lSize)
    {
        fclose(confiFilePtr);
        return GT_BAD_SIZE;
    }
    /* the whole file is now loaded in the memory buffer */
    confiBuffer.bufSize = lSize;
    fclose(confiFilePtr);
    confiBuffer.dataSize = lSize;
#endif
   /* parse the loaded confi */
   if(confi2Parse() != GT_OK)
   {
       return GT_FAIL;
   }

   return rc;
}

#if 0
/*******************************************************************************
* confiStore
*
* DESCRIPTION:
*       Store the confi binary format to the flash
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                   - succeeded
*       GT_OUT_OF_CPU_MEM       - out of CPU memory
*       GT_ERROR                - failed
*
* COMMENTS:
*       Public.
*
*******************************************************************************/
GT_STATUS confi2Store
(
    IN GT_CHAR        theConfiId
)
{
    GT_CHAR *pBuffer;         /* internal buffer        */
    GT_U32  comprSize;          /* compressed confi size  */
    int     rc;                 /* return code            */

    /* Check the section number */
    if ((theConfiId == 0) || (theConfiId > CONFI_FLASH_NUM_OF_CONFIGS))
    {
        return GT_BAD_PARAM;
    }

    /* check if confi loaded */
    if(confiBuffer.buf == NULL)
    {
        return GT_ERROR;
    }

    /* allocate buffer for compressed confi */
    comprSize = confiBuffer.dataSize +  (confiBuffer.dataSize >> 2) + 12 +
                CONFI_SIGNATURE_SIZE;
    pBuffer = (GT_CHAR*)osMalloc(comprSize);
    if (NULL == pBuffer)
    {
        errorMessage("confiStore error: it could not"
                     " allocate buffer of size %d\n", comprSize);
        return GT_OUT_OF_CPU_MEM;
    }

    /* compress data */
    rc = gtCompress(pBuffer + CONFI_SIGNATURE_SIZE, &comprSize,
                  confiBuffer.buf, confiBuffer.dataSize);
    if(rc != GT_OK)
    {
        errorMessage ("compress of confi is failed\n");
        osFree(pBuffer);
        return GT_ERROR;
    }

    if(comprSize > CONFI_FLASH_MAX_MEM_SIZE)
    {
        errorMessage ("size of compressed of confi is large\n");
        osFree(pBuffer);
        return GT_ERROR;
    }

    /* prepend signature */
    pBuffer[0] = 'C';
    pBuffer[1] = 'T';
    pBuffer[2] = ' ';
    osStrNCpy(pBuffer + 3, CONFI_VERSION, 4);
    *(GT_U32*)(pBuffer + 8) = comprSize;
    *(GT_U32*)(pBuffer + 12) = confiBuffer.dataSize;

    /* write the ram contents to the flash */
    if (GT_OK != confiFlashWriteInt(pBuffer, comprSize + CONFI_SIGNATURE_SIZE,
                                    theConfiId))
    {
        errorMessage("confiFlash error: confiFlashWrite failed writing"
                     " data of size 0x%x to flash\n", comprSize);
        osFree(pBuffer);
        return GT_FAIL;
    }
    /* finish successfully */
    osFree(pBuffer);
    return GT_OK;
}
#endif

/*******************************************************************************
* confiPrint
*
* DESCRIPTION:
*       Writes the contents of the confi buffer to the standard output.
*
* INPUTS:
*       - None
*
* OUTPUTS:
*       - None
*
* RETURNS:
*       GT_OK           - finished successfully
*       GT_ERROR        - error
*
* COMMENTS:
*
*       Public
*
*******************************************************************************/
GT_STATUS confi2Print
(
    GT_VOID
)
{
    GT_32    nChars;     /* number of printed chars */
    GT_CHAR  *curLine;   /* pointer to current line */
    GT_U32   printLength;/* printed length          */


    if(confiBuffer.buf == NULL)
    {
        errorMessage("confi is not loaded\n");
        return GT_FAIL;
    }

    printLength = 0;
    curLine = (GT_CHAR*)NULL;
    while(printLength < confiBuffer.dataSize)
    {
        /* move to the next line */
        curLine = confiGetNewLine(curLine);
        nChars = osPrintf("%s\n", curLine);
        if(nChars < 0)
        {
            return (GT_STATUS)GT_ERROR;
        }
        printLength += nChars;
    }

   return GT_OK;
}
/*******************************************************************************
* confiRunTask
*
* DESCRIPTION:
*       Run confi script.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK - on success
*       GT_FAIL - on fialure
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS __TASKCONV confiRunTask(GT_VOID *params)
{
    CMD_STATUS     status;            /* returned status          */
    GT_U32         retVal;            /* returned value           */
    GT_CHAR        *inBuffer;         /* input buffer             */
    GT_CHAR        *inFieldBuffer;    /* input field buffer       */
    GT_CHAR        *outputBuffer;     /* output buffer            */
    GT_CHAR        *endptr;           /* pointer to end of string */
    GT_CHAR        *curLine;          /* current line             */
    GT_BOOL        echoOn = (GT_BOOL)params;
    GT_CHAR        logLine[256];      /* Log line                 */
#if 0
    osPrintf("\nConfi script started");
    /* allocate memory */
    inBuffer = (GT_CHAR*)osMalloc(CMD_MAX_BUFFER);
    if(inBuffer == NULL)
    {
        confiTaskRetVal = GT_OUT_OF_CPU_MEM;
        /*cmdEventLoopUnlock();*/
        return GT_OUT_OF_CPU_MEM;
    }

    inFieldBuffer = (GT_CHAR*)osMalloc(CMD_MAX_BUFFER);
    if(inFieldBuffer == NULL)
    {
        confiTaskRetVal = GT_OUT_OF_CPU_MEM;
        osFree(inBuffer);
       /* cmdEventLoopUnlock();*/
        return GT_OUT_OF_CPU_MEM;
    }

    /* get the first line, it should be header */
    curLine = confiGetNewLine((GT_CHAR*)NULL);
    retVal = confiCheckHeader(curLine);
    if(retVal != GT_OK)
    {
        confiTaskRetVal = GT_FAIL;
        osPrintf("\nScript header is wrong");
        osFree(inBuffer);
        osFree(inFieldBuffer);
       /* cmdEventLoopUnlock();*/
        return GT_FAIL;
    }
    while((curLine + osStrlen(curLine)) + 1 <
        (confiBuffer.buf + confiBuffer.dataSize))
    {
        /* move to next line */
        curLine = confiGetNewLine(curLine);
        /* check if we not cross the boundary */
        if(curLine > confiBuffer.buf + confiBuffer.dataSize)
        {
            confiTaskRetVal = GT_OUT_OF_RANGE;
            osPrintf("\nIt's too big");
            osFree(inBuffer);
            osFree(inFieldBuffer);
            /*cmdEventLoopUnlock();*/
            return GT_OUT_OF_RANGE;
        }

        confiGetNewCommand(&curLine, &inBuffer,&inFieldBuffer);

        /* echo the executed string */
        if(echoOn == GT_TRUE)
        {
            osPrintf("\n%s%s", inBuffer, inFieldBuffer);
        }
        outputBuffer = (GT_CHAR*)NULL;
        /* execute command: pass command buffer to cmdEventRun */
        status = cmdEventRun(inBuffer, inFieldBuffer, &outputBuffer);
        /* check error code returned by cmdEventRun */
        if (status == CMD_OK)
        {
            /* check whether output buffer contains any data */
            if ((outputBuffer != NULL) && (osStrlen(outputBuffer) != 0))
            {
                /* get returned status */
                retVal = osStrToU32(outputBuffer + 3, &endptr, 16);
                if(echoOn == GT_TRUE)
                {
                    if(confi2LogFltrEnable == GT_TRUE)
                    {
                        confi2LogFltr(outputBuffer, logLine);
                        osPrintf("\nOutput: %s", logLine);
                    }
                    else
                    {
                        osPrintf("\nOutput: %s", outputBuffer);
                    }

                }
                if(retVal != GT_OK)
                {
                    confiTaskRetVal = GT_FAIL;
                    osPrintf("\nCommand %s%s failed with status %d",inBuffer, inFieldBuffer,retVal);
                    osFree(inBuffer);
                    osFree(inFieldBuffer);
                    /*cmdEventLoopUnlock();*/
                    return GT_FAIL;
                }
            }
        }
        else
        {
            confiTaskRetVal = GT_FAIL;
            if(echoOn == GT_TRUE)
            {
                osPrintf("\nFailed with status = 0x%x", status);
            }
            osFree(inBuffer);
            osFree(inFieldBuffer);
            /*cmdEventLoopUnlock();*/
            return GT_FAIL;
        }
    }
    osPrintf("\nConfi script finished successfully\n");
    confiTaskRetVal = GT_OK;
    osFree(inBuffer);
    osFree(inFieldBuffer);
   /* cmdEventLoopUnlock();*/
#endif
    return GT_OK;
}

/*******************************************************************************
* confi2InitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       theConfiId  - the confi ID
*       echoOn      - if GT_TRUE the echo is on, otherwise the echo is off.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS   confi2InitSystem
(
    IN GT_U8        theConfiId,
    IN GT_BOOL      echoOn
)
{
 #if 0
    GT_TASK      tid;
    GT_STATUS    rc;                /* returned code */
    static GT_BOOL initParam = GT_FALSE;
    return GT_NOT_SUPPORTED;

    if(initParam == GT_FALSE)
    {
        initParam = GT_TRUE;
    }

    if(theConfiId == 0)
    {
        /* check if confi was loaded */
        if(confiBuffer.buf == NULL)
        {
            errorMessage("confi is not loaded\n");
            return GT_FAIL;
        }
    }
    else
    {
        /* Load confi script */
        rc = confi2Restore(theConfiId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* run task */
    rc = osTaskCreate("confiRun",
                      CONFI_TASK_PRIO,
                      0,
                      confiRunTask,
                      (GT_VOID*)echoOn,
                      &tid);

    if(rc != GT_OK)
    {
        return rc;
    }

   /* cmdEventLoopLock();*/
    confiTaskRetVal = GT_OK;
    return confiTaskRetVal;
 #endif
    return GT_NOT_SUPPORTED;
}

#if 0
/*******************************************************************************
* confiFlashReadInt
*
* DESCRIPTION:
*       Read confi binary format from the flash, using the external driver API
*
* INPUTS:
*       theOffset           - the flash offset to read from
*       theLength           - the memory length
*
* OUTPUTS:
*       pTheBuffer          - the memory to read from the flash
*
* RETURNS:
*       GT_OK           - succeeded
*       GT_ERROR        - failed
*
* COMMENTS:
*       Private.
*
*******************************************************************************/
static GT_STATUS confiFlashReadInt
(
    IN const    GT_U32      theOffset,
    IN const    GT_U32      theLength,
    OUT         GT_CHAR     *pTheBuffer,
    IN          GT_CHAR     theConfiId
)
{
#ifdef WIN32
/* flash is not supported in win32 environment */
    return GT_OK;
#else /* WIN32 */
    GT_U32          read;           /* number of bytes read from the flash  */
    GT_U32          sectorOffset;   /* the sector offset                    */
    GT_U32          readFromOffset; /* the offset to read from              */

    sectorOffset = gtExtDrvFlashGetSectorOffset(gtExtDrvFlashGetConfiSector());
    readFromOffset =
        sectorOffset + ((theConfiId - 1) * CONFI_FLASH_MAX_MEM_SIZE);

    /* do the actual read */
    read = gtExtDrvFlashReadBlock(readFromOffset + theOffset, theLength,
                                  pTheBuffer);

    /* check if the number of bytes read is what we wanted */
    return read == theLength ? GT_OK : GT_FAIL;
#endif /* WIN32 */
}

/*******************************************************************************
* confiFlashWriteInt
*
* DESCRIPTION:
*       Write confi binary format to the flash, using the external driver API
*
* INPUTS:
*       pTheAddress         - the memory to write to the flash
*       theLength           - the memory length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - write succeeded
*       GT_ERROR        - write failed
*
* COMMENTS:
*       Private.
*
*******************************************************************************/
static GT_STATUS confiFlashWriteInt
(
    IN          GT_CHAR     *pTheAddress,
    IN const    GT_U32      theLength,
    IN          GT_CHAR     theConfiId
)
{
#ifdef WIN32
/* flash is not supported in win32 environment */
    return GT_OK;
#else /* WIN32 */
    GT_U32      confiSector;
    GT_U32      written;        /* number of bytes written to the flash     */
    GT_U32      startOffset;    /* the start sector offset                  */
    GT_U32      startSector;    /* the start sector                         */
    GT_U32      sectorSize;     /* size of the sector                       */
    GT_U32      nSectors;       /* number of sectors per confi              */
    GT_U32      i;              /* iterator */
    GT_CHAR     *pSectorMemory; /* shadow of the sector memory              */
    GT_U32      read;           /* number of bytes read from the flash      */
    GT_U32      confiPerSector; /* number of confis per sector              */

    if ((theLength > CONFI_FLASH_MAX_MEM_SIZE) ||
        (theConfiId > CONFI_FLASH_NUM_OF_CONFIGS))
    {
        return GT_BAD_PARAM;
    }
    confiSector = gtExtDrvFlashGetConfiSector();
    sectorSize = gtExtDrvFlashGetSectorSize(confiSector);
    startSector = confiSector +
       ((theConfiId - 1) * CONFI_FLASH_MAX_MEM_SIZE) /sectorSize;
    startOffset = gtExtDrvFlashGetSectorOffset(startSector);
    confiPerSector = sectorSize / CONFI_FLASH_MAX_MEM_SIZE;

    nSectors = theLength / sectorSize + (theLength % sectorSize ? 1:0);

    for(i = 0; i < nSectors - 1; i++)
    {
        /* erase the sectors - and re-write it later */
        gtExtDrvFlashEraseSector(confiSector + i);
        /* do the actual write */
        written = gtExtDrvFlashWriteBlock(startOffset + i * sectorSize,
                                          sectorSize,
                                          pTheAddress + i * sectorSize);
        if(written != sectorSize)
        {
            return GT_ERROR;
        }
    }
    /* allocate shadow for sector */
    pSectorMemory = osMalloc(sectorSize);
    if (NULL == pSectorMemory)
    {
        return GT_OUT_OF_CPU_MEM;
    }
    /* read before you erase and write */
    read = gtExtDrvFlashReadBlock(startOffset + (nSectors - 1) * sectorSize,
                                  sectorSize,
                                  pSectorMemory);
    if (0xFFFFFFFF == read)
    {
        osFree(pSectorMemory);
        return GT_ERROR;
    }

    /* copy data */
    if(nSectors > 1)
    {
        /* copy remainder */
        osMemCpy(pSectorMemory, pTheAddress + (nSectors - 1) * sectorSize,
                 theLength - (nSectors - 1) * sectorSize);
    }
    else
    {
        /* copy all confi */
        osMemCpy(pSectorMemory +
                 CONFI_FLASH_MAX_MEM_SIZE * ((theConfiId - 1) % confiPerSector),
                 pTheAddress, theLength);
    }

    /* erase the sectors - and re-write it later */
    gtExtDrvFlashEraseSector(startSector + nSectors - 1);
    /* write the last sector */
    written = gtExtDrvFlashWriteBlock(startOffset + (nSectors - 1)*sectorSize,
                                      sectorSize, pSectorMemory);
    if(written != sectorSize)
    {
        return GT_ERROR;
    }

    return GT_OK;
#endif /* WIN32 */
}
#endif

/*******************************************************************************
* confiGetNewLine
*
* DESCRIPTION:
*       Get the new line following current line. If current line is NULL, then
*       return the first line of the buffer.
*
* INPUTS:
*       curLine  - pointer to the current line. If NULL return the first line
*                  of the buffer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Pointer to the next line.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_CHAR*   confiGetNewLine
(
    IN  GT_CHAR*  curLine
)
{
    GT_CHAR *ptr;

    if(curLine == NULL)
    {
        return confiBuffer.buf;
    }

    ptr = osStrChr(curLine, '\0');
    if(ptr != NULL)
    {
        ptr++; /* move to next symbol after */
    }

    return (GT_CHAR*)ptr;
}


/*******************************************************************************
* confiGetNewCommand
*
* DESCRIPTION:
*       Parse line and return command and fields parameters.
*
* INPUTS:
*       buf  - pointer to parsed line.
*
* OUTPUTS:
*       buf           - point to the last parsed line
*       inBuffer      - returned command
*       inFieldBuffer - returned fields
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS confiGetNewCommand
(
    INOUT  GT_CHAR   **buf,
    OUT    GT_CHAR   **inBuffer,
    OUT    GT_CHAR   **inFieldBuffer
)
{
    GT_CHAR     cmdName[64];
    CMD_COMMAND *command;
    GT_STATUS   status;
#if 0
    osStrCpy(*inBuffer, *buf);
    /* copy command name */
    sscanf(*buf,"%s", cmdName);
    /* find command in Galtis DB */
    status = cmdGetCommand(cmdName, &command);
    if(status == GT_FAIL)
    {
        return GT_FAIL;
    }
    if(status == GT_NOT_FOUND)
    {
        return GT_OK;
    }
    if(command->funcFields != 0)
    {
        *buf = confiGetNewLine(*buf);
        osStrCpy(*inFieldBuffer,*buf);
    }
#endif
    return GT_OK;
}

/*******************************************************************************
* confiCheckHeader
*
* DESCRIPTION:
*       Check whether script contains right header.
*
* INPUTS:
*       line  - pointer to header line.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS confiCheckHeader
(
    IN  GT_CHAR *line
)
{
    /* check header */
    if((line[0] != 'C') || (line[1] != 'T'))
    {
        return GT_FAIL;
    }

    return GT_OK;
}

/*******************************************************************************
* confi2LogFltr
*
* DESCRIPTION:
*       Filter log output, remove all Galtis Agent delimiters
*
* INPUTS:
*       srcLine  - input line.
*
* OUTPUTS:
*       outLine  - filtered line
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS confi2LogFltr
(
    IN  GT_CHAR  *srcLine,
    OUT GT_CHAR  *outLine
)
{
    GT_CHAR* curSym;
    GT_BOOL rep;

    curSym = srcLine;
    rep = GT_FALSE;
    while(*curSym != '\0')
    {
        if(((*curSym >= '0') && (*curSym <= '9')) ||
           ((*curSym >= 'a') && (*curSym <= 'f')) ||
           ((*curSym >= 'A') && (*curSym <= 'F')))
        {
            /* that's hex symbol */
            *outLine = *curSym;
            outLine++;
            rep = GT_FALSE;
        }
        else
        {
            if(rep == GT_FALSE)
            {
                *outLine = 0x20;
                outLine++;
                rep = GT_TRUE;
            }
        }
        curSym++;
    }

    *outLine = '\0';

    return GT_OK;
}

