/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortInterlaken.h
*
* DESCRIPTION:
*       CPSS DxCh Interlaken Port API.
* NOTE:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __cpssDxChPortInterlakenh
#define __cpssDxChPortInterlakenh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>
#include <cpss/generic/port/cpssPortStat.h>

/*******************************************************************************
* cpssDxChPortInterlakenCounterGet
*
* DESCRIPTION:
*       Function gets counters of Port Interlaken interface.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum              - device number
*       portNum             - physical port number.
*       counterId           - counter Id
*       laneNum             - lane number - relevant only for
*                             BLK_TYPE_ERR, DIAG_CRC_ERR, WORD_SYNC_ERR
*                             (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       counterPtr   - (pointer to) 64-bit counter value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on illegal state
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortInterlakenCounterGet
(
    IN  GT_U8                                   devNum,
    IN  GT_PHYSICAL_PORT_NUM                    portNum,
    IN  CPSS_PORT_INTERLAKEN_COUNTER_ID_ENT     counterId,
    IN  GT_U32                                  laneNum,
    OUT GT_U64                                  *counterPtr
);


GT_STATUS prvCpssDxChPortIlknIFBWSet
(
    IN GT_U8    devNum,
    IN GT_U32   ilknIfBWGBps,
    OUT GT_U32 *maxIlknIFSpeedMbpsPtr
);



/*******************************************************************************
* cpssDxChPortIlknChannelSpeedResolutionSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed resolution.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*       channelSpeedResMbps - channel speed resolution in Mbps
*
* OUTPUTS:
*       maxIlknIFSpeedMbpsPtr - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
                                 
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   -  default resultion is 40Mbps
*   -  if resolution is  greater than IF bandwidth, GT_BAD_PARAM is returned
*   -  returns GT_BAD_PARAM if requested resolution is not compatible with any currently existing channels.
*      In that case user shall delete all channels that are incompatible with new resolution and
*      repeat the cpssDxChPortInterlakenChannelSpeedResultionSet();
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedResolutionSet
(
    IN GT_U8    devNum,
    IN GT_U32   channelSpeedResMbps,
    OUT GT_U32 *maxIlknIFSpeedMbpsPtr
);

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedResolutionGet
*
* DESCRIPTION:
*       Get Interlaken channel speed resolution and ilkn IF BW that will be supported.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                - physical device number.
*
* OUTPUTS:
*       speedResulutionMBpsPtr - channel speed resolution in Mbps
*       maxIlknIFSpeedMbpsPtr  - The total bandwidth the Interlaken channels will support with the given resolution (small resolution == small total bandwidth)
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedResolutionGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *speedResulutionMBpsPtr,
    OUT GT_U32  *maxIlknIFSpeedPtr	
);


/*******************************************************************************
* cpssDxChPortIlknChannelSpeedSet
*
* DESCRIPTION:
*       Configure Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       speed       - channel speed in Mbps, resolution depends on ASIC. Use 0 to
*                       free channel resources.
*
* OUTPUTS:
*       actualSpeedPtr - because HW constrains dectate granularity, real TX speed
*                       could differ from requested. Application must use this
*                       value to know how much of general ILKN interface bandwidth
*                       remain free for TX.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_OUT_OF_RANGE   - speed out of range
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   - Could be engaged only after cpssDxChPortModeSpeedSet pass.
*   - the speed shall be defined in MBps in quantum of channel speed resolution, provided by 
*     cpssDxChPortIlknChannelSpeedResolutionSet()
*   - RX speed reconfiguration can cause fragmentation in Packet Reassembly memory
*       and GT_NO_RESOURCE error when actually there is enough free space, to solve
*       this application will have delete all or part of channels and configure them
*       again in one sequence.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  speed,
    OUT GT_U32                  *actualSpeedPtr
);

/*******************************************************************************
* cpssDxChPortIlknChannelSpeedGet
*
* DESCRIPTION:
*       Get Interlaken channel speed.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported!
*
* OUTPUTS:
*       speedPtr    - actual channel speed in Mbps.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_BAD_PTR        - speedPtr == NULL
*       GT_NOT_INITIALIZED - ILKN interface not configured properly
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   In Bobcat2 ILKN_TXFIFO allows 40Mbps granularity of TX channel speed if
*    bandwidth of ILKN channel is 40Gbps and 20Mbps if total BW is 20Gbps.
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelSpeedGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_U32                  *speedPtr
);

/*******************************************************************************
* cpssDxChPortIlknChannelEnableSet
*
* DESCRIPTION:
*       Enable/disable Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX/Both.
*       enable      -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* cpssDxChPortIlknChannelEnableGet
*
* DESCRIPTION:
*       Get Enable/disable state of Interlaken channel.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number of channel.
*       direction   - TX/RX. Attention: Both not supported.
*
* OUTPUTS:
*       enablePtr   -   GT_TRUE - "direction" of channel enabled;
*                       GT_FALSE- "direction" channel disabled.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong parameter
*       GT_NO_RESOURCE    - no bandwidth to supply channel speed
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortIlknChannelEnableGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    OUT GT_BOOL                 *enablePtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortInterlakenh */


