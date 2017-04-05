/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortBufMg.h
*
* DESCRIPTION:
*       CPSS DXCH Port Buffers management API.
*
* FILE REVISION NUMBER:
*       $Revision: 28 $
*******************************************************************************/

#ifndef __cpssDxChPortBufMgh
#define __cpssDxChPortBufMgh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

/*******************************************************************************
* typedef: enum CPSS_DXCH_PORT_HOL_FC_ENT
*
* Description: Enumeration of system modes HOL and Flow Control
*
* Enumerations:
*    CPSS_DXCH_PORT_FC_E       - Flow Control system mode enable (default mode}
*    CPSS_DXCH_PORT_HOL_E      - HOL system mode enable
*******************************************************************************/
typedef enum
{
    CPSS_DXCH_PORT_FC_E,
    CPSS_DXCH_PORT_HOL_E
} CPSS_DXCH_PORT_HOL_FC_ENT;

/*******************************************************************************
* typedef: enum CPSS_DXCH_PORT_BUFFERS_MODE_ENT
*
* Description: Enumeration of buffer modes shared and divided
*
* Enumerations:
*       CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E - Buffers are
*                  shared among all ports. The maximal number of buffers
*                  allocated per port is limited by maximal number of
*                  buffers in the device.
*       CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E -  Buffers are
*                  divided among the ports. The maximal number of buffers
*                  allocated per port is limited by the port buffers limit
*                  configured by cpssDxChPortRxBufLimitSet.
*******************************************************************************/
typedef enum
{
    CPSS_DXCH_PORT_BUFFERS_MODE_SHARED_E,
    CPSS_DXCH_PORT_BUFFERS_MODE_DIVIDED_E
}CPSS_DXCH_PORT_BUFFERS_MODE_ENT;

/*******************************************************************************
* typedef: enum CPSS_DXCH_PORT_GROUP_ENT
*
* Description: Enumeration of port groups
*
* Enumerations:
*       CPSS_DXCH_PORT_GROUP_GIGA_E - Gigabit ports group
*                                     including CPU port
*       CPSS_DXCH_PORT_GROUP_HGS_E  - HyperG.Stack ports group
*                                     including HX/QX ports
*******************************************************************************/

typedef enum
{
    CPSS_DXCH_PORT_GROUP_GIGA_E,
    CPSS_DXCH_PORT_GROUP_HGS_E
}CPSS_DXCH_PORT_GROUP_ENT;

