/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
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
* gtOsGen.h
*
* DESCRIPTION:
*       Operating System wrapper general APIs
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*******************************************************************************/

#ifndef __gtOsGenh
#define __gtOsGenh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>

#define IS_WRAPPER_OPEN_STATUS  \
    if (osWrapperIsOpen() == GT_FALSE)  \
    {                                   \
        return GT_FAIL;                 \
    }

#define IS_WRAPPER_OPEN_PTR     \
    if (osWrapperIsOpen() == GT_FALSE)  \
    {                                   \
        return 0;                    \
    }

/************* Functions ******************************************************/
/*******************************************************************************
* osWrapperOpen
*
* DESCRIPTION:
*       Initialize OS wrapper facility.
*
* INPUTS:
*       osSpec  - ptr to OS specific init parameter(s)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_ALREADY_EXIST - wrapper was already open
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osWrapperOpen(void * osSpec);

/*******************************************************************************
* osWrapperClose
*
* DESCRIPTION:
*       Close OS wrapper facility and free all used resources.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS osWrapperClose(void);

/*******************************************************************************
* osWrapperIsOpen
*
* DESCRIPTION:
*       Returns GT_TRUE if OS wrapper facility was initialized .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_TRUE          - on success
*       GT_FALSE         - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_INLINE GT_BOOL osWrapperIsOpen(void);

#ifdef __cplusplus
}
#endif

#endif  /* __gtOsGenh */
/* Do Not Add Anything Below This Line */

