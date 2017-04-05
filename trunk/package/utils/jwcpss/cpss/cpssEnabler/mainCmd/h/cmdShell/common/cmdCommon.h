/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdCommon.h
*
* DESCRIPTION:
*       Common definition and APIs for Commander Shell and wrappers.
*
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#ifndef __cmdCommon_h_
#define __cmdCommon_h_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* include the external services */
#include <cmdShell/common/cmdExtServices.h>

/***** Constants and Enumerations **************************************/
#define CMD_MAX_BUFFER  4096    /* maximum input buffer size    */

/*
 * Enumeration - CMD_STATUS
 *
 * Description:
 *      Galtis Agent specific error codes
 *
 * Fields:
 *      CMD_OK              - no error
 *      CMD_AGENT_ERROR     - internal error in galtis agent
 *      CMD_SYNTAX_ERROR    - generic syntax error
 *      CMD_ARG_UNDERFLOW   - not enough arguments
 *      CMD_ARG_OVERFLOW    - too many arguments
 *      CMD_FIELD_UNDERFLOW - not enough fields
 *      CMD_FIELD_OVERFLOW  - too many fields
 */
typedef enum
{
    CMD_OK                  = GT_OK,
    CMD_AGENT_ERROR         = 0x1000,
    CMD_SYNTAX_ERROR,
    CMD_ARG_UNDERFLOW,
    CMD_ARG_OVERFLOW,
    CMD_FIELD_UNDERFLOW,
    CMD_FIELD_OVERFLOW
} CMD_STATUS;

/***** Linked List Macros **********************************************/

/*
 * Definition - LINK_ADD
 *
 *  Description:
 *      append an item to global linked list
 *
 *  Parameters:
 *      leaf - a pionter to an item to add to the link list
 *      head - a pionter to the beginning of the list
 *      tail - a pionter to the end of the list
 */
#define LINK_ADD(leaf, head, tail)  \
    if (head) {                     \
        tail->next = leaf;          \
        leaf->prev = tail;          \
        tail = leaf;                \
    }                               \
    else {                          \
        head = leaf;                \
        tail = leaf;                \
    }

/*
 * Definition - LINK_DEL
 *
 *  Description:
 *      remove an item from global linked list
 *
 *  Parameters:
 *      leaf - a pointer to an item to remove from the link list
 *      head - a pointer to the beginning of the list
 *      tail - a pointer to the end of the list
 */
#define LINK_DEL(leaf, head, tail)      \
    if (leaf->prev)                     \
        leaf->prev->next = leaf->next;  \
    else                                \
        head = leaf->next;              \
    if (leaf->next)                     \
        leaf->next->prev = leaf->prev;  \
    else                                \
        tail = leaf->prev;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__cmdCommon_h_*/
