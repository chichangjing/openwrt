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
* appDemoCommonConfigDefs.h
*
* DESCRIPTION:
*       Initialization common definitions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __appDemoCommonConfigDefsh
#define __appDemoCommonConfigDefsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*
 * SAND include files
 */
#include "SAND/Management/include/sand_low_level.h"
#include "SAND/SAND_FM/include/sand_chip_defines.h"

/* supported chassies */
#define DUNE_CHASSY_NEGEV 0
#define DUNE_CHASSY_GOBY  1

/* supported boards*/
#define DUNE_BOARD_BRD0002A 0
#define DUNE_BOARD_BRD0012A 1
#define DUNE_BOARD_EX136DX270BW8XG


#define APPDEMO_INSERT_DEBUG_VARS      \
    char *proc_name = "unknown_proc";  \
    unsigned long error_id = 0;        \
    unsigned long err = 0;             \
    SAND_RET sand_err = SAND_OK;

#define APPDEMO_INSERT_DEBUG_PRELUDE(name) \
  proc_name = name;

/*
 * Macro to handle procedure call which returns standard sand
 * error code and, in case of error, sets error code and quits.
 * Assumes local variables: err, error_id, sand_err
 * Assumes label: exit
 */
#define APPDEMO_PROC_AND_ERR(function_call,local_err_code,gen_command) \
  error_id = function_call ; \
  sand_err = sand_get_error_code_from_error_word(error_id) ; \
  if (SAND_OK != sand_err) \
  { \
    /* \
     * Error detected. Quit with error. \
     */ \
    err = local_err_code ; \
    gen_command ; \
    goto exit ; \
  }

#define APPDEMO_INSERT_DEBUG_EPILOGUE \
  {   \
    char   \
      *operation_fail_text ;   \
    if ((err) || (sand_err != SAND_OK))   \
    {   \
      operation_fail_text = "FAILED" ;   \
    }   \
    else   \
    {   \
      operation_fail_text = "SUCCEEDED" ;   \
    }   \
    sand_os_printf( \
        "\r\n---->\r\n"   \
        "%s: Exit with the following parameters:\r\n"   \
        "Operation has     : %s\r\n",   \
        proc_name,   \
        operation_fail_text   \
          ) ;   \
  }


/*
 *************************************************************
 * This structure contains ALL parameters required by
 * DuneDriver to operate, before registering any device with it.
 * User of the driver must pass this structure (throug FMC_init)
 * that actually opens the driver and registers the devices.
 **************************************************************
 */
typedef struct
{
    /*
     * Maximum number of devices that can be managed by this
     * specific instance of DuneDriver - Implies on the size
     * of tables the driver will allocate.
     */
  unsigned int      max_num_devices ;
    /*
     * Some of the driver's time services are in ticks, and
     * some are in actual time units (nano-sec, micro-sec,
     * mili-sec, sec, etc). This param helps the driver to
     * make the conversion.
     * For instance, if the CPU ticks 100 times a second,
     * this param would be: 10 (= 1000 ms / 100 ticks)
     */
  unsigned int      system_tick_in_ms ;
    /*
     * TCM is the only Task spawned by the driver. User
     * control its system priority using this param
     */
  unsigned int      tcm_task_priority ;
    /*
     * TCM (also) handles unsolicited events (such as
     * interrupts and error events). Therefore it can
     * end up consuming all CPU time available in the
     * system (for instance in case of interrupt flooding).
     * this param control this behaviour by forcing the
     * TCM to release system resources for that amount
     * of system ticks.
     */
  unsigned int      min_time_between_tcm_activation ;
    /*
     * In case device interrupt line is not connected to the CPU,
     * TCM can mockup interrupt handling by poling the ISR.
     * This result in pending interrupts being handled every TCM tick.
     * which mocks up interrupt handling.
     */
  unsigned int      tcm_mockup_interrupts ;
    /*
     * User supplied error handler. When a remote manager
     * takes control of the whole network, errors may be
     * reported both to this routine, and to the manager
     * as well.
     * This "fork" feature is internal to the FMC, so this
     * routine gets actually registered in the FMC and not
     * in the SAND module internal structures.
     * This pointer may be NULL. In that case, no local
     * error_handler will be called and 'error_description'
     * is not meaningful.
     */
  SAND_ERROR_HANDLER_PTR error_handler ;
    /*
     * Pointer to buffer allocated by the user.
     * This buffer will be loaded with the specific error
     * information before calling the 'error_handler' above.
     * May be NULL in case no error handler is used (i.e.,
     * 'error_handler' is NULL). Note that when closing
     * FMC, it is the user's responsibility to free
     * this memory block.
     */
  char                  *error_description ;
    /*
     * User supplied procedure (Part of board BSP). When
     * driver wishes to reset the device, it invokes this
     * procedure.
     */
  SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr ;
    /*
     * Pointers to a user-supplied BSP function:
     * read_callback: Carries out all physical read accesses
     * for SAND devices on board.
     * write_callback: Carries out all physical write accesses
     * for SAND devices on board.
     * Both procedures get, as input, base address, no. of
     * bytes to refer to (read/write) and offset to the
     * device's memory space. 'write_callback' also gets
     * data to write.
     * Any of the ointers may be NULL. In that case, the
     * driver default method is used (That is, access the
     * device directly using 32bits address) .
     * Access methods can be changed later using the
     * SAND API routine 'sand_set_physical_access_hook()'.
     * Two sets of access functions are supplied by the user:
     * The first set (index 0) is used until step 1 of the
     * initialization stage is over. At that point, the
     * second set is applied.
     * Implementation notes:
     *   Currently, only FAP10M uses both sets (e.g., I2C
     *   access until first stage is over, followed by PCI
     *   access henceforth.
     *   For FE200, it is recommended to leave both pointers
     *   as NULL; this will result in ECI access throughout.
     */
  SAND_PHYSICAL_READ_ACCESS_PTR    mem_read_callback[2] ;
  SAND_PHYSICAL_WRITE_ACCESS_PTR   mem_write_callback[2] ;
} APPDEMO_SAND_OPEN_INIT_PARAMS ;

/*
 * General purpose structure for an unsigned
 * int variable plus a flag indicating whether to
 * actually use it to load into indicated device.
 * See, e.g., 'APPDEMO_FE200_INIT_PARAMS' below.
 */
typedef struct
{
  unsigned int  var ;
  unsigned char change_var ;
} UNIT_VAR_N_CHANGE_FLAG ;


/*******************************************************************************
* appDemoSandSetDefaultOsFuncs
*
* DESCRIPTION:
*       Sets default sand bindings to OS
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoSandSetDefaultOsFuncs
(
    void
);

/*******************************************************************************
* appDemoSandInit
*
* DESCRIPTION:
*       Opens and initializes SAND parameters.
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
GT_STATUS appDemoSandInit
(
    void
);

int send_string_to_screen(
    char *out_string,
    int  add_cr_lf
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __appDemoCommonConfigDefsh */

