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
* appDemoFap20mSchedulerConfig.c
*
* DESCRIPTION:
*       Initialization functions for the Scheduler of FAP20M boards.
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
#include <appDemo/sandConfig/appDemoFap20mConfig.h>
#include <appDemo/boardConfig/appDemoBoardConfig.h>

/* SAND includes */
#include <SAND/Utils/include/sand_os_interface.h>
#include <SAND/Management/include/sand_module_management.h>

/* Fx950 includes */
#include <FX950/include/fap20m_api_auto_management.h>
#include <FX950/include/fap20m_scheduler.h>
#include <FX950/include/fap20m_tables_update.h>

#ifdef PRESTERA_DEBUG
#define FAP20M_APP_DEBUG
#endif

#define FAP20M_APP_DEBUG
#ifdef FAP20M_APP_DEBUG
#define DBG_INFO(x)     cpssOsPrintf x
#else
#define DBG_INFO(x)
#endif


#define APPDEMO_FAP20M_CLASS_TYPE_FQ        0
#define APPDEMO_FAP20M_CLASS_WEIGHT_FQ      2

extern  APPDEMO_FAP20M_PARAMS  appDemoFap20mParams;
extern APP_DEMO_PP_CONFIG    appDemoPpConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];

static FAP20M_QUEUE_TYPE 
        appDemo_default_queue_type_id = FAP20M_QUEUE_TYPE_NOF_TYPES;

static GT_STATUS
  appDemoFap20mGetSchedulingSchemeInfo(
    IN  APPDEMO_FAP20M_SCH_SCHEME_INFO  *scheduling_scheme,
    IN  unsigned int                    ports_rate,
    IN  unsigned int                    is_hr_henhanced_supported,
    OUT FAP20M_AUTO_PORT_INFO           *p_auto_ports_info,
    OUT FAP20M_AUTO_AGG_INFO            *p_auto_aggs_info,
    OUT FAP20M_AUTO_FLOW_INFO           *p_auto_flows_info,
    OUT FAP20M_AUTO_QUEUE_INFO          *p_auto_queues_info
  );

static void appDemoFap20mSetScmInfoStrictSimple( 
    OUT FAP20M_AUTO_PORT_INFO       *p_auto_ports_info,
    OUT FAP20M_AUTO_AGG_INFO        *p_auto_aggs_info,
    OUT FAP20M_AUTO_FLOW_INFO       *p_auto_flows_info,
    OUT FAP20M_AUTO_QUEUE_INFO      *p_auto_queues_info
  );

static void appDemoFap20mSetScmInfoStrictWithMinMaxRate(
    IN APPDEMO_FAP20M_SCH_SCHEME_INFO   *sch_scheme,
    OUT FAP20M_AUTO_PORT_INFO           *p_auto_ports_info,
    OUT FAP20M_AUTO_AGG_INFO            *p_auto_aggs_info,
    OUT FAP20M_AUTO_FLOW_INFO           *p_auto_flows_info,
    OUT FAP20M_AUTO_QUEUE_INFO          *p_auto_queues_info
    );


/*******************************************************************************
* appDemoFap20mSchedulerInit
*
* DESCRIPTION:
*       Initialization of the FAP20M scheduler.
*
* INPUTS:
*       deviceId                - device Id.
*       max_nof_faps_in_system  - maximal number of FAPs in system
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
GT_STATUS appDemoFap20mSchedulerInit
(
    IN unsigned int                 deviceId,
    IN unsigned int                 max_nof_faps_in_system
)
{
    static FAP20M_DEVICE_SCHEDULER     device_sch;
    static FAP20M_DEVICE_SCHEDULER     exact_device_sch;
    unsigned long               ret;
    unsigned int                slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES];
    unsigned int                entry_id;
    unsigned int                fap_id;
    SAND_RET                    sand_ret;
    FAP20M_SCH_RANK             sch_rank;
    GT_U32                      i;      /* loop iterator */
    GT_U32                      link_i; /* loop iterator */

    FAP20M_AUTO_SYSTEM_INFO
        auto_info;
    FAP20M_AUTO_PORT_INFO
        auto_ports_info;
    FAP20M_AUTO_AGG_INFO
        auto_aggs_info;
    FAP20M_AUTO_FLOW_INFO
        auto_flows_info;  
    FAP20M_AUTO_QUEUE_INFO
        auto_queues_info;  
    unsigned int   
        max_nof_ports_per_fap;
    unsigned int       
        ports_rate,
        is_hr_henhanced_supported;
    FAP20M_CL_SCHED_CLASS_PARAMS
        class_type_params,
        exact_class_type_params;
    FAP20M_QUEUE_TYPE_PARAM
        auto_q_type_params,
        exact_auto_q_type_params;


    slow_max_rates[0] = 300000;
    slow_max_rates[1] = 10000000;

    fap20m_sch_get_sch_rank_of_fabric(deviceId, &sch_rank);

    ret = fap20m_set_slow_max_rate(deviceId,sch_rank, slow_max_rates);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    /*
    * Enable the Enhanced HR.
    */
    ret = fap20m_set_HR_scheduler_modes(deviceId, sch_rank, GT_TRUE);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    device_sch.valid_mask  = FAP20M_DEVICE_SCHEDULER_MASK_ALL;
    for(i = 0; i < FAP20M_NOF_RCI_LEVELS; i++)
    {
      for(link_i = 0;link_i < FAP20M_NUMBER_OF_LINKS + 1 ; link_i++)
      {
        entry_id = (i * (FAP20M_NUMBER_OF_LINKS + 1)) + link_i;
        device_sch.fcg.drt.rates[entry_id].rci_level = i;
        device_sch.fcg.drt.rates[entry_id].num_active_links = link_i;
        device_sch.fcg.drt.rates[entry_id].rate = 32000;
      }
    }

    device_sch.dev_if_scheduler.valid_mask       = FAP20M_DEVICE_IF_SCHEDULER_MASK_ALL;
    device_sch.dev_if_scheduler.link_a_max_rate   = 12000;
    device_sch.dev_if_scheduler.link_b_max_rate   = 12000;
    device_sch.dev_if_scheduler.link_a_weight     = 10;
    device_sch.dev_if_scheduler.link_b_weight     = 10;

    ret = fap20m_set_device_scheduler(deviceId,
                                      sch_rank,
                                      &device_sch,
                                      &exact_device_sch
    );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    ret = fap20m_get_fap_id(deviceId, &fap_id);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    if(appDemoFap20mParams.nominal_port_rate_mbps < 2500
      )
    {
      slow_max_rates[0] = 100 * 1024;
      fap20m_set_slow_max_rate(deviceId,sch_rank,slow_max_rates);  }
    else
    {
      slow_max_rates[0] = 400 * 1024;
      fap20m_set_slow_max_rate(deviceId,sch_rank,slow_max_rates);
    }

      /* Add 1 port for CPU */
      max_nof_ports_per_fap = 
          appDemoFap20mParams.max_nof_ports_per_hgl * 2 + 1;
        
      class_type_params.id = APPDEMO_FAP20M_CLASS_TYPE_FQ;
      class_type_params.mode = FAP20M_CL_SCHED_CLASS_MODE_5;
      class_type_params.weight_mode = FAP20M_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW;
      ret = fap20m_set_scheduler_class_type_params(deviceId,
                                                   sch_rank,
                                                   &class_type_params,
                                                   &exact_class_type_params) ;
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }

      ret = fap20m_get_HR_scheduler_modes(deviceId, sch_rank, 
                                          &is_hr_henhanced_supported);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }

      /*{ Init */
      fap20m_api_auto_system_info_get_defaults(&auto_info);
      auto_info.max_nof_faps_in_system              = max_nof_faps_in_system;
      auto_info.max_nof_ports_per_fap               = max_nof_ports_per_fap;
      auto_info.device_info[deviceId].local_fap_id  = fap_id;

      ret = fap20m_sch_get_sch_rank_of_fabric(deviceId, 
                                              &(auto_info.fabric_sch_rank));
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }

      ret = fap20m_api_auto_init(&auto_info);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }
      /* Init } */
    
    
    /* { QUEUE */
     /*
      * Currently, all the scheme will use the same
      *  queue parameters.
      */
      ret = 
        fap20m_api_auto_queue_type_params_get_default(&auto_q_type_params);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }
    
      ret = 
        fap20m_api_auto_get_next_vacant_queue_type_id(&appDemo_default_queue_type_id);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }
    
      ret = 
        fap20m_api_auto_queue_set_queue_params(appDemo_default_queue_type_id,
                                               &auto_q_type_params,
                                               &exact_auto_q_type_params
                                               );
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }


      /* } QUEUE */
    
    /* { PER SCHEME INFO */
      /*
      * At init time, all the ports are opened with the
      *  same nominal rate.
      * When there is a port with different nominal rate,
      *  it should be updated with the function:
      *  fap20m_api_auto_update_port().
      */
      ports_rate = appDemoFap20mParams.nominal_port_rate_mbps;
      ret = appDemoFap20mGetSchedulingSchemeInfo(
                        &(appDemoFap20mParams.default_sch_scheme),
                        ports_rate,
                        is_hr_henhanced_supported,
                        &auto_ports_info,
                        &auto_aggs_info,
                        &auto_flows_info,
                        &auto_queues_info
      );
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }

    /* } PER SCHEME INFO */
      
    /* { FLOWS */
      ret = fap20m_api_auto_set_info_and_open_all_ports(&auto_ports_info);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }

      ret = fap20m_api_auto_set_info_and_open_all_aggregates(&auto_aggs_info);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          DBG_INFO(("fap20m_api_auto_set_info_and_open_all_aggregates fails %X\n", ret));
          return GT_FAIL;
      }

      ret = fap20m_api_auto_set_info_and_open_all_queues(&auto_queues_info);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          DBG_INFO(("fap20m_api_auto_set_info_and_open_all_queues fails %X\n", ret));
          return GT_FAIL;
      }

      ret = fap20m_api_auto_set_info_and_open_all_flows(&auto_flows_info);
      sand_ret = sand_get_error_code_from_error_word(ret) ;
      if (sand_ret != SAND_OK)
      {
          return GT_FAIL;
      }

    /* } FLOWS */
    
    return GT_OK;
}

/*******************************************************************************
* appDemoFap20mGetSchedulingSchemeInfo
*
* DESCRIPTION:
*       Initialization of the FAP20M scheduler.
*
* INPUTS:
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
static GT_STATUS appDemoFap20mGetSchedulingSchemeInfo(
    IN  APPDEMO_FAP20M_SCH_SCHEME_INFO  *scheduling_scheme,
    IN  unsigned int                    ports_rate,
    IN  unsigned int                    is_hr_henhanced_supported,
    OUT FAP20M_AUTO_PORT_INFO           *p_auto_ports_info,
    OUT FAP20M_AUTO_AGG_INFO            *p_auto_aggs_info,
    OUT FAP20M_AUTO_FLOW_INFO           *p_auto_flows_info,
    OUT FAP20M_AUTO_QUEUE_INFO          *p_auto_queues_info
  )
{
    unsigned int
        agg_level_i,
        agg_index_i,
        flow_i;
    unsigned long
        ret;
    SAND_RET    sand_ret;

    ret = SAND_OK; 

    ret = fap20m_api_auto_port_get_default(p_auto_ports_info, ports_rate);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }
  

    if(is_hr_henhanced_supported)
    {
      if(scheduling_scheme->scheme_id == APPDEMO_FAP20M_SCH_SCHEME_ENHANCED_STRICT)
      {
        p_auto_ports_info->hr_mode = FAP20M_HR_SCHED_MODE_ENHANCED_PRIO_WFQ;
      }
    }

    ret = fap20m_api_auto_aggregates_get_default(
        p_auto_aggs_info
      );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    for(agg_level_i = 0; agg_level_i < FAP20M_AUTO_NOF_HIERARCY; agg_level_i++)
    {
        for(agg_index_i = 0 ; agg_index_i < FAP20M_AUTO_NOF_INDEX; agg_index_i++)
        {
            p_auto_aggs_info->item[agg_level_i][agg_index_i].flow.sub_flow[0].shaper.max_rate = 
            p_auto_aggs_info->item[agg_level_i][agg_index_i].flow.sub_flow[1].shaper.max_rate = 
                ports_rate * 1100;

            p_auto_aggs_info->item[agg_level_i][agg_index_i].flow.sub_flow[0].shaper.max_burst = 
            p_auto_aggs_info->item[agg_level_i][agg_index_i].flow.sub_flow[1].shaper.max_burst = 
                FAP20M_SUB_FLOW_SHAPER_DEFAULT_MAX_BURST;
        }
    }

    ret = 
        fap20m_api_auto_flows_get_default(p_auto_flows_info);
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }


    for(flow_i = 0; flow_i < FAP20M_FABRIC_UNICAST_CLASSES; flow_i++)
    {
      p_auto_flows_info->flows[flow_i].sub_flow[0].shaper.max_rate =
      p_auto_flows_info->flows[flow_i].sub_flow[1].shaper.max_rate = 
          ports_rate * 1100;

      p_auto_flows_info->flows[flow_i].sub_flow[0].shaper.max_burst = 
      p_auto_flows_info->flows[flow_i].sub_flow[1].shaper.max_burst = 
          FAP20M_SUB_FLOW_SHAPER_DEFAULT_MAX_BURST;
    }

  
    ret = 
    fap20m_api_auto_queue_info_get_default(p_auto_queues_info,
                                           appDemo_default_queue_type_id
                                           );
    sand_ret = sand_get_error_code_from_error_word(ret) ;
    if (sand_ret != SAND_OK)
    {
        return GT_FAIL;
    }

    switch(scheduling_scheme->scheme_id){
    case APPDEMO_FAP20M_SCH_SCHEME_STRICT:
        appDemoFap20mSetScmInfoStrictSimple(p_auto_ports_info,p_auto_aggs_info,
                                            p_auto_flows_info,p_auto_queues_info
                                            );
      break;
    case APPDEMO_FAP20M_SCH_SCHEME_ENHANCED_STRICT:
      if(is_hr_henhanced_supported)
      {
        /*
        * Support minimum and maximum rate according to 
        *  the fields of 'scheduling_scheme'.
        */
         appDemoFap20mSetScmInfoStrictWithMinMaxRate(
             scheduling_scheme,p_auto_ports_info,p_auto_aggs_info,
             p_auto_flows_info,p_auto_queues_info
             );
      }
      else
      {
        /*
        * No minimum and maximum rates per class.
        */
        appDemoFap20mSetScmInfoStrictSimple(
            p_auto_ports_info,p_auto_aggs_info,
            p_auto_flows_info,p_auto_queues_info
        );
      }
      break;
    case APPDEMO_FAP20M_SCH_SCHEME_WFQ_PER_FLOW:
    case APPDEMO_FAP20M_SCH_SCHEME_WFQ_PER_CLASS:
    case APPDEMO_FAP20M_SCH_SCHEME_MIXED_STRICT_AND_CLASS_WEIGHT:
    case APPDEMO_FAP20M_SCH_SCHEME_MIXED_STRICT_AND_FLOW_WEIGHT:
      break;
    default:
      sand_os_printf("fap20m_get_scheduling_scheme_info got undefined scheme");
      return GT_BAD_PARAM;

    }
  return GT_OK;
}

/*
* The scheme can be found at Dune's scheduling schemes
*  document "DN-SW-FAP20M-APP".
*  under the name "Basic Strict Priority" 
* 8 strict priorities.
*/
static void appDemoFap20mSetScmInfoStrictSimple( 
    OUT FAP20M_AUTO_PORT_INFO       *p_auto_ports_info,
    OUT FAP20M_AUTO_AGG_INFO        *p_auto_aggs_info,
    OUT FAP20M_AUTO_FLOW_INFO       *p_auto_flows_info,
    OUT FAP20M_AUTO_QUEUE_INFO      *p_auto_queues_info
  )
{
    FAP20M_AUTO_AGG_ITEM *
        p_curr_agg_item;
    unsigned int
        class_i;

    /*port info is currently not changed*/
    /*queue info is currently not changed*/
    /*aggregate info*/

    p_curr_agg_item = &(p_auto_aggs_info->item[0][0]);
    p_curr_agg_item->valid   = TRUE;
    p_curr_agg_item->flow.id = 
      p_curr_agg_item->sch.id = 
        fap20m_api_auto_get_agg_auto_relative_id(0,0);
    p_curr_agg_item->flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;
    p_curr_agg_item->flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_NONE;
    p_curr_agg_item->flow.sub_flow[0].credit_source =
      FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT;
    p_curr_agg_item->flow.sub_flow[0].subflow_properties.hr.sp_class = 
      FAP20M_SUB_FLOW_HR_SINGLE_CLASS_BE1;
    p_curr_agg_item->sch.type = FAP20M_SUB_FLOW_TYPE_HR;
    p_curr_agg_item->sch.sched_properties.hr.mode =
      FAP20M_HR_SCHED_MODE_SINGLE_WFQ;

    for(class_i=0;class_i<FAP20M_FABRIC_UNICAST_CLASSES;class_i++)
    {
        p_auto_flows_info->flows[class_i].sub_flow[0].type = 
          FAP20M_SUB_FLOW_TYPE_HR;
        p_auto_flows_info->flows[class_i].sub_flow[1].type = 
          FAP20M_SUB_FLOW_TYPE_NONE;

        switch(class_i)
        {
            case 0:
            case 1:
            case 2:
              p_auto_flows_info->flows[class_i].sub_flow[0].credit_source =
                FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT;
              p_auto_flows_info->flows[class_i].sub_flow[0].subflow_properties.hr.sp_class = 
                FAP20M_SUB_FLOW_HR_CLASS_EF1 + class_i;
            break;
            case 3:
              p_auto_flows_info->flows[class_i].sub_flow[0].credit_source =
                FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT;
              p_auto_flows_info->flows[class_i].sub_flow[0].subflow_properties.hr.sp_class = 
                FAP20M_SUB_FLOW_HR_SINGLE_CLASS_AF1_WFQ;
              p_auto_flows_info->flows[class_i].sub_flow[0].subflow_properties.hr.weight = 
                2;
            break;
            case 4:
            case 5:
            case 6:
              p_auto_flows_info->flows[class_i].sub_flow[0].credit_source =
                fap20m_api_auto_get_agg_auto_relative_id(0,0);
              p_auto_flows_info->flows[class_i].sub_flow[0].subflow_properties.hr.sp_class = 
                FAP20M_SUB_FLOW_HR_CLASS_EF1 + class_i - 4;
            break;
            case 7:
            default:
              p_auto_flows_info->flows[class_i].sub_flow[0].credit_source =
                fap20m_api_auto_get_agg_auto_relative_id(0,0);
              p_auto_flows_info->flows[class_i].sub_flow[0].subflow_properties.hr.sp_class = 
                FAP20M_SUB_FLOW_HR_SINGLE_CLASS_BE1;
        }
    }
}

