/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDxChNetIf.c
*
* DESCRIPTION:
*       Include DxCh network interface API functions
*
*
* FILE REVISION NUMBER:
*       $Revision: 78 $
*******************************************************************************/
#define CPSS_LOG_IN_MODULE_ENABLE
#include <cpss/dxCh/dxChxGen/networkIf/private/prvCpssDxChNetworkIfLog.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cos/private/prvCpssDxChCoS.h>
#include <cpss/dxCh/dxChxGen/trunk/private/prvCpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>

/* constant to show "vlan instead vidx"*/
#define VID_INSTEAD_VIDX_CNS            0xfff
#define VID_INSTEAD_VIDX_9_BITS_CNS     0x1ff

/* macro convert the RX cpu code to DSA code
   NOTE : macro return on error !!!
*/
#define RX_OPCODE_TO_DSA_CONVERT_MAC(rxCode,dsaCodePtr)                    \
    {                                                                      \
        GT_STATUS rcTemp = prvCpssDxChNetIfCpuToDsaCode(rxCode,dsaCodePtr);\
        if(rcTemp != GT_OK)                                                \
        {                                                                  \
            return rcTemp;                                                 \
        }                                                                  \
    }

/* macro convert the DSA code RX cpu code
   NOTE : macro return on error !!!
*/
#define DSA_TO_RX_OPCODE_CONVERT_MAC(dsaCode,rxCodePtr)                    \
    {                                                                      \
        GT_STATUS rcTemp = prvCpssDxChNetIfDsaToCpuCode(dsaCode,rxCodePtr);\
        if(rcTemp != GT_OK)                                                \
        {                                                                  \
            return rcTemp;                                                 \
        }                                                                  \
    }

/* the max valid index in the table of : cpu code ip protocol */
#define DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS        7

/* the max valid index in the TCP/UPD Destination Port Range CPU Code Table */
#define DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS        15

/* rate limiter index to be in the "cpu code table" */
#define CPU_CODE_RATE_LIMITER_INDEX_CHECK_MAC(index)    \
    if(index > 31)                                  \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* statistical rate limiter index to be used when access the "statistical rate limiter table" */
#define CPU_CODE_STATISTICAL_RATE_LIMITER_INDEX_CHECK_MAC(index)   \
    if(index > 31 )                                   \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* designated device index to be used when access the "designated device table" */
#define CPU_CODE_DESIGNATED_DEVICE_INDEX_CHECK_MAC(index)          \
    if(index > 7 )                                    \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

/* rate limiter index to be used when access the "rate limiter table" */
#define CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(dev, index)    \
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(dev) == GT_TRUE) { \
        if(index > 255 || index == 0)                                   \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                        \
    } else {                                                            \
        if(index > 31 || index == 0)                                    \
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);                                        \
    }

/* designated device index to be used when access the "designated device table" */
#define CPU_CODE_DESIGNATED_DEVICE_INDEX_AS_POINTER_CHECK_MAC(index)          \
    if(index > 7 || index == 0)                                    \
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG)

static GT_BOOL debug_dumpDsa = GT_FALSE;

/*******************************************************************************
* dxChNetIfDsaRegularTagParse
*
* DESCRIPTION:
*       parse the DSA tag parameters from the DSA tag on the packet. --
*           ===== regular DSA tag ====
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaWord - the DSA tag in the packet (single word -> 32 bits)
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet --> All the common fields are already set !
*                   and the DSA tag
*                   All other fields are "reset"
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR       - one of the parameters in NULL pointer.
*
*
* COMMENTS:
*           ===== regular DSA tag ====
*
*******************************************************************************/
static GT_STATUS dxChNetIfDsaRegularTagParse
(
    IN  GT_U8                          devNum,
    IN  GT_U32                         dsaWord,
    INOUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    GT_STATUS   rc;
    CPSS_DXCH_NET_DSA_TO_CPU_STC         *toCpuPtr = &(dsaInfoPtr->dsaInfo.toCpu);
    CPSS_DXCH_NET_DSA_FROM_CPU_STC       *fromCpuPtr = &(dsaInfoPtr->dsaInfo.fromCpu);
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    *toAnalyzerPtr = &(dsaInfoPtr->dsaInfo.toAnalyzer);
    CPSS_DXCH_NET_DSA_FORWARD_STC        *forwardPtr = &(dsaInfoPtr->dsaInfo.forward);
    GT_U32  cpuCode; /* cpu code  -- for "to cpu" */
    GT_U16  vidx;
    GT_U32  dummyPort = 0;/*dummy port to use when no valid port can be used */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    switch(dsaInfoPtr->dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            toCpuPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,
                                1);

            toCpuPtr->hwDevNum = U32_GET_FIELD_MAC(dsaWord,
                                          24,
                                          5);

            if(toCpuPtr->srcIsTrunk == GT_TRUE)
            {
                /* trunk */

                /* 5 bits */
                toCpuPtr->interface.srcTrunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(dsaWord,
                                                19, /*19*/
                                                5); /*5*/
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toCpuPtr->hwDevNum,dummyPort);
            }
            else
            {
                /* port */

                /* 5 bits */
                toCpuPtr->interface.portNum =
                        U32_GET_FIELD_MAC(dsaWord,
                        19,/*19*/
                        5);  /*5*/
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toCpuPtr->hwDevNum,toCpuPtr->interface.portNum);
            }

           cpuCode =
                (GT_U8)(((U32_GET_FIELD_MAC(dsaWord ,
                              16,
                              3))<< 1) |
                        U32_GET_FIELD_MAC(dsaWord,
                        12,
                        1));

            /* convert CPU code from DSA to the unified "RX CPU code"*/
            rc = prvCpssDxChNetIfDsaToCpuCode(cpuCode,&toCpuPtr->cpuCode);
            if(rc != GT_OK)
            {
                /* should not happen */
                return rc;
            }
        break;

        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            toAnalyzerPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,  /*29*/
                                1);    /*1*/
            toAnalyzerPtr->devPort.hwDevNum = U32_GET_FIELD_MAC(dsaWord,
                                          24,  /*24*/
                                          5);    /*5*/

            /* src is never trunk !!! */

            /* 5 bits */
            toAnalyzerPtr->devPort.portNum = U32_GET_FIELD_MAC(dsaWord,
                                            19, /*19*/
                                            5); /*5*/
            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toAnalyzerPtr->devPort.hwDevNum,toAnalyzerPtr->devPort.portNum);

            toAnalyzerPtr->rxSniffer = (GT_BOOL)
                    U32_GET_FIELD_MAC(dsaWord,
                                    18, /*18*/
                                    1);/*1*/
            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            forwardPtr->srcIsTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,  /*29*/
                                1);    /*1*/
            forwardPtr->srcHwDev = (GT_U8)U32_GET_FIELD_MAC(dsaWord,
                                          24,  /*24*/
                                          5);    /*5*/

            forwardPtr->srcIsTrunk = (GT_BOOL)U32_GET_FIELD_MAC(dsaWord,
                                       18,/*18*/
                                       1);  /*1*/


            if(forwardPtr->srcIsTrunk == GT_TRUE)
            {
                /* trunk */

                /* 5 bits */
                forwardPtr->source.trunkId = (GT_TRUNK_ID)
                        U32_GET_FIELD_MAC(dsaWord,
                                      19,/*19*/
                                      5);  /*5*/

                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(forwardPtr->srcHwDev,dummyPort);
            }
            else
            {
                /* port */

                /* 5 bits */
                forwardPtr->source.portNum = U32_GET_FIELD_MAC(dsaWord,19,5);
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(forwardPtr->srcHwDev,forwardPtr->source.portNum);
            }
            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            if(U32_GET_FIELD_MAC(dsaWord,18,1))/* use vidx */
            {
                /* bit 19 is reserved and is not part of the vidx bits !!! */

                vidx = (GT_U16)
                        U32_GET_FIELD_MAC(dsaWord,
                                20,
                                9 );

                /* check if VIDX means VLAN_ID */
                if(vidx == VID_INSTEAD_VIDX_9_BITS_CNS)
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
                    fromCpuPtr->dstInterface.vlanId = dsaInfoPtr->commonParams.vid;
                }
                else
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
                    fromCpuPtr->dstInterface.vidx = vidx;
                }
            }
            else   /* not using vidx */
            {
                /* destination trunk is not supported */
                fromCpuPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;

                fromCpuPtr->dstInterface.devPort.portNum = (GT_U8)
                    /* 5 bits */
                    U32_GET_FIELD_MAC(dsaWord,
                                    19,
                                    5) ;

                fromCpuPtr->dstInterface.devPort.hwDevNum = (GT_U8)
                    U32_GET_FIELD_MAC(dsaWord,
                                            24, /*24*/
                                            5); /*5*/
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(fromCpuPtr->dstInterface));

                fromCpuPtr->extDestInfo.devPort.dstIsTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(dsaWord,
                                29,  /*29*/
                                1);    /*1*/

            }
            break;
        default:/* default for compilation warnings */
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChNetIfDsaTagParse
*
* DESCRIPTION:
*       parse the DSA tag parameters from the DSA tag on the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_DSA_PARSING_ERROR     - DSA tag parsing error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*           support:
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*           ===== eDSA tag         ====
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfDsaTagParse
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          *dsaBytesPtr,
    OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    GT_STATUS   rc;
    GT_U32  longCpuCode; /*long cpu code  -- for "to cpu" */
    GT_U32  wordsPtr[4] = { 0 }; /* DSA tag can be up to 4 words */
    CPSS_DXCH_NET_DSA_TO_CPU_STC         *toCpuPtr;
    CPSS_DXCH_NET_DSA_FROM_CPU_STC       *fromCpuPtr;
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    *toAnalyzerPtr;
    CPSS_DXCH_NET_DSA_FORWARD_STC        *forwardPtr;
    GT_TRUNK_ID  excludeTrunkId;
    GT_U32  excludeIsTrunk;
    GT_U16  vidx;
    GT_BOOL extendedDsa;
    /* added bytes to original packet size (FEr 89).*/
    GT_U32  addedOrigPacketSize;
    GT_U32  dsaSizeInBytes = 8; /* size of DSA tag in bytes, default is extended DSA */
    GT_U32  getValue; /* used to get needed bits from the DSA */
    GT_U32  dummyPort = 0;/*dummy port to use when no valid port can be used */
    GT_BOOL tag0SrcIsTagged;
    GT_BOOL tag1SrcIsTagged;
    GT_U32  isSip5_20;
    GT_U32  useVidx;
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dsaBytesPtr);
    CPSS_NULL_PTR_CHECK_MAC(dsaInfoPtr);
    isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);

    /* need to copy for alignment issues*/
    wordsPtr[0] = (((GT_U32)dsaBytesPtr[0]) << 24) |
                  (((GT_U32)dsaBytesPtr[1]) << 16) |
                  (((GT_U32)dsaBytesPtr[2]) << 8)  |
                  (((GT_U32)dsaBytesPtr[3]) << 0) ;

    cpssOsMemSet(dsaInfoPtr,0,sizeof(*dsaInfoPtr));

    /* set the common parameters */
    dsaInfoPtr->dsaType = (CPSS_DXCH_NET_DSA_CMD_ENT)
                                U32_GET_FIELD_MAC(wordsPtr[0] ,30,2);

    dsaInfoPtr->commonParams.vid = (GT_U16)U32_GET_FIELD_MAC(wordsPtr[0],0,12);


    dsaInfoPtr->commonParams.vpt = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],13,3);

    extendedDsa = GT_TRUE;

    /* check the extended bit */
    if(0 == U32_GET_FIELD_MAC(wordsPtr[0] ,12,1))
    {
        extendedDsa = GT_FALSE;
    }
    else if(dsaInfoPtr->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
    {
        /* check that the "old" cpu opcode is set the 0xF (with the extended bit) */
        if(0x7 != U32_GET_FIELD_MAC(wordsPtr[0] ,16,3))
        {
            extendedDsa = GT_FALSE;
        }
    }

    if(dsaInfoPtr->dsaType != CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
    {
        dsaInfoPtr->commonParams.cfiBit =
            (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],16,1);

    }

    if(debug_dumpDsa)
    {
        cpssOsPrintf("DSA dump : word 0 [0x%8.8x] \n",wordsPtr[0]);
    }

    if(extendedDsa == GT_FALSE)
    {
        dsaInfoPtr->commonParams.dsaTagType = CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT;

        /* we must set all the common parameters prior to calling next
           function */

        return dxChNetIfDsaRegularTagParse(devNum,wordsPtr[0],dsaInfoPtr);
    }

    /* copy the data of the second word */
    wordsPtr[1] = (((GT_U32)dsaBytesPtr[4]) << 24) |
                  (((GT_U32)dsaBytesPtr[5]) << 16) |
                  (((GT_U32)dsaBytesPtr[6]) <<  8) |
                  (((GT_U32)dsaBytesPtr[7]) <<  0) ;

    if(debug_dumpDsa)
    {
        cpssOsPrintf("           word 1 [0x%8.8x] \n",wordsPtr[1]);
    }

    /* check the extended bit */
    if(0 == U32_GET_FIELD_MAC(wordsPtr[1],31,1))
    {
        /* word 1 is the last word in the DSA tag */
        dsaInfoPtr->commonParams.dsaTagType = CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT;
    }
    else
    {

         /* copy the data of the third word */
        wordsPtr[2] = (((GT_U32)dsaBytesPtr[8]) << 24) |
                      (((GT_U32)dsaBytesPtr[9]) << 16) |
                      (((GT_U32)dsaBytesPtr[10]) <<  8) |
                      (((GT_U32)dsaBytesPtr[11]) <<  0) ;

        if(debug_dumpDsa)
        {
            cpssOsPrintf("           word 2 [0x%8.8x] \n",wordsPtr[2]);
        }

        /* check the extended bit */
        if(0 == U32_GET_FIELD_MAC(wordsPtr[2],31,1))
        {
            /* 3 words DSA tag is not supported */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
        }

        /* copy the data of the forth word */
        wordsPtr[3] = (((GT_U32)dsaBytesPtr[12]) << 24) |
                      (((GT_U32)dsaBytesPtr[13]) << 16) |
                      (((GT_U32)dsaBytesPtr[14]) <<  8) |
                      (((GT_U32)dsaBytesPtr[15]) <<  0) ;

        if(debug_dumpDsa)
        {
            cpssOsPrintf("           word 3 [0x%8.8x] \n",wordsPtr[3]);
        }

        /* word 3 is the last word in the DSA tag */
        dsaInfoPtr->commonParams.dsaTagType = CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT;
        dsaSizeInBytes = 16;
    }

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
       /* VID becomes 16b eVLAN. eVLAN[15:0] = {Word3[30:27], Word0[11:0]} */

        /* 4 more bits */
        getValue = U32_GET_FIELD_MAC(wordsPtr[3],27,4);
        U16_SET_FIELD_MAC(dsaInfoPtr->commonParams.vid,12,4,getValue);
    }
    toCpuPtr = &(dsaInfoPtr->dsaInfo.toCpu);
    fromCpuPtr = &(dsaInfoPtr->dsaInfo.fromCpu);
    toAnalyzerPtr = &(dsaInfoPtr->dsaInfo.toAnalyzer);
    forwardPtr = &(dsaInfoPtr->dsaInfo.forward);

    if(dsaInfoPtr->dsaType == CPSS_DXCH_NET_DSA_CMD_TO_CPU_E)
    {
        dsaInfoPtr->commonParams.cfiBit =
            (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1],30,1);
    }

    switch(dsaInfoPtr->dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:

            toCpuPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[0],29,1);

            toCpuPtr->hwDevNum = U32_GET_FIELD_MAC(wordsPtr[0],24,5);

            toCpuPtr->srcIsTrunk = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[1],27,1);


            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* SrcDev/TrgDev is extended to 12b
                   SrcDev/TrgDev[11:0] = {Word3[6:0], Word0[28:24]} */

                /* 7 more bits */
                getValue = U32_GET_FIELD_MAC(wordsPtr[3],0,7);
                U32_SET_FIELD_MAC(toCpuPtr->hwDevNum,5,7,getValue);

                if(toCpuPtr->srcIsTrunk == GT_TRUE)
                {
                    /* SrcTrunk is extended to 12b
                       SrcTrunk[11:0] = {Word2[14:3] */

                    /* 12 bits */
                    toCpuPtr->interface.srcTrunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(wordsPtr[2],3,12);
                    PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toCpuPtr->hwDevNum,dummyPort);
                }
                else
                {
                    /* set ePort */

                    /* When toCpuPtr->isEgressPipe = GT_FALSE:
                       this field indicates the source ePort number assigned by the ingress device.
                       When toCpuPtr->isEgressPipe = GT_TRUE:
                       This field indicates the target ePort number assigned by the ingress device. */
                    /* SRCePort[16:0]/TRGePort[16:0]/ = {Word2[19:3]} */
                    toCpuPtr->interface.ePort = U32_GET_FIELD_MAC(wordsPtr[2],3,17);
                }


                /* SrcPort/TrgPort extended to 8b
                    SrcPort/TrgPort[7:0] = {Word2[20], Word1[11:10], Word0[23:19]} */
                toCpuPtr->interface.portNum =
                                (U32_GET_FIELD_MAC(wordsPtr[0],19,5) << 0) |
                                (U32_GET_FIELD_MAC(wordsPtr[1],10,2) << 5) |
                                (U32_GET_FIELD_MAC(wordsPtr[2],20,1) << 7) ;

                if(isSip5_20)
                {
                    /* SrcPort/TrgPort extended to 9b
                        SrcPort/TrgPort[8:0] = {Word2[21:20], Word1[11:10], Word0[23:19]} */
                    toCpuPtr->interface.portNum |=
                                (U32_GET_FIELD_MAC(wordsPtr[2],21,1) << 8) ;
                }


                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toCpuPtr->hwDevNum,toCpuPtr->interface.portNum);

                if (U32_GET_FIELD_MAC(wordsPtr[2],25,1))
                {
                    /* set TT offset */
                    toCpuPtr->flowIdTtOffset.ttOffset = U32_GET_FIELD_MAC(wordsPtr[3],7,20);
                }
                else
                {
                    /* set flow ID */
                    toCpuPtr->flowIdTtOffset.flowId = U32_GET_FIELD_MAC(wordsPtr[3],7,20);
                }
            }
            else /* extended DSA (not eDSA) */
            {
                if(toCpuPtr->srcIsTrunk == GT_TRUE)
                {
                    /* trunk */

                    /* 5 bits */
                    toCpuPtr->interface.srcTrunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                    /* 2 more bits */
                    toCpuPtr->interface.srcTrunkId |= (GT_TRUNK_ID)
                            (U32_GET_FIELD_MAC(wordsPtr[1],10,2)) << 5;

                    PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toCpuPtr->hwDevNum,dummyPort);
                }
                else
                {
                    /* port */

                    /* 5 bits */
                    toCpuPtr->interface.portNum = U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                    /* 1 more bit */
                    toCpuPtr->interface.portNum |=
                            (U32_GET_FIELD_MAC(wordsPtr[1],10,1)) << 5;

                    PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toCpuPtr->hwDevNum,toCpuPtr->interface.portNum);
                }
            }


            toCpuPtr->isEgressPipe = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1],8,1);

            toCpuPtr->originByteCount = U32_GET_FIELD_MAC(wordsPtr[1],12,14);

            toCpuPtr->timestamp = U32_GET_FIELD_MAC(wordsPtr[1],12,14) << 1;
            toCpuPtr->timestamp |= U32_GET_FIELD_MAC(wordsPtr[1],9,1);

            /* When a packet is received from a remote device, the CPU should
              calculate the correct byte count by decrementing the DSA tag
              size from the PktOrigBC. (FEr#89)*/
            if((PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
               PRV_CPSS_DXCH_PACKET_ORIG_BYTE_COUNT_WA_E) == GT_TRUE) &&
               (toCpuPtr->isEgressPipe == 0) && (toCpuPtr->hwDevNum != PRV_CPSS_HW_DEV_NUM_MAC(devNum)))
            {
                if(toCpuPtr->isTagged == 1)
                {
                    /* DSA tag bytes has replaced 4 bytes VLAN tag.
                      Remove 4 bytes of VLAN tag from added size. */
                    addedOrigPacketSize = dsaSizeInBytes - 4;
                }
                else
                {
                    /* DSA tag bytes were added. */
                    addedOrigPacketSize = dsaSizeInBytes;
                }

                toCpuPtr->originByteCount -= addedOrigPacketSize;
            }

            toCpuPtr->wasTruncated = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1],26,1);

            longCpuCode = U32_GET_FIELD_MAC(wordsPtr[1],0,8);

            /* convert CPU code from DSA to the unified "RX CPU code"*/
            rc = prvCpssDxChNetIfDsaToCpuCode(longCpuCode,&toCpuPtr->cpuCode);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 29, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 28, 1);
            }

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* get data from word 2 */

                /* packetIsTT = {Word2[25]}*/
                toCpuPtr->packetIsTT = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[2],25,1);

                /* tag0TpidIndex = {Word2[0:2]}*/
                toCpuPtr->tag0TpidIndex = U32_GET_FIELD_MAC(wordsPtr[2],0,3);
            }

        break;

        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            toAnalyzerPtr->isTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[0],29,1);

            toAnalyzerPtr->devPort.hwDevNum =  U32_GET_FIELD_MAC(wordsPtr[0],24,5);

            /* src is never trunk !!! */

            /* 5 bits */
            toAnalyzerPtr->devPort.portNum = U32_GET_FIELD_MAC(wordsPtr[0],19,5);

            /* 1 more bit */
            toAnalyzerPtr->devPort.portNum |=
                    (U32_GET_FIELD_MAC(wordsPtr[1],10,1)) << 5;

            toAnalyzerPtr->rxSniffer = (GT_BOOL)
                    U32_GET_FIELD_MAC(wordsPtr[0],18,1);

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 29, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 28, 1);
            }

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                 /* Device number is extended to 12b
                   SrcDev/TrgDev/AnalyzerDev[11:0] = {Word3[6:0], Word0[28:24]} */

                /* 7 more bits */
                getValue = U32_GET_FIELD_MAC(wordsPtr[3],0,7);
                U32_SET_FIELD_MAC(toAnalyzerPtr->devPort.hwDevNum,5,7,getValue);

                /* Physical ports are extended to 8b
                   SrcPort/TrgPort/AnalyzerPort[7:0] =
                                {Word2[20], Word1[11:10], Word0[23:19]} */

                 /* 2 more bits */
                getValue = U32_GET_FIELD_MAC(wordsPtr[1],11,1);
                U32_SET_FIELD_MAC(toAnalyzerPtr->devPort.portNum,6,1,getValue);

                 getValue = U32_GET_FIELD_MAC(wordsPtr[2],20,1);
                U32_SET_FIELD_MAC(toAnalyzerPtr->devPort.portNum,7,1,getValue);

                /* Get new fields */
                if(U32_GET_FIELD_MAC(wordsPtr[1],26,1))/* The target Analyzer interface useVix */
                {
                    /* eVIDX The target analyzer interface eVIDX */
                    vidx = (GT_U16)U32_GET_FIELD_MAC(wordsPtr[3],7,16);

                    if(vidx == VID_INSTEAD_VIDX_CNS)
                    {
                        toAnalyzerPtr->analyzerTrgType = CPSS_INTERFACE_VID_E;
                    }
                    else
                    {
                        toAnalyzerPtr->analyzerTrgType = CPSS_INTERFACE_VIDX_E;
                        toAnalyzerPtr->extDestInfo.multiDest.analyzerEvidx = vidx;
                    }
                }
                else
                {
                    toAnalyzerPtr->analyzerTrgType = CPSS_INTERFACE_PORT_E;

                    /* The target Analyzer interface indication that the target
                       analyzer port field <Reserved Analyzer_TrgPort> has a valid value */

                    if(U32_GET_FIELD_MAC(wordsPtr[1],27,1))
                    {
                        toAnalyzerPtr->extDestInfo.devPort.analyzerIsTrgPortValid = GT_TRUE;

                        /* The target Analyzer interface physical port number */
                        toAnalyzerPtr->extDestInfo.devPort.analyzerTrgPort = U32_GET_FIELD_MAC(wordsPtr[1],0,8);

                    }
                    else
                    {
                        toAnalyzerPtr->extDestInfo.devPort.analyzerIsTrgPortValid = GT_FALSE;
                    }

                    /* The target analyzer interface ePort */
                    toAnalyzerPtr->extDestInfo.devPort.analyzerTrgEport = U32_GET_FIELD_MAC(wordsPtr[3],7,17);
                }

                /* The target Analyzer interface device number */
                toAnalyzerPtr->extDestInfo.devPort.analyzerHwTrgDev = U32_GET_FIELD_MAC(wordsPtr[1],14,12);

               /* When in in source based forwarding mode: TBD.
                   When in hop-by-hop mode:
                   When <rxSniffer>= GT_TRUE: SRCePort:
                        Together with SrcDev, SRCePort indicates the packet's
                        original Ingress ePort assignment.
                   When <rxSniffer> = GT_FALSE: TRGePort:
                        Together with TrgDev, TRGePort indicates the packet's
                        assigned egress ePort. */

                /* get data from word 2 */

                /* SRCePort[16:0]/TRGePort[16:0]/ = {Word2[19:3]} */
                toAnalyzerPtr->devPort.ePort = U32_GET_FIELD_MAC(wordsPtr[2],3,17);

                /* tag0TpidIndex = {Word2[0:2]}*/
                toAnalyzerPtr->tag0TpidIndex = U32_GET_FIELD_MAC(wordsPtr[2],0,3);

            }

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(toAnalyzerPtr->devPort.hwDevNum,toAnalyzerPtr->devPort.portNum);

            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            useVidx = U32_GET_FIELD_MAC(wordsPtr[1],12,1);
            forwardPtr->srcIsTagged = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)U32_GET_FIELD_MAC(wordsPtr[0],29,1);

            forwardPtr->srcHwDev = U32_GET_FIELD_MAC(wordsPtr[0],24,5);

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* SrcDev is extended to 12b in sip5 devices, and then reduced to 10b in sip5.20
                   SrcDev[11:0] = {Word2[20:14], Word0[28:24]} (sip5)
                   SrcDev[9:0]  = {Word2[18:14], Word0[28:24]} (sip5.20)
                */
                getValue = isSip5_20 ?
                    U32_GET_FIELD_MAC(wordsPtr[2],14,5):
                    U32_GET_FIELD_MAC(wordsPtr[2],14,7);
                U32_SET_FIELD_MAC(forwardPtr->srcHwDev,5,7,getValue);

            }

            forwardPtr->srcIsTrunk = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[0],18,1);

            if(forwardPtr->srcIsTrunk == GT_TRUE)
            {
                /* trunk */

                /* 5 bits */
                forwardPtr->source.trunkId = (GT_TRUNK_ID)
                        U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                /* 2 more bits */
                forwardPtr->source.trunkId |= (GT_TRUNK_ID)
                        (U32_GET_FIELD_MAC(wordsPtr[1],29,2))<< 5;

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    /* SrcTrunk is extended to 12b
                       SrcTrunk[11:0] = {Word2[7:3], Word1[30:29], Word0[23:19]} */

                    getValue = U32_GET_FIELD_MAC(wordsPtr[2],3,5);
                    U16_SET_FIELD_MAC(forwardPtr->source.trunkId,7,5,getValue);
                }

                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(forwardPtr->srcHwDev,dummyPort);

            }
            else
            {
                /* port */

                /* 5 bits */
                forwardPtr->source.portNum = U32_GET_FIELD_MAC(wordsPtr[0],19,5);
                /* 2 more bits */
                forwardPtr->source.portNum |= (U32_GET_FIELD_MAC(wordsPtr[1],29,2)) << 5;

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    /* SrcPort becomes 17b SRCePort
                       This field contains the Source ePort assigned to the
                       packet by the ingress device.
                       SRCePort[16:0] = {Word2[12:3], Word1[30:29], Word0[23:19]}*/

                    getValue = U32_GET_FIELD_MAC(wordsPtr[2],3,10);
                    U32_SET_FIELD_MAC(forwardPtr->source.portNum,7,10,getValue);

                }

                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(forwardPtr->srcHwDev,forwardPtr->source.portNum);
            }

            forwardPtr->egrFilterRegistered = (GT_BOOL)
                        U32_GET_FIELD_MAC(wordsPtr[1],28,1);

            forwardPtr->wasRouted = (GT_BOOL)
                        U32_GET_FIELD_MAC(wordsPtr[1],25,1);

            forwardPtr->srcId =
                        U32_GET_FIELD_MAC(wordsPtr[1],20,5);

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* SrcID is extended to 12 bits
                   SrcID[11:0] = {Word2[27:21], Word1[24:20]}*/

                getValue = U32_GET_FIELD_MAC(wordsPtr[2],21,7);
                U32_SET_FIELD_MAC(forwardPtr->srcId,5,7,getValue);
            }

            forwardPtr->qosProfileIndex =
                        U32_GET_FIELD_MAC(wordsPtr[1],13,7);

            if(useVidx)
            {
                vidx = (GT_U16)
                        U32_GET_FIELD_MAC(wordsPtr[1],0,12);

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    /* VIDX becomes 16b eVIDX Indicates (when eVIDX >= 4K)
                       the L2MLL group or (when VIDX < 4K) the L2 multicast
                       group to which the packet is transmitted.
                       eVIDX[15:0] = {Word3[23:20], Word1[11:0]} */

                     /* 4 more bits */
                    getValue = U32_GET_FIELD_MAC(wordsPtr[3],20,4);
                    U16_SET_FIELD_MAC(vidx,12,4,getValue);
                }

                /* check if VIDX means VLAN_ID */
                if(vidx == VID_INSTEAD_VIDX_CNS)
                {
                    forwardPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
                    forwardPtr->dstInterface.vlanId = dsaInfoPtr->commonParams.vid;
                }
                else
                {
                    forwardPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
                    forwardPtr->dstInterface.vidx = vidx;
                }
            }
            else
            {
                /* the destination is never trunk (the ingress device choose a
                   port in the trunk) */
                forwardPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;

                forwardPtr->dstInterface.devPort.hwDevNum = U32_GET_FIELD_MAC(wordsPtr[1],0,5);

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    forwardPtr->isTrgPhyPortValid = U32_GET_FIELD_MAC(wordsPtr[2],29,1);

                    /* TrgPort is extended to 8 bits
                    TrgPort[7:0] = {Word2[28], Word1[11:5]}
                    The value in <TrgPort> field is valid only when
                    <IsTrgPortValid> == TRUE */
                    /* NOTE: even though the field is 'valid' only on some cases ...
                       still the 'get' should return the value from the DSA */
                    forwardPtr->dstInterface.devPort.portNum = U32_GET_FIELD_MAC(wordsPtr[1],5,7);
                    getValue = U32_GET_FIELD_MAC(wordsPtr[2],28,1);
                    U32_SET_FIELD_MAC(forwardPtr->dstInterface.devPort.portNum
                                    ,7,1,getValue);

                    /* TrgDev is extended to 12b in sip5 devices, and then
                       reduced to 10b in sip5.20.
                       TrgDev[11:0] = {Word3[6:0], Word1[4:0]} (sip5)
                       SrcDev[9:0]  = {Word3[4:0], Word1[4:0]} (sip5.20)
                    */

                    getValue = isSip5_20 ?
                        U32_GET_FIELD_MAC(wordsPtr[3],0,5) :
                        U32_GET_FIELD_MAC(wordsPtr[3],0,7);

                    U32_SET_FIELD_MAC(forwardPtr->dstInterface.devPort.hwDevNum
                                    ,5,7,getValue);

                    /* TRGePort[16:0] = {Word3[23:7]}*/
                    forwardPtr->dstEport = U32_GET_FIELD_MAC(wordsPtr[3],7,17);
                }
                else
                {
                    /* the destination is never trunk (the ingress device choose a
                       port in the trunk) */
                    forwardPtr->dstInterface.devPort.portNum = U32_GET_FIELD_MAC(wordsPtr[1],5,6);
                }
                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(forwardPtr->dstInterface));
            }

            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 27, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 26, 1);
            }


            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* get data from word 2 */

                /* tag0TpidIndex = {Word2[0:2]}*/
                forwardPtr->tag0TpidIndex = U32_GET_FIELD_MAC(wordsPtr[2],0,3);

                /* get data from word 3 */
                tag0SrcIsTagged = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[0],29,1);
                tag1SrcIsTagged = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[3],26,1);

                if (tag0SrcIsTagged == GT_FALSE && tag1SrcIsTagged == GT_FALSE)
                {
                    forwardPtr->srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
                }
                else if (tag0SrcIsTagged == GT_TRUE && tag1SrcIsTagged == GT_FALSE)
                {
                    forwardPtr->srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
                }
                else if (tag0SrcIsTagged == GT_FALSE && tag1SrcIsTagged == GT_TRUE)
                {
                    forwardPtr->srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
                }
                else /* tag0SrcIsTagged == GT_TRUE && tag1SrcIsTagged == GT_TRUE */
                {
                    if (U32_GET_FIELD_MAC(wordsPtr[3],25,1))
                    {
                        forwardPtr->srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
                    }
                    else
                    {
                        forwardPtr->srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
                    }
                }

                if(useVidx)
                {
                    /* origSrcPhyIsTrunk = {Word3[19]} */
                    forwardPtr->origSrcPhyIsTrunk = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[3],19,1);

                    if(forwardPtr->origSrcPhyIsTrunk == GT_TRUE)
                    {
                        /* trunk */

                        /* 12 bits */
                        forwardPtr->origSrcPhy.trunkId = (GT_TRUNK_ID)
                                U32_GET_FIELD_MAC(wordsPtr[3],7,12);

                        PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(forwardPtr->srcHwDev,dummyPort);
                    }
                    else
                    {
                        /* port */

                        /* 12 bits */
                        forwardPtr->origSrcPhy.portNum = U32_GET_FIELD_MAC(wordsPtr[3],7,12);


                        PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(forwardPtr->srcHwDev,forwardPtr->origSrcPhy.portNum);
                    }

                    /* phySrcMcFilterEnable = {Word3[6]} */
                    forwardPtr->phySrcMcFilterEnable = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[3],6,1);
                }

                if (isSip5_20)
                {
                    /* Hash = word0[17], word2[13], word2[20:19], (useVidx ? word3[6:5] : word3[5:4]) */
                    getValue = U32_GET_FIELD_MAC(wordsPtr[0],17,1);
                    U32_SET_FIELD_MAC(forwardPtr->hash, 0, 1, getValue);

                    getValue = U32_GET_FIELD_MAC(wordsPtr[2],13,1);
                    U32_SET_FIELD_MAC(forwardPtr->hash, 1, 1, getValue);

                    getValue = U32_GET_FIELD_MAC(wordsPtr[2],19,2);
                    U32_SET_FIELD_MAC(forwardPtr->hash, 2, 2, getValue);

                    getValue = (useVidx) ?
                        U32_GET_FIELD_MAC(wordsPtr[3],4,2):
                        U32_GET_FIELD_MAC(wordsPtr[3],5,2);
                    U32_SET_FIELD_MAC(forwardPtr->hash, 4, 2, getValue);
                }
            }

            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            fromCpuPtr->srcHwDev = U32_GET_FIELD_MAC(wordsPtr[1],15,5);

            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_PORT_MAC(fromCpuPtr->srcHwDev,dummyPort);

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* 5b SrcDev is extended to 12b
                   SrcDev[11:0] = {Word2[20:14], Word1[19:15]} */

                /* 7 more bits */
                getValue = U32_GET_FIELD_MAC(wordsPtr[2],14,7);
                U32_SET_FIELD_MAC(fromCpuPtr->srcHwDev,5,7,getValue);
            }

            fromCpuPtr->tc = (GT_U8)
               (
                      U32_GET_FIELD_MAC(wordsPtr[0],17,1)        |
                    ((U32_GET_FIELD_MAC(wordsPtr[1],14,1)) << 1) |
                    ((U32_GET_FIELD_MAC(wordsPtr[1],27,1)) << 2)
                );


            /* check for Cheetah3 device */
            if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
                CPSS_PP_FAMILY_CHEETAH3_E)
            {
                /* get Fast Stack Fail over Recovery parameters */
                dsaInfoPtr->commonParams.dropOnSource =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 26, 1);

                dsaInfoPtr->commonParams.packetIsLooped =
                    (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[1], 25, 1);
            }
            /* Not suppoted for Ch3 and above devices.
               To get DP, call cpssDxChNetIfFromCpuDpGet */
            else
            {
                /* cheetah support only those 2 types */
                fromCpuPtr->dp = ((U32_GET_FIELD_MAC(wordsPtr[1],25,1)) ?
                                              CPSS_DP_RED_E :
                                              CPSS_DP_GREEN_E);


                if(U32_GET_FIELD_MAC(wordsPtr[1],26,1))
                {
                    /* unknown setting !! the DXCH not support it */

                    /* since there is no reason to DROP the packet , let
                       Application to handle it */
                    fromCpuPtr->dp = CPSS_DP_RED_E;
                }

            }

            fromCpuPtr->srcId = U32_GET_FIELD_MAC(wordsPtr[1],20,5);

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* 5b SrcID is extended to 12 bits
                   SrcID[11:0] = {Word2[27:21], Word1[24:20]} */

                /* 7 more bits */
                getValue = U32_GET_FIELD_MAC(wordsPtr[2],21,7);
                U32_SET_FIELD_MAC(fromCpuPtr->srcId,5,7,getValue);
            }

            fromCpuPtr->egrFilterEn = U32_GET_FIELD_MAC(wordsPtr[1],30,1);

            fromCpuPtr->cascadeControl = U32_GET_FIELD_MAC(wordsPtr[1],29,1);

            fromCpuPtr->egrFilterRegistered = U32_GET_FIELD_MAC(wordsPtr[1],28,1);

            if(U32_GET_FIELD_MAC(wordsPtr[0],18,1))/* use vidx */
            {
                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    /*Tag0 Src Is Tagged = {Word0[29]}*/
                    fromCpuPtr->extDestInfo.multiDest.srcIsTagged = (CPSS_DXCH_BRG_VLAN_PORT_TAG_CMD_ENT)U32_GET_FIELD_MAC(wordsPtr[0],29,1);
                }

                vidx = (GT_U16)
                    (
                         U32_GET_FIELD_MAC(wordsPtr[0],19,10)       |
                        (U32_GET_FIELD_MAC(wordsPtr[1],12,2 )<< 10)
                    );

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    /* 12b VIDX becomes 16b eVIDX
                       eVIDX indicates the L2MLL group (when eVIDX >= 4K) or
                       L2 multicast group (when eVIDX< 4K) to which the packet
                       is transmitted.
                       eVIDX[15:0] = {Word3[23:20], Word1[13:12], Word0[28:19]} */

                    /* 4 more bits */
                    getValue = U32_GET_FIELD_MAC(wordsPtr[3],20,4);
                    U16_SET_FIELD_MAC(vidx,12,4,getValue);
                }

                /* check if VIDX means VLAN_ID */
                if(vidx == VID_INSTEAD_VIDX_CNS)
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VID_E;
                    fromCpuPtr->dstInterface.vlanId = dsaInfoPtr->commonParams.vid;
                }
                else
                {
                    fromCpuPtr->dstInterface.type = CPSS_INTERFACE_VIDX_E;
                    fromCpuPtr->dstInterface.vidx = vidx;
                }

                excludeIsTrunk = U32_GET_FIELD_MAC(wordsPtr[1],11,1);

                if(excludeIsTrunk)
                {
                    /* bit 10 - MirrorToAllCPUs */
                    fromCpuPtr->extDestInfo.multiDest.mirrorToAllCPUs = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[1],10,1);

                    excludeTrunkId = (GT_TRUNK_ID)U32_GET_FIELD_MAC(wordsPtr[1],0,7);

                    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                    {
                         /* 7b TrunkID is extended to 12b
                            ExcludedTrunk[11:0] = {Word2[7:3], Word1[6:0]} */

                        /* 5 more bits */
                        getValue = U32_GET_FIELD_MAC(wordsPtr[2],3,5);
                        U16_SET_FIELD_MAC(excludeTrunkId,7,5,getValue);
                    }

                    if(excludeTrunkId == 0)
                    {
                        /* no interface was excluded */
                        fromCpuPtr->extDestInfo.multiDest.excludeInterface = GT_FALSE;
                    }
                    else
                    {
                        fromCpuPtr->extDestInfo.multiDest.excludeInterface = GT_TRUE;
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.type = CPSS_INTERFACE_TRUNK_E;
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId = excludeTrunkId;
                    }
                }
                else
                {
                    fromCpuPtr->extDestInfo.multiDest.excludeInterface = GT_TRUE;
                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.type = CPSS_INTERFACE_PORT_E;

                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum =
                        U32_GET_FIELD_MAC(wordsPtr[1],5,6);

                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum =
                        U32_GET_FIELD_MAC(wordsPtr[1],0,5);

                    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                    {
                        /* ExcludedIsPhyPort */
                        if(U32_GET_FIELD_MAC(wordsPtr[3],19,1))
                        {
                            fromCpuPtr->extDestInfo.multiDest.excludedIsPhyPort = GT_TRUE;

                            /* 6b ExludedPort becomes 8b Physical port number*/
                            /* {Word2[1:0], Word1[10:5]} */

                            /* 2 more bits */
                            getValue = U32_GET_FIELD_MAC(wordsPtr[2],0,2);
                            U32_SET_FIELD_MAC(fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum,6,2,getValue);

                            PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(fromCpuPtr->extDestInfo.multiDest.excludedInterface));
                        }
                        else
                        {
                            fromCpuPtr->extDestInfo.multiDest.excludedIsPhyPort = GT_FALSE;

                            /* 6b Exluded Port becomes 17b Excluded ePort */
                            /* ExcludedePort[16:0] = {Word2[13:3], Word1[10:5]} */

                            /* 14 more bits */
                            getValue = U32_GET_FIELD_MAC(wordsPtr[2],3,11);
                            U32_SET_FIELD_MAC(fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum,6,11,getValue);

                        }

                        /* 5b ExcludedDev is extended to 12b
                           ExcludedDev[11:0] = {Word3[6:0], Word1[4:0]}*/

                        /* 7  more bits */
                        getValue = U32_GET_FIELD_MAC(wordsPtr[3],0,7);
                        U32_SET_FIELD_MAC(fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum,5,7,getValue);
                    }
                    else
                    {
                        PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(fromCpuPtr->extDestInfo.multiDest.excludedInterface));
                    }
                }
            }
            else   /* not using vidx */
            {
                /* destination trunk is not supported */
                fromCpuPtr->dstInterface.type = CPSS_INTERFACE_PORT_E;

                fromCpuPtr->dstInterface.devPort.hwDevNum =
                    U32_GET_FIELD_MAC(wordsPtr[0],24,5);

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                     /* target port is extended to 8b
                        TrgPort = {Word2[28], Word1[11:10], Word0[23:19]} */

                    if(U32_GET_FIELD_MAC(wordsPtr[2],29,1) == GT_TRUE)
                    {
                        fromCpuPtr->isTrgPhyPortValid = GT_TRUE;
                    }
                    else
                    {
                        fromCpuPtr->isTrgPhyPortValid = GT_FALSE;
                    }

                    /* NOTE: even though the field is 'valid' only on some cases ...
                       still the 'get' should return the value from the DSA */
                    fromCpuPtr->dstInterface.devPort.portNum =
                    (
                        /* 5 bits */
                        U32_GET_FIELD_MAC(wordsPtr[0],19,5) |
                        /* 1 more bit */
                        (U32_GET_FIELD_MAC(wordsPtr[1],10,1)<< 5)
                    );

                     /* 2 more bits */
                    getValue = U32_GET_FIELD_MAC(wordsPtr[1],11,1);
                    U32_SET_FIELD_MAC(fromCpuPtr->dstInterface.devPort.portNum,6,1,getValue);

                    getValue = U32_GET_FIELD_MAC(wordsPtr[2],28,1);
                    U32_SET_FIELD_MAC(fromCpuPtr->dstInterface.devPort.portNum,7,1,getValue);

                    /* 5b TrgDev is extended to 12b
                       TrgDev[11:0] = {Word3[6:0], Word0[28:24]} */

                    /* 7  more bits */
                    getValue = U32_GET_FIELD_MAC(wordsPtr[3],0,7);
                    U32_SET_FIELD_MAC(fromCpuPtr->dstInterface.devPort.hwDevNum,5,7,getValue);
                }
                else
                {
                    fromCpuPtr->dstInterface.devPort.portNum =
                    (
                        /* 5 bits */
                        U32_GET_FIELD_MAC(wordsPtr[0],19,5) |
                        /* 1 more bit */
                        (U32_GET_FIELD_MAC(wordsPtr[1],10,1)<< 5)
                    );
                }

                PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_INTERFACE_MAC(&(fromCpuPtr->dstInterface));

                fromCpuPtr->extDestInfo.devPort.dstIsTagged = (GT_BOOL)
                                U32_GET_FIELD_MAC(wordsPtr[0],29,1);

                fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU = (GT_BOOL)
                        U32_GET_FIELD_MAC(wordsPtr[1],13,1);

                /* TRGePort[16:0] = {Word3[23:7]}*/
                fromCpuPtr->dstEport = U32_GET_FIELD_MAC(wordsPtr[3],7,17);
            }

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* get data from word 2 */

                /* tag0TpidIndex = {Word2[0:2]}*/
                fromCpuPtr->tag0TpidIndex = U32_GET_FIELD_MAC(wordsPtr[2],0,3);

                tag0SrcIsTagged = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[0],29,1);
                tag1SrcIsTagged = (GT_BOOL)U32_GET_FIELD_MAC(wordsPtr[3],26,1);

                if (tag0SrcIsTagged == GT_FALSE && tag1SrcIsTagged == GT_FALSE)
                {
                    fromCpuPtr->extDestInfo.multiDest.srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
                }
                else if (tag0SrcIsTagged == GT_TRUE && tag1SrcIsTagged == GT_FALSE)
                {
                    fromCpuPtr->extDestInfo.multiDest.srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
                }
                else if (tag0SrcIsTagged == GT_FALSE && tag1SrcIsTagged == GT_TRUE)
                {
                    fromCpuPtr->extDestInfo.multiDest.srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E;
                }
                else /* tag0SrcIsTagged == GT_TRUE && tag1SrcIsTagged == GT_TRUE */
                {
                    if (U32_GET_FIELD_MAC(wordsPtr[3],25,1))
                    {
                        fromCpuPtr->extDestInfo.multiDest.srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E;
                    }
                    else
                    {
                        fromCpuPtr->extDestInfo.multiDest.srcIsTagged = CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E;
                    }
                }

            }

            break;
        default:/* default for compilation warnings */
            break;
    }

    /* check if need to WA the "wrong trunk id source port
       information of packet to CPU" */
    rc = prvCpssDxChTrunkTrafficToCpuWaUpToMemberConvert(devNum,dsaInfoPtr);
    if(rc != GT_OK && rc != GT_NO_CHANGE)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfDsaTagParse
*
* DESCRIPTION:
*       parse the DSA tag parameters from the DSA tag on the packet.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_DSA_PARSING_ERROR     - DSA tag parsing error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*           support:
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*           ===== eDSA tag         ====
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfDsaTagParse
(
    IN  GT_U8                          devNum,
    IN  GT_U8                          *dsaBytesPtr,
    OUT CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfDsaTagParse);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dsaBytesPtr, dsaInfoPtr));

    rc = internal_cpssDxChNetIfDsaTagParse(devNum, dsaBytesPtr, dsaInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dsaBytesPtr, dsaInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* prvCpssDxChNetIfDsaTagToCpuBuild
*
* DESCRIPTION:
*       Build "TO_CPU" DSA tag from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaInfoPtr - (pointer to) the DSA parameters.
*
* OUTPUTS:
*       wordsPtr  - (pointer to) buffer to hold the DSA tag.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal DSA parameter.
*
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfDsaTagToCpuBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U32                         *wordsPtr
)
{
    GT_STATUS rc;
    GT_HW_DEV_NUM  hwDev = 0;
    GT_PORT_NUM    hwPort = 0;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT dsaCpuCode;/* HW dsa cpu opcode */
    CPSS_DXCH_NET_DSA_TYPE_ENT dsaTagType = dsaInfoPtr->commonParams.dsaTagType;
    CPSS_DXCH_NET_DSA_TO_CPU_STC *toCpuPtr = &(dsaInfoPtr->dsaInfo.toCpu);
    GT_U32  isSip5_20 = PRV_CPSS_SIP_5_20_CHECK_MAC(devNum);
    GT_U32  srcPortTrgPortNumBits;/* number of bits of <SrcPort/TrgPort> field */

    /* bits 30:31 - TagCommand 0 = TO_CPU */
    /* U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,0);*/
    if(toCpuPtr->srcIsTrunk == GT_FALSE ||
       dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(toCpuPtr->hwDevNum,toCpuPtr->interface.portNum);
        hwDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
            toCpuPtr->hwDevNum,toCpuPtr->interface.portNum);
        hwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
            toCpuPtr->hwDevNum,toCpuPtr->interface.portNum);
    }
    else
    {
        hwPort = (CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT == dsaTagType) ?
            toCpuPtr->interface.portNum : 0;
        hwDev = toCpuPtr->hwDevNum;
    }

    /* bit 29 -- SrcTagged/TrgTagged */
    if(toCpuPtr->isTagged == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
    }

    /* bits 24:28 -- SrcDev/TrgDev */
    if(((hwDev >= BIT_5) && (dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
       ((hwDev >= BIT_12) && (dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,hwDev);

    if(dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        srcPortTrgPortNumBits = 5;
    }
    else
    if(dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)
    {
        srcPortTrgPortNumBits = 6;
    }
    else /*CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT*/
    {
        if(isSip5_20)
        {
            srcPortTrgPortNumBits = 9;
        }
        else
        {
            srcPortTrgPortNumBits = 8;
        }
    }

    if(dsaTagType != CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        if(toCpuPtr->srcIsTrunk == GT_FALSE)
        {
            if (hwPort > BIT_MASK_MAC(srcPortTrgPortNumBits))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* bits 19:23 -- SrcPort[4:0]/TrgPort[4:0] */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,hwPort);
        }
        else
        {
            if(((toCpuPtr->interface.srcTrunkId >= BIT_7) &&
                (dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)) ||
               ((toCpuPtr->interface.srcTrunkId >= BIT_12) &&
                (dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            if(dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /* bits 19:23 -- SrcTrunk[4:0] */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,toCpuPtr->interface.srcTrunkId);
            }

            if(dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                if (hwPort > BIT_MASK_MAC(srcPortTrgPortNumBits))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }

                /* bits 19:23 -- Src/Trg Port[4:0]
                   In extended DSA 4 words tag this field always contains the source/target physical port
                   number, regardless of the value Orig Is Trunk */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,hwPort);
            }
        }


        /* bits 16:18 -- CPU_Code[3:1] */
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,3,7);
        /* bit 12 -- CPU_Code[0] --> will be set by
           CPSS_DXCH_NET_DSA_TYPE_EXTENDED_E */
    }
    else
    {
        if(hwPort >= BIT_5)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* bits 19:23 -- SrcPort[4:0] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,hwPort);

        rc = prvCpssDxChNetIfCpuToDsaCode(toCpuPtr->cpuCode,&dsaCpuCode);
        if(rc != GT_OK)
        {
            return rc;
        }

        if(dsaCpuCode > 14)
        {
            /* support values 0..15
              but value 15 is special for "internal" use !!!
            */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* bits 16:18 -- CPU_Code[3:1] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],16,3,((GT_U32)dsaCpuCode) >> 1);
        /* bit 12 -- CPU_Code[0] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],12,1,dsaCpuCode);/* use masked marco !! */
    }

    /* bits 13:15 -- UP */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],13,3,dsaInfoPtr->commonParams.vpt);

    switch(dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 1,
               word[2] bit 31 = 1, word[3] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],31,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[2],31,1,1);
            break;
        default:
            break;
    }

    /* bits 0:11 -- VID */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],0,12,dsaInfoPtr->commonParams.vid);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        return GT_OK;
    }

    /******************************************/
    /* build the second, third and forth word */
    /******************************************/

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* VID becomes 16b eVLAN. eVLAN[15:0] = {Word3[30:27], Word0[11:0]} */
        /* 4 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],27,4,dsaInfoPtr->commonParams.vid>>12);
    }

    /* bit 30 - CFI */
    if(dsaInfoPtr->commonParams.cfiBit == 1)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],30,1,1);
    }

    /* bit 27 -- OrigIsTrunk */
    if(toCpuPtr->srcIsTrunk == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,1);
    }

    /* bit 26 -- Truncated */
    if(toCpuPtr->wasTruncated == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,1);
    }

    /* bits 12:25 -- PktOrigBC */
    if(toCpuPtr->originByteCount >= BIT_14)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,14,toCpuPtr->originByteCount);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* SrcDev/TrgDev is extended to 12b
           SrcDev/TrgDev[11:0] = {Word3[6:0], Word0[28:24]} */

        /* 7 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],0,7,hwDev>>5);

    }

    if(toCpuPtr->srcIsTrunk == GT_FALSE)
    {
        /* bit 11 -- reserved with value 0 */

        /* bits 10 -- SrcPort[5]/TrgPort[5] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,1,hwPort>>5);
    }
    else
    {
        if(dsaInfoPtr->commonParams.dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            /* bits 10:11 -- SrcTrunk[6:5] */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,2,toCpuPtr->interface.srcTrunkId>>5);
        }

        if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            /* SrcTrunk is extended to 12b
            SrcTrunk[11:0] = {Word2[14:3]} */

            /* 12 bits */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],3,14,toCpuPtr->interface.srcTrunkId);
        }
    }

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* Extended DSA TO CPU tag always contains the source/target physical port
           number, regardless of the value <OrigIsTrunk>  */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,2,hwPort>>5);
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],20,1,hwPort>>7);

        if(srcPortTrgPortNumBits >= 9)
        {
            /* the 9'th bit */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],21,1,hwPort>>8);
        }

    }

    /* bit 9 -- reserved with value 0 or                                     */
    /*          bit 0 of IPFIX timestamp (only for xCat A1 and above devices */
    if( PRV_CPSS_DXCH_XCAT_FAMILY_CHECK_MAC(devNum))
    {
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],9,1,toCpuPtr->timestamp);
    }

    /* bit 8 -- SrcTrg */
    if(toCpuPtr->isEgressPipe == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],8,1,1);
    }

    rc = prvCpssDxChNetIfCpuToDsaCode(toCpuPtr->cpuCode,&dsaCpuCode);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* bits 0:7-- LongCPUCode */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,8,dsaCpuCode);

    /* check for Cheetah3 device */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
        CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],29,1,1);
        }

        if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
        }
    }

    /* build third and fourth word */
    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* set packetIsTT */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],25,1,toCpuPtr->packetIsTT);

        if (toCpuPtr->packetIsTT)
        {
            if (toCpuPtr->flowIdTtOffset.ttOffset >= BIT_20)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* set TT offset */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,20,toCpuPtr->flowIdTtOffset.ttOffset);
        }
        else
        {
            if (toCpuPtr->flowIdTtOffset.flowId >= BIT_20)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* set flow ID */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,20,toCpuPtr->flowIdTtOffset.flowId);
        }

        /* set ePort */

        /* When toCpuPtr->isEgressPipe = GT_FALSE:
           this field indicates the source ePort number assigned by the ingress device.
           When toCpuPtr->isEgressPipe = GT_TRUE:
           This field indicates the target ePort number assigned by the ingress device. */

        if (toCpuPtr->interface.ePort >= BIT_17)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],3,17,toCpuPtr->interface.ePort);

        if (toCpuPtr->tag0TpidIndex >= BIT_3)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* set tag0TpidIndex */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],0,3,toCpuPtr->tag0TpidIndex);

    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfDsaTagFromCpuBuild
*
* DESCRIPTION:
*       Build "FROM_CPU" DSA tag from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaInfoPtr - (pointer to) the DSA parameters.
*
* OUTPUTS:
*       wordsPtr  - (pointer to) buffer to hold the DSA tag.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal DSA parameter.
*
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfDsaTagFromCpuBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U32                         *wordsPtr
)
{
    GT_HW_DEV_NUM trgHwDev;
    GT_PORT_NUM   trgPort = 0;
    GT_HW_DEV_NUM excludeHwDev;
    GT_PORT_NUM   excludeHwPort;
    CPSS_DXCH_NET_DSA_FROM_CPU_STC  *fromCpuPtr = &(dsaInfoPtr->dsaInfo.fromCpu);
    GT_BOOL useVidx = GT_FALSE;/* use vidx (and vlan) or use single interface (port/trunk)*/
    CPSS_DXCH_NET_DSA_TYPE_ENT dsaTagType = dsaInfoPtr->commonParams.dsaTagType;

    /* bits 30:31 - TagCommand 1 = FROM_CPU */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,1);
    /* bit 29 -- SrcTagged */
    if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_PORT_E &&
       fromCpuPtr->extDestInfo.devPort.dstIsTagged == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
    }

    if(dsaTagType != CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
        {
            useVidx = GT_TRUE;

            if(((dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT) &&
                (fromCpuPtr->dstInterface.vidx >= BIT_12)) ||
               (fromCpuPtr->dstInterface.vidx == VID_INSTEAD_VIDX_CNS))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,10,fromCpuPtr->dstInterface.vidx);
        }
        else if (fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
        {
            useVidx = GT_TRUE;
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,10,VID_INSTEAD_VIDX_CNS);
        }

        /* bit 17 -- TC[0] */
        if(fromCpuPtr->tc >= BIT_3)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],17,1,fromCpuPtr->tc);
    }
    else
    {
        if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
        {
            useVidx = GT_TRUE;
            if(fromCpuPtr->dstInterface.vidx >= (BIT_9-1))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],20,9,fromCpuPtr->dstInterface.vidx);
        }
        else if (fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
        {
            useVidx = GT_TRUE;
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],20,9,VID_INSTEAD_VIDX_9_BITS_CNS);
        }

        /* When the tag is not extended, this field is set to 1. */
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],17,1,1);
    }

    if(useVidx == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],18,1,1);
    }
    else if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_TRUNK_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG); /* trunk is not supported */
    }
    else if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_PORT_E)
    {
        PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
            fromCpuPtr->dstInterface.devPort.hwDevNum,
            fromCpuPtr->dstInterface.devPort.portNum);
        trgHwDev =  PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
            fromCpuPtr->dstInterface.devPort.hwDevNum,
            fromCpuPtr->dstInterface.devPort.portNum);
        trgPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
            fromCpuPtr->dstInterface.devPort.hwDevNum,
            fromCpuPtr->dstInterface.devPort.portNum);

        if(((trgHwDev >= BIT_5) && (dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
           ((trgHwDev >= BIT_12) && (dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if((trgPort >= BIT_5 && dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT) ||
            (trgPort >= BIT_6 && dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT) ||
            (trgPort >= BIT_8 && dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* TrgDev[4:0] = {Word0[28:24]} */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,trgHwDev);

        /* trgHwDev and trgPort */
        if(dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            /* TrgDev[11:5] = {Word3[6:0]} */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],0,7,(trgHwDev >> 5));

            /* target port is extended to 8b
            TrgPort = {Word2[28], Word1[11:10], Word0[23:19]}
            NOTE: The value in this field is valid only when <IsTrgPortValid> == TRUE */
            if(fromCpuPtr->isTrgPhyPortValid == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],29,1,1);/*isTrgPhyPortValid*/

                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,trgPort);
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,2,trgPort >> 5);
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],28,1,trgPort >> 7);
            }
        }
        else
        {
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,trgPort);
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bit 16 - CFI */
    if(dsaInfoPtr->commonParams.cfiBit == 1)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,1,1);
    }

    /* bits 13:15 -- UP */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],13,3,dsaInfoPtr->commonParams.vpt);

    switch(dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 1,
               word[2] bit 31 = 1, word[3] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],31,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[2],31,1,1);
            break;
        default:
            break;
    }

    /* bits 0:11 -- VID */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],0,12,dsaInfoPtr->commonParams.vid);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        return GT_OK;
    }

    /* bit 30 -- EgressFilterEn */
    if(fromCpuPtr->egrFilterEn == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],30,1,1);
    }

    /* bit 29 -- cascadeControl */
    if(fromCpuPtr->cascadeControl == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],29,1,1);
    }

    /* bit 28 -- egrFilterRegistered */
    if(fromCpuPtr->egrFilterRegistered == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
    }

    /* bit 27 -- TC[2] */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,fromCpuPtr->tc>>2);

    /* check for Cheetah3 device */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
        CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,1);
        }

        if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,1);
        }
    }
    /* Not suppoted for Ch3 and above devices.
       To set DP, call cpssDxChNetIfFromCpuDpSet */
    else
    {
       /* cheetah support only those 2 types */
        switch (fromCpuPtr->dp)
        {
        case CPSS_DP_GREEN_E:
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,0);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,0);
            break;
        case CPSS_DP_RED_E:
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,0);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,1);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if(((fromCpuPtr->srcId >= BIT_5) &&
        (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)) ||
       ((fromCpuPtr->srcId >= BIT_12) &&
        (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(((fromCpuPtr->srcHwDev >= BIT_5) &&
        (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)) ||
       ((fromCpuPtr->srcHwDev >= BIT_12) &&
        (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bits 20-24 -- Src-ID */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],20,5,fromCpuPtr->srcId);

    /* bits 15-19 -- SrcDev */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],15,5,fromCpuPtr->srcHwDev);

    /* bit 14 -- TC[1] */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],14,1,fromCpuPtr->tc>>1);

    if(useVidx == GT_TRUE)
    {
        /* bits 12:13 - VIDX[11:10] */
        if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
        {
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],12,2,fromCpuPtr->dstInterface.vidx>>10);

            if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
            {
                /*  12b VIDX becomes 16b eVIDX
                eVIDX indicates the L2MLL group (when eVIDX >= 4K) or L2
                multicast group (when eVIDX< 4K) to which the packet is transmitted.
                eVIDX[15:0] = {Word3[23:20], Word1[13:12], Word0[28:19]} */

                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],20,4,fromCpuPtr->dstInterface.vidx>>12);
            }
        }
        else /* vid */
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,2,VID_INSTEAD_VIDX_CNS>>10);
        }

        if(fromCpuPtr->extDestInfo.multiDest.excludeInterface == GT_TRUE)
        {
            if(fromCpuPtr->extDestInfo.multiDest.excludedInterface.type == CPSS_INTERFACE_TRUNK_E)
            {
                /* bit 11 - ExcludeIsTrunk */
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],11,1,1);

                /* bit 10 - MirrorToAllCPUs */
                if(fromCpuPtr->extDestInfo.multiDest.mirrorToAllCPUs == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],10,1,1);
                }

                /* bits 7:9 -- reserved --> set to 0 */

                if(((fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId >= BIT_7) &&
                    (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)) ||
                   ((fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId >= BIT_12) &&
                    (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                /* bits 0:6 -  ExcludedTrunk*/
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,7,
                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId);

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    /* 7b TrunkID is extended to 12b
                       ExcludedTrunk[11:0] = {Word2[7:3], Word1[6:0]} */

                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],3,5,
                    fromCpuPtr->extDestInfo.multiDest.excludedInterface.trunkId >> 7);

                }
            }
            else if(fromCpuPtr->extDestInfo.multiDest.excludedInterface.type != CPSS_INTERFACE_PORT_E)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG); /* must be port/trunk */
            }
            else /* must be port */
            {

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    if(fromCpuPtr->extDestInfo.multiDest.excludedIsPhyPort == GT_FALSE)
                    {
                        /* no need to check for Dual-Dev */
                        excludeHwDev = fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum;
                        excludeHwPort = fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum;
                        /* The <ExcludedPort /ExcludedePort> field
                           contains a 17b ePort number to exclude */

                        /* word3[22] - ExcludedIsPhyPort */
                        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[3],19,1,0);

                        if(excludeHwPort >= BIT_17)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }

                        /* bits 5:10 -  ExcludedPort*/
                        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],5,6,excludeHwPort);

                         /* 6b ExludedPort becomes 17b ExcludedePort
                           ExcludedePort[16:0] = {Word2[13:3], Word1[10:5]} */
                        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],3,11,excludeHwPort >> 6);
                    }
                    else
                    {
                        /* The <ExcludedPort /ExcludedePort> field contains
                           an 8b Physical port number to exclude */

                        PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum,
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum);
                        excludeHwDev  = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum,
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum);
                        excludeHwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum,
                            fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum);

                         /* word3[22] - ExcludedIsPhyPort */
                        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[3],19,1,1);

                        if(excludeHwPort >= BIT_8)
                        {
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                        }

                         /* 6b ExludedPort becomes 8b Physical port number
                            {Word2[1:0], Word1[10:5]} */

                        /* bits 5:10 -  ExcludedPort*/
                        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],5,6,excludeHwPort);
                        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],0,2,excludeHwPort >> 6);
                    }
                }
                else
                {
                    PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum,
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum);
                    excludeHwDev  = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum,
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum);
                    excludeHwPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.hwDevNum,
                        fromCpuPtr->extDestInfo.multiDest.excludedInterface.devPort.portNum);

                    if(excludeHwPort >= BIT_6)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }

                    /* bits 5:10 -  ExcludedPort*/
                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],5,6,excludeHwPort);
                }

                if(((excludeHwDev >= BIT_5) &&
                    (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)) ||
                   ((excludeHwDev >= BIT_12) &&
                    (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                 /* bits 0:4 -  ExcludedDev*/
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,5,excludeHwDev);

                if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
                {
                    /* 5b ExcludedDev is extended to 12b
                       ExcludedDev[11:0] = {Word3[6:0], Word1[4:0]} */
                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],0,7,excludeHwDev >> 5);
                }
            }
        }
        else
        {
            /* bit 11 - ExcludeIsTrunk */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],11,1,1);

            /* bit 10 - MirrorToAllCPUs */
            if(fromCpuPtr->extDestInfo.multiDest.mirrorToAllCPUs == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],10,1,1);
            }

            /* bits 7:9 -- reserved --> set to 0 */

            /* bits 0:6 -  ExcludedTrunk --> set to 0 */
        }
    }
    else if(fromCpuPtr->dstInterface.type == CPSS_INTERFACE_TRUNK_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG); /* trunk not supported */
    }
    else /* not use vidx -- must not be trunk */
    {
        /* bit 13 - MailBoxToNeighborCPU */
        if(fromCpuPtr->extDestInfo.devPort.mailBoxToNeighborCPU == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],13,1,1);
        }

        /* bit 12 -- reserved with value 0 */

        /* bits 0:9 -- reserved with value 0 */
        if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            if (fromCpuPtr->dstEport >= BIT_17)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            /* TRGePort[16:0] = {Word3[23:7]}*/
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,17,fromCpuPtr->dstEport);
        }
        else
        {
            /* bit 10 - TrgPort[5] */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,1,trgPort>>5);
        }
    }

    /* build third and fourth word */
    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {

        /* 5b SrcID is extended to 12 bits
          SrcID[11:0] = {Word2[27:21], Word1[24:20]} */

        /* 7 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],21,7,fromCpuPtr->srcId>>5);

        /* 5b SrcDev is extended to 12b
           SrcDev[11:0] = {Word2[20:14], Word1[19:15]}*/

        /* 7 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],14,7,fromCpuPtr->srcHwDev>>5);

        if (fromCpuPtr->tag0TpidIndex >= BIT_3)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* tag0TpidIndex = {Word2[0:2]}*/
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[2],0,3,fromCpuPtr->tag0TpidIndex);

        if(useVidx == GT_TRUE)
        {
            switch (fromCpuPtr->extDestInfo.multiDest.srcIsTagged)
            {
            case CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E:
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,0); /* Tag0 SrcTagged */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,0); /* Tag1 SrcTagged */
                break;

            case CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E:
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,1); /* Tag0 SrcTagged */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,0); /* Tag1 SrcTagged */
                break;

            case CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E:
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,0); /* Tag0 SrcTagged */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,1); /* Tag1 SrcTagged */
                break;

            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E:
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,1); /* Tag0 SrcTagged */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,1); /* Tag1 SrcTagged */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],25,1,1); /* SrcTag0 Is OuterTag */
                break;

            case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E:
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,1); /* Tag0 SrcTagged */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,1); /* Tag1 SrcTagged */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],25,1,0); /* SrcTag0 Is OuterTag */
                break;

            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
        }
        /* VID[15:12] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],27,4,(dsaInfoPtr->commonParams.vid>>12));
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfDsaTagToAnalyzerBuild
*
* DESCRIPTION:
*       Build "TO Analyzer" DSA tag from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaInfoPtr - (pointer to) the DSA parameters.
*
* OUTPUTS:
*       wordsPtr  - (pointer to) buffer to hold the DSA tag.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal DSA parameter.
*
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfDsaTagToAnalyzerBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U32                         *wordsPtr
)
{
    GT_HW_DEV_NUM  hwSrcDev;
    GT_PORT_NUM    hwSrcPort;
    GT_U16         vidx;
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC *toAnalyzerPtr = &(dsaInfoPtr->dsaInfo.toAnalyzer);

    /* bits 30:31 - TagCommand 2 = TO_ANALYZER */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,2);

    /* bit 29 -- SrcTagged/TrgTagged */
    if(toAnalyzerPtr->isTagged == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
    }

    PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(toAnalyzerPtr->devPort.hwDevNum,toAnalyzerPtr->devPort.portNum);
    hwSrcDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(toAnalyzerPtr->devPort.hwDevNum,toAnalyzerPtr->devPort.portNum);
    hwSrcPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(toAnalyzerPtr->devPort.hwDevNum,toAnalyzerPtr->devPort.portNum);

    if(((hwSrcDev >= BIT_5) &&
        (dsaInfoPtr->commonParams.dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
       ((hwSrcDev >= BIT_12) &&
        (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if((hwSrcPort >= BIT_5 &&
        dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT) ||
        (hwSrcPort >= BIT_6 &&
        dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT) ||
       (hwSrcPort >= BIT_8  &&
        dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bits 24:28 -- SrcPort/TrgPort */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,hwSrcDev);

    /* bits 19:23 -- SrcDev/TrgDev */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,hwSrcPort);

    /* bit 18 -- rxSniff */
    if(toAnalyzerPtr->rxSniffer == 1)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],18,1,1);
    }

    /* bit 17 = 0 -- reserved */

    /* bit 16 - CFI */
    if(dsaInfoPtr->commonParams.cfiBit == 1)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,1,1);
    }

    /* bits 13:15 -- UP */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],13,3,dsaInfoPtr->commonParams.vpt);

    switch(dsaInfoPtr->commonParams.dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 1,
               word[2] bit 31 = 1, word[3] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],31,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[2],31,1,1);
            break;
        default:
            break;
    }

    /* bits 0:11 -- VID */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],0,12,dsaInfoPtr->commonParams.vid);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        return GT_OK;
    }

    /******************************************/
    /* build the second, third and forth word */
    /******************************************/

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* VID becomes 16b eVLAN. eVLAN[15:0] = {Word3[30:27], Word0[11:0]} */
        /* 4 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],27,4,dsaInfoPtr->commonParams.vid>>12);
    }
    /* bit 31 = 0 -- Extend */

    /* bits 11:27 -- reserved with value 0 */

    /* bits 10 -- SrcPort[5]/TrgPort[5] */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],10,1,hwSrcPort>>5);

    /* bits 0:9 -- reserved with value 0 */
    /* check for Cheetah3 device */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
        CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],29,1,1);
        }

        if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
        }
    }

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
         /* Device number is extended to 12b
           SrcDev/TrgDev/AnalyzerDev[11:0] = {Word3[6:0], Word0[28:24]} */

        /* 7 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],0,7,hwSrcDev>>5);

        /* Physical ports are extended to 8b
           SrcPort/TrgPort/AnalyzerPort[7:0] =
                        {Word2[20], Word1[11:10], Word0[23:19]} */

         /* 2 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],11,1,hwSrcPort>>6);
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],20,1,hwSrcPort>>7);

        /* set new fields */
        switch(toAnalyzerPtr->analyzerTrgType)
        {
            case CPSS_INTERFACE_VID_E:
            case CPSS_INTERFACE_VIDX_E:
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,1); /* use Vix */

                if(toAnalyzerPtr->analyzerTrgType == CPSS_INTERFACE_VIDX_E)
                {
                    vidx = toAnalyzerPtr->extDestInfo.multiDest.analyzerEvidx;
                    if(vidx == VID_INSTEAD_VIDX_CNS)
                    {
                        /*the caller must use CPSS_INTERFACE_VID_E instead of (CPSS_INTERFACE_VIDX_E with VID_INSTEAD_VIDX_CNS) */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                }
                else
                {
                    vidx = VID_INSTEAD_VIDX_CNS;
                }
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,16,vidx);
                break;
            case CPSS_INTERFACE_PORT_E:
                U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,0);/* do not use Vix*/

                 /* The target Analyzer interface indication that the target
                    analyzer port field <Reserved Analyzer_TrgPort> has a valid value */

                if(toAnalyzerPtr->extDestInfo.devPort.analyzerIsTrgPortValid == GT_TRUE)
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,1);

                    if (toAnalyzerPtr->extDestInfo.devPort.analyzerTrgPort >= BIT_8)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                    /* The target Analyzer interface physical port number */
                    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,8,toAnalyzerPtr->extDestInfo.devPort.analyzerTrgPort);

                }
                else
                {
                    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,0);
                }

                if (toAnalyzerPtr->extDestInfo.devPort.analyzerTrgEport >= BIT_17)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                /* The target analyzer interface ePort */
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,17,toAnalyzerPtr->extDestInfo.devPort.analyzerTrgEport);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        if (toAnalyzerPtr->extDestInfo.devPort.analyzerHwTrgDev >= BIT_12)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* The target Analyzer interface device number */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],14,12,toAnalyzerPtr->extDestInfo.devPort.analyzerHwTrgDev);

       /* When in in source based forwarding mode: TBD.
           When in hop-by-hop mode:
           When <rxSniffer>= GT_TRUE: SRCePort:
                Together with SrcDev, SRCePort indicates the packet's
                original Ingress ePort assignment.
           When <rxSniffer> = GT_FALSE: TRGePort:
                Together with TrgDev, TRGePort indicates the packet's
                assigned egress ePort. */

        if (toAnalyzerPtr->devPort.ePort >= BIT_17)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],3,17,toAnalyzerPtr->devPort.ePort);

        if (toAnalyzerPtr->tag0TpidIndex >= BIT_3)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        /* set tag0TpidIndex */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],0,3,toAnalyzerPtr->tag0TpidIndex);
    }
    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChNetIfDsaTagForwardBuild
*
* DESCRIPTION:
*       Build "FORWARD" DSA tag from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaInfoPtr - (pointer to) the DSA parameters.
*
* OUTPUTS:
*       wordsPtr  - (pointer to) buffer to hold the DSA tag.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal DSA parameter.
*
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS prvCpssDxChNetIfDsaTagForwardBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U32                         *wordsPtr
)
{
    GT_HW_DEV_NUM  hwSrcDev;
    GT_PORT_NUM    hwSrcPort = 0;
    GT_HW_DEV_NUM  hwDstDev;
    GT_PORT_NUM    hwDstPort;
    CPSS_DXCH_NET_DSA_FORWARD_STC *forwardPtr = &(dsaInfoPtr->dsaInfo.forward);
    GT_U32         useVidx = 0;
    GT_U32         vidx = 0;
    GT_U32         isSip5_20;

    isSip5_20 = (PRV_CPSS_SIP_5_20_CHECK_MAC(devNum) &&
                 dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT);
    /* bits 30:31 - TagCommand 3 = FORWARD */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],30,2,3);

    /* bit 29 -- SrcTagged */
    if((GT_BOOL)forwardPtr->srcIsTagged == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],29,1,1);
    }

    if (forwardPtr->srcIsTrunk == GT_FALSE)
    {
        PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(forwardPtr->srcHwDev,forwardPtr->source.portNum);
        hwSrcDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(forwardPtr->srcHwDev,forwardPtr->source.portNum);
        hwSrcPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(forwardPtr->srcHwDev,forwardPtr->source.portNum);
    }
    else
    {
        hwSrcDev = forwardPtr->srcHwDev;
    }

    if(((hwSrcDev >= BIT_5) &&
        (dsaInfoPtr->commonParams.dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
       ((hwSrcDev >= BIT_12) &&
        (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
       /* In sip5_20 devices SrcDev reduced to 10 bits (2 bits are freed for Hash). */
       ((hwSrcDev >= BIT_10) && isSip5_20)
      )
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bits 24:28 -- SrcDev */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],24,5,hwSrcDev);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* SrcDev is extended to 12 bits
           SrcDev[11:0] = {Word2[20:14], Word0[28:24]}
        */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],14,7, (hwSrcDev >> 5));
    }

    if(forwardPtr->srcIsTrunk == GT_FALSE)
    {
        switch (dsaInfoPtr->commonParams.dsaTagType)
        {
            case CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT:
                if (hwSrcPort >= BIT_5)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;

            case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
                if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
                {
                    if (hwSrcPort >= BIT_7)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                }
                else
                {
                    if (hwSrcPort >= BIT_6)
                    {
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                    }
                }
                break;

            case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
                if (hwSrcPort >= BIT_17)
                {
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
                }
                break;

            default:
                break;
        }

        /* bits 19:23 -- SrcPort[4:0] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,hwSrcPort);

        /* bit 18 -- SrcIsTrunk */
        /* not trunk */
    }
    else
    {
        if(((forwardPtr->source.trunkId >= BIT_5) &&
            (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)) ||
           ((forwardPtr->source.trunkId >= BIT_7) &&
            (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)) ||
           ((forwardPtr->source.trunkId >= BIT_12) &&
            (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* bits 19:23 -- SrcTrunk[4:0] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],19,5,forwardPtr->source.trunkId);

        /* bit 18 -- SrcIsTrunk */
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],18,1,1);
    }

    /* bit 16 - CFI */
    if(dsaInfoPtr->commonParams.cfiBit == 1)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],16,1,1);
    }

    /* bits 13:15 -- UP */
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],13,3,dsaInfoPtr->commonParams.vpt);

    switch(dsaInfoPtr->commonParams.dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            /* word[0] bit 12 = 1, word[1] bit 31 = 1,
               word[2] bit 31 = 1, word[3] bit 31 = 0 */
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[0],12,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],31,1,1);
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[2],31,1,1);
            break;
        default:
            break;
    }

    /* bits 0:11 -- VID */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],0,12,dsaInfoPtr->commonParams.vid);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        return GT_OK;
    }

    /******************************************/
    /* build the second, third and forth word */
    /******************************************/

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* VID becomes 16b eVLAN. eVLAN[15:0] = {Word3[30:27], Word0[11:0]} */
        /* 4 more bits */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],27,4,dsaInfoPtr->commonParams.vid>>12);
    }

    if(forwardPtr->srcIsTrunk == GT_FALSE)
    {
        if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)
        {
            /* bit 30 */
            /* src port bit 6 for SIP 5, reserved for legacy devices (value = 0) */
            if (PRV_CPSS_SIP_5_CHECK_MAC(devNum))
            {
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],
                                 30,/* 30 */
                                 1,   /*  1 */
                                 (hwSrcPort >> 6));
            }
            /* bit 29 */
            /* set use msb src port bits [5] */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],
                                 29,/* 29 */
                                 1,   /*  1 */
                                 (hwSrcPort >> 5));
        }

        if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            /* bits 29:30 */
            /* set use msb src port bits [6:5] */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],
                             29,/* 29 */
                             2,   /*  2 */
                             (hwSrcPort >> 5));

            /* SrcPort becoms 17b SRCePort
               This field contains the Source ePort assigned to the
               packet by the ingress device.
               SRCePort[16:0] = {Word2[12:3], Word1[30:29], Word0[23:19]} */

             U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],3,10,
                             (hwSrcPort >> 7));
        }
    }
    else
    {
        /* bits 29:30*/
        /* set use msb src trunk bits [6:5] */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],
                             29,/* 29 */
                             2,   /*  2 */
                             (forwardPtr->source.trunkId >> 5));

        if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            /* SrcTrunk is extended to 12b
               SrcTrunk[11:0] = {Word2[7:3], Word1[30:29], Word0[23:19]}*/

             U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],3,5,
                             (forwardPtr->source.trunkId >> 7));
        }
    }

    /* bit 28 -- egrFilterRegistered */
    if(forwardPtr->egrFilterRegistered == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],28,1,1);
    }

    /* bit 25 -- Routed */
    if(forwardPtr->wasRouted == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],25,1,1);
    }

    if(((forwardPtr->srcId >= BIT_5) &&
        (dsaInfoPtr->commonParams.dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
        ((forwardPtr->srcId >= BIT_12) &&
       (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bits 20-24 -- Src-ID */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],20,5,forwardPtr->srcId);

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
    {
        /* SrcID is extended to 12 bits
           SrcID[11:0] = {Word2[27:21], Word1[24:20]}*/

         U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],21,7,
                         (forwardPtr->srcId >> 5));
    }

    if(forwardPtr->qosProfileIndex >= BIT_7)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* bits 13-19 -- QoSProfile*/
    U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],13,7,forwardPtr->qosProfileIndex);

    if(forwardPtr->dstInterface.type == CPSS_INTERFACE_VIDX_E)
    {
        useVidx = 1;
        vidx = forwardPtr->dstInterface.vidx;

        /* bits 0-11 -- VIDX[11:0]*/
        if(((dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT) &&
           (vidx >= BIT_12)) ||
           ((dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT) &&
            (vidx >= BIT_16)) ||
            (vidx == VID_INSTEAD_VIDX_CNS))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else if(forwardPtr->dstInterface.type == CPSS_INTERFACE_VID_E)
    {
        useVidx = 1;
        vidx = VID_INSTEAD_VIDX_CNS;
    }

    if(useVidx)
    {
        /* bit 12 -- use_vidx */
        U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],12,1,1);

        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,12,vidx);

        if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            /* VIDX becomes 16b eVIDX Indicates (when eVIDX >= 4K) the
               L2MLL group or (when VIDX < 4K) the L2 multicast group
               to which the packet is transmitted.
               eVIDX[15:0] = {Word3[23:20], Word1[11:0]} */

             U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],20,4,
                             (vidx >> 12));
        }
    }
    else if(forwardPtr->dstInterface.type == CPSS_INTERFACE_TRUNK_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG); /* trunk is not supported */
    }
    else if(forwardPtr->dstInterface.type == CPSS_INTERFACE_PORT_E)/* port -- not support trunk */
    {
        /* bit 11 -- Reserved with value 0 */
        PRV_CPSS_DXCH_DUAL_HW_DEVICE_AND_PORT_CHECK_MAC(forwardPtr->dstInterface.devPort.hwDevNum,forwardPtr->dstInterface.devPort.portNum);
        hwDstDev = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_DEV_MAC(forwardPtr->dstInterface.devPort.hwDevNum,forwardPtr->dstInterface.devPort.portNum);
        hwDstPort = PRV_CPSS_DXCH_DUAL_HW_DEVICE_CONVERT_PORT_MAC(forwardPtr->dstInterface.devPort.hwDevNum,forwardPtr->dstInterface.devPort.portNum);

        if(((hwDstDev >= BIT_5) &&
            (dsaInfoPtr->commonParams.dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
           ((hwDstDev >= BIT_12 ) &&
            (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)) ||
           /* In sip5_20 devices TrgDev reduced to 10 bits (2 bits are freed for Hash). */
           ((hwDstDev >= BIT_10) && isSip5_20))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

         if((hwDstPort >= BIT_6 &&
            dsaInfoPtr->commonParams.dsaTagType != CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT) ||
           (hwDstPort >= BIT_8 &&
            dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT))
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

         if ((forwardPtr->dstEport >= BIT_17) &&
             (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT))
         {
             CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
         }

       /* bits 0-4 -- TrgDev*/
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],0,5,hwDstDev);

        if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT)
        {
            /* TrgPort is extended to 8 bits
               TrgPort[7:0] = {Word2[28], Word1[11:5]}
               The value in <TrgPort> field is valid only when
               <IsTrgPortValid> == TRUE */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],5,7,hwDstPort);

            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],28,1,(hwDstPort >> 7));

            if(forwardPtr->isTrgPhyPortValid == GT_TRUE)
            {
                U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],29,1,1);
            }

            /* TrgDev is extended to 12b
               TrgDev[11:0] = {Word3[6:0], Word1[4:0]} */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],0,7,(hwDstDev >> 5));

           /* TRGePort[16:0] = {Word3[23:7]}*/
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,17,forwardPtr->dstEport);
        }
        else
        {
             /* bits 5-10 -- TrgPort*/
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[1],5,6,hwDstPort);
        }
    }
    else
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* check for Cheetah3 device */
    if (PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily >=
        CPSS_PP_FAMILY_CHEETAH3_E)
    {
        if (dsaInfoPtr->commonParams.dropOnSource == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],27,1,1);
        }

        if (dsaInfoPtr->commonParams.packetIsLooped == GT_TRUE)
        {
            U32_SET_FIELD_NO_CLEAR_MAC(wordsPtr[1],26,1,1);
        }
    }

    if(dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)
    {
        return GT_OK;
    }

    if (forwardPtr->tag0TpidIndex >= BIT_3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Set data for word 2 */

    /* bits 2:0 -- tag0TpidIndex */
    U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],0,3,forwardPtr->tag0TpidIndex);

    /* Set data for word 3 */

    switch (forwardPtr->srcIsTagged)
    {
        case CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E:
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,0); /* Tag0 SrcTagged */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,0); /* Tag1 SrcTagged */
            break;

        case CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E:
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,1); /* Tag0 SrcTagged */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,0); /* Tag1 SrcTagged */
            break;

        case CPSS_DXCH_BRG_VLAN_PORT_TAG1_CMD_E:
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,0); /* Tag0 SrcTagged */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,1); /* Tag1 SrcTagged */
            break;

        case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG0_INNER_TAG1_CMD_E:
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,1); /* Tag0 SrcTagged */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,1); /* Tag1 SrcTagged */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],25,1,1); /* SrcTag0 Is OuterTag */
            break;

        case CPSS_DXCH_BRG_VLAN_PORT_OUTER_TAG1_INNER_TAG0_CMD_E:
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],29,1,1); /* Tag0 SrcTagged */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],26,1,1); /* Tag1 SrcTagged */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],25,1,0); /* SrcTag0 Is OuterTag */
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if(useVidx)
    {
        /* bit 19 -- origSrcPhyIsTrunk */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],19,1,forwardPtr->origSrcPhyIsTrunk);

        if (forwardPtr->origSrcPhyIsTrunk == GT_TRUE)
        {
            if (forwardPtr->origSrcPhy.trunkId >= BIT_12)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* bits 18:7 -- origSrcPhy.trunkId */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,12,forwardPtr->origSrcPhy.trunkId);
        }
        else
        {
            if (forwardPtr->origSrcPhy.portNum >= BIT_12)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* bits 18:7 -- origSrcPhy.portNum */
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],7,12,forwardPtr->origSrcPhy.portNum);
        }

        /* bit 6 -- phySrcMcFilterEnable */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],6,1,forwardPtr->phySrcMcFilterEnable);
    }


    if (isSip5_20)
    {
        /* Hash: word0[17], word2[13], word2[20:19], (useVidx ? word3[6:5] : word3[5:4])
           You can find the Hash bits overlap SrcDev, TrgDev (both 12 bits in sip 5).
           It's ok because SrcDev, TrgDev are guaranteed (above) to not exceed
           10 bits in sip5_20.
         */
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[0],17,1,forwardPtr->hash);
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],13,1,forwardPtr->hash>>1);
        U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[2],19,2,forwardPtr->hash>>2);
        if (useVidx)
        {
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],4,2,forwardPtr->hash>>4);
        }
        else
        {
            U32_SET_FIELD_NO_CLEAR_MASKED_MAC(wordsPtr[3],5,2,forwardPtr->hash>>4);
        }
    }

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChNetIfDsaTagBuild
*
* DESCRIPTION:
*       Build DSA tag bytes on the packet from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaInfoPtr - (pointer to) the DSA parameters that need to be built into
*                   the packet
*
* OUTPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*                     This pointer in the packet should hold space for :
*                     4 bytes when using regular DSA tag.
*                     8 bytes when using extended DSA tag.
*                     16 bytes when using eDSA tag.
*                     This function only set values into the 4, 8 or 16 bytes
*                     according to the parameters in parameter dsaInfoPtr.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*           support:
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*           ===== eDSA tag         ====
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfDsaTagBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U8                          *dsaBytesPtr
)
{
    GT_STATUS rc;
    GT_U32  wordsPtr[4] = {0,0,0,0};   /* 4 words of DSA tag */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(dsaBytesPtr);
    CPSS_NULL_PTR_CHECK_MAC(dsaInfoPtr);

    /* check common parameters */
    if (dsaInfoPtr->commonParams.cfiBit >= BIT_1 || dsaInfoPtr->commonParams.vpt >= BIT_3)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(dsaInfoPtr->commonParams.dsaTagType)
    {
        case CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT:
        case CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT:
            if (dsaInfoPtr->commonParams.vid >= BIT_12)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }
            break;
        case CPSS_DXCH_NET_DSA_4_WORD_TYPE_ENT:
            /* dsaInfoPtr->commonParams.vid is GT_U16. eDSA tag supports 16 bits
               of eVID. All values of dsaInfoPtr->commonParams.vid are valid. */
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch(dsaInfoPtr->dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            rc = prvCpssDxChNetIfDsaTagToCpuBuild(devNum,dsaInfoPtr,wordsPtr);;
            break;
        case CPSS_DXCH_NET_DSA_CMD_FROM_CPU_E:
            rc = prvCpssDxChNetIfDsaTagFromCpuBuild(devNum,dsaInfoPtr,wordsPtr);
            break;
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            rc = prvCpssDxChNetIfDsaTagToAnalyzerBuild(devNum,dsaInfoPtr,wordsPtr);
            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            rc = prvCpssDxChNetIfDsaTagForwardBuild(devNum,dsaInfoPtr,wordsPtr);
            break;
        default:
            rc = GT_BAD_PARAM;
    }

    if (rc != GT_OK)
    {
        return rc;
    }

    dsaBytesPtr[0] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],24,8);
    dsaBytesPtr[1] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0],16,8);
    dsaBytesPtr[2] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0], 8,8);
    dsaBytesPtr[3] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[0], 0,8);

    if (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_1_WORD_TYPE_ENT)
    {
        return GT_OK;
    }

    dsaBytesPtr[4] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1],24,8);
    dsaBytesPtr[5] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1],16,8);
    dsaBytesPtr[6] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1], 8,8);
    dsaBytesPtr[7] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[1], 0,8);

    if (dsaInfoPtr->commonParams.dsaTagType == CPSS_DXCH_NET_DSA_2_WORD_TYPE_ENT)
    {
        return GT_OK;
    }

    dsaBytesPtr[8] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[2],24,8);
    dsaBytesPtr[9] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[2],16,8);
    dsaBytesPtr[10] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[2], 8,8);
    dsaBytesPtr[11] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[2], 0,8);

    dsaBytesPtr[12] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[3],24,8);
    dsaBytesPtr[13] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[3],16,8);
    dsaBytesPtr[14] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[3], 8,8);
    dsaBytesPtr[15] = (GT_U8)U32_GET_FIELD_MAC(wordsPtr[3], 0,8);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfDsaTagBuild
*
* DESCRIPTION:
*       Build DSA tag bytes on the packet from the DSA tag parameters .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - device number (different DXCH devices may support differently
*                the DSA tag)
*       dsaInfoPtr - (pointer to) the DSA parameters that need to be built into
*                   the packet
*
* OUTPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*                     This pointer in the packet should hold space for :
*                     4 bytes when using regular DSA tag.
*                     8 bytes when using extended DSA tag.
*                     16 bytes when using eDSA tag.
*                     This function only set values into the 4, 8 or 16 bytes
*                     according to the parameters in parameter dsaInfoPtr.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*           support:
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*           ===== eDSA tag         ====
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfDsaTagBuild
(
    IN  GT_U8                          devNum,
    IN  CPSS_DXCH_NET_DSA_PARAMS_STC   *dsaInfoPtr,
    OUT GT_U8                          *dsaBytesPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfDsaTagBuild);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dsaInfoPtr, dsaBytesPtr));

    rc = internal_cpssDxChNetIfDsaTagBuild(devNum, dsaInfoPtr, dsaBytesPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dsaInfoPtr, dsaBytesPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfDuplicateEnableSet
*
* DESCRIPTION:
*       Enable descriptor duplication (mirror, STC and ingress mirroring
*       to analyzer port when the analyzer port is the CPU), Or Disable any kind
*       of duplication.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       enable -
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfDuplicateEnableSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
{
    GT_U32  regAddr;    /* register address */
    GT_U32  bitIndex;   /* bit index of field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
        bitIndex = 15;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg;
        bitIndex = 29;
    }

    return prvCpssHwPpSetRegField(devNum,regAddr,bitIndex,1,BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChNetIfDuplicateEnableSet
*
* DESCRIPTION:
*       Enable descriptor duplication (mirror, STC and ingress mirroring
*       to analyzer port when the analyzer port is the CPU), Or Disable any kind
*       of duplication.
*
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       enable -
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfDuplicateEnableSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfDuplicateEnableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChNetIfDuplicateEnableSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfDuplicateEnableGet
*
* DESCRIPTION:
*       Get descriptor duplication (mirror, STC and ingress mirroring
*       to analyzer port when the analyzer port is the CPU) status.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr -
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfDuplicateEnableGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData;
    GT_U32      regAddr;    /* register address */
    GT_U32      bitIndex;   /* bit index of field */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
        bitIndex = 15;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg;
        bitIndex = 29;
    }

    rc = prvCpssHwPpGetRegField(devNum,
                regAddr,
                bitIndex,
                1,
                &hwData);

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfDuplicateEnableGet
*
* DESCRIPTION:
*       Get descriptor duplication (mirror, STC and ingress mirroring
*       to analyzer port when the analyzer port is the CPU) status.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr -
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfDuplicateEnableGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfDuplicateEnableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChNetIfDuplicateEnableGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfPortDuplicateToCpuSet
*
* DESCRIPTION:
*       set per ingress port if Packets received from the port that are
*       duplicated and their duplication target is the CPU, ( mirror, STC and
*       ingress mirroring to analyzer port when the analyzer port is the CPU)
*       are will be duplicated or just are only forwarded to their original
*       target.
*       NOTE : port may be "CPU port" (63)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       enable -
*               GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortDuplicateToCpuSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  GT_BOOL                        enable
)
{
    GT_U32  bitNum;
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32   localPort;/* local port - support multi-port-groups device */
    GT_U32  regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.duplicationOfPktsToCPUConfig[portNum / 32];
        bitNum = portNum % 32;

        return prvCpssHwPpSetRegField(devNum, regAddr, bitNum, 1, BOOL2BIT_MAC(enable));
    }
    else
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        bitNum = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 28 : localPort;




        return prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg,
                    bitNum,
                    1,
                    BOOL2BIT_MAC(enable));
    }
}

/*******************************************************************************
* cpssDxChNetIfPortDuplicateToCpuSet
*
* DESCRIPTION:
*       set per ingress port if Packets received from the port that are
*       duplicated and their duplication target is the CPU, ( mirror, STC and
*       ingress mirroring to analyzer port when the analyzer port is the CPU)
*       are will be duplicated or just are only forwarded to their original
*       target.
*       NOTE : port may be "CPU port" (63)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       enable -
*               GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortDuplicateToCpuSet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    IN  GT_BOOL                        enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortDuplicateToCpuSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enable));

    rc = internal_cpssDxChNetIfPortDuplicateToCpuSet(devNum, portNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfPortDuplicateToCpuGet
*
* DESCRIPTION:
*       get per ingress port if Packets received from the port that are
*       duplicated and their duplication target is the CPU, ( mirror, STC and
*       ingress mirroring to analyzer port when the analyzer port is the CPU)
*       are will be duplicated or just are only forwarded to their original
*       target.
*       NOTE : port may be "CPU port" (63)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr -
*               GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL Ptr
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortDuplicateToCpuGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT GT_BOOL                        *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      bitNum;
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32       localPort;/* local port - support multi-port-groups device */
    GT_U32      hwData;
    GT_U32      regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);
    PRV_CPSS_DXCH_ENHANCED_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum, portNum);
    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {

        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.duplicationOfPktsToCPUConfig[portNum / 32];
        bitNum = portNum % 32;

        rc = prvCpssHwPpGetRegField(devNum, regAddr, bitNum, 1, &hwData);

    }
    else
    {
        /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
        localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum,portNum);

        bitNum = (localPort == CPSS_CPU_PORT_NUM_CNS) ? 28 : localPort;

        rc = prvCpssHwPpPortGroupGetRegField(devNum,portGroupId,
                    PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.dupPktsToCpuConfReg,
                    bitNum,
                    1,
                    &hwData);
    }

    *enablePtr = BIT2BOOL_MAC(hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfPortDuplicateToCpuGet
*
* DESCRIPTION:
*       get per ingress port if Packets received from the port that are
*       duplicated and their duplication target is the CPU, ( mirror, STC and
*       ingress mirroring to analyzer port when the analyzer port is the CPU)
*       are will be duplicated or just are only forwarded to their original
*       target.
*       NOTE : port may be "CPU port" (63)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr -
*               GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or port number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL Ptr
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortDuplicateToCpuGet
(
    IN  GT_U8                          devNum,
    IN  GT_PHYSICAL_PORT_NUM           portNum,
    OUT GT_BOOL                        *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortDuplicateToCpuGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portNum, enablePtr));

    rc = internal_cpssDxChNetIfPortDuplicateToCpuGet(devNum, portNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
*
* DESCRIPTION:
*       Enable/Disable Application Specific CPU Code for TCP SYN packets
*       forwarded to the CPU - TCP_SYN_TO_CPU.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - Device number.
*       enable - GT_TRUE  - enable application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disable application specific CPU Code for TCP SYN
*                           packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
{
    GT_U32  regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    }

    return prvCpssHwPpSetRegField(devNum,regAddr,12,1,BOOL2BIT_MAC(enable));
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
*
* DESCRIPTION:
*       Enable/Disable Application Specific CPU Code for TCP SYN packets
*       forwarded to the CPU - TCP_SYN_TO_CPU.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - Device number.
*       enable - GT_TRUE  - enable application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disable application specific CPU Code for TCP SYN
*                           packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
(
    IN  GT_U8                          devNum,
    IN  GT_BOOL                        enable
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeTcpSynSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enable));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeTcpSynSet(devNum, enable);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enable));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
*
* DESCRIPTION:
*       Get whether Application Specific CPU Code for TCP SYN packets forwarded
*       to the CPU - TCP_SYN_TO_CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - Device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) is the feature enabled
*               GT_TRUE  - enabled ,application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disabled ,application specific CPU Code for TCP SYN
*                           packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
{
    GT_STATUS   rc;
    GT_U32      data;
    GT_U32  regAddr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(enablePtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    }

    rc = prvCpssHwPpGetRegField(devNum,regAddr,12,1,&data);

    *enablePtr = BIT2BOOL_MAC(data);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
*
* DESCRIPTION:
*       Get whether Application Specific CPU Code for TCP SYN packets forwarded
*       to the CPU - TCP_SYN_TO_CPU.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum - Device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) is the feature enabled
*               GT_TRUE  - enabled ,application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disabled ,application specific CPU Code for TCP SYN
*                           packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
(
    IN  GT_U8                          devNum,
    OUT GT_BOOL                        *enablePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeTcpSynGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, enablePtr));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(devNum, enablePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, enablePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
*
* DESCRIPTION:
*       Set IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code. There are 8 IP Protocols may be defined.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (APPLICABLE RANGES: 0..7)
*       protocol  - IP protocol (APPLICABLE RANGES: 0..255)
*       cpuCode   - CPU Code for given IP protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    IN GT_U8            protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      ipRegIndex;   /* index of the register                        */
    GT_U32      bitOffset;    /* the bit offset inside register               */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode;  /* DSA Tag CPU Code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if (index > DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* calculate the index of the IP Protocol CPU Code registers */
    ipRegIndex = index / 2;

    /* calculate the address of the IP Protocol CPU Code register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.IPProtocolCPUCodeEntry[ipRegIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.ipProtCpuCodeBase + (ipRegIndex * 4);
    }

    /* calculate the bit offset */
    bitOffset = 16 * (index % 2);

    RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);

    /* calculate register data */
    regData = ((dsaCpuCode << 8) | protocol);

    /* Set IP Protocol and CPU Code in the IP Protocol CPU Code register */
    rc = prvCpssHwPpSetRegField(devNum, regAddr, bitOffset, 16, regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* calculate the address of the IP Protocol CPU Code Valid register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.IPProtocolCPUCodeValidConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid;
    }

    /* Set Valid bit in the IP Protocol CPU Code Valid register */
    return prvCpssHwPpSetRegField(devNum, regAddr, index, 1, 1);
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
*
* DESCRIPTION:
*       Set IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code. There are 8 IP Protocols may be defined.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (APPLICABLE RANGES: 0..7)
*       protocol  - IP protocol (APPLICABLE RANGES: 0..255)
*       cpuCode   - CPU Code for given IP protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    IN GT_U8            protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, protocol, cpuCode));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet(devNum, index, protocol, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, protocol, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
*
* DESCRIPTION:
*       Invalidate entry in the IP Protocol CPU Code Table .
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
(
    IN GT_U8            devNum,
    IN GT_U32           index
)
{
    GT_U32      regAddr;      /* The register address to read from.           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if (index > DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* calculate the address of the IP Protocol CPU Code Valid register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.IPProtocolCPUCodeValidConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid;
    }

    /* Set Valid bit in the IP Protocol CPU Code Valid register */
    return prvCpssHwPpSetRegField(devNum, regAddr, index, 1, 0);
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
*
* DESCRIPTION:
*       Invalidate entry in the IP Protocol CPU Code Table .
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
(
    IN GT_U8            devNum,
    IN GT_U32           index
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(devNum, index);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
*
* DESCRIPTION:
*       Get IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       validPtr     - (pointer to)valid entry.
*                       GT_FALSE - the entry is not valid
*                       GT_TRUE - the entry is valid
*       protocolPtr  - (pointer to)IP protocol (APPLICABLE RANGES: 0..255)
*       cpuCodePtr   - (pointer to)CPU Code for given IP protocol
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    OUT GT_BOOL         *validPtr,
    OUT GT_U8           *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      ipRegIndex;   /* index of the register                        */
    GT_U32      bitOffset;    /* the bit offset inside register               */
    GT_U32      dsaCpuCode;  /* DSA Tag CPU Code                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(protocolPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    if (index > DXCH2_CPU_CODE_IP_PROTOCOL_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* calculate the index of the IP Protocol CPU Code registers */
    ipRegIndex = index / 2;

    /* calculate the address of the IP Protocol CPU Code register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.IPProtocolCPUCodeEntry[ipRegIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.ipProtCpuCodeBase + (ipRegIndex * 4);
    }

    /* calculate the bit offset */
    bitOffset = 16 * (index % 2);

    /* Get IP Protocol and CPU Code in the IP Protocol CPU Code register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitOffset, 16, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* calculate register data */
    *protocolPtr = (GT_U8)U32_GET_FIELD_MAC(regData,0,8);
    dsaCpuCode  =        U32_GET_FIELD_MAC(regData,8,8);

    DSA_TO_RX_OPCODE_CONVERT_MAC(dsaCpuCode,cpuCodePtr);

    /* calculate the address of the IP Protocol CPU Code Valid register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.IPProtocolCPUCodeValidConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
            bufferMng.eqBlkCfgRegs.ipProtCpuCodeValid;
    }

    /* Set Valid bit in the IP Protocol CPU Code Valid register */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, index, 1, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *validPtr = BIT2BOOL_MAC(regData);

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
*
* DESCRIPTION:
*       Get IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       validPtr     - (pointer to)valid entry.
*                       GT_FALSE - the entry is not valid
*                       GT_TRUE - the entry is valid
*       protocolPtr  - (pointer to)IP protocol (APPLICABLE RANGES: 0..255)
*       cpuCodePtr   - (pointer to)CPU Code for given IP protocol
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    OUT GT_BOOL         *validPtr,
    OUT GT_U8           *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, validPtr, protocolPtr, cpuCodePtr));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(devNum, index, validPtr, protocolPtr, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, validPtr, protocolPtr, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
*
* DESCRIPTION:
*       Set range to TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (APPLICABLE RANGES: 0..15)
*       maxDstPort - the maximum destination port in this range.
*       minDstPort - the minimum destination port in this range
*       packetType - packet type (Unicast/Multicast)
*       protocol   - protocol type (UDP/TCP)
*       cpuCode    - CPU Code Index for this TCP/UDP range
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex or
*                          bad packetType or bad protocol or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex,
    IN GT_U16                           minDstPort,
    IN GT_U16                           maxDstPort,
    IN CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType,
    IN CPSS_NET_PROT_ENT                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode;  /* DSA Tag CPU Code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if(rangeIndex > DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 0 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.TCPUDPDestPortRangeCPUCodeEntryWord0[rangeIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord0Base + (rangeIndex * 4);
    }

    /* calculate register data */
    regData = ((maxDstPort << 16) | minDstPort);

    /* write Port Range to the TCP/UDP Dest CPU Code word 0 */
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 1 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.TCPUDPDestPortRangeCPUCodeEntryWord1[rangeIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base + (rangeIndex * 4);
    }

    regData = 0;

    /* calculate register data */
    switch (packetType)
    {
        case CPSS_NET_TCP_UDP_PACKET_UC_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,10,2,0);
            break;
        case CPSS_NET_TCP_UDP_PACKET_MC_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,10,2,1);
            break;
        case CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,10,2,2);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    switch (protocol)
    {
        case CPSS_NET_PROT_UDP_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,8,2,1);
            break;
        case CPSS_NET_PROT_TCP_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,8,2,2);
            break;
        case CPSS_NET_PROT_BOTH_UDP_TCP_E:
            U32_SET_FIELD_NO_CLEAR_MAC(regData,8,2,3);
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* translate cpu opcode provided by application to HW cpu opcode */
    RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);

    U32_SET_FIELD_NO_CLEAR_MAC(regData,0,8,dsaCpuCode);

    /* set TCP/UDP Port range parameters in the
       TCP/UDP Port range CPU Code Entry word 1*/
    return prvCpssHwPpSetRegField(devNum, regAddr, 0, 12, regData);
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
*
* DESCRIPTION:
*       Set range to TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (APPLICABLE RANGES: 0..15)
*       maxDstPort - the maximum destination port in this range.
*       minDstPort - the minimum destination port in this range
*       packetType - packet type (Unicast/Multicast)
*       protocol   - protocol type (UDP/TCP)
*       cpuCode    - CPU Code Index for this TCP/UDP range
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex or
*                          bad packetType or bad protocol or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex,
    IN GT_U16                           minDstPort,
    IN GT_U16                           maxDstPort,
    IN CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType,
    IN CPSS_NET_PROT_ENT                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT         cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(devNum, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rangeIndex, minDstPort, maxDstPort, packetType, protocol, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
*
* DESCRIPTION:
*       invalidate range to TCP/UPD Destination Port Range CPU Code Table with
*       specific CPU Code. There are 16 ranges may be defined.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex
)
{
    GT_U32      regAddr;      /* The register address to read from.        */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    if(rangeIndex > DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 1 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.TCPUDPDestPortRangeCPUCodeEntryWord1[rangeIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base + (rangeIndex * 4);
    }

    /* set the 2 bits to value 0  --> no valid entry */
    return prvCpssHwPpSetRegField(devNum, regAddr, 8, 2, 0);
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
*
* DESCRIPTION:
*       invalidate range to TCP/UPD Destination Port Range CPU Code Table with
*       specific CPU Code. There are 16 ranges may be defined.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
(
    IN GT_U8                            devNum,
    IN GT_U32                           rangeIndex
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rangeIndex));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(devNum, rangeIndex);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rangeIndex));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
*
* DESCRIPTION:
*       Get range for TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       validPtr  - (pointer to) is the entry valid
*                   GT_FALSE - range is't valid
*                   GT_TRUE  - range is valid
*       maxDstPortPtr - (pointer to)the maximum destination port in this range.
*       minDstPortPtr - (pointer to)the minimum destination port in this range
*       packetTypePtr - (pointer to)packet type (Unicast/Multicast)
*       protocolPtr- (pointer to)protocol type (UDP/TCP)
*                    NOTE : this field will hold valid value only when
*                           (*validPtr) = GT_TRUE
*                           because in HW the "valid" is one of the protocol
*                           options
*       cpuCodePtr - (pointer to)CPU Code Index for this TCP/UDP range
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
(
    IN GT_U8                             devNum,
    IN GT_U32                            rangeIndex,
    OUT GT_BOOL                          *validPtr,
    OUT GT_U16                           *minDstPortPtr,
    OUT GT_U16                           *maxDstPortPtr,
    OUT CPSS_NET_TCP_UDP_PACKET_TYPE_ENT *packetTypePtr,
    OUT CPSS_NET_PROT_ENT                *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT         *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      dsaCpuCode;  /* DSA Tag CPU Code                         */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(validPtr);
    CPSS_NULL_PTR_CHECK_MAC(minDstPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(maxDstPortPtr);
    CPSS_NULL_PTR_CHECK_MAC(packetTypePtr);
    CPSS_NULL_PTR_CHECK_MAC(protocolPtr);
    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    if(rangeIndex > DXCH2_CPU_CODE_L4_DEST_PORT_RANGE_MAX_INDEX_CNS)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* Address of the TCP/UDP Dest Port Range CPU Code word 0 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.TCPUDPDestPortRangeCPUCodeEntryWord0[rangeIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord0Base + (rangeIndex * 4);
    }

    /* write Port Range to the TCP/UDP Dest CPU Code word 0 */
    rc = prvCpssHwPpReadRegister(devNum, regAddr, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    *minDstPortPtr = (GT_U16)U32_GET_FIELD_MAC(regData, 0,16);
    *maxDstPortPtr = (GT_U16)U32_GET_FIELD_MAC(regData,16,16);


    /* Address of the TCP/UDP Dest Port Range CPU Code word 1 */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            applicationSpecificCPUCodes.TCPUDPDestPortRangeCPUCodeEntryWord1[rangeIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.tcpUdpDstPortRangeCpuCodeWord1Base + (rangeIndex * 4);
    }

    /* set TCP/UDP Port range parameters in the
       TCP/UDP Port range CPU Code Entry word 1*/
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 0, 12, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    dsaCpuCode = U32_GET_FIELD_MAC(regData,0,8);
    DSA_TO_RX_OPCODE_CONVERT_MAC(dsaCpuCode,cpuCodePtr);

    /* calculate register data */
    switch (U32_GET_FIELD_MAC(regData,10,2))
    {
        case 0:
            *packetTypePtr = CPSS_NET_TCP_UDP_PACKET_UC_E;
            break;
        case 1:
            *packetTypePtr = CPSS_NET_TCP_UDP_PACKET_MC_E;
            break;
        case 2:
        default:
            *packetTypePtr = CPSS_NET_TCP_UDP_PACKET_BOTH_UC_MC_E;
            break;
    }

    *validPtr = GT_TRUE;

    switch (U32_GET_FIELD_MAC(regData,8,2))
    {
        case 0:
            *validPtr = GT_FALSE;
            *protocolPtr = CPSS_NET_PROT_UDP_E;/* dummy !!! */
            break;
        case 1:
            *protocolPtr = CPSS_NET_PROT_UDP_E;
            break;
        case 2:
            *protocolPtr = CPSS_NET_PROT_TCP_E;
            break;
        case 3:
        default:
            *protocolPtr = CPSS_NET_PROT_BOTH_UDP_TCP_E;
            break;
    }


    return GT_OK;

}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
*
* DESCRIPTION:
*       Get range for TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (APPLICABLE RANGES: 0..15)
*
* OUTPUTS:
*       validPtr  - (pointer to) is the entry valid
*                   GT_FALSE - range is't valid
*                   GT_TRUE  - range is valid
*       maxDstPortPtr - (pointer to)the maximum destination port in this range.
*       minDstPortPtr - (pointer to)the minimum destination port in this range
*       packetTypePtr - (pointer to)packet type (Unicast/Multicast)
*       protocolPtr- (pointer to)protocol type (UDP/TCP)
*                    NOTE : this field will hold valid value only when
*                           (*validPtr) = GT_TRUE
*                           because in HW the "valid" is one of the protocol
*                           options
*       cpuCodePtr - (pointer to)CPU Code Index for this TCP/UDP range
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rangeIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
(
    IN GT_U8                             devNum,
    IN GT_U32                            rangeIndex,
    OUT GT_BOOL                          *validPtr,
    OUT GT_U16                           *minDstPortPtr,
    OUT GT_U16                           *maxDstPortPtr,
    OUT CPSS_NET_TCP_UDP_PACKET_TYPE_ENT *packetTypePtr,
    OUT CPSS_NET_PROT_ENT                *protocolPtr,
    OUT CPSS_NET_RX_CPU_CODE_ENT         *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rangeIndex, validPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr));

    rc = internal_cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(devNum, rangeIndex, validPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rangeIndex, validPtr, minDstPortPtr, maxDstPortPtr, packetTypePtr, protocolPtr, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* prvCpuCodeIpLinkLocalProtRegAddrAndOffsetGet
*
* DESCRIPTION:
*       Get register address and field offset for CPU code of
*       IP Link Local multicast protocol
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*
* OUTPUTS:
*       regAddrPtr - (pointer to) register address
*       bitOffsetPtr - (pointer to) field bit offset
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on bad ipVer
*
*
*******************************************************************************/
static GT_STATUS prvCpuCodeIpLinkLocalProtRegAddrAndOffsetGet
(
    IN  GT_U8                           devNum,
    IN  CPSS_IP_PROTOCOL_STACK_ENT      ipVer,
    IN  GT_U8                           protocol,
    OUT GT_U32                         *regAddrPtr,
    OUT GT_U32                         *bitOffsetPtr
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regIndex;     /* Register index                               */
    GT_U32      t;            /* variable for address calculation             */

    /* each register has 2 bits for 16 protocols */
    *bitOffsetPtr = (2 * (protocol % 16));

    /* calculate register index and bit offset according to the protocol */
    if((protocol % 32) > 15)
    {
        t = 1;
    }
    else
    {
        t = 0;
    }

    /* calculate the base address of the register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum) == GT_TRUE)
    {
        switch (ipVer)
        {
            case CPSS_IP_PROTOCOL_IPV4_E:
                regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    ctrlTrafficToCPUConfig.IPv4McLinkLocalCPUCodeIndex[protocol / 32][t];
                break;
            case CPSS_IP_PROTOCOL_IPV6_E:
                regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    ctrlTrafficToCPUConfig.IPv6McLinkLocalCPUCodeIndex[protocol / 32][t];
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        switch (ipVer)
        {
            case CPSS_IP_PROTOCOL_IPV4_E:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bufferMng.eqBlkCfgRegs.ipv4McstLinkLocalCpuCodeIndexBase;
                break;
            case CPSS_IP_PROTOCOL_IPV6_E:
                regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                    bufferMng.eqBlkCfgRegs.ipv6McstLinkLocalCpuCodeIndexBase;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        regIndex = (protocol / 32);

        /* Address of the IPv4 or IPv6 Multicast CPU Code register */
        regAddr += (regIndex * 0x1000) + t * 0x8000;
    }


    *regAddrPtr = regAddr;

    return GT_OK;
}


/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeIpLinkLocalProtSet
*
* DESCRIPTION:
*       Configure CPU code for IPv4 and IPv6 Link Local multicast packets
*       with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad ipVer or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeIpLinkLocalProtSet
(
    IN GT_U8                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT    ipVer,
    IN GT_U8                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT  cpuCode
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */
    GT_STATUS   rc;           /* return status */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    /* check cpuCode validity */
    switch (cpuCode)
    {
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E:
            regData = 0;
            break;
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E:
            regData = 1;
            break;
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E:
            regData = 2;
            break;
        case CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E:
            regData = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* calculate register address and bit offset */
    rc = prvCpuCodeIpLinkLocalProtRegAddrAndOffsetGet(devNum, ipVer, protocol,
                                                      &regAddr, &bitOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write IP Multicast CPU Code index to one of the
      IPv4 or IPv6 Multicast Link Local CPU Code Index registers */
    return prvCpssHwPpSetRegField(devNum, regAddr, bitOffset, 2, regData);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIpLinkLocalProtSet
*
* DESCRIPTION:
*       Configure CPU code for IPv4 and IPv6 Link Local multicast packets
*       with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad ipVer or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIpLinkLocalProtSet
(
    IN GT_U8                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT    ipVer,
    IN GT_U8                protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT  cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeIpLinkLocalProtSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipVer, protocol, cpuCode));

    rc = internal_cpssDxChNetIfCpuCodeIpLinkLocalProtSet(devNum, ipVer, protocol, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipVer, protocol, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeIpLinkLocalProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for IPv4 and IPv6 Link Local multicast
*       packets with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to) The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad ipVer
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeIpLinkLocalProtGet
(
    IN GT_U8                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT    ipVer,
    IN GT_U8                protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT  *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    /* calculate register address and bit offset */
    rc = prvCpuCodeIpLinkLocalProtRegAddrAndOffsetGet(devNum, ipVer, protocol,
                                                      &regAddr, &bitOffset);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* write IP Multicast CPU Code index to one of the
      IPv4 or IPv6 Multicast Link Local CPU Code Index registers */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitOffset, 2, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch (regData)
    {
        case 0:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E;
            break;
        case 1:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E;
            break;
        case 2:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E;
            break;
        case 3:
        default:
            *cpuCodePtr = CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIpLinkLocalProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for IPv4 and IPv6 Link Local multicast
*       packets with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to) The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad ipVer
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIpLinkLocalProtGet
(
    IN GT_U8                devNum,
    IN CPSS_IP_PROTOCOL_STACK_ENT    ipVer,
    IN GT_U8                protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT  *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeIpLinkLocalProtGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, ipVer, protocol, cpuCodePtr));

    rc = internal_cpssDxChNetIfCpuCodeIpLinkLocalProtGet(devNum, ipVer, protocol, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, ipVer, protocol, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
*
* DESCRIPTION:
*       Configure CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
(
    IN GT_U8            devNum,
    IN GT_U8            protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regIndex;     /* Register index                               */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);


    /* check validity of cpuCode */
    switch (cpuCode)
    {
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E:
            regData = 0;
            break;
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E:
            regData = 1;
            break;
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E:
            regData = 2;
            break;
        case CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E:
            regData = 3;
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* calculate register index and bit offset according to protocol */
    regIndex = protocol / 16;
    bitOffset = (2 * (protocol % 16));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    ctrlTrafficToCPUConfig.IEEEReservedMcCPUIndex[regIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
                    eqBlkCfgRegs.ieeeResMcstCpuCodeIndexBase + (regIndex * 0x1000);
    }

    /* Set IEEE CPU Code Index for specific protocol */
    return prvCpssHwPpSetRegField(devNum, regAddr, bitOffset, 2, regData);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
*
* DESCRIPTION:
*       Configure CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad cpuCode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
(
    IN GT_U8            devNum,
    IN GT_U8            protocol,
    IN CPSS_NET_RX_CPU_CODE_ENT   cpuCode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, protocol, cpuCode));

    rc = internal_cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(devNum, protocol, cpuCode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, protocol, cpuCode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to)The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
(
    IN GT_U8            devNum,
    IN GT_U8            protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regIndex;     /* Register index                               */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      bitOffset;    /* The bit offset inside the register           */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(cpuCodePtr);

    /* calculate register index and bit offset according to protocol */
    regIndex = protocol / 16;
    bitOffset = (2 * (protocol % 16));

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_L2I_MAC(devNum).
                    ctrlTrafficToCPUConfig.IEEEReservedMcCPUIndex[regIndex];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.
            eqBlkCfgRegs.ieeeResMcstCpuCodeIndexBase + (regIndex * 0x1000);
    }

    /* Set IEEE CPU Code Index for specific protocol */
    rc = prvCpssHwPpGetRegField(devNum, regAddr, bitOffset, 2, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    switch (regData)
    {
        case 0:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E;
            break;
        case 1:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E;
            break;
        case 2:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E;
            break;
        case 3:
        default:
            *cpuCodePtr = CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E;
            break;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to)The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
(
    IN GT_U8            devNum,
    IN GT_U8            protocol,
    OUT CPSS_NET_RX_CPU_CODE_ENT   *cpuCodePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, protocol, cpuCodePtr));

    rc = internal_cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(devNum, protocol, cpuCodePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, protocol, cpuCodePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeTableSet
*
* DESCRIPTION:
*       Function to set the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*       entryInfoPtr - (pointer to) The entry information
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode or bad one of
*                          bad one entryInfoPtr parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeTableSet
(
    IN GT_U8                    devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U32  hwData = 0;
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode=0;  /* DSA Tag CPU Code */
    GT_U32      dp;   /* drop precedence HW value                */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryInfoPtr);
    if(cpuCode != CPSS_NET_ALL_CPU_OPCODES_E)
    {
        RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);
    }
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(entryInfoPtr->tc);
    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, entryInfoPtr->dp, dp);

    CPU_CODE_DESIGNATED_DEVICE_INDEX_CHECK_MAC(entryInfoPtr->designatedDevNumIndex);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if(entryInfoPtr->cpuCodeRateLimiterIndex   >= BIT_8 ||
           entryInfoPtr->cpuCodeStatRateLimitIndex >= BIT_8)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        U32_SET_FIELD_NO_CLEAR_MAC(hwData,24,8,entryInfoPtr->cpuCodeRateLimiterIndex);
        U32_SET_FIELD_NO_CLEAR_MAC(hwData,16,8,entryInfoPtr->cpuCodeStatRateLimitIndex);

        switch(entryInfoPtr->cpuRateLimitMode)
        {
            case CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E:
                break;
            case CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E:
                U32_SET_FIELD_NO_CLEAR_MAC(hwData,15,1,1);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        if(PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_CHEETAH_E)
        {
            /* for CH2 and above */
            CPU_CODE_RATE_LIMITER_INDEX_CHECK_MAC(entryInfoPtr->cpuCodeRateLimiterIndex);

            U32_SET_FIELD_NO_CLEAR_MAC(hwData,14,5,entryInfoPtr->cpuCodeRateLimiterIndex);
        }

        CPU_CODE_STATISTICAL_RATE_LIMITER_INDEX_CHECK_MAC(entryInfoPtr->cpuCodeStatRateLimitIndex);
        U32_SET_FIELD_NO_CLEAR_MAC(hwData, 6,5,entryInfoPtr->cpuCodeStatRateLimitIndex);

        switch(entryInfoPtr->cpuRateLimitMode)
        {
            case CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E:
                break;
            case CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E:
                U32_SET_FIELD_NO_CLEAR_MAC(hwData,19,1,1);
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

    }

    U32_SET_FIELD_NO_CLEAR_MAC(hwData,0,3,entryInfoPtr->tc);

    U32_SET_FIELD_NO_CLEAR_MAC(hwData,3,2,dp);

    if(entryInfoPtr->truncate == GT_TRUE)
    {
        U32_SET_FIELD_NO_CLEAR_MAC(hwData,5,1,1);
    }

    U32_SET_FIELD_NO_CLEAR_MAC(hwData,11,3,entryInfoPtr->designatedDevNumIndex);

    if(cpuCode != CPSS_NET_ALL_CPU_OPCODES_E) /* specific entry */
    {
        rc = prvCpssDxChWriteTableEntry(devNum,
                                        PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                        (GT_U32)dsaCpuCode,/* DSA CPU code */
                                        &hwData);
        if (rc != GT_OK)
        {
            return rc;
        }

        if ( PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
        {
            rc = cpssExtDrvEthCpuCodeToQueue(dsaCpuCode,entryInfoPtr->tc);
            /* ignore GT_NOT_SUPPORTED, GT_NOT_IMPLEMENTED ...*/
            if (rc == GT_FAIL)
            {
                return rc;
            }
        }
    }
    else
    {
        for(ii = 0 ; ii < 256 ; ii++)
        {
            /* write the full table */
            rc = prvCpssDxChWriteTableEntry(devNum,
                                            PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                            ii,/* DSA CPU code */
                                            &hwData);
            if(rc != GT_OK)
            {
                return rc;
            }

            if ( PRV_CPSS_DXCH_CHECK_SDMA_PACKETS_FROM_CPU_CORRUPT_WA_MAC(devNum))
            {
                rc = cpssExtDrvEthCpuCodeToQueue(ii,entryInfoPtr->tc);
                /* ignore GT_NOT_SUPPORTED, GT_NOT_IMPLEMENTED ...*/
                if (rc == GT_FAIL)
                {
                    return rc;
                }
            }
        }
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeTableSet
*
* DESCRIPTION:
*       Function to set the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*       entryInfoPtr - (pointer to) The entry information
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode or bad one of
*                          bad one entryInfoPtr parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeTableSet
(
    IN GT_U8                    devNum,
    IN CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    IN CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode, entryInfoPtr));

    rc = internal_cpssDxChNetIfCpuCodeTableSet(devNum, cpuCode, entryInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode, entryInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeTableGet
*
* DESCRIPTION:
*       Function to get the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*
* OUTPUTS:
*       entryInfoPtr - (pointer to) The entry information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad cpuCode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeTableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    OUT CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData; /* data in the HW format */
    PRV_CPSS_DXCH_NET_DSA_TAG_CPU_CODE_ENT      dsaCpuCode=0;  /* DSA Tag CPU Code */
    GT_U32  hwDp;       /* HW DP value */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(entryInfoPtr);
    RX_OPCODE_TO_DSA_CONVERT_MAC(cpuCode,&dsaCpuCode);

    /* read CPU code entry */
    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_CPU_CODE_E,
                                    (GT_U32)dsaCpuCode,/* DSA CPU code */
                                    &hwData);
    if(rc != GT_OK)
    {
        return rc;
    }

    entryInfoPtr->tc = (GT_U8)U32_GET_FIELD_MAC(hwData,0,3);

    hwDp = U32_GET_FIELD_MAC(hwData,3,2);
    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, hwDp, entryInfoPtr->dp);

    entryInfoPtr->truncate = U32_GET_FIELD_MAC(hwData,5,1) ? GT_TRUE : GT_FALSE;
    entryInfoPtr->designatedDevNumIndex     = U32_GET_FIELD_MAC(hwData,11,3);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        entryInfoPtr->cpuCodeRateLimiterIndex   = U32_GET_FIELD_MAC(hwData,24,8);
        entryInfoPtr->cpuCodeStatRateLimitIndex = U32_GET_FIELD_MAC(hwData,16,8);

        entryInfoPtr->cpuRateLimitMode = U32_GET_FIELD_MAC(hwData,15,1) ?
                CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E :
                CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E ;
    }
    else
    {
        entryInfoPtr->cpuCodeStatRateLimitIndex = U32_GET_FIELD_MAC(hwData, 6,5);
        entryInfoPtr->cpuCodeRateLimiterIndex   = U32_GET_FIELD_MAC(hwData,14,5);

        entryInfoPtr->cpuRateLimitMode = U32_GET_FIELD_MAC(hwData,19,1) ?
                CPSS_NET_CPU_CODE_RATE_LIMIT_AGGREGATE_E :
                CPSS_NET_CPU_CODE_RATE_LIMIT_LOCAL_E ;
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeTableGet
*
* DESCRIPTION:
*       Function to get the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*
* OUTPUTS:
*       entryInfoPtr - (pointer to) The entry information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad cpuCode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeTableGet
(
    IN  GT_U8                    devNum,
    IN  CPSS_NET_RX_CPU_CODE_ENT cpuCode,
    OUT CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC *entryInfoPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, cpuCode, entryInfoPtr));

    rc = internal_cpssDxChNetIfCpuCodeTableGet(devNum, cpuCode, entryInfoPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, cpuCode, entryInfoPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
*
* DESCRIPTION:
*       Function to set the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (APPLICABLE RANGES: 0..31)
*       statisticalRateLimit - The statistical rate limit compared to the
*                              32-bit pseudo-random generator (PRNG).
*           NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U32       statisticalRateLimit
)
{
    GT_U32  hwData[1];

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    /* the index is checked by the tables engine */

    hwData[0] = statisticalRateLimit;

    /* write statistical rate limit entry */
    return prvCpssDxChWriteTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
                                    index,/* index */
                                    hwData);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
*
* DESCRIPTION:
*       Function to set the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (APPLICABLE RANGES: 0..31)
*       statisticalRateLimit - The statistical rate limit compared to the
*                              32-bit pseudo-random generator (PRNG).
*           NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
(
    IN GT_U8        devNum,
    IN GT_U32       index,
    IN GT_U32       statisticalRateLimit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statisticalRateLimit));

    rc = internal_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(devNum, index, statisticalRateLimit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statisticalRateLimit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
*
* DESCRIPTION:
*       Function to get the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       statisticalRateLimitPtr - (pointer to)The statistical rate limit
*             compared to the 32-bit pseudo-random generator (PRNG).
*
*           NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      index,
    OUT GT_U32      *statisticalRateLimitPtr
)
{
    GT_STATUS   rc;           /* return code */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(statisticalRateLimitPtr);
    /* the index is checked by the tables engine */

    /* read statistical rate limit entry */
    rc = prvCpssDxChReadTableEntry(devNum,
                                    PRV_CPSS_DXCH_TABLE_STATISTICAL_RATE_LIMIT_E,
                                    index,/* index */
                                    statisticalRateLimitPtr);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
*
* DESCRIPTION:
*       Function to get the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (APPLICABLE RANGES: 0..31)
*
* OUTPUTS:
*       statisticalRateLimitPtr - (pointer to)The statistical rate limit
*             compared to the 32-bit pseudo-random generator (PRNG).
*
*           NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
(
    IN  GT_U8       devNum,
    IN  GT_U32      index,
    OUT GT_U32      *statisticalRateLimitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, statisticalRateLimitPtr));

    rc = internal_cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(devNum, index, statisticalRateLimitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, statisticalRateLimitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* prvCpssCpuCodeDesignatedDeviceTableRegInfoGet
*
* DESCRIPTION:
*       Function calculates register address and field's position.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (APPLICABLE RANGES: 1..7)
*
* OUTPUTS:
*       regAddrPtr  - pointer to regiter's address
*       startBitPtr - pointer to regiter's start bit of field
*       numBitsPtr  - pointer to regiter's number of bits in field
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad index
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS prvCpssCpuCodeDesignatedDeviceTableRegInfoGet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    OUT GT_U32         *regAddrPtr,
    OUT GT_U32         *startBitPtr,
    OUT GT_U32         *numBitsPtr
)
{
    GT_U32  startBit;   /* start bit */
    GT_U32  regAddr;    /* address of register */
    GT_U32  numBits;    /* number of bits */

    CPU_CODE_DESIGNATED_DEVICE_INDEX_AS_POINTER_CHECK_MAC(index);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        startBit = (index & 1) * 10;
        numBits = 10;
        switch(index)
        {
            case 1:
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).CPUTargetDeviceConfig.CPUTargetDeviceConfigReg0;
                /* device 1 is in bits 0:9 */
                startBit = 0;
                break;
            case 2:
            case 3:
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).CPUTargetDeviceConfig.CPUTargetDeviceConfigReg1;
                break;
            case 4:
            case 5:
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).CPUTargetDeviceConfig.CPUTargetDeviceConfigReg2;
                break;
            case 6:
            case 7:
                regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).CPUTargetDeviceConfig.CPUTargetDeviceConfigReg3;
                break;
            case 0:/* this case never happen due to check of CPU_CODE_DESIGNATED_DEVICE_INDEX_AS_POINTER_CHECK_MAC */
            default:/* this case never happen due to check of CPU_CODE_DESIGNATED_DEVICE_INDEX_AS_POINTER_CHECK_MAC */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        startBit = ((index - 1) % 4) * 5;
        numBits = 5;
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuTargDevConfReg0 +
                                       (((index -1) / 4) * 4);
    }

    *regAddrPtr     = regAddr;
    *startBitPtr    = startBit;
    *numBitsPtr     = numBits;

    return GT_OK;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
*
* DESCRIPTION:
*       Function to set the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (APPLICABLE RANGES: 1..7)
*       designatedHwDevNum - The designated HW device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index or bad
*                          designatedHwDevNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    IN GT_HW_DEV_NUM    designatedHwDevNum
)
{
    GT_STATUS   rc;   /* return code */
    GT_U32  hwData;   /* HW data */
    GT_U32  startBit; /* start bit */
    GT_U32  regAddr;  /* address of register */
    GT_U32  numBits;  /* number of bits */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    if(designatedHwDevNum > PRV_CPSS_DXCH_PP_HW_MAX_VALUE_OF_HW_DEV_NUM_MAC(devNum))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* get register's info */
    rc = prvCpssCpuCodeDesignatedDeviceTableRegInfoGet(devNum, index, &regAddr,
                                                       &startBit, &numBits);
    if (rc != GT_OK)
    {
        return rc;
    }

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        hwData = (designatedHwDevNum & 0x3FF);
    }
    else
    {
        hwData = (designatedHwDevNum & 0x1F);
    }

    /* write designated device entry */
    rc = prvCpssHwPpSetRegField(devNum,regAddr,startBit,numBits,hwData);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
*
* DESCRIPTION:
*       Function to set the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (APPLICABLE RANGES: 1..7)
*       designatedHwDevNum - The designated HW device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index or bad
*                          designatedHwDevNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
(
    IN GT_U8            devNum,
    IN GT_U32           index,
    IN GT_HW_DEV_NUM    designatedHwDevNum
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeDesignatedDeviceTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, designatedHwDevNum));

    rc = internal_cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(devNum, index, designatedHwDevNum);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, designatedHwDevNum));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
*
* DESCRIPTION:
*       Function to get the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (APPLICABLE RANGES: 1..7)
*
* OUTPUTS:
*       designatedHwDevNumPtr - (pointer to)The designated HW device number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_HW_DEV_NUM   *designatedHwDevNumPtr
)
{
    GT_STATUS   rc;   /* return code */
    GT_U32  hwData;   /* HW data */
    GT_U32  startBit; /* start bit */
    GT_U32  regAddr;  /* address of register */
    GT_U32  numBits;  /* number of bits */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    CPSS_NULL_PTR_CHECK_MAC(designatedHwDevNumPtr);

    /* get register's info */
    rc = prvCpssCpuCodeDesignatedDeviceTableRegInfoGet(devNum, index, &regAddr,
                                                       &startBit, &numBits);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* read designated device entry */
    rc = prvCpssHwPpGetRegField(devNum,regAddr,startBit,numBits,&hwData);

    *designatedHwDevNumPtr = hwData;

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
*
* DESCRIPTION:
*       Function to get the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (APPLICABLE RANGES: 1..7)
*
* OUTPUTS:
*       designatedHwDevNumPtr - (pointer to)The designated HW device number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad index
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
(
    IN  GT_U8           devNum,
    IN  GT_U32          index,
    OUT GT_HW_DEV_NUM   *designatedHwDevNumPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeDesignatedDeviceTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, index, designatedHwDevNumPtr));

    rc = internal_cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(devNum, index, designatedHwDevNumPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, index, designatedHwDevNumPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure CPU rate limiter entry attributes: window size and max packets
*       allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           (APPLICABLE RANGES: 1..31).
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           (APPLICABLE RANGES: 0..0xFFF) (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           (APPLICABLE RANGES: 0..0xFFFF) (16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_OUT_OF_RANGE - on pktLimit >= 0x10000 or windowSize >= 0x1000
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeRateLimiterTableSet
(
    IN GT_U8  devNum,
    IN GT_U32 rateLimiterIndex,
    IN GT_U32 windowSize,
    IN GT_U32 pktLimit
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               rateLimiterIndex,
                                               windowSize,
                                               pktLimit);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure CPU rate limiter entry attributes: window size and max packets
*       allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           (APPLICABLE RANGES: 1..31).
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           (APPLICABLE RANGES: 0..0xFFF) (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           (APPLICABLE RANGES: 0..0xFFFF) (16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_OUT_OF_RANGE - on pktLimit >= 0x10000 or windowSize >= 0x1000
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterTableSet
(
    IN GT_U8  devNum,
    IN GT_U32 rateLimiterIndex,
    IN GT_U32 windowSize,
    IN GT_U32 pktLimit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeRateLimiterTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rateLimiterIndex, windowSize, pktLimit));

    rc = internal_cpssDxChNetIfCpuCodeRateLimiterTableSet(devNum, rateLimiterIndex, windowSize, pktLimit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rateLimiterIndex, windowSize, pktLimit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       Get the Configured CPU rate limiter enrty attributes: window size
*       and max packets allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           (APPLICABLE RANGES: 1..31).
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                       of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeRateLimiterTableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *windowSizePtr,
    OUT GT_U32  *pktLimitPtr
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               rateLimiterIndex,
                                               windowSizePtr,
                                               pktLimitPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       Get the Configured CPU rate limiter enrty attributes: window size
*       and max packets allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           (APPLICABLE RANGES: 1..31).
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                       of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterTableGet
(
    IN  GT_U8   devNum,
    IN  GT_U32  rateLimiterIndex,
    OUT GT_U32  *windowSizePtr,
    OUT GT_U32  *pktLimitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeRateLimiterTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rateLimiterIndex, windowSizePtr, pktLimitPtr));

    rc = internal_cpssDxChNetIfCpuCodeRateLimiterTableGet(devNum, rateLimiterIndex, windowSizePtr, pktLimitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rateLimiterIndex, windowSizePtr, pktLimitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_OUT_OF_RANGE - on windowResolution too large(depends on system clock)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8  devNum,
    IN GT_U32 windowResolution
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               windowResolution);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_OUT_OF_RANGE - on windowResolution too large(depends on system clock)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8  devNum,
    IN GT_U32 windowResolution
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, windowResolution));

    rc = internal_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(devNum, windowResolution);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, windowResolution));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       windowResolutionPtr - (pointer to)The TO CPU window size resolution --
*                          this field is the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *windowResolutionPtr
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               windowResolutionPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       windowResolutionPtr - (pointer to)The TO CPU window size resolution --
*                          this field is the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_U8  devNum,
    OUT GT_U32 *windowResolutionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, windowResolutionPtr));

    rc = internal_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(devNum, windowResolutionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, windowResolutionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the cpu code rate limiter packet counter for specific
*       rate limiter index.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           (APPLICABLE RANGES: 1..31).
*
* OUTPUTS:
*       packetCntrPtr  - pointer to the number of packets forwarded to the CPU
*                       during the current window (set by
*                       cpssDxChNetIfCpuCodeRateLimiterTableSet)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter is reset to 0 when the window ends.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_U32              rateLimiterIndex,
    OUT  GT_U32              *packetCntrPtr
)
{

    return cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               rateLimiterIndex,
                                               packetCntrPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the cpu code rate limiter packet counter for specific
*       rate limiter index.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           (APPLICABLE RANGES: 1..31).
*
* OUTPUTS:
*       packetCntrPtr  - pointer to the number of packets forwarded to the CPU
*                       during the current window (set by
*                       cpssDxChNetIfCpuCodeRateLimiterTableSet)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter is reset to 0 when the window ends.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_U32              rateLimiterIndex,
    OUT  GT_U32              *packetCntrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, rateLimiterIndex, packetCntrPtr));

    rc = internal_cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(devNum, rateLimiterIndex, packetCntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, rateLimiterIndex, packetCntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*   get the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       dropCntrPtr  - (pointer to) The total number of dropped packets
*                           due to any of the CPU rate limiters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
(
    IN GT_U8                devNum,
    OUT GT_U32              *dropCntrPtr
)
{

    return cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               dropCntrPtr);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*   get the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       dropCntrPtr  - (pointer to) The total number of dropped packets
*                           due to any of the CPU rate limiters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
(
    IN GT_U8                devNum,
    OUT GT_U32              *dropCntrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeRateLimiterDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropCntrPtr));

    rc = internal_cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(devNum, dropCntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropCntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
*
* DESCRIPTION:
*   set the cpu code rate limiter drop counter to a specific value. This counter
*   counts the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - Device number.
*       dropCntrVal       - the value to be configured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
(
    IN GT_U8                devNum,
    IN GT_U32               dropCntrVal
)
{
    return cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(devNum,
                                               CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                               dropCntrVal);
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
*
* DESCRIPTION:
*   set the cpu code rate limiter drop counter to a specific value. This counter
*   counts the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - Device number.
*       dropCntrVal       - the value to be configured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
(
    IN GT_U8                devNum,
    IN GT_U32               dropCntrVal
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfCpuCodeRateLimiterDropCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dropCntrVal));

    rc = internal_cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(devNum, dropCntrVal);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dropCntrVal));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfFromCpuDpSet
*
* DESCRIPTION:
*       Set DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       dpLevel           - drop precedence level [Green, Red].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, dpLevel
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfFromCpuDpSet
(
    IN GT_U8               devNum,
    IN  CPSS_DP_LEVEL_ENT  dpLevel
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    PRV_CPSS_DXCH_COS_DP_TO_HW_CHECK_AND_CONVERT_MAC(
        devNum, dpLevel, regData);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).DSAConfig;
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.fastStack;
    }

    return prvCpssHwPpSetRegField(devNum, regAddr, 1, 2, regData);
}

/*******************************************************************************
* cpssDxChNetIfFromCpuDpSet
*
* DESCRIPTION:
*       Set DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       dpLevel           - drop precedence level [Green, Red].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, dpLevel
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfFromCpuDpSet
(
    IN GT_U8               devNum,
    IN  CPSS_DP_LEVEL_ENT  dpLevel
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfFromCpuDpSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dpLevel));

    rc = internal_cpssDxChNetIfFromCpuDpSet(devNum, dpLevel);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dpLevel));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfFromCpuDpGet
*
* DESCRIPTION:
*       Get DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       dpLevelPtr        - pointer to drop precedence level
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - invalid hardware value for drop precedence level
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfFromCpuDpGet
(
    IN  GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT  *dpLevelPtr
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_STATUS   rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    CPSS_NULL_PTR_CHECK_MAC(dpLevelPtr);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_TTI_MAC(devNum).DSAConfig;
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->pclRegs.fastStack;
    }

    rc = prvCpssHwPpGetRegField(devNum, regAddr, 1, 2, &regData);
    if(rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_COS_DP_TO_SW_CHECK_AND_CONVERT_MAC(
        devNum, regData, (*dpLevelPtr));

    return GT_OK;

}

/*******************************************************************************
* cpssDxChNetIfFromCpuDpGet
*
* DESCRIPTION:
*       Get DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*
* OUTPUTS:
*       dpLevelPtr        - pointer to drop precedence level
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - invalid hardware value for drop precedence level
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfFromCpuDpGet
(
    IN  GT_U8              devNum,
    OUT CPSS_DP_LEVEL_ENT  *dpLevelPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfFromCpuDpGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, dpLevelPtr));

    rc = internal_cpssDxChNetIfFromCpuDpGet(devNum, dpLevelPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, dpLevelPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaRxResourceErrorModeSet
*
* DESCRIPTION:
*       Set a bit per TC queue which defines the behavior in case of
*       RX resource error
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       queue      - traffic class queue (APPLICABLE RANGES: 0..7)
*       mode       - current packet mode: retry to send or abort
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, queue, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - the request is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaRxResourceErrorModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   queue,
    IN  CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT   mode
)
{
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32         portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queue);

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    /* Due to Cheetah 3 Errata - " FEr#2009: DSDMA resource errors may cause
       PCI Express packets reception malfunction"
       -- see PRV_CPSS_DXCH3_SDMA_WA_E */
    if (PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,PRV_CPSS_DXCH3_SDMA_WA_E) == GT_TRUE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE ||
        CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        /* get the register address of queue 0..7 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxSdmaResourceErrorCountAndMode[queue];

        switch (mode)
        {
            case CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E:
                regData = 2; /* set bit 9 reset bit 8 */
                break;
            case CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E:
                regData = 1; /* reset bit 9 set bit 8 */
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId, regAddr, 8, 2, regData);
    }
    else
    {
        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.sdmaCfgReg;

        switch (mode)
        {
            case CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E:
                regData = 0;
                break;
            case CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E:
                regData = 1;
                break;
            default:
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        /* relevant for bits 8:15 - so need to do shift of 8 bits */
        return prvCpssHwPpPortGroupSetRegField(devNum, portGroupId,regAddr, queue + 8, 1, regData);
    }

}

/*******************************************************************************
* cpssDxChNetIfSdmaRxResourceErrorModeSet
*
* DESCRIPTION:
*       Set a bit per TC queue which defines the behavior in case of
*       RX resource error
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       queue      - traffic class queue (APPLICABLE RANGES: 0..7)
*       mode       - current packet mode: retry to send or abort
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, queue, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - the request is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxResourceErrorModeSet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   queue,
    IN  CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT   mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaRxResourceErrorModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queue, mode));

    rc = internal_cpssDxChNetIfSdmaRxResourceErrorModeSet(devNum, queue, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queue, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfSdmaRxResourceErrorModeGet
*
* DESCRIPTION:
*       Get a bit per TC queue which defines the behavior in case of
*       RX resource error
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       queue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       modePtr       - current packet mode: retry to send or abort
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, queue, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - the request is not supported
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfSdmaRxResourceErrorModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   queue,
    OUT CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT   *modePtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      portGroupId;/* port group Id for multi-port-groups device support */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_DXCH_COS_CHECK_TC_MAC(queue);
    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* we will use the 'First active port group' , to represent the whole device.
       that way we allow application to give SDMA memory to single port group instead
       of split it between all active port groups
    */
    portGroupId = PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE ||
        CPSS_PP_FAMILY_DXCH_XCAT3_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.rxSdmaResourceErrorCountAndMode[queue];
        rc = prvCpssHwPpPortGroupReadRegister(devNum, portGroupId,
                                                 regAddr, &regData);
        if(rc != GT_OK)
            return rc;
        *modePtr = (U32_GET_FIELD_MAC(regData,9,1)) ? CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E : CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E;
    }
    else
    {

        regAddr =  PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->sdmaRegs.sdmaCfgReg;

        /* relevant for bits 8:15 - so need to do shift of 8 bits */
        rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId,regAddr, queue + 8, 1, &regData);

        *modePtr = (regData) ? CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ABORT_E : CPSS_DXCH_NET_RESOURCE_ERROR_MODE_RETRY_E;
    }

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxResourceErrorModeGet
*
* DESCRIPTION:
*       Get a bit per TC queue which defines the behavior in case of
*       RX resource error
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - device number.
*       queue      - traffic class queue (APPLICABLE RANGES: 0..7)
*
* OUTPUTS:
*       modePtr       - current packet mode: retry to send or abort
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, queue, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - the request is not supported
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfSdmaRxResourceErrorModeGet
(
    IN  GT_U8                                   devNum,
    IN  GT_U8                                   queue,
    OUT CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfSdmaRxResourceErrorModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, queue, modePtr));

    rc = internal_cpssDxChNetIfSdmaRxResourceErrorModeGet(devNum, queue, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, queue, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure CPU rate limiter entry attributes: window size and max packets
*       allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                       (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 1..31;
*                           Bobcat2, Caelum, Bobcat3 1..255)
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           (APPLICABLE RANGES: 0..0xFFF) (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during the configured windowSize.
*                           (APPLICABLE RANGES: 0..0xFFFF) (16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE - on pktLimit >= 0x10000 or windowSize >= 0x1000
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   for multi portgroup device, a rate limiter limits the rate of the incoming
*   traffic from a given port group. i.e. if a CPU code is configured to allow up
*   to 10 packets per window, it will actually allow up to 10 packets per window
*   from each portgroup.
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               rateLimiterIndex,
    IN GT_U32               windowSize,
    IN GT_U32               pktLimit
)
{
    GT_STATUS   rc;
    GT_U32      hwData; /* the HW format entry */
    GT_U32      regAddr;/* The register address to write to */
    GT_U32      portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32      entryIndex; /* entry index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(devNum,rateLimiterIndex);
    if(pktLimit >= BIT_16 || windowSize>= BIT_12)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_RANGE, LOG_ERROR_NO_MSG);
    }

    hwData = 0;
    U32_SET_FIELD_NO_CLEAR_MAC(hwData,0,16,pktLimit);
    U32_SET_FIELD_NO_CLEAR_MAC(hwData,16,12,windowSize);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            entryIndex = rateLimiterIndex;
        }
        else
        {   /* Bobcat2 A0 use index - 1 for memory access */
            entryIndex = rateLimiterIndex - 1;
        }

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            rc = prvCpssDxChPortGroupWriteTableEntry(devNum,portGroupId,
                                                PRV_CPSS_DXCH_LION3_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E,
                                                entryIndex,
                                                &hwData);
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterConfBase +
                  (rateLimiterIndex-1) * 4;

        /* loop on all active port groups in the bmp */
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
        {
            /* write rate limit entry */
            rc = prvCpssHwPpPortGroupWriteRegister(devNum,portGroupId,regAddr,hwData);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    }

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure CPU rate limiter entry attributes: window size and max packets
*       allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                       (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 1..31;
*                           Bobcat2, Caelum, Bobcat3 1..255)
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           (APPLICABLE RANGES: 0..0xFFF) (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during the configured windowSize.
*                           (APPLICABLE RANGES: 0..0xFFFF) (16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong parameter
*       GT_OUT_OF_RANGE - on pktLimit >= 0x10000 or windowSize >= 0x1000
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   for multi portgroup device, a rate limiter limits the rate of the incoming
*   traffic from a given port group. i.e. if a CPU code is configured to allow up
*   to 10 packets per window, it will actually allow up to 10 packets per window
*   from each portgroup.
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               rateLimiterIndex,
    IN GT_U32               windowSize,
    IN GT_U32               pktLimit
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, rateLimiterIndex, windowSize, pktLimit));

    rc = internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(devNum, portGroupsBmp, rateLimiterIndex, windowSize, pktLimit);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, rateLimiterIndex, windowSize, pktLimit));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       Get the Configured CPU rate limiter enrty attributes: window size
*       and max packets allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                       (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 1..31;
*                           Bobcat2, Caelum, Bobcat3 1..255)
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                        of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during the configured windowSize.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi portgroup device, a rate limiter limits the rate of the incoming
*   traffic from a given port group. i.e. if a CPU code is configured to allow up
*   to 10 packets per window, it will actually allow up to 10 packets per window
*   from each portgroup.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
(
    IN  GT_U8               devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN  GT_U32              rateLimiterIndex,
    OUT GT_U32              *windowSizePtr,
    OUT GT_U32              *pktLimitPtr
)
{
    GT_STATUS   rc;
    GT_U32      hwData; /* the HW format entry */
    GT_U32      regAddr;/* The register address to read from */
    GT_U32      portGroupId;/*the port group Id- support multi-port-groups device */
    GT_U32      entryIndex; /* entry index */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(devNum,rateLimiterIndex);
    CPSS_NULL_PTR_CHECK_MAC(windowSizePtr);
    CPSS_NULL_PTR_CHECK_MAC(pktLimitPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
        {
            entryIndex = rateLimiterIndex;
        }
        else
        {   /* Bobcat2 A0 use index - 1 for memory access */
            entryIndex = rateLimiterIndex - 1;
        }

        /* read rate limit entry */
        rc = prvCpssDxChPortGroupReadTableEntry(devNum,portGroupId,
                                            PRV_CPSS_DXCH_LION3_TABLE_EQ_TO_CPU_RATE_LIMITER_CONFIG_E,
                                            entryIndex,
                                            &hwData);
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterConfBase +
                  (rateLimiterIndex-1) * 4;

        /* read rate limit entry */
        rc = prvCpssHwPpPortGroupReadRegister(devNum,portGroupId,regAddr,&hwData);
    }

    *pktLimitPtr   = U32_GET_FIELD_MAC(hwData, 0,16);
    *windowSizePtr = U32_GET_FIELD_MAC(hwData,16,12);

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       Get the Configured CPU rate limiter enrty attributes: window size
*       and max packets allowed within that window.
*       cpssDxChNetIfCpuCodeTableSet() points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                       (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 1..31;
*                           Bobcat2, Caelum, Bobcat3 1..255)
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                        of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during the configured windowSize.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   for multi portgroup device, a rate limiter limits the rate of the incoming
*   traffic from a given port group. i.e. if a CPU code is configured to allow up
*   to 10 packets per window, it will actually allow up to 10 packets per window
*   from each portgroup.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet
(
    IN  GT_U8               devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN  GT_U32              rateLimiterIndex,
    OUT GT_U32              *windowSizePtr,
    OUT GT_U32              *pktLimitPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, rateLimiterIndex, windowSizePtr, pktLimitPtr));

    rc = internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(devNum, portGroupsBmp, rateLimiterIndex, windowSizePtr, pktLimitPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, rateLimiterIndex, windowSizePtr, pktLimitPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*                          field range: minimal value is 1. Maximal is 2047.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_OUT_OF_RANGE - on windowResolution too large(depends on system clock)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               windowResolution
)
{
    GT_STATUS   rc;
    GT_U32  regAddr;      /* The register address to read from.           */
    GT_U32  regData;      /* Data read/write from/to register.            */
    GT_U32  fieldResolution; /* resolution of field in the PP reg
                                   in nano seconds                        */
    GT_U32  hwWindowResolution;/* HW resolution is in steps of fieldResolution*/
    GT_U32  roundValue; /* round value is needed in order to calculate HW
                            window resolution in steps of field resolutiuon */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    GT_U32  systemClockCycles; /* window resolution step in system clock cycles */
    GT_BOOL toCpuRateLimitWindowResolutionErr; /* TO CPU rate limit window resolution errata flag */
    GT_U32  hwRateIncrement; /* 1 means that one clock is added by HW */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Rate limiter window size is:
           <Limiter<n> WindowSize>*(<ToCPU RLWindow Resolution>+1)*256 Core Clocks.
           Resolution is (<ToCPU RLWindow Resolution>+1)*256 Core Clocks */
        systemClockCycles = 256;
        hwRateIncrement = 1;
    }
    else
    {
        /* resolution is in steps of 32 core clock cycles with HW values mean:
            1 = 32 system clock cycles
            2 = 64 system clock cycles
            ....
            2047 = 65504 system clock cycles*/
        systemClockCycles = 32;
        hwRateIncrement = 0;
    }

    fieldResolution = (1000 * systemClockCycles) /  PRV_CPSS_PP_MAC(devNum)->coreClock;
    roundValue = fieldResolution / 2;

    /* The resolution is in steps of fieldResolution */
    hwWindowResolution = (windowResolution + roundValue) / fieldResolution;

    /* minimal value in the register is 1 and maximal is 11 bits */
    if ((hwWindowResolution == 0) || ((hwWindowResolution - hwRateIncrement) > BIT_11))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    hwWindowResolution -= hwRateIncrement;

    toCpuRateLimitWindowResolutionErr =
        PRV_CPSS_DXCH_ERRATA_GET_MAC(devNum,
        PRV_CPSS_DXCH_BOBCAT2_EQ_TO_CPU_RATE_LIMIT_WINDOW_RESOLUTION_WA_E);

    if (toCpuRateLimitWindowResolutionErr)
    {
        /* minimal rate limit windows resolution value is 5 */
        if (hwWindowResolution < 5)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    /* Get the address of the Pre Egress Engine register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    }

    regData = hwWindowResolution;

    /* loop on all active port groups in the bmp */
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)
    {
        /* set Window Resolution field in
           Pre-Egress Engine Global Configuration register */
        rc = prvCpssHwPpPortGroupSetRegField(devNum,portGroupId,regAddr,
                                                        0, 11, regData);
        if (GT_OK != rc)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_IN_BMP_MAC(devNum,portGroupsBmp,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*                          field range: minimal value is 1. Maximal is 2047.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_OUT_OF_RANGE - on windowResolution too large(depends on system clock)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               windowResolution
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, windowResolution));

    rc = internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(devNum, portGroupsBmp, windowResolution);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, windowResolution));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       windowResolutionPtr - (pointer to)The TO CPU window size resolution --
*                          this field is the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          field range: minimal value is 1. Maximal is 2047.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    OUT GT_U32               *windowResolutionPtr
)
{
    GT_STATUS   rc;
    GT_U32      regAddr;      /* The register address to read from.           */
    GT_U32      regData;      /* Data read/write from/to register.            */
    GT_U32      fieldResolution; /* resolution of field in the PP reg
                                   in the nano seconds                        */
    GT_U32      hwWindowResolution;/* HW resolution is in steps of fieldResolution*/
    GT_U32      portGroupId;/*the port group Id- support multi-port-groups device */
    GT_U32      systemClockCycles; /* window resolution step in system clock cycles */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(windowResolutionPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_GET_FIRST_ACTIVE_MAC(
        devNum, portGroupsBmp, portGroupId);

    /* Get the address of the Pre Egress Engine register */
    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            preEgrEngineGlobalConfig.preEgrEngineGlobalConfig;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.preEgrEngineGlobal;
    }

    /* get Window Resolution field in
       Pre-Egress Engine Global Configuration register */
    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                                    0, 11, &regData);

    hwWindowResolution = regData;

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* Rate limiter window size is:
           <Limiter<n> WindowSize>*(<ToCPU RLWindow Resolution>+1)*256 Core Clocks.
           Resolution is (<ToCPU RLWindow Resolution>+1)*256 Core Clocks */
        systemClockCycles = 256;
        hwWindowResolution++;
    }
    else
    {
        /* resolution is in steps of 32 core clock cycles with HW values mean:
            1 = 32 system clock cycles
            2 = 64 system clock cycles
            ....
            2047 = 65504 system clock cycles*/
        systemClockCycles = 32;
    }

    fieldResolution = (1000 * systemClockCycles) /  PRV_CPSS_PP_MAC(devNum)->coreClock;

    /* The resolution is in steps of fieldResolution */
    *windowResolutionPtr = hwWindowResolution * fieldResolution;

    return rc;
}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp - bitmap of Port Groups.
*                       NOTEs:
*                       1. for non multi-port groups device this parameter
*                            is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       windowResolutionPtr - (pointer to)The TO CPU window size resolution --
*                          this field is the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          field range: minimal value is 1. Maximal is 2047.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_U8                devNum,
    IN  GT_PORT_GROUPS_BMP   portGroupsBmp,
    OUT GT_U32               *windowResolutionPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, windowResolutionPtr));

    rc = internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(devNum, portGroupsBmp, windowResolutionPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, windowResolutionPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the cpu code rate limiter packet counter for specific
*       rate limiter index.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                       (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 1..31;
*                           Bobcat2, Caelum, Bobcat3 1..255)
*
* OUTPUTS:
*       packetCntrPtr  - pointer to the number of packets forwarded to the CPU
*                       during the current window (set by
*                       cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter is reset to 0 when the window ends.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN   GT_U32              rateLimiterIndex,
    OUT  GT_U32              *packetCntrPtr
)
{
    GT_U32      regAddr; /* The register address to read from */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPU_CODE_RATE_LIMITER_INDEX_AS_POINTER_CHECK_MAC(devNum,rateLimiterIndex);
    CPSS_NULL_PTR_CHECK_MAC(packetCntrPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            toCpuPktRateLimiters.toCpuRateLimiterPktCntr[rateLimiterIndex-1];
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterPktCntrBase +
                  ((rateLimiterIndex - 1) * 4);
    }

    return prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 16,
                                                  packetCntrPtr, NULL);

}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the cpu code rate limiter packet counter for specific
*       rate limiter index.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                       (APPLICABLE RANGES: DxCh2, DxCh3, xCat, xCat3, Lion, xCat2, Lion2 1..31;
*                           Bobcat2, Caelum, Bobcat3 1..255)
*
* OUTPUTS:
*       packetCntrPtr  - pointer to the number of packets forwarded to the CPU
*                       during the current window (set by
*                       cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter is reset to 0 when the window ends.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_PORT_GROUPS_BMP  portGroupsBmp,
    IN   GT_U32              rateLimiterIndex,
    OUT  GT_U32              *packetCntrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, rateLimiterIndex, packetCntrPtr));

    rc = internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(devNum, portGroupsBmp, rateLimiterIndex, packetCntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, rateLimiterIndex, packetCntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*   get the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
*
* OUTPUTS:
*       dropCntrPtr  - (pointer to) The total number of dropped packets
*                           due to any of the CPU rate limiters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    OUT GT_U32              *dropCntrPtr
)
{
    GT_U32      regAddr;/* The register address to read from */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

    CPSS_NULL_PTR_CHECK_MAC(dropCntrPtr);
    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    if(PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        regAddr = PRV_DXCH_REG1_UNIT_EQ_MAC(devNum).
            toCpuPktRateLimiters.CPUCodeRateLimiterDropCntr;
    }
    else
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterDropCntrReg;
    }

    return prvCpssPortGroupsBmpCounterSummary(devNum, portGroupsBmp,
                                                  regAddr, 0, 32,
                                                  dropCntrPtr, NULL);

}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*   get the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
*
* OUTPUTS:
*       dropCntrPtr  - (pointer to) The total number of dropped packets
*                           due to any of the CPU rate limiters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    OUT GT_U32              *dropCntrPtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, dropCntrPtr));

    rc = internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(devNum, portGroupsBmp, dropCntrPtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, dropCntrPtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}
/*******************************************************************************
* internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet
*
* DESCRIPTION:
*   set the cpu code rate limiter drop counter to a specific value. This counter
*   counts the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       dropCntrVal       - the value to be configured.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               dropCntrVal
)
{
    GT_U32  regAddr;/* The register address to write to */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E |
        CPSS_BOBCAT2_E | CPSS_CAELUM_E | CPSS_BOBCAT3_E);

    PRV_CPSS_MULTI_PORT_GROUPS_BMP_CHECK_MAC(devNum,portGroupsBmp);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.eqBlkCfgRegs.cpuRateLimiterDropCntrReg;

    return prvCpssPortGroupsBmpCounterSet(devNum, portGroupsBmp,
                                              regAddr, 0, 32, dropCntrVal);

}

/*******************************************************************************
* cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet
*
* DESCRIPTION:
*   set the cpu code rate limiter drop counter to a specific value. This counter
*   counts the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum            - Device number.
*       portGroupsBmp  - bitmap of Port Groups.
*              NOTEs:
*               1. for non multi-port groups device this parameter is IGNORED.
*               2. for multi-port groups device :
*                  bitmap must be set with at least one bit representing
*                  valid port group(s). If a bit of non valid port group
*                  is set then function returns GT_BAD_PARAM.
*                  value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       dropCntrVal       - the value to be configured.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet
(
    IN GT_U8                devNum,
    IN GT_PORT_GROUPS_BMP   portGroupsBmp,
    IN GT_U32               dropCntrVal
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, portGroupsBmp, dropCntrVal));

    rc = internal_cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(devNum, portGroupsBmp, dropCntrVal);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, portGroupsBmp, dropCntrVal));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}

/*******************************************************************************
* internal_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet
*
* DESCRIPTION:
*       Set which mode the 'TO_CPU' dsa tag field <Flow-ID/TT Offset> will use
*       'flowid' or 'ttOffset'.
*       related to CPSS_DXCH_NET_DSA_TO_CPU_STC::flowIdTtOffset
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number.
*       mode       - one of the 'flowid' or 'ttOffset' modes.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT   mode
)
{
    GT_STATUS   rc;                 /* return value */
    GT_U32      regAddr;            /* register address */
    GT_U32      value;              /* value into the register */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    switch(mode)
    {
        case CPSS_DXCH_NET_TO_CPU_FLOW_ID_MODE_E:
            value = 0;
            break;
        case CPSS_DXCH_NET_TO_CPU_TT_OFFSET_MODE_E:
            value = 1;
            break;
        default :
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);

    }

    /* write value to first register */
    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig1;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 9, 1, value);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* write value to second register */
    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKGlobalConfig;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 14, 1, value);
    return rc;
}

/*******************************************************************************
* cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet
*
* DESCRIPTION:
*       Set which mode the 'TO_CPU' dsa tag field <Flow-ID/TT Offset> will use
*       'flowid' or 'ttOffset'.
*       related to CPSS_DXCH_NET_DSA_TO_CPU_STC::flowIdTtOffset
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number.
*       mode       - one of the 'flowid' or 'ttOffset' modes.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet
(
    IN  GT_U8                                   devNum,
    IN  CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT   mode
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, mode));

    rc = internal_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeSet(devNum, mode);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, mode));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}


/*******************************************************************************
* internal_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet
*
* DESCRIPTION:
*       Get which mode the 'TO_CPU' dsa tag field <Flow-ID/TT Offset> will use
*       'flowid' or 'ttOffset'.
*       related to CPSS_DXCH_NET_DSA_TO_CPU_STC::flowIdTtOffset
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       modePtr    - (pointer to) one of the 'flowid' or 'ttOffset' modes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on non synch value between 2 relevant units.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS internal_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet
(
    IN  GT_U8                                   devNum,
    OUT  CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT   *modePtr
)
{
    GT_STATUS   rc;                 /* return value */
    GT_U32      regAddr;            /* register address */
    GT_U32      value1,value2;      /* values read from register(s) */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
        CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
         CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(modePtr);

    /* read value from first register */
    regAddr = PRV_DXCH_REG1_UNIT_HA_MAC(devNum).haGlobalConfig1;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 9, 1, &value1);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* read value from second register */
    regAddr = PRV_DXCH_REG1_UNIT_ERMRK_MAC(devNum).ERMRKGlobalConfig;
    rc = prvCpssHwPpGetRegField(devNum, regAddr, 14, 1, &value2);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(value1 != value2)
    {
        /* the HA and ERMRK are not synch */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    *modePtr = (value1 == 0) ?
            CPSS_DXCH_NET_TO_CPU_FLOW_ID_MODE_E :
            CPSS_DXCH_NET_TO_CPU_TT_OFFSET_MODE_E;

    return GT_OK;
}

/*******************************************************************************
* cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet
*
* DESCRIPTION:
*       Get which mode the 'TO_CPU' dsa tag field <Flow-ID/TT Offset> will use
*       'flowid' or 'ttOffset'.
*       related to CPSS_DXCH_NET_DSA_TO_CPU_STC::flowIdTtOffset
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       modePtr    - (pointer to) one of the 'flowid' or 'ttOffset' modes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on non synch value between 2 relevant units.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet
(
    IN  GT_U8                                   devNum,
    OUT  CPSS_DXCH_NET_TO_CPU_FLOW_ID_OR_TT_OFFSET_MODE_ENT   *modePtr
)
{
    GT_STATUS rc;
    CPSS_LOG_FUNC_VARIABLE_DECLARE_MAC(funcId, cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet);

    CPSS_API_LOCK_MAC(0,0);
    CPSS_LOG_API_ENTER_MAC((funcId, devNum, modePtr));

    rc = internal_cpssDxChNetIfToCpuFlowIdOrTtOffsetModeGet(devNum, modePtr);

    CPSS_LOG_API_EXIT_MAC(funcId, rc);
    CPSS_APP_SPECIFIC_CB_MAC((funcId, rc, devNum, modePtr));
    CPSS_API_UNLOCK_MAC(0,0);

    return rc;
}



/* debug function to allow print (dump) of the RAW DSA tags that got on the packet .
  regardless to CPSS parsing of the DSA tag */
void dxChNetIfDsaTagParse_DebugDumpEnable
(
    IN GT_BOOL  enableDumpOfDsa
)
{
    debug_dumpDsa = enableDumpOfDsa;
}

