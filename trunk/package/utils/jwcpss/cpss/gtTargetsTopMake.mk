####################################################################
# gtTargetsTopMake.mk
#
# DESCRIPTION:
#       Common CPSS Target rules
#       This is a collection of rules required to build
#       CPSS components
#
# DEPENDENCIES:
#       None
#
# FILE REVISION NUMBER:
#       $Revision: 8 $
####################################################################

ifeq (YES, $(QUIET))
	QUIET_CALL=@
else
	QUIET_CALL=
endif

AD_CFLAGS += $(addprefix $(FD),$(sort $(PROJECT_DEFS)))
OS_DEFINE += $(addprefix $(FD),$(sort $(OS_DEFS)))

ifeq (D_ON,$(DEBUG_INFO))
   CFLAGS += $(FD)DEBUG
endif

ifeq (P_ON,$(API_LOCK_PROTECTION))
	CFLAGS += $(FD)CPSS_API_LOCK_PROTECTION
endif

ifeq (L_ON,$(LOG_ENABLE))
	CFLAGS += $(FD)CPSS_LOG_ENABLE
endif

#
# add the H files of includes , to find what C files to compile
# a change in H file force all C files that has include to it to compile
#
# this section was pulled out from the gtDefsTopMake.mk file in order
# to allow the change of value $(DEP_FILES) after calling gtDefsTopMake.mk
# (before calling gtTargetsTopMake.mk)
#
ifneq (,$(DEP_FILES))
  # MAKECMDGOALS is not one of dep clean_p mkld
  ifeq (, $(filter _$(MAKECMDGOALS),_dep _clean_p _mkld))
    include $(DEP_FILES)
  endif
endif

all: dep touch compile
	+$(QUIET_CALL)set -e;$(DO_LOOP);$(DO_KERNELLOOP)

ifeq (, $(wildcard $(USER_BASE)/tools/build/toolkit/$(TOOLKIT_MAKE_RULES)Targets.mk))
$(error "Make defs (build/toolkit/$(TOOLKIT_MAKE_RULES)Targets.mk) not found)")
endif
include $(USER_BASE)/tools/build/toolkit/$(TOOLKIT_MAKE_RULES)Targets.mk

# removed Level0 conditionned directories create
# build_dir dependence added instead 
dep: build_dir $(DEP_FILE_LIST)
	+$(QUIET_CALL)set -e;$(DO_LOOP);$(DO_KERNELLOOP)

lib: localToLib
	+$(QUIET_CALL)$(DO_LOOP);$(DO_KERNELLOOP)

localToLib: compile
	+$(QUIET_CALL)$(DO_LOOP);$(DO_KERNELLOOP)
ifneq (0, $(words $(OBJ_FILES)))
	$(call AR_template,$(LIB_DIR)/$(LIB_NAME_FIX),$(OBJ_FILES))
endif
ifneq (0, $(words $(ASM_OBJ_FILES)))
	$(call AR_template,$(LIB_DIR)/$(LIB_NAME_FIX),$(ASM_OBJ_FILES))
endif

onelib: localToOneLib
	+$(QUIET_CALL)$(DO_LOOP);$(DO_KERNELLOOP)

localToOneLib: compile
ifneq (0, $(words $(OBJ_FILES)))
	$(call AR_template,$(LIB_DIR)/$(PROJ_NAME).$(LIBEXT),$(OBJ_FILES))
endif
ifneq (0, $(words $(ASM_OBJ_FILES)))
	$(call AR_template,$(LIB_DIR)/$(PROJ_NAME).$(LIBEXT),$(ASM_OBJ_FILES))
endif

#lib_full is command needed to do both "one lib and all libs"
#but with better efficient then separated commands --> for VC
lib_full: localToLib_full
	+$(QUIET_CALL)$(DO_LOOP);$(DO_KERNELLOOP)

localToLib_full:
ifneq (0, $(words $(OBJ_FILES)))
	$(call AR_template,$(LIB_DIR)/$(LIB_NAME_FIX),$(OBJ_FILES))
endif
ifneq (0, $(words $(ASM_OBJ_FILES)))
	$(call AR_template,$(LIB_DIR)/$(LIB_NAME_FIX),$(ASM_OBJ_FILES))
endif


compile: $(OBJ_FILE_LIST) $(ASM_OBJ_FILE_LIST)
		+@$(QUIET_CALL)set -e;$(DO_LOOP)

local: $(OBJ_FILE_LIST) $(ASM_OBJ_FILE_LIST) localToLib localToOneLib


qac_full: qac_dir dep qac qac_rep

qac: $(QAC_FILE_LIST)
	$(QUIET_CALL)set -e;$(DO_LOOP)

qac_rep: $(QAC_REP_FILE_LIST)
	$(QUIET_CALL)set -e;$(DO_LOOP)

touch:
ifneq (0,$(words $(TOUCH_FILE_LIST)))
ifeq (visual, $(TOOL))
	$(TOUCH) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.obj)))
	$(RM) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.obj)))
