/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND, ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsDiagnostic.h
*
* DESCRIPTION:
*
* DEPENDENCIES:
*
******************************************************************************/
#include <private/mvHwsDiagnostic.h>

#include <private/mvHwsPortTypes.h>
#include <common/siliconIf/mvSiliconIf.h>

#include <mac/mvHwsMacIf.h>
#include <mac/geMac/mvHwsGeMacSgIf.h>
#include <mac/geMac/mvHwsGeMacIf.h>
#include <mac/geMac/mvHwsGeMacSgIf.h>
#include <mac/hglMac/mvHwsHglMacIf.h>
#include <mac/xgMac/mvHwsXgMacIf.h>
#include <mac/xlgMac/mvHwsXlgMacIf.h>
#include <mac/ilknMac/mvHwsIlknMacIf.h>
#include <mac/geMac/mvHwsGeMac28nmIf.h>
#include <mac/xgMac/mvHwsXgMac28nmIf.h>
#include <mac/xlgMac/mvHwsXlgMac28nmIf.h>

#include <pcs/mvHwsPcsIf.h>
#include <pcs/gPcs/mvHwsGPcsIf.h>
#include <pcs/hglPcs/mvHwsHglPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsV2If.h>
#include <pcs/xPcs/mvHwsXPcsIf.h>
#include <pcs/gPcs/mvHwsGPcs28nmIf.h>
#include <pcs/mmPcs/mvHwsMMPcs28nmIf.h>
#include <pcs/gPcs/mvHwsGPcs28nmIf.h>
#include <pcs/ilknPcs/mvHwsIlknPcsIf.h>

#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/comPhyH/mvComPhyHIf.h>
#include <serdes/comPhyHRev2/mvComPhyHRev2If.h>
#include <serdes/mvHwsSerdes28nmPrvIf.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3If.h>

#include <mvDdr3TrainingIpPrvIf.h>
#include <mvHwsDdr3Bc2.h>

GT_STATUS mvHwsDiagCheckValidInputParams(GT_U8 devNum, MV_HWS_DEV_TYPE devType);
GT_STATUS mvHwsDiagPCSInitDBStatusGet(GT_U8 devNum);
GT_STATUS mvHwsDiagMACInitDBStatusGet(GT_U8 devNum);
GT_STATUS mvHwsDiagSerdesInitDBStatusGet(GT_U8 devNum);
GT_STATUS mvHwsDiagDdr3TipInitDBStatusGet(GT_U8 devNum);

char *DEV_TYPE[LAST_SIL_TYPE] =
{
    "Lion2A0",
    "Lion2B0",
    "Puma3A0",
    "HooperA0",
    "Puma3B0",
    "NP5A0",
    "BobcatA0",
    "Alleycat3A0",
    "NP5B0"
};

#define MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, pcsMode) \
{ \
    osPrintf("Init DB for: devNum=%d, siliconType=%s, pcsMode=%s is wrong\n", devNum, DEV_TYPE[sType], pcsMode); \
    return GT_BAD_PARAM; \
}

#define MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, macMode) \
{ \
    osPrintf("Init DB for: devNum=%d, siliconType=%s, macMode=%s is wrong\n", devNum, DEV_TYPE[sType], macMode); \
    return GT_BAD_PARAM; \
}

#define MV_HWS_RETURN_SERDES_BAD_PRT(devNum, sType, serdesMode) \
{ \
    osPrintf("Init DB for: devNum=%d, siliconType=%s, serdesMode=%s is wrong\n", devNum, DEV_TYPE[sType], serdesMode); \
    return GT_BAD_PARAM; \
}

/*******************************************************************************
* mvHwsDiagDeviceDbCheck
*
* DESCRIPTION: This API checks the DB initialization of PCS, MAC, Serdes
*              and DDR units
*
* INPUTS:
*       devNum      - device number
*       devType     - device type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
*******************************************************************************/
GT_STATUS mvHwsDiagDeviceDbCheck(GT_U8 devNum, MV_HWS_DEV_TYPE devType)
{
    CHECK_STATUS(mvHwsDiagCheckValidInputParams(devNum, devType));
    CHECK_STATUS(mvHwsDiagPCSInitDBStatusGet(devNum));
    CHECK_STATUS(mvHwsDiagMACInitDBStatusGet(devNum));
    CHECK_STATUS(mvHwsDiagSerdesInitDBStatusGet(devNum));

    /* TBD - Need to add checking to DDR initialization DB */
    /* CHECK_STATUS(mvHwsDiagDdr3TipInitDBStatusGet(devNum)); */

    return GT_OK;
}

/*******************************************************************************
* mvHwsDiagCheckValidInputParams
*
* DESCRIPTION: This function checks the validity of input devNum and devType
*              parameters
*
* INPUTS:
*       devNum      - device number
*       devType     - device type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
*******************************************************************************/
GT_STATUS mvHwsDiagCheckValidInputParams(GT_U8 devNum, MV_HWS_DEV_TYPE devType)
{
    if(devNum >= HWS_MAX_DEVICE_NUM)
    {
        osPrintf("mvHwsDiagDeviceInitDBStatusGet: Device number=%d is bigger than HWS_MAX_DEVICE_NUM=%d value\n", devNum, HWS_MAX_DEVICE_NUM-1);
        return GT_BAD_PARAM;
    }

    if(hwsDeviceSpecInfo[devNum].devNum != devNum)
    {
        osPrintf("mvHwsDiagDeviceInitDBStatusGet: Device number=%d is not initialized in the system\n", devNum);
        return GT_BAD_PARAM;
    }

    if(HWS_DEV_SILICON_TYPE(devNum) != devType)
    {
        osPrintf("mvHwsDiagDeviceInitDBStatusGet: Device number=%d is different than Device type %s\n", devNum, DEV_TYPE[devType]);
        return GT_BAD_PARAM;
    }

    if(hwsDeviceSpecInfo[devNum].devType >= sizeof(DEV_TYPE)/sizeof(char*))
    {
        osPrintf("mvHwsDiagDeviceInitDBStatusGet: the size of DEV_TYPE array is wrong\n");
        return GT_BAD_VALUE;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsDiagPCSInitDBStatusGet
*
* DESCRIPTION: This function checks the DB initialization of PCS unit
*
*
* INPUTS:
*       devNum    - system device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
*******************************************************************************/
GT_STATUS mvHwsDiagPCSInitDBStatusGet(GT_U8 devNum)
{
    MV_HWS_DEV_TYPE sType = HWS_DEV_SILICON_TYPE(devNum);

    MV_HWS_PCS_FUNC_PTRS *hwsPcsFuncsPtr;
    CHECK_STATUS(hwsPcsGetFuncPtr(devNum, &hwsPcsFuncsPtr));

    switch (sType)
    {
    case Lion2B0:
    case HooperA0:
        if(hwsPcsFuncsPtr[GPCS].pcsModeCfgFunc != mvHwsGPcsMode)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "GPCS");
        }

        if(hwsPcsFuncsPtr[MMPCS].pcsModeCfgFunc != mvHwsMMPcsV2Mode)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "MMPCS");
        }

        if(hwsPcsFuncsPtr[HGLPCS].pcsModeCfgFunc != mvHwsHglPcsMode)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "HGLPCS");
        }

        if(hwsPcsFuncsPtr[XPCS].pcsModeCfgFunc != mvHwsXPcsMode)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "XPCS");
        }
        break;

    case Alleycat3A0:
    case BobcatA0:
        if(hwsPcsFuncsPtr[XPCS].pcsModeCfgFunc != mvHwsXPcsMode)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "XPCS");
        }

        if(hwsPcsFuncsPtr[GPCS].pcsModeCfgFunc != mvHwsGPcs28nmModeCfg)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "GPCS");
        }

        if(hwsPcsFuncsPtr[GPCS_NET].pcsModeCfgFunc != mvHwsGPcsNet28nmModeCfg)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "GPCS_NET");
        }

        if(hwsPcsFuncsPtr[QSGMII_PCS].pcsModeCfgFunc != mvHwsGPcsNet28nmModeCfg)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "QSGMII_PCS");
        }

        if(hwsPcsFuncsPtr[MMPCS].pcsModeCfgFunc != mvHwsMMPcs28nmMode)
        {
            MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "MMPCS");
        }
        break;
    default:
        osPrintf("mvHwsDiagPCSInitDBStatusGet: Unsupported device type=%d\n", sType);
        return GT_FAIL;
    }

    if((sType == BobcatA0) && (hwsPcsFuncsPtr[INTLKN_PCS].pcsModeCfgFunc != mvHwsIlknPcsMode))
    {
        MV_HWS_RETURN_PCS_BAD_PRT(devNum, sType, "INTLKN_PCS");
    }

    osPrintf("PCS Initialization DB for: devNum=%d, siliconType=%s is corrected\n", devNum, DEV_TYPE[sType]);

    return GT_OK;
}

/*******************************************************************************
* mvHwsDiagMACInitDBStatusGet
*
* DESCRIPTION: This function checks the DB initialization of MAC unit
*
*
* INPUTS:
*       devNum    - system device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
*******************************************************************************/
GT_STATUS mvHwsDiagMACInitDBStatusGet(GT_U8 devNum)
{
    MV_HWS_DEV_TYPE sType = HWS_DEV_SILICON_TYPE(devNum);

    MV_HWS_MAC_FUNC_PTRS *hwsMacFuncsPtr;
    CHECK_STATUS(hwsMacGetFuncPtr(devNum, &hwsMacFuncsPtr));

    switch (sType)
    {
    case Lion2B0:
    case HooperA0:
        if(hwsMacFuncsPtr[GEMAC_X].macModeCfgFunc != mvHwsGeMacModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "GEMAC_X");
        }

        if(hwsMacFuncsPtr[GEMAC_SG].macModeCfgFunc != mvHwsGeMacSgModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "GEMAC_SG");
        }

        if(hwsMacFuncsPtr[HGLMAC].macModeCfgFunc != mvHwsHglMacModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "HGLMAC");
        }

        if(hwsMacFuncsPtr[XGMAC].macModeCfgFunc != mvHwsXgMacModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "XGMAC");
        }

        if(hwsMacFuncsPtr[XLGMAC].macModeCfgFunc != mvHwsXlgMacModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "XLGMAC");
        }
        break;

    case Alleycat3A0:
    case BobcatA0:
        if(hwsMacFuncsPtr[GEMAC_X].macModeCfgFunc != mvHwsGeMac28nmModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "GEMAC_X");
        }

        if(hwsMacFuncsPtr[GEMAC_NET_X].macModeCfgFunc != mvHwsGeNetMac28nmModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "GEMAC_NET_X");
        }

        if(hwsMacFuncsPtr[QSGMII_MAC].macModeCfgFunc != mvHwsQsgmiiMac28nmModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "QSGMII_MAC");
        }

        if(hwsMacFuncsPtr[GEMAC_SG].macModeCfgFunc != mvHwsGeMacSgModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "GEMAC_SG");
        }

        if(hwsMacFuncsPtr[GEMAC_NET_SG].macModeCfgFunc != mvHwsGeNetMacSgModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "GEMAC_NET_SG");
        }

        if(hwsMacFuncsPtr[XGMAC].macModeCfgFunc != mvHwsXgMac28nmModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "XGMAC");
        }

        if(hwsMacFuncsPtr[XLGMAC].macModeCfgFunc != mvHwsXlgMac28nmModeCfg)
        {
            MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "XLGMAC");
        }
        break;
    default:
        osPrintf("mvHwsDiagMACInitDBStatusGet: Unsupported device type=%d\n", sType);
        return GT_FAIL;
    }

    if((sType == BobcatA0) && (hwsMacFuncsPtr[INTLKN_MAC].macModeCfgFunc != mvHwsIlknMacModeCfg))
    {
        MV_HWS_RETURN_MAC_BAD_PRT(devNum, sType, "INTLKN_MAC");
    }

    osPrintf("MAC Initialization DB for: devNum=%d, siliconType=%s is corrected\n", devNum, DEV_TYPE[sType]);

    return GT_OK;
}

