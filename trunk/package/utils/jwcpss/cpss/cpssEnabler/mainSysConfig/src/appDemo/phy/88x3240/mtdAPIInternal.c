#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and data that are strictly internal
for the proper functioning of the API on the Marvell 88X3240 
ethernet PHY.
********************************************************************/
#include "mtdApiRegs.h"
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdDiagnostics.h"
#include "mtdAPIInternal.h"
#include "mtdFwDownload.h"

#include <stdio.h>

#if DB_TEST_ENVIRONMENT
#include <windows.h> 
#include <time.h>
#endif


#if DB_TEST_FLASHCODE
void amuse_user(void);
static void amuse_user_ram(MTD_U32 buffCount);
#endif

/* Internal PHY Registers for downloading to RAM */
#define MTD_LOW_WORD_REG     3,0xD0F0 /* register to set the low part of the address */
#define MTD_HI__WORD_REG     3,0xD0F1 /* register to set the hi part of the address */
#define MTD_RAM_DATA_REG     3,0xD0F2 /* register to write or read to/from ram */
#define MTD_RAM_CHECKSUM_REG 3,0xD0F3 /* register to read the checksum from */


/* This handles downloading an image pointed to by data which is size bytes long
   to the phy's flash interfacing with the slave code as a helper program.
   Size must be an even number (the flash can only be written to in words).*/
MTD_STATUS mtdMdioFlashDownload(CTX_PTR_TYPE contextPtr, MTD_U16 port,
                                MTD_U8 data[],MTD_U32 size, MTD_U16 *errCode)
{
    MTD_U16 buf_checksum, tmp_checksum, reported_checksum, words_rcvd, words_written, tmp;
    MTD_U32 maxBuffSize = 0, numTransfers, lastTransferSize, transferIndex;
    MTD_U32 byteIndex, stopIndex;
#if DB_TEST_FLASHCODE
    double download_time;
    time_t start_time,stop_time;
#endif

    *errCode = 0;

    if (size%2)
    {
        /* it's an error, size must be an even number of bytes */
        *errCode = MTD_SIZE_NOT_EVEN;
        return MTD_FAIL;
    }

    /* first erase the flash*/
#if DB_TEST_FLASHCODE
    MTD_DBG_INFO("Slave will now erase flash. This may take up to 6 seconds.\n");
#endif
    TRY_IO(mtdHwXmdioWrite(contextPtr, port,COMMAND_REG, ERASE_FLASH_PROGRAM_AREA));

    TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
    while( tmp==ERASE_FLASH_PROGRAM_AREA || tmp==MTD_SLAVE_FLASH_BUSY )
    {
        TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
    }

    if (tmp == MTD_SLAVE_ERR_CMD)
    {
        *errCode = MTD_ERR_ERASING_FLASH;
        return MTD_FAIL;        
    }
    else
    {
        if (tmp == MTD_SLAVE_OK)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_INFO("Flash program areas have been erased.\n");
#endif
        }
        else
        {
            /* unexpected value read back from download code*/
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("Unexpected response from phy. Exiting...\n");
#endif
            *errCode = MTD_ERR_VALUE_READ_BACK;
            return MTD_FAIL;                       
        }
    }

    /* read in the max buffer size from the slave*/
    /* this is the maximum size that can be written at any 1 time*/
    TRY_IO(mtdHwXmdioRead(contextPtr, port,MAX_BUFF_SIZE_OUT_REG,(MTD_U16 *)&maxBuffSize));
    maxBuffSize *= 2; /* now size is in bytes */

    numTransfers = size/maxBuffSize;
    lastTransferSize = size%maxBuffSize;

#if DB_TEST_FLASHCODE
    time(&start_time);
#endif

    /* handle all the full transfers */
    byteIndex=0;
    for(transferIndex=0; transferIndex < numTransfers; transferIndex++)
    {        
        /* Set the flash start address*/
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,LOW_ADDRESS_REG, (MTD_U16)byteIndex));
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,HIGH_ADDRESS_REG, (MTD_U16)(byteIndex>>16)));
        /* Set the size of the buffer we're going to send*/
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,ACTUAL_BUFF_SIZE_IN_REG, (MTD_U16)(maxBuffSize/2)));
        /* Tell the slave we've written the start address and size
        / and now we're going to start writing data*/
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,COMMAND_REG,FILL_BUFFER));

        /* Wait for MTD_SLAVE_OK*/
        TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        while( tmp==ERASE_FLASH_PROGRAM_AREA || tmp==MTD_SLAVE_FLASH_BUSY )
        {
            TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        }

        if (tmp != MTD_SLAVE_OK)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("Expected %d from slave got %d. Exiting...\n", MTD_SLAVE_OK, tmp);
#endif
            *errCode = MTD_ERR_START_WRITE_DATA;
            return MTD_FAIL;            
        }

        /* Write a buffer of data to the slave RAM*/
        stopIndex = byteIndex + maxBuffSize;
        buf_checksum = 0;
        while(byteIndex < stopIndex)
        {
            MTD_U16 value;

            value = data[byteIndex++];
            value |= (((MTD_U16)data[byteIndex++]) << 8);
            buf_checksum += value;
            TRY_IO(mtdHwXmdioWrite(contextPtr, port,DATA_REG,value));
        }

