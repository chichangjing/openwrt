#include <Copyright.h>

/********************************************************************************
* gtPCSCtrl.c
*
* DESCRIPTION:
*       API implementation for 1000BASE-X PCS block register access.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#include <msApi.h>
#include <gtHwCntl.h>
#include <gtDrvSwRegs.h>


/*******************************************************************************
* gpcsGetCommaDet
*
* DESCRIPTION:
*        This routine retrieves Comma Detection status in PCS
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for Comma Detected or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetCommaDet
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetCommaDet Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    if (IS_IN_DEV_GROUP(dev,DEV_PCS_LINK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the CommaDet bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,15,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetPCSLink
*
* DESCRIPTION:
*        This routine retrieves Link up status in PCS
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE for Comma Detected or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetPCSLink
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetPCSLink Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS_LINK))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the PCS Link bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,15,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetSyncOK
*
* DESCRIPTION:
*        This routine retrieves SynOK bit. It is set to a one when the PCS has
*        detected a few comma patterns and is synchronized with its peer PCS 
*        layer.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if synchronized or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetSyncOK
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetSyncOK Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the SyncOK bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,14,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsGetSyncFail
*
* DESCRIPTION:
*        This routine retrieves SynFail bit.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if synchronizaion failed or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetSyncFail
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetSyncFail Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the SyncFail bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,13,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsGetAnBypassed
*
* DESCRIPTION:
*        This routine retrieves Inband Auto-Negotiation bypass status.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if AN is bypassed or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetAnBypassed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetAnBypassed Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the AnBypassed bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,12,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetAnBypassMode
*
* DESCRIPTION:
*        This routine retrieves Enable mode of Inband Auto-Negotiation bypass.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE if AN bypass is enabled or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetAnBypassMode
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *mode
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetAnBypassMode Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the AnBypass bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,11,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetAnBypassMode
*
* DESCRIPTION:
*        This routine retrieves Enable mode of Inband Auto-Negotiation bypass.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable AN bypass mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetAnBypassMode
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetAnBypassMode Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the AnBypass bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,11,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetPCSAnEn
*
* DESCRIPTION:
*        This routine retrieves Enable mode of PCS Inband Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE if PCS AN is enabled or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetPCSAnEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *mode
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetPCSAnEn Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        if (!IS_IN_DEV_GROUP(dev, DEV_INTERNAL_GPHY))
        {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
        }

        if ((hwPort < 4) || (hwPort > 7))
        {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    /* Get the PCSAnEn bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,10,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetPCSAnEn
*
* DESCRIPTION:
*        This routine sets Enable mode of PCS Inband Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_TRUE to enable PCS AN mode or GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetPCSAnEn
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      mode
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetPCSAnEn Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate BOOL to binary */
    BOOL_2_BIT(mode, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        if (!IS_IN_DEV_GROUP(dev, DEV_INTERNAL_GPHY))
        {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
        }

        if ((hwPort < 4) || (hwPort > 7))
        {
            DBG_INFO(("GT_NOT_SUPPORTED\n"));
            return GT_NOT_SUPPORTED;
        }
    }

    /* Get the PCSAnEn bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,10,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetRestartPCSAn
*
* DESCRIPTION:
*        This routine restarts PCS Inband Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetRestartPCSAn
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetRestartPCSAn Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    data = 1;    /* to set RestartPCSAn bit */

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the RestartPCSAn bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,9,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetPCSAnDone
*
* DESCRIPTION:
*        This routine retrieves completion information of PCS Auto-Negotiation.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - GT_TRUE if PCS AN is done or never done
*                GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetPCSAnDone
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *mode
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetPCSAnDone Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_PCS))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* check if the given port supports PCS */
    if (!DOES_DEVPORT_SUPPORT_PCS(dev,hwPort))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the PCSAnDone bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,8,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsSetFCValue
*
* DESCRIPTION:
*        This routine sets Flow Control's force value
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force flow control enabled, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetFCValue
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
	GT_U8			tmpData;

    DBG_INFO(("gpcsSetFCValue Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FC_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    BOOL_2_BIT(state, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

	if(IS_IN_DEV_GROUP(dev,DEV_88E6390_FAMILY))
	{
		retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
		if(retVal != GT_OK)
		{
			DBG_INFO(("Failed.\n"));
			return retVal;
		}
		tmpData &= ~0x4;
		tmpData |= (data << 2);
		retVal = gportSetFlowCtrl(dev,port,0x10,tmpData);
	} else {
		retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,7,1,data);
	}

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetFCValue
*
* DESCRIPTION:
*        This routine retrieves Flow Control Value which will be used for Forcing 
*        Flow Control enabled or disabled.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if FC Force value is one (flow control enabled)
*                 GT_FALSE otherwise (flow control disabled)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetFCValue
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;          
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
	GT_U8           tmpData;

    DBG_INFO(("gpcsGetFCValue Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FC_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);
	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_FAMILY)) {
		retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
		data = (tmpData >> 2) & 0x1;
	} else {
		retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,7,1,&data);
	}

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsSetForcedFC
*
* DESCRIPTION:
*        This routine forces Flow Control. If FCValue is set to one, calling this 
*        routine with GT_TRUE will force Flow Control to be enabled.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force flow control (enable or disable), GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForcedFC
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_BOOL      state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
	GT_U8			tmpData;

    DBG_INFO(("gpcsSetForcedFC Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FC_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    BOOL_2_BIT(state, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

	if(IS_IN_DEV_GROUP(dev,DEV_88E6390_FAMILY))
	{
		retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
		if(retVal != GT_OK)
		{
			DBG_INFO(("Failed.\n"));
			return retVal;
		}
		tmpData &= ~0x8;
		tmpData |= (data << 3);
		retVal = gportSetFlowCtrl(dev,port,0x10,tmpData);
	} else {
		retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,6,1,data);
	}

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetForcedFC
*
* DESCRIPTION:
*        This routine retrieves Forced Flow Control bit
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if ForcedFC bit is one,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForcedFC
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */
	GT_U8			tmpData;

    DBG_INFO(("gpcsGetForcedFC Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FC_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

	if (IS_IN_DEV_GROUP(dev,DEV_88E6390_FAMILY)) {
		retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
		data = (tmpData >> 3) & 0x1;
	} else {
		retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,6,1,&data);
	}

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetFCMode
*
* DESCRIPTION:
*        This routine sets full duplex flow control mode
*
* INPUTS:
*        port - the logical port number.
*        mode - flow control mode
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetFCMode
(
	IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_PORT_FC_MODE      mode
)
{
	GT_U8			tmpData;
    GT_STATUS       retVal;         /* Functions return value.      */

    DBG_INFO(("gpcsSetFCMode Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FC_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
	if(retVal != GT_OK)
	{
		DBG_INFO(("Failed.\n"));
		return retVal;
	}

	tmpData &= ~0x3;
	tmpData |= (GT_U8)mode;
	retVal = gportSetFlowCtrl(dev,port,0x10,tmpData);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsGetFCMode
*
* DESCRIPTION:
*        This routine gets full duplex flow control mode
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        mode - flow control mode
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetFCMode
(
	IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    OUT GT_PORT_FC_MODE      *mode
)
{
	GT_U8			tmpData;
    GT_STATUS       retVal;         /* Functions return value.      */

    DBG_INFO(("gpcsGetFCMode Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FC_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
	if(retVal != GT_OK)
	{
		DBG_INFO(("Failed.\n"));
		return retVal;
	}

	*mode = (GT_PORT_FC_MODE)(tmpData & 0x3);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gprtGetFCPath
*
* DESCRIPTION:
*        This routine retrives the information regarding flow control QC path
*		 considerations.
*
*        If set, the QC will use its FC algorithm only when both the source port
*		 of a frame and its destination port have FC enabled.
*		 If cleared, the QC will use its FC algorithm whenever the source port
*		 of a frame has FC enabled.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if connected or GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        
*
*******************************************************************************/
GT_STATUS gprtGetFCPath
(
	IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL     *state
)
{
	GT_U16          data;
    GT_STATUS       retVal;         /* Functions return value.      */
    /*GT_U8           hwPort;*/         /* the physical port number     */
	GT_U8			tmpData;

    DBG_INFO(("gprtGetFCPath Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_FC_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
/*    hwPort = GT_LPORT_2_PORT(port);*/

	/* Set pointer, 0x10:Port Flow Control Mode */
	retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
	data = (tmpData >> 7) & 0x1;
    
    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);
    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gprtSetFCPath
*
* DESCRIPTION:
*        This routine sets flow control QC path considerations bit.
*
*        If set, the QC will use its FC algorithm only when both the source port
*		 of a frame and its destination port have FC enabled.
*		 If cleared, the QC will use its FC algorithm whenever the source port
*		 of a frame has FC enabled.
*
* INPUTS:
*        port - the logical port number.
*		 state - GT_TRUE if connected or GT_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        
*
*******************************************************************************/
GT_STATUS gprtSetFCPath
(
	IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      state
)
{
	GT_U16          data;
    GT_STATUS       retVal;         /* Functions return value.      */
/*    GT_U8           hwPort;*/         /* the physical port number     */
	GT_U8			tmpData;

    DBG_INFO(("gprtSetFCPath Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_FC_MODE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

	BOOL_2_BIT(state, data);

    /* translate LPORT to hardware port */
   /* hwPort = GT_LPORT_2_PORT(port);*/
	retVal = gportGetFlowCtrl(dev,port,0x10,&tmpData);
	if(retVal != GT_OK)
	{
		DBG_INFO(("Failed.\n"));
		return retVal;
	}
		
	tmpData &= ~0x80;
	tmpData |= data << 7;
	retVal = gportSetFlowCtrl(dev,port,0x10,tmpData);
    
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsSetLinkValue
*
* DESCRIPTION:
*        This routine sets Link's force value
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force link up, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetLinkValue
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetLinkValue Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    BOOL_2_BIT(state, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the LinkValue bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,5,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetLinkValue
*
* DESCRIPTION:
*        This routine retrieves Link Value which will be used for Forcing Link 
*        up or down.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Link Force value is one (link up)
*                 GT_FALSE otherwise (link down)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetLinkValue
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetLinkValue Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the LinkValue bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,5,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsSetForcedLink
*
* DESCRIPTION:
*        This routine forces Link. If LinkValue is set to one, calling this 
*        routine with GT_TRUE will force Link to be up.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force link (up or down), GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForcedLink
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetForcedLink Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    BOOL_2_BIT(state, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the ForcedLink bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,4,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetForcedLink
*
* DESCRIPTION:
*        This routine retrieves Forced Link bit
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if ForcedLink bit is one,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForcedLink
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetForcedLink Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the ForcedLink bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,4,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsSetDpxValue
*
* DESCRIPTION:
*        This routine sets Duplex's Forced value. This function needs to be
*        called prior to gpcsSetForcedDpx.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force full duplex, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetDpxValue
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetDpxValue Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    BOOL_2_BIT(state, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the DpxValue bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,3,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetDpxValue
*
* DESCRIPTION:
*        This routine retrieves Duplex's Forced value
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if Duplex's Forced value is set to Full duplex,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetDpxValue
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetDpxValue Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the DpxValue bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,3,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsSetForcedDpx
*
* DESCRIPTION:
*        This routine forces duplex mode. If DpxValue is set to one, calling this 
*        routine with GT_TRUE will force duplex mode to be full duplex.
*
* INPUTS:
*        port - the logical port number.
*        state - GT_TRUE to force duplex mode, GT_FALSE otherwise
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForcedDpx
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN    GT_BOOL        state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetForcedDpx Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    BOOL_2_BIT(state, data);

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the ForcedDpx bit.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,2,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetForcedDpx
*
* DESCRIPTION:
*        This routine retrieves Forced Duplex.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_TRUE if ForcedDpx bit is one,
*                 GT_FALSE otherwise
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForcedDpx
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *state
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetForcedDpx Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the ForcedDpx bit.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,2,1,&data);

    /* translate binary to BOOL  */
    BIT_2_BOOL(data, *state);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsSetForceSpeed
*
* DESCRIPTION:
*        This routine forces Speed node.
*
* INPUTS:
*        port - the logical port number.
*        mode - GT_PORT_FORCED_SPEED_MODE (10, 100, 1000, 10g,or no force speed)
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsSetForceSpeed
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port,
    IN GT_PORT_FORCED_SPEED_MODE  mode
)
{
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetForceSpeed Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Set the Force Speed bits.  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,0,2,(GT_U16)mode);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetForceSpeed
*
* DESCRIPTION:
*        This routine retrieves Force Speed value
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        state - GT_PORT_FORCED_SPEED_MODE (10, 100, 1000, 10g, or no force speed)
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*        
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
GT_STATUS gpcsGetForceSpeed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_PORT_FORCED_SPEED_MODE   *mode
)
{
    GT_U16          data;           /* Used to poll the SWReset bit */
    GT_STATUS       retVal;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetForceSpeedMode Called.\n"));

    /* check if the given Switch supports this feature. */
    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_WITH_VALUE))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    /* Get the ForceSpeed bits.  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,0,2,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    
    *mode = data;

    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetRGMIITimingDelay
*
* DESCRIPTION:
*        RGMII receive/transmit Timing Control. This api adds delay to RXCLK for
*        IND inputs and GTXCLK for OUTD outputs when port is in RGMII mode.
*        Change to this bit are disruptive to normal operation. Hence any changes
*        to this register must be done only while the port's link is down.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        rxmode - GT_FALSE for default setup, GT_TRUE for adding delay to rxclk
*        txmode - GT_FALSE for default setup, GT_TRUE for adding delay to txclk
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetRGMIITimingDelay
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *rxmode,
    OUT GT_BOOL      *txmode
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetRGMIITimingDelay Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_RGMII_TIMING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (hwPort < (dev->maxPorts - 2))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

  if(((dev->devName==DEV_88E6165)||(dev->devName==DEV_88E6161))&&
	  ((hwPort==4)||((hwPort==5)&&(dev->revision==2)))) /* 88E6123 revision A2 */
  {
    if(hwWritePortReg(dev,4,0x1A,0x81E7) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }
    if(hwReadPortReg(dev,5,0x1A,&data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }
	if((hwPort==5)&&(dev->revision==2))  /* 88E6123 revision A2 */
	{
      *rxmode = (data & 0x2)?GT_TRUE:GT_FALSE;
      *txmode = (data & 0x1)?GT_TRUE:GT_FALSE;
	}
	else
	{
      *rxmode = (data & 0x10)?GT_TRUE:GT_FALSE;
      *txmode = (data & 0x8)?GT_TRUE:GT_FALSE;
	}

  }
  else
  {
    /* Get the register bit(s).  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,14,2,&data);

    *rxmode = (data & 0x2)?GT_TRUE:GT_FALSE;
    *txmode = (data & 0x1)?GT_TRUE:GT_FALSE;

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
  }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetRGMIITimingDelay
*
* DESCRIPTION:
*        RGMII receive/transmit Timing Control. This api adds delay to RXCLK for
*        IND inputs and GTXCLK for OUTD outputs when port is in RGMII mode.
*        Change to this bit are disruptive to normal operation. Hence any changes
*        to this register must be done only while the port's link is down.
*
* INPUTS:
*        port - the logical port number.
*        rxmode - GT_FALSE for default setup, GT_TRUE for adding delay to rxclk
*        txmode - GT_FALSE for default setup, GT_TRUE for adding delay to txclk
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetRGMIITimingDelay
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      rxmode,
    IN  GT_BOOL      txmode
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetRGMIITimingDelay Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_RGMII_TIMING))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);


  if(((dev->devName==DEV_88E6165)||(dev->devName==DEV_88E6161))&&(hwPort < (dev->maxPorts - 2)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

    if((dev->devName==DEV_88E6351_FAMILY)&&((hwPort!=4)&&(hwPort!=5))) /* 88E6351 */
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

    if((dev->devName==DEV_88E6352_FAMILY)&&((hwPort!=5)&&(hwPort!=6))) /* 88E6352 */
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

    if((dev->devName==DEV_88E6321_FAMILY)&&((hwPort!=2)&&(hwPort!=5)&&(hwPort!=6))) /* 88E6321 */
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }

    if((dev->devName==G1_DEV_88E6390_FAMILY)&&((hwPort!=0))) /* 88E6390 */
	{
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_BAD_PARAM;
    }


  if(((dev->devName==DEV_88E6165)||(dev->devName==DEV_88E6161))&&
	  ((hwPort==4)||((hwPort==5)&&(dev->revision==2)))) /* 88E6123 revision A2 */
  {
    if(hwWritePortReg(dev,4,0x1A,0x81E7) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }
    if(hwReadPortReg(dev,5,0x1A,&data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }
	if((hwPort==5)&&(dev->revision==2))  /* 88E6123 revision A2 */
	{
	  data &= 0xfff9;
      data |= (rxmode) ? 0x2 : 0;
      data |= (txmode) ? 0x1: 0;
	}
	else
	{
	  data &= 0xffe7;
      data |= (rxmode) ? 0x10 : 0;
      data |= (txmode) ? 0x8: 0;
	}
    if(hwWritePortReg(dev,5,0x1A, data) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }
     if(hwWritePortReg(dev,4,0x1A,0xC1E7) != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
        return GT_FAIL;
    }
 }
  else
  {
    data = (rxmode) ? 2 : 0;
    data |= (txmode) ? 1 : 0;

    /* Set the register bit(s).  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,14,2,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
  }

    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetRGMIIAutoNego
*
* DESCRIPTION:
*        Set RGMII Auto-Negotiation enable. When this bit is set to a one, RGMII in-band
*        auto-negotiation is supported where the results of the auto-negotiation
*        are sent to the port’s MAC if the RGMII Result Enable bit (Port offset
*        0x17) is set to a one.
*
* INPUTS:
*        port - the logical port number.
*        en - GT_FALSE to disable RGMII Auto-Negotiation, 
*             GT_TRUE to enable RGMII Auto-Negotiation
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetRGMIIAutoNego
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetRGMIIAutoNego Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_RGMII_AUTO))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!((hwPort==2)||(hwPort==5)||(hwPort==6)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    data = (en == GT_TRUE)?1:0;

    /* Get the register bit(s).  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,13,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsGetRGMIIAutoNego
*
* DESCRIPTION:
*        Get RGMII Auto-Negotiation enable. When this bit is set to a one, RGMII in-band
*        auto-negotiation is supported where the results of the auto-negotiation
*        are sent to the port’s MAC if the RGMII Result Enable bit (Port offset
*        0x17) is set to a one.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_FALSE to disable RGMII Auto-Negotiation, 
*             GT_TRUE to enable RGMII Auto-Negotiation
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetRGMIIAutoNego
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetRGMIIAutoNego Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_RGMII_AUTO))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!((hwPort==2)||(hwPort==5)||(hwPort==6)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* Get the register bit(s).  */
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,13,1,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    *en = (data & 0x1)?GT_TRUE:GT_FALSE;
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetForceSpeedEn
*
* DESCRIPTION:
*        Set Force Speed. When this bit is set to a one the speed for this 
*        port’s MAC will be forced to the value in the SpdValue and the AltSpeed 
*        registers regardless of what the normal speed value would be. 
*        When this bit is cleared to a zero, normal speed detection occurs.
*
* INPUTS:
*        port - the logical port number.
*        en - GT_FALSE to Reset Force Speed, 
*             GT_TRUE to Set Force Speed
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetForceSpeedEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetForceSpeedEn Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_SPEED))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

#if 0
    if (!((hwPort==2)||(hwPort==5)||(hwPort==6)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
#endif

    data = (en == GT_TRUE)?1:0;

    /* Set the register bit(s).  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,13,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsGetForceSpeedEn
*
* DESCRIPTION:
*        Get Force Speed. When this bit is set to a one the speed for this 
*        port’s MAC will be forced to the value in the SpdValue and the AltSpeed 
*        registers regardless of what the normal speed value would be. 
*        When this bit is cleared to a zero, normal speed detection occurs.
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_FALSE to Reset Force Speed, 
*             GT_TRUE to Set Force Speed
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetForceSpeedEn
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetForceSpeedEn Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_SPEED))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

#if 0
    if (!((hwPort==2)||(hwPort==5)||(hwPort==6)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }
#endif
    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,13,1,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    *en = (data & 0x1)?GT_TRUE:GT_FALSE;
    /* return */
    return retVal;
}

/*******************************************************************************
* gpcsSetAltSpeed
*
* DESCRIPTION:
*        Set Alternate Speed Mode. When Port 2, 5 or 6’s C_Mode is 0x0 or 0x1 
*        (Port offset 0x00) this bit can be used along with the SpdValue bits 
*        below to force the port’s speed as follows: 0 = Normal 10, 100, 1000 or 
*        10G speed 1 = Alternate speed as defined in the SpdValue register
*
* INPUTS:
*        port - the logical port number.
*        en - GT_FALSE to Reset Alternate Speed Mode, 
*             GT_TRUE to Set Alternate Speed Mode
*
* OUTPUTS:
*        None
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsSetAltSpeed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    IN  GT_BOOL      en
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsSetAltSpeed Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_SPEED))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!((hwPort==0)||(hwPort==9)||(hwPort==10)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED_PORT\n"));
        return GT_NOT_SUPPORTED_PORT;
    }

    data = (en == GT_TRUE)?1:0;

    /* Set the register bit(s).  */
    retVal = hwSetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,12,1,data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    /* return */
    return retVal;
}


/*******************************************************************************
* gpcsGetAltSpeed
*
* DESCRIPTION:
*        Get Alternate Speed Mode. When Port 2, 5 or 6’s C_Mode is 0x0 or 0x1 
*        (Port offset 0x00) this bit can be used along with the SpdValue bits 
*        below to force the port’s speed as follows: 0 = Normal 10, 100, 1000 or 
*        10G speed 1 = Alternate speed as defined in the SpdValue register
*
* INPUTS:
*        port - the logical port number.
*
* OUTPUTS:
*        en - GT_FALSE to Reset Alternate Speed Mode, 
*             GT_TRUE to Set Alternate Speed Mode
*
* RETURNS:
*        GT_OK   - on success
*        GT_FAIL - on error
*        GT_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gpcsGetAltSpeed
(
    IN  GT_QD_DEV    *dev,
    IN  GT_LPORT     port,
    OUT GT_BOOL      *en
)
{
    GT_U16          data;
    GT_STATUS       retVal=GT_OK;         /* Functions return value.      */
    GT_U8           hwPort;         /* the physical port number     */

    DBG_INFO(("gpcsGetAltSpeed Called.\n"));

    if (!IS_IN_DEV_GROUP(dev,DEV_FORCE_SPEED))
    {
        DBG_INFO(("GT_NOT_SUPPORTED\n"));
        return GT_NOT_SUPPORTED;
    }

    /* translate LPORT to hardware port */
    hwPort = GT_LPORT_2_PORT(port);

    if (!((hwPort==0)||(hwPort==9)||(hwPort==10)))
    {
        DBG_INFO(("GT_NOT_SUPPORTED_PORT\n"));
        return GT_NOT_SUPPORTED_PORT;
    }

    retVal = hwGetPortRegField(dev,hwPort, QD_REG_PCS_CONTROL,12,1,&data);

    if(retVal != GT_OK)
    {
        DBG_INFO(("Failed.\n"));
    }
    else
    {
        DBG_INFO(("OK.\n"));
    }
    *en = (data & 0x1)?GT_TRUE:GT_FALSE;
    /* return */
    return retVal;
}




