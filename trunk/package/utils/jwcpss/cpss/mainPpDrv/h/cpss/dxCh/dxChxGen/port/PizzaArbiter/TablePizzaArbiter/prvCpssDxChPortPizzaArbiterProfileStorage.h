/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterProfileStorage.h
*
* DESCRIPTION:
*       CPSS user profile storage
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __PRV_CPSS_DXCH_PIZZAARBITER_PROFILESTORAGE_H
#define __PRV_CPSS_DXCH_PIZZAARBITER_PROFILESTORAGE_H

#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterActiverPortGroupList.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterTables.h>


typedef struct PizzaProfileStorage_STCT
{
    GT_U8                         devNum;
    PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC       portGroupList;  /* list of port greoups use the profile */
    CPSS_DXCH_PIZZA_PROFILE_STC   pizzaProfile;
    /*------------------------------------------*/
    /* usefull data structures for PizzaProfile */
    /*------------------------------------------*/
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC regModePizza;
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC extModePizza;
    CPSS_DXCH_PORT_PRIORITY_MATR_STC            portPriorityMatr;
    PortPizzaCfgSlicesListPool_STC      portPizzaCfgSliceListPool;
    /* usefull data structures for PizzaProfile */
}PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC;


/*******************************************************************************
* pizzaProfileStorageInit
*
* DESCRIPTION: 
*      init pizza profile storage
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       storagePtr  - pointer to Profile
*
* OUTPUTS:
*       storagePtr  - pointer to iterator
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - storagePtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pizzaProfileStorageInit
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC *storagePtr
);

/*******************************************************************************
* pizzaProfileStorageCopy
*
* DESCRIPTION: 
*      copy pizza for devNum, portGroupId into storage
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       storagePtr       - pointer to Profile
*       devNum           - dev number
*       portGroupListPtr - port group list for which the profile is used
*       pizzaProfilePtr  - (pointer to ) profile
*
* OUTPUTS:
*       storagePtr  - pointer to iterator
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pizzaProfileStorageCopy
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC *storagePtr, 
       IN GT_U8 devNum, 
       IN PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr,
       IN CPSS_DXCH_PIZZA_PROFILE_STC              *pizzaProfilePtr
);


#define PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_PROFILE_STORAGE_POOL_SIZE_CNS 8

typedef struct PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC
{
    GT_U32                    profileStorageCur;
    GT_U32                    profileStorageRemains;
    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC   pizzaProfileStorageArr[PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_PROFILE_STORAGE_POOL_SIZE_CNS];
}PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC;

/*-------------------------------------------------------------------
**    Interface to the structure PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC
**------------------------------------------------------------------*/
/*
GT_STATUS pizzaProfileStoragePoolInit (INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC * pPoolPtr);
GT_STATUS pizzaProfileStoragePoolFind (INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC * pPoolPtr, 
                                          IN GT_U8 devNum, 
                                          IN GT_U32 portGroupId,
                                         OUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC ** ppPizzaStoragePtrPtr);
GT_STATUS pizzaProfileStoragePoolAlloc(INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC * pPoolPtr,
                                         OUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC ** ppPizzaStoragePtrPtr);
*/

/*******************************************************************************
* pizzaProfileStoragePoolInit
*
* DESCRIPTION: 
*      init pizza profile storage pool
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr  - pointer to pool
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pizzaProfileStoragePoolInit 
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC *poolPtr
);

/*******************************************************************************
* pizzaProfileStoragePoolFind1
*
* DESCRIPTION: 
*      find pizza profile for devNum portGroupIdx
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       poolPtr  - pointer to pool
*       devNum   - device number
*       portGroupId - port grouop id
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*       ppPizzaStoragePtrPtr - (pointer to) pointer to storage found or NULL 
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*       GT_FAIL         - not find
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pizzaProfileStoragePoolFind1
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC *poolPtr, 
       IN GT_U8 devNum, 
       IN GT_U32 portGroupId,
      OUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC **ppPizzaStoragePtrPtr
);

/*******************************************************************************
* pizzaProfileStoragePoolAlloc
*
* DESCRIPTION: 
*      alloc place for pizza profile for devNum portGroupIdx
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr  - pointer to pool
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*       pizzaStoragePtrPtr - pointer to where the allocated address is stored
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*       GT_NO_MORE      - no place in pool 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS pizzaProfileStoragePoolAlloc
(
    INOUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC *poolPtr,
      OUT PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_STC     **pizzaStoragePtrPtr
);


typedef struct PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STCT
{
    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC * storagePoolPtr;
    GT_U32                                              curIdx;
}PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_ITER_STC;

/*-------------------------------------------------------------*/
/*  the structure to store the speed 2 slice conversion table  */
/*-------------------------------------------------------------*/
typedef struct PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STCT
{
    GT_U32                    cur;
    GT_U32                    remains;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC    arr[150];
}PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC;

