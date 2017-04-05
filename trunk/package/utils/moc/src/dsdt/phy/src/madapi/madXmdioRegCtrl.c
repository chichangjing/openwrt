
#include <madCopyright.h>

/********************************************************************************
* madXmdioRegCtrl.c
* 
* DESCRIPTION:
*       APIs to PCS control in XMDIO registers.
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

/* madXmdioRegCtrl.c */

/*******************************************************************************
* madXmdioSetClkStoppable
*
* DESCRIPTION:
*       This routine sets PCS clock stoppable through Xmdio register.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from madLoadDriver
*       port - The logical port number
*       en - 1: clock stoppable during LPI,
*            0: no stoppable.
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*       MAD_BAD_PARAM   - on bad parameters
*
* COMMENTS:
*       Reg 3_0, Bit 10 : Clock stoppable.
*
*******************************************************************************/
MAD_STATUS madXmdioSetClkStoppable
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    IN  MAD_BOOL        en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO(("madXmdioSetClkStoppable called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
	{
      MAD_DBG_ERROR(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
	}
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_XMDIO_CAPABLE) )
    {
        MAD_DBG_ERROR(("XMDIO not supported.\n"));
        return MAD_PHY_XMDIO_NOT_SUPPORT;
    }

    /* Enable or disable */
    MAD_BOOL_2_BIT(en,data);

    /* Set Copper Control Register 1 */
    if((retVal = madHw45SetPhyRegField(dev,hwPort,
		            MAD_REG_PCS_CTRL_1, 10,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_REG_FIELD);
    }

    return MAD_OK;
}


/*******************************************************************************
* madXmdioGetClkStoppable
*
* DESCRIPTION:
*       This routine gets PCS clock stoppable through Xmdio register.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from madLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       en - 1: clock stoppable during LPI,
*            0: no stoppable.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*       MAD_BAD_PARAM   - on bad parameters
*
* COMMENTS:
*       Reg 3_0, Bit 10 : Clock stoppable.
*
*******************************************************************************/
MAD_STATUS madXmdioGetClkStoppable
(
    IN  MAD_DEV         *dev,
    IN  MAD_LPORT       port,
    OUT  MAD_BOOL       *en
)
{
    MAD_U16     data;
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO(("madXmdioGetClkStoppable called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
	{
      MAD_DBG_ERROR(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
	}

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_XMDIO_CAPABLE) )
    {
        MAD_DBG_ERROR(("XMDIO not supported.\n"));
        return MAD_PHY_XMDIO_NOT_SUPPORT;
    }

    /* Get the required bits in Copper Control Register 1 */
    if((retVal = madHw45GetPhyRegField(dev,hwPort,
		            MAD_REG_PCS_CTRL_1, 10,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR(("Reading to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_GET_REG_FIELD);
    }

    /* Enabled or disabled */
    MAD_BIT_2_BOOL(data,*en);

    return MAD_OK;
}

