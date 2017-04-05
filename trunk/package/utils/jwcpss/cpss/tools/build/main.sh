#!/usr/bin/env bash
#
# PSS/CPSS build script
#
# Enviroment variables:
# 1. Project depended
#   PSS_PATH                required for PSS* projects
#   CPSS_PATH               required for CPSS* projects and PSS_O_CPSS
#
# 2. OS depended
# 2.1. Linux BM
#   LINUX_DIST_PATH         required. Kernel sources should be found here,
#                           kernel will be compiled in this directory
#   LSP_USER_BASE           required.
#   LSP_SUBDIR            try $LSP_USER_BASE/$LSP_SUBDIR/$LINUX_LSP_NAME first
#                         Otherwise use $LSP_USER_BASE/$LINUX_LSP_NAME
#   LINUX_KERNEL_VERSION    (or DEFAULT_LINUX_KERNEL_VERSION from CPU defs)
#   LINUX_LSP_NAME          (or DEFAULT_LINUX_LSP_NAME from CPU defs)
#   LINUX_BUILD_KERNEL    set to YES to build kernel. Default is YES
#
# 2.2. vxWorks:
#   BSP_CONFIG_DIR          (Fx: Z:\cpss\Bsp\config)
#
#
# Options:
#   -T toolkit          Choose toolkit.
#                       VC,eldk,GnuEabi,Tornado,gcc(native for simulation),etc
#                       refers $tool_build/toolkit/${toolkit_name}.sh
#
#                       Default toolkit for each supported OS defined in CPU file
#
#   -b BUS              override management bus.
#                       Default is CPU specific
#
#   -x XBAR             set XBAR type. Default NONE
#
#   -s SO_LIB           Build shared objects. Default NONE.
#
#   -u UTF_YES|UTF_NO
#                       Incude UTF. Default YES
#   -a CPSS_LOG_YES|CPSS_LOG_NO
#                       Enable/disable CPSS API log. Default is CPSS_LOG_YES.
#
#   -C                  Do build in current directory only
#                       This option developed to be used with simple script
#                       Example  (bld_cpss_sim)
#                           #!/bin/bash
#                           export CPSS_PATH=/home/serg/tmp/Marvell/cpss
#                           $CPSS_PATH/Build.sh $1 \
#                               -u UTF_YES -L \
#                               Linux i386 LinuxWM \
#                               DX simulation CPSS_ENABLER \
#                               /home/serg/tmp/Marvell/objs -
#                       Run this script with no option to build whole project
#                       Add -C parameter to build in current directory only
#
#   -D DEV | ENVBASE | CUST
#                       If DEV Run in developer mode - spawn an interactive shell
#                       after finishing of all steps (BSP copy, compile and link)
#
#                       If ENBASE - spawn an interactive
#                       shell after all environment set and host OS depended
#                       preparation actions is done
#
#                       If CUST - spawn an interactive shell
#                       after finishing compile and link
#
#   -q                  Run script silently - only warning and error
#                       messages will be visible
#
#   -l                  lua CLI compilation and build
#
#   -e                  LUA_CLI without examples
#
#   -N NOKERNEL|NOLINK|NOKERNEL_NOLINK
#                       no linux kernel build | no appDemo link
#
#   -R                  The directory to copy uImage and rootfs.tgz
#
#   -g                  force Golden Module Lib usage
#
#   -d                  ClearCase baseline
#
#   -p                  Coverity Server Port Number
#
# Parameters:
#   1. Host OS          Linux, WIN32, FreeBSD
#   2. Target CPU       PPC603, PPC6024, PPC603_VB, ARM_EV, ARM_RD, ARM_SP,
#                       ARM_GM, ARM5181, ARM5281, i386, etc
#                       refers $tool_build/cpu/${target_cpu}.sh
#   3. Target OS        Linux,vxWorks,uClinux,WIN32,FreeBSD
#                       refers $tool_build/os/${target_os}.sh
#
#   4. the prestera PP family (DX, SX, EX, MX, DXSX)
#   5. pp type: HW(BM), simulation, GM
#   6. product type: PSS, CPSS, POC, PSS_O_CPSS
#
#   7. compilation root
#   8. (image) destination path
##############################################
#  FILE VERSION: 23
##############################################
XBAR_TYPE=NONE
#export INCLUDE_UTF=YES
#export INCLUDE_ENHANCED_UTF=YES
export PROJECT_DEFS=""
export PROJECT_INCLUDES=""
export OS_DEFS=""
export OS_CFLAGS=""
export CPU_FLAGS=""
MANAGEMENT_BUS_PARAM=""
TOOLKIT=""
CC_BASELINE=""
COV_PORT="5500"


