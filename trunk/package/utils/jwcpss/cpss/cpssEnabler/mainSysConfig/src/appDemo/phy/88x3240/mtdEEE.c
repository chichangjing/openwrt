#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
higher-level functions using MDIO access to control and read 
status of the energy efficient ethernet (EEE) functions of the Marvell 
88X3240 ethernet PHY.
********************************************************************/
#include "mtdApiRegs.h"
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdHwMsecCntl.h"
#include "mtdAPI.h"
#include "mtdEEE.h"


MTD_STATUS mtdGetLPIStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *latchedValue,
    OUT MTD_U16 *currentValue
)
{
    /* reads it once to get the latched value */
    ATTEMPT(mtdHwXmdioRead(contextPtr, port,3,1,latchedValue));    
    /* reads it again to get the current value */
    ATTEMPT(mtdHwXmdioRead(contextPtr, port,3,1,currentValue));    

    return MTD_OK;
}

MTD_STATUS mtdGetTxLPIReceived
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 latchedValue,
    OUT MTD_BOOL *txlpiReceived
)
{
    MTD_U16 result;
    /*Bit 11 in reg 3.1 gives Tx LPI received status.
    1 - received
    0 - not received*/
    ATTEMPT(mtdHwGetRegFieldFromWord(latchedValue,11,1,&result));
    
    result ? (*txlpiReceived = MTD_TRUE) : (*txlpiReceived = MTD_FALSE);

    return MTD_OK;
}

MTD_STATUS mtdGetRxLPIReceived
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 latchedValue,
    OUT MTD_BOOL *rxlpiReceived
)
{
    MTD_U16 result;
    /*Bit 10 in reg 3.1 gives Rx LPI received status.
    1 - received
    0 - not received*/
    ATTEMPT(mtdHwGetRegFieldFromWord(latchedValue,10,1,&result));
    
    result ? (*rxlpiReceived = MTD_TRUE) : (*rxlpiReceived = MTD_FALSE);
    
    return MTD_OK;
}

MTD_STATUS mtdGetTxLPIIndication
(
 IN CTX_PTR_TYPE contextPtr,
 IN MTD_U16 currentValue,
 OUT MTD_BOOL *txlpiIndication
)
{
    MTD_U16 result;
    /*Bit 9 in reg 3.1 gives Tx LPI currently receiving status.
    1 - receiving
    0 - not receiving*/
    ATTEMPT(mtdHwGetRegFieldFromWord(currentValue,9,1,&result));
    
    result ? (*txlpiIndication = MTD_TRUE) : (*txlpiIndication = MTD_FALSE);
    
    return MTD_OK;
}

MTD_STATUS mtdGetRxLPIIndication
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_BOOL *rxlpiIndication
)
{
    MTD_U16 result;
    /*Bit 8 in reg 3.1 gives Rx LPI currently receiving status.
    1 - receiving
    0 - not receiving*/
    ATTEMPT(mtdHwGetRegFieldFromWord(currentValue,8,1,&result));
    
    result ? (*rxlpiIndication = MTD_TRUE) : (*rxlpiIndication = MTD_FALSE);
    
    return MTD_OK;
}

MTD_STATUS mtdWakeErrorCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *wakeErrCount
)
{
    return (mtdHwXmdioRead(contextPtr,port,3,22,wakeErrCount));
}

MTD_STATUS mtdGetEEESupported
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *EEE_support_bits
)
{
    return (mtdHwXmdioRead(contextPtr,port,3,20,EEE_support_bits));
}


MTD_STATUS mtdAdvert_EEE
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 EEE_advertise_bits, 
    IN MTD_BOOL anRestart
)
{
    /* check to make sure only bits supported are set */
    if (EEE_advertise_bits & ~(MTD_EEE_ALL))
    {
        return MTD_FAIL; /* any extra bits set to 1, return error */
    }
    
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,7,0x003C,EEE_advertise_bits));

    if (anRestart == MTD_TRUE)
    {
        ATTEMPT(mtdAutonegRestart(contextPtr,port));
    }

    return MTD_OK;
}

