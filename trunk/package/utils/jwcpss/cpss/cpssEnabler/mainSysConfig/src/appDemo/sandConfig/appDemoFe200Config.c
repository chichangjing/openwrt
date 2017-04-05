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
* appDemoFe200Config.c
*
* DESCRIPTION:
*       Initialization functions for the Fe200 boards.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

/*
 * FMC_N system-wide includ file. Must be first on every 'c' file
 * within FMC_N.
 */
/*#include "./../../general/include/fmc_n_sys_wide_defs.h"*/

/* { */
/*
 * SAND include files
 */
#include "SAND/SAND_FM/include/sand_chip_defines.h"
#include "SAND/Management/include/sand_chip_consts.h"
#include "SAND/Management/include/sand_low_level.h"
#include "SAND/Management/include/sand_module_management.h"
#include "SAND/Management/include/sand_chip_descriptors.h"
#include "SAND/Utils/include/sand_bitstream.h"
#include "SAND/Utils/include/sand_integer_arithmetic.h"
#include "SAND/Utils/include/sand_trace.h"
#include "SAND/Utils/include/sand_os_interface.h"
#include "SAND/Utils/include/sand_framework.h"

/*
 * FE200 include files
 */
#include "FE200/include/fe200_chip_defines.h"
#include "FE200/include/fe200_api_framework.h"
#include "FE200/include/fe200_api_links.h"
#include "FE200/include/fe200_api_general.h"
#include "FE200/include/fe200_api_configuration.h"
#include "FE200/include/fe200_api_init.h"

#include "FE200/include/fe200_api_routing.h"
/*
 * FMC_N include files
 */

/*#include "./../../general/include/fmc_n_dynamic_services_tables.h"*/
/*#include "./../../general/include/fmc_n_errors_and_debug.h"*/
/*#include "./../include/fmc_n_fe200_register_services.h"*/

#include <appDemo/sandConfig/appDemoFe200Config.h>


#if APPDEMO_DEBUG_PRINT
/* { */
#include "vxworks.h"
#include "semlib.h"
#include "msgQLib.h"
#include "objLib.h"
#include "errnoLib.h"
#include "string.h"
#include "stdio.h"
#include "usrApp.h"
#include UI_DEFX_H
#include UI_PURE_DEFI_H
/* } */
#endif

/*
 * This function is used to as MACRO parameter
 * filler for 'APPDEMO_PROC_AND_ERR'
 */
static
  void
    fmc_n_init_fe200_empty_call(
      void
    )
{
  /*
   * Do nothing.
   */
  return;
}


/*****************************************************
*NAME
*  fmc_n_init_fe200_device
*TYPE: PROC
*DATE: 22/DEC/2003
*FUNCTION:
*  This procedure initializes FE200 device under FMC.
*INPUT:
*  DIRECT:
*    NONE
*OUTPUT:
*  DIRECT:
*    int -
*      If non-zero then an error condition has been
*      encountered and procedure could not accomplish
*      its assigned task.
*SEE ALSO:
******************************************************/
int
  appDemoFe200InitDevice(
    SAND_IN  APPDEMO_FE200_INIT_PARAMS*     fmc_n_fe200_init_params,
    SAND_IN  SAND_RESET_DEVICE_FUNC_PTR   reset_device_ptr,
    SAND_IN  SAND_FMF_DEVICE_TYPE         device_type,
    SAND_IN  SAND_PHYSICAL_ACCESS*        physical_access,
    SAND_OUT unsigned int*                device_handle,
    SAND_OUT unsigned long*               chip_id
  )
{
  unsigned int
    ret,
    rev_b_or_above;
  unsigned long
    ulong_var,
    fe2_not_fe13;
  FE200_HW_ADJUSTMENTS
    hw_ajust;
  FE200_FABRIC_LINK_FC
    link_fc;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003, ERR_004, ERR_005,
    ERR_006, ERR_007, ERR_008, ERR_009, ERR_010,
    ERR_011, ERR_012, ERR_013, ERR_014, ERR_015,
    ERR_016, ERR_017, ERR_018, ERR_019, ERR_020,
    ERR_021, ERR_022, ERR_023, ERR_024, ERR_025,
    ERR_026, ERR_027,
    NUM_ERRORS
  } ;

  APPDEMO_INSERT_DEBUG_VARS;
  APPDEMO_INSERT_DEBUG_PRELUDE("fmc_n_init_fe200_device");

  rev_b_or_above = FALSE;

  /*
   * Here, the FE200, is after reset.
   *
   * FE200-REV-A note:
   * Set the RMGR to be ZERO as soon as it out of reset. After all the
   * application is loaded, change it.
   *
   * Example code - to change the RMGR:
   * 1. Take FE out of reset.
   * 2. offset = SAND_OFFSETOF(FE_REGS,fe_rtp_eci_regs.fe_rtp_eci_detail.routing_processor_enablers);
   *    shift = RMGR_SHIFT;
   *    mask  = RMGR_MASK;
   *    address =  (unsigned long*)(FE_1_BASE_ADDR| offset);
   *    reg_val =  (*address);
   *    reg_val &= ~mask ;
   *    reg_val |= SET_FLD_IN_PLACE(0, shift, mask) ;
   *    (*address) =  reg_val;
   * 3. Run "fmc_n_init_fe200_device()" function.
   * 4. Run "fe200_init_sequence_phase2()" function.
   */



  /*
   * Try to register deivce.
   */
  error_id =  fe200_register_device(
                  fmc_n_fe200_init_params->base_address,
                  reset_device_ptr,
                  device_handle
                );
  sand_err = sand_get_error_code_from_error_word(error_id);

  if (sand_err == SAND_OK)
  {
    /* All is OK */
  }
  else if ( sand_err == SAND_REGISTER_DEVICE_005)
  {
    /*Device do not exist*/
    sand_os_printf(
      "\n\r"
      "\n\r"
      "*** Device do not exist ****\n\r"
      "\n\r"
    );
    *device_handle = -1;
    goto exit;
  }
  else
  {
    /*Other error*/
      err = ERR_001;
      goto exit ;
  }

  /*
   * Get into 'init' stage (logical).
   */
  APPDEMO_PROC_AND_ERR(fe200_start_init_stage(*device_handle),ERR_002,fmc_n_init_fe200_empty_call()) ;

  /*
   * Get if device version, is REV-B or above.
   */
  rev_b_or_above = sand_is_chip_descriptor_chip_ver_bigger_eq(*device_handle, FE200_EXPECTED_CHIP_VER_02);

  /*
   * Init Phase 1.
   */
  hw_ajust.load_serdes_conf = fmc_n_fe200_init_params->change_serdes_status;
  sand_os_memcpy(
    &hw_ajust.all_serdes_status,
    fmc_n_fe200_init_params->serdes_status_ptr,
    sizeof(hw_ajust.all_serdes_status)
  );
  APPDEMO_PROC_AND_ERR(
    fe200_init_sequence_phase1(
      *device_handle,
      &hw_ajust,
      FALSE)
    ,ERR_023
    ,
    fe200_end_init_stage(*device_handle)
  ) ;

  if (fmc_n_fe200_init_params->is_rev_a_device_in_system == FALSE)
  {
    /*
     * System with REV-B and above.
     */

    /*
     * Enable Fabric Link-Level FC.
     */
    link_fc.rx_enable[0] = 0xFFFFFFFF;
    link_fc.rx_enable[1] = 0xFFFFFFFF;
    link_fc.tx_enable[0] = 0xFFFFFFFF;
    link_fc.tx_enable[1] = 0xFFFFFFFF;
    APPDEMO_PROC_AND_ERR(
      fe200_fabric_link_flow_control_set(
        *device_handle,
        &link_fc
      ),
      ERR_027,
      fe200_end_init_stage(*device_handle)
    );
  }

  if (fmc_n_fe200_init_params->chip_id.change_var)
  {
    *chip_id =  (unsigned long)(fmc_n_fe200_init_params->chip_id.var) ;
    APPDEMO_PROC_AND_ERR(
      fe200_logical_write(
        *device_handle,chip_id,FE200_CHIP_ID_FIELD),ERR_003,
        fe200_end_init_stage(*device_handle)) ;
  }
  if (fmc_n_fe200_init_params->initial_profile_id.change_var)
  {
    unsigned
      long
        profile_id ;
    profile_id = (unsigned long)(fmc_n_fe200_init_params->initial_profile_id.var) ;
    APPDEMO_PROC_AND_ERR(
      fe200_set_profile(*device_handle,&profile_id),ERR_004,
        fe200_end_init_stage(*device_handle)) ;
  }
  if (fmc_n_fe200_init_params->fe2_not_fe13.change_var)
  {
    /*
     * Do not change if device is in required state!
     */
    APPDEMO_PROC_AND_ERR(
      fe200_logical_read(
        *device_handle,&fe2_not_fe13,FE200_MODE_FE2_FIELD),ERR_005,
        fe200_end_init_stage(*device_handle)) ;
    /*
     * Note:
     * It is the caller's responsibility to set/reset only the relevant field in
     * 'var'!
     */
    ulong_var = (unsigned long)(fmc_n_fe200_init_params->fe2_not_fe13.var) ;
    if (fe2_not_fe13 != ulong_var)
    {
      unsigned
        long
          current_cdispr ;
      APPDEMO_PROC_AND_ERR(
        fe200_stop_receiving_control_cells(
          *device_handle,&current_cdispr),ERR_006,
          fe200_end_init_stage(*device_handle)) ;
      fe2_not_fe13 = ulong_var ;
      APPDEMO_PROC_AND_ERR(
        fe200_logical_write(
          *device_handle,&fe2_not_fe13,FE200_MODE_FE2_FIELD),ERR_007,
          fe200_end_init_stage(*device_handle)) ;
      APPDEMO_PROC_AND_ERR(
        fe200_start_receiving_control_cells(
          *device_handle,&current_cdispr),ERR_008,
          fe200_end_init_stage(*device_handle)) ;
    }
  }
  if (fmc_n_fe200_init_params->is_multi_stage.change_var)
  {
    unsigned
      long
        is_multi_stage ;
    /*
     * Do not change if device is in required state!
     */
    APPDEMO_PROC_AND_ERR(
      fe200_logical_read(
        *device_handle,&is_multi_stage,FE200_MULTISTAGE_FIELD),ERR_009,
        fe200_end_init_stage(*device_handle)) ;
    /*
     * Note:
     * It is the caller's responsibility to set/reset only the relevant field in
     * 'var'!
     */
    ulong_var = (unsigned long)(fmc_n_fe200_init_params->is_multi_stage.var) ;
    if (is_multi_stage != ulong_var)
    {
      is_multi_stage = ulong_var ;
      APPDEMO_PROC_AND_ERR(
        fe200_logical_write(
          *device_handle,&is_multi_stage,FE200_MULTISTAGE_FIELD),ERR_010,
          fe200_end_init_stage(*device_handle)) ;
    }
  }
  if (fmc_n_fe200_init_params->nof_planes.change_var)
  {
    unsigned
      long
        nof_planes ;
    nof_planes = (unsigned long)(fmc_n_fe200_init_params->nof_planes.var) ;
    /*
     * If specified number of planes is above the maximum allowed,
     * quit with error.
     */
    if (nof_planes > APPDEMO_FE200_MAX_NUM_PLANES)
    {
      err = ERR_011 ;
      goto exit ;
    }
    APPDEMO_PROC_AND_ERR(
      fe200_logical_write(
        *device_handle,&nof_planes,FE200_PLANES_FIELD),ERR_012,
        fe200_end_init_stage(*device_handle)) ;
  }
  if (fmc_n_fe200_init_params->is_repeater.change_var)
  {
    unsigned
      long
        is_repeater ;
    APPDEMO_PROC_AND_ERR(
      fe200_logical_read(
        *device_handle,&is_repeater,FE200_REPEATER_FIELD),ERR_013,
        fe200_end_init_stage(*device_handle)) ;
    /*
     * Note: FE200 can only be put INTO repeater mode but can not
     * be taken out of it.
     */
    if ((is_repeater) && (!(fmc_n_fe200_init_params->is_repeater.var)))
    {
      err = ERR_014 ;
      goto exit ;
    }
    is_repeater = (unsigned long)(fmc_n_fe200_init_params->is_repeater.var) ;
    APPDEMO_PROC_AND_ERR(
      fe200_logical_write(
        *device_handle,&is_repeater,FE200_REPEATER_FIELD),ERR_015,
        fe200_end_init_stage(*device_handle)) ;
  }
  /*
   * Load local data base with up-to-date chip id.
   */
  {
    APPDEMO_PROC_AND_ERR(
      fe200_logical_read(*device_handle,chip_id,FE200_CHIP_ID_FIELD),ERR_016,
      fe200_end_init_stage(*device_handle)) ;
  }
  /*
   * After all setup, make sure the FE2 devices have the 'fe2_not_fe13'
   * set while FE13 devices do not.
   */
  {
    APPDEMO_PROC_AND_ERR(
      fe200_logical_read(
        *device_handle,&fe2_not_fe13,FE200_MODE_FE2_FIELD),ERR_017,
        fe200_end_init_stage(*device_handle)) ;
    if (fe2_not_fe13)
    {
      /*
       * Device is configured as FE2.
       */
      if (device_type != FE2_DEVICE)
      {
        fe200_end_init_stage(*device_handle) ;
        err = ERR_018 ;
        goto exit ;
      }
    }
    else
    {
      /*
       * Device is configured as FE13.
       * In this case, there is no point in setting 'multi-stage'.
       */
      if (device_type != FE13_DEVICE)
      {
        fe200_end_init_stage(*device_handle) ;
        err = ERR_019 ;
        goto exit ;
      }
      if (fmc_n_fe200_init_params->is_multi_stage.change_var)
      {
        fe200_end_init_stage(*device_handle) ;
        err = ERR_020 ;
        goto exit ;
      }
    }
  }
  /*
   * Set physical access for stage 2. For this device,
   * this is probably the same access procedure as in stage 1
   * (and is probably the default).
   * Set the physical access low level routines:
   */
  sand_err = sand_set_physical_access_hook(physical_access) ;
  if (sand_err != SAND_OK)
  {
    /*
     * System has failed to open SAND. Quit with error.
     */
    err = ERR_021 ;
    fe200_end_init_stage(*device_handle) ;
    goto exit ;
  }

  if(rev_b_or_above)
  {
    /*
     * Rev B application
     */

    /*
     * Set the MCI-4 Priorities enable.
     */
    ulong_var = (fmc_n_fe200_init_params->mci_4_prio_enable != FALSE);
    APPDEMO_PROC_AND_ERR(
      fe200_logical_write(
        *device_handle, &ulong_var, FE200_MCI_4_LVL_EN_FIELD),ERR_026,
      fe200_end_init_stage(*device_handle)) ;

  }

  /*
   * Get out of 'init' stage (logical).
   */
  APPDEMO_PROC_AND_ERR(fe200_end_init_stage(*device_handle),ERR_022,fmc_n_init_fe200_empty_call()) ;



  /*
   * Now, all we need to run is the "fe200_init_sequence_phase2()".
   * In Dune's application we do it later in the code, after all board is finished.
   * In case "fe200_init_sequence_phase2()" is not running, the FE will not be operational.
   */


exit:

  if (err == NO_ERR)
  {
    /*assert_fmc_n_init_already_invoked() ;*/
  }
  APPDEMO_INSERT_DEBUG_EPILOGUE;
  ret = err ;
  return (ret) ;
}



/*****************************************************
*NAME
*  fmc_n_fe200_spatial_multicast_app
*TYPE: PROC
*DATE: 22/DEC/2003
*FUNCTION:
*  This procedure configure the FE multicast table
*   to support the
*INPUT:
*  DIRECT:
*    NONE
*OUTPUT:
*  DIRECT:
*    int -
*      If non-zero then an error condition has been
*      encountered and procedure could not accomplish
*      its assigned task.
*SEE ALSO:
******************************************************/
unsigned long
  appDemoFe200SpatialMulticastApp(
    SAND_IN unsigned int        device_id,
    SAND_IN unsigned int        silent
  )
{
  unsigned long
    driver_ret ;
  unsigned int
    multicast_id;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003,
    NOF_ERR
  } err;
  DISTRIBUTION_REPLICATION_MULTI_REPORT
    mul_rep;
  FE2_REPLIC_MULTI_SHORT_ENTRY
    fe2_replic_multi;
  unsigned char
    *proc_name = "fmc_n_fe200_spatial_multicast_app";

  driver_ret = SAND_OK;
  err = NO_ERR;


  /*
   */
  fe2_replic_multi.replication_entry[0] = 0xFFFFFFFF;
  fe2_replic_multi.replication_entry[1] = 0xFFFFFFFF;

  mul_rep.reporting_entity_type = SAND_FE2_ENTITY;
  mul_rep.offset = 0x0; /*MG id*/
  mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[0] = 0x0;
  mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[1] = 0x0;


  /*
   * Load the set-up to the device.
   */
  for (multicast_id=0; multicast_id<=31; ++multicast_id)
  {
    mul_rep.offset = multicast_id; /*Multicast Group Id*/

    /*
     * We take the predefined multicast group replication, and do it again and again.
     */
    mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[0] =
      fe2_replic_multi.replication_entry[0];
    mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[1] =
      fe2_replic_multi.replication_entry[1];

    driver_ret = fe200_set_multicast_table_entry(
                   device_id,
                   (unsigned long*)(&mul_rep)
                 );
    if (sand_get_error_code_from_error_word(driver_ret) != SAND_OK)
    {
      err = ERR_001;
      goto  exit ;
    }
  }

  if (!silent)
  {
    sand_os_printf(
      "    + %s: Opened %u Spatial Multicast Flows (%u to %u) -- success.\n\r",
      proc_name,
      32,
      1,
      31
    );
  }

  if (!silent)
  {
    sand_os_printf(
      "    + %s(%u)                      -- success.\n\r",
      proc_name,
      device_id
    );
  }