else
	$(TOUCH) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.o)))
	$(RM) $(addprefix $(SCRAP_DIR)/,$(notdir $(TOUCH_FILE_LIST:.c=.o)))
endif
endif

PARTIAL_BUILD_FLAGS = -r

# gmake -f $(SW_ROOT)/$(PROJ_NAME)/gtTopMake mkld
# is wrond fore Dune drivers directory, PROJ_MAKE defined in gtDefsTopMake.mk
# as $(SW_ROOT)/$(PROJ_NAME)/gtTopMake and overridden in Dune's gtTopMake
elf: mkld
#	$(MAKE) -f $(PROJ_MAKE) _mkld

mkld: compile
	$(MAKE) -f $(PROJ_MAKE) _mkld

_mkld:
ifneq (0,$(words $(EXE_OBJ_LIST)))
ifeq (visual, $(TOOL))
	$(AR) /nologo /out:$(LIB_DIR)/$(ARCH_NAME).$(LIBEXT) $(SCRAP_DIR)/*.$(OBJ) /IGNORE:4006 
else
ifeq (builder, $(TOOL))
	$(AR) $(LIB_DIR_FIX)\$(ARCH_NAME).$(LIBEXT) $(EXE_OBJ_LIST_FIX)
else
	$(LD) $(LDFLAGS) $(PARTIAL_BUILD_FLAGS) $(BUILD_MAP_FILE) $(LIB_DIR)/$(ARCH_NAME).map $(FO) $(LIB_DIR)/$(ARCH_NAME).o $(SCRAP_DIR)/*.$(OBJ)
endif
endif
endif

ifeq (visual,$(TOOL))
# single lib is a VC specific target which creates one library using all object files
lib_in_one_step: compile
	$(AR) /nologo /out:$(LIB_DIR)/$(ARCH_NAME).$(LIBEXT) $(SCRAP_DIR)/\*.$(OBJ) /IGNORE:4006 
endif

$(DEP_FILE_LIST) : %.dep : %.c
ifeq  ($(findstring gnu,$(TOOL)),gnu)
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(PROJECT_PATH) -M -o $(SCRAP_DIR)/$@ $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))
endif
ifeq  ($(findstring diab,$(TOOL)),diab)
	$(CC) $(OPTION_DEPEND_C) $(CFLAGS) $(INCLUDE_PATH) $(PROJECT_PATH) $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE)) > $(SCRAP_DIR)/$@ 
endif

ifeq (win32, $(OS_RUN))
	-$(MD) -f- $(MDFLAGS) $(subst /I ,-I,$(INCLUDE_PATH) $(PROJECT_PATH)) $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE)) > $(SCRAP_DIR)/$@ 
endif

$(QAC_FILE_LIST) : %.c.err : %.c %.dep
	qac $(QAC_PROJECT_PATH) $(QAC_FLAGS) -via $(QAC_PERSONALITIES) -op $(QAC_OUT_DIR) $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))


$(QAC_REP_FILE_LIST) : %.rep : %.c %.c.err
#	prjdsp qac $(QAC_PROJECT_PATH)  -via $(USER_BASE)\tools\Prqa\personalities.via  -op $(QAC_OUT_DIR) -file $(QAC_OUT_DIR)/$@ $(filter %/$(notdir $<),$(C_FILE_LIST_AFTER_EXCLUDE))
	errdsp qac -via $(QAC_PERSONALITIES) -op $(QAC_OUT_DIR) -file $(QAC_OUT_DIR)/$@ ./$<
	prjdsp qac -via $(QAC_PERSONALITIES) -op $(QAC_OUT_DIR) -file $(QAC_OUT_DIR)/$@.lst ./$<

clean_loop:
	$(QUIET_CALL)$(DO_LOOP);$(DO_KERNELLOOP)
ifneq (0,$(words $(DEP_FILES)))
	$(RM) $(DEP_FILES)
endif
ifneq (0,$(words $(OBJ_FILES)))
	$(RM) $(OBJ_FILES)
endif
ifneq (0,$(words $(ASM_OBJ_FILES)))
	$(RM) $(ASM_OBJ_FILES)
endif
ifneq (0,$(words $(wildcard $(LIB_DIR)/$(LIB_NAME_FIX))))
	$(RM) $(LIB_DIR)/$(LIB_NAME_FIX)
endif

.PHONY: clean_p
clean_p: clean_loop
ifneq (0,$(words $(wildcard $(LIB_DIR)/$(ARCH_NAME).map)))
	$(RM) $(LIB_DIR)/$(ARCH_NAME).map
endif

# used "-p" flag: create all the path without "alredy exist" warning
build_dir: $(SCRAP_DIR) $(LIB_DIR)

$(SCRAP_DIR):
	- @ $(MKDIR) -p $(SCRAP_DIR)

$(LIB_DIR):
	- @ $(MKDIR) -p $(LIB_DIR)

qac_dir: $(QAC_OUT_DIR)

$(QAC_OUT_DIR):
	- @ $(MKDIR) -p $(QAC_OUT_DIR)


