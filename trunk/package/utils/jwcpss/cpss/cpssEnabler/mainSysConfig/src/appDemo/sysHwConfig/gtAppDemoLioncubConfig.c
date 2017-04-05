/*******************************************************************************
*              (c), Copyright 2009, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoLioncubConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Lioncub board.
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*
*******************************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/smi/cpssGenSmi.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <appDemo/sysHwConfig/gtAppDemoLioncubConfig.h>

#ifndef ASIC_SIMULATION_NO_HW
/*******************************************************************************
 * Local variables
 ******************************************************************************/

static GT_BOOL lioncubExists[] = {GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE};
static GT_BOOL lioncubDetectionDone = GT_FALSE;

#define GT_LIONCUB_A0_DEVICE_ID 0xE013
#define GT_LIONCUB_A1_DEVICE_ID 0xE800

/* smi addresses of lioncub devices - for single port group */
GT_U8 *lioncubSmiAddr = NULL;

/* XPCS port of Lioncub - Up0 or Up1 - for both DB and RD boards */
GT_U32 *lion48LcXpcsPortArrayPtr = NULL;

#endif /* #ifndef ASIC_SIMULATION_NO_HW */


#ifndef ASIC_SIMULATION_NO_HW
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
)
{
    GT_STATUS rc = GT_OK;
    GT_U32  devId, smiAddr, data = 0;

    /* enough to check one port of every group */
    smiAddr = lioncubSmiAddr[portGroup*16];

    if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E,
                                        smiAddr, 4, &data)) != GT_OK)
    {
        osPrintf("lioncubRevisionGet:cpssSmiRegisterRead:rc=0x%x\n", rc);
        return rc;
    }

    devId = (data & 0x000FFFF0) >> 4;

    switch (devId)
    {
        case GT_LIONCUB_A0_DEVICE_ID:
            *rev = CPSS_LIONCUB_REV_A0_E;
            break;
        case GT_LIONCUB_A1_DEVICE_ID:
            *rev = CPSS_LIONCUB_REV_A1_E;
            break;
        default:
            *rev = CPSS_LIONCUB_REV_WRONG_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* lioncubDetection
*
* DESCRIPTION:
*      Check where we have lioncub connected
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lioncubDetection
(
    IN GT_U8 dev
)
{
    GT_STATUS rc;
    GT_U32  portGroup;
    CPSS_LIONCUB_REV_ENT rev;

    /* read register 4 (device id) for every lioncub connected to port 0 of every port group */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroup)
    {
        if(CPSS_98CX8203_CNS == PRV_CPSS_PP_MAC(dev)->devType)
        {
            if( (2 == portGroup) || (3 == portGroup) )
                continue;
        }

        if((rc = lioncubRevisionGet(dev, portGroup, &rev))!= GT_OK)
        {
            osPrintf("lioncubDetection: lioncubRevisionGet failed, rc=0x%x\n", rc);
            return rc;
        }

        if (rev != CPSS_LIONCUB_REV_WRONG_E)
        {
            lioncubExists[portGroup] = GT_TRUE;
            osPrintf("Lioncub connected to port group %d.\n",portGroup);
        }
        else
        {
            lioncubExists[portGroup] = GT_FALSE;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroup)

    return GT_OK;
}
#endif

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
    IN GT_U8 dev,
    IN GT_U32 portGroup
)
{
#ifndef ASIC_SIMULATION_NO_HW
    if (lioncubDetectionDone == GT_FALSE)
    {
        lioncubDetection(dev);
        lioncubDetectionDone = GT_TRUE;
    }

    return (lioncubExists[portGroup]);
#else
    return GT_TRUE;
#endif
}

#ifndef ASIC_SIMULATION_NO_HW

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
    IN GT_U8 dev,
    IN GT_U32 port,
    IN GT_U8  xpcsPort
)
{
    GT_U32 data, data1, reg0, reg1;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 0;
    GT_U32  smiAddr;
    GT_STATUS rc;

    portGroupsBmp |= (1 << PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port));
    smiAddr = lioncubSmiAddr[port];

    /* lioncub RXAUI port reset sequence: */
    if (xpcsPort == 0)
    {
        reg0 = 0x1004;
        reg1 = 0x1404;
    }
    else
    {
        reg0 = 0x1804;
        reg1 = 0x1C04;
    }

    smiAddr = lioncubSmiAddr[port];
    if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, reg0, &data)) != GT_OK)
        return rc;

    data &= ~(1<<6); /* reset bit 6 */

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, reg0, data)) != GT_OK)
        return rc;

    if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, reg1, &data1)) != GT_OK)
        return rc;

    data1 &= ~(1<<6); /* reset bit 6 */

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, reg1, data1)) != GT_OK)
        return rc;

    osTimerWkAfter(100);

    data  |= (1<<6); /* set bit 6 */
    data1 |= (1<<6); /* set bit 6 */

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, reg0, data)) != GT_OK)
        return rc;

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, reg1, data1)) != GT_OK)
        return rc;

    return GT_OK;
}