/*-------------------------------------------------------------------
**    Interface to the structure PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC
**------------------------------------------------------------------*/
/*
GT_STATUS PortSpeed2SliceNumPoolInit (INOUT PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC * pPool);
GT_STATUS PortSpeed2SliceNumPoolAlloc(INOUT PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC * pPool, 
                                         IN GT_U32 size, 
                                        OUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC ** pItemList);
*/
/*******************************************************************************
* portSpeed2SliceNumPoolInit
*
* DESCRIPTION: 
*      init speed 2 slice convertion table storage pool
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr  - pointer to pool
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portSpeed2SliceNumPoolInit
(
    INOUT PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC *poolPtr
);

/*******************************************************************************
* portSpeed2SliceNumPoolAlloc
*
* DESCRIPTION: 
*      alloc speed 2 slice convertion table from storage pool
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr  - pointer to pool
*       size     - how many elements to be allocated  
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*       itemListPtrPtr - (pointer to) pointer on allocated items or NULL 
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - pPizza or ppSliceList is NULL
*       GT_NO_RESOURCE  - no place in pool 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS portSpeed2SliceNumPoolAlloc
(
    INOUT PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC *poolPtr, 
       IN GT_U32 size, 
      OUT CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC **itemListPtrPtr
);


typedef struct PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STCT
{
    GT_U8                                                  devNum;
    PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC               portGroupList;
    CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC   portSpeed2SlicesConvTbl;
    /**/
    PRV_CPSS_DXCH_PORT_SPEED_TO_SLICENUM_CONV_POOL_STC     portSpeed2SlicesConvPool;
}PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC;

/*
GT_STATUS SpeedConvTableStorageInit(INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC * storagePtr);
GT_STATUS SpeedConvTableStorageCopy(INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC * storagePtr, 
                                       IN GT_U8 devNum, 
                                       IN GT_U32 portGroupIdx,
                                       IN CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * portSpeed2SlicesConvTblPtr);
*/

/*******************************************************************************
* speedConvTableStorageInit
*
* DESCRIPTION: 
*      init speed comnversion table storage (PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC)
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       storagePtr  - pointer to Profile
*
* OUTPUTS:
*       storagePtr  - pointer to Profile
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - storagePtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS speedConvTableStorageInit
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC *storagePtr
);

/*******************************************************************************
* speedConvTableStorageCopy
*
* DESCRIPTION: 
*      copy the speed conversion table for <dev,port group >
*      into the storage place (PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC)
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*      storagePtr                   - pointer to Profile
*      devNum                       - device Id
*      portGroupListPtr             - port group list
*      portSpeed2SlicesConvTblPtr   - speed conversion table it self
*
* OUTPUTS:
*       storagePtr  - pointer to Profile
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - storagePtr or portSpeed2SlicesConvTblPtr is NULL
*       GT_NO_RESOURCE  - not enough  resources , consider increase constants
* COMMENTS:
*
*******************************************************************************/
GT_STATUS speedConvTableStorageCopy
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC *storagePtr, 
       IN GT_U8 devNum, 
       IN PRV_CPSS_DXCH_ACTIVE_PORT_GROUP_LIST_STC *portGroupListPtr,
       IN CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *portSpeed2SlicesConvTblPtr
);


#define PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_SPEEDCONV_STORAGE_POOL_SIZE_CNS 8

typedef struct PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC
{
    GT_U32                      storageCur;
    GT_U32                      storageRemains;
    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC   speedConvTableStorageArr[PRV_CPSS_DXCH_PORT_PIZZA_ARBITER_SPEEDCONV_STORAGE_POOL_SIZE_CNS];
}PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC;

