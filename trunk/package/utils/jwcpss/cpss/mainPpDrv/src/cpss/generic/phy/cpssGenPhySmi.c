/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
*
* cpssGenPhySmi.c
*
* DESCRIPTION:
*       API implementation for port Core Serial Management Interface facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/phy/private/prvCpssDxChPhyLog.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/phy/private/prvCpssGenPhySmi.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>

/*************************** Private definitions ******************************/

/*************************** Private functions ********************************/

static GT_U32 phy154xMInitSequence[42][2] =
{
    {22, 0x00FA},
    {8 , 0x0010},
    {22, 0x00FB},
    {1 , 0x4099},
    {3 , 0x1120},
    {11, 0x113C},
    {14, 0x8100},
    {15, 0x112A},
    {22, 0x00FC},
    {1 , 0x20B0},
    {22, 0x00FF},
    {17, 0x0000},
    {16, 0x2000},
    {17, 0x4444},
    {16, 0x2140},
    {17, 0x8064},
    {16, 0x2141},
    {17, 0x0108},
    {16, 0x2144},
    {17, 0x0F16},
    {16, 0x2146},
    {17, 0x8C44},
    {16, 0x214B},
    {17, 0x0F90},
    {16, 0x214C},
    {17, 0xBA33},
    {16, 0x214D},
    {17, 0x39AA},
    {16, 0x214F},
    {17, 0x8433},
    {16, 0x2151},
    {17, 0x2010},
    {16, 0x2152},
    {17, 0x99EB},
    {16, 0x2153},
    {17, 0x2F3B},
    {16, 0x2154},
    {17, 0x584E},
    {16, 0x2156},
    {17, 0x1223},
    {16, 0x2158},
    {22, 0x0000}
};

/*******************************************************************************
* phy1340Init
*
* DESCRIPTION:
*   This function implements the WA for the 88E1340.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiWriteRegister    - pointer SMI Write implementation
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phy1340Init
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS     rc;         /* Return code                  */

    /* WA for the short cable problem */
    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x00FF);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG24, 0x2800);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG23, 0x2001);
    if ( rc != GT_OK )
    {
        return rc;
    }

    /* WA for the 88E1340 ESD problem */
    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0000);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0010);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG30, 0x4018);
    if ( rc != GT_OK )
    {
        return rc;
    }

    rc =  smiWriteRegister(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0000);
    
    return rc;

}

/*******************************************************************************
* phy154xMInit
*
* DESCRIPTION:
*   This function implements initialization required for the 154<n>M PHYs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiReadRegister     - pointer SMI Read implementation
*       smiWriteRegister    - pointer SMI Write implementation
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phy154xMInit
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS   rc;             /* return code                         */
    GT_U16      value;          /* register data                       */
    GT_BOOL     autoNegStatus;  /* PP to PHY Auto - Negotiation status */
    GT_U32      ii;             /* loop iterator                       */


    /* check AutoNeg function was bound */
    autoNegStatus = (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->
                        genVctBindFunc.cpssPhyAutoNegotiationGet == NULL)
                        ? GT_FALSE : GT_TRUE;

    /* get AutoNeg status */
    if(autoNegStatus)
    {
        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.
            cpssPhyAutoNegotiationGet(devNum, portNum, &autoNegStatus);
    }

    /* in case AutoNeg enable, disable it*/
    if(autoNegStatus)
    {
        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.
            cpssPhyAutoNegotiationSet(devNum, portNum, GT_FALSE);
    }

    /* initialization sequence is relevant for silicon revision A0, */
    /* Reg 31_251.15:0 = 0x0200 */

    rc = smiWriteRegister(devNum, portNum, 22, 0xFB);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = smiReadRegister(devNum, portNum, 31, &value);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = smiWriteRegister(devNum, portNum, 22, 0x0);
    if( GT_OK != rc )
    {
        return rc;
    }

    if( 0x0200 == value)
    {
        for(ii = 0 ; 
            ii < sizeof(phy154xMInitSequence)/sizeof(phy154xMInitSequence[0]) ; 
            ii++ )
        {
            rc = smiWriteRegister(devNum, 
                                  portNum,
                                  (GT_U8)phy154xMInitSequence[ii][0],
                                  (GT_U16)phy154xMInitSequence[ii][1]);
             if( GT_OK != rc )
             {
                 return rc;
             }
        }
    }

    if(autoNegStatus)
    {
        /* Restore AutoNeg status */
        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.
            cpssPhyAutoNegotiationSet(devNum, portNum, GT_TRUE);
    }

    return rc;
}

