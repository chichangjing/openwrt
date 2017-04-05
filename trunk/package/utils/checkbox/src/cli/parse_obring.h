#ifndef __PARSE_OBRING_H__
#define __PARSE_OBRING_H__
#include "../caselib/cs_type.h"

int parse_display_ring(char* argv);
int parse_clear_ring(char* argv);
int parse_obring_enable(char* argv);
int parse_obring_disable(char* argv);
int parse_obring_view_init(char* argv);
int parse_obring_view_clear(char* argv);
int parse_obring_set_enable(char* argv);
int parse_obring_set_disable(char* argv);
int parse_obring_set_priority(char* argv);
int parse_obring_set_primary_port(char* argv);
int parse_obring_set_second_port(char* argv);

#endif
