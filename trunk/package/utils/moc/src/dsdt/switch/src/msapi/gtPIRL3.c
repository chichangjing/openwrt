#include <Copyright.h>

/*******************************************************************************
* gtPIRL3.c
*
* DESCRIPTION:
*       API definitions for Port based PIRL Resources
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msApi.h>
#include <gtSem.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>

#define MAX_PIRL3_IP_STORM_RES  5  
#define MAX_PIRL3_AVB_RES  3 

/****************************************************************************/
/* PIRL operation function declaration.                                    */
/****************************************************************************/
static GT_STATUS pirl3OperationPerform
(
    IN   GT_QD_DEV            *dev,
    IN   GT_PIRL3_OPERATION    pirlOp,
    INOUT GT_PIRL3_OP_DATA     *opData
);

static GT_STATUS pirl3Initialize
(
    IN  GT_QD_DEV              *dev
);

static GT_STATUS pirl3InitIRLResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes
);

static GT_STATUS pirl3DisableIRLResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes
);

static GT_STATUS pirl3DataToResource
(
    IN  GT_QD_DEV              *dev,
    IN  GT_PIRL3_DATA        *pirlData,
    OUT GT_PIRL3_RESOURCE    *res
);

static GT_STATUS pirl3ResourceToData
(
    IN  GT_QD_DEV              *dev,
    IN  GT_PIRL3_RESOURCE    *res,
    OUT GT_PIRL3_DATA        *pirlData
);

static GT_STATUS pirl3WriteResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    IN  GT_PIRL3_RESOURCE    *res
);

static GT_STATUS pirl3ReadResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    OUT GT_PIRL3_RESOURCE    *res
);

static GT_STATUS pirl3WriteTSMResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    IN  GT_PIRL3_TSM_RESOURCE    *res
);

static GT_STATUS pirl3ReadTSMResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    OUT GT_PIRL3_TSM_RESOURCE    *res
);

