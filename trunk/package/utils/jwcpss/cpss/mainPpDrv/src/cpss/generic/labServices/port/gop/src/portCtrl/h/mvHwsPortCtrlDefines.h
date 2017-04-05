/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsPortCtrlDefines.h
*
* DESCRIPTION:
*       Port Control Definitions
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsPortCtrlDefines_H
#define __mvHwsPortCtrlDefines_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define MV_PORT_CTRL_DEBUG
#ifdef MV_PORT_CTRL_DEBUG
#define mvPcPrintf(str...) osPrintf(str)
#else
#define mvPcPrintf(str...)
#endif

/* Global definitions */
/* ================== */

#if defined BC2_DEV_SUPPORT

#define LOAD_ADDR       (0xFFF80000)
#define SHMEM_BASE      (0xFFF40000)
#define SHMEM_SIZE      (0x40000)

#define HWS_IPC_LINK_ID (IPC_SCPU_FREERTOS_LINK_ID)

#elif defined BOBK_DEV_SUPPORT

#define LOAD_ADDR       (0x0)
#define SHMEM_BASE      (0x1F800)
#define SHMEM_SIZE      (0x800)

#define HWS_IPC_LINK_ID (IPC_CM3_FREERTOS_LINK_ID)

#else

#pragma message "Global Defintion - Load Address, Shared Memory Address / Size not defined"

#endif

/* BobK Device Definition */
/* ====================== */
/* Max number of supported ports */
#define MV_PORT_CTRL_MAX_PORT_NUM        (72)

/* Max number of Executing processes */
#define MV_PORT_CTRL_MAX_PROCESS_NUM     (2)

/* Max number of pending messages per port */
#define MV_PORT_CTRL_PORT_PEND_MSG_NUM   (3)

#define MV_PORT_CTRL_NUM_OF_QUEUE_MSGS   (6)

/* Max number of messages enter execution by the Supervisor */
#define MV_PORT_CTRL_SPV_MSG_EXEC_THRESHOLD (MV_PORT_CTRL_NUM_OF_QUEUE_MSGS)

/* Max number of pending messages per port */
/* 512 entry's x 8 bytes = 4KBytes*/
#define MV_PORT_CTRL_LOG_SIZE            (512)

/* Port Control port number definition - starting 0 */
#define MV_PORT_CTRL_NUM(port)           (0x1 << port)

/* Port Control port number is currently not executing */
#define MV_PORT_CTRL_PORT_NOT_EXECUTING  (0xFF)

/* Port Control training duration */
#define PORT_CTRL_TRAINING_DURATION      (5)
#define PORT_CTRL_TRAINING_INTERVAL      (100)

/* Port Control system config duration */
#define PORT_CTRL_SYSTEM_CONFIG_DURATION (10)

#define PORT_CTRL_TIMER_DEFAULT          (0)

/* Port Control Avago GUI flag */
#define MV_PORT_CTRL_AVAGO_GUI_ENABLED   (0x1)
#define MV_PORT_CTRL_AVAGO_GUI_MASK      (0x1)

/* Port Control port type definition */
typedef enum
{
    REG_PORT = 0,
    AP_PORT  = 1

}MV_PORT_CTRL_TYPE;

typedef enum
{
    INIT_PORT  = 0,
    RESET_PORT = 1

}MV_PORT_CTRL_ACTION;


/* Supervisor module (M1) definition */
/* ================================= */

/* Supervisor module (M1) state definition */
typedef enum
{
    SPV_HIGH_MSG_PROCESS_STATE = 0,
    SPV_LOW_MSG_PROCESS_STATE  = 1,
    SPV_DELAY_STATE            = 2,
    SPV_MAX_STATE              = 3

}MV_SUPERVISOR_STATE;

typedef enum
{
    SPV_HIGH_MSG_EVENT = 0,
    SPV_LOW_MSG_EVENT  = 1,
    SPV_DELAY_EVENT    = 2,
    SPV_MAX_EVENT      = 3

}MV_SUPERVISOR_EVENT;


