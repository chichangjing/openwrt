#ifndef _RATE_LIMIT
#define _RATE_LIMIT
void rate_limit_get_general(void);
void ingress_buck0_get_frame(const char * port);
void ingress_buck1_get_frame(const char * port);
void ingress_buck2_get_frame(const char * port);
void ingress_buck3_get_frame(const char * port);
void ingress_buck4_get_frame(const char * port);

void ingress_buck0_set_status(const char * port, const char * mode);
void ingress_buck0_get_status(const char * port);
void ingress_buck0_set_value(const char * port, const char * value);
void ingress_buck0_set_frame(const char * port, const char * frame);

void ingress_buck1_set_status(const char * port, const char * mode);
void ingress_buck1_get_status(const char * port);
void ingress_buck1_set_value(const char * port, const char * value);
void ingress_buck1_set_frame(const char * port, const char * frame);

void ingress_buck2_set_status(const char * port, const char * mode);
void ingress_buck2_get_status(const char * port);
void ingress_buck2_set_value(const char * port, const char * value);
void ingress_buck2_set_frame(const char * port, const char * frame);

void ingress_buck3_set_status(const char * port, const char * mode);
void ingress_buck3_get_status(const char * port);
void ingress_buck3_set_value(const char * port, const char * value);
void ingress_buck3_set_frame(const char * port, const char * frame);

void ingress_buck4_set_status(const char * port, const char * mode);
void ingress_buck4_get_status(const char * port);
void ingress_buck4_set_value(const char * port, const char * value);
void ingress_buck4_set_frame(const char * port, const char * frame);

void outgress_set_status(const char * port, const char * mode);
void outgress_get_status(const char * port);
void outgress_set_value(const char * port, const char * value);
#endif