/*******************************************************************************
* phy168xInit
*
* DESCRIPTION:
*   This function implements initialization required for the 168x PHYs.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiReadRegister     - pointer SMI Read implementation
*       smiWriteRegister    - pointer SMI Write implementation
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phy168xInit
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS   rc;             /* return code                         */
    GT_BOOL     autoNegStatus;  /* PP to PHY Auto - Negotiation status */

    if(smiReadRegister != NULL)
    {/* fix warning */
        smiReadRegister = smiReadRegister;
    }

    /* check AutoNeg function was bound */
    autoNegStatus = (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->
                        genVctBindFunc.cpssPhyAutoNegotiationGet == NULL)
                        ? GT_FALSE : GT_TRUE;

    /* get AutoNeg status */
    if(autoNegStatus)
    {
        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.
            cpssPhyAutoNegotiationGet(devNum, portNum, &autoNegStatus);
    }

    /* in case AutoNeg enable, disable it*/
    if(autoNegStatus)
    {
        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.
            cpssPhyAutoNegotiationSet(devNum, portNum, GT_FALSE);
    }

    /* initialization sequence is relevant for silicon revision A0 in A1 it should be fixed. */
    /* The following register writes must be done once after hard-reset to all ports:
           Write Reg 22 = 0x00FD   // Write Reg 22 to Page 253
           Write Reg 8 = 0x0B53    // Default 0x0B4F
           Write Reg 7 = 0x200D
           Write Reg 22 = 0x0000  // Write Reg 22 to Page 0
       */

    rc = smiWriteRegister(devNum, portNum, 22, 0xFD);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = smiWriteRegister(devNum, portNum, 8, 0xB53);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = smiWriteRegister(devNum, portNum, 7, 0x200D);
    if( GT_OK != rc )
    {
        return rc;
    }
	
    rc = smiWriteRegister(devNum, portNum, 22, 0x0);
    if( GT_OK != rc )
    {
        return rc;
    }

    if(autoNegStatus)
    {
        /* Restore AutoNeg status */
        PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.
            cpssPhyAutoNegotiationSet(devNum, portNum, GT_TRUE);
    }

    return rc;
}

/*******************************************************************************
* phyLockUpprevent
*
* DESCRIPTION:
*   This function implements the WA for the 88E1112 SERDES lock-up erratum
*   (during the initialization section).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiReadRegister     - Read function
*       smiWriteRegister    - Write function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phyLockUpprevent
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_STATUS     rc;         /* Return code                  */
    GT_U16        tempVal;    /* Value to set to the register */

    rc =  smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x0008);

    rc |= smiReadRegister(devNum,portNum,PRV_CPSS_PHY_REG30,&tempVal);
    tempVal = (GT_U16)(tempVal & 0xFFFC);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,tempVal);

    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x000C);

    rc |= smiReadRegister(devNum,portNum,PRV_CPSS_PHY_REG30,&tempVal);
    tempVal = (GT_U16)(tempVal & 0xFFFC);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,tempVal);

    if ( rc != GT_OK )
    {
        return (rc);
    }

    rc = cpssOsTimerWkAfter(1);
    if ( rc != GT_OK )
    {
        return (rc);
    }

    /* Reset SGMII MAC interface SERDES receive block. */
    rc =  smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x0017);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x4008);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x0000);

    if ( rc != GT_OK )
    {
        return (rc);
    }

    /* Reset Fiber media interface SERDES receive block. */
    rc =  smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG29,0x0018);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x00CA);
    rc |= smiWriteRegister(devNum,portNum,PRV_CPSS_PHY_REG30,0x0000);

    if ( rc != GT_OK )
    {
        return (rc);
    }

    rc = cpssOsTimerWkAfter(2);
    if ( rc != GT_OK )
    {
        return (rc);
    }

    return (GT_OK);
}

/*******************************************************************************
* phy1149GigabitVodSet
*
* DESCRIPTION:
*   This function implements the WA for the 88E1149 revision C1 gigabit VOD
*   erratum (during the initialization section).
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       vodRegValue - value to be set in Vod register
*       smiWriteRegister - pointer SMI Write implementation
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS phy1149GigabitVodSet
(
    IN GT_U8  devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U16 vodRegValue,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    /* set an access to AFE block*/
    smiWriteRegister (devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x00FF);

    /* For Prestera PP + E1149 rev C1 based systems PHY developers
       advise adjusting to 0x7777 for normal operation (lowest Gig VOD)
      If using the IEEE test mode 1, PHY developers advise writing 0xCCCC
      to pass IEEE gig VOD. */
    smiWriteRegister (devNum, portNum, PRV_CPSS_PHY_REG24, vodRegValue);

    /* indirect write: vodRegValue to register 12 */
    smiWriteRegister (devNum, portNum, PRV_CPSS_PHY_REG23, 0x2012);


    return GT_OK ;
}

/*******************************************************************************
* cpssGenPhyTypeGet
*
* DESCRIPTION:
*   Gets the PHY type.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - Device Number.
*       portNum         - Port Number.
*       smiReadRegister - Read function
*
* OUTPUTS:
*       isMarvellPhyPtr - GT_TRUE - phy type is Marvell
*                         GT_FALSE - phy type is not Marvell
*       phyTypePtr      - PHY type
*
* RETURNS :
*       GT_OK   - on success or current phy is not 88E1149.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS phyTypeGet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL     *isMarvellPhyPtr,
    OUT GT_U16      *phyTypePtr,
    IN  CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister
)
{
    GT_STATUS status;

    *isMarvellPhyPtr = GT_FALSE;

    /* read Marvel OUI */
    status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_ID0, phyTypePtr);
    if (status != GT_OK)
    {
        /* HW problem or PHY not connected to a port */
        return status;
    }

    if (*phyTypePtr == PRV_CPSS_MARVELL_OUI_MSB)
    {
        *isMarvellPhyPtr = GT_TRUE;
    }
    else
    {
        /* it's not a Marvell PHY */
        return GT_OK;
    }

    /* read model number and revision */
    status = smiReadRegister(devNum, portNum, PRV_CPSS_PHY_ID1, phyTypePtr);

    return status;
}

