/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssGenDragonite.h
*
* DESCRIPTION:
*       CPSS DRAGONITE APIs.
*
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*******************************************************************************/
#ifndef __cpssGenDragoniteh
#define __cpssGenDragoniteh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/cpssCommonDefs.h>
#include <cpssCommon/cpssPresteraDefs.h>


#define CPSS_GEN_DRAGONITE_PORTS_NUM_CNS 96
#define CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS 12
#define CPSS_GEN_DRAGONITE_POWER_BUDGETS_NUM_CNS 16
#define CPSS_GEN_DRAGONITE_BLD_TIME_STRING_LEN_CNS 20 /* in bytes */
/* major version of firmware; if isn't equal to what appears in System structure 
 * error occurs; must be changed appropriately every time new firmware with new 
 * major version arrives 
 */
#define CPSS_GEN_DRAGONITE_MAJOR_VERSION_CNS 1


/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PRIORITY_ENT
 *
 * Description:
 *      This enum defines the powered device/power source priority level list
 *
 * Fields:
 *     CPSS_GEN_DRAGONITE_PRIORITY_UNKNOWN_E
 *     CPSS_GEN_DRAGONITE_PRIORITY_CRITICAL_E
 *     CPSS_GEN_DRAGONITE_PRIORITY_HIGH_E
 *     CPSS_GEN_DRAGONITE_PRIORITY_LOW_E
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PRIORITY_UNKNOWN_E,
    CPSS_GEN_DRAGONITE_PRIORITY_CRITICAL_E,
    CPSS_GEN_DRAGONITE_PRIORITY_HIGH_E,
    CPSS_GEN_DRAGONITE_PRIORITY_LOW_E
} CPSS_GEN_DRAGONITE_PRIORITY_ENT;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC
 *
 * Description: Dragonite ports layer 2 powered device data structure
 *
 * Fields:
 *      pdRequestedPower - powered device requested power (LSB - 0.1W);
 *      pdPriority - powered device priority;
 *      portCableLen - port cable length LSb = 1meter, reset value 0x64
 */
typedef struct{
    GT_U32                     pdRequestedPower;
    CPSS_GEN_DRAGONITE_PRIORITY_ENT  pdPriority;
    GT_U32                     portCableLen;
} CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT
 *
 * Description:
 *      This enum defines Power source type according to LLDP definitions
 * Fields:
 *      CPSS_GEN_DRAGONITE_POWER_SOURCE_UNKNOWN_E
 *      CPSS_GEN_DRAGONITE_POWER_SOURCE_PRIMARY_E
 *      CPSS_GEN_DRAGONITE_POWER_SOURCE_BACKUP_E
 *      CPSS_GEN_DRAGONITE_POWER_SOURCE_RESERVED_E
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_POWER_SOURCE_UNKNOWN_E,
    CPSS_GEN_DRAGONITE_POWER_SOURCE_PRIMARY_E,
    CPSS_GEN_DRAGONITE_POWER_SOURCE_BACKUP_E,
    CPSS_GEN_DRAGONITE_POWER_SOURCE_RESERVED_E
} CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC
 *
 * Description: Dragonite ports layer 2 power source data structure
 *
 * Fields:
 *        pseAllocPower - Port  PSE allocated power (LSB - 0.1W);
 *        psePriority - port power source priority;
 *        pseType - port power source type;
*/
typedef struct{
    GT_U32                                   pseAllocPower;
    CPSS_GEN_DRAGONITE_PRIORITY_ENT          psePriority;
    CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT pseType;
} CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_LAYER2_STC
 *
 * Description: Dragonite ports layer 2 data structure
 *
 * Fields:
 *      layer2PdInfo - powered device layer 2 info
 *      layer2PseInfo - power source layer 2 info
 */
typedef struct{
    CPSS_GEN_DRAGONITE_PORT_LAYER2_PD_STC layer2PdInfo;
    CPSS_GEN_DRAGONITE_PORT_LAYER2_PSE_STC layer2PseInfo;
} CPSS_GEN_DRAGONITE_PORT_LAYER2_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_MATRIX_STC
 *
 * Description: Mapping of application logical ports to Dragonite's 
 *              physical ports
 *
 * Fields:
 *       physicalPortNumber - Physical port number related to logical port.
 *                            The user can define its own logical order of 
 *                            the ports by matching a logical port number 
 *                            to a physical port location.
 *       chipNumber - The chip number in which the physical port is.
*/
typedef struct{
    GT_U8 physicalPortNumber;
    GT_U8 chipNumber;
} CPSS_GEN_DRAGONITE_PORT_MATRIX_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_CONFIG_STC
 *
 * Description: Dragonite configuration data structure
 *
 * Fields:
 *        dcDisconnectEn - select AC or DC disconnect method: GT_FALSE - AC, GT_TRUE - DC;
 *        externalSyncDis - disable external sync operations: GT_FALSE - external sync enabled
 *                                                            GT_TRUE - external sync disabled
 *        capDis - disable Cap detection: GT_FALSE - Cap enabled; GT_TRUE - Cap disabled;
 *        disPortsOverride - override disable ports line disconnection: 0 - disable ports line disconnects ports
 *                                                                      1 - disable ports line override
 *        rprDisable - disable the RPR protection: 0 - RPR enabled, 1 - RPR disabled;
 *        vmainAtPolicyEn - enable the disconnection of an AT port on Vmain AT low condition: 0 - Vmain AT Disco
 *                                          disabled, 1 - Vmain AT Disco enabled
 *        class0EqAf - when class 0 is detected then decide AF: 0 - class 0 is detected as AT, 
 *                                                    1 - class 0 is detected as AF;
 *        class123EqAf - when port is AT then decide class 123 as AF: 0 - decide class 123 as AT, 
 *                                                     1 - decide class 123 as AF;
 *        classBypass2ndError - if the result of the second class event is not as the first then detection fails: 0 - two
 *                                                 different results are invalid, 1 - two different results are valid;
 *        classErrorEq0 - decide class out when class current was above class 4 limit: 0 - class error decided according
 *                                     to ClassErrorEq4 flag settings, 1 - class error decided as 0
 *        classErrorEq4 - decide class out when class current was above class 4 limit: 0 - class error decided as error,
 *                                                           1 - class error decided as 4
 *        layer2En - Enables Layer2 power management operation: 0 - Layer2 PM Disable, 1 - Layer2 PM Enable
 *        portPrioritySetByPD - Enable port priority settings according to PD LLDP advertisement: 
 *                                                    0 - Layer2 Priority Setting Disable,
 *                                                    1 - Layer2 Priority Setting Enable
 *        privateLableBit - This bit is equal to 0 after reset. The host should set this bit to '1' during config. 
 *                                By monitoring this bit the host can detect self reset events:
 *                                                    0 - reset occurred.
 *                                                    1 - Bit was set by the Host
 *        lowPriDiscoForHiPriStartupEn - when there is a request for startup power when the edge of the budget is reached, 
 *                                          and there is consumption in a lower priority level, if this bit is set then the 
 *                                          highest priority port which is requesting power will be given additional budget 
 *                                          temporarily to enable startup and then the budget will be returned to the previous 
 *                                          level so that low priority ports will be disconnected:
 *                                                    0 - feature disabled
 *                                                    1 - feature enabled
 *        portMethodLimitMode - Port Power limit calculation method for power allocation;
 *        portMethodCalcMode - port power consumption calculation method for power management;
 *        guardBandValue - Guard band value power for startup will be used up to current budget;
 *                                       (LSB -0.1W, Range - 0 - 25.5W)
 *        startupHiPrOverride - when startup power is not enough for the current port, check 
 *           if the following ports can be started up or mark them all as PM:
 *                                            0 - don't try to start the following ports
 *                                            1 - check if startup power is enough for lower priority ports
 *        IcutMaxAt - max Icut value for AT ports: Def - 644mA / 4.88 = 132
 *        tempAlarmTh - temperature level for alarm: Temp = ((reg_value - 684) / (-1.514)) - 40 lsb Vtemp mes:  0.66C
 *        vmainHighTh - max Vmain level;
 *        vmainAtLowTh - AT min Vmain level;
 *        vmainAfLowTh - AF min Vmain level;
 *        matrixPort[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS] - Mapping of application logical ports to Dragonite's 
 *                                                       physical ports
 *        commPollingTime - communication polling time;
 *        irqGenTime - irq generation time;
 */
typedef struct{
    GT_BOOL     dcDisconnectEn;
    GT_BOOL     externalSyncDis;
    GT_BOOL     capDis;
    GT_BOOL     disPortsOverride;
    GT_BOOL     rprDisable;
    GT_BOOL     vmainAtPolicyEn;
    GT_BOOL     class0EqAf;
    GT_BOOL     class123EqAf;
    GT_BOOL     classBypass2ndError;
    GT_BOOL     classErrorEq0;
    GT_BOOL     classErrorEq4;
    GT_BOOL     layer2En;
    GT_BOOL     portPrioritySetByPD;
    GT_BOOL     privateLableBit;
    GT_BOOL     lowPriDiscoForHiPriStartupEn;
    GT_U32      portMethodLimitMode; /* 4 bits seems will be enum for now values not defined in spec */
    GT_U32      portMethodCalcMode; /* 4 bits seems will be enum for now values not defined in spec */
    GT_U32      guardBandValue;
    GT_BOOL     startupHiPrOverride;
    GT_U32      IcutMaxAt;
    GT_U32      tempAlarmTh;
    GT_U32      vmainHighTh;
    GT_U32      vmainAtLowTh;
    GT_U32      vmainAfLowTh;
    CPSS_GEN_DRAGONITE_PORT_MATRIX_STC matrixPort[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
    GT_U32      commPollingTime;
    GT_U32      irqGenTime;
} CPSS_GEN_DRAGONITE_CONFIG_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC
 *
 * Description: Dragonite ports measurements data structure
 *
 * Fields:
 *       powerConsumption - Real port power consumption;
 *       current - port current;
 *       volt - port voltage;
 *       calcPowerConsumption - Static port power consumption;
*/
typedef struct{
    GT_U32 powerConsumption;
    GT_U32 current;
    GT_U32 volt;
    GT_U32 calcPowerConsumption;
} CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT
 *
 * Description:
 *      This enum defines Implementation specific Status for the port
 *
 * Fields:
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_ON_E           -  PortOn - Port was turned on due to a valid signature (res \ cap)
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_ON_TM_E        -  PortOnTM - Port was turned on due to Force Power
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_E           -  Startup - Port is in startup
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_TM_E        -  StartupTM - Port is in startup as force power
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SEARCHING_E         -  Searching - Port needs detection or during detection
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_INVALID_SIGNATURE_E -  InvalidSig - Invalid signature has been detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_CLASS_ERROR_E       -  ClassError - Error in classification has been detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_TEST_MODE_E         -  TestMode - Port needs to be turned on as Test Mode - Force Power
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_VALID_SIGNATURE_E   -  ValidSig - A valid signature has been detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_DISABLED_E          -  Disabled -  Port is disabled
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_OVERLOAD_E  -  StartupOVL - Overload during startup
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_UNDERLOAD_E - StartupUDL - Underload during startup
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_SHORT_E      - StartupShort - Short during startup
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_DVDT_FAIL_E  - DvDtFail - Failure in the Dv/Dt algorithm
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_TEST_ERROR_E - TestError - Port was turned on as Test Mode and has error
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_OVERLOAD_E           - OVL - Overload detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNDERLOAD_E          - UDL - Underload detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SHORT_CIRCUIT_E      - ShortCircuit - Short circuit detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_POWER_MANAGE_E       - PM - port was turned off due to PM
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SYSTEM_DISABLED_E    - SysDisabled - Chip level error
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNKNOWN_E            - Unknown - General chip error
 *     
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_ON_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_ON_TM_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_TM_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SEARCHING_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_INVALID_SIGNATURE_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_CLASS_ERROR_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_TEST_MODE_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_VALID_SIGNATURE_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_DISABLED_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_OVERLOAD_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_UNDERLOAD_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_SHORT_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_DVDT_FAIL_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_TEST_ERROR_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_OVERLOAD_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNDERLOAD_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SHORT_CIRCUIT_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_POWER_MANAGE_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SYSTEM_DISABLED_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNKNOWN_E
} CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT
 *
 * Description:
 *      This enum defines possible 802.3_af/at_PortStatus
 * Fields:
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_DISABLED_E,                - Disabled
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_SEARCHING_E,               - Searching
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_DELIVERING_POWER_E,        - Delivering Power
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_TEST_MODE_E,               - Test Mode
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_TEST_ERROR_E,              - Test Error
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_IMPLEMENTATION_SPECIFIC_E  - Implementation Specific
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_DISABLED_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_SEARCHING_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_DELIVERING_POWER_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_TEST_MODE_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_TEST_ERROR_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_IMPLEMENTATION_SPECIFIC_E
} CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT
 *
 * Description:
 *      This enum defines possible port class
 * Fields:
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_0_E,         - Class 0
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_1_E,         - Class 1
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_2_E,         - Class 2
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_3_E,         - Class 3
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_4_E,         - Class 4
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ERROR_E,     - Class Error
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_RESERVED_E,  - Reserved
 *  CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_UNDEFINED_E  - Class Not Defined
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_0_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_1_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_2_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_3_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_4_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ERROR_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_RESERVED_E,
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_UNDEFINED_E
} CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT;

