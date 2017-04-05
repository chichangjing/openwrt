
/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapCpssDxChTunnel.c
*
* DESCRIPTION:
*       Wrapper functions for Tunnel cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 25 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h>

static  CPSS_TUNNEL_TYPE_ENT                 tunnelType;

/* table cpssDxChTunnelStart (union table)
*
* DESCRIPTION:
*       tunnel start configuration table.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* Fields:
*       stConfigPtr     - points to tunnel start configuration
*
*
* Comments:
*
*/


/* table cpssDxChTunnelStart global variables */

static  CPSS_DXCH_TUNNEL_START_CONFIG_UNT    stConfigPtr;
static  GT_U32                               routerArpTunnelGetIndex;
static  GT_U32                               routerArpTunnelMaxGet;

/*******************************************************************************
* tunnelStartMultiPortGroupsBmpGet
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
static void tunnelStartMultiPortGroupsBmpGet
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

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTunnelStartEntryGet
(
    IN   GT_U8                              devNum,
    IN   GT_U32                             routerArpTunnelStartLineIndex,
    OUT  CPSS_TUNNEL_TYPE_ENT               *tunnelTypePtr,
    OUT  CPSS_DXCH_TUNNEL_START_CONFIG_UNT  *configPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    tunnelStartMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTunnelStartEntryGet(devNum,
                                           routerArpTunnelStartLineIndex,
                                           tunnelTypePtr,
                                           configPtr);
    }
    else
    {
        return cpssDxChTunnelStartPortGroupEntryGet(devNum,
                                                    pgBmp,
                                                    routerArpTunnelStartLineIndex,
                                                    tunnelTypePtr,
                                                    configPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTunnelStartEntrySet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerArpTunnelStartLineIndex,
    IN  CPSS_TUNNEL_TYPE_ENT                tunnelType,
    IN  CPSS_DXCH_TUNNEL_START_CONFIG_UNT   *configPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    tunnelStartMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTunnelStartEntrySet(devNum,
                                           routerArpTunnelStartLineIndex,
                                           tunnelType,
                                           configPtr);
    }
    else
    {
        return cpssDxChTunnelStartPortGroupEntrySet(devNum,
                                                    pgBmp,
                                                    routerArpTunnelStartLineIndex,
                                                    tunnelType,
                                                    configPtr);
    }
}


/*******************************************************************************
* cpssDxChTunnelStartEntrySet
*
* DESCRIPTION:
*       Set a tunnel start entry.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table (0..1023)
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_FAIL           - on failure.
*       GT_OUT_OF_RANGE   - parameter not in valid range.
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Indexes for tunnel start range is (0..1023); Indexes for router ARP
*       addresses range is (0..4095).
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEntrySet_IPV4_CONFIG

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(inArgs[3])
    {
        galtisOutput(outArgs, GT_BAD_STATE, "%d", -1);
        return CMD_OK;
    }

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.ipv4Cfg.tagEnable = (GT_BOOL)inFields[2];
    stConfigPtr.ipv4Cfg.vlanId = (GT_U16)inFields[3];
    stConfigPtr.ipv4Cfg.upMarkMode =
                          (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.ipv4Cfg.up = (GT_U32)inFields[5];
    stConfigPtr.ipv4Cfg.dscpMarkMode =
                          (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[6];
    stConfigPtr.ipv4Cfg.dscp = (GT_U32)inFields[7];
    galtisMacAddr(&stConfigPtr.ipv4Cfg.macDa, (GT_U8*)inFields[8]);
    stConfigPtr.ipv4Cfg.dontFragmentFlag = (GT_BOOL)inFields[9];
    stConfigPtr.ipv4Cfg.ttl = (GT_U32)inFields[10];
    stConfigPtr.ipv4Cfg.autoTunnel = (GT_BOOL)inFields[11];
    stConfigPtr.ipv4Cfg.autoTunnelOffset = (GT_U32)inFields[12];
    galtisIpAddr(&stConfigPtr.ipv4Cfg.destIp, (GT_U8*)inFields[13]);
    galtisIpAddr(&stConfigPtr.ipv4Cfg.srcIp, (GT_U8*)inFields[14]);

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEntrySet_MPLS_CONFIG

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(!inArgs[3])
    {
        galtisOutput(outArgs, GT_BAD_STATE, "%d", -1);
        return CMD_OK;
    }

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.mplsCfg.tagEnable = (GT_BOOL)inFields[2];
    stConfigPtr.mplsCfg.vlanId = (GT_U16)inFields[3];
    stConfigPtr.mplsCfg.upMarkMode = (GT_BOOL)inFields[4];
    stConfigPtr.mplsCfg.up = (GT_BOOL)inFields[5];
    galtisMacAddr(&stConfigPtr.mplsCfg.macDa, (GT_U8*)inFields[6]);
    stConfigPtr.mplsCfg.numLabels = (GT_BOOL)inFields[7];
    stConfigPtr.mplsCfg.ttl = (GT_BOOL)inFields[8];
    stConfigPtr.mplsCfg.label1 = (GT_BOOL)inFields[9];
    stConfigPtr.mplsCfg.exp1MarkMode = (GT_BOOL)inFields[10];
    stConfigPtr.mplsCfg.exp1 = (GT_BOOL)inFields[11];
    stConfigPtr.mplsCfg.label2 = (GT_BOOL)inFields[12];
    stConfigPtr.mplsCfg.exp2MarkMode = (GT_BOOL)inFields[13];
    stConfigPtr.mplsCfg.exp2 = (GT_BOOL)inFields[14];
    stConfigPtr.mplsCfg.retainCRC = (GT_BOOL)inFields[15];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEntryGet
*
* DESCRIPTION:
*       Get a tunnel start entry.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       routerArpTunnelStartLineIndex - line index for the tunnel start entry
*                       in the router ARP / tunnel start table (0..1023)
*
* OUTPUTS:
*       tunnelType    - points to the type of the tunnel, valid values:
*                       CPSS_TUNNEL_X_OVER_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_GRE_IPV4_E,
*                       CPSS_TUNNEL_X_OVER_MPLS_E,
*                       CPSS_TUNNEL_INVALID_E
*       configPtr     - points to tunnel start configuration
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_FAIL           - on failure.
*       GT_OUT_OF_RANGE   - parameter not in valid range.
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       Tunnel start entries table and router ARP addresses table reside at
*       the same physical memory. The table contains 1K lines.
*       Each line can hold:
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       Indexes for tunnel start range is (0..1023); Indexes for router ARP
*       addresses range is (0..4095).
*       Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEntryGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U32                    tmpEntryGet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    routerArpTunnelGetIndex = (GT_U32)inArgs[1];
    tmpEntryGet = (GT_U32)inArgs[2];

    routerArpTunnelMaxGet = routerArpTunnelGetIndex + tmpEntryGet;

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntryGet(devNum, routerArpTunnelGetIndex,
                                               &tunnelType, &stConfigPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    switch(tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:

        inArgs[3] = 0;

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2] = stConfigPtr.ipv4Cfg.tagEnable;
        inFields[3] = stConfigPtr.ipv4Cfg.vlanId;
        inFields[4] = stConfigPtr.ipv4Cfg.upMarkMode;
        inFields[5] = stConfigPtr.ipv4Cfg.up;
        inFields[6] = stConfigPtr.ipv4Cfg.dscpMarkMode;
        inFields[7] = stConfigPtr.ipv4Cfg.dscp;

        inFields[9] = stConfigPtr.ipv4Cfg.dontFragmentFlag;
        inFields[10] = stConfigPtr.ipv4Cfg.ttl;
        inFields[11] = stConfigPtr.ipv4Cfg.autoTunnel;
        inFields[12] = stConfigPtr.ipv4Cfg.autoTunnelOffset;


        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%6b%d%d%d%d%4b%4b",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], inFields[5],  inFields[6],  inFields[7],
                    stConfigPtr.ipv4Cfg.macDa.arEther,
                    inFields[9], inFields[10], inFields[11], inFields[12],
                    stConfigPtr.ipv4Cfg.destIp.arIP,
                    stConfigPtr.ipv4Cfg.srcIp.arIP);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 0);
        break;
    case CPSS_TUNNEL_X_OVER_MPLS_E:

        inArgs[3] = 1;

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2] = stConfigPtr.mplsCfg.tagEnable;
        inFields[3] = stConfigPtr.mplsCfg.vlanId;
        inFields[4] = stConfigPtr.mplsCfg.upMarkMode;
        inFields[5] = stConfigPtr.mplsCfg.up;

        inFields[7] =  stConfigPtr.mplsCfg.numLabels;
        inFields[8] =  stConfigPtr.mplsCfg.ttl;
        inFields[9] =  stConfigPtr.mplsCfg.label1;
        inFields[10] = stConfigPtr.mplsCfg.exp1MarkMode;
        inFields[11] = stConfigPtr.mplsCfg.exp1;
        inFields[12] = stConfigPtr.mplsCfg.label2;
        inFields[13] = stConfigPtr.mplsCfg.exp2MarkMode;
        inFields[14] = stConfigPtr.mplsCfg.exp2;
        inFields[15] = stConfigPtr.mplsCfg.retainCRC;


        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    stConfigPtr.mplsCfg.macDa.arEther,
                    inFields[7],  inFields[8],  inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14], inFields[15]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 1);
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        break;
    }

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEntryGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    routerArpTunnelGetIndex++;

    if(routerArpTunnelGetIndex > 4095 ||
       routerArpTunnelGetIndex >= routerArpTunnelMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntryGet(devNum, routerArpTunnelGetIndex,
                                               &tunnelType, &stConfigPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    switch(tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:

        inArgs[3] = 0;

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2] = stConfigPtr.ipv4Cfg.tagEnable;
        inFields[3] = stConfigPtr.ipv4Cfg.vlanId;
        inFields[4] = stConfigPtr.ipv4Cfg.upMarkMode;
        inFields[5] = stConfigPtr.ipv4Cfg.up;
        inFields[6] = stConfigPtr.ipv4Cfg.dscpMarkMode;
        inFields[7] = stConfigPtr.ipv4Cfg.dscp;

        inFields[9] = stConfigPtr.ipv4Cfg.dontFragmentFlag;
        inFields[10] = stConfigPtr.ipv4Cfg.ttl;
        inFields[11] = stConfigPtr.ipv4Cfg.autoTunnel;
        inFields[12] = stConfigPtr.ipv4Cfg.autoTunnelOffset;


        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%6b%d%d%d%d%4b%4b",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], inFields[5],  inFields[6],  inFields[7],
                    stConfigPtr.ipv4Cfg.macDa.arEther,
                    inFields[9], inFields[10], inFields[11], inFields[12],
                    stConfigPtr.ipv4Cfg.destIp.arIP,
                    stConfigPtr.ipv4Cfg.srcIp.arIP);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 0);
        break;
    case CPSS_TUNNEL_X_OVER_MPLS_E:

        inArgs[3] = 1;

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2] = stConfigPtr.mplsCfg.tagEnable;
        inFields[3] = stConfigPtr.mplsCfg.vlanId;
        inFields[4] = stConfigPtr.mplsCfg.upMarkMode;
        inFields[5] = stConfigPtr.mplsCfg.up;

        inFields[7] =  stConfigPtr.mplsCfg.numLabels;
        inFields[8] =  stConfigPtr.mplsCfg.ttl;
        inFields[9] =  stConfigPtr.mplsCfg.label1;
        inFields[10] = stConfigPtr.mplsCfg.exp1MarkMode;
        inFields[11] = stConfigPtr.mplsCfg.exp1;
        inFields[12] = stConfigPtr.mplsCfg.label2;
        inFields[13] = stConfigPtr.mplsCfg.exp2MarkMode;
        inFields[14] = stConfigPtr.mplsCfg.exp2;
        inFields[15] = stConfigPtr.mplsCfg.retainCRC;


        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    stConfigPtr.mplsCfg.macDa.arEther,
                    inFields[7],  inFields[8],  inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14], inFields[15]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 1);
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        break;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_1Set_IPV4_CONFIG
*
* DESCRIPTION:
*       Set IPv4 tunnel start entry for cpssDxChTunnelStartEntrySet_1 table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_1Set_IPV4_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.ipv4Cfg.tagEnable           = (GT_BOOL)inFields[2];
    stConfigPtr.ipv4Cfg.vlanId              = (GT_U16)inFields[3];
    stConfigPtr.ipv4Cfg.upMarkMode          = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.ipv4Cfg.up                  = (GT_U32)inFields[5];
    stConfigPtr.ipv4Cfg.dscpMarkMode        = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[6];
    stConfigPtr.ipv4Cfg.dscp                = (GT_U32)inFields[7];
    galtisMacAddr(&stConfigPtr.ipv4Cfg.macDa,(GT_U8*)inFields[8]);
    stConfigPtr.ipv4Cfg.dontFragmentFlag    = (GT_BOOL)inFields[9];
    stConfigPtr.ipv4Cfg.ttl                 = (GT_U32)inFields[10];
    stConfigPtr.ipv4Cfg.autoTunnel          = (GT_BOOL)inFields[11];
    stConfigPtr.ipv4Cfg.autoTunnelOffset    = (GT_U32)inFields[12];
    galtisIpAddr(&stConfigPtr.ipv4Cfg.destIp,(GT_U8*)inFields[13]);
    galtisIpAddr(&stConfigPtr.ipv4Cfg.srcIp,(GT_U8*)inFields[14]);
    stConfigPtr.ipv4Cfg.cfi                 = (GT_U32)inFields[15];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_1Set_MPLS_CONFIG
*
* DESCRIPTION:
*       Set MPLS tunnel start entry for cpssDxChTunnelStartEntrySet_1 table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_1Set_MPLS_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.mplsCfg.tagEnable       = (GT_BOOL)inFields[2];
    stConfigPtr.mplsCfg.vlanId          = (GT_U16)inFields[3];
    stConfigPtr.mplsCfg.upMarkMode      = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.mplsCfg.up              = (GT_U32)inFields[5];
    galtisMacAddr(&stConfigPtr.mplsCfg.macDa, (GT_U8*)inFields[6]);
    stConfigPtr.mplsCfg.numLabels       = (GT_U32)inFields[7];
    stConfigPtr.mplsCfg.ttl             = (GT_U32)inFields[8];
    stConfigPtr.mplsCfg.ttlMode         = (CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT)inFields[9];
    stConfigPtr.mplsCfg.label1          = (GT_U32)inFields[10];
    stConfigPtr.mplsCfg.exp1MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[11];
    stConfigPtr.mplsCfg.exp1            = (GT_U32)inFields[12];
    stConfigPtr.mplsCfg.label2          = (GT_U32)inFields[13];
    stConfigPtr.mplsCfg.exp2MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[14];
    stConfigPtr.mplsCfg.exp2            = (GT_U32)inFields[15];
    stConfigPtr.mplsCfg.label3          = (GT_U32)inFields[16];
    stConfigPtr.mplsCfg.exp3MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[17];
    stConfigPtr.mplsCfg.exp3            = (GT_U32)inFields[18];
    stConfigPtr.mplsCfg.retainCRC       = (GT_BOOL)inFields[19];
    stConfigPtr.mplsCfg.setSBit         = (GT_BOOL)inFields[20];
    stConfigPtr.mplsCfg.cfi             = (GT_U32)inFields[21];
    stConfigPtr.mplsCfg.controlWordEnable = GT_FALSE;
    stConfigPtr.mplsCfg.controlWordIndex = 0;
    stConfigPtr.mplsCfg.mplsEthertypeSelect = CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_UC_E;

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_3Set_MPLS_CONFIG
*
* DESCRIPTION:
*       Set MPLS tunnel start entry for cpssDxChTunnelStartEntrySet table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_3Set_MPLS_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.mplsCfg.tagEnable       = (GT_BOOL)inFields[2];
    stConfigPtr.mplsCfg.vlanId          = (GT_U16)inFields[3];
    stConfigPtr.mplsCfg.upMarkMode      = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.mplsCfg.up              = (GT_U32)inFields[5];
    galtisMacAddr(&stConfigPtr.mplsCfg.macDa, (GT_U8*)inFields[6]);
    stConfigPtr.mplsCfg.numLabels       = (GT_U32)inFields[7];
    stConfigPtr.mplsCfg.ttl             = (GT_U32)inFields[8];
    stConfigPtr.mplsCfg.ttlMode         = (CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT)inFields[9];
    stConfigPtr.mplsCfg.label1          = (GT_U32)inFields[10];
    stConfigPtr.mplsCfg.exp1MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[11];
    stConfigPtr.mplsCfg.exp1            = (GT_U32)inFields[12];
    stConfigPtr.mplsCfg.label2          = (GT_U32)inFields[13];
    stConfigPtr.mplsCfg.exp2MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[14];
    stConfigPtr.mplsCfg.exp2            = (GT_U32)inFields[15];
    stConfigPtr.mplsCfg.label3          = (GT_U32)inFields[16];
    stConfigPtr.mplsCfg.exp3MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[17];
    stConfigPtr.mplsCfg.exp3            = (GT_U32)inFields[18];
    stConfigPtr.mplsCfg.retainCRC       = (GT_BOOL)inFields[19];
    stConfigPtr.mplsCfg.setSBit         = (GT_BOOL)inFields[20];
    stConfigPtr.mplsCfg.cfi             = (GT_U32)inFields[21];
    stConfigPtr.mplsCfg.controlWordEnable = (GT_U32)inFields[22];
    stConfigPtr.mplsCfg.controlWordIndex = (GT_U32)inFields[23];
    stConfigPtr.mplsCfg.mplsEthertypeSelect = (CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT)inFields[24];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_4Set_MPLS_CONFIG
*
* DESCRIPTION:
*       Set MPLS tunnel start entry for cpssDxChTunnelStartEntrySet table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4Set_MPLS_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.mplsCfg.tagEnable       = (GT_BOOL)inFields[2];
    stConfigPtr.mplsCfg.vlanId          = (GT_U16)inFields[3];
    stConfigPtr.mplsCfg.upMarkMode      = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.mplsCfg.up              = (GT_U32)inFields[5];
    galtisMacAddr(&stConfigPtr.mplsCfg.macDa, (GT_U8*)inFields[6]);
    stConfigPtr.mplsCfg.numLabels       = (GT_U32)inFields[7];
    stConfigPtr.mplsCfg.ttl             = (GT_U32)inFields[8];
    stConfigPtr.mplsCfg.ttlMode         = (CPSS_DXCH_TUNNEL_START_TTL_MODE_ENT)inFields[9];
    stConfigPtr.mplsCfg.label1          = (GT_U32)inFields[10];
    stConfigPtr.mplsCfg.exp1MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[11];
    stConfigPtr.mplsCfg.exp1            = (GT_U32)inFields[12];
    stConfigPtr.mplsCfg.label2          = (GT_U32)inFields[13];
    stConfigPtr.mplsCfg.exp2MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[14];
    stConfigPtr.mplsCfg.exp2            = (GT_U32)inFields[15];
    stConfigPtr.mplsCfg.label3          = (GT_U32)inFields[16];
    stConfigPtr.mplsCfg.exp3MarkMode    = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[17];
    stConfigPtr.mplsCfg.exp3            = (GT_U32)inFields[18];
    stConfigPtr.mplsCfg.retainCRC       = (GT_BOOL)inFields[19];
    stConfigPtr.mplsCfg.setSBit         = (GT_BOOL)inFields[20];
    stConfigPtr.mplsCfg.cfi             = (GT_U32)inFields[21];
    stConfigPtr.mplsCfg.controlWordEnable = (GT_U32)inFields[22];
    stConfigPtr.mplsCfg.controlWordIndex = (GT_U32)inFields[23];
    stConfigPtr.mplsCfg.mplsEthertypeSelect = (CPSS_DXCH_TUNNEL_START_MPLS_ETHER_TYPE_ENT)inFields[24];
    /* reserve inFields[25] for MPLS PW EXP Marking Mode */
    stConfigPtr.mplsCfg.pushEliAndElAfterLabel1 = (GT_BOOL)inFields[26];
    stConfigPtr.mplsCfg.pushEliAndElAfterLabel2 = (GT_BOOL)inFields[27];
    stConfigPtr.mplsCfg.pushEliAndElAfterLabel3 = (GT_BOOL)inFields[28];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_1Set_MIM_CONFIG
