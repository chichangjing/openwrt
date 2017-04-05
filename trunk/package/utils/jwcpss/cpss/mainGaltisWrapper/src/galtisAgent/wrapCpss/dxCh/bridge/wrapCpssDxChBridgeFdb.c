/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapBridgeFdbCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for BridgeFdb cpss.dxCh functions
*
* FILE REVISION NUMBER:
*       $Revision: 56 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbRouting.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdbHash.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* DxCh max number of entries to be read from tables\classes */
#define  CPSS_DXCH_FDB_MAX_ENTRY_CNS 16384

/* Flag for age bit setting.
   GT_TRUE  - set to default value
   GT_FALSE - set age bit in FDB entry according to given value */
static GT_BOOL  setAgeToDefault = GT_TRUE;

/* support for multi port groups */
static GT_BOOL             multiPortGroupsBmpEnable = GT_FALSE;
static GT_PORT_GROUPS_BMP  multiPortGroupsBmp;

/* current port group for the 'refresh' FDB */
static GT_BOOL useRefreshFdbPerPortGroupId = GT_FALSE;
/* current port group for the 'refresh' FDB */
static GT_U32  refreshFdbCurrentPortGroupId = 0;
/* first port group for the 'refresh' FDB */
static GT_U32  refreshFdbFirstPortGroupId = 0;

static  GT_U32     fdbEntryIndex;      /* first index to get from FDB Table */
static  GT_U32     fdbEntryNumOfEntries;/* for the refresh of FDB Table */
static  GT_U32     fdbEntryIndexMaxGet;/* for the refresh of FDB Table */


/*
   !!! for FDB purposes ONLY !!!
   Get the portGroupsBmp for multi port groups device.
   when 'enabled' --> wrappers will use the APIs with portGroupsBmp parameter*/
#define FDB_MULTI_PORT_GROUPS_BMP_GET(device,enable,portGroupBmp)  \
    fdbMultiPortGroupsBmpGet(device,&enable,&portGroupBmp);

/*******************************************************************************
* fdbMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       !!! for FDB purposes ONLY !!!
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs with portGroupsBmp parameter
*
*                   with port group bmp parameter    |     NO port group bmp parameter
*       =====================================================================================
*       cpssDxChBrgFdbPortGroupMacEntrySet           |   cpssDxChBrgFdbMacEntrySet
*       cpssDxChBrgFdbPortGroupMacEntryDelete        |   cpssDxChBrgFdbMacEntryDelete
*       cpssDxChBrgFdbPortGroupMacEntryWrite         |   cpssDxChBrgFdbMacEntryWrite
*       cpssDxChBrgFdbPortGroupMacEntryRead          |   cpssDxChBrgFdbMacEntryRead
*       cpssDxChBrgFdbPortGroupMacEntryStatusGet     |   cpssDxChBrgFdbMacEntryStatusGet
*       cpssDxChBrgFdbPortGroupMacEntryInvalidate    |   cpssDxChBrgFdbMacEntryInvalidate
*       cpssDxChBrgFdbPortGroupQaSend                |   cpssDxChBrgFdbQaSend
*       cpssDxChBrgFdbPortGroupMacEntryAgeBitSet     |   cpssDxChBrgFdbMacEntryAgeBitSet
*
*       explicit new API :
*       cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet |   cpssDxChBrgFdbFromCpuAuMsgStatusGet
*
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void fdbMultiPortGroupsBmpGet
(
    IN  GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    GT_STATUS   rc;

    *enablePtr  = GT_FALSE;

    if((0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) ||
       (0 == PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum)))
    {
        /* check for valid device , because we need to check :
        PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode   */
        return;
    }

    rc = utilMultiPortGroupsBmpGet(devNum,enablePtr,portGroupsBmpPtr);
    if(rc != GT_OK)
    {
        return;
    }

    if((PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode ==
        PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E) &&
       (*enablePtr == GT_TRUE))
    {
        /* in unified mode the FDB must be used with CPSS_PORT_GROUP_UNAWARE_MODE_CNS */
        *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    return;
}

/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbMacEntrySet
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr
)
{
    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntrySet(devNum,macEntryPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntrySet(devNum,multiPortGroupsBmp,macEntryPtr);
    }
}

/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbMacEntryDelete
(
    IN GT_U8                        devNum,
    IN CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryDelete(devNum,macEntryKeyPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryDelete(devNum,multiPortGroupsBmp,macEntryKeyPtr);
    }
}


/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbFromCpuAuMsgStatusGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *completedPtr,
    OUT GT_BOOL  *succeededPtr
)
{
    /* call this API anyway because prototype of function of
       cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(...) was changed with
       pointers to BMPs */
    return cpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum,completedPtr,succeededPtr,NULL);
}


/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbMacEntryWrite
(
    IN GT_U8                        devNum,
    IN GT_U32                       index,
    IN GT_BOOL                      skip,
    IN CPSS_MAC_ENTRY_EXT_STC       *macEntryPtr

)
{
    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryWrite(devNum,index,skip,macEntryPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryWrite(devNum,multiPortGroupsBmp,index,skip,macEntryPtr);
    }
}


/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbMacEntryRead
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr,
    OUT GT_BOOL                 *agedPtr,
    OUT GT_HW_DEV_NUM           *associatedHwDevNumPtr,
    OUT CPSS_MAC_ENTRY_EXT_STC  *entryPtr
)
{
    /*  a call to fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);
    is done on the 'getFirst' entry */

    if(useRefreshFdbPerPortGroupId == GT_TRUE)
    {
        return cpssDxChBrgFdbPortGroupMacEntryRead(devNum,
            (1 << refreshFdbCurrentPortGroupId),/* bmp of explicit port group */
            index,validPtr,skipPtr,
            agedPtr,associatedHwDevNumPtr,entryPtr);
    }
    else if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryRead(devNum,index,validPtr,skipPtr,
            agedPtr,associatedHwDevNumPtr,entryPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryRead(devNum,multiPortGroupsBmp,index,
                validPtr,skipPtr,agedPtr,associatedHwDevNumPtr,entryPtr);
    }
}


/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  index,
    OUT GT_BOOL                 *validPtr,
    OUT GT_BOOL                 *skipPtr
)
{
    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryStatusGet(devNum,index,validPtr,skipPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryStatusGet(devNum,multiPortGroupsBmp,index,validPtr,skipPtr);
    }
}


/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbMacEntryInvalidate
(
    IN GT_U8         devNum,
    IN GT_U32        index
)
{
    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryInvalidate(devNum,index);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryInvalidate(devNum,multiPortGroupsBmp,index);
    }
}

/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbQaSend
(
    IN  GT_U8                       devNum,
    IN  CPSS_MAC_ENTRY_EXT_KEY_STC   *macEntryKeyPtr
)
{
    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbQaSend(devNum,macEntryKeyPtr);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupQaSend(devNum,multiPortGroupsBmp,macEntryKeyPtr);
    }
}


/* util function to decide if to use 'old' or 'new' API with the portGroupsBmp parameter */
static GT_STATUS wrPrvCpssDxChBrgFdbMacEntryAgeBitSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_BOOL      age
)
{
    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        return cpssDxChBrgFdbMacEntryAgeBitSet(devNum,index,age);
    }
    else
    {
        return cpssDxChBrgFdbPortGroupMacEntryAgeBitSet(devNum,multiPortGroupsBmp,index,age);
    }
}