/*******************************************************************************
* typedef: enum CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT
*
* Description: Enumeration of buffer memory FIFOs
*
* Enumerations:
*       CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_GIGA_E - Gigabit ports FIFO
*       CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_XG_E  - 10G/20G/40G ports FIFO
*       CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_HGL_E - HGL ports FIFO
*       CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_XLG_E - XLG ports FIFO
*       CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_CPU_E - CPU port FIFO
*
*******************************************************************************/
typedef enum
{
    CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_GIGA_E = 0,
    CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_XG_E,
    CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_HGL_E,
    CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_XLG_E,
    CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_CPU_E
}CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT;
/*******************************************************************************
* cpssDxChGlobalXonLimitSet
*
* DESCRIPTION:
*       Sets the Global Xon limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xon limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xonLimit), all ports that support 802.3x Flow Control send an Xon
*       frame.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - PP's device number.
*       xonLimit   - X-ON limit in resolution of 2 buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or xonLimit.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. The Global Xon limit must be not be greater than Global Xoff Limit.
*       2. To disable Global Xon, the 2*xonLimit should be set greater-or-equal
*          of 8*<Max Buffer Limit> (maximal number of buffers allocated for
*          all ports).
*       3. Since in CH3 and above the HW resolution is 8,
*          in case the xonLimit input parameter is not a multiple of 8 we
*          round it UP to the nearest one.
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXonLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   xonLimit
);

/*******************************************************************************
* cpssDxChGlobalXonLimitGet
*
* DESCRIPTION:
*       Gets the Global Xon limit value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - PP's device number.
*
* OUTPUTS:
*       xonLimitPtr  - (pointer to) the X-ON limit value in resolution
*                       of 2 buffers.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXonLimitGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *xonLimitPtr
);

/*******************************************************************************
* cpssDxChGlobalXoffLimitSet
*
* DESCRIPTION:
*       Sets the Global Xoff limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xoff limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xoffLimit), all ports that support 802.3x Flow Control send a pause
*       frame.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - PP's device number.
*       xoffLimit   - X-OFF limit in resolution of 2 buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or xoffLimit.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To disable Global Xoff, the 2*xoffLimit should be set
*          greater-or-equal of 8*<Max Buffer Limit> (maximal number of buffers
*          allocated for all ports).
*       2. Since in CH3 and above the HW resolution is 8,
*          in case the xoffLimit input parameter is not a multiple of 8 we
*          round it UP to the nearest one.
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXoffLimitSet
(
    IN GT_U8    devNum,
    IN GT_U32   xoffLimit
);

/*******************************************************************************
* cpssDxChGlobalXoffLimitGet
*
* DESCRIPTION:
*       Gets the Global Xoff limit value.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - PP's device number.
*
* OUTPUTS:
*       xoffLimitPtr - (pointer to) the X-OFF limit value in resolution
*                       of 2 buffers.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChGlobalXoffLimitGet
(
    IN GT_U8    devNum,
    OUT GT_U32  *xoffLimitPtr
);

/*******************************************************************************
* cpssDxChPortRxFcProfileSet
*
* DESCRIPTION:
*       Bind a port to a flow control profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxFcProfileSet
(
    IN GT_U8                            devNum,
    IN GT_PHYSICAL_PORT_NUM             portNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet
);

/*******************************************************************************
* cpssDxChPortRxFcProfileGet
*
* DESCRIPTION:
*       Get the port's flow control profile.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       profileSetPtr - (pointer to) the Profile Set in which the Flow Control
*                       Parameters are associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxFcProfileGet
(
    IN  GT_U8                            devNum,
    IN  GT_PHYSICAL_PORT_NUM             portNum,
    OUT CPSS_PORT_RX_FC_PROFILE_SET_ENT  *profileSetPtr
);

/*******************************************************************************
* cpssDxChPortXonLimitSet
*
* DESCRIPTION:
*       Sets X-ON port limit. When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xonLimit   - X-ON limit in resolution of 2 buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Since in CH3 the HW resolution is 8, in case the xonLimit input
*       parameter is not a multiple of 8 we round it UP to the nearest one.
*
*       For Bobcat2 buffer resolution in HW is 1.
*******************************************************************************/
GT_STATUS cpssDxChPortXonLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           xonLimit
);

/*******************************************************************************
* cpssDxChPortXonLimitGet
*
* DESCRIPTION:
*       Gets the X-ON port limit. When Flow Control is enabled, X-ON packet
*       sent when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters of
*                    are associated.
*
* OUTPUTS:
*       xonLimitPtr  - (pointer to) the X-ON limit value in resolution
*                       of 2 buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2 buffer resolution in HW is 1.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXonLimitGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *xonLimitPtr
);

/*******************************************************************************
* cpssDxChPortXoffLimitSet
*
* DESCRIPTION:
*       Sets X-OFF port limit. When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xoffLimit  - X-OFF limit in resolution of 2 buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Since in CH3 the HW resolution is 8, in case the xoffLimit input
*       parameter is not a multiple of 8 we round it UP to the nearest one.
*
*       For Bobcat2 buffer resolution in HW is 1.
*******************************************************************************/
GT_STATUS cpssDxChPortXoffLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           xoffLimit
);

/*******************************************************************************
* cpssDxChPortXoffLimitGet
*
* DESCRIPTION:
*       Gets the X-OFF port limit. When Flow Control is enabled, X-OFF packet
*       sent when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       xoffLimitPtr - (pointer to) the X-OFF limit value in resolution
*                       of 2 buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2 buffer resolution in HW is 1.
*
*******************************************************************************/
GT_STATUS cpssDxChPortXoffLimitGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *xoffLimitPtr
);

/*******************************************************************************
* cpssDxChPortRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       rxBufLimit - buffer limit threshold in resolution of 4 buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Since in CH3 and above, the HW resolution is 16, in case the rxBufLimit
*       input parameter is not a multiple of 16 we round it UP to the nearest one.
*
*       For Bobcat2 buffer resolution in HW is 1.
*******************************************************************************/
GT_STATUS cpssDxChPortRxBufLimitSet
(
    IN GT_U8                            devNum,
    IN CPSS_PORT_RX_FC_PROFILE_SET_ENT  profileSet,
    IN GT_U32                           rxBufLimit
);

