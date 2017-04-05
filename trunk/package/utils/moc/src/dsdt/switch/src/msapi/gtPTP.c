#include <Copyright.h>

/*******************************************************************************
* gtPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <msApi.h>
#include <gtSem.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>

#undef USE_SINGLE_READ

/****************************************************************************/
/* PTP operation function declaration.                                    */
/****************************************************************************/
extern GT_STATUS ptpOperationPerform
(
    IN   GT_QD_DEV             *dev,
    IN   GT_PTP_OPERATION    ptpOp,
    INOUT GT_PTP_OP_DATA     *opData
);


/*******************************************************************************
* gptpSetConfig
*
* DESCRIPTION:
*       This routine writes PTP configuration parameters.
*
* INPUTS:
*        ptpData  - PTP configuration parameters.
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
GT_STATUS gptpSetConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PTP_CONFIG    *ptpData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_LPORT            port;
    GT_PTP_PORT_CONFIG    ptpPortData;
    GT_PTP_GLOBAL_CONFIG  ptpGlobalData;

    DBG_INFO(("gptpSetConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

#if 1
    ptpGlobalData.ptpEType = ptpData->ptpEType;
    ptpGlobalData.msgIdTSEn = ptpData->msgIdTSEn;
    ptpGlobalData.tsArrPtr = ptpData->tsArrPtr;
    if((retVal = gptpSetGlobalConfig(dev, &ptpGlobalData)) != GT_OK)
    {
        DBG_INFO(("Failed to call gptpSetGlobalConfig.\n"));
        return GT_FAIL;
    }

#else
    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      opData.ptpPort = 0x1F;    /* Global register */
	}
	else
	{
      opData.ptpPort = 0xF;    /* Global register */
	}
    op = PTP_WRITE_DATA;

    /* setting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    opData.ptpData = ptpData->ptpEType;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing PTPEType.\n"));
        return GT_FAIL;
    }

    /* setting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = ptpData->msgIdTSEn;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return GT_FAIL;
    }

    /* setting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = ptpData->tsArrPtr;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TSArrPtr.\n"));
        return GT_FAIL;
    }
#endif

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        ptpPortData.transSpec = ptpData->transSpec;
        ptpPortData.disTSpec = 1;    /* default value */
        ptpPortData.disTSOverwrite = ptpData->disTSOverwrite;
        ptpPortData.ipJump = 2;        /* default value */
        ptpPortData.etJump = 12;    /* default value */

        /* per port configuration */
        for(port=0; port<dev->numOfPorts; port++)
        {
            ptpPortData.ptpArrIntEn = (ptpData->ptpArrIntEn & (1 << port))? GT_TRUE : GT_FALSE;
            ptpPortData.ptpDepIntEn = (ptpData->ptpDepIntEn & (1 << port))? GT_TRUE : GT_FALSE;

#if 0
			ptpPortData.transSpec = ptpData->ptpPortConfig[port].transSpec;
			ptpPortData.disTSpec = ptpData->ptpPortConfig[port].disTSpec;
			ptpPortData.ipJump = ptpData->ptpPortConfig[port].ipJump;
			ptpPortData.etJump = ptpData->ptpPortConfig[port].etJump;
			ptpPortData.disTSOverwrite = ptpData->ptpPortConfig[port].disTSOverwrite;
#endif
			ptpPortData.filterAct = ptpData->ptpPortConfig[port].filterAct;
			ptpPortData.hwAccPtp = ptpData->ptpPortConfig[port].hwAccPtp;
			ptpPortData.kpFramwSa = ptpData->ptpPortConfig[port].kpFramwSa;
			ptpPortData.fullCheck = ptpData->ptpPortConfig[port].fullCheck;
			ptpPortData.noCorrupt = ptpData->ptpPortConfig[port].noCorrupt;
			ptpPortData.extHwAcc = ptpData->ptpPortConfig[port].extHwAcc;
			ptpPortData.arrLedCtrl = ptpData->ptpPortConfig[port].arrLedCtrl;
			ptpPortData.depLedCtrl = ptpData->ptpPortConfig[port].depLedCtrl;

			if((retVal = gptpSetPortConfig(dev, port, &ptpPortData)) != GT_OK)
			{
                DBG_INFO(("Failed gptpSetPortConfig.\n"));
                return GT_FAIL;
			}
			if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
			{
				if(!((ptpData->ptpPortConfig[port].arrTSMode==GT_PTP_TS_MODE_IN_REG)
				||(ptpData->ptpPortConfig[port].arrTSMode==GT_PTP_TS_MODE_IN_RESERVED_2)
				||(ptpData->ptpPortConfig[port].arrTSMode==GT_PTP_TS_MODE_IN_FRAME_END)))
					ptpData->ptpPortConfig[port].arrTSMode=GT_PTP_TS_MODE_IN_REG;
				if((retVal = gptpSetPortTsMode(dev, port, ptpData->ptpPortConfig[port].arrTSMode)) != GT_OK)
				{
					DBG_INFO(("Failed gptpSetPortTsMode.\n"));
					return GT_FAIL;
				}
			}
        }

        return GT_OK;
    }

    /* old PTP block */
    /* setting PTPArrIntEn, offset 3 */
    op = PTP_WRITE_DATA;
    opData.ptpAddr = 3;
    opData.ptpData = GT_LPORTVEC_2_PORTVEC(ptpData->ptpArrIntEn);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing PTPArrIntEn.\n"));
        return GT_FAIL;
    }

    /* setting PTPDepIntEn, offset 4 */
    opData.ptpAddr = 4;
    opData.ptpData = GT_LPORTVEC_2_PORTVEC(ptpData->ptpDepIntEn);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing PTPDepIntEn.\n"));
        return GT_FAIL;
    }

    /* TransSpec, MsgIDStartBit, DisTSOverwrite bit, offset 5 */
    op = PTP_READ_DATA;
    opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    op = PTP_WRITE_DATA;

    opData.ptpData = ((ptpData->transSpec&0xF) << 12) | (opData.ptpData & 0x1) | ((ptpData->disTSOverwrite?1:0) << 1);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing MsgIDStartBit & DisTSOverwrite.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpGetConfig
*
* DESCRIPTION:
*       This routine reads PTP configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpData  - PTP configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_PTP_CONFIG    *ptpData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_LPORT            port;
    GT_PTP_PORT_CONFIG    ptpPortData;
    GT_PTP_GLOBAL_CONFIG  ptpGlobalData;

    DBG_INFO(("gptpGetConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

#if 1
    if((retVal = gptpGetGlobalConfig(dev, &ptpGlobalData)) != GT_OK)
    {
        DBG_INFO(("Failed to call gptpSetGlobalConfig.\n"));
        return GT_FAIL;
    }
    ptpData->ptpEType = ptpGlobalData.ptpEType;
    ptpData->msgIdTSEn = ptpGlobalData.msgIdTSEn;
    ptpData->tsArrPtr = ptpGlobalData.tsArrPtr;

#else
    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      opData.ptpPort = 0x1F;    /* Global register */
	}
	else
	{
      opData.ptpPort = 0xF;    /* Global register */
	}
    op = PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading PTPEType.\n"));
        return GT_FAIL;
    }

    ptpData->ptpEType = opData.ptpData;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return GT_FAIL;
    }

    ptpData->msgIdTSEn = opData.ptpData;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TSArrPtr.\n"));
        return GT_FAIL;
    }

    ptpData->tsArrPtr = opData.ptpData;
