/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChLpmRamDbg.c
*
* DESCRIPTION:
*       the CPSS LPM Debug Engine support.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTypes.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRam.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamTrie.h>
#include <cpssCommon/private/prvCpssMath.h>
#include <cpssCommon/private/prvCpssDevMemManager.h>
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamUc.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamMc.h>
#include <cpssCommon/private/prvCpssSkipList.h>
#include <cpss/dxCh/dxChxGen/private/lpm/ram/prvCpssDxChLpmRamDbg.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/private/lpm/hw/prvCpssDxChLpmHw.h>
#include <cpss/dxCh/dxChxGen/private/lpm/prvCpssDxChLpmUtils.h>


#define LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure,retVal,entity,addr,message) \
{                                                                              \
    cpssOsPrintf("Error on %s address 0x%p: %s\n", entity, addr, message);     \
    retVal = GT_FAIL;                                                          \
    if (returnOnFailure == GT_TRUE)                                            \
    {                                                                          \
        return retVal;                                                         \
    }                                                                          \
}

#define LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure,retVal,entity,addr,hwAddr,message)           \
{                                                                                                            \
    cpssOsPrintf("Error on %s shadow address 0x%p, HW node address 0x%p: %s\n", entity, addr,hwAddr,message);\
    retVal = GT_FAIL;                                                                                        \
    if (returnOnFailure == GT_TRUE)                                                                          \
    {                                                                                                        \
        return retVal;                                                                                       \
    }                                                                                                        \
}


static GT_U8 baseAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS] = {0};
static GT_U8 baseGroupAddr[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS] = {0};
static GT_U32 grpPrefix = 0;
static GT_BOOL InSrcTree = GT_FALSE;
static GT_U8 *baseGroupAddrPtr = NULL;
static GT_U32 numOfErrors = 0;

typedef struct
{
     GT_U32 lpmLinesPerOctet[2*PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];   /* count the total number of lpm lines per octet */
     GT_U32 bucketPerOctet[2*PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];     /* count total number of buckets per octet */
     GT_U32 bucketPerType[PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E];       /* count total number of buckets per bucket type */
     GT_U32 bucketSizePerType[4];   /* count total buckets size per bucket type */
     GT_U32 bucketTypePerOctet[2*PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS][PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E]; /* count total bucket per octect per type */
     GT_U32 lpmLinesUsed[PRV_CPSS_DXCH_LPM_RAM_NUM_OF_MEMORIES_CNS];  /* LPM lines used in each LPM block by specific virtual router */
     GT_U32 numOfNonDefaultNextHopPointers;  /* number of pointers to route entry with base >= 3 (route entries 0-2 typicaly used for defaults) */
     GT_BOOL printRanges;

}prvLpmDumpPrintInfo_STC;

/* lpm db skip list ptr */
extern GT_VOID *prvCpssDxChIplpmDbSL;

extern GT_VOID * prvCpssSlSearch
(
    IN GT_VOID        *ctrlPtr,
    IN GT_VOID        *dataPtr
);

/* index of RAM (0 to 3) to use with a IPv4 UC/MC LPM engine */
CPSS_TBD_BOOKMARK /* the memory mapping will be decided by the application */
static GT_U8 addrLookupMemIpv4Array[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS] = {0, 1, 2, 3};

/* index of RAM (0 to 15) to use with a IPv6 UC/MC LPM engine */
CPSS_TBD_BOOKMARK /* the memory mapping will be decided by the application */
static GT_U8 addrLookupMemIpv6Array[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS] = {4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

/* index of RAM (0 to 2) to use with a FCoE LPM engine */
CPSS_TBD_BOOKMARK /* the memory mapping will be decided by the application */
static GT_U8 addrLookupMemFcoeArray[PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS] = {0, 0, 0};

/* max depth according to type: 4 for IPv4, 16 for IPv6, 3 for FCoE */
#define PRV_DXCH_LPM_RAM_DBG_MAX_DEPTH_CHECK_MAC(protocol)   \
    ((GT_U32)(((protocol) == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ? PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS:  \
             (((protocol) == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) ? PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS : \
                                                                  PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS)))

/* forward declaration */
static GT_STATUS prvCpssDxChLpmRamDbgBucketShadowValidityCheck
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  GT_U32                                    vrId,
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  GT_U8                                     level,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT       expectedNextPointerType,
    IN  GT_U8                                     numOfMaxAllowedLevels,
    IN  CPSS_UNICAST_MULTICAST_ENT                prefixType,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol,
    IN  GT_U32                                    expectedPointingRangeMemAddr,
    IN  GT_BOOL                                   returnOnFailure
);
/* forward declaration */
static GT_STATUS prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck
(
    IN  GT_U8                                     devNum,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  GT_U32                                    vrId,
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  GT_U8                                     level,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT       expectedNextPointerType,
    IN  GT_U8                                     numOfMaxAllowedLevels,
    IN  CPSS_UNICAST_MULTICAST_ENT                prefixType,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol,
    IN  GT_U32                                    expectedPointingRangeMemAddr,
    IN  GT_BOOL                                   returnOnFailure,
    IN  GT_BOOL                                   isRootBucket
);

static GT_STATUS outputAddress(GT_U8 *address, GT_U32 bytes2Print, PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocol)
{
    while(bytes2Print > 1)
    {
        if ((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ||
            (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E))
        {
            cpssOsPrintf("%03d.%03d.", *address, *(address+1));
        }
        else /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
        {
            cpssOsPrintf("%02X%02X:", *address, *(address+1));
        }

        address+=2;
        bytes2Print-=2;
    }

    if (bytes2Print > 0)
    {
        if ((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ||
            (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E))
        {
            cpssOsPrintf("%03d", *address);
        }
        else /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
        {
            cpssOsPrintf("%02X", *address);
        }
    }

    return GT_OK;
}

void prefixBase2Address(GT_U8 *baseAddress, GT_U32 prefixBase, GT_U32 notZeroMask,
                     GT_U8 *address)
{
    GT_U8 i;
    if(notZeroMask == 0)  /* The mask is zero */
        prefixBase += 8;

    for (i = 0; i < PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS; i++)
    {
        if ((GT_U32)((i+1)*8) <= prefixBase)
        {
            address[i] = baseAddress[i];
        }
        else
        {
            address[i] = 0;
        }
    }
}

GT_U32 getBaseAddrFromHW(CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT type,
                         GT_U32 index,
                         GT_U32 *hwData,GT_U32 bucketPtr)
{
    GT_U32 baseAddr = 0;
    GT_U32 i,hwIdx,tmp;

    switch(type)
    {
    case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
        if(index == 0)
            return 0;
        baseAddr = (hwData[0] >> (8 * (index - 1))) & 0xFF;
        break;

    case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
        if(index == 0)
            return 0;
        if(index == 5)
            return ((bucketPtr >> 5) & 0xFF);
        hwIdx = (index < 5)? index : (index -5);
        if(index > 5)
            baseAddr = (hwData[1] >> (8 * (hwIdx - 1))) & 0xFF;
        else
            baseAddr = (hwData[0] >> (8 * (hwIdx - 1))) & 0xFF;
        break;

    case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
        i = 0;
        hwIdx = 0;

        while(i <= index)
        {
            baseAddr = 0;
            tmp = hwData[hwIdx] & 0xFFFFFF;
            while((tmp != 0) && (i <= index))
            {
                if((tmp & 0x1) != 0)
                {
                    i++;
                }
                baseAddr++;
                tmp = (tmp >> 1) & 0xFFFFFF;
            }
            hwIdx++;
        }

        hwIdx--;
        baseAddr += (hwIdx * 24) - 1;
        break;

    default:
        break;
    }
    return baseAddr;
}

GT_U32 getEntryByPrefixFromHW(GT_U8 octet,CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT type,
                              GT_U32 *hwData, GT_U32 bucketPtr)
{
    GT_U32 baseAddr = 0;
    GT_U32 i,counter,prevCount,hwIdx,tmp,fromWhere;
    GT_U32 lastAddr = 0;
    hwIdx = 0;

    switch(type)
    {
    case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
        for (counter = 0; counter < 4; counter++)
        {
            baseAddr = (hwData[0] >> (8 * counter)) & 0xFF;
            if ((baseAddr > octet) || (lastAddr >= baseAddr))
            {
                break;
            }
            else
            {
                lastAddr = baseAddr;
            }
        }

        hwIdx = counter + 1;
        break;

    case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
        baseAddr = ((bucketPtr >> 0x5) & 0xFF);
        lastAddr = baseAddr;
        if (baseAddr > octet)
        {
            prevCount = 1;
            lastAddr = 0;
            tmp = hwData[0];
        }
        else
        {
            prevCount = 6;
            tmp = hwData[1];
        }

        for (counter = 0; counter < 4; counter++)
        {
            baseAddr = (tmp >> (8 * counter)) & 0xFF;

            if ((baseAddr > octet) || (lastAddr >= baseAddr))
            {
                break;
            }
            else
            {
                lastAddr = baseAddr;
            }
        }

        hwIdx = counter + prevCount;
        break;

    case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
        counter = 0;
        prevCount = hwData[octet / 24] >> 24;
        tmp = hwData[octet / 24] ;
        fromWhere = octet%24;
        for (i = 0; i <= fromWhere; i++)
        {
            counter += (tmp & 0x01);
            tmp >>= 1;
        }

        hwIdx = prevCount + counter;

        /* note that the bit vector counter (8 msb) contains the offset of the
           range (in lines) from current bit vector word; refer to the function
           getMirrorBucketDataAndUpdateRangesAddress for details */
        hwIdx += (octet / 24);
        hwIdx -= 10;

        break;

    default:
        break;
    }

    return hwIdx;
}

GT_U32 getNumOfRangesFromHW(CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT type,GT_U32 *hwData, GT_U32 bucketPtr)
{
    return getEntryByPrefixFromHW(255, type, hwData, bucketPtr);
}

GT_VOID printLpmMcGroup(PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocol,
                        GT_U8 prefixLength,
                        GT_U8 startAddr,
                        GT_U8 endAddr)
{
    GT_U8  startAddress[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U8  endAddress[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U8  address[PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U8  j;
    GT_U32 bytesToPrint;

    cpssOsMemSet(endAddress,0, sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
    cpssOsMemSet(startAddress,0, sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
    cpssOsMemSet(address,0, sizeof(GT_U8) * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);

    /* print group */
    cpssOsPrintf(" Group :  ");
    prefixBase2Address(baseAddr ,prefixLength,1,address);
    for (j = 8; j < prefixLength; j += 8)
    {
        startAddress[(j/8)-1] = address[(j/8)-1];
        endAddress[(j/8)-1] = address[(j/8)-1];
    }
    startAddress[(j/8)-1] = startAddr;
    endAddress[(j/8)-1] = (GT_U8)endAddr;
    if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
    {
        bytesToPrint = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
    }
    else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
    {
        bytesToPrint = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
    }
    outputAddress(startAddress, bytesToPrint, protocol);
    cpssOsPrintf(" - ");
    outputAddress(endAddress, bytesToPrint, protocol);
    cpssOsPrintf(" / %02d\n ", prefixLength);
    cpssOsPrintf(" Src tree for this group :\n");

}

GT_STATUS validateBitVectorOfRegularBucket(GT_U32 *bucketDataPtr, GT_U32 hwAddr, GT_U32 basePrefix)
{
    GT_U32 hwBucketAddr;
    GT_U32 bitSum;
    GT_U32 bitsArray;
    GT_U32 sum;
    GT_U32 i;

    hwBucketAddr = bucketDataPtr[0]; /* bitmap pointer */
    bitSum = (hwBucketAddr >> 24) & 0xFF; /* summry of bits in privious array */
    /* For first word, bitsum must be 0 */
    if (bitSum != 0xA)
    {
        cpssOsPrintf("\nFor first word, sum must be 0!!! Bucket address 0x%x\n",hwAddr);
        cpssOsFree(bucketDataPtr);
        numOfErrors++;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* continue to other branches in the tree */
    }
    /* Bit 0 in word 0 must be set because first range always starts in 0 */
    if ((hwBucketAddr & 0x1) == 0)
    {
        cpssOsPrintf("\nFirst range always starts in 0!!! Bucket address 0x%x\n",hwAddr);
        cpssOsFree(bucketDataPtr);
        numOfErrors++;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* continue to other branches in the tree */
    }

    for (i = 0; i < 10; i++)
    {
        hwBucketAddr = bucketDataPtr[i*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS];
        bitsArray = hwBucketAddr & 0xFFFFFF;
        bitSum = (bucketDataPtr[(i+1)*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS] >> 24) & 0xFF;
        sum = 0; /* number of set bits from 24 bits array */
        while (bitsArray != 0)
        {
            if (bitsArray & 0x1)
            {
                sum++; /* count number of set bits in array */
            }
            bitsArray = bitsArray >> 1;
        }
        sum += (((bucketDataPtr[i*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS] >> 24) & 0xFF) - 1);

        /* For each word in the bitmap other than first
           Sum = previous_sum + number of bits in previous word */
        if (sum != bitSum)
        {
            cpssOsPrintf("\nTotal sum is different then total number of set bits!!! Bucket address 0x%x\n",hwAddr);
            cpssOsFree(bucketDataPtr);
            numOfErrors++;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* continue to other branches in the tree */
        }
    }
    hwBucketAddr = bucketDataPtr[10*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]; /* 11'th range of bitmap */
    /* On last word, last 8 bits must be 0 (those do not count as 11*24) */
    if (((hwBucketAddr >> 16) & 0xFF) != 0)
    {
        cpssOsPrintf("\nOn last word, last 8 bits must be 0 !!! Bucket address 0x%x\n",hwAddr);
        cpssOsFree(bucketDataPtr);
        numOfErrors++;
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* continue to other branches in the tree */
    }
    bitsArray = hwBucketAddr & 0xFFFFFF;
    sum = 0;
    while (bitsArray != 0)
    {
        sum++; /* count number of set bits in array */
        bitsArray = bitsArray >> 1;
    }
    bitSum += sum;
    /* Regular bucket is for 11-255 ranges so there must be at least 11 bits != 0
       exception here for root bucket, which can be regular or compressed 1.
       If it regular then it will have ranges 6-255 -> less then 11 bits */
    if (bitSum < 11)
    {
        if (!((InSrcTree == GT_FALSE) && (basePrefix == 8)))
        {
            cpssOsPrintf("\nAt least 6 ranges should be defined!!! Bucket address 0x%x\n",hwAddr);
            cpssOsFree(bucketDataPtr);
            numOfErrors++;
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG); /* continue to other branches in the tree */
        }
    }
    return GT_OK;
}




/*************************************************************************/
GT_STATUS printLpm(GT_U8 devNum, GT_U32 bucketPtr, GT_U8 *baseAddress,
                   GT_U32 basePrefix, GT_U8 startAddress, GT_U32 endAddress,
                   GT_BOOL isUc, PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocol,
                   GT_U32 depth, prvLpmDumpPrintInfo_STC *printInfo)
{
    GT_U32 *bucketDataPtr;

    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT bucketType = CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E;
    GT_U32 bitMapSize = 0;
    GT_U32 hwAddr = 0;      /* in lines */
    GT_U32 blockIndex = 0;
    GT_STATUS ret = GT_OK;

    GT_U8  addr[2*PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS] = {0};
    GT_U8  prefix = 0;
    GT_U32 basePrefixLength = 0;
    GT_U32 hwBucketAddr = 0;
    GT_U32 i = 0,j = 0;
    GT_U32 numOfRanges = 0;
    GT_U8 startAddr[2*PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U8 endAddr[2*PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS];
    GT_U8 *baseAddrPtr = NULL;
    GT_U32 routeEntriesNum = 0;
    GT_U32 nhPointer = 0;
    GT_U32 rangeStart[10] = {0}; /* Bitmap has 9 bytes;
                              Word 1: [range_4_start ; range_3_start ; range_2_start ; range_1_start]
                              Word 2: [range_9_start ; range_8_start ; range_7_start ; range_6_start]
                                - range_5_start is carried from the pointer to this bucket
                                - range_0_start is always 0 (so no need to add to bitmap) */
    GT_BOOL rangeStartIsZero = GT_FALSE; /* After first range x start is 0, all ranges after must be 0 */
    GT_U32 nextBucketType = 0;
    GT_U32 pointerType = 0;
    PRV_CPSS_DXCH_LPM_ECMP_ENTRY_STC ecmpEntry;

    cpssOsMemSet(&ecmpEntry,0,sizeof(ecmpEntry));
    numOfRanges = 0;
    nextBucketType = (bucketPtr >> 3) & 0x3;
    pointerType = bucketPtr & 0x3;
    if (pointerType == 0x0)
    {
        if (nextBucketType == 0x3)
        {
            cpssOsPrintf("\nBucket Type - Invalid value exists!!! Bucket Type %d\n",bucketPtr);
            numOfErrors++;
            return GT_OK; /* continue to other branches in the tree */
        }
        bucketType = nextBucketType;
    }
    else if (pointerType == 0x1)
    {
        bucketType = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E;
    }
    else if (pointerType == 0x2)
    {
        bucketType = CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E;
    }
    else /* pointerType == 0x3 */
    {
        bucketType = CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E;
    }

    /* Allocate memory for bucket data. Maximum size is regular bucket that
       uses 256 lpm lines for ranges + 11 lpm lines for bitmap */
    bucketDataPtr = cpssOsMalloc(PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS *
                                 PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS);
    if (bucketDataPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    cpssOsMemSet(bucketDataPtr, 0,
                    PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS *
                            PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_BYTES_CNS);

    basePrefixLength = basePrefix + 8;
    baseAddrPtr = baseAddress + 1;
    if(bucketType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
    {
        bitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
        /* hw pointer in LPM entry is in LPM lines*/
        hwAddr = ((bucketPtr & 0x1FFFFFE0) >> 5);
    }
    else if(bucketType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
    {
        bitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS;
        hwAddr = (bucketPtr >> 13);
    }
    else if(bucketType == CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E)
    {
        bitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;
        hwAddr = ((bucketPtr & 0x1FFFFFE0) >> 5);
    }
    else /* bucketType = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E or
                         CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E or
                         CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E */
    {
        bitMapSize = 0;
        hwAddr = 0;
    }

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* the hwAddr is a relative address to the beginning of the LPM */
        blockIndex = hwAddr/PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.totalNumOfLinesInBlockIncludingGap;
    }
    else
    {
        /* find in which LPM bucket located */
        if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
        {
            blockIndex = addrLookupMemIpv4Array[basePrefix/8];
        }
        else if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
        {
            blockIndex = addrLookupMemIpv6Array[basePrefix/8];
        }
        else if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E)
        {
            blockIndex = addrLookupMemFcoeArray[basePrefix/8];
        }
        else
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    if ((bucketType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
        (bucketType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
        (bucketType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
    {
        /* this is a bucket */
        printInfo->bucketPerType[bucketType]++;
        printInfo->bucketPerOctet[depth]++;
        printInfo->bucketTypePerOctet[depth][bucketType]++;
        if (printInfo->printRanges)
        {
            cpssOsPrintf (" %X \n",hwAddr);
        }

        /* read the bit vector */
        ret = prvCpssDxChReadTableMultiEntry(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                             hwAddr, bitMapSize, bucketDataPtr);
        if (ret != GT_OK)
        {
            cpssOsFree(bucketDataPtr);
            return ret;
        }
        numOfRanges = getNumOfRangesFromHW(bucketType,bucketDataPtr,bucketPtr);

        /* read the ranges (the LPM lines after the bit vector) */
        ret = prvCpssDxChReadTableMultiEntry(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                             hwAddr + bitMapSize, numOfRanges,
                                             &bucketDataPtr[bitMapSize*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]);
        if (ret != GT_OK)
        {
            cpssOsFree(bucketDataPtr);
            return ret;
        }

        if (isUc)
        {
            /* IPv4 UC / IPv6 UC/ FCoE Max tree depth is 4/16/3 accordingly */
            if (depth > PRV_DXCH_LPM_RAM_DBG_MAX_DEPTH_CHECK_MAC(protocol))
            {
                cpssOsPrintf("\nUC Max tree depth is %d!!! Bucket address 0x%x\n",PRV_DXCH_LPM_RAM_DBG_MAX_DEPTH_CHECK_MAC(protocol),hwAddr);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
            /* From depth 4/16/3 (according to type) all LPM lines must be NH/ECMP/QoS bucket type */
            else if (depth == PRV_DXCH_LPM_RAM_DBG_MAX_DEPTH_CHECK_MAC(protocol))
            {
                cpssOsPrintf("\nFrom depth %d all LPM lines must be NH, ECMP or QoS bucket type!!! Bucket address 0x%x\n",PRV_DXCH_LPM_RAM_DBG_MAX_DEPTH_CHECK_MAC(protocol),hwAddr);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
        }
        else /* multicast */
        {
            if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
            {
                if (InSrcTree == GT_FALSE)
                {
                    /* Multicast group tree */
                    if (depth > 4)
                    {
                        /* IPv4 Max MC group depth is 4 */
                        cpssOsPrintf("\nIPv4 MC Max tree depth is 4!!! Bucket address 0x%x\n",hwAddr);
                        cpssOsFree(bucketDataPtr);
                        numOfErrors++;
                        return GT_OK; /* continue to other branches in the tree */
                    }
                }
                else
                {
                    /* Multicast source tree */
                    if (depth > 8)
                    {
                        /* Each IPv4 MC group points to root of sources tree (with max depth of 4). So total max depth is 8. */
                        cpssOsPrintf("\nIPv4 MC source Max tree depth is 8!!! Bucket address 0x%x\n",hwAddr);
                        cpssOsFree(bucketDataPtr);
                        numOfErrors++;
                        return GT_OK; /* continue to other branches in the tree */
                    }
                    else if (depth == 8)
                    {
                        /* For IPv4 MC in depth 8 all LPM lines must be NH/ECMP/Qos bucket type */
                        cpssOsPrintf("\nFrom depth %d all LPM lines must be NH/ECMP/QoS bucket type!!! Bucket address 0x%x\n",8,hwAddr);
                        cpssOsFree(bucketDataPtr);
                        numOfErrors++;
                        return GT_OK; /* continue to other branches in the tree */
                    }
                }
            }
            else    /* PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E */
            {
                if (InSrcTree == GT_FALSE)
                {
                    /* Multicast group tree */
                    if (depth > PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS)
                    {
                        /* IPv6 Max MC group depth is 16 */
                        cpssOsPrintf("\nIPv6 MC Max tree depth is 16!!! Bucket address 0x%x\n",hwAddr);
                        cpssOsFree(bucketDataPtr);
                        numOfErrors++;
                        return GT_OK; /* continue to other branches in the tree */
                    }
                }
                else
                {
                    /* Multicast source tree */
                    if (depth > (2 * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS))
                    {
                        /* Each IPv6 MC group points to root of sources tree (with max depth of 16). So total max depth is 32. */
                        cpssOsPrintf("\nIPv6 MC source Max tree depth is 32!!! Bucket address 0x%x\n",hwAddr);
                        cpssOsFree(bucketDataPtr);
                        numOfErrors++;
                        return GT_OK; /* continue to other branches in the tree */
                    }
                    else if (depth == (2 * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS))
                    {
                        /* For IPv6 MC in depth 32 all LPM lines must be NH/ECMP/Qos bucket type */
                        cpssOsPrintf("\nFrom depth %d all LPM lines must be NH/ECMP/QoS bucket type!!! Bucket address 0x%x\n",
                                     (2 * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS), hwAddr);
                        cpssOsFree(bucketDataPtr);
                        numOfErrors++;
                        return GT_OK; /* continue to other branches in the tree */
                    }
                }
            }
        }

        printInfo->lpmLinesUsed[blockIndex] += (bitMapSize + numOfRanges);
        printInfo->lpmLinesPerOctet[depth] += (bitMapSize + numOfRanges);
        printInfo->bucketSizePerType[bucketType] += (bitMapSize + numOfRanges);
    }

    switch (bucketType)
    {
    case (CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E):
        hwBucketAddr = bucketDataPtr[0]; /* bitmap pointer */

        /* Bitmap has 4 bytes [range_4_start ; range_3_start ; range_2_start ; range_1_start]
           Range 0 start is always 0 (so no need to add to bitmap) */
        rangeStart[1] = hwBucketAddr & 0xFF;
        rangeStart[2] = (hwBucketAddr >> 8) & 0xFF;
        rangeStart[3] = (hwBucketAddr >> 16) & 0xFF;
        rangeStart[4] = (hwBucketAddr >> 24) & 0xFF;

        /* For FCoE Compressed 1 can contain one range for level 0 but must not contain one range for the other levels.
           For IPv4/IPv6 Compressed 1 must not contain one range for any level.
           Check that range_1_start (second range) must be != 0 */
        if ((protocol != PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E) || ((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E) && (depth > 0)))
        {
            if (rangeStart[1] == 0)
            {
                cpssOsPrintf("\nAt least 2 ranges should be defined!!! Bucket address 0x%x\n",hwAddr);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
        }

        rangeStartIsZero = 0;
        for (i = 1; i < PRV_CPSS_DXCH_LPM_RAM_NUM_OF_RANGES_IN_LPM_LINE_CNS; i++)
        {
            if (rangeStart[i] == 0)
            {
                /* no new ranges; first range x start is zero */
                rangeStartIsZero = 1;
                continue;
            }
            /* While range x start != 0 Current range start > previous range start */
            if ((rangeStart[i+1] <= rangeStart[i]) && (rangeStart[i+1] != 0))
            {
                cpssOsPrintf("\nNext range should be bigger then current range!!! Bucket address 0x%x\n",hwAddr);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
            /* After first range x start is 0, all ranges after must be 0*/
            if (rangeStartIsZero && (rangeStart[i] != 0))
            {
                cpssOsPrintf("\nAll range starts after first 0 must be 0!!! Bucket address 0x%x\n",hwAddr);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
        }

        for (i = 0; i < numOfRanges; i++)
        {
            hwBucketAddr = bucketDataPtr[(PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS + i)*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS];
            startAddress = (GT_U8)getBaseAddrFromHW(CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E,i,bucketDataPtr,bucketPtr);
            endAddress = (i == (numOfRanges-1))? 255 : getBaseAddrFromHW(CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E,i+1,bucketDataPtr,bucketPtr)- 1;
            if (endAddress > 255)
            {
                cpssOsPrintf("Err!!");
            }
            if ( (isUc == GT_FALSE)    &&
                 (InSrcTree == GT_TRUE)&&
                 (basePrefix == 0)    )
            {
                /* I am in mc src tree in zero bucket */
                baseAddress[0] = startAddress;
                baseAddrPtr = baseAddr;
            }
            else
                baseAddress[1] = startAddress;

            /* check if the next level is a bucket and if it points to a source tree */
            if (((hwBucketAddr & 0x3) == 0x0) && ((hwBucketAddr >> 2) & 0x1))
            {
                if (isUc == GT_TRUE)
                {
                    cpssOsPrintf("\nUnicast tree points to a source tree!!! Bucket address 0x%x\n",hwAddr);
                    cpssOsFree(bucketDataPtr);
                    numOfErrors++;
                    return GT_OK; /* continue to other branches in the tree */
                }
                else if (InSrcTree == GT_TRUE)
                {
                    cpssOsPrintf("\nMulticast source tree points to a source tree!!! Bucket address 0x%x\n",hwAddr);
                    cpssOsFree(bucketDataPtr);
                    numOfErrors++;
                    return GT_OK; /* continue to other branches in the tree */
                }
                else if ((hwBucketAddr & 0x3) == 0)
                {
                    /* It is last group bucket. The next pointer is not next hop
                       pointer type -- the src tree will be started */
                    if (printInfo->printRanges)
                    {
                        printLpmMcGroup(protocol, (GT_U8)basePrefixLength, startAddress, (GT_U8)endAddress);
                    }
                    InSrcTree = GT_TRUE;
                    grpPrefix = basePrefixLength;
                    basePrefixLength = 0;
                    cpssOsMemCpy(baseGroupAddr,baseAddr,sizeof(baseAddr));
                    cpssOsMemSet(baseAddr,0,sizeof(baseAddr));
                    baseGroupAddrPtr = baseAddrPtr;
                    baseAddrPtr = baseAddr;
                }
            }

            ret = printLpm(devNum,hwBucketAddr,baseAddrPtr,basePrefixLength,
                           startAddress,endAddress,isUc,protocol,depth+1,printInfo);
            if (ret != GT_OK)
            {
                cpssOsFree(bucketDataPtr);
                return ret;
            }

            if( (isUc == GT_FALSE)      &&
                (basePrefixLength == 0)       &&
                (InSrcTree == GT_TRUE)  )
            {
                /* The source tree was displayed and the other group regions
                   will be printed*/

                InSrcTree = GT_FALSE;
                basePrefixLength = grpPrefix;
                cpssOsMemCpy(baseAddr,baseGroupAddr,sizeof(baseAddr));
                baseAddrPtr = baseGroupAddrPtr;
                if (printInfo->printRanges)
                {
                    cpssOsPrintf(" End of Src tree for this group. \n");
                }
            }
        }
        break;

    case (CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E):
        hwBucketAddr = bucketDataPtr[0]; /* bitmap 1 pointer */

        /* Bitmap has 9 bytes; Word 1: [range_4_start ; range_3_start ; range_2_start ; range_1_start]
                               Word 2: [range_9_start ; range_8_start ; range_7_start ; range_6_start]
                                - range_5_start is carried from the pointer to this bucket
                                - range_0_start is always 0 (so no need to add to bitmap) */
        rangeStart[1] = hwBucketAddr & 0xFF;
        rangeStart[2] = (hwBucketAddr >> 8) & 0xFF;
        rangeStart[3] = (hwBucketAddr >> 16) & 0xFF;
        rangeStart[4] = (hwBucketAddr >> 24) & 0xFF;
        rangeStart[5] = (bucketPtr >> 5) & 0xFF;

        /* for word 1: all ranges start must be != 0 (if one is 0, then it should be compressed 1);
           Range_5_start must be != 0 (if 0, then should use compressed 1) */
        if ((rangeStart[1] == 0) || (rangeStart[2] == 0) || (rangeStart[3] == 0) || (rangeStart[4] == 0) || (rangeStart[5] == 0))
        {
            cpssOsPrintf("\nAt least 6 ranges should be defined!!! Bucket address 0x%x\n",hwAddr);
            cpssOsFree(bucketDataPtr);
            numOfErrors++;
            return GT_OK; /* continue to other branches in the tree */
        }

        hwBucketAddr = bucketDataPtr[1*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]; /* bitmap 2 pointer */

        rangeStart[6] = hwBucketAddr & 0xFF;
        rangeStart[7] = (hwBucketAddr >> 8) & 0xFF;
        rangeStart[8] = (hwBucketAddr >> 16) & 0xFF;
        rangeStart[9] = (hwBucketAddr >> 24) & 0xFF;

        rangeStartIsZero = 0;
        for (i = 1; i < (2 * PRV_CPSS_DXCH_LPM_RAM_NUM_OF_RANGES_IN_LPM_LINE_CNS + 1); i++)
        {
            if (rangeStart[i] == 0)
            {
                /* no new ranges; first range x start is zero */
                rangeStartIsZero = 1;
                continue;
            }
            /* While range x start != 0 Current range start > previous range start */
            if ((rangeStart[i+1] <= rangeStart[i]) && (rangeStart[i+1] != 0))
            {
                if (i <= PRV_CPSS_DXCH_LPM_RAM_NUM_OF_RANGES_IN_LPM_LINE_CNS)
                {
                    hwBucketAddr = bucketDataPtr[0]; /* bitmap 1 pointer */
                }
                else
                {
                    hwBucketAddr = bucketDataPtr[1*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]; /* bitmap 2 pointer */
                }
                cpssOsPrintf("\nNext range should be bigger then current range!!! Bucket address 0x%x\n",hwAddr);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
            /* After first range x start is 0, all ranges after must be 0*/
            if (rangeStartIsZero && (rangeStart[i] != 0))
            {
                if (i <= PRV_CPSS_DXCH_LPM_RAM_NUM_OF_RANGES_IN_LPM_LINE_CNS)
                {
                    hwBucketAddr = bucketDataPtr[0]; /* bitmap 1 pointer */
                }
                else
                {
                    hwBucketAddr = bucketDataPtr[1*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]; /* bitmap 2 pointer */
                }
                cpssOsPrintf("\nAll ranges must be 0!!! Bucket address 0x%x\n",hwAddr);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
        }

        for (i = 0; i < numOfRanges; i++)
        {
            hwBucketAddr = bucketDataPtr[PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS + i*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS];
            startAddress = (GT_U8)getBaseAddrFromHW(CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E,i,bucketDataPtr,bucketPtr);
            endAddress = (i == (numOfRanges-1))? 255 : getBaseAddrFromHW(CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E,i+1,bucketDataPtr,bucketPtr)- 1;
            if ( (isUc == GT_FALSE)    &&
                 (InSrcTree == GT_TRUE)&&
                 (basePrefix == 0)    )
            {
                /* I am in mc src tree in zero bucket */
                baseAddress[0] = startAddress;
                baseAddrPtr = baseAddr;
            }
            else
                baseAddress[1] = startAddress;

            if (((hwBucketAddr & 0x3) == 0x0) && ((hwBucketAddr >> 2) & 0x1))
            {
                if (isUc == GT_TRUE)
                {
                    cpssOsPrintf("\nUnicast tree points to a source tree!!! Bucket address 0x%x\n",hwAddr);
                    cpssOsFree(bucketDataPtr);
                    numOfErrors++;
                    return GT_OK; /* continue to other branches in the tree */
                }
                else if (InSrcTree == GT_TRUE)
                {
                    cpssOsPrintf("\nMulticast source tree points to a source tree!!! Bucket address 0x%x\n",hwAddr);
                    cpssOsFree(bucketDataPtr);
                    numOfErrors++;
                    return GT_OK; /* continue to other branches in the tree */
                }
                else if ((hwBucketAddr & 0x3) == 0)
                {
                    /* It is last group bucket. The next pointer is not next hop
                       pointer type -- the src tree will be started */
                    if (printInfo->printRanges)
                    {
                        printLpmMcGroup(protocol, (GT_U8)basePrefixLength, startAddress, (GT_U8)endAddress);
                    }
                    InSrcTree = GT_TRUE;
                    grpPrefix = basePrefixLength;
                    basePrefixLength = 0;
                    cpssOsMemCpy(baseGroupAddr,baseAddr,sizeof(baseAddr));
                    cpssOsMemSet(baseAddr,0,sizeof(baseAddr));
                    baseGroupAddrPtr = baseAddrPtr;
                    baseAddrPtr = baseAddr;
                }
            }

            ret = printLpm(devNum,hwBucketAddr,baseAddrPtr,basePrefixLength,
                           startAddress,endAddress,isUc,protocol,depth+1,printInfo);

            if (ret != GT_OK)
            {
                cpssOsFree(bucketDataPtr);
                return ret;
            }
            if( (isUc == GT_FALSE)      &&
                (basePrefixLength == 0)       &&
                (InSrcTree == GT_TRUE)  )
            {
                /* The source tree was displayed and the other group regions
                   will be printed*/

                InSrcTree = GT_FALSE;
                basePrefixLength = grpPrefix;
                cpssOsMemCpy(baseAddr,baseGroupAddr,sizeof(baseAddr));
                baseAddrPtr = baseGroupAddrPtr;
                if (printInfo->printRanges)
                {
                    cpssOsPrintf(" End of Src tree for this group. \n");
                }
            }
        }
        break;

    case (CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E):
        ret = validateBitVectorOfRegularBucket(bucketDataPtr, hwAddr, basePrefix);
        if (ret != GT_OK)
        {
            return GT_OK; /* continue to other branches in the tree */
        }
        for (i = 0; i < numOfRanges; i++)
        {
            hwBucketAddr = bucketDataPtr[11*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS + i*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS];
            startAddress = (GT_U8)getBaseAddrFromHW(CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E,i,bucketDataPtr,bucketPtr);
            endAddress = (i == (numOfRanges-1))? 255 : getBaseAddrFromHW(CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E,i+1,bucketDataPtr,bucketPtr)- 1;
            if ( (isUc == GT_FALSE)    &&
                 (InSrcTree == GT_TRUE)&&
                 (basePrefix == 0)    )
            {
                /* I am in mc src tree in zero bucket */
                baseAddress[0] = startAddress;
                baseAddrPtr = baseAddr;
            }
            else
                baseAddress[1] = startAddress;

            if (((hwBucketAddr & 0x3) == 0x0) && ((hwBucketAddr >> 2) & 0x1))
            {
                if (isUc == GT_TRUE)
                {
                    cpssOsPrintf("\nUnicast tree points to a source tree!!! Bucket address 0x%x\n",hwAddr);
                    cpssOsFree(bucketDataPtr);
                    numOfErrors++;
                    return GT_OK; /* continue to other branches in the tree */
                }
                else if (InSrcTree == GT_TRUE)
                {
                    cpssOsPrintf("\nMulticast source tree points to a source tree!!! Bucket address 0x%x\n",hwAddr);
                    cpssOsFree(bucketDataPtr);
                    numOfErrors++;
                    return GT_OK; /* continue to other branches in the tree */
                }
                else if ((hwBucketAddr & 0x3) == 0)
                {
                    /* It is last group bucket. The next pointer is not next hop
                       pointer type -- the src tree will be started */
                    if (printInfo->printRanges)
                    {
                        printLpmMcGroup(protocol, (GT_U8)basePrefixLength, startAddress, (GT_U8)endAddress);
                    }
                    InSrcTree = GT_TRUE;
                    grpPrefix = basePrefixLength;
                    basePrefixLength = 0;
                    cpssOsMemCpy(baseGroupAddr,baseAddr,sizeof(baseAddr));
                    cpssOsMemSet(baseAddr,0,sizeof(baseAddr));
                    baseGroupAddrPtr = baseAddrPtr;
                    baseAddrPtr = baseAddr;
                }
            }

            ret = printLpm(devNum,hwBucketAddr,baseAddrPtr,basePrefixLength,
                           startAddress,endAddress,isUc,protocol,depth+1,printInfo);
            if (ret != GT_OK)
            {
                cpssOsFree(bucketDataPtr);
                return ret;
            }
            if( (isUc == GT_FALSE)      &&
                (basePrefixLength == 0)       &&
                (InSrcTree == GT_TRUE)  )
            {
                /* The source tree was displayed and the other group regions
                   will be printed*/

                InSrcTree = GT_FALSE;
                basePrefixLength = grpPrefix;
                cpssOsMemCpy(baseAddr,baseGroupAddr,sizeof(baseAddr));
                baseAddrPtr = baseGroupAddrPtr;
                if (printInfo->printRanges)
                {
                    cpssOsPrintf(" End of Src tree for this group. \n");
                }
            }
        }
        break;

    case (CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E):
    case (CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E):
    case (CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E):

        if (bucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
        {
            /* Number of entries in NH table */
            routeEntriesNum = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop;
        }
        else
        {
            /* Number of entries in ECMP/QoS table */
            routeEntriesNum = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.ecmpQos;
        }
        nhPointer = ((bucketPtr >> 7) & 0x00007FFF);

        /* NH base address must be within the NH table */
        if (nhPointer >= routeEntriesNum)
        {
            cpssOsPrintf("\nNH pointer not in range of NH table!!! NH address %d\n",nhPointer);
            cpssOsFree(bucketDataPtr);
            numOfErrors++;
            return GT_OK; /* continue to other branches in the tree */
        }

        if ((bucketType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
            (bucketType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
        {
            ret = prvCpssDxChLpmHwEcmpEntryRead(devNum, nhPointer, &ecmpEntry);
            if (ret != GT_OK)
            {
                cpssOsPrintf("\nFailed to read ECMP entry %d\n",nhPointer);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
            routeEntriesNum = PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.routerNextHop;
            if (ecmpEntry.nexthopBaseIndex + ecmpEntry.numOfPaths >= routeEntriesNum)
            {
                cpssOsPrintf("\nECMP entry points to illegal nexthop index %d\n",nhPointer);
                cpssOsFree(bucketDataPtr);
                numOfErrors++;
                return GT_OK; /* continue to other branches in the tree */
            }
        }

        cpssOsMemSet(endAddr,0, sizeof(GT_U8) * (2 * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS));
        cpssOsMemSet(startAddr,0, sizeof(GT_U8) * (2 * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS));

        prefix = (GT_U8)basePrefix;
        prefixBase2Address(baseAddr ,prefix,1,addr);
        for (j = 8; j < prefix; j += 8)
        {
            startAddr[(j/8)-1] = addr[(j/8)-1];
            endAddr[(j/8)-1] = addr[(j/8)-1];
        }
        startAddr[(j/8)-1] = startAddress;
        endAddr[(j/8)-1] = (GT_U8)endAddress;
        if (printInfo->printRanges)
        {
            if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E)
            {
                outputAddress(startAddr, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, protocol);
                cpssOsPrintf(" - ");
                outputAddress(endAddr, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS, protocol);
            }
            else if (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E)
            {
                outputAddress(startAddr, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, protocol);
                cpssOsPrintf(" - ");
                outputAddress(endAddr, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS, protocol);
            }
            else /* PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E */
            {
                outputAddress(startAddr, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS, protocol);
                cpssOsPrintf(" - ");
                outputAddress(endAddr, PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS, protocol);
            }
            cpssOsPrintf(" / %02d ", prefix);
            if (bucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
            {
                cpssOsPrintf(" -> NH %X\n",nhPointer);
            }
            else
            {
                cpssOsPrintf(" -> ECMP/QoS %X\n",nhPointer);
            }
        }
        if (nhPointer >= PRV_CPSS_DXCH_LPM_RAM_NUM_OF_DEFAULT_ENTRIES)
            printInfo->numOfNonDefaultNextHopPointers++;
        break;

    default:
        break;
    }

    cpssOsFree(bucketDataPtr);
    return GT_OK;
}

GT_STATUS getRootBucketRanges(GT_U8 devNum, GT_U32 vrId,
                              PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT protocol,
                              GT_U32 *bitMapSize, GT_U32 *numOfRanges,
                              GT_U32 *ranges, GT_U32 *bucketDataPtr,
                              prvLpmDumpPrintInfo_STC *printInfo)
{
    GT_U32                              rootNodeAddr = 0;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT nodeType = 0;
    GT_STATUS                           status = 0;
    GT_U32                              i = 0;
    GT_U32                              blockIndex = 0;

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_SUPPORTED_MAC(devNum) == GT_TRUE)
    {
        /* the root bucket for all protocols are located by default in
           the init phase in LPM block 0*/
        blockIndex=0;
    }
    else
    {
        switch (protocol)
        {
        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E:
            blockIndex = addrLookupMemIpv4Array[0];
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E:
            blockIndex = addrLookupMemIpv6Array[0];
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E:
            blockIndex = addrLookupMemFcoeArray[0];
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
    }

    status = prvCpssDxChLpmHwVrfEntryRead(devNum, vrId, protocol,
                                          &nodeType, &rootNodeAddr);
    if (status != GT_OK)
    {
        return status;
    }

    if (nodeType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
    {
        *bitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
        /* hw pointer in LPM entry is in LPM lines*/
    }
    else /* CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E */
    {
        *bitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;
    }

    /* read the bit vector */
    status = prvCpssDxChReadTableMultiEntry(devNum,
                                         PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                         rootNodeAddr, *bitMapSize, bucketDataPtr);
    if (status != GT_OK)
    {
        return status;
    }

    *numOfRanges = getNumOfRangesFromHW(nodeType, bucketDataPtr, 0);

    /* read the ranges (the LPM lines after the bit vector) */
    status = prvCpssDxChReadTableMultiEntry(devNum,
                                         PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                         rootNodeAddr + (*bitMapSize), *numOfRanges,
                                         &bucketDataPtr[(*bitMapSize)*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]);
    if (status != GT_OK)
    {
        return status;
    }

    if (nodeType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
    {
        ranges[0] = 0;
        ranges[1] = bucketDataPtr[0] & 0xFF;
        ranges[2] = (bucketDataPtr[0] >> 8) & 0xFF;
        ranges[3] = (bucketDataPtr[0] >> 16) & 0xFF;
        ranges[4] = (bucketDataPtr[0] >> 24) & 0xFF;
    }
    else /* CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E */
    {
        status = validateBitVectorOfRegularBucket(bucketDataPtr, rootNodeAddr, 8);
        if (status != GT_OK)
        {
            return GT_OK; /* continue to other branches in the tree */
        }
        for (i = 0; i < *numOfRanges; i++)
        {
            ranges[i] = (GT_U8)getBaseAddrFromHW(CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E, i, bucketDataPtr, 0);
        }
    }

    /* update the print info for the first octet */
    printInfo->lpmLinesUsed[blockIndex] += (*bitMapSize + *numOfRanges);
    printInfo->lpmLinesPerOctet[0] = (*bitMapSize + *numOfRanges);
    printInfo->bucketSizePerType[nodeType] = (*bitMapSize + *numOfRanges);
    printInfo->bucketPerType[nodeType] = 1;
    printInfo->bucketPerOctet[0] = 1;
    printInfo->bucketTypePerOctet[0][nodeType] = 1;
    return GT_OK;
}

GT_STATUS printLpmIpv4UcTable(GT_U8 devNum, GT_U32 vrId, prvLpmDumpPrintInfo_STC *printInfo)
{
    GT_U32    numOfRanges;
    GT_U32    ranges[256];
    GT_U32    endAddress;
    GT_U32    *bucketDataPtr;
    GT_U32    bitMapSize;
    GT_U32    i;
    GT_STATUS ret;
    GT_BOOL   needToFreeBucketDataPtr=GT_FALSE;

     /* Allocate memory for bucket data. Maximum size is regular bucket that
       uses 256 lpm lines for ranges + 11 lpm lines for bitmap */
    bucketDataPtr = cpssOsMalloc(PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    if (bucketDataPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    cpssOsMemSet(bucketDataPtr, 0, PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    cpssOsMemSet(baseAddr, 0, sizeof(GT_U8)* PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
    ret = getRootBucketRanges(devNum, vrId, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                              &bitMapSize, &numOfRanges, ranges,
                              bucketDataPtr, printInfo);

    if (ret != GT_OK)
    {
        cpssOsFree(bucketDataPtr);
        return ret;
    }

    /* if an error accur in getRootBucketRanges then bucketDataPtr was already free */
    if(numOfErrors == 0)
        needToFreeBucketDataPtr = GT_TRUE;

    for (i = 0; i < numOfRanges; i++)
    {
        if (ranges[i] < PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS)
        {
            if ((i == (numOfRanges - 1)) ||
                (ranges[i+1] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS))
            {
                endAddress = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS - 1;
            }
            else
            {
                endAddress = ranges[i+1] - 1;
            }
        }
        else
        {
            break;
        }
        baseAddr[0] = (GT_U8)ranges[i];
        ret = printLpm(devNum, bucketDataPtr[bitMapSize+i], baseAddr, 8, baseAddr[0],
                       endAddress, GT_TRUE, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                       1, printInfo);
        if ((ret != GT_OK) || numOfErrors)
        {
            cpssOsPrintf("\nInconsistency exists!!!\n");
        }
    }

    if(needToFreeBucketDataPtr == GT_TRUE)
        cpssOsFree(bucketDataPtr);

    return ret;
}

GT_STATUS printLpmIpv4McTable(GT_U8 devNum, GT_U32 vrId, prvLpmDumpPrintInfo_STC *printInfo)
{
    GT_U32    numOfRanges;
    GT_U32    ranges[256];
    GT_U32    endAddress;
    GT_U32    *bucketDataPtr;
    GT_U32    bitMapSize;
    GT_U32    i;
    GT_STATUS ret;
    GT_BOOL   needToFreeBucketDataPtr=GT_FALSE;

     /* Allocate memory for bucket data. Maximum size is regular bucket that
       uses 256 lpm lines for ranges + 11 lpm lines for bitmap */
    bucketDataPtr = cpssOsMalloc(PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    if (bucketDataPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    cpssOsMemSet(bucketDataPtr, 0, PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    cpssOsMemSet(baseAddr, 0, sizeof(GT_U8)* PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
    ret = getRootBucketRanges(devNum, vrId, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                              &bitMapSize, &numOfRanges, ranges,
                              bucketDataPtr, printInfo);
    if (ret != GT_OK)
    {
        cpssOsFree(bucketDataPtr);
        return ret;
    }

    /* if an error accur in getRootBucketRanges then bucketDataPtr was already free */
    if(numOfErrors == 0)
        needToFreeBucketDataPtr = GT_TRUE;

    for (i = 0; i < numOfRanges; i++)
    {
        if (ranges[i] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_RESERVED_SPACE_ADDRESS_SPACE_CNS)
        {
            break;
        }
        else if (ranges[i] < PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS)
        {
            continue;
        }
        else if (i != (numOfRanges - 1))
        {
            endAddress = ranges[i+1] - 1;
        }
        else
        {
            endAddress = PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_MC_ADDRESS_SPACE_CNS;
        }
        baseAddr[0] = (GT_U8)ranges[i];
        InSrcTree = GT_FALSE;
        ret = printLpm(devNum, bucketDataPtr[bitMapSize+i], baseAddr, 8, baseAddr[0],
                       endAddress, GT_FALSE, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                       1, printInfo);
        if ((ret != GT_OK) || numOfErrors)
        {
            cpssOsPrintf("\nInconsistency exists!!!\n");
        }
    }
    if(needToFreeBucketDataPtr == GT_TRUE)
        cpssOsFree(bucketDataPtr);

    return ret;
}

GT_STATUS printLpmIpv6UcTable(GT_U8 devNum, GT_U32 vrId, prvLpmDumpPrintInfo_STC *printInfo)
{
    GT_U32    numOfRanges;
    GT_U32    ranges[256];
    GT_U32    endAddress;
    GT_U32    *bucketDataPtr;
    GT_U32    bitMapSize;
    GT_U32    i;
    GT_STATUS ret;
    GT_BOOL   needToFreeBucketDataPtr=GT_FALSE;

    /* Allocate memory for bucket data. Maximum size is regular bucket that
       uses 256 lpm lines for ranges + 11 lpm lines for bitmap */
    bucketDataPtr = cpssOsMalloc(PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    if (bucketDataPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    cpssOsMemSet(bucketDataPtr, 0, PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    cpssOsMemSet(baseAddr, 0, sizeof(GT_U8)* PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
    ret = getRootBucketRanges(devNum, vrId, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                              &bitMapSize, &numOfRanges, ranges,
                              bucketDataPtr, printInfo);
    if (ret != GT_OK)
    {
        cpssOsFree(bucketDataPtr);
        return ret;
    }
    /* if an error accur in getRootBucketRanges then bucketDataPtr was already free */
    if(numOfErrors == 0)
        needToFreeBucketDataPtr = GT_TRUE;

    for (i = 0; i < numOfRanges; i++)
    {
        if (ranges[i] < PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS)
        {
            if ((i == (numOfRanges - 1)) ||
                (ranges[i+1] >= PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS))
            {
                endAddress = PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS - 1;
            }
            else
            {
                endAddress = ranges[i+1] - 1;
            }
        }
        else
        {
            break;
        }
        baseAddr[0] = (GT_U8)ranges[i];
        ret = printLpm(devNum, bucketDataPtr[bitMapSize+i], baseAddr, 8, baseAddr[0],
                       endAddress, GT_TRUE, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                       1, printInfo);
        if ((ret != GT_OK) || numOfErrors)
        {
            cpssOsPrintf("\nInconsistency exists!!!\n");
        }
    }
    if(needToFreeBucketDataPtr == GT_TRUE)
        cpssOsFree(bucketDataPtr);

    return ret;
}

GT_STATUS printLpmIpv6McTable(GT_U8 devNum, GT_U32 vrId, prvLpmDumpPrintInfo_STC *printInfo)
{
    GT_U32    numOfRanges = 0;
    GT_U32    ranges[256] = {0};
    GT_U32    endAddress = 0;
    GT_U32    *bucketDataPtr = NULL;
    GT_U32    bitMapSize = 0;
    GT_U32    i = 0;
    GT_STATUS ret = GT_FAIL;
    GT_BOOL   needToFreeBucketDataPtr=GT_FALSE;

    /* Allocate memory for bucket data. Maximum size is regular bucket that
       uses 256 lpm lines for ranges + 11 lpm lines for bitmap */
    bucketDataPtr = cpssOsMalloc(PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    if (bucketDataPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    cpssOsMemSet(bucketDataPtr, 0, PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));

    cpssOsMemSet(baseAddr, 0, sizeof(GT_U8)* PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);

    ret = getRootBucketRanges(devNum, vrId, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                              &bitMapSize, &numOfRanges, ranges,
                              bucketDataPtr, printInfo);
    if (ret != GT_OK)
    {
        cpssOsFree(bucketDataPtr);
        return ret;
    }

    /* if an error accur in getRootBucketRanges then bucketDataPtr was already free */
    if(numOfErrors == 0)
        needToFreeBucketDataPtr = GT_TRUE;

    for (i = 0; i < numOfRanges; i++)
    {
        if (ranges[i] < PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS)
        {
            continue;
        }
        else if (i != (numOfRanges - 1))
        {
            endAddress = ranges[i+1] - 1;
        }
        else
        {
            endAddress = PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV6_MC_ADDRESS_SPACE_CNS;
        }
        baseAddr[0] = (GT_U8)ranges[i];
        InSrcTree = GT_FALSE;
        ret = printLpm(devNum, bucketDataPtr[bitMapSize+i], baseAddr, 8, baseAddr[0],
                       endAddress, GT_FALSE, PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                       1, printInfo);
        if ((ret != GT_OK) || numOfErrors)
        {
            cpssOsPrintf("\nInconsistency exists!!!\n");
        }
    }
    if(needToFreeBucketDataPtr == GT_TRUE)
        cpssOsFree(bucketDataPtr);
    return ret;
}

GT_STATUS printLpmFcoeTable(GT_U8 devNum, GT_U32 vrId, prvLpmDumpPrintInfo_STC *printInfo)
{
    GT_U32    numOfRanges;
    GT_U32    ranges[256];
    GT_U32    endAddress;
    GT_U32    *bucketDataPtr;
    GT_U32    bitMapSize;
    GT_U32    i;
    GT_STATUS ret;
    GT_BOOL   needToFreeBucketDataPtr=GT_FALSE;

    /* Allocate memory for bucket data. Maximum size is regular bucket that
       uses 256 lpm lines for ranges + 11 lpm lines for bitmap */
    bucketDataPtr = cpssOsMalloc(PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    if (bucketDataPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    cpssOsMemSet(bucketDataPtr, 0, PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    cpssOsMemSet(baseAddr, 0, sizeof(GT_U8)* PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
    ret = getRootBucketRanges(devNum, vrId, PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E,
                              &bitMapSize, &numOfRanges, ranges,
                              bucketDataPtr, printInfo);
    if (ret != GT_OK)
    {
        cpssOsFree(bucketDataPtr);
        return ret;
    }

    /* if an error accur in getRootBucketRanges then bucketDataPtr was already free */
    if(numOfErrors == 0)
        needToFreeBucketDataPtr = GT_TRUE;

    for (i = 0; i < numOfRanges; i++)
    {
        baseAddr[0] = (GT_U8)ranges[i];
        if (i != (numOfRanges - 1))
        {
            endAddress = ranges[i+1] - 1;
        }
        else
        {
            endAddress = 255;
        }
        ret = printLpm(devNum, bucketDataPtr[bitMapSize+i], baseAddr, 8, baseAddr[0],
                       endAddress, GT_TRUE, PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E,
                       1, printInfo);
        if ((ret != GT_OK) || numOfErrors)
        {
            cpssOsPrintf("\nInconsistency exists!!!\n");
        }
    }
    if(needToFreeBucketDataPtr == GT_TRUE)
        cpssOsFree(bucketDataPtr);
    return ret;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgHwDfsScan
*
* DESCRIPTION:
*       This function is intended to do hardware LPM validation and dump
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum                       - physical device number.
*       vrId                         - virtual router ID
*       protocol                     - the protocol
*       prefixType                   - the prefix type (Unicast or Multicast)
*       print                        - print ranges
*                                      GT_TRUE --- print ranges
*                                      GT_FALSE --- don't print ranges
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
********************************************************************************/
static GT_STATUS prvCpssDxChLpmRamDbgHwDfsScan
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    IN  CPSS_UNICAST_MULTICAST_ENT              prefixType,
    IN  GT_BOOL                                 print
)
{
    GT_STATUS rc = GT_OK;                   /* return code */
    GT_U32    i;
    GT_U32    maxDepth = 0;

    prvLpmDumpPrintInfo_STC printInfo; /* strcut holds all the printing info */

    cpssOsMemSet(&printInfo, 0, sizeof(prvLpmDumpPrintInfo_STC));

    printInfo.printRanges = print;

    /* check if device is active */
    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);

    numOfErrors = 0;
    switch (protocol)
    {
        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E:
            if (prefixType == CPSS_UNICAST_E)
            {
                rc = printLpmIpv4UcTable(devNum, vrId, &printInfo);
            }
            else /* CPSS_MULTICAST_E */
            {
                rc = printLpmIpv4McTable(devNum, vrId, &printInfo);
            }
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E:
            if (prefixType == CPSS_UNICAST_E)
            {
                rc = printLpmIpv6UcTable(devNum, vrId, &printInfo);
            }
            else /* CPSS_MULTICAST_E */
            {
                rc = printLpmIpv6McTable(devNum, vrId, &printInfo);
            }
            break;

        case PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E:
            rc = printLpmFcoeTable(devNum, vrId, &printInfo);
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }
    if (printInfo.printRanges)
    {
        cpssOsPrintf("LPM lines used by the virtual router buckets:\n");
        for (i = 0 ; i < PRV_CPSS_DXCH_PP_MAC(devNum)->hwInfo.lpm.numOfBlocks; i++)
        {
            cpssOsPrintf("    Block[%d]: %d\n", i, printInfo.lpmLinesUsed[i]);
        }

        cpssOsPrintf("Buckets per type:\n");
        cpssOsPrintf("   Regular node = %d\n", printInfo.bucketPerType[0]);
        cpssOsPrintf("   Compressed 1 node = %d\n", printInfo.bucketPerType[1]);
        cpssOsPrintf("   Compressed 2 node = %d\n", printInfo.bucketPerType[2]);

        cpssOsPrintf("Average bucket size per type:\n");
        cpssOsPrintf("   Regular node = %4.2f\n", (printInfo.bucketPerType[0] == 0)?0:(float)printInfo.bucketSizePerType[0]/printInfo.bucketPerType[0]);
        cpssOsPrintf("   Compressed 1 node = %4.2f\n", (printInfo.bucketPerType[1] == 0)?0:(float)printInfo.bucketSizePerType[1]/printInfo.bucketPerType[1]);
        cpssOsPrintf("   Compressed 2 node = %4.2f\n", (printInfo.bucketPerType[2] == 0)?0:(float)printInfo.bucketSizePerType[2]/printInfo.bucketPerType[2]);

        cpssOsPrintf("\nNumber of non default next hop pointers: %d\n", printInfo.numOfNonDefaultNextHopPointers);

        cpssOsPrintf("\n<octet>  <lpm lines>  <# buckets>  <# comp1>  <# comp2>  <# regular>\n");
        cpssOsPrintf("----------------------------------------------------------------------------------------------------------------\n");

        /* maxDepth must be declared as local var (and not macro in loop), prevent gcc -O2 infinite loop bug */
        maxDepth = 2 * PRV_DXCH_LPM_RAM_DBG_MAX_DEPTH_CHECK_MAC(protocol);
        for (i = 0 ; i < maxDepth; i++)
        {
            cpssOsPrintf("    %d\t %d\t %d\t %d\t %d\t %d\n",i+1,
                            printInfo.lpmLinesPerOctet[i],
                            printInfo.bucketPerOctet[i],
                            printInfo.bucketTypePerOctet[i][1],
                            printInfo.bucketTypePerOctet[i][2],
                            printInfo.bucketTypePerOctet[i][0]);
        }
    }
    if (numOfErrors > 0)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgDump
*
* DESCRIPTION:
*       This function is intended to do hardware LPM dump
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum                       - physical device number.
*       vrId                         - virtual router ID
*       protocol                     - the protocol
*       prefixType                   - the prefix type (Unicast or Multicast)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbgDump
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    IN  CPSS_UNICAST_MULTICAST_ENT              prefixType
)
{
    return prvCpssDxChLpmRamDbgHwDfsScan(devNum, vrId, protocol, prefixType, GT_TRUE);
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgHwValidation
*
* DESCRIPTION:
*       This function is intended to do hardware LPM validation
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum                       - physical device number.
*       vrId                         - virtual router ID
*       protocol                     - the protocol
*       prefixType                   - the prefix type (Unicast or Multicast)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum or vrId.
*       GT_HW_ERROR              - on Hardware error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS: All the checks
*       For every LPM LINE
*           Bucket type (bits 3-4) - all values valid
*
*           If bucket type regular, bits 5-28 are next bucket pointer (index of LPM line, offset from LPM mem start)
*               Can check validity of the regular bitmap of the next bucket (see later)
*
*           If bucket type compressed-1, bits 5-28 are next bucket pointer
*               Can check validity of compressed-1 bitmap of the next bucket (see later)
*
*           If bucket type is compressed-2, bits 13-31 are next bucket point
*               Can check validity of compressed-2 bitmap (and pass its 5th range as parameter)
*
*   Regular bitmap
*   --------------
*       For first word, sum must be 0
*       For each word in the bitmap other than first
*           Sum = previous_sum + number of bits in previous word
*       On last word, last 8 bits must be 0 (those do not count as 11*24 = 264 = 256 + 8)
*
*       Regular bucket is for 11-255 ranges so there must be at least 11 bits != 0 (exception here for first MC source bucket)
*       Bit 0 in word 0 must be set because first range always starts in 0
*
*   Compressed 1 bitmap (ranges are 0 based)
*   ----------------------------------------
*       Bitmap has 4 bytes [range_4_start ; range_3_start ; range_2_start ; range_1_start]
*       Range 0 start is always 0 (so no need to add to bitmap).
*
*       While range x start != 0
*           Current range start > previous range start
*       After first 0, all must be 0
*
*       Compressed 1 must contain 2-5 ranges so range_1_start (second range) must be != 0
*
*   Compressed 2 bitmap (ranges are 0 based)
*   ----------------------------------------
*       Word 1: [range_4_start ; range_3_start ; range_2_start ; range_1_start]
*       Word 2: [range_9_start ; range_8_start ; range_7_start ; range_6_start]
*       range_5_start is carried from the pointer to this bucket
*
*       for each word:
*           each range start > previous range start
*       for word 1:
*           all ranges start must be != 0 (if one is 0, then it should be compressed 1)
*       for word 2:
*           all must be bigger than range_5_start or 0!!
*
*       Range_5_start must be != 0 (if 0, then should use compressed 1)
*
*   Tree based checks
*   -----------------
*   IPv4 UC
*       Max tree depth is 4 so from depth 4 all LPM lines must be NH or ECMP/QoS bucket type
*       Illegal to point to a source tree.
*
*   IPv6 UC
*       Same but depth is 16
*
*   IPv4 MC SRC
*       Max MC group depth is 4 and each group points to root of sources tree (with max depth of 4). So total max depth is 8.
*       Illegal to point to another source tree.
*       If you reach depth 8, then all LPM lines must be pointing to NH or ECMP/QoS.
*
*   IPv6 MC
*       Same but 8 instead of 4 and 16 instead of 8
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbgHwValidation
(
    IN  GT_U8                                   devNum,
    IN  GT_U32                                  vrId,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT    protocol,
    IN  CPSS_UNICAST_MULTICAST_ENT              prefixType
)
{
    return prvCpssDxChLpmRamDbgHwDfsScan(devNum, vrId, protocol, prefixType, GT_FALSE);
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgDbHwMemPrint
*
* DESCRIPTION:
*       Function Relevant mode : High Level API modes
*
*       This function print LPM debug information
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr                 - The LPM DB.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbgDbHwMemPrint
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr
)
{
    GT_STATUS retVal;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *shadowPtr;
    GT_UINTPTR  partitionId;
    GT_U32      i;

    shadowPtr = &lpmDbPtr->shadowArray[0];

    for ( i = 0 ; i < shadowPtr->numOfLpmMemories ; i++ )
    {
        partitionId = shadowPtr->lpmRamStructsMemPoolPtr[i];
        if( partitionId == 0)
            continue;

        retVal = prvCpssDmmPartitionPrint(partitionId);
        if(retVal != GT_OK)
        {
            return (retVal);
        }
    }

    return (GT_OK);
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgRouteEntryShadowValidityCheck
*
* DESCRIPTION:
*       Shadow validation of a route entry
*
* INPUTS:
*       shadowPtr               - pointer to shadow structure
*       routeEntryPtr           - pointer to route entry
*       returnOnFailure         - GT_TRUE: the validation check stops on
*                                          first failure
*                                 GT_FALSE: continue with the test on failure
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_FAIL - on failure
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamDbgRouteEntryShadowValidityCheck
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC            *shadowPtr,
    IN  PRV_CPSS_DXCH_LPM_ROUTE_ENTRY_POINTER_STC   *routeEntryPtr,
    IN  GT_BOOL                                     returnOnFailure
)
{
    GT_U32    shareDevListLen, devIdx, maxMemSize;
    GT_U8     *shareDevsList;  /* List of devices sharing this LPM structure */
    GT_STATUS retVal = GT_OK;

    shareDevsList   = shadowPtr->workDevListPtr->shareDevs;
    shareDevListLen = shadowPtr->workDevListPtr->shareDevNum;

    if (routeEntryPtr == NULL)
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Next hop",
                                        0,
                                        "routeEntryPtr is null");
    }
    else
    {
        if ((routeEntryPtr->routeEntryMethod != PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E) &&
            (routeEntryPtr->routeEntryMethod != PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E) &&
            (routeEntryPtr->routeEntryMethod != PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Next hop",
                                            routeEntryPtr,
                                            "illegal route entry method");
        }

         /* For each device check validity of route entries */
        for (devIdx = 0; devIdx < shareDevListLen; devIdx++)
        {
            if (routeEntryPtr->routeEntryMethod == PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E)
            {
                maxMemSize = PRV_CPSS_DXCH_PP_MAC(shareDevsList[devIdx])->fineTuning.tableSize.routerNextHop;
            }
            else
            {
                maxMemSize = PRV_CPSS_DXCH_PP_MAC(shareDevsList[devIdx])->fineTuning.tableSize.ecmpQos;
            }
            /* Check that the base address is within the table */
            if (routeEntryPtr->routeEntryBaseMemAddr >= maxMemSize)
            {
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Next hop",
                                                routeEntryPtr,
                                                "illegal route entry address");
            }

            if (routeEntryPtr->routeEntryMethod != PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E)
            {
                /* ECMP/QoS */
                maxMemSize = PRV_CPSS_DXCH_PP_MAC(shareDevsList[devIdx])->fineTuning.tableSize.routerNextHop;
                if (routeEntryPtr->routeEntryBaseMemAddr + routeEntryPtr->blockSize > maxMemSize)
                {
                    LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "ECMP",
                                                    routeEntryPtr,
                                                    "illegal ECMP entry, out of bound of nexthop table");
                }
            }
        }

        if (routeEntryPtr->ipv6McGroupScopeLevel > CPSS_IPV6_PREFIX_SCOPE_GLOBAL_E)
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Next hop",
                                            routeEntryPtr,
                                            "illegal IPv6 MC Group Scope Level");
        }
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgTrieShadowValidityCheck
*
* DESCRIPTION:
*       Validation of bucket's trie
*
* INPUTS:
*       rootPtr                    - a root node
*       level                      - the level in the trie. first level is 0.
*       nodeStartAddr              - the start address that this node represents
*       sumOfHigherLevelsMask      - the sum of the masks for this node's higher
*                                    levels
*       returnOnFailure            - GT_TRUE: the validation check stops on
*                                             first failure
*                                    GT_FALSE: continue with the test on failure
*       maskForRangeInTrieArray    - array of the ranges masks as found in the
*                                    trie. The array index represents the range
*                                    start address
*       validRangeInTrieArray      - array to indicate if a range was found in
*                                    the trie. The array index represents the
*                                    range start address.
*
* OUTPUTS:
*       maskForRangeInTrieArray    - array of the ranges masks as found in the
*                                    trie. The array index represents the range
*       validRangeInTrieArray      - array to indicate if a range was found in
*                                    the trie. The array index represents the
*                                    range start address.
*
* RETURNS:
*       GT_FAIL - on failure
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamDbgTrieShadowValidityCheck
(
    IN    PRV_CPSS_DXCH_LPM_RAM_TRIE_NODE_STC     *rootPtr,
    IN    GT_U8                                   level,
    IN    GT_U8                                   nodeStartAddr,
    IN    GT_U8                                   sumOfHigherLevelsMask,
    IN    GT_BOOL                                 returnOnFailure,
    INOUT GT_U8                                   maskForRangeInTrieArray[],
    INOUT GT_BOOL                                 validRangeInTrieArray[]
)
{
    GT_U8 power, rangeIdx, thisLevelMask = 0;
    GT_STATUS status, retVal = GT_OK;

    /* Check that there are not too many levels in the trie */
    if (level > PRV_CPSS_DXCH_LPM_RAM_MAX_LEVEL_LENGTH_CNS)
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Trie",
                                        rootPtr,
                                        "illegal level in the trie");
    }

    /* the trie root must contain pData */
    if ((level == 0) && (rootPtr->pData == NULL))
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Trie",
                                        rootPtr,
                                        "the trie root must contain pData");
    }

    /* the node must hold some info */
    if ((rootPtr->pData == NULL) && (rootPtr->leftSon == NULL) && (rootPtr->rightSon == NULL))
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Trie",
                                        rootPtr,
                                        "the trie node holds no pData and no children");
    }

    /* Father and son should point to each other */
    if (rootPtr->leftSon != NULL)
    {
        if (rootPtr->leftSon->father != rootPtr)
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Trie",
                                            rootPtr,
                                            "father and left son don't point to each other");
        }
    }
    if (rootPtr->rightSon != NULL)
    {
        if (rootPtr->rightSon->father != rootPtr)
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Trie",
                                            rootPtr,
                                            "father and right son don't point to each other");
        }
    }

    /* The node holds a pointer to NH, so startAddr is a start of range */
    if (rootPtr->pData != NULL)
    {
        if (level > 0)
        {
            status = prvCpssMathPowerOf2((GT_U8)(level - 1), &thisLevelMask);
            if (status != GT_OK)
            {
                cpssOsPrintf("prvCpssMathPowerOf2 failed\n");
                return status;
            }
            maskForRangeInTrieArray[nodeStartAddr] =
                (GT_U8)(sumOfHigherLevelsMask + thisLevelMask);
        }
        else
        {
            maskForRangeInTrieArray[nodeStartAddr] = 0;
        }
        validRangeInTrieArray[nodeStartAddr] = GT_TRUE;
    }
    /* For all the levels except the root level:
       In case that the node is a left son and it represents a start address,
       a new range exists and its startAddr is the end address of the node's
       range + 1 */
    if ((level > 0) && (rootPtr->father->leftSon == rootPtr) && (rootPtr->pData != NULL))
    {
        status = prvCpssMathPowerOf2((GT_U8)(8 - level), &power);
        if (status != GT_OK)
        {
            cpssOsPrintf("prvCpssMathPowerOf2 failed\n");
            return status;
        }
        rangeIdx = (GT_U8)(nodeStartAddr + power);
        validRangeInTrieArray[rangeIdx] = GT_TRUE;
        maskForRangeInTrieArray[rangeIdx] = sumOfHigherLevelsMask;
    }

    level++;
    sumOfHigherLevelsMask = (GT_U8)(sumOfHigherLevelsMask + thisLevelMask);
    /* Validate the sub-tries */
    if (rootPtr->leftSon != NULL)
    {
        status = prvCpssDxChLpmRamDbgTrieShadowValidityCheck(rootPtr->leftSon,
                                                             level,
                                                             nodeStartAddr,
                                                             sumOfHigherLevelsMask,
                                                             returnOnFailure,
                                                             maskForRangeInTrieArray,
                                                             validRangeInTrieArray);
        if (status != GT_OK)
        {
            retVal = status;
            if (returnOnFailure == GT_TRUE)
            {
                return retVal;
            }
        }
    }
    if (rootPtr->rightSon != NULL)
    {
        status = prvCpssMathPowerOf2((GT_U8)(8 - level), &power);
        if (status != GT_OK)
        {
            cpssOsPrintf("prvCpssMathPowerOf2 failed\n");
            return status;
        }
        status = prvCpssDxChLpmRamDbgTrieShadowValidityCheck(rootPtr->rightSon,
                                                             level,
                                                             (GT_U8)(nodeStartAddr + power),
                                                             sumOfHigherLevelsMask,
                                                             returnOnFailure,
                                                             maskForRangeInTrieArray,
                                                             validRangeInTrieArray);
        if (status != GT_OK)
        {
            retVal = status;
            if (returnOnFailure == GT_TRUE)
            {
                return retVal;
            }
        }
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgRangesShadowValidityCheck
*
* DESCRIPTION:
*       Shadow validation of bucket's ranges
*
* INPUTS:
*       shadowPtr                 - pointer to shadow structure
*       vrId                      - the virtual router id
*       bucketPtr                 - pointer to the bucket
*       maskForRangeInTrieArray   - array of the ranges masks as found in the
*                                   trie. The array index represents the range
*                                   start address
*       validRangeInTrieArray     - array to indicate if a range was found in
*                                   the trie. The array index represents the
*                                   range start address.
*       level                     - the level in the tree (first level is 0)
*       numOfMaxAllowedLevels     - the maximal number of levels that is allowed
*                                   for the relevant protocol and prefix type
*       prefixType                - unicast or multicast tree
*       protocol                  - protocol
*       returnOnFailure           - GT_TRUE: the validation check stops on
*                                            first failure
*                                   GT_FALSE: continue with the test on failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_FAIL - on failure
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamDbgRangesShadowValidityCheck
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  GT_U32                                    vrId,
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  GT_U8                                     maskForRangeInTrieArray[],
    IN  GT_BOOL                                   validRangeInTrieArray[],
    IN  GT_U8                                     level,
    IN  GT_U8                                     numOfMaxAllowedLevels,
    IN  CPSS_UNICAST_MULTICAST_ENT                prefixType,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol,
    IN  GT_BOOL                                   returnOnFailure
)
{
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC  *rangePtr, *nextRangePtr;
    GT_U32                                  bucketBaseAddr, rangeOffset,
                                            expectedPointingRangeMemAddr;
    GT_U16                                  numOfRanges, numOfRangesForPrefixType;
    GT_U8                                   prefixTypeFirstRange, prefixTypeLastRange;
    GT_U32                                  tmpPrefixTypeRange;
    GT_U8                                   trieRangeIdx;
    GT_STATUS                               status, retVal = GT_OK;

    rangePtr = bucketPtr->rangeList;
    numOfRanges = 0;
    numOfRangesForPrefixType = 0;

    if (level == 0)
    {
        if (prefixType == CPSS_UNICAST_E)
        {
            prefixTypeFirstRange = 0;
            tmpPrefixTypeRange = (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
                (PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS - 1) :
                (PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS - 1);
            prefixTypeLastRange = (GT_U8)tmpPrefixTypeRange;
        }
        else /* CPSS_MULTICAST_E */
        {
            tmpPrefixTypeRange = (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
                PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS :
                PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS;
            prefixTypeFirstRange = (GT_U8)tmpPrefixTypeRange;
            tmpPrefixTypeRange = (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
                PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_MC_ADDRESS_SPACE_CNS :
                PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV6_MC_ADDRESS_SPACE_CNS;
            prefixTypeLastRange = (GT_U8)tmpPrefixTypeRange;
        }
    }
    else
    {
        prefixTypeFirstRange = 0;
        prefixTypeLastRange = 255;
    }

    /* Check that the numbers of actual ranges is bucketPtr->numOfRanges */
    while (rangePtr)
    {
        /* Skip unicast ranges for multicast validation or multicast ranges for
           unicast validation */
        if ((rangePtr->startAddr < prefixTypeFirstRange) ||
            (rangePtr->startAddr > prefixTypeLastRange))
        {
            numOfRanges++;
            rangePtr = rangePtr->next;
            continue;
        }

        /* Check startAddr for the first range of the unicast/multicast prefixes */
        if ((numOfRangesForPrefixType == 0) && (rangePtr->startAddr != prefixTypeFirstRange))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Range",
                                            rangePtr,
                                            "start address of the first range is not 0");
        }

        /* In compress 2, startAddr of the 5th range (actually 6th because the
           first is always zero) must be equal to bucketPtr->fifthAddress */
        if ((bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E) &&
            (numOfRanges == 5) && (rangePtr->startAddr != bucketPtr->fifthAddress))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Range",
                                            rangePtr,
                                            "start address of the fifth range is different than the bucket fifthAddress");
        }

        nextRangePtr = rangePtr->next;
        /* Ranges must be in increasing order */
        if ((nextRangePtr) && (nextRangePtr->startAddr <= rangePtr->startAddr))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Range",
                                            rangePtr,
                                            "the ranges are not in increasing order");
        }

        /* A range that points to bucket must be of length 1 (its start address
           must be equal to its end address) */
        if ((rangePtr->startAddr < 255) &&
            (rangePtr->pointerType <= CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E))
        {
            if (nextRangePtr == NULL)
            {
                /* no next range */
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Range",
                                                rangePtr,
                                                "the range points to bucket but its length is not 1");
            }
            else
            {
                if (rangePtr->startAddr != (nextRangePtr->startAddr - 1))
                {
                    LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Range",
                                                    rangePtr,
                                                    "the range points to bucket but its length is not 1");
                }
            }
        }

        if ((rangePtr->pointerType > CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) &&
            ((GT_U32)rangePtr->pointerType != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Level",
                                            rangePtr,
                                            "The range points to an unknown pointer type");
        }

        /* check that tree level is not over the limit */
        if ((((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) && (level == (2*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS))) ||
             ((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) && (level == (2*PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS)))) &&
            (prefixType == CPSS_MULTICAST_E))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Level",
                                                rangePtr,
                                                "MC tree reached an illegal level");
        }
        else
        {
            if ((((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) && (level == PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS)) ||
                 ((protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E) && (level == PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS))) &&
                (prefixType == CPSS_UNICAST_E))
            {
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Level",
                                                    rangePtr,
                                                    "UC tree reached an illegal level");
            }
        }

        /* Check validity for nexthop */
        if ((rangePtr->pointerType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) ||
            (rangePtr->pointerType == CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) ||
            (rangePtr->pointerType == CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
        {
            status = prvCpssDxChLpmRamDbgRouteEntryShadowValidityCheck(shadowPtr,
                                                                       rangePtr->lowerLpmPtr.nextHopEntry,
                                                                       returnOnFailure);
            if (status != GT_OK)
            {
                retVal = status;
                if (returnOnFailure == GT_TRUE)
                {
                    return retVal;
                }
            }
        }

        /* The range was possibly found in the trie. */
        trieRangeIdx = rangePtr->startAddr;
        if (validRangeInTrieArray[trieRangeIdx] == GT_TRUE)
        {
            if (rangePtr->mask != maskForRangeInTrieArray[trieRangeIdx])
            {
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Range",
                                                rangePtr,
                                                "the range has a different mask in the trie");
            }
            validRangeInTrieArray[trieRangeIdx] = GT_FALSE;
        }
        numOfRanges++;
        numOfRangesForPrefixType++;
        rangePtr = nextRangePtr;
    }

    if (numOfRanges != bucketPtr->numOfRanges)
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,
                                        "the number of actual ranges is different than the bucket's numOfRanges");
    }

    /* Check if all the ranges in the trie exist in the bucket ranges list */
    for (trieRangeIdx = 0; trieRangeIdx < 255; trieRangeIdx++)
    {
        if ((trieRangeIdx < prefixTypeFirstRange) || (trieRangeIdx > prefixTypeLastRange))
        {
            continue;
        }
        if (validRangeInTrieArray[trieRangeIdx] == GT_TRUE)
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,
                                            "a range that appears in the trie does not appear in the ranges list");
        }
    }

    /* Check next buckets */
    rangePtr = bucketPtr->rangeList;
    if (prefixType == CPSS_UNICAST_E)
    {
        bucketBaseAddr =
            shadowPtr->ucSearchMemArrayPtr[protocol][level]->structsBase +
            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
    }
    else
    {
        bucketBaseAddr =
            shadowPtr->mcSearchMemArrayPtr[protocol][level]->structsBase +
            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
    }

    /* number of LPM lines for the bitmap */
    if (bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
    {
        rangeOffset = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
    }
    else if (bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
    {
        rangeOffset = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS;
    }
    else    /* CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E */
    {
        rangeOffset = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;
    }

    while (rangePtr)
    {
        if ((rangePtr->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
            (rangePtr->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
            (rangePtr->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E))
        {
            if ((rangePtr->startAddr < prefixTypeFirstRange) ||
                (rangePtr->startAddr > prefixTypeLastRange))
            {
                rangePtr = rangePtr->next;
                rangeOffset++;
                continue;
            }
            expectedPointingRangeMemAddr = bucketBaseAddr + rangeOffset;
            status = prvCpssDxChLpmRamDbgBucketShadowValidityCheck(shadowPtr,
                                                                   vrId,
                                                                   rangePtr->lowerLpmPtr.nextBucket,
                                                                   (GT_U8)(level + 1),
                                                                   rangePtr->pointerType,
                                                                   numOfMaxAllowedLevels,
                                                                   prefixType,
                                                                   protocol,
                                                                   expectedPointingRangeMemAddr,
                                                                   returnOnFailure);
            if (status != GT_OK)
            {
                retVal = status;
                if (returnOnFailure == GT_TRUE)
                {
                    return retVal;
                }
            }
        }
        rangePtr = rangePtr->next;
        rangeOffset++;
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgBucketShadowValidityCheck
*
* DESCRIPTION:
*       Shadow validation of a bucket
*
* INPUTS:
*       shadowPtr                    - pointer to shadow structure
*       vrId                         - the virtual router id
*       bucketPtr                    - pointer to the bucket
*       level                        - the level in the tree (first level is 0)
*       expectedNextPointerType      - the expected type of the bucket, ignored
*                                      for level 0
*       numOfMaxAllowedLevels        - the maximal allowed number of levels
*                                      for the relevant protocol and prefix type
*       prefixType                   - unicast or multicast tree
*       protocol                     - protocol
*       expectedPointingRangeMemAddr - the expected pointingRangeMemAddr field
*                                      of the bucket
*       returnOnFailure              - GT_TRUE: the validation check stops on
*                                      first failure
*                                      GT_FALSE: continue with the test on
*                                      failure
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_FAIL - on failure
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamDbgBucketShadowValidityCheck
(
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  GT_U32                                    vrId,
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  GT_U8                                     level,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT       expectedNextPointerType,
    IN  GT_U8                                     numOfMaxAllowedLevels,
    IN  CPSS_UNICAST_MULTICAST_ENT                prefixType,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol,
    IN  GT_U32                                    expectedPointingRangeMemAddr,
    IN  GT_BOOL                                   returnOnFailure
)
{
    GT_U32    swapBaseAddr, memSize, baseAddrOfMemBlock;
    GT_U32    beginningOfBucket, endOfBucket;
    GT_STATUS status, retVal = GT_OK;

    /* the mask as retrieved from the trie. The array index is the range start
       address */
    GT_U8   maskForRangeInTrieArray[256] = {0};
    /* indicates whether the range was found during the trie scan. The array
       index is the range start address.
       For each range that is found during the trie scan, the value in the array
       will be updated to GT_TRUE. After that, we will go over the bucket ranges
       and check that all the ranges with GT_TRUE are found */
    GT_BOOL validRangeInTrieArray[256] = {0};

    /* make compiler silent */
    swapBaseAddr = 0;

    /* Check that we don't exceed the number of allowed levels */
    if (level >= numOfMaxAllowedLevels)
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,
                                        "exceeded max number of levels");
    }

    /* Check that bucketType and pointerType of the pointing range are the same.
       Exception: bucketType of a MC source is different than pointerType of the
       pointing range */
    if ((expectedNextPointerType != bucketPtr->bucketType) &&
        ((GT_U32)expectedNextPointerType != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,
                                        "bucket type is different than the pointing range type");
    }

    /* Check validity of bucketType and compatibility with number of ranges */
    switch (bucketPtr->bucketType)
    {
    case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
        /* The root can't be compressed 2 (HW limitation) */
        if (level == 0)
        {
            if ((bucketPtr->numOfRanges < 6) || (bucketPtr->numOfRanges > 256))
            {
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                bucketPtr,
                                                "bucket type is not compatible with the number of ranges");
            }
        }
        else
        {
            if ((bucketPtr->numOfRanges < 11) || (bucketPtr->numOfRanges > 256))
            {
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                bucketPtr,
                                                "bucket type is not compatible with the number of ranges");
            }
        }
        break;

    case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
        if (level == 0)
        {
            /* The root bucket can contain one range */
            if ((bucketPtr->numOfRanges < 1) || (bucketPtr->numOfRanges > 5))
            {
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                bucketPtr,
                                                "bucket type is not compatible with the number of ranges");
            }
        }
        else
        {
            if ((bucketPtr->numOfRanges < 2) || (bucketPtr->numOfRanges > 5))
            {
                LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                bucketPtr,
                                                "bucket type is not compatible with the number of ranges");
            }
        }
        break;

    case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
        /* The root can't be compressed 2 (HW limitation) */
        if (level == 0)
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,
                                            "root bucket type is compressed 2");
        }
        if ((bucketPtr->numOfRanges < 6) || (bucketPtr->numOfRanges > 10))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,
                                            "bucket type is not compatible with the number of ranges");
        }
        break;

    case CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E:
        if ((prefixType != CPSS_MULTICAST_E) ||
            ((GT_U32)expectedNextPointerType != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,
                                            "invalid bucket type");
        }
        break;

    default:
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,
                                        "invalid bucket type");
    }

    if (bucketPtr->bucketType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
    {
        /* Check hwBucketOffsetHandle */
        if (prefixType == CPSS_UNICAST_E)
        {
            baseAddrOfMemBlock = shadowPtr->ucSearchMemArrayPtr[protocol][level]->structsBase;
        }
        else
        {
            baseAddrOfMemBlock = shadowPtr->mcSearchMemArrayPtr[protocol][level]->structsBase;
        }
        swapBaseAddr = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(shadowPtr->swapMemoryAddr);

        memSize = shadowPtr->lpmRamTotalBlocksSizeIncludingGap * shadowPtr->numOfLpmMemories;

        /* the beginning and the end of the bucket in HW */
        beginningOfBucket =
            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
        endOfBucket =
            beginningOfBucket +
            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_SIZE_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
        /* Check that hwBucketOffsetHandle is in range */
        if (endOfBucket > memSize)
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,
                                            "hwBucketOffsetHandle is out of range");
        }

        /* Check that hwBucketOffsetHandle is not within the swapping area */
        memSize = PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS;
        beginningOfBucket += baseAddrOfMemBlock;
        endOfBucket += baseAddrOfMemBlock;
        if (((swapBaseAddr < beginningOfBucket) && (beginningOfBucket < swapBaseAddr + memSize)) ||
            ((swapBaseAddr < endOfBucket) && (endOfBucket < swapBaseAddr + memSize)))
        {
            LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,
                                            "hwBucketOffsetHandle is within the swapping area");
        }
    }
    /* Check pointingRangeMemAddr */
    if (bucketPtr->pointingRangeMemAddr != expectedPointingRangeMemAddr)
    {
        LPM_SHADOW_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,
                                        "pointingRangeMemAddr is wrong");
    }
    /* Trie validity check. It also fills maskForRangeInTrieArray and
       validRangeInTrieArray. They will be used later to verify that all the
       ranges that can be retrieved from the trie exist in the bucket */
    status = prvCpssDxChLpmRamDbgTrieShadowValidityCheck(&bucketPtr->trieRoot,
                                                         0,
                                                         0,
                                                         0,
                                                         returnOnFailure,
                                                         maskForRangeInTrieArray,
                                                         validRangeInTrieArray);
    if (status != GT_OK)
    {
        retVal = status;
        if (returnOnFailure == GT_TRUE)
        {
            return retVal;
        }
    }

    if (bucketPtr->bucketType == CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
    {
        /* This is a root of MC source tree that points directly to a nexthop or
           ECMP/QoS entry */
        status = prvCpssDxChLpmRamDbgRouteEntryShadowValidityCheck(shadowPtr,
                                                                   bucketPtr->rangeList->lowerLpmPtr.nextHopEntry,
                                                                   returnOnFailure);
    }
    else
    {
        /* Ranges validity check */
        status = prvCpssDxChLpmRamDbgRangesShadowValidityCheck(shadowPtr,
                                                               vrId,
                                                               bucketPtr,
                                                               maskForRangeInTrieArray,
                                                               validRangeInTrieArray,
                                                               level,
                                                               numOfMaxAllowedLevels,
                                                               prefixType,
                                                               protocol,
                                                               returnOnFailure);
    }

    if (status != GT_OK)
    {
        retVal = status;
        if (returnOnFailure == GT_TRUE)
        {
            return retVal;
        }
    }
    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgShadowValidityCheck
*
* DESCRIPTION:
*       Validation function for the LPM shadow
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDbPtr        - LPM DB
*       vrId            - virtual router id, 4096 means "all vrIds"
*       protocolBitmap  - protocols bitmap
*       prefixType      - UC/MC/both prefix type
*       returnOnFailure - GT_TRUE: the validation check stops at first failure
*                         GT_FALSE: continue with the test on failure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal input parameter/s
*       GT_NOT_FOUND             - LPM DB was not found
*       GT_NOT_INITIALIZED       - LPM DB is not initialized
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbgShadowValidityCheck
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC     *lpmDbPtr,
    IN GT_U32                                   vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_BMP           protocolBitmap,
    IN CPSS_UNICAST_MULTICAST_ENT               prefixType,
    IN GT_BOOL                                  returnOnFailure
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT      bucketType;
    GT_U32                                   shadowIdx, vrIdStartIdx, vrIdEndIdx;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr;
    GT_U8                                    numOfMaxAllowedLevels;
    GT_STATUS                                status, rc = GT_OK;
    GT_U32                                   blockIndex=0,numOfOctetsInProtocol=0,octetIndex=0;
    PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT     protocolChecked;
    GT_BOOL                                  blockIsUsedByAnOctetOfThisProtocol;

    switch(prefixType)
    {
        case CPSS_UNICAST_E:
        case CPSS_MULTICAST_E:
        case CPSS_UNICAST_MULTICAST_E:
            break;

        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap) == GT_FALSE) &&
        (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap) == GT_FALSE) &&
        (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* FCoE is unicast only */
    if ((PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap)) &&
         (prefixType != CPSS_UNICAST_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* if initialization has not been done for the requested protocol stack -
       return error */
    if ((PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap)) &&
        (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if ((PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap)) &&
             (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if ((PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap)) &&
             (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        if ((vrId >= shadowPtr->vrfTblSize) && (vrId != 4096))
        {
            /* Illegal virtual router id */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap))
        {
            if (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }
        if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap))
        {
            if (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }
        if (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap))
        {
            if (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E] == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }

        /* check if the struct holding block to octet mapping fit the sharing mode */
        if(shadowPtr->lpmRamBlocksAllocationMethod==PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E)
        {
            for (blockIndex=0;blockIndex<shadowPtr->numOfLpmMemories;blockIndex++)
            {
                /* the block is not used at all, so no need to check sharing violation */
                if (shadowPtr->lpmRamOctetsToBlockMappingPtr[blockIndex].isBlockUsed==GT_FALSE)
                {
                    continue;
                }

                for (protocolChecked=PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E;
                     protocolChecked<PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E;
                     protocolChecked++)
                {
                    switch (protocolChecked)
                    {
                        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E:
                            if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap)==GT_FALSE)
                                continue;
                            numOfOctetsInProtocol = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
                            break;
                        case PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E:
                            if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap)==GT_FALSE)
                                continue;
                            numOfOctetsInProtocol = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
                            break;
                        case PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E:
                            if (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap)==GT_FALSE)
                                continue;
                            numOfOctetsInProtocol = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS;
                            break;
                        default:
                            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_VALUE, LOG_ERROR_NO_MSG);
                    }

                    /* the block is not used by the protocol,so no need to check sharing violation */
                    if (PRV_CPSS_DXCH_LPM_RAM_IS_BLOCK_USED_BY_PROTOCOL_GET_MAC(shadowPtr,protocolChecked,blockIndex)==GT_FALSE)
                    {
                        continue;
                    }
                    blockIsUsedByAnOctetOfThisProtocol=GT_FALSE;
                    for (octetIndex = 0; octetIndex < numOfOctetsInProtocol; octetIndex++)
                    {
                        if (PRV_CPSS_DXCH_LPM_RAM_IS_BLOCK_USED_BY_OCTET_IN_PROTOCOL_GET_MAC(shadowPtr,protocolChecked,octetIndex,blockIndex)==GT_TRUE)
                        {
                            if (blockIsUsedByAnOctetOfThisProtocol==GT_FALSE)
                            {
                                blockIsUsedByAnOctetOfThisProtocol=GT_TRUE;
                            }
                            else
                            {
                                /* if blockIsUsedByAnOctetOfThisProtocol is already true
                                   it means we have more than one bit up in the bitmap,
                                   meaning more than one octet sharing this block */
                                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_FAIL, LOG_ERROR_NO_MSG);
                            }
                        }
                    }
                }
            }
        }

        if (vrId == 4096)
        {
            vrIdStartIdx = 0;
            vrIdEndIdx = shadowPtr->vrfTblSize - 1;
        }
        else
        {
            vrIdStartIdx = vrIdEndIdx = vrId;
        }

        for (vrId = vrIdStartIdx; vrId <= vrIdEndIdx; vrId++)
        {
            if (shadowPtr->vrRootBucketArray[vrId].valid == 0)
            {
                continue;
            }

            if ((prefixType == CPSS_UNICAST_E) || (prefixType == CPSS_UNICAST_MULTICAST_E))
            {
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    numOfMaxAllowedLevels = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
                    status = prvCpssDxChLpmRamDbgBucketShadowValidityCheck(shadowPtr,
                                                                           vrId,
                                                                           bucketPtr,
                                                                           0,
                                                                           bucketType,
                                                                           numOfMaxAllowedLevels,
                                                                           CPSS_UNICAST_E,
                                                                           PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                                           0,
                                                                           returnOnFailure);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    numOfMaxAllowedLevels = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
                    status = prvCpssDxChLpmRamDbgBucketShadowValidityCheck(shadowPtr,
                                                                           vrId,
                                                                           bucketPtr,
                                                                           0,
                                                                           bucketType,
                                                                           numOfMaxAllowedLevels,
                                                                           CPSS_UNICAST_E,
                                                                           PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                                           0,
                                                                           returnOnFailure);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E];
                    numOfMaxAllowedLevels = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS;
                    status = prvCpssDxChLpmRamDbgBucketShadowValidityCheck(shadowPtr,
                                                                           vrId,
                                                                           bucketPtr,
                                                                           0,
                                                                           bucketType,
                                                                           numOfMaxAllowedLevels,
                                                                           CPSS_UNICAST_E,
                                                                           PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E,
                                                                           0,
                                                                           returnOnFailure);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
            }
            if ((prefixType == CPSS_MULTICAST_E) || (prefixType == CPSS_UNICAST_MULTICAST_E))
            {
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    numOfMaxAllowedLevels = 8;
                    status = prvCpssDxChLpmRamDbgBucketShadowValidityCheck(shadowPtr,
                                                                           vrId,
                                                                           bucketPtr,
                                                                           0,
                                                                           bucketType,
                                                                           numOfMaxAllowedLevels,
                                                                           CPSS_MULTICAST_E,
                                                                           PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                                           0,
                                                                           returnOnFailure);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    numOfMaxAllowedLevels =  (2 * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
                    status = prvCpssDxChLpmRamDbgBucketShadowValidityCheck(shadowPtr,
                                                                           vrId,
                                                                           bucketPtr,
                                                                           0,
                                                                           bucketType,
                                                                           numOfMaxAllowedLevels,
                                                                           CPSS_MULTICAST_E,
                                                                           PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                                           0,
                                                                           returnOnFailure);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgRangesHwShadowSyncValidityCheck
*
* DESCRIPTION:
*      Shadow and HW synchronization validation of bucket's ranges
*
* INPUTS:
*       devNum                    - The device number
*       shadowPtr                 - pointer to shadow structure
*       vrId                      - the virtual router id
*       bucketPtr                 - pointer to the bucket
*       maskForRangeInTrieArray   - array of the ranges masks as found in the
*                                   trie. The array index represents the range
*                                   start address
*       validRangeInTrieArray     - array to indicate if a range was found in
*                                   the trie. The array index represents the
*                                   range start address.
*       level                     - the level in the tree (first level is 0)
*       numOfMaxAllowedLevels     - the maximal number of levels that is allowed
*                                   for the relevant protocol and prefix type
*       prefixType                - unicast or multicast tree
*       protocol                  - protocol
*       returnOnFailure           - GT_TRUE: the validation check stops on
*                                            first failure
*                                   GT_FALSE: continue with the test on failure
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_FAIL - on failure
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamDbgRangesHwShadowSyncValidityCheck
(
    IN  GT_U8                                     devNum,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  GT_U32                                    vrId,
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  GT_U8                                     level,
    IN  GT_U8                                     numOfMaxAllowedLevels,
    IN  CPSS_UNICAST_MULTICAST_ENT                prefixType,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol,
    IN  GT_BOOL                                   returnOnFailure
)
{
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC  *rangePtr;
    GT_U32                                  bucketBaseAddr, rangeOffset,
                                            expectedPointingRangeMemAddr;
    GT_U8                                   prefixTypeFirstRange, prefixTypeLastRange;
    GT_U32                                  tmpPrefixTypeRange;
    GT_STATUS                               status, retVal = GT_OK;

    rangePtr = bucketPtr->rangeList;
    if (level == 0)
    {
        if (prefixType == CPSS_UNICAST_E)
        {
            prefixTypeFirstRange = 0;
            tmpPrefixTypeRange = (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
                (PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS - 1) :
                (PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS - 1);
            prefixTypeLastRange = (GT_U8)tmpPrefixTypeRange;
        }
        else /* CPSS_MULTICAST_E */
        {
            tmpPrefixTypeRange = (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
                PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV4_MC_ADDRESS_SPACE_CNS :
                PRV_CPSS_DXCH_LPM_RAM_START_OF_IPV6_MC_ADDRESS_SPACE_CNS;
            prefixTypeFirstRange = (GT_U8)tmpPrefixTypeRange;
            tmpPrefixTypeRange = (protocol == PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E) ?
                PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV4_MC_ADDRESS_SPACE_CNS :
                PRV_CPSS_DXCH_LPM_RAM_END_OF_IPV6_MC_ADDRESS_SPACE_CNS;
            prefixTypeLastRange = (GT_U8)tmpPrefixTypeRange;
        }
    }
    else
    {
        prefixTypeFirstRange = 0;
        prefixTypeLastRange = 255;
    }

    /* Check next buckets */
    rangePtr = bucketPtr->rangeList;
    if (prefixType == CPSS_UNICAST_E)
    {
        bucketBaseAddr =
            shadowPtr->ucSearchMemArrayPtr[protocol][level]->structsBase +
            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
    }
    else
    {
        bucketBaseAddr =
            shadowPtr->mcSearchMemArrayPtr[protocol][level]->structsBase +
            PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
    }

    /* number of LPM lines for the bitmap */
    if (bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
    {
        rangeOffset = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
    }
    else if (bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
    {
        rangeOffset = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS;
    }
    else    /* CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E */
    {
        rangeOffset = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;
    }

    while (rangePtr)
    {
        if ((rangePtr->pointerType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E) &&
            (rangePtr->pointerType != CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E) &&
            (rangePtr->pointerType != CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E) &&
            (!(((GT_U32)rangePtr->pointerType==(GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E) &&
             (rangePtr->lowerLpmPtr.nextBucket->bucketType==CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E))))
        {
            if ((rangePtr->startAddr < prefixTypeFirstRange) ||
                (rangePtr->startAddr > prefixTypeLastRange))
            {
                rangePtr = rangePtr->next;
                rangeOffset++;
                continue;
            }
            expectedPointingRangeMemAddr = bucketBaseAddr + rangeOffset;
            status = prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck(devNum,
                                                                 shadowPtr,
                                                                 vrId,
                                                                 rangePtr->lowerLpmPtr.nextBucket,
                                                                 (GT_U8)(level + 1),
                                                                 rangePtr->pointerType,
                                                                 numOfMaxAllowedLevels,
                                                                 prefixType,
                                                                 protocol,
                                                                 expectedPointingRangeMemAddr,
                                                                 returnOnFailure,
                                                                 GT_FALSE/*not the root bucket*/);
            if (status != GT_OK)
            {
                retVal = status;
                if (returnOnFailure == GT_TRUE)
                {
                    return retVal;
                }
            }
        }
        else
        {
            /* in case of NextHop pointers no need to do anything since
               the SW and HW NextHop synchronization is checked in
               prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck */
        }
        rangePtr = rangePtr->next;
        rangeOffset++;
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck
*
* DESCRIPTION:
*       Shadow and HW synchronization validation of a bucket
*
* INPUTS:
*       devNum                       - The device number
*       shadowPtr                    - pointer to shadow structure
*       vrId                         - the virtual router id
*       bucketPtr                    - pointer to the bucket
*       level                        - the level in the tree (first level is 0)
*       expectedNextPointerType      - the expected type of the bucket, ignored
*                                      for level 0
*       numOfMaxAllowedLevels        - the maximal allowed number of levels
*                                      for the relevant protocol and prefix type
*       prefixType                   - unicast or multicast tree
*       protocol                     - protocol
*       expectedPointingRangeMemAddr - the expected pointingRangeMemAddr field
*                                      of the bucket
*       returnOnFailure              - GT_TRUE: the validation check stops on
*                                      first failure
*                                      GT_FALSE: continue with the test on
*                                      failure
*       isRootBucket                - GT_TRUE:the bucketPtr is the root bucket
*                                     GT_FALSE:the bucketPtr is not the root bucket
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       GT_FAIL - on failure
*       GT_OK   - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck
(
    IN  GT_U8                                     devNum,
    IN  PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC          *shadowPtr,
    IN  GT_U32                                    vrId,
    IN  PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC   *bucketPtr,
    IN  GT_U8                                     level,
    IN  CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT       expectedNextPointerType,
    IN  GT_U8                                     numOfMaxAllowedLevels,
    IN  CPSS_UNICAST_MULTICAST_ENT                prefixType,
    IN  PRV_CPSS_DXCH_LPM_PROTOCOL_STACK_ENT      protocol,
    IN  GT_U32                                    expectedPointingRangeMemAddr,
    IN  GT_BOOL                                   returnOnFailure,
    IN  GT_BOOL                                   isRootBucket
)
{
    GT_STATUS status, retVal = GT_OK;
    PRV_CPSS_DXCH_LPM_RAM_RANGE_SHADOW_STC *tempRangePtr;/* Current range pointer.   */
    GT_U32    hwNumOfRanges;   /* the number of ranges in the HW will be calculated according to the bit vector*/
    GT_U32    hwRanges[256];   /* the ranges values retrieved from the HW */
    GT_U32    *hwBucketDataPtr;/* pointer to all the data retrieved for HW for a specific bucket */
    GT_U32    hwRangeData=0;   /* one range line retrieved from HW */
    GT_U32    hwBitMapSize=0;  /* the size of the bitmap in HW: 1 for comp_1, 2 for comp_2, 11 for regular*/
    GT_U32    hwBitMapLine;    /* one line data from the bit map vector HW value*/
    GT_U32    hwPointerType;   /* The type of the next entry this entry points to. can be Bucket/Regular_Leaf/ECMP_Leaf/QoS_Leaf */
    GT_U32    hwNextBucketType;/* The next bucket (point to by NextBucketPointer) type. can be Regular/One_Compressed/Two_Compressed */
    GT_U32    hwNhPointer;     /* This is the next hop pointer for accessing next hop table or
                                  the pointer for the ECMP table for fetching the ECMP attributes from ECMP table */
    GT_U32    hwAddr = 0;      /* The pointer to the next bucket entry in lines */
    GT_U32    i;
    GT_BOOL   hwNeedToFreeBucketDataPtr=GT_FALSE;
    prvLpmDumpPrintInfo_STC printInfo; /* strcut holds all the printing info */
    GT_U32                              swNodeAddr = 0;/* SW node address kept in shadow(hwBucketOffsetHandle) in line representation from the base */
    GT_U32                              hwNodeAddr = 0;/* HW node address */
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT hwNodeType = 0;/* the bucketType retrieved from HW */

    if (isRootBucket==GT_TRUE)
    {
        /* 1. check that the Root SW bucketType is the same as HW bucketType */

        /* read the HW data directly from the VRF table and compare it to the SW data in the shadow */
        status = prvCpssDxChLpmHwVrfEntryRead(devNum, vrId, protocol,
                                          &hwNodeType, &hwNodeAddr);
        if (status != GT_OK)
        {
            cpssOsPrintf("Error on reading Vrf table for vrfId=%d, and protocol=%d\n", vrId, protocol);
            return status;
        }
    }
    else
    {
        /* if this is not a call to the root bucket phase, but a call done from the ranges loop in
           prvCpssDxChLpmRamDbgRangesHwShadowSyncCheck, then we need to take the data address of
           the bucket from the shadow and to retrieved all the HW data */
        if(bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
        {
            hwBitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_1_CNS;
        }
        else if(bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
        {
            hwBitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_COMPRESSED_2_CNS;
        }
        else if(bucketPtr->bucketType == CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E)
        {
            hwBitMapSize = PRV_CPSS_DXCH_LPM_RAM_BUCKET_BIT_VEC_SIZE_REGULAR_CNS;
        }
        else /* bucketType = CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E or
                             CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E or
                             CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E */
        {
            /* error - we should not get here in case of a leaf */
            cpssOsPrintf("Error - we should not get here in case of a leaf. vrfId=%d, and protocol=%d\n", vrId, protocol);
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        hwNodeAddr = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
        hwNodeType = bucketPtr->bucketType;/* we already checked the father nextPointer type to be syncronazid in SW and HW */
    }


    /* check that the HW next bucket type is the same as SW bucketType
       Exception: bucketType of a MC source is different than pointerType of the
       pointing range */
    if(((GT_U32)expectedNextPointerType != (GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E))
    {
        switch (expectedNextPointerType)
        {
            case CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E:
                if(hwNodeType!=CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E)
                    LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                    bucketPtr,hwNodeAddr,
                                                    "Error: no synchronization between HW and SW - NextPointerType\n");
                break;
            case CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E:
                if(hwNodeType!=CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E)
                    LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                    bucketPtr,hwNodeAddr,
                                                    "Error: no synchronization between HW and SW - NextPointerType\n");
                break;
            case CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E:
                /* the VR table can only point to regular or compress_1 bucket*/
                if (isRootBucket==GT_TRUE)
                {
                    LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                bucketPtr,hwNodeAddr,
                                                "invalid VR bucket type");
                }
                else
                {
                    if(hwNodeType!=CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
                        LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                    bucketPtr,hwNodeAddr,
                                                    "Error: no synchronization between HW and SW - NextPointerType\n");
                }
                break;
            case CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E:
            default:
                LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                bucketPtr,hwNodeAddr,
                                                "invalid bucket type");
        }
    }
    else
    {
        /* in case of PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E need to check HW bit 2 is set.
           In the process of (*,G) lookup.
           When an entry has this bit set, the (*, G) lookup terminates with a match
           at the current entry, and (S, G) SIP based lookup is triggered. */

        status = prvCpssDxChReadTableMultiEntry(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                             expectedPointingRangeMemAddr,
                                             1,
                                             &hwRangeData);
        if (status != GT_OK)
        {
            return status;
        }

        if(((hwRangeData >> 2) & 0x1)!=1)
        {
            LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                bucketPtr,hwNodeAddr,
                                                "invalid bucket type");
        }
    }

    if (isRootBucket==GT_TRUE)
    {
        /* 2. check that the Root SW Head Of Trie is the same as HW Head Of Trie */
        swNodeAddr = PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(bucketPtr->hwBucketOffsetHandle);
        if(swNodeAddr != hwNodeAddr)
        {
            LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,hwNodeAddr,
                                            "Error: no synchronization between HW and SW - Root Head Of Trie\n");
        }
    }
    else
    {
        /* no need to make the check since in case of non-root bucket we are assigning hwNodeAddr to be the value retrieved from the shadow
           meaning hwNodeAddr is swNodeAddr, we are doing so since we are interested to check the data of this address and not the address itself
           the checking of the address was done in previous recursive call */
    }

    cpssOsMemSet(&printInfo, 0, sizeof(prvLpmDumpPrintInfo_STC));

    printInfo.printRanges = GT_FALSE;

     /* Allocate memory for bucket data. Maximum size is regular bucket that
       uses 256 lpm lines for ranges + 11 lpm lines for bitmap */
    hwBucketDataPtr = cpssOsMalloc(PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    if (hwBucketDataPtr == NULL)
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);

    cpssOsMemSet(hwBucketDataPtr, 0, PRV_CPSS_DXCH_LPM_RAM_MAX_SIZE_OF_BUCKET_IN_LPM_LINES_CNS * sizeof(GT_U32));
    cpssOsMemSet(hwRanges, 0, sizeof(hwRanges));
    if (isRootBucket==GT_TRUE)
    {
        /* this function retrieves according to the VRF table and the LPM Memory the number of ranges, ranges values, and ranges data */
        status = getRootBucketRanges(devNum, vrId, protocol,
                                  &hwBitMapSize, &hwNumOfRanges, hwRanges,
                                  hwBucketDataPtr, &printInfo);
        if (status != GT_OK)
        {
            cpssOsFree(hwBucketDataPtr);
            return status;
        }
    }
    else
    {
        /* we are dealing with a non-root bucket */

        /* read the bit vector according to the address retrieved from the Shadow bucket */
        status = prvCpssDxChReadTableMultiEntry(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                             hwNodeAddr,
                                             hwBitMapSize,
                                             hwBucketDataPtr);
        if (status != GT_OK)
        {
            cpssOsFree(hwBucketDataPtr);
            return status;
        }

        if (bucketPtr->bucketType == CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)
        {
            /* in case of compress_2 read the 6 range where we have the "Two Compressed Last Offset" value,
               we need this data for the number of ranges calculation in getNumOfRangesFromHW.
               this value is keep in the father of this bucket */
            status = prvCpssDxChReadTableMultiEntry(devNum,
                                             PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                             expectedPointingRangeMemAddr,
                                             1,
                                             &hwRangeData);
            if (status != GT_OK)
            {
                cpssOsFree(hwBucketDataPtr);
                return status;
            }
        }

        hwNumOfRanges = getNumOfRangesFromHW(bucketPtr->bucketType, hwBucketDataPtr, hwRangeData);

        /* calculate HW ranges values according to bit vector */
        switch (bucketPtr->bucketType)
        {
            case (CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E):
                hwBitMapLine = hwBucketDataPtr[0]; /* bitmap pointer */

                /* Bitmap has 4 bytes [range_4_start ; range_3_start ; range_2_start ; range_1_start]
                   Range 0 start is always 0 (so no need to add to bitmap) */
                hwRanges[1] = hwBitMapLine & 0xFF;
                hwRanges[2] = (hwBitMapLine >> 8) & 0xFF;
                hwRanges[3] = (hwBitMapLine >> 16) & 0xFF;
                hwRanges[4] = (hwBitMapLine >> 24) & 0xFF;
                break;

            case (CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E):
                hwBitMapLine = hwBucketDataPtr[0]; /* bitmap 1 pointer */

                /* Bitmap has 9 bytes; Word 1: [range_4_start ; range_3_start ; range_2_start ; range_1_start]
                                       Word 2: [range_9_start ; range_8_start ; range_7_start ; range_6_start]
                                        - range_5_start is carried from the pointer to this bucket
                                        - range_0_start is always 0 (so no need to add to bitmap) */
                hwRanges[1] = hwBitMapLine & 0xFF;
                hwRanges[2] = (hwBitMapLine >> 8) & 0xFF;
                hwRanges[3] = (hwBitMapLine >> 16) & 0xFF;
                hwRanges[4] = (hwBitMapLine >> 24) & 0xFF;
                hwRanges[5] = (hwRangeData >> 5) & 0xFF;

                hwBitMapLine = hwBucketDataPtr[1*PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]; /* bitmap 2 pointer */

                hwRanges[6] = hwBitMapLine & 0xFF;
                hwRanges[7] = (hwBitMapLine >> 8) & 0xFF;
                hwRanges[8] = (hwBitMapLine >> 16) & 0xFF;
                hwRanges[9] = (hwBitMapLine >> 24) & 0xFF;

                break;

            case (CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E):
                for (i = 0; i < hwNumOfRanges; i++)
                {
                    hwRanges[i] = (GT_U8)getBaseAddrFromHW(CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E, i, hwBucketDataPtr, 0);
                }
                break;

            case (CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E):
            case (CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E):
            case (CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E):
            default:
                break;
        }

        /* read all the ranges (the LPM lines after the bit vector) */
        status = prvCpssDxChReadTableMultiEntry(devNum,
                                         PRV_CPSS_DXCH_LION3_TABLE_LPM_MEM_E,
                                         hwNodeAddr + hwBitMapSize, hwNumOfRanges,
                                         &hwBucketDataPtr[hwBitMapSize * PRV_CPSS_DXCH_LPM_RAM_SIZE_OF_LPM_ENTRY_IN_WORDS_CNS]);
        if (status != GT_OK)
        {
            cpssOsFree(hwBucketDataPtr);
            return status;
        }
    }

    /* if an error accur in getRootBucketRanges then bucketDataPtr was already free */
    if(numOfErrors == 0)
        hwNeedToFreeBucketDataPtr = GT_TRUE;

    /* 3. check that the SW ranges is the same as HW bitVector values */
    if(bucketPtr->numOfRanges!=hwNumOfRanges)
    {
        if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
        {
            cpssOsFree(hwBucketDataPtr);
        }
        LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,hwNodeAddr,
                                        "Error: no synchronization between HW and SW - Root num of ranges\n");
    }

    /* 4. check that the SW ranges values is the same as HW ranges values */
    tempRangePtr = bucketPtr->rangeList;
    for (i=0; (i<hwNumOfRanges && tempRangePtr!=NULL); i++)
    {
        if(tempRangePtr->startAddr != hwRanges[i])
        {
            if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
            {
                cpssOsFree(hwBucketDataPtr);
            }
            LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,hwNodeAddr,
                                        "Error: no synchronization between HW and SW - ranges values\n");
        }
        else
        {
            /* range value is synchronized between HW and SW so now check that the SW range type
               is the same as HW range type, if it is the same the compare the range pointer data */
            hwPointerType = (hwBucketDataPtr[i+hwBitMapSize]) & 0x3;
            hwNextBucketType = (hwBucketDataPtr[i+hwBitMapSize] >> 3) & 0x3;

            if(((tempRangePtr->pointerType <= CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E)&&(hwPointerType!=0))||
               ((tempRangePtr->pointerType==CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)&&(hwPointerType!=1))||
               ((tempRangePtr->pointerType==CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E)&&(hwPointerType!=2))||
               ((tempRangePtr->pointerType==CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E)&&(hwPointerType!=3)))
            {
                if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                {
                    cpssOsFree(hwBucketDataPtr);
                }
                LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,hwNodeAddr,
                                        "Error: no synchronization between HW and SW - range Pointer Type value\n");
            }
            else
            {
                /* If the pointerType is to a MC source and the MC source bucket is a regular bucket then it means that
                   this is a root of MC source tree that points directly to a nexthop or ECMP/QoS entry */
                if (((GT_U32)tempRangePtr->pointerType==(GT_U32)PRV_CPSS_DXCH_LPM_RAM_TRIE_PTR_TYPE_E) &&
                    (tempRangePtr->lowerLpmPtr.nextBucket->bucketType==CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E))
                {
                    switch (tempRangePtr->lowerLpmPtr.nextBucket->rangeList->lowerLpmPtr.nextHopEntry->routeEntryMethod)
                    {
                        case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_QOS_E:
                        case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_ECMP_E:
                            /* 5. check that if a SW ranges is of type ECMP/QOS then HW range should point to the same NH */
                            hwNhPointer = ((hwBucketDataPtr[i+hwBitMapSize] >> 7) & 0x00003FFF);
                            if (tempRangePtr->lowerLpmPtr.nextBucket->rangeList->lowerLpmPtr.nextHopEntry->routeEntryBaseMemAddr != hwNhPointer)
                            {
                                if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                                {
                                    cpssOsFree(hwBucketDataPtr);
                                }
                                LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,hwNodeAddr,
                                            "Error: no synchronization between HW and SW - Pointer value\n");
                            }
                            break;
                        case PRV_CPSS_DXCH_LPM_ENTRY_TYPE_REGULAR_E:
                            /* 5. check that if a SW ranges is of type NH then HW range should point to the same NH */
                            hwNhPointer = ((hwBucketDataPtr[i+hwBitMapSize] >> 7) & 0x00007FFF);
                            if (tempRangePtr->lowerLpmPtr.nextBucket->rangeList->lowerLpmPtr.nextHopEntry->routeEntryBaseMemAddr != hwNhPointer)
                            {
                                if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                                {
                                    cpssOsFree(hwBucketDataPtr);
                                }
                                LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,hwNodeAddr,
                                            "Error: no synchronization between HW and SW - Pointer value\n");
                            }
                            break;
                        default:
                            if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                            {
                                cpssOsFree(hwBucketDataPtr);
                            }
                            LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,hwNodeAddr,
                                            "Error: no synchronization between HW and SW - error in routeEntryMethod \n");
                            break;
                    }
                }
                else
                {
                    switch (hwPointerType)
                    {
                        case 0:/* pointing to bucket */
                            /* check the bucket type: regular/comp_1/comp_2 */
                            switch(hwNextBucketType)
                            {
                                case 0: /* CPSS_DXCH_LPM_REGULAR_NODE_PTR_TYPE_E */
                                case 1: /* CPSS_DXCH_LPM_COMPRESSED_1_NODE_PTR_TYPE_E */
                                     /* hw pointer in LPM entry is in LPM lines*/
                                    hwAddr = ((hwBucketDataPtr[i+hwBitMapSize] & 0x1FFFFFE0) >> 5);
                                    break;
                                case 2: /* CPSS_DXCH_LPM_COMPRESSED_2_NODE_PTR_TYPE_E */
                                    hwAddr = (hwBucketDataPtr[i+hwBitMapSize] >> 13);
                                    break;
                                default:
                                    if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                                    {
                                        cpssOsFree(hwBucketDataPtr);
                                    }
                                    LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                        bucketPtr,hwNodeAddr,
                                                        "Error: no synchronization between HW and SW - error in Next Bucket Pointer Type \n");
                            }
                            /* check the bucket address */
                            if (hwAddr!=PRV_CPSS_DXCH_LPM_RAM_GET_LPM_OFFSET_FROM_DMM_MAC(tempRangePtr->lowerLpmPtr.nextBucket->hwBucketOffsetHandle))
                            {
                                if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                                {
                                    cpssOsFree(hwBucketDataPtr);
                                }
                                LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                                        bucketPtr,hwNodeAddr,
                                                        "Error: no synchronization between HW and SW - error in Next Bucket Pointer Adress\n");
                            }
                            break;
                        case 0x1:/* CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E */
                            /* 5. check that if a SW ranges is of type NH then HW range should point to the same NH */
                            hwNhPointer = ((hwBucketDataPtr[i+hwBitMapSize] >> 7) & 0x00007FFF);
                            if (tempRangePtr->lowerLpmPtr.nextHopEntry->routeEntryBaseMemAddr != hwNhPointer)
                            {
                                if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                                {
                                    cpssOsFree(hwBucketDataPtr);
                                }
                                LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,hwNodeAddr,
                                            "Error: no synchronization between HW and SW - Pointer value\n");
                            }
                            break;
                        case 0x2:/* CPSS_DXCH_LPM_ECMP_ENTRY_PTR_TYPE_E */
                        case 0x3:/* CPSS_DXCH_LPM_QOS_ENTRY_PTR_TYPE_E */

                            /* 5. check that if a SW ranges is of type ECMP/QOS then HW range should point to the same NH */
                            hwNhPointer = ((hwBucketDataPtr[i+hwBitMapSize] >> 7) & 0x00003FFF);
                            if (tempRangePtr->lowerLpmPtr.nextHopEntry->routeEntryBaseMemAddr != hwNhPointer)
                            {
                                if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                                {
                                    cpssOsFree(hwBucketDataPtr);
                                }
                                LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,hwNodeAddr,
                                            "Error: no synchronization between HW and SW - Pointer value\n");
                            }
                            break;
                        default:
                            if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
                            {
                                cpssOsFree(hwBucketDataPtr);
                            }
                            LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                            bucketPtr,hwNodeAddr,
                                            "Error: no synchronization between HW and SW - error in HW Bucket Pointer Type\n");
                            break;

                    }
                }

                if (tempRangePtr->next != NULL)
                    tempRangePtr = tempRangePtr->next;
                else
                    break;
            }
        }
    }
    if(i != (hwNumOfRanges-1))
    {
        if ((returnOnFailure == GT_TRUE)&&(hwNeedToFreeBucketDataPtr == GT_TRUE))
        {
            cpssOsFree(hwBucketDataPtr);
        }
        LPM_SHADOW_HW_SYNC_VALIDATION_ERROR_MAC(returnOnFailure, retVal, "Bucket",
                                        bucketPtr,hwNodeAddr,
                                        "Error: no synchronization between HW and SW - ranges values\n");
    }

    /* 6. if the SW range point to a next bucket then recursively check the new bucket, stages 1-5  */
    if (bucketPtr->bucketType != CPSS_DXCH_LPM_ROUTE_ENTRY_NEXT_PTR_TYPE_E)
    {
        /* Ranges validity check */
        status = prvCpssDxChLpmRamDbgRangesHwShadowSyncValidityCheck(devNum,
                                                             shadowPtr,
                                                             vrId,
                                                             bucketPtr,
                                                             level,
                                                             numOfMaxAllowedLevels,
                                                             prefixType,
                                                             protocol,
                                                             returnOnFailure);
    }
    if (status != GT_OK)
    {
        retVal = status;
        if (returnOnFailure == GT_TRUE)
        {
            if(hwNeedToFreeBucketDataPtr == GT_TRUE)
            {
                cpssOsFree(hwBucketDataPtr);
            }
            return retVal;
        }
    }

    if(hwNeedToFreeBucketDataPtr == GT_TRUE)
    {
        cpssOsFree(hwBucketDataPtr);
    }

    return retVal;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgHwShadowSyncValidityCheck
*
* DESCRIPTION:
*   This function validates synchronization between the SW and HW of the LPM
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - The device number
*       lpmDbPtr        - LPM DB
*       vrId            - virtual router id, 256 means "all vrIds"
*       protocolBitmap  - protocols bitmap
*       prefixType      - UC/MC/both prefix type
*       returnOnFailure - GT_TRUE: the validation check stops at first failure
*                         GT_FALSE: continue with the test on failure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal input parameter/s
*       GT_NOT_FOUND             - LPM DB was not found
*       GT_NOT_INITIALIZED       - LPM DB is not initialized
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbgHwShadowSyncValidityCheck
(
    IN GT_U8                                    devNum,
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC     *lpmDbPtr,
    IN GT_U32                                   vrId,
    IN PRV_CPSS_DXCH_LPM_PROTOCOL_BMP           protocolBitmap,
    IN CPSS_UNICAST_MULTICAST_ENT               prefixType,
    IN GT_BOOL                                  returnOnFailure
)
{
    PRV_CPSS_DXCH_LPM_RAM_BUCKET_SHADOW_STC  *bucketPtr;
    CPSS_DXCH_LPM_NEXT_POINTER_TYPE_ENT      bucketType;
    GT_U32                                   shadowIdx, vrIdStartIdx, vrIdEndIdx;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC         *shadowPtr;
    GT_U8                                    numOfMaxAllowedLevels;
    GT_STATUS                                status, rc = GT_OK;

    switch(prefixType)
    {
        case CPSS_UNICAST_E:
        case CPSS_MULTICAST_E:
        case CPSS_UNICAST_MULTICAST_E:
            break;
        default:
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    if ((PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap) == GT_FALSE) &&
        (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap) == GT_FALSE) &&
        (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* FCoE is unicast only */
    if ((PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap)) &&
         (prefixType != CPSS_UNICAST_E))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    /* if initialization has not been done for the requested protocol stack -
       return error */
    if ((PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap)) &&
        (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if ((PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap)) &&
             (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }
    else if ((PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap)) &&
             (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(lpmDbPtr->protocolBitmap) == GT_FALSE))
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
    }

    for (shadowIdx = 0; shadowIdx < lpmDbPtr->numOfShadowCfg; shadowIdx++)
    {
        shadowPtr = &lpmDbPtr->shadowArray[shadowIdx];
        if ((vrId >= shadowPtr->vrfTblSize) && (vrId != 4096))
        {
            /* Illegal virtual router id */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }
        if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap))
        {
            if (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }
        if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap))
        {
            if (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }
        if (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap))
        {
            if (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E] == GT_FALSE)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_INITIALIZED, LOG_ERROR_NO_MSG);
            }
        }

        if (vrId == 4096)
        {
            vrIdStartIdx = 0;
            vrIdEndIdx = shadowPtr->vrfTblSize - 1;
        }
        else
        {
            vrIdStartIdx = vrIdEndIdx = vrId;
        }

        for (vrId = vrIdStartIdx; vrId <= vrIdEndIdx; vrId++)
        {
            if (shadowPtr->vrRootBucketArray[vrId].valid == 0)
            {
                continue;
            }

            if ((prefixType == CPSS_UNICAST_E) || (prefixType == CPSS_UNICAST_MULTICAST_E))
            {
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    numOfMaxAllowedLevels = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV4_PROTOCOL_CNS;
                    status = prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck(devNum,
                                                                         shadowPtr,
                                                                         vrId,
                                                                         bucketPtr,
                                                                         0,
                                                                         bucketType,
                                                                         numOfMaxAllowedLevels,
                                                                         CPSS_UNICAST_E,
                                                                         PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                                         0,
                                                                         returnOnFailure,
                                                                         GT_TRUE);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    numOfMaxAllowedLevels = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_IPV6_PROTOCOL_CNS;
                    status = prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck(devNum,
                                                                         shadowPtr,
                                                                         vrId,
                                                                         bucketPtr,
                                                                         0,
                                                                         bucketType,
                                                                         numOfMaxAllowedLevels,
                                                                         CPSS_UNICAST_E,
                                                                         PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                                         0,
                                                                         returnOnFailure,
                                                                         GT_TRUE);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E];
                    numOfMaxAllowedLevels = PRV_CPSS_DXCH_LPM_NUM_OF_OCTETS_IN_FCOE_PROTOCOL_CNS;
                    status = prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck(devNum,
                                                                         shadowPtr,
                                                                         vrId,
                                                                         bucketPtr,
                                                                         0,
                                                                         bucketType,
                                                                         numOfMaxAllowedLevels,
                                                                         CPSS_UNICAST_E,
                                                                         PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E,
                                                                         0,
                                                                         returnOnFailure,
                                                                         GT_TRUE);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
            }
            if ((prefixType == CPSS_MULTICAST_E) || (prefixType == CPSS_UNICAST_MULTICAST_E))
            {
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E];
                    numOfMaxAllowedLevels = 8;
                    status = prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck(devNum,
                                                                         shadowPtr,
                                                                         vrId,
                                                                         bucketPtr,
                                                                         0,
                                                                         bucketType,
                                                                         numOfMaxAllowedLevels,
                                                                         CPSS_MULTICAST_E,
                                                                         PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E,
                                                                         0,
                                                                         returnOnFailure,
                                                                         GT_TRUE);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
                if (PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_STATUS_GET_MAC(protocolBitmap))
                {
                    bucketPtr = shadowPtr->vrRootBucketArray[vrId].rootBucket[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    bucketType = shadowPtr->vrRootBucketArray[vrId].rootBucketType[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E];
                    numOfMaxAllowedLevels =  (2 * PRV_CPSS_DXCH_LPM_MAX_NUM_OF_OCTETS_FOR_ADDRESS_CNS);
                    status = prvCpssDxChLpmRamDbgBucketHwShadowSyncValidityCheck(devNum,
                                                                         shadowPtr,
                                                                         vrId,
                                                                         bucketPtr,
                                                                         0,
                                                                         bucketType,
                                                                         numOfMaxAllowedLevels,
                                                                         CPSS_MULTICAST_E,
                                                                         PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E,
                                                                         0,
                                                                         returnOnFailure,
                                                                         GT_TRUE);
                    if (status != GT_OK)
                    {
                        rc = status;
                        if (returnOnFailure == GT_TRUE)
                        {
                            return rc;
                        }
                    }
                }
            }
        }
    }
    return rc;
}

/*******************************************************************************
* prvCpssDxChLpmRamDbgHwOctetsToBlockMappingInfoPrint
*
* DESCRIPTION:
*   Print Octet to Block mapping debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       lpmDBId               - LPM DB id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - otherwise
*       GT_NOT_FOUND             - if can't find the lpm DB
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
GT_STATUS prvCpssDxChLpmRamDbgHwOctetsToBlockMappingInfoPrint
(
    IN GT_U32                           lpmDbId
)
{
    PRV_CPSS_DXCH_LPM_SHADOW_STC        *lpmDbPtr,tmpLpmDb;
    PRV_CPSS_DXCH_LPM_HW_ENT            lpmHw;
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC    *shadowPtr;
    PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC *shadowsPtr;
    GT_U32      i,k;

    tmpLpmDb.lpmDbId = lpmDbId;
    lpmDbPtr = prvCpssSlSearch(prvCpssDxChIplpmDbSL,&tmpLpmDb);
    if (lpmDbPtr == NULL)
    {
        /* can't find the lpm DB */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    lpmHw = prvCpssDxChLpmGetHwType(lpmDbPtr->shadowType);
    if (lpmHw == PRV_CPSS_DXCH_LPM_HW_TCAM_E)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
    }

    shadowsPtr = (PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC*)(lpmDbPtr->shadow);
    for (k = 0; k < shadowsPtr->numOfShadowCfg; k++ )
    {
        shadowPtr =  &(shadowsPtr->shadowArray[k]);

        if ((shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E] == GT_FALSE) &&
            (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E] == GT_FALSE) &&
            (shadowPtr->isProtocolInitialized[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E] == GT_FALSE))
            continue;

        cpssOsPrintf("\nLPM RAM SHADOW ID [%d] GENERAL HW INFO\n\n",k);
        cpssOsPrintf("NumEntriesBetweenBlocks 0x%x\t BlocksAllocationMethod %s\n",shadowPtr->lpmRamTotalBlocksSizeIncludingGap,
                     (shadowPtr->lpmRamBlocksAllocationMethod == PRV_CPSS_DXCH_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E ? " NO_SHARING" : " WITH_SHARING"));
        for (i = 0; i < shadowPtr->numOfLpmMemories; i++)
        {
            cpssOsPrintf("MemBlock[%d]\t BlocksSize 0x%x\t memPoolId 0x%x\t isBlockUsed   %s\t  octetsToBlockMappingBitmap:\t IPv4 0x%x     IPv6 0x%x     FCOE 0x%x\n",
                         i,
                         shadowPtr->lpmRamBlocksSizeArrayPtr[i],
                         shadowPtr->lpmRamStructsMemPoolPtr[i],
                         (shadowPtr->lpmRamOctetsToBlockMappingPtr[i].isBlockUsed ? " YES" : " NO"),
                         shadowPtr->lpmRamOctetsToBlockMappingPtr[i].octetsToBlockMappingBitmap[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV4_E],
                         shadowPtr->lpmRamOctetsToBlockMappingPtr[i].octetsToBlockMappingBitmap[PRV_CPSS_DXCH_LPM_PROTOCOL_IPV6_E],
                         shadowPtr->lpmRamOctetsToBlockMappingPtr[i].octetsToBlockMappingBitmap[PRV_CPSS_DXCH_LPM_PROTOCOL_FCOE_E]);
        }
        cpssOsPrintf("------------------------------------------------------------------------------------------------\n");
    }

    return GT_OK;
}

