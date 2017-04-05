/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* wrapSysconfig.c
*
* DESCRIPTION:
*       This file implements wrapper functions for the 'sysconfig' library;
*       It also defines the command database entries for the commander engine.
*
* DEPENDENCIES:
*
*
* FILE REVISION NUMBER:
*       $Revision: 83 $
*
*******************************************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <galtisAgent/wrapUtil/cmdCpssSysConfig.h>

#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/*******************************************************************************
 * External usage variables
 ******************************************************************************/
extern GT_INTFUNCPTR    gtInitSystemFuncPtr;


#if (defined ASIC_SIMULATION) && (! (defined RTOS_ON_SIM))
    #define USE_WIN_SYS_CONF_OUT_STR_CNS
    extern GT_CHAR winSysconfOutStr[/*100*/];
#endif

/* flag for debug to remove the hwDevNum convert*/
GT_BOOL gtHwDevNumConvert = GT_TRUE;

/*******************************************************************************
* mngInitSystem (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngInitSystem
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
    return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = mngInitSystem();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* mngGetSysFirstElement (general command)
*
* DESCRIPTION:
*     Returns the first element of the sysconf.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngGetSysFirstElement
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

        /* check for valid arguments */
        if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

        /* call tapi api function */
        mngGetSysFirstElement();

        /* pack output arguments to galtis string */
        *outArgs = 0;
#ifdef USE_WIN_SYS_CONF_OUT_STR_CNS
        cmdOsStrCpy((GT_CHAR*)outArgs,winSysconfOutStr);
#endif /* USE_WIN_SYS_CONF_OUT_STR_CNS */
        return CMD_OK;
}

/*******************************************************************************
* mngGetSysNextElement (general command)
*
* DESCRIPTION:
*     Returns the next element of the sysconf.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngGetSysNextElement
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
    return CMD_AGENT_ERROR;

    /* call tapi api function */
    mngGetSysNextElement();

    /* pack output arguments to galtis string */
    *outArgs = 0;
#ifdef USE_WIN_SYS_CONF_OUT_STR_CNS
    cmdOsStrCpy((GT_CHAR*)outArgs,winSysconfOutStr);
#endif /* USE_WIN_SYS_CONF_OUT_STR_CNS */
    return CMD_OK;
}

/*******************************************************************************
* sys (general command)
*
* DESCRIPTION:
*     Prints the sysconf elements.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrSys
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
    return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = sys();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

GT_U32 physicalPortDevMapArray[4][4];
/*******************************************************************************
* wrMngPhysicalPortDevConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPhysicalPortDevConvertSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(inFields[0] > 3)
    {
        galtisOutput(outArgs, result, "\nIndex out of range 0..3\n");
        return CMD_OK;
    }

    physicalPortDevMapArray[inFields[0]][0] = (GT_U32)inFields[1];
    physicalPortDevMapArray[inFields[0]][1] = (GT_U32)inFields[2];
    physicalPortDevMapArray[inFields[0]][2] = (GT_U32)inFields[3];
    physicalPortDevMapArray[inFields[0]][3] = (GT_U32)inFields[4];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPhysicalPortDevConvertGet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_U32 tableIndexPhy;
static CMD_STATUS wrMngPhysicalPortDevConvertGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndexPhy = 0;

    inFields[0] = tableIndexPhy;
    inFields[1] = physicalPortDevMapArray[0][0];
    inFields[2] = physicalPortDevMapArray[0][1];
    inFields[3] = physicalPortDevMapArray[0][2];
    inFields[4] = physicalPortDevMapArray[0][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

static CMD_STATUS wrMngPhysicalPortDevConvertGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndexPhy++;
    if(tableIndexPhy == 4)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tableIndexPhy;
    inFields[1] = physicalPortDevMapArray[tableIndexPhy][0];
    inFields[2] = physicalPortDevMapArray[tableIndexPhy][1];
    inFields[3] = physicalPortDevMapArray[tableIndexPhy][2];
    inFields[4] = physicalPortDevMapArray[tableIndexPhy][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrMngPhysicalPortDevConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPhysicalPortDevConvertClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(physicalPortDevMapArray, 0, sizeof(physicalPortDevMapArray));

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* gtIsPhysicalPortsMapConvert
*
* DESCRIPTION:
*       Do ports mapping.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID gtPhysicalPortsDevConvert
(
   INOUT GT_U8 *devNum,
   INOUT GT_PHYSICAL_PORT_NUM *portNum
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if(((GT_U8)physicalPortDevMapArray[i][0] == (*devNum)) &&
            ((GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][1] == (*portNum)))
        {
            *devNum = (GT_U8)physicalPortDevMapArray[i][2];
            *portNum = (GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][3];

            break;
        }
    }
}

/*******************************************************************************
* gtPhysicalPortsDevConvertBack
*
* DESCRIPTION:
*       Do ports mapping.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID gtPhysicalPortsDevConvertBack
(
   INOUT GT_U8                  *devNum,
   INOUT GT_PHYSICAL_PORT_NUM   *portNum
)
{
    GT_U32          i;

    for(i=0 ; i<4 ; i++)
    {
        if(((GT_U8)physicalPortDevMapArray[i][2] == (*devNum)) &&
           ((GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][3] == (*portNum)))
        {
            *devNum = (GT_U8)physicalPortDevMapArray[i][0];
            *portNum = (GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][1];
            break;
        }
    }
}


void gtPhysicalPortsSwDevConvert
(
   INOUT GT_HW_DEV_NUM            *hwDevNum,
   INOUT GT_PHYSICAL_PORT_NUM     *portNum
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((physicalPortDevMapArray[i][0] == (*hwDevNum)) &&
           ((GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][1] == (*portNum)))
        {
            *hwDevNum = physicalPortDevMapArray[i][2];
            *portNum = (GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][3];
            break;
        }
    }
}

void gtPhysicalPortsDevConvertDataBack
(
   INOUT GT_HW_DEV_NUM           *hwDevNumPtr,
   INOUT GT_PHYSICAL_PORT_NUM    *portNum
)
{
    GT_U32          i;
    GT_HW_DEV_NUM   hwDevNum = *hwDevNumPtr;

    if(GT_TRUE == gtHwDevNumConvert)
    {
        /* first we need to find the SW device that hold the hwDevNum */

        /* need to fix value of PRV_CPSS_MAX_PP_DEVICES_CNS*/
        CPSS_TBD_BOOKMARK_EARCH

        for(i = 0 ;i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            if(prvCpssPpConfig[i] == NULL)
            {
                continue;
            }

            if(PRV_CPSS_HW_DEV_NUM_MAC(i) == hwDevNum)
            {
                /* this is the 'sw_devNum' we look for */
                *hwDevNumPtr = i;
                break;
            }
        }

        /* if we got here and i == PRV_CPSS_MAX_PP_DEVICES_CNS , meaning that the
           HW-devNum not belongs to any of the cpss devices , so we assume 'remote device'
           and we can't convert it to 'sw-devNum' so we will use it 'as is' */
    }

    for(i=0 ; i<4 ; i++)
    {
        if((physicalPortDevMapArray[i][2] == (*hwDevNumPtr)) && ((GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][3] == (*portNum)))
        {
            *hwDevNumPtr = physicalPortDevMapArray[i][0];
            *portNum = (GT_PHYSICAL_PORT_NUM)physicalPortDevMapArray[i][1];
            break;
        }
    }
}


GT_U8 portDevMapArray[4][4];
/*******************************************************************************
* wrMngPortDevConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortDevConvertSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(inFields[0] > 3)
    {
        galtisOutput(outArgs, result, "\nIndex out of range 0..3\n");
        return CMD_OK;
    }

    portDevMapArray[inFields[0]][0] = (GT_U8)inFields[1];
    portDevMapArray[inFields[0]][1] = (GT_U8)inFields[2];
    portDevMapArray[inFields[0]][2] = (GT_U8)inFields[3];
    portDevMapArray[inFields[0]][3] = (GT_U8)inFields[4];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortDevConvertGet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_U32 tableIndex;
static CMD_STATUS wrMngPortDevConvertGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex = 0;

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portDevMapArray[0][0];
    inFields[2] = (GT_U32)portDevMapArray[0][1];
    inFields[3] = (GT_U32)portDevMapArray[0][2];
    inFields[4] = (GT_U32)portDevMapArray[0][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

static CMD_STATUS wrMngPortDevConvertGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex++;
    if(tableIndex == 4)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portDevMapArray[tableIndex][0];
    inFields[2] = (GT_U32)portDevMapArray[tableIndex][1];
    inFields[3] = (GT_U32)portDevMapArray[tableIndex][2];
    inFields[4] = (GT_U32)portDevMapArray[tableIndex][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrMngPortDevConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortDevConvertClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(portDevMapArray, 0, sizeof(portDevMapArray));

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* gtIsPortsMapConvert
*
* DESCRIPTION:
*       Do ports mapping.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID gtPortsDevConvert
(
   INOUT GT_U8 *devNum,
   INOUT GT_U8 *portNum
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portDevMapArray[i][0] == (*devNum)) && (portDevMapArray[i][1] == (*portNum)))
        {
            *devNum = portDevMapArray[i][2];
            *portNum = portDevMapArray[i][3];
            break;
        }
    }
}


/*******************************************************************************
* gt_U32_PortsDevConvert
*
* DESCRIPTION:
*       Do ports mapping.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       none
*
* COMMENTS:
*
*
*******************************************************************************/
GT_VOID gt_U32_PortsDevConvert
(
   INOUT GT_U8 *devNum,
   INOUT GT_U32 *portNum
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portDevMapArray[i][0] == (*devNum)) && (portDevMapArray[i][1] == (*portNum)))
        {
            *devNum = portDevMapArray[i][2];
            *portNum = portDevMapArray[i][3];
            break;
        }
    }
}

void gtPortsSwDevConvert
(
   INOUT GT_HW_DEV_NUM            *hwDevNum,
   INOUT GT_PORT_NUM              *portNum
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portDevMapArray[i][0] == (*hwDevNum)) && (portDevMapArray[i][1] == (*portNum)))
        {
            *hwDevNum = portDevMapArray[i][2];
            *portNum = portDevMapArray[i][3];
            break;
        }
    }
}
void gtPortsDevConvertDataBack
(
   INOUT GT_HW_DEV_NUM  *hwDevNumPtr,
   INOUT GT_PORT_NUM    *portNum
)
{
    GT_U32          i;
    GT_HW_DEV_NUM   hwDevNum = *hwDevNumPtr;

    if(GT_TRUE == gtHwDevNumConvert)
    {
        /* first we need to find the SW device that hold the hwDevNum */

        /* need to fix value of PRV_CPSS_MAX_PP_DEVICES_CNS*/
        CPSS_TBD_BOOKMARK_EARCH

        for(i = 0 ;i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            if(prvCpssPpConfig[i] == NULL)
            {
                continue;
            }

            if(PRV_CPSS_HW_DEV_NUM_MAC(i) == hwDevNum)
            {
                /* this is the 'sw_devNum' we look for */
                *hwDevNumPtr = i;
                break;
            }
        }

        /* if we got here and i == PRV_CPSS_MAX_PP_DEVICES_CNS , meaning that the
           HW-devNum not belongs to any of the cpss devices , so we assume 'remote device'
           and we can't convert it to 'sw-devNum' so we will use it 'as is' */
    }

    for(i=0 ; i<4 ; i++)
    {
        if((portDevMapArray[i][2] == (*hwDevNumPtr)) && (portDevMapArray[i][3] == (*portNum)))
        {
            *hwDevNumPtr = portDevMapArray[i][0];
            *portNum = portDevMapArray[i][1];
            break;
        }
    }
}

