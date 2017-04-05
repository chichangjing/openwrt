/****************************************************
* cmdFS.h
*
* DESCRIPTION:
*       file system API.
*       Currently required for Lua running on board
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/
#ifndef __cmdFS_h__
#define __cmdFS_h__

#include <extUtils/FS/genFS.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/***** Public Functions ************************************************/

/*******************************************************************************
* cmdFSinit
*
* DESCRIPTION:
*       Initialize cmdFS, initialize built-in files
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0   - on success
*
* COMMENTS:
*
*******************************************************************************/
int cmdFSinit(void);

extern genFS_STC cmdFS;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cmdFS_h__ */