/*******************************************************************************
* prvCpssDxChLpmDbgHwOctetPerBlockPrint
*
* DESCRIPTION:
*
*   Print Octet per Block debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId     - The LPM DB id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_VOID prvCpssDxChLpmDbgHwOctetPerBlockPrint
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *shadowPtr;
    GT_U32 octetsToBlockBitmap;
    GT_U32 i,j,octetId;
    static GT_CHAR *protocolNames[] = {"IPv4","IPv6","FCOE"};

    shadowPtr = &lpmDbPtr->shadowArray[0];

    for ( i = 0 ; i < shadowPtr->numOfLpmMemories ; i++ )
    {
        if (shadowPtr->lpmRamOctetsToBlockMappingPtr[i].isBlockUsed==GT_TRUE)
        {
            cpssOsPrintf("\nBlock [%d]\n",i);
        }
        for (j = 0; j < PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E; j++)
        {
            octetsToBlockBitmap = shadowPtr->lpmRamOctetsToBlockMappingPtr[i].octetsToBlockMappingBitmap[j];
            if (octetsToBlockBitmap)
            {
               cpssOsPrintf("\tprotocol   %s :", protocolNames[j]);
            }
            else
            {
                continue;
            }

            octetId = 0;
            while (octetsToBlockBitmap)
            {
                if (octetsToBlockBitmap & 0x1)
                    cpssOsPrintf("\t Octet [%d]     ",octetId);

                octetsToBlockBitmap >>= 1;
                octetId++;
            }
            cpssOsPrintf("\n");
        }
    }

    return;
}


/*******************************************************************************
* prvCpssDxChLpmDbgHwOctetPerProtocolPrint
*
* DESCRIPTION:
*
*   Print Octet per Protocol debug information for a specific LPM DB
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       lpmDbId     - The LPM DB id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*******************************************************************************/
GT_VOID prvCpssDxChLpmDbgHwOctetPerProtocolPrint
(
    IN PRV_CPSS_DXCH_LPM_RAM_SHADOWS_DB_STC      *lpmDbPtr
)
{
    PRV_CPSS_DXCH_LPM_RAM_SHADOW_STC  *shadowPtr;
    GT_U32 octetsToBlockBitmap;
    GT_U32 i,j,octetId;
    static GT_CHAR *protocolNames[] = {"IPv4","IPv6","FCOE"};

    shadowPtr = &lpmDbPtr->shadowArray[0];

    for (j = 0; j < PRV_CPSS_DXCH_LPM_PROTOCOL_LAST_E; j++)
    {
        cpssOsPrintf("\nprotocol   %s : ",protocolNames[j]);
        for ( i = 0 ; i < shadowPtr->numOfLpmMemories ; i++ )
        {
            octetsToBlockBitmap = shadowPtr->lpmRamOctetsToBlockMappingPtr[i].octetsToBlockMappingBitmap[j];
            if (octetsToBlockBitmap)
                cpssOsPrintf("\nBlock [%d]",i);
            else
                continue;
            octetId = 0;
            while (octetsToBlockBitmap)
            {
                if (octetsToBlockBitmap & 0x1)
                    cpssOsPrintf("\t Octet [%d]",octetId);

                octetsToBlockBitmap >>= 1;
                octetId++;
            }
        }
        cpssOsPrintf("\n");
    }

    return;
}