GT_VOID gtPhysicalPortsBitmapDevConvert
(
   INOUT GT_U8                  *devNum,
   INOUT CPSS_PORTS_BMP_STC     *portBitMap
)
{
    GT_U32 i, j;
    CPSS_PORTS_BMP_STC localPortBitMap = {{0,0}};
    GT_U8   localDevNum = *devNum;

    for( i = 0 ; i < CPSS_MAX_PORTS_NUM_CNS ; i++ )
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(portBitMap,i))
        {
            for ( j = 0 ; j < 4 ; j++ )
            {
                if ( (physicalPortDevMapArray[j][0] == localDevNum) && (physicalPortDevMapArray[j][1] == i) )
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,physicalPortDevMapArray[j][3]);

                    *devNum = (GT_U8)physicalPortDevMapArray[j][0];

                    break;
                }
            }

            if ( 4 == j)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,i);
            }
        }
    }

    *portBitMap = localPortBitMap;
}

GT_VOID gtPhysicalPortsBitmapDevConvertDataBack
(
   INOUT GT_U8                  *devNum,
   INOUT CPSS_PORTS_BMP_STC     *portBitMap
)
{
    GT_U32 i, j;
    CPSS_PORTS_BMP_STC localPortBitMap = {{0,0}};
    GT_U8   localDevNum = *devNum;

    if(GT_TRUE == gtHwDevNumConvert)
    {
        /* first we need to find the SW device that hold the hwDevNum */
        for(i = 0 ;i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            if(prvCpssPpConfig[i] == NULL)
            {
                continue;
            }

            if(PRV_CPSS_HW_DEV_NUM_MAC(i) == localDevNum)
            {
                /* this is the 'sw_devNum' we look for */
                localDevNum = (GT_U8) i;
                break;
            }
        }

        /* if we got here and i == PRV_CPSS_MAX_PP_DEVICES_CNS , meaning that the
           HW-devNum not belongs to any of the cpss devices , so we assume 'remote device'
           and we can't convert it to 'sw-devNum' so we will use it 'as is' */
    }

    for( i = 0 ; i < CPSS_MAX_PORTS_NUM_CNS ; i++ )
    {
        if (PRV_CPSS_SIP_5_CHECK_MAC(localDevNum) == 0)
        {
            if ((!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(localDevNum, i))
                && (i != CPSS_CPU_PORT_NUM_CNS))
                continue;
        }

        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(portBitMap,i))
        {
            for ( j = 0 ; j < 4 ; j++ )
            {
                if ( (physicalPortDevMapArray[j][2] == localDevNum) && (physicalPortDevMapArray[j][3] == i) )
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,physicalPortDevMapArray[j][1]);

                    *devNum = (GT_U8)physicalPortDevMapArray[j][0];

                    break;
                }
            }

            if ( 4 == j)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&localPortBitMap,i);
            }
        }
        else
        {
            for ( j = 0 ; j < 4 ; j++ )
            {
                if ( (physicalPortDevMapArray[j][2] == localDevNum) && (physicalPortDevMapArray[j][3] == i) )
                {
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&localPortBitMap,physicalPortDevMapArray[j][1]);

                    *devNum = (GT_U8)physicalPortDevMapArray[j][0];

                    break;
                }
            }
        }
    }

    *portBitMap = localPortBitMap;
}

/* enable/disable the hwDevNum convert .
function sets the flag of gtHwDevNumConvert */
GT_STATUS gtHwDevNumConvertSet
(
   IN GT_BOOL enableConvert
)
{
    gtHwDevNumConvert = enableConvert;
    return GT_OK;
}

/*******************************************************************************
* cpssInitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       boardIdx      - The index of the board to be initialized from the board
*                       list.
*       boardRevId    - Board revision Id.
*       reloadEeprom  - Whether the Eeprom should be reloaded when
*                       corePpHwStartInit() is called.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssInitSystem
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_STATUS result;
     GT_U32  boardIdx;
     GT_U32  boardRevId;
     GT_U32  reloadEeprom;

   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    boardIdx=(GT_U32)inArgs[0];
    boardRevId=(GT_U32)inArgs[1];
    reloadEeprom=(GT_U32)inArgs[2];

    result = cmdInitSystem(boardIdx,boardRevId,reloadEeprom);
   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssInitSystemGet
*
* DESCRIPTION:
*       Function gets parameters of cpss init system proccess.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       boardIdxPtr     - (pointer to) The index of the board to be initialized
*                         from the board list.
*       boardRevIdPtr   - (pointer to) Board revision Id.
*       reloadEepromPtr - (pointer to) Whether the Eeprom should be reloaded when
*                         corePpHwStartInit() is called.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssInitSystemGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_U32  boardIdx;
     GT_U32  boardRevId;
     GT_U32  reloadEeprom;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdInitSystemGet(&boardIdx,&boardRevId,&reloadEeprom);
   /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%d%d", boardIdx, boardRevId, reloadEeprom);
    return CMD_OK;
}

/*******************************************************************************
* cpssResetSystem
*
* DESCRIPTION:
*       Function resets system for later cpss init system proccess.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssResetSystem
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_STATUS result;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    result = cmdResetSystem();
   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* confi2InitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       theConfiId  - the confi ID
*       echoOn      - if GT_TRUE the echo is on, otherwise the echo is off.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrConfi2InitSystem
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    IN GT_U8        theConfiId;
    IN GT_BOOL      echoOn;
    GT_STATUS result;
      /* check for valid arguments */
       if (!inArgs || !outArgs)
           return CMD_AGENT_ERROR;

       theConfiId=(GT_U8)inArgs[0];
       if ((GT_U32)inArgs[1] == 0)
       {
           echoOn = GT_FALSE;
       }
       else
       {
           echoOn = GT_TRUE;
       }
       result = cmdInitConfi(theConfiId,echoOn);
      /* pack output arguments to galtis string */
       galtisOutput(outArgs, result, "");
       return CMD_OK;

}
GT_U8 portGroupDevMapArray[4][4];
/*******************************************************************************
* wrMngPortGroupConvertSet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupConvertSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(inFields[0] > 3)
    {
        galtisOutput(outArgs, result, "\nIndex out of range 0..3\n");
        return CMD_OK;
    }

    portGroupDevMapArray[inFields[0]][0] = (GT_U8)inFields[1];
    portGroupDevMapArray[inFields[0]][1] = (GT_U8)inFields[2];
    portGroupDevMapArray[inFields[0]][2] = (GT_U8)inFields[3];
    portGroupDevMapArray[inFields[0]][3] = (GT_U8)inFields[4];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupConvertGet (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupConvertGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex = 0;

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portGroupDevMapArray[0][0];
    inFields[2] = (GT_U32)portGroupDevMapArray[0][1];
    inFields[3] = (GT_U32)portGroupDevMapArray[0][2];
    inFields[4] = (GT_U32)portGroupDevMapArray[0][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

static CMD_STATUS wrMngPortGroupConvertGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tableIndex++;
    if(tableIndex == 4)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tableIndex;
    inFields[1] = (GT_U32)portGroupDevMapArray[tableIndex][0];
    inFields[2] = (GT_U32)portGroupDevMapArray[tableIndex][1];
    inFields[3] = (GT_U32)portGroupDevMapArray[tableIndex][2];
    inFields[4] = (GT_U32)portGroupDevMapArray[tableIndex][3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupConvertClear (general command)
*
* DESCRIPTION:
*     Initialize the system according to the pp configuration paramters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupConvertClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(portGroupDevMapArray, 0, sizeof(portGroupDevMapArray));

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/* convert from
    devNum,portGroupsBmp    of test values  (received from Galtis/RDE test)

    to

    devNum,portGroupsBmp    of CPSS values
*/
GT_VOID gtPortGroupDevConvert
(
   INOUT GT_U8 *devNum,
   INOUT GT_PORT_GROUPS_BMP *portGroupsBmpPtr
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portGroupDevMapArray[i][0] == (*devNum)) && (portGroupDevMapArray[i][1] == (*portGroupsBmpPtr)))
        {
            *devNum = portGroupDevMapArray[i][2];
            *portGroupsBmpPtr = portGroupDevMapArray[i][3];
            break;
        }
    }
}

/* convert from
    devNum,portGroupsBmp    of CPSS values

    to

    devNum,portGroupsBmp    of test values (to send to Galtis/RDE test)
*/
GT_VOID gtPortGroupDevConvertBack
(
   INOUT GT_U8 *devNum,
   INOUT GT_PORT_GROUPS_BMP *portGroupsBmpPtr
)
{
    GT_U32 i;

    for(i=0 ; i<4 ; i++)
    {
        if((portGroupDevMapArray[i][2] == (*devNum)) && (portGroupDevMapArray[i][3] == (*portGroupsBmpPtr)))
        {
            *devNum = portGroupDevMapArray[i][0];
            *portGroupsBmpPtr = portGroupDevMapArray[i][1];
            break;
        }
    }
}

/*******************************************************************************
* wrMngPortGroupsBmpEnable (general command)
*
* DESCRIPTION:
*     enable/disable usage of API with new portGroupsBmp parameter
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupsBmpEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_U8           devNum;
    GT_BOOL         enable,tmpEnable;
    GT_PORT_GROUPS_BMP  portGroupsBmp;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    rc = utilMultiPortGroupsBmpGet(devNum,&tmpEnable,&portGroupsBmp);

    enable = (GT_BOOL)inArgs[1];

    rc = utilMultiPortGroupsBmpSet(devNum,enable,portGroupsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupsBmpSet (general command)
*
* DESCRIPTION:
*     Set the 'portGroupsBmp'
*
Arguments:
1.         GT_BOOL         unawareMode    -- True ' all port groups (unaware mode) , and we ignore all other parameters (portGroupId_0..7)
                                                               False ' use info in portGroupId_0..7
2.         GT_BOOL         portGroupId_0    - True ' set that port group Id 0 need to be in the bitmap of portGroupsBmp
                                                              False ' port group Id 0 NOT need to be in the bitmap of portGroupsBmp
3.         GT_BOOL         portGroupId_1 . Similar to portGroupId_0 but refer to port group Id 1
4.         GT_BOOL         portGroupId_2 . Similar to portGroupId_0 but refer to port group Id 2
5.         GT_BOOL         portGroupId_3 . Similar to portGroupId_0 but refer to port group Id 3
6.         GT_BOOL         portGroupId_4 . Similar to portGroupId_0 but refer to port group Id 4
7.         GT_BOOL         portGroupId_5 . Similar to portGroupId_0 but refer to port group Id 5
8.         GT_BOOL         portGroupId_6 . Similar to portGroupId_0 but refer to port group Id 6
9.         GT_BOOL         portGroupId_7 . Similar to portGroupId_0 but refer to port group Id 7
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupsBmpSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_BOOL         unawareMode;
    GT_U32          ii = 0;
    GT_U8           devNum;
    GT_U32          startIndex;
    GT_BOOL         enable;
    GT_PORT_GROUPS_BMP  portGroupsBmp,tmpBmp;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[ii++];
    unawareMode = (GT_BOOL)inArgs[ii++];

    if(unawareMode == GT_TRUE)
    {
        portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }
    else
    {
        portGroupsBmp = 0;

        startIndex = ii;

        for(/* continue ii*/ ; ii < (startIndex + 8) ; ii++)
        {
            if(((GT_BOOL)inArgs[ii]) == GT_TRUE)
            {
                portGroupsBmp |= 1 << (ii - startIndex);
            }
        }
    }

    rc = utilMultiPortGroupsBmpGet(devNum,&enable,&tmpBmp);

    rc = utilMultiPortGroupsBmpSet(devNum,enable,portGroupsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrMngPortGroupsBmpSet (general command)
*
* DESCRIPTION:
*     Get the 'portGroupsBmp'
*
Arguments:
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrMngPortGroupsBmpGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_U8           devNum;
    GT_BOOL         enable;
    GT_PORT_GROUPS_BMP  portGroupsBmp;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    rc = utilMultiPortGroupsBmpGet(devNum,&enable,&portGroupsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d",enable,portGroupsBmp);
    return CMD_OK;
}

/*******************************************************************************
* allowProcessingOfAuqMessages (general command)
*
* DESCRIPTION:
*     Function to allow set the flag of : allowProcessingOfAuqMessages
*
* INPUTS:
*       enable - GT_TRUE: allow the processing of the AUQ messages
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrAllowProcessingOfAuqMessages
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       rc;
    GT_BOOL         enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    enable = (GT_BOOL)inArgs[0];

    rc = cmdAppAllowProcessingOfAuqMessages(enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/******************************************************************************
* wrTraceHwAccessEnable
*
* DESCRIPTION:
*       Trace HW read access information.
*
* INPUTS:
*       devNum      - PP device number
*       accessType  - access type: read or write
*       enable      - GT_TRUE: enable tracing for given access type
*                     GT_FALSE: disable tracing for given access type
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on bad access type
*
* COMMENTS:
*       None.
*
******************************************************************************/
static CMD_STATUS wrTraceHwAccessEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           rc;
    GT_U8                               devNum;
    GT_U32                              accessType;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    accessType = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    rc = cmdAppTraceHwAccessEnable(devNum, accessType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}


/******************************************************************************
* appDemoTraceHwAccessOutputModeSet
*
* DESCRIPTION:
*       Set output tracing mode.
*
* INPUTS:
*       devNum      - PP device number
*       mode        - output tracing mode: print, printSync or store.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - on bad state
*       GT_BAD_PARAM - on bad mode
*
* COMMENTS:
*       It is not allowed to change mode, while one of the HW Access DB
*       is enabled.
*
******************************************************************************/
static CMD_STATUS wrTraceHwAccessOutputModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        rc;
    GT_U32                          mode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    mode = (GT_U32)inArgs[0];

    rc = cmdAppTraceHwAccessOutputModeSet(mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************/
/* table simulationDynamicSlan */
/*******************************/

/* SW devNum */
static GT_U8   simulationDynamicSlan_swDevNum = 0;
/* global physical port number */
static GT_PHYSICAL_PORT_NUM   simulationDynamicSlan_globalPhysicalPort = 0;
/* simulation deviceId */
static GT_U32   simulationDynamicSlan_simDeviceId = 0xFFFFFFFF;

#ifdef ASIC_SIMULATION
    static char simulationDynamicSlan_slanName[20];

    #define GT_BIT  GT_U32
    #include <asicSimulation/SKernel/smain/smain.h>

    #ifdef CHX_FAMILY
        #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
        #define UTF_CPSS_PP_MAX_PHYSICAL_PORTS_NUM_CNS(_devNum) \
            PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(_devNum)
    #else /* ! CHX_FAMILY */
        #define UTF_CPSS_PP_MAX_PHYSICAL_PORTS_NUM_CNS(_devNum)     64
    #endif  /* ! CHX_FAMILY */

    #if ((defined GT_PCI) || (defined GT_PEX))
        #define SUPPORT_SIM_CONVERT
    #endif/*((defined GT_PCI) || (defined GT_PEX))*/

    /* convert CPSS devNum, coreId to simulation deviceId */
    extern GT_STATUS prvCpssDrvDevNumToSimDevIdConvert
    (
        IN  GT_U8       devNum,
        IN  GT_U32      portGroupId,
        OUT GT_U32      *simDeviceIdPtr
    );


    static GT_U32   getSlanIdFromName
    (
        IN char*    slanName
    )
    {
        GT_U32      slanId;
        if(slanName[0] == '\0' || slanName[0] != 's' ||
           slanName[1] == '\0' || slanName[1] != 'l' ||
           slanName[2] == '\0' || slanName[2] != 'a' ||
           slanName[3] == '\0' || slanName[3] != 'n' ||
           slanName[4] == '\0' ||
           slanName[5] == '\0' )
        {
            slanId = 99999;
        }
        else if(slanName[4] < '0' || slanName[4] > '9' ||
                slanName[5] < '0' || slanName[5] > '9')
        {
            slanId = 88888;
        }
        else if(slanName[6] != '\0')
        {
            slanId = 77777;
        }
        else
        {
            /* the string hold decimal number between 00..99 */
            slanId = ((slanName[4] - '0') * 10) + (slanName[5] - '0');
        }

        return slanId;
    }

    extern GT_STATUS   getSimDevIdFromSwDevNum
    (
        IN GT_U8    swDevNum,
        IN  GT_U32  portGroupId,
        OUT GT_U32  *simDeviceIdPtr
    );

    static GT_STATUS    getPortSlanName
    (
        IN GT_U32       swDevNum,
        IN GT_U32       simDeviceId,
        IN GT_BOOL      isFirst,
        IN  GT_PHYSICAL_PORT_NUM  startGlobalPort,
        OUT GT_PHYSICAL_PORT_NUM  *currentGlobalPortPtr
    )
    {
        GT_STATUS rc;
        GT_U32      ii;
        GT_U32      slanMaxLength = 20;
        GT_BOOL     portBound;

        if(isFirst == GT_TRUE)
        {
            ii = 0;
        }
        else
        {
            ii = startGlobalPort;
        }

        do
        {
            portBound = GT_FALSE;

            if(isFirst == GT_TRUE)
            {
                isFirst = GT_FALSE;
                ii--;/* compensation for the ++ that will come next step */
            }

            do
            {
                ii++;

                if(ii >= UTF_CPSS_PP_MAX_PHYSICAL_PORTS_NUM_CNS(swDevNum))
                {
                    /* no more physical ports for this device */
                    return GT_NO_MORE;
                }

                if(1 == PRV_CPSS_PHY_PORT_IS_EXIST_MAC(swDevNum,ii)
                    || ii == CPSS_CPU_PORT_NUM_CNS)
                {
                    /* next valid port / CPU */
                    break;
                }

            }while(1);

            rc = skernelDevPortSlanGet(simDeviceId,
                                  ii,
                                  slanMaxLength,
                                  &portBound,
                                  simulationDynamicSlan_slanName);
            if(rc != GT_OK &&
               ii != CPSS_CPU_PORT_NUM_CNS)/*fix CQ#131878*/
            {
                return rc;
            }

            if(portBound == GT_TRUE)
            {
                /* this port is bound */
                break;
            }
        }
        while(1);

        *currentGlobalPortPtr = (GT_PHYSICAL_PORT_NUM)ii;

        return GT_OK;
    }

#endif /*ASIC_SIMULATION*/

/*******************************************************************************
* wrSimulationDynamicSlanGet (refresh command)
*
* DESCRIPTION:
*     get info about the next port bound to slan
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrSimulationDynamicSlanGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#ifdef ASIC_SIMULATION
    GT_STATUS   rc;
    GT_U32      slanId;
    GT_BOOL     isFirst = GT_FALSE;

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(simulationDynamicSlan_swDevNum))
    {
        goto endOfTable_lbl;
    }

    if(simulationDynamicSlan_simDeviceId == 0xFFFFFFFF)
    {
        rc = getSimDevIdFromSwDevNum(simulationDynamicSlan_swDevNum,0,
                               &simulationDynamicSlan_simDeviceId);
        if(rc != GT_OK)
        {
            goto endOfTable_lbl;
        }

        isFirst = GT_TRUE;
    }

    rc = getPortSlanName(simulationDynamicSlan_swDevNum,
                         simulationDynamicSlan_simDeviceId,
                         isFirst,
                         simulationDynamicSlan_globalPhysicalPort,
                         &simulationDynamicSlan_globalPhysicalPort);
    if(rc != GT_OK)
    {
        goto endOfTable_lbl;
    }

    slanId = getSlanIdFromName(simulationDynamicSlan_slanName);
    if(slanId >= 100)
    {
        /*indication that slan name is not in the expected format */
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d", simulationDynamicSlan_globalPhysicalPort, slanId, 0);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;

endOfTable_lbl:
#endif /*! ASIC_SIMULATION*/

    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

static CMD_STATUS wrSimulationDynamicSlanGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    simulationDynamicSlan_globalPhysicalPort = 0;
    simulationDynamicSlan_swDevNum = inArgs[0];
    simulationDynamicSlan_simDeviceId = 0xFFFFFFFF;

    return wrSimulationDynamicSlanGet(inArgs,inFields,numFields,outArgs);
}

static CMD_STATUS wrSimulationDynamicSlanGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    return wrSimulationDynamicSlanGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrSimulationDynamicSlanSet (general command)
*
* DESCRIPTION:
*     bind the port to needed slan , with option to disconnect other ports from
*     this slan (in any device)
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrSimulationDynamicSlanSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc = GT_FAIL;
#ifdef ASIC_SIMULATION
    GT_U8   swDevNum;
    GT_U32  simDeviceId;
    GT_U32  portNum;
    GT_U32      slanId;
    GT_BOOL      unbindOtherPortsOnThisSlan;
    char    slanName[7] = {'s','l','a','n','X','X',0};

    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    swDevNum = inArgs[0];
    portNum = inFields[0];
    slanId =  inFields[1];
    unbindOtherPortsOnThisSlan = (GT_BOOL)inFields[2];

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(swDevNum))
    {
        rc = GT_BAD_PARAM;
        goto error_lbl;
    }

    if(slanId >= 100)
    {
        rc = GT_BAD_PARAM;
        goto error_lbl;
    }

    /* convert to simulation deviceId */
    rc = getSimDevIdFromSwDevNum(swDevNum,0,
                           &simDeviceId);
    if(rc != GT_OK)
    {
        goto error_lbl;
    }

    slanName[4] = '0' + (slanId / 10);
    slanName[5] = '0' + (slanId % 10);

    /* bind the port */
    rc = skernelBindDevPort2Slan(simDeviceId,portNum,slanName,unbindOtherPortsOnThisSlan);
    if(rc != GT_OK)
    {
        goto error_lbl;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;

error_lbl:
#endif/*ASIC_SIMULATION*/
    galtisOutput(outArgs, rc, "");
    return CMD_OK;

}