/*
 * typedef: struct CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC
 *
 * Description: Dragonite port status data
 *
 * Fields:
 *   status - Implementation specific Status for the port;
 *   extStatus - 802.3_af/at_PortStatus;
 *   portClass - Port Class;
 *   portAtBehavior - after classification this bit indicates if the port is 
 *                    decided to be AF or AT:
 *                             GT_FALSE - port decided to be AF
 *                             GT_TRUE - port decided to be AT
*/
typedef struct
{
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT         status;
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_AF_AT_STATUS_ENT   extStatus;
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_PORT_CLASS_ENT     portClass;
    GT_BOOL                                        portAtBehavior;
} CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC;

/*
 * typedef: struct CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC
 *
 * Description: Indications of different port's problems
 *
 * Fields:
 *  underload        GT_TRUE - Underload Detected
 *  overload         GT_TRUE - Overload Detected
 *  shortCircuit     GT_TRUE - Short Circuit detected
 *  invalidSignature GT_TRUE - Invalid PD Resistor Signature Detected
 *  validSignature   GT_TRUE - Valid PD Resistor Signature Detected
 *  powerDenied      GT_TRUE - Power Was Denied
 *  validCapacitor   GT_TRUE - Valid Capacitor signature Detected
 *  backoff          GT_TRUE - Backoff state has occurred
 *  classError       GT_TRUE - Class Error has occurred
 */
typedef struct
{
    GT_BOOL underload;
    GT_BOOL overload;
    GT_BOOL shortCircuit;
    GT_BOOL invalidSignature; 
    GT_BOOL validSignature; 
    GT_BOOL powerDenied;
    GT_BOOL validCapacitor;
    GT_BOOL backoff;
    GT_BOOL classError;
} CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_STATUSES_STC
 *
 * Description: Dragonite ports status structure
 *
 * Fields:
 *  portSr - port status;
 *  lastDisconnect - Reason for ports last disconnection:
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_DISABLED_E           - Disabled -  Port is disabled
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_OVERLOAD_E   - StartupOVL - Overload during startup
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_UNDERLOAD_E  - StartupUDL - Underload during startup
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_SHORT_E      - StartupShort - Short during startup
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_DVDT_FAIL_E  - DvDtFail - Failure in the Dv/Dt algorithm
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STARTUP_TEST_ERROR_E - TestError - Port was turned on as Test Mode and has error
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_OVERLOAD_E           - OVL - Overload detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNDERLOAD_E          - UDL - Underload detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SHORT_CIRCUIT_E      - ShortCircuit - Short circuit detected
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_POWER_MANAGE_E       - PM - port was turned off due to PM
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_SYSTEM_DISABLED_E    - SysDisabled - Chip level error
 *      CPSS_GEN_DRAGONITE_PORT_STATUS_SR_UNKNOWN_E            - Unknown - General chip error
 *  indications - Indications of different port's problems
 */
typedef struct{
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STC        portSr;
    CPSS_GEN_DRAGONITE_PORT_STATUS_SR_STATUS_ENT lastDisconnect;
    CPSS_GEN_DRAGONITE_PORT_INDICATIONS_STC      indications;
} CPSS_GEN_DRAGONITE_PORT_STATUSES_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC
 *
 * Description: Dragonite ports counters data structure
 *
 * Fields:
 *      invalidSignatureCounter - Cyclic counter for invalid signature events;
 *      powerDeniedCounter      - Cyclic counter for Power Denied events
 *      overloadCounter         - Cyclic counter for Overload events
 *      shortCyrcuitCounter     - Cyclic counter for Short Circuit events
 *      underloadCounter        - Cyclic counter for Under Load events
 *      classErrorCounter       - Cyclic counter for Class Error events
*/
typedef struct{
/* left here array inside structure, because in this case with GT_U8 it could save some memory */
    GT_U8 invalidSignatureCounter[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
    GT_U8 powerDeniedCounter[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
    GT_U8 overloadCounter[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
    GT_U8 shortCyrcuitCounter[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
    GT_U8 underloadCounter[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
    GT_U8 classErrorCounter[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
} CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT
 *
 * Description:
 *      This enum defines possible port DRAGONITE control status
 *
 * Fields:
 *       CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_DISABLED_E
 *       CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENABLED_E
 *       CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_FORCE_E
 *       CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_RESERVED_E
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_DISABLED_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENABLED_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_FORCE_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_RESERVED_E
} CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT
 *
 * Description:
 *      This enum defines port DRAGONITE pair status
 * Fields:
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_RESERVED_0_E
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ALTER_A_E
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ALTER_B_E - backoff enable
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_RESERVED_3_E
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_RESERVED_0_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ALTER_A_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ALTER_B_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_RESERVED_3_E
} CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT
 *
 * Description:
 *      This enum defines possible port DRAGONITE AT/AF mode
 * Fields:
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AF_E,
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AT_E,
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AT4PAIR_E,
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_RESERVED_E
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AF_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AT_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_AT4PAIR_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_RESERVED_E
} CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT
 *
 * Description:
 *      This enum defines possible port DRAGONITE priority
 * Fields:
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_CRITICAL_E,
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_HIGH_E,    
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_LOW_E,     
 *      CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_RESERVED_E 
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_CRITICAL_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_HIGH_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_LOW_E,
    CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_RESERVED_E
} CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT;

/*
 * typedef: struct CPSS_GEN_DRAGONITE_PORT_CTRL_STC
 *
 * Description: Dragonite port control command data
 *
 * Fields:
 *   pseEnable - Port Enable Status;
 *   pairControl - Port Pair Control;
 *   portMode - Port Type Definition;
 *   portPriority - Port Priority Level;
*/
typedef struct
{
    CPSS_GEN_DRAGONITE_PORT_CTRL_STATUS_ENT      pseEnable;
    CPSS_GEN_DRAGONITE_PORT_CTRL_PAIRCTRL_ENT    pairControl;
    CPSS_GEN_DRAGONITE_PORT_CTRL_MODE_ENT        portMode;
    CPSS_GEN_DRAGONITE_PORT_CTRL_PRIORITY_ENT    portPriority;
} CPSS_GEN_DRAGONITE_PORT_CTRL_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_PORT_COMMAND_STC
 *
 * Description: Dragonite ports configuration data structure
 *
 * Fields:
 *       portCtrl - port control values;
 *       portPpl - initialization port power allocation limit
 *       portTppl - on going port power allocation limit
*/
typedef struct{
    CPSS_GEN_DRAGONITE_PORT_CTRL_STC portCtrl;
    GT_U32 portPpl;
    GT_U32 portTppl;
 } CPSS_GEN_DRAGONITE_PORT_COMMAND_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_CHIP_STC
 *
 * Description: Dragonite chips status data structure
 *
 * Fields:
 *      chipInfo - chip info
 *      measuredTemp - measured temperature;
 *      maxMeasuredTemp - maximum measured temperature;
 *      numOfPorts - number of ports connected to this DRAGONITE chip
 *      vmainHighError -  Vmain is over the upper threshold
 *      overTempError - The temperature is over the threshold
 *      disablePortsActiveError - Disable ports line is active
 *      vmainLowAfError - Vmain is under AF low threshold
 *      vmainLowAtError - Vmain is under AT low threshold
 *      tempAlarm - The temperature is over the alarm threshold
 *      chipDevId - chip device Id
 */
typedef struct{
    GT_U32 chipInfo; /* RO */
    GT_U32 measuredTemp; /* RO */
    GT_U32 maxMeasuredTemp; /* RO */
    GT_U32 numOfPorts; /* RO */    
    GT_BOOL vmainHighError; /* RO */    
    GT_BOOL overTempError; /* RO */    
    GT_BOOL disablePortsActiveError;/* RO */    
    GT_BOOL vmainLowAfError;/* RO */    
    GT_BOOL vmainLowAtError;/* RO */    
    GT_BOOL tempAlarm;/* RO */    
    GT_U32 chipDevId; /* RO */
} CPSS_GEN_DRAGONITE_CHIP_STC;

/*
 * typedef struct CPSS_GEN_DRAGONITE_POWER_BUDGET_STC
 *
 * Description: Power budgets data
 *
 * Fields:
 *   availablePower - power budget for state 000 of the power good lines (LSB - 0.1W);
 *   powerSourceType - Power source type according to LLDP definitions
*/
typedef struct{
    GT_U32                             availablePower;
    CPSS_GEN_DRAGONITE_POWER_SOURCE_TYPE_ENT powerSourceType;
} CPSS_GEN_DRAGONITE_POWER_BUDGET_STC;

/*
 * Typedef: enum CPSS_GEN_DRAGONITE_CHIP_STATE_ENT
 *
 * Description:
 *      This enum defines the type of info in DRAGONITE data structure
 * Fields:
 *     CPSS_GEN_DRAGONITE_CHIP_STATE_NOT_EXISTS_E - DRAGONITE chip doesn't exists;
 *     CPSS_GEN_DRAGONITE_CHIP_STATE_EXISTS_E - DRAGONITE chip exists;
 *     CPSS_GEN_DRAGONITE_CHIP_STATE_NA_E - state not applicable;
 *     CPSS_GEN_DRAGONITE_CHIP_STATE_ERROR_E - DRAGONITE chip error;
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_CHIP_STATE_NOT_EXISTS_E,
    CPSS_GEN_DRAGONITE_CHIP_STATE_EXISTS_E,
    CPSS_GEN_DRAGONITE_CHIP_STATE_NA_E,
    CPSS_GEN_DRAGONITE_CHIP_STATE_ERROR_E
} CPSS_GEN_DRAGONITE_CHIP_STATE_ENT;

/*
 * typedef struct CPSS_GEN_DRAGONITE_SYSTEM_STC
 *
 * Description: Dragonite system data structure
 *
 * Fields:
 *       systemMask0 - reserved;
 *       activeBudget (read only) - the current active budget as read from the ASICs;
 *       vmain (read only) - the average Vmain measurement from all Dragonite drivers
 *       powerBudgetArr - array of power budgets states
 *       vmainHigh (read only) - Vmain is over the upper threshold
 *       vmainLowAT (read only) - Vmain is under AT low threshold
 *       vmainLowAF (read only) - Vmain is under AF low threshold
 *       tempAlarm (read only) - The temperature is over the alarm threshold
 *       overTemp (read only) - The temperature is over the threshold
 *       disablePortsActive (read only) - Disable ports line is active
 *       osStatus (read only) - TBD in reg spec
 *       chipStatusArr (read only) - DRAGONITE chips status
 *       sysTotalCriticalCons (read only) - calculated system power consumption of critical priority ports
 *       sysTotalHighCons (read only) - calculated system power consumption of high priority ports
 *       sysTotalLowCons (read only) - calculated system power consumption of low priority ports
 *       sysTotalCriticalReq (read only) - total system power request for critical priority ports
 *       sysTotalHighReq (read only) - total system power request for high priority ports
 *       sysTotalLowReq (read only) - total system power request for low priority ports
 *       sysTotalCalcPowerCons (read only) - the sum of the calculated power consumption by all the system
 *                                               ?(HighPriorityConsSlaveX + MedPriorityConsSlaveX + LowPriorityConsSlaveX)
 *       sysTotalPowerRequest (read only) - total power request for the whole system
 *       sysTotalDeltaPower (read only) - the power held by the system, for use of the rack level algorithm.
 *                                             Note - can be negative so the MSb is a sign bit.
 *                                             [LSB = 0.1W(CurrBudget - SysTotalCalcPowerCons - SysTotalPowerReq)]
 *       sysTotalRealPowerCons (read only) - the sum of the real power consumed by all the system
 *                                                                     (?TotalPowerConsSlaveX)
 *       minorVersion (read only) - Minor product information numbering
 *       majorVersion (read only) - Major product information numbering
 *       bldDateTime (read only) - String Array representing the folowing string: MMM dd yyyy,HH:mm:ss
 *       checkSumErrorCounter - checksum communication error counter;
 *       lengthErrorCounter - message length communication error counter;
 *       structVersionErrorCounter - structure version communication error counter;
 *       typeErrorCounter - wrong structure type communication error counter;
 *       configErrorCounter - this counter will be incremented due to any reason of unsuccessful config attempt;
 *       irqErrorCounter - The counter will be incremented when IMO bit is high and the IRQ_Cause_Out value is not
 *                            equal to 0.
 */
typedef struct{
    GT_U32     systemMask0; /* reserved*/
    GT_U32     activeBudget;
    GT_U32     vmain;
    CPSS_GEN_DRAGONITE_POWER_BUDGET_STC powerBudgetArr[CPSS_GEN_DRAGONITE_POWER_BUDGETS_NUM_CNS];
    GT_BOOL     vmainHigh;
    GT_BOOL     vmainLowAT;
    GT_BOOL     vmainLowAF;
    GT_BOOL     tempAlarm;
    GT_BOOL     overTemp;
    GT_BOOL     disablePortsActive;
    GT_U32      osStatus; /* TBD in reg spec*/
    CPSS_GEN_DRAGONITE_CHIP_STATE_ENT chipStatusArr[CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS];
    GT_U32      sysTotalCriticalCons;
    GT_U32      sysTotalHighCons;
    GT_U32      sysTotalLowCons;
    GT_U32      sysTotalCriticalReq;
    GT_U32      sysTotalHighReq;
    GT_U32      sysTotalLowReq;
    GT_U32      sysTotalCalcPowerCons;
    GT_U32      sysTotalPowerRequest;
    GT_U32      sysTotalDeltaPower;
    GT_U32      sysTotalRealPowerCons;
    GT_U32      minorVersion;
    GT_U32      majorVersion;
    GT_U8       bldDateTime[CPSS_GEN_DRAGONITE_BLD_TIME_STRING_LEN_CNS];
    GT_U32      checkSumErrorCounter;
    GT_U32      lengthErrorCounter;
    GT_U32      structVersionErrorCounter;
    GT_U32      typeErrorCounter;
    GT_U32      configErrorCounter;
    GT_U32      irqErrorCounter;
} CPSS_GEN_DRAGONITE_SYSTEM_STC;


/*
 * Typedef: enum CPSS_GEN_DRAGONITE_DATA_TYPE_ENT
 *
 * Description:
 *      This enum defines the type of info in DRAGONITE data structure
 * Fields:
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E - DRAGONITE data structure holds MCU configuration data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E - DRAGONITE data structure holds MCU system data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E - DRAGONITE data structure holds driver/device data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E - DRAGONITE data structure holds port commands data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E - DRAGONITE data structure holds port counters data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E - DRAGONITE data structure holds port status data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E - DRAGONITE data structure holds port measurements data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E - DRAGONITE data structure holds port layer2 data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_DEBUG_E - DRAGONITE data structure holds MCU debug data;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_OTHER_E - reserved;
 *     CPSS_GEN_DRAGONITE_DATA_TYPE_MAX_E - for generic implementation purposes;
 *
 */
typedef enum
{
    CPSS_GEN_DRAGONITE_DATA_TYPE_CONFIG_E = 0,
    CPSS_GEN_DRAGONITE_DATA_TYPE_SYSTEM_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_CHIP_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_COMMANDS_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_CNTRS_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_STATUS_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_PORT_MEASURE_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_LAYER2_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_DEBUG_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_OTHER_E,
    CPSS_GEN_DRAGONITE_DATA_TYPE_MAX_E = (CPSS_GEN_DRAGONITE_DATA_TYPE_OTHER_E)
} CPSS_GEN_DRAGONITE_DATA_TYPE_ENT;

/*
 * typedef: struct CPSS_GEN_DRAGONITE_DATA_STC
 *
 * Description: Dragonite data structure
 *
 * Fields:
 *   dataType        - type of data that structure holds at the moment;
 *   dragoniteData         - union holding data accordingly to dataType;
 */
typedef struct CPSS_GEN_DRAGONITE_DATA_STCT
{
    CPSS_GEN_DRAGONITE_DATA_TYPE_ENT         dataType;

    union {
        CPSS_GEN_DRAGONITE_SYSTEM_STC            system;
        CPSS_GEN_DRAGONITE_CHIP_STC              chip[CPSS_GEN_DRAGONITE_CHIPS_NUM_CNS];
        CPSS_GEN_DRAGONITE_PORT_COMMAND_STC      portCmd[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
        CPSS_GEN_DRAGONITE_PORT_COUNTERS_STC     portCntrs;
        CPSS_GEN_DRAGONITE_PORT_STATUSES_STC     portStat[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
        CPSS_GEN_DRAGONITE_PORT_MEASUREMENTS_STC portMeasure[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
        CPSS_GEN_DRAGONITE_CONFIG_STC            config;
        CPSS_GEN_DRAGONITE_PORT_LAYER2_STC       layer2[CPSS_GEN_DRAGONITE_PORTS_NUM_CNS];
        GT_U32                             *debugDataPtr;
        GT_U32                             *otherDataPtr;
    } dragoniteData; /* union */

} CPSS_GEN_DRAGONITE_DATA_STC;

/*******************************************************************************
* cpssDragoniteInit
*
* DESCRIPTION:
*           Initialize DRAGONITE management library:
*               get shared memory base pointer from BSP; 
*               get system structure and check version compatibility;
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*    intVectNum - DRAGONITE interrupt vector number
*    intMask    - DRAGONITE interrupt mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_INIT_ERROR - if major version of firmware isn't
*                       equal to CPSS_GEN_DRAGONITE_MAJOR_VERSION_CNS
*       GT_NOT_READY  - DRAGONITE controller not finished yet 
*                       copy system structure to shared memory
*       GT_BAD_STATE  - data received, but not of System type
*       GT_BAD_PTR    - if wrong shared mem base addr
*
* COMMENTS:
*       This call may return NOT_READY if DRAGONITE manager still not initialized 
*       application must set some timeout for this case and try again,
*       it can't do nothing DRAGONITE related till this call not succeeded.
*
*******************************************************************************/
GT_STATUS cpssDragoniteInit
(
    IN GT_U32 intVectNum,
    IN GT_U32 intMask
);

/*******************************************************************************
* cpssDragoniteReadTrigger
*
* DESCRIPTION:
*      Trigger DRAGONITE controller to put in shared memory data of requested type
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       type - type of data structure application wants to read 
*                  from DRAGONITE MCU
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM   - on unknown type
*       GT_NOT_READY   - DRAGONITE controller not finished yet 
*                                          previous operation
*       GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDragoniteReadTrigger
(
    IN CPSS_GEN_DRAGONITE_DATA_TYPE_ENT type
);

/*******************************************************************************
* cpssDragoniteReadData
*
* DESCRIPTION:
*      Read DRAGONITE data structure
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       None
*
* OUTPUTS:
*    dataPtr  - pointer to structure, that is actually union,
*             where to put data read from shared memory.
*             Pay attention dataPtr is just memory pointer - any setting in it 
*             will be ignorred - this function just returns to application 
*             what it has read from shared memory, it doesn't check if it's 
*             what application asked in ReadTrigger or Write called before
*
* RETURNS:
* --------------------
*  Return codes generated by CPSS API itself:
* --------------------
* GT_OK                    - on success
* GT_NOT_READY   - DRAGONITE controller not finished yet 
*                                    copy data to shared memory
* GT_BAD_PTR         - Illegal pointer value
* ------------------------
* Following errors returned by DRAGONITE controller communication
* protocol:
* ------------------------
* GT_CHECKSUM_ERROR - checksum error. 
* GT_BAD_SIZE - Structure length error.
* GT_NOT_SUPPORTED - Structure version mismatch.
* GT_BAD_PARAM - Unknown Type. When this bit is set,
*         there will be no data structure returned, only the
*         header.
* GT_OUT_OF_RANGE - Out of range value in the structure
*        data. The DRAGONITE manager will fill the data in the structure
*        with zeros as long as the received data was in range. In
*        case of data out of range, the received out of range
*        data will be returned.
* GT_BAD_STATE - Config is not allowed - Application can
*         send config structure to DRAGONITE controller only once after
*         reset.
* GT_ERROR - could happen only if there is something in protocol
*               that cpss is not aware of 
* GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENT:
*     Just one error code returned at once - error priority list:
*     GT_CHECKSUM_ERROR - highest priority
*     GT_BAD_PARAM
*     GT_NOT_SUPPORTED
*     GT_BAD_SIZE
*     GT_BAD_STATE (specific for Config structure)
*     GT_OUT_OF_RANGE (fields with wrong size returned in
*                                           dataPtr)
*
*******************************************************************************/
GT_STATUS cpssDragoniteReadData
(
    OUT CPSS_GEN_DRAGONITE_DATA_STC *dataPtr
);

/*******************************************************************************
* cpssDragoniteWrite
*
* DESCRIPTION:
*           Write DRAGONITE data structure of requested type
*
* APPLICABLE DEVICES:  
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3; ExMx; Puma2; Puma3.
*
* INPUTS:
*       dataPtr  - pointer to structure that is actually union,
*                  contain data to send to DRAGONITE controller
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_READY     - if shared memory isn't free yet
*       GT_BAD_VALUE     - if one of fields exceeds number of bits for field
*       GT_BAD_PARAM     - wrong structure type
*       GT_NOT_SUPPORTED - on try to write to Read Only structure type
*       GT_NOT_INITIALIZED - DragoniteInit not executed
*
* COMMENT: 
*       after cpssDxChDragoniteWrite application should 
*       call cpssDxChDragoniteReadData to ensure there was no 
*       communication error - it could be done by polling after 
*       delay or upon write_done interrupt.
*
*******************************************************************************/
GT_STATUS cpssDragoniteWrite
(
    IN CPSS_GEN_DRAGONITE_DATA_STC *dataPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssGenDragoniteh */

