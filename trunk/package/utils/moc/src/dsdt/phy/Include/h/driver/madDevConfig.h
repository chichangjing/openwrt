#include <madCopyright.h>

/********************************************************************************
* madDevConfig.h
*
* DESCRIPTION:
*       Includes device specific configurations.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __madDevConfigh
#define __madDevConfigh

#ifdef __cplusplus
extern "C" {
#endif

#include <madApi.h>

/*******************************************************************************
* madDevPreInit
*
* DESCRIPTION:
*       This function configures Marvell Phy Device to function properly.
*       This function applied only once when driver is loaded. 
*
* INPUTS:
*        None.
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_OK               - on success, or
*       MAD_FAIL             - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MAD_STATUS madDevPreInit
(
    IN MAD_DEV    *dev
);

#ifdef __cplusplus
}
#endif

#endif /* __madDevConfigh */