export UNI_DIR=

# $tool_build already defined in top level wrapper
all_args="$*"

while getopts "c:T:b:d:h:p:x:f:u:a:D:qsleg:CN:R:o:G:" opt;
do
    case $opt in
    C ) # Set BUILD_ONLY to (C)PSS relative path
        cur_dir="`(cd .;pwd)`"
        proj_dir="${tool_build%/*}"
        proj_dir="${proj_dir%/*}"
        if [ "$cur_dir" != "${cur_dir#$proj_dir/}" ]; then
            BUILD_ONLY="${cur_dir#$proj_dir/}"
            export BUILD_ONLY
            echo -E "BUILD_ONLY=$BUILD_ONLY"
        fi
        ;;
    T ) # Set toolkit
        TOOLKIT="$OPTARG"
        ;;
    d ) # Set ClearCase baseline - for coverity description
        CC_BASELINE="$OPTARG"
        ;;
    p ) # Set Coverity Server Port Number
        COV_PORT="$OPTARG"
        ;;
    b ) # Set management bus
        MANAGEMENT_BUS_PARAM="$OPTARG"
        ;;
    c ) # Set COVERITY RUN
        case $OPTARG in
        pss_dx_vc  | pss_dx_vx | pss_pm_vx | cpss_dx_vc  | cpss_dx_vx | cpss_pm_vx)
            RUN_PSS_CPSS_COVERITY=YES
            COV_PRODUCT="$OPTARG"
            ;;
        * )
            echo "Unsupported coverity case: $OPTARG passed with -c option."
            exit 1
            ;;
        esac
        ;;
    l ) # set lua cli to include
         export CMD_LUA_CLI="yes"
        ;;
    e ) # LUA_CLI without examples
         export NO_LUACLI_EXAMPLES="yes"
        ;;
    N ) # no kernel build or no link
            case $OPTARG in
               NOKERNEL )
                  export LINUX_BUILD_KERNEL=NO
                  ;;
               NOLINK )
                  export DONT_LINK=YES
                  ;;
               NOKERNEL_NOLINK )
                  export LINUX_BUILD_KERNEL=NO
                  export DONT_LINK=YES
                  ;;
               esac
               ;;

    R ) # The directory to copy uImage and rootfs.tgz
        if [ ! -d $OPTARG ]; then
            echo "The directory for uImage and rootfs doesn't exists: $OPTARG"
            exit 1
        fi
        TARGET_DIR_ROOTFS=$OPTARG
        ;;
    g ) # force GM lib usage
        # no galtis option
        case $OPTARG in
         lion )
            export FORCE_GM_LION_USE="EXISTS"
            ;;
         lion2 )
            export FORCE_GM_LION2_USE="EXISTS"
            ;;
         xcat2 )
            export FORCE_GM_XCAT2_USE="EXISTS"
            ;;
         puma3 )
            export FORCE_GM_PUMA3_USE="EXISTS"
            ;;
         bobcat2 )
            export FORCE_GM_BOBCAT2_USE="EXISTS"
            ;;
         bobcat2_b0 )
            export FORCE_GM_BOBCAT2_B0_USE="EXISTS"
            ;;
         bobk_caelum )
            export FORCE_GM_BOBK_CAELUM_USE="EXISTS"
            ;;
         bobcat3 )
            export FORCE_GM_BOBCAT3_USE="EXISTS"
            ;;
        * )
            echo "Unsupported GM lib type: $OPTARG passed with -g option"
            exit 1
            ;;
        esac
        # force "no_stub_usage"
        export FORCE_GM_USE="EXISTS"
        ;;

    G )
        # no galtis option
      case $OPTARG in
        no_galtis )
           export EXCLUDE_GALTIS="EXCLUDE_LIB"
           ;;
        * )
            echo "Unsupported GALTIS option: $OPTARG passed with -G option"
            exit 1
           ;;
       esac
       ;;

    h ) # force GM lib usage
        case $OPTARG in
         sub20 )
            export FORCE_SUB20_USE="EXISTS"
            ;;
         * )
             echo "Unsupported option: $OPTARG passed as -h option"
             exit 1
             ;;
         esac
        ;;
    o ) # Set SO LIB
        export SO_LIB=YES
		export EXCLUDE_GALTIS=EXCLUDE_LIB
        ;;
    x ) # Set xbar
        XBAR_TYPE="$OPTARG"
        ;;
    u ) # Set INCLUDE_UTF
        case $OPTARG in
           NO | UTF_NO )
               export INCLUDE_UTF_OPTION=YES
               export INCLUDE_UTF=NO
               export INCLUDE_ENHANCED_UTF=NO
           ;;
           YES | UTF_YES )
               export INCLUDE_UTF_OPTION=YES
               export INCLUDE_UTF=YES
               export INCLUDE_ENHANCED_UTF=YES
           ;;
        * )
            echo "Unsupported UTF type: $OPTARG passed with -u option."
            exit 1
            ;;
        esac
        ;;
    a ) # enable/disable the CPSS API log
        case $OPTARG in
           CPSS_LOG_NO )
                export LOG_ENABLE="L_OFF"
                export API_LOG_ENABLE="NO" # any non-empty value is ok
                ;;
           CPSS_LOG_YES )
                export LOG_ENABLE="L_ON"
                export API_LOG_ENABLE=""   # should be emtpy
                ;;
        esac
        ;;

    f ) # Set make flags
        MAKE_FLAGS="$OPTARG"
        echo " >>> MAKE_FLAGS - $MAKE_FLAGS"
        ;;
    D )
        case $OPTARG in
        dev | DEV )  mode=developer  ;;
        envbase | ENVBASE ) mode=envbase ;;
        cust | CUST )  mode=customer  ;;
        * )
            echo "Unsupported argument: $OPTARG for -D option."
            exit 1
            ;;
        esac
        ;;
    q )
        SCRIPT_QUIET_MODE=YES
        ;;
    s )
        export PSS_MODE=PSS_DPSS
        export DPSS_DIR=T2CDPSS
        ;;
    * ) echo "Usupported option"
        exit 1
        ;;
    esac