/*******************************************************************************
* mvHwsDiagSerdesInitDBStatusGet
*
* DESCRIPTION: This function checks the DB initialization of Serdes unit
*
*
* INPUTS:
*       devNum    - system device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
*******************************************************************************/
GT_STATUS mvHwsDiagSerdesInitDBStatusGet(GT_U8 devNum)
{
    MV_HWS_DEV_TYPE sType  = HWS_DEV_SILICON_TYPE(devNum);
    HWS_DEV_GOP_REV gopRev = HWS_DEV_GOP_REV(devNum);

    MV_HWS_SERDES_FUNC_PTRS *hwsSerdesFuncsPtr;
    CHECK_STATUS(hwsSerdesGetFuncPtr(devNum, &hwsSerdesFuncsPtr));

    switch (sType)
    {
    case Lion2B0:
    case HooperA0:
        if(hwsSerdesFuncsPtr[COM_PHY_H_REV2].serdesPowerCntrlFunc != mvHwsComHRev2SerdesPowerCtrl)
        {
            MV_HWS_RETURN_SERDES_BAD_PRT(devNum, sType, "COM_PHY_H_REV2");
        }
        break;
    case Alleycat3A0:
    case BobcatA0:
        switch (gopRev)
        {
        case GOP_28NM_REV1:
            if(hwsSerdesFuncsPtr[COM_PHY_28NM].serdesPowerCntrlFunc != mvHwsComH28nmSerdesPowerCtrl)
            {
                MV_HWS_RETURN_SERDES_BAD_PRT(devNum, sType, "COM_PHY_28NM");
            }
            break;
        case GOP_28NM_REV2:
            if(hwsSerdesFuncsPtr[COM_PHY_28NM].serdesManualRxCfgFunc != mvHwsComH28nmRev3SerdesManualRxConfig)
            {
                MV_HWS_RETURN_SERDES_BAD_PRT(devNum, sType, "COM_PHY_28NM");
            }
            break;
        default:
            osPrintf("mvHwsDiagSerdesInitDBStatusGet: Unsupported GOP revision\n");
            return GT_FAIL;
        }
        break;
    default:
        osPrintf("mvHwsDiagSerdesInitDBStatusGet: Unsupported device type=%d\n", sType);
        return GT_FAIL;
    }

    if((sType == Alleycat3A0) && (hwsSerdesFuncsPtr[COM_PHY_28NM].serdesManualRxCfgFunc != mvHwsComH28nmRev3SerdesManualRxConfig))
    {
        MV_HWS_RETURN_SERDES_BAD_PRT(devNum, sType, "COM_PHY_28NM");
    }

    osPrintf("Serdes Initialization DB for: devNum=%d, siliconType=%s is corrected\n", devNum, DEV_TYPE[sType]); 

    return GT_OK;
}

#if 0
/*******************************************************************************
* mvHwsDiagDdr3TipInitDBStatusGet
*
* DESCRIPTION: This function checks the DB initialization of DDR unit
*
*
* INPUTS:
*       devNum    - system device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
*******************************************************************************/
GT_STATUS mvHwsDiagDdr3TipInitDBStatusGet(GT_U8 devNum)
{
    MV_HWS_DEV_TYPE sType = HWS_DEV_SILICON_TYPE(devNum);

    MV_HWS_TRAINING_IP_FUNC_PTRS *hwsDdr3TipFuncsPtr;

    /* need to fix warning here, the hwsTrainingIpFuncsPtr declared
       as static function in mvHwsDdr3TrainingIpIf.c */
    CHECK_STATUS(hwsTrainingIpFuncsPtr(devNum, &hwsDdr3TipFuncsPtr));

    switch (sType)
    {
    case BobcatA0:
        if(hwsDdr3TipFuncsPtr->trainingIpInitController != mvHwsDdr3TipInitController)
        {
            osPrintf("Init DB for: devNum=%d, siliconType=%s, portMode=%s is wrong\n", devNum, DEV_TYPE[sType], "Ddr3Tip");
            return GT_BAD_PARAM;
        }
        break;
    default:
        osPrintf("mvHwsDiagDdr3TipInitDBStatusGet: Unsupported device type=%d\n", sType);
        return GT_FAIL;
    }

    osPrintf("Ddr3Tip Initialization DB for: devNum=%d, siliconType=%s is corrected\n", devNum, DEV_TYPE[sType]); 

    return GT_OK;
}
#endif


