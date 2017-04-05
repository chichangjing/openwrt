/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChMacCtrl.h
*
* DESCRIPTION:
*       bobcat2 mac control 
*
* FILE REVISION NUMBER:
*       $Revision: 2$
*******************************************************************************/

#ifndef __PRV_CPSS_DXCH_MAC_CTRL_H
#define __PRV_CPSS_DXCH_MAC_CTRL_H

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/generic/port/cpssPortCtrl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Typedef: struct PRV_CPSS_DXCH_PORT_REG_CONFIG_STC
 *
 * Description: A structure to describe data to write to register
 *
 * Fields:
 *      regAddr     -  register address
 *      fieldOffset -  bit number inside register      
 *      fieldLength -  bit number inside register      
 *      fieldData   -  value to write into the register
*/
typedef struct
{
    GT_U32   regAddr;
    GT_U32   fieldOffset;
    GT_U32   fieldLength;
    GT_U32   fieldData;
}PRV_CPSS_DXCH_PORT_REG_CONFIG_STC;


#define PRV_CPSS_DXCH_BOBCAT2_MAX_MAC_D 72

/*--------------------------------------------------------*/
/* GIGA MAC                                               */
/*--------------------------------------------------------*/
#define GIGA_MAC_CTRL0_PORT_TYPE_FLD_OFFS_D      1
#define GIGA_MAC_CTRL0_PORT_TYPE_FLD_LEN_D       1

#define GIGA_MAC_CTRL1_PREAMBLE_LEN_4_D          4
#define GIGA_MAC_CTRL1_PREAMBLE_LEN_8_D          8

/* Mac Control Get 1 */
#define GIGA_MAC_CTRL1_PREAMBLE_LEN_FLD_OFFS_D   15
#define GIGA_MAC_CTRL1_PREAMBLE_LEN_FLD_LEN_D    1


/* Mac Control Get 3 */
#define GIGA_MAC_CTRL3_IPG_MIN_LEN_D             0
#define GIGA_MAC_CTRL3_IPG_MAX_LEN_D             511

#define GIGA_MAC_CTRL3_IPG_LEN_FLD_OFFS_D        6
#define GIGA_MAC_CTRL3_IPG_LEN_FLD_LEN_D         9


/*--------------------------------------------------------*/
/* XLG MAC                                                */
/*--------------------------------------------------------*/
#define XLG_MAC_CTRL5_TX_IPG_MIN_VAL_D          8
#define XLG_MAC_CTRL5_TX_IPG_MAX_VAL_D          15

#define XLG_MAC_CTRL5_TX_IPG_LEN_FLD_OFFS_D       0
#define XLG_MAC_CTRL5_TX_IPG_LEN_FLD_LEN_D        4

#define XLG_MAC_CTRL5_PREAMBLE_MIN_VAL_D          1
#define XLG_MAC_CTRL5_PREAMBLE_MAX_VAL_D          8


#define XLG_MAC_CTRL5_PREAMBLE_FLD_LEN_D          3
#define XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_OFFS_D  4
#define XLG_MAC_CTRL5_PREAMBLE_LEN_TX_FLD_LEN_D   XLG_MAC_CTRL5_PREAMBLE_FLD_LEN_D
#define XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_OFFS_D  7
#define XLG_MAC_CTRL5_PREAMBLE_LEN_RX_FLD_LEN_D   XLG_MAC_CTRL5_PREAMBLE_FLD_LEN_D


#define XLG_MAC_CTRL5_NUM_CRC_MIN_VAL_D           1
#define XLG_MAC_CTRL5_NUM_CRC_MAX_VAL_D           4

#define XLG_MAC_CTRL5_NUM_CRC_BYTES_FLD_LEN_D     3
#define XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_OFFS_D 10
#define XLG_MAC_CTRL5_TX_NUM_CRC_BYTES_FLD_LEN_D  XLG_MAC_CTRL5_NUM_CRC_BYTES_FLD_LEN_D
#define XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_OFFS_D 13
#define XLG_MAC_CTRL5_RX_NUM_CRC_BYTES_FLD_LEN_D  XLG_MAC_CTRL5_NUM_CRC_BYTES_FLD_LEN_D

