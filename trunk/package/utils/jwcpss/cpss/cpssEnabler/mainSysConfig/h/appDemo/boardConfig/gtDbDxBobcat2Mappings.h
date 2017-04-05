/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxBobcat2Mappings.h
*
* DESCRIPTION:
*       Initialization Mapping and Port for the Bobcat2 - SIP5 - board.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __GT_DB_DX_BOBCAT2_MAPPING_H
#define __GT_DB_DX_BOBCAT2_MAPPING_H

#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>


/*******************************************************************************
* configBoardAfterPhase1 : relevant data structures for port mapping
*
*       This function performs all needed configurations that should be done
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define GT_NA ((GT_U32)~0)

typedef struct
{
    GT_U8                   boardRevId;
    CPSS_DXCH_PORT_MAP_STC *mappingPtr;
    GT_U32                  mappingSize;
}CPSS_DXCH_REVID_2_PORT_MAP_STC;



/*******************************************************************************
* configBoardAfterPhase1MappingGet
*
* DESCRIPTION:
*       This function gets relevant mapping for boardRevId,
*       if not found , selects default
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       revId2PortMapList - mapping list
*       listSize          - mapping list size
*
* OUTPUTS:
*       mappingPtrPtr   - pointer to found mapping
*       mappingPtrPtr   - pointer to mapping size
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PTR - on bad ptr
*       GT_BAD_PARAM - on empty list (list size = 0)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS configBoardAfterPhase1MappingGet
(
    IN  GT_U8                             boardRevId,
    OUT CPSS_DXCH_PORT_MAP_STC          **mappingPtrPtr,
    OUT GT_U32                           *mappingSizePtr
);


/*******************************************************************************
* bcat2PortInterfaceInit
*
*******************************************************************************/
#define APP_INV_PORT_CNS ((GT_U32)(~0))

typedef enum
{
     PORT_LIST_TYPE_EMPTY = 0
    ,PORT_LIST_TYPE_INTERVAL
    ,PORT_LIST_TYPE_LIST
}PortListType_ENT;

typedef struct
{
    PortListType_ENT             entryType;
    GT_PHYSICAL_PORT_NUM         portList[20];  /* depending on list type */
                                                /* interval : 0 startPort     */
                                                /*            1 stopPort      */
                                                /*            2 step          */
                                                /*            3 APP_INV_PORT_CNS */
                                                /* list     : 0...x  ports     */
                                                /*            APP_INV_PORT_CNS */
    CPSS_PORT_SPEED_ENT          speed;
    CPSS_PORT_INTERFACE_MODE_ENT interfaceMode;
}PortInitList_STC;

typedef struct
{
    GT_PHYSICAL_PORT_NUM         portNum;
    CPSS_PORT_SPEED_ENT          speed;
    CPSS_PORT_INTERFACE_MODE_ENT interfaceMode;
}PortInitInternal_STC;

typedef struct
{
    GT_U32                       coreClock;
    GT_U8                        boardRevId;
    CPSS_PP_SUB_FAMILY_TYPE_ENT  subFamily;
    GT_U32                       enabledPipesBmp;
    PortInitList_STC            *portsInitListPtr;
}boardRevId2PortsInitList_STC;

/*******************************************************************************
* bcat2PortInterfaceInitPortInitListGet
*
* DESCRIPTION:
*     Execute predefined ports configuration.
*
* INPUTS:
*       devNum     - devNum
*       boardRevId - board revision ID
*       coreClock  - core clock
*
* OUTPUTS:
*       portInitListPtrPtr     =  (pointer to) port init list
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bcat2PortInterfaceInitPortInitListGet
(
    IN  GT_U8                             devNum,
    IN  GT_U8                             boardRevId,
    IN  GT_U32                            coreClock,
    OUT PortInitList_STC                **portInitListPtrPtr
);

/*******************************************************************************
* appDemoDxChMaxMappedPortSet
*
* DESCRIPTION:
*   	Calculate and set maximal mapped port number,
*   	keep port mapping in appDemoPpConfigList DB.
*
* INPUTS:
*       dev         - device number
*       mapArrLen   - number of ports to map, array size
*       mapArrPtr   - pointer to array of mappings
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success,
*       GT_FAIL     - otherwise.
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM- on wrong mapAttLen
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoDxChMaxMappedPortSet
(
    IN  GT_U8  dev,
    IN  GT_U32 mapArrLen,
    IN  CPSS_DXCH_PORT_MAP_STC *mapArrPtr
);

/*******************************************************************************
* appDemoDxChMappedPortGet
*
* DESCRIPTION:
*   	Get port mapping from appDemoPpConfigList DB.
*
* INPUTS:
*       dev         - device number
*
* OUTPUTS:
*       mapArrLenPtr   - number of ports 
*       mapArrPtr      - ports mapping 
*
* RETURNS:
*       GT_OK       - on success,
*       GT_FAIL     - otherwise.
*       GT_BAD_PTR  - on NULL pointer
*       GT_BAD_PARAM- on wrong mapAttLen
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS appDemoDxChMaxMappedPortGet
(
    IN  GT_U8                   dev,
    OUT  GT_U32                 *mapArrLenPtr,
    OUT  CPSS_DXCH_PORT_MAP_STC **mapArrPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
