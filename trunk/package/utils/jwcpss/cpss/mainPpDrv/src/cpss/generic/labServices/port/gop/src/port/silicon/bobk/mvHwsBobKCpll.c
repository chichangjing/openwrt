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
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsBobKCpllInit.c
*
* DESCRIPTION:
*       BobK CPLL init
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <silicon/bobk/mvHwsBobKCpll.h>
/************************* definition *****************************************************/


/************************* Globals *******************************************************/
GT_U32 cpllDataArray25to156[MAX_CPLL_DATA_INDEX]=
{
    0x7010005,  0x0,        0x7080000,  0x0,        0x7080000,  0x0,        0x711080A,
    0x0,        0x711080A,  0x0,        0x70E1B18,  0x0,        0x726E641,  0x0,
    0x7200420,  0x0,        0x71f3800,  0x0,        0x719448D,  0x0,        0x7275600,
    0x0,        0x7092150,  0x0,        0x70D4400,  0x0,        0x70A8555,  0x0,
    0x70B5555,  0x0,        0x7092170,  0x0,        0x7092150,  0x0,        0x7080010,
    0x0
};

GT_U32 cpllDataArray25to78[MAX_CPLL_DATA_INDEX] =
{
    0x7010005,  0x0,        0x7080000,  0x0,        0x7080000,  0x0,        0x711080A,
    0x0,        0x711080A,  0x0,        0x70E1B18,  0x0,        0x726E641,  0x0,
    0x7200420,  0x0,        0x71f3800,  0x0,        0x719448D,  0x0,        0x7275600,
    0x0,        0x7092150,  0x0,        0x70D4800,  0x0,        0x70A8555,  0x0,
    0x70B5555,  0x0,        0x7092170,  0x0,        0x7092150,  0x0,        0x7080010,
    0x0
};

GT_U32 cpllDataArray25to200[MAX_CPLL_DATA_INDEX] =
{
    0x7010005,  0x0,        0x7080000,  0x0,        0x7080000,  0x0,        0x711080A,
    0x0,        0x711080A,  0x0,        0x70E1B18,  0x0,        0x726E641,  0x0,
    0x7200420,  0x0,        0x71f3800,  0x0,        0x719448D,  0x0,        0x7275600,
    0x0,        0x7092210,  0x0,        0x70DA400,  0x0,        0x70A5800,  0x0,
    0x70B0000,  0x0,        0x7092230,  0x0,        0x7092210,  0x0,        0x7080010,
    0x0
};

GT_U32 cpllDataArray156to156[MAX_CPLL_DATA_INDEX] =
{
    0x7010005,  0x0,        0x7080000,  0x0,        0x7080000,  0x0,        0x711080A,
    0x0,        0x711080A,  0x0,        0x70E1B18,  0x0,        0x726E641,  0x0,
    0x7200420,  0x0,        0x71f3800,  0x0,        0x719448D,  0x0,        0x7275600,
    0x0,        0x709C010,  0x0,        0x70D4400,  0x0,        0x70A8000,  0x0,
    0x70B0000,  0x0,        0x709C030,  0x0,        0x709C010,  0x0,        0x7080010,
    0x0
};

GT_U32 cpllDataArray156to78[MAX_CPLL_DATA_INDEX] =
{
    0x7010005,  0x0,        0x7080000,  0x0,        0x7080000,  0x0,        0x711080A,
    0x0,        0x711080A,  0x0,        0x70E1B18,  0x0,        0x726E641,  0x0,
    0x7200420,  0x0,        0x71f3800,  0x0,        0x719448D,  0x0,        0x7275600,
    0x0,        0x709C010,  0x0,        0x70D4800,  0x0,        0x70A8000,  0x0,
    0x70B0000,  0x0,        0x709C030,  0x0,        0x709C010,  0x0,        0x7080010,
    0x0
};

GT_U32 cpllDataArray156to200[MAX_CPLL_DATA_INDEX] ={0};

GT_U32 *cpllDataArray [MV_HWS_MAX_INPUT_FREQUENCY][MV_HWS_MAX_OUTPUT_FREQUENCY]=
{
    {cpllDataArray25to156,  cpllDataArray25to78,  cpllDataArray25to200},
    {cpllDataArray156to156, cpllDataArray156to78, cpllDataArray156to200}
};

/************************* pre-declaration ***********************************************/


/*****************************************************************************************/

/*******************************************************************************
* mvHwsCpllControl
*
* DESCRIPTION:
*       Configure the requested CPLL (0, 1, 2) or bypass on specific CPLL
*
* INPUTS:
*       devNum   - system device number
*       cpllNum  - cpll number
*       bypassEn - bypass enable/disable
*       inFreq   - input frequency
*       outFreq  - output frequency
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCpllControl
(
    GT_U8                           devNum,
    MV_HWS_CPLL_NUMBER              cpllNum,
    GT_BOOL                         bypass_en,
    MV_HWS_CPLL_INPUT_FREQUENCY     inFreq,
    MV_HWS_CPLL_OUTPUT_FREQUENCY    outFreq
)
{
    GT_U32  i, mask, mask1;
    GT_U32  *data_ptr;
    GT_U32  data;

    switch (cpllNum)
    {
    case CPLL0: /* clum_gop_cpll1_bypass */
        mask  = 0xFCFFFFFF;
        mask1 = 0x400;
        break;
    case CPLL1: /* clum_gop_cpll0_bypass */
        mask  = 0xFAFFFFFF;
        mask1 = 0x200;
        break;
    case CPLL2: /* ctus_gop_cpll_bypass */
        mask  = 0xF9FFFFFF;
        mask1 = 0x100;
        break;
    default:
        return GT_BAD_PARAM;
    }

    if (bypass_en)
    {
        CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, &data));
        CHECK_STATUS(hwsServerRegSetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, data & ~mask1))
    }
    else
    {
        CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, &data));
        CHECK_STATUS(hwsServerRegSetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, data | mask1));

        data_ptr = cpllDataArray[inFreq][outFreq];

        for (i=0; i < MAX_CPLL_DATA_INDEX; i++)
        {
            CHECK_STATUS(hwsServerRegSetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_21, (*(data_ptr+i) & mask)));

            /* set 20ms delay */
            hwsOsExactDelayPtr(devNum, devNum, 20);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsSrcClockModeConfigSet
*
* DESCRIPTION:
*       Configure the setting of registers and requested CPLL(0, 1, 2) on SERDES
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - SERDES number
*       extConfigRegValue  - setting value for SERDES_EXTERNAL_CONFIGURATION_0
*       setGeneralCtlReg17 - set/clear bit #19 in DEVICE_GENERAL_CONTROL_17
*                            GT_FALSE: clear bit #19
*                            GT_TRUE: set bit #19
*       setGeneralCtlReg20 - set/clear bit #11 in DEVICE_GENERAL_CONTROL_20
*                            GT_FALSE: clear bit #11
*                            GT_TRUE: set bit #11
*       cpllNum  - cpll number
*       inFreq   - input frequency
*       outFreq  - output frequency
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
static GT_STATUS mvHwsSrcClockModeConfigSet
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          serdesNum,
    GT_U32                          extConfigRegValue,
    GT_BOOL                         setGeneralCtlReg17,
    GT_BOOL                         setGeneralCtlReg20,
    MV_HWS_CPLL_NUMBER              cpllNum,
    MV_HWS_CPLL_INPUT_FREQUENCY     inFreq,
    MV_HWS_CPLL_OUTPUT_FREQUENCY    outFreq
)
{
    GT_U32  data;

    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, extConfigRegValue, (1 << 8)));

    CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_17, &data));
    data = (setGeneralCtlReg17) ? (data | (1 << 19)) : (data & ~(1 << 19));
    CHECK_STATUS(hwsServerRegSetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_17, data));

    if ((setGeneralCtlReg20 == GT_TRUE) || (setGeneralCtlReg20 == GT_FALSE))
    {
        CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, &data));
        data = (setGeneralCtlReg20) ? (data | (1 << 11)) : (data & ~(1 << 11));
        CHECK_STATUS(hwsServerRegSetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, data));
    }

    if ((cpllNum == CPLL0) || (cpllNum == CPLL1) || (cpllNum == CPLL2))
        CHECK_STATUS(mvHwsCpllControl(devNum, cpllNum, GT_FALSE, inFreq, outFreq));

    return GT_OK;
}


/*******************************************************************************
* mvHwsSerdesClockGlobalControlGet
*
* DESCRIPTION:
*       Getting Input clock source after
*       Global configuration per Serdes for referance clock control source.
*
* INPUTS:
*       devNum             - system device number
*       serdesNum          - SERDES number
*
* OUTPUTS:
*       srcClockPtr        - Input clock source
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on hardware error
*       GT_NOT_INITIALIZED - if the driver was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_BAD_PTR         - srcClockPtr is NULL
*       GT_BAD_STATE       - on bad state
*
*******************************************************************************/
GT_STATUS mvHwsSerdesClockGlobalControlGet
(
    GT_U8                           devNum,
    GT_U32                          serdesNum,
    MV_HWS_INPUT_SRC_CLOCK         *srcClockPtr
)
{
    GT_U32 data;
    GT_U32 data1;
    GT_U32 portGroup = 0;

    if(NULL == srcClockPtr)
    {
        return GT_BAD_PTR;
    }

    if (serdesNum < 12)
    {
        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &data, 0));
        if((data & (1 << 8)) == 0)
        {
            CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, &data1));
            if((data1 & (1 << 11)) == 0)
            {
                CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_17, &data));
                if((data & (1 << 19)) == 0)
                {
                    *srcClockPtr = MV_HWS_ANA_GRP0;
                }
                else
                {
                    return GT_BAD_STATE;
                }
            }
            else
            {
                CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_17, &data));
                if((data & (1 << 19)) == 0)
                {
                    if((data1 & (1 << 9)) > 0)
                    {
                        *srcClockPtr = MV_HWS_ANA_GRP1;
                    }
                    else
                    {
                        return GT_BAD_STATE;
                    }
                }
                else
                {
                    return GT_BAD_STATE;
                }
            }
        }
        else
        {
            CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_17, &data));
            if((data & (1 << 19)) == 0)
            {
                CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, &data1));
                if((data1 & (1 << 11)) == 0)
                {
                    if((data1 & (1 << 10)) > 0)
                    {
                        *srcClockPtr = MV_HWS_CPLL;
                    }
                    else
                    {
                        return GT_BAD_STATE;
                    }
                }
                else
                {
                    return GT_BAD_STATE;
                }
            }
            else
            {
                CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, &data1));
                if((data1 & (1 << 11)) == 0)
                {
                    if((data1 & (1 << 10)) > 0)
                    {
                        *srcClockPtr = MV_HWS_RCVR_CLK_IN;
                    }
                    else
                    {
                        return GT_BAD_STATE;
                    }
                }
                else
                {
                    return GT_BAD_STATE;
                }
            }
        }
    }
    else
    {
        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &data, 0));
        if((data & (1 << 8)) == 0)
        {
            *srcClockPtr = MV_HWS_ANA_GRP1;
        }
        else
        {
            CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_17, &data));
            CHECK_STATUS(hwsServerRegGetFuncPtr(devNum, DEVICE_GENERAL_CONTROL_20, &data1));
            if((data & (1 << 19)) == 0)
            {
                if((data1 & (1 << 8)) > 0)
                {
                     *srcClockPtr = MV_HWS_CPLL;
                }
                else
                {
                    return GT_BAD_STATE;
                }
            }
            else
            {
                if((data1 & (1 << 8)) > 0)
                {
                     *srcClockPtr = MV_HWS_RCVR_CLK_IN;
                }
                else
                {
                    return GT_BAD_STATE;
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsSerdesClockGlobalControl
*
* DESCRIPTION:
*       Global configuration per Serdes for referance clock control source.
*       Need to be done before any Serdes in the system is powered up.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - SERDES number
*       srcClock  - Input clock source
*       inFreq    - input frequency
*       outFreq   - output frequency
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsSerdesClockGlobalControl
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          serdesNum,
    MV_HWS_INPUT_SRC_CLOCK          srcClock,
    MV_HWS_CPLL_INPUT_FREQUENCY     inFreq,
    MV_HWS_CPLL_OUTPUT_FREQUENCY    outFreq
)
{
    if (serdesNum < 12)
    {
        switch (srcClock)
        {
        case MV_HWS_ANA_GRP1:
            /* For CPLL1: set bit #8=0 in SERDES_EXTERNAL_CONFIGURATION_0, clear bit #19 in DEVICE_GENERAL_CONTROL_17,
               set bit #11 in DEVICE_GENERAL_CONTROL_20 */
            CHECK_STATUS(mvHwsSrcClockModeConfigSet(devNum, portGroup, serdesNum, 0, GT_FALSE, GT_TRUE, CPLL1, inFreq, outFreq));
            break;
        case MV_HWS_ANA_GRP0:
            /* 0xFFFF = No CPLL configuration */
            /* set bit #8=0 in SERDES_EXTERNAL_CONFIGURATION_0, clear bit #19 in DEVICE_GENERAL_CONTROL_17,
               clear bit #11 in DEVICE_GENERAL_CONTROL_20 */
            CHECK_STATUS(mvHwsSrcClockModeConfigSet(devNum, portGroup, serdesNum, 0, GT_FALSE, GT_FALSE, 0xFFFF, inFreq, outFreq));
            break;
        case MV_HWS_CPLL:
            /* For CPLL0: set bit #8=1 in SERDES_EXTERNAL_CONFIGURATION_0, clear bit #19 in DEVICE_GENERAL_CONTROL_17,
               clear bit #11 in DEVICE_GENERAL_CONTROL_20 */
            CHECK_STATUS(mvHwsSrcClockModeConfigSet(devNum, portGroup, serdesNum, (1 << 8), GT_FALSE, GT_FALSE, CPLL0, inFreq, outFreq));
            break;
        case MV_HWS_RCVR_CLK_IN:
            /* For CPLL0: set bit #8=1 in SERDES_EXTERNAL_CONFIGURATION_0, set bit #19 in DEVICE_GENERAL_CONTROL_17,
               clear bit #11 in DEVICE_GENERAL_CONTROL_20 */
            CHECK_STATUS(mvHwsSrcClockModeConfigSet(devNum, portGroup, serdesNum, (1 << 8), GT_TRUE, GT_FALSE, CPLL0, inFreq, outFreq));
            break;
        default:
            return GT_BAD_PARAM;
        }
    }
    else
    {
        switch (srcClock)
        {
           case MV_HWS_ANA_GRP1:
               CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, portGroup, EXTERNAL_REG, serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, 0, (1 << 8)));
               break;
           case MV_HWS_CPLL:
               /* 0xFFFF = No configuration for DEVICE_GENERAL_CONTROL_20 */
               /* For CPLL2: set bit #8=1 in SERDES_EXTERNAL_CONFIGURATION_0, clear bit #19 in DEVICE_GENERAL_CONTROL_17 */
               CHECK_STATUS(mvHwsSrcClockModeConfigSet(devNum, portGroup, serdesNum, (1 << 8), GT_FALSE, 0xFFFF, CPLL2, inFreq, outFreq));
               break;
           case MV_HWS_RCVR_CLK_IN:
               /* 0xFFFF = No configuration for DEVICE_GENERAL_CONTROL_20 */
               /* For CPLL2: set bit #8=1 in SERDES_EXTERNAL_CONFIGURATION_0, set bit #19 in DEVICE_GENERAL_CONTROL_17 */
               CHECK_STATUS(mvHwsSrcClockModeConfigSet(devNum, portGroup, serdesNum, (1 << 8), GT_TRUE, 0xFFFF, CPLL2, inFreq, outFreq));
               break;
           default:
               return GT_BAD_PARAM;
       }
    }

    return GT_OK;
}