/*******************************************************************************
* prvCpssDxChBobcat2PortMacIsSupported
*
* DESCRIPTION:
*       check whether XLG mac is supported for specific port
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portType - port type (mac)
*
* OUTPUTS:
*       isSupportedPtr - pointer to is supported
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacIsSupported
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              portNum,
    IN PRV_CPSS_PORT_TYPE_ENT            portType,
    OUT GT_BOOL                         *isSupportedPtr
);


/*******************************************************************************
* prvCpssDxChBobcat2PortMacIPGLengthSet
*
* DESCRIPTION:
*       set XLG mac IPG length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       length   = ipg length in bytes
*
* OUTPUTS:
*       isSupportedPtr.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacIPGLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN GT_U32                   length
);

/*******************************************************************************
* prvCpssDxChBobcat2PortMacIPGLengthGet
*
* DESCRIPTION:
*       get XLG mac IPG length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portType - port type
*
* OUTPUTS:
*       lengthPtr   = pointer to length in bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacIPGLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN PRV_CPSS_PORT_TYPE_ENT   portType,
    IN GT_U32                  *lengthPtr
);


/*******************************************************************************
* prvCpssDxChBobcat2PortMacPreambleLengthSet
*
* DESCRIPTION:
*       set XLG mac Preable length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       direction - direction (RX/TX/both)
*       length   = ipg length in bytes
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacPreambleLengthSet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                   length
);

/*******************************************************************************
* prvCpssDxChBobcat2PortMacPreambleLengthGet
*
* DESCRIPTION:
*       get XLG mac Preable length
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       portType - port type
*       direction - direction (RX/TX/both)
*
* OUTPUTS:
*       lengthPtr - preable ipg length in bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacPreambleLengthGet
(
    IN GT_U8                    devNum,
    IN GT_PHYSICAL_PORT_NUM     portNum,
    IN PRV_CPSS_PORT_TYPE_ENT   portType,
    IN CPSS_PORT_DIRECTION_ENT  direction,
    IN GT_U32                  *lengthPtr
);


/*******************************************************************************
* prvCpssDxChBobcat2PortMacCrcModeSet
*
* DESCRIPTION:
*       Set XLG mac CRC number of bytes
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       direction - direction (RX/TX/both)
*       numCrcBytes - number of bytes
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacCrcModeSet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN GT_U32                            numCrcBytes
);

/*******************************************************************************
* prvCpssDxChBobcat2PortMacCrcModeGet
*
* DESCRIPTION:
*       Get XLG mac CRC number of bytes
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       direction - direction (RX/TX/both)
*
* OUTPUTS:
*       numCrcBytesPtr - number of bytes
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChBobcat2PortMacCrcModeGet
(
    IN GT_U8                             devNum,
    IN GT_PHYSICAL_PORT_NUM              portNum,
    IN CPSS_PORT_DIRECTION_ENT           portDirection,
    IN GT_U32                           *numCrcBytesPtr
);


/*******************************************************************************
* prvCpssDxChPortMacConfiguration
*
* DESCRIPTION:
*        Write value to register field and duplicate it to other members of SW 
*           combo if needed
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       regDataArray - array of register's address/offset/field lenght/value to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on NULL ptr
*       GT_HW_ERROR - if write failed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacConfiguration
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,     
    IN  const PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   *regDataArray
);

/*******************************************************************************
* prvCpssDxChPortMacConfigurationClear
*
* DESCRIPTION:
*        Clear array of registers data
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; Lion2; xCat2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       regDataArray - "clean" array of register's data
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PTR  - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChPortMacConfigurationClear
(
    INOUT PRV_CPSS_DXCH_PORT_REG_CONFIG_STC   *regDataArray
);

/*******************************************************************************
* prvCpssDxChMacByPhysPortGet
*
* DESCRIPTION:
*       get mac by port number 
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; Lion2; xCat2.
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       macPtr   - pointer to is supported
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_BAD_PTR      - on bad ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChMacByPhysPortGet
(
    IN  GT_U8                    devNum,
    IN  GT_PHYSICAL_PORT_NUM     portNum,
    OUT GT_U32                  *macPtr
);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
