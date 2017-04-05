# Build freeRTOS

. $tool_build/product/cpss_common.inc


os_pre_build()
{
    return 0
}

os_build()
{  
   ${CPSS_PATH}/freeRTOS/build_free_rtos.sh
}

os_link()
{
    return 0
}

product_post_build()
{
    trace_message "=> END Build freeRTOS"
    exit 0
}

