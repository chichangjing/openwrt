#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegsVer1.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPizzaArbiterWorkConserving.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/gtDbDxBobcat2PhyConfig.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern GT_STATUS cpssInitSystem    
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
);

extern GT_STATUS appDemoTraceHwAccessEnable
(
    IN GT_U8                                devNum,
    IN APP_DEMO_TRACE_HW_ACCESS_TYPE_ENT    accessType,
    IN GT_BOOL                              enable
);


extern GT_STATUS appDemoDbEntryAdd
(
    IN  GT_CHAR *namePtr,
    IN  GT_U32  value
);

extern GT_STATUS appDemoDebugDeviceIdSet
(
    IN GT_U8    devNum,
    IN GT_U32   deviceId
);

extern GT_STATUS appDemoBc2PortListInit
(
    IN  GT_U8 dev,
    PortInitList_STC * portInitList
);

extern GT_STATUS gtBobcat2PortPizzaArbiterIfStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
);

extern GT_STATUS gtBobcat2PortPizzaArbiterIfUnitStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId,
    IN  CPSS_DXCH_PA_UNIT_ENT unit
);

extern GT_STATUS appDemoBobcat2PortListResourcesPrint
(
    IN GT_U8 dev
);

extern GT_STATUS appDemoBobk2PortGobalResourcesPrint
(
    IN GT_U8 dev
);


extern GT_STATUS gtBobcat2PortMappingDump
(
    IN  GT_U8  dev
);

extern GT_STATUS appDemoBcat2B0SimCoreClockSet
(
    GT_U8  devNum,
    GT_U32 coreClock
);


#ifdef __cplusplus
}
#endif /* __cplusplus */



/* just for test porpose */
static CPSS_DXCH_PORT_MAP_STC bc2TmEnableMap_40G_TM_IlknChannel_CPU[] =
{ /* Port,            mappingType                           portGroupm, intefaceNum, txQPort,          tmEnable,    tmPortInd*/
     {  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,   GT_NA,             GT_TRUE,           48 }
    ,{ 128, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        0,   GT_NA,             GT_TRUE,          128 }
    ,{ 129, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        1,   GT_NA,             GT_TRUE,          129 }
    ,{ 130, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        2,   GT_NA,             GT_TRUE,          130 }
    ,{ 131, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        3,   GT_NA,             GT_TRUE,          131 }
    ,{ 132, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        4,   GT_NA,             GT_TRUE,          132 }
    ,{ 133, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        5,   GT_NA,             GT_TRUE,          133 }
    ,{ 134, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        6,   GT_NA,             GT_TRUE,          134 }
    ,{ 135, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        7,   GT_NA,             GT_TRUE,          135 }
    ,{ 136, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        8,   GT_NA,             GT_TRUE,          136 }
    ,{ 137, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        9,   GT_NA,             GT_TRUE,          137 }
    /* */
    ,{ 138, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       10,   GT_NA,             GT_TRUE,          138 }
    ,{ 139, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       11,   GT_NA,             GT_TRUE,          139 }
    ,{ 140, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       12,   GT_NA,             GT_TRUE,          140 }
    ,{ 141, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       13,   GT_NA,             GT_TRUE,          141 }
    ,{ 142, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       14,   GT_NA,             GT_TRUE,          142 }
    ,{ 143, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       15,   GT_NA,             GT_TRUE,          143 }
    ,{ 144, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       16,   GT_NA,             GT_TRUE,          144 }
    ,{ 145, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       17,   GT_NA,             GT_TRUE,          145 }
    ,{ 146, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       18,   GT_NA,             GT_TRUE,          146 }
    ,{ 147, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       19,   GT_NA,             GT_TRUE,          147 }
    /* */
    ,{ 148, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       20,   GT_NA,             GT_TRUE,          148 }
    ,{ 149, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       21,   GT_NA,             GT_TRUE,          149 }
    ,{ 150, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       22,   GT_NA,             GT_TRUE,          150 }
    ,{ 151, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       23,   GT_NA,             GT_TRUE,          151 }
    ,{ 152, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       24,   GT_NA,             GT_TRUE,          152 }
    ,{ 153, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       25,   GT_NA,             GT_TRUE,          153 }
    ,{ 154, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       26,   GT_NA,             GT_TRUE,          154 }
    ,{ 155, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       27,   GT_NA,             GT_TRUE,          155 }
    ,{ 156, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       28,   GT_NA,             GT_TRUE,          156 }
    ,{ 157, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       29,   GT_NA,             GT_TRUE,          157 }
    /* */
    ,{ 158, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       30,   GT_NA,             GT_TRUE,          158 }
    ,{ 159, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       31,   GT_NA,             GT_TRUE,          159 }
    ,{ 160, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       32,   GT_NA,             GT_TRUE,          160 }
    ,{ 161, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       33,   GT_NA,             GT_TRUE,          161 }
    ,{ 162, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       34,   GT_NA,             GT_TRUE,          162 }
    ,{ 163, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       35,   GT_NA,             GT_TRUE,          163 }
    ,{ 164, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       36,   GT_NA,             GT_TRUE,          164 }
    ,{ 165, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       37,   GT_NA,             GT_TRUE,          165 }
    ,{ 166, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       38,   GT_NA,             GT_TRUE,          166 }
    ,{ 167, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       39,   GT_NA,             GT_TRUE,          167 }
    /* */
    ,{ 168, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       40,   GT_NA,             GT_TRUE,          168 }
    ,{ 169, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       41,   GT_NA,             GT_TRUE,          169 }
    ,{ 170, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       42,   GT_NA,             GT_TRUE,          170 }
    ,{ 171, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       43,   GT_NA,             GT_TRUE,          171 }
    ,{ 172, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       44,   GT_NA,             GT_TRUE,          172 }
    ,{ 173, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       45,   GT_NA,             GT_TRUE,          173 }
    ,{ 174, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       46,   GT_NA,             GT_TRUE,          174 }
    ,{ 175, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       47,   GT_NA,             GT_TRUE,          175 }
    ,{ 176, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       48,   GT_NA,             GT_TRUE,          176 }
    ,{ 177, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       49,   GT_NA,             GT_TRUE,          177 }
    /* */
    ,{ 178, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       50,   GT_NA,             GT_TRUE,          178 }
    ,{ 179, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       51,   GT_NA,             GT_TRUE,          179 }
    ,{ 180, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       52,   GT_NA,             GT_TRUE,          180 }
    ,{ 181, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       53,   GT_NA,             GT_TRUE,          181 }
    ,{ 182, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       54,   GT_NA,             GT_TRUE,          182 }
    ,{ 183, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       55,   GT_NA,             GT_TRUE,          183 }
    ,{ 184, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       56,   GT_NA,             GT_TRUE,          184 }
    ,{ 185, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       57,   GT_NA,             GT_TRUE,          185 }
    ,{ 186, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       58,   GT_NA,             GT_TRUE,          186 }
    ,{ 187, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       59,   GT_NA,             GT_TRUE,          187 }
    /* */
    ,{ 188, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       60,   GT_NA,             GT_TRUE,          188 }
    ,{ 189, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       61,   GT_NA,             GT_TRUE,          189 }
    ,{ 190, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       62,   GT_NA,             GT_TRUE,          190 }
    ,{ 191, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       63,   GT_NA,             GT_TRUE,          191 }
    /* */
    ,{  63, CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,                   0,    GT_NA,      63,            GT_FALSE,        GT_NA }
};


GT_STATUS paTest_EthTXFIFO_IlknTxFIFO
(
    /* GT_U8 dev */
)
{
    GT_STATUS rc;
    GT_U32 i;
    CPSS_PORTS_BMP_STC portsBmp;
    GT_U32             actualSpeed;
    GT_U32             speed2configure;
    GT_U32             estIlknIFBW;
    GT_U8 dev = 0;

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU)/sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]),
                                        &bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU)/sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]),
                                     &bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,48);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_SR_LR_E,CPSS_PORT_SPEED_40000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,128);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_ILKN8_E,CPSS_PORT_SPEED_40000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    speed2configure = 625;
    rc = cpssDxChPortIlknChannelSpeedResolutionSet(dev,speed2configure,/*OUT*/&estIlknIFBW);
    if (rc != GT_OK)
    {
        return rc;
    }



    osPrintf("\n+-------------------------------");
    osPrintf("\n| Before Ilkn                   ");
    osPrintf("\n+-------------------------------");
    speed2configure = 625;
    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);
    for (i = 0 ; i < 64 ; i++)
    {
        osPrintf("\n  Channel %3d physPort = %3d ...",i,128+i);
        rc = cpssDxChPortIlknChannelSpeedSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E, speed2configure, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortIlknChannelSpeedGet(dev,128+i,CPSS_PORT_DIRECTION_TX_E, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (actualSpeed != speed2configure)
        {
            return GT_FAIL;
        }

        /*gtBobcat2PortPizzaArbiterIfStateDump(dev,0);*/

        rc = cpssDxChPortIlknChannelEnableSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E,GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
        osPrintf(" OK");
    }
    return GT_OK;
}




GT_STATUS paTest_EthTXFIFO_IlknTxFIFO_Reconfig
(
    /* GT_U8 dev */
)
{
    GT_STATUS rc;
    GT_U32 i;
    CPSS_PORTS_BMP_STC portsBmp;
    GT_U32             actualSpeed; 
    GT_U8 dev = 0;

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU)/sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]),
                                        &bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU)/sizeof(bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]),
                                     &bc2TmEnableMap_40G_TM_IlknChannel_CPU[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,48);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_SR_LR_E,CPSS_PORT_SPEED_40000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,128);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_ILKN4_E,CPSS_PORT_SPEED_20000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0; i < 5 ; i++)
    {
        rc = cpssDxChPortIlknChannelSpeedSet(dev,128,CPSS_PORT_DIRECTION_BOTH_E, 20000, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortIlknChannelEnableSet(dev,128,CPSS_PORT_DIRECTION_BOTH_E,GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
        /*gtBobcat2PortPizzaArbiterIfStateDump(dev,0);*/
    }
    osPrintf(" OK");
    return GT_OK;
}




/*------------------------------------------------------*/
/* just for test porpose */
static CPSS_DXCH_PORT_MAP_STC bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU[] =
{ /* Port,            mappingType                           portGroupm, intefaceNum, txQPort,          tmEnable,    tmPortInd*/
     {  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,      48,            GT_FALSE,        GT_NA }
    ,{ 128, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        0,      64,            GT_FALSE,        GT_NA } 
    ,{  63, CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,                   0,    GT_NA,      63,            GT_FALSE,        GT_NA } 
};


GT_STATUS paTest_IlknIFSingleChannelWoTM
(
    /* GT_U8 dev */
)
{
    GT_STATUS rc;
    CPSS_PORTS_BMP_STC portsBmp;
    GT_U8 dev = 0;
    GT_U32 speed2configureinMpbs, actualSpeed;

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }
    /*--------------------*/
    /* mapping            */
    /*--------------------*/
    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU)/sizeof(bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU[0]),
                                        &bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU)/sizeof(bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU[0]),
                                     &bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU)/sizeof(bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU[0]),
                                     &bc2TmEnableMap_40G_wo_TM_IlknPort_singleChannelWoTM_CPU[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    /*--------------------------*/
    /* port speed configuartion */
    /*--------------------------*/
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,48);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_SR_LR_E,CPSS_PORT_SPEED_40000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,128);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_ILKN8_E,CPSS_PORT_SPEED_40000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    speed2configureinMpbs = 40000;
    rc = cpssDxChPortIlknChannelSpeedSet(dev,128,CPSS_PORT_DIRECTION_BOTH_E, speed2configureinMpbs, &actualSpeed);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortIlknChannelEnableSet(dev,128,CPSS_PORT_DIRECTION_BOTH_E,GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    gtBobcat2PortMappingDump(dev);
    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);
    return GT_OK;
}




#if 0
GT_STATUS paTest_80GDev_ports68_40G
(
    /* GT_U8 dev */
) 
{ 
    GT_STATUS rc; 
    CPSS_PORTS_BMP_STC portsBmp; 
    GT_U8 dev = 0; 
    
    rc = appDemoDbEntryAdd("initSerdesDefaults", 0); 
    if (rc != GT_OK) 
    {
        return rc;
    }
    
    appDemoDebugDeviceIdSet(dev, CPSS_98DX4201_CNS); /* 0xFC0111AB */
    
    rc = cpssInitSystem(29, 1, 0); 
    if (rc != GT_OK) 
    {
        return rc;
    }
    
    /*--------------------------*/
    /* port speed configuartion */
    /*--------------------------*/
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp); 
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp, 68); 
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_40000_E); 
    if (rc != GT_OK) 
    {
        return rc;
    }
    
    
    gtBobcat2PortMappingDump(dev); 
    gtBobcat2PortPizzaArbiterIfStateDump(dev, 0); 
    return GT_OK;
}

GT_STATUS paTest_250MHz
(
    /* GT_U8 dev */
    ) 
{ 
    GT_STATUS rc; 
    CPSS_PORTS_BMP_STC portsBmp; 
    GT_U8 dev = 0; 
    GT_U32 regAddr; 
    GT_U32 fldOffs; 
    GT_U32 fldLen; 
    GT_U32 fldVal; 
    
    rc = appDemoDebugDeviceIdSet(dev, CPSS_98DX4201_CNS); 
    if (rc != GT_OK) 
    {
        return rc;
    }
    
    rc = appDemoDbEntryAdd("initSerdesDefaults", 0); 
    if (rc != GT_OK) 
    {
        return rc;
    }
    
    rc = cpssInitSystem(29, 1, 0); 
    if (rc != GT_OK) 
    {
        return rc;
    }
    
    regAddr = PRV_CPSS_DXCH_DEV_REGS_VER1_MAC(dev)->txDMA.txDMAPerSCDMAConfigs.SCDMAConfigs[0]; 
    fldOffs = 19; 
    fldLen  = 28 - 19 + 1; 
    fldVal  = 0; 
    
    rc = prvCpssHwPpPortGroupGetRegField(dev, 0, regAddr, fldOffs, fldLen, &fldVal); 
    if (GT_OK != rc) 
    {
        return rc;
    }
    if (fldVal != 0) 
    {
        return GT_FAIL;
    }
    
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp); 
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp, 0); 
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE, CPSS_PORT_INTERFACE_MODE_QSGMII_E, CPSS_PORT_SPEED_1000_E); 
    if (rc != GT_OK) 
    {
        return rc;
    }
    
    rc = prvCpssHwPpPortGroupGetRegField(dev, 0, regAddr, fldOffs, fldLen, &fldVal); 
    if (GT_OK != rc) 
    {
        return rc;
    }
    if (fldVal != 0) 
    {
        return GT_FAIL;
    }
    
    
    return GT_OK;
}
#endif


GT_STATUS paTest_Mac
(
    /* GT_U8 dev */
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortPreambleLengthSet(dev, 0, CPSS_PORT_DIRECTION_BOTH_E,1);
    if (rc == GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortPreambleLengthSet(dev, 0, CPSS_PORT_DIRECTION_BOTH_E,4);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortPreambleLengthSet(dev, 0, CPSS_PORT_DIRECTION_BOTH_E,8);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssDxChPortPreambleLengthSet(dev, 48, CPSS_PORT_DIRECTION_BOTH_E,1);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* test IPG */
    rc = cpssDxChPortIpgSet(dev, 0,  8);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortIpgSet(dev, 48, 16);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChCscdHyperGPortCrcModeSet(dev,0,CPSS_PORT_DIRECTION_BOTH_E,CPSS_DXCH_CSCD_PORT_CRC_ONE_BYTE_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChCscdHyperGPortCrcModeSet(dev,48,CPSS_PORT_DIRECTION_BOTH_E,CPSS_DXCH_CSCD_PORT_CRC_TWO_BYTES_E);
    if (rc != GT_OK)
    {
        return rc;
    }


    return GT_OK;
}



GT_STATUS paTest_29_1_12x10G_No_CPU
(
    /* GT_U8 dev */
)
{
    GT_STATUS rc=0;
    CPSS_PORTS_BMP_STC portsBmp;
    GT_U32 port;
    GT_U8 dev = 0;
    GT_32 ppm;

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);


    /*--------------------------*/
    /* port speed configuartion */
    /*--------------------------*/
    for (port = 48; port <= 59; port++)
    {
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,port);

        rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_SR_LR_E,CPSS_PORT_SPEED_10000_E);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    gtBobcat2PortMappingDump(dev);
    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);

    port = 56;
    ppm = -80;
    rc = cpssDxChPortSerdesPpmSet(dev,port,-80);
    if (rc != GT_OK)
    {
        return rc;
    }
    ppm = 10000;
    rc = cpssDxChPortSerdesPpmGet(dev,port,&ppm);
    if (rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("\nport = %2d ppm = %d",port,ppm);

    return GT_OK;
}

GT_STATUS paTest_29_1_30GWA_No_CPU
(
    GT_U32 implementWa,
    GT_U32 var
)
{
    GT_STATUS rc=0;
    CPSS_PORTS_BMP_STC portsBmp;
    GT_U32 portIdx;
    GT_U8 dev = 0;

    static char * speed2str[] = 
    {
        "10M  ",   /* 0 */
        "100M ",   /* 1 */
        "1G   ",   /* 2 */
        "10G  ",   /* 3 */
        "12G  ",   /* 4 */
        "2.5G ",   /* 5 */
        "5G   ",   /* 6 */
        "13.6G",   /* 7 */
        "20G  ",   /* 8 */
        "40G  ",   /* 9 */
        "16G  ",   /* 10 */
        "15G  ",   /* 11 */
        "75G  ",   /* 12 */
        "100G ",   /* 13 */
        "50G  ",   /* 14 */
        "140G "    /* 15 */
    };

    typedef struct 
    {
        GT_PHYSICAL_PORT_NUM         portNum;
        CPSS_PORT_SPEED_ENT          speed;
        CPSS_PORT_INTERFACE_MODE_ENT ifMode;
    }PORTINITLIST_STC;

    typedef struct 
    {
        GT_U32             var;
        PORTINITLIST_STC * listPtr;
    }PORTINITLIST_PER_VAR_STC;


    static PORTINITLIST_STC portInitList_3x40G[] = 
    { 
         { 48,  CPSS_PORT_SPEED_40000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 52,  CPSS_PORT_SPEED_40000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 56,  CPSS_PORT_SPEED_40000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ ~0,  CPSS_PORT_SPEED_NA_E,    CPSS_PORT_INTERFACE_MODE_NA_E    }
    };

    static PORTINITLIST_STC portInitList_1x40G_8x10G[] = 
    { 
         { 48,  CPSS_PORT_SPEED_40000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 50,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 51,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 52,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 53,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 54,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 55,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 56,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 57,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ ~0,  CPSS_PORT_SPEED_NA_E,    CPSS_PORT_INTERFACE_MODE_NA_E    }
    };

    static PORTINITLIST_STC portInitList_2x40G_4x10G[] = 
    { 
         { 48,  CPSS_PORT_SPEED_40000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 56,  CPSS_PORT_SPEED_40000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 50,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 51,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 52,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ 53,  CPSS_PORT_SPEED_10000_E, CPSS_PORT_INTERFACE_MODE_SR_LR_E }
        ,{ ~0,  CPSS_PORT_SPEED_NA_E,    CPSS_PORT_INTERFACE_MODE_NA_E    }
    };



    static PORTINITLIST_PER_VAR_STC portInitList[] = 
    {
         { 0, &portInitList_3x40G[0]             }
        ,{ 1, &portInitList_2x40G_4x10G[0]       }
        ,{ 2, &portInitList_1x40G_8x10G[0]       }
    };

    PORTINITLIST_STC * listPtr;

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("BC2_RevA0_40G_2_30GWA", implementWa);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);


    /*--------------------------*/
    /* port speed configuartion */
    /*--------------------------*/
    /* 48,52,56,60,64,68 */
    listPtr = portInitList[var].listPtr;

    for (portIdx = 0; listPtr[portIdx].portNum != ~0 ; portIdx++)
    {
        cpssOsPrintf("\nPort %2d : %s",listPtr[portIdx].portNum, speed2str[listPtr[portIdx].speed]);

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,listPtr[portIdx].portNum);

        rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,listPtr[portIdx].ifMode,listPtr[portIdx].speed);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    cpssOsPrintf("\n");

    gtBobcat2PortMappingDump(dev);
    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);

    return GT_OK;
}



#if 0
extern GT_VOID cpssDxChPortIlknChannelSpeedSetUT(GT_VOID);
extern GT_VOID cpssDxChPortIlknChannelSpeedResolutionSetUT(GT_VOID);


GT_STATUS paTest_IlknLR_SR_2_Ilkn
(
    /* GT_U8 dev */
)
{
    GT_STATUS rc;
    CPSS_PORTS_BMP_STC    portsBmp;
    GT_U32                portIdx;
    GT_PHYSICAL_PORT_NUM  portNum;
    GT_PHYSICAL_PORT_NUM  portNumList[] = {48, 49, 50, 51, 56, 58, 80};

 
    GT_U8 dev = 0;

    /* 
    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }
    */

    rc = cpssInitSystem(29,2,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*--------------------------*/
    /* port speed configuartion */
    /*--------------------------*/
    for (portIdx = 0; portIdx < sizeof(portNumList)/sizeof(portNumList[0]); portIdx++) 
    {
        portNum = portNumList[portIdx];
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,portNum);
        rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_SR_LR_E,CPSS_PORT_SPEED_10000_E);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    gtBobcat2PortPizzaArbiterIfUnitStateDump(dev,0,CPSS_DXCH_PA_UNIT_RXDMA_E);

    cpssDxChPortIlknChannelSpeedResolutionSetUT();

    gtBobcat2PortPizzaArbiterIfUnitStateDump(dev,0,CPSS_DXCH_PA_UNIT_RXDMA_E);

    cpssDxChPortIlknChannelSpeedSetUT();

    gtBobcat2PortPizzaArbiterIfUnitStateDump(dev,0,CPSS_DXCH_PA_UNIT_RXDMA_E);

    gtBobcat2PortMappingDump(dev);
    /* gtBobcat2PortPizzaArbiterIfStateDump(dev,0); */
    return GT_OK;
}

#endif


#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagPacketGenerator.h>
GT_STATUS prvCpssDxChMacTGDrvInit
(
    IN    GT_U8                   devNum
);

GT_STATUS prvCpssDxChMacTGDrvQeueryInit
(
    GT_U8 devNum
);



GT_STATUS macTG
(
    GT_U32               systemType,
    GT_PHYSICAL_PORT_NUM portNum
)
{
    GT_U32 portIdx;
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_BOOL                              connect;
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC config;
    CPSS_PORTS_BMP_STC portsBmp;

    if (portNum == 48)
    {
        rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    rc = cpssInitSystem(systemType,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (portNum == 48)
    {
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,portNum);
        rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_SR_LR_E,CPSS_PORT_SPEED_10000_E);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    for (portIdx = 0; portIdx < 4; portIdx++)
    {
        connect = GT_TRUE;
        config.macDa.arEther[0] = 0;
        config.macDa.arEther[1] = 0;
        config.macDa.arEther[2] = 0;
        config.macDa.arEther[3] = 0;
        config.macDa.arEther[4] = 0;
        config.macDa.arEther[5] = 2;
        config.macDaIncrementEnable = GT_TRUE;
        config.macDaIncrementLimit  = 1000;
        config.macSa.arEther[0] = 0;
        config.macSa.arEther[1] = 0;
        config.macSa.arEther[2] = 0;
        config.macSa.arEther[3] = 0;
        config.macSa.arEther[4] = 0;
        config.macSa.arEther[5] = 1;
        config.vlanTagEnable = GT_TRUE;
        config.vpt = 5;
        config.cfi = 1;
        config.vid = 2;
        config.etherType = 0x666;
        config.payloadType = CPSS_DIAG_PG_PACKET_PAYLOAD_RANDOM_E;
        config.packetLengthType = CPSS_DIAG_PG_PACKET_LENGTH_CONSTANT_E;
        config.packetLength     = 60;
        config.undersizeEnable  = GT_FALSE;
        config.transmitMode     = CPSS_DIAG_PG_TRANSMIT_SINGLE_BURST_E;
        config.packetCount      = 10;
        config.packetCountMultiplier  = CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_1_E;
        config.ipg              = 12;
        config.interfaceSize    = CPSS_DIAG_PG_IF_SIZE_DEFAULT_E;
        /* GT_U8                                    cyclicPatternArr[8]; */
        rc = cpssDxChDiagPacketGeneratorConnectSet(0,portNum+portIdx*4,connect, &config);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* 
         1G  GE   8Byte
        10G  XLG  
        port 48 --> 10G -- 8  bytes 
                    20G -- 8  bytes
                    40G -- 32 Bytes
        */
        /* 
        rc = cpssDxChDiagPacketGeneratorTransmitEnable(0,portNum+portIdx*4,GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
        */
    }
    return GT_OK;
}

#ifdef WIN32
    #define cpssOsPrintf osPrintf
#endif

GT_STATUS jiraCPSS_1395_ilkn40Gto20G
(
    /* GT_U8 dev */
)
{
    GT_STATUS rc;
    GT_U32 i;
    CPSS_PORTS_BMP_STC portsBmp;
    GT_U32             actualSpeed;
    GT_U32             speed2configure;
    GT_U32             estIlknIFBW;
    GT_U8 dev = 0;

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,2,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\n+-------------------------------");
    cpssOsPrintf("\n| Power Up 40G ILKN8 IF         ");
    cpssOsPrintf("\n+-------------------------------");

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,128);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_ILKN8_E,CPSS_PORT_SPEED_40000_E);
    if (rc != GT_OK)
    {
        return rc;
    }


    cpssOsPrintf("\n+-------------------------------");
    cpssOsPrintf("\n| Configure 4x10G Channels      ");
    cpssOsPrintf("\n+-------------------------------");
    speed2configure = 10000;
    rc = cpssDxChPortIlknChannelSpeedResolutionSet(dev,speed2configure,/*OUT*/&estIlknIFBW);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0 ; i < 4 ; i++)
    {
        cpssOsPrintf("\n  Channel %3d physPort = %3d ...",i,128+i);
        rc = cpssDxChPortIlknChannelSpeedSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E, speed2configure, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortIlknChannelSpeedGet(dev,128+i,CPSS_PORT_DIRECTION_TX_E, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (actualSpeed != speed2configure)
        {
            return GT_FAIL;
        }

        /*gtBobcat2PortPizzaArbiterIfStateDump(dev,0);*/

        rc = cpssDxChPortIlknChannelEnableSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E,GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf(" OK");
    }

    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);

    cpssOsPrintf("\n+-------------------------------");
    cpssOsPrintf("\n| Disable 4x10G Channels        ");
    cpssOsPrintf("\n+-------------------------------");
    speed2configure = 0;
    for (i = 0 ; i < 4 ; i++)
    {
        cpssOsPrintf("\n  Channel %3d physPort = %3d ...",i,128+i);
        actualSpeed = 0;
        rc = cpssDxChPortIlknChannelEnableSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E,GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortIlknChannelSpeedSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E, speed2configure, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortIlknChannelSpeedGet(dev,128+i,CPSS_PORT_DIRECTION_TX_E, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (actualSpeed != speed2configure)
        {
            return GT_FAIL;
        }

        /*gtBobcat2PortPizzaArbiterIfStateDump(dev,0);*/

        cpssOsPrintf(" OK");
    }
    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);

    cpssOsPrintf("\n+-------------------------------");
    cpssOsPrintf("\n| Power Down/Up 20G ILKN4 IF    ");
    cpssOsPrintf("\n+-------------------------------");
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,128);
    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_FALSE,CPSS_PORT_INTERFACE_MODE_ILKN8_E,CPSS_PORT_SPEED_40000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_ILKN4_E,CPSS_PORT_SPEED_20000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\n+-------------------------------");
    cpssOsPrintf("\n| Configure 4x5G Channels       ");
    cpssOsPrintf("\n+-------------------------------");
    speed2configure = 5000;
    rc = cpssDxChPortIlknChannelSpeedResolutionSet(dev,speed2configure,/*OUT*/&estIlknIFBW);
    if (rc != GT_OK)
    {
        return rc;
    }
    for (i = 0 ; i < 4 ; i++)
    {
        actualSpeed = 0;
        cpssOsPrintf("\n  Channel %3d physPort = %3d ...",i,128+i);
        rc = cpssDxChPortIlknChannelEnableSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E,GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortIlknChannelSpeedSet(dev,128+i,CPSS_PORT_DIRECTION_BOTH_E, speed2configure, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortIlknChannelSpeedGet(dev,128+i,CPSS_PORT_DIRECTION_TX_E, &actualSpeed);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (actualSpeed != speed2configure)
        {
            return GT_FAIL;
        }

        cpssOsPrintf(" OK");
    }

    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);
    return GT_OK;
}









GT_STATUS initSys29_2
(
)
{
    static CPSS_DXCH_PORT_MAP_STC bc2TmEnableMap[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort,        tmEnable,      tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        0,     GT_NA,         GT_TRUE,            0 }
        ,{   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        1,     GT_NA,         GT_TRUE,            1 }
        ,{   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        2,     GT_NA,         GT_TRUE,            2 }
        ,{   3, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        3,     GT_NA,         GT_TRUE,            3 }
        ,{   4, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        4,     GT_NA,         GT_TRUE,            4 }
        ,{   5, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        5,     GT_NA,         GT_TRUE,            5 }
        ,{   6, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        6,     GT_NA,         GT_TRUE,            6 }
        ,{   7, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        7,     GT_NA,         GT_TRUE,            7 }
        ,{   8, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        8,     GT_NA,         GT_TRUE,            8 }
        ,{   9, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        9,     GT_NA,         GT_TRUE,            9 }
        ,{  10, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       10,     GT_NA,         GT_TRUE,           10 }
        ,{  11, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       11,     GT_NA,         GT_TRUE,           11 }
        ,{  12, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       12,     GT_NA,         GT_TRUE,           12 }
        ,{  13, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       13,     GT_NA,         GT_TRUE,           13 }
        ,{  14, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       14,     GT_NA,         GT_TRUE,           14 }
        ,{  15, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       15,     GT_NA,         GT_TRUE,           15 }
        ,{  16, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       16,     GT_NA,         GT_TRUE,           16 }
        ,{  17, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       17,     GT_NA,         GT_TRUE,           17 }
        ,{  18, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       18,     GT_NA,         GT_TRUE,           18 }
        ,{  19, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       19,     GT_NA,         GT_TRUE,           19 }
        ,{  20, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       20,     GT_NA,         GT_TRUE,           20 }
        ,{  21, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       21,     GT_NA,         GT_TRUE,           21 }
        ,{  22, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       22,     GT_NA,         GT_TRUE,           22 }
        ,{  23, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       23,     GT_NA,         GT_TRUE,           23 }
        ,{  24, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       24,     GT_NA,         GT_TRUE,           24 }
        ,{  25, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       25,     GT_NA,         GT_TRUE,           25 }
        ,{  26, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       26,     GT_NA,         GT_TRUE,           26 }
        ,{  27, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       27,     GT_NA,         GT_TRUE,           27 }
        ,{  28, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       28,     GT_NA,         GT_TRUE,           28 }
        ,{  29, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       29,     GT_NA,         GT_TRUE,           29 }
        ,{  30, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       30,     GT_NA,         GT_TRUE,           30 }
        ,{  31, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       31,     GT_NA,         GT_TRUE,           31 }
        ,{  32, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       32,     GT_NA,         GT_TRUE,           32 }
        ,{  33, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       33,     GT_NA,         GT_TRUE,           33 }
        ,{  34, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       34,     GT_NA,         GT_TRUE,           34 }
        ,{  35, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       35,     GT_NA,         GT_TRUE,           35 }
        ,{  36, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       36,     GT_NA,         GT_TRUE,           36 }
        ,{  37, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       37,     GT_NA,         GT_TRUE,           37 }
        ,{  38, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       38,     GT_NA,         GT_TRUE,           38 }
        ,{  39, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       39,     GT_NA,         GT_TRUE,           39 }
        ,{  40, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       40,     GT_NA,         GT_TRUE,           40 }
        ,{  41, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       41,     GT_NA,         GT_TRUE,           41 }
        ,{  42, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       42,     GT_NA,         GT_TRUE,           42 }
        ,{  43, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       43,     GT_NA,         GT_TRUE,           43 }
        ,{  44, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       44,     GT_NA,         GT_TRUE,           44 }
        ,{  45, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       45,     GT_NA,         GT_TRUE,           45 }
        ,{  46, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       46,     GT_NA,         GT_TRUE,           46 }
        ,{  47, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       47,     GT_NA,         GT_TRUE,           47 }
        ,{  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,        48,        GT_FALSE,        GT_NA }
        ,{  49, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       49,        49,        GT_FALSE,        GT_NA }
        ,{  50, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       50,        50,        GT_FALSE,        GT_NA }
        ,{  51, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       51,        51,        GT_FALSE,        GT_NA }
        ,{  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       56,        56,        GT_FALSE,        GT_NA }
        ,{  58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       58,        58,        GT_FALSE,        GT_NA }
        ,{  80, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       60,        60,        GT_FALSE,        GT_NA }
        ,{  82, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       62,        62,        GT_FALSE,        GT_NA }
        ,{  63, CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,                   0,    GT_NA,        63,        GT_FALSE,        GT_NA }
      /* Port,            mappingType                           portGroupm, intefaceNum, txQPort,        tmEnable,      tmPortInd    */
      /* Port 128 + i  <---> channel i  0 <= i <= 63   tm  128+i                                                                     */
        ,{ 128, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        0,     GT_NA,         GT_TRUE,          128 }
        ,{ 129, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        1,     GT_NA,         GT_TRUE,          129 }
        ,{ 130, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        2,     GT_NA,         GT_TRUE,          130 }
        ,{ 131, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        3,     GT_NA,         GT_TRUE,          131 }
        ,{ 132, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        4,     GT_NA,         GT_TRUE,          132 }
        ,{ 133, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        5,     GT_NA,         GT_TRUE,          133 }
        ,{ 134, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        6,     GT_NA,         GT_TRUE,          134 }
        ,{ 135, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        7,     GT_NA,         GT_TRUE,          135 }
        ,{ 136, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        8,     GT_NA,         GT_TRUE,          136 }
        ,{ 137, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,        9,     GT_NA,         GT_TRUE,          137 }
        ,{ 138, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       10,     GT_NA,         GT_TRUE,          138 }
        ,{ 139, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       11,     GT_NA,         GT_TRUE,          139 }
        ,{ 140, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       12,     GT_NA,         GT_TRUE,          140 }
        ,{ 141, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       13,     GT_NA,         GT_TRUE,          141 }
        ,{ 142, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       14,     GT_NA,         GT_TRUE,          142 }
        ,{ 143, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       15,     GT_NA,         GT_TRUE,          143 }
        ,{ 144, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       16,     GT_NA,         GT_TRUE,          144 }
        ,{ 145, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       17,     GT_NA,         GT_TRUE,          145 }
        ,{ 146, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       18,     GT_NA,         GT_TRUE,          146 }
        ,{ 147, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       19,     GT_NA,         GT_TRUE,          147 }
        ,{ 148, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       20,     GT_NA,         GT_TRUE,          148 }
        ,{ 149, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       21,     GT_NA,         GT_TRUE,          149 }
        ,{ 150, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       22,     GT_NA,         GT_TRUE,          150 }
        ,{ 151, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       23,     GT_NA,         GT_TRUE,          151 }
        ,{ 152, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       24,     GT_NA,         GT_TRUE,          152 }
        ,{ 153, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       25,     GT_NA,         GT_TRUE,          153 }
        ,{ 154, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       26,     GT_NA,         GT_TRUE,          154 }
        ,{ 155, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       27,     GT_NA,         GT_TRUE,          155 }
        ,{ 156, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       28,     GT_NA,         GT_TRUE,          156 }
        ,{ 157, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       29,     GT_NA,         GT_TRUE,          157 }
        ,{ 158, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       30,     GT_NA,         GT_TRUE,          158 }
        ,{ 159, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       31,     GT_NA,         GT_TRUE,          159 }
        ,{ 160, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       32,     GT_NA,         GT_TRUE,          160 }
        ,{ 161, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       33,     GT_NA,         GT_TRUE,          161 }
        ,{ 162, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       34,     GT_NA,         GT_TRUE,          162 }
        ,{ 163, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       35,     GT_NA,         GT_TRUE,          163 }
        ,{ 164, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       36,     GT_NA,         GT_TRUE,          164 }
        ,{ 165, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       37,     GT_NA,         GT_TRUE,          165 }
        ,{ 166, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       38,     GT_NA,         GT_TRUE,          166 }
        ,{ 167, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       39,     GT_NA,         GT_TRUE,          167 }
        ,{ 168, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       40,     GT_NA,         GT_TRUE,          168 }
        ,{ 169, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       41,     GT_NA,         GT_TRUE,          169 }
        ,{ 170, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       42,     GT_NA,         GT_TRUE,          170 }
        ,{ 171, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       43,     GT_NA,         GT_TRUE,          171 }
        ,{ 172, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       44,     GT_NA,         GT_TRUE,          172 }
        ,{ 173, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       45,     GT_NA,         GT_TRUE,          173 }
        ,{ 174, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       46,     GT_NA,         GT_TRUE,          174 }
        ,{ 175, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       47,     GT_NA,         GT_TRUE,          175 }
        ,{ 176, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       48,     GT_NA,         GT_TRUE,          176 }
        ,{ 177, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       49,     GT_NA,         GT_TRUE,          177 }
        ,{ 178, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       50,     GT_NA,         GT_TRUE,          178 }
        ,{ 179, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       51,     GT_NA,         GT_TRUE,          179 }
        ,{ 180, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       52,     GT_NA,         GT_TRUE,          180 }
        ,{ 181, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       53,     GT_NA,         GT_TRUE,          181 }
        ,{ 182, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       54,     GT_NA,         GT_TRUE,          182 }
        ,{ 183, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       55,     GT_NA,         GT_TRUE,          183 }
        ,{ 184, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       56,     GT_NA,         GT_TRUE,          184 }
        ,{ 185, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       57,     GT_NA,         GT_TRUE,          185 }
        ,{ 186, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       58,     GT_NA,         GT_TRUE,          186 }
        ,{ 187, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       59,     GT_NA,         GT_TRUE,          187 }
        ,{ 188, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       60,     GT_NA,         GT_TRUE,          188 }
        ,{ 189, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       61,     GT_NA,         GT_TRUE,          189 }
        ,{ 190, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       62,     GT_NA,         GT_TRUE,          190 }
        ,{ 191, CPSS_DXCH_PORT_MAPPING_TYPE_ILKN_CHANNEL_E,               0,       63,     GT_NA,         GT_TRUE,          191 }
    };

    GT_STATUS rc=0;
    GT_U8 dev = 0;
    CPSS_PORTS_BMP_STC portsBmp;

#if 0
    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }
#endif

    rc = cpssInitSystem(29,2,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    gtBobcat2PortMappingDump(dev);              

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,0);

    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_FALSE,CPSS_PORT_INTERFACE_MODE_QSGMII_E,CPSS_PORT_SPEED_1000_E);


    gtBobcat2PortPizzaArbiterIfStateDump(dev,0); 

    bc2TmEnableMap[0].physicalPortNumber = 0;
    bc2TmEnableMap[0].mappingType        = CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E;
    bc2TmEnableMap[0].portGroup          = 0;
    bc2TmEnableMap[0].interfaceNum       = 0;
    bc2TmEnableMap[0].txqPortNumber      = 0;
    bc2TmEnableMap[0].tmEnable           = GT_FALSE;
    bc2TmEnableMap[0].tmPortInd          = GT_NA;                         


    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2TmEnableMap)/sizeof(bc2TmEnableMap[0]),
                                        &bc2TmEnableMap[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2TmEnableMap)/sizeof(bc2TmEnableMap[0]),
                                     &bc2TmEnableMap[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }


    gtBobcat2PortMappingDump(dev);              

    rc = cpssDxChPortModeSpeedSet(dev, portsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_QSGMII_E,CPSS_PORT_SPEED_1000_E);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

#define RUN_PA_UT    0
#if RUN_PA_UT == 1

    #include <utf/private/prvUtfExtras.h>
    extern GT_VOID utPADebugPrintSet(GT_VOID);
    extern GT_VOID utPADebugVarNSet(GT_U32 varN);

    GT_STATUS paTestUT_29_1
    (
        /* GT_U8 dev */
        GT_U32 numberOfVar
    )
    {
        extern GT_VOID cpssDxChBobcat2PortModeSpeedSetUT(GT_VOID);

        GT_STATUS rc;

        if (numberOfVar == 0)
        {
            numberOfVar = 10000;
        }
        utPADebugPrintSet(); 
        utPADebugVarNSet(numberOfVar); 

        prvUtfBaselineTestsExecutionFlagSet(GT_TRUE);
        rc = cpssInitSystem(29,1,0);
        if (rc != GT_OK)
        {
            return rc;
        }
        cpssDxChBobcat2PortModeSpeedSetUT();
        return GT_OK;
    }

#endif


#define RUN_SMI_TEST 0
#if RUN_SMI_TEST == 1

    extern GT_VOID cpssDxChPhyAutoPollNumOfPortsSetUT(GT_VOID);
    extern GT_VOID cpssDxChPhyAutoPollNumOfPortsGetUT(GT_VOID);
    extern GT_VOID cpssDxChPhyPortAddrSetUT(GT_VOID);
    extern GT_VOID cpssDxChPhyPortAddrGetUT(GT_VOID);
    extern GT_VOID cpssDxChPhyPortSmiRegisterReadUT(GT_VOID);
    extern GT_VOID cpssDxChPhyPortSmiRegisterWriteUT(GT_VOID);
    extern GT_VOID cpssDxChPhyAutonegSmiSetUT(GT_VOID);
    extern GT_VOID cpssDxChPhyAutonegSmiGetUT(GT_VOID);
    extern GT_VOID cpssDxChPhySmiAutoMediaSelectSetUT(GT_VOID);
    extern GT_VOID cpssDxChPhySmiAutoMediaSelectGetUT(GT_VOID);

    GT_STATUS smiTest
    (
        GT_U32 sys
    )
    {
        GT_STATUS rc;
        if (sys >= 2)
        {
            cpssOsPrintf("\nERROR :  unknown system %d",sys);
            return GT_BAD_PARAM;
        }
        rc = cpssInitSystem(29,sys,0);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPhySmiAutoMediaSelectSet(0,24,1);
        if (rc != GT_OK)
        {
            return rc;
        }

        cpssDxChPhyAutoPollNumOfPortsSetUT();
        cpssDxChPhyAutoPollNumOfPortsGetUT();
        cpssDxChPhyPortAddrSetUT();
        cpssDxChPhyPortAddrGetUT();
        cpssDxChPhyPortSmiRegisterReadUT();
        cpssDxChPhyPortSmiRegisterWriteUT();
        cpssDxChPhyAutonegSmiSetUT();
        cpssDxChPhyAutonegSmiGetUT();
        cpssDxChPhySmiAutoMediaSelectSetUT();
        cpssDxChPhySmiAutoMediaSelectGetUT();
        return GT_OK;
    }

#endif



static char * str_speedRes[] = 
{
     "INVALID"
    ,"500"
    ,"1000"
    ,"2000"
    ,"5000"
    ,"10000"
};


GT_STATUS pa240G_40x1G_12x10G_2x40G
(
    GT_U32 workConsStatus
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32                  unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes;
    GT_U32 txqPort;
    GT_BOOL status;

    static PortInitList_STC portInitlist_240G_40x1G_12x10G_2x40G[] = 
    {
         { PORT_LIST_TYPE_LIST,      { 64,68,       APP_INV_PORT_CNS }, CPSS_PORT_SPEED_40000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 48,59,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  {  0,39,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    gtBobcat2PortPizzaArbiterIfStateDump(dev,0);



    unitBWGbps = 240;
    speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

    rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nunit BW configuration:");
    cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
    cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);
    rc = appDemoBc2PortListInit(dev,&portInitlist_240G_40x1G_12x10G_2x40G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }


    for (txqPort = 0 ; txqPort < PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS; txqPort++)
    {

        rc = prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeGet(dev,txqPort,&status);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (status != GT_FALSE)
        {
            return GT_BAD_PARAM;
        }
    }

    cpssOsPrintf("\nwork conserving mode config");
    rc = cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet(dev,CPSS_DXCH_PA_UNIT_TXQ_E,GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nwork conserving mode check");
    for (txqPort = 0 ; txqPort < PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS; txqPort++)
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeGet(dev,txqPort,&status);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (status != GT_TRUE)
        {
            return GT_BAD_PARAM;
        }
    }
    return GT_OK;
}



GT_STATUS pa240G_40x1G_12x10G_TM_8x10G
(
    GT_U32 workConsStatus
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32                  unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes;
    GT_U32 txqPort;
    GT_BOOL status;

    static CPSS_DXCH_PORT_MAP_STC bc2Map[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort, trafficManegerEnable , tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        0,       0,           GT_FALSE,          GT_NA}
        ,{   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        1,       1,           GT_FALSE,          GT_NA}
        ,{   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        2,       2,           GT_FALSE,          GT_NA}
        ,{   3, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        3,       3,           GT_FALSE,          GT_NA}
        ,{   4, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        4,       4,           GT_FALSE,          GT_NA}
        ,{   5, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        5,       5,           GT_FALSE,          GT_NA}
        ,{   6, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        6,       6,           GT_FALSE,          GT_NA}
        ,{   7, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        7,       7,           GT_FALSE,          GT_NA}
        ,{   8, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        8,       8,           GT_FALSE,          GT_NA}
        ,{   9, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        9,       9,           GT_FALSE,          GT_NA}
        ,{  10, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       10,      10,           GT_FALSE,          GT_NA}
        ,{  11, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       11,      11,           GT_FALSE,          GT_NA}
        ,{  12, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       12,      12,           GT_FALSE,          GT_NA}
        ,{  13, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       13,      13,           GT_FALSE,          GT_NA}
        ,{  14, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       14,      14,           GT_FALSE,          GT_NA}
        ,{  15, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       15,      15,           GT_FALSE,          GT_NA}
        ,{  16, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       16,      16,           GT_FALSE,          GT_NA}
        ,{  17, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       17,      17,           GT_FALSE,          GT_NA}
        ,{  18, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       18,      18,           GT_FALSE,          GT_NA}
        ,{  19, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       19,      19,           GT_FALSE,          GT_NA}
        ,{  20, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       20,      20,           GT_FALSE,          GT_NA}
        ,{  21, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       21,      21,           GT_FALSE,          GT_NA}
        ,{  22, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       22,      22,           GT_FALSE,          GT_NA}
        ,{  23, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       23,      23,           GT_FALSE,          GT_NA}
        ,{  24, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       24,      24,           GT_FALSE,          GT_NA}
        ,{  25, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       25,      25,           GT_FALSE,          GT_NA}
        ,{  26, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       26,      26,           GT_FALSE,          GT_NA}
        ,{  27, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       27,      27,           GT_FALSE,          GT_NA}
        ,{  28, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       28,      28,           GT_FALSE,          GT_NA}
        ,{  29, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       29,      29,           GT_FALSE,          GT_NA}
        ,{  30, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       30,      30,           GT_FALSE,          GT_NA}
        ,{  31, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       31,      31,           GT_FALSE,          GT_NA}
        ,{  32, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       32,      32,           GT_FALSE,          GT_NA}
        ,{  33, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       33,      33,           GT_FALSE,          GT_NA}
        ,{  34, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       34,      34,           GT_FALSE,          GT_NA}
        ,{  35, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       35,      35,           GT_FALSE,          GT_NA}
        ,{  36, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       36,      36,           GT_FALSE,          GT_NA}
        ,{  37, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       37,      37,           GT_FALSE,          GT_NA}
        ,{  38, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       38,      38,           GT_FALSE,          GT_NA}
        ,{  39, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       39,      39,           GT_FALSE,          GT_NA}
        ,{  40, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       40,      40,           GT_FALSE,          GT_NA}
        ,{  41, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       41,      41,           GT_FALSE,          GT_NA}
        ,{  42, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       42,      42,           GT_FALSE,          GT_NA}
        ,{  43, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       43,      43,           GT_FALSE,          GT_NA}
        ,{  44, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       44,      44,           GT_FALSE,          GT_NA}
        ,{  45, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       45,      45,           GT_FALSE,          GT_NA}
        ,{  46, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       46,      46,           GT_FALSE,          GT_NA}
        ,{  47, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       47,      47,           GT_FALSE,          GT_NA}
        ,{  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,      48,           GT_FALSE,          GT_NA}
        ,{  49, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       49,   GT_NA,            GT_TRUE,             49}
        ,{  50, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       50,   GT_NA,            GT_TRUE,             50}
        ,{  51, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       51,   GT_NA,            GT_TRUE,             51}
        ,{  52, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       52,   GT_NA,            GT_TRUE,             52}
        ,{  53, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       53,   GT_NA,            GT_TRUE,             53}
        ,{  54, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       54,   GT_NA,            GT_TRUE,             54}
        ,{  55, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       55,   GT_NA,            GT_TRUE,             55}
        ,{  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       56,   GT_NA,            GT_TRUE,             56}
        ,{  57, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       57,   GT_NA,            GT_TRUE,             57}
        ,{  58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       58,   GT_NA,            GT_TRUE,             58}
        ,{  59, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       59,   GT_NA,            GT_TRUE,             59}
        ,{  80, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       60,      60,           GT_FALSE,          GT_NA}
        ,{  81, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       61,      61,           GT_FALSE,          GT_NA} /* virtual router port */
        ,{  82, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       62,      62,           GT_FALSE,          GT_NA}
        ,{  64, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       64,   GT_NA,            GT_TRUE,             64}
        ,{  65, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       65,      65,           GT_FALSE,          GT_NA}
        ,{  66, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       66,      66,           GT_FALSE,          GT_NA}
        ,{  67, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       67,      67,           GT_FALSE,          GT_NA}
        ,{  68, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       68,      68,           GT_FALSE,          GT_NA}
        ,{  69, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       69,      69,           GT_FALSE,          GT_NA}
        ,{  70, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       70,      70,           GT_FALSE,          GT_NA}
        ,{  71, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       71,      71,           GT_FALSE,          GT_NA}
    };

    static PortInitList_STC portInitlist_240G_40x1G_12x10G_TM_8x40G[] = 
    {
         { PORT_LIST_TYPE_INTERVAL,  { 64,71,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 48,59,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 0,39,1,      APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };


    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2Map)/sizeof(bc2Map[0]),
                                        &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2Map)/sizeof(bc2Map[0]),
                                     &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    unitBWGbps = 240;
    speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

    rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nunit BW configuration:");
    cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
    cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);
    rc = appDemoBc2PortListInit(dev,&portInitlist_240G_40x1G_12x10G_TM_8x40G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nwork conserving mode config");
    rc = cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet(dev,CPSS_DXCH_PA_UNIT_TXQ_E,GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nwork conserving mode check");
    for (txqPort = 0 ; txqPort < PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS; txqPort++)
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeGet(dev,txqPort,&status);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (status != GT_TRUE)
        {
            return GT_BAD_PARAM;
        }
    }
    return GT_OK;
}






GT_STATUS pa_ALU_4x40G_1x1G_CPU
(
    GT_VOID
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;

    static CPSS_DXCH_PORT_MAP_STC bc2Map[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort, trafficManegerEnable , tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,      48,           GT_FALSE,          GT_NA} /* 40G */
        ,{   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       52,      52,           GT_FALSE,          GT_NA} /* 40G */
        ,{  50, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       60,      60,           GT_FALSE,          GT_NA} /* 40G */
        ,{  51, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       64,      64,           GT_FALSE,          GT_NA} /* 40G */
        ,{  71, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       71,      71,           GT_FALSE,          GT_NA} /*  1G */
        ,{  63, CPSS_DXCH_PORT_MAPPING_TYPE_CPU_SDMA_E,                   0,    GT_NA,      63,           GT_FALSE,          GT_NA}
    };

    static PortInitList_STC portInitlist_4x40G_1x1G[] = 
    {
         { PORT_LIST_TYPE_LIST,      {  0, 1, 50, 51,  APP_INV_PORT_CNS }, CPSS_PORT_SPEED_40000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 71,             APP_INV_PORT_CNS },  CPSS_PORT_SPEED_1000_E,  CPSS_PORT_INTERFACE_MODE_1000BASE_X_E }
        ,{ PORT_LIST_TYPE_EMPTY,     {                 APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssDxChPortPhysicalPortMapSet(dev, 
                                        sizeof(bc2Map)/sizeof(bc2Map[0]),
                                        &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2Map)/sizeof(bc2Map[0]),
                                     &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoBc2PortListInit(dev,&portInitlist_4x40G_1x1G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS paALU_168G_2x40G_2x20G_48x1G
(
    GT_U32 workConsStatus
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32                  unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes;


    static PortInitList_STC portInitlist_168G_2x40G_2x20G_48x1G[] = 
    {
         { PORT_LIST_TYPE_INTERVAL,  {  0,47,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_LIST,      { 80,64,       APP_INV_PORT_CNS }, CPSS_PORT_SPEED_40000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 66,68,       APP_INV_PORT_CNS }, CPSS_PORT_SPEED_20000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    unitBWGbps = 168;
    speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

    rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nunit BW configuration:");
    cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
    cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);
    rc = appDemoBc2PortListInit(dev,&portInitlist_168G_2x40G_2x20G_48x1G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


GT_STATUS pa175Mhz_TM
(
    GT_VOID
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;


    static CPSS_DXCH_PORT_MAP_STC bc2Map[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort, trafficManegerEnable , tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        0,       0,            GT_TRUE,              1}
        ,{   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        1,       1,           GT_FALSE,          GT_NA}
        ,{   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        2,       2,           GT_FALSE,          GT_NA}
        ,{   3, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        3,       3,           GT_FALSE,          GT_NA}
        ,{  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       56,   GT_NA,            GT_TRUE,             56}
        ,{  57, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       57,   GT_NA,            GT_TRUE,             57}
        ,{  58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       58,   GT_NA,            GT_TRUE,             58}
        ,{  59, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       59,   GT_NA,            GT_TRUE,             59}
    };

    static PortInitList_STC portInitlist_4x10G_4x1G[] = 
    {
         { PORT_LIST_TYPE_LIST,      {  0,  1,  2,  3,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_LIST,      { 56, 57, 58, 59,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {                   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };


    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    #ifdef ASIC_SIMULATION
        appDemoBcat2B0SimCoreClockSet(dev,175); 
        /* simCoreClockOverwrite(175); */
    #endif

    rc = cpssInitSystem(29,2,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2Map)/sizeof(bc2Map[0]),
                                        &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoBc2PortListInit(dev,&portInitlist_4x10G_4x1G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoBobcat2PortListResourcesPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}



GT_STATUS pa175Mhz_TM_4x10G_TM_4x10G_4x1G
(
    GT_VOID
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32 unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes;


    static CPSS_DXCH_PORT_MAP_STC bc2Map[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort, trafficManegerEnable , tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        0,       0,           GT_FALSE,          GT_NA}
        ,{   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        1,       1,           GT_FALSE,          GT_NA}
        ,{   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        2,       2,           GT_FALSE,          GT_NA}
        ,{   3, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        3,       3,           GT_FALSE,          GT_NA}
        ,{  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       56,   GT_NA,            GT_TRUE,             56}
        ,{  57, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       57,   GT_NA,            GT_TRUE,             57}
        ,{  58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       58,   GT_NA,            GT_TRUE,             58}
        ,{  59, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       59,   GT_NA,            GT_TRUE,             59}
        ,{  80, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       60,   GT_NA,            GT_TRUE,             60}
        ,{  81, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       61,      61,           GT_FALSE,          GT_NA}
        ,{  82, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       62,      62,           GT_FALSE,          GT_NA}
        ,{  83, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       63,      63,           GT_FALSE,          GT_NA}
    };

    static PortInitList_STC portInitlist_8x10G_4x1G[] = 
    {
         { PORT_LIST_TYPE_LIST,      {  0,  1,  2,  3,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_LIST,      { 80, 81, 82, 83,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 56, 57, 58, 59,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {                   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };


    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    #ifdef ASIC_SIMULATION
        simCoreClockOverwrite(175);
    #endif

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2Map)/sizeof(bc2Map[0]),
                                        &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2Map)/sizeof(bc2Map[0]),
                                     &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    unitBWGbps = 84;
    speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

    rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDxChPortDynamicPizzaArbiterIfUnitBWSet(dev, CPSS_DXCH_PA_UNIT_ETH_TX_FIFO_E, unitBWGbps);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet(dev,CPSS_DXCH_PA_UNIT_TXQ_E,GT_TRUE);
    if (GT_OK != rc)
    {
        return rc;
    }

    prvCpssDxChPortBcat2CreditsCheckSet(GT_FALSE);

    cpssOsPrintf("\nunit BW configuration:");
    cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
    cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);
    rc = appDemoBc2PortListInit(dev,&portInitlist_8x10G_4x1G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


extern GT_STATUS appDemoBc2PortListDelete
(
    IN GT_U8 dev,
    IN PortInitList_STC * portInitList
);

GT_STATUS pa175Mhz_84Gpbs(GT_VOID)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32 unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes;
    GT_U32  i;


    static PortInitList_STC portInitlist_8x10G_4x1G[] = 
    {
         { PORT_LIST_TYPE_LIST,      {  0, 1,   2,  3,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_LIST,      { 52, 53, 54, 56,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 57, 58, 59, 60,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {                   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    static PortInitList_STC portInitlist_2x40G_4x1G[] = 
    {
         { PORT_LIST_TYPE_LIST,      {  0, 1,   2,  3,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_LIST,      { 52, 56,           APP_INV_PORT_CNS }, CPSS_PORT_SPEED_40000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {                   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    static PortInitList_STC portInitlist_1x40G_4x10G_2x1G[] = 
    {
         { PORT_LIST_TYPE_LIST,      {  0, 1,   2,  3,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_LIST,      { 52,               APP_INV_PORT_CNS }, CPSS_PORT_SPEED_40000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 57, 58, 59, 60,   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {                   APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    typedef struct PortInitListStr_STC
    {
        PortInitList_STC * portInitlistPtr;
        GT_CHAR          * namePtr;
    }PortInitListStr_STC;

    PortInitListStr_STC portInitlistArr[] = 
    {
         { &portInitlist_8x10G_4x1G[0],        "8x10G_4x1G"            }
        ,{ &portInitlist_2x40G_4x1G[0],        "2x40G_4x1G"            }
        ,{ &portInitlist_1x40G_4x10G_2x1G[0],  "1x40G_4x10G_2x1G"      }
        ,{ (PortInitList_STC *)NULL         ,   (GT_CHAR *)NULL        }
    };

    typedef struct HighSpeedPortList_STC
    {
        GT_U32 size;
        GT_U32 list[8];
    }HighSpeedPortList_STC;

    HighSpeedPortList_STC highSpeedPortListConfArr[] = 
    {
         { 2, { 1, 2   } }
        ,{ 3, { 1, 2, 3} }   
        ,{ 3, { 2, 3, 4} }   /* delete 1, add 4 */
        ,{ 0, { 0      } }
    };

    HighSpeedPortList_STC getList;

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    #ifdef ASIC_SIMULATION
        simCoreClockOverwrite(175);
    #endif

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }

    for (i = 0; i < sizeof(highSpeedPortListConfArr)/sizeof(highSpeedPortListConfArr[0]); i++)
    {
        GT_U32 dqList[8];
        GT_U32 llList[8];
        rc = prvCpssDxChPortDynamicPATxQHighSpeedPortSet(dev,highSpeedPortListConfArr[i].size,&highSpeedPortListConfArr[i].list[0]);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortDynamicPATxQHighSpeedPortGet(dev,&getList.size,&getList.list[0],NULL);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortDynamicPATxQHighSpeedPortDumpGet(dev,&dqList[0], &llList[0]);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssOsMemCmp(&dqList[0],&llList[0],sizeof(dqList));
        if (rc != 0)
        {
            return rc;
        }
    }

    unitBWGbps = 84;
    speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

    rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nunit BW configuration:");
    cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
    cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);

    for (i = 0 ; portInitlistArr[i].portInitlistPtr != NULL; i++)
    {
        cpssOsPrintf("\nConfigure : %s",portInitlistArr[i].namePtr);
        rc = appDemoBc2PortListInit(dev,portInitlistArr[i].portInitlistPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssDxChPortDynamicPATxQHighSpeedPortGet(dev,&getList.size,&getList.list[0],NULL);
        if (rc != GT_OK)
        {
            return rc;
        }

        cpssOsPrintf("\nDelete : %s",portInitlistArr[i].namePtr);
        rc = appDemoBc2PortListDelete(dev,portInitlistArr[i].portInitlistPtr);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}





GT_STATUS paHalfDuplex10M
(
    GT_VOID
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32 i;
    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                      *initPortsBmpPtr;/* pointer to bitmap */

    /*
    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }
    */

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    initPortsBmpPtr = &initPortsBmp;

    for (i = 0; i < 48; i++)
    {
        CPSS_PORT_SPEED_ENT speedGet;
#if 0
        rc = cpssDxChPortSpeedGet(dev, i, /*OUT*/&speedGet);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (speedGet != CPSS_PORT_SPEED_1000_E)
        {
            cpssOsPrintf("\nport %d speed = %d instead of 1G(2)\n",i,speedGet);
            return GT_FAIL;
        }
#endif
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,i);
        rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_QSGMII_E, CPSS_PORT_SPEED_10_E);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssDxChPortSpeedGet(dev, i, /*OUT*/&speedGet);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (speedGet != CPSS_PORT_SPEED_10_E)
        {
            cpssOsPrintf("\nport %d speed = %d instead of 10M(0)\n",i,speedGet);
            return GT_FAIL;
        }

        rc = cpssDxChPortDuplexModeSet(dev,i,CPSS_PORT_HALF_DUPLEX_E);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}


GT_STATUS pa168G_48x1G_NoTM_12x10G_TM
(
    GT_U32 workConsStatus
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32                  unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes; 

    GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];

    typedef struct LineSwapRes_STC
    {
        GT_PHYSICAL_PORT_NUM port;
        GT_BOOL              swapRes;
    }LineSwapRes_STC;


    static CPSS_DXCH_PORT_MAP_STC bc2Map[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort, trafficManegerEnable , tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        0,       0,           GT_FALSE,          GT_NA}
        ,{   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        1,       1,           GT_FALSE,          GT_NA}
        ,{   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        2,       2,           GT_FALSE,          GT_NA}
        ,{   3, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        3,       3,           GT_FALSE,          GT_NA}
        ,{   4, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        4,       4,           GT_FALSE,          GT_NA}
        ,{   5, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        5,       5,           GT_FALSE,          GT_NA}
        ,{   6, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        6,       6,           GT_FALSE,          GT_NA}
        ,{   7, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        7,       7,           GT_FALSE,          GT_NA}
        ,{   8, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        8,       8,           GT_FALSE,          GT_NA}
        ,{   9, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        9,       9,           GT_FALSE,          GT_NA}
        ,{  10, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       10,      10,           GT_FALSE,          GT_NA}
        ,{  11, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       11,      11,           GT_FALSE,          GT_NA}
        ,{  12, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       12,      12,           GT_FALSE,          GT_NA}
        ,{  13, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       13,      13,           GT_FALSE,          GT_NA}
        ,{  14, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       14,      14,           GT_FALSE,          GT_NA}
        ,{  15, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       15,      15,           GT_FALSE,          GT_NA}
        ,{  16, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       16,      16,           GT_FALSE,          GT_NA}
        ,{  17, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       17,      17,           GT_FALSE,          GT_NA}
        ,{  18, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       18,      18,           GT_FALSE,          GT_NA}
        ,{  19, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       19,      19,           GT_FALSE,          GT_NA}
        ,{  20, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       20,      20,           GT_FALSE,          GT_NA}
        ,{  21, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       21,      21,           GT_FALSE,          GT_NA}
        ,{  22, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       22,      22,           GT_FALSE,          GT_NA}
        ,{  23, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       23,      23,           GT_FALSE,          GT_NA}
        ,{  24, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       24,      24,           GT_FALSE,          GT_NA}
        ,{  25, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       25,      25,           GT_FALSE,          GT_NA}
        ,{  26, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       26,      26,           GT_FALSE,          GT_NA}
        ,{  27, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       27,      27,           GT_FALSE,          GT_NA}
        ,{  28, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       28,      28,           GT_FALSE,          GT_NA}
        ,{  29, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       29,      29,           GT_FALSE,          GT_NA}
        ,{  30, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       30,      30,           GT_FALSE,          GT_NA}
        ,{  31, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       31,      31,           GT_FALSE,          GT_NA}
        ,{  32, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       32,      32,           GT_FALSE,          GT_NA}
        ,{  33, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       33,      33,           GT_FALSE,          GT_NA}
        ,{  34, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       34,      34,           GT_FALSE,          GT_NA}
        ,{  35, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       35,      35,           GT_FALSE,          GT_NA}
        ,{  36, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       36,      36,           GT_FALSE,          GT_NA}
        ,{  37, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       37,      37,           GT_FALSE,          GT_NA}
        ,{  38, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       38,      38,           GT_FALSE,          GT_NA}
        ,{  39, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       39,      39,           GT_FALSE,          GT_NA}
        ,{  40, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       40,      40,           GT_FALSE,          GT_NA}
        ,{  41, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       41,      41,           GT_FALSE,          GT_NA}
        ,{  42, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       42,      42,           GT_FALSE,          GT_NA}
        ,{  43, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       43,      43,           GT_FALSE,          GT_NA}
        ,{  44, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       44,      44,           GT_FALSE,          GT_NA}
        ,{  45, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       45,      45,           GT_FALSE,          GT_NA}
        ,{  46, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       46,      46,           GT_FALSE,          GT_NA}
        ,{  47, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       47,      47,           GT_FALSE,          GT_NA}
        ,{  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,   GT_NA,            GT_TRUE,             48}
        ,{  49, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       49,   GT_NA,            GT_TRUE,             49}
        ,{  50, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       50,   GT_NA,            GT_TRUE,             50}
        ,{  51, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       51,   GT_NA,            GT_TRUE,             51}
        ,{  52, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       52,   GT_NA,            GT_TRUE,             52}
        ,{  53, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       53,   GT_NA,            GT_TRUE,             53}
        ,{  54, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       54,   GT_NA,            GT_TRUE,             54}
        ,{  55, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       55,   GT_NA,            GT_TRUE,             55}
        ,{  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       56,   GT_NA,            GT_TRUE,             56}
        ,{  57, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       57,   GT_NA,            GT_TRUE,             57}
        ,{  58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       58,   GT_NA,            GT_TRUE,             58}
        ,{  59, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       59,   GT_NA,            GT_TRUE,             59}
        ,{  80, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       60,   GT_NA,            GT_TRUE,             60}
        ,{  81, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       61,   GT_NA,            GT_TRUE,             61} /* virtual router port */
        ,{  82, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       62,   GT_NA,            GT_TRUE,             62}
        ,{  83, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       63,   GT_NA,            GT_TRUE,             63}
        ,{  64, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       64,   GT_NA,            GT_TRUE,             64}
        ,{  65, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       65,   GT_NA,            GT_TRUE,             65}
        ,{  66, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       66,   GT_NA,            GT_TRUE,             66}
        ,{  67, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       67,   GT_NA,            GT_TRUE,             67}
        ,{  68, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       68,   GT_NA,            GT_TRUE,             68}
        ,{  69, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       69,      69,           GT_FALSE,          GT_NA}
        ,{  70, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       70,      70,           GT_FALSE,          GT_NA}
        ,{  71, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       71,      71,           GT_FALSE,          GT_NA}
    };

    static PortInitList_STC portInitlist_168G_48x1G_12x10G[] = 
    {
         { PORT_LIST_TYPE_LIST,      { 56,57,58,59, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 80,81,82,83, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_LIST,      { 64,65,67,68, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  {  0,47,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    static LineSwapRes_STC lineSwapArr[] = 
    {
         { 56, GT_TRUE  }
        ,{ 57, GT_FALSE }
        ,{ 58, GT_FALSE }
        ,{ 59, GT_FALSE }
        ,{ 80, GT_TRUE  }
        ,{ 81, GT_FALSE }
        ,{ 82, GT_FALSE }
        ,{ 83, GT_FALSE }
        ,{ 64, GT_TRUE  }
        ,{ 65, GT_FALSE }
        ,{ 67, GT_FALSE }
        ,{ 68, GT_FALSE }
    };
    GT_U32 i;

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2Map)/sizeof(bc2Map[0]),
                                        &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2Map)/sizeof(bc2Map[0]),
                                     &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    unitBWGbps = 168;
    speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

    rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nunit BW configuration:");
    cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
    cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);
    rc = appDemoBc2PortListInit(dev,&portInitlist_168G_48x1G_12x10G[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*
    cpssOsPrintf("\nwork conserving mode config");
    rc = cpssDxChPortDynamicPizzaArbiterIfWorkConservingModeSet(dev,CPSS_DXCH_PA_UNIT_TXQ_E,GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nwork conserving mode check");
    for (txqPort = 0 ; txqPort < PRV_CPSS_DXCH_BC2B0_PORT_WORK_PORTN_CNS; txqPort++)
    {
        rc = prvCpssDxChPortDynamicPizzaArbiterIfTxQPortWorkConservingModeGet(dev,txqPort,&status);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (status != GT_TRUE)
        {
            return GT_BAD_PARAM;
        }
    }
    */

    rxSerdesLaneArr[0] = 0;
    rxSerdesLaneArr[1] = 1;
    rxSerdesLaneArr[2] = 2;
    rxSerdesLaneArr[3] = 3;

    txSerdesLaneArr[0] = 0;
    txSerdesLaneArr[1] = 1;
    txSerdesLaneArr[2] = 2;
    txSerdesLaneArr[3] = 3;

    for (i = 0 ; i < sizeof(lineSwapArr)/sizeof(lineSwapArr[0]); i++)
    {
        rc = cpssDxChPortXgPscLanesSwapSet(dev, lineSwapArr[i].port, rxSerdesLaneArr, txSerdesLaneArr);
        if (rc == GT_OK && lineSwapArr[i].swapRes != GT_TRUE)
        {
            return rc;
        }
        if (rc != GT_OK && lineSwapArr[i].swapRes == GT_TRUE)
        {
            return rc;
        }
    }
    return GT_OK;
}

GT_STATUS bw240G_24x10G
(
    GT_U32 ifModeIdx
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32                  unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes;

    static CPSS_DXCH_PORT_MAP_STC bc2Map[] =
    { /* Port,            mappingType                           portGroupm, intefaceNum, txQPort, trafficManegerEnable , tmPortInd*/
         {   0, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        0,       0,           GT_FALSE,          GT_NA}
        ,{   1, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        1,       1,           GT_FALSE,          GT_NA}
        ,{   2, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        2,       2,           GT_FALSE,          GT_NA}
        ,{   3, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        3,       3,           GT_FALSE,          GT_NA}
        ,{   4, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        4,       4,           GT_FALSE,          GT_NA}
        ,{   5, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        5,       5,           GT_FALSE,          GT_NA}
        ,{   6, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        6,       6,           GT_FALSE,          GT_NA}
        ,{   7, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        7,       7,           GT_FALSE,          GT_NA}
        ,{   8, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        8,       8,           GT_FALSE,          GT_NA}
        ,{   9, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,        9,       9,           GT_FALSE,          GT_NA}
        ,{  10, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       10,      10,           GT_FALSE,          GT_NA}
        ,{  11, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       11,      11,           GT_FALSE,          GT_NA}
        ,{  12, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       12,      12,           GT_FALSE,          GT_NA}
        ,{  13, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       13,      13,           GT_FALSE,          GT_NA}
        ,{  14, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       14,      14,           GT_FALSE,          GT_NA}
        ,{  15, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       15,      15,           GT_FALSE,          GT_NA}
        ,{  16, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       16,      16,           GT_FALSE,          GT_NA}
        ,{  17, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       17,      17,           GT_FALSE,          GT_NA}
        ,{  18, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       18,      18,           GT_FALSE,          GT_NA}
        ,{  19, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       19,      19,           GT_FALSE,          GT_NA}
        ,{  20, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       20,      20,           GT_FALSE,          GT_NA}
        ,{  21, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       21,      21,           GT_FALSE,          GT_NA}
        ,{  22, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       22,      22,           GT_FALSE,          GT_NA}
        ,{  23, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       23,      23,           GT_FALSE,          GT_NA}
        ,{  24, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       24,      24,           GT_FALSE,          GT_NA}
        ,{  25, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       25,      25,           GT_FALSE,          GT_NA}
        ,{  26, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       26,      26,           GT_FALSE,          GT_NA}
        ,{  27, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       27,      27,           GT_FALSE,          GT_NA}
        ,{  28, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       28,      28,           GT_FALSE,          GT_NA}
        ,{  29, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       29,      29,           GT_FALSE,          GT_NA}
        ,{  30, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       30,      30,           GT_FALSE,          GT_NA}
        ,{  31, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       31,      31,           GT_FALSE,          GT_NA}
        ,{  32, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       32,      32,           GT_FALSE,          GT_NA}
        ,{  33, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       33,      33,           GT_FALSE,          GT_NA}
        ,{  34, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       34,      34,           GT_FALSE,          GT_NA}
        ,{  35, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       35,      35,           GT_FALSE,          GT_NA}
        ,{  36, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       36,      36,           GT_FALSE,          GT_NA}
        ,{  37, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       37,      37,           GT_FALSE,          GT_NA}
        ,{  38, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       38,      38,           GT_FALSE,          GT_NA}
        ,{  39, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       39,      39,           GT_FALSE,          GT_NA}
        ,{  40, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       40,      40,           GT_FALSE,          GT_NA}
        ,{  41, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       41,      41,           GT_FALSE,          GT_NA}
        ,{  42, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       42,      42,           GT_FALSE,          GT_NA}
        ,{  43, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       43,      43,           GT_FALSE,          GT_NA}
        ,{  44, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       44,      44,           GT_FALSE,          GT_NA}
        ,{  45, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       45,      45,           GT_FALSE,          GT_NA}
        ,{  46, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       46,      46,           GT_FALSE,          GT_NA}
        ,{  47, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       47,      47,           GT_FALSE,          GT_NA}
        ,{  48, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       48,      48,           GT_FALSE,          GT_NA}
        ,{  49, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       49,      49,           GT_FALSE,          GT_NA}
        ,{  50, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       50,      50,           GT_FALSE,          GT_NA}
        ,{  51, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       51,      51,           GT_FALSE,          GT_NA}
        ,{  52, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       52,      52,           GT_FALSE,          GT_NA}
        ,{  53, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       53,      53,           GT_FALSE,          GT_NA}
        ,{  54, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       54,      54,           GT_FALSE,          GT_NA}
        ,{  55, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       55,      55,           GT_FALSE,          GT_NA}
        ,{  56, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       56,      56,           GT_FALSE,          GT_NA}
        ,{  57, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       57,      57,           GT_FALSE,          GT_NA}
        ,{  58, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       58,      58,           GT_FALSE,          GT_NA}
        ,{  59, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       59,      59,           GT_FALSE,          GT_NA}
        ,{  80, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       60,      60,           GT_FALSE,          GT_NA}
        ,{  81, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       61,      61,           GT_FALSE,          GT_NA} /* virtual router port */
        ,{  82, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       62,      62,           GT_FALSE,          GT_NA}
        ,{  83, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       63,      63,           GT_FALSE,          GT_NA}
        ,{  64, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       64,      64,           GT_FALSE,          GT_NA}
        ,{  65, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       65,      65,           GT_FALSE,          GT_NA}
        ,{  66, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       66,      66,           GT_FALSE,          GT_NA}
        ,{  67, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       67,      67,           GT_FALSE,          GT_NA}
        ,{  68, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       68,      68,           GT_FALSE,          GT_NA}
        ,{  69, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       69,      69,           GT_FALSE,          GT_NA}
        ,{  70, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       70,      70,           GT_FALSE,          GT_NA}
        ,{  71, CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E,               0,       71,      71,           GT_FALSE,          GT_NA}
    };

    static PortInitList_STC portInitlist_240G_24x10G_SR_LR[] = 
    {
         { PORT_LIST_TYPE_INTERVAL,  { 64,71,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 48,59,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 80,83,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_SR_LR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    static PortInitList_STC portInitlist_240G_24x10G_KR[] = 
    {
         { PORT_LIST_TYPE_INTERVAL,  { 64,71,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 48,59,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 80,83,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E      }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E      }
    };

    #ifdef ASIC_SIMULATION
        simCoreClockOverwrite(521);
    #endif

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssDxChPortPhysicalPortMapSet(dev,
                                        sizeof(bc2Map)/sizeof(bc2Map[0]),
                                        &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortPhysicalPortMapSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    rc = appDemoDxChMaxMappedPortSet(dev,
                                     sizeof(bc2Map)/sizeof(bc2Map[0]),
                                     &bc2Map[0]);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("appDemoDxChMaxMappedPortSet", rc);
    if (GT_OK != rc)
    {
        return rc;
    }

    unitBWGbps = 240;
    speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

    rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nunit BW configuration:");
    cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
    cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);

    switch(ifModeIdx)
    {
        case 0:
            rc = appDemoBc2PortListInit(dev,&portInitlist_240G_24x10G_SR_LR[0]);
            if (rc != GT_OK)
            {
                return rc;
            }
            break;
        case 1:
            rc = appDemoBc2PortListInit(dev,&portInitlist_240G_24x10G_KR[0]);
            if (rc != GT_OK)
            {
                return rc;
            }
            break;
        default:
            cpssOsPrintf("\r\nNo ports were initialized, use manual configuration\r\n");
            break;  
    }

    return GT_OK;
}


/*---------------------------------------*
 *                                       *
 *---------------------------------------*/
GT_STATUS runBc2OnCaelumBooard
(
    GT_U32 initPorts,
    GT_U32 initPhy
)
{
    GT_U32                  unitBWGbps;
    CPSS_DXCH_MIN_SPEED_ENT speedRes;

    GT_U8 dev = 0;
    static PortInitList_STC portInitlist_168G_48x1G_12x10G[] = 
    {
         { PORT_LIST_TYPE_LIST,      { 56,57,58,59, APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E         }
        ,{ PORT_LIST_TYPE_INTERVAL,  { 64,71,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_10000_E,  CPSS_PORT_INTERFACE_MODE_KR_E         }
        ,{ PORT_LIST_TYPE_INTERVAL,  {  0,47,1,     APP_INV_PORT_CNS }, CPSS_PORT_SPEED_1000_E,   CPSS_PORT_INTERFACE_MODE_QSGMII_E     }
        ,{ PORT_LIST_TYPE_EMPTY,     {              APP_INV_PORT_CNS }, CPSS_PORT_SPEED_NA_E,     CPSS_PORT_INTERFACE_MODE_NA_E         }
    };

    /* GT_U8 dev = 0; */
    GT_STATUS rc;
    extern GT_STATUS userForceBoardTypeCaelum(GT_VOID);

    rc = userForceBoardTypeCaelum();
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("noCpu", 1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(29,1,0); 
    if (rc != GT_OK)
    {
        return rc;
    }



    if (initPorts != 0)
    {

        unitBWGbps = 168;
        speedRes   = CPSS_DXCH_MIN_SPEED_1000_Mbps_E;

        rc = cpssDxChPortPizzaArbiterPipeBWMinPortSpeedResolutionSet(dev,unitBWGbps,speedRes);
        if (rc != GT_OK)
        {
            return rc;
        }

        cpssOsPrintf("\nunit BW configuration:");
        cpssOsPrintf("\n   BW  : %d Gbps",unitBWGbps);
        cpssOsPrintf("\n   Res : %s Mbps",str_speedRes[speedRes]);


        rc = appDemoBc2PortListInit(dev,&portInitlist_168G_48x1G_12x10G[0]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    if (initPhy != 0)
    {
        rc = bobcat2BoardPhyConfig(1, dev);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}


/*---------------------------------------*
 * Run 29,2,  with specified coreClock   *
 *---------------------------------------*/
GT_STATUS runTM(GT_U32 coreClock)
{
    GT_STATUS rc;
    GT_U8 dev = 0;

    #ifdef ASIC_SIMULATION
        rc = appDemoBcat2B0SimCoreClockSet(dev,coreClock); 
        /* simCoreClockOverwrite(175); */
        if (rc != GT_OK)
        {
            return rc;
        }

    #endif

    rc = cpssInitSystem(29,2,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = appDemoBobcat2PortListResourcesPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoBobk2PortGobalResourcesPrint(dev);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}




