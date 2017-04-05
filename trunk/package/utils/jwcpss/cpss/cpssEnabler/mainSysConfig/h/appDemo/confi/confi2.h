/*******************************************************************************
*         Copyright 2004, MARVELL SEMICONDUCTOR ISRAEL, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL SEMICONDUCTOR ISRAEL. (MSIL),  MARVELL TAIWAN, LTD. AND      *
* SYSKONNECT GMBH.                                                             *
********************************************************************************
* confi2.h
*                                                                                
* DESCRIPTION: 
*       Contains API for configuration script processing.
*                                                                                
* DEPENDENCIES:                                                              
*       None.                                                                    
*                                                                                
* FILE REVISION NUMBER:                                                      
*       $Revision: 3 $                                                           
*                                                                                
*******************************************************************************/
#ifndef __confi2h
#define __confi2h

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <gtOs/gtGenTypes.h>

/*
 * Typedef: struct GT_CONFI_BUFFER
 *
 * Description:
 *    This structure contains byte array data and length.
 *
 * Fields:
 *   buf      - pointer to buffer
 *   bufSize  - buffer size
 *   dataSize - data size
 */
typedef struct
{
    GT_CHAR *buf;      
    GT_U32  bufSize;   
    GT_U32  dataSize;  

} GT_CONFI_BUFFER;

/*******************************************************************************
* confiRead
*
* DESCRIPTION:
*       Read configuration script from input stream.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*   
*
*******************************************************************************/
GT_STATUS   confi2Read();


/*******************************************************************************
* confi2FileRead
*
* DESCRIPTION:
*       Read configuration script from file.
*
* INPUTS:
*       filename - filename.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*   
*
*******************************************************************************/
GT_STATUS   confi2FileRead
(
    IN  GT_U8   *filename
);


/*******************************************************************************
* confiRestore
*
* DESCRIPTION:
*       Restore the confi binary format from the flash
*
* INPUTS:
*       theConfiId  - the confi ID
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                   - succeeded
*       GT_OUT_OF_CPU_MEM       - out of CPU memory
*       GT_ERROR                - failed
*
* COMMENTS:
*       Public.
*
*******************************************************************************/
GT_STATUS confi2Restore
(
    IN GT_U8        theConfiId
);


/*******************************************************************************
* confiStore
*
* DESCRIPTION:
*       Store the confi binary format to the flash
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       theConfiId  - the confi ID
*
* RETURNS:
*       GT_OK                   - succeeded
*       GT_OUT_OF_CPU_MEM       - out of CPU memory
*       GT_ERROR                - failed
*
* COMMENTS:
*       Public.
*
*******************************************************************************/
GT_STATUS confi2Store
(
    IN GT_U8        theConfiId
);

/*******************************************************************************
* confiPrint
*
* DESCRIPTION:
*       Writes the contents of the confi buffer to the standard output. 
*
* INPUTS:
*       - None
*
* OUTPUTS:
*       - None
*
* RETURNS:
*       GT_OK           - finished successfully
*       GT_ERROR        - error 
*
* COMMENTS:
*
*       Public
*
*******************************************************************************/
GT_STATUS confi2Print
(
    GT_VOID
);


/*******************************************************************************
* confi2InitSystem
*
* DESCRIPTION:
*       This is the main board initialization function.
*
* INPUTS:
*       theConfiId  - the confi ID
*       echoOn      - if GT_TRUE the echo is on, otherwise the echo is off.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - if succeeded,
*       GT_FAIL - if failed
*
* COMMENTS:
*   
*
*******************************************************************************/
GT_STATUS   confi2InitSystem
(
    IN GT_U8        theConfiId, 
    IN GT_BOOL      echoOn
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __confiLoaderh */
