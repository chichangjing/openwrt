#include <madCopyright.h>

/********************************************************************************
* madXmdioRegStatus.c
*
* DESCRIPTION:
*       APIs to get PCS status in XMDIO registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>

/* madXmdioRegStatus.c */

/*******************************************************************************
* madPCSStatus
*
* DESCRIPTION:
*       This routine get PCS status through XMDIO.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from madLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       status - MAD_PCS_STATUS.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*       Rag: 3_1, Bit 0-15 : PCS status
*
*******************************************************************************/
MAD_STATUS madPCSStatus
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_PCS_STATUS *status
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO(("madPCSStatus called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }
  /* check if device supports this feature */
  if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_XMDIO_CAPABLE) )
  {
    MAD_DBG_ERROR(("XMDIO not supported.\n"));
    return MAD_PHY_XMDIO_NOT_SUPPORT;
  }

  /* Get PCS Status Device 3 Register 1 */
  if((retVal = madHw45ReadPhyReg(dev,hwPort,MAD_REG_PCS_STATUS_1,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR(("Reading from phy reg failed.\n"));
    return (retVal | MAD_API_FAIL_READ_REG);
  }

    if (data & MAD_BIT_11)
      status->txLpIdleReceived = 1;
	else
      status->txLpIdleReceived = 0;

    if (data & MAD_BIT_10)
      status->rxLpIdleReceived = 1;
	else
      status->rxLpIdleReceived = 0;

    if (data & MAD_BIT_9)
      status->txLpIdleIndication = 1;
	else
      status->txLpIdleIndication = 0;

    if (data & MAD_BIT_8)
      status->rxLpIdleIndication = 1;
	else
      status->rxLpIdleIndication = 0;

    if (data & MAD_BIT_2)
      status->receivedLnkSt = 1;
	else
      status->receivedLnkSt = 0;

    if (data & MAD_BIT_1)
      status->lowPowerAbility = 1;
	else
      status->lowPowerAbility = 0;

    return MAD_OK;
}

/*******************************************************************************
* madPCSEEECapability
*
* DESCRIPTION:
*       This routine get PCS EEE Capability through XMDIO.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from madLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       capability - MAD_PCS_EEE_CAPABILITY.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*       Rag: 3_14, Bit 0-15 : PCS status
*
*******************************************************************************/
MAD_STATUS madPCSEEECapability
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_PCS_EEE_CAPABILITY *capability
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO(("madPCSEEECapability called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n",  (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }
  /* check if device supports this feature */
  if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_XMDIO_CAPABLE) )
  {
    MAD_DBG_ERROR(("XMDIO not supported.\n"));
    return MAD_PHY_XMDIO_NOT_SUPPORT;
  }

  /* Get PCS Status Device 3 Register 0x14 */
  if((retVal = madHw45ReadPhyReg(dev,hwPort,MAD_REG_PCS_EEE_CAPABILITY,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR(("Reading from phy reg failed.\n"));
    return (retVal | MAD_API_FAIL_READ_REG);
  }

    if (data & MAD_BIT_6)
      capability->EEE10GBaseKR = 1;
	else
      capability->EEE10GBaseKR = 0;

    if (data & MAD_BIT_5)
      capability->EEE10GBaseKX4 = 1;
	else
      capability->EEE10GBaseKX4 = 0;

    if (data & MAD_BIT_4)
      capability->EEE1000BaseKX = 1;
	else
      capability->EEE1000BaseKX = 0;

    if (data & MAD_BIT_3)
      capability->EEE10GBaseT = 1;
	else
      capability->EEE10GBaseT = 0;

    if (data & MAD_BIT_2)
      capability->EEE1000BaseT = 1;
	else
      capability->EEE1000BaseT = 0;

    if (data & MAD_BIT_1)
      capability->EEE100BaseTX = 1;
	else
      capability->EEE100BaseTX = 0;

    return MAD_OK;
}

/*******************************************************************************
* madGetPCSWakeErrCounter
*
* DESCRIPTION:
*       This routine get PCS Wake error counter through XMDIO.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from madLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       counter - Wake error counter.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*       Rag: 3_16, Bit 0-15 : PCS swake error counter
*
*******************************************************************************/
MAD_STATUS madGetPCSWakeErrCounter
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_U16     *wcount
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO(("madGetPCSWakeErrCounter called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n",  (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }
  /* check if device supports this feature */
  if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_XMDIO_CAPABLE) )
  {
    MAD_DBG_ERROR(("XMDIO not supported.\n"));
    return MAD_PHY_XMDIO_NOT_SUPPORT;
  }

  /* Get PCS Status Device 3 Register 0x16 */
  if((retVal = madHw45ReadPhyReg(dev,hwPort,MAD_REG_PCS_EEE_WAKE_ERR_COUNTER, wcount))
      != MAD_OK)
  {
    MAD_DBG_ERROR(("Reading from phy reg failed.\n"));
    return (retVal | MAD_API_FAIL_READ_REG);
  }


    return MAD_OK;
}

/*******************************************************************************
* madPCSEEEAdvertisement
*
* DESCRIPTION:
*       This routine get PCS EEE Advertisement through XMDIO.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from madLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       capability - MAD_PCS_EEE_CAPABILITY.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*       Rag: 3_14, Bit 0-15 : PCS status
*
*******************************************************************************/
MAD_STATUS madPCSEEEAdvertisement
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_PCS_EEE_CAPABILITY *capability
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO(("madPCSEEEAdvertisement called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n",  (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }
  /* check if device supports this feature */
  if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_XMDIO_CAPABLE) )
  {
    MAD_DBG_ERROR(("XMDIO not supported.\n"));
    return MAD_PHY_XMDIO_NOT_SUPPORT;
  }

  /* Get PCS Status Device 3 Register 0x3c */
  if((retVal = madHw45ReadPhyReg(dev,hwPort,MAD_REG_EEE_ADVERTISEMENT,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR(("Reading from phy reg failed.\n"));
    return (retVal | MAD_API_FAIL_READ_REG);
  }

    if (data & MAD_BIT_6)
      capability->EEE10GBaseKR = 1;
	else
      capability->EEE10GBaseKR = 0;

    if (data & MAD_BIT_5)
      capability->EEE10GBaseKX4 = 1;
	else
      capability->EEE10GBaseKX4 = 0;

    if (data & MAD_BIT_4)
      capability->EEE1000BaseKX = 1;
	else
      capability->EEE1000BaseKX = 0;

    if (data & MAD_BIT_3)
      capability->EEE10GBaseT = 1;
	else
      capability->EEE10GBaseT = 0;

    if (data & MAD_BIT_2)
      capability->EEE1000BaseT = 1;
	else
      capability->EEE1000BaseT = 0;

    if (data & MAD_BIT_1)
      capability->EEE100BaseTX = 1;
	else
      capability->EEE100BaseTX = 0;

    return MAD_OK;
}

/*******************************************************************************
* madPCSEEELnkPartnerAdvertisement
*
* DESCRIPTION:
*       This routine get PCS EEE Link Partener Advertisement through XMDIO.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from madLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       capability - MAD_PCS_EEE_CAPABILITY.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*       Rag: 3_14, Bit 0-15 : PCS status
*
*******************************************************************************/
MAD_STATUS madPCSEEELnkPartnerAdvertisement
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_PCS_EEE_CAPABILITY *capability
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO(("madPCSEEELnkPartnerAdvertisement called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n",  (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }
  /* check if device supports this feature */
  if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_XMDIO_CAPABLE) )
  {
    MAD_DBG_ERROR(("XMDIO not supported.\n"));
    return MAD_PHY_XMDIO_NOT_SUPPORT;
  }

  /* Get PCS Status Device 3 Register 0x3d */
  if((retVal = madHw45ReadPhyReg(dev,hwPort,MAD_REG_EEE_LNK_PARTNER_ADVERTISEMENT,&data))
      != MAD_OK)
  {
    MAD_DBG_ERROR(("Reading from phy reg failed.\n"));
    return (retVal | MAD_API_FAIL_READ_REG);
  }

    if (data & MAD_BIT_6)
      capability->EEE10GBaseKR = 1;
	else
      capability->EEE10GBaseKR = 0;

    if (data & MAD_BIT_5)
      capability->EEE10GBaseKX4 = 1;
	else
      capability->EEE10GBaseKX4 = 0;

    if (data & MAD_BIT_4)
      capability->EEE1000BaseKX = 1;
	else
      capability->EEE1000BaseKX = 0;

    if (data & MAD_BIT_3)
      capability->EEE10GBaseT = 1;
	else
      capability->EEE10GBaseT = 0;

    if (data & MAD_BIT_2)
      capability->EEE1000BaseT = 1;
	else
      capability->EEE1000BaseT = 0;

    if (data & MAD_BIT_1)
      capability->EEE100BaseTX = 1;
	else
      capability->EEE100BaseTX = 0;

    return MAD_OK;
}

