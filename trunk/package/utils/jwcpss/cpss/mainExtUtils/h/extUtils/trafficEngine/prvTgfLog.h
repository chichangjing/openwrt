/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfLog.h
*
* DESCRIPTION:
*       Internal header which defines API for TGF logger
*       It is used only in the tgfEngine
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef __prvTgfLogh
#define __prvTgfLogh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */

#define TGF_ARGVP_ARR_1_MAC(argvp,arg1)   \
            GT_UINTPTR argvp[1];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);

#define TGF_ARGVP_ARR_2_MAC(argvp,arg1,arg2)   \
            GT_UINTPTR argvp[2];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);

#define TGF_ARGVP_ARR_3_MAC(argvp,arg1,arg2,arg3)   \
            GT_UINTPTR argvp[3];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);                                      \
            argvp[2] = (GT_UINTPTR)(arg3);

#define TGF_ARGVP_ARR_4_MAC(argvp,arg1,arg2,arg3,arg4)   \
            GT_UINTPTR argvp[4];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);                                      \
            argvp[2] = (GT_UINTPTR)(arg3);                                      \
            argvp[3] = (GT_UINTPTR)(arg4);

#define TGF_ARGVP_ARR_5_MAC(argvp,arg1,arg2,arg3,arg4,arg5)   \
            GT_UINTPTR argvp[5];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);                                      \
            argvp[2] = (GT_UINTPTR)(arg3);                                      \
            argvp[3] = (GT_UINTPTR)(arg4);                                      \
            argvp[4] = (GT_UINTPTR)(arg5);

#define TGF_ARGVP_ARR_6_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6)   \
            GT_UINTPTR argvp[6];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);                                      \
            argvp[2] = (GT_UINTPTR)(arg3);                                      \
            argvp[3] = (GT_UINTPTR)(arg4);                                      \
            argvp[4] = (GT_UINTPTR)(arg5);                                      \
            argvp[5] = (GT_UINTPTR)(arg6);

#define TGF_ARGVP_ARR_7_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7)   \
            GT_UINTPTR argvp[7];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);                                      \
            argvp[2] = (GT_UINTPTR)(arg3);                                      \
            argvp[3] = (GT_UINTPTR)(arg4);                                      \
            argvp[4] = (GT_UINTPTR)(arg5);                                      \
            argvp[5] = (GT_UINTPTR)(arg6);                                      \
            argvp[6] = (GT_UINTPTR)(arg7);

#define TGF_ARGVP_ARR_8_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8)   \
            GT_UINTPTR argvp[8];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);                                      \
            argvp[2] = (GT_UINTPTR)(arg3);                                      \
            argvp[3] = (GT_UINTPTR)(arg4);                                      \
            argvp[4] = (GT_UINTPTR)(arg5);                                      \
            argvp[5] = (GT_UINTPTR)(arg6);                                      \
            argvp[6] = (GT_UINTPTR)(arg7);                                      \
            argvp[7] = (GT_UINTPTR)(arg8);

#define TGF_ARGVP_ARR_9_MAC(argvp,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9)\
            GT_UINTPTR argvp[9];                                                \
            argvp[0] = (GT_UINTPTR)(arg1);                                      \
            argvp[1] = (GT_UINTPTR)(arg2);                                      \
            argvp[2] = (GT_UINTPTR)(arg3);                                      \
            argvp[3] = (GT_UINTPTR)(arg4);                                      \
            argvp[4] = (GT_UINTPTR)(arg5);                                      \
            argvp[5] = (GT_UINTPTR)(arg6);                                      \
            argvp[6] = (GT_UINTPTR)(arg7);                                      \
            argvp[7] = (GT_UINTPTR)(arg8);                                      \
            argvp[8] = (GT_UINTPTR)(arg9);
/*
 * Description: Defines routines to be done to log message.
 *      It is used when there is no any argument in output string.
 *
 * Parameters:
 *      x - format string of log message
 */
#define PRV_TGF_LOG0_MAC(x)                                                    \
    do {                                                                       \
        prvTgfLogMessage(x, NULL, 0);                                          \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there is only one argument in output string.
 *
 * Parameters:
 *      x       -   format string of log message
 *      arg1    -   argument for output string
 */
#define PRV_TGF_LOG1_MAC(x, arg1)                                              \
    do {                                                                       \
        TGF_ARGVP_ARR_1_MAC(argvl,arg1);                        \
        prvTgfLogMessage(x, argvl, 1);                                         \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are two arguments in output string.
 *
 * Parameters:
 *      x           -   format string of log message
 *      arg1, arg2  -   arguments for output string
 */
#define PRV_TGF_LOG2_MAC(x, arg1, arg2)                                        \
    do {                                                                       \
        TGF_ARGVP_ARR_2_MAC(argvl,arg1,arg2);                        \
        prvTgfLogMessage(x, argvl, 2);                                         \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are three arguments in output string.
 *
 * Parameters:
 *      x                   -   format string of log message
 *      arg1, arg2, arg3    -   arguments for output string
 */
#define PRV_TGF_LOG3_MAC(x, arg1, arg2, arg3)                                  \
    do {                                                                       \
        TGF_ARGVP_ARR_3_MAC(argvl,arg1,arg2,arg3);                        \
        prvTgfLogMessage(x, argvl, 3);                                         \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are four arguments in output string.
 *
 * Parameters:
 *      x                           -   format string of log message
 *      arg1, arg2, arg3, arg4      -   arguments for output string
 */
#define PRV_TGF_LOG4_MAC(x, arg1, arg2, arg3, arg4)                            \
    do {                                                                       \
        TGF_ARGVP_ARR_4_MAC(argvl,arg1,arg2,arg3,arg4);                        \
        prvTgfLogMessage(x, argvl, 4);                                         \
    } while (0)


/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are five arguments in output string.
 *
 * Parameters:
 *      x                               -   format string of log message
 *      arg1, arg2, arg3, arg4, arg5    -   arguments for output string
 */
#define PRV_TGF_LOG5_MAC(x, arg1, arg2, arg3, arg4, arg5)                      \
    do {                                                                       \
        TGF_ARGVP_ARR_5_MAC(argvl,arg1,arg2,arg3,arg4,arg5);                        \
        prvTgfLogMessage(x, argvl, 5);                                         \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are six arguments in output string.
 *
 * Parameters:
 *      x                                   -   format string of log message
 *      arg1, arg2, arg3, arg4, arg5, arg6  -   arguments for output string
 */
#define PRV_TGF_LOG6_MAC(x, arg1, arg2, arg3, arg4, arg5, arg6)                \
    do {                                                                       \
        TGF_ARGVP_ARR_6_MAC(argvl,arg1,arg2,arg3,arg4,arg5,arg6);              \
        prvTgfLogMessage(x, argvl, 6);                                         \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are seven arguments in output string.
 *
 * Parameters:
 *      x                                   -   format string of log message
 *      arg1, arg2, arg3, arg4, arg5, arg6, arg7  -   arguments for output string
 */
#define PRV_TGF_LOG7_MAC(x, arg1, arg2, arg3, arg4, arg5, arg6, arg7)          \
    do {                                                                       \
        TGF_ARGVP_ARR_7_MAC(argvl,arg1,arg2,arg3,arg4,arg5,arg6,arg7);         \
        prvTgfLogMessage(x, argvl, 7);                                         \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are seven arguments in output string.
 *
 * Parameters:
 *      x                                       - format string of log message
 *      arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8 - arguments for output string
 */
#define PRV_TGF_LOG8_MAC(x, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)    \
    do {                                                                       \
        TGF_ARGVP_ARR_8_MAC(argvl,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8);    \
        prvTgfLogMessage(x, argvl, 8);                                         \
    } while (0)

/*
 * Description: Defines routines to be done to log message.
 *      It is used when there are seven arguments in output string.
 *
 * Parameters:
 *      x                                            - format string of log message
 *      arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9 - arguments for output string
 */
#define PRV_TGF_LOG9_MAC(x,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9)       \
    do {                                                                       \
        TGF_ARGVP_ARR_9_MAC(argvl,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9);\
        prvTgfLogMessage(x, argvl, 9);                                         \
    } while (0)


/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there is no any argument in output string.
 *
 * Parameters:
 *      e -    expexted value
 *      r -    received value
 *      s -    format string for output
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define TGF_VERIFY_EQUAL0_STRING_MAC(e, r, s)                                           \
    do {                                                                                \
        GT_BOOL err = tgfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            err = tgfFailureMsgLog(s, NULL, 0);                                         \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there is only one argument in output string.
 *
 * Parameters:
 *      e -    expexted value
 *      r -    received value
 *      s -    format string for output
 *      arg1 - argument for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define TGF_VERIFY_EQUAL1_STRING_MAC(e, r, s, arg1)                                     \
    do {                                                                                \
        GT_BOOL err = tgfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            TGF_ARGVP_ARR_1_MAC(argvp,arg1);                        \
            err = tgfFailureMsgLog(s, argvp, 1);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are two arguments in output string.
 *
 * Parameters:
 *      e -           expexted value
 *      r -           received value
 *      s -           format string for output
 *      arg1, arg2 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define TGF_VERIFY_EQUAL2_STRING_MAC(e, r, s, arg1, arg2)                               \
    do {                                                                                \
        GT_BOOL err = tgfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            TGF_ARGVP_ARR_2_MAC(argvp,arg1,arg2);                        \
            err = tgfFailureMsgLog(s, argvp, 2);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are three arguments in output string.
 *
 * Parameters:
 *      e -                 expexted value
 *      r -                 received value
 *      s -                 format string for output
 *      arg1, arg2, arg3 -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define TGF_VERIFY_EQUAL3_STRING_MAC(e, r, s, arg1, arg2, arg3)                         \
    do {                                                                                \
        GT_BOOL err = tgfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            TGF_ARGVP_ARR_3_MAC(argvp,arg1,arg2,arg3);                        \
            err = tgfFailureMsgLog(s, argvp, 3);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

/*
 * Description: This macro checks if received value is equal to expected value.
 *      It is used when there are four arguments in output string.
 *
 * Parameters:
 *      e                       -  expexted value
 *      r                       -  received value
 *      s                       -  format string for output
 *      arg1, arg2, arg3, arg4  -  arguments for output string
 *
 * Comments: Test cases must use this macro for test flow validation.
 */
#define TGF_VERIFY_EQUAL4_STRING_MAC(e, r, s, arg1, arg2, arg3, arg4)                   \
    do {                                                                                \
        GT_BOOL err = tgfEqualVerify(e, r, __LINE__, __FILE__);                         \
        if (GT_FALSE == err) {                                                          \
            TGF_ARGVP_ARR_4_MAC(argvp,arg1,arg2,arg3,arg4);                        \
            err = tgfFailureMsgLog(s, argvp, 4);                                        \
            if (GT_FALSE == err) return;                                                \
        }                                                                               \
    } while(0)

#define PRV_TGF_VERIFY_GT_OK(rc, msg)                                        \
    if (GT_OK != rc)                                                         \
    {                                                                        \
        PRV_TGF_LOG2_MAC("\n[TGF]: %s FAILED, rc = [%d]\n", msg, rc);        \
        return rc;                                                           \
    }

/*******************************************************************************
* prvTgfLogMessage
*
* DESCRIPTION:
*       This routine writes message to the output interface.
*       This function handles all issues regarding the logging to specific
*       interfaces.
*
* INPUTS:
*       formatStringPtr -   (pointer to) format string.
*       argvPtr         -   (pointer to) vector of output string arguments.
*       argc            -   number of arguments in output string.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       -   logging has been done successfully.
*       GT_FAIL     -   General failure error. Should never happen.
*
* COMMENTS:
*       It uses OS abstraction layer to output.
*******************************************************************************/
GT_VOID prvTgfLogMessage
(
    IN const GT_CHAR    *formatStringPtr,
    IN const GT_UINTPTR argvPtr[],
    IN GT_U32           argc
);

/*TODO*/
GT_BOOL tgfEqualVerify
(
    IN GT_UINTPTR   e,
    IN GT_UINTPTR   r,
    IN GT_32        lineNum,
    IN GT_CHAR      *fileNamePtr
);

/*TODO*/
GT_BOOL tgfFailureMsgLog
(
    IN const GT_CHAR    *failureMsgPtr,
    IN const GT_UINTPTR argvPtr[],
    IN GT_U32           argc
);

/*TODO*/
GT_VOID tgfPrintKeepAlive
(
    GT_VOID
);

GT_U32 tgfLogPrintModeFinal
(
    GT_VOID
);
GT_U32 tgfLogPrintModeRestore
(
    IN GT_U32       savedmode
);



/* LOGS */
typedef GT_VOID (*TGF_LOG_MESSAGE_FUNC)
(
    IN const GT_CHAR    *formatStringPtr,
    IN const GT_UINTPTR argvPtr[],
    IN GT_U32           argc
);
typedef GT_BOOL (*TGF_EQUAL_VERIFY_FUNC)
(
    IN GT_UINTPTR   e,
    IN GT_UINTPTR   r,
    IN GT_32        lineNum,
    IN GT_CHAR      *fileNamePtr
);
typedef GT_BOOL (*TGF_FAILURE_MSG_LOG_FUNC)
(
    IN const GT_CHAR    *failureMsgPtr,
    IN const GT_UINTPTR argvPtr[],
    IN GT_U32           argc
);
typedef GT_VOID (*TGF_PRINT_KEEPALIVE_FUNC)
(
    GT_VOID
);
typedef GT_U32  (*TGF_LOG_PRINT_MODE_FINAL_FUNC)
(
    GT_VOID
);
typedef GT_U32  (*TGF_LOG_PRINT_MODE_RESTORE_FUNC)
(
    IN GT_U32       savedmode
);
typedef struct {
    TGF_LOG_MESSAGE_FUNC            logMessageFunc;
    TGF_EQUAL_VERIFY_FUNC           equalVerifyFunc;
    TGF_FAILURE_MSG_LOG_FUNC        failureMgsLogFunc;
    TGF_PRINT_KEEPALIVE_FUNC        printKeepAliveFunc;
    TGF_LOG_PRINT_MODE_FINAL_FUNC   logPrintModeFinalFunc;
    TGF_LOG_PRINT_MODE_RESTORE_FUNC logPrintModeRestoreFunc;

} TGF_LOG_BINDINGS_STC;


/* TODO */
GT_VOID prvTgfLogBindFunctions
(
    IN TGF_LOG_BINDINGS_STC *bindings
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvUtfHelpersh */

