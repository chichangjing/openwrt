#
# This sharedMakefile.mk is used ONLY for Shared Memory approach.
# It is inluded into Makefile on SHARED_MEMORY=1
#

#
#	Variables declaration part which are used for libcpss.so/libhelper.so
#
CPSS_VER_MAJOR=0
CPSS_SUBVER=0.0
CPSS_VER_FULL=$(CPSS_VER_MAJOR).$(CPSS_SUBVER)

CPSS_CPSS_LIB_OBJETCTS=$(FPGA_FILES) $(UTIL_FILES) $(COMMON_LIBS) $(FA_FILES) $(XBAR_FILES) $(TM_API_FILES)


#	Cannot use $(PP_LIBS) 'cause mainPp*.$(LIBEXT) already include *FdbHash.$(LIBEXT)...
ifneq ($(NO_PP_USED),NOT_USED)
	CPSS_CPSS_LIB_OBJETCTS += $(COMPILATION_ROOT_FIX)/mainPpDrv/libs/$(CPU_DIR)/mainPpDrv.$(LIBEXT)
endif

#	Objects which are located in cpssEnabler but are the part of libcpss.so
CPSS_CPSS_LIB_OBJETCTS += $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/mainExtDrv.$(LIBEXT)
CPSS_CPSS_LIB_OBJETCTS += $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/shrMemIntraCPSSdata.$(LIBEXT)	

CPSS_HELPER_LIB_OBJETCTS =  $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/gtOs.$(LIBEXT)
CPSS_HELPER_LIB_OBJETCTS += $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/gtStack.$(LIBEXT)
CPSS_HELPER_LIB_OBJETCTS += $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/gtUtil.$(LIBEXT)
	
SCRIPTLD_ROOT=$(USER_BASE)/cpssEnabler/mainOs/src/gtOs/sharedMemory/scriptsld/
ifeq (1, $(LINUX_SIM))
    ifeq (Linux, $(OS_TARGET))
		SCRIPTLD_CPSS_LIB=$(SCRIPTLD_ROOT)/linuxSim/libcpss_ld_script.$(CPU_DIR)
		SCRIPTLD_HELPER_LIB=$(SCRIPTLD_ROOT)/linuxSim/libhelper_ld_script.$(CPU_DIR)
    else
#		??? Not supported yet!
    endif
else
    ifeq (linux, $(OS_RUN))
		SCRIPTLD_CPSS_LIB=$(SCRIPTLD_ROOT)/$(OS_RUN)/libcpss_ld_script.$(CPU)
		SCRIPTLD_HELPER_LIB=$(SCRIPTLD_ROOT)/$(OS_RUN)/libhelper_ld_script.$(CPU)
    else
#		??? Not supported yet!
    endif
endif

BUILD_MAP_FILE=-Map
MAPFILE_CPSS_LIB=$(COMPILATION_ROOT_FIX)/libcpss.map
MAPFILE_HELPER_LIB=$(COMPILATION_ROOT_FIX)/libhelper.map
	
ifeq (1,$(LINUX_SIM))
#   For simulation COMPILER can reside in any unexpected place (/usr/local/bin/, ~/bin/, etc.)
#   So don't rely on this path, use ususal convention-defined.
    XCOMP_ROOT_PATH_FIX=
    
#   Use wrappers for ALL functions from libc which are used by simulation (libcpss.so shouldn't call libc directly!)
##    SIM_LIBC_FUNCS=access __assert_fail calloc __errno_location exit fclose fgets fopen fprintf free malloc \
##		memcmp memcpy memset printf qsort rand sprintf sscanf strcat strcmp strcpy strlen strncmp strncpy \
##		strrchr strstr strtok system vsprintf

#	Use wrappers ONLY for dynamic allocation to provide shared alorithm
    SIM_LIBC_FUNCS=calloc free malloc realloc

ifeq (EXISTS,$(PM_FAMILY))
#	SIM_LIB  = $(USER_BASE)/simulation/libs/GM/Linux/libPUMA2_gm.a
	SIM_LIB  = 
	SIM_LIB += $(USER_BASE)/simulation/libs/GM/Linux/mainOs.a
#	Golden Module uses some additional libc functions
##	SIM_LIBC_FUNCS += alarm atexit chdir closedir fflush getcwd getenv getwd isspace log10 lseek memmove nanosleep open opendir popen \
##					  pow pthread_mutex_lock pthread_mutex_unlock  putchar puts raise readdir setvbuf signal srand time write 
	
#   mainOs.a conflicts with gm98EX24X.a !!!
	SIM_DROP_MODULES=gtMath.o lkernelLib.o lmsgQLib.o lsemLib.o ltaskLib.o lwdLib.o
	SIM_LIB_ARCH=$(USER_BASE)/simulation/libs/GM/Linux/libPUMA2_gm.a
	SIM_LIB_SHARED = $(USER_BASE)/simulation/libs/GM/Linux/libPUMA2_gm.a
	LIB_EXTRA += \
	    $(USER_BASE)/simulation/libs/GM/Linux/libnsa_socket_driver.a \
		$(USER_BASE)/simulation/libs/GM/Linux/libidtplatform.a \
		$(USER_BASE)/simulation/libs/GM/Linux/libidtresource.a \
		$(USER_BASE)/simulation/libs/GM/Linux/libidtutility.a
	CPLUSPLUS_LIB_REQUIRED=yes
endif
ifeq (EXISTS, $(FORCE_GM_USE))
	LIB_EXTRA += $(USER_BASE)/simulation/libs/GM/Linux/PpInterfaceLibs/lionB/liblionB_gm.a
	CPLUSPLUS_LIB_REQUIRED=yes
endif

    SIM_LIBC_WRAPPERS = $(foreach func,$(SIM_LIBC_FUNCS),--wrap $(func) )
ifneq (,$(LIBC_A))
	SIM_STATIC_LIB_STAT_MODULE=$(COMPILATION_ROOT_FIX)/stat.o
endif
else
#   For BM doesn't fix path
    XCOMP_ROOT_PATH_FIX=$(XCOMP_ROOT_PATH)
ifeq (arm, $(CPU_TOOL))
#	linker flags for BlackMode with Shared memory (used for shared libraries)
ifeq (,$(filter 4.%,$(GCC_VER)))
	BM_SHM_LDFLAGS=-EL -ldl -Bsymbolic-functions
else
	BM_SHM_LDFLAGS=-Xlinker -EL -ldl -Xlinker -Bsymbolic-functions
endif
endif
ifdef USE_GNUEABI
#	Some arch-specific routines are located into additional shared libraries
	BM_SHM_ADDITIONAL_ARCH_SO_DEPS=$(XCOMP_ROOT_PATH_FIX)/../lib/libgcc_s.so
endif
endif

# Note! We should implement more elaborate resolution of standard libraries names to accept newer version
# not less than current usable.
#ifneq (,$(wildcard $(XCOMP_ROOT_PATH_FIX)/lib/*linux*/libpthread.so.0))
#  HELPER_USE_VERSION_FROM_LIBS=$(wildcard $(XCOMP_ROOT_PATH_FIX)/lib/*linux*/libpthread.so.0) \
#        $(wildcard $(XCOMP_ROOT_PATH_FIX)/lib/*linux*/libc.so.6)
#else
#  HELPER_USE_VERSION_FROM_LIBS=$(XCOMP_ROOT_PATH_FIX)/lib/libpthread.so.0 $(XCOMP_ROOT_PATH_FIX)/lib/libc.so.6
#endif
HELPER_USE_VERSION_FROM_LIBS=-lc -lpthread

#
#	Variables declaration part which are used for CPSS Enabler
#
CPSS_ENABLER_OBJECTS = 
#$(COMPILATION_ROOT_FIX)/mainGaltisWrapper/libs/$(CPU_DIR)/mainGaltisWrapper.o
ifneq (yes, $(CMD_LUA_CLI))
#CPSS_ENABLER_OBJECTS+=$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/mainCmd.$(LIBEXT)
endif
#CPSS_ENABLER_OBJECTS+=$(wildcard $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/luaCLI.$(LIBEXT))
CPSS_ENABLER_OBJECTS+=$(wildcard $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/confi.$(LIBEXT))
CPSS_ENABLER_OBJECTS+=$(wildcard $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/extPhyM.$(LIBEXT))
CPSS_ENABLER_APP_DEMO_CPU_ETH_PORT_MANAGEOBJECTS+=$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/mainSysConfig.$(LIBEXT)

#	Cannot use $(UTF_LIB) 'cause $(wildcard ) doesn't work for non-existent files
#	(they will be built only as a result of this make into 'cpss' target)
ifeq (YES,$(INCLUDE_UTF))
	CPSS_ENABLER_OBJECTS += $(COMPILATION_ROOT_FIX)/mainUT/libs/$(CPU_DIR)/mainUT.$(LIBEXT)
endif

ifeq (yes, $(CMD_LUA_CLI))
    CPSS_ENABLER_LUA_WRAPPERS = \
        -Xlinker --whole-archive \
    		$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/ed.$(LIBEXT) \
    		$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/mainCmd.$(LIBEXT) \
    		$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/luaCLI.$(LIBEXT) \
            $(COMPILATION_ROOT_FIX)/mainLuaWrapper/libs/$(CPU_DIR)/mainLuaWrapper.$(LIBEXT) \
        -Xlinker --no-whole-archive
endif

ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
CPSS_ENABLER_OBJECTS+=$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/eventsEmulation.$(LIBEXT)
endif

#   Including some libraries several times to resolve depencies
ifneq (yes, $(CMD_LUA_CLI))
CPSS_ENABLER_OBJECTS+=$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/mainCmd.$(LIBEXT)
endif
CPSS_ENABLER_OBJECTS+=$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/mainSysConfig.$(LIBEXT)
ifneq (yes, $(CMD_LUA_CLI))
CPSS_ENABLER_OBJECTS+=$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/mainCmd.$(LIBEXT)
endif
#CPSS_ENABLER_OBJECTS+=$(wildcard $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/luaCLI.$(LIBEXT))
CPSS_ENABLER_OBJECTS+=$(wildcard $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/confi.$(LIBEXT))
CPSS_ENABLER_OBJECTS+=$(wildcard $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/extPhyM.$(LIBEXT))

#	Clients shouldn't copy relocatable objects from .so libraries!
CFLAGS_NO_COPY_RELOC=-Xlinker -z -Xlinker nocopyreloc 
ifeq (EXISTS, $(PM_FAMILY))
CFLAGS_NO_COPY_RELOC+=-DEXMXPM_FAMILY
endif
ifeq (EXISTS, $(CHX_FAMILY))
CFLAGS_NO_COPY_RELOC+=-DCHX_FAMILY
endif

#   Variables for test clients (to be built as separate executables)
BUILD_SHMT_PATH=cpssEnabler/mainOs/src/gtOs/sharedMemory/testClients
SHMT_INCLUDE=-IcpssEnabler/mainOs/h/ -Icommon/h/ -ImainPpDrv/h/ -IcpssEnabler/mainExtDrv/h/
SHMT_INCLUDE+=-IcpssEnabler/mainSysConfig/h/
SHMT_INCLUDE+=-IcpssEnabler/mainCmd/h
SHMT_INCLUDE+=-ImainExtUtils/h
# to be removed???
SHMT_INCLUDE+=-ImainGaltisWrapper/h/

SHMT_LDFLAGS=-lpthread -lrt -ldl -lm

ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
SHMT_LDFLAGS+=-DAPP_DEMO_CPU_ETH_PORT_MANAGE 
endif

ifeq (yes, $(CPLUSPLUS_LIB_REQUIRED))
#	Goden Module (ASIC simulation for PUMA, lion) uses C++, use stdc++ library
	SHMT_LDFLAGS+= -lstdc++
endif

ifeq (1,$(LINUX_SIM))
ifeq (,$(filter 4.%,$(GCC_VER)))
    LDSHLIB=$(LD) -shared $(LDFLAGS)
    XLINKER=
else
    LDSHLIB=$(CC) -shared $(CFLAGS2)
    XLINKER=-Xlinker
endif
endif

ifneq (,$(wildcard $(XCOMP_ROOT_PATH_FIX)/usr/lib/*-linux-*))
#  LDFLAGS := -L$(wildcard $(XCOMP_ROOT_PATH_FIX)/usr/lib/*-linux-*) $(LDFLAGS)
endif

#
# Following targets are used only for shared memory approach
#

# Note - shared Makefile has own 'all' target.
SHARED_APPS = fdbLearning RxTxProcess
ifeq (yes,$(LUACLI_STANDALONE))
SHARED_APPS += luaCLI
endif

all : cpss_shared application_shared $(SHARED_APPS)
	
ifneq (,$(wildcard $(XCOMP_ROOT_PATH_FIX)/usr/lib/*linux*/libc.a))
  LIBC_A = $(wildcard $(XCOMP_ROOT_PATH_FIX)/usr/lib/*linux*/libc.a)
else
  LIBC_A = $(wildcard $(XCOMP_ROOT_PATH_FIX)/usr/lib/libc.a)
endif
# additional steps for CPSS - build DSOs
cpss_shared:
#	Shared library version should also build .so libraries
	$(RM) -f $(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) $(COMPILATION_ROOT_FIX)/libcpss.o.$(CPSS_VER_FULL) \
		$(COMPILATION_ROOT_FIX)/libcpss.so
ifeq (1,$(LINUX_SIM))
ifeq (EXISTS,$(PM_FAMILY))
	# !! waring - gm98EX24X.a uses private v2linux library (without shared ability).
	# Drop it and use v2linux from libhelper.so !
	#$(COPY) $(SIM_LIB_ARCH) $(SIM_LIB_SHARED)
	#$(AR) d $(SIM_LIB_SHARED) $(SIM_DROP_MODULES)
endif
	$(LDSHLIB) -fpic -fPIC $(foreach word,$(SIM_LIBC_WRAPPERS),$(XLINKER) $(word)) \
		$(XLINKER) --whole-archive \
			$(CPSS_CPSS_LIB_OBJETCTS) \
			$(COMPILATION_ROOT_FIX)/simulation/libs/$(CPU_DIR)/simulation.$(LIBEXT) \
			$(SLAN_LIB) \
		$(XLINKER) -no-whole-archive  \
		$(XLINKER) $(BUILD_MAP_FILE) $(XLINKER) $(MAPFILE_CPSS_LIB) \
		-T $(SCRIPTLD_CPSS_LIB) $(XLINKER) -soname $(XLINKER) libcpss.so.$(CPSS_VER_MAJOR) \
		-o $(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_FULL)
else
ifeq (,$(filter 4.%,$(GCC_VER)))
	# GCC_VER != 4.*
	$(LD) -shared $(BM_SHM_LDFLAGS) --whole-archive $(CPSS_CPSS_LIB_OBJETCTS) -no-whole-archive	\
		$(BUILD_MAP_FILE) $(MAPFILE_CPSS_LIB) -T $(SCRIPTLD_CPSS_LIB)  \
		-soname libcpss.so.$(CPSS_VER_MAJOR) \
		-o $(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_FULL) $(BM_SHM_ADDITIONAL_ARCH_SO_DEPS)
else
	# GCC_VER == 4.*
	$(CC) -shared $(CC_ARCH) $(BM_SHM_LDFLAGS) \
		-Xlinker --whole-archive $(CPSS_CPSS_LIB_OBJETCTS) -Xlinker --no-whole-archive	\
		$(addprefix -Xlinker , \
		    $(BUILD_MAP_FILE) $(MAPFILE_CPSS_LIB) \
			-T $(SCRIPTLD_CPSS_LIB)  \
		    -soname libcpss.so.$(CPSS_VER_MAJOR)) \
 		-o $(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_FULL) $(BM_SHM_ADDITIONAL_ARCH_SO_DEPS)
endif
endif
	ln -s libcpss.so.$(CPSS_VER_FULL) $(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR)

	$(RM) -f $(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) $(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_FULL) \
		$(COMPILATION_ROOT_FIX)/libhelper.so
ifeq (1,$(LINUX_SIM))
#	stat routine cannot be resolved dynamically for i386(used only for simulation), link it into .so object
#	Note - change dir to output directory to make file there
ifneq (,$(LIBC_A))
	cd $(COMPILATION_ROOT_FIX); $(AR) x $(LIBC_A) stat.o
endif
endif
#	link with libpthread.so to resolve correct version of glibc
#	(pthread_cond_init@@GLIBC_2.3.2 should be used)!
ifeq (,$(filter 4.%,$(GCC_VER)))
	# GCC_VER != 4.*
	$(LD) -shared $(BM_SHM_LDFLAGS) --whole-archive $(CPSS_HELPER_LIB_OBJETCTS) -no-whole-archive \
		$(SIM_STATIC_LIB_STAT_MODULE) $(HELPER_USE_VERSION_FROM_LIBS) \
		-T $(SCRIPTLD_HELPER_LIB) $(BUILD_MAP_FILE) $(MAPFILE_HELPER_LIB) \
		-soname libhelper.so.0 -o $(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_FULL) \
		$(BM_SHM_ADDITIONAL_ARCH_SO_DEPS)
else
	# GCC_VER == 4.*
	$(CC) -shared $(CC_ARCH) $(CFLAGS2) $(BM_SHM_LDFLAGS) \
		-Xlinker --whole-archive $(CPSS_HELPER_LIB_OBJETCTS) -Xlinker --no-whole-archive \
 		$(SIM_STATIC_LIB_STAT_MODULE) $(HELPER_USE_VERSION_FROM_LIBS) \
		$(addprefix -Xlinker , \
		    -T $(SCRIPTLD_HELPER_LIB) \
			$(BUILD_MAP_FILE) $(MAPFILE_HELPER_LIB) \
		    -soname libhelper.so.0) \
		-o $(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_FULL) \
 		$(BM_SHM_ADDITIONAL_ARCH_SO_DEPS)
endif
	ln -s libhelper.so.$(CPSS_VER_FULL) $(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR)
	ln -s libhelper.so.$(CPSS_VER_MAJOR) $(COMPILATION_ROOT_FIX)/libhelper.so

ifneq (1, $(LINUX_SIM))
# If no simulation then we use BM target to build CPSS Enabler
application_shared : appDemo_shared
appDemo_shared : cpss_shared
	echo " begin appDemo for shared library"
	cd  ${COMPILATION_ROOT_FIX}
	rm -f ${COMPILATION_ROOT_FIX}/appDemo
	
#   Shared memory approach uses dynamic libraries libcpss.so and libhelper.so
#	$(CC) $(CC_ARCH) -o ${COMPILATION_ROOT_FIX}/appDemo $(CFLAGS) $(CFLAGS_NO_COPY_RELOC) \
		$(CPSS_ENABLER_OBJECTS) $(CPSS_ENABLER_LUA_WRAPPERS) \
		$(LDFLAGS) \
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) -lrt -lm \
		-Xlinker $(BUILD_MAP_FILE) -Xlinker ${COMPILATION_ROOT_FIX}/appDemo.map
ifneq (NO, $(LINUX_BUILD_KERNEL))
#   Copy libraries to /usr/lib/ into image directory
	cp $(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_FULL) $(HOME_INITRD)/usr/lib/
	cp $(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_FULL) $(HOME_INITRD)/usr/lib/
	ln -sf libcpss.so.$(CPSS_VER_FULL) $(HOME_INITRD)/usr/lib/libcpss.so.$(CPSS_VER_MAJOR)
	ln -sf libhelper.so.$(CPSS_VER_FULL) $(HOME_INITRD)/usr/lib/libhelper.so.$(CPSS_VER_MAJOR)
	echo -e "cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo"
	cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo
	#echo -e "\t"$(STRIP)" "$(HOME_INITRD)"/usr/bin/appDemo"
	$(STRIP) $(HOME_INITRD)/usr/bin/appDemo
	chmod 777 $(HOME_INITRD)/usr/bin/appDemo
	$(STRIP) $(HOME_INITRD)/usr/lib/libcpss.so.$(CPSS_VER_FULL)
	$(STRIP) $(HOME_INITRD)/usr/lib/libhelper.so.$(CPSS_VER_FULL)
endif

# else of ifneq (1, $(LINUX_SIM))
else
	
# If simulation is used then we use WM target to build CPSS Enabler
application_shared: appDemoSim_shared
appDemoSim_shared : cpss_shared
#   Shared memory approach uses dynamic libraries libcpss.so and libhelper.so
	$(CC) $(CFLAGS) $(CFLAGS_NO_COPY_RELOC) \
		$(CPSS_ENABLER_OBJECTS) $(CPSS_ENABLER_LUA_WRAPPERS) \
		-Wl,-rpath,$(COMPILATION_ROOT_FIX) \
		-Xlinker $(BUILD_MAP_FILE) -Xlinker ${COMPILATION_ROOT_FIX}/appDemoSim.map \
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) \
		$(LDFLAGS) -o $(COMPILATION_ROOT_FIX)/$@ 
	echo "appDemoSim for shared library created"

# end of ifneq (1, $(LINUX_SIM))
endif

testClients:
ifeq (1,$(INCLUDE_SHMT))
#   Target to build test clients for shared memory approach
#   Applicable ONLY for SHARED_MEMORY=1
#   Build both clients:
	$(CC) $(CFLAGS) $(CFLAGS_NO_COPY_RELOC) $(BUILD_SHMT_PATH)/shmtClient1.c	\
		$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/sharedMemoryUT.$(LIBEXT) 	\
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) \
		-o $(COMPILATION_ROOT_FIX)/shmtClient1 $(SHMT_INCLUDE) $(SHMT_LDFLAGS)
	$(CC) $(CFLAGS) $(CFLAGS_NO_COPY_RELOC) $(BUILD_SHMT_PATH)/shmtClient2.c	\
		$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/sharedMemoryUT.$(LIBEXT)	\
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) \
		-o $(COMPILATION_ROOT_FIX)/shmtClient2 $(SHMT_INCLUDE) $(SHMT_LDFLAGS)
	echo "testClients created"
else        
	echo testClients NOT used in configuration use INCLUDE_SHMT to switch ON 
endif

ifeq ($(ENDIAN), )
ENDIAN = LE
endif
fdbLearning: cpss_shared
ifeq (1,$(INCLUDE_SHMT))
#   Target to build test clients for shared memory approach
#   Applicable ONLY for SHARED_MEMORY=1
#   Build both clients:
	$(CC) $(CFLAGS) $(CFLAGS2) -g $(CFLAGS_NO_COPY_RELOC) \
		-Wl,-rpath,$(COMPILATION_ROOT_FIX) \
		$(addprefix -D,$(sort CPU=$(CPU) $(CPU_FAMILY) CPU_$(ENDIAN) $(OS_DEFS) $(PROJECT_DEFS))) \
		$(BUILD_SHMT_PATH)/fdbLearning.c	\
		-Xlinker $(BUILD_MAP_FILE) -Xlinker ${COMPILATION_ROOT_FIX}/fdbLearning.map \
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) \
		-o $(COMPILATION_ROOT_FIX)/fdbLearning $(SHMT_INCLUDE) $(SHMT_LDFLAGS)
    ifneq (1, $(LINUX_SIM))
    ifneq (NO, $(LINUX_BUILD_KERNEL))
		echo -e "cp ${COMPILATION_ROOT_FIX}/fdbLearning $(HOME_INITRD)/usr/bin/fdbLearning"
		cp ${COMPILATION_ROOT_FIX}/fdbLearning $(HOME_INITRD)/usr/bin/fdbLearning
		$(STRIP) $(HOME_INITRD)/usr/bin/fdbLearning
    endif
    endif

	echo "fdbLearning created"

#-----------------------------------------------------------------------------------------

    ifneq (1, $(LINUX_SIM))
    ifneq (NO, $(LINUX_BUILD_KERNEL))
	# copy librt.so.1 as symlink
	( cd $(XCOMP_ROOT_PATH)/lib$(LIB_1) && cp -P librt.so.1 librt-2.*so $(HOME_INITRD)/lib )
	#cp -f $(XCOMP_ROOT_PATH)/lib$(LIB_1)/librt.so.1 $(HOME_INITRD)/lib
	#cp -f $(XCOMP_ROOT_PATH)/lib$(LIB_1)/librt-2.*so $(HOME_INITRD)/lib
    endif
    endif

#-----------------------------------------------------------------------------------------

else        
	echo testClients NOT used in configuration use INCLUDE_SHMT to switch ON 
endif

luaCLI: cpss_shared
	$(CC) $(CFLAGS) $(CFLAGS_NO_COPY_RELOC) \
		-Xlinker --whole-archive \
		    $(COMPILATION_ROOT_FIX)/luaCLI_standalone/libs/$(CPU_DIR)/luaCLI_standalone.$(LIBEXT) 	\
		-Xlinker --no-whole-archive \
		-Wl,-rpath,$(COMPILATION_ROOT_FIX) \
		-Xlinker $(BUILD_MAP_FILE) -Xlinker ${COMPILATION_ROOT_FIX}/$@.map \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(LDFLAGS) -o $(COMPILATION_ROOT_FIX)/$@ -lm
ifneq (1, $(LINUX_SIM))
ifneq (NO, $(LINUX_BUILD_KERNEL))
	echo -e "cp ${COMPILATION_ROOT_FIX}/luaCLI $(HOME_INITRD)/usr/bin/luaCLI"
	cp ${COMPILATION_ROOT_FIX}/luaCLI $(HOME_INITRD)/usr/bin/luaCLI
	$(STRIP) $(HOME_INITRD)/usr/bin/luaCLI
endif
endif
	echo "luaCLI created"


#$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/sharedMemoryUT.$(LIBEXT) 	\

EVENT_EMUL_LIB=
ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
EVENT_EMUL_LIB=$(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/eventsEmulation.$(LIBEXT)
endif

RxTxProcess: $(COMPILATION_ROOT_FIX)/RxTxProcess
ifeq (EXISTS,$(PM_FAMILY))
RXTX_PROCESS_FILE=RxTxProcess_PM.c
endif
ifeq (EXISTS,$(CHX_FAMILY))
RXTX_PROCESS_FILE=RxTxProcess.c
endif

$(COMPILATION_ROOT_FIX)/RxTxProcess: cpss_shared
ifeq (1,$(INCLUDE_SHMT))
#   Target to build test clients for shared memory approach
#   Applicable ONLY for SHARED_MEMORY=1
#   Build both clients:

	$(CC) $(CFLAGS) $(CFLAGS2)  $(CFLAGS_NO_COPY_RELOC) \
		-Wl,-rpath,$(COMPILATION_ROOT_FIX) \
		$(addprefix -D,$(sort CPU=$(CPU) $(CPU_FAMILY) CPU_$(ENDIAN) $(OS_DEFS) $(PROJECT_DEFS))) \
		$(BUILD_SHMT_PATH)/$(RXTX_PROCESS_FILE)	\
		-Xlinker $(BUILD_MAP_FILE) -Xlinker $@.map \
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) \
		$(EVENT_EMUL_LIB) \
		-o $@ $(SHMT_INCLUDE) $(SHMT_LDFLAGS)
    ifneq (1, $(LINUX_SIM))
    ifneq (NO, $(LINUX_BUILD_KERNEL))
		echo -e "cp $@ $(HOME_INITRD)/usr/bin/RxTxProcess"
		cp $@ $(HOME_INITRD)/usr/bin/RxTxProcess
		$(STRIP) $(HOME_INITRD)/usr/bin/RxTxProcess
    endif
    endif
	echo "RxTxProcess created"
else        
	echo testClients NOT used in configuration use INCLUDE_SHMT to switch ON 
endif


LUA_LIB=$(USER_BASE)/liblua.a
LUA_H_PATH=-I/localtempstore/work/lua/lua-5.1/src

luaProcess:
#ifeq (1,$(INCLUDE_SHMT))
ifeq (EXISTS,$(LUA_PROCESS))
#   Target to build test clients for shared memory approach
#   Applicable ONLY for SHARED_MEMORY=1
#   Build both clients:
	$(CC) $(CFLAGS) -g $(CFLAGS_NO_COPY_RELOC) $(BUILD_SHMT_PATH)/luaProcess.c	\
		-Xlinker $(BUILD_MAP_FILE) -Xlinker ${COMPILATION_ROOT_FIX}/luaProcess.map \
		$(LUA_LIB) $(LUA_H_PATH) \
		-lm \
		$(COMPILATION_ROOT_FIX)/libcpss.so.$(CPSS_VER_MAJOR) \
		$(COMPILATION_ROOT_FIX)/libhelper.so.$(CPSS_VER_MAJOR) \
		-o $(COMPILATION_ROOT_FIX)/luaProcess $(SHMT_INCLUDE) $(SHMT_LDFLAGS)
    ifneq (1, $(LINUX_SIM))
    ifneq (NO, $(LINUX_BUILD_KERNEL))
		echo -e "cp ${COMPILATION_ROOT_FIX}/fdbLearning $(HOME_INITRD)/usr/bin/fdbLearning"
		cp ${COMPILATION_ROOT_FIX}/luaProcess $(HOME_INITRD)/usr/bin/luaProcess
    endif
    endif

	echo "luaProcess created"

else        
	echo testClients NOT used in configuration use INCLUDE_SHMT to switch ON 
endif
#endif

