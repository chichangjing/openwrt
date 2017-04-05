#!/usr/bin/env bash
##################################################################################
##  build_cpss.sh
##################################################################################
##
##  APPDEMO IMAGE BUILD WRAPPER
##
##################################################################################
## ARGUMENTS         :  Can be defined in any order
##
##     CPU and  PP_TYPE  are the mandatory parameters  -  the rest are optional
##
##      MANDATORY PARAMETERS
##      -----------------------------------
## CPU      : ARM5181 ARM5281 PPC603 PPC85XX EP3041A XCAT XCAT_BE i386 i386_64 XCAT_2.6.31
##            XCAT_BE_2.6.31 ARM78200 ARM78200_BE ARMADAXP ARMADAXP_BE XCAT_3.4 XCAT_3.4_BE
##            MIPS64 INTEL64 INTEL64_32 INTEL32 MSYS MSYS_2_6_32 ARMADAXP_3.4.69 ARMADA_385
##            MSYS_3_10 ARMADA_390 MSYS_BE
##
## PP_TYPE  : DX_ALL EX EXMXPM EX_DX_ALL EXMXPM_DX_CH
##
##      OPTIONAL PARAMETERS
##      -------------------------------
##
## CROSSBAR          :  FOX  DUNE     -   no default value   ( no build for crossbar )
##
## PRODUCT           :  CPSS_ENABLER  CPSS_LIB  CPSS_ENABLER_NOKM CPSS_BLOB - default value CPSS_ENABLER
##
## UT                :  UTF_YES UTF_NO       - default value UTF_YES  ( build UT )
## CPSS_LOG          :  CPSS_LOG_YES CPSS_LOG_NO - default value  CPSS_LOG_YES
##
## LUA               :  NOLUA (the same as LUA_NO) -  by default LUA code is included, NOLUA removes LUA support
##
## NO_EXAMPLES       :  remove examples from LUA code
##
## GALTIS            :  NOGALTIS - galtis code not used
##
## TOOLKIT           :  SDK_5_V5 SDK_5_V7 SDK_3.2 SDK_1.2.4 ELDK_3.1
##
## Build options     :  UNZIP   ENVBASE  CUST  SHLIB SO_LIB     - no defaulf value
##
## Build options     :  UNZIP   ENVBASE  CUST  SHLIB      - no defaulf value
##
## GM_LION           :  specify using of   GM libs for DX LION simulation image build
##
## GM_PUMA3          :  specify using of   GM libs for PUMA3 simulation image build
##
## GM_PUMA3          :  specify using of   GM libs for PUMA3 simulation image build
##
## GM_BOBCAT2        :  specify using of   GM libs for GM_BOBCAT2 simulation image build
##
## GM_BOBCAT2_B0     :  specify using of   GM libs for GM_BOBCAT2_B0 simulation image build
##
## GM_BOBCAT3        :  specify using of   GM libs for GM_BOBCAT3 simulation image build
##
## NOKERNEL 		 :  Do not build kernel
##
## NOLINK			 :  Only build libs - do not try to link
##
##################################################
##
##  ENVIRONMENT VARIABLES
##
##  The following parameters can be define as environment variables:
##
## --------------------------------------------------------------------------------------
## variable                         default value
## --------------------------------------------------------------------------------------
##  WORKING_DIRECTORY             currect directory (pwd)
##  CPSS_PATH                     /cpss/SW/prestera
##  LINUX_DIST_PATH               $WORKING_DIRECTORY/linux
##  LSP_USER_BASE                 $WORKING_DIRECTORY/lsp
##  COMPILATION_ROOT              $WORKING_DIRECTORY/compilation_root
##  IMAGE_PATH                    $WORKING_DIRECTORY/compilation_root
##  LINUX_BUILD_KERNEL            ""
##  DONT_LINK                     ""
##  TARGET_DIR_ROOTFS             ""
## -----------------------------------------------------------------------------------------
##
##
##       OPTIONS:
##
##               UNZIP  - this option can be used by customer to:
##                                 - unzip CPSS components
##                                 - compile CPSS modules
##                                 - build CPSS appDemo
##
##
##               ENVBASE  - this option can be used by customer to set CPSS build environment
##
##
##      EXAMPLES:
##
##-----------------------------------------------------------------------------------------------
##          ${CPSS_PATH}/build_cpss.sh   DX_ALL   PPC603
##
##              appDemo  goes to ./compilation_root , objects   in  ./compilation_root
##
##------------------------------------------------------------------------------------------------
##
##          ${CPSS_PATH}/build_cpss.sh   EXMXPM   PPC85XX UNZIP
##
##          The following  zip files should be located in current working directory:
##              - CPSS-PP-PM***.zip
##              - CPSS-FA-Xbar***.zip
##              - CPSS-FX***.zip
##              - EnablerSuite***.zip
##              - ExtUtils***.zip
##              - GaltisSuite***.zip
##              - ReferenceCode***.zip
##              - UT*.zip
##              - lsp*.zip
##
##              The script :
##                  - unpack CPSS code to directory     ./unpack/cpss
##                  - unpack lsp to directory ./lsp
##                  - defines
##                              export  CPSS_PATH= ./unpack/cpss
##                              export  LSP_USER_BASE= ./unpack/lsp
##                  - build   EXMXPM   appDemo   in  ./compilation_root
##
##------------------------------------------------------------------------------------------------
##
##
##          ${CPSS_PATH}/build_cpss.sh   EXMXPM   PPC85XX   ENVBASE
##             The script set CPSS build environment for defined CPU and PP_TYPE and
##             gives the prompt. In thia case a customer can work in the CPSS build
##             environment
##
##------------------------------------------------------------------------------------------------
##
##          ${CPSS_PATH}/build_cpss.sh   DX_ALL  XCAT SHLIB
##              Build CPSS Shared Lib appDemo in developer mode
##
##          ${CPSS_PATH}/build_cpss.sh   DX_ALL  XCAT SHLIB UNZIP
##              Build CPSS Shared Lib appDemo in client mode
##
##------------------------------------------------------------------------------------------------
##      No linux kernel build:
##             export LINUX_BUILD_KERNEL=NO
##      No appDemo link:
##             export DONT_LINK=NO
##      Directory to copy uImage and rootfs.tgz:
##             export TARGET_DIR_ROOTFS=path_Directory_to_copy
##
##          $Revision: 51 $
##
##################################################################


export FAST_KERNEL_COMP=TRUE

# set environment variables
if [ -z "${WORKING_DIRECTORY}" ]
then
    export WORKING_DIRECTORY=`pwd`
fi


# set LINUX_DIST_PATH
if [ -z $LINUX_DIST_PATH ]
then
    export LINUX_DIST_PATH="$WORKING_DIRECTORY/linux"
fi


# set COMPILATION_ROOT
if [ -z  $COMPILATION_ROOT ]
then
    export COMPILATION_ROOT="$WORKING_DIRECTORY/compilation_root"
    if [ ! -d $WORKING_DIRECTORY/compilation_root ]
    then
         mkdir -p $WORKING_DIRECTORY/compilation_root
    fi
    #echo "COMPILATION_ROOT=$WORKING_DIRECTORY/compilation_root"
fi

# set IMAGE_PATH
if [ -z  $IMAGE_PATH ]
then
    export IMAGE_PATH="$COMPILATION_ROOT"
fi

obj_dir=$COMPILATION_ROOT

# default settings
options=""
host_os=`uname`
target_cpu="unknown"
target_os=`uname`
pp_family="unknown"
pp_type="HW"
product="CPSS_ENABLER"
unpack_cpss="NO"
utf_opt="UTF_YES"
cust_mode="NO"
endianess=""
bus_option=""


####################
# HELP option
####################
if [ "$1" = "help" ]; then
   echo ""
   echo "   COMMAND LINE FORMAT:"

   echo "      build_cpss.sh < Packet Processor > < CPU/Compiler > < Options >"
   echo ""
   echo "   CPU/Compiler:"
   echo "      ARMADAXP ARMADAXP_BE ARMADAXP_3.4.69 ARMADA_385 ARMADA_390"
   echo "      ARM5181 ARM5281 ARM78200 ARM78200_BE ARM78200RD ARM78200RD_BE "
   echo "      EP3041A INTEL64 INTEL64_32 INTEL32 i386 i386_BE MIPS64 MSYS MSYS_2_6_32 PPC603 PPC85XX"
   echo "      XCAT_3.4 XCAT_3.4_BE XCAT XCAT_BE XCAT_2.6.31 XCAT_BE_2.6.31 MSYS_3_10 MSYS_BE"
   echo ""
   echo "   Packet Processor:"
   echo "      DX_ALL EX EXMXPM EX_DX_ALL EXMXPM_DX_CH"
   echo ""
   echo "   OPTIONS:"
   echo "      Toolkit options:"
   echo "         eldk gnueabi gnueabi_be gnu eldk_3.1 sdk_1.2.4 sdk_3.2 sdk_5_v5 sdk_5_v7 "
   echo ""
   echo "      UT options:"
   echo "         UT_NO         - UT code not included "
   echo ""
   echo "      LUA options:"
   echo "         NOLUA         - Lua code not included "
   echo ""
   echo "         NO_EXAMPLES   - remove examples from LUA code "
   echo ""
   echo "      Galtis options:"
   echo "         NOGALTIS      - Galtis code not included "
   echo ""
   echo "      GM options:"
   echo "         GM_LION GM_PUMA3 GM_BOBCAT2 GM_BOBCAT2_B0 GM_BOBK_CAELUM GM_BOBCAT3 GM_PUMA3 "
   echo ""
   echo "      Shared lib options:"
   echo "         shlib "
   echo ""
   echo "      Target product options:"
   echo "         CPSS_LIB CPSS_ENABLER CPSS_ENABLER_NOKM CPSS_BLOB CPSS_CHECK_CPP FREERTOS"
   echo ""
   echo "      SCRIPT MODE options:"
   echo "         UNZIP      - specifies the following steps: "
   echo "                       - unzip CPSS zip files to working directory "
   echo "                       - compile unziped source files "
   echo "                       - build CPSS appDemo "
   echo "         CUST       - specifies the following steps: "
   echo "                       - compile unziped source files "
   echo "                       - build CPSS appDemo "

    exit 0
fi


####################
# for Linux lua by default is inside
####################
if [ "${target_os}" != FreeBSD ]; then
    lua_options="-l"
fi

# Build lua with examples by default
no_examples=

#----------------------------------------------------------------
for arg in "$@"
do
    case $arg in
      UNZIP | unzip)
         unpack_cpss="YES"
         ;;
      CUST | cust )
         cust_mode="YES"
         ;;
    esac
done

#----------------------------------------------------------------
if [ $unpack_cpss == "YES" ] ||  [  $cust_mode  == "YES"  ]
then

    # set CPSS_UNPACK directory
    if [ -z  $CPSS_UNPACK ]
    then
        export CPSS_UNPACK="${WORKING_DIRECTORY}"
    fi

    # set LSP_UNPACK directory
    if [ -z  $LSP_UNPACK ]
    then
        export LSP_UNPACK="${WORKING_DIRECTORY}/lsp"
    fi

    if [ -z  ${CPSS_PATH} ]
    then
        export CPSS_PATH=$CPSS_UNPACK/cpss
    fi

    if [ -z  ${LSP_UNPACK} ]
    then
        export LSP_USER_BASE=$LSP_UNPACK
    fi

    # set LSP_USER_BASE
    if [ -z  $LSP_USER_BASE ]
    then
        export LSP_USER_BASE="$WORKING_DIRECTORY/lsp"
    fi

    export LIB_BASE="$CPSS_PATH"

else # if [ $unpack_cpss == "YES" ] ||  [  $cust_mode  == "YES"  ]

#----------------------------------------------------------------

       host_os="linux_marvell_cpss_dev"

       # For CC
       export RELEASE_DEP=/cpss/SW/prestera

       # set CPSS_PATH
       if [ -z $CPSS_PATH ]
       then
           #export CPSS_PATH="$WORKING_DIRECTORY/cpss"
           export CPSS_PATH=$(cd $(dirname $0) && pwd);
       fi

       echo "CPSS_PATH=$CPSS_PATH"

       export  CLONE_PATH=$(cd $(dirname $CPSS_PATH) && pwd);
       echo "CLONE_PATH=$CLONE_PATH"

       export  LINUX_CLONE_PATH=$(cd $(dirname $CLONE_PATH) && pwd);
       echo "LINUX_CLONE_PATH=$LINUX_CLONE_PATH"

       # set LSP_USER_BASE
       if [ -z  $LSP_USER_BASE ]
       then
           #export LSP_USER_BASE="$WORKING_DIRECTORY/lsp"
           export  LSP_USER_BASE="$CLONE_PATH/lsp"
           if [ "${arg}" = ARMADA_385 -o "${arg}" = MSYS_3_10 -o "$arg" = ARMADA_390 ]
           then
               export  LSP_USER_BASE="$LINUX_CLONE_PATH/uni"
           fi
       fi
       #echo "LSP_USER_BASE=$LSP_USER_BASE"

       if [ -z  $TARGET_OS ]
       then
           export TARGET_OS=Linux
       fi

       echo "WORKING_DIRECTORY=$WORKING_DIRECTORY"
       echo "CPSS_PATH=$CPSS_PATH"
       #echo "TARGET_OS=$TARGET_OS"

       if [ $TARGET_OS \!= "win32" -a TARGET_OS \!= "vxWorks" ]
       then

           eval LIB_BASE="/swdev/fileril103/Objects/cpss/bin/lib"
           export LIB_BASE="/swdev/fileril103/Objects/cpss/bin/lib"
           echo "LIB_BASE=$LIB_BASE"

           if [ -z  $NO_COPY_OBJECTS ]
           then
               export NO_COPY_OBJECTS="TRUE"
           fi

           ##################
           echo "CPSS_PATH=$CPSS_PATH"
           cd $CPSS_PATH

           export TMP_DIRECTORY=~/tmp/ctemp
           #if [ ! -d $TMP_DIRECTORY ]
           #then
                mkdir -p $TMP_DIRECTORY
           #fi

           cs_config_path=~/tmp/ctemp/git_cs_config.sh
           changed_files_path=~/tmp/ctemp/changed_files.txt

           TARGET_OS="Linux"

           LOG_LEVEL=10
           FILE_LOG_LEVEL=10

           if [ -z  $LOG_LEVEL ]
           then
               export LOG_LEVEL=10
           fi

           if [ -z  $FILE_LOG_LEVEL ]
           then
               export FILE_LOG_LEVEL=10
           fi

           if [ -z  $GET_GIT_MODE ]
           then
               export GET_GIT_MODE=1
           fi

           echo "get_git_param.py" "$GET_GIT_MODE" "$TARGET_OS" "$CPSS_PATH" "$cs_config_path" "$changed_files_path" "$LOG_LEVEL" "$FILE_LOG_LEVEL"
           python ./get_git_param.py "$GET_GIT_MODE" "$TARGET_OS" "$CPSS_PATH" "$cs_config_path" "$changed_files_path" "$LOG_LEVEL" "$FILE_LOG_LEVEL"

           unset LOG_LEVEL
           unset FILE_LOG_LEVEL
           unset GET_GIT_MODE


           chmod 777  $cs_config_path
           .  $cs_config_path

           set_cs_parameters

           echo "CC_CURRENT_CHANGE_SET =$CC_CURRENT_CHANGE_SET"
           echo "CC_CURRENT_BASE_LINE  =$CC_CURRENT_BASE_LINE"
           echo "CC_RELEASE_STREAM     =$CC_RELEASE_STREAM"
        fi

fi # if [ $unpack_cpss == "YES" ] ||  [  $cust_mode  == "YES"  ]
#----------------------------------------------------------------

# echo "unpack_cpss=${unpack_cpss}"
# echo "cust_mode=${cust_mode}"

tmp_array=()
cpu_array=()

