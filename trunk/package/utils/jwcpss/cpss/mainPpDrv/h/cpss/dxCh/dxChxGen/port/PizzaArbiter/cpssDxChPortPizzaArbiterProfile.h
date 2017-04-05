/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChPortPizzaArbiterProfile.h
*
* DESCRIPTION:
*       Pizza arbiter profile definitons
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#ifndef __CPSS_DXCH_PORT_PIZZA_ARBITER_PROFILE_H
#define __CPSS_DXCH_PORT_PIZZA_ARBITER_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpss/generic/cpssTypes.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h> 

#define PIZZA_DEV_NUM_MAX_CNS               PRV_CPSS_MAX_PP_DEVICES_CNS
#define PIZZA_MAX_SLICES_PER_PORT_CNS       10
#define PIZZA_MAX_PORTS_PER_PORTGROUP_CNS   PRV_CPSS_GEN_PP_MAX_PORTS_IN_PORT_GROUP_CNS
#define PIZZA_MAX_SLICES_PER_PORTGROUP_CNS  24

#define CPSS_PA_INVALID_PORT           0xFF         /* CPSS_PORT ARBITER INVALID_PORT  :  max port per port group is 16 */
#define CPSS_PA_INVALID_SLICE          0xFF         /* CPSS_PORT ARBITER INVALID_SLICE :  max slice is 242 at TxQ Unit  */

typedef GT_U8   CPSS_DXCH_PIZZA_PORT_ID;
typedef GT_U8   CPSS_DXCH_PIZZA_SLICE_ID;


typedef GT_U32  PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT;
#define PRV_CPSS_DXCH_PORT_PA_SLICE_NUM_PER_PORT_NONE_CNS   0


/*******************************************************************************
* CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC data structures
*
* DESCRIPTION:
*         Conversion of speed to slice number
*
* APPLICABLE DEVICES:
*        
*
* NOT APPLICABLE DEVICES:
*        
*
* Function : 
*       prvCpssDxChPizzaArbiterSelectSlicesBySpeed()
*
* INPUTS:
*       CPSS_PORT_SPEED_ENT portSpeed : portSpeed to be converted to slices 
* OUTPUTS:
*       GT_U32 *  sliceNumUsedPtr  = 0 if not found (not supported) 
*                                    n if the speed is supported
* RETURNS:
*       GT_NOT_SUPPORTED if the speed is not supported
*       GT_OK            otherwise
*
* COMMENTS:
*       last item in the list shall be <CPSS_PORT_SPEED_NA_E, CPSS_INVALID_SLICE_NUM (== -1 )>
*       see define below
*******************************************************************************/

#define CPSS_INVALID_SLICE_NUM  -1

typedef struct
{
    CPSS_PORT_SPEED_ENT speed;
    GT_32               slicesNum;
}CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC;


typedef struct
{
    GT_U32 portN;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC * portSpeed2SliceNumListPtr;
}CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC;

/* one is used for last empty entry */
typedef struct
{
    /*GT_U32                                    portN;  */
    CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC   arr[PIZZA_MAX_PORTS_PER_PORTGROUP_CNS+1];
}CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC;


/*---------------------------------------------------------------------*/
/* Port Priority Graph representation in sparse Matrix form            */
/*    for each port list of ports having priority smaller than its one */
/*    End of list is indicated by CPSS_PA_INVALID_PORT  value          */
/*    Empty list consists of CPSS_PA_INVALID_PORT value                */
/*---------------------------------------------------------------------*/

#define PORTPRIORITY_MAX_CNS  10
typedef struct
{
    GT_U32         portN;
    CPSS_DXCH_PIZZA_PORT_ID  portPriorityList[PIZZA_MAX_PORTS_PER_PORTGROUP_CNS][PORTPRIORITY_MAX_CNS];
}CPSS_DXCH_PORT_PRIORITY_MATR_STC;


/*----------------------------*/
/* slices list                */
/*----------------------------*/
typedef CPSS_DXCH_PIZZA_SLICE_ID CPSS_DXCH_PORT_PIZZA_CFG_SLICE_LIST_STC[PIZZA_MAX_SLICES_PER_PORT_CNS+1];

/*--------------------------------------------------------*/
/* list of port' slices list for each port == Pizza Config*/
/*--------------------------------------------------------*/
typedef struct
{
    GT_U32                   maxPortNum; 
    CPSS_DXCH_PORT_PIZZA_CFG_SLICE_LIST_STC   portConfArr[PIZZA_MAX_PORTS_PER_PORTGROUP_CNS];
}CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC;


/*--------------------------------------------------------------------*/
/* Pizza : Pizza Config for each possible number of slices            */
/*                          port priority graph                       */
/*--------------------------------------------------------------------*/
typedef struct
{
    GT_U32                       sliceNum2Config;
    CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC  *pizzaPtr;
}SliceNumXPortPizzaCfgSlicesList_STC;

#define PIZZACFGPERPORTMAX_NUM    10

typedef struct
{
    GT_U32                                  portNum;
    GT_U32                                  totalSlices;
    SliceNumXPortPizzaCfgSlicesList_STC     pizzaCfg[PIZZACFGPERPORTMAX_NUM];
}CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC;


/*---------------------------------------------------------------------------*/
/* PizzaDef : Pizza Config for each mode (currently regular and extended     */
/*              number of ports to be controlled                             */
/*              number of slices to be controlled                            */
/*              for transmit Queue -- repetiotion number (how many time      */
/*                                    the slices configuration repeat itself)*/
/*---------------------------------------------------------------------------*/

#define CPSS_PA_NUMBER_OF_CPU_SLICES 4

typedef enum 
{
    CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E = 0,   /* no CPU defined */
    CPSS_DXCH_CPUPORT_TYPE_FIXED_E,           /* CPU port' slices are configured on fixed position */
    CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E,        /* CPU port' slices are configured in addition to 
                                                  slices of all other port (after all others slices)*/
    CPSS_EXMXPM_CPUPORT_TYPE_MAX_E            /* never cross this boundary */
}CPSS_DXCH_CPUPORT_TYPE_ENT;

typedef struct
{
    CPSS_DXCH_CPUPORT_TYPE_ENT    type;
    GT_U32                        cpuPortNum;
    CPSS_DXCH_PIZZA_SLICE_ID      cpuPortSlices[CPSS_PA_NUMBER_OF_CPU_SLICES + 1]; /* The last shall be CPSS_PA_INVALIDE_SLICE */
}CPSS_DXCH_CPU_PORTDEF_STC;

typedef struct
{
    GT_U32                    pizzaRepetitionNum;
    CPSS_DXCH_CPU_PORTDEF_STC cpuPortDef;
}CPSS_DXCH_TXQDEF_STC; 

typedef struct
{
    GT_U32              portNum;
    GT_U32              maxSliceNum;
    CPSS_DXCH_TXQDEF_STC  txQDef;
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * regModePizzaPtr;
    CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * extModePizzaPtr;
    CPSS_DXCH_PORT_PRIORITY_MATR_STC            * pPortPriorityMatrPtr;
}CPSS_DXCH_PIZZA_PROFILE_STC;



#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif


