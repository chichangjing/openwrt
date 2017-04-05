/*******************************************************************************
*              Copyright 2002, MARVELL TECHNOLOGY, LTD.
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
* appDemoFap20mEvents.c
*
* DESCRIPTION:
*       Initialization and handling FAP20M/FX950 events.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/

/* Cpss includes */
#include <cpss/generic/private/prvCpssCommonDefs.h>
#include <gtExtDrv/drivers/gtIntDrv.h>
#include <appDemo/sandConfig/appDemoFap20mConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

#ifdef IMPL_GALTIS
#include <galtisAgent/wrapUtil/wrapAppDemoFx950_api_interrupts.h>
#endif

/* FX950/FAP20M includes */
#include <FX950/include/fap20m_api_cell.h>
#include <FX950/include/fap20m_cell.h>
#include <FX950/include/fap20m_api_diagnostics.h>
#include <FX950/include/fap20m_api_callback.h>
#include <FX950/include/fap20m_interrupt_services.h>
#include <FX950/include/fx950_api_interrupts.h>
#include <FX950/include/fx950_regs.h>
#include <FX950/include/fx950_interrupt_services.h>

#include <gtOs/gtOsMsgQ.h>
#include <gtOs/gtOsStdLib.h>


/* Fx950 includes */



#ifdef PRESTERA_DEBUG
#define FAP20M_APP_DEBUG
#endif

#define FAP20M_APP_DEBUG
#ifdef FAP20M_APP_DEBUG
#define DBG_INFO(x)     cpssOsPrintf x
#else
#define DBG_INFO(x)
#endif


/* flag to open debug of FX950 ISR */
volatile int appDemoFx950IsrDebug = 0;

/* flag to open debug of FX950 Interrupt handlers */
volatile int appDemoFx950IntDebug = 0;

/* counter of FX950 ISR calls */
volatile int appDemoFx950IsrCount = 0;

#define ISR_DUMP(x)         if (appDemoFx950IsrDebug)cpssOsPrintSync x
#define INT_DBG_INFO(x)     if (appDemoFx950IntDebug)cpssOsPrintSync x


/* holds the previous status of Fap20m links */
static GT_U32 appDemoFap20mLinkStatusLastResultDb[FAP20M_NUMBER_OF_LINKS];

#ifdef IMPL_GALTIS
/* Event Counter increment function. Implemented in Galtis*/
FX950_EVENT_COUNTER_INCREMENT_FUNC *fx950EventIncrementFunc = NULL;
#endif

#define APPDEMO_FAP20M_GLOBAL_EVENTS_NUM            2

#define APPDEMO_FAP20M_FE_PORT_EVENTS_NUM           FAP20M_NUMBER_OF_LINKS

#define APPDEMO_FAP20M_LINK_STATUS_FIRST_CNTR_CNS   APPDEMO_FAP20M_GLOBAL_EVENTS_NUM

#define APPDEMO_FX950_GLOBAL_FIRST_CNTR_CNS         \
    (APPDEMO_FAP20M_LINK_STATUS_FIRST_CNTR_CNS + APPDEMO_FAP20M_FE_PORT_EVENTS_NUM)
                                                    
#define APPDEMO_FX950_PEX_FIRST_CNTR_CNS            \
    (APPDEMO_FX950_GLOBAL_FIRST_CNTR_CNS + FX950_GLOBAL_NOF_INT)

#define APPDEMO_FX950_PORT_FIRST_CNTR_CNS           \
    (APPDEMO_FX950_PEX_FIRST_CNTR_CNS + FX950_PEX_NOF_INT)

#define APPDEMO_FX950_PORT_LANE_FIRST_CNTR_CNS      \
    (APPDEMO_FX950_PORT_FIRST_CNTR_CNS + (FX950_PORT_NOF_INT * FX950_NOF_HGS_LINKS))

/********************************************************************/
 /*
 *   first parameter - unsigned long -
 *     User identifier of callback. This parameter is returned to the 
 *     user with value indicated by the user when specifying this 
 *     callback procedure. It allows the user to create one callback 
 *     procedure to handle a few cases (using 'switch').
 *   second parameter - unsigned long * -
 *      Pointer to (user supplied) buffer containing required results 
 *      as per action specified forthis callback procedure.
 *      Buffer is of size SAND_CALLBACK_BUF_SIZE and is replaced by 
 *      this procedure (using third parameter  below) every time this 
 *      callback procedure is invoked.
 *      The contents of this buffer vary per callback.
 *    third parameter - unsigned long ** -
 *      This callback procedure loads memory pointed by third parameter 
 *      by pointer to buffer to be used as second parameter on the next 
 *      invocation of this callback procedure. Buffer is, therefore, 
 *      under the same restrictions as for second parameter (size
 *      SAND_CALLBACK_BUF_SIZE, including terminating null).
 *    Fourth parameter - unsigned long -
 *      Size of meaningful results, loaded into buffer pointed by second 
 *      parameter. Must be smaller than maximal size (SAND_CALLBACK_BUF_SIZE).
 *    Fifth parameter - unsigned long -
 *      Error return value. See formatting rules in ERROR RETURN VALUES.
 *      If error code is not SAND_OK then this parameter indicates 
 *      source of error and whether all other parameters are meaningful.
 *    Sixth parameter - unsigned long device_id -
 *      Value of 'device_id' for which this polling
 *      transaction has been started.
 *    Seventh parameter - unsigned long polling_transaction_id -
 *      Value of 'polling_transaction_id' assigned to
 *      this polling transaction.
 *    Eighth parameter - unsigned long param_01 -
 *      Meaning-less if not other specified.
 *    Ninth parameter - unsigned long param_02 -
 *      Meaning-less if not other specified.
*/

static FAP20M_RX_SR_DATA_BUFFER appDemoFap20mRxCellBuffer;

/*********************************************************************/
int appDemoFap20mRxCellCallback(
    unsigned long   user_callback_id, /* not used */ 
    unsigned long  *bufferPtr, 
    unsigned long **newBufferPtrPtr,
    unsigned long   sizeOfBuffer,
    unsigned long   errorCode, 
    unsigned long   deviceId,
    unsigned long   polling_transaction_id, 
    unsigned long   param_01_not_used, 
    unsigned long   param_02_not_used
    )
{
    FAP20M_RX_SR_DATA_BUFFER * cellDataPtr = (FAP20M_RX_SR_DATA_BUFFER *) bufferPtr;

    /* check buffer size */
    if(sizeof(FAP20M_RX_SR_DATA_BUFFER) != sizeOfBuffer)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFap20mRxCellCallback: bad buffer size %d need %d:\n\r", 
                   sizeOfBuffer, sizeof(FAP20M_RX_SR_DATA_BUFFER)));
        return 1;
    }

    /* check pointer */
    if((&appDemoFap20mRxCellBuffer) != cellDataPtr)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFap20mRxCellCallback: bad buffer pointer 0x%X wait 0x%X:\n\r", 
                  cellDataPtr, &appDemoFap20mRxCellBuffer));
        return 1;
    }

    /* use same buffer */
    *newBufferPtrPtr = 0;

    INT_DBG_INFO(("  + appDemoFap20mRxCellCallback: got the Cell:\n\r"));

    if (appDemoFx950IntDebug) 
    {
        fap20m_print_FAP20M_RX_SR_DATA_BUFFER(cellDataPtr);
    }

    /* update Galtis total interrupt counter db */
    if(fx950EventIncrementFunc != NULL)
    {
        /* don't care about the return value */
        (void)fx950EventIncrementFunc(deviceId, 0);
    }

    return 0;
}

static FAP20M_QDR_GET_NUM_SFT_ERR_REPORT  appDemoFap20mQdrGetNumSftErrReportBuffer;

/*********************************************************************/
int appDemoFap20mQdrGetNumSftErrReportCallback(
    unsigned long   user_callback_id, /* not used */ 
    unsigned long  *bufferPtr, 
    unsigned long **newBufferPtrPtr,
    unsigned long   sizeOfBuffer,
    unsigned long   errorCode, 
    unsigned long   deviceId,
    unsigned long   polling_transaction_id, 
    unsigned long   param_01_not_used, 
    unsigned long   param_02_not_used
    )
{
    FAP20M_QDR_GET_NUM_SFT_ERR_REPORT * qdrGetNumSftErrReportPtr = 
        (FAP20M_QDR_GET_NUM_SFT_ERR_REPORT *) bufferPtr;

    /* check buffer size */
    if(sizeof(FAP20M_QDR_GET_NUM_SFT_ERR_REPORT) != sizeOfBuffer)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFap20mQdrGetNumSftErrReportCallback: bad buffer size %d need %d:\n\r", 
                   sizeOfBuffer, sizeof(FAP20M_QDR_GET_NUM_SFT_ERR_REPORT)));
        return 1;
    }

    /* check pointer */
    if((&appDemoFap20mQdrGetNumSftErrReportBuffer) != qdrGetNumSftErrReportPtr)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFap20mQdrGetNumSftErrReportCallback: bad buffer pointer 0x%X wait 0x%X:\n\r", 
                  qdrGetNumSftErrReportPtr, &appDemoFap20mQdrGetNumSftErrReportBuffer));
        return 1;
    }

    /* use same buffer */
    *newBufferPtrPtr = 0;

    INT_DBG_INFO(("  + appDemoFap20mQdrGetNumSftErrReportCallback: results:\n\r"));

    if (appDemoFx950IntDebug) 
    {
        /* Uuser callback invoked only if error counter has actually changed */
        INT_DBG_INFO(("  + Accumulated QDR SFT Error counter is 0x%X\n\r", qdrGetNumSftErrReportPtr->accumulated_qdr_sft_err));
    }

    /* update Galtis total interrupt counter db */
    if(fx950EventIncrementFunc != NULL)
    {
        /* don't care about the return value */
        (void)fx950EventIncrementFunc(deviceId, 1);
    }

    return 0;
}


static FAP20M_ALL_LINK_STATUS  appDemoFap20mLinkStatusBuffer;

