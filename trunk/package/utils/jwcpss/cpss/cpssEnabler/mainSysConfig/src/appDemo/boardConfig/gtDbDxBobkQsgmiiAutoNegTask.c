#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChRegsVer1.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChMacCtrl.h>
#include <cpss/dxCh/dxChxGen/port/macCtrl/prvCpssDxChGEMacCtrl.h> 
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortMapping.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/boardConfig/gtDbDxBobcat2Mappings.h>
#include <appDemo/boardConfig/gtDbDxBobcat2PhyConfig.h>


#include <gtOs/gtOsTask.h>
#include <gtOs/gtOsMsgQ.h>
#include <gtOs/gtOsSem.h>

#define  APPDEMO_AN_TASK_TEST_CNS     1
#if (APPDEMO_AN_TASK_TEST_CNS == 1)
    #define APPDEMO_AN_TASK_UNIT_TEST        0
    #define APPDEMO_AN_TASK_DEBUG_PRINT_EN   1
#else
    #define  APPDEMO_AN_TASK_UNIT_TEST       0
    #define APPDEMO_AN_TASK_DEBUG_PRINT_EN   0
#endif




#undef APPDEMO_AN_WA_USE_INTR

/*-------------------------------------------------------------------------------------------
Goal:
    AN-WA-Task is intended to solve the problem in HW auto-negotiation process, 
    when PHY updates the corresponding MAC about results of auto-negotiation, 
    but MAC fails to update auto-negotiation configuration register.
    The problem exists at BobK Caelum/Cetus packet-processors on ports with MAC 
    59-59, 61-72
                                                                            BobK-Cetus/Caelum
    +---------+   AN-Results                                 AN-Results   +---------+
    |  MAC    |<----+                                               +---->|  MAC    |
    +---------+     |                                               |     +---------+
HW update|          |   +------+      AN-exchange      +------+     |         | HW Update failed
         |          +---|  PHY |<--------------------->|  PHY |-----+         | 
    +---------+         +------+                       +------+            +--------+
    |         |AN config register                                          |        |  AN config register
    +---------+                                                            +--------+



    The solution is to take control of the following interrupts 
    1. Sync Change
    2. AN complete
    read results of AN process and to update AN configuration register in software.
    
                                                                            BobK-Cetus/Caelum    
    +---------+   AN-Results                                 AN-Results   +---------+          +-------------+
    |  MAC    |<----+                                               +---->|  MAC    |--------->|  AN-WA-Task | 
    +---------+     |                                               |     +---------+          +-------------+       
HW update|          |   +------+      AN-exchange      +------+     |                               |
failed   |          +---|  PHY |<--------------------->|  PHY |-----+    AN config register         |
    +---------+         +------+                       +------+            +--------+               |
    |         |AN config register                                          |        |<--------------+ 
    +---------+                                                            +--------+



Use Cases:

    The following interface modes require usage of auto-negotiating process WA : SGMII/QSGMII/1000BaseX.
    
    The work-around support the following interconnection modes:
    
    1. 2 different ports
        1. 1000BaseX
        2.  SGMII  1G (+ PHY transceiver)
        3.  QSGMII 1G (+ PHY transceiver)  
        3.  SGMII  1G with cable (no PHY, port 1 shall be master, port 2 shall be slave )
        4.  QSGMII 1G with cable (no PHY, port 1 shall be master, port 2 shall be slave,  ports should not share same SERDES)  

    2. single port SERDES-loopback
        1. 1000BaseX with serdes-loopback   
        
        SGMII/QSGMII interfaces not supports SERDES-loopback mode.

    Mode 1: SQGMII/QSGMII interfaces with PHY (master/slave definition is not applicable):
    
    PHYs perform AN process and inform corresponding MACs about AN results.
    
    +---------+   AN-Results                                 AN-Results   +---------+
    |  MAC    |<----+                                               +---->|  MAC    |
    +---------+     |                                               |     +---------+
                    |   +------+      AN-exchange      +------+     |  
                    +---|  PHY |<--------------------->|  PHY |-----+  
                        +------+                       +------+        
                                                                       
                                                                       

    Mode 2: SQGMII/QSGMII no PHY/(master/slave definition applicable): 	MACs perform AN process.	
    +---------+          +---------+
    |  MAC    |<-------->|  MAC    |
    | (master)|          | (slave) |
    +---------+          +---------+
                        
    Mode 3: 1000BaseX (master/slave definition is not applicable):   MACs perform AN process.	
    +---------+          +---------+
    |  MAC    |<-------->|  MAC    |
    | (master)|          | (slave) |
    +---------+          +---------+
        
        
    Table below summarize all available/supported mode.
    +-------------+-------------------+--------+-------------+------------+
    |             |    Speed          | Duplex |  Connection |  Serdes    |
    |             | 10    100   1000  | HD  FD |  PHY no-PHY | LoopBack   |
    +-------------+-------------------+--------+-------------+------------+
    |  1000BaseX  |  -     -     V    |  V   V |   -     V   |     V      |
    |  SGMII      |  V     V     V    |  V   V |   V     V   |     -      |
    |  QSGMII     |  V     V     V    |  V   V |   V     V   |     -      |
    +-------------+-------------------+--------+-------------+------------+

    The following parameters are under control of AN process:
    1. Enable/Disable speed AN
    2. Enable/Disable duplex mode AN
    3. Enable/Disable Flow control AN
    4. MAC mode selection (master/slave).
    5. Speed Advertising  (10M/100M/1000M)
    6. Duplex Advertising (Half-Duplex/Full-Duplex)
    7. Flow Control Advertising (No-Flow-Control/Flow Control Present)
    
    
Usage:
    Initialisation.
        add entry "initAnWaTask" with value 1 to application demo DB.
        call appDemoDbEntryAdd("initAnWaTask", 1);   before cpssInitSystem(). 
        Task will be initialized automatically.
        
    Enable AN process on port and configuration of all parameters at single call.
        1. call cpssDxChPortModeSpeedSet() and define port interface mode and speed.
        2. call AN_WA_Task_PortAnEnable() in order to allow AN for the port and defining all AN parameters.

    Changing all parameters of AN process on already configured port and restart AN process.
        1. call AN_WA_Task_AnParamsSet() .

    Changing Single parameter of AN process on already configured port and restart AN process.
        1. Single parameter change 
            a. call cpssDxChPortSpeedSet() to change speed
            b. call cpssDxChPortDuplexModeSet() to change duplex
            c. call cpssDxChPortFlowCntrlAutoNegEnableSet() to change flow control.
        2. call cpssDxChPortInbandAutoNegRestart()  in order to restart AN process.

        
    Disable AN process on port
        1. AN_WA_Task_PortAnDisable()  in order to remove the port from AN process.		
        




+-----------------------------------------------+----------------------------------+---------------+
|     IF          CONNECTION       CONNECTION   |    params                        |  Status       |
|                    TYPE            MODE       |   master/slave                   |               |
|-----------------------------------------------|----------------------------------|---------------|
|  SGMII             phy           2-ports      |  both slaves                     |    PASSED     |
|  QSGMII            phy           2-ports      |  both slaves                     |    PASSED     | 
|-----------------------------------------------|----------------------------------|---------------|
|  1000BaseX         no-phy        2-ports      |  both slaves (but not relevant)  |    PASSED     |
|  SGMII             no-phy        2-ports      |  1-master 1-slave                |    PASSED     |
|  QSGMII            no-phy        2-ports      |  1-master 1-slave                |    PASSED     | ports should not be on same SERDES
|-----------------------------------------------|----------------------------------|---------------|
|  1000BaseX         ---           loop-back    |  both slaves (not relevant)      |    PASSED     |
|  SGMII             no-phy        loop-back    |  should not work                 |    ----       | 
|  QSGMII            no-phy        loop-back    |  should not work                 |    ----       | 
+-----------------------------------------------+----------------------------------+---------------+

Speed (for all IFs  : 
    SGMII/QSGMII -- all options (PHY dependent) 
    1000BaseX    -- just 1000    
+--------------+--------+-----------------+
|  port1 port2 | result |    Test         |
|  phy    phy  |  speed |    result       |
+--------------+--------+-----------------+
|   10     10  |   10   | -- PASSED (*)   |
|   10    100  |   10   | -- PASSED (*)   |
|   10   1000  |   10   | -- PASSED (*)   |
|  100    100  |  100   | -- PASSED (*)   |
|  100   1000  |  100   | -- PASSED (*)   |
| 1000   1000  | 1000   | -- PASSED       |
+--------------+--------+-----------------+

(*) Marvell PHYs are configured to support speeds 10M,100M,1000M, therefore result shall be 1000M independently on portModeSpeedSet() parameters.
   
Duplex (for all IFs  : SGMII/QSGMII/1000BaseX)
SGMII/QSGMII :  PHY dependant.
1000NaseX    :  MAC dependant.
+--------------+--------+-----------------+
|  port1 port2 | result |    Test         |
|              |        |    result       |
+--------------+--------+-----------------+
|  Half   half | Half   | -- PASSED (*)   |
|  Half   Full | Half   | -- PASSED (*)   |
|  Full   Full | Full   | -- PASSED       |
+--------------+--------+-----------------+

(*) SGMII/QSGMII : Marvell PHYs are configured to support both HD and FD. Therefore the result shall be FD independently on portDuplexModeSet() parameters.
1000NaseX    : depends on portDuplexModeSet() parameters.

Flow Control 
    pause adv <==> support flow control
 
SGMII/QSGMII :  phy dependant.
1000NaseX    :  mac dependant.
+--------------+--------+-----------------+
|  port1 port2 | result |    Test         |
|              |        |    result       |
+--------------+--------+-----------------+
|   0      0   | No FC  |                 |
|   1      0   | No FC  |                 |
|   1      1   |  FC    |                 |
+--------------+--------+-----------------+

SGMII/QSGMII : Marvell PHYs are configured to support flow control, therefore result shall be flow control supported independently on flow control settings parameters.
1000NaseX    : depends on cpssDxChPortFlowCntrlAutoNegEnableSet() parameters.
 
   
Sequence diagrams (user perspective)
        
        1.  2 port configuration on different devices:
        
                AppDemo               AN-WA-Task              CPSS               HW
              Enable port for AN
              ----------------
                   |                     |                      |                |
                   | portModeSpeedSet(port1,if,speed)           |                |
                   |------------------------------------------->|                |
                   |                     |                      |                |         
                   | AN_WA_Task_PortAnEnable(port1)             |                |
                   |-------------------->|                      |                |
                   |                     |           INTR-SyncChange(port1)      |
                   |                     |<--------------------------------------|
                   |                     |                      |                |
                   |                     |           INTR-An-Complete(port1)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Conf-Update(port1)     |
                   |                     |-------------------------------------->|
              Change In params           |                      |                |     
              ----------------           |                      |                |     
                   | msgPortAnParamsSet(port1)                  |                |
                   |-------------------->|                      |                |
                   |                     | portInbandAutoNegRestart(port1)       |             
                   |                     |--------------------->|                |
                   |                     |           INTR-An-Complete(port1)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Update(port1)          |
                   |                     |-------------------------------------->|
                   |                     |                      |                |     
                   |                     |                      |                |     
              Change in Single param     |                      |                |     
              ----------------------     |                      |                |     
                   |cpssDxChPortSpeedSet(port,speed)            |                |     
                   |------------------------------------------->|                |
                   |portInbandAutoNegRestart(port1)             |                |         
                   |------------------------------------------->|                |
                   |                     |           INTR-An-Complete(port1)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Conf-Update(port1)     |
                   |                     |-------------------------------------->|
                   |                     |                      |                |     
              Detaching and attaching cable (at already configured port          |     
              ----------------------     |                      |                | attach cable    
                   |                     |           INTR-SyncChange(port1)      |<--------------
                   |                     |<--------------------------------------|
                   |                     |           INTR-An-Complete(port1)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Update(port1)          |
                   |                     |-------------------------------------->|
                   |                     |                      |                |     
                   
                   
            
        2.  2 port configuration on single device:
        
           1. sequential configuration of ports
        
                AppDemo               AN-WA-Task              CPSS               HW
                   |                     |                      |                |
                   | portModeSpeedSet(port1,if,speed)           |                |
                   |------------------------------------------->|                |
                   | AN_WA_Task_PortAnEnable(port1)             |                |
                   |-------------------->|                      |                |
                   | portModeSpeedSet(port2,if,speed)           |                |
                   |------------------------------------------->|                |         
                   | AN_WA_Task_PortAnEnable(port2)             |                |
                   |-------------------->|                      |                |
                   |                     |           INTR-SyncChange(port1)      |
                   |                     |<--------------------------------------|
                   |                     |           INTR-SyncChange(port2)      |
                   |                     |<--------------------------------------|
                   |                     |                      |                |
                   |                     |                      |                |
                   |                     |           INTR-An-Complete(port1)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Update(port1)          |
                   |                     |-------------------------------------->|
                   |                     |                      |                |     
                   |                     |           INTR-An-Complete(port2)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Update(port1)          |
                   |                     |-------------------------------------->|
                   |                     |                      |                |
                   |                     |                      |                |
                   
           2. Parallel configuration of ports
        
                AppDemo               AN-WA-Task              CPSS               HW
                   |                     |                      |                |
                   | portModeSpeedSet(port1,if,speed)           |                |
                   |------------------------------------------->|                |
                   | portModeSpeedSet(port2,if,speed)           |                |
                   |------------------------------------------->|                |
                   |                     |                      |                |         
                   | AN_WA_Task_PortAnEnable(port1)             |                |
                   |-------------------->|                      |                |
                   | AN_WA_Task_PortAnEnable(port2)             |                |
                   |-------------------->|                      |                |
                   |                     |           INTR-SyncChange(port1)      |
                   |                     |<--------------------------------------|
                   |                     |           INTR-SyncChange(port2)      |
                   |                     |<--------------------------------------|
                   |                     |                      |                |
                   |                     |                      |                |
                   |                     |           INTR-An-Complete(port1)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Update(port1)          |
                   |                     |-------------------------------------->|
                   |                     |                      |                |     
                   |                     |           INTR-An-Complete(port2)     |
                   |                     |<--------------------------------------|
                   |                     | Port-AN-Config-Update(port1)          |
                   |                     |-------------------------------------->|
                   |                     |                      |                |
                   

Detailed sequence diagrams: 
---------------------------
      MsgAnPortEnable
      MsgParamsSet
      Ev-Sync-Change
      Ev-AN-Complete
      MsgPortAnDisable

   
    Terminal                 AN-Task                  UserEventHandler                       CPSS                   HW-MAC                        Cable
       Task                                               Task     
         |                      |                           |                                  |                        |                           |          
     Init phase 
         |                      |                           |                                  |                        |                           |                
         |Close  Itr SynChnge/AN-Complete for all ports     |                                  |                        |                           |          
         |                      |                           |                                  |                        |                           |          
         |                      |                           |                                  |                        |                           |      
         |                      |                           |                                  |                        |  Connected                |      
         |                      |                           |                                  |                        |<--------------------------|
         | cpssDxChPortModeSpeedSet(mac=0,QSGMII,1000)      |                                  |                        |                           |
         |------------------------------------------------------------------------------------>| GMIISpee = 1(1000)     |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      |                           |                                  | MII speed = xxx        |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      |                           |                                  |  Port-Type = 1(SGMII)  |                           |
         |                      |                           |                                  |----------------------->|                           |
         | MsgAnPortEnable(mac=0, master/slave??  speedAn=1,DuplexAn=1,1)                      |                        |                           |
         |--------------------->|                           |                                  |                        |                           |
         |                      | cpssDxChPortDuplexAutoNegEnableSet(mac=0,0)                  |                        |                           |
         |                      |------------------------------------------------------------->|   AnDuplexEn =1        |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | cpssDxChPortSpeedAutoNegEnableSet(mac=0,0)                   |                        |                           |
         |                      |------------------------------------------------------------->|  AnSpeedEn = 1         |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | cpssDxChPortDuplexModeSet(port,duplexMode)                   |                        |                           |
         |                      |------------------------------------------------------------->| SetFullDuplex          |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | BobKAutoNegTask_AnFcEnPauseAdvAsmPauseAdvSet(mac=0,0,)       |                        |                           |
         |                      |------------------------------------------------------------->| AnFCEn =1              |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      |                           |                                  | PauseAdv=1             |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | prvCpssDxChBobcat2PortGigaMacAnMasterModeSet(mac=0,master/slave)                      |                           |
         |                      |------------------------------------------------------------->| MacType MasterMode=0   |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | cpssDxChPortInbandAutoNegEnableSet(mac=0,1)                  |                        |                           |
         |                      |------------------------------------------------------------->| InBandAnEn = 1         |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | Enable intr per mac=0     |                                  |                        |                           |
         |                      |------------------------------------------------------------->|                        |                           |
         |                      |                           |                                  |                        |                           |
         |                      |                           |                                  |                        |                           |
         |                      |                           |            Intr Sync-Change      |                        |                           |
         |                      |    Ev-Sync-Change(mac=0)  |<----------------------------------------------------------|                           |
         |                      |<--------------------------|                                  |                        |                           |
         |                      |                           |                                  |                        |                           |
         |                   +-----+                        |                                  |                        |                           |
         |                   |     |  PROCEDURE-UPON        |                                  |                        |                           |
         |                   |     |   EV-SynChange         |                                  |                        |                           |
         |                   |     |                        |                                  |                        |                           |
         |                   |     |                        |                                  |                        |                           |
         |                   +-----+                        |                                  |                        |                           |
         |                      |                           |                                  |                        |                           |
         |                      |                           |            Intr AN-Complete      |                        |                           |
         |                      |                           |<----------------------------------------------------------|                           |
         |                      |                           | cpssDxChAnByPassActGet()         |                        |                           |        
         |                      |                           |--------------------------------->|   AnByPassAct          |                           |        
         |                      |                           |                                  |----------------------->|                           |        
         |                      |                           |                                  |  AnByPassAct == 1      |                           |        
         |                      |                           |   AnByPassAct == 1               |<-----------------------|                           |
         |                      |                           |<---------------------------------|                        |                           |
         |                      |                    Do nothing                                |                        |                           |
         |                      |                           |            Intr AN-Complete      |                        |                           |        
         |                      |                           |<----------------------------------------------------------|                           |
         |                      |                           | cpssDxChAnByPassActGet()         |                        |                           |        
         |                      |                           |--------------------------------->|   AnByPassAct          |                           |        
         |                      |                           |                                  |----------------------->|                           |        
         |                      |                           |                                  |  AnByPassAct == 0      |                           |        
         |                      |                           |   AnByPassAct == 0               |<-----------------------|                           |
         |                      |                           |<---------------------------------|                        |                           |
         |                      |    Ev-AN-Complete(mac=0,codeword)                            |                        |                           |
         |                      |<--------------------------|                                  |                        |                           |
         |                      |                           |                                  |                        |                           |
         |                   +-----+                        |                                  |                        |                           |
         |                   |     |  PROCEDURE-UPON        |                                  |                        |                           |
         |                   |     |   Ev-AN-Complete       |                                  |                        |                           |
         |                   |     |                        |                                  |                        |                           |
         |                   |     |                        |                                  |                        |                           |
         |                   +-----+                        |                                  |                        |                           |
         |                      |                           |                                  |                        |                           |
         | MsgPortAnDisable (mac=0)                         |                                  |                        |                           |
         |--------------------->|                           |                                  |                        |                           |
         |                      | prvCpssDxChBobcat2PortGigaMacAnMasterModeSet(mac=0,0)        |                        |                           |
         |                      |------------------------------------------------------------->| MacType MasterMode=0   |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | cpssDxChPortInbandAutoNegEnableSet(mac=0,0)                  |                        |                           |
         |                      |------------------------------------------------------------->| InBandAnEn = 0         |                           |
         |                      |                           |                                  |----------------------->|                           |
         |                      | Disab;e intr per mac=0    |                                  |                        |                           |
         |                      |------------------------------------------------------------->|                        |                           |
         |                      |                           |                                  |                        |                           |



HW configurations:

/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Port Auto-Negotiation Configuration/Port Auto-Negotiation Configuration
    13-13 AnDuplexEn(Relevant when the port speed is not 1000 Mbps)  
        0 – An duplex disable                                          cpssDxChPortDuplexAutoNegEnableSet
        1 – An duplex enable                                           cpssDxChPortDuplexAutoNegEnableGet
    12-12 SetFullDuplex Relevant when the port speed is not 1000 Mbps. 
        0 – Half duplex                                                cpssDxChPortDuplexModeSet(dev,port,mode)
        1 – Full Duplex
    11-11 AnFCEn Enables Auto-Negotiation for Flow
        0 – AN of FlowControl disable                                  cpssDxChPortFlowCntrlAutoNegEnableSet
        1 – AN of FlowControl enable
    10-10 PauseAsmAdv Relevant for 1000Base-X only.                    (???) cpssDxChPortFlowCntrlAutoNegEnableSet   NO API ??? 
        0 – Symmetric FC
        1 – Asymmetric FC
    9-9 PauseAdv This is the Flow Control advertise                    cpssDxChPortFlowCntrlAutoNegEnableSet
        0 - No flow Control
        1 – support Flow Control.
    7-7  AnSpeedEn (relevant when port type == 0 (SGMII)
        0 – Disable Update                                             cpssDxChPortSpeedAutoNegEnableSet
        1 - Enable Update
    6-6  SetGMIISpeed
        0 – not 1000Mbps                                               inside cpssDxChPortModeSpeedSet/cpssDxChPortSpeedSet
        1 – 1000Mbps
    5-5  SetMIISpeed 
        0 – 10Mbps                                                     inside cpssDxChPortModeSpeedSet/cpssDxChPortSpeedSet
        1 – 100Mbps
    2-2 InBandAnEn In-band Auto-Negotiation enable               cpssDxChPortInbandAutoNegEnableSet(dev, port, GT_TRUE);
        0 – disable                                              cpssDxChPortInbandAutoNegEnableGet(dev, port, &status);
        1 – enable
    4-4 InbandRestartAN                                          cpssDxChPortInbandAutoNegRestart(dev,port)
        0 – Don’t Restart
        1 - Restart
    1-1  ForceLinkUp                                             cpssDxChPortForceLinkPassEnableSet
    0-0  ForceLinkDonw                                           cpssDxChPortForceLinkDownEnableSet

        
/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Port Interrupt/Port Interrupt Cause
                            Auto Neg Completed On Port Sync Status Change
                            
/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/SGMII Configuration/Port SERDES Configuration Register3
    
    0-15 Auto-Negotiation code word value received from a link partner. 
    
    ---> is used in PROCEDURE-UPON-EVENT-AN-COMPLETE  at  operator  ReadRxConfigReg =!Port SERDES Configuration Register3[15:0]


/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/SGMII Configuration/Port SERDES Configuration Register0/
     11 - 11 Auto-Negotiation Master Mode Enable 
                 0 - Disable (Slave)    MAC Mode 
                 1  -Enabkle (Master)   PHY Mode 


    
/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Tri-Speed Port MAC Configuration/Port MAC Control Register0
    1-1  Port-Type 
            0 -- SGMII                                                  prvCpssDxChBobcat2PortGigaMacTypeGet
            1 – 1000BaseX 

/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Tri-Speed Port MAC Configuration/Port MAC Control Register4
        4-4 SetFcEnTx                                               cpssDxChPortFlowControlEnableSet(dev,port, Rx/Tx)
        3-3 SetFcEnRx
/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Tri-Speed Port MAC Configuration/Macro Control  (???)
/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Port Status/Port Status Register0
        14-14 SyncOK
        10-10 SyncFail10ms    


Control Of AN process : 
    /Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Port Auto-Negotiation Configuration/Port Auto-Negotiation Configuration
    15  - 15    TxConfigRegToEncodeTable 
    14  - 14    AutoMedia SelectEn   
    13  - 13    AnDuplexEn    
    12  - 12    SetFullDuplex  0
    11  - 11    AnFcEn         0 - AnFc Disable
                               1 - AnFc Enable
    10  - 10    PauseAsmAdv    0 - Symmetric Flow Control  
                               1 - Assimetric
     9  -  9    PauseAdv       0 - No Flow Control
                               1 - Flow Control
     7  -  7    AnSpeedEn      0 - Disable Update
                               1 - Enable Update
     6  -  6    SetGMIISpeed    1000_Mbps.
     5  -  5    SetMIISpeed    100 Mbps Speed
     4  -  4    InBand ReStartAn    0
     3  -  3    InBandAn ByPassEn    Bypass
     2  -  2    InBandAnEn    Enabled
     1  -  1    ForceLinkUp    Force Link UP
     0  -  0    ForceLinkDown    Dont_Force Link

--------------------------------------------------------------------------------------------------------------------*/



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern GT_STATUS bcat2MacGESpeedSet
(
    IN  GT_U8                devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  CPSS_PORT_SPEED_ENT  speed
);

extern GT_CHAR * CPSS_SPEED_2_STR
(
    CPSS_PORT_SPEED_ENT speed
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#define UNUSED_PARAM(x) x = x

typedef struct 
{
    GT_BOOL forceLinkUp;
    GT_BOOL forceLinkDown;
}appDemoForceLinkUpDownState_STC;


typedef struct 
{
    GT_BOOL              autoNegSpeedEn;
    GT_BOOL              autoNegDuplexEn;
    GT_BOOL              autoNegDuplexMode;
    GT_BOOL              autoNegFlowCtrlEn;
    GT_BOOL              autoNegFlowCtrlPauseAdv;
    GT_BOOL              autoNegFlowCtrlPauseAsmAdv;
    GT_BOOL              autoNegMacMasterModeEn;
}APPDEMO_AN_CONFIG_PARAMS_STC;

typedef struct 
{
    GT_BOOL                            portSyncOK;
    GT_BOOL                            syncFail10ms;
}APPDEMO_AN_SYNC_CHANGE_STC;

typedef struct 
{
    GT_U32  pearCodeword;
}APPDEMO_AN_COMPLETE_STC;

#define BAD_VALUE  ((GT_U32)~0)


typedef enum 
{
     AN_WA_Task_EVENT_PORT_AN_ENABLE_E  = 0
    ,AN_WA_Task_EVENT_PORT_AN_DISABLE_E
    ,AN_WA_Task_EVENT_PORT_AN_PARAM_SET_E
    ,AN_WA_Task_EVENT_INTR_SYNC_CHANGE_E
    ,AN_WA_Task_EVENT_INTR_AN_COMPLETE_E
    ,AN_WA_Task_EVENT_MAX_E                 /* never cross this boundary */
}AN_WA_TASK_EVENT_ENT;

typedef GT_STATUS (*AN_WA_TASK_EV_PROC)
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_VOID             *msgDataPtr
);

typedef struct 
{
    AN_WA_TASK_EVENT_ENT ev;
    AN_WA_TASK_EV_PROC   proc;
}AN_TASK_EV_PROC_STC;

typedef struct 
{
    AN_WA_TASK_EVENT_ENT ev;
    GT_U8                devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    union
    {
        APPDEMO_AN_CONFIG_PARAMS_STC anParams;
        APPDEMO_AN_SYNC_CHANGE_STC   syncCangeData;
        APPDEMO_AN_COMPLETE_STC      anCompleteData;
    }user_data;
}AN_WA_Task_Msg_STC;



typedef struct 
{
    GT_PHYSICAL_PORT_NUM portPhysNumber;
    GT_BOOL              antoNegEnable;
    GT_BOOL              autoNegSpeedEn;
    GT_BOOL              autoNegDuplexEn;
    GT_BOOL              autoNegDuplexMode;
    GT_BOOL              autoNegFlowCtrlEn;
    GT_BOOL              autoNegFlowCtrlPauseAdv;
    GT_BOOL              autoNegFlowCtrlPauseAsmAdv;
    GT_BOOL              interruptAnCompleteShallbeNoServed;
    GT_U32               prevANCompleteCodeWord;             /* code word from previous AN-Complete interrupt */	
}AN_WA_CONFIG_PARAMS_STC;

typedef struct 
{
    AN_WA_CONFIG_PARAMS_STC      autoNegConfigParam[CPSS_MAX_PORTS_NUM_CNS];
}AN_WA_PP_CONFIG_PARAMS_STC;

typedef struct
{
    GT_BOOL                taskCreated;
    GT_TASK                taskId;
    GT_MSGQ_ID             msgQId;
    #if (APPDEMO_AN_TASK_UNIT_TEST == 1)
        GT_SEM                 semId;
    #endif
    AN_WA_TASK_EV_PROC              eventProcArr[AN_WA_Task_EVENT_MAX_E];
    GT_BOOL                         APPDEMO_AN_TASK_DEBUG_PRINT;
    AN_WA_PP_CONFIG_PARAMS_STC     *ppConfigParamsArr[APP_DEMO_PP_CONFIG_SIZE_CNS];
}AN_WA_Task_STC;

AN_WA_Task_STC  g_anWa_TaskData;



/*-------------------------------------------------------*
 *   Task DB                                             *
 *-------------------------------------------------------*/

GT_STATUS appDemoAnConfigurationParamsDBPtrGet
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM       portNum,
    OUT AN_WA_CONFIG_PARAMS_STC **paramsPtrPtr
)
{
    GT_U32 maxPortNum;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(paramsPtrPtr);

    if (g_anWa_TaskData.ppConfigParamsArr[devNum] == NULL)
    {
        return GT_BAD_PARAM;
    }

    maxPortNum = sizeof(g_anWa_TaskData.ppConfigParamsArr[devNum]->autoNegConfigParam)/sizeof(g_anWa_TaskData.ppConfigParamsArr[devNum]->autoNegConfigParam[0]);
    if (portNum >= maxPortNum)
    {
        return GT_BAD_PARAM;
    }
    *paramsPtrPtr = &(g_anWa_TaskData.ppConfigParamsArr[devNum]->autoNegConfigParam[portNum]);
    return GT_OK;
}


GT_STATUS appDemoAnConfigurationDBClear
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC * anConfigPtr;

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&anConfigPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    anConfigPtr->portPhysNumber    = BAD_VALUE;
    anConfigPtr->antoNegEnable     = GT_FALSE;
    anConfigPtr->autoNegDuplexEn   = GT_FALSE;
    anConfigPtr->autoNegFlowCtrlEn = GT_FALSE;
    anConfigPtr->autoNegSpeedEn    = GT_FALSE;
    anConfigPtr->prevANCompleteCodeWord = BAD_VALUE;
    anConfigPtr->interruptAnCompleteShallbeNoServed = GT_FALSE;


    return GT_OK;

}

GT_STATUS appDemoAnConfigurationParamsDBSet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN APPDEMO_AN_CONFIG_PARAMS_STC *paramsPtr
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC *anParamsDBPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&anParamsDBPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    anParamsDBPtr->portPhysNumber             = portNum;
    anParamsDBPtr->autoNegSpeedEn             = paramsPtr->autoNegSpeedEn;
    anParamsDBPtr->autoNegDuplexEn            = paramsPtr->autoNegDuplexEn;
    anParamsDBPtr->autoNegDuplexMode          = paramsPtr->autoNegDuplexMode;
    anParamsDBPtr->autoNegFlowCtrlEn          = paramsPtr->autoNegFlowCtrlEn;
    anParamsDBPtr->autoNegFlowCtrlPauseAdv    = paramsPtr->autoNegFlowCtrlPauseAdv;
    anParamsDBPtr->autoNegFlowCtrlPauseAsmAdv = paramsPtr->autoNegFlowCtrlPauseAsmAdv;
    return GT_OK;
}

GT_STATUS appDemoAnConfigurationParamsDBGet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    OUT APPDEMO_AN_CONFIG_PARAMS_STC *paramsPtr
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC *anParamsDBPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(paramsPtr);

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&anParamsDBPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    paramsPtr->autoNegSpeedEn             = anParamsDBPtr->autoNegSpeedEn;
    paramsPtr->autoNegDuplexEn            = anParamsDBPtr->autoNegDuplexEn; 
    paramsPtr->autoNegDuplexMode          = anParamsDBPtr->autoNegDuplexMode;
    paramsPtr->autoNegFlowCtrlEn          = anParamsDBPtr->autoNegFlowCtrlEn;
    paramsPtr->autoNegFlowCtrlPauseAdv    = anParamsDBPtr->autoNegFlowCtrlPauseAdv;
    paramsPtr->autoNegFlowCtrlPauseAsmAdv = anParamsDBPtr->autoNegFlowCtrlPauseAsmAdv;

    return GT_OK;
}

typedef enum 
{
     APPDEMO_AN_PORT_IS_AN_ENABLED_E  = 0
    ,APPDEMO_AN_PARAM_SPEED_AN_EN_E
    ,APPDEMO_AN_PARAM_DUPLEX_AN_EN_E
    ,APPDEMO_AN_PARAM_DUPLEX_MODE_E
    ,APPDEMO_AN_PARAM_FLOW_CTRL_AN_EN_E
    ,APPDEMO_AN_PARAM_FLOW_CTRL_AN_PASUE_ADV_E
    ,APPDEMO_AN_PARAM_FLOW_CTRL_AN_PASUE_ASM_ADV_E
}APPDEMO_AN_PARAM_ENT;

GT_STATUS appDemoAnConfigurationDBSingleParamSet
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN APPDEMO_AN_PARAM_ENT type,
    IN GT_BOOL state
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC *anParamsDBPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&anParamsDBPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    switch (type)
    {
        case APPDEMO_AN_PORT_IS_AN_ENABLED_E:               anParamsDBPtr->antoNegEnable               = state; break;
        case APPDEMO_AN_PARAM_SPEED_AN_EN_E:                anParamsDBPtr->autoNegSpeedEn              = state; break;
        case APPDEMO_AN_PARAM_DUPLEX_AN_EN_E:               anParamsDBPtr->autoNegDuplexEn             = state; break;
        case APPDEMO_AN_PARAM_DUPLEX_MODE_E:                anParamsDBPtr->autoNegDuplexMode           = state; break;
        case APPDEMO_AN_PARAM_FLOW_CTRL_AN_EN_E:            anParamsDBPtr->autoNegFlowCtrlEn           = state; break;
        case APPDEMO_AN_PARAM_FLOW_CTRL_AN_PASUE_ADV_E:     anParamsDBPtr->autoNegFlowCtrlPauseAdv     = state; break;
        case APPDEMO_AN_PARAM_FLOW_CTRL_AN_PASUE_ASM_ADV_E: anParamsDBPtr->autoNegFlowCtrlPauseAsmAdv  = state; break;
        default:
        {
            return GT_BAD_PARAM;
        }

    }
    return GT_OK;
}

GT_STATUS appDemoAnConfigurationDBSingleParamGet
(
    IN  GT_U8   devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN  APPDEMO_AN_PARAM_ENT type,
    OUT GT_BOOL *statePtr
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC *anParamsDBPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&anParamsDBPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    switch (type)
    {
        case APPDEMO_AN_PORT_IS_AN_ENABLED_E:               *statePtr = anParamsDBPtr->antoNegEnable              ; break;
        case APPDEMO_AN_PARAM_SPEED_AN_EN_E:                *statePtr = anParamsDBPtr->autoNegSpeedEn             ; break;
        case APPDEMO_AN_PARAM_DUPLEX_AN_EN_E:               *statePtr = anParamsDBPtr->autoNegDuplexEn            ; break;
        case APPDEMO_AN_PARAM_DUPLEX_MODE_E:                *statePtr = anParamsDBPtr->autoNegDuplexMode          ; break;
        case APPDEMO_AN_PARAM_FLOW_CTRL_AN_EN_E:            *statePtr = anParamsDBPtr->autoNegFlowCtrlEn          ; break;
        case APPDEMO_AN_PARAM_FLOW_CTRL_AN_PASUE_ADV_E:     *statePtr = anParamsDBPtr->autoNegFlowCtrlPauseAdv    ; break;
        case APPDEMO_AN_PARAM_FLOW_CTRL_AN_PASUE_ASM_ADV_E: *statePtr = anParamsDBPtr->autoNegFlowCtrlPauseAsmAdv ; break;

        default:
        {
            return GT_BAD_PARAM;
        }
    }
    return GT_OK;
}


GT_STATUS appDemoAnConfigurationDBMac2PhysPortClear
(
    IN GT_U8   devNum,
    IN GT_PHYSICAL_PORT_NUM portNum 
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC *anParamsDBPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&anParamsDBPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    anParamsDBPtr->portPhysNumber = BAD_VALUE;
    return GT_OK;

}



/*-------------------------------------------------------------*/

GT_STATUS appDemoAnConfigurationDBANCompleteShallNotBeServedSet
(
    IN GT_U8                 devNum,
    IN GT_PHYSICAL_PORT_NUM  portNum,
    IN GT_BOOL               state,
    IN GT_U32                codeword	
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC *paramsPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&paramsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    paramsPtr->interruptAnCompleteShallbeNoServed = state;
    paramsPtr->prevANCompleteCodeWord = codeword;
    return GT_OK;
}

GT_STATUS appDemoAnConfigurationDBANCompleteShallNotBeServedGet
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  portNum,
    OUT GT_BOOL              *statePtr,
    OUT GT_U32               *codewordPtr
)
{
    GT_STATUS rc;
    AN_WA_CONFIG_PARAMS_STC *paramsPtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);
    CPSS_NULL_PTR_CHECK_MAC(codewordPtr);
    CPSS_NULL_PTR_CHECK_MAC(statePtr);

    rc = appDemoAnConfigurationParamsDBPtrGet(devNum,portNum,/*OUT*/&paramsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    *codewordPtr = paramsPtr->prevANCompleteCodeWord;
    *statePtr = paramsPtr->interruptAnCompleteShallbeNoServed;
    return GT_OK;
}



/*--------------------------------------------------*
 *  AN interrupt per port Enable/disable            *
 *       AN_WA_AutoNegInterruptsStateSet()          *
 *       AN_WA_PortAutoNegInterruptsStateSet()      *
 *--------------------------------------------------*/
#ifdef  APPDEMO_AN_WA_USE_INTR
    GT_STATUS AN_WA_AutoNegInterruptsStateSet
    (
        GT_U8 devNum,
        IN CPSS_EVENT_MASK_SET_ENT state
    )
    {
        GT_STATUS rc;

        if (state != CPSS_EVENT_MASK_E && state != CPSS_EVENT_UNMASK_E)
        {
            return GT_BAD_PARAM;
        }

        rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_PORT_SYNC_STATUS_CHANGED_E, state);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_PORT_AN_COMPLETED_E, state);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }


    GT_STATUS AN_WA_MacAutoNegInterruptsStateSet
    (
        IN GT_U8 devNum,
        IN GT_U32 portMacNum,
        IN CPSS_EVENT_MASK_SET_ENT state
    )
    {
        GT_STATUS rc;

        if (state != CPSS_EVENT_MASK_E && state != CPSS_EVENT_UNMASK_E)
        {
            return GT_BAD_PARAM;
        }

        rc = cpssEventDeviceMaskWithEvExtDataSet(devNum, CPSS_PP_PORT_SYNC_STATUS_CHANGED_E, portMacNum, state);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssEventDeviceMaskWithEvExtDataSet(devNum, CPSS_PP_PORT_AN_COMPLETED_E, portMacNum, state);
        if (rc != GT_OK)
        {
            return rc;
        }
        return GT_OK;
    }

#endif

GT_STATUS AN_WA_PortAutoNegInterruptsStateByMacSet
(
    IN GT_U8   devNum,
    IN GT_U32  portMacNum,
    IN CPSS_EVENT_MASK_SET_ENT state
)
{
    GT_STATUS rc;

    if (state != CPSS_EVENT_MASK_E && state != CPSS_EVENT_UNMASK_E)
    {
        return GT_BAD_PARAM;
    }

    rc = cpssEventDeviceMaskWithEvExtDataSet(devNum, CPSS_PP_PORT_SYNC_STATUS_CHANGED_E, portMacNum, state);
    if (rc != GT_OK)
    {
        return rc;
    }
    rc = cpssEventDeviceMaskWithEvExtDataSet(devNum, CPSS_PP_PORT_AN_COMPLETED_E, portMacNum, state);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


GT_STATUS AN_WA_PortAutoNegInterruptsStateSet
(
    IN GT_U8 devNum,
    IN GT_PHYSICAL_PORT_NUM portNum,
    IN CPSS_EVENT_MASK_SET_ENT state
)
{
    GT_STATUS rc;
    CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;

    rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum, portNum, /*OUT*/&portMapShadow);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (portMapShadow.valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }


    rc = AN_WA_PortAutoNegInterruptsStateByMacSet(devNum,portMapShadow.portMap.macNum,state);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}




/*--------------------------------------------------*
 *  Force Link-Up / ForceLinkDown                   *
 *--------------------------------------------------*/

typedef struct 
{
    GT_BOOL forceLinkDown;
    GT_BOOL forceLinkUp;
}APPDEMO_ForceLinkUpLinkDown_STC;

#define PRV_CPSS_DXCH_PORT_MAC_FORCE_LINK_DOWN_OFFS_CNS   0 
#define PRV_CPSS_DXCH_PORT_MAC_FORCE_LINK_UP_OFFS_CNS     1 


GT_STATUS BobKAutoNegTask_ForceLinkUpLinkDownSet
(
    GT_U8  devNum,
    GT_PHYSICAL_PORT_NUM portNum,
    APPDEMO_ForceLinkUpLinkDown_STC *linkStatePtr
)
{
    GT_STATUS rc;
    GT_U32 portMacNum;
    GT_U32 regAddr;
    GT_U32 fldData;

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return GT_NOT_SUPPORTED;
    }

    fldData = (linkStatePtr->forceLinkUp << 1) | linkStatePtr->forceLinkDown;
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 
                            PRV_CPSS_DXCH_PORT_MAC_FORCE_LINK_DOWN_OFFS_CNS, 
                            2, 
                            fldData);
    if (rc != GT_OK)
    {
        return rc;
    }
    return rc;
}


GT_STATUS BobKAutoNegTask_ForceLinkUpLinkDownGet
(
    IN  GT_U8  devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    OUT APPDEMO_ForceLinkUpLinkDown_STC *linkStatePtr
)
{
    GT_STATUS rc;
    GT_U32 portMacNum;
    GT_U32 regAddr;
    GT_U32 fldData;

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);
    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return GT_NOT_SUPPORTED;
    }

    fldData = (linkStatePtr->forceLinkUp << 1) | linkStatePtr->forceLinkDown;
    rc = prvCpssDrvHwPpGetRegField(devNum, regAddr, 
                            PRV_CPSS_DXCH_PORT_MAC_FORCE_LINK_DOWN_OFFS_CNS, 
                            2, 
                            &fldData);
    if (rc != GT_OK)
    {
        return rc;
    }

    linkStatePtr->forceLinkDown = (GT_BOOL)((fldData >> PRV_CPSS_DXCH_PORT_MAC_FORCE_LINK_DOWN_OFFS_CNS) & 0x1);
    linkStatePtr->forceLinkUp   = (GT_BOOL)((fldData >> PRV_CPSS_DXCH_PORT_MAC_FORCE_LINK_UP_OFFS_CNS)   & 0x1);
    return rc;

}



#define PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_OFFS_CNS    PRV_CPSS_DXCH_AN_CONF_FULL_DUPLEX_MODE_OFFS_CNS
#define PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_LEN_CNS      1

GT_STATUS BobKAutoNegTask_SetDuplexMode
(
    GT_U8 devNum,
    GT_PHYSICAL_PORT_NUM portNum,
    GT_BOOL isFullduplex
)
{
    GT_STATUS rc;
    GT_U32 portMacNum;
    GT_U32 duplexModeRegAddr;
    GT_U32 fldData;

    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&duplexModeRegAddr);
    if(duplexModeRegAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return GT_NOT_SUPPORTED;
    }

    fldData = isFullduplex; /* 1 -- full duplex, 0 - half duplex */
    rc = prvCpssDrvHwPpSetRegField(devNum, duplexModeRegAddr, 
                            PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_OFFS_CNS, 
                            PRV_CPSS_DXCH_PORT_MAC_DUPLEX_FLD_LEN_CNS, 
                            fldData);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}

#define PRV_CPSS_DXCH_PORT_MAC_REG4_FC_EN_TX_OFFS_CNS  4
#define PRV_CPSS_DXCH_PORT_MAC_REG4_FC_EN_RX_OFFS_CNS  3

GT_STATUS BobKAutoNegTask_FcEnRxTxSet
(
    GT_U8 devNum,
    GT_PHYSICAL_PORT_NUM portNum,
    GT_BOOL stateRx,
    GT_BOOL stateTx
)
{
    GT_STATUS rc;
    GT_U32 portMacNum;
    GT_U32 regAddr;
    GT_U32 fldData;


    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    PRV_CPSS_DXCH_PORT_MAC_CTRL4_REG_MAC(devNum,portMacNum,PRV_CPSS_PORT_GE_E,&regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return GT_NOT_SUPPORTED;
    }

    fldData = stateRx | (stateTx << 1);
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 
                            PRV_CPSS_DXCH_PORT_MAC_REG4_FC_EN_RX_OFFS_CNS, 
                            2, 
                            fldData);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


#define PRV_CPSS_DXCH_AN_CONF_PAUSE_ADV_OFFS_CNS              9
#define PRV_CPSS_DXCH_AN_CONF_PAUSE_ASM_ADV_OFFS_CNS         10
#define PRV_CPSS_DXCH_AN_CONF_AN_FLOW_CTRL_EN_OFFS_CNS       11

GT_STATUS BobKAutoNegTask_AnFcEnPauseAdvAsmPauseAdvSet
(
    GT_U8 devNum,
    GT_PHYSICAL_PORT_NUM portNum,
    GT_BOOL fcAnEn,
    GT_BOOL pauseAdv,
    GT_BOOL pauseAsmAdv
)
{
    GT_STATUS rc;
    GT_U32 portMacNum;
    GT_U32 regAddr;
    GT_U32 fldData;


    PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,portNum,portMacNum);
    PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portMacNum,&regAddr);

    if(regAddr == PRV_CPSS_SW_PTR_ENTRY_UNUSED)
    {
        return GT_NOT_SUPPORTED;
    }

    fldData = pauseAdv | (pauseAsmAdv << 1 | fcAnEn << 2);
    rc = prvCpssDrvHwPpSetRegField(devNum, regAddr, 
                            PRV_CPSS_DXCH_AN_CONF_PAUSE_ADV_OFFS_CNS, 
                            PRV_CPSS_DXCH_AN_CONF_AN_FLOW_CTRL_EN_OFFS_CNS - PRV_CPSS_DXCH_AN_CONF_PAUSE_ADV_OFFS_CNS +1, 
                            fldData);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}


GT_STATUS BobKAutoNegTask_AnParamsHWSet
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    APPDEMO_AN_CONFIG_PARAMS_STC * anParamsPtr
)
{
    GT_STATUS rc;
    CPSS_PORT_DUPLEX_ENT  dMode;
    rc = cpssDxChPortDuplexAutoNegEnableSet(devNum,physPortNum,anParamsPtr->autoNegDuplexEn);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortSpeedAutoNegEnableSet(devNum,physPortNum,anParamsPtr->autoNegSpeedEn);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (anParamsPtr->autoNegDuplexMode == GT_TRUE)
    {
        dMode = CPSS_PORT_FULL_DUPLEX_E;
    }
    else
    {
        dMode = CPSS_PORT_HALF_DUPLEX_E;
    }
    rc = cpssDxChPortDuplexModeSet(devNum,physPortNum,dMode);
    if (rc != GT_OK)
    {
        return rc;
    }
    /* 
    rc = cpssDxChPortFlowCntrlAutoNegEnableSet(devNum,physPortNum,anParamsPtr->autoNegFlowCtrlEn,anParamsPtr->autoNegFlowCtrlPauseAdv);
    if (rc != GT_OK)
    {
        return rc;
    }
    */
    rc = BobKAutoNegTask_AnFcEnPauseAdvAsmPauseAdvSet(devNum,physPortNum,anParamsPtr->autoNegFlowCtrlEn,anParamsPtr->autoNegFlowCtrlPauseAdv,anParamsPtr->autoNegFlowCtrlPauseAsmAdv);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}


GT_STATUS BobKAutoNegTask_ProcUpon_PortAnEnable
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_VOID             *msgDataPtr
)
{
    GT_STATUS rc;
    APPDEMO_AN_CONFIG_PARAMS_STC * anParamsPtr;
    APPDEMO_AN_CONFIG_PARAMS_STC anParamsHW;
    static CPSS_DXCH_DETAILED_PORT_MAP_STC portMap;
    CPSS_DXCH_DETAILED_PORT_MAP_STC *portMapPtr = &portMap;


    anParamsPtr = (APPDEMO_AN_CONFIG_PARAMS_STC *)msgDataPtr;

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    EVENT An Enable() : port %d (speedAn = %d duplexAn = %d duplexMode = %d fcAn = %d pauseAdv = %d pauseAsmAdv = %d)",physPortNum
            ,anParamsPtr->autoNegSpeedEn
            ,anParamsPtr->autoNegDuplexEn
            ,anParamsPtr->autoNegDuplexMode
            ,anParamsPtr->autoNegFlowCtrlEn
            ,anParamsPtr->autoNegFlowCtrlPauseAdv
            ,anParamsPtr->autoNegFlowCtrlPauseAsmAdv);
    }

    rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum,physPortNum,/*OUT*/portMapPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (portMapPtr->valid == GT_FALSE)
    {
        return rc;
    }
    if (portMapPtr->portMap.mappingType != CPSS_DXCH_PORT_MAPPING_TYPE_ETHERNET_MAC_E)
    {
        return GT_BAD_PARAM;
    }

    /*------------------------------*
     * configure DB                 *
     *     -- AN params             *
     *     -- port mac2phys portNum *
     *     -- AN enable             *
     *------------------------------*/
    rc = appDemoAnConfigurationParamsDBSet(devNum,physPortNum,anParamsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoAnConfigurationDBSingleParamSet(devNum,physPortNum,APPDEMO_AN_PORT_IS_AN_ENABLED_E,GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    /*---------------------------------*
     * configure HW  (all disabled!!!) *
     *---------------------------------*/
    anParamsHW.autoNegDuplexEn   = GT_FALSE;
    anParamsHW.autoNegFlowCtrlEn = GT_FALSE;
    anParamsHW.autoNegSpeedEn    = GT_FALSE;
    anParamsHW.autoNegDuplexMode          = anParamsPtr->autoNegDuplexMode;
    anParamsHW.autoNegFlowCtrlPauseAdv    = anParamsPtr->autoNegFlowCtrlPauseAdv;
    anParamsHW.autoNegFlowCtrlPauseAsmAdv = anParamsPtr->autoNegFlowCtrlPauseAsmAdv;

    rc = BobKAutoNegTask_AnParamsHWSet(devNum,physPortNum,&anParamsHW);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* set master mode at MAC */

    rc = prvCpssDxChBobcat2PortGigaMacAnMasterModeSet(devNum,physPortNum,anParamsPtr->autoNegMacMasterModeEn);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* enable inBand AN */
    rc = cpssDxChPortInbandAutoNegEnableSet(devNum,physPortNum,GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS BobKAutoNegTask_ProcUpon_PortAnDisable
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_VOID             *msgDataPtr
)
{
    GT_STATUS rc;

    UNUSED_PARAM(msgDataPtr);

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    EVENT An Disable() : port %d ",physPortNum);
    }

    rc = appDemoAnConfigurationDBSingleParamSet(devNum,physPortNum,APPDEMO_AN_PORT_IS_AN_ENABLED_E,GT_FALSE);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* disable inBand AN */
    rc = cpssDxChPortInbandAutoNegEnableSet(devNum,physPortNum,GT_FALSE);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = appDemoAnConfigurationDBMac2PhysPortClear(devNum,physPortNum);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}



/*-------------------------------------------------------*
 *  Process EVENT PARAM SET                              *
 *-------------------------------------------------------*/
GT_STATUS BobKAutoNegTask_ProcUpon_ParamSet
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_VOID             *msgDataPtr
)
{
    /*----------------------------------------------------*
     *  1. Param TRUE
     *  App          AN-Task            DB        HW
     *   | ParamsSet   |                   |         |
     *   |------------>| TRUE              |         |
     *   |             |------------------>|         |
     *   |             | FALSE             |         |
     *   |             |-------------------|-------->|
     *
     *  2. Param FALSE
     *  App          AN-Task            DB        HW
     *   | ParamsSet   |                   |         |
     *   |------------>| FALSE             |         |
     *   |             |------------------>|         |
     *   |             | FALSE             |         |
     *   |             |-------------------|-------->|
     *
     *----------------------------------------------------*/
    GT_STATUS rc;
    APPDEMO_AN_CONFIG_PARAMS_STC * anParamsPtr;
    APPDEMO_AN_CONFIG_PARAMS_STC anParamsHW;

    anParamsPtr = (APPDEMO_AN_CONFIG_PARAMS_STC *)msgDataPtr;

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    EVENT ParamSet() : port %d (speedAn = %d duplexAnEn = %d duplexMode = %d fcAnEn = %d pauseAdv = %d pauseAsmAdv = %d)",physPortNum
                                        ,anParamsPtr->autoNegSpeedEn
                                        ,anParamsPtr->autoNegDuplexEn
                                        ,anParamsPtr->autoNegDuplexMode
                                        ,anParamsPtr->autoNegFlowCtrlEn
                                        ,anParamsPtr->autoNegFlowCtrlPauseAdv
                                        ,anParamsPtr->autoNegFlowCtrlPauseAsmAdv);
    }

    /* configure DB */
    rc = appDemoAnConfigurationParamsDBSet(devNum,physPortNum,anParamsPtr);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* configure HW  (all disabled!!!) */
    anParamsHW.autoNegDuplexEn   = GT_FALSE;
    anParamsHW.autoNegFlowCtrlEn = GT_FALSE;
    anParamsHW.autoNegSpeedEn    = GT_FALSE;
    anParamsHW.autoNegDuplexMode          = anParamsPtr->autoNegDuplexMode;
    anParamsHW.autoNegFlowCtrlPauseAdv    = anParamsPtr->autoNegFlowCtrlPauseAdv;
    anParamsHW.autoNegFlowCtrlPauseAsmAdv = anParamsPtr->autoNegFlowCtrlPauseAsmAdv;

    rc = BobKAutoNegTask_AnParamsHWSet(devNum,physPortNum,&anParamsHW);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssDxChPortInbandAutoNegRestart(devNum,physPortNum);
    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

/*-------------------------------------------------------*
 *  Process EVENT SYNC_CHANGE                            *
 *-------------------------------------------------------*/
GT_STATUS BobKAutoNegTask_ProcUpon_SyncChange
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_VOID             *msgDataPtr
)
{
    static appDemoForceLinkUpDownState_STC forceLinkUpDownState_macType_x_InBandAnEn[PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_MAX][2] =   
    {                    /* InBandAnEn  =        GT_FALSE,               GT_TRUE */
                                         /* linkUp,  LinkDown         linkUp, LinkDown  */
        /* macType == SGMII = 0 */      { { GT_TRUE,  GT_FALSE },   { GT_FALSE, GT_TRUE } } 
        /* macType == 1000BaseX = 1 */ ,{ { GT_FALSE, GT_FALSE },   { GT_FALSE, GT_TRUE } }
    };


    GT_STATUS rc;
    GT_BOOL SyncOK,SyncFail10Ms;

    GT_BOOL inBandAnEn;
    PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_ENT portType;
    APPDEMO_ForceLinkUpLinkDown_STC linkState;
    APPDEMO_AN_SYNC_CHANGE_STC * syncDataPtr;
    APPDEMO_AN_CONFIG_PARAMS_STC anParams;


    syncDataPtr = (APPDEMO_AN_SYNC_CHANGE_STC *)msgDataPtr;
    SyncOK       = syncDataPtr->portSyncOK;
    SyncFail10Ms = syncDataPtr->syncFail10ms;


    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    EVENT SyncChange() : port %d (portSyncOK = %d syncFail10ms = %d)",physPortNum
                                                   ,syncDataPtr->portSyncOK
                                                   ,syncDataPtr->syncFail10ms);
    }
    if (SyncOK == GT_TRUE && SyncFail10Ms == GT_TRUE)
    {
        /*----------------------------------------------------------------* 
         * configure HW:                                                  *
         *       PortAutoNegConfiguration <AnDuplexEn> = DISABLE          *
         *       PortAutoNegConfiguration <AnFCEn>     = DISABLE          *
         *       PortAutoNegConfiguration <AnSpeedEn>  = DISABLE          *
         *----------------------------------------------------------------*/
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n            Set PortAutoNegConfiguration <AnDuplexEn> = DISABLE");
            cpssOsPrintf("\n            Set PortAutoNegConfiguration <AnFCEn>     = DISABLE");
            cpssOsPrintf("\n            Set PortAutoNegConfiguration <AnSpeedEn>  = DISABLE");
        }

        /* configure HW  (all disabled!!!) */
        anParams.autoNegDuplexEn   = GT_FALSE;
        anParams.autoNegFlowCtrlEn = GT_FALSE;
        anParams.autoNegSpeedEn    = GT_FALSE;
        rc = BobKAutoNegTask_AnParamsHWSet(devNum,physPortNum,&anParams);
        if (rc != GT_OK)
        {
            return rc;
        }
        /* get inbandAnEn field */
        rc = cpssDxChPortInbandAutoNegEnableGet(devNum,physPortNum,/*OUT*/&inBandAnEn);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = prvCpssDxChBobcat2PortGigaMacTypeGet(devNum,physPortNum,&portType);
        if (rc != GT_OK)
        {
            return rc;
        }
       /*-------------------------------------------------------------------------------------------*
        *  Set [ForceLinkDown,ForceLinkUp] as function(PortType,InbandAnEn)                         *
        *                InBandAnEn                GT_FALSE,               GT_TRUE                  *
        *       macType                        linkUp,  LinkDown         linkUp, LinkDown           *
        *   macType == SGMII = 0          { { GT_TRUE,  GT_FALSE },   { GT_FALSE, GT_TRUE } }       *
        *   macType == 1000BaseX = 1     ,{ { GT_FALSE, GT_FALSE },   { GT_FALSE, GT_TRUE } }       *
        *-------------------------------------------------------------------------------------------*/
        linkState.forceLinkDown = forceLinkUpDownState_macType_x_InBandAnEn[portType][inBandAnEn].forceLinkDown;
        linkState.forceLinkUp   = forceLinkUpDownState_macType_x_InBandAnEn[portType][inBandAnEn].forceLinkUp;

        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        inBandAnEn = %d SGMII port = %d",inBandAnEn,portType);
            cpssOsPrintf("\n            Set linkState.forceLinkDown = %d", linkState.forceLinkDown);
            cpssOsPrintf("\n            Set linkState.forceLinkUp   = %d", linkState.forceLinkUp  );
        }

        rc = BobKAutoNegTask_ForceLinkUpLinkDownSet(devNum,physPortNum,&linkState);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        linkState.forceLinkDown = GT_TRUE;
        linkState.forceLinkUp   = GT_FALSE;

        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        Set linkState.forceLinkDown = %d", linkState.forceLinkDown);
            cpssOsPrintf("\n        Set linkState.forceLinkUp   = %d", linkState.forceLinkUp  );
        }

        rc = BobKAutoNegTask_ForceLinkUpLinkDownSet(devNum,physPortNum,&linkState);
        if (rc != GT_OK)
        {
            return rc;
        }

    }
    return GT_OK;
}


typedef struct 
{
    GT_BOOL             ANAck;
    CPSS_PORT_SPEED_ENT speed;
    GT_BOOL             fullDuplex;
    GT_BOOL             fcEnTx;
    GT_BOOL             fcEnRx;
    GT_BOOL             linkStatus;
    GT_BOOL             pauseAdv;
    GT_BOOL             pauseAsmAdv;
}APPDEMO_AN_LINK_SGMII_PARTNER_PARAMS_STC;


#define PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_FC_Rx_EN_OFFS_CNS    8
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_FC_Tx_EN_OFFS_CNS    9

#define PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_SPEED_OFFS_CNS       10
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_SPEED_LEN_CNS        2

#define PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_FULL_DUPLEX_OFFS_CNS 12
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_AN_ACK_OFFS_CNS      14
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_LINK_STATUS_OFFS_CNS 15



#define PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_FULL_DUPLEX_OFFS_CNS  5
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_HALF_DUPLEX_OFFS_CNS  6

#define PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_PAUSE_ADV_OFFS_CNS     7
#define PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_PAUSE_ASM_ADV_OFFS_CNS 8


#define PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_AN_ACK_OFFS_CNS      14
/*#define PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_LINK_STATUS_OFFS_CNS 15 */

typedef enum 
{
     APPDEMO_AN_DECODE_TYPE_SGMII_E
    ,APPDEMO_AN_DECODE_TYPE_1000BASEX_E
}APPDEMO_AN_DECODE_TYPE_ENT;

GT_STATUS AppDemoLinkPartnerCodeWordDecode
(
    IN  GT_U32 codeword,
    IN  APPDEMO_AN_DECODE_TYPE_ENT decodeType,
    OUT APPDEMO_AN_LINK_SGMII_PARTNER_PARAMS_STC *anLinkPartnerParamPtr
)
{
    cpssOsMemSet(anLinkPartnerParamPtr,0,sizeof(*anLinkPartnerParamPtr));
    if (decodeType == APPDEMO_AN_DECODE_TYPE_SGMII_E)
    {
        anLinkPartnerParamPtr->ANAck       = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_AN_ACK_OFFS_CNS)      & 0x1);
        anLinkPartnerParamPtr->speed       = (CPSS_PORT_SPEED_ENT)((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_SPEED_OFFS_CNS)       & 0x3);
        anLinkPartnerParamPtr->fullDuplex  = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_FULL_DUPLEX_OFFS_CNS) & 0x1);
        anLinkPartnerParamPtr->linkStatus  = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_LINK_STATUS_OFFS_CNS) & 0x1);
        anLinkPartnerParamPtr->fcEnRx      = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_FC_Rx_EN_OFFS_CNS)    & 0x1);
        anLinkPartnerParamPtr->fcEnTx      = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_FC_Tx_EN_OFFS_CNS)    & 0x1);
        anLinkPartnerParamPtr->pauseAdv    = GT_FALSE;
        anLinkPartnerParamPtr->pauseAsmAdv = GT_FALSE;
    }
    else
    {
        anLinkPartnerParamPtr->ANAck       = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_AN_ACK_OFFS_CNS)      & 0x1);
        anLinkPartnerParamPtr->fullDuplex  = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_FULL_DUPLEX_OFFS_CNS) & 0x1);
        anLinkPartnerParamPtr->speed       = CPSS_PORT_SPEED_1000_E;
        anLinkPartnerParamPtr->pauseAdv    = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_PAUSE_ADV_OFFS_CNS    ) & 0x1);
        anLinkPartnerParamPtr->pauseAsmAdv = (GT_BOOL)            ((codeword >> PRV_CPSS_DXCH_AN_LINK_PARTNER_1000BASEX_PAUSE_ASM_ADV_OFFS_CNS) & 0x1);
        
    }
    return GT_OK;
}






GT_STATUS BobKAutoNegTask_ProcUpon_AN_Complete_PortType_SGMII_SlaveMode
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_U32               codeword
)
{
   /*----------------------------------------------------------------------------*
    *   If(ApplicationReq<AnSpeedEn> == Enabled)                                 *
    *   {                                                                        *
    *          SetGMIISpeed = (ReadRxConfigReg[11:10] == 2'b10) ? 1 : 0    1000M *
    *          SetMIISpeed = (ReadRxConfigReg[11:10] == 2'b00) ? 0 : 1       10M *
    *   }                                                                        *
    *                                                                            *
    *   If(ApplicationReq<AnDuplexEn> == Enabled)                                *
    *   {                                                                        *
    *      SetFullDuplex = ReadRxConfigReg[12]                                   *
    *   }                                                                        *
    *                                                                            *
    *   // Standard not define SGMII AN on Flow Control                          *
    *   If(ApplicationReq<AnFCEn> == Enabled) {                                  *
    *       SetFcEnTx = (ReadRxConfigReg[12] == 1) ?    ReadRxConfigReg[9] : 0   *
    *        SetFcEnRx = (ReadRxConfigReg[12] == 1) ? ReadRxConfigReg[8] : 0     *
    *   }                                                                        *
    *                                                                            *
    *   ForceLinkDown = ~ReadRxConfigReg[15]                                     *
    *   ForceLinkUp = ReadRxConfigReg[15]                                        *
    *----------------------------------------------------------------------------*/
    GT_STATUS rc;
    static APPDEMO_AN_LINK_SGMII_PARTNER_PARAMS_STC anLinkPartnerParam;
    APPDEMO_AN_CONFIG_PARAMS_STC             appReqAnParams;
    APPDEMO_ForceLinkUpLinkDown_STC          linkState;

    rc = AppDemoLinkPartnerCodeWordDecode(codeword,APPDEMO_AN_DECODE_TYPE_SGMII_E,/*OUT*/&anLinkPartnerParam);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n        codeword");
        cpssOsPrintf("\n              ANAck      = %d",anLinkPartnerParam.ANAck      );
        cpssOsPrintf("\n              speed      = %s",CPSS_SPEED_2_STR(anLinkPartnerParam.speed));
        cpssOsPrintf("\n              fullDuplex = %d",anLinkPartnerParam.fullDuplex );
        cpssOsPrintf("\n              linkStatus = %d",anLinkPartnerParam.linkStatus );
        cpssOsPrintf("\n              fcEnRx     = %d",anLinkPartnerParam.fcEnRx     );
        cpssOsPrintf("\n              fcEnTx     = %d",anLinkPartnerParam.fcEnTx     );
    }


    rc = appDemoAnConfigurationParamsDBGet(devNum,physPortNum,/*OUT*/&appReqAnParams);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (appReqAnParams.autoNegSpeedEn == GT_TRUE)
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        Set speed %s ...",CPSS_SPEED_2_STR(anLinkPartnerParam.speed));
        }
        rc = bcat2MacGESpeedSet(devNum,physPortNum,anLinkPartnerParam.speed);
        if (rc != GT_OK)
        {
            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("Failed rc = %d",rc);
            }
            return rc;
        }
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("OK");
        }
    }

    if (appReqAnParams.autoNegDuplexEn == GT_TRUE)
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        Set duplex %d ...",anLinkPartnerParam.fullDuplex);
        }
        rc = BobKAutoNegTask_SetDuplexMode(devNum,physPortNum,anLinkPartnerParam.fullDuplex);
        if (rc != GT_OK)
        {
            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("Failed rc = %d",rc);
            }
            return rc;
        }
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("OK");
        }
    }
    if (appReqAnParams.autoNegFlowCtrlEn == GT_TRUE) 
    {
        GT_BOOL stateRx = GT_FALSE;
        GT_BOOL stateTx = GT_FALSE;
        if (anLinkPartnerParam.fullDuplex == GT_TRUE)
        {
            stateRx = anLinkPartnerParam.fcEnRx;
            stateTx = anLinkPartnerParam.fcEnTx;
        }
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        Set FlowCntrl Rx = %d Tx = %d ...",stateRx,stateTx);
        }
        rc = BobKAutoNegTask_FcEnRxTxSet(devNum,physPortNum,stateRx,stateTx);
        if (rc != GT_OK)
        {
            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("Failed rc = %d",rc);
            }
            return rc;
        }
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("OK");
        }
    }
    linkState.forceLinkDown = (GT_BOOL)!anLinkPartnerParam.linkStatus;
    linkState.forceLinkUp   = anLinkPartnerParam.linkStatus;

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n        Set forceLinkDown = %d forceLinkUp = %d ...",linkState.forceLinkDown,linkState.forceLinkUp);
    }

    rc = BobKAutoNegTask_ForceLinkUpLinkDownSet(devNum,physPortNum,&linkState);
    if (rc != GT_OK)
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("Failed rc = %d",rc);
        }
        return rc;
    }
    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("OK");
    }
    return GT_OK;
}

GT_STATUS BobKAutoNegTask_ProcUpon_AN_Complete_PortType_SGMII_MasterMode
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_U32               codeword
)
{
    GT_STATUS                                rc;
    APPDEMO_ForceLinkUpLinkDown_STC          linkState;

    if (codeword == 0x4001)
    {
        /*--------------------------------* 
         * forceLinkDown = Don'tForce     *
         * forceLinkUp   = Force          *
         *--------------------------------*/
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n            forceLinkDown = 0 forceLinkUp = 1");
        }

        linkState.forceLinkDown = GT_FALSE;
        linkState.forceLinkUp   = GT_TRUE;
        rc = BobKAutoNegTask_ForceLinkUpLinkDownSet(devNum,physPortNum,&linkState);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n            Restart AN");
        }

        rc = cpssDxChPortInbandAutoNegRestart(devNum,physPortNum);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

GT_STATUS BobKAutoNegTask_ProcUpon_AN_Complete_PortType_1000BaseX
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_U32               codeword
)
{
    /*--------------------------------------------------------------------*   
    *  SetGMIISpeed =  1                                                 *
    *                                                                    *
    *  If(ApplicationReq<AnDuplexEn> == Enabled)                         *
    *  {                                                                 *
    *     SetFullDuplex = ReadRxConfigReg[5]           FullDuplex        *
    *  }                                                                 *
    *                                                                    *
    *  If(ApplicationReq<AnFCEn> == Enabled)                             *
    *  {                                                                 *
    *       if( (ReadRxConfigReg[5] == 1) &&                             *
    *           ((!PauseAdv && PauseAsmAdv &&                            *
    *           ReadRxConfigReg[7] && ReadRxConfigReg[8])  ||            *
    *           (PauseAdv && ReadRxConfigReg[7])) )                      *
    *                         SetFcEnTx  = 1'b1;                         *
    *                       else                                         *
    *                        SetFcEnTx  = 1'b0;                          *
    *                                                                    *
    *       if( (ReadRxConfigReg[5] == 1) &&                             *
    *           ((PauseAdv && PauseAsmAdv &&                             *
    *           !ReadRxConfigReg[7] && ReadRxConfigReg[8])  ||           *
    *           (PauseAdv && ReadRxConfigReg[7])) )                      *
    *       {                                                            *
    *           SetFcEnRx  = 1'b1;                                       *
    *       }                                                            *
    *       else                                                         *
    *       {                                                            *
    *           SetFcEnRx  = 1'b0;                                       *
    *       }                                                            *
    *  }                                                                 *
    *                                                                    *
    *  ForceLinkDown = DontForce                                         *
    *  ForceLinkUp = DontForce                                           *
    *--------------------------------------------------------------------*/
    GT_STATUS                                rc;
    APPDEMO_AN_LINK_SGMII_PARTNER_PARAMS_STC anLinkPartnerParam;
    APPDEMO_AN_CONFIG_PARAMS_STC             appReqAnParams;
    APPDEMO_ForceLinkUpLinkDown_STC          linkState;

    rc = AppDemoLinkPartnerCodeWordDecode(codeword,APPDEMO_AN_DECODE_TYPE_1000BASEX_E,/*OUT*/&anLinkPartnerParam);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n        codeword");
        cpssOsPrintf("\n              ANAck       = %d",anLinkPartnerParam.ANAck       );
        cpssOsPrintf("\n              speed       = %s",CPSS_SPEED_2_STR(anLinkPartnerParam.speed));
        cpssOsPrintf("\n              fullDuplex  = %d",anLinkPartnerParam.fullDuplex  );
        cpssOsPrintf("\n              pauseAdv    = %d",anLinkPartnerParam.pauseAdv    );
        cpssOsPrintf("\n              pauseAsmAdv = %d",anLinkPartnerParam.pauseAsmAdv );
    }


    rc = appDemoAnConfigurationParamsDBGet(devNum,physPortNum,/*OUT*/&appReqAnParams);
    if (rc != GT_OK)
    {
        return rc;
    }


    if (appReqAnParams.autoNegDuplexEn == GT_TRUE)
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        set duplex mode = %d ...",anLinkPartnerParam.fullDuplex);
        }
        rc = BobKAutoNegTask_SetDuplexMode(devNum,physPortNum,anLinkPartnerParam.fullDuplex);
        if (rc != GT_OK)
        {
            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("Failed rc = %d",rc);
            }
            return rc;
        }
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("OK");
        }

    }
    if (appReqAnParams.autoNegFlowCtrlEn == GT_TRUE)
    {
        if (anLinkPartnerParam.fullDuplex == GT_TRUE)
        {
            typedef struct 
            {
                GT_BOOL stateTx;
                GT_BOOL stateRx;
            }FCRES_STC;
            static FCRES_STC fcEn_TxRx_Arr[16] = 
            {
                /*------------------------------------------------------------------------------------------*/
                /*     My                         Peer          ||                 Output                   */
                /* pauseAdv PauseAsmAdv   pauseAdv PauseAsmAdv  ||        stateTx               stateRx     */
                /*------------------------------------------------------------------------------------------*/
                /*  0        0              0       0           || */   { GT_FALSE      ,       GT_FALSE  }    
                /*  0        0              0       1           || */  ,{ GT_FALSE      ,       GT_FALSE  }    
                /*  0        0              1       0           || */  ,{ GT_FALSE      ,       GT_FALSE  }    
                /*  0        0              1       1           || */  ,{ GT_FALSE      ,       GT_FALSE  }    
                /* ---------------------------------------------||-------------------------------------------*/  
                /*  0        1              0       0           || */  ,{ GT_FALSE      ,       GT_FALSE  }    
                /*  0        1              0       1           || */  ,{ GT_FALSE      ,       GT_FALSE  }    
                /*  0        1              1       0           || */  ,{ GT_FALSE      ,       GT_FALSE  }    
                /*  0        1              1       1           || */  ,{  GT_TRUE      ,       GT_FALSE  }    
                /* ---------------------------------------------||-------------------------------------------*/
                /*  1        0              0       0           || */  ,{ GT_FALSE      ,       GT_FALSE  }   
                /*  1        0              0       1           || */  ,{ GT_FALSE      ,       GT_FALSE  }   
                /*  1        0              1       0           || */  ,{  GT_TRUE      ,        GT_TRUE  }   
                /*  1        0              1       1           || */  ,{  GT_TRUE      ,        GT_TRUE  }   
                /* ---------------------------------------------||-------------------------------------------*/
                /*  1        1              0       0           || */  ,{ GT_FALSE      ,       GT_FALSE  }   
                /*  1        1              0       1           || */  ,{ GT_FALSE      ,        GT_TRUE  }   
                /*  1        1              1       0           || */  ,{  GT_TRUE      ,        GT_TRUE  }   
                /*  1        1              1       1           || */  ,{  GT_TRUE      ,        GT_TRUE  }   
                /*-------------------------------------------------------------------------------------------*/
            };

            GT_BOOL my_pauseAdv;
            GT_BOOL my_pauseAsmAdv;
            GT_U32  idx;
            GT_BOOL stateRx;
            GT_BOOL stateTx;

            rc = prvCpssDxChPortAutoNegPauseAdvPauseAsmAdvGet(devNum,physPortNum,/*OUT*/&my_pauseAdv,/*OUT*/&my_pauseAsmAdv);
            if (rc != GT_OK)
            {
                return rc;
            }


            idx = 8*my_pauseAdv + 4*my_pauseAsmAdv + 2*anLinkPartnerParam.pauseAdv + anLinkPartnerParam.pauseAsmAdv;

            stateRx = fcEn_TxRx_Arr[idx].stateRx;
            stateTx = fcEn_TxRx_Arr[idx].stateTx;


            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("\n        Set FlowCntrl Rx = %d Tx = %d ...",stateRx,stateTx);
            }

            rc = BobKAutoNegTask_FcEnRxTxSet(devNum,physPortNum,stateRx,stateTx);
            if (rc != GT_OK)
            {
                if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
                {
                    cpssOsPrintf("Failed rc = %d",rc);
                }
                return rc;
            }
            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("OK");
            }
        }
    }


    linkState.forceLinkDown = GT_FALSE;
    linkState.forceLinkUp   = GT_FALSE;

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n        Set forceLinkDown = %d forceLinkUp = %d ...",linkState.forceLinkDown,linkState.forceLinkUp);
    }

    rc = BobKAutoNegTask_ForceLinkUpLinkDownSet(devNum,physPortNum,&linkState);
    if (rc != GT_OK)
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("Failed rc = %d",rc);
        }
        return rc;
    }
    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("OK");
    }

    return GT_OK;
}


/*-------------------------------------------------------*
 *  Process EVENT AN COMPLETE                            *
 *-------------------------------------------------------*/
GT_STATUS BobKAutoNegTask_ProcUpon_AN_Complete
(
    GT_U8  devNum,
    GT_PHYSICAL_PORT_NUM physPortNum,
    GT_VOID             *msgDataPtr
)
{
    GT_STATUS rc;
    APPDEMO_ForceLinkUpLinkDown_STC linkState;
    GT_BOOL forceLinkUp;
    APPDEMO_AN_COMPLETE_STC * anCompleteDataPtr;

    anCompleteDataPtr = (APPDEMO_AN_COMPLETE_STC*)msgDataPtr;

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    EVENT AN_Complete() : port %d (codeword = 0x%4x)",physPortNum
                                                    ,anCompleteDataPtr->pearCodeword);
    }


    rc = cpssDxChPortForceLinkPassEnableGet(devNum,physPortNum,/*OUT*/&forceLinkUp);
    if (rc != GT_OK)
    {
        goto restore_intr;
    }
    if (forceLinkUp == GT_TRUE)
    {
        /*--------------------------------* 
         * forceLinkDown = Force          *
         * forceLinkUp   = Don'tForce     *
         * restart AN                     *
         *--------------------------------*/
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        ForceLinkUp = TRUE");
        }

        linkState.forceLinkDown = GT_TRUE;
        linkState.forceLinkUp   = GT_FALSE;
        rc = BobKAutoNegTask_ForceLinkUpLinkDownSet(devNum,physPortNum,&linkState);
        if (rc != GT_OK)
        {
            goto restore_intr;
        }

        rc = cpssDxChPortInbandAutoNegRestart(devNum,physPortNum);
        if (rc != GT_OK)
        {
            goto restore_intr;
        }
    }
    else
    {
        GT_U32  codeword;
        GT_BOOL masterModeEn;
        PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_ENT portType;

        codeword = anCompleteDataPtr->pearCodeword;

        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n        ForceLinkUp = FALSE");
        }

        rc = prvCpssDxChBobcat2PortGigaMacTypeGet(devNum,physPortNum,/*OUT*/&portType);
        if (rc != GT_OK)
        {
            goto restore_intr;
        }
        if (portType == PRV_CPSS_DXCH_MAC_GE_PORT_TYPE_SGMII_E)
        {
            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("\n        portType = SGMII");
            }

            rc = prvCpssDxChBobcat2PortGigaMacAnMasterModeGet(devNum,physPortNum,/*OUT*/&masterModeEn);
            if (masterModeEn == GT_FALSE) /* SGMII-Type == MAC */
            {
                if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
                {
                    cpssOsPrintf("\n        SGMII type = PHY (slave Mode)");
                }

                rc = BobKAutoNegTask_ProcUpon_AN_Complete_PortType_SGMII_SlaveMode(devNum,physPortNum,codeword);
                if (rc != GT_OK)
                {
                     goto restore_intr;
                }
            }
            else
            {
                if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
                {
                    cpssOsPrintf("\n        SGMII type = MAC (?) (master mode)");
                }

                rc = BobKAutoNegTask_ProcUpon_AN_Complete_PortType_SGMII_MasterMode(devNum,physPortNum,codeword);
                if (rc != GT_OK)
                {
                    goto restore_intr;
                }
            }
        }
        else /* not SGMII port ===> 1000BaseX */
        {
            if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
            {
                cpssOsPrintf("\n        portType = 1000BaseX");
            }

            rc = BobKAutoNegTask_ProcUpon_AN_Complete_PortType_1000BaseX(devNum,physPortNum,codeword);
            if (rc != GT_OK)
            {
                goto restore_intr;
            }
        }
    }
    rc = GT_OK;
restore_intr:
    (GT_VOID)AN_WA_PortAutoNegInterruptsStateSet(devNum,physPortNum,CPSS_EVENT_UNMASK_E);
    return rc;
}

/*-------------------------------------------------------*
 *  AN WA Task                                           *
 *-------------------------------------------------------*/

AN_TASK_EV_PROC_STC anTask_EvProc_InitList[AN_WA_Task_EVENT_MAX_E] = 
{
      { AN_WA_Task_EVENT_PORT_AN_ENABLE_E,      BobKAutoNegTask_ProcUpon_PortAnEnable }
     ,{ AN_WA_Task_EVENT_PORT_AN_DISABLE_E,     BobKAutoNegTask_ProcUpon_PortAnDisable}
     ,{ AN_WA_Task_EVENT_PORT_AN_PARAM_SET_E,   BobKAutoNegTask_ProcUpon_ParamSet     }
     ,{ AN_WA_Task_EVENT_INTR_SYNC_CHANGE_E,    BobKAutoNegTask_ProcUpon_SyncChange   }
     ,{ AN_WA_Task_EVENT_INTR_AN_COMPLETE_E,    BobKAutoNegTask_ProcUpon_AN_Complete  }
};





GT_STATUS APPDEMO_AN_TASK_DEBUG_PRINT_Set(GT_U32 value)
{
    #if (APPDEMO_AN_TASK_DEBUG_PRINT_EN == 1)
        if (value == 0)
        {
            g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT = GT_FALSE;
        }
        else
        {
            g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT = GT_TRUE;
        }
        return GT_OK;
    #else
        UNUSED_PARAM(value);
        cpssOsPrint("\nSet APPDEMO_AN_TASK_DEBUG_PRINT_EN == 1\n");
        return GT_FAIL;
    #endif
}


static unsigned __TASKCONV AN_WA_Task
(
    GT_VOID * param
)
{
    GT_STATUS rc;
    AN_WA_Task_Msg_STC msg;
    GT_U32             msgSize;
    GT_PHYSICAL_PORT_NUM physPortNum;
    GT_U32               maxPortNum;


    maxPortNum = sizeof(g_anWa_TaskData.ppConfigParamsArr[0]->autoNegConfigParam)/sizeof(g_anWa_TaskData.ppConfigParamsArr[0]->autoNegConfigParam[0]);
    for (;;)
    {
        msgSize = (GT_U32)sizeof(msg);
        rc = osMsgQRecv(g_anWa_TaskData.msgQId,&msg,&msgSize,OS_MSGQ_WAIT_FOREVER);
        if (rc != GT_OK)
        {
            continue; /* nothing to do , something wrong */
        }

        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n    Get EVENT %d phys port %d",msg.ev, msg.portNum);
        }


        if (msg.portNum >= maxPortNum)  /* invalide mac number */
        {
            continue;
        }
        /* obtain physical port by mac number*/
        physPortNum = msg.portNum;

        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n    Proc EVENT %d port %d",msg.ev, physPortNum);
        }
        
        rc = (g_anWa_TaskData.eventProcArr[msg.ev])(msg.devNum,physPortNum,(GT_VOID*)&msg.user_data);
        if (rc != GT_OK)
        {
            continue; /* nothing to do , something wrong */
        }
        #if (APPDEMO_AN_TASK_UNIT_TEST == 1)
            osSemSignal(g_anWa_TaskData.semId);
        #endif
    }
    return GT_OK;
}


GT_STATUS AN_WA_Task_EventProcTblInit
(
    INOUT AN_WA_Task_STC * anWa_TaskDataPtr,
    IN AN_TASK_EV_PROC_STC * anTask_EvProc_InitList,
    IN GT_U32 size
)
{
    GT_U32 i;
    for (i = 0 ; i < sizeof(anWa_TaskDataPtr->eventProcArr)/sizeof(anWa_TaskDataPtr->eventProcArr[0]); i++)
    {
        anWa_TaskDataPtr->eventProcArr[i] = (AN_WA_TASK_EV_PROC)NULL;
    }
    for (i = 0 ; i < size; i++)
    {
        anWa_TaskDataPtr->eventProcArr[anTask_EvProc_InitList[i].ev] = anTask_EvProc_InitList[i].proc;
    }
    return GT_OK;
}


