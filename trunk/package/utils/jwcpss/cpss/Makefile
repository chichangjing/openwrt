#*******************************************************************************
#                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
# THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
# NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
# OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
# DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
# THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
# IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
#                                                                              *
# MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
# MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
# SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
# (MJKK), GALILEO TECHNOLOGY LTD. (GTL) ,GALILEO TECHNOLOGY, INC. (GTI). and   *
# RADLAN COMPUTER COMMUNICATIONS LTD. (RND).
#*******************************************************************************
# Makefile for Linux and uClinux
#
# DESCRIPTION:
#       This file contains rules for building PSS for
#           Linux and uClinux on the specified board
#           using the tool chain environment. (refer to LSP manual.)
#
#*******************************************************************************

######### Debug level selector #########
DEBUG_AND_OPTIMIZE_LEVEL	= -O0 -g
##DEBUG_AND_OPTIMIZE_LEVEL	= -O2

#the compilation of the 'RTOS on SIM' should be distinguished from 'RTOS'(non simulation)
ifeq (EXISTS, $(RTOS_ON_SIM))
	TMP_POSTFIX=_SIM
else
	TMP_POSTFIX=
endif

## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.
## Should update the kernel
VENDOR		= Marvell
HOME_ROMFS	= $(DIST_HOME)/romfs
HOME_ZIMAGE	= $(DIST_HOME)/$(KERNELVER)/arch/armnommu/boot
HOME_INITRD = $(DIST_HOME)/rootfs

RELEASE		= application

#specify extra components & include path
EXTRA_INCLUDE =
LIB_EXTRA =
MACH_EXTRA =

#specify components for cpss as lib 
CPSS_LIB = cpssDrvLib.a

#Shared libraries 
CPSS_LIB_SO = cpssDrvLib.so
CPSS_ENABLER_LIB_SO = cpssEnabler.so
MAIN_UT_LIB_SO = mainUT.so
MAIN_LUA_LIB_SO = mainLUA.so
MAIN_GALTIS_LIB_SO = mainGaltis.so
SIMULATION_LIB_SO = simulation.so

COMMON_LIBS_LB = 
MACH_EXTRA_LB =
LIB_EXTRA_LB =

include $(USER_BASE)/cpssCommon.mk

# DO NOT CHANGE libraries order
# IT IS important for successfull link
LIB_EXTRA_LB += $(UTF_LIB) $(COMPILATION_ROOT_FIX)/$(CPSS_LIB)

#add simulation libraries - must be 3 times for link pass
ifeq (1, $(LINUX_SIM))

LIB_EXTRA += $(SIM_LIB)
LIB_EXTRA += $(SIM_LIB)
LIB_EXTRA += $(SIM_LIB)
LIB_EXTRA += $(SIM_EXTRA)

endif


#specify files for cpss as lib 
#MACH_EXTRA_LB += $(COMPILATION_ROOT_FIX)/cpssEnabler/libs/$(CPU_DIR)/cpssEnabler.o 
MACH_EXTRA_LB += $(CPSS_ENABLER_FILES) \
	$(UTIL_FILES) \
	$(GALTIS_FILES) \
	$(LUA_WRAPPER_FILES) \
	$(TM_API_FILES)

CFLAGS = $(CC_ARCH)
ifeq (D_ON,$(DEBUG_INFO))
  CFLAGS += $(DEBUG_AND_OPTIMIZE_LEVEL)
endif

#Set linker flags
ifeq (FreeBSD, $(OS_TARGET))
ifeq (4, $(OS_TARGET_RELEASE))
LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib -pthread -rdynamic
else
LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib -lpthread -rdynamic
endif
endif

ifeq (linux, $(OS_RUN))
   ifeq (Linux, $(OS_TARGET))
      ifeq (YES,$(IS_64BIT_OS))                          # new linux platform - 64 bit
         ifeq (32,$(WORDSIZE_REQUIRED))                  # build 32 bits on 64 bits platform 
            CFLAGS2 += -m32
            LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib32 -lpthread -ldl -rdynamic -m32 
            # if $(GCC_VER <= 4.6.*)
            ifeq (, $(filter-out 3.% 4.0.% 4.1.% 4.2.% 4.3.% 4.4.% 4.5.% 4.6.%,$(GCC_VER)))
                LDFLAGS += -melf_i386
            endif
         else                                            # build 64 bits on 64 bits platform
            CFLAGS2 += -D__WORDSIZE=64
            LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib${LIB_1} -lpthread -ldl -rdynamic
         endif
      else                                               # build 32 bits on 32 bits platform
         LDFLAGS = -L$(XCOMP_ROOT_PATH)/usr/lib -lpthread -ldl -rdynamic
      endif
      ifeq (PPC603, $(CPU))
         LDFLAGS = -L$(XCOMP_ROOT_PATH)/lib -L$(XCOMP_ROOT_PATH)/usr/lib \
				   -Xlinker -rpath-link -Xlinker $(XCOMP_ROOT_PATH)/lib \
				   -pthread -ldl -rdynamic $(XCOMP_ROOT_PATH)/usr/lib/libm.a
      endif
      ifeq (PPC85XX, $(CPU))
         LDFLAGS += $(XCOMP_ROOT_PATH)/lib/ld.so.1
      endif
   endif

   ifeq (1, $(LINUX_SIM))
      ifneq (EXISTS, $(RTOS_ON_SIM))
         ifeq (EXISTS, $(FORCE_GM_USE))
            LDFLAGS += -lstdc++
         endif
         ifneq (FreeBSD, $(OS_TARGET))
            LDFLAGS += -lrt
         endif
      endif
   else
    LDFLAGS += $(XCOMP_ROOT_PATH)${XCOMP_ROOT_PATH_1}/usr/lib${LIB_1}/librt.a
   endif
endif


ifeq (yes, $(CMD_LUA_CLI))
   LDFLAGS += -lm
endif

ifeq (EXISTS, $(RTOS_ON_SIM))
  LDFLAGS += $(XCOMP_ROOT_PATH)${XCOMP_ROOT_PATH_1}/usr/lib${LIB_1}/librt.a
endif

ifeq (FreeBSD, $(OS_RUN))
  ifneq (1, $(LINUX_SIM))
	LDFLAGS += -Wl,-E
  else
    ifneq (EXISTS, $(RTOS_ON_SIM))
      ifeq (EXISTS, $(FORCE_GM_USE))
        LDFLAGS += -lstdc++
      endif
    endif
  endif
endif

# define object files get to cpss lib
OBJ_FILES_01 = $(COMPILATION_ROOT_FIX)/common/objTemp/$(CPU_DIR)/*.o
OBJ_FILES_02 = $(COMPILATION_ROOT_FIX)/mainPpDrv/objTemp/$(CPU_DIR)/*.o
OBJ_FILES_03 = $(COMPILATION_ROOT_FIX)/mainTmDrv/objTemp/$(CPU_DIR)/*.o

#For creation of shared objects
CPSS_ENABLER_OBJ  = $(COMPILATION_ROOT_FIX)/cpssEnabler/objTemp/$(CPU_DIR)/*.o 
MAIN_UT_OBJ  = $(COMPILATION_ROOT_FIX)/mainUT/objTemp/$(CPU_DIR)/*.o 
MAIN_LUA_OBJ = $(COMPILATION_ROOT_FIX)/mainLuaWrapper/objTemp/$(CPU_DIR)/*.o
MAIN_GAL_OBJ = $(COMPILATION_ROOT_FIX)/mainGaltisWrapper/objTemp/$(CPU_DIR)/*.o
SIM_OBJ      = $(COMPILATION_ROOT_FIX)/simulation/objTemp/$(CPU_DIR)/*.o
SIM_LIB_EXTRA += ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(SIMULATION_LIB_SO)

ifeq (YES, $(CPP_LINK))
    LINKER=$(GPP)
else
    LINKER=$(CC)
endif

#Set build all rules
ifeq (1, $(SHARED_MEMORY))
# Defines the specific declarations and targets for "shared memory" CPSS and libs 
include sharedMemMakefile.mk
else
all : cpss $(DUNE_EXTRA_TARGETS) application
endif

#Build cpss lib and appDemo
all_lb: cpss cpss_lib appDemo_lb

#build cpss lib only
cpss_lb: cpss cpss_lib

ifneq (YES, $(SO_LIB))
ifneq (1, $(LINUX_SIM))
application : appDemo
else
application : appDemoSim
endif
else
application : so_lib
endif

#Build CPSS
cpss :
	echo " PREFIX - ${COMP_PREFIX}"
	$(MAKE) -f presteraTopMake full

cpss_lib:
	echo "Building cpss_lib"
	$(AR) crus $(COMPILATION_ROOT_FIX)/$(CPSS_LIB) $(OBJ_FILES_02)
	$(AR) rcs $(COMPILATION_ROOT_FIX)/$(CPSS_LIB) $(OBJ_FILES_01)

so_lib:
	echo "Building so_lib"
	mkdir -p ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)
#	$(CC) -c -o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/main.o $(USER_BASE)/cpssEnabler/mainSysConfig/src/appDemo/entryPoints/LinuxSim/so_main.c	
	$(CC) -shared -o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(CPSS_LIB_SO) $(OBJ_FILES_02) $(OBJ_FILES_01)
#	$(CC) -shared -o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(MAIN_UT_LIB_SO) $(MAIN_UT_OBJ)
#	$(CC) -shared -o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(MAIN_LUA_LIB_SO) $(MAIN_LUA_OBJ)
#	$(CC) -shared -o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(MAIN_GALTIS_LIB_SO) $(MAIN_GAL_OBJ)

	$(CC) -shared -o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(CPSS_ENABLER_LIB_SO) $(CPSS_ENABLER_OBJ)
ifeq (1, $(LINUX_SIM))
	echo "Creating simulation lib"
	$(CC) -shared -o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(SIMULATION_LIB_SO) $(SIM_OBJ)	
endif
	$(CC) -o  ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/appDemoSimSo ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/main.o ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(CPSS_LIB_SO) ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(CPSS_ENABLER_LIB_SO) ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(MAIN_GALTIS_LIB_SO) ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(MAIN_LUA_LIB_SO) ${COMPILATION_ROOT_FIX}/SO_LIB/$(CPU_DIR)/$(MAIN_UT_LIB_SO) $(SIM_LIB_EXTRA) -lpthread -lm -ldl -lrt -melf_i386


cpss_blob:
	echo "Building cpss_blob"
	$(LD) -r $(BUILD_MAP_FILE) $(COMPILATION_ROOT_FIX)/cpss_blob.map $(FO) $(COMPILATION_ROOT_FIX)/cpss_blob.o $(OBJ_FILES_01) $(OBJ_FILES_02) $(OBJ_FILES_03)
	$(NM) $(COMPILATION_ROOT_FIX)/cpss_blob.o \
		| sed -n 's/^ *U //p' | grep -v -e '^__aeabi_' -e '^memcpy$$' -e '^memset$$' \
		| sort -u > $(COMPILATION_ROOT_FIX)/unresolved_symbols.txt
	test \! -s $(COMPILATION_ROOT_FIX)/unresolved_symbols.txt

#Build appDemo
appDemo :
	echo " begin appDemo"
	cd  ${COMPILATION_ROOT_FIX}
	rm -f ${COMPILATION_ROOT_FIX}/appDemo
ifeq (FreeBSD, $(OS_RUN))
	ppc-fbsd-e500-ranlib $(LIB_EXTRA)
endif
ifneq (YES,$(STATIC_LINKED_APPDEMO))
	echo "link.sh $(LINKER) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) $(MACH_EXTRA) $(LIB_EXTRA) $(LDFLAGS)"
	./link.sh $(LINKER) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) $(MACH_EXTRA) $(LIB_EXTRA) $(LDFLAGS)
else
	echo "int __SymbolTable;int __SymbolTableLength; "> $(COMPILATION_ROOT_FIX)/symtable.c
	./link.sh $(LINKER) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) -static $(MACH_EXTRA) $(COMPILATION_ROOT_FIX)/symtable.c $(LIB_EXTRA) $(LDFLAGS)
	# generate map file
	# 1. Generate list
	# x86_64
	$(NM) ${COMPILATION_ROOT_FIX}/appDemo |sed -n 's/^.* T \(.*\)$$/\1/p'|LANG=C sort -u >$(COMPILATION_ROOT_FIX)/symtable.list
	cat ${COMPILATION_ROOT_FIX}/symtable.list |sed -n 's/^\(.*\)$$/int \1();/p' >$(COMPILATION_ROOT_FIX)/symtable.c
	echo "struct { const char *name; int (*funcptr)();} __SymbolTable[] = {" >>$(COMPILATION_ROOT_FIX)/symtable.c
	cat ${COMPILATION_ROOT_FIX}/symtable.list |sed -n 's/^\(.*\)$$/ { "\1", \1 },/p' >>$(COMPILATION_ROOT_FIX)/symtable.c
	echo " { (void*)0L, (void*)0L }" >>$(COMPILATION_ROOT_FIX)/symtable.c
	echo "};" >>$(COMPILATION_ROOT_FIX)/symtable.c
	echo "int __SymbolTableLength = sizeof(__SymbolTable)/sizeof(__SymbolTable[0])-1;" >>$(COMPILATION_ROOT_FIX)/symtable.c
	$(CC) $(CFLAGS) -c -o ${COMPILATION_ROOT_FIX}/symtable.o $(COMPILATION_ROOT_FIX)/symtable.c 2>/dev/null
	./link.sh $(LINKER) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) -static $(MACH_EXTRA) $(COMPILATION_ROOT_FIX)/symtable.o $(LIB_EXTRA) $(LDFLAGS)
endif
ifeq (YES, $(LIST_UNRESOLVED_SYMBOLS))
	-$(NM) $(COMPILATION_ROOT_FIX)/appDemo \
		| sed -n 's/^ *U \([^@]*\)\(@@GLIB.*\)\?$$/\1/p' | grep -v -e '^__aeabi_'|sort -u > $(COMPILATION_ROOT_FIX)/unresolved_symbols.txt
endif
ifneq (YES, $(APP_DEMO_ONLY))	
ifeq (linux, $(OS_RUN))
ifneq (NO, $(LINUX_BUILD_KERNEL))
	echo -e "cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo"
	cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo
	echo -e "\t"$(STRIP)" "$(HOME_INITRD)"/usr/bin/appDemo"
	#Strip debug info only without debugging flag (we can use gdb on hardware)!
	$(STRIP) $(HOME_INITRD)/usr/bin/appDemo
	chmod 777 $(HOME_INITRD)/usr/bin/appDemo
endif
endif
endif

MINI_XCAT3_OBJECTS = $(wildcard $(COMPILATION_ROOT_FIX)/miniXcat3/libs/$(CPU_DIR)/*.a)
miniXcat3.bin:
	rm -f ${COMPILATION_ROOT_FIX}/miniXcat3.bin
	$(CC) -o ${COMPILATION_ROOT_FIX}/miniXcat3.bin $(CFLAGS) $(MINI_XCAT3_OBJECTS) $(LDFLAGS)
	$(CC) -static -o ${COMPILATION_ROOT_FIX}/miniXcat3.static $(CFLAGS) $(MINI_XCAT3_OBJECTS) $(LDFLAGS)
	#$(STRIP) ${COMPILATION_ROOT_FIX}/miniXcat3.bin
	#$(STRIP) ${COMPILATION_ROOT_FIX}/miniXcat3.static

#build appDemo using cpss lib
appDemo_lb :
	echo " begin appDemo_lb"
	cd  ${COMPILATION_ROOT_FIX}
	rm -f ${COMPILATION_ROOT_FIX}/appDemo
	echo "link.sh $(CC) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) $(MACH_EXTRA_LB) $(LIB_EXTRA_LB) $(LDFLAGS)"
	./link.sh $(CC) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) $(MACH_EXTRA_LB) $(LIB_EXTRA_LB) $(LDFLAGS)
	echo -e "cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo"
	cp ${COMPILATION_ROOT_FIX}/appDemo $(HOME_INITRD)/usr/bin/appDemo
	echo -e "\t"$(STRIP)" "$(HOME_INITRD)"/usr/bin/appDemo"
	$(STRIP) $(HOME_INITRD)/usr/bin/appDemo
	chmod 777 $(HOME_INITRD)/usr/bin/appDemo



# Build simulation image
appDemoSim :
	echo "link.sh $(CC) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) $(MACH_EXTRA) $(LIB_EXTRA) $(LDFLAGS)"
	./link.sh $(CC) ${COMPILATION_ROOT_FIX} $(CPU_DIR) $(CFLAGS) $(MACH_EXTRA) $(LIB_EXTRA) $(LDFLAGS)
ifeq (YES, $(LIST_UNRESOLVED_SYMBOLS))
	-$(NM) $(COMPILATION_ROOT_FIX)/appDemoSim \
		| sed -n 's/^ *U \([^@]*\)\(@@GLIB.*\)\?$$/\1/p' | sort -u > $(COMPILATION_ROOT_FIX)/unresolved_symbols.txt
endif
ifeq (EXISTS,$(RTOS_ON_SIM))
	cp ${COMPILATION_ROOT_FIX}/appDemoSim $(HOME_INITRD)/usr/bin/appDemo
	chmod 777 $(HOME_INITRD)/usr/bin/appDemo
ifneq (D_ON, $(DEBUG_INFO))
	$(STRIP) $(HOME_INITRD)/usr/bin/appDemo
endif
endif
	echo "appDemoSim created"
#----------------------------------------------------------
ifeq ($(ENDIAN), )
  ENDIAN = LE
endif
#----------------------------------------------------------

CHECK_CPP_CFLAGS += $(CFLAGS1) $(CFLAGS2) $(CFLAGS_OPT) $(OS_DEFINE) $(CPU_FLAGS) $(LFLAGS) $(AD_CFLAGS) $(CDEBUG)
CHECK_CPP_CFLAGS += $(FD)CPU=$(CPU) $(FD)_$(OS_RUN) $(FD)$(CPU_FAMILY) $(FD)CPU_$(ENDIAN) $(DEBUG_TYPE)
CHECK_CPP_CFLAGS += $(OS_CFLAGS) $(DBGFLAGS)
CHECK_CPP_CFLAGS +=-Icommon/h -ImainPpDrv/h -ImainExtUtils/h
CHECK_CPP_CFLAGS += $(FIS)common/h/cpss/extServices/os
CHECK_CPP_CFLAGS += $(FIS)mainTmDrv/h
CHECK_CPP_CFLAGS += $(addprefix $(FD),$(sort $(PROJECT_DEFS)))
CHECK_CPP_CFLAGS += $(addprefix $(FD),$(sort $(OS_DEFS)))

#----------------------------------------------------------
#ifeq (EXISTS, $(CHX_FAMILY))

        CHECK_CPP_CFLAGS += $(FD)CHX_FAMILY
        CHECK_CPP_CFLAGS += $(FD)CONFIG_DDR3
        CHECK_CPP_CFLAGS += $(FIS)common/h
        CHECK_CPP_CFLAGS += $(FIS)cpssEnabler/mainOs/h                 
        CHECK_CPP_CFLAGS += $(FIS)mainPpDrv/src/cpss/generic/labServices/ddr/bap/h
        CHECK_CPP_CFLAGS += $(FIS)mainPpDrv/src/cpss/generic/labServices/ddr/ddr3libv2/h/Driver
        CHECK_CPP_CFLAGS += $(FIS)mainPpDrv/src/cpss/generic/labServices/ddr/ddr3libv2/h/Driver/ddr3
        CHECK_CPP_CFLAGS += $(FIS)mainPpDrv/src/cpss/generic/labServices/ddr/ddr3libv2/h/Silicon
        CHECK_CPP_CFLAGS += $(FIS)mainPpDrv/src/cpss/generic/labServices/h
        CHECK_CPP_CFLAGS += $(FIS)mainPpDrv/src/cpss/generic/labServices/port/gop/h
        CHECK_CPP_CFLAGS += $(FIS)mainPpDrv/src/cpss/generic/labServices/port/gop/h/port
        CHECK_CPP_CFLAGS += $(FIS)mainTmDrv/h                
        CHECK_CPP_CFLAGS += $(FIS)mainTmDrv/h/core           
        CHECK_CPP_CFLAGS += $(FIS)mainTmDrv/h/platform     
        CHECK_CPP_CFLAGS += $(FIS)mainTmDrv/h/private        
#endif

#----------------------------------------------------------
ifeq (1,$(LINUX_SIM))
  CHECK_CPP_CFLAGS += $(addprefix $(FIS),$(sort $(SIM_INCLUDES)))
endif
#----------------------------------------------------------
checkCpp:
	echo "============================================================"
	echo "*** Check C++ errors/warning for common,mainPpDrv,mainTmDrv:"
	echo "============================================================"
	for f in $(COMPILATION_ROOT_FIX)/hdr*.cc; do \
		$(CXX) $(CHECK_CPP_CFLAGS) -c $$f -o $$f.o; done

#----------------------------------------------------------
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean_all:  clean
clean:
	rm -f appDemo*
	rm -f *.gdb
	find . -name '*~' -exec rm -f {} \;
	find . -name '*.o' -exec rm -f {} \;
	find . -name '*.$(LIBEXT)' -exec rm -f {} \;
	find . -name '*.dep' -exec rm -f {} \;
	find . -name '*.map' -exec rm -f {} \;

unix:
	find . -name '*.c' -exec dos2unix -q {} \;
	find . -name '*.cpp' -exec dos2unix -q {} \;
	find . -name '*.h' -exec dos2unix -q {} \;
	find . -name '*.C' -exec dos2unix -q {} \;
	find . -name '*.CPP' -exec dos2unix -q {} \;
	find . -name '*.H' -exec dos2unix -q {} \;
	find . -name Makefile -exec dos2unix -q {} \;
	find . -name '*.mk' -exec dos2unix -q {} \;
	find . -name presteraTopMake -exec dos2unix -q {} \;
	find . -name gtTopMake -exec dos2unix -q {} \;
	find . -name gtBuild -exec dos2unix -q {} \;

dos:
	find . -name '*.c' -exec unix2dos -q {} \;
	find . -name '*.cpp' -exec unix2dos -q {} \;
	find . -name '*.h' -exec unix2dos -q {} \;
	find . -name '*.C' -exec unix2dos -q {} \;
	find . -name '*.CPP' -exec unix2dos -q {} \;
	find . -name '*.H' -exec unix2dos -q {} \;
	find . -name Makefile -exec unix2dos -q {} \;
	find . -name '*.mk' -exec unix2dos -q {} \;
	find . -name presteraTopMake -exec unix2dos -q {} \;
	find . -name gtTopMake -exec unix2dos -q {} \;
	find . -name gtBuild -exec unix2dos -q {} \;





