/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssPciDrv.h
*
* DESCRIPTION:
*       Includes PCI functions wrappers, for PCI device discovering and
*       configuration read/write operation.
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __cpssPciDrvh
#define __cpssPciDrvh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>


/*
 * typedef: enum GT_PCI_INT
 *
 * Description: Enumeration For PCI interrupt lines.
 *
 * Enumerations:
 *      GT_PCI_INT_A - PCI INT# A
 *      GT_PCI_INT_B - PCI INT# B
 *      GT_PCI_INT_C - PCI INT# C
 *      GT_PCI_INT_D - PCI INT# D
 */
typedef enum
{
    CPSS_EXTDRV_PCI_INT_A_E = 1,
    CPSS_EXTDRV_GT_PCI_INT_B_E,
    CPSS_EXTDRV_GT_PCI_INT_C_E,
    CPSS_EXTDRV_GT_PCI_INT_D_E
}CPSS_EXTDRV_PCI_INT_ENT;



/*******************************************************************************
* CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC
*
* DESCRIPTION:
*       This routine write register to the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*       data     - data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC)
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    IN  GT_U32  data
);



/*******************************************************************************
* CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC
*
* DESCRIPTION:
*       This routine read register from the PCI configuration space.
*
* INPUTS:
*       busNo    - PCI bus number.
*       devSel   - the device devSel.
*       funcNo   - function number.
*       regAddr  - Register offset in the configuration space.
*
* OUTPUTS:
*       dataPtr     - (pointer to)the read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC)
(
    IN  GT_U32  busNo,
    IN  GT_U32  devSel,
    IN  GT_U32  funcNo,
    IN  GT_U32  regAddr,
    OUT GT_U32  *dataPtr
);



/*******************************************************************************
* CPSS_EXTDRV_PCI_DEV_FIND_FUNC
*
* DESCRIPTION:
*       This routine returns the next instance of the given device (defined by
*       vendorId & devId).
*
* INPUTS:
*       vendorId - The device vendor Id.
*       devId    - The device Id.
*       instance - The requested device instance.
*
* OUTPUTS:
*       busNoPtr    - (pointer to)PCI bus number.
*       devSelPtr   - (pointer to)the device devSel.
*       funcNoPtr   - (pointer to)function number.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_PCI_DEV_FIND_FUNC)
(
    IN  GT_U16  vendorId,
    IN  GT_U16  devId,
    IN  GT_U32  instance,
    OUT GT_U32  *busNoPtr,
    OUT GT_U32  *devSelPtr,
    OUT GT_U32  *funcNoPtr
);



/*******************************************************************************
* CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intVecPtrPtr - (pointer)PCI interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC)
(
    IN  CPSS_EXTDRV_PCI_INT_ENT  pciInt,
    OUT void        **intVecPtr
);

/*******************************************************************************
* CPSS_EXTDRV_INT_MASK_GET_FUNC
*
* DESCRIPTION:
*       This routine return the PCI interrupt vector.
*
* INPUTS:
*       pciInt - PCI interrupt number.
*
* OUTPUTS:
*       intMaskPtr - (pointer to)PCI interrupt mask.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       PCI interrupt mask should be used for interrupt disable/enable.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_INT_MASK_GET_FUNC)
(
    IN  CPSS_EXTDRV_PCI_INT_ENT  pciInt,
    OUT GT_UINTPTR   *intMaskPtr
);

/*******************************************************************************
* CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC
*
* DESCRIPTION:
*       This function enables / disables the PCI writes / reads combining
*       feature.
*       Some system controllers support combining memory writes / reads. When a
*       long burst write / read is required and combining is enabled, the master
*       combines consecutive write / read transactions, if possible, and
*       performs one burst on the PCI instead of two. (see comments)
*
* INPUTS:
*       enWrCombine - GT_TRUE enables write requests combining.
*       enRdCombine - GT_TRUE enables read requests combining.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success,
*       GT_NOT_SUPPORTED    - if the controller does not support this feature,
*       GT_FAIL             - otherwise.
*
* COMMENTS:
*       1.  Example for combined write scenario:
*           The controller is required to write a 32-bit data to address 0x8000,
*           while this transaction is still in progress, a request for a write
*           operation to address 0x8004 arrives, in this case the two writes are
*           combined into a single burst of 8-bytes.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC)
(
    IN  GT_BOOL     enWrCombine,
    IN  GT_BOOL     enRdCombine
);

/*******************************************************************************
* CPSS_EXTDRV_PCI_DOUBLE_WRITE_FUNC
*
* DESCRIPTION:
*        This routine will write a 64-bit data  to given address
*
* INPUTS:
*        address - address to write to
*       word1 - the first half of double word to write (MSW)
*       word2 - the second half of double word to write (LSW)
*
* OUTPUTS:
*      none
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_PCI_DOUBLE_WRITE_FUNC)
(
    IN  GT_U32 address,
    IN  GT_U32 word1, 
    IN  GT_U32 word2
);

/*******************************************************************************
* CPSS_EXTDRV_PCI_DOUBLE_READ_FUNC
*
* DESCRIPTION:
*        This routine will read a 64-bit data  from given address
*
* INPUTS:
*        address - address to read from
*
* OUTPUTS:
*       data     -  pointer to place the received data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_PCI_DOUBLE_READ_FUNC)
(
    IN  GT_U32  address,
    OUT GT_U64  *dataPtr
);

/* CPSS_EXT_DRV_PCI_STC -
    structure that hold the "external driver PCI " functions needed be
    bound to cpss.
*/
typedef struct{
    CPSS_EXTDRV_PCI_CONFIG_WRITE_REG_FUNC     extDrvPciConfigWriteRegFunc;
    CPSS_EXTDRV_PCI_CONFIG_READ_REG_FUNC      extDrvPciConfigReadRegFunc;
    CPSS_EXTDRV_PCI_DEV_FIND_FUNC             extDrvPciDevFindFunc;
    CPSS_EXTDRV_PCI_INT_VEC_GET_FUNC          extDrvPciIntVecFunc;
    CPSS_EXTDRV_INT_MASK_GET_FUNC             extDrvPciIntMaskFunc;
    CPSS_EXTDRV_PCI_COMBINED_ACCESS_ENABLE_FUNC  extDrvPciCombinedAccessEnableFunc;
    CPSS_EXTDRV_PCI_DOUBLE_WRITE_FUNC         extDrvPciDoubleWriteFunc;
    CPSS_EXTDRV_PCI_DOUBLE_READ_FUNC          extDrvPciDoubleReadFunc;	
}CPSS_EXT_DRV_PCI_STC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssPciDrvh */


