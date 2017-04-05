#include "mtdCopyright.h" 
/* Copyright 2014 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data defines/data for
higher-level functions that are shared by the H Unit (host/interface
to the MAC) and the X Unit (media/fiber interface).
********************************************************************/
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdApiRegs.h"
#include "mtdHXunit.h"

/******************************************************************************
 SERDES control (common)
******************************************************************************/

MTD_STATUS mtdSetSerdesControl1
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL loopback,
    IN MTD_BOOL rx_powerdown,
    IN MTD_BOOL block_tx_on_loopback
)
{
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL1,12,1,MTD_GET_BOOL_AS_BIT(loopback)));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL1,8,1,MTD_GET_BOOL_AS_BIT(rx_powerdown)));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL1,6,1,MTD_GET_BOOL_AS_BIT(block_tx_on_loopback)));
    
    return MTD_OK;
}

MTD_STATUS mtdGetSerdesControl1
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *loopback,
    OUT MTD_BOOL *rx_powerdown,
    OUT MTD_BOOL *block_tx_on_loopback
)
{
    MTD_U16 temp;    

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL1,12,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *loopback);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL1,8,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *rx_powerdown);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL1,6,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *block_tx_on_loopback);

    return MTD_OK;
}

MTD_STATUS mtdSetSerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL invert_input_pol,
    IN MTD_BOOL invert_output_pol
)
{
    MTD_U8 input_pol_bit, output_pol_bit;

    if (HorXunit == MTD_H_UNIT)
    {
        input_pol_bit = 14;
        output_pol_bit = 12;
    }
    else
    {
        input_pol_bit = 15;
        output_pol_bit = 13;
    }

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL2,input_pol_bit,1,MTD_GET_BOOL_AS_BIT(invert_input_pol)));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL2,output_pol_bit,1,MTD_GET_BOOL_AS_BIT(invert_output_pol)));        

    return MTD_OK;
}

MTD_STATUS mtdGetSerdesLanePolarity
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL *invert_input_pol,
    IN MTD_BOOL *invert_output_pol
)
{
    MTD_U8 input_pol_bit, output_pol_bit;
    MTD_U16 temp;

    if (HorXunit == MTD_H_UNIT)
    {
        input_pol_bit = 14;
        output_pol_bit = 12;
    }
    else
    {
        input_pol_bit = 15;
        output_pol_bit = 13;
    }

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL2,input_pol_bit,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *invert_input_pol);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_SERDES_CONTROL2,output_pol_bit,1,&temp));        
    MTD_CONVERT_UINT_TO_BOOL(temp, *invert_output_pol);
    
    return MTD_OK;
}

MTD_STATUS mtdSetSerdesInterruptEnable
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL fifo_overflow,
    IN MTD_BOOL fifo_underflow,
    IN MTD_BOOL pkt_check_crc
)
{
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CRC_INTR_ENABLE,1,1,MTD_GET_BOOL_AS_BIT(fifo_overflow)));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CRC_INTR_ENABLE,0,1,MTD_GET_BOOL_AS_BIT(fifo_underflow)));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CRC_INTR_ENABLE,2,1,MTD_GET_BOOL_AS_BIT(pkt_check_crc)));
    
    return MTD_OK;
}

MTD_STATUS mtdGetSerdesInterruptEnable
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *fifo_overflow,
    OUT MTD_BOOL *fifo_underflow,
    OUT MTD_BOOL *pkt_check_crc
)
{
    MTD_U16 temp;

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CRC_INTR_ENABLE,1,1,&temp));    
    MTD_CONVERT_UINT_TO_BOOL(temp, *fifo_overflow);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CRC_INTR_ENABLE,0,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *fifo_underflow);
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CRC_INTR_ENABLE,2,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp, *pkt_check_crc);
    
    return MTD_OK;
}

MTD_STATUS mtdGetSerdesInterruptStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *fifo_overflow,
    OUT MTD_BOOL *fifo_underflow,
    OUT MTD_BOOL *pkt_check_crc
)
{
    MTD_U16 temp,temp2;

    /* clears latch bits, so need to read register in one operation */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_FIFO_CRC_INTR_STATUS,&temp));

    mtdHwGetRegFieldFromWord(temp,1,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2, *fifo_overflow);

    mtdHwGetRegFieldFromWord(temp,0,1,&temp2);    
    MTD_CONVERT_UINT_TO_BOOL(temp2, *fifo_underflow);

    mtdHwGetRegFieldFromWord(temp,2,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2, *pkt_check_crc);
        
    return MTD_OK;
}

MTD_STATUS mtdSetGetSerdesPPMFifo
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,    
    INOUT MTD_U16 *fifo_offset
)
{
    if (*fifo_offset > MTD_SERDES_PPM_FIFO_GETONLY)
    {
        return MTD_FAIL; /* out of range */
    }

    if (*fifo_offset < MTD_SERDES_PPM_FIFO_GETONLY)
    {
        /* update it in the register, leave input parameter the same */
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CONTROL1,14,2,*fifo_offset));            
        
    }
    else
    {
        /* get only, read don't change the register setting */
        ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_FIFO_CONTROL1,14,2,fifo_offset));            
    }
    
    return MTD_OK;
}

MTD_STATUS mtdConfigurePktGeneratorChecker
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL readToClear,
    IN MTD_BOOL dontuseSFDinChecker,
    IN MTD_U16 pktPatternControl,
    IN MTD_BOOL generateCRCoff,
    IN MTD_U32 initialPayload,
    IN MTD_U16 frameLengthControl,
    IN MTD_U16 numPktsToSend,
    IN MTD_BOOL randomIPG,
    IN MTD_U16 ipgDuration
)
{
    if (pktPatternControl > MTD_PKT_RANDOM_WORD || pktPatternControl == 1)
    {
        return MTD_FAIL;
    }

    if (frameLengthControl == 6 || frameLengthControl == 7)
    {
        return MTD_FAIL;
    }
    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_CONTROL1,15,1,MTD_GET_BOOL_AS_BIT(readToClear)));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_CONTROL1,2,1,MTD_GET_BOOL_AS_BIT(dontuseSFDinChecker)));

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_CONTROL2,4,4,pktPatternControl));
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_CONTROL2,3,1,MTD_GET_BOOL_AS_BIT(generateCRCoff)));

    /* load up initial payload */
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,HorXunit,MTD_PKT_GEN_PAYLOAD1,(MTD_U16)initialPayload));
    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,HorXunit,MTD_PKT_GEN_PAYLOAD2,(MTD_U16)(initialPayload>>16)));

    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,HorXunit,MTD_PKT_GEN_LENGTH,frameLengthControl));

    ATTEMPT(mtdHwXmdioWrite(contextPtr,port,HorXunit,MTD_PKT_GEN_BURSTSEQ,numPktsToSend));

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_IPG,15,1,MTD_GET_BOOL_AS_BIT(randomIPG)));
    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_IPG,0,15,ipgDuration));
    
    return MTD_OK;
}

MTD_STATUS mtdEnablePktGeneratorChecker
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL enableGenerator,
    IN MTD_BOOL enableChecker
)
{
    MTD_U16 temp=0;
    
    /* better to start/stop at the same time if possible, use a single write to limit delays between them */
    temp = ((MTD_GET_BOOL_AS_BIT(enableGenerator))<<1) | (MTD_GET_BOOL_AS_BIT(enableChecker));    

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_CONTROL1,0,2,temp));

    return MTD_OK;    
}

MTD_STATUS mtdPktGeneratorCounterReset
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit    
)
{
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_CONTROL1,6,1,1));   
    /* assumes delay is long enough to clear counters, this makes an assumption about the speed */
    /* of the MDIO as being relatively slow compared to the number of cycles it takes to clear the */
    /* clear the counters, relatively fast. may need a delay here for F2R, or really fast MDIO */
    /* speeds */
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_PKT_GEN_CONTROL1,6,1,0));    

    return MTD_OK;    
}

MTD_STATUS mtdPktGeneratorGetCounter
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_U16 whichCounter,
    OUT MTD_U64 *packetCount,
    OUT MTD_U64 *byteCount    
)
{
    MTD_U16 temp;
    
    *packetCount = *byteCount = 0;
    
    switch (whichCounter)
    {
        case MTD_PKT_GET_TX:
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_TXPKTCTR1,&temp));
            *packetCount = temp;
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_TXPKTCTR2,&temp));
            *packetCount |= (((MTD_U64)(temp))<<16);
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_TXPKTCTR3,&temp));
            *packetCount |= (((MTD_U64)(temp))<<32);

            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_TXBYTCTR1,&temp));
            *byteCount = temp;
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_TXBYTCTR2,&temp));
            *byteCount |= (((MTD_U64)(temp))<<16);
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_TXBYTCTR3,&temp));
            *byteCount |= (((MTD_U64)(temp))<<32);            
            break;

        case MTD_PKT_GET_RX:
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_RXPKTCTR1,&temp));
            *packetCount = temp;
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_RXPKTCTR2,&temp));
            *packetCount |= (((MTD_U64)(temp))<<16);
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_RXPKTCTR3,&temp));
            *packetCount |= (((MTD_U64)(temp))<<32);

            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_RXBYTCTR1,&temp));
            *byteCount = temp;
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_RXBYTCTR2,&temp));
            *byteCount |= (((MTD_U64)(temp))<<16);
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_RXBYTCTR3,&temp));
            *byteCount |= (((MTD_U64)(temp))<<32);            
            break;

        case MTD_PKT_GET_ERR:
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_ERRPKTCTR1,&temp));
            *packetCount = temp;
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_ERRPKTCTR2,&temp));
            *packetCount |= (((MTD_U64)(temp))<<16);
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_PKT_GEN_ERRPKTCTR3,&temp));
            *packetCount |= (((MTD_U64)(temp))<<32);            
            break;

        default:
            return MTD_FAIL;
            break;            
    }
    
    return MTD_OK;
}

MTD_STATUS mtdPktGeneratorCheckerGetLinkDrop
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_U16 *linkDropCounter        
)
{
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_LINK_DROP_COUNTER,linkDropCounter));
    
    return MTD_OK;
}

/******************************************************************************
 Functions shared between H unit and X unit for 1000BX/SGMII
******************************************************************************/

MTD_STATUS mtdSet1000BXSGMIIControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL loopback,
    IN MTD_U16 speed,
    IN MTD_BOOL an_enable,
    IN MTD_BOOL power_down,
    IN MTD_BOOL restart_an,
    IN MTD_BOOL sw_reset
)
{       
    MTD_U16 bit6, bit13;
    
    if (speed > MTD_SGMII_SPEED_ASIS)
    {
        return MTD_FAIL;
    }

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,14,1,MTD_GET_BOOL_AS_BIT(loopback)));   

    if (speed != MTD_SGMII_SPEED_ASIS)
    {
        switch (speed)
        {
            case MTD_SGMII_SPEED_10M:
                bit6 = bit13 = 0;
                break;

            case MTD_SGMII_SPEED_100M:
                bit6 = 0;
                bit13 = 1;                
                break;

            case MTD_SGMII_SPEED_1G:
                bit6 = 1;
                bit13 = 0;
                break;

            default:
                return MTD_FAIL;
                break;
        }

        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,6,1,bit6));   
        ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,13,1,bit13));   
        
    }

    
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,12,1,MTD_GET_BOOL_AS_BIT(an_enable)));   
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,11,1,MTD_GET_BOOL_AS_BIT(power_down)));   
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,9,1,MTD_GET_BOOL_AS_BIT(restart_an)));   
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,15,1,MTD_GET_BOOL_AS_BIT(sw_reset)));   
    
    return MTD_OK;    
}

MTD_STATUS mtdGet1000BXSGMIIControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *loopback,
    OUT MTD_U16 *speed,
    OUT MTD_BOOL *an_enable,
    OUT MTD_BOOL *power_down
)
{
    MTD_U16 temp,bit6,bit13; 
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,14,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp,*loopback);
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,6,1,&bit6));   
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,13,1,&bit13));
    *speed = (bit6<<1) | bit13;

    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,12,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp,*an_enable);
    
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_CONTROL,11,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp,*power_down);
    
    return MTD_OK;
}

MTD_STATUS mtdGet1000BXSGMIIStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *an_complete,
    OUT MTD_BOOL *remote_fault,
    OUT MTD_BOOL *link_status_latched,
    OUT MTD_BOOL *link_status_current
)
{
    MTD_U16 temp,temp2;

    /* read once to get all bits including latched low link status (may clear LL link status) */
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_1000X_SGMII_STATUS,&temp));

    mtdHwGetRegFieldFromWord(temp,5,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2,*an_complete);

    mtdHwGetRegFieldFromWord(temp,4,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2,*remote_fault);

    mtdHwGetRegFieldFromWord(temp,2,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2,*link_status_latched);

    /* read it again to get current link status */
    ATTEMPT(mtdHwGetPhyRegField(contextPtr,port,HorXunit,MTD_1000X_SGMII_STATUS,2,1,&temp));
    MTD_CONVERT_UINT_TO_BOOL(temp,*link_status_current);
    
    return MTD_OK;
}

/******************************************************************************
 Functions shared between H unit and X unit for 10GBASE-R PCS
******************************************************************************/

MTD_STATUS mtdSet10GBRControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    IN MTD_BOOL loopback,
    IN MTD_BOOL power_down,
    IN MTD_BOOL sw_reset
)
{
    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_10GBR_PCS_CONTROL,14,1,MTD_GET_BOOL_AS_BIT(loopback)));   

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_10GBR_PCS_CONTROL,11,1,MTD_GET_BOOL_AS_BIT(power_down)));   

    ATTEMPT(mtdHwSetPhyRegField(contextPtr,port,HorXunit,MTD_10GBR_PCS_CONTROL,15,1,MTD_GET_BOOL_AS_BIT(sw_reset)));   
    
    return MTD_OK;
}

MTD_STATUS mtdGet10GBRControl
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *loopback,
    OUT MTD_BOOL *power_down
)
{
    MTD_U16 lb,pd,temp;
    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_10GBR_PCS_CONTROL,&temp));
    mtdHwGetRegFieldFromWord(temp,14,1,&lb);
    mtdHwGetRegFieldFromWord(temp,11,1,&pd);
    MTD_CONVERT_UINT_TO_BOOL(lb, *loopback);
    MTD_CONVERT_UINT_TO_BOOL(pd, *power_down);
        
    return MTD_OK;
}

MTD_STATUS mtdGet10GBRStatus1
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,
    OUT MTD_BOOL *tx_lpi_latch,
    OUT MTD_BOOL *rx_lpi_latch,
    OUT MTD_BOOL *tx_lpi_current,
    OUT MTD_BOOL *rx_lpi_current,
    OUT MTD_BOOL *fault,
    OUT MTD_BOOL *link_status_latch,
    OUT MTD_BOOL *link_status_current    
)
{
    MTD_U16 temp1,temp2,temp3;

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_10GBR_PCS_STATUS1,&temp1)); /* latched value */

    mtdHwGetRegFieldFromWord(temp1,11,1,&temp3);
    MTD_CONVERT_UINT_TO_BOOL(temp3, *tx_lpi_latch);
    mtdHwGetRegFieldFromWord(temp1,10,1,&temp3);
    MTD_CONVERT_UINT_TO_BOOL(temp3, *rx_lpi_latch);
    mtdHwGetRegFieldFromWord(temp1,2,1,&temp3);
    MTD_CONVERT_UINT_TO_BOOL(temp3, *link_status_latch);
            
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_10GBR_PCS_STATUS1,&temp2)); /* current value */

    mtdHwGetRegFieldFromWord(temp2,9,1,&temp3);
    MTD_CONVERT_UINT_TO_BOOL(temp3, *tx_lpi_current);
    mtdHwGetRegFieldFromWord(temp2,8,1,&temp3);
    MTD_CONVERT_UINT_TO_BOOL(temp3, *rx_lpi_current);
    mtdHwGetRegFieldFromWord(temp2,7,1,&temp3);
    MTD_CONVERT_UINT_TO_BOOL(temp3, *fault);
    mtdHwGetRegFieldFromWord(temp2,2,1,&temp3);
    MTD_CONVERT_UINT_TO_BOOL(temp3, *link_status_current);
        
    return MTD_OK;
}


