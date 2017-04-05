#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
interfacing with the the Macsec registers in the C-unit. These
registers require special handling from other XGMII MDIO
registers.

These functions as written were tested with a USB-interface to
access Marvell's EVK. These functions must be ported to 
the host's specific platform.
********************************************************************/
#ifndef MTD_HWMSECCNTL_H
#define MTD_HWMSECCNTL_H

#if C_LINKAGE
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

/******************************************************************************
MTD_STATUS mtdHwMsecWrite
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg, 
    IN MTD_U32 value,
    IN MTD_BOOL useIndirectAddressing
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31 
    dev - MMD device address, 0-31
    reg - MMD register address (must be even)
    value - data to write
    useIndirectAddressing - set to MTD_FALSE to address MacSec registers directly,
        set to MTD_TRUE to address MacSec registers indirectly (to address
        errata in A0 and earlier set to MTD_TRUE)

 Outputs:
    None

 Returns:
    MTD_OK - wrote successfully
    MTD_FAIL - an error occurred

 Description:
    Writes a 32-bit word to the Macsec register in 2 writes, first the low
    16-bits of "value" to the address "reg", then the upper 16-bits of
    "value" to "reg+1".

    Address is in format X.Y.Z, where X selects the MDIO port (0-31), Y selects
    the MMD/Device (0-31), and Z selects the register.

    If useIndirectAddressing is MTD_TRUE, uses T unit processor to read/write
    MacSec registers indirectly as a workaround to an errata in the A0
    and earlier revisions.

 Side effects:
    None

 Notes/Warnings:
    "reg" must be even, or MTD_FAIL is returned.

    Macsec registers must always be written in pairs, first low word, then upper
    word.

    Verify firmware supports indirect method before setting useIndirectAddressing
    to MTD_TRUE.

******************************************************************************/
MTD_STATUS mtdHwMsecWrite
( 
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg, 
    IN MTD_U32 value,
    IN MTD_BOOL useIndirectAddressing
);

/******************************************************************************
 MTD_STATUS mtdHwMsecRead
 (
     IN CTX_PTR_TYPE contextPtr, 
     IN MTD_U16 port, 
     IN MTD_U16 dev, 
     IN MTD_U16 reg,
     OUT MTD_U32 *data,
     IN MTD_BOOL useIndirectAddressing
 );

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31 
    dev - MMD device address, 0-31
    reg - MMD register address (must be even)
    useIndirectAddressing - set to MTD_FALSE to address MacSec registers directly,
        set to MTD_TRUE to address MacSec registers indirectly (to address
        errata in A0 and earlier set to MTD_TRUE)

 Outputs:
    data - Returns 32-bit word from the MDIO

 Returns:
    MTD_OK - read successful
    MTD_FAIL - read was unsuccessful
    
 Description:
    Reads two MDIO registers using the Macsec indirect read method outlined in the
    datasheet, and returns the low 16-bits from the address "reg" and in the upper
    16-bits, returns the data from "reg + 1".

    Address is in format X.Y.Z, where X selects the MDIO port (0-31), Y selects the 
    MMD/Device (0-31), and Z selects the register.

    If useIndirectAddressing is MTD_TRUE, uses T unit processor to read/write
    MacSec registers indirectly as a workaround to an errata in the A0
    and earlier revisions.

 Side effects:
    None

 Notes/Warnings:
    "reg" must be even or MTD_FAIL is returned.

    Verify firmware supports indirect method before setting useIndirectAddressing
    to MTD_TRUE.

******************************************************************************/
MTD_STATUS mtdHwMsecRead
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg,
    OUT MTD_U32 *data,
    IN MTD_BOOL useIndirectAddressing
);

/******************************************************************************
MTD_STATUS mtdHwMsecWrite
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 dev,
    IN MTD_U16 reg,
    IN MTD_U64 value,
    IN MTD_BOOL useIndirectAddressing
);

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    dev - MMD device address, 0-31
    reg - MMD register address (must be even)
    value - data to write
    useIndirectAddressing - set to MTD_FALSE to address MacSec registers directly,
        set to MTD_TRUE to address MacSec registers indirectly (to address
        errata in A0 and earlier set to MTD_TRUE)

 Outputs:
    None

 Returns:
    MTD_OK - wrote successfully
    MTD_FAIL - an error occurred

 Description:
    Writes a 64-bit word to the Macsec register in 2 writes, first the low
    16-bits of "value" to the address "reg", then the second 16-bits of
    "value" to "reg+1", then the third 16-bits of "value" to "reg+2",
    finally the fourth 16-bits of "value" to "reg+3".

    Address is in format X.Y.Z, where X selects the MDIO port (0-31), Y selects
    the MMD/Device (0-31), and Z selects the register.

    If useIndirectAddressing is MTD_TRUE, uses T unit processor to read/write
    MacSec registers indirectly as a workaround to an errata in the A0
    and earlier revisions.

 Side effects:
    None

 Notes/Warnings:
    "reg" must be on a 4 word boundary, or MTD_FAIL is returned.

    Macsec registers must always be written in pairs, first low word, then upper
    word.

    Verify firmware supports indirect method before setting useIndirectAddressing
    to MTD_TRUE.

******************************************************************************/
MTD_STATUS mtdHwMsecDoubleWrite
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 dev,
    IN MTD_U16 reg,
    IN MTD_U64 value,
    IN MTD_BOOL useIndirectAddressing
);


/*******************************************************************************
  MTD_STATUS mtdHwGetMsecPhyRegField
  (
      IN  CTX_PTR_TYPE contextPtr, 
      IN  MTD_U16      port, 
      IN  MTD_U16      dev, 
      IN  MTD_U16      regAddr,
      IN  MTD_U8       fieldOffset,
      IN  MTD_U8       fieldLength,
      OUT MTD_U32      *data
  );

  Inputs:  
        contextPtr - pointer to host's private data
        port       - The port number, 0-31
        dev        - The MMD device, 0-31 
        regAddr    - The register's address
        fieldOffset - The field start bit index. (0 - 31)
        fieldLength - Number of bits to read

  Outputs:
        data        - The read register field

  Returns:
        MTD_OK on success, or
        MTD_FAIL  - on error

  Description:
        This function reads a specified field from a Macsec register.
        It first reads the register, then returns the specified bit
        field from what was read.

        It uses mtdHwMsecRead() to do indirect reads using the
        Macsec indirect read method outlines in the datasheet.

        It operates on 32-bit fields as defined in the Macsec
        datasheet.

  Side effects:
     None

  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 32.

        Reading a register with latched bits may clear the latched bits.
        Use with caution for registers with latched bits.

        To operate on several bits within a register which has latched bits
        before reading the register again, first read the register with
        mtdHwMsecRead() to get the register value, then operate on the
        register data repeatedly using mtdHwGetRegFieldFromLongWord() to
        take apart the bit fields without re-reading the register again.

        This approach should also be used to reduce IO to the PHY when reading
        multiple bit fields (do a single read, then grab different fields 
        from the register by using mtdHwGetRegFieldFromLongWord() repeatedly).

        "regAddr" must be even or MTD_FAIL is returned.

*******************************************************************************/
MTD_STATUS mtdHwGetMsecPhyRegField
(
    IN  CTX_PTR_TYPE contextPtr, 
    IN  MTD_U16      port, 
    IN  MTD_U16      dev, 
    IN  MTD_U16      regAddr,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U32      *data
);

/*******************************************************************************
  MTD_STATUS mtdHwSetMsecPhyRegField
  (
      IN CTX_PTR_TYPE contextPtr, 
      IN MTD_U16      port, 
      IN MTD_U16      dev, 
      IN MTD_U16      regAddr,
      IN MTD_U8       fieldOffset,
      IN MTD_U8       fieldLength,
      IN MTD_U32      data
  );
  
  Inputs:
        contextPtr - pointer to host's private data  
        port       - The port number, 0-31
        dev        - The MMD device, 0-31
        regAddr    -  The register's address
        fieldOffset - The field start bit index. (0 - 31)
        fieldLength - Number of bits to write
        data        - Data to be written.
  
  Outputs:
        None.
  
  Returns:
        MTD_OK on success, or
        MTD_FAIL  - on error

  Description:
        This function writes to specified field in a Macsec register.

        It operates on 32-bit registers, 2 x 16-bit registers at-a-time
        using the other Macsec interface functions, mtdHwMsecRead() and
        mtdHwMsecWrite().
  
  Side effects:
     None
  
  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 32.

        "regAddr" must be even or MTD_FAIL is returned.
  
*******************************************************************************/
MTD_STATUS mtdHwSetMsecPhyRegField
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16      port, 
    IN MTD_U16      dev, 
    IN MTD_U16      regAddr,
    IN MTD_U8       fieldOffset,
    IN MTD_U8       fieldLength,
    IN MTD_U32      data
);

