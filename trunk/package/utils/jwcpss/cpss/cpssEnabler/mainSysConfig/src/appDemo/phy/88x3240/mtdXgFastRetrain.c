#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions prototypes and global defines/data for
higher-level functions using MDIO access to control and get
status of the 10GBASE-T fast retrain functionality for the Marvell 
88X3240 ethernet PHY.
********************************************************************/
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdXgFastRetrain.h"

/*******************************************************************
 Fast Retrain Control/Status - Top Level Functions

 Use these functions to configure/enable the NFR/10GBTFR and to 
 check the resolution to see which fast retrain type will be used
 following auto-negotiation.

 It is suggested to use only the top-level function for configuration.

 After the link is up, the function mtdGetFastRetrainResolution()
 should be called to determine the type of fast retrain which is being
 used. 

 While the link is up, mtdGetFastRetrainStatus() can be used to read
 the status (which will clear bits in 1.147) and the corresponding
 set of functions can be called to test the status (see the list of 
 functions in the description for mtdGetFastRetrainStatus()).

 If only the 10GBTFR is desired, the functions in the 10GBTFR section
 plus mtdGetFastRetrainStatus() can be used instead of the top-level
 functions.

 If only the NFR is desired, the functions in the NFR section plus
 mtdGetFastRetrainResolution() and mtdGetFastRetrainStatus() can 
 be used instead of the top-level configuration function.

 *******************************************************************/

MTD_STATUS mtdConfigureFastRetrain
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 fr_mode, 
    IN MTD_U16 frSignalType,
    IN MTD_BOOL anRestart
)
{
    MTD_STATUS stat = MTD_OK;
    
    switch (fr_mode)
    {
        case MTD_DISABLE_FR:
            stat |= mtdDoNotAdvertNFR(contextPtr, port, MTD_FALSE);
            stat |= mtdDisable10GBTFR(contextPtr, port);
            stat |= mtdDoNotAdvert10GBTFR(contextPtr, port, MTD_FALSE);
            break;

        case MTD_ENABLE_10GBTFR_ONLY:            
            stat |= mtdDoNotAdvertNFR(contextPtr, port, MTD_FALSE); 
            stat |= mtdEnable10GBTFR(contextPtr, port, frSignalType);
            stat |= mtdAdvert10GBTFR(contextPtr, port, MTD_FALSE);            
            break;

        case MTD_ENABLE_NFR_ONLY:
            stat |= mtdDisable10GBTFR(contextPtr, port); /* This order is important*/
            stat |= mtdDoNotAdvert10GBTFR(contextPtr, port, MTD_FALSE);
            stat |= mtdAdvertNFR(contextPtr, port, frSignalType, MTD_FALSE);
            break;

        case MTD_ENABLE_BOTH_FR:
            stat |= mtdAdvertNFR(contextPtr, port, frSignalType, MTD_FALSE);
            stat |= mtdEnable10GBTFR(contextPtr, port, frSignalType);
            stat |= mtdAdvert10GBTFR(contextPtr, port, MTD_FALSE);            
            break;
            
        default:
            return MTD_FAIL; /* some kind of error occurred*/
            break;
    }

    if(anRestart)
    {
        stat |= mtdAutonegEnable(contextPtr, port);
        stat |= mtdAutonegRestart(contextPtr, port);
    }   

    return stat;
}

MTD_STATUS mtdGetFastRetrainResolution
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    OUT MTD_U16 *frResolution
)
{
    return (mtdHwGetPhyRegField(contextPtr,port,7,
        32781,10,3,frResolution));
}

MTD_STATUS mtdGetFastRetrainStatus
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 *currentValue
)
{

    return(mtdHwXmdioRead(contextPtr, port,1,147,currentValue));
}

    
/*******************************************************************
 Fast Retrain Control/Status - NFR-specific Functions

 Use these functions to configure/enable and check the status of
 only the NFR (negotiated fast retrain).
 *******************************************************************/