done

eval HOST_OS=\${$OPTIND}
shift $OPTIND
CPU_BOARD=${1}
TARGET_OS=${2}
FAMILY=${3}
SIMULATION=${4}
PRODUCT_TYPE=${5}
COMPILATION_ROOT="${6}"
IMAGE_PATH="${7}"

#----------------------------------------------------------------

echo "main.sh CPU_BOARD       =${CPU_BOARD}"
echo "main.sh TARGET_OS       =${TARGET_OS}"
echo "main.sh FAMILY          =${FAMILY}"
echo "main.sh SIMULATION      =${SIMULATION}"
echo "main.sh PRODUCT_TYPE    =${PRODUCT_TYPE}"
echo "main.sh COMPILATION_ROOT=${COMPILATION_ROOT}"
echo "main.sh IMAGE_PATH      =${IMAGE_PATH}"

###############################
# Set CPSS API_LOCK_PROTECTION
###############################
if [ -z $API_LOCK_PROTECTION ]; then
    export API_LOCK_PROTECTION="P_ON"
fi

###############################
# Set PSS_PRODUCT_TYPE
###############################
if [ "$PRODUCT_TYPE" = "PSS" -o "$PRODUCT_TYPE" = "POC" ]; then
   PSS_PRODUCT_TYPE=${5}
else
   PSS_PRODUCT_TYPE="NONE"
fi



###############################
# no copy GM VC images to zip
# because IMAGE_PATH=""
###############################
#if [ "$FORCE_GM_USE" = "EXISTS" -o "$FORCE_SUB20_USE" = "EXISTS" ]; then
#   IMAGE_PATH=""
#fi

export COMPILATION_ROOT

trace_message()
{
    if [ \! -z "$SCRIPT_DEBUG_MODE" ]; then
        echo -E "$@"
    fi
    return 0
}
T()
{
	trace_message "$@"
	"$@"
}

info_message()
{
    if [ -z "$SCRIPT_QUIET_MODE" ]; then
        echo -E "$@"
    fi
    return 0
}
I()
{
	info_message "$@"
	"$@"
}

warning_message()
{
    echo -E "$@"
    return 0
}

error_message()
{
    echo -E "$@" >&2
    return 0
}

##################################################
# declare all functions - they can be owerriden
##################################################

# host specific actions.
#   release unpacking, version control,
#   packing release, image, copying results, etc
host_shell()
{
    $SHELL
}

host_pre_build()
{
    return 0
}

host_post_build()
{
    return 0
}

# target os specific actions
#   prepare to build (build kernel, libraries, rootfs, etc
#   build, link, make image
os_pre_build()
{
    return 0
}

os_build()
{
    # compile project
    info_message "${MAKE} ${MAKE_FLAGS} -C ${USER_BASE} -f ${USER_MAKE} ${MAKE_TARGET:-full}"
    ${MAKE} ${MAKE_FLAGS} -C "${USER_BASE}" -f ${USER_MAKE} ${MAKE_TARGET:-full}
    if [ $? -ne 0 ] # test result
    then
        error_message -e "\tfailed to build $USER_BASE"
        return 1
    fi
    return 0
}

os_pre_link()
{
    return 0
}

os_link()
{
    return 0
}

os_post_build()
{
    return 0
}

# product specific actions
#
product_pre_build()
{
    if [ "$TARGET_OS" != "vxWorks" -o "$DRAGONITE_TYPE" != "A1" ]
    then
   dragoniteFwPath=${CPSS_PATH}/cpssEnabler/mainSysConfig/src/appDemo/dragonite/firmware
   if [ -e ${dragoniteFwPath}/drigonite_fw.s ]
   then
       info_message "Dragonite FW assembly delete:rm -f ${dragoniteFwPath}/drigonite_fw.s"
       rm -f ${dragoniteFwPath}/drigonite_fw.s
   fi
    fi

    return 0
}

product_pre_link()
{
    return 0
}

product_post_build()
{
    return 0
}






# check parameters
check_param()
{
    if [ \! -f $tool_build/$1/$2.sh ]; then
        echo "Unknown $3: $2.sh not found $tool_build/$1/"
        exit 1
    fi
}
check_param host ${HOST_OS} "host OS"
info_message "HOST_OS=${HOST_OS}, including $tool_build/host/${HOST_OS}.sh"
. $tool_build/host/${HOST_OS}.sh

check_param cpu ${CPU_BOARD} "cpu"
info_message "CPU_BOARD=${CPU_BOARD}, including $tool_build/cpu/${CPU_BOARD}.sh"
. $tool_build/cpu/${CPU_BOARD}.sh
if [ -z "$TOOLKIT" ]
then
    # if no toolkit spefified then use default toolkit for this
    # CPU and target OS
    eval TOOLKIT=\${DEFAULT_TOOLKIT_${TARGET_OS}}
    if [ -z "$TOOLKIT" ]
    then
        echo "Toolkit not specified"
        exit 1
    fi
fi
# after toolkit selected get value of $CPU_PRE_TOOLKIT_toolkit and execute it
# For example if TOOLKIT == vxWorks then eval results
# ${CPU_PRE_TOOLKIT_vxWorks}
# After that shell expands value of CPU_PRE_TOOLKIT_vxWorks and execute it
# If this variable is not defined then this will be evaluated to empty line
# and then shell will silently do nothing
eval \${CPU_PRE_TOOLKIT_${TOOLKIT}}

check_param toolkit ${TOOLKIT} "toolkit"
info_message "TOOLKIT=${TOOLKIT}, including $tool_build/toolkit/${TOOLKIT}.sh"
. $tool_build/toolkit/${TOOLKIT}.sh

check_param os ${TARGET_OS} "target os"
info_message "TARGET_OS=${TARGET_OS}, including $tool_build/os/${TARGET_OS}.sh"
. $tool_build/os/${TARGET_OS}.sh

check_param product ${PRODUCT_TYPE} "product type"

if [ -f $tool_build/quirks/${TOOLKIT}_${CPU_BOARD}.sh ]; then
	info_message "Applying toolkit==${TOOLKIT} + cpu=${CPU_BOARD} rules"
	. $tool_build/quirks/${TOOLKIT}_${CPU_BOARD}.sh
fi

# set default value of LOG_ENABLE if it wasn't set
export LOG_ENABLE=${LOG_ENABLE-L_ON}

# export host/target vars??

# product configuration based on parameters 6,7,8
. $tool_build/product/${PRODUCT_TYPE}.sh

if [ "$PSS_PRODUCT_TYPE" != "POC" ]; then
   if [ "$INCLUDE_UTF_OPTION" != "YES" ]; then
      export INCLUDE_UTF=YES
      export INCLUDE_ENHANCED_UTF=YES
   fi
   if [ "$INCLUDE_UTF" = "YES" ]; then
      PROJECT_DEFS="$PROJECT_DEFS INCLUDE_UTF"
   fi
   if [ "$INCLUDE_ENHANCED_UTF" = "YES" ]; then
      PROJECT_DEFS="$PROJECT_DEFS INCLUDE_TGF"
   fi
fi

###############################
### Configured
###############################


###############################
### Run COVERITY
###############################

if [ "$RUN_PSS_CPSS_COVERITY" = "YES" ]; then

   echo "Run COVERITY"
   win32_add_paths $CPSS_TOOLS_PATH'\tools\bin'
   cd ${USER_BASE}

   info_message "rm -rf C:\\prevent\\"$COV_PRODUCT"\\*"
   rm -rf C://prevent//"$COV_PRODUCT"

   #compile for COVERITY CHECKING
   info_message "cov-build --dir C:\\prevent\\"$COV_PRODUCT" gmake -sf presteraTopMake full"
   cov-build --dir C:\\prevent\\"$COV_PRODUCT" gmake -f presteraTopMake full

   info_message "cov-analyze --checker-option DEADCODE:no_dead_default:true --dir  C:\\prevent\\"$COV_PRODUCT""
   cov-analyze --checker-option DEADCODE:no_dead_default:true --dir  C:\\prevent\\"$COV_PRODUCT"

   info_message "cov-commit-defects  --remote pt-pss01 --port "$COV_PORT" --description "$CC_BASELINE" --product "$COV_PRODUCT" --user admin --password password --dir C:\\prevent\\"$COV_PRODUCT""
   cov-commit-defects  --remote pt-pss01 --port "$COV_PORT" --description "$CC_BASELINE" --product "$COV_PRODUCT" --user admin --password password --dir C:\\prevent\\"$COV_PRODUCT"
   exit 0
fi

trace_message "<= BEGIN host preparation"
host_pre_build || exit
trace_message "=> END host preparation"


if [ "$mode" = "envbase" ]; then
    echo "Environment set, starting shell."
    echo 'Type "exit" to finish'
    host_shell
    exit 0
fi

trace_message "<= BEGIN target OS preparation"
os_pre_build || exit
trace_message "=> END target OS preparation"

trace_message "<= BEGIN product preparation"
product_pre_build || exit
trace_message "=> END product preparation"

#############################
# do build in customer mode
#############################
if [ "$RUN_COVERITY" = "YES" ]; then
   trace_message "<= BEGIN build"
   coverity_os_build || exit
   trace_message "=> END build"
   exit 0
fi

if [ "$mode" != "developer" -a "$mode" != "customer" ]; then
   trace_message "<= BEGIN build"
   os_build || exit
   trace_message "=> END build"
fi

if [ "$DONT_LINK" = YES ]; then
    exit 0
fi

trace_message "<= BEGIN product link preparation"
product_pre_link || exit
trace_message "=> END product link preparation"

trace_message "<= BEGIN target OS link preparation"
os_pre_link || exit
trace_message "=> END target OS link preparation"

trace_message "<= BEGIN link"
os_link || exit
trace_message "=> END link"

if [ "$RUN_COVERITY" = "YES" ]; then
    echo "End of COVERITY RUNNING - exit "
    exit 0
fi

trace_message "<= BEGIN product post build phase"
product_post_build || exit
trace_message "=> END product post build phase"

trace_message "<= BEGIN target OS post build phase"
os_post_build || exit
trace_message "=> END target OS post build phase"

trace_message "<= BEGIN host post build phase"
host_post_build || exit
trace_message "=> END host post build phase"

#############################################
## if defined developer mode or customer mode
## goto cmd shell
#############################################
if [ "$mode" = "developer" -o "$mode" = "customer" ]; then
    echo "Environment set, starting shell."
    echo 'Type "exit" to finish'
    host_shell
    exit 0
fi
