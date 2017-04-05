# Source files in this directory
CSOURCES	= madDSPVct.c madDSPVct_gain.c madDSPVct_linkdown.c  
ifeq ($(VCT_PENDING),TRUE)
CSOURCES	= madDSPVct_sm.c madDSPVct_linkdown_sm.c  
ADDED_CFLAGS	+= -DMAD_SM_VCT
else
CSOURCES	= madDSPVct.c madDSPVct_gain.c madDSPVct_linkdown.c  
endif

ADDED_CFLAGS	+= -DMAD_DSP_VCT
ifeq ($(MVL_MATH),TRUE)
ADDED_CFLAGS	+= -DMAD_MVL_MATH
endif

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

# Include common build rules
ifeq ($(OS_RUN),VXWORKS)
include $(MAD_TOOL_DIR)\make.rules
endif
ifeq ($(OS_RUN),LINUX)
include $(MAD_TOOL_DIR)/makelnx.rules
endif

# end of file
