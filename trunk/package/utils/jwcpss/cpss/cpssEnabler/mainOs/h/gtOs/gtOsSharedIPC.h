/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* gtOsSharedIPC.h
*
* DESCRIPTION:
*       
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*******************************************************************************/

#ifndef __gtOsSharedIPCh
#define __gtOsSharedIPCh


#ifdef __cplusplus
extern "C" {
#endif

/************* Includes *******************************************************/

#include <gtOs/gtGenTypes.h>
#ifdef CHX_FAMILY
/*
*	This struct used to transmit the Transmit Start parameters from appDemo process to
* 	RxTxProcess
*	Fields:
*
*	cpssDxChTxPacketDescTbl			-	pointer to packet descriptor table
*	modeSettings includes two fields:   
*			cyclesNum		-	The maximum number of loop cycles (-1 = forever)
*			gap	   		-	The time to wit between two cycles.
*	txDxChTblCapacity 			-	number of descriptors in the table
*
*/

typedef struct{
    void* cpssDxChTxPacketDescTbl;
    struct
    {
        GT_U32  cyclesNum;
        GT_U32  gap;
    }  modeSettings;
    GT_U32 txDxChTblCapacity;
#ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
    void * txPoolPtr;
#endif
} CPSS_MP_DXCH_START_TRANSMIT_PARAMS_STC;

/*
*	This struct used to transmit the Transmit Stop parameters from appDemo process to
* 	RxTxProcess
*	Fields:
*
*/
typedef struct{
} CPSS_MP_DXCH_STOP_TRANSMIT_PARAMS_STC;

#endif /* CHX_FAMILY */

#ifdef  EXMXPM_FAMILY
#include <galtisAgent/wrapUtil/cmdCpssNetReceive.h>

#include <galtisAgent/wrapCpss/Gen/networkIf/wrapCpssGenNetIf.h>

/*
*	This struct used to transmit the Transmit Start parameters from appDemo process to
* 	RxTxProcess
*	Fields:
*
*	cpssDxChTxPacketDescTbl			-	pointer to packet descriptor table
*	modeSettings includes two fields:   
*			cyclesNum		-	The maximum number of loop cycles (-1 = forever)
*			gap	   		-	The time to wit between two cycles.
*	txDxChTblCapacity 			-	number of descriptors in the table
*
*/

typedef struct
{
    GT_U32  cyclesNum;
    GT_U32  gap;
} EXMXPM_MODE_SETTINGS_STC;


typedef struct{
	CPSS_TX_PKT_DESC * cpssExMxPmTxPacketDescTbl;
	EXMXPM_MODE_SETTINGS_STC modeSettings;
	GT_U32 txExMxPmTxTblCapacity;
} CPSS_MP_EXMXPM_START_TRANSMIT_PARAMS_STC;

/*
*	This struct used to transmit the Transmit Stop parameters from appDemo process to
* 	RxTxProcess
*	Fields:
*
*/
typedef struct{
} CPSS_MP_EXMXPM_STOP_TRANSMIT_PARAMS_STC;

#endif /* EXMXPM_FAMILY */


/*
*	This struct used to transmit the NetIfRxPacketTableClear command from appDemo
*	process to RxTxProcess
*	Fields:
*
*/
typedef struct{
} CPSS_MP_CLEAR_RX_PACKET_TABLE_PARAMS_STC;

typedef enum {
    CPSS_MP_REMOTE_RXTX_E = 0,
    CPSS_MP_REMOTE_FDBLEARNING_E,
    CPSS_MP_REMOTE_MAX_E
} CPSS_MP_REMOTE_DEST;

typedef enum {
    CPSS_MP_CMD_RXTX_START_TRANSMIT_E,
    CPSS_MP_CMD_RXTX_STOP_TRANSMIT_E,
    CPSS_MP_CMD_RXTX_RXPKT_CLEAR_TBL_E
} CPSS_MP_COMMAND;

/* this structure used to transmit commands from appDemo to other processes
 *
 * Fields:
 *      remote  - remote program ideitifier (RxTxProcess, FdbLearning, etc)
 *      command - command ID
 *      data    - storage for parameters
 */
typedef struct {
    CPSS_MP_REMOTE_DEST     remote;
    CPSS_MP_COMMAND         command;

    union {
#ifdef CHX_FAMILY
        CPSS_MP_DXCH_START_TRANSMIT_PARAMS_STC   rxTxStartTransmit;
        CPSS_MP_DXCH_STOP_TRANSMIT_PARAMS_STC    rxTxStopTransmit;
#endif
#ifdef EXMXPM_FAMILY
        CPSS_MP_EXMXPM_START_TRANSMIT_PARAMS_STC   rxTxStartTransmit;
        CPSS_MP_EXMXPM_STOP_TRANSMIT_PARAMS_STC    rxTxStopTransmit;
#endif
        CPSS_MP_CLEAR_RX_PACKET_TABLE_PARAMS_STC rxTxClearRxPacketTable;
    } data;
} CPSS_MP_REMOTE_COMMAND;





/*******************************************************************************
* cpssMultiProcComExecute
*
* DESCRIPTION:
*       
*	Used to execute function in another process
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_MP_REMOTE_COMMAND     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComExecute
(
	CPSS_MP_REMOTE_COMMAND *paramPtr
);

/*******************************************************************************
* cpssMultiProcComWait
*
* DESCRIPTION:
*       
*	Used to to wait for command in another process
*
* INPUTS:
*       program  - program identifier which waits for command
*       
*
* OUTPUTS:
*  		paramPtr - pointer to struct CPSS_MP_REMOTE_COMMAND     
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComWait
(
    IN  CPSS_MP_REMOTE_DEST    program,
	OUT CPSS_MP_REMOTE_COMMAND *paramPtr
);

/*******************************************************************************
* cpssMultiProcComComplete
*
* DESCRIPTION:
*       
*	Used to confirm remote command finished
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_MP_REMOTE_COMMAND     
*       rc       - return code
*       
* OUTPUTS:
*       none
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComComplete
(
	IN  CPSS_MP_REMOTE_COMMAND *paramPtr,
	IN  GT_STATUS              rc
);


#ifdef __cplusplus
}
#endif

#endif  /* __gtOsSharedIPCh */
/* Do Not Add Anything Below This Line */
