all: checkbox
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

SW_LIBS = -lubox -lubus -lblobmsg_json
PWD_DIR			=$(shell cd ./; pwd)
CLI_DIR			=$(PWD_DIR)/cli
UBUS_DIR		=$(PWD_DIR)/ubus
CASELIB_DIR   =$(PWD_DIR)/caselib

CHECKBOX 		= $(CLI_DIR)/checkbox.o


CASELIB_O=$(CASELIB_DIR)/cs_type.o
PARSE_O=$(CLI_DIR)/cli_parse.o $(CLI_DIR)/cli.o $(CLI_DIR)/parse_flow.o \
		  $(CLI_DIR)/parse_aggr.o $(CLI_DIR)/parse_if_cfg.o $(CLI_DIR)/parse_lldp.o \
		  $(CLI_DIR)/parse_mcast.o $(CLI_DIR)/parse_mirror.o $(CLI_DIR)/parse_qos.o \
		  $(CLI_DIR)/parse_snmp.o $(CLI_DIR)/parse_system.o $(CLI_DIR)/parse_vlan.o \
		  $(CLI_DIR)/parse_rate_limit.o $(CLI_DIR)/parse_fdb.o $(CLI_DIR)/parse_obring.o 
UBUS_O=$(UBUS_DIR)/eth_stats.o $(UBUS_DIR)/port_config.o $(UBUS_DIR)/port_mirror.o\
		$(UBUS_DIR)/port_aggr.o $(UBUS_DIR)/mcast_config.o $(UBUS_DIR)/rate_limit.o\
		$(UBUS_DIR)/vlan.o $(UBUS_DIR)/qos.o $(UBUS_DIR)/system_info.o\
		$(UBUS_DIR)/netconfig.o $(UBUS_DIR)/lldp.o $(UBUS_DIR)/snmp.o\
		$(UBUS_DIR)/ubus_call.o $(UBUS_DIR)/device_info.o $(UBUS_DIR)/fdb.o\
		$(UBUS_DIR)/ob_ring.o $(UBUS_DIR)/device.o

checkbox: $(CHECKBOX) $(UBUS_O) $(PARSE_O) $(CASELIB_O)
	$(CC) -o $@ $(CHECKBOX) $(SW_LIBS) $(UBUS_O) $(PARSE_O) $(CASELIB_O)


.PHONY:clean
clean:
	rm -f *.o
