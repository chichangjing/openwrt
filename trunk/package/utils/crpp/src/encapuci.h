#ifndef _ENCAPUCI_H_
#define _ENCAPUCI_H_

#include <uci.h>

int uci_load_config(const char *name,enum uci_option_type type,struct uci_ptr *ptr,struct uci_context *ctx);
#endif
