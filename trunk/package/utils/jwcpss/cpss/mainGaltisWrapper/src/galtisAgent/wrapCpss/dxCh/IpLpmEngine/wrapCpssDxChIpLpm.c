/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChIpLpm.c
*
* DESCRIPTION:
*       Wrapper functions for Ip Lpm cpss functions for dx ch.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 63 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/cpssDxChIpLpm.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamCommonTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChPrvIpLpmTypes.h>
#include <cpss/dxCh/dxChxGen/ipLpmEngine/private/cpssDxChIpLpmDbg.h>
#include <cpssCommon/private/prvCpssSkipList.h>

#define DXCH_MAX_VR_ID_CNS                 1024
#define DXCH2_MAX_VR_ID_CNS                1
#define DXCH3_MAX_VR_ID_CNS                4096
#define DXCH_XCAT_MAX_VR_ID_CNS            4096
#define DXCH_LION3_MAX_VR_ID_CNS           4096

static GT_U32       maxNumOfVr;
static GT_U32       vrId=0;
static GT_BOOL      isDefaultVrfIdEntry = GT_FALSE;
static GT_U32       lastVrfIdTreated;

static GT_UINTPTR                               lpmDbIterator;
static GT_U32                                   lpmDBId;
static PRV_CPSS_DXCH_LPM_SHADOW_STC             *lpmDbPtr;
static GT_UINTPTR                               vrIterator;
static GT_U32                                   vrIdValue;
static PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC  *vrEntryPtr;
static PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC  vrEntryRam;
static CPSS_IP_PROTOCOL_STACK_ENT               protocolStack;
static CPSS_DXCH_IP_LPM_VR_CONFIG_STC           getVrConfigInfo;
static CPSS_DXCH_IP_LPM_VR_CONFIG_STC           vrConfigInfo_2;
static GT_BOOL                                  ipv4_set = GT_FALSE;    /* is ipv4 set   */
static GT_BOOL                                  ipv6_set = GT_FALSE;    /* is ipv6 set   */
static GT_U32                                   bulkIndex;
static GT_BOOL                                  bulkDefragmentationEnable;
static GT_U32                                   devIndex;


extern GT_VOID      *prvCpssDxChIplpmDbSL;

extern GT_STATUS appDemoBc2IpLpmRamDefaultConfigCalc_elements
(
    IN  GT_U8       devNum,
    IN  GT_U32      maxNumOfPbrEntries,
    OUT GT_U32      *numOfBlocksPtr,
    OUT GT_U32      blocksSizeArray[],
    OUT GT_U32      *blocksAllocationMethodPtr    
);

#define SWITCH_BYTES(_byte_arr, _len)                       \
        do{                                                 \
            GT_U8 _i, _temp;                                \
            for(_i=0; _i<(_len)/2; _i++)                    \
            {                                               \
                _temp = _byte_arr[_i];                      \
                _byte_arr[_i] = _byte_arr[(_len) - _i - 1]; \
                _byte_arr[(_len) - _i - 1] = _temp;         \
            }                                               \
        }while(0)


/*
 * typedef: enum WRAP_BULK_ENTRY_STATE_ENT (type)
 *
 * Description: the role of the entry in wrapper for bulk functions purposes.
 *              indicates the role of an entry in order to determain if to
 *              gather it in a bulk or not and to determain if start or end of
 *              bulk.
 *
 * Enumerations:
        REGULAR_ENTRY       - Does not impose any special bulk operation
 *      BULK_STARTING_ENTRY - indicates that this entry starts a bulk of entries
 *      BULK_ENDING_ENTRY   - indicates that this entry ends a bulk of entries
 */
typedef enum
{
    REGULAR_ENTRY = 0,
    BULK_STARTING_ENTRY = 1,
    BULK_ENDING_ENTRY

}WRAP_BULK_ENTRY_ROLE_ENT;

/*
 * typedef: enum WRAP_BULK_FUNCTION_ENT (type)
 *
 * Description: Whether to use bulk add or delete API
 *
 * Enumerations:
 *      BULK_ADD    - use bulk add API
 *      BULK_DELETE - use bulk delete API
 */
typedef enum
{
    BULK_ADD = 0,
    BULK_DELETE
}WRAP_BULK_FUNCTION_ENT;

static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
);

/*******************************************************************************
* cpssDxChIpLpmShadowTypeGet
*
* DESCRIPTION:
*   Private Helping function for the retrieval of shadow Type.
*
*
* INPUTS:
*       lpmDBId               - the LPM DB id.
*
* OUTPUTS:
*       shadowType            - the shadow type
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND   - if can't find the lpm DB
*       GT_BAD_PTR     - if shadowType is uninitialized
*
*******************************************************************************/
static GT_STATUS cpssDxChIpLpmShadowTypeGet
(
    IN    GT_U32                                lpmDBId,
    OUT   CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT      *shadowType
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC *lpmDbPtr,tmpLpmDb;

    if (shadowType == NULL)
        return GT_BAD_PTR;

    tmpLpmDb.lpmDbId = lpmDBId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL, &tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    *shadowType = lpmDbPtr->shadowType;

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChIpLpmConvertRouteTypeForLegacyTests
*
* DESCRIPTION:
*   Convert CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E to
*   CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E for RAM based LPM whenever input
*   type is CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E and numOfPaths is 0
*   This is needed for backward compatibility of legacy tests.
*
*
* INPUTS:
*       lpmDBId               - LPM DB id
*       setWrapper            - whether the function is used for a set or get
*                               wrapper:
*                               GT_TRUE - used for a set wrapper
*                               GT_FALSE - used for a get wrapper
*       ipLttEntryPtr         - (pointer to) the original IP LTT entry
*
* OUTPUTS:
*       ipLttEntryPtr         - (pointer to) the changed IP LTT entry
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND   - if can't find the lpm DB
*       GT_BAD_PTR     - on null pointer or if shadowType is uninitialized
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpLpmConvertRouteTypeForLegacyTests
(
    IN    GT_U32                          lpmDBId,
    IN    GT_BOOL                         setWrapper,
    INOUT CPSS_DXCH_IP_LTT_ENTRY_STC      *ipLttEntryPtr
)
{
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT shadowType;
    GT_STATUS                        status;

    if (ipLttEntryPtr == NULL)
    {
        return GT_BAD_PTR;
    }

    status = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);
    if (status != GT_OK)
    {
        return status;
    }

    if (setWrapper) /* set wrapper */
    {
        if ((shadowType == CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E) &&
            (ipLttEntryPtr->routeType == CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E) &&
            (ipLttEntryPtr->numOfPaths == 0))
        {
            ipLttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E;
        }
    }
    else            /* get wrapper */
    {
        if ((shadowType == CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E) &&
            (ipLttEntryPtr->routeType == CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_REGULAR_E))
        {
            ipLttEntryPtr->routeType = CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ECMP_E;
            ipLttEntryPtr->numOfPaths = 0;
        }
    }

    return GT_OK;
}


static GT_BOOL gIsLpmDbCreateWithVrSupport = GT_FALSE;

/*******************************************************************************
* cpssDxChIpLpmDBCreate
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId               - the LPM DB id.
*       shadowType            - the type of shadow to maintain (cheetah/cheetha2)
*       protocolStack         - the type of protocol stack this LPM DB support.
*       indexesRangePtr       - the range of TCAM indexes availble for this
*                               LPM DB (see explanation in
*                               CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnable       - weather to enable partitioning of the router
*                               TCAM according to the
*                               tcamLpmManagerCapcityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - a structure which hold the capacity
*                               configuration required from this TCAM LPM
*                               manager, relevant only if
*                               partitionEnable = GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_ALREADY_EXIST - if the LPM DB id is already used.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       if partitionEnable = GT_FALSE then the TCAM is one big shared resource
*       for all kinds of prefixes. In a shared resource situation a best effort
*       is applied to put the inserted prefixes in a way they wouldn't interfere
*       with other prefixes. But there could be a situation where due to
*       fragmentation a prefix couldn't be inserted, in this situation all
*       prefix insert function has a defragmationEnable to enable a performance
*       costing de-fragmentation process in order to fit the inserted prefix.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBCreate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                       lpmDBId;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT             shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT           lpmMemoryConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    shadowType = (CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    if (shadowType != CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E)
    {
        indexesRange.firstIndex = (GT_U32)inArgs[3];
        indexesRange.lastIndex = (GT_U32)inArgs[4];
        lpmMemoryConfig.tcamDbCfg.partitionEnable = (GT_BOOL)inArgs[5];
        tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = (GT_U32)inArgs[6];
        tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = (GT_U32)inArgs[7];
        tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = (GT_U32)inArgs[8];
        lpmMemoryConfig.tcamDbCfg.indexesRangePtr = &indexesRange;
        lpmMemoryConfig.tcamDbCfg.tcamLpmManagerCapcityCfgPtr = &tcamLpmManagerCapcityCfg;
        lpmMemoryConfig.tcamDbCfg.tcamManagerHandlerPtr = NULL;
    }

    if (gIsLpmDbCreateWithVrSupport == GT_FALSE)
    {
        /* call cpss api function */
        status = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack,
                                       &lpmMemoryConfig);
    }
    else
    {
        CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC pclTcamCfg = {{300,300},{100,600}};
        CPSS_DXCH_IP_TCAM_LPM_MANGER_CHEETAH2_VR_SUPPORT_CFG_STC routerTcamCfg = {{300,500},{301,900}};


        status = cpssDxChIpLpmDBCheetah2VrSupportCreate(lpmDBId,protocolStack,
                                                        &pclTcamCfg, &routerTcamCfg);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* prvCpssDxChIpLpmRamDefaultConfigCalc
*
* DESCRIPTION:
*   This function calculates the default RAM LPM DB configuration for a shared LPM managment.
*
* INPUTS:
*       devNum           - The Pp device number to get the parameters for.
*
* OUTPUTS:
*       ramDbCfgPtr      - (pointer to) ramDbCfg structure to hold the defaults calculated
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on failure
*       GT_BAD_PTR       - Null pointer
*       GT_BAD_VALUE     - on bad value
* 
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChIpLpmRamDefaultConfigCalc
(
    IN  GT_U8                        devNum,
    OUT CPSS_DXCH_LPM_RAM_CONFIG_STC *ramDbCfgPtr 
)
{

    GT_STATUS rc = GT_OK;
    GT_U32    i=0;
    GT_U32    numOfBlocks=0;
    GT_U32    blocksSizeArray[PRV_CPSS_DXCH_LPM_RAM_MAX_NUM_OF_MEMORY_BLOCKS_CNS];
    GT_U32    blocksAllocationMethod=0;

    CPSS_NULL_PTR_CHECK_MAC(ramDbCfgPtr);

    cpssOsMemSet(&blocksSizeArray,0,sizeof(blocksSizeArray));

    rc = appDemoBc2IpLpmRamDefaultConfigCalc_elements(devNum,
                                                      PRV_CPSS_DXCH_PP_MAC(devNum)->moduleCfg.ip.maxNumOfPbrEntries,
                                                      &numOfBlocks,
                                                      blocksSizeArray,
                                                      &blocksAllocationMethod);
    if (rc != GT_OK)
    {
        return rc;
    }

    ramDbCfgPtr->numOfBlocks = numOfBlocks;
    for (i=0;i<numOfBlocks;i++)
    {
        ramDbCfgPtr->blocksSizeArray[i] = blocksSizeArray[i]; 
    }
    switch (blocksAllocationMethod)
    {
        case 0:
            ramDbCfgPtr->blocksAllocationMethod = CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E;
            break;
        case 1:
            ramDbCfgPtr->blocksAllocationMethod = CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITH_BLOCK_SHARING_E;
            break;
        default:
            return GT_BAD_VALUE;        
    }
    return rc;
}

/*******************************************************************************
* wrCpssDxChIpLpmDBCreate_1
*
* DESCRIPTION:
*   This function creates an LPM DB for a shared LPM managment.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - devNum to be used to calculate legal RAM configuration
*       lpmDBId               - the LPM DB id.
*       protocolStack         - the type of protocol stack this LPM DB support.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_ALREADY_EXIST - if the LPM DB id is already used.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBCreate_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                       lpmDBId,devNum;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT             shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                   protocolStack;    
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT           lpmMemoryConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U32)inArgs[0];
    lpmDBId = (GT_U32)inArgs[1];    
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    shadowType = CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E;

    cpssOsMemSet(&lpmMemoryConfig, 0, sizeof(lpmMemoryConfig));
    status = prvCpssDxChIpLpmRamDefaultConfigCalc((GT_U8)devNum,&lpmMemoryConfig.ramDbCfg);
    if (status != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, status, "");
    }
    /* call cpss api function */
    status = cpssDxChIpLpmDBCreate(lpmDBId, shadowType, protocolStack,&lpmMemoryConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBDelete
*
* DESCRIPTION:
*   This function deletes LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId           - the LPM DB id (range 32 bits)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Deleting LPM DB is allowed only if no VR present in the LPM DB (since
*       prefixes reside within VR it means that no prefixes present as well).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status = GT_OK;
    GT_U32      lpmDBId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    status = cpssDxChIpLpmDBDelete(lpmDBId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBConfigGet
*
* DESCRIPTION:
*   This function retrieves configuration of the LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId                       - the LPM DB id.
*
* OUTPUTS:
*       shadowTypePtr                 - points to the shadow type
*       protocolStackPtr              - points to the protocol stack this LPM DB support
*       indexesRangePtr               - the range of TCAM indexes availble for this LPM DB
*                                       (refer to CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC)
*       partitionEnablePtr            - GT_TRUE:  TCAM is partitioned to different prefix types
*                                       GT_FALSE: TCAM entries are allocated on demand
*       tcamLpmManagerCapcityCfgPtr   - holds the capacity configuration required
*                                       from this TCAM LPM manager
*       isTcamManagerInternalPtr      - GT_TRUE:  TCAM manager is used and created
*                                                 internally by the LPM DB
*                                       GT_FALSE: otherwise
*       tcamManagerHandlerPtr         - the TCAM manager handler, NULL if TCAM
*                                       manager is not used
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PTR               - NULL pointer.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       status;
    GT_U32                                          lpmDBId;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT                shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC    indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC    tcamLpmManagerCapcityCfg;
    GT_BOOL                                         isTcamManagerInternal;
    GT_PTR                                          tcamManagerHandler;
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT              lpmMemoryConfig;

#ifdef IS_64BIT_OS
    GT_CHAR *gFormat = "%d%d%d%d%d%d%d%d%d%l";
#else
    GT_CHAR *gFormat = "%d%d%d%d%d%d%d%d%d%d";
#endif /* IS_64BIT_OS */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    lpmMemoryConfig.tcamDbCfg.indexesRangePtr = &indexesRange;
    lpmMemoryConfig.tcamDbCfg.tcamLpmManagerCapcityCfgPtr = &tcamLpmManagerCapcityCfg;
    lpmMemoryConfig.tcamDbCfg.tcamManagerHandlerPtr = &tcamManagerHandler;
    status = cpssDxChIpLpmDBConfigGet(lpmDBId,&shadowType,&protocolStack,
                                      &lpmMemoryConfig);

    if (shadowType != CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E)
    {
        isTcamManagerInternal = (lpmMemoryConfig.tcamDbCfg.tcamManagerHandlerPtr ? GT_FALSE : GT_TRUE);
        galtisOutput(outArgs, status, gFormat,
                     shadowType,protocolStack,
                     lpmMemoryConfig.tcamDbCfg.indexesRangePtr->firstIndex,
                     lpmMemoryConfig.tcamDbCfg.indexesRangePtr->lastIndex,
                     lpmMemoryConfig.tcamDbCfg.partitionEnable,
                     lpmMemoryConfig.tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4Prefixes,
                     lpmMemoryConfig.tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv4McSourcePrefixes,
                     lpmMemoryConfig.tcamDbCfg.tcamLpmManagerCapcityCfgPtr->numOfIpv6Prefixes,
                     isTcamManagerInternal,(GT_UINTPTR)lpmMemoryConfig.tcamDbCfg.tcamManagerHandlerPtr);
    }
    else
    {
        galtisOutput(outArgs, status, gFormat,
                     shadowType,protocolStack,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0,
                     0);
    }

    return CMD_OK;
}
/*******************************************************************************
* wrCpssDxChIpLpmDBConfigGet_1
*
* DESCRIPTION:
*   This function retrieves configuration of the LPM DB.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDBId                       - the LPM DB id.
*
* OUTPUTS:
*       shadowTypePtr                 - points to the shadow type
*       protocolStackPtr              - points to the protocol stack this LPM DB support 
*       blocksAllocationMethodPtr     - points to the method of blocks allocation 
*       numOfBlocksPtr                - points to the number of RAM blocks that LPM uses. 
*       blocksSizeArray               - array that holds the sizes of the RAM blocks in bytes
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PTR               - NULL pointer.
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBConfigGet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       status;
    GT_U32                                          lpmDBId;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT                shadowType;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    CPSS_DXCH_IP_LPM_MEMORY_CONFIG_UNT              lpmMemoryConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    cpssOsMemSet(&lpmMemoryConfig.ramDbCfg,0,sizeof(CPSS_DXCH_LPM_RAM_CONFIG_STC));
    status = cpssDxChIpLpmDBConfigGet(lpmDBId,&shadowType,&protocolStack,
                                      &lpmMemoryConfig);

    if (shadowType != CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E)
    {        
         galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                     shadowType,protocolStack,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
    }
    else
    {
        galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                     shadowType,protocolStack,
                     lpmMemoryConfig.ramDbCfg.blocksAllocationMethod,
                     lpmMemoryConfig.ramDbCfg.numOfBlocks,
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[0],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[1],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[2],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[3],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[4],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[5],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[6],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[7],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[8],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[9],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[10],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[11],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[12],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[13],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[14],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[15],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[16],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[17],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[18],
                     lpmMemoryConfig.ramDbCfg.blocksSizeArray[19]);
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmDBCreateCh2Mode
*
* DESCRIPTION:
*   This function chooses an LPM DB creation mode for a shared LPM managment.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       isVrSupportEnable - if GT_TRUE - when wrCpssDxChIpLpmDBCreate is called
*                                        it will create LPM Db for Ch2 with Vr
*                                        support
*                           if GT_FALSE - when wrCpssDxChIpLpmDBCreate is called
*                                        it will create LPM Db for Ch2 without Vr
*                                        support
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChIpLpmDBCreateCh2Mode
(
    IN  GT_BOOL isVrSupportEnable
)
{
    gIsLpmDbCreateWithVrSupport = isVrSupportEnable;
    return CMD_OK;
}

/****************cpssDxChIpLpmDBDevList Table************/
static GT_U8*        gDevList = NULL;
static GT_U32        gNumOfDevs;

/*******************************************************************************
* cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBDevListAddSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                          ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    gNumOfDevs = 0;/*reset on first*/

    /* map input arguments to locals */
    if(gDevList == NULL)
    {
        gDevList = (GT_U8*)cmdOsMalloc(sizeof(GT_U8));
    }
    if(gDevList == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }


    ind = (GT_U32)inFields[0];

    if(ind > gNumOfDevs)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gDevList[ind] = (GT_U8)inFields[1];

    gNumOfDevs++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBDevListAddSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    gDevList = (GT_U8*)
       cmdOsRealloc(gDevList,
       sizeof(GT_U8) * (gNumOfDevs+1));

    if(gDevList == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > gNumOfDevs)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    gDevList[ind] = (GT_U8)inFields[1];

    gNumOfDevs++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing LPM DB . this addition will
*   invoke a hot sync of the newly added devices.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to add to the LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBDevListAddEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32      lpmDBId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    status =  cpssDxChIpLpmDBDevListAdd(lpmDBId, gDevList, gNumOfDevs);

    cmdOsFree(gDevList);
    gDevList = NULL;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevsListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing LPM DB . this remove will
*   invoke a hot sync removal of the devices.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId        - the LPM DB id.
*       devList        - the array of device ids to remove from the
*                        LPM DB.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK          - on success
*       GT_NOT_FOUND - if the LPM DB id is not found.
*       GT_BAD_PARAM   - on wrong parameters
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBDevsListRemove
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32   lpmDBId;
    GT_U8    devList[1];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    devList[0] = (GT_U8)inFields[1];

    /* call cpss api function */
    status =  cpssDxChIpLpmDBDevsListRemove(lpmDBId, devList, 1);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterPclActionMap_Set
*
* DESCRIPTION:
*       This function maps input arguments to Router PCL Action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterPclActionMap_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_PCL_ACTION_STC                *pclActionPtr = NULL;
    /* inFields[3] indicate if ipv4 or ipv6 pcl action data */
    if (inFields[3] == 0) /* ipv4 */
    {
        pclActionPtr   = &vrConfigInfo_2.defIpv4UcNextHopInfo.pclIpUcAction;
    }
    else /* ipv6 */
    {
        pclActionPtr = &vrConfigInfo_2.defIpv6UcNextHopInfo.pclIpUcAction;
    }
    pclActionPtr->pktCmd = (CPSS_PACKET_CMD_ENT)inFields[4];
    pclActionPtr->mirror.cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[5];
    pclActionPtr->mirror.mirrorToRxAnalyzerPort = (GT_BOOL)inFields[6];
    pclActionPtr->matchCounter.enableMatchCount = (GT_BOOL)inFields[7];
    pclActionPtr->matchCounter.matchCounterIndex = (GT_U32)inFields[8];
    pclActionPtr->egressPolicy = (GT_BOOL)inFields[9];

    if(pclActionPtr->egressPolicy != GT_FALSE)
    {
        pclActionPtr->qos.egress.modifyDscp =
            (inFields[10] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? GT_TRUE : GT_FALSE;
        pclActionPtr->qos.egress.modifyUp =
            (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
        pclActionPtr->qos.egress.dscp = (GT_U8)inFields[12];
        pclActionPtr->qos.egress.up = (GT_U8)inFields[13];
    }
    else
    {
        pclActionPtr->qos.ingress.modifyDscp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
        pclActionPtr->qos.ingress.modifyUp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
        pclActionPtr->qos.ingress.profileIndex = (GT_U32)inFields[14];
        pclActionPtr->qos.ingress.profileAssignIndex = (GT_BOOL)inFields[15];
        pclActionPtr->qos.ingress.profilePrecedence =
                        (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[16];
    }
    pclActionPtr->redirect.redirectCmd =
                              (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[17];

    if(pclActionPtr->redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        pclActionPtr->redirect.data.outIf.outInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inFields[18];
        pclActionPtr->redirect.data.outIf.outInterface.devPort.
                                                    hwDevNum = (GT_U8)inFields[19];
        pclActionPtr->redirect.data.outIf.outInterface.devPort.
                                                    portNum = (GT_U8)inFields[20];

        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(pclActionPtr->redirect.data.outIf.outInterface.devPort.hwDevNum,
                             pclActionPtr->redirect.data.outIf.outInterface.devPort.portNum);

        pclActionPtr->redirect.data.outIf.outInterface.trunkId =
                                                        (GT_TRUNK_ID)inFields[21];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(pclActionPtr->redirect.data.outIf.outInterface.trunkId);
        pclActionPtr->redirect.data.outIf.outInterface.vidx = (GT_U16)inFields[22];
        pclActionPtr->redirect.data.outIf.outInterface.vlanId = (GT_U16)inFields[23];
        pclActionPtr->redirect.data.outIf.vntL2Echo = (GT_BOOL)inFields[24];
        pclActionPtr->redirect.data.outIf.tunnelStart = (GT_BOOL)inFields[25];
        pclActionPtr->redirect.data.outIf.tunnelPtr = (GT_U32)inFields[26];
    }
    if(pclActionPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        pclActionPtr->redirect.data.routerLttIndex = (GT_U32)inFields[27];
    }

    pclActionPtr->policer.policerEnable = (GT_BOOL)inFields[28];
    pclActionPtr->policer.policerId = (GT_U32)inFields[29];

    switch (pclActionPtr->egressPolicy)
    {
        case GT_FALSE:
            pclActionPtr->vlan.ingress.modifyVlan =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[30];
            pclActionPtr->vlan.ingress.nestedVlan = (GT_BOOL)inFields[31];
            pclActionPtr->vlan.ingress.vlanId = (GT_U16)inFields[32];
            pclActionPtr->vlan.ingress.precedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
            break;

        default:
        case GT_TRUE:
            pclActionPtr->vlan.egress.vlanCmd =
                ((inFields[30] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                 || (inFields[30] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
            pclActionPtr->vlan.egress.vlanId = (GT_U16)inFields[32];
            break;
    }

    pclActionPtr->ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[34];
    pclActionPtr->ipUcRoute.arpDaIndex = (GT_U32)inFields[35];
    pclActionPtr->ipUcRoute.decrementTTL = (GT_BOOL)inFields[36];
    pclActionPtr->ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[37];
    pclActionPtr->ipUcRoute.icmpRedirectCheck = (GT_BOOL)inFields[38];
    if (inFields[0] == 0) /* ipv4 */
    {
        ipv4_set = GT_TRUE;
    }
    else /* ipv6 */
    {
        ipv6_set = GT_TRUE;
    }
    return CMD_OK;

}
/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*       vrConfigPtr          - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAddPCL_2SetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_IP_PROTOCOL_STACK_ENT               protocolStack;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&vrConfigInfo_2 , 0, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));
    vrConfigInfo_2.supportIpv4Mc = GT_FALSE;
    vrConfigInfo_2.supportIpv6Mc = GT_FALSE;
    vrConfigInfo_2.supportIpv4Uc = GT_FALSE;
    vrConfigInfo_2.supportIpv6Uc = GT_FALSE;
    ipv4_set = GT_FALSE;
    ipv6_set = GT_FALSE;

    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[2];

    /* Check Validity */
    if (inFields[0] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be ipv4 or ipv6).\n");
        return CMD_AGENT_ERROR;
    }

    if ((protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
    {
        vrConfigInfo_2.supportIpv4Uc = GT_TRUE;

    }
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
    {
        vrConfigInfo_2.supportIpv6Uc = GT_TRUE;

    }

    wrCpssDxChIpLpmVirtualRouterPclActionMap_Set(inArgs,inFields,numFields,outArgs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*       vrConfigPtr          - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAddPCL_2SetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_STATUS                               status = GT_OK;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inFields[0];
    vrId = (GT_U32)inFields[1];
    /* Check Validity */
    if (inFields[0] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be ipv4 or ipv6).\n");
        return CMD_AGENT_ERROR;
    }
    if ((inFields[0] == 0) && ipv4_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {ipv4,ipv6}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[0] == 1) && ipv6_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {ipv4,ipv6}.\n");
        return CMD_AGENT_ERROR;
    }


    wrCpssDxChIpLpmVirtualRouterPclActionMap_Set(inArgs,inFields,numFields,outArgs);

    /* call cpss api function */
    if ((ipv4_set == GT_TRUE) && (ipv6_set == GT_TRUE))
    {
        status =  cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo_2);
        /* clear globals */
        ipv4_set = GT_FALSE;
        ipv6_set = GT_FALSE;
        cmdOsMemSet(&vrConfigInfo_2 , 0, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*       vrConfigPtr          - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAddPCL_2EndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (ipv4_set || ipv6_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}



/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_PCL_2_EntryGet
*
* DESCRIPTION:
*       This function get single virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_PCL_2_EntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               status;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo;
    CPSS_DXCH_PCL_ACTION_STC                *pclActionPtr = NULL;
    GT_U32 i;
    GT_HW_DEV_NUM                           hwDev;   /* Dummy for converting. */
    GT_PORT_NUM                             port;  /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    cmdOsMemSet(&vrConfigInfo,0,sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));

    /* first to get Ipv4 entry */

    /* Call cpss api function. Note that the virtual router must exists as it
       is in the LPM DB virtual routers list */
    status =  cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfigInfo);

    if(status != GT_OK)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    /* determine the supported protocols */
    if ((vrConfigInfo.supportIpv6Uc == GT_TRUE))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
    }
    else
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    }

    /* if protocol stack was determined to be ipv6, we will check if it ipv4v6 */
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) && (vrConfigInfo.supportIpv4Uc == GT_TRUE))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    }
    inFields[0] = lpmDBId;
    inFields[1] = vrIdValue;
    inFields[3] = protocolStack;

    for (i = 0; i < 2; i++)
    {

        inFields[4] = i;      /* ipv4/6 pcl action line */

        /* if UC is not supported zero out vrConfigInfo to get deteministic results */
        if (vrConfigInfo.supportIpv4Uc == GT_FALSE)
            cmdOsMemSet(&vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction, 0,sizeof(CPSS_DXCH_PCL_ACTION_STC));
        if (vrConfigInfo.supportIpv6Uc == GT_FALSE)
            cmdOsMemSet(&vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction, 0,sizeof(CPSS_DXCH_PCL_ACTION_STC));

        if (i == 0) /* ipv4 */
        {
            pclActionPtr   = &vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction;
        }
        else /* ipv6 */
        {
            pclActionPtr = &vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction;
        }

        inFields[2] = pclActionPtr->pktCmd;
        inFields[5] = pclActionPtr->mirror.cpuCode;
        inFields[6] = pclActionPtr->mirror.mirrorToRxAnalyzerPort;
        inFields[7] = pclActionPtr->matchCounter.enableMatchCount;
        inFields[8] = pclActionPtr->matchCounter.matchCounterIndex;
        inFields[9] = pclActionPtr->egressPolicy;

        if(pclActionPtr->egressPolicy != GT_FALSE)
        {
            inFields[10] =
                (pclActionPtr->qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                    ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                    : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[11] =
                    (pclActionPtr->qos.egress.modifyUp ==
                     CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                        ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                        : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[12] = pclActionPtr->qos.egress.dscp;
            inFields[13] = pclActionPtr->qos.egress.up;
        }
        else
        {
            inFields[10] = pclActionPtr->qos.ingress.modifyDscp;
            inFields[11] = pclActionPtr->qos.ingress.modifyUp;
            inFields[14] = pclActionPtr->qos.ingress.profileIndex;
            inFields[15] = pclActionPtr->qos.ingress.profileAssignIndex;
            inFields[16] = pclActionPtr->qos.ingress.profilePrecedence;
        }

        inFields[17] = pclActionPtr->redirect.redirectCmd;


        if(pclActionPtr->redirect.redirectCmd ==
                                        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
        {
            inFields[18] = pclActionPtr->redirect.data.outIf.outInterface.type;

            hwDev  = pclActionPtr->redirect.data.outIf.outInterface.devPort.hwDevNum ;
            port = pclActionPtr->redirect.data.outIf.outInterface.devPort.portNum;
            CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
            inFields[19] = hwDev;
            inFields[20] = port;
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(pclActionPtr->redirect.data.outIf.outInterface.trunkId);
            inFields[21] = pclActionPtr->redirect.data.outIf.outInterface.trunkId;
            inFields[22] = pclActionPtr->redirect.data.outIf.outInterface.vidx;
            inFields[23] = pclActionPtr->redirect.data.outIf.outInterface.vlanId;
            inFields[24] = pclActionPtr->redirect.data.outIf.vntL2Echo;
            inFields[25] = pclActionPtr->redirect.data.outIf.tunnelStart;
            inFields[26] = pclActionPtr->redirect.data.outIf.tunnelPtr;
        }
        if(pclActionPtr->redirect.redirectCmd
                                    == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
        {
            inFields[27] = pclActionPtr->redirect.data.routerLttIndex;
        }

        inFields[28] = pclActionPtr->policer.policerEnable;
        inFields[29] = pclActionPtr->policer.policerId;
        if (pclActionPtr->egressPolicy == GT_FALSE)
        {
            inFields[30] = pclActionPtr->vlan.ingress.modifyVlan;
            inFields[31] = pclActionPtr->vlan.ingress.nestedVlan;
            inFields[32] = pclActionPtr->vlan.ingress.vlanId;
            inFields[33] = pclActionPtr->vlan.ingress.precedence;
        }
        else
        {
            inFields[30] =
                (pclActionPtr->vlan.egress.vlanCmd ==
                 CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                    ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                    : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            inFields[32] = pclActionPtr->vlan.egress.vlanId;
        }
        inFields[34] = pclActionPtr->ipUcRoute.doIpUcRoute;
        inFields[35] = pclActionPtr->ipUcRoute.arpDaIndex;
        inFields[36] = pclActionPtr->ipUcRoute.decrementTTL;
        inFields[37] = pclActionPtr->ipUcRoute.bypassTTLCheck;
        inFields[38] = pclActionPtr->ipUcRoute.icmpRedirectCheck;
        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            inFields[0], inFields[1], inFields[2], inFields[3],inFields[4],
            inFields[5], inFields[6],inFields[7], inFields[8], inFields[9],
            inFields[10],inFields[11], inFields[12], inFields[13],inFields[14],
            inFields[15],inFields[16], inFields[17],inFields[18], inFields[19],
            inFields[20],inFields[21], inFields[22],inFields[23], inFields[24],
            inFields[25],inFields[26], inFields[27],inFields[28], inFields[29],
            inFields[30],inFields[31], inFields[32],inFields[33], inFields[34],
            inFields[35],inFields[36], inFields[37],inFields[38]);
    }
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAddPCL_2GetFirst
*
* DESCRIPTION:
*       This function get first virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAddPCL_2GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDbIterator   = 0;    /*reset on first*/
    lpmDBId         = 0;    /*reset on first*/
    lpmDbPtr        = NULL;
    vrIterator      = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/
    vrEntryPtr      = NULL;

    /* get first LPM DB that has virtual router */
    while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
    {
        lpmDBId = lpmDbPtr->lpmDbId;
        if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
        {
            continue;
        }

        /* get first virtual router */
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;
            return wrCpssDxChIpLpmVirtualRouterAdd_PCL_2_EntryGet(inArgs,inFields,numFields,outArgs);
        }
    }
    /* there are no LPM DB with virtual routers */
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAddPCL_2GetNext
*
* DESCRIPTION:
*       This function get next virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAddPCL_2GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    /* get next virtual router within the current LPM DB */
    tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
    while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
    {
        vrIdValue = vrEntryPtr->vrId;

        return wrCpssDxChIpLpmVirtualRouterAdd_PCL_2_EntryGet(inArgs,inFields,numFields,outArgs);
    }

    /* no more virtual routers in the current LPM DB,
       find next LPM DB that has virtual router */
    while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
    {
        lpmDBId = lpmDbPtr->lpmDbId;

        /* get first virtual router */
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;

            return wrCpssDxChIpLpmVirtualRouterAdd_PCL_2_EntryGet(inArgs,inFields,numFields,outArgs);
        }
    }

    /* there are no more LPM DB with virtual routers */
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       defUcNextHopInfoPtr - the route entry info accosiated with the default
*                             UC route entry.
*       defMcRouteLttEntryPtr-the LTT entry pointing to the default MC route
*                             entry.
*                             relevant for cheetha2 devices only, for others
*                             it should be NULL
*       protocolStack       - types of IP stack used in this virtual router.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate TCAM memory.
*       GT_BAD_STATE - if the existing VR is not empty.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_PCL
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  defUcNextHop;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&defUcNextHop, 0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inArgs[1];
    defUcNextHop.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inArgs[2];
    defUcNextHop.pclIpUcAction.mirror.cpuCode =
                                            (CPSS_NET_RX_CPU_CODE_ENT)inArgs[3];
    defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                            (GT_BOOL)inArgs[4];
    defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount =
                                                            (GT_BOOL)inArgs[5];
    defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex =
                                                            (GT_U32)inArgs[6];
    defUcNextHop.pclIpUcAction.egressPolicy = (GT_BOOL)inArgs[7];


    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        defUcNextHop.pclIpUcAction.qos.egress.modifyDscp =
            (inFields[8] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? GT_TRUE : GT_FALSE;
        defUcNextHop.pclIpUcAction.qos.egress.modifyUp =
            (inFields[9] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
        defUcNextHop.pclIpUcAction.qos.egress.dscp = (GT_U8)inArgs[13];
        defUcNextHop.pclIpUcAction.qos.egress.up = (GT_U8)inArgs[14];
    }
    else
    {
        defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp =
                              (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inArgs[8];
        defUcNextHop.pclIpUcAction.qos.ingress.modifyUp =
                              (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inArgs[9];
        defUcNextHop.pclIpUcAction.qos.ingress.profileIndex =
                                                            (GT_U32)inArgs[10];
        defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex =
                                                           (GT_BOOL)inArgs[11];
        defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence =
                        (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[12];
    }
    defUcNextHop.pclIpUcAction.redirect.redirectCmd =
                              (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inArgs[15];

    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inArgs[16];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    hwDevNum = (GT_U8)inArgs[17];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    portNum = (GT_U8)inArgs[18];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                             defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                        (GT_TRUNK_ID)inArgs[19];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                            (GT_U16)inArgs[20];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                            (GT_U16)inArgs[21];

        defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                            (GT_BOOL)inArgs[22];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                            (GT_BOOL)inArgs[23];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                            (GT_U32)inArgs[24];
    }
    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex =
                                                            (GT_U32)inArgs[25];
    }

    defUcNextHop.pclIpUcAction.policer.policerEnable = (GT_BOOL)inArgs[26];
    defUcNextHop.pclIpUcAction.policer.policerId = (GT_U32)inArgs[27];

    switch (defUcNextHop.pclIpUcAction.egressPolicy)
    {
        case GT_FALSE:
            defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inArgs[29];
            defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inArgs[30];
            defUcNextHop.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inArgs[31];
            defUcNextHop.pclIpUcAction.vlan.ingress.precedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[32];
            break;

        default:
        case GT_TRUE:
            defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd =
                ((inArgs[29] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                 || (inArgs[29] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
            defUcNextHop.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inArgs[31];
            break;
    }

    defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inArgs[33];
    defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inArgs[34];
    defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inArgs[35];
    defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inArgs[36];
    defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                            (GT_BOOL)inArgs[37];

    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[38];
    vrConfigInfo.supportIpv4Mc = GT_FALSE;
    vrConfigInfo.supportIpv6Mc = GT_FALSE;
    vrConfigInfo.supportIpv4Uc = GT_FALSE;
    vrConfigInfo.supportIpv6Uc = GT_FALSE;

    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction,
                    &defUcNextHop.pclIpUcAction,
                    sizeof(CPSS_DXCH_PCL_ACTION_STC));
    }
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction,
                    &defUcNextHop.pclIpUcAction,
                    sizeof(CPSS_DXCH_PCL_ACTION_STC));

    }

    /* call cpss api function */
    status =  cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************************** Table function **********************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAddPCLEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  defUcNextHop;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&defUcNextHop, 0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inFields[0];

    vrId = (GT_U32)inFields[1];
    defUcNextHop.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[2];
    defUcNextHop.pclIpUcAction.mirror.cpuCode =
                                            (CPSS_NET_RX_CPU_CODE_ENT)inFields[3];
    defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                            (GT_BOOL)inFields[4];
    defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount =
                                                            (GT_BOOL)inFields[5];
    defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex =
                                                            (GT_U32)inFields[6];
    defUcNextHop.pclIpUcAction.egressPolicy = (GT_BOOL)inFields[7];

    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        defUcNextHop.pclIpUcAction.qos.egress.modifyDscp =
            (inFields[8] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? GT_TRUE : GT_FALSE;
        defUcNextHop.pclIpUcAction.qos.egress.modifyUp =
            (inFields[9] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
        defUcNextHop.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[10];
        defUcNextHop.pclIpUcAction.qos.egress.up = (GT_U8)inFields[11];
    }
    else
    {
        defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp =
                              (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[8];
        defUcNextHop.pclIpUcAction.qos.ingress.modifyUp =
                              (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[9];
        defUcNextHop.pclIpUcAction.qos.ingress.profileIndex =
                                                            (GT_U32)inFields[12];
        defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex =
                                                           (GT_BOOL)inFields[13];
        defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence =
                        (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[14];
    }
    defUcNextHop.pclIpUcAction.redirect.redirectCmd =
                              (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[15];

    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inFields[16];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    hwDevNum = (GT_U8)inFields[17];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    portNum = (GT_U8)inFields[18];

        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                             defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                        (GT_TRUNK_ID)inFields[19];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                            (GT_U16)inFields[20];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                            (GT_U16)inFields[21];

        defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                            (GT_BOOL)inFields[22];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                            (GT_BOOL)inFields[23];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                            (GT_U32)inFields[24];
    }
    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex =
                                                            (GT_U32)inFields[25];
    }

    defUcNextHop.pclIpUcAction.policer.policerEnable = (GT_BOOL)inFields[26];
    defUcNextHop.pclIpUcAction.policer.policerId = (GT_U32)inFields[27];

    switch (defUcNextHop.pclIpUcAction.egressPolicy)
    {
        case GT_FALSE:
            defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan =
                            (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[28];
            defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inFields[29];
            defUcNextHop.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inFields[30];
            defUcNextHop.pclIpUcAction.vlan.ingress.precedence =
                        (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[31];
            break;

        default:
        case GT_TRUE:
            defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd =
                ((inFields[28] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                 || (inFields[28] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
            defUcNextHop.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inFields[30];
            break;
    }

    defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[32];
    defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inFields[33];
    defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inFields[34];
    defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[35];
    defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                            (GT_BOOL)inFields[36];


    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[37];
    vrConfigInfo.supportIpv4Mc = GT_FALSE;
    vrConfigInfo.supportIpv6Mc = GT_FALSE;
    vrConfigInfo.supportIpv4Uc = GT_FALSE;
    vrConfigInfo.supportIpv6Uc = GT_FALSE;

    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.pclIpUcAction,
                    &defUcNextHop.pclIpUcAction,
                    sizeof(CPSS_DXCH_PCL_ACTION_STC));
    }
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.pclIpUcAction,
                    &defUcNextHop.pclIpUcAction,
                    sizeof(CPSS_DXCH_PCL_ACTION_STC));

    }

    /* call cpss api function */
    status =  cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       defUcNextHopInfoPtr - the route entry info accosiated with the default
*                             UC route entry.
*       defMcRouteLttEntryPtr-the LTT entry pointing to the default MC route
*                             entry.
*                             relevant for cheetha2 devices only, for others
*                             it should be NULL
*       protocolStack       - types of IP stack used in this virtual router.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - if failed to allocate TCAM memory.
*       GT_BAD_STATE - if the existing VR is not empty.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                               status;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  defUcNextHop;
    CPSS_DXCH_IP_LTT_ENTRY_STC              defMcRouteLttEntry;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&defUcNextHop, 0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inArgs[1];
    defUcNextHop.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[2];
    defUcNextHop.ipLttEntry.numOfPaths = (GT_U32)inArgs[3];
    defUcNextHop.ipLttEntry.routeEntryBaseIndex = (GT_U32)inArgs[4];
    defUcNextHop.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inArgs[5];
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inArgs[6];
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[7];

    defMcRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[8];
    defMcRouteLttEntry.numOfPaths = (GT_U32)inArgs[9];
    defMcRouteLttEntry.routeEntryBaseIndex = (GT_U32)inArgs[10];
    defMcRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inArgs[11];
    defMcRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inArgs[12];
    defMcRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[13];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defUcNextHop.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defMcRouteLttEntry);

    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[14];
    vrConfigInfo.supportIpv4Mc = GT_FALSE;
    vrConfigInfo.supportIpv6Mc = GT_FALSE;
    vrConfigInfo.supportIpv4Uc = GT_FALSE;
    vrConfigInfo.supportIpv6Uc = GT_FALSE;
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                    &defUcNextHop.ipLttEntry,
                    sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        if (gIsLpmDbCreateWithVrSupport == GT_FALSE)
        {
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv4McRouteLttEntry,
                        &defMcRouteLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

        }
    }
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                    &defUcNextHop.ipLttEntry,
                    sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        if (gIsLpmDbCreateWithVrSupport == GT_FALSE)
        {
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv6McRouteLttEntry,
                        &defMcRouteLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
    }

    status =  cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************************** Table function **********************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  defUcNextHop;
    CPSS_DXCH_IP_LTT_ENTRY_STC              defMcRouteLttEntry;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&defUcNextHop, 0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inFields[0];

    vrId = (GT_U32)inFields[1];
    defUcNextHop.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[2];
    defUcNextHop.ipLttEntry.numOfPaths = (GT_U32)inFields[3];
    defUcNextHop.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[4];
    defUcNextHop.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[5];
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[6];
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[7];

    defMcRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[8];
    defMcRouteLttEntry.numOfPaths = (GT_U32)inFields[9];
    defMcRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[10];
    defMcRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[11];
    defMcRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[12];
    defMcRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[13];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defUcNextHop.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defMcRouteLttEntry);

    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[14];
    vrConfigInfo.supportIpv4Mc = GT_FALSE;
    vrConfigInfo.supportIpv6Mc = GT_FALSE;
    vrConfigInfo.supportIpv4Uc = GT_FALSE;
    vrConfigInfo.supportIpv6Uc = GT_FALSE;
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv4Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                    &defUcNextHop.ipLttEntry,
                    sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        if (gIsLpmDbCreateWithVrSupport == GT_FALSE)
        {
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv4McRouteLttEntry,
                        &defMcRouteLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

        }
    }
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        vrConfigInfo.supportIpv6Uc = GT_TRUE;
        cmdOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                    &defUcNextHop.ipLttEntry,
                    sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        if (gIsLpmDbCreateWithVrSupport == GT_FALSE)
        {
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv6McRouteLttEntry,
                        &defMcRouteLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
    }

    status =  cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*                             future support only , currently only vrId = 0 is
*                             supported.
*
* OUTPUTS:
*       defUcNextHopInfoPtr - the route entry info accosiated with the default
*                             UC route entry.
*       defMcRouteLttEntryPtr-the LTT entry pointing to the default MC route
*                             entry.
*                             relevant for cheetha2 devices only, for others
*                             it should be NULL
*       protocolStackPtr    - types of IP stack used in this virtual router.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;
    GT_U32      vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDbIterator   = 0;/*reset on first*/
    lpmDBId         = 0;/*reset on first*/
    lpmDbPtr        = NULL;
    vrIterator      = 0;/*reset on first*/
    vrIdValue       = 0;/*reset on first*/
    vrEntryPtr      = NULL;
    protocolStack   = CPSS_IP_PROTOCOL_IPV4_E;

    /* get the first lpmDbPtr when lpmDbIterator=0 */
    lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator);
    if (lpmDbPtr == NULL)
    {
        /* can't find any lpm DB */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        /* vrEntryRam is used for TCAM backward compatible code */
        vrEntryRam.valid=GT_TRUE;

        ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
        for (vrId = 0; vrId < ramShadow->vrfTblSize; vrId++)
        {
            status = cpssDxChIpLpmVirtualRouterGet(lpmDbPtr->lpmDbId,vrId,&vrConfig);

            if (status == GT_OK)
            {
                vrIdValue = vrId;
                vrEntryRam.vrId = vrId;
                vrEntryPtr = &vrEntryRam;
                break;
            }
            else
            {
                if (status != GT_NOT_FOUND)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
        }
        if (vrId == ramShadow->vrfTblSize)
        {
            /* can't find any VR */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }
    else
    {
        /* Get the first vrEntryPtr when vrIterator=0.*/
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator);
        if (vrEntryPtr == NULL)
        {
            /* can't find any VR */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        vrIdValue   = vrEntryPtr->vrId;
    }

    lpmDBId     = lpmDbPtr->lpmDbId;

    /* reset all fields */
    cmdOsMemSet(&getVrConfigInfo, 0, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));

    /* call cpss api function */
    status =  cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrIdValue, &getVrConfigInfo);
    /* there is no entry for the given protocol */
    if(status == GT_NOT_FOUND)
    {
        return wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntryGetNext(inArgs,inFields,numFields,outArgs);
    }
    if(status != GT_OK)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &getVrConfigInfo.defIpv4McRouteLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &getVrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &getVrConfigInfo.defIpv6McRouteLttEntry);

    inFields[0] = lpmDBId;
    inFields[1] = vrIdValue;
    inFields[2] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeType;
    inFields[3] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths;
    inFields[4] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[5] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[6] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[7] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[8] = getVrConfigInfo.defIpv4McRouteLttEntry.routeType;
    inFields[9] = getVrConfigInfo.defIpv4McRouteLttEntry.numOfPaths;
    inFields[10] = getVrConfigInfo.defIpv4McRouteLttEntry.routeEntryBaseIndex;
    inFields[11] = getVrConfigInfo.defIpv4McRouteLttEntry.ucRPFCheckEnable;
    inFields[12] = getVrConfigInfo.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[13] = getVrConfigInfo.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel;

    /* determine the supported protocols */
    if ((getVrConfigInfo.supportIpv6Uc == GT_TRUE) || (getVrConfigInfo.supportIpv6Mc == GT_TRUE))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
    }
    else
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    }

    /* if protocol stack was determined to be ipv6, we will check if it ipv4v6 */
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) &&
        ((getVrConfigInfo.supportIpv4Uc == GT_TRUE) || (getVrConfigInfo.supportIpv4Mc == GT_TRUE)))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    }

    inFields[14] = protocolStack;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],inFields[4],
        inFields[5], inFields[6],inFields[7], inFields[8], inFields[9],
        inFields[10],inFields[11], inFields[12], inFields[13],inFields[14]);

    galtisOutput(outArgs, status, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       protocolStack       - types of IP stack used in this virtual router.
*
* OUTPUTS:
*       defUcNextHopInfoPtr - the route entry info accosiated with the default
*                             UC route entry.
*       defMcRouteLttEntryPtr-the LTT entry pointing to the default MC route
*                             entry.
*                             relevant for cheetha2 devices only, for others
*                             it should be NULL
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;
    GT_U32      vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if((lpmDBId       == lpmDbPtr->lpmDbId)&&
       (vrIdValue     == vrEntryPtr->vrId)&&
       (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E))
    {
        /* display the entry for IPV6 protocol */

        inFields[0] = lpmDBId;
        inFields[1] = vrIdValue;
        inFields[2] = getVrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeType;
        inFields[3] = getVrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths;
        inFields[4] = getVrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
        inFields[5] = getVrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
        inFields[6] = getVrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
        inFields[7] = getVrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
        inFields[8] = getVrConfigInfo.defIpv6McRouteLttEntry.routeType;
        inFields[9] = getVrConfigInfo.defIpv6McRouteLttEntry.numOfPaths;
        inFields[10] = getVrConfigInfo.defIpv6McRouteLttEntry.routeEntryBaseIndex;
        inFields[11] = getVrConfigInfo.defIpv6McRouteLttEntry.ucRPFCheckEnable;
        inFields[12] = getVrConfigInfo.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable;
        inFields[13] = getVrConfigInfo.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel;
        inFields[14] = CPSS_IP_PROTOCOL_IPV6_E;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            inFields[0], inFields[1], inFields[2], inFields[3],inFields[4],
            inFields[5], inFields[6],inFields[7], inFields[8], inFields[9],
            inFields[10],inFields[11], inFields[12], inFields[13],inFields[14]);

        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        galtisOutput(outArgs, GT_OK, "%f");
        return CMD_OK;

    }
    else
    {
        /* reset all fields */
        cmdOsMemSet(&getVrConfigInfo, 0, sizeof(CPSS_DXCH_IP_LPM_VR_CONFIG_STC));

        /* we need to set the protocol to IPV4 and get the next vrid valid entry*/
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;

        /* Get the next valid vrid */
        if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
        {
            ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
            for (vrId = vrIdValue + 1; vrId < ramShadow->vrfTblSize; vrId++)
            {
                status = cpssDxChIpLpmVirtualRouterGet(lpmDbPtr->lpmDbId,vrId,&vrConfig);
                if (status == GT_OK)
                {
                    vrIdValue = vrId;
                    break;
                }
                else
                {
                    if (status != GT_NOT_FOUND)
                    {
                        /* pack output arguments to galtis string */
                        galtisOutput(outArgs, status, "%d", -1);
                        return CMD_OK;
                    }
                }
            }
            if (vrId == ramShadow->vrfTblSize)
            {
                /* need to get the next valid lpmDbPtr, and reset the vrid iterator */

                /* get the next lpmDbPtr */
                lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator);
                if (lpmDbPtr == NULL)
                {
                    /* can't find any lpm DB */
                    galtisOutput(outArgs, GT_OK, "%d", -1);
                    return CMD_OK;
                }

                /* Get the first vrId */
                ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
                for (vrId = 0; vrId < ramShadow->vrfTblSize; vrId++)
                {
                    status = cpssDxChIpLpmVirtualRouterGet(lpmDbPtr->lpmDbId,vrId,&vrConfig);
                    if (status == GT_OK)
                    {
                        vrIdValue = vrId;
                        vrEntryRam.vrId = vrId;
                        vrEntryPtr = &vrEntryRam;
                        break;
                    }
                    else
                    {
                        if (status != GT_NOT_FOUND)
                        {
                            /* pack output arguments to galtis string */
                            galtisOutput(outArgs, status, "%d", -1);
                            return CMD_OK;
                        }
                    }
                }
                if (vrId == ramShadow->vrfTblSize)
                {
                    /* can't find any VR */
                    galtisOutput(outArgs, GT_OK, "%d", -1);
                    return CMD_OK;
                }
            }
        }
        else
        {
            tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
            vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator);
            if (vrEntryPtr == NULL)
            {
                /* need to get the next valid lpmDbPtr, and reset the vrid iterator */

                /* get the next lpmDbPtr */
                lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator);
                if (lpmDbPtr == NULL)
                {
                    /* can't find any lpm DB */
                    galtisOutput(outArgs, GT_OK, "%d", -1);
                    return CMD_OK;
                }

                /* Get the first vrEntryPtr when vrIterator=0.*/
                vrIterator = 0;
                tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
                vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator);
                if (vrEntryPtr == NULL)
                {
                    /* can't find any VR */
                    galtisOutput(outArgs, GT_OK, "%d", -1);
                    return CMD_OK;
                }
            }
            vrIdValue   = vrEntryPtr->vrId;
        }

        lpmDBId     = lpmDbPtr->lpmDbId;

        /* call cpss api function */
        status =  cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrIdValue, &getVrConfigInfo);
        /* there is no entry for the given protocol */
        if(status == GT_NOT_FOUND)
        {
            return wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntryGetNext(inArgs,inFields,numFields,outArgs);
        }
        if(status != GT_OK)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &getVrConfigInfo.defIpv4McRouteLttEntry);

        inFields[0] = lpmDBId;
        inFields[1] = vrIdValue;
        inFields[2] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeType;
        inFields[3] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths;
        inFields[4] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
        inFields[5] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
        inFields[6] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
        inFields[7] = getVrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
        inFields[8] = getVrConfigInfo.defIpv4McRouteLttEntry.routeType;
        inFields[9] = getVrConfigInfo.defIpv4McRouteLttEntry.numOfPaths;
        inFields[10] = getVrConfigInfo.defIpv4McRouteLttEntry.routeEntryBaseIndex;
        inFields[11] = getVrConfigInfo.defIpv4McRouteLttEntry.ucRPFCheckEnable;
        inFields[12] = getVrConfigInfo.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable;
        inFields[13] = getVrConfigInfo.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel;

        /* determine the supported protocols */
        if ((getVrConfigInfo.supportIpv6Uc == GT_TRUE) || (getVrConfigInfo.supportIpv6Mc == GT_TRUE))
        {
            protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
        }
        else
        {
            protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
        }

        /* if protocol stack was determined to be ipv6, we will check if it ipv4v6 */
        if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) &&
            ((getVrConfigInfo.supportIpv4Uc == GT_TRUE) || (getVrConfigInfo.supportIpv4Mc == GT_TRUE)))
        {
            protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
        }

        inFields[14] = protocolStack;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            inFields[0], inFields[1], inFields[2], inFields[3],inFields[4],
            inFields[5], inFields[6],inFields[7], inFields[8], inFields[9],
            inFields[10],inFields[11], inFields[12], inFields[13],inFields[14]);

        galtisOutput(outArgs, status, "%f");
        return CMD_OK;
    }
}


/********************************************/
/* cpssDxChIpLpmVirtualRouterAdd_LTT_1 Table */
/********************************************/

static GT_BOOL      sharedVrExists  = GT_FALSE;
static GT_U32       sharedVrId      = 0;

/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntrySet
*
* DESCRIPTION:
*       This function adds virtual router (shared or non-shared).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               status;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  defUcNextHop;
    CPSS_DXCH_IP_LTT_ENTRY_STC              defMcRoute;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    GT_BOOL                                 isSharedVr;
    GT_BOOL                                 supportUc;
    GT_BOOL                                 supportMc;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&defUcNextHop, 0,sizeof(defUcNextHop));

    lpmDBId                                             = (GT_U32)inFields[0];
    vrId                                                = (GT_U32)inFields[1];
    isSharedVr                                          = (GT_BOOL)inFields[2];
    supportUc                                           = (GT_BOOL)inFields[3];
    supportMc                                           = (GT_BOOL)inFields[4];

    defUcNextHop.ipLttEntry.routeType                   = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    defUcNextHop.ipLttEntry.numOfPaths                  = (GT_U32)inFields[6];
    defUcNextHop.ipLttEntry.routeEntryBaseIndex         = (GT_U32)inFields[7];
    defUcNextHop.ipLttEntry.ucRPFCheckEnable            = (GT_BOOL)inFields[8];
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable    = (GT_BOOL)inFields[9];
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel       = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[10];

    defMcRoute.routeType                                = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[11];
    defMcRoute.numOfPaths                               = (GT_U32)inFields[12];
    defMcRoute.routeEntryBaseIndex                      = (GT_U32)inFields[13];
    defMcRoute.ucRPFCheckEnable                         = (GT_BOOL)inFields[14];
    defMcRoute.sipSaCheckMismatchEnable                 = (GT_BOOL)inFields[15];
    defMcRoute.ipv6MCGroupScopeLevel                    = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[16];

    protocolStack                                       = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[17];
    vrConfigInfo.supportIpv4Mc = GT_FALSE;
    vrConfigInfo.supportIpv6Mc = GT_FALSE;
    vrConfigInfo.supportIpv4Uc = GT_FALSE;
    vrConfigInfo.supportIpv6Uc = GT_FALSE;

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defUcNextHop.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defMcRoute);

    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV4_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        if (supportUc == GT_TRUE)
        {
            vrConfigInfo.supportIpv4Uc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                        &defUcNextHop.ipLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
        if (supportMc == GT_TRUE)
        {
            vrConfigInfo.supportIpv4Mc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv4McRouteLttEntry,
                        &defMcRoute,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

        }
    }
    if ( (protocolStack == CPSS_IP_PROTOCOL_IPV6_E) || (protocolStack == CPSS_IP_PROTOCOL_IPV4V6_E) )
    {
        if (supportUc == GT_TRUE)
        {
            vrConfigInfo.supportIpv6Uc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                        &defUcNextHop.ipLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
         }
        if (supportMc == GT_TRUE)
        {
            vrConfigInfo.supportIpv6Mc = GT_TRUE;
            cmdOsMemCpy(&vrConfigInfo.defIpv6McRouteLttEntry,
                        &defMcRoute,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
    }

    /* call cpss api function */
    if (isSharedVr == GT_TRUE)
    {
        status = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId,vrId,&vrConfigInfo);

        /* if adding the shared VR succeeded, then save the shared VR
           id for later use by Get functions */
        if (status == GT_OK)
        {
            sharedVrExists  = GT_TRUE;
            sharedVrId      = vrId;
        }
    }
    else
    {
        status =  cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet
*
* DESCRIPTION:
*       This function get single virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               status;
    GT_BOOL                                 supportUc = GT_FALSE;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  defUcNextHop;
    GT_BOOL                                 supportMc = GT_FALSE;
    CPSS_DXCH_IP_LTT_ENTRY_STC              defMcRoute;
    CPSS_IP_PROTOCOL_STACK_ENT              protocolStack;
    GT_BOOL                                 isSharedVr;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&defUcNextHop, 0,sizeof(defUcNextHop));

    /* Call cpss api function. Note that the virtual router must exists as it
       is in the LPM DB virtual routers list */
    status =  cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfigInfo);

    if(status != GT_OK)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv4McRouteLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv6McRouteLttEntry);

    if ((sharedVrExists == GT_TRUE) && (vrIdValue == sharedVrId))
        isSharedVr = GT_TRUE;
    else
        isSharedVr = GT_FALSE;

    if ( (vrConfigInfo.supportIpv4Uc == GT_TRUE) || (vrConfigInfo.supportIpv6Uc == GT_TRUE) )
    {
        supportUc = GT_TRUE;
    }
    if ( (vrConfigInfo.supportIpv4Mc == GT_TRUE) || (vrConfigInfo.supportIpv6Mc == GT_TRUE) )
    {
        supportMc = GT_TRUE;
    }
    /* determine the supported protocols */
    if ((vrConfigInfo.supportIpv6Uc == GT_TRUE) || (vrConfigInfo.supportIpv6Mc == GT_TRUE))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV6_E;
    }
    else
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4_E;
    }

    /* if protocol stack was determined to be ipv6, we will check if it ipv4v6 */
    if ((protocolStack == CPSS_IP_PROTOCOL_IPV6_E) &&
        ((vrConfigInfo.supportIpv4Uc == GT_TRUE) || (vrConfigInfo.supportIpv4Mc == GT_TRUE)))
    {
        protocolStack = CPSS_IP_PROTOCOL_IPV4V6_E;
    }

    inFields[0] = lpmDBId;
    inFields[1] = vrIdValue;
    inFields[2] = isSharedVr;
    inFields[3] = supportUc;
    inFields[4] = supportMc;

    /* if UC is not supported zero out defUcNextHopInfo to get deteministic results */
    if (supportUc == GT_FALSE)
        cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));
    /* if MC is not supported zero out defMcRouteLttEntry to get deteministic results */
    if (supportMc == GT_FALSE)
        cmdOsMemSet(&defMcRoute,0,sizeof(defMcRoute));
    if (supportUc == GT_TRUE)
    {
        if( protocolStack == CPSS_IP_PROTOCOL_IPV6_E )
        {
            cmdOsMemCpy(&defUcNextHop.ipLttEntry,
                        &vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
        else
        {
            cmdOsMemCpy(&defUcNextHop.ipLttEntry,
                        &vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
    }

    if (supportMc == GT_TRUE)
    {
        if( protocolStack == CPSS_IP_PROTOCOL_IPV6_E )
        {
            cmdOsMemCpy(&defMcRoute,
                        &vrConfigInfo.defIpv6McRouteLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
        else
        {
            cmdOsMemCpy(&defMcRoute,
                        &vrConfigInfo.defIpv4McRouteLttEntry,
                        sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
        }
    }

    inFields[5] = defUcNextHop.ipLttEntry.routeType;
    inFields[6] = defUcNextHop.ipLttEntry.numOfPaths;
    inFields[7] = defUcNextHop.ipLttEntry.routeEntryBaseIndex;
    inFields[8] = defUcNextHop.ipLttEntry.ucRPFCheckEnable;
    inFields[9] = defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[10] = defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel;

    inFields[11] = defMcRoute.routeType;
    inFields[12] = defMcRoute.numOfPaths;
    inFields[13] = defMcRoute.routeEntryBaseIndex;
    inFields[14] = defMcRoute.ucRPFCheckEnable;
    inFields[15] = defMcRoute.sipSaCheckMismatchEnable;
    inFields[16] = defMcRoute.ipv6MCGroupScopeLevel;

    inFields[17] = protocolStack;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],inFields[4],
        inFields[5], inFields[6],inFields[7], inFields[8], inFields[9],
        inFields[10],inFields[11], inFields[12], inFields[13],inFields[14],
        inFields[15],inFields[16], inFields[17]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGetFirst
*
* DESCRIPTION:
*       This function get first virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         status = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDbIterator   = 0;    /*reset on first*/
    lpmDBId         = 0;    /*reset on first*/
    lpmDbPtr        = NULL;
    vrIterator      = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/
    vrEntryPtr      = NULL;

    /* get first LPM DB that has virtual router */
    while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
    {
        lpmDBId = lpmDbPtr->lpmDbId;

        /* get first virtual router */
        if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
        {
            ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
            for (vrIdValue = 0; vrIdValue < ramShadow->vrfTblSize; vrIdValue++)
            {
                status = cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfig);
                if (status == GT_OK)
                {
                    return wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet(inArgs,inFields,numFields,outArgs);
                }
                if(status != GT_OK)
                {
                    if (status != GT_NOT_FOUND)
                    {
                        galtisOutput(outArgs, status, "%d", -1);
                        return CMD_OK;
                    }
                }

            }
        }
        else
        {
            tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
            while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
            {
                vrIdValue = vrEntryPtr->vrId;

                return wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet(inArgs,inFields,numFields,outArgs);
            }
        }
    }

    /* there are no LPM DB with virtual routers */
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGetNext
*
* DESCRIPTION:
*       This function get next virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         status = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;

    GT_U32 vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next virtual router within the current LPM DB */
    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        /* vrEntryRam is used for TCAM backward compatible code */
        vrEntryRam.valid=GT_TRUE;

        ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
        for (vrId = vrIdValue + 1; vrId < ramShadow->vrfTblSize; vrId++)
        {
            status = cpssDxChIpLpmVirtualRouterGet(lpmDbPtr->lpmDbId,vrId,&vrConfig);
            if (status == GT_OK)
            {
                vrIdValue = vrId;
                vrEntryRam.vrId = vrId;
                vrEntryPtr = &vrEntryRam;
                return wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet(inArgs,inFields,numFields,outArgs);
            }
            if(status != GT_OK)
            {
                if (status != GT_NOT_FOUND)
                {
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
        }

        while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
        {
            lpmDBId = lpmDbPtr->lpmDbId;

            /* get first virtual router */
            ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
            for (vrId = 0; vrId < ramShadow->vrfTblSize; vrId++)
            {
                status = cpssDxChIpLpmVirtualRouterGet(lpmDbPtr->lpmDbId,vrId,&vrConfig);
                if (status == GT_OK)
                {
                    vrIdValue = vrId;
                    vrEntryRam.vrId = vrId;
                    vrEntryPtr = &vrEntryRam;
                    return wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet(inArgs,inFields,numFields,outArgs);
                }
                if(status != GT_OK)
                {
                    if (status != GT_NOT_FOUND)
                    {
                        galtisOutput(outArgs, status, "%d", -1);
                        return CMD_OK;
                    }
                }
            }
        }
    }
    else
    {
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;

            return wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet(inArgs,inFields,numFields,outArgs);
        }

        /* no more virtual routers in the current LPM DB,
           find next LPM DB that has virtual router */
        while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
        {
            lpmDBId = lpmDbPtr->lpmDbId;

            /* get first virtual router */
            tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
            while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
            {
                vrIdValue = vrEntryPtr->vrId;

                return wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGet(inArgs,inFields,numFields,outArgs);
            }
        }
    }

    /* there are no more LPM DB with virtual routers */
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntrySet
*
* DESCRIPTION:
*       This function adds virtual router (shared or non-shared).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               status;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_BOOL                                 isSharedVr;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo = {0};

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId                                             = (GT_U32)inFields[0];
    vrId                                                = (GT_U32)inFields[1];
    isSharedVr                                          = (GT_BOOL)inFields[2];
    vrConfigInfo.supportIpv4Uc                          = (GT_BOOL)inFields[3];
    vrConfigInfo.supportIpv4Mc                          = (GT_BOOL)inFields[4];

    vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeType                   = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
    vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths                  = (GT_U32)inFields[6];
    vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex         = (GT_U32)inFields[7];
    vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable            = (GT_BOOL)inFields[8];
    vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable    = (GT_BOOL)inFields[9];
    vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel       = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[10];

    vrConfigInfo.defIpv4McRouteLttEntry.routeType       = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[11];
    vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths                           = (GT_U32)inFields[12];
    vrConfigInfo.defIpv4McRouteLttEntry.routeEntryBaseIndex                  = (GT_U32)inFields[13];
    vrConfigInfo.defIpv4McRouteLttEntry.ucRPFCheckEnable                     = (GT_BOOL)inFields[14];
    vrConfigInfo.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable             = (GT_BOOL)inFields[15];
    vrConfigInfo.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel                = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[16];

    vrConfigInfo.supportIpv6Uc                                               = (GT_BOOL)inFields[17];
    vrConfigInfo.supportIpv6Mc                                               = (GT_BOOL)inFields[18];
    vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeType                   = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[19];
    vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths                  = (GT_U32)inFields[20];
    vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex         = (GT_U32)inFields[21];
    vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.ucRPFCheckEnable            = (GT_BOOL)inFields[22];
    vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable    = (GT_BOOL)inFields[23];
    vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel       = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[24];

    vrConfigInfo.defIpv6McRouteLttEntry.routeType       = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[25];
    vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths                           = (GT_U32)inFields[26];
    vrConfigInfo.defIpv6McRouteLttEntry.routeEntryBaseIndex                  = (GT_U32)inFields[27];
    vrConfigInfo.defIpv6McRouteLttEntry.ucRPFCheckEnable                     = (GT_BOOL)inFields[28];
    vrConfigInfo.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable             = (GT_BOOL)inFields[29];
    vrConfigInfo.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel                = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[30];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &vrConfigInfo.defIpv4McRouteLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &vrConfigInfo.defIpv6McRouteLttEntry);

    /* call cpss api function */
    if (isSharedVr == GT_TRUE)
    {
        status = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId,vrId,&vrConfigInfo);

        /* if adding the shared VR succeeded, then save the shared VR
           id for later use by Get functions */
        if (status == GT_OK)
        {
            sharedVrExists  = GT_TRUE;
            sharedVrId      = vrId;
        }
    }
    else
    {
        status =  cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfigInfo);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet
*
* DESCRIPTION:
*       This function get single virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               status;
    GT_BOOL                                 isSharedVr;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfigInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Call cpss api function. Note that the virtual router must exists as it
       is in the LPM DB virtual routers list */
    status =  cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfigInfo);

    if(status != GT_OK)
    {
        galtisOutput(outArgs, status, "%d", -1);
        return CMD_OK;
    }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv4McRouteLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry);
    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &vrConfigInfo.defIpv6McRouteLttEntry);

    if ((sharedVrExists == GT_TRUE) && (vrIdValue == sharedVrId))
        isSharedVr = GT_TRUE;
    else
        isSharedVr = GT_FALSE;

    inFields[0] = lpmDBId;
    inFields[1] = vrIdValue;
    inFields[2] = isSharedVr;
    inFields[3] = vrConfigInfo.supportIpv4Uc;
    inFields[4] = vrConfigInfo.supportIpv4Mc;

    /* if Ipv4 UC is not supported zero out defUcNextHopInfo to get deteministic results */
    if (vrConfigInfo.supportIpv4Uc == GT_FALSE)
        cmdOsMemSet(&vrConfigInfo.defIpv4UcNextHopInfo,0,sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    /* if Ipv4 MC is not supported zero out defMcRouteLttEntry to get deteministic results */
    if (vrConfigInfo.supportIpv4Mc == GT_FALSE)
        cmdOsMemSet(&vrConfigInfo.defIpv4McRouteLttEntry,0,sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));

    inFields[5] = vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeType;
    inFields[6] = vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths;
    inFields[7] = vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[8] = vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[9] = vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[10]= vrConfigInfo.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

    inFields[11] = vrConfigInfo.defIpv4McRouteLttEntry.routeType;
    inFields[12] = vrConfigInfo.defIpv4McRouteLttEntry.numOfPaths;
    inFields[13] = vrConfigInfo.defIpv4McRouteLttEntry.routeEntryBaseIndex;
    inFields[14] = vrConfigInfo.defIpv4McRouteLttEntry.ucRPFCheckEnable;
    inFields[15] = vrConfigInfo.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[16] = vrConfigInfo.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel;

    inFields[17] = vrConfigInfo.supportIpv6Uc;
    inFields[18] = vrConfigInfo.supportIpv6Mc;

    /* if Ipv4 UC is not supported zero out defUcNextHopInfo to get deteministic results */
    if (vrConfigInfo.supportIpv6Uc == GT_FALSE)
        cmdOsMemSet(&vrConfigInfo.defIpv6UcNextHopInfo,0,sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    /* if Ipv4 MC is not supported zero out defMcRouteLttEntry to get deteministic results */
    if (vrConfigInfo.supportIpv6Mc == GT_FALSE)
        cmdOsMemSet(&vrConfigInfo.defIpv6McRouteLttEntry,0,sizeof(CPSS_DXCH_IP_LTT_ENTRY_STC));
    inFields[19] = vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeType;
    inFields[20] = vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths;
    inFields[21] = vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[22] = vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[23] = vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[24]= vrConfigInfo.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

    inFields[25] = vrConfigInfo.defIpv6McRouteLttEntry.routeType;
    inFields[26] = vrConfigInfo.defIpv6McRouteLttEntry.numOfPaths;
    inFields[27] = vrConfigInfo.defIpv6McRouteLttEntry.routeEntryBaseIndex;
    inFields[28] = vrConfigInfo.defIpv6McRouteLttEntry.ucRPFCheckEnable;
    inFields[29] = vrConfigInfo.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[30] = vrConfigInfo.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
        inFields[0], inFields[1], inFields[2], inFields[3],inFields[4],
        inFields[5], inFields[6],inFields[7], inFields[8], inFields[9],
        inFields[10],inFields[11], inFields[12], inFields[13],inFields[14],
        inFields[15],inFields[16], inFields[17],inFields[18],inFields[19],
        inFields[20],inFields[21], inFields[22], inFields[23],inFields[24],
        inFields[25],inFields[26], inFields[27],inFields[28], inFields[29],
        inFields[30]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGetFirst
*
* DESCRIPTION:
*       This function get first virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         status = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDbIterator   = 0;    /*reset on first*/
    lpmDBId         = 0;    /*reset on first*/
    lpmDbPtr        = NULL;
    vrIterator      = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/
    vrEntryPtr      = NULL;

    /* vrEntryRam is used for TCAM backward compatible code */
    vrEntryRam.valid=GT_TRUE;

    /* get first LPM DB that has virtual router */
    while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
    {
        lpmDBId = lpmDbPtr->lpmDbId;

        /* get first virtual router */
        if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
        {
            ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
            for (vrId = 0; vrId < ramShadow->vrfTblSize; vrId++)
            {
                status = cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrId,&vrConfig);
                if (status == GT_OK)
                {
                    vrIdValue = vrId;
                    vrEntryRam.vrId = vrId;
                    vrEntryPtr = &vrEntryRam;
                    return wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet(inArgs,inFields,numFields,outArgs);
                }
                if(status != GT_OK)
                {
                    if (status != GT_NOT_FOUND)
                    {
                        galtisOutput(outArgs, status, "%d", -1);
                        return CMD_OK;
                    }
                }
            }
        }
        else
        {
            tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
            while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
            {
                vrIdValue = vrEntryPtr->vrId;

                return wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet(inArgs,inFields,numFields,outArgs);
            }
        }
    }

    /* there are no LPM DB with virtual routers */
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGetNext
*
* DESCRIPTION:
*       This function get next virtual router.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                         status = GT_OK;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;
    GT_U32 vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next virtual router within the current LPM DB */
    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
        for (vrId = vrIdValue + 1; vrId < ramShadow->vrfTblSize; vrId++)
        {
            status = cpssDxChIpLpmVirtualRouterGet(lpmDbPtr->lpmDbId,vrId,&vrConfig);
            if (status == GT_OK)
            {
                vrIdValue = vrId;
                vrEntryRam.vrId = vrId;
                vrEntryPtr = &vrEntryRam;
                return wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet(inArgs,inFields,numFields,outArgs);
            }
            else
            {
                if(status != GT_NOT_FOUND)
                {
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
        }

        while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
        {
            lpmDBId = lpmDbPtr->lpmDbId;

            /* get first virtual router */
            ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
            for (vrId = 0; vrId < ramShadow->vrfTblSize; vrId++)
            {
                status = cpssDxChIpLpmVirtualRouterGet(lpmDbPtr->lpmDbId,vrId,&vrConfig);
                if (status == GT_OK)
                {
                    vrIdValue = vrId;
                    vrEntryRam.vrId = vrId;
                    vrEntryPtr = &vrEntryRam;
                    return wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet(inArgs,inFields,numFields,outArgs);
                }
                else
                {
                    if(status != GT_NOT_FOUND)
                    {
                        galtisOutput(outArgs, status, "%d", -1);
                        return CMD_OK;
                    }
                }
            }
        }
    }
    else
    {
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;

            return wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet(inArgs,inFields,numFields,outArgs);
        }

        /* no more virtual routers in the current LPM DB,
           find next LPM DB that has virtual router */
        while ((lpmDbPtr = prvCpssSlGetNext(prvCpssDxChIplpmDbSL,&lpmDbIterator)) != NULL)
        {
            lpmDBId = lpmDbPtr->lpmDbId;

            /* get first virtual router */
            tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
            while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
            {
                vrIdValue = vrEntryPtr->vrId;

                return wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGet(inArgs,inFields,numFields,outArgs);
            }
        }
    }

    /* there are no more LPM DB with virtual routers */
    galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.
*                             future support only , currently only vrId = 0 is
*                             supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND - if the LPM DB id is not found
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmVirtualRouterDel
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inArgs[1];

    /* call cpss api function */
    status = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);

    /* if deleting the VR succeeded and the VR is the shared VR, then delete the
       shared VR id for later use by Get functions */
    if ((status == GT_OK) && (sharedVrExists == GT_TRUE) && (sharedVrId == vrId))
    {
        sharedVrExists  = GT_FALSE;
        sharedVrId      = 0;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - If failed to allocate TCAM memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAdd_PCL
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_BOOL                                  override;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];
    defUcNextHop.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inArgs[4];
    defUcNextHop.pclIpUcAction.mirror.cpuCode =
                                            (CPSS_NET_RX_CPU_CODE_ENT)inArgs[5];
    defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                            (GT_BOOL)inArgs[6];
    defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount =
                                                            (GT_BOOL)inArgs[7];
    defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex =
                                                            (GT_U32)inArgs[8];
    defUcNextHop.pclIpUcAction.egressPolicy = (GT_BOOL)inArgs[9];

    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        defUcNextHop.pclIpUcAction.qos.egress.modifyDscp =
            (inFields[10] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? GT_TRUE : GT_FALSE;
        defUcNextHop.pclIpUcAction.qos.egress.modifyUp =
            (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
        defUcNextHop.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[12];
        defUcNextHop.pclIpUcAction.qos.egress.up = (GT_U8)inFields[13];
    }
    else
    {
        defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
        defUcNextHop.pclIpUcAction.qos.ingress.modifyUp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
        defUcNextHop.pclIpUcAction.qos.ingress.profileIndex =
            (GT_U32)inFields[14];
        defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex =
            (GT_BOOL)inFields[15];
        defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence =
            (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[16];
    }

    defUcNextHop.pclIpUcAction.redirect.redirectCmd =
                              (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inArgs[17];

    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inArgs[18];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    hwDevNum = (GT_U8)inArgs[19];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    portNum = (GT_U8)inArgs[20];

        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                             defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                        (GT_TRUNK_ID)inArgs[21];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                            (GT_U16)inArgs[22];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                            (GT_U16)inArgs[23];

        defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                            (GT_BOOL)inArgs[24];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                            (GT_BOOL)inArgs[25];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                            (GT_U32)inArgs[26];
    }
    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex =
                                                            (GT_U32)inArgs[27];
    }

    defUcNextHop.pclIpUcAction.policer.policerEnable = (GT_BOOL)inArgs[28];
    defUcNextHop.pclIpUcAction.policer.policerId = (GT_U32)inArgs[29];


    switch (defUcNextHop.pclIpUcAction.egressPolicy)
    {
        case GT_FALSE:
            defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inArgs[31];
            defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inArgs[32];
            defUcNextHop.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inArgs[33];
            defUcNextHop.pclIpUcAction.vlan.ingress.precedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[34];
            break;

        default:
        case GT_TRUE:
            defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd =
                ((inArgs[31] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                 || (inArgs[31] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
            defUcNextHop.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inArgs[33];
            break;
    }

    defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inArgs[35];
    defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inArgs[36];
    defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inArgs[37];
    defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inArgs[38];
    defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                            (GT_BOOL)inArgs[39];
    override = (GT_BOOL)inArgs[40];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************************** Table function **********************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAddPCLEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_BOOL                                  override;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inFields[0];
    vrId = (GT_U32)inFields[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inFields[2]);
    prefixLen = (GT_U32)inFields[3];
    defUcNextHop.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[4];
    defUcNextHop.pclIpUcAction.mirror.cpuCode =
                                            (CPSS_NET_RX_CPU_CODE_ENT)inFields[5];
    defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                            (GT_BOOL)inFields[6];
    defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount =
                                                            (GT_BOOL)inFields[7];
    defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex =
                                                            (GT_U32)inFields[8];
    defUcNextHop.pclIpUcAction.egressPolicy = (GT_BOOL)inFields[9];

    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        defUcNextHop.pclIpUcAction.qos.egress.modifyDscp =
            (inFields[10] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? GT_TRUE : GT_FALSE;
        defUcNextHop.pclIpUcAction.qos.egress.modifyUp =
            (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
        defUcNextHop.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[12];
        defUcNextHop.pclIpUcAction.qos.egress.up = (GT_U8)inFields[13];
    }
    else
    {
        defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
        defUcNextHop.pclIpUcAction.qos.ingress.modifyUp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
        defUcNextHop.pclIpUcAction.qos.ingress.profileIndex =
            (GT_U32)inFields[14];
        defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex =
            (GT_BOOL)inFields[15];
        defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence =
            (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[16];
    }

    defUcNextHop.pclIpUcAction.redirect.redirectCmd =
                              (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[17];

    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inFields[18];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    hwDevNum = (GT_U8)inFields[19];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    portNum = (GT_U8)inFields[20];

        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                             defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                        (GT_TRUNK_ID)inFields[21];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                            (GT_U16)inFields[22];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                            (GT_U16)inFields[23];

        defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                            (GT_BOOL)inFields[24];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                            (GT_BOOL)inFields[25];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                            (GT_U32)inFields[26];
    }
    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex =
                                                            (GT_U32)inFields[27];
    }

    defUcNextHop.pclIpUcAction.policer.policerEnable = (GT_BOOL)inFields[28];
    defUcNextHop.pclIpUcAction.policer.policerId = (GT_U32)inFields[29];

    switch (defUcNextHop.pclIpUcAction.egressPolicy)
    {
        case GT_FALSE:
            defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[30];
            defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inFields[31];
            defUcNextHop.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inFields[32];
            defUcNextHop.pclIpUcAction.vlan.ingress.precedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
            break;

        default:
        case GT_TRUE:
            defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd =
                ((inFields[30] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                 || (inFields[30] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
            defUcNextHop.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inFields[32];
            break;
    }

    defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[34];
    defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inFields[35];
    defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inFields[36];
    defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[37];
    defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                            (GT_BOOL)inFields[38];
    override = (GT_BOOL)inFields[39];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - If failed to allocate TCAM memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAdd_LTT
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_BOOL                                  override;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];
    defUcNextHop.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[4];
    defUcNextHop.ipLttEntry.numOfPaths = (GT_U32)inArgs[5];
    defUcNextHop.ipLttEntry.routeEntryBaseIndex = (GT_U32)inArgs[6];
    defUcNextHop.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inArgs[7];
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inArgs[8];
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[9];
    override = (GT_BOOL)inArgs[16];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defUcNextHop.ipLttEntry);

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************************** Table function **********************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAddLTT_EntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_BOOL                                  override;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inFields[0];
    vrId = (GT_U32)inFields[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inFields[2]);
    prefixLen = (GT_U32)inFields[3];
    defUcNextHop.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[4];
    defUcNextHop.ipLttEntry.numOfPaths = (GT_U32)inFields[5];
    defUcNextHop.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[6];
    defUcNextHop.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[7];
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[8];
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[9];
    override = (GT_BOOL)inFields[16];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defUcNextHop.ipLttEntry);

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixesFlush
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;


    lpmDbIterator   = 0;    /*reset on first*/
    lpmDBId         = 0;    /*reset on first*/
    lpmDbPtr        = NULL;
    vrIterator      = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/
    vrEntryPtr      = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&lpmDbIterator);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
        for (vrIdValue = 0; vrIdValue < ramShadow->vrfTblSize; vrIdValue++)
        {
            status = cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfig);
            if (status == GT_OK)
            {
                status =  cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId, vrIdValue);
                if (status != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }

            }
            else
            {
                if (status != GT_NOT_FOUND)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
        }
    }
    else
    {
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;
            /* call cpss api function */
            status =  cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId, vrIdValue);

            /* pack output arguments to galtis string */
            galtisOutput(outArgs, status, "");
        }
    }

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv4 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixesFlushVr
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    lpmDBId         = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrIdValue = (GT_U32)inArgs[1];

    status =  cpssDxChIpLpmIpv4UcPrefixesFlush(lpmDBId, vrIdValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If found, this is the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixSearch_PCL
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   rowIndex;
    GT_U32                                   columnIndex;
    GT_HW_DEV_NUM                            hwDev;   /* Dummy for converting. */
    GT_PORT_NUM                              port;  /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &defUcNextHop,&rowIndex,&columnIndex);

    inFields[0] = defUcNextHop.pclIpUcAction.pktCmd;
    inFields[1] = defUcNextHop.pclIpUcAction.mirror.cpuCode;
    inFields[2] = defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
    inFields[3] = defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount;
    inFields[4] = defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex;
    inFields[5] = defUcNextHop.pclIpUcAction.egressPolicy;
    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        inFields[6] =
            (defUcNextHop.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[7] =
                (defUcNextHop.pclIpUcAction.qos.egress.modifyUp ==
                 CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                    ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                    : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[8] = defUcNextHop.pclIpUcAction.qos.egress.dscp;
        inFields[9] = defUcNextHop.pclIpUcAction.qos.egress.up;
    }
    else
    {
        inFields[6] = defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp;
        inFields[7] = defUcNextHop.pclIpUcAction.qos.ingress.modifyUp;
        inFields[10] = defUcNextHop.pclIpUcAction.qos.ingress.profileIndex;
        inFields[11] = defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex;
        inFields[12] = defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence;
    }
    inFields[13] = defUcNextHop.pclIpUcAction.redirect.redirectCmd;
    inFields[14] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                          type;
    hwDev = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                devPort.hwDevNum;
    port = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                               devPort.portNum;
    CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
    inFields[15] = hwDev;
    inFields[16] = port;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        trunkId);
    inFields[17] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        trunkId;
    inFields[18] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                          vidx;
    inFields[19] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        vlanId;
    inFields[20] = defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo;
    inFields[21] = defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart;
    inFields[22] = defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr;
    inFields[23] = defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex;
    inFields[24] = defUcNextHop.pclIpUcAction.policer.policerEnable;
    inFields[25] = defUcNextHop.pclIpUcAction.policer.policerId;

    inFields[26] = GT_FALSE; /*defUcNextHop.pclIpUcAction.vlan.egressTaggedModify;*/
    if (defUcNextHop.pclIpUcAction.egressPolicy == GT_FALSE)
    {
        inFields[27] = defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan;
        inFields[28] = defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan;
        inFields[29] = defUcNextHop.pclIpUcAction.vlan.ingress.vlanId;
        inFields[30] = defUcNextHop.pclIpUcAction.vlan.ingress.precedence;
    }
    else
    {
        inFields[28] =
            (defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd ==
             CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
        inFields[30] = defUcNextHop.pclIpUcAction.vlan.egress.vlanId;
    }
    inFields[31] = defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute;
    inFields[32] = defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex;
    inFields[33] = defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL;
    inFields[34] = defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck;
    inFields[35] = defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck;


    /* pack and output fields */
    galtisOutput(outArgs, status,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If found, this is the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixSearch_LTT
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   rowIndex;
    GT_U32                                   columnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &defUcNextHop,&rowIndex,&columnIndex);

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &defUcNextHop.ipLttEntry);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d",
                 defUcNextHop.ipLttEntry.routeType,
                 defUcNextHop.ipLttEntry.numOfPaths,
                 defUcNextHop.ipLttEntry.routeEntryBaseIndex,
                 defUcNextHop.ipLttEntry.ucRPFCheckEnable,
                 defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable,
                 defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel);

    return CMD_OK;
}



/***********Table: cpssDxChIpLpmIpv4UcPrefix***************/
/*set*/
static  GT_U32                                  gIpv4PrefixArrayLen;
static  CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*    gIpv4PrefixArray;

/*get*/
static  GT_IPADDR                               gIpAddr;
static  GT_U32                                  gPrefixLen;
static  GT_U32                                  gIndex;

static  GT_BOOL                                 gBulkGather = GT_FALSE;

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssDxChIpLpmBulkGatherFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIpv4PrefixArrayLen = 0; /*Reset on First*/

    /* map input arguments to locals */
    if(gIpv4PrefixArray == NULL)
    {
        gIpv4PrefixArray = (CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*)cmdOsMalloc
                                (sizeof(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC));
    }
    if(gIpv4PrefixArray == NULL)
    {
      return GT_NO_RESOURCE;
    }

    cmdOsMemSet(
        gIpv4PrefixArray,
        0, (sizeof(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC)));

    ind = (GT_U32)inFields[0];

    if(ind > gIpv4PrefixArrayLen)
    {
      return GT_BAD_VALUE;
    }

    if(!inArgs[1])/*for PCL*/
    {
        gIpv4PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpAddr(&(gIpv4PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpv4PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[5];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.cpuCode =
                                                (CPSS_NET_RX_CPU_CODE_ENT)inFields[6];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                                (GT_BOOL)inFields[7];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount =
                                                                (GT_BOOL)inFields[8];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex =
                                                                (GT_U32)inFields[9];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy = (GT_BOOL)inFields[10];
        if(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyDscp =
                (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? GT_TRUE : GT_FALSE;
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyUp =
                (inFields[12] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[13];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.up = (GT_U8)inFields[14];
        }
        else
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyUp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileIndex = (GT_U32)inFields[15];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex = (GT_BOOL)inFields[16];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence =
                            (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[17];
        }
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd =
                                  (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[18];

        if(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd ==
                                        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                                (CPSS_INTERFACE_TYPE_ENT)inFields[19];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        hwDevNum = (GT_U8)inFields[20];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        portNum = (GT_U8)inFields[21];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                                 gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                            (GT_TRUNK_ID)inFields[22];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                                (GT_U16)inFields[23];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                                (GT_U16)inFields[24];

            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                                (GT_BOOL)inFields[25];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                                (GT_BOOL)inFields[26];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                                (GT_U32)inFields[27];
        }
        if(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd
                                    == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex =
                                                                (GT_U32)inFields[28];
        }

        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerEnable = (GT_BOOL)inFields[29];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerId = (GT_U32)inFields[30];
        /*gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egressTaggedModify = (GT_BOOL)inFields[31];*/
        switch (gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy)
        {
            case GT_FALSE:
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[32];
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inFields[33];
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inFields[34];
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.precedence =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[35];
                break;

            default:
            case GT_TRUE:
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd =
                    ((inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                     || (inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inFields[34];
                break;
        }

        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[36];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inFields[37];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inFields[38];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[39];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                                (GT_BOOL)inFields[40];

        gIpv4PrefixArray[ind].override = (GT_BOOL)inFields[42];
        gIpv4PrefixArray[ind].returnStatus = (GT_STATUS)inFields[43];
    }



    else/*for LTT*/
    {
        gIpv4PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpAddr(&(gIpv4PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpv4PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.routeType =
                                  (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.numOfPaths =
                                                                (GT_U32)inFields[6];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.routeEntryBaseIndex =
                                                                (GT_U32)inFields[7];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.ucRPFCheckEnable =
                                                               (GT_BOOL)inFields[8];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable =
                                                               (GT_BOOL)inFields[9];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel =
                                            (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[10];

        gIpv4PrefixArray[ind].override = (GT_BOOL)inFields[12];
        gIpv4PrefixArray[ind].returnStatus = (GT_STATUS)inFields[13];

        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE,
                                                       &gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry);
    }

    gIpv4PrefixArrayLen++;

    return GT_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssDxChIpLpmBulkGatherNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIpv4PrefixArray = (CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*)cmdOsRealloc
                                (gIpv4PrefixArray, (gIpv4PrefixArrayLen + 1) *
                                 sizeof(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC));
    if (gIpv4PrefixArray == NULL)
    {
      return GT_NO_RESOURCE;
    }

    cmdOsMemSet(
        &(gIpv4PrefixArray[gIpv4PrefixArrayLen]),
        0, (sizeof(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC)));

    ind = (GT_U32)inFields[0];

    if(ind > gIpv4PrefixArrayLen)
    {
      return GT_BAD_VALUE;
    }

    if(!inArgs[1])/*for PCL*/
    {
        gIpv4PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpAddr(&(gIpv4PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpv4PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[5];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.cpuCode =
                                                (CPSS_NET_RX_CPU_CODE_ENT)inFields[6];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                                (GT_BOOL)inFields[7];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount =
                                                                (GT_BOOL)inFields[8];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex =
                                                                (GT_U32)inFields[9];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy = (GT_BOOL)inFields[10];
        if(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyDscp =
                (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? GT_TRUE : GT_FALSE;
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyUp =
                (inFields[12] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[13];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.up = (GT_U8)inFields[14];
        }
        else
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyUp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileIndex = (GT_U32)inFields[15];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex = (GT_BOOL)inFields[16];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[17];
        }

        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd =
                                  (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[18];

        if(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd ==
                                        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                                (CPSS_INTERFACE_TYPE_ENT)inFields[19];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        hwDevNum = (GT_U8)inFields[20];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        portNum = (GT_U8)inFields[21];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                                 gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                            (GT_TRUNK_ID)inFields[22];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                                (GT_U16)inFields[23];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                                (GT_U16)inFields[24];

            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                                (GT_BOOL)inFields[25];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                                (GT_BOOL)inFields[26];
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                                (GT_U32)inFields[27];
        }
        if(gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd
                                    == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
        {
            gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex =
                                                                (GT_U32)inFields[28];
        }

        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerEnable = (GT_BOOL)inFields[29];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerId = (GT_U32)inFields[30];
        /*gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egressTaggedModify = (GT_BOOL)inFields[31];*/
        switch (gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy)
        {
            case GT_FALSE:
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[32];
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inFields[33];
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inFields[34];
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.precedence =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[35];
                break;

            default:
            case GT_TRUE:
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd =
                    ((inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                     || (inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
                gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inFields[34];
                break;
        }

        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[36];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inFields[37];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inFields[38];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[39];
        gIpv4PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                                (GT_BOOL)inFields[40];

        gIpv4PrefixArray[ind].override = (GT_BOOL)inFields[42];
        gIpv4PrefixArray[ind].returnStatus = (GT_STATUS)inFields[43];
    }



    else/*for LTT*/
    {
        gIpv4PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpAddr(&(gIpv4PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpv4PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.routeType =
                                  (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.numOfPaths =
                                                                (GT_U32)inFields[6];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.routeEntryBaseIndex =
                                                                (GT_U32)inFields[7];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.ucRPFCheckEnable =
                                                               (GT_BOOL)inFields[8];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable =
                                                               (GT_BOOL)inFields[9];
        gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel =
                                            (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[10];

        gIpv4PrefixArray[ind].override = (GT_BOOL)inFields[12];
        gIpv4PrefixArray[ind].returnStatus = (GT_STATUS)inFields[13];

        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE,
                                                       &gIpv4PrefixArray[ind].nextHopInfo.ipLttEntry);
    }

    gIpv4PrefixArrayLen++;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{/*just a dummy for galtis*/
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixDel
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1 = GT_OK;
    GT_STATUS   status2 = GT_OK;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gBulkGather)
        {
            status1 = wrCpssDxChIpLpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step delete*/
        {
            vrId = (GT_U32)inFields[2];
            galtisIpAddr(&ipAddr, (GT_U8*)inFields[3]);
            prefixLen = (GT_U32)inFields[4];

            /* call cpss api function */
            status1 =  cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId,
                                                   ipAddr, prefixLen);
        }
        break;

    case BULK_STARTING_ENTRY:
        gBulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssDxChIpLpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssDxChIpLpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);

        status1 =  cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, gIpv4PrefixArrayLen,
                                                             gIpv4PrefixArray);
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        cmdOsFree(gIpv4PrefixArray);
        gIpv4PrefixArray = NULL;
        gIpv4PrefixArrayLen = 0;

        gBulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArray    - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAdd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1 = GT_OK;
    GT_STATUS   status2 = GT_OK;

    GT_U32                                   lpmDBId;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gBulkGather)
        {
            status1 = wrCpssDxChIpLpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step SET*/
        {

            status2 = wrCpssDxChIpLpmBulkGatherFirst(inArgs, inFields,
                                                    numFields, outArgs);
            /* call cpss api function */
            status1 =  cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId,
                                             gIpv4PrefixArray[0].vrId,
                                             gIpv4PrefixArray[0].ipAddr,
                                             gIpv4PrefixArray[0].prefixLen,
                                             &(gIpv4PrefixArray[0].nextHopInfo),
                                             gIpv4PrefixArray[0].override);


            if(status2 != GT_OK)
            {
                status1 = status2;
            }

            cmdOsFree(gIpv4PrefixArray);
            gIpv4PrefixArray = NULL;
            gIpv4PrefixArrayLen = 0;
        }
        break;

    case BULK_STARTING_ENTRY:
        gBulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssDxChIpLpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssDxChIpLpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);

        status1 =  cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, gIpv4PrefixArrayLen,
                                                             gIpv4PrefixArray);
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        cmdOsFree(gIpv4PrefixArray);
        gIpv4PrefixArray = NULL;
        gIpv4PrefixArrayLen = 0;

        gBulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*                      future support only , currently only vrId = 0 is
*                      supported.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, this is the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixGetNext_GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                                    lpmDBId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT          shadowType;
    GT_U32                                    rowIndex;
    GT_U32                                    columnIndex;
    GT_HW_DEV_NUM                             hwDev; /* Dummy for converting. */
    GT_PORT_NUM                               port;  /* Dummy for converting. */

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    gIndex = 0;
    vrId = 0;/*for first entry 0 is entered*/
    gIpAddr.u32Ip = 0; /*for first entry 0 is entered*/
    gPrefixLen = 0; /*for first entry 0 is entered*/

    isDefaultVrfIdEntry = GT_FALSE;

    result = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* allocate the virtual routers skip list*/
    switch (shadowType)
    {
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E:
        maxNumOfVr = DXCH_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E:
        maxNumOfVr = DXCH2_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E:
        maxNumOfVr = DXCH3_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E:
    case CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        maxNumOfVr = DXCH_XCAT_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E:
        maxNumOfVr = DXCH_LION3_MAX_VR_ID_CNS;
        break;
    default:
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    for( ; vrId < maxNumOfVr ; vrId++)
    {
        /* call cpss api function */
        result = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, gIpAddr,
                                              gPrefixLen, &nextHopInfo,
                                              &rowIndex, &columnIndex);

        /* result is GT_NOT_FOUND if the virtual router doesn't exist,
           or GT_NOT_INITIALIZED if the virtual router is not valid */
         if ((result == GT_NOT_FOUND)||
             (result == GT_NOT_INITIALIZED))
            continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E)/*Refresh PCL TAB*/
        || (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[3] = (GT_U32)gIpAddr.u32Ip;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[4] = gPrefixLen;
        inFields[5] = nextHopInfo.pclIpUcAction.pktCmd;
        inFields[6] = nextHopInfo.pclIpUcAction.mirror.cpuCode;
        inFields[7] = nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
        inFields[8] = nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount;
        inFields[9] = nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex;
        inFields[10] = nextHopInfo.pclIpUcAction.egressPolicy;
        if(nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            inFields[11] =
                (nextHopInfo.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                    ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                    : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[12] =
                    (nextHopInfo.pclIpUcAction.qos.egress.modifyUp ==
                     CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                        ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                        : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[13] = nextHopInfo.pclIpUcAction.qos.egress.dscp;
            inFields[14] = nextHopInfo.pclIpUcAction.qos.egress.up;
        }
        else
        {
            inFields[11] = nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp;
            inFields[12] = nextHopInfo.pclIpUcAction.qos.ingress.modifyUp;
            inFields[15] = nextHopInfo.pclIpUcAction.qos.ingress.profileIndex;
            inFields[16] = nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex;
            inFields[17] = nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence;
        }
        inFields[18] = nextHopInfo.pclIpUcAction.redirect.redirectCmd;
        inFields[19] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              type;
        hwDev = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                    devPort.hwDevNum;
        port = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                   devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[20] = hwDev;
        inFields[21] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[22] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                         trunkId;
        inFields[23] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              vidx;
        inFields[24] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            vlanId;
        inFields[25] = nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo;
        inFields[26] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart;
        inFields[27] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr;
        inFields[28] = nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex;
        inFields[29] = nextHopInfo.pclIpUcAction.policer.policerEnable;
        inFields[30] = nextHopInfo.pclIpUcAction.policer.policerId;
        inFields[31] = GT_FALSE; /*nextHopInfo.pclIpUcAction.vlan.egressTaggedModify;*/
        if (nextHopInfo.pclIpUcAction.egressPolicy == GT_FALSE)
        {
            inFields[32] = nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan;
            inFields[33] = nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.ingress.vlanId;
            inFields[35] = nextHopInfo.pclIpUcAction.vlan.ingress.precedence;
        }
        else
        {
            inFields[32] =
                (nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd ==
                 CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                    ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                    : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.egress.vlanId;
        }
        inFields[36] = nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute;
        inFields[37] = nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex;
        inFields[38] = nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL;
        inFields[39] = nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck;
        inFields[40] = nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck;

        /* inFields[42] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[41] = 0;
        inFields[42] = 0;
        inFields[43] = result;

        /* pack and output table fields for TAB 0 (PCL)*/
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                    %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10], inFields[11],
                    inFields[12], inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22], inFields[23],
                    inFields[24], inFields[25], inFields[26], inFields[27],
                    inFields[28], inFields[29], inFields[30], inFields[31],
                    inFields[32], inFields[33], inFields[34], inFields[35],
                    inFields[36], inFields[37], inFields[38], inFields[39],
                    inFields[40],inFields[41],inFields[42],inFields[43]);

        galtisOutput(outArgs, GT_OK, "%d%f", 0);
    }

    else/*Refresh LTT TAB*/
    {
        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &nextHopInfo.ipLttEntry);

        /* pack and output table fields for TAB 1 (LTT)*/
        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[3] = (GT_U32)gIpAddr.u32Ip;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[4] = gPrefixLen;
        inFields[5] = nextHopInfo.ipLttEntry.routeType;
        inFields[6] = nextHopInfo.ipLttEntry.numOfPaths;
        inFields[7] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
        inFields[8] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
        inFields[9] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
        inFields[10] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

        /* inFields[12] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[11] = 0;
        inFields[12] = 0;
        inFields[13] = result;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10],
                    inFields[11], inFields[12], inFields[13]);

        galtisOutput(outArgs, GT_OK, "%d%f", 1);
    }

    gIndex++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*                      future support only , currently only vrId = 0 is
*                      supported.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, this is the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixGetNext_GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                                    lpmDBId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT          shadowType;
    GT_U32                                    rowIndex;
    GT_U32                                    columnIndex;
    GT_HW_DEV_NUM                             hwDev; /* Dummy for converting. */
    GT_PORT_NUM                               port;  /* Dummy for converting. */

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(vrId != lastVrfIdTreated)
    {
         gIpAddr.u32Ip = 0; /*for first entry 0 is entered*/
         gPrefixLen = 0; /*for first entry 0 is entered*/

        for( ; vrId < maxNumOfVr ; vrId++)
        {
            /* call cpss api function */
            result = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, gIpAddr,
                                              gPrefixLen, &nextHopInfo,
                                              &rowIndex, &columnIndex);

            /* result is GT_NOT_FOUND if the virtual router doesn't exist
               or GT_NOT_INITIALIZED if the virtual router is not valid */
            if ((result == GT_NOT_FOUND)||
                (result == GT_NOT_INITIALIZED))
                continue;
            else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    result = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in
           wrCpssDxChIpLpmIpv4UcPrefixGetNext_GetFirst or in previuos call
           to this wrapper   */
        if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E)/*Refresh PCL TAB*/
            || (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
        {
            inFields[0] = gIndex;
            inFields[1] = REGULAR_ENTRY;
            inFields[2] = vrId;
            SWITCH_BYTES(gIpAddr.arIP, 4);
            inFields[3] = (GT_U32)gIpAddr.u32Ip;
            SWITCH_BYTES(gIpAddr.arIP, 4);
            inFields[4] = gPrefixLen;
            inFields[5] = nextHopInfo.pclIpUcAction.pktCmd;
            inFields[6] = nextHopInfo.pclIpUcAction.mirror.cpuCode;
            inFields[7] = nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
            inFields[8] = nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount;
            inFields[9] = nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex;
            inFields[10] = nextHopInfo.pclIpUcAction.egressPolicy;
            if(nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
            {
                inFields[11] =
                    (nextHopInfo.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                        ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                        : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                inFields[12] =
                        (nextHopInfo.pclIpUcAction.qos.egress.modifyUp ==
                         CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                            ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                            : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                inFields[13] = nextHopInfo.pclIpUcAction.qos.egress.dscp;
                inFields[14] = nextHopInfo.pclIpUcAction.qos.egress.up;
            }
            else
            {
                inFields[11] = nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp;
                inFields[12] = nextHopInfo.pclIpUcAction.qos.ingress.modifyUp;
                inFields[15] = nextHopInfo.pclIpUcAction.qos.ingress.profileIndex;
                inFields[16] = nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex;
                inFields[17] = nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence;
            }
            inFields[18] = nextHopInfo.pclIpUcAction.redirect.redirectCmd;
            inFields[19] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                  type;
            hwDev = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        devPort.hwDevNum;
            port = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                       devPort.portNum;
            CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
            inFields[20] = hwDev;
            inFields[21] = port;
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                trunkId);
            inFields[22] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                             trunkId;
            inFields[23] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                  vidx;
            inFields[24] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                vlanId;
            inFields[25] = nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo;
            inFields[26] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart;
            inFields[27] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr;
            inFields[28] = nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex;
            inFields[29] = nextHopInfo.pclIpUcAction.policer.policerEnable;
            inFields[30] = nextHopInfo.pclIpUcAction.policer.policerId;
            inFields[31] = GT_FALSE; /*nextHopInfo.pclIpUcAction.vlan.egressTaggedModify;*/
            if (nextHopInfo.pclIpUcAction.egressPolicy == GT_FALSE)
            {
                inFields[32] = nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan;
                inFields[33] = nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan;
                inFields[34] = nextHopInfo.pclIpUcAction.vlan.ingress.vlanId;
                inFields[35] = nextHopInfo.pclIpUcAction.vlan.ingress.precedence;
            }
            else
            {
                inFields[32] =
                    (nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd ==
                     CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                        ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                        : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
                inFields[34] = nextHopInfo.pclIpUcAction.vlan.egress.vlanId;
            }
            inFields[36] = nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute;
            inFields[37] = nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex;
            inFields[38] = nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL;
            inFields[39] = nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck;
            inFields[40] = nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck;

            /* inFields[42] is the override field. It has only meaning in the set, but
               we should display it also in the get, else the command wrapper will fail*/
            inFields[41] = 0;
            inFields[42] = 0;
            inFields[43] = result;

            /* pack and output table fields for TAB 0 (PCL)*/
            fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                        %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                        ,inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10], inFields[11],
                        inFields[12], inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18], inFields[19],
                        inFields[20], inFields[21], inFields[22], inFields[23],
                        inFields[24], inFields[25], inFields[26], inFields[27],
                        inFields[28], inFields[29], inFields[30], inFields[31],
                        inFields[32], inFields[33], inFields[34], inFields[35],
                        inFields[36], inFields[37], inFields[38], inFields[39],
                        inFields[40],inFields[41],inFields[42],inFields[43]);

            galtisOutput(outArgs, GT_OK, "%d%f", 0);
        }

        else/*Refresh LTT TAB*/
        {
            prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &nextHopInfo.ipLttEntry);

            /* pack and output table fields for TAB 1 (LTT)*/
            inFields[0] = gIndex;
            inFields[1] = REGULAR_ENTRY;
            inFields[2] = vrId;
            SWITCH_BYTES(gIpAddr.arIP, 4);
            inFields[3] = (GT_U32)gIpAddr.u32Ip;
            SWITCH_BYTES(gIpAddr.arIP, 4);
            inFields[4] = gPrefixLen;
            inFields[5] = nextHopInfo.ipLttEntry.routeType;
            inFields[6] = nextHopInfo.ipLttEntry.numOfPaths;
            inFields[7] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
            inFields[8] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
            inFields[9] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
            inFields[10] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

            /* inFields[12] is the override field. It has only meaning in the set, but
               we should display it also in the get, else the command wrapper will fail*/
            inFields[11] = 0;
            inFields[12] = 0;
            inFields[13] = result;

            /* pack and output table fields */
            fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                        ,inFields[0], inFields[1], inFields[2], inFields[3],
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10],
                        inFields[11], inFields[12], inFields[13]);

            galtisOutput(outArgs, GT_OK, "%d%f", 1);
        }

        gIndex++;
        isDefaultVrfIdEntry = GT_FALSE;

        return CMD_OK;

    }

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &gIpAddr,
                                              &gPrefixLen, &nextHopInfo,
                                              &rowIndex, &columnIndex);

     if (result == GT_NOT_FOUND)
     {
        vrId++;

        return wrCpssDxChIpLpmIpv4UcPrefixGetNext_GetNext(inArgs,inFields,numFields,outArgs);
    }


    if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E)/*Refresh PCL TAB*/
        || (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[3] = (GT_U32)gIpAddr.u32Ip;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[4] = gPrefixLen;
        inFields[5] = nextHopInfo.pclIpUcAction.pktCmd;
        inFields[6] = nextHopInfo.pclIpUcAction.mirror.cpuCode;
        inFields[7] = nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
        inFields[8] = nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount;
        inFields[9] = nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex;
        inFields[10] = nextHopInfo.pclIpUcAction.egressPolicy;
        if(nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            inFields[11] =
                (nextHopInfo.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                    ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                    : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[12] =
                    (nextHopInfo.pclIpUcAction.qos.egress.modifyUp ==
                     CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                        ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                        : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[13] = nextHopInfo.pclIpUcAction.qos.egress.dscp;
            inFields[14] = nextHopInfo.pclIpUcAction.qos.egress.up;
        }
        else
        {
            inFields[11] = nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp;
            inFields[12] = nextHopInfo.pclIpUcAction.qos.ingress.modifyUp;
            inFields[15] = nextHopInfo.pclIpUcAction.qos.ingress.profileIndex;
            inFields[16] = nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex;
            inFields[17] = nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence;
        }
        inFields[18] = nextHopInfo.pclIpUcAction.redirect.redirectCmd;
        inFields[19] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              type;
        hwDev = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                    devPort.hwDevNum;
        port = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                   devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[20] = hwDev;
        inFields[21] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[22] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                         trunkId;
        inFields[23] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              vidx;
        inFields[24] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            vlanId;
        inFields[25] = nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo;
        inFields[26] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart;
        inFields[27] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr;
        inFields[28] = nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex;
        inFields[29] = nextHopInfo.pclIpUcAction.policer.policerEnable;
        inFields[30] = nextHopInfo.pclIpUcAction.policer.policerId;
        inFields[31] = GT_FALSE; /*nextHopInfo.pclIpUcAction.vlan.egressTaggedModify;*/
        if (nextHopInfo.pclIpUcAction.egressPolicy == GT_FALSE)
        {
            inFields[32] = nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan;
            inFields[33] = nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.ingress.vlanId;
            inFields[35] = nextHopInfo.pclIpUcAction.vlan.ingress.precedence;
        }
        else
        {
            inFields[32] =
                (nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd ==
                 CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                    ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                    : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.egress.vlanId;
        }
        inFields[36] = nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute;
        inFields[37] = nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex;
        inFields[38] = nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL;
        inFields[39] = nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck;
        inFields[40] = nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck;

        /* inFields[42] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[41] = 0;
        inFields[42] = 0;
        inFields[43] = result;

        /* pack and output table fields for TAB 0 (PCL)*/
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                    %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10], inFields[11],
                    inFields[12], inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22], inFields[23],
                    inFields[24], inFields[25], inFields[26], inFields[27],
                    inFields[28], inFields[29], inFields[30], inFields[31],
                    inFields[32], inFields[33], inFields[34], inFields[35],
                    inFields[36], inFields[37], inFields[38], inFields[39],
                    inFields[40],inFields[41],inFields[42],inFields[43]);

        galtisOutput(outArgs, GT_OK, "%d%f", 0);
    }

    else/*Refresh LTT TAB*/
    {
        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &nextHopInfo.ipLttEntry);

        /* pack and output table fields for TAB 1 (LTT)*/
        inFields[0] = gIndex;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[3] = (GT_U32)gIpAddr.u32Ip;
        SWITCH_BYTES(gIpAddr.arIP, 4);
        inFields[4] = gPrefixLen;
        inFields[5] = nextHopInfo.ipLttEntry.routeType;
        inFields[6] = nextHopInfo.ipLttEntry.numOfPaths;
        inFields[7] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
        inFields[8] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
        inFields[9] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
        inFields[10] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

        /* inFields[12] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[11] = 0;
        inFields[12] = 0;
        inFields[13] = result;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10],
                    inFields[11], inFields[12], inFields[13]);

        galtisOutput(outArgs, GT_OK, "%d%f", 1);
    }

    gIndex++;

    return CMD_OK;
}
/*****************Table: cpssDxChIpLpmIpv4Mc**************/
static GT_IPADDR    gIpv4McGroup;
static GT_U32       gIpv4McGroupPrefixLen;
static GT_IPADDR    gIpv4McSrc;
static GT_U32       gIpv4McSrcPrefixLen;
static GT_U32       gIpv4McInd;
/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual private network identifier.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefix         - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - the LTT entry pointing to the MC route entry
*                             associated with this MC route.
*       override            - weather to override an mc Route pointer for the
*                             given prefix
*       defragmentationEnable - wether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM if failed to allocate TCAM memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntryAdd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    GT_IPADDR                       ipGroup;
    GT_U32                          ipGroupPrefixLen;
    GT_IPADDR                       ipSrc;
    GT_U32                          ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC      mcRouteLttEntry;
    GT_BOOL                         override;
    GT_BOOL                         defragmentationEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inFields[2]);
    ipGroupPrefixLen = (GT_U32)inFields[3];
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[4]);
    ipSrcPrefixLen = (GT_U32)inFields[5];
    mcRouteLttEntry.routeType =
                               (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[6];
    mcRouteLttEntry.numOfPaths = (GT_U32)inFields[7];
    mcRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[8];
    mcRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[9];
    mcRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[10];
    mcRouteLttEntry.ipv6MCGroupScopeLevel =
                                      (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[11];
    override = (GT_BOOL)inFields[12];
    defragmentationEnable = (GT_BOOL)inFields[13];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &mcRouteLttEntry);

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup,
                                          ipGroupPrefixLen, ipSrc,
                                          ipSrcPrefixLen , &mcRouteLttEntry,
                                          override, defragmentationEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist, or
*       GT_NOT_FOUND - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntryDel
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    GT_IPADDR                       ipGroup;
    GT_U32                          ipGroupPrefixLen;
    GT_IPADDR                       ipSrc;
    GT_U32                          ipSrcPrefixLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inFields[2]);
    ipGroupPrefixLen = (GT_U32)inFields[3];
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[4]);
    ipSrcPrefixLen = (GT_U32)inFields[5];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId, ipGroup,
                                          ipGroupPrefixLen, ipSrc,
                                          ipSrcPrefixLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntriesFlush
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       status;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;

    lpmDbPtr        = NULL;
    lpmDbIterator   = 0;    /*reset on first*/
    vrIterator      = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/
    vrEntryPtr      = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&lpmDbIterator);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
        for (vrIdValue = 0; vrIdValue < ramShadow->vrfTblSize; vrIdValue++)
        {
            status = cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfig);
            if (status == GT_OK)
            {
                status =  cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrIdValue);
                if (status != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }

            }
            else
            {
                if (status != GT_NOT_FOUND)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
        }
    }
    else
    {
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;
            /* call cpss api function */
            status =  cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrIdValue);

            /* pack output arguments to galtis string */
            galtisOutput(outArgs, status, "");
        }
    }

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntriesFlushVr
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;
    GT_U32                          lpmDBId;
    GT_U32                          vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId    = (GT_U32)inArgs[1];

    status =  cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - The next ip Group address.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The next ip Source address.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*       mcRouteLttEntryPtr  - the LTT entry pointer pointing to the MC route
*                             entry associated with this MC route.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntryGetNext_GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                              lpmDBId;
    CPSS_DXCH_IP_LTT_ENTRY_STC          mcRouteLttEntry;
    GT_U32                              gRowIndex;
    GT_U32                              gColumnIndex;
    GT_U32                              sRowIndex;
    GT_U32                              sColumnIndex;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT    shadowType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    gIpv4McInd = 0;
    vrId = 0;/*currently only 0 is valid*/
    gIpv4McGroup.arIP[0] = 0xE0;
    gIpv4McGroup.arIP[1] = 0;
    gIpv4McGroup.arIP[2] = 0;
    gIpv4McGroup.arIP[3] = 0;
    gIpv4McGroupPrefixLen = 4;
    gIpv4McSrc.u32Ip = 0; /*for first entry 0 is entered*/
    gIpv4McSrcPrefixLen = 0; /*for first entry 0 is entered*/

    isDefaultVrfIdEntry = GT_FALSE;

    result = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* allocate the virtual routers skip list*/
    switch (shadowType)
    {
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E:
        maxNumOfVr = DXCH_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E:
        maxNumOfVr = DXCH2_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E:
        maxNumOfVr = DXCH3_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E:
        maxNumOfVr = DXCH_XCAT_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E:
        maxNumOfVr = DXCH_LION3_MAX_VR_ID_CNS;
        break;
    default:
        return GT_FAIL;
    }

    for( ; vrId < maxNumOfVr ; vrId++)
    {
       /* call cpss api function */
       result = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, gIpv4McGroup,
                                             gIpv4McGroupPrefixLen,
                                             gIpv4McSrc, gIpv4McSrcPrefixLen,
                                             &mcRouteLttEntry, &gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);

        /* result is GT_NOT_FOUND if the virtual router doesn't exist,
           or GT_NOT_INITIALIZED if the virtual router is not valid */
        if ((result == GT_NOT_FOUND)||
            (result == GT_NOT_INITIALIZED))
             continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    /* pack and output table fields*/
    inFields[0] = gIpv4McInd;
    inFields[1] = vrId;
    inFields[3] = gIpv4McGroupPrefixLen;
    inFields[5] = gIpv4McSrcPrefixLen;
    inFields[6] = mcRouteLttEntry.routeType;
    inFields[7] = mcRouteLttEntry.numOfPaths;
    inFields[8] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[9] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[10] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[11] = mcRouteLttEntry.ipv6MCGroupScopeLevel;

    /* inFields[12] is the override field. It has only meaning in the set, but
       we should display it also in the get, else the command wrapper will fail*/
    inFields[12] = 0;
    inFields[13] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%4b%d%4b%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], gIpv4McGroup.arIP, inFields[3],
                gIpv4McSrc.arIP, inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12],inFields[13]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIpv4McInd++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - The next ip Group address.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The next ip Source address.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*       mcRouteLttEntryPtr  - the LTT entry pointer pointing to the MC route
*                             entry associated with this MC route.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntryGetNext_GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                          lpmDBId;
    CPSS_DXCH_IP_LTT_ENTRY_STC      mcRouteLttEntry;
    GT_U32                          gRowIndex;
    GT_U32                          gColumnIndex;
    GT_U32                          sRowIndex;
    GT_U32                          sColumnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* reset mcRouteLttEntry*/
    cmdOsMemSet(&mcRouteLttEntry, 0, sizeof(mcRouteLttEntry));

    if(vrId != lastVrfIdTreated)
    {
        gIpv4McGroup.arIP[0] = 0xE0;
        gIpv4McGroup.arIP[1] = 0;
        gIpv4McGroup.arIP[2] = 0;
        gIpv4McGroup.arIP[3] = 0;
        gIpv4McGroupPrefixLen = 4;

        gIpv4McSrc.u32Ip = 0; /*for first entry 0 is entered*/
        gIpv4McSrcPrefixLen = 0; /*for first entry 0 is entered*/

        for( ; vrId < maxNumOfVr ; vrId++)
        {
            /* call cpss api function */
            result = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, gIpv4McGroup,
                                             gIpv4McGroupPrefixLen,
                                             gIpv4McSrc, gIpv4McSrcPrefixLen,
                                             &mcRouteLttEntry, &gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);

            /* result is GT_NOT_FOUND if the virtual router doesn't exist,
               or GT_NOT_INITIALIZED if the virtual router is not valid */
            if ((result == GT_NOT_FOUND)||
                (result == GT_NOT_INITIALIZED))
                continue;
            else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in
           wrCpssDxChIpLpmIpv4McEntryGetNext_GetFirst or in previuos call
           to this wrapper   */

        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

        /* pack and output table fields*/
        inFields[0] = gIpv4McInd;
        inFields[1] = vrId;
        inFields[3] = gIpv4McGroupPrefixLen;
        inFields[5] = gIpv4McSrcPrefixLen;
        inFields[6] = mcRouteLttEntry.routeType;
        inFields[7] = mcRouteLttEntry.numOfPaths;
        inFields[8] = mcRouteLttEntry.routeEntryBaseIndex;
        inFields[9] = mcRouteLttEntry.ucRPFCheckEnable;
        inFields[10] = mcRouteLttEntry.sipSaCheckMismatchEnable;
        inFields[11] = mcRouteLttEntry.ipv6MCGroupScopeLevel;

        /* inFields[12] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[12] = 0;
        inFields[13] = 0;

        /* pack and output table fields */
        fieldOutput("%d%d%4b%d%4b%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], gIpv4McGroup.arIP, inFields[3],
                    gIpv4McSrc.arIP, inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10], inFields[11],
                    inFields[12],inFields[13]);

        galtisOutput(outArgs, GT_OK, "%f");

        gIpv4McInd++;
        isDefaultVrfIdEntry = GT_FALSE;

        return CMD_OK;

    }

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &gIpv4McGroup,
                                             &gIpv4McGroupPrefixLen,
                                             &gIpv4McSrc, &gIpv4McSrcPrefixLen,
                                             &mcRouteLttEntry,&gRowIndex,
                                             &gColumnIndex, &sRowIndex,&sColumnIndex);
    if (result == GT_NOT_FOUND)
    {
        vrId++;

        return wrCpssDxChIpLpmIpv4McEntryGetNext_GetNext(inArgs,inFields,numFields,outArgs);
    }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    /* pack and output table fields*/
    inFields[0] = gIpv4McInd;
    inFields[1] = vrId;
    inFields[3] = gIpv4McGroupPrefixLen;
    inFields[5] = gIpv4McSrcPrefixLen;
    inFields[6] = mcRouteLttEntry.routeType;
    inFields[7] = mcRouteLttEntry.numOfPaths;
    inFields[8] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[9] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[10] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[11] = mcRouteLttEntry.ipv6MCGroupScopeLevel;

    /* inFields[12] is the override field. It has only meaning in the set, but
       we should display it also in the get, else the command wrapper will fail*/
    inFields[12] = 0;
    inFields[13] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%4b%d%4b%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], gIpv4McGroup.arIP, inFields[3],
                gIpv4McSrc.arIP, inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12],inFields[13]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIpv4McInd++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*                          future support only , currently only vrId = 0 is
*                          supported.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr - the LTT entry pointer pointing to the MC route
*                            entry associated with this MC route.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntrySearch
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipGroup;
    GT_U32                                   ipGroupPrefixLen;
    GT_IPADDR                                ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC               mcRouteLttEntry;
    GT_U32                                   gRowIndex;
    GT_U32                                   gColumnIndex;
    GT_U32                                   sRowIndex;
    GT_U32                                   sColumnIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inArgs[2]);
    ipGroupPrefixLen = (GT_U32)inArgs[3];
    galtisIpAddr(&ipSrc, (GT_U8*)inArgs[4]);
    ipSrcPrefixLen = (GT_U32)inArgs[5];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup,
                                             ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry,
                                             &gRowIndex, &gColumnIndex,
                                             &sRowIndex, &sColumnIndex);

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d",
                 mcRouteLttEntry.routeType,
                 mcRouteLttEntry.numOfPaths,
                 mcRouteLttEntry.routeEntryBaseIndex,
                 mcRouteLttEntry.ucRPFCheckEnable,
                 mcRouteLttEntry.sipSaCheckMismatchEnable,
                 mcRouteLttEntry.ipv6MCGroupScopeLevel);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - If failed to allocate TCAM memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixAdd_PCL
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                   status;

    GT_U32                                      lpmDBId;
    GT_U32                                      vrId;
    GT_IPV6ADDR                                 ipAddr;
    GT_U32                                      prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT      defUcNextHop;
    GT_BOOL                                     override;
    GT_BOOL                                     defragmentationEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));


    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];
    defUcNextHop.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inArgs[4];
    defUcNextHop.pclIpUcAction.mirror.cpuCode =
                                            (CPSS_NET_RX_CPU_CODE_ENT)inArgs[5];
    defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                            (GT_BOOL)inArgs[6];
    defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount =
                                                            (GT_BOOL)inArgs[7];
    defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex =
                                                            (GT_U32)inArgs[8];
    defUcNextHop.pclIpUcAction.egressPolicy = (GT_BOOL)inArgs[9];

    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        defUcNextHop.pclIpUcAction.qos.egress.modifyDscp =
            (inArgs[10] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? GT_TRUE : GT_FALSE;
        defUcNextHop.pclIpUcAction.qos.egress.modifyUp =
            (inArgs[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
        defUcNextHop.pclIpUcAction.qos.egress.dscp = (GT_U8)inArgs[12];
        defUcNextHop.pclIpUcAction.qos.egress.up = (GT_U8)inArgs[13];
    }
    else
    {
        defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inArgs[10];
        defUcNextHop.pclIpUcAction.qos.ingress.modifyUp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inArgs[11];
        defUcNextHop.pclIpUcAction.qos.ingress.profileIndex = (GT_U32)inArgs[14];
        defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex = (GT_BOOL)inArgs[15];
        defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence =
            (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[16];
    }
    defUcNextHop.pclIpUcAction.redirect.redirectCmd =
                              (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inArgs[17];

    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inArgs[18];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    hwDevNum = (GT_U8)inArgs[19];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    portNum = (GT_U8)inArgs[20];

        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                             defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                        (GT_TRUNK_ID)inArgs[21];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                            (GT_U16)inArgs[22];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                            (GT_U16)inArgs[23];

        defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                            (GT_BOOL)inArgs[24];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                            (GT_BOOL)inArgs[25];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                            (GT_U32)inArgs[26];
    }
    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex =
                                                            (GT_U32)inArgs[27];
    }

    defUcNextHop.pclIpUcAction.policer.policerEnable = (GT_BOOL)inArgs[28];
    defUcNextHop.pclIpUcAction.policer.policerId = (GT_U32)inArgs[29];
    /*defUcNextHop.pclIpUcAction.vlan.egressTaggedModify = (GT_BOOL)inArgs[30];*/
    switch (defUcNextHop.pclIpUcAction.egressPolicy)
    {
        case GT_FALSE:
            defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inArgs[31];
            defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inArgs[32];
            defUcNextHop.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inArgs[33];
            defUcNextHop.pclIpUcAction.vlan.ingress.precedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[34];
            break;

        default:
        case GT_TRUE:
            defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd =
                ((inArgs[31] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                 || (inArgs[31] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
            defUcNextHop.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inArgs[33];
            break;
    }

    defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inArgs[35];
    defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inArgs[36];
    defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inArgs[37];
    defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inArgs[38];
    defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                            (GT_BOOL)inArgs[39];

    override = (GT_BOOL)inArgs[40];
    defragmentationEnable = (GT_BOOL)inArgs[41];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override,
                                           defragmentationEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************************** Table function **********************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixAddPCLEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                   status;

    GT_U32                                      lpmDBId;
    GT_U32                                      vrId;
    GT_IPV6ADDR                                 ipAddr;
    GT_U32                                      prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT      defUcNextHop;
    GT_BOOL                                     override;
    GT_BOOL                                     defragmentationEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&defUcNextHop, 0, sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inFields[0];
    vrId = (GT_U32)inFields[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[2]);
    prefixLen = (GT_U32)inFields[3];
    defUcNextHop.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[4];
    defUcNextHop.pclIpUcAction.mirror.cpuCode =
                                            (CPSS_NET_RX_CPU_CODE_ENT)inFields[5];
    defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                            (GT_BOOL)inFields[6];
    defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount =
                                                            (GT_BOOL)inFields[7];
    defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex =
                                                            (GT_U32)inFields[8];
    defUcNextHop.pclIpUcAction.egressPolicy = (GT_BOOL)inFields[9];

    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        defUcNextHop.pclIpUcAction.qos.egress.modifyDscp =
            (inFields[10] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? GT_TRUE : GT_FALSE;
        defUcNextHop.pclIpUcAction.qos.egress.modifyUp =
            (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
        defUcNextHop.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[12];
        defUcNextHop.pclIpUcAction.qos.egress.up = (GT_U8)inFields[13];
    }
    else
    {
        defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[10];
        defUcNextHop.pclIpUcAction.qos.ingress.modifyUp =
            (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
        defUcNextHop.pclIpUcAction.qos.ingress.profileIndex = (GT_U32)inFields[14];
        defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex = (GT_BOOL)inFields[15];
        defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence =
            (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[16];
    }

    defUcNextHop.pclIpUcAction.redirect.redirectCmd =
                              (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[17];

    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inFields[18];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    hwDevNum = (GT_U8)inFields[19];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                    portNum = (GT_U8)inFields[20];

        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                             defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                        (GT_TRUNK_ID)inFields[21];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                            (GT_U16)inFields[22];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                            (GT_U16)inFields[23];

        defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                            (GT_BOOL)inFields[24];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                            (GT_BOOL)inFields[25];
        defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                            (GT_U32)inFields[26];
    }
    if(defUcNextHop.pclIpUcAction.redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex =
                                                            (GT_U32)inFields[27];
    }

    defUcNextHop.pclIpUcAction.policer.policerEnable = (GT_BOOL)inFields[28];
    defUcNextHop.pclIpUcAction.policer.policerId = (GT_U32)inFields[29];
    switch (defUcNextHop.pclIpUcAction.egressPolicy)
    {
        case GT_FALSE:
            defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[30];
            defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inFields[31];
            defUcNextHop.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inFields[32];
            defUcNextHop.pclIpUcAction.vlan.ingress.precedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
            break;

        default:
        case GT_TRUE:
            defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd =
                ((inFields[30] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                 || (inFields[30] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
            defUcNextHop.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inFields[32];
            break;
    }

    defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[34];
    defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inFields[35];
    defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inFields[36];
    defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[37];
    defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                            (GT_BOOL)inFields[38];

    override = (GT_BOOL)inFields[39];
    defragmentationEnable = (GT_BOOL)inFields[40];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override,
                                           defragmentationEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override an existing entry for this mask if it already
*                         exists, or don't override and return error.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM - If failed to allocate TCAM memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixAdd_LTT
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                   status;

    GT_U32                                      lpmDBId;
    GT_U32                                      vrId;
    GT_IPV6ADDR                                 ipAddr;
    GT_U32                                      prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT      defUcNextHop;
    GT_BOOL                                     override;
    GT_BOOL                                     defragmentationEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];
    defUcNextHop.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inArgs[4];
    defUcNextHop.ipLttEntry.numOfPaths = (GT_U32)inArgs[5];
    defUcNextHop.ipLttEntry.routeEntryBaseIndex = (GT_U32)inArgs[6];
    defUcNextHop.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inArgs[7];
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inArgs[8];
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[9];

    override = (GT_BOOL)inArgs[10];
    defragmentationEnable = (GT_BOOL)inArgs[11];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defUcNextHop.ipLttEntry);

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override,
                                           defragmentationEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/**************************** Table function **********************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixAdd_LTTEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                   status;

    GT_U32                                      lpmDBId;
    GT_U32                                      vrId;
    GT_IPV6ADDR                                 ipAddr;
    GT_U32                                      prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT      defUcNextHop;
    GT_BOOL                                     override;
    GT_BOOL                                     defragmentationEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inFields[0];
    vrId = (GT_U32)inFields[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[2]);
    prefixLen = (GT_U32)inFields[3];
    defUcNextHop.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[4];
    defUcNextHop.ipLttEntry.numOfPaths = (GT_U32)inFields[5];
    defUcNextHop.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[6];
    defUcNextHop.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[7];
    defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[8];
    defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[9];

    override = (GT_BOOL)inFields[10];
    defragmentationEnable = (GT_BOOL)inFields[11];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &defUcNextHop.ipLttEntry);

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                           &defUcNextHop, override,
                                           defragmentationEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}
/***********Table: cpssDxChIpLpmIpv6UcPrefix***************/
/*set*/
static  GT_U32                                  gIpV6PrefixArrayLen;
static  CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*    gIpV6PrefixArray;

/*get*/
static  GT_IPV6ADDR                             gIpV6Addr;
static  GT_U32                                  gIpV6PrefixLen;
static  GT_U32                                  gIpV6Index;

static  GT_BOOL                                 gIpV6BulkGather = GT_FALSE;
/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssDxChIpV6LpmBulkGatherFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIpV6PrefixArrayLen = 0; /*Reset on First*/

    /* map input arguments to locals */
    if(gIpV6PrefixArray == NULL)
    {
        gIpV6PrefixArray = (CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*)cmdOsMalloc
                                (sizeof(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC));
    }
    if(gIpV6PrefixArray == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    cmdOsMemSet(
        gIpV6PrefixArray,
        0, (sizeof(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC)));

    ind = (GT_U32)inFields[0];

    if(ind > gIpV6PrefixArrayLen)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }


    if(!inArgs[2])/*for PCL*/
    {
        gIpV6PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpv6Addr(&(gIpV6PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpV6PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[5];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.cpuCode =
                                                (CPSS_NET_RX_CPU_CODE_ENT)inFields[6];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                                (GT_BOOL)inFields[7];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount =
                                                                (GT_BOOL)inFields[8];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex =
                                                                (GT_U32)inFields[9];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy = (GT_BOOL)inFields[10];
        if(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyDscp =
                (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? GT_TRUE : GT_FALSE;
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyUp =
                (inFields[12] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[13];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.up = (GT_U8)inFields[14];
        }
        else
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyUp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileIndex = (GT_U32)inFields[15];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex = (GT_BOOL)inFields[16];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence =
                            (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[17];
        }
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd =
                                  (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[18];

        if(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd ==
                                        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                                (CPSS_INTERFACE_TYPE_ENT)inFields[19];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        hwDevNum = (GT_U8)inFields[20];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        portNum = (GT_U8)inFields[21];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                                 gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                            (GT_TRUNK_ID)inFields[22];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                                (GT_U16)inFields[23];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                                (GT_U16)inFields[24];

            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                                (GT_BOOL)inFields[25];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                                (GT_BOOL)inFields[26];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                                (GT_U32)inFields[27];
        }
        if(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd
                                    == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex =
                                                                (GT_U32)inFields[28];
        }

        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerEnable = (GT_BOOL)inFields[29];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerId = (GT_U32)inFields[30];

        switch (gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy)
        {
            case GT_FALSE:
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[32];
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inFields[33];
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inFields[34];
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.precedence =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[35];
                break;

            default:
            case GT_TRUE:
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd =
                    ((inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                     || (inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inFields[34];
                break;
        }

        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[36];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inFields[37];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inFields[38];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[39];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                                (GT_BOOL)inFields[40];

        gIpV6PrefixArray[ind].override = (GT_BOOL)inFields[42];
        gIpV6PrefixArray[ind].returnStatus = (GT_STATUS)inFields[43];
    }

    else/*for LTT*/
    {
        gIpV6PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpv6Addr(&(gIpV6PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpV6PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.routeType =
                                  (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.numOfPaths =
                                                                (GT_U32)inFields[6];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.routeEntryBaseIndex =
                                                                (GT_U32)inFields[7];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.ucRPFCheckEnable =
                                                               (GT_BOOL)inFields[8];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable =
                                                               (GT_BOOL)inFields[9];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel =
                                            (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[10];

        gIpV6PrefixArray[ind].override = (GT_BOOL)inFields[12];
        gIpV6PrefixArray[ind].returnStatus = (GT_STATUS)inFields[13];

        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE,
                                                       &gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry);
    }

    gIpV6PrefixArrayLen++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static GT_STATUS wrCpssDxChIpV6LpmBulkGatherNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32      ind;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    gIpV6PrefixArray = (CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*)cmdOsRealloc
                                (gIpV6PrefixArray, (gIpV6PrefixArrayLen + 1) *
                                 sizeof(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC));

    if(gIpV6PrefixArray == NULL)
    {
      galtisOutput(outArgs, GT_NO_RESOURCE, "");
      return CMD_OK;
    }

    cmdOsMemSet(
        &(gIpV6PrefixArray[gIpV6PrefixArrayLen]),
        0, (sizeof(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC)));

    ind = (GT_U32)inFields[0];

    if(ind > gIpV6PrefixArrayLen)
    {
      galtisOutput(outArgs, GT_BAD_VALUE, "");
      return CMD_OK;
    }

    if(!inArgs[2])/*for PCL*/
    {
        gIpV6PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpv6Addr(&(gIpV6PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpV6PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.pktCmd = (CPSS_PACKET_CMD_ENT)inFields[5];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.cpuCode =
                                                (CPSS_NET_RX_CPU_CODE_ENT)inFields[6];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort =
                                                                (GT_BOOL)inFields[7];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount =
                                                                (GT_BOOL)inFields[8];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex =
                                                                (GT_U32)inFields[9];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy = (GT_BOOL)inFields[10];

        if(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyDscp =
                (inFields[11] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? GT_TRUE : GT_FALSE;
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.modifyUp =
                (inFields[12] == CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E)
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E;
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.dscp = (GT_U8)inFields[13];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.egress.up = (GT_U8)inFields[14];
        }
        else
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[11];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.modifyUp =
                (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[12];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileIndex = (GT_U32)inFields[15];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex = (GT_BOOL)inFields[16];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence =
                (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[17];
        }

        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd =
                                  (CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ENT)inFields[18];

        if(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd ==
                                        CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.type =
                                                (CPSS_INTERFACE_TYPE_ENT)inFields[19];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        hwDevNum = (GT_U8)inFields[20];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.
                                                        portNum = (GT_U8)inFields[21];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.hwDevNum,
                                 gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.devPort.portNum);

            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId =
                                                            (GT_TRUNK_ID)inFields[22];
            CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.trunkId);
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vidx =
                                                                (GT_U16)inFields[23];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.vlanId =
                                                                (GT_U16)inFields[24];

            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo =
                                                                (GT_BOOL)inFields[25];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart =
                                                                (GT_BOOL)inFields[26];
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr =
                                                                (GT_U32)inFields[27];
        }
        if(gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.redirectCmd
                                    == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
        {
            gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex =
                                                                (GT_U32)inFields[28];
        }

        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerEnable = (GT_BOOL)inFields[29];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.policer.policerId = (GT_U32)inFields[30];

        switch (gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.egressPolicy)
        {
            case GT_FALSE:
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_CMD_ENT)inFields[32];
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan = (GT_BOOL)inFields[33];
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.vlanId = (GT_U16)inFields[34];
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.ingress.precedence =
                    (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[35];
                break;

            default:
            case GT_TRUE:
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd =
                    ((inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E)
                     || (inFields[32] == CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_UNTAGGED_E))
                    ? CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E
                    : CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_MODIFY_OUTER_TAG_E;
                gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.vlan.egress.vlanId = (GT_U16)inFields[34];
                break;
        }

        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute = (GT_BOOL)inFields[36];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex = (GT_U32)inFields[37];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL = (GT_BOOL)inFields[38];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck = (GT_BOOL)inFields[39];
        gIpV6PrefixArray[ind].nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck =
                                                                (GT_BOOL)inFields[40];

        gIpV6PrefixArray[ind].override = (GT_BOOL)inFields[42];
        gIpV6PrefixArray[ind].returnStatus = (GT_STATUS)inFields[43];
    }

    else/*for LTT*/
    {
        gIpV6PrefixArray[ind].vrId = (GT_U32)inFields[2];
        galtisIpv6Addr(&(gIpV6PrefixArray[ind].ipAddr), (GT_U8*)inFields[3]);
        gIpV6PrefixArray[ind].prefixLen = (GT_U32)inFields[4];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.routeType =
                                  (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[5];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.numOfPaths =
                                                                (GT_U32)inFields[6];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.routeEntryBaseIndex =
                                                                (GT_U32)inFields[7];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.ucRPFCheckEnable =
                                                               (GT_BOOL)inFields[8];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable =
                                                               (GT_BOOL)inFields[9];
        gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel =
                                            (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[10];

        gIpV6PrefixArray[ind].override = (GT_BOOL)inFields[12];
        gIpV6PrefixArray[ind].returnStatus = (GT_STATUS)inFields[13];

        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE,
                                                       &gIpV6PrefixArray[ind].nextHopInfo.ipLttEntry);
    }

    gIpV6PrefixArrayLen++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixAdd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1 = GT_OK;
    GT_STATUS   status2 = GT_OK;

    GT_U32                                   lpmDBId;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;
    GT_BOOL                                  defragEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];
    defragEnable = (GT_BOOL)inArgs[1];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gIpV6BulkGather)
        {
            status1 = wrCpssDxChIpV6LpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step SET*/
        {

            status2 = wrCpssDxChIpV6LpmBulkGatherFirst(inArgs, inFields,
                                                    numFields, outArgs);
            /* call cpss api function */
            status1 =  cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId,
                                             gIpV6PrefixArray[0].vrId,
                                             gIpV6PrefixArray[0].ipAddr,
                                             gIpV6PrefixArray[0].prefixLen,
                                             &(gIpV6PrefixArray[0].nextHopInfo),
                                             gIpV6PrefixArray[0].override,
                                             defragEnable);


            if(status2 != GT_OK)
            {
                status1 = status2;
            }

            cmdOsFree(gIpV6PrefixArray);
            gIpV6PrefixArray = NULL;
            gIpV6PrefixArrayLen = 0;
        }
        break;

    case BULK_STARTING_ENTRY:
        gIpV6BulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssDxChIpV6LpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssDxChIpV6LpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);

        status1 =  cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId,
                                                    gIpV6PrefixArrayLen,
                                                    gIpV6PrefixArray,
                                                    defragEnable);
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        cmdOsFree(gIpV6PrefixArray);
        gIpV6PrefixArray = NULL;
        gIpV6PrefixArrayLen = 0;

        gIpV6BulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixDel
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status1 = GT_OK;
    GT_STATUS   status2 = GT_OK;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipAddr;
    GT_U32                                   prefixLen;
    WRAP_BULK_ENTRY_ROLE_ENT                 entryBulkRole;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    entryBulkRole = (WRAP_BULK_ENTRY_ROLE_ENT)inFields[1];

    lpmDBId = (GT_U32)inArgs[0];

    switch(entryBulkRole)
    {

    case REGULAR_ENTRY:
        if(gIpV6BulkGather)
        {
            status1 = wrCpssDxChIpV6LpmBulkGatherNext(inArgs, inFields,
                                            numFields, outArgs);
        }
        else/*Regular non bulk one step delete*/
        {
            vrId = (GT_U32)inFields[2];
            galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[3]);
            prefixLen = (GT_U32)inFields[4];

            /* call cpss api function */
            status1 =  cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId,
                                                   ipAddr, prefixLen);
        }
        break;

    case BULK_STARTING_ENTRY:
        gIpV6BulkGather = GT_TRUE; /*Indicate that Bulk gathering is taking place*/
        status1 = wrCpssDxChIpV6LpmBulkGatherFirst(inArgs, inFields,
                                                numFields, outArgs);
        break;

    case BULK_ENDING_ENTRY:
        status2 = wrCpssDxChIpV6LpmBulkGatherNext(inArgs, inFields,
                                               numFields, outArgs);

        status1 =  cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, gIpV6PrefixArrayLen,
                                                             gIpV6PrefixArray);
        if(status2 != GT_OK)
        {
            status1 = status2;
        }

        cmdOsFree(gIpV6PrefixArray);
        gIpV6PrefixArray = NULL;
        gIpV6PrefixArrayLen = 0;

        gIpV6BulkGather = GT_FALSE;/*End Bulk Gathering indication*/

        break;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status1, "");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArray    - The UC prefix array.
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{/*just a dummy for galtis*/
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.
*                       future support only , currently only vrId = 0 is
*                       supported.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       1.  The values of (ipAddrPtr,prefixLenPtr) must be a valid values, it
*           means that they exist in the IP-UC Table, unless this is the first
*           call to this function, then the value of (ipAddrPtr,prefixLenPtr) is
*           (0,0).
*       2.  In order to get route pointer information for (0,0) use the Ipv6
*           UC prefix get function.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixGetNext_GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                                    lpmDBId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT          shadowType;
    GT_U32                                    tcamRowIndex;
    GT_U32                                    tcamColumnIndex;
    GT_HW_DEV_NUM                             hwDev; /* Dummy for converting. */
    GT_PORT_NUM                               port;  /* Dummy for converting. */

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    gIpV6Index = 0;
    vrId = 0;/*for first entry 0 is entered*/
    gIpV6Addr.u32Ip[0] = 0; /*for first entry 0 is entered*/
    gIpV6Addr.u32Ip[1] = 0; /*for first entry 0 is entered*/
    gIpV6Addr.u32Ip[2] = 0; /*for first entry 0 is entered*/
    gIpV6Addr.u32Ip[3] = 0; /*for first entry 0 is entered*/
    gIpV6PrefixLen = 0; /*for first entry 0 is entered*/

    isDefaultVrfIdEntry = GT_FALSE;

    result = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* allocate the virtual routers skip list*/
    switch (shadowType)
    {
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E:
        maxNumOfVr = DXCH_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E:
        maxNumOfVr = DXCH2_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E:
        maxNumOfVr = DXCH3_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E:
    case CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
        maxNumOfVr = DXCH_XCAT_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E:
        maxNumOfVr = DXCH_LION3_MAX_VR_ID_CNS;
        break;
    default:
        return GT_FAIL;
    }

    for( ; vrId < maxNumOfVr ; vrId++)
    {
        /* call cpss api function */
        result = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, gIpV6Addr,
                                                 gIpV6PrefixLen, &nextHopInfo,
                                                 &tcamRowIndex,&tcamColumnIndex);
        /* result is GT_NOT_FOUND if the virtual router doesn't exist,
           or GT_NOT_INITIALIZED if the virtual router is not valid */
        if ((result == GT_NOT_FOUND)||
            (result == GT_NOT_INITIALIZED))
            continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }


    if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E)/*Refresh PCL TAB*/
        || (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[3] = (GT_UINTPTR)gIpV6Addr.u32Ip;
        inFields[4] = gIpV6PrefixLen;
        inFields[5] = nextHopInfo.pclIpUcAction.pktCmd;
        inFields[6] = nextHopInfo.pclIpUcAction.mirror.cpuCode;
        inFields[7] = nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
        inFields[8] = nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount;
        inFields[9] = nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex;
        inFields[10] = nextHopInfo.pclIpUcAction.egressPolicy;
        if(nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            inFields[11] =
                (nextHopInfo.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                    ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                    : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[12] =
                    (nextHopInfo.pclIpUcAction.qos.egress.modifyUp ==
                     CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                        ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                        : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[13] = nextHopInfo.pclIpUcAction.qos.egress.dscp;
            inFields[14] = nextHopInfo.pclIpUcAction.qos.egress.up;
        }
        else
        {
            inFields[11] = nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp;
            inFields[12] = nextHopInfo.pclIpUcAction.qos.ingress.modifyUp;
            inFields[15] = nextHopInfo.pclIpUcAction.qos.ingress.profileIndex;
            inFields[16] = nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex;
            inFields[17] = nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence;
        }
        inFields[18] = nextHopInfo.pclIpUcAction.redirect.redirectCmd;
        inFields[19] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              type;
        hwDev = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                    devPort.hwDevNum;
        port = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                   devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[20] = hwDev;
        inFields[21] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[22] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                         trunkId;
        inFields[23] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              vidx;
        inFields[24] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            vlanId;
        inFields[25] = nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo;
        inFields[26] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart;
        inFields[27] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr;
        inFields[28] = nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex;
        inFields[29] = nextHopInfo.pclIpUcAction.policer.policerEnable;
        inFields[30] = nextHopInfo.pclIpUcAction.policer.policerId;
        inFields[31] = GT_FALSE; /*nextHopInfo.pclIpUcAction.vlan.egressTaggedModify;*/
        if (nextHopInfo.pclIpUcAction.egressPolicy == GT_FALSE)
        {
            inFields[32] = nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan;
            inFields[33] = nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.ingress.vlanId;
            inFields[35] = nextHopInfo.pclIpUcAction.vlan.ingress.precedence;
        }
        else
        {
            inFields[32] =
                (nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd ==
                 CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                    ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                    : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.egress.vlanId;
        }
        inFields[36] = nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute;
        inFields[37] = nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex;
        inFields[38] = nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL;
        inFields[39] = nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck;
        inFields[40] = nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck;

        /* inFields[42] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[41] = 0;
        inFields[42] = 0;
        inFields[43] = result;

        /* pack and output table fields for TAB 0 (PCL)*/
        fieldOutput("%d%d%d%16B%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                    %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10], inFields[11],
                    inFields[12], inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22], inFields[23],
                    inFields[24], inFields[25], inFields[26], inFields[27],
                    inFields[28], inFields[29], inFields[30], inFields[31],
                    inFields[32], inFields[33], inFields[34], inFields[35],
                    inFields[36], inFields[37], inFields[38], inFields[39],
                    inFields[40],inFields[41],inFields[42],inFields[43]);

        galtisOutput(outArgs, GT_OK, "%d%f", 0);
    }

    else/*Refresh LTT TAB*/
    {
        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &nextHopInfo.ipLttEntry);

        /* pack and output table fields for TAB 1 (LTT)*/
        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[3] = (GT_UINTPTR)gIpV6Addr.u32Ip;
        inFields[4] = gIpV6PrefixLen;
        inFields[5] = nextHopInfo.ipLttEntry.routeType;
        inFields[6] = nextHopInfo.ipLttEntry.numOfPaths;
        inFields[7] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
        inFields[8] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
        inFields[9] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
        inFields[10] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

        /* inFields[12] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[11] = 0;
        inFields[12] = 0;
        inFields[13] = result;

        /* pack and output table fields */
        fieldOutput("%d%d%d%16B%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10],inFields[11],
                    inFields[12],inFields[13]);

        galtisOutput(outArgs, GT_OK, "%d%f", 1);
    }

    gIpV6Index++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one, it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.
*                       future support only , currently only vrId = 0 is
*                       supported.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if no more entries are left in the IP table.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixGetNext_GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                                    lpmDBId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT          shadowType;
    GT_U32                                    tcamRowIndex;
    GT_U32                                    tcamColumnIndex;
    GT_HW_DEV_NUM                             hwDev; /* Dummy for converting. */
    GT_PORT_NUM                               port;  /* Dummy for converting. */

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(vrId != lastVrfIdTreated)
    {
        gIpV6Addr.u32Ip[0] = 0; /*for first entry 0 is entered*/
        gIpV6Addr.u32Ip[1] = 0; /*for first entry 0 is entered*/
        gIpV6Addr.u32Ip[2] = 0; /*for first entry 0 is entered*/
        gIpV6Addr.u32Ip[3] = 0; /*for first entry 0 is entered*/
        gIpV6PrefixLen = 0; /*for first entry 0 is entered*/

        for( ; vrId < maxNumOfVr ; vrId++)
        {
            /* call cpss api function */
            result = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, gIpV6Addr,
                                                     gIpV6PrefixLen, &nextHopInfo,
                                                     &tcamRowIndex, &tcamColumnIndex);

            /* result is GT_NOT_FOUND if the virtual router doesn't exist,
               or GT_NOT_INITIALIZED if the virtual router is not valid */
            if ((result == GT_NOT_FOUND)||
                (result == GT_NOT_INITIALIZED))
                continue;
            else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    result = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in
           wrCpssDxChIpLpmIpv6UcPrefixGetNext_GetFirst or in previuos call
           to this wrapper                                                   */

        /* show default entry for vrId*/
        if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E)/*Refresh PCL TAB*/
            || (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
        {
            inFields[0] = gIpV6Index;
            inFields[1] = REGULAR_ENTRY;
            inFields[2] = vrId;
            inFields[3] = (GT_UINTPTR)gIpV6Addr.u32Ip;
            inFields[4] = gIpV6PrefixLen;
            inFields[5] = nextHopInfo.pclIpUcAction.pktCmd;
            inFields[6] = nextHopInfo.pclIpUcAction.mirror.cpuCode;
            inFields[7] = nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
            inFields[8] = nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount;
            inFields[9] = nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex;
            inFields[10] = nextHopInfo.pclIpUcAction.egressPolicy;
            if(nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
            {
                inFields[11] =
                    (nextHopInfo.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                        ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                        : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                inFields[12] =
                        (nextHopInfo.pclIpUcAction.qos.egress.modifyUp ==
                         CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                            ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                            : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
                inFields[13] = nextHopInfo.pclIpUcAction.qos.egress.dscp;
                inFields[14] = nextHopInfo.pclIpUcAction.qos.egress.up;
            }
            else
            {
                inFields[11] = nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp;
                inFields[12] = nextHopInfo.pclIpUcAction.qos.ingress.modifyUp;
                inFields[15] = nextHopInfo.pclIpUcAction.qos.ingress.profileIndex;
                inFields[16] = nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex;
                inFields[17] = nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence;
            }
            inFields[18] = nextHopInfo.pclIpUcAction.redirect.redirectCmd;
            inFields[19] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                  type;
            hwDev = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        devPort.hwDevNum;
            port = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                       devPort.portNum;
            CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
            inFields[20] = hwDev;
            inFields[21] = port;
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                trunkId);
            inFields[22] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                             trunkId;
            inFields[23] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                  vidx;
            inFields[24] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                                vlanId;
            inFields[25] = nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo;
            inFields[26] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart;
            inFields[27] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr;
            inFields[28] = nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex;
            inFields[29] = nextHopInfo.pclIpUcAction.policer.policerEnable;
            inFields[30] = nextHopInfo.pclIpUcAction.policer.policerId;
            inFields[31] = GT_FALSE; /*nextHopInfo.pclIpUcAction.vlan.egressTaggedModify;*/
            if (nextHopInfo.pclIpUcAction.egressPolicy == GT_FALSE)
            {
                inFields[32] = nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan;
                inFields[33] = nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan;
                inFields[34] = nextHopInfo.pclIpUcAction.vlan.ingress.vlanId;
                inFields[35] = nextHopInfo.pclIpUcAction.vlan.ingress.precedence;
            }
            else
            {
                inFields[32] =
                    (nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd ==
                     CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                        ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                        : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
                inFields[34] = nextHopInfo.pclIpUcAction.vlan.egress.vlanId;
            }
            inFields[36] = nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute;
            inFields[37] = nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex;
            inFields[38] = nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL;
            inFields[39] = nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck;
            inFields[40] = nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck;

            /* inFields[42] is the override field. It has only meaning in the set, but
               we should display it also in the get, else the command wrapper will fail*/
            inFields[41] = 0;
            inFields[42] = 0;
            inFields[43] = result;

            /* pack and output table fields for TAB 0 (PCL)*/
            fieldOutput("%d%d%d%16B%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                        %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                        ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10], inFields[11],
                        inFields[12], inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18], inFields[19],
                        inFields[20], inFields[21], inFields[22], inFields[23],
                        inFields[24], inFields[25], inFields[26], inFields[27],
                        inFields[28], inFields[29], inFields[30], inFields[31],
                        inFields[32], inFields[33], inFields[34], inFields[35],
                        inFields[36], inFields[37], inFields[38], inFields[39],
                        inFields[40],inFields[41],inFields[42],inFields[43]);


            galtisOutput(outArgs, GT_OK, "%d%f", 0);
        }

        else/*Refresh LTT TAB*/
        {
            prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &nextHopInfo.ipLttEntry);

            /* pack and output table fields for TAB 1 (LTT)*/
            inFields[0] = gIpV6Index;
            inFields[1] = REGULAR_ENTRY;
            inFields[2] = vrId;
            inFields[3] = (GT_UINTPTR)gIpV6Addr.u32Ip;
            inFields[4] = gIpV6PrefixLen;
            inFields[5] = nextHopInfo.ipLttEntry.routeType;
            inFields[6] = nextHopInfo.ipLttEntry.numOfPaths;
            inFields[7] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
            inFields[8] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
            inFields[9] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
            inFields[10] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

            /* inFields[12] is the override field. It has only meaning in the set, but
               we should display it also in the get, else the command wrapper will fail*/
            inFields[11] = 0;
            inFields[12] = 0;
            inFields[13] = result;

            /* pack and output table fields */
            fieldOutput("%d%d%d%16B%d%d%d%d%d%d%d%d%d%d"
                        ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                        inFields[4], inFields[5], inFields[6], inFields[7],
                        inFields[8], inFields[9], inFields[10],inFields[11],
                        inFields[12],inFields[13]);

            galtisOutput(outArgs, GT_OK, "%d%f", 1);
        }

        gIpV6Index++;
        isDefaultVrfIdEntry = GT_FALSE;

        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &gIpV6Addr,
                                              &gIpV6PrefixLen, &nextHopInfo,
                                              &tcamRowIndex,&tcamColumnIndex);

    if (result == GT_NOT_FOUND)
    {
        vrId++;

        return wrCpssDxChIpLpmIpv6UcPrefixGetNext_GetNext(inArgs,inFields,numFields,outArgs);
    }


    if ((shadowType == CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E)/*Refresh PCL TAB*/
        || (shadowType == CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E))
    {
        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[3] = (GT_UINTPTR)gIpV6Addr.u32Ip;
        inFields[4] = gIpV6PrefixLen;
        inFields[5] = nextHopInfo.pclIpUcAction.pktCmd;
        inFields[6] = nextHopInfo.pclIpUcAction.mirror.cpuCode;
        inFields[7] = nextHopInfo.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
        inFields[8] = nextHopInfo.pclIpUcAction.matchCounter.enableMatchCount;
        inFields[9] = nextHopInfo.pclIpUcAction.matchCounter.matchCounterIndex;
        inFields[10] = nextHopInfo.pclIpUcAction.egressPolicy;
        if(nextHopInfo.pclIpUcAction.egressPolicy != GT_FALSE)
        {
            inFields[11] =
                (nextHopInfo.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                    ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                    : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[12] =
                    (nextHopInfo.pclIpUcAction.qos.egress.modifyUp ==
                     CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                        ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                        : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
            inFields[13] = nextHopInfo.pclIpUcAction.qos.egress.dscp;
            inFields[14] = nextHopInfo.pclIpUcAction.qos.egress.up;
        }
        else
        {
            inFields[11] = nextHopInfo.pclIpUcAction.qos.ingress.modifyDscp;
            inFields[12] = nextHopInfo.pclIpUcAction.qos.ingress.modifyUp;
            inFields[15] = nextHopInfo.pclIpUcAction.qos.ingress.profileIndex;
            inFields[16] = nextHopInfo.pclIpUcAction.qos.ingress.profileAssignIndex;
            inFields[17] = nextHopInfo.pclIpUcAction.qos.ingress.profilePrecedence;
        }
        inFields[18] = nextHopInfo.pclIpUcAction.redirect.redirectCmd;
        inFields[19] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              type;
        hwDev = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                    devPort.hwDevNum;
        port = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                   devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[20] = hwDev;
        inFields[21] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[22] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                         trunkId;
        inFields[23] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                              vidx;
        inFields[24] = nextHopInfo.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                            vlanId;
        inFields[25] = nextHopInfo.pclIpUcAction.redirect.data.outIf.vntL2Echo;
        inFields[26] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelStart;
        inFields[27] = nextHopInfo.pclIpUcAction.redirect.data.outIf.tunnelPtr;
        inFields[28] = nextHopInfo.pclIpUcAction.redirect.data.routerLttIndex;
        inFields[29] = nextHopInfo.pclIpUcAction.policer.policerEnable;
        inFields[30] = nextHopInfo.pclIpUcAction.policer.policerId;
        inFields[31] = GT_FALSE; /*nextHopInfo.pclIpUcAction.vlan.egressTaggedModify;*/
        if (nextHopInfo.pclIpUcAction.egressPolicy == GT_FALSE)
        {
            inFields[32] = nextHopInfo.pclIpUcAction.vlan.ingress.modifyVlan;
            inFields[33] = nextHopInfo.pclIpUcAction.vlan.ingress.nestedVlan;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.ingress.vlanId;
            inFields[35] = nextHopInfo.pclIpUcAction.vlan.ingress.precedence;
        }
        else
        {
            inFields[32] =
                (nextHopInfo.pclIpUcAction.vlan.egress.vlanCmd ==
                 CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                    ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                    : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
            inFields[34] = nextHopInfo.pclIpUcAction.vlan.egress.vlanId;
        }
        inFields[36] = nextHopInfo.pclIpUcAction.ipUcRoute.doIpUcRoute;
        inFields[37] = nextHopInfo.pclIpUcAction.ipUcRoute.arpDaIndex;
        inFields[38] = nextHopInfo.pclIpUcAction.ipUcRoute.decrementTTL;
        inFields[39] = nextHopInfo.pclIpUcAction.ipUcRoute.bypassTTLCheck;
        inFields[40] = nextHopInfo.pclIpUcAction.ipUcRoute.icmpRedirectCheck;

        /* inFields[42] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[41] = 0;
        inFields[42] = 0;
        inFields[43] = result;

        /* pack and output table fields for TAB 0 (PCL)*/
        fieldOutput("%d%d%d%16B%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                    %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10], inFields[11],
                    inFields[12], inFields[13], inFields[14], inFields[15],
                    inFields[16], inFields[17], inFields[18], inFields[19],
                    inFields[20], inFields[21], inFields[22], inFields[23],
                    inFields[24], inFields[25], inFields[26], inFields[27],
                    inFields[28], inFields[29], inFields[30], inFields[31],
                    inFields[32], inFields[33], inFields[34], inFields[35],
                    inFields[36], inFields[37], inFields[38], inFields[39],
                    inFields[40],inFields[41],inFields[42],inFields[43]);


        galtisOutput(outArgs, GT_OK, "%d%f", 0);
    }

    else/*Refresh LTT TAB*/
    {
        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &nextHopInfo.ipLttEntry);

        /* pack and output table fields for TAB 1 (LTT)*/
        inFields[0] = gIpV6Index;
        inFields[1] = REGULAR_ENTRY;
        inFields[2] = vrId;
        inFields[3] = (GT_UINTPTR)gIpV6Addr.u32Ip;
        inFields[4] = gIpV6PrefixLen;
        inFields[5] = nextHopInfo.ipLttEntry.routeType;
        inFields[6] = nextHopInfo.ipLttEntry.numOfPaths;
        inFields[7] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
        inFields[8] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
        inFields[9] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
        inFields[10] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;

        /* inFields[12] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[11] = 0;
        inFields[12] = 0;
        inFields[13] = result;

        /* pack and output table fields */
        fieldOutput("%d%d%d%16B%d%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], gIpV6Addr.arIP,
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10],inFields[11],
                    inFields[12],inFields[13]);

        galtisOutput(outArgs, GT_OK, "%d%f", 1);
    }

    gIpV6Index++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixesFlush
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;


    lpmDbIterator   = 0;    /*reset on first*/
    lpmDBId         = 0;    /*reset on first*/
    lpmDbPtr        = NULL;
    vrIterator      = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/
    vrEntryPtr      = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&lpmDbIterator);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }

    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
        for (vrIdValue = 0; vrIdValue < ramShadow->vrfTblSize; vrIdValue++)
        {
            status = cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfig);
            if (status == GT_OK)
            {
                status =  cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrIdValue);
                if (status != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
            else
            {
                if (status != GT_NOT_FOUND)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
        }
    }
    else
    {
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;
            /* call cpss api function */
            status =  cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrIdValue);

            /* pack output arguments to galtis string */
            galtisOutput(outArgs, status, "");
        }
    }

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixesFlush
*
* DESCRIPTION:
*   Flushes the unicast IPv6 Routing table and stays with the default prefix
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixesFlushVr
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;

    lpmDBId         = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrIdValue = (GT_U32)inArgs[1];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixesFlush(lpmDBId, vrIdValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixSearch_PCL
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   tcamRowIndex;
    GT_U32                                   tcamColumnIndex;
    GT_HW_DEV_NUM                            hwDev;   /* Dummy for converting. */
    GT_PORT_NUM                              port;  /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &defUcNextHop, &tcamRowIndex,&tcamColumnIndex);

    inFields[0] = defUcNextHop.pclIpUcAction.pktCmd;
    inFields[1] = defUcNextHop.pclIpUcAction.mirror.cpuCode;
    inFields[2] = defUcNextHop.pclIpUcAction.mirror.mirrorToRxAnalyzerPort;
    inFields[3] = defUcNextHop.pclIpUcAction.matchCounter.enableMatchCount;
    inFields[4] = defUcNextHop.pclIpUcAction.matchCounter.matchCounterIndex;
    inFields[5] = defUcNextHop.pclIpUcAction.egressPolicy;
    if(defUcNextHop.pclIpUcAction.egressPolicy != GT_FALSE)
    {
        inFields[6] =
            (defUcNextHop.pclIpUcAction.qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[7] =
                (defUcNextHop.pclIpUcAction.qos.egress.modifyUp ==
                 CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                    ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                    : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[8] = defUcNextHop.pclIpUcAction.qos.egress.dscp;
        inFields[9] = defUcNextHop.pclIpUcAction.qos.egress.up;
    }
    else
    {
        inFields[6] = defUcNextHop.pclIpUcAction.qos.ingress.modifyDscp;
        inFields[7] = defUcNextHop.pclIpUcAction.qos.ingress.modifyUp;
        inFields[10] = defUcNextHop.pclIpUcAction.qos.ingress.profileIndex;
        inFields[11] = defUcNextHop.pclIpUcAction.qos.ingress.profileAssignIndex;
        inFields[12] = defUcNextHop.pclIpUcAction.qos.ingress.profilePrecedence;
    }
    inFields[13] = defUcNextHop.pclIpUcAction.redirect.redirectCmd;
    inFields[14] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                          type;
    hwDev = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                devPort.hwDevNum;
    port = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                               devPort.portNum;
    CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
    inFields[15] = hwDev;
    inFields[16] = port;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        trunkId);
    inFields[17] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        trunkId;
    inFields[18] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                          vidx;
    inFields[19] = defUcNextHop.pclIpUcAction.redirect.data.outIf.outInterface.
                                                                        vlanId;
    inFields[20] = defUcNextHop.pclIpUcAction.redirect.data.outIf.vntL2Echo;
    inFields[21] = defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelStart;
    inFields[22] = defUcNextHop.pclIpUcAction.redirect.data.outIf.tunnelPtr;
    inFields[23] = defUcNextHop.pclIpUcAction.redirect.data.routerLttIndex;
    inFields[24] = defUcNextHop.pclIpUcAction.policer.policerEnable;
    inFields[25] = defUcNextHop.pclIpUcAction.policer.policerId;
    inFields[26] = GT_FALSE; /*defUcNextHop.pclIpUcAction.vlan.egressTaggedModify;*/
    if (defUcNextHop.pclIpUcAction.egressPolicy == GT_FALSE)
    {
        inFields[27] = defUcNextHop.pclIpUcAction.vlan.ingress.modifyVlan;
        inFields[28] = defUcNextHop.pclIpUcAction.vlan.ingress.nestedVlan;
        inFields[29] = defUcNextHop.pclIpUcAction.vlan.ingress.vlanId;
        inFields[30] = defUcNextHop.pclIpUcAction.vlan.ingress.precedence;
    }
    else
    {
        inFields[28] =
            (defUcNextHop.pclIpUcAction.vlan.egress.vlanCmd ==
             CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
        inFields[30] = defUcNextHop.pclIpUcAction.vlan.egress.vlanId;
    }
    inFields[31] = defUcNextHop.pclIpUcAction.ipUcRoute.doIpUcRoute;
    inFields[32] = defUcNextHop.pclIpUcAction.ipUcRoute.arpDaIndex;
    inFields[33] = defUcNextHop.pclIpUcAction.ipUcRoute.decrementTTL;
    inFields[34] = defUcNextHop.pclIpUcAction.ipUcRoute.bypassTTLCheck;
    inFields[35] = defUcNextHop.pclIpUcAction.ipUcRoute.icmpRedirectCheck;


    /* pack and output fields */
    galtisOutput(outArgs, status,"%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*
* RETURNS:
*       GT_OK - if the required entry was found, or
*       GT_NOT_FOUND - if the given ip prefix was not found.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixSearch_LTT
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   tcamRowIndex;
    GT_U32                                   tcamColumnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &defUcNextHop, &tcamRowIndex,&tcamColumnIndex);

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &defUcNextHop.ipLttEntry);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d",
                 defUcNextHop.ipLttEntry.routeType,
                 defUcNextHop.ipLttEntry.numOfPaths,
                 defUcNextHop.ipLttEntry.routeEntryBaseIndex,
                 defUcNextHop.ipLttEntry.ucRPFCheckEnable,
                 defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable,
                 defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel);

    return CMD_OK;
}


/*****************Table: cpssDxChIpLpmIpv6Mc**************/
static GT_IPV6ADDR      gIpv6McGroup;
static GT_U32           gIpv6McGroupPrefixLen;
static GT_IPV6ADDR      gIpv6McSrc;
static GT_U32           gIpv6McSrcPrefixLen;
static GT_U32           gIpv6McInd;
/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefix     - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - the LTT entry pointing to the MC route entry
*                            associated with this MC route.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*       defragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM if failed to allocate PP memory, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntryAdd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    GT_IPV6ADDR                     ipGroup;
    GT_U32                          ipGroupPrefixLen;
    GT_IPV6ADDR                     ipSrc;
    GT_U32                          ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC      mcRouteLttEntry;
    GT_BOOL                         override;
    GT_BOOL                         defragmentationEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inFields[2]);
    ipGroupPrefixLen = (GT_U32)inFields[3];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inFields[4]);
    ipSrcPrefixLen = (GT_U32)inFields[5];
    mcRouteLttEntry.routeType =
                               (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[6];
    mcRouteLttEntry.numOfPaths = (GT_U32)inFields[7];
    mcRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[8];
    mcRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[9];
    mcRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[10];
    mcRouteLttEntry.ipv6MCGroupScopeLevel =
                                      (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[11];
    override = (GT_BOOL)inFields[12];
    defragmentationEnable = (GT_BOOL)inFields[13];

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_TRUE, &mcRouteLttEntry);

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup,
                                          ipGroupPrefixLen, ipSrc,
                                          ipSrcPrefixLen , &mcRouteLttEntry,
                                          override, defragmentationEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success, or
*       GT_ERROR - if the virtual router does not exist, or
*       GT_NOT_FOUND - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       1.  Inorder to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntryDel
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;

    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    GT_IPV6ADDR                     ipGroup;
    GT_U32                          ipGroupPrefixLen;
    GT_IPV6ADDR                     ipSrc;
    GT_U32                          ipSrcPrefixLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inFields[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inFields[2]);
    ipGroupPrefixLen = (GT_U32)inFields[3];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inFields[4]);
    ipSrcPrefixLen = (GT_U32)inFields[5];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId, ipGroup,
                                          ipGroupPrefixLen, ipSrc,
                                          ipSrcPrefixLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntriesFlush
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;
    PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC *tcamShadow;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *ramShadow;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC     vrConfig;


    lpmDbPtr        = NULL;
    lpmDbIterator   = 0;    /*reset on first*/
    vrIterator      = 0;    /*reset on first*/
    vrIdValue       = 0;    /*reset on first*/
    vrEntryPtr      = NULL;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&lpmDbIterator);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        return GT_NOT_FOUND;
    }
    if (lpmDbPtr->shadowType == PRV_CPSS_DXCH_LPM_RAM_LION3_SHADOW_E)
    {
        ramShadow = ((PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow))->shadowArray;
        for (vrIdValue = 0; vrIdValue < ramShadow->vrfTblSize; vrIdValue++)
        {
            status = cpssDxChIpLpmVirtualRouterGet(lpmDBId,vrIdValue,&vrConfig);
            if(status == GT_OK)
            {
                status =  cpssDxChIpLpmIpv4McEntriesFlush(lpmDBId, vrIdValue);
                if(status != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
            else
            {
                if (status != GT_NOT_FOUND)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, status, "%d", -1);
                    return CMD_OK;
                }
            }
        }
    }
    else
    {
        tcamShadow = (PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC*)(lpmDbPtr->shadow);
        while ((vrEntryPtr = prvCpssSlGetNext(tcamShadow->vrSl,&vrIterator)) != NULL)
        {
            vrIdValue = vrEntryPtr->vrId;
            /* call cpss api function */
            status =  cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrIdValue);

            /* pack output arguments to galtis string */
            galtisOutput(outArgs, status, "");
        }
    }

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6McEntriesFlush
*
* DESCRIPTION:
*   Flushes the multicast IP Routing table and stays with the default entry
*   only for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId     - The LPM DB id.
*       vrId        - The virtual router identifier.
*                     future support only, currently only vrId = 0 is supported.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntriesFlushVr
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                       status;
    GT_U32                          lpmDBId;
    GT_U32                          vrId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    status =  cpssDxChIpLpmIpv6McEntriesFlush(lpmDBId, vrId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - The next ip Group address.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The next ip Source address.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*       mcRouteLttEntryPtr  - the LTT entry pointer pointing to the MC route
*                             entry associated with this MC route.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntryGetNext_GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                              lpmDBId;
    CPSS_DXCH_IP_LTT_ENTRY_STC          mcRouteLttEntry;
    GT_U32                              gRowIndex;
    GT_U32                              sRowIndex;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT    shadowType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    gIpv6McInd = 0;
    vrId = 0;/*currently only 0 is valid*/
    cmdOsMemSet(&gIpv6McGroup, 0, sizeof(gIpv6McGroup));
    gIpv6McGroup.arIP[0] = 0xFF;/*for first entry 0xff is entered*/
    gIpv6McGroupPrefixLen = 8;
    gIpv6McSrc.u32Ip[0] = 0; /*for first entry 0 is entered*/
    gIpv6McSrc.u32Ip[1] = 0; /*for first entry 0 is entered*/
    gIpv6McSrc.u32Ip[2] = 0; /*for first entry 0 is entered*/
    gIpv6McSrc.u32Ip[3] = 0; /*for first entry 0 is entered*/
    gIpv6McSrcPrefixLen = 0; /*for first entry 0 is entered*/

    isDefaultVrfIdEntry = GT_FALSE;

    result = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* allocate the virtual routers skip list*/
    switch (shadowType)
    {
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E:
        maxNumOfVr = DXCH_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E:
        maxNumOfVr = DXCH2_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E:
        maxNumOfVr = DXCH3_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E:
        maxNumOfVr = DXCH_XCAT_MAX_VR_ID_CNS;
        break;
    case CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E:
        maxNumOfVr = DXCH_LION3_MAX_VR_ID_CNS;
        break;
    default:
        return GT_FAIL;
    }

    for( ; vrId < maxNumOfVr ; vrId++)
    {
        /* call cpss api function */
        result = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, gIpv6McGroup,
                                             gIpv6McGroupPrefixLen,
                                             gIpv6McSrc, gIpv6McSrcPrefixLen,
                                             &mcRouteLttEntry, &gRowIndex, &sRowIndex);
        /* result is GT_NOT_FOUND if the virtual router doesn't exist,
           or GT_NOT_INITIALIZED if the virtual router is not valid */
        if ((result == GT_NOT_FOUND)||
            (result == GT_NOT_INITIALIZED))
            continue;
        else
        {
            lastVrfIdTreated = vrId;
            break;
        }
    }

    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    /* pack and output table fields*/
    inFields[0] = gIpv6McInd;
    inFields[1] = vrId;
    inFields[2] = (GT_UINTPTR)gIpv6McGroup.arIP;
    inFields[3] = gIpv6McGroupPrefixLen;
    inFields[4] = (GT_UINTPTR)gIpv6McSrc.arIP;
    inFields[5] = gIpv6McSrcPrefixLen;
    inFields[6] = mcRouteLttEntry.routeType;
    inFields[7] = mcRouteLttEntry.numOfPaths;
    inFields[8] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[9] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[10] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[11] = mcRouteLttEntry.ipv6MCGroupScopeLevel;

    /* inFields[12] is the override field. It has only meaning in the set, but
       we should display it also in the get, else the command wrapper will fail*/
    inFields[12] = 0;
    inFields[13] = 0;


    /* pack and output table fields */
    fieldOutput("%d%d%16B%d%16B%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12],inFields[13]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIpv6McInd++;

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr          - The next ip Group address.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The next ip Source address.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*       mcRouteLttEntryPtr  - the LTT entry pointer pointing to the MC route
*                             entry associated with this MC route.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntryGetNext_GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;

    GT_U32                          lpmDBId;
    CPSS_DXCH_IP_LTT_ENTRY_STC      mcRouteLttEntry;
    GT_U32                          gRowIndex;
    GT_U32                          sRowIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* reset mcRouteLttEntry*/
    cmdOsMemSet(&mcRouteLttEntry, 0, sizeof(mcRouteLttEntry));


    if(vrId != lastVrfIdTreated)
    {
        cmdOsMemSet(&gIpv6McGroup, 0, sizeof(gIpv6McGroup));
        gIpv6McGroup.arIP[0] = 0xFF;/*for first entry 0xff is entered*/
        gIpv6McGroupPrefixLen = 8;
        gIpv6McSrc.u32Ip[0] = 0; /*for first entry 0 is entered*/
        gIpv6McSrc.u32Ip[1] = 0; /*for first entry 0 is entered*/
        gIpv6McSrc.u32Ip[2] = 0; /*for first entry 0 is entered*/
        gIpv6McSrc.u32Ip[3] = 0; /*for first entry 0 is entered*/
        gIpv6McSrcPrefixLen = 0; /*for first entry 0 is entered*/

        for( ; vrId < maxNumOfVr ; vrId++)
        {
             /* call cpss api function */
            result = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, gIpv6McGroup,
                                             gIpv6McGroupPrefixLen,
                                             gIpv6McSrc, gIpv6McSrcPrefixLen,
                                             &mcRouteLttEntry, &gRowIndex, &sRowIndex);

            /* result is GT_NOT_FOUND if the virtual router doesn't exist,
               or GT_NOT_INITIALIZED if the virtual router is not valid */
            if ((result == GT_NOT_FOUND)||
                (result == GT_NOT_INITIALIZED))
                continue;
            else
            {
                lastVrfIdTreated = vrId;
                isDefaultVrfIdEntry = GT_TRUE;
                break;
            }
        }
    }

    /* all virtual routers were scanned */
    if (vrId == maxNumOfVr)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(isDefaultVrfIdEntry == GT_TRUE)
    {
        /* The first default entry was already treated in
           wrCpssDxChIpLpmIpv6McEntryGetNext_GetFirst or in previuos call
           to this wrapper     */

        prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

        /* pack and output table fields*/
        inFields[0] = gIpv6McInd;
        inFields[1] = vrId;
        inFields[2] = (GT_UINTPTR)gIpv6McGroup.arIP;
        inFields[3] = gIpv6McGroupPrefixLen;
        inFields[4] = (GT_UINTPTR)gIpv6McSrc.arIP;
        inFields[5] = gIpv6McSrcPrefixLen;
        inFields[6] = mcRouteLttEntry.routeType;
        inFields[7] = mcRouteLttEntry.numOfPaths;
        inFields[8] = mcRouteLttEntry.routeEntryBaseIndex;
        inFields[9] = mcRouteLttEntry.ucRPFCheckEnable;
        inFields[10] = mcRouteLttEntry.sipSaCheckMismatchEnable;
        inFields[11] = mcRouteLttEntry.ipv6MCGroupScopeLevel;

        /* inFields[12] is the override field. It has only meaning in the set, but
           we should display it also in the get, else the command wrapper will fail*/
        inFields[12] = 0;
        inFields[13] = 0;


        /* pack and output table fields */
        fieldOutput("%d%d%16B%d%16B%d%d%d%d%d%d%d%d%d"
                    ,inFields[0], inFields[1], inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6], inFields[7],
                    inFields[8], inFields[9], inFields[10], inFields[11],
                    inFields[12],inFields[13]);

        galtisOutput(outArgs, GT_OK, "%f");

        gIpv6McInd++;
        isDefaultVrfIdEntry = GT_FALSE;

        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &gIpv6McGroup,
                                             &gIpv6McGroupPrefixLen,
                                             &gIpv6McSrc, &gIpv6McSrcPrefixLen,
                                             &mcRouteLttEntry, &gRowIndex, &sRowIndex);
    if (result == GT_NOT_FOUND)
    {
        vrId++;

        return wrCpssDxChIpLpmIpv6McEntryGetNext_GetNext(inArgs,inFields,numFields,outArgs);
    }

   /* The first default entry was already treated in
       wrCpssDxChIpLpmIpv6McEntryGetNext_GetFirst or in previuos call
       to this wrapper     */

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    /* pack and output table fields*/
    inFields[0] = gIpv6McInd;
    inFields[1] = vrId;
    inFields[2] = (GT_UINTPTR)gIpv6McGroup.arIP;
    inFields[3] = gIpv6McGroupPrefixLen;
    inFields[4] = (GT_UINTPTR)gIpv6McSrc.arIP;
    inFields[5] = gIpv6McSrcPrefixLen;
    inFields[6] = mcRouteLttEntry.routeType;
    inFields[7] = mcRouteLttEntry.numOfPaths;
    inFields[8] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[9] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[10] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[11] = mcRouteLttEntry.ipv6MCGroupScopeLevel;

    /* inFields[12] is the override field. It has only meaning in the set, but
       we should display it also in the get, else the command wrapper will fail*/
    inFields[12] = 0;
    inFields[13] = 0;


    /* pack and output table fields */
    fieldOutput("%d%d%16B%d%16B%d%d%d%d%d%d%d%d%d"
                ,inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12],inFields[13]);

    galtisOutput(outArgs, GT_OK, "%f");

    gIpv6McInd++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*                          future support only , currently only vrId = 0 is
*                          supported.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr - the LTT entry pointer pointing to the MC route
*                            entry associated with this MC route.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntrySearch
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_U32                                   ipGroupPrefixLen;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC               mcRouteLttEntry;
    GT_U32                                   gRowIndex;
    GT_U32                                   sRowIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inArgs[2]);
    ipGroupPrefixLen = (GT_U32)inArgs[3];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inArgs[4]);
    ipSrcPrefixLen = (GT_U32)inArgs[5];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup,
                                             ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry,
                                             &gRowIndex, &sRowIndex);

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d",
                 mcRouteLttEntry.routeType,
                 mcRouteLttEntry.numOfPaths,
                 mcRouteLttEntry.routeEntryBaseIndex,
                 mcRouteLttEntry.ucRPFCheckEnable,
                 mcRouteLttEntry.sipSaCheckMismatchEnable,
                 mcRouteLttEntry.ipv6MCGroupScopeLevel);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmDBMemSizeGet
*
* DESCRIPTION:
*   This function gets the memory size needed to export the Lpm DB,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*
* OUTPUTS:
*       lpmDbSizePtr  - the table size calculated (in bytes)
*
* RETURNS:
*       GT_OK on success
*       GT_FAIL otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBMemSizeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32 lpmDBId;
    GT_U32 lpmDbSize = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    status =  cpssDxChIpLpmDBMemSizeGet(lpmDBId, &lpmDbSize);

    galtisOutput(outArgs, status, "%d", lpmDbSize);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBExport
*
* DESCRIPTION:
*   This function exports the Lpm DB into the preallocated memory,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - pointer to block data size supposed to be exported.
*                               in current iteration.
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - pointer to block data size that was not used.
*                               in current iteration.
*       lpmDbMemBlockPtr      - pointer to allocated for lpm DB memory area.
*       iterPtr               - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBExport
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32 lpmDBId;
    GT_U32 lpmDb;
    GT_U32 exportSize;
    GT_UINTPTR iter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    exportSize = (GT_U32)inArgs[1];
    iter = (GT_U32)inArgs[2];

    /* call cpss api function */
    status =  cpssDxChIpLpmDBExport(lpmDBId, &lpmDb, &exportSize, &iter);

    galtisOutput(outArgs, status, "%d%d", lpmDb, iter);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBImport
*
* DESCRIPTION:
*   This function imports the Lpm DB recived and reconstruct it,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - pointer to block data size supposed to be imported.
*                               in current iteration.
*       lpmDbMemBlockPtr      - pointer to allocated for lpm DB memory area.
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - pointer to block data size that was not used.
*                               in current iteration.
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBImport
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS   status;

    GT_U32 lpmDBId;
    GT_U32 lpmDb;
    GT_U32 importSize;
    GT_UINTPTR iter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    lpmDb = (GT_U32)inArgs[1];
    importSize = (GT_U32)inArgs[2];
    iter = (GT_U32)inArgs[3];

    /* call cpss api function */
    status =  cpssDxChIpLpmDBImport(lpmDBId, &lpmDb, &importSize, &iter);

    galtisOutput(outArgs, status, "%d%d", lpmDb, iter);

    return CMD_OK;
}

static GT_U32  *lpmDbDataPtr = NULL;

/*******************************************************************************
* cpssDxChIpLpmDBExport
*
* DESCRIPTION:
*   This function exports the Lpm DB into the preallocated memory,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - pointer to block data size supposed to be exported.
*                               in current iteration.
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - pointer to block data size that was not used.
*                               in current iteration.
*       lpmDbMemBlockPtr      - pointer to allocated for lpm DB memory area.
*       iterPtr               - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBSimpleExport
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    GT_U32      lpmDBId;
    GT_U32      lpmDbSize;
    GT_U32      exportSize;
    GT_U32      tempExportSize;
    GT_UINTPTR  iter;
    GT_U32      numOfIterations;
    GT_BOOL     isMultipleIterations;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId              = (GT_U32)inArgs[0];
    isMultipleIterations = (GT_BOOL)inArgs[1];

    if (isMultipleIterations == GT_TRUE)
        exportSize = (GT_U32)inArgs[2];
    else
        exportSize = 0xFFFFFFFF;

    /* get the LPM data size */
    iter = 0;
    status = cpssDxChIpLpmDBMemSizeGet(lpmDBId,&lpmDbSize);
    if (status != GT_OK)
    {
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    cmdOsPrintf("LPM DB data size: %d\n", lpmDbSize);

    /* allocate memory for the LPM DB data, if previous memory is allocated, it will be free
       note that lpmDbSize is in bytes while lpmDbDataPtr is pointer to words array */
    if (lpmDbDataPtr != NULL)
    {
        cmdOsFree(lpmDbDataPtr);
    }
    lpmDbDataPtr = cmdOsMalloc((lpmDbSize / 4) * sizeof(GT_U32));
    if (lpmDbDataPtr == NULL)
    {
        cmdOsPrintf("Error, not enough memory for IP LPM data\n");
        status = GT_OUT_OF_CPU_MEM;
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    /* now do the actual export, count number of iterations during the export */
    numOfIterations = 0;
    iter = 0;
    tempExportSize = exportSize;
    do
    {
        numOfIterations++;
        status = cpssDxChIpLpmDBExport(lpmDBId,lpmDbDataPtr,&exportSize,&iter);
        if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }
        exportSize = tempExportSize + exportSize;
    } while (iter != 0);

    cmdOsPrintf("Number of iterations used: %d\n",numOfIterations);

    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBImport
*
* DESCRIPTION:
*   This function imports the Lpm DB recived and reconstruct it,
*   (used for HSU and unit Hotsync)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId               - The LPM DB id.
*       lpmDbMemBlockSizePtr  - pointer to block data size supposed to be imported.
*                               in current iteration.
*       lpmDbMemBlockPtr      - pointer to allocated for lpm DB memory area.
*       iterPtr               - the iterator, to start - set to 0.
*
* OUTPUTS:
*       lpmDbMemBlockSizePtr  - pointer to block data size that was not used.
*                               in current iteration.
*       iterPtr   - the iterator, if = 0 then the operation is done.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBSimpleImport
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   status;
    GT_U32      lpmDBId;
    GT_U32      importSize;
    GT_U32      tempImportSize;
    GT_UINTPTR  iter;
    GT_U32      numOfIterations;
    GT_BOOL     isMultipleIterations;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId              = (GT_U32)inArgs[0];
    isMultipleIterations = (GT_BOOL)inArgs[1];

    if (isMultipleIterations == GT_TRUE)
        importSize = (GT_U32)inArgs[2];
    else
        importSize = 0xFFFFFFFF;

    if (lpmDbDataPtr == NULL)
    {
        cmdOsPrintf("Error: no previously export LPM DB data found\n");
        status = GT_NOT_FOUND;
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    /* now do the actual import, count number of iterations during the import */
    numOfIterations = 0;
    iter = 0;
    tempImportSize = importSize;
    do
    {
        numOfIterations++;
        status = cpssDxChIpLpmDBImport(lpmDBId,lpmDbDataPtr,&importSize,&iter);
        if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }
        importSize = tempImportSize + importSize;
    } while (iter != 0);

    /* free LPM DB data after successful import */
    cmdOsFree(lpmDbDataPtr);
    lpmDbDataPtr = NULL;

    cmdOsPrintf("Number of iterations used: %d\n",numOfIterations);

    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChIpLpmDump
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8  devNum;
    GT_U32  vrId;
    CPSS_IP_PROTOCOL_STACK_ENT       protocol;
    CPSS_UNICAST_MULTICAST_ENT       prefixType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    protocol = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    prefixType = (CPSS_UNICAST_MULTICAST_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpLpmDump(devNum, vrId, protocol, prefixType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vrId);

    return CMD_OK;
}


static GT_IPADDR nextEntry;
static GT_U32    nextEntryPrefixLen;
static GT_IPADDR srcNextEntry;
static GT_U32    srcNextEntryPrefixLen;
static GT_U32    index;

/*******************************************************************************
* wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*                      future support only , currently only vrId = 0 is
*                      supported.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*       tcamColumnIndexptr   - TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;
    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_U32                                    tcamRowIndex;
    GT_U32                                    tcamColumnIndex;
    CPSS_DXCH_IP_LTT_ENTRY_STC                *lttPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];


    lttPtr=&(nextHopInfo.ipLttEntry);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId,vrId, &nextEntry,&nextEntryPrefixLen,
                                             &nextHopInfo,&tcamRowIndex,
                                             &tcamColumnIndex);

    /*no more entries to get*/
    if(result==GT_NOT_FOUND)
      {
       galtisOutput(outArgs, GT_OK, "%d", -1);
       return CMD_OK;
    }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, lttPtr);

    inFields[0]=index++;
    inFields[2]=nextEntryPrefixLen;
    inFields[3]=lttPtr->routeType;
    inFields[4]=lttPtr->numOfPaths;
    inFields[5]=lttPtr->routeEntryBaseIndex;
    inFields[6]=lttPtr->ucRPFCheckEnable;
    inFields[7]=lttPtr->sipSaCheckMismatchEnable;
    inFields[8]=lttPtr->ipv6MCGroupScopeLevel;
    inFields[9]=tcamRowIndex;
    inFields[10]=tcamColumnIndex;


     /* pack and output table fields */
    fieldOutput("%d%4b%d%d%d%d%d%d%d%d%d",
                inFields[0],nextEntry.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10]);
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetFirst
*
* DESCRIPTION:
*   This wrapper searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - if found, this is the route entry info.
*                               accosiated with this UC prefix.
*       tcamRowIndexPtr      - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex;
    GT_U32                                  tcamColumnIndex;
    CPSS_DXCH_IP_LTT_ENTRY_STC              *lttPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen= (GT_U32)inArgs[3];

    index=0;
    lttPtr=&(nextHopInfo.ipLttEntry);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId,vrId, ipAddr,prefixLen,
                                             &nextHopInfo,&tcamRowIndex,
                                             &tcamColumnIndex);
    /*save adress and length for get next*/
    nextEntry.u32Ip=ipAddr.u32Ip;
    nextEntryPrefixLen=prefixLen;

    /*if entry isn't found go to get next*/
    if(result==GT_NOT_FOUND)
      {
       return wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetNext(inArgs,
                                                        inFields,
                                                        numFields,
                                                        outArgs);
      }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, lttPtr);

    inFields[0]=index++;
    inFields[2]=prefixLen;
    inFields[3]=lttPtr->routeType;
    inFields[4]=lttPtr->numOfPaths;
    inFields[5]=lttPtr->routeEntryBaseIndex;
    inFields[6]=lttPtr->ucRPFCheckEnable;
    inFields[7]=lttPtr->sipSaCheckMismatchEnable;
    inFields[8]=lttPtr->ipv6MCGroupScopeLevel;
    inFields[9]=tcamRowIndex;
    inFields[10]=tcamColumnIndex;


     /* pack and output table fields */
    fieldOutput("%d%4b%d%d%d%d%d%d%d%d%d",
                inFields[0],ipAddr.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*                      future support only , currently only vrId = 0 is
*                      supported.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*       tcamColumnIndexptr   - TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                                 result;
    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_U32                                    tcamRowIndex;
    GT_U32                                    tcamColumnIndex;
    CPSS_DXCH_PCL_ACTION_STC                  *pclPtr;
    GT_HW_DEV_NUM                             hwDev; /* Dummy for converting. */
    GT_PORT_NUM                               port;  /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];


    pclPtr=&(nextHopInfo.pclIpUcAction);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId,vrId, &nextEntry,&nextEntryPrefixLen,
                                             &nextHopInfo,&tcamRowIndex,
                                             &tcamColumnIndex);

    /*no more entries to get*/
    if(result==GT_NOT_FOUND)
      {
       galtisOutput(outArgs, GT_OK, "%d", -1);
       return CMD_OK;
    }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }
    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    inFields[0]=index++;
    inFields[2]=nextEntryPrefixLen;
    inFields[3]=pclPtr->pktCmd;
    inFields[4]=pclPtr->actionStop;
    inFields[5]=pclPtr->egressPolicy;

    /*mirror*/
    inFields[6]=pclPtr->mirror.cpuCode;
    inFields[7]=pclPtr->mirror.mirrorToRxAnalyzerPort;

    /*match counter*/
    inFields[8]=pclPtr->matchCounter.enableMatchCount;
    inFields[9]=pclPtr->matchCounter.matchCounterIndex;

    /*qos*/
    if(pclPtr->egressPolicy != GT_FALSE)
    {
        inFields[10] =
            (pclPtr->qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[11] =
            (pclPtr->qos.egress.modifyUp ==
                CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[12]=pclPtr->qos.egress.dscp;
        inFields[13]=pclPtr->qos.egress.up;
    }
    else
    {
        inFields[10]=pclPtr->qos.ingress.modifyDscp;
        inFields[11]=pclPtr->qos.ingress.modifyUp;
        inFields[14]=pclPtr->qos.ingress.profileIndex;
        inFields[15]=pclPtr->qos.ingress.profileAssignIndex;
        inFields[16]=pclPtr->qos.ingress.profilePrecedence;
    }

     /*redirect*/
    inFields[17]=pclPtr->redirect.redirectCmd;

    if(pclPtr->redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        inFields[18]=pclPtr->redirect.data.outIf.outInterface.type;
        hwDev =pclPtr->redirect.data.outIf.outInterface.devPort.hwDevNum;
        port=pclPtr->redirect.data.outIf.outInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[19] = hwDev;
        inFields[20] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(pclPtr->redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[21]=pclPtr->redirect.data.outIf.outInterface.trunkId;
        inFields[22]=pclPtr->redirect.data.outIf.outInterface.vidx;
        inFields[23]=pclPtr->redirect.data.outIf.outInterface.vlanId;
        inFields[24]=pclPtr->redirect.data.outIf.vntL2Echo;
        inFields[25]=pclPtr->redirect.data.outIf.tunnelStart;
        inFields[26]=pclPtr->redirect.data.outIf.tunnelPtr;
        inFields[27]=pclPtr->redirect.data.outIf.tunnelType;
        inFields[28]=0 /*not used */;
    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        inFields[29]=pclPtr->redirect.data.routerLttIndex;

    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E)
    {
        inFields[30]=pclPtr->redirect.data.vrfId;

    }
    /* policer */
    inFields[31]=pclPtr->policer.policerEnable;
    inFields[32]=pclPtr->policer.policerId;

    /*vlan*/
    if (pclPtr->egressPolicy == GT_FALSE)
    {
        inFields[33] = pclPtr->vlan.ingress.modifyVlan;
        inFields[34] = pclPtr->vlan.ingress.nestedVlan;
        inFields[35] = pclPtr->vlan.ingress.vlanId;
        inFields[36] = pclPtr->vlan.ingress.precedence;
    }
    else
    {
        inFields[33] =
            (pclPtr->vlan.egress.vlanCmd ==
             CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
        inFields[35] = pclPtr->vlan.egress.vlanId;
    }

    /*ipUcRoute*/
    inFields[37]=pclPtr->ipUcRoute.doIpUcRoute;
    inFields[38]=pclPtr->ipUcRoute.arpDaIndex;
    inFields[39]=pclPtr->ipUcRoute.decrementTTL;
    inFields[40]=pclPtr->ipUcRoute.bypassTTLCheck;
    inFields[41]=pclPtr->ipUcRoute.icmpRedirectCheck;

    /*sourece id*/
    inFields[42]=pclPtr->sourceId.assignSourceId;
    inFields[43]=pclPtr->sourceId.sourceIdValue;

    inFields[44]=tcamRowIndex;
    inFields[45]=tcamColumnIndex;


     /* pack and output table fields*/
    fieldOutput("%d%4b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],nextEntry.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],
                inFields[12],inFields[13],inFields[14],inFields[15],
                inFields[16],inFields[17],inFields[18],inFields[19],
                inFields[20],inFields[21],inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27],
                inFields[28],inFields[29],inFields[30],inFields[31],
                inFields[32],inFields[33],inFields[34],inFields[35],
                inFields[36],inFields[37],inFields[38],inFields[39],
                inFields[40],inFields[41],inFields[42],inFields[43],
                inFields[44],inFields[45]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetFirst
*
* DESCRIPTION:
*   This wrapper searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - if found, this is the route entry info.
*                               accosiated with this UC prefix.
*       tcamRowIndexPtr      - if found, TCAM row index of this uc prefix.
*       tcamColumnIndexPtr   - if found, TCAM column index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex;
    GT_U32                                  tcamColumnIndex;
    CPSS_DXCH_PCL_ACTION_STC                *pclPtr;
    GT_HW_DEV_NUM                           hwDev; /* Dummy for converting. */
    GT_PORT_NUM                             port;  /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen= (GT_U32)inArgs[3];

    index=0;
    pclPtr=&(nextHopInfo.pclIpUcAction);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId,vrId, ipAddr,prefixLen,
                                             &nextHopInfo,&tcamRowIndex,
                                             &tcamColumnIndex);
    /*save adress and length for get next*/
    nextEntry.u32Ip=ipAddr.u32Ip;
    nextEntryPrefixLen=prefixLen;

    /*if entry isn't found go to get next*/
    if(result==GT_NOT_FOUND)
      {
       return wrCpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetNext(inArgs,
                                                        inFields,
                                                        numFields,
                                                        outArgs);
      }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    inFields[0]=index++;
    inFields[2]=nextEntryPrefixLen;
    inFields[3]=pclPtr->pktCmd;
    inFields[4]=pclPtr->actionStop;
    inFields[5]=pclPtr->egressPolicy;

    /*mirror*/
    inFields[6]=pclPtr->mirror.cpuCode;
    inFields[7]=pclPtr->mirror.mirrorToRxAnalyzerPort;

    /*match counter*/
    inFields[8]=pclPtr->matchCounter.enableMatchCount;
    inFields[9]=pclPtr->matchCounter.matchCounterIndex;

    /*qos*/
    if(pclPtr->egressPolicy != GT_FALSE)
    {
        inFields[10] =
            (pclPtr->qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[11] =
            (pclPtr->qos.egress.modifyUp ==
                CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[12]=pclPtr->qos.egress.dscp;
        inFields[13]=pclPtr->qos.egress.up;
    }
    else
    {
        inFields[10]=pclPtr->qos.ingress.modifyDscp;
        inFields[11]=pclPtr->qos.ingress.modifyUp;
        inFields[14]=pclPtr->qos.ingress.profileIndex;
        inFields[15]=pclPtr->qos.ingress.profileAssignIndex;
        inFields[16]=pclPtr->qos.ingress.profilePrecedence;
    }

     /*redirect*/
    inFields[17]=pclPtr->redirect.redirectCmd;


    if(pclPtr->redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        inFields[18]=pclPtr->redirect.data.outIf.outInterface.type;
        hwDev=pclPtr->redirect.data.outIf.outInterface.devPort.hwDevNum;
        port=pclPtr->redirect.data.outIf.outInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[19] = hwDev;
        inFields[20] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(pclPtr->redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[21]=pclPtr->redirect.data.outIf.outInterface.trunkId;
        inFields[22]=pclPtr->redirect.data.outIf.outInterface.vidx;
        inFields[23]=pclPtr->redirect.data.outIf.outInterface.vlanId;
        inFields[24]=pclPtr->redirect.data.outIf.vntL2Echo;
        inFields[25]=pclPtr->redirect.data.outIf.tunnelStart;
        inFields[26]=pclPtr->redirect.data.outIf.tunnelPtr;
        inFields[27]=pclPtr->redirect.data.outIf.tunnelType;
        inFields[28]=0 /*not used */;
    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        inFields[29]=pclPtr->redirect.data.routerLttIndex;

    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E)
    {
        inFields[30]=pclPtr->redirect.data.vrfId;

    }
    /* policer */
    inFields[31]=pclPtr->policer.policerEnable;
    inFields[32]=pclPtr->policer.policerId;

    /*vlan*/
    if (pclPtr->egressPolicy == GT_FALSE)
    {
        inFields[33] = pclPtr->vlan.ingress.modifyVlan;
        inFields[34] = pclPtr->vlan.ingress.nestedVlan;
        inFields[35] = pclPtr->vlan.ingress.vlanId;
        inFields[36] = pclPtr->vlan.ingress.precedence;
    }
    else
    {
        inFields[33] =
            (pclPtr->vlan.egress.vlanCmd ==
             CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
        inFields[35] = pclPtr->vlan.egress.vlanId;
    }

    /*ipUcRoute*/
    inFields[37]=pclPtr->ipUcRoute.doIpUcRoute;
    inFields[38]=pclPtr->ipUcRoute.arpDaIndex;
    inFields[39]=pclPtr->ipUcRoute.decrementTTL;
    inFields[40]=pclPtr->ipUcRoute.bypassTTLCheck;
    inFields[41]=pclPtr->ipUcRoute.icmpRedirectCheck;

    /*sourece id*/
    inFields[42]=pclPtr->sourceId.assignSourceId;
    inFields[43]=pclPtr->sourceId.sourceIdValue;

    inFields[44]=tcamRowIndex;
    inFields[45]=tcamColumnIndex;


     /* pack and output table fields*/
    fieldOutput("%d%4b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],nextEntry.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],
                inFields[12],inFields[13],inFields[14],inFields[15],
                inFields[16],inFields[17],inFields[18],inFields[19],
                inFields[20],inFields[21],inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27],
                inFields[28],inFields[29],inFields[30],inFields[31],
                inFields[32],inFields[33],inFields[34],inFields[35],
                inFields[36],inFields[37],inFields[38],inFields[39],
                inFields[40],inFields[41],inFields[42],inFields[43],
                inFields[44],inFields[45]);

    galtisOutput(outArgs, result, "%f");


    return CMD_OK;

}
/*****************************ipv6 lpm uc tables**********************************/

static GT_IPV6ADDR nextEntry_ipv6;
static GT_IPV6ADDR srcNextEntry_ipv6;

/*******************************************************************************
* wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.
*                      future support only , currently only vrId = 0 is
*                      supported.
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                                 result;
    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_U32                                    tcamRowIndex;
    GT_U32                                    tcamColumnIndex;
    CPSS_DXCH_IP_LTT_ENTRY_STC                *lttPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];


    lttPtr=&(nextHopInfo.ipLttEntry);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId,vrId, &nextEntry_ipv6,&nextEntryPrefixLen,
                                             &nextHopInfo,&tcamRowIndex,&tcamColumnIndex);


    /*no more entries to get*/
    if(result==GT_NOT_FOUND)
      {
       galtisOutput(outArgs, GT_OK, "%d", -1);
       return CMD_OK;
    }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, lttPtr);

    inFields[0]=index++;
    inFields[2]=nextEntryPrefixLen;
    inFields[3]=lttPtr->routeType;
    inFields[4]=lttPtr->numOfPaths;
    inFields[5]=lttPtr->routeEntryBaseIndex;
    inFields[6]=lttPtr->ucRPFCheckEnable;
    inFields[7]=lttPtr->sipSaCheckMismatchEnable;
    inFields[8]=lttPtr->ipv6MCGroupScopeLevel;
    inFields[9]=tcamRowIndex;



     /* pack and output table fields */
    fieldOutput("%d%16B%d%d%d%d%d%d%d%d",
                inFields[0],nextEntry_ipv6.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9]);
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetFirst
*
* DESCRIPTION:
*   This wrapper searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If  found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - if found, TCAM row index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_IPV6ADDR                             ipAddr;
    GT_U32                                  prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex;
    GT_U32                                  tcamColumnIndex;
    CPSS_DXCH_IP_LTT_ENTRY_STC              *lttPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen= (GT_U32)inArgs[3];

    index=0;
    lttPtr=&(nextHopInfo.ipLttEntry);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId,vrId, ipAddr,prefixLen,
                                             &nextHopInfo,&tcamRowIndex,&tcamColumnIndex);

    /*save adress and length for get next*/
    nextEntry_ipv6.u32Ip[0]=ipAddr.u32Ip[0];
    nextEntry_ipv6.u32Ip[1]=ipAddr.u32Ip[1];
    nextEntry_ipv6.u32Ip[2]=ipAddr.u32Ip[2];
    nextEntry_ipv6.u32Ip[3]=ipAddr.u32Ip[3];

    nextEntryPrefixLen=prefixLen;

    /*if entry isn't found go to get next*/
    if(result==GT_NOT_FOUND)
      {
       return wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetNext(inArgs,
                                                        inFields,
                                                        numFields,
                                                        outArgs);
      }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, lttPtr);

    inFields[0]=index++;
    inFields[2]=prefixLen;
    inFields[3]=lttPtr->routeType;
    inFields[4]=lttPtr->numOfPaths;
    inFields[5]=lttPtr->routeEntryBaseIndex;
    inFields[6]=lttPtr->ucRPFCheckEnable;
    inFields[7]=lttPtr->sipSaCheckMismatchEnable;
    inFields[8]=lttPtr->ipv6MCGroupScopeLevel;
    inFields[9]=tcamRowIndex;


     /* pack and output table fields */
    fieldOutput("%d%16B%d%d%d%d%d%d%d%d",
                inFields[0],ipAddr.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one and prefix Tcam index; it used for iterating over the exisiting
*   prefixes.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.
*                       future support only , currently only vrId = 0 is
*                       supported.
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - If found,TCAM row index of this uc prefix.
*
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*************************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                                 result;
    GT_U32                                    lpmDBId;
    GT_U32                                    vrId;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT    nextHopInfo;
    GT_U32                                    tcamRowIndex;
    GT_U32                                    tcamColumnIndex;
    CPSS_DXCH_PCL_ACTION_STC                  *pclPtr;
    GT_HW_DEV_NUM                             hwDev; /* Dummy for converting. */
    GT_PORT_NUM                               port;  /* Dummy for converting. */


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];


    pclPtr=&(nextHopInfo.pclIpUcAction);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId,vrId, &nextEntry_ipv6,&nextEntryPrefixLen,
                                             &nextHopInfo,&tcamRowIndex,&tcamColumnIndex);


    /*no more entries to get*/
    if(result==GT_NOT_FOUND)
      {
       galtisOutput(outArgs, GT_OK, "%d", -1);
       return CMD_OK;
    }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }
    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    inFields[0]=index++;
    inFields[2]=nextEntryPrefixLen;
    inFields[3]=pclPtr->pktCmd;
    inFields[4]=pclPtr->actionStop;
    inFields[5]=pclPtr->egressPolicy;

    /*mirror*/
    inFields[6]=pclPtr->mirror.cpuCode;
    inFields[7]=pclPtr->mirror.mirrorToRxAnalyzerPort;

    /*match counter*/
    inFields[8]=pclPtr->matchCounter.enableMatchCount;
    inFields[9]=pclPtr->matchCounter.matchCounterIndex;

    /*qos*/
    if(pclPtr->egressPolicy != GT_FALSE)
    {
        inFields[10] =
            (pclPtr->qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[11] =
            (pclPtr->qos.egress.modifyUp ==
                CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[12]=pclPtr->qos.egress.dscp;
        inFields[13]=pclPtr->qos.egress.up;
    }
    else
    {
        inFields[10]=pclPtr->qos.ingress.modifyDscp;
        inFields[11]=pclPtr->qos.ingress.modifyUp;
        inFields[14]=pclPtr->qos.ingress.profileIndex;
        inFields[15]=pclPtr->qos.ingress.profileAssignIndex;
        inFields[16]=pclPtr->qos.ingress.profilePrecedence;
    }

     /*redirect*/
    inFields[17]=pclPtr->redirect.redirectCmd;

    if(pclPtr->redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        inFields[18]=pclPtr->redirect.data.outIf.outInterface.type;
        hwDev=pclPtr->redirect.data.outIf.outInterface.devPort.hwDevNum;
        port=pclPtr->redirect.data.outIf.outInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[19] = hwDev;
        inFields[20] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(pclPtr->redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[21]=pclPtr->redirect.data.outIf.outInterface.trunkId;
        inFields[22]=pclPtr->redirect.data.outIf.outInterface.vidx;
        inFields[23]=pclPtr->redirect.data.outIf.outInterface.vlanId;
        inFields[24]=pclPtr->redirect.data.outIf.vntL2Echo;
        inFields[25]=pclPtr->redirect.data.outIf.tunnelStart;
        inFields[26]=pclPtr->redirect.data.outIf.tunnelPtr;
        inFields[27]=pclPtr->redirect.data.outIf.tunnelType;
        inFields[28]=0 /*not used */;
    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        inFields[29]=pclPtr->redirect.data.routerLttIndex;

    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E)
    {
        inFields[30]=pclPtr->redirect.data.vrfId;

    }
    /* policer */
    inFields[31]=pclPtr->policer.policerEnable;
    inFields[32]=pclPtr->policer.policerId;

    /*vlan*/
    if (pclPtr->egressPolicy == GT_FALSE)
    {
        inFields[33] = pclPtr->vlan.ingress.modifyVlan;
        inFields[34] = pclPtr->vlan.ingress.nestedVlan;
        inFields[35] = pclPtr->vlan.ingress.vlanId;
        inFields[36] = pclPtr->vlan.ingress.precedence;
    }
    else
    {
        inFields[33] =
            (pclPtr->vlan.egress.vlanCmd ==
             CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
        inFields[35] = pclPtr->vlan.egress.vlanId;
    }

    /*ipUcRoute*/
    inFields[37]=pclPtr->ipUcRoute.doIpUcRoute;
    inFields[38]=pclPtr->ipUcRoute.arpDaIndex;
    inFields[39]=pclPtr->ipUcRoute.decrementTTL;
    inFields[40]=pclPtr->ipUcRoute.bypassTTLCheck;
    inFields[41]=pclPtr->ipUcRoute.icmpRedirectCheck;

    /*sourece id*/
    inFields[42]=pclPtr->sourceId.assignSourceId;
    inFields[43]=pclPtr->sourceId.sourceIdValue;

    inFields[44]=tcamRowIndex;



     /* pack and output table fields*/
    fieldOutput("%d%16B%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],nextEntry_ipv6.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],
                inFields[12],inFields[13],inFields[14],inFields[15],
                inFields[16],inFields[17],inFields[18],inFields[19],
                inFields[20],inFields[21],inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27],
                inFields[28],inFields[29],inFields[30],inFields[31],
                inFields[32],inFields[33],inFields[34],inFields[35],
                inFields[36],inFields[37],inFields[38],inFields[39],
                inFields[40],inFields[41],inFields[42],inFields[43],
                inFields[44],inFields[45]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetFirst
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.
*                         future support only , currently only vrId = 0 is
*                         supported.
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If  found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - if found, TCAM row index of this uc prefix.
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U32                                  lpmDBId;
    GT_U32                                  vrId;
    GT_IPV6ADDR                             ipAddr;
    GT_U32                                  prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex;
    GT_U32                                  tcamColumnIndex;
    CPSS_DXCH_PCL_ACTION_STC                *pclPtr;
    GT_HW_DEV_NUM                           hwDev; /* Dummy for converting. */
    GT_PORT_NUM                             port;  /* Dummy for converting. */


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen= (GT_U32)inArgs[3];

    index=0;
    pclPtr=&(nextHopInfo.pclIpUcAction);
    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId,vrId, ipAddr,prefixLen,
                                             &nextHopInfo,&tcamRowIndex,&tcamColumnIndex);

    /*save adress and length for get next*/
    nextEntry_ipv6.u32Ip[0]=ipAddr.u32Ip[0];
    nextEntry_ipv6.u32Ip[1]=ipAddr.u32Ip[1];
    nextEntry_ipv6.u32Ip[2]=ipAddr.u32Ip[2];
    nextEntry_ipv6.u32Ip[3]=ipAddr.u32Ip[3];

    nextEntryPrefixLen=prefixLen;

    /*if entry isn't found go to get next*/
    if(result==GT_NOT_FOUND)
      {
       return wrCpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetNext(inArgs,
                                                        inFields,
                                                        numFields,
                                                        outArgs);
      }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    cmdOsMemSet(inFields,0,CMD_MAX_FIELDS);

    inFields[0]=index++;
    inFields[2]=nextEntryPrefixLen;
    inFields[3]=pclPtr->pktCmd;
    inFields[4]=pclPtr->actionStop;
    inFields[5]=pclPtr->egressPolicy;

    /*mirror*/
    inFields[6]=pclPtr->mirror.cpuCode;
    inFields[7]=pclPtr->mirror.mirrorToRxAnalyzerPort;

    /*match counter*/
    inFields[8]=pclPtr->matchCounter.enableMatchCount;
    inFields[9]=pclPtr->matchCounter.matchCounterIndex;

    /*qos*/
    if(pclPtr->egressPolicy != GT_FALSE)
    {
        inFields[10] =
            (pclPtr->qos.egress.modifyDscp == CPSS_DXCH_PCL_ACTION_EGRESS_DSCP_EXP_CMD_KEEP_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[11] =
            (pclPtr->qos.egress.modifyUp ==
                CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_MODIFY_KEEP_PREVIOUS_E
                : CPSS_PACKET_ATTRIBUTE_MODIFY_ENABLE_E;
        inFields[12]=pclPtr->qos.egress.dscp;
        inFields[13]=pclPtr->qos.egress.up;
    }
    else
    {
        inFields[10]=pclPtr->qos.ingress.modifyDscp;
        inFields[11]=pclPtr->qos.ingress.modifyUp;
        inFields[14]=pclPtr->qos.ingress.profileIndex;
        inFields[15]=pclPtr->qos.ingress.profileAssignIndex;
        inFields[16]=pclPtr->qos.ingress.profilePrecedence;
    }

     /*redirect*/
    inFields[17]=pclPtr->redirect.redirectCmd;


    if(pclPtr->redirect.redirectCmd ==
                                    CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_OUT_IF_E)
    {
        inFields[18]=pclPtr->redirect.data.outIf.outInterface.type;
        hwDev=pclPtr->redirect.data.outIf.outInterface.devPort.hwDevNum;
        port=pclPtr->redirect.data.outIf.outInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(hwDev, port) ;
        inFields[19] = hwDev;
        inFields[20] = port;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(pclPtr->redirect.data.outIf.outInterface.
                                                                            trunkId);
        inFields[21]=pclPtr->redirect.data.outIf.outInterface.trunkId;
        inFields[22]=pclPtr->redirect.data.outIf.outInterface.vidx;
        inFields[23]=pclPtr->redirect.data.outIf.outInterface.vlanId;
        inFields[24]=pclPtr->redirect.data.outIf.vntL2Echo;
        inFields[25]=pclPtr->redirect.data.outIf.tunnelStart;
        inFields[26]=pclPtr->redirect.data.outIf.tunnelPtr;
        inFields[27]=pclPtr->redirect.data.outIf.tunnelType;
        inFields[28]=0 /*not used */;
    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_ROUTER_E)
    {
        inFields[29]=pclPtr->redirect.data.routerLttIndex;

    }
    if(pclPtr->redirect.redirectCmd
                                == CPSS_DXCH_PCL_ACTION_REDIRECT_CMD_VIRT_ROUTER_E)
    {
        inFields[30]=pclPtr->redirect.data.vrfId;

    }
    /* policer */
    inFields[31]=pclPtr->policer.policerEnable;
    inFields[32]=pclPtr->policer.policerId;

    /*vlan*/
    if (pclPtr->egressPolicy == GT_FALSE)
    {
        inFields[33] = pclPtr->vlan.ingress.modifyVlan;
        inFields[34] = pclPtr->vlan.ingress.nestedVlan;
        inFields[35] = pclPtr->vlan.ingress.vlanId;
        inFields[36] = pclPtr->vlan.ingress.precedence;
    }
    else
    {
        inFields[33] =
            (pclPtr->vlan.egress.vlanCmd ==
             CPSS_DXCH_PCL_ACTION_EGRESS_TAG0_CMD_DO_NOT_MODIFY_E)
                ? CPSS_PACKET_ATTRIBUTE_ASSIGN_DISABLED_E
                : CPSS_PACKET_ATTRIBUTE_ASSIGN_FOR_TAGGED_E;
        inFields[35] = pclPtr->vlan.egress.vlanId;
    }

    /*ipUcRoute*/
    inFields[37]=pclPtr->ipUcRoute.doIpUcRoute;
    inFields[38]=pclPtr->ipUcRoute.arpDaIndex;
    inFields[39]=pclPtr->ipUcRoute.decrementTTL;
    inFields[40]=pclPtr->ipUcRoute.bypassTTLCheck;
    inFields[41]=pclPtr->ipUcRoute.icmpRedirectCheck;

    /*sourece id*/
    inFields[42]=pclPtr->sourceId.assignSourceId;
    inFields[43]=pclPtr->sourceId.sourceIdValue;

    inFields[44]=tcamRowIndex;



     /* pack and output table fields*/
    fieldOutput("%d%16B%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],nextEntry_ipv6.arIP,inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],
                inFields[12],inFields[13],inFields[14],inFields[15],
                inFields[16],inFields[17],inFields[18],inFields[19],
                inFields[20],inFields[21],inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26],inFields[27],
                inFields[28],inFields[29],inFields[30],inFields[31],
                inFields[32],inFields[33],inFields[34],inFields[35],
                inFields[36],inFields[37],inFields[38],inFields[39],
                inFields[40],inFields[41],inFields[42],inFields[43],
                inFields[44]);

    galtisOutput(outArgs, result, "%f");


    return CMD_OK;

}

/*****************************ipv4 lpm mc tables**********************************/
GT_IPADDR                     nextIpGroup;
GT_IPADDR                     nextIpSrc;
GT_U32                        nextGroupPrefixLen;
GT_U32                        nextSrcPrefixLen;
/*******************************************************************************
* wrCpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - The next ip Group address.
*       ipGroupPrefixLenPtr        - the ipGroup prefix Length.
*       ipSrcPtr                   - The next ip Source address.
*       ipSrcPrefixLenPtr          - ipSrc prefix length.
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The value of ipGroupPtr must be a valid value, it
*           means that it exists in the IP-Mc Table, unless this is the first
*           call to this function, then it's value is 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                       result;
    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    CPSS_DXCH_IP_LTT_ENTRY_STC      mcRouteLttEntry;
    GT_U32                          tcamGroupRowIndex;
    GT_U32                          tcamGroupColumnIndex;
    GT_U32                          tcamSrcRowIndex;
    GT_U32                          tcamSrcColumnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];


      /* call cpss api function */
    result = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId,vrId, &nextIpGroup,&nextGroupPrefixLen,
                                             &nextIpSrc,&nextSrcPrefixLen,
                                             &mcRouteLttEntry,&tcamGroupRowIndex,
                                             &tcamGroupColumnIndex,&tcamSrcRowIndex,
                                             &tcamSrcColumnIndex);

    /*no more entries to get*/
    if(result==GT_NOT_FOUND)
      {
       galtisOutput(outArgs, GT_OK, "%d", -1);
       return CMD_OK;
    }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    inFields[0]=index++;
    inFields[2]=nextGroupPrefixLen;
    inFields[4]=nextSrcPrefixLen;

    inFields[5]=mcRouteLttEntry.routeType;
    inFields[6]=mcRouteLttEntry.numOfPaths;
    inFields[7]=mcRouteLttEntry.routeEntryBaseIndex;
    inFields[8]=mcRouteLttEntry.ucRPFCheckEnable;
    inFields[9]=mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[10]=mcRouteLttEntry.ipv6MCGroupScopeLevel;

    inFields[11]=tcamGroupRowIndex;
    inFields[12]=tcamGroupColumnIndex;
    inFields[13]=tcamSrcRowIndex;
    inFields[14]=tcamSrcColumnIndex;

     /* pack and output table fields */
    fieldOutput("%d%4b%d%4b%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],nextIpGroup.arIP,inFields[2],nextIpSrc.arIP,
                inFields[4], inFields[5], inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],
                inFields[12],inFields[13],inFields[14]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv4McEntryGetFirst
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*                          future support only , currently only vrId = 0 is
*                          supported.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    IN  GT_U32                                  lpmDBId;
    IN  GT_U32                                  vrId;
    IN  GT_IPADDR                               ipGroup;
    IN  GT_U32                                  ipGroupPrefixLen;
    IN  GT_IPADDR                               ipSrc;
    IN  GT_U32                                  ipSrcPrefixLen;

    OUT CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    OUT GT_U32                                  tcamGroupRowIndex;
    OUT GT_U32                                  tcamGroupColumnIndex;
    OUT GT_U32                                  tcamSrcRowIndex;
    OUT GT_U32                                  tcamSrcColumnIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inArgs[2]);
    ipGroupPrefixLen= (GT_U32)inArgs[3];
    galtisIpAddr(&ipSrc, (GT_U8*)inArgs[4]);
    ipSrcPrefixLen= (GT_U32)inArgs[5];

    index=0;

    /* if the prefixlen is 0 then the only reasonable posibility is to look for
       the default MC entry */
    if(ipGroupPrefixLen==0)
    {
        ipGroup.arIP[0] = 0xE0;
        ipGroup.arIP[1] = 0;
        ipGroup.arIP[2] = 0;
        ipGroup.arIP[3] = 0;
        ipGroupPrefixLen = 4;
    }
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId,vrId, ipGroup,ipGroupPrefixLen,
                                             ipSrc,ipSrcPrefixLen,
                                             &mcRouteLttEntry,&tcamGroupRowIndex,
                                             &tcamGroupColumnIndex,&tcamSrcRowIndex,
                                             &tcamSrcColumnIndex);
    /*save adress and length for get next*/
    nextIpGroup.u32Ip=ipGroup.u32Ip;
    nextGroupPrefixLen=ipGroupPrefixLen;
    nextIpSrc.u32Ip=ipSrc.u32Ip;
    nextSrcPrefixLen=ipSrcPrefixLen;

     /*if entry isn't found go to get next*/
    if(result==GT_NOT_FOUND)
    {
        return wrCpssDxChIpLpmIpv4McEntryGetNext(inArgs,
                                                 inFields,
                                                 numFields,
                                                 outArgs);
    }

    if(result!=GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    inFields[0]=index++;
    inFields[2]=nextGroupPrefixLen;
    inFields[4]=nextSrcPrefixLen;

    inFields[5]=mcRouteLttEntry.routeType;
    inFields[6]=mcRouteLttEntry.numOfPaths;
    inFields[7]=mcRouteLttEntry.routeEntryBaseIndex;
    inFields[8]=mcRouteLttEntry.ucRPFCheckEnable;
    inFields[9]=mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[10]=mcRouteLttEntry.ipv6MCGroupScopeLevel;

    inFields[11]=tcamGroupRowIndex;
    inFields[12]=tcamGroupColumnIndex;
    inFields[13]=tcamSrcRowIndex;
    inFields[14]=tcamSrcColumnIndex;

     /* pack and output table fields */
    fieldOutput("%d%4b%d%4b%d%d%d%d%d%d%d%d%d%d%d",
            inFields[0],nextIpGroup.arIP,inFields[2],nextIpSrc.arIP,
            inFields[4], inFields[5], inFields[6],inFields[7],
            inFields[8],inFields[9],inFields[10],inFields[11],
            inFields[12],inFields[13],inFields[14]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*****************************ipv6 lpm mc tables**********************************/
GT_IPV6ADDR                     nextIpGroup_ipv6;
GT_IPV6ADDR                     nextIpSrc_ipv6;
/*******************************************************************************
* wrCpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next muticast (ipSrc,ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc,ipGroup TCAM indexes.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.
*                             future support only , currently only vrId = 0 is
*                             supported.
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroup prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupPrefixLenPtr  - ipGroup prefix length.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       1.  The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*           values, it means that they exist in the IP-Mc Table, unless this is
*           the first call to this function, then the value of (ipGroupPtr,
*           ipSrcPtr) is (0,0).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{   GT_STATUS                       result;
    GT_U32                          lpmDBId;
    GT_U32                          vrId;
    CPSS_DXCH_IP_LTT_ENTRY_STC      mcRouteLttEntry;
    GT_U32                          tcamGroupRowIndex;
    GT_U32                          tcamSrcRowIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];


      /* call cpss api function */
    result = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId,vrId, &nextIpGroup_ipv6,&nextGroupPrefixLen,
                                             &nextIpSrc_ipv6,&nextSrcPrefixLen,
                                             &mcRouteLttEntry,&tcamGroupRowIndex,
                                             &tcamSrcRowIndex);


    /*no more entries to get*/
    if(result==GT_NOT_FOUND)
      {
       galtisOutput(outArgs, GT_OK, "%d", -1);
       return CMD_OK;
    }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    inFields[0]=index++;
    inFields[2]=nextGroupPrefixLen;
    inFields[4]=nextSrcPrefixLen;

    inFields[5]=mcRouteLttEntry.routeType;
    inFields[6]=mcRouteLttEntry.numOfPaths;
    inFields[7]=mcRouteLttEntry.routeEntryBaseIndex;
    inFields[8]=mcRouteLttEntry.ucRPFCheckEnable;
    inFields[9]=mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[10]=mcRouteLttEntry.ipv6MCGroupScopeLevel;

    inFields[11]=tcamGroupRowIndex;
    inFields[12]=tcamSrcRowIndex;


     /* pack and output table fields */
    fieldOutput("%d%16B%d%16B%d%d%d%d%d%d%d%d%d",
                inFields[0],nextIpGroup_ipv6.arIP,inFields[2],nextIpSrc_ipv6.arIP,
                inFields[4], inFields[5],inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],
                inFields[12]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}
/*******************************************************************************
* wrCpssDxChIpLpmIpv4McEntryGetFirst
*
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc,ipGroup TCAM indexes
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId           - The LPM DB id.
*       vrId              - The virtual router Id.
*                           future support only , currently only vrId = 0 is
*                           supported.
*       ipGroup           - The ip Group address to get the entry for.
*       ipGroupPrefixLen  - ipGroup prefix length.
*       ipSrc             - The ip Source address to get the entry for.
*       ipSrcPrefixLenPtr - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    IN  GT_U32                                  lpmDBId;
    IN  GT_U32                                  vrId;
    IN  GT_IPV6ADDR                             ipGroup;
    IN  GT_U32                                  ipGroupPrefixLen;
    IN  GT_IPV6ADDR                             ipSrc;
    IN  GT_U32                                  ipSrcPrefixLen;

    OUT CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    OUT GT_U32                                  tcamGroupRowIndex;
    OUT GT_U32                                  tcamSrcRowIndex;



    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*map arguments into locals*/
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inArgs[2]);
    ipGroupPrefixLen= (GT_U32)inArgs[3];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inArgs[4]);
    ipSrcPrefixLen= (GT_U32)inArgs[5];

    index=0;

    /* IPv6 multicast group prefix address format ff00::/8 */
    if ((ipGroupPrefixLen!=0) && (ipGroup.arIP[0] != 0xFF))
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_OK;
    }
    /* if the prefixlen is 0 then the only reasonable posibility is to look for
       the default MC entry */
    if(ipGroupPrefixLen==0)
    {
        cmdOsMemSet(&ipGroup, 0, sizeof(ipGroup));
        ipGroup.arIP[0] = 0xFF;
        ipGroupPrefixLen = 8;
    }

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId,vrId, ipGroup,ipGroupPrefixLen,
                                             ipSrc,ipSrcPrefixLen,
                                             &mcRouteLttEntry,&tcamGroupRowIndex,
                                             &tcamSrcRowIndex);

    /*save adress and length for get next*/
    nextIpGroup_ipv6.u32Ip[0]=ipGroup.u32Ip[0];
    nextIpGroup_ipv6.u32Ip[1]=ipGroup.u32Ip[1];
    nextIpGroup_ipv6.u32Ip[2]=ipGroup.u32Ip[2];
    nextIpGroup_ipv6.u32Ip[3]=ipGroup.u32Ip[3];

    nextGroupPrefixLen=ipGroupPrefixLen;

    nextIpSrc_ipv6.u32Ip[0]=ipSrc.u32Ip[0];
    nextIpSrc_ipv6.u32Ip[1]=ipSrc.u32Ip[1];
    nextIpSrc_ipv6.u32Ip[2]=ipSrc.u32Ip[2];
    nextIpSrc_ipv6.u32Ip[3]=ipSrc.u32Ip[3];

    nextSrcPrefixLen=ipSrcPrefixLen;

     /*if entry isn't found go to get next*/
    if(result==GT_NOT_FOUND)
      {
       return wrCpssDxChIpLpmIpv6McEntryGetNext(inArgs,
                                                inFields,
                                                numFields,
                                                outArgs);
      }

    if(result!=GT_OK)
      {
       galtisOutput(outArgs, result, "%d", -1);
       return CMD_OK;
      }

    prvCpssDxChIpLpmConvertRouteTypeForLegacyTests(lpmDBId, GT_FALSE, &mcRouteLttEntry);

    inFields[0]=index++;
    inFields[2]=nextGroupPrefixLen;
    inFields[4]=nextSrcPrefixLen;

    inFields[5]=mcRouteLttEntry.routeType;
    inFields[6]=mcRouteLttEntry.numOfPaths;
    inFields[7]=mcRouteLttEntry.routeEntryBaseIndex;
    inFields[8]=mcRouteLttEntry.ucRPFCheckEnable;
    inFields[9]=mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[10]=mcRouteLttEntry.ipv6MCGroupScopeLevel;

    inFields[11]=tcamGroupRowIndex;
    inFields[12]=tcamSrcRowIndex;

     /* pack and output table fields */
    fieldOutput("%d%16B%d%16B%d%d%d%d%d%d%d%d%d",
                inFields[0],nextIpGroup_ipv6.arIP,inFields[2],nextIpSrc_ipv6.arIP,
                inFields[4], inFields[5],inFields[6],inFields[7],
                inFields[8],inFields[9],inFields[10],inFields[11],
                inFields[12]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
*
* DESCRIPTION:
*   The function uses available TCAM rules at the beginning of the TCAM range
*   allocated to the LPM DB and place there "do nothing" rules that will make
*   sure that Multicast traffic is matched by these rules and not by the
*   Unicast default rule.
*   When policy based routing Unicast traffic coexists with IP based Multicast
*   (S,G,V) bridging (used in IGMP and MLD protocols), there is a need to add
*   default Multicast rules in order to make sure that the Multicast traffic
*   will not be matched by the Unicast default rule.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       pclIdArrayLen             - size of the PCL ID array (range 1..1048575)
*       pclIdArray                - array of PCL ID that may be in used by the LPM DB
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_BAD_PARAM              - if wrong value in any of the parameters
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_ALREADY_EXIST          - if default MC already set
*       GT_NOT_SUPPORTED          - if the LPM DB doesn't configured to
*                                   operate in policy based routing mode
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - the LPM DB must be configured to use policy based routing
*       One default Multicast rule will be set for each protocol stack supported
*       by the LPM DB and for each PCL ID that may be used in the LPM DB (each
*       virtaul router uses unique PCL ID).
*       For each protocol stack, the Multicast default rules will use the
*       prefixes allocated for Unicast prefixes for this protocol. For example
*       if the application allocates 10 IPv4 Unicast prefixes, then after setting
*       one default Multicast prefix, only 9 IPv4 Unicast prefixes will be
*       available.
*       The default Multicast rules will capture all IPv4/IPv6 Multicast
*       traffic with the same assigned PCL ID. As a result all rules configured
*       to match IP Multicast traffic with same assigned PCL ID that reside
*       after the Multicast defaults will not be matched.
*       The default Multicast rules can not be deleted after set.
*       The default Multicast rules can be set no more than once.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmPolicyBasedRoutingDefaultMcSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      lpmDBId;
    GT_U32      pclIdArray[1];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    pclIdArray[0] = 0;
    result = cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet(lpmDBId,1,pclIdArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
*
* DESCRIPTION:
*   The function gets whether the LPM DB is configured to support default MC
*   rules in policy based routing mode and the rule indexes and PCL ID of those
*   default rules.
*   Refer to cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet for more details.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       lpmDBId                           - the LPM DB id.
*       defaultIpv4RuleIndexArrayLenPtr   - points to the allocated size of the array
*       defaultIpv6RuleIndexArrayLenPtr   - points to the allocated size of the array
*       pclIdArrayLenPtr                  - points to the allocated size of the array
*
* OUTPUTS:
*       defaultMcUsedPtr                  - whether default MC is used for this LPM DB
*       protocolStackPtr                  - protocol stack supported by this LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArray         - rule indexes of the default IPv4 MC (in case
*                                           the LPM DB support IPv4)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv4RuleIndexArrayLenPtr   - points to number of elements filled in the array
*       defaultIpv6RuleIndexArray         - rule indexes of the default IPv6 MC (in case
*                                           the LPM DB support IPv6)
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       defaultIpv6RuleIndexArrayLenPtr   - points to number of elements filled in the array
*       pclIdArray                        - array of PCL ID that may be in used by the LPM DB
*                                           relevant only if defaultMcUsedPtr == GT_TRUE
*       pclIdArrayLenPtr                  - points to number of elements filled in the array
*
* RETURNS:
*       GT_OK                             - on success
*       GT_NOT_FOUND                      - if the LPM DB id is not found
*       GT_BAD_PTR                        - if one of the parameters is NULL pointer.
*       GT_FULL                           - if any of the arrays is not big enough
*       GT_FAIL                           - on failure
*       GT_NOT_APPLICABLE_DEVICE          - on not applicable device
*
* COMMENTS:
*       The rule indexes are according to explanations in cpssDxChPclRuleSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmPolicyBasedRoutingDefaultMcGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDBId;
    GT_BOOL defaultMcUsed;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_U32 defaultIpv4RuleIndexArray[1] = {0};
    GT_U32 defaultIpv4RuleIndexArrayLen = 1;
    GT_U32 defaultIpv6RuleIndexArray[1] = {0};
    GT_U32 defaultIpv6RuleIndexArrayLen = 1;
    GT_U32 pclIdArray[1] = {0};
    GT_U32 pclIdArrayLen = 1;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet(lpmDBId,
                                                         &defaultMcUsed,
                                                         &protocolStack,
                                                         defaultIpv4RuleIndexArray,
                                                         &defaultIpv4RuleIndexArrayLen,
                                                         defaultIpv6RuleIndexArray,
                                                         &defaultIpv6RuleIndexArrayLen,
                                                         pclIdArray,
                                                         &pclIdArrayLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", defaultMcUsed, protocolStack,
                                              defaultIpv4RuleIndexArray[0],
                                              defaultIpv6RuleIndexArray[0]);

    return CMD_OK;
}

/****************cpssDxChIpLpmTcamLinesReserve Table************/
static GT_U32*  linesList = NULL;
static GT_U32   numOfLines;

/*******************************************************************************
* cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmTcamLinesReserveSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                          ind;

    numOfLines = 0; /*reset on first*/

    /* map input arguments to locals */
    if(linesList == NULL)
    {
        linesList = (GT_U32*)cmdOsMalloc(sizeof(GT_U32));
    }

    if(linesList == NULL)
    {
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > numOfLines)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "");
        return CMD_OK;
    }

    linesList[ind] = (GT_U32)inFields[1];

    numOfLines++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmTcamLinesReserveSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                          ind;

    /* map input arguments to locals */
    linesList = (GT_U32*) cmdOsRealloc(linesList,
                          sizeof(GT_U32) * (numOfLines+1));

    if(linesList == NULL)
    {
        linesList = (GT_U32*)cmdOsMalloc(sizeof(GT_U32));
    }

    if(linesList == NULL)
    {
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }

    ind = (GT_U32)inFields[0];

    if(ind > numOfLines)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "");
        return CMD_OK;
    }

    linesList[ind] = (GT_U32)inFields[1];

    numOfLines++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmTcamLinesReserve
*
* DESCRIPTION:
*   This function reserves lines in the router TCAM. The LPM DB will not use
*   those lines for prefixes. Those lines will be considered as allocated
*   entries.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       lpmDBId                   - the LPM DB id.
*       linesToReserveArray       - lines to be reserved
*       numberOfLinesToReserve    - number of lines in linesToReserveArray
*       allocateReserveLinesAs    - relevant only when LPM DB is configured to
*                                   use dynamic TCAM partitioning.
*                                   Indicates as what entry type to treat the
*                                   reserved lines.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success
*       GT_NOT_FOUND              - if the LPM DB id is not found
*       GT_BAD_STATE              - if the existing LPM DB is not empty
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*       The following conditions must be matched:
*         - the LPM DB must be empty (no prefixes and no virtual routers)
*         - all lines to reserve must reside in the TCAM section allocated
*           to the LPM DB
*         - all lines to reserve must be unique
*       In case the LPM DB is configured to use dynamic partitioning those
*       additional conditions must be matched:
*         - the sum of the lines in allocateReserveLinesAs must be equal to
*           the number of lines to reserve
*         - the lines in allocateReserveLinesAs must comply with the protocol
*           stacks supported by the LPM DB
*         - the LPM DB must have enough space to hold the allocateReserveLinesAs
*           as allocated entires.
*       Note that the parameters in allocateReserveLinesAs are in lines. Meaning
*       that if (allocateReserveLinesAs.numOfIpv4Prefixes == 1) it means that
*       one whole TCAM line will be considered allocated as IPv4 prefixes. In
*       4 columns TCAM it means that 4 IPv4 prefixes will be "allocated" in
*       this line to reserve.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmTcamLinesReserveEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32    lpmDBId;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC allocateReserveLinesAs;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    allocateReserveLinesAs.numOfIpv4Prefixes = (GT_U32)inArgs[1];
    allocateReserveLinesAs.numOfIpv4McSourcePrefixes = (GT_U32)inArgs[2];
    allocateReserveLinesAs.numOfIpv6Prefixes = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpLpmTcamLinesReserve(lpmDBId, linesList, numOfLines,
                                                    allocateReserveLinesAs);

    cmdOsFree(linesList);
    linesList = NULL;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpLpmDBCapacityUpdate
*
* DESCRIPTION:
*   This function updates the initial LPM DB allocation.
*
* APPLICABLE DEVICES: DxChXcat and above devices.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       tcamLpmManagerCapcityCfgPtr - the new capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means new prefixes
*                                     partition, when this was set to GT_FALSE
*                                     this means the new prefixes guaranteed
*                                     allocation.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                       - on success
*       GT_BAD_PARAM                - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*       GT_NOT_FOUND                - if the LPM DB id is not found
*       GT_NO_RESOURCE              - failed to allocate resources needed to the
*                                     new configuration
*
* COMMENTS:
*       This API is relevant only when using TCAM Manager. This API is used for
*       updating only the capacity configuration of the LPM. for updating the
*       lines reservation for the TCAM Manger use cpssDxChTcamManagerRangeUpdate.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBCapacityUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDBId;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    indexesRange.firstIndex = (GT_U32)inArgs[1];
    indexesRange.lastIndex = (GT_U32)inArgs[2];
    tcamLpmManagerCapcityCfg.numOfIpv4Prefixes = (GT_U32)inArgs[3];
    tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes = (GT_U32)inArgs[4];
    tcamLpmManagerCapcityCfg.numOfIpv6Prefixes = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssDxChIpLpmDBCapacityUpdate(lpmDBId, &indexesRange,
                                            &tcamLpmManagerCapcityCfg);

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
* cpssDxChIpLpmDBCapacityGet
*
* DESCRIPTION:
*   This function gets the current LPM DB allocation.
*
* APPLICABLE DEVICES: DxChXcat and above devices.
*
* INPUTS:
*       lpmDBId                     - the LPM DB id.
*
* OUTPUTS:
*       indexesRangePtr             - the range of TCAM indexes availble for this
*                                     LPM DB (see explanation in
*                                     CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC).
*                                     this field is relevant when partitionEnable
*                                     in cpssDxChIpLpmDBCreate was GT_TRUE.
*       partitionEnablePtr          - GT_TRUE:  the TCAM is partitioned according
*                                     to the capacityCfgPtr, any unused TCAM entries
*                                     were allocated to IPv4 UC entries.
*                                     GT_FALSE: TCAM entries are allocated on demand
*                                     while entries are guaranteed as specified
*                                     in capacityCfgPtr.
*       tcamLpmManagerCapcityCfgPtr - the current capacity configuration. when
*                                     partitionEnable in cpssDxChIpLpmDBCreate
*                                     was set to GT_TRUE this means current
*                                     prefixes partition, when this was set to
*                                     GT_FALSE this means the current guaranteed
*                                     prefixes allocation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NO_RESOURCE           - failed to allocate resources needed to the
*                                  new configuration.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmDBCapacityGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDBId;
    GT_BOOL partitionEnable;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_INDEX_RANGE_STC indexesRange;
    CPSS_DXCH_IP_TCAM_LPM_MANGER_CAPCITY_CFG_STC tcamLpmManagerCapcityCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpLpmDBCapacityGet(lpmDBId, &partitionEnable,
                                &indexesRange, &tcamLpmManagerCapcityCfg);

    galtisOutput(outArgs, result, "%d%d%d%d%d%d", indexesRange.firstIndex,
                                                  indexesRange.lastIndex,
                                                  tcamLpmManagerCapcityCfg.numOfIpv4Prefixes,
                                                  tcamLpmManagerCapcityCfg.numOfIpv4McSourcePrefixes,
                                                  tcamLpmManagerCapcityCfg.numOfIpv6Prefixes,
                                                  partitionEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddMany
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Unicast prefixes and
*       returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAddMany
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_IPADDR lastIpAddr;
    GT_U32 routeEntryBaseMemAddr;
    GT_U32 numberOfPrefixesToAdd;
    GT_U32 numOfAddedPrefixes;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr,(GT_U8*)inArgs[2]);
    routeEntryBaseMemAddr = (GT_U32)inArgs[4];
    numberOfPrefixesToAdd = (GT_U32)inArgs[5];


    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixAddMany(lpmDbId,
                                               vrId,
                                               ipAddr,
                                               routeEntryBaseMemAddr,
                                               numberOfPrefixesToAdd,
                                               &lastIpAddr,
                                               &numOfAddedPrefixes);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfAddedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddManyRandom
*
* DESCRIPTION:
*       This function tries to add many random IPv4 Unicast prefixes and
*       returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*        Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       isWholeIpRandom       - GT_TRUE: all IP octets calculated by cpssOsRand
*                               GT_FALSE: only 2 LSB octets calculated by cpssOsRand
*
* OUTPUTS:
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAddManyRandom
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_U32  routeEntryBaseMemAddr;
    GT_U32 numberOfPrefixesToAdd;
    GT_BOOL     isWholeIpRandom;
    GT_U32 numOfAddedPrefixes = 0;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr,(GT_U8*)inArgs[2]);

    routeEntryBaseMemAddr = (GT_U32)inArgs[4];
    numberOfPrefixesToAdd = (GT_U32)inArgs[6];
    isWholeIpRandom = (GT_BOOL)inArgs[7];
    /* call cpss api function */

    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    result = cpssDxChIpLpmIpv4UcPrefixAddManyRandom(lpmDbId,
                                                    vrId,
                                                    ipAddr,
                                                    routeEntryBaseMemAddr,
                                                    numberOfPrefixesToAdd,
                                                    isWholeIpRandom,
                                                    &numOfAddedPrefixes);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfAddedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmIpv4UcPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Unicast prefixes acordint to octet order selection
*        and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       firstOctetToChange    - first octet to change in startIpAddrfrom up till 255
*       secondOctetToChange   - second octet to change in startIpAddrfrom up till 255
*       thirdOctetToChange    - third octet to change in startIpAddrfrom up till 255
*       forthOctetToChange    - forth octet to change in startIpAddrfrom up till 255
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixAddManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_IPADDR lastIpAddr;
    GT_U32 routeEntryBaseMemAddr;
    GT_U32 numberOfPrefixesToAdd;
    GT_U32 numOfAddedPrefixes;
    GT_U32 firstOctetToChange;
    GT_U32 secondOctetToChange;
    GT_U32 thirdOctetToChange;
    GT_U32 forthOctetToChange;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr,(GT_U8*)inArgs[2]);
    routeEntryBaseMemAddr = (GT_U32)inArgs[3];
    numberOfPrefixesToAdd = (GT_U32)inArgs[4];
    firstOctetToChange = (GT_U32)inArgs[5];
    secondOctetToChange= (GT_U32)inArgs[6];
    thirdOctetToChange= (GT_U32)inArgs[7];
    forthOctetToChange= (GT_U32)inArgs[8];



    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixAddManyByOctet(lpmDbId,
                                                vrId,
                                                ipAddr,
                                                routeEntryBaseMemAddr,
                                                numberOfPrefixesToAdd,
                                                firstOctetToChange,
                                                secondOctetToChange,
                                                thirdOctetToChange,
                                                forthOctetToChange,
                                                &lastIpAddr,
                                                &numOfAddedPrefixes);


    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfAddedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmIpv4UcPrefixDelManyByOctet
*
* DESCRIPTION:
*       This function tries to delete many sequential IPv4 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully deleted.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       numOfPrefixesToDel    - the number of prefixes to delete
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrDeletedPtr    - points to the last prefix successfully
*                                 deleted (NULL to ignore)
*       numOfPrefixesDeletedPtr - points to the nubmer of prefixes that were
*                                 successfully deleted (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to delete the number of the prefixes
*       that was added by cpssDxChIpLpmIpv4UcPrefixAddManyByOctet. 
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixDelManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_IPADDR lastIpAddr;
    GT_U32 numberOfPrefixesToDel;
    GT_U32 numOfDeletedPrefixes;
    GT_U32 firstOctetToChange;
    GT_U32 secondOctetToChange;
    GT_U32 thirdOctetToChange;
    GT_U32 forthOctetToChange;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr,(GT_U8*)inArgs[2]);
    numberOfPrefixesToDel = (GT_U32)inArgs[3];
    firstOctetToChange = (GT_U32)inArgs[4];
    secondOctetToChange= (GT_U32)inArgs[5];
    thirdOctetToChange= (GT_U32)inArgs[6];
    forthOctetToChange= (GT_U32)inArgs[7];



    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixDelManyByOctet(lpmDbId,
                                                    vrId,
                                                    ipAddr,
                                                    numberOfPrefixesToDel,
                                                    firstOctetToChange,
                                                    secondOctetToChange,
                                                    thirdOctetToChange,
                                                    forthOctetToChange,
                                                    &lastIpAddr,
                                                    &numOfDeletedPrefixes);


    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfDeletedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Unicast prefixes (that were
*       added by cpssDxChIpLpmIpv4UcPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToGet    - the number of prefixes to get
*       firstOctetToChange    - first octet to change in startIpAddrfrom up till 255
*       secondOctetToChange   - second octet to change in startIpAddrfrom up till 255
*       thirdOctetToChange    - third octet to change in startIpAddrfrom up till 255
*       forthOctetToChange    - forth octet to change in startIpAddrfrom up till 255
*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4UcPrefixGetManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR ipAddr;
    GT_U32 numberOfPrefixesToGet;
    GT_U32 numOfRetrievedPrefixes;
    GT_U32 firstOctetToChange;
    GT_U32 secondOctetToChange;
    GT_U32 thirdOctetToChange;
    GT_U32 forthOctetToChange;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    numberOfPrefixesToGet = (GT_U32)inArgs[4];
    firstOctetToChange = (GT_U32)inArgs[5];
    secondOctetToChange= (GT_U32)inArgs[6];
    thirdOctetToChange= (GT_U32)inArgs[7];
    forthOctetToChange= (GT_U32)inArgs[8];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixGetManyByOctet(lpmDbId,
                                                vrId,
                                                ipAddr,
                                                numberOfPrefixesToGet,
                                                firstOctetToChange,
                                                secondOctetToChange,
                                                thirdOctetToChange,
                                                forthOctetToChange,
                                                &numOfRetrievedPrefixes);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfRetrievedPrefixes);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpLpmIpv6UcPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv6 Unicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       lastIpAddrAddedPtr    - points to the last prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 128). The route entry is not written.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixAddManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPV6ADDR ipAddr;
    GT_IPV6ADDR lastIpAddr;
    GT_U32 routeEntryBaseMemAddr;
    GT_U32 numberOfPrefixesToAdd;
    GT_U32 numOfAddedPrefixes;
    GT_U32 firstOctetToChange;
    GT_U32 secondOctetToChange;
    GT_U32 thirdOctetToChange;
    GT_U32 forthOctetToChange;
    GT_U32 fifthOctetToChange;
    GT_U32 sixthOctetToChange;
    GT_U32 seventhOctetToChange;
    GT_U32 eighthOctetToChange;
    GT_U32 ninthOctetToChange;
    GT_U32 tenthOctetToChange;
    GT_U32 elevenOctetToChange;
    GT_U32 twelveOctetToChange;
    GT_U32 thirteenOctetToChange;
    GT_U32 fourteenOctetToChange;
    GT_U32 fifteenOctetToChange;
    GT_U32 sixteenOctetToChange;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr,(GT_U8*)inArgs[2]);
    routeEntryBaseMemAddr = (GT_U32)inArgs[3];
    numberOfPrefixesToAdd = (GT_U32)inArgs[4];
    firstOctetToChange = (GT_U32)inArgs[5];
    secondOctetToChange= (GT_U32)inArgs[6];
    thirdOctetToChange= (GT_U32)inArgs[7];
    forthOctetToChange= (GT_U32)inArgs[8];
    fifthOctetToChange= (GT_U32)inArgs[9];
    sixthOctetToChange= (GT_U32)inArgs[10];
    seventhOctetToChange= (GT_U32)inArgs[11];
    eighthOctetToChange= (GT_U32)inArgs[12];
    ninthOctetToChange= (GT_U32)inArgs[13];
    tenthOctetToChange= (GT_U32)inArgs[14];
    elevenOctetToChange= (GT_U32)inArgs[15];
    twelveOctetToChange= (GT_U32)inArgs[16];
    thirteenOctetToChange= (GT_U32)inArgs[17];
    fourteenOctetToChange= (GT_U32)inArgs[18];
    fifteenOctetToChange= (GT_U32)inArgs[19];
    sixteenOctetToChange= (GT_U32)inArgs[20];



    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixAddManyByOctet(lpmDbId,
                                                        vrId,
                                                        ipAddr,
                                                        routeEntryBaseMemAddr,
                                                        numberOfPrefixesToAdd,
                                                        firstOctetToChange,
                                                        secondOctetToChange,
                                                        thirdOctetToChange,
                                                        forthOctetToChange,
                                                        fifthOctetToChange,
                                                        sixthOctetToChange,
                                                        seventhOctetToChange,
                                                        eighthOctetToChange,
                                                        ninthOctetToChange,
                                                        tenthOctetToChange,
                                                        elevenOctetToChange,
                                                        twelveOctetToChange,
                                                        thirteenOctetToChange,
                                                        fourteenOctetToChange,
                                                        fifteenOctetToChange,
                                                        sixteenOctetToChange,
                                                        &lastIpAddr,
                                                        &numOfAddedPrefixes);


    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfAddedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv6 Unicast prefixes (that were
*       added by cpssDxChIpLpmIpv6UcPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       startIpAddr           - the first address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToGet    - the number of prefixes to get
*       firstOctetToChange    - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       secondOctetToChange   - second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirdOctetToChange    - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       forthOctetToChange    - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifthOctetToChange    - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixthOctetToChange    - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       seventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       eighthOctetToChange   - eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       ninthOctetToChange    - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       tenthOctetToChange    - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       elevenOctetToChange   - eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       twelveOctetToChange   - twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       thirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       fifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       sixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6UcPrefixGetManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPV6ADDR ipAddr;
    GT_U32 numberOfPrefixesToGet;
    GT_U32 numOfRetrievedPrefixes;
    GT_U32 firstOctetToChange;
    GT_U32 secondOctetToChange;
    GT_U32 thirdOctetToChange;
    GT_U32 forthOctetToChange;
    GT_U32 fifthOctetToChange;
    GT_U32 sixthOctetToChange;
    GT_U32 seventhOctetToChange;
    GT_U32 eighthOctetToChange;
    GT_U32 ninthOctetToChange;
    GT_U32 tenthOctetToChange;
    GT_U32 elevenOctetToChange;
    GT_U32 twelveOctetToChange;
    GT_U32 thirteenOctetToChange;
    GT_U32 fourteenOctetToChange;
    GT_U32 fifteenOctetToChange;
    GT_U32 sixteenOctetToChange;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    numberOfPrefixesToGet = (GT_U32)inArgs[4];
    firstOctetToChange = (GT_U32)inArgs[5];
    secondOctetToChange= (GT_U32)inArgs[6];
    thirdOctetToChange= (GT_U32)inArgs[7];
    forthOctetToChange= (GT_U32)inArgs[8];
    fifthOctetToChange= (GT_U32)inArgs[9];
    sixthOctetToChange= (GT_U32)inArgs[10];
    seventhOctetToChange= (GT_U32)inArgs[11];
    eighthOctetToChange= (GT_U32)inArgs[12];
    ninthOctetToChange= (GT_U32)inArgs[13];
    tenthOctetToChange= (GT_U32)inArgs[14];
    elevenOctetToChange= (GT_U32)inArgs[15];
    twelveOctetToChange= (GT_U32)inArgs[16];
    thirteenOctetToChange= (GT_U32)inArgs[17];
    fourteenOctetToChange= (GT_U32)inArgs[18];
    fifteenOctetToChange= (GT_U32)inArgs[19];
    sixteenOctetToChange= (GT_U32)inArgs[20];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixGetManyByOctet(lpmDbId,
                                                vrId,
                                                ipAddr,
                                                numberOfPrefixesToGet,
                                                firstOctetToChange,
                                                secondOctetToChange,
                                                thirdOctetToChange,
                                                forthOctetToChange,
                                                fifthOctetToChange,
                                                sixthOctetToChange,
                                                seventhOctetToChange,
                                                eighthOctetToChange,
                                                ninthOctetToChange,
                                                tenthOctetToChange,
                                                elevenOctetToChange,
                                                twelveOctetToChange,
                                                thirteenOctetToChange,
                                                fourteenOctetToChange,
                                                fifteenOctetToChange,
                                                sixteenOctetToChange,
                                                &numOfRetrievedPrefixes);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfRetrievedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmIpv4McPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv4 Multicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       grpLastIpAddrAddedPtr - points to the last group prefix successfully
*                               added (NULL to ignore)
*       srcLastIpAddrAddedPtr - points to the last source prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 32). The route entry is not written.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McPrefixAddManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR   grpStartIpAddr;
    GT_IPADDR   grpLastIpAddr;
    GT_IPADDR   srcStartIpAddr;
    GT_IPADDR   srcLastIpAddr;
    GT_U32      routeEntryBaseMemAddr;
    GT_U32      numberOfPrefixesToAdd;
    GT_U32      numOfAddedPrefixes;
    GT_BOOL     changeGrpAddr;
    GT_U32      grpFirstOctetToChange;
    GT_U32      grpSecondOctetToChange;
    GT_U32      grpThirdOctetToChange;
    GT_U32      grpForthOctetToChange;
    GT_BOOL     changeSrcAddr;
    GT_U32      srcFirstOctetToChange;
    GT_U32      srcSecondOctetToChange;
    GT_U32      srcThirdOctetToChange;
    GT_U32      srcForthOctetToChange;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&grpStartIpAddr,(GT_U8*)inArgs[2]);
    galtisIpAddr(&srcStartIpAddr,(GT_U8*)inArgs[3]);
    routeEntryBaseMemAddr = (GT_U32)inArgs[4];
    numberOfPrefixesToAdd = (GT_U32)inArgs[5];

    changeGrpAddr=(GT_BOOL)inArgs[6];
    grpFirstOctetToChange= (GT_U32)inArgs[7];
    grpSecondOctetToChange= (GT_U32)inArgs[8];
    grpThirdOctetToChange= (GT_U32)inArgs[9];
    grpForthOctetToChange= (GT_U32)inArgs[10];
    changeSrcAddr= (GT_BOOL)inArgs[11];
    srcFirstOctetToChange= (GT_U32)inArgs[12];
    srcSecondOctetToChange= (GT_U32)inArgs[13];
    srcThirdOctetToChange= (GT_U32)inArgs[14];
    srcForthOctetToChange= (GT_U32)inArgs[15];


    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssDxChIpLpmIpv4McPrefixAddManyByOctet(lpmDbId,
                                                       vrId,
                                                       grpStartIpAddr,
                                                       srcStartIpAddr,
                                                       routeEntryBaseMemAddr,
                                                       numberOfPrefixesToAdd,
                                                       changeGrpAddr,
                                                       grpFirstOctetToChange,
                                                       grpSecondOctetToChange,
                                                       grpThirdOctetToChange,
                                                       grpForthOctetToChange,
                                                       changeSrcAddr,
                                                       srcFirstOctetToChange,
                                                       srcSecondOctetToChange,
                                                       srcThirdOctetToChange,
                                                       srcForthOctetToChange,
                                                       &grpLastIpAddr,
                                                       &srcLastIpAddr,
                                                       &numOfAddedPrefixes);

    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfAddedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmIpv4McPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Multicast prefixes (that were
*       added by cpssDxChIpLpmIpv4McPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToGet    - the number of prefixes to get
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255*
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv4McPrefixGetManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPADDR grpStartIpAddr;
    GT_IPADDR srcStartIpAddr;
    GT_U32      numberOfPrefixesToGet;
    GT_U32      numOfRetrievedPrefixes;
    GT_BOOL     changeGrpAddr;
    GT_U32      grpFirstOctetToChange;
    GT_U32      grpSecondOctetToChange;
    GT_U32      grpThirdOctetToChange;
    GT_U32      grpForthOctetToChange;
    GT_BOOL     changeSrcAddr;
    GT_U32      srcFirstOctetToChange;
    GT_U32      srcSecondOctetToChange;
    GT_U32      srcThirdOctetToChange;
    GT_U32      srcForthOctetToChange;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&grpStartIpAddr,(GT_U8*)inArgs[2]);
    galtisIpAddr(&srcStartIpAddr,(GT_U8*)inArgs[3]);
    numberOfPrefixesToGet = (GT_U32)inArgs[5];
    changeGrpAddr=(GT_BOOL)inArgs[6];
    grpFirstOctetToChange= (GT_U32)inArgs[7];
    grpSecondOctetToChange= (GT_U32)inArgs[8];
    grpThirdOctetToChange= (GT_U32)inArgs[9];
    grpForthOctetToChange= (GT_U32)inArgs[10];
    changeSrcAddr= (GT_BOOL)inArgs[11];
    srcFirstOctetToChange= (GT_U32)inArgs[12];
    srcSecondOctetToChange= (GT_U32)inArgs[13];
    srcThirdOctetToChange= (GT_U32)inArgs[14];
    srcForthOctetToChange= (GT_U32)inArgs[15];



    /* call cpss api function */
    result = cpssDxChIpLpmIpv4McPrefixGetManyByOctet(lpmDbId,
                                                       vrId,
                                                       grpStartIpAddr,
                                                       srcStartIpAddr,
                                                       numberOfPrefixesToGet,
                                                       changeGrpAddr,
                                                       grpFirstOctetToChange,
                                                       grpSecondOctetToChange,
                                                       grpThirdOctetToChange,
                                                       grpForthOctetToChange,
                                                       changeSrcAddr,
                                                       srcFirstOctetToChange,
                                                       srcSecondOctetToChange,
                                                       srcThirdOctetToChange,
                                                       srcForthOctetToChange,
                                                       &numOfRetrievedPrefixes);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfRetrievedPrefixes);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpLpmIpv6McPrefixAddManyByOctet
*
* DESCRIPTION:
*       This function tries to add many sequential IPv6 Multicast prefixes according
*       to octet order selection and returns the number of prefixes successfully added.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       ipGroupRuleIndex      - index of the IPv6 mc group rule in the TCAM.
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToAdd    - the number of prefixes to add
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*
* OUTPUTS:
*       grpLastIpAddrAddedPtr - points to the last group prefix successfully
*                               added (NULL to ignore)
*       srcLastIpAddrAddedPtr - points to the last source prefix successfully
*                               added (NULL to ignore)
*       numOfPrefixesAddedPtr - points to the nubmer of prefixes that were
*                               successfully added (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that can be added. All the prefixes are added with exact match (prefix
*       length 128). The route entry is not written.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McPrefixAddManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPV6ADDR   grpStartIpAddr;
    GT_IPV6ADDR   grpLastIpAddr;
    GT_IPV6ADDR   srcStartIpAddr;
    GT_IPV6ADDR   srcLastIpAddr;
    GT_U32      routeEntryBaseMemAddr;
    GT_U32      numberOfPrefixesToAdd;
    GT_U32      numOfAddedPrefixes;
    GT_BOOL     changeGrpAddr;
    GT_U32      grpFirstOctetToChange;
    GT_U32      grpSecondOctetToChange;
    GT_U32      grpThirdOctetToChange;
    GT_U32      grpForthOctetToChange;
    GT_U32      grpFifthOctetToChange;
    GT_U32      grpSixthOctetToChange;
    GT_U32      grpSeventhOctetToChange;
    GT_U32      grpEighthOctetToChange;
    GT_U32      grpNinthOctetToChange;
    GT_U32      grpTenthOctetToChange;
    GT_U32      grpElevenOctetToChange;
    GT_U32      grpTwelveOctetToChange;
    GT_U32      grpThirteenOctetToChange;
    GT_U32      grpFourteenOctetToChange;
    GT_U32      grpFifteenOctetToChange;
    GT_U32      grpSixteenOctetToChange;
    GT_BOOL     changeSrcAddr;
    GT_U32      srcFirstOctetToChange;
    GT_U32      srcSecondOctetToChange;
    GT_U32      srcThirdOctetToChange;
    GT_U32      srcForthOctetToChange;
    GT_U32      srcFifthOctetToChange;
    GT_U32      srcSixthOctetToChange;
    GT_U32      srcSeventhOctetToChange;
    GT_U32      srcEighthOctetToChange;
    GT_U32      srcNinthOctetToChange;
    GT_U32      srcTenthOctetToChange;
    GT_U32      srcElevenOctetToChange;
    GT_U32      srcTwelveOctetToChange;
    GT_U32      srcThirteenOctetToChange;
    GT_U32      srcFourteenOctetToChange;
    GT_U32      srcFifteenOctetToChange;
    GT_U32      srcSixteenOctetToChange;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&grpStartIpAddr,(GT_U8*)inArgs[2]);
    galtisIpv6Addr(&srcStartIpAddr,(GT_U8*)inArgs[3]);
    routeEntryBaseMemAddr = (GT_U32)inArgs[4];
    numberOfPrefixesToAdd = (GT_U32)inArgs[5];

    changeGrpAddr=(GT_BOOL)inArgs[6];
    grpFirstOctetToChange= (GT_U32)inArgs[7];
    grpSecondOctetToChange= (GT_U32)inArgs[8];
    grpThirdOctetToChange= (GT_U32)inArgs[9];
    grpForthOctetToChange= (GT_U32)inArgs[10];
    grpFifthOctetToChange= (GT_U32)inArgs[11];
    grpSixthOctetToChange= (GT_U32)inArgs[12];
    grpSeventhOctetToChange= (GT_U32)inArgs[13];
    grpEighthOctetToChange= (GT_U32)inArgs[14];
    grpNinthOctetToChange= (GT_U32)inArgs[15];
    grpTenthOctetToChange= (GT_U32)inArgs[16];
    grpElevenOctetToChange= (GT_U32)inArgs[17];
    grpTwelveOctetToChange= (GT_U32)inArgs[18];
    grpThirteenOctetToChange= (GT_U32)inArgs[19];
    grpFourteenOctetToChange= (GT_U32)inArgs[20];
    grpFifteenOctetToChange= (GT_U32)inArgs[21];
    grpSixteenOctetToChange= (GT_U32)inArgs[22];
    changeSrcAddr= (GT_BOOL)inArgs[23];
    srcFirstOctetToChange= (GT_U32)inArgs[24];
    srcSecondOctetToChange= (GT_U32)inArgs[25];
    srcThirdOctetToChange= (GT_U32)inArgs[26];
    srcForthOctetToChange= (GT_U32)inArgs[27];
    srcFifthOctetToChange= (GT_U32)inArgs[28];
    srcSixthOctetToChange= (GT_U32)inArgs[29];
    srcSeventhOctetToChange= (GT_U32)inArgs[30];
    srcEighthOctetToChange= (GT_U32)inArgs[31];
    srcNinthOctetToChange= (GT_U32)inArgs[32];
    srcTenthOctetToChange= (GT_U32)inArgs[33];
    srcElevenOctetToChange= (GT_U32)inArgs[34];
    srcTwelveOctetToChange= (GT_U32)inArgs[35];
    srcThirteenOctetToChange= (GT_U32)inArgs[36];
    srcFourteenOctetToChange= (GT_U32)inArgs[37];
    srcFifteenOctetToChange= (GT_U32)inArgs[38];
    srcSixteenOctetToChange= (GT_U32)inArgs[39];


    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssDxChIpLpmIpv6McPrefixAddManyByOctet(lpmDbId,
                                                        vrId,
                                                        grpStartIpAddr,
                                                        srcStartIpAddr,
                                                        routeEntryBaseMemAddr,
                                                        numberOfPrefixesToAdd,
                                                        changeGrpAddr,
                                                        grpFirstOctetToChange,
                                                        grpSecondOctetToChange,
                                                        grpThirdOctetToChange,
                                                        grpForthOctetToChange,
                                                        grpFifthOctetToChange,
                                                        grpSixthOctetToChange,
                                                        grpSeventhOctetToChange,
                                                        grpEighthOctetToChange,
                                                        grpNinthOctetToChange,
                                                        grpTenthOctetToChange,
                                                        grpElevenOctetToChange,
                                                        grpTwelveOctetToChange,
                                                        grpThirteenOctetToChange,
                                                        grpFourteenOctetToChange,
                                                        grpFifteenOctetToChange,
                                                        grpSixteenOctetToChange,
                                                        changeSrcAddr,
                                                        srcFirstOctetToChange,
                                                        srcSecondOctetToChange,
                                                        srcThirdOctetToChange,
                                                        srcForthOctetToChange,
                                                        srcFifthOctetToChange,
                                                        srcSixthOctetToChange,
                                                        srcSeventhOctetToChange,
                                                        srcEighthOctetToChange,
                                                        srcNinthOctetToChange,
                                                        srcTenthOctetToChange,
                                                        srcElevenOctetToChange,
                                                        srcTwelveOctetToChange,
                                                        srcThirteenOctetToChange,
                                                        srcFourteenOctetToChange,
                                                        srcFifteenOctetToChange,
                                                        srcSixteenOctetToChange,
                                                        &grpLastIpAddr,
                                                        &srcLastIpAddr,
                                                        &numOfAddedPrefixes);

    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfAddedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChIpLpmIpv4McPrefixGetManyByOctet
*
* DESCRIPTION:
*       This function tries to get many sequential IPv4 Multicast prefixes (that were
*       added by cpssDxChIpLpmIpv4McPrefixAddManyByOctet) and returns the number of
*       successfully retrieved prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDbId               - the LPM DB id
*       vrId                  - the virtual router id
*       grpStartIpAddr        - the first group address to add
*       ipGroupRuleIndex      - index of the IPv6 mc group rule in the TCAM.
*       srcStartIpAddr        - the first source address to add
*       routeEntryBaseMemAddr - base memory Address (offset) of the route entry
*       numOfPrefixesToGet    - the number of prefixes to get
*       changeGrpAddr         - GT_TRUE: change the group address
*                               GT_FALSE: don't change the group address
*       grpFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       grpSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       changeSrcAddr         - GT_TRUE: change the source address
*                               GT_FALSE: don't change the source address
*       srcFirstOctetToChange - first octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSecondOctetToChange- second octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirdOctetToChange - third octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcForthOctetToChange - forth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifthOctetToChange - fifth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixthOctetToChange - sixth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSeventhOctetToChange  - seventh octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcEighthOctetToChange- eighth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcNinthOctetToChange - ninth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTenthOctetToChange - tenth octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcElevenOctetToChange- eleven octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcTwelveOctetToChange- twelve octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcThirteenOctetToChange - thirteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFourteenOctetToChange - fourteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcFifteenOctetToChange  - fifteen octet to change in the prefix address.
*                               value of the octet can be up to 255
*       srcSixteenOctetToChange  - sixteen octet to change in the prefix address.
*                               value of the octet can be up to 255
* OUTPUTS:
*       numOfRetrievedPrefixesPtr - points to the nubmer of prefixes that were
*                                   successfully retrieved (NULL to ignore)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This debug function is intended to check the number of the prefixes
*       that were be added.
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpmIpv6McPrefixGetManyByOctet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lpmDbId;
    GT_U32 vrId;
    GT_IPV6ADDR grpStartIpAddr;
    GT_IPV6ADDR srcStartIpAddr;
    GT_U32      numberOfPrefixesToGet;
    GT_U32      numOfRetrievedPrefixes;
    GT_BOOL     changeGrpAddr;
    GT_U32      grpFirstOctetToChange;
    GT_U32      grpSecondOctetToChange;
    GT_U32      grpThirdOctetToChange;
    GT_U32      grpForthOctetToChange;
    GT_U32      grpFifthOctetToChange;
    GT_U32      grpSixthOctetToChange;
    GT_U32      grpSeventhOctetToChange;
    GT_U32      grpEighthOctetToChange;
    GT_U32      grpNinthOctetToChange;
    GT_U32      grpTenthOctetToChange;
    GT_U32      grpElevenOctetToChange;
    GT_U32      grpTwelveOctetToChange;
    GT_U32      grpThirteenOctetToChange;
    GT_U32      grpFourteenOctetToChange;
    GT_U32      grpFifteenOctetToChange;
    GT_U32      grpSixteenOctetToChange;
    GT_BOOL     changeSrcAddr;
    GT_U32      srcFirstOctetToChange;
    GT_U32      srcSecondOctetToChange;
    GT_U32      srcThirdOctetToChange;
    GT_U32      srcForthOctetToChange;
    GT_U32      srcFifthOctetToChange;
    GT_U32      srcSixthOctetToChange;
    GT_U32      srcSeventhOctetToChange;
    GT_U32      srcEighthOctetToChange;
    GT_U32      srcNinthOctetToChange;
    GT_U32      srcTenthOctetToChange;
    GT_U32      srcElevenOctetToChange;
    GT_U32      srcTwelveOctetToChange;
    GT_U32      srcThirteenOctetToChange;
    GT_U32      srcFourteenOctetToChange;
    GT_U32      srcFifteenOctetToChange;
    GT_U32      srcSixteenOctetToChange;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDbId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&grpStartIpAddr,(GT_U8*)inArgs[2]);
    galtisIpv6Addr(&srcStartIpAddr,(GT_U8*)inArgs[3]);
    numberOfPrefixesToGet = (GT_U32)inArgs[5];
    changeGrpAddr=(GT_BOOL)inArgs[6];
    grpFirstOctetToChange= (GT_U32)inArgs[7];
    grpSecondOctetToChange= (GT_U32)inArgs[8];
    grpThirdOctetToChange= (GT_U32)inArgs[9];
    grpForthOctetToChange= (GT_U32)inArgs[10];
    grpFifthOctetToChange= (GT_U32)inArgs[11];
    grpSixthOctetToChange= (GT_U32)inArgs[12];
    grpSeventhOctetToChange= (GT_U32)inArgs[13];
    grpEighthOctetToChange= (GT_U32)inArgs[14];
    grpNinthOctetToChange= (GT_U32)inArgs[15];
    grpTenthOctetToChange= (GT_U32)inArgs[16];
    grpElevenOctetToChange= (GT_U32)inArgs[17];
    grpTwelveOctetToChange= (GT_U32)inArgs[18];
    grpThirteenOctetToChange= (GT_U32)inArgs[19];
    grpFourteenOctetToChange= (GT_U32)inArgs[20];
    grpFifteenOctetToChange= (GT_U32)inArgs[21];
    grpSixteenOctetToChange= (GT_U32)inArgs[22];
    changeSrcAddr= (GT_BOOL)inArgs[23];
    srcFirstOctetToChange= (GT_U32)inArgs[24];
    srcSecondOctetToChange= (GT_U32)inArgs[25];
    srcThirdOctetToChange= (GT_U32)inArgs[26];
    srcForthOctetToChange= (GT_U32)inArgs[27];
    srcFifthOctetToChange= (GT_U32)inArgs[28];
    srcSixthOctetToChange= (GT_U32)inArgs[29];
    srcSeventhOctetToChange= (GT_U32)inArgs[30];
    srcEighthOctetToChange= (GT_U32)inArgs[31];
    srcNinthOctetToChange= (GT_U32)inArgs[32];
    srcTenthOctetToChange= (GT_U32)inArgs[33];
    srcElevenOctetToChange= (GT_U32)inArgs[34];
    srcTwelveOctetToChange= (GT_U32)inArgs[35];
    srcThirteenOctetToChange= (GT_U32)inArgs[36];
    srcFourteenOctetToChange= (GT_U32)inArgs[37];
    srcFifteenOctetToChange= (GT_U32)inArgs[38];
    srcSixteenOctetToChange= (GT_U32)inArgs[39];



    /* call cpss api function */
    result = cpssDxChIpLpmIpv6McPrefixGetManyByOctet(lpmDbId,
                                                        vrId,
                                                        grpStartIpAddr,
                                                        srcStartIpAddr,
                                                        numberOfPrefixesToGet,
                                                        changeGrpAddr,
                                                        grpFirstOctetToChange,
                                                        grpSecondOctetToChange,
                                                        grpThirdOctetToChange,
                                                        grpForthOctetToChange,
                                                        grpFifthOctetToChange,
                                                        grpSixthOctetToChange,
                                                        grpSeventhOctetToChange,
                                                        grpEighthOctetToChange,
                                                        grpNinthOctetToChange,
                                                        grpTenthOctetToChange,
                                                        grpElevenOctetToChange,
                                                        grpTwelveOctetToChange,
                                                        grpThirteenOctetToChange,
                                                        grpFourteenOctetToChange,
                                                        grpFifteenOctetToChange,
                                                        grpSixteenOctetToChange,
                                                        changeSrcAddr,
                                                        srcFirstOctetToChange,
                                                        srcSecondOctetToChange,
                                                        srcThirdOctetToChange,
                                                        srcForthOctetToChange,
                                                        srcFifthOctetToChange,
                                                        srcSixthOctetToChange,
                                                        srcSeventhOctetToChange,
                                                        srcEighthOctetToChange,
                                                        srcNinthOctetToChange,
                                                        srcTenthOctetToChange,
                                                        srcElevenOctetToChange,
                                                        srcTwelveOctetToChange,
                                                        srcThirteenOctetToChange,
                                                        srcFourteenOctetToChange,
                                                        srcFifteenOctetToChange,
                                                        srcSixteenOctetToChange,
                                                        &numOfRetrievedPrefixes);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfRetrievedPrefixes);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterAdd
*
* DESCRIPTION:
*   This function adds a virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_BAD_PARAM             - if wrong value in any of the parameters
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       Refer to cpssDxChIpLpmVirtualRouterSharedAdd for limitation when shared
*       virtual router is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1VirtualRouterAdd
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfig;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inFields[i++];

    vrConfig.supportIpv4Uc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;

    vrConfig.supportIpv4Mc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;


    vrConfig.supportIpv6Uc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;

    vrConfig.supportIpv6Mc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmVirtualRouterAdd(lpmDBId, vrId, &vrConfig);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterDel
*
* DESCRIPTION:
*   This function removes a virtual router in system for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - the LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK on success, or
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1VirtualRouterDel
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inFields[0];

    /* call cpss api function */
    result = cpssDxChIpLpmVirtualRouterDel(lpmDBId, vrId);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       vrConfigPtr    - Virtual router configuration
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1VirtualRouterGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfig;
    CPSS_DXCH_IP_LPM_SHADOW_TYPE_ENT        shadowType;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    result = cpssDxChIpLpmShadowTypeGet(lpmDBId, &shadowType);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* allocate the virtual routers skip list*/
    switch (shadowType)
    {
        case CPSS_DXCH_IP_LPM_TCAM_CHEETAH_SHADOW_E:
            maxNumOfVr = DXCH_MAX_VR_ID_CNS;
            break;
        case CPSS_DXCH_IP_LPM_TCAM_CHEETAH2_SHADOW_E:
            maxNumOfVr = DXCH2_MAX_VR_ID_CNS;
            break;
        case CPSS_DXCH_IP_LPM_TCAM_CHEETAH3_SHADOW_E:
            maxNumOfVr = DXCH3_MAX_VR_ID_CNS;
            break;
        case CPSS_DXCH_IP_LPM_TCAM_XCAT_SHADOW_E:
        case CPSS_DXCH_IP_LPM_TCAM_XCAT_POLICY_BASED_ROUTING_SHADOW_E:
            maxNumOfVr = DXCH_XCAT_MAX_VR_ID_CNS;
            break;
        case CPSS_DXCH_IP_LPM_RAM_LION3_SHADOW_E:
            maxNumOfVr = DXCH_LION3_MAX_VR_ID_CNS;
            break;
        default:
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
    }

    for (vrId = 0; vrId < maxNumOfVr; vrId++)
    {
        /* call cpss api function */
        result = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfig);
        if (result == GT_OK)
        {
            break;
        }
        else if (result != GT_NOT_FOUND)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    }

    if (vrId == maxNumOfVr)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = vrId;

    inFields[i++] = vrConfig.supportIpv4Uc;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    inFields[i++] = vrConfig.supportIpv4Mc;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    inFields[i++] = vrConfig.supportIpv6Uc;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    inFields[i++] = vrConfig.supportIpv6Mc;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterGet
*
* DESCRIPTION:
*   This function gets the virtual router in system for specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*
* OUTPUTS:
*       vrConfigPtr    - Virtual router configuration
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1VirtualRouterGetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfig;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    for (vrId++; vrId < maxNumOfVr; vrId++)
    {
        /* call cpss api function */
        result = cpssDxChIpLpmVirtualRouterGet(lpmDBId, vrId, &vrConfig);
        if (result == GT_OK)
        {
            break;
        }
        else if (result != GT_NOT_FOUND)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    }

    if (vrId == maxNumOfVr)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = vrId;

    inFields[i++] = vrConfig.supportIpv4Uc;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    inFields[i++] = vrConfig.supportIpv4Mc;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    inFields[i++] = vrConfig.supportIpv6Uc;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    inFields[i++] = vrConfig.supportIpv6Mc;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.routeType;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.numOfPaths;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = vrConfig.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], inFields[6], inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmVirtualRouterSharedAdd
*
* DESCRIPTION:
*   This function adds a shared virtual router in system for specific LPM DB.
*   Prefixes that reside within shared virtual router will participate in the
*   lookups of all virtual routers.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual's router ID.(APPLICABLE RANGES: 0..4095)
*       vrConfigPtr         - Virtual router configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - if success
*       GT_NOT_FOUND             - if the LPM DB id is not found
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_BAD_PTR               - if illegal pointer value
*       GT_BAD_STATE             - if the existing VR is not empty.
*       GT_NOT_APPLICABLE_DEVICE - if not applicable device
*
* COMMENTS:
*       1.  Only one shared virtual router can exists at any time within a
*           given LPM DB.
*       2.  Virtual router ID that is used for non-shared virtual router can't
*           be used for the shared virtual router and via versa.
*       3.  Shared prefixes can't overlap non-shared prefixes. If the shared
*           virtual router supports shared prefixes type then adding non-shared
*           virtual router that supports the same prefixes type will fail.
*           Also, if a non-shared virtual router that supports prefixes type
*           exists, then adding a shared virtual router that supports the same
*           prefixes type will fail.
*       4.  When the shared virtual router supports IPv4 UC prefixes, then the
*           the following will apply:
*             - The <match all> default TCAM entry will not be written to TCAM
*               (however TCAM entry will still be allocated to this entry)
*             - The following prefixes will be added to match all non-MC traffic:
*               0x00/1, 0x80/2, 0xC0/3, 0xF0/4
*           Same applies when the shared virtual router supports IPv6 UC
*           prefixes. The prefixes added to match all non-MC traffic are:
*           0x00/1, 0x80/2, 0xC0/3, 0xE0/4, 0xF0/5, 0xF8/6, 0xFC/7, 0xFE/8
*       5.  The application should not delete the non-MC prefixes (when added).
*       6.  Manipulation of the default UC will not be applied on the non-MC
*           prefixes. The application should manipulate those non-MC entries
*           directly when needed.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1VirtualRouterSharedAdd
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LPM_VR_CONFIG_STC          vrConfig;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];

    vrId = (GT_U32)inFields[i++];

    vrConfig.supportIpv4Uc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;

    vrConfig.supportIpv4Mc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv4McRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;

    vrConfig.supportIpv6Uc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6UcNextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;

    vrConfig.supportIpv6Mc = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.numOfPaths = (GT_U32)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    vrConfig.defIpv6McRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmVirtualRouterSharedAdd(lpmDBId, vrId, &vrConfig);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv4 prefix in a Virtual Router for
*   the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override the existing entry if it already exists
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory, or
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR                - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixAdd
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_BOOL                                 override;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpAddr(&ipAddr, (GT_U8*)inFields[i++]);
    prefixLen = (GT_U32)inFields[i++];
    nextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    nextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    nextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    override = (GT_BOOL)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                          &nextHopInfo, override);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv4 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixDel
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpAddr(&ipAddr, (GT_U8*)inFields[i++]);
    prefixLen = (GT_U32)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - TCAM column index of this uc prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex=0;
    GT_U32                                  tcamColumnIndex=0;
    GT_U32                                  i = 0;
    GT_IPADDR                               ipAddr;
    GT_U32                                  prefixLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    nextEntry.u32Ip = 0;
    nextEntryPrefixLen = 0;

    ipAddr.u32Ip=0;
    prefixLen=0;

    /* call cpss api function to get default ipv4 UC prefix */
    result = cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId,
                                          ipAddr,prefixLen,
                                          &nextHopInfo,
                                          &tcamRowIndex,
                                          &tcamColumnIndex);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = prefixLen;
    inFields[i++] = nextHopInfo.ipLttEntry.routeType;
    inFields[i++] = nextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;   /* override */
    inFields[i++] = tcamRowIndex;
    inFields[i++] = tcamColumnIndex;

    fieldOutput("%4b%d%d%d%d%d%d%d%d%d%d%d",
                ipAddr.arIP, inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8], inFields[9], inFields[10]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId      - The LPM DB id.
*       vrId         - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr    - The ip address to start the search from.
*       prefixLenPtr - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr            - The ip address of the found entry.
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - If found, this is the route entry info accosiated
*                              with this UC prefix.
*       tcamRowIndexPtr      - TCAM row index of this uc prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - TCAM column index of this uc prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixGetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex=0;
    GT_U32                                  tcamColumnIndex=0;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4UcPrefixGetNext(lpmDBId, vrId, &nextEntry,
                                              &nextEntryPrefixLen, &nextHopInfo,
                                              &tcamRowIndex, &tcamColumnIndex);
    if (result != GT_OK)
    {
        if (result == GT_NOT_FOUND)
        {
            result = GT_OK;
        }
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = nextEntryPrefixLen;
    inFields[i++] = nextHopInfo.ipLttEntry.routeType;
    inFields[i++] = nextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;   /* override */
    inFields[i++] = tcamRowIndex;
    inFields[i++] = tcamColumnIndex;

    fieldOutput("%4b%d%d%d%d%d%d%d%d%d%d%d",
                nextEntry.arIP, inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8], inFields[9], inFields[10]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the
*   information associated with it.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr       - if found, this is the route entry info.
*                               accosiated with this UC prefix.
*       tcamRowIndexPtr      - if found, TCAM row index of this UC prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - if found, TCAM column index of this UC prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixSearch
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                status;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   rowIndex;
    GT_U32                                   columnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &defUcNextHop,&rowIndex,&columnIndex);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d",
                 defUcNextHop.ipLttEntry.routeType,
                 defUcNextHop.ipLttEntry.numOfPaths,
                 defUcNextHop.ipLttEntry.routeEntryBaseIndex,
                 defUcNextHop.ipLttEntry.ucRPFCheckEnable,
                 defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable,
                 defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel,                 
                 GT_FALSE,
                 rowIndex,
                 columnIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr         - The prefix length of the found entry.
*       nextHopInfoPtr       - if found, this is the route entry info.
*                               accosiated with this UC prefix.
*       tcamRowIndexPtr      - if found, TCAM row index of this UC prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr   - if found, TCAM column index of this UC prefix.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                status;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   rowIndex;
    GT_U32                                   columnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipAddr, (GT_U8*)inArgs[2]);

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4UcPrefixGet(lpmDBId, vrId, ipAddr, &prefixLen,
                                              &defUcNextHop,&rowIndex,&columnIndex);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d%d%d",
                 prefixLen,
                 defUcNextHop.ipLttEntry.routeType,
                 defUcNextHop.ipLttEntry.numOfPaths,
                 defUcNextHop.ipLttEntry.routeEntryBaseIndex,
                 defUcNextHop.ipLttEntry.ucRPFCheckEnable,
                 defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable,
                 defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel,
                 GT_FALSE,
                 GT_FALSE,
                 rowIndex,
                 columnIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAdd
*
* DESCRIPTION:
*   Creates a new or override an existing Ipv6 prefix in a Virtual Router
*   for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of this prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*       nextHopInfoPtr  - the route entry info accosiated with this UC prefix.
*       override        - override the existing entry if it already exists
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If prefix length is too big.
*       GT_ERROR                 - If the vrId was not created yet.
*       GT_OUT_OF_CPU_MEM        - If failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - If failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented.
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       To change the default prefix for the VR use prefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixAdd
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPV6ADDR                             ipAddr;
    GT_U32                                  prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_BOOL                                 override;
    GT_BOOL                                 tcamDefragmentationEnable;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[i++]);
    prefixLen = (GT_U32)inFields[i++];
    nextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    nextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    nextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    nextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    override = (GT_BOOL)inFields[i++];
    tcamDefragmentationEnable = (GT_BOOL)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixAdd(lpmDBId, vrId, ipAddr, prefixLen,
                                          &nextHopInfo, override,
                                          tcamDefragmentationEnable);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixDel
*
* DESCRIPTION:
*   Deletes an existing Ipv6 prefix in a Virtual Router for the specified LPM
*   DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address of the prefix.
*       prefixLen       - The number of bits that are actual valid in the ipAddr.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If prefix length is too big, or
*       GT_ERROR                 - If the vrId was not created yet, or
*       GT_NO_SUCH               - If the given prefix doesn't exitst in the VR, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       the default prefix (prefixLen = 0) can't be deleted!
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixDel
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPV6ADDR                             ipAddr;
    GT_U32                                  prefixLen;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpv6Addr(&ipAddr, (GT_U8*)inFields[i++]);
    prefixLen = (GT_U32)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixDel(lpmDBId, vrId, ipAddr, prefixLen);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - If found, TCAM row index of this uc prefix.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - If found, TCAM column index of this uc prefix.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex=0;
    GT_U32                                  tcamColumnIndex=0;
    GT_U32                                  i = 0;
    GT_IPV6ADDR                             ipAddr;
    GT_U32                                  prefixLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    nextEntry_ipv6.u32Ip[0] = 0;
    nextEntry_ipv6.u32Ip[1] = 0;
    nextEntry_ipv6.u32Ip[2] = 0;
    nextEntry_ipv6.u32Ip[3] = 0;
    nextEntryPrefixLen = 0;

    ipAddr.u32Ip[0] = 0;
    ipAddr.u32Ip[1] = 0;
    ipAddr.u32Ip[2] = 0;
    ipAddr.u32Ip[3] = 0;
    prefixLen = 0;

    /* call cpss api function to get defualt ipv6 UC prefix */
    result = cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId,
                                             ipAddr,prefixLen,
                                             &nextHopInfo,
                                             &tcamRowIndex,
                                             &tcamColumnIndex);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = prefixLen;
    inFields[i++] = nextHopInfo.ipLttEntry.routeType;
    inFields[i++] = nextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = tcamRowIndex;
    inFields[i++] = tcamColumnIndex;

    fieldOutput("%16b%d%d%d%d%d%d%d%d%d%d%d%d",
                ipAddr.arIP, inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8], inFields[9], inFields[10],
                inFields[11]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGetNext
*
* DESCRIPTION:
*   This function returns an IP-Unicast prefix with larger (ip,prefix) than
*   the given one; it used for iterating over the exisiting prefixes.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId       - The LPM DB id.
*       vrId          - The virtual router Id to get the entry from.(APPLICABLE RANGES: 0..4095)
*       ipAddrPtr     - The ip address to start the search from.
*       prefixLenPtr  - Prefix length of ipAddr.
*
* OUTPUTS:
*       ipAddrPtr       - The ip address of the found entry.
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - If found, TCAM row index of this uc prefix.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - If found, TCAM column index of this uc prefix.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if no more entries are left in the IP table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixGetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT  nextHopInfo;
    GT_U32                                  tcamRowIndex=0;
    GT_U32                                  tcamColumnIndex=0;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6UcPrefixGetNext(lpmDBId, vrId, &nextEntry_ipv6,
                                              &nextEntryPrefixLen, &nextHopInfo,
                                              &tcamRowIndex, &tcamColumnIndex);
    if (result != GT_OK)
    {
        if (result == GT_NOT_FOUND)
        {
            result = GT_OK;
        }
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = nextEntryPrefixLen;
    inFields[i++] = nextHopInfo.ipLttEntry.routeType;
    inFields[i++] = nextHopInfo.ipLttEntry.numOfPaths;
    inFields[i++] = nextHopInfo.ipLttEntry.routeEntryBaseIndex;
    inFields[i++] = nextHopInfo.ipLttEntry.ucRPFCheckEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = tcamRowIndex;
    inFields[i++] = tcamColumnIndex;

    fieldOutput("%16b%d%d%d%d%d%d%d%d%d%d%d%d",
                nextEntry_ipv6.arIP, inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5], inFields[6],
                inFields[7], inFields[8], inFields[9], inFields[10],
                inFields[11]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixSearch
*
* DESCRIPTION:
*   This function searches for a given ip-uc address, and returns the next
*   hop pointer associated with it and TCAM prefix index.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*       prefixLen       - The number of bits that are actual valid in the
*                         ipAddr.
*
* OUTPUTS:
*       nextHopInfoPtr  - If  found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - if found, TCAM row index of this uc prefix.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixSearch
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                status;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   rowIndex;
    GT_U32                                   columnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);
    prefixLen = (GT_U32)inArgs[3];

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixSearch(lpmDBId, vrId, ipAddr, prefixLen,
                                              &defUcNextHop,&rowIndex,&columnIndex);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d",
                 defUcNextHop.ipLttEntry.routeType,
                 defUcNextHop.ipLttEntry.numOfPaths,
                 defUcNextHop.ipLttEntry.routeEntryBaseIndex,
                 defUcNextHop.ipLttEntry.ucRPFCheckEnable,
                 defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable,
                 defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel,
                 GT_FALSE,
                 rowIndex,
                 columnIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixGet
*
* DESCRIPTION:
*       This function gets a given ip address, find LPM match in the trie and
*       returns the prefix length and pointer to the next hop information bound
*       to the longest prefix match.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router id.(APPLICABLE RANGES: 0..4095)
*       ipAddr          - The destination IP address to look for.
*
* OUTPUTS:
*       prefixLenPtr    - The prefix length of the found entry.
*       nextHopInfoPtr  - If  found, the route entry info accosiated with
*                         this UC prefix.
*       tcamRowIndexPtr - if found, TCAM row index of this uc prefix.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamColumnIndexPtr - if found, TCAM column index of this uc prefix.
*                            (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                             DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if the required entry was found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given ip prefix was not found.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                status;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipAddr;
    GT_U32                                   prefixLen;
    CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT   defUcNextHop;
    GT_U32                                   rowIndex;
    GT_U32                                   columnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* zero defUcNextHop */
    cmdOsMemSet(&defUcNextHop,0,sizeof(defUcNextHop));

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipAddr, (GT_U8*)inArgs[2]);

    cpssOsMemSet(&defUcNextHop, 0, sizeof(CPSS_DXCH_IP_TCAM_ROUTE_ENTRY_INFO_UNT));
    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6UcPrefixGet(lpmDBId, vrId, ipAddr, &prefixLen,
                                              &defUcNextHop,&rowIndex,&columnIndex);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d",
                 prefixLen,
                 defUcNextHop.ipLttEntry.routeType,
                 defUcNextHop.ipLttEntry.numOfPaths,
                 defUcNextHop.ipLttEntry.routeEntryBaseIndex,
                 defUcNextHop.ipLttEntry.ucRPFCheckEnable,
                 defUcNextHop.ipLttEntry.sipSaCheckMismatchEnable,
                 defUcNextHop.ipLttEntry.ipv6MCGroupScopeLevel,
                 GT_FALSE,
                 rowIndex,
                 columnIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryAdd
*
* DESCRIPTION:
*   Add IP multicast route for a particular/all source and a particular
*   group address. this is done for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup             - The IP multicast group address.
*       ipGroupPrefixLen    - The number of bits that are actual valid in,
*                             the ipGroup.
*       ipSrc               - the root address for source base multi tree
*                             protocol.
*       ipSrcPrefixLen      - The number of bits that are actual valid in,
*                             the ipSrc.
*       mcRouteLttEntryPtr  - the LTT entry pointing to the MC route entry
*                             associated with this MC route.
*       override            - whether to override an mc Route pointer for the
*                             given prefix
*       tcamDefragmentationEnable - whether to enable performance costing
*                             de-fragmentation process in the case that there
*                             is no place to insert the prefix. To point of the
*                             process is just to make space for this prefix.
*                             relevant only if the LPM DB was created with
*                             partitionEnable = GT_FALSE.
*                             (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                              DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - if one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate TCAM memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4McEntryAdd
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPADDR                               ipGroup, ipSrc;
    GT_U32                                  ipGroupPrefixLen, ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_BOOL                                 override;
    GT_BOOL                                 tcamDefragmentationEnable;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpAddr(&ipGroup, (GT_U8*)inFields[i++]);
    ipGroupPrefixLen = (GT_U32)inFields[i++];
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[i++]);
    ipSrcPrefixLen = (GT_U32)inFields[i++];
    mcRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    mcRouteLttEntry.numOfPaths = (GT_U32)inFields[i++];
    mcRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    mcRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    mcRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    mcRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    override = (GT_BOOL)inFields[i++];
    tcamDefragmentationEnable = (GT_BOOL)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4McEntryAdd(lpmDBId, vrId, ipGroup, ipGroupPrefixLen,
                                         ipSrc, ipSrcPrefixLen,
                                         &mcRouteLttEntry, override,
                                         tcamDefragmentationEnable);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4McEntryDel
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPADDR                               ipGroup, ipSrc;
    GT_U32                                  ipGroupPrefixLen, ipSrcPrefixLen;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpAddr(&ipGroup, (GT_U8*)inFields[i++]);
    ipGroupPrefixLen = (GT_U32)inFields[i++];
    galtisIpAddr(&ipSrc, (GT_U8*)inFields[i++]);
    ipSrcPrefixLen = (GT_U32)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4McEntryDel(lpmDBId, vrId,
                                         ipGroup, ipGroupPrefixLen,
                                         ipSrc, ipSrcPrefixLen);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - The next ip Group address.
*       ipGroupPrefixLenPtr        - the ipGroup prefix Length.
*       ipSrcPtr                   - The next ip Source address.
*       ipSrcPrefixLenPtr          - ipSrc prefix length.
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The value of ipGroupPtr must be a valid value, it
*       means that it exists in the IP-Mc Table, unless this is the first
*       call to this function, then it's value is 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4McEntryGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  tcamGroupRowIndex=0;
    GT_U32                                  tcamGroupColumnIndex=0;
    GT_U32                                  tcamSrcRowIndex=0;
    GT_U32                                  tcamSrcColumnIndex=0;
    GT_U32                                  i = 0;
    GT_IPADDR                               ipGroup;
    GT_U32                                  ipGroupPrefixLen;
    GT_IPADDR                               ipSrc;
    GT_U32                                  ipSrcPrefixLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    nextEntry.u32Ip = 224;
    nextEntryPrefixLen = 0;
    srcNextEntry.u32Ip = 0;
    srcNextEntryPrefixLen = 0;


    ipGroup.arIP[0] = 224;
    ipGroup.arIP[1] = 0;
    ipGroup.arIP[2] = 0;
    ipGroup.arIP[3] = 0;
    ipGroupPrefixLen = 4;
    ipSrc.u32Ip = 0;
    ipSrcPrefixLen = 0;

    /* call cpss api function to get default ipv4 MC prefix */
    result = cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId,
                                           ipGroup,ipGroupPrefixLen,
                                           ipSrc,ipSrcPrefixLen,
                                           &mcRouteLttEntry,
                                           &tcamGroupRowIndex,
                                           &tcamGroupColumnIndex,
                                           &tcamSrcRowIndex,
                                           &tcamSrcColumnIndex);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = ipGroupPrefixLen;
    inFields[i++] = ipSrcPrefixLen;
    inFields[i++] = mcRouteLttEntry.routeType;
    inFields[i++] = mcRouteLttEntry.numOfPaths;
    inFields[i++] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = mcRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = tcamGroupRowIndex;
    inFields[i++] = tcamGroupColumnIndex;
    inFields[i++] = tcamSrcRowIndex;
    inFields[i++] = tcamSrcColumnIndex;

    fieldOutput("%4b%d%4b%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                ipGroup.arIP, inFields[0], ipSrc.arIP, inFields[1],
                inFields[2], inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8], inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - the ipGroup prefix Length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr                 - The next ip Group address.
*       ipGroupPrefixLenPtr        - the ipGroup prefix Length.
*       ipSrcPtr                   - The next ip Source address.
*       ipSrcPrefixLenPtr          - ipSrc prefix length.
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route.
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The value of ipGroupPtr must be a valid value, it
*       means that it exists in the IP-Mc Table, unless this is the first
*       call to this function, then it's value is 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4McEntryGetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  tcamGroupRowIndex=0;
    GT_U32                                  tcamGroupColumnIndex=0;
    GT_U32                                  tcamSrcRowIndex=0;
    GT_U32                                  tcamSrcColumnIndex=0;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call cpss api function */
    result = cpssDxChIpLpmIpv4McEntryGetNext(lpmDBId, vrId, &nextEntry,
                                             &nextEntryPrefixLen, &srcNextEntry,
                                             &srcNextEntryPrefixLen,
                                             &mcRouteLttEntry,
                                             &tcamGroupRowIndex,
                                             &tcamGroupColumnIndex,
                                             &tcamSrcRowIndex,
                                             &tcamSrcColumnIndex);
    if (result != GT_OK)
    {
        if (result == GT_NOT_FOUND)
        {
            result = GT_OK;
        }
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = nextEntryPrefixLen;
    inFields[i++] = srcNextEntryPrefixLen;
    inFields[i++] = mcRouteLttEntry.routeType;
    inFields[i++] = mcRouteLttEntry.numOfPaths;
    inFields[i++] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = mcRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = tcamGroupRowIndex;
    inFields[i++] = tcamGroupColumnIndex;
    inFields[i++] = tcamSrcRowIndex;
    inFields[i++] = tcamSrcColumnIndex;

    fieldOutput("%4b%d%4b%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                nextEntry.arIP, inFields[0], srcNextEntry.arIP, inFields[1],
                inFields[2], inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8], inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4McEntrySearch
*
* DESCRIPTION:
*   This function returns the multicast (ipSrc, ipGroup) entry, used
*   to find specific multicast adrress entry, and ipSrc + ipGroup
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr         - the LTT entry pointer pointing to the MC route
*                                    entry associated with this MC route.
*       tcamGroupRowIndexPtr       - pointer to TCAM group row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamGroupColumnIndexPtr    - pointer to TCAM group column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr         - pointer to TCAM source row  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcColumnIndexPtr      - pointer to TCAM source column  index.
*                                    (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond;
*                                     DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4McEntrySearch
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                status;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPADDR                                ipGroup;
    GT_U32                                   ipGroupPrefixLen;
    GT_IPADDR                                ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC               mcRouteLttEntry;
    GT_U32                                   gRowIndex;
    GT_U32                                   gColumnIndex;
    GT_U32                                   sRowIndex;
    GT_U32                                   sColumnIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpAddr(&ipGroup, (GT_U8*)inArgs[2]);
    ipGroupPrefixLen = (GT_U32)inArgs[3];
    galtisIpAddr(&ipSrc, (GT_U8*)inArgs[4]);
    ipSrcPrefixLen = (GT_U32)inArgs[5];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv4McEntrySearch(lpmDBId, vrId, ipGroup,
                                             ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry,
                                             &gRowIndex, &gColumnIndex,
                                             &sRowIndex, &sColumnIndex);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d%d%d",
                 mcRouteLttEntry.routeType,
                 mcRouteLttEntry.numOfPaths,
                 mcRouteLttEntry.routeEntryBaseIndex,
                 mcRouteLttEntry.ucRPFCheckEnable,
                 mcRouteLttEntry.sipSaCheckMismatchEnable,
                 mcRouteLttEntry.ipv6MCGroupScopeLevel,
                 GT_FALSE,
                 gRowIndex,
                 gColumnIndex,
                 sRowIndex,
                 sColumnIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryAdd
*
* DESCRIPTION:
*   To add the multicast routing information for IP datagrams from a particular
*   source and addressed to a particular IP multicast group address for a
*   specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual private network identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*       mcRouteLttEntryPtr - the LTT entry pointing to the MC route entry
*                            associated with this MC route.
*       override        - weather to override the mcRoutePointerPtr for the
*                         given prefix
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big.
*       GT_ERROR                 - if the virtual router does not exist.
*       GT_OUT_OF_CPU_MEM        - if failed to allocate CPU memory.
*       GT_OUT_OF_PP_MEM         - if failed to allocate PP memory.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented .
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       to override the default mc route use ipGroup = ipGroupPrefixLen = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6McEntryAdd
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPV6ADDR                             ipGroup, ipSrc;
    GT_U32                                  ipGroupPrefixLen, ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_BOOL                                 override;
    GT_BOOL                                 tcamDefragmentationEnable;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpv6Addr(&ipGroup, (GT_U8*)inFields[i++]);
    ipGroupPrefixLen = (GT_U32)inFields[i++];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inFields[i++]);
    ipSrcPrefixLen = (GT_U32)inFields[i++];
    mcRouteLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    mcRouteLttEntry.numOfPaths = (GT_U32)inFields[i++];
    mcRouteLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    mcRouteLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    mcRouteLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    mcRouteLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    override = (GT_BOOL)inFields[i++];
    tcamDefragmentationEnable = (GT_BOOL)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6McEntryAdd(lpmDBId, vrId, ipGroup,
                                         ipGroupPrefixLen, ipSrc,
                                         ipSrcPrefixLen, &mcRouteLttEntry,
                                         override, tcamDefragmentationEnable);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryDel
*
* DESCRIPTION:
*   To delete a particular mc route entry for a specific LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId         - The LPM DB id.
*       vrId            - The virtual router identifier.(APPLICABLE RANGES: 0..4095)
*       ipGroup         - The IP multicast group address.
*       ipGroupPrefixLen- The number of bits that are actual valid in,
*                         the ipGroup.
*       ipSrc           - the root address for source base multi tree protocol.
*       ipSrcPrefixLen  - The number of bits that are actual valid in,
*                         the ipSrc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success, or
*       GT_OUT_OF_RANGE         -  If one of prefixes' lengths is too big, or
*       GT_ERROR                 - if the virtual router does not exist, or
*       GT_NOT_FOUND             - if the (ipGroup,prefix) does not exist, or
*       GT_FAIL                  - otherwise.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  In order to delete the multicast entry and all the src ip addresses
*           associated with it, call this function with ipSrc = ipSrcPrefix = 0.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6McEntryDel
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_IPV6ADDR                             ipGroup, ipSrc;
    GT_U32                                  ipGroupPrefixLen, ipSrcPrefixLen;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    galtisIpv6Addr(&ipGroup, (GT_U8*)inFields[i++]);
    ipGroupPrefixLen = (GT_U32)inFields[i++];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inFields[i++]);
    ipSrcPrefixLen = (GT_U32)inFields[i++];

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6McEntryDel(lpmDBId, vrId,
                                         ipGroup, ipGroupPrefixLen,
                                         ipSrc, ipSrcPrefixLen);
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroup prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupPrefixLenPtr  - ipGroup prefix length.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*       values, it means that they exist in the IP-Mc Table, unless this is
*       the first call to this function, then the value of (ipGroupPtr,
*       ipSrcPtr) is (0,0).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6McEntryGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  tcamGroupRowIndex=0;
    GT_U32                                  tcamSrcRowIndex=0;
    GT_U32                                  i = 0;
    GT_IPV6ADDR                             ipGroup;
    GT_U32                                  ipGroupPrefixLen;
    GT_IPV6ADDR                             ipSrc;
    GT_U32                                  ipSrcPrefixLen;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];

    nextEntry_ipv6.u32Ip[0] = 0xff;
    nextEntry_ipv6.u32Ip[1] = 0;
    nextEntry_ipv6.u32Ip[2] = 0;
    nextEntry_ipv6.u32Ip[3] = 0;
    nextEntryPrefixLen = 0;
    srcNextEntry_ipv6.u32Ip[0] = 0;
    srcNextEntry_ipv6.u32Ip[1] = 0;
    srcNextEntry_ipv6.u32Ip[2] = 0;
    srcNextEntry_ipv6.u32Ip[3] = 0;
    srcNextEntryPrefixLen = 0;


    ipGroup.u32Ip[0] = 0xff;
    ipGroup.u32Ip[1] = 0;
    ipGroup.u32Ip[2] = 0;
    ipGroup.u32Ip[3] = 0;
    ipGroupPrefixLen = 8;
    ipSrc.u32Ip[0] = 0;
    ipSrc.u32Ip[1] = 0;
    ipSrc.u32Ip[2] = 0;
    ipSrc.u32Ip[3] = 0;
    ipSrcPrefixLen = 0;

    /* call cpss api function to get default ipv6 MC prefix */
    result = cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId,
                                            ipGroup,ipGroupPrefixLen,
                                            ipSrc,ipSrcPrefixLen,
                                            &mcRouteLttEntry,
                                            &tcamGroupRowIndex,
                                            &tcamSrcRowIndex);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = ipGroupPrefixLen;
    inFields[i++] = ipSrcPrefixLen;
    inFields[i++] = mcRouteLttEntry.routeType;
    inFields[i++] = mcRouteLttEntry.numOfPaths;
    inFields[i++] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = mcRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = tcamGroupRowIndex;
    inFields[i++] = tcamSrcRowIndex;

    fieldOutput("%16b%d%16b%d%d%d%d%d%d%d%d%d%d%d%d",
                ipGroup.arIP, inFields[0], ipSrc.arIP, inFields[1],
                inFields[2], inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8], inFields[9],
                inFields[10], inFields[11], inFields[12]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntryGetNext
*
* DESCRIPTION:
*   This function returns the next multicast (ipSrc, ipGroup) entry, used
*   to iterate over the exisiting multicast addresses for a specific LPM DB,
*   and ipSrc + ipGroup.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; xCat2.
*
* INPUTS:
*       lpmDBId             - The LPM DB id.
*       vrId                - The virtual router Id.(APPLICABLE RANGES: 0..4095)
*       ipGroupPtr          - The ip Group address to get the next entry for.
*       ipGroupPrefixLenPtr - ipGroup prefix length.
*       ipSrcPtr            - The ip Source address to get the next entry for.
*       ipSrcPrefixLenPtr   - ipSrc prefix length.
*
* OUTPUTS:
*       ipGroupPtr           - The next ip Group address.
*       ipGroupPrefixLenPtr  - ipGroup prefix length.
*       ipSrcPtr             - The next ip Source address.
*       ipSrcPrefixLenPtr    - ipSrc prefix length.
*       mcRouteLttEntryPtr   - the LTT entry pointer pointing to the MC route
*                              entry associated with this MC route.
*       tcamGroupRowIndexPtr - pointer to TCAM group row  index.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*       tcamSrcRowIndexPtr   - pointer to TCAM source row  index.
*                              (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                               DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
*
* RETURNS:
*       GT_OK                    - if found.
*       GT_OUT_OF_RANGE          - if one of prefix length is too big.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*       GT_NOT_FOUND             - if the given address is the last one on the IP-Mc table.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       The values of (ipGroupPtr,ipGroupPrefixLenPtr) must be a valid
*       values, it means that they exist in the IP-Mc Table, unless this is
*       the first call to this function, then the value of (ipGroupPtr,
*       ipSrcPtr) is (0,0).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6McEntryGetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    CPSS_DXCH_IP_LTT_ENTRY_STC              mcRouteLttEntry;
    GT_U32                                  tcamGroupRowIndex=0;
    GT_U32                                  tcamSrcRowIndex=0;
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call cpss api function */
    result = cpssDxChIpLpmIpv6McEntryGetNext(lpmDBId, vrId, &nextEntry_ipv6,
                                             &nextEntryPrefixLen,
                                             &srcNextEntry_ipv6,
                                             &srcNextEntryPrefixLen,
                                             &mcRouteLttEntry,
                                             &tcamGroupRowIndex,
                                             &tcamSrcRowIndex);
    if (result != GT_OK)
    {
        if (result == GT_NOT_FOUND)
        {
            result = GT_OK;
        }
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[i++] = nextEntryPrefixLen;
    inFields[i++] = srcNextEntryPrefixLen;
    inFields[i++] = mcRouteLttEntry.routeType;
    inFields[i++] = mcRouteLttEntry.numOfPaths;
    inFields[i++] = mcRouteLttEntry.routeEntryBaseIndex;
    inFields[i++] = mcRouteLttEntry.ucRPFCheckEnable;
    inFields[i++] = mcRouteLttEntry.sipSaCheckMismatchEnable;
    inFields[i++] = mcRouteLttEntry.ipv6MCGroupScopeLevel;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = GT_FALSE;
    inFields[i++] = tcamGroupRowIndex;
    inFields[i++] = tcamSrcRowIndex;

    i = 0;
    fieldOutput("%16b%d%16b%d%d%d%d%d%d%d%d%d%d%d%d",
                nextEntry_ipv6.arIP, inFields[0], srcNextEntry_ipv6.arIP, inFields[1],
                inFields[2], inFields[3], inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8], inFields[9],
                inFields[10], inFields[11], inFields[12]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6McEntrySearch
*
* DESCRIPTION:
*   This function returns the muticast (ipSrc,ipGroup) entry, used
*   to find specific multicast adrress entry.
*
* APPLICABLE DEVICES:
*       All TCAM based routing ASICS
*
* INPUTS:
*       lpmDBId          - The LPM DB id.
*       vrId             - The virtual router Id.
*                          future support only , currently only vrId = 0 is
*                          supported.
*       ipGroup          - The ip Group address to get the next entry for.
*       ipGroupPrefixLen - The ip Group prefix len.
*       ipSrc            - The ip Source address to get the next entry for.
*       ipSrcPrefixLen   - ipSrc prefix length.
*
* OUTPUTS:
*       mcRouteLttEntryPtr - the LTT entry pointer pointing to the MC route
*                            entry associated with this MC route.
*
* RETURNS:
*       GT_OK if found, or
*       GT_NOT_FOUND - If the given address is the last one on the IP-Mc table.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6McEntrySearch
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                status;
    GT_U32                                   lpmDBId;
    GT_U32                                   vrId;
    GT_IPV6ADDR                              ipGroup;
    GT_U32                                   ipGroupPrefixLen;
    GT_IPV6ADDR                              ipSrc;
    GT_U32                                   ipSrcPrefixLen;
    CPSS_DXCH_IP_LTT_ENTRY_STC               mcRouteLttEntry;
    GT_U32                                   gRowIndex;
    GT_U32                                   sRowIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    vrId = (GT_U32)inArgs[1];
    galtisIpv6Addr(&ipGroup, (GT_U8*)inArgs[2]);
    ipGroupPrefixLen = (GT_U32)inArgs[3];
    galtisIpv6Addr(&ipSrc, (GT_U8*)inArgs[4]);
    ipSrcPrefixLen = (GT_U32)inArgs[5];

    /* call cpss api function */
    status =  cpssDxChIpLpmIpv6McEntrySearch(lpmDBId, vrId, ipGroup,
                                             ipGroupPrefixLen, ipSrc,
                                             ipSrcPrefixLen, &mcRouteLttEntry,
                                             &gRowIndex, &sRowIndex);

    galtisOutput(outArgs, status, "%d%d%d%d%d%d%d%d%d",
                 mcRouteLttEntry.routeType,
                 mcRouteLttEntry.numOfPaths,
                 mcRouteLttEntry.routeEntryBaseIndex,
                 mcRouteLttEntry.ucRPFCheckEnable,
                 mcRouteLttEntry.sipSaCheckMismatchEnable,
                 mcRouteLttEntry.ipv6MCGroupScopeLevel,
                 GT_FALSE,
                 gRowIndex,
                 sRowIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixBulkSetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    gIpv4PrefixArrayLen = (GT_U32)inArgs[1];

    if (gIpv4PrefixArray == NULL)
    {
        gIpv4PrefixArray = (CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC*)cmdOsMalloc
            (gIpv4PrefixArrayLen * sizeof(CPSS_DXCH_IP_LPM_IPV4_UC_PREFIX_STC));
        if (gIpv4PrefixArray == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    bulkIndex = 0;
    gIpv4PrefixArray[bulkIndex].vrId = (GT_U32)inFields[i++];
    galtisIpAddr(&gIpv4PrefixArray[bulkIndex].ipAddr, (GT_U8*)inFields[i++]);
    gIpv4PrefixArray[bulkIndex].prefixLen = (GT_U32)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    gIpv4PrefixArray[bulkIndex].override = (GT_BOOL)inFields[i++];
    gIpv4PrefixArray[bulkIndex].returnStatus = (GT_STATUS)inFields[i++];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixBulkSetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    bulkIndex++;
    if (bulkIndex >= gIpv4PrefixArrayLen)
    {
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    gIpv4PrefixArray[bulkIndex].vrId = (GT_U32)inFields[i++];
    galtisIpAddr(&gIpv4PrefixArray[bulkIndex].ipAddr, (GT_U8*)inFields[i++]);
    gIpv4PrefixArray[bulkIndex].prefixLen = (GT_U32)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    gIpv4PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    gIpv4PrefixArray[bulkIndex].override = (GT_BOOL)inFields[i++];
    gIpv4PrefixArray[bulkIndex].returnStatus = (GT_STATUS)inFields[i++];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv4UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv4 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv4PrefixArrayLen - Length of UC prefix array.
*       ipv4PrefixArrayPtr - The UC prefix array.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE         -  on one of prefixes' lengths is too big
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv4UcPrefixBulkEndSet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WRAP_BULK_FUNCTION_ENT                  bulkFunction;
    GT_STATUS                               result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    bulkFunction = (WRAP_BULK_FUNCTION_ENT)inArgs[2];

    /* call cpss api function */
    if (bulkFunction == BULK_ADD)
    {
        result = cpssDxChIpLpmIpv4UcPrefixAddBulk(lpmDBId, gIpv4PrefixArrayLen,
                                                  gIpv4PrefixArray);
    }
    else /* BULK_DELETE */
    {
        result = cpssDxChIpLpmIpv4UcPrefixDelBulk(lpmDBId, gIpv4PrefixArrayLen,
                                                  gIpv4PrefixArray);
    }
    cmdOsFree(gIpv4PrefixArray);
    gIpv4PrefixArray = NULL;

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixBulkSetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    lpmDBId = (GT_U32)inArgs[0];
    gIpV6PrefixArrayLen = (GT_U32)inArgs[1];
    bulkDefragmentationEnable = (GT_BOOL)inArgs[2];

    if (gIpV6PrefixArray == NULL)
    {
        gIpV6PrefixArray = (CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC*)cmdOsMalloc
            (gIpV6PrefixArrayLen * sizeof(CPSS_DXCH_IP_LPM_IPV6_UC_PREFIX_STC));
        if (gIpV6PrefixArray == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    bulkIndex = 0;
    gIpV6PrefixArray[bulkIndex].vrId = (GT_U32)inFields[i++];
    galtisIpv6Addr(&gIpV6PrefixArray[bulkIndex].ipAddr, (GT_U8*)inFields[i++]);
    gIpV6PrefixArray[bulkIndex].prefixLen = (GT_U32)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    gIpV6PrefixArray[bulkIndex].override = (GT_BOOL)inFields[i++];
    gIpV6PrefixArray[bulkIndex].returnStatus = (GT_STATUS)inFields[i++];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixBulkSetNext
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                                  i = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    bulkIndex++;
    if (bulkIndex >= gIpV6PrefixArrayLen)
    {
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    gIpV6PrefixArray[bulkIndex].vrId = (GT_U32)inFields[i++];
    galtisIpv6Addr(&gIpV6PrefixArray[bulkIndex].ipAddr, (GT_U8*)inFields[i++]);
    gIpV6PrefixArray[bulkIndex].prefixLen = (GT_U32)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeType = (CPSS_DXCH_IP_ROUTE_ENTRY_METHOD_ENT)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.numOfPaths = (GT_U32)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.routeEntryBaseIndex = (GT_U32)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ucRPFCheckEnable = (GT_BOOL)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.sipSaCheckMismatchEnable = (GT_BOOL)inFields[i++];
    gIpV6PrefixArray[bulkIndex].nextHopInfo.ipLttEntry.ipv6MCGroupScopeLevel = (CPSS_IPV6_PREFIX_SCOPE_ENT)inFields[i++];
    /* bit removed from CPSS */
    i++;
    gIpV6PrefixArray[bulkIndex].override = (GT_BOOL)inFields[i++];
    gIpV6PrefixArray[bulkIndex].returnStatus = (GT_STATUS)inFields[i++];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmIpv6UcPrefixAddBulk
*
* DESCRIPTION:
*   Creates a new or override an existing bulk of Ipv6 prefixes in a Virtual
*   Router for the specified LPM DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       lpmDBId            - The LPM DB id.
*       ipv6PrefixArrayLen - Length of UC prefix array.
*       ipv6PrefixArrayPtr - The UC prefix array.
*       tcamDefragmentationEnable - wether to enable performance costing
*                         de-fragmentation process in the case that there is no
*                         place to insert the prefix. To point of the process is
*                         just to make space for this prefix.
*                         relevant only if the LPM DB was created with
*                         partitionEnable = GT_FALSE.
*                         (APPLICABLE DEVICES: DxCh1; DxCh1_Diamond; DxCh2;
*                          DxCh3; xCat; xCat3; Lion; xCat2; Lion2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_OUT_OF_RANGE          - on one of prefixes' lengths is too big.
*       GT_NOT_IMPLEMENTED       - if this request is not implemented
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*       GT_BAD_PTR               - if one of the parameters is NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1Ipv6UcPrefixBulkEndSet
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    WRAP_BULK_FUNCTION_ENT                  bulkFunction;
    GT_STATUS                               result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    bulkFunction = (WRAP_BULK_FUNCTION_ENT)inArgs[2];

    /* call cpss api function */
    if (bulkFunction == BULK_ADD)
    {
        result = cpssDxChIpLpmIpv6UcPrefixAddBulk(lpmDBId, gIpV6PrefixArrayLen,
                                                  gIpV6PrefixArray,
                                                  bulkDefragmentationEnable);
    }
    else /* BULK_DELETE */
    {
        result = cpssDxChIpLpmIpv6UcPrefixDelBulk(lpmDBId, gIpV6PrefixArrayLen,
                                                  gIpV6PrefixArray);
    }
    cmdOsFree(gIpV6PrefixArray);
    gIpV6PrefixArray = NULL;

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevListGet
*
* DESCRIPTION:
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDbId        - the LPM DB id.
*       numOfDevsPtr   - he size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr   - the number of devices retrieved
*       devListArray   - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to
*                                  the device list
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1DBDevsListGetFirst
(
 IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
 IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lpmDBId = (GT_U32)inArgs[0];
    gNumOfDevs = (GT_U32)inArgs[1];
    if (gDevList == NULL)
    {
        gDevList = (GT_U8*)cmdOsMalloc(sizeof(GT_U8));
        if (gDevList == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* call cpss api function */
    result = cpssDxChIpLpmDBDevListGet(lpmDBId, &gNumOfDevs, gDevList);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    devIndex = 0;

    if (gNumOfDevs > 0)
    {
        inFields[0] = devIndex;
        inFields[1] = gDevList[0];
    }

    fieldOutput("%d%d", inFields[0],  inFields[1]);
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpLpmDBDevListGet
*
* DESCRIPTION:
*       This function retrieves the list of devices in an existing LPM DB.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       lpmDbId        - the LPM DB id.
*       numOfDevsPtr   - he size of devListArray
*
* OUTPUTS:
*       numOfDevsPtr   - the number of devices retrieved
*       devListArray   - array of device ids in the LPM DB
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the LPM DB id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_SIZE              - in case not enough memory was allocated to
*                                  the device list
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The application should allocate memory for the device list array.
*       If the function returns GT_OK, then number of devices holds
*       the number of devices the function filled.
*       If the function returns GT_BAD_SIZE, then the memory allocated by the
*       application to the device list is not enough. In this case the
*       number of devices will hold the size of array needed.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpLpm1DBDevsListGetNext
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

    devIndex++;

    if (devIndex >= gNumOfDevs)
    {
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }
    inFields[0] = devIndex;
    inFields[1] = gDevList[0];

    fieldOutput("%d%d", inFields[0], inFields[1]);
    galtisOutput(outArgs, GT_OK, "%f");
    cmdOsFree(gDevList);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChIpLpmDBCreate",
        &wrCpssDxChIpLpmDBCreate,
        9, 0},

    {"cpssDxChIpLpmDBDelete",
        &wrCpssDxChIpLpmDBDelete,
        1, 0},

    {"cpssDxChIpLpmDBConfigGet",
        &wrCpssDxChIpLpmDBConfigGet,
        1, 0},

    {"cpssDxChIpLpmDBDevListSetFirst",
        &wrCpssDxChIpLpmDBDevListAddSetFirst,
        0, 2},

    {"cpssDxChIpLpmDBDevListSetNext",
        &wrCpssDxChIpLpmDBDevListAddSetNext,
        0, 2},

    {"cpssDxChIpLpmDBDevListEndSet",
        &wrCpssDxChIpLpmDBDevListAddEndSet,
        1, 0},

    {"cpssDxChIpLpmDBDevListDelete",
        &wrCpssDxChIpLpmDBDevsListRemove,
        1, 1},
    {"cpssDxChIpLpmVirtualRouterAddPCL",
        &wrCpssDxChIpLpmVirtualRouterAdd_PCL,
        39, 0},

    {"cpssDxChIpLpmVirtualRouterAddPCL_2SetFirst",
        &wrCpssDxChIpLpmVirtualRouterAddPCL_2SetFirst,
        2, 36},
    {"cpssDxChIpLpmVirtualRouterAddPCL_2SetNext",
        &wrCpssDxChIpLpmVirtualRouterAddPCL_2SetNext,
        2, 36},

    {"cpssDxChIpLpmVirtualRouterAddPCL_2EndSet",
        &wrCpssDxChIpLpmVirtualRouterAddPCL_2EndSet,
        0, 0},

    {"cpssDxChIpLpmVirtualRouterAddPCL_2GetFirst",
        &wrCpssDxChIpLpmVirtualRouterAddPCL_2GetFirst,
        0, 0},

    {"cpssDxChIpLpmVirtualRouterAddPCL_2GetNext",
        &wrCpssDxChIpLpmVirtualRouterAddPCL_2GetNext,
        0, 0},


    {"cpssDxChIpLpmVirtualRouterAdd_PCLSet",
        &wrCpssDxChIpLpmVirtualRouterAddPCLEntrySet,
        0, 38},

    {"cpssDxChIpLpmVirtualRouterAddLTT",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT,
        15, 0},

    {"cpssDxChIpLpmVirtualRouterAdd_LTTSet",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntrySet,
        0, 15},

    {"cpssDxChIpLpmVirtualRouterAdd_LTTGetFirst",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntryGetFirst,
        0, 0},

    {"cpssDxChIpLpmVirtualRouterAdd_LTTGetNext",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_EntryGetNext,
        0, 0},

    {"cpssDxChIpLpmVirtualRouterAdd_LTT_1Set",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntrySet,
        0, 18},

    {"cpssDxChIpLpmVirtualRouterAdd_LTT_1GetFirst",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGetFirst,
        0, 0},

    {"cpssDxChIpLpmVirtualRouterAdd_LTT_1GetNext",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_1_EntryGetNext,
        0, 0},
    {"cpssDxChIpLpmVirtualRouterAdd_LTT_2Set",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntrySet,
        0, 31},

    {"cpssDxChIpLpmVirtualRouterAdd_LTT_2GetFirst",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGetFirst,
        0, 0},

    {"cpssDxChIpLpmVirtualRouterAdd_LTT_2GetNext",
        &wrCpssDxChIpLpmVirtualRouterAdd_LTT_2_EntryGetNext,
        0, 0},


    {"cpssDxChIpLpmVirtualRouterDel",
        &wrCpssDxChIpLpmVirtualRouterDel,
        2, 0},

    {"cpssDxChIpLpmIpv4UcPrefixAdd_PCL",
        &wrCpssDxChIpLpmIpv4UcPrefixAdd_PCL,
        41, 0},

    {"cpssDxChIpLpmIpv4UcPrefixAdd_PCLSet",
        &wrCpssDxChIpLpmIpv4UcPrefixAddPCLEntrySet,
        0, 40},

    {"cpssDxChIpLpmIpv4UcPrefixAdd_LTT",
        &wrCpssDxChIpLpmIpv4UcPrefixAdd_LTT,
        17, 0},

    {"cpssDxChIpLpmIpv4UcPrefixAdd_LTTSet",
        &wrCpssDxChIpLpmIpv4UcPrefixAddLTT_EntrySet,
        0, 17},

    {"cpssDxChIpLpmIpv4UcPrefixClear",
        &wrCpssDxChIpLpmIpv4UcPrefixesFlush,
        2, 0},

    {"cpssDxChIpLpmIpv4UcPrefixesFlush",
        &wrCpssDxChIpLpmIpv4UcPrefixesFlushVr,
        2, 0},

    {"cpssDxChIpLpmIpv4UcPrefixSearch_PCL",
        &wrCpssDxChIpLpmIpv4UcPrefixSearch_PCL,
        4, 0},

    {"cpssDxChIpLpmIpv4UcPrefixSearch_LTT",
        &wrCpssDxChIpLpmIpv4UcPrefixSearch_LTT,
        4, 0},

    {"cpssDxChIpLpmIpv4UcPrefixDelete",
        &wrCpssDxChIpLpmIpv4UcPrefixDel,
        2, 44},

    {"cpssDxChIpLpmIpv4UcPrefixSetFirst",
        &wrCpssDxChIpLpmIpv4UcPrefixAdd,
        2, 44},

    {"cpssDxChIpLpmIpv4UcPrefixSetNext",
        &wrCpssDxChIpLpmIpv4UcPrefixAdd,
        2, 44},

    {"cpssDxChIpLpmIpv4UcPrefixEndSet",
        &wrCpssDxChIpLpmIpv4UcPrefixEndSet,
        2, 0},

    {"cpssDxChIpLpmIpv4UcPrefixGetFirst",
        &wrCpssDxChIpLpmIpv4UcPrefixGetNext_GetFirst,
        2, 0},

    {"cpssDxChIpLpmIpv4UcPrefixGetNext",
        &wrCpssDxChIpLpmIpv4UcPrefixGetNext_GetNext,
        2, 0},

    {"cpssDxChIpLpmIpv4McSet",
        &wrCpssDxChIpLpmIpv4McEntryAdd,
        1, 14},

    {"cpssDxChIpLpmIpv4McDelete",
        &wrCpssDxChIpLpmIpv4McEntryDel,
        1, 14},

    {"cpssDxChIpLpmIpv4McClear",
        &wrCpssDxChIpLpmIpv4McEntriesFlush,
        1, 0},

    {"cpssDxChIpLpmIpv4McPrefixesFlush",
        &wrCpssDxChIpLpmIpv4McEntriesFlushVr,
        2,0},

    {"cpssDxChIpLpmIpv4McGetFirst",
        &wrCpssDxChIpLpmIpv4McEntryGetNext_GetFirst,
        1, 0},

    {"cpssDxChIpLpmIpv4McGetNext",
        &wrCpssDxChIpLpmIpv4McEntryGetNext_GetNext,
        1, 0},

    {"cpssDxChIpLpmIpv4McEntrySearch",
        &wrCpssDxChIpLpmIpv4McEntrySearch,
        6, 0},

    {"cpssDxChIpLpmIpv6UcPrefixAdd_PCL",
        &wrCpssDxChIpLpmIpv6UcPrefixAdd_PCL,
        42, 0},

    {"cpssDxChIpLpmIpv6UcPrefixAdd_PCLSet",
        &wrCpssDxChIpLpmIpv6UcPrefixAddPCLEntrySet,
        0, 41},

    {"cpssDxChIpLpmIpv6UcPrefixAdd_LTT",
        &wrCpssDxChIpLpmIpv6UcPrefixAdd_LTT,
        12, 0},

    {"cpssDxChIpLpmIpv6UcPrefixAdd_LTTSet",
        &wrCpssDxChIpLpmIpv6UcPrefixAdd_LTTEntrySet,
        0, 12},

    {"cpssDxChIpLpmIpv6UcPrefixSetFirst",
        &wrCpssDxChIpLpmIpv6UcPrefixAdd,
        3, 44},

    {"cpssDxChIpLpmIpv6UcPrefixSetNext",
        &wrCpssDxChIpLpmIpv6UcPrefixAdd,
        3, 44},

    {"cpssDxChIpLpmIpv6UcPrefixEndSet",
        &wrCpssDxChIpLpmIpv6UcPrefixEndSet,
        3, 0},

    {"cpssDxChIpLpmIpv6UcPrefixGetFirst",
        &wrCpssDxChIpLpmIpv6UcPrefixGetNext_GetFirst,
        3, 0},

    {"cpssDxChIpLpmIpv6UcPrefixGetNext",
        &wrCpssDxChIpLpmIpv6UcPrefixGetNext_GetNext,
        3, 0},

    {"cpssDxChIpLpmIpv6UcPrefixDelete",
        &wrCpssDxChIpLpmIpv6UcPrefixDel,
        3, 44},

    {"cpssDxChIpLpmIpv6UcPrefixClear",
        &wrCpssDxChIpLpmIpv6UcPrefixesFlush,
        3, 0},

    {"cpssDxChIpLpmIpv6UcPrefixesFlush",
        &wrCpssDxChIpLpmIpv6UcPrefixesFlushVr,
        2, 0},

    {"cpssDxChIpLpmIpv6UcPrefixSearch_PCL",
        &wrCpssDxChIpLpmIpv6UcPrefixSearch_PCL,
        4, 0},

    {"cpssDxChIpLpmIpv6UcPrefixSearch_LTT",
        &wrCpssDxChIpLpmIpv6UcPrefixSearch_LTT,
        4, 0},

    {"cpssDxChIpLpmIpv6McSet",
        &wrCpssDxChIpLpmIpv6McEntryAdd,
        1, 14},

    {"cpssDxChIpLpmIpv6McDelete",
        &wrCpssDxChIpLpmIpv6McEntryDel,
        1, 14},

    {"cpssDxChIpLpmIpv6McClear",
        &wrCpssDxChIpLpmIpv6McEntriesFlush,
        1, 0},

    {"cpssDxChIpLpmIpv6McPrefixesFlush",
        &wrCpssDxChIpLpmIpv6McEntriesFlushVr,
        2,0},

    {"cpssDxChIpLpmIpv6McGetFirst",
        &wrCpssDxChIpLpmIpv6McEntryGetNext_GetFirst,
        1, 0},

    {"cpssDxChIpLpmIpv6McGetNext",
        &wrCpssDxChIpLpmIpv6McEntryGetNext_GetNext,
        1, 0},

    {"cpssDxChIpLpmIpv6McEntrySearch",
        &wrCpssDxChIpLpmIpv6McEntrySearch,
        6, 0},

    {"cpssDxChIpLpmDBMemSizeGet",
        &wrCpssDxChIpLpmDBMemSizeGet,
        4, 0},

    {"cpssDxChIpLpmDBExport",
        &wrCpssDxChIpLpmDBExport,
        3, 0},

    {"cpssDxChIpLpmDBImport",
        &wrCpssDxChIpLpmDBImport,
        4, 0},

    {"cpssDxChIpLpmDBSimpleExport",
        &wrCpssDxChIpLpmDBSimpleExport,
        3, 0},

    {"cpssDxChIpLpmDBSimpleImport",
        &wrCpssDxChIpLpmDBSimpleImport,
        3, 0},
    {"cpssDxChIpLpmDump",
         &wrCpssDxChIpLpmDump,
         4, 0},
    {"cpssDxChIpLpmIpv4UcPrefix_IP_LTTGetFirst",
        &wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetFirst,
        4, 0},
    {"cpssDxChIpLpmIpv4UcPrefix_IP_LTTGetNext",
        &wrCpssDxChIpLpmIpv4UcPrefix_IP_LTTGetNext,
        4, 0},
    {"cpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetFirst",
        &wrCpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetFirst,
        4, 0},
    {"cpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetNext",
        &wrCpssDxChIpLpmIpv4UcPrefix_PCL_ACTIONGetNext,
        4, 0},

    {"cpssDxChIpLpmIpv6UcPrefix_IP_LTTGetFirst",
        &wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetFirst,
        4, 0},
    {"cpssDxChIpLpmIpv6UcPrefix_IP_LTTGetNext",
        &wrCpssDxChIpLpmIpv6UcPrefix_IP_LTTGetNext,
        4, 0},
    {"cpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetFirst",
        &wrCpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetFirst,
        4, 0},
    {"cpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetNext",
        &wrCpssDxChIpLpmIpv6UcPrefix_PCL_ACTIONGetNext,
        4, 0},
    {"cpssDxChIpLpmIpv4McEntryGetFirst",
        &wrCpssDxChIpLpmIpv4McEntryGetFirst,
        6, 0},
    {"cpssDxChIpLpmIpv4McEntryGetNext",
        &wrCpssDxChIpLpmIpv4McEntryGetNext,
        6, 0},
    {"cpssDxChIpLpmIpv6McEntryGetFirst",
        &wrCpssDxChIpLpmIpv6McEntryGetFirst,
        6, 0},
    {"cpssDxChIpLpmIpv6McEntryGetNext",
        &wrCpssDxChIpLpmIpv6McEntryGetNext,
        6, 0},
    {"cpssDxChIpLpmPolicyBasedRoutingDefaultMcSet",
        &wrCpssDxChIpLpmPolicyBasedRoutingDefaultMcSet,
        1, 0},
    {"cpssDxChIpLpmPolicyBasedRoutingDefaultMcGet",
        &wrCpssDxChIpLpmPolicyBasedRoutingDefaultMcGet,
        1, 0},

    {"cpssDxChIpLpmTcamLinesReserveSetFirst",
        &wrCpssDxChIpLpmTcamLinesReserveSetFirst,
        0, 6},
    {"cpssDxChIpLpmTcamLinesReserveSetNext",
        &wrCpssDxChIpLpmTcamLinesReserveSetNext,
        0, 6},
    {"cpssDxChIpLpmTcamLinesReserveEndSet",
        &wrCpssDxChIpLpmTcamLinesReserveEndSet,
        0, 0},

    {"cpssDxChIpLpmDBCapacityUpdate",
        &wrCpssDxChIpLpmDBCapacityUpdate,
        6, 0},
    {"cpssDxChIpLpmDBCapacityGet",
        &wrCpssDxChIpLpmDBCapacityGet,
        1, 0},
    {"cpssDxChIpLpmIpv4UcPrefixAddMany",
        &wrCpssDxChIpLpmIpv4UcPrefixAddMany,
        6, 0},
    {"cpssDxChIpLpmIpv4UcPrefixAddManyRandom",
        &wrCpssDxChIpLpmIpv4UcPrefixAddManyRandom,
        8, 0},
    {"cpssDxChIpLpmIpv4UcPrefixAddManyByOctet",
     &wrCpssDxChIpLpmIpv4UcPrefixAddManyByOctet,
     9, 0},
    {"cpssDxChIpLpmIpv4UcPrefixDelManyByOctet",
     &wrCpssDxChIpLpmIpv4UcPrefixDelManyByOctet,
     8, 0},
    {"cpssDxChIpLpmIpv4UcPrefixGetManyByOctet",
     &wrCpssDxChIpLpmIpv4UcPrefixGetManyByOctet,
     9, 0},
    {"cpssDxChIpLpmIpv6UcPrefixAddManyByOctet",
     &wrCpssDxChIpLpmIpv6UcPrefixAddManyByOctet,
     21, 0},
    {"cpssDxChIpLpmIpv6UcPrefixGetManyByOctet",
     &wrCpssDxChIpLpmIpv6UcPrefixGetManyByOctet,
     21, 0},
    {"cpssDxChIpLpmIpv4McPrefixAddManyByOctet",
     &wrCpssDxChIpLpmIpv4McPrefixAddManyByOctet,
     16, 0},
    {"cpssDxChIpLpmIpv4McPrefixGetManyByOctet",
     &wrCpssDxChIpLpmIpv4McPrefixGetManyByOctet,
     16, 0},
    {"cpssDxChIpLpmIpv6McPrefixAddManyByOctet",
     &wrCpssDxChIpLpmIpv6McPrefixAddManyByOctet,
     40, 0},
    {"cpssDxChIpLpmIpv6McPrefixGetManyByOctet",
     &wrCpssDxChIpLpmIpv6McPrefixGetManyByOctet,
     40, 0},

    /* wrappers for eArch - the wrappers support only devices that use
       ipLttEntry. Devices that use pclIpUcAction can't use these wrappers */

    {"cpssDxChIpLpm1DBCreate",
    &wrCpssDxChIpLpmDBCreate,
    9, 0},

    {"cpssDxChIpLpm1DBCreate_1",
    &wrCpssDxChIpLpmDBCreate_1,
    3, 0},

    {"cpssDxChIpLpm1DBDelete",
    &wrCpssDxChIpLpmDBDelete,
    1, 0},

    {"cpssDxChIpLpm1DBConfigGet",
    &wrCpssDxChIpLpmDBConfigGet,
    1, 0},

    {"cpssDxChIpLpm1DBConfigGet_1",
    &wrCpssDxChIpLpmDBConfigGet_1,
    1, 0},

    {"cpssDxChIpLpm1DBCapacityUpdate",
    &wrCpssDxChIpLpmDBCapacityUpdate,
    6, 0},

    {"cpssDxChIpLpm1DBCapacityGet",
    &wrCpssDxChIpLpmDBCapacityGet,
    1, 0},

    {"cpssDxChIpLpm1DBDevListSetFirst",
    &wrCpssDxChIpLpmDBDevListAddSetFirst,
    0, 2},

    {"cpssDxChIpLpm1DBDevListSetNext",
    &wrCpssDxChIpLpmDBDevListAddSetNext,
    0, 2},

    {"cpssDxChIpLpm1DBDevListEndSet",
    &wrCpssDxChIpLpmDBDevListAddEndSet,
    1, 0},

    {"cpssDxChIpLpm1DBDevListDelete",
    &wrCpssDxChIpLpmDBDevsListRemove,
    1, 2},

    {"cpssDxChIpLpm1DBDevListGetFirst",
    &wrCpssDxChIpLpm1DBDevsListGetFirst,
    2, 2},

    {"cpssDxChIpLpm1DBDevListGetNext",
    &wrCpssDxChIpLpm1DBDevsListGetNext,
    2, 2},

    {"cpssDxChIpLpm1TcamLinesReserveSetFirst",
    &wrCpssDxChIpLpmTcamLinesReserveSetFirst,
    0, 6},

    {"cpssDxChIpLpm1TcamLinesReserveSetNext",
    &wrCpssDxChIpLpmTcamLinesReserveSetNext,
    0, 6},

    {"cpssDxChIpLpm1TcamLinesReserveEndSet",
    &wrCpssDxChIpLpmTcamLinesReserveEndSet,
    0, 0},

    {"cpssDxChIpLpm1VirtualRouterSet",
    &wrCpssDxChIpLpm1VirtualRouterAdd,
    1, 33},

    {"cpssDxChIpLpm1VirtualRouterDelete",
    &wrCpssDxChIpLpm1VirtualRouterDel,
    1, 33},

    {"cpssDxChIpLpm1VirtualRouterGetFirst",
    &wrCpssDxChIpLpm1VirtualRouterGetFirst,
    1, 0},

    {"cpssDxChIpLpm1VirtualRouterGetNext",
    &wrCpssDxChIpLpm1VirtualRouterGetNext,
    1, 0},

    {"cpssDxChIpLpm1VirtualRouterSharedSet",
    &wrCpssDxChIpLpm1VirtualRouterSharedAdd,
    1, 33},

    {"cpssDxChIpLpm1Ipv4UcPrefixSet",
    &wrCpssDxChIpLpm1Ipv4UcPrefixAdd,
    2, 12},

    {"cpssDxChIpLpm1Ipv4UcPrefixDelete",
    &wrCpssDxChIpLpm1Ipv4UcPrefixDel,
    2, 12},

    {"cpssDxChIpLpm1Ipv4UcPrefixGetFirst",
    &wrCpssDxChIpLpm1Ipv4UcPrefixGetFirst,
    2, 0},

    {"cpssDxChIpLpm1Ipv4UcPrefixGetNext",
    &wrCpssDxChIpLpm1Ipv4UcPrefixGetNext,
    2, 0},

    {"cpssDxChIpLpm1Ipv4UcPrefixClear",
    &wrCpssDxChIpLpmIpv4UcPrefixesFlush,
    2, 0},

    {"cpssDxChIpLpm1Ipv4UcPrefixSearch",
    wrCpssDxChIpLpm1Ipv4UcPrefixSearch,
    4, 0},

    {"cpssDxChIpLpm1Ipv4UcPrefixGet",
    wrCpssDxChIpLpm1Ipv4UcPrefixGet,
    3, 0},

    {"cpssDxChIpLpm1Ipv4UcPrefixesFlush",
    &wrCpssDxChIpLpmIpv4UcPrefixesFlushVr,
    2, 0},

    {"cpssDxChIpLpm1Ipv6UcPrefixSet",
    &wrCpssDxChIpLpm1Ipv6UcPrefixAdd,
    2, 13},

    {"cpssDxChIpLpm1Ipv6UcPrefixDelete",
    &wrCpssDxChIpLpm1Ipv6UcPrefixDel,
    2, 13},

    {"cpssDxChIpLpm1Ipv6UcPrefixGetFirst",
    &wrCpssDxChIpLpm1Ipv6UcPrefixGetFirst,
    2, 0},

    {"cpssDxChIpLpm1Ipv6UcPrefixGetNext",
    &wrCpssDxChIpLpm1Ipv6UcPrefixGetNext,
    2, 0},

    {"cpssDxChIpLpm1Ipv6UcPrefixClear",
    &wrCpssDxChIpLpmIpv6UcPrefixesFlush,
    2, 0},

    {"cpssDxChIpLpm1Ipv6UcPrefixSearch",
    wrCpssDxChIpLpm1Ipv6UcPrefixSearch,
    4, 0},

    {"cpssDxChIpLpm1Ipv6UcPrefixGet",
    wrCpssDxChIpLpm1Ipv6UcPrefixGet,
    3, 0},

    {"cpssDxChIpLpm1Ipv6UcPrefixesFlush",
    &wrCpssDxChIpLpmIpv4UcPrefixesFlushVr,
    2, 0},

    {"cpssDxChIpLpm1Ipv4McEntrySet",
    &wrCpssDxChIpLpm1Ipv4McEntryAdd,
    2, 17},

    {"cpssDxChIpLpm1Ipv4McEntryDelete",
    &wrCpssDxChIpLpm1Ipv4McEntryDel,
    2, 17},

    {"cpssDxChIpLpm1Ipv4McEntryGetFirst",
    &wrCpssDxChIpLpm1Ipv4McEntryGetFirst,
    2, 0},

    {"cpssDxChIpLpm1Ipv4McEntryGetNext",
    &wrCpssDxChIpLpm1Ipv4McEntryGetNext,
    2, 0},

    {"cpssDxChIpLpm1Ipv4McEntryClear",
    &wrCpssDxChIpLpmIpv4McEntriesFlush,
    2, 0},

    {"cpssDxChIpLpm1Ipv4McEntrySearch",
    wrCpssDxChIpLpm1Ipv4McEntrySearch,
    6, 0},

    {"cpssDxChIpLpm1Ipv4McEntriesFlush",
    &wrCpssDxChIpLpmIpv4McEntriesFlushVr,
    2, 0},

    {"cpssDxChIpLpm1Ipv6McEntrySet",
    &wrCpssDxChIpLpm1Ipv6McEntryAdd,
    2, 15},

    {"cpssDxChIpLpm1Ipv6McEntryDelete",
    &wrCpssDxChIpLpm1Ipv6McEntryDel,
    2, 15},

    {"cpssDxChIpLpm1Ipv6McEntryGetFirst",
    &wrCpssDxChIpLpm1Ipv6McEntryGetFirst,
    2, 0},

    {"cpssDxChIpLpm1Ipv6McEntryGetNext",
    &wrCpssDxChIpLpm1Ipv6McEntryGetNext,
    2, 0},

    {"cpssDxChIpLpm1Ipv6McEntryClear",
    &wrCpssDxChIpLpmIpv6McEntriesFlush,
    2, 0},

    {"cpssDxChIpLpm1Ipv6McEntrySearch",
    wrCpssDxChIpLpm1Ipv6McEntrySearch,
    6, 0},

    {"cpssDxChIpLpm1Ipv6McEntriesFlush",
    &wrCpssDxChIpLpmIpv6McEntriesFlushVr,
    2, 0},

    {"cpssDxChIpLpm1Ipv4UcPrefixBulkSetFirst",
    &wrCpssDxChIpLpm1Ipv4UcPrefixBulkSetFirst,
    3, 12},

    {"cpssDxChIpLpm1Ipv4UcPrefixBulkSetNext",
    &wrCpssDxChIpLpm1Ipv4UcPrefixBulkSetNext,
    3, 12},

    {"cpssDxChIpLpm1Ipv4UcPrefixBulkEndSet",
    &wrCpssDxChIpLpm1Ipv4UcPrefixBulkEndSet,
    3, 0},

    {"cpssDxChIpLpm1Ipv6UcPrefixBulkSetFirst",
    &wrCpssDxChIpLpm1Ipv6UcPrefixBulkSetFirst,
    4, 12},

    {"cpssDxChIpLpm1Ipv6UcPrefixBulkSetNext",
    &wrCpssDxChIpLpm1Ipv6UcPrefixBulkSetNext,
    4, 12},

    {"cpssDxChIpLpm1Ipv6UcPrefixBulkEndSet",
    &wrCpssDxChIpLpm1Ipv6UcPrefixBulkEndSet,
    4, 0},

    {"cpssDxChIpLpm1DBMemSizeGet",
    &wrCpssDxChIpLpmDBMemSizeGet,
    4, 0},

    {"cpssDxChIpLpm1DBExport",
    &wrCpssDxChIpLpmDBExport,
    3, 0},

    {"cpssDxChIpLpm1DBImport",
    &wrCpssDxChIpLpmDBImport,
    4, 0},

    {"cpssDxChIpLpm1DBSimpleExport",
    &wrCpssDxChIpLpmDBSimpleExport,
    3, 0},

    {"cpssDxChIpLpm1DBSimpleImport",
    &wrCpssDxChIpLpmDBSimpleImport,
    3, 0}

    /************ cpssDxChIpLpmDBCapacityTable end *************/

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChIpLpm
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
GT_STATUS cmdLibInitCpssDxChIpLpm
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

/*******************************************************************************
* cmdRouterTcamDump
*
* DESCRIPTION:
*     make physical router tcam scanning and prints its contens.
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
GT_STATUS cmdRouterTcamDump
(
    GT_BOOL dump
)
{
    return dumpRouteTcam( dump);
}