#CPSS_PATH=/local/store/shared/git/111/493_L/cpss
CPSS_CPU_PATH=${CPSS_PATH}/tools/build/cpu
#echo "CPSS_CPU_PATH=${CPSS_CPU_PATH}"
tmp_array=($(ls -r ${CPSS_CPU_PATH}/*.sh))

for ind in "${!tmp_array[@]}"
do
    line=(${tmp_array[ind]})
    line2=${line/${CPSS_CPU_PATH}\/}
    cpu_entry=${line2/.sh/}
    cpu_array+=(${cpu_entry})
done

#----------------------------------------------------------------
for arg in "$@"
do


    #----------------------------------------------------------------
    cpu_found=0
    for ind in "${!cpu_array[@]}"
    do
        cpu_entry=(${cpu_array[ind]})

        if [ "${cpu_entry}" = "${arg}" ]; then
            cpu_found=1

            if [ "${arg}" = XCAT_BE -o "${arg}" = XCAT_BE_2.6.31 -o "${arg}" = ARM78200_BE -o "${arg}" = ARM78200RD_BE -o "${arg}" = ARMADAXP_BE ]
             then endianess="_be"
            fi

            if [ "${arg}" = ARM78200RD -o "${arg}" = ARM78200RD_BE ]
             then export LION_RD=1
            fi

            if [ "${arg}" = i386 -o "${arg}" = i386_64 ]; then
                target_os=${target_os}WM
                pp_type=simulation
            fi

             target_cpu=$arg
             break
        fi
    done
    #echo "${arg} cpu_found=${cpu_found}"
    #----------------------------------------------------------------


   case $arg in

      DX_ALL | EX | EXMXPM | EX_DX_ALL | EXMXPM_DX_CH )
         pp_family=$arg
         if [ $pp_family = "DX_ALL" ]||[ $pp_family = "EXMXPM" ];then
            lua_options="-l"
         else
            lua_options=""
         fi
         ;;

      ELDK | eldk | GNUEABI | gnueabi | GNUEABI_BE | gnueabi_be | GNU | gnu )
         arg_lo=`echo $arg | tr 'A-Z' 'a-z'`
         options="$options -T $arg_lo"
         ;;

     ELDK_3.1 | eldk_3.1 )
         options="$options -T eldk$endianess"
         ;;

     SDK_1.2.4 | sdk_1.2.4 )
         options="$options -T gnueabi$endianess"
         ;;

     SDK_3.2 | sdk_3.2 )
         options="$options -T mv5sft$endianess"
         ;;

     SDK_5_V5 | sdk_5_v5 )
         options="$options -T marvell_v5sft$endianess"
         ;;

     SDK_5_V7 | sdk_5_v7 )
         options="$options -T marvell_v7sft$endianess"
         ;;

      FOX | DUNE )
         # set xbar
         options="$options -x $arg"
         ;;

      CPSS_ENABLER | CPSS_LIB | CPSS_ENABLER_NOKM | CPSS_BLOB | CPSS_CHECK_CPP|FREERTOS)
         product=$arg
         ;;

      UTF_NO | UTF_YES  )
                        utf_opt=$arg
         options="$options -u $arg"
         ;;

      CPSS_LOG_YES | CPSS_LOG_NO )
         options="$options -a $arg"
         ;;

      SO_LIB  )
                        so_opt=$arg
         options="$options -o $arg"
         ;;

      UNZIP | unzip)
         unpack_cpss="YES"
         ;;
      ENVBASE | envbase )
         options="$options  -Denvbase"
         ;;
      CUST | cust )
         cust_mode="YES"
         ;;
      SHLIB | shlib )
         case ${product:-CPSS_ENABLER} in
           CPSS_ENABLER | CPSS_LIB)
               product="${product:-CPSS_ENABLER}_SHARED"
               ;;
           *)
               echo "The option $arg is not supported for product $product"
               exit 1
         esac
         ;;
      NOLUA | LUA_NO )
         lua_options=
         ;;
      NO_EXAMPLES )
         no_examples="-e"
         ;;
      NOGALTIS )
         galtis_options="NOGALTIS"
         options="$options  -Gno_galtis"
         ;;
      GM_LION )
         gm_options="GM_LION"
         export FORCE_GM_LION_USE="EXISTS"
         export FORCE_GM_USE="EXISTS"
         ;;
      GM_PUMA3 )
         gm_options="GM_PUMA3"
         export FORCE_GM_PUMA3_USE="EXISTS"
         ;;
      GM_BOBCAT2 )
         gm_options="GM_BOBCAT2"
         export FORCE_GM_BOBCAT2_USE="EXISTS"
         export FORCE_GM_USE="EXISTS"
         ;;
      GM_BOBCAT2_B0 )
         gm_options="GM_BOBCAT2_B0"
         export FORCE_GM_BOBCAT2_B0_USE="EXISTS"
         export FORCE_GM_USE="EXISTS"
         ;;
      GM_BOBK_CAELUM )
         gm_options="GM_BOBK_CAELUM"
         export FORCE_GM_BOBK_CAELUM_USE="EXISTS"
         export FORCE_GM_USE="EXISTS"
         ;;
      GM_BOBCAT3 )
         gm_options="GM_BOBCAT3"
         export FORCE_GM_BOBCAT3_USE="EXISTS"
         export FORCE_GM_USE="EXISTS"
         ;;
      GM_PUMA3 )
         gm_options="GM_PUMA3"
         export FORCE_GM_PUMA3_USE="EXISTS"
         ;;
	  NOKERNEL | NOLINK | NOKERNEL_NOLINK )
		 options="$options -N $arg"
         ;;
      PCI | SMI | PCI_SMI | PCI_SMI_I2C)
        options="$options -b $arg"
        ;;

      #----------------------------------------
      * )

        if [ ${cpu_found} != 1 ]; then
            echo "Wrong command line argument - $arg"
            exit 0
        fi
        ;;

   esac
done

# add lua options for build
options="$options  $lua_options $no_examples"

# threat warnings as errors if i386 && !CUST && !UNZIP
if [ "$pp_type" == "simulation" -a "$cust_mode" != "YES" -a "$unpack_cpss" != "YES" ]; then
   export THREAT_WARNINGS_AS_ERRORS=YES
fi

if [ "$target_cpu" = "unknown" ]; then
   echo "CPU parameter required"
   exit 1
fi

if [ "$pp_family" = "unknown" ]; then
   echo "PP_TYPE parameter required"
   exit 1
fi


#echo "target_cpu   =${target_cpu}"
#echo "LSP_USER_BASE=${LSP_USER_BASE}"
#----------------------------------------------------------------

##################################################
##   check  :
##                  - directory LINUX_DIST_PATH exists
##                   - linux tarball exists in LINUX_DIST_PATH
##                   - busybox tarball exists in LINUX_DIST_PATH
##  in developer mode :
##                    - copy linux tarballs from server
##                    - copy busybox tarball from server
##################################################
if  [  -d  $LINUX_DIST_PATH   ]
then
    if [ $unpack_cpss  != "YES" ] &&  [  $cust_mode  != "YES"  ]
    then
        echo "Check linux tarballs in  $LINUX_DIST_PATH"

        if [ ! -f  $LINUX_DIST_PATH/linux-3.2.22.tar.bz2 ]
        then
            echo "cp /swtools/devsources/kernel/linux-3.2.22.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/kernel/linux-3.2.22.tar.bz2  $LINUX_DIST_PATH
        fi
        if [ ! -f  $LINUX_DIST_PATH/linux-2.6.22.18.tar.bz2  ]
        then
            echo "cp /swtools/devsources/kernel/linux-2.6.22.18.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/kernel/linux-2.6.22.18.tar.bz2  $LINUX_DIST_PATH
        fi

        if [ ! -f  $LINUX_DIST_PATH/linux-2.6.31.8.tar.bz2 ]
        then
            echo "cp /swtools/devsources/kernel/linux-2.6.31.8.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/kernel/linux-2.6.31.8.tar.bz2  $LINUX_DIST_PATH
        fi

        if [ ! -f  $LINUX_DIST_PATH/linux-2.6.32.59.tar.bz2  ]
        then
            echo "cp /swtools/devsources/kernel/linux-2.6.32.59.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/kernel/linux-2.6.32.59.tar.bz2  $LINUX_DIST_PATH
        fi

        if [ ! -f  $LINUX_DIST_PATH/linux-2.6.35.13.tar.bz2  ]
        then
            echo "cp /swtools/devsources/kernel/linux-2.6.35.13.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/kernel/linux-2.6.35.13.tar.bz2  $LINUX_DIST_PATH
        fi

        if [ ! -f  $LINUX_DIST_PATH/linux-3.4.tar.bz2 ]
        then
            echo "cp /swtools/devsources/kernel/linux-3.4.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/kernel/linux-3.4.tar.bz2  $LINUX_DIST_PATH
        fi
        if [ ! -f  $LINUX_DIST_PATH/linux-3.2.40.tar.bz2 ]
        then
            echo "cp /swtools/devsources/kernel/linux-3.2.40.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/kernel/linux-3.2.40.tar.bz2  $LINUX_DIST_PATH
        fi

        if [ ! -f  $LINUX_DIST_PATH/busybox-1.01.tar.bz2  ]
        then
            echo "Copy busybox 1.01 tarball from server"
            echo "cp /swtools/devsources/root_fs/files/busybox/busybox-1.01.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/root_fs/files/busybox/busybox-1.01.tar.bz2  $LINUX_DIST_PATH
        fi
        if [ ! -f  $LINUX_DIST_PATH/busybox-1.2.1.tar.bz2  ]
        then
            echo "Copy busybox 1.2.1 tarball from server"
            echo "cp /swtools/devsources/root_fs/files/busybox/busybox-1.2.1.tar.bz2  $LINUX_DIST_PATH"
            cp /swtools/devsources/root_fs/files/busybox/busybox-1.2.1.tar.bz2  $LINUX_DIST_PATH
        fi

    fi
fi

###############################
##  unpack   cpss and lsp zips
###############################
if [ $unpack_cpss  = YES ]
then

       ##########################
        # check  if  ALL zips  exist
        ########################

        FILES_TO_UNZIP="all"

        for file in ./Cpss-PP*.zip
        do
            if [ ! -f "$file" ]
            then
                echo "Not found $file"
                FILES_TO_UNZIP="not_all"
            fi
        done


        if [ $target_cpu  != i386  ] &&  [  $target_cpu  != i386_64  ] && [ "$APP_DEMO_ONLY" != "YES" ]; then
            if [ ! -f  ./lsp*.zip ]
            then
                echo " >> Not found lsp*.zip"
                FILES_TO_UNZIP="not_all"
            fi
        fi

        if [ $pp_family  = EX  ];then
            if [ ! -f  ./Cpss-FX*.zip ]; then
                echo " >> Not found Cpss-FX*.zip"
                FILES_TO_UNZIP="not_all"
            fi
            if [ ! -f  ./Cpss-FA*.zip ];then
                echo " >> Not found Cpss-FA*.zip"
                FILES_TO_UNZIP="not_all"
            fi
        fi

        if  [ $utf_opt = UTF_YES  ]; then
            if [ ! -f  ./UT*.zip ]; then
                echo " >> Not found UT*.zip"
                FILES_TO_UNZIP="not_all"
            fi
        fi

        if  [ "$lua_options" = "-l"  ]; then
            if [ ! -f  ./LuaSuite*.zip ]; then
                echo " >> Not found LuaSuite*.zip"
                FILES_TO_UNZIP="not_all"
            fi
        fi

        # check EnablerSuite*.zip
        if [ ! -f  ./EnablerSuite*.zip ]; then
            echo " >> Not found EnablerSuite*.zip"
            FILES_TO_UNZIP="not_all"
        fi

        # check ExtUtils*.zip
        if [ ! -f  ./ExtUtils*.zip ];  then
            echo " >> Not found ExtUtils*.zip"
            FILES_TO_UNZIP="not_all"
        fi

        # check  ReferenceCode*.zip
        if [ ! -f  ./ReferenceCode*.zip ]; then
            echo " >> Not found ReferenceCode*.zip"
            FILES_TO_UNZIP="not_all"
        fi

        # check GaltisSuite*.zip
        if [ ! -f  ./GaltisSuite*.zip ]&&[ "$galtis_options" != "NOGALTIS" ]; then
            echo " >> Not found GaltisSuite*.zip"
            FILES_TO_UNZIP="not_all"
        fi

        # check Simulation*.zip
        if [ $target_cpu  = i386 -o $target_cpu  = i386_64 ]
        then

            for file in ./Simulation*.zip
            do
                if [ ! -f "$file" ]
                then
                    echo "Not found $file"
                    FILES_TO_UNZIP="not_all"
                fi
            done

            if [ $pp_family  = DX_ALL  -a  "$gm_options" = "GM_LION"  ]
            then
                if [ -f  ./GM_DX*.zip  ]
                then
                    echo "Not found GM_DX*.zip"
                    FILES_TO_UNZIP="not_all"
                fi
            fi


        fi

        if [ $FILES_TO_UNZIP != "all" ];then
            echo " >> ERROR - not all required zip files exist"
            exit 0
        fi



        ##########################
        # check  if  zips  exist and unpack
        ##########################

        # unpack Cpss-PP*.zip
#        if [ -f  ./Cpss-PP*.zip ]
#        then
#           unzip  -d $CPSS_UNPACK  -o  Cpss-PP*.zip

#        else
#            echo "Not found Cpss-PP*.zip"
#            exit 0
#        fi

        # unpack lsp*.zip
        if [ $target_cpu  != i386  ] &&  [  $target_cpu  != i386_64  ] && [ "$APP_DEMO_ONLY" != "YES" ]
        then
            if [ -f  ./lsp*.zip ]
            then
                unzip  -d $LSP_UNPACK  -o  lsp*.zip
            else
                echo "Not found lsp*.zip"
                exit 0
            fi
        fi

        if [ $pp_family  = EX  ]
        then
            if [ -f  ./Cpss-FX*.zip ]
            then
                unzip  -d $CPSS_UNPACK/cpss  -o  Cpss-FX*.zip
            else
                echo "Not found Cpss-FX*.zip"
                exit 0
            fi
            if [ -f  ./Cpss-FA*.zip ]
            then
                unzip  -d $CPSS_UNPACK/cpss  -o  Cpss-FA*.zip
            else
                echo "Not found Cpss-FA*.zip"
                exit 0
            fi
        fi

        if  [ $utf_opt = UTF_YES  ]
        then
            if [ -f  ./UT*.zip ]
            then
                unzip     -d $CPSS_UNPACK/cpss   -o UT*.zip
            else
                echo "Not found UT*.zip"
                exit 0
            fi
        else
            echo "UTF_OPTION - $utf_opt  :  UT*.zip not unpacked"
        fi

        if  [ "$lua_options" = "-l"  ]
        then
            if [ -f  ./LuaSuite*.zip ]
            then
                unzip     -d $CPSS_UNPACK/cpss   -o LuaSuite*.zip
            else
                echo "Not found LuaSuite*.zip"
                exit 0
            fi
        else
            echo "LUA_OPTIONS - $$lua_options  :  LuaSuite*.zip not unpacked"
        fi

        # unpack EnablerSuite*.zip
        if [ -f  ./EnablerSuite*.zip ]
        then
            unzip     -d $CPSS_UNPACK/cpss   -o  EnablerSuite*.zip
        else
            echo "Not found EnablerSuite*.zip"
            exit 0
        fi


        # unpack ExtUtils*.zip
        if [ -f  ./ExtUtils*.zip ]
        then
            unzip     -d $CPSS_UNPACK/cpss   -o ExtUtils*.zip
        else
            echo "Not found ExtUtils*.zip"
            exit 0
        fi

        # unpack ReferenceCode*.zip
        if [ -f  ./ReferenceCode*.zip ]
        then
            unzip     -d $CPSS_UNPACK/cpss/cpssEnabler   -o ReferenceCode*.zip
        else
            echo "Not found ReferenceCode*.zip"
            exit 0
        fi

        # unpack GaltisSuite*.zip
        if [ "$galtis_options" != "NOGALTIS" ];then

           if [ -f  ./GaltisSuite*.zip ];then
              unzip     -d $CPSS_UNPACK/cpss   -o GaltisSuite*.zip
           else
              echo "Not found GaltisSuite*.zip"
              exit 0
           fi
        fi

        if [ "$target_cpu"  = "i386"  -o "$target_cpu"  = "i386_64" ]
        then
            for file in ./Simulation*.zip
            do
              if [ -f "$file" ]
              then
                  echo "unzip  -d $CPSS_UNPACK/cpss  -o  $file"
                  unzip  -d $CPSS_UNPACK/cpss  -o  $file
                  continue
              else
                  echo "Not found $file"
                  exit 0
              fi
            done

            if [ $pp_family  = DX_ALL  -a  "$gm_options" = "GM_LION"  ]
            then
                if [ -f  ./GM_DX*.zip  ]
                then
                    unzip  -d $CPSS_UNPACK/cpss  -o  GM_DX*.zip
                else
                    echo "Not found GM_DX*.zip"
                    exit 0
                fi
            fi
        fi

    cd cpss
    chmod -R -c 777 *
    cd ..
fi

######################################################
##          CUST mode
##     should be used after UNZIP required  CPSS zip files
######################################################
#if [ $cust_mode  = YES ]
#then




    # check if CPSS was unziped
#    if [ ! -d ${CPSS_PATH} ]
#    then
#        echo " CPSS not unziped "
#        exit 0
#    fi

    # check if  LSP was unziped

#    if [ $target_cpu  != i386 ] &&  [  $target_cpu  != i386_64  ] && [ "$APP_DEMO_ONLY" != "YES" ]
#    then
#        if [ ! -d $LSP_UNPACK ]
#        then
#            echo " LSP not unziped "
#            exit 0
#        fi
#    fi
#fi

# Enable parallel build
export MAKEFLAGS=-j24

# for CleaCase mode - to allow copy of objects from server
if [ $unpack_cpss  != "YES" ] &&  [  $cust_mode  != "YES"  ]
then
   host_os="linux_marvell_cpss_dev"
fi

########################################
## RUN BUILD.SH WITH DEFINED PARAMETERS
########################################

echo "--------------------------------------"
echo "WORKING_DIRECTORY - $WORKING_DIRECTORY"
echo "COMPILATION_ROOT - $COMPILATION_ROOT"
echo "CPSS_PATH - $CPSS_PATH"
echo "LSP_USER_BASE - $LSP_USER_BASE"
echo "LINUX_DIST_PATH - $LINUX_DIST_PATH"

echo "options    - $options   "
echo "host_os    - $host_os   "
echo "target_cpu - $target_cpu"
echo "target_os  - $target_os "
echo "pp_family  - $pp_family "
echo "pp_type    - $pp_type   "
echo "product    - $product   "
echo "obj_dir    - $obj_dir   "
echo "IMAGE_PATH - $IMAGE_PATH   "
echo "--------------------------------------"

echo "bash ${CPSS_PATH}/Build.sh \
   $options   \
   $host_os $target_cpu $target_os \
   $pp_family $pp_type $product \
   $obj_dir $IMAGE_PATH"

bash ${CPSS_PATH}/Build.sh \
   $options   \
   $host_os $target_cpu $target_os \
   $pp_family $pp_type $product \
   $obj_dir $IMAGE_PATH


unset NO_COPY_OBJECTS



