/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 17821 2009-11-11 09:00:00Z dts12 $
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include "jwsMirrorTable.h"
#include "jws_ubus_if.h"
#include "stdlib.h"
#include "jwsDevBaseInfo.h"

/* create a new row in the (unsorted) table */
struct jwsMirrorTable_entry *
jwsMirrorTable_createEntry(long  jwsMirrorSourcePort);

/** Initializes the jwsMirrorTable module */
void
init_jwsMirrorTable(void)
{
  /* here we initialize all the tables we're planning on supporting */
    initialize_table_jwsMirrorTable();
}

//  # Determine the first/last column names

/** Initialize the jwsMirrorTable table by defining its contents and how it's structured */
void
initialize_table_jwsMirrorTable(void)
{
    static oid jwsMirrorTable_oid[] = {1,3,6,1,4,1,33444,2,2,4,1,2};
    size_t jwsMirrorTable_oid_len   = OID_LENGTH(jwsMirrorTable_oid);
    netsnmp_handler_registration    *reg;
    netsnmp_iterator_info           *iinfo;
    netsnmp_table_registration_info *table_info;

    int i;
    struct stJwsDevBaseInfo jwsDevBaseInfo;

    reg = netsnmp_create_handler_registration(
              "jwsMirrorTable",     jwsMirrorTable_handler,
              jwsMirrorTable_oid, jwsMirrorTable_oid_len,
              HANDLER_CAN_RWRITE
              );

    table_info = SNMP_MALLOC_TYPEDEF( netsnmp_table_registration_info );
    netsnmp_table_helper_add_indexes(table_info,
                           ASN_INTEGER,  /* index: jwsMirrorSourcePort */
                           0);
    table_info->min_column = COLUMN_JWSMIRRORSOURCEPORT;
    table_info->max_column = COLUMN_JWSMIRRORMODE;
    
    iinfo = SNMP_MALLOC_TYPEDEF( netsnmp_iterator_info );
    iinfo->get_first_data_point = jwsMirrorTable_get_first_data_point;
    iinfo->get_next_data_point  = jwsMirrorTable_get_next_data_point;
    iinfo->table_reginfo        = table_info;
    
    netsnmp_register_table_iterator( reg, iinfo );

    /* Initialise the contents of the table here */
    getJwsDevBaseInfo(&jwsDevBaseInfo);
    for (i = 1; i <= jwsDevBaseInfo.port_num; i++)
    {
        jwsMirrorTable_createEntry(i);
    }

}

    /* Typical data structure for a row entry */
struct jwsMirrorTable_entry {
    /* Index values */
//    long jwsMirrorSourcePort;

    /* Column values */
    long jwsMirrorSourcePort;
    long jwsMirrorMode;
    long old_jwsMirrorMode;

    /* Illustrate using a simple linked list */
    int   valid;
    struct jwsMirrorTable_entry *next;
};

struct jwsMirrorTable_entry  *jwsMirrorTable_head;

/* create a new row in the (unsorted) table */
struct jwsMirrorTable_entry *
jwsMirrorTable_createEntry(
                 long  jwsMirrorSourcePort
                ) {
    struct jwsMirrorTable_entry *entry;

    entry = SNMP_MALLOC_TYPEDEF(struct jwsMirrorTable_entry);
    if (!entry)
        return NULL;

    entry->jwsMirrorSourcePort = jwsMirrorSourcePort;
    entry->next = jwsMirrorTable_head;
    jwsMirrorTable_head = entry;
    return entry;
}

/* remove a row from the table */
void
jwsMirrorTable_removeEntry( struct jwsMirrorTable_entry *entry ) {
    struct jwsMirrorTable_entry *ptr, *prev;

    if (!entry)
        return;    /* Nothing to remove */

    for ( ptr  = jwsMirrorTable_head, prev = NULL;
          ptr != NULL;
          prev = ptr, ptr = ptr->next ) {
        if ( ptr == entry )
            break;
    }
    if ( !ptr )
        return;    /* Can't find it */

    if ( prev == NULL )
        jwsMirrorTable_head = ptr->next;
    else
        prev->next = ptr->next;

    SNMP_FREE( entry );   /* XXX - release any other internal resources */
}


/* Example iterator hook routines - using 'get_next' to do most of the work */
netsnmp_variable_list *
jwsMirrorTable_get_first_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    *my_loop_context = jwsMirrorTable_head;
    return jwsMirrorTable_get_next_data_point(my_loop_context, my_data_context,
                                    put_index_data,  mydata );
}

netsnmp_variable_list *
jwsMirrorTable_get_next_data_point(void **my_loop_context,
                          void **my_data_context,
                          netsnmp_variable_list *put_index_data,
                          netsnmp_iterator_info *mydata)
{
    struct jwsMirrorTable_entry *entry = (struct jwsMirrorTable_entry *)*my_loop_context;
    netsnmp_variable_list *idx = put_index_data;

    if ( entry ) {
        snmp_set_var_typed_integer( idx, ASN_INTEGER, entry->jwsMirrorSourcePort );
        idx = idx->next_variable;
        *my_data_context = (void *)entry;
        *my_loop_context = (void *)entry->next;
        return put_index_data;
    } else {
        return NULL;
    }
}


/** handles requests for the jwsMirrorTable table */
int
jwsMirrorTable_handler(
    netsnmp_mib_handler               *handler,
    netsnmp_handler_registration      *reginfo,
    netsnmp_agent_request_info        *reqinfo,
    netsnmp_request_info              *requests) {

    netsnmp_request_info       *request;
    netsnmp_table_request_info *table_info;
    struct jwsMirrorTable_entry          *table_entry;

    int ret = SNMP_ERR_NOERROR;
    int data = 0;
    char str[100] = {0};
    char value[10] = {0};

    switch (reqinfo->mode) {
        /*
         * Read-support (also covers GetNext requests)
         */
    case MODE_GET:
        for (request=requests; request; request=request->next) {
            table_entry = (struct jwsMirrorTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSMIRRORSOURCEPORT:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }
                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsMirrorSourcePort);
                break;
            case COLUMN_JWSMIRRORMODE:
                if ( !table_entry ) {
                    netsnmp_set_request_error(reqinfo, request,
                                              SNMP_NOSUCHINSTANCE);
                    continue;
                }

                ubus_get_port_cmd_enc(str, sizeof(str), table_entry->jwsMirrorSourcePort, "mode");
                ret = ubus_invoke_command("port_mirror", "port_mirror_get", 
                                        port_mirror_table_get_ret_handler_cb, str);
                if (ret != 0)
                {
                    return SNMP_ERR_NOSUCHNAME;
                }
                data = atoi(jws_read_buf_str());
                table_entry->jwsMirrorMode = data;

                snmp_set_var_typed_integer( request->requestvb, ASN_INTEGER,
                                            table_entry->jwsMirrorMode);
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
            table_entry = (struct jwsMirrorTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSMIRRORMODE:
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
            table_entry = (struct jwsMirrorTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);

            bzero(str, sizeof(str));
    
            switch (table_info->colnum) {
            case COLUMN_JWSMIRRORMODE:
                table_entry->old_jwsMirrorMode = table_entry->jwsMirrorMode;
                table_entry->jwsMirrorMode     = *request->requestvb->val.integer;

                snprintf(value, sizeof(value), "%ld", table_entry->jwsMirrorMode);
                ubus_set_port_cmd_enc(str, sizeof(str), table_entry->jwsMirrorSourcePort, "mode", value);
                ret = ubus_invoke_command("port_mirror", "port_mirror_set", 
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
            table_entry = (struct jwsMirrorTable_entry *)
                              netsnmp_extract_iterator_context(request);
            table_info  =     netsnmp_extract_table_info(      request);
    
            switch (table_info->colnum) {
            case COLUMN_JWSMIRRORMODE:
                table_entry->jwsMirrorMode     = table_entry->old_jwsMirrorMode;
                table_entry->old_jwsMirrorMode = 0;
                break;
            }
        }
        break;

    case MODE_SET_COMMIT:
        break;
    }
    return SNMP_ERR_NOERROR;
}
