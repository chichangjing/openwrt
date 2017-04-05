/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssHwMultiPortGroups.h
*
* DESCRIPTION:
*       Private API definition for multi-port-groups devices utilities.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

#ifndef __prvCpssHwMultiPortGroupsh
#define __prvCpssHwMultiPortGroupsh

#include <cpss/generic/cpssTypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Bool function tables for prvCpssPortGroupRegistersStateBusyWait */
#define PRV_CPSS_HW_BOOL_FUNCTION_TABLE_AND_CNS 0x8
#define PRV_CPSS_HW_BOOL_FUNCTION_TABLE_OR_CNS 0xE

/*******************************************************************************
* prvCpssPortGroupBusyWait
*
* DESCRIPTION:
*       function do 'Busy wait' on a bit ,until cleared.
*       function wait on the register in the specified port group.
*       if portGroupId = 'unaware' the function loop on all active
*       port groups and wait for all of them to finish
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum       - the device number
*       portGroupId  - The port group id.
*                      relevant only to 'multi-port-group' devices.
*                      supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr      - the register address to query
*       selfClearBit - the bit to query
*       onlyCheck    - do we want only to check the current status , or to wait
*                      until ready
*                      GT_TRUE - check status , without busy wait
*                      GT_FALSE - loop on the bit until cleared , on all port-groups
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_TIMEOUT  - when  onlyCheck == GT_FALSE and the busy wait time expired
*       GT_BAD_STATE - when onlyCheck == GT_TRUE and the bit was not cleared
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupBusyWait
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   selfClearBit,
    IN GT_BOOL  onlyCheck
);

/*******************************************************************************
* prvCpssPortGroupBusyWaitByMask
*
* DESCRIPTION:
*       function do 'Busy wait' on specific mask of the register.
*       function wait on the register in the specified port group.
*       if portGroupId = 'unaware' the function loop on all active
*       port groups and wait for all of them to finish
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx..
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId   - The port group id.
*                      relevant only to 'multi-port-group' devices.
*                      supports value CPSS_PORT_GROUP_UNAWARE_MODE_CNS
*       regAddr - the register address to query
*       regMask - the mask of the register that we wait for to clear
*       onlyCheck - do we want only to check the current status , or to wait
*                   until ready
*                   GT_TRUE - check status , without busy wait
*                   GT_FALSE - loop on the value until value reached , on all port-groups
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_TIMEOUT  - when  onlyCheck == GT_FALSE and the busy wait time expired
*       GT_BAD_STATE - when onlyCheck == GT_TRUE and the value was not reached
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupBusyWaitByMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   regMask,
    IN GT_BOOL  onlyCheck
);

/*******************************************************************************
* prvCpssPortGroupRegistersStateBusyWait
*
* DESCRIPTION:
*       function do 'Busy wait' on specified set of registers for the specified state.
*       function wait on the register in the specified port group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum        - the device number
*       portGroupId   - the port group Id , to support multi-port group devices that need to access
*                       specific port group
*       regNum        - amount of registers to check
*       regAddrArr    - array of the register addresses to query
*       regMaskArr    - array of the masks of the registers that
*                        we wait to get values specified in pattern
*       regPatternArr - array of the pattern values of the registers
*       booleanOpArr  -  array of logical operations
*                       Boolean operation is table of function F(a,b)
*                       where a and b in the set of {0,1} and result also 0 or 1.
*                       It is 4-bit value.
*                       F(0,0)=bit0, F(0,1)=bit1, F(1,0)=bit2, F(1,1)=bit3,
*                       i.e. F(a,b) = ((F >> ((2 * a) + b)) & 1);
*                       For example OR is 0xE, AND is 0x8
*                       The state check algorithm is follows:
*                       The result initialized as 1 (i.e. GT_TRUE)
*                       and in loop by index from 0 to (regNum - 1)
*                       result recalculated:
*                          result = booleanOp[index](
*                             result,
*                             ((value & regMaskArr[index]) == regPatternArr[index]))
*                          where value retrieved from register regAddrArr[index].
*                       Note0: there are cases when for result recalculation
*                          not needed to read register: when F(result, 0) == F(result, 1)
*                       in these cases register should not be read.
*                       Note1: Application should typically specify
*                       booleanOp[0]==0x8 (i.e. AND)
*       onlyCheck     - do we want only to check the current status,
*                       or to wait until ready
*                       GT_TRUE - check status , without busy wait
*                       GT_FALSE - loop on the value until value reached
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_TIMEOUT  - when  onlyCheck == GT_FALSE and the busy wait time expired
*       GT_BAD_STATE - when onlyCheck == GT_TRUE and the value was not reached
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupRegistersStateBusyWait
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regNum,
    IN GT_U32   regAddrArr[],
    IN GT_U32   regMaskArr[],
    IN GT_U32   regPatternArr[],
    IN GT_U32   booleanOpArr[],
    IN GT_BOOL  onlyCheck
);

/*******************************************************************************
* prvCpssPortGroupsBmpCounterSummary
*
* DESCRIPTION:
*       summary the counters from all the port groups in the bitmap,
*       from the input register
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupsBmp  - bitmap of Port Groups.
*       regAddr - The register's address to read the counter value from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read from the register.
*
* OUTPUTS:
*       counter32bitValuePtr - (pointer to) the summary counter from the port groups, in 32 bits
*                               when NULL --> ignored
*       counter64bitValuePtr - (pointer to) the summary counter from the port groups, in 64 bits
*                               when NULL --> ignored
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsBmpCounterSummary
(
    IN GT_U8                 devNum,
    IN GT_PORT_GROUPS_BMP    portGroupsBmp,
    IN GT_U32                regAddr,
    IN GT_U32                fieldOffset,
    IN GT_U32                fieldLength,
    OUT GT_U32               *counter32bitValuePtr,
    OUT GT_U64               *counter64bitValuePtr
);

/*******************************************************************************
* prvCpssPortGroupsCounterSummary
*
* DESCRIPTION:
*       summary the counters from all the port groups , from the input register
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to read the counter value from.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be read from the register.
*
* OUTPUTS:
*       counter32bitValuePtr - (pointer to) the summary counter from the port groups, in 32 bits
*                               when NULL --> ignored
*       counter64bitValuePtr - (pointer to) the summary counter from the port groups, in 64 bits
*                               when NULL --> ignored
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsCounterSummary
(
    IN GT_U8   devNum,
    IN GT_U32  regAddr,
    IN GT_U32  fieldOffset,
    IN GT_U32  fieldLength,
    OUT GT_U32 *counter32bitValuePtr,
    OUT GT_U64 *counter64bitValuePtr
);


/*******************************************************************************
* prvCpssPortGroupsBmpMultiCounterSummary
*
* DESCRIPTION:
*       The function make summ of the counters for port group bitmap from the
*       input register. The register holds several (2 or 3) counters.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx..
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       portGroupsBmp   - bitmap of Port Groups.
*       regAddr         - The register's address to read the counter value from.
*       fieldOffset1    - The start bit number in the register - counter 1
*       fieldLength1    - The number of bits to be read from the register - counter 1
*       fieldOffset2    - The start bit number in the register - counter 2
*                         when 0 --> counter ignored
*       fieldLength2    - The number of bits to be read from the register - counter 2
*                         when 0 --> counter ignored
*       fieldOffset3    - The start bit number in the register - counter 3
*                         when 0 --> counter ignored
*       fieldLength3    - The number of bits to be read from the register - counter 3
*                         when 0 --> counter ignored
*
* OUTPUTS:
*       counterValue1Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 1
*       counterValue2Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 2
*       counterValue3Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 3
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsBmpMultiCounterSummary
(
    IN  GT_U8               devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  GT_U32              regAddr,
    IN  GT_U32              fieldOffset1,
    IN  GT_U32              fieldLength1,
    OUT GT_U32              *counterValue1Ptr,
    IN  GT_U32              fieldOffset2,
    IN  GT_U32              fieldLength2,
    OUT GT_U32              *counterValue2Ptr,
    IN  GT_U32              fieldOffset3,
    IN  GT_U32              fieldLength3,
    OUT GT_U32              *counterValue3Ptr
);

/*******************************************************************************
* prvCpssPortGroupsMultiCounterSummary
*
* DESCRIPTION:
*       summary the counters from all the port groups , from the input register.
*       The register hold several (2 or 3) counters
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to read the counter value from.
*       fieldOffset1 - The start bit number in the register - counter 1
*       fieldLength1 - The number of bits to be read from the register - counter 1.
*       fieldOffset2 - The start bit number in the register - counter 2
*                   when 0 --> counter ignored
*       fieldLength2 - The number of bits to be read from the register - counter 2.
*                   when 0 --> counter ignored
*       fieldOffset3 - The start bit number in the register - counter 3
*                   when 0 --> counter ignored
*       fieldLength3 - The number of bits to be read from the register - counter 3.
*                   when 0 --> counter ignored
*
* OUTPUTS:
*       counterValue1Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 1
*       counterValue2Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 2
*       counterValue3Ptr - (pointer to) the summary counter from the
*                           port groups , in 32 bits - counter 3
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsMultiCounterSummary
(
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset1,
    IN GT_U32 fieldLength1,
    OUT GT_U32 *counterValue1Ptr,
    IN GT_U32 fieldOffset2,
    IN GT_U32 fieldLength2,
    OUT GT_U32 *counterValue2Ptr,
    IN GT_U32 fieldOffset3,
    IN GT_U32 fieldLength3,
    OUT GT_U32 *counterValue3Ptr
);

/*******************************************************************************
* prvCpssPortGroupCounterAverageValueGet
*
* DESCRIPTION:
*   The function gets average value of counter for current port group.
*   The cntr defines input value of counter that will be distributed between
*   portGroupNum port groups. The distribution is fair between port groups.
*   In best case port groups get same value of counter = cntr / portGroupNum.
*   The reminder from division above is distributed between first port groups.
*   E.g. cntr = 7, 4 port groups
*      port group index   output counter value
*           0                2
*           1                2
*           2                2
*           3                1
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*        portGroupNum   - number of port groups
*        portGroupIndex - port group index
*        counterValue   - input value of counter
* OUTPUTS:
*        averagePtr     - (pointer to) average value of counter for portGroupIndex
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvCpssPortGroupCounterAverageValueGet
(
    IN  GT_U16                            portGroupNum,
    IN  GT_U16                            portGroupIndex,
    IN  GT_U64                            counterValue,
    OUT GT_U64                           *averagePtr
);

/*******************************************************************************
* prvCpssPortGroupsBmpCounterSet
*
* DESCRIPTION:
*       Set the counter on all the port groups in the bitmap,
*       from the input register.
*       The function set an average value of counter
*       to get the given value summing over
*       all port groups according to bitmap.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       portGroupsBmp  - bitmap of Port Groups.
*       regAddr - The register's address to set the counter value to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       counterValue - the counter value to set
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsBmpCounterSet
(
    IN GT_U8                 devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32                regAddr,
    IN GT_U32                fieldOffset,
    IN GT_U32                fieldLength,
    IN GT_U32                counterValue
);

/*******************************************************************************
* prvCpssPortGroupsCounterSet
*
* DESCRIPTION:
*       set the counter on all the port groups , from the input register
*       the function set first active port group with the needed value , and set other
*       port groups with value 0 --> so the summary will not count multi-values
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       regAddr - The register's address to set the counter value to.
*       fieldOffset - The start bit number in the register.
*       fieldLength - The number of bits to be written to register.
*       counterValue - the counter value to set
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsCounterSet
(
    IN GT_U8  devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 counterValue
);

/*******************************************************************************
* prvCpssPortGroupsBmpCounterArraySet
*
* DESCRIPTION:
*       set the counters on all the port groups in the bitmap, from the input register
*       the function set first active port group with the needed values , and set other
*       port groups with value 0 --> so the summary will not count multi-values
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum          - The PP device number to write to.
*       portGroupsBmp   - bitmap of Port Groups.
*       startAddr       - The register's start address to set the counters values to.
*       numberOfWords   - number of words to set (registers)
*       counterValueArr[] - (array of)the counters values to set
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - when  numberOfWords = 0
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsBmpCounterArraySet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               startAddr,
    IN GT_U32               numberOfWords,
    IN GT_U32               counterValueArr[]
);

/*******************************************************************************
* prvCpssPortGroupsCounterArraySet
*
* DESCRIPTION:
*       set the counters on all the port groups , from the input register
*       the function set first active port group with the needed values , and set other
*       port groups with value 0 --> so the summary will not count multi-values
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - The PP device number to write to.
*       startAddr - The register's start address to set the counters values to.
*       numberOfWords - number of words to set (registers)
*       counterValueArr[] - (array of)the counters values to set
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - when  numberOfWords = 0
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsCounterArraySet
(
    IN GT_U8 devNum,
    IN GT_U32 startAddr,
    IN GT_U32 numberOfWords,
    IN GT_U32 counterValueArr[]
);

/*******************************************************************************
* prvCpssPortGroupsNextPortGroupGet
*
* DESCRIPTION:
*       get the next active port group.
*       NOTE: the function 'wraparound' the port group meaning that when
*           currentPortGroupId == 3 --> the  *nextPortGroupPtr will be 0
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3; Puma3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Puma2.
*                      --ONLY for multi-port-group device (and more then single port-group)
*
* INPUTS:
*       devNum             - The PP device number
*       currentPortGroupId - the current port group id
*
* OUTPUTS:
*       nextPortGroupIdPtr  - (pointer to) the next port group id(may 'wraparound')
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - currentPortGroupId >= CPSS_MAX_PORT_GROUPS_CNS
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_NOT_APPLICABLE_DEVICE - non multi port groups device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsNextPortGroupGet
(
    IN  GT_U8  devNum,
    IN  GT_U32 currentPortGroupId,
    OUT GT_U32 *nextPortGroupIdPtr
);

/*******************************************************************************
* prvCpssPortGroupsPrevPortGroupGet
*
* DESCRIPTION:
*       get the portGroupId of the previous active port group.
*       NOTE: the function 'wraparound' the portGroupIds meaning that when
*             currentPortGroupId == 0 --> the  *prevPortGroupIdPtr will be 3
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3; Puma3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Puma2.
*                      --ONLY for multi-port-group device (and more then single port-group)
*
* INPUTS:
*       devNum             - The PP device number
*       currentPortGroupId - the current port group Id
*
* OUTPUTS:
*      prevPortGroupIdPtr - (pointer to) the previous portGroupId (may 'wraparound')
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM - currentPortGroupId >= CPSS_MAX_PORT_GROUPS_CNS
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_NOT_APPLICABLE_DEVICE - non multi port groups device
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
GT_STATUS prvCpssPortGroupsPrevPortGroupGet
(
    IN GT_U8   devNum,
    IN GT_U32  currentPortGroupId,
    OUT GT_U32 *prevPortGroupIdPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssHwMultiPortGroupsh */
