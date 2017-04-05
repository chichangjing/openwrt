#include <madCopyright.h>

/*******************************************************************************
* madPTP.c
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

/* #define madPtpDefaultPort 2 */

/*******************************************************************************
* mdPtpSetEnable
*
* DESCRIPTION:
*       This routine set all ports PTP Enable/Disable.
*
* INPUTS:
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
MAD_STATUS mdPtpSetEnable
(
  IN  MAD_DEV     *dev,
  IN  MAD_BOOL    en
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetEnable
*
* DESCRIPTION:
*       This routine Get default port PTP Enable/Disable.
*
* INPUTS:
*       None.
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
MAD_STATUS mdPtpGetEnable
(
  IN  MAD_DEV     *dev,
  OUT  MAD_BOOL    *en
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
   
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* mdPtpSetRefClkSrc
*
* DESCRIPTION:
*       This routine sets PTP reference clock source.
*
* INPUTS:
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
MAD_STATUS mdPtpSetRefClkSrc
(
  IN  MAD_DEV     *dev,
  IN  MAD_BOOL    rClk
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetRefClkSrc_port(dev, port, rClk);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetRefClkSrc
*
* DESCRIPTION:
*       This routine gets PTP reference clock source.
*
* INPUTS:
*       None.
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
MAD_STATUS mdPtpGetRefClkSrc
(
  IN  MAD_DEV     *dev,
  OUT MAD_BOOL    *rClk
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
   
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetRefClkSrc_port(dev, port, rClk);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpSetInputSrc
*
* DESCRIPTION:
*       This routine sets PTP input source.
*
* INPUTS:
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
MAD_STATUS mdPtpSetInputSrc
(
  IN  MAD_DEV     *dev,
  IN  MAD_BOOL    inSrc
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetInputSrc_port(dev, port, inSrc);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetInputSrc
*
* DESCRIPTION:
*       This routine gets PTP input source.
*
* INPUTS:
*       None.
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
MAD_STATUS mdPtpGetInputSrc
(
  IN  MAD_DEV     *dev,
  OUT MAD_BOOL    *inSrc
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
   
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetInputSrc_port(dev, port, inSrc);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpSetOutputSrc
*
* DESCRIPTION:
*       This routine sets PTP output source.
*
* INPUTS:
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
MAD_STATUS mdPtpSetOutputSrc
(
  IN  MAD_DEV     *dev,
  IN  MAD_BOOL    outSrc
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetOutputSrc_port(dev, port, outSrc);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetOutputSrc
*
* DESCRIPTION:
*       This routine gets PTP output source.
*
* INPUTS:
*       None.
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
MAD_STATUS mdPtpGetOutputSrc
(
  IN  MAD_DEV     *dev,
  OUT MAD_BOOL    *outSrc
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
   
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetOutputSrc_port(dev, port, outSrc);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpSetIntEnable
*
* DESCRIPTION:
*       This routine set all ports PTP interrupt Enable/Disable.
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
MAD_STATUS mdPtpSetIntEnable
(
  IN  MAD_DEV     *dev,
  IN  MAD_BOOL    en
)
{
  MAD_STATUS           retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetIntEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetIntEnable
*
* DESCRIPTION:
*       This routine get default port PTP Interrupt Enable/Disable.
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
MAD_STATUS mdPtpGetIntEnable
(
  IN  MAD_DEV     *dev,
  OUT  MAD_BOOL    *en
)
{
  MAD_STATUS           retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetIntEnable_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpSetConfig
*
* DESCRIPTION:
*       This routine writes all ports PTP configuration parameters.
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
MAD_STATUS mdPtpSetConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_PTP_CONFIG    *ptpData
)
{
  MAD_STATUS           retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* mdPtpGetConfig
*
* DESCRIPTION:
*       This routine reads default port PTP configuration parameters.
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
MAD_STATUS mdPtpGetConfig
(
  IN  MAD_DEV     *dev,
  OUT MAD_PTP_CONFIG    *ptpData
)
{
  MAD_STATUS           retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}

/*******************************************************************************
* mdPtpSetGlobalConfig
*
* DESCRIPTION:
*       This routine writes all ports PTP global configuration parameters.
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
MAD_STATUS mdPtpSetGlobalConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_PTP_GLOBAL_CONFIG    *ptpData
)
{
  MAD_STATUS           retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetGlobalConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* mdPtpGetGlobalConfig
*
* DESCRIPTION:
*       This routine reads default port PTP global configuration parameters.
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
MAD_STATUS mdPtpGetGlobalConfig
(
  IN  MAD_DEV     *dev,
  OUT MAD_PTP_GLOBAL_CONFIG    *ptpData
)
{
  MAD_STATUS           retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetGlobalConfig_port(dev, port, ptpData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}

/*******************************************************************************
* mdPtpSetTsMode
*
* DESCRIPTION:
*       This routine set PTP arrive 0 TS mode on all ports.
*
* INPUTS:
*       None.
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
MAD_STATUS mdPtpSetTsMode
(
    IN  MAD_DEV     *dev,
    IN  MAD_PTP_TS_MODE  tsMode 
)
{
	MAD_STATUS      retVal;
	MAD_LPORT		port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpSetTsMode_port(dev, port, tsMode);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpGetTsMode
*
* DESCRIPTION:
*       This routine get PTP arrive 0 TS mode on a default port.
*
* INPUTS:
*       None.
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
MAD_STATUS mdPtpGetTsMode
(
    IN  MAD_DEV     *dev,
    OUT MAD_PTP_TS_MODE  *tsMode 
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpGetTsMode_port(dev, port, tsMode);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpSetMeanPathDelay
*
* DESCRIPTION:
*		This routine sets ingress mean path delay on all ports.
* 
* INPUTS:
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
MAD_STATUS mdPtpSetMeanPathDelay
(
	IN   MAD_DEV	*dev,
    IN   MAD_U16	delay
)
 {
	MAD_STATUS      retVal;
	MAD_LPORT		port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpSetMeanPathDelay_port(dev, port, delay);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpGetMeanPathDelay
*
* DESCRIPTION:
*		This routine gets ingress mean path delay on a default port.
* 
* INPUTS:
*		None.
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
MAD_STATUS mdPtpGetMeanPathDelay
(
	IN   MAD_DEV	*dev,
    OUT  MAD_U16	*delay 
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpGetMeanPathDelay_port(dev, port, delay);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpSetIngPathDelayAsym
*
* DESCRIPTION:
*		This routine sets ingress path delay asymmetry on all ports.
* 
* INPUTS:
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
MAD_STATUS mdPtpSetIngPathDelayAsym
(
	IN   MAD_DEV	*dev,
    IN   MAD_U16	asym
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpSetIngPathDelayAsym_port(dev, port, asym);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpGetIngPathDelayAsym
*
* DESCRIPTION:
*		This routine gets ingress path delay asymmetry on a default port.
* 
* INPUTS:
*		None.
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
MAD_STATUS mdPtpGetIngPathDelayAsym
(
	IN   MAD_DEV	*dev,
    OUT  MAD_U16	*asym
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpGetIngPathDelayAsym_port(dev, port, asym);
		if(retVal!=MAD_OK)
		 return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpSetEgPathDelayAsym
*
* DESCRIPTION:
*		This routine sets egress path delay asymmetry on all ports.
* 
* INPUTS:
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
MAD_STATUS mdPtpSetEgPathDelayAsym
(
	IN   MAD_DEV	*dev,
    IN   MAD_U16	asym
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpSetEgPathDelayAsym_port(dev, port, asym);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpGetEgPathDelayAsym
*
* DESCRIPTION:
*		This routine gets egress path delay asymmetry on a default port.
* 
* INPUTS:
*		None.
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
MAD_STATUS mdPtpGetEgPathDelayAsym
(
	IN   MAD_DEV	*dev,
    OUT  MAD_U16	*asym
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpGetEgPathDelayAsym_port(dev, port, asym);
		if(retVal!=MAD_OK)
		 return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpSetPTPEn
*
* DESCRIPTION:
*       This routine enables or disables all ports PTP.
*
* INPUTS:
*       en - MAD_TRUE to enable PTP, MAD_FALSE to disable PTP
*
* OUTPUTS:
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
MAD_STATUS mdPtpSetPTPEn
(
  IN  MAD_DEV   *dev,
  IN  MAD_BOOL          en
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpSetPTPEn_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* mdPtpGetPTPEn
*
* DESCRIPTION:
*       This routine checks if default port PTP is enabled.
*
* INPUTS:
*       None
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
MAD_STATUS mdPtpGetPTPEn
(
  IN  MAD_DEV   *dev,
  OUT MAD_BOOL          *en
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetPTPEn_port(dev, port, en);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}

/*******************************************************************************
* mdPtpGetPTPInt
*
* DESCRIPTION:
*       This routine gets default port PTP interrupt status for each port.
*    The PTP Interrupt bit gets set for a given port when an incoming PTP 
*    frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*    Similary PTP Interrupt bit gets set for a given port when an outgoing
*    PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*    This bit gets cleared upon software reading and clearing the corresponding
*    time counter valid bits that are valid for that port.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*    ptpInt     - interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
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
MAD_STATUS mdPtpGetPTPInt
(
  IN  MAD_DEV     *dev,
  OUT MAD_U32        *ptpInt
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetPTPInt_port(dev, port, ptpInt);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetPTPIntStatus
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for a specific port.
*
* INPUTS:
*       None.
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
MAD_STATUS mdPtpGetPTPIntStatus
(
    IN  MAD_DEV						*dev,
    OUT MAD_PTP_INTERRUPT_STATUS	*ptpIntSt
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetPTPIntStatus_port(dev, port, ptpIntSt);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* mdPtpGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the default port global timer value that is running off of the free
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
MAD_STATUS mdPtpGetPTPGlobalTime
(
  IN  MAD_DEV     *dev,
  OUT MAD_U32        *ptpTime
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetPTPGlobalTime_port(dev, port, ptpTime);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetTSDiscardCount
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Departure frame discard counter 
*        for PTP frames that need hardware time stamping.
*
* INPUTS:
*		 None.
*
* OUTPUTS:
*        discardCount    - discard counters
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
MAD_STATUS mdPtpGetTSDiscardCount
(
    IN  MAD_DEV						*dev,
    OUT MAD_PTP_TS_DISCARD_COUNTER	*discardCount
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetTSDiscardCount_port(dev, port, discardCount);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpGetOutFCSErr
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Output CRC Error counter.
*
* INPUTS:
*       None
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
MAD_STATUS mdPtpGetOutFCSErr
(
    IN  MAD_DEV     *dev,
    OUT MAD_U8      *outFcsErr
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = mdPtpGetOutFCSErr_port(dev, port, outFcsErr);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiSetEventConfig
*
* DESCRIPTION:
*       This routine sets all ports TAI Event Capture configuration parameters.
*
* INPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* OUTPUTS:
*       port - The logical port number
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
MAD_STATUS madTaiSetEventConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_TAI_EVENT_CONFIG    *eventData
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetEventConfig_port(dev, port, eventData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventConfig
*
* DESCRIPTION:
*       This routine gets default port TAI Event Capture configuration parameters.
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
MAD_STATUS madTaiGetEventConfig
(
  IN  MAD_DEV     *dev,
  OUT MAD_TAI_EVENT_CONFIG    *eventData
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetEventConfig_port(dev, port, eventData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventStatus
*
* DESCRIPTION:
*       This routine gets default port TAI Event Capture status.
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
MAD_STATUS madTaiGetEventStatus
(
  IN  MAD_DEV     *dev,
  OUT MAD_TAI_EVENT_STATUS    *status
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetEventStatus_port(dev, port, status);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetEventInt
*
* DESCRIPTION:
*       This routine gets default port TAI Event Capture Interrupt status.
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
MAD_STATUS madTaiGetEventInt
(
  IN  MAD_DEV     *dev,
  OUT MAD_BOOL    *intStatus
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetEventInt_port(dev, port, intStatus);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiClearEventInt
*
* DESCRIPTION:
*       This routine clear TAI Event Capture Interrupt status.
*
* INPUTS:
*       None.
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
MAD_STATUS madTaiClearEventInt
(
    IN  MAD_DEV			*dev
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiClearEventInt_port(dev, port);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiSetClockSelect
*
* DESCRIPTION:
*       This routine sets several clock select in TAI.
*
* INPUTS:
*        clkSelect  - TAI clock select configuration parameters.
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
MAD_STATUS madTaiSetClockSelect
(
    IN  MAD_DEV					*dev,
    IN  MAD_TAI_CLOCK_SELECT    *clkSelect
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetClockSelect_port(dev, port, clkSelect);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiGetClockSelect
*
* DESCRIPTION:
*       This routine gets several clock select in TAI.
*
* INPUTS:
*       None.
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
MAD_STATUS madTaiGetClockSelect
(
    IN   MAD_DEV				*dev,
    OUT  MAD_TAI_CLOCK_SELECT   *clkSelect
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetClockSelect_port(dev, port, clkSelect);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiSetClockConfig
*
* DESCRIPTION:
*       This routine sets clock configuration in TAI.
*
* INPUTS:
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
MAD_STATUS madTaiSetClockConfig
(
    IN  MAD_DEV					*dev,
    IN  MAD_TAI_CLOCK_CONFIG	*clkConfig
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetClockConfig_port(dev, port, clkConfig);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiGetClockConfig
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
MAD_STATUS madTaiGetClockConfig
(
    IN  MAD_DEV					*dev,
    OUT MAD_TAI_CLOCK_CONFIG    *clkConfig
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetClockConfig_port(dev, port, clkConfig);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiGetTrigInt
*
* DESCRIPTION:
*       This routine gets default port TAI Trigger Interrupt status.
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
MAD_STATUS madTaiGetTrigInt
(
  IN  MAD_DEV     *dev,
  OUT MAD_BOOL    *intStatus
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTrigInt_port(dev, port, intStatus);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiClearTrigInt
*
* DESCRIPTION:
*       This routine clears TAI Trigger Interrupt status.
*
* INPUTS:
*       None.
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
MAD_STATUS madTaiClearTrigInt
(
    IN  MAD_DEV       *dev
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiClearTrigInt_port(dev, port);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiSetTrigConfig
*
* DESCRIPTION:
*       This routine sets all ports TAI Trigger configuration parameters.
*
* INPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is MAD_TRUE).
*
* OUTPUTS:
*       port - The logical port number
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
MAD_STATUS madTaiSetTrigConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_BOOL     trigEn,
  IN  MAD_TAI_TRIGGER_CONFIG    *trigData
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTrigConfig_port(dev, port, trigEn, trigData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiGetTrigConfig
*
* DESCRIPTION:
*       This routine gets default port TAI Trigger configuration parameters.
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
MAD_STATUS madTaiGetTrigConfig
(
  IN  MAD_DEV     *dev,
  OUT MAD_BOOL     *trigEn,
  OUT MAD_TAI_TRIGGER_CONFIG    *trigData
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTrigConfig_port(dev, port, trigEn, trigData);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiSetTrigLock
*
* DESCRIPTION:
*       This routine sets TAI Trigger lock.
*
* INPUTS:
*       trigLockCfg - trig lock configuration:
*        trigLock       - trigger lock set.
*        trigLockRange  - trigger lock range.
*        lockCorrect    -  Trig Lock Correction amount 
*        lockCorrect2    - Trig Lock Correction amount 2
*        trigGenTime     - Trigger Generation Time. 
*        trigGen2Time    - Trigger Generation Time 2. 
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
MAD_STATUS madTaiSetTrigLock
(
    IN  MAD_DEV							*dev,
    INOUT  MAD_TAI_TRIG_LOCK_CONFIG		*trigLockCfg
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTrigLock_port(dev, port, trigLockCfg);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiGetTrigLock
*
* DESCRIPTION:
*       This routine gets TAI Trigger lock and trigger lock range.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       trigLockCfg - trig lock configuration:
*        trigLock       - trigger lock set.
*        trigLockRange  - trigger lock range.
*        lockCorrect    -  Trig Lock Correction amount 
*        lockCorrect2    - Trig Lock Correction amount 2
*        trigGenTime     - Trigger Generation Time. 
*        trigGen2Time    - Trigger Generation Time 2. 
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
MAD_STATUS madTaiGetTrigLock
(
    IN		MAD_DEV						*dev,
    INOUT   MAD_TAI_TRIG_LOCK_CONFIG    *trigLockCfg
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTrigLock_port(dev, port, trigLockCfg);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* madTaiGetTSClkPer
*
* DESCRIPTION:
*      default port Time Stamping Clock Period in pico seconds.
*    This routine specifies the clock period for the time stamping clock supplied 
*    to the PTP hardware logic.
*    This is the clock that is used by the hardware logic to update the PTP 
*    Global Time Counter.
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
MAD_STATUS madTaiGetTSClkPer
(
  IN  MAD_DEV     *dev,
  OUT MAD_U32        *clk
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTSClkPer_port(dev, port, clk);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetTSClkPer
*
* DESCRIPTION:
*     all ports Time Stamping Clock Period in pico seconds.
*    This routine specifies the clock period for the time stamping clock supplied 
*    to the PTP hardware logic.
*    This is the clock that is used by the hardware logic to update the PTP 
*    Global Time Counter.
*
* INPUTS:
*    clk        - time stamping clock period
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*     MAD_OK      - on success
*     MAD_[ERROR_STATUS]    - on error
*     MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     None
*
*******************************************************************************/
MAD_STATUS madTaiSetTSClkPer
(
  IN  MAD_DEV     *dev,
  IN  MAD_U32        clk
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTSClkPer_port(dev, port, clk);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetMultiPTPSync
*
* DESCRIPTION:
*     This routine sets all ports Multiple PTP device sync mode and sync time (TrigGenAmt).
*    When enabled, the hardware logic detects a low to high transition on the 
*    EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*    register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*    multiEn        - enable/disable Multiple PTP device sync mode
*    syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* OUTPUTS:
*    None.
*
* RETURNS:
*    MAD_OK      - on success
*    MAD_[ERROR_STATUS]    - on error
*    MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*    and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiSetMultiPTPSync
(
  IN  MAD_DEV     *dev,
  IN  MAD_BOOL     multiEn,
  IN  MAD_32        syncTime
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetMultiPTPSync_port(dev, port, multiEn, syncTime);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetMultiPTPSync
*
* DESCRIPTION:
*    This routine sets default port Multiple PTP device sync mode and sync time (TrigGenAmt).
*    When enabled, the hardware logic detects a low to high transition on the 
*    EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*    register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*       port - The logical port number
*
* OUTPUTS:
*    multiEn        - enable/disable Multiple PTP device sync mode
*    syncTime    - sync time (valid only if multiEn is MAD_TRUE)
*
* RETURNS:
*    MAD_OK      - on success
*    MAD_[ERROR_STATUS]    - on error
*    MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     When enabled, madTaiSetTrigConfig, madTaiSetEventConfig, madTaiSetTimeInc,
*    and madTaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
MAD_STATUS madTaiGetMultiPTPSync
(
  IN  MAD_DEV     *dev,
  OUT MAD_BOOL     *multiEn,
  OUT MAD_32        *syncTime
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetMultiPTPSync_port(dev, port, multiEn, syncTime);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;

}


/*******************************************************************************
* madTaiGetTimeIncDec
*
* DESCRIPTION:
*    This routine retrieves default port Time increment/decrement setup.
*    This amount specifies the number of units of PTP Global Time that need to be 
*    incremented or decremented. This is used for adjusting the PTP Global Time 
*    counter value by a certain amount.
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
MAD_STATUS madTaiGetTimeIncDec
(
  IN  MAD_DEV		*dev,
  OUT MAD_BOOL		*expired,
  OUT MAD_BOOL		*inc,
  OUT MAD_U32		*amount
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  port = madPtpDefaultPort;
  {
    retVal = madTaiGetTimeIncDec_port(dev, port, expired, inc, amount);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeInc
*
* DESCRIPTION:
*     This routine enables all ports time increment by the specifed time increment amount.
*    The amount specifies the number of units of PTP Global Time that need to be 
*    incremented. This is used for adjusting the PTP Global Time counter value by
*    a certain amount.
*    Increment occurs just once.
*
* INPUTS:
*    amount    - time increment amount (0 ~ 0x7FF)
*
* OUTPUTS:
*       port - The logical port number
*
* RETURNS:
*     MAD_OK      - on success
*     MAD_[ERROR_STATUS]    - on error
*     MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeInc
(
  IN  MAD_DEV     *dev,
  IN  MAD_U32     amount
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTimeInc_port(dev, port, amount);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}


/*******************************************************************************
* madTaiSetTimeDec
*
* DESCRIPTION:
*     This routine enables all ports time decrement by the specifed time decrement amount.
*    The amount specifies the number of units of PTP Global Time that need to be 
*    decremented. This is used for adjusting the PTP Global Time counter value by
*    a certain amount.
*    Decrement occurs just once.
*
* INPUTS:
*       port - The logical port number
*    amount    - time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     MAD_OK      - on success
*     MAD_[ERROR_STATUS]    - on error
*     MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     None
*
*******************************************************************************/
MAD_STATUS madTaiSetTimeDec
(
  IN  MAD_DEV     *dev,
  IN  MAD_U32        amount
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = madTaiSetTimeDec_port(dev, port, amount);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpModeSet
*
* DESCRIPTION:
*        Set PTP_GLOBAL_CONFIG_PTP_MODE_REG from PTP point/data registers. 
*
* INPUTS:
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
MAD_STATUS mdPtpModeSet
(
	IN  MAD_DEV				*dev,
    IN  MAD_PTP_MODE_CONFIG	*mode
)
{
  MAD_STATUS   retVal;
  MAD_LPORT    port;
    
  for (port=0; port<dev->numOfPorts; port++)
  {
    retVal = mdPtpModeSet_port(dev, port, mode);
    if(retVal!=MAD_OK)
      return retVal;
  }
  return MAD_OK;
}

/*******************************************************************************
* mdPtpModeGet
*
* DESCRIPTION:
*        Get PTP_GLOBAL_CONFIG_PTP_MODE_REG from PTP point/data registers. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtpModeGet
(
	IN  MAD_DEV				*dev,
    OUT MAD_PTP_MODE_CONFIG	*mode
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpModeGet_port(dev, port, mode);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpSetExtTimeFormat
*
* DESCRIPTION:
*        Set External Time Format. 
*
* INPUTS:
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
MAD_STATUS mdPtpSetExtTimeFormat
(
	IN  MAD_DEV						*dev,
    IN  MAD_PTP_EXTERNAL_TIME_FORM  *format
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpSetExtTimeFormat_port(dev, port, format);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpGetExtTimeFormat
*
* DESCRIPTION:
*        Get External Time Format. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtpGetExtTimeFormat
(
	IN  MAD_DEV						*dev,
    OUT MAD_PTP_EXTERNAL_TIME_FORM  *format
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpGetExtTimeFormat_port(dev, port, format);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpSetIngActVec
*
* DESCRIPTION:
*        Set IEEE 1588 or IEEE 802.1AS Ingress Action Vectors. 
*
* INPUTS:
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
MAD_STATUS mdPtpSetIngActVec
(
    IN  MAD_DEV			*dev,
	IN  MAD_PTP_SPEC	spec,
    IN  MAD_U32			vec
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpSetIngActVec_port(dev, port, spec, vec);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpGetIngActVec
*
* DESCRIPTION:
*        Get IEEE 1588 or IEEE 802.1AS Ingress Action Vectors. 
*
* INPUTS:
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
MAD_STATUS mdPtpGetIngActVec
(
    IN  MAD_DEV			*dev,
	IN  MAD_PTP_SPEC	spec,
    OUT MAD_U32			*vec
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpGetIngActVec_port(dev, port, spec, vec);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpSetEgActVec
*
* DESCRIPTION:
*        Set IEEE_1588 or IEEE 802.1AS Egress Action Vectors. 
*
* INPUTS:
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
MAD_STATUS mdPtpSetEgActVec
(
    IN  MAD_DEV				*dev,
	IN  MAD_PTP_SPEC		spec,
    IN  MAD_PTP_EG_ACT_VEC	vec
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpSetEgActVec_port(dev, port, spec, vec);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpGetEgActVec
*
* DESCRIPTION:
*        Get IEEE_1588 or IEEE 802.1AS Egress Action Vectors. 
*
* INPUTS:
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
MAD_STATUS mdPtpGetEgActVec
(
    IN  MAD_DEV				*dev,
	IN  MAD_PTP_SPEC		spec,
    OUT MAD_PTP_EG_ACT_VEC	*vec
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpGetEgActVec_port(dev, port, spec, vec);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODLoadPointSet
*
* DESCRIPTION:
*        Set Time of Day Load Point Register. 
*
* INPUTS:
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
MAD_STATUS mdPtpTODLoadPointSet
(
    IN  MAD_DEV		*dev,
    IN  MAD_U32     loadData
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpTODLoadPointSet_port(dev, port, loadData);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODLoadPointGet
*
* DESCRIPTION:
*        Get Time of Day Load Point Register. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtpTODLoadPointGet
(
    IN  MAD_DEV       *dev,
    OUT MAD_U32       *loadData
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpTODLoadPointGet_port(dev, port, loadData);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODOpCtrlSet
*
* DESCRIPTION:
*        Set ToD operation control. 
*
* INPUTS:
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
MAD_STATUS mdPtpTODOpCtrlSet
(
    IN  MAD_DEV				*dev,
    IN  MAD_PTP_TOD_OP_CTRL	*todOpCtrl
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpTODOpCtrlSet_port(dev, port, todOpCtrl);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODOpCtrlGet
*
* DESCRIPTION:
*        Get ToD operation control. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtpTODOpCtrlGet
(
    IN   MAD_DEV				*dev,
    OUT  MAD_PTP_TOD_OP_CTRL	*todOpCtrl
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpTODOpCtrlGet_port(dev, port, todOpCtrl);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpPulseSet
*
* DESCRIPTION:
*        Set PTP Pulse parameters. 
*
* INPUTS:
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
MAD_STATUS mdPtpPulseSet
(
    IN  MAD_DEV					*dev,
    IN  MAD_PTP_PULSE_STRUCT    *pulsePara
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpPulseSet_port(dev, port, pulsePara);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpPulseGet
*
* DESCRIPTION:
*        Get PTP Pulse parameters. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtpPulseGet
(
    IN   MAD_DEV				*dev,
    OUT  MAD_PTP_PULSE_STRUCT   *pulsePara
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpPulseGet_port(dev, port, pulsePara);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODTimeSet
*
* DESCRIPTION:
*        Set Time Array Time of Day,. 
*
* INPUTS:
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
MAD_STATUS mdPtpTODTimeSet
(
    IN  MAD_DEV				*dev,
    IN  MAD_PTP_TS_STRUCT	*ts
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpTODTimeSet_port(dev, port, ts);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODTimeGet
*
* DESCRIPTION:
*        Get Time Array Time of Day,. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtpTODTimeGet
(
    IN  MAD_DEV				*dev,
    OUT MAD_PTP_TS_STRUCT    *ts
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpTODTimeGet_port(dev, port, ts);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtp1722TimeSet
*
* DESCRIPTION:
*        Set Time Array 1722 Time of Day. 
*
* INPUTS:
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
MAD_STATUS mdPtp1722TimeSet
(
    IN  MAD_DEV				*dev,
    IN  unsigned long long   ts
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtp1722TimeSet_port(dev, port, ts);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtp1722TimeGet
*
* DESCRIPTION:
*        Get Time Array 1722 Time of Day. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtp1722TimeGet
(
    IN  MAD_DEV				*dev,
    OUT unsigned long long  *ts
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtp1722TimeGet_port(dev, port, ts);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODCompSet
*
* DESCRIPTION:
*        Set PTP time array time of day compensation. 
*
* INPUTS:
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
MAD_STATUS mdPtpTODCompSet
(
    IN  MAD_DEV		*dev,
    IN  MAD_U32		comp
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	for (port=0; port<dev->numOfPorts; port++)
	{
		retVal = mdPtpTODCompSet_port(dev, port, comp);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}

/*******************************************************************************
* mdPtpTODCompGet
*
* DESCRIPTION:
*        Get PTP time array time of day compensation. 
*
* INPUTS:
*        None.
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
MAD_STATUS mdPtpTODCompGet
(
    IN   MAD_DEV		*dev,
    OUT  MAD_U32		*comp
)
{
	MAD_STATUS   retVal;
	MAD_LPORT    port;
    
	port = madPtpDefaultPort;
	{
		retVal = mdPtpTODCompGet_port(dev, port, comp);
		if(retVal!=MAD_OK)
			return retVal;
	}
	return MAD_OK;
}




/*  1-step PTP support */
#if 0
/*******************************************************************************
* madNPtpSetClockInput
*
* DESCRIPTION:
*     This routine select PTP clock input.
*
* INPUTS:
*    port - The logical port number
*    clkIn    - Ptp clock input
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     MAD_OK      - on success
*     MAD_[ERROR_STATUS]    - on error
*     MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     None
*
*******************************************************************************/
MAD_STATUS madNPtpSetClockInput
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U16      clkin
)
{
  MAD_STATUS           retVal;
  MAD_U8          hwPort;         /* the physical port number     */
  MAD_U16    page;
  MAD_U8     reg;
  MAD_U16    data;     

    
    MAD_DBG_INFO((("madNPtpSetClockInput Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_1STEP_PTP_CAPABLE) )
    {
        MAD_DBG_ERROR(("PTP 1step not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	page = 20;
	reg = 6;
    data = clkIn;
    /* Set the required bits for SGMII Output Amplitude */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,page,reg,
                    8,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }


  return MAD_OK;
}

/*******************************************************************************
* madNPtpGetClockInput
*
* DESCRIPTION:
*     This routine read PTP clock input.
*
* INPUTS:
*    port - The logical port number
*
* OUTPUTS:
*    clkIn    - Ptp clock input
*
* RETURNS:
*     MAD_OK      - on success
*     MAD_[ERROR_STATUS]    - on error
*     MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     None
*
*******************************************************************************/
MAD_STATUS madNPtpGetClockInput
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U16     *clkin
)
{
  MAD_STATUS           retVal;
  MAD_U8          hwPort;         /* the physical port number     */
  MAD_U16    page;
  MAD_U8     reg;
  MAD_U16    data;     

    
    MAD_DBG_INFO((("madNPtpGetClockInput Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_1STEP_PTP_CAPABLE) )
    {
        MAD_DBG_ERROR(("PTP 1step not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	page = 20;
	reg = 6;
    /* Get the required bits for SGMII Output Amplitude */
    if((retVal = madHwGetPagedPhyRegField(
                    dev,hwPort,page,reg,
                    8,1,&data)) != MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }

    *clkIn = data;

  return MAD_OK;
}

/*******************************************************************************
* madNPtpSetEventCapValid
*
* DESCRIPTION:
*     This routine select PTP clock input.
*
* INPUTS:
*    port - The logical port number
*    clkIn    - Ptp clock input
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     MAD_OK      - on success
*     MAD_[ERROR_STATUS]    - on error
*     MAD_API_PTP_NOT_SUPPORT - if current device does not support this feature.
*
* COMMENTS:
*     None
*
*******************************************************************************/
MAD_STATUS madNPtpSetEventCapValid
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT    port,
  IN  MAD_U16      clkin
)
{
  MAD_STATUS           retVal;
  MAD_U8          hwPort;         /* the physical port number     */
  MAD_U16    page;
  MAD_U8     reg;
  MAD_U16    data;     

    
    MAD_DBG_INFO((("madNPtpSetEventCapValid Called.\n"));
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    /* check if device supports this feature */
    if( !MAD_DEV_CAPABILITY1(dev, MAD_PHY_1STEP_PTP_CAPABLE) )
    {
        MAD_DBG_ERROR(("PTP 1step not supported.\n"));
        return MAD_API_PTP_NOT_SUPPORT;
    }

	page = 20;
	reg = 6;
    data = clkIn;
    /* Set the required bits for SGMII Output Amplitude */
    if((retVal = madHwSetPagedPhyRegField(
                    dev,hwPort,page,reg,
                    8,1,data)) != MAD_OK)
    {
        MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
        return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
    }


  return MAD_OK;
}
#endif
