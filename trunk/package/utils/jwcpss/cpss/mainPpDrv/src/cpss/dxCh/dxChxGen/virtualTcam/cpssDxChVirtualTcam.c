/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChVirtualTcam.c
*
* DESCRIPTION:
*       The CPSS DXCH High Level Virtual TCAM Manager
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcam.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcamDb.h>
#include <cpss/dxCh/dxChxGen/tcam/private/prvCpssDxChTcam.h>
#include <cpss/dxCh/dxChxGen/pcl/private/prvCpssDxChPcl.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwTables.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/private/prvCpssDxChVirtualTcamLog.h>
#include <cpss/generic/log/prvCpssLog.h>
#include <cpssCommon/private/prvCpssSkipList.h>

/* TCAM manager DB */
static PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_PTR vTcamMngDB[CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS + 1] = {0};

/* indication to allow Rule To Be Absent From Tcam During Priority Update */
static GT_BOOL  allowRuleToBeAbsentFromTcamDuringPriorityUpdate = GT_FALSE;

/* debug flag to enable the use of debug_writeRulePosition
    to allow 'ruleWrite' to add rule at the start of the priority
    (and not to the end) */
static GT_BOOL      debug_writeRulePosition_enable = GT_FALSE;
static CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   debug_writeRulePosition = CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E;
/* function to force (or not to force) the 'ruleWrite' API to use 'first' position.
   (by default it uses the 'last' position)
 debug mode to allow quick testing of the 'position first' that
   the 'rule Update' API allow to use. */
void  prvCpssDxChVirtualTcamDbVTcam_debug_writeRulePositionForceFirstSet(
    GT_BOOL forceFirstPosition
)
{
    debug_writeRulePosition_enable = forceFirstPosition;
}

static GT_STATUS perDevice_ruleValidate
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_BOOL                                     valid
);
static GT_STATUS perDevice_ruleMove
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                                      srcRuleId,
    IN  GT_U32                                      dstRuleId,
    IN  GT_BOOL                                     moveOrCopy
);
static GT_STATUS ruleMoveByLogicalIndex
(
    IN  GT_U32                                      vTcamMngId,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr,
    IN  GT_U32                                      srcLogicalIndex,
    IN  GT_U32                                      srcDeviceRuleIndex,
    IN  GT_U32                                      dstLogicalIndex,
    IN  GT_U32                                      dstDeviceRuleIndex,
    IN  GT_BOOL                                     calledFromResize
);
GT_STATUS priorityTableEntryRemoveLogicalIndexUpdateRange
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC           *vTcamMngDBPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vtcamInfoPtr,
    IN   GT_U32                                       priority,
    IN   GT_U32                                       logicalIndex
);

static GT_STATUS copyAllRulesFromSrcDevToDevBmp(
    IN GT_U32   vTcamMngId,
    IN GT_U8    representativeDevNum ,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP newDevsBmp
);

static GT_STATUS rawEntryFormatOperationReadFromHw
(
    IN  GT_U8                        devNum,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                        ruleIndex
);

static GT_STATUS rawEntryFormatOperationWriteToHw
(
    IN  GT_U8                        devNum,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                        ruleIndex
);

/*******************************************************************************
 * prvCpssDxChVirtualTcamRuleFormatToRuleSize
 *
 * Description:
 *     This Function maps PCL TCAM rule formats to virtual TCAM size enumeration.
 *
 * APPLICABLE DEVICES:
 *        xCat3; Bobcat2; Caelum; Bobcat3.
 *
 * NOT APPLICABLE DEVICES:
 *        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
 *
 * INPUTS:
 *       ruleFormat      - PCL rule format.
 *
 * OUTPUT:
 *       ruleSizePtr   - [Pointer to] rule size corresponding to ruleFormat
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters (invalid rule format)
 *
 *  Comments:
 *       None
 *
 *******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamRuleFormatToRuleSize
(
    IN  CPSS_DXCH_PCL_RULE_FORMAT_TYPE_ENT  ruleFormat,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT *ruleSizePtr
)
{
    switch (ruleFormat)
    {
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_NOT_IP_E                     /*0*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IP_L2_QOS_E                  /*1*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_L4_E                    /*2*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV6_DIP_E                   /*3*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_NOT_IP_E                      /*7*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IP_L2_QOS_E                   /*8*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_STD_IPV4_L4_E                     /*9*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_UDB_E                        /*13*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_STD_IPV4_ROUTED_ACL_QOS_E        /*15*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_30_E                         /*23*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_30_E                          /*29*/  :

            *ruleSizePtr = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E;
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_NOT_IPV6_E                   /*4*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L2_E                    /*5*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV6_L4_E                    /*6*/   :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_NOT_IPV6_E                    /*10*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L2_E                     /*11*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV6_L4_E                     /*12*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_UDB_E                        /*14*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_EXT_IPV4_PORT_VLAN_QOS_E         /*16*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_EXT_IPV4_RACL_VACL_E              /*19*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_60_E                         /*26*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_60_E                          /*32*/  :

            *ruleSizePtr= CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E;
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_10_E                         /*21*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_10_E                          /*27*/  :

            *ruleSizePtr = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E;
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_20_E                         /*22*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_20_E                          /*28*/  :

            *ruleSizePtr = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E;
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_40_E                         /*24*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_40_E                          /*30*/  :

            *ruleSizePtr = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E;
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_UDB_50_E                         /*25*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_UDB_50_E                          /*31*/  :

            *ruleSizePtr = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E;
            break;

        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_PORT_VLAN_QOS_E       /*17*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_INGRESS_ULTRA_IPV6_ROUTED_ACL_QOS_E      /*18*/  :
        case CPSS_DXCH_PCL_RULE_FORMAT_EGRESS_ULTRA_IPV6_RACL_VACL_E            /*20*/  :
        default:

            /* Rule format is not supported at virtual tcam */
            return GT_BAD_PARAM;
    }

    return GT_OK;
}

/*******************************************************************************
 * prvCpssDxChVirtualTcamRuleIndexToRulePclIndex
 *
 * Description:
 *     This Function maps virtual TCAM rule index to the index to be used on PCL TCAM API,
 *      depending on device family.
 *
 * APPLICABLE DEVICES:
 *        xCat3; Bobcat2; Caelum; Bobcat3.
 *
 * NOT APPLICABLE DEVICES:
 *        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
 *
 * INPUTS:
 *       devnum      - Device number.
 *       ruleSize       - Rule size corresponding to ruleFormat
 *       ruleIndex      - Virtual TCAM rule index.
 *
 * OUTPUT:
 *       rulePclIndexPtr   - [Pointer to] rule index to be used on PCL TCAM API
 *
 * RETURNS:
 *       GT_OK                    - on success
 *       GT_BAD_PARAM             - on wrong parameters (either wrong rule size or incorrect alignment)
 *
 *  Comments:
 *       None
 *
 *******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamRuleIndexToRulePclIndex
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                                      ruleIndex,
    OUT GT_U32                                     *rulePclIndexPtr
)
{
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        /* No Adjustment needed on BC2/3/BobK */
        *rulePclIndexPtr = ruleIndex;
    }
    else
    {
        /* Adjust the rule index according to xCat3 PCL index rules */
        switch (ruleSize)
        {
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E:

                /* Std. rule */
                if ((ruleIndex % 3) != 0)
                {
                    /* Must be correctly aligned */
                    return GT_BAD_PARAM;
                }
                *rulePclIndexPtr = ruleIndex / 3;
                break;

            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E:

                /* Ext. rule like 6 10-byte rules */
                if ((ruleIndex % 6) != 0)
                {
                    /* Must be correctly aligned */
                    return GT_BAD_PARAM;
                }
                *rulePclIndexPtr = ruleIndex / 6;
                break;

            default:
                return GT_BAD_PARAM;
        }
    }

    return GT_OK;
}

/* get tcam size */
#define GET_TCAM_SIZE_MAC(_devNum,_tcamType)  \
    PRV_CPSS_DXCH_PP_MAC(_devNum)->fineTuning.tableSize._tcamType

/*
 * typedef: enum RESIZE_MODE_ENT
 *
 * Description:
 *      Resize modes.
 *
 * Enumerations:
 *
 *     RESIZE_MODE_WITH_INPUT_INDEX_E     - Move rules with Rule pointed by index
 *     RESIZE_MODE_WITHOUT_INPUT_INDEX_E  - Move rules without Rule pointed by index
 *
 */
typedef enum{
    RESIZE_MODE_WITH_INPUT_INDEX_E      ,
    RESIZE_MODE_WITHOUT_INPUT_INDEX_E

}RESIZE_MODE_ENT;

static GT_STATUS vtcamDbSegmentTableVTcamResize
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  GT_U32                                      resizeValue,
    IN  RESIZE_MODE_ENT                             resizeMode,
    IN  GT_U32                                      resizeReferenceLogicalIndex
);

/*
 * prvCpssDxChVirtualTcamManagerBlockToLookupMap
 *
 * Description:
 *     This Function maps TCAM segment to lookup (i.e. pair of client_group and hit_number).
 *
 * INPUTS:
 *       cookiePtr      - device info passed to callback function.
 *       baseRow        - base row of mapped segment.
 *       baseColumn     - base column of mapped segment.
 *       lookupId       - id of lookup for mapping.
 *
 *  Comments:
 *
 */
static GT_STATUS prvCpssDxChVirtualTcamManagerBlockToLookupMap
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      baseRow,
    IN  GT_U32      baseColumn,
    IN  GT_U32      lookupId
)
{
    GT_STATUS rc;          /* return code      */
    GT_U32    vTcamMngId;  /* vTCAM Manager Id */
    GT_U8     devNum;      /* device number    */
    GT_U32    i, min, max; /* loop bounds      */
    GT_U32    floorIndex;  /* floor Index      */
    CPSS_DXCH_TCAM_BLOCK_INFO_STC  floorInfoArr[CPSS_DXCH_TCAM_MAX_NUM_BLOCKS_CNS];

    vTcamMngId = (GT_U32)((char*)cookiePtr - (char*)0);

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    floorIndex = baseRow / 256;

    /* iteration over the devices */
    do
    {
        rc = cpssDxChTcamIndexRangeHitNumAndGroupGet(
            devNum, floorIndex, floorInfoArr);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(! PRV_CPSS_SIP_5_15_CHECK_MAC(devNum))
        {
            /* BC2, BC3 */
            if (baseColumn < 6)
            {
                min = 0;
                max = 1;
            }
            else
            {
                min = 1;
                max = 2;
            }
        }
        else
        {
            /* Caelum */
            if (baseColumn < 6)
            {
                min = 0;
                max = 3;
            }
            else
            {
                min = 3;
                max = 6;
            }
        }

        for (i = min; (i < max); i++)
        {
            floorInfoArr[i].group  =
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_TO_CLIENT_GROUP_MAC(lookupId);
            floorInfoArr[i].hitNum =
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_TO_HIT_NUMBER_MAC(lookupId);
        }

        rc = cpssDxChTcamIndexRangeHitNumAndGroupSet(
            devNum, floorIndex, floorInfoArr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    return GT_OK;
}

/*
 * prvCpssDxChVirtualTcamManagerRuleInvalidate
 *
 * Description:
 *     This Function invalidates rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr            - device info passed to callback function.
 *       rulePhysicalIndex    - rule physical index.
 *       ruleSize             - rule size in TCAM minimal rules (10 bytes)
 *
 *  Comments:
 *
 */
static GT_STATUS prvCpssDxChVirtualTcamManagerRuleInvalidate
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      rulePhysicalIndex,
    IN  GT_U32      ruleSize
)
{
    GT_STATUS rc;         /* return code      */
    GT_U32    vTcamMngId; /* vTCAM Manager Id */
    GT_U8     devNum;     /* device number    */

    vTcamMngId = (GT_U32)((char*)cookiePtr - (char*)0);

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* iteration over the devices */
    do
    {
        /* invaliddate the rule in the device */
        rc = perDevice_ruleValidate(
            devNum,
            rulePhysicalIndex,
            (CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT) (ruleSize - 1),
            GT_FALSE /*valid*/);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    return GT_OK;
}

/*
 * prvCpssDxChVirtualTcamManagerRuleMove
 *
 * Description:
 *     This Function moves rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr               - device info passed to callback function.
 *       srcRulePhysicalIndex    - source rule physical index.
 *       dstRulePhysicalIndex    - destination rule physical index.
 *       ruleSize                - rule size in TCAM minimal rules (10 bytes)
 *
 *  Comments:
 *
 */
static GT_STATUS prvCpssDxChVirtualTcamManagerRuleMove
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      srcRulePhysicalIndex,
    IN  GT_U32      dstRulePhysicalIndex,
    IN  GT_U32      ruleSize
)
{
    GT_STATUS rc;         /* return code      */
    GT_U32    vTcamMngId; /* vTCAM Manager Id */
    GT_U8     devNum;     /* device number    */
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT ruleSizeEnum;

    vTcamMngId = (GT_U32)((char*)cookiePtr - (char*)0);
    ruleSizeEnum = (CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT)
        (CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E + (ruleSize - 1));

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* iteration over the devices */
    do
    {
        /* move the rule in the device */
        rc = perDevice_ruleMove(
            devNum, ruleSizeEnum,
            srcRulePhysicalIndex,dstRulePhysicalIndex,GT_TRUE/*move*/);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChVirtualTcamManagerInit
*
* DESCRIPTION:
*       Initialize Virtual TCAM manager structure.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                  - device number
*       vTcamMngId              - virtual TCAM Manager ID
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                   - on success
*       GT_OUT_OF_CPU_MEM       - on CPU memory alocation fail
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamManagerInit
(
    IN  GT_U8                                               devNum,
    IN  GT_U32                                              vTcamMngId
)
{
    GT_STATUS                                            rc; /* return code */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_CFG_STC        tcamSegCfg;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr;

    pVTcamMngPtr = vTcamMngDB[vTcamMngId];

    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        tcamSegCfg.tcamColumnsAmount        = 12;
        tcamSegCfg.tcamRowsAmount           = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelTerm /
                                              tcamSegCfg.tcamColumnsAmount;
        tcamSegCfg.lookupColumnAlignment    = 6;
        tcamSegCfg.lookupRowAlignment       = 256;
    }
    else
    {
        /*
         * xCat3 settings:
         *   fineTuning.policyTcamRaws contains number of PCL Tcam rows,
         *   and each row contains either 4 standard or 2 extended rules.
         *   At the virtual Tcam, 6 columns are the width of either 2 standard
         *   or 1 extended rule, that's half a PCL Tcam row.
         *   tcamColumnsAmount must be 6 columns in order to
         *   insure contigous allocation,
         *   So that at the virtual Tcam, number of rows is double the number
         *   of PCL Tcam rows.
         */
        tcamSegCfg.tcamColumnsAmount        = 6;
        tcamSegCfg.tcamRowsAmount           = 2 * PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws;
        tcamSegCfg.lookupColumnAlignment    = tcamSegCfg.tcamColumnsAmount; /* On xCat3 PCL: Only one huge block */
        tcamSegCfg.lookupRowAlignment       = tcamSegCfg.tcamRowsAmount;
    }
    /* cookie for using prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst/Next */
    tcamSegCfg.cookiePtr                = (void*)((char*)0 + vTcamMngId);
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        tcamSegCfg.lookupMapFuncPtr         = prvCpssDxChVirtualTcamManagerBlockToLookupMap;
    }
    else
    {
        tcamSegCfg.lookupMapFuncPtr         = NULL;
    }
    tcamSegCfg.ruleInvalidateFuncPtr    = prvCpssDxChVirtualTcamManagerRuleInvalidate;
    tcamSegCfg.ruleMoveFuncPtr          = prvCpssDxChVirtualTcamManagerRuleMove;
    /* set maximal amount of segments as amount of 10-bytes rules in all of TCAM */
    tcamSegCfg.maxSegmentsInDb = (tcamSegCfg.tcamColumnsAmount * tcamSegCfg.tcamRowsAmount);

    /* currently this parameter not come from the application so set it here
       according to actual tcam size */
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        pVTcamMngPtr->tcamNumEntriesInSmallestRuleSize = GET_TCAM_SIZE_MAC(devNum,tunnelTerm);
    }
    else
    {
        /*
         * xCat3 settings:
         *   fineTuning.policyTcamRaws contains number of PCL Tcam rows,
         *   and each row contains either 4 standard or 2 extended rules.
         *   So that the number of entries in smallest rule size (i.e. standard
         *   rule size) is four times the number of PCL Tcam rows.
         */
        pVTcamMngPtr->tcamNumEntriesInSmallestRuleSize = GET_TCAM_SIZE_MAC(devNum,policyTcamRaws) * 4;
    }

    pVTcamMngPtr->vTcamCfgPtrArrSize = CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS + 1;
    pVTcamMngPtr->vTcamCfgPtrArr =
        (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR*)cpssOsMalloc(
            sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR)
            * (CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS + 1));
    if (pVTcamMngPtr->vTcamCfgPtrArr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(
        pVTcamMngPtr->vTcamCfgPtrArr, 0,
        (sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR)
            * (CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS + 1)));

    rc = prvCpssDxChVirtualTcamDbSegmentTableCreate(
        pVTcamMngPtr, &tcamSegCfg , GT_TRUE/*fillSegFreeTrees*/);
    if (rc != GT_OK)
    {
        cpssOsFree(pVTcamMngPtr->vTcamCfgPtrArr);
        return rc;
    }

    rc = prvCpssDxChVirtualTcamDbRuleIdTableCreate(
        pVTcamMngPtr,
        (tcamSegCfg.tcamColumnsAmount * tcamSegCfg.tcamRowsAmount) /*maxEntriesInDb*/);
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        cpssOsFree(pVTcamMngPtr->vTcamCfgPtrArr);
        return rc;
    }

    rc = prvCpssDxChVirtualTcamDbPriorityTableCreate(
        pVTcamMngPtr,
        (tcamSegCfg.tcamColumnsAmount * tcamSegCfg.tcamRowsAmount) /*maxEntriesInDb*/);
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbRuleIdTableDelete(pVTcamMngPtr);
        prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
        cpssOsFree(pVTcamMngPtr->vTcamCfgPtrArr);
        return rc;
    }

    return GT_OK;
}

/* forward declaration */
static GT_STATUS priorityDbRemoveRuleId(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                 *vTcamMngDBPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC        *vtcamInfoPtr,
    IN  GT_U32                                              logicalIndex
);
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN   GT_BOOL                                    checkExistance,
    OUT  GT_U32                                     *logicalIndexPtr,
    OUT  GT_U32                                     *physicalIndexPtr
);

/*******************************************************************************
* prvCpssDxChVirtualTcamRuleDeleteFromDB
*
* DESCRIPTION:
*       Delete Rule From internal DB.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamRuleDeleteFromDB
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* convert the ruleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &logicalIndex, &deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    if (vtcamInfoPtr->usedRulesAmount == 0)
    {
        /* 'management' error */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* update the DB about the removed rule */
    vtcamInfoPtr->usedRulesBitmapArr[logicalIndex>>5] &= ~(1<<(logicalIndex & 0x1f));

    /* update the counter */
    vtcamInfoPtr->usedRulesAmount--;

    /* for priority mode - remove rule ID entry from DB */
    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
    {
        vTcamMngDBPtr = vTcamMngDB[vTcamMngId];

        rc = priorityDbRemoveRuleId(vTcamMngDBPtr, vtcamInfoPtr, logicalIndex);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamManagerDelete
*
* DESCRIPTION:
*       Delete Virtual TCAM manager structure contents.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - virtual TCAM Manager ID
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                   - on success
*       GT_FAIL                 - on error
*       GT_NOT_FOUND            - on TCAM manager not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDxChVirtualTcamManagerDelete
(
    IN GT_U32   vTcamMngId
)
{
    GT_STATUS                                            rc; /* return code */
    GT_STATUS                                            totalRc; /* return code */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                   *pVTcamMngPtr;
    GT_U32                                               vTcamId;
    GT_U32                                               ruleId;

    totalRc = GT_OK;

    pVTcamMngPtr = vTcamMngDB[vTcamMngId];

    if (pVTcamMngPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    if (pVTcamMngPtr->vTcamCfgPtrArr)
    {
        for (vTcamId = 0; (vTcamId < pVTcamMngPtr->vTcamCfgPtrArrSize); vTcamId++)
        {
            if (pVTcamMngPtr->vTcamCfgPtrArr[vTcamId])
            {
                /* remove all rules */
                while (GT_OK == cpssDxChVirtualTcamNextRuleIdGet(
                    vTcamMngId, vTcamId, CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS, &ruleId))
                {
                    rc = prvCpssDxChVirtualTcamRuleDeleteFromDB(
                        vTcamMngId, vTcamId, ruleId);
                    if (rc != GT_OK)
                    {
                        totalRc = rc;
                    }
                }
                /* remove vTcam */
                rc = cpssDxChVirtualTcamRemove(vTcamMngId, vTcamId);
                if (rc != GT_OK)
                {
                    totalRc = rc;
                }
            }
        }
    }

    /* delete DB priority table */
    rc = prvCpssDxChVirtualTcamDbPriorityTableDelete(pVTcamMngPtr);
    if (rc != GT_OK)
    {
        totalRc = rc;
    }
    /* delete DB rule ID table */
    rc = prvCpssDxChVirtualTcamDbRuleIdTableDelete(pVTcamMngPtr);
    if (rc != GT_OK)
    {
        totalRc = rc;
    }
    /* delete DB segment table  */
    rc = prvCpssDxChVirtualTcamDbSegmentTableDelete(pVTcamMngPtr);
    if (rc != GT_OK)
    {
        totalRc = rc;
    }
    if (pVTcamMngPtr->vTcamCfgPtrArr)
    {
        /* free vTcam configuration structure */
        cpssOsFree(pVTcamMngPtr->vTcamCfgPtrArr);
        pVTcamMngPtr->vTcamCfgPtrArr = NULL;
    }

    return totalRc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamManagerDelete
*
* DESCRIPTION:
*       Delete Virtual TCAM manager structure.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - virtual TCAM Manager ID
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                   - on success
*       GT_FAIL                 - on error
*       GT_BAD_STATE            - on devices still attached to TCAM manager
*       GT_NOT_FOUND            - on TCAM manager not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamManagerDelete
(
    IN GT_U32   vTcamMngId
)
{
    GT_STATUS                                           rc; /* return code */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr;
    GT_U8                                               devNum;

    pVTcamMngPtr = vTcamMngDB[vTcamMngId];
    if (pVTcamMngPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(
        vTcamMngId, &devNum);
    if (rc == GT_OK)
    {
        /* has yet not removed devices */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    cpssOsFree(pVTcamMngPtr);
    vTcamMngDB[vTcamMngId] = NULL;
    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamManagerDelete
*
* DESCRIPTION:
*       Delete Virtual TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                   - on success
*       GT_BAD_STATE            - on devices still attached to TCAM manager
*       GT_NOT_FOUND            - on TCAM manager not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerDelete
(
    IN  GT_U32                              vTcamMngId
)
{

    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamManagerDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId));

    rc = internal_cpssDxChVirtualTcamManagerDelete(vTcamMngId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamManagerCreate
*
* DESCRIPTION:
*       Create Virtual TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_ALREADY_EXIST         - if already exist TCAM Manager with given Id
*       GT_OUT_OF_CPU_MEM        - on CPU memory allocation fail
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamManagerCreate
(
    IN  GT_U32                              vTcamMngId
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *pVTcamMngPtr;

    if (vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vTcamMngDB[vTcamMngId] != NULL)
    {
        /* can't redefine an vTcamMng DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    vTcamMngDB[vTcamMngId] = (PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC*)cpssOsMalloc(sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC));
    if (vTcamMngDB[vTcamMngId] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    pVTcamMngPtr = vTcamMngDB[vTcamMngId];
    cpssOsMemSet(pVTcamMngPtr, 0, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC));
    pVTcamMngPtr->vTcamMngId       = vTcamMngId;
    pVTcamMngPtr->disableRearrange = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamManagerCreate
*
* DESCRIPTION:
*       Create Virtual TCAM manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_ALREADY_EXIST         - if already exist TCAM Manager with given Id
*       GT_OUT_OF_CPU_MEM        - on CPU memory allocation fail
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerCreate
(
    IN  GT_U32                              vTcamMngId
)
{

    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamManagerCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId));

    rc = internal_cpssDxChVirtualTcamManagerCreate(vTcamMngId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamManagerDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing Virtual TCAM manager.
*   TCAM of one of old devices (if exist) copied to TCAMs the newly added devices.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to add to the Virtual TCAM Manager.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_ALREADY_EXIST         - if device already exist in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamManagerDevListAdd
(
    IN GT_U32   vTcamMngId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_U8               i;
    GT_U8               devNum;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_PTR tcamMngDbPtr;
    GT_U32 tcamSize = 0;                /* device TCAM size */
    GT_STATUS           rc;
    GT_BOOL             noDevicesBeforAdding;
    GT_U8               representativeDevNum;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP  newDevsBmp = {0};
    GT_U32  tcamNumEntriesInSmallestRuleSize;

    if (vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(numOfDevs >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(numOfDevs)
    {
        CPSS_NULL_PTR_CHECK_MAC(devListArr);
    }

    if (vTcamMngDB[vTcamMngId] == NULL)
    {
        /* can't find the vTcamMng DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    tcamMngDbPtr = vTcamMngDB[vTcamMngId];

    /* check if the DB already hold existing device */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&representativeDevNum);
    if(rc == GT_OK)
    {
        /* there is at least one device in the TCAM manager prior to calling this function */
        /* so it may hold entries in the HW that need to copy to the new devices */

        /* representativeDevNum is valid device */
        noDevicesBeforAdding = GT_FALSE;
    }
    else
    {
        /* there are no devices in the TCAM manager prior to calling this function */

        /* representativeDevNum is NOT valid device */
        noDevicesBeforAdding = GT_TRUE;
    }

    for (i = 0; i < numOfDevs; i++)
    {
        devNum = devListArr[i];
        /* check that device exist */
        PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
        /* Support only xCat3 or SIP5 devices */
        if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
            (GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        }

        /* Check device family */
        if (noDevicesBeforAdding == GT_TRUE)
        {
            tcamMngDbPtr->devFamily = PRV_CPSS_PP_MAC(devNum)->devFamily;
        }
        else
        {
            if(PRV_CPSS_PP_MAC(devNum)->devFamily != tcamMngDbPtr->devFamily)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
            }
        }

        /* Check TCAM size */
        if (tcamSize == 0)
        {
            /* Get TCAM size for the first device */
            if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                tcamSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelTerm;
            }
            else
            {
               /*
                * xCat3 settings:
                *   fineTuning.policyTcamRaws contains number of PCL Tcam rows,
                *   and each row contains either 4 standard or 2 extended rules.
                *   So that the number of entries in smallest rule size (i.e.
                *   standard rule size) is 4 times the number of PCL Tcam rows.
                */
                tcamSize = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws * 4;
            }
        }
        else
        {
            /* Check TCAM size of other devices */
            if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                if (tcamSize != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.tunnelTerm)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
            else
            {
                if (tcamSize != PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.policyTcamRaws * 4)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
            }
        }

        /* check that the device wasn't added already */
        if (PRV_CPSS_VIRTUAL_TCAM_MNG_IS_DEV_BMP_SET_MAC(tcamMngDbPtr->devsBitmap, devNum))
        {
             /* the device exists */
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }

        if(tcamMngDbPtr->numOfDevices >= PRV_CPSS_MAX_PP_DEVICES_CNS)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: can't increment numOfDevices as already with maximal value[%d]",
                tcamMngDbPtr->numOfDevices);
        }

        if (noDevicesBeforAdding == GT_TRUE)
        {
            /* we hold no devices before the call to this function , so no need
               to copy TCAM from other device */
            if(i == 0)
            {
                /* Initialize TCAM manager structure for the first added device */
                rc = internal_cpssDxChVirtualTcamManagerInit(devNum, vTcamMngId);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        tcamNumEntriesInSmallestRuleSize = tcamSize;
        if(tcamNumEntriesInSmallestRuleSize <
            tcamMngDbPtr->tcamNumEntriesInSmallestRuleSize)
        {
            /* the current added device hold TCAM with less entries than the
                the size of the created one ... we can not support it */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"the added device [%d] hold TCAM with less entries [%d] than the entries[%d] of the created one",
                devNum ,
                tcamNumEntriesInSmallestRuleSize ,
                tcamMngDbPtr->tcamNumEntriesInSmallestRuleSize);
        }

        /* update devices counter */
        tcamMngDbPtr->numOfDevices++;

        /* Add device to bitmap */
        PRV_CPSS_VIRTUAL_TCAM_MNG_DEV_BMP_SET_MAC(tcamMngDbPtr->devsBitmap, devNum);

        /* Add device to bitmap of 'newDevsBmp' .
            NOTE: will be used only if (noDevicesBeforAdding == GT_FALSE)

            because only on such case there may be existing rules in the
            TCAM manager.
        */
        PRV_CPSS_VIRTUAL_TCAM_MNG_DEV_BMP_SET_MAC(newDevsBmp, devNum);
    }

    if (noDevicesBeforAdding == GT_FALSE)
    {
        /* we hold devices before the call to this function , so no need
           to copy rules of vTcams from existing device */
        /* need to copy from representativeDevNum to all new device */
        rc = copyAllRulesFromSrcDevToDevBmp(vTcamMngId , representativeDevNum , newDevsBmp);
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamManagerDevListAdd
*
* DESCRIPTION:
*   This function adds devices to an existing Virtual TCAM manager.
*   TCAM of one of old devices (if exist) copied to TCAMs the newly added devices.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to add to the Virtual TCAM Manager.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_ALREADY_EXIST         - if device already exist in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerDevListAdd
(
    IN GT_U32   vTcamMngId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamManagerDevListAdd);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, devListArr, numOfDevs));

    rc = internal_cpssDxChVirtualTcamManagerDevListAdd(vTcamMngId, devListArr, numOfDevs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, devListArr, numOfDevs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamManagerDevListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing Virtual TCAM manager.
*   TCAMs of removed devices not updated.
*   Removing last device caused cleaning up all Tcam Manager configurations.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to remove from the
*                        Virtual TCAM Manager.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_NOT_INITIALIZED       - if there are no devices in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamManagerDevListRemove
(
    IN GT_U32   vTcamMngId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_STATUS           rc;
    GT_U8               i;
    GT_U8               devNum;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_PTR tcamMngDbPtr;
    GT_U32              removedDevices = 0; /* Number of removed devices */

    if (vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(numOfDevs >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(numOfDevs)
    {
        CPSS_NULL_PTR_CHECK_MAC(devListArr);
    }

    if (vTcamMngDB[vTcamMngId] == NULL)
    {
        /* can't find the vTcamMng DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    tcamMngDbPtr = vTcamMngDB[vTcamMngId];

    for (i = 0; i < numOfDevs; i++)
    {
        devNum = devListArr[i];
        /* check that device exist */
        PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
        /* Support only xCat3 or SIP5 devices */
        if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
            (GT_FALSE == PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
        }

        /* check that the device was added to bitmap */
        if (PRV_CPSS_VIRTUAL_TCAM_MNG_IS_DEV_BMP_SET_MAC(tcamMngDbPtr->devsBitmap, devNum))
        {
            /* Remove device from bitmap */
            PRV_CPSS_VIRTUAL_TCAM_MNG_DEV_BMP_CLEAR_MAC(tcamMngDbPtr->devsBitmap, devNum);
            removedDevices++;

            if(tcamMngDbPtr->numOfDevices == 0)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: can't decrement numOfDevices with value 0");
            }

            /* update devices counter */
            tcamMngDbPtr->numOfDevices--;
        }
    }

    /* There are no devices in DB to remove */
    if (removedDevices == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    if (tcamMngDbPtr->numOfDevices == 0)
    {
        /* Delete TCAM manager */
        rc = prvCpssDxChVirtualTcamManagerDelete(vTcamMngId);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamManagerDevListRemove
*
* DESCRIPTION:
*   This function removes devices from an existing Virtual TCAM manager.
*   TCAMs of removed devices not updated.
*   Removing last device caused cleaning up all Tcam Manager configurations.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       devListArr     - the array of device ids to remove from the
*                        Virtual TCAM Manager.
*       numOfDevs      - the number of device ids in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - if the Virtual TCAM Manager id is not found.
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if devListArr is NULL pointer.
*       GT_NOT_INITIALIZED       - if there are no devices in DB
*
* COMMENTS:
*       NONE
*
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamManagerDevListRemove
(
    IN GT_U32   vTcamMngId,
    IN GT_U8    devListArr[],
    IN GT_U32   numOfDevs
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamManagerDevListRemove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, devListArr, numOfDevs));

    rc = internal_cpssDxChVirtualTcamManagerDevListRemove(vTcamMngId, devListArr, numOfDevs);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, devListArr, numOfDevs));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

void  copyBitsInMemory(
    IN GT_U32                  *memPtr,
    IN GT_U32                  targetStartBit,
    IN GT_U32                  sourceStartBit,
    IN GT_U32                  numBits
);


/*******************************************************************************/
/*******************************************************************************/
/****************** start DB operation *****************************************/
/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamMngGet
*
* DESCRIPTION:
*       Get DB info about a vTcam manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the DB of the vTcam.
*       NULL on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC* prvCpssDxChVirtualTcamDbVTcamMngGet
(
    IN  GT_U32                                      vTcamMngId
)
{
    if(vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        /* vTcamMngId out of range */
        return NULL;
    }

    return vTcamMngDB[vTcamMngId];
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbVTcamGet
*
* DESCRIPTION:
*       Get DB info about a vTcam in vTcam manager.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       pointer to the DB of the vTcam.
*       NULL on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* prvCpssDxChVirtualTcamDbVTcamGet
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;

    if(vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        /* vTcamMngId out of range */
        return NULL;
    }

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if(vTcamMngDBPtr == NULL)
    {
        return NULL;
    }

    if(vTcamId >= vTcamMngDBPtr->vTcamCfgPtrArrSize)
    {
        /* vTcamId out of range */
        return NULL;
    }

    return vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId];/* may be NULL ! */
}

/*******************************************************************************
* deviceIteratorGetNext
*
* DESCRIPTION:
*       The function returns the first/next devNum that is bound to the tcam manager.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       isFirst      - is First or next
*       devNumPtr       - (pointer to) hold 'current' devNum . (relevant for 'next')
*
* OUTPUTS:
*       devNumPtr       - (pointer to) hold 'next' devNum .
*
* RETURNS:
*       GT_OK       - iterator initialization OK
*       GT_NO_MORE  - no devices attached to the vTcamMngId
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_STATUS deviceIteratorGetNext
(
    IN  GT_U32  vTcamMngId,
    IN  GT_BOOL isFirst,
    INOUT GT_U8   *devNumPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    GT_U32  ii;/* must not be GT_U8 */

    if(vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        /* vTcamMngId out of range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if(vTcamMngDBPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(isFirst == GT_TRUE)
    {
        ii = 0;
        *devNumPtr = 0;
    }
    else
    {
        ii = *devNumPtr;

        ii++;
    }

    for(/* continue */; ii < PRV_CPSS_MAX_PP_DEVICES_CNS; ii++)
    {
        if(PRV_CPSS_VIRTUAL_TCAM_MNG_IS_DEV_BMP_SET_MAC(vTcamMngDBPtr->devsBitmap, ii))
        {
            *devNumPtr = (GT_U8)ii;
            return GT_OK;
        }
    }

    return /* NOT to register as error to LOG !!! */ GT_NO_MORE;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst
*
* DESCRIPTION:
*       The function returns the first devNum that is bound to the tcam manager.
*       it must be called before any iteration on the device list of the vTcam.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       devNumPtr       - (pointer to) hold 'first' devNum .
*
* RETURNS:
*       GT_OK       - iterator initialization OK
*       GT_NO_MORE  - no devices attached to the vTcamMngId
*       GT_FAIL     - on error
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst
(
    IN  GT_U32  vTcamMngId,
    OUT GT_U8   *devNumPtr
)
{
    return deviceIteratorGetNext(vTcamMngId,GT_TRUE,devNumPtr);
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbDeviceIteratorGetNext
*
* DESCRIPTION:
*       The function returns the next devNum that is bound to the tcam manager.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       devNumPtr       - (pointer to) hold 'current' devNum .
*
* OUTPUTS:
*       devNumPtr       - (pointer to) hold 'next' devNum .
*
* RETURNS:
*       0       - no more devices. ('current' device is the last one)
*       1       - devNumPtr hold 'next' devNum
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_U32  prvCpssDxChVirtualTcamDbDeviceIteratorGetNext
(
    IN  GT_U32  vTcamMngId,
    INOUT GT_U8   *devNumPtr
)
{
    GT_STATUS   rc;

    rc = deviceIteratorGetNext(vTcamMngId,GT_FALSE,devNumPtr);
    if(rc == GT_OK)
    {
        return 1; /*devPtr hold 'next' devNum */
    }

    return 0;/*no more devices. ('current' device is the last one) */
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryGet
*
* DESCRIPTION:
*       Get rule Id entry by rule Id.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       ruleId                  - ruleId to find.
*
* OUTPUTS:
*       entryPtr                - (pointer to) find rule entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - the ruleId was not found.
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryGet
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                     ruleId,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC   *entryPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC    ruleIdEntry; /* key for search */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC   *dbEntryPtr;  /* pointer to entry in DB */

    ruleIdEntry.ruleId = ruleId;
    /* find rule ID entry in AVL tree */
    dbEntryPtr = prvCpssAvlSearch(tcamCfgPtr->ruleIdIdTree, &ruleIdEntry);
    if (dbEntryPtr == NULL)
    {
        return /* do not register as error , it is valid state !!! */ GT_NOT_FOUND;
    }
    *entryPtr =  *dbEntryPtr;
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryRemove
*
* DESCRIPTION:
*       Remove rule Id entry.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       ruleId                  - ruleId to remove.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on ruleId or/and logicalIndex already used
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryRemove
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN   GT_U32                                             ruleId
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC ruleIdEntry, * ruleIdEntryPtr;


    ruleIdEntry.ruleId = ruleId;
    /* find rule ID entry in AVL tree */
    ruleIdEntryPtr = prvCpssAvlSearch(tcamCfgPtr->ruleIdIdTree, &ruleIdEntry);
    if (ruleIdEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    /* remove rule ID entry from the tree */
    prvCpssAvlItemRemove(tcamCfgPtr->ruleIdIdTree, ruleIdEntryPtr);
    /* remove rule ID entry from the index tree */
    prvCpssAvlItemRemove(tcamCfgPtr->ruleIdIndexTree, ruleIdEntryPtr);

    /* free a buffer back to its pool.*/
    rc = cpssBmPoolBufFree(pVTcamMngPtr->ruleIdPool, ruleIdEntryPtr);

    return rc;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryAdd
*
* DESCRIPTION:
*       Add rule Id entry.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       entryPtr        - (pointer to) rule Id entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on ruleId or/and logicalIndex already used
*       GT_FAIL                  - on error
*
* COMMENTS:
*       treePathArr               - (pointer to) tree path array.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryAdd
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC               *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC      *tcamCfgPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC  *entryPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC * ruleIdEntryPtr;


    /* find rule ID entry in AVL tree */
    ruleIdEntryPtr = prvCpssAvlSearch(tcamCfgPtr->ruleIdIdTree, entryPtr);
    if (ruleIdEntryPtr)
    {
        /* rule ID entry already exist */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* get free buffer from pool.*/
    ruleIdEntryPtr = cpssBmPoolBufGet(pVTcamMngPtr->ruleIdPool);
    if (ruleIdEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    /* copy rule ID entry to allocated buffer */
    cpssOsMemCpy(ruleIdEntryPtr, entryPtr, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC));

    /* add rule ID entry to the tree */
    rc = prvCpssAvlItemInsert(tcamCfgPtr->ruleIdIdTree, ruleIdEntryPtr);
    if (rc != GT_OK)
    {
        cpssBmPoolBufFree(pVTcamMngPtr->ruleIdPool, ruleIdEntryPtr);
        return rc;
    }
    /* add rule ID entry to the index tree */
    rc = prvCpssAvlItemInsert(tcamCfgPtr->ruleIdIndexTree, ruleIdEntryPtr);
    if (rc != GT_OK)
    {
        prvCpssAvlItemRemove(tcamCfgPtr->ruleIdIdTree, ruleIdEntryPtr);
        cpssBmPoolBufFree(pVTcamMngPtr->ruleIdPool, ruleIdEntryPtr);
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* numHwMovesForHoleCalc
*
* DESCRIPTION:
*       calc number of HW moves needed to create hole at the start/end of the range
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr    - (pointer to)virtual TCAM structure
*       fromEndOrStart  - the type of direction
*                       GT_TRUE  - hole from end   of range
*                       GT_FALSE - hole from start of range
*       holeSize        - the size of the needed hole
* OUTPUTS:
*       numMovesPtr     - (pointer to) number of moves needed
*                       NOTE: when (*numMovesPtr) == 0 meaning that the start/end
*                       of the range is already empty.
*                       so no need to move any index in the HW.
*       targetLogicalIndexPtr - (pointer to) the logical index of the first
*           non-used , that should be used to move used indexes into it.
*                       NOTE: relevant only when (*numMovesPtr) != 0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - bad hole size
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS numHwMovesForHoleCalc
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC               *vtcamInfoPtr,
    IN   GT_BOOL                                                    fromEndOrStart,
    IN   GT_U32                                                     holeSize,
    OUT  GT_U32                                                    *numMovesPtr,
    OUT  GT_U32                                                    *targetLogicalIndexPtr
)
{
    GT_U32  ii,iiStart,iiLast;
    GT_U32  *arrPtr = vtcamInfoPtr->usedRulesBitmapArr;
    GT_U32  value;
    GT_U32  holesCounter = 0;/* number of unused indexes in the range that expected to get the movements */
    GT_U32  numUsedInFinalHole = 0;/* number of used indexes with size <holeSize> from start/end of the range*/
    GT_U32  tmpLogicalIndex;

    *numMovesPtr = 0;
    *targetLogicalIndexPtr = 0;

    if(fromEndOrStart == GT_TRUE)
    {
        /* start from the end : direction of search is 'down' */
        iiStart = vtcamInfoPtr->rulesAmount - 1;
        iiLast  = iiStart - holeSize;

        /* calc number of used indexes with size <holeSize> from end of the range */
        for(ii = iiStart ; ii > iiLast ; ii--)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value)
            {
                numUsedInFinalHole++;
            }

            if(ii == 0)
            {
                break;
            }
        }

        if(numUsedInFinalHole == 0)
        {
            /* the end of the table is empty !!! no need to do any moves */
            *numMovesPtr = 0;
            *targetLogicalIndexPtr = 0;/* should not be used by the caller on this case */
            return GT_OK;
        }

        tmpLogicalIndex = ii;
        /* logicalIndexOfFirstHoleForTheMove:
           calc from current ii , the range that hold unused indexes in the
           amount of <numUsedInFinalHole>  */
        for(/*continue*/ ; /*no criteria*/ ; ii--)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value == 0)
            {
                holesCounter++;
            }

            if(holesCounter == numUsedInFinalHole)
            {
                /* this is the number of HW movements that needed , to create
                   hole at the end of the range */
                *numMovesPtr = tmpLogicalIndex - ii + 1;
                *targetLogicalIndexPtr = ii;
                return GT_OK;
            }

            if(ii == 0)
            {
                break;
            }
        }
    }
    else
    {
        /* start from the start : direction of search is 'up' */
        iiStart = 0;
        iiLast  = holeSize;

        /* calc number of used indexes with size <holeSize> from start of the range */
        for(ii = iiStart ; ii < iiLast ; ii++)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value)
            {
                numUsedInFinalHole++;
            }
        }

        if(numUsedInFinalHole == 0)
        {
            /* the start of the table is empty !!! no need to do any moves */
            *numMovesPtr = 0;
            *targetLogicalIndexPtr = 0;/* should not be used by the caller on this case */
            return GT_OK;
        }

        tmpLogicalIndex = ii;
        /* logicalIndexOfFirstHoleForTheMove:
           calc from current ii , the range that hold unused indexes in the
           amount of <numUsedInFinalHole>  */
        for(/*continue*/ ; ii < vtcamInfoPtr->rulesAmount ; ii++)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value == 0)
            {
                holesCounter++;
            }

            if(holesCounter == numUsedInFinalHole)
            {
                /* this is the number of HW movements that needed , to create
                   hole at the end of the range */
                *numMovesPtr = ii - tmpLogicalIndex + 1;
                *targetLogicalIndexPtr = ii;
                return GT_OK;
            }
        }
    }
    /* not found such hole size */
    /* but the caller made sure that we have enough holes !? */
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* findNearestLogicalIndexUsed
*
* DESCRIPTION:
*       find nearest to specific logical index that is used logical index
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       inLogicalIndex          - input logical index
*       lessOrMore              - the type of search
*                               GT_TRUE  - find index valid that is less than current
*                               GT_FALSE - find index valid that is more than current
*       lastLogicalIndexToCheck - last logical index to check
* OUTPUTS:
*       outLogicalIndexPtr      - (pointer to) output logical index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_FOUND             - no such used index
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS findNearestLogicalIndexUsed
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC               *vtcamInfoPtr,
    IN   GT_U32                                                     inLogicalIndex,
    IN   GT_BOOL                                                    lessOrMore,
    IN   GT_U32                                                     lastLogicalIndexToCheck,
    OUT  GT_U32                                                    *outLogicalIndexPtr
)
{
    GT_U32  ii;
    GT_U32  *arrPtr = vtcamInfoPtr->usedRulesBitmapArr;
    GT_U32  value;

    if(lessOrMore == GT_TRUE)
    {
        /* direction of search is 'down' */
        if(inLogicalIndex == 0)
        {
            return /* do not register as error , it is valid state !!! */ GT_NOT_FOUND;
        }
        /* look for nearest used from specific index */
        for(ii = inLogicalIndex - 1 ; /*no criteria*/ ; ii--)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value)
            {
                *outLogicalIndexPtr = ii;
                /* found used index */
                return GT_OK;
            }

            if(ii == 0)
            {
                break;
            }

            if(ii == lastLogicalIndexToCheck)
            {
                break;
            }
        }
    }
    else
    {
        /* direction of search is 'up' */
        /* look for nearest used from specific index */
        for(ii = inLogicalIndex+1 ; ii < vtcamInfoPtr->rulesAmount ; ii++)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr,ii,1,value);
            if(value)
            {
                *outLogicalIndexPtr = ii;
                /* found used index */
                return GT_OK;
            }

            if(ii == lastLogicalIndexToCheck)
            {
                break;
            }
        }
    }

    return /* do not register as error , it is valid state !!! */ GT_NOT_FOUND;
}

