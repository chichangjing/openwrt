#include <madCopyright.h>

/********************************************************************************
* madWol.c
* 
* DESCRIPTION:
*       APIs of WOL(Wake On Line) to control and get WOL status.
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
#include <madDrvConfig.h>

/*******************************************************************************
* madWolEnable
*
* DESCRIPTION:
*       This routine enables or disables WOL.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       wolType  - Type of WOL: combination of 
*                  MAD_PHY_WOL_TYPE_PKT   
*                  MAD_PHY_WOL_TYPE_MAGIC   
*                  MAD_PHY_WOL_TYPE_LINKUP 
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15  : Soft reset
*       Page 17, Reg 16, Bit 13-15 : Enable WOL
*
*******************************************************************************/
MAD_STATUS madWolEnable
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_U16     wolType
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO(("madWolEnable called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Set WOL Control Register */
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_CONTROL,13,3,wolType)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}

/*******************************************************************************
* madWolGetEnable
*
* DESCRIPTION:
*       This routine gets if WOL is enabled.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       wolType  - Type of WOL: combination of 
*                  MAD_PHY_WOL_TYPE_PKT   
*                  MAD_PHY_WOL_TYPE_MAGIC   
*                  MAD_PHY_WOL_TYPE_LINKUP 
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15  : Soft reset
*       Page 17, Reg 16, Bit 13-15 : Enable WOL
*
*******************************************************************************/
MAD_STATUS madWolGetEnable
(
  IN   MAD_DEV     *dev,
  IN   MAD_LPORT   port,
  OUT  MAD_U16     *wolType
)
{
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO(("madWolGetEnable called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }


    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
        MAD_DBG_ERROR(("Copper not supported.\n"));
        return MAD_API_COPPER_NOT_SUPPORT;
    }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Get WOL Control Register */
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_CONTROL,13,3,wolType)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Reading to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }

    return MAD_OK;
}

/*******************************************************************************
* madWolGetStatus
*
* DESCRIPTION:
*       This routine retrieves the status of WOL.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       wolType  - Type of WOL: combination of 
*                  MAD_PHY_WOL_TYPE_PKT   
*                  MAD_PHY_WOL_TYPE_MAGIC   
*                  MAD_PHY_WOL_TYPE_LINKUP 
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STATUS] - on error
*
* COMMENTS:
*       Page 0, Reg 0, Bit 15  : Soft reset
*       Page 17, Reg 17, Bit 13-15 : status of WOL
*
*******************************************************************************/
MAD_STATUS madWolGetStatus
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT  MAD_U16     *wolType
)
{
    MAD_STATUS  retVal;
    MAD_U8      hwPort;

    MAD_DBG_INFO(("madWolGetStatus called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MAD_LPORT_2_PORT(port);
    if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
    {
      MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
      return MAD_API_ERROR_OVER_PORT_RANGE;
    }

    if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
    {
        MAD_DBG_ERROR(("Copper not supported.\n"));
        return MAD_API_COPPER_NOT_SUPPORT;
    }

  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Get WOL Control Register */
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_STATUS,13,3,wolType)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Reading to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }

    return MAD_OK;
}


static MAD_STATUS madWolSramWrite
(
  IN  MAD_DEV     *dev,
  IN  MAD_U8      port,
  IN  MAD_U8      byteAddr,
  IN  MAD_U8      rawAddr,
  IN  MAD_U8      mask,
  IN  MAD_U8      value
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;

  data = ((mask&0x1)<<8) | (value&0xff);
  if((retVal=madHwWritePagedPhyReg(dev,port,MAD_PAGE17,MAD_REG_WOL_SRAM_BYTE_DATA_CTRL,data)) != MAD_OK)
  {
    MAD_DBG_ERROR(("SMI Paged Write failed (port%i, MAD_REG_WOL_SRAM_BYTE_DATA_CTRL)\n",port));
    return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
  }

  data = ((byteAddr&0x7)<<7) | (rawAddr&0x7f);
  data |= ((1)<<10);
  if((retVal=madHwWritePagedPhyReg(dev,port,MAD_PAGE17,MAD_REG_WOL_SRAM_BYTE_ADDR_CTRL,data)) != MAD_OK)
  {
    MAD_DBG_ERROR(("SMI Paged Write failed (port%i, MAD_REG_WOL_SRAM_BYTE_ADDR_CTRL)\n",port));
    return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
  }
  return MAD_OK;
}

static MAD_STATUS madWolSramRead
(
  IN  MAD_DEV     *dev,
  IN  MAD_U8      port,
  IN  MAD_U8      byteAddr,
  IN  MAD_U8      rawAddr,
  OUT  MAD_U8     * mask,
  OUT  MAD_U8     * value
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;

  /* clear read bit first */
  data = ((byteAddr&0x7)<<7) | (rawAddr&0x7f);
  if((retVal=madHwWritePagedPhyReg(dev,port,MAD_PAGE17,MAD_REG_WOL_SRAM_BYTE_ADDR_CTRL,data)) != MAD_OK)
  {
    MAD_DBG_ERROR(("SMI Paged Write failed (port%i, MAD_REG_WOL_SRAM_BYTE_ADDR_CTRL)\n",port));
    return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
  }
  data = ((1)<<11) | ((byteAddr&0x7)<<7) | (rawAddr&0x7f);
  if((retVal=madHwWritePagedPhyReg(dev,port,MAD_PAGE17,MAD_REG_WOL_SRAM_BYTE_ADDR_CTRL,data)) != MAD_OK)
  {
    MAD_DBG_ERROR(("SMI Paged Write failed (port%i, MAD_REG_WOL_SRAM_BYTE_ADDR_CTRL)\n",port));
    return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
  }

  if((retVal=madHwReadPagedPhyReg(dev,port,MAD_PAGE17,MAD_REG_WOL_SRAM_READ_BYTE_DATA,&data)) != MAD_OK)
  {
    MAD_DBG_ERROR(("SMI Paged Read failed (port%i, MAD_REG_WOL_SRAM_READ_BYTE_DATA)\n",port));
    return (retVal | MAD_DRV_FAIL_READ_PAGED_REG);
  }
  *mask = (data>>8)&1;
  *value = data&0xff;

  return MAD_OK;
}

/*******************************************************************************
* madSetWolPktConfig
*
* DESCRIPTION:
*       This sets configuration for WOL Sram Packet match (frame event).
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       pktConfig  - MAD_WOL_PKT_MATCH_STRUCT
*                     sramPktNum   - Sram packet number(0-7);
*                     enable       - Enable;
*                     matchStatus  - Real time enable status;
*                     sramPktLenth - Sram Packet Match length;;
*                     sramPacket[128] - Sram packet data: 128 X {enable,byte}
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 17, Reg 16, Bit 0 - 7 : Enable sram packet WOL
*       Page 17, Reg 17, Bit 0 - 7 : status of matching sram packet WOL 
*       Page 17, Reg 18-21 : match length of sram packet WOL 
*       Page 17, Reg 26-28 : access of sram packet WOL 
*
*******************************************************************************/
MAD_STATUS madSetWolPktConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_WOL_PKT_MATCH_STRUCT     *pktConfig
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  int         sramPktNum;
  MAD_U8      reg, offset;
  int i;

  MAD_DBG_INFO(("madSetWolPktConfig called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Set sram packet to enable */
  sramPktNum = pktConfig->sramPktNum;
  data = pktConfig->enable;
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_CONTROL,sramPktNum,1,data)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  /* Set sram packet match length */
  sramPktNum = pktConfig->sramPktNum;
  reg = sramPktNum/2;
  offset = (pktConfig->sramPktNum%2)?7:0;
  data = pktConfig->sramPktLength;
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_SRAM_PKT_10_LEN-reg,offset,7,data)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  /* Set sram packet match data */
  for(i=0; i<128; i++)
  {
    if((retVal = madWolSramWrite(dev, hwPort, sramPktNum, i, pktConfig->sramPacket[i].enable, pktConfig->sramPacket[i].byte)) != MAD_OK)
    {
      MAD_DBG_ERROR(("madWolSramWrite failed.\n"));
      return (retVal);
    }
  }

  return MAD_OK;
}

/*******************************************************************************
* madGetWolPktConfig
*
* DESCRIPTION:
*       This gets configuration for WOL Sram Packet match (frame event).
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       pktConfig  - MAD_WOL_PKT_MATCH_STRUCT
*                     sramPktNum   - Sram packet number(0-7);
*                     enable       - Enable;
*                     matchStatus  - Real time enable status;
*                     sramPktLenth - Sram Packet Match length;;
*                     sramPacket[128] - Sram packet data: 128 X {enable,byte}
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 17, Reg 26-28 : access of sram packet WOL 
*
*******************************************************************************/
MAD_STATUS madGetWolPktConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT  MAD_WOL_PKT_MATCH_STRUCT     *pktConfig
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  int         sramPktNum;
  MAD_U8      reg, offset;
  int i;

  MAD_DBG_INFO(("madGetWolPktConfig called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Get sram packet enable */
  sramPktNum = pktConfig->sramPktNum;
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_CONTROL,sramPktNum,1,&data)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }
  pktConfig->enable = data;

  /* Get sram packet status */
  sramPktNum = pktConfig->sramPktNum;
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_STATUS,sramPktNum,1,&data)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }
  pktConfig->matchStatus = data;

  /* Get sram packet match length */
  sramPktNum = pktConfig->sramPktNum;
  reg = sramPktNum/2;
  offset = (pktConfig->sramPktNum%2)?7:0;
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_SRAM_PKT_10_LEN-reg,offset,7,&data)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Reading paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }
  pktConfig->sramPktLength = madShort2Char(data);

  /* Get sram packet match data */
  for(i=0; i<128; i++)
  {
    if((retVal = madWolSramRead(dev, hwPort, sramPktNum, i, &(pktConfig->sramPacket[i].enable), &(pktConfig->sramPacket[i].byte))) != MAD_OK)
    {
      MAD_DBG_ERROR(("madWolSramRead failed.\n"));
      return (retVal);
    }
  }

  return MAD_OK;
}

/*******************************************************************************
* madSetWolMagicPktConfig
*
* DESCRIPTION:
*       This sets configuration for WOL Magic Packet match .
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       destAddr  - Destination address matched.
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 17, Reg 23-25 : destination address of magic packet WOL 
*
*******************************************************************************/
MAD_STATUS madSetWolMagicPktConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_U8     *destAddr
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  int i;

  MAD_DBG_INFO(("madSetWolMagicPktConfig called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Set magic packet destination address */
  for (i=0; i<3; i++)
  {
    data = (destAddr[i*2]<<8) | destAddr[i*2+1];
    if((retVal=madHwWritePagedPhyReg(dev,port,MAD_PAGE17,MAD_REG_WOL_MAGIC_PKT_DST_ADDR_2+i,data)) != MAD_OK)
	{
      MAD_DBG_ERROR(("SMI Paged Write failed (port%i, MAD_REG_WOL_MAGIC_PKT_DST_ADDR_2)\n",port));
      return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
	}
  }

  return MAD_OK;
}

/*******************************************************************************
* madGetWolMagicPktConfig
*
* DESCRIPTION:
*       This gets configuration for WOL Magic Packet match .
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       destAddr  - Destination address matched.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 17, Reg 23-25 : destination address of magic packet WOL 
*
*******************************************************************************/
MAD_STATUS madGetWolMagicPktConfig
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT  MAD_U8     *destAddr
)
{
  MAD_U16     data;
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  int i;

  MAD_DBG_INFO(("madGetWolMagicPktConfig called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Set magic packet destination address */
  for (i=0; i<3; i++)
  {
    if((retVal=madHwReadPagedPhyReg(dev,port,MAD_PAGE17,MAD_REG_WOL_MAGIC_PKT_DST_ADDR_2+i,&data)) != MAD_OK)
	{
      MAD_DBG_ERROR(("SMI Paged Read failed (port%i, MAD_REG_WOL_MAGIC_PKT_DST_ADDR_2)\n",port));
      return (retVal | MAD_DRV_FAIL_WRITE_PAGED_REG);
	}
    destAddr[i*2] = data>>8;
    destAddr[i*2+1] = data&0xff;
  }

  return MAD_OK;
}

/*******************************************************************************
* madWolClearStatus
*
* DESCRIPTION:
*       This routine clear status of WOL.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 17, Reg 16, Bit 12 : Clear status WOL
*       Page 17, Reg 17, Bit 0-15 : Cleared status WOL
*
*******************************************************************************/
MAD_STATUS madWolClearStatus
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;

  MAD_DBG_INFO(("madWolClearStatus called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Set status clear bit in WOL Control Register */
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_CONTROL,12,1,1)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}


/*******************************************************************************
* madWol10BLowPwSel
*
* DESCRIPTION:
*       This routine selects 10BT low power of WOL.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 17, Reg 16, Bit 8 : 10BT low power select
*
*******************************************************************************/
MAD_STATUS madWol10BLowPwSel
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  IN  MAD_BOOL    en
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16     data;

  MAD_DBG_INFO(("madWol10BLowPwSel called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  MAD_BOOL_2_BIT(en,data);
  /* Set 10BT low power bit in WOL Control Register */
  if((retVal = madHwSetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_CONTROL,8,1,data)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Writing to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_SET_PAGED_REG_FIELD);
  }

  return MAD_OK;
}

/*******************************************************************************
* madWol10BLowPwGet
*
* DESCRIPTION:
*       This routine gets selection of 10BT low power of WOL.
*
* INPUTS:
*       dev  - pointer to MAD driver structure returned from mdLoadDriver
*       port - The logical port number
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*       en   - MAD_TRUE if enabled, MAD_FALSE otherwise
*   
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK   - on success
*       MAD_[ERROR_STAUE] - on error
*
* COMMENTS:
*       Page 17, Reg 16, Bit 8 : 10BT low power select
*
*******************************************************************************/
MAD_STATUS madWol10BLowPwGet
(
  IN  MAD_DEV     *dev,
  IN  MAD_LPORT   port,
  OUT MAD_BOOL    *en
)
{
  MAD_STATUS  retVal;
  MAD_U8      hwPort;
  MAD_U16     data;

  MAD_DBG_INFO(("madWol10BLowPwGet called.\n"));

  /* translate LPORT to hardware port */
  hwPort = MAD_LPORT_2_PORT(port);
  if(!(MAD_DEV_PORT_CHECK(dev, hwPort)))
  {
    MAD_DBG_INFO(("The port %d is over range.\n", (int)port));
    return MAD_API_ERROR_OVER_PORT_RANGE;
  }

  if(!MAD_DEV_HWMODE(dev, hwPort, MAD_PHY_MODE_COPPER))
  {
    MAD_DBG_ERROR(("Copper not supported.\n"));
    return MAD_API_COPPER_NOT_SUPPORT;
  }
  
  if(!MAD_DEV_CAPABILITY1(dev, MAD_PHY_WOL))
  {
    MAD_DBG_ERROR(("WOL not supported.\n"));
    return MAD_API_WOL_NOT_SUPPORT;
  }

  /* Get 10BT low power bit in WOL Control Register */
  if((retVal = madHwGetPagedPhyRegField(dev,hwPort,MAD_PAGE17,MAD_REG_WOL_CONTROL,8,1,&data)) != MAD_OK)
  {
     MAD_DBG_ERROR(("Reading to paged phy reg failed.\n"));
     return (retVal | MAD_API_FAIL_GET_PAGED_REG_FIELD);
  }
  MAD_BIT_2_BOOL(data,*en);

  return MAD_OK;
}