#endif

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        ptpData->ptpArrIntEn = 0;
        ptpData->ptpDepIntEn = 0;

        /* per port configuration */
        for(port=0; port<dev->numOfPorts; port++)
        {
            if((retVal = gptpGetPortConfig(dev, port, &ptpPortData)) != GT_OK)
            {
                DBG_INFO(("Failed gptpGetPortConfig.\n"));
                return GT_FAIL;
            }

            ptpData->ptpArrIntEn |= (ptpPortData.ptpArrIntEn ? (1 << port) : 0);
            ptpData->ptpDepIntEn |= (ptpPortData.ptpDepIntEn ? (1 << port) : 0);

			ptpData->ptpPortConfig[port].transSpec = ptpPortData.transSpec;
			ptpData->ptpPortConfig[port].disTSpec = ptpPortData.disTSpec;
			ptpData->ptpPortConfig[port].ipJump = ptpPortData.ipJump;
			ptpData->ptpPortConfig[port].etJump = ptpPortData.etJump;
			ptpData->ptpPortConfig[port].disTSOverwrite = ptpPortData.disTSOverwrite;
			ptpData->ptpPortConfig[port].filterAct = ptpPortData.filterAct;
			ptpData->ptpPortConfig[port].hwAccPtp = ptpPortData.hwAccPtp;
			ptpData->ptpPortConfig[port].kpFramwSa = ptpPortData.kpFramwSa;
			ptpData->ptpPortConfig[port].fullCheck = ptpPortData.fullCheck;
			ptpData->ptpPortConfig[port].noCorrupt = ptpPortData.noCorrupt;
			ptpData->ptpPortConfig[port].extHwAcc = ptpPortData.extHwAcc;
			ptpData->ptpPortConfig[port].arrLedCtrl = ptpPortData.arrLedCtrl;
			ptpData->ptpPortConfig[port].depLedCtrl = ptpPortData.depLedCtrl;

          if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
		  {
            if((retVal = gptpGetPortTsMode(dev, port, &ptpData->ptpPortConfig[port].arrTSMode)) != GT_OK)
            {
                DBG_INFO(("Failed gptpGetPortConfig.\n"));
                return GT_FAIL;
            }
		  }
        }

        ptpData->transSpec = ptpPortData.transSpec;
        ptpData->disTSOverwrite = ptpPortData.disTSOverwrite;

        ptpData->msgIdStartBit = 4;

        return GT_OK;
    }

    /* old PTP block */
    /* getting PTPArrIntEn, offset 3 */
    op = PTP_READ_DATA;
    opData.ptpAddr = 3;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading PTPArrIntEn.\n"));
        return GT_FAIL;
    }
    opData.ptpData &= dev->validPortVec;
    ptpData->ptpArrIntEn = GT_PORTVEC_2_LPORTVEC(opData.ptpData);


    /* getting PTPDepIntEn, offset 4 */
    opData.ptpAddr = 4;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading PTPDepIntEn.\n"));
        return GT_FAIL;
    }

    opData.ptpData &= dev->validPortVec;
    ptpData->ptpDepIntEn = GT_PORTVEC_2_LPORTVEC(opData.ptpData);

    /* MsgIDStartBit, DisTSOverwrite bit, offset 5 */
    opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    ptpData->transSpec = (opData.ptpData >> 12) & 0xF;
    ptpData->msgIdStartBit = 0;
    ptpData->disTSOverwrite = ((opData.ptpData >> 1) & 0x1) ? GT_TRUE : GT_FALSE;

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpSetGlobalConfig
*
* DESCRIPTION:
*       This routine writes PTP global configuration parameters.
*
* INPUTS:
*        ptpData  - PTP global configuration parameters.
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
GT_STATUS gptpSetGlobalConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PTP_GLOBAL_CONFIG    *ptpData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpSetGlobalConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      opData.ptpPort = 0x1F;    /* Global register */
	}
	else
	{
      opData.ptpPort = 0xF;    /* Global register */
	}
    op = PTP_WRITE_DATA;

    /* setting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    opData.ptpData = ptpData->ptpEType;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing PTPEType.\n"));
        return GT_FAIL;
    }

    /* setting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = ptpData->msgIdTSEn;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return GT_FAIL;
    }

    /* setting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = ptpData->tsArrPtr;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TSArrPtr.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpGetGlobalConfig
*
* DESCRIPTION:
*       This routine reads PTP global configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetGlobalConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_PTP_GLOBAL_CONFIG    *ptpData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpGetGlobalConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      opData.ptpPort = 0x1F;    /* Global register */
	}
	else
	{
      opData.ptpPort = 0xF;    /* Global register */
	}
    op = PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading PTPEType.\n"));
        return GT_FAIL;
    }

    ptpData->ptpEType = opData.ptpData;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return GT_FAIL;
    }

    ptpData->msgIdTSEn = opData.ptpData;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TSArrPtr.\n"));
        return GT_FAIL;
    }

    ptpData->tsArrPtr = opData.ptpData;

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpSetPortConfig
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortConfig
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_PTP_PORT_CONFIG    *ptpData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32          hwPort;         /* the physical port number     */
    GT_PORT_STP_STATE  state;

    DBG_INFO(("gptpSetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (ptpData->transSpec > 0xF)    /* 4 bits */
        return GT_BAD_PARAM;

    if (ptpData->etJump > 0x1F)    /* 5 bits */
        return GT_BAD_PARAM;

    if (ptpData->ipJump > 0x3F)    /* 6 bits */
        return GT_BAD_PARAM;

     /* Port should be disabled before Set Force Flow Control bit */
    retVal = gstpGetPortState(dev,port, &state);
    if(retVal != GT_OK)
    {
        DBG_INFO(("gstpGetPortState failed.\n"));
        return retVal;
    }

    retVal = gstpSetPortState(dev,port, GT_PORT_DISABLE);
    if(retVal != GT_OK)
    {
        DBG_INFO(("gstpSetPortState failed.\n"));
        return retVal;
    }

   
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    opData.ptpPort = hwPort;

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    op = PTP_READ_DATA;
    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    op = PTP_WRITE_DATA;
	opData.ptpData = (ptpData->transSpec << 12) | (opData.ptpData & 0x1) |
                    ((ptpData->disTSpec?1:0) << 11) |
                    ((ptpData->disTSOverwrite?1:0) << 1);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
        return GT_FAIL;
    }

    /* setting etJump and ipJump, offset 1 */
    opData.ptpAddr = 1;
    opData.ptpData = (ptpData->ipJump << 8) | ptpData->etJump;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing MsgIDTSEn.\n"));
        return GT_FAIL;
    }

    /* setting Int, offset 2 */
    opData.ptpAddr = 2;
    opData.ptpData = (ptpData->ptpArrIntEn?1:0) |
                    ((ptpData->ptpDepIntEn?1:0) << 1);
    if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
    {
       opData.ptpData |= ((ptpData->arrTSMode&0xff) << 8);  /* from Agate to set ArrTSMode */
    }

    if (IS_IN_DEV_GROUP(dev, DEV_LED_FILTER))
    {
      opData.ptpData |= ((ptpData->filterAct?1:0) << 7); /* Filter 802.1 Act from LEDs */
	}

    if (IS_IN_DEV_GROUP(dev, DEV_HW_ACC_PTP))
	{
		opData.ptpData |= ((ptpData->hwAccPtp?1:0) << 6);  /* Hardware Accelerated PTP */
		opData.ptpData |= ((ptpData->kpFramwSa?1:0) << 5); /* KS = Keep Frame’s SA */
		opData.ptpData |= ((ptpData->fullCheck?1:0) << 4); /* Full check  */
		opData.ptpData |= ((ptpData->noCorrupt?1:0) << 3); /* do not corrupt */
		{
			opData.ptpData |= ((ptpData->extHwAcc?1:0) << 2);/* External Hardware Acceleration */
		}
	}

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TSArrPtr.\n"));
        return GT_FAIL;
    }

    /* setting Led control, offset 3 */
    if (IS_IN_DEV_GROUP(dev, DEV_LED_CONTROL))
    {
      opData.ptpAddr = 3;
      opData.ptpData = (ptpData->arrLedCtrl<<8) |
                       (ptpData->depLedCtrl);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing LED control.\n"));
        return GT_FAIL;
	  }
    }
    /* Restore original stp state. */
    if(gstpSetPortState(dev,port, state) != GT_OK)
    {
        DBG_INFO(("gstpSetPortState failed.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpGetPortConfig
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortConfig
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_PTP_PORT_CONFIG    *ptpData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32          hwPort;         /* the physical port number     */

    DBG_INFO(("gptpGetPortConfig Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    opData.ptpPort = hwPort;
    op = PTP_READ_DATA;

    /* TransSpec, DisTSpecCheck, DisTSOverwrite bit, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading port config register.\n"));
        return GT_FAIL;
    }

    ptpData->transSpec = opData.ptpData >> 12;
    ptpData->disTSpec = ((opData.ptpData >> 11) & 0x1) ? GT_TRUE : GT_FALSE;
    ptpData->disTSOverwrite = ((opData.ptpData >> 1) & 0x1) ? GT_TRUE : GT_FALSE;

    /* getting MsgIDTSEn, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading MsgIDTSEn.\n"));
        return GT_FAIL;
    }

    ptpData->ipJump = (opData.ptpData >> 8) & 0x3F;
    ptpData->etJump = opData.ptpData & 0x1F;

    /* getting TSArrPtr, offset 2 */
    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TSArrPtr.\n"));
        return GT_FAIL;
    }

    ptpData->ptpDepIntEn = ((opData.ptpData >> 1) & 0x1) ? GT_TRUE : GT_FALSE;
    ptpData->ptpArrIntEn = (opData.ptpData & 0x1) ? GT_TRUE : GT_FALSE;
    if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
    {
      ptpData->arrTSMode = (opData.ptpData &0xff00) >> 8;  /* from Agate to get ArrTSMode */
    }
    if (IS_IN_DEV_GROUP(dev, DEV_LED_FILTER))
    {
      ptpData->filterAct = ((opData.ptpData >> 7 ) & 0x1) ? GT_TRUE : GT_FALSE;
	}
    if (IS_IN_DEV_GROUP(dev, DEV_HW_ACC_PTP))
	{
      ptpData->hwAccPtp = ((opData.ptpData >> 6) & 0x1) ? GT_TRUE : GT_FALSE;
      ptpData->kpFramwSa = ((opData.ptpData >> 5) & 0x1) ? GT_TRUE : GT_FALSE;
      ptpData->fullCheck = ((opData.ptpData >> 4) & 0x1) ? GT_TRUE : GT_FALSE;
      ptpData->noCorrupt = ((opData.ptpData >> 3) & 0x1) ? GT_TRUE : GT_FALSE;
	  if (IS_IN_DEV_GROUP(dev, DEV_EXT_HW_ACC_PTP))
	  {
		ptpData->extHwAcc = ((opData.ptpData >> 2) & 0x1) ? GT_TRUE : GT_FALSE;
	  }
	}
    
    /* getting Led control, offset 3 */
    if (IS_IN_DEV_GROUP(dev, DEV_LED_CONTROL))
    {
      opData.ptpAddr = 3;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading LED control.\n"));
	  }

      ptpData->arrLedCtrl = qdLong2Char((opData.ptpData &0xff00) >> 8);  
      ptpData->depLedCtrl = qdLong2Char(opData.ptpData &0xff);  /* from Agate to get ArrTSMode */
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpSetPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP.
*
* INPUTS:
*        en - GT_TRUE to enable PTP, GT_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPTPEn
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        en
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_LPORT            port;

    DBG_INFO(("gptpSetPTPEn Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))  
    {
        /* per port configuration */
        for(port=0; port<dev->numOfPorts; port++)
        {
            if((retVal = gptpSetPortPTPEn(dev, port, en)) != GT_OK)
            {
                DBG_INFO(("Failed gptpSetPortPTPEn.\n"));
                return GT_FAIL;
            }
        }

        return GT_OK;
    }

    /* old PTP block */
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      opData.ptpPort = 0x1F;    /* Global register */
	}
	else
	{
      opData.ptpPort = 0xF;    /* Global register */
	}
    op = PTP_READ_DATA;
    opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    op = PTP_WRITE_DATA;
    opData.ptpData &= ~0x1;
    opData.ptpData |= (en ? 0 : 1);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing MsgIDStartBit & DisTSOverwrite.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpGetPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        en - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPEn
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *en
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpGetPTPEn Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        if((retVal = gptpGetPortPTPEn(dev, 0, en)) != GT_OK)
        {
               DBG_INFO(("Failed gptpGetPortPTPEn.\n"));
            return GT_FAIL;
        }

        return GT_OK;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      opData.ptpPort = 0x1F;    /* Global register */
	}
	else
	{
      opData.ptpPort = 0xF;    /* Global register */
	}
    op = PTP_READ_DATA;
    opData.ptpAddr = 5;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    *en = (opData.ptpData & 0x1) ? GT_FALSE : GT_TRUE;

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpSetPortPTPEn
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*        en - GT_TRUE to enable PTP, GT_FALSE to disable PTP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortPTPEn
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_BOOL        en
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32            hwPort;

    DBG_INFO(("gptpSetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpAddr = 0;

    opData.ptpPort = hwPort;

    op = PTP_READ_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    op = PTP_WRITE_DATA;

    if (en)
        opData.ptpData &= ~0x1;
    else
        opData.ptpData |= 0x1;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TransSpec,DisTSpecCheck,DisTSOverwrite.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpGetPortPTPEn
*
* DESCRIPTION:
*       This routine checks if PTP is enabled on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        en - GT_TRUE if enabled, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortPTPEn
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_BOOL        *en
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32            hwPort;

    DBG_INFO(("gptpGetPortPTPEn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    opData.ptpAddr = 0;
    opData.ptpPort = hwPort;

    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    *en = (opData.ptpData & 0x1) ? GT_FALSE : GT_TRUE;

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpSetPortTsMode
*
* DESCRIPTION:
*       This routine set PTP arrive 0 TS mode on a port.
*
* INPUTS:
*        tsMod - GT_PTP_TS_MODE_IN_REG
*                GT_PTP_TS_MODE_IN_RESERVED_2
*                GT_PTP_TS_MODE_IN_FRAME_END
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetPortTsMode
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    IN  GT_PTP_TS_MODE  tsMode 
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32            hwPort;

    DBG_INFO(("gptpSetPortTsMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (!(IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpAddr = 2;

    opData.ptpPort = hwPort;

    op = PTP_READ_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TsMode.\n"));
        return GT_FAIL;
    }

    op = PTP_WRITE_DATA;

    opData.ptpData &= 0xff;
    switch (tsMode)
    {
      case GT_PTP_TS_MODE_IN_REG:
        break;
      case GT_PTP_TS_MODE_IN_RESERVED_2:
        opData.ptpData |= (PTP_TS_LOC_RESERVED_2<<8); /* set TS in reserved 1 */
        break;
      case GT_PTP_TS_MODE_IN_FRAME_END:
        opData.ptpData |= (PTP_FRAME_SIZE<<8); /* set TS in end of PTP frame */
        break;
      default:
        DBG_INFO(("GT_NOT_SUPPORTED the TS mode\n"));
        return GT_NOT_SUPPORTED;
    }

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing Ts Mode.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpGetPortTsMode
*
* DESCRIPTION:
*       This routine get PTP arrive 0 TS mode on a port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        tsMod - GT_PTP_TS_MODE_IN_REG
*                GT_PTP_TS_MODE_IN_RESERVED_2
*                GT_PTP_TS_MODE_IN_FRAME_END
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPortTsMode
(
    IN  GT_QD_DEV     *dev,
    IN    GT_LPORT    port,
    OUT GT_PTP_TS_MODE  *tsMode 
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32            hwPort;
    GT_U16            tmpData;

    DBG_INFO(("gptpGetPortTsMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (!(IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpAddr = 2;

    opData.ptpPort = hwPort;

    op = PTP_READ_DATA;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TsMode.\n"));
        return GT_FAIL;
    }

    tmpData = qdLong2Short(opData.ptpData >>8);
    if (tmpData>=PTP_FRAME_SIZE)
      *tsMode = GT_PTP_TS_MODE_IN_FRAME_END;
    else if (tmpData == PTP_TS_LOC_RESERVED_2)
      *tsMode = GT_PTP_TS_MODE_IN_RESERVED_2;
    else
      *tsMode = GT_PTP_TS_MODE_IN_REG;

    DBG_INFO(("OK.\n"));
    return GT_OK;
}
 
/*******************************************************************************
* gptpSetMeanPathDelay
*
* DESCRIPTION:
*		This routine sets ingress mean path delay on a port.
* 
* INPUTS:
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpSetMeanPathDelay
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    IN   GT_U16		delay
)
{
	GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32				hwPort;

    DBG_INFO(("gptpSetMeanPathDelay Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

	if (!IS_IN_DEV_GROUP(dev,DEV_PATH_DELAY_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpAddr = 0x1C;
    opData.ptpPort = hwPort;
	opData.ptpData = delay;
    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing mean path delay.\n"));
        return GT_FAIL;
    }

	DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpGetMeanPathDelay
*
* DESCRIPTION:
*		This routine gets ingress mean path delay on a port.
* 
* INPUTS:
*		None.
*
* OUTPUTS:
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* RETURNS:
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/

GT_STATUS gptpGetMeanPathDelay
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    OUT  GT_U16		*delay
)
{
	GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32				hwPort;

    DBG_INFO(("gptpGetMeanPathDelay Called.\n"));

	/* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);

	if (!IS_IN_DEV_GROUP(dev,DEV_PATH_DELAY_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpAddr = 0x1C;
    opData.ptpPort = hwPort;
    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading mean path delay.\n"));
        return GT_FAIL;
    }

	*delay = qdLong2Short(opData.ptpData);

	DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpSetIngPathDelayAsym
*
* DESCRIPTION:
*		This routine sets ingress path delay asymmetry on a port.
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
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpSetIngPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    IN   GT_U16		asym
)
{
	GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32				hwPort;

    DBG_INFO(("gptpSetIngPathDelayAsym Called.\n"));

	if (!IS_IN_DEV_GROUP(dev,DEV_PATH_DELAY_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
	
	opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
	opData.ptpAddr = 0x1D;
    opData.ptpPort = hwPort;
	opData.ptpData = asym;
    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ingress path delay asymmetry.\n"));
        return GT_FAIL;
    }

	DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpGetIngPathDelayAsym
*
* DESCRIPTION:
*		This routine gets ingress path delay asymmetry on a port.
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
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpGetIngPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    OUT  GT_U16		*asym
)
{
	GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32				hwPort;

    DBG_INFO(("gptpGetIngPathDelayAsym Called.\n"));

	if (!IS_IN_DEV_GROUP(dev,DEV_PATH_DELAY_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
	
	opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
	opData.ptpAddr = 0x1D;
    opData.ptpPort = hwPort;
    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading ingress path delay asymmetry.\n"));
        return GT_FAIL;
    }

	*asym = qdLong2Short(opData.ptpData);

	DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpSetEgPathDelayAsym
*
* DESCRIPTION:
*		This routine sets egress path delay asymmetry on a port.
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
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpSetEgPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    IN   GT_U16		asym
)
{
	GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32				hwPort;

    DBG_INFO(("gptpSetEgPathDelayAsym Called.\n"));

	if (!IS_IN_DEV_GROUP(dev,DEV_PATH_DELAY_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
	
	opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
	opData.ptpAddr = 0x1E;
    opData.ptpPort = hwPort;
	opData.ptpData = asym;
    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writeg egress path delay asymmetry.\n"));
        return GT_FAIL;
    }

	DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpGetEgPathDelayAsym
*
* DESCRIPTION:
*		This routine gets egress path delay asymmetry on a port.
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
*		GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
*
*******************************************************************************/
GT_STATUS gptpGetEgPathDelayAsym
(
	IN   GT_QD_DEV	*dev,
    IN   GT_LPORT	port,
    OUT  GT_U16		*asym
)
{
	GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32				hwPort;

    DBG_INFO(("gptpGetEgPathDelayAsym Called.\n"));

	if (!IS_IN_DEV_GROUP(dev,DEV_PATH_DELAY_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
	
	opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
	opData.ptpAddr = 0x1E;
    opData.ptpPort = hwPort;
    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed readeg egress path delay asymmetry.\n"));
        return GT_FAIL;
    }

	*asym = qdLong2Short(opData.ptpData);

	DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpGetPTPInt
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for each port.
*        The PTP Interrupt bit gets set for a given port when an incoming PTP 
*        frame is time stamped and PTPArrIntEn for that port is set to 0x1.
*        Similary PTP Interrupt bit gets set for a given port when an outgoing
*        PTP frame is time stamped and PTPDepIntEn for that port is set to 0x1.
*        This bit gets cleared upon software reading and clearing the corresponding
*        time counter valid bits that are valid for that port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpInt     - interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPInt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32        *ptpInt
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpGetPTPInt Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      opData.ptpPort = 0x1F;    /* Global register */
      op = PTP_READ_MULTIPLE_DATA;
      opData.nData = 2;
	}
	else
	{
      opData.ptpPort = 0xF;    /* Global register */
      op = PTP_READ_DATA;
	}
    opData.ptpAddr = 8;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_3))
	{
      if(dev->maxPorts<11)
	  {
        opData.ptpMultiData[0] &= (1 << dev->maxPorts) - 1;
        *ptpInt = GT_PORTVEC_2_LPORTVEC(opData.ptpMultiData[0]);
	  }
	  else
	  {
        *ptpInt = opData.ptpMultiData[1];
        *ptpInt <<= 11;
        *ptpInt |= (opData.ptpMultiData[0] & ((1 << 11) - 1));
	  }
    }
	else
	{
      opData.ptpData &= (1 << dev->maxPorts) - 1;
      *ptpInt = GT_PORTVEC_2_LPORTVEC(opData.ptpData);
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpGetPTPIntStatus
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for global and each port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpIntSt   - interrupt status for
*                       ptpTrigGenInt;      Trigger generate mode Interrupt
*                       ptpEventInt;        Event Capture Interrupt 
*                       ptpUpperPortInt;    Upper Ports Interrupt   
*                       ptpIntStPortVect
*                         interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPIntStatus
(
    IN  GT_QD_DEV     *dev,
    OUT GT_PTP_INTERRUPT_STATUS        *ptpIntSt
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpGetPTPIntStatus Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_MULTIPLE_DATA;
    opData.nData = 2;
    opData.ptpAddr = 8;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading PTP status.\n"));
        return GT_FAIL;
    }

    if(dev->maxPorts<11)
	{
      opData.ptpMultiData[0] &= (1 << dev->maxPorts) - 1;
      ptpIntSt->ptpIntStPortVect = GT_PORTVEC_2_LPORTVEC(opData.ptpMultiData[0]);
	}
	else
	{
      ptpIntSt->ptpIntStPortVect = opData.ptpMultiData[1];
      ptpIntSt->ptpIntStPortVect <<= 11;
      ptpIntSt->ptpIntStPortVect |= (opData.ptpMultiData[0] & ((1 << 11) - 1));
	}
    ptpIntSt->ptpTrigGenInt = qdLong2Char((opData.ptpMultiData[0]>>15)&1);
    ptpIntSt->ptpEventInt = qdLong2Char((opData.ptpMultiData[0]>>14)&1);
    ptpIntSt->ptpUpperPortInt = qdLong2Char((opData.ptpMultiData[0]>>11)&1);


    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the global timer value that is running off of the free
*        running switch core clock.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetPTPGlobalTime
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32        *ptpTime
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpGetPTPGlobalTime Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

#ifndef USE_SINGLE_READ
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
	if(IS_IN_DEV_GROUP(dev,DEV_TAI))
	{
		if(IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			opData.ptpPort = 0x1E;
		else
			opData.ptpPort = 0xE;
	}
	else
	{
		opData.ptpPort = 0xF;
	}
	
    op = PTP_READ_MULTIPLE_DATA;
    opData.ptpAddr = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:9;
    opData.nData = 2;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading PTP Global Time.\n"));
        return GT_FAIL;
    }

    *ptpTime = GT_PTP_BUILD_TIME(opData.ptpMultiData[1],opData.ptpMultiData[0]);
#else
    {
    GT_U32 data[2];

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:0xF;    /* Global register */
    op = PTP_READ_DATA;
    opData.ptpAddr = IS_IN_DEV_GROUP(dev,DEV_TAI)?0xE:9;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    data[0] = opData.ptpData;

    op = PTP_READ_DATA;
    opData.ptpAddr++;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    data[1] = opData.ptpData;

    *ptpTime = GT_PTP_BUILD_TIME(data[1],data[0]);

    }
#endif

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpGetTimeStamped
*
* DESCRIPTION:
*        This routine retrieves the PTP port status that includes time stamp value 
*        and sequence Id that are captured by PTP logic for a PTP frame that needs 
*        to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToRead    - Arr0, Arr1, or Dep time (GT_PTP_TIME enum type)
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetTimeStamped
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN    GT_PTP_TIME    timeToRead,
    OUT GT_PTP_TS_STATUS    *ptpStatus
)
{
    GT_STATUS           retVal;
    GT_U32                hwPort;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32                baseReg;

    DBG_INFO(("gptpGetTimeStamped Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (hwPort == GT_INVALID_PORT)
    {
        DBG_INFO(("Invalid port number\n"));
        return GT_BAD_PARAM;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
        baseReg = 8;
    else
        baseReg = 0;

    switch (timeToRead)
    {
        case PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            DBG_INFO(("Invalid time to be read\n"));
            return GT_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

#ifndef USE_SINGLE_READ
    op = PTP_READ_TIMESTAMP_DATA;
    opData.nData = 4;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    ptpStatus->isValid = (opData.ptpMultiData[0] & 0x1) ? GT_TRUE : GT_FALSE;
    ptpStatus->status = (GT_PTP_INT_STATUS)((opData.ptpMultiData[0] >> 1) & 0x3);
    ptpStatus->timeStamped = GT_PTP_BUILD_TIME(opData.ptpMultiData[2],opData.ptpMultiData[1]);
    ptpStatus->ptpSeqId = opData.ptpMultiData[3];
#else
    {
    GT_U32 data[4], i;

    op = PTP_READ_DATA;

    for (i=0; i<4; i++)
    {
        if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
        {
            DBG_INFO(("Failed reading DisPTP.\n"));
            return GT_FAIL;
        }
    
        data[i] = opData.ptpData;
        opData.ptpAddr++;
    }

    ptpStatus->isValid = (data[0] & 0x1) ? GT_TRUE : GT_FALSE;
    ptpStatus->status = (GT_PTP_INT_STATUS)((data[0] >> 1) & 0x3);
    ptpStatus->timeStamped = GT_PTP_BUILD_TIME(data[2],data[1]);
    ptpStatus->ptpSeqId = data[3];

    }
#endif

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gptpResetTimeStamp
*
* DESCRIPTION:
*        This routine resets PTP Time valid bit so that PTP logic can time stamp
*        a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       port         - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (GT_PTP_TIME enum type)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpResetTimeStamp
(
    IN  GT_QD_DEV		*dev,
    IN  GT_LPORT		port,
    IN    GT_PTP_TIME	timeToReset
)
{
    GT_STATUS			retVal;
    GT_U32              hwPort;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;
    GT_U32              baseReg;

    DBG_INFO(("gptpResetTimeStamp Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (hwPort == GT_INVALID_PORT)
    {
        DBG_INFO(("Invalid port number\n"));
        return GT_BAD_PARAM;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
        baseReg = 8;
    else
        baseReg = 0;

    switch (timeToReset)
    {
        case PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + 0;
            break;
        case PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + 4;
            break;
        case PTP_DEP_TIME:
            opData.ptpAddr = baseReg + 8;
            break;
        default:
            DBG_INFO(("Invalid time to reset\n"));
            return GT_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpData = 0;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing Port Status.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpGetTSDiscardCount
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Departure frame discard counter 
*        for PTP frames that need hardware time stamping.
*
* INPUTS:
*       port         - logical port number.
*
* OUTPUTS:
*       discardCount    - discard counters
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetTSDiscardCount
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    OUT GT_PTP_TS_DISCARD_COUNTER    *discardCount
)
{
    GT_STATUS           retVal;
    GT_U32                hwPort;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
/*    GT_U32                baseReg; */

    DBG_INFO(("gptpGetTSDiscardCount Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (hwPort == GT_INVALID_PORT)
    {
        DBG_INFO(("Invalid port number\n"));
        return GT_BAD_PARAM;
    }

    opData.ptpAddr = 0x15;
    opData.ptpPort = hwPort;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    discardCount->depTSDisCount = qdLong2Char((opData.ptpData>>12)&0xf);
    discardCount->depNOTSDisCount = qdLong2Char((opData.ptpData>>8)&0xf);

    if (IS_IN_DEV_GROUP(dev,DEV_PTP_2))
	{
      discardCount->arrTSDisCount = qdLong2Char((opData.ptpData>>4)&0xf);
      discardCount->arrNOTSDisCount = qdLong2Char((opData.ptpData>>0)&0xf);
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpGetOutFCSErr
*
* DESCRIPTION:
*        This routine gets Precise Time Protocol Output CRC Error counter.
*
* INPUTS:
*       port         - logical port number.
*
* OUTPUTS:
*       outFcsErr    - PTP Output CRC Error counter.
*
* RETURNS:
*       GT_OK         - on success
*       GT_FAIL     - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetOutFCSErr
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    OUT GT_U8       *outFcsErr
)
{
    GT_STATUS           retVal;
    GT_U32                hwPort;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
/*    GT_U32                baseReg; */

    DBG_INFO(("gptpGetOutFCSErr Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    hwPort = (GT_U32)GT_LPORT_2_PORT(port);
    if (hwPort == GT_INVALID_PORT)
    {
        DBG_INFO(("Invalid port number\n"));
        return GT_BAD_PARAM;
    }

    opData.ptpAddr = 0x14;
    opData.ptpPort = hwPort;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    *outFcsErr = qdLong2Char((opData.ptpData)&0xff);

    DBG_INFO(("OK.\n"));
    return GT_OK;
}







/*******************************************************************************
* gptpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpGetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    OUT GT_U32        *data
)
{
    GT_STATUS           retVal;
    GT_U32                hwPort;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpGetReg Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    hwPort = (GT_U32)port;

    if (regOffset > 0x1F)
    {
        DBG_INFO(("Invalid reg offset\n"));
        return GT_BAD_PARAM;
    }

    op = PTP_READ_DATA;
    opData.ptpPort = hwPort;
    opData.ptpAddr = regOffset;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    *data = opData.ptpData;

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gptpSetReg
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U32        regOffset,
    IN  GT_U32        data
)
{
    GT_STATUS           retVal;
    GT_U32                hwPort;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpSetReg Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    hwPort = (GT_U32)port;

    if ((regOffset > 0x1F) || (data > 0xFFFF))
    {
        DBG_INFO(("Invalid reg offset/data\n"));
        return GT_BAD_PARAM;
    }

    op = PTP_WRITE_DATA;
    opData.ptpPort = hwPort;
    opData.ptpAddr = regOffset;
    opData.ptpData = data;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }


    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/* gptpGlobalPointDataSet  */
/*******************************************************************************
* gptpGlobalPointDataSet
*
* DESCRIPTION:
*        Set indirect register pointer control data from PTP Global registers. 
*
* INPUTS:
*        point - Pointer to following index registers:
*            GT_PTP_GLOBAL_CONFIG_PTP_MODE_REG                  
*            GT_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM       
*            GT_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC          
*            GT_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC           
*            GT_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC            
*            GT_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC             
*        data -  PTP Global configuration data write to the indirect register 
*                pointed to by the point above.
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
GT_STATUS gptpGlobalPointDataSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    IN  GT_U8             data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U32   count, tmpData;
    DBG_INFO(("gptpGlobalPointDataSet Called.\n"));

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

    /* program QoS Weight Table, 4 sequences at a time */

    /* Wait until the Scratch and Misc control is ready. */
    count = 5;
    tmpData = 0x8000;
    while(tmpData ==0x8000)
    {
        retVal = gptpGetReg(dev, 0x1f, 7, &tmpData);
           if(retVal != GT_OK)
           {
            gtSemGive(dev,dev->tblRegsSem);
               return retVal;
           }
	  if(--count==0) break;
    }

  tmpData =  (GT_U16)((1 << 15) | (point << 8) | data);

  retVal = gptpSetReg(dev, 0x1f, 7, tmpData);
  if(retVal != GT_OK)
  {
     DBG_INFO(("Failed.\n"));
     gtSemGive(dev,dev->tblRegsSem);
     return retVal;
  }

  gtSemGive(dev,dev->tblRegsSem);

  return retVal;

}

/*******************************************************************************
* gptpGlobalPointDataGet
*
* DESCRIPTION:
*        Get indirect register pointer control data from PTP Global registers. 
*
* INPUTS:
*        point - Pointer to following index registers:
*            GT_PTP_GLOBAL_CONFIG_PTP_MODE_REG                  
*            GT_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM       
*            GT_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC          
*            GT_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC           
*            GT_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC            
*            GT_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC             
* OUTPUTS:
*        data -  PTP Global configuration data read from the indirect register 
*                pointed to by the point above.
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
GT_STATUS gptpGlobalPointDataGet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U8             point,
    OUT  GT_U8            *data
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U32            tmpData;
    int count=0x10;
    DBG_INFO(("gptpGlobalPointDataGet Called.\n"));

    gtSemTake(dev,dev->tblRegsSem,OS_WAIT_FOREVER);

	tmpData =  (GT_U16)((point << 8) | 0);
	retVal = gptpSetReg(dev, 0x1f,7, tmpData);
	if(retVal != GT_OK)
	{
	   DBG_INFO(("Failed.\n"));
	   gtSemGive(dev,dev->tblRegsSem);
	   return retVal;
	}
	
    do {
        retVal = gptpGetReg(dev, 0x1f, 7, &tmpData);
        if(retVal != GT_OK)
           {
               DBG_INFO(("Failed.\n"));
            gtSemGive(dev,dev->tblRegsSem);
               return retVal;
        }
		if((count--)==0) return GT_FAIL;
    } while (tmpData&0x8000);

    *data = qdLong2Char(tmpData&0xff);

    gtSemGive(dev,dev->tblRegsSem);


    return retVal;
}

/* PTP Global TOD Operation  */
/*******************************************************************************
* gptpTODLoadPointSet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODLoadPointSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32            loadData
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptpTODLoadPointSet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_WRITE_DATA;
    opData.ptpAddr = 0x10;
    opData.ptpData = qdLong2Short(loadData&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TOD Load Point.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x11;
    opData.ptpData = qdLong2Short((loadData>>16)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TOD Load Point.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpTODLoadPointGet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODLoadPointGet
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U32            *loadData
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptpTODLoadPointGet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_MULTIPLE_DATA;
    opData.nData = 2;
    opData.ptpAddr = 0x10;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TOD Load Point.\n"));
        return GT_FAIL;
    }

    *loadData = opData.ptpMultiData[1];
    *loadData <<= 16;
    *loadData |= (opData.ptpMultiData[0]);

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpTODOpCtrlSet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODOpCtrlSet
(
    IN  GT_QD_DEV             *dev,
    IN  GT_PTP_TOD_OP_CTRL    *todOpCtrl
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32        tmpData;
    int count=0x10;

    DBG_INFO(("gptpTODOpCtrlSet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    do {
        retVal = gptpGetReg(dev, 0x1f, 0x12, &tmpData);
        if(retVal != GT_OK)
           {
               DBG_INFO(("Failed.\n"));
            gtSemGive(dev,dev->tblRegsSem);
               return retVal;
        }
		if((count--)==0) return GT_FAIL;
    } while (tmpData&0x8000);

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_WRITE_DATA;
    opData.ptpAddr = 0x12;
    opData.ptpData = ((1 << 15) | 
		              (((todOpCtrl->ptpOpCtrlTodOp)&7)<<12) | 
                      (((todOpCtrl->ptpOpCtrlTimeArray)&3)<<9) |
                      (((todOpCtrl->ptpOpCtrlClkValid)&1)<<8) |
                      (todOpCtrl->ptpOpCtrlDomainNum&0xff));
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD operation control.\n"));
        return GT_FAIL;
    }

  return retVal;

}

/*******************************************************************************
* gptpTODOpCtrlGet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODOpCtrlGet
(
    IN  GT_QD_DEV             *dev,
    OUT  GT_PTP_TOD_OP_CTRL    *todOpCtrl
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptpTODOpCtrlGet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_DATA;
    opData.ptpAddr = 0x12;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }
	todOpCtrl->ptpOpCtrlTodOp = qdLong2Char((opData.ptpData>>12)&7); 
    todOpCtrl->ptpOpCtrlTimeArray = qdLong2Char((opData.ptpData>>9)&3);
    todOpCtrl->ptpOpCtrlClkValid = qdLong2Char((opData.ptpData>>8)&1);
    todOpCtrl->ptpOpCtrlDomainNum = qdLong2Char(opData.ptpData&0xff);


    return retVal;
}

/*******************************************************************************
* gptpPulseSet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpPulseSet
(
    IN  GT_QD_DEV             *dev,
    IN  GT_PTP_PULSE_STRUCT    *pulsePara
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gptpPulseSet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_WRITE_DATA;
    opData.ptpAddr = 0x1E;
    opData.ptpData = ((((pulsePara->ptpPulseWidth)&0xf)<<12) | 
                      (((pulsePara->ptp1ppsWidthRange)&7)<<8) |
                      (((pulsePara->ptp1ppsPhase)&1)<<3) |
                      (pulsePara->ptp1ppsSelect&0x3));
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing Pulse parameters.\n"));
        return GT_FAIL;
    }

  return retVal;

}

/*******************************************************************************
* gptpPulseGet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpPulseGet
(
    IN  GT_QD_DEV             *dev,
    OUT  GT_PTP_PULSE_STRUCT    *pulsePara
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptpPulseGet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_DATA;
    opData.ptpAddr = 0x1E;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading Pulse parameters.\n"));
        return GT_FAIL;
    }
	pulsePara->ptpPulseWidth = qdLong2Char((opData.ptpData>>12)&0xF); 
    pulsePara->ptp1ppsWidthRange = qdLong2Char((opData.ptpData>>8)&7);
    pulsePara->ptp1ppsPhase = qdLong2Char((opData.ptpData>>3)&1);
    pulsePara->ptp1ppsSelect = qdLong2Char(opData.ptpData&0x3);

    return retVal;
}

/*******************************************************************************
* gptpTODTimeSet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODTimeSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_PTP_TS_STRUCT    *ts
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptpTODTimeSet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_WRITE_DATA;

    opData.ptpAddr = 0x13;
    opData.ptpData = ts->nanoseconds&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x14;
    opData.ptpData = (ts->nanoseconds>>16)&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x15;
    opData.ptpData = ts->seconds&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x16;
    opData.ptpData = (ts->seconds>>16)&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x17;
    opData.ptpData = (ts->topseconds)&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gptpTODTimeGet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODTimeGet
(
    IN  GT_QD_DEV         *dev,
    OUT GT_PTP_TS_STRUCT    *ts
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptpTODLoadPointGet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;
    opData.ptpAddr = 0x13;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_DATA;
    opData.ptpAddr = 0x17;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading ToD.\n"));
        return GT_FAIL;
    }


    ts->nanoseconds = opData.ptpMultiData[1]&0xffff;
    ts->nanoseconds <<= 16;
    ts->nanoseconds |= (opData.ptpMultiData[0]&0xffff);
    ts->seconds = opData.ptpMultiData[3]&0xffff;
    ts->seconds <<= 16;
    ts->seconds |= (opData.ptpMultiData[2]&0xffff);
    ts->topseconds = (GT_U16)(opData.ptpData&0xffff);

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptpTODCompSet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODCompSet
(
    IN  GT_QD_DEV	*dev,
    IN  GT_U32		comp
)
{
    GT_STATUS			retVal;         /* Functions return value.      */
    GT_PTP_OPERATION	op;
    GT_PTP_OP_DATA		opData;

    DBG_INFO(("gptpPulseSet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_WRITE_DATA;
    opData.ptpAddr = 0x1C;
    opData.ptpData = (comp & 0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD compensation.\n"));
        return GT_FAIL;
    }

	opData.ptpAddr = 0x1D;
	opData.ptpData = ((comp >> 16) & 0xffff);
	if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing ToD compensation.\n"));
        return GT_FAIL;
    }

	return retVal;
}

/*******************************************************************************
* gptpTODCompGet
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpTODCompGet
(
    IN   GT_QD_DEV	*dev,
    OUT  GT_U32		*comp
)
{
    GT_STATUS			retVal;         /* Functions return value.      */
    GT_PTP_OPERATION	op;
    GT_PTP_OP_DATA		opData;

    DBG_INFO(("gptpPulseSet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_MULTIPLE_DATA;
	opData.nData = 2;
    opData.ptpAddr = 0x1C;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading ToD compensation.\n"));
        return GT_FAIL;
    }

	*comp = (opData.ptpMultiData[1] & 0xffff) << 16;
	*comp |= opData.ptpMultiData[0];

	return retVal;
}


/*******************************************************************************
* gptp1722TimeSet
*
* DESCRIPTION:
*        Set Time Array 1722 Time of Day. 
*        The four 1722 ToD registers (at PTP Global offsets 0x18 to 0x1B) 
*        contain an 8 byte representation of time used in IEEE 1722 frames 
*        (IEEE1722 uses only the lower 32-bits of this time. The 64-bit 
*        representation is used in PCI-e and it is a simple extension of the 
*        IEEE 1722 representation of time that wraps). These registers are used
*        to load this representation of time into the selected Time Array on 
*        ToD Store All Registers operations. They contain the selected Time 
*        Array’s representation of this time after ToD Capture operations complete.*
*
* INPUTS:
*        ts_hi - High 4 bytes of Full 1722 TOD time structure (8 bytes).
*        ts_lo - Low 4 bytes of Full 1722 TOD time structure (8 bytes).
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
GT_STATUS gptp1722TimeSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_U32   ts_hi,
    IN  GT_U32   ts_lo
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptp1722TimeSet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_WRITE_DATA;

    opData.ptpAddr = 0x18;
    opData.ptpData = ts_lo&0xffff;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing 1722 ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x19;
    opData.ptpData = ((ts_lo>>16)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing 1722 ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x1a;
    opData.ptpData = ((ts_hi)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing 1722 ToD.\n"));
        return GT_FAIL;
    }
    opData.ptpAddr = 0x1b;
    opData.ptpData = ((ts_hi>>16)&0xffff);
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing 1722 ToD.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gptp1722TimeGet
*
* DESCRIPTION:
*        Get Time Array 1722 Time of Day. 
*        The four 1722 ToD registers (at PTP Global offsets 0x18 to 0x1B) 
*        contain an 8 byte representation of time used in IEEE 1722 frames 
*        (IEEE1722 uses only the lower 32-bits of this time. The 64-bit 
*        representation is used in PCI-e and it is a simple extension of the 
*        IEEE 1722 representation of time that wraps). These registers are used
*        to load this representation of time into the selected Time Array on 
*        ToD Store All Registers operations. They contain the selected Time 
*        Array’s representation of this time after ToD Capture operations complete.*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        ts_hi - High 4 bytes of Full 1722 TOD time structure (8 bytes).
*        ts_lo - Low 4 bytes of Full 1722 TOD time structure (8 bytes).
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
GT_STATUS gptp1722TimeGet
(
    IN  GT_QD_DEV         *dev,
    OUT GT_U32   *ts_hi,
    OUT GT_U32   *ts_lo
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    DBG_INFO(("gptp1722TimeGet Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    opData.ptpPort = 0x1F;    /* Global register */
    op = PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;
    opData.ptpAddr = 0x18;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading 1722 ToD.\n"));
        return GT_FAIL;
    }

    *ts_hi = opData.ptpMultiData[3];
    *ts_hi <<= 16;
    *ts_hi |= opData.ptpMultiData[2];
    *ts_lo = opData.ptpMultiData[1];
    *ts_lo <<= 16;
    *ts_lo |= (opData.ptpMultiData[0]);

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/* TAI functions */
/*******************************************************************************
*
*
*
********************************************************************************/
/*******************************************************************************
* gtaiSetEventConfig
*
* DESCRIPTION:
*       This routine sets TAI Event Capture configuration parameters.
*
* INPUTS:
*        eventData  - TAI Event Capture configuration parameters.
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
GT_STATUS gtaiSetEventConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_TAI_EVENT_CONFIG    *eventData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetEventConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }

    opData.ptpData &= ~((3<<14)|(1<<8));
    if (eventData->intEn)
        opData.ptpData |= (1 << 8);
    if (eventData->eventOverwrite)
        opData.ptpData |= (1 << 15);
    if (eventData->eventCtrStart)
        opData.ptpData |= (1 << 14);
    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      opData.ptpData &= ~(1<<13);
      if (eventData->eventPhase)
        opData.ptpData |= (1 << 13);
    }
	
    op = PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI global config.\n"));
        return GT_FAIL;
    }
    
	if(IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
	{
     /* getting Capture trigger, offset 9 */
      op = PTP_READ_DATA;
      opData.ptpAddr = 9;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
	  }
      opData.ptpData &= ~(1<<14);
      opData.ptpData |= (eventData->captTrigEvent==GT_TRUE)?0x4000:0x0;

      op = PTP_WRITE_DATA;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
	  }
	}

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gtaiGetEventConfig
*
* DESCRIPTION:
*       This routine gets TAI Event Capture configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_TAI_EVENT_CONFIG    *eventData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiGetEventConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }

    eventData->intEn = (opData.ptpData & (1<<8))?GT_TRUE:GT_FALSE;
    eventData->eventOverwrite = (opData.ptpData & (1<<15))?GT_TRUE:GT_FALSE;
    eventData->eventCtrStart = (opData.ptpData & (1<<14))?GT_TRUE:GT_FALSE;
    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      eventData->eventPhase = (opData.ptpData & (1<<13))?GT_TRUE:GT_FALSE;
    }

	if(IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
	{
     /* getting Capture trigger, offset 9 */
      op = PTP_READ_DATA;
      opData.ptpAddr = 9;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
	  }
      eventData->captTrigEvent = (opData.ptpData & (1<<14))?GT_TRUE:GT_FALSE;
	}
    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gtaiGetEventStatus
*
* DESCRIPTION:
*       This routine gets TAI Event Capture status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        eventData  - TAI Event Capture configuration parameters.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventStatus
(
    IN  GT_QD_DEV     *dev,
    OUT GT_TAI_EVENT_STATUS    *status
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32                 data[2];

    DBG_INFO(("gtaiGetEventStatus Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

    opData.ptpAddr = 9;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }

    status->isValid = (opData.ptpData & (1<<8))?GT_TRUE:GT_FALSE;
    status->eventCtr = opData.ptpData & 0xFF;
    status->eventErr = (opData.ptpData & (1<<9))?GT_TRUE:GT_FALSE;

    if (status->isValid == GT_FALSE)
    {
        return GT_OK;
    }

    opData.ptpAddr = 0x0A;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading event capture register.\n"));
        return GT_FAIL;
    }
    data[0] = opData.ptpData;

    opData.ptpAddr = 0x0B;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading event capture register.\n"));
        return GT_FAIL;
    }
    data[1] = opData.ptpData;

    status->eventTime = GT_PTP_BUILD_TIME(data[1],data[0]);

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gtaiGetEventInt
*
* DESCRIPTION:
*       This routine gets TAI Event Capture Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Event capture
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetEventInt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *intStatus
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiGetEventInt Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1F;    /* TAI register */
      opData.ptpAddr = 8;
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
      opData.ptpAddr = 9;
	}
    op = PTP_READ_DATA;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
      *intStatus = (opData.ptpData & 0x4000)?GT_TRUE:GT_FALSE;
	else
      *intStatus = (opData.ptpData & 0x8000)?GT_TRUE:GT_FALSE;

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gtaiClearEventInt
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiClearEventInt
(
    IN  GT_QD_DEV     *dev
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiClearEventInt Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1F;    /* TAI register */
      opData.ptpAddr = 8;
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
      opData.ptpAddr = 9;
	}
    op = PTP_READ_DATA;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading eventInt.\n"));
        return GT_FAIL;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
      opData.ptpData &= 0xBEFF;
	else
      opData.ptpData &= 0x7EFF;
    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing eventInt.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gtaiSetClockSelect
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetClockSelect
(
    IN  GT_QD_DEV     *dev,
    IN  GT_TAI_CLOCK_SELECT    *clkSelect
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetClockSelect Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

    /* getting Tai clock select*/
	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_AVB_FAMILY))
		opData.ptpAddr = 0x1E;
	else
		opData.ptpAddr = 8;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }
    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpData &= ~(0x5F1F);
      opData.ptpData |= (clkSelect->priRecClkSel)&0x1F;
      opData.ptpData |= (((clkSelect->syncRecClkSel)&0x1F) << 8);
      opData.ptpData |= (((clkSelect->ptpExtClk)&1) << 14);
	}
	else
	{
      opData.ptpData &= ~(0x4077);
      opData.ptpData |= (clkSelect->priRecClkSel)&7;
      opData.ptpData |= (((clkSelect->syncRecClkSel)&7) << 4);
      opData.ptpData |= (((clkSelect->ptpExtClk)&1) << 14);
	}

    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI global config.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gtaiGetClockSelect
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetClockSelect
(
    IN  GT_QD_DEV     *dev,
    OUT  GT_TAI_CLOCK_SELECT    *clkSelect
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;

    DBG_INFO(("gtaiGetClockSelect Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

     /* getting Tai clock select*/
	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_AVB_FAMILY))
		opData.ptpAddr = 0x1E;
	else
		opData.ptpAddr = 8;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      clkSelect->priRecClkSel = qdLong2Char(opData.ptpData&0x1f);
      clkSelect->syncRecClkSel = qdLong2Char((opData.ptpData >> 8) & 0x1f);
      clkSelect->ptpExtClk = (opData.ptpData>> 14) & 1;
	}
	else
	{
      clkSelect->priRecClkSel = qdLong2Char(opData.ptpData&7);
      clkSelect->syncRecClkSel = qdLong2Char((opData.ptpData >> 4) & 7);
      clkSelect->ptpExtClk = (opData.ptpData>> 14) & 1;
	}

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gtaiSetClockConfig
*
* DESCRIPTION:
*       This routine sets clock configuration in TAI.
*
* INPUTS:
*        clkConfig  - TAI clock configuration parameters.
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
GT_STATUS gtaiSetClockConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_TAI_CLOCK_CONFIG    *clkConfig
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetClockConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}

    /* getting Time Stamping Clock Period in pico seconds, offset 1 */
#if 0
    op = PTP_READ_DATA;
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading PTPEType.\n"));
        return GT_FAIL;
    }
#endif


	if (IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		op = PTP_READ_DATA;
		opData.ptpAddr = 0;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRL clock enable.\n"));
			return GT_FAIL;
		}

		opData.ptpData &= ~(1<<10);
		if (clkConfig->irlClkEn)
			opData.ptpData |= (1 << 10);

		op = PTP_WRITE_DATA;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing IRL clock enable.\n"));
			return GT_FAIL;
		}
	}

    opData.ptpData = clkConfig->tsClkPer;
    opData.ptpAddr = 1;
	op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing time stamping clock period.\n"));
        return GT_FAIL;
    }

	if(!IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		/* getting System on a chip Clock Period, offset 6 */
		op = PTP_READ_DATA;
		opData.ptpAddr = 6;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading SoC clk period.\n"));
			return GT_FAIL;
		}
		opData.ptpData &= ~(0x1ff);
		opData.ptpData = clkConfig->soCClkPer;

		op = PTP_WRITE_DATA;

		opData.ptpAddr = 6;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing SoC clk period.\n"));
			return GT_FAIL;
		}

		/* getting System on a Chip Clock Compensation Amount in pico seconds, offset 7 */
		#if 0
		op = PTP_READ_DATA;
		opData.ptpAddr = 7;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading PTPEType.\n"));
			return GT_FAIL;
		}
		#endif
		opData.ptpData = clkConfig->soCClkComp;

		op = PTP_WRITE_DATA;

		opData.ptpAddr = 7;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing SoC clock compensation.\n"));
			return GT_FAIL;
		}
	}
	/* Peridot needs to set IRL clock */
	else
	{
		opData.ptpData = clkConfig->irlClkGenAmt;
		op = PTP_WRITE_DATA;
		opData.ptpAddr = 6;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing IRL clock generation amount.\n"));
			return GT_FAIL;
		}
	
		opData.ptpData = ((clkConfig->compDir == GT_TRUE)?1:0);
		opData.ptpData = (opData.ptpData << 15) | clkConfig->irlClkComp;
		op = PTP_WRITE_DATA;
		opData.ptpAddr = 7;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing IRL clock compensation amount.\n"));
			return GT_FAIL;
		}

		opData.ptpData = (clkConfig->irlClkCompSubPs & 0xff);
		op = PTP_WRITE_DATA;
		opData.ptpAddr = 8;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing IRL clock comp amount in subpico secs.\n"));
			return GT_FAIL;
		}
	}

	DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gtaiGetClockConfig
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetClockConfig
(
    IN  GT_QD_DEV     *dev,
    OUT  GT_TAI_CLOCK_CONFIG    *clkConfig
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA      opData;

    DBG_INFO(("gtaiGetClockConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

	if (IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		opData.ptpAddr = 0;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRL clock enable.\n"));
			return GT_FAIL;
		}
        clkConfig->irlClkEn = (opData.ptpData & (1<<10))?GT_TRUE:GT_FALSE;
	}

    /* getting Time Stamping Clock Period in pico seconds, offset 1 */
    opData.ptpAddr = 1;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading time stamping clock period.\n"));
        return GT_FAIL;
    }
    clkConfig->tsClkPer = qdLong2Short(opData.ptpData);

	if(!IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		/* getting System on a chip Clock Period, offset 6 */
		opData.ptpAddr = 6;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading SoC clock period.\n"));
			return GT_FAIL;
		}
		clkConfig->soCClkPer = qdLong2Short(opData.ptpData&0x1ff);

		/* getting System on a Chip Clock Compensation Amount in pico seconds, offset 7 */
		opData.ptpAddr = 7;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading SoC clock compensation amount.\n"));
			return GT_FAIL;
		}
		clkConfig->soCClkComp = qdLong2Short(opData.ptpData);
	}
	/* Peridot needs to set IRL clock */
	else
	{
		opData.ptpAddr = 6;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRL clock generation amount.\n"));
			return GT_FAIL;
		}
		clkConfig->irlClkGenAmt = qdLong2Short(opData.ptpData&0xffff);

		opData.ptpAddr = 7;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRL clock compensation amount.\n"));
			return GT_FAIL;
		}
		clkConfig->irlClkComp = (GT_U16)(opData.ptpData & 0x7fff);
		clkConfig->compDir = ((opData.ptpData & 0x8000)?1:0);

		opData.ptpAddr = 8;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRL clock compensation sub pico seconds.\n"));
			return GT_FAIL;
		}
		clkConfig->irlClkCompSubPs = qdLong2Char(opData.ptpData&0xff);
	}

    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gtaiGetTrigInt