/*******************************************************************************
* prvCpssGenPhyErrataInitFix
*
* DESCRIPTION:
*   This function implements the WA for phys during the initialization.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - Device Number.
*       portNum             - Port Number.
*       smiReadRegister     - Read function
*       smiWriteRegister    - Write function
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssGenPhyErrataInitFix
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_READ_FUNC  smiReadRegister,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{
    GT_U16 phyType;           /* PHY type */
    GT_BOOL isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
    GT_STATUS status;         /* returned status */

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    PRV_CPSS_PHY_PORT_CHECK_MAC(devNum,portNum);

    /* No errata to fix on XG port */
    if (PRV_CPSS_PP_MAC(devNum)->phyPortInfoArray[portNum].portType >= PRV_CPSS_PORT_XG_E)
    {
        return GT_OK;
    }

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY
       not connected to a port */
    phyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, smiReadRegister);

    /* Fix errata for Marvell PHYs */
    if (isMarvellPhy)
    {
        switch(phyType & PRV_CPSS_PHY_MODEL_MASK)
        {
            case PRV_CPSS_DEV_E1112: /* for PHY 1112 fix the SERDES lock-up erratum */
                                     if ((status = phyLockUpprevent(devNum,
                                                                    portNum,
                                                                    smiReadRegister,
                                                                    smiWriteRegister)) != GT_OK)
                                     {
                                         return status;
                                     }
                                     break;

            case PRV_CPSS_DEV_E1149: if ((phyType & PRV_CPSS_PHY_REV_MASK) >= 4)
                                     {
                                        /* for PHY 1149 Rev. C1 fix the gigabit VOD erratum */
                                        if ((status = phy1149GigabitVodSet(devNum,
                                                                           portNum,
                                                                           0x7777,
                                                                           smiWriteRegister)) != GT_OK)
                                        {
                                            return status;
                                        }

                                        /* set enumerator(in bitmap) for phy 1149 */
                                        PRV_CPSS_PHY_ERRATA_SET(devNum,
                                                                portNum,
                                                                PRV_CPSS_PHY_ERRATA_1149_REG_9_WRITE_WA_E);

                                     }
                                     break;

            case PRV_CPSS_DEV_E1340_X0:
            case PRV_CPSS_DEV_E1340: /* WA for the 88E1340 */
                                     if ((status = phy1340Init(devNum,
                                                               portNum,
                                                               smiWriteRegister)) != GT_OK)
                                     {
                                         return status;
                                     }
                                     break;

            case PRV_CPSS_DEV_E1540: /* Initialization for the 88E154<n>M */
                                     if((phyType & 0xF) == 0)/* revision check - only A0 should be applied */
                                     {
                                         if ((status = phy154xMInit(devNum,
                                                                    portNum,
                                                                    smiReadRegister,
                                                                    smiWriteRegister)) != GT_OK)
                                         {
                                             return status;
                                         }
                                     }
                                     break;


            case PRV_CPSS_DEV_E1680: /* Initialization for the 88E168x */
            case PRV_CPSS_DEV_E1680M:
                                     if ((status = phy168xInit(devNum,
                                                               portNum,
                                                               smiReadRegister,
                                                               smiWriteRegister)) != GT_OK)
                                     {
                                         return status;
                                     }
                                     break;

            default: break;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssPhyRegWriteErrataFix
*
* DESCRIPTION:
*   Implement PHY register write workarounds.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*       regPhy    - phy's register address
*       data      - value to be set
*       smiWriteRegister - pointer to relevant SMI write implementation
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssPhyRegWriteErrataFix
(
    IN GT_U8  devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN GT_U8  regPhy,
    IN GT_U16 data,
    IN CPSS_GEN_PHY_PORT_SMI_REGISTER_WRITE_FUNC smiWriteRegister
)
{

    GT_U32    testMode; /* test mode: 0 - Normal Mode
                                      1 - Transmit waveform test
                                      2 - Transmit Jitter Test, master mode
                                      3 - Transmit Jitter Test, slave mode
                                      4 - Transmit Distortion test */
    GT_STATUS rc;       /* return code */
    GT_U16    tmpData;  /* temporary data*/

    if((PRV_CPSS_PHY_ERRATA_GET(devNum,
                               portNum,
                          PRV_CPSS_PHY_ERRATA_1149_REG_9_WRITE_WA_E) == GT_TRUE)
    {
        /* WA is relevant for register 9 only */
        if(regPhy == 9)
        {

            /* check if Test mode - bits 13:15 being set*/
            testMode = (data >> 13);

            switch(testMode)
            {
                case 0:
                    /* Set Vod to 14% */
                    tmpData = 0x7777;
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                    /* Set Vod to 0% */
                    tmpData = 0x0;
                    break;
                default:
                    /* Set Vod to 14% as for normal mode */
                    tmpData = 0x7777;
                    break;
            }/* of switch */

            if ((rc = phy1149GigabitVodSet(devNum,
                                               portNum,
                                               tmpData,
                                               smiWriteRegister)) != GT_OK)
            {
                return rc;
            }
        }/* register 9*/
    }

    return GT_OK ;
}

/*******************************************************************************
* prvCpssFixPhy1240SgmiiLockUpErrata
*
* DESCRIPTION:
*   This function implements the WA for the 88E1240 SGMII lock-up issue per 
*   port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on access to phy other then 1240.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssFixPhy1240SgmiiLockUpErrata
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
)
{
    GT_STATUS rc;               /* returned status */
    GT_U16    regData;          /* write to register */
    GT_U16    phyType;          /* PHY type */
    GT_BOOL   isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
    GT_U32    i;                /* loop iterator */

    /* get PHY type. Don't check return status. It may be not GT_OK if PHY */
    /* not connected to a port */
    phyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, 
               (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.cpssPhyRegisterRead));

    if( (isMarvellPhy == GT_FALSE)  || 
        ((phyType & PRV_CPSS_PHY_MODEL_MASK) != PRV_CPSS_DEV_E1240) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    /* 5 iteration of WA algorithm */
    for (i = 0; i < 5; i++)
    {
        /* set Page2 */
        rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x2);
        if(rc != GT_OK)
            return rc;

        /* read register 17 on page 2 */
        rc = PRV_PHY_REG_READ_MAC(devNum, portNum, PRV_CPSS_PHY_SPEC_STATUS, &regData);
        if(rc != GT_OK)
            return rc;

        /* check 17.10 bit - SGMII link status and 17.6 bit - SGMII sync status*/
        if ((regData & BIT_10) && (regData & BIT_6))
        {
            /* no need to perform WA */
            /* restore future PHY access to page 0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0);
            if(rc != GT_OK)
                return rc;
            break;
        }
        else
        {   
            /* Perform WA */

            /* Power down the PHY */
            /* set Page0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_CNTRL, 0x1940);
            if(rc != GT_OK)
                return rc;
    
            /* Reset PCS digital logic */
            /* Write Register 29 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x001D);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0xEF00);
            if(rc != GT_OK)
                return rc;
    
            /* Reset TBG */
            /* Write Register 29 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0017);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x8300);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x8100);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x0000);
            if(rc != GT_OK)
                return rc;
    
            rc = cpssOsTimerWkAfter(5);
            if ( rc != GT_OK )
            {
                return (rc);
            }
            
            /* De-assert PCS reset */
            /* Write Register 29 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x001D);
            if(rc != GT_OK)
                return rc;
    
            /* Write Register 30 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x0000);
            if(rc != GT_OK)
                return rc;
    
            /* Release PHY port from Power down mode */
            /* Write register 0 */
            rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_CNTRL, 0x9140);
            if(rc != GT_OK)
                return rc;
    
            rc = cpssOsTimerWkAfter(100);
            if ( rc != GT_OK )
            {
                return (rc);
            }
        }
    }
    
    return GT_OK;
}

/*******************************************************************************
* prvCpssFixPhy1340BgaInitErrata
*
* DESCRIPTION:
*   This function implements the WA for the 88E1340 BGA package init sequence
*   per port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device Number.
*       portNum   - Port Number.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK   - on success.
*       GT_FAIL - otherwise.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on access to phy other then 1340.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssFixPhy1340BgaInitErrata
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum
)
{
    GT_STATUS rc;               /* returned status */
    GT_U16    phyType;          /* PHY type */
    GT_BOOL   isMarvellPhy;     /* indicator whether the PHY is a Marvell PHY */
    
    /* get PHY type. Don't check return status. It may be not GT_OK if PHY */
    /* not connected to a port */
    phyTypeGet(devNum, portNum, &isMarvellPhy, &phyType, 
               (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, portNum)->genVctBindFunc.cpssPhyRegisterRead));

    if( (isMarvellPhy == GT_FALSE)  || 
        (((phyType & PRV_CPSS_PHY_MODEL_MASK) != PRV_CPSS_DEV_E1340_X0) &&
         ((phyType & PRV_CPSS_PHY_MODEL_MASK) != PRV_CPSS_DEV_E1340)) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }
    
    /* set Page0 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_GE_PHY_EXT_ADDR, 0x0);
    if(rc != GT_OK)
        return rc;

    /* Write Register 29 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0003);
    if(rc != GT_OK)
        return rc;
    
    /* Write Register 30 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG30, 0x0002);
    if(rc != GT_OK)
        return rc;
    
    /* Write Register 29 */
    rc = PRV_PHY_REG_WRITE_MAC(devNum, portNum, PRV_CPSS_PHY_REG29, 0x0);
    return rc;
}

/*******************************************************************************
* internal_cpssPhyErrataWaExecute
*
* DESCRIPTION:
*   This function execute workaround for PHY errata from a selected list.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       errataWa    - the selected workaround to execute
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on not initialized function pointers for
*                                  read and write phy registers operations.
*       GT_BAD_PARAM             - on wrong device, port or requested WA
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssPhyErrataWaExecute
(
   IN GT_U8                     devNum,
   IN GT_PHYSICAL_PORT_NUM      portNum,
   IN CPSS_PHY_ERRATA_WA_ENT    errataWa
)
{
    GT_STATUS   status;         /* returned status */

    if( (PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) == 0) ||
        (!PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum)) )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check read and write functions were bound */
    if (( (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                                         genVctBindFunc.cpssPhyRegisterRead) ==  NULL ) ||
        ( (PRV_CPSS_PP_VCT_BIND_FUNC_MAC(devNum, port)->
                                         genVctBindFunc.cpssPhyRegisterWrite) ==  NULL ))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    
    switch(errataWa)
    {
        case CPSS_PHY_ERRATA_WA_88E1240_SGMII_LOCKUP_E:
            status = prvCpssFixPhy1240SgmiiLockUpErrata(devNum, portNum);
            break;
        case CPSS_PHY_ERRATA_WA_88E1340_BGA_INIT_E:
            status = prvCpssFixPhy1340BgaInitErrata(devNum, portNum);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    
    return status;
}

/*******************************************************************************
* cpssPhyErrataWaExecute
*
* DESCRIPTION:
*   This function execute workaround for PHY errata from a selected list.
*
* APPLICABLE DEVICES:  
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; Puma2; Puma3; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - Device Number.
*       portNum     - Port Number.
*       errataWa    - the selected workaround to execute
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - on not initialized function pointers for
*                                  read and write phy registers operations.
*       GT_BAD_PARAM             - on wrong device, port or requested WA
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssPhyErrataWaExecute
(
   IN GT_U8                     devNum,
   IN GT_PHYSICAL_PORT_NUM      portNum,
   IN CPSS_PHY_ERRATA_WA_ENT    errataWa
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssPhyErrataWaExecute);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, errataWa));

    rc = internal_cpssPhyErrataWaExecute(devNum, portNum, errataWa);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, errataWa));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