/*******************************************************************************
* gpirl3WriteResource
*
* DESCRIPTION:
*        This routine writes resource bucket parameters to the given resource
*        of the port.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 7).
*                   IRLRes numbers 0x0 to 0x4 are IP/Storm Rate Limiting Resources.
*                   IRLRes numbers 0x5 to 0x7 are AVB Rate Limiting Resources
*        pirlData - PIRL resource parameters.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gpirl3WriteResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    IN  GT_PIRL3_DATA    *pirlData
)
{
    GT_STATUS           retVal;
    GT_PIRL3_RESOURCE    pirlRes;
    GT_U32               irlPort;         /* the physical port number     */
    GT_U32                maxRes;

    DBG_INFO(("gpirl3WriteResource Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PIRL3_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    {
        maxRes = MAX_PIRL3_IP_STORM_RES;
    }

    if (irlRes >= maxRes)
    {
        DBG_INFO(("GT_BAD_PARAM irlRes\n"));
        return GT_BAD_PARAM;
    }

    irlPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (irlPort == GT_INVALID_PORT)
    {
        DBG_INFO(("GT_BAD_PARAM port\n"));
        return GT_BAD_PARAM;
    }

    /* Initialize internal counters */
    retVal = pirl3InitIRLResource(dev,irlPort,irlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Write Resource failed.\n"));
        return retVal;
    }

    /* Program the Ingress Rate Resource Parameters */
    retVal = pirl3DataToResource(dev,pirlData,&pirlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Data to PIRL Resource conversion failed.\n"));
        return retVal;
    }

    retVal = pirl3WriteResource(dev,irlPort,irlRes,&pirlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Write Resource failed.\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gpirl3ReadResource
*
* DESCRIPTION:
*        This routine retrieves IRL Parameter.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 7).
*                   IRLRes numbers 0x0 to 0x4 are IP/Storm Rate Limiting Resources.
*                   IRLRes numbers 0x5 to 0x7 are AVB Rate Limiting Resources
*
* OUTPUTS:
*        pirlData - PIRL resource parameters.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gpirl3ReadResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    OUT GT_PIRL3_DATA    *pirlData
)
{
    GT_STATUS           retVal;
    GT_U32                irlPort;
    GT_PIRL3_RESOURCE    pirlRes;
    GT_U32                maxRes;

    DBG_INFO(("gpirl3ReadResource Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PIRL3_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    {
        maxRes = MAX_PIRL3_IP_STORM_RES;
    }

    if (irlRes >= maxRes)
    {
        DBG_INFO(("GT_BAD_PARAM irlRes\n"));
        return GT_BAD_PARAM;
    }

    irlPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (irlPort == GT_INVALID_PORT)
    {
        DBG_INFO(("GT_BAD_PARAM port\n"));
        return GT_BAD_PARAM;
    }

    /* Read the Ingress Rate Resource Parameters */
    retVal = pirl3ReadResource(dev,irlPort,irlRes,&pirlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Read Resource failed.\n"));
        return retVal;
    }

    retVal = pirl3ResourceToData(dev,&pirlRes,pirlData);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Resource to PIRL Data conversion failed.\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gpirl3DisableResource
*
* DESCRIPTION:
*       This routine disables Ingress Rate Limiting for the given bucket.
*
* INPUTS:
*       port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 7).
*                   IRLRes numbers 0x0 to 0x4 are IP/Storm Rate Limiting Resources.
*                   IRLRes numbers 0x5 to 0x7 are AVB Rate Limiting Resources
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirl3DisableResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes
)
{
    GT_STATUS           retVal;
    GT_U32                irlPort;
    GT_U32                maxRes;

    DBG_INFO(("gpirl3DisableResource Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PIRL3_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    {
        maxRes = MAX_PIRL3_IP_STORM_RES;
    }

    if (irlRes >= maxRes)
    {
        DBG_INFO(("GT_BAD_PARAM irlRes\n"));
        return GT_BAD_PARAM;
    }

    irlPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (irlPort == GT_INVALID_PORT)
    {
        DBG_INFO(("GT_BAD_PARAM port\n"));
        return GT_BAD_PARAM;
    }

    /* disable irl resource */
    retVal = pirl3DisableIRLResource(dev, irlPort, irlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Getting Port State failed\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gpirl3SetCurTimeUpInt
*
* DESCRIPTION:
*       This function sets the current time update interval.
*        Please contact FAE for detailed information.
*
* INPUTS:
*       upInt - updata interval (0 ~ 7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpirl3SetCurTimeUpInt
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                upInt
)
{
    GT_STATUS       retVal;        /* Functions return value */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA    opData;

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PIRL3_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (upInt > 0x7)
        return GT_BAD_PARAM;

    {
      op = PIRL_READ_RESOURCE_1;
    }


    opData.irlPort = 0xF;
    opData.irlRes = 0;
    opData.irlReg = 1;
    opData.irlData = 0;

    retVal = pirl3OperationPerform(dev, op, &opData);
    if (retVal != GT_OK)
    {
           DBG_INFO(("PIRL OP Failed.\n"));
           return retVal;
    }

    op = PIRL_WRITE_RESOURCE;
    opData.irlData = (opData.irlData & 0xFFF8) | (GT_U16)upInt;

    retVal = pirl3OperationPerform(dev, op, &opData);
    if (retVal != GT_OK)
    {
           DBG_INFO(("PIRL OP Failed.\n"));
           return retVal;
    }

    return GT_OK;    
}


/*******************************************************************************
* gpirl3WriteTSMResource
*
* DESCRIPTION:
*        This routine writes rate resource bucket parameters in Time Slot Metering
*        mode to the given resource of the port.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 1).
*        pirlData - PIRL TSM resource parameters.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Only Resource 0 and 1 can be supported for TSM Mode.
*
*******************************************************************************/
GT_STATUS gpirl3WriteTSMResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    IN  GT_PIRL3_TSM_DATA    *pirlData
)
{
    GT_STATUS           retVal;
    GT_PIRL3_TSM_RESOURCE    pirlRes;
    GT_U32               irlPort;         /* the physical port number     */
    GT_U32                maxRes;
    GT_U32                cbs, cts, i, rate;

    DBG_INFO(("gpirl3WriteTSMResource Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TSM_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given bucket number is valid */
    maxRes = MAX_PIRL3_AVB_RES;

    if (irlRes >= maxRes)
    {
        DBG_INFO(("GT_BAD_PARAM irlRes\n"));
        return GT_BAD_PARAM;
    }

    irlPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (irlPort == GT_INVALID_PORT)
    {
        DBG_INFO(("GT_BAD_PARAM port\n"));
        return GT_BAD_PARAM;
    }

    /* Initialize internal counters */
    retVal = pirl3InitIRLResource(dev,irlPort,irlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Write Resource failed.\n"));
        return retVal;
    }

    if (pirlData->customSetup.isValid == GT_TRUE)
    {
        pirlRes.cbsLimit = pirlData->customSetup.cbsLimit;
        pirlRes.ctsIntv = pirlData->customSetup.ctsIntv;
        pirlRes.ebsLimit = pirlData->customSetup.ebsLimit;
    }
    else
    {
        /* convert ingressRate to cbsLimit and ctsIntv */
        cts = 1; 
        cbs = 0;
        i = 3;
        rate = pirlData->ingressRate;
        while(cts < 16)
        {
            cbs = TSM_GET_CBS(rate, cts);
            if ((cbs == 0) || (cbs <= 0xFFFF))
                break;
            cts += i;
            i = cts;
        }

        if (cts > 16)
        {
            return GT_BAD_PARAM;
        }

        switch (cts)
        {
            case 1:
                pirlRes.ctsIntv = 3;
                break;
            case 4:
                pirlRes.ctsIntv = 2;
                break;
            case 8:
                pirlRes.ctsIntv = 1;
                break;
            case 16:
                pirlRes.ctsIntv = 0;
                break;
            default:
                return GT_FAIL;
        }

        pirlRes.cbsLimit = cbs;
        pirlRes.ebsLimit = 0xFFFF;
    }

    pirlRes.priSelect = pirlData->priSelect;
    pirlRes.stopAll = pirlData->stopAll;
    pirlRes.allStoped = pirlData->allStoped;
    pirlRes.itsmModeEn = GT_TRUE;

    if (pirlData->itsmModeEn == GT_FALSE)
    {
        pirlRes.itsmModeEn = 0;
        pirlRes.cbsLimit = 0;
        pirlRes.ctsIntv = 0;
        pirlRes.ebsLimit = 0;
        pirlRes.stopAll = GT_FALSE;
        pirlRes.allStoped = GT_FALSE;
        pirlRes.priSelect = 0;
    }

    retVal = pirl3WriteTSMResource(dev,irlPort,irlRes,&pirlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Write Resource failed.\n"));
        return retVal;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gpirl3ReadTSMResource
*
* DESCRIPTION:
*        This routine retrieves IRL Parameter.
*        Returned ingressRate would be rough number. Instead, customSetup will
*        have the exact configured value.
*
* INPUTS:
*        port     - logical port number.
*        irlRes   - bucket to be used (0 ~ 1).
*
* OUTPUTS:
*        pirlData - PIRL resource parameters.
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Only Resource 0 and 1 can be supported for TSM Mode.
*
*******************************************************************************/
GT_STATUS gpirl3ReadTSMResource
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        irlRes,
    OUT GT_PIRL3_TSM_DATA    *pirlData
)
{
    GT_STATUS           retVal;
    GT_U32                irlPort;
    GT_PIRL3_TSM_RESOURCE    pirlRes;
    GT_U32                maxRes, cbs, cts;

    DBG_INFO(("gpirl3ReadTSMResource Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TSM_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given bucket number is valid */
    maxRes = MAX_PIRL3_AVB_RES;

    if (irlRes >= maxRes)
    {
        DBG_INFO(("GT_BAD_PARAM irlRes\n"));
        return GT_BAD_PARAM;
    }

    irlPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (irlPort == GT_INVALID_PORT)
    {
        DBG_INFO(("GT_BAD_PARAM port\n"));
        return GT_BAD_PARAM;
    }

    /* Read the Ingress Rate Resource Parameters */
    retVal = pirl3ReadTSMResource(dev,irlPort,irlRes,&pirlRes);
    if(retVal != GT_OK)
    {
        DBG_INFO(("PIRL Read Resource failed.\n"));
        return retVal;
    }

    if (pirlRes.itsmModeEn == 0)
    {
        /* TMS Mode is not enabled */
        pirlData->itsmModeEn = GT_FALSE;
        pirlData->ingressRate = 0;
        pirlData->priSelect = 0;
        pirlData->customSetup.isValid = 0;
        pirlData->customSetup.cbsLimit = 0;
        pirlData->customSetup.ctsIntv = 0;
        pirlData->customSetup.ebsLimit = 0;
        pirlData->customSetup.actionMode = 0;
        pirlRes.stopAll = GT_FALSE;
        pirlRes.allStoped = GT_FALSE;
        return GT_OK;
    }

    cbs = pirlRes.cbsLimit;
    switch (pirlRes.ctsIntv)
    {
        case 0:
            cts = 16;
            break;
        case 1:
            cts = 8;
            break;
        case 2:
            cts = 4;
            break;
        case 3:
            cts = 1;
            break;
        default:
            return GT_FAIL;
    }

    pirlData->ingressRate = TSM_GET_RATE(cbs,cts);

    pirlData->priSelect = pirlRes.priSelect;
    pirlData->stopAll = pirlRes.stopAll;
    pirlData->allStoped = pirlRes.allStoped;

    pirlData->customSetup.isValid = GT_TRUE;
    pirlData->customSetup.cbsLimit = pirlRes.cbsLimit;
    pirlData->customSetup.ctsIntv = pirlRes.ctsIntv;
    pirlData->customSetup.ebsLimit = pirlRes.ebsLimit;

    DBG_INFO(("OK.\n"));
    return GT_OK;

}




/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* gpirl3Initialize
*
* DESCRIPTION:
*       This routine initializes PIRL Resources.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gpirl3Initialize
(
    IN  GT_QD_DEV              *dev
)
{
    GT_STATUS           retVal;

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PIRL3_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    retVal = pirl3Initialize(dev);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return retVal;
    }

    return GT_OK;
}


/*******************************************************************************
* pirl3OperationPerform
*
* DESCRIPTION:
*       This function accesses Ingress Rate Command Register and Data Register.
*
* INPUTS:
*       pirlOp     - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       pirlData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS pirl3OperationPerform
(
    IN    GT_QD_DEV             *dev,
    IN    GT_PIRL3_OPERATION    pirlOp,
    INOUT GT_PIRL3_OP_DATA        *opData
)
{
    GT_STATUS       retVal;    /* Functions return value */
    GT_U16          data;     /* temporary Data storage */

    gtSemTake(dev,dev->pirlRegsSem,OS_WAIT_FOREVER);

    /* Wait until the pirl in ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
      regAccess.rw_reg_list[0].reg = QD_REG_INGRESS_RATE_COMMAND;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->pirlRegsSem);
        return retVal;
      }
    }
#else
    data = 1;
    while(data == 1)
    {
        retVal = hwGetGlobal2RegField(dev,QD_REG_INGRESS_RATE_COMMAND,15,1,&data);
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->pirlRegsSem);
            return retVal;
        }
    }
#endif

    /* Set the PIRL Operation register */
    switch (pirlOp)
    {
        case PIRL_INIT_ALL_RESOURCE:
		  {
            data = (1 << 15) | (PIRL_INIT_ALL_RESOURCE << 13);
		  }
            retVal = hwWriteGlobal2Reg(dev,QD_REG_INGRESS_RATE_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->pirlRegsSem);
                return retVal;
            }
            break;
        case PIRL_INIT_RESOURCE:
		  {
            data = (GT_U16)((1 << 15) | (PIRL_INIT_RESOURCE << 13) | 
                    (opData->irlPort << 8) |
                    (opData->irlRes << 5));
		  }

            retVal = hwWriteGlobal2Reg(dev,QD_REG_INGRESS_RATE_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->pirlRegsSem);
                return retVal;
            }
            break;

        case PIRL_WRITE_RESOURCE:
            data = (GT_U16)opData->irlData;
            retVal = hwWriteGlobal2Reg(dev,QD_REG_INGRESS_RATE_DATA,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->pirlRegsSem);
                return retVal;
            }

		  {
            data = (GT_U16)((1 << 15) | (PIRL_WRITE_RESOURCE << 13) | 
                    (opData->irlPort << 8)    |
                    (opData->irlRes << 5)    |
                    (opData->irlReg & 0xF));
		  }
            retVal = hwWriteGlobal2Reg(dev,QD_REG_INGRESS_RATE_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->pirlRegsSem);
                return retVal;
            }
            break;

        case PIRL_READ_RESOURCE:
        case PIRL_READ_RESOURCE_1:
		  {
            data = (GT_U16)((1 << 15) | (PIRL_READ_RESOURCE_1 << 13) | 
                    (opData->irlPort << 8)    |
                    (opData->irlRes << 5)    |
                    (opData->irlReg & 0xF));
		  }
            retVal = hwWriteGlobal2Reg(dev,QD_REG_INGRESS_RATE_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->pirlRegsSem);
                return retVal;
            }

#ifdef GT_RMGMT_ACCESS
            {
              HW_DEV_REG_ACCESS regAccess;

              regAccess.entries = 1;
  
              regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
              regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[0].reg = QD_REG_INGRESS_RATE_COMMAND;
              regAccess.rw_reg_list[0].data = 15;
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                gtSemGive(dev,dev->pirlRegsSem);
                return retVal;
              }
            }
#else
            data = 1;
            while(data == 1)
            {
                retVal = hwGetGlobal2RegField(dev,QD_REG_INGRESS_RATE_COMMAND,15,1,&data);
                if(retVal != GT_OK)
                {
                    gtSemGive(dev,dev->pirlRegsSem);
                    return retVal;
                }
            }
#endif

            retVal = hwReadGlobal2Reg(dev,QD_REG_INGRESS_RATE_DATA,&data);
            opData->irlData = (GT_U32)data;
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->pirlRegsSem);
                return retVal;
            }
            gtSemGive(dev,dev->pirlRegsSem);
            return retVal;

        default:
            
            gtSemGive(dev,dev->pirlRegsSem);
            return GT_FAIL;
    }

    /* Wait until the pirl in ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;
      regAccess.entries = 1;
  
      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
      regAccess.rw_reg_list[0].reg = QD_REG_INGRESS_RATE_COMMAND;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->pirlRegsSem);
        return retVal;
      }
    }
#else
    data = 1;
    while(data == 1)
    {
        retVal = hwGetGlobal2RegField(dev,QD_REG_INGRESS_RATE_COMMAND,15,1,&data);
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->pirlRegsSem);
            return retVal;
        }
    }
#endif

    gtSemGive(dev,dev->pirlRegsSem);
    return retVal;
}

/*
 * Initialize all PIRL resources to the inital state.
*/
static GT_STATUS pirl3Initialize
(
    IN  GT_QD_DEV              *dev
)
{
    GT_STATUS       retVal;    /* Functions return value */
    GT_PIRL3_OPERATION    op;

    op = PIRL_INIT_ALL_RESOURCE;


    retVal = pirl3OperationPerform(dev, op, NULL);
    if (retVal != GT_OK)
    {
        DBG_INFO(("PIRL OP Failed.\n"));
        return retVal;
    }


    retVal = gpirl3SetCurTimeUpInt(dev,4);
    if (retVal != GT_OK)
    {
        DBG_INFO(("PIRL OP Failed.\n"));
    }

    return retVal;
}