MTD_STATUS mtdAdvertNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 frSignalType,
    IN MTD_BOOL anRestart
)
{    
    /* set the signalling type for what gets sent to the MAC on a fast retrain */
    /* this will also clear the counts since it reads 1.147 */
    if (frSignalType == MTD_FR_IDLE || frSignalType == MTD_FR_LOCAL_FAULT ||
        frSignalType == MTD_FR_LINK_INTRRUPTION)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,147,1,2,frSignalType));
    }
    else
    {
        return MTD_FAIL;
    }

    /* disable the 10GBTFR */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,147,0,1,0));
    
    /* enable NFR to be advertised */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,7,0x800C,0,1,1));

    if(anRestart)
    {
        return ((MTD_STATUS)(mtdAutonegEnable(contextPtr, port) ||
                             mtdAutonegRestart(contextPtr, port)));
    }

    return MTD_OK;

}

MTD_STATUS mtdDoNotAdvertNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL anRestart
)
{
    /* disable NFR to be advertised */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,7,0x800C,0,1,0));

    if(anRestart)
    {
        return ((MTD_STATUS)(mtdAutonegEnable(contextPtr, port) ||
                             mtdAutonegRestart(contextPtr, port)));
    }

    return MTD_OK;
}

MTD_STATUS mtdLPNFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *lpNFRAbility
)
{
    MTD_U16 val;

    *lpNFRAbility = MTD_FALSE;
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,7,0x800D, 0,1, &val));

    if (val)
    {
        *lpNFRAbility = MTD_TRUE;
    }

    return MTD_OK;
}

MTD_STATUS mtdGetLDNFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *ldCount
)
{    
    /* Bits 10:6 in reg 1.147 give the LD fast retrain count */
    return(mtdHwGetRegFieldFromWord(currentValue,6,5,ldCount));
}


/*******************************************************************
 Fast Retrain Control/Status - 10GBTFR-specific Functions

 Use these functions to configure/enable and check the status of
 only the 10GBTFR (standard-defined fast retrain).
 *******************************************************************/

MTD_STATUS mtdGetLP10GBTFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *lpCount
)
{    
    /*Bits 15:11 in reg 1.147 give the LP fast retrain count.*/
    return(mtdHwGetRegFieldFromWord(currentValue,11,5,lpCount));
}

MTD_STATUS mtdGetLD10GBTFRCount
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_U16 *ldCount
)
{
    /*Bits 10:6 in reg 1.147 give the LD fast retrain count.*/
    return(mtdHwGetRegFieldFromWord(currentValue,6,5,ldCount));
}

MTD_STATUS mtdIs10GBTFRNegotiated
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 currentValue,
    OUT MTD_BOOL *isNegotiated
)
{
    MTD_U16 result;

    /*Bit 3 in reg 1.147 give status if 10GBTFR was negotiated. If '1' it 
      was negotiated else it was not negotiated*/
    ATTEMPT(mtdHwGetRegFieldFromWord(currentValue,3,1,&result));

    result ? (*isNegotiated = MTD_TRUE) : (*isNegotiated = MTD_FALSE);

    return MTD_OK;
}

MTD_STATUS mtdEnable10GBTFR
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    IN MTD_U16 frSignalType
)
{
    /*Bit 2;1 in reg 1.147 is the fast retrain signal type and bit '0'
    is the enable bit. */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,147,1,2,frSignalType));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,1,147,0,1,1));

    return MTD_OK;
}

MTD_STATUS mtdDisable10GBTFR
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port
)
{
    return(mtdHwXmdioWrite(contextPtr, port, 1, 147, 0x0000));    
}

MTD_STATUS mtdAdvert10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL anRestart
)
{
    /* 7.32.1 = 1 */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,7,32,1,1,1));

    if(anRestart)
    {
        ATTEMPT(mtdAutonegRestart(contextPtr, port));
    }        

    return MTD_OK;
}

MTD_STATUS mtdDoNotAdvert10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 anRestart
)
{
    /* 7.32.1 = 1 */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,7,32,1,1,0));

    if(anRestart)
    {
        ATTEMPT(mtdAutonegRestart(contextPtr, port));
    }        

    return MTD_OK;
}

/* Does Link partner have Fast Retrain ability */ 
MTD_STATUS mtdLP10GBTFR
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *lp10BTFRAble
)
{
    MTD_U16 val;

    *lp10BTFRAble = MTD_FALSE;

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,7,33,1,1,&val));

    if (val)
    {
        *lp10BTFRAble = MTD_TRUE;
    }

    return MTD_OK;
}


/*******************************************************************************
Copyright (C) 2011, Marvell International Ltd. and its affiliates
If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.
*******************************************************************************/

