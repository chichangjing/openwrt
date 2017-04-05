/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChFabricHGLink.h
*
* DESCRIPTION:
*       CPSS DxCh Fabric Connectivity HyperG.Link interface API.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

#ifndef __cpssDxChFabricHGLinkh
#define __cpssDxChFabricHGLinkh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>

/* in Lion2/3 6 xpcs lanes per port */
#define CPSS_DXCH_FABRIC_HGL_SERDES_LANES_NUM_CNS 6

/*
 * typedef: struct CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC
 *
 * Description: HyperG.Link Rx error conters
 *
 * Fields:
 *      badLengthCells     - Counts the number of dropped cells
 *                           due to cell length error
 *      badHeaderCells     - Counts the number of dropped cells
 *                           due to cell header error. An ECC or CRC
 *                           check failure Or The cell type is
 *                           invalid ( a value of 0 or 7)
 *      reformatErrorCells - Counts the number of dropped cells
 *                           due to an error in PCS to MAC Reformat
 *
 */
typedef struct
{
    GT_U32   badLengthCells;
    GT_U32   badHeaderCells;
    GT_U32   reformatErrorCells;
} CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC;

/*
 * typedef: enum CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT
 *
 * Description: Cell error correction algorithm
 *
 * Enumerations:
 *  CPSS_DXCH_FABRIC_HGL_ECC_ECC7_E - Error Check algorithm ECC 7 bits(aka SECDED)
 *  CPSS_DXCH_FABRIC_HGL_ECC_CRC8_E - Error Check algorithm CRC 8 bits
 */
typedef enum
{
    CPSS_DXCH_FABRIC_HGL_ECC_ECC7_E,
    CPSS_DXCH_FABRIC_HGL_ECC_CRC8_E
} CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT;

/*
 * typedef: CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT
 *
 * Description:
 *      Traffic direction.
 *
 * Enumerations:
 *      CPSS_DXCH_FABRIC_HGL_DIRECTION_RX_E  - RX traffic.
 *      CPSS_DXCH_FABRIC_HGL_DIRECTION_TX_E  - TX traffic.
 */
typedef enum
{
    CPSS_DXCH_FABRIC_HGL_DIRECTION_RX_E,
    CPSS_DXCH_FABRIC_HGL_DIRECTION_TX_E
} CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT;

/*
 * typedef: CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT
 * Description:
 *      Which type of cells to count.
 *
 * Enumerations:
 *   CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_UC_E,
 *   CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_MC_E,
 *   CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_BC_E,
 *   CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_OTHER_E
*/
typedef enum
{
    CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_UC_E,
    CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_MC_E,
    CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_BC_E,
    CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_OTHER_E
} CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT;

/*
 * typedef: CPSS_DXCH_FABRIC_HGL_RX_DSA_STC
 *
 * Description:
 *      Fabric RX DSA tag fields.
 * Fields:
 *      vid - VID assigned to the DSA tag that is attached to the incoming cells
 *      srcId  - Source-ID field assigned to the DSA tag that is attached to the 
 *                  incoming cells.
 * Comment:
 *
 */
typedef struct
{
    GT_U32 vid;
    GT_U32 srcId;
}CPSS_DXCH_FABRIC_HGL_RX_DSA_STC;

/*
 * typedef: CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC
 *
 * Description:
 *      Fabric FC RX E2E (QCN-like) fields.
 *
 * Fields:
 *      etherType   - EtherType for the E2E packet
 *      prioVlanId  - E2E Encapsulated VLAN ID and priority
 *      cpId        - E2E congestion point ID
 *      version     - E2E Version
 *      macLowByte  - bits[8:0] of E2E encapsulated destination MAC address, 
 *                      default value is the MAC's hardwired port number. 
 *                      Bits [47:9] are constant zero.
 * Comment:
 *
 */
typedef struct
{
    GT_U32  etherType;
    GT_U32  prioVlanId;
    GT_U32  cpId;
    GT_U32  version;
    GT_U8   macLowByte;
}CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC;

/*
 * typedef: CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC
 *
 * Description:
 *      Fabric FC RX Link Layer (LL) packet and DSA tag fields.
 *
 * Fields:
 *      etherType - LL-FC cells are encapsulated in PFC-like packets. 
 *                  This field defines the EtherType field indicating that 
 *                  the packet is PFC-like.
 *      pfcOpcode - This field defines the Opcode field in the PFC header.
 *      up - The UP field assigned to the DSA tag that is attached to the 
 *              incoming LL-FC cell.
 *      qosProfile - The QoS Profile field in the DSA tagged that is assigned 
 *                      by the HGL MAC to the incoming LL-FC cell.
 *      trgDev - The target device field in the DSA tagged that is assigned by 
 *                      the HGL MAC to the incoming LL-FC cell.
 *      trgPort - The Target Port field assigned to the DSA tag that is attached 
 *                  to the incoming LL-FC cell.
 *
 * Comment:
 *
 */
typedef struct
{
    GT_U32  etherType;
    GT_U32  pfcOpcode;
    GT_U32  up;
    GT_U32  qosProfile;
    GT_U8   trgDev;
    GT_PHYSICAL_PORT_NUM trgPort;
}CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC;


/*
 * typedef: CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT
 * Description:
 *      Types of fabric descriptors.
 *
 * Enumerations:
 *   CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_20B_E – 20-byte descriptor
 *   CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_24B_E – 24-byte descriptor
*/
typedef enum
{
    CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_20B_E,
    CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_24B_E
} CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT;