/*
 * Initialize the selected PIRL resource to the inital state.
 * This function initializes only the BSM structure for the IRL Unit.
*/
static GT_STATUS pirl3InitIRLResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes
)
{
    GT_STATUS       retVal;    /* Functions return value */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA     opData;

    op = PIRL_INIT_RESOURCE;
    opData.irlPort = irlPort;
    opData.irlRes = irlRes;

    retVal = pirl3OperationPerform(dev, op, &opData);
    if (retVal != GT_OK)
    {
        DBG_INFO(("PIRL OP Failed.\n"));
        return retVal;
    }

    return retVal;
}

/*
 * Disable the selected PIRL resource.
*/
static GT_STATUS pirl3DisableIRLResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes
)
{
    GT_STATUS       retVal;            /* Functions return value */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA    opData;
    int                i;

    op = PIRL_WRITE_RESOURCE;

    for(i=0; i<9; i++)
    {
        opData.irlPort = irlPort;
        opData.irlRes = irlRes;
        opData.irlReg = i;
        opData.irlData = 0;

        retVal = pirl3OperationPerform(dev, op, &opData);
        if (retVal != GT_OK)
        {
            DBG_INFO(("PIRL OP Failed.\n"));
            return retVal;
        }
    }

    return GT_OK;    
}

/*
 * convert PIRL Data structure to PIRL Resource structure.
 * if PIRL Data is not valid, return GT_BAD_PARARM;
*/
static GT_STATUS pirl3DataToResource
(
    IN  GT_QD_DEV              *dev,
    IN  GT_PIRL3_DATA        *pirlData,
    OUT GT_PIRL3_RESOURCE    *res
)
{
    GT_U32 data;

    gtMemSet((void*)res,0,sizeof(GT_PIRL3_RESOURCE));

    data = (GT_U32)(pirlData->accountQConf|pirlData->accountFiltered|
                    pirlData->mgmtNrlEn|pirlData->saNrlEn|pirlData->daNrlEn|
                    pirlData->samplingMode);

    if (data > 1)
    {
        DBG_INFO(("GT_BAD_PARAM (Boolean)\n"));
        return GT_BAD_PARAM;
    }

    res->accountQConf = pirlData->accountQConf;
    res->accountFiltered = pirlData->accountFiltered;
    res->mgmtNrlEn = pirlData->mgmtNrlEn;
    res->saNrlEn = pirlData->saNrlEn;
    res->daNrlEn = pirlData->daNrlEn;
    res->samplingMode = pirlData->samplingMode;

    res->fcAction = pirlData->fcAction;
    switch(pirlData->fcMode)
    {
        case GT_PIRL_FC_DEASSERT_EMPTY:
        case GT_PIRL_FC_DEASSERT_CBS_LIMIT:
            res->fcMode = pirlData->fcMode;
            break;
        default:
                res->fcMode    = GT_PIRL_FC_DEASSERT_EMPTY;
                break;
            DBG_INFO(("GT_BAD_PARAM fcMode\n"));
            return GT_BAD_PARAM;
    }

    if(pirlData->customSetup.isValid == GT_TRUE)
    {
        res->ebsLimit = pirlData->customSetup.ebsLimit;
        res->cbsLimit = pirlData->customSetup.cbsLimit;
        res->bktIncrement = pirlData->customSetup.bktIncrement;
        res->bktRateFactorGreen = pirlData->customSetup.bktRateFactor;
/*        res->bktRateFactorYel = pirlData->customSetup.bktRateFactor; */
    }
    else
    {
        if(pirlData->ingressRate == 0)
        {
            DBG_INFO(("GT_BAD_PARAM ingressRate(%i)\n",pirlData->ingressRate));
            return GT_BAD_PARAM;
        }

        if(pirlData->ingressRate < 1000)    /* less than 1Mbps */
        {
            /* it should be divided by 64 */
            if(pirlData->ingressRate % 64)
            {
                DBG_INFO(("GT_BAD_PARAM ingressRate(%i)\n",pirlData->ingressRate));
                return GT_BAD_PARAM;
            }
            res->bktRateFactorGreen = pirlData->ingressRate/64;
/*            res->bktRateFactorYel = pirlData->ingressRate/64; */
        }
        else if(pirlData->ingressRate < 10000)    /* less than or equal to 10Mbps */
        {
            /* it should be divided by 1000 */
            if(pirlData->ingressRate % 1000)
            {
                DBG_INFO(("GT_BAD_PARAM ingressRate(%i)\n",pirlData->ingressRate));
                return GT_BAD_PARAM;
            }
            res->bktRateFactorGreen = pirlData->ingressRate/128 + ((pirlData->ingressRate % 128)?1:0);
/*            res->bktRateFactorYel = pirlData->ingressRate/128 + ((pirlData->ingressRate % 128)?1:0); */
        }
        else if(pirlData->ingressRate < 100000)    /* less than or equal to 100Mbps */
        {
            /* it should be divided by 1000 */
            if(pirlData->ingressRate % 1000)
            {
                DBG_INFO(("GT_BAD_PARAM ingressRate(%i)\n",pirlData->ingressRate));
                return GT_BAD_PARAM;
            }
            res->bktRateFactorGreen = pirlData->ingressRate/1000;
/*            res->bktRateFactorYel = pirlData->ingressRate/1000; */
        }
        else if(pirlData->ingressRate <= 200000)    /* less than or equal to 200Mbps */
        {
            /* it should be divided by 10000 */
            if(pirlData->ingressRate % 10000)
            {
                DBG_INFO(("GT_BAD_PARAM ingressRate(%i)\n",pirlData->ingressRate));
                return GT_BAD_PARAM;
            }
            res->bktRateFactorGreen = pirlData->ingressRate/1000;
/*            res->bktRateFactorYel = pirlData->ingressRate/1000; */
        }
        else
        {
            DBG_INFO(("GT_BAD_PARAM ingressRate(%i)\n",pirlData->ingressRate));
            return GT_BAD_PARAM;
        }

        res->ebsLimit = RECOMMENDED_ESB_LIMIT(dev, pirlData->ingressRate);
        res->cbsLimit = RECOMMENDED_CBS_LIMIT(dev, pirlData->ingressRate);
        res->bktIncrement = RECOMMENDED_BUCKET_INCREMENT(dev, pirlData->ingressRate);
    }



               res->bktTypeMask = pirlData->bktTypeMask;
            res->priORpt = pirlData->priORpt;
            res->priSelect = qdLong2Char(pirlData->priSelect);

    switch(pirlData->byteTobeCounted)
    {
        case GT_PIRL2_COUNT_FRAME:
        case GT_PIRL2_COUNT_ALL_LAYER1:
        case GT_PIRL2_COUNT_ALL_LAYER2:
        case GT_PIRL2_COUNT_ALL_LAYER3:
            res->byteTobeCounted = pirlData->byteTobeCounted;
            break;
        default:
            DBG_INFO(("GT_BAD_PARAM byteTobeCounted(%#x)\n",pirlData->byteTobeCounted));
            return GT_BAD_PARAM;
    }

    return GT_OK;            
}

/*
 * convert PIRL Resource structure to PIRL Data structure.
*/
static GT_STATUS pirl3ResourceToData
(
    IN  GT_QD_DEV              *dev,
    IN  GT_PIRL3_RESOURCE    *res,
    OUT GT_PIRL3_DATA        *pirlData
)
{
    GT_U32    rate;
    GT_U32    factor;

    pirlData->accountQConf = res->accountQConf;
    pirlData->accountFiltered = res->accountFiltered;
    pirlData->mgmtNrlEn = res->mgmtNrlEn;
    pirlData->saNrlEn = res->saNrlEn;
    pirlData->daNrlEn = res->daNrlEn;
    pirlData->samplingMode = res->samplingMode;
    pirlData->fcMode = res->fcMode;
    pirlData->fcAction = res->fcAction;

    pirlData->customSetup.isValid = GT_FALSE;

    FACTOR_FROM_BUCKET_INCREMENT(dev,res->bktIncrement,factor);

    rate = res->bktRateFactorGreen * factor;
/*    rate = res->bktRateFactorYel * factor; */
    if(factor == 128)
    {
        pirlData->ingressRate = rate - (rate % 1000);
    }
    else if (factor == 0)
    {
        pirlData->ingressRate = 0;
        pirlData->customSetup.isValid = GT_TRUE;
        pirlData->customSetup.ebsLimit = res->ebsLimit;
        pirlData->customSetup.cbsLimit = res->cbsLimit;
        pirlData->customSetup.bktIncrement = res->bktIncrement;
        pirlData->customSetup.bktRateFactor = res->bktRateFactorGreen;
/*        pirlData->customSetup.bktRateFactor = res->bktRateFactorYel; */
    }
    else
    {
        pirlData->ingressRate = rate;
    }

    pirlData->bktTypeMask = res->bktTypeMask;

    if (pirlData->bktTypeMask & 0x80)
    {
        res->bktTypeMask &= ~0x80;
        res->bktTypeMask |= BUCKET_TRAFFIC_ARP;
    }
            
    pirlData->priORpt = res->priORpt;
    pirlData->priSelect = res->priSelect;

    pirlData->byteTobeCounted = res->byteTobeCounted;

    return GT_OK;            
}

/*******************************************************************************
* pirl3WriteResource
*
* DESCRIPTION:
*       This function writes IRL Resource to BCM (Bucket Configuration Memory)
*
* INPUTS:
*       irlPort - physical port number.
*        irlRes  - bucket to be used (0 ~ 4).
*       res     - IRL Resource data
*
* OUTPUTS:
*       Nont.
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS pirl3WriteResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    IN  GT_PIRL3_RESOURCE    *res
)
{
    GT_STATUS       retVal;            /* Functions return value */
    GT_U16          data[16];     /* temporary Data storage */
    GT_U32          tmpData;     /* temporary Data */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA     opData;
    int                i;

    op = PIRL_WRITE_RESOURCE;

    /* reg0 data */
    data[0] = (GT_U16)(res->bktTypeMask);         /* Bit[15:0] : Traffic Type   */

    /* reg1 data */
    data[1] = (GT_U16)((res->priSelect)           | /* Bit[7:0]  : Priority Queue Select */
                    (res->framePrio << 8)         | /* Bit[8] : Frame Priority*/
                    (res->priORpt << 9)           | /* Bit[9] : Priority OR PacketType */
                    (res->accountFiltered << 10)  | /* Bit[10] : Account for All */
                    (res->accountQConf << 11)     | /* Bit[11] : account discarded frames due to queue congestion */
                    (res->acct4GrnOvflow << 12)   | /* Bit[12] : Account for Green Overflow */
                    (res->colorAware << 13)      | /* Bit[13] : Color Aware */
                    (res->samplingMode << 14));      /* Bit[14] : Sampling Mode */
	
    /* reg2 data */
	data[2] = (GT_U16)(res->bktIncrement          | /* Bit[12:0] : Bucket Increment */
                    (res->byteTobeCounted << 14) | /* Bit[15:14] : Bytes to be counted */
                    (res->tcamFlow << 13));        /* Bit[13] : Account QConf */
    
    /* reg3 data */
	data[3] = (GT_U16)res->bktRateFactorGreen;    /* Bit[15:0] : Bucket Rate Factor */

    /* reg4 data */
    data[4] = (GT_U16)(res->cbsLimit&0xFFFF);      /* Bit[15:0] : CBS Limit[15:0] */
 
    /* reg5 data */
    tmpData = (GT_U16)(res->cbsLimit >> 16);       /* CBS Limit[23:16] */
	
    data[5] = (GT_U16)(tmpData);    /* Bit[8] : CBS Limit[23:16] */

    /* reg6 data */
    data[6] = (GT_U16)res->bktRateFactorYel;     /* Bit[15:0] : Bucket Rate Factor Yellow*/ 
	
    /* reg7 data */
    data[7] = (GT_U16)(res->ebsLimit&0xFFFF);      /* Bit[15:0] : EBS Limit[15:0] */

    /* reg8 data */
	tmpData = (GT_U16)(res->ebsLimit >> 16);       /* EBS Limit[23:16] */
	
    data[8] = (GT_U16)(tmpData |
                      (res->daNrlEn << 8)       |    /* Bit[8]  : DA Nrl En */
                      (res->saNrlEn << 9)        |    /* Bit[9]  : SA Nrl En */
                      (res->mgmtNrlEn << 10)     |    /* Bit[10]  : MGMT Nrl En */
                      (res->fcAction<< 11)        |    /* Bit[11] : Flow control action */
                      (res->fcMode << 12)        |    /* Bit[12] : Flow control mode */
                      (res->fcPriority << 13));        /* Bit[15:13] : Flow control Priority */

    for(i=0; i<9; i++)
    {
        opData.irlPort = irlPort;
        opData.irlRes = irlRes;
        opData.irlReg = i;
        opData.irlData = data[i];

        retVal = pirl3OperationPerform(dev, op, &opData);
        if (retVal != GT_OK)
        {
            DBG_INFO(("PIRL OP Failed.\n"));
            return retVal;
        }
    }

    return GT_OK;    
}


/*******************************************************************************
* pirl3ReadResource
*
* DESCRIPTION:
*       This function reads IRL Resource from BCM (Bucket Configuration Memory)
*
* INPUTS:
*       irlPort  - physical port number.
*        irlRes   - bucket to be used (0 ~ 4).
*
* OUTPUTS:
*       res - IRL Resource data
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS pirl3ReadResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    OUT GT_PIRL3_RESOURCE    *res
)
{
    GT_STATUS       retVal;        /* Functions return value */
    GT_U16          data[9];     /* temporary Data storage */
    GT_U32          tmpData;     /* temporary Data */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA   opData;
    int                i;

    {
      op = PIRL_READ_RESOURCE_1;
    }

    for(i=0; i<9; i++)
    {
        opData.irlPort = irlPort;
        opData.irlRes = irlRes;
        opData.irlReg = i;
        opData.irlData = 0;

        retVal = pirl3OperationPerform(dev, op, &opData);
        if (retVal != GT_OK)
        {
            DBG_INFO(("PIRL OP Failed.\n"));
            return retVal;
        }

        data[i] = (GT_U16)opData.irlData;
    }

    /* reg0 data */
    res->bktTypeMask = data[0];   
	
    /* reg1 data */
    res->priSelect = data[1]&0xff;         /* Bit[7:0]  : Priority Queue Select */
    res->framePrio = (data[1]>>8)&1;        /* Bit[8] : Frame Priority*/
    res->priORpt = (data[1]>>9)&1;          /* Bit[9] : Priority OR PacketType */
    res->accountFiltered = (data[1]>>10)&1; /* Bit[10] : Account for All */
    res->accountQConf = (data[1]>>11)&1;    /* Bit[11] : account discarded frames due to queue congestion */
    res->acct4GrnOvflow = (data[1]>>12)&1;  /* Bit[12] : Account for Green Overflow */
    res->colorAware = (data[1]>>13)&1;      /* Bit[13] : Color Aware */
    res->samplingMode = (data[1]>>14)&1;    /* Bit[14] : Sampling Mode */
	
    /* reg2 data */
    res->bktIncrement = data[2]&0x1fff;      /* Bit[12:0] : Bucket Increment */
    res->byteTobeCounted = (data[2]>>14)&3;  /* Bit[15:14] : Bytes to be counted */
    res->tcamFlow = (data[2]>>13)&1;         /* Bit[13] : Account QConf */
    
    /* reg3 data */
    res->bktRateFactorGreen = data[3];    /* Bit[15:0] : Bucket Rate Factor */

    /* reg4 data */
    tmpData = data[5]&0xFF;    /* CBS Limit[23:16] */
	tmpData <<= 16;
    tmpData |= data[4];       /* CBS Limit[15:0] */
	res->cbsLimit = tmpData; 
 
    /* reg5 data */

    /* reg6 data */
    res->bktRateFactorYel = data[6];     /* Bit[15:0] : Bucket Rate Factor Yellow*/ 
	
    /* reg7 data */
    tmpData = data[8]&0xFF;       /* EBS Limit[23:16] */
	tmpData <<= 16;
    tmpData |= data[7];       /* EBS Limit[15:0] */
	res->ebsLimit = tmpData;      

    /* reg8 data */
    res->daNrlEn = (data[8]>>8)&1;        /* Bit[8]  : DA Nrl En */
    res->saNrlEn = (data[8]>>9)&1;        /* Bit[9]  : SA Nrl En */
    res->mgmtNrlEn = (data[8]>>10)&1;     /* Bit[10]  : MGMT Nrl En */
    res->fcAction = (data[8]>>11)&1;      /* Bit[11] : Flow control action */
    res->fcMode = (data[8]>>12)&1;        /* Bit[12] : Flow control mode */
    res->fcPriority = (data[8]>>13)&7;    /* Bit[15:13] : Flow control Priority */

    return GT_OK;
}


