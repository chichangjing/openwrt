#include <string.h>
#include <stdlib.h>

#include "encapuci.h"
#include "utils.h"

/* this encapsulation split an uci tuple string 、look up an element and check if correct
 * Suitable occasion: packet、section、option have known
 */ 
int uci_load_config(const char *name,enum uci_option_type type,struct uci_ptr *ptr,struct uci_context *ctx)
{
    if(name == NULL)
    {
        LOG_ERROR("invalid name");
        return -1;
    }

    char *name_bak = strdup(name);
    if(UCI_OK != uci_lookup_ptr(ctx,ptr,name_bak,true))
    {
        LOG_ERROR("no matching arguement: %s",name);
        free(name_bak);
        return -1;
    }
    free(name_bak);

    if(ptr->o != NULL && ptr->o->type == type)
        return 0;

    LOG_ERROR("error during matching option type");
    return -1;
}
