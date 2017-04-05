/*******************************************************************************
*              Copyright 2002, MARVELL TECHNOLOGY, LTD.
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
* appDemoFap20mConfig.c
*
* DESCRIPTION:
*       Initialization functions for the FAP20M boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

/* Cpss includes */
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <gtExtDrv/drivers/gtIntDrv.h>
#include <appDemo/sandConfig/appDemoFap20mConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#include <SAND/Utils/include/sand_os_interface.h>
#include <SAND/Utils/include/sand_extServices.h>
#include <gtOs/gtOsMsgQ.h>
#include <gtOs/gtOsStdLib.h>

#include <SAND/Management/include/sand_module_management.h>

/* Fx950 includes */
#include <FX950/include/fx950_host_management.h>
#include <FX950/include/fap20m_api_egress_ports.h>
#include <FX950/include/fap20m_chip_defines.h>
#include <FX950/include/fap20m_api_ingress_queue.h>
#include <FX950/include/fap20m_api_diagnostics.h>
#include <FX950/include/fap20m_scheduler.h>
#include <FX950/include/fap20m_api_spatial_multicast.h>
#include <FX950/include/fap20m_auto_mngr.h>
#include <FX950/include/fx950_api_general.h>
#include <FX950/include/fx950_host_management.h>
#include <FX950/include/fx950_api_interrupts.h>
#include <FX950/include/fap20m_api_auto_management.h>
#include <FX950/include/fap20m_api_routing.h>
#include <FX950/include/fap20m_api_cell.h>
#include <FX950/include/fap20m_cell.h>
#include <FX950/include/fap20m_api_links.h>
#include <FX950/include/fap20m_api_callback.h>
#include <FX950/include/fap20m_interrupt_services.h>



#ifdef PRESTERA_DEBUG
#define FAP20M_APP_DEBUG
#endif

#define FAP20M_APP_DEBUG
#ifdef FAP20M_APP_DEBUG
#define DBG_INFO(x)     cpssOsPrintf x
#else
#define DBG_INFO(x)
#endif

extern APP_DEMO_PP_CONFIG    appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];


#define APPDEMO_FAP20M_NOF_PORTS            24

#define APPDEMO_FAP20M_SWP_TOP_QUE_BLK_0    (8181)

/*
 * Default Stack size 12000 bytes
 */
#define APPDEMO_FAP20M_DEFAULT_STACK_SIZE 12000
/*
 * Multicast Constants
 */
#define APPDEMO_FAP20M_NOF_MULTIACST_IDS_IN_BLOCK (17)
#define APPDEMO_FAP20M_NOF_MULTIACST_IDS_BLOCKS   (1)
#define APPDEMO_FAP20M_NOF_MULTIACST_GROUPS       (APPDEMO_FAP20M_NOF_MULTIACST_IDS_IN_BLOCK * APPDEMO_FAP20M_NOF_MULTIACST_IDS_BLOCKS)
#define APPDEMO_FAP20M_FIRST_MG_ID                (0)
#define APPDEMO_FAP20M_LAST_MG_ID                 (APPDEMO_FAP20M_FIRST_MG_ID + APPDEMO_FAP20M_NOF_MULTIACST_GROUPS - 1)

/*
 * AppDemo FAP20M parameters
 */
APPDEMO_FAP20M_PARAMS  appDemoFap20mParams;
static SAND_PHYSICAL_ACCESS   sand_physical_access ;


static void  appDemoFap20mLoadParams
(
    IN APPDEMO_FAP20M_PP_TYPE_ENT  ppType
);

static GT_STATUS appDemoFap20mInitSequence
(
    IN unsigned int         deviceId,
    GT_U32                  fapId
);

static GT_STATUS appDemoFap20mQdpInit
(
    IN unsigned int     deviceId
);

static GT_STATUS appDemoFap20mSetQueueParams
(
    IN unsigned int                deviceId
);


static GT_STATUS appDemoFap20mSetSystemRelatedSettings
(
    IN unsigned int     deviceId,
    IN unsigned int     max_nof_faps_in_system,
    IN unsigned int     max_nof_fes_in_system
);

static GT_STATUS appDemoFap20mSetMulticastSpatialApp
(
    IN unsigned int            deviceId
);

static unsigned __TASKCONV fx950Task
(
    GT_VOID * param
);


#ifdef ASIC_SIMULATION
    extern void scibReadMemory
    (
        IN  GT_U32        deviceId,
        IN  GT_U32        memAddr,
        IN  GT_U32        length,
        OUT GT_U32 *      dataPtr
    );

    extern void scibWriteMemory
    (
        IN  GT_U32        deviceId,
        IN  GT_U32        memAddr,
        IN  GT_U32        length,
        IN  GT_U32 *      dataPtr
    );


    SAND_RET
      appDemo_fap20m_simulation_physical_read(
        SAND_INOUT  unsigned long *array,
        SAND_IN     unsigned long *base_address,
        SAND_IN     unsigned long offset,
        SAND_IN     unsigned long size
     );

    SAND_RET
      appDemo_fap20m_simulation_physical_write(
        SAND_IN     unsigned long *array,
        SAND_INOUT  unsigned long *base_address,
        SAND_IN     unsigned long offset,
        SAND_IN     unsigned long size
     );

