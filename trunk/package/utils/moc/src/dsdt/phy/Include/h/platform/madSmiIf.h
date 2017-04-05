/* #include <madCopyright.h> */

/********************************************************************************
* madMiiSmiIf.h
*
* DESCRIPTION:
*       Includes functions prototypes for initializing and accessing the
*       MII / SMI interface.
*       This is the only file to be included from upper layers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __madMiiSmiIfh
#define __madMiiSmiIfh


#include <madApi.h>

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* madSmiIfReadRegister
*
* DESCRIPTION:
*       This function reads a register throw the SMI / MII interface, to be used
*       by upper layers.
*
* INPUTS:
*       phyAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*
* OUTPUTS:
*       data        - The register's data.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madSmiIfReadRegister
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      phyAddr,
    IN  MAD_U8      regAddr,
    OUT MAD_U16     *data
);


/*******************************************************************************
* madSmiIfWriteRegister
*
* DESCRIPTION:
*       This function writes to a register throw the SMI / MII interface, to be
*       used by upper layers.
*
* INPUTS:
*       phyAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       data        - The data to be written to the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madSmiIfWriteRegister
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      phyAddr,
    IN  MAD_U8      regAddr,
    IN  MAD_U16     data
);

/*******************************************************************************
* madXSmiIfReadRegister
*
* DESCRIPTION:
*       This function reads a register throw the XSMI / XMII interface, to be used
*       by upper layers.
*
* INPUTS:
*       phyAddr     - The PHY address to be read.
*       devAddr     - The device address to read.
*       regAddr     - The register address to read.
*
* OUTPUTS:
*       data        - The register's data.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madXSmiIfReadRegister
(
    IN  MAD_DEV     *dev,
    IN  MAD_U16      phyAddr,
    IN  MAD_U16     devAddr,
    IN  MAD_U16     regAddr,
    OUT MAD_U16     *data
);

/*******************************************************************************
* madXSmiIfWriteRegister
*
* DESCRIPTION:
*       This function writes to a register throw the XSMI / XMII interface, to be
*       used by upper layers.
*
* INPUTS:
*       phyAddr     - The PHY address to be written.
*       devAddr     - The device address to written.
*       regAddr     - The register address to written.
*       data        - The data to be written to the register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MAD_STATUS madXSmiIfWriteRegister
(
    IN  MAD_DEV     *dev,
    IN  MAD_U16     phyAddr,
    IN  MAD_U16     devAddr,
    IN  MAD_U16     regAddr,
    IN  MAD_U16     data
);

#ifdef __cplusplus
}
#endif

#endif /* __madMiiSmiIfh */