#if DB_TEST_FLASHCODE            
        MTD_DBG_INFO("."); /* Amuse the user*/
#endif

        /* check and see if we can go on to the write*/
        TRY_IO(mtdHwXmdioRead(contextPtr, port,CHECKSUM_REG,&tmp_checksum));
        TRY_IO(mtdHwXmdioRead(contextPtr, port,WORDS_RCVD_REG,&words_rcvd));
        if (tmp_checksum != buf_checksum || words_rcvd != (MTD_U16)(maxBuffSize/2))
        {
            /* Host might want to issue a retry here instead failing*/
            /* Note that the flash start address must be reset before resending the buffer*/
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR( "Slave failed to get all the data correctly\n");
#endif
            *errCode = MTD_START_WRITE_DATA;
            return MTD_FAIL;            
        }

        /* One full RAM buffer inside DSP is ready to write to flash now*/
        /* Tell the slave to write it*/
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\nTelling slave to save %d bytes of buffer %d\n",\
            (int)maxBuffSize, (int)transferIndex); 
#endif

        TRY_IO(mtdHwXmdioWrite(contextPtr, port,COMMAND_REG,WRITE_BUFFER));

#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("Waiting for slave to finish programming flash");
#endif
        /* Wait for MTD_SLAVE_OK */
        TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        while(tmp == WRITE_BUFFER || tmp == MTD_SLAVE_FLASH_BUSY)
        {
            /* this can take several 2-3 seconds, don't poll phy too frequently*/
            mtdWait(contextPtr, 500); 
            /* since every read causes an interrupt on the phy */

#if DB_TEST_FLASHCODE
            amuse_user();
#endif

            TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        }

        if (tmp == MTD_SLAVE_ERR_CMD)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("\nSome kind of error occurred on Slave. Exiting\n");
#endif
            *errCode = MTD_ERR_ON_SLAVE;
            return MTD_FAIL;
        }
        else
        {
            if (tmp != MTD_SLAVE_OK)
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_ERROR("Expected %d from slave got %d. Exiting...\n", MTD_SLAVE_OK, tmp);
#endif
                *errCode = MTD_ERR_ON_SLAVE;
                return MTD_FAIL;
                
            }
            else
            {
                /* readback checksum of what was stored in flash */
                TRY_IO(mtdHwXmdioRead(contextPtr, port,CHECKSUM_REG,&reported_checksum)); 
                if (reported_checksum != buf_checksum)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Expected %d checksum but got %d. Exiting...\n",\
                        buf_checksum, reported_checksum);
#endif
                    *errCode = MTD_ERR_CHECKSUM;
                    return MTD_FAIL;                    
                }
            }
        }

        TRY_IO(mtdHwXmdioRead(contextPtr,port,WORDS_WRITTEN_REG,&words_written));
        if (words_written != (maxBuffSize/2))
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("\nSlave didn't write enough words to flash. Error occurred. Exit\n");
#endif
            *errCode = MTD_ERR_SLAVE_WRITE_FULL;
            return MTD_FAIL;
            
        }
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\n");
#endif
    }

    /* now handle last transfer */
    if (lastTransferSize)
    {

        /* Set the flash start address */
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,LOW_ADDRESS_REG, (MTD_U16)byteIndex));
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,HIGH_ADDRESS_REG, (MTD_U16)(byteIndex>>16)));
        /* Set the size of the buffer we're going to send */
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,ACTUAL_BUFF_SIZE_IN_REG, (MTD_U16)(lastTransferSize/2)));
        /* Tell the slave we've written the start address and size */
        /* and now we're going to start writing data */
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,COMMAND_REG,FILL_BUFFER));

        /* Wait for MTD_SLAVE_OK */
        TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        while(tmp == FILL_BUFFER)
        {
            TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        }

        if (tmp != MTD_SLAVE_OK)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("Expected %d from slave got %d. Exiting...\n", MTD_SLAVE_OK, tmp);
#endif
            *errCode = MTD_ERR_LAST_TRANSFER;
            return MTD_FAIL;            
        }

        /* Write a buffer of data to the slave RAM */
        stopIndex = byteIndex + lastTransferSize;
        buf_checksum = 0;
        while(byteIndex < stopIndex)
        {
            MTD_U16 value;

            value = data[byteIndex++];
            value |= (((MTD_U16)data[byteIndex++]) << 8);
            buf_checksum += value;
            TRY_IO(mtdHwXmdioWrite(contextPtr, port,DATA_REG,value));
        }

        /* Last buffer is ready to write to flash now
           Tell the slave to write it */
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\nTelling slave to save %d bytes of last buffer\n", (int)lastTransferSize); 
#endif
        TRY_IO(mtdHwXmdioWrite(contextPtr, port,COMMAND_REG,WRITE_BUFFER));

        /* Wait until the slave is finished */
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("Waiting for slave to finish programming last buffer to flash");
#endif

        /* Wait for MTD_SLAVE_OK */
        TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        while(tmp == WRITE_BUFFER || tmp == MTD_SLAVE_FLASH_BUSY)
        {
            mtdWait(contextPtr, 500);  /* this can take several 2-3 seconds,
            don't poll phy too frequently since every read causes an interrupt on the phy */
#if DB_TEST_FLASHCODE
            amuse_user();
#endif
            TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
        }

        if (tmp == MTD_SLAVE_ERR_CMD)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("\nSome kind of error occurred on Slave. Exiting\n");
#endif
            *errCode = MTD_ERR_LAST_TRANSFER;
            return MTD_FAIL;            
        }
        else
        {
            if (tmp != MTD_SLAVE_OK)
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_ERROR("Expected %d from slave got %d. Exiting...\n", MTD_SLAVE_OK, tmp);
#endif
                *errCode = MTD_ERR_LAST_TRANSFER;
                return MTD_FAIL;
            }
            else
            {
                /* readback checksum of what was stored in flash */
                TRY_IO(mtdHwXmdioRead(contextPtr, port,CHECKSUM_REG,&reported_checksum)); 
                if (reported_checksum != buf_checksum)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Expected %d checksum but got %d. Exiting...\n",\
                        buf_checksum, reported_checksum);
#endif
                    *errCode = MTD_ERR_CHECKSUM;
                    return MTD_FAIL;                    
                }
            }
        }

        TRY_IO(mtdHwXmdioRead(contextPtr, port,WORDS_WRITTEN_REG,&words_written));
        if ( words_written != (MTD_U16)(lastTransferSize/2))
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("\nSlave didn't write enough words to flash. Error occurred. Exit\n");
#endif
            *errCode = MTD_ERR_SLAVE_WRITE_FULL;
            return MTD_FAIL;            
        }

#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\n");
#endif
    }

#if DB_TEST_FLASHCODE
    time(&stop_time);
    download_time = difftime(stop_time,start_time);
    MTD_DBG_INFO("\nTime = %d seconds\n",(int)download_time);
#endif

    return MTD_OK;
}

/* This function downloads code to RAM in the DSP and then starts the application
 which was downloaded. "size" should be an even number (memory can only be written word-wise) */
MTD_STATUS mtdMdioRamDownload(CTX_PTR_TYPE contextPtr,MTD_U8 data[],
                              MTD_U32 size, MTD_U16 port, MTD_U8 use_ram_checksum,
                              MTD_U16 *errCode) 
{
    MTD_U32 buffCount;
    MTD_U8  lowByte, highByte;
    MTD_U16 tmp;
    MTD_U16 ram_checksum;
    MTD_U16 expected_checksum;
#if DB_TEST_FLASHCODE
    double download_time;
    time_t start_time,stop_time;
#endif

    *errCode = 0;

    if (size%2)
    {
        /* it's an error, size must be an even number of bytes */
        *errCode = MTD_SIZE_NOT_EVEN;
        return MTD_FAIL;
    }

    /* Put PHY in download mode and reset PHY */
    ATTEMPT(mtdPutPhyInMdioDownloadMode(contextPtr, port)); /* will leave errCode as 0 if fails */
    
    /* Allow reset to complete */
    ATTEMPT(mtdWait(contextPtr, 250)); /* will leave errCode as 0 if fails */
    
    /* Make sure we can access the PHY
       and it's in the correct mode (waiting for download) */
    TRY_IO(mtdHwXmdioRead(contextPtr, port,MTD_BOOT_STATUS_REG,&tmp));
        
    if (tmp != 0x000A)
    {
#if DB_TEST_FLASHCODE
        MTD_DBG_ERROR( "PHY is not in waiting on download mode. Expected 0x000A, read 0x%04X\n",\
                       (unsigned)tmp);
        MTD_DBG_ERROR( "Download failed\n");
#endif
        *errCode = MTD_PHY_NOT_IN_DOWNLOAD_MODE;
        return MTD_FAIL;
    }
    else
    {
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("Downloading code to PHY RAM, please wait...\n");
#endif
    }

    if(use_ram_checksum)
    {
        /* this read clears the checksum */
        TRY_IO(mtdHwXmdioRead(contextPtr, port, MTD_RAM_CHECKSUM_REG,&ram_checksum));        
    }

    /* Set starting address in RAM to 0x00100000 */
    TRY_IO(mtdHwXmdioWrite(contextPtr, port,MTD_LOW_WORD_REG,0));
    TRY_IO(mtdHwXmdioWrite(contextPtr, port,MTD_HI__WORD_REG,0x0010));

#if DB_TEST_FLASHCODE
    time(&start_time);
#endif

    /* Copy the code to the phy's internal RAM, calculating checksum as we go */
    buffCount=0;
    expected_checksum = 0;  
    while(buffCount < size)
    {
        lowByte = data[buffCount++];
        highByte = data[buffCount++];
        expected_checksum += (lowByte + highByte);  
        TRY_IO(mtdHwXmdioWrite(contextPtr,port,MTD_RAM_DATA_REG,(((MTD_U16)highByte)<<8)|lowByte));        
        
#if DB_TEST_FLASHCODE
        /* Let the user know something's going on... */
        amuse_user_ram(buffCount);
#endif
    }

    if(use_ram_checksum)
    {
        /* Read hardware checksum register and see if it matches the locally computed checksum */
        TRY_IO(mtdHwXmdioRead(contextPtr,port,MTD_RAM_CHECKSUM_REG,&ram_checksum));        
        
        if(expected_checksum != ram_checksum)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_INFO( "Error downloading code. Expected RAM HW checsum to be %hu but it was %hu", expected_checksum, ram_checksum);
#endif
            *errCode = MTD_RAM_HW_CHECKSUM_ERR;
            return MTD_FAIL;
        }
    }

