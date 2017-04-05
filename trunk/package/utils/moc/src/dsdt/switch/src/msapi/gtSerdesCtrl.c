#include <Copyright.h>

/********************************************************************************
* gtSerdesCtrl.h
* 
* DESCRIPTION:
* API definitions for Phy Serdes control facility.
* All input argument port in serdes API is phy port, except for some indicated functions. 
* The phy port is mapped into related serdes port in the functions.
*
* DEPENDENCIES:
* None.
*
* FILE REVISION NUMBER:
* $Revision: 10 $
*******************************************************************************/

#include <msApi.h>
#include <gtHwCntl.h>
#include <gtDrvConfig.h>
#include <gtDrvSwRegs.h>
#include <gtVct.h>
#include <gtSem.h>

/*
 * This routine sets Auto Mode and Reset the phy
*/
static GT_U8 _getSerdesPageNumber(GT_QD_DEV *dev)
{
    switch (dev->deviceId)
    {
        case GT_88E6172:
        case GT_88E6176:
        case GT_88E6240:
        case GT_88E6352:
        case GT_88E6320:
        case GT_88E6321:
        case GT_88E6999:
		case GT_88E6390:
	    case GT_88E6390X:
		case GT_88E6360:
		case GT_88E6290:
		case GT_88E6290A:
		case GT_88E6191:
		case GT_88E6190:
		case GT_88E6190X:
            return 1;
            break;
        default:
            return 0;
    }

  return 0;
}

static 
GT_STATUS _serdesSetAutoMode
(
    IN GT_QD_DEV *dev,
    IN GT_U8 serdesPort,
    IN GT_PHY_INFO     *serdesInfo,
    IN GT_PHY_AUTO_MODE mode
)
{
      GT_U16          u16Data;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);

    DBG_INFO(("_serdesSetAutoMode Called.\n"));

    if(hwReadPagedPhyReg(dev,serdesPort,pageNum,QD_SERDES_AUTONEGO_AD_REG,serdesInfo->anyPage,&u16Data) != GT_OK)
    {
        DBG_INFO(("Not able to read Serdes Reg(port:%d,offset:%d).\n",serdesPort,QD_PHY_AUTONEGO_AD_REG));
           return GT_FAIL;
    }

    /* Mask out all auto mode related bits. */
    u16Data &= ~QD_PHY_MODE_AUTO_AUTO;

    switch(mode)
    {
        case SPEED_AUTO_DUPLEX_AUTO:
                u16Data |= QD_PHY_MODE_AUTO_AUTO;
                break;
        case SPEED_AUTO_DUPLEX_FULL:
                u16Data  |= QD_PHY_MODE_AUTO_FULL;
                break;
        case SPEED_AUTO_DUPLEX_HALF:
                u16Data  |= QD_PHY_MODE_AUTO_HALF;
                break;
        case SPEED_100_DUPLEX_AUTO:
                u16Data |= QD_PHY_MODE_100_AUTO;
                break;
        case SPEED_10_DUPLEX_AUTO:
                u16Data |= QD_PHY_MODE_10_AUTO;
                break;
        case SPEED_100_DUPLEX_FULL:
                u16Data |= QD_PHY_100_FULL;
                break;
        case SPEED_100_DUPLEX_HALF:
                u16Data |= QD_PHY_100_HALF;
                break;
        case SPEED_10_DUPLEX_FULL:
                u16Data |= QD_PHY_10_FULL;
                break;
        case SPEED_10_DUPLEX_HALF:
                u16Data |= QD_PHY_10_HALF;
                break;
        default:
                DBG_INFO(("Unknown Auto Mode (%d)\n",mode));
                return GT_BAD_PARAM;
    }

     /* Write to Phy AutoNegotiation Advertisement Register.  */
    if(hwWritePagedPhyReg(dev,serdesPort,pageNum, QD_SERDES_AUTONEGO_AD_REG,serdesInfo->anyPage,u16Data) != GT_OK)
    {
        DBG_INFO(("Not able to write Serdes Reg(port:%d,offset:%d,data:%#x).\n",serdesPort,QD_PHY_AUTONEGO_AD_REG,u16Data));
           return GT_FAIL;
    }


    return GT_OK;
}

static 
GT_STATUS serdesSetAutoMode
(
    IN GT_QD_DEV *dev,
    IN GT_U8 hwPort,
    IN GT_PHY_INFO *serdesInfo,
    IN GT_PHY_AUTO_MODE mode
)
{
    GT_U16       u16Data;
    GT_STATUS    status;
	GT_U8        serdesPort;
	GT_U8        pageNum = _getSerdesPageNumber(dev);

    DBG_INFO(("serdesSetAutoMode Called.\n"));

	serdesPort = hwPort;

    {
        if((status=_serdesSetAutoMode(dev,serdesPort,serdesInfo,mode)) != GT_OK)
        {
               return status;
        }
        /* Read to Serdes Control Register.  */
        if(hwReadPagedPhyReg(dev,serdesPort,pageNum, QD_SERDES_CONTROL_REG,serdesInfo->anyPage,&u16Data) != GT_OK)
            return GT_FAIL;

        u16Data |= QD_SERDES_AUTONEGO;

        DBG_INFO(("Write to phy(%d) register: regAddr 0x%x, data %#x",
                  serdesPort,QD_SERDES_CONTROL_REG,u16Data));

        /* Write to Serdes Control Register.  */
        if(hwWritePagedPhyReg(dev,serdesPort,pageNum, QD_SERDES_CONTROL_REG,serdesInfo->anyPage,u16Data) != GT_OK)
            return GT_FAIL;
    }

    return hwSerdesReset(dev,serdesPort,pageNum, 0xFF);
}



