/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssUnitGenDrv.h
*
* DESCRIPTION:
*       prvCpssUnitGenDrv.c.
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * typedef: struct PRV_CPSS_DRV_FLD_ARR_DEF_STC
 *
 * Description: general driver field definiton structure
 *
 * Fields:
 *       isSupported           - whether the field is supported
 *       regAddrOffsInStruct   - offset of field's register address in some structure
 *       offs;                 - offset of field in register
 *       len;                  - len of field in register
 *       arrsize               - thereis array of reguisters of same type 
 *       minValue              - min value of field
 *       maxValue              - max value of field
 *       namePtr               - field name (for debugging purpose)
 */

typedef struct PRV_CPSS_DRV_FLD_ARR_DEF_STC
{
    GT_BOOL  isSupported;
    GT_U32   regAddrOffsInStruct;
    GT_U32   offs;
    GT_U32   len;
    GT_U32   arrsize;    /* thereis array of reguisters of same type */
    GT_U32   minValue;
    GT_U32   maxValue;
    GT_CHAR *namePtr;
}PRV_CPSS_DRV_FLD_ARR_DEF_STC;


/*
 * typedef: struct PRV_CPSS_DXCH_ARR_DRV_STC
 *
 * Description: general driver structure, contains info about fields and qeuery
 *
 * Fields:
 *       fldDefListPtr         - pointer to table of all defined registers
 *       totalFldNum           - number of entries in the table
 *       query                 - qeuery related information
 */
typedef struct PRV_CPSS_DXCH_ARR_DRV_STC
{
    PRV_CPSS_DRV_FLD_ARR_DEF_STC   *fldDefListPtr;
    GT_U32                          totalFldNum;
}PRV_CPSS_DXCH_ARR_DRV_STC;


/*
 * typedef: struct PRV_CPSS_DRV_FLD_QEUERY_STC
 *
 * Description: general driver qeuery contain info from where main paraemeters shall be taken
 *              for consequent read/write operations
 *
 * Fields:
 *       devNum                - device number
 *       portGroupId           - port group id
 *       regStructBaseAddrPtr  - address of structure of field's register address
 */
typedef struct PRV_CPSS_DRV_FLD_ARR_QEUERY_STC
{
    PRV_CPSS_DXCH_ARR_DRV_STC * drvPtr;
    GT_U8   devNum;
    GT_U32  portGroupId;
    GT_U32 *regStructBaseAddrPtr;
}PRV_CPSS_DRV_FLD_ARR_QEUERY_STC;


/*
 * typedef: struct PRV_CPSS_DRV_FLD_ARR_DEF_STC
 *
 * Description: record in list of supported structure, is used for initialization of corresponding table in driver
 *
 * Fields:
 *       regAddrOffsInStruct   - offeset of field's register address in some structure
 *       offs;                 - offset of field in register
 *       len;                  - len of field in register
 *       minValue              - min value of field
 *       maxValue              - max value of field
 *       namePtr               - field name (for debugging purpose)
 */
typedef struct PRV_CPSS_DRV_FLD_ARR_INIT_STCT
{
    GT_U32   fldId;
    struct REG_ARR_DEF_STC
    {
        GT_U32   regAddrOffsInStruct;
        GT_U32   offs;
        GT_U32   len;
    }regDef;
    GT_U32   arrSize;
    struct ARR_FLD_BND_STC
    {
        GT_U32   minValue;
        GT_U32   maxValue;
    }fldBnd;
    GT_CHAR *namePtr;
}PRV_CPSS_DRV_FLD_ARR_INIT_STC;

/*
 * typedef: PRV_BAD_VAL
 *
 * Description: in list of suppored fields this value is used to indicate end of list
 */
 #define PRV_BAD_VAL (GT_U32)(~0)


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
);

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
);

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
);


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
);


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
);


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
);

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
);

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
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