#if DB_TEST_FLASHCODE
    /* Calculate download time and print out results */
    time(&stop_time);
    download_time = difftime(stop_time,start_time);
    MTD_DBG_INFO("\nDownload of code to RAM complete. Time = %d seconds\n",(int)download_time);
    MTD_DBG_INFO("\nDownload complete, starting code.\n");
#endif

    /* Now start code which was downloaded */
    TRY_IO(mtdHwSetPhyRegField(contextPtr,port,MTD_BOOT_STATUS_REG,6,1,1));

    ATTEMPT(mtdWait(contextPtr, 500)); /* Give app code time to start*/

    return MTD_OK;
}


/********************************************************************/

/**************** Parallel Download *********************/

/********************************************************************/
/* This function downloads code to several ports' RAM in the DSP "in parallel" meaning
   multiple ports are downloaded at the same time and then starts the application
   which was downloaded.*/

/* Make sure "size" is an even number(memory can only be written word-wise) */
MTD_STATUS mtdParallelMdioRamDownload(CTX_PTR_TYPE contextPtr, MTD_U8 data[],MTD_U32 size,
                                      MTD_U16 ports[], MTD_U16 numPorts, MTD_U16 erroredPorts[],
                                      MTD_U8 use_ram_checksum, MTD_U16 *errCode) 
{
    unsigned     buffCount;
    MTD_U8 lowByte, highByte;
    MTD_U16 tmp;
    MTD_U16 ram_checksum[MAX_PORTS_TO_DOWNLOAD];
    MTD_U16 expected_checksum;
#if DB_TEST_FLASHCODE
    double download_time;
    time_t start_time,stop_time;
#endif
    MTD_U32 i;

    *errCode = 0;

    if (size%2)
    {
        /* it's an error, size must be an even number of bytes */
        *errCode = MTD_SIZE_NOT_EVEN;
        return MTD_FAIL;
    }
    
    for(i = 0; i < numPorts; i++)
    {
        /* really only need to do the first port on each chip, but we don't know the mix of ports per chip */
        /* this should always work, leaving all chips in download mode */
        /* some chips will get reset multiple times, which is ok */
        ATTEMPT(mtdPutPhyInMdioDownloadMode(contextPtr, ports[i])); /* if fails, returns errCode of 0 */
    }
    
    /* Allow reset to complete */
    ATTEMPT(mtdWait(contextPtr, 250)); /* if fails, returns errCode of 0 */   

    /* Make sure we can access the DSP
      And it's in the correct mode (waiting for download) */
    for(i = 0; i < numPorts; i++)
    {
        TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],MTD_BOOT_STATUS_REG,&tmp));
        if (tmp != 0x000A)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR( "DSP in port %d is not in waiting on download mode. Expected 0x000A, read 0x%04X\n",\
                (int)ports[i], (int)tmp);
            MTD_DBG_ERROR( "Download failed on that port\n");
#endif
            erroredPorts[i] = MTD_PHY_NOT_IN_DOWNLOAD_MODE;
        }
    } 

    if(use_ram_checksum) /* always true on X32X0 chips... */
    {
        for(i = 0; i < numPorts; i++)
        {
            TRY_IO(mtdHwXmdioRead(contextPtr, ports[i], MTD_RAM_CHECKSUM_REG,&tmp)); /* clear the register by reading */
        }
    }

    /*Write to all the ports at the same time using the PHY over write pin (PHYPRT_OW) = 1.
      Writing it to one port will ingore the port and write to all the ports connected on that pin.
      When reading set the pin to 0*/
    for(i = 0; i < numPorts; i++)
    {
        TRY_IO(mtdPutPhyInParallelWriteMode(contextPtr, ports[i]));
    }

    /* Set starting address in RAM to 0x00100000 on all ports */
    TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],MTD_LOW_WORD_REG,0));
    TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],MTD_HI__WORD_REG,0x0010));

#if DB_TEST_FLASHCODE
    time(&start_time);
#endif

    /* Copy the code to the phy's internal RAM */
    buffCount=0;
    expected_checksum = 0;
    while(buffCount < size)
    {
        lowByte = data[buffCount++];
        highByte = data[buffCount++];
        expected_checksum += (lowByte + highByte);  
        TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],MTD_RAM_DATA_REG,(((MTD_U16)highByte)<<8)|lowByte));      

#if DB_TEST_FLASHCODE
        /* Let the user know something's going on... */
        amuse_user_ram(buffCount);
#endif
    }

    /* writing to 1 clears all */
    TRY_IO(mtdRemovePhyFromParallelWriteMode(contextPtr, ports[0]));
    
    if(use_ram_checksum) /* always true for X32X0 */
    {
        for(i = 0; i < numPorts; i++)
        {
            /* read the hardware register checksum and see if it matches with the locally computed checksum */
            TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],MTD_RAM_CHECKSUM_REG,&ram_checksum[i])); 
            if(expected_checksum != ram_checksum[i])   
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_INFO( "Error downloading code on port %d. Expected RAM HW checksum to be 0x%04X but it was 0x%04X",(int)ports[i],\
                              (int)expected_checksum, (int)ram_checksum);