/*******************************************************************************
* findNearestLogicalIndexHole
*
* DESCRIPTION:
*       find nearest to specific logical index an unused logical index
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       inLogicalIndex          - input logical index
*
* OUTPUTS:
*       outLogicalIndexPtr      - (pointer to) output logical index
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FULL                  - on table full (no free logical index)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS findNearestLogicalIndexHole
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC               *vtcamInfoPtr,
    IN   GT_U32                                                     inLogicalIndex,
    OUT  GT_U32                                                    *outLogicalIndexPtr
)
{
    GT_U32  ii;
    GT_BOOL upWithMoreEntriesOrDown;
    GT_U32  maxSharedOffset;
    GT_U32  maxIterator;
    GT_U32  *arrPtr = vtcamInfoPtr->usedRulesBitmapArr;
    GT_U32  value;

    if (vtcamInfoPtr->usedRulesAmount >= vtcamInfoPtr->rulesAmount)
    {
        if (vtcamInfoPtr->tcamInfo.autoResize == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
        }
        else
        {
            return /* do not register as error */ GT_FULL;
        }
    }

    if(inLogicalIndex >= (vtcamInfoPtr->rulesAmount / 2))
    {
        /* we are in the upper half of the array */
        /* meaning that we potentially have more indexes to go down than to go up */
        maxSharedOffset = vtcamInfoPtr->rulesAmount - inLogicalIndex;/* the end is closest */
        upWithMoreEntriesOrDown = GT_FALSE;
        maxIterator = inLogicalIndex + 1;

    }
    else
    {
        /* we are in the lower half of the array */
        /* meaning that we potentially have more indexes to go up than to go down */
        maxSharedOffset = inLogicalIndex + 1; /* the start is closest */
        upWithMoreEntriesOrDown = GT_TRUE;
        maxIterator = vtcamInfoPtr->rulesAmount - inLogicalIndex;
    }

    CPSS_LOG_INFO_PARAM_MAC(maxSharedOffset);
    CPSS_LOG_INFO_PARAM_MAC(upWithMoreEntriesOrDown);
    CPSS_LOG_INFO_PARAM_MAC(maxIterator);

    /* look for nearest hole from specific index */
    for(ii = 0 ; ii < maxSharedOffset ; ii++)
    {
        U32_GET_FIELD_IN_ENTRY_MAC(arrPtr,inLogicalIndex + ii,1,value);
        if(value == 0)
        {
            *outLogicalIndexPtr = inLogicalIndex + ii;
            /* found free index */
            return GT_OK;
        }


        U32_GET_FIELD_IN_ENTRY_MAC(arrPtr,inLogicalIndex - ii,1,value);
        if(value == 0)
        {
            *outLogicalIndexPtr = inLogicalIndex - ii;
            /* found free index */
            return GT_OK;
        }
    }

    for(/* continue */; ii < maxIterator ; ii++)
    {
        if(upWithMoreEntriesOrDown == GT_TRUE)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr,inLogicalIndex + ii,1,value);
            if(value == 0)
            {
                *outLogicalIndexPtr = inLogicalIndex + ii;
                /* found free index */
                return GT_OK;
            }
        }
        else
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr,inLogicalIndex - ii,1,value);
            if(value == 0)
            {
                *outLogicalIndexPtr = inLogicalIndex - ii;
                /* found free index */
                return GT_OK;
            }
        }
    }

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
}

/*******************************************************************************
* prioGetNodeByIndex
*
* DESCRIPTION:
*       get priority entry node by logical index
*       return NULL if that logical index not in range of any priority
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       logicalIndex            - input logical index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to the priority entry node that logical index is in it's range
*       NULL if that logical index not in range of any priority
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*
    prioGetNodeByIndex(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN GT_U32 logicalIndex
)
{
    PRV_CPSS_AVL_TREE_PATH      avlTreePath;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    needToFindInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    GT_BOOL found1;

    needToFindInfo.priority = 0;/* don't care */
    needToFindInfo.baseLogIndex = logicalIndex;
    needToFindInfo.rangeSize = 0;/* don't care */
    /* check if this logical index is occupied */
    found1 = prvCpssAvlPathFind(vtcamInfoPtr->priorityIndexTree,
            PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,
            &needToFindInfo,/*pData*/
            avlTreePath,/*path*/
            &dbEntryPtr /* use dedicated var to avoid warnings */);
    foundPriorityEntryPtr = dbEntryPtr;

    if(found1 == GT_FALSE)
    {
        /* no node that hold it */
        return NULL;
    }

    if(logicalIndex >= (foundPriorityEntryPtr->baseLogIndex +
                        foundPriorityEntryPtr->rangeSize))
    {
        /* no node that hold it */
        return NULL;
    }

    return foundPriorityEntryPtr;
}

/*******************************************************************************
* prioGetNodeByPriority
*
* DESCRIPTION:
*       get priority entry node by priority
*       return NULL if that priority not in DB
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       priority                - input priority
*
* OUTPUTS:
*       None
*
* RETURNS:
*       pointer to the priority entry node of the priority
*       NULL if that priority not in DB
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*
    prioGetNodeByPriority(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN GT_U32 priority
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    needToFindInfo;

    needToFindInfo.priority = priority;
    needToFindInfo.baseLogIndex = 0;/* don't care */
    needToFindInfo.rangeSize = 0;/* don't care */
    /* check if this logical index is occupied */
    return prvCpssAvlSearch(vtcamInfoPtr->priorityPriTree,
            &needToFindInfo/*pData*/);
}

/*******************************************************************************
* prioGetNextNodeByPriority
*
* DESCRIPTION:
*       get 'next' priority entry node by priority
*       return NULL if there is no 'next' priority
*       NOTE: the 'current' priority can be 'non valid' (not exists)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       priorityPtr             - (pointer to) current priority
*
* OUTPUTS:
*       priorityPtr             - (pointer to) next priority
*
* RETURNS:
*       pointer to the 'next' priority entry node of the priority
*       NULL if no more 'next'
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*
    prioGetNextNodeByPriority(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    INOUT GT_U32 *priorityPtr
)
{
    PRV_CPSS_AVL_TREE_PATH      avlTreePath;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    needToFindInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    GT_BOOL found1;
    GT_BOOL needFirst;

    if((*priorityPtr) > 0xFFFFFFF0)
    {
        /* meaning need to get 'first' */
        (*priorityPtr) = 0;
        needFirst = GT_TRUE;
    }
    else
    {
        needFirst = GT_FALSE;
    }

    needToFindInfo.priority = *priorityPtr;
    needToFindInfo.baseLogIndex = 0;/* don't care */
    needToFindInfo.rangeSize = 0;/* don't care */

    found1 = prvCpssAvlPathFind(vtcamInfoPtr->priorityPriTree,
            PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
            &needToFindInfo,/*pData*/
            avlTreePath,/*path*/
            &dbEntryPtr /* use dedicated var to avoid warnings */);
    foundPriorityEntryPtr = dbEntryPtr;

    if(found1 == GT_FALSE)
    {
        return NULL;
    }

    if(needFirst == GT_FALSE)
    {
        if(foundPriorityEntryPtr->priority <= needToFindInfo.priority)
        {
            found1 = prvCpssAvlPathSeek(vtcamInfoPtr->priorityPriTree,
                PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                avlTreePath,
                &dbEntryPtr /* use dedicated var to avoid warnings */);
            if(found1 == GT_FALSE)
            {
                return NULL;
            }
            foundPriorityEntryPtr = dbEntryPtr;
        }
    }

    *priorityPtr = foundPriorityEntryPtr->priority;

    return foundPriorityEntryPtr;
}

/*******************************************************************************
* prioGetPrevNodeByPriority
*
* DESCRIPTION:
*       get 'previous' priority entry node by priority
*       return NULL if there is no 'previous' priority
*       NOTE: the 'current' priority can be 'non valid' (not exists)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       priorityPtr             - (pointer to) current priority
*
* OUTPUTS:
*       priorityPtr             - (pointer to) previous priority
*
* RETURNS:
*       pointer to the 'previous' priority entry node of the priority
*       NULL if no more 'previous'
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC*
    prioGetPrevNodeByPriority(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    INOUT GT_U32 *priorityPtr
)
{
    PRV_CPSS_AVL_TREE_PATH      avlTreePath;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    needToFindInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    GT_BOOL found1;

    needToFindInfo.priority = *priorityPtr;
    needToFindInfo.baseLogIndex = 0;/* don't care */
    needToFindInfo.rangeSize = 0;/* don't care */

    found1 = prvCpssAvlPathFind(vtcamInfoPtr->priorityPriTree,
            PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,
            &needToFindInfo,/*pData*/
            avlTreePath,/*path*/
            &dbEntryPtr /* use dedicated var to avoid warnings */);
    foundPriorityEntryPtr = dbEntryPtr;

    if(found1 == GT_FALSE)
    {
        return NULL;
    }

    if(foundPriorityEntryPtr->priority >= needToFindInfo.priority)
    {
        found1 = prvCpssAvlPathSeek(vtcamInfoPtr->priorityPriTree,
            PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E,
            avlTreePath,
            &dbEntryPtr /* use dedicated var to avoid warnings */);
        if(found1 == GT_FALSE)
        {
            return NULL;
        }
        foundPriorityEntryPtr = dbEntryPtr;
    }

    *priorityPtr = foundPriorityEntryPtr->priority;

    return foundPriorityEntryPtr;
}

/*******************************************************************************
* priorityDbLogicalIndexRemoveAndMovePriority_sameIndex
*
* DESCRIPTION:
*       newLogicalIndex == removedLogicalIndex :
*       update the trees of 'priority' that mapped to 'logical indexes'.
*       find the priority that hold the 'new' logical index, update it's
*       entry about the 'number of used' and maybe it's base index.
*       all other priorities between 'new' index to 'removed' index need to
*       modify 'base logical index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       priority                - the priority that will get the new index
*       position                - position in range of rule of the
*                                 specified priority (see enum)
*       removedLogicalIndex     - the removed logical index (equal to the new logical index)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_STATE    - on DB bad state.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityDbLogicalIndexRemoveAndMovePriority_sameIndex
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN GT_U32 priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    IN GT_U32 removedLogicalIndex
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    GT_U32  currPriority;

    position = position;

    /* handle priority that hold the removed index */
    foundPriorityEntryPtr = prioGetNodeByIndex(vtcamInfoPtr,removedLogicalIndex);
    if(foundPriorityEntryPtr)
    {
        currPriority = foundPriorityEntryPtr->priority;

        /* notify the priority DB that the logical index is removed from the range */
        /* if this is the last logical index in the range it will update the range */
        rc = priorityTableEntryRemoveLogicalIndexUpdateRange(vTcamMngDBPtr,vtcamInfoPtr,
            foundPriorityEntryPtr->priority,
            removedLogicalIndex);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        /* get again the node (that may changed or not exists any more) */
        foundPriorityEntryPtr = prioGetNodeByPriority(vtcamInfoPtr,currPriority);
        if(foundPriorityEntryPtr &&
           foundPriorityEntryPtr->baseLogIndex == removedLogicalIndex)
        {
            foundPriorityEntryPtr->baseLogIndex++;
            foundPriorityEntryPtr->rangeSize--;
        }
    }

    /* handle priority that get the new index */
    foundPriorityEntryPtr = prioGetNodeByPriority(vtcamInfoPtr,priority);
    if(foundPriorityEntryPtr)
    {
        foundPriorityEntryPtr->rangeSize++;
        if(foundPriorityEntryPtr->baseLogIndex == (removedLogicalIndex+1))
        {
            foundPriorityEntryPtr->baseLogIndex--;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* priorityDbLogicalIndexRemoveAndMovePriority_newIndexLower
*
* DESCRIPTION:
*       newLogicalIndex < removedLogicalIndex :
*       update the trees of 'priority' that mapped to 'logical indexes'.
*       find the priority that hold the 'new' logical index, update it's
*       entry about the 'number of used' and maybe it's base index.
*       all other priorities between 'new' index to 'removed' index need to
*       modify 'base logical index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       priority                - the priority that will get the new index
*       position                - position in range of rule of the
*                                 specified priority (see enum)
*       newLogicalIndex         - the new logical index
*       removedLogicalIndex     - the removed logical index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_STATE    - on DB bad state.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityDbLogicalIndexRemoveAndMovePriority_newIndexLower
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN GT_U32 priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    IN GT_U32 newLogicalIndex, /* newLogicalIndex < removedLogicalIndex*/
    IN GT_U32 removedLogicalIndex
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *lastFoundPriorityEntryPtr;
    GT_BOOL updateFirst = GT_FALSE;
    GT_U32  firstBaseIndex = 0xFFFFFFFF;
    GT_U32  lastFoundPriority_baseIndex;
    GT_U32  lastFoundPriority_range;
    GT_U32  baseOfRemovedPriority = removedLogicalIndex;
    GT_U32  currPriority;

    currPriority = priority-1;

    lastFoundPriorityEntryPtr = NULL;
    lastFoundPriority_baseIndex = 0;
    lastFoundPriority_range = 0;

    /* handle all the others in the middle */
    do{
        foundPriorityEntryPtr =
            prioGetNextNodeByPriority(vtcamInfoPtr,&currPriority);

        if(foundPriorityEntryPtr == NULL)
        {
            break;
        }
        else
        if(foundPriorityEntryPtr->baseLogIndex > baseOfRemovedPriority)
        {
            break;
        }

        lastFoundPriorityEntryPtr = foundPriorityEntryPtr;
        lastFoundPriority_baseIndex = lastFoundPriorityEntryPtr->baseLogIndex;
        lastFoundPriority_range = lastFoundPriorityEntryPtr->rangeSize;

        if(updateFirst == GT_FALSE && currPriority == priority)
        {
            updateFirst = GT_TRUE;
            if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E &&
               (newLogicalIndex ==  foundPriorityEntryPtr->baseLogIndex))
            {
                /* add the removed index to the range */
                foundPriorityEntryPtr->rangeSize++;
                firstBaseIndex = foundPriorityEntryPtr->baseLogIndex;
            }
            else
            if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E &&
               (newLogicalIndex ==  (foundPriorityEntryPtr->baseLogIndex + foundPriorityEntryPtr->rangeSize)))
            {
                /* add the added index to the range */
                foundPriorityEntryPtr->rangeSize++;
                firstBaseIndex = foundPriorityEntryPtr->baseLogIndex;
            }
        }

        if(currPriority == priority)
        {
            /* case to set the 'lastFoundPriorityEntryPtr' */
        }
        else
        {
            foundPriorityEntryPtr->baseLogIndex++;
        }

    }while(1);

    if(lastFoundPriorityEntryPtr)
    {
        if(firstBaseIndex == lastFoundPriority_baseIndex)
        {
            /* do not modify those that did rangeSize++ */
        }
        else
        if(removedLogicalIndex >=  lastFoundPriority_baseIndex &&
           removedLogicalIndex <  (lastFoundPriority_baseIndex + lastFoundPriority_range))
        {
            lastFoundPriorityEntryPtr->rangeSize--;

            if(lastFoundPriorityEntryPtr->rangeSize == 0)
            {
                /* remove the node from the tree */
                /* update the trees of the priority */
                rc = prvCpssDxChVirtualTcamDbPriorityTableEntrySet(
                    vTcamMngDBPtr,vtcamInfoPtr,lastFoundPriorityEntryPtr);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }


    return GT_OK;
}

/*******************************************************************************
* priorityDbLogicalIndexRemoveAndMovePriority_newIndexHigher
*
* DESCRIPTION:
*       newLogicalIndex > removedLogicalIndex :
*       update the trees of 'priority' that mapped to 'logical indexes'.
*       find the priority that hold the 'new' logical index, update it's
*       entry about the 'number of used' and maybe it's base index.
*       all other priorities between 'new' index to 'removed' index need to
*       modify 'base logical index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       priority                - the priority that will get the new index
*       position                - position in range of rule of the
*                                 specified priority (see enum)
*       newLogicalIndex         - the new logical index
*       removedLogicalIndex     - the removed logical index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_STATE    - on DB bad state.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityDbLogicalIndexRemoveAndMovePriority_newIndexHigher
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN GT_U32 priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    IN GT_U32 newLogicalIndex, /* newLogicalIndex > removedLogicalIndex*/
    IN GT_U32 removedLogicalIndex
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *lastFoundPriorityEntryPtr;
    /*GT_U32  lastOfRemovedPriority = removedLogicalIndex;*/
    GT_U32  currPriority;

    GT_BOOL updateFirst = GT_FALSE;
    GT_U32  lastFoundPriority_baseIndex;
    GT_U32  lastFoundPriority_range;
    GT_U32  baseOfRemovedPriority = removedLogicalIndex;

    currPriority = priority + 1;

    lastFoundPriorityEntryPtr = NULL;
    lastFoundPriority_baseIndex = 0xFFFFFFFF;
    lastFoundPriority_range = 0;

    /* handle all the others in the middle */
    do{
        foundPriorityEntryPtr =
            prioGetPrevNodeByPriority(vtcamInfoPtr,&currPriority);

        if(foundPriorityEntryPtr == NULL)
        {
            break;
        }
        else
        if((foundPriorityEntryPtr->baseLogIndex + foundPriorityEntryPtr->rangeSize) <=
            baseOfRemovedPriority)
        {
            break;
        }

        lastFoundPriorityEntryPtr = foundPriorityEntryPtr;
        lastFoundPriority_baseIndex = lastFoundPriorityEntryPtr->baseLogIndex;/* save before the base-- */
        lastFoundPriority_range = lastFoundPriorityEntryPtr->rangeSize;

        if(foundPriorityEntryPtr->baseLogIndex <= baseOfRemovedPriority)
        {
            /* not update the base */
        }
        else
        {
            foundPriorityEntryPtr->baseLogIndex--;
        }

        if(updateFirst == GT_FALSE && currPriority == priority)
        {
            updateFirst = GT_TRUE;
            if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E &&
               (newLogicalIndex ==  foundPriorityEntryPtr->baseLogIndex))
            {
                /* add the removed index to the range */
                foundPriorityEntryPtr->rangeSize++;
            }
            else
            if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E &&
               (newLogicalIndex ==  (foundPriorityEntryPtr->baseLogIndex + foundPriorityEntryPtr->rangeSize)))
            {
                /* add the added index to the range */
                foundPriorityEntryPtr->rangeSize++;
            }
        }

    }while(1);

    if(lastFoundPriorityEntryPtr)
    {
        if(lastFoundPriorityEntryPtr->priority == priority)
        {
            /* this range handled first and not need more treatment  */
        }
        else
        if(removedLogicalIndex >=  lastFoundPriority_baseIndex &&
           removedLogicalIndex <  (lastFoundPriority_baseIndex + lastFoundPriority_range))
        {
            lastFoundPriorityEntryPtr->rangeSize--;

            if(lastFoundPriorityEntryPtr->rangeSize == 0)
            {
                /* remove the node from the tree */
                /* update the trees of the priority */
                rc = prvCpssDxChVirtualTcamDbPriorityTableEntrySet(
                    vTcamMngDBPtr,vtcamInfoPtr,lastFoundPriorityEntryPtr);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* priorityDbLogicalIndexRemoveAndMovePriority
*
* DESCRIPTION:
*       update the trees of 'priority' that mapped to 'logical indexes'.
*       find the priority that hold the 'new' logical index, update it's
*       entry about the 'number of used' and maybe it's base index.
*       all other priorities between 'new' index to 'removed' index need to
*       modify 'base logical index'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       priority                - the priority that will get the new index
*       position                - position in range of rule of the
*                                 specified priority (see enum)
*       newLogicalIndex         - the new logical index
*       removedLogicalIndex     - the removed logical index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_STATE    - on DB bad state.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityDbLogicalIndexRemoveAndMovePriority
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN GT_U32 priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    IN GT_U32 newLogicalIndex,
    IN GT_U32 removedLogicalIndex
)
{
    if(newLogicalIndex == removedLogicalIndex)
    {
        return priorityDbLogicalIndexRemoveAndMovePriority_sameIndex(
            vTcamMngDBPtr,vtcamInfoPtr,priority,position,removedLogicalIndex);
    }

    if(newLogicalIndex < removedLogicalIndex)
    {
        return priorityDbLogicalIndexRemoveAndMovePriority_newIndexLower(
            vTcamMngDBPtr,vtcamInfoPtr,priority,position,
            newLogicalIndex,removedLogicalIndex);
    }

    return priorityDbLogicalIndexRemoveAndMovePriority_newIndexHigher(
        vTcamMngDBPtr,vtcamInfoPtr,priority,position,
        newLogicalIndex,removedLogicalIndex);
}

/*******************************************************************************
* rulesDbLogicalIndexRemove
*
* DESCRIPTION:
*       update the trees of 'rules' that mapped to 'logical indexes'
*
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr            - (pointer to)virtual TCAM structure
*       newLogicalIndex         - the new logical index
*       removedLogicalIndex     - the removed logical index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_STATE    - on DB bad state.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS rulesDbLogicalIndexRemove
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN GT_U32 newLogicalIndex,
    IN GT_U32 removedLogicalIndex
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC needToFindInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC *ruleIdEntryPtr;
    GT_U32  startLogicalIndex;
    GT_U32  endLogicalIndex;
    GT_U32  operatorPlusPlus;
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    PRV_CPSS_AVL_TREE_SEEK_ENT  seekMode;
    PRV_CPSS_AVL_TREE_FIND_ENT  findMode;
    GT_BOOL found1,found2;

    if(newLogicalIndex == removedLogicalIndex)
    {
        return GT_OK;
    }

    if(removedLogicalIndex > newLogicalIndex)
    {
        /* move indexes up to close the hole */
        operatorPlusPlus = 1;
    }
    else
    {
        /* move indexes down to close the hole */
        operatorPlusPlus = 0;
    }

    startLogicalIndex = removedLogicalIndex;
    endLogicalIndex = newLogicalIndex;

    needToFindInfo.ruleId = 0;/*dont care */
    needToFindInfo.logicalIndex = startLogicalIndex;
    needToFindInfo.applicationDataPtr = 0;/*dont care */

    findMode = operatorPlusPlus == 0 ?
        PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E :
        PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E;

    /* check if this logical index is occupied */
    found1 = prvCpssAvlPathFind(vtcamInfoPtr->ruleIdIndexTree,
            findMode,
            &needToFindInfo,/*pData*/
            avlTreePath,/*path*/
            &dbEntryPtr /* use dedicated var to avoid warnings */);
    ruleIdEntryPtr = dbEntryPtr;

    if(found1 == GT_FALSE)
    {
        findMode = operatorPlusPlus == 0 ?
            PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E :
            PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E;

        found2 = prvCpssAvlPathFind(vtcamInfoPtr->ruleIdIndexTree,
                findMode,
                &needToFindInfo,/*pData*/
                avlTreePath,/*path*/
                &dbEntryPtr /* use dedicated var to avoid warnings */);
         ruleIdEntryPtr = dbEntryPtr;
    }
    else
    {
        found2 = GT_FALSE;
    }

    if(found1 == GT_FALSE && found2 == GT_FALSE)
    {
        /* empty DB ... no need to update anything */
        return GT_OK;
    }

    seekMode = operatorPlusPlus ?
                PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E:
                PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
    /* modify content of the elements in the tree but without changing the
        'tree path' because modification in ascending/descending order */
    do
    {
        if(operatorPlusPlus)
        {
            if((ruleIdEntryPtr->logicalIndex + 1) >= vtcamInfoPtr->rulesAmount)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: can't iterate to next logical index , since out of range");
            }

            ruleIdEntryPtr->logicalIndex++;
        }
        else
        {
            if(ruleIdEntryPtr->logicalIndex == 0)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: can't iterate to previous logical index , since out of range");
            }

            ruleIdEntryPtr->logicalIndex--;
        }

        if(operatorPlusPlus && ((ruleIdEntryPtr->logicalIndex-1) <= endLogicalIndex))
        {
            /* no more updates */
            break;
        }
        else
        if((operatorPlusPlus == 0) && ((ruleIdEntryPtr->logicalIndex+1) >= endLogicalIndex))
        {
            /* no more updates */
            break;
        }

        found1 = prvCpssAvlPathSeek(vtcamInfoPtr->ruleIdIndexTree,
            seekMode,
            avlTreePath,
            &dbEntryPtr /* use dedicated var to avoid warnings */);
         ruleIdEntryPtr = dbEntryPtr;

        if(GT_FALSE == found1)
        {
            /* no more ?! */
            break;
        }

    }
    while(1);

    return GT_OK;
}

/* original index is not valid */
#define ORIG_INDEX_NOT_USED_CNS   (0xFFFFFFFF - 1)

/*******************************************************************************
* prvCpssDxChVirtualTcamDbUtilNewRuleLogIndexGet
*
* DESCRIPTION:
*       Get place for new rule.
*       If needed move existing rules.
*       If needed resize vTCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId              - vTCAM Manager Id
*                                 (APPLICABLE RANGES: 0..31)
*       vTcamId                 - virtual TCAM Id
*                                 (APPLICABLE RANGES: 0..0xFFFF)
*       origIndexForPriorityUpdate - the original index of the entry that doing
*                               'priority update'
*                               value ORIG_INDEX_NOT_USED_CNS meaning that it is
*                               not relevant.
*       vTcamMaxSize            - maximal size of vTCAM. (no resize to greater).
*                                 The current size retrieved from Segment Table.
*       hwContextPtr            - (pointer to)HW context for moving existing rules.
*       priority                - new rule priority
*       position                - position in range of rule of the
*                                 specified priority (see enum)
*       sizeIncrement           - amount of rules to be added if resize will needed.
*       recalled                - recalled due to autoresize
*
* OUTPUTS:
*       logIndexPtr         - (pointer to) Logical Index of new rule
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_FULL                  - on table full (no free logical index)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbUtilNewRuleLogIndexGet
(
    IN   GT_U32                                                    vTcamMngId,
    IN   GT_U32                                                    vTcamId,
    IN   GT_U32                                                    origIndexForPriorityUpdate ,
    IN   GT_U32                                                    vTcamMaxSize,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_HW_CONTEXT_STC  *hwContextPtr,
    IN   GT_U32                                                    priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    IN   GT_U32                                                    sizeIncrement,
    IN   GT_BOOL                                                   recalled,
    OUT  GT_U32                                                    *logIndexPtr
)
{
    GT_STATUS   rc;
    GT_BOOL     found;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                  *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC         *vtcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC     needToFindInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    GT_U32      logicalIndex = 0;
    GT_U32      freeLogicalIndex = 0;
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    GT_U32      ii;
    GT_BOOL     lookForUnusedEntry;
    GT_U32      higherValidIndex;
    GT_U32      saveGuaranteedSize;
    GT_BOOL     indexIsOk = GT_TRUE;
    RESIZE_MODE_ENT  resizeMode;

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if(vTcamMngDBPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    vTcamMaxSize = vTcamMaxSize;CPSS_TBD_BOOKMARK

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod !=
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
    {
        /* the function is not for this mode */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: the function is only for priority mode");
    }

    /* find the priority in the DB */
    rc = prvCpssDxChVirtualTcamDbPriorityTableEntryGet(
        vtcamInfoPtr,priority,&priorityEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    CPSS_LOG_INFORMATION_MAC("the priority[%d] hold : baseLogIndex[%d] rangeSize[%d] \n",
        priorityEntry.priority,
        priorityEntry.baseLogIndex,
        priorityEntry.rangeSize);

    if(priorityEntry.priority != priority)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: the DB hold different priority[%d] than expected[%d]",
            priorityEntry.priority,priority);
    }

    if((priorityEntry.baseLogIndex + priorityEntry.rangeSize) >
        vtcamInfoPtr->rulesAmount)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: the DB hold mismatch information regarding baseLogIndex[%d] , rangeSize[%d] and rulesAmount[%d]",
            priorityEntry.baseLogIndex,
            priorityEntry.rangeSize,
            vtcamInfoPtr->rulesAmount);
    }

    logicalIndex = 0;/*dummy for compiler warnings*/
    resizeMode   = RESIZE_MODE_WITH_INPUT_INDEX_E;
    lookForUnusedEntry = GT_TRUE;
    if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E)
    {
        if((priorityEntry.baseLogIndex + priorityEntry.rangeSize) ==
            vtcamInfoPtr->rulesAmount)
        {
            indexIsOk = GT_FALSE;/* the index is not OK because it not match the {base,range} */
            resizeMode = RESIZE_MODE_WITHOUT_INPUT_INDEX_E;
            if( priorityEntry.rangeSize == 0)
            {
                /* new priority is added to the vtcam as worst priority and got indication that
                   end of range is used */
                if(priorityEntry.baseLogIndex == 0)
                {
                    logicalIndex = 0;
                    CPSS_LOG_INFORMATION_MAC("use logical index[%d] (empty vtcam) as last index and check if can auto-resize in the new priority \n",
                        logicalIndex);
                }
                else
                {
                    logicalIndex = priorityEntry.baseLogIndex - 1;
                    CPSS_LOG_INFORMATION_MAC("use logical index[%d] (last index in the vtcam) as last index and push others to make room for index in the new priority \n",
                        logicalIndex);
                }
            }
            else
            {
                /* this priority uses last indexes in the vtcam , so can't use 'next index' */
                /* we need to calculate where is the nearest hole. */
                logicalIndex = priorityEntry.baseLogIndex + priorityEntry.rangeSize - 1;
                CPSS_LOG_INFORMATION_MAC("use logical index[%d] (last index in the vtcam) as last index and push others to make room for last index in the priority \n",
                    logicalIndex);
            }

            lookForUnusedEntry = GT_FALSE;
        }
    }
    else /* position is 'FIRST' */
    {
        if(priorityEntry.baseLogIndex == vtcamInfoPtr->rulesAmount)
        {
            /* rangeSize must be 0 at this point ! so this is new priority */
            /* so use index inside the valid rules amount ... like for 'LAST' */

            indexIsOk = GT_FALSE;/* the index is not OK because it not match the {base,range} */
            resizeMode = RESIZE_MODE_WITHOUT_INPUT_INDEX_E;
            if(priorityEntry.baseLogIndex == 0)
            {
                logicalIndex = 0;
                CPSS_LOG_INFORMATION_MAC("use logical index[%d] (empty vtcam) as first index and check if can auto-resize in the new priority \n",
                    logicalIndex);
            }
            else
            {
                logicalIndex = priorityEntry.baseLogIndex - 1;
                CPSS_LOG_INFORMATION_MAC("use logical index[%d] (last index in the vtcam) as first index and push others to make room for index in the new priority \n",
                    logicalIndex);
            }
            lookForUnusedEntry = GT_FALSE;
        }
    }

    if(lookForUnusedEntry == GT_TRUE)
    {
        if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E)
        {
            /* we need new logical index .
                start with the obvious one --> the 'next index'
            */
            logicalIndex = priorityEntry.baseLogIndex + priorityEntry.rangeSize;
            CPSS_LOG_INFORMATION_MAC("check the logical index[%d] as next after last index in the priority \n",
                logicalIndex);
        }
        else
        {
            /* we need new logical index .
                start with the obvious one --> the 'first index'
            */
            logicalIndex = priorityEntry.baseLogIndex;
            CPSS_LOG_INFORMATION_MAC("check the logical index[%d] as lowest index in the priority \n",
                logicalIndex);
        }
    }
    else
    {
        /* already got logicalIndex */
    }

    /* check if can keep the same index without HW moves */
    if(logicalIndex < vtcamInfoPtr->rulesAmount)/* index within the range */
    {
        if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E)
        {
            if((logicalIndex == (origIndexForPriorityUpdate + 1) && (lookForUnusedEntry == GT_TRUE)) || /*last in current priority
                            (if rangeSize == 0 than it is last in previous priority) */
               logicalIndex == origIndexForPriorityUpdate)  /*first in next priority */
            {
                CPSS_LOG_INFORMATION_MAC("no HW moves needed (to set rule as last)\n");
                logicalIndex = origIndexForPriorityUpdate;
                freeLogicalIndex = logicalIndex;

                goto foundSameIndexToUse_lbl;
            }
        }
        else
        {
            if((logicalIndex == (origIndexForPriorityUpdate + 1) && (lookForUnusedEntry == GT_TRUE)) ||/*last in previous priority */
               logicalIndex == origIndexForPriorityUpdate)/* first in current priority
               (if rangeSize == 0 than it is first in next priority) */
            {
                /*    this 'first' is attached to last rule in previous priority */
                /* or this 'first' is already first rule in current or next priority */
                CPSS_LOG_INFORMATION_MAC("no HW moves needed (to set rule as first)\n");
                logicalIndex = origIndexForPriorityUpdate;
                freeLogicalIndex = logicalIndex;

                goto foundSameIndexToUse_lbl;
            }
        }
    }

    if(lookForUnusedEntry == GT_TRUE)
    {
        cpssOsMemSet(&needToFindInfo,0,sizeof(needToFindInfo));
        needToFindInfo.logicalIndex = logicalIndex;
        /* check if this logical index is occupied */
        found = prvCpssAvlPathFind(vtcamInfoPtr->priorityIndexTree,
                PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
                &needToFindInfo,/*pData*/
                avlTreePath,/*path*/
                &dbEntryPtr /* use dedicated var to avoid warnings */);
         foundPriorityEntryPtr = dbEntryPtr;

        if((found == GT_FALSE) || /* no equal or higher index */
           (foundPriorityEntryPtr->baseLogIndex > logicalIndex)) /* no equal index */
        {
            if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E)
            {
                /* the logical index is not used .. but look for better higher index that is not used */
                CPSS_LOG_INFORMATION_MAC("the logical Index[%d] is free  .. but look for better higher index that is not used \n");

                if(priorityEntry.rangeSize != 0)
                {
                    if(found == GT_FALSE)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: the DB not found priority[%d] in one DB but found in another",
                            priorityEntry.priority);
                    }

                    rc = findNearestLogicalIndexUsed(vtcamInfoPtr,
                        logicalIndex,
                        GT_FALSE, /*more : lessOrMore*/
                        priorityEntry.baseLogIndex + priorityEntry.rangeSize - 1,/*lastLogicalIndexToCheck*/
                        &higherValidIndex
                    );

                    if(rc != GT_OK)
                    {
                        /* we don't have better index than logicalIndex */
                        CPSS_LOG_INFORMATION_MAC("not found better logical Index , so [%d] will be used \n");
                    }
                    else
                    {
                        /* we got higher index of 'used index' ... so need to
                           use the one before it */
                        logicalIndex = higherValidIndex - 1;
                        CPSS_LOG_INFORMATION_MAC("found better logical Index[%d] and will be used \n");
                    }
                }
                else  /*priorityEntry.rangeSize == 0*/
                {
                    /* this is new range */
                    CPSS_LOG_INFORMATION_MAC("priority[%d] considered new and will be added with first logical index[%d] \n",
                        priorityEntry.priority ,
                        logicalIndex);
                    /*
                        the code below will do for this case:
                       priorityEntry.rangeSize++;

                       meaning that priorityEntry.rangeSize will be 1 ,
                       indicating that we add the first entry to the range
                    */
                }

            }
            else /*position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E*/
            {
                /* the logical index is not used */
                CPSS_LOG_INFORMATION_MAC("the logical Index[%d] is free and will be used \n");
            }

            /* we can use this new logical index */
            *logIndexPtr = logicalIndex;

            if(position == CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E ||
              (priorityEntry.baseLogIndex + priorityEntry.rangeSize) <= logicalIndex) /* POSITION_FIRST and add new priority entry into the DB */
            {
                priorityEntry.rangeSize++;
            }

            /* updated the DB about the new range value */
            rc = prvCpssDxChVirtualTcamDbPriorityTableEntrySet(
                vTcamMngDBPtr,vtcamInfoPtr,&priorityEntry);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }

            return GT_OK;
        }

        /* the logical index is used */
        CPSS_LOG_INFORMATION_MAC("the logical Index[%d] is used by priority[%d] baseLogIndex[%d] rangeSize[%d] \n",
            logicalIndex,
            foundPriorityEntryPtr->priority,
            foundPriorityEntryPtr->baseLogIndex,
            foundPriorityEntryPtr->rangeSize);
    }

    /* find nearest to specific logical index an unused logical index */
    rc = findNearestLogicalIndexHole(vtcamInfoPtr,
        logicalIndex,
        &freeLogicalIndex);
    if(rc != GT_OK)
    {
        /* No Hole - needed resize */
        if (vtcamInfoPtr->tcamInfo.autoResize == GT_FALSE)
        {
            return rc;
        }

        if (recalled != GT_FALSE)
        {
            /* should not occur */
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        /* Auto resize */
        /* resize memory for Virtual TCAM in Segment Table for TCAM.*/

        /* save guaranteedNumOfRules updated by resize function */
        saveGuaranteedSize = vtcamInfoPtr->tcamInfo.guaranteedNumOfRules;
        rc = vtcamDbSegmentTableVTcamResize(
            vTcamMngId ,vTcamId,
            sizeIncrement,
            resizeMode,
            logicalIndex);
        /* restore guaranteedNumOfRules - should remain as was */
        vtcamInfoPtr->tcamInfo.guaranteedNumOfRules = saveGuaranteedSize;
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
        }

        /* recursive call after resize */
        return prvCpssDxChVirtualTcamDbUtilNewRuleLogIndexGet(
            vTcamMngId, vTcamId,
            origIndexForPriorityUpdate,
            vTcamMaxSize, hwContextPtr,
            priority, position, GT_TRUE /*recalled*/,
            sizeIncrement, logIndexPtr);
    }

    if(freeLogicalIndex >= vtcamInfoPtr->rulesAmount)
    {
        /* wrong calculations */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: wrong calculations : freeLogicalIndex[%d] >= vtcamInfoPtr->rulesAmount[%d]",
            freeLogicalIndex,vtcamInfoPtr->rulesAmount);
    }

    /* so we got a hole in the logical index range */

    /* we need to move all logical indexes from current logical index to the
       free logical index */
    /* the move procedure  must invalidate source rule to guaranty consistency
       of following copied one. because write/copy updates trio action, pattern
       and mask sequentially but not in atomic manner.*/
    if(freeLogicalIndex < logicalIndex)
    {
        if(indexIsOk == GT_TRUE)
        {
            /* the logicalIndex is kept in the 'upper' priority */
            /* so decrease out index */
            logicalIndex--;
        }

        for(ii = freeLogicalIndex ; ii < logicalIndex; ii++)
        {
            /* call the CB function to move each entry at a time */
            rc = hwContextPtr->ruleMoveFuncPtr(hwContextPtr->cookiePtr,
                ii+1,/*srcRuleLogicalIndex*/
                ii,/*dstRuleLogicalIndex*/
                vtcamInfoPtr->tcamInfo.ruleSize,
                GT_TRUE/*move*/);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }
        }

        /* copy the bits in the DB of vtcamInfoPtr->usedRulesBitmapArr */
        copyBitsInMemory(vtcamInfoPtr->usedRulesBitmapArr,/*memPtr*/
                 freeLogicalIndex,/*targetStartBit*/
                 freeLogicalIndex + 1, /*sourceStartBit*/
                 logicalIndex - freeLogicalIndex);/*numBits*/
    }
    else
    if(freeLogicalIndex > logicalIndex)
    {
        for(ii = freeLogicalIndex-1 ; ii >= logicalIndex; ii--)
        {
            /* call the CB function to move each entry at a time */
            rc = hwContextPtr->ruleMoveFuncPtr(hwContextPtr->cookiePtr,
                ii,/*srcRuleLogicalIndex*/
                ii+1,/*dstRuleLogicalIndex*/
                vtcamInfoPtr->tcamInfo.ruleSize,
                GT_TRUE/*move*/);
            if (rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }

            if(ii == 0)/* logicalIndex is 0 */
            {
                break;
            }
        }

        /* copy the bits in the DB of vtcamInfoPtr->usedRulesBitmapArr */
        copyBitsInMemory(vtcamInfoPtr->usedRulesBitmapArr,/*memPtr*/
                 logicalIndex + 1,/*targetStartBit*/
                 logicalIndex, /*sourceStartBit*/
                 freeLogicalIndex - logicalIndex);/*numBits*/
    }
    else /* freeLogicalIndex == logicalIndex */
    {
        foundSameIndexToUse_lbl:
        /*do nothing more*/
        ;
    }

    /* update the priority DB about the removed index , that will move the ranges of them */
    rc = priorityDbLogicalIndexRemoveAndMovePriority(vTcamMngDBPtr,vtcamInfoPtr,
        priority,position,
        logicalIndex,freeLogicalIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* get the priority again from the DB ... because it may have changed */
    rc = prvCpssDxChVirtualTcamDbPriorityTableEntryGet(
        vtcamInfoPtr,priority,&priorityEntry);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* update the rules DB about the removed index , that will change logical indexes*/
    rc = rulesDbLogicalIndexRemove(vtcamInfoPtr,logicalIndex,freeLogicalIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    if(priorityEntry.baseLogIndex == vtcamInfoPtr->rulesAmount)
    {
        if(priorityEntry.rangeSize != 0)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: expected new priority but already hold rangeSize[%d]",
                priorityEntry.rangeSize);
        }
        priorityEntry.baseLogIndex--;
        priorityEntry.rangeSize = 1;

        /* updated the DB about the new range value */
        rc = prvCpssDxChVirtualTcamDbPriorityTableEntrySet(
            vTcamMngDBPtr,vtcamInfoPtr,&priorityEntry);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }
    else
    if(priorityEntry.rangeSize == 0)
    {
        priorityEntry.rangeSize++;
        /* updated the DB about the new range value */
        rc = prvCpssDxChVirtualTcamDbPriorityTableEntrySet(
            vTcamMngDBPtr,vtcamInfoPtr,&priorityEntry);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    /* the freeLogicalIndex got the 'shift' so we actually state to caller that
       'logicalIndex' is actually free now !!! */
    *logIndexPtr = logicalIndex;


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert
*
* DESCRIPTION:
*       convert rule Id to logical index and to physical index.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId                  - ruleId to find.
*       checkExistance      - indication if need to check that the ruleId already
*                           exists in the DB.
*
* OUTPUTS:
*       logicalIndexPtr      - (pointer to) the logical index.  (can be NULL)
*       physicalIndexPtr     - (pointer to) the physical index. (can be NULL)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN   GT_BOOL                                    checkExistance,
    OUT  GT_U32                                     *logicalIndexPtr,
    OUT  GT_U32                                     *physicalIndexPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC ruleIdEntry;
    GT_U32 logicalIndex;

    if(vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        /* vTcamMngId out of range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"vTcamMngId[%d] it out of valid range[0..%d] \n",
            vTcamMngId,
            CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS);
    }

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if(vTcamMngDBPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"vTcamMngId[%d] was not created \n",
            vTcamMngId);
    }

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"vTcamId[%d] was not created \n",
            vTcamId);
    }

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E)
    {
        logicalIndex = ruleId;

        if(logicalIndex >= vtcamInfoPtr->rulesAmount)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"ruleId[%d] it out of valid range[0..%d] \n",
                ruleId,
                vtcamInfoPtr->rulesAmount-1);
        }
    }
    else /*CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E*/
    {
        /* convert the ruleId to logical index */
        rc = prvCpssDxChVirtualTcamDbRuleIdTableEntryGet(vtcamInfoPtr, ruleId, &ruleIdEntry);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"The ruleId[%d] was not created",
                ruleId);
        }

        logicalIndex = ruleIdEntry.logicalIndex;

        if(logicalIndex >= vtcamInfoPtr->rulesAmount)
        {
            /* error in DB management */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error:logicalIndex[%d] >= vtcamInfoPtr->rulesAmount[%d] ",
                logicalIndex ,
                vtcamInfoPtr->rulesAmount);
        }
    }

    CPSS_LOG_INFORMATION_MAC("converted rule[%d] to logical index[%d] and HW index[%d] \n",
        ruleId,
        logicalIndex,
        vtcamInfoPtr->rulePhysicalIndexArr[logicalIndex]);

    if(checkExistance == GT_TRUE)
    {
        /* check if the rule exists in the DB  */
        if(0 == (vtcamInfoPtr->usedRulesBitmapArr[logicalIndex>>5] & (1<<(logicalIndex & 0x1f))))
        {
            /* the rule not exists in the DB */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND,"the logicalIndex[%d] of rule[%d] not exists in the DB",
                logicalIndex,
                ruleId);
        }
    }

    /* convert the logical index to physical index in the HW */
    if(physicalIndexPtr)
    {
        *physicalIndexPtr = vtcamInfoPtr->rulePhysicalIndexArr[logicalIndex];
    }

    if(logicalIndexPtr)
    {
        *logicalIndexPtr = logicalIndex;
    }

    return GT_OK;
}

/* cookie for callback_virtualTcamDbRuleIdInvalidate(...) */
typedef struct{
    GT_U32  vTcamMngId;
    GT_U32  vTcamId;
}CALLBACK_COOKIE_INFO_STC;
/*
 * Typedef: function PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_INVALIDATE_FUNC
 *
 * Description:
 *     This Function invalidates rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr            - device info passed to callback function.
 *       ruleLogicalIndex     - rule logial index.
 *       ruleSize             - size of rules used in virtual TCAM.
 *
 *  Comments:
 *
 */
static GT_STATUS callback_virtualTcamDbRuleIdInvalidate
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      ruleLogicalIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT      ruleSize
)
{
    GT_STATUS   rc;
    GT_U8      devNum;
    CALLBACK_COOKIE_INFO_STC*  myCookiePtr = cookiePtr;

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(myCookiePtr->vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* iteration over the devices */
    do
    {
        /* invalidate the rule in the device */
        rc = perDevice_ruleValidate(devNum,
                                    ruleLogicalIndex,
                                    ruleSize,
                                    GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(myCookiePtr->vTcamMngId,&devNum));

    return GT_OK;
}

/*
 * Typedef: function PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_RULE_MOVE_FUNC
 *
 * Description:
 *     This Function move/copy rule in TCAM.
 *
 * INPUTS:
 *       cookiePtr               - device info passed to callback function.
 *       srcRuleLogicalIndex     - source rule logical index.
 *       dstRuleLogicalIndex     - destination rule logical index.
 *       ruleSize                - size of rules used in virtual TCAM.
 *
 *  Comments:
 *
 */
static GT_STATUS callback_virtualTcamDbRuleIdMove
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      srcRuleLogicalIndex,
    IN  GT_U32      dstRuleLogicalIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT      ruleSize,
    IN  GT_BOOL     moveOrCopy
)
{
    GT_STATUS   rc;
    GT_U8      devNum;
    CALLBACK_COOKIE_INFO_STC*  myCookiePtr = cookiePtr;
    GT_U32  srcDeviceRuleIndex,trgDeviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;

    CPSS_LOG_INFORMATION_MAC("Move Rule from logical index[%d] to [%d] size[%d] \n",
        srcRuleLogicalIndex ,  dstRuleLogicalIndex,  ruleSize);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(myCookiePtr->vTcamMngId,myCookiePtr->vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(myCookiePtr->vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* convert the logical index to physical index in the HW */
    srcDeviceRuleIndex = vtcamInfoPtr->rulePhysicalIndexArr[srcRuleLogicalIndex];
    trgDeviceRuleIndex = vtcamInfoPtr->rulePhysicalIndexArr[dstRuleLogicalIndex];

    /* iteration over the devices */
    do
    {
        /* move the rule in the device */
        rc = perDevice_ruleMove(devNum,ruleSize,
                    srcDeviceRuleIndex,trgDeviceRuleIndex,moveOrCopy);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(myCookiePtr->vTcamMngId,&devNum));

    return GT_OK;
}

/*******************************************************************************
* priorityAddNewRuleNeedNewLogicalIndex
*
* DESCRIPTION:
*       Get place for new rule.
*       If needed move existing rules.
*       If needed resize vTCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       origIndexForPriorityUpdate - the original index of the entry that doing
*                               'priority update'
*                               value ORIG_INDEX_NOT_USED_CNS meaning that it is
*                               not relevant.
*       priority                - new rule priority
*       position                - position in range of rule of the
*                                 specified priority (see enum)
*
* OUTPUTS:
*       logIndexPtr         - (pointer to) Logical Index of new rule
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_FULL                  - on table full (no free logical index)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityAddNewRuleNeedNewLogicalIndex
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN   GT_U32                                                    origIndexForPriorityUpdate ,
    IN   GT_U32                                                    priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    OUT  GT_U32                                                    *logIndexPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_TAB_HW_CONTEXT_STC hwContext;
    CALLBACK_COOKIE_INFO_STC cookieInfo;
    GT_U32 logicalIndex;
    GT_U32 autoResizeAddition;

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if(vTcamMngDBPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    autoResizeAddition =
        (vtcamInfoPtr->tcamInfo.autoResize == GT_FALSE)
            ? 0 : PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_AUTO_RESIZE_GRANULARITY_CNS;

    cookieInfo.vTcamMngId = vTcamMngId;
    cookieInfo.vTcamId = vTcamId;

    /* build the call back info */
    hwContext.cookiePtr = &cookieInfo;
    hwContext.ruleInvalidateFuncPtr = callback_virtualTcamDbRuleIdInvalidate;
    hwContext.ruleMoveFuncPtr = callback_virtualTcamDbRuleIdMove;

    /* Get logical index for the new rule .
       Get place for new rule.
       If needed move existing rules.
       If needed resize vTCAM.
    */
    rc = prvCpssDxChVirtualTcamDbUtilNewRuleLogIndexGet(
        vTcamMngId, vTcamId,
        origIndexForPriorityUpdate,
        (vtcamInfoPtr->rulesAmount + autoResizeAddition),
        &hwContext,
        priority,
        position,
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_AUTO_RESIZE_GRANULARITY_CNS,/*sizeIncrement,*/
        GT_FALSE /*recalled*/,
        &logicalIndex
    );

    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    *logIndexPtr = logicalIndex;

    return GT_OK;
}

/*******************************************************************************
* priorityRuleWritePreparations
*
* DESCRIPTION:
*       make preparations before adding new rule in priority mode.
*       the place for new rule may require to :
*       1. move existing rules.
*       2. resize vTCAM.
*       3. close holes in the tcam.
*
*       NOTE: the actual 'new rule' addition is not done in this API
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       vtcamInfoPtr      - (pointer to) virtual TCAM entry
*       ruleId            - ruleId to find.
*       priority          - priority.
* OUTPUTS:
*       logicalIndexPtr  - (pointer to) the logical index of the rule
*       isNotExistPtr       - (pointer to) indication that the ruleId does not exist in DB
*                           prior to this action.
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityRuleWritePreparations
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  GT_U32                                      priority,
    OUT  GT_U32                                     *logicalIndexPtr,
    OUT GT_BOOL                                     *isNotExistPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC ruleIdEntry;
    GT_U32 logicalIndex;
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry;
    CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT position;

    *isNotExistPtr = GT_TRUE;

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E)
    {
        /* the function is not for this mode */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
    }

    /* convert the ruleId to logical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdTableEntryGet(vtcamInfoPtr,ruleId, &ruleIdEntry);
    if(rc == GT_OK)/* already exists ... has logical index */
    {
        CPSS_LOG_INFORMATION_MAC("ruleId[%d] already exists with logical index[%d] \n",
            ruleId,
            ruleIdEntry.logicalIndex);

        *isNotExistPtr = GT_FALSE;

        logicalIndex = ruleIdEntry.logicalIndex;
        if(logicalIndex >= vtcamInfoPtr->rulesAmount)
        {
            /* error in DB management */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error:logicalIndex[%d] >= vtcamInfoPtr->rulesAmount[%d] ",
                logicalIndex , vtcamInfoPtr->rulesAmount);
        }

        /* check if the rule was already in the DB  */
        if(0 == (vtcamInfoPtr->usedRulesBitmapArr[logicalIndex>>5] & (1<<(logicalIndex & 0x1f))))
        {
            /* error in DB management */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error:logicalIndex[%d] not found in usedRulesBitmapArr[]",
                logicalIndex);
        }

        /* need to check if the priority in the DB is the same as the given one */
        rc = prvCpssDxChVirtualTcamDbPriorityTableEntryFind(
            vtcamInfoPtr,
            logicalIndex,
            PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,/*findType*/
            avlTreePath,
            &priorityEntry
        );
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        if(priorityEntry.priority != priority)
        {
            /* mismatch */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"the priority[%d] must be the same as the DB priority[%d]",
                priority,priorityEntry.priority);
        }

        if(priorityEntry.baseLogIndex > logicalIndex ||
           (priorityEntry.baseLogIndex + priorityEntry.rangeSize) <= logicalIndex )
        {

            /* 'management' error */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: the DB hold mismatch information regarding baseLogIndex[%d] , rangeSize[%d] and logicalIndex[%d]",
                priorityEntry.baseLogIndex,
                priorityEntry.rangeSize,
                logicalIndex);
        }

    }
    else
    if(rc == GT_NOT_FOUND) /* not found .. need new logical index */
    {
        CPSS_LOG_INFORMATION_MAC("ruleId[%d] not found .. need new logical index \n",
            ruleId);

         *isNotExistPtr = GT_TRUE;

         if (vtcamInfoPtr->usedRulesAmount > vtcamInfoPtr->rulesAmount)
         {
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: vtcam [%d] : usedRulesAmount[%d] > rulesAmount[%d]",
                 vTcamId,
                 vtcamInfoPtr->usedRulesAmount,
                 vtcamInfoPtr->rulesAmount);
         }

         if (vtcamInfoPtr->usedRulesAmount == vtcamInfoPtr->rulesAmount)
         {
             if (vtcamInfoPtr->tcamInfo.autoResize == GT_FALSE)
             {
                 /* the vtcam already full without auto-resize option */
                 CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL,"vtcam [%d] is full and without auto-resize option for new rule in priority[%d]",
                     vTcamId,priority);
             }
         }

         if(GT_TRUE == debug_writeRulePosition_enable)
         {
            /* debug mode to allow quick testing of the 'position first' that
               the 'rule Update' API allow to use. */
            position = debug_writeRulePosition;
         }
         else
         {
            position = CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E;
         }

        /* Get logical index for the new rule .
           Get place for new rule.
           If needed move existing rules.
           If needed resize vTCAM.
        */
        rc = priorityAddNewRuleNeedNewLogicalIndex(
            vTcamMngId,
            vTcamId,
            ORIG_INDEX_NOT_USED_CNS,/*origIndexForPriorityUpdate*/
            priority,
            position,/*position*/
            &logicalIndex
        );
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        CPSS_LOG_INFORMATION_MAC("ruleId[%d] got new logical index[%d] \n",
            ruleId,
            logicalIndex);

    }
    else
    {
        /* error */
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    *logicalIndexPtr = logicalIndex;
    return GT_OK;
}

/*******************************************************************************
* priorityNewRuleIdAddedDbUpdate
*
* DESCRIPTION:
*       priority mode : update the DB about new rule ID that was added.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - ruleId to find.
*       logicalIndex      - logical index

* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on ruleId or/and logicalIndex already used
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS priorityNewRuleIdAddedDbUpdate
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  GT_U32                                      logicalIndex
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC ruleIdEntry;

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if(vTcamMngDBPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    cpssOsMemSet(&ruleIdEntry,0,sizeof(ruleIdEntry));

    ruleIdEntry.ruleId = ruleId;
    ruleIdEntry.logicalIndex = logicalIndex;

    CPSS_LOG_INFORMATION_MAC("Add Rule to DB : ruleId[%d] , logicalIndex[%d] \n");
    rc =  prvCpssDxChVirtualTcamDbRuleIdTableEntryAdd(vTcamMngDBPtr,
        vtcamInfoPtr,&ruleIdEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* NOTE: the priority DB was updated during
     prvCpssDxChVirtualTcamDbUtilNewRuleLogIndexGet(...) */


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableEntrySet
*
* DESCRIPTION:
*       Add or Update Logical Index Range to Priority Table.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       entryPtr        - (pointer to) entry with info
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       1. The given range should not overlap any existing range.
*       2. if priority0 >= priority1 then baseLogIndex0 >= baseLogIndex1
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableEntrySet
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC           *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *tcamCfgPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC   *entryPtr
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    * priorityEntryPtr;

    /* find priority entry in AVL tree */
    priorityEntryPtr = prvCpssAvlSearch(tcamCfgPtr->priorityPriTree, entryPtr);
    /* priority entry not found */
    if (priorityEntryPtr == NULL)
    {
        /* get free buffer from priority pool.*/
        priorityEntryPtr = cpssBmPoolBufGet(pVTcamMngPtr->priorityPool);
        if (priorityEntryPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL,"cpssBmPoolBufGet: error getting free buffer from pool");
        }

        cpssOsMemCpy(priorityEntryPtr, entryPtr,
                     sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC));

        /* insert new logical index range into priority tree */
        rc = prvCpssAvlItemInsert(tcamCfgPtr->priorityPriTree, priorityEntryPtr);
        if (rc != GT_OK)
        {
            cpssBmPoolBufFree(pVTcamMngPtr->priorityPool, priorityEntryPtr);
            return rc;
        }
        /* insert new logical index range into priority index tree */
        rc = prvCpssAvlItemInsert(tcamCfgPtr->priorityIndexTree, priorityEntryPtr);
        if (rc != GT_OK)
        {
            prvCpssAvlItemRemove(tcamCfgPtr->priorityPriTree, priorityEntryPtr);
            cpssBmPoolBufFree(pVTcamMngPtr->priorityPool, priorityEntryPtr);

            return rc;
        }
    }
    else
    {
        /* priority entry found in tree */
        if (entryPtr->rangeSize == 0)
        {
            /* delete entry from priority tree */
            prvCpssAvlItemRemove(tcamCfgPtr->priorityPriTree, priorityEntryPtr);
            /* delete entry from priority index tree */
            prvCpssAvlItemRemove(tcamCfgPtr->priorityIndexTree, priorityEntryPtr);
            /* free a buffer back to its pool */
            rc = cpssBmPoolBufFree(pVTcamMngPtr->priorityPool, priorityEntryPtr);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            /* update existing logical index range in priority tree */
            cpssOsMemCpy(priorityEntryPtr, entryPtr,
                         sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC));
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableEntryGet
*
* DESCRIPTION:
*       Get Logical Index Range from Priority Table.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       priority        - rules priority.
*
* OUTPUTS:
*       entryPtr        - (pointer to) entry with info
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       For not existing range returned *rangeSizePtr == 0 and
*       *baseLogIndexPtr points to the end of the last range
*       that should be before the required range, i.e. the range with
*       minimal priority greater the given (if not exist - *baseLogIndexPtr == 0)
*       This value can be used as base of this not existing range adding it.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableEntryGet
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *tcamCfgPtr,
    IN   GT_U32                                       priority,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC   *entryPtr
)
{
    PRV_CPSS_AVL_TREE_PATH      avlTreePath;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry, * priorityEntryPtr;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    GT_BOOL retVal;

    /* set priority to start find in tree */
    priorityEntry.priority = priority;
    priorityEntry.baseLogIndex = 0; /* don't care */
    priorityEntry.rangeSize = 0;    /* don't care */

    retVal = prvCpssAvlPathFind(tcamCfgPtr->priorityPriTree,
                                PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,
                                &priorityEntry, /* pData */
                                avlTreePath,    /* path */
                                &dbEntryPtr     /* use dedicated var to avoid warnings */);
    if (retVal == GT_TRUE)
    {
        priorityEntryPtr = dbEntryPtr;
        /* exact match */
        if (priorityEntryPtr->priority == priority)
        {
            /* copy to output entry  */
            cpssOsMemCpy(entryPtr, priorityEntryPtr,
                         sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC));
        }
        else
        {
            /* maximal less */
            entryPtr->priority = priority;
            /* calculate base logical index for new priority */
            entryPtr->baseLogIndex = priorityEntryPtr->baseLogIndex + priorityEntryPtr->rangeSize;
            entryPtr->rangeSize = 0;
        }
    }
    else
    {
        /* not found - required less than all present */
        entryPtr->priority = priority;
        entryPtr->baseLogIndex = 0;
        entryPtr->rangeSize = 0;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbPriorityTableEntryFind
*
* DESCRIPTION:
*       Find entry Id rule by logical index.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr      - (pointer to) VTCAM Manager structure
*       tcamCfgPtr        - (pointer to)virtual TCAM structure
*       logicalIndex      - logical index to search.
*       findType          - find type: see enum.
*
* OUTPUTS:
*       treePathArr       - (pointer to) tree path array.
*       entryPtr          - (pointer to) entry with info
*
* RETURNS:
*       GT_OK             - on success
*       GT_NOT_FOUND      - entry not found
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbPriorityTableEntryFind
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC            *tcamCfgPtr,
    IN   GT_U32                                                  logicalIndex,
    IN   PRV_CPSS_AVL_TREE_FIND_ENT                              findType,
    OUT  PRV_CPSS_AVL_TREE_PATH                                  treePathArr,
    OUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC       *entryPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry, * priorityEntryPtr;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    GT_BOOL retVal;

    /* set logical index to start find in tree */
    priorityEntry.priority = 0;     /* don't care */
    priorityEntry.baseLogIndex = logicalIndex;
    priorityEntry.rangeSize = 0;    /* don't care */

    retVal = prvCpssAvlPathFind(tcamCfgPtr->priorityIndexTree,
                                findType,
                                &priorityEntry, /* pData */
                                treePathArr,    /* path */
                                &dbEntryPtr     /* use dedicated var to avoid warnings */);
    /* entry not found */
    if (retVal == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND,"prvCpssAvlPathFind: entry with logical index %d not found", logicalIndex);
    }

    priorityEntryPtr = dbEntryPtr;
    /* copy to output entry  */
    cpssOsMemCpy(entryPtr, priorityEntryPtr,
                 sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC));

    return GT_OK;
}

/*******************************************************************************
* priorityTableEntryRemoveLogicalIndexUpdateRange
*
* DESCRIPTION:
*       Remove Logical Index from the Range of Priority.
*       ONLY if the removed logical index is the 'last' in the range we need to update
*       the range.
*       otherwise we not update the range.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr    - (pointer to) VTCAM Manager structure
*       tcamCfgPtr      - (pointer to)virtual TCAM structure
*       priority        - rules priority.
*       logicalIndex    - the logical index to remove from the range
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS priorityTableEntryRemoveLogicalIndexUpdateRange
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC           *vTcamMngDBPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC  *vtcamInfoPtr,
    IN   GT_U32                                       priority,
    IN   GT_U32                                       logicalIndex
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry;
    GT_U32  prevValidIndex;

    /* find the priority in the DB */
    rc = prvCpssDxChVirtualTcamDbPriorityTableEntryGet(
        vtcamInfoPtr,priority,&priorityEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(priorityEntry.rangeSize == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"rangeSize is 0 for priority[%d]",
            priority);
    }

    if((priorityEntry.baseLogIndex + priorityEntry.rangeSize - 1) != logicalIndex)
    {
        /* nothing to do .. not the last index in the range */
        return GT_OK;
    }

    CPSS_LOG_INFORMATION_MAC("deleting the last logical index[%d] for priority[%d] , need to calculate new rangeSize \n",
        logicalIndex,priority);

    if(priorityEntry.baseLogIndex == logicalIndex)
    {
        /* the current priority should be removed */
        priorityEntry.rangeSize = 0;
        /* we remove the single 'first' and 'last' index */
        CPSS_LOG_INFORMATION_MAC("The priority[%d] is now without logical indexes .. will be removed \n",
            priorityEntry.rangeSize);
    }
    else
    {
        /* removing the last logical index of the priority ..
           so check for collapse until first valid logical index */
        rc = findNearestLogicalIndexUsed(vtcamInfoPtr,
            logicalIndex,
            GT_TRUE, /*less : lessOrMore*/
            priorityEntry.baseLogIndex,/*lastLogicalIndexToCheck*/
            &prevValidIndex
        );

        if(rc == GT_OK)
        {
            if(prevValidIndex < priorityEntry.baseLogIndex)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"prevValidIndex[%d] can't be less than priorityEntry.baseLogIndex[%d]",
                    prevValidIndex,
                    priorityEntry.baseLogIndex);
            }

            /* found next valid index previous to the removed one ...
                update the rangeSize */
            priorityEntry.rangeSize = 1 + (prevValidIndex - priorityEntry.baseLogIndex);

            CPSS_LOG_INFORMATION_MAC("new priorityEntry.rangeSize[%d] for priority[%d] \n",
                priorityEntry.rangeSize);
        }
        else
        {
            /* the current priority should be removed */
            priorityEntry.rangeSize = 0;

            CPSS_LOG_INFORMATION_MAC("The priority[%d] is now without logical indexes .. will be removed \n",
                priorityEntry.rangeSize);
        }
    }

    /* update the trees of the priority */
    rc = prvCpssDxChVirtualTcamDbPriorityTableEntrySet(
        vTcamMngDBPtr,vtcamInfoPtr,&priorityEntry);

    return rc;

}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbRuleIdTableEntryRemoveByLogicalIndex
*
* DESCRIPTION:
*       Remove rule Id entry by logical index from tree : ruleIdIndexTree
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       logicalIndex            - logical index of ruleId to remove.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on ruleId or/and logicalIndex already used
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChVirtualTcamDbRuleIdTableEntryRemoveByLogicalIndex
(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                *pVTcamMngPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC       *tcamCfgPtr,
    IN  GT_U32                                              logicalIndex
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC ruleIdEntry, * ruleIdEntryPtr;

    ruleIdEntry.logicalIndex = logicalIndex;
    /* find logicalIndex in entry in AVL tree */
    ruleIdEntryPtr = prvCpssAvlSearch(tcamCfgPtr->ruleIdIndexTree, &ruleIdEntry);
    if (ruleIdEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* call to delete by ruleId */
    rc = prvCpssDxChVirtualTcamDbRuleIdTableEntryRemove(
            pVTcamMngPtr,
            tcamCfgPtr,
            ruleIdEntryPtr->ruleId);

    return rc;
}
/*******************************************************************************
* priorityDbRemoveRuleId
*
* DESCRIPTION:
*       Remove rule Id entry and update priority entry if needed when the ruleId
*       is deleted.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       pVTcamMngPtr            - (pointer to) VTCAM Manager structure
*       tcamCfgPtr              - (pointer to)virtual TCAM structure
*       logicalIndex            - logicalIndex of the ruleId.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_BAD_STATE             - on DB mismatch
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS priorityDbRemoveRuleId(
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                 *vTcamMngDBPtr,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC        *vtcamInfoPtr,
    IN  GT_U32                                              logicalIndex
)
{
    GT_STATUS rc;
    GT_U32  priority;/* the priority of the ruleId */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry;
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;

    /* Remove rule Id entry by logical index from tree : ruleIdIndexTree */
    rc = prvCpssDxChVirtualTcamDbRuleIdTableEntryRemoveByLogicalIndex(
            vTcamMngDBPtr,
            vtcamInfoPtr,
            logicalIndex);
    if(rc != GT_OK)
    {
        return rc;
    }
    /* get the priority of the ruleId */
    rc = prvCpssDxChVirtualTcamDbPriorityTableEntryFind(
        vtcamInfoPtr,
        logicalIndex,
        PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,/*findType*/
        avlTreePath,
        &priorityEntry
    );
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    priority = priorityEntry.priority;

    /* notify the priority DB that the logical index is removed from the range */
    /* if this is the last logical index in the range it will update the range */
    rc = priorityTableEntryRemoveLogicalIndexUpdateRange(vTcamMngDBPtr,vtcamInfoPtr,
        priority,
        logicalIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/* dynamic allocated array */
PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RESIZE_HW_INDEX_MAP_INFO_STC    *prvCpssDxChVirtualTcamDbResizeHwIndexDbArr = NULL;
GT_U32                           prvCpssDxChVirtualTcamDbNumEntries_resizeHwIndexDbArr = 0;

/*******************************************************************************
* resizeMapHwIndexesToLogicalIndexes
*
* DESCRIPTION:
*       (for resize purposes) map HW index to logical indexes of
*       the 'old'/'new' logical index.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr     - (pointer to)virtual TCAM manager structure
*       vTcamInfoPtr      - (pointer to) virtual TCAM structure
*       oldOrNew          - GT_TRUE - old mapping, GT_FALSE-new mapping.
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_CPU_MEM        - on Cpu memory allocation failed.
*       GT_BAD_STATE             - on DB management error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS resizeMapHwIndexesToLogicalIndexes(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr,
    IN GT_BOOL                                      oldOrNew
)
{
    GT_U32  logicalIndex;
    GT_U32  hwIndex;
    GT_U32  maxSegmentsInDb = vTcamMngDBPtr->tcamSegCfg.maxSegmentsInDb;
    GT_U32  memSize = sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RESIZE_HW_INDEX_MAP_INFO_STC) * maxSegmentsInDb;

    if((prvCpssDxChVirtualTcamDbResizeHwIndexDbArr != NULL) &&
        (prvCpssDxChVirtualTcamDbNumEntries_resizeHwIndexDbArr < maxSegmentsInDb))
    {
        /* There is not enough space at the array - reallocate it */
        cpssOsFree(prvCpssDxChVirtualTcamDbResizeHwIndexDbArr);
        prvCpssDxChVirtualTcamDbResizeHwIndexDbArr = NULL;
        prvCpssDxChVirtualTcamDbNumEntries_resizeHwIndexDbArr = 0;
    }

    if(prvCpssDxChVirtualTcamDbResizeHwIndexDbArr == NULL)
    {
        if(oldOrNew == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        prvCpssDxChVirtualTcamDbResizeHwIndexDbArr = cpssOsMalloc(memSize);
        if(prvCpssDxChVirtualTcamDbResizeHwIndexDbArr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        prvCpssDxChVirtualTcamDbNumEntries_resizeHwIndexDbArr = maxSegmentsInDb;
    }

    if(oldOrNew == GT_TRUE)
    {
        for(hwIndex = 0; hwIndex < maxSegmentsInDb; hwIndex++)
        {
            prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].oldLogicalIndex = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS;
            prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].newLogicalIndex = PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS;
        }
    }

    for(logicalIndex = 0; logicalIndex < vTcamInfoPtr->rulesAmount; logicalIndex++)
    {
        hwIndex = vTcamInfoPtr->rulePhysicalIndexArr[logicalIndex];

        if(hwIndex >= maxSegmentsInDb)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "DB error : rulesAmount[%d] logicalIndex[%d], hwIndex[%d] >= maxSegmentsInDb[%d]",
                vTcamInfoPtr->rulesAmount,logicalIndex,hwIndex,maxSegmentsInDb);
        }

        if(oldOrNew == GT_TRUE)
        {
            prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].oldLogicalIndex = logicalIndex;
        }
        else
        {
            prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].newLogicalIndex = logicalIndex;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* resizeMoveLogicalIndexes
*
* DESCRIPTION:
*       (for resize purposes) move the 'old' logical index rules to 'new'
*       logical index.
*       NOTE: using prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[]
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamInfoPtr      - (pointer to) virtual TCAM structure
*       resizeValue       -
*                           value > 0 --> resize to bigger size
*       resizeMode        - resize mode (with/without index)
*       resizeReferenceLogicalIndex - resize reference logical index
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_CPU_MEM        - on Cpu memory allocation failed.
*       GT_BAD_STATE             - on DB management error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS resizeMoveLogicalIndexes(
    IN  GT_U32                              vTcamMngId,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr,
    IN  GT_U32                                      resizeValue,
    IN  RESIZE_MODE_ENT                             resizeMode,
    IN  GT_U32                                      resizeReferenceLogicalIndex
)
{
    GT_STATUS   rc;
    GT_U32  oldLogicalIndex;
    GT_U32  newSrc_LogicalIndex;
    GT_U32  newTrg_LogicalIndex;
    GT_U32  oldHwIndex,newHwIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    GT_U32  maxSegmentsInDb = vTcamMngDBPtr->tcamSegCfg.maxSegmentsInDb;
    GT_BOOL needBreak = GT_FALSE;
    GT_U32  stepSize = STEP_SIZE_MAC(vTcamInfoPtr->tcamInfo.ruleSize);
    GT_U32  startIndex = 0;
    GT_U32  endIndex = maxSegmentsInDb;
    GT_BOOL needStraightDirection = GT_FALSE;

    if(resizeValue == 0)
    {
        /* the function is not for such cases */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* do revers iterator , to keep lookup order and not to override any existing rule */

    /* loop on the HW indexes provide assurance that the lookup order will not be
       changed because the added segments are */

    for(oldHwIndex = maxSegmentsInDb - stepSize; needBreak == GT_FALSE ; oldHwIndex -= stepSize)
    {
        if(oldHwIndex == 0)
        {
            /* break will come after we will finish handling index 0 */
            needBreak = GT_TRUE;
        }

        /* the 'old' index is now at this index ... so this is SRC */
        newSrc_LogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].newLogicalIndex;

        if(newSrc_LogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS)
        {
            /* prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].newLogicalIndex was not mapped */
            continue;
        }

        /* do not move non-valid rule */
        if(0 == (vTcamInfoPtr->usedRulesBitmapArr[newSrc_LogicalIndex>>5] & (1<<(newSrc_LogicalIndex&0x1f))))
        {
            continue;
        }

        oldLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].oldLogicalIndex;

        if(oldLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS)
        {
            /* prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].oldLogicalIndex was not mapped */
            continue;
        }

        newTrg_LogicalIndex = oldLogicalIndex;

        if((oldLogicalIndex > resizeReferenceLogicalIndex) ||
           ((oldLogicalIndex == resizeReferenceLogicalIndex) && resizeMode == RESIZE_MODE_WITH_INPUT_INDEX_E))
        {
            /* the TRG index need to be according to the resizeValue with offset from 'old' index*/
            newTrg_LogicalIndex = oldLogicalIndex + resizeValue;
        }
        else
        {
            newTrg_LogicalIndex = oldLogicalIndex;
        }

        if(newSrc_LogicalIndex == newTrg_LogicalIndex)
        {
            /* no move needed */
            continue;
        }

        if(newSrc_LogicalIndex > newTrg_LogicalIndex)
        {
            /* all next moves need straight iterator (not revers iterator) */
            endIndex = oldHwIndex + stepSize;
            needStraightDirection = GT_TRUE;
            break;
        }

        newHwIndex = vTcamInfoPtr->rulePhysicalIndexArr[newTrg_LogicalIndex];

        /* move the index in HW and in DB */
        rc = ruleMoveByLogicalIndex(vTcamMngId,vTcamInfoPtr,
            newSrc_LogicalIndex,oldHwIndex,
            newTrg_LogicalIndex,newHwIndex,
            GT_TRUE/*calledFromResize*/);

        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(needStraightDirection == GT_FALSE)
    {
        /* no more needed */
        return GT_OK;
    }

    for(oldHwIndex = startIndex; oldHwIndex < endIndex ; oldHwIndex += stepSize)
    {
        /* the 'old' index is now at this index ... so this is SRC */
        newSrc_LogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].newLogicalIndex;

        if(newSrc_LogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS)
        {
            /* prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].newLogicalIndex was not mapped */
            continue;
        }

        /* do not move non-valid rule */
        if(0 == (vTcamInfoPtr->usedRulesBitmapArr[newSrc_LogicalIndex>>5] & (1<<(newSrc_LogicalIndex&0x1f))))
        {
            continue;
        }

        oldLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].oldLogicalIndex;

        if(oldLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS)
        {
            /* prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[oldHwIndex].oldLogicalIndex was not mapped */
            continue;
        }

        newTrg_LogicalIndex = oldLogicalIndex;

        if((oldLogicalIndex > resizeReferenceLogicalIndex) ||
           ((oldLogicalIndex == resizeReferenceLogicalIndex) && resizeMode == RESIZE_MODE_WITH_INPUT_INDEX_E))
        {
            /* the TRG index need to be according to the resizeValue with offset from 'old' index*/
            newTrg_LogicalIndex = oldLogicalIndex + resizeValue;
        }
        else
        {
            newTrg_LogicalIndex = oldLogicalIndex;
        }

        if(newSrc_LogicalIndex == newTrg_LogicalIndex)
        {
            /* no move needed */
            continue;
        }

        if(newSrc_LogicalIndex < newTrg_LogicalIndex)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,
                "DB error: algorithm error : unexpected 'opposite' direction.newSrc_LogicalIndex[%d] < newTrg_LogicalIndex[%d] \n",
                newSrc_LogicalIndex , newTrg_LogicalIndex);
        }

        newHwIndex = vTcamInfoPtr->rulePhysicalIndexArr[newTrg_LogicalIndex];

        /* move the index in HW and in DB */
        rc = ruleMoveByLogicalIndex(vTcamMngId,vTcamInfoPtr,
            newSrc_LogicalIndex,oldHwIndex,
            newTrg_LogicalIndex,newHwIndex,
            GT_TRUE/*calledFromResize*/);

        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* priorityResizeUpdateTrees
*
* DESCRIPTION:
*       (for resize purposes) for priority mode :
*       update the content of next trees that hold logical indexes values:
*       vTcamInfoPtr->ruleIdIndexTree,vTcamInfoPtr->priorityIndexTree
*       it will use the DB of prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[]
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr     - (pointer to)virtual TCAM manager structure
*       vTcamInfoPtr      - (pointer to) virtual TCAM structure
*       resizeValue       -
*                           value > 0 --> resize to bigger size
*                           value < 0 --> resize to smaller size
*       resizeMode        - resize mode (with/without index)
*       resizeReferenceLogicalIndex - resize reference logical index
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_STATE             - on DB management error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityResizeUpdateTrees(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr,
    IN  GT_32                                       resizeValue,
    IN  RESIZE_MODE_ENT                             resizeMode,
    IN  GT_U32                                      resizeReferenceLogicalIndex
)
{
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    PRV_CPSS_AVL_TREE_SEEK_ENT seekMode;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC *ruleIdEntryPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC *priorityEntryPtr;
    GT_U32  hwIndex;
    GT_BOOL needBreak;
    GT_U32  oldLogicalIndex,newLogicalIndex,treeLogicalIndex;
    GT_BOOL foundMatch;
    GT_U32  maxSegmentsInDb = vTcamMngDBPtr->tcamSegCfg.maxSegmentsInDb;
    GT_U32  startIndex,endIndex;
    GT_U32  stepSize = STEP_SIZE_MAC(vTcamInfoPtr->tcamInfo.ruleSize);

    if(resizeValue > 0)
    {
        /* do reverse scan on the vTcamInfoPtr->ruleIdIndexTree */
        seekMode = PRV_CPSS_AVL_TREE_SEEK_LAST_E;
        startIndex = maxSegmentsInDb - stepSize;

        endIndex = 0;
    }
    else
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_FIRST_E;
        startIndex = 0;
        endIndex = maxSegmentsInDb - stepSize;
    }

    hwIndex = startIndex;
    needBreak = GT_FALSE;

    while(GT_TRUE ==
        prvCpssAvlPathSeek(vTcamInfoPtr->ruleIdIndexTree,
        seekMode,
        avlTreePath,
        &dbEntryPtr /* use dedicated var to avoid warnings */))
    {
        if(resizeValue > 0)
        {
            seekMode = PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E;
        }
        else
        {
            seekMode = PRV_CPSS_AVL_TREE_SEEK_NEXT_E;
        }
        ruleIdEntryPtr = dbEntryPtr;

        treeLogicalIndex = ruleIdEntryPtr->logicalIndex;

        foundMatch = GT_FALSE;
        /* update the logicalIndex */
        for(/* already initialized*/; needBreak == GT_FALSE ; /*no iteration change here */)
        {
            if(hwIndex == endIndex)
            {
                /* break will come after we will finish handling index 0 */
                needBreak = GT_TRUE;
            }

            oldLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].oldLogicalIndex;
            newLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].newLogicalIndex;

            if(oldLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS ||
               newLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS )
            {
                goto ruleTree_modifyIterator_lbl;/* instead of continue */
            }

            if(oldLogicalIndex == treeLogicalIndex)
            {
                foundMatch = GT_TRUE;
                /* found match ... update the tree with newLogicalIndex */
                ruleIdEntryPtr->logicalIndex = newLogicalIndex;
                break;
            }
ruleTree_modifyIterator_lbl:
            if(resizeValue > 0)
            {
                hwIndex -= stepSize;
            }
            else
            {
                hwIndex += stepSize;
            }
        }

        if(foundMatch == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: mismatch between prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[] and vTcamInfoPtr->ruleIdIndexTree[] \n");
        }
    }

    if(resizeValue < 0)
    {
        /* can not handle here the modifications in vTcamInfoPtr->priorityIndexTree */
        return GT_OK;
    }

    /* do reverse scan on the vTcamInfoPtr->ruleIdIndexTree */
    seekMode = PRV_CPSS_AVL_TREE_SEEK_LAST_E;
    startIndex = maxSegmentsInDb - stepSize;

    hwIndex = startIndex;
    needBreak = GT_FALSE;

    while(GT_TRUE ==
        prvCpssAvlPathSeek(vTcamInfoPtr->priorityIndexTree,
        seekMode,
        avlTreePath,
        &dbEntryPtr /* use dedicated var to avoid warnings */))
    {
        seekMode = PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E;

        priorityEntryPtr = dbEntryPtr;

        treeLogicalIndex = priorityEntryPtr->baseLogIndex;
        foundMatch = GT_FALSE;
        /* update the logicalIndex */
        for(/* already initialized*/; needBreak == GT_FALSE ; hwIndex -= stepSize)
        {
            if(hwIndex == 0)
            {
                /* break will come after we will finish handling index 0 */
                needBreak = GT_TRUE;
            }

            oldLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].oldLogicalIndex;
            newLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].newLogicalIndex;

            if(oldLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS ||
               newLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS )
            {
                continue;
            }

            if(oldLogicalIndex == treeLogicalIndex)
            {
                foundMatch = GT_TRUE;

                if((oldLogicalIndex > resizeReferenceLogicalIndex) ||
                   ((oldLogicalIndex == resizeReferenceLogicalIndex) && resizeMode == RESIZE_MODE_WITH_INPUT_INDEX_E))
                {
                    /* the TRG index need to be according to the resizeValue with offset from 'old' index*/
                    newLogicalIndex = oldLogicalIndex + resizeValue;
                }
                else
                {
                    newLogicalIndex = oldLogicalIndex;
                }

                /* found match ... update the tree with newLogicalIndex */
                priorityEntryPtr->baseLogIndex = newLogicalIndex;
                break;
            }
        }

        if(foundMatch == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: mismatch between prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[] and vTcamInfoPtr->priorityIndexTree[] \n");
        }

    }

    return GT_OK;
}
/*******************************************************************************
* resizeUpdateLogicalIndexBmp
*
* DESCRIPTION:
*       (for resize purposes)
*       update the content of next BMP that hold logical indexes values :
*       vTcamInfoPtr->usedRulesBitmapArr
*       it will use the DB of prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[]
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr     - (pointer to)virtual TCAM manager structure
*       vTcamInfoPtr      - (pointer to) virtual TCAM structure
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_STATE             - on DB management error
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS resizeUpdateLogicalIndexBmp(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr
)
{
    GT_U32  hwIndex;
    GT_U32  maxSegmentsInDb = vTcamMngDBPtr->tcamSegCfg.maxSegmentsInDb;
    GT_BOOL needBreak = GT_FALSE;
    GT_U32  bmpSize ;
    GT_U32  *old_usedRulesBitmapArr = vTcamInfoPtr->usedRulesBitmapArr;
    GT_U32  oldLogicalIndex,newLogicalIndex;
    GT_U32  stepSize = STEP_SIZE_MAC(vTcamInfoPtr->tcamInfo.ruleSize);

    bmpSize =
        (((vTcamInfoPtr->rulesAmount + 31) / 32)
          * sizeof(GT_U32));

    if (bmpSize == 0)
    {
        if (vTcamInfoPtr->usedRulesBitmapArr != NULL)
        {
            /* the new state is null-sized */
            cpssOsFree(vTcamInfoPtr->usedRulesBitmapArr);
            vTcamInfoPtr->usedRulesBitmapArr = NULL;
            return GT_OK;
        }
    }

    /* malloc new usedRulesBitmapArr[] */
    vTcamInfoPtr->usedRulesBitmapArr = (GT_U32*)cpssOsMalloc(bmpSize);
    if (vTcamInfoPtr->usedRulesBitmapArr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    /* memset to 0 the BMP */
    cpssOsMemSet(vTcamInfoPtr->usedRulesBitmapArr,0,bmpSize);

    if (old_usedRulesBitmapArr == NULL)
    {
        /* an old state was null-sized */
        return GT_OK;
    }

    for(hwIndex = maxSegmentsInDb - stepSize; needBreak == GT_FALSE ; hwIndex -= stepSize)
    {
        if(hwIndex == 0)
        {
            /* break will come after we will finish handling index 0 */
            needBreak = GT_TRUE;
        }

        oldLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].oldLogicalIndex;
        newLogicalIndex = prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[hwIndex].newLogicalIndex;

        if(oldLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS ||
           newLogicalIndex == PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_HW_INDEX_NOT_USED_CNS )
        {
            continue;
        }

        if(old_usedRulesBitmapArr[oldLogicalIndex>>5]&(1<<(oldLogicalIndex&0x1f)))
        {
            /* the oldLogicalIndex was set in old_usedRulesBitmapArr[] */
            /* so need set bit newLogicalIndex in usedRulesBitmapArr */

            vTcamInfoPtr->usedRulesBitmapArr[newLogicalIndex>>5] |= (1<<(newLogicalIndex&0x1f));
        }
    }

    /* free the old array ... as it is not relevant any more */
    cpssOsFree(old_usedRulesBitmapArr);

    return GT_OK;
}


/*******************************************************************************
* vtcamDbSegmentTableVTcamAllocOrResize
*
* DESCRIPTION:
*       Allocate/resize memory for Virtual TCAM in Segment Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr     - (pointer to) TCAM Manager info
*       vTcamId           - unique Id of  Virtual TCAM
*       resizeValue       - value == 0 --> no resize (initial alloc)
*                           value > 0 --> resize to bigger size
*       alloc_vTcamInfoPtr - pointer to vtcam that temporary gets segments for the resize operation
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS vtcamDbSegmentTableVTcamAllocOrResize
(
    IN  PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr,
    IN  GT_U32                                      vTcamId,
    IN  GT_U32                                      resizeValue,
    INOUT  PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *alloc_vTcamInfoPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_SEG_TAB_ALLOC_REQUEST_STC    allocRequest; /* request*/

    /* get vtcam DB */
    vTcamInfoPtr  = vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId];

    allocRequest.lookupId        =
        PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_LOOKUP_ID_MAC(
            vTcamInfoPtr->tcamInfo.clientGroup, vTcamInfoPtr->tcamInfo.hitNumber);
    allocRequest.vTcamId         = vTcamId;
    allocRequest.rulesAmount     = vTcamInfoPtr->tcamInfo.guaranteedNumOfRules;
    switch (vTcamInfoPtr->tcamInfo.ruleSize)
    {
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E:
            allocRequest.ruleSize =     1;
            allocRequest.ruleAlignment = 1;
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E:
            allocRequest.ruleSize =     2;
            allocRequest.ruleAlignment = 2;
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E:
            allocRequest.ruleSize =     3;
            allocRequest.ruleAlignment = 3;
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E:
            allocRequest.ruleSize =     4;
            allocRequest.ruleAlignment = 6;
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E:
            allocRequest.ruleSize =     5;
            allocRequest.ruleAlignment = 6;
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E:
            allocRequest.ruleSize =     6;
            allocRequest.ruleAlignment = 6;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(resizeValue == 0)
    {
        alloc_vTcamInfoPtr = vTcamInfoPtr;
    }
    else /*if(resizeValue > 0)*/
    {
        /* set proper needed size */
        allocRequest.rulesAmount = resizeValue;
    }

    /*Allocate memory for Virtual TCAM in Segment Table for TCAM.*/
    rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocate(
        vTcamMngDBPtr,  alloc_vTcamInfoPtr, &allocRequest);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* vtcamDbSegmentTableVTcamDownSize__LogicalIndexMode
*
* DESCRIPTION:
*       logical Index Mode :
*       down size memory for Virtual TCAM in Segment Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       downSizeValue       -
*                           value > 0 --> down size to smaller size
*       downSizeReferenceLogicalIndex - down size reference logical index
*                           relevant only when downSizeValue != 0
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS vtcamDbSegmentTableVTcamDownSize__LogicalIndexMode
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  GT_U32                                      downSizeValue,
    IN  GT_U32                                      downSizeReferenceLogicalIndex
)
{
    GT_STATUS   rc;
    GT_U32  ii,iiMax;
    GT_U32  src_logicalIndex,srcDeviceRuleIndex;
    GT_U32  dst_logicalIndex,trgDeviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr;
    GT_U32   srcExists;
    GT_U32   dstExists;

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    vTcamInfoPtr  = vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId];

    /* the vTcamInfoPtr->usedRulesAmount should not be changed during the operation */

    if((downSizeReferenceLogicalIndex + downSizeValue) > vTcamInfoPtr->rulesAmount)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    iiMax = vTcamInfoPtr->rulesAmount - downSizeValue;
    ii = downSizeReferenceLogicalIndex;

    /* invalidate existing rules at the decreased area,
     *   while moving higher numbered rules to fill the gap */
    for(/*already initialized*/ ; ii < iiMax ;ii++)
    {
        src_logicalIndex = ii + downSizeValue;
        dst_logicalIndex = ii;

        srcDeviceRuleIndex = vTcamInfoPtr->rulePhysicalIndexArr[src_logicalIndex];
        trgDeviceRuleIndex = vTcamInfoPtr->rulePhysicalIndexArr[dst_logicalIndex];

        /* check if 'src' was used index */
        srcExists = (vTcamInfoPtr->usedRulesBitmapArr[src_logicalIndex>>5] & (1<<(src_logicalIndex & 0x1f)));
        /* check if destination exists in DB */
        dstExists = (vTcamInfoPtr->usedRulesBitmapArr[dst_logicalIndex>>5] & (1<<(dst_logicalIndex & 0x1f)));

        if(dstExists)
        {
            if(vTcamInfoPtr->usedRulesAmount == 0)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "DB error: usedRulesAmount : can not decrement from ZERO ");
            }

            /* remove the index in HW and in DB */
            /* Note: this is done also in case source exists, since move
             *  operation below can't update rule+action atomically, possibly
             *  leading to wrong results on traffic */
            rc = cpssDxChVirtualTcamRuleDelete(vTcamMngId,vTcamId, dst_logicalIndex);

            if(rc != GT_OK)
            {
                return rc;
            }
        }
        if(srcExists)
        {
            /* move the index in HW and in DB */
            rc = ruleMoveByLogicalIndex(vTcamMngId,vTcamInfoPtr,
                src_logicalIndex,srcDeviceRuleIndex,
                dst_logicalIndex,trgDeviceRuleIndex,
                GT_TRUE/*calledFromResize*/);

            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* continue invalidating existing rules until the end of the decreased area,
     *   in case moving rules  didn't cover all of the decreased area */
    iiMax = downSizeReferenceLogicalIndex + downSizeValue;
    for(/*already initialized*/ ; ii < iiMax ;ii++)
    {
        dst_logicalIndex = ii;

        trgDeviceRuleIndex = vTcamInfoPtr->rulePhysicalIndexArr[dst_logicalIndex];

        /* check is destination exists in DB */
        dstExists = (vTcamInfoPtr->usedRulesBitmapArr[dst_logicalIndex>>5] & (1<<(dst_logicalIndex & 0x1f)));

        if(dstExists)
        {
            /* remove the index in HW and in DB */
            rc = cpssDxChVirtualTcamRuleDelete(vTcamMngId,vTcamId, dst_logicalIndex);

            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* after the entries not occupy the last indices any more , we need to
       restore them into the 'free' segments

       the function will update vTcamInfoPtr->rulesAmount
    */
    rc = prvCpssDxChVirtualTcamDbSegmentTableDetachSpace(vTcamMngDBPtr,
        vTcamInfoPtr,vTcamId,
        GT_TRUE,/*from the end*/
        vTcamInfoPtr->rulesAmount - downSizeValue);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    if(vTcamInfoPtr->rulesAmount < vTcamInfoPtr->tcamInfo.guaranteedNumOfRules)
    {
        vTcamInfoPtr->tcamInfo.guaranteedNumOfRules = vTcamInfoPtr->rulesAmount;
    }

    return GT_OK;
}

/*******************************************************************************
* dbOperationRemoveEmptyRangeOfLogicalIndexes
*
* DESCRIPTION:
*       DB operation :
*       removing the start/end <downSizeValue> logical indexes , and move
*       the rest of them to the 'other side'.
*
*       NOTE: the range of those removed indexes is expected to be 'unused' by
*           valid rules
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngDBPtr     - (pointer to)virtual TCAM manager structure
*       vtcamInfoPtr    - (pointer to)virtual TCAM structure
*       fromEndOrStart  - the type of direction
*                       GT_TRUE  - hole from end   of range
*                       GT_FALSE - hole from start of range
*       downSizeValue   - the size of the unused indexes
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_CPU_MEM        - on Cpu memory allocation failed.
*       GT_BAD_STATE             - on DB management error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS dbOperationRemoveEmptyRangeOfLogicalIndexes
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC                        *vTcamMngDBPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC               *vTcamInfoPtr,
    IN   GT_BOOL                                                    fromEndOrStart,
    IN   GT_U32                                                     downSizeValue
)
{
    GT_STATUS   rc;
    GT_U32 logicalIndex;
    GT_U32  orig_rulesAmount = vTcamInfoPtr->rulesAmount;

    if(vTcamInfoPtr->rulesAmount < downSizeValue)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if(fromEndOrStart == GT_FALSE)
    {
        /* map HW index to logical indexes of the 'old' logical index (before the change) */
        rc = resizeMapHwIndexesToLogicalIndexes(vTcamMngDBPtr,vTcamInfoPtr,GT_TRUE/*old*/);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        /* update vTcamInfoPtr->rulePhysicalIndexArr[] */
        for(logicalIndex = 0;
            logicalIndex < (vTcamInfoPtr->rulesAmount - downSizeValue);
            logicalIndex++)
        {
            /* move the indexes */
            vTcamInfoPtr->rulePhysicalIndexArr[logicalIndex] =
                vTcamInfoPtr->rulePhysicalIndexArr[logicalIndex + downSizeValue];
        }

        /* update vTcamInfoPtr->usedRulesBitmapArr[] */
        copyBitsInMemory(vTcamInfoPtr->usedRulesBitmapArr,
            0,/*targetStartBit*/
            downSizeValue,/*sourceStartBit*/
            orig_rulesAmount - downSizeValue);/*numBits*/

        /* NO need to update usedRulesAmount because the range of those removed
          indexes is expected to be 'unused' by valid rules */

        /* update vTcamInfoPtr->rulesAmount (needed for resizeMapHwIndexesToLogicalIndexes)*/
        vTcamInfoPtr->rulesAmount -= downSizeValue;

        /* map HW index to logical indexes of the 'new' logical index (after the change) */
        rc = resizeMapHwIndexesToLogicalIndexes(vTcamMngDBPtr,vTcamInfoPtr,GT_FALSE/*new*/);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        if(vTcamInfoPtr->tcamInfo.ruleAdditionMethod ==
            CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
        {
            /* update the content of next trees that hold logical indexes values:
                vTcamInfoPtr->ruleIdIndexTree,vTcamInfoPtr->priorityIndexTree
                it will use the DB of prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[]
            */
            rc = priorityResizeUpdateTrees(vTcamMngDBPtr,vTcamInfoPtr,
                (GT_32)(0-downSizeValue),RESIZE_MODE_WITH_INPUT_INDEX_E,0/*start index*/);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }
        }
    }
    else
    {
        /* no need to update vTcamInfoPtr->rulePhysicalIndexArr[] because last indexes will be ignored */
        /* no need to update vTcamInfoPtr->usedRulesBitmapArr[] because last indexes will be ignored */

        /* NO need to update usedRulesAmount because the range of those removed
          indexes is expected to be 'unused' by valid rules */

        /* update vTcamInfoPtr->rulesAmount (needed for resizeMapHwIndexesToLogicalIndexes)*/
        vTcamInfoPtr->rulesAmount -= downSizeValue;
    }

    /* no need to call resizeUpdateLogicalIndexBmp(...) because we already updated
        rulePhysicalIndexArr[]     */

    if(vTcamInfoPtr->rulesAmount < vTcamInfoPtr->usedRulesAmount)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

#define MAX_LL_ELEM_NUM _64K
static GT_COMP_RES myLlCompare
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC *data1,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC *data2
)
{
    /* Check the null cases     */
    if(data1->priority == data2->priority)
        return GT_EQUAL;

    if (data1->priority > data2->priority)
       return GT_GREATER;

    return GT_SMALLER;
}
static GT_VOID  *llHeadPtr = NULL;
static GT_STATUS linkListInit(void)
{
    if(llHeadPtr == NULL)
    {
        llHeadPtr = prvCpssSlInit((FUNCP_SL_COMP)myLlCompare, MAX_LL_ELEM_NUM);
        if(llHeadPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/* dummy node data free function for Skip List */
static void linkListDataDummyFreeFunc(GT_VOID *p) {p = p;}

static GT_STATUS linkListDestroy(void)
{
    GT_STATUS rc;
    if (llHeadPtr != NULL)
    {
        rc = prvCpssSlDestroy(
            llHeadPtr, (GT_VOIDFUNCPTR)linkListDataDummyFreeFunc);
        llHeadPtr = NULL;
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

static GT_STATUS linkListAdd(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC               *pVTcamMngPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC *ptr
)
{
    GT_UINTPTR iteratorId;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC *priorityEntryPtr;
    /* get free buffer from priority pool.*/
    priorityEntryPtr = cpssBmPoolBufGet(pVTcamMngPtr->priorityPool);
    if (priorityEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL,"cpssBmPoolBufGet: error getting free buffer from pool");
    }

    *priorityEntryPtr = *ptr;

    return prvCpssSlAdd(llHeadPtr,priorityEntryPtr,&iteratorId);
}


static GT_STATUS priorityTreeDownSizeRangeInfoGet(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr,
    IN GT_U32                                       oldLogicalIndex,
    OUT PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC **currentPriorityEntryPtrPtr
)
{
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    needToFindInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    *foundPriorityEntryPtr;
    GT_BOOL found;

    needToFindInfo.priority = 0;/* don't care */
    needToFindInfo.baseLogIndex = oldLogicalIndex;
    needToFindInfo.rangeSize = 0;/* don't care */

    found = prvCpssAvlPathFind(vTcamInfoPtr->priorityIndexTree,
            PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,
            &needToFindInfo,/*pData*/
            avlTreePath,/*path*/
            &dbEntryPtr /* use dedicated var to avoid warnings */);
    if (found == GT_FALSE)
    {
        found = prvCpssAvlPathSeek(vTcamInfoPtr->priorityPriTree,
                    PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
                    avlTreePath,
                    &dbEntryPtr /* use dedicated var to avoid warnings */);
        if (found == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
    }

    foundPriorityEntryPtr = dbEntryPtr;

    if(oldLogicalIndex >= (foundPriorityEntryPtr->baseLogIndex + foundPriorityEntryPtr->rangeSize))
    {
        found = prvCpssAvlPathSeek(vTcamInfoPtr->priorityPriTree,
                    PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                    avlTreePath,
                    &dbEntryPtr /* use dedicated var to avoid warnings */);
        if(found == GT_FALSE)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
        }
    }

    *currentPriorityEntryPtrPtr = dbEntryPtr;

    return GT_OK;
}

static GT_STATUS priorityTreeDownSizeRangeUpdate_fromLinkList(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC               *pVTcamMngPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr
)
{
    GT_VOID  *pDelData;
    GT_STATUS   rc;
    GT_UINTPTR  iterator;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC *priorityEntryPtr;

    while((pDelData = prvCpssSlGetFirst(llHeadPtr,NULL,&iterator)) != NULL)
    {
        priorityEntryPtr = pDelData;

        /* update the trees of the priority */
        rc = prvCpssDxChVirtualTcamDbPriorityTableEntrySet(
            pVTcamMngPtr,vTcamInfoPtr,priorityEntryPtr);
        if(rc != GT_OK)
        {
            return rc;
        }

        pDelData = prvCpssSlDelete(llHeadPtr,pDelData);
        /* free a buffer back to its pool.*/
        rc = cpssBmPoolBufFree(pVTcamMngPtr->priorityPool, pDelData);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* priorityTreeDownSizeRangeUpdate
*
* DESCRIPTION:
*       (for resize purposes of down size)
*       move the 'old' logical index rules to 'new' logical index.
*       according to needed amount of moves
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamInfoPtr      - (pointer to) virtual TCAM structure
*       fromEndOrStart  - the type of direction
*                       GT_TRUE  - hole from end   of range
*                       GT_FALSE - hole from start of range
*       downSizeValue       -
*                           value > 0 --> down size to smaller size
*       firstTargetLogicalIndex - the first logical index to move used indexes to it
* OUTPUTS:
*       lastIndexMovedPtr - (pointer to) the last index that was used.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_CPU_MEM        - on Cpu memory allocation failed.
*       GT_BAD_STATE             - on DB management error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityTreeDownSizeRangeUpdate (
    IN GT_U32                                       vTcamMngId,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr,
    IN   GT_BOOL                                    fromEndOrStart,
    IN  GT_U32                                      downSizeValue,
    IN  GT_U32                                      firstTargetLogicalIndex
)
{
    GT_STATUS   rc;
    GT_U32  ii,iiStart,iiLast;
    GT_U32  *arrPtr = vTcamInfoPtr->usedRulesBitmapArr;
    GT_U32  value;
    GT_U32  numOfHolesInRange = 0;/* number of holes in 'moved' range */
    GT_U32  kk,kkMax;
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC priorityEntry;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC *currentPriorityEntryPtr = NULL;
    GT_U32  lastIndexMoved = 0;
    GT_BOOL isOk = GT_FALSE;
    GT_U32  countHolesGlobal,countHolesInPriority;
    GT_U32  nextBase;
    GT_U32  currPriority = 0;

    rc = linkListInit();
    if(rc != GT_OK)
    {
        return rc;
    }

    if(fromEndOrStart == GT_TRUE)
    {
        /* start from the end : direction of move is 'down' (from high to low index) */
        iiStart = firstTargetLogicalIndex;
        iiLast  = vTcamInfoPtr->rulesAmount;

        /* find rules that need the move */
        for(ii = iiStart ; ii < iiLast ; ii++)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value == 0)
            {
                numOfHolesInRange ++;
                if(numOfHolesInRange == downSizeValue)
                {
                    lastIndexMoved = ii;
                    isOk = GT_TRUE;
                    break;
                }
            }
        }
    }
    else
    {
        /* start from the start : direction of move is 'up' (from low to high index) */
        iiStart = firstTargetLogicalIndex;

        /* find rules that need the move */
        for(ii = iiStart ; /*no criteria*/ ; ii--)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value == 0)
            {
                numOfHolesInRange ++;
                if(numOfHolesInRange == downSizeValue)
                {
                    lastIndexMoved = ii;
                    isOk = GT_TRUE;
                    break;
                }
            }

            if(ii == 0)
            {
                break;
            }
        }
    }

    if(isOk == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* so we got 'lastIndexMoved' */

    if(firstTargetLogicalIndex > lastIndexMoved)
    {
        GT_U32  tmp = lastIndexMoved;
        lastIndexMoved = firstTargetLogicalIndex;
        firstTargetLogicalIndex = tmp;
    }

    priorityEntry.priority = 0;/* don't care */
    priorityEntry.rangeSize = 0;    /* don't care */
    nextBase = 0xFFFFFFFF;

    countHolesGlobal = 0;
    for(ii = firstTargetLogicalIndex ; ii <= lastIndexMoved ; ii++)
    {
        /* update the priority tree */
        rc = priorityTreeDownSizeRangeInfoGet(vTcamInfoPtr,
            ii,
            &currentPriorityEntryPtr);
        if (rc != GT_OK)
        {
            if (rc == GT_NOT_FOUND)
            {
                /* all ranges of priority contained rules passed */
                break;
            }
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        currPriority = currentPriorityEntryPtr->priority;

        if(ii < currentPriorityEntryPtr->baseLogIndex)
        {
            /* we got hole that need to be filled */
            /* the current priority need to come after previous one */
            /* jump over the hole */
            countHolesInPriority = MIN((lastIndexMoved+1),currentPriorityEntryPtr->baseLogIndex) - ii;
            countHolesGlobal += countHolesInPriority;
            ii += countHolesInPriority;

            if(ii > lastIndexMoved)
            {
                /* this priority range starts after the end of the section */
                /* state that is moved according to the end of previous section */

                priorityEntry.priority = currentPriorityEntryPtr->priority;
                priorityEntry.rangeSize = currentPriorityEntryPtr->rangeSize;

                if(currentPriorityEntryPtr->baseLogIndex < countHolesGlobal)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                /* this priority base address shifted to close the holes that accumulated */
                priorityEntry.baseLogIndex = currentPriorityEntryPtr->baseLogIndex - countHolesGlobal;

                rc = linkListAdd(vTcamMngDB[vTcamMngId],&priorityEntry);
                if(rc != GT_OK)
                {
                    linkListDestroy();
                    return rc;
                }

                break;/* no more */
            }
        }
        else if(ii >= (currentPriorityEntryPtr->baseLogIndex + currentPriorityEntryPtr->rangeSize))
        {
            /* how ii can't be is valid range but not reached (lastIndexMoved+1) ?! */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        kkMax = MIN((lastIndexMoved+1),(currentPriorityEntryPtr->baseLogIndex + currentPriorityEntryPtr->rangeSize));
        countHolesInPriority = 0;
        /* start counting number of holes from this point */
        for(kk = ii ; kk < kkMax; kk++)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, kk,1,value);
            if(value == 0)
            {
                countHolesInPriority++;
            }
        }


        priorityEntry.priority = currentPriorityEntryPtr->priority;
        if(currentPriorityEntryPtr->rangeSize < countHolesInPriority)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
        priorityEntry.rangeSize = currentPriorityEntryPtr->rangeSize - countHolesInPriority;

        if(nextBase == 0xFFFFFFFF)
        {
            if (firstTargetLogicalIndex < currentPriorityEntryPtr->baseLogIndex)
            {
                priorityEntry.baseLogIndex = firstTargetLogicalIndex;
            }
            else
            {
                priorityEntry.baseLogIndex = currentPriorityEntryPtr->baseLogIndex;
            }
            nextBase = priorityEntry.baseLogIndex + priorityEntry.rangeSize;
        }
        else
        {
            priorityEntry.baseLogIndex = nextBase;

            nextBase += priorityEntry.rangeSize;
        }

        rc = linkListAdd(vTcamMngDB[vTcamMngId],&priorityEntry);
        if(rc != GT_OK)
        {
            linkListDestroy();
            return rc;
        }

        countHolesGlobal += countHolesInPriority;

        ii = kkMax-1;
    }

    /* keep loop of priorities and update their base */
    priorityEntry.priority = currPriority;/* current prio */
    priorityEntry.baseLogIndex = 0;/* don't care */
    priorityEntry.rangeSize = 0;/* don't care */

    if(GT_TRUE == prvCpssAvlPathFind(vTcamInfoPtr->priorityPriTree,
            PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
            &priorityEntry,/*pData*/
            avlTreePath,/*path*/
            &dbEntryPtr /* use dedicated var to avoid warnings */))
    {
        /* jump to next */
        while(GT_TRUE ==prvCpssAvlPathSeek(vTcamInfoPtr->priorityPriTree,
            PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
            avlTreePath,
            &dbEntryPtr /* use dedicated var to avoid warnings */))
        {
            currentPriorityEntryPtr = dbEntryPtr;

            priorityEntry.priority = currentPriorityEntryPtr->priority;
            priorityEntry.rangeSize = currentPriorityEntryPtr->rangeSize;

            if(currentPriorityEntryPtr->baseLogIndex < countHolesGlobal)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            /* those priority base address shifted to close the holes that accumulated */
            priorityEntry.baseLogIndex = currentPriorityEntryPtr->baseLogIndex - countHolesGlobal;

            rc = linkListAdd(vTcamMngDB[vTcamMngId],&priorityEntry);
            if(rc != GT_OK)
            {
                linkListDestroy();
                return rc;
            }
        }
    }

    /* make updates in vTcamInfoPtr->ruleIdIndexTree */
    rc = priorityTreeDownSizeRangeUpdate_fromLinkList(
        vTcamMngDB[vTcamMngId],vTcamInfoPtr);
    if(rc != GT_OK)
    {
        linkListDestroy();
        return rc;
    }
    linkListDestroy();

    return GT_OK;
}

/*******************************************************************************
* priorityDownSizeMoveLogicalIndexes
*
* DESCRIPTION:
*       (for resize purposes of down size)
*       move the 'old' logical index rules to 'new' logical index.
*       according to needed amount of moves
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamInfoPtr      - (pointer to) virtual TCAM structure
*       fromEndOrStart  - the type of direction
*                       GT_TRUE  - hole from end   of range
*                       GT_FALSE - hole from start of range
*       numOfHwMoves       - number of valid rules to move
*       firstTargetLogicalIndex - the first logical index to move used indexes to it
*       downSizeValue       -
*                           value > 0 --> down size to smaller size
* OUTPUTS:
*       lastIndexMovedPtr - (pointer to) the last index that was used.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_CPU_MEM        - on Cpu memory allocation failed.
*       GT_BAD_STATE             - on DB management error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS priorityDownSizeMoveLogicalIndexes(
    IN GT_U32                                       vTcamMngId,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr,
    IN   GT_BOOL                                    fromEndOrStart,
    IN  GT_U32                                      numOfHwMoves,
    IN  GT_U32                                      firstTargetLogicalIndex,
    IN  GT_U32                                      downSizeValue,
    OUT GT_U32                                      *lastIndexMovedPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii,iiStart,iiLast;
    GT_U32  jj;
    GT_U32  *arrPtr = vTcamInfoPtr->usedRulesBitmapArr;
    GT_U32  value;
    GT_U32  numOfMovedEntries = 0;/* number of entries that we moved in HW */
    GT_U32  src_logicalIndex,srcDeviceRuleIndex;
    GT_U32  dst_logicalIndex,trgDeviceRuleIndex;
    GT_BOOL didMove;

    downSizeValue = downSizeValue;

    if(fromEndOrStart == GT_TRUE)
    {
        /* start from the end : direction of move is 'down' (from high to low index) */
        iiStart = firstTargetLogicalIndex;
        iiLast  = vTcamInfoPtr->rulesAmount;
        jj = iiStart + 1;

        /* find rules that need the move */
        for(ii = iiStart ; ii < iiLast ; ii++)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value)
            {
                /* when ii == iiStart the bit was found as empty by the caller */
                /* for other ii our call to ruleMoveByLogicalIndex(...)
                   should have evacuated this bit already ! */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            didMove = GT_FALSE;
            /* index ii is empty , we need index to occupy it */
            for(/*continue*/ ; (jj < iiLast) && (didMove == GT_FALSE) ; jj++)
            {
                U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, jj,1,value);
                if(value == 0)
                {
                    continue;
                }
                /* index jj is used ... need to move the entry from jj to index ii */
                src_logicalIndex = jj;
                dst_logicalIndex = ii;
                /* convert the logical index to physical index in the HW */
                srcDeviceRuleIndex = vTcamInfoPtr->rulePhysicalIndexArr[src_logicalIndex];
                trgDeviceRuleIndex = vTcamInfoPtr->rulePhysicalIndexArr[dst_logicalIndex];

                rc = ruleMoveByLogicalIndex(vTcamMngId,vTcamInfoPtr,
                    src_logicalIndex,srcDeviceRuleIndex,
                    dst_logicalIndex,trgDeviceRuleIndex,
                    GT_TRUE/*calledFromResize*/);

                if(rc != GT_OK)
                {
                    return rc;
                }

                didMove = GT_TRUE;/* indication to update jj
                                     and break the loop */
            }

            if(didMove == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            numOfMovedEntries++;

            if(numOfMovedEntries == numOfHwMoves)
            {
                /* we are done with the moves */
                *lastIndexMovedPtr = ii;/* the last move was into this index */
                return GT_OK;
            }
        }
    }
    else
    {
        if(firstTargetLogicalIndex == 0)
        {
            /* can't search indexes to move */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* start from the start : direction of move is 'up' (from low to high index) */
        iiStart = firstTargetLogicalIndex;
        jj = iiStart - 1;

        /* find rules that need the move */
        for(ii = iiStart ; /*no criteria*/ ; ii--)
        {
            U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, ii,1,value);
            if(value)
            {
                /* when ii == iiStart the bit was found as empty by the caller */
                /* for other ii our call to ruleMoveByLogicalIndex(...)
                   should have evacuated this bit already ! */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            didMove = GT_FALSE;
            /* index ii is empty , we need index to occupy it */
            for(/*continue*/ ; (didMove == GT_FALSE) ; jj--)
            {
                U32_GET_FIELD_IN_ENTRY_MAC(arrPtr, jj,1,value);
                if(value == 0)
                {
                    if(jj == 0)
                    {
                        break;
                    }

                    continue;
                }
                /* index jj is used ... need to move the entry from jj to index ii */
                src_logicalIndex = jj;
                dst_logicalIndex = ii;
                /* convert the logical index to physical index in the HW */
                srcDeviceRuleIndex = vTcamInfoPtr->rulePhysicalIndexArr[src_logicalIndex];
                trgDeviceRuleIndex = vTcamInfoPtr->rulePhysicalIndexArr[dst_logicalIndex];

                rc = ruleMoveByLogicalIndex(vTcamMngId,vTcamInfoPtr,
                    src_logicalIndex,srcDeviceRuleIndex,
                    dst_logicalIndex,trgDeviceRuleIndex,
                    GT_TRUE/*calledFromResize*/);

                if(rc != GT_OK)
                {
                    return rc;
                }

                didMove = GT_TRUE;/* indication to update jj
                                     and break the loop */

                if(jj == 0)
                {
                    break;
                }
            }

            if(didMove == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            numOfMovedEntries++;

            if(numOfMovedEntries == numOfHwMoves)
            {
                /* we are done with the moves */
                *lastIndexMovedPtr = ii;/* the last move was into this index */
                return GT_OK;
            }

            if(ii == 0)
            {
                break;
            }

        }
    }

    /* not moved number of needed entries */

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
}


/*******************************************************************************
* vtcamDbSegmentTableVTcamDownSize__PriorityMode
*
* DESCRIPTION:
*       priority Mode :
*       down size memory for Virtual TCAM in Segment Table for TCAM.
*
*       NOTE: the operation start by trying to reduce rules from the specified
*               place , but if not enough empty space there ... it will look for
*               empty space at other points for removal.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       downSizeValue       -
*                           value > 0 --> down size to smaller size
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS vtcamDbSegmentTableVTcamDownSize__PriorityMode
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  GT_U32                                      downSizeValue
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr;
    GT_U32  numHwMovesForHoleAtEnd;  /* number of HW moves needed to create hole at the end   of the range */
    GT_U32  numHwMovesForHoleAtStart;/* number of HW moves needed to create hole at the start of the range*/
    GT_U32  targetIndexEndSide,targetIndexStartSide;/* the first index to get moved HW rule */
    GT_U32  targetIndex;
    GT_U32  lastIndexMoved = 0;/* the last logical index that was moved */
    GT_BOOL fromEndOrStart;/* indication that the hole is at the start/end */
    GT_U32  numOfHwMoves;
    GT_U32  origRulesAmount;
    GT_U32  ii;

    /* the vTcamInfoPtr->usedRulesAmount should not be changed during the operation */

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    vTcamInfoPtr  = vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId];
    origRulesAmount = vTcamInfoPtr->rulesAmount;

    if(downSizeValue > vTcamInfoPtr->rulesAmount)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(vTcamInfoPtr->rulesAmount < (vTcamInfoPtr->usedRulesAmount + downSizeValue))
    {
        /* not enough rules to remove (totally ... not only from specific please) */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* make decision : compress 'up' or compress 'down'
       compress 'up'   means that we need to leave first downSizeValue indexes as free.
       compress 'down' means that we need to leave last  downSizeValue indexes as free.
    */
    /* calc 'cost' from the end */
    rc = numHwMovesForHoleCalc(vTcamInfoPtr,
            GT_TRUE/* from the end */,
            downSizeValue, /* size of hole */
            &numHwMovesForHoleAtEnd,
            &targetIndexEndSide);
    if(rc != GT_OK)
    {
        return rc;
    }

    numOfHwMoves = 0;

    if(numHwMovesForHoleAtEnd == 0)
    {
        /* no HW moves needed , and no logical indexes moves needed */
        /* there are no rules that hold used logical indexes in the needed range */
        fromEndOrStart = GT_TRUE;/*from end*/
    }
    else
    {
        /* calc 'cost' from the start */
        rc = numHwMovesForHoleCalc(vTcamInfoPtr,
                GT_FALSE/* from the start */,
                downSizeValue, /* size of hole */
                &numHwMovesForHoleAtStart,
                &targetIndexStartSide);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(numHwMovesForHoleAtStart == 0)
        {
            /* no HW moves needed , only logical indexes moves needed */

            /* there are no rules that hold used logical indexes in the needed range */
            fromEndOrStart = GT_FALSE;/*from start*/
            targetIndex = (downSizeValue - 1);

            /* update the priority ranges */
            rc = priorityTreeDownSizeRangeUpdate(vTcamMngId,
                vTcamInfoPtr,
                fromEndOrStart,
                downSizeValue,
                targetIndex);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        else
        {
            if(numHwMovesForHoleAtEnd <= numHwMovesForHoleAtStart)
            {
                /* making the hole at the end of the range is better (than at the start)*/
                fromEndOrStart = GT_TRUE;/*from end*/
                numOfHwMoves = numHwMovesForHoleAtEnd;
                targetIndex = targetIndexEndSide;
            }
            else
            {
                /* making the hole at the start of the range is better (than at the end)*/
                fromEndOrStart = GT_FALSE;/*from start*/
                numOfHwMoves = numHwMovesForHoleAtStart;
                targetIndex = targetIndexStartSide;
            }

            /* update the priority ranges */
            rc = priorityTreeDownSizeRangeUpdate(vTcamMngId,
                vTcamInfoPtr,
                fromEndOrStart,
                downSizeValue,
                targetIndex);
            if(rc != GT_OK)
            {
                return rc;
            }

            /*
               move the 'old' logical index rules to 'new' logical index.
               according to needed amount of moves
            */
            rc = priorityDownSizeMoveLogicalIndexes(vTcamMngId,
                vTcamInfoPtr,
                fromEndOrStart,
                numOfHwMoves,
                targetIndex,
                downSizeValue,
                &lastIndexMoved);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(fromEndOrStart == GT_TRUE  &&/*from end */
               lastIndexMoved > (vTcamInfoPtr->rulesAmount - downSizeValue))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
            else
            if(fromEndOrStart == GT_FALSE  &&/*from start */
               lastIndexMoved > downSizeValue)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

        }
    }

    /*
      DB operation :
      removing the start/end <holeSize> logical indexes , and move
      the rest of them to the 'other side'.

      NOTE: the range of those removed indexes is expected to be 'unused' by
          valid rules
    */
    rc = dbOperationRemoveEmptyRangeOfLogicalIndexes(vTcamMngDBPtr,
        vTcamInfoPtr,
        fromEndOrStart,
        downSizeValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    vTcamInfoPtr->rulesAmount = origRulesAmount;/* restore value changed
            in dbOperationRemoveEmptyRangeOfLogicalIndexes */

    /* after the entries not occupy the last indexes any more , we need to
       restore them into the 'free' segments

       the function will update vTcamInfoPtr->rulesAmount
    */
    rc = prvCpssDxChVirtualTcamDbSegmentTableDetachSpace(vTcamMngDBPtr,
        vTcamInfoPtr,vTcamId,
        fromEndOrStart,
        vTcamInfoPtr->rulesAmount - downSizeValue);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    if(vTcamInfoPtr->rulesAmount < vTcamInfoPtr->tcamInfo.guaranteedNumOfRules)
    {
        vTcamInfoPtr->tcamInfo.guaranteedNumOfRules = vTcamInfoPtr->rulesAmount;
    }

    if(((origRulesAmount - downSizeValue) < vTcamInfoPtr->rulesAmount) &&
        (fromEndOrStart == GT_FALSE))
    {
        /* we actually released segment smaller than the requested size ...
           due to granularity issues */
        /* so need to clean this extra from usedRulesBitmapArr[] */
        for(ii = (origRulesAmount - downSizeValue) ; ii < vTcamInfoPtr->rulesAmount ; ii++)
        {
            vTcamInfoPtr->usedRulesBitmapArr[ii>>5] &= ~ (1<<(ii & 0x1f));

            /* move the extra indexes that not moved in the original loop */
            vTcamInfoPtr->rulePhysicalIndexArr[ii] =
                vTcamInfoPtr->rulePhysicalIndexArr[ii + downSizeValue];
        }
    }

    return GT_OK;
}

/*******************************************************************************
* vtcamDbSegmentTableVTcamResize
*
* DESCRIPTION:
*       Allocate/resize memory for Virtual TCAM in Segment Table for TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       resizeValue       - value == 0 --> no resize (initial alloc)
*                           value > 0 --> resize to bigger size
*       resizeMode        - resize mode (with/without index)
*                           relevant only when resizeValue != 0
*       resizeReferenceLogicalIndex - resize reference logical index
*                           relevant only when resizeValue != 0
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS vtcamDbSegmentTableVTcamResize
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  GT_U32                                      resizeValue,
    IN  RESIZE_MODE_ENT                             resizeMode,
    IN  GT_U32                                      resizeReferenceLogicalIndex
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vTcamInfoPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC tmp_vTcamInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *alloc_vTcamInfoPtr = &tmp_vTcamInfo;
    GT_U32  actualResizeValue;/*actual resize value (due to granularity issues)*/

    /* get vtcam DB */
    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    vTcamInfoPtr  = vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId];

    /* allocate to temp info */
    cpssOsMemSet(&tmp_vTcamInfo,0,sizeof(tmp_vTcamInfo));

    rc =  vtcamDbSegmentTableVTcamAllocOrResize( vTcamMngDBPtr, vTcamId,
        resizeValue,
        &tmp_vTcamInfo);
    if(rc != GT_OK)
    {
        return rc;
    }
    actualResizeValue = tmp_vTcamInfo.rulesAmount;
    /* function prvCpssDxChVirtualTcamDbSegmentTableVTcamAllocate Filled next in the vtcam :
        alloc_vTcamInfoPtr->segmentsTree
        alloc_vTcamInfoPtr->rulesAmount
        alloc_vTcamInfoPtr->rulePhysicalIndexArr[]

        all those only for the 'added size'
    */

    /* free this memory as it MUST not be used */
    cpssOsFree(alloc_vTcamInfoPtr->rulePhysicalIndexArr);
    alloc_vTcamInfoPtr->rulePhysicalIndexArr = NULL;

    /* map HW index to logical indexes of the 'old' logical index */
    rc = resizeMapHwIndexesToLogicalIndexes(vTcamMngDBPtr,vTcamInfoPtr,GT_TRUE/*old*/);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* update the number of rules needed to be 'guaranteed' */
    vTcamInfoPtr->tcamInfo.guaranteedNumOfRules += resizeValue;
    /* function prvCpssDxChVirtualTcamDbSegmentTableVTcamCombine will update
       vTcamInfoPtr->rulesAmount  */

    /* bind the new segmentsTree from alloc_vTcamInfoPtr into vTcamInfoPtr */
    /* and align the info in the tree of vTcamInfoPtr->segmentsTree */
    rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamCombine(vTcamMngDBPtr,
        vTcamInfoPtr,alloc_vTcamInfoPtr);

    /* remove temporary tree of added segments */
    prvCpssAvlTreeDelete(
        alloc_vTcamInfoPtr->segmentsTree,
        (GT_VOIDFUNCPTR)NULL,
        (GT_VOID*)NULL);

    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* map HW index to logical indexes of the 'new' logical index */
    rc = resizeMapHwIndexesToLogicalIndexes(vTcamMngDBPtr,vTcamInfoPtr,GT_FALSE/*new*/);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    if(vTcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
    {
        /* update the content of next trees that hold logical indexes values:
            vTcamInfoPtr->ruleIdIndexTree,vTcamInfoPtr->priorityIndexTree
            it will use the DB of prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[]
        */
        rc = priorityResizeUpdateTrees(vTcamMngDBPtr,vTcamInfoPtr,
            actualResizeValue,resizeMode,resizeReferenceLogicalIndex);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    /* update the content of next BMP that hold logical indexes values :
        vTcamInfoPtr->usedRulesBitmapArr
        it will use the DB of prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[]
    */
    rc = resizeUpdateLogicalIndexBmp(
        vTcamMngDBPtr,vTcamInfoPtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* move logical indexes to their final position in the DB and in the HW
       with the assurance of the resizeValue
       it will use the DB of prvCpssDxChVirtualTcamDbResizeHwIndexDbArr[]
    */
    rc = resizeMoveLogicalIndexes(vTcamMngId,vTcamInfoPtr,
        actualResizeValue,resizeMode,resizeReferenceLogicalIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    return rc;
}

/*******************************************************************************/
/*******************************************************************************/
/******************** end DB operation *****************************************/
/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************/
/*******************************************************************************/
/****************** start 'per device' functions *******************************/
/*******************************************************************************/
/*******************************************************************************/


/*******************************************************************************
* perDevice_ruleWrite
*
* DESCRIPTION:
*       Write Rule into the TCAM of specific device.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - the device number.
*       ruleIndex     - the rule index in the TCAM. (CPSS API physical index)
*       ruleTypePtr   -     (pointer to)type of contents of rule
*       ruleDataPtr   -     (pointer to)data of contents of rule
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS perDevice_ruleWrite
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
)
{
    GT_STATUS rc;
    CPSS_DXCH_PCL_RULE_OPTION_ENT ruleOptionsBmp;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT ruleSize;  /* Rule size derived from PCL rule format. Used only on xCat3 PCL */
    GT_U32                        rulePclIndex;     /* Rule index, adjusted according to underlying API and rule size */

    switch(ruleTypePtr->ruleType)
    {
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E:
            /* The PCL API supports the creation of rule as 'not valid' */
            if(ruleDataPtr->valid == GT_FALSE)
            {
                ruleOptionsBmp = CPSS_DXCH_PCL_RULE_OPTION_WRITE_INVALID_E;
            }
            else
            {
                ruleOptionsBmp = 0;
            }

            rc = prvCpssDxChVirtualTcamRuleFormatToRuleSize(ruleTypePtr->rule.pcl.ruleFormat,
                                                                &ruleSize);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                                ruleSize,
                                                                ruleIndex,
                                                                &rulePclIndex);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssDxChPclRuleSet(devNum,
                ruleTypePtr->rule.pcl.ruleFormat,
                rulePclIndex,
                ruleOptionsBmp,
                ruleDataPtr->rule.pcl.maskPtr,
                ruleDataPtr->rule.pcl.patternPtr,
                ruleDataPtr->rule.pcl.actionPtr
                );
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E:
            rc = cpssDxChTtiRuleSet(devNum,
                ruleIndex,
                ruleTypePtr->rule.tti.ruleFormat,
                ruleDataPtr->rule.tti.patternPtr,
                ruleDataPtr->rule.tti.maskPtr,
                CPSS_DXCH_TTI_ACTION_TYPE2_ENT,
                ruleDataPtr->rule.tti.actionPtr
                );
            if(rc != GT_OK)
            {
                return rc;
            }

            /* The TTI API NOT supports the creation of rule as 'not valid' */
            /* so we invalidate the rule after it's creation */
            if(ruleDataPtr->valid == GT_FALSE)
            {
                rc = cpssDxChTtiRuleValidStatusSet(devNum,
                    ruleIndex,
                    GT_FALSE);
            }

            break;
        default:
            rc = GT_BAD_PARAM;
            break;
    }

    return rc;

}

/*******************************************************************************
* perDevice_ruleRead
*
* DESCRIPTION:
*       Read Rule from the TCAM of specific device.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - the device number.
*       ruleIndex     - the rule index in the TCAM. (CPSS API physical index)
*       ruleTypePtr   -     (pointer to)type of contents of rule
*
* OUTPUTS:
*       ruleDataPtr   -     (pointer to)data of contents of rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS perDevice_ruleRead
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
)
{
    GT_STATUS rc;
    GT_U32                        rulePclIndex;     /* Rule index, adjusted according to underlying API and rule size */
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT ruleSize;  /* Rule size derived from PCL rule format. Used only on xCat3 PCL */

    switch(ruleTypePtr->ruleType)
    {
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E:

            rc = prvCpssDxChVirtualTcamRuleFormatToRuleSize(ruleTypePtr->rule.pcl.ruleFormat,
                                                                &ruleSize);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                                ruleSize,
                                                                ruleIndex,
                                                                &rulePclIndex);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssDxChPclRuleParsedGet(devNum,
                ruleTypePtr->rule.pcl.ruleFormat,
                rulePclIndex,
                0,/*ruleOptionsBmp -- unused parameter */
                &ruleDataPtr->valid,
                ruleDataPtr->rule.pcl.maskPtr,
                ruleDataPtr->rule.pcl.patternPtr,
                ruleDataPtr->rule.pcl.actionPtr
                );
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E:
            rc = cpssDxChTtiRuleGet(devNum,
                ruleIndex,
                ruleTypePtr->rule.tti.ruleFormat,
                ruleDataPtr->rule.tti.patternPtr,
                ruleDataPtr->rule.tti.maskPtr,
                CPSS_DXCH_TTI_ACTION_TYPE2_ENT,
                ruleDataPtr->rule.tti.actionPtr
                );
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = cpssDxChTtiRuleValidStatusGet(devNum,
                ruleIndex,
                &ruleDataPtr->valid);

            break;
        default:
            rc = GT_BAD_PARAM;
            break;
    }

    return rc;

}

/*******************************************************************************
* perDevice_ruleActionUpdate
*
* DESCRIPTION:
*       Update the action of a Rule in the TCAM of specific device.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - the device number.
*       ruleIndex     - the rule index in the TCAM. (CPSS API physical index)
*       ruleSize       - the rule size, used to compute actual HW rule index in certain devices
*       actionTypePtr   - (pointer to)type of contents of action
*       actionDataPtr   - (pointer to)data of contents of action
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS perDevice_ruleActionUpdate
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
)
{
    GT_STATUS rc;
    CPSS_PCL_RULE_SIZE_ENT        pclRuleSize;      /* Rule size derived from vTcam rule size. Used only on xCat3 PCL */
    GT_U32                        rulePclIndex;     /* Rule index, adjusted according to underlying API and rule size */

    switch(actionTypePtr->ruleType)
    {
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E:
            CPSS_NULL_PTR_CHECK_MAC(actionDataPtr->action.pcl.actionPtr);

            if(actionTypePtr->action.pcl.direction != CPSS_PCL_DIRECTION_INGRESS_E &&
               actionTypePtr->action.pcl.direction != CPSS_PCL_DIRECTION_EGRESS_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(actionTypePtr->action.pcl.direction == CPSS_PCL_DIRECTION_INGRESS_E &&
               actionDataPtr->action.pcl.actionPtr->egressPolicy != GT_FALSE)
            {
                /* the 'direction' state 'ingress' , but the 'egressPolicy' not state it */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(actionTypePtr->action.pcl.direction == CPSS_PCL_DIRECTION_EGRESS_E &&
               actionDataPtr->action.pcl.actionPtr->egressPolicy != GT_TRUE)
            {
                /* the 'direction' state 'egress' , but the 'egressPolicy' not state it */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            switch(ruleSize)
            {
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_10_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_20_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_30_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_40_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_50_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_60_BYTES_E; break;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                                ruleSize,
                                                                ruleIndex,
                                                                &rulePclIndex);
            if (rc != GT_OK)
            {
                return rc;
            }


            rc = cpssDxChPclRuleActionUpdate(devNum,
                pclRuleSize,/* - not used in bc2 and above */
                rulePclIndex,
                actionDataPtr->action.pcl.actionPtr
                );
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E:
            rc = cpssDxChTtiRuleActionUpdate(devNum,
                ruleIndex,
                CPSS_DXCH_TTI_ACTION_TYPE2_ENT,
                actionDataPtr->action.tti.actionPtr
                );
            break;
        default:
            rc = GT_BAD_PARAM;
            break;
    }

    return rc;

}

/* the size of tti action type 2 in words */
#define TTI_ACTION_TYPE_2_SIZE_CNS      8
extern GT_STATUS prvCpssDxChTtiActionHw2LogicFormat
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              *hwFormatArray,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *logicFormatPtr
);

/* logic taken from function internal_cpssDxChTtiPortGroupRuleGet */
static GT_STATUS ttiRuleActionGet
(
    IN  GT_U8                              devNum,
    IN  GT_U32                             index,
    OUT CPSS_DXCH_TTI_ACTION_UNT           *actionPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwTtiActionArray[TTI_ACTION_TYPE_2_SIZE_CNS];                 /* TTI action in hw format */
    GT_U32      entryNumber;                                                  /* Rule's index in TCAM    */
    GT_U32      dummy;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    /* convert rule global index to TCAM entry number */
    rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,index,&entryNumber,&dummy);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* read action */
    rc = prvCpssDxChReadTableEntry(devNum,
                             PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                             (entryNumber/2),
                             hwTtiActionArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* convert tti action from hw format to logic format */
    rc = prvCpssDxChTtiActionHw2LogicFormat(devNum,hwTtiActionArray,CPSS_DXCH_TTI_ACTION_TYPE2_ENT,actionPtr);

    return rc;
}

/*******************************************************************************
* perDevice_ruleActionGet
*
* DESCRIPTION:
*       Get the action of a Rule from the TCAM of specific device.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - the device number.
*       ruleIndex     - the rule index in the TCAM. (CPSS API physical index)
*       ruleSize       - the rule size, used to compute actual HW rule index in certain devices
*       actionTypePtr   - (pointer to)type of contents of action
*       actionDataPtr   - (pointer to)data of contents of action
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS perDevice_ruleActionGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
)
{
    GT_STATUS rc;
    CPSS_PCL_RULE_SIZE_ENT        pclRuleSize;      /* Rule size derived from vTcam rule size. Used only on xCat3 PCL */
    GT_U32                        rulePclIndex;     /* Rule index, adjusted according to underlying API and rule size */

    switch(actionTypePtr->ruleType)
    {
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_PCL_E:

            switch(ruleSize)
            {
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_10_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_20_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_30_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_40_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_50_BYTES_E; break;
                case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_60_BYTES_E; break;
                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                                ruleSize,
                                                                ruleIndex,
                                                                &rulePclIndex);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssDxChPclRuleActionGet(devNum,
                pclRuleSize,/* - not used in bc2 and above */
                rulePclIndex,
                actionTypePtr->action.pcl.direction,
                actionDataPtr->action.pcl.actionPtr
                );
            break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E:
            rc = ttiRuleActionGet(devNum,
                ruleIndex,
                actionDataPtr->action.tti.actionPtr
                );
            break;
        default:
            rc = GT_BAD_PARAM;
            break;
    }

    return rc;

}

/*******************************************************************************
* perDevice_ruleValidate
*
* DESCRIPTION:
*       Validate/invalidate Rule in the TCAM of specific device.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - the device number.
*       ruleIndex     - the rule index in the TCAM. (CPSS API physical index)
*       ruleSize       - the rule size, used to compute actual HW rule index in certain devices
*       valid         - set the rule as valid on invalid
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS perDevice_ruleValidate
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_BOOL                                     valid
)
{
    GT_STATUS rc;
    CPSS_PCL_RULE_SIZE_ENT pclRuleSize;      /* Rule size derived from vTcam rule size. Used only on xCat3 PCL */
    GT_U32                 rulePclIndex;     /* Rule index, adjusted according to underlying API and rule size */

    /* USE TTI API regardless to 'TTI/PCL' actual format */
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        rc = cpssDxChTtiRuleValidStatusSet(devNum,
            ruleIndex,
            valid
            );
    }
    else
    {
        switch(ruleSize)
        {
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_10_BYTES_E; break;
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_20_BYTES_E; break;
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_30_BYTES_E; break;
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_40_BYTES_E; break;
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_50_BYTES_E; break;
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_60_BYTES_E; break;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                            ruleSize,
                                                            ruleIndex,
                                                            &rulePclIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPclRuleValidStatusSet(devNum,
                                            pclRuleSize,
                                            rulePclIndex,
                                            valid);
    }

    return rc;

}
/*******************************************************************************
* perDevice_ruleValidStatusGet
*
* DESCRIPTION:
*       Gat Valid state of Rule in the TCAM of specific device.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - the device number.
*       ruleIndex     - the rule index in the TCAM. (CPSS API physical index)
*       ruleSize       - the rule size, used to compute actual HW rule index in certain devices
*
* OUTPUTS:
*       validPtr         - (pointer to) the rule as valid on invalid
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS perDevice_ruleValidStatusGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      ruleIndex,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_BOOL                                     *validPtr
)
{
    GT_STATUS rc;
    GT_U32                        rulePclIndex;     /* Rule index, adjusted according to underlying API and rule size */
    CPSS_PCL_RULE_SIZE_ENT        rulePclSize;      /* PCL Rule size translated from TCAM rule size */

    /* USE TTI API regardless to 'TTI/PCL' actual format */
    if(PRV_CPSS_SIP_5_CHECK_MAC(devNum))
    {
        rc = cpssDxChTtiRuleValidStatusGet(devNum,
            ruleIndex,
            validPtr
            );
    }
    else
    {
        /* translate rule size enum to VTCAM and PCL rule size enums */
        switch(ruleSize)
        {
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E:
                    rulePclSize = CPSS_PCL_RULE_SIZE_30_BYTES_E;
                    break;
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E:
                    rulePclSize = CPSS_PCL_RULE_SIZE_60_BYTES_E;
                    break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                            ruleSize,
                                                            ruleIndex,
                                                            &rulePclIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPclRuleAnyStateGet(devNum,
                                        rulePclSize,
                                        rulePclIndex,
                                        validPtr,
                                        &rulePclSize);
    }

    return rc;

}
/*******************************************************************************
* perDevice_ruleMove
*
* DESCRIPTION:
*       Validate/invalidate Rule in the TCAM of specific device.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - the device number.
*       ruleSize      - the rule size to move.
*       srcRuleId     - source rule id - the source rule index in the TCAM. (CPSS API physical index)
*       dstRuleId     - destination rule id - the destination rule index in the TCAM. (CPSS API physical index)
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS perDevice_ruleMove
(
    IN  GT_U8                                       devNum,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT        ruleSize,
    IN  GT_U32                                      srcRuleId,
    IN  GT_U32                                      dstRuleId,
    IN  GT_BOOL                                     moveOrCopy
)
{
    GT_STATUS rc;
    CPSS_PCL_RULE_SIZE_ENT pclRuleSize;      /* Rule size derived from vTcam rule size. Used only on xCat3 PCL */
    GT_U32                 srcPclRuleId;     /* Source Rule index, adjusted according to underlying API and rule size */
    GT_U32                 dstPclRuleId;     /* Destination Rule index, adjusted according to underlying API and rule size */

    switch(ruleSize)
    {
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_10_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_10_BYTES_E; break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_20_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_20_BYTES_E; break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_30_BYTES_E; break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_40_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_40_BYTES_E; break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_50_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_50_BYTES_E; break;
        case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E: pclRuleSize = CPSS_PCL_RULE_SIZE_60_BYTES_E; break;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                        ruleSize,
                                                        srcRuleId,
                                                        &srcPclRuleId);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                        ruleSize,
                                                        dstRuleId,
                                                        &dstPclRuleId);
    if (rc != GT_OK)
    {
        return rc;
    }

    CPSS_LOG_INFORMATION_MAC("[%s] Rule from HW index[%d] to [%d] size[%d] \n",
        (moveOrCopy == GT_TRUE ? "Move" : "Copy") ,
        srcRuleId ,  dstRuleId,  ruleSize);

    /* USE PCL API regardless to 'TTI/PCL' actual format */

    /* copy the rule */
    rc = cpssDxChPclRuleCopy(devNum,
        pclRuleSize,
        srcPclRuleId,
        dstPclRuleId
        );
    if(rc != GT_OK)
    {
        return rc;
    }

    if(moveOrCopy == GT_TRUE)/* the move requires also invalidation of the source */
    {
        /* invalidate the src index */
        rc = perDevice_ruleValidate(devNum,srcRuleId,ruleSize,GT_FALSE);
    }

    return rc;

}

/*******************************************************************************
* ruleMoveByLogicalIndex
*
* DESCRIPTION:
*       Validate/invalidate Rule in the TCAM of specific device, by logical indexes.
*       and update the DB about it.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vtcamInfoPtr      - (pointer to) virtual TCAM structure
*       srcLogicalIndex    - source logical index.
*       srcDeviceRuleIndex - source rule id - the source rule index in the TCAM. (CPSS API physical index)
*       dstLogicalIndex    - destination logical index.
*       dstDeviceRuleIndex - destination rule id - the destination rule index in the TCAM. (CPSS API physical index)
*       calledFromResize   - indication that called from resize operation
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS ruleMoveByLogicalIndex
(
    IN  GT_U32                                      vTcamMngId,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *vtcamInfoPtr,
    IN  GT_U32                                      srcLogicalIndex,
    IN  GT_U32                                      srcDeviceRuleIndex,
    IN  GT_U32                                      dstLogicalIndex,
    IN  GT_U32                                      dstDeviceRuleIndex,
    IN  GT_BOOL                                     calledFromResize
)
{
    GT_STATUS   rc;
    GT_U8     devNum;     /* device number    */

    if(srcDeviceRuleIndex != dstDeviceRuleIndex)/*check HW movement*/
    {
        CPSS_LOG_INFORMATION_MAC("move HW index[%d] to index[%d] (shift[%d]) \n",
            srcDeviceRuleIndex,dstDeviceRuleIndex,((int)dstDeviceRuleIndex-(int)srcDeviceRuleIndex));

        /* get first devNum iterator */
        rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* iteration over the devices */
        do
        {
            /* move the rule in the device */
            rc = perDevice_ruleMove(devNum,vtcamInfoPtr->tcamInfo.ruleSize,
                        srcDeviceRuleIndex,dstDeviceRuleIndex,GT_TRUE/*move*/);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        /* get next devNum iterator */
        while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));
    }

    if(srcLogicalIndex != dstLogicalIndex)/*check DB movement*/
    {
        CPSS_LOG_INFORMATION_MAC("move logical index[%d] to index[%d] (shift[%d]) \n",
            srcLogicalIndex,dstLogicalIndex,((int)dstLogicalIndex-(int)srcLogicalIndex));

        if(calledFromResize == GT_FALSE)
        {
            /*Remove from the DB the 'src' rule (and it's logical index) */
            /* update the DB about the 'src' rule */
            vtcamInfoPtr->usedRulesBitmapArr[srcLogicalIndex>>5] &= ~(1<<(srcLogicalIndex & 0x1f));
            /* update the counter */
            vtcamInfoPtr->usedRulesAmount--;
        }
        else
        {
            if(vtcamInfoPtr->usedRulesBitmapArr[srcLogicalIndex>>5] & (1<<(srcLogicalIndex & 0x1f)))
            {
                /* update the DB about the 'src' rule */
                vtcamInfoPtr->usedRulesBitmapArr[srcLogicalIndex>>5] &= ~(1<<(srcLogicalIndex & 0x1f));
            }
        }

        /* for priority mode - remove 'src' rule ID entry from DB */
        if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
            CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
        {
            if(calledFromResize == GT_FALSE)
            {
                rc = priorityDbRemoveRuleId(vTcamMngDB[vTcamMngId], vtcamInfoPtr, srcLogicalIndex);
                if(rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
                }
            }
            else  /* update the src logical Index entry to trg logical index */
            {
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC ruleIdEntry, * ruleIdEntryPtr;

                ruleIdEntry.logicalIndex = srcLogicalIndex;
                /* find logicalIndex in entry in AVL tree */
                ruleIdEntryPtr = prvCpssAvlSearch(vtcamInfoPtr->ruleIdIndexTree, &ruleIdEntry);
                if (ruleIdEntryPtr == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "DB error: srcLogicalIndex[%d] was not found in ruleIdIndexTree",
                        srcLogicalIndex);
                }

                /* remove rule ID entry from the index tree */
                ruleIdEntryPtr = prvCpssAvlItemRemove(vtcamInfoPtr->ruleIdIndexTree, ruleIdEntryPtr);
                if(ruleIdEntryPtr == NULL)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "DB error: tree ruleIdIndexTree : failed to remove srcLogicalIndex[%d] (needed temporary removal before adding it again)",
                        srcLogicalIndex);
                }
                /* update the logical index and add it to the tree */
                ruleIdEntryPtr->logicalIndex = dstLogicalIndex;
                /* add updated rule ID entry to the index tree */
                rc = prvCpssAvlItemInsert(vtcamInfoPtr->ruleIdIndexTree, ruleIdEntryPtr);
                if(rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "DB error: tree ruleIdIndexTree : failed to re-insert dstLogicalIndex[%d] (after temporary removed)",
                        dstLogicalIndex);
                }
            }
        }

        /* NOTE: next is never valid for 'priority mode' because the dstRuleId
           already checked for existence */
        if(0 == (vtcamInfoPtr->usedRulesBitmapArr[dstLogicalIndex>>5] & (1<<(dstLogicalIndex & 0x1f))))
        {
            /* update the DB about the 'trg' rule */

            /* dst was not valid */

            if(calledFromResize == GT_FALSE)
            {
                /* update the counter */
                vtcamInfoPtr->usedRulesAmount++;
            }

            /* update the DB about the moved rule */
            vtcamInfoPtr->usedRulesBitmapArr[dstLogicalIndex>>5] |= (1<<(dstLogicalIndex & 0x1f));
        }
    }


    return  GT_OK;
}

/*******************************************************************************/
/*******************************************************************************/
/******************** end 'per device' functions *******************************/
/*******************************************************************************/
/*******************************************************************************/
/*******************************************************************************
* copySingleRuleFromSrcDevToDevBmp
*
* DESCRIPTION:
*       Copy single rule from a representative device to all needed devices.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       representativeDevNum - the device to copy from it
*       newDevsBmp           - the devices to copy to them
*       hwIndex              - the hwIndex to read from the representativeDevNum
*                               and to copy to newDevsBmp device(s)
*       ruleSize             - the rule size of the read and copy entry
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameter
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS copySingleRuleFromSrcDevToDevBmp(
    IN GT_U8    representativeDevNum ,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP newDevsBmp,
    IN GT_U32   hwIndex,
    IN CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize
)
{
    GT_STATUS   rc;
    GT_U32  currDevNum;

    /* read from HW */
    rc = rawEntryFormatOperationReadFromHw(representativeDevNum,ruleSize,hwIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    for (currDevNum = 0; currDevNum < PRV_CPSS_MAX_PP_DEVICES_CNS; currDevNum++)
    {
        if(newDevsBmp[currDevNum>>5] == 0)
        {
            currDevNum += 31;/* with the '++' it will be 32 */
            continue;
        }

        if(0 == (newDevsBmp[currDevNum>>5] & (1<<(currDevNum&0x1f))))
        {
            continue;
        }

        /* write to HW */
        rc = rawEntryFormatOperationWriteToHw((GT_U8)currDevNum,ruleSize,hwIndex);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;

}
/*******************************************************************************
* copyAllRulesFromSrcDevToDevBmp_perVtcam
*
* DESCRIPTION:
*       Copy all rules from vtcam of the TCAM , from srcDevice to all needed devices.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vtcamInfoPtr          - (pointer to)  Virtual TCAM
*       representativeDevNum - the device to copy from it
*       newDevsBmp          - the devices to copy to them
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS copyAllRulesFromSrcDevToDevBmp_perVtcam(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr,
    IN GT_U8    representativeDevNum ,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP newDevsBmp
)
{
    GT_STATUS   rc;
    GT_U32  logicalIndex;
    GT_U32  hwIndex;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize = (GT_U32)vtcamInfoPtr->tcamInfo.ruleSize;

    for(logicalIndex = 0 ; logicalIndex < vtcamInfoPtr->rulesAmount ; logicalIndex++)
    {
        if(0 == (vtcamInfoPtr->usedRulesBitmapArr[logicalIndex>>5] & (1<<(logicalIndex & 0x1f))))
        {
            /* not used logical index */
            continue;
        }

        /* convert logical index to HW index */
        hwIndex = vtcamInfoPtr->rulePhysicalIndexArr[logicalIndex];

        /* copy single rule from device to bmp of devices */
        rc = copySingleRuleFromSrcDevToDevBmp(representativeDevNum,newDevsBmp,hwIndex,ruleSize);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}
/*******************************************************************************
* copyAllRulesFromSrcDevToDevBmp
*
* DESCRIPTION:
*       Copy all rules from all vtcams of the TCAM , from srcDevice to all needed devices.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       representativeDevNum - the device to copy from it
*       newDevsBmp          - the devices to copy to them
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS copyAllRulesFromSrcDevToDevBmp(
    IN GT_U32   vTcamMngId,
    IN GT_U8    representativeDevNum ,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP newDevsBmp
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *tcamMngDbPtr;
    GT_U32  vTcamId;

    tcamMngDbPtr = vTcamMngDB[vTcamMngId];

    for(vTcamId = 0; vTcamId < tcamMngDbPtr->vTcamCfgPtrArrSize ; vTcamId++)
    {
        if(tcamMngDbPtr->vTcamCfgPtrArr[vTcamId] == NULL)
        {
            continue;
        }

        /* do copy for this vtcam */
        rc = copyAllRulesFromSrcDevToDevBmp_perVtcam(tcamMngDbPtr->vTcamCfgPtrArr[vTcamId],
            representativeDevNum,newDevsBmp);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************/
/*******************************************************************************/
/****************** start APIs *************************************************/
/*******************************************************************************/
/*******************************************************************************/

/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleWrite
*
* DESCRIPTION:
*       Write Rule to Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*                           if rule with given rule Id already exists it overridden only
*                           when the same priotity specified, otherwize GT_BAD_PARAM returned.
*       ruleAttributesPtr - (pointer to)rule attributes (for priority driven vTCAM - priority)
*       ruleTypePtr   -     (pointer to)type of contents of rule
*       ruleDataPtr   -     (pointer to)data of contents of rule
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleWrite
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC  *ruleAttributesPtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_BOOL newRule = GT_FALSE;
    GT_U32  logicalIndex = 0; /* logical index that converted from ruleId */

    CPSS_NULL_PTR_CHECK_MAC(ruleAttributesPtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleDataPtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E)
    {
        CPSS_LOG_INFORMATION_MAC("mode : LOGICAL_INDEX \n");
        logicalIndex = ruleId;

        CPSS_LOG_INFORMATION_MAC("logicalIndex[%d] \n" , logicalIndex);

        if(logicalIndex >= vtcamInfoPtr->rulesAmount)
        {
            CPSS_LOG_INFORMATION_MAC("error : logicalIndex[%d] >= vtcamInfoPtr->rulesAmount[%d] \n",
                logicalIndex ,vtcamInfoPtr->rulesAmount);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* check if the rule was already in the DB  */
        newRule = (vtcamInfoPtr->usedRulesBitmapArr[logicalIndex>>5] & (1<<(logicalIndex & 0x1f))) ?
            GT_FALSE :
            GT_TRUE;
        if(newRule == GT_TRUE)
        {
            CPSS_LOG_INFORMATION_MAC("new rule \n");

            if(vtcamInfoPtr->usedRulesAmount >= vtcamInfoPtr->rulesAmount)
            {
                CPSS_LOG_INFORMATION_MAC("error : vtcamInfoPtr->usedRulesAmount[%d] >= vtcamInfoPtr->rulesAmount[%d] \n",
                    vtcamInfoPtr->usedRulesAmount , vtcamInfoPtr->rulesAmount);
                /* 'management' error */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            CPSS_LOG_INFORMATION_MAC("existing rule \n");
        }
    }
    else
    {
        CPSS_LOG_INFORMATION_MAC("mode : PRIORITY \n");

        if(ruleAttributesPtr->priority >= BIT_16)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"priority out of range 64K [%d]",
                ruleAttributesPtr->priority);
        }

        /*
        *       Get place for new rule.
        *       If rule exists it returns it's logical index (and no changes needed).
        *       If needed move existing rules.
        *       If needed resize vTCAM.
        */
        rc = priorityRuleWritePreparations(vTcamMngId,vTcamId,vtcamInfoPtr,
            ruleId,
            ruleAttributesPtr->priority,
            &logicalIndex,&newRule);
        if(rc != GT_OK)
        {
            return rc;
        }

        CPSS_LOG_INFORMATION_MAC("logicalIndex[%d] \n" , logicalIndex);
    }

    /* convert the logical index to physical index in the HW */
    deviceRuleIndex = vtcamInfoPtr->rulePhysicalIndexArr[logicalIndex];

    CPSS_LOG_INFORMATION_MAC("convert the logical index[%d] to physical index in the HW[%d] \n" ,
        deviceRuleIndex , vtcamInfoPtr->rulePhysicalIndexArr[logicalIndex]);

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("no valid first devNum \n");
        return rc;
    }

    /* iteration over the devices */
    do
    {
        CPSS_LOG_INFORMATION_MAC("devNum iterator [%d] \n" ,
            devNum);

        /* write the rule to the device */
        rc = perDevice_ruleWrite(devNum,deviceRuleIndex,
                ruleTypePtr,ruleDataPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    /* check if the rule was already in the DB  */
    if(newRule == GT_TRUE)
    {
        CPSS_LOG_INFORMATION_MAC("update the DB about the new rule \n");

        if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
            CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
        {
            /* update the DB about the new rule */
            rc = priorityNewRuleIdAddedDbUpdate(vTcamMngId,vTcamId,
                ruleId,
                logicalIndex);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* NOTE: even if logicalIndex bit is already set ... we need to update
           the the counter.
           because the bit in this case was preserved 'high' during the shifting
           of valid ranges into 'hole' that was found to allow the new rule to
           fit into it's priority */
        /* update the DB about the new rule */
        vtcamInfoPtr->usedRulesBitmapArr[logicalIndex>>5] |= (1<<(logicalIndex & 0x1f));
        /* update the counter */
        vtcamInfoPtr->usedRulesAmount++;

        CPSS_LOG_INFORMATION_MAC("updated the DB counter : usedRulesAmount[%d] \n" ,
            vtcamInfoPtr->usedRulesAmount);
    }
    else
    {
        /* already in the DB */
    }




    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleWrite
*
* DESCRIPTION:
*       Write Rule to Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*                           if rule with given rule Id already exists it overridden only
*                           when the same priotity specified, otherwize GT_BAD_PARAM returned.
*       ruleAttributesPtr - (pointer to)rule attributes (for priority driven vTCAM - priority)
*       ruleTypePtr   -     (pointer to)type of contents of rule
*       ruleDataPtr   -     (pointer to)data of contents of rule
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleWrite
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC  *ruleAttributesPtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleWrite);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, ruleAttributesPtr, ruleTypePtr, ruleDataPtr));

    rc = internal_cpssDxChVirtualTcamRuleWrite(vTcamMngId, vTcamId, ruleId, ruleAttributesPtr, ruleTypePtr, ruleDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, ruleAttributesPtr, ruleTypePtr, ruleDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleRead
*
* DESCRIPTION:
*       Read Rule from Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*       ruleTypePtr   -     (pointer to)type of contents of rule
*
* OUTPUTS:
*       ruleDataPtr   -      (pointer to)data of contents of rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleRead
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */

    CPSS_NULL_PTR_CHECK_MAC(ruleTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(ruleDataPtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    /* convert the ruleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &logicalIndex,&deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* read the rule from the first device */
    rc = perDevice_ruleRead(devNum,deviceRuleIndex,
            ruleTypePtr,ruleDataPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleRead
*
* DESCRIPTION:
*       Read Rule from Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*       ruleTypePtr   -     (pointer to)type of contents of rule
*
* OUTPUTS:
*       ruleDataPtr   -      (pointer to)data of contents of rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleRead
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        *ruleTypePtr,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        *ruleDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleRead);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, ruleTypePtr, ruleDataPtr));

    rc = internal_cpssDxChVirtualTcamRuleRead(vTcamMngId, vTcamId, ruleId, ruleTypePtr, ruleDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, ruleTypePtr, ruleDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleActionUpdate
*
* DESCRIPTION:
*       Update Rule Action in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId      - Virtual TCAM Manager Id
*                         (APPLICABLE RANGES: 0..31)
*       vTcamId         - unique Id of  Virtual TCAM
*       ruleId          - for logical-index driven vTCAM - logical-index
*                         for priority driven vTCAM - ruleId that used to refer existing rule
*       actionTypePtr   - (pointer to)type of contents of action
*       actionDataPtr   - (pointer to)data of contents of action
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleActionUpdate
(
    IN  GT_U32                                        vTcamMngId,
    IN  GT_U32                                        vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */

    CPSS_NULL_PTR_CHECK_MAC(actionTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(actionDataPtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    /* convert the ruleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &logicalIndex,&deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* iteration over the devices */
    do
    {
        /* update the action in the device */
        rc = perDevice_ruleActionUpdate(devNum,deviceRuleIndex,vtcamInfoPtr->tcamInfo.ruleSize,
                actionTypePtr,actionDataPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleActionUpdate
*
* DESCRIPTION:
*       Update Rule Action in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId      - Virtual TCAM Manager Id
*                         (APPLICABLE RANGES: 0..31)
*       vTcamId         - unique Id of  Virtual TCAM
*       ruleId          - for logical-index driven vTCAM - logical-index
*                         for priority driven vTCAM - ruleId that used to refer existing rule
*       actionTypePtr   - (pointer to)type of contents of action
*       actionDataPtr   - (pointer to)data of contents of action
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleActionUpdate
(
    IN  GT_U32                                        vTcamMngId,
    IN  GT_U32                                        vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleActionUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, actionTypePtr, actionDataPtr));

    rc = internal_cpssDxChVirtualTcamRuleActionUpdate(vTcamMngId, vTcamId, ruleId, actionTypePtr, actionDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, actionTypePtr, actionDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleActionGet
*
* DESCRIPTION:
*       Get Rule Action in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId      - Virtual TCAM Manager Id
*                         (APPLICABLE RANGES: 0..31)
*       vTcamId         - unique Id of  Virtual TCAM
*       ruleId          - for logical-index driven vTCAM - logical-index
*                         for priority driven vTCAM - ruleId that used to refer existing rule
*       actionTypePtr   - (pointer to)type of contents of action
*
* OUTPUTS:
*       actionDataPtr   - (pointer to)data of contents of action
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleActionGet
(
    IN  GT_U32                                        vTcamMngId,
    IN  GT_U32                                        vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */

    CPSS_NULL_PTR_CHECK_MAC(actionTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(actionDataPtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    /* convert the ruleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &logicalIndex,&deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* read the action from the first device */
    rc = perDevice_ruleActionGet(devNum,deviceRuleIndex,vtcamInfoPtr->tcamInfo.ruleSize,
            actionTypePtr,actionDataPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleActionGet
*
* DESCRIPTION:
*       Get Rule Action in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId      - Virtual TCAM Manager Id
*                         (APPLICABLE RANGES: 0..31)
*       vTcamId         - unique Id of  Virtual TCAM
*       ruleId          - for logical-index driven vTCAM - logical-index
*                         for priority driven vTCAM - ruleId that used to refer existing rule
*       actionTypePtr   - (pointer to)type of contents of action
*
* OUTPUTS:
*       actionDataPtr   - (pointer to)data of contents of action
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleActionGet
(
    IN  GT_U32                                        vTcamMngId,
    IN  GT_U32                                        vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                ruleId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC   *actionTypePtr,
    OUT CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC   *actionDataPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleActionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, actionTypePtr, actionDataPtr));

    rc = internal_cpssDxChVirtualTcamRuleActionGet(vTcamMngId, vTcamId, ruleId, actionTypePtr, actionDataPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, actionTypePtr, actionDataPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleValidStatusSet
*
* DESCRIPTION:
*       Set Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       valid            - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleValidStatusSet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId,
    IN   GT_BOOL                            valid
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    /* convert the ruleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &logicalIndex,&deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* iteration over the devices */
    do
    {
        /* invalidate the rule in the device */
        rc = perDevice_ruleValidate(devNum,deviceRuleIndex,vtcamInfoPtr->tcamInfo.ruleSize,valid);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    return GT_OK;
}


/*******************************************************************************
* cpssDxChVirtualTcamRuleValidStatusSet
*
* DESCRIPTION:
*       Set Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       valid            - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleValidStatusSet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId,
    IN   GT_BOOL                            valid
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleValidStatusSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, valid));

    rc = internal_cpssDxChVirtualTcamRuleValidStatusSet(vTcamMngId, vTcamId, ruleId, valid);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, valid));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleValidStatusGet
*
* DESCRIPTION:
*       Get Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       validPtr         - (pointer to)GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleValidStatusGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId,
    OUT  GT_BOOL                            *validPtr
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */

    CPSS_NULL_PTR_CHECK_MAC(validPtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    /* convert the ruleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &logicalIndex,&deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* invalidate the rule in the device */
    rc = perDevice_ruleValidStatusGet(devNum,deviceRuleIndex,vtcamInfoPtr->tcamInfo.ruleSize,validPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleValidStatusGet
*
* DESCRIPTION:
*       Get Rule Valid Status in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       validPtr         - (pointer to)GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleValidStatusGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId,
    OUT  GT_BOOL                            *validPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleValidStatusGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, validPtr));

    rc = internal_cpssDxChVirtualTcamRuleValidStatusGet(vTcamMngId, vTcamId, ruleId, validPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, validPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleDelete
*
* DESCRIPTION:
*       Delete Rule From internal DB and Invalidate it in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleDelete
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId
)
{
    GT_STATUS   rc;
    GT_U32  deviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */
    GT_U32  saveGuaranteedNumOfRules;
    GT_BOOL doAutoResize;


    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    /* convert the ruleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &logicalIndex,&deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    if(vtcamInfoPtr->usedRulesAmount == 0)
    {
        /* 'management' error */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* iteration over the devices */
    do
    {
        /* invalidate the rule in the device */
        rc = perDevice_ruleValidate(devNum,deviceRuleIndex,vtcamInfoPtr->tcamInfo.ruleSize,GT_FALSE);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    /* update the DB about the removed rule */
    vtcamInfoPtr->usedRulesBitmapArr[logicalIndex>>5] &= ~(1<<(logicalIndex & 0x1f));

    /* update the counter */
    vtcamInfoPtr->usedRulesAmount--;

    /* for priority mode - remove rule ID entry from DB */
    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
    {
        vTcamMngDBPtr = vTcamMngDB[vTcamMngId];

        rc = priorityDbRemoveRuleId(vTcamMngDBPtr, vtcamInfoPtr, logicalIndex);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    /* for priority mode - autoresize */
    doAutoResize = vtcamInfoPtr->tcamInfo.autoResize;
    if (vtcamInfoPtr->tcamInfo.ruleAdditionMethod !=
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
    {
        doAutoResize = GT_FALSE;
    }
    /* really can be == only */
    if (vtcamInfoPtr->rulesAmount <= vtcamInfoPtr->tcamInfo.guaranteedNumOfRules)
    {
        doAutoResize = GT_FALSE;
    }
    /* really can be == only */
    if (vtcamInfoPtr->rulesAmount <= vtcamInfoPtr->usedRulesAmount)
    {
        doAutoResize = GT_FALSE;
    }
    if (vtcamInfoPtr->usedRulesAmount > vtcamInfoPtr->tcamInfo.guaranteedNumOfRules)
    {
        /* granularity applicable onle when used rules more then guaranteed */
        if ((vtcamInfoPtr->rulesAmount - vtcamInfoPtr->usedRulesAmount)
            < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_AUTO_RESIZE_GRANULARITY_CNS)
        {
            doAutoResize = GT_FALSE;
        }
        if ((vtcamInfoPtr->rulesAmount - vtcamInfoPtr->tcamInfo.guaranteedNumOfRules)
            < PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_AUTO_RESIZE_GRANULARITY_CNS)
        {
            doAutoResize = GT_FALSE;
        }
    }

    if (doAutoResize != GT_FALSE)
    {
        /* decreaze vTCAM size */
        saveGuaranteedNumOfRules = vtcamInfoPtr->tcamInfo.guaranteedNumOfRules;
        rc = vtcamDbSegmentTableVTcamDownSize__PriorityMode(
            vTcamMngId, vTcamId,
            (vtcamInfoPtr->rulesAmount - vtcamInfoPtr->usedRulesAmount) /*downSizeValue*/);
        vtcamInfoPtr->tcamInfo.guaranteedNumOfRules = saveGuaranteedNumOfRules;
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleDelete
*
* DESCRIPTION:
*       Delete Rule From internal DB and Invalidate it in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleDelete
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     ruleId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleDelete);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId));

    rc = internal_cpssDxChVirtualTcamRuleDelete(vTcamMngId, vTcamId, ruleId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChVirtualTcamRuleMove
*
* DESCRIPTION:
*       Move Rule in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       srcRuleId        - source rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       dstRuleId        - destination rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRuleMove
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     srcRuleId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     dstRuleId
)
{
    GT_STATUS   rc;
    GT_U32  srcDeviceRuleIndex,trgDeviceRuleIndex;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U32  src_logicalIndex; /* source logical index that converted from srcRuleId */
    GT_U32  dst_logicalIndex; /* destination logical index that converted from dstRuleId */
    GT_BOOL dst_checkExistance; /*indication if need to check that the dstRuleId already
                          exists in the DB. */

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    /* convert the srcRuleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,srcRuleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &src_logicalIndex,&srcDeviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    if(srcRuleId == dstRuleId)
    {
        CPSS_LOG_INFORMATION_MAC("No action done when srcRuleId[%d] == dstRuleId \n",
            srcRuleId);

        /* nothing to update */
        return GT_OK;
    }

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E)
    {
        /* the trgRule is not checked for existence */
        dst_checkExistance = GT_FALSE;
    }
    else
    {
        /* the trgRule is checked for existence */
        dst_checkExistance = GT_TRUE;
    }

    /* convert the dstRuleId to logical index and to physical index */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,dstRuleId,
        dst_checkExistance,
        &dst_logicalIndex,&trgDeviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    /* move the index in HW and in DB */
    rc = ruleMoveByLogicalIndex(vTcamMngId,vtcamInfoPtr,
        src_logicalIndex,srcDeviceRuleIndex,
        dst_logicalIndex,trgDeviceRuleIndex,
        GT_FALSE/*calledFromResize*/);

    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRuleMove
*
* DESCRIPTION:
*       Move Rule in Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       srcRuleId        - source rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*       dstRuleId        - destination rule id: for logical-index driven vTCAM - logical-index
*                          for priority driven vTCAM - ruleId that used to refer existing rule
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRuleMove
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     srcRuleId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID     dstRuleId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRuleMove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, srcRuleId, dstRuleId));

    rc = internal_cpssDxChVirtualTcamRuleMove(vTcamMngId, vTcamId, srcRuleId, dstRuleId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, srcRuleId, dstRuleId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/* buffers for rawEntryFormatOperation() */
static GT_U32       rawEntryFormatOperation_action[CPSS_DXCH_PCL_ACTION_SIZE_IN_WORDS_CNS]; /* Size of action in Bobcat2; Caelum; Bobcat3 is 8 words for TTI and PCL */
static GT_U32       rawEntryFormatOperation_mask[CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS];     /* Rule entry size in words - defined by Bobcat2; Caelum; Bobcat3 size 7 x 3 words for PCL. for TTI it's 8 words */
static GT_U32       rawEntryFormatOperation_pattern[CPSS_DXCH_PCL_RULE_SIZE_IN_WORDS_CNS];  /* Rule entry size in words - defined by Bobcat2; Caelum; Bobcat3 size 7 x 3 words for PCL. for TTI it's 8 words */
static GT_BOOL      rawEntryFormatOperation_valid;

/*******************************************************************************
* rawEntryFormatOperationReadFromHw
*
* DESCRIPTION:
*       1. copy raw entry (pattern,mask,action) from HW to SW buffer.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       ruleSize           - rule size
*       ruleIndex          - HW index of the rule in the TCAM from which pattern,
*                             mask and action are taken.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       function based on : prvCpssDxChPclLion3RuleCopy
*
*******************************************************************************/
static GT_STATUS rawEntryFormatOperationReadFromHw
(
    IN  GT_U8                        devNum,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                        ruleIndex
)
{
    GT_STATUS    rc;                                             /* return code */
    GT_BOOL      is_eArch;                                       /* whether device is eArch */
    CPSS_PCL_RULE_SIZE_ENT       pclRuleSize;                   /* PCL Rule size translated from TCAM rule size */
    CPSS_PCL_RULE_SIZE_ENT       pclRuleSizeFound;              /* previously configured PCL rule size by given rule index */
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT vTcamRuleSize;         /* Virtual TCAM rule size translated from TCAM rule size */
    GT_U32                       pclRuleIndex;                  /* Rule index, adjusted according to underlying API and rule size */
    CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSizeFound;                 /* previously configured rule size by given rule index */
    GT_U32       entryNumber;                                    /* Rule's index in TCAM */
    GT_U32       dummy;

    GT_PORT_GROUPS_BMP portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    GT_U32  portGroupId=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    /* Support only xCat3 or SIP5 devices */
    is_eArch = PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum);
    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
        (GT_FALSE == is_eArch))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if (is_eArch)
    {
        /* get Validity and size of source rule */
        rc = cpssDxChTcamPortGroupRuleValidStatusGet(devNum,portGroupsBmp,ruleIndex,&rawEntryFormatOperation_valid,&ruleSizeFound);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to get validity and size of source rule for ruleIndex[%d] \n",
                ruleIndex);
        }

        if (ruleSize != ruleSizeFound)
        {
            /* source rule has another size */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"mismatch : for ruleIndex[%d] : expected rule size[%d] but actual is[%d] \n",
                ruleIndex,ruleSize,ruleSizeFound);
        }

        /* Converts rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,ruleIndex,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to convert rule global index to TCAM entry number : for ruleIndex[%d] \n",
                ruleIndex);
        }

        /* get action of source rule */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,
                                                portGroupId,
                                                PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                (entryNumber/2),
                                                rawEntryFormatOperation_action);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to get action of source rule : for ruleIndex[%d] \n",
                ruleIndex);
        }

        /* get mask and pattern */
        rc =  cpssDxChTcamPortGroupRuleRead(devNum,portGroupsBmp,ruleIndex,
            &rawEntryFormatOperation_valid,
            &ruleSizeFound,
            rawEntryFormatOperation_pattern,
            rawEntryFormatOperation_mask);
    }
    else
    {
        /* translate rule size enum to VTCAM and PCL rule size enums */
        switch(ruleSize)
        {
            case CPSS_DXCH_TCAM_RULE_SIZE_30_B_E:
                    pclRuleSize = CPSS_PCL_RULE_SIZE_30_BYTES_E;
                    vTcamRuleSize = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E;
                    break;
            case CPSS_DXCH_TCAM_RULE_SIZE_60_B_E:
                    pclRuleSize = CPSS_PCL_RULE_SIZE_60_BYTES_E;
                    vTcamRuleSize = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E;
                    break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                            vTcamRuleSize,
                                                            ruleIndex,
                                                            &pclRuleIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* get Validity and size of source rule */
        rc = cpssDxChPclPortGroupRuleAnyStateGet(devNum,
                                                    portGroupsBmp,
                                                    pclRuleSize,
                                                    pclRuleIndex,
                                                    &rawEntryFormatOperation_valid,
                                                    &pclRuleSizeFound);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to get validity and size of source rule for ruleIndex[%d] \n",
                ruleIndex);
        }

        if ( pclRuleSize !=  pclRuleSizeFound)
        {
            /* source rule has another size */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"mismatch : for ruleIndex[%d] : expected rule size[%d] but actual is[%d] \n",
                ruleIndex,pclRuleSize,pclRuleSizeFound);
        }

        /* get action, mask and pattern */
        rc = cpssDxChPclPortGroupRuleGet(
            devNum, portGroupsBmp, pclRuleSize, pclRuleIndex,
            rawEntryFormatOperation_mask, rawEntryFormatOperation_pattern, rawEntryFormatOperation_action);
    }
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to get mask and pattern : for ruleIndex[%d] \n",
            ruleIndex);
    }

    return GT_OK;
}

/*******************************************************************************
* rawEntryFormatOperationWriteToHw
*
* DESCRIPTION:
*       1. copy raw entry (pattern,mask,action) from HW to SW buffer.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       ruleSize           - rule size
*       ruleIndex          - HW index of the rule in the TCAM to which pattern,
*                             mask and action are written.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       function based on : prvCpssDxChPclLion3RuleCopy
*
*******************************************************************************/
static GT_STATUS rawEntryFormatOperationWriteToHw
(
    IN  GT_U8                        devNum,
    IN  CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize,
    IN  GT_U32                        ruleIndex
)
{
    GT_STATUS    rc;                                             /* return code */
    GT_BOOL      is_eArch;                                       /* whether device is eArch */
    GT_U32       ruleSizeVal=0;                                  /* rule size (in std rules)      */
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT vTcamRuleSize;          /* Virtual TCAM rule size translated from TCAM rule size */
    GT_U32                       pclRuleIndex;                   /* Rule index, adjusted according to underlying API and rule size */
    GT_U32       entryNumber;                                    /* Rule's index in TCAM */
    GT_U32       dummy;

    GT_PORT_GROUPS_BMP portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
    GT_U32  portGroupId=0;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    /* Support only xCat3 or SIP5 devices */
    is_eArch = PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum);
    if((PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_DXCH_XCAT3_E) &&
        (GT_FALSE == is_eArch))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if (is_eArch)
    {
        /* Converts rule global index to TCAM entry number */
        rc = prvCpssDxChTcamRuleIndexToEntryNumber(devNum,ruleIndex,&entryNumber,&dummy);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to convert rule global index to TCAM entry number : for ruleIndex[%d] \n",
                ruleIndex);
        }

        /* write action to destination rule */
        rc = prvCpssDxChPortGroupWriteTableEntry(devNum,
                                                 portGroupId,
                                                 PRV_CPSS_DXCH_LION3_TABLE_TCAM_PCL_TTI_ACTION_E,
                                                 (entryNumber/2),
                                                 rawEntryFormatOperation_action);
        if (rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to write action to destination rule : for ruleIndex[%d] \n",
                ruleIndex);
        }

        /* write destination rule */
        rc = cpssDxChTcamPortGroupRuleWrite(devNum,portGroupsBmp,ruleIndex,
            rawEntryFormatOperation_valid,
            ruleSize,
            rawEntryFormatOperation_pattern,
            rawEntryFormatOperation_mask);
    }
    else
    {
        /* translate rule size enum to VTCAM and PCL rule size enums */
        switch(ruleSize)
        {
            case CPSS_DXCH_TCAM_RULE_SIZE_30_B_E:
                    vTcamRuleSize = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E;
                    ruleSizeVal = 1;
                    break;
            case CPSS_DXCH_TCAM_RULE_SIZE_60_B_E:
                    vTcamRuleSize = CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E;
                    ruleSizeVal = 2;
                    break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        rc = prvCpssDxChVirtualTcamRuleIndexToRulePclIndex(devNum,
                                                            vTcamRuleSize,
                                                            ruleIndex,
                                                            &pclRuleIndex);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChPclTcamRuleWrite(
            devNum, portGroupId, ruleSizeVal, pclRuleIndex,
            rawEntryFormatOperation_valid,
            GT_TRUE,    /* waitPrevReady */
            rawEntryFormatOperation_action,
            rawEntryFormatOperation_mask, rawEntryFormatOperation_pattern);
    }

    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"failed to write destination mask and pattern : for ruleIndex[%d] \n",
            ruleIndex);
    }

    return GT_OK;
}
/* operation type for rawEntryFormatOperation */
typedef enum{
    RAW_ENTRY_FORMAT_OPERATION_UPLOAD_HW_TO_SW_E,   /*read from HW to SW */
    RAW_ENTRY_FORMAT_OPERATION_DOWNLOAD_SW_TO_HW_E  /*write from SW to HW */
}RAW_ENTRY_FORMAT_OPERATION_ENT;

/*******************************************************************************
* rawEntryFormatOperation
*
* DESCRIPTION:
*       This function supports 2 operations :
*       1. copy raw entry (pattern,mask,action) from HW to SW buffer.
*       2. copy raw entry (pattern,mask,action) from SW buffer to HW.
*       This function copies the Rule's mask, pattern and action to new TCAM position.
*       The source Rule is not invalidated by the function. To implement move TCAM
*       Rule from old position to new one at first prvCpssDxChTcamRuleCopy
*       should be called. And after this cpssDxChTcamGortGroupRuleValidStatusSet should
*       be used to invalidate rule in old position.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vtcamInfoPtr        - (pointer to)virtual TCAM structure
*       operation           - operation type
*       ruleHwIndex        - HW index of the rule in the TCAM . (to read from or to write to)
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter, and when the rule cannot start in a given index.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*
* COMMENTS:
*       function based on : prvCpssDxChPclLion3RuleCopy
*
*******************************************************************************/
static GT_STATUS rawEntryFormatOperation
(
    IN   GT_U32                                        vTcamMngId,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC   *vtcamInfoPtr,
    IN    RAW_ENTRY_FORMAT_OPERATION_ENT                operation,
    IN  GT_U32                                          ruleHwIndex
)
{
    GT_STATUS   rc;
    GT_U8   devNum;
    CPSS_DXCH_TCAM_RULE_SIZE_ENT ruleSize = (GT_U32)vtcamInfoPtr->tcamInfo.ruleSize;


    /* get first devNum iterator */
    rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* iteration over the devices */
    do
    {
        if(operation == RAW_ENTRY_FORMAT_OPERATION_DOWNLOAD_SW_TO_HW_E)
        {
            /* write to HW */
            rc = rawEntryFormatOperationWriteToHw(devNum,ruleSize,ruleHwIndex);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            /* read from HW */
            rc = rawEntryFormatOperationReadFromHw(devNum,ruleSize,ruleHwIndex);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }
            break;/* from single device */
        }
    }
    /* get next devNum iterator */
    while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));

    return GT_OK;
}

/*******************************************************************************
* vtcamFullMoveRuleByRawFormat
*
* DESCRIPTION:
*       we need to move HW entry from old place to new but the table is full and
*       FORCED resize didn't helped.
*       the rule will be absent from the HW for some time during the function
*       operation.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - rule id: that used to refer existing rule
*       priority         - the new priority of the rule
*       position         - position in range of rule of the
*                          specified priority (see enum)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS vtcamFullMoveRuleByRawFormat
(
    IN   GT_U32                                        vTcamMngId,
    IN   GT_U32                                                    vTcamId,
    IN   PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC   *vtcamInfoPtr,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                            ruleId,
    IN   GT_U32                                                    priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position,
    IN   GT_U32  orig_deviceRuleIndex,
    IN   GT_U32  orig_logicalIndex
)
{
    GT_STATUS rc;
    GT_U32  new_deviceRuleIndex;/* new HW index that will be for the ruleId */
    GT_U32  new_logicalIndex; /* new logical index that will be for the ruleId */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    GT_U32  orig_usedRulesAmount;

    /* the table is full and FORCED resize didn't helped */
    CPSS_LOG_INFORMATION_MAC("the Table is FULL and operation will remove the "
        "entry from the HW (for short time...) and then reposition it to new place \n");

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];

    /* 1. copy raw entry (pattern,mask,action) from HW to SW buffer.*/
    rc = rawEntryFormatOperation(vTcamMngId,vtcamInfoPtr,
        RAW_ENTRY_FORMAT_OPERATION_UPLOAD_HW_TO_SW_E,
        orig_deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* 2. remove the entry from the DB */
    {
        /* remove the old rule according to logical index */
        rc = priorityDbRemoveRuleId(vTcamMngDBPtr, vtcamInfoPtr, orig_logicalIndex);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }

        /* remove the original logical index from the DB */
        vtcamInfoPtr->usedRulesBitmapArr[orig_logicalIndex>>5] &= ~(1<<(orig_logicalIndex & 0x1f));
    }

    /* update the counter */
    orig_usedRulesAmount = vtcamInfoPtr->usedRulesAmount;
    vtcamInfoPtr->usedRulesAmount--;

    /* 3. re-calculate the new logical index ... now it must succeed
        (we have empty place according to DB) */
    /* Get logical index for the moved rule .
       Get place for new rule.
       If needed move existing rules.
       If needed resize vTCAM.
    */
    rc = priorityAddNewRuleNeedNewLogicalIndex(
        vTcamMngId,
        vTcamId,
        ORIG_INDEX_NOT_USED_CNS,/*origIndexForPriorityUpdate*/
        priority,
        position,
        &new_logicalIndex
    );
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    if(orig_usedRulesAmount != (vtcamInfoPtr->usedRulesAmount + 1))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    new_deviceRuleIndex = vtcamInfoPtr->rulePhysicalIndexArr[new_logicalIndex];
    CPSS_LOG_INFORMATION_MAC("ruleId[%d] got new logical index[%d] and new HW index[%d] \n",
        ruleId,
        new_logicalIndex,
        new_deviceRuleIndex);

    /* 4. copy raw entry (pattern,mask,action) from SW buffer to HW.*/
    rc = rawEntryFormatOperation(vTcamMngId,vtcamInfoPtr,
        RAW_ENTRY_FORMAT_OPERATION_DOWNLOAD_SW_TO_HW_E,
        new_deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* update the DB about the new logical index */
    vtcamInfoPtr->usedRulesBitmapArr[new_logicalIndex>>5] |= (1<<(new_logicalIndex & 0x1f));

    /* update the DB about the 'new' rule */
    rc = priorityNewRuleIdAddedDbUpdate(vTcamMngId,vTcamId,
        ruleId,
        new_logicalIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }


    /*restore*/
    vtcamInfoPtr->usedRulesAmount = orig_usedRulesAmount;

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChVirtualTcamRulePriorityUpdate
*
* DESCRIPTION:
*       Update rule priority (for priority driven vTCAM).
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - rule id: that used to refer existing rule
*       priority         - the new priority of the rule
*       position         - position in range of rule of the
*                          specified priority (see enum)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_FULL                  - when the TCAM (not only the vTcam) is FULL
*                                  and moving the rule will require the entry to
*                                  be absent from the HW for some time.
*                                  (the function can not guarantee loss less hits
*                                   on that rule)
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamRulePriorityUpdate
(
    IN   GT_U32                                                    vTcamMngId,
    IN   GT_U32                                                    vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                            ruleId,
    IN   GT_U32                                                    priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC  *vTcamMngDBPtr;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U8   devNum;
    GT_U32  orig_deviceRuleIndex;
    GT_U32  orig_logicalIndex; /* logical index that converted from ruleId */

    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry;
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    GT_U32  new_deviceRuleIndex;/* new HW index that will be for the ruleId */
    GT_U32  new_logicalIndex; /* new logical index that will be for the ruleId */
    GT_BOOL isAutoResize;
    GT_BOOL tableIsFull = GT_FALSE;
    GT_U32  actualResizeNumRules = 0;/* relevant when tableIsFull == GT_TRUE */

    if(priority >= BIT_16)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"priority out of range 64K [%d]",
            priority);
    }

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod !=
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"the function relevant only to 'priority' mode");
    }

    /* check priority */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_PRIORITY_PARAM_CHECK_MAC(priority);
    /* check input param */
    switch(position)
    {
        case CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_FIRST_E:
        case CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_LAST_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"Unknown position[%d] parameter",
                position);
    }

    /* check validity of the ruleId */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &orig_logicalIndex,&orig_deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    /* get the priority of the orig ruleId */
    rc = prvCpssDxChVirtualTcamDbPriorityTableEntryFind(
        vtcamInfoPtr,
        orig_logicalIndex,
        PRV_CPSS_AVL_TREE_FIND_MAX_LESS_OR_EQUAL_E,/*findType*/
        avlTreePath,
        &priorityEntry
    );
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    CPSS_LOG_INFORMATION_MAC("In the DB : the ruleId[%d] hold priority[%d] \n",
        ruleId,
        priorityEntry.priority);

    if(priorityEntry.baseLogIndex > orig_logicalIndex ||
       ((priorityEntry.baseLogIndex + priorityEntry.rangeSize) <= orig_logicalIndex))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"DB error: the orig logical index [%d] is out of the 'orig' priority range[%d..%d]",
            orig_logicalIndex,
            priorityEntry.baseLogIndex,
            priorityEntry.baseLogIndex + priorityEntry.rangeSize - 1);
    }

    if(vtcamInfoPtr->rulesAmount == vtcamInfoPtr->usedRulesAmount)
    {
        tableIsFull = GT_TRUE;
    }
    else
    {
        tableIsFull = GT_FALSE;
    }

    if(tableIsFull == GT_TRUE)
    {
        /* increase the table by 1 for the new priority*/
        rc = cpssDxChVirtualTcamResize(vTcamMngId,vTcamId,
            priority,GT_TRUE/*toInsert*/,1/*sizeInRules*/);

        /* don't care about rc !
            even if it failed ... the values of
                vtcamInfoPtr->rulesAmount , vtcamInfoPtr->usedRulesAmount
            are ok
        */
        actualResizeNumRules = vtcamInfoPtr->rulesAmount -
                               vtcamInfoPtr->usedRulesAmount;
    }

    /* save the value */
    isAutoResize = vtcamInfoPtr->tcamInfo.autoResize;
    /* FORCE the operation to not do any auto resize */
    vtcamInfoPtr->tcamInfo.autoResize = GT_FALSE;

    /* Get logical index for the moved rule .
       Get place for new rule.
       If needed move existing rules.
       If needed resize vTCAM.
    */
    rc = priorityAddNewRuleNeedNewLogicalIndex(
        vTcamMngId,
        vTcamId,
        orig_logicalIndex,/*origIndexForPriorityUpdate*/
        priority,
        position,
        &new_logicalIndex
    );
    /*NOTE: the 'rc' will be checked later !!!
      do not return from here !!!!*/

    if((rc == GT_FULL) && (tableIsFull == GT_TRUE) && (actualResizeNumRules == 0))
    {
        /* the table is full and FORCED resize didn't helped */

        /*
        * when the TCAM (not only the vTcam) is FULL
        * and moving the rule will require the entry to
        * be absent from the HW for some time.
        * (the function can not guarantee loss less hits
        *  on that rule)
        */

        if(allowRuleToBeAbsentFromTcamDuringPriorityUpdate == GT_TRUE)
        {
            rc = vtcamFullMoveRuleByRawFormat(vTcamMngId,vTcamId,
                vtcamInfoPtr,ruleId,priority,position,
                orig_deviceRuleIndex,orig_logicalIndex);
        }
        else
        {
            rc = GT_FULL;
        }

        /* restore value */
        vtcamInfoPtr->tcamInfo.autoResize = isAutoResize;

        return /* this is NOT error ! it valid case that we can't support */ rc;
    }

    /* restore value */
    vtcamInfoPtr->tcamInfo.autoResize = isAutoResize;

    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    /* convert AGAIN the ruleId to logical index and to physical index
       because the logic may have moved it (when trying to find place for the new priority)
    */
    rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        &orig_logicalIndex,&orig_deviceRuleIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    new_deviceRuleIndex = vtcamInfoPtr->rulePhysicalIndexArr[new_logicalIndex];
    CPSS_LOG_INFORMATION_MAC("ruleId[%d] got new logical index[%d] and new HW index[%d] \n",
        ruleId,
        new_logicalIndex,
        new_deviceRuleIndex);

    CPSS_LOG_INFORMATION_MAC("start removing the original logical index [%d] for the rule \n",
        orig_logicalIndex);

    if(orig_logicalIndex != new_logicalIndex)
    {
        /* remove the old rule according to logical index */
        rc = priorityDbRemoveRuleId(vTcamMngDBPtr, vtcamInfoPtr, orig_logicalIndex);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        /* only Remove rule Id entry by logical index from tree : ruleIdIndexTree */
        rc = prvCpssDxChVirtualTcamDbRuleIdTableEntryRemoveByLogicalIndex(
                vTcamMngDBPtr,
                vtcamInfoPtr,
                orig_logicalIndex);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    /* remove the original logical index from the DB */
    vtcamInfoPtr->usedRulesBitmapArr[orig_logicalIndex>>5] &= ~(1<<(orig_logicalIndex & 0x1f));

    CPSS_LOG_INFORMATION_MAC("start adding the new logical index [%d] for the rule \n",
        new_logicalIndex);

    if(orig_deviceRuleIndex != new_deviceRuleIndex)
    {
        /* get first devNum iterator */
        rc = prvCpssDxChVirtualTcamDbDeviceIteratorGetFirst(vTcamMngId,&devNum);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* iteration over the devices */
        do
        {
            /* move the rule in the device */
            rc = perDevice_ruleMove(devNum,vtcamInfoPtr->tcamInfo.ruleSize,
                        orig_deviceRuleIndex,new_deviceRuleIndex,GT_TRUE/*move*/);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        /* get next devNum iterator */
        while(prvCpssDxChVirtualTcamDbDeviceIteratorGetNext(vTcamMngId,&devNum));
    }

    /* update the DB about the new logical index */
    vtcamInfoPtr->usedRulesBitmapArr[new_logicalIndex>>5] |= (1<<(new_logicalIndex & 0x1f));

    /* update the DB about the 'new' rule */
    rc = priorityNewRuleIdAddedDbUpdate(vTcamMngId,vTcamId,
        ruleId,
        new_logicalIndex);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
    }

    if(actualResizeNumRules && (tableIsFull == GT_TRUE))
    {
        /* decrease the table by the value that we caused to add */
        rc = cpssDxChVirtualTcamResize(vTcamMngId,vTcamId,
            priority,GT_FALSE/*toInsert*/,actualResizeNumRules/*sizeInRules*/);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRulePriorityUpdate
*
* DESCRIPTION:
*       Update rule priority (for priority driven vTCAM).
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*       vTcamId          - unique Id of  Virtual TCAM
*       ruleId           - rule id: that used to refer existing rule
*       priority         - the new priority of the rule
*       position         - position in range of rule of the
*                          specified priority (see enum)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_FULL                  - when the TCAM (not only the vTcam) is FULL
*                                  and moving the rule will require the entry to
*                                  be absent from the HW for some time.
*                                  (the function can not guarantee loss less hits
*                                   on that rule)
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id or rule with rileId not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRulePriorityUpdate
(
    IN   GT_U32                                                    vTcamMngId,
    IN   GT_U32                                                    vTcamId,
    IN   CPSS_DXCH_VIRTUAL_TCAM_RULE_ID                            ruleId,
    IN   GT_U32                                                    priority,
    IN   CPSS_DXCH_VIRTUAL_TCAM_EQUAL_PRIORITY_RULE_POSITION_ENT   position
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRulePriorityUpdate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, priority, position));

    rc = internal_cpssDxChVirtualTcamRulePriorityUpdate(vTcamMngId, vTcamId, ruleId, priority, position);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, priority, position));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamDbRuleIdToHwIndexConvert
*
* DESCRIPTION:
*       debug function - convert rule Id to logical index and to hardware index.
*       purpose is to allow application to know where the CPSS inserted the rule
*       in to the hardware.
*       this is 'DB' (DataBase) operation that not access the hardware.
*       NOTE: the function will return failure if the ruleId was not inserted.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - ruleId to find.
*
* OUTPUTS:
*       logicalIndexPtr  - (pointer to) the logical index.  (can be NULL)
*       hwIndexPtr       - (pointer to) the hardware index. (can be NULL)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - if both logicalIndexPtr and hwIndexPtr are NULL
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS internal_cpssDxChVirtualTcamDbRuleIdToHwIndexConvert
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_RULE_ID              ruleId,
    OUT  GT_U32                                     *logicalIndexPtr,
    OUT  GT_U32                                     *hwIndexPtr
)
{
    if(logicalIndexPtr == NULL && hwIndexPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PTR, LOG_ERROR_NO_MSG);
    }

    /* convert the ruleId to logical index and to physical index */
    return prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
        vTcamMngId,vTcamId,ruleId,
        GT_TRUE,/* check that the ruleId already exists in the DB */
        logicalIndexPtr,hwIndexPtr);
}

/*******************************************************************************
* cpssDxChVirtualTcamDbRuleIdToHwIndexConvert
*
* DESCRIPTION:
*       debug function - convert rule Id to logical index and to hardware index.
*       purpose is to allow application to know where the CPSS inserted the rule
*       in to the hardware.
*       this is 'DB' (DataBase) operation that not access the hardware.
*       NOTE: the function will return failure if the ruleId was not inserted.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - ruleId to find.
*
* OUTPUTS:
*       logicalIndexPtr  - (pointer to) the logical index.  (can be NULL)
*       hwIndexPtr       - (pointer to) the hardware index. (can be NULL)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - if both logicalIndexPtr and hwIndexPtr are NULL
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamDbRuleIdToHwIndexConvert
(
    IN  GT_U32                                      vTcamMngId,
    IN  GT_U32                                      vTcamId,
    IN   GT_U32                                     ruleId,
    OUT  GT_U32                                     *logicalIndexPtr,
    OUT  GT_U32                                     *hwIndexPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamDbRuleIdToHwIndexConvert);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, logicalIndexPtr, hwIndexPtr));

    rc = internal_cpssDxChVirtualTcamDbRuleIdToHwIndexConvert(vTcamMngId, vTcamId, ruleId, logicalIndexPtr, hwIndexPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, logicalIndexPtr, hwIndexPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cpssDxChVirtualTcamUsageGet
*
* DESCRIPTION:
*       Get Virtual TCAM usage.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       vTcamUsagePtr   - (pointer to) Virtual TCAM Usage structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamUsageGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    OUT  CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC   *vTcamUsagePtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;

    CPSS_NULL_PTR_CHECK_MAC(vTcamUsagePtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    vTcamUsagePtr->rulesUsed = vtcamInfoPtr->usedRulesAmount;
    vTcamUsagePtr->rulesFree = vtcamInfoPtr->rulesAmount - vtcamInfoPtr->usedRulesAmount;

    return GT_OK;
}
/*******************************************************************************
* cpssDxChVirtualTcamUsageGet
*
* DESCRIPTION:
*       Get Virtual TCAM usage.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       vTcamUsagePtr   - (pointer to) Virtual TCAM Usage structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamUsageGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    OUT  CPSS_DXCH_VIRTUAL_TCAM_USAGE_STC   *vTcamUsagePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamUsageGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, vTcamUsagePtr));

    rc = internal_cpssDxChVirtualTcamUsageGet(vTcamMngId, vTcamId, vTcamUsagePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, vTcamUsagePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamInfoGet
*
* DESCRIPTION:
*       Get Virtual TCAM info.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       vTcamInfoPtr    - (pointer to) Virtual TCAM Info structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamInfoGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    OUT  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC    *vTcamInfoPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;

    CPSS_NULL_PTR_CHECK_MAC(vTcamInfoPtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, "vTcamMngId[%d] : vTcamId[%d] not exists",
            vTcamMngId,vTcamId);
    }

    *vTcamInfoPtr = vtcamInfoPtr->tcamInfo;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamInfoGet
*
* DESCRIPTION:
*       Get Virtual TCAM info.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       vTcamInfoPtr    - (pointer to) Virtual TCAM Info structure
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_FOUND             - vTcam with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamInfoGet
(
    IN   GT_U32                             vTcamMngId,
    IN   GT_U32                             vTcamId,
    OUT  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC    *vTcamInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamInfoGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, vTcamInfoPtr));

    rc = internal_cpssDxChVirtualTcamInfoGet(vTcamMngId, vTcamId, vTcamInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, vTcamInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);


    return rc;
}


/*******************************************************************************
* internal_cpssDxChVirtualTcamCreate
*
* DESCRIPTION:
*       Create Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - Virtual TCAM Manager Id
*                        (APPLICABLE RANGES: 0..31)
*       vTcamId        - unique Id of  Virtual TCAM
*       vTcamInfoPtr   - (pointer to) Virtual TCAM info structure
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_ALREADY_EXIST         - vTcam with given Id exists
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamCreate
(
    IN  GT_U32                            vTcamMngId,
    IN  GT_U32                            vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC   *vTcamInfoPtr
)
{
    GT_STATUS                                   rc;               /* return code */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr;   /* TCAM manager */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *tcamCfgPtr;      /* vTCAM*/
    GT_U32                                      bmpSize;          /* size of used rules BMP */

    if(vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        /* vTcamMngId out of range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if (vTcamMngDBPtr == NULL)
    {
        /* can't find the vTcamMng DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    if (vTcamId >= vTcamMngDBPtr->vTcamCfgPtrArrSize)
    {
        /* vTcamId out of range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if (vTcamMngDBPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
    {
        if (vTcamInfoPtr->clientGroup != 0)
        {
            /* Only group #0 (PCL) is currently allowed on this device family */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (vTcamInfoPtr->hitNumber != 0)
        {
            /* Parallel lookups not allowed on this device family */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        switch (vTcamInfoPtr->ruleSize)
        {
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E:
            case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E:
                /* OK */
                break;
            default:
                /* Invalid size on this device family */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                break;
        }
    }

    if (vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] != NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
    }

    vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] =
        (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC*)
        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC));

    if (vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
    }

    tcamCfgPtr = vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId];
    cpssOsMemSet(tcamCfgPtr, 0, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC));

    /* save the caller info */
    tcamCfgPtr->tcamInfo = *vTcamInfoPtr;

    /* allocation of trees */
    /*Create Local per-vTcam Access Trees for Rule Id Table.*/
    rc = prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesCreate(
        vTcamMngDBPtr,  tcamCfgPtr);
    if (rc != GT_OK)
    {
        cpssOsFree(tcamCfgPtr);
        vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] = NULL;
        return rc;
    }
    /*Create Local per-vTcam Access Trees for Priority Table.*/
    rc = prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesCreate(
        vTcamMngDBPtr,  tcamCfgPtr);
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete(
            vTcamMngDBPtr,  tcamCfgPtr);
        cpssOsFree(tcamCfgPtr);
        vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] = NULL;
        return rc;
    }

    /*Allocate memory for Virtual TCAM in Segment Table for TCAM.*/
    rc = vtcamDbSegmentTableVTcamAllocOrResize(
        vTcamMngDBPtr, vTcamId,
        0/* no realloc*/,NULL/*NA*/);
    if (rc != GT_OK)
    {
        prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete(
            vTcamMngDBPtr,  tcamCfgPtr);
        prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete(
            vTcamMngDBPtr,  tcamCfgPtr);
        cpssOsFree(tcamCfgPtr);
        vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] = NULL;
        return rc;
    }

    if (vTcamInfoPtr->guaranteedNumOfRules)
    {
        bmpSize = (((vTcamInfoPtr->guaranteedNumOfRules + 31) / 32) * sizeof(GT_U32));
        tcamCfgPtr->usedRulesBitmapArr = (GT_U32*)cpssOsMalloc(bmpSize);
        if (tcamCfgPtr->usedRulesBitmapArr == NULL)
        {
            prvCpssDxChVirtualTcamDbSegmentTableVTcamFree(
                vTcamMngDBPtr,  tcamCfgPtr);
            prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete(
                vTcamMngDBPtr,  tcamCfgPtr);
            prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete(
                vTcamMngDBPtr,  tcamCfgPtr);
            cpssOsFree(tcamCfgPtr);
            vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] = NULL;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
        /*reset the allocated memory */
        cpssOsMemSet(tcamCfgPtr->usedRulesBitmapArr,0,bmpSize);
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamCreate
*
* DESCRIPTION:
*       Create Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - Virtual TCAM Manager Id
*                        (APPLICABLE RANGES: 0..31)
*       vTcamId        - unique Id of  Virtual TCAM
*       vTcamInfoPtr   - (pointer to) Virtual TCAM info structure
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_ALREADY_EXIST         - vTcam with given Id exists
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamCreate
(
    IN  GT_U32                           vTcamMngId,
    IN  GT_U32                           vTcamId,
    IN  CPSS_DXCH_VIRTUAL_TCAM_INFO_STC   *vTcamInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamCreate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, vTcamInfoPtr));

    rc = internal_cpssDxChVirtualTcamCreate(vTcamMngId, vTcamId, vTcamInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, vTcamInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamRemove
*
* DESCRIPTION:
*       Remove Virtual TCAM.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - vTcam contains valid rules
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS internal_cpssDxChVirtualTcamRemove
(
    IN  GT_U32                             vTcamMngId,
    IN  GT_U32                             vTcamId
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC          *vTcamMngDBPtr;   /* TCAM manager */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *tcamCfgPtr;      /* vTCAM*/

    if(vTcamMngId > CPSS_DXCH_VIRTUAL_TCAM_MNG_MAX_ID_CNS)
    {
        /* vTcamMngId out of range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"vTcamMngId %d out of range", vTcamMngId);
    }

    vTcamMngDBPtr = vTcamMngDB[vTcamMngId];
    if (vTcamMngDBPtr == NULL)
    {
        /* can't find the vTcamMng DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND,"Can't find the vTcamMng DB");
    }

    if (vTcamId >= vTcamMngDBPtr->vTcamCfgPtrArrSize)
    {
        /* vTcamId out of range */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"vTcamId %d out of range", vTcamId);
    }

    if (vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND,"Can't find the vTcam [%d] in DB",vTcamId);
    }

    tcamCfgPtr = vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId];
    if (tcamCfgPtr->usedRulesAmount)
    {
        /* vTcam contains valid rules */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,"vTcam[%d] contains [%d]valid rules",
            vTcamId,tcamCfgPtr->usedRulesAmount);
    }

    /* free rule ID tree */
    rc = prvCpssDxChVirtualTcamDbRuleIdTableVTcamTreesDelete(
        vTcamMngDBPtr,  tcamCfgPtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete failed");
    }
    /* free priority trees */
    rc = prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete(
        vTcamMngDBPtr,  tcamCfgPtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChVirtualTcamDbPriorityTableVTcamTreesDelete failed");
    }
    /* free segement table tree */
    rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamFree(
        vTcamMngDBPtr,  tcamCfgPtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc,"prvCpssDxChVirtualTcamDbSegmentTableVTcamFree failed");
    }

    /* free rule state bitmap */
    if (tcamCfgPtr->usedRulesBitmapArr != NULL)
    {
        cpssOsFree(tcamCfgPtr->usedRulesBitmapArr);
    }

    /* free TCAM manager */
    cpssOsFree(vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId]);
    vTcamMngDBPtr->vTcamCfgPtrArr[vTcamId] = 0;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamRemove
*
* DESCRIPTION:
*       Remove Virtual TCAM.
*
* APPLICABLE DEVICES:
*         xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat;Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId   - Virtual TCAM Manager Id
*                      (APPLICABLE RANGES: 0..31)
*       vTcamId      - unique Id of  Virtual TCAM
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - vTcam contains valid rules
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device in devListArr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamRemove
(
    IN  GT_U32                             vTcamMngId,
    IN  GT_U32                             vTcamId
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamRemove);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId));

    rc = internal_cpssDxChVirtualTcamRemove(vTcamMngId, vTcamId);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamResize
*
* DESCRIPTION:
*       Resize Virtual TCAM. Guaranteed amount of rules updated.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - Virtual TCAM Manager Id
*                        (APPLICABLE RANGES: 0..31)
*       vTcamId        - unique Id of  Virtual TCAM
*       rulePlace      - place to insert/remove space after it.
*                        for logical-index driven vTCAMs - logical-index,
*                        for priority driven vTCAMs - priority value.
*                        ----------------------------------------
*                        For logical-index driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after logical-index == rulePlace.
*                             Logical indexes of existing rules higher than inserted space
*                             (i.e. rules with old-logical-index >= rulePlace)
*                             are increased by <sizeInRules>.
*                        b) toInsert == GT_FALSE
*                             Function invalidates existing rules with logical indexes
*                             between <rulePlace> and <rulePlace + sizeInRules - 1>.
*                             This space became free. Function decreases logical indexes of
*                             existing rules higher than freed space
*                             (i.e. rules with old-logical-index >= rulePlace) by <sizeInRules>.
*                        ----------------------------------------
*                        For priority driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                        b) toInsert == GT_FALSE
*                             Function removes and frees space of <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                             No rules are removed, but rules can be moved in TCAM
*                             to gather free spaces to one chunk .
*                             Rules with given and lower priorities can be compressed
*                             using free spaces between them.
*                        ----------------------------------------
*                        CPSS_DXCH_VIRTUAL_TCAM_APPEND_CNS - used
*                        for logical-index driven vTCAMs only
*                        to append free space (i.e toInsert == GT_TRUE)
*       toInsert       - GT_TRUE - insert rule-places, GT_FALSE - remove rule-places
*                        logical index of rules up to inserted space increased,
*                        logical index of rules up to removed space - decreased,
*                        rules inside removed space - removed.
*       sizeInRules    - amount of rule-space to insert/remove
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       Example1 (logical-index driven vTCAM):
*           Old vTcam size = 200, rulePlace = 50, toInsert = GT_TRUE, sizeInRules = 100
*           New vTam size is 300.
*           Rules 0-49 logical index unchanged.
*           Rules 50-199 logical index increased by 100 to be 150-299.
*           Place for rules 50-149 added, contains no valid rules.
*       Example2 (logical-index driven vTCAM)
*           Old vTcam size = 300, rulePlace = 50, toInsert = GT_FALSE, sizeInRules = 100
*           New vTam size is 200.
*           Rules 0-49 logical index unchanged.
*           Rules 150-299 logical index decreased by 100 to be 50-199.
*           Place of rules 50-149 removed with all rules that contained.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamResize
(
    IN  GT_U32                                vTcamMngId,
    IN  GT_U32                                vTcamId,
    IN  GT_U32                                rulePlace,
    IN  GT_BOOL                               toInsert,
    IN  GT_U32                                sizeInRules
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_PRIORITY_ENTRY_STC    priorityEntry;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U32      baseRuleIndex;
    GT_U32      priority;
    RESIZE_MODE_ENT resizeMode;

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"vTcamId[%d] was not created \n",
            vTcamId);
    }

    /* check that at least one device exists , otherwise return error */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_DEVS_BITMAP_IS_EMPTY_MAC(vTcamMngId);

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E)
    {
        if(rulePlace == CPSS_DXCH_VIRTUAL_TCAM_BEFORE_START_CNS && (toInsert == GT_TRUE))
        {
            /* 'before' first logical index */
            baseRuleIndex = 0;
            resizeMode = RESIZE_MODE_WITH_INPUT_INDEX_E;/*include move of baseRuleIndex */
        }
        else
        if(rulePlace == CPSS_DXCH_VIRTUAL_TCAM_APPEND_CNS && (toInsert == GT_TRUE))
        {
            /* 'after' last logical index */
            baseRuleIndex = vtcamInfoPtr->rulesAmount;
            resizeMode = RESIZE_MODE_WITHOUT_INPUT_INDEX_E;/*add from baseRuleIndex */
        }
        else
        {
            if(rulePlace >= vtcamInfoPtr->rulesAmount)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* 'after' current logical index */
            baseRuleIndex = rulePlace;
            resizeMode = RESIZE_MODE_WITH_INPUT_INDEX_E;/*include move of baseRuleIndex */
        }

        if(toInsert == GT_FALSE)
        {
            if((baseRuleIndex + sizeInRules) > vtcamInfoPtr->rulesAmount)
            {
                /* try to remove rules beyond the range of logical indexes */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }

    }
    else /* priority mode */
    {
        priority = rulePlace;

        if(priority >= BIT_16)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,"priority out of range 64K [%d]",
                priority);
        }

        /* find the info for this priority */
        rc = prvCpssDxChVirtualTcamDbPriorityTableEntryGet(
            vtcamInfoPtr,priority,&priorityEntry);
        if(rc != GT_OK)
        {
            return rc;
        }

        baseRuleIndex = priorityEntry.baseLogIndex + priorityEntry.rangeSize;
        resizeMode = RESIZE_MODE_WITH_INPUT_INDEX_E;/*include move of baseRuleIndex */
    }

    if(sizeInRules == 0)
    {
        return GT_OK;
    }

    if(toInsert == GT_FALSE)
    {
        if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
            CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E)
        {
            rc = vtcamDbSegmentTableVTcamDownSize__LogicalIndexMode(vTcamMngId ,vTcamId,
                sizeInRules, baseRuleIndex);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }
        }
        else
        {
            rc = vtcamDbSegmentTableVTcamDownSize__PriorityMode(vTcamMngId ,vTcamId,
                sizeInRules);
            if(prvCpssDxChVirtualTcamDbResizeHwIndexDbArr != NULL)
            {
                cpssOsFree(prvCpssDxChVirtualTcamDbResizeHwIndexDbArr);
                prvCpssDxChVirtualTcamDbResizeHwIndexDbArr = NULL;
                prvCpssDxChVirtualTcamDbNumEntries_resizeHwIndexDbArr = 0;
            }
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
            }
        }
    }
    else
    {
        /* resize memory for Virtual TCAM in Segment Table for TCAM.*/
        rc = vtcamDbSegmentTableVTcamResize(vTcamMngId ,vTcamId,
            sizeInRules, resizeMode , baseRuleIndex);
        if(prvCpssDxChVirtualTcamDbResizeHwIndexDbArr != NULL)
        {
            cpssOsFree(prvCpssDxChVirtualTcamDbResizeHwIndexDbArr);
            prvCpssDxChVirtualTcamDbResizeHwIndexDbArr = NULL;
            prvCpssDxChVirtualTcamDbNumEntries_resizeHwIndexDbArr = 0;
        }
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamResize
*
* DESCRIPTION:
*       Resize Virtual TCAM. Guaranteed amount of rules updated.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId     - Virtual TCAM Manager Id
*                        (APPLICABLE RANGES: 0..31)
*       vTcamId        - unique Id of  Virtual TCAM
*       rulePlace      - place to insert/remove space after it.
*                        for logical-index driven vTCAMs - logical-index,
*                        for priority driven vTCAMs - priority value.
*                        ----------------------------------------
*                        For logical-index driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after logical-index == rulePlace.
*                             Logical indexes of existing rules higher than inserted space
*                             (i.e. rules with old-logical-index >= rulePlace)
*                             are increased by <sizeInRules>.
*                        b) toInsert == GT_FALSE
*                             Function invalidates existing rules with logical indexes
*                             between <rulePlace> and <rulePlace + sizeInRules - 1>.
*                             This space became free. Function decreases logical indexes of
*                             existing rules higher than freed space
*                             (i.e. rules with old-logical-index >= rulePlace) by <sizeInRules>.
*                        ----------------------------------------
*                        For priority driven vTCAMs:
*                        a) toInsert == GT_TRUE
*                             Function adds free space with size <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                        b) toInsert == GT_FALSE
*                             Function removes and frees space of <sizeInRules> rules
*                             after all rules with priority == rulePlace.
*                             No rules are removed, but rules can be moved in TCAM
*                             to gather free spaces to one chunk .
*                             Rules with given and lower priorities can be compressed
*                             using free spaces between them.
*                        ----------------------------------------
*                        CPSS_DXCH_VIRTUAL_TCAM_APPEND_CNS - used
*                        for logical-index driven vTCAMs only
*                        to append free space (i.e toInsert == GT_TRUE)
*       toInsert       - GT_TRUE - insert rule-places, GT_FALSE - remove rule-places
*                        logical index of rules up to inserted space increased,
*                        logical index of rules up to removed space - decreased,
*                        rules inside removed space - removed.
*       sizeInRules    - amount of rule-space to insert/remove
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_RESOURCE           - on tree/buffers resource errors
*       GT_FULL                  - the TCAM space is full for current allocation
*                                   request
*
* COMMENTS:
*       Example1 (logical-index driven vTCAM):
*           Old vTcam size = 200, rulePlace = 50, toInsert = GT_TRUE, sizeInRules = 100
*           New vTam size is 300.
*           Rules 0-49 logical index unchanged.
*           Rules 50-199 logical index increased by 100 to be 150-299.
*           Place for rules 50-149 added, contains no valid rules.
*       Example2 (logical-index driven vTCAM)
*           Old vTcam size = 300, rulePlace = 50, toInsert = GT_FALSE, sizeInRules = 100
*           New vTam size is 200.
*           Rules 0-49 logical index unchanged.
*           Rules 150-299 logical index decreased by 100 to be 50-199.
*           Place of rules 50-149 removed with all rules that contained.
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamResize
(
    IN  GT_U32                                vTcamMngId,
    IN  GT_U32                                vTcamId,
    IN  GT_U32                                rulePlace,
    IN  GT_BOOL                               toInsert,
    IN  GT_U32                                sizeInRules
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamResize);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, rulePlace, toInsert, sizeInRules));

    rc = internal_cpssDxChVirtualTcamResize(vTcamMngId, vTcamId, rulePlace, toInsert, sizeInRules);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, rulePlace, toInsert, sizeInRules));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChVirtualTcamNextRuleIdGet
*
* DESCRIPTION:
*        Function gets next existing rule ID.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*                           CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS used to start iteration of rules
*
* OUTPUTS:
*       nextRuleIdPtr     -      (pointer to)id of next rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_EMPTY                 - on 'START_RULE_ID' indication and no rules exists.
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_MORE               - all rule already passed by iteration
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamNextRuleIdGet
(
    IN  GT_U32        vTcamMngId,
    IN  GT_U32        vTcamId,
    IN  GT_U32        ruleId,
    OUT GT_U32        *nextRuleIdPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;
    GT_U32  logicalIndex; /* logical index that converted from ruleId */
    GT_U32  nextLogicalIndex;/* next logical index*/
    PRV_CPSS_AVL_TREE_PATH      avlTreePath;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    GT_BOOL    found;/* indication that the current ruleId found */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC    needToFindInfo;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_RULE_ID_ENTRY_STC    *foundRuleIdEntryPtr;

    CPSS_NULL_PTR_CHECK_MAC(nextRuleIdPtr);

    /* get vtcam DB */
    vtcamInfoPtr = prvCpssDxChVirtualTcamDbVTcamGet(vTcamMngId,vTcamId);
    if(vtcamInfoPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    if(vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_LOGICAL_INDEX_E)
    {
        if(ruleId == CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS)
        {
            logicalIndex = 0;
            if(vtcamInfoPtr->usedRulesBitmapArr[0] & BIT_0)
            {
                *nextRuleIdPtr = 0;
                return GT_OK;
            }
        }
        else
        {
            /* convert the ruleId to logical index */
            rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
                vTcamMngId,vTcamId,ruleId,
                GT_TRUE,/* check that the ruleId already exists in the DB */
                &logicalIndex,NULL);
            if(rc != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
            }
        }

        /* find nearest to specific logical index that is used logical index */
        rc = findNearestLogicalIndexUsed(vtcamInfoPtr,
            logicalIndex,
            GT_FALSE, /*more : lessOrMore*/
            0xFFFFFFFF,/*lastLogicalIndexToCheck*/
            &nextLogicalIndex
        );

        if(rc != GT_OK)
        {
            if(ruleId == CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS)
            {
                /* empty table */
                return /* NOT to register as error to LOG !!! */ GT_EMPTY;
            }
            else
            {
                return /* NOT to register as error to LOG !!! */ GT_NO_MORE;
            }
        }

        /* return the 'next' ruleId */
        *nextRuleIdPtr = nextLogicalIndex;

        return GT_OK;
    }

    /* vtcamInfoPtr->tcamInfo.ruleAdditionMethod ==
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ADDITION_METHOD_PRIORITY_E*/


    if(ruleId == CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS)
    {
        found = prvCpssAvlPathSeek(vtcamInfoPtr->ruleIdIndexTree,
            PRV_CPSS_AVL_TREE_SEEK_FIRST_E,
            avlTreePath,
            &dbEntryPtr /* use dedicated var to avoid warnings */);

        if(found == GT_FALSE)
        {
            /* empty table */
            return /* NOT to register as error to LOG !!! */ GT_EMPTY;
        }

        foundRuleIdEntryPtr = dbEntryPtr;
    }
    else
    {
        /* convert the ruleId to logical index */
        rc = prvCpssDxChVirtualTcamDbRuleIdToLogicalIndexConvert(
            vTcamMngId,vTcamId,ruleId,
            GT_TRUE,/* check that the ruleId already exists in the DB */
            &logicalIndex,NULL);
        if(rc != GT_OK)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
        }

        cpssOsMemSet(&needToFindInfo,0,sizeof(needToFindInfo));
        needToFindInfo.logicalIndex = logicalIndex;

        found = prvCpssAvlPathFind(vtcamInfoPtr->ruleIdIndexTree,
            PRV_CPSS_AVL_TREE_FIND_MIN_GREATER_OR_EQUAL_E,
            &needToFindInfo,/*pData*/
            avlTreePath,/*path*/
            &dbEntryPtr /* use dedicated var to avoid warnings */);

        foundRuleIdEntryPtr = dbEntryPtr;

        if(found == GT_TRUE &&
           foundRuleIdEntryPtr->logicalIndex == logicalIndex)
        {
            /* the DB found 'equal' .. but we need 'next' ,
               use the found node to get the next one ! */
            found = prvCpssAvlPathSeek(vtcamInfoPtr->ruleIdIndexTree,
                PRV_CPSS_AVL_TREE_SEEK_NEXT_E,
                avlTreePath,
                &dbEntryPtr /* use dedicated var to avoid warnings */);

            foundRuleIdEntryPtr = dbEntryPtr;

            if(found == GT_TRUE &&
               foundRuleIdEntryPtr->logicalIndex == logicalIndex)
            {
                /* the 'next' hold info of 'current' ?! */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, "DB error: the 'next' hold info of 'current' logicalIndex[%d] (ruleId) ?! ",
                    logicalIndex,ruleId);
            }
        }

        if(found == GT_FALSE)
        {
            return /* NOT to register as error to LOG !!! */ GT_NO_MORE;
        }
    }

    /* return the 'next' ruleId */
    *nextRuleIdPtr = foundRuleIdEntryPtr->ruleId;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamNextRuleIdGet
*
* DESCRIPTION:
*        Function gets next existing rule ID.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamId           - unique Id of  Virtual TCAM
*       ruleId            - for logical-index driven vTCAM - logical-index
*                           for priority driven vTCAM - ruleId that used to refer existing rule
*                           CPSS_DXCH_VIRTUAL_TCAM_START_RULE_ID_CNS used to start iteration of rules
*
* OUTPUTS:
*       nextRuleIdPtr     -      (pointer to)id of next rule
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_EMPTY                 - on 'START_RULE_ID' indication and no rules exists.
*       GT_NOT_FOUND             - vTcam with given Id not found
*       GT_NO_MORE               - all rule already passed by iteration
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamNextRuleIdGet
(
    IN  GT_U32        vTcamMngId,
    IN  GT_U32        vTcamId,
    IN  GT_U32        ruleId,
    OUT GT_U32        *nextRuleIdPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamNextRuleIdGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamId, ruleId, nextRuleIdPtr));

    rc = internal_cpssDxChVirtualTcamNextRuleIdGet(vTcamMngId, vTcamId, ruleId, nextRuleIdPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamId, ruleId, nextRuleIdPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* cpssDxChVirtualTcamDefrag
*
* DESCRIPTION:
*       Defragment TCAM manager memory.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId       - Virtual TCAM Manager Id
*                          (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamDefrag
(
    IN  GT_U32                              vTcamMngId
)
{
    vTcamMngId = vTcamMngId;

    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_IMPLEMENTED, LOG_ERROR_NO_MSG);
}

/*
 * typedef: enum VTCAM_BASIC_INFO_STC
 *
 * Description:
 *      info needed for comparing vtcams to decide optimal insert to tcam when start
   from scratch
 *
 * Enumerations:
 *
 *     vTcamId     - vtcamId
 *     ruleSize    - ruleSize
 *     newOrAddedNumRules - new or added number of rules
 *     newVtcam    - new/existing vtcam indication
 *     newTcamInfo - new vtcam info
 */
typedef struct
{
    GT_U32                               vTcamId;
    CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_ENT ruleSize;
    GT_U32                               newOrAddedNumRules;
    GT_BOOL                              newVtcam;
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC      newTcamInfo;
} VTCAM_BASIC_INFO_STC;

/*******************************************************************************
* compareVtcamsForInsertOrder
*
* DESCRIPTION:
*       callback function to help to build 'tree' of : best layout depend on
*       vtcam rule size insert order.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       ptr1       - 'left' leaf
*       ptr2       - 'right' leaf
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_GREATER            - ptr1 > ptr2
*       GT_SMALLER            - ptr1 < ptr2
*       GT_EQUAL              - ptr1 == ptr2
*
* COMMENTS:
*
*******************************************************************************/
static GT_COMP_RES compareVtcamsForInsertOrder
(
    IN  VTCAM_BASIC_INFO_STC* ptr1,
    IN  VTCAM_BASIC_INFO_STC* ptr2
)
{
    if (ptr1->newTcamInfo.clientGroup > ptr2->newTcamInfo.clientGroup)
        return GT_GREATER;
    if (ptr1->newTcamInfo.clientGroup < ptr2->newTcamInfo.clientGroup)
        return GT_SMALLER;

    if (ptr1->newTcamInfo.hitNumber > ptr2->newTcamInfo.hitNumber)
        return GT_GREATER;
    if (ptr1->newTcamInfo.hitNumber < ptr2->newTcamInfo.hitNumber)
        return GT_SMALLER;

    if (ptr1->ruleSize > ptr2->ruleSize)
        return GT_GREATER;
    if (ptr1->ruleSize < ptr2->ruleSize)
        return GT_SMALLER;

    if (ptr1->newOrAddedNumRules > ptr2->newOrAddedNumRules)
        return GT_GREATER;
    if (ptr1->newOrAddedNumRules < ptr2->newOrAddedNumRules)
        return GT_SMALLER;

    if (ptr1->vTcamId > ptr2->vTcamId)
        return GT_GREATER;
    if (ptr1->vTcamId < ptr2->vTcamId)
        return GT_SMALLER;

    return GT_EQUAL;
}

/*******************************************************************************
* createVtcamsInsertOrderTree
*
* DESCRIPTION:
*       create pool and tree for 'best vtcam insert order' from scratch
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       maxNumOfVtcams  - max number of vtcams to be in the tree
*
* OUTPUTS:
*       vTcamsInsertOrderPoolDataPtr - (pointer to) the created pool-Id for data in tree.
*       vTcamsInsertOrderPoolNodesPtr - (pointer to) the created pool-Id for nodes in tree.
*       vTcamsInsertOrderTreePtr - (pointer to) the created tree-Id.
*
* RETURNS :
*       GT_OK - For successful operation.
*       GT_NO_RESOURCE - No memory is available to create that pool/tree.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS createVtcamsInsertOrderTree
(
    IN GT_U32                        maxNumOfVtcams,
    OUT CPSS_BM_POOL_ID             *vTcamsInsertOrderPoolDataPtr,
    OUT CPSS_BM_POOL_ID             *vTcamsInsertOrderPoolNodesPtr,
    OUT PRV_CPSS_AVL_TREE_ID        *vTcamsInsertOrderTreePtr
)
{
    GT_STATUS rc;

    *vTcamsInsertOrderPoolDataPtr = NULL;
    *vTcamsInsertOrderPoolNodesPtr = NULL;
    *vTcamsInsertOrderTreePtr = NULL;

    rc = cpssBmPoolCreate(
        sizeof(VTCAM_BASIC_INFO_STC),
        CPSS_BM_POOL_4_BYTE_ALIGNMENT_E,
         maxNumOfVtcams,
        vTcamsInsertOrderPoolDataPtr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "failed on cpssBmPoolCreate : vTcamsInsertOrderPoolData \n");
    }

    /* pool for buffers that used in tree :vTcamsInsertOrderTree */
    rc = prvCpssAvlMemPoolCreate(maxNumOfVtcams,vTcamsInsertOrderPoolNodesPtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "failed on prvCpssAvlMemPoolCreate : vTcamsInsertOrderPoolNodes \n");
    }

    /* tree for insert order */
    rc = prvCpssAvlTreeCreate(
        (GT_INTFUNCPTR)compareVtcamsForInsertOrder,
        *vTcamsInsertOrderPoolNodesPtr,
        vTcamsInsertOrderTreePtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, "failed on prvCpssAvlTreeCreate : vTcamsInsertOrderTree \n");
    }

    return GT_OK;
}
/*******************************************************************************
* deleteVtcamsInsertOrderTree
*
* DESCRIPTION:
*       delete pool and tree for 'best vtcam insert order'
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamsInsertOrderPoolData - the pool-Id for data in tree.
*       vTcamsInsertOrderPoolNodes - the pool-Id for nodes in tree.
*       vTcamsInsertOrderTree - the tree-Id.
*
* OUTPUTS:
*
* RETURNS :
*       GT_OK - For successful operation.
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS deleteVtcamsInsertOrderTree
(
    IN CPSS_BM_POOL_ID             vTcamsInsertOrderPoolData,
    IN CPSS_BM_POOL_ID             vTcamsInsertOrderPoolNodes,
    IN PRV_CPSS_AVL_TREE_ID        vTcamsInsertOrderTree
)
{
    /* delete the tree with the data and the nodes */
    if(vTcamsInsertOrderTree && vTcamsInsertOrderPoolData)
    {
        prvCpssAvlTreeDelete(
            vTcamsInsertOrderTree,
            (GT_VOIDFUNCPTR)cpssBmPoolBufFree,
            (GT_VOID*)vTcamsInsertOrderPoolData);
    }

    if(vTcamsInsertOrderPoolNodes)
    {
        /* delete the pool */
        prvCpssAvlMemPoolDelete(vTcamsInsertOrderPoolNodes);
    }

    if(vTcamsInsertOrderPoolData)
    {
        /* delete the pool */
        prvCpssAvlMemPoolDelete(vTcamsInsertOrderPoolData);
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChVirtualTcamDbAllocWithOnlySegmentTable
*
* DESCRIPTION:
*       allocate vtcam with segments tree
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngPtr  - (pointer to) the TCAM manager
*       vTcamId      - the vtcam ID to allocate for
*       tcamInfoPtr  - (pointer to) the vtcam info
*       allocSkeletonOnly   - indication to allocate only skeleton
* OUTPUTS:
*       vTcamMngPtr  - (pointer to) the TCAM manager , with the added vtcam
*                       allocation at index vTcamId.
*
* RETURNS :
*       pointer to allocated vtcam memory (on NULL)
*
* COMMENTS:
*
*******************************************************************************/
PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* prvCpssDxChVirtualTcamDbAllocWithOnlySegmentTable
(
    INOUT PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC      *vTcamMngPtr,
    IN GT_U32                                   vTcamId,
    IN CPSS_DXCH_VIRTUAL_TCAM_INFO_STC         *tcamInfoPtr,
    IN    GT_BOOL                               allocSkeletonOnly
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC* vtcamInfoPtr;

    vtcamInfoPtr =
        (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC*)
        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC));

    if (vtcamInfoPtr == NULL)
    {
        return NULL;
    }

    cpssOsMemSet(vtcamInfoPtr, 0, sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC));

    vtcamInfoPtr->tcamInfo = *tcamInfoPtr;
    if(allocSkeletonOnly == GT_FALSE)
    {
        rc = prvCpssAvlTreeCreate(
            (GT_INTFUNCPTR)prvCpssDxChVirtualTcamDbSegmentTableLookupOrderCompareFunc,
            vTcamMngPtr->segNodesPool, &vtcamInfoPtr->segmentsTree);
        if (rc != GT_OK)
        {
            cpssOsFree(vtcamInfoPtr);
            return NULL;
        }
    }

    /* bind it to proper place */
    vTcamMngPtr->vTcamCfgPtrArr[vTcamId] = vtcamInfoPtr;

    return vtcamInfoPtr;
}


/*******************************************************************************
* virtualTcamMemoryAvailabilityCheck_fromScratch
*
* DESCRIPTION:
*       Check availability of TCAM manager memory for list of new and existing
*       Virtual TCAMs ... if we will try to add it from scratch
*       NOTE: existing vtcams comes from origVTcamMngPtr
*             new + updated vtcams comes from the vTcamCheckInfoArr[]
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       origVTcamMngPtr  - (pointer to) the original TCAM manager
*       newVTcamMngPtr - ((pointer to) to new TCAM manager.
*       vTcamAmount       - amount of Virtual TCAMs in the array.
*       vTcamCheckInfoArr - array of infos for Virtual TCAMs.
*
* OUTPUTS:
*
* OUTPUTS:
*
* RETURNS :
*       GT_OK - For successful operation.
*       GT_FULL - the TCAM is full and can not support all those vtcams
*       others - other errors
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS virtualTcamMemoryAvailabilityCheck_fromScratch
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC      *origVTcamMngPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC      *newVTcamMngPtr,
    IN   GT_U32                                 vTcamAmount,
    IN   CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC  vTcamCheckInfoArr[]
)
{
    GT_STATUS rc;
    PRV_CPSS_AVL_TREE_PATH  avlTreePath;
    PRV_CPSS_AVL_TREE_SEEK_ENT seekMode;
    GT_VOID_PTR                 dbEntryPtr;  /* pointer to entry in DB             */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC*  vTcamCfgPtr; /* pointer to vTcam         */
    CPSS_BM_POOL_ID             vTcamsInsertOrderPoolData = NULL;
    CPSS_BM_POOL_ID             vTcamsInsertOrderPoolNodes = NULL;
    PRV_CPSS_AVL_TREE_ID        vTcamsInsertOrderTree = NULL;
    CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC *currCheckInfoPtr;
    VTCAM_BASIC_INFO_STC        *newElemetPtr;
    VTCAM_BASIC_INFO_STC        *tmpElementPtr;
    GT_U32  vTcamId;
    GT_U32  ii;

    /* create pool and tree */
    rc = createVtcamsInsertOrderTree(origVTcamMngPtr->vTcamCfgPtrArrSize,
        &vTcamsInsertOrderPoolData,&vTcamsInsertOrderPoolNodes,&vTcamsInsertOrderTree);
    if(rc != GT_OK)
    {
        goto cleanExit_lbl;
    }

    /* add updated/new vtcams to the tree */
    CPSS_LOG_INFORMATION_MAC("add updated/new vtcams to the tree \n");
    currCheckInfoPtr = &vTcamCheckInfoArr[0];
    for(ii = 0 ; ii < vTcamAmount; ii++ , currCheckInfoPtr++)
    {
        vTcamId = currCheckInfoPtr->vTcamId;

        /* get free buffer from pool.*/
        newElemetPtr = cpssBmPoolBufGet(vTcamsInsertOrderPoolData);
        if (newElemetPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }

        newElemetPtr->vTcamId = vTcamId;
        newElemetPtr->ruleSize = currCheckInfoPtr->vTcamInfo.ruleSize;
        newElemetPtr->newTcamInfo = currCheckInfoPtr->vTcamInfo;
        /* the 'from scratch' need to ignore current number of rules as it assumes
           that no vtcam exists and no rules to add when created */
        /* new vtcam */
        newElemetPtr->newOrAddedNumRules = currCheckInfoPtr->vTcamInfo.guaranteedNumOfRules;
        newElemetPtr->newVtcam = GT_TRUE;

        if(newElemetPtr->newVtcam == GT_TRUE)
        {
            CPSS_LOG_INFORMATION_MAC("add new vTcamId[%d] : client[%d] hitNum[%d] ruleSize[%d B] rulesAmount[%d] \n",
                vTcamId ,
                newElemetPtr->newTcamInfo.clientGroup ,
                newElemetPtr->newTcamInfo.hitNumber ,
                10*(newElemetPtr->ruleSize+1) ,
                newElemetPtr->newOrAddedNumRules);
        }
        else
        {
            CPSS_LOG_INFORMATION_MAC("resize existing vTcamId[%d] : client[%d] hitNum[%d] ruleSize[%d B] ADDING memory for rulesAmount[%d] \n",
                vTcamId ,
                newElemetPtr->newTcamInfo.clientGroup ,
                newElemetPtr->newTcamInfo.hitNumber ,
                10*(newElemetPtr->ruleSize+1) ,
                newElemetPtr->newOrAddedNumRules);
        }

        /* add new/existing entry to the tree */
        rc = prvCpssAvlItemInsert(vTcamsInsertOrderTree, newElemetPtr);
        if (rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("failed on prvCpssAvlItemInsert : vTcamsInsertOrderTree (case 1) \n");
            goto cleanExit_lbl;
        }
    }

    /* add only existing vtcams that not updated to the tree */
    CPSS_LOG_INFORMATION_MAC("add only existing vtcams that not updated to the tree \n");
    for(vTcamId = 0 ; vTcamId < origVTcamMngPtr->vTcamCfgPtrArrSize ; vTcamId++)
    {
        /* from 'orig' */
        vTcamCfgPtr = origVTcamMngPtr->vTcamCfgPtrArr[vTcamId];

        if(vTcamCfgPtr == NULL)
        {
            /* not exists vtcam */
            continue;
        }

        currCheckInfoPtr = &vTcamCheckInfoArr[0];
        for(ii = 0 ; ii < vTcamAmount; ii++ , currCheckInfoPtr++)
        {
            if(currCheckInfoPtr->vTcamId == vTcamId)
            {
                /* already added to the tree */
                break;
            }
        }

        if(ii < vTcamAmount)
        {
            /* already added to the tree */
            continue;
        }

        /* get free buffer from pool.*/
        newElemetPtr = cpssBmPoolBufGet(vTcamsInsertOrderPoolData);
        if (newElemetPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_RESOURCE, LOG_ERROR_NO_MSG);
        }
        newElemetPtr->vTcamId = vTcamId;
        newElemetPtr->ruleSize = vTcamCfgPtr->tcamInfo.ruleSize;
        newElemetPtr->newTcamInfo = vTcamCfgPtr->tcamInfo;
        newElemetPtr->newOrAddedNumRules = vTcamCfgPtr->rulesAmount;
        newElemetPtr->newVtcam = GT_TRUE;

        CPSS_LOG_INFORMATION_MAC("existing vTcamId[%d] : client[%d] hitNum[%d] ruleSize[%d B] existing memory for rulesAmount[%d] \n",
            vTcamId ,
            newElemetPtr->newTcamInfo.clientGroup ,
            newElemetPtr->newTcamInfo.hitNumber ,
            10*(newElemetPtr->ruleSize+1) ,
            newElemetPtr->newOrAddedNumRules);

        rc = prvCpssAvlItemInsert(vTcamsInsertOrderTree, newElemetPtr);
        if (rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("failed on prvCpssAvlItemInsert : vTcamsInsertOrderTree (case 2) \n");
            goto cleanExit_lbl;
        }
    }

    CPSS_LOG_INFORMATION_MAC("done adding to the tree , start creating/resize vTcams \n");

    seekMode = PRV_CPSS_AVL_TREE_SEEK_LAST_E;

    while(GT_TRUE ==
        prvCpssAvlPathSeek(vTcamsInsertOrderTree,
        seekMode,
        avlTreePath,
        &dbEntryPtr /* use dedicated var to avoid warnings */))
    {
        tmpElementPtr = dbEntryPtr;
        seekMode = PRV_CPSS_AVL_TREE_SEEK_PREVIOUS_E;

        vTcamId = tmpElementPtr->vTcamId;

        /* from 'new' */
        vTcamCfgPtr = newVTcamMngPtr->vTcamCfgPtrArr[vTcamId];
        if(vTcamCfgPtr == NULL)
        {
            /* allocate vtcam with segments tree support .. in the new tcam manager */
            vTcamCfgPtr =
                prvCpssDxChVirtualTcamDbAllocWithOnlySegmentTable(newVTcamMngPtr,
                    vTcamId,
                    &tmpElementPtr->newTcamInfo,
                    GT_TRUE/* only skeleton*/);
            if (vTcamCfgPtr == NULL)
            {
                CPSS_LOG_INFORMATION_MAC("fail to allocate memory for vTCAM [%d] \n",
                    ii);
                rc = GT_OUT_OF_CPU_MEM;
                goto cleanExit_lbl;
            }
        }

        CPSS_LOG_INFORMATION_MAC("vTcamId[%d] : client[%d] hitNum[%d] Allocate memory for ruleSize[%d B] rulesAmount[%d] \n",
            vTcamId ,
            tmpElementPtr->newTcamInfo.clientGroup ,
            tmpElementPtr->newTcamInfo.hitNumber ,
            10*(tmpElementPtr->ruleSize+1) ,
            tmpElementPtr->newOrAddedNumRules);

        /* Allocate memory for Virtual TCAM in Segment Table for TCAM. */
        rc = vtcamDbSegmentTableVTcamAllocOrResize(
            newVTcamMngPtr /* from 'new' */, vTcamId,
            0/* no realloc*/,NULL/*NA*/);
        if(rc != GT_OK)
        {
            CPSS_LOG_INFORMATION_MAC("failed on vtcamDbSegmentTableVTcamAllocOrResize : on vTcamId[%d] \n",
                vTcamId);
            goto cleanExit_lbl;
        }

        CPSS_LOG_INFORMATION_MAC("vTcamId[%d] : new rulesAmount[%d] (using [%d] '10B' rules)\n",
            vTcamId , vTcamCfgPtr->rulesAmount , vTcamCfgPtr->rulesAmount * (tmpElementPtr->ruleSize+1));
    }

    rc = GT_OK;

    CPSS_LOG_INFORMATION_MAC("done creating/resize the vtcams successfully \n");

cleanExit_lbl:
    /* delete pool and tree */
    deleteVtcamsInsertOrderTree(vTcamsInsertOrderPoolData,vTcamsInsertOrderPoolNodes,vTcamsInsertOrderTree);

    return rc;
}
/*******************************************************************************
* virtualTcamMemoryAvailabilityCheck_fromCurrentState
*
* DESCRIPTION:
*       Check availability of TCAM manager memory for list of new and existing
*       Virtual TCAMs ... if we will try to add it NOW (from current state)
*       NOTE: existing vtcams comes from origVTcamMngPtr
*             new + updated vtcams comes from the vTcamCheckInfoArr[]
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       origVTcamMngPtr  - (pointer to) the original TCAM manager
*       newVTcamMngPtr - ((pointer to) to new TCAM manager.
*       vTcamAmount       - amount of Virtual TCAMs in the array.
*       vTcamCheckInfoArr - array of infos for Virtual TCAMs.
* OUTPUTS:
*
* OUTPUTS:
*
* RETURNS :
*       GT_OK - For successful operation.
*       GT_FULL - the TCAM is full and can not support all those vtcams
*       others - other errors
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS virtualTcamMemoryAvailabilityCheck_fromCurrentState
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC      *origVTcamMngPtr,
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC      *newVTcamMngPtr,
    IN   GT_U32                                 vTcamAmount,
    IN   CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC  vTcamCheckInfoArr[]
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC*  vTcamCfgPtr; /* pointer to vTcam         */
    CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC *currCheckInfoPtr;
    GT_U32  vTcamId = 0;
    GT_U32  ii;

    origVTcamMngPtr = origVTcamMngPtr;

    /* add updated/new vtcams to the tree */
    CPSS_LOG_INFORMATION_MAC("treat elements from the vTcamCheckInfoArr[] to the tcam \n");
    currCheckInfoPtr = &vTcamCheckInfoArr[0];
    for(ii = 0 ; ii < vTcamAmount; ii++ , currCheckInfoPtr++)
    {
        vTcamId = currCheckInfoPtr->vTcamId;

        /* from 'new' */
        vTcamCfgPtr = newVTcamMngPtr->vTcamCfgPtrArr[vTcamId];

        if(vTcamCfgPtr)
        {
            if(currCheckInfoPtr->vTcamInfo.guaranteedNumOfRules < vTcamCfgPtr->rulesAmount)
            {/* decreased vtcam */
                #ifdef CPSS_LOG_ENABLE
                {
                    GT_U32  neededResizeValue;/*needed resize value */

                    neededResizeValue = vTcamCfgPtr->rulesAmount - currCheckInfoPtr->vTcamInfo.guaranteedNumOfRules;

                    CPSS_LOG_INFORMATION_MAC("request to decrease existing vTcamId[%d] : client[%d] hitNum[%d] ruleSize[%d B] removing memory of rulesAmount[%d] \n",
                        vTcamId ,
                        currCheckInfoPtr->vTcamInfo.clientGroup ,
                        currCheckInfoPtr->vTcamInfo.hitNumber ,
                        10*(currCheckInfoPtr->vTcamInfo.ruleSize+1) ,
                        neededResizeValue);
                }
                #endif /*CPSS_LOG_ENABLE*/
                /* we assumes that the application will do ALL necessary action to
                   decrease this vtcam */

                /* the function will update vTcamInfoPtr->rulesAmount */
                rc = prvCpssDxChVirtualTcamDbSegmentTableDetachSpace(newVTcamMngPtr/* from 'new' */,
                    vTcamCfgPtr,vTcamId,
                    GT_TRUE,/*from the end*/
                    currCheckInfoPtr->vTcamInfo.guaranteedNumOfRules);/*numOfRulesToKeep*/
                if(rc != GT_OK)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(rc,LOG_ERROR_NO_MSG);
                }

                CPSS_LOG_INFORMATION_MAC("Actual decrease vtcam[%d] to new size[%d] \n",
                    vTcamId,
                    vTcamCfgPtr->rulesAmount);

                if(vTcamCfgPtr->rulesAmount < vTcamCfgPtr->tcamInfo.guaranteedNumOfRules)
                {
                    vTcamCfgPtr->tcamInfo.guaranteedNumOfRules = vTcamCfgPtr->rulesAmount;
                }
            }
            else /* keep 'as is' vtcam */
            if(currCheckInfoPtr->vTcamInfo.guaranteedNumOfRules == vTcamCfgPtr->rulesAmount)
            {
                CPSS_LOG_INFORMATION_MAC("request to keep size 'as is' in existing vTcamId[%d] : client[%d] hitNum[%d] ruleSize[%d B] keep rulesAmount[%d] \n",
                    vTcamId ,
                    currCheckInfoPtr->vTcamInfo.clientGroup ,
                    currCheckInfoPtr->vTcamInfo.hitNumber ,
                    10*(currCheckInfoPtr->vTcamInfo.ruleSize+1) ,
                    vTcamCfgPtr->rulesAmount);
            }
            else/* increased vtcam */
            {
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC tmp_vTcamInfo;
                PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC *alloc_vTcamInfoPtr = &tmp_vTcamInfo;
                GT_U32  neededResizeValue;/*needed resize value */

                neededResizeValue = currCheckInfoPtr->vTcamInfo.guaranteedNumOfRules -
                                        vTcamCfgPtr->rulesAmount;

                CPSS_LOG_INFORMATION_MAC("request to increase resize existing vTcamId[%d] : client[%d] hitNum[%d] ruleSize[%d B] ADDING memory for rulesAmount[%d] \n",
                    vTcamId ,
                    currCheckInfoPtr->vTcamInfo.clientGroup ,
                    currCheckInfoPtr->vTcamInfo.hitNumber ,
                    10*(currCheckInfoPtr->vTcamInfo.ruleSize+1) ,
                    neededResizeValue);

                /* allocate to temp info */
                cpssOsMemSet(&tmp_vTcamInfo,0,sizeof(tmp_vTcamInfo));

                rc =  vtcamDbSegmentTableVTcamAllocOrResize( newVTcamMngPtr /* from 'new' */,
                    vTcamId,
                    neededResizeValue,
                    &tmp_vTcamInfo);
                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("failed on vtcamDbSegmentTableVTcamAllocOrResize : on vTcamId[%d] \n",
                        vTcamId);
                    goto cleanExit_lbl;
                }

                /* free this memory as it MUST not be used */
                cpssOsFree(alloc_vTcamInfoPtr->rulePhysicalIndexArr);
                alloc_vTcamInfoPtr->rulePhysicalIndexArr = NULL;

                /* function prvCpssDxChVirtualTcamDbSegmentTableVTcamCombine will update
                   vTcamInfoPtr->rulesAmount  */

                /* bind the new segmentsTree from alloc_vTcamInfoPtr into vTcamInfoPtr */
                /* and align the info in the tree of vTcamInfoPtr->segmentsTree */
                rc = prvCpssDxChVirtualTcamDbSegmentTableVTcamCombine(newVTcamMngPtr /* from 'new' */,
                    vTcamCfgPtr,alloc_vTcamInfoPtr);

                prvCpssAvlTreeDelete(
                    alloc_vTcamInfoPtr->segmentsTree,
                    (GT_VOIDFUNCPTR)NULL,
                    (GT_VOID*)NULL);

                if(rc != GT_OK)
                {
                    CPSS_LOG_INFORMATION_MAC("failed on prvCpssDxChVirtualTcamDbSegmentTableVTcamCombine : on vTcamId[%d] \n",
                        vTcamId);
                    goto cleanExit_lbl;
                }

                CPSS_LOG_INFORMATION_MAC("Actual increase vtcam[%d] to new size[%d] \n",
                    vTcamId,
                    vTcamCfgPtr->rulesAmount);
            }
        }
        else /* new vtcam */
        {
            CPSS_LOG_INFORMATION_MAC( "add new vTcamId[%d] : client[%d] hitNum[%d] ruleSize[%d B] rulesAmount[%d] \n",
                vTcamId ,
                currCheckInfoPtr->vTcamInfo.clientGroup ,
                currCheckInfoPtr->vTcamInfo.hitNumber ,
                10*(currCheckInfoPtr->vTcamInfo.ruleSize+1) ,
                currCheckInfoPtr->vTcamInfo.guaranteedNumOfRules);

            /* allocate vtcam with segments tree support .. in the new tcam manager */
            vTcamCfgPtr =
                prvCpssDxChVirtualTcamDbAllocWithOnlySegmentTable(newVTcamMngPtr,
                    vTcamId,
                    &currCheckInfoPtr->vTcamInfo,
                    GT_TRUE/* only skeleton*/);
            if (vTcamCfgPtr == NULL)
            {
                CPSS_LOG_INFORMATION_MAC("fail to allocate memory for vTCAM [%d] \n",
                    ii);
                rc = GT_OUT_OF_CPU_MEM;
                goto cleanExit_lbl;
            }

            /* Allocate memory for Virtual TCAM in Segment Table for TCAM. */
            rc = vtcamDbSegmentTableVTcamAllocOrResize(
                newVTcamMngPtr /* from 'new' */, vTcamId,
                0/* no realloc*/,NULL/*NA*/);
            if(rc != GT_OK)
            {
                CPSS_LOG_INFORMATION_MAC("failed on vtcamDbSegmentTableVTcamAllocOrResize : on vTcamId[%d] \n",
                    vTcamId);
                goto cleanExit_lbl;
            }
        }
    }

    rc = GT_OK;

    CPSS_LOG_INFORMATION_MAC("done creating/updating the vtcams in vTcamCheckInfoArr[] successfully \n");
cleanExit_lbl:
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("failed to creating/updating all vtcams , stopped at vTcamCheckInfoArr[%d] vTcamId[%d] \n",
            ii,vTcamId);
    }

    return rc;
}

/*******************************************************************************
* deleteTcamManagerWithOnlySegmentTable
*
* DESCRIPTION:
*       delete tcam manager that was created with createTcamManagerWithOnlySegmentTable(...)
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       origVTcamMngPtr  - (pointer to) the TCAM manager
*
* OUTPUTS:
*       none
*
* RETURNS :
*       GT_OK                   - on success
*       GT_FAIL                  - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS deleteTcamManagerWithOnlySegmentTable
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC     *pVTcamMngPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii;

    rc = prvCpssDxChVirtualTcamDbSegmentTableDeleteWithVtcamsSegments(pVTcamMngPtr);

    /************************************/
    /* do memory allocation clean up before returning the ERROR code to the caller */
    /************************************/
    if(pVTcamMngPtr->vTcamCfgPtrArr)
    {
        for(ii = 0 ; ii < pVTcamMngPtr->vTcamCfgPtrArrSize ; ii++)
        {
            if(pVTcamMngPtr->vTcamCfgPtrArr[ii])
            {
                if(pVTcamMngPtr->vTcamCfgPtrArr[ii]->rulePhysicalIndexArr)
                {
                    cpssOsFree(pVTcamMngPtr->vTcamCfgPtrArr[ii]->rulePhysicalIndexArr);
                }

                cpssOsFree(pVTcamMngPtr->vTcamCfgPtrArr[ii]);
            }
        }

        cpssOsFree(pVTcamMngPtr->vTcamCfgPtrArr);
    }
    cpssOsFree(pVTcamMngPtr);

    return rc;
}

/* dummy HW access functions for Tcam Manager With Only Segment Table */

GT_STATUS dummyBlockToLookupMapFunction
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      baseRow,
    IN  GT_U32      baseColumn,
    IN  GT_U32      lookupId
)
{
    cookiePtr    = cookiePtr;
    baseRow      = baseRow;
    baseColumn   = baseColumn;
    lookupId     = lookupId;
    return GT_OK;
}

GT_STATUS dummyRuleInvalidateFunction
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      rulePhysicalIndex,
    IN  GT_U32      ruleSize
)
{
    cookiePtr         = cookiePtr;
    rulePhysicalIndex = rulePhysicalIndex;
    ruleSize          = ruleSize;
    return GT_OK;
}

GT_STATUS dummyRuleMoveFunction
(
    IN  GT_VOID     *cookiePtr,
    IN  GT_U32      srcRulePhysicalIndex,
    IN  GT_U32      dstRulePhysicalIndex,
    IN  GT_U32      ruleSize
)
{
    cookiePtr              = cookiePtr;
    srcRulePhysicalIndex   = srcRulePhysicalIndex;
    dstRulePhysicalIndex   = dstRulePhysicalIndex;
    ruleSize               = ruleSize;
    return GT_OK;
}

/*******************************************************************************
* createTcamManagerWithOnlySegmentTable
*
* DESCRIPTION:
*       create new TCAM manager with support only for 'segments table' and it is
*       based on the same parameters of the 'orig' TCAM manager.
*       with option to also copy the full segments layout of the orig TCAM manager.
*           on such option it copies : TCAM : free segments (trees , buffers)
*                                      per vtcam : used segments (tree) and rulesAmount
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       origVTcamMngPtr  - (pointer to) the original TCAM manager
*       copyOrigLayout   - indication if to copy the layout from the orig TCAM or not.
*                       GT_TRUE - new layout will be copy the orig layout from orig TCAM.
*                       GT_FALSE - new layout is empty.
*
* OUTPUTS:
*       newVTcamMngPtrPtr - (pointer to) the (pointer to) new TCAM manager.
*
* RETURNS :
*       GT_OK                   - on success
*       GT_OUT_OF_CPU_MEM       - on CPU memory allocation fail
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong parameter
*       GT_FAIL                  - on error
*       GT_NO_RESOURCE           - not enough free place in TCAM
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS createTcamManagerWithOnlySegmentTable
(
    IN PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC     *origVTcamMngPtr,
    IN GT_BOOL                                 copyOrigLayout,
    OUT PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC*   *newVTcamMngPtrPtr
)
{
    GT_STATUS   rc = GT_OK;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC *pVTcamMngPtr;
    GT_BOOL  fillSegFreeTrees;
    GT_BOOL  allocSkeletonOnly;

    *newVTcamMngPtrPtr = NULL;

    CPSS_LOG_INFORMATION_MAC("allocate new TCAM manager \n");
    pVTcamMngPtr = (PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC*)
        cpssOsMalloc(sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC));
    if(pVTcamMngPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }
    cpssOsMemSet(pVTcamMngPtr, 0,sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC));

    CPSS_LOG_INFORMATION_MAC("allocate array of vTcams for new TCAM manager \n");
    pVTcamMngPtr->vTcamCfgPtrArrSize = CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS + 1;
    pVTcamMngPtr->vTcamCfgPtrArr =
        (PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR*)cpssOsMalloc(
            sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR)
            * (CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS + 1));
    if (pVTcamMngPtr->vTcamCfgPtrArr == NULL)
    {
        CPSS_LOG_INFORMATION_MAC("failed to alloc vTcamCfgPtrArr \n");
        rc = GT_OUT_OF_CPU_MEM;
        goto cleanExit_lbl;
    }
    cpssOsMemSet(
        pVTcamMngPtr->vTcamCfgPtrArr, 0,
        (sizeof(PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_PTR)
            * (CPSS_DXCH_VIRTUAL_TCAM_VTCAM_MAX_ID_CNS + 1)));

    CPSS_LOG_INFORMATION_MAC("fill new TCAM manager base on size parameters from orig TCAM manager \n");

    fillSegFreeTrees = (copyOrigLayout == GT_TRUE) ? GT_FALSE : GT_TRUE;

    rc = prvCpssDxChVirtualTcamDbSegmentTableCreate(pVTcamMngPtr,
        &origVTcamMngPtr->tcamSegCfg, fillSegFreeTrees);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("failed on prvCpssDxChVirtualTcamDbSegmentTableCreate \n");
        goto cleanExit_lbl;
    }

    /* rebind HW-access function to dummy */
    pVTcamMngPtr->tcamSegCfg.lookupMapFuncPtr      = dummyBlockToLookupMapFunction;
    pVTcamMngPtr->tcamSegCfg.ruleInvalidateFuncPtr = dummyRuleInvalidateFunction;
    pVTcamMngPtr->tcamSegCfg.ruleMoveFuncPtr       = dummyRuleMoveFunction;

    if(copyOrigLayout == GT_TRUE)
    {
        CPSS_LOG_INFORMATION_MAC("copy to the new TCAM manager the used segments of the orig TCAM manager \n");
    }

    allocSkeletonOnly = (copyOrigLayout == GT_TRUE) ? GT_FALSE : GT_TRUE;

    rc = prvCpssDxChVirtualTcamDbSegmentTableCopy(origVTcamMngPtr,pVTcamMngPtr,allocSkeletonOnly);
    if(rc != GT_OK)
    {
        CPSS_LOG_INFORMATION_MAC("failed on prvCpssDxChVirtualTcamDbSegmentTableCopy \n");
        goto cleanExit_lbl;
    }

    if(copyOrigLayout == GT_TRUE)
    {
        CPSS_LOG_INFORMATION_MAC("ended copy to the new TCAM manager the vTCAMs used segments \n");
    }

cleanExit_lbl:
    if(rc != GT_OK)
    {
        /************************************/
        /* do memory allocation clean up before returning the ERROR code to the caller */
        /************************************/
        deleteTcamManagerWithOnlySegmentTable(pVTcamMngPtr);

        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    *newVTcamMngPtrPtr = pVTcamMngPtr;

    return GT_OK;
}

/* macro to check (and return 'bad param' if check fails) of field in tcamInfo info */
#define CHECK_VTCAM_INFO_SAME_VALUE_MAC(existVtcamInfoPtr,updatedVtcamInfoPtr,field)    \
    if(updatedVtcamInfoPtr->field != existVtcamInfoPtr->field)    \
    {                                                             \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,               \
            "the vTcamId[%d] in vTcamCheckInfoArr index [%d] : hold field [%s] value[%d] mismatch from exists value[%d]", \
            vTcamId,ii,                                           \
            #field,                                               \
            updatedVtcamInfoPtr->field,                           \
            existVtcamInfoPtr->field                              \
            );                                                    \
    }

/*******************************************************************************
* memoryAvailabilityCheckParams
*
* DESCRIPTION:
*       Check parameters for cpssDxChVirtualTcamMemoryAvailabilityCheck(...)
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamAmount       - amount of Virtual TCAMs in the array.
*       vTcamCheckInfoArr - array of infos for Virtual TCAMs.
*
*
* OUTPUTS:
*       tcamAvailabilityPtr       - (pointer to)TCAM Availability (see enum)
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if pointer or array-pointer is NULL.
*       GT_NOT_FOUND             - TCAM manager with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS memoryAvailabilityCheckParams
(
    IN   GT_U32                                 vTcamMngId,
    IN   GT_U32                                 vTcamAmount,
    IN   CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC  vTcamCheckInfoArr[],
    OUT  CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT *tcamAvailabilityPtr
)
{
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC* vTcamMngPtr; /* pointer to vTcam Manager */
    PRV_CPSS_DXCH_VIRTUAL_TCAM_DB_VTCAM_CFG_STC*  vTcamCfgPtr; /* pointer to vTcam         */
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC *existVtcamInfoPtr;
    CPSS_DXCH_VIRTUAL_TCAM_INFO_STC *updatedVtcamInfoPtr;
    CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC *currCheckInfoPtr;
    GT_U32 vTcamId;
    GT_U32  ii;

    CPSS_NULL_PTR_CHECK_MAC(vTcamCheckInfoArr);
    CPSS_NULL_PTR_CHECK_MAC(tcamAvailabilityPtr);

    vTcamMngPtr = prvCpssDxChVirtualTcamDbVTcamMngGet(vTcamMngId);
    if(vTcamMngPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, "TCAM manager id [%d] was not found",
            vTcamMngId);
    }

    if(vTcamAmount == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, "no meaning calling function with amount 'ZERO'");
    }

    currCheckInfoPtr = &vTcamCheckInfoArr[0];
    for(ii = 0 ; ii < vTcamAmount; ii++ , currCheckInfoPtr++)
    {
        vTcamId = currCheckInfoPtr->vTcamId;

        /*******************/
        /* validity checks */
        /*******************/
        if(vTcamId >= vTcamMngPtr->vTcamCfgPtrArrSize)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
                "the vTcamId[%d] in vTcamCheckInfoArr index [%d] is over max size [%d]",
                vTcamId,ii,vTcamMngPtr->vTcamCfgPtrArrSize);
        }

        vTcamCfgPtr = vTcamMngPtr->vTcamCfgPtrArr[vTcamId];

        if(currCheckInfoPtr->isExist != GT_FALSE && vTcamCfgPtr == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
                "the vTcamId[%d] in vTcamCheckInfoArr index [%d] expected to exists but is not",
                vTcamId,ii);
        }
        else
        if(currCheckInfoPtr->isExist == GT_FALSE && vTcamCfgPtr)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM,
                "the vTcamId[%d] in vTcamCheckInfoArr index [%d] exist although not expected to be",
                vTcamId,ii);
        }

        if(currCheckInfoPtr->isExist == GT_TRUE && vTcamCfgPtr)
        {
            existVtcamInfoPtr   = &vTcamCfgPtr->tcamInfo;
            updatedVtcamInfoPtr = &currCheckInfoPtr->vTcamInfo;

            /* the updated info must be with same values as exists
               except for 'guaranteedNumOfRules' that can be different */
            CHECK_VTCAM_INFO_SAME_VALUE_MAC(existVtcamInfoPtr,updatedVtcamInfoPtr,clientGroup);
            CHECK_VTCAM_INFO_SAME_VALUE_MAC(existVtcamInfoPtr,updatedVtcamInfoPtr,hitNumber);
            CHECK_VTCAM_INFO_SAME_VALUE_MAC(existVtcamInfoPtr,updatedVtcamInfoPtr,ruleSize);
            CHECK_VTCAM_INFO_SAME_VALUE_MAC(existVtcamInfoPtr,updatedVtcamInfoPtr,autoResize);
            CHECK_VTCAM_INFO_SAME_VALUE_MAC(existVtcamInfoPtr,updatedVtcamInfoPtr,ruleAdditionMethod);

        }

        if(currCheckInfoPtr->isExist == GT_FALSE && (vTcamCfgPtr == NULL))
        {
            if (vTcamMngPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
                /* Extra check for parameters in xCat3 */
                updatedVtcamInfoPtr = &currCheckInfoPtr->vTcamInfo;

                if (updatedVtcamInfoPtr->clientGroup != 0)
                {
                    /* Only group #0 (PCL) is currently allowed on this device family */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                if (updatedVtcamInfoPtr->hitNumber != 0)
                {
                    /* Parallel lookups not allowed on this device family */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                switch (updatedVtcamInfoPtr->ruleSize)
                {
                    case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_30_B_E:
                    case CPSS_DXCH_VIRTUAL_TCAM_RULE_SIZE_60_B_E:
                        /* OK */
                        break;
                    default:
                        /* Invalid size on this device family */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        break;
                }
            }

        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChVirtualTcamMemoryAvailabilityCheck
*
* DESCRIPTION:
*       Check availability of TCAM manager memory for list of new and existing Virtual TCAMs.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamAmount       - amount of Virtual TCAMs in the array.
*       vTcamCheckInfoArr - array of infos for Virtual TCAMs.
*
*
* OUTPUTS:
*       tcamAvailabilityPtr       - (pointer to)TCAM Availability (see enum)
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if pointer or array-pointer is NULL.
*       GT_NOT_FOUND             - TCAM manager with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChVirtualTcamMemoryAvailabilityCheck
(
    IN   GT_U32                                 vTcamMngId,
    IN   GT_U32                                 vTcamAmount,
    IN   CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC  vTcamCheckInfoArr[],
    OUT  CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT *tcamAvailabilityPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC* feasibilityCheck_vTcamMngPtr; /* pointer to temporary
                    vTcam Manager that is used for feasibility checks*/
    IN GT_BOOL                                 copyOrigLayout;/* indication if to copy
                the layout from the orig TCAM or not.
                GT_TRUE - new layout will be copy the orig layout from orig TCAM.
                GT_FALSE - new layout is empty.*/
    PRV_CPSS_DXCH_VIRTUAL_TCAM_MNG_STC* vTcamMngPtr; /* pointer to vTcam Manager */

    /* check parameters */
    rc = memoryAvailabilityCheckParams(vTcamMngId,vTcamAmount,vTcamCheckInfoArr,tcamAvailabilityPtr);
    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, "Failed on parameters check");
    }

    vTcamMngPtr = prvCpssDxChVirtualTcamDbVTcamMngGet(vTcamMngId);
    if(vTcamMngPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, "TCAM manager id [%d] was not found",
            vTcamMngId);
    }

    /* check if can be supported as 'from current state'*/
    copyOrigLayout = GT_TRUE;
    /* get vTcam Manager that is used for feasibility checks */
    rc = createTcamManagerWithOnlySegmentTable(vTcamMngPtr,
        copyOrigLayout,
        &feasibilityCheck_vTcamMngPtr);
    if(rc != GT_OK)
    {
        /* all allocations free by the function itself */
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    CPSS_LOG_INFORMATION_MAC("check ability to insert from current state \n");
    /* check ability to insert from current state */
    rc = virtualTcamMemoryAvailabilityCheck_fromCurrentState(
        vTcamMngPtr,
        feasibilityCheck_vTcamMngPtr,
        vTcamAmount,vTcamCheckInfoArr);
    if(rc == GT_OK)
    {
        *tcamAvailabilityPtr = CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ALREADY_E;
        goto cleanExit_lbl;
    }
    else if(rc == GT_FULL)
    {
        /* continue to check if possible 'from scratch' */
    }
    else
    {
        /* ERROR */
        goto cleanExit_lbl;
    }

    /* release resources */
    deleteTcamManagerWithOnlySegmentTable(feasibilityCheck_vTcamMngPtr);

    /* check if can be supported as 'from scretch'*/
    copyOrigLayout = GT_FALSE;
    /* get vTcam Manager that is used for feasibility checks */
    rc = createTcamManagerWithOnlySegmentTable(vTcamMngPtr,
        copyOrigLayout,
        &feasibilityCheck_vTcamMngPtr);
    if(rc != GT_OK)
    {
        /* all allocations free by the function itself */
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    CPSS_LOG_INFORMATION_MAC("check ability to insert from scratch \n");
    rc = virtualTcamMemoryAvailabilityCheck_fromScratch(
        vTcamMngPtr,
        feasibilityCheck_vTcamMngPtr,
        vTcamAmount,vTcamCheckInfoArr);
    if(rc == GT_OK)
    {
        *tcamAvailabilityPtr = CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_FROM_SCRATCH_E;
    }
    else if(rc == GT_FULL)
    {
        *tcamAvailabilityPtr = CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_NEVER_E;
        rc = GT_OK;/*!!!*/
    }
    else
    {
        /* ERROR */
        goto cleanExit_lbl;
    }

cleanExit_lbl:
    if(feasibilityCheck_vTcamMngPtr)
    {
        deleteTcamManagerWithOnlySegmentTable(feasibilityCheck_vTcamMngPtr);
    }

    if(rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }


    return GT_OK;
}

/*******************************************************************************
* cpssDxChVirtualTcamMemoryAvailabilityCheck
*
* DESCRIPTION:
*       Check availability of TCAM manager memory for list of new and existing Virtual TCAMs.
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       vTcamMngId        - Virtual TCAM Manager Id
*                           (APPLICABLE RANGES: 0..31)
*       vTcamAmount       - amount of Virtual TCAMs in the array.
*       vTcamCheckInfoArr - array of infos for Virtual TCAMs.
*
*
* OUTPUTS:
*       tcamAvailabilityPtr       - (pointer to)TCAM Availability (see enum)
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - if pointer or array-pointer is NULL.
*       GT_NOT_FOUND             - TCAM manager with given Id not found
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChVirtualTcamMemoryAvailabilityCheck
(
    IN   GT_U32                                 vTcamMngId,
    IN   GT_U32                                 vTcamAmount,
    IN   CPSS_DXCH_VIRTUAL_TCAM_CHECK_INFO_STC  vTcamCheckInfoArr[],
    OUT  CPSS_DXCH_VIRTUAL_TCAM_AVAILABILTY_ENT *tcamAvailabilityPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChVirtualTcamMemoryAvailabilityCheck);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, vTcamMngId, vTcamAmount, vTcamCheckInfoArr, tcamAvailabilityPtr));

    rc = internal_cpssDxChVirtualTcamMemoryAvailabilityCheck(vTcamMngId, vTcamAmount, vTcamCheckInfoArr, tcamAvailabilityPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, vTcamMngId, vTcamAmount, vTcamCheckInfoArr, tcamAvailabilityPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


