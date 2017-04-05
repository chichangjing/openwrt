/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChModuleConfig.h
*
* DESCRIPTION:
*       This file includes the declaration of the Module configuration
*       parameters such as tables addresses , num entries , entries size ,
*       entries offset
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/
#ifndef __prvCpssDxChModuleConfigh
#define __prvCpssDxChModuleConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssHwInit/cpssHwInit.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>

/*
 * Typedef: struct PRV_CPSS_DXCH_MODULE_CONFIG_STC
 *
 * Description: Includes configuration parameters for initializing port group modules.
 *
 * Fields:
 *      Each field represents a port group unit, including configuration params.
 *      specific to that unit.
 *
 */
typedef struct
{
    /*
     * typedef: struct auCfg
     *
     * Description: Configuration structure for the Au management.
     *              Includes the blocks from which the AU descriptors should be
     *              allocated.
     *              --Note about 'per port group' :
     *                  those parameters not used beyond the init stage (phase2,ppLogicalInit)
     *                  so no need to hold here 'per port group' info.
     *                  only the global parameters from the application saved here.
     *                  instead used in runtime : PRV_CPSS_PP_MAC(devNum)->intCtrl.auDescCtrl[portGroupId]
     * Fields:
     *  auDescBlock     - The block from which the Au descriptors should be
     *                    allocated from (a none-cached memory block).
     *  auDescBlockSize - Size of auDescBlock (in Bytes).
     *
     */
    struct
    {
        GT_U8   *auDescBlock;
        GT_U32  auDescBlockSize;
    }auCfg;

    /*
     * typedef: struct fuCfg
     *
     * Description: Configuration structure for the Fu management.
     *              Includes the blocks from which the FU descriptors should be
     *              allocated.
     *              --Note about 'per port group' :
     *                  those parameters not used beyond the init stage (phase2,ppLogicalInit)
     *                  so no need to hold here 'per port group' info.
     *                  only the global parameters from the application saved here.
     *                  instead used in runtime : PRV_CPSS_PP_MAC(devNum)->intCtrl.fuDescCtrl[portGroupId]
     *
     * Fields:
     *  fuDescBlock     - The block from which the Fu descriptors should be
     *                    allocated from (an uncached memory block).
     *  fuDescBlockSize - Size of fuDescBlock (in Bytes).
     *
     */
    struct
    {
        GT_U8   *fuDescBlock;
        GT_U32  fuDescBlockSize;
    }fuCfg;

    /*    fuqUseSeparate   - GT_TRUE - use FU queue (for DxCh2 devices only),
    *                     - GT_FALSE - use AU queue.
    *
    *                       NOTE: for 'multi port groups' device , this parameter refer
    *                             to all port groups in the same way
    */
    GT_BOOL   fuqUseSeparate;

    /*
     * typedef: struct netIfCfg
     *
     * Description: Configuration structure for Network interface management.
     *                    Relevant only if useMultiNetIfSdma == GT_FALSE.
     *
     * Fields:
     *  txDescBlock     - Pointer to a block of memory to be used for allocating
     *                    Tx descriptor structures.
     *  txDescBlockSize - The raw size in bytes of txDescBlock memory.
     *  rxDescBlock     - Pointer to a block memory to be used for allocating Rx
     *                    description structures.
     *  rxDescBlockSize - The raw size in byte of rxDescBlock.
     *  rxBufInfo       - Rx buffers allocation information.
     *
     */
    struct
    {
        GT_U32  *txDescBlock;
        GT_U32   txDescBlockSize;
        GT_U32  *rxDescBlock;
        GT_U32   rxDescBlockSize;
        CPSS_RX_BUF_INFO_STC rxBufInfo;
    }netIfCfg;

    /* useMultiNetIfSdma - Enable Multi-Group network interface configurations.*/
    GT_BOOL useMultiNetIfSdma;

    /*    multiNetIfCfg - Multi-Group related network interface configuration 
     *                    parameters.
     *                    Relevant only if useMultiNetIfSdma == GT_TRUE.
     */
    CPSS_MULTI_NET_IF_CFG_STC   multiNetIfCfg;

    /*
     * typedef: struct ppEventsCfg[]
     *
     * Description: 'Per port group' Configuration structure for the interrupts unit.
     *
     * Fields:
     *  intVecNum       - The interrupt vector number this PP is connected to.
     *  intMask         - The interrupt mask to enable PP interrupts.
     */
    struct
    {
        GT_U32      intVecNum;
        GT_UINTPTR  intMask;
    }ppEventsCfg[CPSS_MAX_PORT_GROUPS_CNS];

    /*
     * struct : ip
     *
     * Description: ip values
     *
     * Fields:
     *      routingMode - PP routing mode: Policy based routing Or TCAM based Routing
     *      maxNumOfPbrEntries - max number of LPM leafs that can be allocated
     *                           in the RAM for policy based routing
     *                           (APPLICABLE DEVICES: Bobcat2, Caelum, Bobcat3)
    */
    struct
    {
        CPSS_DXCH_CFG_ROUTING_MODE_ENT routingMode;
        GT_U32                         maxNumOfPbrEntries;
    }ip;

    /*    useDoubleAuq   - Support configuration of two AUQ memory regions.
     *                     GT_TRUE - CPSS manages two AU Queues with the same size:
     *                     auqCfg->auDescBlockSize / 2.
     *                     GT_FALSE - CPSS manages single AU Queue with size:
     *                     auqCfg->auDescBlockSize.
     */
    GT_BOOL useDoubleAuq;

}PRV_CPSS_DXCH_MODULE_CONFIG_STC;



/*******************************************************************************
* prvCpssDxChTtiDbTablesInit
*
* DESCRIPTION:
*       init TTI tables internal DB.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2 ; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_OK                    - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS  prvCpssDxChTtiDbTablesInit
(
    IN GT_U8   devNum
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvCpssDxChModuleConfigh */

