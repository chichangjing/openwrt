####################################################################
#   Common Definitions required to Build/link project
####################################################################

ifndef COMPILATION_ROOT
	COMPILATION_ROOT = $(USER_BASE)
endif
COMPILATION_ROOT_FIX = $(subst \,/,$(COMPILATION_ROOT))

ifeq (, $(wildcard $(USER_BASE)/tools/build/toolkit/$(TOOLKIT_MAKE_RULES)Defs.mk))
$(error "Make defs (build/toolkit/$(TOOLKIT_MAKE_RULES)Defs.mk) not found)")
endif
include $(USER_BASE)/tools/build/toolkit/$(TOOLKIT_MAKE_RULES)Defs.mk

ifeq (win32,$(OS_RUN))
	LIBOBJ=lib
else
	LIBOBJ=o
endif

### handle the PP files ###
PP_LIBS =$(wildcard $(COMPILATION_ROOT_FIX)/mainPpDrv/libs/$(CPU_DIR)/mainPp*.$(LIBEXT))
ifneq (win32,$(OS_RUN))
PP_LIBS+=$(wildcard $(COMPILATION_ROOT_FIX)/mainPpDrv/libs/$(CPU_DIR)/*FdbHash.$(LIBEXT))
PP_LIBS+=$(wildcard $(COMPILATION_ROOT_FIX)/mainPpDrv/libs/$(CPU_DIR)/*IpLpmEngine.$(LIBEXT))
endif

ifeq (RHODES, $(FPGA_TYPE))
    FPGA_FILES = $(wildcard $(COMPILATION_ROOT_FIX)/mainRhodesDrv/libs/$(CPU_DIR)/mainRhodesDrv.$(LIBEXT))
endif


UTF_LIB=$(wildcard $(COMPILATION_ROOT_FIX)/mainUT/libs/$(CPU_DIR)/mainU*.$(LIBEXT))

UTIL_FILES = $(wildcard $(COMPILATION_ROOT_FIX)/mainExtUtils/libs/$(CPU_DIR)/mainExtUtils.$(LIBOBJ))


ifeq ($(NO_PP_USED),NOT_USED)
	PP_LIBS =
endif

### handle the COMMON files ###
COMMON_LIBS = $(COMPILATION_ROOT_FIX)/common/libs/$(CPU_DIR)/common.$(LIBOBJ)

### handle the FA files ###
ifeq (PRESTERA, $(FA_VENDOR))
	FA_FILES = $(COMPILATION_ROOT_FIX)/mainFaDrv/libs/$(CPU_DIR)/mainFaDrv.$(LIBOBJ)
endif

### handle the XBAR files ###
ifeq (PRESTERA, $(XBAR_VENDOR))
	XBAR_FILES = $(COMPILATION_ROOT_FIX)/mainXbarDrv/libs/$(CPU_DIR)/mainXbarDrv.$(LIBOBJ)
endif

### handle the Galtis files ###
ifeq (EXCLUDE_LIB, $(EXCLUDE_GALTIS))
GALTIS_FILES = 
else
GALTIS_FILES = $(COMPILATION_ROOT_FIX)/mainGaltisWrapper/libs/$(CPU_DIR)/mainGaltisWrapper.$(LIBOBJ)
endif

### handle the Lua wrappers files ###
ifeq (yes, $(CMD_LUA_CLI))
LUA_WRAPPER_FILES = $(COMPILATION_ROOT_FIX)/mainLuaWrapper/libs/$(CPU_DIR)/mainLuaWrapper.$(LIBOBJ)
else
LUA_WRAPPER_FILES =
endif

### handle TM API files
TM_API_FILES = $(COMPILATION_ROOT_FIX)/mainTmDrv/libs/$(CPU_DIR)/mainTmDrv.$(LIBOBJ)

### handle the Cpss Enabler files ###
CPSS_ENABLER_FILES = $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/cpssEnabler.$(LIBOBJ)

#For simulation only
include $(USER_BASE)/simulationCommon.mk


#
# check if we have extension libraries/objects
#
ifneq (, $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainPpDrv.$(LIBEXT)))

  ifeq (visual,$(TOOL))
    FILE_EXT = lib
    EXTERN_SYMBOL = /INCLUDE:_
  else
    FILE_EXT = o
    ## use this for GNU and DIAB linkers
    EXTERN_SYMBOL = -u _
  endif
  
    #the linker will add those libs,objects to image
    ifneq (, $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainPpDrv.$(LIBEXT)))
      PP_LIBS  += $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainPpDrv.$(LIBEXT))
    endif
    ifneq (, $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainGaltisWrapper.$(FILE_EXT)))
      GALTIS_FILES_EXT += $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainGaltisWrapper.$(FILE_EXT))
    endif
    ifneq (, $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainLuaWrapper.$(FILE_EXT)))
      LUA_WRAPPER_FILES += $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainLuaWrapper.$(FILE_EXT))
    endif
    ifneq (, $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/cpssEnabler.$(FILE_EXT)))
      CPSS_ENABLER_FILES_EXT += $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/cpssEnabler.$(FILE_EXT))
    endif
    ifneq (, $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainU*.$(LIBEXT)))
      UTF_LIB += $(wildcard $(COMPILATION_ROOT_FIX)/extension/*/*/libs/$(CPU_DIR)/mainU*.$(LIBEXT))
      # add mainUt libs again to fix link errors of some GNU like linkers
      UTF_LIB +=$(wildcard $(COMPILATION_ROOT_FIX)/mainUT/libs/$(CPU_DIR)/mainU*.$(LIBEXT))
    endif

                    
    #set value of EXTENSION_SYMBOL in the environment variables
    # linker will add this symbol to force it into image.
    ifeq (YES, $(INC_EXTENSION_SYMBOL))
      EXTENSION_SYMBOL = extensionLib
      CFLAGS += $(EXTERN_SYMBOL)$(EXTENSION_SYMBOL)
    endif
  
    ifeq (YES, $(INC_EXTENSION_SYMBOL1))
      EXTENSION_SYMBOL1 = extensionLib1
      CFLAGS += $(EXTERN_SYMBOL)$(EXTENSION_SYMBOL1) 
    endif

    ifeq (YES, $(INC_EXTENSION_SYMBOL_G3))
      EXTENSION_SYMBOL_G3 = extensionLib_G3
      CFLAGS += $(EXTERN_SYMBOL)$(EXTENSION_SYMBOL_G3) 
    endif

      
endif 


  ifneq (YES, $(INCLUDE_UTF))
    UTF_LIB  = 
  endif


# DO NOT CHANGE libraries order
# IT IS important for successfull link
LIB_EXTRA += $(UTF_LIB) $(PP_LIBS)

#currently nothing to add as "extra libraries"
MACH_EXTRA += $(CPSS_ENABLER_FILES) \
	$(COMMON_LIBS) \
	$(UTIL_FILES) \
	$(XBAR_FILES) \
	$(GALTIS_FILES) \
	$(LUA_WRAPPER_FILES) \
	$(FA_FILES) \
	$(TM_API_FILES)

ifeq (1, $(LINUX_SIM)) 
	MACH_EXTRA +=$(SIM_LIB)
endif


ifeq (DUNE, $(XBAR_VENDOR))
ifneq (EXISTS, $(PM_FAMILY)) 
  LIB_EXTRA += $(COMPILATION_ROOT_FIX)/dune/mainDuneDrv/libs/$(CPU_DIR)/mainDuneDrv.$(LIBEXT)
  LIB_EXTRA += $(COMPILATION_ROOT_FIX)/dune/mainUT/libs/$(CPU_DIR)/mainUT.$(LIBEXT)
  MACH_EXTRA += $(COMPILATION_ROOT_FIX)/dune/sweep/libs/$(CPU_DIR)/sweep.$(LIBEXT)
  MACH_EXTRA += $(COMPILATION_ROOT_FIX)/dune/mainGaltisWrapper/libs/$(CPU_DIR)/mainGaltisWrapper.$(LIBEXT)  
else
  MACH_EXTRA += $(COMPILATION_ROOT_FIX)/mainFxDrv/libs/$(CPU_DIR)/mainFxDrv.$(LIBOBJ)
endif
endif

ifeq (RHODES, $(FPGA_TYPE))
	MACH_EXTRA +=   $(FPGA_FILES)
endif

 #LIB_EXTRA += $(COMPILATION_ROOT_FIX)/obcplib/libControl.a
 #LIB_EXTRA += $(COMPILATION_ROOT_FIX)/obcplib/libKits.a
 
