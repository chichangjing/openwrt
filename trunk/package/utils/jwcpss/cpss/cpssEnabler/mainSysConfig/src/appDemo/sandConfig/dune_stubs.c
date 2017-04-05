/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* dune_stubs.c
*
* DESCRIPTION:
*       Stubs for dune code
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*
*******************************************************************************/

#include <gtOs/gtGenTypes.h>

/*
 * The various types of crates:
 */
#define CRATE_TEST_EVAL              0  /*Negev-Rev-A*/
#define CRATE_32_PORT_40G_STANDALONE 1  /*Gobi: FAP20V*/
#define CRATE_32_PORT_40G_IO         2
#define CRATE_FABRIC_BOX             3
#define CRATE_64_PORT_10G_STANDALONE 4  /*Gobi: FAP10M*/
#define CRATE_32_PORT_10G_IO         5
#define CRATE_NEGEV_REV_B            6

#ifndef __VXWORKS__
/* { */
#define STATUS int
#define FALSE 0
#define TRUE  1
#if	!defined(ERROR)
  #define ERROR TRUE
#endif
#endif

/* SWEEP stubs */

unsigned long       PEX_FAP_base_addr;   /* base address of FAP on PEX */

int
  fap20m_agent_error_handler(
    unsigned long   err_id,
    char            *err_desc,
    char            **new_buf,
    unsigned long   param_01,
    unsigned long   param_02,
    unsigned long   param_03,
    unsigned long   param_04,
    unsigned long   param_05,
    unsigned long   param_06
  )
{
    return 0;
}

/*
 * Original in usrAppLoader.c
 * include: usrApp.h
 */
unsigned
  char
    get_slot_id(
      void
    )
{
  unsigned char
    ret;

  ret = 1;

  return (ret) ;
}

/*****************************************************
*NAME
*  utils_is_crate_gobi
*TYPE: PROC
*DATE: 16/AUG/2004
*MODIFIED: 16/AUG/2004
*FUNCTION:
*  Check if the crate ID is Gobi CRATE.
*CALLING SEQUENCE:
*  utils_is_crate_gobi()
*INPUT:
*  DIRECT:
*    const unsigned int crate_type -
*      Crate type
*  INDIRECT:
*    Nonr.
*OUTPUT:
*  DIRECT:
*    unsigned int
*      Indicator.
*      - TRUE:  Is Gobi Id.
*      - FALSE: Not Gobi Id.
*  INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
*****************************************************/
unsigned int
  utils_is_crate_gobi(
    const unsigned int crate_type
  )
{
  unsigned int
    is_crate_gobi;

  is_crate_gobi = FALSE;

  switch (crate_type)
  {
    case CRATE_32_PORT_40G_STANDALONE:
    case CRATE_64_PORT_10G_STANDALONE:
       is_crate_gobi = TRUE;
      break ;

    default:
      is_crate_gobi = FALSE ;
      break ;
  }

  return is_crate_gobi;
}

/*****************************************************
*NAME
*  utils_is_crate_negev
*TYPE: PROC
*DATE: 22/AUG/2004
*MODIFIED: 22/AUG/2004
*FUNCTION:
*  Check if the crate ID is Negev CRATE.
*CALLING SEQUENCE:
*  utils_is_crate_negev()
*INPUT:
*  DIRECT:
*    const unsigned int crate_type -
*      Crate type
*  INDIRECT:
*    Nonr.
*OUTPUT:
*  DIRECT:
*    unsigned int
*      Indicator.
*      - TRUE:  Is Negev Id.
*      - FALSE: Not Negev Id.
*  INDIRECT:
*    None.
*REMARKS:
*
*SEE ALSO:
*****************************************************/
unsigned int
  utils_is_crate_negev(
    const unsigned int crate_type
  )
{
  unsigned int
    is_crate_negev;

  is_crate_negev = FALSE;

  switch (crate_type)
  {
    case CRATE_TEST_EVAL:
    case CRATE_NEGEV_REV_B:
      is_crate_negev = TRUE;
      break ;

    default:
      is_crate_negev = FALSE ;
      break ;
  }

  return is_crate_negev;
}

typedef enum
{
  D_USR_APP_FLAVOR_NONE               = 0,
  D_USR_APP_FLAVOR_NORMAL             = 1,
  D_USR_APP_FLAVOR_MINIMAL            = 2,
  D_USR_APP_FLAVOR_WIN_01             = 3,
  D_USR_APP_FLAVOR_SERVER             = 4,
  D_USR_APP_FLAVOR_NORMAL_AND_SERVER  = 5,
  D_USR_APP_FLAVOR_SERVER_WITH_INIT   = 6,
  D_USR_APP_FLAVOR_MINIMAL_NO_APP     = 7,
  D_USR_APP_FLAVOR_NOF_ENUMS

} D_USR_APP_FLAVOR;

void
  d_usr_app_flavor_get(
     D_USR_APP_FLAVOR* usr_app_flavor
  )
{
  *usr_app_flavor = D_USR_APP_FLAVOR_WIN_01;
}

unsigned long
  read_timer_3_4(
    void
  )
{
  static int timer_3_4 = 1;
  return ++timer_3_4;
}

int
  gen_err(
        int compare,
        int reverse_logic,
        int status,
        int reference,
    const char *msg,
    const char *proc_name,
        int severity,
        int err_id,
        unsigned int store_in_nv,
        unsigned int alarm_flags,
        short min_time_between_events,
        unsigned int send_trap
  )
{
  return 0;
}

int
  gen_err_msg(
    const char *msg,
    const char *proc_name,
          unsigned int store_in_nv,
          unsigned int alarm_flags,
          short min_time_between_events,
          unsigned int send_trap
  )
{
  int
    answer;

  answer = gen_err(
             FALSE, FALSE,
             0, 0,
             msg,proc_name,
             0, 0, store_in_nv, alarm_flags,
             min_time_between_events, send_trap
           );

  return answer;
}

int download_cli_file(char * a, char *b, char *c )
{
    return 0;
}

unsigned long
  timer_3_4_to_micro(unsigned long time)
{
  return 0;
}


/*
 * Original in usrAppLoader.c
 * include: usrApp.h
 */
unsigned
  char
    get_crate_type(
      void
    )
{
  unsigned
    char
      ret = CRATE_NEGEV_REV_B;
  return (ret) ;
}

unsigned
  long 
    fap20m_get_LBP_UNT_entry_from_mirror(
      unsigned int device_id, 
      unsigned int destination_id, 
      unsigned int* fap20m_lbp_unt_mirror_base_q, 
      unsigned int* fap20m_lbp_unt_mirror_valid_bit
    )
{
    return 0;
}

