#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
interfacing with the host's hardware-specific MDIO and general-purpose
IO in order to control and query the Marvell 88X3240 ethernet PHY.

These functions as written were tested with a USB-interface to
access Marvell's EVK. These functions must be ported to 
the host's specific platform.
********************************************************************/
#ifndef MTD_HWCNTL_H
#define MTD_HWCNTL_H

#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

/* This type is provided as a type for the host to pass a context parameter
   into/through the API for whatever purposes the host may need */
typedef void *CTX_PTR_TYPE;   

/* Parallel Download - RAM & FLASH */
#define MAX_PORTS_TO_DOWNLOAD        32    /* user input */
/*MTD_U32 portsToDownload[MAX_PORTS_TO_DOWNLOAD] = {0, 1, 3, 5};
  MTD_U16 errorInPorts[MAX_PORTS_TO_DOWNLOAD] = {0,0,0,0};*/


/*******************************************************************
   PORT THESE FUNCTIONS
 *******************************************************************/

/******************************************************************************
MTD_STATUS mtdHwXmdioWrite
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg, 
    IN MTD_U16 value
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31 
    dev - MMD device address, 0-31
    reg - MMD register address
    value - data to write

 Outputs:
    None

 Returns:
    MTD_OK - wrote successfully
    MTD_FAIL - an error occurred

 Description:
    Writes a 16-bit word to the MDIO
    Address is in format X.Y.Z, where X selects the MDIO port (0-31), Y selects
    the MMD/Device (0-31), and Z selects the register.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdHwXmdioWrite
( 
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg, 
    IN MTD_U16 value
);

/******************************************************************************
 MTD_STATUS mtdHwXmdioRead
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port, 
     IN MTD_U16 dev, 
     IN MTD_U16 reg,
     OUT MTD_U16 *data
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31 
    dev - MMD device address, 0-31
    reg - MMD register address

 Outputs:
    data - Returns 16 bit word from the MDIO

 Returns:
    MTD_OK - read successful
    MTD_FAIL - read was unsuccessful
    
 Description:
    Reads a 16-bit word from the MDIO
    Address is in format X.Y.Z, where X selects the MDIO port (0-31), Y selects the 
    MMD/Device (0-31), and Z selects the register.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdHwXmdioRead
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg,
    OUT MTD_U16 *data
);


/*******************************************************************************
  MTD_STATUS mtdHwGetPhyRegField
  (
      IN  CTX_PTR_TYPE contextPtr, 
      IN  MTD_U16      port, 
      IN  MTD_U16      dev, 
      IN  MTD_U16      regAddr,
      IN  MTD_U8       fieldOffset,
      IN  MTD_U8       fieldLength,
      OUT MTD_U16      *data
  );

  Inputs:  
        contextPtr - pointer to host's private data
        port       - The port number, 0-31
        dev        - The MMD device, 0-31 
        regAddr    - The register's address
        fieldOffset - The field start bit index. (0 - 15)
        fieldLength - Number of bits to read

  Outputs:
        data        - The read register field

  Returns:
        MTD_OK on success, or
        MTD_FAIL  - on error

  Description:
        This function reads a specified field from a port's phy register.
        It first reads the register, then returns the specified bit
        field from what was read.

  Side effects:
     None

  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 16

        Reading a register with latched bits may clear the latched bits.
        Use with caution for registers with latched bits.

        To operate on several bits within a register which has latched bits
        before reading the register again, first read the register with
        mtdHwXmdioRead() to get the register value, then operate on the
        register data repeatedly using mtdHwGetRegFieldFromWord() to
        take apart the bit fields without re-reading the register again.

        This approach should also be used to reduce IO to the PHY when reading
        multiple bit fields (do a single read, then grab different fields 
        from the register by using mtdHwGetRegFieldFromWord() repeatedly).

*******************************************************************************/
MTD_STATUS mtdHwGetPhyRegField
(
    IN  CTX_PTR_TYPE contextPtr, 
    IN  MTD_U16      port, 
    IN  MTD_U16      dev, 
    IN  MTD_U16      regAddr,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U16      *data
);

/*******************************************************************************
  MTD_STATUS mtdHwSetPhyRegField
  (
      IN CTX_PTR_TYPE contextPtr, 
      IN MTD_U16      port, 
      IN MTD_U16      dev, 
      IN MTD_U16      regAddr,
      IN MTD_U8       fieldOffset,
      IN MTD_U8       fieldLength,
      IN MTD_U16      data
  );
  
  Inputs:
        contextPtr - pointer to host's private data  
        port       - The port number, 0-31
        dev        - The MMD device, 0-31
        regAddr    -  The register's address
        fieldOffset - The field start bit index. (0 - 15)
        fieldLength - Number of bits to write
        data        - Data to be written.
  
  Outputs:
        None.
  
  Returns:
        MTD_OK on success, or
        MTD_FAIL  - on error

  Description:
        This function writes to specified field in a port's phy register.
  
  Side effects:
     None
  
  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 16.
  
*******************************************************************************/
MTD_STATUS mtdHwSetPhyRegField
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16      port, 
    IN MTD_U16      dev, 
    IN MTD_U16      regAddr,
    IN MTD_U8       fieldOffset,
    IN MTD_U8       fieldLength,
    IN MTD_U16      data
);

/*******************************************************************************
  MTD_STATUS mtdHwGetRegFieldFromWord
  (
      IN  MTD_U16      regData,
      IN  MTD_U8       fieldOffset,
      IN  MTD_U8       fieldLength,
      OUT MTD_U16      *data
  );

  Inputs:  
        regData    - The data previously read from the register
        fieldOffset - The field start bit index. (0 - 15)
        fieldLength - Number of bits to read

  Outputs:
        data        - The data from the associated bit field

  Returns:
        MTD_OK always

  Description:
        This function grabs a value from a bitfield within a word. It could
        be used to get the value of a bitfield within a word which was previously
        read from the PHY.

  Side effects:
     None

  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 16

        This register acts on data passed in. It does no hardware access.

        This function is useful if you want to do 1 register access and then
        get different bit fields without doing another register access either
        because there are latched bits in the register to avoid another read,
        or to keep hardware IO down to improve performance/throughput.

        Example:

        MTD_U16 aword, nibble1, nibble2;

        mtdHwXmdioRead(contextPtr,0,MTD_TUNIT_IEEE_PCS_CTRL1,&aword); // Read 3.0 from port 0
        mtdHwGetRegFieldFromWord(aword,0,4,&nibble1); // grab first nibble
        mtdHwGetRegFieldFromWord(aword,4,4,&nibble2); // grab second nibble
                
*******************************************************************************/
MTD_STATUS mtdHwGetRegFieldFromWord
(
    IN  MTD_U16      regData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U16      *data
);

/*******************************************************************************
  MTD_STATUS mtdHwSetRegFieldToWord
  (
      IN  MTD_U16      regData,
      IN  MTD_U16      bitFieldData,
      IN  MTD_U8       fieldOffset,
      IN  MTD_U8       fieldLength,
      OUT MTD_U16      *data
  );

  Inputs:  
        regData - original word to modify
        bitFieldData   - The data to set the register field to 
                     (must be <= largest value for that bit field,
                      no range checking is done by this function)
        fieldOffset - The field start bit index. (0 - 15)
        fieldLength - Number of bits to write to regData

  Outputs:
        data        - The new/modified regData with the bitfield changed

  Returns:
        MTD_OK always

  Description:
        This function write a value to a bitfield within a word. 

  Side effects:
     None

  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 16

        This register acts on data passed in. It does no hardware access.

        This function is useful to reduce IO if several bit fields of a register
        that has been read is to be changed before writing it back.

        MTD_U16 aword;

        mtdHwXmdioRead(contextPtr,0,MTD_TUNIT_IEEE_PCS_CTRL1,&aword); // Read 3.0 from port 0
        mtdHwSetRegFieldToWord(aword,2,0,4,&aword); // Change first nibble to 2
        mtdHwSetRegFieldToWord(aword,3,4,4,&aword); // Change second nibble to 3
        
*******************************************************************************/
MTD_STATUS mtdHwSetRegFieldToWord
(
    IN  MTD_U16      regData,
    IN  MTD_U16      bitFieldData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U16      *data
);


/******************************************************************************
MTD_STATUS mtdWait
(
    IN CTX_PTR_TYPE contextPtr, 
    IN unsigned x
);

 Inputs:
    contextPtr - pointer to host's private data
    x - number of milliseconds to wait

 Outputs:
    None

 Returns:
    MTD_OK if wait was successful, MTD_FAIL otherwise

 Description:
    Waits X milliseconds

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
MTD_STATUS mtdWait
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_UINT x
);

#if 0
#if DB_TEST_ENVIRONMENT
/******************************************************************************
MTD_INT mtdOpenReadFile
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN FILE **pfp, 
    IN MTD_8 *fname, 
    OUT MTD_U8 *data,
    IN MTD_UINT max_data_size
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    *fname - Header file name to open
    max_data_size - Size of the data to be downloaded

 Outputs:
    **pfp - Pointer to the FILE structure which will be opened/read    
    *data - hdr file image read in from the file

 Returns:
    0 - failure
    int - no of bytes read

 Description:
   Opens the file with file name fname as a binary file and reads 
   max_data_size bytes into the array data then returns the number of
   bytes actually read.

   Returns 0 on failure.

 Side effects:
    None

 Notes/Warnings:
    The header file stdio.h needs to be included before compiling and using 
    this function

Example:
    See the "Example" section of the function declarations of mtdUpdateFlashImage()
    and mtdMdioRamDownload() to understand how mtdOpenReadFile() can be used.
******************************************************************************/
MTD_INT mtdOpenReadFile
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN FILE **pfp, 
    IN MTD_8 *fname, 
    OUT MTD_U8 *data,
    IN MTD_UINT max_data_size
);

#endif
#endif

#if C_LINKAGE
#if defined __cplusplus 
}
#endif 
#endif

#endif /* defined MTD_HWCNTL_H */