*
* DESCRIPTION:
*       This routine gets TAI Trigger Interrupt status.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        intStatus     - interrupt status for TAI Trigger
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetTrigInt
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *intStatus
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiGetTrigInt Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1F;    /* Move to PTP Global register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;
    opData.ptpAddr = 8;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    *intStatus = (opData.ptpData & 0x8000)?GT_TRUE:GT_FALSE;

    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/*******************************************************************************
* gtaiClearTrigInt
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiClearTrigInt
(
    IN  GT_QD_DEV     *dev
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiClearTrigInt Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1F;    /* PTP Global register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;
    opData.ptpAddr = 8;
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI global config.\n"));
        return GT_FAIL;
    }

    opData.ptpData &= 0x7fff;
    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/*******************************************************************************
* gtaiSetTrigConfig
*
* DESCRIPTION:
*       This routine sets TAI Trigger configuration parameters.
*
* INPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is GT_TRUE).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL    - on error
*       GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetTrigConfig
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL     trigEn,
    IN  GT_TAI_TRIGGER_CONFIG    *trigData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetTrigConfig Called.\n"));
    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}

    op = PTP_WRITE_DATA;

    opData.ptpAddr = 2;
    opData.ptpData = GT_PTP_L16_TIME(trigData->trigGenAmt);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing Trig generation amount.\n"));
        return GT_FAIL;
    }

    opData.ptpAddr = 3;
    opData.ptpData = GT_PTP_H16_TIME(trigData->trigGenAmt);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing Trig generation amount.\n"));
        return GT_FAIL;
    }

    op = PTP_WRITE_DATA;
    opData.ptpAddr = 4;        /* TrigClkComp */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpData = (trigData->clkCompDir)?0x8000:0;
      opData.ptpData |= (trigData->trigClkComp&=0x7fff);
	}
    else
	{
      opData.ptpData = trigData->trigClkComp;
	}

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
      DBG_INFO(("Failed writing TAI trig compare clock.\n"));
      return GT_FAIL;
    }

    op = PTP_READ_DATA;
    opData.ptpAddr = 5;        /* PulseWidth */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
      DBG_INFO(("Failed reading TAI global config.\n"));
      return GT_FAIL;
    }

    op = PTP_WRITE_DATA;
    opData.ptpAddr = 5;        /* PulseWidth */

    opData.ptpData &= (~0xF000);
    opData.ptpData |= (GT_U16)(trigData->pulseWidth << 12);
    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpData &= ~(0x3ff);
      opData.ptpData |= ((trigData->pulseWidthRange&7)<<8);
      opData.ptpData |= ((trigData->trigClkCompSub&0xff));
	}
	else
	{
      if (trigData->timeDec == GT_TRUE)
        opData.ptpData |= (1 << 11);
	  
	  else
        opData.ptpData &= ~(1 << 11);
      opData.ptpData &= ~(0x3ff);
      opData.ptpData |= trigData->incDecAmt;
	}

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
      DBG_INFO(("Failed writing TAI global config.\n"));
      return GT_FAIL;
    }

	if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
	{
      op = PTP_READ_DATA;
	  if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN_2))
	  {
		opData.ptpAddr = 0x1E;
	  }
	  else
	  {
		opData.ptpAddr = 8;
	  }
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing TAI global config.\n"));
			return GT_FAIL;
		}

		op = PTP_WRITE_DATA;
        if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN_2))
		{
	    	opData.ptpData &= (~0x2080);
	    	opData.ptpData |= (GT_U16)(trigData->trigWindow << 13);
		    opData.ptpData |= (GT_U16)(trigData->trig2Signal << 7);
		}
		else
		{
	    	opData.ptpData &= (~0x2000);
	    	opData.ptpData |= (GT_U16)(trigData->trigWindow << 13);
		}

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing TAI global config.\n"));
			return GT_FAIL;
		}
	}

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN)) /* after 6320 */
    {
      op = PTP_WRITE_DATA;
      opData.ptpAddr = 0x10;
      opData.ptpData = GT_PTP_L16_TIME(trigData->trigGenTime);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing trigger generation time.\n"));
        return GT_FAIL;
	  }

      opData.ptpAddr = 0x11;
      opData.ptpData = GT_PTP_H16_TIME(trigData->trigGenTime);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing trigger generation time.\n"));
        return GT_FAIL;
	  }

      opData.ptpAddr = 0x13;
      opData.ptpData = GT_PTP_L16_TIME(trigData->trigGenDelay);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing trigger generation delay.\n"));
        return GT_FAIL;
	  }

	  /* lockCorrect read only 
      opData.ptpAddr = 0x12;
      opData.ptpData = 0xF & GT_PTP_L16_TIME(trigData->lockCorrect);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing PTPEType.\n"));
        return GT_FAIL;
	  }
      */
	  if (!IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	  {
		opData.ptpAddr = 0x14;
		opData.ptpData = GT_PTP_L16_TIME(trigData->trigGen2Time);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing triGen2 time.\n"));
			return GT_FAIL;
		}

		opData.ptpAddr = 0x15;
		opData.ptpData = GT_PTP_H16_TIME(trigData->trigGen2Time);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing triGen2 time.\n"));
			return GT_FAIL;
		}

		opData.ptpAddr = 0x17;
		opData.ptpData = GT_PTP_L16_TIME(trigData->trigGen2Delay);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing triGen2 delay.\n"));
			return GT_FAIL;
		}
	  }
	  /* Peridot has IRLGen instead of TrigGen2 registers */
	  else
	  {
		opData.ptpAddr = 0x14;
		opData.ptpData = GT_PTP_L16_TIME(trigData->irlGenTime);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing IRLGen time.\n"));
			return GT_FAIL;
		}

		opData.ptpAddr = 0x15;
		opData.ptpData = GT_PTP_H16_TIME(trigData->irlGenTime);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing IRLGen time.\n"));
			return GT_FAIL;
		}
	  }
	  /* lockCorrect2 read only 
      opData.ptpAddr = 0x16;
      opData.ptpData = 0xF & GT_PTP_L16_TIME(trigData->lockCorrect2);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing PTPEType.\n"));
        return GT_FAIL;
	  }
      */
    }

    /* Set Trig control, offset 0 */
    op = PTP_READ_DATA;

    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }
/*
    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      if (trigData->trigPhase)
        opData.ptpData |= (1 << 12);
	  else
        opData.ptpData &= ~(1 << 12);
    }
*/
    if (trigData->intEn == GT_TRUE)
        opData.ptpData |= (1 << 9);
	else
        opData.ptpData &= ~(1 << 9);

    if (trigData->trigPhase == GT_TRUE)
        opData.ptpData |= (1 << 12);
	else
        opData.ptpData &= ~(1 << 12);

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      if (trigData->trigLock == GT_TRUE)
        opData.ptpData |= (1 << 7);
      else
        opData.ptpData &= ~(1 << 7);
      opData.ptpData &= ~(7<<4);
      opData.ptpData |= ((trigData->trigLockRange&7)<<4);
    }

    if (trigData->incDecEn == GT_TRUE)
	{
      if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	  {
        opData.ptpData |= (1 << 11);
	  }
	  else
	  {
        opData.ptpData |= (1 << 3);
	  }
	}
	else
	{
      if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	  {
        opData.ptpData &= ~(1 << 11);
	  }
	  else
	  {
        opData.ptpData &= ~(1 << 3);
	  }
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      if (trigData->blkUpdate == GT_TRUE)
        opData.ptpData |= (1 << 3);
      else
        opData.ptpData &= ~(1 << 3);
    }
    
	if (trigData->multiSyncMode == GT_TAI_MULTI_PTP_SYNC_ENABLE)
        opData.ptpData |= (1 << 2);
	else
        opData.ptpData &= ~(1 << 2);

    if (trigData->mode == GT_TAI_TRIG_ON_GIVEN_TIME)
        opData.ptpData |= (1 << 1);
	else
        opData.ptpData &= ~(1 << 1);

    if (trigEn == GT_TRUE)
    {
      opData.ptpData |= 1;
    }
	else
    {
      opData.ptpData &= ~1;
    }

    op = PTP_WRITE_DATA;
    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI global config.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}

/*******************************************************************************
* gtaiGetTrigConfig
*
* DESCRIPTION:
*       This routine gets TAI Trigger configuration parameters.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*        trigEn    - enable/disable TAI Trigger.
*        trigData  - TAI Trigger configuration parameters (valid only if trigEn is GT_TRUE).
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
GT_STATUS gtaiGetTrigConfig
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL     *trigEn,
    OUT GT_TAI_TRIGGER_CONFIG    *trigData
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

	DBG_INFO(("gtaiGetTrigConfig Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;
    opData.ptpAddr = 3;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI trigGenAmt.\n"));
        return GT_FAIL;
    }
    trigData->trigGenAmt = opData.ptpData;

    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI trigGenAmt.\n"));
        return GT_FAIL;
    }
    trigData->trigGenAmt = (trigData->trigGenAmt)<<16 | opData.ptpData;

    op = PTP_READ_DATA;
   
	opData.ptpAddr = 4;        /* TrigClkComp */
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI TrigClkComp.\n"));
        return GT_FAIL;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
	  trigData->clkCompDir = (opData.ptpData & 0x8000)?1:0;
      trigData->trigClkComp = opData.ptpData &0x7fff;
	}
    else
	{
      trigData->trigClkComp = opData.ptpData;
	}

    op = PTP_READ_DATA;
    opData.ptpAddr = 5;        /* PulseWidth */

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
      DBG_INFO(("Failed reading TAI PulseWidth.\n"));
      return GT_FAIL;
    }

    trigData->pulseWidth = qdLong2Char((opData.ptpData &0xF000)>>12);

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      trigData->pulseWidthRange = qdLong2Char((opData.ptpData>>8)&0x7);
      trigData->trigClkCompSub = opData.ptpData&0xff;
	}
	else
	{
      trigData->timeDec = (opData.ptpData >> 11)&1;
	  trigData->incDecAmt = opData.ptpData&0x3ff;
	}

	if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
	{
		op = PTP_READ_DATA;
	    if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN_2))
			opData.ptpAddr = 0x1E;
		else
			opData.ptpAddr = 8;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI global config.\n"));
			return GT_FAIL;
		}
		trigData->trigWindow = (opData.ptpData & 0x2000)?1:0;
	    if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN_2))
	      trigData->trig2Signal = (opData.ptpData & 0x80)?1:0;
	}

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN)) /* after 6320 */
    {
      op = PTP_READ_DATA;
      opData.ptpAddr = 0x11;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI trigGenTime.\n"));
        return GT_FAIL;
	  }
      trigData->trigGenTime = opData.ptpData;
      opData.ptpAddr = 0x10;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI trigGenTime.\n"));
        return GT_FAIL;
	  }

     trigData->trigGenTime = (trigData->trigGenTime)<<16 | opData.ptpData;

	 opData.ptpAddr = 0x13;

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI trigGenDelay.\n"));
        return GT_FAIL;
	  }
      trigData->trigGenDelay = opData.ptpData;

	  /* lockCorrect read only */
      opData.ptpAddr = 0x12;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI lockCorrect.\n"));
        return GT_FAIL;
	  }
      trigData->lockCorrect = qdLong2Char(opData.ptpData & 0xF);

	  if (!IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	  {
		op = PTP_READ_DATA;
		opData.ptpAddr = 0x15;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI trigGen2Time.\n"));
			return GT_FAIL;
		}
		trigData->trigGen2Time = opData.ptpData;
		opData.ptpAddr = 0x14;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI trigGen2Time.\n"));
			return GT_FAIL;
		}

		trigData->trigGen2Time = (trigData->trigGen2Time)<<16 | opData.ptpData;
		opData.ptpAddr = 0x17;

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI trigGen2Delay.\n"));
			return GT_FAIL;
		}
		trigData->trigGen2Delay = opData.ptpData;

		/* lockCorrect read only */
		opData.ptpAddr = 0x16;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI lockCorrect2.\n"));
			return GT_FAIL;
		}
		trigData->lockCorrect2 = qdLong2Char(opData.ptpData & 0xF);
	  }
	  /* Peridot has IRLGen instead of TrigGen2 registers */
	  else
	  {
		op = PTP_READ_DATA;
		opData.ptpAddr = 0x15;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRLGen Time.\n"));
			return GT_FAIL;
		}
		trigData->irlGenTime = opData.ptpData;
		opData.ptpAddr = 0x14;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRLGen Time.\n"));
			return GT_FAIL;
		}

		trigData->irlGenTime = (trigData->irlGenTime)<<16 | opData.ptpData;

		/* lockCorrect read only */
		opData.ptpAddr = 0x16;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading IRL Lock Correction.\n"));
			return GT_FAIL;
		}
		trigData->irlCorrValid = ((opData.ptpData & 0x10)?GT_TRUE:GT_FALSE);
		trigData->irlLockCorrect = qdLong2Char(opData.ptpData & 0xF);	
	  }
    }
	

    /* Set Trig control, offset 0 */
    op = PTP_READ_DATA;

    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI trig config 0.\n"));
        return GT_FAIL;
    }

    trigData->intEn = ((opData.ptpData >> 9)&1)?GT_TRUE:GT_FALSE;

    trigData->trigPhase = ((opData.ptpData >> 12)&1)?GT_TRUE:GT_FALSE;

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      trigData->trigLock = ((opData.ptpData >> 7)&1)?GT_TRUE:GT_FALSE;
      trigData->trigLockRange = qdLong2Char((opData.ptpData >>4)&7);
    }

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      trigData->incDecEn = ((opData.ptpData >> 11)&1)?GT_TRUE:GT_FALSE;
	}
	else
	{
      trigData->incDecEn = ((opData.ptpData >> 3)&1)?GT_TRUE:GT_FALSE;
	}

	if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      trigData->blkUpdate = ((opData.ptpData >> 3)&1)?GT_TRUE:GT_FALSE;
    }

	trigData->multiSyncMode = (opData.ptpData&4)?GT_TAI_MULTI_PTP_SYNC_ENABLE:GT_TAI_MULTI_PTP_SYNC_DISABLE;
	trigData->mode = (opData.ptpData&2)?GT_TAI_TRIG_ON_GIVEN_TIME:GT_TAI_TRIG_PERIODIC_PULSE;
	*trigEn = (opData.ptpData&1)?GT_TRUE:GT_FALSE;



    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gtaiSetTrigLock
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiSetTrigLock
(
    IN  GT_QD_DEV     *dev,
    INOUT  GT_TAI_TRIG_LOCK_CONFIG    *trigLockCfg
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetTrigLock Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}

    op = PTP_READ_DATA;
    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }

    opData.ptpData &= ~(0xf<<4);

    opData.ptpData |= (trigLockCfg->trigLock==GT_TRUE) ?0x80:0;
    opData.ptpData |= ((trigLockCfg->trigLockRange&0x7)<<4);

    op = PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI global config.\n"));
        return GT_FAIL;
    }
    

      op = PTP_WRITE_DATA;
      opData.ptpAddr = 0x10;
      opData.ptpData = GT_PTP_L16_TIME(trigLockCfg->trigGenTime);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing trigGen time.\n"));
        return GT_FAIL;
	  }

      opData.ptpAddr = 0x11;
      opData.ptpData = GT_PTP_H16_TIME(trigLockCfg->trigGenTime);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing trigGen time.\n"));
        return GT_FAIL;
	  }

	  /* lockCorrect read only 
      opData.ptpAddr = 0x12;
      opData.ptpData = 0xF & GT_PTP_L16_TIME(trigLockCfg->lockCorrect);

      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed writing PTPEType.\n"));
        return GT_FAIL;
	  }
      */
	  if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN_1))
	  {
		opData.ptpAddr = 0x14;
		opData.ptpData = GT_PTP_L16_TIME(trigLockCfg->trigGen2Time);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
		  DBG_INFO(("Failed writing trigGen2 time.\n"));
		  return GT_FAIL;
		}

		opData.ptpAddr = 0x15;
		opData.ptpData = GT_PTP_H16_TIME(trigLockCfg->trigGen2Time);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing trigGen2 time.\n"));
			return GT_FAIL;
		}

		/* lockCorrect2 read only 
		opData.ptpAddr = 0x16;
		opData.ptpData = 0xF & GT_PTP_L16_TIME(trigLockCfg->lockCorrect2);

		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed writing PTPEType.\n"));
			return GT_FAIL;
		}
		*/
	  }
    DBG_INFO(("OK.\n"));
    return GT_OK;
}

/*******************************************************************************
* gtaiGetTrigLock
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
*       GT_OK      - on success
*       GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*       GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS gtaiGetTrigLock
(
    IN  GT_QD_DEV     *dev,
    INOUT  GT_TAI_TRIG_LOCK_CONFIG    *trigLockCfg
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiGetTrigLock Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */
    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}

    op = PTP_READ_DATA;
    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI global config.\n"));
        return GT_FAIL;
    }

    trigLockCfg->trigLock = (opData.ptpData&0x80)?GT_TRUE:GT_FALSE;
    trigLockCfg->trigLockRange = (GT_U8)((opData.ptpData&0x70)>>4);

      op = PTP_READ_DATA;
      opData.ptpAddr = 0x11;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI trigGenTime.\n"));
        return GT_FAIL;
	  }
      trigLockCfg->trigGenTime = opData.ptpData;
      opData.ptpAddr = 0x10;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI trigGenTime.\n"));
        return GT_FAIL;
	  }

     trigLockCfg->trigGenTime = (trigLockCfg->trigGenTime)<<16 | opData.ptpData;

	  /* lockCorrect read only */
      opData.ptpAddr = 0x12;
      if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
	  {
        DBG_INFO(("Failed reading TAI lockCorrect.\n"));
        return GT_FAIL;
	  }
      trigLockCfg->lockCorrect = qdLong2Char(opData.ptpData & 0xF);

	  if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN_1))
	  {
		op = PTP_READ_DATA;
		opData.ptpAddr = 0x15;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI trigGen2Time.\n"));
			return GT_FAIL;
		}
		trigLockCfg->trigGen2Time = opData.ptpData;

		opData.ptpAddr = 0x14;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI trigGen2Time.\n"));
			return GT_FAIL;
		}

		trigLockCfg->trigGen2Time = (trigLockCfg->trigGen2Time)<<16 | opData.ptpData;

		/* lockCorrect read only */
		opData.ptpAddr = 0x16;
		if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
		{
			DBG_INFO(("Failed reading TAI lockCorrect2.\n"));
			return GT_FAIL;
		}
		trigLockCfg->lockCorrect2 = qdLong2Char(opData.ptpData & 0xF);
	  }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/*******************************************************************************
* gtaiGetTSClkPer
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*         None.
*
* OUTPUTS:
*        clk        - time stamping clock period
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiGetTSClkPer
(
    IN  GT_QD_DEV     *dev,
    OUT GT_U32        *clk
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiGetTSClkPer Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

    opData.ptpAddr = 1;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TS clock period.\n"));
        return GT_FAIL;
    }

    *clk = (GT_U32)opData.ptpData;

    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/*******************************************************************************
* gtaiSetTSClkPer
*
* DESCRIPTION:
*         Time Stamping Clock Period in pico seconds.
*        This routine specifies the clock period for the time stamping clock supplied 
*        to the PTP hardware logic.
*        This is the clock that is used by the hardware logic to update the PTP 
*        Global Time Counter.
*
* INPUTS:
*        clk        - time stamping clock period
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiSetTSClkPer
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        clk
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetTSClkPer Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_WRITE_DATA;

    opData.ptpAddr = 1;

    opData.ptpData = (GT_U16)clk; 

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TS clock period.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/*******************************************************************************
* gtaiSetMultiPTPSync
*
* DESCRIPTION:
*         This routine sets Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is GT_TRUE)
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
*         When enabled, gtaiSetTrigConfig, gtaiSetEventConfig, gtaiSetTimeInc,
*        and gtaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
GT_STATUS gtaiSetMultiPTPSync
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL     multiEn,
    IN  GT_32        syncTime
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetMultiPTPSync Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI_MULTI_PTP_SYNC))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI register.\n"));
        return GT_FAIL;
    }

    opData.ptpData &= ~(1 << 2);

    if (multiEn == GT_FALSE)
    {
        op = PTP_WRITE_DATA;

        if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
        {
            DBG_INFO(("Failed writing TAI register.\n"));
            return GT_FAIL;
        }

        return GT_OK;
    }

    opData.ptpData |= (1 << 2);

    op = PTP_WRITE_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI register.\n"));
        return GT_FAIL;
    }

    opData.ptpAddr = 2;
    opData.ptpData = GT_PTP_L16_TIME(syncTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI register.\n"));
        return GT_FAIL;
    }

    opData.ptpAddr = 3;
    opData.ptpData = GT_PTP_H16_TIME(syncTime);

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI register.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gtaiGetMultiPTPSync
*
* DESCRIPTION:
*         This routine sets Multiple PTP device sync mode and sync time (TrigGenAmt).
*        When enabled, the hardware logic detects a low to high transition on the 
*        EventRequest(GPIO) and transfers the sync time into the PTP Global Time
*        register. The EventCapTime is also updated at that instant.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        multiEn        - enable/disable Multiple PTP device sync mode
*        syncTime    - sync time (valid only if multiEn is GT_TRUE)
*
* RETURNS:
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if invalid parameter is given
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         When enabled, gtaiSetTrigConfig, gtaiSetEventConfig, gtaiSetTimeInc,
*        and gtaiSetTimeDec APIs are not operational.
*
*******************************************************************************/
GT_STATUS gtaiGetMultiPTPSync
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL     *multiEn,
    OUT GT_32        *syncTime
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;
    GT_U32                 data[2];

    DBG_INFO(("gtaiGetMultiPTPSync Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI_MULTI_PTP_SYNC))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	}
    op = PTP_READ_DATA;

    /* getting PTPEType, offset 0 */
    opData.ptpAddr = 0;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI register.\n"));
        return GT_FAIL;
    }

    if(!(opData.ptpData & (1 << 2)))
    {
        *multiEn = GT_FALSE;
        *syncTime = 0;
        return GT_OK;
    }

    opData.ptpAddr = 2;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI register.\n"));
        return GT_FAIL;
    }
    data[0] = opData.ptpData;

    opData.ptpAddr = 3;
    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI register.\n"));
        return GT_FAIL;
    }
    data[1] = opData.ptpData;

    *syncTime = GT_PTP_BUILD_TIME(data[1],data[0]);

    DBG_INFO(("OK.\n"));
    return GT_OK;

}


/*******************************************************************************
* gtaiGetTimeIncDec
*
* DESCRIPTION:
*         This routine retrieves Time increment/decrement setup.
*        This amount specifies the number of units of PTP Global Time that need to be 
*        incremented or decremented. This is used for adjusting the PTP Global Time 
*        counter value by a certain amount.
*
* INPUTS:
*         None.
*
* OUTPUTS:
*		 expired    - GT_TRUE if inc/dec occurred, GT_FALSE otherwise
*        inc        - GT_TRUE if increment, GT_FALSE if decrement
*        amount    - increment/decrement amount
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         Time increment or decrement will be excuted once.
*
*******************************************************************************/
GT_STATUS gtaiGetTimeIncDec
(
    IN  GT_QD_DEV     *dev,
    OUT GT_BOOL        *expired,
    OUT GT_BOOL        *inc,
    OUT GT_U32        *amount
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiGetTimeIncDec Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI) ||
		IS_IN_DEV_GROUP(dev,DEV_88E6321_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
      opData.ptpAddr = 0x1f;
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
      opData.ptpAddr = 5;
	}
    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    *inc = (opData.ptpData & 0x800)?GT_FALSE:GT_TRUE;
    *amount = (GT_U32)(opData.ptpData & 0x7FF);

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

	if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
		*expired = (opData.ptpData & 0x800)?GT_FALSE:GT_TRUE;
	else
		*expired = (opData.ptpData & 0x8)?GT_FALSE:GT_TRUE;
	

    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/*******************************************************************************
* gtaiSetTimeInc
*
* DESCRIPTION:
*         This routine enables time increment by the specifed time increment amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        incremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Increment occurs just once.
*
* INPUTS:
*        amount    - time increment amount (0 ~ 0x7FF)
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiSetTimeInc
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        amount
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetTimeInc Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI) ||
		IS_IN_DEV_GROUP(dev,DEV_88E6321_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* set TimeIncAmt */
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
      opData.ptpAddr = 0x1f;
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
      opData.ptpAddr = 5;
	}
    op = PTP_READ_DATA;


    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    opData.ptpData &= 0xF000;
    opData.ptpData |= amount;

    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    /* set TimeIncEn */
    op = PTP_READ_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

	if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
		opData.ptpData |= 0x800;
	else
		opData.ptpData |= 0x8;

    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading DisPTP.\n"));
        return GT_FAIL;
    }

    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/*******************************************************************************
* gtaiSetTimeDec
*
* DESCRIPTION:
*         This routine enables time decrement by the specifed time decrement amount.
*        The amount specifies the number of units of PTP Global Time that need to be 
*        decremented. This is used for adjusting the PTP Global Time counter value by
*        a certain amount.
*        Decrement occurs just once.
*
* INPUTS:
*        amount    - time decrement amount (0 ~ 0x7FF)
*
* OUTPUTS:
*         None.
*
* RETURNS:
*         GT_OK      - on success
*         GT_FAIL    - on error
*         GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*         None
*
*******************************************************************************/
GT_STATUS gtaiSetTimeDec
(
    IN  GT_QD_DEV     *dev,
    IN  GT_U32        amount
)
{
    GT_STATUS           retVal;
    GT_PTP_OPERATION    op;
    GT_PTP_OP_DATA        opData;

    DBG_INFO(("gtaiSetTimeInc Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_TAI) ||
		IS_IN_DEV_GROUP(dev,DEV_88E6321_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* set TimeIncAmt */
    opData.ptpBlock = GT_AVB_BLOCK_PTP;    /* PTP register space */

    if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
	{
      opData.ptpPort = 0x1E;    /* TAI register */
	  opData.ptpAddr = 0x1f;
	}
	else
	{
      opData.ptpPort = 0xE;    /* TAI register */
	  opData.ptpAddr = 5;
	}
    op = PTP_READ_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI register.\n"));
        return GT_FAIL;
    }

    opData.ptpData &= 0xF000;
    opData.ptpData |= amount;
    opData.ptpData |= 0x800;    /* decrement */

    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI register.\n"));
        return GT_FAIL;
    }

    /* set TimeIncEn */
    op = PTP_READ_DATA;

    opData.ptpAddr = 0;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed reading TAI register.\n"));
        return GT_FAIL;
    }

	if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
		opData.ptpData |= 0x800;
	else
		opData.ptpData |= 0x8;

    op = PTP_WRITE_DATA;

    if((retVal = ptpOperationPerform(dev, op, &opData)) != GT_OK)
    {
        DBG_INFO(("Failed writing TAI register.\n"));
        return GT_FAIL;
    }
	
    DBG_INFO(("OK.\n"));
    return GT_OK;
}


/* gptpModeSet  */
/*******************************************************************************
* gptpModeSet
*
* DESCRIPTION:
*        Set GT_PTP_GLOBAL_CONFIG_PTP_MODE_REG register from PTP point/data registers. 
*
* INPUTS:
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpModeSet
(
    IN  GT_QD_DEV         *dev,
    IN  GT_PTP_MODE_CONFIG  *mode
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U8   data;
    DBG_INFO(("gptpModeSet Called.\n"));
    /* check if device supports this feature */

    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
	data = (((mode->tsAtSFD&1)<<7) |
            ((mode->disPtp2Switch&1)<<6) |
            ((mode->AltScheme&1)<<4) |
            ((mode->GrandMstr&1)<<3) |
            ((mode->OneStep&1)<<2) |
            (mode->PTPMode&3));

	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_AVB_FAMILY))
		data |= (mode->accelNewPTP&1) << 5;

    if((retVal = gptpGlobalPointDataSet(dev, GT_PTP_GLOBAL_CONFIG_PTP_MODE_REG, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }
  return retVal;
}

/* gptpModeGet  */
/*******************************************************************************
* gptpModeGet
*
* DESCRIPTION:
*        Get GT_PTP_GLOBAL_CONFIG_PTP_MODE_REG register from PTP point/data registers. 
*
* INPUTS:
*        None   .
*
* OUTPUTS:
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
GT_STATUS gptpModeGet
(
    IN  GT_QD_DEV           *dev,
    OUT GT_PTP_MODE_CONFIG  *mode
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U8   data;
    DBG_INFO(("gptpModeGet Called.\n"));
    /* check if device supports this feature */

    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if((retVal = gptpGlobalPointDataGet(dev, GT_PTP_GLOBAL_CONFIG_PTP_MODE_REG, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }

	mode->tsAtSFD = (data>>7)&1;
    mode->disPtp2Switch = (data>>6)&1;
    mode->AltScheme = (data>>4)&1;
    mode->GrandMstr = (data>>3)&1;
    mode->OneStep = (data>>2)&1;
    mode->PTPMode = data&3;

	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_AVB_FAMILY))
		mode->accelNewPTP = (data>>5)&1;

  return retVal;
}

/* gptpSetExtTimeFormat  */
/*******************************************************************************
* gptpSetExtTimeFormat
*
* DESCRIPTION:
*        Set GT_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM from PTP point/data registers. 
*
* INPUTS:
*        format-
*			intPTPTime    - Internal PTP Time.
*			extMask    - External Mask on the lower bits of the selected 1722 time.
*			ext1722Time    - IEEE 1722 Time Format to External switch blocks.
*			extTASel    - Time Array Selection for External switch blocks.
*
* OUTPUTS:
*          None.
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
GT_STATUS gptpSetExtTimeFormat
(
	IN  GT_QD_DEV         *dev,
    IN  GT_PTP_EXTERNAL_TIME_FORM  *format
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U8    data;
    DBG_INFO(("gptpSetExtTimeFormat Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
	data = (((format->intPTPTime&1)<<6) |
            ((format->extMask&3)<<4) |
            ((format->ext1722Time&1)<<3) |
            (format->extTASel&3));

    if((retVal = gptpGlobalPointDataSet(dev, GT_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }
  return retVal;
}

/* gptpGetExtTimeFormat  */
/*******************************************************************************
* gptpGetExtTimeFormat
*
* DESCRIPTION:
*        Get GT_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM from PTP point/data registers. 
*
* INPUTS:
*		 None.
*        
* OUTPUTS:
*        format-
*			intPTPTime    - Internal PTP Time.
*			extMask    - External Mask on the lower bits of the selected 1722 time.
*			ext1722Time    - IEEE 1722 Time Format to External switch blocks.
*			extTASel    - Time Array Selection for External switch blocks.
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
GT_STATUS gptpGetExtTimeFormat
(
    IN  GT_QD_DEV           *dev,
    OUT GT_PTP_EXTERNAL_TIME_FORM  *format
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
    GT_U8   data;
    DBG_INFO(("gptpGetExtTimeFormat Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if((retVal = gptpGlobalPointDataGet(dev, GT_PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }

	format->intPTPTime = (data>>6)&1;
    format->extMask = (data>>4)&3;
    format->ext1722Time = (data>>3)&1;
    format->extTASel = data&3;
	
	return retVal;
}


/* gptpGetIngActVec  */
/*******************************************************************************
* gptpGetIngActVec
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
/*
GT_STATUS gptpGetIngActVec
(
    IN  GT_QD_DEV	*dev,
	IN  GT_PTP_SPEC spec,
    OUT GT_U32		*vec
)
{
    GT_STATUS    retVal;         
	GT_U8		 data;
	GT_U8		 point;
    DBG_INFO(("gptpGetIngActVec Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_AVB_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	point = (spec == PTP_IEEE_1588 ? GT_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC :
		GT_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC);

    if((retVal = gptpGlobalPointDataGet(dev, point, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }
	*vec = data;

	if((retVal = gptpGlobalPointDataGet(dev, point+1, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }
	*vec |= data << 8;

	if((retVal = gptpGlobalPointDataGet(dev, point+2, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }
	*vec |= data << 16;
	
	return retVal;
}
*/

/* gptpSetIngActVec  */
/*******************************************************************************
* gptpSetIngActVec
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
/*
GT_STATUS gptpSetIngActVec
(
    IN  GT_QD_DEV	*dev,
	IN  GT_PTP_SPEC spec,
    IN  GT_U32		vec
)
{
    GT_STATUS    retVal;
	GT_U8		 data;
	GT_U8		 point;
    DBG_INFO(("gptpSetIngActVec Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_AVB_FAMILY))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	point = (spec == PTP_IEEE_1588 ? GT_PTP_GLOBAL_CONFIG_1588_ING_ACT_VEC :
		GT_PTP_GLOBAL_CONFIG_AS_ING_ACT_VEC);
	
	data = (GT_U8)(vec & 0xff);
    if((retVal = gptpGlobalPointDataSet(dev, point, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }
	
	data = (GT_U8)((vec >> 8) & 0xff);
	if((retVal = gptpGlobalPointDataSet(dev, point+1, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }

	data = (GT_U8)((vec >> 16) & 0xff);
	if((retVal = gptpGlobalPointDataSet(dev, point+2, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }
	
	return retVal;
}
*/

/* gptpGetEgActVec  */
/*******************************************************************************
* gptpGetEgActVec
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
GT_STATUS gptpGetEgActVec
(
    IN  GT_QD_DEV			*dev,
	IN  GT_PTP_SPEC			spec,
    OUT GT_PTP_EG_ACT_VEC	*vec
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
	GT_U8		 data;
	GT_U8		 point;
    DBG_INFO(("gptpGetEgActVec Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	point = (spec == PTP_IEEE_1588 ? GT_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC :
		GT_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC);

    if((retVal = gptpGlobalPointDataGet(dev, point, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }
	*vec = data&0xFF;

	if((retVal = gptpGlobalPointDataGet(dev, point+1, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }
	*vec |= (data&0xFF) << 8;

	if((retVal = gptpGlobalPointDataGet(dev, point+2, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }
	*vec |= (data&0xFF) << 16;

	if((retVal = gptpGlobalPointDataGet(dev, point+3, &data)) != GT_OK)
    {
        DBG_INFO(("Failed get PTP point/data.\n"));
        return GT_FAIL;
    }
	*vec |= (data&0xFF) << 24;
	
	return retVal;
}

/* gptpSetEgActVec  */
/*******************************************************************************
* gptpSetEgActVec
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
*        GT_OK      - on success
*        GT_FAIL    - on error
*        GT_BAD_PARAM - if input parameters are beyond range.
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
*******************************************************************************/
GT_STATUS gptpSetEgActVec
(
    IN  GT_QD_DEV			*dev,
	IN  GT_PTP_SPEC			spec,
    IN  GT_PTP_EG_ACT_VEC	vec
)
{
    GT_STATUS    retVal;         /* Functions return value.      */
	GT_U8		 data;
	GT_U8		 point;
    DBG_INFO(("gptpSetEgActVec Called.\n"));

    /* check if device supports this feature */
    if (!IS_IN_DEV_GROUP(dev,DEV_PTP_3))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	point = (spec == PTP_IEEE_1588 ? GT_PTP_GLOBAL_CONFIG_1588_EG_ACT_VEC :
		GT_PTP_GLOBAL_CONFIG_AS_EG_ACT_VEC);

	data = (GT_U8)(vec & 0xFF);
    if((retVal = gptpGlobalPointDataSet(dev, point, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }

	data = (GT_U8)((vec >> 8) & 0xFF);
	if((retVal = gptpGlobalPointDataSet(dev, point+1, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }

	data = (GT_U8)((vec >> 16) & 0xFF);
	if((retVal = gptpGlobalPointDataSet(dev, point+2, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }

	data = (GT_U8)((vec >> 24) & 0xFF);
	if((retVal = gptpGlobalPointDataSet(dev, point+3, data)) != GT_OK)
    {
        DBG_INFO(("Failed set PTP point/data.\n"));
        return GT_FAIL;
    }
	
	return retVal;
}



/*******************************************************************************
*
*
*
********************************************************************************/
/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/

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
*       GT_OK on success,
*       GT_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS ptpOperationPerform
(
    IN    GT_QD_DEV             *dev,
    IN    GT_PTP_OPERATION      ptpOp,
    INOUT GT_PTP_OP_DATA        *opData
)
{
    GT_STATUS       retVal;    /* Functions return value */
    GT_U32             i;
	GT_U16          ptpOperation;


    gtSemTake(dev,dev->ptpRegsSem,OS_WAIT_FOREVER);

    /* Wait until the ptp in ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;

      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
      regAccess.rw_reg_list[0].reg = QD_REG_PTP_COMMAND;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
      }
    }
#else
    {
    GT_U16 data = 1;
    while(data == 1)
    {
        retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->ptpRegsSem);
            return retVal;
        }
    }
    }
#endif 

    /* Set the PTP Operation register */
    switch (ptpOp)
    {
        case PTP_WRITE_DATA:
#ifdef GT_RMGMT_ACCESS
            {
              HW_DEV_REG_ACCESS regAccess;

              regAccess.entries = 2;

              regAccess.rw_reg_list[0].cmd = HW_REG_WRITE;
              regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[0].reg = QD_REG_PTP_DATA;
              regAccess.rw_reg_list[0].data = (GT_U16)opData->ptpData;
              regAccess.rw_reg_list[1].cmd = HW_REG_WRITE;
              regAccess.rw_reg_list[1].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[1].reg = QD_REG_PTP_COMMAND;
			  ptpOperation = PTP_WRITE_DATA;
              if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			  {
                ptpOperation &= 3;
                ptpOperation <<=13;
			  }
			  else
			  {
                ptpOperation <<=12;
			  }
              regAccess.rw_reg_list[1].data = (GT_U16)((1 << 15) | ptpOperation |
                                (opData->ptpPort << 8)  |
                                (opData->ptpBlock << 5) |
                                (opData->ptpAddr & 0x1F));
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
              }
            }
#else
    {
    GT_U16 data;
            data = (GT_U16)opData->ptpData;
            retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_DATA,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }

			ptpOperation = PTP_WRITE_DATA;
            if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			{
              ptpOperation &= 3;
              ptpOperation <<=13;
			}
			else
			{
              ptpOperation <<=12;
			}
            data = (GT_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }
    }
#endif
            break;

        case PTP_READ_DATA:
#ifdef GT_RMGMT_ACCESS
            {
              HW_DEV_REG_ACCESS regAccess;

              regAccess.entries = 3;

              regAccess.rw_reg_list[0].cmd = HW_REG_WRITE;
              regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[0].reg = QD_REG_PTP_COMMAND;
	          ptpOperation = PTP_READ_DATA;
              if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			  {
                ptpOperation &= 3;
                ptpOperation <<=13;
			  }
			  else
			  {
                ptpOperation <<=12;
			  }
              regAccess.rw_reg_list[0].data = (GT_U16)((1 << 15) | (ptpOperation) |
                                (opData->ptpPort << 8)  |
                                (opData->ptpBlock << 5) |
                                (opData->ptpAddr & 0x1F));
              regAccess.rw_reg_list[1].cmd = HW_REG_WAIT_TILL_0;
              regAccess.rw_reg_list[1].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[1].reg = QD_REG_PTP_COMMAND;
              regAccess.rw_reg_list[1].data = 15;
              regAccess.rw_reg_list[2].cmd = HW_REG_READ;
              regAccess.rw_reg_list[2].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[2].reg = QD_REG_PTP_DATA;
              regAccess.rw_reg_list[2].data = 0;
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
              }
              opData->ptpData = (GT_U32)    regAccess.rw_reg_list[2].data;
            }
#else
    {
    GT_U16 data;
	        ptpOperation = PTP_READ_DATA;
            if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			{
              ptpOperation &= 3;
              ptpOperation <<=13;
			}
			else
			{
              ptpOperation <<=12;
			}
            data = (GT_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
                retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
                if(retVal != GT_OK)
                {
                    gtSemGive(dev,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
            opData->ptpData = (GT_U32)data;
    }
#endif
            gtSemGive(dev,dev->ptpRegsSem);
            return retVal;

        case PTP_READ_MULTIPLE_DATA:
#ifdef GT_RMGMT_ACCESS
            {
              HW_DEV_REG_ACCESS regAccess;

              regAccess.rw_reg_list[0].cmd = HW_REG_WRITE;
              regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[0].reg = QD_REG_PTP_COMMAND;
	          ptpOperation = PTP_READ_MULTIPLE_DATA;
              if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			  {
                ptpOperation &= 3;
                ptpOperation <<=13;
			  }
			  else
			  {
                ptpOperation <<=12;
			  }
              regAccess.rw_reg_list[0].data = (GT_U16)((1 << 15) | (ptpOperation) |
                                (opData->ptpPort << 8)  |
                                (opData->ptpBlock << 5) |
                                (opData->ptpAddr & 0x1F));
              regAccess.rw_reg_list[1].cmd = HW_REG_WAIT_TILL_0;
              regAccess.rw_reg_list[1].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[1].reg = QD_REG_PTP_COMMAND;
              regAccess.rw_reg_list[1].data = 15;
              for(i=0; i<opData->nData; i++)
              {
                regAccess.rw_reg_list[2+i].cmd = HW_REG_READ;
                regAccess.rw_reg_list[2+i].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
                regAccess.rw_reg_list[2+i].reg = QD_REG_PTP_DATA;
                regAccess.rw_reg_list[2+i].data = 0;
              }
              regAccess.entries = 2+i;
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
              }
              for(i=0; i<opData->nData; i++)
              {
                opData->ptpMultiData[i] = (GT_U32)    regAccess.rw_reg_list[2+i].data;
              }
            }
#else
    {
    GT_U16 data;
	        ptpOperation = PTP_READ_MULTIPLE_DATA;
            if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			{
              ptpOperation &= 3;
              ptpOperation <<=13;
			}
			else
			{
              ptpOperation <<=12;
			}
            data = (GT_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
                retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
                if(retVal != GT_OK)
                {
                    gtSemGive(dev,dev->ptpRegsSem);
                    return retVal;
                }
            }

            for(i=0; i<opData->nData; i++)
            {
                retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
                opData->ptpMultiData[i] = (GT_U32)data;
                if(retVal != GT_OK)
                {
                    gtSemGive(dev,dev->ptpRegsSem);
                    return retVal;
                }
            }
    }
#endif

            gtSemGive(dev,dev->ptpRegsSem);
            return retVal;

        case PTP_READ_TIMESTAMP_DATA:
#ifdef GT_RMGMT_ACCESS
            {
              HW_DEV_REG_ACCESS regAccess;

              regAccess.entries = 3;

              regAccess.rw_reg_list[0].cmd = HW_REG_WRITE;
              regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[0].reg = QD_REG_PTP_COMMAND;
	          ptpOperation = PTP_READ_MULTIPLE_DATA;
              if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			  {
                ptpOperation &= 3;
                ptpOperation <<=13;
			  }
			  else
			  {
                ptpOperation <<=12;
			  }
              regAccess.rw_reg_list[0].data = (GT_U16)((1 << 15) | (ptpOperation) | 
                                (opData->ptpPort << 8)  |
                                (opData->ptpBlock << 5) |
                                (opData->ptpAddr & 0x1F));
              regAccess.rw_reg_list[1].cmd = HW_REG_WAIT_TILL_0;
              regAccess.rw_reg_list[1].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[1].reg = QD_REG_PTP_COMMAND;
              regAccess.rw_reg_list[1].data = 15;
              regAccess.rw_reg_list[2].cmd = HW_REG_READ;
              regAccess.rw_reg_list[2].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[2].reg = QD_REG_PTP_DATA;
              regAccess.rw_reg_list[2].data = 0;
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
              }
              opData->ptpMultiData[0] = (GT_U32)    regAccess.rw_reg_list[2].data;

              if (!(opData->ptpMultiData[0] & 0x1))
              {
                /* valid bit is not set */
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
              }

              for(i=0; i<(opData->nData-1); i++)
              {
                regAccess.rw_reg_list[i].cmd = HW_REG_READ;
                regAccess.rw_reg_list[i].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
                regAccess.rw_reg_list[i].reg = QD_REG_PTP_DATA;
                regAccess.rw_reg_list[i].data = 0;
              }
              regAccess.entries = i;
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
              }
              for(i=0; i<(opData->nData-1); i++)
              {
                opData->ptpMultiData[i+1] = (GT_U32)    regAccess.rw_reg_list[i].data;
              }


              regAccess.entries = 2;

              regAccess.rw_reg_list[0].cmd = HW_REG_WRITE;
              regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[0].reg = QD_REG_PTP_DATA;
              regAccess.rw_reg_list[0].data = (GT_U16)0;
              regAccess.rw_reg_list[1].cmd = HW_REG_WRITE;
              regAccess.rw_reg_list[1].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
              regAccess.rw_reg_list[1].reg = QD_REG_PTP_COMMAND;
	          ptpOperation = PTP_WRITE_DATA;
              if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			  {
                ptpOperation &= 3;
                ptpOperation <<=13;
			  }
			  else
			  {
                ptpOperation <<=12;
			  }
              regAccess.rw_reg_list[1].data = (GT_U16)((1 << 15) | (ptpOperation) |
                                (opData->ptpPort << 8)  |
                                (opData->ptpBlock << 5) |
                                (opData->ptpAddr & 0x1F));
              retVal = hwAccessMultiRegs(dev, &regAccess);
              if(retVal != GT_OK)
              {
                gtSemGive(dev,dev->tblRegsSem);
                return retVal;
              }
            }
#else
    {
    GT_U16 data;
	        ptpOperation = PTP_READ_MULTIPLE_DATA;
            if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			{
              ptpOperation &= 3;
              ptpOperation <<=13;
			}
			else
			{
              ptpOperation <<=12;
			}
            data = (GT_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }

            data = 1;
            while(data == 1)
            {
                retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
                if(retVal != GT_OK)
                {
                    gtSemGive(dev,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
            opData->ptpMultiData[0] = (GT_U32)data;
            if(retVal != GT_OK)
               {
                   gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }

            if (!(data & 0x1))
            {
                /* valid bit is not set */
                   gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }

            for(i=1; i<opData->nData; i++)
            {
                retVal = hwReadGlobal2Reg(dev,QD_REG_PTP_DATA,&data);
                opData->ptpMultiData[i] = (GT_U32)data;
                if(retVal != GT_OK)
                {
                    gtSemGive(dev,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_DATA,0);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }

	        ptpOperation = PTP_WRITE_DATA;
            if (IS_IN_DEV_GROUP(dev,DEV_TAI_3))
			{
              ptpOperation &= 3;
              ptpOperation <<=13;
			}
			else
			{
              ptpOperation <<=12;
			}
            data = (GT_U16)((1 << 15) | (ptpOperation) | 
                    (opData->ptpPort << 8)    |
                    (opData->ptpBlock << 5)    |
                    (opData->ptpAddr & 0x1F));
            retVal = hwWriteGlobal2Reg(dev,QD_REG_PTP_COMMAND,data);
            if(retVal != GT_OK)
            {
                gtSemGive(dev,dev->ptpRegsSem);
                return retVal;
            }
    }
#endif
            gtSemGive(dev,dev->ptpRegsSem);
            break;

        default:
            
            gtSemGive(dev,dev->ptpRegsSem);
            return GT_FAIL;
    }

    /* Wait until the ptp is ready. */
#ifdef GT_RMGMT_ACCESS
    {
      HW_DEV_REG_ACCESS regAccess;

      regAccess.entries = 1;

      regAccess.rw_reg_list[0].cmd = HW_REG_WAIT_TILL_0;
      regAccess.rw_reg_list[0].addr = CALC_SMI_DEV_ADDR(dev, 0, GLOBAL2_REG_ACCESS);
      regAccess.rw_reg_list[0].reg = QD_REG_PTP_COMMAND;
      regAccess.rw_reg_list[0].data = 15;
      retVal = hwAccessMultiRegs(dev, &regAccess);
      if(retVal != GT_OK)
      {
        gtSemGive(dev,dev->tblRegsSem);
        return retVal;
      }
    }
#else
    {
    GT_U16 data;
    data = 1;
    while(data == 1)
    {
        retVal = hwGetGlobal2RegField(dev,QD_REG_PTP_COMMAND,15,1,&data);
        if(retVal != GT_OK)
        {
            gtSemGive(dev,dev->ptpRegsSem);
            return retVal;
        }
    }
    }
#endif
    gtSemGive(dev,dev->ptpRegsSem);
    return retVal;
}



