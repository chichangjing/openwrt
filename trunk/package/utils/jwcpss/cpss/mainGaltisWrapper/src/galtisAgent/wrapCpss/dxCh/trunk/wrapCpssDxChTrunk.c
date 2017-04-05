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
* wrapCpssDxChTrunk.c
*
* DESCRIPTION:
*       Wrapper functions for cpssDxCh Trunk functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 46 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>

/* number of trunks */
#define DXCH_NUM_TRUNKS_127_CNS  127

#define TRUNK_WA_SKIP_TRUNK_ID_MAC(_trunkId)    \
        if(dxChTrunkWaNeeded == GT_TRUE)        \
        {                                       \
            if(_trunkId == 64)                  \
            {                                   \
                _trunkId = 126;                 \
            }                                   \
        }

#define CHECK_RC(rc)\
    if(rc != GT_OK) return rc

static GT_TASK taskId = 0;

/* indication that the device uses check of physical port by mask of port */
#define IS_PHY_PORT_BY_MASK_MAC(devNum) \
    ((PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->validityMask.port >= 0xFF) ? 1 : 0)

/*
function return
1 - the bmp of ports     covers all 'existing ports of the device'
0 - the bmp of ports NOT covers all 'existing ports of the device'
*/
static GT_U32   areAllExistingPorts(IN GT_U8 devNum , IN CPSS_PORTS_BMP_STC* currentPortsBmpPtr)
{
    CPSS_PORTS_BMP_STC tmpPortsBmp;
    GT_U32  returnValue;

    if(IS_PHY_PORT_BY_MASK_MAC(devNum))
    {
        CPSS_PORTS_BMP_PORT_SET_ALL_MAC(&tmpPortsBmp);
        returnValue = CPSS_PORTS_BMP_ARE_EQUAL_MAC(currentPortsBmpPtr, &tmpPortsBmp);
    }
    else
    {
        CPSS_PORTS_BMP_BITWISE_AND_MAC(&tmpPortsBmp,currentPortsBmpPtr,&PRV_CPSS_PP_MAC(devNum)->existingPorts);

        returnValue = CPSS_PORTS_BMP_ARE_EQUAL_MAC(&tmpPortsBmp,&PRV_CPSS_PP_MAC(devNum)->existingPorts);
    }

    return returnValue;
}

/* macro return :
1 - the bmp of ports     covers all 'existing ports of the device'
0 - the bmp of ports NOT covers all 'existing ports of the device'
*/
#define ARE_ALL_EXISTING_PORTS_MAC(devNum,currentPortsBmpPtr)  \
   areAllExistingPorts(devNum,currentPortsBmpPtr)


static struct{
    GT_BOOL valid;
    GT_BOOL useEntireTable;
    GT_BOOL useVid;
}designatedTableModeInfo[PRV_CPSS_MAX_PP_DEVICES_CNS] = {{0}};

static GT_U32   debugDesignatedMembers = 0;

/*******************************************************************************
* cpssDxChTrunkInit
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS DxCh Trunk initialization of PP Tables/registers and
*       SW shadow data structures, all ports are set as non-trunk ports.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum             - device number
*       maxNumberOfTrunks  - maximum number of trunk groups.(0..127)
*                            when this number is 0 , there will be no shadow used
*                            Note:
*                            that means that API's that used shadow will FAIL.
*       trunkMembersMode   - type of how the CPSS SW will fill the HW with
*                            trunk members
*
* OUTPUTS:
*           none.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_BAD_PARAM     - wrong devNum or bad trunkLbhMode
*       GT_OUT_OF_RANGE  - the numberOfTrunks > 127
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkInit

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    GT_U8                               maxNumberOfTrunks;
    CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT    trunkMembersMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maxNumberOfTrunks = (GT_U8)inArgs[1];
    trunkMembersMode = (CPSS_DXCH_TRUNK_MEMBERS_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTrunkInit(devNum, maxNumberOfTrunks, trunkMembersMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTrunkDesignatedMemberSet
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
* APPLICABLE DEVICES:   All DxCh Devices
*
* INPUTS:
*       devNum      - the device number on which to add member to the trunk
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
*       memberPtr   - (pointer to)the member to add to the trunk.
*                     relevant only when enable = GT_TRUE
*
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
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST         - this member already exists in another trunk.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedMemberSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              rc;
    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    GT_BOOL                enable;
    CPSS_TRUNK_MEMBER_STC designatedMember;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    enable = (GT_BOOL)inArgs[2];
    designatedMember.port = (GT_U32)inArgs[3];
    designatedMember.hwDevice = (GT_U32)inArgs[4];
    CONVERT_DEV_PORT_DATA_MAC(designatedMember.hwDevice, designatedMember.port);

    /* call cpss api function */
    rc = cpssDxChTrunkDesignatedMemberSet(devNum,trunkId, enable, &designatedMember);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/* tmp function until Galtis function ready */
GT_STATUS tmp_cpssDxChTrunkDesignatedMemberSet
(
    IN GT_U32                   devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_BOOL                  enable,
    IN GT_U32                   member_port,
    IN GT_U32                   member_device
)
{
    CPSS_TRUNK_MEMBER_STC designatedMember;

    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    designatedMember.port = member_port;
    designatedMember.hwDevice = member_device;
    CONVERT_DEV_PORT_DATA_MAC(designatedMember.hwDevice, designatedMember.port);

    return cpssDxChTrunkDesignatedMemberSet((GT_U8)devNum,trunkId, enable, &designatedMember);
}

/*******************************************************************************
* cpssDxChTrunkDbDesignatedMemberGet
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
* APPLICABLE DEVICES:   All DxCh Devices
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
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDbDesignatedMemberGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              rc;
    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    GT_BOOL                enable;
    CPSS_TRUNK_MEMBER_STC designatedMember;
    GT_HW_DEV_NUM           __HwDev; /* Dummy for converting. */
    GT_PHYSICAL_PORT_NUM    __Port; /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* call cpss api function */
    rc = cpssDxChTrunkDbDesignatedMemberGet(devNum,trunkId, &enable, &designatedMember);

    __Port = designatedMember.port;
    __HwDev  = designatedMember.hwDevice;
    CONVERT_BACK_DEV_PORT_DATA_MAC(__HwDev, __Port) ;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d",enable,__Port,__HwDev);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkDbTrunkTypeGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Get the trunk type.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh Devices
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
static CMD_STATUS wrCpssDxChTrunkDbTrunkTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              rc;
    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    CPSS_TRUNK_TYPE_ENT    trunkType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* call cpss api function */
    rc = cpssDxChTrunkDbTrunkTypeGet(devNum,trunkId, &trunkType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",trunkType);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTrunkHashCrcParametersSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the CRC hash parameter , relevant for CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E .
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       devNum  - The device number.
*       crcMode - The CRC mode .
*       crcSeed - The seed used by the CRC computation .
*                 when crcMode is CRC_6 mode : crcSeed is in range of 0..0x3f (6 bits value)
*                 when crcMode is CRC_16 mode : crcSeed is in range of 0..0xffff (16 bits value)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or crcMode
*       GT_OUT_OF_RANGE          - crcSeed out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashCrcParametersSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              rc;
    GT_U8                  devNum;
    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     crcMode;
    GT_U32                               crcSeed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    crcMode = (CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT)inArgs[1];
    crcSeed = (GT_U32)inArgs[2];

    /* call cpss api function */
    rc = cpssDxChTrunkHashCrcParametersSet(devNum,crcMode, crcSeed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashCrcParametersGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the CRC hash parameter , relevant for CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E .
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       devNum  - The device number.
*
* OUTPUTS:
*       crcModePtr - (pointer to) The CRC mode .
*       crcSeedPtr - (pointer to) The seed used by the CRC computation .
*                 when crcMode is CRC_6 mode : crcSeed is in range of 0..0x3f (6 bits value)
*                 when crcMode is CRC_16 mode : crcSeed is in range of 0..0xffff (16 bits value)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashCrcParametersGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              rc;
    GT_U8                  devNum;
    CPSS_DXCH_TRUNK_LBH_CRC_MODE_ENT     crcMode;
    GT_U32                               crcSeed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    rc = cpssDxChTrunkHashCrcParametersGet(devNum,&crcMode, &crcSeed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d",crcMode,crcSeed);
    return CMD_OK;
}

/**************Table: cpssDxChTrunkMembersSet*****************/

static  GT_TRUNK_ID     gTrunkId;

/*******************************************************************************
* cpssDxChTrunkMembersSet
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
* APPLICABLE DEVICES:   All DxCh devices
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
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST - one of the members already exists in another trunk
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkMembersSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_U32                 numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC  membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                 numOfDisabledMembers;
    GT_TRUNK_ID            trunkId;
    GT_U8                  i, j;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    numOfEnabledMembers = (GT_U32)inFields[1];
    numOfDisabledMembers = (GT_U32)inFields[2];

    if(numOfEnabledMembers + numOfDisabledMembers > CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "");
        return CMD_OK;
    }

    j = 3;

    for(i=0; i < numOfEnabledMembers; i++)
    {
         membersArray[i].port = (GT_PHYSICAL_PORT_NUM)inFields[j];
         membersArray[i].hwDevice = (GT_HW_DEV_NUM)inFields[j+1];
         CONVERT_DEV_PORT_DATA_MAC(membersArray[i].hwDevice, membersArray[i].port);
         j = j+2;
    }

    for(/* continue i , j*/ ; i < (numOfEnabledMembers + numOfDisabledMembers); i++)
    {
        membersArray[i].port = (GT_PHYSICAL_PORT_NUM)inFields[j];
        membersArray[i].hwDevice = (GT_HW_DEV_NUM)inFields[j+1];
        CONVERT_DEV_PORT_DATA_MAC(membersArray[i].hwDevice, membersArray[i].port);
        j = j+2;
    }

    /* call cpss api function */
    result = cpssDxChTrunkMembersSet(devNum, trunkId, numOfEnabledMembers,
                                     &membersArray[0], numOfDisabledMembers,
                                     &membersArray[numOfEnabledMembers]);
    if(result != GT_OK)
    {
         /* pack output arguments to galtis string */
         galtisOutput(outArgs, result, "");
         return CMD_OK;
    }

    if(cmdAppTrunkMembersSet != 0)
    {
        result = cmdAppTrunkMembersSet(devNum, trunkId, numOfEnabledMembers,
                                         &membersArray[0], numOfDisabledMembers,
                                         &membersArray[numOfEnabledMembers]);
        if(result != GT_OK)
        {
             /* pack output arguments to galtis string */
             galtisOutput(outArgs, result, "");
             return CMD_OK;
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*
 the function keep on looking for next trunk that is not empty starting with
 trunkId = (gTrunkId+1).

 function return the trunk members (enabled + disabled)
*/
static CMD_STATUS wrCpssDxChTrunkMembersGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_U32                 numOfEnabledMembers=0;
    CPSS_TRUNK_MEMBER_STC  enabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                 numOfDisabledMembers=0;
    CPSS_TRUNK_MEMBER_STC  disabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U8                  i, j;
    GT_TRUNK_ID     tmpTrunkId;
    CPSS_TRUNK_TYPE_ENT     trunkType;/* trunk type */
    CPSS_PORTS_BMP_STC      cascadePortsBmp;/* cascade ports bmp */
    GT_U32          ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tmpTrunkId = gTrunkId;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(tmpTrunkId);

    /*check if trunk is valid by checking if GT_OK*/
    while(gTrunkId <= DXCH_NUM_TRUNKS_127_CNS)
    {
        gTrunkId++;

        TRUNK_WA_SKIP_TRUNK_ID_MAC(gTrunkId);

        tmpTrunkId = gTrunkId;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(tmpTrunkId);


        result = cpssDxChTrunkDbTrunkTypeGet(devNum,tmpTrunkId,&trunkType);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
            return CMD_OK;
        }

        switch(trunkType)
        {
            case CPSS_TRUNK_TYPE_FREE_E:
            case CPSS_TRUNK_TYPE_REGULAR_E:
                numOfEnabledMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
                result = cpssDxChTrunkDbEnabledMembersGet(devNum, tmpTrunkId,
                                                              &numOfEnabledMembers,
                                                              enabledMembersArray);
                if(result != GT_OK)
                {
                    galtisOutput(outArgs, result, "%d",-1);/* Error ???? */
                    return CMD_OK;
                }

                /*Trunk gTrunkId seems to be valid. Now go on and get disabled members*/
                numOfDisabledMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
                result = cpssDxChTrunkDbDisabledMembersGet(devNum, tmpTrunkId,
                                                                  &numOfDisabledMembers,
                                                                  disabledMembersArray);
                if(result != GT_OK)
                {
                    galtisOutput(outArgs, result, "%d",-1);/* Error ???? */
                    return CMD_OK;
                }
                break;
            case CPSS_TRUNK_TYPE_CASCADE_E:
                numOfDisabledMembers = 0;/* no disabled ports in cascade trunk */
                numOfEnabledMembers = 0;/* initialize with 0 */

                result = cpssDxChTrunkCascadeTrunkPortsGet(devNum,tmpTrunkId,&cascadePortsBmp);
                if(result != GT_OK)
                {
                    galtisOutput(outArgs, result, "%d",-1);/* Error ???? */
                    return CMD_OK;
                }

                for(ii = 0 ; ii < CPSS_MAX_PORTS_NUM_CNS; ii++)
                {
                    if(numOfEnabledMembers == CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
                    {
                        /* can't display more then the 8 members , even though
                           the cascade trunk may support more ... (Lion) */
                        break;
                    }

                    if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&cascadePortsBmp,ii))
                    {
                        continue;
                    }

                    enabledMembersArray[numOfEnabledMembers].port = (GT_PHYSICAL_PORT_NUM)ii;
                    enabledMembersArray[numOfEnabledMembers].hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
                    numOfEnabledMembers++;
                }
                break;
            default:
                galtisOutput(outArgs, GT_NOT_IMPLEMENTED, "%d",-1);/* Error ???? */
                return CMD_OK;
        }


        if(numOfEnabledMembers != 0 || numOfDisabledMembers != 0)
        {
            /* this is non-empty trunk */
            break;
        }
    }

    if(gTrunkId > DXCH_NUM_TRUNKS_127_CNS &&
       numOfEnabledMembers == 0 &&
       numOfDisabledMembers == 0)
    {
        /* we done with the last trunk , or last trunk is empty */

        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
        return CMD_OK;
    }

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(tmpTrunkId);
    inFields[0] = tmpTrunkId;
    inFields[1] = numOfEnabledMembers;
    inFields[2] = numOfDisabledMembers;

    if(numOfEnabledMembers + numOfDisabledMembers > CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
    {
        /* CPSS error ??? */
        galtisOutput(outArgs, GT_BAD_VALUE, "%d",-1);/* no more trunks */
        return CMD_OK;
    }


    j = 3;

    for(i=0; i < numOfEnabledMembers; i++)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[i].hwDevice,
                             enabledMembersArray[i].port);
        inFields[j] = enabledMembersArray[i].port;
        inFields[j+1] = enabledMembersArray[i].hwDevice;
        j = j+2;
    }

    for(i=0; i < numOfDisabledMembers; i++)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[i].hwDevice,
                             disabledMembersArray[i].port);
        inFields[j] = disabledMembersArray[i].port;
        inFields[j+1] = disabledMembersArray[i].hwDevice;

        j = j+2;
    }

    /* fill "zero" for empty fields */
    for(/*j*/ ; j < 19 ; j+=2)
    {
        inFields[j] = 0;
        inFields[j+1] = 0;
    }

    /*Show max num of fields*/
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], inFields[5],  inFields[6],  inFields[7],
                inFields[8], inFields[9],  inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkDbEnabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the enabled members of the trunk
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All ExMx devices
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
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       enabledMembersArray - (array of) enabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
/*******************************************************************************
* cpssDxChTrunkDbDisabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the disabled members of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh devices
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
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       disabledMembersArray - (array of) disabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkMembersGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;/*reset on first*/

    return wrCpssDxChTrunkMembersGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxTrunkDbEnabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the enabled members of the trunk
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All ExMx devices
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
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       enabledMembersArray - (array of) enabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
/*******************************************************************************
* cpssDxChTrunkDbDisabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the disabled members of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh devices
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
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       disabledMembersArray - (array of) disabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkMembersGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTrunkMembersGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsAdd
*
* DESCRIPTION:
*       Function Relevant mode : Low Level mode
*
*       add the ports to the trunk's non-trunk entry .
*       NOTE : the ports are add to the "non trunk" table only and not effect
*              other trunk relate tables/registers.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to add to
*                             "non-trunk members"
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsAdd

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&nonTrunkPortsBmp);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    nonTrunkPortsBmp.ports[0] = (GT_U32)inArgs[2];
    nonTrunkPortsBmp.ports[1] = (GT_U32)inArgs[3];
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,nonTrunkPortsBmp);

    /* call cpss api function */
    result = cpssDxChTrunkNonTrunkPortsAdd(devNum, trunkId, &nonTrunkPortsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsRemove
*
* DESCRIPTION:
*       Function Relevant mode : Low Level mode
*
*      Removes the ports from the trunk's non-trunk entry .
*      NOTE : the ports are removed from the "non trunk" table only and not
*              effect other trunk relate tables/registers.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to remove from
*                             "non-trunk members"
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsRemove

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&nonTrunkPortsBmp);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    nonTrunkPortsBmp.ports[0] = (GT_U32)inArgs[2];
    nonTrunkPortsBmp.ports[1] = (GT_U32)inArgs[3];
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,nonTrunkPortsBmp);

    /* call cpss api function */
    result = cpssDxChTrunkNonTrunkPortsRemove(devNum, trunkId, &nonTrunkPortsBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkPortTrunkIdSet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*       memberOfTrunk - is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*
*       trunkId - the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 127 = The port is a member of the trunk
*                 this value relevant only when memberOfTrunk = GT_TRUE
*
* OUTPUTS:
*       none.
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
static CMD_STATUS wrCpssDxChTrunkPortTrunkIdSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_PHYSICAL_PORT_NUM        portId;
    GT_BOOL      memberOfTrunk;
    GT_TRUNK_ID  trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portId = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    memberOfTrunk = (GT_BOOL)inArgs[2];
    trunkId = (GT_TRUNK_ID)inArgs[3];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portId);

    /* call cpss api function */
    result = cpssDxChTrunkPortTrunkIdSet(devNum, portId, memberOfTrunk,
                                                               trunkId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkPortTrunkIdGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*
* OUTPUTS:
*       memberOfTrunkPtr - (pointer to) is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*       trunkIdPtr - (pointer to)the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 127 = The port is a member of the trunk
*                 this value relevant only when (*memberOfTrunkPtr) = GT_TRUE
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_BAD_STATE - the trunkId value is not synchronized in the 2 registers
*                      that should hold the same value
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkPortTrunkIdGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portId;
    GT_BOOL  memberOfTrunkPtr;
    GT_TRUNK_ID  trunkIdPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portId = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portId);

    /* call cpss api function */
    result = cpssDxChTrunkPortTrunkIdGet(devNum, portId, &memberOfTrunkPtr,
                                                               &trunkIdPtr);

    if(memberOfTrunkPtr == GT_TRUE)
    {
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(trunkIdPtr);
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", memberOfTrunkPtr, trunkIdPtr);
    return CMD_OK;
}



/***************************Table: cpssDxChTrunk********************/
static GT_U32                gIndGet;

/*******************************************************************************
* cpssDxChTrunkTableEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk table entry , and set the number of members in it.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*       numMembers - num of enabled members in the trunk
*                    Note : value 0 is not recommended.
*       membersArray - array of enabled members of the trunk
*
* OUTPUTS:
*       none.
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_OUT_OF_RANGE - numMembers exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkTableEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_TRUNK_MEMBER_STC member;
    GT_U8   devNum;
    GT_BOOL enable;
    GT_TRUNK_ID trunkId,tempTrunkId;
    CPSS_TRUNK_TYPE_ENT     trunkType;/* trunk type */
    CPSS_PORTS_BMP_STC      cascadePortsBmp;/* cascade ports bmp */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    member.port = (GT_U32)inFields[0];
    member.hwDevice = (GT_U32)inFields[1];
    CONVERT_DEV_PORT_DATA_MAC(member.hwDevice, member.port);

    enable = (GT_BOOL)inFields[2];

    result = cpssDxChTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    switch(trunkType)
    {
        case CPSS_TRUNK_TYPE_FREE_E:
        case CPSS_TRUNK_TYPE_REGULAR_E:
            if(enable == GT_TRUE)
            {
                result = cpssDxChTrunkDbIsMemberOfTrunk(devNum, &member, &tempTrunkId);

                if(result == GT_NOT_FOUND)/*It is not a member --> so add it */
                {
                   result = cpssDxChTrunkMemberAdd(devNum, trunkId, &member);
                   if(result != GT_OK)
                   {
                       /* pack output arguments to galtis string */
                       galtisOutput(outArgs, result, "");
                       return CMD_OK;
                   }
                   if(cmdAppTrunkMemberAdd != 0)
                   {
                       result = cmdAppTrunkMemberAdd(devNum, trunkId, &member);
                       if(result != GT_OK)
                       {
                           /* pack output arguments to galtis string */
                           galtisOutput(outArgs, result, "");
                           return CMD_OK;
                       }
                   }
                }
                else if(result == GT_OK)
                {
                    result = cpssDxChTrunkMemberEnable(devNum, trunkId, &member);
                    if(result != GT_OK)
                    {
                        /* pack output arguments to galtis string */
                        galtisOutput(outArgs, result, "");
                    }
                    if(cmdAppTrunkMemberEnable != 0)
                    {
                        result = cmdAppTrunkMemberEnable(devNum, trunkId, &member);
                        if(result != GT_OK)
                        {
                            /* pack output arguments to galtis string */
                            galtisOutput(outArgs, result, "");
                            return CMD_OK;
                        }
                    }
                }
            }
            else /*Disable*/
            {
                result = cpssDxChTrunkMemberDisable(devNum, trunkId, &member);
                if(result != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, result, "");
                }
                if(cmdAppTrunkMemberDisable != 0)
                {
                    result = cmdAppTrunkMemberDisable(devNum, trunkId,
                                                           &member);
                    if(result != GT_OK)
                    {
                        /* pack output arguments to galtis string */
                        galtisOutput(outArgs, result, "");
                        return CMD_OK;
                    }
                }
            }

            break;

        case CPSS_TRUNK_TYPE_CASCADE_E:
            if(enable != GT_TRUE)
            {
                galtisOutput(outArgs, GT_BAD_PARAM, "");/* not support 'Disable port in cascade trunk' */
                return CMD_OK;
            }

            if(member.hwDevice != PRV_CPSS_HW_DEV_NUM_MAC(devNum))
            {
                galtisOutput(outArgs, GT_BAD_PARAM, "");/* cascade trunk support only local ports */
                return CMD_OK;
            }

            if(member.port >= 64 )
            {
                galtisOutput(outArgs, GT_BAD_PARAM, "");/* cascade trunk support bmp of 64 ports */
                return CMD_OK;
            }

            result = cpssDxChTrunkCascadeTrunkPortsGet(devNum,trunkId,&cascadePortsBmp);
            if(result != GT_OK)
            {
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }

            /* add the port to BMP */
            CPSS_PORTS_BMP_PORT_SET_MAC(&cascadePortsBmp, member.port);

            result = cpssDxChTrunkCascadeTrunkPortsSet(devNum,trunkId,&cascadePortsBmp);
            if(result != GT_OK)
            {
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
            if(cmdAppTrunkCascadeTrunkPortsSet != 0)
            {
                result = cmdAppTrunkCascadeTrunkPortsSet(devNum, trunkId,
                                                         &cascadePortsBmp);
                if(result != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, result, "");
                    return CMD_OK;
                }
            }
            break;

        default:
            galtisOutput(outArgs, GT_NOT_IMPLEMENTED, "%d",-1);/* Error ???? */
            return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTrunkTableEntryGetEntry
*
* DESCRIPTION:
*       Get the next trunk member (enabled/disabled)
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*
* OUTPUTS:
*       member -  member of the trunk
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkTableEntryGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                 devNum;
    GT_TRUNK_ID           trunkId;
    GT_U32                numEnabledMembers;
    GT_U32                numDisabledMembers;
    CPSS_TRUNK_MEMBER_STC  membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    CPSS_TRUNK_TYPE_ENT     trunkType;/* trunk type */
    CPSS_PORTS_BMP_STC      cascadePortsBmp;/* cascade ports bmp */
    GT_U32          ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    result = cpssDxChTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    switch(trunkType)
    {
        case CPSS_TRUNK_TYPE_FREE_E:
        case CPSS_TRUNK_TYPE_REGULAR_E:
            numEnabledMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

            /* call cpss api function */
            result = cpssDxChTrunkDbEnabledMembersGet(devNum, trunkId, &numEnabledMembers,
                                                                     membersArray);

            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
            break;
        case CPSS_TRUNK_TYPE_CASCADE_E:
            numDisabledMembers = 0;/* no disabled ports in cascade trunk */
            numEnabledMembers = 0;/* initialize with 0 */

            result = cpssDxChTrunkCascadeTrunkPortsGet(devNum,trunkId,&cascadePortsBmp);
            if(result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d",-1);/* Error ???? */
                return CMD_OK;
            }

            for(ii = 0 ; ii < CPSS_MAX_PORTS_NUM_CNS; ii++)
            {
                if(numEnabledMembers == CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
                {
                    /* can't display more then the 8 members , even though
                       the cascade trunk may support more ... (Lion) */
                    break;
                }

                if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&cascadePortsBmp,ii))
                {
                    continue;
                }

                membersArray[numEnabledMembers].port = ii;
                membersArray[numEnabledMembers].hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
                numEnabledMembers++;
            }
            break;
        default:
            galtisOutput(outArgs, GT_NOT_IMPLEMENTED, "%d",-1);/* Error ???? */
            return CMD_OK;
    }

    if(gIndGet < numEnabledMembers)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[gIndGet].hwDevice,
                             membersArray[gIndGet].port);
        inFields[0] = membersArray[gIndGet].port;
        inFields[1] = membersArray[gIndGet].hwDevice;

        inFields[2] = GT_TRUE;
        /* we need to retrieve another enabled trunk member */
        /* pack and output table fields */
        fieldOutput("%d%d%d", inFields[0],  inFields[1],  inFields[2]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%f");
        return CMD_OK;
    }

    switch(trunkType)
    {
        case CPSS_TRUNK_TYPE_FREE_E:
        case CPSS_TRUNK_TYPE_REGULAR_E:
            numDisabledMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
            result = cpssDxChTrunkDbDisabledMembersGet(devNum, trunkId, &numDisabledMembers,
                                                                     membersArray);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
            break;
        default:
            numDisabledMembers = 0;
            break;
    }

    if(gIndGet < (numEnabledMembers+numDisabledMembers))
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[gIndGet-numEnabledMembers].hwDevice,
                             membersArray[gIndGet-numEnabledMembers].port);
        inFields[0] = membersArray[gIndGet-numEnabledMembers].port;
        inFields[1] = membersArray[gIndGet-numEnabledMembers].hwDevice;
        inFields[2] = GT_FALSE;
        /* we need to retrieve another disabled trunk member */
        /* pack and output table fields */
        fieldOutput("%d%d%d", inFields[0],  inFields[1],  inFields[2]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%f");
        return CMD_OK;
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

}

/*******************************************************************************
* cpssDxChTrunkTableEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS
*                      members
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkTableEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gIndGet = 0; /*reset on first*/

    return wrCpssDxChTrunkTableEntryGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkTableEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS
*                      members
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkTableEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gIndGet++;/* go to next index */

    return wrCpssDxChTrunkTableEntryGetEntry(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************
* cpssDxChTrunkMemberRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function remove member from a trunk in the device.
*       If member not exists in this trunk , function do nothing and
*       return GT_OK.
*
* APPLICABLE DEVICES:   All DxCh devices
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
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0xE0) != 0  means that the HW can't support
*                                              this value , since HW has 5 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkTableEntryDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;
    CPSS_TRUNK_TYPE_ENT     trunkType;/* trunk type */
    CPSS_PORTS_BMP_STC      cascadePortsBmp;/* cascade ports bmp */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    member.port = (GT_U32)inFields[0];
    member.hwDevice = (GT_U32)inFields[1];
    CONVERT_DEV_PORT_DATA_MAC(member.hwDevice, member.port);

    result = cpssDxChTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    switch(trunkType)
    {
        case CPSS_TRUNK_TYPE_FREE_E:
        case CPSS_TRUNK_TYPE_REGULAR_E:
            /* call cpss api function */
            result = cpssDxChTrunkMemberRemove(devNum, trunkId, &member);
            if(result != GT_OK)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
            if(cmdAppTrunkMemberRemove != 0)
            {
                result = cmdAppTrunkMemberRemove(devNum, trunkId, &member);
                if(result != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, result, "");
                    return CMD_OK;
                }
            }
            break;
        case CPSS_TRUNK_TYPE_CASCADE_E:
            if(member.hwDevice != PRV_CPSS_HW_DEV_NUM_MAC(devNum))
            {
                galtisOutput(outArgs, GT_BAD_PARAM, "");/* cascade trunk support only local ports */
                return CMD_OK;
            }

            if(member.port >= 64 )
            {
                galtisOutput(outArgs, GT_BAD_PARAM, "");/* cascade trunk support bmp of 64 ports */
                return CMD_OK;
            }

            result = cpssDxChTrunkCascadeTrunkPortsGet(devNum,trunkId,&cascadePortsBmp);
            if(result != GT_OK)
            {
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }

            /* remove the port from BMP */
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(&cascadePortsBmp, member.port);

            result = cpssDxChTrunkCascadeTrunkPortsSet(devNum,trunkId,&cascadePortsBmp);
            if(result != GT_OK)
            {
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
            if(cmdAppTrunkCascadeTrunkPortsSet != 0)
            {
                result = cmdAppTrunkCascadeTrunkPortsSet(devNum, trunkId,
                                                         &cascadePortsBmp);
                if(result != GT_OK)
                {
                    /* pack output arguments to galtis string */
                    galtisOutput(outArgs, result, "");
                    return CMD_OK;
                }
            }
            break;

        default:
            galtisOutput(outArgs, GT_NOT_IMPLEMENTED, "%d",-1);/* Error ???? */
            return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*********************Table cpssDxChTrunkNonTrunkPorts***************/

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk's non-trunk ports specific bitmap entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&nonTrunkPorts);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);


    /* first get the bmp of 2 words --> support Lion */
    result = cpssDxChTrunkNonTrunkPortsEntryGet(devNum, trunkId,
                                                &nonTrunkPorts);

    nonTrunkPorts.ports[0] = (GT_U32)inFields[1];


    /* call cpss api function */
    result = cpssDxChTrunkNonTrunkPortsEntrySet(devNum, trunkId,
                                                &nonTrunkPorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk's non-trunk ports bitmap specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*
* OUTPUTS:
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrTrunkNonTrunkPortsEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 numOfWordsForPortsBmp
)
{
    GT_STATUS result;
    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;
    GT_TRUNK_ID     tmpTrunkId;
    GT_U32  ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(gTrunkId >= PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw)/* No more trunks */
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    tmpTrunkId = gTrunkId;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(tmpTrunkId);

    /* call cpss api function */
    result = cpssDxChTrunkNonTrunkPortsEntryGet(devNum, tmpTrunkId,
                                                &nonTrunkPorts);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);/* error */
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,nonTrunkPorts);
    inFields[0] = tmpTrunkId;
    inFields[1] = nonTrunkPorts.ports[0];
    inFields[2] = nonTrunkPorts.ports[1];

    gTrunkId++;

    for(ii = 0 ; (ii < numOfWordsForPortsBmp) &&  (ii < CPSS_MAX_PORTS_BMP_NUM_CNS); ii++)
    {
        inFields[ii+1] = nonTrunkPorts.ports[ii];
    }

    if(numOfWordsForPortsBmp == 4) /*128 ports */
    {
        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],inFields[2] , inFields[3],inFields[4]);
    }
    else if(numOfWordsForPortsBmp == 2) /*64 ports */
    {
        /* pack and output table fields */
        fieldOutput("%d%d%d", inFields[0], inFields[1],inFields[2]);
    }
    else/*32 ports */
    {
        /* pack and output table fields */
        fieldOutput("%d%d", inFields[0], inFields[1]);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk's non-trunk ports bitmap specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*
* OUTPUTS:
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntryGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;

    return wrTrunkNonTrunkPortsEntryGet(inArgs,inFields,numFields,outArgs,1);
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrTrunkNonTrunkPortsEntryGet(inArgs,inFields,numFields,outArgs,1);
}

/*********************Table cpssDxChTrunkNonTrunkPorts1***************/

/*******************************************************************************
* cpssDxChTrunkNonTrunkPorts1EntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk's non-trunk ports specific bitmap entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS internal_TrunkNonTrunkPortsEntry1Set

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&nonTrunkPorts);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    nonTrunkPorts.ports[0] = (GT_U32)inFields[1];
    nonTrunkPorts.ports[1] = (GT_U32)inFields[2];
    if(numFields >= 5)
    {
        nonTrunkPorts.ports[2] = (GT_U32)inFields[3];
        nonTrunkPorts.ports[3] = (GT_U32)inFields[4];
    }
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,nonTrunkPorts);

    /* call cpss api function */
    result = cpssDxChTrunkNonTrunkPortsEntrySet(devNum, trunkId,
                                                &nonTrunkPorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntry1Set

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_TrunkNonTrunkPortsEntry1Set(inArgs,inFields,numFields,outArgs);
}

static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntry128Set

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_TrunkNonTrunkPortsEntry1Set(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkNonTrunkPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk's non-trunk ports bitmap specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
*
* OUTPUTS:
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntry1GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;

    return wrTrunkNonTrunkPortsEntryGet(inArgs,inFields,numFields,outArgs,2);
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntry1GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrTrunkNonTrunkPortsEntryGet(inArgs,inFields,numFields,outArgs,2);
}

static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntry128GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;

    return wrTrunkNonTrunkPortsEntryGet(inArgs,inFields,numFields,outArgs,4);
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkNonTrunkPortsEntry128GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrTrunkNonTrunkPortsEntryGet(inArgs,inFields,numFields,outArgs,4);
}

/*************Table: cpssDxChTrunkDesignatedPorts**************/
static GT_U32  gEntryIndex;
/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&designatedPorts);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryIndex = (GT_U32)inFields[0];

    /* first get the bmp of 2 words --> support Lion */
    (void) cpssDxChTrunkDesignatedPortsEntryGet(devNum, entryIndex,
                                                  &designatedPorts);

    designatedPorts.ports[0] = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntrySet(devNum, entryIndex,
                                                  &designatedPorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/* print the trunk designated table */
GT_STATUS printDxChTrunkDesignatedTable(IN GT_U32   dev)
{
    GT_STATUS   rc;
    GT_U8   devNum = (GT_U8)dev;
    GT_U32      ii;
    CPSS_PORTS_BMP_STC  designatedPorts;
    GT_U32 numOfEntries;

    numOfEntries = (0 != PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) ?
                        PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw :
                        0;

    cmdOsPrintf("TrunkDesignatedTable \n");
    cmdOsPrintf("index  ports[0]   ports[1]   ports[2]   ports[3]\n");
    cmdOsPrintf("=============================================== \n");
    for(ii = 0 ; ii < numOfEntries ; ii++)
    {
        rc = cpssDxChTrunkDesignatedPortsEntryGet(devNum,ii,&designatedPorts);
        if(rc != GT_OK)
        {
            return rc;
        }

        cmdOsPrintf("%d     0x%8.8x     0x%8.8x     0x%8.8x     0x%8.8x \n" , ii,
                    designatedPorts.ports[0],
                    designatedPorts.ports[1],
                    designatedPorts.ports[2],
                    designatedPorts.ports[3]
                    );
    }

    return GT_OK;
}

/* print the trunk 'non-trunk ports' table */
GT_STATUS printDxChTrunkNonTrunkPortsTable(IN GT_U32   dev)
{
    GT_STATUS   rc;
    GT_U8   devNum = (GT_U8)dev;
    GT_TRUNK_ID      trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;

    cmdOsPrintf("TrunkNonTrunkTable \n");
    cmdOsPrintf("trunkId  ports[0]   ports[1]  ports[2]   ports[3] \n");
    cmdOsPrintf("================================================= \n");
    for(trunkId = 0 ; trunkId < PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw ; trunkId++)
    {
        rc = cpssDxChTrunkNonTrunkPortsEntryGet(devNum,trunkId,&nonTrunkPorts);
        if(rc != GT_OK)
        {
            return rc;
        }

        cmdOsPrintf("%d     0x%8.8x     0x%8.8x     0x%8.8x     0x%8.8x \n" , trunkId,
                    nonTrunkPorts.ports[0],
                    nonTrunkPorts.ports[1],
                    nonTrunkPorts.ports[2],
                    nonTrunkPorts.ports[3]
                    );
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntryGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  designatedPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gEntryIndex = 0;  /*reset on first*/

    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntryGet(devNum, gEntryIndex,
                                                  &designatedPorts);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gEntryIndex;
    inFields[1] = designatedPorts.ports[0];

    gEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntryGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  designatedPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntryGet(devNum, gEntryIndex,
                                                  &designatedPorts);

    if (result != GT_OK)
    {
        GT_U32 numOfEntries;

        numOfEntries = (0 != PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) ?
                            PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw :
                            0;

        if(gEntryIndex >= numOfEntries)
        {
            /*table ended */
            result = GT_OK;
        }

        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gEntryIndex;
    inFields[1] = designatedPorts.ports[0];

    gEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*************Table: cpssDxChTrunkDesignatedPorts1**************/

/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntry1Set

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&designatedPorts);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryIndex = (GT_U32)inFields[0];
    designatedPorts.ports[0] = (GT_U32)inFields[1];
    designatedPorts.ports[1] = (GT_U32)inFields[2];
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,designatedPorts);

    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntrySet(devNum, entryIndex,
                                                  &designatedPorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkDesignatedPorts1EntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntry1GetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  designatedPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gEntryIndex = 0;  /*reset on first*/

    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntryGet(devNum, gEntryIndex,
                                                  &designatedPorts);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,designatedPorts);
    inFields[0] = gEntryIndex;
    inFields[1] = designatedPorts.ports[0];
    inFields[2] = designatedPorts.ports[1];

    gEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1],inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkDesignatedPorts1EntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntry1GetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  designatedPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntryGet(devNum, gEntryIndex,
                                                  &designatedPorts);

    if (result != GT_OK)
    {
        GT_U32 numOfEntries;

        numOfEntries = (0 != PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) ?
                            PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw :
                            0;

        if(gEntryIndex >= numOfEntries)
        {
            /*table ended */
            result = GT_OK;
        }

        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,designatedPorts);
    inFields[0] = gEntryIndex;
    inFields[1] = designatedPorts.ports[0];
    inFields[2] = designatedPorts.ports[1];

    gEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1],inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*************Table: cpssDxChTrunkDesignatedPorts2**************/

/*******************************************************************************
* cpssDxChTrunkDesignatedPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntry2Set

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&designatedPorts);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryIndex = (GT_U32)inFields[0];
    designatedPorts.ports[0] = (GT_U32)inFields[1];
    designatedPorts.ports[1] = (GT_U32)inFields[2];
    designatedPorts.ports[2] = (GT_U32)inFields[3];
    designatedPorts.ports[3] = (GT_U32)inFields[4];
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,designatedPorts);

    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntrySet(devNum, entryIndex,
                                                  &designatedPorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}





/*******************************************************************************
* cpssDxChTrunkDesignatedPorts1EntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntryGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  designatedPorts;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    /* call cpss api function */
    result = cpssDxChTrunkDesignatedPortsEntryGet(devNum, gEntryIndex,
                                                  &designatedPorts);

    if (result != GT_OK)
    {
        GT_U32 numOfEntries;

        numOfEntries = (0 != PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) ?
                            PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numOfDesignatedTrunkEntriesHw :
                            0;

        if(gEntryIndex >= numOfEntries)
        {
            /*table ended */
            result = GT_OK;
        }

        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,designatedPorts);
    inFields[0] = gEntryIndex;
    inFields[1] = designatedPorts.ports[0];
    inFields[2] = designatedPorts.ports[1];
    inFields[3] = designatedPorts.ports[2];
    inFields[4] = designatedPorts.ports[3];

    gEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],inFields[2], inFields[3], inFields[4]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChTrunkDesignatedPorts2EntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntry2GetFirst

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

    gEntryIndex = 0;  /*reset on first*/

    return wrCpssDxChTrunkDesignatedPortsEntryGet(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* cpssDxChTrunkDesignatedPorts2EntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDesignatedPortsEntry2GetNext

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

    return wrCpssDxChTrunkDesignatedPortsEntryGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is trunk member the function retrieve the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All DxCh devices
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
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number
*       GT_NOT_FOUND - the pair {devNum,port} not a trunk member
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDbIsMemberOfTrunk

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  devNum;
    CPSS_TRUNK_MEMBER_STC  memberPtr;
    GT_TRUNK_ID            trunkIdPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memberPtr.port = (GT_U32)inArgs[1];
    memberPtr.hwDevice = (GT_U32)inArgs[2];
    CONVERT_DEV_PORT_DATA_MAC(memberPtr.hwDevice, memberPtr.port);

    /* call cpss api function */
    result = cpssDxChTrunkDbIsMemberOfTrunk(devNum, &memberPtr, &trunkIdPtr);

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(trunkIdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", trunkIdPtr);
    return CMD_OK;
}

static CMD_STATUS wrTrunkHashDesignatedTableModeSet
(
    IN  GT_U8   devNum,
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT    mode;


    if(designatedTableModeInfo[devNum].useEntireTable == GT_FALSE)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_NOT_SUPPORTED, "");
        return CMD_OK;
    }
    else
    {
        if(designatedTableModeInfo[devNum].useVid == GT_TRUE)
        {
            mode = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E;
        }
        else
        {
            mode = CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E;
        }
    }


    /* call cpss api function */
    rc = cpssDxChTrunkHashDesignatedTableModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device using the entire designated port trunk table
