/*
 * Note: this file originally auto-generated by mib2c using
 *  : mib2c.iterate.conf 17821 2009-11-11 09:00:00Z dts12 $
 */
#ifndef JWSMIRRORTABLE_H
#define JWSMIRRORTABLE_H

/* function declarations */
void init_jwsMirrorTable(void);
void initialize_table_jwsMirrorTable(void);
Netsnmp_Node_Handler jwsMirrorTable_handler;
Netsnmp_First_Data_Point  jwsMirrorTable_get_first_data_point;
Netsnmp_Next_Data_Point   jwsMirrorTable_get_next_data_point;

/* column number definitions for table jwsMirrorTable */
       #define COLUMN_JWSMIRRORSOURCEPORT		1
       #define COLUMN_JWSMIRRORMODE		2
#endif /* JWSMIRRORTABLE_H */