MTD_STATUS mtdGet10GBRFault
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit,    
    OUT MTD_BOOL *tx_fault_latch,
    OUT MTD_BOOL *rx_fault_latch,
    OUT MTD_BOOL *tx_fault_current,
    OUT MTD_BOOL *rx_fault_current
)
{
    MTD_U16 temp1, fault;
    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_10GBR_PCS_FAULT,&temp1)); /* latched value */
    mtdHwGetRegFieldFromWord(temp1,11,1,&fault);
    MTD_CONVERT_UINT_TO_BOOL(fault, *tx_fault_latch);
    mtdHwGetRegFieldFromWord(temp1,10,1,&fault);
    MTD_CONVERT_UINT_TO_BOOL(fault, *rx_fault_latch);

    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_10GBR_PCS_FAULT,&temp1)); /* current value */
    mtdHwGetRegFieldFromWord(temp1,11,1,&fault);
    MTD_CONVERT_UINT_TO_BOOL(fault, *tx_fault_current);
    mtdHwGetRegFieldFromWord(temp1,10,1,&fault);
    MTD_CONVERT_UINT_TO_BOOL(fault, *rx_fault_current);

    return MTD_OK;
}

MTD_STATUS mtdGet10GBRReceiveStatus
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit, 
    OUT MTD_BOOL *link_up,
    OUT MTD_BOOL *high_ber,
    OUT MTD_BOOL *block_lock    
)
{
    MTD_U16 temp1, temp2;
    
    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_10GBR_PCS_RXSTATUS,&temp1)); /* current value */
    mtdHwGetRegFieldFromWord(temp1,12,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2, *link_up);
    mtdHwGetRegFieldFromWord(temp1,1,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2, *high_ber);
    mtdHwGetRegFieldFromWord(temp1,0,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2, *block_lock);
    
    
    return MTD_OK;
}


MTD_STATUS mtdGet10GBRStatus2
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 HorXunit, 
    OUT MTD_BOOL *has_block_lock,
    OUT MTD_BOOL *reported_high_ber,
    OUT MTD_U8 *ber_counter,
    OUT MTD_U8 *errored_blocks_counter    
)
{
    MTD_U16 temp1, temp2;

    ATTEMPT(mtdHwXmdioRead(contextPtr,port,HorXunit,MTD_10GBR_PCS_STATUS2,&temp1)); /* latch value, clears latch/counters */
    mtdHwGetRegFieldFromWord(temp1,15,1,&temp2);

    MTD_CONVERT_UINT_TO_BOOL(temp2, *has_block_lock);

    mtdHwGetRegFieldFromWord(temp1,14,1,&temp2);
    MTD_CONVERT_UINT_TO_BOOL(temp2, *reported_high_ber);

    mtdHwGetRegFieldFromWord(temp1,8,6,&temp2);
    *ber_counter = (MTD_U8)temp2;
    
    mtdHwGetRegFieldFromWord(temp1,0,8,&temp2);
    *errored_blocks_counter = (MTD_U8)temp2;
    
    return MTD_OK;
}

