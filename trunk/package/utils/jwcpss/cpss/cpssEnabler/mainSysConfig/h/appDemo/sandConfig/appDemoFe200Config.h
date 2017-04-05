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
* appDemoFe200Config.h
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
#ifndef __appDemoFe200Configh
#define __appDemoFe200Configh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <appDemo/sandConfig/appDemoCommonConfigDefs.h>

/*
 * FE200 include files
 */
#include "FE200/include/fe200_api_links.h"

/*
 * Input parameter indicating number of planes in FE200
 * device. The actual nof_plane is 2^j where j is
 * between 0-6.
 */
typedef enum
{
  APPDEMO_FE200_INIT_1_PLANE = 0,
  APPDEMO_FE200_INIT_2_PLANES,
  APPDEMO_FE200_INIT_4_PLANES,
  APPDEMO_FE200_INIT_8_PLANES,
  APPDEMO_FE200_INIT_16_PLANES,
  APPDEMO_FE200_INIT_32_PLANES,
  APPDEMO_FE200_MAX_NUM_PLANES = APPDEMO_FE200_INIT_32_PLANES
} APPDEMO_FE200_INIT_NOF_PLANES ;

/*
 **********************************************************
 * structure defining the initialization values
 * the FE200 device should start with.
 * This structutre is passed to the FMC_init method,
 * as a part of a table of all devices on board,
 * or directly to fmc_fe200_init method when registering
 * a new fe200 device to an FMC that is already activated.
 **********************************************************
 */
typedef struct
{
     /*
      * User supplied device base address in CPU memory space
      */
   unsigned long            *base_address ;
     /*
      * Device's chip_id - Should be unique in the system.
      * Used as input (i.e. applied to the chip) only if
      * the flag 'change_var' is non-zero.
      */
   UNIT_VAR_N_CHANGE_FLAG   chip_id ;
     /*
      * Flag. Non-zero value indicates this fe200 device
      * is a part of a multi-stage network.
      * Applied to the chip only if the flag change_var is non-zero
      */
   UNIT_VAR_N_CHANGE_FLAG   is_multi_stage ;
     /*
      * Flag. Non-zero indicates this is an FE2 device,
      * Otherwise, and FE13 device.
      * Applied to the chip only if the flag change_var is non-zero
      */
   UNIT_VAR_N_CHANGE_FLAG   fe2_not_fe13 ;
     /*
      * Number of planes on this FE200 device, must be between
      * j = 0-6, then the actual number of planes will be 2^j.
      * Applied to the chip only if the flag change_var is non-zero
      */
   UNIT_VAR_N_CHANGE_FLAG   nof_planes ;
     /*
      * Flag. Non-zero indicates this FE200 device acts as
      * repeater. If true, 'is_multi_stage', 'fe13_not_fe2'
      * & 'nof_planes' are meaningless.
      * Applied to the chip only if the flag change_var is non-zero
      */
   UNIT_VAR_N_CHANGE_FLAG   is_repeater ;
     /*
      * System profile to apply at startup. This is a general name
      * to a collection of parameters which may be selected
      * by the user.
      * See fe200_set_profile() and FE200_STANDARD_PROFILE.
      * Applied to the chip only if the flag change_var is non-zero
      * Note that the 'profile' is executed BEFORE most of
      * the parameters above so, in case of overlap, they could
      * override some of the 'profile'.
      */
   UNIT_VAR_N_CHANGE_FLAG   initial_profile_id ;
     /*
      * pointer to a structure holding FE200_NUM_OF_SERDESES
      * serdes configuration params.
      * Applied to the chip only if the flag change_serdes_status is non-zero
      */
   FE200_ALL_SERDES_STATUS  *serdes_status_ptr ;
   unsigned int             change_serdes_status ;

   /*
    * REV-B only.
    * The fabric link flow-control enable/disable setting.
    * The RX/TX should be enabled, only if the other side TX/RX is enabled.
    */
   FE200_FABRIC_LINK_FC     fabric_link_fc_setting;

   /*
    * REV-B only.
    * MCI 4 priorities Enable/Disable.
    * Should be enabled, only if it is enable trough-out all of the system!
    */
   unsigned int             mci_4_prio_enable ;
  /*
   * REV-B only.
   * Is there any device (fe or fap) from revB.
   */
   unsigned int             is_rev_a_device_in_system;


} APPDEMO_FE200_INIT_PARAMS ;


/*
 * Structure containing initial device parameters as passed to
 * FMC_N via fmc_n_fmc_init().
 */
typedef struct
{
    /*
     * Can only be FE2_DEVICE / FE13_DEVICE / FAP_DEVICE
     */
  SAND_FMF_DEVICE_TYPE device_type ;
    /*
     * Can only be FE200_FE_TYPE / FAP10M_FAP_TYPE / FAP20V_FAP_TYPE
     */
  union
  {
    SAND_FMF_FAP_TYPE sand_fmf_fap_type ;
    SAND_FMF_FE_TYPE  sand_fmf_fe_type ;
  } device_sub_type ;
    /*
     * This is an OUTPUT value.
     * fmc_n_fmc_init() loads this location with the
     * internal identifier of this device (issued by SAND).
     * FMC user may use this identifier for interfacing
     * driver's interrupt procedure.
     * See, for example, fe200_int_handler() or
     * fmc_n_get_dev_handle_from_base_addr().
     */
  unsigned int     device_handle ;
     /* This is an OUTPUT value. Regardless of
      * whether chip_id has been loaded loaded by
      * fmc_n_fmc_init() or just left as is,
      * its final value is read and then loaded
      * into this variable.
      */
   unsigned int    chip_id ;
  union
  {
      APPDEMO_FE200_INIT_PARAMS  fmc_n_fe200_init_params ;
  } init_params ;
} APPDEMO_SAND_DEVICE_INIT_PARAMS ;

/*
 * Structure containing all initial and system-wide working
 * parameters of FMC_N (device parameters and SAND
 * parameters). These parameters are initially passes to
 * FMC_N via fmc_n_fmc_init() and later updates via the various
 * services.
 */
typedef struct
{
    /*
     * Number of devices registered with FMC_N as being on this
     * unit. This is also the number of meaningful elements on the
     * fmc_n_sand_device_init_params[] array.
     */
  unsigned int                  nof_sand_devices_on_board ;
    /*
     * Pointer to a structure containing SAND initial parameters.
     */
  APPDEMO_SAND_OPEN_INIT_PARAMS   *fmc_n_sand_open_init_params ;
    /*
     * Pointer to an array of structures containing initial
     * parameters related to each device registered via FMC.
     * Number of meaningful elements in this array is
     * 'nof_sand_devices_on_board'. Total number of elements
     * is fmc_n_sand_open_init_params->max_num_devices.
     */
  APPDEMO_SAND_DEVICE_INIT_PARAMS *fmc_n_sand_device_init_params ;
} APPDEMO_ANCHOR ;

/*
 *  This procedure initializes FE200 device under FMC.
 */
int
  appDemoFe200InitDevice(
    SAND_IN  APPDEMO_FE200_INIT_PARAMS*   fmc_n_fe200_init_params,
    SAND_IN  SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SAND_IN  SAND_FMF_DEVICE_TYPE  device_type,
    SAND_IN  SAND_PHYSICAL_ACCESS* physical_access,
    SAND_OUT unsigned int*         device_handle,
    SAND_OUT unsigned long*        chip_id
  );

unsigned long
  appDemoFe200SpatialMulticastApp(
    SAND_IN unsigned int        device_id,
    SAND_IN unsigned int        silent
  );

/*******************************************************************************
* appDemoDuneGetSerdesParameters
*
* DESCRIPTION:
*       Gets parameters of serdes lane initial configuration.
*
* INPUTS:
*     chassysType    -   DUNE_CHASSY_NEGEV or DUNE_CHASSY_GOBY ...
*     fapBoardType   -   DUNE_BOARD_BRD0012A or another FAP card
*     feBoardType    -   DUNE_BOARD_BRD0002A (others not supported)
*     serdesOnFap    -   GT_FALSE - port is on FE card, otherwise on FAP
*     slotNum        -   number of slot (from 0) in FAP or in FE slots
*     devRelNum      -   relatine device number on the card
*     portNum        -   port number on the device
*
* OUTPUTS:
*       serdesStatusPtr  - see SINGLE_SERDES_STATUS description.
*
* RETURNS:
*       GT_STATUS value
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDuneGetSerdesParameters
(
    IN  GT_U32               chassysType,
    IN  GT_U32               fapBoardType,
    IN  GT_U32               feBoardType,
    IN  GT_BOOL              serdesOnFap,
    IN  GT_U32               slotNum,
    IN  GT_U32               devRelNum,
    IN  GT_U32               portNum,
    OUT SINGLE_SERDES_STATUS *serdesStatusPtr
);

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
  );

/*
 * Clears various counters/indicators of the FE.
 */
unsigned long
  appDemoFe200ClearVariousCounters(
    SAND_IN unsigned int device_id
  );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoFe200Configh */

