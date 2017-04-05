/********************************************************************
This file contains function prototypes and global defines for
interfacing with the host's hardware-specific MDIO and general-purpose
IO in order to control and query the Solarflare SFX7101 and SFT9001 
ethernet phys.

These functions as written were tested with a USB-interface to
access Solarflare's EVK. These functions must be ported to 
the host's specific platform.

Copyright (c) 2010, Solarflare Communications
See copyright and confidentiality notice at the bottom of this file 
for details
********************************************************************/
#ifndef SFPHY_PORT_H
#define SFPHY_PORT_H

#define SF_EVK_TEST_ENVIRONMENT 0 /* set to 0 for host environment */
                                  /* set to 1 to build SF sample code */
                                  /* to interface to Solarflare EVK via USB */                                     

#if SF_EVK_TEST_ENVIRONMENT
#include "stdafx.h"
#include <windows.h> 
#include "UsbInterface.h"
#endif

#if SF_EVK_TEST_ENVIRONMENT == 0
#if defined __cplusplus 
    extern "C" { 
#endif 
#endif

/* This type is provided as a type for the host to pass a context parameter
   into/through the API for whatever purposes the host may need */
typedef void *CTX_PTR_TYPE;   

/* Parallel Download - RAM & FLASH */
#define MAX_PORTS_TO_DOWNLOAD        4    /* user input */
/* unsigned long portsToDownload[MAX_PORTS_TO_DOWNLOAD] = {0, 1, 3, 5}; */
/* unsigned short errorInPorts[MAX_PORTS_TO_DOWNLOAD] = {0,0,0,0}; */


/*******************************************************************
   PORT THESE FUNCTIONS
 *******************************************************************/

/******************************************************************************
void SFPhyMdioWrite(CTX_PTR_TYPE contextPtr, unsigned long port, 
                    unsigned long dev, 
                    unsigned long reg, unsigned short value)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31 
    dev - MMD device address
    reg - MMD register address
    value - data to write

 Outputs:
    None

 Returns:
    None

 Description:
    Writes a 16-bit word to the MDIO
    Address is in format X.Y.Z, where X selects the MDIO port (0-31), Y selects
    the MMD/Device (0-31), and Z selects the register.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyMdioWrite(CTX_PTR_TYPE contextPtr, unsigned long port, 
                    unsigned long dev, 
                    unsigned long reg, unsigned short value);

/******************************************************************************
unsigned short SFPhyMdioRead(CTX_PTR_TYPE contextPtr, unsigned long port, 
                             unsigned long dev, unsigned long reg)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31 
    dev - MMD device address
    reg - MMD register address

 Outputs:
    None

 Returns:
    Returns 16 bit word from the MDIO

 Description:
    Reads a 16-bit word from the MDIO
    Address is in format X.Y.Z, where X selects the MDIO port (0-31), Y selects the 
    MMD/Device (0-31), and Z selects the register.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
unsigned short SFPhyMdioRead(CTX_PTR_TYPE contextPtr, unsigned long port, 
                             unsigned long dev, unsigned long reg);

/******************************************************************************
void SFPhyPutPhyInDownloadMode(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets FLASH_CFG[1] pin to 1, boot code will wait for download when this pin is 
    high following a reset

 Side effects:
    None

 Notes/Warnings:
    None
******************************************************************************/
void SFPhyPutPhyInDownloadMode(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
void SFPhyPutPhyInParallelWriteMode(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets PHYPRT_OW pin to 1 (on) to put EVK in parallel write mode. 

 Side effects:
    Setting this pin will causes the PHY to ignore the MDIO port address on 
    writes.


 Notes/Warnings:
    1. Reset is not needed after setting this pin to 1.
    2. The read will not work with this pin set. Therefore to read, this pin has 
    to be set to '0' and read has to be done one port at a time. The function 
    SFPhyRemovePhyInParallelWriteMode() can be used to set PHYPRT_W pin to 0.
    3. User to make sure that only ports that need to be activated should be
    connected to this pin. 
******************************************************************************/
void SFPhyPutPhyInParallelWriteMode(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
void SFPhyRemovePhyInParallelWriteMode(CTX_PTR_TYPE contextPtr,
                                 unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets PHYPRT_OW pin to 0 (off) to take EVK out of parallel write mode. 

 Side effects:
    None.
    
 Notes/Warnings:
    1. Reset is not needed after setting this pin to 0.
    2. For normal MDIO operation the PHYPRT_OW pin must be 0.
******************************************************************************/
void SFPhyRemovePhyInParallelWriteMode(CTX_PTR_TYPE contextPtr,
                                       unsigned long port);

/******************************************************************************
void SFPhyRemovePhyDownloadMode(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Sets FLASH_CFG[1] pin to 0, boot code will load and run code from flash following 
    a reset if this pin is low

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyRemovePhyDownloadMode(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
void SFPhyPutInHwLowPowerMode(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Set PWD_INn pin low.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyPutInHwLowPowerMode(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
void SFPhyRemoveHwLowPowerMode(CTX_PTR_TYPE contextPtr, unsigned long port)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31

 Outputs:
    None

 Returns:
    None

 Description:
    Set PWD_INn pin high then does a special software reset to restore normal
    operation.

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyRemoveHwLowPowerMode(CTX_PTR_TYPE contextPtr, unsigned long port);

/******************************************************************************
void SFPhyWait(CTX_PTR_TYPE contextPtr, unsigned x)

 Inputs:
    contextPtr - pointer to host's private data
    x - number of milliseconds to wait

 Outputs:
    None

 Returns:
    None

 Description:
    Waits X milliseconds

 Side effects:
    None

 Notes/Warnings:
    None

******************************************************************************/
void SFPhyWait(CTX_PTR_TYPE contextPtr, unsigned x);

#if SF_EVK_TEST_ENVIRONMENT
/******************************************************************************
int SFPhyOpenReadFile(CTX_PTR_TYPE contextPtr, unsigned long port,
                    FILE **pfp, char *fname, unsigned char *data,
                         unsigned max_data_size)

 Inputs:
    contextPtr - pointer to host's private data
    port - MDIO port address, 0-31
    **pfp - Pointer to the code that need to be downloaded. 
            (eg: binary image of the *.hdr file)
    *fname - Header file name
    *data - hdr file image
    max_data_size - Size of the data to be downloaded

 Outputs:
    None

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
    See the "Example" section of the function declarations of SFPhyDownLoadFlash()
    and SFPhyMdioRamDownload() to understand how SFPhyOpenReadFile() can be used.
******************************************************************************/
int SFPhyOpenReadFile(CTX_PTR_TYPE contextPtr, unsigned long port,
                    FILE **pfp, char *fname, unsigned char *data,
                         unsigned max_data_size);

#endif
/********************************************************************

Copyright (c) 2010, Solarflare Communications

The information is proprietary and confidential to Solarflare Communications, Inc.,
and for its customers’ internal use.

The terms of use for this software are as set forth in the Solarflare API Software 
License Agreement.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR OR HIS EMPLOYER BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/
#if SF_EVK_TEST_ENVIRONMENT == 0
#if defined __cplusplus 
}
#endif 
#endif

#endif /* defined SFPHY_PORT_H */