/* Port Management module (M2) definition */
/* ====================================== */

/* Port Management module (M2) state definition */
typedef enum
{
    PORT_MNG_MSG_PROCESS_STATE = 0,
    PORT_MNG_DELAY_STATE       = 1,
    PORT_MNG_MAX_STATE         = 2 

}MV_PORT_MNG_STATE;

typedef enum
{
    PORT_MNG_MSG_PROCESS_EVENT = 0,
    PORT_MNG_DELAY_EVENT       = 1,
    PORT_MNG_MAX_EVENT         = 2

}MV_PORT_MNG_EVENT;

/* Port State Machine module (M3) definition */
/* ========================================= */

/* Port State Machine module (M3) state definition */
typedef enum
{
    PORT_SM_IDLE_STATE            = 0,
    PORT_SM_SERDES_CONFIG_STATE   = 1,
    PORT_SM_SERDES_TRAINING_STATE = 2,
    PORT_SM_MAC_PCS_CONFIG_STATE  = 3,
    PORT_SM_ACTIVE_STATE          = 4,
    PORT_SM_MAX_STATE             = 5

}MV_PORT_SM_STATE;

/* Port State Machine module (M3) status definition */
typedef enum
{
    PORT_SM_START_EXECUTE                        = 1,
    PORT_SM_SERDES_CONFIG_IN_PROGRESS            = 2,
    PORT_SM_SERDES_CONFIG_SUCCESS                = 3,
    PORT_SM_SERDES_CONFIG_FAILURE                = 4,
    PORT_SM_SERDES_TRAINING_IN_PROGRESS          = 5,
    PORT_SM_SERDES_TRAINING_SUCCESS              = 6,
    PORT_SM_SERDES_TRAINING_FAILURE              = 7,
    PORT_SM_SERDES_TRAINING_CONFIG_FAILURE       = 8,
    PORT_SM_SERDES_TRAINING_END_WAIT_FOR_SYS_ACK = 9,
    PORT_SM_MAC_PCS_CONFIG_IN_PROGRESS           = 10,
    PORT_SM_MAC_PCS_CONFIG_SUCCESS               = 11,
    PORT_SM_MAC_PCS_CONFIG_FAILURE               = 12,
    PORT_SM_DELETE_IN_PROGRESS                   = 13,
    PORT_SM_DELETE_SUCCESS                       = 14,
    PORT_SM_DELETE_FAILURE                       = 15,
    PORT_SM_FEC_CONFIG_IN_PROGRESS               = 16,
    PORT_SM_FEC_CONFIG_SUCCESS                   = 17,
    PORT_SM_FEC_CONFIG_FAILURE                   = 18,
    PORT_SM_FC_STATE_SET_IN_PROGRESS             = 19,
    PORT_SM_FC_STATE_SET_SUCCESS                 = 20,
    PORT_SM_FC_STATE_SET_FAILURE                 = 21,
    PORT_SM_MAX_STATUS                           = 22

}MV_PORT_SM_STATUS;

/* Port State Machine module (M3) system config ("PIzza Arbiter") definition */
typedef enum
{
    PORT_SM_SERDES_SYSTEM_ENABLE  = 0x01,
    PORT_SM_SERDES_SYSTEM_DISABLE = 0x00

}MV_PORT_SM_SYS_CONFIG;

/* Port State Machine module (M3) system state ("PIzza Arbiter") definition */
typedef enum
{
    PORT_SM_SERDES_SYSTEM_VALID     = 0x01,
    PORT_SM_SERDES_SYSTEM_NOT_VALID = 0x00

}MV_PORT_SM_SYS_STATE;

/* AP Port Management module (M4) definition */
/* ========================================= */

/* AP Port Management module (M4) state definition */
typedef enum
{
    AP_PORT_MNG_ACTIVE_PORT_STATE = 1,
    AP_PORT_MNG_MSG_PROCESS_STATE = 2,
    AP_PORT_MNG_PORT_DELAY_STATE  = 3,
    AP_PORT_MNG_MAX_STATE         = 4

}MV_AP_PORT_MNG_STATE;

typedef enum
{
    AP_PORT_MNG_ACTIVE_PORT_EVENT = 0,
    AP_PORT_MNG_MSG_PROCESS_EVENT = 1,
    AP_PORT_MNG_DELAY_EVENT       = 2,
    AP_PORT_MNG_MAX_EVENT         = 3

}MV_AP_PORT_MNG_EVENT;

/* AP Port Management module (M4) status definition */
typedef enum
{
    AP_PORT_MNG_POWER_UP_START = 1,
    AP_PORT_MNG_POWER_UP_STOP  = 2

}MV_AP_PORT_MNG_STATUS;


/* AP Port State Machine module (M5) definition */
/* ============================================ */

/* AP Port Detect module (M5) state definition */
typedef enum
{
    AP_PORT_DETECT_ACTIVE_PORT_STATE = 1,
    AP_PORT_DETECT_MSG_PROCESS_STATE = 2,
    AP_PORT_DETECT_DELAY_STATE       = 3,
    AP_PORT_DETECT_MAX_STATE         = 4

}MV_AP_PORT_DET_STATE;

/* AP Port Detect module (M5) event definition */
typedef enum
{
    AP_PORT_DETECT_ACTIVE_PORT_EVENT = 0,
    AP_PORT_DETECT_MSG_PROCESS_EVENT = 1,
    AP_PORT_DETECT_DELAY_EVENT       = 2,
    AP_PORT_DETECT_MAX_EVENT         = 3

}MV_AP_PORT_DET_EVENT;

/* AP Port State Machine module (M5) state definition */
typedef enum
{
    AP_PORT_SM_IDLE_STATE       = 0,
    AP_PORT_SM_INIT_STATE       = 1,
    AP_PORT_SM_TX_DISABLE_STATE = 2,
    AP_PORT_SM_RESOLUTION_STATE = 3,
    AP_PORT_SM_ACTIVE_STATE     = 4,
    AP_PORT_SM_DELETE_STATE     = 5,
    AP_PORT_SM_MAX_STATE        = 6

}MV_AP_PORT_SM_STATE;

/* AP Port State Machine module (M5) status definition */
typedef enum
{
    AP_PORT_START_EXECUTE                  = 0,
    AP_PORT_SERDES_INIT_FAILURE            = 1,
    AP_PORT_INIT_SUCCESS                   = 2,
    AP_PORT_TX_DISABLE_IN_PROGRESS         = 3,
    AP_PORT_TX_DISABLE_FAILURE             = 4,
    AP_PORT_TX_DISABLE_SUCCESS             = 5,
    AP_PORT_RESOLUTION_IN_PROGRESS         = 6,
    AP_PORT_PD_RESOLUTION_IN_PROGRESS      = 7,
    AP_PORT_PD_RESOLUTION_FAILURE          = 8,
    AP_PORT_AP_RESOLUTION_IN_PROGRESS      = 9,
    AP_PORT_AP_NO_RESOLUTION               = 10,
    AP_PORT_AP_RESOLUTION_TIMER_FAILURE    = 11,
    AP_PORT_AP_RESOLUTION_MAX_LOOP_FAILURE = 12,
    AP_PORT_RESOLUTION_SUCCESS             = 13,
    AP_PORT_LINK_CHECK_START               = 14,
    AP_PORT_LINK_CHECK_VALIDATE            = 15,
    AP_PORT_LINK_CHECK_FAILURE             = 16,
    AP_PORT_LINK_CHECK_SUCCESS             = 17,
    AP_PORT_DELETE_IN_PROGRESS             = 18,
    AP_PORT_DELETE_SUCCESS                 = 19,
    AP_PORT_MAX_STATUS                     = 20

}MV_AP_PORT_SM_STATUS;

/* AP Port State Machine module (M5) Resolution result definition */
typedef enum
{
    AP_PORT_RESOLUTION_NOT_RESOLVED = 1,
    AP_PORT_RESOLUTION_RESOLVED     = 2

}MV_AP_PORT_RESOLUTION_RESULT;

/* AP Port State Machine module (M5) Resolution type definition */
typedef enum
{
    AP_PORT_RESOLUTION_1000_BASE_KX = 0,
    AP_PORT_RESOLUTION_10G_BASE_KX4 = 1,
    AP_PORT_RESOLUTION_10G_BASE_R   = 2,
    AP_PORT_RESOLUTION_40G_BASE_R   = 3

}MV_AP_PORT_RESOLUTION_TYPE;

/* AP Port State Machine module (M5) FEC definition */
typedef enum
{
    AP_PORT_RESOLUTION_FEC_DIS = 0,
    AP_PORT_RESOLUTION_FEC_EN  = 1

}MV_AP_PORT_RESOLUTION_FEC;

/* AP Port State Machine module (M5) Signal Detect definition */
typedef enum
{
    AP_PORT_RESOLUTION_NO_SIGNAL_DETECT = 0,
    AP_PORT_RESOLUTION_SIGNAL_DETECT    = 1

}MV_AP_PORT_RESOLUTION_SIGNAL_DETECT;

/* AP Port State Machine module (M5) CDR Lock definition */
typedef enum
{
    AP_PORT_RESOLUTION_NO_CDR_LOCK = 0,
    AP_PORT_RESOLUTION_CDR_LOCK    = 1

}MV_AP_PORT_RESOLUTION_CDR_LOCK;

/* AP Port State Machine module (M5) PCS Lock definition */
typedef enum
{
    AP_PORT_RESOLUTION_NO_PCS_LOCK = 0,
    AP_PORT_RESOLUTION_PCS_LOCK    = 1

}MV_AP_PORT_RESOLUTION_PCS_LOCK;

/* AP Port State Machine module (M5) Rx Flow Control definition */
typedef enum
{
    AP_PORT_RESOLUTION_RX_FLOW_CTRL_DIS = 0,
    AP_PORT_RESOLUTION_RX_FLOW_CTRL_EN  = 1

}MV_AP_PORT_RESOLUTION_RX_FLOW_CTRL;

/* AP Port State Machine module (M5) Tx Flow Control definition */
typedef enum
{
    AP_PORT_RESOLUTION_TX_FLOW_CTRL_DIS = 0,
    AP_PORT_RESOLUTION_TX_FLOW_CTRL_EN  = 1

}MV_AP_PORT_RESOLUTION_TX_FLOW_CTRL;

/* AP Port State Machine module (M5) Introp definition */
typedef enum
{
    AP_PORT_INTROP_TX_DIS               = 0x01,
    AP_PORT_INTROP_ABILITY_DUR          = 0x02,
    AP_PORT_INTROP_ABILITY_MAX_INT      = 0x04,
    AP_PORT_INTROP_ABILITY_MAX_FAIL_INT = 0x08,
    AP_PORT_INTROP_AP_LINK_DUR          = 0x10,
    AP_PORT_INTROP_AP_LINK_MAX_INT      = 0x20,
    PD_PORT_INTROP_AP_LINK_DUR          = 0x40,
    PD_PORT_INTROP_AP_LINK_MAX_INT      = 0x80

}MV_AP_DETECT_ITEROP;

/* Port Control AP Tx Dis duration */
#define PORT_CTRL_AP_TX_DIS_DURATION        (60)

/* Port Control AP Ability Detect duration */
#define PORT_CTRL_AP_ABILITY_DURATION       (6)
#define PORT_CTRL_AP_ABILITY_MAX_COUNT      (21)
#define PORT_CTRL_AP_ABILITY_FAIL_MAX_COUNT (10)

/* Port Control Link Check duration */
#define PORT_CTRL_AP_LINK_CHECK_MODE        (0)
#define PORT_CTRL_AP_LINK_CHECK_DURATION    (10)
#define PORT_CTRL_AP_LINK_CHECK_MAX_COUNT   (50)
#define PORT_CTRL_PD_LINK_CHECK_MODE        (1)
#define PORT_CTRL_PD_LINK_CHECK_DURATION    (25)
#define PORT_CTRL_PD_LINK_CHECK_MAX_COUNT   (10)

/* Port General module (M6) definition */
/* =================================== */
typedef enum
{
    PORT_GEN_MSG_PROCESS_STATE = 0,
    PORT_GEN_DELAY_STATE       = 1,
    PORT_GEN_MAX_STATE         = 2

}MV_PORT_GENERAL_STATE;

typedef enum
{
    PORT_GEN_MSG_EVENT   = 0,
    PORT_GEN_DELAY_EVENT = 1,
    PORT_GEN_MAX_EVENT   = 2

}MV_PORT_GENERAL_EVENT;

#define MV_GEN_LINK_DOWN (0x0)
#define MV_GEN_LINK_UP   (0x1)
#define MV_GEN_TUNE_FAIL (0x0)
#define MV_GEN_TUNE_PASS (0x1)

/* Global OS definitions */
/* ===================== */
#define MV_PROCESS_MSG_TX_DELAY    (0) /* msec */
#define MV_PROCESS_MSG_RX_DELAY    (1) /* msec */
#define MV_PROCESS_MSG_RX_NO_DELAY (0)

#define MV_GENERAL_STACK           (512) /* words - 1Kbytes */
#define MV_GENERAL_PRIORITY        (2)   /* 2 - Medium priority */
#define MV_GENERAL_MSG_NUM         (MV_PORT_CTRL_NUM_OF_QUEUE_MSGS)
#define MV_GENERAL_MSG_SIZE        (sizeof (MV_HWS_IPC_CTRL_MSG_STRUCT))

#define MV_PORT_STACK              (512) /* words - 1Kbytes */
#define MV_PORT_PRIORITY           (2)   /* 2 - Medium priority */
#define MV_PORT_MSG_NUM            (MV_PORT_CTRL_NUM_OF_QUEUE_MSGS)
#define MV_PORT_MSG_SIZE           (sizeof (MV_HWS_IPC_CTRL_MSG_STRUCT))

#define MV_AP_STACK                (256) /* words - 512 bytes */
#define MV_AP_PRIORITY             (3)   /* 3 - High priority */
#define MV_AP_MSG_NUM              (MV_PORT_CTRL_NUM_OF_QUEUE_MSGS)
#define MV_AP_MSG_SIZE             (sizeof (MV_HWS_IPC_CTRL_MSG_STRUCT))

#define MV_SUPERVISOR_STACK        (256) /* words - 512 bytes */
#define MV_SUPERVISOR_PRIORITY     (4)   /* 4- Very high priority */

#ifdef FREE_RTOS_HWS_ENHANCED_PRINT_MODE

#define MV_DEBUG_STACK             (256) /* words - 512 bytes */
#define MV_DEBUG_PRIORITY          (1)   /* 1- Low priority */

#endif /* FREE_RTOS_HWS_ENHANCED_PRINT_MODE */

/* Global Port Ctrl Debug definitions */
/* ================================== */
#define PORT_CTRL_DBG_REG_DUMP_DISABLE (0)
#define PORT_CTRL_DBG_REG_INT_NONE     (0x0)
#define PORT_CTRL_DBG_REG_INT_REDUCE   (0x1)
#define PORT_CTRL_DBG_REG_INT_FULL     (0x2)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvHwsPortCtrlDefines_H */

