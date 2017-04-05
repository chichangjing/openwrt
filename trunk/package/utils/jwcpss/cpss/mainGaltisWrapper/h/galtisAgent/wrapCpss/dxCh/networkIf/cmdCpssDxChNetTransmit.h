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
* cmdCpssDxChNetTransmit.h
*
* DESCRIPTION:
*       This file implements Receive/Transmit functionaly user exit functions
*       for Galtis.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*
******************************************************************************/

#ifndef __cmdCpssDxChNetTransmith__
#define __cmdCpssDxChNetTransmith__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include <cpss/generic/cos/cpssCosTypes.h>
#include <cpss/generic/networkIf/cpssGenNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>

#define   MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN   64
/*number of buffer at the pool*/
#define   MAX_NUM_OF_BUFFERS_AT_POOL 124
#define   TX_BUFFER_SIZE 500

/* Size of block to aligned to. Must be power of 2, 2 is minimum */
#define ALIGN_ADDR_CHUNK 64
/* This macro used to get alighnment address from allocated */
#define ALIGN_ADR_FROM_MEM_BLOCK(adr,align)\
                   (( ((GT_U32)( ((GT_U8*)(adr)) + (ALIGN_ADDR_CHUNK) - 1)) &\
                   ((0xFFFFFFFF - (ALIGN_ADDR_CHUNK) + 1) )) + (align))


/*
 * Typedef: enum TX_CMD_ENT
 *
 * Description: Enumeration of Transmit command Types
 *
 * Enumerations:
 *      TX_BY_VIDX  - Send packets using "netSendPktByVid" function and
 *                    defines to use spesific parameters of this function.
 *      TX_BY_PORT  - Send packets using "netSendPktByLport" function and
 *                    defines to use spesific parameters of this function.
 */
typedef enum
{
    DXCH_TX_BY_VIDX = 0,
    DXCH_TX_BY_PORT
} DXCH_TX_CMD_ENT;


/*
 * typedef: structure DXCH_PKT_DESC_STC
 *
 * Description: structure of Tx parameters , that contain full information on
 *              how to send TX packet from CPU to PP.
 *
 * fields:
 *      cmdType         - Defines type of transmition (VLAN, Port).
 *      valid           - This flag indicates that the entry is valid (or not)
 *      entryID         - Entry ID number for Delete/Get/ChangeAllign operations
 *      txSyncMode      - should the packet be send in a sync mode or not
 *      packetIsTagged - the packet (buffers) contain the vlan TAG inside.
 *                  GT_TRUE  - the packet contain the vlan TAG inside.
 *                  GT_FALSE - the packet NOT contain the vlan TAG inside.
 *
 *      cookie  - The user's data to be returned to the Application when in
 *                the Application's buffers can be re-used .
 *
 *      sdmaInfo - the info needed when send packets using the SDMA.
 *
 *      dsaParam - the DSA parameters
 *      pcktsNum        - Number of packets to send.
 *      gap             - The time is calculated in multiples of 64 clock cycles
 *                        Valid values are 0 (no delay between packets to
 *                        CPU), through 0x3FFE (1,048,544 Clk cycles).
 *                        0x3FFF - has same effect as 0.
 *      waitTime        - The wait time before moving to the next entry.
 *      pcktData        - Array of pointers to packet binary data.
 *      pcktDataLen     - Array of lengths of pcktData pointers.
 *      numOfBuffers    - number of buffers used by pcktData.
 *      numSentPackets  - Number of sent packets.
 *
*/
typedef struct{
    DXCH_TX_CMD_ENT                 cmdType;
    GT_BOOL                         valid;
    GT_U32                          entryId;
    GT_BOOL                         txSyncMode;

    GT_BOOL                         packetIsTagged;
    /*GT_PTR                        cookie;*/
    CPSS_DXCH_NET_SDMA_TX_PARAMS_STC    sdmaInfo;
    CPSS_DXCH_NET_DSA_PARAMS_STC        dsaParam;

    GT_U32                          pcktsNum;
    GT_U32                          gap;
    GT_U32                          waitTime;
    GT_U8                           *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    GT_U32                          pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN];
    GT_U32                          numOfBuffers;
    GT_U32                          numSentPackets;

}DXCH_PKT_DESC_STC; /*CPSS_NET_TX_PARAMS_STC;*/

/*
 * Typedef: enum DXCH_TX_GEN_ACTION_TYPE_ENT
 *
 * Description: Enumeration of traffic generator entry action
 *
 * Enumerations:
 *      DXCH_TX_GEN_ACTION_ADD_E    - execute add procedure, save packet id in the DB.
 *      DXCH_TX_GEN_ACTION_UPDATE_E - execute update procedure, use packet ID to call “update” API
 */
typedef enum {
  DXCH_TX_GEN_ACTION_ADD_E,
  DXCH_TX_GEN_ACTION_UPDATE_E
} DXCH_TX_GEN_ACTION_TYPE_ENT;

/*
 * typedef: structure DXCH_PKT_GEN_DESC_STC
 *
 * Description: structure of Tx generator parameters, that contain full information on
 *              how to transmit TX packet in scope of traffic generator.
 *
 * fields:
 *      actionType          - add/update entry
 *      dsaType             - the type of DSA tag (FORWARD/FROM_CPU)
 *      packetGenDescIndex  - index in global descriptor tables
 *      packetId            - parameter of traffic generator API.
 */
typedef struct {
    DXCH_TX_GEN_ACTION_TYPE_ENT     actionType;
    CPSS_DXCH_NET_DSA_CMD_ENT       dsaType;
    GT_U32                          packetGenDescIndex;
    GT_U32                          packetId;
}DXCH_PKT_GEN_DESC_STC;
/*
 * Typedef: struct MODE_SETINGS_STC
 *
 * Description: Packet Description structure
 *
 *      cyclesNum - The maximum number of loop cycles (-1 = forever)
 *      gap       - The time to wit between two cycles.
 *
 */

typedef struct
{
    GT_U32  cyclesNum;
    GT_U32  gap;
} DXCH_MODE_SETINGS_STC;


/*
 * Typedef: enum GT_MAC_TYPE
 *
 * Description: Defines the different Mac-Da types of a transmitted packet.
 *
 * Fields:
 *      UNICAST_MAC         - MAC_DA[0] = 1'b0
 *      MULTICAST_MAC       - MAC_DA[0] = 1'b1
 *      BROADCAST_MAC       - MAC_DA = 0xFFFFFFFF
 *
 */
typedef enum
{
    UNICAST_MAC,
    MULTICAST_MAC,
    BROADCAST_MAC
}DXCH_MAC_TYPE;

/*******************************************************************************
* cmdCpssDxChTxSetPacketDesc
*
* DESCRIPTION:
*       Creates new transmit parameters description entry in the table.
*
* INPUTS:
*       entryID      - Entry ID of packet descriptor to return.
*       packetDesc   - Pointer to Packet descriptor new entry in the table
*       extended     - flag indicates if this is the extended table
*
* OUTPUTS:
*       newEntryIndex   - The index of the new entry in the table.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPcktDescs
*
* Toolkit:
*       Using - <netSendPktByLport, netSendPktByIf, netSendPktByVid,
*                netFreeTxGtBuf, gtBufCreate>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxSetPacketDesc
(
    IN  GT_U32                  entryId,
    IN  DXCH_PKT_DESC_STC       *packetDesc,
    IN  GT_BOOL                 extended,
    OUT GT_U32                  *newEntryIndex
);

/*******************************************************************************
* cmdCpssDxChTxGetPacketDesc
*
* DESCRIPTION:
*       Returns specific packet descriptor entry from the table.
*
* INPUTS:
*       entryIndex   - Entry index in packet descriptor table.
*       dsaCmd       - DSA tag commands
*
* OUTPUTS:
*     packetDesc   - Packet descriptor from the table.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL  - on error
*       GT_EMPTY - on invalid index
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGetPacketDesc
(
    INOUT GT_U32            *entryIndex,
    IN CPSS_DXCH_NET_DSA_CMD_ENT  dsaCmd,
    OUT DXCH_PKT_DESC_STC   *packetDesc
);

/*******************************************************************************
* cmdCpssDxChTxBeginGetPacketDesc
*
* DESCRIPTION:
*       Indicates for cmdTxGetPacketDesc function that user go to get many
*       records in one session. This API make exact copy of table so that
*       user will get table without changes that can be made during get
*       operation. This mechanism is best suitable for Refresh table
*       operation.
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
*
* GalTis:
*       Table - cmdTxPacketDescs
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxBeginGetPacketDesc
(
void
);

/*******************************************************************************
* cmdCpssDxChTxEndGetPacketDesc
*
* DESCRIPTION:
*       Finish operation began by cmdCpssDxChTxBeginGetPacketDesc API function
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
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxEndGetPacketDesc
(
    void
);

/*******************************************************************************
* cmdCpssDxChTxClearPacketDesc
*
* DESCRIPTION:
*       Clear all packet descriptors entries from the table.
*
* INPUTS:
*     dsaCmd       - DSA tag commands
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Table - cmdTxPacketDescs
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxClearPacketDesc
(
    IN CPSS_DXCH_NET_DSA_CMD_ENT  dsaCmd
);

/*******************************************************************************
* cmdCpssDxChTxDelPacketDesc
*
* DESCRIPTION:
*       Delete packet descriptor entry from the table.
*
* INPUTS:
*       entryID     - Entry ID of packet descriptor to delete.
*       dsaCmd      - DSA tag commands
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*
* COMMENTS:
*       After delete operation all record will be shifted
*       so the table will be not fragmented.
*
* GalTis:
*       Table - cmdTxPcktDescs
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxDelPacketDesc
(
    IN  GT_U32      entryID,
    IN CPSS_DXCH_NET_DSA_CMD_ENT  dsaCmd
);

/*******************************************************************************
* cmdCpssDxChTxStart
*
* DESCRIPTION:
*       Starts transmition of packets
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command   - cmdCpssDxChTxStart
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxStart
(
void
);

/*******************************************************************************
* cmdCpssDxChTxStop
*
* DESCRIPTION:
*       Stop transmition of packets.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command   - cmdCpssDxChTxStop
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*       Using     - <netSendPktByLport, netSendPktByIf, netSendPktByVid,
*                    netFreeTxGtBuf, gtBufCreate>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxStop
(
void
);

/*******************************************************************************
* cmdCpssDxChTxSetMode
*
* DESCRIPTION:
*       This command will set the the transmit parameters.
*
* INPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command     - cmdTxSetMode
*
* Toolkit:
*       Interface   - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxSetMode
(
    IN GT_U32   cyclesNum,
    IN GT_U32   gap
);

/*******************************************************************************
* cmdCpssDxChTxGetMode
*
* DESCRIPTION:
*       This command will get the the transmit parameters.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       cyclesNum - The maximum number of loop cycles (-1 = forever)
*       gap       - The time to wit between two cycles.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*       Command - cmdTxGetMode
*
* Toolkit:
*       Interface - <prestera/tapi/networkif/commands.api>
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGetMode
(
    OUT GT_U32 *cyclesNum,
    OUT GT_U32 *gap
);

/*******************************************************************************
* cmdCpssDxChConvertPacketDesc
*
* DESCRIPTION:
*       Convert packet desciptor structure to CPSS_DXCH_NET_TX_PARAMS_STC
*
* INPUTS:
*       netTxPacketDescPtr      - pointer to TX packet descriptor structure
*
* OUTPUTS:
*       cpssNetTxPacketDescPtr  - pointer to CPSS TX packet descriptor structure
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
GT_VOID cmdCpssDxChConvertPacketDesc
(
    IN DXCH_PKT_DESC_STC                * netTxPacketDescPtr,
    OUT CPSS_DXCH_NET_TX_PARAMS_STC     * cpssNetTxPacketDescPtr
);

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescSet
*
* DESCRIPTION:
*       Bind TX generator transmit parameters description entry to DB.
*
* INPUTS:
*       packetGenDescPtr        - pointer to TX Packet generator descriptor
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator descriptor table entry
*       packetGenDescTblSizePtr - pointer to TX packet generator descriptor tale size
*
* OUTPUTS:
*       packetDescTblEntryPtr - pointer to updated TX packet generator descriptor table entry
*       packetDescTblSizePtr - pointer to updated TX packet generator descriptor tale size
*       newEntryIndex   - The index of the new entry in the table.
*
* RETURNS:
*       GT_OK               - on success
*       GT_OUT_OF_CPU_MEM   - on memory allocation fail
*       GT_ALREADY_EXIST    - on already exist entry
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescSet
(
    IN  DXCH_PKT_GEN_DESC_STC       * packetGenDescPtr,
    INOUT DXCH_PKT_GEN_DESC_STC     ** packetGenDescTblEntryPtrPtr,
    INOUT GT_U32                    * packetGenDescTblSizePtr,
    OUT GT_U32                      * newEntryIndexPtr
);

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescDelete
*
* DESCRIPTION:
*       Delete TX generator transmit parameters description entry.
*
* INPUTS:
*       devNum                      - Device number.
*       txQueue                     - Tx queue.
*       portGroup                   - Port group ID.
*       packetId                    - Packet ID to delete.
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator table
*
* OUTPUTS:
*       packetGenDescTblEntryPtrPtr - pointer to TX packet generator table
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - on entry not found
*       GT_BAD_PTR      - on bad entry pointer
*       GT_EMPTY        - on empty table
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescDelete
(
    IN GT_U8                            devNum,
    IN GT_U8                            txQueue,
    IN GT_U32                           portGroup,
    IN GT_U32                           packetId,
    INOUT DXCH_PKT_GEN_DESC_STC      ** packetGenDescTblEntryPtrPtr,
    INOUT GT_U32                      * packetGenDescTblSizePtr
);

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescClearAll
*
* DESCRIPTION:
*       Clear TX generator transmit parameters DB.
*
* INPUTS:
*       devNum                      - Device number.
*       txQueue                     - Tx queue.
*       portGroup                   - Port group ID.
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator table
*
* OUTPUTS:
*       packetGenDescTblEntryPtrPtr - pointer to pointer to TX packet generator table
*
* RETURNS:
*       GT_OK           - on success
*       GT_NOT_FOUND    - on entry not found
*       GT_EMPTY        - on empty table
*
* COMMENTS:
*       None
*
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescClearAll
(
    IN GT_U8                            devNum,
    IN GT_U8                            txQueue,
    IN GT_U32                           portGroup,
    INOUT DXCH_PKT_GEN_DESC_STC      ** packetGenDescTblEntryPtrPtr
);

/*******************************************************************************
* cmdCpssDxChTxGenPacketDescGet
*
* DESCRIPTION:
*       Returns TX generator packet descriptor entry from the table.
*
* INPUTS:
*       entryIndex          - table entry index
*       packetGenDescTblEntryPtr
*                           - pointer to TX packet generator table
*       packetGenDescTblSize
*                           - TX packet generator table size
*
* OUTPUTS:
*       packetGenDescPtr    - pointer to entry in TX packet generator descriptor table.
*
* RETURNS:
*       GT_OK    - on success
*       GT_BAD_PTR - on bad entry pointer
*       GT_OUT_OF_RANGE - on invalid index
*       GT_EMPTY - on empty table
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdCpssDxChTxGenPacketDescGet
(
    IN  GT_U32                      entryIndex,
    IN  DXCH_PKT_GEN_DESC_STC       * packetGenDescTblEntryPtr,
    IN  GT_U32                      packetGenDescTblSize,
    OUT DXCH_PKT_GEN_DESC_STC       * packetGenDescPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /*__cmdCpssDxChNetTransmith__*/


