/*******************************************************************************
* osObjIdLib.h
*
* DESCRIPTION:
*       object by id support micro lib
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#ifndef _osObjIdLib_h_
#define _osObjIdLib_h_

#include <limits.h>

#define OS_OBJ_NAME_LEN_CNS 16
typedef struct osObjectHeaderSTC {
    int     type; /* 0 means unused */
    char    name[OS_OBJ_NAME_LEN_CNS];
} OS_OBJECT_HEADER_STC;

typedef void* (*OS_OBJ_ALLOC_FUNC_TYPE)(size_t);
typedef void (*OS_OBJ_FREE_FUNC_TYPE)(void*);
typedef struct {
    OS_OBJECT_HEADER_STC    **list;
    int                     allocated;
    int                     allocChunk;
    size_t                  objSize;
    OS_OBJ_ALLOC_FUNC_TYPE  allocFuncPtr;
    OS_OBJ_FREE_FUNC_TYPE   freeFuncPtr;
} OS_OBJECT_LIST_STC;



/*******************************************************************************
* osObjLibGetNewObject
*
* DESCRIPTION:
*       allocate new object
*
* INPUTS:
*       objListPtr  - pointer to object list structure
*       type        - type assign to object
*       name        - object name
*
* OUTPUTS:
*       objPtr      - pointer to object
*
* RETURNS:
*       obj Id
*       <= 0 on error
*
* COMMENTS:
*       list must be already protected
*
*******************************************************************************/
int osObjLibGetNewObject
(
    OS_OBJECT_LIST_STC      *objListPtr,
    int                     type,
    const char              *name,
    OS_OBJECT_HEADER_STC*   *objPtr
);


#endif /* _osObjIdLib_h_ */
