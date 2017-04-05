/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChPortAp.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS DXCH Port AP API
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
extern GT_STATUS appDemoConfigApPorts
(
    GT_U8 devNum,
    GT_BOOL apEnable,
    GT_U32 numOfPortsToConfig,
    CPSS_PORTS_BMP_STC *apPrtBitmapPtr,
    CPSS_DXCH_PORT_AP_PARAMS_STC *apPortsArrPtr,
    GT_BOOL *useCpssDefultsArrPtr
);

/* counter for "get" functions of cpssDxChPortApPortConfig table */
static GT_PHYSICAL_PORT_NUM   portConfigEntryCount;

/* counter for cpssDxChPortApPortStatus table */
static GT_PHYSICAL_PORT_NUM   portStatusEntryCount;

/* counter for cpssDxChPortApPortStats table */
static GT_PHYSICAL_PORT_NUM   portStatsEntryCount;


/*******************************************************************************
* cpssDxChPortApEnableSet
*
* DESCRIPTION:
*       Enable/disable AP engine (loads AP code into shared memory and starts AP
*       engine).
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupsBmp   - bitmap of cores where to run AP engine
*                           (0x7FFFFFF - for ALL)
*       enable  - GT_TRUE  - enable AP on port group
*                 GT_FALSE - disbale
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortApEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8    devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (1<<(GT_U32)inArgs[1]);
    enable  = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortApEnableSet(devNum, portGroupsBmp, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortApEnableGet
*
* DESCRIPTION:
*       Get AP engine enabled and functional on port group (local core) status.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - AP enabled and functional on port group
*                    GT_FALSE - disbaled or not functional
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - enabledPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortApEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U32     portGroupNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortApEnableGet(devNum, portGroupNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortApPortConfigSet
*
* DESCRIPTION:
*       Enable/disable AP process on port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - number of physical port
*       apEnable    - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters for port
*                               (NULL - for CPSS defaults).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortApPortConfigEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL                         apEnable;
    GT_BOOL                         useCpssDefault;
    CPSS_DXCH_PORT_AP_PARAMS_STC    apParams, *apParamsPtr = NULL;
    GT_U32                          i, j; /* loop iterators */
    GT_U32                          laneNum, 
                                    lanesCount;
    GT_BOOL                         supported;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inFields[0];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);
	
    apEnable    = (GT_BOOL)inFields[1];
    if(GT_FALSE == apEnable)
    {
        goto config;
    }

    useCpssDefault = (GT_BOOL)inFields[2];
    if(GT_TRUE == useCpssDefault)
    {
        goto config;
    }
    else
    {
        apParamsPtr = &apParams;
    }

    apParams.fcAsmDir       = (GT_BOOL)inFields[3];
    apParams.fcPause        = (GT_BOOL)inFields[4];
    apParams.fecRequired    = (GT_BOOL)inFields[5];
    apParams.fecSupported   = (GT_BOOL)inFields[6];
    apParams.laneNum        = (GT_U32)inFields[7];
    apParams.noneceDisable  = (GT_U32)inFields[8];

    for(i = 9, j = 0; i < 9+2*CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS; i+=2, j++)
    {
        apParams.modesAdvertiseArr[j].ifMode = 
            (CPSS_PORT_INTERFACE_MODE_ENT)inFields[i];
        if(CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E == apParams.modesAdvertiseArr[j].ifMode)
        {
            break;
        }
        apParams.modesAdvertiseArr[j].speed = (CPSS_PORT_SPEED_ENT)inFields[i+1];
    }

config:

    if(GT_TRUE == apEnable)
    {
        GT_U32 portMacMap;
        
        result = cpssDxChPortInterfaceSpeedSupportGet(devNum, portNum, 
                                                  CPSS_PORT_INTERFACE_MODE_XGMII_E,
                                                  CPSS_PORT_SPEED_10000_E,
                                                  &supported);
        if(result != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortInterfaceSpeedSupportGet(portNum=%d,XGMII):rc=%d\n",
                            portNum, result);
            return result;
        }
    
        PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                     portNum,
                                                                     portMacMap);
        
        if(supported)
        {
            lanesCount = 6;
            for(laneNum = 0; laneNum < lanesCount; laneNum++)
            {
                (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                            CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
                                                            ((portMacMap<<8)|laneNum),
                                                            CPSS_EVENT_MASK_E);            
            }                
        }
        else
        {
            result = cpssDxChPortInterfaceSpeedSupportGet(devNum, portNum, 
                                                      CPSS_PORT_INTERFACE_MODE_RXAUI_E,
                                                      CPSS_PORT_SPEED_10000_E,
                                                      &supported);
            if(result != GT_OK)
            {
                cpssOsPrintf("cpssDxChPortInterfaceSpeedSupportGet(portNum=%d,RXAUI):rc=%d\n",
                                portNum, result);
                return result;
            }
            if(supported)
            {
                lanesCount = 2;
                for(laneNum = 0; laneNum < lanesCount; laneNum++)
                {
                    (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                                CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
                                                                ((portMacMap<<8)|laneNum),
                                                                CPSS_EVENT_MASK_E);
                }                
            }
        }
    
        (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                    CPSS_PP_PORT_SYNC_STATUS_CHANGED_E,
                                                    portMacMap,
                                                    CPSS_EVENT_MASK_E);
    
        (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                    CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                                    portMacMap,
                                                    CPSS_EVENT_MASK_E);
        
        if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
        {
            (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                        CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
                                                        portMacMap,
                                                        CPSS_EVENT_MASK_E);
        }
    }

    /* call cpss api function */
    result = cpssDxChPortApPortConfigSet(devNum, portNum, apEnable, apParamsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortApPortConfigGet
*
* DESCRIPTION:
*       Get AP configuration of port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       apEnablePtr - AP enable/disable on port
*       apParamsPtr - (ptr to) AP parameters of port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apEnablePtr or apParamsPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS prvWrCpssDxChPortApPortConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_BOOL                         apEnable;
    CPSS_DXCH_PORT_AP_PARAMS_STC    apParams;
    GT_U32                          i, j; /* loop iterators */
    GT_U32                          portGroup;
    GT_BOOL                         apEnabledOnCore;
    static CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr = &portMapShadow;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

	CONVERT_BACK_DEV_PHYSICAL_PORT_MAC(devNum, portConfigEntryCount);
	
    /* skip CPU and not existing ports */
    for(;portConfigEntryCount < CPSS_MAX_PORTS_NUM_CNS; portConfigEntryCount++)
    {
        if (PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portConfigEntryCount) ||
            (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE))
        {
            if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                result = cpssDxChPortPhysicalPortDetailedMapGet(devNum, 
                                    portConfigEntryCount, /*OUT*/portMapShadowPtr);
                if (result != GT_OK)
                {
                    return result;
                }

                if((portMapShadowPtr->valid != GT_TRUE) || (portMapShadowPtr->portMap.mappingType !=
                                                            CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E))
                {
                    continue;
                }                    
            }

			if (PRV_CPSS_GE_PORT_GE_ONLY_E == 
				PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, portConfigEntryCount))
			{
				continue;
			}

            break;
        }
        else
            continue;
    }

    if(portConfigEntryCount >= CPSS_MAX_PORTS_NUM_CNS)
    {
        /* no ports */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    
    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                          portConfigEntryCount);
    result = cpssDxChPortApEnableGet(devNum,portGroup,&apEnabledOnCore);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(apEnabledOnCore)
    {
		cpssOsMemSet(&apParams, 0, sizeof(CPSS_DXCH_PORT_AP_PARAMS_STC));
        /* call cpss api function */
        result = cpssDxChPortApPortConfigGet(devNum,portConfigEntryCount,&apEnable,
                                             &apParams);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    
        inFields[1] = apEnable;
        inFields[3] = apParams.fcAsmDir;
        inFields[4] = apParams.fcPause;
        inFields[5] = apParams.fecRequired;
        inFields[6] = apParams.fecSupported;
        inFields[7] = apParams.laneNum;
        inFields[8] = apParams.noneceDisable;
        
        for(i = 9, j = 0; i < 9+2*CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS; i+=2, j++)
        {
            inFields[i] = apParams.modesAdvertiseArr[j].ifMode;
            inFields[i+1] = apParams.modesAdvertiseArr[j].speed;
        }
    }
    else
    {
        inFields[1] = GT_FALSE;
        for(i = 3; i<30; i++)
        {
            inFields[i] = 0;
        }
    }
    inFields[0] = portConfigEntryCount;
    inFields[2] = GT_FALSE; /* useCpssDefault */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20],
                inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],
                inFields[27], inFields[28]);


    galtisOutput(outArgs, GT_OK, "%f");

    portConfigEntryCount++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPortApPortConfigEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portConfigEntryCount = 0;

    return prvWrCpssDxChPortApPortConfigGet(inArgs,inFields,numFields,outArgs);
}