/*******************************************************************************
* gprtGetSerdesMode
*
* DESCRIPTION:
*       This routine reads Serdes Interface Mode.
*
* INPUTS:
*        port -  The physical SERDES device address(4/5)
*
* OUTPUTS:
*       mode    - Serdes Interface Mode
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       logical port number is supported only for the devices made production 
*       before 2009. 
*  (Serdes devices: 88E6131, 88E6122, 88E6108, 88E6161, 88E6165, 88E6352 and 88E320 family)
*
*******************************************************************************/
GT_STATUS gprtGetSerdesMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_SERDES_MODE *mode
)
{
    GT_U16          u16Data;           /* The register's read data.    */
    GT_U8           hwPort,  serdesPort;         /* the physical port number     */
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;


    DBG_INFO(("gprtGetSerdesMode Called.\n"));

    if(!IS_IN_DEV_GROUP(dev,DEV_SERDES_CORE))
    {
        return GT_NOT_SUPPORTED;
    }


	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);

	hwPort = serdesPort;


    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    /* Get Serdes Register. */
    if(hwReadPagedPhyReg(dev,serdesPort,pageNum, 16,serdesInfo.anyPage,&u16Data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    *mode = u16Data&0x3;

    gtSemGive(dev,dev->phyRegsSem);
    return GT_OK;
}


/*******************************************************************************
* gprtSetSerdesMode
*
* DESCRIPTION:
*       This routine sets Serdes Interface Mode.
*
* INPUTS:
*       port -  The physical SERDES device address(4/5)
*       mode    - Serdes Interface Mode
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       logical port number is supported only for the devices made production 
*       before 2009. 
*  (Serdes devices: 88E6131, 88E6122, 88E6108, 88E6161, 88E6165 family)
*
*******************************************************************************/
GT_STATUS gprtSetSerdesMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_SERDES_MODE mode
)
{
    GT_U16          u16Data;       
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_STATUS    retVal;
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtSetSerdesMode Called.\n"));

    if(!IS_IN_DEV_GROUP(dev,DEV_SERDES_CORE))
    {
        return GT_NOT_SUPPORTED;
    }
    if(IS_IN_DEV_GROUP(dev,DEV_SERDES_CORE_1))
    {
        return GT_NOT_SUPPORTED;
    }

	serdesPort = port;

    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    u16Data = mode;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    /* Set Serdes Register. */
    if(hwSetPagedPhyRegField(dev,serdesPort, pageNum, 16,0,2,serdesInfo.anyPage,u16Data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    gtSemGive(dev,dev->phyRegsSem);
    retVal = hwSerdesReset(dev,serdesPort,pageNum, 0xFF);
    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}

/* Set SERDES Register. */
/*******************************************************************************
* gprtSetSerdesReg
*
* DESCRIPTION:
*       This routine writes Phy Serdes Registers.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_U32            regAddr,
    IN  GT_U16            data
)
{
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
	GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;


    DBG_INFO(("gprtSetSerdesReg Called.\n"));

    if(!IS_IN_DEV_GROUP(dev,DEV_SERDES_UP_PORT))
    {
        return GT_NOT_SUPPORTED;
    }

	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    /* Write to Serdes Register */
    if(hwWritePagedPhyReg(dev,serdesPort,pageNum, (GT_U8)regAddr,serdesInfo.anyPage,data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    gtSemGive(dev,dev->phyRegsSem);
    return GT_OK;
}

/* Get SERDES Register. */
/*******************************************************************************
* gprtGetSerdesReg
*
* DESCRIPTION:
*       This routine reads Phy Serdes Registers.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The read register's data.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_U32         regAddr,
    OUT GT_U16         *data
)
{
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
	GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtGetSerdesReg Called.\n"));

    if(!IS_IN_DEV_GROUP(dev,DEV_SERDES_UP_PORT))
    {
        return GT_NOT_SUPPORTED;
    }

	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    if(hwPort > dev->maxPhyNum)
    {
        /* check if input is physical serdes address */    
        if(dev->validSerdesVec & (1<<port))
        {
            hwPort = (GT_U8)port;
        }
        else
            return GT_NOT_SUPPORTED;
    }

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    /* Get Serdes Register. */
    /* Write to Serdes Register */
    if(hwReadPagedPhyReg(dev,serdesPort,pageNum,(GT_U8)regAddr,serdesInfo.anyPage,data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    gtSemGive(dev,dev->phyRegsSem);
    return GT_OK;

}

/*******************************************************************************
* gprtSetSerdesRegField
*
* DESCRIPTION:
*		This routine sets Serdes Register.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.	
*
* OUTPUTS:
*		 None.
*
* RETURNS:
*		GT_OK			- on success
*		GT_FAIL 		- on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesRegField
(
IN GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    IN  GT_U8    regAddr,
    IN  GT_U8    fieldOffset,
    IN  GT_U8    fieldLength,
    IN  GT_U16   data
)
{
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
	GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;
		
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


		
    if(hwSetPagedPhyRegField(dev,serdesPort, pageNum,regAddr,fieldOffset,fieldLength,serdesInfo.anyPage,data) != GT_OK)
    	{
     	   DBG_INFO(("Failed.\n"));
           return GT_FAIL;
    	}
	return GT_OK;
}
/* Get SERDES Register. */
/*******************************************************************************
* gprtGetSerdesRegField
*
* DESCRIPTION:
*		This routine gets Serdes Register.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       regAddr     - The register's address.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data        - Data to be read.	
*
* RETURNS:
*		GT_OK			- on success
*		GT_FAIL 		- on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesRegField
(
IN GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    IN  GT_U8    regAddr,
    IN  GT_U8    fieldOffset,
    IN  GT_U8    fieldLength,
    OUT  GT_U16   *data
)
{
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;

	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


		
    if(hwGetPagedPhyRegField(dev,serdesPort,pageNum,regAddr,fieldOffset,fieldLength,serdesInfo.anyPage,data) != GT_OK)
    	{
     	   DBG_INFO(("Failed.\n"));
           return GT_FAIL;
    	}
		return GT_OK;
}

/*******************************************************************************
* gprtSetSerdesIntEnable
*
* DESCRIPTION:
*       This routine set Serdes interrupt enable.
*
* INPUTS:
*       port -    The logical port number.
*       data    - Interrupt bits.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtSetSerdesIntEnable 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    IN  GT_U16         data
)
{
    DBG_INFO(("gprtSetSerdesIntEnable Called.\n"));

    if(!IS_IN_DEV_GROUP(dev,DEV_SERDES_UP_PORT))
    {
        return GT_NOT_SUPPORTED;
    }

    /* Set SERDES Register 18. Enable all initerrupts */
    return (gprtSetSerdesRegField(dev, port, 18, 0, 16, data) );
}

/*******************************************************************************
* gprtGetSerdesIntEnable
*
* DESCRIPTION:
*       This routine get Serdes interrupt enable setting.
*
* INPUTS:
*       port -    The logical port number.
*
* OUTPUTS:
*       data    - Interrupt bits.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesIntEnable 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT  GT_U16         *data
)
{
    DBG_INFO(("gprtGetSerdesIntEnable Called.\n"));

    if(!IS_IN_DEV_GROUP(dev,DEV_SERDES_UP_PORT))
    {
        return GT_NOT_SUPPORTED;
    }

    /* Set SERDES Register 18. Enable all initerrupts */
    return (gprtGetSerdesRegField(dev, port, 18, 0, 16, data) );
}

/*******************************************************************************
* gprtGetSerdesIntStatus
*
* DESCRIPTION:
*       This routine get Serdes interrupt status and clear the status.
*
* INPUTS:
*       port -    The logical port number.
*       regAddr - The register's address.
*
* OUTPUTS:
*       data    - The interrupt status.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesIntStatus 
(
    IN  GT_QD_DEV        *dev,
    IN  GT_LPORT        port,
    OUT  GT_U16          *data
)
{
    DBG_INFO(("gprtGetSerdesIntStatus Called.\n"));

    if(!IS_IN_DEV_GROUP(dev,DEV_SERDES_UP_PORT))
    {
        return GT_NOT_SUPPORTED;
    }

    /* Get and Reset SERDES Register 19, initerrupt status */
    return (gprtGetSerdesRegField(dev, port, 19, 0, 16, data) );
}


/*******************************************************************************
* gprtSerdesReset
*
* DESCRIPTION:
*       This routine preforms SERDES reset.
*        After reset, serdes will be in Autonegotiation mode.
*
* INPUTS:
* port - The logical port number, unless SERDES device is accessed
*        The physical address, if SERDES device is accessed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
* COMMENTS:
* data sheet register 0.15 - Reset
* data sheet register 0.13 - Speed
* data sheet register 0.12 - Autonegotiation
* data sheet register 0.8  - Duplex Mode
*******************************************************************************/

GT_STATUS gprtSerdesReset
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port
)
{

    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtSerdesReset Called.\n"));
    
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);

    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown SERDES device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    /* set Auto Negotiation AD Register */
    retVal = serdesSetAutoMode(dev,hwPort,&serdesInfo,SPEED_AUTO_DUPLEX_AUTO);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    gtSemGive(dev,dev->phyRegsSem);

    return retVal;
}

/*******************************************************************************
* gprtSetSerdesLoopback
*
* DESCRIPTION:
* Enable/Disable Internal Serdes Loopback. 
*
* INPUTS:
* port - The logical port number, unless SERDES device is accessed
*        The physical address, if SERDES device is accessed
* enable - If GT_TRUE, enable loopback mode
* If GT_FALSE, disable loopback mode
*
* OUTPUTS:
* None.
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* data sheet register 0.14 - Loop_back
*
*******************************************************************************/

GT_STATUS gprtSetSerdesLoopback
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL   enable
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
    GT_PHY_INFO        serdesInfo;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);

    DBG_INFO(("gprtSetSerdesLoopback Called.\n"));
    

	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);

    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown SERDES device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    BOOL_2_BIT(enable,u16Data);

    /* Write to Phy Control Register.  */
    retVal = hwSetPagedPhyRegField(dev,serdesPort, pageNum,QD_SERDES_CONTROL_REG,14,1,serdesInfo.anyPage,u16Data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}

/*******************************************************************************
* gprtGetSerdesLoopback
*
* DESCRIPTION:
* Gets status of Enable/Disable Internal Serdes Loopback. 
*
* INPUTS:
* port - The logical port number, unless SERDES device is accessed
*        The physical address, if SERDES device is accessed
*
* OUTPUTS:
* enable - If GT_TRUE, enable loopback mode
* If GT_FALSE, disable loopback mode
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* data sheet register 0.14 - Loop_back
*
*******************************************************************************/

GT_STATUS gprtGetSerdesLoopback
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_BOOL   *enable
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
    GT_PHY_INFO        serdesInfo;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);

    DBG_INFO(("gprtGetSerdesLoopback Called.\n"));
    
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);

    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown SERDES device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    /* Read to Serdes Control Register.  */
    retVal = hwGetPagedPhyRegField(dev,serdesPort, pageNum,QD_SERDES_CONTROL_REG,14,1,serdesInfo.anyPage,&u16Data);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }

    BIT_2_BOOL(u16Data, *enable);

    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}


