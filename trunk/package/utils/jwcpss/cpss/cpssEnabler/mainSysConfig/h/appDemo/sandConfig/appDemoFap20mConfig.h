/*******************************************************************************
*              Copyright 2002, Marvell TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* appDemoFap20mConfig.h
*
* DESCRIPTION:
*       Includes FAP20M specific initialization definitions and data-structures.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#ifndef __appDemoFap20mConfigh
#define __appDemoFap20mConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* CPSS includes */
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

/* SAND includes */
#include <SAND/Management/include/sand_low_level.h>
#include <SAND/SAND_FM/include/sand_mem_access.h>
#include <SAND/SAND_FM/include/sand_chip_defines.h>

/* FX950 includes */
#include <FX950/include/fap20m_api_general.h>
#include <FX950/include/fap20m_api_init.h>

#include <appDemo/sandConfig/appDemoCommonConfigDefs.h>

#define APPDEMO_FAP20M_MAX_LINE_SIZE  (200)

/* 
 * In Dune's reference application, all the unicast Queues (4 - 8K)
 *  used for the auto management scheme.
 */
#define APPDEMO_FAP20M_AUTO_FIRST_QUEUE_ID      4
/* 8 queues are used for CPU port */
#define APPDEMO_FAP20M_CPU_PORT_QUEUES_NUM      8

typedef enum{
  APPDEMO_FAP20M_SCH_SCHEME_STRICT,
  APPDEMO_FAP20M_SCH_SCHEME_ENHANCED_STRICT,
  APPDEMO_FAP20M_SCH_SCHEME_WFQ_PER_FLOW,
  APPDEMO_FAP20M_SCH_SCHEME_WFQ_PER_CLASS,
  APPDEMO_FAP20M_SCH_SCHEME_MIXED_STRICT_AND_FLOW_WEIGHT,
  APPDEMO_FAP20M_SCH_SCHEME_MIXED_STRICT_AND_CLASS_WEIGHT,

  APPDEMO_FAP20M_SCH_SCHEME_LAST
}APPDEMO_FAP20M_SCH_SCHEME_IDS;

typedef struct
{
  APPDEMO_FAP20M_SCH_SCHEME_IDS scheme_id;
  /*
   * Each class will have minimum_rate_by_percentage percentage of
   * the port rate reserved to it.
   * 0 - Stands for no assured rate.
   * Valid values: 0-100.
   */
  unsigned int                  minimum_rate_by_percentage;
  /*
   * No class will have more than maximum_rate_by_percentage
   * percentage of the port rate.
   * 0 - Stands for not limitation on the class rate.
   * Valid values: 0-100.
   */
  unsigned int                  maximum_rate_by_percentage;
 /*
  * Add a second sub-flow that will decrease the latency.
  * When this is enabled, the maximum_rate_by_percentage,
  * will behave differently!!!!!
  */
  unsigned int                  support_low_latency;

}APPDEMO_FAP20M_SCH_SCHEME_INFO;

typedef struct
{
  /*
   * Offset as in DS.
   * Example, offset 0x1 is the Identification-Register
   */
  unsigned long offset;
  unsigned long shift;
  unsigned long mask;
  unsigned long data_to_write;
}APPDEMO_FAP20M_FIELD;


/*
 * parameters for the fap20m sweep application
 */
typedef struct
{

  /************************************************************************/
  /* General Parameters                                                   */
  /************************************************************************/

  /*
   * Indicator.
   * TRUE: Reset the FAP, before init.
   * FALSE: Do no reset the FAP, before init.
   */
  unsigned int reset_fap;


  /************************************************************************/
  /* FAP20M Related System Parameters                                     */
  /************************************************************************/

  /*
   * Credit byte size to use (256, 512, 1024, 2048)
   */
  SAND_CREDIT_SIZE credit_size;

  /*
   * QDR field parameter
   * Related to DS register 'Link SRAM configuration mode' at 0x2054 - field 'NumBuffMem'.
   */
  unsigned int     qdr_nof_buff_mem_field;

  unsigned int     qdr_burst_length;

  unsigned int     qdr_read_delay;

  unsigned int     qdr_data_valid_delay;

  unsigned int     qdr_start_burst_delay;

  /*
   * QDP buffer size field parameter
   * Related to DS register 'General Controls' at 0x0010 - field 'BuffSize'
   */
  unsigned int     dram_buff_size_field;

  /************************************************************************/
  /* External Interface Related System Parameters                         */
  /************************************************************************/

  /*
   * Number of ports per HyperG Link interface.
   * Example: used by the All-to-All application.
   */
  unsigned int     max_nof_ports_per_hgl;

  /*
   * nominal (physical line) rate for scheduler port,
   * in Mega-Bits-Sec units.
   */
  unsigned int     nominal_port_rate_mbps;

  /*
   * Speed up of the scheduler ports in percents.
   * 100% - no speed up.
   * 120% -speed up of 20%.
   */
  unsigned int     port_speed_up;

  /*
   * Array of egress queues types (0, 1, 2, 3).
   * Only 'max_nof_ports_per_hgl' entries are valid.
   */
  unsigned int     egress_queues_weights[FAP20M_NOF_DATA_PORTS];


  FAP20M_LINK_INTERFACE  link_if[FAP20M_NOF_DATA_PORTS];

  /************************************************************************/
  /* General Register Configuration                                       */
  /************************************************************************/

  /*
   * Number of valid entries in 'general_fields'.
   */
  unsigned int  nof_general_fields;

  /*
   * Free parameters to load to any register/field.
   */
  APPDEMO_FAP20M_FIELD  general_fields[APPDEMO_FAP20M_MAX_LINE_SIZE];

  /*
  * The scheduling scheme to be applied at init.
  */
  APPDEMO_FAP20M_SCH_SCHEME_INFO default_sch_scheme;

 /*
  *
  */
  FAP20M_REVISION_CONTROL rev_ctrl;

  /*
   * There are two schedulers in the Fabric Output Processor a
   * Primary Scheduler and a Secondary Scheduler.
   * This parameters sets the schedulers which is assigned to fabric
   */
  FAP20M_SCH_RANK         sch_rank_of_fabric;


}APPDEMO_FAP20M_PARAMS;

