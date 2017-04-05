
export CDT_ROOT = $(shell cd ./; pwd)

export DSDT_ROOT=$(CDT_ROOT)/dsdt
export DSDT_USER_BASE=$(DSDT_ROOT)/switch
export DSDT_PROJ_NAME=qdDrv
export DSDT_TOOL_DIR=$(DSDT_USER_BASE)/tools
export RELEASE=NO
export OS_RUN=LINUX
export MAD_USER_BASE=$(DSDT_ROOT)/phy
export MAD_PROJ_NAME=madDrv
export MAD_TOOL_DIR=$(MAD_USER_BASE)/tools
export USER_API_NAME=
export USER_NAME=
export DSP_VCT=FALSE
export VCT_PENDING=TRUE
export DSDT_USE_MAD=FALSE

export DSDT_LIB_DIR=$(DSDT_USER_BASE)/Library
export MAD_LIB_DIR=$(MAD_USER_BASE)/Library

export CHECK_API_SELECT=GT_RMGMT_ACCESS

ifeq ($(CFLAGS),)
export CFLAGS = 
endif

#CFLAGS += -$(DCHECK_API_SELECT)

ifeq ($(IFLAGS),)
export IFLAGS = 
endif

IFLAGS += -I $(DSDT_ROOT)/phy/Include \
          -I $(DSDT_ROOT)/phy/Include/h/driver \
          -I $(DSDT_ROOT)/phy/Include/h/macsec \
          -I $(DSDT_ROOT)/phy/Include/h/madApi \
          -I $(DSDT_ROOT)/phy/Include/h/platform \
          -I $(DSDT_ROOT)/switch/Include \
          -I $(DSDT_ROOT)/switch/Include/h/driver \
          -I $(DSDT_ROOT)/switch/Include/h/msApi \
          -I $(DSDT_ROOT)/switch/Include/h/platform \
          -I $(CDT_ROOT)/device \
          -I $(CDT_ROOT)/app \
          -I $(CDT_ROOT)/platform

ifeq ($(LDFLAGS),)
	export LDFLAGS =
endif
LDFLAGS += -r -X
ifeq ($(CC),)
	export CC = gcc
endif
ifeq ($(MAKE),)
	export MAKE = make
endif
ifeq ($(RM),)
	export RM = rm
endif
ifeq ($(LD),)
	export LD = ld
endif

#export DEBUG_QD

all: 
	#@ $(MAKE) clean
	@ $(MAKE) -C $(CFLAGS) ./dsdt
	@ $(MAKE) -C $(CFLAGS) ./device
	@ $(MAKE) -C $(CFLAGS) ./app
	@ $(MAKE) -C $(CFLAGS) ./platform
	@ $(CC) -o test platform/pfm.o app/app.o device/dev.o $(DSDT_LIB_DIR)/$(DSDT_PROJ_NAME).o $(MAD_LIB_DIR)/$(MAD_PROJ_NAME).o 

	
clean:
	@ $(RM) test
	@ $(MAKE) -C $(CFLAGS) ./dsdt clean
	@ $(MAKE) -C $(CFLAGS) ./device clean
	@ $(MAKE) -C $(CFLAGS) ./app clean
	@ $(MAKE) -C $(CFLAGS) ./platform clean
	
	
