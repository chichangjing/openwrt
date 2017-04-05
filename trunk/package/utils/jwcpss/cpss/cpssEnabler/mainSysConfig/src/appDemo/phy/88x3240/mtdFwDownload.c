#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions using MDIO access to download firmware
into the internal RAM or attached flash memory of the Marvell 88X3240 
ethernet PHY.
********************************************************************/
#include "mtdApiRegs.h"
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdFwDownload.h"
#include "mtdAPIInternal.h"

#if DB_TEST_FLASHCODE
void amuse_user(void);
#endif

MTD_STATUS mtdUpdateFlashImage(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U8 appData[],
    IN MTD_U32 appSize, 
    IN MTD_U8 slaveData[],
    IN MTD_U32 slaveSize,
    OUT MTD_U16 *errCode)
{    
    MTD_U16 tmp;           /* holds result of an MdioRead() */ 
    MTD_U8 use_ram_checksum;     /* tells the RAM download function whether to -
                                        use hardware RAM checksum register or not */
    *errCode = 0;
    
    /******************************************************************************* 
       Check if the code about to be downloaded can fit into the device's memory
    *******************************************************************************/
    if (appSize > (mtdGetDevMemorySize(contextPtr, port) + MTD_HEADER_SIZE))
    {
        /* App size cannot be larger than the device memory size. Code download cannot proceed*/
#if DB_TEST_FLASHCODE
        MTD_DBG_ERROR("Image is larger than the device memory size!\n");
#endif
        *errCode = MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD; 
        return MTD_FAIL;
    }
        
    use_ram_checksum = 1; /* All X32X0 have a ram checksum register, can change to 0 */
                          /* if wish to ignore it for some reason (not advised but */
                          /* might want to for debug purposes) */
    
    /*******************************************************************************
             Download slave code to phy's RAM and start it,
             this code assists in programming the flash connected by SPI
    *******************************************************************************/     
    ATTEMPT(mtdMdioRamDownload(contextPtr,slaveData,slaveSize,port,use_ram_checksum,errCode));     

    /* make sure the slave code started */
    TRY_IO(mtdHwGetPhyRegField(contextPtr,port,MTD_BOOT_STATUS_REG,4,1,&tmp));
   
    if (!tmp)
    {
#if DB_TEST_FLASHCODE
        MTD_DBG_ERROR("Slave code did not start. Expected bit 4 to be 1, read 0x%04X\n",\
            (unsigned)tmp);
        MTD_DBG_ERROR("Slave download failed. Exiting...\n");
#endif
        *errCode = MTD_SLAVE_CODE_DID_NOT_START;        
        return MTD_FAIL;
    }

    /*******************************************************************************
       Write the image to flash with slave's help
    *******************************************************************************/
    ATTEMPT(mtdMdioFlashDownload(contextPtr, port, appData, appSize, errCode));
    
    /*******************************************************************************
       Let slave verify image
    *******************************************************************************/

    /* Using slave code to verify image.
       This commands slave to read in entire flash image and calculate checksum and make sure
       checksum matches the checksum in the header. A failure means flash was corrupted.
      
       Another method would be to reset the phy (with SPI_CONFIG[1]= 0) and see that the new code
       starts successfully, since a bad checksum will result in the code not being started */

#if DB_TEST_FLASHCODE
    MTD_DBG_INFO("Flash programming complete. Verifying image via slave.\n");
#endif

    TRY_IO(mtdHwXmdioWrite(contextPtr, port,COMMAND_REG,VERIFY_FLASH));

    TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
    while(tmp == VERIFY_FLASH || tmp == MTD_SLAVE_FLASH_BUSY)
    {
        ATTEMPT(mtdWait(contextPtr, 500));  /* Don't poll phy too frequently, 
                                               every read causes an interrupt */
#if DB_TEST_FLASHCODE
        amuse_user();
#endif
        TRY_IO(mtdHwXmdioRead(contextPtr, port,COMMAND_REG,&tmp));
    }

    if (tmp == MTD_SLAVE_OK)
    {
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("\nFlash image verified. Reset F_CFG1 to 0 and reboot to execute new code\n");
#endif
        return MTD_OK;   /*Flash download complete */
    }
    else
    {
        if (tmp == MTD_SLAVE_VERIFY_ERR)
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("\nFlash verifed FAILED! Flash probably corrupted. Re-try download.\n");
#endif
            *errCode = MTD_VERIFY_ERR;                
            return MTD_FAIL;
        }
        else
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_ERROR("\nExpected %d from slave got %d. Exiting...\n", MTD_SLAVE_OK, tmp);
#endif
            *errCode = MTD_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL;
            return MTD_FAIL;
        }
    }

}

MTD_STATUS mtdUpdateRamImage(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U8 appData[],
    IN MTD_U32 appSize,
    OUT MTD_U16 *errCode)
{
    MTD_U16 tmp;           /* holds result of an MdioRead() */ 
    MTD_U8 use_ram_checksum;     /* tells the RAM download function whether to -
                                        use hardware RAM checksum register or not */

    *errCode = 0;

    /******************************************************************************* 
       Check if the code about to be downloaded can fit into the device's memory
    *******************************************************************************/
    if (appSize > (mtdGetDevMemorySize(contextPtr, port) + MTD_HEADER_SIZE))
    {
        /* App size cannot be larger than the device memory size. Code download cannot proceed */
#if DB_TEST_FLASHCODE
        MTD_DBG_ERROR("Image is larger than the device memory size!\n");
#endif
        *errCode = MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD; 
        return MTD_FAIL;
    }
        
    use_ram_checksum = 1; /* All X32X0 have a ram checksum register */

    /*******************************************************************************
       This handles downloading the binary part of the HDR file to RAM and executing
       it without modifying the flash.
    *******************************************************************************/
    
    /*******************************************************************************
           Sample of what downloading an HDR file to RAM and executing it would
           look like (non-flash installation or when another image than what's in
           flash is desired to be executed but without changing the flash).
    *******************************************************************************/

    /* Puts phy in download mode, skips over header and downloads binary to RAM
    then starts the app code running */
    ATTEMPT(mtdMdioRamDownload(contextPtr, &appData[MTD_HEADER_SIZE],(appSize-MTD_HEADER_SIZE), \
                           port, use_ram_checksum,errCode));

    /* make sure the app code started */
    TRY_IO(mtdHwGetPhyRegField(contextPtr,port,MTD_BOOT_STATUS_REG,4,1,&tmp));
    if (!tmp)
    {
#if DB_TEST_FLASHCODE
        MTD_DBG_ERROR("App code did not start. Expected bit 4 to be 1, read 0x%04X\n",\
            (unsigned)tmp);
        MTD_DBG_ERROR("App download failed. Exiting...\n");
#endif

        *errCode = MTD_APP_CODE_DID_NOT_START;
        return MTD_FAIL;
    }
    else
    {
#if DB_TEST_FLASHCODE
        MTD_DBG_INFO("App code started. Exiting...\n");
#endif
    }

    return MTD_OK;  /* Ram download is done*/    
}


MTD_STATUS mtdParallelUpdateRamImage(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 ports[],
    IN MTD_U8 appData[],
    IN MTD_U32 appSize, 
    IN MTD_U16 numPorts,
    OUT MTD_U16 erroredPorts[],
    OUT MTD_U16 *errCode)
{
    MTD_U16 tmp;              /* holds result of reading an MDIO register */ 
    MTD_U16 i;
    MTD_U8 use_ram_checksum;  /* tells the RAM download function whether to -
                                 use hardware RAM checksum register or not */

    *errCode = 0;

    if(numPorts > MAX_PORTS_TO_DOWNLOAD)
    {
        *errCode = MTD_NUM_PORTS_TOO_LARGE;
        return MTD_FAIL;
    }

    for(i = 0; i < numPorts; i++) /* Initialize all port status to no error */   
    {
        erroredPorts[i] = 0;
    }

    /******************************************************************************* 
       Check if the code about to be downloaded can fit into the device's memory
    *******************************************************************************/
    if (appSize > (mtdGetDevMemorySize(contextPtr, ports[0]) + MTD_HEADER_SIZE))
    {
        /* App size cannot be larger than the device memory size. Code download cannot proceed */
#if DB_TEST_FLASHCODE
        MTD_DBG_ERROR("Image is larger than the device memory size!\n");
#endif
        *errCode = MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD; 
        return MTD_FAIL;
    }
        
    use_ram_checksum = 1; /* All X32X0 have a ram checksum register */

    /*******************************************************************************
       This handles downloading the binary part of the HDR file to RAM and executing
       it without modifying the flash.
    *******************************************************************************/
    
    /*******************************************************************************
           Sample of what downloading an HDR file to RAM and executing it would
           look like (non-flash installation or when another image than what's in
           flash is desired to be executed but without changing the flash).
    *******************************************************************************/

    /* Puts phy in download mode, skips over header and downloads binary to RAM
    then starts the app code running, puts PHY in parallel write mode as needed, or when
    reading an MDIO register, removes parallel write mode as needed */
    ATTEMPT(mtdParallelMdioRamDownload(contextPtr, &appData[MTD_HEADER_SIZE],(appSize-MTD_HEADER_SIZE), ports, \
                                       numPorts, erroredPorts,use_ram_checksum,errCode));    
        
    /* make sure the app code started on all ports downloaded */
    for(i = 0; i < numPorts; i++)
    {
        if(erroredPorts[i] == 0)
        {
            TRY_IO(mtdHwGetPhyRegField(contextPtr,ports[i],MTD_BOOT_STATUS_REG,4,1,&tmp));
            
            if (!tmp)
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_ERROR("App code did not start on port %d. Expected bit 4 to be 1, read 0x%04X\n",(int)ports[i],\
                              (unsigned)tmp);
                MTD_DBG_ERROR("App download failed. Exiting...\n");
#endif             
                erroredPorts[i] = MTD_APP_CODE_DID_NOT_START;
            }
            else
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_INFO("App code started on port %d.\n", (int)ports[i]);
#endif
            }
        }
        else
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_INFO( "Error downloading to port %d, so code not started.\n",(int)ports[i]);        
#endif
        }
        
    }
    
    for(i = 0; i < numPorts; i++)
    {
        if(erroredPorts[i] != 0)
        {
            *errCode = MTD_PAR_DOWNLOAD_FAILED;
            return MTD_FAIL;  /* Parallel Ram download is done, at least one port has error*/
        }
    }

    return MTD_OK;  /* Parallel Ram download is done, successful on all ports*/    
}

MTD_STATUS mtdParallelUpdateFlashImage
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 ports[],
    IN MTD_U8 appData[],
    IN MTD_U32 appSize, 
    IN MTD_U8 slaveData[],
    IN MTD_U32 slaveSize,
    IN MTD_U16 numPorts, 
    OUT MTD_U16 erroredPorts[],
    OUT MTD_U16 *errCode
)
{
    MTD_U16 tmp;           /* holds result of an mdio register read  */ 
    MTD_U16 i;
    MTD_U8 use_ram_checksum;     /* tells the RAM download function whether to -
                                    use hardware RAM checksum register or not */
    
    if(numPorts > MAX_PORTS_TO_DOWNLOAD)
    {
        *errCode = MTD_NUM_PORTS_TOO_LARGE;
        return MTD_FAIL;
    }

    for(i = 0; i < numPorts; i++) /* initialize to no errors on all ports */
    {
        erroredPorts[i] = 0;
    }

    /******************************************************************************* 
       Check if the code about to be downloaded can fit into the device's memory
    *******************************************************************************/
    if (appSize > (mtdGetDevMemorySize(contextPtr, ports[0]) + MTD_HEADER_SIZE))
    {
        /* App size cannot be larger than the device memory size. Code download cannot proceed */
#if DB_TEST_FLASHCODE
        MTD_DBG_ERROR("Image is larger than the device memory size!\n");
#endif
        *errCode = MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD; 
        return MTD_FAIL;
    }
        
    use_ram_checksum = 1; /* All X32X0 devices have a ram checksum */

    /* Flash is being updated */
    
    /*******************************************************************************
             Download slave code to phy's RAM and start it
    *******************************************************************************/

    /* Puts phy in download mode, skips over header and downloads binary to RAM
    then starts the slave code running */
    ATTEMPT(mtdParallelMdioRamDownload(contextPtr, slaveData,slaveSize, ports, \
                                       numPorts, erroredPorts, use_ram_checksum,errCode));   
        
    /* Parallel download pin must be in the OFF position here. */
    /* make sure the app code started */
    for(i = 0; i < numPorts; i++)
    {
        if(erroredPorts[i] == 0)
        {
            TRY_IO(mtdHwGetPhyRegField(contextPtr,ports[i],MTD_BOOT_STATUS_REG,4,1,&tmp));
            if (!tmp)
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_ERROR("Slave code did not start on port %d. Expected bit 4 to be 1, read 0x%04X\n",(int)ports[i],\
                               (unsigned)tmp);
                MTD_DBG_ERROR("Slave download failed.\n");
#endif             
                erroredPorts[i] = MTD_SLAVE_CODE_DID_NOT_START;            
            }
            else
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_INFO("Slave code started on port %d.\n", ports[i]);
#endif
            }
        }
        else
        {
#if DB_TEST_FLASHCODE
            MTD_DBG_INFO( "Slav code wasn't downloaded to port %d\n", ports[i]);
#endif
        }
        
    }

    /*******************************************************************************
       Write the image to flash with slave's help
    *******************************************************************************/

    ATTEMPT(mtdParallelMdioFlashDownload(contextPtr, appData, appSize, ports, numPorts, erroredPorts,errCode));    

    /*******************************************************************************
       Let slave verify image
    *******************************************************************************/

    /* Using slave code to verify image.
       This commands slave to read in entire flash image and calculate checksum and make sure
       checksum matches the checksum in the header. A failure means flash was corrupted.
      
       Another method would be to reset the phy (with SPI_CONFIG = 0) and see that the new code
       starts successfully, since a bad checksum will result in the code not being started */
#if DB_TEST_FLASHCODE
    MTD_DBG_INFO("Flash programming complete. Verifying image via slave.\n");
#endif

    /* Set the PHY Overwrite pin */ 
    /* have to do it on all ports */
    for(i = 0; i < numPorts; i++)
    {
        TRY_IO(mtdPutPhyInParallelWriteMode(contextPtr, ports[i]));
    }

    TRY_IO(mtdHwXmdioWrite(contextPtr, ports[0],COMMAND_REG,VERIFY_FLASH));

    /* writing to 1 clears all */
    TRY_IO(mtdRemovePhyFromParallelWriteMode(contextPtr, ports[0]));

    for(i = 0; i < numPorts; i++)
    {
        if(erroredPorts[i] == 0)
        {
            TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
            while(tmp == VERIFY_FLASH || tmp == MTD_SLAVE_FLASH_BUSY)
            {
                ATTEMPT(mtdWait(contextPtr, 100));  /* Don't poll phy too frequently, 
                                                       every read causes an interrupt, will leave errCode 0 on failure */
#if DB_TEST_FLASHCODE
                amuse_user();
#endif
                TRY_IO(mtdHwXmdioRead(contextPtr, ports[i],COMMAND_REG,&tmp));
            }

            if (tmp == MTD_SLAVE_OK)
            {
#if DB_TEST_FLASHCODE
                MTD_DBG_INFO("\nFlash image verified.");
                MTD_DBG_INFO("Reset SPI_CONFIG to 0 on port %d and reboot to execute new code\n", ports[i]);
#endif
            }
            else
            {
                if (tmp == MTD_SLAVE_VERIFY_ERR)
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("Flash image verify failed on port %d\n",(int)ports[i]);
#endif
                    erroredPorts[i] = MTD_VERIFY_ERR;                  
                }
                else
                {
#if DB_TEST_FLASHCODE
                    MTD_DBG_ERROR("\nExpected 0x%04X from slave on port %d got 0x%04X.\n", MTD_SLAVE_OK,(int)ports[i],(int)tmp);
#endif
                    erroredPorts[i] = MTD_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL;
                }
            }
        }
    }
        
    for(i = 0; i < numPorts; i++)
    {
        if(erroredPorts[i] != 0)
        {
            *errCode = MTD_PAR_DOWNLOAD_FAILED;            
            return MTD_FAIL;  /* at least one port has error*/
        }           
    }

    return MTD_OK;
}