/*******************************************************************************
* gprtSetSerdesSpeed
*
* DESCRIPTION:
*         Sets speed for a specific logical port. This function will keep the duplex 
*        mode and loopback mode to the previous value, but disable others, such as 
*        Autonegotiation.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*        speed - port speed.
*                PHY_SPEED_10_MBPS for 10Mbps
*                PHY_SPEED_100_MBPS for 100Mbps
*                PHY_SPEED_1000_MBPS for 1000Mbps
*
* OUTPUTS:
* None.
*
* RETURNS:
* GT_OK - on success
* GT_FAIL - on error
*
* COMMENTS:
* data sheet register 0.13 - Speed Selection (LSB)
* data sheet register 0.6  - Speed Selection (MSB)
*
*******************************************************************************/

GT_STATUS gprtSetSerdesSpeed
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_PHY_SPEED speed
)
{
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
    GT_PHY_INFO        serdesInfo;
    GT_STATUS        retVal;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);

    DBG_INFO(("gprtSetSerdesSpeed Called.\n"));
    
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);

    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown SERDES device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    if(hwReadPagedPhyReg(dev,serdesPort,pageNum,QD_SERDES_CONTROL_REG,serdesInfo.anyPage,&u16Data) != GT_OK)
    {
        DBG_INFO(("Not able to read Serdes Reg(port:%d,offset:%d).\n",serdesPort,QD_SERDES_CONTROL_REG));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    switch(speed)
    {
        case PHY_SPEED_10_MBPS:
            if ((serdesInfo.flag & GT_PHY_GIGABIT) && !(serdesInfo.flag & GT_PHY_COPPER))
            {
                gtSemGive(dev,dev->phyRegsSem);
                return GT_BAD_PARAM;
            }
            u16Data = u16Data & (QD_SERDES_LOOPBACK | QD_SERDES_DUPLEX);
            break;
        case PHY_SPEED_100_MBPS:
            u16Data = (u16Data & (QD_SERDES_LOOPBACK | QD_SERDES_DUPLEX)) | QD_SERDES_SPEED;
            break;
        case PHY_SPEED_1000_MBPS:
            if (!(serdesInfo.flag & GT_PHY_GIGABIT))
            {
                gtSemGive(dev,dev->phyRegsSem);
                return GT_BAD_PARAM;
            }
            u16Data = (u16Data & (QD_SERDES_LOOPBACK | QD_SERDES_DUPLEX)) | QD_SERDES_SPEED_MSB;
            break;
        default:
            gtSemGive(dev,dev->phyRegsSem);
            return GT_FAIL;
    }

    DBG_INFO(("Write to serdes(%d) register: regAddr 0x%x, data %#x",
              serdesPort,QD_SERDES_CONTROL_REG,u16Data));

    retVal = hwSerdesReset(dev,serdesPort,pageNum, u16Data);
    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}


/*******************************************************************************
* gprtSerdesAutoNegEnable
*
* DESCRIPTION:
*         Enable/disable an Auto-Negotiation.
*        This routine simply sets Auto Negotiation bit (bit 12) of Control 
*        Register and reset the serdes.
*        For Speed and Duplex selection, please use gprtSetSerdesAutoMode.
*
* INPUTS:
*        port -  The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*         state - GT_TRUE for enable Auto-Negotiation,
*                GT_FALSE otherwise
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.12 - Auto-Negotiation Enable
*         data sheet register 4.8, 4.7, 4.6, 4.5 - Auto-Negotiation Advertisement
*
*******************************************************************************/
GT_STATUS gprtSerdesAutoNegEnable
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL   state
)
{
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
    GT_STATUS        retVal;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtSerdesAutoNegEnable Called.\n"));
    
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    /* check if the port is configurable */
    if(!IS_CONFIGURABLE_PHY(dev,hwPort))
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(hwReadPagedPhyReg(dev,serdesPort,pageNum,QD_SERDES_CONTROL_REG,serdesInfo.anyPage,&u16Data) != GT_OK)
    {
        DBG_INFO(("Not able to read Serdes Reg(port:%d,offset:%d).\n",serdesPort,QD_SERDES_CONTROL_REG));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    if(state)
    {
        u16Data = (u16Data & (QD_SERDES_SPEED | QD_SERDES_DUPLEX)) | QD_SERDES_AUTONEGO;
    }
    else
    {
        u16Data = u16Data & (QD_SERDES_SPEED | QD_SERDES_DUPLEX);
    }


    DBG_INFO(("Write to serdes(%d) register: regAddr 0x%x, data %#x",
              serdesPort,QD_SERDES_CONTROL_REG,u16Data));

    retVal = hwSerdesReset(dev,serdesPort, pageNum, u16Data);
    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}

/*******************************************************************************
* gprtSerdesPowerDown
*
* DESCRIPTION:
*        Enable/disable (power down) on specific logical port.
*        Phy configuration remains unchanged after Power down.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*        state -    GT_TRUE: power down
*                 GT_FALSE: normal operation
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.11 - Power Down
*
*******************************************************************************/

GT_STATUS gprtSerdesPowerDown
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_BOOL   state
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtSerdesPowerDown Called.\n"));
    
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    /* check if the port is configurable */
    if(!IS_CONFIGURABLE_PHY(dev,hwPort))
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    BOOL_2_BIT(state,u16Data);

    if((retVal=hwSetPagedPhyRegField(dev,serdesPort, pageNum,QD_SERDES_CONTROL_REG,11,1,serdesInfo.anyPage,u16Data)) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return retVal;
    }

    gtSemGive(dev,dev->phyRegsSem);
    return GT_OK;
}

/*******************************************************************************
* gprtGetSerdesPowerDownSt
*
* DESCRIPTION:
*        Gets status of Enable/disable (power down) on specific logical port.
*        Phy configuration remains unchanged after Power down.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*
* OUTPUTS:
*        state -    GT_TRUE: power down
*                 GT_FALSE: normal operation
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.11 - Power Down
*
*******************************************************************************/

GT_STATUS gprtGetSerdesPowerDownSt
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
OUT GT_BOOL  *state
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtGetSerdesPowerDownSt Called.\n"));
    
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    /* check if the port is configurable */
    if(!IS_CONFIGURABLE_PHY(dev,hwPort))
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }


    if((retVal=hwGetPagedPhyRegField(dev,serdesPort, pageNum,QD_SERDES_CONTROL_REG,11,1,serdesInfo.anyPage,&u16Data)) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return retVal;
    }
    BIT_2_BOOL(u16Data, *state);

    gtSemGive(dev,dev->phyRegsSem);
    return GT_OK;
}

/*******************************************************************************
* gprtSerdesRestartAutoNeg
*
* DESCRIPTION:
*         Restart AutoNegotiation. If AutoNegotiation is not enabled, it'll enable 
*        it. Loopback and Power Down will be disabled by this routine.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.9 - Restart Auto-Negotiation
*
*******************************************************************************/