/*******************************************************************************
* cpssDxChFabricHGLinkPcsLoopbackEnableSet
*
* DESCRIPTION:
*       Enable/disable PCS loopback on Hyper.G Link 
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE  - enable loopback
*                 GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*       If loopback enbaled PCS Rx is connected directly to PCS Tx, bypassing 
*       the SERDES Interface.
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkPcsLoopbackEnableSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChFabricHGLinkPcsLoopbackEnableGet
*
* DESCRIPTION:
*       Get PCS loopback on Hyper.G Link status
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - loopback enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK               - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR          - enablePtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*       If loopback enbaled PCS Rx is connected directly to PCS Tx, bypassing 
*       the SERDES Interface.
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkPcsLoopbackEnableGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL              *enablePtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkPcsMaxIdleCntEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Idle counter in the Tx SM. When Idle counter is 
*           disabled, it is the MACs responsibilty to generate idles.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       idleCntEnable - GT_TRUE  - idle count enable
*                       GT_FALSE - idle count disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_HW_ERROR         - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkPcsMaxIdleCntEnableSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL              idleCntEnable
);

/*******************************************************************************
* cpssDxChFabricHGLinkPcsMaxIdleCntEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of the Idle counter in the Tx SM. When Idle 
*           counter is disabled, it is the MACs responsibilty to generate idles.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       idleCntEnablePtr - GT_TRUE  - idle count enable
*                           GT_FALSE - idle count disable
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_BAD_PTR      - idleCntEnablePtr is NULL
*       GT_HW_ERROR         - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkPcsMaxIdleCntEnableGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL              *idleCntEnablePtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkPcsMaxIdleCntSet
*
* DESCRIPTION:
*       Configure the number of cycles between ||k|| sequences.
*        ||K|| sequence must be sent every programmable
*        number of cycles in order to keep a sufficient
*        frequency of commas for each lane and maintain
*        code group alignment. 
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       maxIdleCnt  - number of cycles between ||k|| sequences,
*                       (APPLICABLE RANGES: 0..32768)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_HW_ERROR         - on hardware error
*       GT_OUT_OF_RANGE  - if maxIdleCnt out of range
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkPcsMaxIdleCntSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U32               maxIdleCnt
);

/*******************************************************************************
* cpssDxChFabricHGLinkPcsMaxIdleCntGet
*
* DESCRIPTION:
*       Get the number of cycles between ||k|| sequences.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       maxIdleCntPtr - number of cycles between ||k|| sequences
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR  - if maxIdleCntPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkPcsMaxIdleCntGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32               *maxIdleCntPtr
);


/*******************************************************************************
* cpssDxChFabricHGLinkPcsRxStatusGet
*
* DESCRIPTION:
*       Get the XPCS lanes synchronization status
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       laneSyncOkArr - array of statuses of lanes
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR  - if laneSyncOkArr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkPcsRxStatusGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL              laneSyncOkArr[CPSS_DXCH_FABRIC_HGL_SERDES_LANES_NUM_CNS]
);

/*******************************************************************************
* cpssDxChFabricHGLinkRxErrorCntrGet
*
* DESCRIPTION:
*       Get the XPCS lanes synchronization status
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       rxErrorsCntrsPtr - array of statuses of lanes (6 in Lion2)
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR  - if rxErrorsCntrsPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkRxErrorCntrGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC *rxErrorsCntrsPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkConfigEccTypeSet
*
* DESCRIPTION:
*    This routine defines how to calculate
*    the error correction on the HyperG.Link cell
*    Both Rx and Tx.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       eccType - the Error Correction Type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM        - on wrong port number,device,eccType 
*       GT_HW_ERROR         - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkConfigEccTypeSet
(
    IN GT_U8                 devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT eccType
);

/*******************************************************************************
* cpssDxChFabricHGLinkConfigEccTypeGet
*
* DESCRIPTION:
*    Get current type of the error correction on the HyperG.Link cell
*    Both Rx and Tx.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       eccTypePtr - current Error Correction Type of port
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM        - on wrong port number,device
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR  - if eccTypePtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkConfigEccTypeGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT *eccTypePtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkRxCrcCheckEnableSet
*
* DESCRIPTION:
*    Enable/disable CRC check of received cells.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE  - enable CRC check
*                 GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PARAM  - on wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkRxCrcCheckEnableSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_BOOL              enable
);

/*******************************************************************************
* cpssDxChFabricHGLinkRxCrcCheckEnableGet
*
* DESCRIPTION:
*    Get status of CRC check of received cells.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - enable CRC check
*                    GT_FALSE - disable
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_BAD_PTR    - if enablePtr is NULL
*       GT_HW_ERROR         - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkRxCrcCheckEnableGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL              *enablePtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkLbiEnableSet
*
* DESCRIPTION:
*    Defines if the forwarding is affected from the Load Balancing Index (LBI) 
*       field in the cell header
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE  - The target is defined according to the Target 
*                               Device and LBI fields in the cell header
*                 GT_FALSE - The target is defined only according to the Target 
*                               Device field in the cell header
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkLbiEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChFabricHGLinkLbiEnableGet
*
* DESCRIPTION:
*    Defines if the forwarding is affected from the Load Balancing Index (LBI) 
*       field in the cell header
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       enablePtr - GT_TRUE  - The target is defined according to the Target 
*                               Device and LBI fields in the cell header
*                   GT_FALSE - The target is defined only according to the Target 
*                               Device field in the cell header
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR  - if enablePtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkLbiEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *enablePtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkCellsCntrTypeSet
*
* DESCRIPTION:
*    Configure the types of received/transmitted cells to be 
*       counted in the received/transmitted cell counter
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       cellDirection - cells direction
*       cellType  - counted cells type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkCellsCntrTypeSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT  cellDirection,
    IN  CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT  cellType
);

/*******************************************************************************
* cpssDxChFabricHGLinkCellsCntrTypeGet
*
* DESCRIPTION:
*    Get the types of received/transmitted cells
*       counted in the received/transmitted cell counter
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       cellDirection - cells direction
*
* OUTPUTS:
*       cellTypePtr - current counted cells type
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR    - cellTypePtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkCellsCntrTypeGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT  cellDirection,
    OUT CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT  *cellTypePtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkCellsCntrGet
*
* DESCRIPTION:
*    Get the counter of received/transmitted cells
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       cellDirection - cells direction
*
* OUTPUTS:
*       cellCntrPtr - counter value
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR    - cellCntrPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkCellsCntrGet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT  cellDirection,
    OUT GT_U32                              *cellCntrPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkFcMacSaSet
*
* DESCRIPTION:
*    Configure the MAC-SA of LL-FC and E2E constructed packets
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       macPtr  - new source mac address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR    - if macPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkFcMacSaSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_ETHERADDR            *macPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkFcMacSaGet
*
* DESCRIPTION:
*    Configure the MAC-SA of LL-FC and E2E constructed packets
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       macPtr  - source mac address
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR    - if macPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkFcMacSaGet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_ETHERADDR         *macPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkRxDsaParamsSet
*
* DESCRIPTION:
*    Configure some Fabric RX DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       dsaParamsPtr  - DSA parameters for incomming cells
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PTR    - if dsaParamsPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*   Also used for E2E-FC and LL-FC DSA Tag parameters configuration
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkRxDsaParamsSet
(
    IN  GT_U8                            devNum,
    IN  GT_PHYSICAL_PORT_NUM             portNum,
    IN  CPSS_DXCH_FABRIC_HGL_RX_DSA_STC  *dsaParamsPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkRxDsaParamsGet
*
* DESCRIPTION:
*    Get some configurable Fabric RX DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       dsaParamsPtr  - DSA parameters for incomming cells
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PTR    - if dsaParamsPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkRxDsaParamsGet
(
    IN  GT_U8                            devNum,
    IN  GT_PHYSICAL_PORT_NUM             portNum,
    OUT CPSS_DXCH_FABRIC_HGL_RX_DSA_STC  *dsaParamsPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkFcRxE2eParamsSet
*
* DESCRIPTION:
*    Configure some Fabric FC RX E2E header fields
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       e2eParamsPtr  - parameters for incomming E2E FC cells
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PTR    - if e2eParamsPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkFcRxE2eParamsSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC *e2eParamsPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkFcRxE2eParamsGet
*
* DESCRIPTION:
*    Get some Fabric FC RX E2E header fields
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       e2eParamsPtr  - parameters for incomming E2E FC cells
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PTR    - if e2eParamsPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkFcRxE2eParamsGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT  CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC *e2eParamsPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkRxLinkLayerFcParamsSet
*
* DESCRIPTION:
*    Configure some Fabric Link Layer FC RX packet and DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       llParamsPtr  - parameters for incomming Link Layer FC cells
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PTR    - if llParamsPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkRxLinkLayerFcParamsSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC *llParamsPtr
);

/*******************************************************************************
* cpssDxChFabricHGLinkRxLinkLayerFcParamsGet
*
* DESCRIPTION:
*    Get configurable Fabric Link Layer FC RX packet and DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       llParamsPtr  - parameters for incomming Link Layer FC cells
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR   - on hardware error
*       GT_BAD_PTR    - if llParamsPtr is NULL
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkRxLinkLayerFcParamsGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC *llParamsPtr
);


/*******************************************************************************
* cpssDxChFabricHGLinkDescriptorTypeSet
*
* DESCRIPTION:
*    Configure the descriptor type.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       type    - descriptor type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkDescriptorTypeSet
(
    IN  GT_U8                                      devNum,
    IN  GT_PHYSICAL_PORT_NUM                       portNum,
    IN  CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT   type
);
 

 

/*******************************************************************************
* cpssDxChFabricHGLinkDescriptorTypeGet
*
* DESCRIPTION:
*    Gets the descriptor type.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       typePtr    - (pointer to) descriptor type
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on wrong parameter
*       GT_HW_ERROR         - on hardware error
*       GT_BAD_PTR          - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - wrong device family
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChFabricHGLinkDescriptorTypeGet
(
    IN  GT_U8                                      devNum,
    IN  GT_PHYSICAL_PORT_NUM                       portNum,
    OUT CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT   *typePtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChFabricHGLinkh */


