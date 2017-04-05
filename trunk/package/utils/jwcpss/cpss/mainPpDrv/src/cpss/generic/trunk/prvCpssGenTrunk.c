/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenTrunk.c
*
* DESCRIPTION:
*       CPSS generic Trunk implementation, for setting ,resetting and editing
*       of a trunks.
*
*
* FILE REVISION NUMBER:
*       $Revision: 37 $
******************************************************************************/

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>/*include for prvCpssDrvPortsFullMaskGet(...) */
#ifdef CHX_FAMILY
    #include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

    #define __PortsBmpAdjustToDevice    prvCpssDxChTablePortsBmpAdjustToDevice
#else
    #define __PortsBmpAdjustToDevice(a,b,c)
#endif

/*******************************************************************************
* External usage environment parameters
*******************************************************************************/

/* macro to get the trunk entry from DB
   return (pointer to)entry in the format of: PRV_CPSS_TRUNK_ENTRY_INFO_STC
   or NULL
*/
#define TRUNK_ENTRY_GET_MAC(devNum,trunkId)   trunkDbInfoGet(devNum,trunkId)

/* check if this is DB trunk valid entry */
#define TRUNK_ENTRY_CHECK_MAC(trunkEntryPtr) \
    if(trunkEntryPtr == NULL) CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* check if the library was initialized */
#define TRUNK_LIB_INIT_CHECK_MAC(devNum)    \
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))                     \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                        \
    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->initDone == GT_FALSE)   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG)

/* SW check that the trunkId is valid trunk number -- as index to DB */
#define SW_TRUNK_ID_AS_INDEX_CHECK_MAC(devNum,index) \
    if((index) >= PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numberOfTrunks)\
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* macro to get the trunk virtual functions of the device */
#define TRUNK_VIRTUAL_FUNC_MAC(devNum) \
    PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->virtualFunctionsPtr

/* BMP of ports with no ports members */
const CPSS_PORTS_BMP_STC cpssAllZeroPortsBmp = {{0}};

/* The number of trunk members is 8, and the member     */
/* list is A,B,B,A,B,A,A,B (where A,B are the sw        */
/* trunk members.                                       */
static const GT_U8 trunkMembersWorkAround_2Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,1,1,0,1,0,0,1};


/* The number of trunk members is 8, and the member     */
/* list is C,A,C,A,B,C,B,C (where A,B,C are the sw      */
/* trunk members.                                       */
static const GT_U8 trunkMembersWorkAround_3Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {2,0,2,0,1,2,1,2};

/* The number of trunk members is 8, and the member     */
/* list is C,C,C,A,B,D,D,D (where A,B,C,D are the sw    */
/* trunk members.                                       */
static const GT_U8 trunkMembersWorkAround_4Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {2,2,2,0,1,3,3,3};

/* work around for 3 ports in trunk for ExMx devices
    see function trunk3PortsWorkAroundIndexGet(...)
*/
static const GT_U8 designated3PortsIndexArray[16] =
                                    {0,1,2,0,0,1,1,2,0,2,2,1,0,2,1,0};

/*
1. In order to have the same load balance decisions in the cheetah and in the
    opal the cheetah need to have redundancy ports in the trunk .
    that way the cheetahs use of MODULO ("%") operator will be like in SOHO
    (SOHO use %8 (last 3 bits)- for all traffic)
2. Due to Erratum in cheetah+ A1 version(trunk and next hop) , the cheetah need
    to have always 8 members in HW

Meaning that if trunk has 0,1,2,3,4,5,6,7 ports the PP must "think" that there
   are 8 ports in it
*/
/* emulation from 0 ports to 8 ----> still we will have the NULL port as member
   in all members */
static const GT_U8 trunkMembersForceRedandentTo8Members_0Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,0,0,0,0,0,0,0};

/* emulation from 1 ports to 8 */
static const GT_U8 trunkMembersForceRedandentTo8Members_1Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,0,0,0,0,0,0,0};

/* emulation from 2 ports to 8 */
static const GT_U8 trunkMembersForceRedandentTo8Members_2Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,1,0,1,0,1,0,1};

/* emulation from 3 ports to 8 */
static const GT_U8 trunkMembersForceRedandentTo8Members_3Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,1,2,0,1,2,0,1};
/* emulation from 4 ports to 8 */
static const GT_U8 trunkMembersForceRedandentTo8Members_4Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,1,2,3,0,1,2,3};
/* emulation from 5 ports to 8 */
static const GT_U8 trunkMembersForceRedandentTo8Members_5Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,1,2,3,4,0,1,2};
/* emulation from 6 ports to 8 */
static const GT_U8 trunkMembersForceRedandentTo8Members_6Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,1,2,3,4,5,0,1};
/* emulation from 7 ports to 8 */
static const GT_U8 trunkMembersForceRedandentTo8Members_7Ports[PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS] =
                                    {0,1,2,3,4,5,6,0};

/* enum : IS_LOCAL_ENT
   description : type of 'is local' indications
   values:
        IS_LOCAL_FALSE_E - the member is not local to the device
        IS_LOCAL_TRUE_E - the member is local to the device
        IS_LOCAL_CHECK_E - the INPUT member need to be checked to know if
                           local or not (not apply to removed port from trunk
                           because it was already removed from the DB !!!)
*/
typedef enum{
    IS_LOCAL_FALSE_E,
    IS_LOCAL_TRUE_E,
    IS_LOCAL_CHECK_E
}IS_LOCAL_ENT;

/* this API supported only for regular trunks or free trunk */
#define TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr)     \
    switch(trunkInfoPtr->type)                                 \
    {                                                          \
        case CPSS_TRUNK_TYPE_FREE_E:                           \
        case CPSS_TRUNK_TYPE_REGULAR_E:                        \
            break;                                             \
        default:                                               \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                               \
    }

/* this API supported only for cascade trunks or free trunk */
#define TRUNK_TYPE_SUPPORT_CASCADE_CHECK_MAC(trunkInfoPtr) \
    switch(trunkInfoPtr->type)                             \
    {                                                      \
        case CPSS_TRUNK_TYPE_FREE_E:                       \
        case CPSS_TRUNK_TYPE_CASCADE_E:                    \
            break;                                         \
        default:                                           \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);                           \
    }


/* set the trunkId of the local port */
#define LOCAL_MEMBER_DB_SET_MAC(_devNum,_trunkId,_trunkInfoPtr,_index,_localPort) \
    if(_trunkInfoPtr->isLocalMembersPtr[_index] == GT_TRUE)                       \
        PRV_CPSS_DEV_TRUNK_INFO_MAC(_devNum)->portTrunkIdArray[_localPort] = _trunkId

/* unset the trunkId of the local port --> no trunk */
#define LOCAL_MEMBER_DB_UNSET_MAC(_devNum,_trunkId,_trunkInfoPtr,_index,_localPort) \
    if(_trunkInfoPtr->isLocalMembersPtr[_index] == GT_TRUE)                         \
        PRV_CPSS_DEV_TRUNK_INFO_MAC(_devNum)->portTrunkIdArray[_localPort] = 0

#ifdef CHX_FAMILY
/* The max number of 'port' as member of trunk -- this value is 'not valid' */
#define MAX_MEMBER_PORT_NUM_MAC(_devNum)                  \
    PRV_CPSS_DXCH_MAX_PHY_PORT_NUMBER_MAC(_devNum)
#else
/* The max number of 'port' as member of trunk -- this value is 'not valid' */
#define MAX_MEMBER_PORT_NUM_MAC(_devNum)                  \
    (GT_U32)(PRV_CPSS_IS_DUAL_HW_DEVICE_MAC(PRV_CPSS_HW_DEV_NUM_MAC(_devNum)) ?  \
    ((PRV_CPSS_DEV_TRUNK_INFO_MAC(_devNum)->validityMask.port + 1) * 2) :        \
     (PRV_CPSS_DEV_TRUNK_INFO_MAC(_devNum)->validityMask.port + 1))
#endif
/* indication that the device uses check of physical port by mask of port */
#define IS_PHY_PORT_BY_MASK_MAC(devNum) \
    ((PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->isPhysicalPortByMask == GT_TRUE) ? 1 : 0)

/* check for physical port that checked by 'mask' of port */
#define CHECK_PHY_PORT_BY_MASK_MAC(devNum,portNum)                          \
    if((portNum) > PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->validityMask.port)  \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* skip for physical port that not in 'mask' of port */
#define SKIP_NON_VALID_PHY_PORT_BY_MASK_MAC(devNum,portNum)                 \
    if((portNum) > PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->validityMask.port)  \
        continue

/* portsBmpPtr of pointer to type CPSS_PORTS_BMP_STC */
#define PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,portsBmpPtr)              \
    if(IS_PHY_PORT_BY_MASK_MAC(devNum))                                     \
    {                                                                       \
        CPSS_PORTS_BMP_PORT_SET_ALL_MAC(portsBmpPtr);                       \
        /* adjusted physical ports of BMP to hold only bmp of existing ports*/ \
        __PortsBmpAdjustToDevice(devNum , portsBmpPtr , portsBmpPtr);       \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        prvCpssDrvPortsFullMaskGet(PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts,portsBmpPtr);\
    }

/* check if need use virtual port range instead of 'physical ports' range */
#define USE_VIRTUAL_PORT_MAC(_devNum) \
    ((PRV_CPSS_PP_MAC(_devNum)->numOfPorts != PRV_CPSS_PP_MAC(_devNum)->numOfVirtPorts) ? 1 : 0)

static GT_STATUS trunkDbMembersForTrunkIdSort
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId
);

/* mask with value more than 0xFF support full range */
#define PHY_PORT_ALL_RANGE_SUPPORTED_CNS    0xFF


/*******************************************************************************
* trunkDbInfoGet
*
* DESCRIPTION:
*       This routine returns pointer to a local trunk entry's info.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - the device to create the trunk on
*       trunkId     - The allocated trunk id to associate with this trunk.
*
* OUTPUTS:
*       NONE
*
* RETURNS:
*       pointer to the trunk info , or NULL if trunk not exists in DB
*
* COMMENTS:
*       "local trunk" feature
*
*******************************************************************************/
static PRV_CPSS_TRUNK_ENTRY_INFO_STC* trunkDbInfoGet
(
    IN GT_U8        devNum,
    IN GT_TRUNK_ID  trunkId
)
{

    /* the DB not use entry index 0 .
       the DB hold numOfTrunks+1 entries !!!
    */
    if((trunkId == 0) ||
       trunkId > PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numberOfTrunks)
    {
        return (PRV_CPSS_TRUNK_ENTRY_INFO_STC*)NULL;
    }

    return &PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->trunksArray[trunkId];
}

/*******************************************************************************
* trunkDbMemberCompareByIndex
*
* DESCRIPTION:
*       check if INPUT member can be found in the DB , and in what index.
*       there are 2 types of checking :
*           1. when hwDevNumChangedPtr == NULL -->
*              only as exact match, between the INPUT member and the DB trunk members
*           2. when hwDevNumChangedPtr != NULL -->
*              the INPUT member has device == PRV_CPSS_HW_DEV_NUM_MAC(devNum) and the
*              DB trunks member is 'Local' but with 'old hwDevNum'
*              and INPUT member has port == DB trunks member port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - the device to create the trunk on
*       trunkInfoPtr - The trunk DB for current trunkId
*       index       - index of member in DB
*       memberPtr   - (pointer to)the member in question
*       hwDevNumChangedPtr - choose compare type :
*                       NULL - option 1 (see above)
*                       no NULL - option 2 (see above)
*
* OUTPUTS:
*       hwDevNumChangedPtr - (pointer to) indication that the 'not equal'
*                   could have been 'equal' if we used the 'local member'
*                   indication
*
* RETURNS:
*       are the members equal ?
*       GT_TRUE  - equal
*       GT_FALSE - not equal
* COMMENTS:
*
*
*******************************************************************************/
static GT_BOOL trunkDbMemberCompareByIndex
(
    IN GT_U8        devNum,
    IN PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr,
    IN GT_U32       index,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr,
    INOUT GT_BOOL      *hwDevNumChangedPtr
)
{
    if(hwDevNumChangedPtr)
    {
        *hwDevNumChangedPtr = GT_FALSE;
    }

    if(trunkInfoPtr->membersPtr[index].port != memberPtr->port)
    {
        /* not the same port */
        return GT_FALSE;
    }

    if(trunkInfoPtr->isLocalMembersPtr[index] == GT_TRUE)
    {
        if(hwDevNumChangedPtr)
        {
            *hwDevNumChangedPtr = (trunkInfoPtr->membersPtr[index].hwDevice == PRV_CPSS_HW_DEV_NUM_MAC(devNum)) ?
                GT_FALSE : GT_TRUE;
        }
    }

    return (trunkInfoPtr->membersPtr[index].hwDevice == memberPtr->hwDevice) ?
        GT_TRUE : GT_FALSE;
}


/*******************************************************************************
* trunkDbMemberIndexGet
*
* DESCRIPTION:
*       get the member index in the DB.
*       there are 2 types of checking :
*           1. when hwDevNumChangedPtr == NULL -->
*              only as exact match, between the INPUT member and the DB trunk members
*           2. when hwDevNumChangedPtr != NULL -->
*              first check of exact match , and if no match then check:
*              the INPUT member has device == PRV_CPSS_HW_DEV_NUM_MAC(devNum) and the
*              DB trunks member is 'Local' but with 'old hwDevNum'
*              and INPUT member has port == DB trunks member port
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - the device to create the trunk on
*       trunkInfoPtr - The trunk DB for current trunkId
*       memberPtr   - the member to find in DB
* OUTPUTS:
*       dbIndexPtr - (pointer to) the index in DB of the member (if found)
*       hwDevNumChangedPtr - (pointer to) indication that the 'found'
*                   could was due to 'local member' indication
*                   GT_TRUE - the member found only because we used 'local member'
*                             indication
*                   GT_FALSE - the member found due to exact match
*
* RETURNS:
*       GT_OK - member was found
*       GT_NOT_FOUND - member was not found
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS trunkDbMemberIndexGet
(
    IN GT_U8        devNum,
    IN PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr,
    OUT GT_U32      *dbIndexPtr,
    OUT GT_BOOL      *hwDevNumChangedPtr
)
{
    GT_U32  ii;
    GT_U32  count;/* number of enabled and disabled members in trunk */
    GT_BOOL  hwDevNumChanged;/*indication that the 'not equal'
                could have been 'equal' if we used the 'local member'
                indication*/

    count = trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount;

    /* check if port exists */
    for(ii = 0 ; ii < count ; ii++)
    {
        if(GT_TRUE == trunkDbMemberCompareByIndex(devNum,trunkInfoPtr,ii,memberPtr,NULL))
        {
            if(hwDevNumChangedPtr)
            {
                *hwDevNumChangedPtr = GT_FALSE;
            }
            *dbIndexPtr = ii;
            return GT_OK;
        }
    }

    if(hwDevNumChangedPtr)
    {
        /* port was not found 'as is' , so check it as 'local port' */
        for(ii = 0 ; ii < count ; ii++)
        {
            hwDevNumChanged = GT_FALSE;
            if(GT_FALSE == trunkDbMemberCompareByIndex(devNum,trunkInfoPtr,ii,memberPtr,&hwDevNumChanged) &&
               hwDevNumChanged == GT_TRUE)
            {
                *hwDevNumChangedPtr = GT_TRUE;
                *dbIndexPtr = ii;
                return GT_OK;
            }
        }
    }

    return /* this is not ERROR for log*/ GT_NOT_FOUND;
}

/*******************************************************************************
* trunkDbDesignatedMemberIsEnabledAndLocal
*
* DESCRIPTION:
*       check that if the designated member is local member , then make sure it
*       is registered on ownDevNum.
*       function return indication that member is local and enabled member.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - the device to create the trunk on
*       trunkInfoPtr - The trunk DB for current trunkId
*
* OUTPUTS:
*       None.
*
*
* RETURNS:
*       if designated member is local and enabled
*       GT_TRUE - the designated member is local to the device and is enabled member
*       GT_FALSE - the designated member is NOT local to the device Or NOT enabled member
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_BOOL trunkDbDesignatedMemberIsEnabledAndLocal
(
    IN GT_U8        devNum,
    IN PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr
)
{
    GT_U32  ii;
    CPSS_TRUNK_MEMBER_STC    *memberPtr = &trunkInfoPtr->designatedMember;
    GT_BOOL hwDevNumChanged;/* indication that the 'found'
                            could was due to 'local member' indication
                            GT_TRUE - the member found only because we used 'local member'
                                      indication
                            GT_FALSE - the member found due to exact match*/

    if(trunkInfoPtr->designatedMemberIsLocal == GT_FALSE)
    {
        return GT_FALSE;
    }

     /*
    application responsibility to set 'new designated member'
    make sure the 'Local' designated member holds the current hwDevNum
    trunkInfoPtr->designatedMember.device = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
    */

    if(GT_OK != trunkDbMemberIndexGet(devNum,trunkInfoPtr,memberPtr,&ii,&hwDevNumChanged))
    {
        /* the designated member was not added to the trunk yet ??? */
        return GT_FALSE;
    }

    /* check if member enabled */
    if(ii < trunkInfoPtr->enabledCount)
    {
        return GT_TRUE;
    }
    else
    {
        return GT_FALSE;
    }
}

/*******************************************************************************
* trunkDbMemberIsLocal
*
* DESCRIPTION:
*       is a member of the trunk 'local member'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       trunkInfoPtr - The trunk DB for current trunkId
*       memberPtr   - the member to check if is local member
* OUTPUTS:
*       None.
*
*
* RETURNS:
*       if designated member is local and enabled
*       GT_TRUE - the designated member is local to the device and is enabled member
*       GT_FALSE - the designated member is NOT local to the device Or NOT enabled member
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_BOOL trunkDbMemberIsLocal
(
    IN PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_U32  ii;
    GT_U32  count;/* number of enabled and disabled members in trunk */

    count = trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount;

    for(ii = 0 ; ii < count ; ii++)
    {
        if(trunkInfoPtr->membersPtr[ii].hwDevice == memberPtr->hwDevice &&
           trunkInfoPtr->membersPtr[ii].port == memberPtr->port )
        {
            return trunkInfoPtr->isLocalMembersPtr[ii];
        }
    }

    /* the member not in the DB --- !!!! ERROR !!!! */
    /* this is error to call this function for member that is not in the trunk */
    return GT_FALSE;
}

/*******************************************************************************
* trunkDbMemberSet
*
* DESCRIPTION:
*       set the member info in the DB
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - the device to create the trunk on
*       trunkInfoPtr - The trunk DB for current trunkId
*       index       - the needed index
*       memberPtr    - (pointer to) the member
*                       when NULL --> use {0,0}
* OUTPUTS:
*       None.
*
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void trunkDbMemberSet
(
    IN GT_U8        devNum,
    IN PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr,
    IN GT_U32       index,
    IN CPSS_TRUNK_MEMBER_STC   *memberPtr
)
{
    if(memberPtr)
    {
        trunkInfoPtr->membersPtr[index] = *memberPtr;
        trunkInfoPtr->isLocalMembersPtr[index] =
            (memberPtr->hwDevice == PRV_CPSS_HW_DEV_NUM_MAC(devNum)) ? GT_TRUE : GT_FALSE;
    }
    else
    {
        trunkInfoPtr->membersPtr[index].hwDevice = 0;/*just for cleanup*/
        trunkInfoPtr->membersPtr[index].port = 0;  /*just for cleanup*/
        trunkInfoPtr->isLocalMembersPtr[index] = GT_FALSE;/*just for cleanup*/
    }

    return ;
}

/*******************************************************************************
* trunkDbMemberCopy
*
* DESCRIPTION:
*       copy the member info in the DB to 'new index' from 'old index'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       trunkInfoPtr - The trunk DB for current trunkId
*       newIndex    - the new index
*       oldIndex    - the current index
* OUTPUTS:
*       None.
*
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
static void trunkDbMemberCopy
(
    IN PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr,
    IN GT_U32       newIndex,
    IN GT_U32       oldIndex
)
{
    trunkInfoPtr->membersPtr[newIndex] = trunkInfoPtr->membersPtr[oldIndex];
    trunkInfoPtr->isLocalMembersPtr[newIndex] = trunkInfoPtr->isLocalMembersPtr[oldIndex];

    return;
}


/*******************************************************************************
* trunkDbMembersRefresh
*
* DESCRIPTION:
*       refresh all the trunk members in case that hwDevNum changed
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum  - the device to create the trunk on
*       trunkId - The trunkId
* OUTPUTS:
*       None.
*
*
* RETURNS:
*       None
*
* COMMENTS:
*       In 'sort mode' the DB will also be re-sorted in case of modifications
*
*******************************************************************************/
static GT_STATUS trunkDbMembersRefresh
(
    IN GT_U8        devNum,
    IN GT_TRUNK_ID      trunkId
)
{
    GT_STATUS                       rc;
    GT_U32                          ii;
    GT_U32                          count;/* total number of ports in this trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC   *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    GT_HW_DEV_NUM                   hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
    GT_BOOL                         modificationDone = GT_FALSE;/*indication that the refresh actually changed the DB */

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    if(NULL == trunkInfoPtr)/* fix COVERITY warning */
    {
        /* should not happen because the caller of this function , must already check this */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    count = trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount;

    for(ii = 0 ; ii < count ; ii++)
    {
        if(trunkInfoPtr->isLocalMembersPtr[ii] == GT_FALSE)
        {
            continue;
        }

        modificationDone = GT_TRUE;

        /* make sure the 'Local' members holds the current hwDevNum */
        trunkInfoPtr->membersPtr[ii].hwDevice = hwDevNum;
    }

    /* application responsibility to set 'new designated member'
    if(trunkInfoPtr->designatedMemberIsLocal == GT_TRUE)
    {
        make sure the 'Local' designated member holds the current hwDevNum
        trunkInfoPtr->designatedMember.device = hwDevNum;
    }
    */

    if(modificationDone == GT_FALSE)
    {
        /* no need to continue for the sorting section because no modification done*/
        return GT_OK;
    }

    /* check if need to do sorting on the trunk members. */
    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->sortTrunkMembers == GT_TRUE)
    {
        rc = trunkDbMembersForTrunkIdSort(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* trunkDbInit
*
* DESCRIPTION:
*       initialize the SW DB about the trunks in this device .
*       NOTE : get specific info via virtual function pointers.
*              (those pointers initialized during "phase-1")
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device num
*       maxNumberOfTrunks  - maximum number of trunk groups.
*                            when this number is 0 , there will be no shadow
*                            used.
*                            Note:
*                            that means that API's that used shadow will FAIL.
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_BAD_STATE - if library already initialized with different number of
*                       trunks than requested
*       GT_NOT_INITIALIZED - the trunk virtual functions were not initialized
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDbInit
(
    IN  GT_U8   devNum,
    IN  GT_U32  maxNumberOfTrunks
)
{
    GT_STATUS rc;    /* return error core */
    GT_U32  ii;
    PRV_CPSS_TRUNK_DB_INFO_STC *genTrunkDbPtr;/* (pointer to) trunk db of the
                                                   device */
    PRV_CPSS_GEN_PP_CONFIG_STC *devPtr = PRV_CPSS_PP_MAC(devNum);/* pointer to
                                                common device info */

    /* init coreGen trunk DB */
    genTrunkDbPtr = &(devPtr->trunkInfo);

    if(genTrunkDbPtr->initDone    == GT_TRUE ||
       genTrunkDbPtr->shadowValid == GT_TRUE)
    {
        /* DB already initialized */


        /* check that the current number of trunks equal to the 'new number' */
        if(maxNumberOfTrunks != genTrunkDbPtr->numberOfTrunks)
        {
            /* can't change the number of trunks supported */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE,
            "can't change the number of trunks [%d] supported to [%d]",
                genTrunkDbPtr->numberOfTrunks,
                maxNumberOfTrunks);
        }

        return GT_OK;
    }

    /* reset the DB */
    cpssOsMemSet(genTrunkDbPtr,0,sizeof(PRV_CPSS_TRUNK_DB_INFO_STC));

    if(prvCpssFamilyInfoArray[devPtr->devFamily] == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* set the pointer to the virtual functions -- for quick accessing */
    genTrunkDbPtr->virtualFunctionsPtr =
        &prvCpssFamilyInfoArray[devPtr->devFamily]->trunkInfo.boundFunc;

    /* check that there are valid virtual functions */
    if(genTrunkDbPtr->virtualFunctionsPtr->dbFlagsInitFunc == NULL      ||
       genTrunkDbPtr->virtualFunctionsPtr->portTrunkIdSetFunc == NULL   ||
       genTrunkDbPtr->virtualFunctionsPtr->membersSetFunc == NULL       ||
       genTrunkDbPtr->virtualFunctionsPtr->nonMembersBmpSetFunc == NULL ||
       genTrunkDbPtr->virtualFunctionsPtr->nonMembersBmpGetFunc == NULL ||
       genTrunkDbPtr->virtualFunctionsPtr->designatedMembersBmpSetFunc == NULL ||
       genTrunkDbPtr->virtualFunctionsPtr->designatedMembersBmpGetFunc == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    /* set default for the number of members in trunk */
    genTrunkDbPtr->numMembersInTrunk = PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;

    /* call appropriate virtual function */
    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->dbFlagsInitFunc(devNum);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* this value can be checked only after the DB of the device was
       initialized */
    if(maxNumberOfTrunks >
       PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, "maxNumberOfTrunks[%d] is more than supported [%d]",
            maxNumberOfTrunks , PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw);
    }

    if(maxNumberOfTrunks == 0)
    {
        /* no shadow -- at ALL */
        return GT_OK;
    }

    genTrunkDbPtr->numberOfTrunks = maxNumberOfTrunks;

    genTrunkDbPtr->trunksArray =
        cpssOsMalloc(sizeof(PRV_CPSS_TRUNK_ENTRY_INFO_STC) *
            (genTrunkDbPtr->numberOfTrunks + 1));

    if(genTrunkDbPtr->trunksArray == NULL)
    {
        rc = GT_OUT_OF_CPU_MEM;
        goto exit_cleanly_lbl;
    }

    cpssOsMemSet(genTrunkDbPtr->trunksArray,0,
             sizeof(PRV_CPSS_TRUNK_ENTRY_INFO_STC) *
                (genTrunkDbPtr->numberOfTrunks + 1));

    /* dynamic allocation of the trunk members */
    genTrunkDbPtr->allMembersArray = cpssOsMalloc(
            genTrunkDbPtr->numMembersInTrunk * genTrunkDbPtr->numberOfTrunks *
                                             sizeof(CPSS_TRUNK_MEMBER_STC));

    if(genTrunkDbPtr->allMembersArray == NULL)
    {
        rc = GT_OUT_OF_CPU_MEM;
        goto exit_cleanly_lbl;
    }

    cpssOsMemSet(genTrunkDbPtr->allMembersArray,0,
             genTrunkDbPtr->numMembersInTrunk * genTrunkDbPtr->numberOfTrunks *
                                             sizeof(CPSS_TRUNK_MEMBER_STC));

    /* loop on all trunks and "attach" each trunk with it's members */
    for(ii = 0 ; ii < genTrunkDbPtr->numberOfTrunks ; ii++)
    {
        genTrunkDbPtr->trunksArray[ii + 1].membersPtr =
                &genTrunkDbPtr->allMembersArray[ii * genTrunkDbPtr->numMembersInTrunk];
    }

    /* dynamic allocation of the trunk 'Local indications' */
    genTrunkDbPtr->allMembersIsLocalArray = cpssOsMalloc(
            genTrunkDbPtr->numMembersInTrunk * genTrunkDbPtr->numberOfTrunks *
                                             sizeof(GT_BOOL));

    if(genTrunkDbPtr->allMembersIsLocalArray == NULL)
    {
        rc = GT_OUT_OF_CPU_MEM;
        goto exit_cleanly_lbl;
    }

    cpssOsMemSet(genTrunkDbPtr->allMembersIsLocalArray,0,
             genTrunkDbPtr->numMembersInTrunk * genTrunkDbPtr->numberOfTrunks *
                                             sizeof(GT_BOOL));

    /* loop on all trunks and "attach" each trunk with it's 'Local indications' */
    for(ii = 0 ; ii < genTrunkDbPtr->numberOfTrunks ; ii++)
    {
        genTrunkDbPtr->trunksArray[ii + 1].isLocalMembersPtr =
                &genTrunkDbPtr->allMembersIsLocalArray[ii * genTrunkDbPtr->numMembersInTrunk];
    }



exit_cleanly_lbl:
    if(rc != GT_OK)
    {
        FREE_PTR_MAC(genTrunkDbPtr->trunksArray);
        FREE_PTR_MAC(genTrunkDbPtr->allMembersArray);
        FREE_PTR_MAC(genTrunkDbPtr->allMembersIsLocalArray);

        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }
    else
    {
        /* set the shadow tables as valid :
           trunksArray
           allMembersIsLocalArray
        */
        genTrunkDbPtr->shadowValid = GT_TRUE;
    }

    return rc;
}

/*******************************************************************************
* trunkDbMembersFromStartIndexSort
*
* DESCRIPTION:
*       perform sort action from the index of trunk members according to number of members.
*       sorting according to ascending device number and then by ascending port number.
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       trunkInfoPtr - pointer to trunk info
*       startIndex - start index in the trunk
*       numMembers - num of members in the trunk
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDbMembersFromStartIndexSort
(
    IN    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr,
    IN    GT_U32    startIndex,
    IN    GT_U32    numMembers
)
{
    GT_U32      ii,jj;
    CPSS_TRUNK_MEMBER_STC       tempMember;/* temporary member */
    GT_U32  finalIndex;  /* final index for a sorted member */
    GT_U32  bmpOfUnSortedMembers = BIT_MASK_MAC(numMembers);/* bmp of unsorted members - set bits 0..(numMembers-1) as 1*/
    CPSS_TRUNK_MEMBER_STC *membersArray = &trunkInfoPtr->membersPtr[startIndex];
    GT_BOOL *isLocalMembersPtr = &trunkInfoPtr->isLocalMembersPtr[startIndex];
    GT_BOOL tempIsLocalMember;

    if(numMembers <= 1)
    {
        /* no sorting needed for no members or single member */
        return GT_OK;
    }

    ii = 0;

    while(bmpOfUnSortedMembers)
    {
        /*************************************/
        /* look for next member to sort [ii] */
        /*************************************/
        do
        {
            if(ii == numMembers)
            {
                /* cyclic loop - go to start */
                ii = 0;
            }

            if(U32_GET_FIELD_MAC(bmpOfUnSortedMembers,ii,1))
            {
                /* found index with unsorted member */
                /* do not increment ii in this case , because we will need (maybe)
                   to re-sort the new member in this index */
                break;
            }

        }while(++ii);/* the 'while' is 'forever' , use it to also increment ii */

        /********************************************************/
        /* calculate final index for the current member to sort */
        /* current member to sort is in [ii]                    */
        /********************************************************/
        finalIndex = 0;

        for(jj = 0 ; jj < numMembers; jj++)
        {
            if(ii == jj)
            {
                continue;
            }

            if((membersArray[ii].hwDevice > membersArray[jj].hwDevice) ||
               (membersArray[ii].hwDevice == membersArray[jj].hwDevice &&
                membersArray[ii].port > membersArray[jj].port))
            {
                finalIndex++;
            }
        }

        if(finalIndex != ii)
        {
            /*****************************************************/
            /* swap places of members in [ii] and in [finalIndex]*/
            /*****************************************************/

            /* save the value for the swapping */
            tempMember = membersArray[finalIndex];
            /* update the array of the ports */
            membersArray[finalIndex] = membersArray[ii];
            /* complete the swapping */
            membersArray[ii] = tempMember;

            /* swap also the info in isLocalMembersPtr[] */
            tempIsLocalMember = isLocalMembersPtr[finalIndex];
            isLocalMembersPtr[finalIndex] = isLocalMembersPtr[ii];
            isLocalMembersPtr[ii] = tempIsLocalMember;
        }

        /* state that the member in [finalIndex] not need to be sorted again */
        U32_SET_FIELD_MAC(bmpOfUnSortedMembers,finalIndex,1,0);
    }

    /*********************************/
    /* check that the sorting was OK */
    /*********************************/
    for(ii = 0 ; ii < (numMembers-1) ; ii++)
    {
        jj = ii + 1;
        /* compare member with the member next in array */
        if((membersArray[ii].hwDevice > membersArray[jj].hwDevice) ||
           (membersArray[ii].hwDevice == membersArray[jj].hwDevice &&
            membersArray[ii].port > membersArray[jj].port))
        {
            /* if we got here we have bug in the sort algorithm above*/
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_SET_ERROR, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* trunkDbMembersForTrunkIdSort
*
* DESCRIPTION:
*       perform sort action on the trunk members.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       trunkId - the trunkId
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDbMembersForTrunkIdSort
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId
)
{
    GT_STATUS rc;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* sort the enabled members */
    rc = trunkDbMembersFromStartIndexSort(trunkInfoPtr,0,trunkInfoPtr->enabledCount);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* sort the disabled members */
    rc = trunkDbMembersFromStartIndexSort(trunkInfoPtr,trunkInfoPtr->enabledCount,trunkInfoPtr->disabledCount);
    return rc;
}

/*******************************************************************************
* trunkMembersWorkAroundGet
*
* DESCRIPTION:
*       perform actions to Achieve a better load balancing in UC traffic.
*       the "workaround" is for twist/D --- not needed for Salsa devices.
*       this "workaround" is only applicable for uc traffic. the changes
*       are only affect the HW, the SW shadow reflects the original trunk.
*       The goal of this function is to configure the devices Hw trunk
*       membership configuration, in a way that ensures that trunks with 2 or 3
*       or 4 members have equal distribution among the trunk members.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*       membersArray - array of original enabled members of the trunk
*       numEnabledMembersPtr - (pointer to) num of enabled members in the trunk
*
* OUTPUTS:
*       membersArray - array of "workaround" enabled members of the trunk for HW
*       numEnabledMembersPtr - (pointer to) num of enabled members in the trunk
*                      for HW
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkMembersWorkAroundGet
(
    IN    GT_U8                       devNum,
    INOUT CPSS_TRUNK_MEMBER_STC       membersArray[PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS],
    INOUT GT_U32                      *numEnabledMembersPtr
)
{
    GT_U32      ii;/* iterator */
    GT_U32      numEnabledMembers = *numEnabledMembersPtr;/*num of enabled
                    members in the trunk in Hw*/
    CPSS_TRUNK_MEMBER_STC tmpMembersArray[PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];/* temporary
                                                       array of trunk members */
    GT_U8       *memberIndexPtr = NULL;/* (pointer to) chosen index for member
                                        in the trunk */
    GT_BOOL     force8Members;/* forcing 8 members in trunk */

    force8Members = (PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->loadBalanceType ==
                        PRV_CPSS_TRUNK_LOAD_BALANCE_SX_EMULATION_E ||
                     PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->loadBalanceType ==
                        PRV_CPSS_TRUNK_LOAD_BALANCE_FORCE_8_MEMBERS_E) ?
                            GT_TRUE : GT_FALSE;

    if(numEnabledMembers == 0)
    {
        membersArray[0].port   = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->nullPort;/* null port */
        membersArray[0].hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);

        if(force8Members == GT_FALSE)
        {
            *numEnabledMembersPtr = 1;

            return GT_OK;
        }
    }

    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->loadBalanceType ==
       PRV_CPSS_TRUNK_LOAD_BALANCE_NATIVE_E)
    {
        return GT_OK;
    }
    else if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->loadBalanceType ==
       PRV_CPSS_TRUNK_LOAD_BALANCE_WA_TWIST_C_D_SAMBA_E)
    {
        if(numEnabledMembers == 2)
        {
            memberIndexPtr = (GT_U8*)trunkMembersWorkAround_2Ports;
        }
        else if(numEnabledMembers == 3)
        {
            memberIndexPtr = (GT_U8*)trunkMembersWorkAround_3Ports;
        }
        else if(numEnabledMembers == 4)
        {
            memberIndexPtr = (GT_U8*)trunkMembersWorkAround_4Ports;
        }
        else
        {
            /* Attention --- if enabledMembersCounter > 4 the PP will not be doing
            the load balance correct !!!! */
            return GT_OK;
        }
    }
    else if(force8Members == GT_TRUE)
    {
        switch(numEnabledMembers)
        {
            case 0:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_0Ports;
                break;
            case 1:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_1Ports;
                break;
            case 2:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_2Ports;
                break;
            case 3:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_3Ports;
                break;
            case 4:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_4Ports;
                break;
            case 5:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_5Ports;
                break;
            case 6:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_6Ports;
                break;
            case 7:
                memberIndexPtr =
                    (GT_U8*)trunkMembersForceRedandentTo8Members_7Ports;
                break;
            case 8:
                /* no need to correct the members !!! */
                return GT_OK;
            default:
                /* should not happen */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk >
       PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS)
    {
        /* should not happen */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    /* calculate updated members */
    for(ii = 0 ; ii < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS ; ii++)
    {
        tmpMembersArray[ii] = membersArray[(memberIndexPtr[ii])];
    }

    /* update the info for the OUT parameters */
    *numEnabledMembersPtr = PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS;

    for(ii = 0 ; ii < PRV_CPSS_TRUNK_8_MAX_NUM_OF_MEMBERS_CNS ; ii++)
    {
        membersArray[ii] = tmpMembersArray[ii];
    }

    return GT_OK;
}

/*******************************************************************************
* trunkPortTrunkIdSet
*
* DESCRIPTION:
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portId  - the port number.
*       memberOfTrunk - is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*
*       trunkId - the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*
*                 this value relevant only when memberOfTrunk = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS trunkPortTrunkIdSet
(
    IN GT_U8                            devNum,
    IN GT_PHYSICAL_PORT_NUM             portId,
    IN GT_BOOL                          memberOfTrunk,
    IN GT_TRUNK_ID                      trunkId
)
{

    if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsTrunkIdByApplication),portId))
    {
        /* this port is managed by the application , CPSS not modify the HW !
           application must use the low level API to set trunkId ... as needed.
        */
        return GT_OK;
    }

    /* call the virtual function */
    return TRUNK_VIRTUAL_FUNC_MAC(devNum)->portTrunkIdSetFunc(devNum,portId,
                                           memberOfTrunk,trunkId);
}

/*******************************************************************************
* Function: trunkNonTrunkPortUpdateHw
*
* DESCRIPTION:
*       update the trunk's non-trunk members specific entry registers
*           -- HW operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device to update.
*       trunkId     - the trunk to act on.
*       memberPtr   - (pointer to)the member to act on .
*                      this pointer is used only when the action is one of:
*                      PRV_CPSS_TRUNK_ACTION_REMOVE_E or
*                      PRV_CPSS_TRUNK_ACTION_DISABLE_E
*                      so pointer can be NULL
*
*       nonTrunkPortsPtr - (pointer to) non trunk ports for the trunk
*                      this bmp is calculated by SW to reflect only the trunk
*                      members (so the trunk members bits are cleared).
*                      this bmp is NOT reflecting the ports that the application
*                      manipulated via the "add/remove port to/from non-trunk
*                      members"
*       action      -  action type currently done by the trunk lib
*       isLocalMember - indication that the removed member is local to this device
*                           GT_TRUE - the member is local to this device
*                           GT_FALSE - the member is NOT local to this device
*                       NOTE: relevant only to actions :
*                           PRV_CPSS_TRUNK_ACTION_REMOVE_E
*                           PRV_CPSS_TRUNK_ACTION_DISABLE_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       this function will read the actual non-trunk members from HW in order to
*       know what extra ports application added to the non trunk members.
*
*******************************************************************************/
static GT_STATUS trunkNonTrunkPortUpdateHw
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsPtr,
    IN PRV_CPSS_TRUNK_ACTION_ENT action,
    IN GT_BOOL                  isLocalMember
)
{
    GT_STATUS rc;    /* return error core */
    CPSS_PORTS_BMP_STC  hwNonTrunkMembers;/* non-trunk members from HW */
    CPSS_PORTS_BMP_STC extendedTrunkMembers;/* "trunk members" that are not
             really trunk members but added to the "nonTrunkMembers" indirect */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/
    CPSS_PORTS_BMP_STC tmpPortsBmp;

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);

    /* when action is : initialize or clear the trunk
       we don't care about data from HW , because we must set it to the needed
       value --- all ports must be non trunk members at this time

       also true when we set full members as single action
       */
    if((action != PRV_CPSS_TRUNK_ACTION_CLEAR_TRUNK_E ) &&
       (action != PRV_CPSS_TRUNK_ACTION_INITIALIZATION_E) &&
       (action != PRV_CPSS_TRUNK_ACTION_SET_KEEP_THE_CURRENT_TRUNK_E))
    {
        /* first -- read from HW -- fix bug #31281 */
        /* call appropriate virtual function */
        rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpGetFunc(devNum,
                trunkId,&hwNonTrunkMembers);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* calc what ports are down in the hwNonTrunkMembers
                           but up in the nonTrunkPorts */

        /* get the "members" that the HW has */
        CPSS_PORTS_BMP_BITWISE_NOT_MAC(&extendedTrunkMembers,&hwNonTrunkMembers);

        /* remove from those "members" the real members */
        CPSS_PORTS_BMP_BITWISE_AND_MAC(&extendedTrunkMembers,&extendedTrunkMembers,nonTrunkPortsPtr);

        if(action == PRV_CPSS_TRUNK_ACTION_REMOVE_E ||
           action == PRV_CPSS_TRUNK_ACTION_DISABLE_E)
        {
            /* now we need to check about the "port in action" */
            if(isLocalMember == GT_TRUE)
            {
                /* if a port is being removed/disabled :
                   remove it from the "extra members of the trunk"
                */
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&extendedTrunkMembers,memberPtr->port);
            }
        }

        /* the final action is to "remove" the "extended trunk members" from
           the non trunk members
        */
        CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(nonTrunkPortsPtr,nonTrunkPortsPtr,&extendedTrunkMembers);

        /* now we are ready to write to HW */
    }
    else
    {
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&extendedTrunkMembers);
    }

    if(trunkInfoPtr && /* during initialization the DB is still down */
       trunkInfoPtr->allowMultiDestFloodBack == GT_TRUE)
    {
        /* this trunk wish to flood back to it */
        /* so the reference 'non-trunk' entry should be 'all ones' , and not
           as calculated by the DB */

        /* set 'all ones' */
        PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&tmpPortsBmp);

        /* remove the port which was 'removed from trunk' or the ports set outside
           the 'high level APIs' */
        CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,&tmpPortsBmp,&extendedTrunkMembers);

        /* update the 'pointer' (nonTrunkPortsPtr) But not it's content (ports bmp) ! .
           NOTE : this change not effect the value in the
           function which called the trunkNonTrunkPortUpdateHw ! */
        nonTrunkPortsPtr = &tmpPortsBmp;
    }

    /* call appropriate virtual function */
    return TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpSetFunc(devNum,
            trunkId, nonTrunkPortsPtr);
}


/*******************************************************************************
* trunkTableUpdateHw
*
* DESCRIPTION:
*       set the trunk table entry -- HW operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device to update.
*       trunkId     - trunk id
*       enabledMembersPtr - array of enabled members of the trunk
*       numOfEnabledMembers - num of enabled members in the trunk
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkTableUpdateHw
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    *enabledMembersPtr
)
{
    GT_STATUS rc;    /* return error core */
    CPSS_TRUNK_MEMBER_STC  *membersPtr;/* (pointer to) member in the trunk */
    CPSS_TRUNK_MEMBER_STC  tmpMembersArray[PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                 tmpNumMembers = numOfEnabledMembers;
    GT_U32                 ii;

    for(ii = 0; ii <  numOfEnabledMembers ; ii++)
    {
        /* fix bug #28113 :
           since the function trunkMembersWorkAroundGet(...)
           may change the array of ports , we dont want the change to be on
           the original array !
        */
        tmpMembersArray[ii] = enabledMembersPtr[ii];
    }

    membersPtr = tmpMembersArray;

    /* let the work around figure real num of ports to write to HW */
    rc = trunkMembersWorkAroundGet(devNum,membersPtr,&tmpNumMembers);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* call appropriate virtual function */
    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->membersSetFunc(devNum,
            trunkId,tmpNumMembers,membersPtr);

    return rc;
}

/*******************************************************************************
* trunk3PortsWorkAroundIndexGet
*
* DESCRIPTION: calculate the index of the member in the trunk to be selected to
*              the designated trunk table
*
*    fix bug# 23836
*    when there are only 3 ports in the trunk the
*      "regular" filling of the designated trunk table is not
*      good enough !!!
*
*    Trunk Hash function has next properties:
*       1. the hash function based on :
*          a. src port
*          b. src device
*          c. vid
*          d. vidx (reversed bits !!!)
*       2. when vid == vidx :
*          a. the hash return only 4 different values (out of 16)
*             for specific src port and device !!!
*          b. there only 4 sets of different sets of values !!
*
*          meaning that the hash values are:
*          set0: 0, 6, 9 , 15
*          set1: 1, 7, 8 , 14
*          set2: 2, 4, 11, 13
*          set3: 3, 5, 10, 12
*       3. when vid != vidx :
*          the hash return all 16 different values (out of 16) .
*          for specific src port and device.
*
*
*
*       Conclusions:
*       the balance need to be
*       (remember that there are 3 ports in trunk):
*
*       hash entry | trunks port  | belongs to set
*       ------------------------------------------
*          0       |   0          |    0
*          1       |   1          |    1
*          2       |   2          |    2
*          3       |   0          |    3
*          4       |   0          |    2
*          5       |   1          |    3
*          6       |   1          |    0
*          7       |   2          |    1
*          8       |   0          |    1
*          9       |   2          |    0
*          10      |   2          |    3
*          11      |   1          |    2
*          12      |   0          |    3
*          13      |   2          |    2
*          14      |   1          |    1
*          15      |   0          |    0
*
*       Entries should be filled by setting increment port of trunk
*       in a specific set :
*
*       for example:
*
*       set0 is hash entries : 0, 6, 9 , 15
*       so trunk ports are   : 0, 1, 2,  0
*
*      set1                  : 1, 7, 8 , 14
*      so trunk ports are    : 1, 2, 0 , 1
*
*      set2                  : 2, 4, 11, 13
*      so trunk ports are    : 2, 0,  1, 2
*
*      set3                  : 3, 5, 10, 12
*      so trunk ports are    : 0, 1,  2, 0
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       hashIndex  - the index in the designated trunk table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       index of the member in the trunk to be selected to the designated trunk
*       table
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32  trunk3PortsWorkAroundIndexGet(
    IN GT_U32      hashIndex
)
{
    return designated3PortsIndexArray[hashIndex];
}

/*******************************************************************************
* trunkDesignatedPortsUpdateHw
*
* DESCRIPTION:
*       set the designated trunk table entry in HW .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - the device number to write to.
*       entryIndex      - the HW index to write to
*       designatedPortsPtr - (pointer to)designated ports
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDesignatedPortsUpdateHw
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    IN  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
{
    /* call appropriate virtual function */
    return  TRUNK_VIRTUAL_FUNC_MAC(devNum)->designatedMembersBmpSetFunc(devNum,
                                            entryIndex,designatedPortsPtr);
}

/*******************************************************************************
* trunkDesignatedPortsGetHw
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE          - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDesignatedPortsGetHw
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    OUT  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
{
    /* call appropriate virtual function */
    return  TRUNK_VIRTUAL_FUNC_MAC(devNum)->designatedMembersBmpGetFunc(devNum,
                                            entryIndex,designatedPortsPtr);
}


/*******************************************************************************
* trunkDesignatedPortsTableUpdate
*
* DESCRIPTION:
*       rewrite the designated ports table due to adding/remove enabled member
*       to/from trunk
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - the trunk id
*       numOfEnabledMembers - num of enabled members in the trunk
*       enabledMembersPtr - array of enabled members of the trunk
*       nonTrunkPortsPtr - non trunk ports for the trunk
*       localPortsRemovedPtr - (pointer to) bmp of local port removed from trunk
*                           ignored when NULL
*       numOfWeightedMembers - number of members in the array weightedMembersArray.
*                   value 0 - meaning that this trunk not used with weights
*       weightedMembersArray - (array of) members of the weighted cascade trunk.
*                              each member hold relative weight (relative to Weight of all members)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDesignatedPortsTableUpdate
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    *enabledMembersPtr,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsPtr,
    IN CPSS_PORTS_BMP_STC       *localPortsRemovedPtr,
    IN GT_U32                   numOfWeightedMembers,
    IN CPSS_TRUNK_WEIGHTED_MEMBER_STC       weightedMembersArray[]

)
{
    GT_STATUS rc = GT_OK;    /* return error core */
    GT_U32    entryIndex;/* index of entry in the designated ports trunk table */
    GT_U32      currentSelectedIndex;/* current selected index of the port in
                                        the trunk to be designated port in
                                        the designated trunk table*/
    CPSS_PORTS_BMP_STC designatedPorts;/* the designated ports */
    CPSS_PORTS_BMP_STC *designatedPortsPtr;/* (pointer to) the designated ports
                        of specific entry in the designated ports trunk table*/
    GT_BOOL     doTrunkDesignatedTableWorkAround;/* do 3 ports work around */
    GT_U32  numDesig;/* number of entries in the designated ports trunk table */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    GT_BOOL designatedMemberExists;/* do we have designated member */
    GT_BOOL useDesignatedMember = GT_FALSE;/*relevant when designatedMemberExists = GT_TRUE
                                and means that we need to use/not the designated member
                                in this device , due to :
                                member in the device , member in the current trunk */
    GT_BOOL useNonTrunkPortsForEnabled;/* use non-trunk ports instead of the :numOfEnabledMembers,enabledMembersPtr
                           to know the members of the trunk... used for cascade trunk */
    GT_U32  portOnLocalDev = 0;/* port on local device iterator */
    CPSS_PORTS_BMP_STC fullPortsBmp; /* full ports bmp of this device */
    CPSS_PORTS_BMP_STC tmpPortsBmp; /* temp ports bmp  */
    GT_U32 *tmpWeightsArr2 = NULL;    /* array2 of temporary weights*/
    GT_U32 *tmpWeightsArr4 = NULL;    /* array4 of temporary weights*/
    GT_U32  totalWeight;/* total weight of the members */
    GT_U32  loopCount;/* number of loop counting to prohibit endless loop*/
    GT_U32  accumulatedPortsSkip; /* counting ports skipped steps due to MC disable */
    GT_BOOL currentDesigMcTable = GT_FALSE; /* indication that we config the part of 'MC designated table' .
                                    can be GT_TRUE only for devices with 'split' table */
    GT_BOOL currentDesigUcTable = GT_FALSE; /* indication that we config the part of 'UC designated table' .
                                    can be GT_TRUE only for devices with 'split' table */

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);
    numDesig = PRV_CPSS_TRUNK_DESIGNATED_TABLE_SIZE_MAC(devNum);

    if(numOfWeightedMembers)
    {
        if(numOfWeightedMembers > numDesig)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        tmpWeightsArr2 = cpssOsMalloc(numDesig             * sizeof(tmpWeightsArr2[0]));
        if(tmpWeightsArr2 == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }
        tmpWeightsArr4 = cpssOsMalloc(numOfWeightedMembers * sizeof(tmpWeightsArr4[0]));
        if(tmpWeightsArr4 == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        if(numOfWeightedMembers == 1)
        {
            /* ignore the weight info when single member */
            for(entryIndex = 0; entryIndex < numDesig ; entryIndex++)
            {
                tmpWeightsArr2[entryIndex] = 0;
            }
        }
        else /*numOfWeightedMembers != 1*/
        {
            totalWeight = 0;
            /* init tmpWeightsArr4 */
            for(entryIndex = 0 ; entryIndex < numOfWeightedMembers ; entryIndex++)
            {
                tmpWeightsArr4[entryIndex] = weightedMembersArray[entryIndex].weight;

                /* convert weight to exact number of occurrences */
                totalWeight += tmpWeightsArr4[entryIndex];
            }

            for(entryIndex = 0 ; entryIndex < numOfWeightedMembers ; entryIndex++)
            {
                /* convert weight to exact number of occurrences */
                tmpWeightsArr4[entryIndex] *= (numDesig / totalWeight);
            }

            /* init tmpWeightsArr2 */
            currentSelectedIndex = numOfWeightedMembers - 1;/* after ++ it will be 0 */
            for(entryIndex = 0; entryIndex < numDesig ; entryIndex++)
            {
                /* find next index in tmpWeightsArr4 to use */
                do{
                    currentSelectedIndex++;
                    if(currentSelectedIndex == numOfWeightedMembers)
                    {
                        currentSelectedIndex = 0;
                    }
                }while(tmpWeightsArr4[currentSelectedIndex] == 0);

                tmpWeightsArr2[entryIndex] = currentSelectedIndex;

                /* state that this weight was used */
                tmpWeightsArr4[currentSelectedIndex]--;
            }
        } /*numOfWeightedMembers != 1*/
    }
    else/* numOfWeightedMembers == 0 */
    {
        tmpWeightsArr2 = NULL;
        tmpWeightsArr4 = NULL;
    }/* numOfWeightedMembers == 0 */


    if(trunkInfoPtr->type == CPSS_TRUNK_TYPE_CASCADE_E)
    {
        numOfEnabledMembers = 0;

        PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&fullPortsBmp);

        CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,&fullPortsBmp,nonTrunkPortsPtr);

        if(CPSS_PORTS_BMP_IS_ZERO_MAC(&tmpPortsBmp))
        {
            /* trunk is empty */
            useNonTrunkPortsForEnabled = GT_FALSE;
        }
        else
        {
            useNonTrunkPortsForEnabled = GT_TRUE;
        }
    }
    else
    {
        useNonTrunkPortsForEnabled = GT_FALSE;
    }

    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->doDesignatedTableWorkAround &&
       numOfEnabledMembers == 3)
    {
        doTrunkDesignatedTableWorkAround = GT_TRUE;
    }
    else
    {
        doTrunkDesignatedTableWorkAround = GT_FALSE;
    }

    currentSelectedIndex = 0;

    if(trunkInfoPtr->designatedMemberExists == GT_TRUE)
    {
        designatedMemberExists = GT_TRUE;

        /* check that the designated member is enabled in trunk and is local to
           the device */
        useDesignatedMember = trunkDbDesignatedMemberIsEnabledAndLocal(devNum,trunkInfoPtr);
    }
    else
    {
        designatedMemberExists = GT_FALSE;
    }

    designatedPortsPtr = &designatedPorts;

    accumulatedPortsSkip = 0;

    /* pointer to start of the DB table */
    for(entryIndex = 0;
         entryIndex < PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw;
         entryIndex++)
    {
        /* get "existing" entry value */
        rc = trunkDesignatedPortsGetHw(devNum,entryIndex,designatedPortsPtr);
        if (rc != GT_OK)
        {
            goto cleanExit_lbl;
        }

        if(localPortsRemovedPtr)
        {
            /* port removed from the trunk --> add it to the bmp */
            CPSS_PORTS_BMP_BITWISE_OR_MAC(designatedPortsPtr,designatedPortsPtr,localPortsRemovedPtr);
        }

        /* remove local ports from the designated entry */
        CPSS_PORTS_BMP_BITWISE_AND_MAC(designatedPortsPtr,designatedPortsPtr,nonTrunkPortsPtr);

        if (PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->designatedTrunkTableSplit == GT_TRUE)
        {
            if (entryIndex >= numDesig)
            {
                currentDesigMcTable = GT_TRUE;
                currentDesigUcTable = GT_FALSE;
            }
            else
            {
                currentDesigMcTable = GT_FALSE;
                currentDesigUcTable = GT_TRUE;
            }
        }

        loopCount = 0;

        if(designatedMemberExists == GT_TRUE)
        {
            /* if designated port is in local device and is enabled member in the trunk */
            if(useDesignatedMember == GT_TRUE)
            {
                /* set the local port to be designated for the trunk in ALL
                   entries of designated table */
                CPSS_PORTS_BMP_PORT_SET_MAC(designatedPortsPtr,
                    trunkInfoPtr->designatedMember.port);
            }
        }
        /* handle weighted members */
        else if(numOfWeightedMembers && tmpWeightsArr2)
        {
            /* get the index to select member in weightedMembersArray[] */
            currentSelectedIndex = tmpWeightsArr2[entryIndex % numDesig];

            if((trunkInfoPtr->type == CPSS_TRUNK_TYPE_CASCADE_E) ||   /*cascade trunk*/
               (GT_TRUE == trunkDbMemberIsLocal(trunkInfoPtr,&weightedMembersArray[currentSelectedIndex].member)))/*regular trunk*/
            {
                /* add the local port to be designated for the trunk in this
                   entry of designated table */
                CPSS_PORTS_BMP_PORT_SET_MAC(designatedPortsPtr,
                    weightedMembersArray[currentSelectedIndex].member.port);
            }
        }
        /* we need to select new designated port */
        else if(numOfEnabledMembers)
        {
            if(doTrunkDesignatedTableWorkAround == GT_TRUE)
            {
                currentSelectedIndex =
                    trunk3PortsWorkAroundIndexGet(entryIndex);
            }
            else
            {
                if(currentDesigMcTable == GT_TRUE)
                {
                    /* the MC part of designated table */

                    /* force needed ports into the entry.
                       The load balance done for the other ports as if those 'forced ports'
                       are not in the trunk !!!
                       meaning we will have more then single port in the entry */

                    /* get the relevant ports for current trunk from portsMcForce */
                    /* tmpPortsBmp =  portsMcForce & (~nonTrunkPorts)*/
                    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,
                        &PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForce,
                        nonTrunkPortsPtr);

                    /* force ports member to the trunk :
                        designatedPorts = designatedPorts | tmpPortsBmp */
                    CPSS_PORTS_BMP_BITWISE_OR_MAC(designatedPortsPtr,designatedPortsPtr,&tmpPortsBmp);

                    accumulatedPortsSkip = accumulatedPortsSkip % numOfEnabledMembers;
                    currentSelectedIndex = ((entryIndex - numDesig) + accumulatedPortsSkip) % numOfEnabledMembers;

                    /* skip ports that are in 'forbid' mode */
                    /* skip ports that are in 'force' mode --- already added ! */
                    /* NOTE: those cases are suitable for cascade trunk only */
                    while((GT_TRUE == trunkDbMemberIsLocal(trunkInfoPtr,&enabledMembersPtr[currentSelectedIndex])) &&
                           (CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForbid,/*forbid*/
                                                           enabledMembersPtr[currentSelectedIndex].port) ||
                            CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForce, /*force*/
                                                           enabledMembersPtr[currentSelectedIndex].port)) &&
                           (loopCount != numOfEnabledMembers))
                    {
                        currentSelectedIndex = (currentSelectedIndex + 1) % numOfEnabledMembers;
                        accumulatedPortsSkip++;
                        loopCount++;
                    }
                }
                else
                {
                    if (currentDesigUcTable == GT_TRUE)
                    {
                        /* For device with split designated table the low half of */
                        /* the designated table used for unicast packets with the    */
                        /* remote destination device when path to the destination    */
                        /* device is mapped to the trunk.                            */
                        /* Put just all enabled local ports ignoring remote ports    */
                        /* instead of skipping remote ports.                         */
                        /* Each table line will contain one local port.              */

                        /* init or increment currentSelectedIndex */
                        if (entryIndex == 0)
                        {
                            currentSelectedIndex = 0;
                        }
                        else
                        {
                            currentSelectedIndex =
                                (currentSelectedIndex + 1) % numOfEnabledMembers;
                        }
                        /* look for next local port                          */
                        /* if all ports are remote currentSelectedIndex will */
                        /* point to remote port that will be filtered below  */
                        loopCount = 0;
                        while((GT_FALSE == trunkDbMemberIsLocal(
                            trunkInfoPtr, &enabledMembersPtr[currentSelectedIndex]))
                               && (loopCount < numOfEnabledMembers))
                        {
                            currentSelectedIndex = (currentSelectedIndex + 1) % numOfEnabledMembers;
                            loopCount++;
                        }
                        /* restore variable for MC part of table */
                        loopCount = 0;
                    }
                    else
                    {
                        /* case of device with not split designated table */
                        currentSelectedIndex = entryIndex % numOfEnabledMembers;
                    }
                }
            }

            if((GT_TRUE == trunkDbMemberIsLocal(trunkInfoPtr,&enabledMembersPtr[currentSelectedIndex])) &&
               (loopCount != numOfEnabledMembers))
            {
                /* add the local port to be designated for the trunk in this
                   entry of designated table */
                CPSS_PORTS_BMP_PORT_SET_MAC(designatedPortsPtr,
                    enabledMembersPtr[currentSelectedIndex].port);
            }
        }
        else if(useNonTrunkPortsForEnabled == GT_TRUE)
        {
            if(entryIndex == numDesig)
            {
                portOnLocalDev = 0;
            }

            if(currentDesigMcTable == GT_TRUE)
            {
                /* the MC part of designated table */

                /* force needed ports into the entry.
                   The load balance done for the other ports as if those 'forced ports'
                   are not in the trunk !!!
                   meaning we will have more then single port in the entry */

                /* get the relevant ports for current trunk from portsMcForce */
                /* tmpPortsBmp =  portsMcForce & (~nonTrunkPorts)*/
                CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,
                    &PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForce,
                    nonTrunkPortsPtr);

                /* force ports member to the trunk :
                    designatedPorts = designatedPorts | tmpPortsBmp */
                CPSS_PORTS_BMP_BITWISE_OR_MAC(designatedPortsPtr,designatedPortsPtr,&tmpPortsBmp);
            }

            for(/* continue*/ ;portOnLocalDev <= MAX_MEMBER_PORT_NUM_MAC(devNum) ; portOnLocalDev++, loopCount++)
            {
                if( loopCount == MAX_MEMBER_PORT_NUM_MAC(devNum))
                {
                    break;
                }

                if(portOnLocalDev == MAX_MEMBER_PORT_NUM_MAC(devNum))
                {
                    portOnLocalDev = 0;
                }

                if(0 == USE_VIRTUAL_PORT_MAC(devNum))
                {
                    if(IS_PHY_PORT_BY_MASK_MAC(devNum))
                    {
                        SKIP_NON_VALID_PHY_PORT_BY_MASK_MAC(devNum,portOnLocalDev);
                    }
                    else
                    if(!CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                        &PRV_CPSS_PP_MAC(devNum)->existingPorts,portOnLocalDev))
                    {
                        continue;
                    }

                    if(currentDesigMcTable == GT_TRUE)
                    {
                        /* skip ports that are in 'forbid' mode */
                        /* skip ports that are in 'force' mode --- already added ! */
                        /* NOTE: those cases are suitable for cascade trunk only */
                        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                            &PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForbid,
                                                                portOnLocalDev))
                        {
                            continue;
                        }

                        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                            &PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForce,
                                                                portOnLocalDev))
                        {
                            continue;
                        }
                    }
                }

                if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(nonTrunkPortsPtr,portOnLocalDev))
                {
                    /* this is trunk member , and should be in the load balance of the ports */
                    break;
                }
            }

            if( loopCount != MAX_MEMBER_PORT_NUM_MAC(devNum))
            {
                /* add the local port to be designated for the trunk in this
                    entry of designated table */
                CPSS_PORTS_BMP_PORT_SET_MAC(designatedPortsPtr,portOnLocalDev);

                portOnLocalDev++;
            }
        }

        /* update hw */
        rc = trunkDesignatedPortsUpdateHw(devNum,entryIndex,designatedPortsPtr);
        if (rc != GT_OK)
        {
            goto cleanExit_lbl;
        }
    }

    /*label for clean exit... not forget free dynamic allocation (if was any) */
cleanExit_lbl:
    if(tmpWeightsArr2)
    {
        cpssOsFree(tmpWeightsArr2);
    }

    if(tmpWeightsArr4)
    {
        cpssOsFree(tmpWeightsArr4);
    }

    return rc;
}


/*******************************************************************************
* trunkTableHwInit
*
* DESCRIPTION:
*       init the trunk table entry -- HW operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device to update.
*
* OUTPUTS:
*       None.
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkTableHwInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS rc;    /* return error core */
    GT_TRUNK_ID  trunkId; /* trunk id */

    for(trunkId = 1 ;
        trunkId <= PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw ;
        trunkId++)
    {
        rc = trunkTableUpdateHw(devNum,trunkId,0,NULL);/* no members */
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* trunkNonTrunkPortsHwInit
*
* DESCRIPTION:
*       init non trunk members for all trunks -- in HW
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - the device to write to.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkNonTrunkPortsHwInit
(
    IN GT_U8    devNum

)
{
    GT_STATUS rc;    /* return error core */
    GT_TRUNK_ID  trunkId; /* trunk id */
    CPSS_PORTS_BMP_STC fullPortsBmp;

    PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&fullPortsBmp);

    for(trunkId = 0 ;
        trunkId <= PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw ;
        trunkId++)
    {
        rc = trunkNonTrunkPortUpdateHw(devNum,trunkId,
                        NULL,/* don't care on initialization */
                        &fullPortsBmp,
                        PRV_CPSS_TRUNK_ACTION_INITIALIZATION_E,/*initialization*/
                        GT_FALSE);/* don't care on initialization */
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}


/*******************************************************************************
* trunkDesignatedPortsTableHwInit
*
* DESCRIPTION:
*       init designated ports trunk table --  HW operation
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDesignatedPortsTableHwInit
(
    IN GT_U8    devNum
)
{
    GT_STATUS rc;    /* return error core */
    GT_U32    entryIndex;/* index of entry in the designated ports trunk
                           table */
    CPSS_PORTS_BMP_STC fullPortsBmp;

    PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&fullPortsBmp);

    for(entryIndex = 0 ;
        entryIndex < PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw ;
        entryIndex++ )
    {
        rc = trunkDesignatedPortsUpdateHw(devNum,entryIndex,&fullPortsBmp);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* trunkHwInit
*
* DESCRIPTION:
*       Trunk initialization of PP tables/registers .
*       all ports are set to non-trunk ports (for a single device).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - The device number to initialize.
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
static GT_STATUS trunkHwInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS                rc;    /* return error core */
    GT_PHYSICAL_PORT_NUM     port;/* port number */

    /* for case when init done from cascading */
    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->initDone == GT_TRUE)
    {
        return GT_OK;
    }

    /* init the mapping of port to trunk-id */
    for (port = 0 ; port < PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts ; port++)
    {
        if(0 == USE_VIRTUAL_PORT_MAC(devNum))
        {
            if(IS_PHY_PORT_BY_MASK_MAC(devNum))
            {
                SKIP_NON_VALID_PHY_PORT_BY_MASK_MAC(devNum,port);
            }
            else
            if(!CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                &PRV_CPSS_PP_MAC(devNum)->existingPorts,port))
            {
                continue;
            }
        }

        rc = trunkPortTrunkIdSet(devNum,port,GT_FALSE,0);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* init Trunk count and members tables */
    rc = trunkTableHwInit(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* init non trunk members */
    rc = trunkNonTrunkPortsHwInit(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* init the designated trunk table */
    rc = trunkDesignatedPortsTableHwInit(devNum);

    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->initDone = GT_TRUE;

    return GT_OK;
}



/*******************************************************************************
* trunkDbNonTrunkPortsCalc
*
* DESCRIPTION:
*       calculate the bitmap of non-trunk members
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device num
*       trunkId - the trunk id.
*
* OUTPUTS:
*       nonTrunkPortsPtr - (pointer to) non trunk members
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_BAD_PARAM - entry does not exist.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkDbNonTrunkPortsCalc
(
    IN  GT_U8                       devNum,
    IN  GT_U16                      trunkId,
    OUT CPSS_PORTS_BMP_STC          *nonTrunkPortsPtr
)
{
    GT_U32    ii;/* iterator */
    GT_U32    count;/* total number of ports in this trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    count = trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount;

    PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,nonTrunkPortsPtr);

    for(ii = 0 ; ii < count ;ii++)
    {
        if(trunkInfoPtr->isLocalMembersPtr[ii] == GT_FALSE)
        {
            continue;
        }

        /* remove this port from the bitmap */
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(nonTrunkPortsPtr,trunkInfoPtr->membersPtr[ii].port);
    }

    return GT_OK;
}

/*******************************************************************************
* trunkDbMembersAlreadySetCheck
*
* DESCRIPTION:
*       check it the action of setting trunk members is the same settings as the
*       DB hold .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       enabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfEnabledMembers = 0)
*       numOfEnabledMembers - number of enabled members in the array.
*       disabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfDisabledMembers = 0)
*       numOfDisabledMembers - number of enabled members in the array.
*
* OUTPUTS:
*       sameDbEntryPtr - (pointer to)Does the DB hold the same entry as the new
*                         settings.
*                         GT_TRUE - the DB has same setting
*                         GT_FALSE - the DB has different setting
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_OUT_OF_RANGE - when the sum of number of enabled members + number of
*                         disabled members exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM - bad trunkId number , or
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMembersAlreadySetCheck
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[],
    OUT GT_BOOL                 *sameDbEntryPtr
)
{
    GT_U32      ii;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC   *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    GT_BOOL  hwDevNumChanged;/*indication that the 'not equal'
                could have been 'equal' if we used the 'local member'
                indication*/

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    if((numOfEnabledMembers + numOfDisabledMembers) >
        PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    /* set that the DB is not like the new requested settings */
    *sameDbEntryPtr = GT_FALSE;

    if(numOfEnabledMembers  != trunkInfoPtr->enabledCount ||
       numOfDisabledMembers != trunkInfoPtr->disabledCount)
    {
        /* no need to keep look for match .
           there is difference */
        return GT_OK;
    }

    /* check the enabled members */
    for(ii = 0 ; ii < numOfEnabledMembers ; ii++)
    {
        if(GT_FALSE == trunkDbMemberCompareByIndex(devNum,trunkInfoPtr,
                ii,
                &enabledMembersArray[ii],
                &hwDevNumChanged) &&
           hwDevNumChanged == GT_FALSE)
        {
            /* no need to keep look for match .
               there is difference */
            return GT_OK;
        }
    }

    /* check the disabled members */
    for(ii = 0 ; ii < numOfDisabledMembers ; ii++)
    {
        if(GT_FALSE == trunkDbMemberCompareByIndex(devNum,trunkInfoPtr,
                        ii+numOfEnabledMembers,
                        &disabledMembersArray[ii],
                        &hwDevNumChanged) &&
           hwDevNumChanged == GT_FALSE)
        {
            /* no need to keep look for match .
               there is difference */
            return GT_OK;
        }
    }


    /* set that the DB is the same as the new requested settings */
    *sameDbEntryPtr = GT_TRUE;
    return GT_OK;

}

/*******************************************************************************
* trunkDbMemberMaskCheck
*
* DESCRIPTION:
*       set members in the DB for the trunk : enabled + disabled
*       -- override the last configuration
*
*       This function support next "set entry" options :
*       1. "reset" the entry
*          function will remove the previous settings
*       2. set entry after the entry was empty
*          function will set new settings
*       3. set entry with the same members that it is already hold
*          function will rewrite the HW entries as it was
*       4. set entry with different setting then previous setting
*          a. function will remove the previous settings
*          b. function will set new settings
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkMemberPtr  - pointer to the trunk member to check
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_BAD_PARAM - bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMemberMaskCheck
(
    IN GT_U8                    devNum,
    IN CPSS_TRUNK_MEMBER_STC    *trunkMemberPtr
)
{
    GT_U32  maskPort , maskDevice;/* mask for port , device */
    GT_HW_DEV_NUM           currMemberHwDevNum;/* HW devNum of current member of the trunk */
    GT_PHYSICAL_PORT_NUM    currMemberPortNum;/* port num of current member of the trunk */

    maskPort   = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->validityMask.port;
    maskDevice = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->validityMask.hwDevice;

    currMemberHwDevNum = trunkMemberPtr->hwDevice;
    currMemberPortNum  = trunkMemberPtr->port;

    /* for members that are 'Local to this device' we need to check that
       those refer to valid port . --> fix CQ#127954 , CQ#126994 */
    if(PRV_CPSS_HW_DEV_NUM_MAC(devNum) == trunkMemberPtr->hwDevice)
    {
        /******************************/
        /* NOTE : CPU port is invalid */
        /******************************/

        if(USE_VIRTUAL_PORT_MAC(devNum))
        {
            /* ExMxTg / ExMxPm - need to check 'virtual port' */
            PRV_CPSS_VIRTUAL_PORT_CHECK_MAC(devNum,trunkMemberPtr->port);
        }
        else
        if(maskPort >= PHY_PORT_ALL_RANGE_SUPPORTED_CNS)
        {
            /* no special existence needed ... (all ports in mask are valid) */
        }
        else
        {
            /* device not support 'virtual port' only physical ports. */
            PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,trunkMemberPtr->port);
        }
    }

    if(PRV_CPSS_IS_DUAL_HW_DEVICE_MAC(currMemberHwDevNum))
    {
        PRV_CPSS_DUAL_HW_DEVICE_CHECK_MAC(currMemberHwDevNum);
        /* the mask of the port add another bit */
        maskPort = (maskPort << 1) | 1;
        /* the mask of the device clears bit 0 */
        currMemberHwDevNum &= ~ 1;
    }

    /* check that member is valid to HW */
    if((currMemberPortNum & (~ maskPort)) ||
       (currMemberHwDevNum & (~maskDevice)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;

}

/*******************************************************************************
* trunkDbMembersValidityCheck
*
* DESCRIPTION:
*       set members in the DB for the trunk : enabled + disabled
*       -- override the last configuration
*
*       This function support next "set entry" options :
*       1. "reset" the entry
*          function will remove the previous settings
*       2. set entry after the entry was empty
*          function will set new settings
*       3. set entry with the same members that it is already hold
*          function will rewrite the HW entries as it was
*       4. set entry with different setting then previous setting
*          a. function will remove the previous settings
*          b. function will set new settings
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       enabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfEnabledMembers = 0)
*       numOfEnabledMembers - number of enabled members in the array.
*       disabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfDisabledMembers = 0)
*       numOfDisabledMembers - number of enabled members in the array.
*
* OUTPUTS:
*       None
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_OUT_OF_RANGE - when the sum of number of enabled members + number of
*                         disabled members exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_ALREADY_EXIST - one of the members already exists in another trunk
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMembersValidityCheck
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
)
{
    GT_STATUS   rc;
    GT_U32      ii;
    GT_TRUNK_ID existTrunkId;

    for(ii = 0 ; ii < numOfEnabledMembers ; ii++)
    {
        rc = trunkDbMemberMaskCheck(devNum,&enabledMembersArray[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* check if member already exists */
        rc = prvCpssGenericTrunkDbIsMemberOfTrunk(devNum,
                &enabledMembersArray[ii],&existTrunkId);
        if(rc == GT_OK && existTrunkId != trunkId)
        {
            /* the member exists in another trunk */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }
    }

    for(ii = 0 ; ii < numOfDisabledMembers ; ii++)
    {
        rc = trunkDbMemberMaskCheck(devNum,&disabledMembersArray[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if member already exists */
        rc = prvCpssGenericTrunkDbIsMemberOfTrunk(devNum,
                &disabledMembersArray[ii],&existTrunkId);
        if(rc == GT_OK && existTrunkId != trunkId)
        {
            /* the member exists in another trunk */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* trunkDbMemberAdd
*
* DESCRIPTION:
*       add port to the DB for the trunk
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       memberPtr   - trunk member info
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_ALREADY_EXIST - The members already exists in another trunk
*       GT_FULL - trunk is FULL and port not already in trunk
*       GT_FAIL - on error.
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMemberAdd(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_STATUS   rc; /* return value on error */
    GT_U32  count;/* total number of members in this trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    GT_BOOL hwDevNumChanged;/* indication that the 'found'
                            could was due to 'local member' indication
                            GT_TRUE - the member found only because we used 'local member'
                                      indication
                            GT_FALSE - the member found due to exact match*/
    GT_U32  ii;

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    /* check parameters validity */
    rc = trunkDbMembersValidityCheck(devNum,trunkId,1,memberPtr,0,NULL);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check if port already exists in this trunk */
    if(GT_OK == trunkDbMemberIndexGet(devNum,trunkInfoPtr,memberPtr,&ii,&hwDevNumChanged))
    {
        if(hwDevNumChanged == GT_TRUE)
        {
            /* the application try to add a member BUT ,
               but the 'similar' member was added to the trunk when an old
               hwDevNum of current local device */

            /* so trying to add this memberPtr to the trunk MUST not be success
               application must explicitly delete the member with the 'old hwDevNum'
            */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
        }

        /* member already exists in this trunk */
        return GT_OK;
    }

    count = trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount;

    if(count == PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
    }

    if(trunkInfoPtr->disabledCount)
    {
        /* fix bug # 28094 */
        /* push first disabled port to be last disabled port */
        /* make room for the new enabled port */
        trunkDbMemberCopy(trunkInfoPtr,count,trunkInfoPtr->enabledCount);
    }

    trunkDbMemberSet(devNum,trunkInfoPtr,trunkInfoPtr->enabledCount,memberPtr);
    LOCAL_MEMBER_DB_SET_MAC(devNum,trunkId,trunkInfoPtr,trunkInfoPtr->enabledCount,memberPtr->port);

    /*update DB counters*/
    trunkInfoPtr->enabledCount++ ;
    trunkInfoPtr->type = CPSS_TRUNK_TYPE_REGULAR_E;

    return GT_OK;
}


/*******************************************************************************
* trunkDbMemberDel
*
* DESCRIPTION:
*       remove port from the DB for the trunk
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       memberPtr   - trunk member info
*
* OUTPUTS:
*       isLocalMemberPtr  - (pointer to)indication that the removed member is local to this
*                           device
*                           IS_LOCAL_TRUE_E - the member is local to this device
*                           IS_LOCAL_FALSE_E - the member is NOT local to this device
*
* RETURNS:
*       GT_OK   - on success,
*       GT_NOT_FOUND - port was not found
*       GT_FAIL - on error.
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMemberDel(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr,
    OUT IS_LOCAL_ENT            *isLocalMemberPtr
)
{
    GT_U32  ii;
    GT_U32  count;/* number of enabled and disabled members in trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    GT_BOOL hwDevNumChanged;/* indication that the 'found'
                            could was due to 'local member' indication
                            GT_TRUE - the member found only because we used 'local member'
                                      indication
                            GT_FALSE - the member found due to exact match*/

    *isLocalMemberPtr = IS_LOCAL_FALSE_E;

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    if(GT_OK != trunkDbMemberIndexGet(devNum,trunkInfoPtr,memberPtr,&ii,&hwDevNumChanged))
    {
        /* port was not found --

           not return error because:
           1. support "last transaction" for high availability
           2. the caller asked to remove port from trunk , and after the action
              the port is not in the trunk
        */
        return GT_OK;
    }

    if(hwDevNumChanged == GT_TRUE)
    {
        /* the application try to delete a member that was not added to the trunk ,
           but the member was added to the trunk when an old hwDevNum of current
           local device */

        /* so trying to remove the memberPtr from the trunk MUST not be success
           application must explicitly delete the member with the 'old hwDevNum'
        */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    LOCAL_MEMBER_DB_UNSET_MAC(devNum,trunkId,trunkInfoPtr,ii,memberPtr->port);

    *isLocalMemberPtr = (trunkInfoPtr->isLocalMembersPtr[ii] == GT_TRUE) ?
                        IS_LOCAL_TRUE_E : IS_LOCAL_FALSE_E;

    count = trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount;

    /* fix bug #27289 :
       it seems that "old Core" :
       1. replaced the last enabled port in the trunk with the enabled port that
          is being removed
       2. replace the last disabled port in the trunk with the first disabled
          port
    */
    if(ii < trunkInfoPtr->enabledCount)
    {
        /* remove an enabled port */

        /*
           1. override with the last enabled port in the trunk on the enabled port
              that is being removed
        */
        trunkDbMemberCopy(trunkInfoPtr,ii,trunkInfoPtr->enabledCount-1);

        /*
           2. replace the last disabled port in the trunk with the first
              disabled port
        */
        trunkDbMemberCopy(trunkInfoPtr,trunkInfoPtr->enabledCount-1,count-1);
    }
    else
    {
        /* remove a disabled port */

        /*
           1. override with the last disabled port in the trunk on the disabled
              port that is being removed
        */
        trunkDbMemberCopy(trunkInfoPtr,ii,count-1);/*fixed bug #66689*/
    }

    trunkDbMemberSet(devNum,trunkInfoPtr,count-1,NULL);/* just for the "clean" */

    if(ii < trunkInfoPtr->enabledCount)
    {
        /* we delete port from the enabled ports */
        trunkInfoPtr->enabledCount--;
    }
    else
    {
        /* we delete port from the disabled ports */
        trunkInfoPtr->disabledCount--;
    }

    if((trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount) == 0)
    {
        trunkInfoPtr->type = CPSS_TRUNK_TYPE_FREE_E;
    }

    return GT_OK;
}

/*******************************************************************************
* trunkDbMemberEnable
*
* DESCRIPTION:
*       enable port in the DB for the trunk
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to enable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to enable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on error.
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMemberEnable(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_U32  ii;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    GT_BOOL hwDevNumChanged;/* indication that the 'found'
                            could was due to 'local member' indication
                            GT_TRUE - the member found only because we used 'local member'
                                      indication
                            GT_FALSE - the member found due to exact match*/

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    if(GT_OK != trunkDbMemberIndexGet(devNum,trunkInfoPtr,memberPtr,&ii,&hwDevNumChanged))
    {
        /* member was not found */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    if(hwDevNumChanged == GT_TRUE)
    {
        /* the application try to enable a member that was not added to the trunk ,
           but the member was added to the trunk when an old hwDevNum of current
           local device */

        /* so trying to enable the memberPtr from the trunk MUST not be success
           application must first delete the member with the 'old hwDevNum'
        */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }


    if(ii < trunkInfoPtr->enabledCount)
    {
        /* member already enabled */
        /* already in the wanted state */
        return GT_OK;
    }

    /*
        swap the member to enabled with the first disabled member
    */
    trunkDbMemberCopy(trunkInfoPtr,ii,trunkInfoPtr->enabledCount);

    trunkDbMemberSet(devNum,trunkInfoPtr,trunkInfoPtr->enabledCount,memberPtr);

    /*update DB counters*/
    trunkInfoPtr->enabledCount++;
    trunkInfoPtr->disabledCount--;

    return GT_OK;
}

/*******************************************************************************
* trunkDbMemberDisable
*
* DESCRIPTION:
*       disable port in the DB for the trunk
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       memberPtr   - trunk member info
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_NOT_FOUND - port was not found
*       GT_FAIL - on error.
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMemberDisable(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_U32  ii;
    GT_U32  count;/* number of enabled members in trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    GT_BOOL hwDevNumChanged;/* indication that the 'found'
                            could was due to 'local member' indication
                            GT_TRUE - the member found only because we used 'local member'
                                      indication
                            GT_FALSE - the member found due to exact match*/

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    count = trunkInfoPtr->enabledCount;

    if(GT_OK != trunkDbMemberIndexGet(devNum,trunkInfoPtr,memberPtr,&ii,&hwDevNumChanged))
    {
        /* port was not found */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    if(hwDevNumChanged == GT_TRUE)
    {
        /* the application try to disable a member that was not added to the trunk ,
           but the member was added to the trunk when an old hwDevNum of current
           local device */

        /* so trying to disable the memberPtr from the trunk MUST not be success
           application must first delete the member with the 'old hwDevNum'
        */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }

    if(ii >= count)
    {
        /* port already disabled */
        /* already in the wanted state */
        return GT_OK;
    }

    /*
        swap the enabled port (to be disable) with the last enabled port --
        this action actually make this port to be first disabled port
    */
    trunkDbMemberCopy(trunkInfoPtr,ii,trunkInfoPtr->enabledCount - 1);

    trunkDbMemberSet(devNum,trunkInfoPtr,trunkInfoPtr->enabledCount - 1,memberPtr);

    /*update DB counters*/
    trunkInfoPtr->enabledCount--;
    trunkInfoPtr->disabledCount++;

    return GT_OK;
}

/*******************************************************************************
* trunkDbMemberAsDisabledAdd
*
* DESCRIPTION:
*       add member to the DB -- as disabled -- for the trunk
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       memberPtr   - trunk member info
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_ALREADY_EXIST - The members already exists in another trunk
*       GT_FULL - trunk is FULL and port not already in trunk
*       GT_FAIL - on error.
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbMemberAsDisabledAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_STATUS   rc; /* return value on error */
    GT_U32  count;/* total number of members in this trunk */
    GT_TRUNK_ID  existTrunkId;/* trunkId where the port may already exist in */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* check parameters validity */
    rc = trunkDbMembersValidityCheck(devNum,trunkId,1,memberPtr,0,NULL);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check if member already exists in this trunk */
    rc = prvCpssGenericTrunkDbIsMemberOfTrunk(devNum,memberPtr,&existTrunkId);
    if(rc == GT_OK)
    {
        /* member already exists in this trunk -- make sure it is disabled */
        return prvCpssGenericTrunkMemberDisable(devNum,trunkId,memberPtr);
    }

    count = trunkInfoPtr->enabledCount + trunkInfoPtr->disabledCount;

    if(count == PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FULL, LOG_ERROR_NO_MSG);
    }

    trunkDbMemberSet(devNum,trunkInfoPtr,count,memberPtr);
    LOCAL_MEMBER_DB_SET_MAC(devNum,trunkId,trunkInfoPtr,count,memberPtr->port);

    /*update DB counters*/
    trunkInfoPtr->disabledCount++ ;

    trunkInfoPtr->type = CPSS_TRUNK_TYPE_REGULAR_E;

    return GT_OK;
}

/*******************************************************************************
* trunkMemberAction
*
* DESCRIPTION:
*       This function add/remove/enable/disable member to.from/in the trunk
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to act on
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to act on .
*       action      - the type of action to perform .
*                     action should be one of :
*                     add/remove/enable/disable/add-as-disabled
*       isLocalMember - indication that the member is local to this device
*                           IS_LOCAL_TRUE_E - the member is local to this device
*                               relevant only to next actions : PRV_CPSS_TRUNK_ACTION_REMOVE_E
*                           IS_LOCAL_FALSE_E - the member is NOT local to this device
*                               relevant only to next actions : PRV_CPSS_TRUNK_ACTION_REMOVE_E
*                           IS_LOCAL_CHECK_E -
*                               relevant to all actions , but NOT to: PRV_CPSS_TRUNK_ACTION_REMOVE_E
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
*
*
*******************************************************************************/
static GT_STATUS trunkMemberAction
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr,
    IN PRV_CPSS_TRUNK_ACTION_ENT action,
    IN IS_LOCAL_ENT              isLocalMember

)
{
    GT_STATUS rc;    /* return error core */
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
                                    /* array of enabled trunk members */
    GT_U32 numEnabledMembers; /*num of enabled members in the trunk in Hw*/
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */
    GT_BOOL addPortSequence = GT_FALSE;/* 'add' port sequence flag */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    CPSS_PORTS_BMP_STC localPortsRemoved;/* bmp of local port removed */
    GT_BOOL portAddedToDesignated = GT_FALSE; /* is port added from designated table */

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    numEnabledMembers = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk;
    rc = prvCpssGenericTrunkDbEnabledMembersGet(devNum,trunkId,
                                        &numEnabledMembers,enabledMembersArray);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = trunkDbNonTrunkPortsCalc(devNum,trunkId,&nonTrunkPorts);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(isLocalMember == IS_LOCAL_CHECK_E)
    {
        if(GT_TRUE == trunkDbMemberIsLocal(trunkInfoPtr,memberPtr))
        {
            isLocalMember = IS_LOCAL_TRUE_E;
        }
        else
        {
            isLocalMember = IS_LOCAL_FALSE_E;
        }
    }

    if(isLocalMember == IS_LOCAL_TRUE_E)
    {
        if(action == PRV_CPSS_TRUNK_ACTION_ADD_E ||
           action == PRV_CPSS_TRUNK_ACTION_ADD_AS_DISABLED_E)
        {
            addPortSequence = GT_TRUE;
        }
        else if(action == PRV_CPSS_TRUNK_ACTION_REMOVE_E)
        {
            /* update port control on trunk */
            rc = trunkPortTrunkIdSet(devNum,memberPtr->port,
                                     GT_FALSE,/* port not in trunk */
                                     0);/* not relevant value */
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        if(addPortSequence == GT_FALSE)
        {
            /* set the non-trunk ports bitmap */
            rc = trunkNonTrunkPortUpdateHw(devNum,trunkId,memberPtr,
                                           &nonTrunkPorts,action,
                                           GT_TRUE);/* local port */
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* update HW about the trunk enabled members and the number of enabled
       members */
    rc = trunkTableUpdateHw(devNum,trunkId,numEnabledMembers,enabledMembersArray);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* no need to do next for PRV_CPSS_TRUNK_ACTION_ADD_AS_DISABLED_E
       since this member was not designated in the first place */
    if(action == PRV_CPSS_TRUNK_ACTION_REMOVE_E ||
       action == PRV_CPSS_TRUNK_ACTION_DISABLE_E)
    {
        if(isLocalMember == IS_LOCAL_TRUE_E)
        {
            portAddedToDesignated = GT_TRUE;

            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&localPortsRemoved);
            /* notify the designated ports table that port was removed */
            CPSS_PORTS_BMP_PORT_SET_MAC(&localPortsRemoved,memberPtr->port);
        }
    }

    /* update designated ports table */
    rc = trunkDesignatedPortsTableUpdate(devNum,trunkId,numEnabledMembers,
                                enabledMembersArray,&nonTrunkPorts,
                                (portAddedToDesignated == GT_TRUE ? &localPortsRemoved  : NULL),
                                0/*relevant to weighted trunk only*/,
                                NULL/*relevant to weighted trunk only*/);
    if (rc != GT_OK)
    {
        return rc;
    }


    if(addPortSequence == GT_TRUE)
    {
        /* when add port to trunk , need to update the 'non-trunk' table
           after updating the designated table , because:

           1. for 'Cascade trunks' - this entry in the table defines the trunk's
           *                members , so when masked with the 'Designated table'
           *                will lead to single member.
           *                BUT if we update add member to 'non-trunk' table
           *                before the designated table was updated for this port
           *                it means that the designated table may point to 2 ports
           *                as 'destination' , and the BM (in PP) may not release
           *                the buffers correctly.
           *  for 'Regular trunks' - this entry prevents traffic from a trunk to
           *                be flooded (multi destination) back to the source trunk.
           *                so potentially ports that already in the trunk can
           *                start flooding traffic to this 'new port' , due to
           *                the settings in 'Designated table' , BUT this behavior
           *                seems acceptable since this is 'Transition time' ,
           *                and the flooding that the port was getting anyway
           *                (if vlan member) , is kept until the 'non-trunk table
           *                updated.
           *
            */

        /* set the non-trunk ports bitmap */
        rc = trunkNonTrunkPortUpdateHw(devNum,trunkId,memberPtr,
                                       &nonTrunkPorts,action,
                                       GT_TRUE);/* local port */
        if (rc != GT_OK)
        {
            return rc;
        }

        /* when add port to trunk , need to update the 'port control' only
           after updating the 'non-trunk' table , because:
           when the added port is doing 'flooding' in the vlan (traffic ingress
           from it and doing flooding) , MUST not be flooded back to this port
           during the operation.
           but if we will update the 'port control' before the 'non-trunk' table,
           from the 'non-trunk' table point of view , the port is not in the
           trunk , so flooding back to it from the trunk is allowed !!! --> ERROR
        */

        /* update port control on trunk */
        rc = trunkPortTrunkIdSet(devNum,memberPtr->port,
                                GT_TRUE,/* set the trunkId */
                                trunkId);
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericTrunkMemberDisable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function disable (enabled)existing member of trunk in the device.
*       If member is already disabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the disabled member
*       else
*           1. If disabled member is not the designated member - set its relevant bits to 0
*           2. If disabled member is the designated member set its relevant bits
*               on all indexes to 0.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to disable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to disable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkMemberDisable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_STATUS rc;

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(memberPtr);

    /* disable the member in the DB */
    rc = trunkDbMemberDisable(devNum,trunkId,memberPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check if need to do sorting on the trunk members. */
    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->sortTrunkMembers == GT_TRUE)
    {
        rc = trunkDbMembersForTrunkIdSort(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* disable  the member in the HW */
    rc = trunkMemberAction(devNum,trunkId,memberPtr,
                            PRV_CPSS_TRUNK_ACTION_DISABLE_E,
                            IS_LOCAL_CHECK_E);

    return rc;
}


/*******************************************************************************
* prvCpssGenericTrunkDbEnabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the enabled members of the trunk
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfEnabledMembersPtr - (pointer to) max num of enabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfEnabledMembersPtr - (pointer to) the actual num of enabled members
*                      in the trunk (up to max number supported by the PP)
*       enabledMembersArray - (array of) enabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbEnabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfEnabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
)
{
    GT_U32  ii;
    GT_U32  origNumOfPortsInTrunk;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    CPSS_NULL_PTR_CHECK_MAC(numOfEnabledMembersPtr);
    if(*numOfEnabledMembersPtr != 0)
    {
        CPSS_NULL_PTR_CHECK_MAC(enabledMembersArray);
    }

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    origNumOfPortsInTrunk = *numOfEnabledMembersPtr;

    *numOfEnabledMembersPtr = trunkInfoPtr->enabledCount ;

    if(origNumOfPortsInTrunk > trunkInfoPtr->enabledCount)
    {
        origNumOfPortsInTrunk = trunkInfoPtr->enabledCount;
    }

    for(ii = 0; ii < origNumOfPortsInTrunk ;ii++)
    {
        enabledMembersArray[ii] = trunkInfoPtr->membersPtr[ii];
    }

    return GT_OK;
}


/*******************************************************************************
* prvCpssGenericTrunkDbDisabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the disabled members of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfDisabledMembersPtr - (pointer to) max num of disabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfDisabledMembersPtr -(pointer to) the actual num of disabled members
*                      in the trunk (up to max number supported by the PP)
*       disabledMembersArray - (array of) disabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbDisabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfDisabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   disabledMembersArray[]
)
{
    GT_U32  ii;
    GT_U32  baseIdx;/* base index of disabled ports */
    GT_U32  origNumOfPortsInTrunk;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    CPSS_NULL_PTR_CHECK_MAC(numOfDisabledMembersPtr);
    if(*numOfDisabledMembersPtr != 0)
    {
        CPSS_NULL_PTR_CHECK_MAC(disabledMembersArray);
    }

    origNumOfPortsInTrunk = *numOfDisabledMembersPtr;

    *numOfDisabledMembersPtr = trunkInfoPtr->disabledCount ;

    if(origNumOfPortsInTrunk > trunkInfoPtr->disabledCount)
    {
        origNumOfPortsInTrunk = trunkInfoPtr->disabledCount;
    }

    baseIdx = trunkInfoPtr->enabledCount;

    for(ii = 0; ii < origNumOfPortsInTrunk ;ii++)
    {
        disabledMembersArray[ii] = trunkInfoPtr->membersPtr[baseIdx + ii];
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is trunk member the function retrieve the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number.
*       memberPtr - (pointer to) the member to check if is trunk member
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) trunk id of the port .
*                    this pointer allocated by the caller.
*                    this can be NULL pointer if the caller not require the
*                    trunkId(only wanted to know that the member belongs to a
*                    trunk)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_FOUND             - the pair {devNum,port} not a trunk member
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
)
{
    GT_U32              ii;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/
    GT_TRUNK_ID         currentTrunkId;

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(memberPtr);

    if(memberPtr->hwDevice >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    currentTrunkId = 1;
    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,currentTrunkId);

    while(trunkInfoPtr)
    {
        if(GT_OK == trunkDbMemberIndexGet(devNum,trunkInfoPtr,memberPtr,&ii,NULL))
        {
            if(trunkIdPtr)
            {
                *trunkIdPtr = currentTrunkId;
            }
            return GT_OK;
        }

        /* update the current trunk Id */
        currentTrunkId++;
        /* get the next DB entry */
        trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,currentTrunkId);
    }

    if((memberPtr->hwDevice == PRV_CPSS_HW_DEV_NUM_MAC(devNum)) &&
       (memberPtr->port < MAX_MEMBER_PORT_NUM_MAC(devNum)))
    {
        /* check the member with local ports (if set from 'Cascade trunk') */
        /* set the ports as members of this trunk */
        currentTrunkId = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portTrunkIdArray[memberPtr->port];
        if(currentTrunkId != 0)
        {
            if(trunkIdPtr)
            {
                *trunkIdPtr = currentTrunkId;
            }
            return GT_OK;
        }
    }

    return /* do not register as error to the ERROR LOG */ GT_NOT_FOUND;

}

/*******************************************************************************
* prvCpssGenericTrunkDbTrunkTypeGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Get the trunk type.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number.
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       typePtr - (pointer to) the trunk type
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbTrunkTypeGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT CPSS_TRUNK_TYPE_ENT     *typePtr
)
{
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(typePtr);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    *typePtr = trunkInfoPtr->type;

    return GT_OK;
}

/*******************************************************************************
* trunkMemebersAllRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function remove all the trunk members enable,disabled
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_OUT_OF_RANGE - when the sum of number of enabled members + number of
*                         disabled members exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkMemebersAllRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId
)
{
    GT_STATUS   rc;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC   *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    CPSS_TRUNK_MEMBER_STC   currMember;

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* remove all disabled ports */
    while(trunkInfoPtr->disabledCount)/* do not decrement this counter !!
                                         prvCpssGenericTrunkMemberRemove will
                                         do it !*/
    {
        /* always remove the LAST disabled member */
        /* Note: the function prvCpssGenericTrunkMemberRemove will update the
           DB
        */

        /* copy the member to local variable so the
           DB will not be affected directly */
        currMember = trunkInfoPtr->membersPtr[trunkInfoPtr->enabledCount +
                                          trunkInfoPtr->disabledCount - 1];

        rc = prvCpssGenericTrunkMemberRemove(devNum,trunkId,&currMember);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* remove all enabled ports */
    while(trunkInfoPtr->enabledCount)/* do not decrement this counter !!
                                         prvCpssGenericTrunkMemberRemove will
                                         do it !*/
    {
        /* always remove the LAST enabled member */
        /* Note: the function prvCpssGenericTrunkMemberRemove will update the
           DB
        */

        /* copy the member to local variable so the
           DB will not be affected directly */
        currMember = trunkInfoPtr->membersPtr[trunkInfoPtr->enabledCount +
                                          trunkInfoPtr->disabledCount - 1];

        rc = prvCpssGenericTrunkMemberRemove(devNum,trunkId,&currMember);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* trunkMembersSetAction
*
* DESCRIPTION:
*       This function set the trunk with the specified enable and disabled
*       members.
*
*       The trunk must be empty prior to this call !!!
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       enabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfEnabledMembers = 0)
*       numOfEnabledMembers - number of enabled members in the array.
*       disabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfDisabledMembers = 0)
*       numOfDisabledMembers - number of enabled members in the array.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkMembersSetAction
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
)
{
    GT_STATUS   rc;
    GT_U32  ii;

    /******************************/
    /* add the members one by one */
    /******************************/
    for(ii = 0 ; ii < numOfEnabledMembers; ii++)
    {
        /* add the member to the DB */
        rc = trunkDbMemberAdd(devNum,trunkId,&enabledMembersArray[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if need to do sorting on the trunk members. */
        if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->sortTrunkMembers == GT_TRUE)
        {
            rc = trunkDbMembersForTrunkIdSort(devNum,trunkId);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* add the member to the HW */
        rc = trunkMemberAction(devNum,trunkId,&enabledMembersArray[ii],
                                PRV_CPSS_TRUNK_ACTION_ADD_E,
                                IS_LOCAL_CHECK_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    for(ii = 0 ; ii < numOfDisabledMembers ; ii++)
    {
        /* add the member to the DB */
        rc = trunkDbMemberAsDisabledAdd(devNum,trunkId,&disabledMembersArray[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* check if need to do sorting on the trunk members. */
        if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->sortTrunkMembers == GT_TRUE)
        {
            rc = trunkDbMembersForTrunkIdSort(devNum,trunkId);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* add the member to the HW */
        rc = trunkMemberAction(devNum,trunkId,&disabledMembersArray[ii],
                                PRV_CPSS_TRUNK_ACTION_ADD_AS_DISABLED_E,
                                IS_LOCAL_CHECK_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;

}

/*******************************************************************************
* trunkMembersKeepCurrentConfiguration
*
* DESCRIPTION:
*       This function write the current trunk configuration to the HW
*       The function assume that the DB is already current configuration
*
*       This function needed for the issues of "High availability" and
*       "last transaction"
*       So if Application added port as last transaction to the CPSS but the HW
*       was disconnected at the time from the CPU, then application can set the
*       same entry again .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS trunkMembersKeepCurrentConfiguration
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId
)
{
    GT_STATUS rc;    /* return error core */
    CPSS_TRUNK_MEMBER_STC   membersArray[PRV_CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
                                    /* array of enabled,disabled trunk members */
    GT_U32 numEnabledMembers; /*num of enabled members in the trunk */
    GT_U32 numDisabledMembers; /*num of disabled members in the trunk*/
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */
    GT_U32  ii;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    numEnabledMembers = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk;
    rc = prvCpssGenericTrunkDbEnabledMembersGet(devNum,trunkId,
                                        &numEnabledMembers,&membersArray[0]);
    if(rc != GT_OK)
    {
        return rc;
    }

    numDisabledMembers = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numMembersInTrunk - numEnabledMembers;
    if(numDisabledMembers)
    {
        /* get the disabled ports -- put them at the end of the enabled array */
        rc = prvCpssGenericTrunkDbDisabledMembersGet(devNum,trunkId,
                                    &numDisabledMembers,
                                    &membersArray[numEnabledMembers]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    rc = trunkDbNonTrunkPortsCalc(devNum,trunkId,&nonTrunkPorts);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set both the enabled and disabled member of MY device to be
       associated with trunk in the port control
    */
    for(ii = 0; ii < (numEnabledMembers + numDisabledMembers); ii++)
    {
        if(trunkInfoPtr->isLocalMembersPtr[ii] == GT_FALSE)
        {
            continue;
        }

        /* update port control on trunk */
        rc = trunkPortTrunkIdSet(devNum,membersArray[ii].port,
                                GT_TRUE,/* set the trunkId */
                                trunkId);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* set the non-trunk ports bitmap */
    rc = trunkNonTrunkPortUpdateHw(devNum,trunkId,NULL,&nonTrunkPorts,
                PRV_CPSS_TRUNK_ACTION_SET_KEEP_THE_CURRENT_TRUNK_E,
                GT_FALSE);/* don't care on this action */

    /* update HW about the trunk enabled members and the number of enabled
       members */
    rc = trunkTableUpdateHw(devNum,trunkId,numEnabledMembers,&membersArray[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* update designated ports table */
    rc = trunkDesignatedPortsTableUpdate(devNum,trunkId,numEnabledMembers,
                                &membersArray[0],&nonTrunkPorts,NULL,
                                0/*relevant to weighted trunk only*/,
                                NULL/*relevant to weighted trunk only*/);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkDesignatedMemberSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function Configures per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*       Setting value replace previously assigned designated member.
*
*       NOTE that:
*       under normal operation this should not be used with cascade Trunks,
*       due to the fact that in this case it affects all types of traffic -
*       not just Multi-destination as in regular Trunks.
*
*  Diagram 1 : Regular operation - Traffic distribution on all enabled
*              members (when no specific designated member defined)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   1   %   0    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   1    %    0   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    0   %  1   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*  Diagram 2: Traffic distribution once specific designated member defined
*             (M3 in this case - which is currently enabled member in trunk)
*
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*  index \ member  %   M1  %   M2   %    M3  %  M4  %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 0       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 1       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 2       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 3       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 4       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 5       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 6       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*    Index 7       %   0   %   0    %    1   %  0   %
*  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*       enable      - enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to)the designated member we set to the trunk.
*                     relevant only when enable = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDesignatedMemberSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_BOOL                  enable,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_STATUS rc;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    if(enable == GT_TRUE)
    {
        CPSS_NULL_PTR_CHECK_MAC(memberPtr);

        /* check parameters validity */
        rc = trunkDbMembersValidityCheck(devNum,trunkId,1,memberPtr,0,NULL);
        if(rc != GT_OK)
        {
            return rc;
        }

        trunkInfoPtr->designatedMemberExists = GT_TRUE;
        trunkInfoPtr->designatedMember = (*memberPtr);

        if(memberPtr->hwDevice == PRV_CPSS_HW_DEV_NUM_MAC(devNum))
        {
            trunkInfoPtr->designatedMemberIsLocal = GT_TRUE;
        }
        else
        {
            trunkInfoPtr->designatedMemberIsLocal = GT_FALSE;
        }
    }
    else
    {
        trunkInfoPtr->designatedMemberExists = GT_FALSE;
    }

    /* calculate the bitmap of non-trunk members */
    rc = trunkDbNonTrunkPortsCalc(devNum,trunkId,&nonTrunkPorts);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* update designated ports table */
    rc = trunkDesignatedPortsTableUpdate(devNum,trunkId,
        trunkInfoPtr->enabledCount,&trunkInfoPtr->membersPtr[0],
        &nonTrunkPorts,NULL,
        0/*relevant to weighted trunk only*/,
        NULL/*relevant to weighted trunk only*/);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkDbDesignatedMemberGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function get Configuration per-trunk the designated member -
*       value should be stored (to DB) even designated member is not currently
*       a member of Trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       enablePtr   - (pointer to) enable/disable designated trunk member.
*                     GT_TRUE -
*                       1. Clears all current Trunk member's designated bits
*                       2. If input designated member, is currently an
*                          enabled-member at Trunk (in local device) enable its
*                          bits on all indexes
*                       3. Store designated member at the DB (new DB parameter
*                          should be created for that)
*
*                     GT_FALSE -
*                       1. Redistribute current Trunk members bits (on all enabled members)
*                       2. Clear designated member at  the DB
*
*       memberPtr   - (pointer to) the designated member of the trunk.
*                     relevant only when enable = GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbDesignatedMemberGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT GT_BOOL                 *enablePtr,
    OUT CPSS_TRUNK_MEMBER_STC   *memberPtr
)
{
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);
    CPSS_NULL_PTR_CHECK_MAC(memberPtr);

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    /* return value from the DB */
    *enablePtr = trunkInfoPtr->designatedMemberExists;
    *memberPtr = trunkInfoPtr->designatedMember;

    return GT_OK;
}


/*******************************************************************************
* prvCpssGenericTrunkMemebersSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function set the trunk with the specified enable and disabled
*       members.
*       this setting override the previous setting of the trunk members.
*
*       the user can "invalidate/unset" trunk entry by setting :
*           numOfEnabledMembers = 0 and  numOfDisabledMembers = 0
*
*       This function support next "set entry" options :
*       1. "reset" the entry
*          function will remove the previous settings
*       2. set entry after the entry was empty
*          function will set new settings
*       3. set entry with the same members that it is already hold
*          function will rewrite the HW entries as it was
*       4. set entry with different setting then previous setting
*          a. function will remove the previous settings
*          b. function will set new settings
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           distribute MC/Cascade trunk traffic among the enabled members
*       else
*           1. Set all member ports bits with 0
*           2. If designated member is one of the enabled members, set its relevant
*           bits on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       enabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfEnabledMembers = 0)
*       numOfEnabledMembers - number of enabled members in the array.
*       disabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfDisabledMembers = 0)
*       numOfDisabledMembers - number of enabled members in the array.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_OUT_OF_RANGE - when the sum of number of enabled members + number of
*                         disabled members exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_ALREADY_EXIST - one of the members already exists in another trunk
*
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkMembersSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
)
{
    GT_STATUS rc;
    GT_BOOL   sameDbEntry;/* Does the DB hold the same entry as the new settings*/
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    if(numOfEnabledMembers)
    {
        CPSS_NULL_PTR_CHECK_MAC(enabledMembersArray);
    }

    if(numOfDisabledMembers)
    {
        CPSS_NULL_PTR_CHECK_MAC(disabledMembersArray);
    }

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    if(trunkInfoPtr)
    {
        /* this API supported only for regular trunks or free trunk */
        TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);
    }

    if(numOfEnabledMembers == 0 && numOfDisabledMembers == 0)
    {
        /* update the members in case that changed hwDevNum */
        rc = trunkDbMembersRefresh(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* remove the current configuration */
        rc = trunkMemebersAllRemove(devNum,trunkId);

        return rc;
    }

    /* check if the new settings already exists in the DB for this trunk */
    rc = trunkDbMembersAlreadySetCheck(devNum,trunkId,
        numOfEnabledMembers,enabledMembersArray,
        numOfDisabledMembers,disabledMembersArray,
        &sameDbEntry);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(sameDbEntry == GT_FALSE)
    {
        /* when we set the same setting that already in DB we not need to do the
           next check.
        */
        rc = trunkDbMembersValidityCheck(devNum,trunkId,
                numOfEnabledMembers,enabledMembersArray,
                numOfDisabledMembers,disabledMembersArray);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* now we know that we did all the checking and the new configuration is
           valid */

        /* update the members in case that changed hwDevNum */
        rc = trunkDbMembersRefresh(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* remove the current configuration */
        rc = trunkMemebersAllRemove(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* set full members to the HW */
        rc = trunkMembersSetAction(devNum,trunkId,
                numOfEnabledMembers,enabledMembersArray,
                numOfDisabledMembers,disabledMembersArray);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* update the members in case that changed hwDevNum */
        rc = trunkDbMembersRefresh(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* keep the current configuration , and write it back to HW */
        rc = trunkMembersKeepCurrentConfiguration(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* trunkDbCascadeCheckAndSet
*
* DESCRIPTION:
*       check it :
*       1. the trunk can be used as 'Cascade trunk'
*       2. the action of setting trunk ports is the same settings as the DB hold .
*
*       set the bmp to the DB , state that the trunk is:
*           -- portsMembersPtr == NULL --> free trunk
*           -- otherwise --> cascade only trunk
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       portsMembersPtr - (pointer to) local ports bitmap to be members of the
*                   cascade trunk.
*                          when NULL --> meaning 'invalidate' the cascade trunk.
*                          (set trunk type as CPSS_TRUNK_TYPE_FREE_E)
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_OUT_OF_RANGE    - there are ports in the bitmap that not supported by
*                            the device.
*       GT_BAD_PARAM       - bad trunkId number , or number
*                            of ports (in the bitmap) larger then the number of
*                            entries in the 'Designated trunk table'
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbCascadeCheckAndSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
    GT_U32      ii;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC   *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    GT_U32      numOfPorts = 0;/* number of ports in the bitmap */
    CPSS_PORTS_BMP_STC  tmpPortsBmp;/* temp ports bmp*/
    CPSS_PORTS_BMP_STC fullPortsBmp; /* full ports bmp of this device */
    CPSS_PORTS_BMP_STC  tmp2PortsBmp;/* temp2 ports bmp*/

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for cascade trunks or free trunk */
    TRUNK_TYPE_SUPPORT_CASCADE_CHECK_MAC(trunkInfoPtr);

    if(trunkInfoPtr->designatedMemberExists == GT_TRUE)
    {
        /* the cascade trunk must not hold specific designated member , because
           there will be not load balance between the trunk members. */
        /* meaning that the caller must first disable the designated member from
           this trunk before setting it as cascade trunk*/
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    if(portsMembersPtr)
    {
        /* filtering not relevant 'Ranges of ports' -
          build bmp of port that should care for this device ... to support
          change in the size of CPSS_PORTS_BMP_STC that 'old applications' may
          have initialized only ports[0] and ports[1] */
        PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&fullPortsBmp);

        if(0 == USE_VIRTUAL_PORT_MAC(devNum))
        {
            if(IS_PHY_PORT_BY_MASK_MAC(devNum))
            {
                /* build tmpPortsBmp = empty; remove existing ports */
                CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&tmpPortsBmp);
            }
            else
            {
                /* build tmpPortsBmp = portsMembersPtr &~ existingPorts
                   remove existing ports */
                CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,portsMembersPtr,&PRV_CPSS_PP_MAC(devNum)->existingPorts);
            }
            /* mask tmpPortsBmp to use only relevant 'Ranges of ports' */
            CPSS_PORTS_BMP_BITWISE_AND_MAC(&tmpPortsBmp,&tmpPortsBmp,&fullPortsBmp);
        }
        else
        {
            /* build tmpPortsBmp = portsMembersPtr &~ fullPortsBmp
               remove virtual ports */
            CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,portsMembersPtr,&fullPortsBmp);

            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&tmp2PortsBmp);
            for(ii = 0 ; ii <= (GT_U32)(PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts >> 5) ; ii++)
            {
                /* this index in the BMP is relevant */
                tmp2PortsBmp.ports[ii] = 0xFFFFFFFF;
            }

            /* mask tmpPortsBmp to use only relevant 'Ranges of ports' */
            CPSS_PORTS_BMP_BITWISE_AND_MAC(&tmpPortsBmp,&tmpPortsBmp,&tmp2PortsBmp);
        }
        /* check that the application not gave ports that are not relevant to
           the device (after filtering not relevant 'Ranges of ports')*/
        if(0 == CPSS_PORTS_BMP_IS_ZERO_MAC(&tmpPortsBmp))
        {
            /* non existing ports to the cascaded trunk */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* check if the ports are members of other trunk */
        for(ii = 0; ii < MAX_MEMBER_PORT_NUM_MAC(devNum); ii++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr,ii))
            {
                continue;
            }

            if(USE_VIRTUAL_PORT_MAC(devNum))
            {
                /* ExMxTg / ExMxPm - need to check 'virtual port' */
                PRV_CPSS_VIRTUAL_PORT_CHECK_MAC(devNum,ii);
            }
            else
            if(IS_PHY_PORT_BY_MASK_MAC(devNum))
            {
                CHECK_PHY_PORT_BY_MASK_MAC(devNum,ii);
            }
            else
            {
                /* device not support 'virtual port' only physical ports. */
                PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,ii);
            }

            if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portTrunkIdArray[ii] != 0 &&
               PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portTrunkIdArray[ii] != trunkId)
            {
                /* already exists in another trunk */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_ALREADY_EXIST, LOG_ERROR_NO_MSG);
            }

            numOfPorts++;
        }

        if(numOfPorts > PRV_CPSS_TRUNK_DESIGNATED_TABLE_SIZE_MAC(devNum))
        {
            /* number of ports (in the bitmap) larger then the number of entries in
               the 'Designated trunk table' */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* trunk is 'Cascacde only' */
        trunkInfoPtr->type = CPSS_TRUNK_TYPE_CASCADE_E;
    }
    else
    {
        /* release the trunk */
        trunkInfoPtr->type = CPSS_TRUNK_TYPE_FREE_E;
    }

    /* remove previous settings */
    for(ii = 0; ii < MAX_MEMBER_PORT_NUM_MAC(devNum); ii++)
    {
        if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portTrunkIdArray[ii] == trunkId)
        {
            PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portTrunkIdArray[ii] = 0;
        }
    }

    if(portsMembersPtr)
    {
        /* set the ports as members of this trunk */
        for(ii = 0; ii < MAX_MEMBER_PORT_NUM_MAC(devNum); ii++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr,ii))
            {
                continue;
            }

            PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portTrunkIdArray[ii] = trunkId;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* trunkCascadeSet
*
* DESCRIPTION:
*       set to HW the 'Cascade trunk' configurations :
*       1. non-trunk table (single entry)
*       2. designated trunk table (full table)
*       3. 'per port' trunkId
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       portsMembersPtr - (pointer to) local ports bitmap to be members of the
*                   cascade trunk.
*       numOfWeightedMembers - number of members in the array weightedMembersArray.
*                   value 0 - meaning that this trunk not used with weights
*       weightedMembersArray - (array of) members of the weighted cascade trunk.
*                              each member hold relative weight (relative to Weight of all members)
*
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK              - on success.
*       GT_HW_ERROR        - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkCascadeSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr,
    IN GT_U32                   numOfWeightedMembers,
    IN CPSS_TRUNK_WEIGHTED_MEMBER_STC       weightedMembersArray[]

)
{
    GT_STATUS rc;    /* return error core */
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);/*The trunk DB for current trunkId*/
    CPSS_PORTS_BMP_STC fullPortsBmp; /* full ports bmp of this device */
    CPSS_PORTS_BMP_STC currentNonTrunkPortsBmp; /* current non trunk ports bmp */
    CPSS_PORTS_BMP_STC addedToTrunkPortsBmp; /* ports added to the trunk */
    CPSS_PORTS_BMP_STC removedFromTrunkPortsBmp; /* ports removed from the trunk */
    CPSS_PORTS_BMP_STC tmpPortsBmp;/* temp ports bmp */
    GT_BOOL            removedPorts;/* ports removed from trunk ? */
    GT_BOOL            addedPorts;  /* ports added to trunk ? */
    GT_U32             ii;
    GT_U32             numOfVirtPorts;

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* call appropriate virtual function */
    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpGetFunc(devNum,
            trunkId, &currentNonTrunkPortsBmp);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(IS_PHY_PORT_BY_MASK_MAC(devNum))
    {
        numOfVirtPorts = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->validityMask.port + 1;
    }
    else
    {
        numOfVirtPorts = PRV_CPSS_PP_MAC(devNum)->numOfVirtPorts;
    }

    PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&fullPortsBmp);

    /* calculate the non-trunk members */
    if(portsMembersPtr)
    {
        CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&nonTrunkPorts,&fullPortsBmp,portsMembersPtr);
    }
    else
    {
        nonTrunkPorts = fullPortsBmp;
    }

    /* ports added to the trunk are ports that in HW (currentNonTrunkPortsBmp)
       are 'Not in trunk' but need to be 'In trunk' (nonTrunkPorts) */
    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&addedToTrunkPortsBmp,&currentNonTrunkPortsBmp,&nonTrunkPorts);


    /* ports removed from the trunk are ports that need to be 'Not In trunk' (nonTrunkPorts)
       But in HW (currentNonTrunkPortsBmp) are 'in trunk'*/
    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&removedFromTrunkPortsBmp,&nonTrunkPorts,&currentNonTrunkPortsBmp);

    if(0 == CPSS_PORTS_BMP_IS_ZERO_MAC(&addedToTrunkPortsBmp))
    {
        addedPorts = GT_TRUE;
    }
    else
    {
        addedPorts = GT_FALSE;
    }

    if(0 == CPSS_PORTS_BMP_IS_ZERO_MAC(&removedFromTrunkPortsBmp))
    {
        removedPorts = GT_TRUE;
    }
    else
    {
        removedPorts = GT_FALSE;
    }

    if(removedPorts == GT_TRUE)
    {
        for(ii = 0; ii < numOfVirtPorts; ii++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&removedFromTrunkPortsBmp,ii))
            {
                continue;
            }

            if(0 == USE_VIRTUAL_PORT_MAC(devNum))
            {
                if(IS_PHY_PORT_BY_MASK_MAC(devNum))
                {
                    SKIP_NON_VALID_PHY_PORT_BY_MASK_MAC(devNum,ii);
                }
                else
                if(!CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                    &PRV_CPSS_PP_MAC(devNum)->existingPorts,ii))
                {
                    continue;
                }
            }

            /* update port control on trunk */
            rc = trunkPortTrunkIdSet(devNum,(GT_U8)ii,
                                     GT_FALSE,/* port not in trunk */
                                     0);/* not relevant value */
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* need to set a temporary configuration into the non-trunk table ,
           so traffic destined to this trunk will not egress from 2 ports of the
           trunk during the action -->
           because in next step we are going to modify the 'Designated trunk table'
        */

        /* so build bmp of 'Non-trunk' ports with the 'Removed ports' from trunk */
        CPSS_PORTS_BMP_BITWISE_OR_MAC(&tmpPortsBmp,&currentNonTrunkPortsBmp , &removedFromTrunkPortsBmp);

        /* call appropriate virtual function */
        rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpSetFunc(devNum,
                trunkId, &tmpPortsBmp);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    if((removedPorts == GT_TRUE) || (addedPorts == GT_TRUE) ||
        numOfWeightedMembers)/* change of weights within the same members must also trigger modifications */
    {
        /* update designated ports table */
        rc = trunkDesignatedPortsTableUpdate(devNum,trunkId,
                                            0,   /* don't care for cascade trunk */
                                            NULL,/* don't care for cascade trunk */
                                            &nonTrunkPorts,
                                            &removedFromTrunkPortsBmp,
                                            numOfWeightedMembers,   /* supporting weighted ports */
                                            weightedMembersArray);  /* supporting weighted ports */
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    if(addedPorts == GT_TRUE)
    {
        if(portsMembersPtr == NULL)
        {
            /* should not happen because when 'portsMembersPtr == NULL' it means
               we remove  ALL ports from the cascade trunk , so there are no 'ports to add' */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        /* when add port to trunk , need to update the 'non-trunk' table
           after updating the designated table , because:

           *  for 'Cascade trunks' - this entry in the table defines the trunk's
           *                members , so when masked with the 'Designated table'
           *                will lead to single member.
           *                BUT if we update add member to 'non-trunk' table
           *                before the designated table was updated for this port
           *                it means that the designated table may point to 2 ports
           *                as 'destination' , and the BM (in PP) may not release
           *                the buffers correctly.
           *
           */

        /* call appropriate virtual function */
        rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpSetFunc(devNum,
                trunkId, &nonTrunkPorts);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* when add port to trunk , need to update the 'port control' only
           after updating the 'non-trunk' table , because:
           when the added port is doing 'flooding' in the vlan (traffic ingress
           from it and doing flooding) , MUST not be flooded back to this port
           during the operation.
           but if we will update the 'port control' before the 'non-trunk' table,
           from the 'non-trunk' table point of view , the port is not in the
           trunk , so flooding back to it from the trunk is allowed !!! --> ERROR
        */

        for(ii = 0; ii < numOfVirtPorts; ii++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr,ii))
            {
                continue;
            }

            if(0 == USE_VIRTUAL_PORT_MAC(devNum))
            {
                if(IS_PHY_PORT_BY_MASK_MAC(devNum))
                {
                    SKIP_NON_VALID_PHY_PORT_BY_MASK_MAC(devNum,ii);
                }
                else
                if(!CPSS_PORTS_BMP_IS_PORT_SET_MAC(
                    &PRV_CPSS_PP_MAC(devNum)->existingPorts,ii))
                {
                    continue;
                }
            }

            /* update port control on trunk */
            rc = trunkPortTrunkIdSet(devNum,(GT_U8)ii,
                                    GT_TRUE,/* set the trunkId */
                                    trunkId);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }


    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericTrunkCascadeTrunkPortsSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the 'cascade' trunk with the specified 'Local ports'
*       overriding any previous setting.
*       The cascade trunk may be invalidated/unset by portsMembersPtr = NULL.
*       Local ports are ports of only configured device.
*       This functions sets the trunk-type according to :
*           portsMembersPtr = NULL --> CPSS_TRUNK_TYPE_FREE_E
*           otherwise --> CPSS_TRUNK_TYPE_CASCADE_E
*       Cascade trunk is:
*           - members are ports of only configured device pointed by devNum
*           - trunk members table is empty (see cpssDxChTrunkTableEntrySet)
*             Therefore it cannot be used as target by ingress engines like FDB,
*             Router, TTI, Ingress PCL and so on.
*           - members ports trunk ID are set (see cpssDxChTrunkPortTrunkIdSet).
*             Therefore packets ingresses in member ports are associated with trunk
*           - all members are enabled only and cannot be disabled.
*           - may be used for cascade traffic and pointed by the 'Device map table'
*             as the local target to reach to the 'Remote device'.
*             (For 'Device map table' refer to cpssDxChCscdDevMapTableSet(...))
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       portsMembersPtr - (pointer to) local ports bitmap to be members of the
*                   cascade trunk.
*                   NULL - meaning that the trunk-id is 'invalidated' and
*                          trunk-type will be changed to : CPSS_TRUNK_TYPE_FREE_E
*                   not-NULL - meaning that the trunk-type will be : CPSS_TRUNK_TYPE_CASCADE_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - there are ports in the bitmap that not supported by
*                            the device.
*       GT_BAD_PARAM       - bad device number , or bad trunkId number , or number
*                            of ports (in the bitmap) larger then the number of
*                            entries in the 'Designated trunk table'
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*   1. This function does not set ports are 'Cascade ports' (and also not
*       check that ports are 'cascade').
*   2. This function sets only next tables :
*       a. the designated trunk table:
*          distribute MC/Cascade trunk traffic among the members
*       b. the 'Non-trunk' table entry.
*       c. 'per port' trunkId
*   3. because this function not set the 'Trunk members' table entry , the
*       application should not point to this trunk from any ingress unit , like:
*       FDB , PCL action redirect , NH , TTI action redirect , PVE ...
*       (it should be pointed ONLY from the device map table)
*   4. this API supports only trunks with types : CPSS_TRUNK_TYPE_FREE_E or
*       CPSS_TRUNK_TYPE_CASCADE_E.
*   5. next APIs are not supported from trunk with type : CPSS_TRUNK_TYPE_CASCADE_E
*       cpssDxChTrunkMembersSet ,
*       cpssDxChTrunkMemberAdd , cpssDxChTrunkMemberRemove,
*       cpssDxChTrunkMemberEnable , cpssDxChTrunkMemberDisable
*       cpssDxChTrunkDbEnabledMembersGet , cpssDxChTrunkDbDisabledMembersGet
*       cpssDxChTrunkDesignatedMemberSet , cpssDxChTrunkDbDesignatedMemberGet
*
********************************************************************************
*   Comparing the 2 function :
*
*        cpssDxChTrunkCascadeTrunkPortsSet   |   cpssDxChTrunkMembersSet
*   ----------------------------------------------------------------------------
*   1. purpose 'Cascade trunk'               | 1. purpose 'Network trunk' , and
*                                            |    also 'Cascade trunk' with up to
*                                            |    8 members
*   ----------------------------------------------------------------------------
*   2. supported number of members depends   | 2. supports up to 8 members
*      on number of entries in the           |    (also in Lion).
*      'Designated trunk table'              |
*      -- Lion supports 64 entries (so up to |
*         64 ports in the 'Cascade trunk').  |
*      -- all other devices supports 8       |
*         entries (so up to 8 ports in the   |
*         'Cascade trunk').                  |
*   ----------------------------------------------------------------------------
*   3. manipulate next trunk tables :        | 3. manipulate all trunk tables :
*      'Per port' trunk-id ,                 |  'Per port' trunk-id , 'Trunk members',
*      'Non-trunk' , 'Designated trunk'      |  'Non-trunk' , 'Designated trunk' tables.
*   ----------------------------------------------------------------------------
*   4. ingress unit must not point to this   | 4. no restriction on ingress/egress
*      trunk (because 'Trunk members' entry  |    units.
*       hold no ports)                       |
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkCascadeTrunkPortsSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
    GT_STATUS rc;

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    /* check validity and set to DB */
    rc = trunkDbCascadeCheckAndSet(devNum,trunkId,portsMembersPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the HW with the 'Cascade trunk' info */
    rc = trunkCascadeSet(devNum,trunkId,portsMembersPtr,
            0/*relevant to weighted trunk only*/,
            NULL/*relevant to weighted trunk only*/);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkCascadeTrunkPortsGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       Gets the 'Local ports' of the 'cascade' trunk .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*
* OUTPUTS:
*       portsMembersPtr - (pointer to) local ports bitmap of the cascade trunk.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PARAM       - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkCascadeTrunkPortsGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    OUT CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
    GT_STATUS rc;    /* return error core */
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/
    CPSS_PORTS_BMP_STC fullPortsBmp; /* full ports bmp of this device */

    TRUNK_LIB_INIT_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for cascade trunks or free trunk */
    TRUNK_TYPE_SUPPORT_CASCADE_CHECK_MAC(trunkInfoPtr);

    /* call appropriate virtual function */
    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpGetFunc(devNum,
            trunkId, &nonTrunkPorts);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&fullPortsBmp);

    /* calculate the local ports from the non-trunk members */
    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(portsMembersPtr,&fullPortsBmp,&nonTrunkPorts);

    return GT_OK;
}

/*******************************************************************************
* trunkDbCascadeWeightedPortsCheckAndSet
*
* DESCRIPTION:
*       check it :
*       1. the trunk can be used as 'Cascade trunk'
*       2. the action of setting trunk ports is the same settings as the DB hold .
*
*    the total weights of all the trunk members is restricted to :
*      a. must be equal to power of 2 (1,2,4,8,16,32,64...)
*      b. must not be larger then actual designated table size of the device.
*         (Lion ,Lion2 : 64  , other devices : 8)
*      c. not relevant when single member exists
*
*       set the bmp to the DB , state that the trunk is:
*           -- numOfMembers == 0 --> free trunk
*           -- otherwise --> cascade only trunk
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - device number
*       trunkId     - trunk id
*       numOfMembers - number of members in the array.
*                   value 0 - meaning that the trunk-id is 'invalidated' and
*                          trunk-type will be changed to : CPSS_TRUNK_TYPE_FREE_E
*                   value != 0 - meaning that the trunk-type will be : CPSS_TRUNK_TYPE_CASCADE_E
*       weightedMembersArray - (array of) members of the cascade trunk.
*                              each member hold relative weight (relative to Weight of all members)
*
* OUTPUTS:
*       portsMembersPtr - (pointer to) bmp of port members in the cascade trunk
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_OUT_OF_RANGE    - there are ports in the bitmap that not supported by
*                            the device.
*       GT_BAD_PARAM       - bad trunkId number , or number
*                            of ports (in the bitmap) larger then the number of
*                            entries in the 'Designated trunk table'
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS trunkDbCascadeWeightedPortsCheckAndSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfMembers,
    IN CPSS_TRUNK_WEIGHTED_MEMBER_STC       weightedMembersArray[],
    OUT CPSS_PORTS_BMP_STC       *portsMembersPtr
)
{
    GT_STATUS   rc;
    GT_U32      ii;
    GT_U32      totalWeights = 0;/* the total Weights of the members */

    CPSS_NULL_PTR_CHECK_MAC(portsMembersPtr);

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(portsMembersPtr);

    if(numOfMembers)
    {
        for(ii = 0; ii < numOfMembers; ii++)
        {
            if((numOfMembers != 1) && /* skip weight check of single trunk member */
                weightedMembersArray[ii].weight == 0)
            {
                /* the weight must have value other then 0 */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(weightedMembersArray[ii].member.hwDevice != PRV_CPSS_HW_DEV_NUM_MAC(devNum))
            {
                /* the cascade trunk hold members only from the local device */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(portsMembersPtr,weightedMembersArray[ii].member.port))
            {
                /* this port already exists in the members */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            CPSS_PORTS_BMP_PORT_SET_MAC(portsMembersPtr,weightedMembersArray[ii].member.port);

            totalWeights += weightedMembersArray[ii].weight;
        }

        /* check totalWeights must be equal to power of 2 (1,2,4,8,16,32,64...)
           meaning that one and only one bit should be set in the binary value

           not relevant when single member exists.
        */
        if((numOfMembers != 1) && (1 != prvCpssPpConfigBitmapNumBitsGet(totalWeights)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* check that totalWeights must not be larger then actual designated table size of the device

           not relevant when single member exists.
        */
        if((numOfMembers != 1) && (totalWeights > PRV_CPSS_TRUNK_DESIGNATED_TABLE_SIZE_MAC(devNum)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* check that the ports can be in the trunk , and set the ports in the
           DB as trunk members */
        rc = trunkDbCascadeCheckAndSet(devNum,trunkId,portsMembersPtr);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        /* check that we can free the trunk , and set the trunk as free */
        rc = trunkDbCascadeCheckAndSet(devNum,trunkId,NULL);
        if(rc != GT_OK)
        {
            return rc;
        }
    }


    return GT_OK;
}


/*******************************************************************************
* prvCpssGenericTrunkCascadeTrunkWithWeightedPortsSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the 'cascade' trunk with the Weighted specified 'Local ports'
*       overriding any previous setting.
*       the weights effect the number of times that each member will get representation
*       in the 'designated table' .
*       The cascade trunk may be invalidated by numOfMembers = 0.
*       Local ports are ports of only configured device.
*       Cascade trunk is:
*           - members are ports of only configured device pointed by devNum
*           - trunk members table is empty (see prvCpssGenericTrunkTableEntrySet)
*             Therefore it cannot be used as target by ingress engines like FDB,
*             Router, TTI, Ingress PCL and so on.
*           - members ports trunk ID are set (see prvCpssGenericTrunkPortTrunkIdSet).
*             Therefore packets ingresses in member ports are associated with trunk
*           - all members are enabled only and cannot be disabled.
*           - may be used for cascade traffic and pointed by the 'Device map table'
*             as the local target to reach to the 'Remote device'.
*             (For 'Device map table' refer to cpssDxChCscdDevMapTableSet(...))
*
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       numOfMembers - number of members in the array.
*                   value 0 - meaning that the trunk-id is 'invalidated' and
*                          trunk-type will be changed to : CPSS_TRUNK_TYPE_FREE_E
*                   value != 0 - meaning that the trunk-type will be : CPSS_TRUNK_TYPE_CASCADE_E
*       weightedMembersArray - (array of) members of the cascade trunk.
*                              each member hold relative weight (relative to Weight of all members)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - there are members that not supported by the device.
*       GT_BAD_PARAM       - bad device number , or bad trunkId number , or bad total
*                            weights (see restrictions below)
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk ,
*                            or this trunk hold members defined using prvCpssGenericTrunkMembersSet(...)
*       GT_BAD_PTR       - when numOfMembers != 0 and weightedMembersArray = NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*   1. This function does not set ports as 'Cascade ports' (and also not
*       check that ports are 'cascade').
*   2. This function sets only next tables :
*       a. the designated trunk table:
*          distribute MC/Cascade trunk traffic among the members according to their weight
*       b. the 'Non-trunk' table entry.
*       c. 'per port' trunkId
*   3. because this function not set the 'Trunk members' table entry , the
*       application should not point to this trunk from any ingress unit , like:
*       FDB , PCL action redirect , NH , TTI action redirect , PVE ...
*       (it should be pointed ONLY from the device map table)
*   4. this API supports only trunks with types : CPSS_TRUNK_TYPE_FREE_E or
*       CPSS_TRUNK_TYPE_CASCADE_E.
*   5. next APIs are not supported from trunk with type : CPSS_TRUNK_TYPE_CASCADE_E
*       prvCpssGenericTrunkMembersSet ,
*       prvCpssGenericTrunkMemberAdd , prvCpssGenericTrunkMemberRemove,
*       prvCpssGenericTrunkMemberEnable , prvCpssGenericTrunkMemberDisable
*       prvCpssGenericTrunkDbEnabledMembersGet , prvCpssGenericTrunkDbDisabledMembersGet
*       prvCpssGenericTrunkDesignatedMemberSet , prvCpssGenericTrunkDbDesignatedMemberGet
*   6. the total weights of all the trunk members is restricted to :
*      a. must be equal to power of 2 (1,2,4,8,16,32,64...)
*      b. must not be larger then actual designated table size of the device.
*         (Lion ,Lion2 : 64  , other devices : 8)
*      c. not relevant when single member exists
*
********************************************************************************
*   Comparing the 2 function :
*
*prvCpssGenericTrunkCascadeTrunkWithWeightedPortsSet |   prvCpssGenericTrunkMembersSet
*   ----------------------------------------------------------------------------
*   1. purpose 'Cascade trunk'               | 1. purpose 'Network trunk' , and
*                                            |    also 'Cascade trunk' with up to
*                                            |    8 members
*   ----------------------------------------------------------------------------
*   2. supported number of members depends   | 2. supports up to 8 members
*      on number of entries in the           |    (also in Lion).
*      'Designated trunk table'              |
*      -- Lion supports 64 entries (so up to |
*         64 ports in the 'Cascade trunk').  |
*      -- all other devices supports 8       |
*         entries (so up to 8 ports in the   |
*         'Cascade trunk').                  |
*   ----------------------------------------------------------------------------
*   3. manipulate next trunk tables :        | 3. manipulate all trunk tables :
*      'Per port' trunk-id                   |  'Per port' trunk-id , 'Trunk members',
*      'Non-trunk' , 'Designated trunk'      |  'Non-trunk' , 'Designated trunk' tables.
*   ----------------------------------------------------------------------------
*   4. ingress unit must not point to this   | 4. no restriction on ingress/egress
*      trunk (because 'Trunk members' entry  |    units.
*       hold no ports)                       |
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkCascadeTrunkWithWeightedPortsSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfMembers,
    IN CPSS_TRUNK_WEIGHTED_MEMBER_STC       weightedMembersArray[]
)
{
    GT_STATUS rc;
    CPSS_PORTS_BMP_STC       portsMembers;/* bmp of the cascade ports*/
    CPSS_PORTS_BMP_STC       *portsMembersPtr;/*pointer to the cascade ports bmp*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    /* check validity and set to DB */
    rc = trunkDbCascadeWeightedPortsCheckAndSet(devNum,trunkId,numOfMembers,weightedMembersArray,&portsMembers);
    if(rc != GT_OK)
    {
        return rc;
    }

    portsMembersPtr = (numOfMembers == 0) ? NULL : (&portsMembers);

    /* set the HW with the 'Cascade trunk' Weighted Ports info */
    rc = trunkCascadeSet(devNum,trunkId,portsMembersPtr,numOfMembers,weightedMembersArray);

    return rc;
}



/*******************************************************************************
* prvCpssGenericTrunkCascadeTrunkWithWeightedPortsGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       This function gets the Weighted 'Local ports' of 'cascade' trunk .
*       the weights reflect the number of times that each member is represented
*       in the 'designated table'
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        ExMxPm; ExMx.
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
*       numOfMembersPtr - (pointer to) max num of members to retrieve - this value refer to the number of
*                                members that the array of weightedMembersArray[] can retrieve.
*
* OUTPUTS:
*       numOfMembersPtr - (pointer to) the actual num of members in the trunk
*                       (up to : Lion,Lion2 : 64 , others : 8)
*       weightedMembersArray - (array of) members that are members of the cascade trunk.
*                              each member hold relative weight (relative to Weight of all members)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_CASCADE_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkCascadeTrunkWithWeightedPortsGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                   *numOfMembersPtr,
    OUT CPSS_TRUNK_WEIGHTED_MEMBER_STC       weightedMembersArray[]
)
{
    GT_STATUS rc;    /* return error core */
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */
    CPSS_PORTS_BMP_STC  trunkMembers;/* ports that are in the trunk */
    CPSS_PORTS_BMP_STC  designatedPorts;/* the entry in the designate ports table*/
    CPSS_PORTS_BMP_STC  trunkMembersInDesignatedPorts;/* the trunk members in the current entry in the designate ports table*/
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/
    CPSS_PORTS_BMP_STC fullPortsBmp; /* full ports bmp of this device */
    GT_U32  ii,jj;
    GT_U32  numDesig;/* number of entries in the designated ports trunk table */
    GT_U32  maxNumOfMembers;/* max number of members to put into weightedMembersArray[]*/
    GT_U32  numOfTrunkMembers;/* number of members in the trunk*/
    GT_U32  numOfTrunkMembersInDesignatedTable;/* number of members in the trunk in the designated entry (must be 1 - for cascade trunk)*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(numOfMembersPtr);
    CPSS_NULL_PTR_CHECK_MAC(weightedMembersArray);

    numDesig = PRV_CPSS_TRUNK_DESIGNATED_TABLE_SIZE_MAC(devNum);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for cascade trunks or free trunk */
    TRUNK_TYPE_SUPPORT_CASCADE_CHECK_MAC(trunkInfoPtr);

    /* call appropriate virtual function */
    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpGetFunc(devNum,
            trunkId, &nonTrunkPorts);
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_TRUNK_PORTS_FULL_MASK_MAC(devNum,&fullPortsBmp);

    /* calculate the local ports from the non-trunk members */
    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&trunkMembers,&fullPortsBmp,&nonTrunkPorts);

    numOfTrunkMembers = 0;
    for(ii = 0 ; ii < CPSS_MAX_PORTS_BMP_NUM_CNS ; ii++)
    {
        numOfTrunkMembers += prvCpssPpConfigBitmapNumBitsGet(trunkMembers.ports[ii]);
    }

    maxNumOfMembers = (numOfTrunkMembers > (*numOfMembersPtr)) ?
                      (*numOfMembersPtr) :
                      numOfTrunkMembers;

    ii = 0;
    /* fill the members of the array */
    for(jj = 0 ; jj < maxNumOfMembers ; jj++)
    {
        /* this is cascade trunk so all the ports belong to this device */
        weightedMembersArray[jj].member.hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);

        do
        {
            if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&trunkMembers,ii))
            {
                /* found index of trunk member */
                break;
            }
        }while((++ii) < CPSS_MAX_PORTS_NUM_CNS);

        weightedMembersArray[jj].member.port = ii;
        weightedMembersArray[jj].weight = 0;
        /* continue from next index */
        ii++;
    }

    if(numOfTrunkMembers != 0)
    {
        /* calculate the weights of the members according to the number of time represented in the trunk entry */
        for(ii = 0; ii < numDesig; ii++)
        {
            /* get "existing" entry value */
            rc = trunkDesignatedPortsGetHw(devNum,ii,&designatedPorts);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* get the bmp of port(s) that is(are) member(s) in the trunk that appears in current designated entry */
            CPSS_PORTS_BMP_BITWISE_AND_MAC(&trunkMembersInDesignatedPorts , &designatedPorts , &trunkMembers);


            /* the trunk must not hold more then single member --> otherwise the
               buffer management of the device may not release all the buffers */
            numOfTrunkMembersInDesignatedTable = 0;
            for(jj = 0 ; jj < CPSS_MAX_PORTS_BMP_NUM_CNS ; jj++)
            {
                numOfTrunkMembersInDesignatedTable += prvCpssPpConfigBitmapNumBitsGet(trunkMembersInDesignatedPorts.ports[jj]);
            }

            if(1 != numOfTrunkMembersInDesignatedTable)
            {
                /* for cascade trunk - the device must hold exactly single member of the trunk in a designated table entry */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
            }

            /* for each member of the trunk check if it is member in the trunk */
            for(jj = 0 ; jj < maxNumOfMembers ; jj++)
            {
                if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&trunkMembersInDesignatedPorts,weightedMembersArray[jj].member.port))
                {
                    weightedMembersArray[jj].weight ++;
                    break;/* we already checked that the entry hold exactly single member of the trunk */
                }
            }
        }

        if((*numOfMembersPtr) >= numOfTrunkMembers)
        {
            /* compress the values ONLY when all values are available to the caller*/
            for( ii = 0 ; ii < 32; ii++)
            {
                /* compress the results : do as must as possible divisions by 2 */
                for(jj = 0 ; jj < maxNumOfMembers ; jj++)
                {
                    if(weightedMembersArray[jj].weight & 1)
                    {
                        /* Odd number --> can't compress any more */
                        break;
                    }
                }

                if(jj != maxNumOfMembers)
                {
                    /* can't compress any more */
                    break;
                }

                /* divide all weights by 2 */
                for(jj = 0 ; jj < maxNumOfMembers ; jj++)
                {
                    weightedMembersArray[jj].weight >>= 1;
                }
            }
        }
    }/*numOfTrunkMembers != 0*/

    /* update the caller about the actual number of members in the trunk */
    *numOfMembersPtr = numOfTrunkMembers;

    return GT_OK;
}


/*******************************************************************************
* prvCpssGenericTrunkMemberAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function add member to the trunk in the device.
*       If member is already in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members,
*           now taking into account also the added member
*       else
*           1. If added member is not the designated member - set its relevant bits to 0
*           2. If added member is the designated member & it's enabled,
*              set its relevant bits on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to add member to the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to add to the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_FULL - trunk already contains maximum supported members
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkMemberAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_STATUS rc;

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(memberPtr);

    /* add the member to the DB */
    rc = trunkDbMemberAdd(devNum,trunkId,memberPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check if need to do sorting on the trunk members. */
    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->sortTrunkMembers == GT_TRUE)
    {
        rc = trunkDbMembersForTrunkIdSort(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* add the member to the HW */
    rc = trunkMemberAction(devNum,trunkId,memberPtr,
                            PRV_CPSS_TRUNK_ACTION_ADD_E,
                            IS_LOCAL_CHECK_E);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkMemberRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function remove member from a trunk in the device.
*       If member not exists in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the removed member
*       else
*           1. If removed member is not the designated member - nothing to do
*           2. If removed member is the designated member set its relevant bits
*              on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to remove member from the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to remove from the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkMemberRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_STATUS rc;
    IS_LOCAL_ENT   isLocalMember; /*indication that the removed member is local to this device
                                IS_LOCAL_TRUE_E - the member is local to this device
                                IS_LOCAL_FALSE_E - the member is NOT local to this device*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(memberPtr);

    /* remove the member from the DB */
    rc = trunkDbMemberDel(devNum,trunkId,memberPtr,&isLocalMember);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check if need to do sorting on the trunk members. */
    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->sortTrunkMembers == GT_TRUE)
    {
        rc = trunkDbMembersForTrunkIdSort(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* remove the member from the HW */
    rc = trunkMemberAction(devNum,trunkId,memberPtr,
                            PRV_CPSS_TRUNK_ACTION_REMOVE_E,
                            isLocalMember);

    if(rc == GT_HW_ERROR)
    {
        return rc;
    }

    /* the return value need to be GT_OK , because the remove action succeeded.
       all the validity checks were done by trunkDbMemberDel

       So the only reason that the function trunkMemberAction not return GT_OK
       was on extended parameters checking , meaning that the member was not in
       HW in the first place.

       so we need to return GT_OK
       */

    return GT_OK;
}


/*******************************************************************************
* prvCpssGenericTrunkMemberEnable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function enable (disabled)existing member of trunk in the device.
*       If member is already enabled in this trunk , function do nothing and
*       return GT_OK.
*
*       Notes about designated trunk table:
*       If (no designated defined)
*           re-distribute MC/Cascade trunk traffic among the enabled members -
*           now taking into account also the enabled member
*       else
*           1. If enabled member is not the designated member - set its relevant bits to 0
*           2. If enabled member is the designated member set its relevant bits
*               on all indexes to 1.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number on which to enable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to enable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkMemberEnable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
{
    GT_STATUS rc;

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    CPSS_NULL_PTR_CHECK_MAC(memberPtr);

    /* enable the member in the DB */
    rc = trunkDbMemberEnable(devNum,trunkId,memberPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* check if need to do sorting on the trunk members. */
    if(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->sortTrunkMembers == GT_TRUE)
    {
        rc = trunkDbMembersForTrunkIdSort(devNum,trunkId);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* enable the member in the HW */
    rc = trunkMemberAction(devNum,trunkId,memberPtr,
                            PRV_CPSS_TRUNK_ACTION_ENABLE_E,
                            IS_LOCAL_CHECK_E);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkNonTrunkPortsAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       add the ports to the trunk's non-trunk entry .
*       NOTE : the ports are add to the "non trunk" table only and not effect
*              other trunk relate tables/registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to add to
*                             "non-trunk members"
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkNonTrunkPortsAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
{
    GT_STATUS rc;    /* return error core */
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpGetFunc(devNum,
            trunkId,&nonTrunkPorts);
    if(rc != GT_OK)
    {
        return rc;
    }

    CPSS_PORTS_BMP_BITWISE_OR_MAC(&nonTrunkPorts,&nonTrunkPorts,nonTrunkPortsBmpPtr);

    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpSetFunc(devNum,
            trunkId,&nonTrunkPorts);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkNonTrunkPortsRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*      Removes the ports from the trunk's non-trunk entry .
*      NOTE : the ports are removed from the "non trunk" table only and not
*              effect other trunk relate tables/registers.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to remove from
*                             "non-trunk members"
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkNonTrunkPortsRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
{
    GT_STATUS rc;    /* return error core */
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpGetFunc(devNum,
            trunkId,&nonTrunkPorts);
    if(rc != GT_OK)
    {
        return rc;
    }

    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&nonTrunkPorts,&nonTrunkPorts,nonTrunkPortsBmpPtr);

    rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpSetFunc(devNum,
            trunkId,&nonTrunkPorts);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkInit
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS generic Trunk initialization of PP Tables/registers and
*       SW shadow data structures, all ports are set as non-trunk ports.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum             - device number
*       maxNumberOfTrunks  - maximum number of trunk groups.
*                            when this number is 0 , there will be no shadow
*                            used.
*                            Note:
*                            that means that API's that used shadow will FAIL.
*
* OUTPUTS:
*           None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on other error.
*       GT_BAD_PARAM             - wrong devNum
*       GT_OUT_OF_RANGE  - the numberOfTrunks is more then what HW support
*                          the Ex1x5, Ex1x6 devices support 31 trunks.
*                          the Ex6x5, Ex6x8 , Ex6x0 devices support 7 trunks.
*       GT_BAD_STATE - if library already initialized with different number of
*                       trunks than requested
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkInit
(
    IN  GT_U8       devNum,
    IN  GT_U32      maxNumberOfTrunks
)
{
    GT_STATUS rc;    /* return error core */

    /* check device number */
    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* initialize the CPSS SW DB */
    rc = trunkDbInit(devNum,maxNumberOfTrunks);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* initialize the HW */
    rc = trunkHwInit(devNum);

    return rc;
}

/*******************************************************************************
* prvCpssGenericTrunkMcLocalSwitchingEnableSet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Enable/Disable sending multi-destination packets back to its source
*       trunk on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       enable      - Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. the behavior of multi-destination traffic ingress from trunk is
*       not-affected by setting of cpssDxChBrgVlanLocalSwitchingEnableSet
*       and not-affected by setting of cpssDxChBrgPortEgressMcastLocalEnable
*       2. the functionality manipulates the 'non-trunk' table entry of the trunkId
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkMcLocalSwitchingEnableSet
(
    IN GT_U8            devNum,
    IN GT_TRUNK_ID      trunkId,
    IN GT_BOOL          enable
)
{
    GT_STATUS   rc;
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/
    CPSS_PORTS_BMP_STC      nonTrunkPorts;/* ports that not member in the trunk */

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    /* calculate the bitmap of non-trunk members */
    rc = trunkDbNonTrunkPortsCalc(devNum,trunkId,&nonTrunkPorts);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* update the DB before calling trunkNonTrunkPortUpdateHw(...) */
    trunkInfoPtr->allowMultiDestFloodBack = enable;

    /* update the non-trunk entry */
    return trunkNonTrunkPortUpdateHw(devNum,
                        trunkId,
                        NULL,/* not relevant*/
                        &nonTrunkPorts,
                        PRV_CPSS_TRUNK_ACTION_MULTI_DEST_FLOOD_BACK_SET_E,
                        GT_FALSE);/* not relevant*/
}

/*******************************************************************************
* prvCpssGenericTrunkDbMcLocalSwitchingEnableGet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Get the current status of Enable/Disable sending multi-destination packets
*       back to its source trunk on the local device.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*
* OUTPUTS:
*       enablePtr   - (pointer to) Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbMcLocalSwitchingEnableGet
(
    IN GT_U8            devNum,
    IN GT_TRUNK_ID      trunkId,
    OUT GT_BOOL          *enablePtr
)
{
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);

    /********************************/
    /* check if this trunk is in DB */
    /********************************/
    TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

    /* this API supported only for regular trunks or free trunk */
    TRUNK_TYPE_SUPPORT_REGULAR_CHECK_MAC(trunkInfoPtr);

    *enablePtr = trunkInfoPtr->allowMultiDestFloodBack;

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericTrunkPortMcEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the mode how multicast destination traffic egress the
*       selected port while it is a trunk member.
*       see details in description of CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       mode     - the 'designated mc table' mode of the port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PARAM       - bad device or port number or mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If disabled multicast destination packets would not egress through the
*       configured port.
*       This configuration has no influence on unicast destination traffic or
*       when the port is not a trunk member.
*
********************************************************************************/
GT_STATUS prvCpssGenericTrunkPortMcEnableSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT mode
)
{
    GT_STATUS rc;   /* return status */
    GT_TRUNK_ID trunkId;    /* trunk Id of port */
    PRV_CPSS_TRUNK_ENTRY_INFO_STC *trunkInfoPtr;/*The trunk DB for current trunkId*/
    CPSS_PORTS_BMP_STC  nonTrunkPorts;/* ports that not member in the trunk */

    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    if(GT_FALSE == PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->designatedTrunkTableSplit)
    {
        /* the device is not applicable for this feature */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    /* clear bit from : portsMcForbid,portsMcForce */
    CPSS_PORTS_BMP_PORT_CLEAR_MAC(
        &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForbid), portNum);

    CPSS_PORTS_BMP_PORT_CLEAR_MAC(
        &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForce), portNum);

    if( CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORCE_ALL_E == mode )
    {
        /* add port to 'portsMcForce' */
        CPSS_PORTS_BMP_PORT_SET_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForce), portNum);
    }
    else
    if( CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORBID_ALL_E == mode )
    {
        /* add port to 'portsMcForbid' */
        CPSS_PORTS_BMP_PORT_SET_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForbid), portNum);
    }

    trunkId = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portTrunkIdArray[portNum];
    if(trunkId != 0)
    {
        trunkInfoPtr = TRUNK_ENTRY_GET_MAC(devNum,trunkId);
        TRUNK_ENTRY_CHECK_MAC(trunkInfoPtr);

        /* calculate the bitmap of non-trunk members */
        /*rc = trunkDbNonTrunkPortsCalc(devNum,trunkId,&nonTrunkPorts);*/
        rc = TRUNK_VIRTUAL_FUNC_MAC(devNum)->nonMembersBmpGetFunc(devNum,
                                             trunkId,&nonTrunkPorts);
        if(GT_OK != rc)
        {
            return rc;
        }

        /* update designated ports table */
        rc = trunkDesignatedPortsTableUpdate(devNum,trunkId,
             trunkInfoPtr->enabledCount,&trunkInfoPtr->membersPtr[0],
             &nonTrunkPorts,NULL,
             0/*relevant to weighted trunk only*/,
             NULL/*relevant to weighted trunk only*/);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericTrunkDbPortMcEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function gets the mode how multicast destination traffic egress the
*       selected port while it is a trunk member.
*       see details in description of CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT
*
* APPLICABLE DEVICES:
*        xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number.
*
* OUTPUTS:
*       modePtr     - (pointer to) the 'designated mc table' mode of the port
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_BAD_PARAM       - bad device or port number
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The function output is retrieved from CPSS related trunk DB.
*       This configuration has no influence on unicast destination traffic or
*       when the port is not a trunk member.
*
********************************************************************************/
GT_STATUS prvCpssGenericTrunkDbPortMcEnableGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT  *modePtr
)
{
    TRUNK_LIB_INIT_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    if(GT_FALSE == PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->designatedTrunkTableSplit)
    {
        /* the device is not applicable for this feature */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForbid),portNum))
    {
        *modePtr = CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORBID_ALL_E;
    }
    else
    if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsMcForce),portNum))
    {
        *modePtr = CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_FORCE_ALL_E;
    }
    else
    {
        *modePtr = CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_NATIVE_WEIGHTED_E;
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericTrunkDbPortTrunkIdModeSet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Function allow application to be responsible for the 'TrunkId of the port'.
*       in High level mode APIs the 'TrunkId of the port' is set internally be
*       the CPSS and may override the current value of the 'TrunkId of the port'.
*       This API allows application to notify the CPSS to not manage the 'TrunkId
*       of the port'.
*
*       function cpssDxChTrunkPortTrunkIdSet(...) allow application to manage
*       the trunkId of the port
*
*       NOTE: this is 'DB' operation (database) , without HW access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portNum  - physical port number.
*       manageMode - the management mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_BAD_PARAM             - bad device number , or
*                               bad port number , or manageMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbPortTrunkIdModeSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT manageMode
)
{
    TRUNK_LIB_INIT_CHECK_MAC(devNum);

    if( CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_APPLICATION_E == manageMode )
    {
        /* add port to 'portsTrunkIdByApplication' */
        CPSS_PORTS_BMP_PORT_SET_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsTrunkIdByApplication), portNum);
    }
    else if( CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_CPSS_E == manageMode )
    {
        /* remove port from 'portsTrunkIdByApplication' */
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsTrunkIdByApplication), portNum);
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssGenericTrunkDbPortTrunkIdModeGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Function get the 'management mode' of the port's trunkId.
*
*       NOTE: this is 'DB' operation (database) , without HW access.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - the device number
*       portNum  - physical port number.
*
* OUTPUTS:
*       manageModePtr - (pointer to) the management mode.
*
* RETURNS:
*       GT_OK                    - successful completion
*       GT_FAIL                  - an error occurred.
*       GT_BAD_PARAM             - bad device number , or
*                               bad port number , or manageMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenericTrunkDbPortTrunkIdModeGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    OUT CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_ENT *manageModePtr
)
{
    TRUNK_LIB_INIT_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(manageModePtr);

    if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(
            &(PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->portsTrunkIdByApplication),portNum))
    {
        *manageModePtr = CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_APPLICATION_E;
    }
    else
    {
        *manageModePtr = CPSS_TRUNK_PORT_TRUNK_ID_MANAGEMENT_MODE_BY_CPSS_E;
    }

    return GT_OK;
}

