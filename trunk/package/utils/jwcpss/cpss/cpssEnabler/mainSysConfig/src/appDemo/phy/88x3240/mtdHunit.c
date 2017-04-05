#include "mtdCopyright.h" 
/* Copyright 2014 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
higher-level functions for controlling and getting status from 
the Host Unit (H Unit) of the 88X32X0 PHY.
********************************************************************/
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdApiRegs.h"
#include "mtdCunit.h"
#include "mtdHunit.h"

/******************************************************************************
 Mac Interface functions
******************************************************************************/

MTD_STATUS mtdSetMacInterfaceControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 macType,
    IN MTD_BOOL macIfPowerDown,
    IN MTD_U16 macIfSnoopSel,
    IN MTD_U16 macIfActiveLaneSelect,
    IN MTD_U16 macLinkDownSpeed,
    IN MTD_BOOL doSwReset
)
{
    MTD_U16 cunitPortCtrl, cunitModeConfig;

    /* do range checking on parameters */
    if ((macType > MTD_MAC_LEAVE_UNCHANGED) || 
        (macType == 3))
    {
        return MTD_FAIL;
    }

    if ((macIfSnoopSel > MTD_MAC_SNOOP_LEAVE_UNCHANGED) || 
        (macIfSnoopSel == 1))
    {
        return MTD_FAIL;
    }
    
    if (macIfActiveLaneSelect > 1)
    {
        return MTD_FAIL;
    }

    if (macLinkDownSpeed > MTD_MAC_SPEED_LEAVE_UNCHANGED)
    {
        return MTD_FAIL;
    }

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_CUNIT_PORT_CTRL,&cunitPortCtrl));
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_CUNIT_MODE_CONFIG,&cunitModeConfig))

    /* Because writes of some of these bits don't show up in the register on a read */
    /* until after the software reset, we can't do repeated read-modify-writes */
    /* to the same register or we will lose those changes. */

    /* This approach also cuts down on IO and speeds up the code */

    if (macType < MTD_MAC_LEAVE_UNCHANGED)
    {
        ATTEMPT(mtdHwSetRegFieldToWord(cunitPortCtrl,macType,0,3,&cunitPortCtrl));
    }

    ATTEMPT(mtdHwSetRegFieldToWord(cunitModeConfig,(MTD_U16)macIfPowerDown,3,1,&cunitModeConfig));

    if (macIfSnoopSel < MTD_MAC_SNOOP_LEAVE_UNCHANGED)
    {
        ATTEMPT(mtdHwSetRegFieldToWord(cunitModeConfig,macIfSnoopSel,8,2,&cunitModeConfig));
    }

    ATTEMPT(mtdHwSetRegFieldToWord(cunitModeConfig,macIfActiveLaneSelect,10,1,&cunitModeConfig));

    if (macLinkDownSpeed < MTD_MAC_SPEED_LEAVE_UNCHANGED)
    {
        ATTEMPT(mtdHwSetRegFieldToWord(cunitModeConfig,macLinkDownSpeed,6,2,&cunitModeConfig));
    }

    /* Now write changed values */
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_CUNIT_PORT_CTRL,cunitPortCtrl));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_CUNIT_MODE_CONFIG,cunitModeConfig));

    if (doSwReset == MTD_TRUE)
    {
        ATTEMPT(mtdCunitSwReset(contextPtr,port));

        if (macLinkDownSpeed < MTD_MAC_SPEED_LEAVE_UNCHANGED)
        {
            ATTEMPT(mtdCunitSwReset(contextPtr,port)); /* need 2x for changes to macLinkDownSpeed */
        }
    }
    
    return MTD_OK;
}

MTD_STATUS mtdGetMacInterfaceControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_U16 *macType,
    OUT MTD_BOOL *macIfPowerDown,
    OUT MTD_U16 *macIfSnoopSel,
    OUT MTD_U16 *macIfActiveLaneSelect,
    OUT MTD_U16 *macLinkDownSpeed
)
{
    MTD_U16 temp;
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_PORT_CTRL,0,3,macType));
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,3,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *macIfPowerDown);

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,8,2,macIfSnoopSel));                        
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,10,1,macIfActiveLaneSelect)); 
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_CUNIT_MODE_CONFIG,6,2,macLinkDownSpeed))
    
    return MTD_OK;
}

/******************************************************************************
 SGMII functions
******************************************************************************/

MTD_STATUS mtdGetSGMIIAutoneg_Advertisement
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    OUT MTD_BOOL *link_status,
    OUT MTD_BOOL *full_duplex,
    OUT MTD_U16  *speed,
    OUT MTD_BOOL *tx_pause,
    OUT MTD_BOOL *rx_pause,
    OUT MTD_BOOL *fiber_media,
    OUT MTD_BOOL *eee_enabled,
    OUT MTD_BOOL *clock_stop_lpi
)
{
    MTD_U16 temp;

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,15,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *link_status);

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,12,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *full_duplex);
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,10,2,speed));
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,9,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *tx_pause);

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,8,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *rx_pause);
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,7,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *fiber_media);
    

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,6,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *eee_enabled);
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_1000X_SGMII_AN_ADV,5,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *clock_stop_lpi);
        
    return MTD_OK;
}

/******************************************************************************
 X2 (RXAUI) functions
******************************************************************************/

MTD_STATUS mtdSetX2SerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL invert_input_pol_l0,
    IN MTD_BOOL invert_output_pol_l0,
    IN MTD_BOOL invert_input_pol_l1,
    IN MTD_BOOL invert_output_pol_l1
)
{
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,15,1,MTD_GET_BOOL_AS_BIT(invert_input_pol_l1)));    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,14,1,MTD_GET_BOOL_AS_BIT(invert_input_pol_l0)));    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,13,1,MTD_GET_BOOL_AS_BIT(invert_output_pol_l1)));    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,12,1,MTD_GET_BOOL_AS_BIT(invert_output_pol_l0)));    

    return MTD_OK;
}

MTD_STATUS mtdGetX2SerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_BOOL *invert_input_pol_l0,
    IN MTD_BOOL *invert_output_pol_l0,
    IN MTD_BOOL *invert_input_pol_l1,
    IN MTD_BOOL *invert_output_pol_l1    
)
{
    MTD_U16 temp;

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,15,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *invert_input_pol_l1);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,14,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *invert_input_pol_l0);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,13,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *invert_output_pol_l1);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,MTD_H_UNIT,MTD_SERDES_CONTROL2,12,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *invert_output_pol_l0);
    
    return MTD_OK;
}