GT_STATUS AN_WA_Task_Create
(
    GT_VOID
)
{
    GT_STATUS rc;
    GT_U32    dbValueInitAnWaTask;
    GT_U8     dev;
    GT_PHYSICAL_PORT_NUM    portIdx;
    GT_U32    devidx;


    g_anWa_TaskData.taskCreated = GT_FALSE;
    g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT = GT_FALSE;

    rc = appDemoDbEntryGet("initAnWaTask", &dbValueInitAnWaTask);
    if (rc != GT_OK)   /* string not exists ---> return OK, but don't create task */
    {
        return GT_OK;
    }
    if (0 == dbValueInitAnWaTask)     /* string exists, but is 0 --> return OK, but don't create task */
    {
        return GT_OK;
    }

    for(devidx = SYSTEM_DEV_NUM_MAC(0) ; devidx < SYSTEM_DEV_NUM_MAC(appDemoPpConfigDevAmount);devidx++)
    {
        if(appDemoPpConfigList[devidx].valid == GT_FALSE)
        {
            continue;
        }

        dev = appDemoPpConfigList[devidx].devNum;
        /*-------------------------------------------------------*
         * WA is applicable just for BobK devices : Cetus/Caelum *
         *-------------------------------------------------------*/
        if( PRV_CPSS_DXCH_CETUS_CHECK_MAC(dev) || PRV_CPSS_DXCH_CAELUM_CHECK_MAC(dev))
        {
            /*--------------------------------------------* 
             *  allocate memeory for specific device      *
             *--------------------------------------------*/
            g_anWa_TaskData.ppConfigParamsArr[dev] = (AN_WA_PP_CONFIG_PARAMS_STC *)cpssOsMalloc(sizeof(AN_WA_PP_CONFIG_PARAMS_STC));

            for (portIdx = 0 ; portIdx < CPSS_MAX_PORTS_NUM_CNS; portIdx++)
            {
                rc = appDemoAnConfigurationDBClear(dev,portIdx);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

   /*-----------------------------------------*
    * init event --> proc table               *
    *-----------------------------------------*/
    rc = AN_WA_Task_EventProcTblInit(&g_anWa_TaskData,&anTask_EvProc_InitList[0],sizeof(anTask_EvProc_InitList)/sizeof(anTask_EvProc_InitList[0]));
    if (rc != GT_OK)
    {
        return rc;
    }
    /*--------------------------------------------*
     * Init Message Queue                         *
     *--------------------------------------------*/
    rc = osMsgQCreate("AN_WA_Task_MsgQ", 
                       200,
                       sizeof(AN_WA_Task_Msg_STC), 
                       &g_anWa_TaskData.msgQId);
    if (rc != GT_OK)
    {
        return rc;
    }

    #if (APPDEMO_AN_TASK_UNIT_TEST == 1)
        rc = osSemBinCreate("AN_WA_Task_Sem",OS_SEMB_EMPTY,&g_anWa_TaskData.semId);
        if (rc != GT_OK)
        {
            return rc;
        }
    #endif


    /*--------------------------------------------*
     * Create Task                                *
     *--------------------------------------------*/
    rc = osTaskCreate("AN_WA_Task",       /* Task Name      */
                        250,              /* Task Priority  */
                        _8KB,             /* Stack Size     */
                        AN_WA_Task,       /* Starting Point */
                        NULL,             /* Arguments list */
                        &g_anWa_TaskData.taskId);     /* task ID        */
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsTimerWkAfter(100);
    g_anWa_TaskData.taskCreated = GT_TRUE;
    return GT_OK;
}

/*--------------------------------------------*
 * procedures that sends message to task
 *        AN Enable
 *        AN Params Set
 *        AN Intr SyncChange
 *        AN Intr AN_Complete
 *        AN Disable
 *--------------------------------------------*/
GT_VOID AN_WA_Task_MsgInit
(
    AN_WA_Task_Msg_STC *msgPtr,
    AN_WA_TASK_EVENT_ENT ev,
    GT_U8 devNum,
    GT_U32 portNum
)
{
    cpssOsMemSet(msgPtr,0,sizeof(*msgPtr));
    msgPtr->ev        = ev;
    msgPtr->devNum    = devNum;
    msgPtr->portNum   = portNum;
}


GT_STATUS AN_WA_Task_SyncChangeEventSend
(
    GT_U8                devNum,
    GT_PHYSICAL_PORT_NUM portNum,
    GT_BOOL              portSyncOK,
    GT_BOOL              syncFail10ms
)
{
    GT_STATUS rc;
    AN_WA_Task_Msg_STC msg;

    AN_WA_Task_MsgInit(&msg,AN_WA_Task_EVENT_INTR_SYNC_CHANGE_E,devNum,portNum);
    msg.user_data.syncCangeData.portSyncOK   = portSyncOK;
    msg.user_data.syncCangeData.syncFail10ms = syncFail10ms;

    rc = osMsgQSend(g_anWa_TaskData.msgQId,&msg,sizeof(msg),OS_MSGQ_WAIT_FOREVER);

    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}

GT_STATUS AN_WA_Task_AnCompleteEventSend
(
    GT_U8    devNum,
    GT_PHYSICAL_PORT_NUM portNum,
    GT_U32   anPeerCodeword
)
{
    GT_STATUS rc;
    AN_WA_Task_Msg_STC msg;

    AN_WA_Task_MsgInit(&msg,AN_WA_Task_EVENT_INTR_AN_COMPLETE_E,devNum,portNum);
    msg.user_data.anCompleteData.pearCodeword = anPeerCodeword;

    rc = osMsgQSend(g_anWa_TaskData.msgQId,&msg,sizeof(msg),OS_MSGQ_WAIT_FOREVER);

    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;
}


GT_STATUS AN_WA_Task_PortAnEnable
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM physPortNum,
    IN GT_BOOL autoNegSpeedEn,
    IN GT_BOOL autoNegDuplexEn,
    IN GT_BOOL autoNegDuplexMode,
    IN GT_BOOL autoNegFlowCtrlEn,
    IN GT_BOOL autoNegFlowCtrlPauseAdv,
    IN GT_BOOL autoNegFlowCtrlPauseAsmAdv,
    IN GT_BOOL autoNegMacMasterModeEn   /* default shall be disble */
)
{
    GT_STATUS rc;
    AN_WA_Task_Msg_STC msg;
    CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum,physPortNum,/*OUT*/&portMapShadow);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (portMapShadow.valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    Send EVENT AN_Enable() : port = %d mac = %d",physPortNum,portMapShadow.portMap.macNum);
    }

    AN_WA_Task_MsgInit(&msg,AN_WA_Task_EVENT_PORT_AN_ENABLE_E,devNum,physPortNum);
    msg.user_data.anParams.autoNegSpeedEn             = autoNegSpeedEn;
    msg.user_data.anParams.autoNegDuplexEn            = autoNegDuplexEn;
    msg.user_data.anParams.autoNegDuplexMode          = autoNegDuplexMode;
    msg.user_data.anParams.autoNegFlowCtrlEn          = autoNegFlowCtrlEn;
    msg.user_data.anParams.autoNegFlowCtrlPauseAdv    = autoNegFlowCtrlPauseAdv;
    msg.user_data.anParams.autoNegFlowCtrlPauseAsmAdv = autoNegFlowCtrlPauseAsmAdv;
    msg.user_data.anParams.autoNegMacMasterModeEn     = autoNegMacMasterModeEn;

    rc = osMsgQSend(g_anWa_TaskData.msgQId,&msg,sizeof(msg),OS_MSGQ_WAIT_FOREVER);

    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}


GT_STATUS AN_WA_Task_PortAnDisable
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM physPortNum
)
{
    GT_STATUS rc;
    AN_WA_Task_Msg_STC msg;
    CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum,physPortNum,/*OUT*/&portMapShadow);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (portMapShadow.valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    Send EVENT AN_Disable() : port = %d mac = %d",physPortNum,portMapShadow.portMap.macNum);
    }

    AN_WA_Task_MsgInit(&msg,AN_WA_Task_EVENT_PORT_AN_DISABLE_E,devNum,physPortNum);

    rc = osMsgQSend(g_anWa_TaskData.msgQId,&msg,sizeof(msg),OS_MSGQ_WAIT_FOREVER);

    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}


GT_STATUS AN_WA_Task_AnParamsSet
(
    IN GT_U8    devNum,
    IN GT_PHYSICAL_PORT_NUM physPortNum,
    IN GT_BOOL autoNegSpeedEn,
    IN GT_BOOL autoNegDuplexEn,
    IN GT_BOOL autoNegDuplexMode,
    IN GT_BOOL autoNegFlowCtrlEn,
    IN GT_BOOL autoNegFlowCtrlPauseAdv,
    IN GT_BOOL autoNegFlowCtrlPauseAsmAdv
)
{
    GT_STATUS rc;
    AN_WA_Task_Msg_STC msg;
    CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);

    rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum,physPortNum,/*OUT*/&portMapShadow);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (portMapShadow.valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }

    if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
    {
        cpssOsPrintf("\n    Send EVENT AN_ParamSet() : port %d mac = %d",physPortNum,portMapShadow.portMap.macNum);
    }

    AN_WA_Task_MsgInit(&msg,AN_WA_Task_EVENT_PORT_AN_PARAM_SET_E,devNum,physPortNum);
    msg.user_data.anParams.autoNegSpeedEn             = autoNegSpeedEn;
    msg.user_data.anParams.autoNegDuplexEn            = autoNegDuplexEn;
    msg.user_data.anParams.autoNegDuplexMode          = autoNegDuplexMode;
    msg.user_data.anParams.autoNegFlowCtrlEn          = autoNegFlowCtrlEn;
    msg.user_data.anParams.autoNegFlowCtrlPauseAdv    = autoNegFlowCtrlPauseAdv;
    msg.user_data.anParams.autoNegFlowCtrlPauseAsmAdv = autoNegFlowCtrlPauseAsmAdv;


    rc = osMsgQSend(g_anWa_TaskData.msgQId,&msg,sizeof(msg),OS_MSGQ_WAIT_FOREVER);

    if (rc != GT_OK)
    {
        return rc;
    }
    return GT_OK;

}


GT_STATUS AN_WA_Task_SyncChangeEventBuildAndSend
(
    GT_U8                 devNum,
    GT_PHYSICAL_PORT_NUM  physPortNum
)
{
    GT_STATUS rc;
    GT_BOOL portSyncOK;
    GT_BOOL SyncFail10Ms;
    GT_BOOL isAnEnabled;

    if (g_anWa_TaskData.taskCreated == GT_TRUE)
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n    User Evant Handler: Send EVENT SyncChanged() : port = %d",physPortNum);
            cpssOsPrintf("\n            AN enabled ... ");
        }

        rc = appDemoAnConfigurationDBSingleParamGet(devNum,physPortNum,APPDEMO_AN_PORT_IS_AN_ENABLED_E,/*OUT*/&isAnEnabled);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (isAnEnabled == GT_FALSE)
        {
            return GT_OK;
        }
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("OK");
        }

        rc = prvCpssDxChPortAutoNegSyncChangeStateGet(devNum,physPortNum,/*OPUT*/&portSyncOK,&SyncFail10Ms);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n    User Evant Handler: Send EVENT SyncChanged() : port = %d : portSyncOK %d SyncFail10Ms = %d",physPortNum,portSyncOK,SyncFail10Ms);
        }

        rc = AN_WA_Task_SyncChangeEventSend(devNum,physPortNum,portSyncOK,SyncFail10Ms);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}


GT_STATUS AN_WA_Task_AnCompleteEventBuildAndSend
(
    GT_U8                  devNum,
    GT_PHYSICAL_PORT_NUM   physPortNum
)
{
    GT_STATUS rc;
    GT_U32    codeword;
    GT_U32    prevCodeword;
    GT_BOOL   inBandAutoNegBypassAct;
    GT_BOOL   isAnEnabled;
    GT_BOOL   interruptAnCompleteShallbeNoServed;

    if (g_anWa_TaskData.taskCreated == GT_TRUE)
    {
        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n    User Evant Handler: Send EVENT AnComplete() : port = %d",physPortNum);
            cpssOsPrintf("\n            AN enabled ... ");
        }

        rc = appDemoAnConfigurationDBSingleParamGet(devNum,physPortNum,APPDEMO_AN_PORT_IS_AN_ENABLED_E,/*OUT*/&isAnEnabled);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (isAnEnabled == GT_FALSE)
        {
            return GT_OK;
        }
        /*-----------------------------------------------------------* 
         *  AN-WA for that port is enabled , lets proceed with INTR  *
         *----------------------------------------------------------------------------------------------------* 
         * Since there is great number of interrupt of unknown origin and interupt masking does not help much *
         * and we get in interrupt tha was locked just before intruupt was descable                           *
         * DON"T PROCESS any interrupt AFTER already served                                                   *
         *----------------------------------------------------------------------------------------------------*/

        rc = appDemoAnConfigurationDBANCompleteShallNotBeServedGet(devNum,physPortNum,/*OUT*/&interruptAnCompleteShallbeNoServed,&prevCodeword);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (interruptAnCompleteShallbeNoServed == GT_TRUE)
        {
            appDemoAnConfigurationDBANCompleteShallNotBeServedSet(devNum,physPortNum,GT_FALSE,BAD_VALUE);
            return GT_OK;
        }


        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("OK");
        }

        /*--------------------------------------------------------------* 
         * mask(disable) interrupt                                            *
         *--------------------------------------------------------------*/
        (GT_VOID)AN_WA_PortAutoNegInterruptsStateSet(devNum,physPortNum,CPSS_EVENT_MASK_E);

        /*--------------------------------------------------------------------------------------------------*
         * check whether inBandAutoNegBypassAct is actibated             
         *  if inBandAutoNegBypassAct == 1 , than AN process is not finished still , codeword is void 
         *  no sence even send the message to task, 
         *--------------------------------------------------------------------------------------------------*/ 
        rc = prvCpssDxChPortAutoNegBypassActGet(devNum,physPortNum,/*OUT*/&inBandAutoNegBypassAct);
        if (rc != GT_OK)
        {
            goto end_process_wo_msg;
        }
        /* if inBandAutoNegBypassAct == 1 , than AN process not finished still , codeword is void 
           no sence even send the message to task, */ 
        if (inBandAutoNegBypassAct == GT_TRUE)
        {
            rc = GT_OK;
            goto end_process_wo_msg;
        }
        rc = prvCpssDxChPortAutoNegLinkPartnerCodeWordGet(devNum,physPortNum,/*OUT*/&codeword);
        if (rc != GT_OK)
        {
            goto end_process_wo_msg;
        }

        if (g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT==GT_TRUE)
        {
            cpssOsPrintf("\n    User Evant Handler: Send EVENT AnComplete() : port = %d : codeword = 0x%0x",physPortNum,codeword);
        }
        /*----------------------------------------------------------------------------------------------------* 
         * Since there is great number of interrupt of unknown origin and interupt masking does not help much *
         * and we get in interrupt tha was locked just before intruupt was descable                           *
         * DON"T PROCESS interrupt AFTER already served if they have same code word                           *
         *----------------------------------------------------------------------------------------------------*/
        if (prevCodeword == codeword)
        {
            goto end_process_wo_msg;
        }


        rc = AN_WA_Task_AnCompleteEventSend(devNum,physPortNum,codeword);
        if (rc != GT_OK)
        {
            goto end_process_wo_msg;
        }
        
        appDemoAnConfigurationDBANCompleteShallNotBeServedSet(devNum,physPortNum,GT_TRUE,codeword);
end_process_wo_msg:
        /*--------------------------------------------------------------* 
         * unmask(enable) interrupt                                            *
         *--------------------------------------------------------------*/
        (GT_VOID)AN_WA_PortAutoNegInterruptsStateSet(devNum,physPortNum,CPSS_EVENT_UNMASK_E);
        return rc;
    }
    return GT_OK;
}


/*------------------------------------------------*
 * For debug purposse                             *
 *------------------------------------------------*/
GT_STATUS AN_WA_Task_StatusSet(GT_BOOL isEnabled)
{
    g_anWa_TaskData.taskCreated = isEnabled;
    return GT_OK;
}



GT_STATUS AN_TASK_PrintCodeWord
(
    GT_U8 devNum, 
    GT_U32 physPortNum
)
{
    GT_STATUS rc;
    GT_U32 codeword;
    OUT APPDEMO_AN_LINK_SGMII_PARTNER_PARAMS_STC anLinkPartnerParam;

    CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);



    rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum, physPortNum, &portMapShadow);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (portMapShadow.valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }

    rc = prvCpssDxChPortAutoNegLinkPartnerCodeWordGet(devNum,physPortNum,/*OUT*/&codeword);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = AppDemoLinkPartnerCodeWordDecode(codeword,APPDEMO_AN_DECODE_TYPE_SGMII_E,/*OUT*/&anLinkPartnerParam);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("\nPORT %d codeword = 0x%04x",physPortNum,codeword);
    cpssOsPrintf("\b   MAC = %d",portMapShadow.portMap.macNum);
    cpssOsPrintf("\n   ANAck      = %d",anLinkPartnerParam.ANAck      );
    cpssOsPrintf("\n   speed      = %s",CPSS_SPEED_2_STR(anLinkPartnerParam.speed));
    cpssOsPrintf("\n   fullDuplex = %d",anLinkPartnerParam.fullDuplex );
    cpssOsPrintf("\n   linkStatus = %d",anLinkPartnerParam.linkStatus );
    cpssOsPrintf("\n   fcEnRx     = %d",anLinkPartnerParam.fcEnRx     );
    cpssOsPrintf("\n   fcEnTx     = %d",anLinkPartnerParam.fcEnTx     );
    cpssOsPrintf("\n");
    return GT_OK;
}


/* 
/Cider/EBU/BobK/BobK {Current}/Switching Core/GOP/Units/GOP/<Gige MAC IP> Gige MAC IP Units%g/Port Auto-Negotiation Configuration/Port Auto-Negotiation Configuration
    15  - 15    TxConfigRegToEncodeTable 
    14  - 14    AutoMedia SelectEn   
    13  - 13    AnDuplexEn    
    12  - 12    SetFullDuplex  0
    11  - 11    AnFcEn         0 - AnFc Disable
                                1 - AnFc Enable
    10  - 10    PauseAsmAdv    0 - Symmetric Flow Control  
                            1 - Assimetric
    9  -  9    PauseAdv       0 - No Flow Control
                            1 - Flow Control
    7  -  7    AnSpeedEn      0 - Disable Update
                            1 - Enable Update
    6  -  6    SetGMIISpeed    1000_Mbps.
    5  -  5    SetMIISpeed    100 Mbps Speed
    4  -  4    InBand ReStartAn    0
    3  -  3    InBandAn ByPassEn    Bypass
    2  -  2    InBandAnEn    Enabled
    1  -  1    ForceLinkUp    Force Link UP
    0  -  0    ForceLinkDown    Dont_Force Link
    */

GT_STATUS AN_TASK_PrintANConfig
(
    GT_U8 devNum, 
    GT_PHYSICAL_PORT_NUM physPortNum
)
{
    GT_STATUS rc;
    PRV_CPSS_DXCH_PORT_AN_CONFIG_RES_STC anConfig;
    CPSS_DXCH_DETAILED_PORT_MAP_STC portMapShadow;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E
                                            | CPSS_LION_E | CPSS_LION2_E | CPSS_XCAT2_E);


    rc = cpssDxChPortPhysicalPortDetailedMapGet(devNum, physPortNum, &portMapShadow);
    if (rc != GT_OK)
    {
        return rc;
    }
    if (portMapShadow.valid == GT_FALSE)
    {
        return GT_BAD_PARAM;
    }


    rc = prvCpssDxChPortAutoNegConfigGet(devNum,physPortNum,&anConfig);
    if (rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("\bPORT = %d",physPortNum);
    cpssOsPrintf("\b   MAC = %d",portMapShadow.portMap.macNum);
    cpssOsPrintf("\n   ForceLinkDown     = %d",anConfig.ForceLinkDown    );
    cpssOsPrintf("\n   ForceLinkUp       = %d",anConfig.ForceLinkUp      );
    cpssOsPrintf("\n   InBandAnEn        = %d",anConfig.InBandAnEn       );
    cpssOsPrintf("\n   InBandAnByPassEn  = %d",anConfig.InBandAnByPassEn );
    cpssOsPrintf("\n   SetMIISpeed       = %d",anConfig.SetMIISpeed      );
    cpssOsPrintf("\n   SetGMIISpeed      = %d",anConfig.SetGMIISpeed     );
    cpssOsPrintf("\n   AnSpeedEn         = %d",anConfig.AnSpeedEn        );
    cpssOsPrintf("\n   PauseAdv          = %d",anConfig.PauseAdv         );
    cpssOsPrintf("\n   PauseAsmAdv       = %d",anConfig.PauseAsmAdv      );
    cpssOsPrintf("\n   AnFcEn            = %d",anConfig.AnFcEn           );
    cpssOsPrintf("\n   SetFullDuplex     = %d",anConfig.SetFullDuplex    );
    cpssOsPrintf("\n   AnDuplexEn        = %d",anConfig.AnDuplexEn       );
    cpssOsPrintf("\n");
    return GT_OK;
}



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern GT_STATUS cpssInitSystem
(
    IN  GT_U32  boardIdx,
    IN  GT_U32  boardRevId,
    IN  GT_U32  reloadEeprom
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#if (APPDEMO_AN_TASK_TEST_CNS == 1)

    #if (APPDEMO_AN_TASK_UNIT_TEST == 1)
        GT_STATUS AN_WA_Test_Port_SyncChange
        (
            GT_U8     devNum,
            GT_PHYSICAL_PORT_NUM portNum
        )
        {
            GT_STATUS           rc;
            CPSS_PORTS_BMP_STC  portsBmp;
            GT_U32 testSuiteIdx;
            GT_U32 testCaseidx;

            typedef struct 
            {
                /* IN  */
                IN  GT_BOOL    inbandAnEnable;
                IN  GT_BOOL    portSyncOK;
                IN  GT_BOOL    syncFail10ms;
                /* OUT  */
                OUT GT_BOOL    forceLinkDonw;
                OUT GT_BOOL    forceLinkUp;
            }testCase_STC;

            typedef struct 
            {
                CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
                testCase_STC                    testList[8];
            }testSuite_STC;

            static testSuite_STC  testSuite[] = 
            {
                {
                     CPSS_PORT_INTERFACE_MODE_QSGMII_E
                    ,{
                            /* InBandAnEn, portSyncOK, syncFail10ms, ||     forceLinkDonw, forceLinkUp */
                          {      GT_FALSE,   GT_FALSE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {      GT_FALSE,   GT_FALSE,      GT_TRUE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {      GT_FALSE,    GT_TRUE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {      GT_FALSE,    GT_TRUE,      GT_TRUE, /*OUT*/     GT_FALSE,   GT_TRUE  } 
                        /*----------*/
                        , {       GT_TRUE,   GT_FALSE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {       GT_TRUE,   GT_FALSE,      GT_TRUE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {       GT_TRUE,    GT_TRUE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {       GT_TRUE,    GT_TRUE,      GT_TRUE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                     }
                }
               ,{
                     CPSS_PORT_INTERFACE_MODE_1000BASE_X_E
                    ,{
                            /* InBandAnEn, portSyncOK, syncFail10ms, ||     forceLinkDonw, forceLinkUp */
                          {      GT_FALSE,   GT_FALSE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {      GT_FALSE,   GT_FALSE,      GT_TRUE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {      GT_FALSE,    GT_TRUE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {      GT_FALSE,    GT_TRUE,      GT_TRUE, /*OUT*/     GT_FALSE,   GT_FALSE } 
                        /*----------*/
                        , {       GT_TRUE,   GT_FALSE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {       GT_TRUE,   GT_FALSE,      GT_TRUE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {       GT_TRUE,    GT_TRUE,     GT_FALSE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                        , {       GT_TRUE,    GT_TRUE,      GT_TRUE, /*OUT*/      GT_TRUE,   GT_FALSE } 
                     }
                }
            };

            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
            CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,portNum);

            for (testSuiteIdx = 0 ; testSuiteIdx < sizeof(testSuite)/sizeof(testSuite[0]); testSuiteIdx++)
            {
                GT_BOOL duplexState;
                GT_BOOL speedAnEn;
                GT_BOOL fcAnEn;
                GT_BOOL pauseAdvEn;
                rc = cpssDxChPortModeSpeedSet(devNum,portsBmp,GT_TRUE,testSuite[testSuiteIdx].ifMode,CPSS_PORT_SPEED_1000_E);
                if (rc != GT_OK)
                {
                    return rc;
                }

                AN_WA_Task_AnParamsSet(devNum,portNum,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE);
                osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);

                rc = cpssDxChPortDuplexAutoNegEnableGet(devNum,portNum,&duplexState);
                if (rc != GT_OK)
                {
                    return rc;
                }
                if (duplexState != GT_FALSE)
                {
                    return GT_FAIL;
                }


                rc = cpssDxChPortSpeedAutoNegEnableGet(devNum,portNum,&speedAnEn);
                if (rc != GT_OK)
                {
                    return rc;
                }
                if (speedAnEn != GT_FALSE)
                {
                    return GT_FAIL;
                }

                rc = cpssDxChPortFlowCntrlAutoNegEnableGet(devNum,portNum,&fcAnEn,&pauseAdvEn);
                if (rc != GT_OK)
                {
                    return rc;
                }
                if (fcAnEn != GT_FALSE)
                {
                    return GT_FAIL;
                }
                if (pauseAdvEn != GT_FALSE)
                {
                    return GT_FAIL;
                }

                for (testCaseidx = 0 ; testCaseidx < 8; testCaseidx++)
                {
                    APPDEMO_ForceLinkUpLinkDown_STC linkState;
                    testCase_STC * testCasePtr = &testSuite[testSuiteIdx].testList[testCaseidx];

                    rc = cpssDxChPortInbandAutoNegEnableSet(devNum,portNum,testCasePtr->inbandAnEnable);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }

                    AN_WA_Task_SyncChangeEventSend(devNum,portNum,testCasePtr->portSyncOK,testCasePtr->syncFail10ms);
                    osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);

                    /* now check results */
                    rc = BobKAutoNegTask_ForceLinkUpLinkDownGet(devNum,portNum,&linkState);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                    if (linkState.forceLinkDown != testCasePtr->forceLinkDonw)
                    {
                        return GT_FAIL;
                    }
                    if (linkState.forceLinkUp != testCasePtr->forceLinkUp)
                    {
                        return GT_FAIL;
                    }
                }
            }
            return GT_OK;
        }

        GT_STATUS AN_WA_Test_Port_AN_Complete
        (
            GT_U8     devNum,
            GT_PHYSICAL_PORT_NUM physPortNum
        )
        {
            GT_STATUS           rc;
            CPSS_PORTS_BMP_STC  portsBmp;
            GT_U32              codeword;


            /* AN params all true , master moe -- false */
            AN_WA_Task_PortAnEnable(devNum,physPortNum,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_FALSE);
            osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);


            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
            CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,physPortNum);
            rc = cpssDxChPortModeSpeedSet(devNum,portsBmp,GT_TRUE,CPSS_PORT_INTERFACE_MODE_QSGMII_E,CPSS_PORT_SPEED_1000_E);
            if (rc != GT_OK)
            {
                return rc;
            }

            AN_WA_Task_SyncChangeEventSend(devNum,physPortNum,GT_TRUE,GT_TRUE);
            osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);
            /*----------------------------------------------------------* 
             *  set ForceLinkUp == GT_TRUE , send message AN complete   *
             *       --> result restart AN
             *----------------------------------------------------------*/
            rc = cpssDxChPortForceLinkPassEnableSet(devNum,physPortNum,GT_TRUE);
            if (rc != GT_OK)
            {
                return rc;
            }

            AN_WA_Task_AnCompleteEventSend(devNum,physPortNum,0);
            osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);


            AN_WA_Task_AnCompleteEventSend(devNum,physPortNum,0);  /* just restart !!! */
            osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);


            AN_WA_Task_AnCompleteEventSend(devNum,physPortNum,0x4001);  /* change to ForceLinkDown == GT_FALSE, ForceLinkUp == GT_TRUE */
            osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);

            AN_WA_Task_AnCompleteEventSend(devNum,physPortNum,0);   /* restart AN */
            osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);



            codeword = /*speed GMII */  (0x2 << PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_SPEED_OFFS_CNS) + 
                       /* duplex    */  (0x1 << PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_FULL_DUPLEX_OFFS_CNS) + 
                       /*link status*/  (0x1 << PRV_CPSS_DXCH_AN_LINK_PARTNER_SGMII_LINK_STATUS_OFFS_CNS);

            AN_WA_Task_AnCompleteEventSend(devNum,physPortNum,codeword);   /* restart AN */
            osSemWait(g_anWa_TaskData.semId,OS_WAIT_FOREVER);


            return GT_OK;
        }



        GT_STATUS AN_WA_UnitTest()
        {
            GT_STATUS rc;
            GT_U8     devNum;
            GT_PHYSICAL_PORT_NUM portNum;


            rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = appDemoDbEntryAdd("initAnWaTask", 1);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssInitSystem(29,1,0);
            if (rc != GT_OK)
            {
                return rc;
            }

            g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT = GT_TRUE;


            devNum = 0;
            portNum = 0;
    #if 0
            rc = AN_WA_Test_Port_SyncChange(devNum,portNum);
            if (rc != GT_OK)
            {
                return rc;
            }
    #endif

            rc = AN_WA_Test_Port_AN_Complete(devNum,portNum);
            if (rc != GT_OK)
            {
                return rc;
            }

            return GT_OK;
        }
    #endif


    GT_STATUS AN_WA_Test_ConfigurePorts
    (
        GT_U8 devNum, 
        GT_PHYSICAL_PORT_NUM portNum1, 
        GT_PHYSICAL_PORT_NUM portNum2, 
        CPSS_PORT_INTERFACE_MODE_ENT ifMode,
        CPSS_PORT_SPEED_ENT speed
    )
    {
        GT_STATUS              rc;
        CPSS_PORTS_BMP_STC     portsBmp;


        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,portNum1);

        /* config port and send message AN params */
        rc = cpssDxChPortModeSpeedSet(devNum,portsBmp,GT_TRUE,ifMode,speed);
        if (rc != GT_OK)
        {
            return rc;
        }


        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsBmp);
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsBmp,portNum2);

        /* config port and send message AN params */
        rc = cpssDxChPortModeSpeedSet(devNum,portsBmp,GT_TRUE,ifMode,speed);
        if (rc != GT_OK)
        {
            return rc;
        }


        rc = AN_WA_Task_PortAnEnable(devNum,portNum1,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = AN_WA_Task_PortAnEnable(devNum,portNum2,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_TRUE,GT_FALSE);
        if (rc != GT_OK)
        {
            return rc;
        }

        return rc;
    }

    GT_STATUS AN_WA_Test_InitSystem(GT_BOOL debugPrint)
    {
        GT_STATUS rc;
        GT_U8     devNum = 0;


        rc = appDemoDbEntryAdd("initSerdesDefaults", 0);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = appDemoDbEntryAdd("initAnWaTask", 1);
        if (rc != GT_OK)
        {
            return rc;
        }
        rc = cpssInitSystem(29,1,0);
        if (rc != GT_OK)
        {
            return rc;
        }

        /*-------------------------------------------------------------------------*
         *  required to run in order to allow phy normal processing                *
         *  since defining "initSerdesDefaults" prevents also phy configuration    *
         *-------------------------------------------------------------------------*/
        rc = bobcat2BoardPhyConfig(1, devNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        g_anWa_TaskData.APPDEMO_AN_TASK_DEBUG_PRINT = debugPrint;   /* just for debug */
        return GT_OK;
    }


    GT_STATUS AN_WA_Test_ports
    (
        GT_PHYSICAL_PORT_NUM portNum1,
        GT_PHYSICAL_PORT_NUM portNum2,
        GT_BOOL isQSGMIIor1000BaseX,
        GT_BOOL debugPrint
    )
    {
        GT_STATUS rc;
        GT_U8     devNum = 0;
        CPSS_PORT_INTERFACE_MODE_ENT ifMode;

        devNum = 0;

        rc = AN_WA_Test_InitSystem(debugPrint);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (isQSGMIIor1000BaseX == GT_TRUE)
        {
            ifMode = CPSS_PORT_INTERFACE_MODE_QSGMII_E;
        }
        else
        {
            ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
        }

        rc = AN_WA_Test_ConfigurePorts(devNum,portNum1,portNum2,ifMode,CPSS_PORT_SPEED_1000_E);
        if (rc != GT_OK)
        {
            return rc;
        }

        cpssOsPrintf("\n\n");
        return GT_OK;
    }

#endif