static CMD_STATUS wrCpssDxChPortApPortConfigEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return prvWrCpssDxChPortApPortConfigGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChPortApPortStatusGet
*
* DESCRIPTION:
*       Get status of AP on port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apStatusPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS prvWrCpssDxChPortApPortStatusEntryGet
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32       numFields,
    IN  GT_BOOL     invokedForAllPorts,
    OUT GT_8        outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_DXCH_PORT_AP_STATUS_STC    apStatus;
    GT_U32                          portGroup;
    GT_BOOL                         apEnabled;
    static CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr = &portMapShadow;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
	CONVERT_BACK_DEV_PHYSICAL_PORT_MAC(devNum, portConfigEntryCount);		
    if(invokedForAllPorts)
    {
        /* skip CPU and not existing ports */
        for(;portStatusEntryCount < CPSS_MAX_PORTS_NUM_CNS; portStatusEntryCount++)
        {
            if (PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portStatusEntryCount) ||
                (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE))
            {
                if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
                {
                    result = cpssDxChPortPhysicalPortDetailedMapGet(devNum, 
                                        portStatusEntryCount, /*OUT*/portMapShadowPtr);
                    if (result != GT_OK)
                    {
                        return result;
                    }

                    if((portMapShadowPtr->valid != GT_TRUE) || (portMapShadowPtr->portMap.mappingType !=
                                                                CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E))
                    {
                        continue;
                    }                    
                }

				if (PRV_CPSS_GE_PORT_GE_ONLY_E == 
					PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, portStatusEntryCount))
				{
					continue;
				}

                break;
            }
            else
                continue;
        }
                                
        if(portStatusEntryCount >= CPSS_MAX_PORTS_NUM_CNS)
        {
            /* no ports */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }
    else
    {
        portStatusEntryCount = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, 
                                                      portStatusEntryCount);
    result = cpssDxChPortApEnableGet(devNum,portGroup,&apEnabled);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    
    if(apEnabled)
    {                
		cpssOsMemSet(&apStatus, 0, sizeof(CPSS_DXCH_PORT_AP_STATUS_STC));
        /* call cpss api function */
        result = cpssDxChPortApPortStatusGet(devNum, portStatusEntryCount, 
                                             &apStatus);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        inFields[1] = apStatus.postApPortNum;
        inFields[2] = apStatus.hcdFound;
        inFields[4] = apStatus.fecEnabled;
        inFields[5] = apStatus.portMode.ifMode;
        inFields[6] = apStatus.portMode.speed;
        inFields[7] = apStatus.fcRxPauseEn;
        inFields[8] = apStatus.fcTxPauseEn;
    }
    else
    {
        inFields[1] = 0;
        inFields[2] = 0;
        inFields[4] = 0;
        inFields[5] = CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E;
        inFields[6] = CPSS_PORT_SPEED_NA_E;
        inFields[7] = GT_FALSE;
        inFields[8] = GT_FALSE;
    }

    inFields[0] = portStatusEntryCount;
    inFields[3] = 0; /* was portInit */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6], inFields[7], inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");

    portStatusEntryCount++;

    return CMD_OK;
}

static CMD_STATUS  wrCpssDxChPortApPortStatusEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portStatusEntryCount = 0;

    return prvWrCpssDxChPortApPortStatusEntryGet(inArgs, inFields, numFields,
                                                 GT_TRUE, outArgs);
}

static CMD_STATUS  wrCpssDxChPortApPortStatusEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return prvWrCpssDxChPortApPortStatusEntryGet(inArgs, inFields, numFields,
                                                 GT_TRUE, outArgs);
}

static CMD_STATUS  wrCpssDxChPortApPortStatusOnPortGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return prvWrCpssDxChPortApPortStatusEntryGet(inArgs, inFields, numFields,
                                                 GT_FALSE, outArgs);
}

/*******************************************************************************
* wrCpssDxChPortApGetNext
*
* DESCRIPTION:
*
*   This function is called for ending table printing.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortApGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)

{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortApResolvedPortsBmpGet
*
* DESCRIPTION:
*       Get bitmap of ports on port group (local core) where AP process finished
*       with agreed for both sides resolution
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupNum - number of port group (local core)
*
* OUTPUTS:
*       apResolvedPortsBmpPtr  - 1's set for ports of local core where AP
*                                   resolution acheaved
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port group or device
*       GT_BAD_PTR               - apResolvedPortsBmpPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortApResolvedPortsBmpGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U32     portGroupNum;
    GT_U32     apResolvedPortsBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortApResolvedPortsBmpGet(devNum, portGroupNum, 
                                               &apResolvedPortsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", apResolvedPortsBmp);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortApEnableSet
*
* DESCRIPTION:
*       Enable/disable AP engine (loads AP code into shared memory and starts AP
*       engine).
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portGroupsBmp   - bitmap of cores where to run AP engine
*                           (0x7FFFFFF - for ALL)
*       enable  - GT_TRUE  - enable AP on port group
*                 GT_FALSE - disbale
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortApSetActiveMode

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
    CPSS_PORT_SPEED_ENT             speed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    ifMode  = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[2];
    speed   = (CPSS_PORT_SPEED_ENT)inArgs[3];

	CONVERT_DEV_PHYSICAL_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChPortApSetActiveMode(devNum, portNum, ifMode, speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPortApStatsReset
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* APPLICABLE DEVICES:
*        BC2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChPortApStatsReset
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    result = cpssDxChPortApStatsReset(devNum, portNum);

    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* wrCpssDxChPortApIntropSet
*
* DESCRIPTION:
*       Set AP port introp information
*
* APPLICABLE DEVICES:
*        BC2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apStatusPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChPortApIntropSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PORT_AP_INTROP_STC    apIntropParams;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum                                  = (GT_U8)inArgs[0];
    portNum                                 = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    apIntropParams.attrBitMask              = (GT_U16)inArgs[2];
    apIntropParams.txDisDuration            = (GT_U16)inArgs[3];
    apIntropParams.abilityDuration          = (GT_U16)inArgs[4];
    apIntropParams.abilityMaxInterval       = (GT_U16)inArgs[5];
    apIntropParams.abilityFailMaxInterval   = (GT_U16)inArgs[6];
    apIntropParams.apLinkDuration           = (GT_U16)inArgs[7];
    apIntropParams.apLinkMaxInterval        = (GT_U16)inArgs[8];
    apIntropParams.pdLinkDuration           = (GT_U16)inArgs[9];
    apIntropParams.pdLinkMaxInterval        = (GT_U16)inArgs[10];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    result = cpssDxChPortApIntropSet(devNum, portNum, &apIntropParams);

    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/*******************************************************************************
* wrCpssDxChPortApIntropGet
*
* DESCRIPTION:
*       Get AP port introp information
*
* APPLICABLE DEVICES:
*        BC2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apStatusPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChPortApIntropGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PORT_AP_INTROP_STC    apIntropParams;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    result = cpssDxChPortApIntropGet(devNum, portNum, &apIntropParams);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
    }
    else
    {
        inFields[0] = apIntropParams.attrBitMask;
        inFields[1] = apIntropParams.txDisDuration;
        inFields[2] = apIntropParams.abilityDuration;
        inFields[3] = apIntropParams.abilityMaxInterval;
        inFields[4] = apIntropParams.abilityFailMaxInterval;
        inFields[5] = apIntropParams.apLinkDuration;
        inFields[6] = apIntropParams.apLinkMaxInterval;
        inFields[7] = apIntropParams.pdLinkDuration;
        inFields[8] = apIntropParams.pdLinkMaxInterval;

        galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d", apIntropParams.attrBitMask, apIntropParams.txDisDuration,
                     apIntropParams.abilityDuration, apIntropParams.abilityMaxInterval, apIntropParams.abilityFailMaxInterval,
                     apIntropParams.apLinkDuration, apIntropParams.apLinkMaxInterval, apIntropParams.pdLinkDuration,
                     apIntropParams.pdLinkMaxInterval);
    }

    return CMD_OK;
}

/*******************************************************************************
* prvWrCpssDxChPortApPortsStatsGet
*
* DESCRIPTION:
*       Get AP port statistics information
*
* APPLICABLE DEVICES:
*        BC2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       apStatusPtr - (ptr to) AP parameters for port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - apStatusPtr is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - AP engine not run
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS prvWrCpssDxChPortApPortsStatsGet
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32       numFields,
    IN  GT_BOOL     invokedForAllPorts,
    OUT GT_8        outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_DXCH_PORT_AP_STATS_STC     apStats;
    GT_U32                          portGroup;
    GT_BOOL                         apEnabled;
    GT_U16                          intropAbilityMaxInterval;
    static CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr = &portMapShadow;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    CONVERT_BACK_DEV_PHYSICAL_PORT_MAC(devNum, portStatsEntryCount);

    if(invokedForAllPorts)
    {
        /* skip CPU and not existing ports */
        for(;portStatsEntryCount < CPSS_MAX_PORTS_NUM_CNS; portStatsEntryCount++)
        {
            if (PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portStatsEntryCount) ||
                (PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE))
            {
                if(PRV_CPSS_DXCH_PP_HW_INFO_PORT_MAP_CHECK_SUPPORTED_MAC(devNum) == GT_TRUE)
                {
                    result = cpssDxChPortPhysicalPortDetailedMapGet(devNum,
                                        portStatsEntryCount, /*OUT*/portMapShadowPtr);
                    if (result != GT_OK)
                    {
                        return result;
                    }
                    if((portMapShadowPtr->valid != GT_TRUE) || (portMapShadowPtr->portMap.mappingType !=
                                                                CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E))
                    {
                        continue;
                    }
                }

                if (PRV_CPSS_GE_PORT_GE_ONLY_E == 
                    PRV_CPSS_DXCH_PORT_TYPE_OPTIONS_MAC(devNum, portStatsEntryCount))
                {
                    continue;
                }


                break;
            }
            else
                continue;
        }

        if(portStatsEntryCount >= CPSS_MAX_PORTS_NUM_CNS)
        {
            /* no ports */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }
    else
    {
        portStatsEntryCount = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    }

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portStatsEntryCount);
    result = cpssDxChPortApEnableGet(devNum,portGroup,&apEnabled);

    if (result != GT_OK || apEnabled == GT_FALSE)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChPortApStatsGet(devNum, portStatsEntryCount, &apStats, &intropAbilityMaxInterval);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(!invokedForAllPorts)
    {
        inFields[0] = apStats.txDisCnt;
        inFields[1] = apStats.abilityCnt % intropAbilityMaxInterval;
        inFields[2] = apStats.abilityCnt / intropAbilityMaxInterval;
        inFields[3] = apStats.abilitySuccessCnt;
        inFields[4] = apStats.linkFailCnt;
        inFields[5] = apStats.linkSuccessCnt;
        inFields[6] = apStats.hcdResoultionTime;
        inFields[7] = apStats.linkUpTime;

        fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                    inFields[3], inFields[4], inFields[5], inFields[6], inFields[7]);
    }
    else
    {
        inFields[0] = portStatsEntryCount;
        inFields[1] = apStats.txDisCnt;
        inFields[2] = apStats.abilityCnt % intropAbilityMaxInterval;
        inFields[3] = apStats.abilityCnt / intropAbilityMaxInterval;
        inFields[4] = apStats.abilitySuccessCnt;
        inFields[5] = apStats.linkFailCnt;
        inFields[6] = apStats.linkSuccessCnt;
        inFields[7] = apStats.hcdResoultionTime;
        inFields[8] = apStats.linkUpTime;

        portStatsEntryCount++;

        fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                    inFields[3], inFields[4], inFields[5], inFields[6], inFields[7], inFields[8]);

    }

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

static CMD_STATUS  wrCpssDxChPortApPortsStatsGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portStatsEntryCount = 0;

    return prvWrCpssDxChPortApPortsStatsGet(inArgs, inFields, numFields,
                                                 GT_TRUE, outArgs);
}

static CMD_STATUS  wrCpssDxChPortApPortsStatsGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return prvWrCpssDxChPortApPortsStatsGet(inArgs, inFields, numFields,
                                                 GT_TRUE, outArgs);
}

static CMD_STATUS  wrCpssDxChPortApPortStatsGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return prvWrCpssDxChPortApPortsStatsGet(inArgs, inFields, numFields,
                                                 GT_FALSE, outArgs);
}

/*******************************************************************************
* wrCpssDxChPortApGetNext
*
* DESCRIPTION:
*
*   This function is called for ending table printing.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortApPortStatsGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)

{
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

static CPSS_DXCH_PORT_AP_PARAMS_STC         apPortsArray[CPSS_MAX_PORTS_NUM_CNS];
static GT_BOOL                              useCpssDefaultsArray[CPSS_MAX_PORTS_NUM_CNS];
static GT_U32                               numOfApPortsToConfig;
static CPSS_PORTS_BMP_STC                   apPortBitmap;

/*******************************************************************************
* internalAPPortsSet
*
* DESCRIPTION:
*       configure AP ports
*
* APPLICABLE DEVICES:
*        Bobcat2; Lion3
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - wrong devNum, portNum
*       GT_BAD_PTR            - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
*
********************************************************************************/
static CMD_STATUS internalAPPortsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN GT_U32   opCode
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    GT_PHYSICAL_PORT_NUM                    port;
    GT_BOOL                                 apEnable;
    GT_U32                                  i,j;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(opCode == 0)/*start registration*/
    {
        numOfApPortsToConfig = 0;
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&apPortBitmap);
    }
    else if(opCode == 0xFFFFFFFF)/*end registration*/
    {
        devNum = (GT_U8)inArgs[0];
        apEnable = (GT_BOOL)inArgs[1];

        result = appDemoConfigApPorts(devNum, apEnable, numOfApPortsToConfig, &apPortBitmap, apPortsArray, useCpssDefaultsArray);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d",-1);/* Error ! */
            return CMD_OK;
        }
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    if (numOfApPortsToConfig > 255)
    {
        galtisOutput(outArgs, GT_FULL, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    port = (GT_PHYSICAL_PORT_NUM)inFields[0];
    devNum = (GT_U8)inArgs[0];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);
    CPSS_PORTS_BMP_PORT_SET_MAC(&apPortBitmap, port);

    useCpssDefaultsArray[numOfApPortsToConfig] = (GT_BOOL)inFields[1];


    if(useCpssDefaultsArray[numOfApPortsToConfig] == GT_FALSE)
    {
        apPortsArray[numOfApPortsToConfig].fcAsmDir = (GT_BOOL)inFields[2];
        apPortsArray[numOfApPortsToConfig].fcPause = (CPSS_DXCH_PORT_AP_FLOW_CONTROL_ENT)inFields[3];
        apPortsArray[numOfApPortsToConfig].fecRequired = (GT_BOOL)inFields[4];
        apPortsArray[numOfApPortsToConfig].fecSupported = (GT_BOOL)inFields[5];
        apPortsArray[numOfApPortsToConfig].laneNum = (GT_BOOL)inFields[6];
        apPortsArray[numOfApPortsToConfig].noneceDisable = (GT_U32)inFields[7];

        for(i = 8, j = 0; j < CPSS_DXCH_PORT_AP_IF_ARRAY_SIZE_CNS; i+=2, j++)
        {
            apPortsArray[numOfApPortsToConfig].modesAdvertiseArr[j].ifMode = (CPSS_PORT_INTERFACE_MODE_ENT)inFields[i];
            if(CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E == apPortsArray[numOfApPortsToConfig].modesAdvertiseArr[j].ifMode)
            {
                break;
            }
            apPortsArray[numOfApPortsToConfig].modesAdvertiseArr[j].speed = (CPSS_PORT_SPEED_ENT)inFields[i+1];
        }
    }
    numOfApPortsToConfig++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPortApPortsSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalAPPortsSet(inArgs,inFields,numFields,outArgs,0);
}

