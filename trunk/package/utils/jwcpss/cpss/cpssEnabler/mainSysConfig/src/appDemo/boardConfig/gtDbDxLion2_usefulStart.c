#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegsVer1.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortIfModeCfgBcat2Resource.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortInterlaken.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortDynamicPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPizzaArbiterWorkConserving.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/cpssDxChPortDynamicPAUnitBW.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/DynamicPizzaArbiter/prvCpssDxChPortTxQHighSpeedPortsDrv.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cscd/cpssDxChCscd.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>       
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiterProfile.h>


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

extern GT_STATUS gtBobcat2PortMappingDump
(
    IN  GT_U8  dev
);



#ifdef __cplusplus
}
#endif /* __cplusplus */


                                                                                    
                                                                                    
static CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumLIst_NoALLOC[] = 
{     /* port speed        ,     slices N */                                    
     {             CPSS_PORT_SPEED_NA_E,       CPSS_INVALID_SLICE_NUM }     
};                                                                              
                                                                                
/*-----------------------------------------------------------------------------*
 * cores 0 : speed 2 slice conversion table
 *-----------------------------------------------------------------------------*/
static CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumList_47200[] = 
{     /* port speed        ,     slices N */                                        
      {          CPSS_PORT_SPEED_11800_E,          2 }
     ,{          CPSS_PORT_SPEED_47200_E,          5 }
     ,{             CPSS_PORT_SPEED_NA_E,       CPSS_INVALID_SLICE_NUM }     
};                                                 
                                                   
static CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumList_10000[] = 
{     /* port speed        ,     slices N */                                        
      {          CPSS_PORT_SPEED_10000_E,          1 }
     ,{             CPSS_PORT_SPEED_NA_E,       CPSS_INVALID_SLICE_NUM }     
};                                                 


                                                           
CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC portGroupSpeed2SliceNumList_3x47200_1x10000 =  
{                                                          
     /* 10  */                                             
     {                                                     
          {   0, &portSpeed2SliceNumList_47200    [0] }
         ,{   1, &portSpeed2SliceNumLIst_NoALLOC  [0] }
         ,{   2, &portSpeed2SliceNumLIst_NoALLOC  [0] }
         ,{   3, &portSpeed2SliceNumLIst_NoALLOC  [0] }
         ,{   4, &portSpeed2SliceNumList_47200    [0] }
         ,{   5, &portSpeed2SliceNumLIst_NoALLOC  [0] }
         ,{   6, &portSpeed2SliceNumLIst_NoALLOC  [0] }
         ,{   7, &portSpeed2SliceNumLIst_NoALLOC  [0] }
         ,{   8, &portSpeed2SliceNumList_47200    [0] }
         ,{   9, &portSpeed2SliceNumList_10000    [0] }
         ,{ CPSS_PA_INVALID_PORT  , NULL     }
     }                                                     
};                                                         
                                                           
                                                           
static CPSS_DXCH_PORT_PRIORITY_MATR_STC portPriority_3x47200_1x10000 = 
{                                                              
/* GT_U32 portN;     */  10                                    
/* portPriorityList  */  ,{                                    
    /* port  0  */             {  CPSS_PA_INVALID_PORT }
    /* port  1  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  2  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  3  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  4  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  5  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  6  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  7  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  8  */            ,{  CPSS_PA_INVALID_PORT }
    /* port  9  */            ,{  CPSS_PA_INVALID_PORT }
                          }                                    
};    


/*-------------------------------------------------------
 * core 0 : port configuration
 *-------------------------------------------------------
 */
static CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC portConfig_3x47200_1x10000_mode_regular_by_1Slice = 
{ 
/* maxPortNum*/ 10
/* portArr   */,{
/* PORT  0  */      { CPSS_PA_INVALID_SLICE }
/* PORT  1  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  2  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  3  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  4  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  5  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  6  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  7  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  8  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  9  */     ,{ 15, CPSS_PA_INVALID_SLICE }
                }
};

static CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC portConfig_3x47200_1x10000_mode_regular_by_2Slice = 
{ 
/* maxPortNum*/ 10
/* portArr   */,{
/* PORT  0  */      { 0, 6, CPSS_PA_INVALID_SLICE }
/* PORT  1  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  2  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  3  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  4  */     ,{ 1, 7, CPSS_PA_INVALID_SLICE }
/* PORT  5  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  6  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  7  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  8  */     ,{ 2, 8, CPSS_PA_INVALID_SLICE }
/* PORT  9  */     ,{ CPSS_PA_INVALID_SLICE }
                }
};


static CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC portConfig_3x47200_1x10000_mode_regular_by_5Slice = 
{ 
/* maxPortNum*/ 10
/* portArr   */,{
/* PORT  0  */      { 0, 3, 6,  9, 12, CPSS_PA_INVALID_SLICE }
/* PORT  1  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  2  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  3  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  4  */     ,{ 1, 4, 7, 10, 13, CPSS_PA_INVALID_SLICE }
/* PORT  5  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  6  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  7  */     ,{ CPSS_PA_INVALID_SLICE }
/* PORT  8  */     ,{ 2, 5, 8, 11, 14, CPSS_PA_INVALID_SLICE }
/* PORT  9  */     ,{ CPSS_PA_INVALID_SLICE }
                }
};


static CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC portConfig_3x47200_1x10000_mode_regular = 
{                                 
/* portNum */       10
/* maxSliceNum */  ,16
/* pizzaCfg */     ,{             
                       {  1 , &portConfig_3x47200_1x10000_mode_regular_by_1Slice }
                      ,{  2 , &portConfig_3x47200_1x10000_mode_regular_by_2Slice }
                      ,{  5 , &portConfig_3x47200_1x10000_mode_regular_by_5Slice }
                      ,{  0 , NULL                                       }
                    }             
};                                
                                  


static CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC portConfig_3x47200_1x10000_mode_extended = 
{                                 
/* portNum */       10
/* maxSliceNum */  ,16
/* pizzaCfg */     ,{             
                       {  1 , &portConfig_3x47200_1x10000_mode_regular_by_1Slice }
                      ,{  2 , &portConfig_3x47200_1x10000_mode_regular_by_2Slice }
                      ,{  5 , &portConfig_3x47200_1x10000_mode_regular_by_5Slice }
                      ,{  0 , NULL                                       }
                    }             
};                                
                                  
                                  


CPSS_DXCH_PIZZA_PROFILE_STC  portConfig_3x47200_1x10000 = 
{                           
    /* portNum        */  10
    /* maxSliceNum    */ ,16
    /* txQDef         */ ,{       
    /* TXQ_repetition */     7   
    /*    TxQDef      */    ,{    
    /*       CPU TYPE */         CPSS_DXCH_CPUPORT_TYPE_FIXED_E
    /*       CPU Port */        ,15
    /*       Slices   */        ,{  31, CPSS_PA_INVALID_SLICE }
    /*                */     }     
    /*                */  }        
    /* reg mode       */ ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&portConfig_3x47200_1x10000_mode_regular
    /* ext mode       */ ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&portConfig_3x47200_1x10000_mode_extended
    /* port priority  */ ,(CPSS_DXCH_PORT_PRIORITY_MATR_STC *)&portPriority_3x47200_1x10000
};                           
     
extern GT_STATUS lion2PortInfoPrint
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    portNum
);


typedef enum APP_DEMO_SYSTYPE_ENT
{
    APPDEMO_SYSTYPE_LION2_E = 27,
    APPDEMO_SYSTYPE_BC2_E   = 29
}APP_DEMO_SYSTYPE_ENT;


GT_STATUS paSerdes_12p1875GHz
(
    GT_U32 systemType
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32 i;
    GT_U32 portNumArr[3];
    GT_U32 portGroupBmp = BIT_0;
    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                      *initPortsBmpPtr;/* pointer to bitmap */

    switch (systemType) /* BC 2*/
    {
        case APPDEMO_SYSTYPE_LION2_E: /*LION2*/  
            portNumArr[0] =  0; 
            portNumArr[1] =  4;
            portNumArr[2] =  8;
            rc = cpssDxChPortPizzaArbiterIfUserTableSet(dev,portGroupBmp,
                                                        &portConfig_3x47200_1x10000,
                                                        &portGroupSpeed2SliceNumList_3x47200_1x10000);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case APPDEMO_SYSTYPE_BC2_E: /*BC2 */   
            portNumArr[0] =  48; 
            portNumArr[1] =  52;
            portNumArr[2] =  56;
        break;
        default:
        {
            cpssOsPrintf("\nERROR : Unknow system %d",systemType);
            return GT_FAIL;
        }

    }
    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(systemType,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    for (i = 0; i < 3; i++)
    {
        initPortsBmpPtr = &initPortsBmp;
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNumArr[i]);
        rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_XHGS_E, CPSS_PORT_SPEED_11800_E);
        if (rc != GT_OK)
        {
            return rc;
        }

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNumArr[i]);
        rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_XHGS_E, CPSS_PORT_SPEED_47200_E);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (systemType == APPDEMO_SYSTYPE_LION2_E)
        {
            rc = lion2PortInfoPrint(dev,portNumArr[i]);
            if (rc != GT_OK)
            {
                return rc;
            }

        }

    }

    return GT_OK;
}





GT_STATUS paSerdesLion2_12p1875GHz_as40G
(
    
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32 i;
    GT_U32 portNumArr[4];

    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                      *initPortsBmpPtr;/* pointer to bitmap */

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    portNumArr[0] =  0; 
    portNumArr[1] =  4;
    portNumArr[2] =  8;
    portNumArr[3] =  9;

    rc = cpssInitSystem(27,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    for (i = 0; i < sizeof(portNumArr)/sizeof(portNumArr[0]); i++)
    {
        initPortsBmpPtr = &initPortsBmp;

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNumArr[i]);
        rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_XHGS_E, CPSS_PORT_SPEED_47200_E);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = lion2PortInfoPrint(dev,portNumArr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    cpssOsPrintf("\n\n");
    return GT_OK;
}

GT_STATUS paSerdes_12p1875GHz_SR
(
    GT_U32 systemType
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32 i;
    GT_U32 portNumArr[3];
    GT_U32 portGroupBmp = BIT_0;
    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                      *initPortsBmpPtr;/* pointer to bitmap */

    switch (systemType) /* BC 2*/
    {
        case APPDEMO_SYSTYPE_LION2_E: /*LION2*/  
            portNumArr[0] =  0; 
            portNumArr[1] =  4;
            portNumArr[2] =  8;
            rc = cpssDxChPortPizzaArbiterIfUserTableSet(dev,portGroupBmp,
                                                        &portConfig_3x47200_1x10000,
                                                        &portGroupSpeed2SliceNumList_3x47200_1x10000);
            if (rc != GT_OK)
            {
                return rc;
            }
        break;
        case APPDEMO_SYSTYPE_BC2_E: /*BC2 */   
            portNumArr[0] =  48; 
            portNumArr[1] =  52;
            portNumArr[2] =  56;
        break;
        default:
        {
            cpssOsPrintf("\nERROR : Unknow system %d",systemType);
            return GT_FAIL;
        }
    }
    
    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssInitSystem(systemType,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    for (i = 0; i < 3; i++)
    {
        initPortsBmpPtr = &initPortsBmp;
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNumArr[i]);
        rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_XHGS_SR_E, CPSS_PORT_SPEED_11800_E);
        if (rc != GT_OK)
        {
            return rc;
        }

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNumArr[i]);
        rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_XHGS_SR_E, CPSS_PORT_SPEED_47200_E);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (systemType == APPDEMO_SYSTYPE_LION2_E)
        {
            rc = lion2PortInfoPrint(dev,portNumArr[i]);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    return GT_OK;
}


GT_STATUS paSerdesLion2_12p1875GHz_as40G_SR
(
    GT_VOID
)
{
    GT_STATUS rc;
    GT_U8 dev = 0;
    GT_U32 i;
    GT_U32 portNumArr[4];

    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                      *initPortsBmpPtr;/* pointer to bitmap */

    rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
    if (rc != GT_OK)
    {
        return rc;
    }

    portNumArr[0] =  0; 
    portNumArr[1] =  4;
    portNumArr[2] =  8;
    portNumArr[3] =  9;

    rc = cpssInitSystem(27,1,0);
    if (rc != GT_OK)
    {
        return rc;
    }


    for (i = 0; i < sizeof(portNumArr)/sizeof(portNumArr[0]); i++)
    {
        initPortsBmpPtr = &initPortsBmp;

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNumArr[i]);
        rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE,CPSS_PORT_INTERFACE_MODE_XHGS_SR_E, CPSS_PORT_SPEED_47200_E);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = lion2PortInfoPrint(dev,portNumArr[i]);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    cpssOsPrintf("\n\n");
    return GT_OK;
}



