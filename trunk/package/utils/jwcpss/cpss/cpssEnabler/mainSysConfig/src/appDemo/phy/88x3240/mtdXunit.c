#include "mtdCopyright.h" 
/* Copyright 2014 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
higher-level functions for controlling and getting status from 
the Fiber Unit (X Unit) of the 88X32X0 PHY.
********************************************************************/
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdApiRegs.h"
#include "mtdXunit.h"

/******************************************************************************
 1000BX/SGMII functions
******************************************************************************/

MTD_STATUS mtdSet1000BXAutoneg_Advertisement
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 remote_fault,
    IN MTD_U16 pause,
    IN MTD_BOOL restart_an    
)
{
    if (remote_fault > MTD_1000BX_REMOTE_FAULT_ANERR ||
        pause > MTD_SYM_ASYM_PAUSE)
    {
        return MTD_FAIL;
    }
    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_X_UNIT,MTD_1000X_SGMII_AN_ADV,12,2,remote_fault));
    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_X_UNIT,MTD_1000X_SGMII_AN_ADV,7,2,pause));

    if (restart_an == MTD_TRUE)
    {
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_X_UNIT,MTD_1000X_SGMII_CONTROL,9,1,1)); /* bit self clears */
    }
    
    return MTD_OK;
}

MTD_STATUS mtdGet1000BXAutoneg_Advertisement
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *remote_fault,
    OUT MTD_U16 *pause
)
{
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_X_UNIT,MTD_1000X_SGMII_AN_ADV,12,2,remote_fault));
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_X_UNIT,MTD_1000X_SGMII_AN_ADV,7,2,pause));

    return MTD_OK;
}

MTD_STATUS mtdGet1000BXLinkPartner_Ability
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *ack,
    OUT MTD_U16 *remote_fault,
    OUT MTD_U16 *pause,
    OUT MTD_BOOL *half_duplex,
    OUT MTD_BOOL *full_duplex
)
{
    MTD_U16 temp,temp2;

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_X_UNIT,MTD_1000X_SGMII_LP_ABL,&temp));

    ATTEMPT(mtdHwGetRegFieldFromWord(temp,14,1,&temp2));
    MTD_CONVERT_UINT_TO_BOOL(temp2,*ack);

    ATTEMPT(mtdHwGetRegFieldFromWord(temp,12,2,remote_fault));
    ATTEMPT(mtdHwGetRegFieldFromWord(temp,7,2,pause));
    
    ATTEMPT(mtdHwGetRegFieldFromWord(temp,6,1,&temp2));
    MTD_CONVERT_UINT_TO_BOOL(temp2,*half_duplex);
    
    ATTEMPT(mtdHwGetRegFieldFromWord(temp,5,1,&temp2));
    MTD_CONVERT_UINT_TO_BOOL(temp2,*full_duplex);
        
    return MTD_OK;
}