*
* DESCRIPTION:
*       Set MIM tunnel start entry for cpssDxChTunnelStartEntrySet_1 table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_1Set_MIM_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.mimCfg.tagEnable       = (GT_BOOL)inFields[2];
    stConfigPtr.mimCfg.vlanId          = (GT_U16)inFields[3];
    stConfigPtr.mimCfg.upMarkMode      = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.mimCfg.up              = (GT_U32)inFields[5];
    /* mimCfg.ttl field was removed from the API and will be ignored */
    galtisMacAddr(&stConfigPtr.mimCfg.macDa, (GT_U8*)inFields[7]);
    stConfigPtr.mimCfg.retainCrc       = (GT_BOOL)inFields[8];
    stConfigPtr.mimCfg.iSid            = (GT_U32)inFields[9];
    /* mimCfg.iSidAssignMode field was removed from the API and will be ignored */
    stConfigPtr.mimCfg.iUp             = (GT_U32)inFields[11];
    stConfigPtr.mimCfg.iUpMarkMode     = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[12];
    stConfigPtr.mimCfg.iDp             = (GT_U32)inFields[13];
    stConfigPtr.mimCfg.iDpMarkMode     = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[14];
    stConfigPtr.mimCfg.iTagReserved    = (GT_U32)inFields[15];
    stConfigPtr.mimCfg.iSidAssignMode  = CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_FROM_TS_ENTRY_E;

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_1GetEntry
*
* DESCRIPTION:
*       Get single tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_1GetEntry

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     devNum;

    /* map input arguments to locals */
    devNum                  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntryGet(devNum, routerArpTunnelGetIndex,
                                               &tunnelType, &stConfigPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    switch(tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
    case CPSS_TUNNEL_GENERIC_IPV4_E:
        if (tunnelType == CPSS_TUNNEL_GENERIC_IPV4_E)
        {
            if (stConfigPtr.ipv4Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E)
            {
                tunnelType = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;
            }
            if (stConfigPtr.ipv4Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_IP_E)
            {
                tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;
            }

        }
        inFields[0]  = routerArpTunnelGetIndex;
        inFields[1]  = tunnelType;

        inFields[2]  = stConfigPtr.ipv4Cfg.tagEnable;
        inFields[3]  = stConfigPtr.ipv4Cfg.vlanId;
        inFields[4]  = stConfigPtr.ipv4Cfg.upMarkMode;
        inFields[5]  = stConfigPtr.ipv4Cfg.up;
        inFields[6]  = stConfigPtr.ipv4Cfg.dscpMarkMode;
        inFields[7]  = stConfigPtr.ipv4Cfg.dscp;
        /*inFields[8]  = stConfigPtr.ipv4Cfg.macDa;*/
        inFields[9]  = stConfigPtr.ipv4Cfg.dontFragmentFlag;
        inFields[10] = stConfigPtr.ipv4Cfg.ttl;
        inFields[11] = stConfigPtr.ipv4Cfg.autoTunnel;
        inFields[12] = stConfigPtr.ipv4Cfg.autoTunnelOffset;
        /*inFields[13]  = stConfigPtr.ipv4Cfg.destIp;*/
        /*inFields[14]  = stConfigPtr.ipv4Cfg.srcIp;*/
        inFields[15] = stConfigPtr.ipv4Cfg.cfi;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%6b%d%d%d%d%4b%4b%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], inFields[5],  inFields[6],  inFields[7],
                    stConfigPtr.ipv4Cfg.macDa.arEther,
                    inFields[9], inFields[10], inFields[11], inFields[12],
                    stConfigPtr.ipv4Cfg.destIp.arIP,
                    stConfigPtr.ipv4Cfg.srcIp.arIP,
                    inFields[15]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 0);
        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2]  = stConfigPtr.mplsCfg.tagEnable;
        inFields[3]  = stConfigPtr.mplsCfg.vlanId;
        inFields[4]  = stConfigPtr.mplsCfg.upMarkMode;
        inFields[5]  = stConfigPtr.mplsCfg.up;
        /*inFields[6]  = stConfigPtr.mplsCfg.macDa;*/
        inFields[7]  = stConfigPtr.mplsCfg.numLabels;
        inFields[8]  = stConfigPtr.mplsCfg.ttl;
        inFields[9]  = stConfigPtr.mplsCfg.ttlMode;
        inFields[10] = stConfigPtr.mplsCfg.label1;
        inFields[11] = stConfigPtr.mplsCfg.exp1MarkMode;
        inFields[12] = stConfigPtr.mplsCfg.exp1;
        inFields[13] = stConfigPtr.mplsCfg.label2;
        inFields[14] = stConfigPtr.mplsCfg.exp2MarkMode;
        inFields[15] = stConfigPtr.mplsCfg.exp2;
        inFields[16] = stConfigPtr.mplsCfg.label3;
        inFields[17] = stConfigPtr.mplsCfg.exp3MarkMode;
        inFields[18] = stConfigPtr.mplsCfg.exp3;
        inFields[19] = stConfigPtr.mplsCfg.retainCRC;
        inFields[20] = stConfigPtr.mplsCfg.setSBit;
        inFields[21] = stConfigPtr.mplsCfg.cfi;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    stConfigPtr.mplsCfg.macDa.arEther,
                    inFields[7],  inFields[8],  inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18],
                    inFields[19], inFields[20], inFields[21]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 1);
        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2]  = stConfigPtr.mimCfg.tagEnable;
        inFields[3]  = stConfigPtr.mimCfg.vlanId;
        inFields[4]  = stConfigPtr.mimCfg.upMarkMode;
        inFields[5]  = stConfigPtr.mimCfg.up;
        inFields[6]  = 0; /* ttl was removed from mimCfg struct and will always return 0 */
        /*inFields[7]  = stConfigPtr.mimCfg.macDa;*/
        inFields[8]  = stConfigPtr.mimCfg.retainCrc;
        inFields[9]  = stConfigPtr.mimCfg.iSid;
        inFields[10] = 0; /* iSidAssignMode was removed from mimCfg struct and will always return 0 */
        inFields[11] = stConfigPtr.mimCfg.iUp;
        inFields[12] = stConfigPtr.mimCfg.iUpMarkMode;
        inFields[13] = stConfigPtr.mimCfg.iDp;
        inFields[14] = stConfigPtr.mimCfg.iDpMarkMode;
        inFields[15] = stConfigPtr.mimCfg.iTagReserved;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    inFields[6],  stConfigPtr.mimCfg.macDa.arEther,
                    inFields[8],  inFields[9],  inFields[10],
                    inFields[11], inFields[12], inFields[13],
                    inFields[14], inFields[15]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 2);
        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        break;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_3GetEntry
*
* DESCRIPTION:
*       Get single tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_3GetEntry

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     devNum;

    /* map input arguments to locals */
    devNum                  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntryGet(devNum, routerArpTunnelGetIndex,
                                               &tunnelType, &stConfigPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    switch(tunnelType)
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
    case CPSS_TUNNEL_GENERIC_IPV4_E:
        if (tunnelType == CPSS_TUNNEL_GENERIC_IPV4_E)
        {
            if (stConfigPtr.ipv4Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_GRE_E)
            {
                tunnelType = CPSS_TUNNEL_X_OVER_GRE_IPV4_E;
            }
            if (stConfigPtr.ipv4Cfg.ipHeaderProtocol == CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_IP_E)
            {
                tunnelType = CPSS_TUNNEL_X_OVER_IPV4_E;
            }

        }
        inFields[0]  = routerArpTunnelGetIndex;
        inFields[1]  = tunnelType;

        inFields[2]  = stConfigPtr.ipv4Cfg.tagEnable;
        inFields[3]  = stConfigPtr.ipv4Cfg.vlanId;
        inFields[4]  = stConfigPtr.ipv4Cfg.upMarkMode;
        inFields[5]  = stConfigPtr.ipv4Cfg.up;
        inFields[6]  = stConfigPtr.ipv4Cfg.dscpMarkMode;
        inFields[7]  = stConfigPtr.ipv4Cfg.dscp;
        /*inFields[8]  = stConfigPtr.ipv4Cfg.macDa;*/
        inFields[9]  = stConfigPtr.ipv4Cfg.dontFragmentFlag;
        inFields[10] = stConfigPtr.ipv4Cfg.ttl;
        inFields[11] = stConfigPtr.ipv4Cfg.autoTunnel;
        inFields[12] = stConfigPtr.ipv4Cfg.autoTunnelOffset;
        /*inFields[13]  = stConfigPtr.ipv4Cfg.destIp;*/
        /*inFields[14]  = stConfigPtr.ipv4Cfg.srcIp;*/
        inFields[15] = stConfigPtr.ipv4Cfg.cfi;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%6b%d%d%d%d%4b%4b%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], inFields[5],  inFields[6],  inFields[7],
                    stConfigPtr.ipv4Cfg.macDa.arEther,
                    inFields[9], inFields[10], inFields[11], inFields[12],
                    stConfigPtr.ipv4Cfg.destIp.arIP,
                    stConfigPtr.ipv4Cfg.srcIp.arIP,
                    inFields[15]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 0);
        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2]  = stConfigPtr.mplsCfg.tagEnable;
        inFields[3]  = stConfigPtr.mplsCfg.vlanId;
        inFields[4]  = stConfigPtr.mplsCfg.upMarkMode;
        inFields[5]  = stConfigPtr.mplsCfg.up;
        /*inFields[6]  = stConfigPtr.mplsCfg.macDa;*/
        inFields[7]  = stConfigPtr.mplsCfg.numLabels;
        inFields[8]  = stConfigPtr.mplsCfg.ttl;
        inFields[9]  = stConfigPtr.mplsCfg.ttlMode;
        inFields[10] = stConfigPtr.mplsCfg.label1;
        inFields[11] = stConfigPtr.mplsCfg.exp1MarkMode;
        inFields[12] = stConfigPtr.mplsCfg.exp1;
        inFields[13] = stConfigPtr.mplsCfg.label2;
        inFields[14] = stConfigPtr.mplsCfg.exp2MarkMode;
        inFields[15] = stConfigPtr.mplsCfg.exp2;
        inFields[16] = stConfigPtr.mplsCfg.label3;
        inFields[17] = stConfigPtr.mplsCfg.exp3MarkMode;
        inFields[18] = stConfigPtr.mplsCfg.exp3;
        inFields[19] = stConfigPtr.mplsCfg.retainCRC;
        inFields[20] = stConfigPtr.mplsCfg.setSBit;
        inFields[21] = stConfigPtr.mplsCfg.cfi;
        inFields[22] = stConfigPtr.mplsCfg.controlWordEnable;
        inFields[23] = stConfigPtr.mplsCfg.controlWordIndex;
        inFields[24] = stConfigPtr.mplsCfg.mplsEthertypeSelect;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    stConfigPtr.mplsCfg.macDa.arEther,
                    inFields[7],  inFields[8],  inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18],
                    inFields[19], inFields[20], inFields[21],
                    inFields[22], inFields[23], inFields[24]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 1);
        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2]  = stConfigPtr.mimCfg.tagEnable;
        inFields[3]  = stConfigPtr.mimCfg.vlanId;
        inFields[4]  = stConfigPtr.mimCfg.upMarkMode;
        inFields[5]  = stConfigPtr.mimCfg.up;
        inFields[6]  = 0; /* ttl was removed from mimCfg struct and will always return 0 */
        /*inFields[7]  = stConfigPtr.mimCfg.macDa;*/
        inFields[8]  = stConfigPtr.mimCfg.retainCrc;
        inFields[9]  = stConfigPtr.mimCfg.iSid;
        inFields[10] = 0; /* iSidAssignMode was removed from mimCfg struct and will always return 0 */
        inFields[11] = stConfigPtr.mimCfg.iUp;
        inFields[12] = stConfigPtr.mimCfg.iUpMarkMode;
        inFields[13] = stConfigPtr.mimCfg.iDp;
        inFields[14] = stConfigPtr.mimCfg.iDpMarkMode;
        inFields[15] = stConfigPtr.mimCfg.iTagReserved;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    inFields[6],  stConfigPtr.mimCfg.macDa.arEther,
                    inFields[8],  inFields[9],  inFields[10],
                    inFields[11], inFields[12], inFields[13],
                    inFields[14], inFields[15]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 2);
        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        break;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_1GetFirst
*
* DESCRIPTION:
*       Get first tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_1GetFirst

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

    /* map input arguments to locals */
    routerArpTunnelGetIndex = (GT_U32)inArgs[1];
    routerArpTunnelMaxGet   = routerArpTunnelGetIndex + (GT_U32)inArgs[2];

    if(routerArpTunnelGetIndex > 4095 ||
       routerArpTunnelGetIndex >= routerArpTunnelMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChTunnelStart_1GetEntry(
        inArgs, inFields ,numFields ,outArgs);
}

/*******************************************************************************
* wrCpssDxChTunnelStart_3GetFirst
*
* DESCRIPTION:
*       Get first tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_3GetFirst

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

    /* map input arguments to locals */
    routerArpTunnelGetIndex = (GT_U32)inArgs[1];
    routerArpTunnelMaxGet   = routerArpTunnelGetIndex + (GT_U32)inArgs[2];

    if(routerArpTunnelGetIndex > 4095 ||
       routerArpTunnelGetIndex >= routerArpTunnelMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChTunnelStart_3GetEntry(
        inArgs, inFields ,numFields ,outArgs);
}

/*******************************************************************************
* wrCpssDxChTunnelStart_1GetNext
*
* DESCRIPTION:
*       Get next tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_1GetNext

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

    /* map input arguments to locals */
    routerArpTunnelGetIndex++;

    if(routerArpTunnelGetIndex > 4095 ||
       routerArpTunnelGetIndex >= routerArpTunnelMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChTunnelStart_1GetEntry(
        inArgs, inFields ,numFields ,outArgs);
}

/*******************************************************************************
* wrCpssDxChTunnelStart_3GetNext
*
* DESCRIPTION:
*       Get next tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_3GetNext

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

    /* map input arguments to locals */
    routerArpTunnelGetIndex++;

    if(routerArpTunnelGetIndex > 4095 ||
       routerArpTunnelGetIndex >= routerArpTunnelMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChTunnelStart_3GetEntry(
        inArgs, inFields ,numFields ,outArgs);
}

/*******************************************************************************
* cpssDxChIpv4TunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for ipv4 tunnel termination.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable ipv4 tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - on bad parameter.
*       GT_FAIL       - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4TunnelTermPortSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_PHYSICAL_PORT_NUM                         port;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChIpv4TunnelTermPortSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv4TunnelTermPortGet
*
* DESCRIPTION:
*       Get ipv4 tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get ipv4 tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to ipv4 tunnel termination state
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PTR    - on NULL pointer
*       GT_BAD_PARAM  - on bad parameter.
*       GT_FAIL       - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4TunnelTermPortGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_PHYSICAL_PORT_NUM                         port;
    GT_BOOL                       enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChIpv4TunnelTermPortGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMplsTunnelTermPortSet
*
* DESCRIPTION:
*       Set port enable/disable state for mpls tunnel termination.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum    - physical device number
*       port      - port to enable/disable mpls tunnel termination
*       enable    - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - on bad parameter.
*       GT_FAIL       - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMplsTunnelTermPortSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_PHYSICAL_PORT_NUM                         port;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChMplsTunnelTermPortSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMplsTunnelTermPortGet
*
* DESCRIPTION:
*       Get mpls tunnel termination port enable/disable state.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       port          - port to get mpls tunnel termination state
*
* OUTPUTS:
*       enablePtr     - points to mpls tunnel termination state
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PTR    - on NULL pointer
*       GT_BAD_PARAM  - on bad parameter.
*       GT_FAIL       - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMplsTunnelTermPortGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_PHYSICAL_PORT_NUM                         port;
    GT_BOOL                       enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChMplsTunnelTermPortGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/* Table: cpssDxChTunnelTerm (Union Table)
*
* Description:
*     Tunnel Term Configuration table
*
* Fields:
*
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
*       See include file cpss/dxCh/dxChxGen/tunnel/cpssDxChTunnel.h
*       for more details.
*
*
* Comments:
*
*
*
*/


/* table cpssDxChTunnelTerm global variables */

static    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    configPtr;
static    CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT    configMaskPtr;
static    CPSS_DXCH_TUNNEL_TERM_ACTION_STC    actionPtr;
static    GT_U32                              TunnelTermTcamIndexCnt;

/*******************************************************************************
* actionRetrieve
*
* DESCRIPTION:
*   Retrueves the RULE Action data from the table fields
*
*
* INPUTS:
*       inputFields          - source input fields array
*       baseFieldIndex       - index of first field related to the action
*
* OUTPUTS:
*       actionPtr            - retrieves CPSS action
*
* RETURNS:
*       None
*
* COMMENTS:
*       NONE
*
*
*
*******************************************************************************/
static void actionRetrieve