#endif
                erroredPorts[i] = MTD_RAM_HW_CHECKSUM_ERR;
            }
        }
    }

#if DB_TEST_FLASHCODE
    /* Calculate download time and print out results */
    time(&stop_time);
    download_time = difftime(stop_time,start_time);
    MTD_DBG_INFO("\nDownload of code to RAM complete. Time = %d seconds\n",(int)download_time);
    MTD_DBG_INFO("\nDownload complete, starting code.\n");
#endif

    /* Now start code which was downloaded to all ports*/
    for(i = 0; i < numPorts; i++)
    {
        if (erroredPorts[i] == 0)
        {
            TRY_IO(mtdHwSetPhyRegField(contextPtr,ports[i],MTD_BOOT_STATUS_REG,6,1,1));

            ATTEMPT(mtdWait(contextPtr, 50)); /* will leave errCode as 0 if fails */ 
        }
    }
    ATTEMPT(mtdWait(contextPtr, 100)); /* give app code time to start, will leave errCode to 0 if it fails*/

    return MTD_OK;
}


MTD_STATUS mtdParallelMdioFlashDownload(CTX_PTR_TYPE contextPtr, MTD_U8 data[],
                                 MTD_U32 size, 
                                 MTD_U16 ports[], MTD_U16 numPorts,
                                 MTD_U16 erroredPorts[],
                                 MTD_U16 *errCode)
{

    MTD_U16 buf_checksum, tmp_checksum, reported_checksum, words_rcvd, tmp;
    MTD_U32 maxBuffSize = 0, numTransfers, lastTransferSize, transferIndex;
    MTD_U32 byteIndex, stopIndex;
#if DB_TEST_FLASHCODE
    double download_time;
    time_t start_time,stop_time;
#endif
    MTD_U16 i;

    if (size%2)
    {
        /* it's an error, size must be an even number of bytes */
        *errCode = MTD_SIZE_NOT_EVEN;
        return MTD_FAIL;
    }

    /* first erase the flash*/
#if DB_TEST_FLASHCODE
    MTD_DBG_INFO("Slave will now erase flash. This may take up to 6 seconds.\n");
#endif
    
    for(i = 0; i < numPorts; i++)
    {
        TRY_IO(mtdPutPhyInParallelWriteMode(contextPtr, ports[i]));  /* parallel write */
    }

    TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],COMMAND_REG, ERASE_FLASH_PROGRAM_AREA));

    TRY_IO(mtdRemovePhyFromParallelWriteMode(contextPtr, ports[0]));   /*remove parallel write*/

    for(i = 0; i < numPorts; i++)
    {
        if(erroredPorts[i] == 0)
        {
            TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
            while( tmp==ERASE_FLASH_PROGRAM_AREA || tmp==MTD_SLAVE_FLASH_BUSY )
            {
                ATTEMPT(mtdWait(contextPtr, 100)); /* don't poll too often, this can take awhile */
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
            }

            if (tmp == MTD_SLAVE_ERR_CMD)
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_ERROR("Slave encountered error while erasing flash on port %d. \n",ports[i]);
#endif
                erroredPorts[i] = MTD_ERR_ERASING_FLASH;        
            }
            else
            {
                if (tmp == MTD_SLAVE_OK)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_INFO("Flash program areas have been erased for port %d.\n",ports[i]);
#endif
                }
                else
                {
            /* unexpected value read back from download code*/
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Unexpected response from phy on port %d is 0x%04X \n",ports[i],(unsigned)tmp);
#endif
                    erroredPorts[i] = MTD_ERR_VALUE_READ_BACK;
                       
                }
            }
        }
    }

    /* read in the max buffer size from the slave*/
    /* this is the maximum size that can be written at any 1 time, it will be the same on all slaves */
    TRY_IO(mtdHwXmdioRead(contextPtr, ports[0],MAX_BUFF_SIZE_OUT_REG,(MTD_U16 *)&maxBuffSize));
    maxBuffSize *= 2; /* now it's in bytes */

    numTransfers = size/maxBuffSize;
    lastTransferSize = size%maxBuffSize;

#if DB_TEST_FLASHCODE
    time(&start_time);
#endif

    /* handle all the full transfers */
    
    byteIndex=0;
    for(transferIndex=0; transferIndex < numTransfers; transferIndex++)
    {
        for(i = 0; i < numPorts; i++)
        {
            /* put all ports in parallel write mode */
            TRY_IO(mtdPutPhyInParallelWriteMode(contextPtr, ports[i]));  
        }

        /* Set the flash start address*/
        TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],LOW_ADDRESS_REG, (MTD_U16)byteIndex));
        TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],HIGH_ADDRESS_REG, (MTD_U16)(byteIndex>>16)));
        /* Set the size of the buffer we're going to send*/
        TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],ACTUAL_BUFF_SIZE_IN_REG, (MTD_U16)(maxBuffSize/2)));
        /* Tell the slave we've written the start address and size
        / and now we're going to start writing data*/
        TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],COMMAND_REG,FILL_BUFFER));

        mtdRemovePhyFromParallelWriteMode(contextPtr, ports[0]);   

        for(i = 0; i < numPorts; i++)
        {
            if(erroredPorts[i] == 0)
            {
                /* Wait for MTD_SLAVE_OK*/
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                while(tmp == FILL_BUFFER)
                {
                    TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                }

                if (tmp != MTD_SLAVE_OK)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Expected 0x%04X from slave got 0x%04X. on port %d \n", MTD_SLAVE_OK, (unsigned)tmp, ports[i]);
#endif
                    erroredPorts[i] = MTD_ERR_START_WRITE_DATA;            
                }
            }
        }

        /* Write a buffer of data to the slave RAM*/
        for(i = 0; i < numPorts; i++)
        {        
            TRY_IO(mtdPutPhyInParallelWriteMode(contextPtr, ports[i]));  
        }
        
        stopIndex = byteIndex + maxBuffSize;
        buf_checksum = 0;
        while(byteIndex < stopIndex)
        {
            MTD_U16 value;

            value = data[byteIndex++];
            value |= (((MTD_U16)data[byteIndex++]) << 8);
            buf_checksum += value;
            TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],DATA_REG,value));
        }
        
        TRY_IO(mtdRemovePhyFromParallelWriteMode(contextPtr, ports[0]));   

