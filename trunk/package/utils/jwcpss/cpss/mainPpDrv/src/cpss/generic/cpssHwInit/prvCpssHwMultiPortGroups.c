/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssHwMultiPortGroups.c
*
* DESCRIPTION:
*       Private API definition for multi-port-groups devices utilities.
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*
*******************************************************************************/

#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpssCommon/private/prvCpssMath.h>


/*******************************************************************************
* portGroupBusyWait
*
* DESCRIPTION:
*       function do 'Busy wait' on a bit ,until cleared.
*       function wait on the register in the specified port group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port group devices that need to access
*                specific port group
*       regAddr - the register address to query
*       selfClearBit - the bit to query
*       onlyCheck - do we want only to check the current status , or to wait
*                   until ready
*                   GT_TRUE - check status , without busy wait
*                   GT_FALSE - loop on the bit until cleared
*
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
static GT_STATUS portGroupBusyWait
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   selfClearBit,
    IN GT_BOOL  onlyCheck
)
{
    GT_STATUS   rc;
    GT_U32  busyIterator; /* busy wait iterator */
    GT_U32  regValue;     /* current register value*/

    busyIterator = 0;
    /* use "busy" wait */
    do
    {
        /* query the needed bit in the needed register in the specified port group*/
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, selfClearBit, 1, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* check that number of iteration not over the limit */
        PRV_CPSS_MAX_NUM_ITERATIONS_CHECK_CNS(devNum,(busyIterator++));
    }
    while(regValue == 1 && (onlyCheck == GT_FALSE));

    if(regValue == 1)
    {
        /* the case of (onlyCheck == GT_FALSE) would have already returned GT_TIMEOUT
            from the macro of PRV_CPSS_MAX_NUM_ITERATIONS_CHECK_CNS */
        return /* do not register as error (up to the caller to register error if needed) */ GT_BAD_STATE;
    }

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */
    GT_BOOL isAdditionalRegDupNeeded;
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup;
    GT_U32  ii;

    if(GT_FALSE ==
        prvCpssDuplicatedMultiPortGroupsGet(devNum,portGroupId,regAddr,&portGroupsBmp,
           &isAdditionalRegDupNeeded,&additionalRegDup))
    {
        if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        else
        {
            portGroupsBmp = BIT_0 << portGroupId;
        }
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = portGroupBusyWait(devNum,portGroupId,regAddr,selfClearBit,onlyCheck);
        if(rc != GT_OK)
        {
            return rc;
        }
        if (isAdditionalRegDupNeeded == GT_TRUE)
        {
            for(ii = 0 ; ii < additionalRegDup.numOfAdditionalAddresses; ii++)
            {
                rc = portGroupBusyWait(devNum,portGroupId,additionalRegDup.additionalAddressesArr[ii],selfClearBit,onlyCheck);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* portGroupBusyWaitByMask
*
* DESCRIPTION:
*       function do 'Busy wait' on specific mask of the register.
*       function wait on the register in the specified port group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum  - the device number
*       portGroupId - the port group Id , to support multi-port group devices that need to access
*                specific port group
*       regAddr - the register address to query
*       regMask - the mask of the register that we wait for to clear
*       onlyCheck - do we want only to check the current status , or to wait
*                   until ready
*                   GT_TRUE - check status , without busy wait
*                   GT_FALSE - loop on the value until value reached
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
static GT_STATUS portGroupBusyWaitByMask
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN GT_U32   regAddr,
    IN GT_U32   regMask,
    IN GT_BOOL  onlyCheck
)
{
    GT_STATUS   rc;
    GT_U32  busyIterator; /* busy wait iterator */
    GT_U32  regValue;     /* current register value*/

    busyIterator = 0;
    /* use "busy" wait */
    do
    {
        /* query the needed bit in the needed register in the specified port group*/
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,regAddr, &regValue);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* check that number of iteration not over the limit */
        PRV_CPSS_MAX_NUM_ITERATIONS_CHECK_CNS(devNum,(busyIterator++));
    }
    while((regValue & regMask) && (onlyCheck == GT_FALSE));

    if(regValue & regMask)
    {
        /* the case of (onlyCheck == GT_FALSE) would have already returned GT_TIMEOUT
            from the macro of PRV_CPSS_MAX_NUM_ITERATIONS_CHECK_CNS */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

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
)
{
    GT_STATUS rc;
    GT_PORT_GROUPS_BMP  portGroupsBmp;/*port groups bmp */
    GT_BOOL isAdditionalRegDupNeeded;
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup;
    GT_U32  ii;

    if(GT_FALSE ==
        prvCpssDuplicatedMultiPortGroupsGet(devNum,portGroupId,regAddr,&portGroupsBmp,
           &isAdditionalRegDupNeeded,&additionalRegDup))
    {
        if(portGroupId == CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            portGroupsBmp = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
        }
        else
        {
            portGroupsBmp = BIT_0 << portGroupId;
        }
    }

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = portGroupBusyWaitByMask(devNum,portGroupId,regAddr,regMask,onlyCheck);
        if(rc != GT_OK)
        {
            return rc;
        }
        if (isAdditionalRegDupNeeded == GT_TRUE)
        {
            for(ii = 0 ; ii < additionalRegDup.numOfAdditionalAddresses; ii++)
            {
                rc = portGroupBusyWaitByMask(devNum,portGroupId,additionalRegDup.additionalAddressesArr[ii],regMask,onlyCheck);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* __boolOp
*
* DESCRIPTION:
*       function calculates boolean operation result
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       op  - 4-bit table of boolean funcion of two boolean variables
*       a   - The first variable.
*       b   - The second variable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       boolean operation result
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_U32 __boolOp
(
    IN  GT_U32 op,
    IN  GT_U32 a,
    IN  GT_U32 b
)
{
    return ((op >> ((2 * a) + b)) & 1);
}

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
)
{
    GT_STATUS   rc;           /* return code               */
    GT_U32      busyIterator; /* busy wait iterator        */
    GT_U32      regValue;     /* current register value    */
    GT_U32      i;            /* loop index                */
    GT_U32      result;       /* 0 - GT_FALSE, 1 - GT_TRUE */
    GT_U32      res1;         /* 0 - GT_FALSE, 1 - GT_TRUE */

    busyIterator = 0;
    /* use "busy" wait */
    do
    {
        /* calculate checked condition */
        result = 1;
        for (i = 0; (i < regNum); i++)
        {
            res1 = __boolOp(booleanOpArr[i], result, 0);
            if (res1 == __boolOp(booleanOpArr[i], result, 1))
            {
                /* the next result not depend from condition */
                /* currently being checked                   */
                /* skip register value checking              */
                result = res1;
                continue;
            }

            rc = prvCpssDrvHwPpPortGroupReadRegister(
                devNum, portGroupId, regAddrArr[i], &regValue);
            if (rc != GT_OK)
            {
                return rc;
            }

            res1    = ((regValue & regMaskArr[i]) == regPatternArr[i]) ? 1 : 0;
            result  = __boolOp(booleanOpArr[i], result, res1);
        }

        /* check that number of iteration not over the limit */
        PRV_CPSS_MAX_NUM_ITERATIONS_CHECK_CNS(devNum,(busyIterator++));
    }
    while((result != 0) && (onlyCheck == GT_FALSE));

    if (result == 0)
    {
        /* the case of (onlyCheck == GT_FALSE) would have already returned GT_TIMEOUT
            from the macro of PRV_CPSS_MAX_NUM_ITERATIONS_CHECK_CNS */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

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
)
{
    GT_STATUS  rc;
    GT_U64     summaryValue; /* summary from all port groups for counter */
    GT_U64     coreValue;    /* value of the register from current port group */
    GT_U32     portGroupId;  /* the port group Id - support multi port groups device */
    GT_PORT_GROUPS_BMP    portGroupsBmpValue; /* bitmap of Port Groups */
    GT_BOOL isAdditionalRegDupNeeded;
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup;
    GT_U32  ii;


    /* This manipulation is needed for multiportgroup devices
       where counter may be not in all groups */
    if(GT_TRUE  == prvCpssDuplicatedMultiPortGroupsGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS ,regAddr, &portGroupsBmpValue,
        &isAdditionalRegDupNeeded,&additionalRegDup))
    {
        if (portGroupsBmp != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            /*  check that portGroupsBmp has only existing
                port groups for the regAddr */
            if((portGroupsBmpValue | portGroupsBmp) != portGroupsBmpValue)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        /*  in this case value should be read only from existing
            port groups for counter */
        else
        {
            portGroupsBmp = portGroupsBmpValue;
        }
    }

    coreValue.l[1] = 0;/* not support read of 64 bit counters */
    summaryValue.l[0] = 0;
    summaryValue.l[1] = 0;

    /* loop on all port groups to get the counter value from */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                                regAddr, fieldOffset,
                                                fieldLength, &coreValue.l[0]);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* summary the counter from all the port groups */
        summaryValue = prvCpssMathAdd64(summaryValue,coreValue);

        if(isAdditionalRegDupNeeded == GT_TRUE)
        {
            for(ii = 0 ; ii < additionalRegDup.numOfAdditionalAddresses; ii++)
            {
                rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,
                                                        additionalRegDup.additionalAddressesArr[ii], fieldOffset,
                                                        fieldLength, &coreValue.l[0]);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* summary the counter from all the port groups */
                summaryValue = prvCpssMathAdd64(summaryValue,coreValue);
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    if(counter32bitValuePtr)
    {
        *counter32bitValuePtr = summaryValue.l[0];
    }

    if(counter64bitValuePtr)
    {
        *counter64bitValuePtr = summaryValue;
    }

    return GT_OK;

}

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
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    OUT GT_U32 *counter32bitValuePtr,
    OUT GT_U64 *counter64bitValuePtr
)
{

    return prvCpssPortGroupsBmpCounterSummary(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               regAddr, fieldOffset, fieldLength,
                                               counter32bitValuePtr,
                                               counter64bitValuePtr);
}

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
)
{
    GT_STATUS           rc;
    GT_U32              summaryValue1; /* summary from all port groups for counter# 1*/
    GT_U32              summaryValue2; /* summary from all port groups for counter# 2*/
    GT_U32              summaryValue3; /* summary from all port groups for counter# 3*/
    GT_U32              coreValue;     /* value of the register from current port group */
    GT_U32              portGroupId;    /* the port group Id - support multi port groups device */
    GT_PORT_GROUPS_BMP  portGroupsBmpValue; /* bitmap of Port Groups */
    GT_BOOL isAdditionalRegDupNeeded;
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup;
    GT_U32  ii;

    /* This manipulation is needed for multiportgroup devices
       where counter may be not in all groups */
    if(GT_TRUE  == prvCpssDuplicatedMultiPortGroupsGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS ,regAddr, &portGroupsBmpValue,
        &isAdditionalRegDupNeeded,&additionalRegDup))
    {
        if (portGroupsBmp != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            /*  check that portGroupsBmp has only existing
                port groups for the regAddr */
            if((portGroupsBmpValue | portGroupsBmp) != portGroupsBmpValue)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        /*  in this case value should be read only from existing
            port groups for counter */
        else
        {
            portGroupsBmp = portGroupsBmpValue;
        }
    }

    summaryValue1 = 0;
    summaryValue2 = 0;
    summaryValue3 = 0;


    /* loop on all port groups to get the counter value from */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&coreValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* summary the counter from all the port groups */
        summaryValue1 += U32_GET_FIELD_MAC(coreValue,fieldOffset1,fieldLength1);
        /* summary the counter from all the port groups */
        summaryValue2 += U32_GET_FIELD_MAC(coreValue,fieldOffset2,fieldLength2);
        /* summary the counter from all the port groups */
        summaryValue3 += U32_GET_FIELD_MAC(coreValue,fieldOffset3,fieldLength3);


        if(isAdditionalRegDupNeeded == GT_TRUE)
        {
            for(ii = 0 ; ii < additionalRegDup.numOfAdditionalAddresses; ii++)
            {
                rc = prvCpssDrvHwPpPortGroupReadRegister(devNum,portGroupId,additionalRegDup.additionalAddressesArr[ii],&coreValue);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* summary the counter from all the port groups */
                summaryValue1 += U32_GET_FIELD_MAC(coreValue,fieldOffset1,fieldLength1);
                /* summary the counter from all the port groups */
                summaryValue2 += U32_GET_FIELD_MAC(coreValue,fieldOffset2,fieldLength2);
                /* summary the counter from all the port groups */
                summaryValue3 += U32_GET_FIELD_MAC(coreValue,fieldOffset3,fieldLength3);
            }
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    if(counterValue1Ptr)
    {
        *counterValue1Ptr = summaryValue1;
    }

    if(fieldOffset2 && fieldLength2 && counterValue2Ptr)
    {
        *counterValue2Ptr = summaryValue2;
    }

    if(fieldOffset3 && fieldLength3 && counterValue3Ptr)
    {
        *counterValue3Ptr = summaryValue3;
    }

    return GT_OK;
}

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
)
{
    return prvCpssPortGroupsBmpMultiCounterSummary(
                                        devNum,
                                        CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                        regAddr,
                                        fieldOffset1,
                                        fieldLength1,
                                        counterValue1Ptr,
                                        fieldOffset2,
                                        fieldLength2,
                                        counterValue2Ptr,
                                        fieldOffset3,
                                        fieldLength3,
                                        counterValue3Ptr);
}

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
)
{
    GT_U16    cntrMod;       /* reminder from (cntr / portGroupNum) */
    GT_U64    w64;           /* work variable                       */

    /* calculate average counter value */
    cntrMod = prvCpssMathMod64By16(counterValue, portGroupNum);
    *averagePtr = prvCpssMathDiv64By16(counterValue, portGroupNum);

    /* distribute reminder withing first port groups.
      E.g. counter value is 7 will be distributed within
     4 port groups as next:
      port group   output counter value
        0                2
        1                2
        2                2
        3                1*/
    if (portGroupIndex < cntrMod)
    {
        /* add one to average */
        w64.l[1] = 0;
        w64.l[0] = 1;
        *averagePtr = prvCpssMathAdd64(*averagePtr, w64);
    }
}

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
)
{
    GT_STATUS   rc;
    GT_U32  regValue;    /* register value */
    GT_U32  portGroupId; /* the port group Id - support multi port groups device */
    GT_PORT_GROUPS_BMP    portGroupsBmpValue; /* bitmap of Port Groups */
    GT_U32  portGroupNum;   /* number of port groups */
    GT_U32  portGroupIdx;   /* port group index      */
    GT_U32  counterAverage; /* counter average       */
    GT_U32  counterMod;     /* counter modulo        */
    GT_BOOL isAdditionalRegDupNeeded;
    PRV_CPSS_DUPLICATED_ADDITIONAL_ADDRESSES_INFO_STC  additionalRegDup;
    GT_U32  ii;

    /* This manipulation is needed for multiportgroup devices
       where counter may be not in all groups */
    if(GT_TRUE  == prvCpssDuplicatedMultiPortGroupsGet(
        devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS ,regAddr, &portGroupsBmpValue,
        &isAdditionalRegDupNeeded,&additionalRegDup))
    {
        if (portGroupsBmp != CPSS_PORT_GROUP_UNAWARE_MODE_CNS)
        {
            /*  check that portGroupsBmp has only existing
                port groups for the regAddr */
            if((portGroupsBmpValue | portGroupsBmp) != portGroupsBmpValue)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        /*  in this case value should be read only from existing
            port groups for counter */
        else
        {
            portGroupsBmp = portGroupsBmpValue;
        }
    }

    /* amount of port groups */
    portGroupNum = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        portGroupNum ++;
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    /* must never occure, but if occures    */
    /*due to bug in caller causes exception */
    if (portGroupNum == 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(isAdditionalRegDupNeeded == GT_TRUE)
    {
        /* each core spread it's value to all duplicated units */
        portGroupNum *= (1 + additionalRegDup.numOfAdditionalAddresses);
    }

    /* average and modulo vor value dispersion */
    counterAverage = counterValue / portGroupNum;
    counterMod     = counterValue % portGroupNum;


    /* loop on all port groups to get the counter value from */
    portGroupIdx = 0;
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)
    {
        /* value to store */
        regValue = (portGroupIdx < counterMod)
            ? (counterAverage + 1)
            : counterAverage;

        /* index increment */
        portGroupIdx ++;


        if (fieldOffset == 0 && fieldLength == 32)
        {
            /* avoid the 'read' action of the 'set field' action , because it
               may cause unnecessary 'active memory read' action on the register */
            rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                      regAddr, regValue);
        }
        else
        {
            rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                    regAddr, fieldOffset,
                                                    fieldLength, regValue);
        }

        if(rc != GT_OK)
        {
            return rc;
        }

        if(isAdditionalRegDupNeeded == GT_TRUE)
        {
            for(ii = 0 ; ii < additionalRegDup.numOfAdditionalAddresses; ii++)
            {
                /* value to store */
                regValue = (portGroupIdx < counterMod)
                    ? (counterAverage + 1)
                    : counterAverage;

                /* index increment */
                portGroupIdx ++;

                if (fieldOffset == 0 && fieldLength == 32)
                {
                    /* avoid the 'read' action of the 'set field' action , because it
                       may cause unnecessary 'active memory read' action on the register */
                    rc = prvCpssDrvHwPpPortGroupWriteRegister(devNum, portGroupId,
                                                              additionalRegDup.additionalAddressesArr[ii], regValue);
                }
                else
                {
                    rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId,
                                                            additionalRegDup.additionalAddressesArr[ii], fieldOffset,
                                                            fieldLength, regValue);
                }

                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }

    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(
        devNum, portGroupsBmp, portGroupId)

    return GT_OK;

}

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
    IN GT_U8 devNum,
    IN GT_U32 regAddr,
    IN GT_U32 fieldOffset,
    IN GT_U32 fieldLength,
    IN GT_U32 counterValue
)
{
    return prvCpssPortGroupsBmpCounterSet(devNum,
                                              CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                              regAddr, fieldOffset,
                                              fieldLength, counterValue);
}


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
)
{
    GT_U32  rc = GT_BAD_PARAM; /* 'Bad param' when numberOfWords = 0*/
    GT_U32  ii; /* iterator */

    for(ii = 0 ; ii < numberOfWords; ii++)
    {
        /* set each word with needed value */
        rc = prvCpssPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                                startAddr + (4 * ii), 0, 32,
                                                counterValueArr[ii]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return rc;

}

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
)
{
    return prvCpssPortGroupsBmpCounterArraySet(devNum,
                                                   CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                   startAddr,
                                                   numberOfWords,
                                                   counterValueArr);
}

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
)
{
    GT_U32  portGroupId;/*the port group Id - support multi port groups device */

    if(!PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) ||
        (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup ==
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if(0 == (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (1<<(currentPortGroupId))))
    {
        /* non active current port group */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(currentPortGroupId == PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup)
    {
        *nextPortGroupIdPtr = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
        return GT_OK;
    }

    *nextPortGroupIdPtr =  PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup;

    /* loop on all port groups , in reverse operation */
    for(portGroupId = (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup - 1);
        portGroupId != PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;
        portGroupId--)
    {
        if(0 == (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (1<<(portGroupId))))
        {
            continue;
        }

        if(portGroupId == currentPortGroupId)
        {
            break;
        }

        /* update the 'Next portGroupId' */
        *nextPortGroupIdPtr = portGroupId;
    }

    return GT_OK;
}


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
)
{
    GT_U32  portGroupId;/*the port group Id - support multi port groups device */

    if(!PRV_CPSS_IS_MULTI_PORT_GROUPS_DEVICE_MAC(devNum) ||
        (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup ==
            PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    if(0 == (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (1<<(currentPortGroupId))))
    {
        /* non active current port group */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(currentPortGroupId == PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup)
    {
        *prevPortGroupIdPtr = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup;
        return GT_OK;
    }

    *prevPortGroupIdPtr =  PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup;

    /* loop on all port groups */
    for(portGroupId = (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.firstActivePortGroup + 1);
        portGroupId != PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.lastActivePortGroup;
        portGroupId++)
    {
        if(0 == (PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp & (1<<(portGroupId))))
        {
            continue;
        }

        if(portGroupId == currentPortGroupId)
        {
            break;
        }

        /* update the 'Previous portGroupId' */
        *prevPortGroupIdPtr = portGroupId;
    }

    return GT_OK;
}


