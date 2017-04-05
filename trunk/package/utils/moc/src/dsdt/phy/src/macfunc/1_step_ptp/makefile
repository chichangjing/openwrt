# Source files in this directory
TARGET = 
CSOURCES	= ptp_init.c ptp_funcs.c ptp_api.c ptp_utils.c ptp_samples.c ptp_constants.c

ASOURCES	=

# Include common variable definitions
ifeq ($(OS_RUN),VXWORKS)
include $(MAD_TOOL_DIR)\make.defs
endif
ifeq ($(OS_RUN),LINUX)
include $(MAD_TOOL_DIR)/makelnx.defs
endif

# Add in extra stuffs
EXTRA_INCLUDE	+=
EXTRA_DEFINE	+=
ADDED_CFLAGS	+=
ADDED_CFLAGS	+= -DDEBUG_ALL
*ADDED_CFLAGS	+= -DDEBUG_MAD

# Include common build rules
ifeq ($(OS_RUN),VXWORKS)
include $(MAD_TOOL_DIR)\make.rules
endif
ifeq ($(OS_RUN),LINUX)
include $(MAD_TOOL_DIR)/makelnx.rules
endif

# end of file