/*
*  under the name "Enhanced Strict" 
* 8 classes. Each class get assured rate, and the rest
*  of the bandwidth is distributed by strict priority.
*/
static void appDemoFap20mSetScmInfoStrictWithMinMaxRate(
    IN APPDEMO_FAP20M_SCH_SCHEME_INFO *sch_scheme,
    OUT FAP20M_AUTO_PORT_INFO       *p_auto_ports_info,
    OUT FAP20M_AUTO_AGG_INFO        *p_auto_aggs_info,
    OUT FAP20M_AUTO_FLOW_INFO       *p_auto_flows_info,
    OUT FAP20M_AUTO_QUEUE_INFO      *p_auto_queues_info
  )
{
    FAP20M_AUTO_AGG_ITEM *
      p_curr_agg_item;
    unsigned int
      class_i,
      fq_i;

    /*port info is currently not changed*/
    /*queue info is currently not changed*/
    /*aggregate info*/

   /*
    * The FQ aggregate schedulers will use the HR port scheduler
    *  and the HR aggregate schedulers as credit sources.
    */
    for(fq_i=0;fq_i<FAP20M_FABRIC_UNICAST_CLASSES;fq_i++)
    {
        p_curr_agg_item = &(p_auto_aggs_info->item[0][fq_i]);

        p_curr_agg_item->sch.type = FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL;
        p_curr_agg_item->sch.sched_properties.cl.class_type = APPDEMO_FAP20M_CLASS_TYPE_FQ;
        p_curr_agg_item->valid   = TRUE;
        p_curr_agg_item->flow.id = 
          p_curr_agg_item->sch.id = 
            fap20m_api_auto_get_agg_auto_relative_id(0,fq_i);

        p_curr_agg_item->flow.sub_flow[0].type = FAP20M_SUB_FLOW_TYPE_HR;

        p_curr_agg_item->flow.sub_flow[0].credit_source =
          FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT;
        p_curr_agg_item->flow.sub_flow[1].credit_source =
          FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT;

        p_curr_agg_item->flow.sub_flow[0].subflow_properties.hr.weight = 2; 

        /*
        * If 'minimum_rate_by_percentage' is requested, the first
        *  SP will distribute the assured rate to each class.
        */
        if(sch_scheme->minimum_rate_by_percentage)
        {
          p_curr_agg_item->flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_HR;
          p_curr_agg_item->flow.sub_flow[1].subflow_properties.hr.sp_class = 
            FAP20M_SUB_FLOW_HR_CLASS_EF1;
          /*Port is in Mbit, shaper in Kbits*/
          p_curr_agg_item->flow.sub_flow[1].shaper.max_rate = 
            p_auto_ports_info->eg_port.nominal_bandwidth * 10 * 
            sch_scheme->minimum_rate_by_percentage; 
        }
        else
        {
          p_curr_agg_item->flow.sub_flow[1].type = FAP20M_SUB_FLOW_TYPE_NONE;
        }

        if(sch_scheme->maximum_rate_by_percentage &&
           sch_scheme->maximum_rate_by_percentage < 100
          )
        {
          p_curr_agg_item->flow.sub_flow[0].shaper.max_rate = 
            p_auto_ports_info->eg_port.nominal_bandwidth * 10 * 
            (sch_scheme->maximum_rate_by_percentage -
              sch_scheme->minimum_rate_by_percentage ); 
        }

        switch(fq_i)
        {
            case 0:
            case 1:
              p_curr_agg_item->flow.sub_flow[0].subflow_properties.hr.sp_class = 
                FAP20M_SUB_FLOW_HR_CLASS_EF1 + (fq_i + 1);
            break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            default:
              p_curr_agg_item->flow.sub_flow[0].subflow_properties.hr.sp_class = 
                FAP20M_SUB_FLOW_HR_ENHANCED_CLASS_AF1 + fq_i - 2;
        }
    }

    /*Flows info*/
    /*
    * The flows will use the FQ schedulers as credit source.
    */
    for(class_i=0;class_i<FAP20M_FABRIC_UNICAST_CLASSES;class_i++)
    {
        p_auto_flows_info->flows[class_i].sub_flow[1].subflow_properties.hr.sp_class = 
          FAP20M_SUB_FLOW_HR_ENHANCED_CLASS_BE2;
        p_auto_flows_info->flows[class_i].sub_flow[1].type = 
          FAP20M_SUB_FLOW_TYPE_HR;
        p_auto_flows_info->flows[class_i].sub_flow[1].credit_source =
          FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT;

        if(sch_scheme->support_low_latency == FALSE)
        {
            p_auto_flows_info->flows[class_i].sub_flow[1].type = 
                FAP20M_SUB_FLOW_TYPE_NONE;
        }
        else
        {
            if(sch_scheme->maximum_rate_by_percentage &&
               sch_scheme->maximum_rate_by_percentage < 100)
            {
                p_auto_flows_info->flows[class_i].sub_flow[1].shaper.max_rate = 32;
            }
            else
            {
                p_auto_flows_info->flows[class_i].sub_flow[1].shaper.max_rate = 
                  FAP20M_SUB_FLOW_SHAPER_DEFAULT_MAX_RATE;
                p_auto_flows_info->flows[class_i].is_slow_enabled |= 
                  FAP20M_FLOW_SLOW_SUB_FLOW_2_SLOW_INDEX; 
            } 
        }
        p_auto_flows_info->flows[class_i].sub_flow[0].type = 
          FAP20M_SUB_FLOW_TYPE_CL;
        p_auto_flows_info->flows[class_i].sub_flow[0].subflow_properties.cl.class_val = 
          FAP20M_SUB_FLOW_CL_CLASS_SP1_WFQ;
        p_auto_flows_info->flows[class_i].sub_flow[0].subflow_properties.cl.weight = 
          APPDEMO_FAP20M_CLASS_WEIGHT_FQ;
        p_auto_flows_info->flows[class_i].sub_flow[0].credit_source =
          fap20m_api_auto_get_agg_auto_relative_id(0,class_i);

        p_auto_flows_info->flows[class_i].sub_flow[0].shaper.max_rate =
          p_auto_ports_info->eg_port.nominal_bandwidth * 1500;
    } 
}

unsigned long   
    appDemoSetShaper(
      unsigned int                     device_id, 
      FAP20M_SCH_RANK                  sch_rank,
      unsigned int                     sub_flow_id,
      unsigned int                                PeakRateMan,
      unsigned int                                PeakRateExp,
      unsigned int                                MaxBurst,
      unsigned int                                MaxBurstUpdate

)
{
    FAP20M_INTERNAL_SUB_FLOW_DESC    internal_sub_flow;
    internal_sub_flow.PeakRateMan = PeakRateMan;
    internal_sub_flow.PeakRateExp = PeakRateExp;
    internal_sub_flow.MaxBurst = MaxBurst;
    internal_sub_flow.MaxBurstUpdate = MaxBurstUpdate;


    return fap20m_set_SCH_SHD_entry_unsafe(device_id, 
                                           sch_rank,
                                           sub_flow_id, 
                                           &internal_sub_flow);

}
