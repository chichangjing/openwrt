# Source files in this directory
CSOURCES	=  log10f.c sinf.c sqrtf.c powf.c floorf.c polevlf.c powif.c constf.c
ADDED_CFLAGS	+= -DMAD_DSP_VCT

ASOURCES	=

# Include common variable definitions
ifeq ($(OS_RUN),VXWORKS)
include $(TOOL_DIR)\make.defs
endif
ifeq ($(OS_RUN),LINUX)
include $(TOOL_DIR)/makelnx.defs
endif

# Add in extra stuffs
EXTRA_INCLUDE	+=
EXTRA_DEFINE	+=
ADDED_CFLAGS	+=

# Include common build rules
ifeq ($(OS_RUN),VXWORKS)
include $(TOOL_DIR)\make.rules
endif
ifeq ($(OS_RUN),LINUX)
include $(TOOL_DIR)/makelnx.rules
endif

# end of file