(
    IN  GT_UINTPTR                          inputFields[],
    IN  GT_32                               baseFieldIndex,
    OUT CPSS_DXCH_TUNNEL_TERM_ACTION_STC    *actionPtr
)
{
    GT_UINTPTR *inFields;

    inFields = &(inputFields[baseFieldIndex]);

    /* retrieve command from Galtis Drop Down List */
    switch (inFields[0])
    {
        case 0:
            actionPtr->command = CPSS_PACKET_CMD_FORWARD_E;
            break;
        case 1:
            actionPtr->command = CPSS_PACKET_CMD_MIRROR_TO_CPU_E;
            break;
        case 2:
            actionPtr->command = CPSS_PACKET_CMD_TRAP_TO_CPU_E;
            break;
        case 3:
            actionPtr->command = CPSS_PACKET_CMD_DROP_HARD_E;
            break;
        case 4:
            actionPtr->command = CPSS_PACKET_CMD_DROP_SOFT_E;
            break;
        case 5:
            actionPtr->command = CPSS_PACKET_CMD_ROUTE_E;
            break;
        case 6:
            actionPtr->command = CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E;
            break;
        case 7:
            actionPtr->command = CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E;
            break;
        case 8:
            actionPtr->command = CPSS_PACKET_CMD_BRIDGE_E;
            break;
        case 9:
            actionPtr->command = CPSS_PACKET_CMD_NONE_E;
            break;
        default:
            actionPtr->command = (CPSS_PACKET_CMD_ENT)0xFF;
            cmdOsPrintf(" *** WRONG PACKET COMMAND ***\n");
            break;
    }

    actionPtr->userDefinedCpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[1];
    actionPtr->passengerPacketType =
                                  (CPSS_TUNNEL_PASSENGER_PACKET_ENT)inFields[2];
    actionPtr->egressInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[3];

    switch(inFields[3])
    {
    case 0:
        actionPtr->egressInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[4];
        actionPtr->egressInterface.devPort.portNum = (GT_PORT_NUM)inFields[5];
        break;
    case 1:
        actionPtr->egressInterface.trunkId = (GT_TRUNK_ID)inFields[6];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->egressInterface.trunkId);
        break;
    case 2:
        actionPtr->egressInterface.vidx = (GT_U16)inFields[7];
        break;
    case 3:
        actionPtr->egressInterface.vlanId = (GT_U16)inFields[8];
        break;
    default:
        break;
    }

    actionPtr->isTunnelStart = (GT_BOOL)inFields[9];
    actionPtr->tunnelStartIdx = (GT_U32)inFields[10];
    actionPtr->vlanId = (GT_U16)inFields[11];
    actionPtr->vlanPrecedence =
                      (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[12];
    actionPtr->nestedVlanEnable = (GT_BOOL)inFields[13];
    actionPtr->copyTtlFromTunnelHeader = (GT_BOOL)inFields[14];
    actionPtr->qosMode = (CPSS_DXCH_TUNNEL_QOS_MODE_ENT)inFields[15];
    actionPtr->qosPrecedence =
                      (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[16];
    actionPtr->qosProfile = (GT_U32)inFields[17];
    actionPtr->defaultUP = (GT_U32)inFields[18];
    actionPtr->modifyUP = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[19];
    actionPtr->remapDSCP = (GT_BOOL)inFields[20];
    actionPtr->modifyDSCP = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[21];
    actionPtr->mirrorToIngressAnalyzerEnable = (GT_BOOL)inFields[22];
    actionPtr->policerEnable = (GT_BOOL)inFields[23];
    actionPtr->policerIndex = (GT_U32)inFields[24];
    actionPtr->matchCounterEnable = (GT_BOOL)inFields[25];
    actionPtr->matchCounterIndex = (GT_U32)inFields[26];
}

/*******************************************************************************
* cpssDxChTunnelTermEntrySet
*
* DESCRIPTION:
*       Set a tunnel termination entry.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_FAIL           - on failure.
*       GT_OUT_OF_RANGE   - parameter not in valid range.
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelTermEntrySet_ipv4Cfg

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             routerTunnelTermTcamIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(inArgs[1])
    {
        galtisOutput(outArgs, GT_BAD_STATE, "%d", -1);
        return CMD_OK;
    }

    routerTunnelTermTcamIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    configPtr.ipv4Cfg.srcPortTrunk = (GT_U32)inFields[2];
    configPtr.ipv4Cfg.srcIsTrunk = (GT_U32)inFields[3];
    configPtr.ipv4Cfg.srcDev = (GT_U8)inFields[4];
    configPtr.ipv4Cfg.vid = (GT_U16)inFields[5];
    galtisMacAddr(&configPtr.ipv4Cfg.macDa, (GT_U8*)inFields[6]);
    galtisIpAddr(&configPtr.ipv4Cfg.srcIp, (GT_U8*)inFields[7]);
    galtisIpAddr(&configPtr.ipv4Cfg.destIp, (GT_U8*)inFields[8]);

    configMaskPtr.ipv4Cfg.srcPortTrunk = (GT_U32)inFields[9];
    configMaskPtr.ipv4Cfg.srcIsTrunk = (GT_U32)inFields[10];
    configMaskPtr.ipv4Cfg.srcDev = (GT_U8)inFields[11];
    configMaskPtr.ipv4Cfg.vid = (GT_U16)inFields[12];
    galtisMacAddr(&configMaskPtr.ipv4Cfg.macDa, (GT_U8*)inFields[13]);
    galtisIpAddr(&configMaskPtr.ipv4Cfg.srcIp, (GT_U8*)inFields[14]);
    galtisIpAddr(&configMaskPtr.ipv4Cfg.destIp, (GT_U8*)inFields[15]);

    actionRetrieve(inFields, 16, &actionPtr);

    /* call cpss api function */
    result = cpssDxChTunnelTermEntrySet(devNum, routerTunnelTermTcamIndex,
                                                  tunnelType, &configPtr,
                                              &configMaskPtr, &actionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelTermEntrySet_mplsCfg

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             routerTunnelTermTcamIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(!inArgs[1])
    {
        galtisOutput(outArgs, GT_BAD_STATE, "%d", -1);
        return CMD_OK;
    }

    routerTunnelTermTcamIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    configPtr.mplsCfg.srcPortTrunk = (GT_U32)inFields[2];
    configPtr.mplsCfg.srcIsTrunk = (GT_U32)inFields[3];
    configPtr.mplsCfg.srcDev = (GT_U8)inFields[4];
    configPtr.mplsCfg.vid = (GT_U16)inFields[5];
    galtisMacAddr(&configPtr.mplsCfg.macDa, (GT_U8*)inFields[6]);
    configPtr.mplsCfg.label1 = (GT_U32)inFields[7];
    configPtr.mplsCfg.sBit1 = (GT_U32)inFields[8];
    configPtr.mplsCfg.exp1 = (GT_U32)inFields[9];
    configPtr.mplsCfg.label2 = (GT_U32)inFields[10];
    configPtr.mplsCfg.sBit2 = (GT_U32)inFields[11];
    configPtr.mplsCfg.exp2 = (GT_U32)inFields[12];

    configMaskPtr.mplsCfg.srcPortTrunk = (GT_U32)inFields[13];
    configMaskPtr.mplsCfg.srcIsTrunk = (GT_U32)inFields[14];
    configMaskPtr.mplsCfg.srcDev = (GT_U8)inFields[15];
    configMaskPtr.mplsCfg.vid = (GT_U16)inFields[16];
    galtisMacAddr(&configPtr.mplsCfg.macDa, (GT_U8*)inFields[17]);
    configMaskPtr.mplsCfg.label1 = (GT_U32)inFields[18];
    configMaskPtr.mplsCfg.sBit1 = (GT_U32)inFields[19];
    configMaskPtr.mplsCfg.exp1 = (GT_U32)inFields[20];
    configMaskPtr.mplsCfg.label2 = (GT_U32)inFields[21];
    configMaskPtr.mplsCfg.sBit2 = (GT_U32)inFields[22];
    configMaskPtr.mplsCfg.exp2 = (GT_U32)inFields[23];

    actionRetrieve(inFields, 24, &actionPtr);

    /* call cpss api function */
    result = cpssDxChTunnelTermEntrySet(devNum, routerTunnelTermTcamIndex,
                                                  tunnelType, &configPtr,
                                              &configMaskPtr, &actionPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelTermEntryGet
*
* DESCRIPTION:
*       Get tunnel termination entry from hardware at a given index.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       validPtr      - points to whether this tunnel termination entry is valid
*       tunnelTypePtr - points to the type of the tunnel; valid options:
*                       CPSS_TUNNEL_IPV4_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E
*                       CPSS_TUNNEL_IPV6_OVER_MPLS_E
*                       CPSS_TUNNEL_ETHERNET_OVER_MPLS_E,
*                       CPSS_TUNNEL_INVALID_E
*       configPtr     - points to tunnel termination configuration
*       configMaskPtr - points to tunnel termination configuration mask. The
*                       mask is AND styled one. Mask bit's 0 means don't care
*                       bit (corresponding bit in the pattern is not used in
*                       the TCAM lookup). Mask bit's 1 means that corresponding
*                       bit in the pattern is used in the TCAM lookup.
*       actionPtr     - points to tunnel termination action
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_BAD_PTR        - on NULL pointer
*       GT_FAIL           - on failure.
*       GT_OUT_OF_RANGE   - parameter not in valid range.
*       GT_BAD_STATE      - on invalid tunnel type
*
* COMMENTS:
*       Tunnel termination entries and IP lookup entries both reside in
*       router / tunnel termination TCAM. The router / tunnel termination TCAM
*       contains 1K lines. Each line can hold:
*       - 1 tunnel termination entry
*       - 1 ipv6 addresses
*       - 5 ipv4 addresses
*       Indexes for entries that takes one full line (meaning tunnel termination
*       and ipv6 lookup address) range (0..1023); Indexes for other entires
*       range (0..5119); Those indexes are counted COLUMN BY COLUMN  meaning
*       indexes 0..1023 reside in the first column, indexes 1024..2047 reside
*       in the second column and so on.
*       Therefore, tunnel termination entry at index n share the same TCAM line
*       with ipv6 lookup address at index n and share the same TCAM line with
*       ipv4 lookup addresses at index n, 1024+n, 2048+n, 3072+n and 4096+n.
*       For example, tunnel termination entry at TCAM line 100 share the
*       same TCAM line with ipv6 lookup address at line 100 and also share the
*       same TCAM line with ipv4 lookup addresses at indexes 100, 1124, 2148,
*       3172 and 4196.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelTermEntryGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_BOOL                         validPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    TunnelTermTcamIndexCnt = 0;

    do{
         /* call cpss api function */
        result = cpssDxChTunnelTermEntryGet(devNum, TunnelTermTcamIndexCnt,
                                            &validPtr, &tunnelType, &configPtr,
                                            &configMaskPtr, &actionPtr);

         if (result != GT_OK)
         {
             /* check end of table return code */
             if(GT_BAD_PARAM == result)
             {
                 /* the result is ok, this is end of table */
                 result = GT_OK;
             }

             galtisOutput(outArgs, result, "%d", -1);
             return CMD_OK;
          }

      TunnelTermTcamIndexCnt++;

    }while (!validPtr);


    /*
     * result is always GT_OK here,
     * validPtr is always GT_TRUE here
     */
    switch(tunnelType)
    {
    case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:

        inArgs[1] = 0;

        inFields[0] = TunnelTermTcamIndexCnt - 1;
        inFields[1] = tunnelType;

        inFields[2] = configPtr.ipv4Cfg.srcPortTrunk;
        inFields[3] = configPtr.ipv4Cfg.srcIsTrunk;
        inFields[4] = configPtr.ipv4Cfg.srcDev;
        inFields[5] = configPtr.ipv4Cfg.vid;

        inFields[9] = configMaskPtr.ipv4Cfg.srcPortTrunk;
        inFields[10] = configMaskPtr.ipv4Cfg.srcIsTrunk;
        inFields[11] = configMaskPtr.ipv4Cfg.srcDev;
        inFields[12] = configMaskPtr.ipv4Cfg.vid;

        inFields[16] = actionPtr.command;
        inFields[17] = actionPtr.userDefinedCpuCode;
        inFields[18] = actionPtr.passengerPacketType;
        inFields[19] = actionPtr.egressInterface.type;

        switch(actionPtr.egressInterface.type)
        {
        case 0:
            inFields[20] = actionPtr.egressInterface.devPort.hwDevNum;
            inFields[21] = actionPtr.egressInterface.devPort.portNum;
            break;
        case 1:
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr.egressInterface.trunkId);
            inFields[22] = actionPtr.egressInterface.trunkId;
            break;
        case 2:
            inFields[23] = actionPtr.egressInterface.vidx;
            break;
        case 3:
            inFields[24] = actionPtr.egressInterface.vlanId;
            break;
        default:
            break;
        }

        inFields[25] = actionPtr.isTunnelStart;
        inFields[26] = actionPtr.tunnelStartIdx;
        inFields[27] = actionPtr.vlanId;
        inFields[28] = actionPtr.vlanPrecedence;
        inFields[29] = actionPtr.nestedVlanEnable;
        inFields[30] = actionPtr.copyTtlFromTunnelHeader;
        inFields[31] = actionPtr.qosMode;
        inFields[32] = actionPtr.qosPrecedence;
        inFields[33] = actionPtr.qosProfile;
        inFields[34] = actionPtr.defaultUP;
        inFields[35] = actionPtr.modifyUP;
        inFields[36] = actionPtr.remapDSCP;
        inFields[37] = actionPtr.modifyDSCP;
        inFields[38] = actionPtr.mirrorToIngressAnalyzerEnable;
        inFields[39] = actionPtr.policerEnable;
        inFields[40] = actionPtr.policerIndex;
        inFields[41] = actionPtr.matchCounterEnable;
        inFields[42] = actionPtr.matchCounterIndex;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%4b%4b%d%d%d%d%6b%4b%4b",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              configPtr.ipv4Cfg.macDa.arEther,
                              configPtr.ipv4Cfg.srcIp.arIP,
                              configPtr.ipv4Cfg.destIp.arIP,
                              inFields[9],  inFields[10], inFields[11],
                              inFields[12],
                              configMaskPtr.ipv4Cfg.macDa.arEther,
                              configMaskPtr.ipv4Cfg.srcIp.arIP,
                              configMaskPtr.ipv4Cfg.destIp.arIP,
                              inFields[16], inFields[17], inFields[18],
                              inFields[19], inFields[20], inFields[21],
                              inFields[22], inFields[23], inFields[24],
                              inFields[25], inFields[26], inFields[27],
                              inFields[28], inFields[29], inFields[30],
                              inFields[31], inFields[32], inFields[33],
                              inFields[34], inFields[35], inFields[36],
                              inFields[37], inFields[38], inFields[39],
                              inFields[40], inFields[41], inFields[42]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 0);
        break;
    case CPSS_TUNNEL_IP_OVER_MPLS_E:
    case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:

        inArgs[1] = 1;

        inFields[0] = TunnelTermTcamIndexCnt - 1;
        inFields[1] = tunnelType;

        inFields[2] = configPtr.mplsCfg.srcPortTrunk;
        inFields[3] = configPtr.mplsCfg.srcIsTrunk;
        inFields[4] = configPtr.mplsCfg.srcDev;
        inFields[5] = configPtr.mplsCfg.vid;

        inFields[7] = configPtr.mplsCfg.label1;
        inFields[8] = configPtr.mplsCfg.sBit1;
        inFields[9] = configPtr.mplsCfg.exp1;
        inFields[10] = configPtr.mplsCfg.label2;
        inFields[11] = configPtr.mplsCfg.sBit2;
        inFields[12] = configPtr.mplsCfg.exp2;

        inFields[13] = configMaskPtr.mplsCfg.srcPortTrunk;
        inFields[14] = configMaskPtr.mplsCfg.srcIsTrunk;
        inFields[15] = configMaskPtr.mplsCfg.srcDev;
        inFields[16] = configMaskPtr.mplsCfg.vid;

        inFields[18] = configMaskPtr.mplsCfg.label1;
        inFields[19] = configMaskPtr.mplsCfg.sBit1;
        inFields[20] = configMaskPtr.mplsCfg.exp1;
        inFields[21] = configMaskPtr.mplsCfg.label2;
        inFields[22] = configMaskPtr.mplsCfg.sBit2;
        inFields[23] = configMaskPtr.mplsCfg.exp2;

        inFields[24] = actionPtr.command;
        inFields[25] = actionPtr.userDefinedCpuCode;
        inFields[26] = actionPtr.passengerPacketType;
        inFields[27] = actionPtr.egressInterface.type;

        switch(actionPtr.egressInterface.type)
        {
        case 0:
            inFields[28] = actionPtr.egressInterface.devPort.hwDevNum;
            inFields[29] = actionPtr.egressInterface.devPort.portNum;
            break;
        case 1:
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr.egressInterface.trunkId);
            inFields[30] = actionPtr.egressInterface.trunkId;
            break;
        case 2:
            inFields[31] = actionPtr.egressInterface.vidx;
            break;
        case 3:
            inFields[32] = actionPtr.egressInterface.vlanId;
            break;
        default:
            break;
        }

        inFields[33] = actionPtr.isTunnelStart;
        inFields[34] = actionPtr.tunnelStartIdx;
        inFields[35] = actionPtr.vlanId;
        inFields[36] = actionPtr.vlanPrecedence;
        inFields[37] = actionPtr.nestedVlanEnable;
        inFields[38] = actionPtr.copyTtlFromTunnelHeader;
        inFields[39] = actionPtr.qosMode;
        inFields[40] = actionPtr.qosPrecedence;
        inFields[41] = actionPtr.qosProfile;
        inFields[42] = actionPtr.defaultUP;
        inFields[43] = actionPtr.modifyUP;
        inFields[44] = actionPtr.remapDSCP;
        inFields[45] = actionPtr.modifyDSCP;
        inFields[46] = actionPtr.mirrorToIngressAnalyzerEnable;
        inFields[47] = actionPtr.policerEnable;
        inFields[48] = actionPtr.policerIndex;
        inFields[49] = actionPtr.matchCounterEnable;
        inFields[50] = actionPtr.matchCounterIndex;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%6b%d%d%d%d%d%d%d\
                     %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              configPtr.mplsCfg.macDa.arEther,
                              inFields[7],  inFields[8],  inFields[9],
                              inFields[10], inFields[11], inFields[12],
                              inFields[13], inFields[14], inFields[15],
                              inFields[16],
                              configMaskPtr.mplsCfg.macDa.arEther,
                              inFields[18], inFields[19], inFields[20],
                              inFields[21], inFields[22], inFields[23],
                              inFields[24], inFields[25], inFields[26],
                              inFields[27], inFields[28], inFields[29],
                              inFields[30], inFields[31], inFields[32],
                              inFields[33], inFields[34], inFields[35],
                              inFields[36], inFields[37], inFields[38],
                              inFields[39], inFields[40], inFields[41],
                              inFields[42], inFields[43], inFields[44],
                              inFields[45], inFields[46], inFields[47],
                              inFields[48], inFields[49], inFields[50]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 1);
        break;
    default:
        break;
    }

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelTermEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_BOOL                         validPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    TunnelTermTcamIndexCnt++;

    if(TunnelTermTcamIndexCnt > 1023)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    do{
        result = cpssDxChTunnelTermEntryGet(devNum, TunnelTermTcamIndexCnt,
                                            &validPtr, &tunnelType, &configPtr,
                                            &configMaskPtr, &actionPtr);

        if (result != GT_OK)
        {
            /* check end of table return code */
            if(GT_BAD_PARAM == result)
            {
                /* the result is ok, this is end of table */
                result = GT_OK;
            }

            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
         }


        TunnelTermTcamIndexCnt++;

    }while ((!validPtr));


    /*
     * result is always GT_OK here,
     * validPtr is always GT_TRUE here
     */
    switch(tunnelType)
    {
    case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
    case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:

        inArgs[1] = 0;

        inFields[0] = TunnelTermTcamIndexCnt - 1;
        inFields[1] = tunnelType;

        inFields[2] = configPtr.ipv4Cfg.srcPortTrunk;
        inFields[3] = configPtr.ipv4Cfg.srcIsTrunk;
        inFields[4] = configPtr.ipv4Cfg.srcDev;
        inFields[5] = configPtr.ipv4Cfg.vid;

        inFields[9] = configMaskPtr.ipv4Cfg.srcPortTrunk;
        inFields[10] = configMaskPtr.ipv4Cfg.srcIsTrunk;
        inFields[11] = configMaskPtr.ipv4Cfg.srcDev;
        inFields[12] = configMaskPtr.ipv4Cfg.vid;

        inFields[16] = actionPtr.command;
        inFields[17] = actionPtr.userDefinedCpuCode;
        inFields[18] = actionPtr.passengerPacketType;
        inFields[19] = actionPtr.egressInterface.type;

        switch(actionPtr.egressInterface.type)
        {
        case 0:
            inFields[20] = actionPtr.egressInterface.devPort.hwDevNum;
            inFields[21] = actionPtr.egressInterface.devPort.portNum;
            break;
        case 1:
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr.egressInterface.trunkId);
            inFields[22] = actionPtr.egressInterface.trunkId;
            break;
        case 2:
            inFields[23] = actionPtr.egressInterface.vidx;
            break;
        case 3:
            inFields[24] = actionPtr.egressInterface.vlanId;
            break;
        default:
            break;
        }

        inFields[25] = actionPtr.isTunnelStart;
        inFields[26] = actionPtr.tunnelStartIdx;
        inFields[27] = actionPtr.vlanId;
        inFields[28] = actionPtr.vlanPrecedence;
        inFields[29] = actionPtr.nestedVlanEnable;
        inFields[30] = actionPtr.copyTtlFromTunnelHeader;
        inFields[31] = actionPtr.qosMode;
        inFields[32] = actionPtr.qosPrecedence;
        inFields[33] = actionPtr.qosProfile;
        inFields[34] = actionPtr.defaultUP;
        inFields[35] = actionPtr.modifyUP;
        inFields[36] = actionPtr.remapDSCP;
        inFields[37] = actionPtr.modifyDSCP;
        inFields[38] = actionPtr.mirrorToIngressAnalyzerEnable;
        inFields[39] = actionPtr.policerEnable;
        inFields[40] = actionPtr.policerIndex;
        inFields[41] = actionPtr.matchCounterEnable;
        inFields[42] = actionPtr.matchCounterIndex;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%4b%4b%d%d%d%d%6b%4b%4b",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              configPtr.ipv4Cfg.macDa.arEther,
                              configPtr.ipv4Cfg.srcIp.arIP,
                              configPtr.ipv4Cfg.destIp.arIP,
                              inFields[9],  inFields[10], inFields[11],
                              inFields[12],
                              configMaskPtr.ipv4Cfg.macDa.arEther,
                              configMaskPtr.ipv4Cfg.srcIp.arIP,
                              configMaskPtr.ipv4Cfg.destIp.arIP,
                              inFields[16], inFields[17], inFields[18],
                              inFields[19], inFields[20], inFields[21],
                              inFields[22], inFields[23], inFields[24],
                              inFields[25], inFields[26], inFields[27],
                              inFields[28], inFields[29], inFields[30],
                              inFields[31], inFields[32], inFields[33],
                              inFields[34], inFields[35], inFields[36],
                              inFields[37], inFields[38], inFields[39],
                              inFields[40], inFields[41], inFields[42]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 0);
        break;
    case CPSS_TUNNEL_IP_OVER_MPLS_E:
    case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:

        inArgs[1] = 1;

        inFields[0] = TunnelTermTcamIndexCnt - 1;
        inFields[1] = tunnelType;

        inFields[2] = configPtr.mplsCfg.srcPortTrunk;
        inFields[3] = configPtr.mplsCfg.srcIsTrunk;
        inFields[4] = configPtr.mplsCfg.srcDev;
        inFields[5] = configPtr.mplsCfg.vid;

        inFields[7] = configPtr.mplsCfg.label1;
        inFields[8] = configPtr.mplsCfg.sBit1;
        inFields[9] = configPtr.mplsCfg.exp1;
        inFields[10] = configPtr.mplsCfg.label2;
        inFields[11] = configPtr.mplsCfg.sBit2;
        inFields[12] = configPtr.mplsCfg.exp2;

        inFields[13] = configMaskPtr.mplsCfg.srcPortTrunk;
        inFields[14] = configMaskPtr.mplsCfg.srcIsTrunk;
        inFields[15] = configMaskPtr.mplsCfg.srcDev;
        inFields[16] = configMaskPtr.mplsCfg.vid;

        inFields[18] = configMaskPtr.mplsCfg.label1;
        inFields[19] = configMaskPtr.mplsCfg.sBit1;
        inFields[20] = configMaskPtr.mplsCfg.exp1;
        inFields[21] = configMaskPtr.mplsCfg.label2;
        inFields[22] = configMaskPtr.mplsCfg.sBit2;
        inFields[23] = configMaskPtr.mplsCfg.exp2;

        inFields[24] = actionPtr.command;
        inFields[25] = actionPtr.userDefinedCpuCode;
        inFields[26] = actionPtr.passengerPacketType;
        inFields[27] = actionPtr.egressInterface.type;

        switch(actionPtr.egressInterface.type)
        {
        case 0:
            inFields[28] = actionPtr.egressInterface.devPort.hwDevNum;
            inFields[29] = actionPtr.egressInterface.devPort.portNum;
            break;
        case 1:
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr.egressInterface.trunkId);
            inFields[30] = actionPtr.egressInterface.trunkId;
            break;
        case 2:
            inFields[31] = actionPtr.egressInterface.vidx;
            break;
        case 3:
            inFields[32] = actionPtr.egressInterface.vlanId;
            break;
        default:
            break;
        }

        inFields[33] = actionPtr.isTunnelStart;
        inFields[34] = actionPtr.tunnelStartIdx;
        inFields[35] = actionPtr.vlanId;
        inFields[36] = actionPtr.vlanPrecedence;
        inFields[37] = actionPtr.nestedVlanEnable;
        inFields[38] = actionPtr.copyTtlFromTunnelHeader;
        inFields[39] = actionPtr.qosMode;
        inFields[40] = actionPtr.qosPrecedence;
        inFields[41] = actionPtr.qosProfile;
        inFields[42] = actionPtr.defaultUP;
        inFields[43] = actionPtr.modifyUP;
        inFields[44] = actionPtr.remapDSCP;
        inFields[45] = actionPtr.modifyDSCP;
        inFields[46] = actionPtr.mirrorToIngressAnalyzerEnable;
        inFields[47] = actionPtr.policerEnable;
        inFields[48] = actionPtr.policerIndex;
        inFields[49] = actionPtr.matchCounterEnable;
        inFields[50] = actionPtr.matchCounterIndex;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%6b%d%d%d%d%d%d%d\
                     %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                              inFields[0],  inFields[1],  inFields[2],
                              inFields[3],  inFields[4],  inFields[5],
                              configPtr.mplsCfg.macDa.arEther,
                              inFields[7],  inFields[8],  inFields[9],
                              inFields[10], inFields[11], inFields[12],
                              inFields[13], inFields[14], inFields[15],
                              inFields[16],
                              configMaskPtr.mplsCfg.macDa.arEther,
                              inFields[18], inFields[19], inFields[20],
                              inFields[21], inFields[22], inFields[23],
                              inFields[24], inFields[25], inFields[26],
                              inFields[27], inFields[28], inFields[29],
                              inFields[30], inFields[31], inFields[32],
                              inFields[33], inFields[34], inFields[35],
                              inFields[36], inFields[37], inFields[38],
                              inFields[39], inFields[40], inFields[41],
                              inFields[42], inFields[43], inFields[44],
                              inFields[45], inFields[46], inFields[47],
                              inFields[48], inFields[49], inFields[50]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 1);
        break;
    default:
        break;
    }

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelTermEntryInvalidate
*
* DESCRIPTION:
*       Invalidate a tunnel termination entry.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       routerTunnelTermTcamIndex  - index for the tunnel termination entry in
*                       the router / tunnel termination TCAM
*                       (APPLICABLE RANGES: 0...1023)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - on bad parameter.
*       GT_FAIL           - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelTermEntryInvalidate

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             routerTunnelTermTcamIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerTunnelTermTcamIndex = (GT_U32)inFields[0];

    /* call cpss api function */
    result = cpssDxChTunnelTermEntryInvalidate(devNum,
                            routerTunnelTermTcamIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringSet
*
* DESCRIPTION:
*       Set globally whether to subject Tunnel Start packets to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameter.
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEgressFilteringSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTunnelStartEgressFilteringSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEgressFilteringGet
*
* DESCRIPTION:
*       Get if Tunnel Start packets are globally subject to egress VLAN
*       filtering and to egress Spanning Tree filtering.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameter.
*       GT_BAD_PTR      - on NULL pointer
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEgressFilteringGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_BOOL                       enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTunnelStartEgressFilteringGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelTermExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelTermExceptionCmdSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT     exceptionType;
    CPSS_PACKET_CMD_ENT                     command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT)inArgs[1];
    command = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTunnelTermExceptionCmdSet(devNum, exceptionType, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelTermExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PTR  - on NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TUNNEL_TERM_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelCtrlTtExceptionCmdGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT     exceptionType;
    CPSS_PACKET_CMD_ENT                     commandPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TUNNEL_TERM_EXCEPTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTunnelCtrlTtExceptionCmdGet(devNum, exceptionType,
                                                           &commandPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", commandPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingSet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*       enable      - enable / disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameter.
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChEthernetOverMplsTunnelStartTaggingSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChEthernetOverMplsTunnelStartTaggingSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTaggingGet
*
* DESCRIPTION:
*       Set the global Tagging state for the Ethernet passenger packet to
*       be Tagged or Untagged.
*       The Ethernet passenger packet may have a VLAN tag added, removed,
*       or modified prior to its Tunnel Start encapsulation.
*       The Ethernet passenger packet is treated according to the following
*       modification modes:
*         - Transmit the passenger packet without any modifications to its VLAN
*           tagged state (i.e. if it arrived untagged, transmit untagged; if it
*           arrived tagged, transmit tagged)
*         - Transmit the passenger packet with an additional (nested) VLAN
*           tag regardless of whether it was received tagged or untagged
*         - Transmit the passenger packet tagged (i.e. if it arrived untagged,
*           a tag is added; if it arrived tagged it is transmitted tagged with
*           the new VID assignment)
*         - Transmit the passenger packet untagged (i.e. if it arrived tagged
*           it is transmitted untagged; if it arrived untagged it is
*           transmitted untagged)
*       Based on the global Ethernet passenger tag mode and the Ingress Policy
*       Action Nested VLAN Access mode, the following list indicates how the
*       Ethernet passenger is modified prior to its being encapsulated by the
*       tunnel header:
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 1
*                   Ethernet passenger tagging is not modified, regardless of
*                   whether it is tagged or untagged.
*         - <Tunnel Start tag> is Untagged & <Nested VLAN Access> = 0
*                   Transmit Untagged. NOTE: If the Ethernet passenger packet
*                   is tagged, the tag is removed. If the Ethernet passenger
*                   packet is untagged, the packet is not modified.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 1
*                   A new tag is added to the Ethernet passenger packet,
*                   regardless of whether it is tagged or untagged.
*         - <Tunnel Start tag> is Tagged & <Nested VLAN Access> = 0
*                   Transmit Tagged. NOTE: If the Ethernet passenger packet
*                   is untagged, a tag is added. If the Ethernet passenger
*                   packet is tagged, the existing tag VID is set to the
*                   packet VID assignment.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points to enable / disable
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on bad parameter.
*       GT_FAIL         - on error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChEthernetOverMplsTunnelStartTaggingGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_BOOL                       enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChEthernetOverMplsTunnelStartTaggingGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChTunnelStartEntrySet
*
* DESCRIPTION:
*       switch command for cpssDxChTunnelStart table
*       tunnel start entry configuration table.
*
* RETURNS:
*       GT_BAD_PARAM      - on bad parameter.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    cmdOsMemSet(&stConfigPtr , 0, sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));

    /* by amount of fields, the table resides only in this comment  */
    /* don't remove it                                              */
    /* 15 - wrCpssDxChTunnelStartEntrySet_IPV4_CONFIG               */
    /* 16 - wrCpssDxChTunnelStartEntrySet_MPLS_CONFIG               */

    /* switch by tunnelType in inFields[1]               */
    switch (inFields[1])
    {
        case CPSS_TUNNEL_X_OVER_IPV4_E:
        case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
            return wrCpssDxChTunnelStartEntrySet_IPV4_CONFIG(
                        inArgs, inFields ,numFields ,outArgs);
        case CPSS_TUNNEL_X_OVER_MPLS_E:
            return wrCpssDxChTunnelStartEntrySet_MPLS_CONFIG(
                        inArgs, inFields ,numFields ,outArgs);
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "");
            return CMD_AGENT_ERROR;
    }
}

/*******************************************************************************
* wrCpssDxChTunnelStart_1Set
*
* DESCRIPTION:
*       switch command for cpssDxChTunnelStart table
*       tunnel start entry configuration table.
*
* RETURNS:
*       GT_BAD_PARAM      - on bad parameter.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_1Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    cmdOsMemSet(&stConfigPtr , 0, sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));

    /* switch by tunnelType in inFields[1]               */
    switch (inFields[1])
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        return wrCpssDxChTunnelStart_1Set_IPV4_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        return wrCpssDxChTunnelStart_1Set_MPLS_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        return wrCpssDxChTunnelStart_1Set_MIM_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }
}

/*******************************************************************************
* wrCpssDxChTunnelStart_3Set
*
* DESCRIPTION:
*       switch command for cpssDxChTunnelStart table
*       tunnel start entry configuration table.
*
* RETURNS:
*       GT_BAD_PARAM      - on bad parameter.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_3Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    cmdOsMemSet(&stConfigPtr , 0, sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));

    /* switch by tunnelType in inFields[1]               */
    switch (inFields[1])
    {
    case CPSS_TUNNEL_X_OVER_IPV4_E:
    case CPSS_TUNNEL_X_OVER_GRE_IPV4_E:
        return wrCpssDxChTunnelStart_1Set_IPV4_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        return wrCpssDxChTunnelStart_3Set_MPLS_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        return wrCpssDxChTunnelStart_1Set_MIM_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }
}

/*******************************************************************************
*  cpssDxChTunnelTerm
*
* DESCRIPTION:
*       switch command for cpssDxChTunnelTerm table
*       tunnel term entry configuration table.
*
* RETURNS:
*       GT_BAD_PARAM      - on bad parameter.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelTermSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    cmdOsMemSet(&configPtr , 0, sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));
    cmdOsMemSet(&configMaskPtr , 0, sizeof(CPSS_DXCH_TUNNEL_TERM_CONFIG_UNT));

    /* by amount of fields, the table resides only in this comment  */
    /* don't remove it                                              */
    /* 43 - wrCpssDxChTunnelTermEntrySet_ipv4Cfg                    */
    /* 51 - wrCpssDxChTunnelTermEntrySet_mplsCfg                    */

    /* switch by tunnelType in inFields[1]                          */
    switch (inFields[1])
    {
        case CPSS_TUNNEL_IPV4_OVER_IPV4_E:
        case CPSS_TUNNEL_IPV4_OVER_GRE_IPV4_E:
        case CPSS_TUNNEL_IPV6_OVER_IPV4_E:
        case CPSS_TUNNEL_IPV6_OVER_GRE_IPV4_E:
            return wrCpssDxChTunnelTermEntrySet_ipv4Cfg(
                   inArgs, inFields ,numFields ,outArgs);
        case CPSS_TUNNEL_IP_OVER_MPLS_E:
        case CPSS_TUNNEL_ETHERNET_OVER_MPLS_E:
            return wrCpssDxChTunnelTermEntrySet_mplsCfg(
                   inArgs, inFields ,numFields ,outArgs);
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "");
            return CMD_AGENT_ERROR;
    }
}

/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableSet
*
* DESCRIPTION:
*       Controls Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum      - physical device number
*       enable      - GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartPassengerVlanTranslationEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;

    GT_U8                         devNum;
    GT_BOOL                       enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    result = cpssDxChTunnelStartPassengerVlanTranslationEnableSet (devNum, enable);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartPassengerVlanTranslationEnableGet
*
* DESCRIPTION:
*       Gets the Egress Vlan Translation of Ethernet tunnel start passengers.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       enablePtr   - points enable state.
*                     GT_TRUE: Ethernet tunnel start passengers are egress vlan
*                              translated, regardless of the VlanTranslationEnable
*                              configuration.
*                     GT_FALSE: Ethernet tunnel start passengers are to be egress
*                               vlan translated in accordance to the
*                               VlanTranslationEnable configuration.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartPassengerVlanTranslationEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    result = cpssDxChTunnelStartPassengerVlanTranslationEnableGet(devNum, &enable);

    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeSet
*
* DESCRIPTION:
*       Set the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum      - physical device number
*       tagMode     - tunnel start ethernet-over-x vlan tag mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChEthernetOverMplsTunnelStartTagModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tagMode = (CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChEthernetOverMplsTunnelStartTagModeSet(devNum, tagMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChEthernetOverMplsTunnelStartTagModeGet
*
* DESCRIPTION:
*       Get the vlan tag mode of the passanger packet for an
*       Ethernet-over-xxx tunnel start packet.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       tagModePtr  - pointer to tunnel start ethernet-over-x vlan tag mode
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_STATE             - on bad state.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_HW_ERROR              - on hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChEthernetOverMplsTunnelStartTagModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TUNNEL_START_ETHERNET_OVER_X_TAG_MODE_ENT tagMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChEthernetOverMplsTunnelStartTagModeGet(devNum, &tagMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tagMode);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
*
* DESCRIPTION:
*       This feature allows overriding the <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES: DxChXcat
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PHYSICAL_PORT_NUM               port;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port   = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
*
* DESCRIPTION:
*       The function gets status of the feature which allows overriding the
*       <total length> in the IP header.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       This API enables this feature per port.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES: DxChXcat
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable additional offset to tunnel total length
*                       GT_TRUE: Add offset to tunnel total length
*                       GT_FALSE: Don't add offset to tunnel total length
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;
    GT_U8                         devNum;
    GT_PHYSICAL_PORT_NUM          port;
    GT_BOOL                       enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port   = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet(devNum, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
*
* DESCRIPTION:
*       This API sets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES: DxChXcat
*
* INPUTS:
*       devNum            - device number
*       additionToLength  - Ip tunnel total length offset (6 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range values
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartIpTunnelTotalLengthOffsetSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             additionToLength;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum       = (GT_U8)inArgs[0];
    additionToLength = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet(devNum, additionToLength);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
*
* DESCRIPTION:
*       This API gets the value for <IP Tunnel Total Length Offset>.
*       When the egress port is enabled for this feature, then the new
*       <Total Length> is Old<Total Length> + <IP Tunnel Total Length Offset>.
*
*       For example: when sending Eth-Over-IPv4 to a port connected to MacSec Phy,
*       then total length of the tunnel header need to be increased by 4 bytes
*       because the MacSec adds additional 4 bytes to the passenger packet but
*       is unable to update the tunnel header alone.
*
* APPLICABLE DEVICES: DxChXcat
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       additionToLengthPtr  - (pointer to) Ip tunnel total length offset (6 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartIpTunnelTotalLengthOffsetGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;
    GT_U8                         devNum;
    GT_U32                        additionToLengthPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet(devNum, &additionToLengthPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", additionToLengthPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelPushEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS PW label push.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelPushEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelPushEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelPushEnableGet
*
* DESCRIPTION:
*       Returns if MPLS PW Label Push is enabled or disabled.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelPushEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelPushEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelSet
*
* DESCRIPTION:
*       Set the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       label       - MPLS label (20 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or label.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             label;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    label  = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelSet(devNum, portNum, label);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelGet
*
* DESCRIPTION:
*       Returns the MPLS PW label value to push incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       labelPtr    - (pointer to) MPLS label
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             label;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelGet(devNum, portNum, &label);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", label);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEntryExtensionSet
*
* DESCRIPTION:
*       Set tunnel start entry extension value.
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
*       tsExtension - tunnel start entry extension value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEntryExtensionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             tsExtension;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    tsExtension = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartEntryExtensionSet(devNum, portNum, tsExtension);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEntryExtensionGet
*
* DESCRIPTION:
*       Get tunnel start entry extension value.
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
*
* OUTPUTS:
*       tsExtensionPtr  - (pointer to) tunnel start entry extension value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEntryExtensionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             tsExtension;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartEntryExtensionGet(devNum, portNum, &tsExtension);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tsExtension);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartEgessVlanTableServiceIdSet
*
* DESCRIPTION:
*       Set egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
*       vlanServiceId - tunnel start entry extension value (APPLICABLE RANGES: 0...FFFFFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEgessVlanTableServiceIdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U16                             vlanId;
    GT_U32                             vlanServiceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];
    vlanServiceId = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTunnelStartEgessVlanTableServiceIdSet(devNum, vlanId, vlanServiceId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTunnelStartEgessVlanTableServiceIdGet
*
* DESCRIPTION:
*       Get egress vlan table service Id.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       vlanId        - VLAN id, used as index in the Egress Vlan Translation Table.
*                       (APPLICABLE RANGES:0..8191).
* OUTPUTS:
*       vlanServiceIdPtr  - (pointer to) vlan service Id.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartEgessVlanTableServiceIdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U16                             vlanId;
    GT_U32                             vlanServiceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTunnelStartEgessVlanTableServiceIdGet(devNum, vlanId, &vlanServiceId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vlanServiceId);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelEnableSet
*
* DESCRIPTION:
*       Enable/Disable MPLS flow label per port.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - enable/disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsFlowLabelEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsFlowLabelEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelEnableGet
*
* DESCRIPTION:
*       Returns if MPLS flow Label is enabled or disabled per port.
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsFlowLabelEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_BOOL                            enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsFlowLabelEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelTtlSet
*
* DESCRIPTION:
*       Set the MPLS flow label TTL
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       ttl         - the TTL value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsFlowLabelTtlSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U8                              ttl;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ttl = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsFlowLabelTtlSet(devNum, ttl);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsFlowLabelTtlGet
*
* DESCRIPTION:
*       Get the MPLS flow label TTL
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) the TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsFlowLabelTtlGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U8                              ttl;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsFlowLabelTtlGet(devNum, &ttl);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ttl);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelExpSet
*
* DESCRIPTION:
*       Set the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       exp         - EXP value (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or exp.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelExpSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             exp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    exp  = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelExpSet(devNum, portNum, exp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelExpGet
*
* DESCRIPTION:
*       Returns the EXP value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       expPtr      - (pointer to) EXP value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelExpGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             exp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelExpGet(devNum, portNum, &exp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", exp);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelTtlSet
*
* DESCRIPTION:
*       Set the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       ttl         - TTL value (APPLICABLE RANGES: 0...255)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port or ttl.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelTtlSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             ttl;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    ttl = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelTtlSet(devNum, portNum, ttl);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartMplsPwLabelTtlGet
*
* DESCRIPTION:
*       Returns the TTL value to push to the PW label incase that <MPLS PW label push enable> = Enabled
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       ttlPtr      - (pointer to) TTL value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwLabelTtlGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             ttl;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwLabelTtlGet(devNum, portNum, &ttl);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ttl);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwControlWordSet
*
* DESCRIPTION:
*       Sets a Pseudo Wire control word
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*       value       - the PW word to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - the PW word index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwControlWordSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_U32                             index;
    GT_U32                             value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index  = (GT_U32)inArgs[1];
    value  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwControlWordSet(devNum, index, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartMplsPwControlWordGet
*
* DESCRIPTION:
*       Gets a Pseudo Wire control word
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*
* OUTPUTS:
*       valuePtr    - the read PW word
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - the PW word index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwControlWordGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                     result;
    GT_U8                         devNum;
    GT_U32                        index;
    GT_U32                        value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index  = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTunnelStartMplsPwControlWordGet(devNum, index, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwETreeEnableSet
*
* DESCRIPTION:
*       Enable/Disable E-TREE assignment for a specified PW Control Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*       enable      - enable/disable the E-TREE assignment
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwETreeEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    GT_U32      index;
    GT_BOOL     enable;
    GT_STATUS   result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    result = cpssDxChTunnelStartMplsPwETreeEnableSet(devNum, index, enable);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartMplsPwETreeEnableGet
*
* DESCRIPTION:
*       Return whether E-TREE assignment is enabled for a specified PW Control
*       Word.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       index       - the PW word index (APPLICABLE RANGES: 0...14)
*
* OUTPUTS:
*       enablePtr   - the E-TREE assignment enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or illegal PW word index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartMplsPwETreeEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    GT_U32      index;
    GT_BOOL     enable;
    GT_STATUS   result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    result = cpssDxChTunnelStartMplsPwETreeEnableGet(devNum, index, &enable);

    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTunnelStartHeaderTpidSelectSet
*
* DESCRIPTION:
*       Function sets index of TPID tunnel-start header
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum         - device number
*       portNum        - port number (CPU port is supported)
*       tpidEntryIndex - TPID table entry index (APPLICABLE RANGES: 0...7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_OUT_OF_RANGE          - illegal tpidEntryIndex (not 0-7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartHeaderTpidSelectSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             tpidEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    tpidEntryIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartHeaderTpidSelectSet(devNum, portNum, tpidEntryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChTunnelStartHeaderTpidSelectGet
*
* DESCRIPTION:
*       Function gets index of TPID tunnel-start header
*
* APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number (CPU port is supported)
*
* OUTPUTS:
*       tpidEntryIndexPtr - (pointer to) TPID table entry index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, portNum, ethMode
*       GT_BAD_PTR               - tpidEntryIndexPtr is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when a TS entry is processed (from ingress
*       pipe assignment or from egress ePort assignment).
*       Applicable to all TS types
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartHeaderTpidSelectGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PORT_NUM                        portNum;
    GT_U32                             tpidEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTunnelStartHeaderTpidSelectGet(devNum, portNum, &tpidEntryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tpidEntryIndex);
    return CMD_OK;
}



/*******************************************************************************
* wrCpssDxChTunnelStart_4Set_IPV4_CONFIG
*
* DESCRIPTION:
*       Set IPv4 tunnel start entry for cpssDxChTunnelStartEntrySet_4 table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4Set_IPV4_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.ipv4Cfg.tagEnable           = (GT_BOOL)inFields[2];
    stConfigPtr.ipv4Cfg.vlanId              = (GT_U16)inFields[3];
    stConfigPtr.ipv4Cfg.upMarkMode          = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.ipv4Cfg.up                  = (GT_U32)inFields[5];
    stConfigPtr.ipv4Cfg.dscpMarkMode        = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[6];
    stConfigPtr.ipv4Cfg.dscp                = (GT_U32)inFields[7];
    galtisMacAddr(&stConfigPtr.ipv4Cfg.macDa,(GT_U8*)inFields[8]);
    stConfigPtr.ipv4Cfg.dontFragmentFlag    = (GT_BOOL)inFields[9];
    stConfigPtr.ipv4Cfg.ttl                 = (GT_U32)inFields[10];
    stConfigPtr.ipv4Cfg.autoTunnel          = (GT_BOOL)inFields[11];
    stConfigPtr.ipv4Cfg.autoTunnelOffset    = (GT_U32)inFields[12];
    galtisIpAddr(&stConfigPtr.ipv4Cfg.destIp,(GT_U8*)inFields[13]);
    galtisIpAddr(&stConfigPtr.ipv4Cfg.srcIp,(GT_U8*)inFields[14]);
    stConfigPtr.ipv4Cfg.cfi                 = (GT_U32)inFields[15];
    stConfigPtr.ipv4Cfg.retainCRC           = (GT_BOOL)inFields[16];
    stConfigPtr.ipv4Cfg.ipHeaderProtocol    = (CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT)inFields[17];
    stConfigPtr.ipv4Cfg.profileIndex        = (GT_U32)inFields[18];
    stConfigPtr.ipv4Cfg.greProtocolForEthernet  = (GT_U32)inFields[19];
    stConfigPtr.ipv4Cfg.greFlagsAndVersion      = (GT_U32)inFields[20];
    stConfigPtr.ipv4Cfg.udpDstPort          = (GT_U32)inFields[21];
    stConfigPtr.ipv4Cfg.udpSrcPort          = (GT_U32)inFields[22];


    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChTunnelStart_4Set_IPV6_CONFIG
*
* DESCRIPTION:
*       Set IPv4 tunnel start entry for cpssDxChTunnelStartEntrySet_4 table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4Set_IPV6_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.ipv6Cfg.tagEnable           = (GT_BOOL)inFields[2];
    stConfigPtr.ipv6Cfg.vlanId              = (GT_U16)inFields[3];
    stConfigPtr.ipv6Cfg.upMarkMode          = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.ipv6Cfg.up                  = (GT_U32)inFields[5];
    stConfigPtr.ipv6Cfg.dscpMarkMode        = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[6];
    stConfigPtr.ipv6Cfg.dscp                = (GT_U32)inFields[7];
    galtisMacAddr(&stConfigPtr.ipv6Cfg.macDa,(GT_U8*)inFields[8]);

    stConfigPtr.ipv6Cfg.ttl                 = (GT_U32)inFields[9];

    galtisIpv6Addr(&stConfigPtr.ipv6Cfg.destIp,(GT_U8*)inFields[10]);
    galtisIpv6Addr(&stConfigPtr.ipv6Cfg.srcIp,(GT_U8*)inFields[11]);

    stConfigPtr.ipv6Cfg.retainCRC           = (GT_BOOL)inFields[12];
    stConfigPtr.ipv6Cfg.ipHeaderProtocol    = (CPSS_DXCH_TUNNEL_START_IP_HEADER_PROTOCOL_ENT)inFields[13];
    stConfigPtr.ipv6Cfg.profileIndex        = (GT_U32)inFields[14];
    stConfigPtr.ipv6Cfg.greProtocolForEthernet  = (GT_U32)inFields[15];
    stConfigPtr.ipv6Cfg.greFlagsAndVersion  = (GT_U32)inFields[16];

    stConfigPtr.ipv6Cfg.udpDstPort          = (GT_U32)inFields[17];
    stConfigPtr.ipv6Cfg.udpSrcPort          = (GT_U32)inFields[18];
    stConfigPtr.ipv6Cfg.flowLabelMode       = (CPSS_DXCH_TUNNEL_START_FLOW_LABEL_ASSIGN_MODE_ENT)inFields[19];
    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* wrCpssDxChTunnelStart_4Set_MIM_CONFIG
*
* DESCRIPTION:
*       Set MIM tunnel start entry for cpssDxChTunnelStartEntrySet_4 table.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4Set_MIM_CONFIG
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    GT_U32                       routerArpTunnelStartLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerArpTunnelStartLineIndex = (GT_U32)inFields[0];
    tunnelType = (CPSS_TUNNEL_TYPE_ENT)inFields[1];

    stConfigPtr.mimCfg.tagEnable       = (GT_BOOL)inFields[2];
    stConfigPtr.mimCfg.vlanId          = (GT_U16)inFields[3];
    stConfigPtr.mimCfg.upMarkMode      = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[4];
    stConfigPtr.mimCfg.up              = (GT_U32)inFields[5];
    /* mimCfg.ttl field was removed from the API and will be ignored */
    galtisMacAddr(&stConfigPtr.mimCfg.macDa, (GT_U8*)inFields[6]);
    stConfigPtr.mimCfg.retainCrc       = (GT_BOOL)inFields[7];
    stConfigPtr.mimCfg.iSid            = (GT_U32)inFields[8];
    /* mimCfg.iSidAssignMode field was removed from the API and will be ignored */
    stConfigPtr.mimCfg.iUp             = (GT_U32)inFields[9];
    stConfigPtr.mimCfg.iUpMarkMode     = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[10];
    stConfigPtr.mimCfg.iDp             = (GT_U32)inFields[11];
    stConfigPtr.mimCfg.iDpMarkMode     = (CPSS_DXCH_TUNNEL_START_QOS_MARK_MODE_ENT)inFields[12];
    stConfigPtr.mimCfg.iTagReserved    = (GT_U32)inFields[13];
    stConfigPtr.mimCfg.iSidAssignMode  = (CPSS_DXCH_TUNNEL_START_MIM_I_SID_ASSIGN_MODE_ENT)inFields[14];
    stConfigPtr.mimCfg.bDaAssignMode   = (CPSS_DXCH_TUNNEL_START_MIM_B_DA_ASSIGN_MODE_ENT)inFields[15];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntrySet(devNum, routerArpTunnelStartLineIndex,
                                                      tunnelType, &stConfigPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTunnelStart_4Set
*
* DESCRIPTION:
*       switch command for cpssDxChTunnelStart table
*       tunnel start entry configuration table.
*
* RETURNS:
*       GT_BAD_PARAM      - on bad parameter.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    cmdOsMemSet(&stConfigPtr , 0, sizeof(CPSS_DXCH_TUNNEL_START_CONFIG_UNT));

    /* switch by tunnelType in inFields[1]               */
    switch (inFields[1])
    {
    case CPSS_TUNNEL_GENERIC_IPV4_E:
        return wrCpssDxChTunnelStart_4Set_IPV4_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    case CPSS_TUNNEL_X_OVER_MPLS_E:
        return wrCpssDxChTunnelStart_4Set_MPLS_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    case CPSS_TUNNEL_MAC_IN_MAC_E:
        return wrCpssDxChTunnelStart_4Set_MIM_CONFIG(
            inArgs, inFields ,numFields ,outArgs);

    case CPSS_TUNNEL_GENERIC_IPV6_E:
        return wrCpssDxChTunnelStart_4Set_IPV6_CONFIG(
            inArgs, inFields ,numFields ,outArgs);


    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }
}



/*******************************************************************************
* wrCpssDxChTunnelStart_4GetEntry
*
* DESCRIPTION:
*       Get single tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4GetEntry

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     devNum;

    /* map input arguments to locals */
    devNum                  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartEntryGet(devNum, routerArpTunnelGetIndex,
                                               &tunnelType, &stConfigPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    switch(tunnelType)
    {
    case CPSS_TUNNEL_GENERIC_IPV4_E:

        inFields[0]  = routerArpTunnelGetIndex;
        inFields[1]  = tunnelType;

        inFields[2]  = stConfigPtr.ipv4Cfg.tagEnable;
        inFields[3]  = stConfigPtr.ipv4Cfg.vlanId;
        inFields[4]  = stConfigPtr.ipv4Cfg.upMarkMode;
        inFields[5]  = stConfigPtr.ipv4Cfg.up;
        inFields[6]  = stConfigPtr.ipv4Cfg.dscpMarkMode;
        inFields[7]  = stConfigPtr.ipv4Cfg.dscp;
        /*inFields[8]  = stConfigPtr.ipv4Cfg.macDa;*/
        inFields[9]  = stConfigPtr.ipv4Cfg.dontFragmentFlag;
        inFields[10] = stConfigPtr.ipv4Cfg.ttl;
        inFields[11] = stConfigPtr.ipv4Cfg.autoTunnel;
        inFields[12] = stConfigPtr.ipv4Cfg.autoTunnelOffset;
        /*inFields[13]  = stConfigPtr.ipv4Cfg.destIp;*/
        /*inFields[14]  = stConfigPtr.ipv4Cfg.srcIp;*/
        inFields[15] = stConfigPtr.ipv4Cfg.cfi;
        inFields[16] = stConfigPtr.ipv4Cfg.retainCRC;
        inFields[17] = stConfigPtr.ipv4Cfg.ipHeaderProtocol;
        inFields[18] = stConfigPtr.ipv4Cfg.profileIndex;
        inFields[19] = stConfigPtr.ipv4Cfg.greProtocolForEthernet;
        inFields[20] = stConfigPtr.ipv4Cfg.greFlagsAndVersion;
        inFields[21] = stConfigPtr.ipv4Cfg.udpDstPort;
        inFields[22] = stConfigPtr.ipv4Cfg.udpSrcPort;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%6b%d%d%d%d%4b%4b%d%d%d%d%d%d%d%d",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], inFields[5],  inFields[6],  inFields[7],
                    stConfigPtr.ipv4Cfg.macDa.arEther,
                    inFields[9], inFields[10], inFields[11], inFields[12],
                    stConfigPtr.ipv4Cfg.destIp.arIP,
                    stConfigPtr.ipv4Cfg.srcIp.arIP,
                    inFields[15],inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 0);
        break;

    case CPSS_TUNNEL_X_OVER_MPLS_E:

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2]  = stConfigPtr.mplsCfg.tagEnable;
        inFields[3]  = stConfigPtr.mplsCfg.vlanId;
        inFields[4]  = stConfigPtr.mplsCfg.upMarkMode;
        inFields[5]  = stConfigPtr.mplsCfg.up;
        /*inFields[6]  = stConfigPtr.mplsCfg.macDa;*/
        inFields[7]  = stConfigPtr.mplsCfg.numLabels;
        inFields[8]  = stConfigPtr.mplsCfg.ttl;
        inFields[9]  = stConfigPtr.mplsCfg.ttlMode;
        inFields[10] = stConfigPtr.mplsCfg.label1;
        inFields[11] = stConfigPtr.mplsCfg.exp1MarkMode;
        inFields[12] = stConfigPtr.mplsCfg.exp1;
        inFields[13] = stConfigPtr.mplsCfg.label2;
        inFields[14] = stConfigPtr.mplsCfg.exp2MarkMode;
        inFields[15] = stConfigPtr.mplsCfg.exp2;
        inFields[16] = stConfigPtr.mplsCfg.label3;
        inFields[17] = stConfigPtr.mplsCfg.exp3MarkMode;
        inFields[18] = stConfigPtr.mplsCfg.exp3;
        inFields[19] = stConfigPtr.mplsCfg.retainCRC;
        inFields[20] = stConfigPtr.mplsCfg.setSBit;
        inFields[21] = stConfigPtr.mplsCfg.cfi;
        inFields[22] = stConfigPtr.mplsCfg.controlWordEnable;
        inFields[23] = stConfigPtr.mplsCfg.controlWordIndex;
        inFields[24] = stConfigPtr.mplsCfg.mplsEthertypeSelect;
        /* reserve inFields[25] for MPLS PW EXP Marking Mode */
        inFields[25] = 0;
        inFields[26] = stConfigPtr.mplsCfg.pushEliAndElAfterLabel1;
        inFields[27] = stConfigPtr.mplsCfg.pushEliAndElAfterLabel2;
        inFields[28] = stConfigPtr.mplsCfg.pushEliAndElAfterLabel3;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    stConfigPtr.mplsCfg.macDa.arEther,
                    inFields[7],  inFields[8],  inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18],
                    inFields[19], inFields[20], inFields[21],
                    inFields[22], inFields[23], inFields[24],
                    inFields[25], inFields[26], inFields[27],
                    inFields[28]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 1);
        break;

    case CPSS_TUNNEL_MAC_IN_MAC_E:

        inFields[0] = routerArpTunnelGetIndex;
        inFields[1] = tunnelType;

        inFields[2]  = stConfigPtr.mimCfg.tagEnable;
        inFields[3]  = stConfigPtr.mimCfg.vlanId;
        inFields[4]  = stConfigPtr.mimCfg.upMarkMode;
        inFields[5]  = stConfigPtr.mimCfg.up;
        inFields[7]  = stConfigPtr.mimCfg.retainCrc;
        inFields[8]  = stConfigPtr.mimCfg.iSid;
        inFields[9] = stConfigPtr.mimCfg.iUp;
        inFields[10] = stConfigPtr.mimCfg.iUpMarkMode;
        inFields[11] = stConfigPtr.mimCfg.iDp;
        inFields[12] = stConfigPtr.mimCfg.iDpMarkMode;
        inFields[13] = stConfigPtr.mimCfg.iTagReserved;
        inFields[14] = stConfigPtr.mimCfg.iSidAssignMode;
        inFields[15] = stConfigPtr.mimCfg.bDaAssignMode;


        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],
                    inFields[3],  inFields[4],  inFields[5],
                    stConfigPtr.mimCfg.macDa.arEther,
                    inFields[7],  inFields[8],  inFields[9],
                    inFields[10], inFields[11], inFields[12],
                    inFields[13], inFields[14],inFields[15]);
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 2);
        break;

    case CPSS_TUNNEL_GENERIC_IPV6_E:

        inFields[0]  = routerArpTunnelGetIndex;
        inFields[1]  = tunnelType;

        inFields[2]  = stConfigPtr.ipv6Cfg.tagEnable;
        inFields[3]  = stConfigPtr.ipv6Cfg.vlanId;
        inFields[4]  = stConfigPtr.ipv6Cfg.upMarkMode;
        inFields[5]  = stConfigPtr.ipv6Cfg.up;
        inFields[6]  = stConfigPtr.ipv6Cfg.dscpMarkMode;
        inFields[7]  = stConfigPtr.ipv6Cfg.dscp;
        /*inFields[8]  = stConfigPtr.ipv4Cfg.macDa;*/
        inFields[9] = stConfigPtr.ipv6Cfg.ttl;
        /*inFields[10]  = stConfigPtr.ipv4Cfg.destIp;*/
        /*inFields[11]  = stConfigPtr.ipv4Cfg.srcIp;*/
        inFields[12] = stConfigPtr.ipv6Cfg.retainCRC;
        inFields[13] = stConfigPtr.ipv6Cfg.ipHeaderProtocol;
        inFields[14] = stConfigPtr.ipv6Cfg.profileIndex;
        inFields[15] = stConfigPtr.ipv6Cfg.greProtocolForEthernet;
        inFields[16] = stConfigPtr.ipv6Cfg.greFlagsAndVersion;
        inFields[17] = stConfigPtr.ipv6Cfg.udpDstPort;
        inFields[18] = stConfigPtr.ipv6Cfg.udpSrcPort;
        inFields[19] = stConfigPtr.ipv6Cfg.flowLabelMode;


        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%6b%d%16b%16b%d%d%d%d%d%d%d%d%",
                    inFields[0], inFields[1],  inFields[2],  inFields[3],
                    inFields[4], inFields[5],  inFields[6],  inFields[7],
                    stConfigPtr.ipv6Cfg.macDa.arEther,
                    inFields[9], stConfigPtr.ipv6Cfg.destIp.arIP,
                    stConfigPtr.ipv6Cfg.srcIp.arIP,
                    inFields[12],inFields[13], inFields[14],
                    inFields[15],inFields[16],inFields[17], inFields[18],inFields[19]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%f", 3);
        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        break;
    }

    return CMD_OK;
}



/*******************************************************************************
* wrCpssDxChTunnelStart_4GetFirst
*
* DESCRIPTION:
*       Get first tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4GetFirst

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

    /* map input arguments to locals */
    routerArpTunnelGetIndex = (GT_U32)inArgs[1];
    routerArpTunnelMaxGet   = routerArpTunnelGetIndex + (GT_U32)inArgs[2];

    if(routerArpTunnelGetIndex > 32767 ||
       routerArpTunnelGetIndex >= routerArpTunnelMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChTunnelStart_4GetEntry(
        inArgs, inFields ,numFields ,outArgs);
}

/*******************************************************************************
* wrCpssDxChTunnelStart_4GetNext
*
* DESCRIPTION:
*       Get next tunnel start entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStart_4GetNext

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

    /* map input arguments to locals */
    routerArpTunnelGetIndex++;

    if(routerArpTunnelGetIndex > 32767 ||
       routerArpTunnelGetIndex >= routerArpTunnelMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChTunnelStart_4GetEntry(
        inArgs, inFields ,numFields ,outArgs);
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTunnelStartGenProfileTableEntrySet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    tunnelStartMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTunnelStartGenProfileTableEntrySet(devNum,
                                           profileIndex,
                                           profileDataPtr);
    }
    else
    {
        return cpssDxChTunnelStartPortGroupGenProfileTableEntrySet(devNum,
                                                    pgBmp,
                                                    profileIndex,
                                                    profileDataPtr);
    }
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTunnelStartGenProfileTableEntryGet
(
    IN GT_U8                                            devNum,
    IN GT_U32                                           profileIndex,
    IN CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC        *profileDataPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    tunnelStartMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTunnelStartGenProfileTableEntryGet(devNum,
                                           profileIndex,
                                           profileDataPtr);
    }
    else
    {
        return cpssDxChTunnelStartPortGroupGenProfileTableEntryGet(devNum,
                                                    pgBmp,
                                                    profileIndex,
                                                    profileDataPtr);
    }
}


/*******************************************************************************
* wrCpssDxChTunnelStartGenProfileEntryByteSet
*
* DESCRIPTION:
*       configures Generic Tunnel-start Profile table entry.
*       It serves as logical extension to the Generic IPv4/6 Tunnel-Start
*       when the IPv4/6 tunnel header includes either a GRE or UDP header
*
* RETURNS:
*       GT_BAD_PARAM      - on bad parameter.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartGenProfileEntryByteSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                           result;
    GT_U8                                               devNum;
    GT_U32                                              profileIndex;
    CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC           profileData;
    GT_U8                                               byteNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    byteNum = (GT_U8)inArgs[2];

    cmdOsMemSet(&profileData , 0, sizeof(CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC));

    /* get profileDataPtr */
    result = pg_wrap_cpssDxChTunnelStartGenProfileTableEntryGet(devNum, profileIndex,
                                                                &profileData);

    profileData.templateDataSize = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_SIZE_ENT)inFields[0];
    profileData.hashShiftLeftBitsNumber = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[1];
    profileData.udpSrcPortMode = (CPSS_DXCH_TUNNEL_START_UDP_SRC_PORT_ASSIGN_MODE_ENT)inFields[2];
    profileData.templateDataBitsCfg[byteNum*8+7] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[3];
    profileData.templateDataBitsCfg[byteNum*8+6] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[4];
    profileData.templateDataBitsCfg[byteNum*8+5] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[5];
    profileData.templateDataBitsCfg[byteNum*8+4] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[6];
    profileData.templateDataBitsCfg[byteNum*8+3] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[7];
    profileData.templateDataBitsCfg[byteNum*8+2] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[8];
    profileData.templateDataBitsCfg[byteNum*8+1] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[9];
    profileData.templateDataBitsCfg[byteNum*8] = (CPSS_DXCH_TUNNEL_START_TEMPLATE_DATA_CFG_ENT)inFields[10];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartGenProfileTableEntrySet(devNum, profileIndex,
                                                                &profileData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}





/*******************************************************************************
* wrCpssDxChTunnelStartGenProfileEntryByteGetFirst
*
* DESCRIPTION:
*       Get first profile Data.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartGenProfileEntryByteGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     devNum;
    CPSS_DXCH_TUNNEL_START_GEN_IP_PROFILE_STC       profileData;
    GT_U8                     byteNum;
    GT_U32                    profileGetIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;




    /* map input arguments to locals */
    devNum                  = (GT_U8)inArgs[0];
    profileGetIndex = (GT_U32)inArgs[1];
    byteNum = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTunnelStartGenProfileTableEntryGet(devNum,profileGetIndex,
                                                                &profileData);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]  = profileData.templateDataSize;
    inFields[1]  = profileData.hashShiftLeftBitsNumber;
    inFields[2]  = profileData.udpSrcPortMode;
    inFields[3]  = profileData.templateDataBitsCfg[8*byteNum+7];
    inFields[4]  = profileData.templateDataBitsCfg[8*byteNum+6];
    inFields[5]  = profileData.templateDataBitsCfg[8*byteNum+5];
    inFields[6]  = profileData.templateDataBitsCfg[8*byteNum+4];
    inFields[7]  = profileData.templateDataBitsCfg[8*byteNum+3];
    inFields[8]  = profileData.templateDataBitsCfg[8*byteNum+2];
    inFields[9]  = profileData.templateDataBitsCfg[8*byteNum+1];
    inFields[10] = profileData.templateDataBitsCfg[8*byteNum];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], inFields[5],  inFields[6],  inFields[7],
                inFields[8], inFields[9], inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%f", 0);

    return CMD_OK;
}



/*******************************************************************************
* wrCpssDxChTunnelStartGenDymmyGetNext
*
* DESCRIPTION:
*   The function used to return "No Next Table Entry"
*
* COMMENTS: Generic
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTunnelStartGenDymmyGetNext
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

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTunnelStartSet",
        &wrCpssDxChTunnelStartEntrySet,
        4, 20},          /*  fields number is variable(see function) */

    {"cpssDxChTunnelStartGetFirst",
        &wrCpssDxChTunnelStartEntryGetFirst,
        4, 0},

    {"cpssDxChTunnelStartGetNext",
        &wrCpssDxChTunnelStartEntryGetNext,
        4, 0},

    {"cpssDxChTunnelStart_1Set",
        &wrCpssDxChTunnelStart_1Set,
        4, 30},          /*  fields number is variable(see function) */

    {"cpssDxChTunnelStart_1GetFirst",
        &wrCpssDxChTunnelStart_1GetFirst,
        4, 0},

    {"cpssDxChTunnelStart_1GetNext",
        &wrCpssDxChTunnelStart_1GetNext,
        4, 0},

    {"cpssDxChTunnelStart_3Set",
        &wrCpssDxChTunnelStart_3Set,
        4, 30},          /*  fields number is variable(see function) */

    {"cpssDxChTunnelStart_3GetFirst",
        &wrCpssDxChTunnelStart_3GetFirst,
        4, 0},

    {"cpssDxChTunnelStart_3GetNext",
        &wrCpssDxChTunnelStart_3GetNext,
        4, 0},

    {"cpssDxChTunnelStart_4Set",
        &wrCpssDxChTunnelStart_4Set,
        4, 29 },          /*  fields number is variable(see function) */

    {"cpssDxChTunnelStart_4GetFirst",
        &wrCpssDxChTunnelStart_4GetFirst,
        4, 0},

    {"cpssDxChTunnelStart_4GetNext",
        &wrCpssDxChTunnelStart_4GetNext,
        4, 0},

    {"cpssDxChIpv4TunnelTermPortSet",
        &wrCpssDxChIpv4TunnelTermPortSet,
        3, 0},

    {"cpssDxChIpv4TunnelTermPortGet",
        &wrCpssDxChIpv4TunnelTermPortGet,
        2, 0},

    {"cpssDxChMplsTunnelTermPortSet",
        &wrCpssDxChMplsTunnelTermPortSet,
        3, 0},

    {"cpssDxChMplsTunnelTermPortGet",
        &wrCpssDxChMplsTunnelTermPortGet,
        2, 0},

    {"cpssDxChTunnelTermSet",
        &wrCpssDxChTunnelTermSet,
        2, 70},          /*  fields number is variable(see function) */

    {"cpssDxChTunnelTermGetFirst",
        &wrCpssDxChTunnelTermEntryGetFirst,
        2, 0},

    {"cpssDxChTunnelTermGetNext",
        &wrCpssDxChTunnelTermEntryGetNext,
        2, 0},

    {"cpssDxChTunnelTermDelete",
        &wrCpssDxChTunnelTermEntryInvalidate,
        2, 1},

    {"cpssDxChTunnelStartEgressFilteringSet",
        &wrCpssDxChTunnelStartEgressFilteringSet,
        2, 0},

    {"cpssDxChTunnelStartEgressFilteringGet",
        &wrCpssDxChTunnelStartEgressFilteringGet,
        1, 0},

    {"cpssDxChTunnelTermExceptionCmdSet",
        &wrCpssDxChTunnelTermExceptionCmdSet,
        3, 0},

    {"cpssDxChTunnelTermExceptionCmdGet",
        &wrCpssDxChTunnelCtrlTtExceptionCmdGet,
        2, 0},

    {"cpssDxChEthernetOverMplsTunnelStartTaggingSet",
        &wrCpssDxChEthernetOverMplsTunnelStartTaggingSet,
        2, 0},

    {"cpssDxChEthernetOverMplsTunnelStartTaggingGet",
        &wrCpssDxChEthernetOverMplsTunnelStartTaggingGet,
        1, 0},

    {"cpssDxChTunnelStartPassengerVlanTransEnableSet",
        &wrCpssDxChTunnelStartPassengerVlanTranslationEnableSet,
        2, 0},

    {"cpssDxChTunnelStartPassengerVlanTransEnableGet",
        &wrCpssDxChTunnelStartPassengerVlanTranslationEnableGet,
        1, 0},

    {"cpssDxChEthernetOverMplsTunnelStartTagModeSet",
         &wrCpssDxChEthernetOverMplsTunnelStartTagModeSet,
         2, 0},

    {"cpssDxChEthernetOverMplsTunnelStartTagModeGet",
         &wrCpssDxChEthernetOverMplsTunnelStartTagModeGet,
         1, 0},

    {"cpssDxChTSportIpTunnelTotalLengthOffsetEnableSet",
        &wrCpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableSet,
        3, 0},

    {"cpssDxChTSportIpTunnelTotalLengthOffsetEnableGet",
        &wrCpssDxChTunnelStartPortIpTunnelTotalLengthOffsetEnableGet,
        2, 0},

    {"cpssDxChTunnelStartIpTunnelTotalLengthOffsetSet",
        &wrCpssDxChTunnelStartIpTunnelTotalLengthOffsetSet,
        2, 0},

    {"cpssDxChTunnelStartIpTunnelTotalLengthOffsetGet",
        &wrCpssDxChTunnelStartIpTunnelTotalLengthOffsetGet,
        1, 0},

    {"cpssDxChTunnelStartMplsPwLabelPushEnableSet",
        &wrCpssDxChTunnelStartMplsPwLabelPushEnableSet,
        3, 0},

    {"cpssDxChTunnelStartMplsPwLabelPushEnableGet",
        &wrCpssDxChTunnelStartMplsPwLabelPushEnableGet,
        2, 0},

    {"cpssDxChTunnelStartMplsPwLabelSet",
        &wrCpssDxChTunnelStartMplsPwLabelSet,
        3, 0},

    {"cpssDxChTunnelStartMplsPwLabelGet",
        &wrCpssDxChTunnelStartMplsPwLabelGet,
        2, 0},

    {"cpssDxChTunnelStartMplsFlowLabelEnableSet",
        &wrCpssDxChTunnelStartMplsFlowLabelEnableSet,
        3, 0},

    {"cpssDxChTunnelStartMplsFlowLabelEnableGet",
        &wrCpssDxChTunnelStartMplsFlowLabelEnableGet,
        2, 0},

    {"cpssDxChTunnelStartMplsFlowLabelTtlSet",
        &wrCpssDxChTunnelStartMplsFlowLabelTtlSet,
        2, 0},

    {"cpssDxChTunnelStartMplsFlowLabelTtlGet",
        &wrCpssDxChTunnelStartMplsFlowLabelTtlGet,
        1, 0},

    {"cpssDxChTunnelStartMplsPwLabelExpSet",
        &wrCpssDxChTunnelStartMplsPwLabelExpSet,
        3, 0},

    {"cpssDxChTunnelStartMplsPwLabelExpGet",
        &wrCpssDxChTunnelStartMplsPwLabelExpGet,
        2, 0},

    {"cpssDxChTunnelStartMplsPwLabelTtlSet",
        &wrCpssDxChTunnelStartMplsPwLabelTtlSet,
        3, 0},

    {"cpssDxChTunnelStartMplsPwLabelTtlGet",
        &wrCpssDxChTunnelStartMplsPwLabelTtlGet,
        2, 0},

    {"cpssDxChTunnelStartMplsPwControlWordSet",
        &wrCpssDxChTunnelStartMplsPwControlWordSet,
        3, 0},

    {"cpssDxChTunnelStartMplsPwControlWordGet",
        &wrCpssDxChTunnelStartMplsPwControlWordGet,
        2, 0},

    {"cpssDxChTunnelStartMplsPwETreeEnableSet",
        &wrCpssDxChTunnelStartMplsPwETreeEnableSet,
        3, 0},

    {"cpssDxChTunnelStartMplsPwETreeEnableGet",
        &wrCpssDxChTunnelStartMplsPwETreeEnableGet,
        2, 0},

    {"cpssDxChTunnelStartHeaderTpidSelectSet",
        &wrCpssDxChTunnelStartHeaderTpidSelectSet,
        3, 0},

    {"cpssDxChTunnelStartHeaderTpidSelectGet",
        &wrCpssDxChTunnelStartHeaderTpidSelectGet,
        2, 0},
    {"cpssDxChTunnelStartGenProfileEntryByteSet",
        &wrCpssDxChTunnelStartGenProfileEntryByteSet,
        3, 11},          /*  fields number is variable(see function) */
    {"cpssDxChTunnelStartGenProfileEntryByteGetFirst",
        &wrCpssDxChTunnelStartGenProfileEntryByteGetFirst,
        3, 0},
    {"cpssDxChTunnelStartGenProfileEntryByteGetNext",
        &wrCpssDxChTunnelStartGenDymmyGetNext,
        3, 0},
    {"cpssDxChTunnelStartEntryExtensionSet",
        &wrCpssDxChTunnelStartEntryExtensionSet,
        3, 0},
    {"cpssDxChTunnelStartEntryExtensionGet",
        &wrCpssDxChTunnelStartEntryExtensionGet,
        2, 0},
    {"cpssDxChTunnelStartEgessVlanTableServiceIdSet",
        &wrCpssDxChTunnelStartEgessVlanTableServiceIdSet,
        3, 0},
    {"cpssDxChTunnelStartEgessVlanTableServiceIdGet",
        &wrCpssDxChTunnelStartEgessVlanTableServiceIdGet,
        2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTunnel
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
GT_STATUS cmdLibInitCpssDxChTunnel
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


