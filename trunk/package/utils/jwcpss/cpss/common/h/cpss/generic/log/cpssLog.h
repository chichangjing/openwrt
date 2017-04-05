/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssLog.h
*
* DESCRIPTION:
*       Includes definitions for CPSS log functions.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __cpssLogh
#define __cpssLogh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/extServices/os/gtOs/gtGenTypes.h>



/*
 * Typedef: enum CPSS_LOG_POINTER_FORMAT_ENT
 *
 * Description:
 *      This enum determines how pointer values will look in the log.
 *
 * Enumerations:
 *     CPSS_LOG_POINTER_FORMAT_SIMPLE_E       - usual hex value: 0xfff
 *     CPSS_LOG_POINTER_FORMAT_PREFIX_E       - hex with prefix: (addr)0xfff
 *
 *  Comments:
 *      None.
 *
 */
typedef enum {
    CPSS_LOG_POINTER_FORMAT_SIMPLE_E,
    CPSS_LOG_POINTER_FORMAT_PREFIX_E
}CPSS_LOG_POINTER_FORMAT_ENT;

/*
 * Typedef: enum CPSS_LOG_API_FORMAT_ENT
 *
 * Description:
 *      This enum defines the different formats of the log for documenting the
 *      parameters values of a function.
 *
 * Enumerations:
 *     CPSS_LOG_API_FORMAT_NO_PARAMS_E          - not documenting any paramter
 *     CPSS_LOG_API_FORMAT_ALL_PARAMS_E         - documenting all the parameters
 *                                                  values
 *     CPSS_LOG_API_FORMAT_NON_ZERO_PARAMS_E    - documenting only the non zero
 *                                                  parameters values
 *
 *  Comments:
 *      None.
 *
 */
typedef enum {
    CPSS_LOG_API_FORMAT_NO_PARAMS_E,
    CPSS_LOG_API_FORMAT_ALL_PARAMS_E,
    CPSS_LOG_API_FORMAT_NON_ZERO_PARAMS_E
}CPSS_LOG_API_FORMAT_ENT;

/*
 * Typedef: enum CPSS_LOG_LIB_ENT
 *
 * Description:
 *      This enum defines the different libraries in cpss.
 *      used for enable/disable the log activity for each lib.
 *
 * Enumerations:
 *      CPSS_LOG_LIB_APP_DRIVER_CALL_E      - driver call
 *      CPSS_LOG_LIB_BRIDGE_E               - bridge
 *      CPSS_LOG_LIB_CNC_E                  - cnc
 *      CPSS_LOG_LIB_CONFIG_E               - confing
 *      CPSS_LOG_LIB_COS_E                  - cos
 *      CPSS_LOG_LIB_HW_INIT_E              - hw init
 *      CPSS_LOG_LIB_CSCD_E                 - cscd
 *      CPSS_LOG_LIB_CUT_THROUGH_E          - cut through
 *      CPSS_LOG_LIB_DIAG_E                 - diag
 *      CPSS_LOG_LIB_FABRIC_E               - fabric
 *      CPSS_LOG_LIB_IP_E                   - ip
 *      CPSS_LOG_LIB_IPFIX_E                - ipfix
 *      CPSS_LOG_LIB_IP_LPM_E               - ip lpm
 *      CPSS_LOG_LIB_L2_MLL_E               - l2 mll
 *      CPSS_LOG_LIB_LOGICAL_TARGET_E       - logical target
 *      CPSS_LOG_LIB_LPM_E                  - lpm
 *      CPSS_LOG_LIB_MIRROR_E               - mirror
 *      CPSS_LOG_LIB_MULTI_PORT_GROUP_E     - multy port group
 *      CPSS_LOG_LIB_NETWORK_IF_E           - network if
 *      CPSS_LOG_LIB_NST_E                  - nst
 *      CPSS_LOG_LIB_OAM_E                  - oam
 *      CPSS_LOG_LIB_PCL_E                  - pcl
 *      CPSS_LOG_LIB_PHY_E                  - phy
 *      CPSS_LOG_LIB_POLICER_E              - policer
 *      CPSS_LOG_LIB_PORT_E                 - port
 *      CPSS_LOG_LIB_PROTECTION_E           - protection
 *      CPSS_LOG_LIB_PTP_E                  - ptp
 *      CPSS_LOG_LIB_SYSTEM_RECOVERY_E      - system recovery
 *      CPSS_LOG_LIB_TCAM_E                 - tcam
 *      CPSS_LOG_LIB_TM_GLUE_E              - tm glue
 *      CPSS_LOG_LIB_TRUNK_E                - trunk
 *      CPSS_LOG_LIB_TTI_E                  - tti
 *      CPSS_LOG_LIB_TUNNEL_E               - tunnel
 *      CPSS_LOG_LIB_VNT_E                  - vnt
 *      CPSS_LOG_LIB_RESOURCE_MANAGER_E     - resource manager
 *      CPSS_LOG_LIB_VERSION_E              - version
 *      CPSS_LOG_LIB_TM_E                   - traffic manager
 *      CPSS_LOG_LIB_SMI_E                  - SMI
 *      CPSS_LOG_LIB_INIT_E                 - init
 *      CPSS_LOG_LIB_DRAGONITE_E            - dragonite
 *      CPSS_LOG_LIB_VIRTUAL_TCAM_E         - virtual TCAM
 *
 *      CPSS_LOG_LIB_ALL_E                  - all the libs
 *  Comments:
 *      None.
 *
 */
typedef enum {
    CPSS_LOG_LIB_APP_DRIVER_CALL_E,
    CPSS_LOG_LIB_BRIDGE_E,
    CPSS_LOG_LIB_CNC_E,
    CPSS_LOG_LIB_CONFIG_E,
    CPSS_LOG_LIB_COS_E,
    CPSS_LOG_LIB_HW_INIT_E,
    CPSS_LOG_LIB_CSCD_E,
    CPSS_LOG_LIB_CUT_THROUGH_E,
    CPSS_LOG_LIB_DIAG_E,
    CPSS_LOG_LIB_FABRIC_E,
    CPSS_LOG_LIB_IP_E,
    CPSS_LOG_LIB_IPFIX_E,
    CPSS_LOG_LIB_IP_LPM_E,
    CPSS_LOG_LIB_L2_MLL_E,
    CPSS_LOG_LIB_LOGICAL_TARGET_E,
    CPSS_LOG_LIB_LPM_E,
    CPSS_LOG_LIB_MIRROR_E,
    CPSS_LOG_LIB_MULTI_PORT_GROUP_E,
    CPSS_LOG_LIB_NETWORK_IF_E,
    CPSS_LOG_LIB_NST_E,
    CPSS_LOG_LIB_OAM_E,
    CPSS_LOG_LIB_PCL_E,
    CPSS_LOG_LIB_PHY_E,
    CPSS_LOG_LIB_POLICER_E,
    CPSS_LOG_LIB_PORT_E,
    CPSS_LOG_LIB_PROTECTION_E,
    CPSS_LOG_LIB_PTP_E,
    CPSS_LOG_LIB_SYSTEM_RECOVERY_E,
    CPSS_LOG_LIB_TCAM_E,
    CPSS_LOG_LIB_TM_GLUE_E,
    CPSS_LOG_LIB_TRUNK_E,
    CPSS_LOG_LIB_TTI_E,
    CPSS_LOG_LIB_TUNNEL_E,
    CPSS_LOG_LIB_VNT_E,
    CPSS_LOG_LIB_RESOURCE_MANAGER_E,
    CPSS_LOG_LIB_VERSION_E,
    CPSS_LOG_LIB_TM_E,
    CPSS_LOG_LIB_SMI_E,
    CPSS_LOG_LIB_INIT_E,
    CPSS_LOG_LIB_DRAGONITE_E,
    CPSS_LOG_LIB_VIRTUAL_TCAM_E,

    CPSS_LOG_LIB_ALL_E  /* must be last */
}CPSS_LOG_LIB_ENT;

/*
 * Typedef: enum CPSS_LOG_TYPE_ENT
 *
 * Description:
 *      This enum defines the different types of the log.
 *
 * Enumerations:
 *     CPSS_LOG_TYPE_INFO_E                       - information log
 *     CPSS_LOG_TYPE_ENTRY_LEVEL_FUNCTION_E       - log of entry level of APIs only
 *     CPSS_LOG_TYPE_NON_ENTRY_LEVEL_FUNCTION_E   - log of internal functions and not entry level APIs
 *     CPSS_LOG_TYPE_DRIVER_E                     - driver
 *     CPSS_LOG_TYPE_ERROR_E                      - error log
 *     CPSS_LOG_TYPE_ALL_E                        - all types
 *
 *  Comments:
 *      None.
 *
 */
typedef enum {
    CPSS_LOG_TYPE_INFO_E,
    CPSS_LOG_TYPE_ENTRY_LEVEL_FUNCTION_E,
    CPSS_LOG_TYPE_NON_ENTRY_LEVEL_FUNCTION_E,
    CPSS_LOG_TYPE_DRIVER_E,
    CPSS_LOG_TYPE_ERROR_E,
    CPSS_LOG_TYPE_ALL_E /* must be last */
}CPSS_LOG_TYPE_ENT;

/*
 * Typedef: enum CPSS_LOG_TIME_FORMAT_ENT
 *
 * Description:
 *      This enum describes log time format.
 * Enumerations:
 *      CPSS_LOG_TIME_FORMAT_DATE_TIME_E -
 *          Date and time representation (Thu Aug 23 14:55:02 2001)
 *      CPSS_LOG_TIME_FORMAT_SHORT_DATE_TIME_E -
 *          Short MM/DD/YY and 12-hour clock time (08/23/01 02:55:02 pm)
 *      CPSS_LOG_TIME_FORMAT_SHORT_DATE_ISO_TIME_E -
 *          Short MM/DD/YY and 24-hour clock time (08/23/01 14:55:02)
 *      CPSS_LOG_TIME_FORMAT_NO_DATE_TIME_E -
 *          Timestamp is not logged
 *
*/
typedef enum {
    CPSS_LOG_TIME_FORMAT_DATE_TIME_E,
    CPSS_LOG_TIME_FORMAT_SHORT_DATE_TIME_E,
    CPSS_LOG_TIME_FORMAT_SHORT_DATE_ISO_TIME_E,
    CPSS_LOG_TIME_FORMAT_NO_DATE_TIME_E
}CPSS_LOG_TIME_FORMAT_ENT;

/*******************************************************************************
* cpssLogEnableSet
*
* DESCRIPTION:
*       The function enables/disables Log feature.
*
* INPUTS:
*       enable - GT_TRUE for log enable and GT_False for log disable
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID cpssLogEnableSet
(
    IN GT_BOOL enable
);

/*******************************************************************************
* cpssLogEnableGet
*
* DESCRIPTION:
*       The function gets the status of the Log feature (enabled/disabled).
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_TRUE         - if the log is enabled
*       GT_FALSE        - if the log is disabled
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL cpssLogEnableGet(GT_VOID);

/*******************************************************************************
* cpssLogLibEnableSet
*
* DESCRIPTION:
*       The function enables/disables specific type of log information for
*       specific or all libs.
*
* INPUTS:
*       lib - lib to enable/disable log for.
*       type - log type.
*       enable - GT_TRUE for log enable and GT_FALSE for log disable
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PARAM    - bad parameter lib or type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogLibEnableSet
(
    IN  CPSS_LOG_LIB_ENT    lib,
    IN  CPSS_LOG_TYPE_ENT   type,
    IN  GT_BOOL             enable
);

/*******************************************************************************
* cpssLogLibEnableGet
*
* DESCRIPTION:
*       The function gets the status of specific type of log information for
*       specific lib (enabled/disabled).
*
* INPUTS:
*       lib - lib to get the log status for.
*       type - log type.
*
* OUTPUTS:
*       enablePtr - GT_TRUE - log is enabled for the specific lib and log type.
*                   GT_FALSE - log is enabled for the specific lib and log type.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PTR      - bad pointer enablePtr
*       GT_BAD_PARAM    - bad parameter lib or type
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogLibEnableGet
(
    IN   CPSS_LOG_LIB_ENT   lib,
    IN   CPSS_LOG_TYPE_ENT  type,
    OUT  GT_BOOL            * enablePtr
);

/*******************************************************************************
* cpssLogApiFormatSet
*
* DESCRIPTION:
*       The function set format for API related log.
*
* INPUTS:
*       format - format of API related logs.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PARAM    - bad parameter format
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogApiFormatSet
(
    IN CPSS_LOG_API_FORMAT_ENT format
);

/*******************************************************************************
* cpssLogApiFormatGet
*
* DESCRIPTION:
*       The function gets the format of API related log.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       format of API related log
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CPSS_LOG_API_FORMAT_ENT cpssLogApiFormatGet(GT_VOID);

/*******************************************************************************
* cpssLogStackSizeSet
*
* DESCRIPTION:
*       Defines the size of the stack, how many function will be logged before
*       an error occurred.
*
* INPUTS:
*       size - the amount of functions.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK           - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogStackSizeSet
(
    IN GT_U32 size
);

/*******************************************************************************
* cpssLogStackSizeGet
*
* DESCRIPTION:
*       The function gets the size of the log stack
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       the log stack size
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_U32 cpssLogStackSizeGet(GT_VOID);

/*******************************************************************************
* cpssLogTimeFormatSet
*
* DESCRIPTION:
*       The function sets log time format.
*
* INPUTS:
*       format - log time format.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK           - on success
*       GT_BAD_PARAM    - bad parameter format
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogTimeFormatSet
(
    IN CPSS_LOG_TIME_FORMAT_ENT format
);

/*******************************************************************************
* cpssLogHistoryDump
*
* DESCRIPTION:
*       Dump log history database.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - if history log disabled
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogHistoryDump
(
    GT_VOID
);

/*******************************************************************************
* cpssLogHistoryFileNameSet
*
* DESCRIPTION:
*       Set log history file name.
*
* INPUTS:
*       fileNamePtr - pointer to history file name string
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PTR      - on null pointer
*       GT_BAD_PARAM    - string exceeds maximmal size
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssLogHistoryFileNameSet
(
    IN const char * fileNamePtr
);


/*******************************************************************************
* cpssLogLineNumberEnableSet
*
* DESCRIPTION:
*       The function enables/disables using of a source code line number by
*       some logging functions
*
* INPUTS:
*       enable - GT_TRUE - enable, GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID cpssLogLineNumberEnableSet
(
     IN GT_BOOL enable
);

/*******************************************************************************
* cpssLogLineNumberEnableGet
*
* DESCRIPTION:
*       The function gets the status of the using of a sorce code line number
*       by some logging functions
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_TRUE         - if enabled
*       GT_FALSE        - if disabled
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_BOOL cpssLogLineNumberEnableGet
(
    GT_VOID
);

/*******************************************************************************
* cpssLogPointerFormatSet
*
* DESCRIPTION:
*       Set a format of pointer values
*
* INPUTS:
*       value - a format type
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_VOID cpssLogPointerFormatSet
(
     IN CPSS_LOG_POINTER_FORMAT_ENT value
);

/*******************************************************************************
* cpssLogPointerFormatGet
*
* DESCRIPTION:
*       get a format used for a pointer-values logging.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None
*
* RETURNS :
*       a format used for a pointer-values logging.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CPSS_LOG_POINTER_FORMAT_ENT cpssLogPointerFormatGet
(
    GT_VOID
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssLogh */

