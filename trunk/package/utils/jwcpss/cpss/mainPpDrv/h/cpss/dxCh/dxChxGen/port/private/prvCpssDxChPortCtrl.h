/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortCtrl.h
*
* DESCRIPTION:
*       Includes types and values definition and initialization for the use of
*       CPSS DxCh Port Control feature.
*
*
* FILE REVISION NUMBER:
*       $Revision: 29 $
*
*******************************************************************************/
#ifndef __prvCpssDxChPortCtrlh
#define __prvCpssDxChPortCtrlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
/* get common defs */
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/port/private/prvCpssPortTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>

#include <port/mvHwsPortInitIf.h>

extern GT_BOOL  prvCpssDrvTraceHwDelay[PRV_CPSS_MAX_PP_DEVICES_CNS];

/* serdesSpeed1 register value for 2.5 gig mode */
#define PRV_SERDES_SPEED_2500_REG_VAL_CNS 0x0000264A

/* serdesSpeed1 register value for 1 gig mode*/
#define PRV_SERDES_SPEED_1000_REG_VAL_CNS 0x0000213A

/* at this moment max number of serdeses that could be occupied by port
   is 8 for XLG mode */
#define PRV_CPSS_DXCH_MAX_SERDES_NUM_PER_PORT_CNS 8

#define PRV_PER_PORT_LOOP_MAC for(localPort = startSerdes/2; localPort*2 < startSerdes+serdesesNum; localPort++)
#define PRV_PER_SERDES_LOOP_MAC for (i = startSerdes; i < startSerdes+serdesesNum; i++)

/* macro for HW wait time for configuration, revords time delays if enabled */
#ifndef ASIC_SIMULATION
    #define HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,miliSec) if (prvCpssDrvTraceHwDelay[devNum] == GT_TRUE) \
                                                    {cpssTraceHwAccessDelay(devNum,portGroupId,miliSec); } \
                                                    cpssOsTimerWkAfter(miliSec)
#else /*ASIC_SIMULATION*/
/* the simulation NOT need those 'sleeps' needed in HW */
    #define HW_WAIT_MILLISECONDS_MAC(devNum,portGroupId,miliSec) if (prvCpssDrvTraceHwDelay[devNum] == GT_TRUE) \
                                                    {cpssTraceHwAccessDelay(devNum,portGroupId,miliSec); }
#endif /*ASIC_SIMULATION*/

#define PRV_CPSS_DXCH_BC2_PORT_TXDMA_CONFIG_CALC_MAC(speedForCalc) (((GT_U32)((350+(speedForCalc-1))/speedForCalc))<<3)

/* index in ...SerdesPowerUpSequence arrays of serdes registers defining
    serdes frequency */
#define PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG2_INDEX_CNS   1
#define PRV_CPSS_DXCH_PORT_SERDES_PLL_INTP_REG3_INDEX_CNS   2
#define PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG0_INDEX_E     11
#define PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG1_INDEX_E     12
#define PRV_CPSS_DXCH_PORT_SERDES_TRANSMIT_REG2_INDEX_E     13
#define PRV_CPSS_DXCH_PORT_SERDES_FFE_REG0_INDEX_E          16
#define PRV_CPSS_DXCH_PORT_SERDES_DFE_REG0_INDEX_E          17/* Lion specific */
#define PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_XCAT_INDEX_E  21
#define PRV_CPSS_DXCH_PORT_SERDES_CALIBRATION_REG7_LION_INDEX_E  22

/* in Lion2,3 traffic arbiter must share time between ports of GOP accordingly to number
   of ports in it */
#define PRV_CPSS_DXCH_LION2_3_THREE_MINI_GOPS_PIZZA_SLICES_NUM_CNS 12
#define PRV_CPSS_DXCH_LION2_3_FOUR_MINI_GOPS_PIZZA_SLICES_NUM_CNS 16

/* array defining possible interfaces/ports modes configuration options */
/* APPLICABLE DEVICES:  DxCh */
/* extern GT_BOOL supportedPortsModes[PRV_CPSS_XG_PORT_OPTIONS_MAX_E][CPSS_PORT_INTERFACE_MODE_NA_E]; */
GT_BOOL prvCpssDxChPortTypeSupportedModeCheck
(
    IN PRV_CPSS_PORT_TYPE_OPTIONS_ENT portType,
    IN CPSS_PORT_INTERFACE_MODE_ENT   ifMode
);

GT_STATUS prvCpssDxChPortSupportedModeCheck
(
    IN  GT_U8                         devNum,
    IN  GT_PHYSICAL_PORT_NUM          portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT  ifMode,
    OUT GT_BOOL                      *isSupportedPtr
);


/*******************************************************************************
* PRV_CPSS_PORT_MODE_SPEED_TEST_FUN
*
* DESCRIPTION:
*      The function checks if specific interface mode and speed compatible with
*       given port interface
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       status - pointer to test result:
*         GT_TRUE - suggested mode/speed pair compatible with given port interface,
*         GT_FALSE - otherwise
*
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*PRV_CPSS_PORT_MODE_SPEED_TEST_FUN)
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_BOOL *status
);

/*******************************************************************************
* prvCpssDxChPortIfCfgInit
*
* DESCRIPTION:
*       Initialize port interface mode configuration method in device object
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_OUT_OF_CPU_MEM   - port object allocation failed
*       GT_FAIL             - wrong devFamily
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortIfCfgInit
(
    IN GT_U8    devNum
);

/*******************************************************************************
* prvCpssDxChPortSerdesResetStateSet
*
* DESCRIPTION:
*       Set SERDES Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       startSerdes - first SERDES number
*       serdesesNum - number of SERDESes
*       state  - Reset state
*                GT_TRUE  - Port SERDES is under Reset
*                GT_FALSE - Port SERDES is Not under Reset, normal operation
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSerdesResetStateSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  GT_U32    startSerdes,
    IN  GT_U32    serdesesNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* prvCpssDxChPortStateDisableSet
*
* DESCRIPTION:
*       Disable a specified port on specified device.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portStatePtr - (pointer to) the state (en/dis) of port before calling this function
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortStateDisableSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    OUT GT_U32  *portStatePtr
);

/*******************************************************************************
* prvCpssDxChPortStateEnableSet
*
* DESCRIPTION:
*       Enable a specified port on specified device.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       portState - the state (en/dis) of port before calling the port disable
*                   function
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortStateEnableSet
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U32 portState
);

/*******************************************************************************
* prvCpssDxChPortInbandAutonegMode
*
* DESCRIPTION:
*       Set inband autoneg mode accordingly to required ifMode
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*       ifMode      - port interface mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant when <InBandAnEn> is set to 1.
*       Not relevant for the CPU port.
*       This field may only be changed when the port link is down.
*       In existing devices inband auto-neg. disabled by default.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortInbandAutonegMode
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_PORT_INTERFACE_MODE_ENT ifMode
);

/*******************************************************************************
* prvCpssDxChLpGetFirstInitSerdes
*
* DESCRIPTION:
*       Get first initialize serdes per port
*
* INPUTS:
*       devNum             - physical device number
*       portGroupId        - ports group number
*       startSerdes        - start SERDES number
*
*
* OUTPUTS:
*       initSerdesNumPtr   - pointer to first initialize serdes number per port
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpGetFirstInitSerdes
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  GT_U32 startSerdes,
    OUT GT_U32 *initSerdesNumPtr
);

/*******************************************************************************
* prvCpssDxChLpCheckSerdesInitStatus
*
* DESCRIPTION:
*       Check LP SERDES initialization status.
*
* INPUTS:
*       devNum        - physical device number
*       portGroupId   - ports group number
*       serdesNum     - SERDES number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_INITIALIZED - on not initialized serdes
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpCheckSerdesInitStatus
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum
);

/*******************************************************************************
* prvGetLpSerdesSpeed
*
* DESCRIPTION:
*       Gets LP serdes speed.
*
* INPUTS:
*       devNum - physical device number
*       portGroupId - port group Id, support multi-port-groups device
*       serdesNum - number of first serdes of configured port
*
* OUTPUTS:
*       serdesSpeedPtr - (pointer to) serdes speed
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvGetLpSerdesSpeed
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  GT_U32               serdesNum,
    OUT CPSS_DXCH_PORT_SERDES_SPEED_ENT  *serdesSpeedPtr
);

/******************************************************************************
* prvCpssDxCh3PortGroupSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes per group
*       according to port capabilities.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum     - physical device number
*       portSerdesGroup  - port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portSerdesGroup
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Get SERDES port group according to port by calling to
*     cpssDxChPortSerdesGroupGet.
*
******************************************************************************/
GT_STATUS prvCpssDxCh3PortGroupSerdesPowerStatusSet
(
    IN  GT_U8      devNum,
    IN  GT_U32     portSerdesGroup,
    IN  GT_BOOL    powerUp
);

/*******************************************************************************
* prvCpssDxCh3SerdesSpeedModeGet
*
* DESCRIPTION:
*       Gets speed mode for specified port serdes on specified device.
*
* APPLICABLE DEVICES:
*        DxCh3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       serdesSpeed_2500_EnPtr - pointer to serdes speed mode.
*                                GT_TRUE  - serdes speed is 2.5 gig.
*                                GT_FALSE - other serdes speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxCh3SerdesSpeedModeGet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL                 *serdesSpeed_2500_EnPtr
);

/*******************************************************************************
* prvCpssDxChXcatPortSerdesPowerStatusSet
*
* DESCRIPTION:
*       Set power up or down state to port and serdes.
*
* APPLICABLE DEVICES:
*        xCat; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat3; Lion; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       direction - TX/RX/BOTH serdes direction (ignored)
*       lanesBmp - lanes bitmap         (ignored)
*       powerUp  - power up state:
*                  GT_TRUE  - power up
*                  GT_FALSE - power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChXcatPortSerdesPowerStatusSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORT_DIRECTION_ENT direction,
    IN  GT_U32                  lanesBmp,
    IN  GT_BOOL                 powerUp
);

/*******************************************************************************
* serdesSpeedSet
*
* DESCRIPTION:
*       Configure serdes registers uniqiue for specific frequency
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS serdesSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

/*******************************************************************************
* prvCpssDxChPortSpeedForCutThroughWaCalc
*
* DESCRIPTION:
*       Calculate port speed HW value and field offsets in RX and TX DMAs
*       for Cut Throw WA - packet from slow to fast port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum        - device number
*       localPortNum  - local port number
*       speed         - port speed
*
* OUTPUTS:
*       hwSpeedPtr    - pointer to 2-bit HW speed value.
*       rxRegAddrPtr  - pointer to address of relevant RX DMA register.
*       txRegAddrPtr  - pointer to address of relevant TX DMA register.
*       rxRegOffPtr   - pointer to bit offset of field in relevant RX DMA register.
*       txRegOffPtr   - pointer to bit offset of field in relevant TX DMA register.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedForCutThroughWaCalc
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM localPortNum,
    IN  CPSS_PORT_SPEED_ENT  speed,
    OUT GT_U32               *hwSpeedPtr,
    OUT GT_U32               *rxRegAddrPtr,
    OUT GT_U32               *txRegAddrPtr,
    OUT GT_U32               *rxRegOffPtr,
    OUT GT_U32               *txRegOffPtr
);

/*******************************************************************************
* prvCpssDxChPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or speed
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported speed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - Speed of the port group member is different
*                                  from speed for setting.
*                                  (For DxCh3, not XG ports only.)
*
* COMMENTS:
*       1. If the port is enabled then the function disables the port before
*          the operation and re-enables it at the end.
*       2. For Flex-Link ports the interface mode should be
*          configured before port's speed, see cpssDxChPortInterfaceModeSet.
*       3.This API also checks all SERDES per port initialization. If serdes was
*         not initialized, proper init will be done
*       4.Added callback bind option used to run PHYMAC configuration functions
*         Callbacks can be run before switch MAC configuration and after
*         First callback can set doPpMacConfig TRUE or FALSE. If the return value
*         is TRUE the switch MAC will be configured
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

/*******************************************************************************
* prvCpssDxChPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on no initialized SERDES per port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        This API also checks if at least one serdes per port was initialized.
*        In case there was no initialized SERDES per port GT_NOT_INITIALIZED is
*        returned.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);

/*******************************************************************************
* prvCpssDxChPortGePortTypeSet
*
* DESCRIPTION:
*       Set port type and inband auto-neg. mode of GE MAC of port
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*       ifMode      - port interface mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device, ifMode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortGePortTypeSet
(
    IN GT_U8                        devNum,
    IN GT_PHYSICAL_PORT_NUM         portNum,
    IN CPSS_PORT_INTERFACE_MODE_ENT ifMode
);

/*******************************************************************************
* prvCpssDxChPortForceLinkDownEnable
*
* DESCRIPTION:
*       Enable Force link down on a specified port on specified device and
*           read current force link down state of it.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*
* OUTPUTS:
*       linkDownStatusPtr - (ptr to) current force link down state of port:
*                                   GT_TRUE  - enable force link down on port,
*                                   GT_FALSE - disable force link down on port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortForceLinkDownEnable
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM    portNum,
    OUT GT_BOOL *linkDownStatusPtr
);

/*******************************************************************************
* prvCpssDxChPortForceLinkDownDisable
*
* DESCRIPTION:
*       Disable Force link down on a specified port on specified device if
*       it was previously disabled.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       linkDownStatus - previous force link down state:
*                                   GT_TRUE  - enable force link down on port,
*                                   GT_FALSE - disable force link down on port.
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortForceLinkDownDisable
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM   portNum,
    IN GT_BOOL linkDownStatus
);

/*******************************************************************************
* prvCpssLion2CpssIfModeToHwsTranslate
*
* DESCRIPTION:
*       Translate port interface mode and speed from CPSS enum to hwService enum
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       cpssIfMode - i/f mode in CPSS format
*       cpssSpeed  - port speed in CPSS format
*
* OUTPUTS:
*       hwsIfModePtr - port i/f mode in format of hwServices library
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_SUPPORTED         - i/f mode/speed pair not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssLion2CpssIfModeToHwsTranslate
(
    IN  CPSS_PORT_INTERFACE_MODE_ENT    cpssIfMode,
    IN  CPSS_PORT_SPEED_ENT             cpssSpeed,
    OUT MV_HWS_PORT_STANDARD            *hwsIfModePtr
);

/*******************************************************************************
* prvCpssDxChPortInterfaceModeHwGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port from HW.
*
* APPLICABLE DEVICES:
*        xCat; Lion; xCat2; Lion2; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*   For Lion no possibility to recognize CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E,
*   because it's SW term - function will return CPSS_PORT_INTERFACE_MODE_XGMII_E
*******************************************************************************/
GT_STATUS prvCpssDxChPortInterfaceModeHwGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);

/*******************************************************************************
* prvCpssDxChPortSpeedHwGet
*
* DESCRIPTION:
*       Gets from HW speed for specified port on specified device.
*
* APPLICABLE DEVICES:
*        Lion; xCat; Lion2; xCat2; Bobcat2; xCat3; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - speed not appropriate for interface mode
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSpeedHwGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT CPSS_PORT_SPEED_ENT   *speedPtr
);

/*******************************************************************************
* prvCpssDxChPortTypeSet
*
* DESCRIPTION:
*       Sets port type (mostly means which mac unit used) on a specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.
*       speed   - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None.
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID prvCpssDxChPortTypeSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT            speed
);

/*******************************************************************************
* prvCpssDxChSerdesRefClockTranslateCpss2Hws
*
* DESCRIPTION:
*       Get serdes referense clock from CPSS DB and translate it to HWS format
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       refClockPtr - (ptr to) serdes referense clock in HWS format
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_STATE - if value from CPSS DB not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChSerdesRefClockTranslateCpss2Hws
(
    IN  GT_U8 devNum,
    OUT MV_HWS_REF_CLOCK_SUP_VAL *refClockPtr
);

/*******************************************************************************
* prvCpssDxChPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortForceLinkDownEnableSet
(
    IN  GT_U8     devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    IN  GT_BOOL   state
);

/*******************************************************************************
* prvCpssDxChPortLion2LinkFixWa
*
* DESCRIPTION:
*       For Lion2 A0 "no allignment lock WA"
*       For Lion2 B0 "40G connect/disconnect WA", "false link up WA"
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK    - success
*       GT_FAIL  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortLion2LinkFixWa
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
);

/*******************************************************************************
* prvCpssDxChPortLion2LinkFixWa
*
* DESCRIPTION:
*       Run RXAUI link WA
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*
* OUTPUTS:
*       None
*
* RETURNS :
*       GT_OK    - success
*       GT_FAIL  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortLion2RxauiLinkFixWa
(
    IN GT_U8                devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
);

/*******************************************************************************
* prvCpssDxChPortLion2GeLinkStatusWaEnableSet
*
* DESCRIPTION:
*       Enable/disable WA for FE-4933007 (In Lion2 port in tri-speed mode link 
*       status doesn't change in some cases when cable is disconnected/connected.)
*       Must disable WA if any type of loopback defined on GE port to see link up.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (not CPU port)
*       enable    - If GT_TRUE, enable WA
*                   If GT_FALSE, disable WA
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortLion2GeLinkStatusWaEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* prvCpssDxChLion2PortTypeSet
*
* DESCRIPTION:
*       Sets port type (mostly means which mac unit used) on a specified port.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       ifMode    - Interface mode.
*       speed     - port speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - the speed is not supported on the port
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLion2PortTypeSet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed
);


/*******************************************************************************
* geMacUnitSpeedSet
*
* DESCRIPTION:
*       Configure GE MAC unit of specific device and port to required speed
*
* INPUTS:
*       devNum - physical device number
*       portNum - physical port number
*       speed  - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PARAM     - on wrong speed
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS geMacUnitSpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

/*
 * typedef: enum PRV_DXCH_PORT_MAC_CNTR_READ_MODE_ENT
 *
 * Description: Enumeration of the Get MAC Counters function mode.
 *
 * Enumerations:
 *  PRV_DXCH_PORT_MAC_CNTR_READ_MODE_USUAL_E - usual mode for API call
 *  PRV_DXCH_PORT_MAC_CNTR_READ_MODE_UPDATE_SHADOW_E - read MAC counters from HW
 *                                                     and update shadow DB
 *  PRV_DXCH_PORT_MAC_CNTR_READ_MODE_RESET_HW_E - reset counters in HW by read
 *                                                them. Do not update shadow DB.
 * Comments:
 *         
 */
typedef enum{
    PRV_DXCH_PORT_MAC_CNTR_READ_MODE_USUAL_E,
    PRV_DXCH_PORT_MAC_CNTR_READ_MODE_UPDATE_SHADOW_E,
    PRV_DXCH_PORT_MAC_CNTR_READ_MODE_RESET_HW_E
}PRV_DXCH_PORT_MAC_CNTR_READ_MODE_ENT;

/*******************************************************************************
* prvCpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number,
*                        CPU port if getFromCapture is GT_FALSE
*       getFromCapture  -  GT_TRUE -  Gets the captured Ethernet MAC counter
*                         GT_FALSE - Gets the Ethernet MAC counter
*       readMode       - read counters procedure mode
*
* OUTPUTS:
*       portMacCounterSetArrayPtr - (pointer to) array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Not supported counters: CPSS_BAD_PKTS_RCV_E, CPSS_UNRECOG_MAC_CNTR_RCV_E,
*     CPSS_BadFC_RCV_E, CPSS_GOOD_PKTS_RCV_E, CPSS_GOOD_PKTS_SENT_E.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacCountersOnPortGet
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         getFromCapture,
    OUT CPSS_PORT_MAC_COUNTER_SET_STC   *portMacCounterSetArrayPtr,
    IN  PRV_DXCH_PORT_MAC_CNTR_READ_MODE_ENT readMode
);


/*******************************************************************************
* prvCpssDxChPortMacCountersSpecialShadowReset
*
* DESCRIPTION:
*       Reset Special Mib conters packet in Shadow to work arround hardware error.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number or CPU port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacCountersSpecialShadowReset
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum
);


/******************************************************************************
* prvCpssDxChPortPeriodicFlowControlIntervalSelectionSet
*
* DESCRIPTION:
*       Set Periodic Flow Control interval selection.
*
* APPLICABLE DEVICES:
*        Lion; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       portType  - interval selection: use interval 0 tuned by default for GE
*                                       or interval 1 tuned by default for XG
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad devNum, portNum, portType
*     GT_FAIL          - on error
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
******************************************************************************/
GT_STATUS prvCpssDxChPortPeriodicFlowControlIntervalSelectionSet
(
    IN  GT_U8                               devNum,
    IN  GT_PHYSICAL_PORT_NUM                portNum,
    IN  CPSS_DXCH_PORT_PERIODIC_FC_TYPE_ENT portType
);


/*******************************************************************************
* prvCpssDxChPortLion2InterfaceModeHwGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port from HW.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - wrong media interface mode value received
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortLion2InterfaceModeHwGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT CPSS_PORT_INTERFACE_MODE_ENT   *ifModePtr
);

/* CPSS_PORT_SPEED_47200_E */
typedef struct prvCpssDxChLion2PortInfo_STC
{
    GT_U32 rxDmaIfWidth;                  /* 0 - 64bit       2 - 256 bit      */
    GT_U32 rxDmaSource;                   /* 0 - regular Mac 1 - extended Mac */
    GT_U32 mppmXlgMode[2];                /* 0 - !40G        1 - 40G          */
    GT_U32 txDmaThhresholdOverrideEnable; /* 0 = Disable     1 - enable       */
}prvCpssDxChLion2PortInfo_STC;

GT_STATUS lion2PortInfoGet
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    portNum,
    OUT  prvCpssDxChLion2PortInfo_STC * portInfoPtr
);

/*******************************************************************************
* prvCpssDxChPortPtpReset
*
* DESCRIPTION:
*       Reset/unreset PTP unit.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       resetTxUnit - GT_TRUE - reset PTP trasmit unit,
*                     GT_FALSE - unreset PTP trasmit unit
*                     For Bobcat2 related to both transmit and receive units
*       resetRxUnit - GT_TRUE - reset PTP receive unit
*                     GT_FALSE - unreset PTP receive unit
*                     (APPLICABLE DEVICES: Caelum; Bobcat3)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortPtpReset
(
    IN  GT_U8                           devNum,
    IN  GT_PHYSICAL_PORT_NUM            portNum,
    IN  GT_BOOL                         resetTxUnit,
    IN  GT_BOOL                         resetRxUnit
);

/*******************************************************************************
* prvCpssDxChPortSerdesPartialPowerDownSet
*
* DESCRIPTION:
*       Set power down/up of Tx and Rx on Serdeses.
*
* INPUTS:
*       devNum      - system device number
*       portNum     - physical port number
*       powerDownRx – Status of Serdes Rx (TRUE – power down, FALSE – power up).
*       powerDownTx - Status of Serdes Tx (TRUE – power down, FALSE – power up).
*
* OUTPUTS:
*       None.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2_B0.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; Lion2; xCat; xCat2; Bobcat2_A0
*
* RETURNS:
*   GT_OK                       - on success.
*   GT_FAIL                     - on failed.
*   GT_NOT_SUPPORTED            - not supported
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortSerdesPartialPowerDownSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_BOOL 	                powerDownRx,
    IN GT_BOOL 	                powerDownTx
);

/*******************************************************************************
* prvCpssDxChPortLion2InternalLoopbackEnableSet
*
* DESCRIPTION:
*       Configure MAC and PCS TX2RX loopbacks on port.
*
* APPLICABLE DEVICES:
*        Lion2; xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (not CPU port)
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
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
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortLion2InternalLoopbackEnableSet
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  GT_BOOL                 enable
);

/*******************************************************************************
* prvCpssDxChPortBc2PtpTimeStampFixWa
*
* DESCRIPTION:
*   Set the thresholds in ports Tx FIFO
*
* APPLICABLE DEVICES:
*        Bobcat2
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong port number or device
*       GT_HW_ERROR       - on hardware error
*       GT_NOT_SUPPORTED  - on not supported interface for given port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       WA fix PTP timestamp problem
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortBc2PtpTimeStampFixWa
(
    IN  GT_U8                           devNum
);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __prvCpssDxChPortCtrlh */