#if DB_TEST_FLASHCODE            
        MTD_DBG_INFO("."); /* Amuse the user */
#endif

        for(i = 0; i < numPorts; i++)
        {
            if(erroredPorts[i] == 0)
            {
                /* check and see if we can go on to the write*/
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],CHECKSUM_REG,&tmp_checksum));
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],WORDS_RCVD_REG,&words_rcvd));
                
                if (tmp_checksum != buf_checksum || words_rcvd != maxBuffSize/2)
                {
                    /* Host might want to issue a retry here instead failing*/
                    /* Note that the flash start address must be reset before resending the buffer*/
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR( "Slave failed to get all the data correctly on port %d \n",ports[i]);
#endif
                    erroredPorts[i] = MTD_START_WRITE_DATA;            
                }
            }
        }

        /* One full RAM buffer inside DSP is ready to write to flash now*/
        /* Tell the slave to write it*/
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\nTelling slave to save %d bytes of buffer %d\n",\
            (int)maxBuffSize, (int)transferIndex); 
#endif
        for(i = 0; i < numPorts; i++)
        {
            if (erroredPorts[i] == 0)
            {
                TRY_IO(mtdHwXmdioWrite(contextPtr, ports[i],COMMAND_REG,WRITE_BUFFER));
            }
        }
        

#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("Waiting for slave to finish programming flash\n");
#endif

        for(i = 0; i < numPorts; i++)
        {
            if(erroredPorts[i] == 0)
            {
                /* Wait for MTD_SLAVE_OK */
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                
                while(tmp == WRITE_BUFFER || tmp == MTD_SLAVE_FLASH_BUSY)
                {
                    /* this can take several 2-3 seconds, don't poll phy too frequently*/
                    ATTEMPT(mtdWait(contextPtr, 500)); 
                    /* since every read causes an interrupt on the phy */
#if DB_TEST_FLASHCODE
                    amuse_user();
#endif
                    TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                }

                if (tmp == MTD_SLAVE_ERR_CMD)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Some kind of error occurred on slave on port %d.\n",ports[i]);
#endif
                    erroredPorts[i] = MTD_ERR_ON_SLAVE;
                }
                else
                {
                    if (tmp != MTD_SLAVE_OK)
                    {
#if DB_TEST_FLASHCODE
                        MTD_DBG_ERROR("Expected 0x%04X from slave got 0x%04X on port %d\n", MTD_SLAVE_OK, (unsigned)tmp,ports[i]);
#endif
                        erroredPorts[i] = MTD_ERR_ON_SLAVE;
                    }
                    else
                    {
                        /* readback checksum of what was stored in flash */
                        TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],CHECKSUM_REG,&reported_checksum)); 

                        if (reported_checksum != buf_checksum)
                        {
#if DB_TEST_FLASHCODE
                            MTD_DBG_ERROR("Expected 0x%04X buffer checksum but got 0x%04X on port %d\n",\
                                           (unsigned)buf_checksum, (unsigned)reported_checksum, ports[i]);
#endif
                            erroredPorts[i] = MTD_ERR_CHECKSUM;
                        }
                    }
                }

                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],WORDS_WRITTEN_REG,&tmp));
                if (tmp != (MTD_U16)(maxBuffSize/2))
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Slave didn't write enough words to flash on port %d.\n", ports[i]);
#endif
                    erroredPorts[i] = MTD_ERR_SLAVE_WRITE_FULL;
                }
            }
        }
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\n");
#endif
    }

    /* now handle last transfer */
    
    if (lastTransferSize)
    {
        for(i = 0; i < numPorts; i++)
        {        
            /* Set the flash start address */
            TRY_IO(mtdHwXmdioWrite(contextPtr, ports[i],LOW_ADDRESS_REG, (MTD_U16)byteIndex));
            TRY_IO(mtdHwXmdioWrite(contextPtr, ports[i],HIGH_ADDRESS_REG, (MTD_U16)(byteIndex>>16)));
            /* Set the size of the buffer we're going to send */
            TRY_IO(mtdHwXmdioWrite(contextPtr, ports[i],ACTUAL_BUFF_SIZE_IN_REG, (MTD_U16)(lastTransferSize/2)));
            /* Tell the slave we've written the start address and size */
            /* and now we're going to start writing data */
            TRY_IO(mtdHwXmdioWrite(contextPtr, ports[i],COMMAND_REG,FILL_BUFFER));
        }

        /* Wait for MTD_SLAVE_OK */
        for(i = 0; i < numPorts; i++)
        {
            if(erroredPorts[i] == 0)
            {
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                
                while(tmp == FILL_BUFFER)
                {
                    TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                }

                if (tmp != MTD_SLAVE_OK)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Expected 0x%04X from slave got 0x%04X on port %d\n", MTD_SLAVE_OK, (unsigned)tmp,ports[i]);
#endif
                    erroredPorts[i] = MTD_ERR_LAST_TRANSFER;
                }
            }
        }

        /* Write a buffer of data to the slave RAM */
        for(i = 0; i < numPorts; i++)
        {                
            TRY_IO(mtdPutPhyInParallelWriteMode(contextPtr, ports[i]));  
        }
        
        stopIndex = byteIndex + lastTransferSize;
        buf_checksum = 0;
        while(byteIndex < stopIndex)
        {
            MTD_U16 value;

            value = data[byteIndex++];
            value |= (((MTD_U16)data[byteIndex++]) << 8);
            buf_checksum += value;
            TRY_IO(mtdHwXmdioWrite(contextPtr,ports[0],DATA_REG,value));
        }

        /* Last buffer is ready to write to flash now
           Tell the slave to write it */
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\nTelling slave to save %d bytes of last buffer\n", (int)lastTransferSize); 
#endif
        TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],COMMAND_REG,WRITE_BUFFER));

        /* Wait until the slave is finished */
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("Waiting for slave to finish programming last buffer to flash");
#endif

        TRY_IO(mtdRemovePhyFromParallelWriteMode(contextPtr, ports[0]));  

        /* Wait for MTD_SLAVE_OK */
        for(i = 0; i < numPorts; i++)
        {
            if(erroredPorts[i] == 0)
            {
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                
                while(tmp == WRITE_BUFFER || tmp == MTD_SLAVE_FLASH_BUSY)
                {
                    mtdWait(contextPtr, 500);  /* this can take several 2-3 seconds,
                    don't poll phy too frequently since every read causes an interrupt on the phy */
#if DB_TEST_FLASHCODE
                    amuse_user();
#endif
                    TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
                }

                if (tmp == MTD_SLAVE_ERR_CMD)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("\nSome kind of error occurred on Slave on last transfer on port %d.\n",ports[i]);
#endif
                    erroredPorts[i] = MTD_ERR_LAST_TRANSFER;            
                }
                else
                {
                    if (tmp != MTD_SLAVE_OK)
                    {
#if DB_TEST_FLASHCODE
                        MTD_DBG_ERROR("Expected 0x%04X from slave got 0x%04X on port %d.\n", MTD_SLAVE_OK, (unsigned)tmp,ports[i]);
#endif
                        /*return MTD_ERR_LAST_TRANSFER;*/
                        erroredPorts[i] = MTD_ERR_LAST_TRANSFER;
                
                    }
                    else
                    {
                        /* readback checksum of what was stored in flash */
                        TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],CHECKSUM_REG,&reported_checksum)); 
                        
                        if (reported_checksum != buf_checksum)
                        {
#if DB_TEST_FLASHCODE
                            MTD_DBG_ERROR("Expected 0x%04X checksum on last transfer but got 0x%04X on port %d.\n",\
                                           (unsigned)buf_checksum, (unsigned)reported_checksum,ports[i]);
#endif
                            erroredPorts[i] = MTD_ERR_CHECKSUM;                    
                        }
                    }
                }

                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],WORDS_WRITTEN_REG,&tmp));
                
                if ( tmp != (MTD_U16)(lastTransferSize/2))
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Slave didn't write enough words to flash on port %d\n",ports[i]);
#endif
                    erroredPorts[i] = MTD_ERR_SLAVE_WRITE_FULL;            
                }

#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\n");
#endif
            }
        }
    }

#if DB_TEST_FLASHCODE
    time(&stop_time);
    download_time = difftime(stop_time,start_time);
    MTD_DBG_INFO("\nTime = %d seconds\n",(int)download_time);
#endif

                   /* Done */
    return MTD_OK; /* caller should go through and if any erroredPorts[] are not 0, determine there were errors */
}

/* Returns the memory size available in the connected device for application code */
MEM_SIZE_BYTES mtdGetDevMemorySize(CTX_PTR_TYPE contextPtr, MTD_U16 port)
{
    STRUCT_DEVICE_TYPE devType;
    MEM_SIZE_BYTES ramSize;

    devType = mtdGetDeviceType(contextPtr, port);
    switch(devType.devID)
    {
        case X3240_DEV:                       
            ramSize = MTD_MAX_APP_SIZE;
            break;
        case MTDINVALIDDEV:
        default:
            ramSize = 0;
    }
    
    return(ramSize);
}

/* Returns the PHY type, Revision of the connected device */
STRUCT_DEVICE_TYPE mtdGetDeviceType(CTX_PTR_TYPE contextPtr, MTD_U16 port)
{
    STRUCT_DEVICE_TYPE devType;
    MTD_U16 reg1_3;
    MTD_U16 manuf_model_num;
    MTD_U16 rev_num;
    MTD_U16 oui1,oui2;

    /* Check if this is a Marvell PHY */  
    mtdHwXmdioRead(contextPtr,port,1, 2, &oui1);
     
    mtdHwXmdioRead(contextPtr,port,1, 3, &reg1_3);
   
    mtdHwGetRegFieldFromWord(reg1_3,10,6,&oui2);

    mtdHwGetRegFieldFromWord(reg1_3,4,6,&manuf_model_num);

    mtdHwGetRegFieldFromWord(reg1_3,0,4,&rev_num);    

    /* Marvell OUI is 0x05043, X3240 model num is 0x18 */
    if (!(oui1 == 0x141 && oui2 == 3 && manuf_model_num == 0x18))
    {
        devType.devID = MTDINVALIDDEV;
        devType.devRev = MTDINVALIDREV;
		printf("MTL:  oui1= 0x%x , oui2= 0x%x, manuf= 0x%x, rev= 0x%x\n", oui1, oui2, manuf_model_num, rev_num);
		printf("MTL no device found..\n");
        return devType;
    }

    /* this is currently the only type chip, can expand this later as needed */
    devType.devID       = X3240_DEV;
    devType.devRev      = rev_num;

    return devType;
}

MTD_STATUS mtdDoMeasHalfEye
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 curSelLane,
    IN MTD_U16 reg_8xBE,
    OUT MTD_U32 data[][MTD_EYEDIAGRAM_NCOLS]
)
{
    MTD_U16 tmp, tmpAddr;
    MTD_U16 amp = (MTD_EYEDIAGRAM_NROWS - 1)/ 2 + 2;        /*27*/
    MTD_U16 reg11 = 0, reg14 = 0;
    MTD_U16 lsb, msb;
    MTD_U16 i,j, cnt;
    MTD_U16 onlyOnePhase = 0; /* 0: run all 64 phases, 1: run with center phase only*/

    tmpAddr = 0x8000 + curSelLane*MTD_XFI_LANEOFFSET;

    for (i = 0; i < (MTD_EYEDIAGRAM_NROWS + 1) / 2; i++) /* through all amplitudes*/
    {
        /*i) write amp*/
        if (i < (MTD_EYEDIAGRAM_NROWS + 1) / 4) /*13*/
            amp = amp - 2;
        else if (i == (MTD_EYEDIAGRAM_NROWS + 1) / 4) /*13*/
            amp = 0;
        else if (i > (MTD_EYEDIAGRAM_NROWS + 1) / 4) /*13*/
            amp = amp + 2;
        /*write eye amplitude $V.$PL11.10:6*/
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&reg11));
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,((reg11 & 0xF83F) | (amp << 6))));

        /*ii) read phase value*/
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&reg11));
        for (j = 0; j < 64; j++)  /* through 64 phases (64 columns)*/
        {
            if (onlyOnePhase && (j != (int)reg_8xBE))
            {
            }
            else
            {
                /*write phase*/
                ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,((reg11 & 0xFFC0) | j)));
                /*begin measure*/
                ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&tmp));
                ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,(tmp | 0x8000)));
                /*wait for 1s for measurement to be done*/
                for (cnt = 0; cnt < 1000; cnt++)
                {
                    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x11,&tmp));
                    if ((tmp & 0x8000) == 0)
                        break;
                    ATTEMPT(mtdWait(contextPtr, 1)); /* wait for 1ms*/
                    if (cnt == 999)
                        return MTD_FAIL;
                }
                /*$V.$PL14 -- Read reg14*/
                ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x14,&reg14));
                /*write $V.$PL14.11:9 = 010; Read P even eye error counter*/
                ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x14,((reg14 & 0xF00F) | 0x200)));
                /*read measure result*/
                ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x12,&tmp));
                lsb = (tmp & 0xFFFF);
                /*sw1.WriteLine(reg24 & 0xffff);*/
                ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x13,&tmp));
                msb = (tmp & 0xFFFF);
                /*sw2.WriteLine(reg25 & 0xffff);*/
                data[amp][j] = (msb << 16) + lsb;

                /*write $V.$PL14.11:9 = 011; Read P odd eye error counter*/
                ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x14,((reg14 & 0xF00F) | 0x300)));
                /*read measure result*/
                ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x12,&tmp));
                lsb = (tmp & 0xFFFF);
                /*sw1.WriteLine(reg24 & 0xffff);*/
                ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_XFI_DSP,tmpAddr+0x13,&tmp));
                msb = (tmp & 0xFFFF);
                /*sw2.WriteLine(reg25 & 0xffff);*/
                data[amp][j+64] = (msb << 16) + lsb;
            }
        }
    }

    return MTD_OK;
}


/********************************************************************/

#if DB_TEST_FLASHCODE
/******************************************
 Program host most likely does not need
******************************************/
void amuse_user(void)
{
    static unsigned dot_counter;

    dot_counter++;
    if (dot_counter%2)
    {
        MTD_DBG_INFO(".");
    }
}

/***********************************************************************************
   Function most likely not needed by host (from the ram download)
***********************************************************************************/
static void amuse_user_ram(MTD_U32 buffCount)
{
    static unsigned colCount;
    if ((buffCount%512)==0)
    {
        MTD_DBG_INFO(".");
        colCount++;
        if ((colCount%10)== 0)
        {
            MTD_DBG_INFO(" %d bytes\n", buffCount);
        }
    }
}
#endif


