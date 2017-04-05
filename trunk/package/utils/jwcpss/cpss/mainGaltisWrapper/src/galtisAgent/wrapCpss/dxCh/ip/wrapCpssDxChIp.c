/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChIp.c
*
* DESCRIPTION:
*       Wrapper functions for Cpss Dx Cheetah IP
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 48 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIp.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpNat.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfoEnhanced.h>
#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/dxCh/dxChxGen/cpssDxChTypes.h>


/********************Table: cpssDxChIpLtt****************/
static GT_U32 gLttTTIndex;
/*******************************************************************************
* cpssDxChIpLttWrite
*
* DESCRIPTION:
*    write a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum         - the device number.
*       lttTTIndex     - the entry's index in LTT table (equvalent to
*                        the router tcam index it is attched to.
*       lttEntryPtr    - the lookup translation table entry
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_OUT_OF_RANGE - lttTTIndex is bigger then max IP TCAM row.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       The Lookup translation table is shared with the Tunnel Termination (TT)
*       Table.
*       If the relevant index in the Router Tcam is in line used for tunnel
*       termination key then the indexes in the LTT/TT table will function as
*       Tunnel Termination Action, and you shouldn't write LTT entries in these
*       indexes. It works Vice-verse.
*       For Ipv6 Entries the relevant index is the line prefix (same index sent
*       as parameter to the ipv6 prefix set API). The other LTT/TT entries
*       related to that TCAM line are left unused.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLttWrite
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS                           status;
    GT_U32                              numOfLttRows;
    GT_U8                               dev;
    GT_U32                              lttTTIndex;
    CPSS_DXCH_IP_LTT_ENTRY_STC          lttEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];

    lttTTIndex = (GT_U32)inFields[0];
    lttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[1];
    lttEntry.numOfPaths = (GT_U32)inFields[2];
    lttEntry.routeEntryBaseIndex = (GT_U32)inFields[3];
    lttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[4];
    lttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[5];
    lttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[6];

    /* call cpss api function */
    if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        numOfLttRows = PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(dev);
        status = cpssDxChIpLttWrite(dev,
                                    lttTTIndex % numOfLttRows,
                                    lttTTIndex / numOfLttRows,
                                    &lttEntry);
    }
    else
    {
        status = cpssDxChIpLttWrite(dev,
                                    lttTTIndex / 4,
                                    lttTTIndex % 4,
                                    &lttEntry);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLttRead
*
* DESCRIPTION:
*    read a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number.
*       lttTTIndex      - the entry's index in LTT table (equvalent to
*                         the router tcam index it is attched to.
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - lttTTIndex is bigger then allowed.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       see cpssDxChIpLttWrite
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLttReadFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_LTT_ENTRY_STC          lttEntry;
    GT_U32                              numOfLttRows;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gLttTTIndex = 0;/*reset on first*/

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        numOfLttRows = PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(dev);
        result = cpssDxChIpLttRead(dev,
                                   gLttTTIndex % numOfLttRows,
                                   gLttTTIndex / numOfLttRows,
                                   &lttEntry);
    }
    else
    {
        result = cpssDxChIpLttRead(dev,
                                   gLttTTIndex / 4,
                                   gLttTTIndex % 4,
                                   &lttEntry);
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gLttTTIndex;
    inFields[1] = lttEntry.routeType;
    inFields[2] = lttEntry.numOfPaths;
    inFields[3] = lttEntry.routeEntryBaseIndex;
    inFields[4] = lttEntry.ucRPFCheckEnable;
    inFields[5] = lttEntry.sipSaCheckMismatchEnable;
    inFields[6] = lttEntry.ipv6MCGroupScopeLevel;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, result, "%f");

    gLttTTIndex++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLttRead
*
* DESCRIPTION:
*    read a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number.
*       lttTTIndex      - the entry's index in LTT table (equvalent to
*                         the router tcam index it is attched to.
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - lttTTIndex is bigger then allowed.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       see cpssDxChIpLttWrite
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLttReadNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_LTT_ENTRY_STC          lttEntry;
    GT_U32                              numOfLttRows;
    GT_U32                              numOfLttColumns;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    numOfLttRows = PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(dev);

    if(PRV_CPSS_PP_MAC(dev)->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
        numOfLttColumns = 5;
    else
        numOfLttColumns = 4;

    if(gLttTTIndex > numOfLttRows * numOfLttColumns)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        numOfLttRows = PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(dev);
        result = cpssDxChIpLttRead(dev,
                                   gLttTTIndex % numOfLttRows,
                                   gLttTTIndex / numOfLttRows,
                                   &lttEntry);
    }
    else
    {
        result = cpssDxChIpLttRead(dev,
                                   gLttTTIndex / 4,
                                   gLttTTIndex % 4,
                                   &lttEntry);
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gLttTTIndex;
    inFields[1] = lttEntry.routeType;
    inFields[2] = lttEntry.numOfPaths;
    inFields[3] = lttEntry.routeEntryBaseIndex;
    inFields[4] = lttEntry.ucRPFCheckEnable;
    inFields[5] = lttEntry.sipSaCheckMismatchEnable;
    inFields[6] = lttEntry.ipv6MCGroupScopeLevel;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, result, "%f");

    gLttTTIndex++;

    return CMD_OK;
}



/********************Table: cpssDxChIpLttExt****************/
static GT_U32   minRowLtt;
static GT_U32   maxRowLtt;
static GT_U32   minColumnLtt;
static GT_U32   maxColumnLtt;

static GT_U32   columnIndexLtt;
static GT_U32   rowIndexLtt;

/*******************************************************************************
* cpssDxChIpLttWrite
*
* DESCRIPTION:
*    Writes a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum        - the device number.
*       lttTtRow      - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtColumn   - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttEntryPtr   - the lookup translation table entry
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active or
*                         invalid ipv6MCGroupScopeLevel value.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       The Lookup translation table is shared with the Tunnel Termination (TT)
*       Table.
*       If the relevant index in the Router Tcam is in line used for tunnel
*       termination key then the indexes in the LTT/TT table will function as
*       Tunnel Termination Action, and you shouldn't write LTT entries in these
*       indexes. It works Vice-verse.
*       For Ipv6 Entries the relevant index is the line prefix (same index sent
*       as parameter to the ipv6 prefix set API). The other LTT/TT entries
*       related to that TCAM line are left unused.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLttExtWrite
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;
    GT_U32                          lttTtiRow;
    GT_U32                          lttTtColumn;
    GT_U8                           dev;

    CPSS_DXCH_IP_LTT_ENTRY_STC          lttEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];

    lttTtiRow = (GT_U32)inFields[0];
    lttTtColumn=(GT_U32)inFields[1];
    lttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[2];
    lttEntry.numOfPaths = (GT_U32)inFields[3];
    lttEntry.routeEntryBaseIndex = (GT_U32)inFields[4];
    lttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[5];
    lttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[6];
    lttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[7];

    /* call cpss api function */
    status =  cpssDxChIpLttWrite(dev, lttTtiRow, lttTtColumn, &lttEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLttRead
*
* DESCRIPTION:
*    Reads a LookUp Translation Table Entry.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number.
*       lttTtRow      - the entry's row index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*       lttTtColumn   - the entry's column index in LTT table (equivalent to
*                       the router tcam entry it is attached to)
*
* OUTPUTS:
*       lttEntryPtr     - the lookup translation table entry
*
* RETURNS:
*       GT_OK           - if succeeded
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       See cpssDxChIpLttWrite
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLttReadEntry
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_LTT_ENTRY_STC          lttEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(rowIndexLtt > maxRowLtt)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpLttRead(dev, rowIndexLtt, columnIndexLtt, &lttEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = rowIndexLtt;
    inFields[1] = columnIndexLtt;
    inFields[2] = lttEntry.routeType;
    inFields[3] = lttEntry.numOfPaths;
    inFields[4] = lttEntry.routeEntryBaseIndex;
    inFields[5] = lttEntry.ucRPFCheckEnable;
    inFields[6] = lttEntry.sipSaCheckMismatchEnable;
    inFields[7] = lttEntry.ipv6MCGroupScopeLevel;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6],inFields[7]);

    galtisOutput(outArgs, result, "%f");


    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssDxChIpLttExtReadFirst
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

    minRowLtt    = (GT_U32)inArgs[1];
    maxRowLtt    = (GT_U32)inArgs[2];
    minColumnLtt = (GT_U32)inArgs[3];
    maxColumnLtt = (GT_U32)inArgs[4];

    columnIndexLtt = minColumnLtt;
    rowIndexLtt    = minRowLtt;


    return wrCpssDxChIpLttReadEntry(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************/

static CMD_STATUS wrCpssDxChIpLttExtReadNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    if(columnIndexLtt>=maxColumnLtt)
    {
        columnIndexLtt=minColumnLtt;
        rowIndexLtt++;

    }
    else
        columnIndexLtt++;

    return wrCpssDxChIpLttReadEntry(inArgs,inFields,numFields,outArgs);
}


/* cpssDxChIpv4Prefix Table */
static GT_U32   TcamEntryIndexCnt;

/*******************************************************************************
* cpssDxChIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                         - the device number
*       routerTunnelTermTcamEntryIndex - the prefix index in the router Tcam
*                                       (0... numOfTcamColumn * numOfTcamRows -1)
*       ipAddr                         - The IP address of this prefix.
*       mask                           - The IP address mask of this prefix
*       mcGroupIndex                   - mc Group Index (G-Index).
*                                        if it is other then 0 it's requires
*                                        that the
*                                        routerTunnelTermTcamEntryIndex is in
*                                        the range (numOfTcamRows..numOfTcamRows*3 - 1)
*                                        meaning not in column 0 or 3
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active or
*                         mcGroupIndex and routerTunnelTermTcamEntryIndex
*                         are out of multicast entries range (see comments).
*       GT_OUT_OF_RANGE - routerTunnelTermTcamEntryIndex is bigger then (numOfTcamColumn * numOfTcamRows -1) .
*
* COMMENTS:
*       1. IP prefix entries reside in router TCAM. The router TCAM contains 1K
*           lines that are made of 5 column entries. Each line can hold:
*           - 1 tunnel termination entry
*           - 1 ipv6 addresses
*           - 5 ipv4 addresses (form which only 2 can be Ipv4 MC source address)
*           Indexes for ipv4 entries takes one column entry range (0.. numOfTcamColumn * numOfTcamRows -1);
*           But pay attention that Ipv6 address and tunnel termination entries
*           take a whole line, and the counting is COLUMN BY COLUMN meaning that
*           if an ipv6 prefix resides in index n (out 0..numOfTcamRows-1), it takes indexes
*           0+n,numOfTcamRows+n,numOfTcamRows*2+n,numOfTcamRows*3+n,numOfTcamRows*4+n !
*       2. The match for prefixes is done using a first match , thus
*           if two prefix can match a packet (they have the same prefix , but
*           diffrent prefix length) and we want LPM to work on them we have to
*           make sure to put the longest prefix before the short one.
*           All and all there must be the default prefix 0.0.0.0/mask 0 at the last
*           possible ipv4 prefix, and one before it the default MC prefix
*           224.0.0.0/ mask 240.0.0.0.
*       3. Ipv4 MC source prefixes can reside in indexs (numOfTcamRows.... numOfTcamRows*3 -1 ) only,
*           meaning columns 1 and 2 (out of 0,1,2,3,4). and only for them the MC
*           Group Index (G-index) is relevant.
*           the G-Index cannot be 0 for Ipv4 mc source prefixes!
*           for further details about the use of the G-index refer to the ipv4
*           MC section in the data sheet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixSet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       numOfTcamRows;
    GT_U32                       routerTunnelTermTcamEntryIndex;
    GT_IPADDR                    ipAddr;
    GT_IPADDR                    mask;
    GT_U32                       mcGroupIndex;
    CPSS_DXCH_IPV4_PREFIX_STC   ipv4Prefix;
    CPSS_DXCH_IPV4_PREFIX_STC   ipv4PrefixMask;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    routerTunnelTermTcamEntryIndex = (GT_U32)inFields[0];
    galtisIpAddr(&ipAddr, (GT_U8*)inFields[1]);
    galtisIpAddr(&mask, (GT_U8*)inFields[2]);
    mcGroupIndex = (GT_U32)inFields[3];

    /* call cpss api function */
    ipv4Prefix.vrId = 0;
    ipv4Prefix.ipAddr.u32Ip = ipAddr.u32Ip;
    ipv4Prefix.isMcSource = (mcGroupIndex == 0) ? GT_FALSE : GT_TRUE;

    ipv4PrefixMask.vrId = 0xffff;
    ipv4PrefixMask.ipAddr.u32Ip = mask.u32Ip;
    ipv4PrefixMask.isMcSource = (mcGroupIndex == 0) ? GT_FALSE : GT_TRUE;
    ipv4PrefixMask.mcGroupIndexRow = 0xfff;
    ipv4PrefixMask.mcGroupIndexColumn = 0x7;

    /* call cpss api function */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        numOfTcamRows = fineTuningPtr->tableSize.router;

        ipv4Prefix.mcGroupIndexRow = mcGroupIndex % numOfTcamRows;
        ipv4Prefix.mcGroupIndexColumn = mcGroupIndex / numOfTcamRows;

        result = cpssDxChIpv4PrefixSet(devNum,
                                    routerTunnelTermTcamEntryIndex % numOfTcamRows,
                                    routerTunnelTermTcamEntryIndex / numOfTcamRows,
                                    &ipv4Prefix,&ipv4PrefixMask);
    }
    else
    {
        ipv4Prefix.mcGroupIndexRow = mcGroupIndex / 4;
        ipv4Prefix.mcGroupIndexColumn = mcGroupIndex % 4;

        result = cpssDxChIpv4PrefixSet(devNum,
                                    routerTunnelTermTcamEntryIndex / 4,
                                    routerTunnelTermTcamEntryIndex % 4,
                                    &ipv4Prefix,&ipv4PrefixMask);
    }

  /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                         - the device number
*       routerTunnelTermTcamEntryIndex - the prefix index in the router Tcam
*                                        (0..numOfTcamRows*numOfColumn-1)
*
* OUTPUTS:
*       validPtr        - weather the entry is valid (if the entry isn't valid
*                         all the following fields well not get values)
*       ipAddrPtr       - The IP address of this prefix.
*       maskPtr         - The IP address mask of this prefix
*       mcGroupIndexPtr - mc Group Index (G-Index)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - routerTunnelTermTcamEntryIndex is bigger then 5119.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       see cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixGetFirst

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
    GT_IPADDR                       ipAddrPtr;
    GT_IPADDR                       maskPtr;
    GT_U32                          mcGroupIndexPtr;
    CPSS_DXCH_IPV4_PREFIX_STC       ipv4Prefix;
    CPSS_DXCH_IPV4_PREFIX_STC       ipv4PrefixMask;
    GT_U32                          numOfTcamRows;
    GT_U32                          numOfTcamColumn;
    GT_U32                          numOfRules;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* initialize ipv4Prefix and ipv4PrefixMask */
    cmdOsMemSet(&ipv4Prefix , 0, sizeof(ipv4Prefix));
    cmdOsMemSet(&ipv4PrefixMask , 0, sizeof(ipv4PrefixMask));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
    TcamEntryIndexCnt = 0;

    numOfTcamRows = fineTuningPtr->tableSize.router;

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        numOfTcamColumn = 5;
        break;
    default:
        numOfTcamColumn = 4;
        break;
    }

    numOfRules = numOfTcamColumn * numOfTcamRows;

    do{
        /* call cpss api function */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            result = cpssDxChIpv4PrefixGet(devNum,
                                       TcamEntryIndexCnt % numOfTcamRows,
                                       TcamEntryIndexCnt / numOfTcamRows,
                                       &validPtr, &ipv4Prefix, &ipv4PrefixMask);

            mcGroupIndexPtr = ipv4Prefix.mcGroupIndexColumn * numOfTcamRows + ipv4Prefix.mcGroupIndexRow;
        }
        else
        {
            result = cpssDxChIpv4PrefixGet(devNum,
                                       TcamEntryIndexCnt / 4,
                                       TcamEntryIndexCnt % 4,
                                       &validPtr, &ipv4Prefix, &ipv4PrefixMask);

            mcGroupIndexPtr = ipv4Prefix.mcGroupIndexColumn + (ipv4Prefix.mcGroupIndexRow * 4) ;
        }


        ipAddrPtr.u32Ip = ipv4Prefix.ipAddr.u32Ip;
        maskPtr.u32Ip = ipv4PrefixMask.ipAddr.u32Ip;

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        TcamEntryIndexCnt++;

    }while ((!validPtr) && (TcamEntryIndexCnt < numOfRules));

    TcamEntryIndexCnt--;

    if (!validPtr)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = TcamEntryIndexCnt;
    inFields[3] = mcGroupIndexPtr;

    /* pack and output table fields */
    fieldOutput("%d%4b%4b%d", inFields[0],
                ipAddrPtr.arIP , maskPtr.arIP, inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixGetNext
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
    GT_IPADDR                       ipAddrPtr;
    GT_IPADDR                       maskPtr;
    GT_U32                          mcGroupIndexPtr = 0;
    CPSS_DXCH_IPV4_PREFIX_STC       ipv4Prefix;
    CPSS_DXCH_IPV4_PREFIX_STC       ipv4PrefixMask;
    GT_U32                          numOfTcamRows;
    GT_U32                          numOfTcamColumn;
    GT_U32                          numOfRules;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    /* initialize ipv4Prefix and ipv4PrefixMask */
    cmdOsMemSet(&ipv4Prefix , 0, sizeof(ipv4Prefix));
    cmdOsMemSet(&ipv4PrefixMask , 0, sizeof(ipv4PrefixMask));

    numOfTcamRows = fineTuningPtr->tableSize.router;

    switch(PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    case CPSS_PP_FAMILY_CHEETAH2_E:
        numOfTcamColumn = 5;
        break;
    default:
        numOfTcamColumn = 4;
        break;
    }

    numOfRules = numOfTcamColumn * numOfTcamRows;

    do{

        TcamEntryIndexCnt++;

        if (TcamEntryIndexCnt == numOfRules)
        {
            validPtr = GT_FALSE;
            result = GT_OK;
            break;
        }

       /* call cpss api function */
        if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
        {
            result = cpssDxChIpv4PrefixGet(devNum,
                                       TcamEntryIndexCnt % numOfTcamRows,
                                       TcamEntryIndexCnt / numOfTcamRows,
                                       &validPtr, &ipv4Prefix, &ipv4PrefixMask);

            mcGroupIndexPtr = ipv4Prefix.mcGroupIndexColumn * numOfTcamRows + ipv4Prefix.mcGroupIndexRow;
        }
        else
        {
            result = cpssDxChIpv4PrefixGet(devNum,
                                       TcamEntryIndexCnt / 4,
                                       TcamEntryIndexCnt % 4,
                                       &validPtr, &ipv4Prefix, &ipv4PrefixMask);

            mcGroupIndexPtr = ipv4Prefix.mcGroupIndexColumn + (ipv4Prefix.mcGroupIndexRow * 4) ;
        }

        ipAddrPtr.u32Ip = ipv4Prefix.ipAddr.u32Ip;
        maskPtr.u32Ip = ipv4PrefixMask.ipAddr.u32Ip;

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        /*we are increasing TcamLineIndexCnt at the beginig of the loop so we must
          end in numOfTcamRows - 1*/
    }while ((!validPtr) && (TcamEntryIndexCnt < numOfRules - 1));

    if (!validPtr)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = TcamEntryIndexCnt;
    inFields[3] = mcGroupIndexPtr;

    /* pack and output table fields */
    fieldOutput("%d%4b%4b%d", inFields[0],
                ipAddrPtr.arIP , maskPtr.arIP, inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                         - the device number
*       routerTunnelTermTcamEntryIndex - the prefix index in the router Tcam
*                                        (0..numOfTcamRows*numOfColumn-1)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_OUT_OF_RANGE - routerTunnelTermTcamEntryIndex is bigger then numOfTcamRows*numOfColumn-1.
*       GT_BAD_PARAM    - on devNum not active.
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixInvalidate

(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       routerTunnelTermTcamEntryIndex;
    GT_U32                       numOfTcamRows;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;

    routerTunnelTermTcamEntryIndex = (GT_U32)inFields[0];

    /* call cpss api function */
    if(PRV_CPSS_PP_MAC(devNum)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        numOfTcamRows = fineTuningPtr->tableSize.router;

        /* call cpss api function */
        result = cpssDxChIpv4PrefixInvalidate(devNum,
                                              routerTunnelTermTcamEntryIndex % numOfTcamRows,
                                              routerTunnelTermTcamEntryIndex / numOfTcamRows);
    }
    else
    {
        /* call cpss api function */
        result = cpssDxChIpv4PrefixInvalidate(devNum,
                                              routerTunnelTermTcamEntryIndex / 4,
                                              routerTunnelTermTcamEntryIndex % 4);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/********************cpssDxChIpv4PrefixExt Table********************/


static GT_U32   minRow;
static GT_U32   maxRow;
static GT_U32   minColumn;
static GT_U32   maxColumn;

static GT_U32   columnIndex;
static GT_U32   rowIndex;

/*******************************************************************************
* cpssDxChIpv4PrefixSet
*
* DESCRIPTION:
*    Sets an ipv4 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       routerTtiTcamColumn       - the TCAM column to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on fail.
*       GT_BAD_PARAM    - on devNum not active
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   1.  IP prefix entries reside in router TCAM. The router TCAM is organized
*       in rows, where each row contains 4 column entries for Ch3 and 5 column
*       entries for Ch2. Each line can hold:
*       - 1 TTI (tunnel termination interface) entry
*       - 1 ipv6 address
*       - 4 (ch3)/ 5 (ch2) ipv4 addresses
*       Note that if a line is used for ipv6 address or TTI entry then
*       it can't be used for ipv4 addresses and if an entry in a line is used
*       for ipv4 address, then the other line entries must hold ipv4 addresses
*       as well.
*   2.  The match for prefixes is done using a first match , thus if two prefixes
*       can match a packet (they have the same prefix , but diffrent prefix
*       length) and we want LPM to work on them we have to make sure to put
*       the longest prefix before the short one.
*       Search order is by columns, first column, then second and so on.
*       All and all default UC prefix 0.0.0.0/mask 0 must reside at the last
*       possible ipv4 prefix, and default MC prefix 224.0.0.0/ mask 240.0.0.0
*       must reside at the index before it.
*   3.  In Ch2 MC source addresses can reside only on the second and third
*       columns (on other PP, there is no such restriction).
*   4.  In Ch2 there is no support for multiple virtual routers. Therefore in
*       Ch2 vrId is ignored.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixExtSet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                    result;

    IN  GT_U8       devNum;
    IN  GT_U32      routerTtiTcamRow;
    IN  GT_U32      routerTtiTcamColumn;
    IN  CPSS_DXCH_IPV4_PREFIX_STC   prefix;
    IN  CPSS_DXCH_IPV4_PREFIX_STC   mask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerTtiTcamRow= (GT_U32)inFields[0];
    routerTtiTcamColumn=(GT_U32)inFields[1];

    prefix.vrId=(GT_U32)inFields[2];
    galtisIpAddr(&(prefix.ipAddr), (GT_U8*)inFields[3]);
    prefix.isMcSource=(GT_BOOL)inFields[4];
    prefix.mcGroupIndexRow=(GT_U32)inFields[5];
    prefix.mcGroupIndexColumn=(GT_U32)inFields[6];


    mask.vrId=(GT_U32)inFields[7];
    galtisIpAddr(&(mask.ipAddr), (GT_U8*)inFields[8]);
    mask.isMcSource=(GT_BOOL)inFields[9];
    mask.mcGroupIndexRow=(GT_U32)inFields[10];
    mask.mcGroupIndexColumn=(GT_U32)inFields[11];

    /* call cpss api function */
    result = cpssDxChIpv4PrefixSet(devNum,
        routerTtiTcamRow,
        routerTtiTcamColumn,
        &prefix,&mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv4PrefixGet
*
* DESCRIPTION:
*    Gets an ipv4 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*       routerTtiTcamColumn       - the TCAM column to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       prefixPtr             - prefix to set
*       maskPtr               - mask of the prefix
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixExtGetEntry

(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
   GT_STATUS       result;

   IN  GT_U8       devNum;

   OUT GT_BOOL                     valid;
   OUT CPSS_DXCH_IPV4_PREFIX_STC   prefix;
   OUT CPSS_DXCH_IPV4_PREFIX_STC   mask;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* initialize prefix and mask */
    cmdOsMemSet(&prefix , 0, sizeof(prefix));
    cmdOsMemSet(&mask , 0, sizeof(mask));

    /* call cpss api function */
    do
    {
        result=cpssDxChIpv4PrefixGet(devNum,rowIndex,columnIndex,
                                     &valid,&prefix,&mask);
         if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        if(!valid)
         {
            if(columnIndex>=maxColumn)
            {
                columnIndex=minColumn;
                rowIndex++;
            }
            else
                columnIndex++;

            if(rowIndex>maxRow)
            {
                galtisOutput(outArgs, GT_OK, "%d", -1);
                return CMD_OK;
            }
        }
    } while(!valid);

    inFields[0]=rowIndex;
    inFields[1]=columnIndex;

    inFields[2]=prefix.vrId;
    inFields[4]=prefix.isMcSource;
    inFields[5]=prefix.mcGroupIndexRow;
    inFields[6]=prefix.mcGroupIndexColumn;


    inFields[7]=mask.vrId;
    inFields[9]=mask.isMcSource;
    inFields[10]=mask.mcGroupIndexRow;
    inFields[11]=mask.mcGroupIndexColumn;


    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%4b%d%d%d%d%4b%d%d%d", inFields[0],inFields[1],
                       inFields[2],prefix.ipAddr.arIP , inFields[4], inFields[5],
                inFields[6],inFields[7],mask.ipAddr.arIP,inFields[9],inFields[10],
                inFields[11]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/***********************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixExtGetFirst

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

    minRow   = (GT_U32)inArgs[1];
    maxRow   = (GT_U32)inArgs[2];
    minColumn= (GT_U32)inArgs[3];
    maxColumn= (GT_U32)inArgs[4];

    columnIndex = minColumn;
    rowIndex    = minRow;


    return wrCpssDxChIpv4PrefixExtGetEntry(inArgs,inFields,numFields,outArgs);

}
/************************************************************************************/
static CMD_STATUS wrCpssDxChIpv4PrefixExtGetNext

(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{


    if(columnIndex>=maxColumn)
    {
        columnIndex=minColumn;
        rowIndex++;
    }
    else
        columnIndex++;

    if(rowIndex>maxRow)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChIpv4PrefixExtGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssDxChIpv4PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv4 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to invalidate
*       routerTtiTcamColumn       - the TCAM column to invalidate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*
* COMMENTS:
*       none
*
*
*******************************************************************************/

static CMD_STATUS wrCpssDxChIpv4PrefixExtInvalidate

(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                    result;

    IN  GT_U8       devNum;
    IN  GT_U32      routerTtiTcamRow;
    IN  GT_U32      routerTtiTcamColumn;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerTtiTcamRow = (GT_U32)inFields[0];
    routerTtiTcamColumn = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssDxChIpv4PrefixInvalidate(devNum, routerTtiTcamRow,routerTtiTcamColumn);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/****************end of cpssDxChIpv4PrefixExt Table*****************/

/*************** cpssDxChIpv6Prefix Table *************************/
static GT_U32   TcamLineIndexCnt;

/*
* cpssDxChIpv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                        - the device number
*       routerTunnelTermTcamLineIndex - the prefix line index in the router Tcam
*                                       (0..1023)
*       ipAddr                        - The IP address of this prefix.
*       mask                          - The IP address mask of this prefix
*       mcGroupIndex                  - mc Group Index (G-Index)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - routerTunnelTermTcamLineIndex or mcGroupIndex
*                         are out of range (1023).
*
* COMMENTS:
*       1. IP prefix entries reside in router TCAM. The router TCAM contains 1K
*           lines that are made of 5 column entries. Each line can hold:
*           - 1 tunnel termination entry
*           - 1 ipv6 addresses
*           - 5 ipv4 addresses (form which only 2 can be Ipv4 MC source address)
*           Indexes for ipv6 entries takes one line (5 column entry) range
*           (0..1023), for futher considartion about the shared resource of the
*           router tcam see cpssDxChIpv4PrefixWrite.
*       2. The match for prefixes (as in ipv4) is done using a first match ,thus
*           if two prefix can match a packet (they have the same prefix , but
*           diffrent prefix length) and we want LPM to work on them we have to make
*           sure to put the longest prefix before the short one.
*           All and all there must be the default prefix 0::/mask 0 at the last
*           possible ipv6 prefix, and one before it the default MC prefix
*           FF::/mask FF::.
*       3. Ipv6 MC source prefixes can reside in ipv6 index, and only for them
*           the MC Group Index (G-index) is relevant.
*           the G-Index cannot be 0 for ipv6 mc source prefixes!
*           for further details about the use of the G-index refer to the ipv6
*           MC section in the data sheet.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;

    GT_U8                       devNum;
    GT_U32                      routerTunnelTermTcamLineIndex;
    GT_IPV6ADDR                 ipAddr;
    GT_IPV6ADDR                 mask;
    GT_U32                      mcGroupIndex;
    CPSS_DXCH_IPV6_PREFIX_STC   ipv6Prefix;
    CPSS_DXCH_IPV6_PREFIX_STC   ipv6PrefixMask;
    GT_U32                      j;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerTunnelTermTcamLineIndex = (GT_U32)inFields[0];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[1]);
    galtisIpv6Addr(&mask, (GT_U8*)inFields[2]);
    mcGroupIndex = (GT_U32)inFields[3];

    /* call cpss api function */
    ipv6Prefix.vrId = 0;
    for (j = 0 ; j < 4 ; j++)
        ipv6Prefix.ipAddr.u32Ip[j] = ipAddr.u32Ip[j];
    ipv6Prefix.isMcSource = (mcGroupIndex == 0) ? GT_FALSE : GT_TRUE;
    ipv6Prefix.mcGroupIndexRow = (mcGroupIndex);

    ipv6PrefixMask.vrId = 0xfff;
    for (j = 0 ; j < 4 ; j++)
        ipv6PrefixMask.ipAddr.u32Ip[j] = mask.u32Ip[j];
    ipv6PrefixMask.isMcSource = (mcGroupIndex == 0) ? GT_FALSE : GT_TRUE;
    ipv6PrefixMask.mcGroupIndexRow = 0xfff;

    result = cpssDxChIpv6PrefixSet(devNum,routerTunnelTermTcamLineIndex,
                                   &ipv6Prefix,&ipv6PrefixMask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                        - the device number
*       routerTunnelTermTcamLineIndex - the prefix index in the router Tcam
*                                       (0..numOfTcamRows-1)
*
* OUTPUTS:
*       validPtr        - weather the entry is valid (if the entry isn't valid
*                         all the following fields well not get values)
*       ipAddrPtr       - The IP address of this prefix.
*       maskPtr         - The IP address mask of this prefix
*       mcGroupIndexPtr - mc Group Index (G-Index)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - routerTunnelTermTcamLineIndex is out of range (1023).
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       see cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixGetFirst

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
    GT_IPV6ADDR                     ipAddrPtr;
    GT_IPV6ADDR                     maskPtr;
    GT_U32                          mcGroupIndexPtr;
    CPSS_DXCH_IPV6_PREFIX_STC       ipv6Prefix;
    CPSS_DXCH_IPV6_PREFIX_STC       ipv6PrefixMask;
    GT_U32                          j;
    GT_U32                          numOfTcamRows;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
    numOfTcamRows = fineTuningPtr->tableSize.router;

    TcamLineIndexCnt = 0;

   /* initialize ipv6Prefix and ipv6PrefixMask */
    cmdOsMemSet(&ipv6Prefix , 0, sizeof(ipv6Prefix));
    cmdOsMemSet(&ipv6PrefixMask , 0, sizeof(ipv6PrefixMask));


    do{
        /* call cpss api function */
        result = cpssDxChIpv6PrefixGet(devNum, TcamLineIndexCnt, &validPtr,
                                       &ipv6Prefix,&ipv6PrefixMask);
        for (j = 0 ; j < 4 ; j++)
        {
            ipAddrPtr.u32Ip[j] = ipv6Prefix.ipAddr.u32Ip[j];
            maskPtr.u32Ip[j] = ipv6PrefixMask.ipAddr.u32Ip[j];
        }
        mcGroupIndexPtr = ipv6Prefix.mcGroupIndexRow;

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        TcamLineIndexCnt++;

    }while ((!validPtr) && (TcamLineIndexCnt < numOfTcamRows));

    TcamLineIndexCnt--;

    if (!validPtr)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = TcamLineIndexCnt;
    inFields[3] = mcGroupIndexPtr;

    /* pack and output table fields */
    fieldOutput("%d%16B%16B%d", inFields[0],
                ipAddrPtr.arIP , maskPtr.arIP, inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixGetNext
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
    GT_IPV6ADDR                     ipAddrPtr;
    GT_IPV6ADDR                     maskPtr;
    GT_U32                          mcGroupIndexPtr = 0;
    CPSS_DXCH_IPV6_PREFIX_STC       ipv6Prefix;
    CPSS_DXCH_IPV6_PREFIX_STC       ipv6PrefixMask;
    GT_U32                          j;
    GT_U32                          numOfTcamRows;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
    numOfTcamRows = fineTuningPtr->tableSize.router;


    /* initialize ipv6Prefix and ipv6PrefixMask */
    cmdOsMemSet(&ipv6Prefix , 0, sizeof(ipv6Prefix));
    cmdOsMemSet(&ipv6PrefixMask , 0, sizeof(ipv6PrefixMask));

    do{

        TcamLineIndexCnt++;

        if (TcamLineIndexCnt == numOfTcamRows)
        {
            validPtr = GT_FALSE;
            result = GT_OK;
            break;
        }

        /* call cpss api function */
        result = cpssDxChIpv6PrefixGet(devNum, TcamLineIndexCnt, &validPtr,
                                       &ipv6Prefix,&ipv6PrefixMask);
        for (j = 0 ; j < 4 ; j++)
        {
            ipAddrPtr.u32Ip[j] = ipv6Prefix.ipAddr.u32Ip[j];
            maskPtr.u32Ip[j] = ipv6PrefixMask.ipAddr.u32Ip[j];
        }
        mcGroupIndexPtr = ipv6Prefix.mcGroupIndexRow;

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
        /*we are increasing TcamLineIndexCnt at the beginig of the loop so we must
          end in numOfTcamRows - 1*/
    }while ((!validPtr) && (TcamLineIndexCnt < numOfTcamRows - 1 ));

    if (!validPtr)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = TcamLineIndexCnt;
    inFields[3] = mcGroupIndexPtr;

    /* pack and output table fields */
    fieldOutput("%d%16B%16B%d", inFields[0],
                ipAddrPtr.arIP , maskPtr.arIP, inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6PrefixInvalidate
*
* DESCRIPTION:
*    Invalidates an ipv6 UC or MC prefix in the Router Tcam.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                        - the device number
*       routerTunnelTermTcamLineIndex - the prefix index in the router Tcam
*                                       (0..1023)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_OUT_OF_RANGE - routerTunnelTermTcamLineIndex is out of range (1023).
*
* COMMENTS:
*       pay attention that since this is an ipv6 prefix , it will invalidate
*       the whole line (all 5 columns)
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixInvalidate

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                        devNum;
    GT_U32                       routerTunnelTermTcamLineIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerTunnelTermTcamLineIndex = (GT_U32)inFields[0];

    /* call cpss api function */
    result = cpssDxChIpv6PrefixInvalidate(devNum, routerTunnelTermTcamLineIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/********************cpssDxChIpv6PrefixExt Table******************************/
/****/
static GT_U32   rowIndexIpv6;
static GT_U32   maxRowIndexIpv6;
/*******************************************************************************
* cpssDxChIpv6PrefixSet
*
* DESCRIPTION:
*    Sets an ipv6 UC or MC prefix to the Router Tcam.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to set
*       prefixPtr                 - prefix to set
*       maskPtr                   - mask of the prefix
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*
* COMMENTS:
*       See cpssDxChIpv4PrefixSet
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixExtSet

(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                    result;

    IN  GT_U8           devNum;
    IN  GT_U32          routerTtiTcamRow;
    IN  CPSS_DXCH_IPV6_PREFIX_STC   prefix;
    IN  CPSS_DXCH_IPV6_PREFIX_STC   mask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    routerTtiTcamRow = (GT_U32)inFields[0];
    prefix.vrId=(GT_U32)inFields[1];
    galtisIpv6Addr(&(prefix.ipAddr), (GT_U8*)inFields[2]);
    prefix.isMcSource=(GT_BOOL)inFields[3];
    prefix.mcGroupIndexRow=(GT_U32)inFields[4];



    mask.vrId=(GT_U32)inFields[5];
    galtisIpv6Addr(&(mask.ipAddr), (GT_U8*)inFields[6]);
    mask.isMcSource=(GT_BOOL)inFields[7];
    mask.mcGroupIndexRow=(GT_U32)inFields[8];


    /* call cpss api function */
    result = cpssDxChIpv6PrefixSet(devNum, routerTtiTcamRow,&prefix,&mask);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpv6PrefixGet
*
* DESCRIPTION:
*    Gets an ipv6 UC or MC prefix from the Router Tcam.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum                    - the device number
*       routerTtiTcamRow          - the TCAM row to get from
*
* OUTPUTS:
*       validPtr              - whether the entry is valid (if the entry isn't valid
*                               all the following fields will not get values)
*       vrIdPtr               - The virtual router id, relevant only if isMcSource
*                               is GT_FALSE
*       ipAddrPtr             - The IP address of this prefix.
*       maskPtr               - The IP address mask of this prefix
*       isMcSourcePtr         - GT_TRUE: MC source address
*                               GT_FALSE: MC group address or UC address
*       mcGroupIndexRowPtr    - mc Group index row, relevant only if
*                               isMcSource is GT_TRUE
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       See cpssDxChIpv6PrefixSet
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixExtGetEntry

(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS                       result;

    IN  GT_U8           devNum;

    OUT GT_BOOL         valid;
    OUT CPSS_DXCH_IPV6_PREFIX_STC   prefix;
    OUT CPSS_DXCH_IPV6_PREFIX_STC   mask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* initialize prefix and mask */
    cmdOsMemSet(&prefix , 0, sizeof(prefix));
    cmdOsMemSet(&mask , 0, sizeof(mask));

    do{
        /* call cpss api function */
        result = cpssDxChIpv6PrefixGet(devNum, rowIndexIpv6,&valid,&prefix,&mask);

        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }

        if(!valid)rowIndexIpv6++;

    }while ((!valid) && (rowIndexIpv6 <= maxRowIndexIpv6));

    if(rowIndexIpv6>maxRowIndexIpv6)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = rowIndexIpv6;
    inFields[1] =prefix.vrId;
    /*galtisIpv6Addr(&(prefix.ipAddr), (GT_U8*)inFields[2]);*/
    inFields[3]=prefix.isMcSource;
    inFields[4]=prefix.mcGroupIndexRow;



    inFields[5]=mask.vrId;
    /*galtisIpv6Addr(&(mask.ipAddr), (GT_U8*)inFields[6]);*/
    inFields[7]=mask.isMcSource;
    inFields[8]=mask.mcGroupIndexRow;

    /* pack and output table fields */
    fieldOutput("%d%d%16B%d%d%d%16B%d%d", inFields[0],
        inFields[1],prefix.ipAddr.arIP ,inFields[3],inFields[4],inFields[5],
                 mask.ipAddr.arIP, inFields[7],inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}
/*****************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixExtGetFirst

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

    rowIndexIpv6=0;
    maxRowIndexIpv6=(GT_U32)inArgs[1];
    return wrCpssDxChIpv6PrefixExtGetEntry(inArgs,inFields,numFields,outArgs);

}
/************************************************************************************/
static CMD_STATUS wrCpssDxChIpv6PrefixExtGetNext

(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{


    rowIndexIpv6++;
    if (rowIndexIpv6 > maxRowIndexIpv6)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChIpv6PrefixExtGetEntry(inArgs,inFields,numFields,outArgs);
}

/*************end of cpssDxChIpv6PrefixExt Table*****************************/


/****************cpssDxChIpUcRoute Table************/
/*set*/
static CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC*   gIpUcRouteEntryArrayPtr = NULL;
static GT_U32                             gIpUcNumOfEntries = 0;
static GT_U32                             gIpUcRouteEntryCurrIndx = 0;

/*get*/
#define IPUC_ROUTE_ARR_MAX_SIZE 1024

static GT_U32          gIpUcRouteEntryLastIndx;
static GT_U8           numOfNewEntriesFactor = 10;
/*******************************************************************************
* wrCpssDxChIpUcRouteEntriesWriteSetFirst
*
* DESCRIPTION:
*    Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum               - the device number
*       baseRouteEntryIndex  - the base Index in the Route entries table.
*                              The entries will be written from this
*                              base on.
*       routeEntriesArray    - the uc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed or
*                         on invalid routeEntriesArray parameter.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesWriteSetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC     *regularEntry = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    if(gIpUcRouteEntryArrayPtr == NULL)
    {
        gIpUcRouteEntryArrayPtr = (CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC*)
            cmdOsMalloc(numOfNewEntriesFactor*sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));
        gIpUcNumOfEntries = numOfNewEntriesFactor;

        if(gIpUcRouteEntryArrayPtr == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }
    gIpUcRouteEntryCurrIndx = 0;

    gIpUcRouteEntryArrayPtr[gIpUcRouteEntryCurrIndx].type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;

    regularEntry = &gIpUcRouteEntryArrayPtr[gIpUcRouteEntryCurrIndx].entry.regularEntry;

    regularEntry->cmd = (CPSS_PACKET_CMD_ENT)inFields[0];
    regularEntry->cpuCodeIdx = (CPSS_DXCH_IP_CPU_CODE_INDEX_ENT)inFields[1];
    regularEntry->appSpecificCpuCodeEnable = (GT_BOOL)inFields[2];
    regularEntry->unicastPacketSipFilterEnable = (GT_BOOL)inFields[3];
    regularEntry->ttlHopLimitDecEnable = (GT_BOOL)inFields[4];
    regularEntry->ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[5];
    regularEntry->ingressMirror = (GT_BOOL)inFields[6];
    regularEntry->qosProfileMarkingEnable = (GT_BOOL)inFields[7];
    regularEntry->qosProfileIndex = (GT_U32)inFields[8];
    regularEntry->qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[9];
    regularEntry->modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
    regularEntry->modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
    regularEntry->countSet = (CPSS_IP_CNT_SET_ENT)inFields[12];
    regularEntry->trapMirrorArpBcEnable = (GT_BOOL)inFields[13];
    regularEntry->sipAccessLevel = (GT_U32)inFields[14];
    regularEntry->dipAccessLevel = (GT_U32)inFields[15];
    regularEntry->ICMPRedirectEnable = (GT_BOOL)inFields[16];
    regularEntry->scopeCheckingEnable = (GT_BOOL)inFields[17];
    regularEntry->siteId = (CPSS_IP_SITE_ID_ENT)inFields[18];
    regularEntry->mtuProfileIndex = (GT_U32)inFields[19];
    regularEntry->isTunnelStart = (GT_BOOL)inFields[20];
    regularEntry->nextHopVlanId = (GT_U16)inFields[21];
    regularEntry->nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[22];
    regularEntry->nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[23];
    regularEntry->nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[24];
    CONVERT_DEV_PORT_DATA_MAC(regularEntry->nextHopInterface.devPort.hwDevNum,
        regularEntry->nextHopInterface.devPort.portNum);
    regularEntry->nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[25];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(regularEntry->nextHopInterface.trunkId);
    regularEntry->nextHopInterface.vidx = (GT_U16)inFields[26];
    regularEntry->nextHopInterface.vlanId = (GT_U16)inFields[27];
    regularEntry->nextHopARPPointer = (GT_U32)inFields[28];
    regularEntry->nextHopTunnelPointer = (GT_U32)inFields[29];
    regularEntry->nextHopVlanId1 = 0;
    regularEntry->ingressMirrorToAnalyzerIndex = 0;

    gIpUcRouteEntryCurrIndx++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpUcRouteEntriesWriteSetNext
*
* DESCRIPTION:
*    Write an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum               - the device number
*       baseRouteEntryIndex  - the base Index in the Route entries table.
*                              The entries will be written from this
*                              base on.
*       routeEntriesArray    - the uc route entries array
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed or
*                         on invalid routeEntriesArray parameter.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*
* COMMENTS:
*
*
 *******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesWriteSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC     *regularEntry = NULL;

     /* check for valid arguments */
     if(!inArgs || !outArgs)
         return CMD_AGENT_ERROR;

     if(gIpUcNumOfEntries == IPUC_ROUTE_ARR_MAX_SIZE)
     {
         galtisOutput(outArgs, GT_NO_RESOURCE, "\nUp to 0x%X entries in one command\n",IPUC_ROUTE_ARR_MAX_SIZE);
         return CMD_OK;
     }

     if(gIpUcRouteEntryCurrIndx == gIpUcNumOfEntries)
     {
         GT_U32 addSize;

         if((gIpUcNumOfEntries + numOfNewEntriesFactor) > IPUC_ROUTE_ARR_MAX_SIZE )
             addSize = IPUC_ROUTE_ARR_MAX_SIZE - gIpUcNumOfEntries;
         else
             addSize = numOfNewEntriesFactor;

         gIpUcRouteEntryArrayPtr = (CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC*)
             cmdOsRealloc(gIpUcRouteEntryArrayPtr,
             sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC) * (gIpUcNumOfEntries + addSize));
         gIpUcNumOfEntries += addSize;

         if(gIpUcRouteEntryArrayPtr == NULL)
         {
             galtisOutput(outArgs, GT_NO_RESOURCE, "");
             return CMD_OK;
         }
     }

     gIpUcRouteEntryArrayPtr[gIpUcRouteEntryCurrIndx].type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;

     regularEntry = &gIpUcRouteEntryArrayPtr[gIpUcRouteEntryCurrIndx].entry.regularEntry;

     regularEntry->cmd = (CPSS_PACKET_CMD_ENT)inFields[0];
     regularEntry->cpuCodeIdx = (CPSS_DXCH_IP_CPU_CODE_INDEX_ENT)inFields[1];
     regularEntry->appSpecificCpuCodeEnable = (GT_BOOL)inFields[2];
     regularEntry->unicastPacketSipFilterEnable = (GT_BOOL)inFields[3];
     regularEntry->ttlHopLimitDecEnable = (GT_BOOL)inFields[4];
     regularEntry->ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[5];
     regularEntry->ingressMirror = (GT_BOOL)inFields[6];
     regularEntry->qosProfileMarkingEnable = (GT_BOOL)inFields[7];
     regularEntry->qosProfileIndex = (GT_U32)inFields[8];
     regularEntry->qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[9];
     regularEntry->modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
     regularEntry->modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
     regularEntry->countSet = (CPSS_IP_CNT_SET_ENT)inFields[12];
     regularEntry->trapMirrorArpBcEnable = (GT_BOOL)inFields[13];
     regularEntry->sipAccessLevel = (GT_U32)inFields[14];
     regularEntry->dipAccessLevel = (GT_U32)inFields[15];
     regularEntry->ICMPRedirectEnable = (GT_BOOL)inFields[16];
     regularEntry->scopeCheckingEnable = (GT_BOOL)inFields[17];
     regularEntry->siteId = (CPSS_IP_SITE_ID_ENT)inFields[18];
     regularEntry->mtuProfileIndex = (GT_U32)inFields[19];
     regularEntry->isTunnelStart = (GT_BOOL)inFields[20];
     regularEntry->nextHopVlanId = (GT_U16)inFields[21];
     regularEntry->nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[22];
     regularEntry->nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[23];
     regularEntry->nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[24];
     CONVERT_DEV_PORT_DATA_MAC(regularEntry->nextHopInterface.devPort.hwDevNum,
         regularEntry->nextHopInterface.devPort.portNum);
     regularEntry->nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[25];
     CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(regularEntry->nextHopInterface.trunkId);
     regularEntry->nextHopInterface.vidx = (GT_U16)inFields[26];
     regularEntry->nextHopInterface.vlanId = (GT_U16)inFields[27];
     regularEntry->nextHopARPPointer = (GT_U32)inFields[28];
     regularEntry->nextHopTunnelPointer = (GT_U32)inFields[29];
     regularEntry->nextHopVlanId1 = 0;
     regularEntry->ingressMirrorToAnalyzerIndex = 0;

     gIpUcRouteEntryCurrIndx++;

     /* pack output arguments to galtis string */
     galtisOutput(outArgs, GT_OK, "");

     return CMD_OK;
}

 /*******************************************************************************
 * wrCpssDxChIpUcRouteEntriesWriteEndSet
 *
 * DESCRIPTION:
 *    Write an array of uc route entries to hw.
 *
 * APPLICABLE DEVICES:
 *       All DxCh2 devices.
 *
 * INPUTS:
 *       devNum               - the device number
 *       baseRouteEntryIndex  - the base Index in the Route entries table.
 *                              The entries will be written from this
 *                              base on.
 *       routeEntriesArray    - the uc route entries array
 *       numOfRouteEntries    - the number route entries in the array.
 *
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK           - on success.
 *       GT_BAD_PARAM    - on devNum not active.
 *       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed or
 *                         on invalid routeEntriesArray parameter.
 *       GT_BAD_PTR      - one of the parameters is NULL pointer.
 *
 * COMMENTS:
 *
 *
 *******************************************************************************/
 static CMD_STATUS wrCpssDxChIpUcRouteEntriesWriteEndSet
     (
     IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
     IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
     IN  GT_32 numFields,
     OUT GT_8  outArgs[CMD_MAX_BUFFER]
     )
 {

     GT_STATUS   status;

     GT_U8    dev;
     GT_U32   baseRouteEntryIndex;

     /* check for valid arguments */
     if(!inArgs || !outArgs)
         return CMD_AGENT_ERROR;

     dev = (GT_U8)inArgs[0];
     baseRouteEntryIndex = (GT_U32)inArgs[1];

     /* call cpss api function */
     status =  cpssDxChIpUcRouteEntriesWrite(dev,baseRouteEntryIndex,
         gIpUcRouteEntryArrayPtr,
         gIpUcRouteEntryCurrIndx);

     /* pack output arguments to galtis string */
     galtisOutput(outArgs, status, "");

     cmdOsFree(gIpUcRouteEntryArrayPtr);
     gIpUcRouteEntryArrayPtr = NULL;

     return CMD_OK;
 }

static GT_U32 currEntryInd;

/*******************************************************************************
* wrCpssDxChIpUcRouteEntriesReadGetFirst
*
* DESCRIPTION:
*    read an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum               - the device number
*       baseRouteEntryIndex  - the base index. The entries will be read from
*                              this base on.
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       routeEntriesArray    - the uc route entries array read
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed.
*       GT_BAD_PTR     - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesReadGetFirst
(
     IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
     IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
     IN  GT_32 numFields,
     OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     GT_STATUS result;

     GT_U8   dev;
     GT_U32  baseRouteEntryIndex;
     GT_U32  numOfEntries;
     CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC     *regularEntry = NULL;
     GT_U32  i;

     /* check for valid arguments */
     if(!inArgs || !outArgs)
         return CMD_AGENT_ERROR;

     numOfEntries = (GT_U32)inArgs[2];
     if(numOfEntries > 1024)
         return CMD_AGENT_ERROR;

     gIpUcRouteEntryLastIndx = numOfEntries;
     gIpUcRouteEntryArrayPtr = (CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC*)cmdOsMalloc(numOfEntries * sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));

     if (gIpUcRouteEntryArrayPtr == NULL)
     {
         galtisOutput(outArgs, GT_NO_RESOURCE, "");
         return CMD_OK;
     }

     currEntryInd = 0;

     /* map input arguments to locals */
     dev = (GT_U8)inArgs[0];
     baseRouteEntryIndex = (GT_U32)inArgs[1];

     /* call cpss api function */
     cmdOsMemSet(gIpUcRouteEntryArrayPtr, 0,
         gIpUcRouteEntryLastIndx * sizeof(CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC));

     for (i=0; i < gIpUcRouteEntryLastIndx; i++)
     {
         gIpUcRouteEntryArrayPtr[i].type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;
     }

     result = cpssDxChIpUcRouteEntriesRead(dev,baseRouteEntryIndex,
         gIpUcRouteEntryArrayPtr,
         gIpUcRouteEntryLastIndx);

     if (result != GT_OK)
     {
         galtisOutput(outArgs, result, "");
         cmdOsFree(gIpUcRouteEntryArrayPtr);
         gIpUcRouteEntryArrayPtr = NULL;
         return CMD_OK;
     }

     regularEntry = &gIpUcRouteEntryArrayPtr[currEntryInd].entry.regularEntry;

     inFields[0] = regularEntry->cmd;
     inFields[1] = regularEntry->cpuCodeIdx;
     inFields[2] = regularEntry->appSpecificCpuCodeEnable;
     inFields[3] = regularEntry->unicastPacketSipFilterEnable;
     inFields[4] = regularEntry->ttlHopLimitDecEnable;
     inFields[5] = regularEntry->ttlHopLimDecOptionsExtChkByPass;
     inFields[6] = regularEntry->ingressMirror;
     inFields[7] = regularEntry->qosProfileMarkingEnable;
     inFields[8] = regularEntry->qosProfileIndex;
     inFields[9] = regularEntry->qosPrecedence;
     inFields[10] = regularEntry->modifyUp;
     inFields[11] = regularEntry->modifyDscp;
     inFields[12] = regularEntry->countSet;
     inFields[13] = regularEntry->trapMirrorArpBcEnable;
     inFields[14] = regularEntry->sipAccessLevel;
     inFields[15] = regularEntry->dipAccessLevel;
     inFields[16] = regularEntry->ICMPRedirectEnable;
     inFields[17] = regularEntry->scopeCheckingEnable;
     inFields[18] = regularEntry->siteId;
     inFields[19] = regularEntry->mtuProfileIndex;
     inFields[20] = regularEntry->isTunnelStart;
     inFields[21] = regularEntry->nextHopVlanId;
     inFields[22] = regularEntry->nextHopInterface.type;

     CONVERT_BACK_DEV_PORT_DATA_MAC(regularEntry->nextHopInterface.devPort.hwDevNum,
         regularEntry->nextHopInterface.devPort.portNum);
     inFields[23] = regularEntry->nextHopInterface.devPort.hwDevNum;
     inFields[24] = regularEntry->nextHopInterface.devPort.portNum;

     CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(regularEntry->nextHopInterface.trunkId);
     inFields[25] = regularEntry->nextHopInterface.trunkId;
     inFields[26] = regularEntry->nextHopInterface.vidx;
     inFields[27] = regularEntry->nextHopInterface.vlanId;
     inFields[28] = regularEntry->nextHopARPPointer;
     inFields[29] = regularEntry->nextHopTunnelPointer;

     /* pack and output table fields */
     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
         ,inFields[0], inFields[1], inFields[2], inFields[3],
         inFields[4], inFields[5], inFields[6], inFields[7],
         inFields[8], inFields[9], inFields[10], inFields[11],
         inFields[12], inFields[13], inFields[14], inFields[15],
         inFields[16], inFields[17], inFields[18], inFields[19],
         inFields[20], inFields[21], inFields[22], inFields[23],
         inFields[24], inFields[25], inFields[26], inFields[27],
         inFields[28], inFields[29]);

     galtisOutput(outArgs, result, "%f");

     currEntryInd++;

     return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpUcRouteEntriesReadGetNext
*
* DESCRIPTION:
*    read an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum               - the device number
*       baseRouteEntryIndex  - the base index. The entries will be read from
*                              this base on.
*       numOfRouteEntries    - the number route entries in the array.
*
*
* OUTPUTS:
*       routeEntriesArray    - the uc route entries array read
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed.
*       GT_BAD_PTR     - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesReadGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC     *regularEntry = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(currEntryInd >= gIpUcRouteEntryLastIndx)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        cmdOsFree(gIpUcRouteEntryArrayPtr);
        gIpUcRouteEntryArrayPtr = NULL;
        return CMD_OK;
    }

    regularEntry = &gIpUcRouteEntryArrayPtr[currEntryInd].entry.regularEntry;

    inFields[0] = regularEntry->cmd;
    inFields[1] = regularEntry->cpuCodeIdx;
    inFields[2] = regularEntry->appSpecificCpuCodeEnable;
    inFields[3] = regularEntry->unicastPacketSipFilterEnable;
    inFields[4] = regularEntry->ttlHopLimitDecEnable;
    inFields[5] = regularEntry->ttlHopLimDecOptionsExtChkByPass;
    inFields[6] = regularEntry->ingressMirror;
    inFields[7] = regularEntry->qosProfileMarkingEnable;
    inFields[8] = regularEntry->qosProfileIndex;
    inFields[9] = regularEntry->qosPrecedence;
    inFields[10] = regularEntry->modifyUp;
    inFields[11] = regularEntry->modifyDscp;
    inFields[12] = regularEntry->countSet;
    inFields[13] = regularEntry->trapMirrorArpBcEnable;
    inFields[14] = regularEntry->sipAccessLevel;
    inFields[15] = regularEntry->dipAccessLevel;
    inFields[16] = regularEntry->ICMPRedirectEnable;
    inFields[17] = regularEntry->scopeCheckingEnable;
    inFields[18] = regularEntry->siteId;
    inFields[19] = regularEntry->mtuProfileIndex;
    inFields[20] = regularEntry->isTunnelStart;
    inFields[21] = regularEntry->nextHopVlanId;
    inFields[22] = regularEntry->nextHopInterface.type;
    CONVERT_BACK_DEV_PORT_DATA_MAC(regularEntry->nextHopInterface.devPort.hwDevNum,
         regularEntry->nextHopInterface.devPort.portNum);
    inFields[23] = regularEntry->nextHopInterface.devPort.hwDevNum;
    inFields[24] = regularEntry->nextHopInterface.devPort.portNum;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(regularEntry->nextHopInterface.trunkId);
    inFields[25] = regularEntry->nextHopInterface.trunkId;
    inFields[26] = regularEntry->nextHopInterface.vidx;
    inFields[27] = regularEntry->nextHopInterface.vlanId;
    inFields[28] = regularEntry->nextHopARPPointer;
    inFields[29] = regularEntry->nextHopTunnelPointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28], inFields[29]);

    galtisOutput(outArgs, GT_OK, "%f");

    currEntryInd++;

    return CMD_OK;
}

/****************cpssDxChIpUcRouteEntries Table************/

#define ROUTE_ARRAY_SIZE 1024
#define DEFAULT_NUM_OF_ENTRIES 10
CPSS_DXCH_IP_UC_ROUTE_ENTRY_STC  routeEntriesArray[ROUTE_ARRAY_SIZE];
GT_U32                           cIndexSet;
GT_U32                           cIndexGet;
GT_U32                           numOfEntries;
/*******************************************************************************
* cpssDxChIpUcRouteEntriesWrite
*
* DESCRIPTION:
*    Writes an array of uc route entries to hw.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to write the array
*       routeEntriesArray     - the uc route entries array
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to write)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*
* COMMENTS:
*       In case uRPF ECMP/QOS is globally enabled, then for ECMP/QOS block
*       with x route entries, additional route entry should be included after
*       the block with the uRPF route entry information.
*
*
*******************************************************************************/

/****set for type CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC***********/

static CMD_STATUS wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 IN  GT_BOOL isBobcat2,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    routeEntriesArray[cIndexSet].type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;

    routeEntriesArray[cIndexSet].entry.regularEntry.cmd = (CPSS_PACKET_CMD_ENT)inFields[0];
    routeEntriesArray[cIndexSet].entry.regularEntry.cpuCodeIdx = (CPSS_DXCH_IP_CPU_CODE_INDEX_ENT)inFields[1];
    routeEntriesArray[cIndexSet].entry.regularEntry.appSpecificCpuCodeEnable = (GT_BOOL)inFields[2];
    routeEntriesArray[cIndexSet].entry.regularEntry.unicastPacketSipFilterEnable = (GT_BOOL)inFields[3];
    routeEntriesArray[cIndexSet].entry.regularEntry.ttlHopLimitDecEnable = (GT_BOOL)inFields[4];
    routeEntriesArray[cIndexSet].entry.regularEntry.ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[5];
    routeEntriesArray[cIndexSet].entry.regularEntry.ingressMirror = (GT_BOOL)inFields[6];
    routeEntriesArray[cIndexSet].entry.regularEntry.qosProfileMarkingEnable = (GT_BOOL)inFields[7];
    routeEntriesArray[cIndexSet].entry.regularEntry.qosProfileIndex = (GT_U32)inFields[8];
    routeEntriesArray[cIndexSet].entry.regularEntry.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[9];
    routeEntriesArray[cIndexSet].entry.regularEntry.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
    routeEntriesArray[cIndexSet].entry.regularEntry.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
    routeEntriesArray[cIndexSet].entry.regularEntry.countSet = (CPSS_IP_CNT_SET_ENT)inFields[12];
    routeEntriesArray[cIndexSet].entry.regularEntry.trapMirrorArpBcEnable = (GT_BOOL)inFields[13];
    routeEntriesArray[cIndexSet].entry.regularEntry.sipAccessLevel = (GT_U32)inFields[14];
    routeEntriesArray[cIndexSet].entry.regularEntry.dipAccessLevel = (GT_U32)inFields[15];
    routeEntriesArray[cIndexSet].entry.regularEntry.ICMPRedirectEnable = (GT_BOOL)inFields[16];
    routeEntriesArray[cIndexSet].entry.regularEntry.scopeCheckingEnable = (GT_BOOL)inFields[17];
    routeEntriesArray[cIndexSet].entry.regularEntry.siteId = (CPSS_IP_SITE_ID_ENT)inFields[18];
    routeEntriesArray[cIndexSet].entry.regularEntry.mtuProfileIndex = (GT_U32)inFields[19];
    routeEntriesArray[cIndexSet].entry.regularEntry.isTunnelStart = (GT_BOOL)inFields[20];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopVlanId = (GT_U16)inFields[21];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[22];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[23];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[24];
    CONVERT_DEV_PORT_DATA_MAC(routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.hwDevNum ,
        routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.portNum);
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[25];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.trunkId);
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.vidx = (GT_U16)inFields[26];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.vlanId = (GT_U16)inFields[27];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopARPPointer = (GT_U32)inFields[28];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopTunnelPointer = (GT_U32)inFields[29];

    if (isBobcat2 == GT_TRUE)
    {
        routeEntriesArray[cIndexSet].entry.regularEntry.nextHopVlanId1 = (GT_U16)inFields[30];
        routeEntriesArray[cIndexSet].entry.regularEntry.ingressMirrorToAnalyzerIndex = (GT_U32)inFields[31];
    }
    else
    {
        routeEntriesArray[cIndexSet].entry.regularEntry.nextHopVlanId1 = 0;
        routeEntriesArray[cIndexSet].entry.regularEntry.ingressMirrorToAnalyzerIndex = 0;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/* 2 new fields from wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE:
   isNat nextHopNatPointer */
static CMD_STATUS wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE_2
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

    routeEntriesArray[cIndexSet].type = CPSS_DXCH_IP_UC_ROUTE_ENTRY_E;

    routeEntriesArray[cIndexSet].entry.regularEntry.cmd = (CPSS_PACKET_CMD_ENT)inFields[0];
    routeEntriesArray[cIndexSet].entry.regularEntry.cpuCodeIdx = (CPSS_DXCH_IP_CPU_CODE_INDEX_ENT)inFields[1];
    routeEntriesArray[cIndexSet].entry.regularEntry.appSpecificCpuCodeEnable = (GT_BOOL)inFields[2];
    routeEntriesArray[cIndexSet].entry.regularEntry.unicastPacketSipFilterEnable = (GT_BOOL)inFields[3];
    routeEntriesArray[cIndexSet].entry.regularEntry.ttlHopLimitDecEnable = (GT_BOOL)inFields[4];
    routeEntriesArray[cIndexSet].entry.regularEntry.ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[5];
    routeEntriesArray[cIndexSet].entry.regularEntry.ingressMirror = (GT_BOOL)inFields[6];
    routeEntriesArray[cIndexSet].entry.regularEntry.qosProfileMarkingEnable = (GT_BOOL)inFields[7];
    routeEntriesArray[cIndexSet].entry.regularEntry.qosProfileIndex = (GT_U32)inFields[8];
    routeEntriesArray[cIndexSet].entry.regularEntry.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[9];
    routeEntriesArray[cIndexSet].entry.regularEntry.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
    routeEntriesArray[cIndexSet].entry.regularEntry.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
    routeEntriesArray[cIndexSet].entry.regularEntry.countSet = (CPSS_IP_CNT_SET_ENT)inFields[12];
    routeEntriesArray[cIndexSet].entry.regularEntry.trapMirrorArpBcEnable = (GT_BOOL)inFields[13];
    routeEntriesArray[cIndexSet].entry.regularEntry.sipAccessLevel = (GT_U32)inFields[14];
    routeEntriesArray[cIndexSet].entry.regularEntry.dipAccessLevel = (GT_U32)inFields[15];
    routeEntriesArray[cIndexSet].entry.regularEntry.ICMPRedirectEnable = (GT_BOOL)inFields[16];
    routeEntriesArray[cIndexSet].entry.regularEntry.scopeCheckingEnable = (GT_BOOL)inFields[17];
    routeEntriesArray[cIndexSet].entry.regularEntry.siteId = (CPSS_IP_SITE_ID_ENT)inFields[18];
    routeEntriesArray[cIndexSet].entry.regularEntry.mtuProfileIndex = (GT_U32)inFields[19];
    routeEntriesArray[cIndexSet].entry.regularEntry.isTunnelStart = (GT_BOOL)inFields[20];
    routeEntriesArray[cIndexSet].entry.regularEntry.isNat = (GT_BOOL)inFields[21];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopVlanId = (GT_U16)inFields[22];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[23];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[24];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[25];
    CONVERT_DEV_PORT_DATA_MAC(routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.hwDevNum ,
        routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.devPort.portNum);
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[26];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.trunkId);
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.vidx = (GT_U16)inFields[27];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopInterface.vlanId = (GT_U16)inFields[28];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopARPPointer = (GT_U32)inFields[29];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopTunnelPointer = (GT_U32)inFields[30];        
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopNatPointer = (GT_U32)inFields[31];
    routeEntriesArray[cIndexSet].entry.regularEntry.nextHopVlanId1 = (GT_U16)inFields[32];
    routeEntriesArray[cIndexSet].entry.regularEntry.ingressMirrorToAnalyzerIndex = (GT_U32)inFields[33];
   
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/****set for type CPSS_DXCH_IP_UC_ECMP_RPF_E***********/

static CMD_STATUS wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ECMP_RPF_E
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



    routeEntriesArray[cIndexSet].type = CPSS_DXCH_IP_UC_ECMP_RPF_E;



    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[0] = (GT_U16)inFields[0];
    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[1] = (GT_U16)inFields[1];
    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[2] = (GT_U16)inFields[2];
    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[3] = (GT_U16)inFields[3];
    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[4] = (GT_U16)inFields[4];
    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[5] = (GT_U16)inFields[5];
    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[6] = (GT_U16)inFields[6];
    routeEntriesArray[cIndexSet].entry.ecmpRpfCheck.vlanArray[7] = (GT_U16)inFields[7];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}
/***********************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesSetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
   cIndexSet=0;


   if(inArgs[4]==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE(inArgs,inFields,numFields,GT_FALSE,outArgs);

    if(inArgs[4]==CPSS_DXCH_IP_UC_ECMP_RPF_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);
    else
        return GT_BAD_PARAM;

}
/***********************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesSetFirst_1
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
   cIndexSet=0;


   if(inArgs[4]==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE(inArgs,inFields,numFields,GT_TRUE,outArgs);

    if(inArgs[4]==CPSS_DXCH_IP_UC_ECMP_RPF_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);
    else
        return GT_BAD_PARAM;

}
/***********************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesSetFirst_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
   cIndexSet=0;

   if(inArgs[4]==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE_2(inArgs,inFields,numFields,outArgs);

    if(inArgs[4]==CPSS_DXCH_IP_UC_ECMP_RPF_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);
    else
        return GT_BAD_PARAM;

}
/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesSetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
   cIndexSet++;

   if(inArgs[4]==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE(inArgs,inFields,numFields,GT_FALSE,outArgs);

    if(inArgs[4]==CPSS_DXCH_IP_UC_ECMP_RPF_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);
    else
        return GT_BAD_PARAM;
}
/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesSetNext_1
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
   cIndexSet++;

   if(inArgs[4]==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE(inArgs,inFields,numFields,GT_TRUE,outArgs);

    if(inArgs[4]==CPSS_DXCH_IP_UC_ECMP_RPF_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);
    else
        return GT_BAD_PARAM;
}
/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesSetNext_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
   cIndexSet++;

   if(inArgs[4]==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ROUTE_2(inArgs,inFields,numFields,outArgs);

    if(inArgs[4]==CPSS_DXCH_IP_UC_ECMP_RPF_E)

      return wrCpssDxChIpUcRouteEntriesSet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);
    else
        return GT_BAD_PARAM;
}

/***************************************************************/
 static CMD_STATUS wrCpssDxChIpUcRouteEntriesEndSet
     (
     IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
     IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
     IN  GT_32 numFields,
     OUT GT_8  outArgs[CMD_MAX_BUFFER]
     )
 {

     GT_STATUS   status;

     GT_U8    dev;
     GT_U32   baseRouteEntryIndex;

     /* check for valid arguments */
     if(!inArgs || !outArgs)
         return CMD_AGENT_ERROR;

     dev = (GT_U8)inArgs[0];
     baseRouteEntryIndex = (GT_U32)inArgs[1];

     /* call cpss api function */
     status =  cpssDxChIpUcRouteEntriesWrite(dev,baseRouteEntryIndex,
         routeEntriesArray,cIndexSet+1);

     if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "%d", -1);
            return CMD_OK;
        }


     /* pack output arguments to galtis string */
     galtisOutput(outArgs, status, "");

     return CMD_OK;
 }

/*******************************************************************************
* cpssDxChIpUcRouteEntriesRead
*
* DESCRIPTION:
*    Reads an array of uc route entries from the hw.
*
* APPLICABLE DEVICES:
*       DxCh2 and above.
*
* INPUTS:
*       devNum                - the device number
*       baseRouteEntryIndex   - the index from which to start reading
*       routeEntriesArray     - the uc route entries array, for each element
*                               set the uc route entry type to determine how
*                               entry data will be interpreted
*       numOfRouteEntries     - the number route entries in the array (= the
*                               number of route entries to read)
*
*
* OUTPUTS:
*       routeEntriesArray     - the uc route entries array read
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on devNum not active.
*       GT_BAD_PTR     - one of the parameters is NULL pointer
*
* COMMENTS:
*       See cpssDxChIpUcRouteEntriesWrite
*
*
*******************************************************************************/
/****get for type CPSS_DXCH_IP_UC_ROUTE_ENTRY_FORMAT_STC***********/

static CMD_STATUS wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE
(
  IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
  IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
  IN  GT_32 numFields,
  IN  GT_BOOL isBobcat2,
  OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

 GT_HW_DEV_NUM tempHwDevNum;
 GT_PORT_NUM   tempPort;

 /* check for valid arguments */
 if(!inArgs || !outArgs)
     return CMD_AGENT_ERROR;

 inFields[0]=routeEntriesArray[cIndexGet].entry.regularEntry.cmd ;
 inFields[1]=routeEntriesArray[cIndexGet].entry.regularEntry.cpuCodeIdx ;
 inFields[2]=routeEntriesArray[cIndexGet].entry.regularEntry.appSpecificCpuCodeEnable  ;
 inFields[3]=routeEntriesArray[cIndexGet].entry.regularEntry.unicastPacketSipFilterEnable ;
 inFields[4]=routeEntriesArray[cIndexGet].entry.regularEntry.ttlHopLimitDecEnable  ;
 inFields[5]=routeEntriesArray[cIndexGet].entry.regularEntry.ttlHopLimDecOptionsExtChkByPass  ;
 inFields[6]=routeEntriesArray[cIndexGet].entry.regularEntry.ingressMirror ;
 inFields[7]=routeEntriesArray[cIndexGet].entry.regularEntry.qosProfileMarkingEnable  ;
 inFields[8]=routeEntriesArray[cIndexGet].entry.regularEntry.qosProfileIndex ;
 inFields[9]=routeEntriesArray[cIndexGet].entry.regularEntry.qosPrecedence ;
 inFields[10]=routeEntriesArray[cIndexGet].entry.regularEntry.modifyUp ;
 inFields[11]=routeEntriesArray[cIndexGet].entry.regularEntry.modifyDscp ;
 inFields[12]=routeEntriesArray[cIndexGet].entry.regularEntry.countSet ;
 inFields[13]=routeEntriesArray[cIndexGet].entry.regularEntry.trapMirrorArpBcEnable  ;
 inFields[14]=routeEntriesArray[cIndexGet].entry.regularEntry.sipAccessLevel ;
 inFields[15]=routeEntriesArray[cIndexGet].entry.regularEntry.dipAccessLevel  ;
 inFields[16] =routeEntriesArray[cIndexGet].entry.regularEntry.ICMPRedirectEnable;
 inFields[17]=routeEntriesArray[cIndexGet].entry.regularEntry.scopeCheckingEnable ;
 inFields[18]=routeEntriesArray[cIndexGet].entry.regularEntry.siteId ;
 inFields[19]=routeEntriesArray[cIndexGet].entry.regularEntry.mtuProfileIndex;
 inFields[20]=routeEntriesArray[cIndexGet].entry.regularEntry.isTunnelStart;
 inFields[21]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopVlanId  ;
 inFields[22]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.type;
 tempHwDevNum=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.devPort.hwDevNum  ;
 tempPort=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.devPort.portNum ;
 CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDevNum,tempPort);
 inFields[23]=tempHwDevNum;
 inFields[24]=tempPort;

 CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.trunkId);
 inFields[25]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.trunkId  ;
 inFields[26]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.vidx  ;
 inFields[27]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.vlanId ;
 inFields[28]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopARPPointer  ;
 inFields[29]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopTunnelPointer  ;

 if (isBobcat2 == GT_TRUE)
 {
     inFields[30]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopVlanId1;
     inFields[31]=routeEntriesArray[cIndexGet].entry.regularEntry.ingressMirrorToAnalyzerIndex;

     /* pack and output table fields */
     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28], inFields[29], inFields[30], inFields[31]);
 }
 else
 {
      /* pack and output table fields */
      fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
         ,inFields[0], inFields[1], inFields[2], inFields[3],
         inFields[4], inFields[5], inFields[6], inFields[7],
         inFields[8], inFields[9], inFields[10], inFields[11],
         inFields[12], inFields[13], inFields[14], inFields[15],
         inFields[16], inFields[17], inFields[18], inFields[19],
         inFields[20], inFields[21], inFields[22], inFields[23],
         inFields[24], inFields[25], inFields[26], inFields[27],
         inFields[28], inFields[29]);
 }

 /* pack output arguments to galtis string */
 galtisOutput(outArgs, GT_OK, "%d%f", routeEntriesArray[cIndexGet].type);

 return CMD_OK;
}

/* 2 new fields from wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE:
   isNat nextHopNatPointer */
static CMD_STATUS wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE_2
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    IN  GT_BOOL isBobcat2,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

 GT_HW_DEV_NUM tempHwDevNum;
 GT_PORT_NUM   tempPort;

 /* check for valid arguments */
 if(!inArgs || !outArgs)
     return CMD_AGENT_ERROR;

 inFields[0]=routeEntriesArray[cIndexGet].entry.regularEntry.cmd ;
 inFields[1]=routeEntriesArray[cIndexGet].entry.regularEntry.cpuCodeIdx ;
 inFields[2]=routeEntriesArray[cIndexGet].entry.regularEntry.appSpecificCpuCodeEnable  ;
 inFields[3]=routeEntriesArray[cIndexGet].entry.regularEntry.unicastPacketSipFilterEnable ;
 inFields[4]=routeEntriesArray[cIndexGet].entry.regularEntry.ttlHopLimitDecEnable  ;
 inFields[5]=routeEntriesArray[cIndexGet].entry.regularEntry.ttlHopLimDecOptionsExtChkByPass  ;
 inFields[6]=routeEntriesArray[cIndexGet].entry.regularEntry.ingressMirror ;
 inFields[7]=routeEntriesArray[cIndexGet].entry.regularEntry.qosProfileMarkingEnable  ;
 inFields[8]=routeEntriesArray[cIndexGet].entry.regularEntry.qosProfileIndex ;
 inFields[9]=routeEntriesArray[cIndexGet].entry.regularEntry.qosPrecedence ;
 inFields[10]=routeEntriesArray[cIndexGet].entry.regularEntry.modifyUp ;
 inFields[11]=routeEntriesArray[cIndexGet].entry.regularEntry.modifyDscp ;
 inFields[12]=routeEntriesArray[cIndexGet].entry.regularEntry.countSet ;
 inFields[13]=routeEntriesArray[cIndexGet].entry.regularEntry.trapMirrorArpBcEnable  ;
 inFields[14]=routeEntriesArray[cIndexGet].entry.regularEntry.sipAccessLevel ;
 inFields[15]=routeEntriesArray[cIndexGet].entry.regularEntry.dipAccessLevel  ;
 inFields[16] =routeEntriesArray[cIndexGet].entry.regularEntry.ICMPRedirectEnable;
 inFields[17]=routeEntriesArray[cIndexGet].entry.regularEntry.scopeCheckingEnable ;
 inFields[18]=routeEntriesArray[cIndexGet].entry.regularEntry.siteId ;
 inFields[19]=routeEntriesArray[cIndexGet].entry.regularEntry.mtuProfileIndex;
 inFields[20]=routeEntriesArray[cIndexGet].entry.regularEntry.isTunnelStart;
 inFields[22]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopVlanId  ;
 inFields[23]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.type;
 tempHwDevNum=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.devPort.hwDevNum  ;
 tempPort=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.devPort.portNum ;
 CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDevNum,tempPort);
 inFields[24]=tempHwDevNum;
 inFields[25]=tempPort;

 CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.trunkId);
 inFields[26]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.trunkId  ;
 inFields[27]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.vidx  ;
 inFields[28]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopInterface.vlanId ;
 inFields[29]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopARPPointer  ;
 inFields[30]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopTunnelPointer  ;

 if (isBobcat2 == GT_TRUE)
 {
     inFields[21]=routeEntriesArray[cIndexGet].entry.regularEntry.isNat;
     inFields[31]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopNatPointer;
     inFields[32]=routeEntriesArray[cIndexGet].entry.regularEntry.nextHopVlanId1;
     inFields[33]=routeEntriesArray[cIndexGet].entry.regularEntry.ingressMirrorToAnalyzerIndex;   
 }
 else
 {
     inFields[21]=0;
     inFields[31]=0;
     inFields[32]=0;
     inFields[33]=0;
 }
 /* pack and output table fields */
 fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
    ,inFields[0], inFields[1], inFields[2], inFields[3],
    inFields[4], inFields[5], inFields[6], inFields[7],
    inFields[8], inFields[9], inFields[10], inFields[11],
    inFields[12], inFields[13], inFields[14], inFields[15],
    inFields[16], inFields[17], inFields[18], inFields[19],
    inFields[20], inFields[21], inFields[22], inFields[23],
    inFields[24], inFields[25], inFields[26], inFields[27],
    inFields[28], inFields[29], inFields[30], inFields[31],
    inFields[32],inFields[33]);

 /* pack output arguments to galtis string */
 galtisOutput(outArgs, GT_OK, "%d%f", routeEntriesArray[cIndexGet].type);

 return CMD_OK;
}

 /*******************get for type CPSS_DXCH_IP_UC_ECMP_RPF_E***********/

static CMD_STATUS wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ECMP_RPF_E
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


    inFields[0]=routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[0] ;
    inFields[1]=routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[1]  ;
    inFields[2]=routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[2] ;
    inFields[3]=routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[3]  ;
    inFields[4]=routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[4]  ;
    inFields[5]= routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[5] ;
    inFields[6]=routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[6]  ;
    inFields[7]=routeEntriesArray[cIndexGet].entry.ecmpRpfCheck.vlanArray[7]  ;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d",inFields[0], inFields[1], inFields[2], inFields[3],
                                inFields[4], inFields[5], inFields[6], inFields[7]);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", routeEntriesArray[cIndexGet].type);

    return CMD_OK;
}

/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{  GT_STATUS   result;
   GT_U8    devNum;
   GT_U32   baseRouteEntryIndex;
   GT_U32 i;
   CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT type;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum=(GT_U8)inArgs[0];
    baseRouteEntryIndex=(GT_U32)inArgs[1];
    numOfEntries=(GT_U32)inArgs[2];
    type=(CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT)inArgs[3];
    cIndexGet=0;
    if(numOfEntries==0)
        numOfEntries=DEFAULT_NUM_OF_ENTRIES;
    for(i=0;i<numOfEntries;i++)
        routeEntriesArray[i].type=type;

    /* call cpss api function */
     result =  cpssDxChIpUcRouteEntriesRead(devNum,baseRouteEntryIndex,
         routeEntriesArray,numOfEntries);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)
        result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE(inArgs,inFields,numFields,GT_TRUE,outArgs);
    else
        if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ECMP_RPF_E )
            result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);

    return CMD_OK;
}

/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesGetFirst_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{  GT_STATUS   result;
   GT_U8    devNum;
   GT_U32   baseRouteEntryIndex;
   GT_U32 i;
   CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT type;
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum=(GT_U8)inArgs[0];
    baseRouteEntryIndex=(GT_U32)inArgs[1];
    numOfEntries=(GT_U32)inArgs[2];
    type=(CPSS_DXCH_IP_UC_ROUTE_ENTRY_TYPE_ENT)inArgs[3];
    cIndexGet=0;
    if(numOfEntries==0)
        numOfEntries=DEFAULT_NUM_OF_ENTRIES;
    for(i=0;i<numOfEntries;i++)
        routeEntriesArray[i].type=type;

    /* call cpss api function */
     result =  cpssDxChIpUcRouteEntriesRead(devNum,baseRouteEntryIndex,
         routeEntriesArray,numOfEntries);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)
        result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE_2(inArgs,inFields,numFields,GT_TRUE,outArgs);
    else
        if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ECMP_RPF_E )
            result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);

    return CMD_OK;
}

/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesGetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS   result=GT_OK;

    cIndexGet++;
    if(cIndexGet>=numOfEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)
        result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE(inArgs,inFields,numFields,GT_FALSE,outArgs);
    else
        if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ECMP_RPF_E )
            result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);

    return result;

}
/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesGetNext_1
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS   result=GT_OK;

    cIndexGet++;
    if(cIndexGet>=numOfEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)
        result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE(inArgs,inFields,numFields,GT_TRUE,outArgs);
    else
        if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ECMP_RPF_E )
            result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);

    return result;

}
/****************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteEntriesGetNext_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS   result=GT_OK;

    cIndexGet++;
    if(cIndexGet>=numOfEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ROUTE_ENTRY_E)
        result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ROUTE_2(inArgs,inFields,numFields,GT_TRUE,outArgs);
    else
        if(routeEntriesArray[cIndexGet].type==CPSS_DXCH_IP_UC_ECMP_RPF_E )
            result=wrCpssDxChIpUcRouteEntriesGet_CPSS_DXCH_IP_UC_ECMP_RPF_E(inArgs,inFields,numFields,outArgs);

    return result;

}

/***********************Table: cpssDxChIpMcRoute****************/
static GT_U32 gRouteEntryIndex;
static GT_U32 gNumToRefresh;

/*******************************************************************************
* cpssDxChIpMcRouteEntriesWrite
*
* DESCRIPTION:
*    Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - the device number
*       RouteEntryIndex - the Index in the Route entries table.
*       routeEntryPtr   - the MC route entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed or
*                         on invalid routeEntryPtr parameter.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntriesWrite
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                               dev;
    GT_U32                              routeEntryIndex;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];

    routeEntryIndex = (GT_U32)inFields[0];
    routeEntry.cmd = (CPSS_PACKET_CMD_ENT)inFields[1];
    routeEntry.cpuCodeIdx = (CPSS_DXCH_IP_CPU_CODE_INDEX_ENT)inFields[2];
    routeEntry.appSpecificCpuCodeEnable = (GT_BOOL)inFields[3];
    routeEntry.ttlHopLimitDecEnable = (GT_BOOL)inFields[5];
    routeEntry.ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[6];
    routeEntry.ingressMirror = (GT_BOOL)inFields[7];
    routeEntry.qosProfileMarkingEnable = (GT_BOOL)inFields[8];
    routeEntry.qosProfileIndex = (GT_U32)inFields[9];
    routeEntry.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[10];
    routeEntry.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
    routeEntry.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
    routeEntry.countSet = (CPSS_IP_CNT_SET_ENT)inFields[13];
    routeEntry.multicastRPFCheckEnable = (GT_BOOL)inFields[14];
    routeEntry.multicastRPFVlan = (GT_U16)inFields[15];
    routeEntry.multicastRPFFailCommandMode =
        (CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT)inFields[16];
    routeEntry.RPFFailCommand = (CPSS_PACKET_CMD_ENT)inFields[17];
    routeEntry.scopeCheckingEnable = (GT_BOOL)inFields[18];
    routeEntry.siteId = (CPSS_IP_SITE_ID_ENT)inFields[19];
    routeEntry.mtuProfileIndex = (GT_U32)inFields[20];
    routeEntry.internalMLLPointer = (GT_U32)inFields[21];
    routeEntry.externalMLLPointer = (GT_U32)inFields[22];
    routeEntry.ingressMirrorToAnalyzerIndex = 0;
    routeEntry.multicastIngressVlanCheck = GT_FALSE;
    routeEntry.multicastRPFRoutingSharedTreeIndex = 0;

    /* call cpss api function */
    status =  cpssDxChIpMcRouteEntriesWrite(dev, routeEntryIndex, &routeEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpMcRouteEntriesWrite_2
*
* DESCRIPTION:
*    Write an array of MC route entries to hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - the device number
*       RouteEntryIndex - the Index in the Route entries table.
*       routeEntryPtr   - the MC route entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed or
*                         on invalid routeEntryPtr parameter.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntriesWrite_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                               dev;
    GT_U32                              routeEntryIndex;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];

    routeEntryIndex = (GT_U32)inFields[0];
    routeEntry.cmd = (CPSS_PACKET_CMD_ENT)inFields[1];
    routeEntry.cpuCodeIdx = (CPSS_DXCH_IP_CPU_CODE_INDEX_ENT)inFields[2];
    routeEntry.appSpecificCpuCodeEnable = (GT_BOOL)inFields[3];
    routeEntry.ttlHopLimitDecEnable = (GT_BOOL)inFields[4];
    routeEntry.ttlHopLimDecOptionsExtChkByPass = (GT_BOOL)inFields[5];
    routeEntry.ingressMirror = (GT_BOOL)inFields[6];
    routeEntry.ingressMirrorToAnalyzerIndex = (GT_U32)inFields[7];
    routeEntry.qosProfileMarkingEnable = (GT_BOOL)inFields[8];
    routeEntry.qosProfileIndex = (GT_U32)inFields[9];
    routeEntry.qosPrecedence = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[10];
    routeEntry.modifyUp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
    routeEntry.modifyDscp = (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
    routeEntry.countSet = (CPSS_IP_CNT_SET_ENT)inFields[13];
    routeEntry.multicastRPFCheckEnable = (GT_BOOL)inFields[14];
    routeEntry.multicastIngressVlanCheck = (CPSS_DXCH_IP_MULTICAST_INGRESS_VLAN_CHECK_ENT)inFields[15];
    routeEntry.multicastRPFVlan = (GT_U16)inFields[16];
    routeEntry.multicastRPFRoutingSharedTreeIndex = (GT_U8)inFields[17];
    routeEntry.multicastRPFFailCommandMode =
        (CPSS_DXCH_IP_MULTICAST_RPF_FAIL_COMMAND_MODE_ENT)inFields[18];
    routeEntry.RPFFailCommand = (CPSS_PACKET_CMD_ENT)inFields[19];
    routeEntry.scopeCheckingEnable = (GT_BOOL)inFields[20];
    routeEntry.siteId = (CPSS_IP_SITE_ID_ENT)inFields[21];
    routeEntry.mtuProfileIndex = (GT_U32)inFields[22];
    routeEntry.internalMLLPointer = (GT_U32)inFields[23];
    routeEntry.externalMLLPointer = (GT_U32)inFields[24];

    /* call cpss api function */
    status =  cpssDxChIpMcRouteEntriesWrite(dev, routeEntryIndex, &routeEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpMcRouteEntriesReadFirst
*
* DESCRIPTION:
*    read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntriesReadFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gRouteEntryIndex = 0;/*reset on first*/

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    cmdOsMemSet(&routeEntry, 0, sizeof(routeEntry));

    /* call cpss api function */
    result = cpssDxChIpMcRouteEntriesRead(dev, gRouteEntryIndex, &routeEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gRouteEntryIndex;
    inFields[1] = routeEntry.cmd;
    inFields[2] = routeEntry.cpuCodeIdx;
    inFields[3] = routeEntry.appSpecificCpuCodeEnable;
    inFields[4] = GT_FALSE;
    inFields[5] = routeEntry.ttlHopLimitDecEnable;
    inFields[6] = routeEntry.ttlHopLimDecOptionsExtChkByPass;
    inFields[7] = routeEntry.ingressMirror;
    inFields[8] = routeEntry.qosProfileMarkingEnable;
    inFields[9] = routeEntry.qosProfileIndex;
    inFields[10] = routeEntry.qosPrecedence;
    inFields[11] = routeEntry.modifyUp;
    inFields[12] = routeEntry.modifyDscp;
    inFields[13] = routeEntry.countSet;
    inFields[14] = routeEntry.multicastRPFCheckEnable;
    inFields[15] = routeEntry.multicastRPFVlan;
    inFields[16] = routeEntry.multicastRPFFailCommandMode;
    inFields[17] = routeEntry.RPFFailCommand;
    inFields[18] = routeEntry.scopeCheckingEnable;
    inFields[19] = routeEntry.siteId;
    inFields[20] = routeEntry.mtuProfileIndex;
    inFields[21] = routeEntry.internalMLLPointer;
    inFields[22] = routeEntry.externalMLLPointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22]);

    galtisOutput(outArgs, result, "%f");

    gRouteEntryIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpMcRouteEntriesReadNext
*
* DESCRIPTION:
*    read an array of MC route entries from the hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on baseRouteEntryIndex bigger then allowed.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntriesReadNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    cmdOsMemSet(&routeEntry, 0, sizeof(routeEntry));

    /* call cpss api function */
    result = cpssDxChIpMcRouteEntriesRead(dev, gRouteEntryIndex, &routeEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gRouteEntryIndex;
    inFields[1] = routeEntry.cmd;
    inFields[2] = routeEntry.cpuCodeIdx;
    inFields[3] = routeEntry.appSpecificCpuCodeEnable;
    inFields[4] = GT_FALSE;
    inFields[5] = routeEntry.ttlHopLimitDecEnable;
    inFields[6] = routeEntry.ttlHopLimDecOptionsExtChkByPass;
    inFields[7] = routeEntry.ingressMirror;
    inFields[8] = routeEntry.qosProfileMarkingEnable;
    inFields[9] = routeEntry.qosProfileIndex;
    inFields[10] = routeEntry.qosPrecedence;
    inFields[11] = routeEntry.modifyUp;
    inFields[12] = routeEntry.modifyDscp;
    inFields[13] = routeEntry.countSet;
    inFields[14] = routeEntry.multicastRPFCheckEnable;
    inFields[15] = routeEntry.multicastRPFVlan;
    inFields[16] = routeEntry.multicastRPFFailCommandMode;
    inFields[17] = routeEntry.RPFFailCommand;
    inFields[18] = routeEntry.scopeCheckingEnable;
    inFields[19] = routeEntry.siteId;
    inFields[20] = routeEntry.mtuProfileIndex;
    inFields[21] = routeEntry.internalMLLPointer;
    inFields[22] = routeEntry.externalMLLPointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22]);

    galtisOutput(outArgs, result, "%f");

    gRouteEntryIndex++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChIpMcRouteEntries_1ReadNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 );

/*******************************************************************************
* wrCpssDxChIpMcRouteEntries_1ReadFirst
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntries_1ReadFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;
    GT_U32                              numToRefresh;/*number of entries to get from table*/
    GT_U32                              baseRouteEntryIndex;/* first entry to get from table */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gRouteEntryIndex = 0;/*reset on first*/
    gNumToRefresh    = 0;/*reset on first*/

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    baseRouteEntryIndex = (GT_U32)inArgs[1];
    numToRefresh = (GT_U32)inArgs[2];

    if(numToRefresh == 0)
    {
        cmdOsPrintf("illegal number of entries to refresh\n");
        galtisOutput(outArgs,GT_OK, "%d", -1);
        return CMD_OK;
    }

    gRouteEntryIndex = baseRouteEntryIndex;
    gNumToRefresh = numToRefresh;

    cmdOsMemSet(&routeEntry, 0, sizeof(routeEntry));

    /* call cpss api function */
    result = cpssDxChIpMcRouteEntriesRead(dev, gRouteEntryIndex, &routeEntry);

    if (result != GT_OK)
    {
        if(result == GT_BAD_STATE)
        {
            /* skip to next valid entry */
            gRouteEntryIndex++;
            gNumToRefresh--;
            wrCpssDxChIpMcRouteEntries_1ReadNext(inArgs,inFields,numFields,outArgs);
            return CMD_OK;
        }
        else
        {
           galtisOutput(outArgs, result, "%d", -1);
           return CMD_OK;
        }
    }

    inFields[0] = gRouteEntryIndex;
    inFields[1] = routeEntry.cmd;
    inFields[2] = routeEntry.cpuCodeIdx;
    inFields[3] = routeEntry.appSpecificCpuCodeEnable;
    inFields[4] = GT_FALSE;
    inFields[5] = routeEntry.ttlHopLimitDecEnable;
    inFields[6] = routeEntry.ttlHopLimDecOptionsExtChkByPass;
    inFields[7] = routeEntry.ingressMirror;
    inFields[8] = routeEntry.qosProfileMarkingEnable;
    inFields[9] = routeEntry.qosProfileIndex;
    inFields[10] = routeEntry.qosPrecedence;
    inFields[11] = routeEntry.modifyUp;
    inFields[12] = routeEntry.modifyDscp;
    inFields[13] = routeEntry.countSet;
    inFields[14] = routeEntry.multicastRPFCheckEnable;
    inFields[15] = routeEntry.multicastRPFVlan;
    inFields[16] = routeEntry.multicastRPFFailCommandMode;
    inFields[17] = routeEntry.RPFFailCommand;
    inFields[18] = routeEntry.scopeCheckingEnable;
    inFields[19] = routeEntry.siteId;
    inFields[20] = routeEntry.mtuProfileIndex;
    inFields[21] = routeEntry.internalMLLPointer;
    inFields[22] = routeEntry.externalMLLPointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22]);

    galtisOutput(outArgs, result, "%f");

    gRouteEntryIndex++;
    gNumToRefresh--;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpMcRouteEntries_1ReadNext
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntries_1ReadNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    if(gNumToRefresh == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(&routeEntry, 0, sizeof(routeEntry));

    /* call cpss api function */
    result = cpssDxChIpMcRouteEntriesRead(dev, gRouteEntryIndex, &routeEntry);

    if (result != GT_OK)
    {
        if(result == GT_BAD_STATE)
        {
            /* skip to next valid entry */
            gRouteEntryIndex++;
            gNumToRefresh--;
            wrCpssDxChIpMcRouteEntries_1ReadNext(inArgs,inFields,numFields,outArgs);
            return CMD_OK;
        }
        else
        {
           galtisOutput(outArgs, result, "%d", -1);
           return CMD_OK;
        }
    }

    inFields[0] = gRouteEntryIndex;
    inFields[1] = routeEntry.cmd;
    inFields[2] = routeEntry.cpuCodeIdx;
    inFields[3] = routeEntry.appSpecificCpuCodeEnable;
    inFields[4] = GT_FALSE;
    inFields[5] = routeEntry.ttlHopLimitDecEnable;
    inFields[6] = routeEntry.ttlHopLimDecOptionsExtChkByPass;
    inFields[7] = routeEntry.ingressMirror;
    inFields[8] = routeEntry.qosProfileMarkingEnable;
    inFields[9] = routeEntry.qosProfileIndex;
    inFields[10] = routeEntry.qosPrecedence;
    inFields[11] = routeEntry.modifyUp;
    inFields[12] = routeEntry.modifyDscp;
    inFields[13] = routeEntry.countSet;
    inFields[14] = routeEntry.multicastRPFCheckEnable;
    inFields[15] = routeEntry.multicastRPFVlan;
    inFields[16] = routeEntry.multicastRPFFailCommandMode;
    inFields[17] = routeEntry.RPFFailCommand;
    inFields[18] = routeEntry.scopeCheckingEnable;
    inFields[19] = routeEntry.siteId;
    inFields[20] = routeEntry.mtuProfileIndex;
    inFields[21] = routeEntry.internalMLLPointer;
    inFields[22] = routeEntry.externalMLLPointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22]);

    galtisOutput(outArgs, result, "%f");

    gRouteEntryIndex++;
    gNumToRefresh--;

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChIpMcRouteEntriesReadNext_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 );

/*******************************************************************************
* wrCpssDxChIpMcRouteEntriesReadFirst_2
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntriesReadFirst_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;
    GT_U32                              numToRefresh;/*number of entries to get from table*/
    GT_U32                              baseRouteEntryIndex;/* first entry to get from table */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gRouteEntryIndex = 0;/*reset on first*/
    gNumToRefresh    = 0;/*reset on first*/

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    baseRouteEntryIndex = (GT_U32)inArgs[1];
    numToRefresh = (GT_U32)inArgs[2];

    if(numToRefresh == 0)
    {
        cmdOsPrintf("illegal number of entries to refresh\n");
        galtisOutput(outArgs,GT_OK, "%d", -1);
        return CMD_OK;
    }

    gRouteEntryIndex = baseRouteEntryIndex;
    gNumToRefresh = numToRefresh;

    cmdOsMemSet(&routeEntry, 0, sizeof(routeEntry));

    /* call cpss api function */
    result = cpssDxChIpMcRouteEntriesRead(dev, gRouteEntryIndex, &routeEntry);

    if (result != GT_OK)
    {
        if(result == GT_BAD_STATE)
        {
            /* skip to next valid entry */
            gRouteEntryIndex++;
            gNumToRefresh--;
            wrCpssDxChIpMcRouteEntriesReadNext_2(inArgs,inFields,numFields,outArgs);
            return CMD_OK;
        }
        else
        {
           galtisOutput(outArgs, result, "%d", -1);
           return CMD_OK;
        }
    }

    inFields[0] = gRouteEntryIndex;
    inFields[1] = routeEntry.cmd;
    inFields[2] = routeEntry.cpuCodeIdx;
    inFields[3] = routeEntry.appSpecificCpuCodeEnable;
    inFields[4] = routeEntry.ttlHopLimitDecEnable;
    inFields[5] = routeEntry.ttlHopLimDecOptionsExtChkByPass;
    inFields[6] = routeEntry.ingressMirror;
    inFields[7] = routeEntry.ingressMirrorToAnalyzerIndex;
    inFields[8] = routeEntry.qosProfileMarkingEnable;
    inFields[9] = routeEntry.qosProfileIndex;
    inFields[10] = routeEntry.qosPrecedence;
    inFields[11] = routeEntry.modifyUp;
    inFields[12] = routeEntry.modifyDscp;
    inFields[13] = routeEntry.countSet;
    inFields[14] = routeEntry.multicastRPFCheckEnable;
    inFields[15] = routeEntry.multicastIngressVlanCheck;
    inFields[16] = routeEntry.multicastRPFVlan;
    inFields[17] = routeEntry.multicastRPFRoutingSharedTreeIndex;
    inFields[18] = routeEntry.multicastRPFFailCommandMode;
    inFields[19] = routeEntry.RPFFailCommand;
    inFields[20] = routeEntry.scopeCheckingEnable;
    inFields[21] = routeEntry.siteId;
    inFields[22] = routeEntry.mtuProfileIndex;
    inFields[23] = routeEntry.internalMLLPointer;
    inFields[24] = routeEntry.externalMLLPointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24]);

    galtisOutput(outArgs, result, "%f");

    gRouteEntryIndex++;
    gNumToRefresh--;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpMcRouteEntriesReadNext_2
*
* DESCRIPTION:
*    Reads a MC route entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum          - the device number
*       routeEntryIndex - the route entry index.
*
* OUTPUTS:
*       routeEntryPtr   - the MC route entry read
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_STATE    - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMcRouteEntriesReadNext_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                               dev;
    CPSS_DXCH_IP_MC_ROUTE_ENTRY_STC     routeEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    if(gNumToRefresh == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(&routeEntry, 0, sizeof(routeEntry));

    /* call cpss api function */
    result = cpssDxChIpMcRouteEntriesRead(dev, gRouteEntryIndex, &routeEntry);

    if (result != GT_OK)
    {
        if(result == GT_BAD_STATE)
        {
            /* skip to next valid entry */
            gRouteEntryIndex++;
            gNumToRefresh--;
            wrCpssDxChIpMcRouteEntriesReadNext_2(inArgs,inFields,numFields,outArgs);
            return CMD_OK;
        }
        else
        {
           galtisOutput(outArgs, result, "%d", -1);
           return CMD_OK;
        }
    }

    inFields[0] = gRouteEntryIndex;
    inFields[1] = routeEntry.cmd;
    inFields[2] = routeEntry.cpuCodeIdx;
    inFields[3] = routeEntry.appSpecificCpuCodeEnable;
    inFields[4] = routeEntry.ttlHopLimitDecEnable;
    inFields[5] = routeEntry.ttlHopLimDecOptionsExtChkByPass;
    inFields[6] = routeEntry.ingressMirror;
    inFields[7] = routeEntry.ingressMirrorToAnalyzerIndex;
    inFields[8] = routeEntry.qosProfileMarkingEnable;
    inFields[9] = routeEntry.qosProfileIndex;
    inFields[10] = routeEntry.qosPrecedence;
    inFields[11] = routeEntry.modifyUp;
    inFields[12] = routeEntry.modifyDscp;
    inFields[13] = routeEntry.countSet;
    inFields[14] = routeEntry.multicastRPFCheckEnable;
    inFields[15] = routeEntry.multicastIngressVlanCheck;
    inFields[16] = routeEntry.multicastRPFVlan;
    inFields[17] = routeEntry.multicastRPFRoutingSharedTreeIndex;
    inFields[18] = routeEntry.multicastRPFFailCommandMode;
    inFields[19] = routeEntry.RPFFailCommand;
    inFields[20] = routeEntry.scopeCheckingEnable;
    inFields[21] = routeEntry.siteId;
    inFields[22] = routeEntry.mtuProfileIndex;
    inFields[23] = routeEntry.internalMLLPointer;
    inFields[24] = routeEntry.externalMLLPointer;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24]);

    galtisOutput(outArgs, result, "%f");

    gRouteEntryIndex++;
    gNumToRefresh--;

    return CMD_OK;
}

static GT_U32 ecmpEntryIndex;
static GT_U32 ecmpLastEntryIndex;

/*******************************************************************************
* cpssDxChIpEcmpEntryWrite
*
* DESCRIPTION:
*       Write an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpEntrySet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       dev;
    GT_U32      index;
    CPSS_DXCH_IP_ECMP_ENTRY_STC ecmpEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    index = (GT_U32)inFields[0];
    ecmpEntry.randomEnable = (GT_BOOL)inFields[1];
    ecmpEntry.numOfPaths = (GT_U32)inFields[2];
    ecmpEntry.routeEntryBaseIndex = (GT_U32)inFields[3];

    /* call cpss api function */
    result = cpssDxChIpEcmpEntryWrite(dev, index, &ecmpEntry);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpEntryRead
*
* DESCRIPTION:
*       Read an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpEntryGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       dev;
    CPSS_DXCH_IP_ECMP_ENTRY_STC ecmpEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    ecmpEntryIndex = (GT_U32)inArgs[1];
    numOfEntries = (GT_U32)inArgs[2];
    ecmpLastEntryIndex = numOfEntries + ecmpEntryIndex - 1;

    if (numOfEntries == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChIpEcmpEntryRead(dev, ecmpEntryIndex, &ecmpEntry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ecmpEntryIndex;
    inFields[1] = ecmpEntry.randomEnable;
    inFields[2] = ecmpEntry.numOfPaths;
    inFields[3] = ecmpEntry.routeEntryBaseIndex;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpEntryRead
*
* DESCRIPTION:
*       Read an ECMP entry
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       ecmpEntryIndex  - the index of the entry in the ECMP table
*                         (APPLICABLE RANGES: 0..12287)
*
* OUTPUTS:
*       ecmpEntryPtr    - (pointer to) the ECMP entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpEntryGetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       dev;
    CPSS_DXCH_IP_ECMP_ENTRY_STC ecmpEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    ecmpEntryIndex++;

    if (ecmpEntryIndex > ecmpLastEntryIndex)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChIpEcmpEntryRead(dev, ecmpEntryIndex, &ecmpEntry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ecmpEntryIndex;
    inFields[1] = ecmpEntry.randomEnable;
    inFields[2] = ecmpEntry.numOfPaths;
    inFields[3] = ecmpEntry.routeEntryBaseIndex;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*Table:  cpssDxChIpRouterNextHopTableAgeBits*************/
static      GT_U32          gRouterNextHopTableAgeBitsEntry;

static      GT_U32          gRouterNextHopTableAgeBitsEntryGet;
static      GT_U32          gRouterEntIndex;
/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
*
* DESCRIPTION:
*    set router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*                                             bits.
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerNextHopTableAgeBitsEntryIndex bigger then 128
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status = GT_OK;

    GT_U8           dev;
    GT_U32          routerNextHopTableAgeBitsEntryIndex;
    GT_U32          routeEntryIndex;
    GT_BOOL         activityBit;
    GT_U8           bitOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];

    routeEntryIndex = (GT_U32)inFields[0];
    activityBit = (GT_BOOL)inFields[1];

    /*showing each 32b entry as 32  bool entries*/
    routerNextHopTableAgeBitsEntryIndex = routeEntryIndex / 32;
    bitOffset = (GT_U8)(routeEntryIndex % 32);

    /*reading the entry as a base for changes*/
    cpssDxChIpRouterNextHopTableAgeBitsEntryRead(dev,
        routerNextHopTableAgeBitsEntryIndex,
        &gRouterNextHopTableAgeBitsEntry);

    U32_SET_FIELD_MASKED_MAC(gRouterNextHopTableAgeBitsEntry,bitOffset,1,BOOL2BIT_MAC(activityBit));

    /*if its the end of the 32b entry write it. if not go on accumulating it in
    wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteNext*/
    if(bitOffset == 31)
    {
        /* call cpss api function */
        status =  cpssDxChIpRouterNextHopTableAgeBitsEntryWrite(dev,
            routerNextHopTableAgeBitsEntryIndex,
            gRouterNextHopTableAgeBitsEntry);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
*
* DESCRIPTION:
*    set router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*                                             bits.
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerNextHopTableAgeBitsEntryIndex bigger then 128
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status = GT_OK;

    GT_U8           dev;
    GT_U32          routerNextHopTableAgeBitsEntryIndex;
    GT_U32          routeEntryIndex;
    GT_BOOL         activityBit;
    GT_U8           bitOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];

    routeEntryIndex = (GT_U32)inFields[0];
    activityBit = (GT_BOOL)inFields[1];

    /*showing each 32b entry as 32  bool entries*/
    routerNextHopTableAgeBitsEntryIndex = routeEntryIndex / 32;
    bitOffset = (GT_U8)(routeEntryIndex % 32);

    /*reading the entry as a base for changes*/
    if(bitOffset == 0)
    {
        cpssDxChIpRouterNextHopTableAgeBitsEntryRead(dev,
            routerNextHopTableAgeBitsEntryIndex,
            &gRouterNextHopTableAgeBitsEntry);
    }

    U32_SET_FIELD_MASKED_MAC(gRouterNextHopTableAgeBitsEntry,bitOffset,1,BOOL2BIT_MAC(activityBit));

    /*if its the end of the 32b entry write it. if not go on accumulating it in
    wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteNext*/
    if(bitOffset == 31)
    {
        /* call cpss api function */
        status =  cpssDxChIpRouterNextHopTableAgeBitsEntryWrite(dev,
            routerNextHopTableAgeBitsEntryIndex,
            gRouterNextHopTableAgeBitsEntry);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpRouterNextHopTableAgeBitsEntryWrite
*
* DESCRIPTION:
*    set router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*                                             bits.
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerNextHopTableAgeBitsEntryIndex bigger then 128
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteEnd
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status = GT_OK;

    GT_U8           dev;
    GT_U32          routerNextHopTableAgeBitsEntryIndex;
    GT_U32          routeEntryIndex;
    GT_U8           bitOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];

    routeEntryIndex = (GT_U32)inFields[0];

    routerNextHopTableAgeBitsEntryIndex = routeEntryIndex / 32;
    bitOffset = (GT_U8)(routeEntryIndex % 32);

    if(bitOffset != 31)/*last full word was written already*/
    {
        /* call cpss api function */
        status =  cpssDxChIpRouterNextHopTableAgeBitsEntryWrite(dev,
            routerNextHopTableAgeBitsEntryIndex,
            gRouterNextHopTableAgeBitsEntry);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouteEntryActiveBitRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
* OUTPUTS:
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerNextHopTableAgeBitsEntryIndex bigger then 128
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterNextHopTableAgeBitsEntryReadFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U32  rangeStart;
    GT_U32  routerNextHopTableAgeBitsEntryIndex;
    GT_U8   bitOffset;
    GT_BOOL activityBit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    rangeStart = (GT_U32)inArgs[1];

    gRouterEntIndex = rangeStart;/*reset on first*/
    routerNextHopTableAgeBitsEntryIndex = gRouterEntIndex / 32;
    bitOffset = (GT_U8)(gRouterEntIndex % 32);

    /* call cpss api function */
    result = cpssDxChIpRouterNextHopTableAgeBitsEntryRead(dev,
        routerNextHopTableAgeBitsEntryIndex,
        &gRouterNextHopTableAgeBitsEntryGet);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    activityBit =
        (GT_BOOL)((gRouterNextHopTableAgeBitsEntryGet >> bitOffset) & 0x1);

    inFields[0] = gRouterEntIndex;
    inFields[1] = activityBit;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, result, "%f");

    gRouterEntIndex++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpRouteEntryActiveBitRead
*
* DESCRIPTION:
*     read router next hop table age bits entry.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                              - the device number
*       routerNextHopTableAgeBitsEntryIndex - the router next hop table age bits
*                                             entry index. each entry is 32 Age
*                                             bits.
* OUTPUTS:
*       routerNextHopTableAgeBitsEntry      - a 32 Age Bit map for route entries
*                                             <32*routeEntriesIndexesArrayPtr>..
*                                             <32*routeEntriesIndexesArrayPtr+31>
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerNextHopTableAgeBitsEntryIndex bigger then 128
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterNextHopTableAgeBitsEntryReadNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result = GT_OK;

    GT_U8   dev;
    GT_U32  rangeEnd;
    GT_U32  routerNextHopTableAgeBitsEntryIndex;
    GT_U8   bitOffset;
    GT_BOOL activityBit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    rangeEnd = (GT_U32)inArgs[2];

    if(gRouterEntIndex > rangeEnd)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    routerNextHopTableAgeBitsEntryIndex = gRouterEntIndex / 32;
    bitOffset = (GT_U8)(gRouterEntIndex % 32);

    if(bitOffset == 0)
    {
        /* call cpss api function */
        result = cpssDxChIpRouterNextHopTableAgeBitsEntryRead(dev,
            routerNextHopTableAgeBitsEntryIndex,
            &gRouterNextHopTableAgeBitsEntryGet);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
    }

    activityBit =
        (GT_BOOL)((gRouterNextHopTableAgeBitsEntryGet >> bitOffset) & 0x1);

    inFields[0] = gRouterEntIndex;
    inFields[1] = activityBit;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, result, "%f");

    gRouterEntIndex++;

    return CMD_OK;
}

/**************Table: cpssDxChIpMLLPair****************/
#define MAX_MLL_ENTRIES 2048
GT_U32  gMllPairEntryIndex;
/*******************************************************************************
* cpssDxChIpMLLPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index.
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       MLL entries are two words long. The atomic hw write operation is done
*       on one word. Writing order of MLL parameters could be significant as
*       some parameters in word[1],[3] depend on parameters in word[0],[2].
*       Erroneous handling of mllRPFFailCommand/ nextHopTunnelPointer may
*       result with bad traffic. nextPointer may result with PP infinite loop.
*       cpssDxChIpMLLPairWrite handles the nextPointer parameter in the
*       following way:
*       if (last == 1) first set word[0] or word[2].
*       if (last == 0) first set word[3].
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairWrite
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                                       devNum;
    GT_U32                                      mllPairEntryIndex;
    CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairWriteForm;
    CPSS_DXCH_IP_MLL_PAIR_STC                   mllPairEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    mllPairEntryIndex = (GT_U32)inFields[0];
    mllPairWriteForm = (CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT)inFields[1];
    mllPairEntry.nextPointer = (GT_U16)inFields[2];

    if(mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E ||
        mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E)
    {
        mllPairEntry.firstMllNode.mllRPFFailCommand = (CPSS_PACKET_CMD_ENT)inFields[3];
        mllPairEntry.firstMllNode.isTunnelStart = (GT_BOOL)inFields[4];
        mllPairEntry.firstMllNode.nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[5];

        mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[6];
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[7];
        CONVERT_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
            mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);

        mllPairEntry.firstMllNode.nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[8];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
        mllPairEntry.firstMllNode.nextHopInterface.vidx = (GT_U16)inFields[9];
        mllPairEntry.firstMllNode.nextHopInterface.vlanId = (GT_U16)inFields[10];
        mllPairEntry.firstMllNode.nextHopVlanId = (GT_U16)inFields[11];
        mllPairEntry.firstMllNode.nextHopTunnelPointer = (GT_U32)inFields[12];
        mllPairEntry.firstMllNode.tunnelStartPassengerType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
        mllPairEntry.firstMllNode.ttlHopLimitThreshold = (GT_U16)inFields[13];
        mllPairEntry.firstMllNode.excludeSrcVlan = (GT_BOOL)inFields[14];
        mllPairEntry.firstMllNode.last = (GT_BOOL)inFields[15];
    }

    if(mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E ||
        mllPairWriteForm ==
        CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E)
    {
        mllPairEntry.secondMllNode.mllRPFFailCommand = (CPSS_PACKET_CMD_ENT)inFields[16];
        mllPairEntry.secondMllNode.isTunnelStart = (GT_BOOL)inFields[17];
        mllPairEntry.secondMllNode.nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[18];

        mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[19];
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[20];
        CONVERT_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
            mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);

        mllPairEntry.secondMllNode.nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[21];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
        mllPairEntry.secondMllNode.nextHopInterface.vidx = (GT_U16)inFields[22];
        mllPairEntry.secondMllNode.nextHopInterface.vlanId = (GT_U16)inFields[23];
        mllPairEntry.secondMllNode.nextHopVlanId = (GT_U16)inFields[24];
        mllPairEntry.secondMllNode.nextHopTunnelPointer = (GT_U32)inFields[25];
        mllPairEntry.secondMllNode.tunnelStartPassengerType = CPSS_DXCH_TUNNEL_PASSENGER_OTHER_E;
        mllPairEntry.secondMllNode.ttlHopLimitThreshold = (GT_U16)inFields[26];
        mllPairEntry.secondMllNode.excludeSrcVlan = (GT_BOOL)inFields[27];
        mllPairEntry.secondMllNode.last = (GT_BOOL)inFields[28];
    }

    /* call cpss api function */
    status =  cpssDxChIpMLLPairWrite(devNum, mllPairEntryIndex,
        mllPairWriteForm, &mllPairEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index.
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on devNum not active.
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
 *******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairReadFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                       devNum;
    GT_U32                      rangeStart;
    CPSS_DXCH_IP_MLL_PAIR_STC   mllPairEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rangeStart = (GT_U32)inArgs[1];

    gMllPairEntryIndex = rangeStart;/*reset on first*/

    /* call cpss api function */
    result = cpssDxChIpMLLPairRead(devNum, gMllPairEntryIndex,
                                   CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E, &mllPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gMllPairEntryIndex;
    inFields[2] = mllPairEntry.nextPointer;
    inFields[3] = mllPairEntry.firstMllNode.mllRPFFailCommand;
    inFields[4] = mllPairEntry.firstMllNode.isTunnelStart;
    inFields[5] = mllPairEntry.firstMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);
    inFields[6] = mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[7] = mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
    inFields[8] = mllPairEntry.firstMllNode.nextHopInterface.trunkId;
    inFields[9] = mllPairEntry.firstMllNode.nextHopInterface.vidx;
    inFields[10] = mllPairEntry.firstMllNode.nextHopInterface.vlanId;
    inFields[11] = mllPairEntry.firstMllNode.nextHopVlanId;
    inFields[12] = mllPairEntry.firstMllNode.nextHopTunnelPointer;
    inFields[13] = mllPairEntry.firstMllNode.ttlHopLimitThreshold;
    inFields[14] = mllPairEntry.firstMllNode.excludeSrcVlan;
    inFields[15] = mllPairEntry.firstMllNode.last;
    inFields[16] = mllPairEntry.secondMllNode.mllRPFFailCommand;
    inFields[17] = mllPairEntry.secondMllNode.isTunnelStart;
    inFields[18] = mllPairEntry.secondMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);
    inFields[19] = mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[20] = mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
    inFields[21] = mllPairEntry.secondMllNode.nextHopInterface.trunkId;
    inFields[22] = mllPairEntry.secondMllNode.nextHopInterface.vidx;
    inFields[23] = mllPairEntry.secondMllNode.nextHopInterface.vlanId;
    inFields[24] = mllPairEntry.secondMllNode.nextHopVlanId;
    inFields[25] = mllPairEntry.secondMllNode.nextHopTunnelPointer;
    inFields[26] = mllPairEntry.secondMllNode.ttlHopLimitThreshold;
    inFields[27] = mllPairEntry.secondMllNode.excludeSrcVlan;
    inFields[28] = mllPairEntry.secondMllNode.last;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28]);

    galtisOutput(outArgs, result, "%f");

    gMllPairEntryIndex++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index.
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on devNum not active.
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairReadNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                       devNum;
    GT_U32                      rangeStart;
    GT_U32                      rangeEnd;
    CPSS_DXCH_IP_MLL_PAIR_STC   mllPairEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rangeStart = (GT_U32)inArgs[1];
    rangeEnd = (GT_U32)inArgs[2];

    /*if the user doesn't enter a range show the whole range*/
    if(rangeStart == 0 && rangeEnd == 0)
    {
        rangeEnd = MAX_MLL_ENTRIES - 1;
    }

    if(gMllPairEntryIndex > rangeEnd)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChIpMLLPairRead(devNum, gMllPairEntryIndex,
                                   CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E, &mllPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gMllPairEntryIndex;
    inFields[2] = mllPairEntry.nextPointer;
    inFields[3] = mllPairEntry.firstMllNode.mllRPFFailCommand;
    inFields[4] = mllPairEntry.firstMllNode.isTunnelStart;
    inFields[5] = mllPairEntry.firstMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);
    inFields[6] = mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[7] = mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
    inFields[8] = mllPairEntry.firstMllNode.nextHopInterface.trunkId;
    inFields[9] = mllPairEntry.firstMllNode.nextHopInterface.vidx;
    inFields[10] = mllPairEntry.firstMllNode.nextHopInterface.vlanId;
    inFields[11] = mllPairEntry.firstMllNode.nextHopVlanId;
    inFields[12] = mllPairEntry.firstMllNode.nextHopTunnelPointer;
    inFields[13] = mllPairEntry.firstMllNode.ttlHopLimitThreshold;
    inFields[14] = mllPairEntry.firstMllNode.excludeSrcVlan;
    inFields[15] = mllPairEntry.firstMllNode.last;
    inFields[16] = mllPairEntry.secondMllNode.mllRPFFailCommand;
    inFields[17] = mllPairEntry.secondMllNode.isTunnelStart;
    inFields[18] = mllPairEntry.secondMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);
    inFields[19] = mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[20] = mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
    inFields[21] = mllPairEntry.secondMllNode.nextHopInterface.trunkId;
    inFields[22] = mllPairEntry.secondMllNode.nextHopInterface.vidx;
    inFields[23] = mllPairEntry.secondMllNode.nextHopInterface.vlanId;
    inFields[24] = mllPairEntry.secondMllNode.nextHopVlanId;
    inFields[25] = mllPairEntry.secondMllNode.nextHopTunnelPointer;
    inFields[26] = mllPairEntry.secondMllNode.ttlHopLimitThreshold;
    inFields[27] = mllPairEntry.secondMllNode.excludeSrcVlan;
    inFields[28] = mllPairEntry.secondMllNode.last;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28]);

    galtisOutput(outArgs, result, "%f");

    gMllPairEntryIndex++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairReadFormFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                                        devNum;
    GT_U32                                       rangeStart;
    GT_U32                                       rangeEnd;
    CPSS_DXCH_IP_MLL_PAIR_STC                    mllPairEntry;
    CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairReadForm;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rangeStart = (GT_U32)inArgs[1];
    rangeEnd = (GT_U32)inArgs[2];
    if((rangeStart!=0)&&(rangeStart==rangeEnd))
    {
        mllPairReadForm = (CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT)inArgs[3];
    }
    else
    {
        mllPairReadForm = CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E;
    }

    gMllPairEntryIndex = rangeStart;/*reset on first*/

     /* call cpss api function */
     cmdOsMemSet(&mllPairEntry, 0, sizeof(mllPairEntry));

    /* call cpss api function */
    result = cpssDxChIpMLLPairRead(devNum, gMllPairEntryIndex,
                                   mllPairReadForm, &mllPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gMllPairEntryIndex;
    inFields[2] = mllPairEntry.nextPointer;
    inFields[3] = mllPairEntry.firstMllNode.mllRPFFailCommand;
    inFields[4] = mllPairEntry.firstMllNode.isTunnelStart;
    inFields[5] = mllPairEntry.firstMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);
    inFields[6] = mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[7] = mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
    inFields[8] = mllPairEntry.firstMllNode.nextHopInterface.trunkId;
    inFields[9] = mllPairEntry.firstMllNode.nextHopInterface.vidx;
    inFields[10] = mllPairEntry.firstMllNode.nextHopInterface.vlanId;
    inFields[11] = mllPairEntry.firstMllNode.nextHopVlanId;
    inFields[12] = mllPairEntry.firstMllNode.nextHopTunnelPointer;
    inFields[13] = mllPairEntry.firstMllNode.ttlHopLimitThreshold;
    inFields[14] = mllPairEntry.firstMllNode.excludeSrcVlan;
    inFields[15] = mllPairEntry.firstMllNode.last;
    inFields[16] = mllPairEntry.secondMllNode.mllRPFFailCommand;
    inFields[17] = mllPairEntry.secondMllNode.isTunnelStart;
    inFields[18] = mllPairEntry.secondMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);
    inFields[19] = mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[20] = mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
    inFields[21] = mllPairEntry.secondMllNode.nextHopInterface.trunkId;
    inFields[22] = mllPairEntry.secondMllNode.nextHopInterface.vidx;
    inFields[23] = mllPairEntry.secondMllNode.nextHopInterface.vlanId;
    inFields[24] = mllPairEntry.secondMllNode.nextHopVlanId;
    inFields[25] = mllPairEntry.secondMllNode.nextHopTunnelPointer;
    inFields[26] = mllPairEntry.secondMllNode.ttlHopLimitThreshold;
    inFields[27] = mllPairEntry.secondMllNode.excludeSrcVlan;
    inFields[28] = mllPairEntry.secondMllNode.last;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28]);

    galtisOutput(outArgs, result, "%f");

    gMllPairEntryIndex++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairReadFormNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                                           devNum;
    GT_U32                                          rangeStart;
    GT_U32                                          rangeEnd;
    CPSS_DXCH_IP_MLL_PAIR_STC                       mllPairEntry;
    CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT              mllPairReadForm;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rangeStart = (GT_U32)inArgs[1];
    rangeEnd = (GT_U32)inArgs[2];

    /*if the user doesn't enter a range show the whole range*/
    if(rangeStart == 0 && rangeEnd == 0)
    {
        rangeEnd = MAX_MLL_ENTRIES - 1;
    }

    if(gMllPairEntryIndex > rangeEnd)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(gMllPairEntryIndex == rangeEnd)
    {
        mllPairReadForm = (CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT)inArgs[3];
    }
    else
    {
        mllPairReadForm = CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E;
    }

    /* call cpss api function */
    cmdOsMemSet(&mllPairEntry, 0, sizeof(mllPairEntry));

    /* call cpss api function */
    result = cpssDxChIpMLLPairRead(devNum, gMllPairEntryIndex,
                                   mllPairReadForm, &mllPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gMllPairEntryIndex;
    inFields[2] = mllPairEntry.nextPointer;
    inFields[3] = mllPairEntry.firstMllNode.mllRPFFailCommand;
    inFields[4] = mllPairEntry.firstMllNode.isTunnelStart;
    inFields[5] = mllPairEntry.firstMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);
    inFields[6] = mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[7] = mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
    inFields[8] = mllPairEntry.firstMllNode.nextHopInterface.trunkId;
    inFields[9] = mllPairEntry.firstMllNode.nextHopInterface.vidx;
    inFields[10] = mllPairEntry.firstMllNode.nextHopInterface.vlanId;
    inFields[11] = mllPairEntry.firstMllNode.nextHopVlanId;
    inFields[12] = mllPairEntry.firstMllNode.nextHopTunnelPointer;
    inFields[13] = mllPairEntry.firstMllNode.ttlHopLimitThreshold;
    inFields[14] = mllPairEntry.firstMllNode.excludeSrcVlan;
    inFields[15] = mllPairEntry.firstMllNode.last;
    inFields[16] = mllPairEntry.secondMllNode.mllRPFFailCommand;
    inFields[17] = mllPairEntry.secondMllNode.isTunnelStart;
    inFields[18] = mllPairEntry.secondMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);
    inFields[19] = mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[20] = mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
    inFields[21] = mllPairEntry.secondMllNode.nextHopInterface.trunkId;
    inFields[22] = mllPairEntry.secondMllNode.nextHopInterface.vidx;
    inFields[23] = mllPairEntry.secondMllNode.nextHopInterface.vlanId;
    inFields[24] = mllPairEntry.secondMllNode.nextHopVlanId;
    inFields[25] = mllPairEntry.secondMllNode.nextHopTunnelPointer;
    inFields[26] = mllPairEntry.secondMllNode.ttlHopLimitThreshold;
    inFields[27] = mllPairEntry.secondMllNode.excludeSrcVlan;
    inFields[28] = mllPairEntry.secondMllNode.last;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28]);

    galtisOutput(outArgs, result, "%f");

    gMllPairEntryIndex++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMLLPairWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) pair entry to hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index.
*       mllPairWriteForm  - the way to write the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllPairWriteForm.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       MLL entries are two words long. The atomic hw write operation is done
*       on one word. Writing order of MLL parameters could be significant as
*       some parameters in word[1],[3] depend on parameters in word[0],[2].
*       Erroneous handling of mllRPFFailCommand/ nextHopTunnelPointer may
*       result with bad traffic. nextPointer may result with PP infinite loop.
*       cpssDxChIpMLLPairWrite handles the nextPointer parameter in the
*       following way:
*       if (last == 1) first set word[0] or word[2].
*       if (last == 0) first set word[3].
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairWrite_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                                       devNum;
    GT_U32                                      mllPairEntryIndex;
    CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT          mllPairWriteForm;
    CPSS_DXCH_IP_MLL_PAIR_STC                   mllPairEntry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    mllPairEntryIndex = (GT_U32)inFields[0];
    mllPairWriteForm = (CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT)inFields[1];
    mllPairEntry.nextPointer = (GT_U16)inFields[2];

    if(mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E ||
        mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_FIRST_ONLY_E)
    {
        mllPairEntry.firstMllNode.mllRPFFailCommand = (CPSS_PACKET_CMD_ENT)inFields[3];
        mllPairEntry.firstMllNode.isTunnelStart = (GT_BOOL)inFields[4];
        mllPairEntry.firstMllNode.nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[5];

        mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[6];
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[7];
        CONVERT_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
            mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);

        mllPairEntry.firstMllNode.nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[8];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
        mllPairEntry.firstMllNode.nextHopInterface.vidx = (GT_U16)inFields[9];
        mllPairEntry.firstMllNode.nextHopInterface.vlanId = (GT_U16)inFields[10];
        mllPairEntry.firstMllNode.nextHopVlanId = (GT_U16)inFields[11];
        mllPairEntry.firstMllNode.nextHopTunnelPointer = (GT_U32)inFields[12];
        mllPairEntry.firstMllNode.tunnelStartPassengerType = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[13];
        mllPairEntry.firstMllNode.ttlHopLimitThreshold = (GT_U16)inFields[14];
        mllPairEntry.firstMllNode.excludeSrcVlan = (GT_BOOL)inFields[15];
        mllPairEntry.firstMllNode.last = (GT_BOOL)inFields[16];
    }

    if(mllPairWriteForm == CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E ||
        mllPairWriteForm ==
        CPSS_DXCH_PAIR_READ_WRITE_SECOND_NEXT_POINTER_ONLY_E)
    {
        mllPairEntry.secondMllNode.mllRPFFailCommand = (CPSS_PACKET_CMD_ENT)inFields[17];
        mllPairEntry.secondMllNode.isTunnelStart = (GT_BOOL)inFields[18];
        mllPairEntry.secondMllNode.nextHopInterface.type = (CPSS_INTERFACE_TYPE_ENT)inFields[19];

        mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum = (GT_HW_DEV_NUM)inFields[20];
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum = (GT_PORT_NUM)inFields[21];
        CONVERT_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
            mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);

        mllPairEntry.secondMllNode.nextHopInterface.trunkId = (GT_TRUNK_ID)inFields[22];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
        mllPairEntry.secondMllNode.nextHopInterface.vidx = (GT_U16)inFields[23];
        mllPairEntry.secondMllNode.nextHopInterface.vlanId = (GT_U16)inFields[24];
        mllPairEntry.secondMllNode.nextHopVlanId = (GT_U16)inFields[25];
        mllPairEntry.secondMllNode.nextHopTunnelPointer = (GT_U32)inFields[26];
        mllPairEntry.secondMllNode.tunnelStartPassengerType = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[27];
        mllPairEntry.secondMllNode.ttlHopLimitThreshold = (GT_U16)inFields[28];
        mllPairEntry.secondMllNode.excludeSrcVlan = (GT_BOOL)inFields[29];
        mllPairEntry.secondMllNode.last = (GT_BOOL)inFields[30];
    }

    /* call cpss api function */
    status =  cpssDxChIpMLLPairWrite(devNum, mllPairEntryIndex,
        mllPairWriteForm, &mllPairEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairReadFormFirst_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                                        devNum;
    GT_U32                                       rangeStart;
    GT_U32                                       rangeEnd;
    CPSS_DXCH_IP_MLL_PAIR_STC                    mllPairEntry;
    CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT           mllPairReadForm;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rangeStart = (GT_U32)inArgs[1];
    rangeEnd = (GT_U32)inArgs[2];
    if((rangeStart!=0)&&(rangeStart==rangeEnd))
    {
        mllPairReadForm = (CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT)inArgs[3];
    }
    else
    {
        mllPairReadForm = CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E;
    }

    gMllPairEntryIndex = rangeStart;/*reset on first*/

     /* call cpss api function */
     cmdOsMemSet(&mllPairEntry, 0, sizeof(mllPairEntry));

    /* call cpss api function */
    result = cpssDxChIpMLLPairRead(devNum, gMllPairEntryIndex,
                                   mllPairReadForm, &mllPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gMllPairEntryIndex;
    inFields[2] = mllPairEntry.nextPointer;
    inFields[3] = mllPairEntry.firstMllNode.mllRPFFailCommand;
    inFields[4] = mllPairEntry.firstMllNode.isTunnelStart;
    inFields[5] = mllPairEntry.firstMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);
    inFields[6] = mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[7] = mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
    inFields[8] = mllPairEntry.firstMllNode.nextHopInterface.trunkId;
    inFields[9] = mllPairEntry.firstMllNode.nextHopInterface.vidx;
    inFields[10] = mllPairEntry.firstMllNode.nextHopInterface.vlanId;
    inFields[11] = mllPairEntry.firstMllNode.nextHopVlanId;
    inFields[12] = mllPairEntry.firstMllNode.nextHopTunnelPointer;
    inFields[13] = mllPairEntry.firstMllNode.tunnelStartPassengerType;
    inFields[14] = mllPairEntry.firstMllNode.ttlHopLimitThreshold;
    inFields[15] = mllPairEntry.firstMllNode.excludeSrcVlan;
    inFields[16] = mllPairEntry.firstMllNode.last;
    inFields[17] = mllPairEntry.secondMllNode.mllRPFFailCommand;
    inFields[18] = mllPairEntry.secondMllNode.isTunnelStart;
    inFields[19] = mllPairEntry.secondMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);
    inFields[20] = mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[21] = mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
    inFields[22] = mllPairEntry.secondMllNode.nextHopInterface.trunkId;
    inFields[23] = mllPairEntry.secondMllNode.nextHopInterface.vidx;
    inFields[24] = mllPairEntry.secondMllNode.nextHopInterface.vlanId;
    inFields[25] = mllPairEntry.secondMllNode.nextHopVlanId;
    inFields[26] = mllPairEntry.secondMllNode.nextHopTunnelPointer;
    inFields[27] = mllPairEntry.secondMllNode.tunnelStartPassengerType;
    inFields[28] = mllPairEntry.secondMllNode.ttlHopLimitThreshold;
    inFields[29] = mllPairEntry.secondMllNode.excludeSrcVlan;
    inFields[30] = mllPairEntry.secondMllNode.last;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28], inFields[29], inFields[30]);

    galtisOutput(outArgs, result, "%f");

    gMllPairEntryIndex++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpMLLPairRead
*
* DESCRIPTION:
*    Read a Mc Link List (MLL) pair entry from the hw.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index
*       mllPairReadForm   - the way to read the Mll pair, first part only/
*                           second + next pointer only/ whole Mll pair
*
*
* OUTPUTS:
*       mllPairEntryPtr   - the Mc lisk list pair entry
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid parameter.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLPairReadFormNext_2
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS result;

    GT_U8                                           devNum;
    GT_U32                                          rangeStart;
    GT_U32                                          rangeEnd;
    CPSS_DXCH_IP_MLL_PAIR_STC                       mllPairEntry;
    CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT              mllPairReadForm;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rangeStart = (GT_U32)inArgs[1];
    rangeEnd = (GT_U32)inArgs[2];

    /*if the user doesn't enter a range show the whole range*/
    if(rangeStart == 0 && rangeEnd == 0)
    {
        rangeEnd = MAX_MLL_ENTRIES - 1;
    }

    if(gMllPairEntryIndex > rangeEnd)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(gMllPairEntryIndex == rangeEnd)
    {
        mllPairReadForm = (CPSS_DXCH_PAIR_READ_WRITE_FORM_ENT)inArgs[3];
    }
    else
    {
        mllPairReadForm = CPSS_DXCH_PAIR_READ_WRITE_WHOLE_E;
    }

    /* call cpss api function */
    cmdOsMemSet(&mllPairEntry, 0, sizeof(mllPairEntry));

    /* call cpss api function */
    result = cpssDxChIpMLLPairRead(devNum, gMllPairEntryIndex,
                                   mllPairReadForm, &mllPairEntry);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gMllPairEntryIndex;
    inFields[2] = mllPairEntry.nextPointer;
    inFields[3] = mllPairEntry.firstMllNode.mllRPFFailCommand;
    inFields[4] = mllPairEntry.firstMllNode.isTunnelStart;
    inFields[5] = mllPairEntry.firstMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum);
    inFields[6] = mllPairEntry.firstMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[7] = mllPairEntry.firstMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.firstMllNode.nextHopInterface.trunkId);
    inFields[8] = mllPairEntry.firstMllNode.nextHopInterface.trunkId;
    inFields[9] = mllPairEntry.firstMllNode.nextHopInterface.vidx;
    inFields[10] = mllPairEntry.firstMllNode.nextHopInterface.vlanId;
    inFields[11] = mllPairEntry.firstMllNode.nextHopVlanId;
    inFields[12] = mllPairEntry.firstMllNode.nextHopTunnelPointer;
    inFields[13] = mllPairEntry.firstMllNode.tunnelStartPassengerType;
    inFields[14] = mllPairEntry.firstMllNode.ttlHopLimitThreshold;
    inFields[15] = mllPairEntry.firstMllNode.excludeSrcVlan;
    inFields[16] = mllPairEntry.firstMllNode.last;
    inFields[17] = mllPairEntry.secondMllNode.mllRPFFailCommand;
    inFields[18] = mllPairEntry.secondMllNode.isTunnelStart;
    inFields[19] = mllPairEntry.secondMllNode.nextHopInterface.type;

    CONVERT_BACK_DEV_PORT_DATA_MAC(mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum,
        mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum);
    inFields[20] = mllPairEntry.secondMllNode.nextHopInterface.devPort.hwDevNum;
    inFields[21] = mllPairEntry.secondMllNode.nextHopInterface.devPort.portNum;

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(mllPairEntry.secondMllNode.nextHopInterface.trunkId);
    inFields[22] = mllPairEntry.secondMllNode.nextHopInterface.trunkId;
    inFields[23] = mllPairEntry.secondMllNode.nextHopInterface.vidx;
    inFields[24] = mllPairEntry.secondMllNode.nextHopInterface.vlanId;
    inFields[25] = mllPairEntry.secondMllNode.nextHopVlanId;
    inFields[26] = mllPairEntry.secondMllNode.nextHopTunnelPointer;
    inFields[27] = mllPairEntry.secondMllNode.tunnelStartPassengerType;
    inFields[28] = mllPairEntry.secondMllNode.ttlHopLimitThreshold;
    inFields[29] = mllPairEntry.secondMllNode.excludeSrcVlan;
    inFields[30] = mllPairEntry.secondMllNode.last;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
        ,inFields[0], inFields[1], inFields[2], inFields[3],
        inFields[4], inFields[5], inFields[6], inFields[7],
        inFields[8], inFields[9], inFields[10], inFields[11],
        inFields[12], inFields[13], inFields[14], inFields[15],
        inFields[16], inFields[17], inFields[18], inFields[19],
        inFields[20], inFields[21], inFields[22], inFields[23],
        inFields[24], inFields[25], inFields[26], inFields[27],
        inFields[28], inFields[29], inFields[30]);

    galtisOutput(outArgs, result, "%f");

    gMllPairEntryIndex++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMLLLastBitWrite
*
* DESCRIPTION:
*    Write a Mc Link List (MLL) Last bit to the hw.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum            - the device number
*       mllPairEntryIndex - the mll Pair entry index.
*       lastBit           - the Last bit
*       mllEntryPart      - to which part of the mll Pair to write the Last bit
*                           for.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active or invalid mllEntryPart.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMLLLastBitWrite
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                                            devNum;
    GT_U32                                           mllPairEntryIndex;
    GT_BOOL                                          lastBit;
    CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT    mllEntryPart;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    mllPairEntryIndex = (GT_U32)inArgs[1];
    lastBit = (GT_BOOL)inArgs[2];
    mllEntryPart = (CPSS_DXCH_IP_MLL_PAIR_PART_LAST_BIT_WRITE_ENT)inArgs[3];

    /* call cpss api function */
    status = cpssDxChIpMLLLastBitWrite(devNum, mllPairEntryIndex, lastBit,
        mllEntryPart);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterArpAddrWrite
*
* DESCRIPTION:
*    write a ARP MAC address to the router ARP / Tunnel start Table.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later
*                         in the UC Route entry Arp nextHopARPPointer
*                         field).
*       arpMacAddrPtr   - the ARP MAC address to write
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerArpIndex bigger then allowed.
*       GT_BAD_PTR      - one of the parameters is NULL pointer.
*
* COMMENTS:
*       pay attention that the router ARP table is shard with a tunnel start
*       table, each tunnel start entry takes 4 ARP Address.
*       that is Tunnel start <n> takes ARP addresses <4n>,<4n+1>,<4n+2>,
*       <4n+3>
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterArpAddrWrite
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8               dev;
    GT_U32              routerArpIndex;
    GT_ETHERADDR        arpMacAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    routerArpIndex = (GT_U32)inArgs[1];
    galtisMacAddr(&arpMacAddr, (GT_U8*)inArgs[2]);

    /* call cpss api function */
    status =  cpssDxChIpRouterArpAddrWrite(dev,routerArpIndex, &arpMacAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterArpAddrRead
*
* DESCRIPTION:
*    read a ARP MAC address from the router ARP / Tunnel start Table.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - the device number
*       routerArpIndex  - The Arp Address index (to be inserted later in the
*                         UC Route entry Arp nextHopARPPointer field).
*
*
* OUTPUTS:
*       arpMacAddrPtr   - the ARP MAC address
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_OUT_OF_RANGE - on routerArpIndex bigger then allowed or
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       see cpssDxChIpRouterArpAddrWrite
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterArpAddrRead
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8               dev;
    GT_U32              routerArpIndex;
    GT_ETHERADDR        arpMacAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    routerArpIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    status =  cpssDxChIpRouterArpAddrRead(dev,routerArpIndex, &arpMacAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%6b", arpMacAddr.arEther);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpTcamInvalidate
*
* DESCRIPTION:
*    Invalidate all entries in Router Tcam.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - the device number
*
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on devNum not active.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpTcamInvalidate
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS           status = GT_OK;

    GT_U8               dev;
    GT_U32              routerRowIndex, routerColIndex;
    GT_U32              numOfTcamRows;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev = (GT_U8)inArgs[0];
    numOfTcamRows = PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_GET_MAX_SIZE_MAC(dev);

    routerColIndex = 0;
    while (GT_OK == status)
    {
        for (routerRowIndex = 0; routerRowIndex < numOfTcamRows; routerRowIndex++)
        {
            status = cpssDxChIpv4PrefixInvalidate(dev, routerRowIndex, routerColIndex);
            if (GT_OK != status)
            {
                break;
            }
        }

        routerColIndex++;
    }

    if (GT_BAD_PARAM == status)
    {
        status = GT_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpHashNumBitsSet
*
* DESCRIPTION:
*       Set the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       startBit        - the index of the first bit that is needed by the L3
*                         ECMP hash mechanism (APPLICABLE RANGES: 0..31)
*       numOfBits       - the number of bits that is needed by the L3 ECMP hash
*                         mechanism (APPLICABLE RANGES: 1..16)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*       startBit + numOfBits must not exceed 32.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpHashNumBitsSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U32                      startBit;
    GT_U32                      numOfBits;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    startBit = (GT_U32)inArgs[1];
    numOfBits = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpEcmpHashNumBitsSet(devNum, startBit, numOfBits);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpHashNumBitsGet
*
* DESCRIPTION:
*       Get the start bit and the number of bits needed by the L3 ECMP hash
*       mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       startBitPtr     - (pointer to) the index of the first bit that is needed
*                         by the L3 ECMP hash mechanism
*       numOfBitsPtr    - (pointer to) the number of bits that is needed by the
*                         L3 ECMP hash mechanism
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       startBit is relevant only when randomEnable field in the entry is set to
*       GT_FALSE.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpHashNumBitsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U32                      startBit;
    GT_U32                      numOfBits;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpEcmpHashNumBitsGet(devNum, &startBit, &numOfBits);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", startBit, numOfBits);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpHashSeedValueSet
*
* DESCRIPTION:
*       Set the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*       seed            - the "seed value" parameter used by the L3 ECMP hash
*                         (APPLICABLE RANGES: 1..0xffffffff)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpHashSeedValueSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U32                      seed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    seed = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpEcmpHashSeedValueSet(devNum, seed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpHashSeedValueGet
*
* DESCRIPTION:
*       Get the "seed value" parameter used by the L3 ECMP hash mechanism
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; xCat2; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2.
*
* INPUTS:
*       devNum          - the device number
*
* OUTPUTS:
*       seedPtr         - (pointer to) the "seed value" parameter used by the L3
*                         ECMP hash
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       The "seed value" parameter is relevant only for ECMP entries that are
*       configured with randomEnable == GT_TRUE.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpHashSeedValueGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    GT_U32                      seed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpEcmpHashSeedValueGet(devNum, &seed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", seed);
    return CMD_OK;
}

/* table cpssDxChIpNat44 global variables */
static  CPSS_DXCH_IP_NAT_ENTRY_UNT      natEntry;
/*******************************************************************************
* cpssDxChIpNatEntrySet
*
* DESCRIPTION:
*       Set a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*       natType       - type of the NAT
*       entryPtr      - (pointer to) NAT entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table Tunnel start entries table and router ARP addresses 
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
* 
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpNat44EntrySet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS   status;

    GT_U8       dev;
    GT_U32      nat44Index;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&natEntry, 0, sizeof(CPSS_DXCH_IP_NAT_ENTRY_UNT)); 

    dev = (GT_U8)inArgs[0];

    nat44Index = (GT_U32)inFields[0];
    galtisMacAddr(&natEntry.nat44Entry.macDa, (GT_U8*)inFields[1]);
    
    natEntry.nat44Entry.modifyDip = (GT_BOOL)inFields[2];
    galtisIpAddr(&natEntry.nat44Entry.newDip, (GT_U8*)inFields[3]);    
    natEntry.nat44Entry.modifySip = (GT_BOOL)inFields[4];    
    galtisIpAddr(&natEntry.nat44Entry.newSip, (GT_U8*)inFields[5]);    
    natEntry.nat44Entry.modifyTcpUdpDstPort = (GT_BOOL)inFields[6];
    natEntry.nat44Entry.newTcpUdpDstPort = (GT_U32)inFields[7];       
    natEntry.nat44Entry.modifyTcpUdpSrcPort = (GT_BOOL)inFields[8];
    natEntry.nat44Entry.newTcpUdpSrcPort = (GT_U32)inFields[9];       
  
    /* call cpss api function */
    status =  cpssDxChIpNatEntrySet(dev,nat44Index,CPSS_IP_NAT_TYPE_NAT44_E,&natEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpNatEntryGet
*
* DESCRIPTION:
*       Get a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*       natType       - type of the NAT
*
* OUTPUTS:
*       entryPtr     - (pointer to) NAT entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table Tunnel start entries table and router ARP addresses 
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
* 
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpNat44EntryGet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS               result=GT_OK;

    GT_U8                   dev;
    GT_U32                  nat44GetIndex;
    CPSS_IP_NAT_TYPE_ENT    natTypeGet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev                  = (GT_U8)inArgs[0];
    nat44GetIndex        = (GT_U32)inArgs[1];
    
    cpssOsMemSet(&natEntry.nat44Entry, 0, sizeof(CPSS_DXCH_IP_NAT44_ENTRY_STC)); 

    /* call cpss api function */
    result =  cpssDxChIpNatEntryGet(dev,nat44GetIndex,&natTypeGet, &natEntry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b%d%4b%d%4b%d%d%d%d", 
                 natEntry.nat44Entry.macDa.arEther, 
                 natEntry.nat44Entry.modifyDip,
                 natEntry.nat44Entry.newDip.arIP,  
                 natEntry.nat44Entry.modifySip,
                 natEntry.nat44Entry.newSip.arIP,  
                 natEntry.nat44Entry.modifyTcpUdpDstPort,
                 natEntry.nat44Entry.newTcpUdpDstPort,  
                 natEntry.nat44Entry.modifyTcpUdpSrcPort,
                 natEntry.nat44Entry.newTcpUdpSrcPort);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpNatEntrySet
*
* DESCRIPTION:
*       Set a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*       natType       - type of the NAT
*       entryPtr      - (pointer to) NAT entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table Tunnel start entries table and router ARP addresses 
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
* 
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpNat66EntrySet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS   status;

    GT_U8       dev;
    GT_U32      nat66Index;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&natEntry, 0, sizeof(CPSS_DXCH_IP_NAT_ENTRY_UNT)); 

    dev = (GT_U8)inArgs[0];

    nat66Index = (GT_U32)inFields[0];
    galtisMacAddr(&natEntry.nat66Entry.macDa, (GT_U8*)inFields[1]);    
    natEntry.nat66Entry.modifyCommand = (CPSS_DXCH_IP_NAT66_MODIFY_COMMAND_ENT)inFields[2];
    galtisIpv6Addr(&natEntry.nat66Entry.address, (GT_U8*)inFields[3]);    
    natEntry.nat66Entry.prefixSize = (GT_U32)inFields[4];    
    
    if ((natEntry.nat66Entry.modifyCommand!=CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_DIP_PREFIX_E)&&
        (natEntry.nat66Entry.modifyCommand!=CPSS_DXCH_IP_NAT_TYPE_NAT66_MODIFY_SIP_PREFIX_E))
    {
        /* set the default value */
        natEntry.nat66Entry.prefixSize=64;
    }
    /* call cpss api function */
    status =  cpssDxChIpNatEntrySet(dev,nat66Index,CPSS_IP_NAT_TYPE_NAT66_E,&natEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpNatEntryGet
*
* DESCRIPTION:
*       Get a NAT entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       natIndex      - index for the NAT entry
*                       in the router ARP / tunnel start / NAT table
*       natType       - type of the NAT
*
* OUTPUTS:
*       entryPtr     - (pointer to) NAT entry
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_FAIL                  - on failure.
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_BAD_STATE             - on invalid tunnel type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       NAT entries table Tunnel start entries table and router ARP addresses 
*       table reside at the same physical memory.
*       Each line can hold:
*       - 1 NAT entry
*       - 1 tunnel start entry
*       - 4 router ARP addresses entries
*       NAT entry / Tunnel start entry at index n and router ARP addresses at indexes
*       4*n..4*n+3 share the same memory. For example NAT entry/tunnel start entry at
*       index 100 and router ARP addresses at indexes 400..403 share the same
*       physical memory.
* 
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpNat66EntryGet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{
    GT_STATUS               result=GT_OK;

    GT_U8                   dev;
    GT_U32                  nat66GetIndex;
    CPSS_IP_NAT_TYPE_ENT    natTypeGet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    dev                  = (GT_U8)inArgs[0];
    nat66GetIndex        = (GT_U32)inArgs[1];
    
    cpssOsMemSet(&natEntry.nat66Entry, 0, sizeof(CPSS_DXCH_IP_NAT66_ENTRY_STC)); 

    /* call cpss api function */
    result =  cpssDxChIpNatEntryGet(dev,nat66GetIndex,&natTypeGet, &natEntry);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    } 
    /* pack output arguments to galtis string */
     galtisOutput(outArgs, result,"%6b%d%16b%d",
                natEntry.nat66Entry.macDa.arEther, 
                natEntry.nat66Entry.modifyCommand,
                natEntry.nat66Entry.address.arIP,  
                natEntry.nat66Entry.prefixSize);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpNatDroppedPacketsCntGet
*
* DESCRIPTION:
*      Get the counter that counts the packets that were dropped since NAT could not be applied.
*      If the L4 header is not within the header 64 bytes (For tunnel-terminated packet it must
*      be within the passenger header 64 bytes), the packet is dropped and it is counted by
*      the NAT Drop Counter.
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
*       natDropPktsPtr - (pointer to) the number of dropped packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Clear on read.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpNatDroppedPacketsCntGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U32          natDropPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    
    /* call cpss api function */
    result = cpssDxChIpNatDroppedPacketsCntGet(devNum, &natDropPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", natDropPkts);
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssDxChIpLttSet",
        &wrCpssDxChIpLttWrite,
        1, 7},

    {"cpssDxChIpLttGetFirst",
    &wrCpssDxChIpLttReadFirst,
    1, 0},

    {"cpssDxChIpLttGetNext",
    &wrCpssDxChIpLttReadNext,
    1, 0},

   {"cpssDxChIpLttExtSet",
     &wrCpssDxChIpLttExtWrite,
     1, 8},

    {"cpssDxChIpLttExtGetFirst",
    &wrCpssDxChIpLttExtReadFirst,
    5, 0},

    {"cpssDxChIpLttExtGetNext",
    &wrCpssDxChIpLttExtReadNext,
    5, 0},

    {"cpssDxChIpv4PrefixSet",
    &wrCpssDxChIpv4PrefixSet,
    1, 4},

    {"cpssDxChIpv4PrefixGetFirst",
    &wrCpssDxChIpv4PrefixGetFirst,
    1, 0},

    {"cpssDxChIpv4PrefixGetNext",
    &wrCpssDxChIpv4PrefixGetNext,
    1, 0},

    {"cpssDxChIpv4PrefixDelete",
    &wrCpssDxChIpv4PrefixInvalidate,
    1, 1},


    {"cpssDxChIpv4PrefixExtSet",
    &wrCpssDxChIpv4PrefixExtSet,
    1, 12},

    {"cpssDxChIpv4PrefixExtGetFirst",
    &wrCpssDxChIpv4PrefixExtGetFirst,
    5, 0},

    {"cpssDxChIpv4PrefixExtGetNext",
    &wrCpssDxChIpv4PrefixExtGetNext,
    5, 0},

    {"cpssDxChIpv4PrefixExtDelete",
    &wrCpssDxChIpv4PrefixExtInvalidate,
    1, 1},

    {"cpssDxChIpv6PrefixSet",
    &wrCpssDxChIpv6PrefixSet,
    1, 9},

    {"cpssDxChIpv6PrefixGetFirst",
    &wrCpssDxChIpv6PrefixGetFirst,
    1, 0},

    {"cpssDxChIpv6PrefixGetNext",
    &wrCpssDxChIpv6PrefixGetNext,
    1, 0},

    {"cpssDxChIpv6PrefixDelete",
    &wrCpssDxChIpv6PrefixInvalidate,
    1, 1},

    {"cpssDxChIpv6PrefixExtSet",
    &wrCpssDxChIpv6PrefixExtSet,
    1, 6},

    {"cpssDxChIpv6PrefixExtGetFirst",
    &wrCpssDxChIpv6PrefixExtGetFirst,
    2, 0},

    {"cpssDxChIpv6PrefixExtGetNext",
    &wrCpssDxChIpv6PrefixExtGetNext,
    2, 0},

    {"cpssDxChIpv6PrefixExtDelete",
    &wrCpssDxChIpv6PrefixInvalidate,
    1, 1},


    {"cpssDxChIpUcRouteExtSetFirst",
    &wrCpssDxChIpUcRouteEntriesWriteSetFirst,
    0, 30},

    {"cpssDxChIpUcRouteExtSetNext",
    &wrCpssDxChIpUcRouteEntriesWriteSetNext,
    0, 30},

    {"cpssDxChIpUcRouteExtEndSet",
    &wrCpssDxChIpUcRouteEntriesWriteEndSet,
    2, 0},

    {"cpssDxChIpUcRouteExtGetFirst",
    &wrCpssDxChIpUcRouteEntriesReadGetFirst,
    3, 0},

    {"cpssDxChIpUcRouteExtGetNext",
    &wrCpssDxChIpUcRouteEntriesReadGetNext,
    0, 0},



    {"cpssDxChIpUcRouteEntriesSetFirst",
    &wrCpssDxChIpUcRouteEntriesSetFirst,
    5, 30},

    {"cpssDxChIpUcRouteEntriesSetNext",
    &wrCpssDxChIpUcRouteEntriesSetNext,
    5, 30},

    {"cpssDxChIpUcRouteEntriesEndSet",
    &wrCpssDxChIpUcRouteEntriesEndSet,
    2, 0},

    {"cpssDxChIpUcRouteEntriesGetFirst",
    &wrCpssDxChIpUcRouteEntriesGetFirst,
    5, 0},

    {"cpssDxChIpUcRouteEntriesGetNext",
    &wrCpssDxChIpUcRouteEntriesGetNext,
    5, 0},


    {"cpssDxChIpMcRouteSet",
    &wrCpssDxChIpMcRouteEntriesWrite,
    1, 23},

    {"cpssDxChIpMcRouteGetFirst",
    &wrCpssDxChIpMcRouteEntriesReadFirst,
    1, 0},

    {"cpssDxChIpMcRouteGetNext",
    &wrCpssDxChIpMcRouteEntriesReadNext,
    1, 0},

    {"cpssDxChIpMcRoute_1Set",
    &wrCpssDxChIpMcRouteEntriesWrite,
    3, 23},

    {"cpssDxChIpMcRoute_1GetFirst",
    &wrCpssDxChIpMcRouteEntries_1ReadFirst,
    3, 0},

    {"cpssDxChIpMcRoute_1GetNext",
    &wrCpssDxChIpMcRouteEntries_1ReadNext,
    3, 0},

    {"cpssDxChIpRouterNextHopTableAgeBitsSetFirst",
    &wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteFirst,
    3, 2},

    {"cpssDxChIpRouterNextHopTableAgeBitsSetNext",
    &wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteNext,
    3, 2},

    {"cpssDxChIpRouterNextHopTableAgeBitsEndSet",
    &wrCpssDxChIpRouterNextHopTableAgeBitsEntryWriteEnd,
    3, 0},

    {"cpssDxChIpRouterNextHopTableAgeBitsGetFirst",
    &wrCpssDxChIpRouterNextHopTableAgeBitsEntryReadFirst,
    3, 0},

    {"cpssDxChIpRouterNextHopTableAgeBitsGetNext",
    &wrCpssDxChIpRouterNextHopTableAgeBitsEntryReadNext,
    3, 0},

    {"cpssDxChIpMLLPairSet",
    &wrCpssDxChIpMLLPairWrite,
    3, 29},

    {"cpssDxChIpMLLPairGetFirst",
    &wrCpssDxChIpMLLPairReadFirst,
    3, 0},

    {"cpssDxChIpMLLPairGetNext",
    &wrCpssDxChIpMLLPairReadNext,
    3, 0},

    {"cpssDxChIpMLLPair_1Set",
    &wrCpssDxChIpMLLPairWrite,
    4, 29},

    {"cpssDxChIpMLLPair_1GetFirst",
    &wrCpssDxChIpMLLPairReadFormFirst,
    4, 0},

    {"cpssDxChIpMLLPair_1GetNext",
    &wrCpssDxChIpMLLPairReadFormNext,
    4, 0},

    {"cpssDxChIpMLLPair_2Set",
    &wrCpssDxChIpMLLPairWrite_2,
    4, 31},

    {"cpssDxChIpMLLPair_2GetFirst",
    &wrCpssDxChIpMLLPairReadFormFirst_2,
    4, 0},

    {"cpssDxChIpMLLPair_2GetNext",
    &wrCpssDxChIpMLLPairReadFormNext_2,
    4, 0},

    {"cpssDxChIpMLLLastBitWrite",
    &wrCpssDxChIpMLLLastBitWrite,
    4, 0},

    {"cpssDxChIpRouterArpAddrWrite",
    &wrCpssDxChIpRouterArpAddrWrite,
    3, 0},

    {"cpssDxChIpRouterArpAddrRead",
    &wrCpssDxChIpRouterArpAddrRead,
    2, 0},

    {"cpssDxChIpTcamInvalidate",
    &wrCpssDxChIpTcamInvalidate,
    1, 0},

    /* Bobcat 2, Lion 3 */

    {"cpssDxChIpUcRouteEntries_1SetFirst",
    &wrCpssDxChIpUcRouteEntriesSetFirst_1,
    5, 32},

    {"cpssDxChIpUcRouteEntries_1SetNext",
    &wrCpssDxChIpUcRouteEntriesSetNext_1,
    5, 32},

    {"cpssDxChIpUcRouteEntries_1EndSet",
    &wrCpssDxChIpUcRouteEntriesEndSet,
    2, 0},

    {"cpssDxChIpUcRouteEntries_1GetFirst",
    &wrCpssDxChIpUcRouteEntriesGetFirst,
    5, 0},

    {"cpssDxChIpUcRouteEntries_1GetNext",
    &wrCpssDxChIpUcRouteEntriesGetNext_1,
    5, 0},

    {"cpssDxChIpUcRouteEntries_2SetFirst",
    &wrCpssDxChIpUcRouteEntriesSetFirst_2,
    5, 34},

    {"cpssDxChIpUcRouteEntries_2SetNext",
    &wrCpssDxChIpUcRouteEntriesSetNext_2,
    5, 34},

    {"cpssDxChIpUcRouteEntries_2EndSet",
    &wrCpssDxChIpUcRouteEntriesEndSet,
    2, 0},

    {"cpssDxChIpUcRouteEntries_2GetFirst",
    &wrCpssDxChIpUcRouteEntriesGetFirst_2,
    5, 0},

    {"cpssDxChIpUcRouteEntries_2GetNext",
    &wrCpssDxChIpUcRouteEntriesGetNext_2,
    5, 0},

    {"cpssDxChIpMcRoute_2Set",
    &wrCpssDxChIpMcRouteEntriesWrite_2,
    3, 25},

    {"cpssDxChIpMcRoute_2GetFirst",
    &wrCpssDxChIpMcRouteEntriesReadFirst_2,
    3, 0},

    {"cpssDxChIpMcRoute_2GetNext",
    &wrCpssDxChIpMcRouteEntriesReadNext_2,
    3, 0},

    {"cpssDxChIpEcmpEntrySet",
    &wrCpssDxChIpEcmpEntrySet,
    1, 4},

    {"cpssDxChIpEcmpEntryGetFirst",
    &wrCpssDxChIpEcmpEntryGetFirst,
    3, 0},

    {"cpssDxChIpEcmpEntryGetNext",
    &wrCpssDxChIpEcmpEntryGetNext,
    1, 0},

    {"cpssDxChIpEcmpHashNumBitsSet",
    &wrCpssDxChIpEcmpHashNumBitsSet,
    3, 0},

    {"cpssDxChIpEcmpHashNumBitsGet",
    &wrCpssDxChIpEcmpHashNumBitsGet,
    1, 0},

    {"cpssDxChIpEcmpHashSeedValueSet",
    &wrCpssDxChIpEcmpHashSeedValueSet,
    2, 0},

    {"cpssDxChIpEcmpHashSeedValueGet",
    &wrCpssDxChIpEcmpHashSeedValueGet,
    1, 0},

    {"cpssDxChIpNat44EntrySet",
    &wrCpssDxChIpNat44EntrySet,
    1, 10},       

    {"cpssDxChIpNat44EntryGet",
    &wrCpssDxChIpNat44EntryGet,
    2, 0},

    {"cpssDxChIpNat66EntrySet",
    &wrCpssDxChIpNat66EntrySet,
    1, 4},       

    {"cpssDxChIpNat66EntryGet",
    &wrCpssDxChIpNat66EntryGet,
    2, 0},    

    {"cpssDxChIpNatDroppedPacketsCntGet",
    &wrCpssDxChIpNatDroppedPacketsCntGet,
    1, 0}

    /* end of Bobcat 2, Lion 3 */
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChIp
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
GT_STATUS cmdLibInitCpssDxChIp
(
 GT_VOID
 )
{
    return cmdInitLibrary(dbCommands, numCommands);
}