MTD_STATUS mtdGetAdvert_EEE
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *EEE_advertise_bits     
)
{
    return (mtdHwXmdioRead(contextPtr,port,7,0x3C,EEE_advertise_bits));
}

MTD_STATUS mtdGetLP_Advert_EEE
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *LP_EEE_advertise_bits     
)
{
    return (mtdHwXmdioRead(contextPtr,port,7,0x3D,LP_EEE_advertise_bits));
}

MTD_STATUS mtdGetEEEResolution
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *EEE_resolved_bits     
)
{
    MTD_U16 localAdvert, LPAdvert;

    *EEE_resolved_bits = 0;

    ATTEMPT(mtdGetAdvert_EEE(contextPtr,port,&localAdvert));
    ATTEMPT(mtdGetLP_Advert_EEE(contextPtr,port,&LPAdvert));

    *EEE_resolved_bits = (localAdvert & LPAdvert);

    return MTD_OK;
}

MTD_STATUS mtdEeeBufferConfig
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U8 EEE_Mode_Control, 
    IN MTD_U8 XGMII_Enter_Timeout, 
    IN MTD_U8 GMII_Enter_Timeout, 
    IN MTD_U8 MII_Enter_Timeout, 
    IN MTD_U8 XGMII_Exit_Timeout, 
    IN MTD_U8 GMII_Exit_Timeout, 
    IN MTD_U8 MII_Exit_Timeout, 
    IN MTD_U8 IPG_Length 
)
{
    MTD_U16 macSecEnable;

    /* make sure macsec is not on, can't use internal buffer is macsec is also being used */
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,12,2,&macSecEnable));

    if (macSecEnable == 3)
    {
        return MTD_FAIL;
    }


    if (XGMII_Enter_Timeout)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF003,8,8,(MTD_U16)XGMII_Enter_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF003,8,8,(MTD_U16)0x1A));
    }

    if (XGMII_Exit_Timeout)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF005,8,8,(MTD_U16)XGMII_Exit_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF005,8,8,(MTD_U16)0x8));
    }

    if (GMII_Enter_Timeout)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF004,0,8,(MTD_U16)GMII_Enter_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF004,0,8,(MTD_U16)0x12));
    }

    if (GMII_Exit_Timeout)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF006,0,8,(MTD_U16)GMII_Exit_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF006,0,8,(MTD_U16)0x12));
    }
    
    if (MII_Enter_Timeout)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF004,8,8,(MTD_U16)MII_Enter_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF004,8,8,(MTD_U16)0x20));
    }

    if (MII_Exit_Timeout)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF006,8,8,(MTD_U16)MII_Exit_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF006,8,8,(MTD_U16)0x20));
    }

    if (IPG_Length)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF005,0,8,(MTD_U16)IPG_Length));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF005,0,8,(MTD_U16)0x0C));
    }

    switch (EEE_Mode_Control)
    {
        case MTD_EEE_MODE_DISABLE:
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF003,0,2,(MTD_U16)0));
            break;

        case MTD_EEE_MODE_ENABLE_NO_LEGACY:
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF003,0,2,(MTD_U16)2));
            break;

        case MTD_EEE_MODE_ENABLE_WITH_LEGACY:
            ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,31,0xF003,0,2,(MTD_U16)3));
            break;

        default:
            return MTD_FAIL;
            break;
    }

    return MTD_OK;
}


MTD_STATUS mtdGetEeeBufferConfig
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U8 *EEE_Mode_Control, 
    OUT MTD_U8 *XGMII_Enter_Timeout, 
    OUT MTD_U8 *GMII_Enter_Timeout, 
    OUT MTD_U8 *MII_Enter_Timeout, 
    OUT MTD_U8 *XGMII_Exit_Timeout, 
    OUT MTD_U8 *GMII_Exit_Timeout, 
    OUT MTD_U8 *MII_Exit_Timeout, 
    OUT MTD_U8 *IPG_Length 
)
{
    MTD_U16 U16EEE_Mode_Control,
         U16XGMII_Enter_Timeout,
         U16GMII_Enter_Timeout,
         U16MII_Enter_Timeout,
         U16XGMII_Exit_Timeout,
         U16GMII_Exit_Timeout,
         U16MII_Exit_Timeout,
         U16IPG_Length;

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF003,8,8,&U16XGMII_Enter_Timeout));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF005,8,8,&U16XGMII_Exit_Timeout));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF004,0,8,&U16GMII_Enter_Timeout));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF006,0,8,&U16GMII_Exit_Timeout));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF004,8,8,&U16MII_Enter_Timeout));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF006,8,8,&U16MII_Exit_Timeout));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF005,0,8,&U16IPG_Length));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,31,0xF003,0,2,&U16EEE_Mode_Control));

    *EEE_Mode_Control = (MTD_U8)U16EEE_Mode_Control;
    *XGMII_Enter_Timeout = (MTD_U8)U16XGMII_Enter_Timeout;
    *GMII_Enter_Timeout = (MTD_U8)U16GMII_Enter_Timeout;
    *MII_Enter_Timeout = (MTD_U8)U16MII_Enter_Timeout;
    *XGMII_Exit_Timeout = (MTD_U8)U16XGMII_Exit_Timeout;
    *GMII_Exit_Timeout = (MTD_U8)U16GMII_Exit_Timeout;
    *MII_Exit_Timeout = (MTD_U8)U16MII_Exit_Timeout;
    *IPG_Length = (MTD_U8)U16IPG_Length;

    if (*EEE_Mode_Control == 1)
    {
        return MTD_FAIL; /* This doesn't make sense, it means EEE is disabled with legacy buffer enabled */
    }
    else if (*EEE_Mode_Control == 2 || *EEE_Mode_Control == 3)
    {
        *EEE_Mode_Control -= 1;
    }
    /* else it's 0, disabled, don't do anything to it, returns MTD_EEE_MODE_DISABLE */

    return MTD_OK;  
}


MTD_STATUS mtdMACSECEeeBufferConfig
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U8 EEE_Mode_Control, 
    IN MTD_U8 XGMII_Enter_Timeout, 
    IN MTD_U8 GMII_Enter_Timeout, 
    IN MTD_U8 MII_Enter_Timeout, 
    IN MTD_U8 XGMII_Exit_Timeout, 
    IN MTD_U8 GMII_Exit_Timeout, 
    IN MTD_U8 MII_Exit_Timeout, 
    IN MTD_BOOL Force_Sys_LPI, 
    IN MTD_BOOL Force_Wre_LPI 
)
{
    MTD_U16 macSecEnable;

    /* make sure macsec was already turned on */
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,12,2,&macSecEnable));

    if (macSecEnable != 3)
    {
        return MTD_FAIL;
    }

    if (XGMII_Enter_Timeout)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,4,8,(MTD_U32)XGMII_Enter_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,4,8,(MTD_U32)0xFF));
    }

    if (XGMII_Exit_Timeout)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809E,0,8,(MTD_U32)XGMII_Exit_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809E,0,8,(MTD_U32)0x4B));
    }

    if (GMII_Enter_Timeout)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,12,8,(MTD_U32)GMII_Enter_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,12,8,(MTD_U32)0x12));
    }

    if (GMII_Exit_Timeout)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809E,8,8,(MTD_U32)GMII_Exit_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809E,8,8,(MTD_U32)0x12));
    }
    
    if (MII_Enter_Timeout)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,20,8,(MTD_U32)MII_Enter_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,20,8,(MTD_U32)0x20));
    }

    if (MII_Exit_Timeout)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809E,16,8,(MTD_U32)MII_Exit_Timeout));
    }
    else
    {
        /* it was 0, so set it to default */
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809E,16,8,(MTD_U32)0x20));
    }

    if (Force_Sys_LPI == MTD_TRUE)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,3,1,(MTD_U32)1));
    }
    else
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,3,1,(MTD_U32)0));
    }

    if (Force_Wre_LPI == MTD_TRUE)
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,2,1,(MTD_U32)1));
    }
    else
    {
        ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,2,1,(MTD_U32)0));
    }

    switch (EEE_Mode_Control)
    {
        case MTD_EEE_MODE_DISABLE:
            ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,0,2,(MTD_U32)0));
            break;

        case MTD_EEE_MODE_ENABLE_NO_LEGACY:
            ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,0,2,(MTD_U32)2));
            break;

        case MTD_EEE_MODE_ENABLE_WITH_LEGACY:
            ATTEMPT(mtdHwSetMsecPhyRegField(contextPtr,port,31,0x809C,0,2,(MTD_U32)1));
            break;

        default:
            return MTD_FAIL;
            break;
    }

    return MTD_OK;
    
}


MTD_STATUS mtdGetMACSECEeeBufferConfig
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U8 *EEE_Mode_Control, 
    OUT MTD_U8 *XGMII_Enter_Timeout, 
    OUT MTD_U8 *GMII_Enter_Timeout, 
    OUT MTD_U8 *MII_Enter_Timeout, 
    OUT MTD_U8 *XGMII_Exit_Timeout, 
    OUT MTD_U8 *GMII_Exit_Timeout, 
    OUT MTD_U8 *MII_Exit_Timeout, 
    OUT MTD_BOOL *Force_Sys_LPI, 
    OUT MTD_BOOL *Force_Wre_LPI 
)
{
    MTD_U32 U32EEE_Mode_Control,
        U32XGMII_Enter_Timeout,
        U32GMII_Enter_Timeout,
        U32MII_Enter_Timeout,
        U32XGMII_Exit_Timeout,
        U32GMII_Exit_Timeout,
        U32MII_Exit_Timeout,
        U32Force_Sys_LPI,
        U32Force_Wre_LPI;
    MTD_U16 macSecEnable;

    /* make sure macsec was already turned on */
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,12,2,&macSecEnable));

    if (macSecEnable != 3)
    {
        return MTD_FAIL;
    }
    
    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809C,4,8,&U32XGMII_Enter_Timeout));
    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809E,0,8,&U32XGMII_Exit_Timeout));
    *XGMII_Enter_Timeout = (MTD_U8)U32XGMII_Enter_Timeout;
    *XGMII_Exit_Timeout = (MTD_U8)U32XGMII_Exit_Timeout;

    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809C,12,8,&U32GMII_Enter_Timeout));
    *GMII_Enter_Timeout = (MTD_U8)U32GMII_Enter_Timeout;

    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809E,8,8,&U32GMII_Exit_Timeout));
    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809C,20,8,&U32MII_Enter_Timeout));
    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809E,16,8,&U32MII_Exit_Timeout));
    *GMII_Exit_Timeout = (MTD_U8)U32GMII_Exit_Timeout;
    *MII_Enter_Timeout = (MTD_U8)U32MII_Enter_Timeout;
    *MII_Exit_Timeout = (MTD_U8)U32MII_Exit_Timeout;

    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809C,2,1,&U32Force_Wre_LPI));
    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809C,3,1,&U32Force_Sys_LPI));
    *Force_Sys_LPI = (MTD_BOOL)U32Force_Sys_LPI;
    *Force_Wre_LPI = (MTD_BOOL)U32Force_Wre_LPI;

    ATTEMPT(mtdHwGetMsecPhyRegField(contextPtr,port,31,0x809C,0,2,&U32EEE_Mode_Control));
    *EEE_Mode_Control = (MTD_U8)U32EEE_Mode_Control;
    if (*EEE_Mode_Control == 3)
    {
        return MTD_FAIL; /* This setting doesn't make sense */
    }
    else if (*EEE_Mode_Control == 2)
    {
        *EEE_Mode_Control = MTD_EEE_MODE_ENABLE_NO_LEGACY;
    }
    else if (*EEE_Mode_Control == 1)
    {
        *EEE_Mode_Control = MTD_EEE_MODE_ENABLE_WITH_LEGACY;
    }
    /* else it's 0, disabled, don't do anything to it, returns MTD_EEE_MODE_DISABLE */

    return MTD_OK;  
    
}

MTD_STATUS mtdEeeEnableOverrideMode 
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port  
)
{
    contextPtr = contextPtr;
    port = port;

    return MTD_FAIL; /* not implemented yet */
}

MTD_STATUS mtdEeeDisableOverrideMode 
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port  
)
{
    contextPtr = contextPtr;
    port = port;

    return MTD_FAIL; /* not implemented yet */
}