/*******************************************************************************
* lioncubPortResetSet
*
* DESCRIPTION:
*      Reset single lioncub port
*
* INPUTS:
*       dev - device number
*       port - port number
*       serdesResetEn       - GT_TRUE - serdes reset, GT_FALSE - serdes un-reset
*       xpcsResetEn         - GT_TRUE - xpcs reset, GT_FALSE - xpcs un-reset
*       sgmiiFifoResetEn    - GT_TRUE - sgmiiFifo reset, xgmii un-reset;
*                               GT_FALSE - sgmiiFifo un-reset, xgmii reset;
*       xgmiiFifoResetEn    - GT_TRUE - xgmii reset;
*                               GT_FALSE - xgmii un-reset;
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
static GT_STATUS lioncubPortResetSet
(
    IN GT_U8    dev,
    IN GT_U32   port,
    IN GT_BOOL  serdesResetEn,
    IN GT_BOOL  xpcsResetEn,
    IN GT_BOOL  sgmiiFifoResetEn,
    IN GT_BOOL  xgmiiFifoResetEn
)
{
    GT_U32      data, i, regAddr;
    GT_PORT_GROUPS_BMP  portGroupsBmp = 0;
    GT_U32      smiAddr, localPort, phyPort;
    GT_STATUS   rc;

    portGroupsBmp |= (1 << PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port));
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(dev,port);
    phyPort = localPort + (16*PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, port));
    smiAddr = lioncubSmiAddr[phyPort];

    /* 1. Reset port <Upper Lane[n] RESETn> = 0x0.
        – Register offset 0x60 bits [3:0] - change the relevant bits.
        – Bits [1:0] for port 0, bits [3:2] for port 1 - every two lanes are one port
    */
    if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x60, &data)) != GT_OK)
        return rc;

    if(GT_TRUE == sgmiiFifoResetEn)
        data &= ~(0x3 << 2*lion48LcXpcsPortArrayPtr[port]); /* clear bits [1:0] or [3:2] */
    else
        data |= (0x3 << 2*lion48LcXpcsPortArrayPtr[port]); /* set bits [1:0] or [3:2] up - for SGMII only one lane needed, but
                                      additional lane won't do much harm - just elictricity spent */

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x60, data)) != GT_OK)
        return rc;

    cpssOsTimerWkAfter(10);

    /* De-assert XGMII fifo <XGMII_Fifo_Control> = 0x1.
        – Register offset 0x64 bits [3:0] - change the relevant bits.
        – Bits [1:0] for port 0, bits [3:2] for port 1 - every two lanes are one port.
    */
    if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x64, &data)) != GT_OK)
        return rc;

    if(GT_TRUE == xgmiiFifoResetEn)
        data &= ~(0x3 << 2*lion48LcXpcsPortArrayPtr[port]); /* clear bits [1:0] or [3:2] */
    else
        data |= (0x3 << 2*lion48LcXpcsPortArrayPtr[port]); /* set bits [1:0] or [3:2] up */

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x64, data)) != GT_OK)
        return rc;

    cpssOsTimerWkAfter(10);

    /* 1. Set XPCS to Reset < PCSReset_> = 0x0.
        – Both "Up" and "Down" ports.
        - Port 0 of pair:
        – Register offset 0x200 bit [0].
        – Register offset 0x400 bit [0].
        - Port 1 of pair
        – Register offset 0x600 bit [0].
        – Register offset 0x800 bit [0].
    */
    regAddr = (lion48LcXpcsPortArrayPtr[port] == 0) ? 0x200 : 0x600;
    if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
        return rc;

    if(GT_TRUE == xpcsResetEn)
        data &= 0xFFFFFFFE; /* set bit 0 = 0x0*/
    else
        data |= 0x1; /* set bit 0 = 0x1*/

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
        return rc;

    regAddr = (lion48LcXpcsPortArrayPtr[port] == 0) ? 0x400 : 0x800;
    if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
        return rc;

    if(GT_TRUE == xpcsResetEn)
        data &= 0xFFFFFFFE; /* set bit 0 = 0x0*/
    else
        data |= 0x1; /* set bit 0 = 0x1*/

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
        return rc;

    cpssOsTimerWkAfter(10);

    /* 2. Set SERDES to Reset <sd_reset_in_> = 0x0.
        – This configuration should be performed on both the "UP" and "Down" ports.
        – Bit [3] of the registers at offsets 0x1004 and 0x1404 (Even ports - Two SERDES per RXAUI interface); 0x1804 and 0x1C04 (odd ports).
        – Bit [3] of the registers at offsets 0x2004, 0x2404, 0x2804 and 0x2C04 (Even ports - Four SERDES per XAUI interface).
        – Bit [3] of the registers at offsets 0x3004, 0x3404, 0x3804 and 0x3C04 (Odd ports).
    */
    for(i = 2*lion48LcXpcsPortArrayPtr[port]; i < (2*lion48LcXpcsPortArrayPtr[port])+2; i++)
    {
        regAddr = 0x1004 + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        if(GT_TRUE == serdesResetEn)
            data &= ~(1<<3); /* reset bit 3 */
        else
            data |= (1<<3); /* set bit 3 */

        if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    for(i = 0; i < 4; i++)
    {
        regAddr = 0x2004 + (0x1000*lion48LcXpcsPortArrayPtr[port]) + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        if(GT_TRUE == serdesResetEn)
            data &= ~(1<<3); /* reset bit 3 */
        else
            data |= (1<<3); /* set bit 3 */

        if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    cpssOsTimerWkAfter(10);

    /* 3. Set SERDES to Reset <sd_reset_register> = 0x0.
        – This configuration should be performed on both the "UP" and "Down" ports.
        – Register offset 0x7C bits [3:2] - change the relevant bit for the "UP" port.
        – Register offset 0x7C bits [1:0] - change the relevant bit for the "Down" port.
    */
    if((rc = cpssSmiRegisterRead(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x7C, &data)) != GT_OK)
        return rc;

    if(GT_TRUE == serdesResetEn)
    {
        if(lion48LcXpcsPortArrayPtr[port])
            data &= 0xFFFFFFF5;
        else
            data &= 0xFFFFFFFA;
    }
    else
    {
        if(lion48LcXpcsPortArrayPtr[port])
            data |= 0xFFFFFFFA;
        else
            data |= 0xFFFFFFF5;
    }

    if((rc = cpssSmiRegisterWrite(dev, portGroupsBmp, CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x7C, data)) != GT_OK)
        return rc;

    cpssOsTimerWkAfter(10);

    return GT_OK;
}
#endif

/*******************************************************************************
* lioncubInitPort
*
* DESCRIPTION:
*      Init one port of lioncub
*
* INPUTS:
*       dev             - device number
*       portNum         - global port number
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
*       At the moment only RXAUI 10G and SGMII 1G combinations supported
*
*******************************************************************************/
GT_STATUS lioncubInitPort
(
    IN GT_U8   dev,
    IN GT_U32  portNum,
    IN GT_LIONCUB_INIT *lioncubInitArray
)
{
#ifndef ASIC_SIMULATION_NO_HW
    GT_U32      regAddr, i, data, j;
    GT_U32      portGroup;
    GT_U32      smiAddr;
    GT_STATUS   rc;
    GT_BOOL     serdesResetEn;
    GT_BOOL     xpcsResetEn;
    GT_BOOL     sgmiiFifoResetEn, xgmiiFifoResetEn;

    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(dev, portNum);

    /* check if current port group connected to lioncub */
    if (lioncubConnected(dev, portGroup) == GT_FALSE)
    {
        return GT_HW_ERROR;
    }

    serdesResetEn       = GT_TRUE;
    xpcsResetEn         = GT_TRUE;
    sgmiiFifoResetEn    = GT_TRUE;
    xgmiiFifoResetEn    = GT_TRUE;

    if((rc = lioncubPortResetSet(dev, portNum, serdesResetEn, xpcsResetEn, sgmiiFifoResetEn, xgmiiFifoResetEn)) != GT_OK)
        return rc;

    smiAddr = lioncubSmiAddr[portNum];

    /* 4. Set PCS to ByPass <PCS_ByPass> = 0x1.
        – Register offset 0x0 bits [7:6] - change the relevant bit.
    */
    regAddr = 0x0;
    if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
        return rc;

    if(lioncubInitArray[4].data == 0x6614) /* if new mode is 1G SGMII enable pcs bypass */
        data |= (1 << (6+lion48LcXpcsPortArrayPtr[portNum]));
    else
        data &= ~(1 << (6+lion48LcXpcsPortArrayPtr[portNum]));

    if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
        return rc;

    /* 5. Set mode to 10 bit <SDSel10bitMode> = 0x1.
        – This configuration should be performed on both the "UP" and "Down" ports.
        – Bit [15] of the register at offsets 0x1000 for the "UP" port.
        – Bit [15] of the register at offsets 0x2000 for the "Down" port.
    */
    for(i = 2*lion48LcXpcsPortArrayPtr[portNum]; i < (2*lion48LcXpcsPortArrayPtr[portNum])+2; i++)
    {
        regAddr = 0x1000 + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        if(lioncubInitArray[4].data == 0x6614) /* if new mode is 1G SGMII set 10bit mode */
            data |= (1<<15);
        else
            data &= ~(1<<15);

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    for(i = 0; i < 4; i++)
    {
        regAddr = 0x2000 + (0x1000*lion48LcXpcsPortArrayPtr[portNum]) + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        if(lioncubInitArray[4].data == 0x6614)
            data |= (1<<15);
        else
            data &= ~(1<<15);

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    /* 6. Set Interface Mode to XAUI <rf_media_ifc_mode> = 0x0.
        – This configuration should be performed on both the "UP" and "Down" ports.
        – Bit [5] of the register at offset 0x1014 for the "UP" port.
        – Bit [5] of the register at offset 0x2014 for the "Down" port
    */
    for(i = 2*lion48LcXpcsPortArrayPtr[portNum]; i < (2*lion48LcXpcsPortArrayPtr[portNum])+2; i++)
    {
        regAddr = 0x1014 + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        if(lioncubInitArray[4].data == 0x6614) /* if new mode is 1G SGMII set Interface Mode to XAUI  */
            data &= ~(1<<5);
        else
            data |= (1<<5);

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    for(i = 0; i < 4; i++)
    {
        regAddr = 0x2014 + (0x1000*lion48LcXpcsPortArrayPtr[portNum]) + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        /* down ports stay in XAUI mode always */
        data &= ~(1<<5);

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }
#if 0
    /* 7. De-assert SERDES Reset <sd_reset_in_> = 0x1.
        – This configuration should be performed on both the "UP" and "Down" ports.
        – Register offset 0x1004 bit [3] for the "UP" port.
        – Register offset 0x2004 bit [3] for the "Down" port.
    */
    serdesResetEn       = GT_FALSE;
    xpcsResetEn         = GT_TRUE;
    sgmiiFifoResetEn    = GT_TRUE;
    xgmiiFifoResetEn    = GT_TRUE;

    if((rc = lioncubPortResetSet(dev, portNum, serdesResetEn, xpcsResetEn, sgmiiFifoResetEn, xgmiiFifoResetEn)) != GT_OK)
        return rc;

    cpssOsTimerWkAfter(10);
#endif
    /* Set lioncub configuration */
    for (i = 0; lioncubInitArray[i].firstRegAddr != 0xffff; i++)
    {
        data = lioncubInitArray[i].data;

        if(lioncubInitArray[i].firstRegAddr == 0x7c)
        {
            if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x7C, &data)) != GT_OK)
                return rc;

            if(lion48LcXpcsPortArrayPtr[portNum])
                data |= 0xFFFFFFFA;
            else
                data |= 0xFFFFFFF5;

            if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, 0x7C, data)) != GT_OK)
                return rc;
        }
        else if((lioncubInitArray[i].firstRegAddr & 0xf000) == 0x1000) /* configure up port */
        {
            for(j = 2*lion48LcXpcsPortArrayPtr[portNum]; j < (2*lion48LcXpcsPortArrayPtr[portNum])+2; j++)
            {
                regAddr = lioncubInitArray[i].firstRegAddr + j*0x400;

                if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
                    return rc;
            }
        }
        else if((lioncubInitArray[i].firstRegAddr & 0xf000) == 0x2000) /* configure up port */
        {
            for(j = 0; j < 4; j++)
            {
                regAddr = lioncubInitArray[i].firstRegAddr + (0x1000*lion48LcXpcsPortArrayPtr[portNum]) + j*0x400;

                if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
                    return rc;
            }
        }
        else
        {
            osPrintf("Not supported register address in lioncubInitArray 0x%x (line %d)\n", lioncubInitArray[i].firstRegAddr, i);
            return GT_BAD_VALUE;
        }

        /* wait if delay is not zero */
        if (lioncubInitArray[i].waitTime != 0)
        {
            osTimerWkAfter(lioncubInitArray[i].waitTime);
        }
    }

    /* 10. Set SERDES Init <SDRxInit> = 0x1.
        – This configuration should be performed on both the "UP" and "Down" ports.
        – Bit [4] of the registers at offsets 0x1004 (one SERDES per SGMII interface).
        – Bit [4] of the registers at offsets 0x2004 (one SERDES per SGMII interface).
        – Bit [4] of the registers at offsets 0x3004, 0x3404, 0x3804 and 0x3C04 (Odd ports).
    */
    for(i = 2*lion48LcXpcsPortArrayPtr[portNum]; i < (2*lion48LcXpcsPortArrayPtr[portNum])+2; i++)
    {
        regAddr = 0x1004 + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        data |= (1<<4); /* set bit 4 */

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    for(i = 0; i < 4; i++)
    {
        regAddr = 0x2004 + (0x1000*lion48LcXpcsPortArrayPtr[portNum]) + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        data |= (1<<4); /* set bit 4 */

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    /* 11. Set SERDES Init <SDRxInit> = 0x0.
        – This configuration should be performed on both the "UP" and "Down" ports.
        – Bit [4] of the registers at offsets 0x1004 (one SERDES per SGMII interface).
        – Bit [4] of the registers at offsets 0x2004 (one SERDES per SGMII interface).
    */
    for(i = 2*lion48LcXpcsPortArrayPtr[portNum]; i < (2*lion48LcXpcsPortArrayPtr[portNum])+2; i++)
    {
        regAddr = 0x1004 + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        data &= ~(1<<4); /* set bit 4 */

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    for(i = 0; i < 4; i++)
    {
        regAddr = 0x2004 + (0x1000*lion48LcXpcsPortArrayPtr[portNum]) + i*0x400;

        if((rc = cpssSmiRegisterRead(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, &data)) != GT_OK)
            return rc;

        data &= ~(1<<4); /* set bit 4 */

        if((rc = cpssSmiRegisterWrite(dev, (1 << portGroup), CPSS_PHY_SMI_INTERFACE_0_E, smiAddr, regAddr, data)) != GT_OK)
            return rc;
    }

    cpssOsTimerWkAfter(10);

    serdesResetEn       = GT_FALSE;
    xpcsResetEn         = (lioncubInitArray[4].data == 0x6614) ? GT_TRUE : GT_FALSE;
    sgmiiFifoResetEn    = (lioncubInitArray[4].data == 0x6614) ? GT_FALSE : GT_TRUE;
    xgmiiFifoResetEn    = (lioncubInitArray[4].data == 0x6614) ? GT_TRUE : GT_FALSE;

    if((rc = lioncubPortResetSet(dev, portNum, serdesResetEn, xpcsResetEn, sgmiiFifoResetEn, xgmiiFifoResetEn)) != GT_OK)
        return rc;

#endif
    return GT_OK;
}

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
    IN GT_U8   dev,
    IN GT_U32  portGroup,
    IN GT_LIONCUB_INIT *lioncubInitArray
)
{
    GT_U32  port;    /* local port number in group */
    GT_U32  phyPort; /* global/physical port number */
    GT_STATUS rc;

    /* check if current port group connected to lioncub */
    if (lioncubConnected(dev, portGroup) == GT_FALSE)
    {
        return GT_FAIL;
    }

    for (port = 0; port < 12; port++)
    {
        phyPort = port + (16*portGroup);
        rc = lioncubInitPort(dev, phyPort, lioncubInitArray);
        if(rc != GT_OK)
            return rc;
    }

    return GT_OK;
}

