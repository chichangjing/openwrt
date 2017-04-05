#include <madCopyright.h>

/*******************************************************************************
* madPortPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madSem.h>
#include <madHwCntl.h>
#include <madDrvPhyRegs.h>
#include <madDrvConfig.h>

#define PTP_READPLUS_SUPPORT(page,reg)       (page==MAD_PAGE15)||(page==MAD_PAGE12&&\
											reg>=0xE)

static void madWait (int waitTime)
{
	volatile int count=waitTime*100000;
    while (count--);
}


extern MAD_STATUS msec_en_PTP 
(
    IN  MAD_DEV    *dev,
    IN  int        port,
    IN  int        en
);

/****************************************************************************/
/* PTP operation function declaration.                                    */
/****************************************************************************/
static MAD_STATUS ptpOperationPerform
(
    IN   MAD_DEV             *dev,
    IN   MAD_PTP_OPERATION    ptpOp,
    INOUT MAD_PTP_OP_DATA     *opData
);

/*******************************************************************************
* mdPtpSetEnable_port
*
* DESCRIPTION:
*       This routine set port PTP Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_BOOL    en
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetEnable_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
    if (en==MAD_TRUE)
    {
        data = 0;
    }
    else
    {
        data = 1;
    }
    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,9,1,data))!= MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    if (dev->deviceId!=MAD_88E1340)
    {
      if (en==MAD_TRUE)
      {
         data = 1;
      }
      else
      {
        data = 0;
      }
      if((retVal = msec_en_PTP(dev, port/4, data))!= MAD_OK)
      {
         MAD_DBG_ERROR(("Set macsec bypass PTP failed.\n"));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }
    }

    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetEnable_port
*
* DESCRIPTION:
*       This routine Get port PTP Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT  MAD_BOOL    *en
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetEnable_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,9,1,&data))!=MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    if (data)
    {
        *en=MAD_FALSE;
    }
    else
    {
        *en=MAD_TRUE;
    }
    return MAD_OK;
}



/*******************************************************************************
* mdPtpSetRefClkSrc_port
*
* DESCRIPTION:
*       This routine sets port PTP reference clock source.
*
* INPUTS:
*       port - The logical port number
*       rClk   - 1: use 125MHz clock supplied from config pin. 
*				 0: use internal clock
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetRefClkSrc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_BOOL    rClk
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetRefClkSrc_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if (rClk==MAD_TRUE)
    {
        data = 1;
    }
    else
    {
        data = 0;
    }
    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,8,1,data))!= MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetRefClkSrc_port
*
* DESCRIPTION:
*       This routine gets port PTP reference clock source.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		rClk   - 1: use 125MHz clock supplied from config pin. 
*			   - 0: use internal clock
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetRefClkSrc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_BOOL    *rClk
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetRefClkSrc_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,8,1,&data))!=MAD_OK)
    {
        MAD_DBG_ERROR(("Reading paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    if (data)
    {
        *rClk=MAD_TRUE;
    }
    else
    {
        *rClk=MAD_FALSE;
    }
    return MAD_OK;
}


/*******************************************************************************
* mdPtpSetInputSrc_port
*
* DESCRIPTION:
*       This routine sets port PTP input source.
*
* INPUTS:
*       port - The logical port number
*       inSrc  - 1: use LED1 for PTP event request input. 
*				 0: force input to 0.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetInputSrc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_BOOL    inSrc
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetInputSrc_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if (inSrc==MAD_TRUE)
    {
        data = 1;
    }
    else
    {
        data = 0;
    }
    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,7,1,data))!= MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetInputSrc_port
*
* DESCRIPTION:
*       This routine gets port PTP input source.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       inSrc  - 1: use LED1 for PTP event request input. 
*				 0: force input to 0.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetInputSrc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_BOOL    *inSrc
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetInputSrc_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,7,1,&data))!=MAD_OK)
    {
        MAD_DBG_ERROR(("Reading paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    if (data)
    {
        *inSrc=MAD_TRUE;
    }
    else
    {
        *inSrc=MAD_FALSE;
    }
    return MAD_OK;
}


/*******************************************************************************
* mdPtpSetOutputSrc_port
*
* DESCRIPTION:
*       This routine sets port PTP output source.
*
* INPUTS:
*       port - The logical port number
*       outSrc  - 1: use LED1 for PTP trigger generate output. 
*				  0: force output to 0.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetOutputSrc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_BOOL    outSrc
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetOutputSrc_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if (outSrc==MAD_TRUE)
    {
        data = 1;
    }
    else
    {
        data = 0;
    }
    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,6,1,data))!= MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetOutputSrc_port
*
* DESCRIPTION:
*       This routine gets port PTP output source.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       outSrc  - 1: use LED1 for PTP trigger generate output. 
*				  0: force output to 0.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetOutputSrc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_BOOL    *outSrc
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetOutputSrc_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    page = MAD_PAGE6;
    reg = MAD_REG_GENERAL_CONTROL;
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,6,1,&data))!=MAD_OK)
    {
        MAD_DBG_ERROR(("Reading paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    if (data)
    {
        *outSrc=MAD_TRUE;
    }
    else
    {
        *outSrc=MAD_FALSE;
    }
    return MAD_OK;
}


/*******************************************************************************
* mdPtpSetIntEnable_port
*
* DESCRIPTION:
*       This routine set port PTP interrupt Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*       en    - 1: Power on PTP. 0: Power off.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetIntEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_BOOL    en
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetIntEnable_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
    if (en==MAD_TRUE)
    {
        data = 3;
    }
    else
    {
        data = 0;
    }
    page = MAD_PAGE8;
    reg = MAD_REG_PTP_PORT_CONFIG_2;
    if((retVal = madHwSetPagedPhyRegField(dev,hwPort,page,reg,0,2,data))!= MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetIntEnable_port
*
* DESCRIPTION:
*       This routine get port PTP Interrupt Enable/Disable.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       en    - 1: Power on PTP. 0: Power off.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetIntEnable_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT  MAD_BOOL    *en
)
{
    MAD_STATUS	retVal;
    MAD_U16     data;
    MAD_U16     page;
    MAD_U8      reg;
    MAD_U8      hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetIntEnable_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
    page = MAD_PAGE8;
    reg = MAD_REG_PTP_PORT_CONFIG_2;
    if((retVal = madHwGetPagedPhyRegField(dev,hwPort,page,reg,0,2,&data))!=MAD_OK)
    {
        MAD_DBG_ERROR(("Reading paged phy reg (%i,%i) failed.\n",page,reg));
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
    }

    if (data)
    {
        *en=MAD_TRUE;
    }
    else
    {
        *en=MAD_FALSE;
    }
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetConfig_port
*
* DESCRIPTION:
*       This routine writes port PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*        ptpData  - PTP configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetConfig_port
(
  IN  MAD_DEV			*dev,
  IN  MAD_LPORT			port,
  IN  MAD_PTP_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_PTP_PORT_CONFIG    ptpPortData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetConfig_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_WRITE_DATA;

    /* setting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    opData.ptpData = ptpData->ptpEType;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    /* setting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = ptpData->msgIdTSEn;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    /* setting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = ptpData->tsArrPtr;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TSArrPtr.\n"));
        return MAD_FAIL;
    }

    {
        ptpPortData.transSpec = ptpData->transSpec;
        ptpPortData.disTSpec = 1;    /* default value */
        ptpPortData.disTSOverwrite = ptpData->disTSOverwrite;
        ptpPortData.ipJump = 2;        /* default value */
        ptpPortData.etJump = 12;    /* default value */

        /* per port configuration */
/*        for(port=0; port<dev->numOfPorts; port++)   */
        {
            ptpPortData.ptpArrIntEn = (ptpData->ptpArrIntEn & (1 << port))? MAD_TRUE : MAD_FALSE;
            ptpPortData.ptpDepIntEn = (ptpData->ptpDepIntEn & (1 << port))? MAD_TRUE : MAD_FALSE;

			ptpPortData.filterAct = ptpData->ptpPortConfig[port].filterAct;
			ptpPortData.hwAccPtp = ptpData->ptpPortConfig[port].hwAccPtp;
			ptpPortData.kpFramwSa = ptpData->ptpPortConfig[port].kpFramwSa;
			ptpPortData.fullCheck = ptpData->ptpPortConfig[port].fullCheck;
			ptpPortData.noCorrupt = ptpData->ptpPortConfig[port].noCorrupt;
			ptpPortData.extHwAcc = ptpData->ptpPortConfig[port].extHwAcc;
			ptpPortData.arrLedCtrl = ptpData->ptpPortConfig[port].arrLedCtrl;
			ptpPortData.depLedCtrl = ptpData->ptpPortConfig[port].depLedCtrl;
 
            if((retVal = mdPtpSetPortConfig(dev, port, &ptpPortData)) != MAD_OK)
            {
                MAD_DBG_INFO(("Failed mdPtpSetPortConfig.\n"));
                return MAD_FAIL;
            }

			if (dev->deviceId == MAD_88E151x)
			{
				if(!((ptpData->ptpPortConfig[port].arrTSMode==MAD_PTP_TS_MODE_IN_REG)
				||(ptpData->ptpPortConfig[port].arrTSMode==MAD_PTP_TS_MODE_IN_RESERVED_2)
				||(ptpData->ptpPortConfig[port].arrTSMode==MAD_PTP_TS_MODE_IN_FRAME_END)))
					ptpData->ptpPortConfig[port].arrTSMode=MAD_PTP_TS_MODE_IN_REG;
				if((retVal = mdPtpSetTsMode_port(dev, port, ptpData->ptpPortConfig[port].arrTSMode)) != MAD_OK)
				{
					MAD_DBG_INFO(("Failed mdSetPortConfig.\n"));
					return MAD_FAIL;
				}
			}
        }
    }
    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGetConfig_port
*
* DESCRIPTION:
*       This routine reads port PTP configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpData  - PTP configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_PTP_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_PTP_PORT_CONFIG    ptpPortData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetConfig_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    ptpData->ptpEType = opData.ptpData;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    ptpData->msgIdTSEn = opData.ptpData;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TSArrPtr.\n"));
        return MAD_FAIL;
    }

    ptpData->tsArrPtr = opData.ptpData;

    {
        ptpData->ptpArrIntEn = 0;
        ptpData->ptpDepIntEn = 0;

        /* per port configuration */
/*        for(port=0; port<dev->numOfPorts; port++)  */
        {
            if((retVal = mdPtpGetPortConfig(dev, port, &ptpPortData)) != MAD_OK)
            {
                MAD_DBG_INFO(("Failed mdPtpGetPortConfig.\n"));
                return MAD_FAIL;
            }

            ptpData->ptpArrIntEn |= (ptpPortData.ptpArrIntEn ? (1 << port) : 0);
            ptpData->ptpDepIntEn |= (ptpPortData.ptpDepIntEn ? (1 << port) : 0);

			ptpData->ptpPortConfig[port].filterAct = ptpPortData.filterAct;
			ptpData->ptpPortConfig[port].hwAccPtp = ptpPortData.hwAccPtp;
			ptpData->ptpPortConfig[port].kpFramwSa = ptpPortData.kpFramwSa;
			ptpData->ptpPortConfig[port].fullCheck = ptpPortData.fullCheck;
			ptpData->ptpPortConfig[port].noCorrupt = ptpPortData.noCorrupt;
			ptpData->ptpPortConfig[port].extHwAcc = ptpPortData.extHwAcc;
			ptpData->ptpPortConfig[port].arrLedCtrl = ptpPortData.arrLedCtrl;
			ptpData->ptpPortConfig[port].depLedCtrl = ptpPortData.depLedCtrl;

			if (dev->deviceId == MAD_88E151x)
			{
				if((retVal = mdPtpGetTsMode_port(dev, port, &ptpData->ptpPortConfig[port].arrTSMode)) != MAD_OK)
				{
					MAD_DBG_INFO(("Failed mdGetPortConfig.\n"));
					return MAD_FAIL;
				}
			}
        }

        ptpData->transSpec = ptpPortData.transSpec;
        ptpData->disTSOverwrite = ptpPortData.disTSOverwrite;
        ptpData->msgIdStartBit = 4;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpSetGlobalConfig_port
*
* DESCRIPTION:
*       This routine writes port PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*        ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetGlobalConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetGlobalConfig_port Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_WRITE_DATA;

    /* setting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    opData.ptpData = ptpData->ptpEType;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing PTPEType.\n"));
        return MAD_FAIL;
    }

    /* setting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = ptpData->msgIdTSEn;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    /* setting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = ptpData->tsArrPtr;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TSArrPtr.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGetGlobalConfig_port
*
* DESCRIPTION:
*       This routine reads port PTP global configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetGlobalConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */

    MAD_DBG_INFO(("mdPtpGetGlobalConfig_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    ptpData->ptpEType = opData.ptpData;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    ptpData->msgIdTSEn = opData.ptpData;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TSArrPtr.\n"));
        return MAD_FAIL;
    }

    ptpData->tsArrPtr = opData.ptpData;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpSetPortConfig
*
* DESCRIPTION:
*       This routine writes PTP port configuration parameters.
*
* INPUTS:
*        ptpData  - PTP port configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPortConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_PORT_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if (ptpData->transSpec > 0xF)    /* 4 bits */
        return MAD_API_PTP_BAD_PARAM;

    if (ptpData->etJump > 0x1F)    /* 5 bits */
        return MAD_API_PTP_BAD_PARAM;

    if (ptpData->ipJump > 0x3F)    /* 6 bits */
        return MAD_API_PTP_BAD_PARAM;

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = hwPort;

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    op = MAD_PTP_WRITE_DATA;
    opData.ptpData = (ptpData->transSpec << 12) | (opData.ptpData & 0x1) |
                    ((ptpData->disTSpec?1:0) << 11) |
                    ((ptpData->disTSOverwrite?1:0) << 1);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
        return MAD_FAIL;
    }

    /* setting etJump and ipJump, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = (ptpData->ipJump << 8) | ptpData->etJump;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    /* setting Int, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = (ptpData->ptpArrIntEn?1:0) |
                    ((ptpData->ptpDepIntEn?1:0) << 1);

	if (dev->deviceId == MAD_88E151x)
	{
		
		opData.ptpData |= ((ptpData->arrTSMode&0xff) << 8);
		opData.ptpData |= ((ptpData->filterAct?1:0) << 7); /* Filter 802.1 Act from LEDs */
		opData.ptpData |= ((ptpData->hwAccPtp?1:0) << 6);  /* Hardware Accelerated PTP */
        opData.ptpData |= ((ptpData->kpFramwSa?1:0) << 5); /* KS = Keep Frame’s SA */
		/* opData.ptpData |= ((ptpData->fullCheck?1:0) << 4); */ /* Full check  */
        opData.ptpData |= ((ptpData->noCorrupt?1:0) << 3); /* do not corrupt */
		opData.ptpData |= ((ptpData->extHwAcc?1:0) << 2);  /* External Hardware Acceleration */
	}

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TSArrPtr.\n"));
        return MAD_FAIL;
    }

	/* setting Led control, offset 3 */
	if (dev->deviceId == MAD_88E151x)
	{
	  opData.ptpAddr = 3;
      opData.ptpData = (ptpData->arrLedCtrl<<8) |
                       (ptpData->depLedCtrl);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	  {
        MAD_DBG_INFO(("Failed writing LED control.\n"));
        return MAD_FAIL;
	  }
	}

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGetPortConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters for a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpData  - PTP port configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPortConfig
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT MAD_PTP_PORT_CONFIG    *ptpData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = hwPort;
    op = MAD_PTP_READ_DATA;

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    ptpData->transSpec = opData.ptpData >> 12;
    ptpData->disTSpec = ((opData.ptpData >> 11) & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpData->disTSOverwrite = ((opData.ptpData >> 1) & 0x1) ? MAD_TRUE : MAD_FALSE;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return MAD_FAIL;
    }

    ptpData->ipJump = (opData.ptpData >> 8) & 0x3F;
    ptpData->etJump = opData.ptpData & 0x1F;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TSArrPtr.\n"));
        return MAD_FAIL;
    }

    ptpData->ptpDepIntEn = ((opData.ptpData >> 1) & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpData->ptpArrIntEn = (opData.ptpData & 0x1) ? MAD_TRUE : MAD_FALSE;

	if (dev->deviceId == MAD_88E151x)
	{
		ptpData->arrTSMode = (opData.ptpData &0xff00) >> 8;
		ptpData->filterAct = ((opData.ptpData >> 7 ) & 0x1) ? MAD_TRUE : MAD_FALSE;
		ptpData->hwAccPtp = ((opData.ptpData >> 6) & 0x1) ? MAD_TRUE : MAD_FALSE;
		ptpData->kpFramwSa = ((opData.ptpData >> 5) & 0x1) ? MAD_TRUE : MAD_FALSE;
		/* ptpData->fullCheck = ((opData.ptpData >> 4) & 0x1) ? MAD_TRUE : MAD_FALSE; */
		ptpData->noCorrupt = ((opData.ptpData >> 3) & 0x1) ? MAD_TRUE : MAD_FALSE;
		ptpData->extHwAcc = ((opData.ptpData >> 2) & 0x1) ? MAD_TRUE : MAD_FALSE;
	}

	/* getting Led control, offset 3 */
	if (dev->deviceId == MAD_88E151x)
	{
	  opData.ptpAddr = 3;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	  {
        MAD_DBG_INFO(("Failed reading LED control.\n"));
	  }

      ptpData->arrLedCtrl = madLong2Char((opData.ptpData &0xff00) >> 8);  
      ptpData->depLedCtrl = madLong2Char(opData.ptpData &0xff);
	}

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetPTPEn_port
*
* DESCRIPTION:
*       This routine enables or disables port PTP.
*
* INPUTS:
*       port - The logical port number
*       en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPTPEn_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL        en
)
{
    MAD_STATUS           retVal;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpSetPTPEn_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if((retVal = mdPtpSetPortPTPEn(dev, hwPort, en)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed mdPtpSetPortPTPEn.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGetPTPEn_port
*
* DESCRIPTION:
*       This routine checks if port PTP is enabled.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       en - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPEn_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *en
)
{
    MAD_STATUS           retVal;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("mdPtpGetPTPEn_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if((retVal = mdPtpGetPortPTPEn(dev, hwPort, en)) != MAD_OK)
    {
           MAD_DBG_INFO(("Failed mdPtpGetPortPTPEn.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetPortPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*        en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetPortPTPEn
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_BOOL        en
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8            hwPort;

    MAD_DBG_INFO(("mdPtpSetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpAddr = 0;

    opData.ptpPort = hwPort;

    op = MAD_PTP_READ_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    op = MAD_PTP_WRITE_DATA;

    if (en)
        opData.ptpData &= ~0x1;
    else
        opData.ptpData |= 0x1;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetPortPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        en - MAD_TRUE if enabled, MAD_FALSE otherwise
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPortPTPEn
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT MAD_BOOL        *en
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8            hwPort;

    MAD_DBG_INFO(("mdPtpGetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpAddr = 0;
    opData.ptpPort = hwPort;

    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *en = (opData.ptpData & 0x1) ? MAD_FALSE : MAD_TRUE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetTsMode_port
*
* DESCRIPTION:
*       This routine sets PTP arrive 0 TS mode on a port.
*
* INPUTS:
*       port - The logical port number
*       tsMod - MAD_PTP_TS_MODE_IN_REG
*                MAD_PTP_TS_MODE_IN_RESERVED_2
*                MAD_PTP_TS_MODE_IN_FRAME_END
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetTsMode_port
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TS_MODE  tsMode 
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8             hwPort;	

    MAD_DBG_INFO(("mdPtpSetTsMode_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
    opData.ptpBlock = 0;    /* PTP register space */
    opData.ptpAddr = 2;
	opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpPort = hwPort;

    op = MAD_PTP_READ_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TsMode.\n"));
        return MAD_FAIL;
    }

	op = MAD_PTP_WRITE_DATA;
    opData.ptpData &= 0xff;
    switch (tsMode)
    {
      case MAD_PTP_TS_MODE_IN_REG:
        break;
      case MAD_PTP_TS_MODE_IN_RESERVED_2:
        opData.ptpData |= (PTP_TS_LOC_RESERVED_2<<8); /* set TS in reserved 1 */
        break;
      case MAD_PTP_TS_MODE_IN_FRAME_END:
        opData.ptpData |= (PTP_FRAME_SIZE<<8); /* set TS in end of PTP frame */
        break;
      default:
        MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT the TS mode\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Ts Mode.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetTsMode_port
*
* DESCRIPTION:
*       This routine gets PTP arrive 0 TS mode on a port.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       tsMod - MAD_PTP_TS_MODE_IN_REG
*                MAD_PTP_TS_MODE_IN_RESERVED_2
*                MAD_PTP_TS_MODE_IN_FRAME_END
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTsMode_port
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT MAD_PTP_TS_MODE  *tsMode 
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8             hwPort;
    MAD_U16            tmpData;		

    MAD_DBG_INFO(("mdPtpGetTsMode_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
    opData.ptpBlock = 0;    /* PTP register space */
    opData.ptpAddr = 2;
	opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpPort = hwPort;

    op = MAD_PTP_READ_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TsMode.\n"));
        return MAD_FAIL;
    }

    tmpData = madLong2Short(opData.ptpData >>8);
    if (tmpData>=PTP_FRAME_SIZE)
      *tsMode = MAD_PTP_TS_MODE_IN_FRAME_END;
    else if (tmpData == PTP_TS_LOC_RESERVED_2)
      *tsMode = MAD_PTP_TS_MODE_IN_RESERVED_2;
    else
      *tsMode = MAD_PTP_TS_MODE_IN_REG;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetMeanPathDelay_port
*
* DESCRIPTION:
*		This routine sets ingress mean path delay on a port.
* 
* INPUTS:
*       port - The logical port number
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*		
*******************************************************************************/
MAD_STATUS mdPtpSetMeanPathDelay_port
(
	IN   MAD_DEV	*dev,
    IN   MAD_LPORT	port,
    IN   MAD_U16	delay
)
{
	MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				 hwPort;

    MAD_DBG_INFO(("mdPtpSetMeanPathDelay_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
    {
        MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	opData.ptpBlock = 0;
    opData.ptpAddr = 0x1C;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
	opData.ptpData = delay;
    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing mean path delay.\n"));
        return MAD_FAIL;
    }

	MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetMeanPathDelay_port
*
* DESCRIPTION:
*		This routine gets ingress mean path delay on a port.
* 
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* RETURNS:
*		MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*		
*******************************************************************************/

MAD_STATUS mdPtpGetMeanPathDelay_port
(
	IN   MAD_DEV	*dev,
    IN   MAD_LPORT	port,
    OUT  MAD_U16	*delay
)
{
	MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				 hwPort;

    MAD_DBG_INFO(("mdPtpGetMeanPathDelay_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
    {
        MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	opData.ptpBlock = 0;
    opData.ptpAddr = 0x1C;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading mean path delay.\n"));
        return MAD_FAIL;
    }

	*delay = madLong2Short(opData.ptpData);

	MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetIngPathDelayAsym_port
*
* DESCRIPTION:
*		This routine sets ingress path delay asymmetry on a port.
* 
* INPUTS:
*       port - The logical port number
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*		
*******************************************************************************/
MAD_STATUS mdPtpSetIngPathDelayAsym_port
(
	IN   MAD_DEV	*dev,
    IN   MAD_LPORT	port,
    IN   MAD_U16	asym
)
{
	MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				 hwPort;

	MAD_DBG_INFO(("mdPtpSetIngPathDelayAsym_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
    {
        MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
	
	opData.ptpBlock = 0;    /* PTP register space */
	opData.ptpAddr = 0x1D;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
	opData.ptpData = asym;
    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ingress path delay asymmetry.\n"));
        return MAD_FAIL;
    }

	MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetIngPathDelayAsym_port
*
* DESCRIPTION:
*		This routine gets ingress path delay asymmetry on a port.
* 
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* RETURNS:
*		MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*		
*******************************************************************************/
MAD_STATUS mdPtpGetIngPathDelayAsym_port
(
	IN   MAD_DEV	*dev,
    IN   MAD_LPORT	port,
    OUT  MAD_U16	*asym
)
{
	MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				 hwPort;

	MAD_DBG_INFO(("mdPtpGetIngPathDelayAsym_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
    {
        MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
	
	opData.ptpBlock = 0;    /* PTP register space */
	opData.ptpAddr = 0x1D;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading ingress path delay asymmetry.\n"));
        return MAD_FAIL;
    }

	*asym = madLong2Short(opData.ptpData);

	MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpSetEgPathDelayAsym_port
*
* DESCRIPTION:
*		This routine sets egress path delay asymmetry on a port.
* 
* INPUTS:
*       port - The logical port number
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*		
*******************************************************************************/
MAD_STATUS mdPtpSetEgPathDelayAsym_port
(
	IN   MAD_DEV	*dev,
    IN   MAD_LPORT	port,
    IN   MAD_U16	asym
)
{
	MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				 hwPort;

	MAD_DBG_INFO(("mdPtpSetEgPathDelayAsym_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
    {
        MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
	
	opData.ptpBlock = 0;    /* PTP register space */
	opData.ptpAddr = 0x1E;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
	opData.ptpData = asym;
    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writeg egress path delay asymmetry.\n"));
        return MAD_FAIL;
    }

	MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetEgPathDelayAsym_port
*
* DESCRIPTION:
*		This routine gets egress path delay asymmetry on a port.
* 
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the assymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements. 
*
* RETURNS:
*		MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*		
*******************************************************************************/
MAD_STATUS mdPtpGetEgPathDelayAsym_port
(
	IN   MAD_DEV	*dev,
    IN   MAD_LPORT	port,
    OUT  MAD_U16	*asym
)
{
	MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				 hwPort;

	MAD_DBG_INFO(("mdPtpGetEgPathDelayAsym_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
    {
        MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }
	
	opData.ptpBlock = 0;    /* PTP register space */
	opData.ptpAddr = 0x1E;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed readeg egress path delay asymmetry.\n"));
        return MAD_FAIL;
    }

	*asym = madLong2Short(opData.ptpData);

	MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetPTPInt_port
*
* DESCRIPTION:
*       This routine gets port PTP interrupt status.
*        The PTP Interrupt bit gets set for a given port when an incoming PTP 
*        frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*        Similary PTP Interrupt bit gets set for a given port when an outgoing
*        PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*        This bit gets cleared upon software reading and clearing the corresponding
*        time counter valid bits that are valid for that port.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpInt     - interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPInt_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *ptpInt
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpGetPTPInt_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpPort = 0xF;    /* Global register */
    opData.ptpAddr = 8;
	if (dev->deviceId == MAD_88E151x)
	{
		op = MAD_PTP_READ_MULTIPLE_DATA;
		opData.nData = 2;
	}
	else
	{
		op = MAD_PTP_READ_DATA;
	}

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

/*    opData.ptpData &= (1 << dev->maxPorts) - 1; */

	if (dev->deviceId == MAD_88E151x)
	{
		*ptpInt = opData.ptpMultiData[1];
        *ptpInt <<= 11;
        *ptpInt |= (opData.ptpMultiData[0] & ((1 << 11) - 1));
	}
	else
	{
		*ptpInt = MAD_PORTVEC_2_LPORTVEC(opData.ptpData);
	}

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* mdPtpGetPTPIntStatus_port
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for a port.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       ptpIntSt   - interrupt status for
*                       ptpTrigGenInt;      Trigger generate mode Interrupt
*                       ptpEventInt;        Event Capture Interrupt 
*                       ptpUpperPortInt;    Upper Ports Interrupt   
*                       ptpIntStPortVect
*                         interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_FAIL    - on error
*       MAD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPIntStatus_port
(
    IN  MAD_DEV						*dev,
	IN	MAD_LPORT					port,
    OUT MAD_PTP_INTERRUPT_STATUS	*ptpIntSt
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;         /* the physical port number     */


	MAD_DBG_INFO(("mdPtpGetPTPIntStatus_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = MAD_PTP_READ_MULTIPLE_DATA;
    opData.nData = 2;
    opData.ptpAddr = 8;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTP status.\n"));
        return MAD_FAIL;
    }

    if (dev->deviceId == MAD_88E151x)
	{
		ptpIntSt->ptpIntStPortVect = opData.ptpMultiData[1];
		ptpIntSt->ptpIntStPortVect <<= 11;
		ptpIntSt->ptpIntStPortVect |= (opData.ptpMultiData[0] & ((1 << 11) - 1));

		ptpIntSt->ptpTrigGenInt = madLong2Char((opData.ptpMultiData[0]>>15)&1);
		ptpIntSt->ptpEventInt = madLong2Char((opData.ptpMultiData[0]>>14)&1);
		ptpIntSt->ptpUpperPortInt = madLong2Char((opData.ptpMultiData[0]>>11)&1);
	}

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGetPTPGlobalTime_port
*
* DESCRIPTION:
*       This routine gets the port global timer value that is running off of the free
*        running switch core clock.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetPTPGlobalTime_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *ptpTime
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("mdPtpGetPTPGlobalTime_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }


    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_MULTIPLE_DATA;
#if 0
    opData.ptpPort = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:0xF;    /* Global register */
    opData.ptpAddr = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:9;
#else
	opData.ptpPort = 0xE; /* TAI Global */
	opData.ptpAddr = 14;

#endif
    opData.nData = 2;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTP Global Time.\n"));
        return MAD_FAIL;
    }

    *ptpTime = MAD_PTP_BUILD_TIME(opData.ptpMultiData[1],opData.ptpMultiData[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGetTimeStamped
*
* DESCRIPTION:
*        This routine retrieves the PTP port status that includes time stamp value 
*        and sequce Id that are captured by PTP logic for a PTP frame that needs 
*        to be time stamped.
*
* INPUTS:
*       port - logical port number.
*       timeToRead    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type)
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_[ERROR_STATUS]     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTimeStamped
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TIME    timeToRead,
    OUT MAD_PTP_TS_STATUS    *ptpStatus
)
{
    MAD_STATUS           retVal;
    MAD_U8                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                baseReg;

    MAD_DBG_INFO(("mdPtpGetTimeStamped Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

        baseReg = 8;

    switch (timeToRead)
    {
        case MAD_PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case MAD_PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case MAD_PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            MAD_DBG_INFO(("Invalid time to be read\n"));
            return MAD_API_PTP_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpBlock = 0;

/* #ifndef USE_SINGLE_READ */
#if 1 
    op = MAD_PTP_READ_TIMESTAMP_DATA;
    opData.nData = 4;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    ptpStatus->isValid = (opData.ptpMultiData[0] & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpStatus->status = (MAD_PTP_INT_STATUS)((opData.ptpMultiData[0] >> 1) & 0x3);
    ptpStatus->timeStamped = MAD_PTP_BUILD_TIME(opData.ptpMultiData[2],opData.ptpMultiData[1]);
    ptpStatus->ptpSeqId = opData.ptpMultiData[3];
#else
    {
    MAD_U32 data[4], i;

    op = MAD_PTP_READ_DATA;

    for (i=0; i<4; i++)
    {
        if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
        {
            MAD_DBG_INFO(("Failed reading DisPTP.\n"));
            return MAD_FAIL;
        }
    
        data[i] = opData.ptpData;
        opData.ptpAddr++;
    }

    ptpStatus->isValid = (data[0] & 0x1) ? MAD_TRUE : MAD_FALSE;
    ptpStatus->status = (MAD_PTP_INT_STATUS)((data[0] >> 1) & 0x3);
    ptpStatus->timeStamped = MAD_PTP_BUILD_TIME(data[2],data[1]);
    ptpStatus->ptpSeqId = data[3];

    }
#endif

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpGetTSValidSt
*
* DESCRIPTION:
*        This routine gets PTP Time valid bit.
*
* INPUTS:
*       port - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type)
*
* OUTPUTS:
*    isValid        - MAD_TRUE: valid TS, MAD_FALSE: invalid TS.
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_[ERROR_STATUS]     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTSValidSt
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TIME    timeToReset,
    OUT MAD_BOOL    *isValid
)
{
    MAD_STATUS           retVal;
    MAD_U8                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                baseReg;

    MAD_DBG_INFO(("mdPtpGetTSValidSt Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

        baseReg = 8;

    switch (timeToReset)
    {
        case MAD_PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case MAD_PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case MAD_PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            MAD_DBG_INFO(("Invalid time to get valid\n"));
            return MAD_API_PTP_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpData = 0;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Port Status.\n"));
        return MAD_FAIL;
    }

    *isValid = (opData.ptpData & 0x1) ? MAD_TRUE : MAD_FALSE;
    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpResetTimeStamp
*
* DESCRIPTION:
*        This routine resets PTP Time valid bit so that PTP logic can time stamp
*        a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       port - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MAD_PTP_TIME enum type)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_[ERROR_STATUS]     - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpResetTimeStamp
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_PTP_TIME    timeToReset
)
{
    MAD_STATUS           retVal;
    MAD_U8                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                baseReg;

    MAD_DBG_INFO(("mdPtpResetTimeStamp Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

        baseReg = 8;

    switch (timeToReset)
    {
        case MAD_PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case MAD_PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case MAD_PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            MAD_DBG_INFO(("Invalid time to reset\n"));
            return MAD_API_PTP_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpData = 0;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Port Status.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetTSDiscardCount_port
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Departure frame discard counter 
*        for PTP frames that need hardware time stamping.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       discardCount    - discard counters
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetTSDiscardCount_port
(
    IN  MAD_DEV						*dev,
    IN  MAD_LPORT					port,
    OUT MAD_PTP_TS_DISCARD_COUNTER	*discardCount
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;

	MAD_DBG_INFO(("mdPtpGetTSDiscardCount_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpAddr = 0x15;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpBlock = 0;
    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    discardCount->depTSDisCount = madLong2Char((opData.ptpData>>12)&0xf);
    discardCount->depNOTSDisCount = madLong2Char((opData.ptpData>>8)&0xf);
    discardCount->arrTSDisCount = madLong2Char((opData.ptpData>>4)&0xf);
    discardCount->arrNOTSDisCount = madLong2Char((opData.ptpData>>0)&0xf);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpGetOutFCSErr_port
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Output CRC Error counter.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       outFcsErr    - PTP Output CRC Error counter.
*
* RETURNS:
*       MAD_OK         - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetOutFCSErr_port
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    OUT MAD_U8       *outFcsErr
)
{
    MAD_STATUS           retVal;
    MAD_U8               hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;

    MAD_DBG_INFO(("mdPtpGetOutFCSErr_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpAddr = 0x14;
    opData.ptpPort = hwPort;
	opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpBlock = 0;
    op = MAD_PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTP output CRC error counter.\n"));
        return MAD_FAIL;
    }

    *outFcsErr = madLong2Char((opData.ptpData)&0xff);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port - logical port number.
*       ptpPort     - original PTP port number. (global port is 0xf)
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpGetReg
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_U8    ptpPort,
    IN  MAD_U32        regOffset,
    OUT MAD_U32        *data
)
{
    MAD_STATUS           retVal;
    MAD_U8                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;

    MAD_DBG_INFO(("mdPtpGetReg Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }


    if (regOffset > 0x1F)
    {
        MAD_DBG_INFO(("Invalid reg offset\n"));
        return MAD_API_PTP_BAD_PARAM;
    }

    op = MAD_PTP_READ_DATA;
    opData.ptpPort = ptpPort;
    opData.ptpAddr = regOffset;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *data = opData.ptpData;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* mdPtpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port         - logical port number
*       ptpPort     - original PTP port number. (global port is 0xf)
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS mdPtpSetReg
(
    IN  MAD_DEV     *dev,
    IN  MAD_LPORT    port,
    IN  MAD_U8    ptpPort,
    IN  MAD_U32        regOffset,
    IN  MAD_U32        data
)
{
    MAD_STATUS           retVal;
    MAD_U8                hwPort;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;

    MAD_DBG_INFO(("mdPtpSetReg Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }


    if ((regOffset > 0x1F) || (data > 0xFFFF))
    {
        MAD_DBG_INFO(("Invalid reg offset/data\n"));
        return MAD_API_PTP_BAD_PARAM;
    }

    op = MAD_PTP_WRITE_DATA;
    opData.ptpPort = ptpPort;
    opData.ptpAddr = regOffset;
    opData.ptpData = data;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }


    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiSetEventConfig_port
*
* DESCRIPTION:
*       This routine sets port TAI Event Capture configuration parameters.
*
* INPUTS:
*       port - The logical port number
*       eventData  - TAI Event Capture configuration parameters.
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetEventConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_TAI_EVENT_CONFIG    *eventData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetEventConfig_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= ~((3<<14)|(1<<8));
    if (eventData->intEn)
        opData.ptpData |= (1 << 8);
    if (eventData->eventOverwrite)
        opData.ptpData |= (1 << 15);
    if (eventData->eventCtrStart)
        opData.ptpData |= (1 << 14);

	if (dev->deviceId == MAD_88E151x)
	{
		opData.ptpData &= ~(1<<13);
		if (eventData->eventPhase)
			opData.ptpData |= (1 << 13);
	}

    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI global config.\n"));
        return MAD_FAIL;
    }

	if (dev->deviceId == MAD_88E151x)
	{
		/* getting Capture trigger, offset 9 */
		op = MAD_PTP_READ_DATA;
		opData.ptpAddr = 9;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI global config.\n"));
			return MAD_FAIL;
		}
		opData.ptpData &= ~(1<<14);
		opData.ptpData |= (eventData->captTrigEvent==MAD_TRUE)?0x4000:0x0;

		op = MAD_PTP_WRITE_DATA;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI global config.\n"));
			return MAD_FAIL;
		}
	}

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventConfig_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*       eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_TAI_EVENT_CONFIG    *eventData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetEventConfig_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    eventData->intEn = (opData.ptpData & (1<<8))?MAD_TRUE:MAD_FALSE;
    eventData->eventOverwrite = (opData.ptpData & (1<<15))?MAD_TRUE:MAD_FALSE;
    eventData->eventCtrStart = (opData.ptpData & (1<<14))?MAD_TRUE:MAD_FALSE;

	if (dev->deviceId == MAD_88E151x)
	{
		eventData->eventPhase = (opData.ptpData & (1<<13))?MAD_TRUE:MAD_FALSE;

		op = MAD_PTP_READ_DATA;
		opData.ptpAddr = 9;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI global config.\n"));
			return MAD_FAIL;
		}
		eventData->captTrigEvent = (opData.ptpData & (1<<14))?MAD_TRUE:MAD_FALSE;
	}

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventStatus_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventStatus_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_TAI_EVENT_STATUS    *status
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                 data[2];
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetEventStatus_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 9;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }

    status->isValid = (opData.ptpData & (1<<8))?MAD_TRUE:MAD_FALSE;
    status->eventCtr = opData.ptpData & 0xFF;
    status->eventErr = (opData.ptpData & (1<<9))?MAD_TRUE:MAD_FALSE;

    if (status->isValid == MAD_FALSE)
    {
        return MAD_OK;
    }

    opData.ptpAddr = 0x0A;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }
    data[0] = opData.ptpData;

    opData.ptpAddr = 0x0B;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading PTPEType.\n"));
        return MAD_FAIL;
    }
    data[1] = opData.ptpData;

    status->eventTime = MAD_PTP_BUILD_TIME(data[1],data[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiGetEventInt_port
*
* DESCRIPTION:
*       This routine gets port TAI Event Capture Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Event capture
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetEventInt_port
(
  IN  MAD_DEV      *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *intStatus
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("madTaiGetEventInt_port Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpPort = 0xE;    /* TAI Global register */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 9;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *intStatus = (opData.ptpData & 0x0100)?MAD_TRUE:MAD_FALSE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiClearEventInt_port
*
* DESCRIPTION:
*       This routine clear TAI Event Capture Interrupt status.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiClearEventInt_port
(
    IN  MAD_DEV			*dev,
	IN  MAD_LPORT		port
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;         /* the physical port number     */
    
    MAD_DBG_INFO(("madTaiClearEventInt_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 9;
    opData.ptpBlock = 0;
	opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading eventInt.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= 0x7EFF;
    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing eventInt.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiSetClockSelect_port
*
* DESCRIPTION:
*       This routine sets several clock select in TAI.
*
* INPUTS:
*       port - logical port number.
*       clkSelect  - TAI clock select configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetClockSelect_port
(
    IN  MAD_DEV					*dev,
	IN  MAD_LPORT				port,
    IN  MAD_TAI_CLOCK_SELECT    *clkSelect
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;         /* the physical port number     */
    
    MAD_DBG_INFO(("madTaiSetClockSelect_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xE;    /* TAI register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;
	opData.ptpAddr = 0x1E;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= ~(0x5F1F);
    opData.ptpData |= (clkSelect->priRecClkSel)&0x1F;
    opData.ptpData |= (((clkSelect->syncRecClkSel)&0x1F) << 8);
    opData.ptpData |= (((clkSelect->ptpExtClk)&1) << 14);

    op = MAD_PTP_WRITE_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI global config.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiGetClockSelect_port
*
* DESCRIPTION:
*       This routine gets several clock select in TAI.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       clkSelect  - TAI clock select configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetClockSelect_port
(
    IN   MAD_DEV				*dev,
	IN   MAD_LPORT				port,
    OUT  MAD_TAI_CLOCK_SELECT   *clkSelect
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;         /* the physical port number     */
    
    MAD_DBG_INFO(("madTaiGetClockSelect_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

    opData.ptpBlock = 0;
    opData.ptpPort = 0xE;    /* TAI register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;
	opData.ptpAddr = 0x1E;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    clkSelect->priRecClkSel = madLong2Char(opData.ptpData&0x1f);
    clkSelect->syncRecClkSel = madLong2Char((opData.ptpData >> 8) & 0x1f);
    clkSelect->ptpExtClk = (opData.ptpData>> 14) & 1;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiSetClockConfig_port
*
* DESCRIPTION:
*       This routine sets clock configuration in TAI.
*
* INPUTS:
*       port - logical port number.
*       clkConfig  - TAI clock configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetClockConfig_port
(
    IN  MAD_DEV					*dev,
	IN  MAD_LPORT				port,
    IN  MAD_TAI_CLOCK_CONFIG	*clkConfig
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;         /* the physical port number     */
    
    MAD_DBG_INFO(("madTaiGetClockSelect_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

    opData.ptpBlock = 0;
	opData.ptpPort = 0xE;    /* TAI register */
	opData.ptpPhyPort = hwPort;    /* port number */


	op = MAD_PTP_READ_DATA;
	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading IRL clock enable.\n"));
        return MAD_FAIL;
    }

	opData.ptpData &= ~(1<<10);
	if (clkConfig->irlClkEn)
		opData.ptpData |= (1 << 10);

	op = MAD_PTP_WRITE_DATA;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing IRL clock enable.\n"));
        return MAD_FAIL;
    }

    opData.ptpData = clkConfig->tsClkPer;
    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 1;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing time stamping clock period.\n"));
        return MAD_FAIL;
    }

	opData.ptpData = clkConfig->irlClkGenAmt;
	op = MAD_PTP_WRITE_DATA;
	opData.ptpAddr = 6;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed writing IRL clock generation amount.\n"));
		return MAD_FAIL;
	}
	
	opData.ptpData = ((clkConfig->compDir == MAD_TRUE)?1:0);
	opData.ptpData = (opData.ptpData << 15) | clkConfig->irlClkComp;
	op = MAD_PTP_WRITE_DATA;
	opData.ptpAddr = 7;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed writing IRL clock compensation amount.\n"));
		return MAD_FAIL;
	}

	opData.ptpData = (clkConfig->irlClkCompSubPs & 0xff);
	op = MAD_PTP_WRITE_DATA;
	opData.ptpAddr = 8;

	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed writing IRL clock comp amount in subpico secs.\n"));
		return MAD_FAIL;
	}

	MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiGetClockConfig_port
*
* DESCRIPTION:
*       This routine gets clock configuration in TAI.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       clkConfig  - TAI clock configuration parameters.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetClockConfig_port
(
    IN  MAD_DEV					*dev,
	IN  MAD_LPORT				port,
    OUT MAD_TAI_CLOCK_CONFIG    *clkConfig
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;         /* the physical port number     */
    
    MAD_DBG_INFO(("madTaiGetClockSelect_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

    opData.ptpBlock = 0;
    opData.ptpPort = 0xE;    /* TAI register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;

	opData.ptpAddr = 0;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading IRL clock enable.\n"));
        return MAD_FAIL;
    }
	clkConfig->irlClkEn = (opData.ptpData & (1<<10))?MAD_TRUE:MAD_FALSE;

    /* getting Time Stamping Clock Period in pico seconds, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading time stamping clock period.\n"));
        return MAD_FAIL;
    }
    clkConfig->tsClkPer = madLong2Short(opData.ptpData);

	opData.ptpAddr = 6;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed reading IRL clock generation amount.\n"));
		return MAD_FAIL;
	}
	clkConfig->irlClkGenAmt = madLong2Short(opData.ptpData&0xffff);

	opData.ptpAddr = 7;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed reading IRL clock compensation amount.\n"));
		return MAD_FAIL;
	}
	clkConfig->irlClkComp = (MAD_U16)(opData.ptpData & 0x7fff);
	clkConfig->compDir = ((opData.ptpData & 0x8000)?1:0);

	opData.ptpAddr = 8;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed reading IRL clock compensation sub pico seconds.\n"));
		return MAD_FAIL;
	}
	clkConfig->irlClkCompSubPs = madLong2Char(opData.ptpData&0xff);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiGetTrigInt_port
*
* DESCRIPTION:
*       This routine gets port TAI Trigger Interrupt status.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Trigger
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigInt_port
(
  IN  MAD_DEV      *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *intStatus
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				 hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetTrigInt_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (dev->deviceId == MAD_88E151x)
	{
		opData.ptpPort = 0xF;
	}
	else
	{
		opData.ptpPort = 0xE;    /* Global register */
	}
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 8;
    opData.ptpBlock = 0;
    opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *intStatus = (opData.ptpData & 0x8000)?MAD_TRUE:MAD_FALSE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiClearTrigInt_port
*
* DESCRIPTION:
*       This routine clears TAI Trigger Interrupt status.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiClearTrigInt_port
(
    IN  MAD_DEV       *dev,
	IN  MAD_LPORT     port
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiClearTrigInt_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (dev->deviceId == MAD_88E151x)
	{
		opData.ptpPort = 0xF;
	}
	else
	{
		opData.ptpPort = 0xE;    /* Global register */
	}

    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 8;
    opData.ptpBlock = 0;
	opData.ptpPhyPort = hwPort;    /* port number */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI global config.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= 0x7fff;
    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiSetTrigConfig_port
*
* DESCRIPTION:
*       This routine sets port TAI Trigger configuration parameters.
*
* INPUTS:
*       port - The logical port number
*       trigEn    - enable/disable TAI Trigger.
*       trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetTrigConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL     trigEn,
  IN  MAD_TAI_TRIGGER_CONFIG    *trigData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetTrigConfig_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 2;
    opData.ptpData = MAD_PTP_L16_TIME(trigData->trigGenAmt);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Trig generation amount.\n"));
        return MAD_FAIL;
    }

    opData.ptpAddr = 3;
    opData.ptpData = MAD_PTP_H16_TIME(trigData->trigGenAmt);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Trig generation amount.\n"));
        return MAD_FAIL;
    }

	op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 4;        /* TrigClkComp */
	if (dev->deviceId == MAD_88E151x)
	{
		opData.ptpData = (trigData->clkCompDir)?0x8000:0;
		opData.ptpData |= (trigData->trigClkComp&=0x7fff);
	}
	else
	{
		/* opData.ptpData = (MAD_U16)trigData->trigClkComp; */
		opData.ptpData = trigData->trigClkComp; 
	}

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI trig compare clock.\n"));
        return MAD_FAIL;
    }
    
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 5;        /* PulseWidth */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
		MAD_DBG_INFO(("Failed writing TAI global config.\n"));
        return MAD_FAIL;
    }

    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 5;        /* PulseWidth */
    opData.ptpData &= (~0xF000);
    opData.ptpData |= (MAD_U16)(trigData->pulseWidth << 12);

	if (dev->deviceId == MAD_88E151x)
	{
		opData.ptpData &= ~(0x3ff);
		opData.ptpData |= ((trigData->pulseWidthRange&7)<<8);
		opData.ptpData |= ((trigData->trigClkCompSub&0xff));
	}

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
		MAD_DBG_INFO(("Failed writing TAI global config.\n"));
		return MAD_FAIL;
    }

	if (dev->deviceId == MAD_88E151x)
	{
		op = MAD_PTP_WRITE_DATA;
		opData.ptpAddr = 0x1F;

		if (trigData->timeDec == MAD_TRUE)
			opData.ptpData |= (1 << 11); 
		else
			opData.ptpData &= ~(1 << 11);

		opData.ptpData &= ~(0x7ff);
		opData.ptpData |= trigData->incDecAmt;
		
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing TAI global config.\n"));
			return MAD_FAIL;
		}

		op = MAD_PTP_WRITE_DATA;
		opData.ptpAddr = 0x1E;        /* Trig config */

		opData.ptpData &= (~0x2080);
		opData.ptpData |= (MAD_U16)(trigData->trigWindow << 13);
		opData.ptpData |= (MAD_U16)(trigData->trig2Signal << 7);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing TAI global config.\n"));
			return MAD_FAIL;
		}

		op = MAD_PTP_WRITE_DATA;
		opData.ptpAddr = 0x10;
		opData.ptpData = MAD_PTP_L16_TIME(trigData->trigGenTime);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing trigger generation time.\n"));
			return MAD_FAIL;
		}

		opData.ptpAddr = 0x11;
		opData.ptpData = MAD_PTP_H16_TIME(trigData->trigGenTime);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing trigger generation time.\n"));
			return MAD_FAIL;
		}

		opData.ptpAddr = 0x13;
		opData.ptpData = MAD_PTP_L16_TIME(trigData->trigGenDelay);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing trigger generation delay.\n"));
			return MAD_FAIL;
		}

		opData.ptpAddr = 0x14;
		opData.ptpData = MAD_PTP_L16_TIME(trigData->irlGenTime);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing IRLGen time.\n"));
			return MAD_FAIL;
		}

		opData.ptpAddr = 0x15;
		opData.ptpData = MAD_PTP_H16_TIME(trigData->irlGenTime);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing IRLGen time.\n"));
			return MAD_FAIL;
		}
	}

	/* Set Trig control, offset 0 */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    if (trigData->intEn == MAD_TRUE)
        opData.ptpData |= (1 << 9);
	else
        opData.ptpData &= ~(1 << 9);

	if (trigData->mode == MAD_TAI_TRIG_ON_GIVEN_TIME)
        opData.ptpData |= (1 << 1);
	else
        opData.ptpData &= ~(1 << 1);

	if (trigEn == MAD_TRUE)
		opData.ptpData |= 1;
	else
		opData.ptpData &= ~1;

	if (dev->deviceId == MAD_88E151x)
	{
		if (trigData->trigPhase == MAD_TRUE)
			opData.ptpData |= (1 << 12);
		else
			opData.ptpData &= ~(1 << 12);

		if (trigData->trigLock == MAD_TRUE)
			opData.ptpData |= (1 << 7);
		else
			opData.ptpData &= ~(1 << 7);

		opData.ptpData &= ~(7<<4);
		opData.ptpData |= ((trigData->trigLockRange&7)<<4);

		if (trigData->incDecEn == MAD_TRUE)
			opData.ptpData |= (1 << 11);
		else
			opData.ptpData &= ~(1 << 11);

		if (trigData->blkUpdate == MAD_TRUE)
			opData.ptpData |= (1 << 3);
		else
			opData.ptpData &= ~(1 << 3);

		if (trigData->multiSyncMode == MAD_TAI_MULTI_PTP_SYNC_ENABLE)
			opData.ptpData |= (1 << 2);
		else
			opData.ptpData &= ~(1 << 2);
    }

    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI global config.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;

}

/*******************************************************************************
* madTaiGetTrigConfig_port
*
* DESCRIPTION:
*       This routine gets port TAI Trigger configuration parameters.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigConfig_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *trigEn,
  OUT MAD_TAI_TRIGGER_CONFIG    *trigData
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("madTaiGetTrigConfig_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 3;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI trigGenAmt.\n"));
        return MAD_FAIL;
    }
    trigData->trigGenAmt = opData.ptpData;

    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI trigGenAmt.\n"));
        return MAD_FAIL;
    }
    trigData->trigGenAmt = (trigData->trigGenAmt)<<16 | opData.ptpData;

	 /* getting TrigClkComp, offset 4 */
    opData.ptpAddr = 4;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI TrigClkComp.\n"));
        return MAD_FAIL;
    }
	if (dev->deviceId == MAD_88E151x)
	{
		trigData->clkCompDir = (opData.ptpData & 0x8000)?1:0;
		trigData->trigClkComp = opData.ptpData &0x7fff;
	}
	else
	{
		trigData->trigClkComp = (MAD_U32)opData.ptpData;
	}

    opData.ptpAddr = 5;        /* PulseWidth */
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI PulseWidth.\n"));
        return MAD_FAIL;
    }
    trigData->pulseWidth = (MAD_U32)((opData.ptpData >> 12) & 0xF);

	if (dev->deviceId == MAD_88E151x)
	{
		trigData->pulseWidthRange = madLong2Char((opData.ptpData>>8)&0x7);
		trigData->trigClkCompSub = opData.ptpData&0xff;

		op = MAD_PTP_READ_DATA;
		opData.ptpAddr = 0x1F;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI PulseWidth.\n"));
			return MAD_FAIL;
		}
		trigData->timeDec = (opData.ptpData >> 11)&1;
		trigData->incDecAmt = opData.ptpData&0x7ff;

		op = MAD_PTP_READ_DATA;
		opData.ptpAddr = 0x1E;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI global config.\n"));
			return MAD_FAIL;
		}
		trigData->trigWindow = (opData.ptpData & 0x2000)?1:0;
		trigData->trig2Signal = (opData.ptpData & 0x80)?1:0;

		op = MAD_PTP_READ_DATA;
		opData.ptpAddr = 0x11;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI trigGenTime.\n"));
			return MAD_FAIL;
		}
		trigData->trigGenTime = opData.ptpData;

		opData.ptpAddr = 0x10;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI trigGenTime.\n"));
			return MAD_FAIL;
		}

		trigData->trigGenTime = (trigData->trigGenTime)<<16 | opData.ptpData;

		opData.ptpAddr = 0x13;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI trigGenDelay.\n"));
			return MAD_FAIL;
		}
		trigData->trigGenDelay = opData.ptpData;

		/* lockCorrect read only */
		opData.ptpAddr = 0x12;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading TAI lockCorrect.\n"));
			return MAD_FAIL;
		}
		trigData->lockCorrect = madLong2Char(opData.ptpData & 0xF);

		op = MAD_PTP_READ_DATA;
		opData.ptpAddr = 0x15;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading IRLGen Time.\n"));
			return MAD_FAIL;
		}
		trigData->irlGenTime = opData.ptpData;
		opData.ptpAddr = 0x14;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading IRLGen Time.\n"));
			return MAD_FAIL;
		}

		trigData->irlGenTime = (trigData->irlGenTime)<<16 | opData.ptpData;

		/* lockCorrect read only */
		opData.ptpAddr = 0x16;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed reading IRL Lock Correction.\n"));
			return MAD_FAIL;
		}
		trigData->irlCorrValid = ((opData.ptpData & 0x10)?MAD_TRUE:MAD_FALSE);
		trigData->irlLockCorrect = madLong2Char(opData.ptpData & 0xF);
	}

	/* Set Trig control, offset 0 */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI trig config 0.\n"));
        return MAD_FAIL;
    }

	trigData->intEn = ((opData.ptpData >> 9)&1)?MAD_TRUE:MAD_FALSE;
	trigData->mode = (opData.ptpData&2)?MAD_TAI_TRIG_ON_GIVEN_TIME:MAD_TAI_TRIG_PERIODIC_PULSE;
	*trigEn = (opData.ptpData&1)?MAD_TRUE:MAD_FALSE;

	if (dev->deviceId == MAD_88E151x)
	{
		trigData->trigPhase = ((opData.ptpData >> 12)&1)?MAD_TRUE:MAD_FALSE;
		trigData->trigLock = ((opData.ptpData >> 7)&1)?MAD_TRUE:MAD_FALSE;
		trigData->trigLockRange = madLong2Char((opData.ptpData >>4)&7);
		trigData->incDecEn = ((opData.ptpData >> 11)&1)?MAD_TRUE:MAD_FALSE;
		trigData->blkUpdate = ((opData.ptpData >> 3)&1)?MAD_TRUE:MAD_FALSE;
		trigData->multiSyncMode = (opData.ptpData&4)?MAD_TAI_MULTI_PTP_SYNC_ENABLE:MAD_TAI_MULTI_PTP_SYNC_DISABLE;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiSetTrigLock_port
*
* DESCRIPTION:
*       This routine sets TAI Trigger lock.
*
* INPUTS:
*       port - logical port number.
*       trigLockCfg - trig lock configuration:
*        trigLock       - trigger lock set.
*        trigLockRange  - trigger lock range.
*        lockCorrect    -  Trig Lock Correction amount 
*        trigGenTime     - Trigger Generation Time.  
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiSetTrigLock_port
(
    IN  MAD_DEV							*dev,
	IN  MAD_LPORT						port,
    INOUT  MAD_TAI_TRIG_LOCK_CONFIG		*trigLockCfg
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;    /* the physical port number     */

    MAD_DBG_INFO(("madTaiSetTrigLock_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xE;    /* TAI register */
    opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;
    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= ~(0xf<<4);
    opData.ptpData |= (trigLockCfg->trigLock==MAD_TRUE) ?0x80:0;
    opData.ptpData |= ((trigLockCfg->trigLockRange&0x7)<<4);

    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI global config.\n"));
        return MAD_FAIL;
    }

    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 0x10;
    opData.ptpData = MAD_PTP_L16_TIME(trigLockCfg->trigGenTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed writing trigGen time.\n"));
		return MAD_FAIL;
	}

    opData.ptpAddr = 0x11;
    opData.ptpData = MAD_PTP_H16_TIME(trigLockCfg->trigGenTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
		MAD_DBG_INFO(("Failed writing trigGen time.\n"));
		return MAD_FAIL;
	}
 
    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiGetTrigLock_port
*
* DESCRIPTION:
*       This routine gets TAI Trigger lock and trigger lock range.
*
* INPUTS:
*       port - logical port number.
*
* OUTPUTS:
*       trigLockCfg - trig lock configuration:
*        trigLock       - trigger lock set.
*        trigLockRange  - trigger lock range.
*        lockCorrect    -  Trig Lock Correction amount 
*        trigGenTime     - Trigger Generation Time. 
*
* RETURNS:
*       MAD_OK      - on success
*       MAD_[ERROR_STATUS]    - on error
*       MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MAD_STATUS madTaiGetTrigLock_port
(
    IN		MAD_DEV						*dev,
	IN		MAD_LPORT					port,
    INOUT   MAD_TAI_TRIG_LOCK_CONFIG    *trigLockCfg
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
	MAD_U8				 hwPort;    /* the physical port number     */

    MAD_DBG_INFO(("madTaiGetTrigLock_port Called.\n"));
	/* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  
    opData.ptpBlock = 0;
    opData.ptpPort = 0xE;    /* TAI register */
    opData.ptpPhyPort = hwPort;    /* port number */

    op = MAD_PTP_READ_DATA;
    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI global config.\n"));
        return MAD_FAIL;
    }

    trigLockCfg->trigLock = (opData.ptpData&0x80)?MAD_TRUE:MAD_FALSE;
    trigLockCfg->trigLockRange = (MAD_U8)((opData.ptpData&0x70)>>4);

    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 0x11;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
        MAD_DBG_INFO(("Failed reading TAI trigGenTime.\n"));
        return MAD_FAIL;
	}
    trigLockCfg->trigGenTime = opData.ptpData;
    opData.ptpAddr = 0x10;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
        MAD_DBG_INFO(("Failed reading TAI trigGenTime.\n"));
        return MAD_FAIL;
	}

    trigLockCfg->trigGenTime = (trigLockCfg->trigGenTime)<<16 | opData.ptpData;

	/* lockCorrect read only */
    opData.ptpAddr = 0x12;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
	{
        MAD_DBG_INFO(("Failed reading TAI lockCorrect.\n"));
        return MAD_FAIL;
	}
    trigLockCfg->lockCorrect = madLong2Char(opData.ptpData & 0xF);
	  
    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* madTaiGetTSClkPer_port
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the port PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        clk        - time stamping clock period
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_[ERROR_STATUS]    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiGetTSClkPer_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_U32        *clk
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetTSClkPer_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 1;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TS clock period.\n"));
        return MAD_FAIL;
    }

    *clk = (MAD_U32)opData.ptpData;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetTSClkPer_port
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the port PTP 
*        Global Time Counter.
*
* INPUTS:
*        port - logical port number.
*        clk        - time stamping clock period
*
* OUTPUTS:
*		 None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_[ERROR_STATUS]    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTSClkPer_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        clk
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */

    MAD_DBG_INFO(("madTaiSetTSClkPer_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 1;

    opData.ptpData = (MAD_U16)clk; 

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TS clock period.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetMultiPTPSync_port
*
* DESCRIPTION:
*         This routine sets port Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiSetMultiPTPSync_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_BOOL     multiEn,
  IN  MAD_32        syncTime
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */

    MAD_DBG_INFO(("madTaiSetMultiPTPSync_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= ~(1 << 2);

    if (multiEn == MAD_FALSE)
    {
        op = MAD_PTP_WRITE_DATA;

        if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
        {
            MAD_DBG_INFO(("Failed writing TAI register.\n"));
            return MAD_FAIL;
        }

        return MAD_OK;
    }

    opData.ptpData |= (1 << 2);


    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpAddr = 2;
    opData.ptpData = MAD_PTP_L16_TIME(syncTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpAddr = 3;
    opData.ptpData = MAD_PTP_H16_TIME(syncTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiGetMultiPTPSync_port
*
* DESCRIPTION:
*         This routine gets port Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*        and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiGetMultiPTPSync_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL     *multiEn,
  OUT MAD_32        *syncTime
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U32                 data[2];
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetMultiPTPSync_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    if(!(opData.ptpData & (1 << 2)))
    {
        *multiEn = MAD_FALSE;
        *syncTime = 0;
        return MAD_OK;
    }
    *multiEn = MAD_TRUE;

    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }
    data[0] = opData.ptpData;

    opData.ptpAddr = 3;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }
    data[1] = opData.ptpData;

    *syncTime = MAD_PTP_BUILD_TIME(data[1],data[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiGetTimeIncDec_port
*
* DESCRIPTION:
*         This routine retrieves port Time increment/decrement setup.
*        This amount specifies the number of units of PTP Global Time that need to be 
*        incremented or decremented. This is used for adjusting the PTP Global Time 
*        counter value by a certain amount.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*        expired    - MAD_TRUE if inc/dec occurred, MAD_FALSE otherwise
*        inc        - MAD_TRUE if increment, MAD_FALSE if decrement
*        amount    - increment/decrement amount
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_[ERROR_STATUS]    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         Time increment or decrement will be excuted once.
*
*******************************************************************************/
MAD_STATUS madTaiGetTimeIncDec_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  OUT MAD_BOOL        *expired,
  OUT MAD_BOOL        *inc,
  OUT MAD_U32        *amount
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiGetTimeIncDec_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

	if (dev->deviceId == MAD_88E151x)
		opData.ptpAddr = 0x1F;
	else
		opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    *inc = (opData.ptpData & 0x800)?MAD_FALSE:MAD_TRUE;
    *amount = (MAD_U32)(opData.ptpData & 0x7FF);

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

	if (dev->deviceId == MAD_88E151x)
		*expired = (opData.ptpData & 0x800)?MAD_FALSE:MAD_TRUE;
	else
		*expired = (opData.ptpData & 0x8)?MAD_FALSE:MAD_TRUE;

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeInc_port
*
* DESCRIPTION:
*         This routine enables port time increment by the specifed time increment amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        incremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Increment occurs just once.
*
* INPUTS:
*        port - The logical port number
*        amount    - time increment amount (0 ~ 0x7FF)
*
* OUTPUTS:
*		 None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_[ERROR_STATUS]    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeInc_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        amount
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetTimeInc_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    /* set TimeIncAmt */
    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

	if (dev->deviceId == MAD_88E151x)
		opData.ptpAddr = 0x1F;
	else
		opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= 0xF000;
    opData.ptpData |= amount;

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    /* set TimeIncEn */
    opData.ptpBlock = 0x0;    /* PTP register space */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    if (dev->deviceId == MAD_88E151x)
		opData.ptpData |= 0x800;
	else
		opData.ptpData |= 0x8;

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading DisPTP.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeDec_port
*
* DESCRIPTION:
*         This routine enables port time decrement by the specifed time decrement amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        decremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Decrement occurs just once.
*
* INPUTS:
*       port - The logical port number
*        amount    - time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         MAD_OK      - on success
*         MAD_[ERROR_STATUS]    - on error
*         MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeDec_port
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U32        amount
)
{
    MAD_STATUS           retVal;
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
    MAD_U8          hwPort;         /* the physical port number     */

    /* translate LPORT to hardware port */
    MAD_DBG_INFO(("madTaiSetTimeDec_port Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

    /* set TimeIncAmt */
    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

	if (dev->deviceId == MAD_88E151x)
		opData.ptpAddr = 0x1F;
	else
		opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    opData.ptpData &= 0xF000;
    opData.ptpData |= amount;
    opData.ptpData |= 0x800;    /* decrement */

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    /* set TimeIncEn */
    opData.ptpBlock = 0x0;    /* PTP register space */
    opData.ptpPhyPort = hwPort;    /* port number */

    opData.ptpPort = 0xE;    /* TAI register */
    op = MAD_PTP_READ_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TAI register.\n"));
        return MAD_FAIL;
    }

    if (dev->deviceId == MAD_88E151x)
		opData.ptpData |= 0x800;
	else
		opData.ptpData |= 0x8;

    op = MAD_PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TAI register.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}


/*******************************************************************************
* mdPtpGlobalPointDataSet
*
* DESCRIPTION:
*        Set indirect register pointer control data from PTP Global registers. 
*
* INPUTS:
*        port - logical port number.
*        point - Pointer to following index registers:
*            MAD_PTP_GLOBAL_CONFIG_PTP_MODE_REG                  
*            MAD_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM       
*            MAD_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC          
*            MAD_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC           
*            MAD_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC            
*            MAD_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC             
*        data -  PTP Global configuration data write to the indirect register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpGlobalPointDataSet
(
    IN  MAD_DEV         *dev,
	IN  MAD_LPORT		  port,
    IN  MAD_U8             point,
    IN  MAD_U8             data
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
    MAD_U32       count, tmpData;
    MAD_DBG_INFO(("mdPtpGlobalPointDataSet Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	madSemTake(dev,dev->ptpRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Scratch and Misc control is ready. */
    count = 5;
    tmpData = 0x8000;
    while(tmpData ==0x8000)
    {
        retVal = mdPtpGetReg(dev, port, 0xf, 7, &tmpData);
           if(retVal != MAD_OK)
           {
				madSemGive(dev,dev->ptpRegsSem);
				return retVal;
           }
	  if(--count==0) break;
    }

  tmpData =  (MAD_U16)((1 << 15) | (point << 8) | data);

  retVal = mdPtpSetReg(dev, port, 0xf, 7, tmpData);
  if(retVal != MAD_OK)
  {
     MAD_DBG_INFO(("Failed.\n"));
     madSemGive(dev,dev->ptpRegsSem);
     return retVal;
  }

  madSemGive(dev,dev->ptpRegsSem);

  return retVal;

}

/*******************************************************************************
* mdPtpGlobalPointDataGet
*
* DESCRIPTION:
*        Get indirect register pointer control data from PTP Global registers. 
*
* INPUTS:
*        point - Pointer to following index registers:
*            MAD_PTP_GLOBAL_CONFIG_PTP_MODE_REG                  
*            MAD_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM       
*            MAD_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC          
*            MAD_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC           
*            MAD_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC            
*            MAD_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC             
* OUTPUTS:
*        data -  PTP Global configuration data read from the indirect register 
*                pointed to by the point above.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpGlobalPointDataGet
(
    IN  MAD_DEV         *dev,
	IN  MAD_LPORT		   port,
    IN  MAD_U8             point,
    OUT  MAD_U8            *data
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
    MAD_U32       tmpData;
    int count=0x10;
    MAD_DBG_INFO(("mdPtpGlobalPointDataSet Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	madSemTake(dev,dev->ptpRegsSem,OS_WAIT_FOREVER);

	tmpData =  (MAD_U16)((point << 8) | 0);
	retVal = mdPtpSetReg(dev, port, 0xf, 7, tmpData);
	if(retVal != MAD_OK)
	{
	   MAD_DBG_INFO(("Failed.\n"));
	   madSemGive(dev,dev->ptpRegsSem);
	   return retVal;
	}
	
    do {
        retVal = mdPtpGetReg(dev, port, 0xf, 7, &tmpData);
        if(retVal != MAD_OK)
           {
               MAD_DBG_INFO(("Failed.\n"));
			   madSemGive(dev,dev->ptpRegsSem);
               return retVal;
        }
		if((count--)==0) return MAD_FAIL;
    } while (tmpData&0x8000);

    *data = madLong2Char(tmpData&0xff);

    madSemGive(dev,dev->ptpRegsSem);

    return retVal;
}

/*******************************************************************************
* mdPtpModeSet_port
*
* DESCRIPTION:
*        Set MAD_PTP_GLOBAL_CONFIG_PTP_MODE_REG from PTP point/data registers. 
*
* INPUTS:
*        port - logical port number.
*        mode -  
*          tsAtSFD    - Time Stamp At Start of Frame Delimiter
*          disPtp2Switch    - Disable PTP Signals to the Switch core
*		   accelNewPTP	 - Hardware accelerates newer PTP frames
*          AltScheme    - Alternate Scheme
*          GrandMstr    - Grand Master Enable.
*          OneStep    - OneStep Enable.
*          PTPMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpModeSet_port
(
	IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
    IN  MAD_PTP_MODE_CONFIG	*mode
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
    MAD_U8		data;

    MAD_DBG_INFO(("mdPtpModeSet_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

	data = (((mode->tsAtSFD&1)<<7) |
            ((mode->disPtp2Switch&1)<<6) |
			((mode->accelNewPTP&1)<< 5) |
            ((mode->altScheme&1)<<4) |
            ((mode->grandMstr&1)<<3) |
            ((mode->oneStep&1)<<2) |
            (mode->PTPMode&3));

    if((retVal = mdPtpGlobalPointDataSet(dev, port, MAD_PTP_GLOBAL_CONFIG_PTP_MODE_REG, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }
	return retVal;
}

/*******************************************************************************
* mdPtpModeGet_port
*
* DESCRIPTION:
*        Get MAD_PTP_GLOBAL_CONFIG_PTP_MODE_REG from PTP point/data registers. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*        mode -  
*          tsAtSFD    - Time Stamp At Start of Frame Delimiter
*          disPtp2Switch    - Disable PTP Signals to the Switch core
*		   accelNewPTP	 - Hardware accelerates newer PTP frames
*          AltScheme    - Alternate Scheme
*          GrandMstr    - Grand Master Enable.
*          OneStep    - OneStep Enable.
*          ptpMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpModeGet_port
(
	IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
    OUT MAD_PTP_MODE_CONFIG	*mode
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
    MAD_U8		data;

    MAD_DBG_INFO(("mdPtpModeGet_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

	if((retVal = mdPtpGlobalPointDataGet(dev, port, MAD_PTP_GLOBAL_CONFIG_PTP_MODE_REG, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }

	mode->tsAtSFD = (data>>7)&1;
    mode->disPtp2Switch = (data>>6)&1;
	mode->accelNewPTP = (data>>5)&1;
    mode->altScheme = (data>>4)&1;
    mode->grandMstr = (data>>3)&1;
    mode->oneStep = (data>>2)&1;
    mode->PTPMode = data&3;

	return retVal;
}


/*******************************************************************************
* mdPtpSetExtTimeFormat_port
*
* DESCRIPTION:
*        Set MAD_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM from PTP point/data registers. 
*
* INPUTS:
*        port - logical port number.
*        format-
*			intPTPTime    - Internal PTP Time.
*			extMask    - External Mask on the lower bits of the selected 1722 time.
*			ext1722Time    - IEEE 1722 Time Format to External switch blocks.
*			extTASel    - Time Array Selection for External switch blocks.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpSetExtTimeFormat_port
(
	IN  MAD_DEV						*dev,
	IN  MAD_LPORT					port,
    IN  MAD_PTP_EXTERNAL_TIME_FORM  *format
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
    MAD_U8		data;

    MAD_DBG_INFO(("mdPtpSetExtTimeFormat_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

	data = (((format->intPTPTime&1)<<6) |
            ((format->extMask&3)<<4) |
            ((format->ext1722Time&1)<<3) |
            (format->extTASel&3));

    if((retVal = mdPtpGlobalPointDataSet(dev, port, MAD_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }
	return retVal;
}

/*******************************************************************************
* mdPtpGetExtTimeFormat_port
*
* DESCRIPTION:
*        Get MAD_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM from PTP point/data registers. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*        format-
*			intPTPTime    - Internal PTP Time.
*			extMask    - External Mask on the lower bits of the selected 1722 time.
*			ext1722Time    - IEEE 1722 Time Format to External switch blocks.
*			extTASel    - Time Array Selection for External switch blocks.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpGetExtTimeFormat_port
(
	IN  MAD_DEV						*dev,
	IN  MAD_LPORT					port,
    OUT MAD_PTP_EXTERNAL_TIME_FORM  *format
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
    MAD_U8		data;

    MAD_DBG_INFO(("mdPtpGetExtTimeFormat_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

    if((retVal = mdPtpGlobalPointDataGet(dev, port, MAD_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }

	format->intPTPTime = (data>>6)&1;
    format->extMask = (data>>4)&3;
    format->ext1722Time = (data>>3)&1;
    format->extTASel = data&3;

	return retVal;
}

/*******************************************************************************
* mdPtpGetIngActVec_port
*
* DESCRIPTION:
*        Get IEEE 1588 or IEEE 802.1AS Ingress Action Vectors. 
*
* INPUTS:
*        port - logical port number.
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*        
* OUTPUTS:
*        vec - IEEE 1588 or IEEE 802.1AS Ingress Action Vector 2 to 0.
*			
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpGetIngActVec_port
(
    IN  MAD_DEV	*dev,
	IN  MAD_LPORT	port,
	IN  MAD_PTP_SPEC spec,
    OUT MAD_U32		*vec
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
	MAD_U8		 data;
	MAD_U8		 point;

	MAD_DBG_INFO(("mdPtpGetIngActVec_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (dev->deviceId == MAD_88E151x)
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}    

	point = (spec == MAD_PTP_IEEE_1588 ? MAD_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC :
		MAD_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC);

    if((retVal = mdPtpGlobalPointDataGet(dev, port, point, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }
	*vec = data;

	if((retVal = mdPtpGlobalPointDataGet(dev, port, point+1, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }
	*vec |= data << 8;

	if((retVal = mdPtpGlobalPointDataGet(dev, port, point+2, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }
	*vec |= data << 16;
	
	return retVal;
}

/*******************************************************************************
* mdPtpSetIngActVec_port
*
* DESCRIPTION:
*        Set IEEE 1588 or IEEE 802.1AS Ingress Action Vectors. 
*
* INPUTS:
*        port - logical port number.
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*		 vec - IEEE 1588 or IEEE 802.1AS Ingress Action Vector 2 to 0.
*        
* OUTPUTS:
*        None.	
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpSetIngActVec_port
(
    IN  MAD_DEV			*dev,
	IN  MAD_LPORT		port,
	IN  MAD_PTP_SPEC	spec,
    IN  MAD_U32			vec
)
{
    MAD_STATUS   retVal;         /* Functions return value.      */
	MAD_U8		 data;
	MAD_U8		 point;

	MAD_DBG_INFO(("mdPtpSetIngActVec_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (dev->deviceId == MAD_88E151x)
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}    

	point = (spec == MAD_PTP_IEEE_1588 ? MAD_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC :
	MAD_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC);

    data = (MAD_U8)(vec & 0xff);
    if((retVal = mdPtpGlobalPointDataSet(dev, port, point, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }
	
	data = (MAD_U8)((vec >> 8) & 0xff);
	if((retVal = mdPtpGlobalPointDataSet(dev, port, point+1, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }

	data = (MAD_U8)((vec >> 16) & 0xff);
	if((retVal = mdPtpGlobalPointDataSet(dev, port, point+2, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }
	
	return retVal;
}

/*******************************************************************************
* mdPtpGetEgActVec_port
*
* DESCRIPTION:
*        Get IEEE_1588 or IEEE 802.1AS Egress Action Vectors. 
*
* INPUTS:
*        port - logical port number.
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*        
* OUTPUTS:
*        vec - IEEE_1588 or IEEE 802.1AS Egress Action Vector 3 to 0.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpGetEgActVec_port
(
    IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
	IN  MAD_PTP_SPEC		spec,
    OUT MAD_PTP_EG_ACT_VEC	*vec
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
	MAD_U8		 data;
	MAD_U8		 point;

    MAD_DBG_INFO(("mdPtpGetEgActVec_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}    

	point = (spec == MAD_PTP_IEEE_1588 ? MAD_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC :
		MAD_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC);

    if((retVal = mdPtpGlobalPointDataGet(dev, port, point, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }
	*vec = data&0xFF;

	if((retVal = mdPtpGlobalPointDataGet(dev, port, point+1, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }
	*vec |= (data&0xFF) << 8;

	if((retVal = mdPtpGlobalPointDataGet(dev, port, point+2, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }
	*vec |= (data&0xFF) << 16;

	if((retVal = mdPtpGlobalPointDataGet(dev, port, point+3, &data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed get PTP point/data.\n"));
        return MAD_FAIL;
    }
	*vec |= (data&0xFF)<< 24;
	
	return retVal;
}

/*******************************************************************************
* mdPtpSetEgActVec_port
*
* DESCRIPTION:
*        Set IEEE_1588 or IEEE 802.1AS Egress Action Vectors. 
*
* INPUTS:
*        port - logical port number.
*		 spec - PTP_IEEE_1588 or PTP_IEEE_802_1AS.
*        vec - IEEE_1588 or IEEE 802.1AS Egress Action Vector 3 to 0.
*        
* OUTPUTS:
*		 None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpSetEgActVec_port
(
    IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
	IN  MAD_PTP_SPEC		spec,
    IN  MAD_PTP_EG_ACT_VEC	vec
)
{
	MAD_STATUS    retVal;         /* Functions return value.      */
	MAD_U8		 data;
	MAD_U8		 point;

    MAD_DBG_INFO(("mdPtpSetEgActVec_port Called.\n"));

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}    

	point = (spec == MAD_PTP_IEEE_1588 ? MAD_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC :
		MAD_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC);

	data = (MAD_U8)(vec & 0xFF);
    if((retVal = mdPtpGlobalPointDataSet(dev, port, point, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }

	data = (MAD_U8)((vec >> 8) & 0xFF);
	if((retVal = mdPtpGlobalPointDataSet(dev, port, point+1, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }

	data = (MAD_U8)((vec >> 16) & 0xFF);
	if((retVal = mdPtpGlobalPointDataSet(dev, port, point+2, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }

	data = (MAD_U8)((vec >> 24) & 0xFF);
	if((retVal = mdPtpGlobalPointDataSet(dev, port, point+3, data)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed set PTP point/data.\n"));
        return MAD_FAIL;
    }

	return retVal;
}

/*******************************************************************************
* mdPtpTODLoadPointSet_port
*
* DESCRIPTION:
*        Set Time of Day Load Point Register. 
*
* INPUTS:
*        port - logical port number.
*        loadData - The ToDLoadPt register is used:
*          ToD Store All Registers – it is used to determine the instant in time 
*          that the selected Time Array is loaded. The load occurs at the instant
*          the PTP Global Time (TAI Global offset 0x0E & 0x0F) matches the contents
*          of this register.
*          ToD Capture – it is used to capture the instant in time that the Capture
*          occurred. On each ToD Capture, the contents of this register will be loaded
*          with the current value contained in the PTP Global Time (TAI Global offsets
*          0x0E & 0x0F).
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODLoadPointSet_port
(
    IN  MAD_DEV		*dev,
	IN  MAD_LPORT	port,
    IN  MAD_U32     loadData
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION   op;
    MAD_PTP_OP_DATA     opData;
	MAD_U8				hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpTODLoadPointSet_port Called.\n"));

	hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
    op = MAD_PTP_WRITE_DATA;
	opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpAddr = 0x10;
    opData.ptpData = madLong2Short(loadData&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing TOD Load Point.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x11;
    opData.ptpData = madLong2Short((loadData>>16)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TOD Load Point.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpTODLoadPointGet_port
*
* DESCRIPTION:
*        Get Time of Day Load Point Register. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*        loadData - The ToDLoadPt register is used:
*          ToD Store All Registers – it is used to determine the instant in time 
*          that the selected Time Array is loaded. The load occurs at the instant
*          the PTP Global Time (TAI Global offset 0x0E & 0x0F) matches the contents
*          of this register.
*          ToD Capture – it is used to capture the instant in time that the Capture
*          occurred. On each ToD Capture, the contents of this register will be loaded
*          with the current value contained in the PTP Global Time (TAI Global offsets
*          0x0E & 0x0F).
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODLoadPointGet_port
(
    IN  MAD_DEV			*dev,
	IN  MAD_LPORT		port,
    OUT MAD_U32         *loadData
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION   op;
    MAD_PTP_OP_DATA     opData;
	MAD_U8				hwPort;         /* the physical port number     */

    MAD_DBG_INFO(("mdPtpTODLoadPointGet_port Called.\n"));

	hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */ 
	opData.ptpPhyPort = hwPort;    /* port number */
    opData.ptpAddr = 0x10;
    op = MAD_PTP_READ_MULTIPLE_DATA;
    opData.nData = 2;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading TOD Load Point.\n"));
        return MAD_FAIL;
    }

    *loadData = opData.ptpMultiData[1];
    *loadData <<= 16;
    *loadData |= (opData.ptpMultiData[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpTODOpCtrlSet_port
*
* DESCRIPTION:
*        Set ToD operation control. 
*
* INPUTS:
*        port - logical port number.
*        todOpCtrl -  following parameters:
*           ptpOpCtrlTodOp
*           ptpOpCtrlTimeArray;
*           ptpOpCtrlClkValid;
*           ptpOpCtrlDomainNum;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODOpCtrlSet_port
(
    IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
    IN  MAD_PTP_TOD_OP_CTRL	*todOpCtrl
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION   op;
    MAD_PTP_OP_DATA     opData;
    MAD_U32				tmpData;
	MAD_U8				hwPort;
    int count=0x10;

    MAD_DBG_INFO(("mdPtpTODOpCtrlSet_port Called.\n"));

	hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}

    do {
        retVal = mdPtpGetReg(dev, port, 0xf, 0x12, &tmpData);
        if(retVal != MAD_OK)
           {
               MAD_DBG_INFO(("Failed.\n"));
			   madSemGive(dev,dev->ptpRegsSem);
               return retVal;
        }
		if((count--)==0) return MAD_FAIL;
		madWait(10000);
    } while (tmpData&0x8000);

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 0x12;
    opData.ptpData = ((1 << 15) | 
		              (((todOpCtrl->ptpOpCtrlTodOp)&7)<<12) | 
                      (((todOpCtrl->ptpOpCtrlTimeArray)&3)<<9) |
                      (((todOpCtrl->ptpOpCtrlClkValid)&1)<<8) |
                      (todOpCtrl->ptpOpCtrlDomainNum&0xff));

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD operation control.\n"));
        return MAD_FAIL;
    }

	return retVal;
}

/*******************************************************************************
* mdPtpTODOpCtrlGet_port
*
* DESCRIPTION:
*        Get ToD operation control. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*        todOpCtrl -  following parameters:
*           ptpOpCtrlTodOp
*           ptpOpCtrlTimeArray;
*           ptpOpCtrlClkValid;
*           ptpOpCtrlDomainNum;
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODOpCtrlGet_port
(
    IN   MAD_DEV				*dev,
	IN   MAD_LPORT				port,
    OUT  MAD_PTP_TOD_OP_CTRL	*todOpCtrl
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION   op;
    MAD_PTP_OP_DATA     opData;
	MAD_U8				hwPort;
    int count=0x10;

    MAD_DBG_INFO(("mdPtpTODOpCtrlGet_port Called.\n"));

	hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 0x12;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading ToD operation control.\n"));
        return MAD_FAIL;
    }
	todOpCtrl->ptpOpCtrlTodOp = madLong2Char((opData.ptpData>>12)&7); 
    todOpCtrl->ptpOpCtrlTimeArray = madLong2Char((opData.ptpData>>9)&3);
    todOpCtrl->ptpOpCtrlClkValid = madLong2Char((opData.ptpData>>8)&1);
    todOpCtrl->ptpOpCtrlDomainNum = madLong2Char(opData.ptpData&0xff);

	return retVal;
}

/*******************************************************************************
* mdPtpPulseSet_port
*
* DESCRIPTION:
*        Set PTP Pulse parameters. 
*
* INPUTS:
*        port - logical port number.
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpPulseSet_port
(
    IN  MAD_DEV					*dev,
	IN  MAD_LPORT				port,
    IN  MAD_PTP_PULSE_STRUCT    *pulsePara
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION	op;
    MAD_PTP_OP_DATA		opData;
	MAD_U8				hwPort;

	MAD_DBG_INFO(("mdPtpPulseSet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 0x1E;
    opData.ptpData = ((((pulsePara->ptpPulseWidth)&0xf)<<12) | 
                      (((pulsePara->ptp1ppsWidthRange)&7)<<8) |
                      (((pulsePara->ptp1ppsPhase)&1)<<3) |
                      (pulsePara->ptp1ppsSelect&0x3));
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing Pulse parameters.\n"));
        return MAD_FAIL;
    }

	return retVal;
}

/*******************************************************************************
* mdPtpPulseGet_port
*
* DESCRIPTION:
*        Get PTP Pulse parameters. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpPulseGet_port
(
    IN   MAD_DEV				*dev,
	IN   MAD_LPORT				port,
    OUT  MAD_PTP_PULSE_STRUCT   *pulsePara
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION	op;
    MAD_PTP_OP_DATA		opData;
	MAD_U8				hwPort;

	MAD_DBG_INFO(("mdPtpPulseGet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_DATA;
    opData.ptpAddr = 0x1E;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading Pulse parameters.\n"));
        return MAD_FAIL;
    }
	pulsePara->ptpPulseWidth = madLong2Char((opData.ptpData>>12)&0xF); 
    pulsePara->ptp1ppsWidthRange = madLong2Char((opData.ptpData>>8)&7);
    pulsePara->ptp1ppsPhase = madLong2Char((opData.ptpData>>3)&1);
    pulsePara->ptp1ppsSelect = madLong2Char(opData.ptpData&0x3);

	return retVal;
}

/*******************************************************************************
* mdPtpTODTimeSet_port
*
* DESCRIPTION:
*        Set Time Array Time of Day,. 
*
* INPUTS:
*        port - logical port number.
*        ts - Full TOD time structure (10 bytes).
*             The five ToD registers (at PTP Global offsets 0x13 to 0x17) 
*             contain the 10 byte representation of time used in IEEE 1588 & 
*             IEEE 802.1AS frames. These registers are used to load this 
*             representation of time into the selected Time Array on ToD Store 
*             All Registers operations. They contain the selected Time Array’s 
*             representation of this time after ToD Capture operations complete.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODTimeSet_port
(
    IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
    IN  MAD_PTP_TS_STRUCT	*ts
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION   op;
    MAD_PTP_OP_DATA     opData;
	MAD_U8				hwPort;

    MAD_DBG_INFO(("mdPtpTODTimeSet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 0x13;
    opData.ptpData = ts->nanoseconds&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x14;
    opData.ptpData = (ts->nanoseconds>>16)&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x15;
    opData.ptpData = ts->seconds&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x16;
    opData.ptpData = (ts->seconds>>16)&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x17;
    opData.ptpData = (ts->topseconds)&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpTODTimeGet_port
*
* DESCRIPTION:
*        Get Time Array Time of Day,. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*        ts - Full TOD time structure (10 bytes).
*             The five ToD registers (at PTP Global offsets 0x13 to 0x17) 
*             contain the 10 byte representation of time used in IEEE 1588 & 
*             IEEE 802.1AS frames. These registers are used to load this 
*             representation of time into the selected Time Array on ToD Store 
*             All Registers operations. They contain the selected Time Array’s 
*             representation of this time after ToD Capture operations complete.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODTimeGet_port
(
    IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
    OUT MAD_PTP_TS_STRUCT    *ts
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION   op;
    MAD_PTP_OP_DATA     opData;
	MAD_U8				hwPort;

    MAD_DBG_INFO(("mdPtpTODTimeGet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;
    opData.ptpAddr = 0x13;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading ToD.\n"));
        return MAD_FAIL;
    }

    ts->nanoseconds = opData.ptpMultiData[1]&0xffff;
    ts->nanoseconds <<= 16;
    ts->nanoseconds |= (opData.ptpMultiData[0]&0xffff);
    ts->seconds = opData.ptpMultiData[3]&0xffff;
    ts->seconds <<= 16;
    ts->seconds |= (opData.ptpMultiData[2]&0xffff);

	op = MAD_PTP_READ_DATA;
	opData.ptpAddr = 0x17;
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading ToD.\n"));
        return MAD_FAIL;
    }

    ts->topseconds = madLong2Short((opData.ptpData&0xffff));

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtp1722TimeSet_port
*
* DESCRIPTION:
*        Set Time Array 1722 Time of Day. 
*
* INPUTS:
*        port - logical port number.
*        ts - Full 1722 TOD time structure (8 bytes).
*           The four 1722 ToD registers (at PTP Global offsets 0x18 to 0x1B) 
*           contain an 8 byte representation of time used in IEEE 1722 frames 
*           (IEEE1722 uses only the lower 32-bits of this time. The 64-bit 
*           representation is used in PCI-e and it is a simple extension of the 
*           IEEE 1722 representation of time that wraps). These registers are used
*           to load this representation of time into the selected Time Array on 
*           ToD Store All Registers operations. They contain the selected Time 
*           Array’s representation of this time after ToD Capture operations complete.*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtp1722TimeSet_port
(
    IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
    IN  unsigned long long   ts
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA      opData;
    MAD_U8				hwPort;

    MAD_DBG_INFO(("mdPtp1722TimeSet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_WRITE_DATA;

    opData.ptpAddr = 0x18;
    opData.ptpData = madLonglong2Long(ts&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing 1722 ToD.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x19;
    opData.ptpData = madLonglong2Long((ts>>16)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing 1722 ToD.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x1a;
    opData.ptpData = madLonglong2Long((ts>>32)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing 1722 ToD.\n"));
        return MAD_FAIL;
    }
    opData.ptpAddr = 0x1b;
    opData.ptpData = madLonglong2Long((ts>>48)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing 1722 ToD.\n"));
        return MAD_FAIL;
    }

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtp1722TimeGet_port
*
* DESCRIPTION:
*        Get Time Array 1722 Time of Day. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*        ts - Full 1722 TOD time structure (8 bytes).
*           The four 1722 ToD registers (at PTP Global offsets 0x18 to 0x1B) 
*           contain an 8 byte representation of time used in IEEE 1722 frames 
*           (IEEE1722 uses only the lower 32-bits of this time. The 64-bit 
*           representation is used in PCI-e and it is a simple extension of the 
*           IEEE 1722 representation of time that wraps). These registers are used
*           to load this representation of time into the selected Time Array on 
*           ToD Store All Registers operations. They contain the selected Time 
*           Array’s representation of this time after ToD Capture operations complete.*
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtp1722TimeGet_port
(
    IN  MAD_DEV				*dev,
	IN  MAD_LPORT			port,
    OUT unsigned long long  *ts
)
{
    MAD_STATUS    retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION    op;
    MAD_PTP_OP_DATA        opData;
	MAD_U8				hwPort;

    MAD_DBG_INFO(("mdPtp1722TimeGet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;
    opData.ptpAddr = 0x18;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading 1722 ToD.\n"));
        return MAD_FAIL;
    }

    *ts = opData.ptpMultiData[3];
    *ts <<= 48;
    *ts = opData.ptpMultiData[2];
    *ts <<= 32;
    *ts = opData.ptpMultiData[1];
    *ts <<= 16;
    *ts |= (opData.ptpMultiData[0]);

    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdPtpTODCompSet_port
*
* DESCRIPTION:
*        Set PTP time array time of day compensation. 
*
* INPUTS:
*        port - logical port number.
*        comp - PPM difference between local crystal clocking this PTP block and
*				the PTP Grand Master device that this Time Array is tracking
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODCompSet_port
(
    IN  MAD_DEV		*dev,
	IN  MAD_LPORT	port,
    IN  MAD_U32		comp
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION	op;
    MAD_PTP_OP_DATA		opData;
    MAD_U8				hwPort;

    MAD_DBG_INFO(("mdPtpTODCompSet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_WRITE_DATA;
    opData.ptpAddr = 0x1C;
    opData.ptpData = (comp & 0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD compensation.\n"));
        return MAD_FAIL;
    }

	opData.ptpAddr = 0x1D;
	opData.ptpData = ((comp >> 16) & 0xffff);
	if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed writing ToD compensation.\n"));
        return MAD_FAIL;
    }

	return retVal;
}

/*******************************************************************************
* mdPtpTODCompGet_port
*
* DESCRIPTION:
*        Get PTP time array time of day compensation. 
*
* INPUTS:
*        port - logical port number.
*
* OUTPUTS:
*		 comp - PPM difference between local crystal clocking this PTP block and
*				the PTP Grand Master device that this Time Array is tracking
*
* RETURNS:
*        MAD_OK      - on success
*        MAD_[ERROR_STATUS]    - on error
*        MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
MAD_STATUS mdPtpTODCompGet_port
(
    IN   MAD_DEV		*dev,
	IN   MAD_LPORT		port,
    OUT  MAD_U32		*comp
)
{
    MAD_STATUS			retVal;         /* Functions return value.      */
    MAD_PTP_OPERATION	op;
    MAD_PTP_OP_DATA		opData;
	MAD_U8				hwPort;

    MAD_DBG_INFO(("mdPtpTODCompGet_port Called.\n"));

    hwPort = MAD_LPORT_2_PORT(port);
	if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }
    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_PTP_TAI_CAPABLE) ||
        !MAD_DEV_HWMODE(dev, 0, MAD_PHY_MODE_COPPER) )
    {
        MAD_DBG_ERROR(("PTP not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	if (!(dev->deviceId == MAD_88E151x))
	{
		MAD_DBG_INFO(("MAD_API_PTP_NOT_SUPPORT\n"));
        return MAD_API_PTP_NOT_SUPPORT;
	}  

    opData.ptpBlock = 0;
    opData.ptpPort = 0xF;    /* Global register */
	opData.ptpPhyPort = hwPort;    /* port number */
    op = MAD_PTP_READ_MULTIPLE_DATA;
	opData.nData = 2;
    opData.ptpAddr = 0x1C;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != MAD_OK)
    {
        MAD_DBG_INFO(("Failed reading ToD compensation.\n"));
        return MAD_FAIL;
    }

	*comp = (opData.ptpMultiData[1] & 0xffff) << 16;
	*comp |= opData.ptpMultiData[0];

	return retVal;
}



/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
/*******************************************************************************
* mapRegs
*
* DESCRIPTION:
*       This function maps parameters in PTP_OP_DATA to page and register number of PHY.
*
* INPUTS:
*       opData    - points to the data storage that the operation requires.
*
* OUTPUTS:
*       port    - port number.
*       page    - page number.
*       reg     - register number.
*
* RETURNS:
*       MAD_OK on success,
*       MAD_[ERROR_STATUS] otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MAD_STATUS mapRegs(MAD_PTP_OP_DATA    *opData, MAD_U8 *port, MAD_U16 *page, MAD_U8 *reg)
{
  *port = madLong2Char(opData->ptpPhyPort);   
  switch (opData->ptpPort)
  {
    case 0: /* PTP Port configuration/Status */
    case 1: /* PTP Port configuration/Status */
    case 2: /* PTP Port configuration/Status */
    case 3: /* PTP Port configuration/Status */
    /* Port 0, 1, 2, 3 for 1340*/
      *page = MAD_PAGE8;
      *reg = madLong2Char(opData->ptpAddr);
      if (opData->ptpAddr>15)
      {
        *page = MAD_PAGE9;
        *reg = madLong2Char(opData->ptpAddr-16);
      }
      break;
    case 0xe: /* TAI configuration/Status */
      *page = MAD_PAGE12;
      *reg = madLong2Char(opData->ptpAddr);
      if (opData->ptpAddr>15)
      {
        *page = MAD_PAGE13;
        *reg = madLong2Char(opData->ptpAddr-16);
      }
      break;
    case 0xf:/* PTP Global configuration/Status */
      *page = MAD_PAGE14;
      *reg = madLong2Char(opData->ptpAddr);
      if (opData->ptpAddr>15)
      {
        *page = MAD_PAGE15;
        *reg = madLong2Char(opData->ptpAddr-16);
      }
      break;
    default:
      return MAD_FAIL;
  }
  return MAD_OK;
}


/*******************************************************************************
* ptpReadplus
*
* DESCRIPTION:
*       This function accesses ToD Registers and some TAI Registers.
*
* INPUTS:
*       port - Port number to write the register for.
*       page - Page number to be accessed.
*       reg  - The register's address.
*		multi - Multi read(MAD_TRUE) or single read(MAD_FALSE).
*
* OUTPUTS:
*		ptpData    - points to the data storage that the operation requires.
*
* RETURNS:
*       MAD_OK on success,
*       MAD_[ERROR_STATUS] otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MAD_STATUS ptpReadplus
(
    IN   MAD_DEV    *dev,
    IN   MAD_U8     port,
    IN   MAD_U16    page,
    IN   MAD_U8     reg,
	IN	 MAD_BOOL	multi,
	INOUT MAD_PTP_OP_DATA        *opData
)
{
  MAD_STATUS	retVal;    /* Functions return value */
  MAD_U16		tmpData;
  unsigned int	i;

  if (page == MAD_PAGE12)
  {
	if (reg >= 0xE)
	{
		tmpData = (reg & 0xF) | 0x8E00;
	}
	else
	{
		MAD_DBG_ERROR(("Readplus should not be called.\n"));
		return MAD_FAIL;
	}
  }
  else if (page == MAD_PAGE15)
  {
	tmpData = (reg & 0xF) | 0x8F10;
  }
  else
  {
	MAD_DBG_ERROR(("Readplus should not be called.\n"));
	return MAD_FAIL;
  }

  if((retVal = madHwWritePagedPhyReg(dev,port,14,14,tmpData))!= MAD_OK)
  {
	MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",14,14));
	return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
  }

  if (multi)
  {
	for (i = 0; i < opData->nData; i++)
	{
	  if((retVal = madHwReadPagedPhyReg(dev,port,14,15,&tmpData))!= MAD_OK)
	  {
		MAD_DBG_ERROR(("Reading paged phy reg (%i,%i) failed.\n",14,15));
		return (retVal | MAD_API_FAIL_READ_PAGED_REG);
	  }
	  opData->ptpMultiData[i] = (MAD_U32)tmpData;
    }
  }
  else
  {
	if((retVal = madHwReadPagedPhyReg(dev,port,14,15,&tmpData))!= MAD_OK)
	{
	  MAD_DBG_ERROR(("Reading paged phy reg (%i,%i) failed.\n",14,15));
	  return (retVal | MAD_API_FAIL_READ_PAGED_REG);
	}
	opData->ptpData = tmpData;
  }

  return MAD_OK;
}


/*******************************************************************************
* ptpOperationPerform
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       ptpOp      - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       ptpData    - points to the data storage that the operation requires.
*
* RETURNS:
*       MAD_OK on success,
*       MAD_[ERROR_STATUS] otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MAD_STATUS ptpOperationPerform
(
  IN    MAD_DEV             *dev,
  IN    MAD_PTP_OPERATION        ptpOp,
  INOUT MAD_PTP_OP_DATA        *opData
)
{
  MAD_STATUS       retVal;    /* Functions return value */
  MAD_U16          data;     /* temporary Data storage */
  MAD_U32     i;
  MAD_U8     port;
  MAD_U16    page;
  MAD_U8     reg, kpReg;


  madSemTake(dev,dev->ptpRegsSem,OS_WAIT_FOREVER);

  /* Set the PTP Operation register */
  switch (ptpOp)
  {
    case MAD_PTP_WRITE_DATA:
      data = (MAD_U16)opData->ptpData;

      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
        return retVal;
      }

      if((retVal = madHwWritePagedPhyReg(dev,port,page,reg,data))!= MAD_OK)
      {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        madSemGive(dev,dev->ptpRegsSem);
		return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }
 
      madSemGive(dev,dev->ptpRegsSem);
      break;

    case MAD_PTP_READ_DATA:
      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
		return retVal;
      }
	  /* PTP global time registers and ToD registers should use readplus */
	  if (PTP_READPLUS_SUPPORT(page,reg))
	  {
		retVal = ptpReadplus(dev,port,page,reg,MAD_FALSE,opData);
	  }
	  else
	  {
		retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
        opData->ptpData = data;
	  }
      if(retVal != MAD_OK)
      {
        MAD_DBG_ERROR(("Reading paged phy reg (%i,%i) failed.\n",page,reg));
        madSemGive(dev,dev->ptpRegsSem);
        return (retVal | MAD_API_FAIL_READ_PAGED_REG);
      }
  
      madSemGive(dev,dev->ptpRegsSem);
      break;

    case MAD_PTP_READ_MULTIPLE_DATA:
	  retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
        return retVal;
      }
		
	  if (PTP_READPLUS_SUPPORT(page,reg))
	  {
		retVal = ptpReadplus(dev,port,page,reg,MAD_TRUE,opData);
		if(retVal != MAD_OK)
        {
          madSemGive(dev,dev->ptpRegsSem);
		  return retVal;
		}
	  }
	  else
	  {
		for(i=0; i<opData->nData; i++)
		{
		  retVal = mapRegs(opData, &port, &page, &reg);
          if(retVal != MAD_OK)
          {
            madSemGive(dev,dev->ptpRegsSem);
            return retVal;
          }
		  retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
          opData->ptpMultiData[i] = (MAD_U32)data;
          if(retVal != MAD_OK)
          {
            madSemGive(dev,dev->ptpRegsSem);
		    return retVal;
		  }
		  if (!PTP_READPLUS_SUPPORT(page,reg))
			opData->ptpAddr++;
		}
      }

      madSemGive(dev,dev->ptpRegsSem);
      break;

    case MAD_PTP_READ_TIMESTAMP_DATA:
      kpReg = madLong2Char(opData->ptpAddr);
      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
		return retVal;
      }

      retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
      opData->ptpMultiData[0] = (MAD_U32)data;
      if (!(data & 0x1))
      {
        /* valid bit is not set */
        madSemGive(dev,dev->ptpRegsSem);
        return retVal;
      }

      for(i=1; i<opData->nData; i++)
      {
        opData->ptpAddr=kpReg+i;
        retVal = mapRegs(opData, &port, &page, &reg);
        if(retVal != MAD_OK)
        {
          madSemGive(dev,dev->ptpRegsSem);
		  return retVal;
        }
		retVal = madHwReadPagedPhyReg(dev,port,page,reg,&data);
        opData->ptpMultiData[i] = (MAD_U32)data;
        if(retVal != MAD_OK)
        {
          madSemGive(dev,dev->ptpRegsSem);
          return retVal;
		}
        opData->ptpAddr++;
      }

      data =0;
      opData->ptpAddr = kpReg;
      retVal = mapRegs(opData, &port, &page, &reg);
      if(retVal != MAD_OK)
      {
        madSemGive(dev,dev->ptpRegsSem);
		return retVal;
      }

      if((retVal = madHwWritePagedPhyReg(dev,port,page,reg,data))!= MAD_OK)
      {
        MAD_DBG_ERROR(("Writing to paged phy reg (%i,%i) failed.\n",page,reg));
        madSemGive(dev,dev->ptpRegsSem);
        return (retVal | MAD_API_FAIL_WRITE_PAGED_REG);
      }

      madSemGive(dev,dev->ptpRegsSem);
      break;

    default:
      madSemGive(dev,dev->ptpRegsSem);
      return MAD_FAIL;
  }

  madSemGive(dev,dev->ptpRegsSem);
  return MAD_OK;
}