static CMD_STATUS wrCpssDxChPortApPortsSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalAPPortsSet(inArgs,inFields,numFields,outArgs,1);
}

static CMD_STATUS wrCpssDxChPortApPortsEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalAPPortsSet(inArgs,inFields,numFields,outArgs,0xFFFFFFFF);
}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPortApEnableSet",
        &wrCpssDxChPortApEnableSet,
        3, 0},

    {"cpssDxChPortApEnableGet",
        &wrCpssDxChPortApEnableGet,
        2, 0},


    {"cpssDxChPortApPortConfigEntrySet",
        &wrCpssDxChPortApPortConfigEntrySet,
        1, 29},
    {"cpssDxChPortApPortConfigEntryGetFirst",
        &wrCpssDxChPortApPortConfigEntryGetFirst,
        1, 0},
    {"cpssDxChPortApPortConfigEntryGetNext",
        &wrCpssDxChPortApPortConfigEntryGetNext,
        1, 0},

    {"cpssDxChPortApPortStatusEntryExtGetFirst",
        &wrCpssDxChPortApPortStatusEntryGetFirst,
        1, 0},
    {"cpssDxChPortApPortStatusEntryExtGetNext",
        &wrCpssDxChPortApPortStatusEntryGetNext,
        1, 0},


    {"cpssDxChPortApPortStatusOnPortGetFirst",
        &wrCpssDxChPortApPortStatusOnPortGetFirst,
        2, 0},
    {"cpssDxChPortApPortStatusOnPortGetNext",
        &wrCpssDxChPortApGetNext,
        2, 0},

    {"cpssDxChPortApResolvedPortsBmpGet",
        &wrCpssDxChPortApResolvedPortsBmpGet,
        2, 0},

    {"cpssDxChPortApSetActiveMode",
        &wrCpssDxChPortApSetActiveMode,
        4, 0},

    {"cpssDxChPortApStatsGetFirst",
        &wrCpssDxChPortApPortsStatsGetFirst,
        1, 0},
    {"cpssDxChPortApStatsGetNext",
        &wrCpssDxChPortApPortsStatsGetNext,
        1, 0},
    {"cpssDxChPortApStatsOnPortGetFirst",
        &wrCpssDxChPortApPortStatsGetFirst,
        2, 0},
    {"cpssDxChPortApStatsOnPortGetNext",
        &wrCpssDxChPortApPortStatsGetNext,
        2, 0},
    {"cpssDxChPortApStatsReset",
        &wrCpssDxChPortApStatsReset,
        2, 0},
    {"cpssDxChPortApIntropSet",
        &wrCpssDxChPortApIntropSet,
        11, 0},
    {"cpssDxChPortApIntropGet",
        &wrCpssDxChPortApIntropGet,
        2, 0},
    {"cpssDxChPortApPortConfigEntriesSetFirst",
        &wrCpssDxChPortApPortsSetFirst,
        2, 27},
    {"cpssDxChPortApPortConfigEntriesSetNext",
        &wrCpssDxChPortApPortsSetNext,
        2, 27},
    {"cpssDxChPortApPortConfigEntriesEndSet",
        &wrCpssDxChPortApPortsEndSet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortAp
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssDxChPortAp
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

