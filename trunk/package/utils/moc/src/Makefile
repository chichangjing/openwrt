export CDT_ROOT = $(shell cd ./; pwd)

export DSDT_ROOT=$(CDT_ROOT)/dsdt
export DSDT_USER_BASE=$(DSDT_ROOT)/switch
export DSDT_PROJ_NAME=qdDrv
export DSDT_TOOL_DIR=$(DSDT_USER_BASE)/tools
export RELEASE=YES
export OS_RUN=LINUX
export MAD_USER_BASE=$(DSDT_ROOT)/phy
export MAD_PROJ_NAME=madDrv
export MAD_TOOL_DIR=$(MAD_USER_BASE)/tools
export USER_API_NAME=
export USER_NAME=
export DSP_VCT=FALSE
export VCT_PENDING=TRUE
export DSDT_USE_MAD=TRUE

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
          -I $(CDT_ROOT)/acci/include \
          -I $(CDT_ROOT)/app/include \
          -I $(CDT_ROOT)/board/include \
          -I $(CDT_ROOT)/device/include \
          -I $(CDT_ROOT)/system/include

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
ifeq ($(CP),)
	export CP =cp 
endif
ifeq ($(LD),)
	export LD = ld
endif

export OBJ_DSDT = $(DSDT_LIB_DIR)/$(DSDT_PROJ_NAME).o
export OBJ_DAD  = $(MAD_LIB_DIR)/$(MAD_PROJ_NAME).o
export OBJ_BRD  = $(CDT_ROOT)/board/objboard.o
export OBJ_DEV  = $(CDT_ROOT)/device/objdevice.o
export OBJ_SYS  = $(CDT_ROOT)/system/objsystem.o
export OBJ_ACC	  = $(CDT_ROOT)/acci/objacci.o

all: 
	@ $(MAKE) clean
	@ $(MAKE) -C $(CFLAGS) ./acci
	@ $(MAKE) -C $(CFLAGS) ./app
	@ $(MAKE) -C $(CFLAGS) ./board
	@ $(MAKE) -C $(CFLAGS) ./device
	@ $(MAKE) -C $(CFLAGS) ./dsdt
	@ $(MAKE) -C $(CFLAGS) ./system
	@ $(CC) -o test app/main.o $(OBJ_BRD) $(OBJ_DEV) $(OBJ_SYS) $(OBJ_DSDT) $(OBJ_DAD) $(OBJ_ACC) -lpthread -lm 
	@ $(CC) -o mdio app/mdio.o $(OBJ_BRD) $(OBJ_DEV) $(OBJ_SYS) $(OBJ_DSDT) $(OBJ_DAD) $(OBJ_ACC) -lpthread -lm
	@ $(CC) -o xmdio app/xmdio.o $(OBJ_BRD) $(OBJ_DEV) $(OBJ_SYS) $(OBJ_DSDT) $(OBJ_DAD) $(OBJ_ACC) -lpthread -lm 
	@ $(CC) -o pphy app/pphy.o $(OBJ_BRD) $(OBJ_DEV) $(OBJ_SYS) $(OBJ_DSDT) $(OBJ_DAD) $(OBJ_ACC) -lpthread -lm 

	
clean:
	@ $(RM) test mdio xmdio
	@ $(MAKE) -C $(CFLAGS) ./acci clean
	@ $(MAKE) -C $(CFLAGS) ./app clean
	@ $(MAKE) -C $(CFLAGS) ./board clean
	@ $(MAKE) -C $(CFLAGS) ./device clean
	@ $(MAKE) -C $(CFLAGS) ./dsdt clean
	@ $(MAKE) -C $(CFLAGS) ./system clean
	
	
