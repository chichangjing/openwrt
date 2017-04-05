/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssGenNetworkIfDsaTag.h
*
*       define CPSS build and parsing of DSA tag
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*******************************************************************************/

#ifndef __prvCpssGenNetworkIfDsaTagh
#define __prvCpssGenNetworkIfDsaTagh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/
#include <cpss/generic/cpssTypes.h>


#define PRV_CPSS_DSA_TAG_PCKT_OFFSET_CNS        12
#define PRV_CPSS_VLAN_ETHR_TYPE_MSB_CNS         0x81
#define PRV_CPSS_VLAN_ETHR_TYPE_LSB_CNS         0x00

/* offset in original packet for un/tagged frames */
#define PRV_CPSS_BUFF_PCKT_UNTAGGED_OFFSET_CNS  12
#define PRV_CPSS_BUFF_PCKT_TAGGED_OFFSET_CNS    16

/*
 * typedef: enum PRV_CPSS_NET_DSA_TYPE_ENT
 *
 * Description: Enumeration of DSA tag types
 *
 * Enumerations:
 *    PRV_CPSS_NET_DSA_TYPE_REGULAR_E  - regular DSA tag (single word)
 *    PRV_CPSS_NET_DSA_TYPE_EXTENDED_E - extended DSA tag (two words)
 *
 */
typedef enum
{
    PRV_CPSS_NET_DSA_TYPE_REGULAR_E,
    PRV_CPSS_NET_DSA_TYPE_EXTENDED_E

}PRV_CPSS_NET_DSA_TYPE_ENT;


/*
 * typedef: enum PRV_CPSS_NET_DSA_CMD_ENT
 *
 * Description: Enumeration of DSA tag commands
 *
 * Enumerations:
 *    PRV_CPSS_NET_DSA_CMD_TO_CPU_E  - DSA command is "To CPU"
 *    PRV_CPSS_NET_DSA_CMD_FROM_CPU_E  - DSA command is "From CPU"
 *    PRV_CPSS_NET_DSA_CMD_TO_ANALYZER_E - DSA command is "To Analyzer"
 *    PRV_CPSS_NET_DSA_CMD_FORWARD_E  - DSA command is "Forward"
 *
 */
typedef enum
{
    PRV_CPSS_NET_DSA_CMD_TO_CPU_E = 0 ,
    PRV_CPSS_NET_DSA_CMD_FROM_CPU_E    ,
    PRV_CPSS_NET_DSA_CMD_TO_ANALYZER_E ,
    PRV_CPSS_NET_DSA_CMD_FORWARD_E
} PRV_CPSS_NET_DSA_CMD_ENT;




/*
 * typedef: structure PRV_CPSS_NET_DSA_PARAMS_STC
 *
 * Description: structure of DSA tag parameters
 *
 * Fields:
 *  numberOfWords - indicate the number of word to set in DSA tag.
 *                  (can be used to set only first word in extended DSA)
 *  vpt - the value vlan priority tag
 *  vid - the value of vlan id
 *  cfi - CFI bit of the vlan tag
 *  dsaCmd - the command of the DSA tag (forward/fromCpu/toCpu/toAnalyzer)
 *  dsaFormat - the specific DSA tag format
 *
 *  ******  FORWARD command specific fields ************
 *
 *      srcTagged - src packet received tagged
 *      srcDevNum - the device number on which packet was received
 *      srcIsTrunk - the packet was received from trunk
 *         source.srcTrunkId - src trunk id on which packet was received
 *                      NOTE : relevant when srcIsTrunk = GT_TRUE
 *         source.srcPortNum - src port num on which packet was received
 *                      NOTE : relevant when srcIsTrunk = GT_FALSE
 *
 *  ****** FROM_CPU command specific fields ************
 *
 *      dstTagged - packet need to be transmitted tagged
 *      tc        - the traffic class for the transmitted packet
 *      useVidx     - Use / Don't use the Vidx field for forwarding the packet.
 *      dest        - Packet destination:
 *         vidx     - the vidx group that packet will be sent to.
 *                    NOTE: relevant only when useVidx = GT_TRUE
 *         devPort.tgtDev - the target ExMx device that packet will be send
 *                    to .
 *                    NOTE: relevant only when useVidx = GT_FALSE
 *         devPort.tgtPort - the target port on ExMx device that packet will be
 *                    send to .
 *                    NOTE: relevant only when useVidx = GT_FALSE
 *
 *  ******  TO_ANALYZER command specific fields ************
 *
 *
 *  ******  TO_CPU command specific fields ************
 *
 *
 */
typedef struct{
    GT_U32                      numberOfWords;
    GT_U8                       vpt;
    GT_U16                      vid;
    GT_U8                       cfi;
    PRV_CPSS_NET_DSA_CMD_ENT    dsaCmd;
    union{
        struct{
            GT_BOOL     srcTagged;
            GT_U8       srcDevNum;
            GT_BOOL     srcIsTrunk;
            union
            {
                GT_TRUNK_ID srcTrunkId;
                GT_U8       srcPortNum;
            }source;
        }forward;

        struct{
            GT_BOOL     dstTagged;
            GT_U8       tc;

            GT_BOOL     useVidx;
            union
            {
                GT_U16      vidx;
                struct
                {
                    GT_U8   tgtDev;
                    GT_U8   tgtPort;
                }devPort;
            }dest;
        }fromCpu;

        struct{
            GT_U32  dummy;/* not implemented yet (not needed yet)*/
        }toAnalyzer;

        struct{
            GT_U32  dummy;/* not implemented yet (not needed yet)*/
        }toCpu;

    }dsaFormat;

}PRV_CPSS_NET_DSA_PARAMS_STC;

/*******************************************************************************
* prvCpssNetDsaTagBuild
*
* DESCRIPTION:
*       Builds the DSA Tag .
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; ExMxPm; ExMx.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       dsaTagType     - the type of DSA tag to build (regular/extended)
*       dsaParamsPtr  - (pointer to) dsa parameters.
*
* OUTPUTS:
*       dsaTagPtr - (pointer to) the DSA tag.
*
* RETURNS:
*       GT_OK                    - if successful, or
*       GT_NOT_IMPLEMENTED       - the request is not implemented yet.
*       GT_BAD_PARAM             - one of the parameters is not valid
*       GT_BAD_PTR               - one of the pointers is NULL
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS prvCpssNetDsaTagBuild
(
    IN PRV_CPSS_NET_DSA_TYPE_ENT    dsaTagType,
    IN PRV_CPSS_NET_DSA_PARAMS_STC  *dsaParamsPtr,
    OUT GT_U32                      *dsaTagPtr
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif   /* __prvCpssGenNetworkIfDsaTagh */


