#error "Deprecated"
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
* gtOsMem.h
*
* DESCRIPTION:
*       Operating System wrapper. Memory manipulation facility.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cliMemLibh
#define __cliMemLibh

#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>


/*******************************************************************************
* CLI_MEMORY_ALLOC_FUNC
*
* DESCRIPTION:
*       Allocates memory
*
* INPUTS:
*       size - size in bytes
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the allocated memory or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID* (*CLI_MEMORY_ALLOC_FUNC)
(
    unsigned int size
);

/*******************************************************************************
* CLI_MEMORY_FREE_FUNC
*
* DESCRIPTION:
*       Free allocated memory
*
* INPUTS:
*       ptr - A pointer to the allocated memory
*
* OUTPUTS:
*       None
*
* RETURNS:
*       A pointer to the allocated memory or NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
typedef GT_VOID (*CLI_MEMORY_FREE_FUNC)
(
    GT_VOID *ptr
);


/*
 * typedef: enum GT_PSS_VERSION_ENT
 *
 * Description: This enumeration defines the memory allocation method
 *
 * Fields:
 *      CLI_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E - in the case of memory 
 *                          chunk depletion an out of memory will be reported
 *      CLI_MEMORY_MANAGE_METHOD_REALLOC_ON_DEMAND_E - in the case of memory 
 *                          chunk depletion a new chunk will be allocated
 */
typedef enum
{
    CLI_MEMORY_MANAGE_METHOD_NO_REALLOC_ON_DEMAND_E,
    CLI_MEMORY_MANAGE_METHOD_REALLOC_ON_DEMAND_E

}CLI_MEMORY_MANAGE_METHOD_ENT;




/************* Functions ******************************************************/

/*******************************************************************************
* cliMemLibInit
*
* DESCRIPTION:
*       Initialize the memory management with a user memory function. 
*       Can be used to allocate from a dedicated memory pool that remains till 
*       HW reset.
*
* INPUTS:
*       memAllocFuncPtr - the function to be used in memory allocation.
*       memFreeFuncPtr - the function to be used in memory allocation.
*       size - The memory size to manage
*       memAllocMethod - determines the method of the allocation 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_NO_RESOURCE - not enough memory in the system.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cliMemLibInit
(    
    IN CLI_MEMORY_ALLOC_FUNC memAllocFuncPtr,
    IN CLI_MEMORY_FREE_FUNC memFreeFuncPtr,
    IN GT_U32 size,
    IN CLI_MEMORY_MANAGE_METHOD_ENT memAllocMethod
);

/*******************************************************************************
* cliMemLibReset
*
* DESCRIPTION:
*       Reset memory pools to empty state
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - not initialized yet
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cliMemLibReset(void);

/*******************************************************************************
* cliMalloc
*
* DESCRIPTION:
*       Allocates memory block of specified size.
*
* INPUTS:
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID *cliMalloc
(
    IN GT_U32 size
);

/*******************************************************************************
* cliRealloc
*
* DESCRIPTION:
*       Reallocate memory block of specified size.
*
* INPUTS:
*       ptr  - pointer to previously allocated buffer
*       size - bytes to allocate
*
* OUTPUTS:
*       None
*
* RETURNS:
*       Void pointer to the allocated space, or NULL if there is
*       insufficient memory available
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*       Recommended to avoid usage of this function.
*
*******************************************************************************/
GT_VOID *cliRealloc
(
    IN GT_VOID * ptr ,
    IN GT_U32    size
);

/*******************************************************************************
* cliFree
*
* DESCRIPTION:
*       Deallocates or frees a specified memory block.
*
* INPUTS:
*       memblock - previously allocated memory block to be freed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Usage of this function is NEVER during initialization.
*
*******************************************************************************/
GT_VOID cliFree
(
    IN GT_VOID* const memblock
);

#ifdef __cplusplus
}
#endif

#endif  /* __cliMemLibh */
/* Do Not Add Anything Below This Line */

