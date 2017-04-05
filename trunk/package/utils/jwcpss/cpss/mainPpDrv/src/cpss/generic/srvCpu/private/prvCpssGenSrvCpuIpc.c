/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChOamSrv.c
*
* DESCRIPTION:
*       Service CPU IPC API's
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/extServices/os/gtOs/gtGenTypes.h>

#ifdef LINUX
#include <unistd.h>
#include <fcntl.h>

#define IPC_MAX_MSG_SIZE		(0x200) /* sync with usrappl and rtos. giora*/
#define PRV_CPSS_GEN_SRV_CPU_IPC_MSG_MAX_SIZE	IPC_MAX_MSG_SIZE

#endif
/*******************************************************************************
* prvCpssGenSrvCpuIpcSendMessage
*
* DESCRIPTION:
*       Send a message to the service CPU.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; 
*       Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                   - device number.
*       msgPtr                   - (pointer to) message.
*       msgSize                  - message size.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenSrvCpuIpcMessageSend
(
    IN GT_U8    devNum,
    IN GT_U8    *msgPtr,
    IN GT_U32   msgSize
)
{
#if defined(LINUX) && !defined(CPSS_BLOB)
    GT_STATUS   rc = GT_OK;/* function return code*/
    char        buf[1024]; /* message buffer*/
    int         len; /* message length*/
    int         scfd;/* file descriptor*/

    CPSS_NULL_PTR_CHECK_MAC(msgPtr);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
    CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    cpssOsMemCpy(&buf, msgPtr, msgSize);
    len = msgSize + 4;
    scfd = open("/dev/mvSc", O_RDWR | O_SYNC);
    if(write(scfd, buf, len) != len)
    {
        rc = GT_FAIL;
    }
    if(close(scfd) != 0)
        rc = GT_FAIL;
    return rc;
#else
    (GT_VOID)devNum;
    (GT_VOID)msgPtr;
    (GT_VOID)msgSize;
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
#endif
}

/*******************************************************************************
* prvCpssGenSrvCpuIpcMessageGet
*
* DESCRIPTION:
*       Get a message from the service CPU.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2;
*       Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum                   - device number.
*
* OUTPUTS:
*       msgPtr                   - (pointer to) message.
*       msgSizePtr               - (pointer to) message size.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_BAD_PARAM             - wrong devNum or flowId.
*       GT_BAD_PTR               - one of the parameters is NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssGenSrvCpuIpcMessageGet
(
    IN GT_U8    devNum,
    OUT GT_U8   *msgPtr,
    OUT GT_U32  *msgSizePtr
)
{
#if defined(LINUX) && !defined(CPSS_BLOB)
    char        buf[1024]; /* message buffer*/
    int         len; /* message length*/
    int         scfd;/* file descriptor*/

    CPSS_NULL_PTR_CHECK_MAC(msgPtr);
    CPSS_NULL_PTR_CHECK_MAC(msgSizePtr);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
    CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    scfd = open("/dev/mvSc", O_RDWR | O_SYNC);
	len = read(scfd, buf, PRV_CPSS_GEN_SRV_CPU_IPC_MSG_MAX_SIZE);
    if(close(scfd) != 0)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
	if(len <=0)
    {
        *msgSizePtr = 0;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
	}    
    cpssOsMemCpy(msgPtr, buf, len);
    *msgSizePtr = len;
    return GT_OK;
#else
    (GT_VOID)devNum;
    (GT_VOID)msgPtr;
    (GT_VOID)msgSizePtr;
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
#endif
}