/*******************************************************************************
* wrSimulationDynamicSlanDelete (general command)
*
* DESCRIPTION:
*     unbind the port from it's slan
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrSimulationDynamicSlanDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc = GT_FAIL;
#ifdef ASIC_SIMULATION
    GT_U8   swDevNum;
    GT_U32  simDeviceId;
    GT_U32  portNum;

    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    swDevNum = inArgs[0];
    portNum = inFields[0];

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(swDevNum))
    {
        rc = GT_BAD_PARAM;
        goto error_lbl;
    }

    /* convert to simulation deviceId */
    rc = getSimDevIdFromSwDevNum(swDevNum,0,
                           &simDeviceId);
    if(rc != GT_OK)
    {
        goto error_lbl;
    }

    /* unbind the port */
    rc = skernelUnbindDevPort2Slan(simDeviceId,portNum);
    if(rc != GT_OK)
    {
        goto error_lbl;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;

error_lbl:
#endif/*ASIC_SIMULATION*/
    galtisOutput(outArgs, rc, "");
    return CMD_OK;

}

#ifdef ASIC_SIMULATION
static GT_BOOL forceAll = GT_FALSE;
extern GT_STATUS wrSimulationDynamicSlanDevDelete_forceAll(IN GT_BOOL force)
{
    forceAll = force;
    return GT_OK;
}
/*******************************************************************************
* wrSimulationDynamicSlanDevDelete
*
* DESCRIPTION:
*     unbind from slans all ports that bound to slans 00..99.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
extern GT_STATUS wrSimulationDynamicSlanDevDelete(IN GT_U8 swDevNum)
{
    GT_STATUS   rc;
    GT_U32  simDeviceId;
    GT_U32  portNum = 0;
    GT_U32      slanId;

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(swDevNum))
    {
        rc = GT_BAD_PARAM;
        return rc;
    }

    /* convert to simulation deviceId */
    rc = getSimDevIdFromSwDevNum(swDevNum,0,
                           &simDeviceId);
    if(rc != GT_OK)
    {
        return rc;
    }

    do
    {
        rc = getPortSlanName(swDevNum,
                             simDeviceId,
                             (portNum == 0) ? GT_TRUE : GT_FALSE,
                             portNum,
                             &portNum);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(forceAll == GT_TRUE && portNum != CPSS_CPU_PORT_NUM_CNS)
        {
            slanId = 0;/* the port forced to be unbound */
        }
        else
        {
            slanId = getSlanIdFromName(simulationDynamicSlan_slanName);
        }
        if(slanId >= 100)
        {
            /*indication that slan name is not in the expected format */
            /* we ignore it */
        }
        else
        {
            /* unbind the port */
            rc = skernelUnbindDevPort2Slan(simDeviceId,portNum);
            if(rc != GT_OK)
            {
                return rc;
            }

            cmdOsTimerWkAfter(10);/* allow the interrupts due to link change to be over before we unbind next port */
        }
    }while(1);

    return GT_OK;
}
#endif/*ASIC_SIMULATION*/

/*******************************************************************************
* wrSimulationDynamicSlanClear (general command)
*
* DESCRIPTION:
*     unbind from slans all ports that bound to slans 00..99.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrSimulationDynamicSlanClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc = GT_FAIL;
#ifdef ASIC_SIMULATION
    GT_U8   swDevNum;

    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    swDevNum = inArgs[0];

    rc = wrSimulationDynamicSlanDevDelete(swDevNum);
#endif/*ASIC_SIMULATION*/
    galtisOutput(outArgs, rc, "");
    return CMD_OK;

}

/*******************************************************************************
* osMemStartHeapAllocationCounter
*
* DESCRIPTION:
*        set the current value of heap allocated bytes to the allocation counter
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrOsMemStartHeapAllocationCounter
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32       numFields,
    OUT GT_8        outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call api function */
    cmdAppStartHeapAllocCounter();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* osMemGetHeapAllocationCounter
*
* DESCRIPTION:
*        returns the delta of current allocated bytes number and the value of allocation counter set by
*        preveous startHeapAllocationCounter() function
*
* INPUTS:
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrOsMemGetHeapAllocationCounter
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32       numFields,
    OUT GT_8        outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 counter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call api function */
    counter = cmdAppGetHeapAllocCounter();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", counter);
    return CMD_OK;
}

/*******************************************************************************
* appDemoEventFatalErrorEnable
*
* DESCRIPTION:
*       Enable/Disable execution of osFatalError from event handler.
*
* INPUTS:
*       enable  - GT_TRUE - Enable execution of osFatalError from event handler.
*                 GT_FALSE - Disable execution of osFatalError from event handler.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrEventFatalErrorEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           rc;
    GT_32                               enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    enable = (GT_32)inArgs[0];

    rc = cmdAppEventFatalErrorEnable(enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* generalDeviceInfoGet
*
* DESCRIPTION:
*       return to the caller general device info
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrGeneralDeviceInfoGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
/*    GT_U32                               devNum; */
    typedef enum{
         DEVICE_MODEL_NON_WM_E     /* device is NOT white mode (black mode) */
        ,DEVICE_MODEL_WM_NON_GM_E  /* device is white mode , NON GM */
        ,DEVICE_MODEL_WM_GM_E      /* device is white mode and GM */
    }WR_DEVICE_MODEL_ENT;
    WR_DEVICE_MODEL_ENT    deviceModel ;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* currently the devNum not used ... but it is needed to support devices */
/*    devNum = (GT_U32)inArgs[0]; */

#ifdef ASIC_SIMULATION
    #ifdef GM_USED
        deviceModel = DEVICE_MODEL_WM_GM_E;  /*device is white mode and GM*/
    #else /*! GM_USED*/
        deviceModel = DEVICE_MODEL_WM_NON_GM_E;/*device is white mode , NON GM*/
    #endif /*! GM_USED*/
#else /*! ASIC_SIMULATION*/
        deviceModel = DEVICE_MODEL_NON_WM_E;/* device is NOT white mode (black mode) */
#endif/*! ASIC_SIMULATION*/

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d" , deviceModel);
    return CMD_OK;
}

/*******************************************************************************
* appDemoPpConfigPrint
*
* DESCRIPTION:
*       Return system configuration that was set by user .
*
* INPUTS:
*       devNum              - device number.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK               - on success
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrPpConfigPrint
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_STATUS result;
     GT_U8     devNum;

   /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    result = cmdAppPpConfigPrint(devNum);

   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] = {

    {"mngInitSystem",
        &wrMngInitSystem,
        0, 0},

    {"mngGetSysFirstElement",
        &wrMngGetSysFirstElement,
        0, 0},

    {"mngGetSysNextElement",
        &wrMngGetSysNextElement,
        0, 0},

    {"sys",
        &wrSys,
        0, 0},


    {"mngPortDevConvertSet",
     &wrMngPortDevConvertSet,
     0, 5},

    {"mngPortDevConvertGetFirst",
     &wrMngPortDevConvertGetFirst,
     0, 0},

    {"mngPortDevConvertGetNext",
     &wrMngPortDevConvertGetNext,
     0, 0},

    {"mngPortDevConvertClear",
     &wrMngPortDevConvertClear,
     0, 0},

    {"mngPhysicalPortDevConvertSet",
    &wrMngPhysicalPortDevConvertSet,
    0, 5},

    {"mngPhysicalPortDevConvertGetFirst",
    &wrMngPhysicalPortDevConvertGetFirst,
    0, 0},

    {"mngPhysicalPortDevConvertGetNext",
    &wrMngPhysicalPortDevConvertGetNext,
    0, 0},

    {"mngPhysicalPortDevConvertClear",
    &wrMngPhysicalPortDevConvertClear,
    0, 0},

    /* start table mngPortGroupConvert */
    {"mngPortGroupConvertSet",
     &wrMngPortGroupConvertSet,
     0, 5},

    {"mngPortGroupConvertGetFirst",
     &wrMngPortGroupConvertGetFirst,
     0, 0},

    {"mngPortGroupConvertGetNext",
     &wrMngPortGroupConvertGetNext,
     0, 0},

    {"mngPortGroupConvertClear",
     &wrMngPortGroupConvertClear,
     0, 0},
    /* end table mngPortGroupConvert */

    {"mngPortGroupsBmpEnable",
        &wrMngPortGroupsBmpEnable,
        2, 0},

    {"mngPortGroupsBmpSet",
        &wrMngPortGroupsBmpSet,
        10, 0},

    {"mngPortGroupsBmpGet",
        &wrMngPortGroupsBmpGet,
        1, 0},

    {"allowProcessingOfAuqMessages",
        &wrAllowProcessingOfAuqMessages,
        1, 0},

    {"traceHwAccessEnable",
        &wrTraceHwAccessEnable,
        3, 0},

    {"traceHwAccessOutputModeSet",
        &wrTraceHwAccessOutputModeSet,
        1, 0},

    {"wrCpssInitSystem",
     &wrCpssInitSystem,
     3, 0},

    {"wrCpssInitSystemGet",
     &wrCpssInitSystemGet,
     0, 0},

    {"wrCpssResetSystem",
     &wrCpssResetSystem,
     0, 0},

    {"confi2InitSystem",
     &wrConfi2InitSystem,
     2, 0},

    {"eventFatalErrorEnable",
        &wrEventFatalErrorEnable,
        1, 0},

    /* start table simulationDynamicSlan */

    {"simulationDynamicSlanSet",
     &wrSimulationDynamicSlanSet,
     1, 3},

    {"simulationDynamicSlanDelete",
     &wrSimulationDynamicSlanDelete,
     1, 3},

    {"simulationDynamicSlanGetFirst",
     &wrSimulationDynamicSlanGetFirst,
     1, 0},

    {"simulationDynamicSlanGetNext",
     &wrSimulationDynamicSlanGetNext,
     1, 0},

    {"simulationDynamicSlanClear",
     &wrSimulationDynamicSlanClear,
     1, 0},
    /* end table simulationDynamicSlan */

    {"osMemStartHeapAllocationCounter",
     &wrOsMemStartHeapAllocationCounter,
     0, 0},
    {"osMemGetHeapAllocationCounter",
     &wrOsMemGetHeapAllocationCounter,
     0, 0},

     {"generalDeviceInfoGet",
        &wrGeneralDeviceInfoGet,
        1, 0},

     {"ppConfigPrint",
      &wrPpConfigPrint,
      1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitSysconfig
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
GT_STATUS cmdCpssLibInitSysconfig()
{
    return cmdInitLibrary(dbCommands, numCommands);
}

