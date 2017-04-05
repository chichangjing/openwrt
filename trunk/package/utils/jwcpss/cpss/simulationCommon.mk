####################################################################
#   Simulation related definitions required to Build/link project
#   This file PSS/CPSS shared
####################################################################

# $(USER_BASE) -> $(LIB_BASE)

### handle the Simulation files
ifeq (EXISTS, $(RTOS_ON_SIM))
    SIM_LIB = $(COMPILATION_ROOT_FIX)/simulation/libs/$(CPU_DIR)/simulation.$(LIBEXT)
else
    SIM_LIB = $(wildcard $(COMPILATION_ROOT_FIX)/simulation/libs/$(CPU_DIR)/*.$(LIBEXT))
    ifeq (EXISTS_Linux, $(FORCE_GM_LION_USE)_$(OS_TARGET))
        SIM_LIB += $(LIB_BASE)/simulation/libs/GM/Linux/PpInterfaceLibs/lionB/liblionB_gm.a
    endif
    ifeq (EXISTS, $(PM_FAMILY))
        ifeq (FreeBSD, $(OS_TARGET))
            SIM_LIB += $(LIB_BASE)/simulation/libs/GM/FreeBsd/$(OS_TARGET_RELEASE)/gm98EX24X.a
        endif
        ifeq (Linux, $(OS_TARGET))
            gmLibLinux = NOT_USED
            LINUX_GM=$(LIB_BASE)/simulation/libs/GM/Linux
            SIM_EXTRA += -lm
            #IDT TCAM simulation libs
            ifeq (IDT, $(EXTERNAL_TCAM_TYPE))
                gmLibLinux = $(LINUX_GM)/PpInterfaceLibs/puma2_ext_tcam/libpuma2_gm.a
#                SIM_LIB+=$(LINUX_GM)/PpInterfaceLibs/puma2_ext_tcam/libpuma2_gm.a
            else
                ifeq (EXISTS, $(FORCE_GM_PUMA3_USE))
                    gmLibLinux = $(LINUX_GM)/PpInterfaceLibs/puma3/Puma3_b0/libpuma3_gm.a
#                    SIM_LIB+=$(LINUX_GM)/PpInterfaceLibs/puma3/Puma3_b0/libpuma3_gm.a
                else
                    gmLibLinux = $(LINUX_GM)/PpInterfaceLibs/puma2/libpuma2_gm.a
#                   SIM_LIB+=$(LINUX_GM)/PpInterfaceLibs/puma2/libpuma2_gm.a
                endif
            endif
            ifneq (NOT_USED, $(gmLibLinux))
            ifneq (, $(FORCE_GM_EXPLICIT_FILE))
                gmLibLinux = $(FORCE_GM_EXPLICIT_FILE)
            endif
            SIM_LIB += $(gmLibLinux)
            endif
            ifneq (EXISTS, $(FORCE_GM_PUMA3_USE))
                SIM_LIB+=$(LINUX_GM)/libidtplatform.a
                SIM_LIB+=$(LINUX_GM)/libidtresource.a
                SIM_LIB+=$(LINUX_GM)/libidtutility.a
                SIM_LIB+=$(LINUX_GM)/libnsa_socket_driver.a
            endif
            ifneq (, $(LINUX_GM)/NL/extDevices.lib)
                SIM_EXTRA +=  $(LINUX_GM)/NL/extDevices.lib \
                            $(LINUX_GM)/NL/nlm8kcommon.a \
                            $(LINUX_GM)/NL/nlm8kmodel.a
            endif
        endif
    endif

##############################
#                 win32
##############################

    ifeq (win32, $(OS_RUN))
        #extra libs when ASIC is needed

        #allow the environmet to explicitly give path to SLAN/SHOST lib that is not from the 'Server'
        #this file ignores the SLAN/SHOST from other sources.
    ifneq (, $(FORCE_SHOST_EXPLICIT_FILE))
        shostLib = $(FORCE_SHOST_EXPLICIT_FILE)
    else
            shostLib = $(LIB_BASE)\simulation\libs\shost\$(SHOST_LIB_DIR)\SHOST.lib
    endif

    ifneq (, $(FORCE_SLAN_EXPLICIT_FILE))
        slanLib = $(FORCE_SLAN_EXPLICIT_FILE)
    else
        slanLib = $(LIB_BASE)\simulation\libs\slan\$(SHOST_LIB_DIR)\SLAN.LIB
    endif

        SIM_EXTRA += $(shostLib)
        SIM_EXTRA += $(slanLib)

        ifeq (EXISTS, $(IMPL_PRINT_CALL_STACK))
            SIM_EXTRA += $(LIB_BASE)\simulation\libs\shost\$(SHOST_LIB_DIR)\StackWalker.obj
        endif

        #SUB-20 library
        ifeq (EXISTS, $(FORCE_SUB20_USE))
          SIM_EXTRA += $(LIB_BASE)\simulation\libs\CM\VC\sub20.lib
        endif

        gmLib = NOT_USED
        WIN_GM=$(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)
        #check if need to add to the simulation the GM lib
        ifeq (EXISTS, $(FORCE_GM_LION_USE))
            gmLib = $(WIN_GM)/PpInterfaceLibs/lionb/PPInterface.lib
        endif
        ifeq (EXISTS, $(FORCE_GM_XCAT2_USE))
            gmLib = $(WIN_GM)/PpInterfaceLibs/xcat2/PPInterface.lib
        endif
        ifeq (EXISTS, $(FORCE_GM_LION2_USE))
            gmLib = $(WIN_GM)/PpInterfaceLibs/lion2/PPInterface.lib
        endif
        ifeq (EXISTS, $(FORCE_GM_PUMA3_USE))
            ifeq (6, $(VC_VER))
                gmLib = $(WIN_GM)/PpInterfaceLibs/puma3/PPInterface.lib
            else
                gmLib = $(WIN_GM)/PpInterfaceLibs/puma3_B0/PPInterface.lib
            endif
        endif
        ifeq (EXISTS, $(FORCE_GM_BOBCAT2_USE))
            gmLib = $(WIN_GM)/PpInterfaceLibs/bobcat2/PPInterface.lib
        endif
        ifeq (EXISTS, $(FORCE_GM_BOBCAT3_USE))
            gmLib = $(WIN_GM)/PpInterfaceLibs/bobcat3/PPInterface.lib
        endif
        ifeq (EXISTS, $(FORCE_GM_BOBCAT2_B0_USE))
            gmLib = $(WIN_GM)/PpInterfaceLibs/bobcat2_b0/PPInterface.lib
        endif
        ifeq (EXISTS, $(FORCE_GM_BOBK_CAELUM_USE))
            gmLib = $(WIN_GM)/PpInterfaceLibs/bobk/PPInterface.lib
        endif

        ifeq (NOT_USED, $(gmLib))
            ifeq (EXISTS, $(PM_FAMILY))
                ifeq (IDT, $(EXTERNAL_TCAM_TYPE))
                    gmLib = $(WIN_GM)/PpInterfaceLibs/puma2_extTcam/PPInterface.lib
                else
                    ifeq (NL, $(EXTERNAL_TCAM_TYPE))
                        gmLib = $(WIN_GM)/PpInterfaceLibs/puma2_extTcam/PPInterface.lib
                    else
                        gmLib = $(WIN_GM)/PpInterfaceLibs/puma2/PPInterface.lib
                    endif
                endif
            endif
        endif

        #allow the environmet to explicitly give path to GM lib that is not from the 'Server'
        #this file ignores the 'family type' of the GM
        ifneq (NOT_USED, $(gmLib))
            ifneq (, $(FORCE_GM_EXPLICIT_FILE))
                gmLib = $(FORCE_GM_EXPLICIT_FILE)
            endif
        endif

        ifneq (NOT_USED, $(gmLib))
            SIM_LIB += $(gmLib)

            # the EXMXPM simulation need the extra GM lib
            ifeq (EXISTS, $(PM_FAMILY))
                ifeq (6, $(VC_VER))
                    #currently not supported for VC8
                    # check if external devices (TCAM) library exists
                    # NL TCAM simulation libs
                    ifeq (NL, $(EXTERNAL_TCAM_TYPE))
                        ifneq (, $(wildcard $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/NL.lib))
                            SIM_LIB += $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/NL.lib
                        endif
                    endif
                    # IDT TCAM simulation libs
                    ifeq (IDT, $(EXTERNAL_TCAM_TYPE))
                        SIM_LIB += $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/libidtplatform.a
                        SIM_LIB += $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/libidtresource.a
                        SIM_LIB += $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/libidtutility.a
                        SIM_LIB += $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/libnsa_socket_driver.a
                        SIM_LIB += $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/libgcc.a
                        ifneq (, $(wildcard $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/IDT.lib))
                            SIM_LIB += $(LIB_BASE)/simulation/libs/GM/vc$(VC_VER)/IDT.lib
                        endif
                    endif
                endif
            endif

        endif


    endif

    ifeq (1, $(LINUX_SIM))

        gmLib = NOT_USED
        LINUX_GM=$(LIB_BASE)/simulation/libs/GM/Linux

        #check if need to add to the simulation the GM lib
        ifeq (EXISTS, $(FORCE_GM_BOBCAT2_USE))
            gmLib = $(LINUX_GM)/PpInterfaceLibs/bobcat2/libBobcat2_gm.a
        endif
        ifeq (EXISTS, $(FORCE_GM_BOBCAT2_B0_USE))
            gmLib = $(LINUX_GM)/PpInterfaceLibs/bobcat2_b0/libBobcat2b0_gm.a
        endif
        ifeq (EXISTS, $(FORCE_GM_BOBCAT3_USE))
            gmLib = $(LINUX_GM)/PpInterfaceLibs/bobcat3/libBC3Gm_gm.a
        endif
        ifeq (EXISTS, $(FORCE_GM_BOBK_CAELUM_USE))
            gmLib = $(LINUX_GM)/PpInterfaceLibs/bobk/libBobk_caelum_gm.a
        endif

        #allow the environmet to explicitly give path to GM lib that is not from the 'Server'
        #this file ignores the 'family type' of the GM
        ifneq (NOT_USED, $(gmLib))
            ifneq (, $(FORCE_GM_EXPLICIT_FILE))
                gmLib = $(FORCE_GM_EXPLICIT_FILE)
            endif
        endif
        ifneq (NOT_USED, $(gmLib))
            SIM_LIB += $(gmLib)
        endif
    endif


endif


