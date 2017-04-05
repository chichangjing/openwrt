/*******************************************************************************
*              (c), Copyright 2010, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* phyMacInit.h
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#include <gtOs/gtGenTypes.h>
#include <cpss/generic/phy/cpssGenPhySmi.h> 
#include <cpss/generic/cpssTypes.h> 
#include <cpss/generic/extMac/cpssExtMacDrv.h> 

/* PHY INFO data base unit per port*/
typedef struct {
        GT_U8   devNum;
        GT_PHYSICAL_PORT_NUM  portNum;
        CPSS_PHY_SMI_INTERFACE_ENT smiBus;
        GT_U32 smiAddr;
        GT_U32 phySlice;
} phy1540MInfo;


typedef struct {
    phy1540MInfo * phyInfoPtr;
    GT_U32 phyInfoDBSize;
} phy1540MDataBase;
            

/* smi write */
typedef GT_STATUS (* CPSS_SMI_REGISTER_WRITE_FUNC)
(
 IN  GT_U8   devNum,
 IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
 IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
 IN  GT_U32  smiAddr, 
 IN  GT_U32  regAddr,
 OUT GT_U16  data
);


/* smi read */
typedef GT_STATUS (* CPSS_SMI_REGISTER_READ_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN  CPSS_PHY_SMI_INTERFACE_ENT   smiInterface,
    IN  GT_U32  smiAddr, 
    IN  GT_U32  regAddr,
    OUT GT_U16  *data
);

/* start smi transaction */
typedef GT_STATUS (* CPSS_SMI_START_TRANSACTION_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL *pollStatus
);

/* finish smi transaction */
typedef GT_STATUS (* CPSS_SMI_FINISH_TRANSACTION_FUNC)
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL  pollStatus
);


/*******************************************************************************
* macDrv1540MInit
*
* DESCRIPTION:
*       - creates  PHYMAC 1540 object
*       - init of phy1540Info database
*       - bind of SMI read/write functions
*       - bind of the object unit functions
*
* INPUTS:
*       - ptr to SMI write function
*       - ptr to SMI read function
*       - ptr to phy1540Info per port array

*
* OUTPUTS:
*       returns pointer to the phyMac object
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAMETER - if phy data base is not correct
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS macDrv1540MInit
(
    IN  CPSS_SMI_REGISTER_WRITE_FUNC  smiWriteRegister,
    IN  CPSS_SMI_REGISTER_READ_FUNC   smiReadRegister,
    IN  CPSS_SMI_START_TRANSACTION_FUNC  smiStartTransaction,
    IN  CPSS_SMI_FINISH_TRANSACTION_FUNC  smiFinishTransaction,
    IN  phy1540MDataBase * phyMacDataBasePtr,
    OUT CPSS_MACDRV_OBJ_STC ** macDrvObjPtr
);