/*
 * typedef: enum APPDEMO_FAP20M_PP_TYPE_ENT_ENT
 *
 * Description: Defines PP type.
 *
 * Enumerations:
 *  APPDEMO_FAP20M_PP_EXMXPM_24G_E   - Puma PP with 24 with Giga ports.
 *  APPDEMO_FAP20M_PP_EXMXPM_2XG_E   - Puma PP with 2 XG ports.
 *
 */
typedef enum
{
    APPDEMO_FAP20M_PP_EXMXPM_24G_E,
    APPDEMO_FAP20M_PP_EXMXPM_2XG_E
}
APPDEMO_FAP20M_PP_TYPE_ENT;


#define APPDEMO_FAP20M_TOP_BUFF_BLK_0_QDR_0_5       ((497 * 1024) - 60)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_1_QDR_0_5       ((502 * 1024) - 40)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_2_QDR_0_5       ((507 * 1024) - 20)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_0_QDR_1_0       ((1009 * 1024) - 60)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_1_QDR_1_0       ((1014 * 1024) - 40)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_2_QDR_1_0       ((1019 * 1024) - 20)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_0_QDR_2_0       ((2033 * 1024) - 60)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_1_QDR_2_0       ((2038 * 1024) - 40)
#define APPDEMO_FAP20M_TOP_BUFF_BLK_2_QDR_2_0       ((2043 * 1024) - 20)

/*******************************************************************************
* appDemoFap20mHwAccessInit
*
* DESCRIPTION:
*       Binds read/write callbacks to SAND module.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFap20mHwAccessInit
(
    void
);

/*******************************************************************************
* appDemoFap20mDeviceInit
*
* DESCRIPTION:
*       Registers the device on FAP20M and initialize QDP,
*       System related parameters and Multicast.
*
* INPUTS:
*       baseAddrPtr             - pointer to FAP20M device base address.
*       pexBaseAddrPtr          - pointer to PEX base address.
*       ppType                  - PP Type: 24G ports or 2XG ports.
*       fapId                   - Fabric ID.
*       maxNumOfFapsInSystem    - maximum number of FAPs in system
*
* OUTPUTS:
*       deviceIdPtr     - FAP20M device ID.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFap20mDeviceInit
(
    IN  unsigned long               *baseAddrPtr,
    IN  unsigned long               *pexBaseAddrPtr,
    IN  APPDEMO_FAP20M_PP_TYPE_ENT  ppType,
    IN  unsigned int                fapId,
    IN  unsigned int                maxNumOfFapsInSystem,
    OUT unsigned int                *deviceIdPtr
);

/*******************************************************************************
* appDemoFap20mSchedulerInit
*
* DESCRIPTION:
*       Initialization of the FAP20M scheduler.
*
* INPUTS:
*       deviceId                - device Id.
*       max_nof_faps_in_system  - maximal number of FAPs in system
*       ppType                  - PP Type: 24G ports or 2XG ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFap20mSchedulerInit
(
    IN unsigned int                 deviceId,
    IN unsigned int                 max_nof_faps_in_system
);


/*******************************************************************************
* appDemoFap20mPortShedulerIdGet
*
* DESCRIPTION:
*       Calculates FAP20M port scheduler ID for a given Packet Processor port 
*       number.
*
* INPUTS:
*       pp_port_num      - Packet Processor port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       Scheduler ID.
*
* COMMENTS:
*
*
*******************************************************************************/
unsigned int  appDemoFap20mPortShedulerIdGet
(
    IN unsigned int     pp_port_num 
);

/*******************************************************************************
* appDemoFap20mInitInterrupts
*
* DESCRIPTION:
*       Init interrupts part of appdemo
*
* INPUTS:
*       deviceId      - device Id.
*       intVecNum - interrupt vector number to attach ISR to
*       intMask   - interrupt bits to enable (unmask) interrupts
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFap20mInitInterrupts
(
    IN unsigned int    deviceId,
    IN GT_U32          intVecNum,
    IN GT_U32          intMask
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* appDemoFap20mConfigh */
