/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChCnc.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/
/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cnc/cpssDxChCnc.h>

#define CH3_WR_INDEXES_SIZE 8

static CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  last_format =
    CPSS_DXCH_CNC_COUNTER_FORMAT_MODE_0_E;

/* support for multi port groups */

/*******************************************************************************
* cncMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
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
static void cncMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncBlockClientEnableSet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    IN  GT_BOOL                   updateEnable
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    #ifdef GM_USED
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        if(client == CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E ||
           client == CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E)
        {
            /* CNC events for TXQ clients are not generated in GM */
            return GT_NOT_SUPPORTED;
        }
    }
    #endif
    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncBlockClientEnableSet(
            devNum, blockNum, client, updateEnable);
    }
    else
    {
        return cpssDxChCncPortGroupBlockClientEnableSet(
            devNum, pgBmp, blockNum, client, updateEnable);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncBlockClientEnableGet
(
    IN  GT_U8                     devNum,
    IN  GT_U32                    blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT  client,
    OUT GT_BOOL                   *updateEnablePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    #ifdef GM_USED
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        if(client == CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E ||
           client == CPSS_DXCH_CNC_CLIENT_EGRESS_QUEUE_PASS_DROP_E)
        {
            /* CNC events for TXQ clients are not generated in GM */
            return GT_NOT_SUPPORTED;
        }
    }
    #endif

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncBlockClientEnableGet(
            devNum, blockNum, client, updateEnablePtr);
    }
    else
    {
        return cpssDxChCncPortGroupBlockClientEnableGet(
            devNum, pgBmp, blockNum, client, updateEnablePtr);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncBlockClientRangesSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    IN  GT_U64                      *indexRangesBmpPtr /* use pointer to avoid ARM DIAB compiler problems */
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */
    GT_U64              indexRangesBmp;

    indexRangesBmp.l[0] = indexRangesBmpPtr->l[0];
    indexRangesBmp.l[1] = indexRangesBmpPtr->l[1];

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncBlockClientRangesSet(
            devNum, blockNum, client, indexRangesBmp);
    }
    else
    {
        return cpssDxChCncPortGroupBlockClientRangesSet(
            devNum, pgBmp, blockNum, client, indexRangesBmp);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncBlockClientRangesGet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum,
    IN  CPSS_DXCH_CNC_CLIENT_ENT    client,
    OUT GT_U64                      *indexRangesBmpPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncBlockClientRangesGet(
            devNum, blockNum, client, indexRangesBmpPtr);
    }
    else
    {
        return cpssDxChCncPortGroupBlockClientRangesGet(
            devNum, pgBmp, blockNum, client, indexRangesBmpPtr);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncCounterWraparoundIndexesGet
(
    IN    GT_U8    devNum,
    IN    GT_U32   blockNum,
    INOUT GT_U32   *indexNumPtr,
    OUT   GT_U32   indexesArr[]
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncCounterWraparoundIndexesGet(
            devNum, blockNum, indexNumPtr, indexesArr);
    }
    else
    {
        return cpssDxChCncPortGroupCounterWraparoundIndexesGet(
            devNum, pgBmp, blockNum, indexNumPtr,
            NULL /*portGroupIdArr*/, indexesArr);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncCounterSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    IN  CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncCounterSet(
            devNum, blockNum, index, format, counterPtr);
    }
    else
    {
        return cpssDxChCncPortGroupCounterSet(
            devNum, pgBmp, blockNum, index, format, counterPtr);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncCounterGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  GT_U32                            index,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT CPSS_DXCH_CNC_COUNTER_STC         *counterPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncCounterGet(
            devNum, blockNum, index, format, counterPtr);
    }
    else
    {
        return cpssDxChCncPortGroupCounterGet(
            devNum, pgBmp, blockNum, index, format, counterPtr);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncBlockUploadTrigger
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      blockNum
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncBlockUploadTrigger(
            devNum, blockNum);
    }
    else
    {
        return cpssDxChCncPortGroupBlockUploadTrigger(
            devNum, pgBmp, blockNum);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncBlockUploadInProcessGet
(
    IN  GT_U8                       devNum,
    OUT GT_U32                     *inProcessBlocksBmpPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncBlockUploadInProcessGet(
            devNum, inProcessBlocksBmpPtr);
    }
    else
    {
        return cpssDxChCncPortGroupBlockUploadInProcessGet(
            devNum, pgBmp, inProcessBlocksBmpPtr);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncUploadedBlockGet
(
    IN     GT_U8                             devNum,
    INOUT  GT_U32                            *numOfCounterValuesPtr,
    IN     CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format,
    OUT    CPSS_DXCH_CNC_COUNTER_STC         *counterValuesPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChCncUploadedBlockGet(
            devNum, numOfCounterValuesPtr, format, counterValuesPtr);
    }
    else
    {
        return cpssDxChCncPortGroupUploadedBlockGet(
            devNum, pgBmp, numOfCounterValuesPtr, format, counterValuesPtr);
    }
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncCounterFormatSet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    IN  CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        pgBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    /* save format for other commands */
    last_format = format;

    return cpssDxChCncPortGroupCounterFormatSet(
        devNum, pgBmp, blockNum, format);
}

/* Port Group and Regular version wrapper      */
/* description see in original function header */
GT_STATUS pg_wrap_cpssDxChCncCounterFormatGet
(
    IN  GT_U8                             devNum,
    IN  GT_U32                            blockNum,
    OUT CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  *formatPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    cncMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        pgBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    }

    return cpssDxChCncPortGroupCounterFormatGet(
        devNum, pgBmp, blockNum, formatPtr);
}

/*******************************************************************************
* cpssDxChCncBlockClientEnableSet
*
* DESCRIPTION:
*   The function binds/unbinds the selected client to/from a counter block.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum         - device number
*       blockNum       - CNC block number 0..11
*       client         - CNC client
*       updateEnable   - the client enable to update the block
*                        GT_TRUE - enable, GT_FALSE - disable
*                        It is forbidden to enable update the same
*                        block by more then one client. When an
*                        application enables some client it is responsible
*                        to disable all other clients it enabled before
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockClientEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_BOOL updateEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];
    updateEnable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockClientEnableSet(
        devNum, blockNum, client, updateEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncBlockClientEnableGet
*
* DESCRIPTION:
*   The function gets bind/unbind of the selected client to a counter block.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number 0..11
*       client         -  CNC client
* OUTPUTS:
*       updateEnablePtr - (pointer to) the client enable to update the block
*                         GT_TRUE - enable, GT_FALSE - disable
*                         It is forbidden to enable update the same
*                         block by more then one client. When an
*                         application enables some client it is responsible
*                         to disable all other clients it enabled before
*
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockClientEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_BOOL updateEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockClientEnableGet(
        devNum, blockNum, client, &updateEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", updateEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesSet
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       client           - CNC client
*       indexRangesBmp   - there are 8 ranges of index counters, each
*                         2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - on not relevant bits in indexRangesBmp
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockClientRangesSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_U64 indexRangesBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];
    indexRangesBmp.l[0] = (GT_U32)inArgs[3];
    indexRangesBmp.l[1] = 0;

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockClientRangesSet(
        devNum, blockNum, client, &indexRangesBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesGet
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap
*                         there are 8 ranges of index counters, each
*                         2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Allowded to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockClientRangesGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_U64 indexRangesBmp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockClientRangesGet(
        devNum, blockNum, client, &indexRangesBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", indexRangesBmp.l[0]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesSet64
*
* DESCRIPTION:
*   The function sets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       client           - CNC client
*       indexRangesBmp   - there are 8 ranges of index counters, each
*                         2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Allowed to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong parameters
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - on not relevant bits in indexRangesBmp
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockClientRangesSet64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_U64 indexRangesBmp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];
    indexRangesBmp.l[0] = (GT_U32)inArgs[3];
    indexRangesBmp.l[1] = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockClientRangesSet(
        devNum, blockNum, client, &indexRangesBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncBlockClientRangesGet64
*
* DESCRIPTION:
*   The function gets index ranges per CNC client and Block
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       client           - CNC client
*
* OUTPUTS:
*       indexRangesBmpPtr - (pointer to) bitmap
*                         there are 8 ranges of index counters, each
*                         2048 indexes (0..(2K-1), 2K..(4K-1), ..., 14K..(16K-1))
*                         the n-th bit 1 value maps the n-th index range
*                         to the block (2048 counters)
*                         Allowded to map more then one range to the block
*                         but it will cause updating the same counters via
*                         different indexes.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockClientRangesGet64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_U64 indexRangesBmp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockClientRangesGet(
        devNum, blockNum, client, &indexRangesBmp);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d",
        indexRangesBmp.l[0], indexRangesBmp.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncPortClientEnableSet
*
* DESCRIPTION:
*   The function sets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncPortClientEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCncPortClientEnableSet(
        devNum, portNum, client, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncPortClientEnableGet
*
* DESCRIPTION:
*   The function gets the given client Enable counting per port.
*   Currently only L2/L3 Ingress Vlan client supported.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       portNum          - port number
*       client           - CNC client
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncPortClientEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCncPortClientEnableGet(devNum, portNum, client, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
*
* DESCRIPTION:
*   The function enables or disables counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       enable           - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncIngressVlanPassDropFromCpuCountEnableSet
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
    result = cpssDxChCncIngressVlanPassDropFromCpuCountEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
*
* DESCRIPTION:
*   The function gets status of counting of FROM_CPU packets
*   for the Ingress Vlan Pass/Drop CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       enablePtr        - (pointer to) enable
*                          GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncIngressVlanPassDropFromCpuCountEnableGet
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
    result = cpssDxChCncIngressVlanPassDropFromCpuCountEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncVlanClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncVlanClientIndexModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    CPSS_DXCH_CNC_CLIENT_ENT          vlanClient;
    CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT indexMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    vlanClient = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[1];
    indexMode  = (CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCncVlanClientIndexModeSet(
        devNum, vlanClient, indexMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncVlanClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Vlan CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       vlanClient         - VLAN client
*                            relevant clients:
*                            CPSS_DXCH_CNC_CLIENT_INGRESS_VLAN_PASS_DROP_E
*                            CPSS_DXCH_CNC_CLIENT_EGRESS_VLAN_PASS_DROP_E
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_STATE             - enexpected HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncVlanClientIndexModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    CPSS_DXCH_CNC_CLIENT_ENT          vlanClient;
    CPSS_DXCH_CNC_VLAN_INDEX_MODE_ENT indexMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    vlanClient = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncVlanClientIndexModeGet(
        devNum, vlanClient, &indexMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", indexMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncPacketTypePassDropToCpuModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2 DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*       toCpuMode        - to CPU packets index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncPacketTypePassDropToCpuModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                           result;
    GT_U8                                               devNum;
    CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT toCpuMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    toCpuMode = (CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncPacketTypePassDropToCpuModeSet(
        devNum, toCpuMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncPacketTypePassDropToCpuModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode of To CPU packets
*   for the Packet Type Pass/Drop CNC client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2 DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       toCpuModePtr        - to CPU packets index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncPacketTypePassDropToCpuModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                           result;
    GT_U8                                               devNum;
    CPSS_DXCH_CNC_PACKET_TYPE_PASS_DROP_TO_CPU_MODE_ENT toCpuMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCncPacketTypePassDropToCpuModeGet(
        devNum, &toCpuMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", toCpuMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncTmClientIndexModeSet
*
* DESCRIPTION:
*   The function sets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2 DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       indexMode          - index calculation mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncTmClientIndexModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_DXCH_CNC_TM_INDEX_MODE_ENT indexMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    indexMode = (CPSS_DXCH_CNC_TM_INDEX_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncTmClientIndexModeSet(
        devNum, indexMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncTmClientIndexModeGet
*
* DESCRIPTION:
*   The function gets index calculation mode
*   for the Traffic manager CNC clients.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2 DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       indexModePtr       - (pointer to)index calculation mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncTmClientIndexModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_DXCH_CNC_TM_INDEX_MODE_ENT indexMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCncTmClientIndexModeGet(
        devNum, &indexMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", indexMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncClientByteCountModeSet
*
* DESCRIPTION:
*   The function sets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*       countMode        - count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncClientByteCountModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[1];
    countMode = (CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCncClientByteCountModeSet(devNum, client, countMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncClientByteCountModeGet
*
* DESCRIPTION:
*   The function gets byte count counter mode for CNC client.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       client           - CNC client
*
* OUTPUTS:
*       countModePtr     - (pointer to) count mode
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncClientByteCountModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_CLIENT_ENT client;
    CPSS_DXCH_CNC_BYTE_COUNT_MODE_ENT countMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    client = (CPSS_DXCH_CNC_CLIENT_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncClientByteCountModeGet(devNum, client, &countMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", countMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeSet
*
* DESCRIPTION:
*   The function sets Egress VLAN Drop counting mode.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       mode             - Egress VLAN Drop counting mode
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
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncEgressVlanDropCountModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncEgressVlanDropCountModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncEgressVlanDropCountModeGet
*
* DESCRIPTION:
*   The function gets tEgress VLAN Drop counting mode.
*
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       modePtr             - (pointer to) Egress VLAN Drop counting mode
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncEgressVlanDropCountModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_EGRESS_DROP_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCncEgressVlanDropCountModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableSet
*
* DESCRIPTION:
*   The function enable/disables clear by read mode of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
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
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterClearByReadEnableSet
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
    result = cpssDxChCncCounterClearByReadEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadEnableGet
*
* DESCRIPTION:
*   The function gets clear by read mode status of CNC counters read
*   operation.
*   If clear by read mode is disable the counters after read
*   keep the current value and continue to count normally.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterClearByReadEnableGet
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
    result = cpssDxChCncCounterClearByReadEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueSet
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterClearByReadValueSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_COUNTER_STC counter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    counter.byteCount.l[0]=(GT_U32)inArgs[1];
    counter.byteCount.l[1]=(GT_U32)inArgs[2];
    counter.packetCount.l[0]=(GT_U32)inArgs[3];
    counter.packetCount.l[1]=0;

    /* call cpss api function */
    result = cpssDxChCncCounterClearByReadValueSet(
        devNum, last_format, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueGet
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterClearByReadValueGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_COUNTER_STC counter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCncCounterClearByReadValueGet(
        devNum, last_format, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d",
        counter.byteCount.l[0],counter.byteCount.l[1],
        counter.packetCount.l[0]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueSet64
*
* DESCRIPTION:
*   The function sets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterClearByReadValueSet64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;
    CPSS_DXCH_CNC_COUNTER_STC         counter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    format = (CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)inArgs[1];
    counter.byteCount.l[0]=(GT_U32)inArgs[2];
    counter.byteCount.l[1]=(GT_U32)inArgs[3];
    counter.packetCount.l[0]=(GT_U32)inArgs[4];
    counter.packetCount.l[1]=(GT_U32)inArgs[5];

    last_format = format;

    /* call cpss api function */
    result = cpssDxChCncCounterClearByReadValueSet(
        devNum, format, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterClearByReadValueGet64
*
* DESCRIPTION:
*   The function gets the counter clear by read globally configured value.
*   If clear by read mode is enable the counters load a globally configured
*   value instead of the current value and continue to count normally.
*
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterClearByReadValueGet64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;
    CPSS_DXCH_CNC_COUNTER_STC         counter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

     devNum = (GT_U8)inArgs[0];
     format = (CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)inArgs[1];

     last_format = format;

    /* call cpss api function */
    result = cpssDxChCncCounterClearByReadValueGet(
        devNum, format, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        counter.byteCount.l[0],counter.byteCount.l[1],
        counter.packetCount.l[0], counter.packetCount.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableSet
*
* DESCRIPTION:
*   The function enables/disables wraparound for all CNC counters
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*       enable       - GT_TRUE - enable, GT_FALSE - disable
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
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterWraparoundEnableSet
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
    result = cpssDxChCncCounterWraparoundEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterWraparoundEnableGet
*
* DESCRIPTION:
*   The function gets status of wraparound for all CNC counters
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       enablePtr    - (pointer to) enable
*                      GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterWraparoundEnableGet
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
    result = cpssDxChCncCounterWraparoundEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/***********Table(Refreash only):cpssDxChCncCounterWraparoundIndexes************/

static GT_U32  counterWraparoundIndexesArr[CH3_WR_INDEXES_SIZE];
static  GT_U32 counterWraparoundIndexNum;
static  GT_U32 counterWraparoundIndex;
/*******************************************************************************
* cpssDxChCncCounterWraparoundIndexesGet
*
* DESCRIPTION:
*   The function gets the counter Wrap Around last 8 indexes
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*       blockNum     - number of 2K-counters block
*       indexNumPtr  - (pointer to) maximal size of array of indexes
*
* OUTPUTS:
*       indexNumPtr  - (pointer to) actual size of array of indexes
*       indexesArr   - (pointer to) array of indexes of counters wrapped around
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*       Read and Clear data
*
* WRAPPER COMMENTS:
*       In wrapper indexNumPtr(actual size of array of indexes) outputed to table
*       in the first field.Number of entries that in the table after refreash is
*       is indexNumPtr.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterWraparoundIndexesGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    blockNum= (GT_U8)inArgs[1];
    counterWraparoundIndexNum=(GT_U8)inArgs[2];

    /* number of wraparound indexes signaled is limited to 8 - as the array size */
    if(counterWraparoundIndexNum > CH3_WR_INDEXES_SIZE)
    {
        return CMD_ARG_OVERFLOW;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncCounterWraparoundIndexesGet(
        devNum, blockNum, &counterWraparoundIndexNum, counterWraparoundIndexesArr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if (counterWraparoundIndexNum == 0)
    {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
    }
    counterWraparoundIndex=0;

    inFields[0]=counterWraparoundIndex;
    inFields[1]=counterWraparoundIndexesArr[counterWraparoundIndex];



    /* pack output arguments to galtis string */
    fieldOutput("%d%d", inFields[0],inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*****************************************************************************************************/

static CMD_STATUS wrCpssDxChCncCounterWraparoundIndexesGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    counterWraparoundIndex++;

    if(counterWraparoundIndex > counterWraparoundIndexNum-1)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    inFields[0]=counterWraparoundIndex;
    inFields[1]=counterWraparoundIndexesArr[counterWraparoundIndex];



    /* pack output arguments to galtis string */
   fieldOutput("%d%d", inFields[0],inFields[1]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterSet
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       index            - counter index in the block 0..2047
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    GT_U32 index;
    CPSS_DXCH_CNC_COUNTER_STC counter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum=(GT_U32)inArgs[1];
    index=(GT_U32)inArgs[2];
    counter.byteCount.l[0]=(GT_U32)inArgs[3];
    counter.byteCount.l[1]=(GT_U32)inArgs[4];
    counter.packetCount.l[0]=(GT_U32)inArgs[5];
    counter.packetCount.l[1]=0;

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncCounterSet(
        devNum, blockNum, index,
        last_format, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterGet
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       index            - counter index in the block 0..2047
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;
    GT_U32 index;
    CPSS_DXCH_CNC_COUNTER_STC counter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum=(GT_U32)inArgs[1];
    index=(GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncCounterGet(
        devNum, blockNum, index,
        last_format, &counter);

    /* pack output arguments to galtis string */
     galtisOutput(
         outArgs, result, "%d%d%d",
         counter.byteCount.l[0],counter.byteCount.l[1],
         counter.packetCount.l[0]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterSet64
*
* DESCRIPTION:
*   The function sets the counter contents
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       index            - counter index in the block 0..2047
*       counterPtr       - (pointer to) counter contents
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterSet64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    GT_U32                            blockNum;
    GT_U32                            index;
    CPSS_DXCH_CNC_COUNTER_STC         counter;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum   = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    index    = (GT_U32)inArgs[2];
    format   = (CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)inArgs[3];
    counter.byteCount.l[0]=(GT_U32)inArgs[4];
    counter.byteCount.l[1]=(GT_U32)inArgs[5];
    counter.packetCount.l[0]=(GT_U32)inArgs[6];
    counter.packetCount.l[1]=(GT_U32)inArgs[7];

    last_format = format;

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncCounterSet(
        devNum, blockNum, index,
        format, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterGet64
*
* DESCRIPTION:
*   The function gets the counter contents
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*       index            - counter index in the block 0..2047
*
* OUTPUTS:
*       counterPtr       - (pointer to) counter contents
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterGet64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    GT_U32                            blockNum;
    GT_U32                            index;
    CPSS_DXCH_CNC_COUNTER_STC         counter;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum   = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];
    index    = (GT_U32)inArgs[2];
    format   = (CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)inArgs[3];

    last_format = format;

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncCounterGet(
        devNum, blockNum, index,
        last_format, &counter);

    /* pack output arguments to galtis string */
     galtisOutput(
         outArgs, result, "%d%d%d%d",
         counter.byteCount.l[0],counter.byteCount.l[1],
         counter.packetCount.l[0], counter.packetCount.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncBlockUploadTrigger
*
* DESCRIPTION:
*   The function triggers the Upload of the given counters block.
*   The function checks that there is no unfinished CNC and FDB upload (FU).
*   Also the function checks that all FU messages of previous FU were retrieved
*   by cpssDxChBrgFdbFuMsgBlockGet.
*   An application may check that CNC upload finished by
*   cpssDxChCncBlockUploadInProcessGet.
*   An application may sequentially upload several CNC blocks before start
*   to retrieve uploaded counters.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*       blockNum         - CNC block number 0..11
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_STATE   - if the previous CNC upload or FU in process or
*                        FU is finished but all FU messages were not
*                        retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockUploadTrigger
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 blockNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    blockNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockUploadTrigger(devNum, blockNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncBlockUploadInProcessGet
*
* DESCRIPTION:
*   The function gets bitmap of numbers of such counters blocks that upload
*   of them yet in process. The HW cannot keep more then one block in such
*   state, but an API matches the original HW representation of the state.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum           - device number
*
* OUTPUTS:
*       inProcessBlocksBmpPtr     - (pointer to) bitmap of in-process blocks
*                                   value 1 of the bit#n means that CNC upload
*                                   is not finished yet for block n
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*       GT_BAD_PTR     - on null pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncBlockUploadInProcessGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 inProcessBlocksBmp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncBlockUploadInProcessGet(
        devNum, &inProcessBlocksBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", inProcessBlocksBmp);

    return CMD_OK;
}

/***********Table(Refreash only):cpssDxChCncUploadedBlock************/
#define WRR_CNC_UPLOAD_MAX_INDEX_MAC        2048
static  CPSS_DXCH_CNC_COUNTER_STC  uploadedBlockIndexesArr[WRR_CNC_UPLOAD_MAX_INDEX_MAC];
static  GT_U32                     numOfCounterValues;
static  GT_U32                     uploadedBlockIndex;
/*******************************************************************************
* cpssDxChCncUploadedBlockGet
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transfered counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
*  APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL          - on failure
*       GT_BAD_PARAM     - on wrong devNum
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_BAD_STATE     - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
* WRAPPER COMMENTS:
*       In wrapper numOfCounterValuesPtr( actual number of CNC counters values) outputed
*       to table in the first field.Number of entries that in the table after
*       refreash is numOfCounterValuesPtr.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncUploadedBlockGetFirst
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

    devNum =(GT_U8)inArgs[0];
    numOfCounterValues=(GT_U32)inArgs[1];

    /* number of requested indexes greater then array size */
    if(numOfCounterValues > WRR_CNC_UPLOAD_MAX_INDEX_MAC)
    {
        return CMD_ARG_OVERFLOW;
    }
    /* call cpss api function */
    result = pg_wrap_cpssDxChCncUploadedBlockGet(
        devNum, &numOfCounterValues,
        last_format, uploadedBlockIndexesArr);

    if ( ((result != GT_OK) && (result != GT_NO_MORE)) ||
         (numOfCounterValues == 0) )
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
    }

    uploadedBlockIndex=0;

    inFields[0]=uploadedBlockIndex;
    inFields[1]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[0];
    inFields[2]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[1];
    inFields[3]=uploadedBlockIndexesArr[uploadedBlockIndex].packetCount.l[0];


     /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d", inFields[0],inFields[1],inFields[2],inFields[3]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*********************************************************************/
static CMD_STATUS wrCpssDxChCncUploadedBlockGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    uploadedBlockIndex++;

    if(uploadedBlockIndex > numOfCounterValues-1)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    inFields[0]=uploadedBlockIndex;
    inFields[1]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[0];
    inFields[2]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[1];
    inFields[3]=uploadedBlockIndexesArr[uploadedBlockIndex].packetCount.l[0];



    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d", inFields[0],inFields[1],inFields[2],inFields[3]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncUploadedBlockGet64
*
* DESCRIPTION:
*       The function return a block (array) of CNC counter values,
*       the maximal number of elements defined by the caller.
*       The CNC upload may triggered by cpssDxChCncBlockUploadTrigger.
*       The CNC upload transfers whole CNC block (2K CNC counters)
*       to FDB Upload queue. An application must get all transfered counters.
*       An application may sequentially upload several CNC blocks before start
*       to retrieve uploaded counters.
*       The device may transfer only part of CNC block because of FUQ full. In
*       this case the cpssDxChCncUploadedBlockGet may return only part of the
*       CNC block with return GT_OK. An application must to call
*       cpssDxChCncUploadedBlockGet one more time to get rest of the block.
*
*  APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum                - the device number from which FU are taken
*       numOfCounterValuesPtr - (pointer to) maximal number of CNC counters
*                               values to get.This is the size of
*                               counterValuesPtr array allocated by caller.
*
* OUTPUTS:
*       numOfCounterValuesPtr - (pointer to) actual number of CNC
*                               counters values in counterValuesPtr.
*       counterValuesPtr      - (pointer to) array that holds received CNC
*                               counters values. Array must be allocated by
*                               caller.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NO_MORE       - the action succeeded and there are no more waiting
*                          CNC counter value
*       GT_FAIL          - on failure
*       GT_BAD_PARAM     - on wrong devNum
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_BAD_STATE     - if the previous FU messages were not
*                          retrieved yet by cpssDxChBrgFdbFuMsgBlockGet.
*
* COMMENTS:
*    The device use same resource the FDB upload queue (FUQ) for both CNC and
*    FDB uploads. The function cpssDxChHwPpPhase2Init configures parameters for
*    FUQ. The fuqUseSeparate (ppPhase2ParamsPtr) parameter must be set GT_TRUE
*    to enable CNC upload.
*    There are limitations for FDB and CNC uploads if an application use
*    both of them:
*    1. After triggering of FU and before start of CNC upload an application
*       must retrieve all FU messages from FUQ by cpssDxChBrgFdbFuMsgBlockGet.
*    2. After start of CNC upload and before triggering of FU an application
*       must retrieve all CNC messages from FUQ by cpssDxChCncUploadedBlockGet.
* WRAPPER COMMENTS:
*       In wrapper numOfCounterValuesPtr( actual number of CNC counters values) outputed
*       to table in the first field.Number of entries that in the table after
*       refreash is numOfCounterValuesPtr.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncUploadedBlockGetFirst64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    numOfCounterValues=(GT_U32)inArgs[1];
    format = (CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)inArgs[2];

    last_format = format;

    /* number of requested indexes greater then array size */
    if(numOfCounterValues > WRR_CNC_UPLOAD_MAX_INDEX_MAC)
    {
        return CMD_ARG_OVERFLOW;
    }
    /* call cpss api function */
    result = pg_wrap_cpssDxChCncUploadedBlockGet(
        devNum, &numOfCounterValues,
        format, uploadedBlockIndexesArr);

    if ( ((result != GT_OK) && (result != GT_NO_MORE)) ||
         (numOfCounterValues == 0) )
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
    }

    uploadedBlockIndex=0;

    inFields[0]=uploadedBlockIndex;
    inFields[1]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[0];
    inFields[2]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[1];
    inFields[3]=uploadedBlockIndexesArr[uploadedBlockIndex].packetCount.l[0];
    inFields[4]=uploadedBlockIndexesArr[uploadedBlockIndex].packetCount.l[1];


     /* pack output arguments to galtis string */
    fieldOutput(
        "%d%d%d%d%d",
        inFields[0],inFields[1],inFields[2],inFields[3],inFields[4]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*********************************************************************/
static CMD_STATUS wrCpssDxChCncUploadedBlockGetNext64
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    uploadedBlockIndex++;

    if (uploadedBlockIndex > numOfCounterValues-1)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    inFields[0]=uploadedBlockIndex;
    inFields[1]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[0];
    inFields[2]=uploadedBlockIndexesArr[uploadedBlockIndex].byteCount.l[1];
    inFields[3]=uploadedBlockIndexesArr[uploadedBlockIndex].packetCount.l[0];
    inFields[4]=uploadedBlockIndexesArr[uploadedBlockIndex].packetCount.l[1];



    /* pack output arguments to galtis string */
    fieldOutput(
        "%d%d%d%d%d",
        inFields[0],inFields[1],inFields[2],inFields[3],inFields[4]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCountingEnableSet
*
* DESCRIPTION:
*   The function enables counting on selected cnc unit.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*       enable  - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCountingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cncUnit = (CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCncCountingEnableSet(devNum, cncUnit, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCountingEnableGet
*
* DESCRIPTION:
*   The function gets enable counting status on selected cnc unit.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum  - device number
*       cncUnit - selected unit for enable\disable counting
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable
*                     GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCountingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT  cncUnit;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cncUnit = (CPSS_DXCH_CNC_COUNTING_ENABLE_UNIT_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncCountingEnableGet(devNum, cncUnit, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeSet
*
* DESCRIPTION:
*   The function sets Egress Queue client counting mode
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       mode            - Egress Queue client counting mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncEgressQueueClientModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT  mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncEgressQueueClientModeSet(
        devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncEgressQueueClientModeGet
*
* DESCRIPTION:
*   The function gets Egress Queue client counting mode
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       modePtr         - (pointer to) Egress Queue client counting mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncEgressQueueClientModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_CNC_EGRESS_QUEUE_CLIENT_MODE_ENT  mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCncEgressQueueClientModeGet(
        devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableSet
*
* DESCRIPTION:
*   The function enables strict priority of CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       enable          - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCpuAccessStrictPriorityEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncCpuAccessStrictPriorityEnableSet(
        devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCpuAccessStrictPriorityEnableGet
*
* DESCRIPTION:
*   The function gets enable status of strict priority of
*   CPU access to counter blocks
*   Disable: Others clients have strict priority
*   Enable:  CPU has strict priority
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       enablePtr       - (pointer to) GT_TRUE - enable, GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCpuAccessStrictPriorityEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCncCpuAccessStrictPriorityEnableGet(
        devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncCounterFormatSet
*
* DESCRIPTION:
*   The function sets format of CNC counter
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       blockNum        - CNC block number
*                         valid range:
*                         DxCh3, Lion - 0..7, DxChXCat - 0,1
*       format          - CNC counter format
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterFormatSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    GT_U32                            block;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    block   = (GT_U32)inArgs[1];
    format  = (CPSS_DXCH_CNC_COUNTER_FORMAT_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncCounterFormatSet(
        devNum, block, format);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncPortGroupCounterFormatGet
*
* DESCRIPTION:
*   The function gets format of CNC counter
*
* APPLICABLE DEVICES:  Lion and above.
*
* INPUTS:
*       devNum          - device number
*       portGroupsBmp   - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       blockNum        - CNC block number
*                         valid range:
*                         DxCh3, Lion - 0..7, DxChXCat - 0,1
*
* OUTPUTS:
*       formatPtr       - (pointer to) CNC counter format
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*                                  on not supported client for device.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncCounterFormatGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         result;
    GT_U8                             devNum;
    GT_U32                            block;
    CPSS_DXCH_CNC_COUNTER_FORMAT_ENT  format;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    block   = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChCncCounterFormatGet(
        devNum, block, &format);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", format);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncOffsetForNatClientSet
*
* DESCRIPTION:
*      Set the CNC offset for NAT client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*       offset      - CNC offset for NAT client
*                     (APPLICABLE RANGES: 0..65535)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The CNC clients ARP and NAT are muxed between them (since for a
*       given packet the user can access ARP entry or NAT entry).
*       This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncOffsetForNatClientSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    offset = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCncOffsetForNatClientSet(devNum, offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCncOffsetForNatClientGet
*
* DESCRIPTION:
*      Get the CNC offset for NAT client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       offsetPtr   - (pointer to) CNC offset for NAT client
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      The CNC clients ARP and NAT are muxed between them (since for a
*      given packet the user can access ARP entry or NAT entry).
*      This offset is added to the NAT when sending the pointer to the CNC.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCncOffsetForNatClientGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCncOffsetForNatClientGet(devNum, &offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", offset);
    return CMD_OK;
}


/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssDxChCncBlockClientEnableSet",
         &wrCpssDxChCncBlockClientEnableSet,
         4, 0},
        {"cpssDxChCncBlockClientEnableGet",
         &wrCpssDxChCncBlockClientEnableGet,
         3, 0},
        {"cpssDxChCncBlockClientRangesSet",
         &wrCpssDxChCncBlockClientRangesSet,
         4, 0},
        {"cpssDxChCncBlockClientRangesGet",
         &wrCpssDxChCncBlockClientRangesGet,
         3, 0},
        {"cpssDxChCncBlockClientRangesSet64",
         &wrCpssDxChCncBlockClientRangesSet64,
         5, 0},
        {"cpssDxChCncBlockClientRangesGet64",
         &wrCpssDxChCncBlockClientRangesGet64,
         3, 0},
        {"cpssDxChCncPortClientEnableSet",
         &wrCpssDxChCncPortClientEnableSet,
         4, 0},
        {"cpssDxChCncPortClientEnableGet",
         &wrCpssDxChCncPortClientEnableGet,
         3, 0},
        {"cpssDxChCncIngressVlanPassDropFromCpuCntEnableSet",
         &wrCpssDxChCncIngressVlanPassDropFromCpuCountEnableSet,
         2, 0},
        {"cpssDxChCncIngressVlanPassDropFromCpuCntEnableGet",
         &wrCpssDxChCncIngressVlanPassDropFromCpuCountEnableGet,
         1, 0},
        {"cpssDxChCncClientByteCountModeSet",
         &wrCpssDxChCncClientByteCountModeSet,
         3, 0},
        {"cpssDxChCncClientByteCountModeGet",
         &wrCpssDxChCncClientByteCountModeGet,
         2, 0},
        {"cpssDxChCncVlanCncClientIndexModeSet",
         &wrCpssDxChCncVlanClientIndexModeSet,
         3, 0},
        {"cpssDxChCncVlanCncClientIndexModeGet",
         &wrCpssDxChCncVlanClientIndexModeGet,
         2, 0},
        {"cpssDxChCncPacketTypePassDropToCpuModeSet",
         &wrCpssDxChCncPacketTypePassDropToCpuModeSet,
         2, 0},
        {"cpssDxChCncPacketTypePassDropToCpuModeGet",
         &wrCpssDxChCncPacketTypePassDropToCpuModeGet,
         1, 0},
        {"cpssDxChCncTmClientIndexModeSet",
         &wrCpssDxChCncTmClientIndexModeSet,
         2, 0},
        {"cpssDxChCncTmClientIndexModeGet",
         &wrCpssDxChCncTmClientIndexModeGet,
         1, 0},
        {"cpssDxChCncEgressVlanDropCountModeSet",
         &wrCpssDxChCncEgressVlanDropCountModeSet,
         2, 0},
        {"cpssDxChCncEgressVlanDropCountModeGet",
         &wrCpssDxChCncEgressVlanDropCountModeGet,
         1, 0},
        {"cpssDxChCncCounterClearByReadEnableSet",
         &wrCpssDxChCncCounterClearByReadEnableSet,
         2, 0},
        {"cpssDxChCncCounterClearByReadEnableGet",
         &wrCpssDxChCncCounterClearByReadEnableGet,
         1, 0},
        {"cpssDxChCncCounterClearByReadValueSet",
         &wrCpssDxChCncCounterClearByReadValueSet,
         4, 0},
        {"cpssDxChCncCounterClearByReadValueGet",
         &wrCpssDxChCncCounterClearByReadValueGet,
         1, 0},
        {"cpssDxChCncCounterClearByReadValueSet64",
         &wrCpssDxChCncCounterClearByReadValueSet64,
         6, 0},
        {"cpssDxChCncCounterClearByReadValueGet64",
         &wrCpssDxChCncCounterClearByReadValueGet64,
         2, 0},
        {"cpssDxChCncCounterWraparoundEnableSet",
         &wrCpssDxChCncCounterWraparoundEnableSet,
         2, 0},
        {"cpssDxChCncCounterWraparoundEnableGet",
         &wrCpssDxChCncCounterWraparoundEnableGet,
         1, 0},
        {"cpssDxChCncCounterWraparoundIndexesGetFirst",
         &wrCpssDxChCncCounterWraparoundIndexesGetFirst,
         3, 0},
        {"cpssDxChCncCounterWraparoundIndexesGetNext",
         &wrCpssDxChCncCounterWraparoundIndexesGetNext,
         0, 0},
        {"cpssDxChCncCounterSet",
         &wrCpssDxChCncCounterSet,
         6, 0},
        {"cpssDxChCncCounterGet",
         &wrCpssDxChCncCounterGet,
         3, 0},
        {"cpssDxChCncCounterSet64",
         &wrCpssDxChCncCounterSet64,
         8, 0},
        {"cpssDxChCncCounterGet64",
         &wrCpssDxChCncCounterGet64,
         4, 0},

        {"cpssDxChCncBlockUploadTrigger",
         &wrCpssDxChCncBlockUploadTrigger,
         2, 0},
        {"cpssDxChCncBlockUploadInProcessGet",
         &wrCpssDxChCncBlockUploadInProcessGet,
         1, 0},
        {"cpssDxChCncUploadedBlockGetFirst",
         &wrCpssDxChCncUploadedBlockGetFirst,
         2, 0},
        {"cpssDxChCncUploadedBlockGetNext",
         &wrCpssDxChCncUploadedBlockGetNext,
         0, 0},
        {"cpssDxChCncUploadedBlock64GetFirst",
         &wrCpssDxChCncUploadedBlockGetFirst64,
         3, 0},
        {"cpssDxChCncUploadedBlock64GetNext",
         &wrCpssDxChCncUploadedBlockGetNext64,
         0, 0},

        {"cpssDxChCncCountingEnableSet",
         &wrCpssDxChCncCountingEnableSet,
         3, 0},
        {"cpssDxChCncCountingEnableGet",
         &wrCpssDxChCncCountingEnableGet,
         2, 0},

        {"cpssDxChCncEgrQueueClientModeSet",
         &wrCpssDxChCncEgressQueueClientModeSet,
         2, 0},
        {"cpssDxChCncEgrQueueClientModeGet",
         &wrCpssDxChCncEgressQueueClientModeGet,
         1, 0},

        {"cpssDxChCncCpuAccessStrictPrioEnSet",
         &wrCpssDxChCncCpuAccessStrictPriorityEnableSet,
         2, 0},
        {"cpssDxChCncCpuAccessStrictPrioEnGet",
         &wrCpssDxChCncCpuAccessStrictPriorityEnableGet,
         1, 0},

        {"cpssDxChCncCounterFormatSet",
         &wrCpssDxChCncCounterFormatSet,
         3, 0},
        {"cpssDxChCncCounterFormatGet",
         &wrCpssDxChCncCounterFormatGet,
         2, 0},

        {"cpssDxChCncOffsetForNatClientSet",
         &wrCpssDxChCncOffsetForNatClientSet,
         2, 0},
        {"cpssDxChCncOffsetForNatClientGet",
         &wrCpssDxChCncOffsetForNatClientGet,
         1, 0}


};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssDxChCnc
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
GT_STATUS cmdLibInitCpssDxChCnc
(
    GT_VOID
)
{
    return  cmdInitLibrary(dbCommands, numCommands);
}

