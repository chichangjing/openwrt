/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 17821 2009-11-11 09:00:00Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsStpPortTable.h"

#include "jws_ubus_if.h"
#include "stdlib.h"
#include "jwslog.h"
#include "jwsDevBaseInfo.h"

/* create a new row in the (unsorted) table */
struct jwsStpPortTable_entry *
jwsStpPortTable_createEntry(
                 long  jwsStpPort
                );

/** Initializes the jwsStpPortTable module */
void
init_jwsStpPortTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_jwsStpPortTable();
}

//  # Determine the first/last column names

/** Initialize the jwsStpPortTable table by defining its contents and how it's structured */
void
initialize_table_jwsStpPortTable(void)
{
    static oid jwsStpPortTable_oid[] = {1,3,6,1,4,1,33444,2,2,12,1,6};
    size_t jwsStpPortTable_oid_len   = OID_LENGTH(jwsStpPortTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    int i;
    struct stJwsDevBaseInfo jwsDevBaseInfo;

    reg = netsnmp_create_handler_registration(
              "jwsStpPortTable",     jwsStpPortTable_handler,
              jwsStpPortTable_oid, jwsStpPortTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: jwsStpPort */
                           0);
    table_info->min_column = COLUMN_JWSSTPPORT;
    table_info->max_column = COLUMN_JWSSTPPORTPATHCOSTAUTOCALCULATE;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = jwsStpPortTable_get_first_data_point;
    iinfo->get_next_data_point  = jwsStpPortTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
    getJwsDevBaseInfo(&jwsDevBaseInfo);
    for (i = 1; i <= jwsDevBaseInfo.port_num; i++)
    {
        jwsStpPortTable_createEntry(i);
    }
}

    /* Typical data structure for a row entry */
struct jwsStpPortTable_entry {
    /* Index values */
    //long jwsStpPort;

    /* Column values */
    long jwsStpPort;
    long jwsStpPortEnable;
    long old_jwsStpPortEnable;
    long jwsStpPortPriority;
    long old_jwsStpPortPriority;
    long jwsStpPortPathCost;
    long old_jwsStpPortPathCost;
    long jwsStpPortPathCostAutoCalculate;
    long old_jwsStpPortPathCostAutoCalculate;

    /* Illustrate using a simple linked list */
    int   valid;
    struct jwsStpPortTable_entry *next;
};

struct jwsStpPortTable_entry  *jwsStpPortTable_head;

/* create a new row in the (unsorted) table */
struct jwsStpPortTable_entry *
jwsStpPortTable_createEntry(
                 long  jwsStpPort
                ) {
    struct jwsStpPortTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct jwsStpPortTable_entry);
    if (!entry)
        return NULL;

    entry->jwsStpPort = jwsStpPort;
    entry->next = jwsStpPortTable_head;
    jwsStpPortTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
jwsStpPortTable_removeEntry( struct jwsStpPortTable_entry *entry ) {
    struct jwsStpPortTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = jwsStpPortTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        jwsStpPortTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
jwsStpPortTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = jwsStpPortTable_head;
    return jwsStpPortTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
jwsStpPortTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct jwsStpPortTable_entry *entry = (struct jwsStpPortTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->jwsStpPort );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the jwsStpPortTable table */
int
jwsStpPortTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct jwsStpPortTable_entry          *table_entry;

    int ret = SNMP_ERR_NOERROR;
    unsigned int data = 0;
    char str[100] = {0};
    char value[10] = {0};

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsStpPortTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSSTPPORT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsStpPort);
                break;
            case COLUMN_JWSSTPPORTENABLE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_status");
                ret = ubus_invoke_command("rstp", "rstp_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsStpPortEnable = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsStpPortEnable);
                break;
            case COLUMN_JWSSTPPORTPRIORITY:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_priority");
                ret = ubus_invoke_command("rstp", "rstp_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsStpPortPriority = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsStpPortPriority);
                break;
            case COLUMN_JWSSTPPORTPATHCOST:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_path_cost");
                ret = ubus_invoke_command("rstp", "rstp_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsStpPortPathCost = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsStpPortPathCost);
                break;
            case COLUMN_JWSSTPPORTPATHCOSTAUTOCALCULATE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_auto_calculate_cost");
                ret = ubus_invoke_command("rstp", "rstp_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsStpPortPathCostAutoCalculate = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsStpPortPathCostAutoCalculate);
                break;
            default:
                netsnmp_set_request_error(reqinfo, request,
                                          SNMP_NOSUCHOBJECT);
                break;
            }
        }
        break;

        /*
         * Write-support
         */
    case MODE_SET_RESERVE1:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsStpPortTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSSTPPORTENABLE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_JWSSTPPORTPRIORITY:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_JWSSTPPORTPATHCOST:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            case COLUMN_JWSSTPPORTPATHCOSTAUTOCALCULATE:
                /* or possibly 'netsnmp_check_vb_int_range' */
                ret = netsnmp_check_vb_int( request->requestvb );
                if ( ret != SNMP_ERR_NOERROR ) {
                    netsnmp_set_request_error( reqinfo, request, ret );
                    return SNMP_ERR_NOERROR;
                }
                break;
            default:
                netsnmp_set_request_error( reqinfo, request,
                                           SNMP_ERR_NOTWRITABLE );
                return SNMP_ERR_NOERROR;
            }
        }
        break;

    case MODE_SET_RESERVE2:
        break;

    case MODE_SET_FREE:
        break;

    case MODE_SET_ACTION:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsStpPortTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSSTPPORTENABLE:
                table_entry->old_jwsStpPortEnable = table_entry->jwsStpPortEnable;
                table_entry->jwsStpPortEnable     = *request->requestvb->val.integer;
                
                snprintf(value, sizeof(value), "%ld", table_entry->jwsStpPortEnable);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_status", value);
                ret = ubus_invoke_command("rstp", "rstp_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0) {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            case COLUMN_JWSSTPPORTPRIORITY:
                table_entry->old_jwsStpPortPriority = table_entry->jwsStpPortPriority;
                table_entry->jwsStpPortPriority     = *request->requestvb->val.integer;

                snprintf(value, sizeof(value), "%ld", table_entry->jwsStpPortPriority);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_priority", value);
                ret = ubus_invoke_command("rstp", "rstp_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0) {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            case COLUMN_JWSSTPPORTPATHCOST:
                table_entry->old_jwsStpPortPathCost = table_entry->jwsStpPortPathCost;
                table_entry->jwsStpPortPathCost     = *request->requestvb->val.integer;
                
                snprintf(value, sizeof(value), "%ld", table_entry->jwsStpPortPathCost);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_path_cost", value);
                ret = ubus_invoke_command("rstp", "rstp_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0) {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            case COLUMN_JWSSTPPORTPATHCOSTAUTOCALCULATE:
                table_entry->old_jwsStpPortPathCostAutoCalculate = table_entry->jwsStpPortPathCostAutoCalculate;
                table_entry->jwsStpPortPathCostAutoCalculate     = *request->requestvb->val.integer;

                snprintf(value, sizeof(value), "%ld", table_entry->jwsStpPortPathCostAutoCalculate);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsStpPort, "port_auto_calculate_cost", value);
                ret = ubus_invoke_command("rstp", "rstp_set", 
                                    set_ret_handler_cb, str);
                if (ret != 0) {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            }
        }
        break;

    case MODE_SET_UNDO:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsStpPortTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSSTPPORTENABLE:
                table_entry->jwsStpPortEnable     = table_entry->old_jwsStpPortEnable;
                table_entry->old_jwsStpPortEnable = 0;
                break;
            case COLUMN_JWSSTPPORTPRIORITY:
                table_entry->jwsStpPortPriority     = table_entry->old_jwsStpPortPriority;
                table_entry->old_jwsStpPortPriority = 0;
                break;
            case COLUMN_JWSSTPPORTPATHCOST:
                table_entry->jwsStpPortPathCost     = table_entry->old_jwsStpPortPathCost;
                table_entry->old_jwsStpPortPathCost = 0;
                break;
            case COLUMN_JWSSTPPORTPATHCOSTAUTOCALCULATE:
                table_entry->jwsStpPortPathCostAutoCalculate     = table_entry->old_jwsStpPortPathCostAutoCalculate;
                table_entry->old_jwsStpPortPathCostAutoCalculate = 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