/*********************************************************************/
int appDemoFap20mLinkStatusCallback(
    unsigned long   user_callback_id, /* not used */ 
    unsigned long  *bufferPtr, 
    unsigned long **newBufferPtrPtr,
    unsigned long   sizeOfBuffer,
    unsigned long   errorCode, 
    unsigned long   deviceId,
    unsigned long   polling_transaction_id, 
    unsigned long   param_01_not_used, 
    unsigned long   param_02_not_used
    )
{
    unsigned long link_i;
    FAP20M_ALL_LINK_STATUS * linkStatusPtr = (FAP20M_ALL_LINK_STATUS *) bufferPtr;

    /* check buffer size */
    if(sizeof(FAP20M_ALL_LINK_STATUS) != sizeOfBuffer)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFap20mLinkStatusCallback: bad buffer size %d need %d:\n\r", 
                   sizeOfBuffer, sizeof(FAP20M_ALL_LINK_STATUS)));
        return 1;
    }

    /* check pointer */
    if((&appDemoFap20mLinkStatusBuffer) != linkStatusPtr)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFap20mLinkStatusCallback: bad buffer pointer 0x%X wait 0x%X:\n\r", 
                  linkStatusPtr, &appDemoFap20mLinkStatusBuffer));
        return 1;
    }

    /* use same buffer */
    *newBufferPtrPtr = 0;

    INT_DBG_INFO(("  + appDemoFap20mLinkStatusCallback: results:\n\r"));
    
    if (appDemoFx950IntDebug) 
    {
        fap20m_print_FAP20M_ALL_LINK_STATUS(linkStatusPtr, 0);
    }

    /* scan all links for status change */
    for(link_i = 0 ; link_i < FAP20M_NUMBER_OF_LINKS ; ++link_i)
    {
      if (linkStatusPtr->single_link_status[link_i].valid == TRUE)
      {
          /* check if the status was changed */
          if (appDemoFap20mLinkStatusLastResultDb[link_i] != 
              linkStatusPtr->single_link_status[link_i].accepting_cells) 
          {
              /* update Galtis total interrupt counter db */
              if(fx950EventIncrementFunc != NULL)
              {
                  /* don't care about the return value */
                  (void)fx950EventIncrementFunc(deviceId, 
                                                (APPDEMO_FAP20M_LINK_STATUS_FIRST_CNTR_CNS + link_i));

              }

              /* update appDemo link status db */
              appDemoFap20mLinkStatusLastResultDb[link_i] = 
                                linkStatusPtr->single_link_status[link_i].accepting_cells;
          }
      }
    }

    return 0;
}


static FX950_GLOBAL_INTERRUPT_BUFFER  appDemoFx950GlobalIntGetStatusBuffer;

/*   FX950_INTERRUPT_GLOBAL_GET_STATUS
*      result_ptr - Pointer to buffer of type FX950_GLOBAL_INTERRUPT_BUFFER 
*      with read data (status of global interrupts).
*      param1 - bitmap of interrupts to be handled: 
*               1 - interrupt will be handled and application's callback will be 
*               called if interrupt occurs. 
*               0 - interrupt will not be handled. 
*      For example if just FX950_GLOBAL_INT_TWSI_TIME_OUT and 
*      FX950_GLOBAL_INT_PEX_ADDR_UNMAPPED interrupts need to be handled, then 
*      interrupt_bitmap will have the following value:
*      (1 << FX950_GLOBAL_INT_TWSI_TIME_OUT) | (1 << FX950_GLOBAL_INT_PEX_ADDR_UNMAPPED).
*/
#define FX950_GLOBAL_INT (                              \
    ( 1 << FX950_GLOBAL_INT_MISC_TWSI_TIME_OUT)|        \
    ( 1 << FX950_GLOBAL_INT_MISC_TWSI_STATUS)|          \
    ( 1 << FX950_GLOBAL_INT_MISC_PEX_ADDR_UNMAPPED)|    \
    ( 1 << FX950_GLOBAL_INT_BM_AGED_PACKET)|            \
    ( 1 << FX950_GLOBAL_INT_BM_TRIGGERED_AGING_DONE)    \
    )

/*********************************************************************/
int appDemoFx950GlobalIntGetStatusCallback(
    unsigned long   user_callback_id,  
    unsigned long  *bufferPtr, 
    unsigned long **newBufferPtrPtr,
    unsigned long   sizeOfBuffer,
    unsigned long   errorCode, 
    unsigned long   deviceId,
    unsigned long   polling_transaction_id, 
    unsigned long   param_01_not_used, 
    unsigned long   param_02_not_used
    )
{
    int             ii;
    FX950_GLOBAL_INTERRUPT_BUFFER * globalIntStatusPtr = (FX950_GLOBAL_INTERRUPT_BUFFER *) bufferPtr;

    /* check buffer size */
    if(sizeof(FX950_GLOBAL_INTERRUPT_BUFFER) != sizeOfBuffer)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950GlobalIntGetStatusCallback: bad buffer size %d need %d:\n\r", 
                   sizeOfBuffer, sizeof(FX950_GLOBAL_INTERRUPT_BUFFER)));
        return 1;
    }

    /* check pointer */
    if((&appDemoFx950GlobalIntGetStatusBuffer) != globalIntStatusPtr)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950GlobalIntGetStatusCallback: bad buffer pointer 0x%X wait 0x%X:\n\r", 
                  globalIntStatusPtr, &appDemoFx950GlobalIntGetStatusBuffer));
        return 1;
    }

    /* use same buffer */
    *newBufferPtrPtr = 0;

    INT_DBG_INFO(("  + appDemoFx950GlobalIntGetStatusCallback: \n\r"));

    for(ii = 0 ; ii < FX950_GLOBAL_NOF_INT; ii++)
    {
        INT_DBG_INFO(("  + status %d:\n\r", globalIntStatusPtr->interrupt_status[ii]));

        
        /* check if an interrupt occured */
        if (globalIntStatusPtr->interrupt_status[ii]) 
        {
            /* update Galtis total interrupt counter db */
            if(fx950EventIncrementFunc != NULL)
            {
                /* don't care about the return value */
                (void)fx950EventIncrementFunc(deviceId, (APPDEMO_FX950_GLOBAL_FIRST_CNTR_CNS + ii));
            }
        }
    }

    return 0;
}



static FX950_PEX_INTERRUPT_BUFFER  appDemoFx950PexIntGetStatusBuffer;

/* FX950_INTERRUPT_PEX_GET_STATUS
*      result_ptr - Pointer to buffer of type FX950_PEX_INTERRUPT_BUFFER 
*      with read data (status of PEX interrupts).
*      param1 - bitmap of interrupts to be handled: 
*               1 - interrupt will be handled and application's callback will be 
*               called if interrupt occurs. 
*               0 - interrupt will not be handled. 
*      For example if just FX950_PEX_INT_M_DIS and FX950_PEX_INT_F_ERR_DET 
*      interrupts need to be handled, then interrupt_bitmap will have the 
*      following value: (1 << FX950_PEX_INT_M_DIS) | (1 << FX950_PEX_INT_F_ERR_DET).
*/
#define FX950_PEX_INT (                         \
    ( 1 << FX950_PEX_INT_DI_DWN_TX_ACC_ERR) |   \
    ( 1 << FX950_PEX_INT_M_DIS) |               \
    ( 1 << FX950_PEX_INT_ERR_WR_TO_REG) |       \
    ( 1 << FX950_PEX_INT_HIT_DFLT_WIN_ERR) |    \
    ( 1 << FX950_PEX_INT_NF_ERR_DET) |          \
    ( 1 << FX950_PEX_INT_F_ERR_DET) |           \
    ( 1 << FX950_PEX_INT_DSTATE_CHANGE) |       \
    ( 1 << FX950_PEX_INT_BIST) |                \
    ( 1 << FX950_PEX_INT_RCV_CRS) |             \
    ( 1 << FX950_PEX_INT_PEX_SLV_HOT_RESET) |   \
    ( 1 << FX950_PEX_INT_PEX_SLV_DIS_LINK) |    \
    ( 1 << FX950_PEX_INT_PEX_SLV_LB) |          \
    ( 1 << FX950_PEX_INT_PEX_LINK_FAIL)         \
    )
    


/*********************************************************************/
int appDemoFx950PexIntGetStatusCallback(
    unsigned long   user_callback_id,  
    unsigned long  *bufferPtr, 
    unsigned long **newBufferPtrPtr,
    unsigned long   sizeOfBuffer,
    unsigned long   errorCode, 
    unsigned long   deviceId,
    unsigned long   polling_transaction_id, 
    unsigned long   param_01_not_used, 
    unsigned long   param_02_not_used
    )
{
    int             ii;
    FX950_PEX_INTERRUPT_BUFFER * pexIntStatusPtr = (FX950_PEX_INTERRUPT_BUFFER *) bufferPtr;

    /* check buffer size */
    if(sizeof(FX950_PEX_INTERRUPT_BUFFER) != sizeOfBuffer)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950PexIntGetStatusCallback: bad buffer size %d need %d:\n\r", 
                   sizeOfBuffer, sizeof(FX950_PEX_INTERRUPT_BUFFER)));
        return 1;
    }

    /* check pointer */
    if((&appDemoFx950PexIntGetStatusBuffer) != pexIntStatusPtr)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950PexIntGetStatusCallback: bad buffer pointer 0x%X wait 0x%X:\n\r", 
                  pexIntStatusPtr, &appDemoFx950PexIntGetStatusBuffer));
        return 1;
    }

    /* use same buffer */
    *newBufferPtrPtr = 0;

    INT_DBG_INFO(("  + appDemoFx950PexIntGetStatusCallback: \n\r"));

    for(ii = 0 ; ii < FX950_PEX_NOF_INT; ii++)
    {
        INT_DBG_INFO(("  + status %d:\n\r", pexIntStatusPtr->interrupt_status[ii]));

        
        /* check if an interrupt occured */
        if (pexIntStatusPtr->interrupt_status[ii]) 
        {
            /* update Galtis total interrupt counter db */
            if(fx950EventIncrementFunc != NULL)
            {
                /* don't care about the return value */
                (void)fx950EventIncrementFunc(deviceId, (APPDEMO_FX950_PEX_FIRST_CNTR_CNS + ii));
            }
        }
    }

    return 0;
}

static FX950_PORT_INTERRUPT_BUFFER  appDemoFx950PortIntGetStatusBuffer;
/*
*   FX950_INTERRUPT_PORT_GET_STATUS
*      result_ptr - Pointer to buffer of type FX950_PORT_INTERRUPT_BUFFER 
*      with read data (status of port interrupts).
*      param1 - HyperG. Link/ HyperG. Stack port number (0..1)
*      param2 - bitmap of interrupts to be handled: 
*               1 - interrupt will be handled and application's callback will be 
*               called if interrupt occurs. 
*               0 - interrupt will not be handled. 
*      For example if just FX950_PORT_INT_HGL_PING_RECEIVED and 
*      FX950_PORT_INT_HGL_PING_SENT interrupts need to be handled, then 
*      interrupt_bitmap will have the following value:
*      (1 << FX950_PORT_INT_HGL_PING_RECEIVED) | (1 << FX950_PORT_INT_HGL_PING_SENT).
*/
#define FX950_PORT_INT (                                    \
    ( 1 << FX950_PORT_INT_HGL_PING_RECEIVED) |              \
    ( 1 << FX950_PORT_INT_HGL_PING_SENT) |                  \
    ( 1 << FX950_PORT_INT_HGL_RX_ERR_CNT_RL) |              \
    ( 1 << FX950_PORT_INT_HGL_BAD_PCS_2_MAC_REFORM_RX) |    \
    ( 1 << FX950_PORT_INT_HGL_BAD_HEAD_CELL_RX)     |       \
    ( 1 << FX950_PORT_INT_HGL_BAD_LENGTH_CELL_RX)   |       \
    ( 1 << FX950_PORT_INT_HGL_FC_XOFF_DEADLOCK)     |       \
    ( 1 << FX950_PORT_INT_HGL_ADDR_OUT_OF_RANGE)    |       \
    ( 1 << FX950_PORT_INT_XPCS_LINK_STATUS_CHANGE)  |       \
    ( 1 << FX950_PORT_INT_XPCS_DESCEW_ERR)                  \
    )

/*********************************************************************/
int appDemoFx950PortIntGetStatusCallback(
    unsigned long   user_callback_id, /* port number */ 
    unsigned long  *bufferPtr, 
    unsigned long **newBufferPtrPtr,
    unsigned long   sizeOfBuffer,
    unsigned long   errorCode, 
    unsigned long   deviceId,
    unsigned long   polling_transaction_id, 
    unsigned long   param_01_not_used, 
    unsigned long   param_02_not_used
    )
{
    unsigned long   port        = user_callback_id;
    int             ii;
    int             index;
    FX950_PORT_INTERRUPT_BUFFER * portIntStatusPtr = (FX950_PORT_INTERRUPT_BUFFER *) bufferPtr;

    /* check buffer size */
    if(sizeof(FX950_PORT_INTERRUPT_BUFFER) != sizeOfBuffer)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950PortIntGetStatusCallback: bad buffer size %d need %d:\n\r", 
                   sizeOfBuffer, sizeof(FX950_PORT_INTERRUPT_BUFFER)));
        return 1;
    }

    /* check pointer */
    if((&appDemoFx950PortIntGetStatusBuffer) != portIntStatusPtr)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950PortIntGetStatusCallback: bad buffer pointer 0x%X wait 0x%X:\n\r", 
                  portIntStatusPtr, &appDemoFx950PortIntGetStatusBuffer));
        return 1;
    }

    /* use same buffer */
    *newBufferPtrPtr = 0;

    INT_DBG_INFO(("  + appDemoFx950PortIntGetStatusCallback: port %d:\n\r", port));

    for(ii = 0 ; ii < FX950_PORT_NOF_INT; ii++)
    {
        INT_DBG_INFO(("  + port %d status %d:\n\r", port, portIntStatusPtr->interrupt_status[ii]));

        
        /* check if an interrupt occured */
        if (portIntStatusPtr->interrupt_status[ii]) 
        {
            index = (APPDEMO_FX950_PORT_FIRST_CNTR_CNS + (FX950_PORT_NOF_INT * port) + ii);

            /* update Galtis total interrupt counter db */
            if(fx950EventIncrementFunc != NULL)
            {
                /* don't care about the return value */
                (void)fx950EventIncrementFunc(deviceId, index);
            }
        }
    }


    return 0;
}


static FX950_PORT_LANE_INTERRUPT_BUFFER  appDemoFx950PortLaneIntGetStatusBuffer;
/*
*   FX950_INTERRUPT_PORT_LANE_GET_STATUS
*      result_ptr - Pointer to buffer of type FX950_PORT_LANE_INTERRUPT_BUFFER 
*      with read data (status of port lane interrupts).
*      param1 - HyperG. Link/ HyperG. Stack port number (0..1)
*      param2 - Lane number (0..5)
*      param3 - bitmap of interrupts to be handled: 
*               1 - interrupt will be handled and application's callback will be 
*               called if interrupt occurs. 
*               0 - interrupt will not be handled. 
*      For example if just FX950_PORT_LANE_INT_PRBS_ERR and 
*      FX950_PORT_LANE_INT_DETECTED_A 
*      interrupts need to be handled, then interrupt_bitmap will have the 
*      following value: (1 << FX950_PORT_LANE_INT_PRBS_ERR) | 
*      (1 << FX950_PORT_LANE_INT_DETECTED_A).
*/
#define FX950_PORT_LANE_INT (                           \
    (1 << FX950_PORT_LANE_INT_PRBS_ERR) |               \
    ( 1 << FX950_PORT_LANE_INT_DISPARITY_ERR) |         \
    ( 1 << FX950_PORT_LANE_INT_SYMBOL_ERR) |            \
    ( 1 << FX950_PORT_LANE_INT_CJR_PAT_ERR) |           \
    ( 1 << FX950_PORT_LANE_INT_SIGNAL_DETECT_CHANGE) |  \
    ( 1 << FX950_PORT_LANE_INT_SYNC_STATUS_CHANGE)      \
    )

/*********************************************************************/
int appDemoFx950PortLaneIntGetStatusCallback(
    unsigned long   user_callback_id, /* port number and lane number */ 
    unsigned long  *bufferPtr, 
    unsigned long **newBufferPtrPtr,
    unsigned long   sizeOfBuffer,
    unsigned long   errorCode, 
    unsigned long   deviceId,
    unsigned long   polling_transaction_id, 
    unsigned long   param_01_not_used, 
    unsigned long   param_02_not_used
    )
{
    unsigned long   port        = (user_callback_id & 0x1);
    unsigned long   lane        = ((user_callback_id >> 1) & 0x7);
    int             ii;
    int             index;
    FX950_PORT_LANE_INTERRUPT_BUFFER * portLaneIntStatusPtr = 
        (FX950_PORT_LANE_INTERRUPT_BUFFER *) bufferPtr;

    /* check buffer size */
    if(sizeof(FX950_PORT_LANE_INTERRUPT_BUFFER) != sizeOfBuffer)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950PortLaneIntGetStatusCallback: bad buffer size %d need %d:\n\r", 
                   sizeOfBuffer, sizeof(FX950_PORT_LANE_INTERRUPT_BUFFER)));
        return 1;
    }

    /* check pointer */
    if((&appDemoFx950PortLaneIntGetStatusBuffer) != portLaneIntStatusPtr)
    {
        /* error */
        INT_DBG_INFO(("  + appDemoFx950PortLaneIntGetStatusCallback: bad buffer pointer 0x%X wait 0x%X:\n\r", 
                  portLaneIntStatusPtr, &appDemoFx950PortLaneIntGetStatusBuffer));
        return 1;
    }

    /* use same buffer */
    *newBufferPtrPtr = 0;

    INT_DBG_INFO(("  + appDemoFx950PortLaneIntGetStatusCallback: port %d: lane %d: \n\r", port, lane));

    for(ii = 0 ; ii < FX950_PORT_LANE_NOF_INT; ii++)
    {
        INT_DBG_INFO(("  + port %d lane %d status %d:\n\r", port, lane, 
                      portLaneIntStatusPtr->interrupt_status[ii]));

        
        /* check if an interrupt occured */
        if (portLaneIntStatusPtr->interrupt_status[ii]) 
        {
            index = (APPDEMO_FX950_PORT_LANE_FIRST_CNTR_CNS + 
                     (FX950_PORT_LANE_NOF_INT * port * lane) + ii);

            /* update Galtis total interrupt counter db */
            if(fx950EventIncrementFunc != NULL)
            {
                /* don't care about the return value */
                (void)fx950EventIncrementFunc(deviceId, index);
            }
        }
    }

    return 0;
}


/*******************************************************************************
* appDemoFx950Isr
*
* DESCRIPTION:
*       FX950 ISR
*
* INPUTS:
*       deviceId  - device Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None
*
* COMMENTS:
*
*
*******************************************************************************/
void
  appDemoFx950Isr(
    unsigned int  device_id
  )
{
    ISR_DUMP((" appDemoFx950Isr called dev %d\n", device_id));
    appDemoFx950IsrCount++;
    fap20m_interrupt_handler(device_id);
}
/*******************************************************************************
* appDemoFap20mEnableInterrupts
*
* DESCRIPTION:
*       Bind interrupt handler to OS and enable specific interrupt
*
* INPUTS:
*       deviceId  - device Id.
*       intVecNum - interrupt vector number to attach ISR to
*       intMask   - interrupt bits to enable (unmask) interrupts
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS appDemoFap20mEnableInterrupts
(
    IN unsigned int    deviceId,
    IN GT_U32          intVecNum,
    IN GT_U32          intMask
)
{
    GT_STATUS rc ;

    /* Connect the ISR to the interrupt vector. */
    rc = extDrvIntConnect(intVecNum, (GT_VOIDFUNCPTR)appDemoFx950Isr, deviceId);
    if (rc != GT_OK) 
    {
        DBG_INFO(("  + appDemoFap20mEnableInterrupts: ISR connection fail to vector %d\n\r", intVecNum));
        return rc;
    }

    /* enable interrupt.    */
    rc = extDrvIntEnable(intMask);
    if (rc != GT_OK) 
    {
        DBG_INFO(("  + appDemoFap20mEnableInterrupts: IRQ enable fail for mask %d\n\r", intMask));
        return rc;
    }
    return rc;
}

/*******************************************************************************
* appDemoFap20mInitInterrupts
*
* DESCRIPTION:
*       Init interrupts part of appdemo
*
* INPUTS:
*       deviceId      - device Id.
*       intVecNum - interrupt vector number to attach ISR to
*       intMask   - interrupt bits to enable (unmask) interrupts
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFap20mInitInterrupts
(
    IN unsigned int    deviceId,
    IN GT_U32          intVecNum,
    IN GT_U32          intMask
)
{
    unsigned long       ret;                    /* return code */
    SAND_RET            sand_ret;               /* SAND return code */
    GT_STATUS           rc;                     /* CPSS return code */
    SAND_USER_CALLBACK  user_callback_structure;
    unsigned long       callback_id; 
    unsigned int        port_i;
    unsigned int        lane_i;
    unsigned int        i;

    /* check deviceId validity */
    if (deviceId >= SAND_MAX_DEVICE) 
    {
        cpssOsPrintf(("  + appDemoFap20mInitInterrupts: Device number is wrong\n\r"));
        return GT_FAIL;
    }

    /* register FAP20M Cell RX callback */
    user_callback_structure.interrupt_not_poll = TRUE;
    user_callback_structure.callback_func = appDemoFap20mRxCellCallback;
    user_callback_structure.result_ptr    = (unsigned long *)&appDemoFap20mRxCellBuffer;
    user_callback_structure.user_callback_id = 0;

    ret =  fap20m_register_callback_function(deviceId,
                                             FAP20M_RECV_SR_DATA_CELL,
                                             &user_callback_structure,
                                             &callback_id);

    sand_ret = sand_get_error_code_from_error_word(ret);
    if (sand_ret != SAND_OK)
    {
        DBG_INFO(("  + appDemoFap20mInitInterrupts: RX Cell registration fail 0x%X:\n\r", ret));
        return GT_FAIL;
    }

    DBG_INFO(("  + appDemoFap20mInitInterrupts: RX Cell registration done, CB ID 0x%X\n\r", callback_id));

    /* register FAP20M QDR errors accumulation callback */
    user_callback_structure.interrupt_not_poll = TRUE;
    user_callback_structure.callback_func = appDemoFap20mQdrGetNumSftErrReportCallback;
    user_callback_structure.result_ptr    = (unsigned long *)&appDemoFap20mQdrGetNumSftErrReportBuffer;
    user_callback_structure.user_callback_id = 0;

    ret =  fap20m_register_callback_function(deviceId,
                                             FAP20M_QDR_GET_NUM_SFT_ERR,
                                             &user_callback_structure,
                                             &callback_id);

    sand_ret = sand_get_error_code_from_error_word(ret);
    if (sand_ret != SAND_OK)
    {
        DBG_INFO(("  + appDemoFap20mInitInterrupts: QDR Error Accumulation registration fail 0x%X:\n\r", ret));
        return GT_FAIL;
    }

    DBG_INFO(("  + appDemoFap20mInitInterrupts: QDR Error Accumulation registration done, CB ID 0x%X\n\r", callback_id));


    /* initialize Fap20m link status db */
    for (i = 0; i < FAP20M_NUMBER_OF_LINKS; i++) 
    {
        appDemoFap20mLinkStatusLastResultDb[i] = 0;
    }
    
    /* register FE link status calback */
    user_callback_structure.interrupt_not_poll = TRUE;
    user_callback_structure.callback_func = appDemoFap20mLinkStatusCallback;
    user_callback_structure.result_ptr    = (unsigned long *)&appDemoFap20mLinkStatusBuffer;
    user_callback_structure.user_callback_id = 0;
    user_callback_structure.param1 = 0xFFFFFF; /* list of links */
    
    ret =  fap20m_register_callback_function(deviceId,
                                             FAP20M_GET_LINKS_STATUS,
                                             &user_callback_structure,
                                             &callback_id);

    sand_ret = sand_get_error_code_from_error_word(ret);
    if (sand_ret != SAND_OK)
    {
        DBG_INFO(("  + appDemoFap20mInitInterrupts: Link status registration fail 0x%X:\n\r", ret));
        return GT_FAIL;
    }

    DBG_INFO(("  + appDemoFap20mInitInterrupts: Link Status registration done, CB ID 0x%X\n\r", callback_id));


    /* register Fx950 Global interrupts callback */
    user_callback_structure.interrupt_not_poll = TRUE;
    user_callback_structure.callback_func = appDemoFx950GlobalIntGetStatusCallback;
    user_callback_structure.result_ptr    = (unsigned long *)&appDemoFx950GlobalIntGetStatusBuffer;
    user_callback_structure.user_callback_id = 0;
    user_callback_structure.param1 = FX950_GLOBAL_INT;

    ret =  fap20m_register_callback_function(deviceId,
                                             FX950_INTERRUPT_GLOBAL_GET_STATUS,
                                             &user_callback_structure,
                                             &callback_id);

    sand_ret = sand_get_error_code_from_error_word(ret);
    if (sand_ret != SAND_OK)
    {
        DBG_INFO(("  + appDemoFap20mInitInterrupts: FX950 Global registration fail 0x%X:\n\r", ret));
        return GT_FAIL;
    }

    DBG_INFO(("  + appDemoFap20mInitInterrupts: FX950 Global registration done, CB ID 0x%X\n\r", callback_id));


    /* register Fx950 PEX interrupts callback */
    user_callback_structure.interrupt_not_poll = TRUE;
    user_callback_structure.callback_func = appDemoFx950PexIntGetStatusCallback;
    user_callback_structure.result_ptr    = (unsigned long *)&appDemoFx950PexIntGetStatusBuffer;
    user_callback_structure.user_callback_id = 0;
    user_callback_structure.param1 = FX950_PEX_INT;

    ret =  fap20m_register_callback_function(deviceId,
                                             FX950_INTERRUPT_PEX_GET_STATUS,
                                             &user_callback_structure,
                                             &callback_id);

    sand_ret = sand_get_error_code_from_error_word(ret);
    if (sand_ret != SAND_OK)
    {
        DBG_INFO(("  + appDemoFap20mInitInterrupts: FX950 PEX registration fail 0x%X:\n\r", ret));
        return GT_FAIL;
    }

    DBG_INFO(("  + appDemoFap20mInitInterrupts: FX950 PEX registration done, CB ID 0x%X\n\r", callback_id));

    /* register FX950 HGL port/lane callbacks */
    for (port_i = 0; port_i < FX950_NOF_HGS_LINKS; port_i++) 
    {
        /* register HGL port interrupt status callback */
        user_callback_structure.interrupt_not_poll = TRUE;
        user_callback_structure.callback_func = appDemoFx950PortIntGetStatusCallback;
        user_callback_structure.result_ptr    = (unsigned long *)&appDemoFx950PortIntGetStatusBuffer;
        user_callback_structure.user_callback_id = port_i;
        user_callback_structure.param1 = port_i; /* HGL/HGS port number */
        user_callback_structure.param2 = FX950_PORT_INT; /* bitmap of enabled interrupts*/

        ret =  fap20m_register_callback_function(deviceId,
                                                 FX950_INTERRUPT_PORT_GET_STATUS,
                                                 &user_callback_structure,
                                                 &callback_id);

        sand_ret = sand_get_error_code_from_error_word(ret);
        if (sand_ret != SAND_OK)
        {
            DBG_INFO(("  + appDemoFap20mInitInterrupts: HGL Port %d status registration fail 0x%X:\n\r", port_i, ret));
            return GT_FAIL;
        }

        DBG_INFO(("  + appDemoFap20mInitInterrupts: HGL Port %d Status registration done, CB ID 0x%X\n\r", port_i, callback_id));


        for (lane_i = 0; lane_i < FX950_REGS_SERDES_LANES_NUM; lane_i++) 
        {
            /* register HGL Port Lane interrupt status callback */
            user_callback_structure.interrupt_not_poll = TRUE;
            user_callback_structure.callback_func = appDemoFx950PortIntGetStatusCallback;
            user_callback_structure.result_ptr    = (unsigned long *)&appDemoFx950PortIntGetStatusBuffer;
            user_callback_structure.user_callback_id = (port_i | (lane_i << 1));
            user_callback_structure.param1 = port_i; /* HGL/HGS port number */
            user_callback_structure.param2 = lane_i; /* lane number */
            user_callback_structure.param3 = FX950_PORT_LANE_INT; /* bitmap of enabled interrupts*/

            ret =  fap20m_register_callback_function(deviceId,
                                                     FX950_INTERRUPT_PORT_LANE_GET_STATUS,
                                                     &user_callback_structure,
                                                     &callback_id);

            sand_ret = sand_get_error_code_from_error_word(ret);
            if (sand_ret != SAND_OK)
            {
                DBG_INFO(("  + appDemoFap20mInitInterrupts: HGL Port %d Lane %d status registration fail 0x%X:\n\r", port_i, lane_i, ret));
                return GT_FAIL;
            }

            DBG_INFO(("  + appDemoFap20mInitInterrupts: HGL Port %d Lane %d Status registration done, CB ID 0x%X\n\r", port_i, lane_i, callback_id));
        }
    }
    
#ifdef IMPL_GALTIS
    /* Bind Galtis increment routine to AppDemo. */
    wrFx950EventCountersCbGet(&fx950EventIncrementFunc);
#endif

    sand_tcm_set_enable_flag(TRUE);

    /* connect ISR and enable IRQ */
    rc = appDemoFap20mEnableInterrupts(deviceId, intVecNum, intMask);

    return rc;

}

int appDemoFap20DumpIntMask( unsigned int device_id)
{
    unsigned short rc;
    unsigned long stream[8];

    osMemSet(stream, 0, sizeof(stream));

    rc = fap20m_get_interrupts_mask(device_id, stream);

    DBG_INFO(("0x%8X 0x%8X 0x%8X 0x%8X\n\r", stream[0], stream[1],stream[2],stream[3]));
    DBG_INFO(("0x%8X 0x%8X 0x%8X 0x%8X\n\r", stream[4], stream[5],stream[6],stream[7]));

    return rc;
}