GT_STATUS gprtSerdesRestartAutoNeg
( 
IN GT_QD_DEV *dev,
IN GT_LPORT  port
)
{
    GT_STATUS       retVal;      
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtSerdesRestartAutoNeg Called.\n"));
  
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    /* check if the port is configurable */
    if(!IS_CONFIGURABLE_PHY(dev,hwPort))
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(hwReadPagedPhyReg(dev,serdesPort,pageNum,QD_SERDES_CONTROL_REG,serdesInfo.anyPage,&u16Data) != GT_OK)
    {
        DBG_INFO(("Not able to read Serdes Reg(port:%d,offset:%d).\n",serdesPort,QD_SERDES_CONTROL_REG));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    u16Data &= (QD_SERDES_DUPLEX | QD_SERDES_SPEED);
    u16Data |= (QD_SERDES_RESTART_AUTONEGO | QD_SERDES_AUTONEGO);

    DBG_INFO(("Write to serdes(%d) register: regAddr 0x%x, data %#x",
              serdesPort,QD_SERDES_CONTROL_REG,u16Data));

    /* Write to Phy Control Register.  */
    retVal = hwWritePagedPhyReg(dev,serdesPort,pageNum, QD_SERDES_CONTROL_REG,serdesInfo.anyPage,u16Data);
    gtSemGive(dev,dev->phyRegsSem);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    return retVal;
}

/*******************************************************************************
* gprtSetSerdesDuplexMode
*
* DESCRIPTION:
*         Sets duplex mode for a specific logical port. This function will keep 
*        the speed and loopback mode to the previous value, but disable others,
*        such as Autonegotiation.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*         dMode    - dulpex mode
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK     - on success
*         GT_FAIL     - on error
*
* COMMENTS:
*         data sheet register 0.8 - Duplex Mode
*
*******************************************************************************/
GT_STATUS gprtSetSerdesDuplexMode
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_BOOL   dMode
)
{
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
    GT_STATUS        retVal;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtSetSerdesDuplexMode Called.\n"));
    
	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    /* check if the port is configurable */
    if(!IS_CONFIGURABLE_PHY(dev,hwPort))
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(hwReadPagedPhyReg(dev,serdesPort,pageNum,QD_SERDES_CONTROL_REG,serdesInfo.anyPage,&u16Data) != GT_OK)
    {
        DBG_INFO(("Not able to read Serdes Reg(port:%d,offset:%d).\n",serdesPort,QD_SERDES_CONTROL_REG));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    if(dMode)
    {
        u16Data = (u16Data & (QD_SERDES_LOOPBACK | QD_SERDES_SPEED | QD_SERDES_SPEED_MSB)) | QD_SERDES_DUPLEX;
    }
    else
    {
        u16Data = u16Data & (QD_SERDES_LOOPBACK | QD_SERDES_SPEED | QD_SERDES_SPEED_MSB);
    }


    DBG_INFO(("Write to serdes(%d) register: regAddr 0x%x, data %#x",
              serdesPort,QD_SERDES_CONTROL_REG,u16Data));

    /* Write to Phy Control Register.  */
    retVal = hwSerdesReset(dev,serdesPort,pageNum, u16Data);
    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}


/*******************************************************************************
* gprtSetSerdesAutoMode
*
* DESCRIPTION:
*         This routine sets up the port with given Auto Mode.
*        Supported mode is as follows:
*        - Auto for both speed and duplex.
*        - Auto for speed only and Full duplex.
*        - Auto for speed only and Half duplex.
*        - Auto for duplex only and speed 1000Mbps.
*        - Auto for duplex only and speed 100Mbps.
*        - Auto for duplex only and speed 10Mbps.
*        - Speed 1000Mbps and Full duplex.
*        - Speed 1000Mbps and Half duplex.
*        - Speed 100Mbps and Full duplex.
*        - Speed 100Mbps and Half duplex.
*        - Speed 10Mbps and Full duplex.
*        - Speed 10Mbps and Half duplex.
*        
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*         mode - Auto Mode to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - on device without copper
*
* COMMENTS:
*         data sheet register 4.8, 4.7, 4.6, and 4.5 Autonegotiation Advertisement
*         data sheet register 4.6, 4.5 Autonegotiation Advertisement for 1000BX
*         data sheet register 9.9, 9.8 Autonegotiation Advertisement for 1000BT
*******************************************************************************/

GT_STATUS gprtSetSerdesAutoMode
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_PHY_AUTO_MODE mode
)
{

    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_PHY_INFO        serdesInfo;

    DBG_INFO(("gprtSetSerdesAutoMode Called.\n"));

	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    retVal = GT_NOT_SUPPORTED;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);

    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown SERDES device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    retVal = serdesSetAutoMode(dev,hwPort,&serdesInfo,mode);

    gtSemGive(dev,dev->phyRegsSem);
    return retVal;

}


/*******************************************************************************
* gprtSetSerdesPause
*
* DESCRIPTION:
*       This routine will set the pause bit in Autonegotiation Advertisement
*        Register. And restart the autonegotiation.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*        state - GT_SERDES_PAUSE_MODE enum value.
*                GT_SERDES_NO_PAUSE        - disable pause
*                 GT_SERDES_PAUSE        - support pause
*                GT_SERDES_ASYMMETRIC_PAUSE    - support asymmetric pause
*                GT_SERDES_BOTH_PAUSE    - support both pause and asymmetric pause
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
* COMMENTS:
* data sheet register 4. Autonegotiation Advertisement Register
*******************************************************************************/

GT_STATUS gprtSetSerdesPause
(
IN GT_QD_DEV *dev,
IN GT_LPORT  port,
IN GT_PHY_PAUSE_MODE state
)
{
  GT_U8           hwPort, serdesPort;         /* the physical port number     */
  GT_U16             u16Data,regStart;
  GT_STATUS        retVal = GT_OK;
  GT_PHY_INFO        serdesInfo;
  GT_U8           pageNum = _getSerdesPageNumber(dev);

  GT_SERDES_MODE  serdes_mode;

    DBG_INFO(("serdesSetPause Called.\n"));

	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

	if((retVal=gprtGetSerdesMode(dev,port, &serdes_mode)) != GT_OK)
   {
      DBG_INFO(("Not able to get Serdes mode(port:%d).\n",serdes_mode));
      gtSemGive(dev,dev->phyRegsSem);
      return GT_FAIL;
   }
   if(serdes_mode != PHY_SERDES_1000X)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    regStart = 7;

    if(state & GT_PHY_ASYMMETRIC_PAUSE)
    {
        if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
        {
            DBG_INFO(("Unknown SERDES device.\n"));
            gtSemGive(dev,dev->phyRegsSem);
            return GT_FAIL;
        }

        if (!(serdesInfo.flag & GT_PHY_GIGABIT))
        {
            DBG_INFO(("Not Supported\n"));
            gtSemGive(dev,dev->phyRegsSem);
            return GT_BAD_PARAM;
        }

    }

    u16Data = state;

    /* Write to Phy AutoNegotiation Advertisement Register.  */
    if((retVal=hwSetPagedPhyRegField(dev,serdesPort, pageNum,QD_SERDES_AUTONEGO_AD_REG,(GT_U8)regStart,2,serdesInfo.anyPage,u16Data)) != GT_OK)
    {
        DBG_INFO(("Not able to write Serdes Reg(port:%d,offset:%d).\n",serdesPort,QD_SERDES_AUTONEGO_AD_REG));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    /* Restart Auto Negotiation */
    if((retVal=hwSetPagedPhyRegField(dev,serdesPort, pageNum,QD_SERDES_CONTROL_REG,9,1,serdesInfo.anyPage,1)) != GT_OK)
    {
        DBG_INFO(("Not able to write Serdes Reg(port:%d,offset:%d,data:%#x).\n",serdesPort,QD_SERDES_AUTONEGO_AD_REG,serdesInfo.anyPage,u16Data));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }

    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}

/*******************************************************************************
* gprtGetSerdesLinkStatus
*
* DESCRIPTION:
*       This routine retrieves the Link status.
*
* INPUTS:
*        port -    The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*
* OUTPUTS:
*       linkStatus - GT_FALSE if link is not established,
*                     GT_TRUE if link is established.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*        
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gprtGetSerdesLinkStatus
(
    IN GT_QD_DEV *dev,
    IN GT_LPORT  port,
    IN GT_BOOL      *linkStatus
)
{

    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort, serdesPort;         /* the physical port number     */
    GT_U16             u16Data;
    GT_PHY_INFO        serdesInfo;
	  GT_U8           pageNum = _getSerdesPageNumber(dev);

    DBG_INFO(("gprtGetSerdesLinkStatus Called.\n"));

	serdesPort = port;
    GT_GET_SERDES_PORT(dev,&serdesPort);
	hwPort = serdesPort;

    gtSemTake(dev,dev->phyRegsSem,OS_WAIT_FOREVER);
    /* check if the port is configurable */
    if((serdesInfo.phyId=GT_GET_PHY_ID(dev,hwPort)) == GT_INVALID_PHY)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return GT_NOT_SUPPORTED;
    }

    if(driverFindPhyInformation(dev,hwPort,&serdesInfo) != GT_OK)
    {
        DBG_INFO(("Unknown PHY device.\n"));
        gtSemGive(dev,dev->phyRegsSem);
        return GT_FAIL;
    }


    if((retVal=hwGetPagedPhyRegField(dev,serdesPort,pageNum, 17,10,1,serdesInfo.anyPage,&u16Data)) != GT_OK)
    {
        gtSemGive(dev,dev->phyRegsSem);
        return retVal;
    }

    BIT_2_BOOL(u16Data,*linkStatus);

    gtSemGive(dev,dev->phyRegsSem);
    return retVal;
}


