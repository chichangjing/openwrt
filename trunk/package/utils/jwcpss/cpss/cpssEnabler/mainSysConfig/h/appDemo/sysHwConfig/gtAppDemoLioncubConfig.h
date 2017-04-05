/*******************************************************************************
*                Copyright 2009, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* gtAppDemoLioncubConfig.h
*
* DESCRIPTION:
*       This file includes  lioncub configuration for Lion 48 board
*
* DEPENDENCIES:
*
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#ifndef __gtAppDemoLioncubConfigh
#define __gtAppDemoLioncubConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
    GT_U32 firstRegAddr;
    GT_U32 lastRegAddr;
    GT_U32 data;
    GT_U32 waitTime;
}GT_LIONCUB_INIT; 

typedef enum
{
    CPSS_LIONCUB_REV_A0_E,
    CPSS_LIONCUB_REV_A1_E,
    CPSS_LIONCUB_REV_WRONG_E
}CPSS_LIONCUB_REV_ENT;

/*******************************************************************************
* lioncubRevisionGet
*
* DESCRIPTION:
*      Check lioncub revision
*
* INPUTS:
*       dev - device number
*       portGroup - port group number
*
* OUTPUTS:
*       rev - revision of Lioncub
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lioncubRevisionGet
(
    GT_U8  dev,
    GT_U32  portGroup,
    CPSS_LIONCUB_REV_ENT *rev
);

/*******************************************************************************
* lioncubRegRead
*
* DESCRIPTION:
*      Reaf lioncub register - Indirect Read from Lioncub SMI
*
* INPUTS:
*       dev - device number
*       port - Port number
*       regAddr - register address
*       data - pointer where to store readen data
*
* OUTPUTS:
*       data - pointer to place data from read operation
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - fail.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lioncubRegRead
(
    GT_U8 dev,
    GT_U8 port,
    GT_U32 regAddr,
    GT_U32 *data
);

/*******************************************************************************
* lionSmiCoreWrite
*
* DESCRIPTION:
*      Write to Lioncub register - Indirect Write to Lioncub SMI
*
* INPUTS:
*       dev - device number
*       port - Port number
*       regAddr - register address
*       data - data to write
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - fail.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lioncubRegWrite
(
    GT_U8 dev,
    GT_U8 port,
    GT_U32 regAddr,
    GT_U32 *data
);

/*******************************************************************************
* lioncubConnected
*
* DESCRIPTION:
*      Check if lioncub connected to given portGroup
*
* INPUTS:
*       dev - device number
*    portGroup - port group number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE   - if exists
*       GT_FALSE - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL lioncubConnected
(
    GT_U8 dev,
    GT_U32 portGroup
);

/*******************************************************************************
* lioncubPortReset
*
* DESCRIPTION:
*      Reset lioncub RXAUI port
*
* INPUTS:
*       dev - device number
*       port - port number
*       xpcsPort - up port of lioncub, can 0 or 1
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lioncubPortReset
(
    GT_U8  dev,
    GT_U32 port,
    GT_U8  xpcsPort
);

/*******************************************************************************
* lioncubInit
*
* DESCRIPTION:
*      Init lioncub board
*
* INPUTS:
*       dev - device number
*       portGroup - port group
*       lioncubInitArray - array of reg addresses and values to init LionCub
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lioncubInit
(
    GT_U8   dev,
    GT_U32  portGroup,
    GT_LIONCUB_INIT *lioncubInitArray
);

/*******************************************************************************
* lioncubInitPort
*
* DESCRIPTION:
*      Init one port of lioncub
*
* INPUTS:
*       dev - device number
*       portNum - global port number
*       lioncubInitArray - array of reg addresses and values to init LionCub
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS lioncubInitPort
(
    IN GT_U8   dev,
    IN GT_U32  portNum,
    IN GT_LIONCUB_INIT *lioncubInitArray
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*  __gtAppDemoLioncubConfigh */
