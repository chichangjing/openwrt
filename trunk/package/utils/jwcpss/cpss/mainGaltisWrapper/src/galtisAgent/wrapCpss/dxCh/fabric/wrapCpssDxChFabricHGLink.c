/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChFabricHGLink.c
*
* DESCRIPTION:
*       Galtis wrappers for CPSS DxCh Fabric Connectivity HyperG.Link interface 
*       API.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/fabric/cpssDxChFabricHGLink.h>

/*******************************************************************************
* cpssDxChFabricHGLinkPcsLoopbackEnableSet
*
* DESCRIPTION:
*       Enable/disable PCS loopback on Hyper.G Link 
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkPcsLoopbackEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkPcsLoopbackEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkPcsLoopbackEnableGet
*
* DESCRIPTION:
*       Get PCS loopback on Hyper.G Link status
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkPcsLoopbackEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkPcsLoopbackEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkPcsRxStatusGet
*
* DESCRIPTION:
*       Get the XPCS lanes synchronization status
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkPcsRxStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     laneSyncOkArr[CPSS_DXCH_FABRIC_HGL_SERDES_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkPcsRxStatusGet(devNum, portNum, laneSyncOkArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "status of lanes[0-5]: %d,%d,%d,%d,%d,%d", laneSyncOkArr[0], 
                    laneSyncOkArr[1], laneSyncOkArr[2], laneSyncOkArr[3], 
                    laneSyncOkArr[4], laneSyncOkArr[5]);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkRxErrorCntrGet
*
* DESCRIPTION:
*       Get the XPCS lanes synchronization status
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkRxErrorCntrGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_RX_ERROR_CNTRS_STC rxErrorsCntrs;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkRxErrorCntrGet(devNum, portNum, &rxErrorsCntrs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "badHeaderCells=0x%x,badLengthCells=0x%x,\
                    reformatErrorCells=0x%x", rxErrorsCntrs.badHeaderCells, 
                    rxErrorsCntrs.badLengthCells, rxErrorsCntrs.reformatErrorCells);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkConfigTxEccTypeSet
*
* DESCRIPTION:
*    This routine defines how to calculate
*    the error correction on the HyperG.Link cell
*    Both Rx and Tx.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkConfigEccTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT eccType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    eccType  = (CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkConfigEccTypeSet(devNum, portNum, eccType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkConfigEccTypeGet
*
* DESCRIPTION:
*    Get current type of the error correction on the HyperG.Link cell
*    Both Rx and Tx.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkConfigEccTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_ECC_TYPE_ENT eccType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkConfigEccTypeGet(devNum, portNum, &eccType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", eccType);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkRxCrcCheckEnableSet
*
* DESCRIPTION:
*    Enable/disable CRC check of received cells.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkRxCrcCheckEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkRxCrcCheckEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkRxCrcCheckEnableGet
*
* DESCRIPTION:
*    Get status of CRC check of received cells.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkRxCrcCheckEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkRxCrcCheckEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkLbiEnableSet
*
* DESCRIPTION:
*    Defines if the forwarding is affected from the Load Balancing Index (LBI) 
*       field in the cell header
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkLbiEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkLbiEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkLbiEnableGet
*
* DESCRIPTION:
*    Defines if the forwarding is affected from the Load Balancing Index (LBI) 
*       field in the cell header
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkLbiEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkLbiEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkCellsCntrTypeSet
*
* DESCRIPTION:
*    Configure the types of received/transmitted cells to be 
*       counted in the received/transmitted cell counter
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkCellsCntrTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT  cellDirection;
    CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT  cellType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    cellDirection  = (CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT)inArgs[2];
    cellType  = (CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkCellsCntrTypeSet(devNum, portNum, 
                                                    cellDirection, cellType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkCellsCntrTypeGet
*
* DESCRIPTION:
*    Get the types of received/transmitted cells
*       counted in the received/transmitted cell counter
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkCellsCntrTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT  cellDirection;
    CPSS_DXCH_FABRIC_HGL_CELL_COUNTER_TYPE_ENT  cellType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    cellDirection  = (CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkCellsCntrTypeGet(devNum, portNum, 
                                            cellDirection, &cellType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cellType);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkCellsCntrGet
*
* DESCRIPTION:
*    Get the counter of received/transmitted cells
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkCellsCntrGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      cellCntr;
    CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT  cellDirection;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    cellDirection  = (CPSS_DXCH_FABRIC_HGL_DIRECTION_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkCellsCntrGet(devNum, portNum, cellDirection, &cellCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", cellCntr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkFcMacSaSet
*
* DESCRIPTION:
*    Configure the MAC-SA of LL-FC and E2E constructed packets
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkFcMacSaSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_ETHERADDR saMac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    galtisMacAddr(&saMac, (GT_U8*)inArgs[2]);

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkFcMacSaSet(devNum, portNum, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkFcMacSaGet
*
* DESCRIPTION:
*    Configure the MAC-SA of LL-FC and E2E constructed packets
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkFcMacSaGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_ETHERADDR saMac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkFcMacSaGet(devNum, portNum, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b", saMac.arEther);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkRxDsaParamsSet
*
* DESCRIPTION:
*    Configure some Fabric RX DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
*   Also used for E2E-FC and LL-FC DSA Tag para,eters configuration
*******************************************************************************/
static CMD_STATUS wrCpssDxChFabricHGLinkRxDsaParamsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_RX_DSA_STC  dsaParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    dsaParams.vid  = (GT_U32)inArgs[2];
    dsaParams.srcId  = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkRxDsaParamsSet(devNum, portNum, &dsaParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkRxDsaParamsGet
*
* DESCRIPTION:
*    Get some configurable Fabric RX DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkRxDsaParamsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_RX_DSA_STC  dsaParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkRxDsaParamsGet(devNum, portNum, &dsaParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "vid=%d,srcId=%d", dsaParams.vid, dsaParams.srcId);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkFcRxE2eParamsSet
*
* DESCRIPTION:
*    Configure some Fabric FC RX E2E header fields
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkFcRxE2eParamsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC e2eParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    e2eParams.cpId  = (GT_U32)inArgs[2];
    e2eParams.etherType  = (GT_U32)inArgs[3];
    e2eParams.macLowByte  = (GT_U8)inArgs[4];
    e2eParams.prioVlanId  = (GT_U32)inArgs[5];
    e2eParams.version  = (GT_U32)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkFcRxE2eParamsSet(devNum, portNum, &e2eParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkFcRxE2eParamsGet
*
* DESCRIPTION:
*    Get some Fabric FC RX E2E header fields
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkFcRxE2eParamsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_FC_RX_E2E_HEADER_STC e2eParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkFcRxE2eParamsGet(devNum, portNum, &e2eParams);    

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "cpId-0x%x,etherType-0x%x,macLowByte-0x%x,\
                    prioVlanId-0x%x,version-0x%x", e2eParams.cpId, 
                    e2eParams.etherType, e2eParams.macLowByte, 
                    e2eParams.prioVlanId, e2eParams.version);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkRxLinkLayerFcParamsSet
*
* DESCRIPTION:
*    Configure some Fabric Link Layer FC RX packet and DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkRxLinkLayerFcParamsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC llParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    llParams.etherType  = (GT_U32)inArgs[2];
    llParams.pfcOpcode  = (GT_U32)inArgs[3];
    llParams.qosProfile = (GT_U32)inArgs[4];
    llParams.trgDev     = (GT_U8)inArgs[5];
    llParams.trgPort    = (GT_U8)inArgs[6];
    llParams.up         = (GT_U32)inArgs[7];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkRxLinkLayerFcParamsSet(devNum, portNum, &llParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkRxLinkLayerFcParamsGet
*
* DESCRIPTION:
*    Get configurable Fabric Link Layer FC RX packet and DSA tag fields
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkRxLinkLayerFcParamsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_FC_RX_LINK_LAYER_PARAMS_STC llParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkRxLinkLayerFcParamsGet(devNum, portNum, &llParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "etherType-0x%x,pfcOpcode-0x%x,qosProfile-0x%x,\
                    trgDev-0x%x,trgPort-0x%x,up-0x%x", 
                    llParams.etherType, llParams.pfcOpcode, llParams.qosProfile,
                    llParams.trgDev, llParams.trgPort,
                    llParams.up);
    return CMD_OK;
}

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
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkPcsMaxIdleCntSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      maxIdleCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    maxIdleCnt = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkPcsMaxIdleCntSet(devNum, portNum, maxIdleCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkPcsMaxIdleCntGet
*
* DESCRIPTION:
*       Get the number of cycles between ||k|| sequences.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkPcsMaxIdleCntGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_U32      maxIdleCnt;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkPcsMaxIdleCntGet(devNum, portNum, &maxIdleCnt);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxIdleCnt);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkPcsMaxIdleCntEnableSet
*
* DESCRIPTION:
*       Enable/Disable the Idle counter in the Tx SM. When Idle counter is 
*           disabled, it is the MACs responsibilty to generate idles.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkPcsMaxIdleCntEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkPcsMaxIdleCntEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkPcsMaxIdleCntEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of the Idle counter in the Tx SM. When Idle 
*           counter is disabled, it is the MACs responsibilty to generate idles.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkPcsMaxIdleCntEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkPcsMaxIdleCntEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChFabricHGLinkDescriptorTypeSet
*
* DESCRIPTION:
*    Configure the descriptor type.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkDescriptorTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT    type;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    type  = (CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkDescriptorTypeSet(devNum, portNum, type);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
 

/*******************************************************************************
* cpssDxChFabricHGLinkDescriptorTypeGet
*
* DESCRIPTION:
*    Gets the descriptor type.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
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
static CMD_STATUS wrCpssDxChFabricHGLinkDescriptorTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM       portNum;
    CPSS_DXCH_FABRIC_HGL_DESCRIPTOR_TYPE_ENT    type;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChFabricHGLinkDescriptorTypeGet(devNum, portNum, &type);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", type);
    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssDxChFabricHGLinkPcsLoopbackEnableSet",
        &wrCpssDxChFabricHGLinkPcsLoopbackEnableSet,
        3, 0},

    {"cpssDxChFabricHGLinkPcsLoopbackEnableGet",
        &wrCpssDxChFabricHGLinkPcsLoopbackEnableGet,
        2, 0},

    {"cpssDxChFabricHGLinkPcsRxStatusGet",
        &wrCpssDxChFabricHGLinkPcsRxStatusGet,
        2, 0},

    {"cpssDxChFabricHGLinkRxErrorCntrGet",
        &wrCpssDxChFabricHGLinkRxErrorCntrGet,
        2, 0},

    {"cpssDxChFabricHGLinkConfigEccTypeSet",
        &wrCpssDxChFabricHGLinkConfigEccTypeSet,
        3, 0},

    {"cpssDxChFabricHGLinkConfigEccTypeGet",
        &wrCpssDxChFabricHGLinkConfigEccTypeGet,
        2, 0},

    {"cpssDxChFabricHGLinkRxCrcCheckEnableSet",
        &wrCpssDxChFabricHGLinkRxCrcCheckEnableSet,
        3, 0},

    {"cpssDxChFabricHGLinkRxCrcCheckEnableGet",
        &wrCpssDxChFabricHGLinkRxCrcCheckEnableGet,
        2, 0},

    {"cpssDxChFabricHGLinkLbiEnableSet",
        &wrCpssDxChFabricHGLinkLbiEnableSet,
        3, 0},

    {"cpssDxChFabricHGLinkLbiEnableGet",
        &wrCpssDxChFabricHGLinkLbiEnableGet,
        2, 0},

    {"cpssDxChFabricHGLinkCellsCntrTypeSet",
        &wrCpssDxChFabricHGLinkCellsCntrTypeSet,
        4, 0},

    {"cpssDxChFabricHGLinkCellsCntrTypeGet",
        &wrCpssDxChFabricHGLinkCellsCntrTypeGet,
        3, 0},

    {"cpssDxChFabricHGLinkCellsCntrGet",
        &wrCpssDxChFabricHGLinkCellsCntrGet,
        3, 0},

    {"cpssDxChFabricHGLinkFcMacSaSet",
        &wrCpssDxChFabricHGLinkFcMacSaSet,
        3, 0},

    {"cpssDxChFabricHGLinkFcMacSaGet",
        &wrCpssDxChFabricHGLinkFcMacSaGet,
        2, 0},

    {"cpssDxChFabricHGLinkRxDsaParamsSet",
        &wrCpssDxChFabricHGLinkRxDsaParamsSet,
        4, 0},

    {"cpssDxChFabricHGLinkRxDsaParamsGet",
        &wrCpssDxChFabricHGLinkRxDsaParamsGet,
        2, 0},

    {"cpssDxChFabricHGLinkFcRxE2eParamsSet",
        &wrCpssDxChFabricHGLinkFcRxE2eParamsSet,
        7, 0},

    {"cpssDxChFabricHGLinkFcRxE2eParamsGet",
        &wrCpssDxChFabricHGLinkFcRxE2eParamsGet,
        2, 0},

    {"cpssDxChFabricHGLinkRxLinkLayerFcParamsSet",
        &wrCpssDxChFabricHGLinkRxLinkLayerFcParamsSet,
        8, 0},

    {"cpssDxChFabricHGLinkRxLinkLayerFcParamsGet",
        &wrCpssDxChFabricHGLinkRxLinkLayerFcParamsGet,
        2, 0},

    {"cpssDxChFabricHGLinkPcsMaxIdleCntEnableSet",
        &wrCpssDxChFabricHGLinkPcsMaxIdleCntEnableSet,
        3, 0},

    {"cpssDxChFabricHGLinkPcsMaxIdleCntEnableGet",
        &wrCpssDxChFabricHGLinkPcsMaxIdleCntEnableGet,
        2, 0},

    {"cpssDxChFabricHGLinkPcsMaxIdleCntSet",
        &wrCpssDxChFabricHGLinkPcsMaxIdleCntSet,
        3, 0},

    {"cpssDxChFabricHGLinkPcsMaxIdleCntGet",
        &wrCpssDxChFabricHGLinkPcsMaxIdleCntGet,
        2, 0},

    {"cpssDxChFabricHGLinkDescriptorTypeSet",
        &wrCpssDxChFabricHGLinkDescriptorTypeSet,
        3, 0},

    {"cpssDxChFabricHGLinkDescriptorTypeGet",
        &wrCpssDxChFabricHGLinkDescriptorTypeGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChFabricHGLink
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitCpssDxChFabricHGLink
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}




