/*******************************************************************************
* osObjIdLib.c
*
* DESCRIPTION:
*       object by id support micro lib
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#include <string.h>

#include <gtOs/osObjIdLib.h>



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
)
{
    int id;

#define _OBJ objListPtr->list[id]
    /* look for slot */
    for (id = 1; id < objListPtr->allocated; id++)
    {
        if (_OBJ == NULL)
        {
            _OBJ = (OS_OBJECT_HEADER_STC*)
                   objListPtr->allocFuncPtr(objListPtr->objSize);
            _OBJ->type = 0;
        }
        if (_OBJ->type == 0)
            break;

    }
    if (id >= objListPtr->allocated)
    {
        OS_OBJECT_HEADER_STC **newArr;

        /* not found, extend array */
        newArr = (OS_OBJECT_HEADER_STC **)objListPtr->allocFuncPtr(
                        sizeof(OS_OBJECT_HEADER_STC *)
                        * (objListPtr->allocated+(objListPtr->allocChunk)));
        if (newArr == NULL)
            return -1;
        if (objListPtr->list)
        {
            memcpy(newArr, objListPtr->list, 
                    sizeof(OS_OBJECT_HEADER_STC*) * objListPtr->allocated);
        }
        memset(newArr + objListPtr->allocated, 0,
                    sizeof(OS_OBJECT_HEADER_STC*) * objListPtr->allocChunk);
        if (objListPtr->list)
        {
            objListPtr->freeFuncPtr(objListPtr->list);
        }
        objListPtr->list = newArr;
        id = objListPtr->allocated;
        if (id == 0)
            id = 1;
        objListPtr->allocated += objListPtr->allocChunk;
        _OBJ = (OS_OBJECT_HEADER_STC*) objListPtr->allocFuncPtr(objListPtr->objSize);
    }

    memset (_OBJ, 0, objListPtr->objSize);
    _OBJ->type = type;
    strncpy(_OBJ->name, name?name:"", sizeof(_OBJ->name));
    _OBJ->name[sizeof(_OBJ->name)-1] = 0;
    *objPtr = _OBJ;
    return id;
#undef  _OBJ
}
