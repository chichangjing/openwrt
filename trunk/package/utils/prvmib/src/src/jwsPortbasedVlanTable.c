/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 17821 2009-11-11 09:00:00Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsPortbasedVlanTable.h"

#include "jws_ubus_if.h"
#include "stdlib.h"
#include "jws_convert.h"
#include "jwsDevBaseInfo.h"

#define VLAN_MODE_PORT_BASED    (1)
#define VLAN_MODE_802DOT1Q      (2)

/* create a new row in the (unsorted) table */
struct jwsPortbasedVlanTable_entry *
jwsPortbasedVlanTable_createEntry(
                 long  jwsPortbasedVlanPortIndex
                );

/** Initializes the jwsPortbasedVlanTable module */
void
init_jwsPortbasedVlanTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_jwsPortbasedVlanTable();
}

//  # Determine the first/last column names

/** Initialize the jwsPortbasedVlanTable table by defining its contents and how it's structured */
void
initialize_table_jwsPortbasedVlanTable(void)
{
    static oid jwsPortbasedVlanTable_oid[] = {1,3,6,1,4,1,33444,2,2,9,1,2};
    size_t jwsPortbasedVlanTable_oid_len   = OID_LENGTH(jwsPortbasedVlanTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    int i;
    struct stJwsDevBaseInfo jwsDevBaseInfo;

    reg = netsnmp_create_handler_registration(
              "jwsPortbasedVlanTable",     jwsPortbasedVlanTable_handler,
              jwsPortbasedVlanTable_oid, jwsPortbasedVlanTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: jwsPortbasedVlanPortIndex */
                           0);
    table_info->min_column = COLUMN_JWSPORTBASEDVLANPORTINDEX;
    table_info->max_column = COLUMN_JWSPORTBASEDVLANSWITCHPORTS;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = jwsPortbasedVlanTable_get_first_data_point;
    iinfo->get_next_data_point  = jwsPortbasedVlanTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
    getJwsDevBaseInfo(&jwsDevBaseInfo);
    for (i = 1; i <= jwsDevBaseInfo.port_num; i++)
    {
        jwsPortbasedVlanTable_createEntry(i);
    }
    
}

    /* Typical data structure for a row entry */
struct jwsPortbasedVlanTable_entry {
    /* Index values */
    //long jwsPortbasedVlanPortIndex;

    /* Column values */
    long jwsPortbasedVlanPortIndex;
#define STR_NUM (8)
    char jwsPortbasedVlanSwitchPorts[STR_NUM];
    size_t jwsPortbasedVlanSwitchPorts_len;
    char old_jwsPortbasedVlanSwitchPorts[STR_NUM];
    size_t old_jwsPortbasedVlanSwitchPorts_len;

    /* Illustrate using a simple linked list */
    int   valid;
    struct jwsPortbasedVlanTable_entry *next;
};

struct jwsPortbasedVlanTable_entry  *jwsPortbasedVlanTable_head;

/* create a new row in the (unsorted) table */
struct jwsPortbasedVlanTable_entry *
jwsPortbasedVlanTable_createEntry(
                 long  jwsPortbasedVlanPortIndex
                ) {
    struct jwsPortbasedVlanTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct jwsPortbasedVlanTable_entry);
    if (!entry)
        return NULL;

    entry->jwsPortbasedVlanPortIndex = jwsPortbasedVlanPortIndex;
    entry->next = jwsPortbasedVlanTable_head;
    jwsPortbasedVlanTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
jwsPortbasedVlanTable_removeEntry( struct jwsPortbasedVlanTable_entry *entry ) {
    struct jwsPortbasedVlanTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = jwsPortbasedVlanTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        jwsPortbasedVlanTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
jwsPortbasedVlanTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = jwsPortbasedVlanTable_head;
    return jwsPortbasedVlanTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
jwsPortbasedVlanTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct jwsPortbasedVlanTable_entry *entry = (struct jwsPortbasedVlanTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->jwsPortbasedVlanPortIndex );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the jwsPortbasedVlanTable table */
int
jwsPortbasedVlanTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct jwsPortbasedVlanTable_entry          *table_entry;

    int ret;
    char str[100] = {0};
    char value[50];
    int vlan_mode = 0;

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsPortbasedVlanTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSPORTBASEDVLANPORTINDEX:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsPortbasedVlanPortIndex);
                break;
            case COLUMN_JWSPORTBASEDVLANSWITCHPORTS:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ret = ubus_invoke_command("vlan_config", "vlan_config_get", get_ret_handler_cb, 
                                "{\"get_args\":[\"mode\"]}");
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                vlan_mode = atoi(jws_read_buf_str());

                if(VLAN_MODE_PORT_BASED != vlan_mode){
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsPortbasedVlanPortIndex, 
                                "port_base_vlan_portlist");
                ret = ubus_invoke_command("vlan_config", "vlan_config_get", 
                                        get_table_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }

                table_entry->jwsPortbasedVlanSwitchPorts_len = count_char(jws_read_buf_str(), '-') + 1;
                convert_str2array(
                    jws_read_buf_str(), 
                    table_entry->jwsPortbasedVlanSwitchPorts, 
                    table_entry->jwsPortbasedVlanSwitchPorts_len
                    );

                snmp_set_var_typed_value( request->requestvb, ASN_OCTET_STR,
                                 (u_char*)table_entry->jwsPortbasedVlanSwitchPorts,
                                          table_entry->jwsPortbasedVlanSwitchPorts_len);
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
            table_entry = (struct jwsPortbasedVlanTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSPORTBASEDVLANSWITCHPORTS:
	        /* or possiblc 'netsnmp_check_vb_type_and_size' */
                ret = netsnmp_check_vb_type_and_max_size(
                          request->requestvb, ASN_OCTET_STR, sizeof(table_entry->jwsPortbasedVlanSwitchPorts));
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
            table_entry = (struct jwsPortbasedVlanTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSPORTBASEDVLANSWITCHPORTS:
                memcpy( table_entry->old_jwsPortbasedVlanSwitchPorts,
                        table_entry->jwsPortbasedVlanSwitchPorts,
                        sizeof(table_entry->jwsPortbasedVlanSwitchPorts));
                table_entry->old_jwsPortbasedVlanSwitchPorts_len =
                        table_entry->jwsPortbasedVlanSwitchPorts_len;
                memset( table_entry->jwsPortbasedVlanSwitchPorts, 0,
                        sizeof(table_entry->jwsPortbasedVlanSwitchPorts));
                memcpy( table_entry->jwsPortbasedVlanSwitchPorts,
                        request->requestvb->val.string,
                        request->requestvb->val_len);
                table_entry->jwsPortbasedVlanSwitchPorts_len =
                        request->requestvb->val_len;

                convert_array2str(table_entry->jwsPortbasedVlanSwitchPorts, 
                    table_entry->jwsPortbasedVlanSwitchPorts_len, value, 
                    3*table_entry->jwsPortbasedVlanSwitchPorts_len);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsPortbasedVlanPortIndex, 
                                "port_base_vlan_portlist", value);
                ret = ubus_invoke_command("vlan_config", "vlan_config_set", 
                                        set_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }

                break;
            }
        }
        break;

    case MODE_SET_UNDO:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsPortbasedVlanTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSPORTBASEDVLANSWITCHPORTS:
                memcpy( table_entry->jwsPortbasedVlanSwitchPorts,
                        table_entry->old_jwsPortbasedVlanSwitchPorts,
                        sizeof(table_entry->jwsPortbasedVlanSwitchPorts));
                memset( table_entry->old_jwsPortbasedVlanSwitchPorts, 0,
                        sizeof(table_entry->jwsPortbasedVlanSwitchPorts));
                table_entry->jwsPortbasedVlanSwitchPorts_len =
                        table_entry->old_jwsPortbasedVlanSwitchPorts_len;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
