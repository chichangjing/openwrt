/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssUnitGenArrDrv.c
*
* DESCRIPTION:
*       prvCpssUnitGenArrDrv.c array of fields driver
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/private/utils/prvCpssUnitGenArrDrv.h>

#define PRV_CPSS_GEB_DRV_DEBUG 1

GT_STATUS prvCpssDrvFldArrListInit
(
    INOUT PRV_CPSS_DXCH_ARR_DRV_STC  *drvPtr,
    IN    GT_U32                      fldListLen,
    IN    PRV_CPSS_DRV_FLD_ARR_INIT_STC  *fldInitListPtr
)
{
    GT_U32 i;
    PRV_CPSS_DRV_FLD_ARR_DEF_STC *fldPtr;
    PRV_CPSS_DRV_FLD_ARR_INIT_STC * intSeqPtr;  

    intSeqPtr = fldInitListPtr;
    for (i = 0 ; intSeqPtr->fldId != PRV_BAD_VAL; i++, intSeqPtr++)
    {
        if (intSeqPtr->fldId >= fldListLen)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
        fldPtr = &drvPtr->fldDefListPtr[intSeqPtr->fldId];

        fldPtr->isSupported          = GT_TRUE;
        fldPtr->regAddrOffsInStruct  = intSeqPtr->regDef.regAddrOffsInStruct;
        fldPtr->offs                 = intSeqPtr->regDef.offs;
        fldPtr->len                  = intSeqPtr->regDef.len;
        fldPtr->arrsize              = intSeqPtr->arrSize;
        fldPtr->minValue             = intSeqPtr->fldBnd.minValue;
        fldPtr->maxValue             = intSeqPtr->fldBnd.maxValue;
        fldPtr->namePtr              = intSeqPtr->namePtr;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssArrDrvInit
*
* DESCRIPTION:
*       Init general field arr oriented driver
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       drvPtr          - pointer to driver description
*       fldDefListPtr   - pointer to field declaration table
*       fldInitListPtr  - pointer to array of all supported list
*       totalFldNum     - total number of fields in driver (both spuurted and not) = table size
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvInit
(
    INOUT PRV_CPSS_DXCH_ARR_DRV_STC     *drvPtr,
    IN    PRV_CPSS_DRV_FLD_ARR_DEF_STC  *fldDefListPtr, /* memory where field definition shall be stored */
    IN    PRV_CPSS_DRV_FLD_ARR_INIT_STC *fldInitListPtr,
    IN    GT_U32                     totalFldNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DRV_FLD_ARR_DEF_STC *fldPtr;
    GT_U32    i;

    CPSS_NULL_PTR_CHECK_MAC(drvPtr);
    CPSS_NULL_PTR_CHECK_MAC(fldDefListPtr);
    CPSS_NULL_PTR_CHECK_MAC(fldInitListPtr);

    drvPtr->fldDefListPtr      = fldDefListPtr;
    drvPtr->totalFldNum        = totalFldNum;

    /* init field table to empty */
    for (i = 0 ; i < totalFldNum; i++)
    {
        fldPtr = &drvPtr->fldDefListPtr[i];

        fldPtr->isSupported          = GT_FALSE;
        fldPtr->regAddrOffsInStruct  = 0;
        fldPtr->offs                 = 0;
        fldPtr->len                  = 0;
        fldPtr->arrsize              = 0;
        fldPtr->minValue             = 0;
        fldPtr->maxValue             = 0;
        fldPtr->namePtr              = (GT_CHAR *)NULL;
    }

    /* init all suported fields */
    rc = prvCpssDrvFldArrListInit(drvPtr,totalFldNum,fldInitListPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssArrDrvFldAttributeArrSize
*
* DESCRIPTION:
*       Get field attribute size
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       drvPtr                - (pointer to) driver description
*       fldId                 - field id
*
* OUTPUTS:
*       sizePtr               - (pointer to) attribute size
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong fldId
*       GT_BAD_PTR               - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvFldAttributeArrSize
(
     IN   PRV_CPSS_DXCH_ARR_DRV_STC   *drvPtr,
     IN   GT_U32                       fldId,
     OUT  GT_U32                      *sizePtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(drvPtr);

    if (fldId >= drvPtr->totalFldNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    *sizePtr = drvPtr->fldDefListPtr[fldId].arrsize;
    return GT_OK;
}

/*******************************************************************************
* prvCpssArrDrvQeueryInit
*
* DESCRIPTION:
*       Init qeuery
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       queryPtr              - pointer to query description
*       drvPtr                - pointer to driver description
*       devNum                - device number
*       portGroupId           - port group id
*       regStructBaseAddrPtr  - base address of structure from where addresses of appropriate registers will be taken
*
* OUTPUTS:
*       queryPtr              - pointer to query description
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on bad pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvQeueryInit
(
    INOUT PRV_CPSS_DRV_FLD_ARR_QEUERY_STC *queryPtr,
    IN    PRV_CPSS_DXCH_ARR_DRV_STC       *drvPtr,
    IN    GT_U8                            devNum,
    IN    GT_U32                           portGroupId,
    IN    GT_U32                          *regStructBaseAddrPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(queryPtr);
    CPSS_NULL_PTR_CHECK_MAC(drvPtr);

    queryPtr->drvPtr               = drvPtr;
    queryPtr->devNum               = devNum;
    queryPtr->portGroupId          = portGroupId;
    queryPtr->regStructBaseAddrPtr = regStructBaseAddrPtr;
    return GT_OK;
}

/*******************************************************************************
* prvCpssArrDrvQeueryFldAddrGet
*
* DESCRIPTION:
*       Get reg address of field 
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       queryPtr              - pointer to query description
*       fldId                 - field id
*       fldIdx                - index of field in array
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on field id is out of scope of total field 
*       GT_NOT_SUPPORTED         - on field is not supoorted on specific implementation of driver
*       GT_OUT_OF_RANGE          - id is valid, field is suppprted, but value is put of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvQeueryFldAddrGet
(
    IN  PRV_CPSS_DRV_FLD_ARR_QEUERY_STC *queryPtr,
    IN  GT_U32                       fldId,
    IN  GT_U32                       fldIdx,
    OUT GT_U32                      *regAddrPtr
)
{
    GT_U32 *regAddrBasePtr;
    PRV_CPSS_DRV_FLD_ARR_DEF_STC * fldDefPtr;
    PRV_CPSS_DXCH_ARR_DRV_STC     *drvPtr;

    CPSS_NULL_PTR_CHECK_MAC(queryPtr);
    CPSS_NULL_PTR_CHECK_MAC(regAddrPtr);

    drvPtr = queryPtr->drvPtr;

    if (fldId >= drvPtr->totalFldNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fldDefPtr = &drvPtr->fldDefListPtr[fldId];
    if (GT_FALSE == fldDefPtr->isSupported)
    {
        #if (PRV_CPSS_GEN_DRV_DEBUG == 1)
            cpssOsPrintf("\n DRV : field is not supported : %d %s",fldId,fldDefPtr->namePtr);
        #endif
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if (fldIdx >= fldDefPtr->arrsize)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddrBasePtr = (GT_U32*)(fldDefPtr->regAddrOffsInStruct + (GT_CHAR *)queryPtr->regStructBaseAddrPtr);
    *regAddrPtr = regAddrBasePtr[fldIdx];
    return GT_OK;
}



/*******************************************************************************
* prvCpssArrDrvQeueryFldAddrValidCheck
*
* DESCRIPTION:
*       check reg address of field 
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       queryPtr              - pointer to query description
*       fldId                 - field id
*       fldIdx                - index of field in array
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on field id is out of scope of total field 
*       GT_NOT_SUPPORTED         - on field is not supoorted on specific implementation of driver
*       GT_OUT_OF_RANGE          - id is valid, field is suppprted, but value is put of range
*       GT_NOT_INITIALIZED       - reg addr is not initialized
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvQeueryFldAddrValidCheck
(
    IN  PRV_CPSS_DRV_FLD_ARR_QEUERY_STC *queryPtr,
    IN  GT_U32                       fldId,
    IN  GT_U32                       fldIdx
)
{
    GT_STATUS rc;
    GT_U32 regAddr;

    CPSS_NULL_PTR_CHECK_MAC(queryPtr);
    rc = prvCpssArrDrvQeueryFldAddrGet(queryPtr,fldId,fldIdx,/*OUT*/&regAddr);
    if (rc != GT_OK)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(rc, LOG_ERROR_NO_MSG);
    }

    if (regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return GT_NOT_INITIALIZED;
    }
    return GT_OK;
}


/*******************************************************************************
* prvCpssArrDrvQeueryFldSet
*
* DESCRIPTION:
*       Set field 
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       queryPtr              - pointer to query description
*       fldId                 - field id
*       fldIdx                - index of field in array
*       fldValue              - field value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on field id is out of scope of total field 
*       GT_NOT_SUPPORTED         - on field is not spuurted on specific implementation of driver
*       GT_OUT_OF_RANGE          - id is valid, field is suppprted, but value is put of range
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvQeueryFldSet
(
    IN  PRV_CPSS_DRV_FLD_ARR_QEUERY_STC *queryPtr,
    IN  GT_U32                       fldId,
    IN  GT_U32                       fldIdx,
    IN  GT_U32                       fldValue
)
{
    GT_STATUS rc;
    GT_U32 *regAddrBasePtr;
    GT_U32 regAddr;
    PRV_CPSS_DRV_FLD_ARR_DEF_STC * fldDefPtr;
    PRV_CPSS_DXCH_ARR_DRV_STC     *drvPtr;

    CPSS_NULL_PTR_CHECK_MAC(queryPtr);

    drvPtr = queryPtr->drvPtr;

    if (fldId >= drvPtr->totalFldNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fldDefPtr = &drvPtr->fldDefListPtr[fldId];
    if (GT_FALSE == fldDefPtr->isSupported)
    {
        #if (PRV_CPSS_GEN_DRV_DEBUG == 1)
            cpssOsPrintf("\n DRV : field is not supported : %d %s",fldId,fldDefPtr->namePtr);
        #endif
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if (fldIdx >= fldDefPtr->arrsize)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    if (fldDefPtr-> minValue > 0)
    {
        if (fldValue < fldDefPtr-> minValue)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }
    if (fldValue > fldDefPtr-> maxValue)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    regAddrBasePtr = (GT_U32*)(fldDefPtr->regAddrOffsInStruct + (GT_CHAR *)queryPtr->regStructBaseAddrPtr);
    regAddr = regAddrBasePtr[fldIdx];
    rc = prvCpssDrvHwPpPortGroupSetRegField(queryPtr->devNum,queryPtr->portGroupId,regAddr,
                                        fldDefPtr->offs,
                                        fldDefPtr->len,
                                        fldValue);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}

/*******************************************************************************
* prvCpssArrDrvQeueryFldGet
*
* DESCRIPTION:
*       Get the field 
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       queryPtr              - pointer to query
*       fldId                 - field id
*       fldIdx                - index of field in array
*
* OUTPUTS:
*       fldValue              - pointer to field value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on field id is out of scope of total field 
*       GT_NOT_SUPPORTED         - on field is not spuurted on specific implementation of driver
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvQeueryFldGet
(
    IN  PRV_CPSS_DRV_FLD_ARR_QEUERY_STC *queryPtr,
    IN  GT_U32                       fldId,
    IN  GT_U32                       fldIdx,   
    OUT GT_U32                      *fldValuePtr
)
{
    GT_STATUS rc;
    GT_U32 *regAddrBasePtr;
    GT_U32 regAddr;
    PRV_CPSS_DRV_FLD_ARR_DEF_STC * fldDefPtr;
    PRV_CPSS_DXCH_ARR_DRV_STC     *drvPtr;

    CPSS_NULL_PTR_CHECK_MAC(fldValuePtr);

    drvPtr = queryPtr->drvPtr;

    if (fldId >= drvPtr->totalFldNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fldDefPtr = &drvPtr->fldDefListPtr[fldId];
    if (GT_FALSE == fldDefPtr->isSupported)
    {
        #if (PRV_CPSS_GEN_DRV_DEBUG == 1)
            cpssOsPrintf("\n DRV : field is not supported : %d %s",fldId,fldDefPtr->namePtr);
        #endif
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if (fldIdx >= fldDefPtr->arrsize)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }


    regAddrBasePtr = (GT_U32*)(fldDefPtr->regAddrOffsInStruct + (GT_CHAR *)queryPtr->regStructBaseAddrPtr);
    regAddr = regAddrBasePtr[fldIdx];
    rc = prvCpssDrvHwPpPortGroupGetRegField(queryPtr->devNum,queryPtr->portGroupId,regAddr,
                                        fldDefPtr->offs,
                                        fldDefPtr->len,
                                        /*OUT*/fldValuePtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssArrDrvQeueryFldCheck
*
* DESCRIPTION:
*       Check field 
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       queryPtr              - pointer to query
*       fldId                 - field id
*       fldIdx                - index of field in array
*       fldValue              - field value
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on field id is out of scope of total field 
*       GT_NOT_SUPPORTED         - on field is not spuurted on specific implementation of driver
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssArrDrvQeueryFldCheck
(
    IN   PRV_CPSS_DRV_FLD_ARR_QEUERY_STC *queryPtr,
    IN   GT_U32                           fldId,
    IN   GT_U32                           fldIdx,
    OUT  GT_U32                           fldValue
)
{
    PRV_CPSS_DRV_FLD_ARR_DEF_STC * fldDefPtr;
    PRV_CPSS_DXCH_ARR_DRV_STC     *drvPtr;

    CPSS_NULL_PTR_CHECK_MAC(queryPtr);

    drvPtr = queryPtr->drvPtr;
    if (fldId >= drvPtr->totalFldNum)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    fldDefPtr = &drvPtr->fldDefListPtr[fldId];
    if (GT_FALSE == fldDefPtr->isSupported)
    {
        #if (PRV_CPSS_GEN_DRV_DEBUG == 1)
            cpssOsPrintf("\n DRV : field is not supported : %d %s",fldId,fldDefPtr->namePtr);
        #endif
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if (fldIdx >= fldDefPtr->arrsize)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    
    if (fldDefPtr-> minValue > 0)
    {
        if (fldValue < fldDefPtr-> minValue)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
        }
    }
    if (fldValue > fldDefPtr-> maxValue)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }
    return GT_OK;
}

