#include <madCopyright.h>
/********************************************************************************
* madMiiSmiIf.c
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

#include <madApiTypes.h>
#include <madApiDefs.h>
#include <madDrvPhyRegs.h>
#include <madHwCntl.h>
#include <madSmiIf.h>
#include <madPlatformDeps.h>
#include <madSem.h>


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
)
{
    MAD_U32 _phyAddr, _regAddr, _data;
    _phyAddr = phyAddr;
    _regAddr = regAddr;
    if(dev->fmadReadMii(dev,_phyAddr, _regAddr, (unsigned int *)&_data) != MAD_TRUE)
    {
        return MAD_FAIL;
    }
    *data = (MAD_U16)_data;
    return MAD_OK;
}


/*******************************************************************************
* madSmiIfWriteRegister
*
* DESCRIPTION:
*       This function writes to a register throw the SMI / MII interface, to be
*       used by upper layers.
*
* INPUTS:
*       phyAddr     - The PHY address to be written.
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
MAD_STATUS madSmiIfWriteRegister
(
    IN  MAD_DEV     *dev,
    IN  MAD_U8      phyAddr,
    IN  MAD_U8      regAddr,
    IN  MAD_U16     data
)
{
    MAD_U32 _phyAddr, _regAddr, _data;
    _phyAddr = phyAddr;
    _regAddr = regAddr;
    _data = data;
    if(dev->fmadWriteMii(dev, _phyAddr, _regAddr, _data) != MAD_TRUE)
    {
        return MAD_DRV_FAIL_WRITE_REG;
    }
    return MAD_OK;
}


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
)
{
    MAD_U32 _phyAddr, _devAddr,  _regAddr, _data;
    _phyAddr = phyAddr;
    _devAddr = devAddr;
    _regAddr = regAddr;

    if(dev->fmadReadXMii(dev,_phyAddr, _devAddr, _regAddr, (unsigned int *)&_data) != MAD_TRUE)
    {
        return MAD_FAIL;
    }
    *data = (MAD_U16)_data;
    return MAD_OK;
}


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
)
{
    MAD_U32 _phyAddr, _devAddr,  _regAddr, _data;
    _phyAddr = phyAddr;
    _devAddr = devAddr;
    _regAddr = regAddr;
    _data = data;
    if(dev->fmadWriteXMii(dev, _phyAddr, _devAddr, _regAddr, _data) != MAD_TRUE)
    {
        return MAD_DRV_FAIL_WRITE_REG;
    }
    return MAD_OK;
}