#endif

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
)
{
    SAND_RET            sand_ret;               /* SAND return code */

    #ifndef ASIC_SIMULATION
        /* register read and write callbacks on SAND driver */
        sand_physical_access.physical_read = &fx950_host_management_pex_read;
        sand_physical_access.physical_write = &fx950_host_management_pex_write;
    #else
        /* ASIC_SIMULATION */
        sand_physical_access.physical_read = &appDemo_fap20m_simulation_physical_read;
        sand_physical_access.physical_write = &appDemo_fap20m_simulation_physical_write;
    #endif

    sand_ret = sand_set_physical_access_hook(&sand_physical_access);
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    return GT_OK;
}

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
)
{
    unsigned int        faps_id_in_2_steps;     /* FAP ID step is 2*/
    unsigned int        nof_fabric_cards;       /* number of FE cards */
    unsigned int        deviceId;               /* logical device ID */
    unsigned long       ret;                    /* return code */
    GT_U32              i;                      /* loop iterator */
    SAND_RET            sand_ret;               /* SAND return code */
    GT_STATUS           rc;                     /* CPSS return code */
    FAP20M_SCRUBBER_PARAM
        scubber,
        exact_scubber;
    GT_TASK         fx950TaskTid;


    faps_id_in_2_steps     = TRUE;
    nof_fabric_cards       = 1;

    /* allocate FAP20M AppDemo parameters */
    ret = 0;

    sand_os_memset(&appDemoFap20mParams, 0x0, sizeof(APPDEMO_FAP20M_PARAMS));

    appDemoFap20mLoadParams(ppType);


    /* register fap20m and ocelot d evices */
    ret = fap20m_register_device(baseAddrPtr,
                                 NULL,
                                 &deviceId
                                 );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    *deviceIdPtr = deviceId;

    /* Set PEX base address */
    ret = fx950_set_pex_base_address(deviceId, pexBaseAddrPtr);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    rc = appDemoFap20mInitSequence(deviceId, fapId);
    if (rc != GT_OK)
    {
        return rc;
    }


    DBG_INFO(("    + Starting Scheduler.\n\r"));

    rc = appDemoFap20mSchedulerInit(deviceId, maxNumOfFapsInSystem);
    if (rc != GT_OK)
    {
        return rc;
    }

    DBG_INFO(("    + Starting QDP.\n\r"));

    rc = appDemoFap20mQdpInit(deviceId);
    if (rc != GT_OK)
    {
        return rc;
    }

/***********************************************************************
 *  Set information related to number of faps and fes in the system.
 *  Also enable Recycling and CPU.
 ***********************************************************************/
    DBG_INFO(("    + System Related settings QDP.\n\r"));

    rc = appDemoFap20mSetSystemRelatedSettings(deviceId,
                                               maxNumOfFapsInSystem,
                                               0);
    if (rc != GT_OK)
    {
        return rc;
    }

/***********************************************************************
 *
 *  High Level Application
 ***********************************************************************/

  DBG_INFO(("  + Start Multicast:\n\r"));
  /*
   * Loads FAP20M-Sweep Multicast application to the device.
   */
  rc = appDemoFap20mSetMulticastSpatialApp(deviceId);
  if (rc != GT_OK)
  {
      return rc;
  }

  DBG_INFO(("  + Enable FAP20 traffic:\n\r"));
  ret = fap20m_enable_fabric_traffic(deviceId, TRUE);
  sand_ret = sand_get_error_code_from_error_word(ret) ;
  if (sand_ret != SAND_OK)
  {
      return GT_FAIL;
  }

  /* Ingress Queues scrubber */
  scubber.enable      = TRUE;
  scubber.start_queue = FAP20M_SPATIAL_MULTICAST_NOF_QUEUES;
  scubber.end_queue   = APPDEMO_FAP20M_SWP_TOP_QUE_BLK_0;
  scubber.scan_rate   = 200;
  ret = fap20m_set_scrubber_params(deviceId,
                                   &scubber,
                                   &exact_scubber
                                   );
  sand_ret = sand_get_error_code_from_error_word(ret) ;
  if (sand_ret != SAND_OK)
  {
      return GT_FAIL;
  }
  sand_os_printf(" Queue Scrubber:");
  fap20m_print_scrubber_params(&exact_scubber, TRUE);


  /*
  * A list of fields specified in the sweep text file.
  *  to be written before traffic is enabled.
  */
  for (i = 0; i < appDemoFap20mParams.nof_general_fields ; ++i)
  {
    ret = sand_write_field(
                   deviceId,
                   4 * appDemoFap20mParams.general_fields[i].offset, /*From DS offset to CPU*/
                   appDemoFap20mParams.general_fields[i].shift,
                   appDemoFap20mParams.general_fields[i].mask,
                   appDemoFap20mParams.general_fields[i].data_to_write
                 );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }
  }


    return osTaskCreate("singleFapMode",                /* Task Name      */
                        250,                          /* Task Priority  */
                        _8KB,                         /* Stack Size     */
                        fx950Task,         /* Starting Point */
                        (GT_VOID*)deviceId,           /* Arguments list */
                        &fx950TaskTid);    /* task ID        */
    
  DBG_INFO(("  + END FAP20 init.\n\r"));
  DBG_INFO(("\n\r"));

  return GT_OK;
}


/*******************************************************************************
* appDemoFap20mInitSequence
*
* DESCRIPTION:
*       System independent initialization of the FAP20M device.
*
* INPUTS:
*       deviceId      - device Id.
*       fapId         - fabric Id
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
static GT_STATUS appDemoFap20mInitSequence
(
    IN unsigned int         deviceId,
    GT_U32                  fapId
)
{
    FAP20M_HW_ADJUSTMENTS       hw_ajust;
    FAP20M_DRC                  drc;
    FAP20M_DRAM_CONFIGURATION   dram_cfg;
    FAP20M_FABRIC_LINK_FC       link_fc;
    FAP20M_LINK_INTERFACE       link_interface;
    unsigned long               ret;        /* return code      */
    SAND_RET                    sand_ret;   /* SAND return code */
    unsigned int                i;          /* loop iterator    */


    /* clear drc configuration */
    sand_ret = sand_os_memset(&drc, 0x0, sizeof(drc));
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    sand_ret = sand_os_memset(&dram_cfg, 0x0, sizeof(dram_cfg));
    if (sand_ret != SAND_OK)
    {
      return GT_FAIL;
    }

    hw_ajust.load_serdes_conf = FALSE;




    /*
     * QDR configuration
     */
    hw_ajust.load_qdr_conf = TRUE;
    hw_ajust.qdr_conf.nof_buff_mem  = appDemoFap20mParams.qdr_nof_buff_mem_field;
    hw_ajust.qdr_conf.burst_length  = appDemoFap20mParams.qdr_burst_length;
    hw_ajust.qdr_conf.read_delay  = appDemoFap20mParams.qdr_read_delay;
    hw_ajust.qdr_conf.data_valid_delay  = appDemoFap20mParams.qdr_data_valid_delay;
    hw_ajust.qdr_conf.start_burst_delay  = appDemoFap20mParams.qdr_start_burst_delay;

    /*
     * Set scheduler rank
     */
    ret = fap20m_set_primary_scheduler_role(
                    deviceId,
                    (appDemoFap20mParams.sch_rank_of_fabric==FAP20M_SCH_RANK_PRIM ?
                        FAP20M_SCH_ROLE_FABRIC : FAP20M_SCH_ROLE_EG_TM)
                   );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /*
     * GEN configuration
     */
    hw_ajust.load_gen_conf           = FALSE;
    hw_ajust.gen_conf.dram_buff_size = appDemoFap20mParams.dram_buff_size_field;

    /* DRAM configurations */
    hw_ajust.dram_conf.enable_8bank_mode = 0;
    hw_ajust.dram_conf.burst_size_mode   = 0;
    /* AP is placed on addr[10] */
    hw_ajust.dram_conf.auto_precharge_pos = 2;
    /* 10 Column bits - 1024 columns */
    hw_ajust.dram_conf.nof_columns = 2;
    hw_ajust.load_dram_conf = FALSE;

    /* DRC configurations */
    hw_ajust.drc_conf.ddr_control_trig = 0x2;
    hw_ajust.drc_conf.ddr_mode1 = 0x952;
    hw_ajust.drc_conf.ddr_mode2 = 0x852;
    hw_ajust.drc_conf.ddr_ext_mode = 0x446;
    hw_ajust.drc_conf.ac_op_cond1 = 0x441C010;
    hw_ajust.drc_conf.ac_op_cond2 = 0x505203;
    hw_ajust.drc_conf.init_seq_reg = 0x40;
    hw_ajust.drc_conf.ac_op_cond3 = 0x102018C;
    hw_ajust.drc_conf.drc_general_config = 0x14000227;
    hw_ajust.drc_conf.ddr2_extended_mode_wr2_reg = 0;
    hw_ajust.drc_conf.ddr_extended_mode_reg3 = 0;
    hw_ajust.drc_conf.glue_logic = 0x3041;
    hw_ajust.drc_conf.gddr3_cfg = 0x10213;
    hw_ajust.load_drc_conf = FALSE;

    fap20m_enable_revision(deviceId, &appDemoFap20mParams.rev_ctrl, GT_TRUE);

    ret = fap20m_init_sequence_phase1(deviceId, &hw_ajust, GT_TRUE);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fap20m_init_drc_and_dram(deviceId,
                                   &hw_ajust.drc_conf,
                                   &hw_ajust.dram_conf,
                                   hw_ajust.load_drc_conf,
                                   hw_ajust.load_dram_conf,
                                   GT_TRUE
    );

    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fap20m_set_fap_id(deviceId, fapId);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /*
     * Checking the device-type and not only the all system,
     * for cases that "dune_rpc_" is not initialized.
     */
    link_fc.rx_enable = 0xFFFFFF;
    link_fc.tx_enable = 0xFFFFFF;

    ret = fap20m_fabric_link_flow_control_set(deviceId, &link_fc);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /*
     * Link interface
     */
    for (i = 0; i < FAP20M_NOF_DATA_PORTS; i++)
    {
        link_interface = appDemoFap20mParams.link_if[i];

        fap20m_set_port_link_interface(deviceId,
                                       appDemoFap20mParams.sch_rank_of_fabric,
                                       i,
                                       link_interface
                                       );
    }

    ret = fap20m_init_sequence_phase2(deviceId);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    DBG_INFO(("    + fap20m_init_sequence_phase2(%u, TRUE)                    -- success.\n\r"
             "      Transmission of all control cell from device are enabled.\n\r",
             deviceId));

    DBG_INFO(("  + END Initialization Sequence.\n\r"));
    DBG_INFO(("\n\r"));

    return GT_OK;
}

/*******************************************************************************
* appDemoFap20mQdpInit
*
* DESCRIPTION:
*       Initialization of QDP of the FAP20M device.
*
* INPUTS:
*       deviceId      - device Id.
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
static GT_STATUS appDemoFap20mQdpInit
(
    IN unsigned int     deviceId
)
{
    unsigned long
      ret,
      field_val;
    unsigned int
      dram_is_working = GT_FALSE;
    SAND_RET
      sand_ret;
    unsigned long
      top_buff_blk_0,
      top_buff_blk_1,
      top_buff_blk_2;
    GT_STATUS   rc;         /* return code */

    /*
     * set credit size
     */
    ret = fap20m_set_credit_size(deviceId,
                                 appDemoFap20mParams.credit_size
                                 );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    DBG_INFO(("  + Credit Size used: %u\n\r", appDemoFap20mParams.credit_size));

    rc = appDemoFap20mSetQueueParams(deviceId);
    if(rc != GT_OK)
    {
      return rc;
    }

    /*
     * Top Queue in Block - Make block-0 take all DRAM buffers and queues.
     *
     * Top block 0: 8181 for due to errata
     * Top block 1: 8181 for due to errata
     * Top block 2: 8181 for due to errata
     */


    field_val = 8181;
    ret = fap20m_logical_write(
                   deviceId, &field_val, FAP20M_TOP_QUE_BLK_0_FIELD
                 );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    field_val = 8181;
    ret = fap20m_logical_write(
                   deviceId, &field_val, FAP20M_TOP_QUE_BLK_1_FIELD
                 );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    field_val = 8181;
    ret = fap20m_logical_write(
                   deviceId, &field_val, FAP20M_TOP_QUE_BLK_2_FIELD
                 );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    /*
     * Top Buffer in Block - Make block-0 take all DRAM buffers and queues.
     */

    switch (appDemoFap20mParams.qdr_nof_buff_mem_field)
    {
    case 1: /*0.5M buffers (QDR)*/
      top_buff_blk_0 = APPDEMO_FAP20M_TOP_BUFF_BLK_0_QDR_0_5;
      top_buff_blk_1 = APPDEMO_FAP20M_TOP_BUFF_BLK_1_QDR_0_5;
      top_buff_blk_2 = APPDEMO_FAP20M_TOP_BUFF_BLK_2_QDR_0_5;
      break;

    case 2: /*1M buffers (QDR)*/
      top_buff_blk_0 = APPDEMO_FAP20M_TOP_BUFF_BLK_0_QDR_1_0;
      top_buff_blk_1 = APPDEMO_FAP20M_TOP_BUFF_BLK_1_QDR_1_0;
      top_buff_blk_2 = APPDEMO_FAP20M_TOP_BUFF_BLK_2_QDR_1_0;
      break;

    case 3: /*2M buffers (QDR)*/
      top_buff_blk_0 = APPDEMO_FAP20M_TOP_BUFF_BLK_0_QDR_2_0;
      top_buff_blk_1 = APPDEMO_FAP20M_TOP_BUFF_BLK_1_QDR_2_0;
      top_buff_blk_2 = APPDEMO_FAP20M_TOP_BUFF_BLK_2_QDR_2_0;
      break;

    default:
        return GT_BAD_PARAM;
    }

    ret = fap20m_logical_write(deviceId,
                               &top_buff_blk_0,
                               FAP20M_TOP_BUFF_BLK_0_FIELD
                               );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fap20m_logical_write(deviceId,
                               &top_buff_blk_1,
                               FAP20M_TOP_BUFF_BLK_1_FIELD
                               );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fap20m_logical_write(deviceId,
                               &top_buff_blk_2,
                               FAP20M_TOP_BUFF_BLK_2_FIELD
                               );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    /*
     * After setting the queue block and sizes, the QDP must be init.
     */
    ret = fap20m_QDP_init_sequence(deviceId, 1000000, &dram_is_working);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    if (dram_is_working == GT_FALSE)
    {
        DBG_INFO(("    + After 'fap20m_QDP_init_sequence', DRAM is not working.\n\r"));
        return GT_BAD_STATE;
    }

    return GT_OK;
}


/*******************************************************************************
* appDemoFap20mSetQueueParams
*
* DESCRIPTION:
*       Initialization of Queue parameters of the FAP20M device.
*
* INPUTS:
*       deviceId      - device Id.
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
static GT_STATUS appDemoFap20mSetQueueParams
(
    IN unsigned int                deviceId
)
{
    unsigned long
      ret;
    SAND_RET
      sand_ret;
    FAP20M_QUEUE_TYPE
      queue_type_i;
    FAP20M_QUEUE_TYPE_PARAM
      queue_type_param,
      exact_queue_type_param;
    FAP20M_WRED_PARAM
      wred_param,
      exact_wred_param_given;
    unsigned int
      drop_precedence_i;

    for (queue_type_i = FAP20M_QUEUE_0; queue_type_i < FAP20M_NOF_QUEUE_TYPES;
          ++queue_type_i)
    {
        queue_type_param.RED_enable     = FALSE;
        queue_type_param.RED_exp_weight = 0;
        /*
         * 64 Mega-Byte is the DRAM size.
         * Set the max Queue size to be 1/4 of the DRAM.
         */
        queue_type_param.max_q_size          = (64*1024*1024)/4;
        queue_type_param.flow_stat_msg       = 3*1024;
        queue_type_param.flow_slow_th_up     = 4*1024;
        queue_type_param.hysteresis_size     = 2*1024;
        queue_type_param.flow_slow_hys_en    = TRUE;
        queue_type_param.header_compensation = 4;

        ret = fap20m_set_queue_type_params(
                       deviceId,
                       queue_type_i,
                       &queue_type_param,
                       &exact_queue_type_param
                     );
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }

        for (drop_precedence_i = 0; drop_precedence_i < 4; ++drop_precedence_i)
        {

            wred_param.max_probability = 100;

            /*
             * wred_param.max_packet_size = 9500;
             */
            sand_u64_clear(&wred_param.max_packet_size);
            sand_u64_add_long(&wred_param.max_packet_size, 9500);

            switch (drop_precedence_i)
            {
            case 0:
              /*
               * In drop precedence ZERO we want the lowest drop probability of any.
               */
              wred_param.min_avrg_th = 2*1024-2; /* */
              wred_param.max_avrg_th = 2*1024;   /* */
              break;
            case 1:
              wred_param.max_avrg_th = 1648;  /* */
              wred_param.min_avrg_th = wred_param.max_avrg_th-2;
              break;
            case 2:
              wred_param.max_avrg_th = 1304;  /*  */
              wred_param.min_avrg_th = wred_param.max_avrg_th-2;
              break;
            case 3:
              wred_param.max_avrg_th = 1024;  /* */
              wred_param.min_avrg_th = wred_param.max_avrg_th-2;
              break;
            }

            ret = fap20m_set_wred_params(
                           deviceId,
                           queue_type_i,
                           drop_precedence_i,
                           &wred_param,
                           &exact_wred_param_given
                         );
            sand_ret = sand_get_error_code_from_error_word(ret) ;
            if (sand_ret != SAND_OK)
            {
                return GT_FAIL;
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoFap20mSetSystemRelatedSettings
*
* DESCRIPTION:
*       Initialization of Queue parameters of the FAP20M device.
*
* INPUTS:
*       deviceId      - device Id.
*       max_nof_faps_in_system  -   maximal number of FAPs in system
*       max_nof_fes_in_system   -   maximal number of FEs in system
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
static GT_STATUS appDemoFap20mSetSystemRelatedSettings
(
    IN unsigned int     deviceId,
    IN unsigned int     max_nof_faps_in_system,
    IN unsigned int     max_nof_fes_in_system
)
{
    unsigned long
      ret=0;
    SAND_RET
      sand_ret;
    unsigned int
      call_single_fap_mode;

    ret =
      fap20m_detect_if_single_fap_mode_required(
        deviceId,
        &call_single_fap_mode
      );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    if(call_single_fap_mode)
    {
      ret = fap20m_set_single_fap_mode(deviceId, GT_TRUE);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }

      DBG_INFO(("  + fap20m_set_single_fap_mode(%u, TRUE)   -- success.\n\r", deviceId));
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoFap20mSetSystemRelatedSettings
*
* DESCRIPTION:
*       Initialization of Queue parameters of the FAP20M device.
*
* INPUTS:
*       deviceId      - device Id.
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
static GT_STATUS appDemoFap20mSetMulticastSpatialApp
(
    IN unsigned int            deviceId
)
{
    unsigned long       ret = SAND_OK;
    FAP20M_SMS_CONF     sms_conf,
                        exact_sms_conf_given;
    SAND_RET            sand_ret;
    FAP20M_SPATIAL_MULTICAST_FLOW   sp_flow;
    unsigned long       i;                  /* loop iterator */
    unsigned long       dist_uni_mask_new;

    sms_conf.guaranteed_rate        = 6000; /*Mb/sec*/
    sms_conf.guaranteed_burst_size  = 1000;  /*KB*/
    sms_conf.best_effort_rate       = 18000; /*Mb/sec*/
    sms_conf.best_effort_burst_size = 1000;  /*KB*/
    ret = fap20m_set_SMS_configuration(deviceId,
                                       &sms_conf,
                                       &exact_sms_conf_given
                                       );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    sp_flow.spatial_distribution_map    = 0xFFFFFF;
    sp_flow.spatial_distribution_map_op = SAND_NOP;
    
    for (i = APPDEMO_FAP20M_FIRST_MG_ID; i <= APPDEMO_FAP20M_LAST_MG_ID; ++i)
    {
      ret = fap20m_open_spatial_multicast_flow(deviceId, i, &sp_flow) ;
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }
    }
    
    dist_uni_mask_new = 0xFFFFFF;
    ret = fap20m_spatial_multicast_active_links_set(deviceId, dist_uni_mask_new);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
      return GT_FAIL;
    }

    DBG_INFO(("appDemoFap20mSetMulticastSpatialApp: Set SMS -- success.\n\r"));

    return GT_OK;
}

/*******************************************************************************
* appDemoFap20mLoadParams
*
* DESCRIPTION:
*       Sets default values of Fap20m parameters.
*
* INPUTS:
*       ppType      - PP type: Puma2_24G or Puma2_2XG.
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
static void  appDemoFap20mLoadParams
(
    IN APPDEMO_FAP20M_PP_TYPE_ENT  ppType
)
{
    unsigned int    i;              /* loop iterator */
    unsigned long   scheduler_id;   /* scheduler ID */
  /************************************************************************/
  /* General Parameters                                                   */
  /************************************************************************/

  appDemoFap20mParams.reset_fap = FALSE;

  /************************************************************************/
  /* FAP20M Related System Parameters                                     */
  /************************************************************************/

  appDemoFap20mParams.credit_size             = SAND_CR_512;
  appDemoFap20mParams.qdr_nof_buff_mem_field  = 2;
  appDemoFap20mParams.qdr_burst_length        = 0x1;
  appDemoFap20mParams.qdr_read_delay          = 0x3;
  appDemoFap20mParams.qdr_data_valid_delay    = 0;
  appDemoFap20mParams.qdr_start_burst_delay   = 0;
  appDemoFap20mParams.dram_buff_size_field    = 1;

  /************************************************************************/
  /* External Interface Related System Parameters                         */
  /************************************************************************/

  /* The maximal number of ports in FAP is 26: 24 for Giga ports and 2 for XG ports,
     so there 13 ports per each HyperG Link */
  appDemoFap20mParams.max_nof_ports_per_hgl = 13;

  /* check PP type 24G or 2XG */
  if (ppType == APPDEMO_FAP20M_PP_EXMXPM_24G_E) 
  {
      /* 1G port */
      appDemoFap20mParams.nominal_port_rate_mbps = 1000;
  }
  else
  {
      /* 10G port */
      appDemoFap20mParams.nominal_port_rate_mbps = 10000;
  }

  /*
   * Speed up of the scheduler ports in percents.
   * 100 - no speed up.
   * 130 -speed up of 30%.
   */
  appDemoFap20mParams.port_speed_up = 130;

  for (i = 0; i < FAP20M_NOF_DATA_PORTS; i++)
  {
      appDemoFap20mParams.link_if[i] = FAP20M_LINK_INTERFACE_A;
  }


  /* initialize XG ports (24, 25) */
  scheduler_id = appDemoFap20mPortShedulerIdGet(24);
  appDemoFap20mParams.link_if[scheduler_id] = FAP20M_LINK_INTERFACE_A;
  scheduler_id = appDemoFap20mPortShedulerIdGet(25);
  appDemoFap20mParams.link_if[scheduler_id] = FAP20M_LINK_INTERFACE_B;



  /************************************************************************/
  /* General Register Configuration                                       */
  /************************************************************************/

  appDemoFap20mParams.nof_general_fields = 0;
  /*
   * if 'nof_general_fields' differnet from ZERO, add entries at:
   * 'FAP20M_FIELD  general_fields[FAP20M_SWEEP_MAX_NOF_FIELDS]'
   */

  appDemoFap20mParams.default_sch_scheme.scheme_id = APPDEMO_FAP20M_SCH_SCHEME_STRICT;
  appDemoFap20mParams.default_sch_scheme.minimum_rate_by_percentage = 0;
  appDemoFap20mParams.default_sch_scheme.maximum_rate_by_percentage = 0;

  appDemoFap20mParams.sch_rank_of_fabric = FAP20M_SCH_RANK_PRIM;
  /************************************************************************/
  /* Revision control                                                     */
  /************************************************************************/
  appDemoFap20mParams.rev_ctrl.user_application_rev = FAP20M_DEV_REV_0;
}