/*******************************************************************************
  MTD_STATUS mtdHwGetRegFieldFromLongWord
  (
      IN  MTD_U32      regData,
      IN  MTD_U8       fieldOffset,
      IN  MTD_U8       fieldLength,
      OUT MTD_U32      *data
  );

  Inputs:  
        regData    - The data previously read from the register
        fieldOffset - The field start bit index. (0 - 31)
        fieldLength - Number of bits to read

  Outputs:
        data        - The data from the associated bit field

  Returns:
        MTD_OK always

  Description:
        This function grabs a value from a bitfield within a long word. It could
        be used to get the value of a bitfield within a word which was previously
        read from the Macsec.

  Side effects:
     None

  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 32.

        This register acts on data passed in. It does no hardware access.

        This function is useful if you want to do 1 register access and then
        get different bit fields without doing another register access either
        because there are latched bits in the register to avoid another read,
        or to keep hardware IO down to improve performance/throughput.

        Example:

        MTD_U32 aword, nibble1, nibble2;

        mtdHwMsecRead(contextPtr,0,31,0x8C00,&aword); // Reads 31.8c00 and 31.8c01 from port 0
                                                      // and returns them in the low/high
                                                      // 16-bits of "aword" respectively.
        mtdHwGetRegFieldFromLongWord(aword,0,4,&nibble1); // grab first nibble
        mtdHwGetRegFieldFromLongWord(aword,4,4,&nibble2); // grab second nibble
                
*******************************************************************************/
MTD_STATUS mtdHwGetRegFieldFromLongWord
(
    IN  MTD_U32      regData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U32      *data
);

/*******************************************************************************
  MTD_STATUS mtdHwSetRegFieldToLongWord
  (
      IN  MTD_U32      regData,
      IN  MTD_U32      bitFieldData,
      IN  MTD_U8       fieldOffset,
      IN  MTD_U8       fieldLength,
      OUT MTD_U32      *data
  );

  Inputs:  
        regData - original word to modify
        bitFieldData   - The data to set the register field to 
                     (must be <= largest value for that bit field,
                      no range checking is done by this function)
        fieldOffset - The field start bit index. (0 - 32)
        fieldLength - Number of bits to write to regData

  Outputs:
        data        - The new/modified regData with the bitfield changed

  Returns:
        MTD_OK always

  Description:
        This function write a value to a bitfield within a long (32-bit) word.

  Side effects:
     None

  Notes/Warnings:
        The sum of fieldOffset & fieldLength parameters must be smaller-
        equal to 32.

        This register acts on data passed in. It does no hardware access.

        This function is useful to reduce IO if several bit fields of a register
        that has been read is to be changed before writing it back.

        MTD_U32 aword;

        mtdHwMsecRead(contextPtr,0,31,0x8C00,&aword); // Reads 31.8c00 and 31.8c01 from port 0
                                                      // and returns them in the low/high
                                                      // 16-bits of "aword" respectively.
        mtdHwSetRegFieldToWord(aword,2,0,4,&aword); // Change first nibble to 2
        mtdHwSetRegFieldToWord(aword,3,4,4,&aword); // Change second nibble to 3
        
*******************************************************************************/
MTD_STATUS mtdHwMsecSetRegFieldToLongWord
(
    IN  MTD_U32      regData,
    IN  MTD_U32      bitFieldData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U32      *data
);



#if C_LINKAGE
#if defined __cplusplus 
}
#endif 
#endif

#endif /* defined MTD_HWMSECCNTL_H */



