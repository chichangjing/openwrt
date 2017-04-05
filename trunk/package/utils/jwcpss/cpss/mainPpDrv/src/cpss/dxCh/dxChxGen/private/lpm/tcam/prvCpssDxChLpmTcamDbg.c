/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmTcamDbg.c
*
* DESCRIPTION:
*       CPSS DXCH debug functions for TCAM based LPM
*
* FILE REVISION NUMBER:
*       $Revision: 47 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/private/routerTunnelTermTcam/prvCpssDxChRouterTunnelTermTcam.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamPatTrie.h>
#include <cpss/dxCh/dxChxGen/private/lpm/tcam/prvCpssDxChLpmTcamCommonTypes.h>

extern GT_VOID * prvCpssSlSearch
(
    IN GT_VOID        *ctrlPtr,
    IN GT_VOID        *dataPtr
);

/*******************************************************************************
* convertTokenToIndex
*
* DESCRIPTION:
*       Converts token to index of the entry. The index is relative to the first
*       line of the TCAM section managed by the TCAM manager.
*       The TCAM manager maintains array of entries information. The token is
*       implemented as a pointer to the element in the array that is
*       associated with the entry.
*
* INPUTS:
*       tcamManagerPtr        - the TCAM manager object
*       token                 - the token
*
* OUTPUTS:
*       indexPtr              - points to the index associated with the token
*
* RETURNS:
*       GT_OK                 - on success
*       GT_BAD_PTR            - on NULL pointer
*       GT_BAD_PARAM          - token is part of tokens free list or is reserved token
*       GT_OUT_OF_RANGE       - token is pointing outside the entries info array
*       GT_FAIL               - on error
*
* COMMENTS:
*       Note that token is array index + 1 as token value 0 represents no boundary
*       in allocation function.
*
*******************************************************************************/
extern  GT_STATUS convertTokenToIndex
(
    IN  void   *tcamManagerPtr,
    IN  GT_U32                          token,
    OUT GT_U32                          *indexPtr
);

/*
 * typedef: enum PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_ENT
 *
 * Description:
 *      Scan patricia trie codes
 *
 * Enumerations:
 *      PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN        - defines not valid token node.
 *      PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E            - scan trie fail code
 *      PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_INDEX_E       - bad tcam index associated with trie node
 *      PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_NODE_E        - trie node is not equal to trie node that token node pointed to.
 *
 * Comments:
 *      None
 */
typedef enum
{
    PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN = 0,
    PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E     = 0x7FFFFFF0,
    PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_INDEX_E,
    PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_NODE_E
}PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_ENT;

GT_BOOL printMcSrcTreeInProgress = GT_FALSE;
GT_U32  mcPrintSpaceNum = 0;
/*******************************************************************************
* IpLttRead
*
* DESCRIPTION:
*     This func reads  LTT data .
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - the device number
*       lttTtiRow           - router tti TCAM row
*       lttTtiColumn        - router tti TCAM column
* OUTPUTS:
*     dataPtr              - pointer to LTT data
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/

static GT_STATUS IpLttRead
(
    IN  GT_U8    devNum,
    IN  GT_U32   lttTtiRow,
    IN  GT_U32   lttTtiColumn,
    OUT GT_U32   *dataPtr
)
{
    GT_STATUS   retVal = GT_OK;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
         CPSS_CH1_E | CPSS_CH1_DIAMOND_E | CPSS_CH2_E);

    /* ltt size is the same as router TCAM size */
    switch (PRV_CPSS_PP_MAC(devNum)->devFamily)
    {
    default:
        if (lttTtiColumn >= 4)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        break;
    }
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);


    /* now get the 22 bits in the hw according the location of the entry in the line */
    retVal = prvCpssDxChReadTableEntryField(devNum,PRV_CPSS_DXCH3_LTT_TT_ACTION_E,
                                            lttTtiRow,lttTtiColumn,0,22,dataPtr);
    return (retVal);
}

/*******************************************************************************
* readIpv4Prefix
*
* DESCRIPTION:
*     This func reads ipv4 prefixes from physical router tcam .
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum              - the device number
*       row                 - router tti TCAM row
*       column              - router tti TCAM column
* OUTPUTS:
*     word0Ptr              - pointer to first word of TCAM key (hw format) per specific column
*     word1Ptr              - pointer to second word  of TCAM key (hw format) per specific column
*     mask0Ptr              - pointer to first word of TCAM mask (hw format)
*     mask1Ptr              - pointer to second  word of TCAM mask (hw format)
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - if pp family nor CHEETAH2 and nor CHEETAH3.
*
* COMMENTS:
*     none
*
*******************************************************************************/

static GT_STATUS readIpv4Prefix
(
       IN  GT_U8    devNum,
       IN  GT_U32   row,
       IN  GT_U32   column,
       OUT GT_U32  *word0Ptr,
       OUT GT_U32  *word1Ptr,
       OUT GT_U32  *mask0Ptr,
       OUT GT_U32  *mask1Ptr
)
{
       GT_BOOL                     valid;
       GT_U32  keyArr[6];           /* TCAM key in hw format            */
       GT_U32  maskArr[6];          /* TCAM mask in hw format           */
             /* TCAM action (rule) in hw format  */
       GT_U32  actionArr[PRV_CPSS_DXCH_ROUTER_TUNNEL_TERM_TCAM_MAX_LINE_ACTION_SIZE_CNS];
       GT_U32  validArr[5];         /* TCAM line valid bits             */
       GT_U32  compareModeArr[5];   /* TCAM line compare mode           */

       GT_U32  wordOffset, word0, word1, mask0, mask1;
       GT_STATUS retVal;

       /* check parameters */

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E);

       CPSS_NULL_PTR_CHECK_MAC(word0Ptr);
       CPSS_NULL_PTR_CHECK_MAC(word1Ptr);
       CPSS_NULL_PTR_CHECK_MAC(mask0Ptr);
       CPSS_NULL_PTR_CHECK_MAC(mask1Ptr);

        /* clear data */
       cpssOsMemSet(keyArr, 0, sizeof(GT_U32) * 6);
       cpssOsMemSet(maskArr, 0, sizeof(GT_U32) * 6);
       cpssOsMemSet(actionArr, 0, sizeof(GT_U32) * 5);
       cpssOsMemSet(validArr, 0, sizeof(GT_U32) * 5);

       /* read hw data */
       retVal = prvCpssDxChRouterTunnelTermTcamGetLine(devNum,CPSS_PORT_GROUP_UNAWARE_MODE_CNS,
                                                       row,&validArr[0],
                                                       &compareModeArr[0],
                                                       &keyArr[0],&maskArr[0],
                                                       &actionArr[0]);
       if (retVal != GT_OK)
               return retVal;


       /* entry holds valid IPv4 prefix if the followings applies:
            - the entry is valid
            - the compare mode of the entry is single compare mode */
       if ((validArr[column] == 1) && (compareModeArr[column] == 0))
           valid = GT_TRUE;
       else
           valid = GT_FALSE;
       if(valid != GT_TRUE)
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NO_SUCH, LOG_ERROR_NO_MSG);

       switch (column)
       {
       case 0:
       case 2:
               wordOffset = (column == 0) ? 0 : 3;
               word0 = keyArr[wordOffset];
               word1 = keyArr[wordOffset + 1] & 0xffff;
               mask0 = maskArr[wordOffset];
               mask1 = maskArr[wordOffset + 1] & 0xffff;
       break;

       case 1:
       case 3:
               wordOffset = (column == 1) ? 1 : 4;

               word0 = ((keyArr[wordOffset] & 0xffff0000)>>16) | ((keyArr[wordOffset+1] & 0xffff)<<16);
               word1 = (keyArr[wordOffset+1] & 0xffff0000)>>16;
               mask0 = ((maskArr[wordOffset] & 0xffff0000)>>16) | ((maskArr[wordOffset+1] & 0xffff)<<16);
               mask1 = (maskArr[wordOffset+1] & 0xffff0000)>>16;
       break;
       default:
               CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
       }

       *word0Ptr = word0;
       *word1Ptr = word1;
       *mask0Ptr = mask0;
       *mask1Ptr = mask1;

       return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmTcamDbgDumpRouteTcam
*
* DESCRIPTION:
*     This func makes physical router tcam scanning and prints its contents.
*
* APPLICABLE DEVICES:
*        DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*     dump   -  parameter for debugging purposes
*
* OUTPUTS:
*     None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on bad parameter.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_INITIALIZED       - if the driver was not initialized
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - on failure.
*
* COMMENTS:
*     None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbgDumpRouteTcam
(
    IN GT_BOOL dump
)
{
    GT_U8   devNum =0;
    GT_U32  routerTtiTcamRow;
    GT_U32  routerTtiTcamColumn;
    GT_U32  tcamData32 = 0, tcamData16 = 0, tcamMask32 = 0, tcamMask16 = 0, lttData;
    GT_U32  retVal = GT_OK;
    PRV_CPSS_DXCH_PP_CONFIG_FINE_TUNING_STC *fineTuningPtr = &PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning;
       if(dump == GT_FALSE)
               return GT_OK;

       for(routerTtiTcamColumn = 0; routerTtiTcamColumn < 4 ; routerTtiTcamColumn++)
       {
               for(routerTtiTcamRow = 0 ; routerTtiTcamRow < fineTuningPtr->tableSize.router ; routerTtiTcamRow ++)
               {
                       retVal = readIpv4Prefix(devNum,routerTtiTcamRow,routerTtiTcamColumn,
                                               &tcamData32,&tcamData16,&tcamMask32, &tcamMask16);
                       if( retVal == GT_NO_SUCH) /* Non valid entry */
                               continue;
                       if( retVal != GT_OK)
                               return retVal;

                       { /* Read LTT entry and print the information. */
                               retVal = IpLttRead(devNum,routerTtiTcamRow,routerTtiTcamColumn,&lttData);
                               if( retVal != GT_OK)
                                       return retVal;

                               cpssOsPrintf("R:%8d C:%8d    Key:0x%08X 0x%08X    Mask:0x%08X 0x%08X   LTT:0x%08X\n", routerTtiTcamRow,  routerTtiTcamColumn,
                                                 tcamData32, tcamData16,  tcamMask32,  tcamMask16,  lttData);
                       }

               }
       }
       return GT_OK;
}


static void patTriePrintRec
(
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *rootPtr,
    GT_U8                                    *ipAddr,
    GT_U32                                   prefixLen,
    IN CPSS_IP_PROTOCOL_STACK_ENT            protocol,
    IN CPSS_UNICAST_MULTICAST_ENT            prefixType,
    GT_U32                                   spaces,
    GT_U32                                   numOfDash
)
{
    GT_U8 uIp[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS],copyIp[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    GT_U32 i =0,addi = 0, size = 0, tcamIndex = 0;
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *validFather;
    GT_STATUS rc;
    GT_U32 maxPrefixLengthBytes;
    GT_U8 ipAddr2[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupNode = NULL;

    cpssOsMemSet(ipAddr2,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
    cpssOsMemSet(&uIp,0,16);
    prvCpssDxChLpmTcamPatTrieCopyIpAddr(ipAddr,rootPtr->ipAddr,0,rootPtr->prefixLen,prefixLen);
    prvCpssDxChLpmTcamPatTrieCopyIpAddr(uIp,ipAddr,0,(prefixLen+rootPtr->prefixLen),0);
    {
        if (printMcSrcTreeInProgress == GT_TRUE)
        {
            for (i = 0; i< mcPrintSpaceNum;i++)
            {
                cpssOsPrintf(" ");
            }
        }
        for (i = 0; i<(spaces*2);i++)
        {
            cpssOsPrintf(" ");
        }

        addi = 1;
        validFather = prvCpssDxChLpmTcamPatTrieGetValidFather(rootPtr);

        if(NULL != rootPtr->father)
        {
            if(rootPtr == rootPtr->father->leftSon)
            {
                cpssOsPrintf("%d.L", spaces);
            }
            else if(rootPtr == rootPtr->father->rightSon)
            {
                cpssOsPrintf("%d.R", spaces);
            }
            else
            {
                cpssOsPrintf(">>>");
            }
        }
        else
            cpssOsPrintf("root");
        cpssOsPrintf(" ");
        cpssOsPrintf("0x%.08X",rootPtr);

        cpssOsPrintf(" ");
        if (protocol == CPSS_IP_PROTOCOL_IPV4_E )
        {
            maxPrefixLengthBytes = 4;
        }
        else
        {
            maxPrefixLengthBytes = 16;
        }
        for (i = 0; i < maxPrefixLengthBytes; i++)
        {
            cpssOsPrintf("%.3d.",uIp[i]);
        }

        if (! ((rootPtr->father == NULL) && (rootPtr->pData == NULL) && (rootPtr->ipTcamShadowPtr == NULL)) )
        {
            rc = convertTokenToIndex( rootPtr->ipTcamShadowPtr->tcamManagerHandlerPtr,
                                      (GT_U32)(rootPtr->tcamAllocInfo.tokenNode->tcamManagerEntryToken),
                                      &tcamIndex);
            if(rc != GT_OK)
                return;
        }

        cpssOsPrintf("\\%d %.3d 0x%.08X 0x%.08X",
                     rootPtr->prefixLen+prefixLen,
                     rootPtr->size,
                     rootPtr->pData,
                     rootPtr->tcamAllocInfo.tokenNode);
        if (! ((rootPtr->father == NULL) && (rootPtr->pData == NULL) && (rootPtr->ipTcamShadowPtr == NULL)) )
        {
           if(NULL != rootPtr->tcamAllocInfo.tokenNode)
           {
               cpssOsPrintf(" %d:%d 0x%.08X\r\n",
                   rootPtr->tcamAllocInfo.tokenNode->tcamManagerEntryToken,
                   tcamIndex,
                   rootPtr->tcamAllocInfo.tokenNode->trieNode);
           }
        }
        if (validFather == NULL)
            if ( !((printMcSrcTreeInProgress == GT_TRUE) && (rootPtr->tcamAllocInfo.tokenNode != NULL)) )
            {
                 cpssOsPrintf("\r\n");
            }
    }
    cpssOsMemCpy(copyIp,ipAddr,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);


    if ( (prefixType == CPSS_MULTICAST_E) && (rootPtr->pData != NULL) )
    {
        /* print source tree */
        mcGroupNode = (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)(rootPtr->pData);
        if (mcGroupNode->ipMcSrcTrie != NULL)
        {
            printMcSrcTreeInProgress = GT_TRUE;
            mcPrintSpaceNum = spaces*2 + 5;
            cpssOsPrintf("================start src tree: %d===============\n", mcGroupNode->isAPointer2Def);
             patTriePrintRec(mcGroupNode->ipMcSrcTrie,ipAddr2,0,protocol,CPSS_UNICAST_E,0,2);
             cpssOsPrintf("===============end src tree======================\n");
             printMcSrcTreeInProgress = GT_TRUE;
             mcPrintSpaceNum = 0;
        }

    }

    if (rootPtr->rightSon != NULL)
    {
        if (rootPtr->size <= rootPtr->rightSon->size)
        {
            cpssOsPrintf("rightSize Error!\n");
        }
        patTriePrintRec(rootPtr->rightSon,copyIp,prefixLen+rootPtr->prefixLen,
                        protocol,prefixType,spaces+addi,numOfDash);
        size += rootPtr->rightSon->size;
    }

    if (rootPtr->leftSon != NULL)
    {
        if (rootPtr->size <= rootPtr->leftSon->size)
        {
            cpssOsPrintf("leftSize Error!\n");
        }
        patTriePrintRec(rootPtr->leftSon,ipAddr,prefixLen+rootPtr->prefixLen,
                        protocol,prefixType, spaces+addi,numOfDash);
        size += rootPtr->leftSon->size;
    }
    if (size+1 != rootPtr->size)
    {
        cpssOsPrintf("sizeMismatch!!!\n");
    }
}

/*******************************************************************************
* patTrieScan
*
* DESCRIPTION:
*     This func scans recursively Patricia Trie and print its contents
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       rootPtr             - pointer to the current trie node
*       protocol            - ip protocol
*       prefixType          - uc/mc prefix type
*
* OUTPUTS:
*       None
* RETURNS:
*       NoNe
* COMMENTS:
*     none
*
*******************************************************************************/
static void patTrieScan
(
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *rootPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT protocol,
    IN CPSS_UNICAST_MULTICAST_ENT prefixType
)
{
    GT_U8 ipAddr1[PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS];
    cpssOsMemSet(ipAddr1,0,PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_MAX_PREFIX_LEN_IN_BYTES_CNS);
    cpssOsPrintf("location   node address     IP Prefix and length   Size      pData    tokenNode   token:index  TokenNodePtr \n");
    cpssOsPrintf("\n\n");
    patTriePrintRec(rootPtr,ipAddr1,0,protocol,prefixType,0,2);
}

/*******************************************************************************
* prvCpssDxChLpmTcamDbgPatTriePrint
*
* DESCRIPTION:
*     This function prints Patricia trie contents.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*     lpmDbPtr    - the LPM DB information
*     vrId        - The virtual router identifier.
*     protocol    - ip protocol
*     prefixType  - uc/mc prefix type
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK                    - on success
*     GT_NOT_FOUND             - if vrId is not found
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbgPatTriePrint
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT            protocol,
    IN CPSS_UNICAST_MULTICAST_ENT            prefixType
)
{
    /* Pointer the 1st level bucket to which the prefix should be inserted.*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie = NULL;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;

    switch (protocol)
    {
        case CPSS_IP_PROTOCOL_IPV6_E:
            break;
        case CPSS_IP_PROTOCOL_IPV4_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    vrEntryTmp.vrId = vrId;

    vrEntryPtr = prvCpssSlSearch(lpmDbPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    switch (prefixType)
    {
        case CPSS_UNICAST_E:
            pRootTrie = vrEntryPtr->ipUcTrie[protocol];
            break;
        case CPSS_MULTICAST_E:
            pRootTrie = vrEntryPtr->ipMcTrie[protocol];
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (pRootTrie!=NULL)
    {
        patTrieScan(pRootTrie,protocol,prefixType);
    }
    else
    {
        cpssOsPrintf("prvCpssDxChLpmTcamDbgPatTriePrint - return OK since the trie is empty, prefixType=%d with protocol=%d is not supported in vrId=%d .\n",
                     prefixType,protocol,vrId);
    }

    return GT_OK;
}

/*******************************************************************************
* patTrieValidityCheck
*
* DESCRIPTION:
*      This performs a validity checks on the given patrie node.
*      a. left son first bit 0 , right son first bit 1
*      b. length > 0 apart from root , ipv4 < 32 , ipv6 < 128
*      c. every node which is not root has to have a father
*          root node has to have a father==NULL
*      d. non valid node has to have 2 sons
*      e. size has to be sum of two sons + 1 (make sure also when no sons), also count all valid
*          nodes and compare to counters as a total checks this should be done for the whole trie
*      f.  prefixFlagType need to check has only valid values
*      g. Compare Vrid in node to vrid which is checked.
*      h. for non valid node tokenNode value should be equal to one of his sons, and trie node
*          inside should point  to a valid trie node and different from current node, for valid node it
*          must be different from sons
*      i.  every mc valid node other than root need to have trie for src ip.
*
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId           - the virtual router id.
*       rootPtr        - pointer to the current trie node
*       protocol       - ip protocol
*       prefixType     - uc/mc prefix type
*       isRootNode     -  if the given rootPtr is the root of the tree GT_TRUE else GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_FOUND     - if vrId is not found
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_STATUS patTrieValidityCheck
(
    IN GT_U32                                   vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *rootPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocol,
    IN CPSS_UNICAST_MULTICAST_ENT               prefixType,
    IN GT_BOOL                                  isRootNode
)
{
    GT_U32 sonSize;
    PRV_CPSS_DXCH_LPM_TCAM_TOKEN_NODE_STC *RSonTokenNode=NULL,
                                         *LSonTokenNode=NULL;
    PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupNode=NULL;

    /* Node checks */
    /*   a. left son first bit 0 , right son first bit 1   */
    if(rootPtr->rightSon != NULL)
    {
        if((rootPtr->rightSon->ipAddr[0] & 0x80) == 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        /* Will be used later */
        RSonTokenNode = rootPtr->rightSon->tcamAllocInfo.tokenNode;
    }

    if(rootPtr->leftSon != NULL)
    {
        if((rootPtr->leftSon->ipAddr[0] & 0x80) != 0)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        /* Will be used later */
        LSonTokenNode = rootPtr->leftSon->tcamAllocInfo.tokenNode;
    }

    /*   b. length > 0 apart from root , ipv4 < 32 , ipv6 < 128 */
    if(rootPtr->prefixLen > 0)
    {
        if(rootPtr->prefixLen > 128)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        else if((protocol == CPSS_IP_PROTOCOL_IPV4_E) && (rootPtr->prefixLen > 32))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    else if(isRootNode != GT_TRUE)
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    /*   c. every node which is not root has to have a father */
    /*   root node  has to have a father==NULL */
    if(((isRootNode == GT_TRUE) && (rootPtr->father != NULL))||
       ((isRootNode == GT_FALSE) && (rootPtr->father == NULL)))
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    /*   d. non valid node has to have 2 sons */
    if((rootPtr->pData == NULL) &&
       ((rootPtr->rightSon == NULL) || (rootPtr->leftSon == NULL)))
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    /*   e. size has to be sum of two sons + 1 (make sure also when no sons), also count all valid
                   nodes and compare to counters as a total checks this should be done for the whole trie */
    sonSize = 0;
    if(rootPtr->rightSon != NULL)
        sonSize = rootPtr->rightSon->size;
    if(rootPtr->leftSon != NULL)
        sonSize += rootPtr->leftSon->size;
    if((GT_U32)(rootPtr->size) != sonSize + 1)
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    /*   f.  prefixFlagType need to check has only valid values */
    if((rootPtr->prefixFlagType != PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ADDR_E) &&
       (rootPtr->prefixFlagType != PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_PREFIX_FLAG_ECMP_E))
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    /*   g. Compare Vrid in node to vrid which is checked. */
    if(rootPtr->vrId != vrId)
       CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);

    /*   h. for non valid node tokenNode value should be equal to one of his sons, and trie node
              inside should point  to a valid trie node and different from current node, for valid node it
              must be different from sons */
    if(rootPtr->pData == NULL)
    {
        if((rootPtr->tcamAllocInfo.tokenNode != RSonTokenNode) &&
           (rootPtr->tcamAllocInfo.tokenNode != LSonTokenNode))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }
    else
    {
        if((rootPtr->tcamAllocInfo.tokenNode == RSonTokenNode) ||
           (rootPtr->tcamAllocInfo.tokenNode == LSonTokenNode))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /*   i.  every mc valid node other than root need to have trie for src ip.   */
    if((prefixType == CPSS_MULTICAST_E) && (rootPtr->pData != NULL) && (rootPtr->father != NULL))
    {
        mcGroupNode = rootPtr->pData;
        if(mcGroupNode->ipMcSrcTrie == NULL)
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    return GT_OK;
}

/*******************************************************************************
* patTrieScanValidityRec
*
* DESCRIPTION:
*     This func scans recursively Patricia Trie checking bounding
*     son of each trie node in the tree and the trie node itself .
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2, Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       vrId                - the virtual router id.
*       rootPtr             - pointer to the current trie node
*       protocol            - ip protocol
*       prefixType          - uc/mc prefix type
*       isRootNode          -  if the given rootPtr is the root of the tree GT_TRUE else GT_FALSE.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*        Tcam index of rootPtr  on success. Else
*      PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN    - if rootPtr has non valid token node.
*      PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E        - scan trie fail for rootPtr
*      PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_INDEX_E   - bad tcam index associated with trie node
*      PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_NODE_E    - trie node is not equal to trie node that token node pointed to.
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_U32 patTrieScanValidityRec
(
    IN GT_U32                                   vrId,
    IN PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *rootPtr,
    IN CPSS_IP_PROTOCOL_STACK_ENT               protocol,
    IN CPSS_UNICAST_MULTICAST_ENT               prefixType,
    IN GT_BOOL                                  isRootNode
)
{
   GT_U32 leftSonIndex, rightSonIndex, myIndex, srcTrieIndex;

   GT_STATUS rc;
   PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC *mcGroupNode = NULL;


   if (! ((rootPtr->father == NULL) && (rootPtr->pData == NULL) && (rootPtr->ipTcamShadowPtr == NULL)) )
   {
       rc = convertTokenToIndex( rootPtr->ipTcamShadowPtr->tcamManagerHandlerPtr,
                                 (GT_U32)(rootPtr->tcamAllocInfo.tokenNode->tcamManagerEntryToken),
                                 &myIndex);
       if(rc != GT_OK)
           return PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_INDEX_E;

       /* Add one so all indexes in this function
       bigger then PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN*/
       myIndex += 1;
   }
   else
   {
       /* rootPtr is a fake node used in ip mc src trie. In this case
          check if left and right sons are exist and check them independentely.
          No comparation would be done for left and right sones of ip mc src trie root
          and myIndex is not used here. */
       myIndex = 0;
   }

   /* in case of mc group trie for each valid node the src tree validity shaoud be checked */
   if ( (prefixType == CPSS_MULTICAST_E) && (rootPtr->pData != NULL) )
   {
       /* check source tree */
       mcGroupNode = (PRV_CPSS_DXCH_LPM_TCAM_MC_GROUP_SRC_NODE_STC*)(rootPtr->pData);
       if (mcGroupNode->ipMcSrcTrie != NULL)
       {
           prefixType = CPSS_UNICAST_E;
           srcTrieIndex = patTrieScanValidityRec(vrId, mcGroupNode->ipMcSrcTrie, protocol, prefixType, GT_FALSE);
           prefixType = CPSS_MULTICAST_E;
           if(srcTrieIndex >= PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E)
           {
               cpssOsPrintf("scan for ipMcSrcTrie %x of group node %x was failed\n",mcGroupNode->ipMcSrcTrie,rootPtr);
               return srcTrieIndex;
           }
       }
   }
   if (rootPtr->rightSon != NULL)
   {
       rightSonIndex = patTrieScanValidityRec(vrId, rootPtr->rightSon, protocol, prefixType, GT_FALSE);
       if(rightSonIndex >= PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E)
           return rightSonIndex;

       rightSonIndex += 1;/* Add one so all indexes in this function   bigger then PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN*/
   }
   else
   {
       rightSonIndex = PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN;
   }

   if (rootPtr->leftSon != NULL)
   {
       leftSonIndex = patTrieScanValidityRec(vrId, rootPtr->leftSon, protocol, prefixType, GT_FALSE);
       if(leftSonIndex >= PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E)
           return leftSonIndex;

       leftSonIndex += 1; /* Add one so all indexes in this function   bigger then PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN*/
   }
   else
   {
       leftSonIndex = PRV_CPSS_DXCH_LPM_TCAM_SCAN_NON_VALID_TOKEN;
   }

   if ((rootPtr->father == NULL) && (rootPtr->pData == NULL) && (rootPtr->ipTcamShadowPtr == NULL) )
   {
       /* if  rootPtr is a fake node used in ip mc src trie this check is finished */
       return myIndex;
   }

    /* Check bounding validity */
    if(rootPtr->pData != NULL)
    {
        if(myIndex < leftSonIndex)
        {
            cpssOsPrintf("patTrieScanValidityRec - myIndex %d leftt: %d\n", myIndex, leftSonIndex);
            return PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_INDEX_E;
        }
        if(myIndex < rightSonIndex)
        {
            cpssOsPrintf("patTrieScanValidityRec - myIndex %d right: %d\n", myIndex, rightSonIndex);
            return PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_INDEX_E;
        }
        if (rootPtr != rootPtr->tcamAllocInfo.tokenNode->trieNode )
        {
            cpssOsPrintf("patTrieScanValidityRec - trieNode = %x, tokenNode->trieNode =%x\n", rootPtr, rootPtr->tcamAllocInfo.tokenNode->trieNode);
            return PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_NODE_E;
        }
    }
    else
    {
        if( ((leftSonIndex > rightSonIndex) && (myIndex != leftSonIndex )) ||
            ((rightSonIndex > leftSonIndex) && (myIndex != rightSonIndex))    )
            return PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E;
    }

    /* Additional validity check on rootPtr */
    rc = patTrieValidityCheck(vrId, rootPtr, protocol, prefixType,isRootNode);
    if(rc != GT_OK)
        return PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_BAD_NODE_E;

   /* current node has good bounding son, return it's real index */
   return myIndex - 1;
}

/*******************************************************************************
* prvCpssDxChLpmTcamDbgPatTrieValidityCheck
*
* DESCRIPTION:
*   This function checks Patricia trie validity.
*
* APPLICABLE DEVICES:
*        xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       lpmDbPtr    - the LPM DB information
*       vrId        - The virtual router identifier.
*       protocol    - ip protocol
*       prefixType  - uc/mc prefix type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_STATE             - on bad state in patricia trie
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_STATUS prvCpssDxChLpmTcamDbgPatTrieValidityCheck
(
    IN PRV_CPSS_DXCH_LPM_TCAM_SHADOW_STC     *lpmDbPtr,
    IN GT_U32                                vrId,
    IN CPSS_IP_PROTOCOL_STACK_ENT            protocol,
    IN CPSS_UNICAST_MULTICAST_ENT            prefixType
)
{
    GT_U32   scanRet;

    /* Pointer the 1st level bucket to which the prefix should be inserted.*/
    PRV_CPSS_DXCH_LPM_TCAM_PAT_TRIE_NODE_STC *pRootTrie;

    PRV_CPSS_DXCH_LPM_TCAM_VR_TBL_BLOCK_STC *vrEntryPtr,vrEntryTmp;

    switch (protocol)
    {
        case CPSS_IP_PROTOCOL_IPV6_E:
            break;
        case CPSS_IP_PROTOCOL_IPV4_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    vrEntryTmp.vrId = vrId;

    vrEntryPtr = prvCpssSlSearch(lpmDbPtr->vrSl,&vrEntryTmp);
    if (vrEntryPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    switch (prefixType)
    {
    case CPSS_UNICAST_E:
        pRootTrie = vrEntryPtr->ipUcTrie[protocol];

        break;
    case CPSS_MULTICAST_E:
        pRootTrie = vrEntryPtr->ipMcTrie[protocol];
        break;
    default:
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (pRootTrie!=NULL)
    {
        scanRet = patTrieScanValidityRec(vrId, pRootTrie, protocol, prefixType, GT_TRUE);
        if(scanRet >= PRV_CPSS_DXCH_LPM_TCAM_SCAN_TRIE_FAIL_E)
        {
            cpssOsPrintf("prvCpssDxChLpmTcamDbgPatTrieValidityCheck - Error!\n");
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }
    }
    else
    {
        cpssOsPrintf("prvCpssDxChLpmTcamDbgPatTrieValidityCheck - return OK since the trie is empty, prefixType=%d with protocol=%d is not supported in vrId=%d .\n",
                     prefixType,protocol,vrId);
    }
    return GT_OK;
}

