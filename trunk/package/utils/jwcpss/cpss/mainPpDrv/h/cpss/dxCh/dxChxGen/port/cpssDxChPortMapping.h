/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortMapping.h
*
* DESCRIPTION:
*       CPSS physical port mapping
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*******************************************************************************/
#ifndef __CPSS_DXCH_PORT_MAPPING_H
#define __CPSS_DXCH_PORT_MAPPING_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define CPSS_DXCH_PORT_MAPPING_INVALID_PORT_CNS ((GT_U32)(~0))

/*
 * typedef: enum CPSS_DXCH_PORT_MAPPING_TYPE_ENT
 *
 * Description: interface types of physical ports.
 *
 * Enumerations:
 *      CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E - Ethernet MAC
 *      CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E     - CPU SDMA
 *      CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E - Interlaken Channel (all channels 0-63).
 *      CPSS_DXCH_PORT_MAPPING_TYPE_REMOTE_PHYSICAL_PORT_E  - Port represent physical interface on remote device or Interlaken channel.
 *      CPSS_DXCH_PORT_MAPPING_TYPE_MAX_E          - max definiton
 *
 */
typedef enum
 {
     CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E = 0
    ,CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E
    ,CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E
    ,CPSS_DXCH_PORT_MAPPING_TYPE_REMOTE_PHYSICAL_PORT_E
    ,CPSS_DXCH_PORT_MAPPING_TYPE_MAX_E
    ,CPSS_DXCH_PORT_MAPPING_TYPE_INVALID_E = ~0
}CPSS_DXCH_PORT_MAPPING_TYPE_ENT;


#define PRV_CPSS_DXCH_PORT_NUM_CPU_DMA_CNS            72
#define PRV_CPSS_DXCH_PORT_NUM_ILKN_DMA_CNS           64
#define PRV_CPSS_DXCH_PORT_NUM_ILKN_MAC_CNS           PRV_CPSS_DXCH_PORT_NUM_ILKN_DMA_CNS
#define PRV_CPSS_DXCH_PORT_NUM_TM_TxDMA_CNS           73
#define PRV_CPSS_DXCH_PORT_NUM_TM_TxQ_CNS             64

/*
 * Typedef: struct CPSS_DXCH_PORT_MAP_STC
 *
 * Description: General physical port map structure
 *
 * Fields:
 *      physicalPortNumber - physical port number
 *                           (APPLICABLE RANGES: 0..255)
 *      mappingType        - mapping type
 *      portGroup          - port group number of interface.
 *                           Relevant for multi port group devices only.
 *                           (APPLICABLE DEVICES: None)
 *      interfaceNumber    - interface number.
 *                           (APPLICABLE RANGES
 *                            when mappingType ==
 *                              CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E
 *                            or when mappingType ==
 *                              CPSS_DXCH_PORT_MAPPING_TYPE_REMOTE_PHYSICAL_PORT_E:
 *                                  Bobcat2 0..71, Bobcat3 0..31;
 *                            when mappingType ==
 *                              CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E: 0..63)
 *      txqPortNumber      - TXQ port number
 *                           (APPLICABLE RANGES: 0..71)
 *      tmEnable           - enable Traffic Manager processing on egress physical port
 *                           GT_FALSE - TM disabled for port, packet bypass TM
 *                           GT_TRUE  - TM enabled for port, packets handled by TM
 *                           (APPLICABLE DEVICES: Bobcat2)
 *      tmPortInd          - TM port index.
 *                           Relevant only when tmEnable == GT_TRUE.
 *                           (APPLICABLE RANGES: 0..71 and 128..191)
 *                           (APPLICABLE DEVICES: Bobcat2)
 * Comments:
 *
 */
typedef struct
{
    GT_PHYSICAL_PORT_NUM                physicalPortNumber;
    CPSS_DXCH_PORT_MAPPING_TYPE_ENT     mappingType;
    GT_U32                              portGroup;
    GT_U32                              interfaceNum;
    GT_U32                              txqPortNumber;
    GT_BOOL                             tmEnable;
    GT_U32                              tmPortInd;
}CPSS_DXCH_PORT_MAP_STC;



/*
 * Typedef: struct CPSS_DXCH_SHADOW_PORT_MAP_STC
 *
 * Description: A structure to hold detailed information about
 *              physical port mapping.
 *
 * Fields:
 *      mappingType - mapping type
 *      trafficManagerEn - ports traffic forwarded through TM
 *      portGroup   - port group number of interface.
 *                    Relevant for multi port group devices only.
 *                    (APPLICABLE DEVICES: Bobcat3)
 *      macNum      - MAC Number for ETHERNET and REMOTE_PHYSICAL map types
 *                    Parameter is ignored for other mapping types.
 *                    (APPLICABLE RANGES: Bobcat2, Caelum 0..71, Bobcat3 0..31)
 *      rxDmaNum    - RXDMA port number
 *      txDmaNum    - TXDMA port number
 *      txFifoPortNum - TxFIFO port number
 *      txqNum      - TXQ port number
 *                    (APPLICABLE RANGES: 0..71)
 *      ilknChannel - Interlaken channel number.
 *                    Relevant only for mapping type
 *                    CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E.
 *                    (APPLICABLE RANGES: 0..63)
 *      tmPortIdx   - traffic manager port index
 *
 */
typedef struct
{
    CPSS_DXCH_PORT_MAPPING_TYPE_ENT mappingType;
    GT_BOOL                         trafficManagerEn;
    GT_U32                          portGroup;
    GT_U32                          macNum;
    GT_U32                          rxDmaNum;
    GT_U32                          txDmaNum;
    GT_U32                          txFifoPortNum;
    GT_U32                          txqNum;
    GT_U32                          ilknChannel;
    GT_U32                          tmPortIdx;
}CPSS_DXCH_SHADOW_PORT_MAP_STC;


/*
 * Typedef: struct CPSS_DXCH_DETAILED_PORT_MAP_STC
 *
 * Description: A structure to hold shadow of physical port mapping.
 *
 * Fields:
 *      valid       - valid mapping
 *                    GT_TRUE - valid mapping, was checked and set by cpssDxChPortPhysicalPortMapSet
 *                    GT_FALSE - not valid mapping
 *      portMap     - structure that hold detailed port mapping
 *
 */
typedef struct
{
    GT_BOOL                           valid;
    CPSS_DXCH_SHADOW_PORT_MAP_STC     portMap;
}CPSS_DXCH_DETAILED_PORT_MAP_STC;

/*******************************************************************************
* cpssDxChPortPhysicalPortMapSet
*
* DESCRIPTION:
*       configure ports mapping
*
* APPLICABLE DEVICES:
*        Bobcat2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portMapArraySize - Number of ports to map, array size
*       portMapArrayPtr  - pointer to array of mappings
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PARAM          - wrong devNum, portNum
*       GT_BAD_PTR            - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. All usable ports shall be configured at ONCE. Any port, that was mapped previously and is
*          not included into current mapping, losses its mapping.
*       2. Changing mapping under traffic may cause to traffic interruption.
*       3. when CPSS_DXCH_CFG_PORT_MAPPING_TYPE_INLK_CHANNEL_E  is defined the first channel
*          phsical port is used for DMA configuration
*       4. The function checks whether all mapping are consistent.
*          In case of contradiction between mappings
*          (e,g. single TxQ port is mapped to several Tx DMA ports) the function returns with BAD_PARAM.
*       5. Other CPSS APIs that receive GT_PHYSICAL_PORT_NUM as input parameter will use this mapping
*          to convert the physical port to mac/dma/txq  to configure them
*       6. Usage example
*        #define GT_NA ((GT_U32)-1)
*        CPSS_DXCH_CFG_PORT_MAP_STC bc2ExampleMap[] =
*        {
*            {  0, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      0,  0, GT_FALSE},
*            {  1, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,      1,  1, GT_FALSE},
*            { 50, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     50,  2, GT_FALSE},
*            { 53, _INTRFACE_TYPE_ETHERNET_MAC_E, GT_NA,     53,  3, GT_FALSE},
*            { 63, _INTRFACE_TYPE_CPU_SDMA_E,     GT_NA,  GT_NA, 71, GT_FALSE},
*            {100, _INTRFACE_TYPE_ILNK_MAC_E,     GT_NA,  GT_NA, 40, GT_TRUE},
*            {101, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 41, GT_TRUE},
*            {102, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 42, GT_TRUE},
*            {103, _INTRFACE_TYPE_REMOTE_PORT_E,  GT_NA,  GT_NA, 43, GT_TRUE}
*        };
*        rc = cpssDxChCfgPhysicalPortMapSet(devNum, sizeof(bc2ExampleMap)/sizeof(bc2ExampleMap[0]), &bc2ExampleMap[0]);
*
*
********************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapSet
(
    IN  GT_U8                       devNum,
    IN  GT_U32                      portMapArraySize,
    IN  CPSS_DXCH_PORT_MAP_STC     *portMapArrayPtr
);

/*******************************************************************************
* cpssDxChPortPhysicalPortMapGet
*
* DESCRIPTION:
*       get ports mappping
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       firstPhysicalPortNumber - first physical port to bring
*       portMapArraySize - Mapping array size
*
* OUTPUTS:
*       portMapArrayPtr  - pointer to ports mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_FAIL         - when cannot deliver requested number of items
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        firstPhysicalPortNumber,
    IN GT_U32                      portMapArraySize,
    OUT CPSS_DXCH_PORT_MAP_STC    *portMapArrayPtr
);


/*******************************************************************************
* cpssDxChPortPhysicalPortMapIsValidGet
*
* DESCRIPTION:
*       Check if physical port was mapped
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - port number
*
* OUTPUTS:
*       isValidPtr      - (pointer to) is valid mapping
*                         GT_TRUE - given port was mapped
*                         GT_FALSE - there is no valid mapping for given port
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapIsValidGet
(
    IN GT_U8                       devNum,
    IN GT_PHYSICAL_PORT_NUM        portNum,
    OUT GT_BOOL                    *isValidPtr
);

/*******************************************************************************
* cpssDxChPortPhysicalPortMapReverseMappingGet
*
* DESCRIPTION:
*       Function checks and returns physical port number that mapped to given
*       MAC/TXQ/DMA port number.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       origPortType      - original port type
*       origPortNum       - MAC/TXQ/DMA port number
*
* OUTPUTS:
*       physicalPortNumPtr  - (pointer to) physical port number
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, origPortNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_FOUND    - not found
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortMapReverseMappingGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_DXCH_PORT_MAPPING_TYPE_ENT origPortType,
    IN  GT_U32                          origPortNum,
    OUT GT_PHYSICAL_PORT_NUM            *physicalPortNumPtr
);

/*******************************************************************************
* cpssDxChPortPhysicalPortDetailedMapGet
*
* DESCRIPTION:
*       Function returns physical port detailed mapping
*       MAC/TXQ/DMA port number.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum            - device number
*       portNum           - port number
*
* OUTPUTS:
*       portMapShadowPtr  - (pointer to) detailed mapping
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_BAD_PTR      - on bad pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPhysicalPortDetailedMapGet
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    portNum,
    OUT  CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapShadowPtr
);



#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif


