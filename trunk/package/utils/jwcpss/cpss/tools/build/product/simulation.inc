# simulation settings
#
##############################################################################
# similation related environment variables
##############################################################################
#
# ASIC_SIMULATION - do we need to compile code that uses simulation
#                                                    -- empty / EXISTS
# RTOS_ON_SIM - do we use RTOS (Real Time Operating System) to connect to remote
#               simulation                           -- empty / EXISTS
#
# DISTRIBUTED_SIMULATION_ROLE  - do we compile optimized code for distributed
#                                application side / devices side
#                                                    -- APPLICATION_SIDE_ONLY /
#                                                       DEVICES_SIDE_ONLY /
#                                                       BROKER_ONLY /
#                                                       empty
#       NOTE : relevant when ASIC_SIMULATION is used
#
#

unset ASIC_SIMULATION
unset LINUX_SIM
unset RTOS_ON_SIM
unset DISTRIBUTED_SIMULATION_ROLE
case $SIMULATION in
    simulation | GM*)
        export ASIC_SIMULATION=EXISTS
        OS_DEFS="$OS_DEFS ASIC_SIMULATION"
        SIM_INCLUDES="$SIM_INCLUDES simulation/simGeneral/h"
        SIM_INCLUDES="$SIM_INCLUDES simulation/simGeneral/h/common/SHOST"
        SIM_INCLUDES="$SIM_INCLUDES simulation/simDevices/h"
        SIM_INCLUDES="$SIM_INCLUDES simulation/simOs/h"
        SIM_INCLUDES="$SIM_INCLUDES simulation/h"
        export SIM_INCLUDES
        if [ $TARGET_OS \!= win32 -a TARGET_OS \!= vxWorks ]
        then
            export LINUX_SIM=1
            OS_DEFS="$OS_DEFS LINUX_SIM"
        fi
        if [ "$PRODUCT_TYPE" = "BROKER" ]
        then
            # this compile minimal simulation code to support application side
            export DISTRIBUTED_SIMULATION_ROLE=BROKER_ONLY
            OS_DEFS="$OS_DEFS APPLICATION_SIDE_ONLY"
        elif [ $TARGET_OS = Linux -o $TARGET_OS = FreeBSD ]
        then
            export RTOS_ON_SIM=EXISTS
            OS_DEFS="$OS_DEFS EMBEDDED RTOS_ON_SIM APPLICATION_SIDE_ONLY"
            CPU_DIR=${CPU_DIR}_SIM
            export DISTRIBUTED_SIMULATION_ROLE=APPLICATION_SIDE_ONLY
        fi
        if [ "$RTOS_ON_SIM" != "EXISTS" -a "$PM_FAMILY" = "EXISTS" ]; then
            export FORCE_GM_USE=EXISTS
        fi
        if [ "$FORCE_GM_USE" = "EXISTS" ]; then
            OS_DEFS="$OS_DEFS FORCE_GM_USE"
        fi

        case $FAMILY in
            DX | DX_ALL | DXALL | DX_CHX | DX_CH3 | DX_CH3P )
                        if [ "$RTOS_ON_SIM" \!= "EXISTS" -a "$TARGET_OS" = "win32" ]
                        then
                            if [ "$DRAGONITE_TYPE" \!= "NONE" ]; then
                                export DRAGONITE_TYPE=A1
                                PROJECT_DEFS="$PROJECT_DEFS DRAGONITE_TYPE_A1"
                            fi
                        fi
            ;;
            *)
            ;;
        esac
        ;;
    HW | BM)
        OS_DEFS="$OS_DEFS EMBEDDED"
        ;;
    *)
        ;;
esac

# handle GM options
case $SIMULATION in
    GM_lion)
        export FORCE_GM_LION_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ;;
    GM_lion2)
        export FORCE_GM_LION2_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ;;
    GM_xcat2)
        export FORCE_GM_XCAT2_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ;;
    GM_puma3)
        export FORCE_GM_PUMA3_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ;;
    GM_bobcat2)
        export FORCE_GM_BOBCAT2_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ######
        #  redefine CPU_DIR and linux_objects_ext for GM_bobcat2
        ######
        export CPU_DIR="i386_BOBCAT2_GM"
        export linux_objects_ext="i386_BOBCAT2_GM"
        ;;
    GM_bobcat3)
        export FORCE_GM_BOBCAT3_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ######
        #  redefine CPU_DIR and linux_objects_ext for GM_bobcat3
        ######
        export CPU_DIR="i386_BOBCAT3_GM"
        export linux_objects_ext="i386_BOBCAT3_GM"
        ;;
    GM_bobcat2_b0)
        export FORCE_GM_BOBCAT2_B0_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ######
        #  redefine CPU_DIR and linux_objects_ext for GM_bobcat2_b0
        ######
        export CPU_DIR="i386_BOBCAT2_B0_GM"
        export linux_objects_ext="i386_BOBCAT2_B0_GM"
        ;;
    GM_bobk_caelum)
        export FORCE_GM_BOBK_CAELUM_USE="EXISTS"
        export FORCE_GM_USE="EXISTS"
        ######
        #  redefine CPU_DIR and linux_objects_ext for GM_bobk_caelum
        ######
        export CPU_DIR="i386_BOBK_CAELUM_GM"
        export linux_objects_ext="i386_BOBK_CAELUM_GM"
        ;;
esac