exit:
  if (err)
  {
    sand_os_printf(
      "    + %s                         -- ERROR %u.\n\r",
      proc_name,
      err
    );
  }
  return err ;
}

/*****************************************************
*NAME
*  fe200_spatial_multicast_config_group_range
*TYPE: PROC
*DATE: 06/10/05
*FUNCTION:
*  This procedure configure the FE multicast table
*   to support the
*INPUT:
*  DIRECT:
*  device_id       - fe200 handle
*  silent          - BOOL nop zero value means to print trace
*  mc_group_base   - the base of VID/VIDX range
*  mc_group_amount - amount of VID/VIDXs
*OUTPUT:
*  DIRECT:
*    int -
*      If non-zero then an error condition has been
*      encountered and procedure could not accomplish
*      its assigned task.
*SEE ALSO:
******************************************************/
unsigned long
  appDemoFe200SpatialMulticastConfigGroupRange(
    SAND_IN unsigned int        device_id,
    SAND_IN unsigned int        silent,
    SAND_IN unsigned int        mc_group_base,
    SAND_IN unsigned int        mc_group_amount
  )
{
  unsigned long
    driver_ret ;
  unsigned int
    multicast_id;
  unsigned int mcIdMask = 0x03ff;
  unsigned int mcId;
  enum
  {
    NO_ERR = 0,
    ERR_001, ERR_002, ERR_003,
    NOF_ERR
  } err;
  DISTRIBUTION_REPLICATION_MULTI_REPORT
    mul_rep;
  FE2_REPLIC_MULTI_SHORT_ENTRY
    fe2_replic_multi;
  unsigned char
    *proc_name = "fe200_spatial_multicast_config_group_range";

  driver_ret = SAND_OK;
  err = NO_ERR;


  /*
   */
  fe2_replic_multi.replication_entry[0] = 0xFFFFFFFF;
  fe2_replic_multi.replication_entry[1] = 0xFFFFFFFF;

  mul_rep.reporting_entity_type = SAND_FE2_ENTITY;
  mul_rep.offset = 0x0; /*MG id*/
  mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[0] = 0x0;
  mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[1] = 0x0;


  /*
   * Load the set-up to the device.
   */
  for (mcId=mc_group_base;
        (mcId <= (mc_group_base + mc_group_amount));
        mcId++)
  {
      multicast_id = mcId & mcIdMask;

    mul_rep.offset = multicast_id; /*Multicast Group Id*/

    /*
     * We take the predefined multicast group replication, and do it again and again.
     */
    mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[0] =
      fe2_replic_multi.replication_entry[0];
    mul_rep.dist_replic_entry->fe2_replic_multi_short_entry.replication_entry[1] =
      fe2_replic_multi.replication_entry[1];

    driver_ret = fe200_set_multicast_table_entry(
                   device_id,
                   (unsigned long*)(&mul_rep)
                 );
    if (sand_get_error_code_from_error_word(driver_ret) != SAND_OK)
    {
      err = ERR_001;
      goto  exit ;
    }
  }

  if (!silent)
  {
    sand_os_printf(
      "    + %s: Opened %u Spatial Multicast Flows (%u to %u) -- success.\n\r",
      proc_name,
      mc_group_amount,
      mc_group_base,
      (mc_group_base + mc_group_amount - 1)
    );
  }

  if (!silent)
  {
    sand_os_printf(
      "    + %s(%u)                      -- success.\n\r",
      proc_name,
      device_id
    );
  }
exit:
  if (err)
  {
    sand_os_printf(
      "    + %s                         -- ERROR %u.\n\r",
      proc_name,
      err
    );
  }
  return err ;
}


/*
 * Clears various counters/indicators of the FE.
 */
unsigned long
  appDemoFe200ClearVariousCounters(
    SAND_IN unsigned int device_id
  )
{
  unsigned long
    driver_ret,
    dummy_val,
    offset_i,
    links[FE200_FE2_LINKS_NOF_LONGS];
  SAND_RET
    sand_ret;
  ALL_LINK_STATUS
    link_status;
  FE200_ALL_LINK_CONNECTIVITY
    link_connectivity;

  dummy_val = 0x0 ;
  sand_ret = SAND_OK;

  for (offset_i=0; offset_i<FE200_FE2_LINKS_NOF_LONGS; ++offset_i)
  {
    links[offset_i] = 0xFFFFFFFF;
  }
  for (offset_i=0; offset_i<FE200_MAX_NUM_FE2_LINKS; ++offset_i)
  {
    link_status.single_link_status[offset_i].valid = TRUE;
  }

  /*
   * Clears MAC counters at
   * 0x0b00 - MACA
   * 0x0bc0 - MACB
   * 0x0c80 -MACC
   * 0x0d40 -MACD
   * In each MAC the counters are at addresses (add to the specific offset):
   * 0x50 ->0x8e
   */
  for (offset_i=0; offset_i < (sizeof(FE_MAC_MAC_DETAIL)/sizeof(long)); offset_i+=4)
  {
    driver_ret = fe200_mem_read(
                   device_id,
                   &dummy_val,
                   offset_i + SAND_OFFSETOF(FE_REGS, fe_mac_mac_regs.fe_mac_mac_detail.mac_tx_cntrs_1),
                   sizeof(dummy_val),
                   FALSE) ;
    sand_ret = sand_get_error_code_from_error_word(driver_ret);
    if (sand_ret)
    {
      goto exit;
    }
  }

  /*
   * Get the link status, to clear the CRC.
   */
  driver_ret = fe200_get_links_status(
                 device_id,
                 &link_status,
                 links
               ) ;
  sand_ret = sand_get_error_code_from_error_word(driver_ret);
  if (sand_ret)
  {
    goto exit;
  }
  driver_ret = fe200_get_links_status(
                 device_id,
                 &link_status,
                 links
               ) ;
  sand_ret = sand_get_error_code_from_error_word(driver_ret);
  if (sand_ret)
  {
    goto exit;
  }

  /*
   * Get the link status, to clear the CRC.
   */
  driver_ret = fe200_get_links_status(
                 device_id,
                 &link_status,
                 links
               ) ;
  sand_ret = sand_get_error_code_from_error_word(driver_ret);
  if (sand_ret)
  {
    goto exit;
  }
  driver_ret = fe200_get_links_status(
                 device_id,
                 &link_status,
                 links
               ) ;
  sand_ret = sand_get_error_code_from_error_word(driver_ret);
  if (sand_ret)
  {
    goto exit;
  }

  /*
   * connectivity map and errors
   */
  driver_ret = fe200_get_connectivity_map(
                 device_id,
                 &link_connectivity
               );
  if (sand_ret)
  {
    goto exit;
  }


exit:
  return sand_ret ;
}