/*******************************************************************************
* pirl3WriteTSMResource
*
* DESCRIPTION:
*         This function writes IRL Resource to BCM (Bucket Configuration Memory)
*        in Time Slot Metering Mode.
*
* INPUTS:
*        irlPort - physical port number.
*        irlRes  - bucket to be used (0 ~ 2).
*        res     - IRL Resource data
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK on success,
*        GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS pirl3WriteTSMResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    IN  GT_PIRL3_TSM_RESOURCE    *res
)
{
    GT_STATUS       retVal;            /* Functions return value */
    GT_U16          data[9];     /* temporary Data storage */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA     opData;
    int                i;

	irlRes +=5;    /* ITSM res: 5 ~ 7 */
    op = PIRL_WRITE_RESOURCE;

    /* reg0 data */
    data[0] = 0;

    /* reg1 data */
    data[1] = (GT_U16)((res->itsmModeEn << 15) | (res->priSelect));

    /* reg2 data */
    data[2] = 0;

    /* reg3 data */
    data[3] = 0;                            /* Bit[3:0] : Bytes to be counted */

    /* reg4 data */
    data[4] = (GT_U16)(res->cbsLimit&0xFFFF);      /* Bit[15:0] : CBS Limit[15:0] */
 
    /* reg5 data */

    data[5] = (GT_U16)(res->cbsLimit >> 16);    /* Bit[8] : CBS Limit[23:16] */
    data[5] |= (res->ctsIntv << 12) ;
    data[5] |= (res->stopAll << 11) ;
    data[5] |= (res->allStoped <<10) ;
    /* reg6 data */
    data[6] = 0;     /* Bit[15:0] : Bucket Rate Factor Yellow*/ 
	
    /* reg7 data */
    data[7] = (GT_U16)(res->ebsLimit&0xFFFF);      /* Bit[15:0] : EBS Limit[15:0] */

    /* reg8 data */
    data[8] = (GT_U16)(res->ebsLimit >> 16);          /* Bit[15:0] : EBS Limit[15:0] */

    for(i=0; i<9; i++)
    {
        opData.irlPort = irlPort;
        opData.irlRes = irlRes;
        opData.irlReg = i;
        opData.irlData = data[i];

        retVal = pirl3OperationPerform(dev, op, &opData);
        if (retVal != GT_OK)
        {
            DBG_INFO(("PIRL OP Failed.\n"));
            return retVal;
        }
    }

    return GT_OK;    
}


/*******************************************************************************
* pirl3ReadTSMResource
*
* DESCRIPTION:
*        This function reads IRL Resource from BCM (Bucket Configuration Memory)
*        in Time Slot Metering Mode.
*
* INPUTS:
*        irlPort  - physical port number.
*        irlRes   - bucket to be used (0 ~ 2).
*
* OUTPUTS:
*        res - IRL Resource data
*
* RETURNS:
*         GT_OK on success,
*         GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS pirl3ReadTSMResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    OUT GT_PIRL3_TSM_RESOURCE    *res
)
{
    GT_STATUS       retVal;        /* Functions return value */
    GT_U16          data[9];     /* temporary Data storage */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA    opData;
    int                i;

 	irlRes +=5;    /* ITSM res: 5 ~ 7 */
    {
      op = PIRL_READ_RESOURCE_1;
    }

    for(i=0; i<9; i++)
    {
        opData.irlPort = irlPort;
        opData.irlRes = irlRes;
        opData.irlReg = i;
        opData.irlData = 0;

        retVal = pirl3OperationPerform(dev, op, &opData);
        if (retVal != GT_OK)
        {
            DBG_INFO(("PIRL OP Failed.\n"));
            return retVal;
        }

        data[i] = (GT_U16)opData.irlData;
    }
    
    res->itsmModeEn = (data[1]<<15);

    if(res->itsmModeEn == GT_FALSE)
    {
        /* TMS mode is not set */
        res->cbsLimit = 0;
        res->ebsLimit = 0;
        res->priSelect = 0;
        res->ctsIntv = 0;

        return GT_OK;
    }

    /* reg1 data */
    res->itsmModeEn  =  (data[1] >> 15);
	res->priSelect =  data[1] & 0xff;
    /* reg4,5 data */
	res->cbsLimit =   ((data[5] & 0xFF));
	res->cbsLimit =   (data[4]) | (res->cbsLimit << 16);

    /* reg5 data */
    res->ctsIntv = (data[5]>>12) ;
    res->stopAll = (data[5]>>11)&1 ;
    res->allStoped = (data[5]>>10)&1 ;
    
    /* reg7 data */
	res->ebsLimit =   ((data[8] & 0xFF));
	res->ebsLimit =   (data[7]) | (res->ebsLimit << 16);
                                                       

     return GT_OK;
}

#define PIRL3_DEBUG
#ifdef PIRL3_DEBUG
/*******************************************************************************
* pirl3DumpResource
*
* DESCRIPTION:
*       This function dumps IRL Resource register values.
*
* INPUTS:
*       irlPort  - physical port number.
*        irlRes   - bucket to be used (0 ~ 4).
*        dataLen  - data size.
*
* OUTPUTS:
*       data - IRL Resource data
*
* RETURNS:
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pirl3DumpResource
(
    IN  GT_QD_DEV              *dev,
    IN    GT_U32                irlPort,
    IN    GT_U32                irlRes,
    IN    GT_U32                dataLen,
    OUT GT_U16                *data
)
{
    GT_STATUS       retVal;        /* Functions return value */
    GT_PIRL3_OPERATION    op;
    GT_PIRL3_OP_DATA    opData;
    GT_U32                i;

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PIRL3_RESOURCE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    {
      op = PIRL_READ_RESOURCE_1;
    }


    for(i=0; i<dataLen; i++)
    {
        opData.irlPort = irlPort;
        opData.irlRes = irlRes;
        opData.irlReg = i;
        opData.irlData = 0;

        retVal = pirl3OperationPerform(dev, op, &opData);
        if (retVal != GT_OK)
        {
            DBG_INFO(("PIRL OP Failed.\n"));
            return retVal;
        }

        data[i] = (GT_U16)opData.irlData;
    }

    return GT_OK;    
}
#endif /* PIRL3_DEBUG */