/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortSet
*
* DESCRIPTION:
*       Enable/disable forwarding a new mac address message to CPU --per port
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       port    - port num
*       enable  - If GT_TRUE, forward NA message to CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on NULL pointer
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaToCpuPerPortSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_PORT_NUM      port;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbNaToCpuPerPortSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusGet
*
* DESCRIPTION:
*       Get state of new source MAC addresses learning on packets received
*       on specified port.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       statusPtr - (pointer to) GT_TRUE for enable  or GT_FALSE otherwise
*       cmdPtr    - (pointer to) how to forward packets with unknown/changed SA,
*                   when (*statusPtr) is GT_FALSE
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbPortLearnStatusGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_PORT_NUM                     port;
    GT_BOOL                   status;
    CPSS_PORT_LOCK_CMD_ENT    cmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbPortLearnStatusGet(devNum, port, &status,
                                                               &cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", status, cmd);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbPortLearnStatusSet
*
* DESCRIPTION:
*       Enable/disable learning of new source MAC addresses for packets received
*       on specified port.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum    - physical device number
*       port   - physical port number
*       status - GT_TRUE for enable  or GT_FALSE otherwise
*       cmd    - how to forward packets with unknown/changed SA,
*                 if status is GT_FALSE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on unknown command
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbPortLearnStatusSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_PORT_NUM                     port;
    GT_BOOL                   status;
    CPSS_PORT_LOCK_CMD_ENT    cmd;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    status = (GT_U8)inArgs[2];
    cmd = (GT_U8)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbPortLearnStatusSet(devNum, port, status, cmd);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbNaToCpuPerPortGet
*
* DESCRIPTION:
*       Get Enable/disable forwarding a new mac address message to CPU --
*       per port.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum     - physical device number
*       port    - port num
*
* OUTPUTS:
*       enablePtr  - If GT_TRUE, NA message is forwarded to CPU.
*
* RETURNS:
*       GT_OK        - on success
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - on NULL pointer
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       function also called from cascade management
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaToCpuPerPortGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_PORT_NUM                     port;
    GT_BOOL                   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbNaToCpuPerPortGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventSet
*
* DESCRIPTION:
*   Enable/Disable New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:  All DXCH devices
* INPUTS:
*       devNum     - device number
*       port       - port number
*       enable     - GT_TRUE - enable NA Storm Prevention (SP),
*                    GT_FALSE - disable NA Storm Prevention
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success,
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum/portNum/auMsgType
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaStormPreventSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_PORT_NUM    port;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbNaStormPreventSet(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbNaStormPreventGet
*
* DESCRIPTION:
*   Get status of New Address messages Storm Prevention.
*   Controlled learning relies on receiving NA messages for new source MAC
*   address. To prevent forwarding multiple NA messages to the CPU for the
*   same source MAC address, when NA Storm Prevention (SP) is enabled, the
*   device auto-learns this address with a special Storm Prevention flag set in
*   the entry. Subsequent packets from this source address do not generate
*   further NA messages to the CPU. Packets destined to this MAC address however
*   are treated as unknown packets. Upon receiving the single NA
*   message, the CPU can then overwrite the SP entry with a normal FDB
*   forwarding entry.
*   Only relevant in controlled address learning mode.
*
* APPLICABLE DEVICES:  All DXCH devices
* INPUTS:
*       devNum     - device number
*       port       - port number
*
* OUTPUTS:
*       enablePtr     - pointer to the status of repeated NA CPU messages
*                       GT_TRUE - NA Storm Prevention (SP) is enabled,
*                       GT_FALSE - NA Storm Prevention is disabled
*
* RETURNS:
*       GT_OK               - on success,
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - wrong devNum/portNum/auMsgType
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaStormPreventGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_PORT_NUM                     port;
    GT_BOOL                   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbNaStormPreventGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortVid1LearningEnableSet
*
* DESCRIPTION:
*   Enable/Disable copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       enable     - GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbPortVid1LearningEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_PORT_NUM    port;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbPortVid1LearningEnableSet(
        dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortVid1LearningEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable status of copying Tag1 VID from packet to FDB entry
*   at automatic learning.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE - enable copying Tag1 VID from packet to FDB entry,
*                    GT_FALSE - disable, the Tag1Vid in FDB will be 0
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbPortVid1LearningEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_PORT_NUM                     port;
    GT_BOOL                   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbPortVid1LearningEnableGet(
        devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortRoutingEnableSet
*
* DESCRIPTION:
*   Set Enable/Disable using of FDB for Destination IP lookup for IP UC routing.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       protocol    - ipv4 or ipv6 or both
*       enable     - GT_TRUE  - enable using of FDB for DIP lookup for IP UC routing,
*                    GT_FALSE - disable using of FDB for DIP lookup for IP UC routing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbPortRoutingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        dev;
    GT_PORT_NUM                  port;
    CPSS_IP_PROTOCOL_STACK_ENT   protocol;
    GT_BOOL                      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev      = (GT_U8)inArgs[0];
    port     = (GT_PORT_NUM)inArgs[1];
    protocol = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    enable   = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingPortIpUcEnableSet(
        dev, port, protocol, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortRoutingEnableGet
*
* DESCRIPTION:
*   Get Enable/Disable using of FDB for Destination IP lookup for IP UC routing.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*       protocol    - ipv4 or ipv6
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - enable using of FDB for DIP lookup for IP UC routing,
*                    GT_FALSE - disable using of FDB for DIP lookup for IP UC routing
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong parameter value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbPortRoutingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        dev;
    GT_PORT_NUM                  port;
    CPSS_IP_PROTOCOL_STACK_ENT   protocol;
    GT_BOOL                      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev      = (GT_U8)inArgs[0];
    port     = (GT_PORT_NUM)inArgs[1];
    protocol = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutingPortIpUcEnableGet(
        dev, port, protocol, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbDeviceTableSet
*
* DESCRIPTION:
*       This function sets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*       So for proper work of PP the application must set the relevant bits of
*       all devices in the system prior to inserting FDB entries associated with
*       them
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - device number
*       devTableBmp - bmp of devices to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbDeviceTableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32   devTableBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    devTableBmp = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbDeviceTableSet(devNum, devTableBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbDeviceTableGet
*
* DESCRIPTION:
*       This function gets the device table of the PP.
*       the PP use this configuration in the FDB aging daemon .
*       once the aging daemon encounter an entry with non-exists devNum
*       associated with it , the daemon will DELETE the entry without sending
*       the CPU any notification.
*       a non-exists device determined by it's bit in the "device table"
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       devTableBmpPtr - pointer to bmp of devices to set.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbDeviceTableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U32                    devTableBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbDeviceTableGet(devNum, &devTableBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", devTableBmp);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbQaSend
*
* DESCRIPTION:
*       The function Send Query Address (QA) message to the hardware MAC address
*       table.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that QA message was processed by PP.
*       The PP sends Query Response message after QA processing.
*       An application can get QR message by general AU message get API.
*
* INPUTS:
*       devNum          - physical device number
*       macEntryKeyPtr  - pointer to mac entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum/vlan
*       GT_TIMEOUT - timeout of 30 seconds waiting for Signal of HW operation
*                    ended
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend and cpssDxChBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the QA message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbQaSend

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_MAC_ENTRY_EXT_KEY_STC          macEntryKey;
    GT_BYTE_ARRY                        sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    macEntryKey.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inFields[0];

    switch(inFields[0])
    {
    case 6:
        macEntryKey.vid1 = (GT_U16)inFields[5];
        /* no break */
    case 0:
        galtisMacAddr(&macEntryKey.key.macVlan.macAddr, (GT_U8*)inFields[1]);
        macEntryKey.key.macVlan.vlanId = (GT_U16)inFields[2];
        break;

    case 7:
    case 8:
        macEntryKey.vid1 = (GT_U16)inFields[5];
        /* no break */
    case 1:
    case 2:
        macEntryKey.key.ipMcast.vlanId = (GT_U16)inFields[2];

        galtisBArray(&sipBArr, (GT_U8*)inFields[3]);

        cmdOsMemCpy(
            macEntryKey.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[4]);

        cmdOsMemCpy(
            macEntryKey.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
        /* illegal key type */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbQaSend(devNum, &macEntryKey);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/* cpssDxChBrgFdbMac Table */
static GT_U32   indexCnt;

/*
* cpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum          - physical device number
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntrySetByIndex

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       index;
    GT_BOOL                      skip;
    CPSS_MAC_ENTRY_EXT_STC       macEntry;
    GT_BYTE_ARRY                 sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];
    skip = (GT_BOOL)inFields[1];

    macEntry.key.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case 0:
        galtisMacAddr(&macEntry.key.key.macVlan.macAddr,
                                       (GT_U8*)inFields[3]);
        macEntry.key.key.macVlan.vlanId = (GT_U16)inFields[4];
        break;

    case 1:
    case 2:
        macEntry.key.key.ipMcast.vlanId = (GT_U16)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            macEntry.key.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            macEntry.key.key.ipMcast.dip, dipBArr.data, 4);
        break;

    default:
         /* illegal key type */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    macEntry.dstInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[7];

    switch(inFields[7])
    {
    case 0:
        macEntry.dstInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[8];
        macEntry.dstInterface.devPort.portNum = (GT_PORT_NUM)inFields[9];
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.devPort.hwDevNum ,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case 1:
        macEntry.dstInterface.trunkId = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);

        macEntry.dstInterface.hwDevNum = (GT_U8)inFields[8];
        macEntry.dstInterface.devPort.portNum = 0;/* Need only to convert device number */
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.hwDevNum ,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case 2:
        macEntry.dstInterface.vidx = (GT_U16)inFields[11];
        break;

    case 3:
        macEntry.dstInterface.vlanId = (GT_U16)inFields[12];
        break;

    default:
        break;
    }

    macEntry.isStatic = (GT_BOOL)inFields[13];
    macEntry.daCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[14];
    macEntry.saCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[15];
    macEntry.daRoute = (GT_BOOL)inFields[16];
    macEntry.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[17];
    macEntry.sourceID = (GT_U32)inFields[18];
    macEntry.userDefined = (GT_U32)inFields[19];
    macEntry.daQosIndex = (GT_U32)inFields[20];
    macEntry.saQosIndex = (GT_U32)inFields[21];
    macEntry.daSecurityLevel = (GT_U32)inFields[22];
    macEntry.saSecurityLevel = (GT_U32)inFields[23];
    macEntry.appSpecificCpuCode = (GT_BOOL)inFields[24];

    if (setAgeToDefault == GT_FALSE)
    {
        macEntry.age = (GT_BOOL)inFields[25];
    }
    else
    {
        macEntry.age = GT_TRUE;
    }

    macEntry.spUnknown = GT_FALSE;

    /* support Bobcat2 and Lion3 devices FDB Based mirroring */
    macEntry.daMirrorToRxAnalyzerPortEn =
    macEntry.saMirrorToRxAnalyzerPortEn = macEntry.mirrorToRxAnalyzerPortEn;

    if (numFields >= 31)
    {
        macEntry.saMirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[28];
        macEntry.daMirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[29];
        macEntry.key.vid1                   = (GT_BOOL)inFields[30];
    }

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntryWrite(devNum, index, skip, &macEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware MAC address table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum          - physical device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend and cpssDxChBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntrySetByMacAddr

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    CPSS_MAC_ENTRY_EXT_STC       macEntry;
    GT_BYTE_ARRY                 sipBArr, dipBArr;
    GT_U32                       cmdVersion;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init ALL fields , so there are no fields that are not covered by the wrapper ! */
    cpssOsMemSet(&macEntry,0,sizeof(macEntry));


    cmdVersion = 0;
    if (numFields > 27)
    {
        /* version 1 - spUnknown field added */
        cmdVersion ++;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    macEntry.key.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inFields[2];

    switch(inFields[2])
    {
    case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
        galtisMacAddr(&macEntry.key.key.macVlan.macAddr,
                                       (GT_U8*)inFields[3]);
        macEntry.key.key.macVlan.vlanId = (GT_U16)inFields[4];
        break;

    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
        
        macEntry.key.key.ipMcast.vlanId = (GT_U16)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            macEntry.key.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            macEntry.key.key.ipMcast.dip, dipBArr.data, 4);
        break;

    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
    default:
        /* illegal key type */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_AGENT_ERROR;

    }

    macEntry.dstInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[7];

    switch(inFields[7])
    {
    case 0:
        macEntry.dstInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[8];
        macEntry.dstInterface.devPort.portNum = (GT_PORT_NUM)inFields[9];
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.devPort.hwDevNum,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case 1:
        macEntry.dstInterface.trunkId = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);

        macEntry.dstInterface.hwDevNum = (GT_U8)inFields[8];
        macEntry.dstInterface.devPort.portNum = 0;/* Need only to convert device number */
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.hwDevNum ,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case 2:
        macEntry.dstInterface.vidx = (GT_U16)inFields[11];
        break;

    case 3:
        macEntry.dstInterface.vlanId = (GT_U16)inFields[12];
        break;

    default:
        break;
    }

    macEntry.isStatic = (GT_BOOL)inFields[13];
    macEntry.daCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[14];
    macEntry.saCommand = (CPSS_MAC_TABLE_CMD_ENT)inFields[15];
    macEntry.daRoute = (GT_BOOL)inFields[16];
    macEntry.mirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[17];
    macEntry.sourceID = (GT_U32)inFields[18];
    macEntry.userDefined = (GT_U32)inFields[19];
    macEntry.daQosIndex = (GT_U32)inFields[20];
    macEntry.saQosIndex = (GT_U32)inFields[21];
    macEntry.daSecurityLevel = (GT_U32)inFields[22];
    macEntry.saSecurityLevel = (GT_U32)inFields[23];
    macEntry.appSpecificCpuCode = (GT_BOOL)inFields[24];

    if (setAgeToDefault == GT_FALSE)
    {
        macEntry.age = (GT_BOOL)inFields[25];
    }
    else
    {
        macEntry.age = GT_TRUE;
    }


    macEntry.spUnknown = GT_FALSE;
    if (cmdVersion > 0)
    {
        macEntry.spUnknown = (GT_BOOL)inFields[26];
    }

    /* support Bobcat2 and Lion3 devices FDB Based mirroring */
    macEntry.daMirrorToRxAnalyzerPortEn =
    macEntry.saMirrorToRxAnalyzerPortEn = macEntry.mirrorToRxAnalyzerPortEn;

    if (numFields >= 31)
    {
        macEntry.saMirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[28];
        macEntry.daMirrorToRxAnalyzerPortEn = (GT_BOOL)inFields[29];
        macEntry.key.vid1                   = (GT_BOOL)inFields[30];
    }

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntrySet(devNum, &macEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*   The function calls wrCpssDxChBrgFdbMacEntrysetByIndex or
*   wrCpssDxChBrgFdbMacEntrysetByMacAddr according to user's
*   preference as set in inArgs[1].
*
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* the table resides only in this comment  */
    /* don't remove it                         */

    /* switch by byIndex boolean in inArgs[1]  */

    switch (inArgs[1])
    {
        case 0:
            return wrCpssDxChBrgFdbMacEntrySetByMacAddr(
                inArgs, inFields ,numFields ,outArgs);
        case 1:
            return wrCpssDxChBrgFdbMacEntrySetByIndex(
                inArgs, inFields ,numFields ,outArgs);

        default:
            break;
    }

    return CMD_AGENT_ERROR;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with
*                     the entry (even for vidx/trunk entries the field is used
*                     by PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Adderss entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS internalDxChBrgFdbMacEntryReadNext
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_BOOL                         showSkip;
    GT_BOOL                         valid;
    GT_BOOL                         skip;
    GT_BOOL                         aged;
    GT_HW_DEV_NUM                   associatedHwDevNum;

    CPSS_MAC_ENTRY_EXT_STC          entry;
    GT_U32                          tempSip, tempDip;
    GT_PORT_NUM                     dummyPort = 0;/* dummy port num for conversion */
    GT_U32      portGroupId;/* port group Id iterator */


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* by default : 'ignore skip' (the field of inArgs[1] == GT_FALSE --> ignore skip) */
    showSkip = (GT_BOOL)inArgs[1]; /* use the byIndex field */

    do{
         /* call cpss api function */
            result = wrPrvCpssDxChBrgFdbMacEntryRead(devNum, indexCnt, &valid, &skip,
                                            &aged, &associatedHwDevNum, &entry);

         if (result != GT_OK)
         {
             /* check end of table return code */
             if(GT_OUT_OF_RANGE == result)
             {
                 /* the result is ok, this is end of table */
                 result = GT_OK;
             }

             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
         }

         if((entry.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)||
            (entry.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)||
            (entry.key.entryType==CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E))
         {
             /* legacy tables - do not support FDB routing entries */
             indexCnt++;
             continue;
         }

         if(useRefreshFdbPerPortGroupId == GT_TRUE)
         {
            /* find first port group in the bmp */

            /* access to read from next port group */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)
            {
                if(portGroupId <= refreshFdbCurrentPortGroupId)
                {
                    /* skip port groups we already been to */
                    continue;
                }

                refreshFdbCurrentPortGroupId = portGroupId;
                break;
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)

            if(portGroupId > refreshFdbCurrentPortGroupId)
            {
                /* indication that we finished loop on all port groups */
                indexCnt++;

                /* state that the next read come from first port group again */
                refreshFdbCurrentPortGroupId = refreshFdbFirstPortGroupId;
            }
         }
         else
         {
            indexCnt++;
         }
    }while (!valid || (!showSkip && skip));


    /* result is always GT_OK
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    } */

    /* support Bobcat2 and Lion3 FDB mirroring */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        entry.mirrorToRxAnalyzerPortEn = entry.daMirrorToRxAnalyzerPortEn;
    }

    if(useRefreshFdbPerPortGroupId == GT_TRUE &&
       refreshFdbCurrentPortGroupId != refreshFdbFirstPortGroupId)
    {
        /* index was not incremented */
        inFields[0] = indexCnt;
    }
    else
    {
        inFields[0] = indexCnt - 1;
    }
    inFields[1] = skip;
    inFields[2] = entry.key.entryType;

    switch(entry.key.entryType)
    {
    case 0:
    case 6:
        inFields[4] = entry.key.key.macVlan.vlanId;
        break;

    case 1:
    case 2:
    case 7:
    case 8:
        inFields[4] = entry.key.key.ipMcast.vlanId;
        break;

    default:
        /* illegal key type */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    cmdOsMemCpy(&tempSip, &entry.key.key.ipMcast.sip[0], 4);
    tempSip = cmdOsHtonl(tempSip);
    inFields[5] = tempSip;

    cmdOsMemCpy(&tempDip, &entry.key.key.ipMcast.dip[0], 4);
    tempDip = cmdOsHtonl(tempDip);
    inFields[6] = tempDip;

    inFields[7] = entry.dstInterface.type;
    if (entry.dstInterface.type == CPSS_INTERFACE_PORT_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(entry.dstInterface.devPort.hwDevNum,
                             entry.dstInterface.devPort.portNum);
    }
    if (entry.dstInterface.type == CPSS_INTERFACE_TRUNK_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(entry.dstInterface.hwDevNum,
                             entry.dstInterface.devPort.portNum);
                entry.dstInterface.devPort.hwDevNum = entry.dstInterface.hwDevNum;
    }

    inFields[8] = entry.dstInterface.devPort.hwDevNum;
    inFields[9] = entry.dstInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(entry.dstInterface.trunkId);
    inFields[10] = entry.dstInterface.trunkId;
    inFields[11] = entry.dstInterface.vidx;
    inFields[12] = entry.dstInterface.vlanId;

    inFields[13] = entry.isStatic;
    inFields[14] = entry.daCommand;
    inFields[15] = entry.saCommand;
    inFields[16] = entry.daRoute;
    inFields[17] = entry.mirrorToRxAnalyzerPortEn;
    inFields[18] = entry.sourceID;
    inFields[19] = entry.userDefined;
    inFields[20] = entry.daQosIndex;
    inFields[21] = entry.saQosIndex;
    inFields[22] = entry.daSecurityLevel;
    inFields[23] = entry.saSecurityLevel;
    inFields[24] = entry.appSpecificCpuCode;
    inFields[25] = aged;

    CONVERT_BACK_DEV_PORT_DATA_MAC(associatedHwDevNum,
                                   dummyPort);
    if (cmdVersion == 0)
    {
        inFields[26] = associatedHwDevNum;
    }
    else
    {
        inFields[26] = entry.spUnknown;
        inFields[27] = associatedHwDevNum;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                          inFields[0],  inFields[1],  inFields[2],
                          entry.key.key.macVlan.macAddr.arEther,
                          inFields[4],  inFields[5],  inFields[6],
                          inFields[7],  inFields[8],  inFields[9],
                          inFields[10], inFields[11], inFields[12],
                          inFields[13], inFields[14], inFields[15],
                          inFields[16], inFields[17], inFields[18],
                          inFields[19], inFields[20], inFields[21],
                          inFields[22], inFields[23], inFields[24],
                                        inFields[25], inFields[26]);
    if (cmdVersion > 0)
    {
        fieldOutputSetAppendMode();
        fieldOutput("%d", inFields[27]);
    }

    if (cmdVersion > 1)
    {
        fieldOutputSetAppendMode();
        fieldOutput(
            "%d%d%d",
            entry.saMirrorToRxAnalyzerPortEn,
            entry.daMirrorToRxAnalyzerPortEn,
            entry.key.vid1);
    }

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadFirst
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    GT_U8                           devNum;
    GT_U32      portGroupId;/* port group Id iterator */

    indexCnt = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) &&
       PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode >
        PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E))
    {
        if(multiPortGroupsBmpEnable == GT_FALSE)
        {
            multiPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }

        /* get first port group to access */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)
        {
            refreshFdbFirstPortGroupId = portGroupId;
            break;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)

        refreshFdbCurrentPortGroupId = refreshFdbFirstPortGroupId;
        useRefreshFdbPerPortGroupId = GT_TRUE;
    }
    else
    {
        useRefreshFdbPerPortGroupId = GT_FALSE;
    }


    return internalDxChBrgFdbMacEntryReadNext(
        inArgs,inFields,numFields,outArgs,cmdVersion);
}
/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadNext
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    return internalDxChBrgFdbMacEntryReadNext(
        inArgs,inFields,numFields,outArgs,cmdVersion);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadV0First
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbMacEntryReadFirst(
        inArgs,inFields,numFields,outArgs, 0 /*cmdVersion*/);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadV0Next
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbMacEntryReadNext(
        inArgs,inFields,numFields,outArgs, 0 /*cmdVersion*/);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadV1First
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbMacEntryReadFirst(
        inArgs,inFields,numFields,outArgs, 1 /*cmdVersion*/);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadV1Next
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbMacEntryReadNext(
        inArgs,inFields,numFields,outArgs, 1 /*cmdVersion*/);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadV2First
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbMacEntryReadFirst(
        inArgs,inFields,numFields,outArgs, 2 /*cmdVersion*/);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryReadV2Next
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbMacEntryReadNext(
        inArgs,inFields,numFields,outArgs, 2 /*cmdVersion*/);
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*       Delete an old entry in Hardware MAC address table through Address Update
*       message.(AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format with skip bit set to 1.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum          - physical device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or vlanId
*       GT_TIMEOUT - timeout of 30 seconds waiting for Signal of HW operation
*                    ended
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend and cpssDxChBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryDeleteByMacAddr

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_MAC_ENTRY_EXT_KEY_STC          macEntryKey;
    GT_BYTE_ARRY                        sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    macEntryKey.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inFields[2];
    macEntryKey.vid1 = (GT_U16)inFields[30];
    switch(inFields[2])
    {
    case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        galtisMacAddr(&macEntryKey.key.macVlan.macAddr, (GT_U8*)inFields[3]);
        macEntryKey.key.macVlan.vlanId = (GT_U16)inFields[4];
        break;
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        macEntryKey.key.ipMcast.vlanId = (GT_U16)inFields[4];

        galtisBArray(&sipBArr, (GT_U8*)inFields[5]);

        cmdOsMemCpy(
            macEntryKey.key.ipMcast.sip, sipBArr.data, 4);

        galtisBArray(&dipBArr, (GT_U8*)inFields[6]);

        cmdOsMemCpy(
            macEntryKey.key.ipMcast.dip, dipBArr.data, 4);
        break;

    case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
        galtisBArray(&dipBArr, (GT_U8*)inFields[3]);

        cmdOsMemCpy(
            macEntryKey.key.ipv4Unicast.dip, dipBArr.data, 4);

        macEntryKey.key.ipv4Unicast.vrfId = (GT_U16)inFields[5];
        break;
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
        galtisBArray(&dipBArr, (GT_U8*)inFields[4]);

        cmdOsMemCpy(
            macEntryKey.key.ipv6Unicast.dip, dipBArr.data, 16);

        macEntryKey.key.ipv6Unicast.vrfId = (GT_U16)inFields[5];
        break;
    case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
        /* no key for Data format - supported ONLY delete by index */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong FDB ENTRY FORMAT.\n");
        return CMD_AGENT_ERROR;

    default:
        break;
    }


    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntryDelete(devNum, &macEntryKey);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacEntryInvalidate
*
* DESCRIPTION:
*       Invalidate an entry in Hardware MAC address table in specified index.
*       the invalidation done by resetting to first word of the entry
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*       index       - hw mac entry index.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryDeleteByIndex

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U32                    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntryInvalidate(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgFdbMacEntryDelete
*
* DESCRIPTION:
*   The function calls wrCpssDxChBrgFdbMacEntryDeleteByIndex or
*   wrCpssDxChBrgFdbMacEntryDeleteByMacAddr according to user's
*   preference as set in inArgs[1].
*
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* the table resides only in this comment  */
    /* don't remove it                         */

    /* switch by byIndex boolean in inArgs[1]  */

    switch (inArgs[1])
    {
        case 0:
            return wrCpssDxChBrgFdbMacEntryDeleteByMacAddr(
                    inArgs, inFields ,numFields ,outArgs);
        case 1:
            return wrCpssDxChBrgFdbMacEntryDeleteByIndex(
                    inArgs, inFields ,numFields ,outArgs);

        default:
            break;
    }

    return CMD_AGENT_ERROR;
}


/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenSet
*
* DESCRIPTION:
*       Set the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*       lookupLen   - The maximal length of MAC table lookup, this must be
*                     value divided by 4 with no left over.
*                     in range 4..32. (4,8,12..32)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_OUT_OF_RANGE - lookupLen > 32 or lookupLen < 4
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 4
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMaxLookupLenSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       dev;
    GT_U32      lookupLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    lookupLen = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbMaxLookupLenSet(dev, lookupLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMaxLookupLenGet
*
* DESCRIPTION:
*       Get the the number of entries to be looked up in the MAC table lookup
*       (the hash chain length), for all devices in unit.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       lookupLenPtr   - The maximal length of MAC table lookup, this must be
*                        value divided by 4 with no left over.
*                        in range 4..32. (4,8,12..32)
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - If the given lookupLen is too large, or not divided
*                         by 4 with no left over.
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       The Maximal Hash chain length in HW calculated as follows:
*       Maximal Hash chain length = (regVal + 1) * 4
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMaxLookupLenGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       dev;
    GT_U32      lookupLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbMaxLookupLenGet(dev, &lookupLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", lookupLen);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeSet
*
* DESCRIPTION:
*       Sets the VLAN Lookup mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       mode    - lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad param
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacVlanLookupModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS        result;

    GT_U8            dev;
    CPSS_MAC_VL_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_VL_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbMacVlanLookupModeSet(dev, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacVlanLookupModeGet
*
* DESCRIPTION:
*       Get VLAN Lookup mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       modePtr - pointer to lookup mode:
*                 CPSS_IVL_E - Both MAC and Vlan are used for MAC table lookup
*                              and learning.
*                 CPSS_SVL_E - Only MAC is used for MAC table lookup and learning.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad param
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacVlanLookupModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS         result;

    GT_U8             devNum;
    CPSS_MAC_VL_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbMacVlanLookupModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitSet
*
* DESCRIPTION:
*     Set Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum     - device number
*       msgRate    - maximal rate of AU messages per second
*       enable     - enable/disable message limit
*                    GT_TRUE - message rate is limitted with msgRate
*                    GT_FALSE - there is no limit, WA to CPU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_HW_ERROR        - on error.
*       GT_OUT_OF_RANGE    - out of range.
*       GT_BAD_PARAM       - on bad parameter
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when PP Clock 200 Mhz,
*          granularity and rate changed lineary depending on clock
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgRateLimitSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS         result;

    GT_U8             devNum;
    GT_U32            msgRate;
    GT_BOOL           enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    msgRate = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgFdbAuMsgRateLimitSet(devNum, msgRate, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAuMsgRateLimitGet
*
* DESCRIPTION:
*     Get Address Update CPU messages rate limit
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       msgRatePtr - (pointer to) maximal rate of AU messages per second
*       enablePtr  - (pointer to) GT_TRUE - message rate is limited with
*                                           msgRatePtr
*                                 GT_FALSE - there is no limit, WA to CPU
*
* RETURNS:
*       GT_OK              - on success.
*       GT_HW_ERROR        - on error.
*       GT_OUT_OF_RANGE    - out of range.
*       GT_BAD_PARAM       - on bad parameter
*       GT_BAD_PTR         - one of the parameters is NULL pointer
*
* COMMENTS:
*          granularity - 200 messages,
*          rate limit - 0 - 51000 messages,
*          when PP Clock 200 Mhz,
*          granularity and rate changed lineary depending on clock
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgRateLimitGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS         result;

    GT_U8             devNum;
    GT_U32            msgRate;
    GT_BOOL           enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbAuMsgRateLimitGet(devNum, &msgRate, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", msgRate, enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongSet
*
* DESCRIPTION:
*       Enable/Disable sending NA messages to the CPU indicating that the device
*       cannot learn a new SA . It has reached its max hop (bucket is full).
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on other error.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaMsgOnChainTooLongSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS         result;

    GT_U8             devNum;
    GT_BOOL           enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbNaMsgOnChainTooLongSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbNaMsgOnChainTooLongGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending NA messages to the CPU
*       indicating that the device cannot learn a new SA. It has reached its
*       max hop (bucket is full).
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the NA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on other error.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaMsgOnChainTooLongGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS         result;

    GT_U8             devNum;
    GT_BOOL           enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbNaMsgOnChainTooLongGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuSet
*
* DESCRIPTION:
*       Enable/Disable the PP to/from sending an AA and TA address
*       update messages to the CPU.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*       auMsgType   - type of message to enable / disable.
*       enable      - enable or disable the message
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAAandTAToCpuSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbAAandTAToCpuSet(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAAandTAToCpuGet
*
* DESCRIPTION:
*       Get state of sending an AA and TA address update messages to the CPU
*       as configured to PP.
*
* APPLICABLE DEVICES:  All DXCH devices
*
*
* INPUTS:
*       devNum      - physical device number
*       auMsgType   - type of message to get it's state.
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - the messages to CPU enabled
*                                  GT_FALSE - the messages to CPU disabled
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or auMsgType
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAAandTAToCpuGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8            dev;
    GT_BOOL          enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbAAandTAToCpuGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbHashModeSet
*
* DESCRIPTION:
*       Sets the FDB hash function mode.
*       The CRC based hash function provides the best hash index destribution
*       for random addresses and vlans.
*       The XOR based hash function provides optimal hash index destribution
*       for controlled testing scenarios, where sequential addresses and vlans
*       are often used.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       mode    - hash function based mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbHashModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_MAC_HASH_FUNC_MODE_ENT         mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_HASH_FUNC_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbHashModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbHashModeGet
*
* DESCRIPTION:
*       Gets the FDB hash function mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       modePtr - pointer to hash function based mode:
*                 CPSS_FDB_HASH_FUNC_XOR_E - XOR based hash function mode
*                 CPSS_FDB_HASH_FUNC_CRC_E - CRC based hash function mode
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbHashModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_MAC_HASH_FUNC_MODE_ENT         mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbHashModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}


/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutSet
*
* DESCRIPTION:
*       Sets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE : The device support aging time with granularity of 10 seconds
*       (10,20..630) . So value of 234 will be round down to 230 , and value of
*       255 will be round up to 260.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       timeout - aging time in seconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       For core clock of 200 MHZ The value ranges from 10 seconds to 630
*       seconds in steps of 10 seconds.
*       For core clock of 144MHZ the aging timeout ranges from 14 (13.8) to 875
*       seconds in steps of 14 seconds.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgingTimeoutSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_U32         timeout;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    timeout = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgingTimeoutSet(devNum, timeout);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/******************************************************************************
* cpssDxChBrgFdbAgingTimeoutGet
*
* DESCRIPTION:
*       Gets the timeout period in seconds for aging out dynamically learned
*       forwarding information. The standard recommends 300 sec.
*
*       NOTE : The device support aging time with granularity of 10 seconds
*       (10,20..630) .
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       timeoutPtr - pointer to aging time in seconds.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       For core clock of 200 MHZ The value ranges from 10 seconds to 630
*       seconds in steps of 10 seconds.
*       For core clock of 144MHZ the aging timeout ranges from 14 (13.8) to 875
*       seconds in steps of 14 seconds.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgingTimeoutGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_U32         timeout;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgingTimeoutGet(devNum, &timeout);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", timeout);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbSecureAgingSet
*
* DESCRIPTION:
*      Enable or disable secure aging. This mode is relevant to automatic or
*      triggered aging with removal of aged out FDB entries. This mode is
*      applicable together with secure auto learning mode see
*      cpssDxChBrgFdbSecureAutoLearnSet. In the secure aging mode the device
*      sets aged out unicast FDB entry with <Multiple> =  1 and VIDX = 0xfff but
*      not invalidates entry. This causes packets destined to this FDB entry MAC
*      address to be flooded to the VLAN.  But such packets are known ones.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable secure aging
*                 GT_FALSE - disable secure aging
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM       - on wrong input parameters
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecureAgingSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbSecureAgingSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbSecureAgingGet
*
* DESCRIPTION:
*      Get the status (enabled/disabled) of secure aging.
*      This mode is relevant to automatic or triggered aging with removal of
*      aged out FDB entries. This mode is applicable together with secure auto
*      learning mode see cpssDxChBrgFdbSecureAutoLearnGet. In the secure aging
*      mode  the device sets aged out unicast FDB entry with <Multiple> =  1 and
*      VIDX = 0xfff but not invalidates entry. This causes packets destined to
*      this FDB entry MAC address to be flooded to the VLAN.
*      But such packets are known ones.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - enable secure aging
*                    GT_FALSE - disable secure aging
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecureAgingGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbSecureAgingGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnSet
*
* DESCRIPTION:
*        Configure secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum  - device number
*       mode    - Enable/Disable secure automatic learning.
*                 The command assigned to packet with new SA not found in FDB
*                 when secure automatic learning enable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM       - on wrong input parameters
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecureAutoLearnSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                  result;

    GT_U8                                      devNum;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbSecureAutoLearnSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbSecureAutoLearnGet
*
* DESCRIPTION:
*        Get secure auto learning mode.
*        If Secure Automatic Learning is enabled, and a packet enabled for
*        automatic learning, then:
*        - If the source MAC address is found in the FDB, and the associated
*          location of the address has changed, then the FDB entry is updated
*          with the new location of the address.
*        - If the FDB entry is modified with the new location of the address,
*          a New Address Update message is sent to the CPU,
*          if enabled to do so on the port and the packet VLAN.
*        - If the source MAC address is NOT found in the FDB, then
*          Secure Automatic Learning Unknown Source command (Trap to CPU,
*          Soft Drop or Hard Drop)  is assigned to the packet according to mode
*          input parameter and packet treated as security breach event.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr  - Enabled/Disabled secure automatic learning.
*                  The command assigned to packet with new SA not found in FDB
*                  when secure automatic learning enabled.
*
* RETURNS:
*       GT_OK               - on success
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PARAM        - on wrong input parameters
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecureAutoLearnGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                  result;

    GT_U8                                      devNum;
    CPSS_MAC_SECURE_AUTO_LEARN_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbSecureAutoLearnGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnable
*
* DESCRIPTION:
*       This routine determines whether the transplanting operate on static
*       entries.
*       When the PP will do the transplanting , it will/won't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it will/won't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum - physical device number
*       enable - GT_TRUE transplanting is enabled on static entries,
*                GT_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbStaticTransEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbStaticTransEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbStaticTransEnableGet
*
* DESCRIPTION:
*       Get whether the transplanting enabled to operate on static entries.
*       when the PP do the transplanting , it do/don't transplant
*       FDB static entries (entries that are not subject to aging).
*
*       When the PP will do the "port flush" (delete FDB entries associated
*       with to specific port), it do/don't Flush the FDB static entries
*       (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE transplanting is enabled on static entries,
*                   GT_FALSE otherwise.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbStaticTransEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbStaticTransEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnable
*
* DESCRIPTION:
*       This routine determines whether flush delete operates on static entries.
*       When the PP will do the Flush , it will/won't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - device number
*       mode    - FDB deletion mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbStaticDelEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;
    CPSS_DXCH_BRG_FDB_DEL_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_BRG_FDB_DEL_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbStaticDelEnable(dev, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbStaticDelEnableGet
*
* DESCRIPTION:
*       Get whether flush delete operates on static entries.
*       When the PP do the Flush , it do/don't Flush
*       FDB static entries (entries that are not subject to aging).
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*        mode    - FDB deletion mode.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbStaticDelEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    CPSS_DXCH_BRG_FDB_DEL_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbStaticDelEnableGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionsEnableSet
*
* DESCRIPTION:
*       Enables/Disables FDB actions.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE -  Actions are enabled
*                 GT_FALSE -  Actions are disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong input parameters
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionsEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionsEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionsEnableGet
*
* DESCRIPTION:
*       Get the status of FDB actions.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum     - physical device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - actions are enabled
*                    GT_FALSE - actions are disabled
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionsEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionsEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbTrigActionStatusGet
*
* DESCRIPTION:
*       Get the action status of the FDB action registers.
*       When the status is changing from GT_TRUE to GT_FALSE, then the device
*       performs the action according to the setting of action registers.
*       When the status returns to be GT_TRUE then the action is completed.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       actFinishedPtr  - GT_TRUE  - triggered action completed
*                         GT_FALSE - triggered action is not completed yet
*
* RETURNS:
*       GT_OK            - on success
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM       - on wrong input parameters
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbTrigActionStatusGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        actFinished;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbTrigActionStatusGet(devNum, &actFinished);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", actFinished);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeSet
*
* DESCRIPTION:
*       Sets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       mode    - action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on unsupported parameters
*       GT_BAD_PARAM     - on wrong mode
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacTriggerModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_MAC_ACTION_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_MAC_ACTION_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbMacTriggerModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacTriggerModeGet
*
* DESCRIPTION:
*       Gets Mac address table Triggered\Automatic action mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       modePtr - pointer to action mode:
*                 CPSS_ACT_AUTO_E - Action is done Automatically.
*                 CPSS_ACT_TRIG_E - Action is done via a trigger from CPU.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on unsupported parameters
*       GT_BAD_PARAM     - on wrong mode
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacTriggerModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_MAC_ACTION_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbMacTriggerModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemove
*
* DESCRIPTION:
*       enable the ability to delete static mac entries when the aging daemon in
*       PP encounter with entry registered on non-exist device in the
*       "device table" the PP removes it ---> this flag regard the static
*       entries
*
* APPLICABLE DEVICES:  All DXCH devices
* INPUTS:
*       devNum       - device number
*       deleteStatic -  GT_TRUE - Action will delete static entries as well as
*                                 dynamic
*                       GT_FALSE - only dynamic entries will be deleted.
*
* OUTPUTS:
*       NONE
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*
* COMMENTS:
*       It's application responsibility to check the status of Aging Trigger by
*       busy wait (use cpssDxChBrgFdbTrigActionStatusGet API), in order to be
*       sure that that there is currently no action done.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbStaticOfNonExistDevRemove

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  deleteStatic;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    deleteStatic = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbStaticOfNonExistDevRemove(devNum, deleteStatic);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbStaticOfNonExistDevRemoveGet
*
* DESCRIPTION:
*       Get whether enabled/disabled the ability to delete static mac entries
*       when the aging daemon in PP encounter with entry registered on non-exist
*       device in the "device table".
*       entries
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       deleteStaticPtr -  GT_TRUE - Action deletes entries if there associated
*                                    device number don't exist (static etries
*                                                                  as well)
*                          GT_FALSE - Action don't delete entries if there
*                                     associated device number don't exist
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on failure
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on unsupported parameters
*       GT_BAD_PARAM     - on wrong mode
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbStaticOfNonExistDevRemoveGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        deleteStatic;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbStaticOfNonExistDevRemoveGet(devNum,
                                              &deleteStatic);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", deleteStatic);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableSet
*
* DESCRIPTION:
*       Enable/Disable dropping the Address Update messages when the queue is
*       full .
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE - Drop the address update messages
*                 GT_FALSE - Do not drop the address update messages
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbDropAuEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbDropAuEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbDropAuEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of dropping the Address Update messages.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE - The address update messages are dropped
*                    GT_FALSE - The address update messages are not dropped
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbDropAuEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbDropAuEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with trunk,
*       regardless of the device to wich they are associated.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum                      - device number
*       ageOutAllDevOnTrunkEnable   - GT_TRUE : All entries associated with a
*                                     trunk, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     trunk on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgeOutAllDevOnTrunkEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  ageOutAllDevOnTrunkEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ageOutAllDevOnTrunkEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable(devNum,
                                  ageOutAllDevOnTrunkEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with trunk, regardless of
*       the device to wich they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                      trunk, regardless of the device they
*                                      are associated with are aged out.
*                                      GT_FALSE: Only entries associated with a
*                                      trunk on the local device are aged out.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        ageOutAllDevOnTrunkEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet(devNum,
                                 &ageOutAllDevOnTrunkEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ageOutAllDevOnTrunkEnable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable
*
* DESCRIPTION:
*       Enable/Disable aging out of all entries associated with port,
*       regardless of the device to wich they are associated.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum                       - device number
*       ageOutAllDevOnNonTrunkEnable - GT_TRUE : All entries associated with a
*                                     port, regardless of the device they
*                                     are associated with are aged out.
*                                     GT_FALSE: Only entries associated with a
*                                     port on the local device are aged out.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  ageOutAllDevOnNonTrunkEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ageOutAllDevOnNonTrunkEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable(devNum,
                                  ageOutAllDevOnNonTrunkEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet
*
* DESCRIPTION:
*       Get if aging out of all entries associated with port, regardless of
*       the device to wich they are associated is enabled/disabled.
*
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum                         - device number
*
* OUTPUTS:
*       ageOutAllDevOnNonTrunkEnablePtr - GT_TRUE : All entries associated with a
*                                         port, regardless of the device they
*                                         are associated with are aged out.
*                                         GT_FALSE: Only entries associated with a
*                                         port on the local device are aged out.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        ageOutAllDevOnNonTrunkEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet(devNum,
                                 &ageOutAllDevOnNonTrunkEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ageOutAllDevOnNonTrunkEnable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataSet
*
* DESCRIPTION:
*       Prepares the entry for transplanting (old and new interface parameters).
*       VLAN and VLAN mask for transplanting is set by
*       cpssDxChBrgFdbActionActiveVlanSet. Execution of transplanting is done
*       by cpssDxChBrgFdbTrigActionStart.
*
* INPUTS:
*       devNum          - physical device number
*       oldInterfacePtr - pointer to old Interface parameters
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters
*                         (it may be only port or trunk).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionTransplantDataSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_INTERFACE_INFO_STC                 oldInterface;
    CPSS_INTERFACE_INFO_STC                 newInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* init with zeros */
    cmdOsMemSet(&oldInterface, 0, sizeof(oldInterface));
    cmdOsMemSet(&newInterface, 0, sizeof(newInterface));

    oldInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[0];

    switch(inFields[0])
    {
    case 0:
        oldInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[5];
        oldInterface.devPort.portNum = (GT_PORT_NUM)inFields[4];
        CONVERT_DEV_PORT_DATA_MAC(oldInterface.devPort.hwDevNum,
                             oldInterface.devPort.portNum);
        break;

    case 1:
        oldInterface.hwDevNum = (GT_U8)inFields[5];
        oldInterface.devPort.portNum = 0; /* Need only to convert device number */
        CONVERT_DEV_PORT_DATA_MAC(oldInterface.hwDevNum,
                             oldInterface.devPort.portNum);

        oldInterface.trunkId = (GT_TRUNK_ID)inFields[3];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oldInterface.trunkId);
        break;

    case 2:
        oldInterface.vidx = (GT_U16)inFields[2];
        break;

    case 3:
        oldInterface.vlanId = (GT_U16)inFields[1];
        break;

    default:
        break;
    }

    newInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[6];

    switch(inFields[6])
    {
    case 0:
        newInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[7];
        newInterface.devPort.portNum = (GT_PORT_NUM)inFields[8];
        CONVERT_DEV_PORT_DATA_MAC(newInterface.devPort.hwDevNum,
                             newInterface.devPort.portNum);
        break;

    case 1:
        newInterface.hwDevNum = (GT_U8)inFields[5];
        newInterface.devPort.portNum = 0; /* Need only to convert device number */
        CONVERT_DEV_PORT_DATA_MAC(newInterface.hwDevNum,
                             newInterface.devPort.portNum);

        newInterface.trunkId = (GT_TRUNK_ID)inFields[9];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(newInterface.trunkId);
        break;

    case 2:
        newInterface.vidx = (GT_U16)inFields[10];
        break;

    case 3:
        newInterface.vlanId = (GT_U16)inFields[11];
        break;

    default:
        break;
    }

    /* call cpss api function */
    result = cpssDxChBrgFdbActionTransplantDataSet(devNum, &oldInterface,
                                                           &newInterface);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionTransplantDataGet
*
* DESCRIPTION:
*       Get transplant data: old interface parameters and the new ones.
*
* INPUTS:
*       devNum          - physical device number
*
* OUTPUTS:
*       oldInterfacePtr - pointer to old Interface parameters.
*                         (it may be only port or trunk).
*       newInterfacePtr - pointer to new Interface parameters.
*                         (it may be only port or trunk).
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionTransplantDataGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                             result;

    GT_U8                                 devNum;
    CPSS_INTERFACE_INFO_STC               oldInterface;
    CPSS_INTERFACE_INFO_STC               newInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cmdOsMemSet((GT_U32*)&oldInterface, 0, sizeof(oldInterface));
    cmdOsMemSet((GT_U32*)&newInterface, 0, sizeof(newInterface));

    /* call cpss api function */
    result = cpssDxChBrgFdbActionTransplantDataGet(devNum, &oldInterface,
                                                           &newInterface);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = oldInterface.type;
    inFields[1] = oldInterface.vlanId;
    inFields[2] = oldInterface.vidx;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(oldInterface.trunkId);
    inFields[3] = oldInterface.trunkId;
    if (oldInterface.type == CPSS_INTERFACE_PORT_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(oldInterface.devPort.hwDevNum,
                                       oldInterface.devPort.portNum);
    }
    if (oldInterface.type == CPSS_INTERFACE_TRUNK_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(oldInterface.hwDevNum,
                             oldInterface.devPort.portNum);
                oldInterface.devPort.hwDevNum = oldInterface.hwDevNum;
    }

    inFields[4] = oldInterface.devPort.portNum;
    inFields[5] = oldInterface.devPort.hwDevNum;

    inFields[6] = newInterface.type;
    if (newInterface.type == CPSS_INTERFACE_PORT_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(newInterface.devPort.hwDevNum,
                                       newInterface.devPort.portNum);
    }
    if (newInterface.type == CPSS_INTERFACE_TRUNK_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(newInterface.hwDevNum,
                             newInterface.devPort.portNum);
                newInterface.devPort.hwDevNum = newInterface.hwDevNum;
    }

    inFields[7] = newInterface.devPort.hwDevNum;
    inFields[8] = newInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(newInterface.trunkId);
    inFields[9] = newInterface.trunkId;
    inFields[10] = newInterface.vidx;
    inFields[11] = newInterface.vlanId;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],
                  inFields[2], inFields[3], inFields[4],  inFields[5],
                  inFields[6], inFields[7], inFields[8],  inFields[9],
                                            inFields[10], inFields[11]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionTransplantDataGetNext

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
* cpssDxChBrgFdbFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       completedPtr - pointer to AU message processing completion.
*                      GT_TRUE - AU message was proceesed by PP.
*                      GT_FALSE - AU message proceesing is not completed yet by PP.
*       succeededPtr - pointer to a success of AU operation.
*                      GT_TRUE  - the AU action succeeded.
*                      GT_FALSE - the AU action has failed.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFromCpuAuMsgStatusGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        completed;
    GT_BOOL        succeeded;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbFromCpuAuMsgStatusGet(devNum, &completed,
                                                         &succeeded);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", completed, succeeded);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
*
* DESCRIPTION:
*       Get status of FDB Address Update (AU) message processing in the PP.
*       The function returns AU message processing completion and success status.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       completedPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU message processing completion.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation completed , otherwise not completed
*                      for multi-port groups device :
*                           when bit is set (1) - AU message was processed by PP (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU message processed is not
*                                            completed yet by PP. (in the corresponding
*                                            port group)
*       succeededPortGroupsBmpPtr - (pointer to) bitmap of port groups on which
*                      AU operation succeeded.
*                      for NON multi-port groups device :
*                           value CPSS_PORT_GROUP_UNAWARE_MODE_CNS means that
*                           operation succeeded , otherwise failed
*                      for multi-port groups device :
*                           when bit is set (1) - AU action succeeded (in the
*                                            corresponding port group).
*                           when bit is not set (0) - AU action has failed. (in the
*                                            corresponding port group)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad devNum or portGroupsBmp
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For DXCH devices the succeed status doesn't relevant for query
*       address messages (message type = CPSS_QA_E) -  FEr#93.
*       The succeed status is relevant only when completed.
*       An AU message sent by CPU can fail in the following cases:
*       1. The message type = CPSS_NA_E and the hash chain has reached it's
*          maximum length.
*       2. The message type = CPSS_QA_E or CPSS_AA_E and the FDB entry doesn't
*          exist.
*
*       for multi-port groups device :
*           1. Unified FDBs mode:
*              API returns GT_BAD_PARAM if portGroupsBmp != CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum,origDevNum;
    GT_PORT_GROUPS_BMP completedPortGroupsBmp;
    GT_PORT_GROUPS_BMP succeededPortGroupsBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);
    if(multiPortGroupsBmpEnable == GT_FALSE)
    {
        multiPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    /* call cpss api function */
    result = cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet(devNum, multiPortGroupsBmp , &completedPortGroupsBmp,
                                                         &succeededPortGroupsBmp);

    origDevNum = devNum;
    CONVERT_BACK_DEV_PORT_GROUPS_BMP_MAC(origDevNum,completedPortGroupsBmp);
    origDevNum = devNum;
    CONVERT_BACK_DEV_PORT_GROUPS_BMP_MAC(origDevNum,succeededPortGroupsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", completedPortGroupsBmp, succeededPortGroupsBmp);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanSet
*
* DESCRIPTION:
*       Set action active vlan and vlan mask.
*       All actions will be taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*       vlanId      - Vlan Id
*       vlanMask    - vlan mask filter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on unsupported parameters
*       GT_BAD_PARAM     - on wrong mode
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*       There is no check done of current status of action's register
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveVlanSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_U16         vlanId;
    GT_U16         vlanMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    vlanMask = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveVlanSet(devNum, vlanId, vlanMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionActiveVlanGet
*
* DESCRIPTION:
*       Get action active vlan and vlan mask.
*       All actions are taken on entries belonging to a certain VLAN
*       or a subset of VLANs.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       vlanIdPtr      - pointer to Vlan Id
*       vlanMaskPtr    - pointer to vlan mask filter
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on unsupported parameters
*       GT_BAD_PARAM     - on wrong mode
*
* COMMENTS:
*       Use mask 0xFFF and VLAN ID to set mode on a single VLAN
*       Use mask 0 and VLAN 0 to set mode for all VLANs
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveVlanGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_U16         vlanId;
    GT_U16         vlanMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveVlanGet(devNum, &vlanId,
                                                     &vlanMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", vlanId, vlanMask);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevSet
*
* DESCRIPTION:
*       Set the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - device number
*       actDev      - Action active device (0..31)
*       actDevMask  - Action active mask (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_OUT_OF_RANGE - actDev > 31 or actDevMask > 31
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveDevSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_U32         actDev;
    GT_U32         actDevMask;
    GT_U32         origActDev;/* original actDev*/
    GT_PORT_NUM    dummyPort = 0;/* dummy port num for conversion */
    GT_HW_DEV_NUM  dummyHwDev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    actDev = (GT_U32)inArgs[1];
    actDevMask = (GT_U32)inArgs[2];

    origActDev = actDev;
    dummyHwDev = (GT_HW_DEV_NUM)actDev;

    /* convert the actDev by using dummy Port number */
    CONVERT_DEV_PORT_DATA_MAC(dummyHwDev,dummyPort);

    actDev = dummyHwDev;

    if(origActDev == actDevMask)
    {
        /* need to update actDevMask too */
        actDevMask = actDev;
    }

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveDevSet(devNum, actDev, actDevMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionActiveDevGet
*
* DESCRIPTION:
*       Get the Active device number and active device number mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "associated devNum" masked by the "active device number mask" equal to
*       "Active device number"
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actDevPtr      - pointer to action active device (0..31)
*       actDevMaskPtr  - pointer to action active mask (0..31)
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_OUT_OF_RANGE - actDev > 31 or actDevMask > 31
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveDevGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_U32         actDev;
    GT_U32         actDevMask;
    GT_U32         origActDev;/* original actDev*/
    GT_PORT_NUM    dummyPort = 0;/* dummy port */
    GT_HW_DEV_NUM  dummyHwDev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveDevGet(devNum, &actDev,
                                                  &actDevMask);

    origActDev = actDev;
    dummyHwDev = (GT_HW_DEV_NUM)actDev;

    /* convert the actDev by using dummy Port number */
    CONVERT_DEV_PORT_DATA_MAC(dummyHwDev,dummyPort);

    actDev = (GT_U32)dummyHwDev;

    if(origActDev == actDevMask)
    {
        /* need to update actDevMask too */
        actDevMask = actDev;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", actDev, actDevMask);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceSet
*
* DESCRIPTION:
*       Set the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of : Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*       A FDB entry will be treated by the FDB action only if the following
*       cases are true:
*       1. The interface is trunk and "active trunk mask" is set and
*       "associated trunkId" masked by the "active trunk/port mask" equal to
*       "Active trunkId".
*       2. The interface is port and "associated portNumber" masked by the
*       "active trunk/port mask" equal to "Active portNumber".
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum           - physical device number
*       actIsTrunk       - determines if the interface is port or trunk
*       actIsTrunkMask   - action active trunk mask.
*       actTrunkPort     - action active interface (port/trunk)
*       actTrunkPortMask - action active port/trunk mask.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       To enable Trunk-ID filter set actIsTrunk and actIsTrunkMask to 0x1,
*       set actTrunkPort to trunkID and actTrunkPortMask to 0x7F. To disable
*       Trunk-ID filter set all those parameters to 0x0.
*       To enable Port/device filter set actIsTrunk to 0x0, actTrunkPort to
*       portNum, actTrunkPortMask to 0x7F, actDev to device number and
*       actDevMask to 0x1F (all ones) by cpssDxChBrgFdbActionActiveDevSet.
*       To disable port/device filter set all those parameters to 0x0.
*
*       There is no check done of current status of action's register
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveInterfaceSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_PORT_NUM    portNum;
    GT_U32         actIsTrunk;
    GT_U32         actIsTrunkMask;
    GT_U32         actTrunkPort;
    GT_U32         actTrunkPortMask;
    GT_TRUNK_ID    trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    actIsTrunk = (GT_U32)inArgs[1];
    actIsTrunkMask = (GT_U32)inArgs[2];
    actTrunkPort = (GT_U32)inArgs[3];
    actTrunkPortMask = (GT_U32)inArgs[4];

    if (actIsTrunkMask && actIsTrunk)
    {
        trunkId = (GT_TRUNK_ID)actTrunkPort;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        if (actTrunkPortMask == actTrunkPort)
        {
            /* when test set both value and mask to the same value
                       --> convert both */
            actTrunkPortMask = trunkId;

        }

        actTrunkPort = trunkId;
    }
    else if( 0 == actIsTrunk )
    {
        portNum = (GT_PORT_NUM)actTrunkPort;
        CONVERT_DEV_PORT_U32_MAC(devNum, portNum);
        actTrunkPort = portNum;
    }

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveInterfaceSet(devNum, actIsTrunk,
                        actIsTrunkMask, actTrunkPort, actTrunkPortMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionActiveInterfaceGet
*
* DESCRIPTION:
*       Get the Active trunkId/portNumber active trunk/port mask and
*       trunkID mask that control the FDB actions of: Triggered aging with
*       removal, Triggered aging without removal and Triggered address deleting.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum           - physical device number
*
* OUTPUTS:
*       actIsTrunkPtr       - determines if the interface is port or trunk
*       actIsTrunkMaskPtr   - action active trunk mask.
*       actTrunkPortPtr     - action active interface (port/trunk)
*       actTrunkPortMaskPtr - action active port/trunk mask.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveInterfaceGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_U32         actIsTrunk;
    GT_U32         actIsTrunkMask;
    GT_U32         actTrunkPort;
    GT_U32         actTrunkPortMask;
    GT_TRUNK_ID    trunkId;
    GT_HW_DEV_NUM  dummyHwDevNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveInterfaceGet(devNum, &actIsTrunk,
                &actIsTrunkMask, &actTrunkPort, &actTrunkPortMask);

    if (actIsTrunk && actIsTrunkMask)
    {
        trunkId = (GT_TRUNK_ID)actTrunkPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(trunkId);
        if (actTrunkPort == actTrunkPortMask)
        {
            /* when test set both value and mask to the same value
                       --> convert both */
            actTrunkPortMask = trunkId;
        }

        actTrunkPort = trunkId;
    }
    else if( 0 == actIsTrunk )
    {
        dummyHwDevNum = (GT_HW_DEV_NUM)devNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(dummyHwDevNum, actTrunkPort);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", actIsTrunk, actIsTrunkMask,
                                          actTrunkPort, actTrunkPortMask);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbUploadEnableSet
*
* DESCRIPTION:
*       Enable/Disable reading FDB entries via AU messages to the CPU.
*       The API only configures mode of triggered action.
*       To execute upload use the cpssDxChBrgFdbTrigActionStart.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE - enable FDB Upload
*                  GT_FALSE - disable FDB upload
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       There is no check done of current status of action's register
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbUploadEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbUploadEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbUploadEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable of uploading FDB entries via AU messages to the CPU.
*
* APPLICABLE DEVICES:  All DXCH2 devices
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - FDB Upload is enabled
*                     GT_FALSE - FDB upload is disabled
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbUploadEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;

    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbUploadEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbTrigActionStart
*
* DESCRIPTION:
*       Enable actions, sets action type, action mode to CPSS_ACT_TRIG_E and
*       starts triggered action by setting Aging Trigger.
*       This API may be used to start one of triggered actions: Aging, Deleting,
*       Transplanting and FDB Upload.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       mode    - action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on bad param
*       GT_HW_ERROR     - on hardware error
*       GT_TIMEOUT      - on timeout
*
* COMMENTS:
*       Deleting and Transplanting actions can be done only by CPU triggered
*       FDB action that activated by the function.
*       See below sequence of CPSS API calls for Deleting and
*       Transplanting actions.
*
*       Before making Delete or Transplant sequence the application need to
*       disable Actions to avoid wrong automatic aging.
*
*       It is Application responsibility to get and store all parameters
*       that are changed during those actions.
*       1. AA message to CPU status by cpssDxChBrgFdbAAandTAToCpuGet.
*       2. Action Active configuration by
*          cpssDxChBrgFdbActionActiveInterfaceGet,
*          cpssDxChBrgFdbActionActiveDevGet,
*          cpssDxChBrgFdbActionActiveVlanGet.
*       3. Action mode and triger mode by cpssDxChBrgFdbActionModeGet
*          cpssDxChBrgFdbActionTriggerModeGet.
*
*       The AA and TA messages may be disabled before the FDB action
*       4. Disable AA and TA messages to CPU by cpssDxChBrgFdbAAandTAToCpuSet.
*
*       5. Set Active configuration by: cpssDxChBrgFdbActionActiveInterfaceSet,
*       cpssDxChBrgFdbActionActiveVlanSet and cpssDxChBrgFdbActionActiveDevSet.
*
*       6. Start trigered action by cpssDxChBrgFdbTrigActionStart
*
*       7. Wait that triggered action is completed by:
*           -  Busy-wait poling of stastus - cpssDxChBrgFdbTrigActionStatusGet
*           -  Wait of event CPSS_PP_MAC_AGE_VIA_TRIGGER_ENDED_E.
*              This wait may be done in context of dedicated task to restore
*              Active configuration and AA messages configuration.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbTrigActionStart

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_FDB_ACTION_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_FDB_ACTION_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbTrigActionStart(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionModeSet
*
* DESCRIPTION:
*       Sets FDB action mode without setting Action Trigger
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*       mode    - FDB action mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_FDB_ACTION_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_FDB_ACTION_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbActionModeGet
*
* DESCRIPTION:
*       Gets FDB action mode.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       modePtr    - FDB action mode
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_FDB_ACTION_MODE_ENT        mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacTriggerToggle
*
* DESCRIPTION:
*       Toggle Aging Trigger and cause the device to scan its MAC address table.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacTriggerToggle

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbMacTriggerToggle(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeSet
*
* DESCRIPTION:
*       Sets bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number.
*       trunkAgingMode - FDB aging mode for trunk entries.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
*                 In Controlled Aging mode, the device ages Trunk entries
*                 whether it is the owner or not. When a Trunk entry is aged,
*                 it is not deleted from the MAC table, but an Aged
*                 Address (AA) message is sent to the CPU. This allows
*                 the CPU to track whether the MAC has been aged on all
*                 devices that have port members for the Trunk. Only then
*                 the CPU explicitly deletes the Trunk entry on all
*                 devices.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
*                 This is Automatic Trunk Entry Aging mode, which treats
*                 Trunk entries in the MAC table just like non-Trunk entries.
*                 That is, aging is performed only on entries owned by the
*                 device. The advantage of this mode is that it does not
*                 require any CPU intervention. This mode has the
*                 disadvantage of possibly aging a Trunk entry on all devices
*                 when there still may be active traffic for the MAC on a
*                 different device, where packets were sent over another
*                 link of the Trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or aging mode
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbTrunkAgingModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT     trunkAgingMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkAgingMode = (CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbTrunkAgingModeSet(devNum, trunkAgingMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbTrunkAgingModeGet
*
* DESCRIPTION:
*       Get bridge FDB Aging Mode for trunk entries.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       trunkAgingModePtr - (pointer to) FDB aging mode for trunk entries.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ALL_E:
*                 In Controlled Aging mode, the device ages Trunk entries
*                 whether it is the owner or not. When a Trunk entry is aged,
*                 it is not deleted from the MAC table, but an Aged
*                 Address (AA) message is sent to the CPU. This allows
*                 the CPU to track whether the MAC has been aged on all
*                 devices that have port members for the Trunk. Only then
*                 the CPU explicitly deletes the Trunk entry on all
*                 devices.
*            CPSS_FDB_AGE_TRUNK_ENTRIES_ASSOCIATE_ONLY_WITH_OWN_DEVNUM_E:
*                 This is Automatic Trunk Entry Aging mode, which treats
*                 Trunk entries in the MAC table just like non-Trunk entries.
*                 That is, aging is performed only on entries owned by the
*                 device. The advantage of this mode is that it does not
*                 require any CPU intervention. This mode has the
*                 disadvantage of possibly aging a Trunk entry on all devices
*                 when there still may be active traffic for the MAC on a
*                 different device, where packets were sent over another
*                 link of the Trunk.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or aging mode
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbTrunkAgingModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_DXCH_BRG_FDB_AGE_TRUNK_MODE_ENT     trunkAgingMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbTrunkAgingModeGet(devNum, &trunkAgingMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", trunkAgingMode);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbInit
*
* DESCRIPTION:
*       Init FDB system facility for a device.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum         - physical device number.
*       numOfRanges - max possible number of ranges in the system
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbInit

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/* table cpssDxChBrgFdbAuMsgBlock global variable */
static GT_U32   actNumOfAu = 0;
static CPSS_MAC_UPDATE_MSG_EXT_STC    *auMessagesPtr = NULL;
static GT_U32   numOfAu;


/*
* wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint
*
* DESCRIPTION:
*       Print Update (AU) messages
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      NONE
*
* OUTPUTS:
*      NONE
* RETURNS:
*      CMD_OK
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
static CMD_STATUS wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint
(
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    OUT GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    GT_U32                         tempSip, tempDip;
    GT_PORT_NUM                    dummyPort = 0;/* dummy port num for conversion */
    GT_U32                         ii;
    CPSS_MAC_UPDATE_MSG_EXT_STC    *currAuMessagesPtr;/* current message pointer */

    currAuMessagesPtr = &auMessagesPtr[actNumOfAu-1];

    inFields[0] = actNumOfAu;

    inFields[1] = currAuMessagesPtr->updType;
    inFields[2] = currAuMessagesPtr->entryWasFound;
    inFields[3] = currAuMessagesPtr->macEntryIndex;

    inFields[4] = currAuMessagesPtr->macEntry.key.entryType;

    switch(currAuMessagesPtr->macEntry.key.entryType)
    {
    case 0:
       inFields[6] = currAuMessagesPtr->macEntry.key.key.macVlan.vlanId;
       break;

    case 1:
    case 2:
       inFields[6] = currAuMessagesPtr->macEntry.key.key.ipMcast.vlanId;
       break;

    default:
        /* illegal key type */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);
        return CMD_AGENT_ERROR;
    }

    cmdOsMemCpy(&tempSip, &currAuMessagesPtr->macEntry.key.key.ipMcast.sip[0], 4);
    tempSip = cmdOsHtonl(tempSip);
    inFields[7] = tempSip;

    cmdOsMemCpy(&tempDip, &currAuMessagesPtr->macEntry.key.key.ipMcast.dip[0], 4);
    tempDip = cmdOsHtonl(tempDip);
    inFields[8] = tempDip;

    inFields[9] = currAuMessagesPtr->macEntry.dstInterface.type;
    if (currAuMessagesPtr->macEntry.dstInterface.type == CPSS_INTERFACE_PORT_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(currAuMessagesPtr->macEntry.dstInterface.devPort.hwDevNum,
                            currAuMessagesPtr->macEntry.dstInterface.devPort.portNum);
    }

    inFields[10] = currAuMessagesPtr->macEntry.dstInterface.devPort.hwDevNum;
    inFields[11] = currAuMessagesPtr->macEntry.dstInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(currAuMessagesPtr->macEntry.dstInterface.trunkId);
    inFields[12] = currAuMessagesPtr->macEntry.dstInterface.trunkId;
    inFields[13] = currAuMessagesPtr->macEntry.dstInterface.vidx;
    inFields[14] = currAuMessagesPtr->macEntry.dstInterface.vlanId;

    inFields[15] = currAuMessagesPtr->macEntry.isStatic;
    inFields[16] = currAuMessagesPtr->macEntry.daCommand;
    inFields[17] = currAuMessagesPtr->macEntry.saCommand;
    inFields[18] = currAuMessagesPtr->macEntry.daRoute;
    inFields[19] = currAuMessagesPtr->macEntry.mirrorToRxAnalyzerPortEn;
    inFields[20] = currAuMessagesPtr->macEntry.sourceID;
    inFields[21] = currAuMessagesPtr->macEntry.userDefined;
    inFields[22] = currAuMessagesPtr->macEntry.daQosIndex;
    inFields[23] = currAuMessagesPtr->macEntry.saQosIndex;
    inFields[24] = currAuMessagesPtr->macEntry.daSecurityLevel;
    inFields[25] = currAuMessagesPtr->macEntry.saSecurityLevel;
    inFields[26] = currAuMessagesPtr->macEntry.appSpecificCpuCode;

    inFields[27] = currAuMessagesPtr->skip;
    inFields[28] = currAuMessagesPtr->aging;
    CONVERT_BACK_DEV_PORT_DATA_MAC(currAuMessagesPtr->associatedHwDevNum,
                                   dummyPort);

    ii = 29;
    if (cmdVersion > 0)
    {
        /* for version==1 inserted spUnknown fileld */
        inFields[ii++] = currAuMessagesPtr->macEntry.spUnknown;
    }

    inFields[ii++] = currAuMessagesPtr->associatedHwDevNum;
    inFields[ii++] = currAuMessagesPtr->queryHwDevNum;
    inFields[ii++] = currAuMessagesPtr->naChainIsTooLong;
    inFields[ii++] = currAuMessagesPtr->entryOffset;

    if (cmdVersion > 1)
    {
        inFields[ii++] = currAuMessagesPtr->vid1;
        inFields[ii++] = currAuMessagesPtr->up0;
        inFields[ii++] = currAuMessagesPtr->isMoved;
        inFields[ii++] = currAuMessagesPtr->oldSrcId;
        inFields[ii++] = currAuMessagesPtr->oldAssociatedHwDevNum;
        inFields[ii++] = currAuMessagesPtr->oldDstInterface.type;
        if (currAuMessagesPtr->oldDstInterface.type == CPSS_INTERFACE_PORT_E)
        {
            CONVERT_BACK_DEV_PORT_DATA_MAC(currAuMessagesPtr->oldDstInterface.hwDevNum,
                                currAuMessagesPtr->oldDstInterface.devPort.portNum);
        }
        inFields[ii++] = currAuMessagesPtr->oldDstInterface.devPort.hwDevNum;
        inFields[ii++] = currAuMessagesPtr->oldDstInterface.devPort.portNum;

        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(currAuMessagesPtr->oldDstInterface.trunkId);
        inFields[ii++] = currAuMessagesPtr->oldDstInterface.trunkId;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
               %d%d%d%d%d%d%d%d",          inFields[0],  inFields[1],
                             inFields[2],  inFields[3],  inFields[4],
               currAuMessagesPtr->macEntry.key.key.macVlan.macAddr.arEther,
               inFields[6],  inFields[7],  inFields[8],  inFields[9],
               inFields[10], inFields[11], inFields[12], inFields[13],
               inFields[14], inFields[15], inFields[16], inFields[17],
               inFields[18], inFields[19], inFields[20], inFields[21],
               inFields[22], inFields[23], inFields[24], inFields[25],
               inFields[26], inFields[27], inFields[28], inFields[29],
                             inFields[30], inFields[31], inFields[32]);

    if (cmdVersion > 0)
    {
        fieldOutputSetAppendMode();
        fieldOutput("%d", inFields[33]);
    }

    if (cmdVersion > 1)
    {
        fieldOutputSetAppendMode();
        fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[34], inFields[35],
                    inFields[36], inFields[37], inFields[38], inFields[39],
                    inFields[40], inFields[41], inFields[42]);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}


/*
* cpssDxChBrgFdbAuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK       - on success
*       GT_NO_MORE  - the action succeeded and there are no more waiting
*                     AU messages
*
*       GT_FAIL     - on failure
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    GT_STATUS                             result;
    GT_U8                                 devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numOfAu = (GT_U32)inArgs[1];
    actNumOfAu = 1;

    if(numOfAu == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    auMessagesPtr  =
        (CPSS_MAC_UPDATE_MSG_EXT_STC*)
         cmdOsMalloc(sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu);

    cmdOsMemSet(auMessagesPtr, 0, (sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu));


    /* call cpss api function */
    result = cpssDxChBrgFdbAuMsgBlockGet(devNum, &numOfAu, auMessagesPtr);


    if (((result != GT_OK) && (result != GT_NO_MORE)) || (numOfAu == 0))
    {
        CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    return wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint(
        inFields, outArgs, cmdVersion);
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    actNumOfAu++;

    /* call cpss api function */

    if (actNumOfAu > numOfAu)
    {
        CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint(
        inFields, outArgs, cmdVersion);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockV0GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbAuMsgBlockGetFirst(
        inArgs, inFields, numFields, outArgs, 0 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockV0GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbAuMsgBlockGetNext(
        inArgs, inFields, numFields, outArgs, 0 /*cmdVersion*/);
}
/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockV1GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbAuMsgBlockGetFirst(
        inArgs, inFields, numFields, outArgs, 1 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockV1GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbAuMsgBlockGetNext(
        inArgs, inFields, numFields, outArgs, 1 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockV2GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbAuMsgBlockGetFirst(
        inArgs, inFields, numFields, outArgs, 2 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuMsgBlockV2GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbAuMsgBlockGetNext(
        inArgs, inFields, numFields, outArgs, 2 /*cmdVersion*/);
}

/*******************************************************************************
* cpssDxChBrgFdbFuMsgBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller.
*       The PP may place FU messages in common FDB Address Update (AU) messages
*       queue or in separate queue only for FU messages. The function
*       cpssDxChHwPpPhase2Init configures queue for FU messages by the
*       fuqUseSeparate parameter. If common AU queue is used then function
*       returns all AU messages in the queue including FU ones.
*       If separate FU queue is used then function returns only FU messages.
*
* APPLICABLE DEVICES:  DxCh2 and above
*
* INPUTS:
*       devNum     - the device number from which FU are taken
*       numOfFuPtr - (pointer to)max number of FU messages to get
*
* OUTPUTS:
*       numOfFuPtr - (pointer to)actual number of FU messages that were received
*       fuMessagesPtr - array that holds received FU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfFuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          AU messages
*
*       GT_FAIL                  - on failure
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - not all results of the CNC block upload
*                          retrieved from the common used FU and CNC
*                          upload queue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockGetNext
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
);

/* table cpssDxChBrgFdbFuMsgBlock global variable */
static GT_U32   wrNumOfFu;

/* pointer to FU messages array */
static CPSS_MAC_UPDATE_MSG_EXT_STC * wrFuArrayPtr;

/* current index in FU array */
static GT_U32                        currentFuIndex;

static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockGetFirst
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    GT_STATUS                             result;
    GT_U8                                 devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    wrNumOfFu = (GT_U32)inArgs[1];

    wrFuArrayPtr  =
        (CPSS_MAC_UPDATE_MSG_EXT_STC*)
         cmdOsMalloc(sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * wrNumOfFu);

    if (wrFuArrayPtr == NULL)
    {
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : cannot allocate FU buffer\n");
        galtisOutput(outArgs, GT_OUT_OF_RANGE, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(wrFuArrayPtr, 0, (sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * wrNumOfFu));

    /* call cpss api function */
    result = cpssDxChBrgFdbFuMsgBlockGet(devNum, &wrNumOfFu, wrFuArrayPtr);
    if (result != GT_OK)
    {
        if(result != GT_NO_MORE)
        {
            CMD_OS_FREE_AND_SET_NULL_MAC(wrFuArrayPtr);
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    }

    if(wrNumOfFu == 0)
    {  /* there is no messages */
        CMD_OS_FREE_AND_SET_NULL_MAC(wrFuArrayPtr);
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* start from first entry */
    currentFuIndex = 0;

    return wrCpssDxChBrgFdbFuMsgBlockGetNext(
        inArgs,inFields,numFields,outArgs,cmdVersion);
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockGetNext
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 cmdVersion
)
{
    GT_U32                         tempSip, tempDip;
    GT_PORT_NUM                    dummyPort = 0;/* dummy port num for conversion */
    GT_U32                         ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
    {
        CMD_OS_FREE_AND_SET_NULL_MAC(wrFuArrayPtr);
        return CMD_AGENT_ERROR;
    }

    /* check that we have message */
    if (currentFuIndex == wrNumOfFu)
    {  /* there is no more messages */
        CMD_OS_FREE_AND_SET_NULL_MAC(wrFuArrayPtr);
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = wrNumOfFu;
    inFields[1] = wrFuArrayPtr[currentFuIndex].updType;
    inFields[2] = wrFuArrayPtr[currentFuIndex].entryWasFound;
    inFields[3] = wrFuArrayPtr[currentFuIndex].macEntryIndex;

    inFields[4] = wrFuArrayPtr[currentFuIndex].macEntry.key.entryType;

    switch(wrFuArrayPtr[currentFuIndex].macEntry.key.entryType)
    {
    case 0:
        inFields[6] = wrFuArrayPtr[currentFuIndex].macEntry.key.key.macVlan.vlanId;
        break;

    case 1:
    case 2:
        inFields[6] = wrFuArrayPtr[currentFuIndex].macEntry.key.key.ipMcast.vlanId;
        break;

    default:
        /* illegal key type */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        CMD_OS_FREE_AND_SET_NULL_MAC(wrFuArrayPtr);
        return CMD_AGENT_ERROR;
    }

    cmdOsMemCpy(&tempSip, &wrFuArrayPtr[currentFuIndex].macEntry.key.key.ipMcast.sip[0], 4);
    tempSip = cmdOsHtonl(tempSip);
    inFields[7] = tempSip;

    cmdOsMemCpy(&tempDip, &wrFuArrayPtr[currentFuIndex].macEntry.key.key.ipMcast.dip[0], 4);
    tempDip = cmdOsHtonl(tempDip);
    inFields[8] = tempDip;

    inFields[9] = wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.type;
    if (wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.type == CPSS_INTERFACE_PORT_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.devPort.hwDevNum,
                             wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.devPort.portNum);
    }

    inFields[10] = wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.devPort.hwDevNum;
    inFields[11] = wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.trunkId);
    inFields[12] = wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.trunkId;
    inFields[13] = wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.vidx;
    inFields[14] = wrFuArrayPtr[currentFuIndex].macEntry.dstInterface.vlanId;

    inFields[15] = wrFuArrayPtr[currentFuIndex].macEntry.isStatic;
    inFields[16] = wrFuArrayPtr[currentFuIndex].macEntry.daCommand;
    inFields[17] = wrFuArrayPtr[currentFuIndex].macEntry.saCommand;
    inFields[18] = wrFuArrayPtr[currentFuIndex].macEntry.daRoute;
    inFields[19] = wrFuArrayPtr[currentFuIndex].macEntry.mirrorToRxAnalyzerPortEn;
    inFields[20] = wrFuArrayPtr[currentFuIndex].macEntry.sourceID;
    inFields[21] = wrFuArrayPtr[currentFuIndex].macEntry.userDefined;
    inFields[22] = wrFuArrayPtr[currentFuIndex].macEntry.daQosIndex;
    inFields[23] = wrFuArrayPtr[currentFuIndex].macEntry.saQosIndex;
    inFields[24] = wrFuArrayPtr[currentFuIndex].macEntry.daSecurityLevel;
    inFields[25] = wrFuArrayPtr[currentFuIndex].macEntry.saSecurityLevel;
    inFields[26] = wrFuArrayPtr[currentFuIndex].macEntry.appSpecificCpuCode;

    inFields[27] = wrFuArrayPtr[currentFuIndex].skip;
    inFields[28] = wrFuArrayPtr[currentFuIndex].aging;
    CONVERT_BACK_DEV_PORT_DATA_MAC(wrFuArrayPtr[currentFuIndex].associatedHwDevNum,
                             dummyPort);
    ii = 29;
    if (cmdVersion > 0)
    {
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].macEntry.spUnknown;
    }
    inFields[ii++] = wrFuArrayPtr[currentFuIndex].associatedHwDevNum;
    inFields[ii++] = wrFuArrayPtr[currentFuIndex].queryHwDevNum;
    inFields[ii++] = wrFuArrayPtr[currentFuIndex].naChainIsTooLong;
    inFields[ii++] = wrFuArrayPtr[currentFuIndex].entryOffset;

    if (cmdVersion > 1)
    {
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].vid1;
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].up0;
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].isMoved;
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].oldSrcId;
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].oldAssociatedHwDevNum;
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].oldDstInterface.type;
        if (wrFuArrayPtr[currentFuIndex].oldDstInterface.type == CPSS_INTERFACE_PORT_E)
        {
            CONVERT_BACK_DEV_PORT_DATA_MAC(wrFuArrayPtr[currentFuIndex].oldDstInterface.devPort.hwDevNum,
                                 wrFuArrayPtr[currentFuIndex].oldDstInterface.devPort.portNum);
        }
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].oldDstInterface.devPort.hwDevNum;
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].oldDstInterface.devPort.portNum;

        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(wrFuArrayPtr[currentFuIndex].oldDstInterface.trunkId);
        inFields[ii++] = wrFuArrayPtr[currentFuIndex].oldDstInterface.trunkId;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                 %d%d%d%d%d%d%d%d",          inFields[0],  inFields[1],
                               inFields[2],  inFields[3],  inFields[4],
                 wrFuArrayPtr[currentFuIndex].macEntry.key.key.macVlan.macAddr.arEther,
                 inFields[6],  inFields[7],  inFields[8],  inFields[9],
                 inFields[10], inFields[11], inFields[12], inFields[13],
                 inFields[14], inFields[15], inFields[16], inFields[17],
                 inFields[18], inFields[19], inFields[20], inFields[21],
                 inFields[22], inFields[23], inFields[24], inFields[25],
                 inFields[26], inFields[27], inFields[28], inFields[29],
                               inFields[30], inFields[31], inFields[32]);
    if (cmdVersion > 0)
    {
        fieldOutputSetAppendMode();
        fieldOutput("%d", inFields[33]);
    }

    if (cmdVersion > 1)
    {
        fieldOutputSetAppendMode();
        fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[34], inFields[35],
                    inFields[36], inFields[37], inFields[38], inFields[39],
                    inFields[40], inFields[41], inFields[42]);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    /* goto next entry */
    currentFuIndex++;
    return CMD_OK;
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockV0GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbFuMsgBlockGetFirst(
        inArgs, inFields, numFields, outArgs, 0 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockV0GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbFuMsgBlockGetNext(
        inArgs, inFields, numFields, outArgs, 0 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockV1GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbFuMsgBlockGetFirst(
        inArgs, inFields, numFields, outArgs, 1 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockV1GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbFuMsgBlockGetNext(
        inArgs, inFields, numFields, outArgs, 1 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockV2GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbFuMsgBlockGetFirst(
        inArgs, inFields, numFields, outArgs, 2 /*cmdVersion*/);
}

/*************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbFuMsgBlockV2GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbFuMsgBlockGetNext(
        inArgs, inFields, numFields, outArgs, 2 /*cmdVersion*/);
}

/************************************************************************
* cpssDxChBrgFdbAuqFuqMessagesNumberGet
*
* DESCRIPTION:
*       The function scan the AU/FU queues and returns the number of
*       AU/FU messages in the queue.
*
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number on which AU are counted
*       queueType - AUQ or FUQ. FUQ valid for Ch2 and above.
*
* OUTPUTS:
*       numOfAuPtr - (pointer to) number of AU messages in the specified queue.
*       endOfQueueReachedPtr - (pointer to) GT_TRUE: The queue reached to the end.
*                                        GT_FALSE: else
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In order to have the accurate number of entries  application should
*       protect Mutual exclusion between HW access to the AUQ/FUQ
*       SW access to the AUQ/FUQ and calling to this API.
*       i.e. application should stop the PP from sending AU messages to CPU.
*       and should not call  the api's
*       cpssDxChBrgFdbFuMsgBlockGet, cpssDxChBrgFdbFuMsgBlockGet
*
***********************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAuqFuqMessagesNumberGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                          devNum;
    CPSS_DXCH_FDB_QUEUE_TYPE_ENT   queueType;
    GT_U32                         numOfAu;
    GT_BOOL                        endOfQueueReached;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueType = (CPSS_DXCH_FDB_QUEUE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbAuqFuqMessagesNumberGet(devNum,queueType,&numOfAu,&endOfQueueReached);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", numOfAu, endOfQueueReached);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableSet
*
* DESCRIPTION:
*       Enables/disables destination address-based aging. When this bit is
*       set, the aging process is done both on the source and the destination
*       address (i.e. the age bit will be refresh when MAC DA hit occurs, as
*       well as MAC SA hit).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE - enable refreshing
*                 GT_FALSE - disable refreshing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgeBitDaRefreshEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgeBitDaRefreshEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbAgeBitDaRefreshEnableGet
*
* DESCRIPTION:
*       Gets status (enabled/disabled) of destination address-based aging bit.
*       When this bit is set, the aging process is done both on the source and
*       the destination address (i.e. the age bit will be refresh when MAC DA
*       hit occurs, as well as MAC SA hit).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status DA refreshing of aged bit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbAgeBitDaRefreshEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8          devNum;
    GT_BOOL        enable;

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbAgeBitDaRefreshEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableSet
*
* DESCRIPTION:
*       Enable or disable learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
*
* APPLICABLE DEVICES:  DxCh3 and above devices
*
* INPUTS:
*       devNum      - device number
*       enable      - enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutedLearningEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8          devNum;
    GT_BOOL        enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutedLearningEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbRoutedLearningEnableGet
*
* DESCRIPTION:
*       Get Enable or Disable state of learning on routed packets.
*       There may be cases when the ingress device has modified
*       that packet’s MAC SA to the router’s MAC SA.
*       Disable learning on routed packets prevents the FDB from
*       being filled with unnecessary routers’ Source Addresses.
*
* APPLICABLE DEVICES:  DxCh3 and above devices
*
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable or disable Learning from Routed packets
*                     GT_TRUE  - enable Learning from Routed packets
*                     GT_FALSE - disable Learning from Routed packets
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutedLearningEnableGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8          devNum;
    GT_BOOL        enable;

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbRoutedLearningEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryAgeBitSet
*
* DESCRIPTION:
*       Set age bit in specific FDB entry.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       index           - hw mac entry index
*       age            - Age flag that is used for the two-step Aging process.
*                        GT_FALSE - The entry will be aged out in the next pass.
*                        GT_TRUE - The entry will be aged-out in two age-passes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryAgeBitSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U32          index;
    GT_BOOL         age;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    age = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntryAgeBitSet(devNum, index, age);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableSet
*
* DESCRIPTION:
*       Enable/Disable Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                 GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaMsgVid1EnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_BOOL         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbNaMsgVid1EnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbNaMsgVid1EnableGet
*
* DESCRIPTION:
*       Get the status of Tag1 VLAN Id assignment in vid1 field of the NA AU
*       message CPSS_MAC_UPDATE_MSG_EXT_STC (Enabled/Disabled).
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE: vid1 field of the NA AU message is taken from
*                          Tag1 VLAN.
*                    GT_FALSE: vid1 field of the NA AU message is taken from
*                          Tag0 VLAN.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbNaMsgVid1EnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_BOOL         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbNaMsgVid1EnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgFdbGlobalAgeBitEnableSet
*
* DESCRIPTION:
*       Globally enable set of age bit in FDB entry to given value
*       (not to default one).
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - device number
*       enable          - GT_TRUE - each time that the FDB entry will be set,
*                         age bit will be set to the given value.
*                         GT_FALSE - age bit will be allways set to the default value.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_OUT_OF_RANGE          - index out of range
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbGlobalAgeBitEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BOOL     enable;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    enable = (GT_BOOL)inArgs[0];

    setAgeToDefault = (enable == GT_TRUE) ? GT_FALSE : GT_TRUE;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

static GT_U32  numOfSkip;
static GT_U32  numOfValid;
static GT_U32  numOfAged;
static GT_U32  numOfStormPrevention;
static GT_BOOL numOfErrors;
static GT_U32  numBuckets3Or4;/*number of buckets that hold more then 2 'real' entries (valid-skipped) */

static GT_BOOL calledFromGaltis_count_debug = GT_FALSE;
static GT_U32  lastPortGroupId_count_debug = 0;

/*******************************************************************************
* cpssDxChBrgFdbPortGroupCount_debug
*
* DESCRIPTION:
*       This function count number of valid ,skip entries - for specific port group
*
* INPUTS:
*       devNum  - physical device number.
*       portGroupId - port group Id
*           when CPSS_PORT_GROUP_UNAWARE_MODE_CNS meaning read from first port group
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupCount_debug
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId
)
{
    GT_STATUS  rc;
    static GT_U32     hwData[16];
    GT_U32     entryIndex;
    GT_U32     valid;
    GT_U32     skip;
    GT_U32     age;
    GT_U32     spUnknown;
    GT_U32     fdbSize;
    GT_U32     validInBucket = 0;
    GT_U32     fdbUtilization;

    numOfSkip = 0;
    numOfValid = 0;
    numOfAged = 0;
    numOfStormPrevention = 0;
    numOfErrors = 0;
    numBuckets3Or4 = 0;

    rc = cpssDxChCfgTableNumEntriesGet(devNum,CPSS_DXCH_CFG_TABLE_FDB_E,&fdbSize);
    if(rc != GT_OK)
    {
        if(calledFromGaltis_count_debug == GT_FALSE)
        {
        cmdOsPrintf(" === Error ===\n");
        }
        return rc;
    }

    if(calledFromGaltis_count_debug == GT_FALSE)
    {
        cmdOsPrintf(" === FDB counter ===\n");
    }

    if(portGroupId != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
    {
        if(calledFromGaltis_count_debug == GT_FALSE)
        {
            cmdOsPrintf(" === on portGroupId [%d] ===\n" , portGroupId);
        }
    }

    for( entryIndex = 0 ; entryIndex < fdbSize; entryIndex++)
    {
        if((entryIndex & 0x3) == 0)
        {
            validInBucket = 0;
        }

        rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                       PRV_CPSS_DXCH_TABLE_FDB_E,
                                       entryIndex,
                                       &hwData[0]);
        if(rc != GT_OK)
        {
            numOfErrors++;
            continue;
        }

        valid  = U32_GET_FIELD_MAC(hwData[0],0,1);
        skip   = U32_GET_FIELD_MAC(hwData[0],1,1);
        age    = U32_GET_FIELD_MAC(hwData[0],2,1);
        spUnknown = U32_GET_FIELD_MAC(hwData[3],2,1);

        if (valid == 0)
        {
            continue;
        }

        numOfValid++;

        if(skip)
        {
            numOfSkip++;
            continue;
        }

        if(validInBucket == 2)
        {
            numBuckets3Or4++;
        }

        validInBucket++;

        if (age == 0x0)
        {
            numOfAged++;
        }

        if(spUnknown)
        {
            numOfStormPrevention++;
        }
    }

    if(calledFromGaltis_count_debug == GT_FALSE)
    {
        cmdOsPrintf("[%d]valid ,[%d]skip ,[%d](valid-skip) ,[%d]Aged , [%d]sp, [%d]buckets_3or4_occupy  \n"
            ,numOfValid
            ,numOfSkip
            ,(numOfValid - numOfSkip)
            ,numOfAged
            ,numOfStormPrevention
            ,numBuckets3Or4
            );

        /* calculate the percentage of FDB utilization */
        fdbUtilization = (100*(numOfValid - numOfSkip))/fdbSize;

        cmdOsPrintf("FDB utilization [%d %%] in [%d K] FDB table \n",fdbUtilization,(fdbSize/_1K));
    }

    return GT_OK;
}

/*******************************************************************************
* wrCpssDxChBrgFdbCount_debug1
*
* DESCRIPTION:
*       This function count number of valid ,skip entries - for specific port group
*
* INPUTS:
*       devNum  - physical device number.
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbCount_debug1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U32     portGroupId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        /* end of table */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);

        return CMD_OK;
    }

    /* search for current port group to query */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(portGroupId >= lastPortGroupId_count_debug)
        {
            break;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if(portGroupId > PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup)
    {
        /* end of table */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        /* update for next time */
        lastPortGroupId_count_debug = 0;

        return CMD_OK;
    }


    calledFromGaltis_count_debug = GT_TRUE;
    /* call cpss api function */
    result = cpssDxChBrgFdbPortGroupCount_debug(devNum,portGroupId);

    calledFromGaltis_count_debug = GT_FALSE;

    /* update for next time */
    lastPortGroupId_count_debug = portGroupId + 1;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d"
        ,numOfValid
        ,numOfSkip
        ,(numOfValid - numOfSkip)
        ,numOfAged
        ,numOfStormPrevention
        ,numOfErrors
        );
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbCount_debug
*
* DESCRIPTION:
*       This function count number of valid ,skip entries.
*
* INPUTS:
*       devNum  - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbCount_debug
(
    IN  GT_U8   devNum
)
{
    GT_STATUS rc;
    GT_U32     portGroupId;
    GT_U32  tmpNumOfSkip = 0;
    GT_U32  tmpNumOfValid = 0;
    GT_U32  tmpNumOfAged = 0;
    GT_U32  tmpNumOfStormPrevention = 0;
    GT_BOOL tmpNumOfErrors = 0;

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        return GT_BAD_PARAM;
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        rc  = cpssDxChBrgFdbPortGroupCount_debug(devNum,portGroupId);
        if(rc != GT_OK)
        {
            return rc;
        }

        tmpNumOfSkip             += numOfSkip;
        tmpNumOfValid            += numOfValid;
        tmpNumOfAged             += numOfAged;
        tmpNumOfStormPrevention  += numOfStormPrevention;
        tmpNumOfErrors           += numOfErrors;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    if(PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum))
    {
        cmdOsPrintf(" === FDB counter -- port groups summary ===\n");

        cmdOsPrintf("Num of :[%d] valid , [%d] skip ,[%d](valid-skip)  ,[%d] Aged ,[%d] sp,[%d] errors \n"
        ,tmpNumOfValid
        ,tmpNumOfSkip
        ,(tmpNumOfValid - tmpNumOfSkip)
        ,tmpNumOfAged
        ,tmpNumOfStormPrevention
        ,tmpNumOfErrors
        );
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbPortGroupCount_debug
*
* DESCRIPTION:
*       This function count number of valid ,skip entries - for specific port group
*       the results are returned as OUT parameters
*
* INPUTS:
*       devNum  - physical device number.
*       portGroupId - port group Id
*           when CPSS_PORT_GROUP_UNAWARE_MODE_CNS meaning read from first port group
* OUTPUTS:
*       numOfValidPtr   - number of 'valid bit' = 1
*       numOfSkipPtr    - number of 'skip bit' = 1
*       numOfAgedPtr    - number of 'age bit' = 1
*       numOfStormPreventionPtr - number of 'SP bit' = 1
*       numOfErrorsPtr  - number of errors
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChBrgFdbPortGroupCount_debug_outParam
(
    IN  GT_U8   devNum,
    IN  GT_U32  portGroupId,
    OUT GT_U32  *numOfValidPtr,
    OUT GT_U32  *numOfSkipPtr,
    OUT GT_U32  *numOfAgedPtr,
    OUT GT_U32  *numOfStormPreventionPtr,
    OUT GT_U32  *numOfErrorsPtr
)
{
    GT_STATUS rc;

    CPSS_NULL_PTR_CHECK_MAC(numOfValidPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfSkipPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfAgedPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfStormPreventionPtr);
    CPSS_NULL_PTR_CHECK_MAC(numOfErrorsPtr);

    calledFromGaltis_count_debug = GT_TRUE;
    /* call cpss api function */
    rc = cpssDxChBrgFdbPortGroupCount_debug(devNum,portGroupId);
    if(rc != GT_OK)
    {
        return rc;
    }
    calledFromGaltis_count_debug = GT_FALSE;

    *numOfValidPtr                = numOfValid;
    *numOfSkipPtr                 = numOfSkip;
    *numOfAgedPtr                 = numOfAged;
    *numOfStormPreventionPtr      = numOfStormPrevention;
    *numOfErrorsPtr               = numOfErrors;


    return GT_OK;
}



/*******************************************************************************
* cpssDxChBrgFdbHashCalc
*
* DESCRIPTION:
*       This function calculates the hash index for the FDB table.
*       The FDB table hold 2 types of entries :
*       1. for specific mac address and VLAN id.
*       2. for specific src IP , dst IP and VLAN id.
*
*       for more details see CPSS_MAC_ENTRY_EXT_KEY_STC description.
*
* APPLICABLE DEVICES: All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       macEntryKeyPtr  - pointer to key parameters of the mac entry
*                         according to the entry type:
*                         MAC Address entry -> MAC Address + vlan ID.
*                         IPv4/IPv6 entry -> srcIP + dstIP + vald ID.
*
* OUTPUTS:
*       hashPtr - (pointer to) the hash index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum, bad vlan-Id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbHashCalc
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                      devNum;
    CPSS_MAC_ENTRY_EXT_KEY_STC macEntryKey;
    GT_U32                     hash;
    GT_BYTE_ARRY               sipBArr, dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    macEntryKey.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inArgs[1];

    switch(inArgs[1])
    {
        case 6:
            macEntryKey.vid1 = (GT_U16)inArgs[6];
            /* no break! */
        case 0:
            galtisMacAddr(&macEntryKey.key.macVlan.macAddr, (GT_U8*)inArgs[2]);
            macEntryKey.key.macVlan.vlanId = (GT_U16)inArgs[3];
            break;

        case 7:
        case 8:
            macEntryKey.vid1 = (GT_U16)inArgs[6];
            /* no break! */
        case 1:
        case 2:
            macEntryKey.key.ipMcast.vlanId = (GT_U16)inArgs[3];

            galtisBArray(&sipBArr, (GT_U8*)inArgs[4]);

            cmdOsMemCpy(
                macEntryKey.key.ipMcast.sip, sipBArr.data, 4);

            galtisBArray(&dipBArr, (GT_U8*)inArgs[5]);

            cmdOsMemCpy(
                macEntryKey.key.ipMcast.dip, dipBArr.data, 4);
            break;

        default:
            /* illegal key type */
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
            return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = cpssDxChBrgFdbHashCalc(devNum, &macEntryKey, &hash);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hash);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryStatusGet
*
* DESCRIPTION:
*       Get the Valid and Skip Values of a FDB entry.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - index is out of range
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_BOOL valid;
    GT_BOOL skip;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntryStatusGet(
        devNum, index, &valid, &skip);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", valid, skip);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbQueueFullGet
*
* DESCRIPTION:
*       The function returns the state that the AUQ/FUQ - is full/not full
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number
*       queueType - queue type AU/FU queue
*                   NOTE: when using shared queue for AU and FU messages, this
*                         parameter is ignored (and the AUQ is queried)
*                         meaning that this parameter always ignored for DxCh1
*                         devices.
*
*
* OUTPUTS:
*       isFullPtr - (pointer to) is the relevant queue full
*                   GT_TRUE  - the queue is full
*                   GT_FALSE - the queue is not full
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum , queueType
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbQueueFullGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_FDB_QUEUE_TYPE_ENT queueType;
    GT_BOOL isFull;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueType = (CPSS_DXCH_FDB_QUEUE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbQueueFullGet(devNum, queueType, &isFull);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isFull);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbQueueRewindStatusGet
*
* DESCRIPTION:
*       function check if the specific AUQ was 'rewind' since the last time
*       this function was called for that AUQ
*       this information allow the application to know when to finish processing
*       of the WA relate to cpssDxChBrgFdbTriggerAuQueueWa(...)
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       rewindPtr       - (pointer to) was the AUQ rewind since last call
*                           GT_TRUE - AUQ was rewind since last call
*                           GT_FALSE - AUQ wasn't rewind since last call
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbQueueRewindStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL rewind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbQueueRewindStatusGet(devNum, &rewind);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", rewind);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuSet
*
* DESCRIPTION:
*       Enabled/Disabled sending Aged Address (AA) messages to the CPU for the
*       Storm Prevention (SP) entries when those entries reach an aged out status.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE enable sending the SP AA message to the CPU
*                 GT_FALSE for disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSpAaMsgToCpuSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbSpAaMsgToCpuSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbSpAaMsgToCpuGet
*
* DESCRIPTION:
*       Get the status (enabled/disabled) of sending Aged Address (AA) messages
*       to the CPU for the Storm Prevention (SP) entries when those entries reach
*       an aged out status.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       enablePtr  - pointer to status of sending the SP AA message to the CPU
*                    GT_TRUE - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSpAaMsgToCpuGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbSpAaMsgToCpuGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecondaryAuMsgBlockGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U32          cmdVersion = 1;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numOfAu = (GT_U32)inArgs[1];

    /*reset first*/
    actNumOfAu = 1;

    if(numOfAu == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* allocate memory for messages */
    auMessagesPtr  =
        (CPSS_MAC_UPDATE_MSG_EXT_STC*)
         cmdOsMalloc(sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu);

    /* clear memory for messages */
    cmdOsMemSet(auMessagesPtr, 0, (sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu));


    /* call cpss api function */
    result = cpssDxChBrgFdbSecondaryAuMsgBlockGet(devNum, &numOfAu, auMessagesPtr);

    /* check is call failed */
    if (((result != GT_OK) && (result != GT_NO_MORE)) || (numOfAu == 0))
    {
        CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* print results */
    return wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint(inFields, outArgs, cmdVersion);
}

/*******************************************************************************
* cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecondaryAuMsgBlockGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32          cmdVersion = 1;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    actNumOfAu++;

    /* call cpss api function */
    if (actNumOfAu > numOfAu)
    {
        CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* print results */
    return wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint(inFields, outArgs, cmdVersion);
}



/*******************************************************************************
* cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecondaryAuMsgBlockV2GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U32          cmdVersion = 2;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numOfAu = (GT_U32)inArgs[1];

    /*reset first*/
    actNumOfAu = 1;

    if(numOfAu == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* allocate memory for messages */
    auMessagesPtr  =
        (CPSS_MAC_UPDATE_MSG_EXT_STC*)
         cmdOsMalloc(sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu);

    /* clear memory for messages */
    cmdOsMemSet(auMessagesPtr, 0, (sizeof(CPSS_MAC_UPDATE_MSG_EXT_STC) * numOfAu));


    /* call cpss api function */
    result = cpssDxChBrgFdbSecondaryAuMsgBlockGet(devNum, &numOfAu, auMessagesPtr);

    /* check is call failed */
    if (((result != GT_OK) && (result != GT_NO_MORE)) || (numOfAu == 0))
    {
        CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* print results */
    return wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint(inFields, outArgs, cmdVersion);
}

/*******************************************************************************
* cpssDxChBrgFdbSecondaryAuMsgBlockGet
*
* DESCRIPTION:
*       The function returns a block (array) of FDB Address Update (AU) messages,
*       the max number of elements defined by the caller --> from the secondary AUQ.
*       The function cpssDxChHwPpPhase2Init configures queue for secondary AU
*       messages. If common FU queue is used then function returns all AU
*       messages in the queue including FU ones.
*       If separate AU queue is used then function returns only AU messages.
*
*  APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - the device number from which AU are taken
*       numOfAuPtr - (pointer to)max number of AU messages to get
*
* OUTPUTS:
*       numOfAuPtr - (pointer to)actual number of AU messages that were received
*       auMessagesPtr - array that holds received AU messages
*                       pointer is allocated by the caller , with enough memory
*                       for *numOfAuPtr entries that was given as INPUT
*                       parameter.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NO_MORE   - the action succeeded and there are no more waiting
*                      AU messages
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSecondaryAuMsgBlockV2GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32          cmdVersion = 2;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    actNumOfAu++;

    /* call cpss api function */
    if (actNumOfAu > numOfAu)
    {
        CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* print results */
    return wrPrvCpssDxChBrgFdbAuMsgBlockGetPrint(inFields, outArgs, cmdVersion);
}






/*******************************************************************************
* cpssDxChBrgFdbTriggerAuQueueWa
*
* DESCRIPTION:
*       Trigger the Address Update (AU) queue workaround for case when learning
*       from CPU is stopped due to full Address Update (AU) fifo queue
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum                      - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_STATE             - the secondary AUQ not reached the end of
*                                  queue , meaning no need to set the secondary
*                                  base address again.
*                                  this protect the SW from losing unread messages
*                                  in the secondary queue (otherwise the PP may
*                                  override them with new messages).
*                                  caller may use function
*                                  cpssDxChBrgFdbSecondaryAuMsgBlockGet(...)
*                                  to get messages from the secondary queue.
*       GT_NOT_SUPPORTED         - the device not need / not support the WA
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbTriggerAuQueueWa
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbTriggerAuQueueWa(devNum);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdb16BitFidHashEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: For systems with both new and old devices must configure
*       FID = VID in the eVLAN table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdb16BitFidHashEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdb16BitFidHashEnableSet(
       devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdb16BitFidHashEnableGet
*
* DESCRIPTION:
*       Get global configuration for FID 16 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: hash is calculated with FID[11:0],
*                     GT_TRUE : hash is calculated with FID[15:0]
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdb16BitFidHashEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdb16BitFidHashEnableGet(
       devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMaxLengthSrcIdEnableSet
*
* DESCRIPTION:
*       global enable/disable configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMaxLengthSrcIdEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbMaxLengthSrcIdEnableSet(
       devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMaxLengthSrcIdEnableGet
*
* DESCRIPTION:
*       Get global configuration for Src ID 12 bits.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: The SrcID field in FDB table is 9b.
*                               SrcID[11:9] are used for extending the
*                               user defined bits
*                     GT_TRUE : The SrcID filed in FDB is 12b
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMaxLengthSrcIdEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbMaxLengthSrcIdEnableGet(
       devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbVid1AssignmentEnableSet
*
* DESCRIPTION:
*       Enable/Disable using of VID1 in FDB entry.
*       The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*       from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_FALSE: <OrigVID1> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  OrigVID1> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbVid1AssignmentEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbVid1AssignmentEnableSet(
       devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbVid1AssignmentEnableGet
*
* DESCRIPTION:
*        Get Enable/Disable status of using of VID1 in FDB entry.
*        The Source ID bits [8:6], SA Security Level and DA Security Level are read as 0
*        from the FDB entry when VID1 enabled.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_FALSE: <OrigVID1> is not written in the FDB and is not read from the FDB.
*                             <SrcID>[8:6] can be used for src-id filtering and <SA Security Level>
*                             and <DA Security Level> are written/read from the FDB.
*                     GT_TRUE:  OrigVID1> is written in the FDB and read from the FDB
*                             as described in Mac Based VLAN FS section.
*                             <SrcID>[8:6], <SA Security Level> and <DA Security Level>
*                             are read as 0 from the FDB entry.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbVid1AssignmentEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbVid1AssignmentEnableGet(
       devNum, &enable);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbSaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSaLookupAnalyzerIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    index  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgFdbSaLookupAnalyzerIndexSet(
       devNum, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbSaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB SA lookup has
*       <SA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbSaLookupAnalyzerIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbSaLookupAnalyzerIndexGet(
       devNum, &enable, &index);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbDaLookupAnalyzerIndexSet
*
* DESCRIPTION:
*       Set analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE - set mirror to analyzer index field
*                     GT_FALSE - set ZERO to the analyzer index field
*       index       - Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NOTE: If a previous mechanism assigns a different analyzer index,
*             the higher index wins.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbDaLookupAnalyzerIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    index  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgFdbDaLookupAnalyzerIndexSet(
       devNum, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbDaLookupAnalyzerIndexGet
*
* DESCRIPTION:
*       Get analyzer index used when the FDB DA lookup has
*       <DA Lookup Ingress Mirror to Analyzer Enable> set.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                     GT_TRUE  - analyzer index field is NOT ZERO
*                     GT_FALSE - analyzer index field is set to ZERO
*       indexPtr    - (pointer to) Mirror to Analyzer Index (APPLICABLE RANGES: 0..6)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbDaLookupAnalyzerIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbDaLookupAnalyzerIndexGet(
       devNum, &enable, &index);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveUserDefinedSet
*
* DESCRIPTION:
*       Set the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       actUerDefined       - action active UerDefined
*       actUerDefinedMask   - action active UerDefined Mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE          - actUerDefined or actUerDefinedMask out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is no check done of current status of action's register
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveUserDefinedSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32   actUserDefined;
    GT_U32   actUserDefinedMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];
    actUserDefined      = (GT_U32)inArgs[1];
    actUserDefinedMask  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveUserDefinedSet(
       devNum, actUserDefined, actUserDefinedMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbActionActiveUserDefinedGet
*
* DESCRIPTION:
*       Get the Active userDefined and active userDefined mask , that
*       control the FDB actions of : Trigger aging , transplant , flush ,
*       flush port.
*       A FDB entry will be treated by the FDB action only if the entry's
*       "userDefined" masked by the "userDefinedmask" equal to
*       "Active userDefined"
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       actUerDefinedPtr      - pointer to action active UerDefined
*       actUerDefinedMaskPtr   - pointer to action active UerDefined Mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbActionActiveUserDefinedGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32   actUserDefined;
    GT_U32   actUserDefinedMask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum              = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbActionActiveUserDefinedGet(
       devNum, &actUserDefined, &actUserDefinedMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", actUserDefined, actUserDefinedMask);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbLearnPrioritySet
*
* DESCRIPTION:
*       Set Learn priority per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       learnPriority  - Learn priority can be Low or High
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPrioritySet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbLearnPrioritySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                                devNum;
    GT_PORT_NUM                          portNum;
    CPSS_DXCH_FDB_LEARN_PRIORITY_ENT     learnPriority;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    portNum        = (GT_PORT_NUM)inArgs[1];
    learnPriority  = (CPSS_DXCH_FDB_LEARN_PRIORITY_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgFdbLearnPrioritySet(
       devNum, portNum, learnPriority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbLearnPriorityGet
*
* DESCRIPTION:
*       Get Learn priority per port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       learnPriorityPtr  - (pointer to)Learn priority can be Low or High
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkLearnPriorityGet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbLearnPriorityGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                                devNum;
    GT_PORT_NUM                          portNum;
    CPSS_DXCH_FDB_LEARN_PRIORITY_ENT     learnPriority;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    portNum        = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgFdbLearnPriorityGet(
       devNum, portNum, &learnPriority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", learnPriority);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbUserGroupSet
*
* DESCRIPTION:
*       Set User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       userGroup   - user group
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbUserGroupSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS        result;
    GT_U8            devNum;
    GT_PORT_NUM      portNum;
    GT_U32           userGroup;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    portNum        = (GT_PORT_NUM)inArgs[1];
    userGroup      = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgFdbUserGroupSet(
       devNum, portNum, userGroup);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbUserGroupGet
*
* DESCRIPTION:
*       Get User Group per port, used for Mac SA moved logic
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       portNum- port number
*
* OUTPUTS:
*       userGroupPtr  - (pointer to) user group
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       There is a parallel API for trunk configuration: cpssDxChTrunkUserGroupGet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbUserGroupGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS        result;
    GT_U8            devNum;
    GT_PORT_NUM      portNum;
    GT_U32           userGroup;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    portNum        = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChBrgFdbUserGroupGet(
       devNum, portNum, &userGroup);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", userGroup);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbCrcHashUpperBitsModeSet
*
* DESCRIPTION:
*       Set mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       mode        - the upper bits mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbCrcHashUpperBitsModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                          result;
    GT_U8                                              devNum;
    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    mode    = (CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbCrcHashUpperBitsModeSet(
       devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbCrcHashUpperBitsModeGet
*
* DESCRIPTION:
*       Get mode how the CRC hash (CPSS_MAC_HASH_FUNC_CRC_E or
*       CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E)
*       will use 16 'most upper bits' for lookup key of type 'MAC+FID'
*       (CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
*       relevant when using 'Independent VLAN Learning' (CPSS_IVL_E)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       modePtr     - (pointer to) the upper bits mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbCrcHashUpperBitsModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                          result;
    GT_U8                                              devNum;
    CPSS_DXCH_BRG_FDB_CRC_HASH_UPPER_BITS_MODE_ENT     mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbCrcHashUpperBitsModeGet(
       devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgFdbBankCounterValueGet_util
*
* DESCRIPTION:
*       Get the value of counter of the specific FDB table bank.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
* OUTPUTS:
*       valuePtr    - (pointer to)the value of the counter.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbBankCounterValueGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL    getFirst  /* GT_TRUE - first, GT_FALSE - next */
)
{
    static GT_U32         bankIndex = 0;
    GT_STATUS      result;
    GT_U8          devNum;
    GT_U32         value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    if (getFirst == GT_FALSE)
    {
        bankIndex  ++;
    }
    else
    {
        bankIndex  = 0;
    }

    /* call cpss api function */
    result = cpssDxChBrgFdbBankCounterValueGet(
       devNum, bankIndex, &value);

    if (result == GT_OK)
    {
        /* pack output arguments to galtis string */
        fieldOutput("%d%d", bankIndex, value);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", 0xFFFFFFFF);
    }

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChBrgFdbBankCounterTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbBankCounterValueGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE /*getFirst*/);
}

static CMD_STATUS wrCpssDxChBrgFdbBankCounterTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChBrgFdbBankCounterValueGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE /*getFirst*/);
}

/*******************************************************************************
* cpssDxChBrgFdbBankCounterUpdate
*
* DESCRIPTION:
*       Update (increment/decrement) the counter of the specific bank
*       The function will fail (GT_BAD_STATE) if the PP is busy with previous
*       update.
*       the application can determine if PP is ready by calling
*       cpssDxChBrgFdbBankCounterUpdateStatusGet.
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*       NOTE: this function is needed to be called after calling one of the next:
*           'write by index' - cpssDxChBrgFdbMacEntryWrite ,
*                              cpssDxChBrgFdbPortGroupMacEntryWrite
*           'invalidate by index' - cpssDxChBrgFdbMacEntryInvalidate ,
*                              cpssDxChBrgFdbPortGroupMacEntryInvalidate
*
*       The application logic should be:
*           if last action was 'write by index' then :
*               if the previous entry (in the index) was valid --
*                               do no call this function.
*               if the previous entry (in the index) was not valid --
*                               do 'increment'.
*           if last action was 'invalidate by index' then :
*               if the previous entry (in the index) was valid --
*                               do 'decrement'.
*               if the previous entry (in the index) was not valid --
*                               do no call this function.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       bankIndex   - the index of the bank
*                   (APPLICABLE RANGES: 0..15)
*       incOrDec    - increment or decrement the counter by one.
*                   GT_TRUE - increment the counter
*                   GT_FALSE - decrement the counter
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - wrong devNum
*       GT_OUT_OF_RANGE - bankIndex > 15
*       GT_BAD_STATE    - the PP is not ready to get a new update from CPU.
*                         (PP still busy with previous update)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbBankCounterUpdateTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8          devNum;
    GT_U32         bankIndex;
    GT_BOOL        incOrDec;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    bankIndex  = (GT_U32)inFields[0];
    incOrDec   = (GT_BOOL)inFields[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbBankCounterUpdate(
       devNum, bankIndex, incOrDec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbBankCounterUpdateStatusGet
*
* DESCRIPTION:
*       Get indication if PP finished processing last update of counter of a
*       specific bank. (cpssDxChBrgFdbBankCounterUpdate)
*       relevant to CPSS_MAC_HASH_FUNC_CRC_MULTI_HASH_E
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       isFinishedPtr - (pointer to) indication that the PP finished processing
*                   the last counter update.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbBankCounterUpdateStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8          devNum;
    GT_BOOL        isFinished;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChBrgFdbBankCounterUpdateStatusGet(
       devNum, &isFinished);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isFinished);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryMove
*
* DESCRIPTION:
*       The function uses the NA message to send to the PP request to move 'valid'
*       entry from <origFdbIndex> in the FDB table to 'new index'.
*       The new index is chosen by the PP (according to the key of the 'Original entry')
*       in similar way to entry that not exists in the FDB and need to be inserted,
*       but with single difference that it can't use the <origFdbIndex>.
*       (AU message to the PP is non direct access to MAC address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*       The PP operation status can be retrieved by cpssDxChBrgFdbFromCpuAuMsgStatusGet
*       NOTE: this operation in the PP may fail on one of:
*           1. Original entry is not valid.
*           2. Original entry type is not supported.(IPv6 data/key)
*           3. No free destinations.
*
*
* APPLICABLE DEVICES:
*        Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2.
*
* INPUTS:
*       devNum          - device number
*       origFdbIndex    - index of the entry that need to be moved to new index
*                       (as will be chosen by the device)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - bad device number
*       GT_OUT_OF_RANGE          - origFdbIndex out of FDB table range
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbMacEntryMove
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS      result;
    GT_U8          devNum;
    GT_U32         origFdbIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
    origFdbIndex  = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChBrgFdbMacEntryMove(
       devNum, origFdbIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbHashCrcMultiResultsCalc
*
* DESCRIPTION:
*       This function calculates the CRC multiple hash results
*       (indexes into the FDB table).
*       NOTE: the function do not access the HW , and do only SW calculations.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       entryKeyPtr  - (pointer to) entry key
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                   (APPLICABLE RANGES: 0..15)
*      numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 .
*                   (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or parameters in entryKeyPtr.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbHashCrcMultiResultsCalc
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U16                      vlanId;
    CPSS_MAC_ENTRY_EXT_KEY_STC  entryKey;
    GT_U32                      multiHashStartBankIndex;
    GT_U32                      numOfBanks;
    GT_U32                      crcMultiHashArr[16];
    GT_BYTE_ARRY                byteArray;
    GT_U16                      vid1;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&(crcMultiHashArr[0]), 0, sizeof(crcMultiHashArr));
    cpssOsMemSet(&entryKey, 0, sizeof(entryKey));

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    vlanId             = (GT_U16)inArgs[1];
    entryKey.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inArgs[2];
    vid1               = (GT_U16)inArgs[8];

    if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
    {
        entryKey.key.macVlan.vlanId = vlanId;
        galtisMacAddr(&(entryKey.key.macVlan.macAddr), (GT_U8*)inArgs[3]);
    }
    else if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E)
    {
        entryKey.key.macVlan.vlanId = vlanId;
        galtisMacAddr(&(entryKey.key.macVlan.macAddr), (GT_U8*)inArgs[3]);
        entryKey.vid1 = (GT_U16)inArgs[8];
    }
    else
    {
        entryKey.key.ipMcast.vlanId = vlanId;
        /*SIP*/
        galtisBArray(&byteArray, (GT_U8*)inArgs[4]);
        cmdOsMemCpy(entryKey.key.ipMcast.sip, byteArray.data, 4);
        galtisBArrayFree(&byteArray);
        /*DIP*/
        galtisBArray(&byteArray, (GT_U8*)inArgs[5]);
        cmdOsMemCpy(entryKey.key.ipMcast.dip, byteArray.data, 4);
        galtisBArrayFree(&byteArray);
        /* vid1 */
        if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E ||
            entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E)
        {
            entryKey.vid1 = vid1;
        }
    }
    multiHashStartBankIndex = (GT_U32)inArgs[6];
    numOfBanks              = (GT_U32)inArgs[7];

    /* call cpss api function */
    result = cpssDxChBrgFdbHashCrcMultiResultsCalc(
       devNum, &entryKey, multiHashStartBankIndex, numOfBanks, crcMultiHashArr);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        crcMultiHashArr[0],  crcMultiHashArr[1],  crcMultiHashArr[2],  crcMultiHashArr[3],
        crcMultiHashArr[4],  crcMultiHashArr[5],  crcMultiHashArr[6],  crcMultiHashArr[7],
        crcMultiHashArr[8],  crcMultiHashArr[9],  crcMultiHashArr[10], crcMultiHashArr[11],
        crcMultiHashArr[12], crcMultiHashArr[13], crcMultiHashArr[14], crcMultiHashArr[15]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbHashCrcMultiResultsCalc
*
* DESCRIPTION:
*       This function calculates the CRC multiple hash results
*       (indexes into the FDB table).
*       NOTE: the function do not access the HW , and do only SW calculations.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       entryKeyPtr  - (pointer to) entry key
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                   (APPLICABLE RANGES: 0..15)
*      numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 .
*                   (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - wrong devNum or parameters in entryKeyPtr.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRoutingHashCrcMultiResultsCalc
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U16                      vrfId;
    CPSS_MAC_ENTRY_EXT_KEY_STC  entryKey;
    GT_U32                      multiHashStartBankIndex;
    GT_U32                      numOfBanks;
    GT_U32                      crcMultiHashArr[16];
    GT_IPADDR                   ipv4Dip;
    GT_IPV6ADDR                 ipv6Dip;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&(crcMultiHashArr[0]), 0, sizeof(crcMultiHashArr));
    cpssOsMemSet(&entryKey, 0, sizeof(entryKey));

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    vrfId              = (GT_U16)inArgs[1];
    entryKey.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inArgs[2];
    if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E)
    {
        entryKey.key.ipv4Unicast.vrfId = vrfId;
        galtisIpAddr(&ipv4Dip, (GT_U8*)inArgs[3]);
        cmdOsMemCpy(&(entryKey.key.ipv4Unicast.dip), &(ipv4Dip.u32Ip), sizeof(GT_IPADDR));
    }
    else if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E)
    {
        entryKey.key.ipv6Unicast.vrfId = vrfId;
        galtisIpv6Addr(&ipv6Dip, (GT_U8*)inArgs[4]);
        cmdOsMemCpy(&(entryKey.key.ipv6Unicast.dip), &(ipv6Dip.u32Ip), sizeof(GT_IPV6ADDR));
    }
    else
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_OK;
    }
    multiHashStartBankIndex = (GT_U32)inArgs[6];
    numOfBanks              = (GT_U32)inArgs[7];

    /* call cpss api function */
    result = cpssDxChBrgFdbHashCrcMultiResultsCalc(
       devNum, &entryKey, multiHashStartBankIndex, numOfBanks, crcMultiHashArr);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        crcMultiHashArr[0],  crcMultiHashArr[1],  crcMultiHashArr[2],  crcMultiHashArr[3],
        crcMultiHashArr[4],  crcMultiHashArr[5],  crcMultiHashArr[6],  crcMultiHashArr[7],
        crcMultiHashArr[8],  crcMultiHashArr[9],  crcMultiHashArr[10], crcMultiHashArr[11],
        crcMultiHashArr[12], crcMultiHashArr[13], crcMultiHashArr[14], crcMultiHashArr[15]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbHashRequestSend
*
* DESCRIPTION:
*       The function Send Hash request (HR) message to PP to generate all values
*       of hash results that relate to the 'Entry KEY'.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that HR message was processed by PP.
*       For the results Application can call cpssDxChBrgFdbHashResultsGet.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       entryKeyPtr  - (pointer to) entry key
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or entryKeyPtr->entryType
*       GT_OUT_OF_RANGE          - one of the parameters of entryKeyPtr is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE - the PP is not ready to get a message from CPU.
*                      (PP still busy with previous message)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the HR message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbHashRequestSend
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U16                      vlanId;
    CPSS_MAC_ENTRY_EXT_KEY_STC  entryKey;
    GT_BYTE_ARRY                byteArray;
    GT_U16                      vid1;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&entryKey, 0, sizeof(entryKey));

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    vlanId             = (GT_U16)inArgs[1];
    vid1               = (GT_U16)inArgs[6];
    
    entryKey.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inArgs[2];
    if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E)
    {
        entryKey.key.macVlan.vlanId = vlanId;
        galtisMacAddr(&(entryKey.key.macVlan.macAddr), (GT_U8*)inArgs[3]);
    }
    else if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E)
    {
        entryKey.key.macVlan.vlanId = vlanId;
        galtisMacAddr(&(entryKey.key.macVlan.macAddr), (GT_U8*)inArgs[3]);
        entryKey.vid1 = vid1;
    }
    else
    {
        entryKey.key.ipMcast.vlanId = vlanId;
        /*SIP*/
        galtisBArray(&byteArray, (GT_U8*)inArgs[4]);
        cmdOsMemCpy(entryKey.key.ipMcast.sip, byteArray.data, 4);
        galtisBArrayFree(&byteArray);
        /*DIP*/
        galtisBArray(&byteArray, (GT_U8*)inArgs[5]);
        cmdOsMemCpy(entryKey.key.ipMcast.dip, byteArray.data, 4);
        galtisBArrayFree(&byteArray);
        /* vid1 */
        if (entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E ||
            entryKey.entryType == CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E)
        {
            entryKey.vid1 = vid1;
        }

    }

    /* call cpss api function */
    result = cpssDxChBrgFdbHashRequestSend(
       devNum, &entryKey);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbHashResultsGet
*
* DESCRIPTION:
*       The function return from the HW the result of last performed AU message
*       from the CPU to PP.
*       Meaning the hash results updated after every call to one of the next:
*       cpssDxChBrgFdbQaSend , cpssDxChBrgFdbMacEntryDelete , cpssDxChBrgFdbHashRequestSend
*       , cpssDxChBrgFdbPortGroupMacEntrySet, cpssDxChBrgFdbPortGroupQaSend ,
*       cpssDxChBrgFdbPortGroupMacEntryDelete , cpssDxChBrgFdbMacEntryMove functions.
*       Application can check that any of those message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet or cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet.
*
*       Retrieving the hash calc are optional per type:
*           1. xorHashPtr = NULL --> no retrieve XOR calc
*           2. crcHashPtr = NULL --> no retrieve CRC calc
*           3. numOfBanks = 0    --> no retrieve of 'multi hash' CRC results
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum       - device number
*       multiHashStartBankIndex  - the first bank for 'multi hash' result.
*                             this parameter relevant only when numOfBanks != 0
*                   (APPLICABLE RANGES: 0..15)
*      numOfBanks    - number of banks for 'multi hash' result.
*                   this value indicates the number of elements that will be
*                   retrieved by crcMultiHashArr[]
*                   value 0 means that no need to retrieve any 'multi hash' result.
*                   restriction of (numOfBanks + multiHashStartBankIndex) <= 16 when numOfBanks !=0.
*                   (APPLICABLE RANGES: 0..16)
*      xorHashPtr   - (pointer to) to receive the XOR hash result.
*                     NULL value means that caller not need to retrieve this value.
*      crcHashPtr   - (pointer to) to receive the CRC hash result.
*                     NULL value means that caller not need to retrieve this value.
*
* OUTPUTS:
*      crcMultiHashArr[] - (array of) 'multi hash' CRC results. index in this array is 'bank Id'.
*                     NULL is allowed only when numOfBanks = 0
*                           (index 0 will hold value relate to bank multiHashStartBankIndex).
*      xorHashPtr   - (pointer to) the XOR hash result.
*                      ignored when NULL pointer.
*      crcHashPtr   - (pointer to) the CRC hash result.
*                      ignored when NULL pointer.
*
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or multiHashStartBankIndex or numOfBanks
*       GT_OUT_OF_RANGE          - one of the parameters of entryKeyPtr is out of range
*       GT_BAD_PTR               - crcMultiHashArr[] is NULL pointer when numOfBanks != 0
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbHashResultsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS     result;
    GT_U8         devNum;
    GT_U32        multiHashStartBankIndex;
    GT_U32        numOfBanks;
    GT_U32        crcMultiHashArr[16];
    GT_U32        xorHash;
    GT_U32        crcHash;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&(crcMultiHashArr[0]), 0, sizeof(crcMultiHashArr));

    /* map input arguments to locals */
    devNum                  = (GT_U8)inArgs[0];
    multiHashStartBankIndex = (GT_U32)inArgs[1];
    numOfBanks              = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChBrgFdbHashResultsGet(
       devNum, multiHashStartBankIndex, numOfBanks, crcMultiHashArr, &xorHash, &crcHash);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        xorHash, crcHash,
        crcMultiHashArr[0],  crcMultiHashArr[1],  crcMultiHashArr[2],  crcMultiHashArr[3],
        crcMultiHashArr[4],  crcMultiHashArr[5],  crcMultiHashArr[6],  crcMultiHashArr[7],
        crcMultiHashArr[8],  crcMultiHashArr[9],  crcMultiHashArr[10], crcMultiHashArr[11],
        crcMultiHashArr[12], crcMultiHashArr[13], crcMultiHashArr[14], crcMultiHashArr[15]);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChBrgFdbMacEntrySet
*
* DESCRIPTION:
*       Create new or update existing entry in Hardware FDB table through
*       Address Update message.(AU message to the PP is non direct access to MAC
*       address table).
*       The function use New Address message (NA) format.
*       The function checks that AU messaging is ready  before using it.
*       The function does not check that AU message was processed by PP.
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*       macEntryPtr     - pointer mac table entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - aging timeout exceeds hardware limitation.
*       GT_HW_ERROR  - on hardware error
*       GT_OUT_OF_RANGE - one of the parameters is out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       Application should synchronize call of cpssDxChBrgFdbMacEntrySet,
*       cpssDxChBrgFdbQaSend and cpssDxChBrgFdbMacEntryDelete functions.
*       Invocations of these functions should be mutual exclusive because they
*       use same HW resources.
*       Application can check that the AU message processing has completed by
*       cpssDxChBrgFdbFromCpuAuMsgStatusGet.
*       If the entry type is IPv6, then the Application should set the four
*       bytes for SIP and DIP that selected by  cpssDxChBrgMcIpv6BytesSelectSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRouteEntrySetByMacAddr

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    CPSS_MAC_ENTRY_EXT_STC       macEntry;
    GT_BYTE_ARRY                 dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init ALL fields , so there are no fields that are not covered by the wrapper ! */
    cpssOsMemSet(&macEntry,0,sizeof(macEntry));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    macEntry.key.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inFields[2];

    switch(macEntry.key.entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:
            
            /* illegal key type for routing tab */
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
                 return CMD_OK;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            galtisBArray(&dipBArr, (GT_U8*)inFields[3]);

            cmdOsMemCpy(macEntry.key.key.ipv4Unicast.dip, dipBArr.data, 4);

            macEntry.key.key.ipv4Unicast.vrfId = (GT_U16)inFields[5];

            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            galtisBArray(&dipBArr, (GT_U8*)inFields[4]);

            cmdOsMemCpy(macEntry.key.key.ipv6Unicast.dip, dipBArr.data, 16);

            macEntry.key.key.ipv6Unicast.vrfId = (GT_U16)inFields[5];

            macEntry.fdbRoutingInfo.nextHopDataBankNumber = (GT_U32)inFields[6];

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            /* no key incase of data only entry type
              data entry is supported only by index */
            /* illegal key type */
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
            return CMD_AGENT_ERROR;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
            return CMD_AGENT_ERROR;
    }

    macEntry.dstInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[7];

    switch(macEntry.dstInterface.type)
    {
    case CPSS_INTERFACE_PORT_E:
        macEntry.dstInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[8];
        macEntry.dstInterface.devPort.portNum = (GT_PORT_NUM)inFields[9];
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.devPort.hwDevNum,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case CPSS_INTERFACE_TRUNK_E:
        macEntry.dstInterface.trunkId = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);

        macEntry.dstInterface.hwDevNum = (GT_U8)inFields[8];
        macEntry.dstInterface.devPort.portNum = 0;/* Need only to convert device number */
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.hwDevNum ,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case CPSS_INTERFACE_VIDX_E:
        macEntry.dstInterface.vidx = (GT_U16)inFields[11];
        break;

    case CPSS_INTERFACE_VID_E:
        macEntry.dstInterface.vlanId = (GT_U16)inFields[12];
        break;

    default:
        break;
    }

    if (setAgeToDefault == GT_FALSE)
    {
        macEntry.age = (GT_BOOL)inFields[13];
    }
    else
    {
        macEntry.age = GT_TRUE;
    }

    /* field used for IPv4 and IPv6*/
    macEntry.fdbRoutingInfo.ttlHopLimitDecEnable = (GT_BOOL)inFields[14];
    macEntry.fdbRoutingInfo.ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[15];
    macEntry.fdbRoutingInfo.ingressMirror = (GT_BOOL)inFields[16];
    macEntry.fdbRoutingInfo.ingressMirrorToAnalyzerIndex = (GT_U32)inFields[17];
    macEntry.fdbRoutingInfo.qosProfileMarkingEnable = (GT_BOOL)inFields[18];
    macEntry.fdbRoutingInfo.qosProfileIndex = (GT_U32)inFields[19];

    macEntry.fdbRoutingInfo.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[20];
    macEntry.fdbRoutingInfo.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[21];
    macEntry.fdbRoutingInfo.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[22];

    macEntry.fdbRoutingInfo.countSet = (CPSS_IP_CNT_SET_ENT)inFields[23];
    macEntry.fdbRoutingInfo.trapMirrorArpBcEnable = (GT_BOOL)inFields[24];
    macEntry.fdbRoutingInfo.dipAccessLevel = (GT_U32)inFields[25];
    macEntry.fdbRoutingInfo.ICMPRedirectEnable = (GT_BOOL)inFields[26];
    macEntry.fdbRoutingInfo.mtuProfileIndex = (GT_U32)inFields[27];
    macEntry.fdbRoutingInfo.isTunnelStart = (GT_BOOL)inFields[28];
    macEntry.fdbRoutingInfo.nextHopVlanId = (GT_U16)inFields[29];
    macEntry.fdbRoutingInfo.nextHopARPPointer = (GT_U32)inFields[30];
    macEntry.fdbRoutingInfo.nextHopTunnelPointer = (GT_U32)inFields[31];

    /* fields used for ipv6 only */
    macEntry.fdbRoutingInfo.scopeCheckingEnable = (GT_BOOL)inFields[32];
    macEntry.fdbRoutingInfo.siteId = (CPSS_IP_SITE_ID_ENT)inFields[33];

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntrySet(devNum, &macEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*
* cpssDxChBrgFdbMacEntryWrite
*
* DESCRIPTION:
*       Write the new entry in Hardware MAC address table in specified index.
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum          - physical device number
*       index           - hw mac entry index
*       skip            - entry skip control
*                         GT_TRUE - used to "skip" the entry ,
*                         the HW will treat this entry as "not used"
*                         GT_FALSE - used for valid/used entries.
*       macEntryPtr     - pointer to MAC entry parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum,saCommand,daCommand
*       GT_OUT_OF_RANGE - vidx/trunkId/portNum-devNum with values bigger then HW
*                         support
*                         index out of range
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRouteEntrySetByIndex

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       index;
    GT_BOOL                      skip;
    CPSS_MAC_ENTRY_EXT_STC       macEntry;
    GT_BYTE_ARRY                 dipBArr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];
    skip = (GT_BOOL)inFields[1];

    macEntry.key.entryType = (CPSS_MAC_ENTRY_EXT_TYPE_ENT)inFields[2];

    switch(macEntry.key.entryType)
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_FID_VID1_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_FID_VID1_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_FID_VID1_E:            
            /* illegal key type for routing tab */
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
                 return CMD_OK;
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
            galtisBArray(&dipBArr, (GT_U8*)inFields[3]);

            cmdOsMemCpy(macEntry.key.key.ipv4Unicast.dip, dipBArr.data, 4);

            macEntry.key.key.ipv4Unicast.vrfId = (GT_U16)inFields[5];

            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
            galtisBArray(&dipBArr, (GT_U8*)inFields[4]);

            cmdOsMemCpy(macEntry.key.key.ipv6Unicast.dip, dipBArr.data, 16);

            macEntry.key.key.ipv6Unicast.vrfId = (GT_U16)inFields[5];

            macEntry.fdbRoutingInfo.nextHopDataBankNumber = (GT_U32)inFields[6];

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
            /* no key incase of data only entry type */
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
            return CMD_AGENT_ERROR;
    }

    macEntry.dstInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[7];

    switch(macEntry.dstInterface.type)
    {
    case CPSS_INTERFACE_PORT_E:
        macEntry.dstInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[8];
        macEntry.dstInterface.devPort.portNum = (GT_PORT_NUM)inFields[9];
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.devPort.hwDevNum,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case CPSS_INTERFACE_TRUNK_E:
        macEntry.dstInterface.trunkId = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(macEntry.dstInterface.trunkId);

        macEntry.dstInterface.hwDevNum = (GT_U8)inFields[8];
        macEntry.dstInterface.devPort.portNum = 0;/* Need only to convert device number */
        CONVERT_DEV_PORT_DATA_MAC(macEntry.dstInterface.hwDevNum ,
                             macEntry.dstInterface.devPort.portNum);
        break;

    case CPSS_INTERFACE_VIDX_E:
        macEntry.dstInterface.vidx = (GT_U16)inFields[11];
        break;

    case CPSS_INTERFACE_VID_E:
        macEntry.dstInterface.vlanId = (GT_U16)inFields[12];
        break;

    default:
        break;
    }

    if (setAgeToDefault == GT_FALSE)
    {
        macEntry.age = (GT_BOOL)inFields[13];
    }
    else
    {
        macEntry.age = GT_TRUE;
    }

    /* field used for IPv4 and IPv6*/
    macEntry.fdbRoutingInfo.ttlHopLimitDecEnable = (GT_BOOL)inFields[14];
    macEntry.fdbRoutingInfo.ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[15];
    macEntry.fdbRoutingInfo.ingressMirror = (GT_BOOL)inFields[16];
    macEntry.fdbRoutingInfo.ingressMirrorToAnalyzerIndex = (GT_U32)inFields[17];
    macEntry.fdbRoutingInfo.qosProfileMarkingEnable = (GT_BOOL)inFields[18];
    macEntry.fdbRoutingInfo.qosProfileIndex = (GT_U32)inFields[19];

    macEntry.fdbRoutingInfo.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[20];
    macEntry.fdbRoutingInfo.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[21];
    macEntry.fdbRoutingInfo.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[22];

    macEntry.fdbRoutingInfo.countSet = (CPSS_IP_CNT_SET_ENT)inFields[23];
    macEntry.fdbRoutingInfo.trapMirrorArpBcEnable = (GT_BOOL)inFields[24];
    macEntry.fdbRoutingInfo.dipAccessLevel = (GT_U32)inFields[25];
    macEntry.fdbRoutingInfo.ICMPRedirectEnable = (GT_BOOL)inFields[26];
    macEntry.fdbRoutingInfo.mtuProfileIndex = (GT_U32)inFields[27];
    macEntry.fdbRoutingInfo.isTunnelStart = (GT_BOOL)inFields[28];
    macEntry.fdbRoutingInfo.nextHopVlanId = (GT_U16)inFields[29];
    macEntry.fdbRoutingInfo.nextHopARPPointer = (GT_U32)inFields[30];
    macEntry.fdbRoutingInfo.nextHopTunnelPointer = (GT_U32)inFields[31];

    /* fields used for ipv6 only */
    macEntry.fdbRoutingInfo.scopeCheckingEnable = (GT_BOOL)inFields[32];
    macEntry.fdbRoutingInfo.siteId = (CPSS_IP_SITE_ID_ENT)inFields[33];

    /* call cpss api function */
    result = wrPrvCpssDxChBrgFdbMacEntryWrite(devNum, index, skip, &macEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgFdbRouteEntrySet
*
* DESCRIPTION:
*   The function calls wrCpssDxChBrgFdbRouteEntrysetByIndex or
*   wrCpssDxChBrgFdbRouteEntrysetByMacAddr according to user's
*   preference as set in inArgs[1].
*
*
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbRouteEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
   /* switch by byIndex boolean in inArgs[1]  */

    switch (inArgs[1])
    {
        case 0:
            return wrCpssDxChBrgFdbRouteEntrySetByMacAddr(
                inArgs, inFields ,numFields ,outArgs);
        case 1:
            return wrCpssDxChBrgFdbRouteEntrySetByIndex(
                inArgs, inFields ,numFields ,outArgs);

        default:
            break;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgFdbEntrySet
*
* DESCRIPTION:
*       This function sets FDB table
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbEntrySet
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

    switch (inArgs[4]) /* FDB entry type tab */
    {
    case 0: /* MAC_ADDR/IPV4_MCAST/IPV6_MCAST */
        return wrCpssDxChBrgFdbMacEntrySet(inArgs,inFields,numFields,outArgs);

    case 1:/* IPV4_UC/IPV6_UC */
        return wrCpssDxChBrgFdbRouteEntrySet(inArgs,inFields,numFields,outArgs);

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong FDB ENTRY FORMAT.\n");
        return CMD_AGENT_ERROR;
    }
}

/*******************************************************************************
* wrCpssDxChBrgFdbEntryEndSet
*
* DESCRIPTION:
*       This function sets FDB table
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbEntryEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChBrgFdbEntryCancelSet
*
* DESCRIPTION:
*       This function is called in case of error in FDB Table
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbEntryCancelSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : result is not GT_OK\n");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChBrgFdbMacEntryRead
*
* DESCRIPTION:
*       Reads the new entry in Hardware MAC address table from specified index.
*       This action do direct read access to RAM .
*
* APPLICABLE DEVICES:  All DXCH devices
*
* INPUTS:
*       devNum      - physical device number
*       index       - hw mac entry index
*
* OUTPUTS:
*       validPtr    - (pointer to) is entry valid
*       skipPtr     - (pointer to) is entry skip control
*       agedPtr     - (pointer to) is entry aged
*       associatedHwDevNumPtr = (pointer to) is HW device number associated with
*                     the entry (even for vidx/trunk entries the field is used
*                     by PP for aging/flush/transplant purpose).
*                     Relevant only in case of Mac Adderss entry.
*       entryPtr    - (pointer to) extended Mac table entry
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - index out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS internalDxChBrgFdbEntryReadNext
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_BOOL                         showSkip;
    GT_BOOL                         valid;
    GT_BOOL                         skip;
    GT_BOOL                         aged;
    GT_HW_DEV_NUM                   associatedHwDevNum;

    CPSS_MAC_ENTRY_EXT_STC          entry;
    GT_U32                          tempSip, tempDip;
    GT_PORT_NUM                     dummyPort = 0;/* dummy port num for conversion */
    GT_U32                          portGroupId;/* port group Id iterator */
    GT_U32                          ipv4Dip[4];
    GT_U32                          ipv6Dip[16];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    /* by default : 'ignore skip' (the field of inArgs[1] == GT_FALSE --> ignore skip) */
    showSkip = (GT_BOOL)inArgs[1]; /* use the byIndex field */

    do{
        if(fdbEntryIndex > fdbEntryIndexMaxGet)
        {
            /* the result is ok, this is the last entry wanted */
            result = GT_OK;

            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        /* call cpss api function */
        result = wrPrvCpssDxChBrgFdbMacEntryRead(devNum, fdbEntryIndex, &valid, &skip,
                                        &aged, &associatedHwDevNum, &entry);

        if (result != GT_OK)
        {
             /* check end of table return code */
             if(GT_OUT_OF_RANGE == result)
             {
                 /* the result is ok, this is end of table */
                 result = GT_OK;
             }

             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
        }

        if(useRefreshFdbPerPortGroupId == GT_TRUE)
        {
            /* find first port group in the bmp */

            /* access to read from next port group */
            PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)
            {
                if(portGroupId <= refreshFdbCurrentPortGroupId)
                {
                    /* skip port groups we already been to */
                    continue;
                }

                refreshFdbCurrentPortGroupId = portGroupId;
                break;
            }
            PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)

            if(portGroupId > refreshFdbCurrentPortGroupId)
            {
                /* indication that we finished loop on all port groups */
                fdbEntryIndex++;

                /* state that the next read come from first port group again */
                refreshFdbCurrentPortGroupId = refreshFdbFirstPortGroupId;
            }
        }
        else
        {
            fdbEntryIndex++;
        }
    }while (!valid || (!showSkip && skip));


    if(useRefreshFdbPerPortGroupId == GT_TRUE &&
       refreshFdbCurrentPortGroupId != refreshFdbFirstPortGroupId)
    {
        /* index was not incremented */
        inFields[0] = fdbEntryIndex;
    }
    else
    {
        inFields[0] = fdbEntryIndex - 1;
    }
    inFields[1] = skip;
    inFields[2] = entry.key.entryType;

    switch (entry.key.entryType) /* FDB entry type tab */
    {
        case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:

            /* support Bobcat2 and Lion3 FDB mirroring */
            if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
            {
                entry.mirrorToRxAnalyzerPortEn = entry.daMirrorToRxAnalyzerPortEn;
            }

            switch(entry.key.entryType)
            {
            case CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E:
                inFields[4] = entry.key.key.macVlan.vlanId;
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_MCAST_E:
            case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_MCAST_E:
                inFields[4] = entry.key.key.ipMcast.vlanId;
                break;

            default:
                break;
            }

            cmdOsMemCpy(&tempSip, &entry.key.key.ipMcast.sip[0], 4);
            tempSip = cmdOsHtonl(tempSip);
            inFields[5] = tempSip;

            cmdOsMemCpy(&tempDip, &entry.key.key.ipMcast.dip[0], 4);
            tempDip = cmdOsHtonl(tempDip);
            inFields[6] = tempDip;

            inFields[7] = entry.dstInterface.type;
            if (entry.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                CONVERT_BACK_DEV_PORT_DATA_MAC(entry.dstInterface.devPort.hwDevNum,
                                     entry.dstInterface.devPort.portNum);
            }
            if (entry.dstInterface.type == CPSS_INTERFACE_TRUNK_E)
            {
                CONVERT_BACK_DEV_PORT_DATA_MAC(entry.dstInterface.hwDevNum,
                                     entry.dstInterface.devPort.portNum);
                        entry.dstInterface.devPort.hwDevNum = entry.dstInterface.hwDevNum;
            }

            inFields[8] = entry.dstInterface.devPort.hwDevNum;
            inFields[9] = entry.dstInterface.devPort.portNum;

            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(entry.dstInterface.trunkId);
            inFields[10] = entry.dstInterface.trunkId;
            inFields[11] = entry.dstInterface.vidx;
            inFields[12] = entry.dstInterface.vlanId;

            inFields[13] = entry.isStatic;
            inFields[14] = entry.daCommand;
            inFields[15] = entry.saCommand;
            inFields[16] = entry.daRoute;
            inFields[17] = entry.mirrorToRxAnalyzerPortEn;
            inFields[18] = entry.sourceID;
            inFields[19] = entry.userDefined;
            inFields[20] = entry.daQosIndex;
            inFields[21] = entry.saQosIndex;
            inFields[22] = entry.daSecurityLevel;
            inFields[23] = entry.saSecurityLevel;
            inFields[24] = entry.appSpecificCpuCode;
            inFields[25] = aged;

            CONVERT_BACK_DEV_PORT_DATA_MAC(associatedHwDevNum,
                                           dummyPort);
            inFields[26] = entry.spUnknown;
            inFields[27] = associatedHwDevNum;

            /* pack and output table fields */
            fieldOutput("%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                          inFields[0],  inFields[1],  inFields[2],
                          entry.key.key.macVlan.macAddr.arEther,
                          inFields[4],  inFields[5],  inFields[6],
                          inFields[7],  inFields[8],  inFields[9],
                          inFields[10], inFields[11], inFields[12],
                          inFields[13], inFields[14], inFields[15],
                          inFields[16], inFields[17], inFields[18],
                          inFields[19], inFields[20], inFields[21],
                          inFields[22], inFields[23], inFields[24],
                                        inFields[25], inFields[26],
                          inFields[27],entry.saMirrorToRxAnalyzerPortEn,
                          entry.daMirrorToRxAnalyzerPortEn,entry.key.vid1);

            galtisOutput(outArgs, GT_OK, "%d%f", 0);

            break;

        case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:
        case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:

            switch(entry.key.entryType)
            {
            case CPSS_MAC_ENTRY_EXT_TYPE_IPV4_UC_E:

                cpssOsMemSet(ipv6Dip,0,sizeof(ipv6Dip));
                cmdOsMemCpy(ipv4Dip, &entry.key.key.ipv4Unicast.dip[0], 4);
                /*tempSip = cmdOsHtonl(tempIpv4dip);*/
                inFields[5] = entry.key.key.ipv4Unicast.vrfId;
                inFields[6] = 0;
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_ADDR_ENTRY_E:

                cpssOsMemSet(ipv4Dip,0,sizeof(ipv4Dip));
                cmdOsMemCpy(ipv6Dip, &entry.key.key.ipv6Unicast.dip[0], 16);

                inFields[5] = 0;
                inFields[6] = entry.fdbRoutingInfo.nextHopDataBankNumber;
                break;

            case CPSS_MAC_ENTRY_EXT_TYPE_IPV6_UC_DATA_ENTRY_E:
                cpssOsMemSet(ipv4Dip,0,sizeof(ipv4Dip));
                cpssOsMemSet(ipv6Dip,0,sizeof(ipv6Dip));
                inFields[5] = entry.key.key.ipv6Unicast.vrfId;
                inFields[6] = 0;
                break;

            default:
                break;
            }

            inFields[7] = entry.dstInterface.type;
            if (entry.dstInterface.type == CPSS_INTERFACE_PORT_E)
            {
                CONVERT_BACK_DEV_PORT_DATA_MAC(entry.dstInterface.devPort.hwDevNum,
                                     entry.dstInterface.devPort.portNum);
            }
            if (entry.dstInterface.type == CPSS_INTERFACE_TRUNK_E)
            {
                CONVERT_BACK_DEV_PORT_DATA_MAC(entry.dstInterface.hwDevNum,
                                     entry.dstInterface.devPort.portNum);
                        entry.dstInterface.devPort.hwDevNum = entry.dstInterface.hwDevNum;
            }

            inFields[8] = entry.dstInterface.devPort.hwDevNum;
            inFields[9] = entry.dstInterface.devPort.portNum;

            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(entry.dstInterface.trunkId);
            inFields[10] = entry.dstInterface.trunkId;
            inFields[11] = entry.dstInterface.vidx;
            inFields[12] = entry.dstInterface.vlanId;

            inFields[13] = entry.age;
            inFields[14] = entry.fdbRoutingInfo.ttlHopLimitDecEnable;
            inFields[15] = entry.fdbRoutingInfo.ttlHopLimDecOptionsExtChkByPass;
            inFields[16] = entry.fdbRoutingInfo.ingressMirror;
            inFields[17] = entry.fdbRoutingInfo.ingressMirrorToAnalyzerIndex;
            inFields[18] = entry.fdbRoutingInfo.qosProfileMarkingEnable;
            inFields[19] = entry.fdbRoutingInfo.qosProfileIndex;
            inFields[20] = entry.fdbRoutingInfo.qosPrecedence;
            inFields[21] = entry.fdbRoutingInfo.modifyUp;
            inFields[22] = entry.fdbRoutingInfo.modifyDscp;
            inFields[23] = entry.fdbRoutingInfo.countSet;
            inFields[24] = entry.fdbRoutingInfo.trapMirrorArpBcEnable;
            inFields[25] = entry.fdbRoutingInfo.dipAccessLevel;
            inFields[26] = entry.fdbRoutingInfo.ICMPRedirectEnable;
            inFields[27] = entry.fdbRoutingInfo.mtuProfileIndex;
            inFields[28] = entry.fdbRoutingInfo.isTunnelStart;
            inFields[29] = entry.fdbRoutingInfo.nextHopVlanId;
            inFields[30] = entry.fdbRoutingInfo.nextHopARPPointer;
            inFields[31] = entry.fdbRoutingInfo.nextHopTunnelPointer;
            inFields[32] = entry.fdbRoutingInfo.scopeCheckingEnable;
            inFields[33] = entry.fdbRoutingInfo.siteId;

            /* pack and output table fields */
            fieldOutput("%d%d%d%4b%16b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                          inFields[0],  inFields[1],  inFields[2],
                          ipv4Dip, ipv6Dip, inFields[5],  inFields[6],
                          inFields[7],  inFields[8],  inFields[9],
                          inFields[10], inFields[11], inFields[12],
                          inFields[13], inFields[14], inFields[15],
                          inFields[16], inFields[17], inFields[18],
                          inFields[19], inFields[20], inFields[21],
                          inFields[22], inFields[23], inFields[24],
                          inFields[25], inFields[26], inFields[27],
                          inFields[28], inFields[29], inFields[30],
                          inFields[31], inFields[32], inFields[33]);

            galtisOutput(outArgs, GT_OK, "%d%f", 1);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong FDB ENTRY FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChBrgFdbEntryReadFirst
*
* DESCRIPTION:
*       This function gets next FDB
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbEntryReadFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    GT_U32      portGroupId;/* port group Id iterator */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    fdbEntryIndex = (GT_32)inArgs[2];
    fdbEntryNumOfEntries= (GT_32)inArgs[3];

    /* check if requested refresh range is legal */
    if (fdbEntryIndex >= PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* check if requested refresh is all */
    if ((fdbEntryIndex == 0) && (fdbEntryNumOfEntries==0))
    {
        fdbEntryNumOfEntries = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb;
    }
    else
    {
        /* check if the application request 0 entries but not all */
        if (fdbEntryNumOfEntries == 0)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }

    fdbEntryIndexMaxGet = fdbEntryIndex + fdbEntryNumOfEntries - 1;


    fdbMultiPortGroupsBmpGet(devNum,&multiPortGroupsBmpEnable, &multiPortGroupsBmp);

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) &&
       PRV_CPSS_DXCH_FAMILY_CHECK_MAC(devNum) &&
       (PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.fdbMode >
        PRV_CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E))
    {
        if(multiPortGroupsBmpEnable == GT_FALSE)
        {
            multiPortGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }

        /* get first port group to access */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)
        {
            refreshFdbFirstPortGroupId = portGroupId;
            break;
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,multiPortGroupsBmp,portGroupId)

        refreshFdbCurrentPortGroupId = refreshFdbFirstPortGroupId;
        useRefreshFdbPerPortGroupId = GT_TRUE;
    }
    else
    {
        useRefreshFdbPerPortGroupId = GT_FALSE;
    }


    return internalDxChBrgFdbEntryReadNext(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChBrgFdbEntryReadNext
*
* DESCRIPTION:
*       This function gets FDB Next Entry
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChBrgFdbEntryReadNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    return internalDxChBrgFdbEntryReadNext(inArgs,inFields,numFields,outArgs);
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChBrgFdbNaToCpuPerPortSet",
        &wrCpssDxChBrgFdbNaToCpuPerPortSet,
        3, 0},

    {"cpssDxChBrgFdbPortLearnStatusGet",
        &wrCpssDxChBrgFdbPortLearnStatusGet,
        2, 0},

    {"cpssDxChBrgFdbPortLearnStatusSet",
        &wrCpssDxChBrgFdbPortLearnStatusSet,
        4, 0},

    {"cpssDxChBrgFdbNaToCpuPerPortGet",
        &wrCpssDxChBrgFdbNaToCpuPerPortGet,
        2, 0},

    {"cpssDxChBrgFdbNaStormPreventSet",
        &wrCpssDxChBrgFdbNaStormPreventSet,
        3, 0},

    {"cpssDxChBrgFdbNaStormPreventGet",
        &wrCpssDxChBrgFdbNaStormPreventGet,
        2, 0},

    {"cpssDxChBrgFdbPortVid1LearningEnableSet",
        &wrCpssDxChBrgFdbPortVid1LearningEnableSet,
        3, 0},

    {"cpssDxChBrgFdbPortVid1LearningEnableGet",
        &wrCpssDxChBrgFdbPortVid1LearningEnableGet,
        2, 0},


    {"cpssDxChBrgFdbPortRoutingEnableSet",
        &wrCpssDxChBrgFdbPortRoutingEnableSet,
        4, 0},

    {"cpssDxChBrgFdbPortRoutingEnableGet",
        &wrCpssDxChBrgFdbPortRoutingEnableGet,
        3, 0},


    {"cpssDxChBrgFdbDeviceTableSet",
        &wrCpssDxChBrgFdbDeviceTableSet,
        2, 0},

    {"cpssDxChBrgFdbDeviceTableGet",
        &wrCpssDxChBrgFdbDeviceTableGet,
        1, 0},

    {"cpssDxChBrgFdbQaSet",
        &wrCpssDxChBrgFdbQaSend,
        1, 5},
    {"cpssDxChBrgFdbQaSetV2",
        &wrCpssDxChBrgFdbQaSend,
        1, 6},


/***   cpssDxChBrgFdbMac table commands   ***/

    {"cpssDxChBrgFdbMacSet",
        &wrCpssDxChBrgFdbMacEntrySet,
        2, 27},

    {"cpssDxChBrgFdbMacGetFirst",
        &wrCpssDxChBrgFdbMacEntryReadV0First,
        2, 0},

    {"cpssDxChBrgFdbMacGetNext",
        &wrCpssDxChBrgFdbMacEntryReadV0Next,
        2, 0},

    {"cpssDxChBrgFdbMacDelete",
        &wrCpssDxChBrgFdbMacEntryDelete,
        2, 27},

/*** end cpssDxChBrgFdbMac table commands ***/

/***   cpssDxChBrgFdbMacV1 table commands   ***/

    {"cpssDxChBrgFdbMacV1Set",
        &wrCpssDxChBrgFdbMacEntrySet,
        2, 28},

    {"cpssDxChBrgFdbMacV1GetFirst",
        &wrCpssDxChBrgFdbMacEntryReadV1First,
        2, 0},

    {"cpssDxChBrgFdbMacV1GetNext",
        &wrCpssDxChBrgFdbMacEntryReadV1Next,
        2, 0},

    {"cpssDxChBrgFdbMacV1Delete",
        &wrCpssDxChBrgFdbMacEntryDelete,
        2, 28},

/*** end cpssDxChBrgFdbMacV1 table commands ***/

/***   cpssDxChBrgFdbMacV2 table commands   ***/

    {"cpssDxChBrgFdbMacV2Set",
        &wrCpssDxChBrgFdbMacEntrySet,
        2, 31},

    {"cpssDxChBrgFdbMacV2GetFirst",
        &wrCpssDxChBrgFdbMacEntryReadV2First,
        2, 0},

    {"cpssDxChBrgFdbMacV2GetNext",
        &wrCpssDxChBrgFdbMacEntryReadV2Next,
        2, 0},

    {"cpssDxChBrgFdbMacV2Delete",
        &wrCpssDxChBrgFdbMacEntryDelete,
        2, 31},

/*** end cpssDxChBrgFdbMacV2 table commands ***/

/***   cpssDxChBrgFdbTable table commands   ***/

    {"cpssDxChBrgFdbTableSet",
        &wrCpssDxChBrgFdbEntrySet,
        5, 35},

    {"cpssDxChBrgFdbTableEndSet",
        &wrCpssDxChBrgFdbEntryEndSet,
        0, 0},

    {"cpssDxChBrgFdbTableCancelSet",
        &wrCpssDxChBrgFdbEntryCancelSet,
        0, 0},

    {"cpssDxChBrgFdbTableGetFirst",
        &wrCpssDxChBrgFdbEntryReadFirst,
        5, 0},

    {"cpssDxChBrgFdbTableGetNext",
        &wrCpssDxChBrgFdbEntryReadNext,
        5, 0},

    {"cpssDxChBrgFdbTableDelete",
        &wrCpssDxChBrgFdbMacEntryDelete,
        5, 35},

/*** end cpssDxChBrgFdbTable table commands ***/

    {"cpssDxChBrgFdbMaxLookupLenSet",
        &wrCpssDxChBrgFdbMaxLookupLenSet,
        2, 0},

    {"cpssDxChBrgFdbMaxLookupLenGet",
        &wrCpssDxChBrgFdbMaxLookupLenGet,
        1, 0},

    {"cpssDxChBrgFdbMacVlanLookupModeSet",
        &wrCpssDxChBrgFdbMacVlanLookupModeSet,
        2, 0},

    {"cpssDxChBrgFdbMacVlanLookupModeGet",
        &wrCpssDxChBrgFdbMacVlanLookupModeGet,
        1, 0},

    {"cpssDxChBrgFdbAuMsgRateLimitSet",
        &wrCpssDxChBrgFdbAuMsgRateLimitSet,
        3, 0},

    {"cpssDxChBrgFdbAuMsgRateLimitGet",
        &wrCpssDxChBrgFdbAuMsgRateLimitGet,
        1, 0},

    {"cpssDxChBrgFdbNaMsgOnChainTooLongSet",
        &wrCpssDxChBrgFdbNaMsgOnChainTooLongSet,
        2, 0},

    {"cpssDxChBrgFdbNaMsgOnChainTooLongGet",
        &wrCpssDxChBrgFdbNaMsgOnChainTooLongGet,
        1, 0},

    {"cpssDxChBrgFdbAAandTAToCpuSet",
        &wrCpssDxChBrgFdbAAandTAToCpuSet,
        2, 0},

    {"cpssDxChBrgFdbAAandTAToCpuGet",
        &wrCpssDxChBrgFdbAAandTAToCpuGet,
        1, 0},

    {"cpssDxChBrgFdbHashModeSet",
        &wrCpssDxChBrgFdbHashModeSet,
        2, 0},

    {"cpssDxChBrgFdbHashModeGet",
        &wrCpssDxChBrgFdbHashModeGet,
        1, 0},

    {"cpssDxChBrgFdbAgingTimeoutSet",
        &wrCpssDxChBrgFdbAgingTimeoutSet,
        2, 0},

    {"cpssDxChBrgFdbAgingTimeoutGet",
        &wrCpssDxChBrgFdbAgingTimeoutGet,
        1, 0},

    {"cpssDxChBrgFdbSecureAgingSet",
        &wrCpssDxChBrgFdbSecureAgingSet,
        2, 0},

    {"cpssDxChBrgFdbSecureAgingGet",
        &wrCpssDxChBrgFdbSecureAgingGet,
        1, 0},

    {"cpssDxChBrgFdbSecureAutoLearnSet",
        &wrCpssDxChBrgFdbSecureAutoLearnSet,
        2, 0},

    {"cpssDxChBrgFdbSecureAutoLearnGet",
        &wrCpssDxChBrgFdbSecureAutoLearnGet,
        1, 0},

    {"cpssDxChBrgFdbStaticTransEnable",
        &wrCpssDxChBrgFdbStaticTransEnable,
        2, 0},

    {"cpssDxChBrgFdbStaticTransEnableGet",
        &wrCpssDxChBrgFdbStaticTransEnableGet,
        1, 0},

    {"cpssDxChBrgFdbStaticDelEnable",
        &wrCpssDxChBrgFdbStaticDelEnable,
        2, 0},

    {"cpssDxChBrgFdbStaticDelEnableGet",
        &wrCpssDxChBrgFdbStaticDelEnableGet,
        1, 0},

    {"cpssDxChBrgFdbActionsEnableSet",
        &wrCpssDxChBrgFdbActionsEnableSet,
        2, 0},

    {"cpssDxChBrgFdbActionsEnableGet",
        &wrCpssDxChBrgFdbActionsEnableGet,
        1, 0},

    {"cpssDxChBrgFdbTrigActionStatusGet",
        &wrCpssDxChBrgFdbTrigActionStatusGet,
        1, 0},

    {"cpssDxChBrgFdbMacTriggerModeSet",
        &wrCpssDxChBrgFdbMacTriggerModeSet,
        2, 0},

    {"cpssDxChBrgFdbMacTriggerModeGet",
        &wrCpssDxChBrgFdbMacTriggerModeGet,
        1, 0},

    {"cpssDxChBrgFdbStaticOfNonExistDevRemove",
        &wrCpssDxChBrgFdbStaticOfNonExistDevRemove,
        2, 0},

    {"cpssDxChBrgFdbStaticOfNonExistDevRemoveGet",
        &wrCpssDxChBrgFdbStaticOfNonExistDevRemoveGet,
        1, 0},

    {"cpssDxChBrgFdbDropAuEnableSet",
        &wrCpssDxChBrgFdbDropAuEnableSet,
        2, 0},

    {"cpssDxChBrgFdbDropAuEnableGet",
        &wrCpssDxChBrgFdbDropAuEnableGet,
        1, 0},

    {"cpssDxChBrgFdbAgeOutAllDevOnTrunkEnable",
        &wrCpssDxChBrgFdbAgeOutAllDevOnTrunkEnable,
        2, 0},

    {"cpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet",
        &wrCpssDxChBrgFdbAgeOutAllDevOnTrunkEnableGet,
        1, 0},

    {"cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable",
        &wrCpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnable,
        2, 0},

    {"cpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet",
        &wrCpssDxChBrgFdbAgeOutAllDevOnNonTrunkEnableGet,
        1, 0},

    {"cpssDxChBrgFdbActionTransplantDataSet",
        &wrCpssDxChBrgFdbActionTransplantDataSet,
        1, 12},

    {"cpssDxChBrgFdbActionTransplantDataGetFirst",
        &wrCpssDxChBrgFdbActionTransplantDataGet,
        1, 0},

    {"cpssDxChBrgFdbActionTransplantDataGetNext",
        &wrCpssDxChBrgFdbActionTransplantDataGetNext,
        1, 0},

    {"cpssDxChBrgFdbFromCpuAuMsgStatusGet",
        &wrCpssDxChBrgFdbFromCpuAuMsgStatusGet,
        1, 0},

    {"cpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet",
        &wrCpssDxChBrgFdbPortGroupFromCpuAuMsgStatusGet,
        1, 0},  /* NOTE: portGroupsBmp not delivered by this API but use the global bmp mode ! */

    {"cpssDxChBrgFdbActionActiveVlanSet",
        &wrCpssDxChBrgFdbActionActiveVlanSet,
        3, 0},

    {"cpssDxChBrgFdbActionActiveVlanGet",
        &wrCpssDxChBrgFdbActionActiveVlanGet,
        1, 0},

    {"cpssDxChBrgFdbActionActiveDevSet",
        &wrCpssDxChBrgFdbActionActiveDevSet,
        3, 0},

    {"cpssDxChBrgFdbActionActiveDevGet",
        &wrCpssDxChBrgFdbActionActiveDevGet,
        1, 0},

    {"cpssDxChBrgFdbActionActiveInterfaceSet",
        &wrCpssDxChBrgFdbActionActiveInterfaceSet,
        5, 0},

    {"cpssDxChBrgFdbActionActiveInterfaceGet",
        &wrCpssDxChBrgFdbActionActiveInterfaceGet,
        1, 0},

    {"cpssDxChBrgFdbUploadEnableSet",
        &wrCpssDxChBrgFdbUploadEnableSet,
        2, 0},

    {"cpssDxChBrgFdbUploadEnableGet",
        &wrCpssDxChBrgFdbUploadEnableGet,
        1, 0},

    {"cpssDxChBrgFdbTrigActionStart",
        &wrCpssDxChBrgFdbTrigActionStart,
        2, 0},

    {"cpssDxChBrgFdbActionModeSet",
        &wrCpssDxChBrgFdbActionModeSet,
        2, 0},

    {"cpssDxChBrgFdbActionModeGet",
        &wrCpssDxChBrgFdbActionModeGet,
        1, 0},

    {"cpssDxChBrgFdbMacTriggerToggle",
        &wrCpssDxChBrgFdbMacTriggerToggle,
        1, 0},

    {"cpssDxChBrgFdbTrunkAgingModeSet",
        &wrCpssDxChBrgFdbTrunkAgingModeSet,
        2, 0},

    {"cpssDxChBrgFdbTrunkAgingModeGet",
        &wrCpssDxChBrgFdbTrunkAgingModeGet,
        1, 0},

    {"cpssDxChBrgFdbInit",
        &wrCpssDxChBrgFdbInit,
        1, 0},

    /* begin of cpssDxChBrgFdbAuMsgBlock table */
    {"cpssDxChBrgFdbAuMsgBlockGetFirst",
        &wrCpssDxChBrgFdbAuMsgBlockV0GetFirst,
        2, 0},

    {"cpssDxChBrgFdbAuMsgBlockGetNext",
        &wrCpssDxChBrgFdbAuMsgBlockV0GetNext,
        2, 0},
    /* end of cpssDxChBrgFdbAuMsgBlock table */

    /* begin of cpssDxChBrgFdbAuMsgBlockV1 table */
    {"cpssDxChBrgFdbAuMsgBlockV1GetFirst",
        &wrCpssDxChBrgFdbAuMsgBlockV1GetFirst,
        2, 0},

    {"cpssDxChBrgFdbAuMsgBlockV1GetNext",
        &wrCpssDxChBrgFdbAuMsgBlockV1GetNext,
        2, 0},
    /* end of cpssDxChBrgFdbAuMsgBlockV1 table */

    /* begin of cpssDxChBrgFdbAuMsgBlockV2 table */
    {"cpssDxChBrgFdbAuMsgBlockV2GetFirst",
        &wrCpssDxChBrgFdbAuMsgBlockV2GetFirst,
        2, 0},

    {"cpssDxChBrgFdbAuMsgBlockV2GetNext",
        &wrCpssDxChBrgFdbAuMsgBlockV2GetNext,
        2, 0},
    /* end of cpssDxChBrgFdbAuMsgBlockV2 table */

    /* begin of cpssDxChBrgFdbFuMsgBlock table */
    {"cpssDxChBrgFdbFuMsgBlockGetFirst",
        &wrCpssDxChBrgFdbFuMsgBlockV0GetFirst,
        2, 0},

    {"cpssDxChBrgFdbFuMsgBlockGetNext",
        &wrCpssDxChBrgFdbFuMsgBlockV0GetNext,
        2, 0},
    /* end of cpssDxChBrgFdbFuMsgBlock table */

    /* begin of cpssDxChBrgFdbFuMsgBlockV1 table */
    {"cpssDxChBrgFdbFuMsgBlockV1GetFirst",
        &wrCpssDxChBrgFdbFuMsgBlockV1GetFirst,
        2, 0},

    {"cpssDxChBrgFdbFuMsgBlockV1GetNext",
        &wrCpssDxChBrgFdbFuMsgBlockV1GetNext,
        2, 0},
    /* end of cpssDxChBrgFdbFuMsgBlockV1 table */

    /* begin of cpssDxChBrgFdbFuMsgBlockV2 table */
    {"cpssDxChBrgFdbFuMsgBlockV2GetFirst",
        &wrCpssDxChBrgFdbFuMsgBlockV2GetFirst,
        2, 0},

    {"cpssDxChBrgFdbFuMsgBlockV2GetNext",
        &wrCpssDxChBrgFdbFuMsgBlockV2GetNext,
        2, 0},
    /* end of cpssDxChBrgFdbFuMsgBlockV2 table */

    {"cpssDxChBrgFdbAuqFuqMessagesNumberGet",
        &wrCpssDxChBrgFdbAuqFuqMessagesNumberGet,
        2, 0},

    {"cpssDxChBrgFdbAgeBitDaRefreshEnableSet",
        &wrCpssDxChBrgFdbAgeBitDaRefreshEnableSet,
        2, 0},

    {"cpssDxChBrgFdbAgeBitDaRefreshEnableGet",
        &wrCpssDxChBrgFdbAgeBitDaRefreshEnableGet,
        1, 0},

    {"cpssDxChBrgFdbRoutedLearningEnableSet",
        &wrCpssDxChBrgFdbRoutedLearningEnableSet,
        2, 0},

    {"cpssDxChBrgFdbRoutedLearningEnableGet",
        &wrCpssDxChBrgFdbRoutedLearningEnableGet,
        1, 0},

    {"cpssDxChBrgFdbMacEntryAgeBitSet",
        &wrCpssDxChBrgFdbMacEntryAgeBitSet,
        3, 0},

    {"cpssDxChBrgFdbGlobalAgeBitEnableSet",
        &wrCpssDxChBrgFdbGlobalAgeBitEnableSet,
        1, 0},

    /* start table cpssDxChBrgFdbCount_debug1 */
    {"cpssDxChBrgFdbCount_debug1GetFirst",
        &wrCpssDxChBrgFdbCount_debug1,
        1,0},

    {"cpssDxChBrgFdbCount_debug1GetNext",
        &wrCpssDxChBrgFdbCount_debug1,
        1,0},
    /* end table cpssDxChBrgFdbCount_debug1 */

    {"cpssDxChBrgFdbHashCalc",
        &wrCpssDxChBrgFdbHashCalc,
        6, 0},

    {"cpssDxChBrgFdbHashCalcV2",
        &wrCpssDxChBrgFdbHashCalc,
        7, 0},
    
    {"cpssDxChBrgFdbMacEntryStatusGet",
        &wrCpssDxChBrgFdbMacEntryStatusGet,
        2, 0},

    {"cpssDxChBrgFdbQueueFullGet",
        &wrCpssDxChBrgFdbQueueFullGet,
        2, 0},

    {"cpssDxChBrgFdbQueueRewindStatusGet",
        &wrCpssDxChBrgFdbQueueRewindStatusGet,
        1, 0},

    {"cpssDxChBrgFdbSpAaMsgToCpuSet",
        &wrCpssDxChBrgFdbSpAaMsgToCpuSet,
        2, 0},

    {"cpssDxChBrgFdbSpAaMsgToCpuGet",
        &wrCpssDxChBrgFdbSpAaMsgToCpuGet,
        1, 0},

    /* begin of cpssDxChBrgFdbSecondaryAuMsgBlockGet table */
    {"cpssDxChBrgFdbSecondaryAuMsgBlockGetFirst",
        &wrCpssDxChBrgFdbSecondaryAuMsgBlockGetFirst,
        2, 0},

    {"cpssDxChBrgFdbSecondaryAuMsgBlockGetNext",
        &wrCpssDxChBrgFdbSecondaryAuMsgBlockGetNext,
        2, 0},
    /* end of cpssDxChBrgFdbSecondaryAuMsgBlockGet table */

    /* begin of cpssDxChBrgFdbSecondaryAuMsgBlockV2Get table */
    {"cpssDxChBrgFdbSecondaryAuMsgBlockGetV2First",
        &wrCpssDxChBrgFdbSecondaryAuMsgBlockV2GetFirst,
        2, 0},

    {"cpssDxChBrgFdbSecondaryAuMsgBlockV2GetNext",
        &wrCpssDxChBrgFdbSecondaryAuMsgBlockV2GetNext,
        2, 0},
    /* end of cpssDxChBrgFdbSecondaryAuMsgBlockV2Get table */

    {"cpssDxChBrgFdbTriggerAuQueueWa",
        &wrCpssDxChBrgFdbTriggerAuQueueWa,
        1, 0},

    {"cpssDxChBrgFdbNaMsgVid1EnableSet",
        &wrCpssDxChBrgFdbNaMsgVid1EnableSet,
        2, 0},

    {"cpssDxChBrgFdbNaMsgVid1EnableGet",
        &wrCpssDxChBrgFdbNaMsgVid1EnableGet,
        1, 0},

    {"cpssDxChBrgFdb16BitFidHashEnableSet",
        &wrCpssDxChBrgFdb16BitFidHashEnableSet,
        2, 0},

    {"cpssDxChBrgFdb16BitFidHashEnableGet",
        &wrCpssDxChBrgFdb16BitFidHashEnableGet,
        1, 0},

    {"cpssDxChBrgFdbMaxLengthSrcIdEnableSet",
        &wrCpssDxChBrgFdbMaxLengthSrcIdEnableSet,
        2, 0},

    {"cpssDxChBrgFdbMaxLengthSrcIdEnableGet",
        &wrCpssDxChBrgFdbMaxLengthSrcIdEnableGet,
        1, 0},

    {"cpssDxChBrgFdbVid1AssignmentEnableSet",
        &wrCpssDxChBrgFdbVid1AssignmentEnableSet,
        2, 0},

    {"cpssDxChBrgFdbVid1AssignmentEnableGet",
        &wrCpssDxChBrgFdbVid1AssignmentEnableGet,
        1, 0},

    {"cpssDxChBrgFdbSaLookupAnalyzerIndexSet",
        &wrCpssDxChBrgFdbSaLookupAnalyzerIndexSet,
        3, 0},

    {"cpssDxChBrgFdbSaLookupAnalyzerIndexGet",
        &wrCpssDxChBrgFdbSaLookupAnalyzerIndexGet,
        1, 0},

    {"cpssDxChBrgFdbDaLookupAnalyzerIndexSet",
        &wrCpssDxChBrgFdbDaLookupAnalyzerIndexSet,
        3, 0},

    {"cpssDxChBrgFdbDaLookupAnalyzerIndexGet",
        &wrCpssDxChBrgFdbDaLookupAnalyzerIndexGet,
        1, 0},

    {"cpssDxChBrgFdbActionActiveUserDefinedSet",
        &wrCpssDxChBrgFdbActionActiveUserDefinedSet,
        3, 0},

    {"cpssDxChBrgFdbActionActiveUserDefinedGet",
        &wrCpssDxChBrgFdbActionActiveUserDefinedGet,
        1, 0},

    {"cpssDxChBrgFdbLearnPrioritySet",
        &wrCpssDxChBrgFdbLearnPrioritySet,
        3, 0},

    {"cpssDxChBrgFdbLearnPriorityGet",
        &wrCpssDxChBrgFdbLearnPriorityGet,
        2, 0},

    {"cpssDxChBrgFdbUserGroupSet",
        &wrCpssDxChBrgFdbUserGroupSet,
        3, 0},

    {"cpssDxChBrgFdbUserGroupGet",
        &wrCpssDxChBrgFdbUserGroupGet,
        2, 0},

    {"cpssDxChBrgFdbCrcHashUpperBitsModeSet",
        &wrCpssDxChBrgFdbCrcHashUpperBitsModeSet,
        2, 0},

    {"cpssDxChBrgFdbCrcHashUpperBitsModeGet",
        &wrCpssDxChBrgFdbCrcHashUpperBitsModeGet,
        1, 0},

    {"cpssDxChBrgFdbBankCounterTableGetFirst",
        &wrCpssDxChBrgFdbBankCounterTableGetFirst,
        1, 0},
    {"cpssDxChBrgFdbBankCounterTableGetNext",
        &wrCpssDxChBrgFdbBankCounterTableGetNext,
        1, 0},

    {"cpssDxChBrgFdbBankCounterUpdateTableSet",
        &wrCpssDxChBrgFdbBankCounterUpdateTableSet,
        1, 2},

    {"cpssDxChBrgFdbBankCounterUpdateStatusGet",
        &wrCpssDxChBrgFdbBankCounterUpdateStatusGet,
        1, 0},

    {"cpssDxChBrgFdbMacEntryMove",
        &wrCpssDxChBrgFdbMacEntryMove,
        2, 0},

    {"cpssDxChBrgFdbHashCrcMultiResultsCalc",
        &wrCpssDxChBrgFdbHashCrcMultiResultsCalc,
        8, 0},

    {"cpssDxChBrgFdbHashCrcMultiResultsCalcV2",
        &wrCpssDxChBrgFdbHashCrcMultiResultsCalc,
        9, 0},
    
    {"cpssDxChBrgFdbRoutingHashCrcMultiResultsCalc",
        &wrCpssDxChBrgFdbRoutingHashCrcMultiResultsCalc,
        8, 0},

    {"cpssDxChBrgFdbHashRequestSend",
        &wrCpssDxChBrgFdbHashRequestSend,
        6, 0},

    {"cpssDxChBrgFdbHashRequestSendV2",
        &wrCpssDxChBrgFdbHashRequestSend,
        7, 0},

    {"cpssDxChBrgFdbHashResultsGet",
        &wrCpssDxChBrgFdbHashResultsGet,
        3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibResetCpssDxChBrgFdb
*
* DESCRIPTION:
*     Library database reset function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID cmdLibResetCpssDxChBrgFdb
(
    GT_VOID
)
{
    /* Flag for age bit setting.
       GT_TRUE  - set to default value
       GT_FALSE - set age bit in FDB entry according to given value */
    setAgeToDefault = GT_TRUE;

    /* support for multi port groups */
    multiPortGroupsBmpEnable = GT_FALSE;

    /* current port group for the 'refresh' FDB */
    useRefreshFdbPerPortGroupId = GT_FALSE;
    /* current port group for the 'refresh' FDB */
    refreshFdbCurrentPortGroupId = 0;
    /* first port group for the 'refresh' FDB */
    refreshFdbFirstPortGroupId = 0;

    actNumOfAu = 0;

    CMD_OS_FREE_AND_SET_NULL_MAC(wrFuArrayPtr);
    CMD_OS_FREE_AND_SET_NULL_MAC(auMessagesPtr);

    /* current index in FU array */
    currentFuIndex = 0;

    calledFromGaltis_count_debug = GT_FALSE;
    lastPortGroupId_count_debug = 0;

}

/*******************************************************************************
* cmdLibInitCpssDxChBridgeFdb
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
GT_STATUS cmdLibInitCpssDxChBridgeFdb
(
    GT_VOID
)
{
    GT_STATUS rc;

    rc = wrCpssRegisterResetCb(cmdLibResetCpssDxChBrgFdb);
    if(rc != GT_OK)
    {
        return rc;
    }

    return cmdInitLibrary(dbCommands, numCommands);
}



