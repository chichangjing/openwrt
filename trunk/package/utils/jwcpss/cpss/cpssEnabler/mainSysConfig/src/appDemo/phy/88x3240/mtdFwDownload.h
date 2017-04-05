#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions using MDIO access to download firmware
into the internal RAM or attached flash memory of the Marvell 88X3240 
ethernet PHY.
********************************************************************/
#ifndef MTDDL_H
#define MTDDL_H

#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

#if DB_TEST_ENVIRONMENT
#define DB_TEST_FLASHCODE   1 /* set to 0 for host environment*/
                              /* (removes many extra print statements) */
                              /* set to 1 to build sample code */
                              /* to interface to Marvell DB via USB2SMI board */
                              /* (extra debug statements during flash/ram download) */
#else
#define DB_TEST_FLASHCODE   0
#endif

#define MTD_MAX_APP_SIZE (216*1024UL)

#define MTD_MAX_SLAVE_SIZE (28*1024)

#define MTD_HEADER_SIZE 32

/* At least one port during a parallel RAM or flash download failed */
#define MTD_PAR_DOWNLOAD_FAILED             0xFFDE
/*Image larger than the target device's memory*/
#define MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD     0xFFDF
/* Slave code did not start. - Slave code failed to download properly. */
#define MTD_SLAVE_CODE_DID_NOT_START        0xFFE0
/* Flash verifed FAILED! Flash probably corrupted */
#define MTD_VERIFY_ERR                      0xFFE1
/* Unknown error, downloading the flash failed! */
#define MTD_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL  0xFFE2
/* App code did not start. - App code failed to download properly in to the RAM. */
#define MTD_APP_CODE_DID_NOT_START          0xFFE3
/* App code failed. - Number of ports to download is greater than maximum ports */
#define MTD_NUM_PORTS_TOO_LARGE             0xFFE4

/* Error reading or writing MDIO register */
#define MTD_IO_ERROR                        0xFFE5
/*size must be an even number of bytes*/
#define MTD_SIZE_NOT_EVEN                   0xFFE6
/* Slave encountered error while erasing flash */
#define MTD_ERR_ERASING_FLASH               0xFFE7
/* unexpected value read back from download code */
#define MTD_ERR_VALUE_READ_BACK             0xFFE8
/* Did not get MTD_SLAVE_OK for writing the data */
#define MTD_ERR_START_WRITE_DATA            0xFFE9
/* Slave failed to get all the data correctly*/
#define MTD_START_WRITE_DATA                0xFFEA
/* Some kind of error occurred on Slave */
#define MTD_ERR_ON_SLAVE                    0xFFEB
/* Checksum error */
#define MTD_ERR_CHECKSUM                    0xFFEC
/* Slave didn't write enough words to flash. Some kind of error occurred*/
#define MTD_ERR_SLAVE_WRITE_FULL            0xFFED
/* last transfer failed */
#define MTD_ERR_LAST_TRANSFER               0xFFEE
/* RAM checksum register (present only in SFT910X RevB or later) had wrong checksum */
#define MTD_RAM_HW_CHECKSUM_ERR             0xFFEF
/* PHY wasn't waiting in download mode */
#define MTD_PHY_NOT_IN_DOWNLOAD_MODE        0xFFFF


/******************************************************************************
 MTD_STATUS mtdUpdateFlashImage(
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     IN MTD_U8 appData[],
     IN MTD_U32 appSize, 
     IN MTD_U8 slaveData[],
     IN MTD_U32 slaveSize,
     OUT MTD_U16 *errCode);
                                  
 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    appData[] - application code to be downloaded in to the flash
    appSize - file size
    slaveData[] - slave code to be downloaded in to the RAM
    slaveSize - file size of the slave code

 Outputs:
    errCode - if function returns MTD_FAIL, more information is contained in errCode
    as follows:
         
         MDIO IO Error 
         MTD_IO_ERROR

         Image larger than the target device's memory
         MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD     

         PHY wasn't waiting in download mode
         MTD_PHY_NOT_IN_DOWNLOAD_MODE        

         RAM checksum register bad checksum (slave code didn't download correctly)
         MTD_RAM_HW_CHECKSUM_ERR             
         Slave code did not start executing
         MTD_SLAVE_CODE_DID_NOT_START        
         
         size must be an even number of bytes
         MTD_SIZE_NOT_EVEN               
         Slave encountered error while erasing flash 
         MTD_ERR_ERASING_FLASH           
         unexpected value read back from download code 
         MTD_ERR_VALUE_READ_BACK         
         Did not get MTD_SLAVE_OK for writing the data 
         MTD_ERR_START_WRITE_DATA        
         Slave failed to get all the data correctly
         MTD_START_WRITE_DATA            
         Some kind of error occurred on Slave 
         MTD_ERR_ON_SLAVE                
         Checksum error 
         MTD_ERR_CHECKSUM                
         Slave didn't write enough words to flash. Some kind of error occurred
         MTD_ERR_SLAVE_WRITE_FULL        
         last transfer failed 
         MTD_ERR_LAST_TRANSFER                    

         Flash verifed FAILED! Flash probably corrupted .
         MTD_VERIFY_ERR                      
         Unknown error, downloading the flash failed! 
         MTD_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL  

 Returns:
    MTD_OK if successful. MTD_FAIL if not.

 Description:
    The function first checks if the app code size is greater than what it 
    should be and returns an error if that was the case.
    Then it calls mtdPutPhyInMdioDownloadMode() to put the PHY in download mode by
    raising the SPI_CONFIG bit and downloads the slave code in to the RAM and starts executing. 
    Once the flash is written with the help of the slave, slave verifies
    the flash program and returns errors if the image is corrupted (the slave
    reads back the image from flash and verifies the checksum matches the expected checksum).
    Any error occurs returns MTD_FAIL and additional information in "errCode".

 Side effects:
    None.

 Notes/Warnings:
    This function calls two main functions ...
    1. mtdMdioRamDownload() to download the slave helper code
    2. mtdMdioFlashDownload() to update the flash using the slave helper code

    The function asserts the SPI_CONFIG when it begins the code download. But it does
    not lower SPI_CONFIG after the download. In order to execute the newly downloaded 
    code, SPI_CONFIG must be lowered by calling mtdRemovePhyDownloadMode() and the 
    PHY must be reset to use the newly updated flash image.

 Example:
    FILE *hFile, *sFile;                        
    MTD_U8 appData[MTD_MAX_APP_SIZE+MTD_HEADER_SIZE];        // hdr file image
    MTD_U8 slaveCode[MTD_MAX_SLAVE_SIZE];    // slave code image
    MTD_STATUS retStatus;
    MTD_U16 error;

    fileSize = mtdOpenReadFile(contextPtr, port, &hFile, "image_name.hdr", appData,
                                    (MTD_MAX_APP_SIZE+ MTD_HEADER_SIZE));
    slaveFileSize = mtdOpenReadFile(contextPtr, port, &sFile, 
                                    "x3240flashdlslave_0_3_0_0_6147.bin", slaveCode,
                                    MTD_MAX_SLAVE_SIZE);
    retStatus = mtdUpdateFlashImage(contextPtr, port, appData, fileSize, slaveCode, 
                                 slaveFileSize, &error);
    if (retStatus == MTD_FAIL)
        ...check "error" for what happened/type of error that occurred
    
    fclose(hFile);
    fclose(sFile);

******************************************************************************/ 
MTD_STATUS mtdUpdateFlashImage(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U8 appData[],
    IN MTD_U32 appSize, 
    IN MTD_U8 slaveData[],
    IN MTD_U32 slaveSize,
    OUT MTD_U16 *errCode);

/******************************************************************************
 MTD_STATUS mtdUpdateRamImage(
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port,
     IN MTD_U8 appData[],
     IN MTD_U32 appSize
     OUT MTD_U16 *errCode);
                                  
 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    appData[] - application code to be downloaded in to the flash
    appSize - file size 

 Outputs:
    errCode - if return status is MTD_FAIL, errCode may contain additional
    information about the error. If it does, it will be one of the following:

         MDIO IO Error
         MTD_IO_ERROR

         size must be an even number of bytes
         MTD_SIZE_NOT_EVEN               
                  
         Image larger than the target device's memory
         MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD     

         PHY wasn't waiting in download mode
         MTD_PHY_NOT_IN_DOWNLOAD_MODE        

         RAM checksum register bad checksum
         MTD_RAM_HW_CHECKSUM_ERR             
     
         App code did not start execution (possible corruption) 
         MTD_APP_CODE_DID_NOT_START        
                  
 Returns:
    MTD_OK if successful, MTD_FAIL if an error was detected

 Description:
    The function first checks if the app code size is greater than what it 
    should be and returns an error if that was the case.

    It calls mtdPutPhyInMdioDownloadMode() to put the PHY in download mode by
    raising the SPI_CONFIG bit (required to execute from RAM).

    Then it downloads the app code in to the RAM and starts executing. Returns
    MTD_FAIL if any error is encountered, with more information in "errCode".
    
 Side effects:
    None

 Notes/Warnings:
    None

Example:
    FILE *hFile;
    MTD_U8 appData[MTD_MAX_APP_SIZE+MTD_HEADER_SIZE];        // hdr file image
    MTD_U16 error;
    MTD_STATUS retStatus;
    
    fileSize = mtdOpenReadFile(contextPtr, port, &hFile, "image_name.hdr", appData,
                                    (MTD_MAX_APP_SIZE+MTD_HEADER_SIZE));
    retStatus = mtdUpdateRamImage(contextPtr, port, appData, fileSize, &error);
    if (retStatus == MTD_FAIL)...check "error" for possible error codes    

    fclose(hFile);
******************************************************************************/
MTD_STATUS mtdUpdateRamImage(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U8 appData[],
    IN MTD_U32 appSize,
    OUT MTD_U16 *errCode);

/******************************************************************************
 MTD_STATUS mtdParallelUpdateRamImage(
     IN CTX_PTR_TYPE contextPtr,
     IN MTD_U16 ports[],
     IN MTD_U8 appData[],
     IN MTD_U32 appSize, 
     IN MTD_U16 numPorts,
     OUT MTD_U16 erroredPorts[]
     OUT MTD_U16 *errCode);
                                  

 Inputs:
    contextPtr - pointer to host's private data
    ports[] - list of port numbers to which the application code needs to be downloaded
    appData[] - application code to be downloaded in to the flash
    appSize - file size
    numPorts - number of ports to download (any number from 0-31)

 Outputs:
    errCode - If return status is MTD_FAIL, errCode will contain one of the following:

        MDIO IO Error
        MTD_IO_ERROR
        Number of ports exceeds MAX_PORTS_TO_DOWNLOAD
        MTD_NUM_PORTS_TOO_LARGE

        size must be an even number of bytes
        MTD_SIZE_NOT_EVEN               
        
        Size of image exceeds size of RAM
        MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD

        At least one port failed
        MTD_PAR_DOWNLOAD_FAILED             
        
    
    erroredPorts[] - Failure code on ports that had error during download.

        The following are the failure codes stored in the array erroredPorts[]
    
        MDIO IO Error
        MTD_IO_ERROR

        Image larger than the target device's memory
        MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD     

        PHY wasn't waiting in download mode
        MTD_PHY_NOT_IN_DOWNLOAD_MODE        

        RAM checksum register bad checksum
        MTD_RAM_HW_CHECKSUM_ERR             

        App code did not start execution (possible corruption) 
        MTD_APP_CODE_DID_NOT_START        
                
 Returns:
    Download successful on all ports - MTD_OK
    Download not successful on at least one port - MTD_FAIL

    
 Description:
    The function first checks if the number of ports to download is greater
    than maximum ports to download and if true returns an error.
    The code then checks if the app code size is greater than what it 
    should be and returns an error if that was the case.
    Then it downloads the app code in to the RAM and starts executing.           

 Side effects:
    None.

 Notes/Warnings:
    This function calls mtdParallelMdioRamDownload()    
    If this function returns an error in errCode, then it should be assumed
    possibly no ports were downloaded successfully.

Example:
    FILE *hFile;
    MTD_U8 appData[MTD_MAX_APP_SIZE+MTD_HEADER_SIZE];        // hdr file image
    MTD_U16 error;
    MTD_STATUS retStatus;
    
    fileSize = mtdOpenReadFile(contextPtr, port, &hFile, "image_name.hdr", appData,
                                    (MTD_MAX_APP_SIZE+MTD_HEADER_SIZE));
    retStatus = mtdParallelUpdateRamImage(contextPtr, port, 
                                          appData, fileSize, noOfPorts,
                                          erroredPorts,&error);
    if (retStatus == MTD_FAIL)
    {
        ...check error, and/or all ports in erroredPorts for failure codes
    }
    fclose(hFile);

******************************************************************************/
MTD_STATUS mtdParallelUpdateRamImage(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 ports[],
    IN MTD_U8 appData[],
    IN MTD_U32 appSize, 
    IN MTD_U16 numPorts,
    OUT MTD_U16 erroredPorts[],
    OUT MTD_U16 *errCode);

/******************************************************************************       
MTD_U16 mtdParallelUpdateFlashImage(CTX_PTR_TYPE contextPtr,
       MTD_U32 ports[], MTD_U8 appData[],
       MTD_U32 appSize, MTD_U8 slaveData[],MTD_U32 slaveSize,
       MTD_U16 numPorts, MTD_U16 erroredPorts[]) 
                                  

 Inputs:
    contextPtr - pointer to host's private data
    ports[] - list of port numbers to which the application code needs to be downloaded
    appData[] - application code to be downloaded in to the flash
    appSize - file size
    slaveData[] - slave code to be downloaded in to the RAM
    slaveSize - file size of the slave code
    numPorts - number of ports to download (any number from 0-31), should be
               one per chip (as multiple ports per chip share the same flash)

 Outputs:
    erroredPorts[] - Failure code on ports that had error during download.

        The following are the failure codes stored in the array erroredPorts[]

        MDIO IO Error 
        MTD_IO_ERROR
            
        PHY wasn't waiting in download mode
        MTD_PHY_NOT_IN_DOWNLOAD_MODE        
        
        RAM checksum register bad checksum (slave code didn't download correctly)
        MTD_RAM_HW_CHECKSUM_ERR             
        Slave code did not start executing
        MTD_SLAVE_CODE_DID_NOT_START        
        
        size must be an even number of bytes
        MTD_SIZE_NOT_EVEN               
        Slave encountered error while erasing flash 
        MTD_ERR_ERASING_FLASH           
        unexpected value read back from download code 
        MTD_ERR_VALUE_READ_BACK         
        Did not get MTD_SLAVE_OK for writing the data 
        MTD_ERR_START_WRITE_DATA        
        Slave failed to get all the data correctly
        MTD_START_WRITE_DATA            
        Some kind of error occurred on Slave 
        MTD_ERR_ON_SLAVE                
        Checksum error 
        MTD_ERR_CHECKSUM                
        Slave didn't write enough words to flash. Some kind of error occurred
        MTD_ERR_SLAVE_WRITE_FULL        
        last transfer failed 
        MTD_ERR_LAST_TRANSFER                    
    
        Flash verifed FAILED! Flash probably corrupted .
        MTD_VERIFY_ERR                      
        Unknown error, downloading the flash failed! 
        MTD_UNKNOWN_DOWNLOAD_TO_FLASH_FAIL  


     errCode - 0 if no overall error occurred, or one of the following:
        Image larger than the target device's memory
        MTD_IMAGE_TOO_LARGE_TO_DOWNLOAD     
        Number of ports entered is too large
        MTD_NUM_PORTS_TOO_LARGE 
        MDIO Error of some kind 
        MTD_IO_ERROR
               
 Returns:
    Download successful on all ports - MTD_OK
    Download not successful on atleast one port - MTD_FAIL

 Description:
    The function first checks if the number of ports to download is greater
    than maximum ports to download and if true returns an error.
    The code then checks if the app code size is greater than what it 
    should be and returns an error if that was the case.
    Then it raises the SPI_CONFIG to put the PHY in download mode and 
    downloads the slave code into the RAM and starts executing. 
    The slave code helps program the app code into the flash and verifies 
    the image after downloading by reading back and matching against the checksum.

 Side effects:
    None.

 Notes/Warnings:
    The function sets SPI_CONFIG when it begins the code download. But it does
    not lower SPI_CONFIG after the download. In order to execute the downloaded 
    code, SPI_CONFIG must be lowered by calling mtdRemovePhyDownloadMode() 
    and the PHY must be reset.

    While using X3240, include only one port on each chip (as 4 ports share
    the same flash) in the list of port numbers ('ports' input)

 Example:
    FILE *hFile, *sFile;                        
    MTD_U8 appData[MTD_MAX_APP_SIZE+MTD_HEADER_SIZE];        // hdr file image
    MTD_U8 slaveCode[MTD_MAX_SLAVE_SIZE];    // slave code image
    MTD_U16 error;
    MTD_STATUS retStatus;
    
    fileSize = mtdOpenReadFile(contextPtr, port, &hFile, 
                               "image_name.hdr", appData, 
                               (MTD_MAX_APP_SIZE + MTD_HEADER_SIZE)); 
    slaveFileSize = mtdOpenReadFile(contextPtr, port, &sFile, 
                                    "flashdownloadslave.bin", slaveCode, 
                                    MTD_MAX_SLAVE_SIZE);
    retStatus = mtdParallelUpdateFlashImage(contextPtr, port, appData, 
                                            fileSize, slaveCode, 
                                            slaveFileSize, noOfPorts,
                                            erroredPorts,&error);
    if (retStatus == MTD_FAIL)
    {
        ...check error, and/or all ports in erroredPorts for failure codes
    }                                
    
    fclose(hFile);
    fclose(sFile);

******************************************************************************/ 
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
);

#if C_LINKAGE
#if defined __cplusplus 
}
#endif 
#endif

#endif /* defined MTDDL_H */
