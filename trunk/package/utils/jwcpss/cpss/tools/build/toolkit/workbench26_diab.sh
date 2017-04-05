# Toolkit definitions for
# ???
#
# In *PSS projects it is used to build vxWorks targets
#
# ???

export TOOL=diab
export TOOL_ENV=workbench
export TOOLKIT_MAKE_RULES=workbench26_diab
export COMPILER=GNU

export WIND_DIR="C:\\Workbench_2.6"
export WRSD_LICENSE_FILE='C:/Workbench_2.6/license'
export WIND_PREFERRED_PACKAGES=vxworks-6.5
export WIND_HOME="C:\\Workbench_2.6"
export WIND_PLATFORM=vxworks-6.5
export WIND_BASE="$WIND_HOME\\$WIND_PREFERRED_PACKAGES"
export WIND_USR="$WIND_BASE\\target\\usr"
export WIND_HOST_TYPE=x86-win32

export WIND_TOOLS="$WIND_HOME\\workbench-2.6"
export WIND_GNU_PATH="$WIND_HOME\\gnu\\3.4.4-vxworks-6.5"

export WIND_DIAB_PATH="$WIND_HOME\\diab\\5.5.1.0"

export LD_LIBRARY_PATH="$WIND_BASE\\host\\$WIND_HOST_TYPE\\lib;$WIND_BASE\\wrwb\\platform\\eclipse\\$WIND_HOST_TYPE\\bin;$WIND_BASE\\$WIND_HOST_TYPE\\lib;$WIND_BASE\\foundation\\4.0.11\\$WIND_HOST_TYPE\\lib"


export COMP_IPNET2=ip_net2-6.5
export WIND_COMPONENTS_LIBPATHS="$WIND_HOME\\components\\obj\\$WIND_PLATFORM\\krnl\\lib"
export WIND_COMPONENTS_INCLUDES="$WIND_HOME\\components\\$COMP_IPNET2\\ipcom\\include;$WIND_HOME\\components\\$COMP_IPNET2\\ipcom\\config;$WIND_HOME\\components\\$COMP_IPNET2\\ipcom\\port\\vxworks\\config;"

export DIABLIB="$WIND_HOME\\diab\\5.5.1.0"
win32_add_paths "$WIND_DIAB_PATH\\$WIN32\\bin"
export COMPILER=WBC

win32_add_paths "$WIND_TOOLS\\$WIND_HOST_TYPE\\bin"
win32_add_paths "$WIND_BASE"
win32_add_paths "$WIND_BASE\\host\\$WIND_HOST_TYPE\\bin"
win32_add_paths "$WIND_TOOLS\\foundation\\4.0.11\\$WIND_HOST_TYPE\\bin"


#CPU_DIR=${CPU_DIR}_DIAB
TOOLKIT_MAKE_RULES=workbench26_diab
