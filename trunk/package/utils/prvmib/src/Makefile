###############################################################################
# filename: Makefile
# description:
# author:
# date:
# note:
###############################################################################
PROJECT_TOP_DIR=$(shell cd ./; pwd)
PROJECT_SRC_DIR=$(PROJECT_TOP_DIR)/src
PROJECT_INC_DIR=$(PROJECT_TOP_DIR)/inc
PROJECT_LIB_DIR=$(PROJECT_TOP_DIR)/lib
PROJECT_OBJ_DIR=$(PROJECT_TOP_DIR)/objs
MKDIR = mkdir -p

# compiler
CC ?= gcc

# library name will be created to lib(LIB_NAME).so file by compiler 
LIB_NAME := jws2100switch
DLLIB := lib$(LIB_NAME).so

# C compiler flag
CFLAGS+=-I. `net-snmp-config --cflags`
BUILDLIBS=`net-snmp-config --libs`
BUILDAGENTLIBS=`net-snmp-config --agent-libs`
JWSCONVERT_INC=$(STAGING_DIR)/usr/include/libjwsconvert
JSSDK_INC=$(STAGING_DIR)/usr/include/jssdk
LIBS =-lubox -lubus -lblobmsg_json -ljwsconvert -luci -ljssdk_us

# shared library flags (assumes gcc)
DLFLAGS =-fPIC -shared

# fetch object file name
SRCS :=$(wildcard $(PROJECT_SRC_DIR)/*.c)  
DIR := $(notdir $(SRCS))  
PROJECT_OBJ := $(patsubst %.c,%.o,$(DIR) )
PROJECT_ALL_OBJS := $(addprefix $(PROJECT_OBJ_DIR)/, $(PROJECT_OBJ))

# command
all: $(DLLIB)

$(DLLIB): $(PROJECT_ALL_OBJS) Makefile
	$(CC) $(DLFLAGS) $(LIBS) $(PROJECT_ALL_OBJS) -o $(PROJECT_LIB_DIR)/$@

$(PROJECT_OBJ_DIR)/%.o : $(PROJECT_SRC_DIR)/%.c
	$(CC) -I$(PROJECT_INC_DIR) -I$(JWSCONVERT_INC) -I$(JSSDK_INC) -c $(CFLAGS) $^ -o $@ 

clean:
	rm -rf $(PROJECT_LIB_DIR)/*
	rm -rf $(PROJECT_OBJ_DIR)/*

.PHONY: clean all