/*******************************************************************************
* fx950Task
*
* DESCRIPTION:
*       Performs FX950 task.
*       1. Perform continuous polling of the FAP mode by 
*          detect_if_single_fap_mode_required() API. If the mode on line is single 
*          it should be changed to be multiple, in order to work in chassy system.
*
* INPUTS:
*       unused - this argument is not used.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static unsigned __TASKCONV fx950Task
(
   GT_VOID * param
)
{
    unsigned int    singleFapModeRequired;
    unsigned int    newSingleFapMode;
    unsigned int    deviceId;
    unsigned long   ret;                    /* FAP20M return code   */
    SAND_RET        sand_ret;               /* SAND return code     */

    deviceId = (unsigned int)param;

    /*  If the system is designed to be with ONE fap, SINGLE-FAP mode should be set.
     *  If the system is designed to be with more than one FAP:
     *      If it start from ONE fap, the FAP is set first to be to SINGLE-FAP mode.
     *          When at least, one more FAP is added to the system, SINGLE-FAP mode 
     *          should be disabled.
     *      If the system starts with many FAPs, and all FAPs (but one) are removed, 
     *      set the remaining FAP to be in SINGLE-FAP mode. 
     *      (again, change it when another FAP is in)*/
    while (1)
    {
        osTimerWkAfter(1000);

        ret = fap20m_detect_if_single_fap_mode_required(deviceId, &singleFapModeRequired);
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }

        if (singleFapModeRequired == FALSE) 
        {
            /* set the FAP to be in multiple mode */
            newSingleFapMode = FALSE; 
        }
        else
        {
            newSingleFapMode = TRUE; 
        }

        ret = fap20m_set_single_fap_mode(deviceId, newSingleFapMode); 
        sand_ret = sand_get_error_code_from_error_word(ret) ;
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }
    }
}


#ifdef ASIC_SIMULATION

SAND_RET
  appDemo_fap20m_simulation_physical_read(
    SAND_INOUT  unsigned long *array,
    SAND_IN     unsigned long *base_address,
    SAND_IN     unsigned long offset,
    SAND_IN     unsigned long size
 )
{
    GT_U32  address;
    GT_U32   local_size;


    if (offset >= FX950_ADDRESS_MAX_OFFSET)
    {
        return SAND_ERR;
    }

    local_size = size >> 2 ;

    address = offset;

    scibReadMemory(1, address, local_size, (GT_U32*)array);

    return SAND_OK ;
}


SAND_RET
  appDemo_fap20m_simulation_physical_write(
    SAND_IN     unsigned long *array,
    SAND_INOUT  unsigned long *base_address,
    SAND_IN     unsigned long offset,
    SAND_IN     unsigned long size
 )
{
    GT_U32  address;
    GT_U32   local_size;

    if (offset >= FX950_ADDRESS_MAX_OFFSET)
    {
        return SAND_ERR;
    }

    local_size    = size >> 2 ;

    address = offset;

    scibWriteMemory(1, address, local_size, (GT_U32*)array);

    SAND_SYNC_IOS;

    return SAND_OK ;
}

#endif


GT_STATUS dumpFap20mRegs(
    IN  GT_U8    devNum,
    IN  GT_U32   registerOffset
)
{
    unsigned long   *devBaseAddress;
    unsigned long   hwValue;
    SAND_RET    sand_ret;


    devBaseAddress = sand_get_chip_descriptor_base_addr(devNum);

    sand_ret = sand_physical_read_from_chip(
                 &hwValue,
                 devBaseAddress,
                 registerOffset,
                 sizeof(hwValue)
               );
    if(sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    cpssOsPrintf("regAddr = 0x%X, data = 0x%X\n\r", registerOffset, hwValue);

    return GT_OK;
}

GT_STATUS   dumpFap20mMemory(
    IN  GT_U8           devNum,
    IN  unsigned long   startOffset,
    IN  unsigned long   numOfWords
)
{
    unsigned long   *devBaseAddress;
    SAND_RET        sand_ret;
    unsigned long   i, j;           /* loop iterators */
    unsigned long   buffer[4];
    unsigned long   size = 4;       /* number of words to read each time */

    devBaseAddress = sand_get_chip_descriptor_base_addr(devNum);
    size = 4; /* 4 words each time */

    for (i = 0; i < numOfWords;)
    {
        if (i + 4 > numOfWords)
        {
            /* last buffer to read */
            size = (numOfWords - i + 1);
        }

        sand_ret = sand_mem_read(devNum,
                                 &buffer[0],
                                 startOffset,
                                 (size * 4), /* the size is in bytes */
                                 FALSE);
        if (sand_ret != SAND_OK)
        {
            return GT_FAIL;
        }

        /* print buffer */
        for (j = 0; j < size; j++)
        {
            cpssOsPrintf("0x%x      ",buffer[j]);
        }
        cpssOsPrintf("\n\r");

        i = i + 4 ;
        startOffset += (4 * 4);
    }

    return GT_OK;
}


void
  fap20m_diagnostics_dump_QDP_PART1_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[10] =
  {
    SAND_IND_PRINT_INFO(FAP20M_QDP_QUT_IND),
    SAND_IND_PRINT_INFO(FAP20M_QDP_QTP_IND),
    SAND_IND_PRINT_INFO(FAP20M_QDP_QWP_IND),
    SAND_IND_PRINT_INFO(FAP20M_QDP_QUEUE_SIZE_IND),
    SAND_IND_PRINT_INFO(FAP20M_QDP_AVRG_QUEUE_SIZE_IND),
    /*SAND_IND_PRINT_INFO(FAP20M_QDP_AVAIL_CREDIT_IND), */
    SAND_IND_PRINT_INFO(FAP20M_QDP_WRITE_DESC_IND),
    SAND_IND_PRINT_INFO(FAP20M_QDP_READ_DESC_IND),
    SAND_IND_PRINT_INFO(FAP20M_QDP_QFL_IND),

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}

void
  fap20m_diagnostics_dump_QDP_PART2_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[2] =
  {
    SAND_IND_PRINT_INFO(FAP20M_QDP_TAG_DESC_IND),

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}


void
  fap20m_diagnostics_dump_QDP_PART3_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[5] =
  {
      SAND_IND_PRINT_INFO(FAP20M_QDP_NON_EMPTY_IND),
      SAND_IND_PRINT_INFO(FAP20M_QDP_QBK_IND),
      SAND_IND_PRINT_INFO(FAP20M_QDP_CREDIT_SCOREBOARD_IND),
      SAND_IND_PRINT_INFO(FAP20M_QDP_AVAIL_CREDIT_IND),

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}


void
  fap20m_diagnostics_dump_QDP_PART4_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[2] =
  {
    SAND_IND_PRINT_INFO(FAP20M_QDP_LINK_DESC_IND),

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}


void
  fap20m_diagnostics_dump_QDP_QFL_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[2] =
  {
    {FAP20M_QDP_QFL_IND_ADDR, 0x1800, "FAP20M_QDP_QFL_IND"},

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}


void
  fap20m_diagnostics_dump_RTP_FDT_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[7] =
  {
      SAND_IND_PRINT_INFO(FAP20M_RTP_UDS_IND),
      SAND_IND_PRINT_INFO(FAP20M_RTP_SMD_IND),

      SAND_IND_PRINT_INFO(FAP20M_FDT_DU4_IND),
      SAND_IND_PRINT_INFO(FAP20M_FDT_DM4_IND),
      SAND_IND_PRINT_INFO(FAP20M_FDT_DU2_IND),
      SAND_IND_PRINT_INFO(FAP20M_FDT_DM2_IND),

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}


void
  fap20m_diagnostics_dump_SCH_PRIM_PART1_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[10] =
  {
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_LINKA_CAL_0_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_LINKA_CAL_1_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_LINKB_CAL_0_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_LINKB_CAL_1_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM(FAP20M_SCH_DRM_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_FFQ_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_FDM_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM(FAP20M_SCH_SHD_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_SEM_IND, FAP20M_SCH_RANK_PRIM),

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}


void
  fap20m_diagnostics_dump_SCH_PRIM_PART2_indirect_tables(
    SAND_IN unsigned int device_id,
    SAND_IN unsigned int print_zero
  )
{
  SAND_RET
    ret = SAND_OK;
  SAND_INDIRECT_PRINT_INFO
    indirect_print[10] =
  {
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_FSF_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_FGM_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_SHC_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_SCC_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM(FAP20M_SCH_SCT_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_FIM_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM_BOTH(FAP20M_SCH_FSM_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM(FAP20M_SCH_SHDD_IND, FAP20M_SCH_RANK_PRIM),
      SAND_IND_PRINT_INFO_PRM(FAP20M_SCH_DHD_IND, FAP20M_SCH_RANK_PRIM),

    {(unsigned long)-1,(unsigned long)-1,NULL}
  };

  unsigned long print_options_bm=0;

  SAND_INIT_ERROR_DEFINITIONS(FAP20M_DIAGNOSTICS_DUMP_INDIRECT_TABLES);

  print_options_bm |= SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SAND_DONT_PRINT_ZEROS;
  }
  ret = sand_print_indirect_tables(
          device_id,
          indirect_print,
          print_options_bm
        );
  SAND_CHECK_FUNC_RESULT(ret, 10, exit);

exit:
  return;
}

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
)
{
    FAP20M_AUTO_PORT_RELATIVE_ID     relative_port_id;
 
    
    /*
     * Relative port id is some kind of Packet Processor (PP) port number analog
     * Relative port id == (PP_port_number + 1) % 64
     * This relation must be correlated with labeling configuration. The    
     * "port_add_enable" option must be set in the FX950 labeling configuration. 
     * PP port number 63 is CPU port gets relative id 0
     * PP port number 0 get relative id 1 and so on.
     * Actual port id is id of port scheduler which is responsible for PP's port
     * The relation is:
     *  - CPU port's scheduler IDs are 0..7, only 0 is used by auto manager
     *  - PP port's scheduler ID is = PP_port_number + 8
     */
  
    /* convert PP port number to the auto manager relative port id */
    relative_port_id = ((pp_port_num + 1) % 64) | SAND_BIT(FAP20M_AUTO_PORT_AUTO_RELATIVE_ID_BIT); ;
    
    /* get sheduler ID */
    return fap20m_auto_get_port_actual_id(relative_port_id );
 
}


