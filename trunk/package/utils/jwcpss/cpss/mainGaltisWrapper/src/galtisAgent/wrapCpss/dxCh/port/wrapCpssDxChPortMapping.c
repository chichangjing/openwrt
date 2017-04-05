/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChPortMapping.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS DXCH Port Mapping API
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

static CPSS_DXCH_PORT_MAP_STC   portMapArray[256];
static GT_U32                   portMapNum;
static GT_U32                   currPortMap;
static GT_U32                   numEntriesReturned = 0;/* number of entries that the 'refresh' already returned to galtis terminal */

/*******************************************************************************
* cpssDxChPortPhysicalPortMapSet
*
* DESCRIPTION:
*       configure ports mapping
*
* APPLICABLE DEVICES:
*        Bobcat2; Lion3
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portMapArraySize - Number of ports to map, array size
*       portMapArrayPtr  - pointer to array of mappings
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - wrong devNum, portNum
*       GT_BAD_PTR            - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. when CPSS_DXCH_CFG_PORT_MAPPING_TYPE_INLK_CHANNEL_E  is defined the first channel
*          phsical port is used for DMA configuration
*       2. The function checks whether all mapping are consistent.
*          In case of contradiction between mappings
*          (e,g. single TxQ port is mapped to several Tx DMA ports) the function returns with BAD_PARAM.
*       3. Other CPSS APIs that receive GT_PHYSICAL_PORT_NUM as input parameter will use this mapping
*          to convert the physical port to mac/dma/txq  to configure them
*       4. Usage example
*        #define GT_NA ((GT_U32)-1)
*        CPSS_DXCH_CFG_PORT_MAP_STC bc2ExampleMap[] =
*        {
*            {  0, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      0,  0, GT_FALSE},
*            {  1, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      1,  1, GT_FALSE},
*            { 50, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     50,  2, GT_FALSE},
*            { 53, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     53,  3, GT_FALSE},
*            { 63, _INTRFACE_TYPE_CPU_SDMA_E,     GT_NA,  GT_NA, 71, GT_FALSE},
*            {100, _INTRFACE_TYPE_ILNK_MAC_E,     GT_NA,  GT_NA, 40, GT_TRUE},
*            {101, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 41, GT_TRUE},
*            {102, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 42, GT_TRUE},
*            {103, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 43, GT_TRUE}
*        };
*        rc = cpssDxChCfgPhysicalPortMapSet(devNum, sizeof(bc2ExampleMap)/sizeof(bc2ExampleMap[0]), &bc2ExampleMap[0]);
*
*
********************************************************************************/
static CMD_STATUS internalPortPhysicalPortMapSet
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
    GT_U32                                  portMapArraySize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(opCode == 0)/*start registration*/
    {
        portMapNum = 0;
    }
    else if(opCode == 0xFFFFFFFF)/*end registration*/
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        /* Dummy port - for device override */
        port = portMapArray[0].physicalPortNumber;

        /* Override Device and Port */
        CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

        portMapArraySize = portMapNum;

        result = cpssDxChPortPhysicalPortMapSet(devNum, portMapArraySize, portMapArray);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d",-1);/* Error ! */
            return CMD_OK;
        }
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    if (portMapNum > 255)
    {
        galtisOutput(outArgs, GT_FULL, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    port = (GT_PHYSICAL_PORT_NUM)inFields[0];
    devNum = (GT_U8)inArgs[0];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    portMapArray[portMapNum].physicalPortNumber = port;
    portMapArray[portMapNum].mappingType = (CPSS_DXCH_PORT_MAPPING_TYPE_ENT)inFields[1];
    portMapArray[portMapNum].portGroup = (GT_U32)inFields[2];
    portMapArray[portMapNum].interfaceNum = (GT_U32)inFields[3];
    portMapArray[portMapNum].txqPortNumber = (GT_U32)inFields[4];
    portMapArray[portMapNum].tmEnable = (GT_BOOL)inFields[5];
    portMapArray[portMapNum].tmPortInd = (GT_U32)inFields[6];
    portMapNum++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPortPhysicalPortMapSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalPortPhysicalPortMapSet(inArgs,inFields,numFields,outArgs,0);
}

static CMD_STATUS wrCpssDxChPortPhysicalPortMapSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalPortPhysicalPortMapSet(inArgs,inFields,numFields,outArgs,1);
}

static CMD_STATUS wrCpssDxChPortPhysicalPortMapEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalPortPhysicalPortMapSet(inArgs,inFields,numFields,outArgs,0xFFFFFFFF);
}


/*******************************************************************************
* cpssDxChPortPhysicalPortMapGet
*
* DESCRIPTION:
*       get ports mappping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       firstPhysicalPortNumber - first physical port to bring
*       portMapArraySize - Mapping array size
*
* OUTPUTS:
*       portMapArrayPtr  - pointer to ports mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_FAIL         - when cannot deliver requested number of items
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPhysicalPortMapGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32          portMapArraySize;
    GT_STATUS       result;
    GT_STATUS       result1;
    GT_U8           devNum;
    GT_BOOL         isValid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    portMapArraySize = (GT_U32)inArgs[2];

    while(1)
    {
        if((numEntriesReturned >= portMapArraySize)/* we returned the max needed */
            ||
           (currPortMap >= 256))                   /* we checked all 256 possible ports */
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        result = cpssDxChPortPhysicalPortMapGet(devNum, currPortMap, 1, portMapArray);
        result1 = cpssDxChPortPhysicalPortMapIsValidGet(devNum, currPortMap, &isValid);

        currPortMap++;

        if(isValid != GT_TRUE)
        {
            continue;
        }

        if((result != GT_OK) || (result1 != GT_OK))
        {
            continue;
        }
        else
        {
            break;
        }
    }

    numEntriesReturned++;/* we returned another entry */

    inFields[0] = portMapArray[0].physicalPortNumber;
    inFields[1] = portMapArray[0].mappingType;
    inFields[2] = portMapArray[0].portGroup;
    inFields[3] = portMapArray[0].interfaceNum;
    inFields[4] = portMapArray[0].txqPortNumber;
    inFields[5] = portMapArray[0].tmEnable;
    inFields[6] = portMapArray[0].tmPortInd;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortPhysicalPortMapGet
*
* DESCRIPTION:
*       get ports mappping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       firstPhysicalPortNumber - first physical port to bring
*       portMapArraySize - Mapping array size
*
* OUTPUTS:
*       portMapArrayPtr  - pointer to ports mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_FAIL         - when cannot deliver requested number of items
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPhysicalPortMapGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8           devNum;
    GT_U32          firstPhysicalPortNumber;
    GT_U32          portMapArraySize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    numEntriesReturned = 0;/* state that no entry was returned */

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    firstPhysicalPortNumber = (GT_U32)inArgs[1];
    portMapArraySize = (GT_U32)inArgs[2];

    if(portMapArraySize == 0)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%s", "portMapArraySize==0");
        return CMD_OK;
    }
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, firstPhysicalPortNumber);

    currPortMap = firstPhysicalPortNumber;

    return wrCpssDxChPortPhysicalPortMapGetNext(inArgs,inFields,numFields,outArgs);
}

/******************************************************************************
* wrCpssDxChPortPhysicalPortMapUpdateOrAdd
*
* DESCRIPTION:
*       The function add new physicalPortNumber mapping or update existing physicalPortNumber mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       mode      - define loopback type or no loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
******************************************************************************/
CMD_STATUS wrCpssDxChPortPhysicalPortMapUpdateOrAdd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_STATUS       result1;
    GT_U8           devNum;
    GT_BOOL         isValid;
    CPSS_DXCH_PORT_MAP_STC  newInfo;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    index = 0;

    devNum                      = (GT_U8)inArgs[index++];  /*0*/
    newInfo.physicalPortNumber  = (GT_PHYSICAL_PORT_NUM)inArgs[index++];  /*1*/
    newInfo.mappingType         = (CPSS_DXCH_PORT_MAPPING_TYPE_ENT) inArgs[index++];  /*2*/
    newInfo.portGroup           = (GT_U32)inArgs[index++];  /*3*/
    newInfo.interfaceNum        = (GT_U32)inArgs[index++];  /*4*/
    newInfo.txqPortNumber       = (GT_U32)inArgs[index++];  /*5*/
    newInfo.tmEnable            = (GT_BOOL) inArgs[index++];  /*6*/
    newInfo.tmPortInd           = (GT_U32)inArgs[index++];  /*7*/

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, newInfo.physicalPortNumber);

    currPortMap = 0;
    index = 0;

    while((currPortMap < 256) && (index < 256))
    {
        result = cpssDxChPortPhysicalPortMapGet(devNum, currPortMap, 1, &portMapArray[index]);
        result1 = cpssDxChPortPhysicalPortMapIsValidGet(devNum, currPortMap, &isValid);

        currPortMap++;

        if(isValid != GT_TRUE)
        {
            continue;
        }
        else
        if((result != GT_OK) || (result1 != GT_OK))
        {
            continue;
        }

        if((currPortMap - 1) == newInfo.physicalPortNumber)
        {
            /* ignore the entry as we going to override it ...but we will put it
                as last entry in the array ... to unify code with 'new port' */
        }
        else
        {
            index++;/* valid entry */
        }
    }

    if(index >= 256)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_FULL, "");
        return CMD_OK;
    }

    /* add new port / update new port as the 'last entry' */
    portMapArray[index] = newInfo;
    index ++;

    result = cpssDxChPortPhysicalPortMapSet(devNum, index, portMapArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;

}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPortPhysicalPortMapSetFirst",
        &wrCpssDxChPortPhysicalPortMapSetFirst,
        3, 6},

    {"cpssDxChPortPhysicalPortMapSetNext",
        &wrCpssDxChPortPhysicalPortMapSetNext,
        3, 6},

    {"cpssDxChPortPhysicalPortMapEndSet",
        &wrCpssDxChPortPhysicalPortMapEndSet,
        3, 0},

    {"cpssDxChPortPhysicalPortMapGetFirst",
        &wrCpssDxChPortPhysicalPortMapGetFirst,
        3, 0},

    {"cpssDxChPortPhysicalPortMapGetNext",
        &wrCpssDxChPortPhysicalPortMapGetNext,
        3, 0},

    {"cpssDxChPortPhysicalPortMapUpdateOrAdd",
        &wrCpssDxChPortPhysicalPortMapUpdateOrAdd,
        8, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortMapping
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
GT_STATUS cmdLibInitCpssDxChPortMapping
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

