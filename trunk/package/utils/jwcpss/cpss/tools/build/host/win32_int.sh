
# host definitions Marvell internal environment
# (WIN32)
#
#

. $tool_build/host/win32.sh



host_post_build()
{

   if [ -f  $DESTINATION_BSP_DIR/appDemoSim.exe ]   
   then
      touch $COMPILATION_ROOT/token.$FAMILY
   fi
    return 0
}