/*******************************************************************************
* cpssDxChPortRxBufLimitGet
*
* DESCRIPTION:
*       Gets the receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       rxBufLimitPtr - (pointer to) the rx buffer limit value in resolution
*                       of 4 buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For Bobcat2 buffer resolution in HW is 1.
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxBufLimitGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_PORT_RX_FC_PROFILE_SET_ENT profileSet,
    OUT GT_U32                          *rxBufLimitPtr
);

/*******************************************************************************
* cpssDxChPortCpuRxBufCountGet
*
* DESCRIPTION:
*       Gets receive buffer count for the CPU on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum  - The device to get the Cpu Rx buffer count for.
*
* OUTPUTS:
*       cpuRxBufCntPtr - (Pointer to) number of buffers allocated for the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCpuRxBufCountGet
(
    IN  GT_U8   devNum,
    OUT GT_U16  *cpuRxBufCntPtr
);

/*******************************************************************************
* cpssDxChPortFcHolSysModeSet
*
* DESCRIPTION:
*       Set Flow Control or HOL system mode on the specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - physical device number
*       modeFcHol - GT_FC_E  : set Flow Control mode
*                   GT_HOL_E : set HOL system mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS cpssDxChPortFcHolSysModeSet
(
    IN  GT_U8                     devNum,
    IN  CPSS_DXCH_PORT_HOL_FC_ENT modeFcHol
);

/*******************************************************************************
* cpssDxChPortBuffersModeSet
*
* DESCRIPTION:
*      The function sets buffers mode either shared or divided.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - device number.
*       bufferMode - buffers mode, divided or shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on invalid input parameters value
*       GT_HW_ERROR                - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChPortBuffersModeSet
(
    IN GT_U8                            devNum,
    IN CPSS_DXCH_PORT_BUFFERS_MODE_ENT  bufferMode
);

/*******************************************************************************
* cpssDxChPortBuffersModeGet
*
* DESCRIPTION:
*      The function gets buffers mode either shared or divided.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       bufferModePtr - pointer to the buffer mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on invalid input parameters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR                - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
GT_STATUS cpssDxChPortBuffersModeGet
(
    IN  GT_U8                           devNum,
    OUT CPSS_DXCH_PORT_BUFFERS_MODE_ENT *bufferModePtr
);

/*******************************************************************************
* cpssDxChPortGroupXonLimitSet
*
* DESCRIPTION:
*       The function sets the port-group X-ON threshold.
*       When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       xonLimit  -  X-ON limit in buffers.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Resolution of xonLimit field in hardware:
*       - DXCH and DXCH2 devices: 2 buffers.
*       - DXCH3 and above devices: 8 buffers.
*       The value is rounded down before write to HW.
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXonLimitSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    IN GT_U32                    xonLimit
);

/*******************************************************************************
* cpssDxChPortGroupXonLimitGet
*
* DESCRIPTION:
*       The function gets the port-group X-ON threshold.
*       When Flow Control is enabled, X-ON packet
*       sent when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       xonLimitPtr  - Pointer to the  X-ON limit in buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXonLimitGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT portGroup,
    OUT GT_U32                   *xonLimitPtr
);

/*******************************************************************************
* cpssDxChPortGroupXoffLimitSet
*
* DESCRIPTION:
*       The function sets the port-group X-OFF threshold.
*       When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       xoffLimit  - X-OFF limit in buffers
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Resolution of xoffLimit field in hardware:
*       - DXCH and DXCH2 devices: 2 buffers.
*       - DXCH3 and above devices: 8 buffers.
*       The value is rounded down before write to HW.
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXoffLimitSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    IN GT_U32                    xoffLimit
);

/*******************************************************************************
* cpssDxChPortGroupXoffLimitGet
*
* DESCRIPTION:
*       The function gets the port-group X-OFF threshold.
*       When Flow Control is enabled, X-OFF packet
*       sent when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       xoffLimitPtr  - Pointer to the X-OFF limit in buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupXoffLimitGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT portGroup,
    OUT GT_U32                   *xoffLimitPtr
);

/*******************************************************************************
* cpssDxChPortGroupRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the specified port-group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       rxBufLimit - Maximal number of buffers that may be allocated for the
*                    ports belong to a group
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Resolution of rxBufLimit field in hardware:
*       - DXCH and DXCH2 devices: 4 buffers.
*       - DXCH3 and above devices: 16 buffers.
*       The value is rounded down before write to HW.
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupRxBufLimitSet
(
    IN GT_U8                     devNum,
    IN CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    IN GT_U32                    rxBufLimit
);

/*******************************************************************************
* cpssDxChPortGroupRxBufLimitGet
*
* DESCRIPTION:
*       Gets the receive buffer limit threshold for the specified port-group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       rxBufLimitPtr - Pointer to value of maximal number of buffers that
*                       may be allocated for the ports belong to a group.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupRxBufLimitGet
(
    IN  GT_U8                     devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    OUT GT_U32                    *rxBufLimitPtr
);

/*******************************************************************************
* cpssDxChPortCrossChipFcPacketRelayEnableSet
*
* DESCRIPTION:
*       Enable/Disable SOHO Cross Chip Flow control packet relay.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE:  Enable SOHO Cross Chip Flow control packet relay.
*                    GT_FALSE: Disable SOHO Cross Chip Flow control packet relay.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCrossChipFcPacketRelayEnableSet
(
    IN GT_U8    devNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortCrossChipFcPacketRelayEnableGet
*
* DESCRIPTION:
*       Gets the current status of SOHO Cross Chip Flow control packet relay.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum    - device number.
*
* OUTPUTS:
*       enablePtr - pointer to SOHO Cross Chip Flow control packet relay staus:
*                   GT_TRUE  -  Enable SOHO Cross Chip Flow control packet relay.
*                   GT_FALSE - Disable SOHO Cross Chip Flow control packet relay.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortCrossChipFcPacketRelayEnableGet
(
    IN  GT_U8    devNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortClearByBufferEnableSet
*
* DESCRIPTION:
*       Set port to work in Clear By Buffer mode.
*
* APPLICABLE DEVICES:
*        DxCh1.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (CPU port supported as well).
*       enable     - GT_TRUE: Clear By Buffer mode.
*                    GT_FALSE: Regular Clear mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortClearByBufferEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortClearByBufferEnableGet
*
* DESCRIPTION:
*       Get port Buffer Memory Clear type.
*
* APPLICABLE DEVICES:
*        DxCh1.
*
* NOT APPLICABLE DEVICES:
*        DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number.
*       portNum    - physical port number (CPU port supported as well).
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE: Clear By Buffer mode.
*                                 GT_FALSE: Regular Clear mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortClearByBufferEnableGet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL *enablePtr
);

/*******************************************************************************
* cpssDxChPortGlobalRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGlobalRxBufNumberGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *numOfBuffersPtr
);

/*******************************************************************************
* cpssDxChPortGroupRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated for the specified
*       port group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device or port group
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGroupRxBufNumberGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_GROUP_ENT  portGroup,
    OUT GT_U32  *numOfBuffersPtr
);

/*******************************************************************************
* cpssDxChPortRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated for the specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxBufNumberGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32  *numOfBuffersPtr
);

/*******************************************************************************
* cpssDxChPortGlobalPacketNumberGet
*
* DESCRIPTION:
*       Gets total number of unique packets currently in the system.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numOfPacketsPtr - (pointer to) number of packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortGlobalPacketNumberGet
(
    IN  GT_U8    devNum,
    OUT GT_U32  *numOfPacketsPtr
);

/*******************************************************************************
* cpssDxChPortRxNumOfAgedBuffersGet
*
* DESCRIPTION:
*       Gets number of buffers cleared by aging.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       agedBuffersPtr - (pointer to) Number of aged buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxNumOfAgedBuffersGet
(
    IN  GT_U8    devNum,
    OUT GT_U32   *agedBuffersPtr
);

/*******************************************************************************
* cpssDxChPortRxMcCntrGet
*
* DESCRIPTION:
*       Gets multicast counter of a certain buffer.
*
* APPLICABLE DEVICES:
*        Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3.
*
* INPUTS:
*       devNum - physical device number
*       cntrIdx - counter index
*                 (APPLICABLE RANGES for Lion, Lion2: 0..16383)
*                 (APPLICABLE RANGES for xCat2: 0..4095)
*                 (APPLICABLE RANGES for Bobcat2, Caelum, Bobcat3: 0..8191)
*
* OUTPUTS:
*       mcCntrPtr - (pointer to) multicast counter of a certain buffer
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device or bufIdx
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortRxMcCntrGet
(
    IN  GT_U8    devNum,
    IN  GT_U32   cntrIdx,
    OUT GT_U32   *mcCntrPtr
);


/*******************************************************************************
* cpssDxChPortBufMemFifosThresholdSet
*
* DESCRIPTION:
*       Sets buffer memory FIFOs Threshold.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - physical device number
*       fifoType - FIFO type
*       descFifoThreshold - number of descriptors per port threshold mode.
*       txFifoThreshold - number of lines per port threshold mode in the Tx FIFO.
*       minXFifoReadThreshold - minimal number of descriptors to read per port threshold mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Improper configuration of values may affect entire system behavior.
*
*******************************************************************************/
GT_STATUS cpssDxChPortBufMemFifosThresholdSet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT fifoType,
    IN  GT_U32   descFifoThreshold,
    IN  GT_U32   txFifoThreshold,
    IN  GT_U32   minXFifoReadThreshold
);