*       or to always use only the first entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*       useEntireTable -
*                       GT_TRUE:
*                           The designated hashing uses the entire table.
*                           (one of 8 entries)
*                       GT_FALSE:
*                           The designated hashing use only the first entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashDesignatedTableModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8    devNum;
    GT_BOOL  useEntireTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    useEntireTable = (GT_BOOL)inArgs[1];

    if((devNum) >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        galtisOutput(outArgs, GT_FAIL, "");
        return CMD_OK;
    }

    if(designatedTableModeInfo[devNum].valid == GT_FALSE)
    {
        designatedTableModeInfo[devNum].valid = GT_TRUE;
        /* defaults of ch1,2,3,xcat */
        designatedTableModeInfo[devNum].useVid = GT_TRUE;
        designatedTableModeInfo[devNum].useEntireTable = GT_TRUE;
    }

    /*save to DB*/
    designatedTableModeInfo[devNum].useEntireTable = useEntireTable;

    return wrTrunkHashDesignatedTableModeSet(devNum,inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the Enable/Disable the device using the entire designated port trunk
*       table or to always use only the first entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       useEntireTablePtr - (pointer to)
*                       GT_TRUE:
*                           The designated hashing uses the entire table.
*                           (one of 8 entries)
*                       GT_FALSE:
*                           The designated hashing use only the first entry
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashDesignatedTableModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BOOL     useEntireTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    useEntireTable = GT_TRUE;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", useEntireTable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device using the entire designated port trunk table
*       or to always use only the first entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*       useEntireTable -
*                       GT_TRUE:
*                           The designated hashing uses the entire table.
*                           (one of 8 entries)
*                       GT_FALSE:
*                           The designated hashing use only the first entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashDesignatedTableModeSet1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   rc;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT    mode;
    GT_U8    devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChTrunkHashDesignatedTableModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashDesignatedTableModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the Enable/Disable the device using the entire designated port trunk
*       table or to always use only the first entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       useEntireTablePtr - (pointer to)
*                       GT_TRUE:
*                           The designated hashing uses the entire table.
*                           (one of 8 entries)
*                       GT_FALSE:
*                           The designated hashing use only the first entry
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashDesignatedTableModeGet1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8    devNum;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashDesignatedTableModeGet(devNum,&mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashGlobalModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum  - the device number.
*       hashMode   - hash mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashGlobalModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashGlobalModeSet(devNum, hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashGlobalModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr   - (pointer to)hash mode
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashGlobalModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  hashModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashGlobalModeGet(devNum, &hashModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hashModePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpAddMacModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the use of mac address bits to trunk hash calculation when packet is
*       IP and the "Ip trunk hash mode enabled".
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum  - the device number.
*       enable  - enable/disable feature
*                   GT_FALSE - If the packet is an IP packet MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added to the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0].
*
*                             NOTE: When the packet is not an IP packet and
*                             <TrunkLBH Mode> = 0, the trunk load balancing
*                             hash = MACTrunkHash, regardless of this setting.
*                             If the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpAddMacModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    GT_BOOL                              enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpAddMacModeSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpAddMacModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the use of mac address bits to trunk hash calculation when packet is
*       IP and the "Ip trunk hash mode enabled".
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)enable/disable feature
*                   GT_FALSE - If the packet is an IP packet MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added to the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0]^MAC_DA[5:0].
*
*                             NOTE: When the packet is not an IP packet and
*                             <TrunkLBH Mode> = 0, the trunk load balancing
*                             hash = MACTrunkHash, regardless of this setting.
*                             If the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpAddMacModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    GT_BOOL                              enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpAddMacModeGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the IP SIP/DIP information,
*       when calculation the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssDxChTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    GT_BOOL                              enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpModeSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the enable/disable of device from considering the IP SIP/DIP
*       information, when calculation the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*       3. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr -(pointer to)
*               GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssDxChTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    GT_BOOL                              enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpModeGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashL4ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       3. Not relevant to multi-destination packets (include routed IPM).
*       4. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*       hashMode - L4 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashL4ModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT      hashMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashL4ModeSet(devNum, hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashL4ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the Enable/Disable of device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       3. Not relevant to multi-destination packets (include routed IPM).
*       4. Not relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_SRC_PORT_E)
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)L4 hash mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashL4ModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT      hashModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashL4ModeGet(devNum, &hashModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hashModePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpv6ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum     - the device number.
*       hashMode   - the Ipv6 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpv6ModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   hashMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpv6ModeSet(devNum, hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpv6ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum     - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)the Ipv6 hash mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpv6ModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT   hashModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpv6ModeGet(devNum, &hashModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hashModePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashVidMultiDestinationModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       The VID assigned to the packet may be added as part of the distribution
*       hash for accessing the Designated Trunk Port Entry<n> Table to select a
*       designated trunk member for Multi-Destination packets.
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum  - the device number.
*       enable  - GT_FALSE - Don't use the VID. Distribution hash is based on
*                            the packet header or Ingress port only.
*                 GT_TRUE  - In addition to the packet header, the distribution
*                            hash is based on the VID assigned to the packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashVidMultiDestinationModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                devNum;
    GT_BOOL                              enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    if((devNum) >= PRV_CPSS_MAX_PP_DEVICES_CNS)
    {
        galtisOutput(outArgs, GT_FAIL, "");
        return CMD_OK;
    }

    if(designatedTableModeInfo[devNum].valid == GT_FALSE)
    {
        designatedTableModeInfo[devNum].valid = GT_TRUE;
        /* defaults of ch1,2,3,xcat */
        designatedTableModeInfo[devNum].useVid = GT_TRUE;
        designatedTableModeInfo[devNum].useEntireTable = GT_TRUE;
    }

    /*save to DB*/
    designatedTableModeInfo[devNum].useVid = enable;

    return wrTrunkHashDesignatedTableModeSet(devNum,inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkHashVidMultiDestinationModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get The VID assigned to the packet may be added as part of the
*       distribution hash for accessing the Designated Trunk Port Entry<n> Table
*       to select a designated trunk member for Multi-Destination packets.
*
* APPLICABLE DEVICES:   All DXCH devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                 GT_FALSE - Don't use the VID. Distribution hash is based on
*                            the packet header or Ingress port only.
*                 GT_TRUE  - In addition to the packet header, the distribution
*                            hash is based on the VID assigned to the packet.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashVidMultiDestinationModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    GT_BOOL                              enable = GT_FALSE;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashDesignatedTableModeGet(devNum,
                                         &mode);

    if(result == GT_OK)
    {
        if(mode == CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E)
        {
            enable = GT_TRUE;
        }
        else
        {
            enable = GT_FALSE;
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashMplsModeEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the MPLS information,
*       when calculating the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON-MPLS packets.
*       2. Relevant when cpssDxChTrunkHashGlobalModeSet(devNum,
*                       CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - MPLS parameter are not used in trunk hash index
*                GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashMplsModeEnableSet
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
    result = cpssDxChTrunkHashMplsModeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashMplsModeEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get trunk MPLS hash mode
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr - (pointer to)the MPLS hash mode.
*               GT_FALSE - MPLS parameter are not used in trunk hash index
*               GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashMplsModeEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     dev;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashMplsModeEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashMaskSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       maskedField - field to apply the mask on
*       maskValue - The mask value to be used (0..0x3F)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or maskedField
*       GT_OUT_OF_RANGE          - maskValue > 0x3F
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashMaskSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;
    GT_U8                          devNum;
    CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField;
    GT_U8                          maskValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    maskedField = (CPSS_DXCH_TRUNK_LBH_MASK_ENT)inArgs[1];
    maskValue   = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTrunkHashMaskSet(devNum, maskedField, maskValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashMaskGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       maskedField - field to apply the mask on
*
* OUTPUTS:
*       maskValuePtr - (pointer to)The mask value to be used (0..0x3F)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or maskedField
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashMaskGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;
    GT_U8                          devNum;
    CPSS_DXCH_TRUNK_LBH_MASK_ENT   maskedField;
    GT_U8                          maskValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    maskedField = (CPSS_DXCH_TRUNK_LBH_MASK_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashMaskGet(devNum, maskedField, &maskValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maskValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpShiftSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*       shiftValue    - The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or protocolStack
*       GT_OUT_OF_RANGE - shiftValue > 3 for IPv4 , shiftValue > 15 for IPv6
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpShiftSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    CPSS_IP_PROTOCOL_STACK_ENT   protocolStack;
    GT_BOOL                      isSrcIp;
    GT_U32                       shiftValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    isSrcIp       = (GT_BOOL)inArgs[2];
    shiftValue    = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpShiftSet(
        devNum, protocolStack, isSrcIp, shiftValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkHashIpShiftGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:   DxChXCat and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*
* OUTPUTS:
*       shiftValuePtr    - (pointer to) The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or protocolStack
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIpShiftGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    CPSS_IP_PROTOCOL_STACK_ENT   protocolStack;
    GT_BOOL                      isSrcIp;
    GT_U32                       shiftValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    isSrcIp       = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTrunkHashIpShiftGet(
        devNum, protocolStack, isSrcIp, &shiftValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", shiftValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssGenericTrunkDumpDb
*
* DESCRIPTION:
*       debug tool --- print all the fields that relate to trunk -- from DB only
*
* INPUTS:
*       devNum - device number
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success,
*       GT_FAIL          - otherwise.
*       GT_BAD_PARAM     - on bad parameters,
*
* COMMENTS:
*
*
*
*
*******************************************************************************/
GT_STATUS cpssGenericTrunkDumpDb
(
    IN    GT_U8 devNum
)
{
    GT_TRUNK_ID     trunkId;
    GT_U32          ii;
    GT_U8           portNum;
    GT_BOOL         enable;
    GT_U32          numMembers;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    CPSS_TRUNK_TYPE_ENT     trunkType;/*trunk type */

    cpssOsPrintf(" cpssGenericTrunkDumpDb - Start \n");

    /* loop on all trunks -- without trunkId = 0 */
    trunkId = 0;
    while(++trunkId)
    {
        if(GT_OK != prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType))
        {
            break;
        }

        if(trunkType == CPSS_TRUNK_TYPE_CASCADE_E)
        {
            /* designated member is not for cascade trunks */
            continue;
        }

        if(GT_OK != prvCpssGenericTrunkDbDesignatedMemberGet(devNum,trunkId,&enable,&membersArray[0]))
        {
            continue;
        }

        if(enable == GT_TRUE)
        {
            cpssOsPrintf("trunk[%d] with designated member(%d,%d) \n",
                trunkId,membersArray[0].port,membersArray[0].hwDevice);
        }

    }

    /* loop on all trunks -- without trunkId = 0 */
    trunkId = 0;
    while(++trunkId)
    {
        if(GT_OK != prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType))
        {
            break;
        }

        if(trunkType == CPSS_TRUNK_TYPE_CASCADE_E)
        {
            /* 'enabled/disabled members get' is not for cascade trunks */
            continue;
        }

        numMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        if(GT_OK != prvCpssGenericTrunkDbEnabledMembersGet(devNum,trunkId,
                &numMembers,&membersArray[0]))
        {
            break;
        }

        if(numMembers)
        {
            cpssOsPrintf("trunk[%d] with enabled members: \n",trunkId);

            for(ii = 0 ; ii < numMembers; ii++)
            {
                cpssOsPrintf("(%d,%d) ",
                    membersArray[ii].port,membersArray[ii].hwDevice);
            }
            cpssOsPrintf("\n");
        }

        numMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;

        if(GT_OK != prvCpssGenericTrunkDbDisabledMembersGet(devNum,trunkId,
                &numMembers,&membersArray[0]))
        {
            break;
        }

        if(numMembers)
        {
            cpssOsPrintf("trunk[%d] with disabled members: \n",trunkId);

            for(ii = 0 ; ii < numMembers; ii++)
            {
                cpssOsPrintf("(%d,%d) ",
                    membersArray[ii].port,membersArray[ii].hwDevice);
            }
            cpssOsPrintf("\n");
        }
    }


    for (portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        membersArray[0].hwDevice = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
        membersArray[0].port = portNum;

        if(GT_OK != prvCpssGenericTrunkDbIsMemberOfTrunk(devNum,&membersArray[0],&trunkId))
        {
            /* not trunk member */
            continue;
        }

        cpssOsPrintf("port[%d] - TrunkGroupId [%d] ",portNum,trunkId);

        if(GT_OK == prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType))
        {
            if(trunkType == CPSS_TRUNK_TYPE_CASCADE_E)
            {
                cpssOsPrintf(" --cascade trunk--");
            }
        }

        cpssOsPrintf("\n");
    }

    cpssOsPrintf("\n");

    cpssOsPrintf(" cpssGenericTrunkDumpDb - End \n");

    return GT_OK;
}

/* debug function to set the debugDesignatedMembers flag */
GT_STATUS cpssDxChTrunkDebugDesignatedMembersEnable(IN GT_U32 enable)
{
    debugDesignatedMembers = enable;
    return GT_OK;
}

/* debug function to set the debugDesignatedMembers flag */
GT_STATUS cpssDxChTrunkDebugDesignatedMembersSet(
    IN GT_U8 devNum,
    IN GT_U32 index,        /* designated table index */
    IN GT_U16 trunkId,      /* the trunk that the port is member of */
    IN GT_U32 portNum    /* the port number */
)
{
    GT_STATUS   rc;
    CPSS_PORTS_BMP_STC portsBmp,tmpPortsBmp1;

    cpssOsPrintf("devNum[%d],index[%d],trunkId[%d],portNum[%d] \n",devNum,index,trunkId,portNum);

    if(portNum >= 64)
    {
        return GT_BAD_PARAM;
    }

    rc = cpssDxChTrunkNonTrunkPortsEntryGet(devNum,trunkId,&portsBmp);
    if(rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChTrunkDesignatedPortsEntryGet(devNum,index,&tmpPortsBmp1);
    if(rc != GT_OK)
    {
        return rc;
    }


    if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&portsBmp,portNum))
    {
        cpssOsPrintf("port not in trunk \n");
        return GT_BAD_PARAM;
    }

    /* remove the trunk ports from the designated entry */
    CPSS_PORTS_BMP_BITWISE_AND_MAC(&tmpPortsBmp1,&tmpPortsBmp1,&portsBmp);

    CPSS_PORTS_BMP_PORT_SET_MAC(&tmpPortsBmp1,portNum);

    /* update*/
    rc = cpssDxChTrunkDesignatedPortsEntrySet(devNum,index,&tmpPortsBmp1);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* cpssDxChTrunkDump
*
* DESCRIPTION:
*       debug tool --- print all the fields that relate to trunk
*
* INPUTS:
*       devNum - device number
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success,
*       GT_FAIL          - otherwise.
*       GT_BAD_PARAM     - on bad parameters,
*
* COMMENTS:
*
*
*
*
*******************************************************************************/
GT_STATUS cpssDxChTrunkDump
(
    IN    GT_U8 devNum
)
{
    GT_STATUS       rc;  /* return error core */
    CPSS_DXCH_TRUNK_L4_LBH_MODE_ENT l4HashMode;
    CPSS_DXCH_TRUNK_IPV6_HASH_MODE_ENT ipv6HashMode;
    CPSS_DXCH_TRUNK_LBH_GLOBAL_MODE_ENT  generalHashMode;
    CPSS_DXCH_TRUNK_DESIGNATED_TABLE_MODE_ENT designatedMode;/* designated table mode */
    GT_BOOL         l3HashEn;
    GT_BOOL         l3AddMacHashEn;
    CPSS_PORTS_BMP_STC portsBmp;
    CPSS_PORTS_BMP_STC tmpPortsBmp,tmpPortsBmp1;
    GT_TRUNK_ID     trunkId;
    GT_U32          ii,jj;
    GT_U32          portNum;
    GT_BOOL         isTrunkMember;
    GT_U32          numMembers;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_BOOL         isEmpty;
    CPSS_TRUNK_TYPE_ENT     trunkType;/*trunk type */
    GT_U32  error;
    GT_U32  count;
    GT_U32          numOfTrunks;/* number of trunks */
    GT_U32  numOfPorts;/*number of physical ports */

    cpssOsPrintf(" cpssDxChTrunkDump - Start \n");

    /* print global device parameters */
    cpssOsPrintf("print global device parameters \n");

    rc = cpssDxChTrunkHashL4ModeGet(devNum,&l4HashMode);
    CHECK_RC(rc);

    rc = cpssDxChTrunkHashIpv6ModeGet(devNum,&ipv6HashMode);
    CHECK_RC(rc);

    rc = cpssDxChTrunkHashGlobalModeGet(devNum,&generalHashMode);
    CHECK_RC(rc);

    rc = cpssDxChTrunkHashIpModeGet(devNum,&l3HashEn);
    CHECK_RC(rc);

    rc = cpssDxChTrunkHashIpAddMacModeGet(devNum,&l3AddMacHashEn);
    CHECK_RC(rc);

    rc = cpssDxChTrunkHashDesignatedTableModeGet(devNum,&designatedMode);
    CHECK_RC(rc);

    cpssOsPrintf("L4LongTrunkHash [%d] , IPv6TrunkHashMode[%d] "
                "(global)TrunkHashMode[%s] , EnL4Hash[%d] "
                "EnIPHash[%d] , AddMACHash[%d] \n"
                "designatedMode[%s] \n",
                l4HashMode == CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E ? 0 :
                l4HashMode == CPSS_DXCH_TRUNK_L4_LBH_LONG_E     ? 1 :
                0,
                ipv6HashMode,
                generalHashMode == CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_E ? "LBH_PACKETS_INFO" :
                generalHashMode == CPSS_DXCH_TRUNK_LBH_INGRESS_PORT_E ? "LBH_INGRESS_PORT" :
                generalHashMode == CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E ? "LBH_PACKETS_INFO_CRC" :
                    "unknown mode",
                l4HashMode == CPSS_DXCH_TRUNK_L4_LBH_DISABLED_E ? 0 : 1,
                l3HashEn,
                l3AddMacHashEn,
                (
                 designatedMode == CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_E           ? "USE_INGRESS_HASH" :
                 designatedMode == CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_INGRESS_HASH_AND_VID_E   ? "USE_INGRESS_HASH_AND_VID" :
                 designatedMode == CPSS_DXCH_TRUNK_DESIGNATED_TABLE_USE_SOURCE_INFO_E            ? "USE_SOURCE_INFO":
                 "unknown mode")
               );
    cpssOsPrintf("\n");

    /* Trunk<n> Non-Trunk Members Table (0<=n<128) */
    cpssOsPrintf("Trunk<n> Non-Trunk Members Table (0<=n<128) \n");

    trunkId = 0;

    isEmpty = GT_TRUE;

    numOfTrunks = PRV_CPSS_DEV_TRUNK_INFO_MAC(devNum)->numTrunksSupportedHw;


    while(trunkId < numOfTrunks)
    {
        if(GT_OK != cpssDxChTrunkNonTrunkPortsEntryGet(devNum,trunkId,&portsBmp))
        {
            trunkId++;
            continue;
        }

        if(0 == ARE_ALL_EXISTING_PORTS_MAC(devNum,&portsBmp))
        {
            /* print only trunks that not all existing ports are in the BMP of the 'non-trunk' */

            cpssOsPrintf("trunk [%d]  : ",trunkId);
            cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[0]);
            if(portsBmp.ports[1] || portsBmp.ports[2] || portsBmp.ports[3] ||
               portsBmp.ports[4] || portsBmp.ports[5] || portsBmp.ports[6] ||
               portsBmp.ports[7])
            {
                cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[1]);

                if(/*portsBmp.ports[1] ||*/ portsBmp.ports[2] || portsBmp.ports[3] ||
                   portsBmp.ports[4] || portsBmp.ports[5] || portsBmp.ports[6] ||
                   portsBmp.ports[7])
                {
                    cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[2]);

                    if(/*portsBmp.ports[1] || portsBmp.ports[2] || */portsBmp.ports[3] ||
                       portsBmp.ports[4] || portsBmp.ports[5] || portsBmp.ports[6] ||
                       portsBmp.ports[7])
                    {
                        cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[3]);

                        if(/*portsBmp.ports[1] || portsBmp.ports[2] || portsBmp.ports[3] ||*/
                           portsBmp.ports[4] || portsBmp.ports[5] || portsBmp.ports[6] ||
                           portsBmp.ports[7])
                        {
                            cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[4]);

                            if(/*portsBmp.ports[1] || portsBmp.ports[2] || portsBmp.ports[3] ||
                               portsBmp.ports[4] ||*/ portsBmp.ports[5] || portsBmp.ports[6] ||
                               portsBmp.ports[7])
                            {
                                cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[5]);

                                if(/*portsBmp.ports[1] || portsBmp.ports[2] || portsBmp.ports[3] ||
                                   portsBmp.ports[4] || portsBmp.ports[5] ||*/ portsBmp.ports[6] ||
                                   portsBmp.ports[7])
                                {
                                    cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[6]);

                                    if(/*portsBmp.ports[1] || portsBmp.ports[2] || portsBmp.ports[3] ||
                                       portsBmp.ports[4] || portsBmp.ports[5] || portsBmp.ports[6] ||*/
                                       portsBmp.ports[7])
                                    {
                                        cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[7]);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            rc = prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType);
            if(rc != GT_OK)
            {
                cpssOsPrintf("prvCpssGenericTrunkDbTrunkTypeGet: FAIL [%d] on trunk[%d]",rc,trunkId);
            }
            else
            {
                if(trunkType == CPSS_TRUNK_TYPE_CASCADE_E)
                {
                    cpssOsPrintf(" --cascade trunk--");
                }
            }

            isEmpty = GT_FALSE;

            /* do simple diagnostic --> the trunk members must not have more
               then single appearance in the designated table in each entry
               --> allowed only 0 or 1 port from the trunk in each entry */

            ii = 0;
            while(1)
            {
                if(GT_OK != cpssDxChTrunkDesignatedPortsEntryGet(devNum,ii,&tmpPortsBmp1))
                {
                    break;
                }

                error = 0;

                if(0 == ARE_ALL_EXISTING_PORTS_MAC(devNum,&tmpPortsBmp1))
                {
                    CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&tmpPortsBmp,&tmpPortsBmp1,&portsBmp);

                    count = 0;
                    if(debugDesignatedMembers)
                    {
                        cpssOsPrintf("trunk[%d] index [%d]",trunkId,ii);
                    }

                    for(jj = 0 ; jj < 128 ; jj++)
                    {
                        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&tmpPortsBmp,jj))
                        {
                            if(debugDesignatedMembers)
                            {
                                cpssOsPrintf("[%d]",jj);
                            }
                            count++;
                        }
                    }

                    if(debugDesignatedMembers)
                    {
                        cpssOsPrintf(" end \n");
                    }

                    if(count > 1)
                    {
                        /* error */
                        error = 1;
                    }
                }
                else
                {
                    /* not error --> will happen when single local member in all trunks */
                    /*error = 1;*/
                }

                if(error)
                {
                    cpssOsPrintf(" ERROR in designated table index [%d] \n",ii);
                }

                ii++;
            }

            cpssOsPrintf("\n");


        }

        trunkId++;
    }

    if(isEmpty == GT_TRUE)
    {
        if(IS_PHY_PORT_BY_MASK_MAC(devNum))
        {
            cpssOsPrintf("All non-trunk entries with all ports (256 ports) \n");
        }
        else
        {
            cpssOsPrintf("All non-trunk entries with :");
            cpssOsPrintf("[0x%8.8x] ",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[0]);
            if(PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[1])
            {
                cpssOsPrintf("[0x%8.8x]",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[1]);
            }
            if(PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[2])
            {
                cpssOsPrintf("[0x%8.8x]",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[2]);
            }
            if(PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[3])
            {
                cpssOsPrintf("[0x%8.8x]",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[3]);
            }
            cpssOsPrintf("\n" );
        }
    }

    cpssOsPrintf("\n");

    /* Designated Trunk Port Entry<n> table */
    cpssOsPrintf("Designated Trunk Port Entry<n> table \n");

    isEmpty = GT_TRUE;

    ii = 0;
    while(1)
    {
        if(GT_OK != cpssDxChTrunkDesignatedPortsEntryGet(devNum,ii,&portsBmp))
        {
            break;
        }

        if(0 == ARE_ALL_EXISTING_PORTS_MAC(devNum,&portsBmp))
        {
            /* print only entries that not all existing ports are in the BMP of the 'designated' */

            cpssOsPrintf("Designated entry [%d]  : ",ii);
            cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[0]);
            if(portsBmp.ports[1])
            {
                cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[1]);
            }
            if(portsBmp.ports[2])
            {
                cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[2]);
            }
            if(portsBmp.ports[3])
            {
                cpssOsPrintf("[0x%8.8x] ",portsBmp.ports[3]);
            }
            cpssOsPrintf("\n" );
            isEmpty = GT_FALSE;
        }
        else
        {
            cpssOsPrintf("Designated entry [%d]  : no 'existing port' port is excluded \n",ii);
        }

        ii++;
    }

    if(isEmpty == GT_TRUE)
    {
        if(IS_PHY_PORT_BY_MASK_MAC(devNum))
        {
            cpssOsPrintf("All designated entries with all ports (256 ports) \n");
        }
        else
        {
            cpssOsPrintf("All designated entries with :");
            cpssOsPrintf("[0x%8.8x] ",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[0]);
            if(PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[1])
            {
                cpssOsPrintf("[0x%8.8x]",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[1]);
            }
            if(PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[2])
            {
                cpssOsPrintf("[0x%8.8x]",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[2]);
            }
            if(PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[3])
            {
                cpssOsPrintf("[0x%8.8x]",PRV_CPSS_PP_MAC(devNum)->existingPorts.ports[3]);
            }
            cpssOsPrintf("\n" );
        }
    }

    cpssOsPrintf("\n");

    cpssOsPrintf("per port , (ingress) trunk indication \n");

    isEmpty = GT_TRUE;

    numOfPorts = IS_PHY_PORT_BY_MASK_MAC(devNum) ? 256 : PRV_CPSS_PP_MAC(devNum)->numOfPorts;

    for (portNum = 0; portNum < numOfPorts; portNum++)
    {
        if(IS_PHY_PORT_BY_MASK_MAC(devNum))
        {
            /* no skip on ports */
        }
        else
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
            continue;

        if(GT_OK != cpssDxChTrunkPortTrunkIdGet(devNum,portNum,&isTrunkMember,&trunkId))
        {
            break;
        }

        if(isTrunkMember == GT_TRUE)
        {
            trunkType = CPSS_TRUNK_TYPE_REGULAR_E;
            rc = prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType);
            if(rc != GT_OK)
            {
                cpssOsPrintf("prvCpssGenericTrunkDbTrunkTypeGet: FAIL [%d] on trunk[%d]",rc,trunkId);
            }

            /* print only ports that registered on trunk */

            cpssOsPrintf("port[%d] - TrunkGroupId [%d]",portNum,
                trunkId);

            if(trunkType == CPSS_TRUNK_TYPE_CASCADE_E)
            {
                cpssOsPrintf(" --cascade trunk--");
            }

            cpssOsPrintf("\n");

            isEmpty = GT_FALSE;
        }
    }

    if(isEmpty == GT_TRUE)
    {
        cpssOsPrintf("no port is associated with trunk \n");
    }

    cpssOsPrintf("\n");

    /* Trunk Members  */
    cpssOsPrintf("Trunk<n> Members Table (1..127) \n");
    cpssOsPrintf("each trunk members is : <port , device> \n");

    isEmpty = GT_TRUE;

    /* loop on all trunks -- without trunkId = 0 */
    trunkId = 1;
    while(trunkId < numOfTrunks)
    {
        if(GT_OK != cpssDxChTrunkTableEntryGet(devNum,trunkId,&numMembers,membersArray))
        {
            trunkId++;
            continue;
        }

        trunkType = CPSS_TRUNK_TYPE_REGULAR_E;
        rc = prvCpssGenericTrunkDbTrunkTypeGet(devNum,trunkId,&trunkType);
        if(rc != GT_OK)
        {
            cpssOsPrintf("prvCpssGenericTrunkDbTrunkTypeGet: FAIL [%d] on trunk[%d]",rc,trunkId);
        }

        if(numMembers == 0)
        {
            /* ERROR */
            cpssOsPrintf("[%d] -- 0 members -- ERROR !!!! \n",trunkId);

            trunkId ++;
            continue;
        }
        else
        if(numMembers == 1 && membersArray[0].port == 62)
        {
            if(trunkType == CPSS_TRUNK_TYPE_CASCADE_E)
            {
                cpssOsPrintf("[%d] --cascade trunk-- \n",trunkId);
                isEmpty = GT_FALSE;
            }

            /* print only trunks with 'real members' */
            trunkId ++;
            continue;
        }

        isEmpty = GT_FALSE;

        cpssOsPrintf("[%d] -- number members [%d]: ",trunkId,numMembers);
        for(ii = 0 ; ii < numMembers ; ii++)
        {
            /* print pairs of (port,device) */
            cpssOsPrintf("(%d,%d),",
                membersArray[ii].port,membersArray[ii].hwDevice);
        }

        cpssOsPrintf("\n");

        trunkId ++;
    }

    if(isEmpty == GT_TRUE)
    {
        cpssOsPrintf("all trunks with only NULL port member \n");
    }

    cpssOsPrintf("\n");

    cpssOsPrintf(" cpssDxChTrunkDump - End \n");

    return cpssGenericTrunkDumpDb(devNum);
}

/* start table : cpssDxChTrunkCascadeTrunk */

/*******************************************************************************
* cpssDxChTrunkCascadeTrunkPortsSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the 'cascade' trunk with the specified 'Local ports'
*       overriding any previous setting.
*       The cascade trunk may be invalidated/unset by portsMembersPtr = NULL.
*       Local ports are ports of only configured device.
*       Cascade trunk is:
*           - members are ports of only configured device pointed by devNum
*           - trunk members table is empty (see cpssDxChTrunkTableEntrySet)
*             Therefore it cannot be used as target by ingress engines like FDB,
*             Router, TTI, Ingress PCL and so on.
*           - members ports trunk ID is not set (see cpssDxChTrunkPortTrunkIdSet).
*             Therefore packets those ingresses in member ports are not associated with trunk
*           - all members are enabled only and cannot be disabled.
*           - may be used for cascade traffic and pointed by the 'Device map table'
*             as the local target to reach to the 'Remote device'.
*             (For 'Device map table' refer to cpssDxChCscdDevMapTableSet(...))
*
*
* APPLICABLE DEVICES:   All DxCh Devices
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
*   3. because this function not set the 'Trunk members' table entry , the
*       application should not point to this trunk from any ingress unit , like:
*       FDB , PCL action redirect , NH , TTI action redirect , PVE ...
*       (it should be pointed ONLY from the device map table)
*   4. Application can manipulate the 'Per port' trunk-id , for those ports ,
*       using the 'Low level API' of : cpssDxChTrunkPortTrunkIdSet(...)
*   5. this API supports only trunks with types : CPSS_TRUNK_TYPE_FREE_E or
*       CPSS_TRUNK_TYPE_CASCADE_E.
*   6. next APIs are not supported from trunk with type : CPSS_TRUNK_TYPE_CASCADE_E
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
*   3. manipulate only 'Non-trunk' table and | 3. manipulate all trunk tables :
*      'Designated trunk' table              |  'Per port' trunk-id , 'Trunk members',
*                                            |  'Non-trunk' , 'Designated trunk' tables.
*   ----------------------------------------------------------------------------
*   4. ingress unit must not point to this   | 4. no restriction on ingress/egress
*      trunk (because 'Trunk members' entry  |    units.
*       hold no ports)                       |
*   ----------------------------------------------------------------------------
*   5. not associated with trunk Id on       | 5. for cascade trunk : since 'Per port'
*      ingress                               | the trunk-Id is set , then load balance
*                                            | according to 'Local port' for traffic
*                                            | that ingress cascade trunk and
*                                            | egress next cascade trunk , will
*                                            | egress only from one of the egress
*                                            | trunk ports. (because all ports associated
*                                            | with the trunk-id)
*   ----------------------------------------------------------------------------
*
*******************************************************************************/
static CMD_STATUS internal_TrunkCascadeTrunkSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result;
    GT_U8              devNum;
    GT_TRUNK_ID        trunkId;
    CPSS_PORTS_BMP_STC portsBmp;
    CPSS_PORTS_BMP_STC addedToTrunkPortsBmp;
    CPSS_PORTS_BMP_STC removedFromTrunkPortsBmp;
    CPSS_PORTS_BMP_STC afterRemoveFromTrunkPortsBmp;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&addedToTrunkPortsBmp);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&removedFromTrunkPortsBmp);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    portsBmp.ports[0] = (GT_U32)inFields[1];
    portsBmp.ports[1] = (GT_U32)inFields[2];
    if(numFields > 3)
    {
        portsBmp.ports[2] = (GT_U32)inFields[3];
        portsBmp.ports[3] = (GT_U32)inFields[4];
    }
    CONVERT_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,portsBmp);

    if(cmdAppTrunkUpdatedPortsGet != 0)
    {
        cmdAppTrunkUpdatedPortsGet(devNum, trunkId, &portsBmp,
                                     &addedToTrunkPortsBmp,
                                     &removedFromTrunkPortsBmp);
    }

    if(cmdAppTrunkCascadeTrunkPortsSet != 0)
    {
        if(CPSS_PORTS_BMP_IS_ZERO_MAC(&removedFromTrunkPortsBmp) == 0)
        {
            /* Clear added ports from ports bitmap, they will be added on 'ADD' sequence call */
            CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(&afterRemoveFromTrunkPortsBmp,
                                               &portsBmp, &addedToTrunkPortsBmp);

            result = cmdAppTrunkCascadeTrunkPortsSet(devNum, trunkId,
                                       &afterRemoveFromTrunkPortsBmp);
            if(result != GT_OK)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
        }
    }

    /* call cpss api function */
    result = cpssDxChTrunkCascadeTrunkPortsSet(devNum, trunkId, &portsBmp);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    if(cmdAppTrunkCascadeTrunkPortsSet != 0)
    {
        if(CPSS_PORTS_BMP_IS_ZERO_MAC(&addedToTrunkPortsBmp) == 0)
        {
            result = cmdAppTrunkCascadeTrunkPortsSet(devNum, trunkId, &portsBmp);
            if(result != GT_OK)
            {
                /* pack output arguments to galtis string */
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
static CMD_STATUS wrCpssDxChTrunkCascadeTrunkSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_TrunkCascadeTrunkSet(inArgs,inFields,numFields,outArgs);
}
static CMD_STATUS wrCpssDxChTrunkCascadeTrunk128PortsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_TrunkCascadeTrunkSet(inArgs,inFields,numFields,outArgs);
}

/*
 the function keep on looking for next trunk that is not empty starting with
 trunkId = (gTrunkId+1).

 function return the trunk members (enabled + disabled)
*/
static CMD_STATUS internal_CascadeTrunkGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_TRUNK_ID            tmpTrunkId;
    CPSS_PORTS_BMP_STC     portsBmp;
    CPSS_TRUNK_TYPE_ENT    trunkType;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tmpTrunkId = gTrunkId;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(tmpTrunkId);

    /*check if trunk is valid by checking if GT_OK*/
    while(gTrunkId <= DXCH_NUM_TRUNKS_127_CNS)
    {
        gTrunkId++;

        TRUNK_WA_SKIP_TRUNK_ID_MAC(gTrunkId);

        tmpTrunkId = gTrunkId;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(tmpTrunkId);


        result = cpssDxChTrunkDbTrunkTypeGet(devNum,tmpTrunkId,&trunkType);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
            return CMD_OK;
        }

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);

        switch(trunkType)
        {
            case CPSS_TRUNK_TYPE_CASCADE_E:
                result = cpssDxChTrunkCascadeTrunkPortsGet(devNum,tmpTrunkId,&portsBmp);
                if(result != GT_OK)
                {
                    galtisOutput(outArgs, result, "%d",-1);/* Error ???? */
                    return CMD_OK;
                }
                break;
            default:
                break;
        }

        if(CPSS_PORTS_BMP_IS_ZERO_MAC(&portsBmp) == 0)
        {
            break;
        }
    }

    if(gTrunkId > DXCH_NUM_TRUNKS_127_CNS)
    {
        /* we done with the last trunk , or last trunk is empty */
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
        return CMD_OK;
    }

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(tmpTrunkId);
    CONVERT_BACK_DEV_PHYSICAL_PORTS_BMP_MAC(devNum,portsBmp);
    inFields[0] = tmpTrunkId;
    inFields[1] = portsBmp.ports[0];
    inFields[2] = portsBmp.ports[1];

    if(numFields <= 3)
    {
        /* pack and output table fields */
        fieldOutput("%d%d%d", inFields[0], inFields[1],  inFields[2]);
    }
    else
    {
        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],  inFields[2],
            portsBmp.ports[2],
            portsBmp.ports[3]);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkCascadeTrunkGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;/*reset on first*/

    return internal_CascadeTrunkGetNext(inArgs,inFields,3,outArgs);
}

static CMD_STATUS wrCpssDxChTrunkCascadeTrunkGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_CascadeTrunkGetNext(inArgs,inFields,3,outArgs);
}

static CMD_STATUS wrCpssDxChTrunkCascadeTrunk128PortsGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;/*reset on first*/

    return internal_CascadeTrunkGetNext(inArgs,inFields,5,outArgs);
}

static CMD_STATUS wrCpssDxChTrunkCascadeTrunk128PortsGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_CascadeTrunkGetNext(inArgs,inFields,5,outArgs);
}


static CMD_STATUS wrCpssDxChTrunkCascadeTrunkDelete

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              result;

    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    /* call cpss api function */
    result = cpssDxChTrunkCascadeTrunkPortsSet(devNum, trunkId, NULL);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if(cmdAppTrunkCascadeTrunkPortsSet != 0)
    {
        result = cmdAppTrunkCascadeTrunkPortsSet(devNum, trunkId, NULL);
        if(result != GT_OK)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkCascadeTrunk128PortsDelete

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTrunkCascadeTrunkDelete(inArgs,inFields,numFields,outArgs);
}


/* end table   : cpssDxChTrunkCascadeTrunk */

/* start table : cpssDxChTrunkPortHashMaskInfo */

/*******************************************************************************
* cpssDxChTrunkPortHashMaskInfoSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set port-based hash mask info.
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       devNum  - The device number.
*       portNum - The port number.
*       overrideEnable - enable/disable the override
*       index - the index to use when 'Override enabled'.
*               (values 0..15) , relevant only when overrideEnable = GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or portNum
*       GT_OUT_OF_RANGE          - when overrideEnable is enabled and index out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkPortHashMaskInfoSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              result;

    GT_U8                  devNum;
    GT_PORT_NUM     portNum;
    GT_BOOL   overrideEnable;
    GT_U32    index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inFields[0];

    CONVERT_DEV_PORT_U32_MAC(devNum,portNum);

    overrideEnable = (GT_BOOL)inFields[1];
    index = (GT_U32)inFields[2];

    /* need add support for eArch ports. at the moment the user can confihure portnum up to GT_U8 value.*/
    CPSS_TBD_BOOKMARK_EARCH

    /* call cpss api function */
    result = cpssDxChTrunkPortHashMaskInfoSet(devNum, (GT_PORT_NUM)portNum, overrideEnable , index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static GT_PORT_NUM    portHashMask;
/*
 portNum = (portHashMask).
*/
static CMD_STATUS wrCpssDxChTrunkPortHashMaskInfoGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_BOOL   overrideEnable=GT_FALSE;
    GT_U32    index=0;
    GT_HW_DEV_NUM       __HwDevNum;
    GT_PORT_NUM         __portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d",-1);/* error */
        return CMD_OK;
    }

    result = GT_FAIL;

    /* need to change 64 for maxNum of ePort */
    CPSS_TBD_BOOKMARK_EARCH

    /*check if trunk is valid by checking if GT_OK*/
    while(portHashMask < 64)
    {
        result = cpssDxChTrunkPortHashMaskInfoGet(devNum, portHashMask, &overrideEnable , &index);

        portHashMask++;

        if(result == GT_OK)
        {
            break;
        }
    }

    /* need to change 64 for maxNum of ePort */
    CPSS_TBD_BOOKMARK_EARCH

    if(portHashMask == 64 && result != GT_OK)
    {
        /* we done with the last port */
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
        return CMD_OK;
    }

    __HwDevNum = devNum;
    __portNum = portHashMask - 1;

    CONVERT_BACK_DEV_PORT_DATA_MAC(__HwDevNum,__portNum);

    inFields[0] = __portNum;
    inFields[1] = overrideEnable;
    inFields[2] = index;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1],  inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkPortHashMaskInfoGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portHashMask = 0;/*reset on first*/

    return wrCpssDxChTrunkPortHashMaskInfoGetNext(inArgs,inFields,numFields,outArgs);
}

/* end table   : cpssDxChTrunkPortHashMaskInfo */

/* start table : cpssDxChTrunkHashMaskCrc */
/*******************************************************************************
* cpssDxChTrunkHashMaskCrcEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the entry of masks in the specified index in 'CRC hash mask table'.
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..27)
*       entryPtr - (pointer to) The entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - one of the fields in entryPtr are out of range
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashMaskCrcSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              result;

    GT_U32                 ii;
    GT_U8                  devNum;
    GT_U32                 index;
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ii = 0;
    index                      = (GT_U32)inFields[ii++];
    entry.l4DstPortMaskBmp     = (GT_U32)inFields[ii++];
    entry.l4SrcPortMaskBmp     = (GT_U32)inFields[ii++];
    entry.ipv6FlowMaskBmp      = (GT_U32)inFields[ii++];
    entry.ipDipMaskBmp         = (GT_U32)inFields[ii++];
    entry.ipSipMaskBmp         = (GT_U32)inFields[ii++];
    entry.macDaMaskBmp         = (GT_U32)inFields[ii++];
    entry.macSaMaskBmp         = (GT_U32)inFields[ii++];
    entry.mplsLabel0MaskBmp    = (GT_U32)inFields[ii++];
    entry.mplsLabel1MaskBmp    = (GT_U32)inFields[ii++];
    entry.mplsLabel2MaskBmp    = (GT_U32)inFields[ii++];
    entry.localSrcPortMaskBmp  = (GT_U32)inFields[ii++];
    entry.udbsMaskBmp          = (GT_U32)inFields[ii++];

    /* call cpss api function */
    result = cpssDxChTrunkHashMaskCrcEntrySet(devNum, index, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static GT_U8    hashMaskCrcIndex;
/*
 index = (hashMaskCrcIndex).
*/
static CMD_STATUS wrCpssDxChTrunkHashMaskCrcGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d",-1);/* error */
        return CMD_OK;
    }

    /*check if trunk is valid by checking if GT_OK*/
    result = cpssDxChTrunkHashMaskCrcEntryGet(devNum, hashMaskCrcIndex, &entry);
    if(result != GT_OK)
    {
        /* we done with the last index */
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d"
                ,hashMaskCrcIndex
                ,entry.l4DstPortMaskBmp
                ,entry.l4SrcPortMaskBmp
                ,entry.ipv6FlowMaskBmp
                ,entry.ipDipMaskBmp
                ,entry.ipSipMaskBmp
                ,entry.macDaMaskBmp
                ,entry.macSaMaskBmp
                ,entry.mplsLabel0MaskBmp
                ,entry.mplsLabel1MaskBmp
                ,entry.mplsLabel2MaskBmp
                ,entry.localSrcPortMaskBmp
                ,entry.udbsMaskBmp);

    hashMaskCrcIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkHashMaskCrcGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    hashMaskCrcIndex = 0;/*reset on first*/

    return wrCpssDxChTrunkHashMaskCrcGetNext(inArgs,inFields,numFields,outArgs);
}

/* end table   : cpssDxChTrunkHashMaskCrc */

/* start : cpssDxChTrunkHashIndexCalculate */

/*******************************************************************************
* cpssDxChTrunkHashIndexCalculate
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Calculate hash index of member load-balancing group for given hash client.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - the device number.
*       hashInputDataPtr   - (pointer to) hash input data (pattern + mask).
*       hashClientType     - client that uses HASH.
*       numberOfMembers    - number of members in load-balancing group.
*
* OUTPUTS:
*       hashIndexPtr       - (pointer to) calculated by means of ingress hash index of load-balancing group member.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - one of the fields are out of range
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This API is relevant when <Random Enable> flag = GT_FALSE. It determines whether the load balancing
*       is based on the ingress hash or on a 16-bit pseudo-random.
*       The only supported hash mode is Enhanced CRC-based hash mode.
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashIndexCalculate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_TRUNK_LBH_INPUT_DATA_STC  hashInputData;
    GT_U32                              l4DstPort;
    GT_U32                              l4SrcPort;
    GT_U32                              ipv6Flow;
    GT_IPADDR                           ipv4Dip;
    GT_IPADDR                           ipv4Sip;
    GT_IPV6ADDR                         ipv6Dip;
    GT_IPV6ADDR                         ipv6Sip;
    GT_ETHERADDR                        macDa;
    GT_ETHERADDR                        macSa;
    GT_U32                              mplsLabel0;
    GT_U32                              mplsLabel1;
    GT_U32                              mplsLabel2;
    CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT hashEntityType;
    GT_U32                              numberOfMembers;
    GT_U32                              hashIndex;
    GT_U32                              i;
    CPSS_DXCH_PCL_PACKET_TYPE_ENT       packetType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    packetType      = (CPSS_DXCH_PCL_PACKET_TYPE_ENT)inArgs[1];
    hashEntityType = (CPSS_DXCH_TRUNK_HASH_CLIENT_TYPE_ENT)inArgs[2];
    numberOfMembers = (GT_U32)inArgs[3];

    l4DstPort       = (GT_U32)inArgs[4];
    for(i=0; i<2; i++)
    {
        hashInputData.l4DstPortArray[i]= (GT_U8)((l4DstPort>>(8*i)) & 0xFF);

    }

    l4SrcPort       = (GT_U32)inArgs[5];
    for(i=0; i<2; i++)
    {
        hashInputData.l4SrcPortArray[i]= (GT_U8)((l4SrcPort>>(8*i)) & 0xFF);
    }

    ipv6Flow   = (GT_U32)inArgs[6];
    for(i=0; i<3; i++)
    {
        hashInputData.ipv6FlowArray[i]= (GT_U8)((ipv6Flow>>(8*i)) & 0xFF);
    }

    cpssOsMemSet(hashInputData.ipDipArray, 0, sizeof(hashInputData.ipDipArray));
    galtisIpAddr(&ipv4Dip, (GT_U8*)inArgs[7]);
    for(i=0; i<4; i++)
    {
        hashInputData.ipDipArray[i] = ipv4Dip.arIP[i];
    }

    cpssOsMemSet(hashInputData.ipSipArray, 0, sizeof(hashInputData.ipSipArray));
    galtisIpAddr(&ipv4Sip, (GT_U8*)inArgs[8]);
    for(i=0; i<4; i++)
    {
        hashInputData.ipSipArray[i] = ipv4Sip.arIP[i];
    }
    galtisIpv6Addr(&ipv6Dip, (GT_U8*)inArgs[9]);
    for(i=0; i<16; i++)
    {
        hashInputData.ipDipArray[i] = ipv6Dip.arIP[i];
    }

    galtisIpv6Addr(&ipv6Sip, (GT_U8*)inArgs[10]);
    for(i=0; i<16; i++)
    {
        hashInputData.ipSipArray[i] = ipv6Sip.arIP[i];
    }

    galtisMacAddr(&macDa, (GT_U8*)inArgs[11]);
    for(i=0; i<6; i++)
    {
        hashInputData.macDaArray[i] = macDa.arEther[i];
    }

    galtisMacAddr(&macSa, (GT_U8*)inArgs[12]);
    for(i=0; i<6; i++)
    {
        hashInputData.macSaArray[i] = macSa.arEther[i];
    }

    mplsLabel0      = (GT_U32)inArgs[13];
    for(i=0; i<3; i++)
    {
        hashInputData.mplsLabel0Array[i]= (GT_U8)((mplsLabel0>>(8*i)) & 0xFF);
    }

    mplsLabel1      = (GT_U32)inArgs[14];
    for(i=0; i<3; i++)
    {
        hashInputData.mplsLabel1Array[i]= (GT_U8)((mplsLabel1>>(8*i)) & 0xFF);
    }

    mplsLabel2      = (GT_U32)inArgs[15];
    for(i=0; i<3; i++)
    {
        hashInputData.mplsLabel2Array[i]= (GT_U8)((mplsLabel2>>(8*i)) & 0xFF);
    }

    hashInputData.localSrcPortArray[0] = (GT_U8)inArgs[16];

    for(i=0; i<32; i++)
    {
        hashInputData.udbsArray[i] = (GT_U8)inArgs[(17+i)];
    }

    /* call cpss api function */
    result = cpssDxChTrunkHashIndexCalculate(devNum,
                                             packetType,
                                             &hashInputData,
                                             hashEntityType,
                                             numberOfMembers,
                                             &hashIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hashIndex);

    return CMD_OK;
}


/* end : cpssDxChTrunkHashIndexCalculate */

/* start table : cpssDxChTrunkHashPearson */

/*******************************************************************************
* cpssDxChTrunkHashPearsonValueSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the Pearson hash value for the specific index.
*
*       NOTE: the Pearson hash used when CRC-16 mode is used.
*
* APPLICABLE DEVICES:   Lion and above devices.
*
* INPUTS:
*       devNum  - the device number.
*       index   - the table index (0..63)
*       value   - the Pearson hash value (0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or index
*       GT_OUT_OF_RANGE          - value > 63
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       related to feature 'CRC hash mode'
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkHashPearsonSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              result;

    GT_U8                  devNum;
    GT_U32                 index;
    GT_U32                 value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inFields[0];
    value = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashPearsonValueSet(devNum, index, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


static GT_U8    pearsonIndex;
/*
 index = (pearsonIndex).
*/
static CMD_STATUS wrCpssDxChTrunkHashPearsonGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_U32                 value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d",-1);/* error */
        return CMD_OK;
    }

    /*check if trunk is valid by checking if GT_OK*/
    result = cpssDxChTrunkHashPearsonValueGet(devNum, pearsonIndex, &value);
    if(result != GT_OK)
    {
        /* we done with the last index */
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d",pearsonIndex,value);

    pearsonIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkHashPearsonGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    pearsonIndex = 0;/*reset on first*/

    return wrCpssDxChTrunkHashPearsonGetNext(inArgs,inFields,numFields,outArgs);
}
/* end table   : cpssDxChTrunkHashPearson */

/*******************************************************************************
* cpssDxChTrunkMcLocalSwitchingEnableSet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Enable/Disable sending multi-destination packets back to its source
*       trunk on the local device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChTrunkMcLocalSwitchingEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              result;

    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    GT_BOOL                 enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);
    enable = (GT_BOOL)inFields[1];

    /* call cpss api function */
    result = cpssDxChTrunkMcLocalSwitchingEnableSet(devNum, trunkId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*
 the function keep on looking for next trunk that is set <multi-dest local switch EN> = GT_TRUE
 starting with trunkId = (gTrunkId+1).

 function return the trunk Id and if <multi-dest local switch EN> = GT_TRUE/GT_FALSE
*/
static CMD_STATUS wrCpssDxChTrunkMcLocalSwitchingEnableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_TRUNK_ID            tmpTrunkId;
    CPSS_TRUNK_TYPE_ENT    trunkType;
    GT_BOOL                enable = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tmpTrunkId = gTrunkId;
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(tmpTrunkId);

    /*check if trunk is valid by checking if GT_OK*/
    while(gTrunkId <= DXCH_NUM_TRUNKS_127_CNS)
    {
        gTrunkId++;

        TRUNK_WA_SKIP_TRUNK_ID_MAC(gTrunkId);

        tmpTrunkId = gTrunkId;
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(tmpTrunkId);


        result = cpssDxChTrunkDbTrunkTypeGet(devNum,tmpTrunkId,&trunkType);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
            return CMD_OK;
        }

        enable = GT_FALSE;

        switch(trunkType)
        {
            case CPSS_TRUNK_TYPE_FREE_E:
            case CPSS_TRUNK_TYPE_REGULAR_E:
                result = cpssDxChTrunkDbMcLocalSwitchingEnableGet(devNum,tmpTrunkId,&enable);
                if(result != GT_OK)
                {
                    galtisOutput(outArgs, result, "%d",-1);/* Error ???? */
                    return CMD_OK;
                }
                break;

            /* NOTE : we skip cascade trunks , as it is not relevant to them ... */
            default:
                break;
        }

        if(enable == GT_TRUE)
        {
            break;
        }
    }

    if(gTrunkId > DXCH_NUM_TRUNKS_127_CNS)
    {
        /* we done with the last trunk , or last trunk is empty */
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
        return CMD_OK;
    }

    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(tmpTrunkId);
    inFields[0] = tmpTrunkId;
    inFields[1] = enable; /* the enable at this stage should be == GT_TRUE .
           because we wish to see only 'Special trunks' that 'flood back to src trunk' ! */

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkMcLocalSwitchingEnableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;/*reset on first*/

    return wrCpssDxChTrunkMcLocalSwitchingEnableGetNext(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkDbMembersSortingEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function enable/disable 'sorting' of trunk members in the
*       'trunk members table' and in the 'designated trunk table'
*       This mode not effect 'cascade trunks' (that not need synchronization between devices)
*       'sorting enabled' : when the application will add/remove/set members in a trunk
*                           cpss will make sure to set the trunk members into the
*                           device in an order that is not affected by the 'history'
*                           of 'add/remove' members to/from the trunk.
*
*       'sorting disabled' : (legacy mode / default mode) when the application
*                           will add/remove/set members in a trunk cpss will set
*                           the trunk members into the device in an order that is
*                           affected by the 'history' of 'add/remove' members
*                           to/from the trunk.
*
*       function uses the DB (no HW operations)
*       The API start to effect only on trunks that do operations (add/remove/set members)
*       after the API call.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*       enable      - enable/disable the sorting
*                     GT_TRUE : enabled , GT_FALSE - disabled
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_BAD_PARAM       - bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDbMembersSortingEnableSet
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
    result = cpssDxChTrunkDbMembersSortingEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTrunkDbMembersSortingEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       Gets the enable/disable 'sorting' of trunk members in the 'trunk members table'
*       and in the 'designated trunk table'
*       This mode not effect 'cascade trunks' (that not need synchronization between devices)
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr      - (pointer to) the sorting is enabled/disabled
*
* RETURNS:
*       GT_OK              - on success.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_BAD_PARAM       - bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDbMembersSortingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     dev;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkDbMembersSortingEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/* next relate to cascade trunk with weighted members */
#define MAX_WEIGHTED_MEMBERS_CNS    64
static GT_U32                               numOfWeightedMembers = 0;
static CPSS_TRUNK_WEIGHTED_MEMBER_STC       weightedMembersArray[MAX_WEIGHTED_MEMBERS_CNS];

/*******************************************************************************
* cpssDxChTrunkCascadeTrunkWithWeightedPortsSet
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
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
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
*                            or this trunk hold members defined using cpssDxChTrunkMembersSet(...)
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
*       cpssDxChTrunkMembersSet ,
*       cpssDxChTrunkMemberAdd , cpssDxChTrunkMemberRemove,
*       cpssDxChTrunkMemberEnable , cpssDxChTrunkMemberDisable
*       cpssDxChTrunkDbEnabledMembersGet , cpssDxChTrunkDbDisabledMembersGet
*       cpssDxChTrunkDesignatedMemberSet , cpssDxChTrunkDbDesignatedMemberGet
*   6. the total weights of all the trunk members is restricted to :
*      a. must be equal to power of 2 (1,2,4,8,16,32,64...)
*      b. must not be larger then actual designated table size of the device.
*         (Lion ,Lion2 : 64  , other devices : 8)
*      c. not relevant when single member exists
*
********************************************************************************
*   Comparing the 2 function :
*
*cpssDxChTrunkCascadeTrunkWithWeightedPortsSet |   cpssDxChTrunkMembersSet
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
static CMD_STATUS internalCascadeTrunkWithWeightedPortsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN GT_U32   opCode
)
{
    GT_STATUS result;
    CPSS_TRUNK_MEMBER_STC member;
    GT_U8   devNum;
    GT_U32 weight;
    GT_TRUNK_ID trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(opCode == 0)/*start registration*/
    {
        numOfWeightedMembers = 0;
    }
    else if(opCode == 0xFFFFFFFF)/*end registration*/
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        trunkId = (GT_TRUNK_ID)inArgs[1];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

        result = cpssDxChTrunkCascadeTrunkWithWeightedPortsSet(devNum,trunkId,numOfWeightedMembers,weightedMembersArray);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d",-1);/* Error ! */
            return CMD_OK;
        }
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    if (numOfWeightedMembers >= MAX_WEIGHTED_MEMBERS_CNS)
    {
        galtisOutput(outArgs, GT_FULL, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    member.port = (GT_PHYSICAL_PORT_NUM)inFields[0];
    member.hwDevice = (GT_HW_DEV_NUM)inFields[1];
    CONVERT_DEV_PORT_DATA_MAC(member.hwDevice, member.port);

    weight = (GT_U32)inFields[2];

    weightedMembersArray[numOfWeightedMembers].member = member;
    weightedMembersArray[numOfWeightedMembers].weight = weight;
    numOfWeightedMembers++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}
static CMD_STATUS wrCpssDxChTrunkCascadeTrunkWithWeightedPortsSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalCascadeTrunkWithWeightedPortsSet(inArgs,inFields,numFields,outArgs,0);
}
static CMD_STATUS wrCpssDxChTrunkCascadeTrunkWithWeightedPortsSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalCascadeTrunkWithWeightedPortsSet(inArgs,inFields,numFields,outArgs,1);
}
static CMD_STATUS wrCpssDxChTrunkCascadeTrunkWithWeightedPortsSetEnd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalCascadeTrunkWithWeightedPortsSet(inArgs,inFields,numFields,outArgs,0xFFFFFFFF);
}


static CMD_STATUS internal_CascadeTrunkWithWeightedPortsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN GT_BOOL isFirst
)
{
    GT_STATUS result;
    CPSS_TRUNK_MEMBER_STC member;
    GT_U8   devNum;
    GT_TRUNK_ID trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(trunkId);

    if(isFirst == GT_TRUE)
    {
        gIndGet = 0; /*reset on first*/
        numOfWeightedMembers = MAX_WEIGHTED_MEMBERS_CNS;
        result = cpssDxChTrunkCascadeTrunkWithWeightedPortsGet(devNum,trunkId,&numOfWeightedMembers,weightedMembersArray);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d",-1);/* Error ! */
            return CMD_OK;
        }
    }

    if(gIndGet >= numOfWeightedMembers)
    {
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more members to get */
        return CMD_OK;
    }

    member = weightedMembersArray[gIndGet].member;
    CONVERT_BACK_DEV_PORT_DATA_MAC(member.hwDevice, member.port);

    inFields[0] = member.port;
    inFields[1] = member.hwDevice;
    inFields[2] = weightedMembersArray[gIndGet].weight;

    /* we need to retrieve another trunk member */
    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0],  inFields[1],  inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    gIndGet ++;

    return CMD_OK;

}

static CMD_STATUS wrCpssDxChTrunkCascadeTrunkWithWeightedPortsGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_CascadeTrunkWithWeightedPortsGet(inArgs,inFields,numFields,outArgs,GT_TRUE);
}

static CMD_STATUS wrCpssDxChTrunkCascadeTrunkWithWeightedPortsGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internal_CascadeTrunkWithWeightedPortsGet(inArgs,inFields,numFields,outArgs,GT_FALSE);
}

/*
GT_STATUS cpssDxChTrunkHashMaskCrcParamOverrideSet
(
    IN GT_U8                                            devNum,
    IN CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT fieldType,
    IN GT_BOOL                                          override
);
*/
static CMD_STATUS wrCpssDxChTrunkHashMaskCrcParamOverrideSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8  devNum;
    CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT fieldType;
    GT_BOOL                                          override ;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    fieldType   = (CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT)inArgs[1];
    override   = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTrunkHashMaskCrcParamOverrideSet(devNum, fieldType, override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkHashMaskCrcParamOverrideGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8  devNum;
    CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT fieldType;
    GT_BOOL                                          override ;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    fieldType   = (CPSS_DXCH_TRUNK_HASH_MASK_CRC_PARAM_OVERRIDE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashMaskCrcParamOverrideGet(devNum, fieldType, &override);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",override);
    return CMD_OK;
}



/*
GT_STATUS cpssDxChTrunkHashCrcSaltByteSet
(
    IN GT_U8    devNum,
    IN GT_U32   byteIndex,
    IN GT_U8    saltValue
);
*/
static CMD_STATUS wrCpssDxChTrunkHashCrcSaltByteSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8  devNum;
    GT_U32   byteIndex;
    GT_U8    saltValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];

    if(inFields[1] > 0xFF)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    byteIndex   = (GT_U32)inFields[0];
    saltValue   = (GT_U8)inFields[1];

    /* call cpss api function */
    result = cpssDxChTrunkHashCrcSaltByteSet(devNum, byteIndex, saltValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static GT_U32   saltByteIndex = 0;

static CMD_STATUS wrCpssDxChTrunkHashCrcSaltByteGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8  devNum;
    GT_U8    saltValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTrunkHashCrcSaltByteGet(devNum, saltByteIndex, &saltValue);
    if(result == GT_BAD_PARAM)
    {
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more bytes */
        return CMD_OK;
    }

    fieldOutput("%d%d",
                saltByteIndex , saltValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTrunkHashCrcSaltByteGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    saltByteIndex = 0;/*reset on first*/

    return wrCpssDxChTrunkHashCrcSaltByteGetEntry(inArgs,inFields,numFields,outArgs);
}

static CMD_STATUS wrCpssDxChTrunkHashCrcSaltByteGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    saltByteIndex++;
    return wrCpssDxChTrunkHashCrcSaltByteGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTrunkPortMcEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets whether multicast destination traffic can egress the
*       selected port while it is a trunk member.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE:  port is enabled for multicast destination traffic.
*                  GT_FALSE: port is disabled for multicast destination traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_NOT_INITIALIZED - the trunk library was not initialized for the device
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PARAM       - bad device or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If disabled multicast destination packets would not egress through the
*       configured port.
*       This configuration has no influence on unicast destination traffic or
*       when the port is not a trunk member.
*
********************************************************************************/
static CMD_STATUS wrCpssDxChTrunkPortMcEnableSet
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
    CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    mode = (CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTrunkPortMcEnableSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTrunkDbPortMcEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function gets whether multicast destination traffic can egress the
*       selected port while it is a trunk member.
*
* APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE:  port is enabled for multicast destination traffic.
*                                  GT_FALSE: port is disabled for multicast destination traffic.
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
*       If disbale state reported it means that multicast destination packets
*       would not egress through that port.
*       This configuration has no influence on unicast destination traffic or
*       when the port is not a trunk member.
*
********************************************************************************/
static CMD_STATUS wrCpssDxChTrunkDbPortMcEnableGet

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
    CPSS_TRUNK_DESIGNATED_TABLE_MC_PORT_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTrunkDbPortMcEnableGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTrunkInit",
        &wrCpssDxChTrunkInit,
        3, 0},

    {"cpssDxChTrunkMembersSetSet",
        &wrCpssDxChTrunkMembersSet,
        1, 19},

    {"cpssDxChTrunkMembersSetGetFirst",
        &wrCpssDxChTrunkMembersGetFirst,
        1, 0},

    {"cpssDxChTrunkMembersSetGetNext",
        &wrCpssDxChTrunkMembersGetNext,
        1, 0},

    {"cpssDxChTrunkNonTrunkPortsAdd",
        &wrCpssDxChTrunkNonTrunkPortsAdd,
        4, 0},

    {"cpssDxChTrunkNonTrunkPortsRemove",
        &wrCpssDxChTrunkNonTrunkPortsRemove,
        4, 0},

    {"cpssDxChTrunkPortTrunkIdSet",
        &wrCpssDxChTrunkPortTrunkIdSet,
        4, 0},

    {"cpssDxChTrunkPortTrunkIdGet",
        &wrCpssDxChTrunkPortTrunkIdGet,
        2, 0},


    {"cpssDxChTrunkTableSet",
        &wrCpssDxChTrunkTableEntrySet,
        2, 3},

    {"cpssDxChTrunkTableGetFirst",
        &wrCpssDxChTrunkTableEntryGetFirst,
        2, 0},

    {"cpssDxChTrunkTableGetNext",
        &wrCpssDxChTrunkTableEntryGetNext,
        2, 0},

    {"cpssDxChTrunkTableDelete",
        &wrCpssDxChTrunkTableEntryDelete,
        2, 2},

    {"cpssDxChTrunkNonTrunkPortsSet",
        &wrCpssDxChTrunkNonTrunkPortsEntrySet,
        1, 2},

    {"cpssDxChTrunkNonTrunkPortsGetFirst",
        &wrCpssDxChTrunkNonTrunkPortsEntryGetFirst,
        1, 0},

    {"cpssDxChTrunkNonTrunkPortsGetNext",
        &wrCpssDxChTrunkNonTrunkPortsEntryGetNext,
        1, 0},

    {"cpssDxChTrunkNonTrunkPorts1Set",
        &wrCpssDxChTrunkNonTrunkPortsEntry1Set,
        1, 3},

    {"cpssDxChTrunkNonTrunkPorts1GetFirst",
        &wrCpssDxChTrunkNonTrunkPortsEntry1GetFirst,
        1, 0},

    {"cpssDxChTrunkNonTrunkPorts1GetNext",
        &wrCpssDxChTrunkNonTrunkPortsEntry1GetNext,
        1, 0},


    {"cpssDxChTrunkDesignatedPortsSet",
        &wrCpssDxChTrunkDesignatedPortsEntrySet,
        1, 2},

    {"cpssDxChTrunkDesignatedPortsGetFirst",
        &wrCpssDxChTrunkDesignatedPortsEntryGetFirst,
        1, 0},

    {"cpssDxChTrunkDesignatedPortsGetNext",
        &wrCpssDxChTrunkDesignatedPortsEntryGetNext,
        1, 0},

    {"cpssDxChTrunkDesignatedPorts2Set",
        &wrCpssDxChTrunkDesignatedPortsEntry2Set,
        1, 5},

    {"cpssDxChTrunkDesignatedPorts2GetFirst",
        &wrCpssDxChTrunkDesignatedPortsEntry2GetFirst,
        1, 0},

    {"cpssDxChTrunkDesignatedPorts2GetNext",
        &wrCpssDxChTrunkDesignatedPortsEntry2GetNext,
        1, 0},
    {"cpssDxChTrunkDesignatedPorts1Set",
        &wrCpssDxChTrunkDesignatedPortsEntry1Set,
        1, 3},

    {"cpssDxChTrunkDesignatedPorts1GetFirst",
        &wrCpssDxChTrunkDesignatedPortsEntry1GetFirst,
        1, 0},

    {"cpssDxChTrunkDesignatedPorts1GetNext",
        &wrCpssDxChTrunkDesignatedPortsEntry1GetNext,
        1, 0},


    {"cpssDxChTrunkDbIsMemberOfTrunk",
        &wrCpssDxChTrunkDbIsMemberOfTrunk,
        3, 0},

    {"cpssDxChTrunkHashDesignatedTableModeSet",
        &wrCpssDxChTrunkHashDesignatedTableModeSet,
        2, 0},

    {"cpssDxChTrunkHashDesignatedTableModeGet",
        &wrCpssDxChTrunkHashDesignatedTableModeGet,
        1, 0},

    {"cpssDxChTrunkHashDesignatedTableModeSet1",
        &wrCpssDxChTrunkHashDesignatedTableModeSet1,
        2, 0},

    {"cpssDxChTrunkHashDesignatedTableModeGet1",
        &wrCpssDxChTrunkHashDesignatedTableModeGet1,
        1, 0},

    {"cpssDxChTrunkHashGlobalModeSet",
        &wrCpssDxChTrunkHashGlobalModeSet,
        2, 0},

    {"cpssDxChTrunkHashGlobalModeGet",
        &wrCpssDxChTrunkHashGlobalModeGet,
        1, 0},

    {"cpssDxChTrunkHashGlobalModeSet1",
        &wrCpssDxChTrunkHashGlobalModeSet,
        2, 0},

    {"cpssDxChTrunkHashGlobalModeGet1",
        &wrCpssDxChTrunkHashGlobalModeGet,
        1, 0},

    {"cpssDxChTrunkHashIpAddMacModeSet",
        &wrCpssDxChTrunkHashIpAddMacModeSet,
        2, 0},

    {"cpssDxChTrunkHashIpAddMacModeGet",
        &wrCpssDxChTrunkHashIpAddMacModeGet,
        1, 0},

    {"cpssDxChTrunkHashIpModeSet",
        &wrCpssDxChTrunkHashIpModeSet,
        2, 0},

    {"cpssDxChTrunkHashIpModeGet",
        &wrCpssDxChTrunkHashIpModeGet,
        1, 0},

    {"cpssDxChTrunkHashL4ModeSet",
        &wrCpssDxChTrunkHashL4ModeSet,
        2, 0},

    {"cpssDxChTrunkHashL4ModeGet",
        &wrCpssDxChTrunkHashL4ModeGet,
        1, 0},

    {"cpssDxChTrunkHashIpv6ModeSet",
        &wrCpssDxChTrunkHashIpv6ModeSet,
        2, 0},

    {"cpssDxChTrunkHashIpv6ModeGet",
        &wrCpssDxChTrunkHashIpv6ModeGet,
        1, 0},

    {"cpssDxChTrunkHashVidMultiDestinationModeSet",
        &wrCpssDxChTrunkHashVidMultiDestinationModeSet,
        2, 0},

    {"cpssDxChTrunkHashVidMultiDestinationModeGet",
        &wrCpssDxChTrunkHashVidMultiDestinationModeGet,
        1, 0},

    {"cpssDxChTrunkHashMplsModeEnableSet",
        &wrCpssDxChTrunkHashMplsModeEnableSet,
        2, 0},

    {"cpssDxChTrunkHashMplsModeEnableGet",
        &wrCpssDxChTrunkHashMplsModeEnableGet,
        1, 0},

    {"cpssDxChTrunkHashMaskSet",
        &wrCpssDxChTrunkHashMaskSet,
        3, 0},

    {"cpssDxChTrunkHashMaskGet",
        &wrCpssDxChTrunkHashMaskGet,
        2, 0},

    {"cpssDxChTrunkHashIpShiftSet",
        &wrCpssDxChTrunkHashIpShiftSet,
        4, 0},

    {"cpssDxChTrunkHashIpShiftGet",
        &wrCpssDxChTrunkHashIpShiftGet,
        3, 0},

    {"cpssDxChTrunkDesignatedMemberSet",
        &wrCpssDxChTrunkDesignatedMemberSet,
        5, 0},

    {"cpssDxChTrunkDbDesignatedMemberGet",
        &wrCpssDxChTrunkDbDesignatedMemberGet,
        2, 0},


    {"cpssDxChTrunkDbTrunkTypeGet",
        &wrCpssDxChTrunkDbTrunkTypeGet,
        2, 0},

    {"cpssDxChTrunkHashCrcParametersSet",
        &wrCpssDxChTrunkHashCrcParametersSet,
        3, 0},

    {"cpssDxChTrunkHashCrcParametersGet",
        &wrCpssDxChTrunkHashCrcParametersGet,
        1, 0},

    /* start table : cpssDxChTrunkCascadeTrunk */
    {"cpssDxChTrunkCascadeTrunkSet",
        &wrCpssDxChTrunkCascadeTrunkSet,
        1, 3},

    {"cpssDxChTrunkCascadeTrunkGetFirst",
        &wrCpssDxChTrunkCascadeTrunkGetFirst,
        1, 0},

    {"cpssDxChTrunkCascadeTrunkGetNext",
        &wrCpssDxChTrunkCascadeTrunkGetNext,
        1, 0},

    {"cpssDxChTrunkCascadeTrunkDelete",
        &wrCpssDxChTrunkCascadeTrunkDelete,
        1, 3},
    /* end table   : cpssDxChTrunkCascadeTrunk */

    /* start table : cpssDxChTrunkPortHashMaskInfo */
    {"cpssDxChTrunkPortHashMaskInfoSet",
        &wrCpssDxChTrunkPortHashMaskInfoSet,
        1, 3},

    {"cpssDxChTrunkPortHashMaskInfoGetFirst",
        &wrCpssDxChTrunkPortHashMaskInfoGetFirst,
        1, 0},

    {"cpssDxChTrunkPortHashMaskInfoGetNext",
        &wrCpssDxChTrunkPortHashMaskInfoGetNext,
        1, 0},

    /* end table   : cpssDxChTrunkPortHashMaskInfo */


    /* start table : cpssDxChTrunkHashMaskCrc */
    {"cpssDxChTrunkHashMaskCrcSet",
        &wrCpssDxChTrunkHashMaskCrcSet,
        1, 13},

    {"cpssDxChTrunkHashMaskCrcGetFirst",
        &wrCpssDxChTrunkHashMaskCrcGetFirst,
        1, 0},

    {"cpssDxChTrunkHashMaskCrcGetNext",
        &wrCpssDxChTrunkHashMaskCrcGetNext,
        1, 0},

    /* end table   : cpssDxChTrunkHashMaskCrc */

    /* start   : cpssDxChTrunkHashIndexCalculate */

    {"cpssDxChTrunkHashIndexCalculate",
        &wrCpssDxChTrunkHashIndexCalculate,
        49, 0},

     /* end   : cpssDxChTrunkHashIndexCalculate */

    /* start table : cpssDxChTrunkHashPearson */
    {"cpssDxChTrunkHashPearsonSet",
        &wrCpssDxChTrunkHashPearsonSet,
        1, 2},

    {"cpssDxChTrunkHashPearsonGetFirst",
        &wrCpssDxChTrunkHashPearsonGetFirst,
        1, 0},

    {"cpssDxChTrunkHashPearsonGetNext",
        &wrCpssDxChTrunkHashPearsonGetNext,
        1, 0},

    /* end table   : cpssDxChTrunkHashPearson */

    /* start table : cpssDxChTrunkMcLocalSwitchingEnable */
    {"cpssDxChTrunkMcLocalSwitchingEnableSet",
        &wrCpssDxChTrunkMcLocalSwitchingEnableSet,
        1, 2},

    {"cpssDxChTrunkMcLocalSwitchingEnableGetFirst",
        &wrCpssDxChTrunkMcLocalSwitchingEnableGetFirst,
        1, 0},

    {"cpssDxChTrunkMcLocalSwitchingEnableGetNext",
        &wrCpssDxChTrunkMcLocalSwitchingEnableGetNext,
        1, 0},
    /* end table   : cpssDxChTrunkMcLocalSwitchingEnable */

    {"cpssDxChTrunkDbMembersSortingEnableSet",
        &wrCpssDxChTrunkDbMembersSortingEnableSet,
        2, 0},

    {"cpssDxChTrunkDbMembersSortingEnableGet",
        &wrCpssDxChTrunkDbMembersSortingEnableGet,
        1, 0},


    /* start table : dxChCascadeTrunkWithWeightedPorts */
    {"dxChCascadeTrunkWithWeightedPortsSetFirst",
        &wrCpssDxChTrunkCascadeTrunkWithWeightedPortsSetFirst,
        2, 3},
    {"dxChCascadeTrunkWithWeightedPortsSetNext",
        &wrCpssDxChTrunkCascadeTrunkWithWeightedPortsSetNext,
        2, 3},

    {"dxChCascadeTrunkWithWeightedPortsEndSet",
        &wrCpssDxChTrunkCascadeTrunkWithWeightedPortsSetEnd,
        2, 0},

    {"dxChCascadeTrunkWithWeightedPortsGetFirst",
        &wrCpssDxChTrunkCascadeTrunkWithWeightedPortsGetFirst,
        2, 0},

    {"dxChCascadeTrunkWithWeightedPortsGetNext",
        &wrCpssDxChTrunkCascadeTrunkWithWeightedPortsGetNext,
        2, 0},

    /* end table : dxChCascadeTrunkWithWeightedPorts */

    /* start table : cpssDxChTrunkCascadeTrunk128Ports */

    {"cpssDxChTrunkCascadeTrunk128PortsSet",
        &wrCpssDxChTrunkCascadeTrunk128PortsSet,
        1, 5},

    {"cpssDxChTrunkCascadeTrunk128PortsGetFirst",
        &wrCpssDxChTrunkCascadeTrunk128PortsGetFirst,
        1, 0},

    {"cpssDxChTrunkCascadeTrunk128PortsGetNext",
        &wrCpssDxChTrunkCascadeTrunk128PortsGetNext,
        1, 0},

    {"cpssDxChTrunkCascadeTrunk128PortsDelete",
        &wrCpssDxChTrunkCascadeTrunk128PortsDelete,
        1, 5},
    /* end table   : cpssDxChTrunkCascadeTrunk */

    /* start table : cpssDxChTrunkNonTrunkPorts128 */
    {"cpssDxChTrunkNonTrunkPorts128Set",
        &wrCpssDxChTrunkNonTrunkPortsEntry128Set,
        1, 5},

    {"cpssDxChTrunkNonTrunkPorts128GetFirst",
        &wrCpssDxChTrunkNonTrunkPortsEntry128GetFirst,
        1, 0},

    {"cpssDxChTrunkNonTrunkPorts128GetNext",
        &wrCpssDxChTrunkNonTrunkPortsEntry128GetNext,
        1, 0},
    /* end table : cpssDxChTrunkNonTrunkPorts128 */

    {"cpssDxChTrunkHashMaskCrcParamOverrideSet",
        &wrCpssDxChTrunkHashMaskCrcParamOverrideSet,
        3 , 0},
    {"cpssDxChTrunkHashMaskCrcParamOverrideGet",
        &wrCpssDxChTrunkHashMaskCrcParamOverrideGet,
        2 , 0},


    /* start table : cpssDxChTrunkHashCrcSaltByte */
    {"cpssDxChTrunkHashCrcSaltByteSet",
        &wrCpssDxChTrunkHashCrcSaltByteSet,
        1, 2},

    {"cpssDxChTrunkHashCrcSaltByteGetFirst",
        &wrCpssDxChTrunkHashCrcSaltByteGetFirst,
        1, 0},

    {"cpssDxChTrunkHashCrcSaltByteGetNext",
        &wrCpssDxChTrunkHashCrcSaltByteGetNext,
        1, 0},
    /* end table : cpssDxChTrunkHashCrcSaltByte */

    {"cpssDxChTrunkPortMcEnableSet",
        &wrCpssDxChTrunkPortMcEnableSet,
        3, 0},
    {"cpssDxChTrunkDbPortMcEnableGet",
        &wrCpssDxChTrunkDbPortMcEnableGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTrunk
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
GT_STATUS cmdLibInitCpssDxChTrunk
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

/*******************************************************************************
* cpssDxChPacketTransminitionTask
*
* DESCRIPTION:
*       galtis task for packet transmission.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       None
*
*******************************************************************************/
GT_STATUS __TASKCONV cmdAppTrunkCascadeTestTask
(
    GT_VOID
)
{
#define APP_DEMO_LION2_TRUNK_WA_UPLINK_PORTS_NUM_CNS             6
    GT_UINTPTR inArgs[CMD_MAX_ARGS];
    GT_UINTPTR inFields[CMD_MAX_FIELDS];
    GT_32 numFields;
    GT_8  outArgs[CMD_MAX_BUFFER];
    GT_PHYSICAL_PORT_NUM  uplinkPorts[APP_DEMO_LION2_TRUNK_WA_UPLINK_PORTS_NUM_CNS] = {68, 73, 84, 89, 100, 105};
    GT_PHYSICAL_PORT_NUM  tempPorts[APP_DEMO_LION2_TRUNK_WA_UPLINK_PORTS_NUM_CNS];
    CPSS_PORTS_BMP_STC portsBmp;
    GT_U32 idx;
    GT_PHYSICAL_PORT_NUM port;
    GT_STATUS            result;

    numFields = 5;

    /* set input arguments */
    inArgs[0] = 0;      /* Device number */
    inFields[0] = 5;    /* Trunk ID */

    /* while forever */
    while(1)
    {
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
        for(idx = 0; idx < APP_DEMO_LION2_TRUNK_WA_UPLINK_PORTS_NUM_CNS; idx++)
        {
            tempPorts[idx] = port = uplinkPorts[cmdOsRand() % 6];
            CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp, port);
        }

        inFields[1] = portsBmp.ports[0];
        inFields[2] = portsBmp.ports[1];
        inFields[3] = portsBmp.ports[2];
        inFields[4] = portsBmp.ports[3];

        result = wrCpssDxChTrunkCascadeTrunk128PortsSet(inArgs, inFields, numFields, outArgs);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
        /* Iterate over all uplink ports */
        for(idx = 0; idx < APP_DEMO_LION2_TRUNK_WA_UPLINK_PORTS_NUM_CNS; idx++)
        {
            port = tempPorts[cmdOsRand() % 6];

            CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portsBmp, port);

            inFields[1] = portsBmp.ports[0];
            inFields[2] = portsBmp.ports[1];
            inFields[3] = portsBmp.ports[2];
            inFields[4] = portsBmp.ports[3];

            result = wrCpssDxChTrunkCascadeTrunk128PortsSet(inArgs, inFields, numFields, outArgs);
            if(result != GT_OK)
            {
                galtisOutput(outArgs, result, "");
                return CMD_OK;
            }
        }

        cmdOsTimerWkAfter(100);
    }
}

/*******************************************************************************
* cmdAppTrunkCascadeTestStart
*
* DESCRIPTION:
*       Starts Lion2 cacsade trunk test
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdCpssDxChTxStart
*
*******************************************************************************/
GT_STATUS cmdAppTrunkCascadeTestStart
(
)
{
    GT_STATUS rc = GT_OK;
    static GT_BOOL   enterOnce = GT_FALSE;

    /* create the task only once */
    if(enterOnce == GT_FALSE)
    {
        rc = cmdOsTaskCreate("GalCscdTrunkTest",                  /* Task Name                      */
                            100,                                    /* Task Priority                  */
                            0x1000,                                 /* Stack Size _4KB                */
                            (unsigned (__TASKCONV *)(void*))cmdAppTrunkCascadeTestTask,     /* Starting Point */
                            (GT_VOID*)NULL,                         /* there is no arguments */
                            &taskId);                              /* returned task ID */
        if(rc != GT_OK)
        {
            return GT_FAIL;
        }

        enterOnce = GT_TRUE;
    }

    return rc;
}
