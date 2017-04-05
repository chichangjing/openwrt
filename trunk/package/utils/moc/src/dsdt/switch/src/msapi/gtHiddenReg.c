#include <Copyright.h>

/*******************************************************************************
* gtHiddenReg.c
*
* DESCRIPTION:
*       API definitions for access hidden registers for debug
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msApi.h>
#include <gtSem.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>

GT_STATUS gsysReadHiddenReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    OUT  GT_U16          *data
);

GT_STATUS gsysReadHiddenReg_inc_port
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    OUT  GT_U16          *data
);


GT_STATUS gsysReadHiddenReg_inc_reg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    OUT  GT_U16          *data
);

GT_STATUS gsysWriteHiddenReg
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    IN  GT_U16            data
);



/*******************************************************************************
* hiddenRegOperationPerform
*
* DESCRIPTION:
*       This function accesses Hidden registers through Command and Data.
*       The device supports the following Hidden register operations
*   GT_HIDDENREG_WRITE_DATA         - Write data to the Hidden register
*   GT_HIDDENREG_READ_DATA          - Read data from Hidden register
*   GT_HIDDENREG_READ_DATA_INC_PORT - Read data from Hidden register, port + 1
*   GT_HIDDENREG_READ_DATA_INC_REG  - Read data from Hidden register, reg + 1
*
* INPUTS:
*       port             - port of Hidden register, 0xff is global register.
*       hiddenRegOp      - Hidden register Opcode.
*       hiddenRegData    - Data to be written to the Hidden register 
*
* OUTPUTS:
*       hiddenRegData    - Data that was read back from the Hidden register. 
*
Command register above.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*       GT_BAD_PARAM - if input parameters are beyond range.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS hiddenRegOperationPerform
(
    IN    GT_QD_DEV               *dev,
    IN    GT_U8                   port,
    IN    GT_HIDDENREG_OPERATION  hiddenRegOp,
    INOUT GT_HIDDENREG_OP_DATA    *opData
)
{
    GT_STATUS       retVal;    /* Functions return value */
    GT_U16          data;     /* temporary Data storage */
    DBG_INFO(("hiddenRegOperationPerform Called.\n"));


	
    /* Wait until the hiddenReg in ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = 0x10;
      regAccess.rw_reg_list[0].reg = QD_REG_HIDDEN_REG_ACCESS;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        return retVal;
      }
    }
#else
    data = 1;
    while(data == 1)
    {
        retVal = hwGetPortRegField(dev, 0, QD_REG_HIDDEN_REG_ACCESS,15,1,&data);
        if(retVal != GT_OK)
        {
            return retVal;
        }
    }
#endif


    /* Set the HIDDENREG Operation register */
    switch (hiddenRegOp)
    {
        case GT_HIDDENREG_WRITE_DATA:
            data = (GT_U16)opData->hiddenRegData;
            retVal = hwWritePortReg(dev,1, QD_REG_HIDDEN_REG_ACCESS,data);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            data = (GT_U16)((1 << 15) | (GT_HIDDENREG_WRITE_DATA << 12) | 
                    ((port&0xF) << 8) | ((opData->hiddenRegAddr&0x7F)));
            retVal = hwWritePortReg(dev,0x0, QD_REG_HIDDEN_REG_ACCESS,data);
            if(retVal != GT_OK)
            {
                return retVal;
            }
            break;

        case GT_HIDDENREG_READ_DATA:
        case GT_HIDDENREG_READ_DATA_INC_PORT:
        case GT_HIDDENREG_READ_DATA_INC_REG:
		{
            data = (GT_U16)((1 << 15) | ((hiddenRegOp&7) << 12) | 
                    ((port&0xF) << 8) | ((opData->hiddenRegAddr&0x7F)));
            retVal = hwWritePortReg(dev,0, QD_REG_HIDDEN_REG_ACCESS,data);
            if(retVal != GT_OK)
            {
                return retVal;
            }

            
#ifdef GT_RMGMT_ACCESS
            {
              HW_DEV_REG_ACCESS regAccess;

              regAccess.entries = 1;
  
              regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
              regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[0].reg = 0x10, QD_REG_HIDDEN_REG_ACCESS;
              regAccess.rw_reg_list[0].data = 15;
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                return retVal;
              }
            }
#else
            data = 1;
            while(data == 1)
            {
                retVal = hwGetPortRegField(dev,0, QD_REG_HIDDEN_REG_ACCESS,15,1,&data);
                if(retVal != GT_OK)
                {
                    return retVal;
                }
            }
#endif
            

            retVal = hwReadPortReg(dev,1, QD_REG_HIDDEN_REG_ACCESS,&data);
            opData->hiddenRegData = (GT_U16)data;
		}
            break;

        default:
            return GT_FAIL;
    }

    return GT_OK;
}


/*******************************************************************************
* gsysReadHiddenReg
*
* DESCRIPTION:
*        Read Hidden register of switch.
*
* INPUTS:
*        port - port of Hidden register, 0xff is global register.
*        addr - Hidden register Address. 
*
* OUTPUTS:
*        data -  Data that was read back from the Hidden register.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysReadHiddenReg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    OUT  GT_U16          *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_HIDDENREG_OPERATION    hiddenRegOp;
    GT_HIDDENREG_OP_DATA      opData;
    DBG_INFO(("gsysReadHiddenReg Called.\n"));
 
    hiddenRegOp = GT_HIDDENREG_READ_DATA;
    opData.hiddenRegAddr = addr;

    retVal = hiddenRegOperationPerform(dev,port,hiddenRegOp,&opData);
    if(retVal != GT_OK)
    {
      return retVal;
    }
    *data = opData.hiddenRegData;
    return GT_OK;

}

/*******************************************************************************
* gsysReadHiddenReg_inc_port
*
* DESCRIPTION:
*        Read Hidden register of switch, after port+1.
*
* INPUTS:
*        port - port of Hidden register, 0xff is global register.
*        addr - Hidden register Address. 
*
* OUTPUTS:
*        data -  Data that was read back from the Hidden register.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysReadHiddenReg_inc_port
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    OUT  GT_U16          *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_HIDDENREG_OPERATION    hiddenRegOp;
    GT_HIDDENREG_OP_DATA      opData;
 
    DBG_INFO(("gsysReadHiddenReg_inc_port Called.\n"));

    hiddenRegOp = GT_HIDDENREG_READ_DATA_INC_PORT;
    opData.hiddenRegAddr = addr;

    retVal = hiddenRegOperationPerform(dev,port,hiddenRegOp,&opData);
    if(retVal != GT_OK)
    {
      return retVal;
    }
    *data = opData.hiddenRegData;
    return GT_OK;

}

/*******************************************************************************
* gsysReadHiddenReg_inc_reg
*
* DESCRIPTION:
*        Read Hidden register of switch, after register address +1.
*
* INPUTS:
*        port - port of Hidden register, 0xff is global register.
*        addr - Hidden register Address. 
*
* OUTPUTS:
*        data -  Data that was read back from the Hidden register.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysReadHiddenReg_inc_reg
(
    IN  GT_QD_DEV        *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    OUT  GT_U16          *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_HIDDENREG_OPERATION    hiddenRegOp;
    GT_HIDDENREG_OP_DATA      opData;
    DBG_INFO(("gsysReadHiddenReg_inc_reg Called.\n"));
 
    hiddenRegOp = GT_HIDDENREG_READ_DATA_INC_REG;
    opData.hiddenRegAddr = addr;

    retVal = hiddenRegOperationPerform(dev,port,hiddenRegOp,&opData);
    if(retVal != GT_OK)
    {
      return retVal;
    }
    *data = opData.hiddenRegData;
    return GT_OK;

}

/*******************************************************************************
* gsysWriteHiddenReg
*
* DESCRIPTION:
*        Write Hidden register of switch.
*
* INPUTS:
*        port - port of Hidden register, 0xff is global register.
*        addr - Hidden register Address. 
*        data - Data to be written to the Hidden register
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gsysWriteHiddenReg
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8            port,
    IN  GT_U8            addr,
    IN  GT_U16            data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_HIDDENREG_OPERATION    hiddenRegOp;
    GT_HIDDENREG_OP_DATA    opData;
    DBG_INFO(("gsysWriteHiddenReg Called.\n"));
 
    hiddenRegOp = GT_HIDDENREG_WRITE_DATA;
    opData.hiddenRegAddr = addr;
    opData.hiddenRegData = data;

    retVal = hiddenRegOperationPerform(dev,port,hiddenRegOp,&opData);
    if(retVal != GT_OK)
    {
      return retVal;
    }
    return GT_OK;

}