/*
GT_STATUS SpeedConvTableStoragePoolInit (INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC * pPoolPtr);
GT_STATUS SpeedConvTableStoragePoolFind (INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC * pPoolPtr, 
                                            IN GT_U8 devNum, 
                                            IN GT_U32 portGroupId,
                                           OUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC ** ppSpeedConvTblStoragePtrPtr);
GT_STATUS SpeedConvTableStoragePoolAlloc(INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC * pPoolPtr,
                                           OUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC ** ppSpeedConvTblStoragePtrPtr);
*/
/*******************************************************************************
* speedConvTableStoragePoolInit
*
* DESCRIPTION: 
*      init pool speed conversion table storages to empty one
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr  - pointer to pool
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS speedConvTableStoragePoolInit
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC *poolPtr
);

/*******************************************************************************
* speedConvTableStoragePoolFind
*
* DESCRIPTION: 
*      search in pool speed conversion table storages for table 
*      for specific <dev , port group>
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       poolPtr      - pointer to pool
*       devNum       - device Id
*       portGroupId  - port group idx
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*       ppSpeedConvTblStoragePtrPtr  - pointer to place where the pointer to found table is stored;
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS speedConvTableStoragePoolFind
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC *poolPtr, 
       IN GT_U8 devNum, 
       IN GT_U32 portGroupId,
      OUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC **ppSpeedConvTblStoragePtrPtr
);

/*******************************************************************************
* speedConvTableStoragePoolAlloc
*
* DESCRIPTION: 
*      alloc place from pool of speed conversion table storages
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       poolPtr  - pointer to pool
*
* OUTPUTS:
*       poolPtr  - pointer to pool
*      ppSpeedConvTblStoragePtrPtr  - pointer to place where the pointer to alloced
*                                     storage is stored;
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*       GT_NO_RESOURCE  - no place at pool 
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS speedConvTableStoragePoolAlloc
(
    INOUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC *poolPtr,
      OUT PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_STC     **ppSpeedConvTblStoragePtrPtr
);



typedef struct PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STCT
{
    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC * storagePoolPtr;
    GT_U32                                          curIdx;
}PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_ITER_STC;

/*--------------------------------------------------------------*/
typedef struct PRV_CPSS_DXCH_PROFILE_SPEED_CONV_TBL_STORAGE_STCY
{
    GT_BOOL                                             isInit;
    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC   pizzaProfilePool;
    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC       speedConvTablePool;
}PRV_CPSS_DXCH_PROFILE_SPEED_CONV_TBL_STORAGE_STC;

/*
GT_STATUS UserProfileSpeedConvTableStorageInit(INOUT PRV_CPSS_DXCH_PROFILE_SPEED_CONV_TBL_STORAGE_STC * pStoragePtr);
*/

/*******************************************************************************
* userProfileSpeedConvTableStorageInit
*
* DESCRIPTION: 
*      init pools of user profile storage and speed conversion table storages to empty one
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       storagePtr  - pointer to pool
*
* OUTPUTS:
*       storagePtr  - pointer to pool
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PTR      - poolPtr is NULL
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS userProfileSpeedConvTableStorageInit
(
    INOUT PRV_CPSS_DXCH_PROFILE_SPEED_CONV_TBL_STORAGE_STC *storagePtr
);


/*******************************************************************************
* pizzaProfileStoragePoolGet
*
* DESCRIPTION: 
*      get user profile storage pool from global variable
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*    PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC* pointer to storage
* 
* COMMENTS:
*    if global storage g_usedProfileSpeedConvTableStorage (invisible for user)
*         is not initialized already, it is initilized first
*
*******************************************************************************/
PRV_CPSS_DXCH_PORT_PIZZA_PROFILE_STORAGE_POOL_STC   * pizzaProfileStoragePoolGet
(
    void
); 

/*******************************************************************************
* speedConvTableStoragePoolGet
*
* DESCRIPTION: 
*      get user spped conversion storage pool from global variable
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*    PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC* pointer to storage
* 
* COMMENTS:
*    if global storage g_usedProfileSpeedConvTableStorage (invisible for user)
*         is not initialized already, it is initilized first
*
*******************************************************************************/
PRV_CPSS_DXCH_SPEED_CONV_TBL_STORAGE_POOL_STC * speedConvTableStoragePoolGet
(
    void
);


/*******************************************************************************
* prvCpssDxChPizzaArbiterSelectSlicesBySpeed
*
* DESCRIPTION:
*       selection slice number by speed                       
*       shall be used in cpss blocks that uses Pizza Arbiter  
* 
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*       devNum       - device number
*       portGroupId  - port group
*       localPort    - local port
*       portSpeed    - port speed
*
* OUTPUTS:
*       sliceNumUsedPtr - number of slices to be used if speed is supported,  NULL othewise
*
* RETURNS:
*       GT_OK.              if speed is supported
*       GT_NOT_SUPPORTED    otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPizzaArbiterSelectSlicesBySpeed
( 
    IN GT_U8               devNum,
    IN GT_U32              portGroupId,
    IN GT_U32              localPort,
    IN CPSS_PORT_SPEED_ENT portSpeed,
   OUT GT_U32             *sliceNumUsedPtr
);



#endif

