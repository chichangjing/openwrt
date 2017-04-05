#include <Copyright.h>

/********************************************************************************
* gtPortPrioMap.c
*
* DESCRIPTION:
*       API definitions for port IP Priority Mapping control
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: $
*******************************************************************************/

#include <msApi.h>
#include <gtSem.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>

/*******************************************************************************
* gprtSetIpPrioMap
*
* DESCRIPTION:
*        This API set port IP Queue and Frame Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        mapLoc  - Mapping table location.  DiffServ in recrived IP frame 
*        mapData - Mapping data.
*              ipYellow;   When the UseIpYellow bit is set to a one (Port offset 0x19) 
*				 this bit is used to indicate the color of frames. When the UseIpYellow 
*				 bit is cleared to a zero (Port offset 0x19) this bit is used 
*				 to indicate these frames are AVB stream frames.
*              disIpQpri;  
*              ipQpri;   
*              disIpFpri;   
*              ipFpri;   
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetIpPrioMap
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U8       loc,
    IN  GT_PRIORITY_MAP_DATA    *mapdata
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U16        data;
    GT_U16        ptr;
    GT_U8           hwPort;         /* the physical port number     */
    int count=0x10;

    DBG_INFO(("gprtSetIpPrioMap Called.\n"));

    hwPort = GT_LPORT_2_PORT(port);
    if (hwPort >= dev->numOfPorts)
        return GT_BAD_PARAM;

    /* Check if Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_IP_PRI_MAPPING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Table is ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, hwPort, PORT_ACCESS);
      regAccess.rw_reg_list[0].reg = QD_REG_IP_PRI_MAPPING;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
      }
    }
#else
    do    
    {
        retVal = hwGetPortRegField(dev,hwPort,QD_REG_IP_PRI_MAPPING,15,1,&data);
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) return GT_FAIL;

    } while(data == 1);
#endif

	ptr = loc&0x3f; 
	data = ((mapdata->ipYellow&1)<<8)|((mapdata->disIpQpri&1)<<7)|((mapdata->ipQpri&7)<<4)|
		((mapdata->disIpFpri&1)<<3)|((mapdata->ipFpri&7)<<0);

	data |= (ptr<<9);

    data = data | 0x8000;

    retVal = hwWritePortReg(dev, hwPort, QD_REG_IP_PRI_MAPPING, data);
    if(retVal != GT_OK)
      {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
    }


    gtSemGive(dev,dev->tblRegsSem);

    return GT_OK;
}


/*******************************************************************************
* gprtGetIpPrioMap
*
* DESCRIPTION:
*        This API get port IP Queue and Frame Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        mapLoc  - Mapping table location.  DiffServ in recrived IP frame 
*
* OUTPUTS:
*        mapData - Mapping data.
*              ipYellow;   When the UseIpYellow bit is set to a one (Port offset 0x19) 
*				 this bit is used to indicate the color of frames. When the UseIpYellow 
*				 bit is cleared to a zero (Port offset 0x19) this bit is used 
*				 to indicate these frames are AVB stream frames.
*              disIpQpri;  
*              ipQpri;   
*              disIpFpri;   
*              ipFpri;   
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetIpPrioMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    IN  GT_U8       loc,
    OUT GT_PRIORITY_MAP_DATA    *mapdata
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U16        data;
    GT_U16        ptr;
    GT_U8           hwPort;         /* the physical port number     */
    int count=0x10;

    DBG_INFO(("gprtGetIpPrioMap Called.\n"));

    hwPort = GT_LPORT_2_PORT(port);
    if (hwPort >= dev->numOfPorts)
        return GT_BAD_PARAM;

    /* Check if Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_IP_PRI_MAPPING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
	ptr = loc&0x3f; 
	data = 0;
	data |= (ptr<<9);

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

#if 0
	/* Set pointer, Map table index */
	data |= 0x8000;
    retVal = hwWritePortReg(dev, hwPort, QD_REG_IP_PRI_MAPPING, data);
    if(retVal != GT_OK)
      {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
    }
#endif
    /* Wait until the Table is ready. */
    do    
    {
      retVal = hwReadPortReg(dev, hwPort, QD_REG_IP_PRI_MAPPING, &data);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
      }
	  if((count--)==0) return GT_FAIL;

    } while( (data&0x8000) == 0x8000);

    gtSemGive(dev,dev->tblRegsSem);

	mapdata->ipYellow  = (data>>8)&1;
	mapdata->disIpQpri = (data>>7)&1;
	mapdata->ipQpri    = (data>>4)&7;
	mapdata->disIpFpri = (data>>3)&1;
	mapdata->ipFpri    = (data>>0)&7;

    return GT_OK;
}

/*******************************************************************************
* gprtSetIEEEPrioMap
*
* DESCRIPTION:
*        This API set port IEEE Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        table    - Frame Priority Table set
*          GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*        mapLoc  - Mapping table location.  Pointer to the desired entry of 
*                  the IEEE Priority Mapping table selected above
*        mapData - Mapping data structure.
*              GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtSetIEEEPrioMap
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_IEEE_PRIORITY_TABLE_SET       table,
    IN  GT_U8       loc,
    IN  GT_IEEE_PRIORITY_TABLE_STRUCT    *prioTblStruct
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U16        data, data1;
    GT_U16        ptr;
    GT_U8           hwPort;         /* the physical port number     */
    int count=0x10;

    DBG_INFO(("gprtSetIEEEPrioMap Called.\n"));

    hwPort = GT_LPORT_2_PORT(port);
    if (hwPort >= dev->numOfPorts)
        return GT_BAD_PARAM;

    /* Check if Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_IEEE_2_PRI_MAPPING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	ptr = ((loc&0x7)<<9) | ((table&0x7)<<12); 

	switch(table)
	{
	  case GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI:
        data1 = prioTblStruct->ingTbl.ieeeYellow;
		data1 &=0xff;
		data = data1<<8;
		data1 = prioTblStruct->ingTbl.disIeeeQpri;
		data1 &=0xff;
		data |= data1<<7;
		data1 =	prioTblStruct->ingTbl.ieeeQpri;
		data1 &=0xff;
		data |= data1<<4;
		data1 =	prioTblStruct->ingTbl.disIeeeFpri;
		data1 &=0xff;
		data |= data1<<3;
		data = prioTblStruct->ingTbl.ieeeFpri;
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP:
        data1 = prioTblStruct->egGreeTblPcp.disVIDMarking;
		data1 &=0xff;
		data = data1<<4;
		data1 = prioTblStruct->egGreeTblPcp.disPCPMarking;
		data1 &=0xff;
		data |= data1<<3;
		data |=	prioTblStruct->egGreeTblPcp.framePCP;
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP:
        data1 = prioTblStruct->egYelTblPcp.disVIDMarking;
		data1 &=0xff;
		data = data1<<4;
		data1 = prioTblStruct->egYelTblPcp.disPCPMarking;
		data1 &=0xff;
		data |= data1<<3;
		data |=	prioTblStruct->egYelTblPcp.framePCP<<1;
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP:
        data1 = prioTblStruct->egAvbTblPcp.disVIDMarking;
		data1 &=0xff;
		data = data1<<4;
		data1 = prioTblStruct->egAvbTblPcp.disPCPMarking;
		data1 &=0xff;
		data |= data1<<3;
		data |=	prioTblStruct->egAvbTblPcp.framePCP;
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP:
        data1 = prioTblStruct->egGreeTblDscp.enDSCPMarking;
		data1 &=0xff;
		data = data1<<6;
		data |= prioTblStruct->egGreeTblDscp.frameDSCP;
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP:
        data1 = prioTblStruct->egYelTblDscp.enDSCPMarking;
		data1 &=0xff;
		data = data1<<6;
		data |= prioTblStruct->egYelTblDscp.frameDSCP;
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP:
        data1 = prioTblStruct->egAvbTbldscp.enDSCPMarking;
		data1 &=0xff;
		data = data1<<6;
		data |= prioTblStruct->egAvbTbldscp.frameDSCP;
        break;
	  default:
        return GT_BAD_PARAM;
	}

	data |= (ptr);

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Wait until the Table is ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, hwPort, PORT_ACCESS);
      regAccess.rw_reg_list[0].reg = QD_REG_IEEE_PRI_MAPPING;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
      }
    }
#else
    do    
    {
        retVal = hwGetPortRegField(dev,hwPort,QD_REG_IEEE_PRI_MAPPING,15,1,&data1);
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->tblRegsSem);
            return retVal;
        }
		if((count--)==0) return GT_FAIL;

    } while(data1 == 1);
#endif

    data = data | 0x8000;

    retVal = hwWritePortReg(dev, hwPort, QD_REG_IEEE_PRI_MAPPING, data);
    if(retVal != GT_OK)
      {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
    }


    gtSemGive(dev,dev->tblRegsSem);

    return GT_OK;
}


/*******************************************************************************
* gprtGetIEEEPrioMap
*
* DESCRIPTION:
*        This API ge tport IEEE Priority Mapping.
*
* INPUTS:
*        port    - the logical port number
*        table    - Frame Priority Table set
*          GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP
*          GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP
*          GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP
*        mapLoc  - Mapping table location.  Pointer to the desired entry of 
*                  the IEEE Priority Mapping table selected above
*        mapData - Mapping data structure.
*              GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* OUTPUTS:
*        mapData - Mapping data structure.
*              GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP_STRUCT;
*              GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP_STRUCT;
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gprtGetIEEEPrioMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    IN  GT_IEEE_PRIORITY_TABLE_SET       table,
    IN  GT_U8       loc,
    OUT GT_IEEE_PRIORITY_TABLE_STRUCT    *prioTblStruct
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U16        data;
    GT_U16        ptr;
    GT_U8           hwPort;         /* the physical port number     */
    int count=0x10;

    DBG_INFO(("gprtGetIpPrioMap Called.\n"));

    hwPort = GT_LPORT_2_PORT(port);
    if (hwPort >= dev->numOfPorts)
        return GT_BAD_PARAM;

    /* Check if Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PORT_IEEE_2_PRI_MAPPING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	ptr = ((loc&0x7)<<9) | ((table&0x7)<<12); 
	data = (ptr);

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);
#if 0
	/* Set pointer, Map table index */
    retVal = hwWritePortReg(dev, hwPort, QD_REG_IEEE_PRI_MAPPING, data);
    if(retVal != GT_OK)
      {
        DBG_INFO(("Failed.\n"));
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
    }
#endif
    /* Wait until the Table is ready. */
    do    
    {
      retVal = hwReadPortReg(dev, hwPort, QD_REG_IEEE_PRI_MAPPING, &data);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
      }
      if((count--)==0) return GT_FAIL;

    } while( (data&0x8000) == 0x8000);

    gtSemGive(dev,dev->tblRegsSem);

	switch(table)
	{
	  case GT_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI:
		prioTblStruct->ingTbl.ieeeFpri = (GT_U8)(data&0x7);
		data >>=3;
        prioTblStruct->ingTbl.disIeeeFpri = (GT_U8)(data&1);
		data >>=1;
		prioTblStruct->ingTbl.ieeeQpri = (GT_U8)(data&0x7);
		data >>=3;
        prioTblStruct->ingTbl.disIeeeQpri = (GT_U8)(data&1);
		data >>=1;
        prioTblStruct->ingTbl.ieeeYellow = (GT_U8)(data&1);
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP:
		prioTblStruct->egGreeTblPcp.framePCP = (GT_U8)(data&0x7);
		data >>=3;
        prioTblStruct->egGreeTblPcp.disPCPMarking = (GT_U8)(data&1);
		data >>=1;
		prioTblStruct->egGreeTblPcp.disVIDMarking = (GT_U8)(data&1);
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP:
		prioTblStruct->egYelTblPcp.framePCP = (GT_U8)(data&0x7);
		data >>=3;
        prioTblStruct->egYelTblPcp.disPCPMarking = (GT_U8)(data&1);
		data >>=1;
		prioTblStruct->egYelTblPcp.disVIDMarking = (GT_U8)(data&1);
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP:
		prioTblStruct->egAvbTblPcp.framePCP = data&0x7;
		data >>=3;
        prioTblStruct->egAvbTblPcp.disPCPMarking = (GT_U8)(data&1);
		data >>=1;
		prioTblStruct->egAvbTblPcp.disVIDMarking = (GT_U8)(data&1);
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP:
		prioTblStruct->egGreeTblDscp.frameDSCP = (GT_U8)(data&0x3f);
		data >>=6;
        prioTblStruct->egGreeTblDscp.enDSCPMarking = (GT_U8)(data&1);
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP:
		prioTblStruct->egYelTblDscp.frameDSCP = (GT_U8)(data&0x3f);
		data >>=6;
        prioTblStruct->egYelTblDscp.enDSCPMarking = (GT_U8)(data&1);
        break;
	  case GT_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP:
		prioTblStruct->egAvbTbldscp.frameDSCP = (GT_U8)(data&0x3f);
		data >>=6;
        prioTblStruct->egAvbTbldscp.enDSCPMarking = (GT_U8)(data&1);
        break;
	  default:
        return GT_BAD_PARAM;
	}

    return GT_OK;
}

/* It might need to add seperate API for different tables */