/*******************************************************************************
* cpssDxChPortBufMemFifosThresholdGet
*
* DESCRIPTION:
*       Gets buffer memory FIFOs Thresholds.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - physical device number
*       fifoType - FIFO type
*
* OUTPUTS:
*       descFifoThresholdPtr - pointer to number of descriptors per port per port threshold mode.
*       txFifoThresholdPtr - pointer to number of lines per port per port threshold mode in the Tx FIFO.
*       minXFifoReadThresholdPtr - pointer to minimal number of descriptors to read per port per port threshold mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortBufMemFifosThresholdGet
(
    IN  GT_U8    devNum,
    IN  CPSS_DXCH_PORT_BUF_MEM_FIFO_TYPE_ENT fifoType,
    OUT GT_U32   *descFifoThresholdPtr,
    OUT GT_U32   *txFifoThresholdPtr,
    OUT GT_U32   *minXFifoReadThresholdPtr
);

/*******************************************************************************
* cpssDxChPortTxdmaBurstLimitEnableSet
*
* DESCRIPTION:
*       For a given port Enable/Disable TXDMA burst limit thresholds use.
*
*       When enabled: instruct the TxDMA to throttle the Transmit Queue Scheduler
*       as a function of the TxDMA FIFOs fill level measured in bytes and descriptor.
*       When disabled: the TxDMA throttles the Transmit Queue Scheduler
*       based on FIFOs fill level measured in descriptors only.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (CPU port supported as well).
*       enable     - GT_TRUE: TXDMA burst limit thresholds are used.
*                    GT_FALSE: TXDMA burst limit thresholds are not used.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxdmaBurstLimitEnableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL  enable
);

/*******************************************************************************
* cpssDxChPortTxdmaBurstLimitEnableGet
*
* DESCRIPTION:
*       Gets the current status for a given port of TXDMA burst limit thresholds use.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (CPU port supported as well).
*
* OUTPUTS:
*       enablePtr   - (pointer to)
*                    GT_TRUE: TXDMA burst limit thresholds are used.
*                    GT_FALSE: TXDMA burst limit thresholds are not used.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxdmaBurstLimitEnableGet
(
    IN  GT_U8    devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_BOOL  *enablePtr
);

/*******************************************************************************
* cpssDxChPortTxdmaBurstLimitThresholdsSet
*
* DESCRIPTION:
*       Sets the TXDMA burst limit thresholds for a given port.
*
*       The TxDMA supports three throttling levels: Normal, Slow and OFF.
*       The levels are controlled using two thresholds (almostFullThreshold and
*       fullThreshold) measuring the FIFOs fill level.
*       Normal - the Transmit Queue scheduler is not throttled.
*       Slow - the Transmit Queue scheduler is throttled.
*       OFF - the Transmit Queue scheduler is paused.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (CPU port supported as well).
*       almostFullThreshold - almost full threshold in Bytes (0..0x3FFC0).
*       fullThreshold - full threshold in Bytes (0..0x3FFC0).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Resolution of almostFullThreshold and fullThreshold fields in hardware:
*       - Lion and Lion2 devices: 64 Bytes.
*       - Bobcat2, Caelum, Bobcat3 devices: 128 Bytes.
*       The value is rounded down before write to HW.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxdmaBurstLimitThresholdsSet
(
    IN GT_U8                  devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_U32                 almostFullThreshold,
    IN GT_U32                 fullThreshold
);

/*******************************************************************************
* cpssDxChPortTxdmaBurstLimitThresholdsGet
*
* DESCRIPTION:
*       Gets the TXDMA burst limit thresholds for a given port.
*
* APPLICABLE DEVICES:
*        Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (CPU port supported as well).
*
* OUTPUTS:
*       almostFullThresholdPtr - (pointer to) almost full threshold in Bytes.
*       fullThresholdPtr - (pointer to) full threshold in Bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxdmaBurstLimitThresholdsGet
(
    IN  GT_U8                  devNum,
    IN  GT_PHYSICAL_PORT_NUM   portNum,
    OUT GT_U32                 *almostFullThresholdPtr,
    OUT GT_U32                 *fullThresholdPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssDxChPortBufMgh */

